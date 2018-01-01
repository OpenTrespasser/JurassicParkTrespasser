/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		class CMatrix3<TR>
 *		class CTransform3<TR>
 *
 * Bugs:
 *
 * To do:
 *		If needed, add flags indicating that matrix is special orthogonal, and add
 *		renormalising functions.  Currently, this is unnecessary as rotations are handled
 *		by a separate class.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/Matrix.hpp                                              $
 * 
 * 25    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 24    8/25/98 11:47a Rvande
 * MW does not support 'that' keyword
 * 
 * 23    98/07/03 19:48 Speter
 * Added identity constants.
 * 
 * 22    98/06/28 17:26 Speter
 * Added divide operators for matrix and transform types, speeding up certain operations.
 * 
 * 21    98/06/01 18:43 Speter
 * Dir * Matrix operation now assumes matrix is rotational, doesn't renormalise.
 * 
 * 20    97/12/03 18:02 Speter
 * Made more operators use 'that'.
 * 
 * 19    97/10/27 20:23 Speter
 * Added VdotM macro, moved mx3 * mx3 inline and sped up.  Moved several transform functions
 * inline.
 * 
 * 18    97/07/07 15:36 Speter
 * A few optimisations by re-organising code.
 * 
 * 17    97/06/14 0:29 Speter
 * Moved vector * matrix multiply to .hpp file.  Moved matrix * matrix (and similar transform
 * concatenations) to .cpp file.  Added hacky instantiation and pragma dreck.
 * 
 * 16    97/06/03 18:43 Speter
 * Bye-bye #include "*.cpp".  Made .cpp files actual independent files by adding explicit
 * template instantiation.
 * 
 * 15    97-04-22 10:50 Speter
 * Sped up Vector * Transform function by doing vector addition along with matrix
 * transformation, rather than afterwards.
 * 
 * 14    97/03/24 14:54 Speter
 * Removed constructors of CDirs and CRotates from ASCII chars; use d3ZAxis etc. constants.
 * When we need optimisation for axes, we'll use derived classes which don't need a switch
 * statement.
 * 
 * 13    97/02/07 19:17 Speter
 * Added CMatrix3 constructor taking 2 CDir3's defining a frame.  Added mx3Transpose().  Changed
 * CVector3 value parameters to const&.
 * 
 * 12    96/12/04 20:18 Speter
 * Changed v3T to v3Pos in all transforms.  Changed mx3T to mx3Mat.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_MATRIX_HPP
#define HEADER_LIB_TRANSFORM_MATRIX_HPP

#include "Vector.hpp"
#include "Translate.hpp"

//**********************************************************************************************
//
template<class TR = TReal> class CMatrix3
//
// Prefix: mx3
//
// A 3x3 matrix.  Can perform any non-translating linear transform.
//
// Conventions:
//		Matrices are POST-multiplied with vectors:
//			v * A * B
//		means transform v by A, then by B.
//		You can think of this as a "row-vector" convention.
//
//		Storage follows the row.column and input.output conventions:
//		For example, element v3X.tY represents row X, column Y; 
//		and contains the Y output value for an X input value.
//
//		We also adhere to a right-handed coordinate system (relevant for rotations).
//
//**************************************
{
public:
	CVector3<TR>		v3X, v3Y, v3Z;

public:
	//******************************************************************************************
	//
	// Constructors.
	//

	// Default constructor creates the unit matrix.
	CMatrix3()
	{
		v3X.tY = v3X.tZ = TR(0);
		v3Y.tX = v3Y.tZ = TR(0);
		v3Z.tX = v3Z.tY = TR(0);
		v3X.tX = v3Y.tY = v3Z.tZ = TR(1);
	}

	// Initialise directly with 3 vectors.
	CMatrix3(const CVector3<TR>& v3_x, const CVector3<TR>& v3_y, const CVector3<TR>& v3_z) 
	{
		v3X = v3_x;
		v3Y = v3_y;
		v3Z = v3_z;
	}

	// Initialise directly with 9 elements.
	CMatrix3(TR t_xx, TR t_xy, TR t_xz, TR t_yx, TR t_yy, TR t_yz, TR t_zx, TR t_zy, TR t_zz)
	{
		v3X.tX = t_xx;		v3X.tY = t_xy;		v3X.tZ = t_xz;
		v3Y.tX = t_yx;		v3Y.tY = t_yy;		v3Y.tZ = t_yz;
		v3Z.tX = t_zx;		v3Z.tY = t_zy;		v3Z.tZ = t_zz;
	}

	// Construct a matrix transforming default axes to a new orthonormal frame.
	// This matrix can be used to represent that frame.
	CMatrix3
	(
		const CDir3<TR>& d3_x, const CDir3<TR>& d3_y,	// Two axes defining a frame.
		bool b_perpendicular							// Axes are asserted to be perpendicular.
	);

	// Initialise with shear parameters.
	// Note: this is a bit of a kludge, because CMatrix3 is being used as the CShear3 class.  
	// See Shear.hpp for more info.
	CMatrix3
	(
		char c_axis_const,				// Axis to shear with respect to ('x', 'y', or 'z').
		TR t_1, TR t_2					// Amount to shear other axes (in x-y-z-x order).
	)
	{
		// Initialise to identity matrix.
		new(this) CMatrix3<TR>;

		switch (c_axis_const)
		{
			case 'x':
				v3X.tY = t_1;
				v3X.tZ = t_2;
				break;
			case 'y':
				v3Y.tZ = t_1;
				v3Y.tX = t_2;
				break;
			case 'z':
				v3Z.tX = t_1;
				v3Z.tY = t_2;
				break;
			default:
				Assert(0);
		}
	}

	//******************************************************************************************
	//
	// Operators.
	//

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
	//
	//	CVector3<TR> VdotM
	//	(
	//		CVector3<TR> v,
	//		CMatrix3<TR> m
	//	)
	//
	//	Multiply a vector by a matrix.  This is a macro to override VC 4.2's abominable
	//	inlining "ability".
	//
	//	Returns:
	//		A sequence of 3 values representing the vector transformed by the matrix.
	//		These values can be inserted directly into vector or matrix constructors.
	//
	//	Cross references:
	//		Used below by vector*matrix.
	//
	//**********************************
	#define VdotM(v, m)		VdotMC(v, m, tX), VdotMC(v, m, tY), VdotMC(v, m, tZ)

	// Return the matrix inverse.
	CMatrix3<TR> operator ~() const;

	//
	// The basic matrx*matrix multiplication.
	//

	friend CMatrix3<TR> operator *(const CMatrix3<TR>& mx3_a, const CMatrix3<TR>& mx3_b)
	{
		return CMatrix3<TR>
		(
			VdotM(mx3_a.v3X, mx3_b),
			VdotM(mx3_a.v3Y, mx3_b),
			VdotM(mx3_a.v3Z, mx3_b)
		);
	}


	CMatrix3<TR>& operator *=(const CMatrix3<TR>& mx3)
	{
		return *this = *this * mx3;
	}

	friend CMatrix3<TR> operator /(const CMatrix3<TR>& mx3_a, const CMatrix3<TR>& mx3_b)
	{
		// Not optimised.
		return mx3_a * ~mx3_b;
	}

	CMatrix3<TR>& operator /=(const CMatrix3<TR>& mx3)
	{
		// Not optimised.
		return *this = *this * ~mx3;
	}

	//
	// The basic vector*matrix multiplication.
	//

	friend forceinline CVector3<TR> operator *(const CVector3<TR>& v3, const CMatrix3<TR>& mx3)
	{
	#ifndef __MWERKS__
		// Check for aliasing.
		Assert(that != &v3);

		that->tX = VdotMC(v3, mx3, tX);
		that->tY = VdotMC(v3, mx3, tY);
		that->tZ = VdotMC(v3, mx3, tZ);
		return *that;
	#else
		CVector3<TR> rv;	// Return Value
		rv.tX = VdotMC(v3, mx3, tX);
		rv.tY = VdotMC(v3, mx3, tY);
		rv.tZ = VdotMC(v3, mx3, tZ);
		return rv;
	#endif
	}

	friend forceinline CVector3<TR>& operator *=(CVector3<TR>& v3, const CMatrix3<TR>& mx3)
	{
		return v3 = v3 * mx3;
	}

	friend forceinline CVector3<TR> operator /(const CVector3<TR>& v3, const CMatrix3<TR>& mx3)
	{
		return mx3.v3InvMat(v3);
	}

	friend forceinline CVector3<TR>& operator /=(CVector3<TR>& v3, const CMatrix3<TR>& mx3)
	{
		return v3 = v3 / mx3;
	}

	//
	// Add directions here to avoid ambiguity with CTransform3 below.
	// These will only work with a pure rotation matrix. 
	// To transform a direction with an non-orthonormal matrix, convert the direction
	// to a vector, then the result back to a direction.
	//

	friend forceinline CDir3<TR> operator *(const CDir3<TR>& d3, const CMatrix3<TR>& mx3)
	{
		// Transform the vector, then call the CDir3 constructor which asserts for normalisation.
		return CDir3<>((CVector3<TR>&)d3 * mx3, true);
	}

	friend forceinline CDir3<TR>& operator *=(CDir3<TR>& d3, const CMatrix3<TR>& mx3)
	{
		return d3 = d3 * mx3;
	}

	friend forceinline CDir3<TR> operator /(const CDir3<TR>& d3, const CMatrix3<TR>& mx3)
	{
		return CDir3<>((CVector3<TR>&)d3 / mx3, true);
	}

	friend forceinline CDir3<TR>& operator /=(CDir3<TR>& d3, const CMatrix3<TR>& mx3)
	{
		return d3 = d3 / mx3;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	CMatrix3<TR> mx3Transpose() const
	//
	// Returns:
	//		The matrix transpose.
	//
	//**********************************
	{
		return CMatrix3<TR>
		(
			v3X.tX, v3Y.tX, v3Z.tX,
			v3X.tY, v3Y.tY, v3Z.tY,
			v3X.tZ, v3Y.tZ, v3Z.tZ
		);
	}

protected:

	//******************************************************************************************
	//
	TR tDet() const;
	//
	// Returns:
	//		The matrix determinant.
	//
	//**********************************

	//******************************************************************************************
	//
	CVector3<TR> v3InvMat(const CVector3<TR>& v3) const;
	//
	// Returns:
	//		v3 transformed by the inverse of this matrix.
	//
	//**********************************
};

//**********************************************************************************************
//
// Global functions for CMatrix3.
//
// These are helpful functions that let other transforms operate on a matrix by simply operating 
// on its component vectors.  Other transforms' operator functions can call these.
//

	//**********************************************************************************************
	//
	template<class TR, class T3> inline CMatrix3<TR> mx3Transform
	(
		const CMatrix3<TR>& mx3,
		const T3& t3
	)
	// Returns:
	//		The matrix transformed by t3.
	//
	//**************************************
	{
		// Transform the matrix by transforming each vector by t3.
		return CMatrix3<TR>
		(
			mx3.v3X * t3,
			mx3.v3Y * t3,
			mx3.v3Z * t3
		);
	}

	//**********************************************************************************************
	//
	template<class TR, class T3> inline CMatrix3<TR>& Transform
	(
		CMatrix3<TR>& mx3, 
		const T3& t3
	)
	//
	// Transforms mx3 in place by t3.
	//
	// Returns:
	//		A reference to mx3.
	// 
	//**************************************
	{
		// Transform each vector.
		mx3.v3X *= t3;
		mx3.v3Y *= t3;
		mx3.v3Z *= t3;
		return mx3;
	}

extern const CMatrix3<> mx3Null;

//**********************************************************************************************
//
template<class TR = TReal> class CTransform3
//
// Prefix: tf3
//
// Encodes any linear transformation, including translation.
// Contains a matrix and translation vector (the matrix operates before the translation).
//
// Note: this is not inherited from CMatrix3 because trying to override operators makes the
// compiler complain about ambiguities (which is a compiler bug).
//
//**************************************
{
public:
	CMatrix3<TR>	mx3Mat;			// The non-translating transform.
	CVector3<TR>	v3Pos;		// The translation to add.

public:
	
	//******************************************************************************************
	//
	// Constructors.
	//

	CTransform3()
	{
		v3Pos.tX = v3Pos.tY = v3Pos.tZ = (TR)0;
	}

	//
	// Provide constructors for all combinations of matrix and translation.
	//

	CTransform3(const CMatrix3<TR>& mx3, const CVector3<TR>& v3)
		: mx3Mat(mx3), v3Pos(v3)
	{
	}

	CTransform3(const CMatrix3<TR>& mx3)
		: mx3Mat(mx3), v3Pos((TR)0, (TR)0, (TR)0)
	{
	}

	// We can convert from a CTranslate3 to a CTransform3, but not directly from
	// a CVector3 to a CTranslate3, because that would cause too many ambiguities.
	CTransform3(const CTranslate3<TR>& tl3)
		: v3Pos(tl3.v3Pos)
	{
	}

	//******************************************************************************************
	//
	void SetOrigin
	(
		const CVector3<TR>& v3_origin	// Point acting as origin of transformation.
	)
	//
	// Adjusts the transform so that the non-translating operation appears to be centred on 
	// v3_origin.  Thus, the point v3_origin remains untransformed, while other points in space
	// transform linearly with respect to their distance from v3_origin.
	//
	// For example, if tf3 were a rotation transformation, and we wanted the rotation to pivot
	// about point (1,2,3), we would call:
	//
	//		tf3.SetOrigin(CVector3<TR>(1, 2, 3));
	//
	//**********************************
	{
		//
		// To do the operation, first translate the object from v3_at back to the origin, 
		// do the transformation, then put the object back at v3_at.
		//
		// This is the same as just setting the translation to -v3_at * mx3 + v3_at.
		//
		v3Pos = -v3_origin * mx3Mat + v3_origin;
	}

	//******************************************************************************************
	//
	void AdjustOrigin
	(
		const CVector3<TR>& v3_origin	// Point acting as origin of transformation.
	)
	//
	// Adjusts the transform so that it appears to be centred on v3_origin.  This is similar
	// to SetOrigin() above, except that the transform's current translation is kept, and added
	// to the new translation.
	//
	//**********************************
	{
		v3Pos += -v3_origin * mx3Mat + v3_origin;
	}

	//******************************************************************************************
	//
	friend forceinline CTransform3<TR> TransformAt
	(
		const CMatrix3<TR>& mx3,			// A matrix transformation.
		const CVector3<TR>& v3_origin	// Point acting as origin of transformation.
	)
	//
	// Returns:
	//		A CTransform3 which performs the transformation mx3 as if the point v3_at
	//		were the origin.
	//
	// Notes:
	//		This function does not have a type prefix because it is meant to be generic.
	//		There is another version for CRotate3 and CPlacement3.
	//
	//**********************************
	{
		return CTransform3<TR>(mx3, -v3_origin * mx3 + v3_origin);
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
	CTransform3<TR> operator ~() const
	{
		// The inverse of a composite operation S*TR is ~TR * ~S, which is CTransform3(~S, -TR * ~S).
	#ifndef __MWERKS__
		that->mx3Mat = ~mx3Mat;
		that->v3Pos  = -v3Pos * that->mx3Mat;
		return *that;
	#else
		CTransform3<TR> rv;		// Return Value
		rv.mx3Mat = ~mx3Mat;
		rv.v3Pos  = -v3Pos * rv.mx3Mat;
		return rv;
	#endif
	}

	//
	// Concatenate with another CTransform3.
	//

	friend CTransform3<TR> operator *(const CTransform3<TR>& tf3_a, const CTransform3<TR>& tf3_b)
	{
		return CTransform3<TR>(tf3_a.mx3Mat * tf3_b.mx3Mat, tf3_a.v3Pos * tf3_b);
	}

	CTransform3<TR>& operator *=(const CTransform3<TR>& tf3);

	CTransform3<TR> operator /(const CTransform3<TR>& tf3);

	CTransform3<TR>& operator /=(const CTransform3<TR>& tf3);

	//
	// Concatenate with a CMatrix3.
	//

	friend CTransform3<TR> operator *(const CTransform3<TR>& tf3, const CMatrix3<TR>& mx3)
	{
		return CTransform3<TR>(tf3.mx3Mat * mx3, tf3.v3Pos * mx3);
	}

	CTransform3<TR>& operator *=(const CMatrix3<TR>& mx3);

	friend CTransform3<TR> operator *(const CMatrix3<TR>& mx3, const CTransform3<TR>& tf3)
	{
		return CTransform3<TR>(mx3 * tf3.mx3Mat, tf3.v3Pos);
	}


	//
	// Concatenate with a translation.
	//

	friend CTransform3<TR> operator *(const CTransform3<TR>& tf3, const CTranslate3<TR>& tl3)
	{
		return CTransform3<TR>(tf3.mx3Mat, tf3.v3Pos + tl3.v3Pos);
	}

	CTransform3<TR>& operator *=(const CTranslate3<TR>& tl3)
	{
		v3Pos += tl3.v3Pos;
		return *this;
	}

	friend forceinline CTransform3<TR> operator *(const CTranslate3<TR>& tl3, const CTransform3<TR>& tf3)
	{
		return CTransform3<TR>(tf3.mx3Mat, tl3.v3Pos * tf3);
	}

	//
	// Construct from a matrix and translation.
	//

	friend forceinline CTransform3<TR> operator *(const CMatrix3<TR>& mx3, const CTranslate3<TR>& tl3)
	{
		return CTransform3<TR>(mx3, tl3.v3Pos);
	}

	friend forceinline CTransform3<TR> operator *(const CTranslate3<TR>& tl3, const CMatrix3<TR>& mx3)
	{
		return CTransform3<TR>(mx3, tl3.v3Pos * mx3);
	}

	//
	// Operate on a vector.
	//

	friend forceinline CVector3<TR> operator *(const CVector3<TR>& v3, const CTransform3<TR>& tf3)
	{
		// Check for aliasing.
	#ifndef __MWERKS__
		Assert(that != &v3);

		that->tX = VdotMC(v3, tf3.mx3Mat, tX) + tf3.v3Pos.tX;
		that->tY = VdotMC(v3, tf3.mx3Mat, tY) + tf3.v3Pos.tY;
		that->tZ = VdotMC(v3, tf3.mx3Mat, tZ) + tf3.v3Pos.tZ;
		return *that;
	#else
		CVector3<TR> rv;	// Return Value
		rv.tX = VdotMC(v3, tf3.mx3Mat, tX) + tf3.v3Pos.tX;
		rv.tY = VdotMC(v3, tf3.mx3Mat, tY) + tf3.v3Pos.tY;
		rv.tZ = VdotMC(v3, tf3.mx3Mat, tZ) + tf3.v3Pos.tZ;
		return rv;
	#endif
	}

	friend forceinline CVector3<TR>& operator *=(CVector3<TR>& v3, const CTransform3<TR>& tf3)
	{
		return v3 = v3 * tf3;
	}

	friend forceinline CVector3<TR> operator /(const CVector3<TR>& v3, const CTransform3<TR>& tf3)
	{
		return (v3 - tf3.v3Pos) / tf3.mx3Mat;
	}

	friend forceinline CVector3<TR>& operator /=(CVector3<TR>& v3, const CTransform3<TR>& tf3)
	{
		v3 -= tf3.v3Pos;
		return v3 /= tf3.mx3Mat;
	}

	//
	// Operate on a direction by skipping the translation step.
	//

	friend forceinline CDir3<TR> operator *(const CDir3<TR>& d3, const CTransform3<TR>& tf3)
	{
		return d3 * tf3.mx3Mat;
	}

	friend forceinline CDir3<TR>& operator *=(CDir3<TR>& d3, const CTransform3<TR>& tf3)
	{
		return d3 *= tf3.mx3Mat;
	}

	friend forceinline CDir3<TR> operator /(const CDir3<TR>& d3, const CTransform3<TR>& tf3)
	{
		return d3 / tf3.mx3Mat;
	}

	friend forceinline CDir3<TR>& operator /=(CDir3<TR>& d3, const CTransform3<TR>& tf3)
	{
		return d3 /= tf3.mx3Mat;
	}

};

extern const CTransform3<> tf3Null;

#if _MSC_VER < 1100
template<class X> class CMicrosoftsCompilerIsBroken
{
};
#endif

#endif

