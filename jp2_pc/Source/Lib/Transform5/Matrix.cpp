/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Matrix.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform5/Matrix.cpp                                             $
 * 
 * 8     97-04-24 18:40 Speter
 * Folded new changes from 4.2 version into this 5.0 specific version.
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

#include <memory.h>

#include "Common.hpp"
#include "Matrix.hpp"

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
		TR t_det = tDet();

		Assert(t_det != TR(0));

		TR t_invdet = (TR)1 / t_det;

		return CMatrix3<TR>
		(
			(v3Y.tY * v3Z.tZ - v3Y.tZ * v3Z.tY) * t_invdet,
			(v3X.tZ * v3Z.tY - v3Z.tZ * v3X.tY) * t_invdet,
			(v3X.tY * v3Y.tZ - v3X.tZ * v3Y.tY) * t_invdet,

			(v3Y.tZ * v3Z.tX - v3Z.tZ * v3Y.tX) * t_invdet,
			(v3X.tX * v3Z.tZ - v3X.tZ * v3Z.tX) * t_invdet,
			(v3X.tZ * v3Y.tX - v3Y.tZ * v3X.tX) * t_invdet,

			(v3Y.tX * v3Z.tY - v3Y.tY * v3Z.tX) * t_invdet,
			(v3X.tY * v3Z.tX - v3X.tX * v3Z.tY) * t_invdet,
			(v3X.tX * v3Y.tY - v3X.tY * v3Y.tX) * t_invdet
		);
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
	//
	//	TR VdotMC
	//	(
	//		CVector3<TR> v,
	//		CMatrix3<TR> m,
	//		c							// The matrix column: either tX, tY, or tZ.
	//	)
	//
	//	Multiply a vector by a matrix column.  This is a macro because the
	//	column is not a variable, but a member field to select out of matrix rows.
	//
	//	Returns:
	//		The dot product of the vector and the specified matrix column.
	//		This is the 'c' component of the vector*matrix vector.
	//
	//	Cross references:
	//		Used below by vector*matrix.
	//
	//**********************************
	#define VdotMC(v, m, c)	(v.tX * m.v3X.c  +  v.tY * m.v3Y.c  +  v.tZ * m.v3Z.c)

	//******************************************************************************************
	template<class TR> CVector3<TR> operator *(const CVector3<TR>& v3, const CMatrix3<TR>& mx3) 
	{
		return CVector3<TR>
		(
			VdotMC(v3, mx3, tX), 
			VdotMC(v3, mx3, tY), 
			VdotMC(v3, mx3, tZ)
		);
	}

	//******************************************************************************************
	template<class TR> CVector3<TR> operator *(const CVector3<TR>& v3, const CTransform3<TR>& tf3) 
	{
		return CVector3<TR>
		(
			VdotMC(v3, tf3.mx3Mat, tX) + tf3.v3Pos.tX, 
			VdotMC(v3, tf3.mx3Mat, tY) + tf3.v3Pos.tY, 
			VdotMC(v3, tf3.mx3Mat, tZ) + tf3.v3Pos.tZ
		);
	}

