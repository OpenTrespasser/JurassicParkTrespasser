/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Matrix.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/Matrix.cpp                                              $
 * 
 * 24    98/09/12 0:52 Speter
 * Added enumeration and subscript access for vectors; useful in loops.
 * 
 * 23    8/26/98 6:13p Asouth
 * #ifdef'd the use of the 'that' keyword
 * 
 * 22    98/08/25 19:04 Speter
 * Construct axis constants with "normalised=true" flag to avoid calling fInvSqrt; fixes init
 * dependency.
 * 
 * 21    98.08.13 4:27p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 20    98/08/13 14:47 Speter
 * Moved init_seg to top of file...better?
 * 
 * 19    98/07/03 19:48 Speter
 * Added identity constants.
 * 
 * 18    98/06/28 17:26 Speter
 * Added divide operators for matrix and transform types, speeding up certain operations.
 * 
 * 17    97/12/03 18:01 Speter
 * Added Assert for 'that' usage.  Added Vector.hpp constants; see note.
 * 
 * 16    97/11/24 16:49 Speter
 * Make use of new 'that' macro for faster returns.
 * 
 * 15    97/10/27 20:23 Speter
 *  Moved several transform functions inline.
 * 
 * 14    97/07/07 15:36 Speter
 * A few optimisations by re-organising code.
 * 
 * 13    97/06/23 20:22 Speter
 * pragma disabling warning moved to Warnings.hpp.
 * 
 * 12    97/06/14 0:29 Speter
 * Moved vector * matrix multiply to .hpp file.  Moved matrix * matrix (and similar transform
 * concatenations) to .cpp file.  Added hacky instantiation and pragma dreck.
 * 
 * 11    97/06/05 17:01 Speter
 * Changed pragma so it inlines CVector constructor.
 * 
 * 10    97/06/04 12:38 Speter
 * Added pragma to force code generation and avoid link errors in Release build.
 * 
 * 9     97/06/03 18:43 Speter
 * Bye-bye #include "*.cpp".  Made .cpp files actual independent files by adding explicit
 * template instantiation.
 * 
 * 8     97-04-22 10:51 Speter
 * Sped up Vector * Transform function by doing vector addition along with matrix
 * transformation, rather than afterwards.
 * 
 * 7     97/02/07 19:17 Speter
 * Added CMatrix3 constructor taking 2 CDir3's defining a frame. 
 * 
 * 6     96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 5     96/08/21 17:56 Speter
 * Changes from code review:
 * Added default template parameter to all classes.
 * Made all in-class friends inline.
 * Inverse operator now multiplies all components by reciprocal of determinant, rather than
 * dividing.
 * 
 * 4     96/06/26 22:07 Speter
 * Added a bunch of comments, prettied things up nicely.
 * 
 * 3     96/06/26 13:16 Speter
 * Changed TGReal to TReal and prefix gr to r.
 * 
 * 2     96/06/20 17:13 Speter
 * Converted into templates and made necessary changes.
 * 
 * 1     96/06/20 15:26 Speter
 * First version of new optimised transform library.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Matrix.hpp"

// Give this module run-time init priority.
#pragma warning(disable: 4073)
#pragma init_seg(lib)

//**********************************************************************************************
//
// CMatrix3<TR> implementation.
//

	//******************************************************************************************
	template<class TR> CMatrix3<TR>::CMatrix3
	(
		const CDir3<TR>& d3_x, const CDir3<TR>& d3_y,
		bool b_perpendicular
	)
	{
		// The third axis is the cross-product of the first two.
		v3Z = d3_x ^ d3_y;

		if (b_perpendicular)
		{
			// This should be unit if the first two are truly perpendicular.
			Assert(v3Z.bIsNormalised());
			v3X = d3_x;
			v3Y = d3_y;
		}
		else
		{
			// Since they are not perpendicular, first construct the third axis as the
			// cross product of the two.  Then set the second axis to be perpendicular to both.
			v3Z.Normalise();
			v3X = d3_x;
			v3Y = v3Z ^ v3X;
		}
	}

	//******************************************************************************************
	template<class TR> CMatrix3<TR> CMatrix3<TR>::operator ~() const
	// Return the inverse matrix.
	{
		// Check for aliasing.
	#ifndef __MWERKS__
		Assert(that != this);
	#endif

		TR t_det = tDet();

		Assert(t_det != TR(0));

		TR t_invdet = (TR)1 / t_det;

		// Make use of the 'that' macro to construct the return value directly.
	#ifdef __MWERKS__
		return CMatrix3<TR>(
			(v3Y.tY * v3Z.tZ - v3Y.tZ * v3Z.tY) * t_invdet,
			(v3X.tZ * v3Z.tY - v3Z.tZ * v3X.tY) * t_invdet,
			(v3X.tY * v3Y.tZ - v3X.tZ * v3Y.tY) * t_invdet,

			(v3Y.tZ * v3Z.tX - v3Z.tZ * v3Y.tX) * t_invdet,
			(v3X.tX * v3Z.tZ - v3X.tZ * v3Z.tX) * t_invdet,
			(v3X.tZ * v3Y.tX - v3Y.tZ * v3X.tX) * t_invdet,

			(v3Y.tX * v3Z.tY - v3Y.tY * v3Z.tX) * t_invdet,
			(v3X.tY * v3Z.tX - v3X.tX * v3Z.tY) * t_invdet,
			(v3X.tX * v3Y.tY - v3X.tY * v3Y.tX) * t_invdet );
	#else
		that->v3X.tX = (v3Y.tY * v3Z.tZ - v3Y.tZ * v3Z.tY) * t_invdet;
		that->v3X.tY = (v3X.tZ * v3Z.tY - v3Z.tZ * v3X.tY) * t_invdet;
		that->v3X.tZ = (v3X.tY * v3Y.tZ - v3X.tZ * v3Y.tY) * t_invdet;

		that->v3Y.tX = (v3Y.tZ * v3Z.tX - v3Z.tZ * v3Y.tX) * t_invdet;
		that->v3Y.tY = (v3X.tX * v3Z.tZ - v3X.tZ * v3Z.tX) * t_invdet;
		that->v3Y.tZ = (v3X.tZ * v3Y.tX - v3Y.tZ * v3X.tX) * t_invdet;

		that->v3Z.tX = (v3Y.tX * v3Z.tY - v3Y.tY * v3Z.tX) * t_invdet;
		that->v3Z.tY = (v3X.tY * v3Z.tX - v3X.tX * v3Z.tY) * t_invdet;
		that->v3Z.tZ = (v3X.tX * v3Y.tY - v3X.tY * v3Y.tX) * t_invdet;

		return *that;
	#endif
	}

	//******************************************************************************************
	template<class TR> TR CMatrix3<TR>::tDet() const
	// Return the determinant.
	{
		return (v3X.tX * v3Y.tY - v3X.tY * v3Y.tX) * v3Z.tZ
			 + (v3X.tZ * v3Y.tX - v3X.tX * v3Y.tZ) * v3Z.tY
			 + (v3X.tY * v3Y.tZ - v3X.tZ * v3Y.tY) * v3Z.tX;
	}

	//******************************************************************************************
	template<class TR> CVector3<TR> CMatrix3<TR>::v3InvMat(const CVector3<TR>& v3) const
	{
		// Check for aliasing.
	#ifndef __MWERKS__
		Assert(that != &v3);
	#endif

		// Construct inverse of mx3, without applying inverse determinant.
		TR t_det = tDet();

		Assert(t_det != TR(0));

		TR t_invdet = (TR)1 / t_det;

		CMatrix3<TR> mx3_inv
		(
			v3Y.tY * v3Z.tZ - v3Y.tZ * v3Z.tY,
			v3X.tZ * v3Z.tY - v3Z.tZ * v3X.tY,
			v3X.tY * v3Y.tZ - v3X.tZ * v3Y.tY,

			v3Y.tZ * v3Z.tX - v3Z.tZ * v3Y.tX,
			v3X.tX * v3Z.tZ - v3X.tZ * v3Z.tX,
			v3X.tZ * v3Y.tX - v3Y.tZ * v3X.tX,

			v3Y.tX * v3Z.tY - v3Y.tY * v3Z.tX,
			v3X.tY * v3Z.tX - v3X.tX * v3Z.tY,
			v3X.tX * v3Y.tY - v3X.tY * v3Y.tX
		);

		// Transform vector by this inverse matrix, while applying inverse determinant.
	#ifdef __MWERKS__
		return CVector3<TR>(
			VdotMC(v3, mx3_inv, tX) * t_invdet,
			VdotMC(v3, mx3_inv, tY) * t_invdet,
			VdotMC(v3, mx3_inv, tZ) * t_invdet );
	#else
		that->tX = VdotMC(v3, mx3_inv, tX) * t_invdet;
		that->tY = VdotMC(v3, mx3_inv, tY) * t_invdet;
		that->tZ = VdotMC(v3, mx3_inv, tZ) * t_invdet;
		return *that;
	#endif
	}

//**********************************************************************************************
//
// CTransform3<TR> implementation.
//

	//
	// Concatenate with another transform.
	//

	//******************************************************************************************
	template<class TR> CTransform3<TR>& CTransform3<TR>::operator *=(const CTransform3<TR>& tf3)
	{
		// Concatenate base transform and translation separately.
		mx3Mat *= tf3.mx3Mat;
		v3Pos  *= tf3;
		return *this;
	}

	//******************************************************************************************
	template<class TR> CTransform3<TR> CTransform3<TR>::operator /(const CTransform3<TR>& tf3)
	{
		CMatrix3<TR> mx3_2inv = ~tf3.mx3Mat;
	#ifdef __MWERKS__
		return CTransform3<TR>(
			mx3Mat * mx3_2inv,
			(v3Pos - tf3.v3Pos) * mx3_2inv );
	#else
		that->mx3Mat = mx3Mat * mx3_2inv,
		that->v3Pos = (v3Pos - tf3.v3Pos) * mx3_2inv;
		return *that;
	#endif
	}

	//******************************************************************************************
	template<class TR> CTransform3<TR>& CTransform3<TR>::operator /=(const CTransform3<TR>& tf3)
	{
		CMatrix3<TR> mx3_2inv = ~tf3.mx3Mat;
		mx3Mat *= mx3_2inv;
		v3Pos -= tf3.v3Pos;
		v3Pos *= mx3_2inv;
		return *this;
	}

	//
	// Concatenate with a CMatrix3.
	//

	//******************************************************************************************
	template<class TR> CTransform3<TR>& CTransform3<TR>::operator *=(const CMatrix3<TR>& mx3)
	{
		// Concatenate matrix and translation separately.
		mx3Mat *= mx3;
		v3Pos  *= mx3;
		return *this;
	}

//
// Explicit instantiation of used template types.
//

template class CMatrix3<>;
template class CTransform3<>;

const CMatrix3<> mx3Null;
const CTransform3<> tf3Null;

//
// Hack.  Place Vector.hpp constant definitions here, because I'm too lazy to create Vector.cpp.
//

const CVector2<> v2Zero(0, 0);
const CVector3<> v3Zero(0, 0, 0);
const CDir3<> d3XAxis(1, 0, 0, true), d3YAxis(0, 1, 0, true), d3ZAxis(0, 0, 1, true);
const CDir3<> ad3Axes[3] = { CDir3<>(1, 0, 0, true), CDir3<>(0, 1, 0, true), CDir3<>(0, 0, 1, true) };
