/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CVector2<TR>
 *			CDir2<TR>
 *		CVector3<TR>
 *			CDir3<TR>
 *
 *	2D and 3D vectors, as templates with specifiable coordinate type.
 *
 * Bugs:
 *
 * To do:
 *		Incorporate fast sqrt math routines when they have enough precision.
 *		Optimise sqrt for non-float types if possible.
 *		Remove automatic normalisation for Vector-to-Dir conversion.  Simply Assert.
 *		Supply a specialised scalar divide operator for fixed point, that actually divides
 *		rather than multiplying by reciprocal.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform5/Vector.hpp                                             $
 * 
 * 19    97-04-24 18:40 Speter
 * Folded new changes from 4.2 version into this 5.0 specific version.
 * 
 * 25    97-04-24 14:37 Speter
 * Made bIsNormalised() const.
 * 
 * 24    97-04-23 14:28 Speter
 * Changed tolerance for normalisation asserts (sorry).
 * 
 * 23    97/03/24 14:54 Speter
 * Removed constructors of CDirs and CRotates from ASCII chars; use d3ZAxis etc. constants.
 * When we need optimisation for axes, we'll use derived classes which don't need a switch
 * statement.
 * 
 * 22    97/03/18 21:34 Speter
 * Changed d3ZAxis, etc. to #defines.
 * 
 * 21    3/07/97 10:49a Mlange
 * Added missing template parameter in the overloaded operator / functions in the CVector2 and
 * CVector3 classes. This allows these vector types to be defined with an integer type.
 * 
 * 20    97/02/07 19:12 Speter
 * Added normalisation flag to CDir2 and CDir3 constructors.  Re-introduced d3XAxis, etc.
 * constants.
 * 
 * 19    2/06/97 7:17p Agrant
 * Added a cross product for 2d vectors, returns a scalar.
 * 
 * 18    97/01/29 18:55 Speter
 * In scalar divide functions, manually inlined call to scalar multiply, as compiler sucks.
 * 
 * 17    12/20/96 3:36p Mlange
 * Fixed typo in template parameter.
 * 
 * 16    12/15/96 6:19p Agrant
 * Made bIsZero() const
 * 
 * 15    11/01/96 3:45p Agrant
 * Added bIsZero() to CVector2<> and CVector3<>
 * 
 * 14    96/10/28 19:13 Speter
 * Changed v3-v2 conversion to do parallel projection.
 * Added v2Project function for perspective projection.
 * 
 * 13    10/15/96 9:16p Agrant
 * Added Normalise(T) which allows normalization to an arbitrary scalar size. 
 * 
 * 12    96/10/02 18:57 Speter
 * Added default initialisation to CDir<> types, to avoid asserts for denormalisation.
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
 * 10    96/09/12 16:27 Speter
 * Replaced bFuzzyEquals with Fuzzy ==.
 * 
 * 9     96/09/10 18:25 Speter
 * Changed conversion from CVector2 to CVector3; now sets Z to 0 rather than 1.
 * 
 * 
 * 8     96/08/22 10:26 Speter
 * Added horrible hack to work around MS compiler bug.  Bastards.
 * 
 * 7     96/08/21 18:11 Speter
 * Reverted to previous scalar divide operators, that multiply by reciprocal.
 * 
 * 6     96/08/21 17:49 Speter
 * Changes from code review:
 * Added default template parameter to all classes.
 * Made all in-class friends inline.
 * Scalar divide operators now divide each component, rather than multiplying by reciprocal.
 * Specialised versions for float multiply by reciprocal.  All divide operators now assert for
 * divide by 0.
 * CVector3 to CVector2 conversion now calls divide operator.  Thus it now asserts on divide by
 * 0.
 * CDir2 and CDir3 now have assignment operators taking CVectors.  Constructors call the
 * assignment operators.
 * Changed tLen2() to tLenSqr().
 * Fixed prefixes, formatting issues.
 * 
 * 5     96/07/22 15:49 Speter
 * Added bIsNormalised function.
 * 
 * 4     96/07/08 12:40 Speter
 * Changed name of CNormal3 to CDir3 (more general).
 * Added specific functions to transform directions, which do not translate positions.
 * 
 * 3     96/06/26 13:16 Speter
 * Changed TGReal to TR and prefix gr to r.
 * 
 * 2     96/06/20 17:13 Speter
 * Converted into templates and made necessary changes.
 * 
 * 1     96/06/20 15:26 Speter
 * First version of new optimised transform library.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_VECTOR_HPP
#define HEADER_LIB_TRANSFORM_VECTOR_HPP

// #include "Lib/Math/FastSqrt.hpp"
#include <math.h>

// The default template parameter type for all transform template classes.
typedef float	TReal;

// Amount we allow a squared length to deviate from 1 for normalisation.
#define fMAX_VECTOR_DENORMALISATION	0.01

//**********************************************************************************************
//
template<class TR = TReal> class CVector2
//
// Prefix: v2
//
// A 2-dimensional vector, with parameterised coordinate type TR.
// Has operators for addition, subtraction, scaling, dot product.
// Has functions to return the length, and a normalised CVector2.
//
//**************************************
{
public:
	TR		tX, tY;					// The coordinates.

	//******************************************************************************************
	//
	// Constructors.
	//

	CVector2() 
	{
	}

	CVector2(TR t_x, TR t_y)
		: tX(t_x), tY(t_y)
	{
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Addition and subtraction.

	CVector2<TR>& operator +=(const CVector2<TR>& v2) 
	{
		tX += v2.tX;  
		tY += v2.tY;  
		return *this;
	}

	CVector2<TR> operator +(const CVector2<TR>& v2) const
	{
		return CVector2<TR>(tX + v2.tX, tY + v2.tY);
	}

	CVector2<TR>& operator -=(const CVector2<TR>& v2) 
	{
		tX -= v2.tX;  
		tY -= v2.tY;  
		return *this;
	}

	CVector2<TR> operator -(const CVector2<TR>& v2) const
	{
		return CVector2<TR>(tX - v2.tX, tY - v2.tY);
	}

	// Negation, and scalar multiplication.

	CVector2<TR> operator -() const
	{
		return CVector2<TR>(-tX, -tY);
	}

	CVector2<TR>& operator *=(TR t) 
	{
		tX *= t;  
		tY *= t;  
		return *this;
	}

	CVector2<TR> operator *(TR t) const
	{
		return CVector2<TR>(tX * t, tY * t);
	}

	//
	// Divide operators multiply by reciprocal, under the assumption that this is
	// faster for all TR types.
	//

	CVector2<TR>& operator /=(TR t) 
	{
		Assert(t != 0);
		t = 1 / t;
		tX *= t;
		tY *= t;
		return *this;
	}

	CVector2<TR> operator /(TR t) const
	{
		Assert(t != 0);
		t = 1 / t;
		return CVector2<TR>(tX * t, tY * t);
	}

	// The dot-product is performed by the * operator.

	TR operator *(const CVector2<TR>& v2) const
	{
		return tX * v2.tX  +  tY * v2.tY;
	}

	//
	// The cross-product is performed by the ^ operator.
	// Note: Beware of precedence!  This has *lower* precedence than +!
	//
	//	Also, this function returns the Z component of the cross product as a scalar.
	//
	//	Z is positive when the point v2 is to the left of the line defined by 0,0 and this
	//	Z is negative when ...					  right  ....
	//  Z is fuzzy zero when v2 lies on the line.
	//
	//  Note also that "left" is determined when viewed from above and facing towards this
	//	from the direction of the origin.
	//
	inline TR operator ^(const CVector2<TR>& v2) const
	{
		return tX * v2.tY - tY * v2.tX;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	TR tLenSqr() const
	//
	// Returns:
	//		Square of the magnitude.
	//
	//**********************************
	{
		return tX * tX  +  tY * tY;
	}

	//******************************************************************************************
	//
	TR tLen() const
	//
	// Returns:
	//		Magnitude of the vector.
	//
	// Notes:
	//		This calls the sqrt, taking and returning a double, regardless of the type TR.  
	//		That means that for a non-double TR, there will be a conversion to and from double.
	//
	//**********************************
	{
		return (TR) sqrt((double)tLenSqr());
	}

	//******************************************************************************************
	//
	void Normalise() 
	//
	// Turn this vector into a normal, by scaling the magnitude to 1.
	//
	//**********************************
	{
		*this /= tLen();
	}

	//******************************************************************************************
	//
	void Normalise(TR t_magnitude) 
	//
	// Normalise by scaling this vector's magnitude to "t_magnitude"
	//
	//**********************************
	{
		// Verify that the vector is non-zero.
		Assert(tLenSqr() != 0);

		*this *= t_magnitude / tLen();
	}

	//******************************************************************************************
	//
	bool bIsNormalised() const
	//
	// Returns:
	//		Whether this vector's length is 1.
	//
	//**********************************
	{
		return Fuzzy(tLenSqr(), (TR)fMAX_VECTOR_DENORMALISATION) == TR(1);
	}

	//******************************************************************************************
	//
	bool bIsZero() const
	//
	// Returns:
	//		true if all components of the vector are == 0.
	//
	//**********************************
	{
		return tX == 0 && tY == 0;
	}

};

//**********************************************************************************************
//
template<class TR = TReal> class CDir2: public CVector2<TR>
//
// Prefix: d2
//
// A derived class designed to hold a unit vector.
// When initialised with a regular CVector2, or specific coordinates,
// it normalises the values.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	CDir2() :
		// Initialise to valid value, so that assertion for normalisation doesn't fail.
		CVector2<TR>(1, 0)
	{
	}

	// Initialise with arbitrary coordinates, optionally normalise.
	CDir2(TR t_x, TR t_y, bool b_is_normalised = false)
		: CVector2<TR>(t_x, t_y)
	{
		if (b_is_normalised)
		{
			Assert(bIsNormalised());
		}
		else
			Normalise();
	}

	// Initialise with a vector, optionally normalise.
	CDir2(const CVector2<TR>& v2, bool b_is_normalised = false)
		: CVector2<TR>(v2)
	{
		if (b_is_normalised)
		{
			Assert(bIsNormalised());
		}
		else
			Normalise();
	}

	// Copy initialiser, assert for normalisation.
	CDir2(const CDir2<TR>& d2)
		: CVector2<TR>(d2)
	{
		Assert(bIsNormalised());
	}

	//******************************************************************************************
	//
	// Assignment operators.
	//

	// Copy the vector, then normalise.
	CDir2<TReal>& operator =(const CVector2<TR>& v2)
	{
		new(this) CDir2<>(v2);
		return *this;
	}

	// Copy the direction, then check for normalisation.
	CDir2<TReal>& operator =(const CDir2<TR>& d2)
	{
		new(this) CDir2<>(d2);
		return *this;
	}
};

//**********************************************************************************************
//
template<class TR = TReal> class CVector3
//
// Prefix: v3
//
// A 3-dimensional vector, with coordinate type TR.
// Allows addition and subtraction, scaling, dot and cross products.
//
//**************************************
{
public:
	TR		tX, tY, tZ;					// The coordinates.

	//******************************************************************************************
	//
	// Constructors.
	//

	CVector3() 
	{
	}

	CVector3(TR t_x, TR t_y, TR t_z)
		: tX(t_x), tY(t_y), tZ(t_z)
	{
	}


	// Copy from a CVector2, and set t_z to 0.
	CVector3(const CVector2<TR>& v2)
		: tX(v2.tX), tY(v2.tY), tZ(0)
	{
	}

	//******************************************************************************************
	//
	// Conversion operators.
	//


	// Conversion to CVector2 by discarding tZ.  See v2Project for perspective projection.
	operator CVector2<TR>() const
	{
		return CVector2<TR>(tX, tY);
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Addition and subtraction.

	CVector3<TR>& operator +=(const CVector3<TR>& v3) 
	{
		tX += v3.tX;  
		tY += v3.tY;  
		tZ += v3.tZ;
		return *this;
	}

	CVector3<TR> operator +(const CVector3<TR>& v3) const
	{
		return CVector3<TR>(tX + v3.tX, tY + v3.tY, tZ + v3.tZ);
	}

	CVector3<TR>& operator -=(const CVector3<TR>& v3) 
	{
		tX -= v3.tX;  
		tY -= v3.tY;  
		tZ -= v3.tZ;
		return *this;
	}

	CVector3<TR> operator -(const CVector3<TR>& v3) const
	{
		return CVector3<TR>(tX - v3.tX, tY - v3.tY, tZ - v3.tZ);
	}

	// Negation, and scalar multiplication.

	CVector3<TR> operator -() const
	{
		return CVector3<TR>(-tX, -tY, -tZ);
	}

	CVector3<TR>& operator *=(TR t) 
	{
		tX *= t;  
		tY *= t;  
		tZ *= t;
		return *this;
	}

	CVector3<TR> operator *(TR t) const
	{
		return CVector3<TR>(tX * t, tY * t, tZ * t);
	}

	//
	// Divide operators multiply by reciprocal, under the assumption that this is
	// faster for all TR types.
	//

	CVector3<TR>& operator /=(TR t) 
	{
		Assert(t != 0);
		t = 1 / t;
		tX *= t;
		tY *= t;
		tZ *= t;
		return *this;
	}

	CVector3<TR> operator /(TR t) const
	{
		Assert(t != 0);
		t = 1 / t;
		return CVector3<TR>(tX * t, tY * t, tZ * t);
	}

	// The dot-product is performed by the * operator.
	TR operator *(const CVector3<TR>& v3) const
	{
		return tX * v3.tX  +  tY * v3.tY  +  tZ * v3.tZ;
	}

	//
	// The cross-product is performed by the ^ operator.
	// Note: Beware of precedence!  This has *lower* precedence than +!
	//
	CVector3<TR> operator ^(const CVector3<TR>& v3) const
	{
		return CVector3<TR>
		(
			tY * v3.tZ  -  tZ * v3.tY, 
			tZ * v3.tX  -  tX * v3.tZ, 
			tX * v3.tY  -  tY * v3.tX
		);
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	TR tLenSqr() const
	//
	// Returns:
	//		Square of the magnitude.
	//
	//**********************************
	{
		return tX * tX + tY * tY + tZ * tZ;
	}

	//******************************************************************************************
	//
	TR tLen() const
	//
	// Returns:
	//		Magnitude of the vector.
	//
	// Notes:
	//		This calls the sqrt, taking and returning a double, regardless of the type TR.  
	//		That means that for a non-double TR, there will be a conversion to and from double.
	//
	//**********************************
	{
		return (TR) sqrt((double)tLenSqr());
	}

	//******************************************************************************************
	//
	void Normalise() 
	//
	// Turn this vector into a normal, by scaling the magnitude to 1.
	//
	//**********************************
	{
		*this /= tLen();
	}

	//******************************************************************************************
	//
	bool bIsNormalised() const
	//
	// Returns:
	//		Whether this vector's length is 1.
	//
	//**********************************
	{
		return Fuzzy(tLenSqr(), (TR)fMAX_VECTOR_DENORMALISATION) == TR(1);
	}

	//******************************************************************************************
	//
	bool bIsZero() const
	//
	// Returns:
	//		true if all components of the vector are == 0.
	//
	//**********************************
	{
		return tX == 0 && tY == 0 && tZ == 0;
	}

	//******************************************************************************************
	//
	CVector2<TR> v2Project() const
	//
	// Returns:
	//		The perspective projection onto the xy plane of the vector.
	//
	//**********************************
	{
		return CVector2<TR>(tX, tY) / tZ;
	}
};

//**********************************************************************************************
//
template<class TR = TReal> class CDir3: public CVector3<TR>
//
// Prefix: d3
//
// A derived class designed to hold a unit vector.
// When initialised by a regular CVector3 or specific coordinates, they are normalised.
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors.
	//

	CDir3()
		// The default CDir3 initialises to the Z axis, so that assertion for normalisation doesn't fail.
		: CVector3<TR>(0, 0, 1)
	{
	}

	// Initialise with arbitrary coordinates, optionally normalise.
	CDir3(TR t_x, TR t_y, TR t_z, bool b_is_normalised = false)
		: CVector3<TR>(t_x, t_y, t_z)
	{
		if (b_is_normalised)
		{
			Assert(bIsNormalised());
		}
		else
			Normalise();
	}

	// Initialise with a vector, optionally normalise.
	CDir3(const CVector3<TR>& v3, bool b_is_normalised = false)
		: CVector3<TR>(v3)
	{
		if (b_is_normalised)
		{
			Assert(bIsNormalised());
		}
		else
			Normalise();
	}

	// Copy initialiser, assert for normalisation.
	CDir3(const CDir3<TR>& d3)
		: CVector3<TR>(d3)
	{
		Assert(bIsNormalised());
	}

	//******************************************************************************************
	//
	// Assignment operators.
	//

	// Copy the vector, then normalise.
	CDir3<TR>& operator =(const CVector3<TR>& v3)
	{
		new(this) CDir3<TR>(v3);
		return *this;
	}

	// Copy the direction, then check for normalisation.
	CDir3<TR>& operator =(const CDir3<TR>& d3)
	{
		new(this) CDir3<TR>(d3);
		return *this;
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Duplicate
	CDir3<TR> operator -() const
	{
		return CDir3<TR>(-tX, -tY, -tZ, true);
	}
#if _MSC_VER < 1100
	//
	// Duplicate all versions of a dot product between normal and vector, 
	// to avoid ambiguities with other transform operators (compiler bug re templates).
	//

	friend inline TR operator *(const CDir3<TR>& d3_a, const CDir3<TR>& d3_b)
	{
		return (const CVector3<TR>&) d3_a * (const CVector3<TR>&) d3_b;
	}

	friend inline TR operator *(const CVector3<TR>& v3_a, const CDir3<TR>& d3_b)
	{
		return v3_a * (const CVector3<TR>&) d3_b;
	}

	friend inline TR operator *(const CDir3<TR>& d3_a, const CVector3<TR>& v3_b)
	{
		return (const CVector3<TR>&) d3_a * (const CVector3<TR>&) v3_b;
	}
#endif
};

	//******************************************************************************************
	//
	// Constants.
	//

	//
	// The following are #defines rather than consts to avoid errors when initialisation code
	// uses these values before the compiler has initialised them.  I don't know whether this
	// is a compiler "bug" or not, but consts and static construction code don't mix!
	//
	#define d3XAxis	CDir3<>(1, 0, 0, true)
	#define d3YAxis	CDir3<>(0, 1, 0, true)
	#define d3ZAxis	CDir3<>(0, 0, 1, true)


#if _MSC_VER < 1100
//
// Now declare a dummy template class with *no default type*.
// Otherwise, you get a spurious syntax error on the next non-template class declared after
// including "Vector.hpp".  Try it, you'll see.
//
template<class X> class CMicrosoftsCompilerIsBuggy
{
};
#endif

#endif

