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
 * $Log:: /JP2_PC/Source/Lib/Transform5/Matrix.hpp                                             $
 * 
 * 14    97-04-24 18:40 Speter
 * Folded new changes from 4.2 version into this 5.0 specific version.
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
 * 
 * 11    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 10    96/08/21 17:55 Speter
 * Changes from code review:
 * Added default template parameter to all classes.
 * Made all in-class friends inline.
 * Default CMatrix3() constructor now inline.
 * Added CMatrix3 * CDir3 operators to avoid ambiguities with CTransform3 * CDir3.
 * Added SetOrigin() and AdjustOrigin() functions.
 * Added comments.
 * 
 * 9     96/08/06 18:18 Speter
 * Added CTransform3 constructor taking a CTranslate3.
 * 
 * 8     96/07/22 15:32 Speter
 * Added operator *(char axis, CMatrix) function for fast axis multiplication.
 * 
 * 7     96/07/08 12:40 Speter
 * Changed name of CNormal3 to CDir3 (more general).
 * Added specific functions to transform directions, which do not translate positions.
 * 
 * 6     96/06/26 22:07 Speter
 * Added a bunch of comments, prettied things up nicely.
 * 
 * 5     96/06/26 16:43 Speter
 * Added TransformAt function.
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
		// Assume that the representation of 0 for any type is all 0 bits.
		memset(this, 0, sizeof(*this));
		v3X.tX = v3Y.tY = v3Z.tZ = TR(1);
	}

	// Initialise directly with 3 vectors.
	CMatrix3(const CVector3<TR>& v3_x, const CVector3<TR>& v3_y, const CVector3<TR>& v3_z) 
		: v3X(v3_x), v3Y(v3_y), v3Z(v3_z)
	{
	}

	// Initialise directly with 9 elements.
	CMatrix3(TR t_xx, TR t_xy, TR t_xz, TR t_yx, TR t_yy, TR t_yz, TR t_zx, TR t_zy, TR t_zz)
		: v3X(t_xx, t_xy, t_xz),  v3Y(t_yx, t_yy, t_yz),  v3Z(t_zx, t_zy, t_zz)
	{
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

	// Return the matrix inverse.
	CMatrix3<TR> operator ~() const;

	//
	// The basic matrix*matrix multiplication.
	//

	CMatrix3<TR>& operator *=(const CMatrix3<TR>& mx3)
	{
		return *this = *this * mx3;
	}

	//
	// The basic vector*matrix multiplication.
	//

	friend CVector3<TR> operator *(const CVector3<TR>& v3, const CMatrix3<TR>& mx3);

	friend inline CVector3<TR>& operator *=(CVector3<TR>& v3, const CMatrix3<TR>& mx3)
	{
		return v3 = v3 * mx3;
	}

	// Add directions here to avoid ambiguity with CTransform3 below.
	friend CDir3<TR> operator *(const CDir3<TR>& d3, const CMatrix3<TR>& mx3)
	{
		return (CVector3<TR>&)d3 * mx3;
	}

	friend inline CDir3<TR>& operator *=(CDir3<TR>& d3, const CMatrix3<TR>& mx3)
	{
		return d3 = d3 * mx3;
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

	friend CVector3<TR> operator *(const CVector3<TR>& v3, const CMatrix3<TR>& mx3);

};

//**********************************************************************************************
//
// Global operators for CMatrix3<>.
//
	//
	// The basic vector*matrix multiplication.
	//

	template<class TR> inline CVector3<TR>& operator *=(CVector3<TR>& v3, const CMatrix3<TR>& mx3)
	{
		return v3 = v3 * mx3;
	}

/*
	// Add directions here to avoid ambiguity with CTransform3 below.
	template<class TR> CDir3<TR> operator *(const CDir3<TR>& d3, const CMatrix3<TR>& mx3)
	{
		return (CVector3<TR>&)d3 * mx3;
	}

	template<class TR> inline CDir3<TR>& operator *=(CDir3<TR>& d3, const CMatrix3<TR>& mx3)
	{
		return d3 = d3 * mx3;
	}
*/

	// Optimised function to transform unix axes.
	template<class TR> inline CDir3<TR> operator *(char c_axis, const CMatrix3<TR>& mx3)
	{
		// Simply return the appropriate row of the matrix.
		// Hopefully, the compiler will optimise out the switch statement for a constant char.
		switch (c_axis)
		{
			case 'x':
				return mx3.v3X;
			case 'y':
				return mx3.v3Y;
			case 'z':
				return mx3.v3Z;
			default:
				Assert(0);

				// Dummy return to make compiler happy.
				return mx3.v3X;
		}
	}

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

	//
	// Now the matrix multiplication operator can be implemented.
	//

	//**********************************************************************************************
	//
	template<class TR> inline CMatrix3<TR> operator *(const CMatrix3<TR>& mx3_a, const CMatrix3<TR>& mx3_b)
	// 
	//**************************************
	{
		return mx3Transform(mx3_a, mx3_b);
	}


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
		: v3Pos((TR)0, (TR)0, (TR)0)
	{
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
		CMatrix3<TR> mx3_inverse = ~mx3Mat;
		return CTransform3<TR>(mx3_inverse, -v3Pos * mx3_inverse);
	}

	CTransform3<TR> operator *(const CTransform3<TR>& tf3) const
	{
		return CTransform3<TR>(mx3Mat * tf3.mx3Mat, v3Pos * tf3);
	}

	CTransform3<TR>& operator *=(const CTransform3<TR>& tf3)
	{
		// Concatenate base transform and translation separately.
		mx3Mat *= tf3.mx3Mat;
		v3Pos  *= tf3;
		return *this;
	}

	//
	// Concatenate with a CMatrix3.
	//

	CTransform3<TR> operator *(const CMatrix3<TR>& mx3) const
	{
		return CTransform3<TR>(mx3Mat * mx3, v3Pos * mx3);
	}

	CTransform3<TR>& operator *=(const CMatrix3<TR>& mx3)
	{
		// Concatenate matrix and translation separately.
		mx3Mat *= mx3;
		v3Pos  *= mx3;
		return *this;
	}

	//
	// Concatenate with a translation.
	//

	CTransform3<TR> operator *(const CTranslate3<TR>& tl3) const
	{
		return CTransform3<TR>(mx3Mat, v3Pos + tl3.v3Pos);
	}

	CTransform3<TR>& operator *=(const CTranslate3<TR>& tl3)
	{
		v3Pos += tl3.v3Pos;
		return *this;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

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

};


//******************************************************************************************
//
// Global operators for CTransform3<>.
//

	template<class TR> inline CTransform3<TR> operator *(const CMatrix3<TR>& mx3, const CTransform3<TR>& tf3)
	{
		return CTransform3<TR>(mx3 * tf3.mx3Mat, tf3.v3Pos);
	}

	template<class TR> inline CTransform3<TR> operator *(const CTranslate3<TR>& tl3, const CTransform3<TR>& tf3)
	{
		return CTransform3<TR>(tf3.mx3Mat, tl3.v3Pos * tf3);
	}

	//
	// Construct from a matrix and translation.
	//

	template<class TR> inline CTransform3<TR> operator *(const CMatrix3<TR>& mx3, const CTranslate3<TR>& tl3)
	{
		return CTransform3<TR>(mx3, tl3.v3Pos);
	}

	template<class TR> inline CTransform3<TR> operator *(const CTranslate3<TR>& tl3, const CMatrix3<TR>& mx3)
	{
		return CTransform3<TR>(mx3, tl3.v3Pos * mx3);
	}

	//
	// Operate on a vector.
	//

	template<class TR> CVector3<TR> operator *(const CVector3<TR>& v3, const CTransform3<TR>& tf3);

	template<class TR> inline CVector3<TR>& operator *=(CVector3<TR>& v3, const CTransform3<TR>& tf3)
	{
		return v3 = v3 * tf3;
	}

	//
	// Operate on a direction by skipping the translation step.
	//

	template<class TR> inline CDir3<TR> operator *(const CDir3<TR>& d3, const CTransform3<TR>& tf3)
	{
		return d3 * tf3.mx3Mat;
	}

	template<class TR> inline CDir3<TR>& operator *=(CDir3<TR>& d3, const CTransform3<TR>& tf3)
	{
		return d3 *= tf3.mx3Mat;
	}

//******************************************************************************************
//
// Global functions for CTransform3<>.
//

	//******************************************************************************************
	//
	template<class TR> inline CTransform3<TR> TransformAt
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
// We must include all the implementation code because this is a template class.
//

#include "Matrix.cpp"


#endif
