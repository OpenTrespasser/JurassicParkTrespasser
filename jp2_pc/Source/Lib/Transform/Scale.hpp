/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CScale3<TR> and CScaleI3<TR>.
 *
 * Bugs:
 *
 * To do:
 *		If needed, provide scaling in an arbitrary direction.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/Scale.hpp                                              $
 * 
 * 13    5/26/97 1:32p Agrant
 * LINT- fixed forward declaration of template type.
 * 
 * 12    12/20/96 4:46p Mlange
 * Updated friend class declaration in CScale3<>.
 * 
 * 11    96/12/04 20:18 Speter
 * Changed v3T to v3Pos in all transforms.
 * 
 * 10    96/10/25 14:43 Speter
 * Removed now-unneeded tScaleFactor function.
 * 
 * 9     96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 8     96/08/21 18:50 Speter
 * Changes from code review:
 * Added default template parameter to all classes.
 * Made all in-class friends inline.
 * Added Asserts for inverse operators.
 * Added operations with CTranslate3 transform.
 * Added comments.
 * 
 * 7     96/07/31 15:44 Speter
 * Changed operator TR() to TR tScaleFactor(), to avoid ambiguities.
 * 
 * 6     7/26/96 4:04p Mlange
 * Added a conversion operator to a TR type variable  to the CScaleI3 class.
 * 
 * 5     96/06/26 22:07 Speter
 * Added a bunch of comments, prettied things up nicely.
 * 
 * 4     96/06/26 13:16 Speter
 * Changed TGReal to TR and prefix gr to r.
 * 
 * 3     96/06/25 14:35 Speter
 * Finalised design of transform classes, with Position3 and Transform3.
 * 
 * 2     96/06/20 17:13 Speter
 * Converted into templates and made necessary changes.
 * 
 * 1     96/06/20 15:26 Speter
 * First version of new optimised transform library.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_SCALE_HPP
#define HEADER_LIB_TRANSFORM_SCALE_HPP

#include "Matrix.hpp"


template <class T> class CScaleI3;

//**********************************************************************************************
//
template<class TR = TReal> class CScale3
//
// Prefix: s3
//
// An arbitrary nonisotropic scaling transform.
//
// Note: This is not inherited from CVector3, because a scale behaves like a transform,
// not like a vector.  For example, CScale3 * CVector3 scales the vector, while 
// CVector3 * CVector3 is the dot product.
//
//**************************
{
protected:
	CVector3<TR>	v3;				// Contains scaling factors in each dimension.

public:
	//******************************************************************************************
	//
	// Constructors.
	//

	CScale3()
		: v3(TR(1), TR(1), TR(1))
	{
	}

	CScale3(TR t_x, TR t_y, TR t_z)
		: v3(t_x, t_y, t_z)
	{
	}

	CScale3(const CVector3<TR>& v3)
		: v3(v3)
	{
	}

	//******************************************************************************************
	//
	// Conversion operators.
	//

	operator CMatrix3<TR> () const
	{
		CMatrix3<TR> mx3;

		mx3.v3X.tX = v3.tX;
		mx3.v3Y.tY = v3.tY;
		mx3.v3Z.tZ = v3.tZ;

		return mx3;
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Return the inverse of the scale.
	CScale3<TR> operator ~() const
	{
		Assert(v3.tX != (TR)0 && v3.tY != (TR)0 && v3.tZ != (TR)0);
		return CScale3<TR>( (TR)1 / v3.tX, (TR)1 / v3.tY, (TR)1 / v3.tZ );
	}

	//
	// Concatenate with another scale transform.
	//

	friend inline CScale3<TR> operator *(const CScale3<TR>& s3_a, const CScale3<TR>& s3_b)
	{
		return CScale3<TR>
		( 
			s3_a.v3.tX * s3_b.v3.tX,
			s3_a.v3.tY * s3_b.v3.tY,
			s3_a.v3.tZ * s3_b.v3.tZ
		);
	}


	CScale3<TR>& operator *=(const CScale3<TR>& s3)
	{
		v3.tX *= s3.v3.tX;
		v3.tY *= s3.v3.tY;
		v3.tZ *= s3.v3.tZ;
		return *this;
	}

	//
	// Concatenate with a matrix.
	//

	friend inline CMatrix3<TR> operator *(const CScale3<TR>& s3, const CMatrix3<TR>& mx3)
	{
		// Scale the columns of the matrix by the scale vector.
		return CMatrix3<TR>
		(
			mx3.v3X.tX * s3.v3.tX,  mx3.v3X.tY * s3.v3.tX,  mx3.v3X.tZ * s3.v3.tX, 
			mx3.v3Y.tX * s3.v3.tY,  mx3.v3Y.tY * s3.v3.tY,  mx3.v3Y.tZ * s3.v3.tY, 
			mx3.v3Z.tX * s3.v3.tZ,  mx3.v3Z.tY * s3.v3.tZ,  mx3.v3Z.tZ * s3.v3.tZ
		);
	}

	friend inline CMatrix3<TR> operator *(const CMatrix3<TR>& mx3, const CScale3<TR>& s3)
	{
		// Scale the rows of the matrix by the scale vector (mx3Transform does this).
		return mx3Transform(mx3, s3);
	}

	friend inline CMatrix3<TR>& operator *=(CMatrix3<TR>& mx3, const CScale3<TR>& s3)
	{
		return Transform(mx3, s3);
	}

	//
	// Concatenate with a transform.
	//

	friend inline CTransform3<TR> operator *(const CScale3<TR>& s3, const CTransform3<TR>& tf3)
	{
		return CTransform3<TR>(s3 * tf3.mx3Mat, tf3.v3Pos);
	}

	friend inline CTransform3<TR> operator *(const CTransform3<TR>& tf3, const CScale3<TR>& s3)
	{
		return CTransform3<TR>(tf3.mx3Mat * s3, tf3.v3Pos * s3);
	}

	friend inline CTransform3<TR>& operator *=(CTransform3<TR>& tf3, const CScale3<TR>& s3)
	{
		tf3.mx3Mat *= s3;
		tf3.v3Pos *= s3;
		return tf3;
	}

	//
	// Concatenate with a translation.
	//

	friend inline CTransform3<TR> operator *(const CScale3<TR>& s3, const CTranslate3<TR>& tl3)
	{
		return CTransform3<TR>(s3, tl3.v3Pos);
	}

	friend inline CTransform3<TR> operator *(const CTranslate3<TR>& tl3, const CScale3<TR>& s3)
	{
		return CTransform3<TR>(s3, tl3.v3Pos * s3);
	}

	//
	// Operate on a vector.
	//

	friend inline CVector3<TR> operator *(const CVector3<TR>& v3, const CScale3<TR>& s3)
	{
		return CVector3<TR>
		( 
			v3.tX * s3.v3.tX, 
			v3.tY * s3.v3.tY, 
			v3.tZ * s3.v3.tZ 
		);
	}


	friend inline CVector3<TR>& operator *=(CVector3<TR>& v3, const CScale3<TR>& s3)
	{
		v3.tX *= s3.v3.tX;
		v3.tY *= s3.v3.tY;
		v3.tZ *= s3.v3.tZ;
		return v3;
	}

	friend CScaleI3<TR>;
	friend CScale3<TR> operator *(const CScale3<TR>& s3, const CScaleI3<TR>& si3);
	friend CScale3<TR>& operator *=(CScale3<TR>& s3, const CScaleI3<TR>& si3);
};

//**********************************************************************************************
//
template<class TR = TReal> class CScaleI3
//
// Prefix: si3
//
// An isotropic scaling operation.
//
//**************************
{
protected:
	TR tScale;							// The scaling factor for each dimension.

public:
	//******************************************************************************************
	//
	// Constructors.
	//

	CScaleI3()
		: tScale((TR) 1)
	{
	}

	CScaleI3(TR t)
		: tScale(t)
	{
	}

	//******************************************************************************************
	//
	// Conversion operators.
	//

	operator CMatrix3<TR> () const
	{
		CMatrix3<TR> mx3;

		mx3.v3X.tX = mx3.v3Y.tY = mx3.v3Z.tZ = tScale;

		return mx3;
	}

	// Convert to an anisotropic scale.
	operator CScale3<TR> () const
	{
		return CScale3<TR>(tScale, tScale, tScale);
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Return the inverse of the scale.
	CScaleI3<TR> operator ~() const
	{
		Assert(tScale != (TR)0);
		return CScaleI3<TR>( (TR)1 / tScale );
	}

	//
	// Concatenate with another ScaleI transform.
	//

	friend inline CScaleI3<TR> operator *(const CScaleI3<TR>& si3_a, const CScaleI3<TR>& si3_b)
	{
		return CScaleI3<TR>(si3_a.tScale * si3_b.tScale);
	}

	CScaleI3<TR>& operator *=(const CScaleI3<TR>& si3)
	{
		tScale *= si3.tScale;
		return *this;
	}

	//
	// Concatenate with a Scale transform (commutative).
	//

	friend inline CScale3<TR> operator *(const CScaleI3<TR>& si3, const CScale3<TR>& s3)
	{
		return s3 * si3;
	}

	friend inline CScale3<TR>& operator *=(CScale3<TR>& s3, const CScaleI3<TR>& si3)
	{
		s3.v3 *= si3.tScale;
		return s3;
	}

	friend inline CScale3<TR> operator *(const CScale3<TR>& s3, const CScaleI3<TR>& si3)
	{
		return CScale3<TR>(s3.v3 * si3.tScale);
	}

	//
	// Concatenate with a Matrix (commutative).
	//

	friend inline CMatrix3<TR> operator *(const CScaleI3<TR>& si3, const CMatrix3<TR>& mx3)
	{
		return mx3Transform(mx3, si3);
	}

	friend inline CMatrix3<TR>& operator *=(CMatrix3<TR>& mx3, const CScaleI3<TR>& si3)
	{
		return Transform(mx3, si3);
	}

	friend inline CMatrix3<TR> operator *(const CMatrix3<TR>& mx3, const CScaleI3<TR>& si3)
	{
		return mx3Transform(mx3, si3);
	}

	//
	// Concatenate with a transform.  Not commutative.
	//

	friend inline CTransform3<TR> operator *(const CScaleI3<TR>& si3, const CTransform3<TR>& tf3)
	{
		return CTransform3<TR>(si3 * tf3.mx3Mat, tf3.v3Pos);
	}

	friend inline CTransform3<TR> operator *(const CTransform3<TR>& tf3, const CScaleI3<TR>& si3)
	{
		return CTransform3<TR>(tf3.mx3Mat * si3, tf3.v3Pos * si3);
	}

	friend inline CTransform3<TR>& operator *=(CTransform3<TR>& tf3, const CScaleI3<TR>& si3)
	{
		tf3.mx3Mat *= si3;
		tf3.v3Pos *= si3;
		return tf3;
	}

	//
	// Concatenate with a translation.
	//

	friend inline CTransform3<TR> operator *(const CScaleI3<TR>& si3, const CTranslate3<TR>& tl3)
	{
		return CTransform3<TR>(si3, tl3.v3Pos);
	}

	friend inline CTransform3<TR> operator *(const CTranslate3<TR>& tl3, const CScaleI3<TR>& si3)
	{
		return CTransform3<TR>(si3, tl3.v3Pos * si3);
	}

	//
	// Operate on a vector.
	//

	friend inline CVector3<TR> operator *(const CVector3<TR>& v3, const CScaleI3<TR>& si3)
	{
		return v3 * si3.tScale;
	}

	friend inline CVector3<TR>& operator *=(CVector3<TR>& v3, const CScaleI3<TR>& si3)
	{
		return v3 *= si3.tScale;
	}
};

#endif
