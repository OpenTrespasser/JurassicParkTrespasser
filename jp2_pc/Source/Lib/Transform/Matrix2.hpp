/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		class CMatrix2<TR>
 *		class CTransform2<TR>
 *
 * Notes:
 *		Unlike CVector2/CDir2, these classes are not included in the corresponding "Matrix.hpp"
 *		file, because there are very few users.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/Matrix2.hpp                                             $
 * 
 * 4     9/02/98 5:03p Asouth
 * A few more 'that's removed.  Note that some remain; these functions
 * aren't used in the build.
 * 
 * 3     8/25/98 2:38p Rvande
 * Added #ifdef around use of 'that' keyword
 * 
 * 2     5/06/98 10:33a Mlange
 * Added missing include guards.
 * 
 * 1     98/03/12 11:07 Speter
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_MATRIX2_HPP
#define HEADER_LIB_TRANSFORM_MATRIX2_HPP

#include "Vector.hpp"
#include "Matrix.hpp"
#include "TransLinear.hpp"

//**********************************************************************************************
//
template<class TR = TReal> class CMatrix2
//
// Prefix: mx2
//
// Encodes a 2D linear transformation, without translation.
//
// This is less fully implemented than CMatrix3.
//
//**************************************
{
public:
	CVector2<TR>	v2X, v2Y;			// The source-vectors of the matrix.

public:
	
	//******************************************************************************************
	//
	// Constructors.
	//

	CMatrix2()
		: v2X(0, 0), v2Y(0, 0)
	{
	}

	CMatrix2(const CVector2<TR>& v2_x, const CVector2<TR>& v2_y)
		: v2X(v2_x), v2Y(v2_y)
	{
	}

	CMatrix2(TR t_xx, TR t_xy, TR t_yx, TR t_yy)
		: v2X(t_xx, t_xy), v2Y(t_yx, t_yy)
	{
	}

	CMatrix2(const CMatrix3<TR>& mx3)
		: v2X(mx3.v3X), v2Y(mx3.v3Y)
	{
	}

	//******************************************************************************************
	//
	// Conversion operators.
	//

	//******************************************************************************************
	//
	// Operators.
	//

	// Return the inverse of the matrix.
	CMatrix2<TR> operator ~() const
	{
	#ifdef __MWERKS__
		TR t_det = v2X.tX * v2Y.tY - v2X.tY * v2Y.tX;
		Assert(t_det != TR(0));
		TR t_invdet = (TR)1 / t_det;

		return CMatrix2<TR>(
			 v2Y.tY * t_invdet,
			-v2X.tY * t_invdet,
			-v2Y.tX * t_invdet,
			 v2X.tX * t_invdet );
	#else
		Assert(that != this);

		TR t_det = v2X.tX * v2Y.tY - v2X.tY * v2Y.tX;
		Assert(t_det != TR(0));
		TR t_invdet = (TR)1 / t_det;

		// Make use of the 'that' macro to construct the return value directly.
		that->v2X.tX =   v2Y.tY * t_invdet;
		that->v2X.tY = - v2X.tY * t_invdet;

		that->v2Y.tX = - v2Y.tX * t_invdet;
		that->v2Y.tY =   v2X.tX * t_invdet;

		return *that;
	#endif
	}

	//
	// Concatenate with another CMatrix2.
	//

	CMatrix2<TR> operator *(const CMatrix2<TR>& mx2)
	{
		return CMatrix2<TR>
		(
			v2X.tX * mx2.v2X.tX  +  v2X.tY * mx2.v2Y.tX,
			v2X.tX * mx2.v2X.tY  +  v2X.tY * mx2.v2Y.tY,
			v2Y.tX * mx2.v2X.tX  +  v2Y.tY * mx2.v2Y.tX,
			v2Y.tX * mx2.v2X.tY  +  v2Y.tY * mx2.v2Y.tY
		);
	}

	CMatrix2<TR>& operator *=(const CMatrix2<TR>& mx2)
	{
		return *this = *this * mx2;
	}

	//
	// Operate on a vector.
	//

	friend inline CVector2<TR> operator *(const CVector2<TR>& v2, const CMatrix2<TR>& mx2)
	{
	#ifdef __MWERKS__
		return CVector2<TR>(
			v2.tX * mx2.v2X.tX  +  v2.tY * mx2.v2Y.tX,
			v2.tX * mx2.v2X.tY  +  v2.tY * mx2.v2Y.tY );
	#else
		// Check for aliasing.
		Assert(that != &v2);

		that->tX = v2.tX * mx2.v2X.tX  +  v2.tY * mx2.v2Y.tX;
		that->tY = v2.tX * mx2.v2X.tY  +  v2.tY * mx2.v2Y.tY;
		return *that;
	#endif
	}

	friend inline CVector2<TR>& operator *=(CVector2<TR>& v2, const CMatrix2<TR>& mx2)
	{
		return v2 = v2 * mx2;
	}
};

//**********************************************************************************************
//
template<class TR = TReal> class CTransform2
//
// Prefix: tf2
//
// Encodes a 2D linear transformation, including translation.
//
// This is less fully implemented than CTransform3.
//
//**************************************
{
public:
	CMatrix2<TR>	mx2Mat;
	CVector2<TR>	v2Pos;

public:
	
	//******************************************************************************************
	//
	// Constructors.
	//

	CTransform2()
		: v2Pos(0, 0)
	{
	}

	CTransform2(const CMatrix2<TR>& mx2, const CVector2<TR>& v2_pos)
		: mx2Mat(mx2), v2Pos(v2_pos)
	{
	}

	CTransform2(const CVector2<TR>& v2_x, const CVector2<TR>& v2_y, const CVector2<TR>& v2_pos)
		: mx2Mat(v2_x, v2_y), v2Pos(v2_pos)
	{
	}

	CTransform2(const CTransform3<TR>& tf3)
		: mx2Mat(tf3.mx3Mat), v2Pos(tf3.v3Pos)
	{
	}

	CTransform2(const CTransLinear2<TR>& tlr2)
		: mx2Mat(tlr2.tlrX.tScale, 0, 0, tlr2.tlrY.tScale), v2Pos(tlr2.tlrX.tOffset, tlr2.tlrY.tOffset)
	{
	}

	//******************************************************************************************
	//
	// Conversion operators.
	//

	//******************************************************************************************
	//
	// Operators.
	//

	// Return the inverse of the transform.
	CTransform2<TR> operator ~() const
	{
		// The inverse of a composite operation S*TR is ~TR * ~S, which is CTransform2(~S, -TR * ~S).
	#ifdef __MWERKS__
		CTransform2<TR> rv;		// Return Value
		rv.mx2Mat = ~mx2Mat;
		rv.v2Pos = -v2Pos * rv.mx2Mat;
		return rv;
	#else
		that->mx2Mat = ~mx2Mat;
		that->v2Pos  = -v2Pos * that->mx2Mat;
		return *that;
	#endif
	}

	//
	// Concatenate with another CTransform2.
	//

	CTransform2<TR> operator *(const CTransform2<TR>& tf2)
	{
		Assert(that != this && that != &tf2);

		that->mx2Mat.v2X = mx2Mat.v2X   * tf2.mx2Mat;
		that->mx2Mat.v2Y = mx2Mat.v2Y   * tf2.mx2Mat;
		that->v2Pos      = v2Pos        * tf2;
		return *that;
	}

	CTransform2<TR>& operator *=(const CTransform2<TR>& tf2)
	{
		mx2Mat *= tf2.mx2Mat;
		v2Pos  *= tf2;
		return *this;
	}

	//
	// Operate on a vector.
	//

	friend CVector2<TR> operator *(const CVector2<TR>& v2, const CTransform2<TR>& tf2)
	{
	#ifdef __MWERKS__
		return CVector2<TR>(
			v2.tX * tf2.mx2Mat.v2X.tX  +  v2.tY * tf2.mx2Mat.v2Y.tX  +  tf2.v2Pos.tX,
			v2.tX * tf2.mx2Mat.v2X.tY  +  v2.tY * tf2.mx2Mat.v2Y.tY  +  tf2.v2Pos.tY );
	#else
		// Check for aliasing.
		Assert(that != &v2);

		that->tX = v2.tX * tf2.mx2Mat.v2X.tX  +  v2.tY * tf2.mx2Mat.v2Y.tX  +  tf2.v2Pos.tX;
		that->tY = v2.tX * tf2.mx2Mat.v2X.tY  +  v2.tY * tf2.mx2Mat.v2Y.tY  +  tf2.v2Pos.tY;
		return *that;
	#endif
	}

	friend inline CVector2<TR>& operator *=(CVector2<TR>& v2, const CTransform2<TR>& tf2)
	{
		return v2 = v2 * tf2;
	}

	//
	// Operate on a direction by skipping the translation step.
	//

	friend inline CDir2<TR> operator *(const CDir2<TR>& d2, const CTransform2<TR>& tf2)
	{
		return d2 * tf2.mx2Mat;
	}

	friend inline CDir2<TR>& operator *=(CDir2<TR>& d2, const CTransform2<TR>& tf2)
	{
		return d2 = d2 * tf2.mx2Mat;
	}
};

#if _MSC_VER < 1100
template<class X> class CMicrosoftsCompilerIsShite
{
};
#endif

#endif