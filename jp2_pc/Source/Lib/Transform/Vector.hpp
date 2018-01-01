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
 *		Optimise sqrt for non-float types if possible.
 *		Supply a specialised scalar divide operator for fixed point, that actually divides
 *		rather than multiplying by reciprocal.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/Vector.hpp                                              $
 * 
 * 47    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 46    98/09/12 0:52 Speter
 * Added enumeration and subscript access for vectors; useful in loops.
 * 
 * 45    98/08/25 19:03 Speter
 * Now use fInvSqrt function in vectors.
 * 
 * 44    6/03/98 8:06p Pkeet
 * Added the 'tManhattanDistance' member function for 3D vectors.
 * 
 * 43    12/20/97 6:56p Agrant
 * v2ZERO should have been v2Zero
 * 
 * 42    97.12.10 3:01p Mmouni
 * Changed fSqrt, fInvSqrt to fSqrtEst, fInvSqrtEst to reflect changes in the functions.
 * 
 * 41    97/12/03 17:59 Speter
 * Changed vector constants from #defines to const variables, for efficiency.
 * 
 * 40    11/10/97 5:58p Agrant
 * pcSave and pcLoad functions
 * 
 * 39    8/28/97 4:04p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 40    97/08/25 11:50a Pkeet
 * Overloaded * for CDirs now needed for 5.0 as well.
 * 
 * 39    97/08/22 11:10 Speter
 * Changed CVector2<>::SetMin and SetMax to take CVector2 as argument, not CVector3.
 * 
 * 38    97/08/08 15:23 Speter
 * Corrected several variables which used default template parameter to use TR instead.
 * 
 * 37    97/08/01 15:15 Speter
 * Added SetMin() and SetMax() to CVector2<>.
 * 
 * 36    7/30/97 7:03p Agrant
 * Make an arbitrary normalise function for 3d vectors.
 * 
 * 35    97/07/22 2:26p Pkeet
 * Added the 'FastNormalise' member function.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_VECTOR_HPP
#define HEADER_LIB_TRANSFORM_VECTOR_HPP

#include "Lib/Math/FastSqrt.hpp"

// The default template parameter type for all transform template classes.
typedef float	TReal;

// Amount we allow a squared length to deviate from 1 for normalisation.
#define fMAX_VECTOR_DENORMALISATION	0.01

//******************************************************************************************
enum EAxis
// Prefix: ea
// Enum for addressing vector elements; useful in loops, etc.
{
	eX, eY, eZ
};

inline EAxis eaMod(int i_ea)
{
	return EAxis(i_ea % 3);
}

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

//lint -save -e1401
// Yeah, we know that we aren't initializing tX, tY, and tZ.
	CVector2() 
	{
	}
//lint -restore

	CVector2(TR t_x, TR t_y)
		: tX(t_x), tY(t_y)
	{
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Access.
	const TR& operator[](EAxis ea) const
	{
		Assert(bWithin(ea, eX, eY));
		return (&tX)[(int)ea];
	}

	TR& operator[](EAxis ea)
	{
		Assert(bWithin(ea, eX, eY));
		return (&tX)[(int)ea];
	}


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
	TR operator ^(const CVector2<TR>& v2) const
	{
		return tX * v2.tY - tY * v2.tX;
	}

	// Comparison operators.

	bool operator ==(const CVector2<TR>& v2) const
	{
//lint -save -e777
//  Yes, we really want to compare floats with == here.
		// Test X and Y coordinates.
		return (u4FromFloat(tX) == u4FromFloat(v2.tX)) &&
			   (u4FromFloat(tY) == u4FromFloat(v2.tY));
//lint -restore
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
	//		This calls the float fSqrt function.
	//		That means that for a non-float TR, there will be a conversion to and from float.
	//
	//**********************************
	{
		return (TR) fSqrt((float)tLenSqr());
	}

	//******************************************************************************************
	//
	TR tInvLen() const
	//
	// Returns:
	//		Reciprocal magnitude of the vector.
	//
	//**********************************
	{
		return (TR) fInvSqrt((float)tLenSqr());
	}

	//******************************************************************************************
	//
	void Normalise() 
	//
	// Turn this vector into a normal, by scaling the magnitude to 1.
	//
	//**********************************
	{
		*this *= tInvLen();
	}

	//******************************************************************************************
	//
	void FastNormalise() 
	//
	// Turn this vector into a normal, by scaling the magnitude to 1.
	//
	//**********************************
	{
	{
		// Calculate the length of the vector using the fast inverse square root function.
		float f_inv_len = fInvSqrtEst(tX * tX + tY * tY);

		// Normalize the vector.
		*this *= f_inv_len;
	}
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

		*this *= t_magnitude * tInvLen();
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

	//******************************************************************************************
	//
	void SetMin
	(
		const CVector2<TR>& v2
	)
	//
	// Sets the values of this vector to the minimum of the existing values or the values
	// passed as a parameter.
	//
	//**********************************
	{
		tX = Min(tX, v2.tX);
		tY = Min(tY, v2.tY);
	}

	//******************************************************************************************
	//
	void SetMax
	(
		const CVector2<TR>& v2
	)
	//
	// Sets the values of this vector to the maximum of the existing values or the values
	// passed as a parameter.
	//
	//**********************************
	{
		tX = Max(tX, v2.tX);
		tY = Max(tY, v2.tY);
	}

	//*****************************************************************************************
	char * pcSave(char * pc_buffer) const
	// Same as the CInstance function of the same name...
	{
		*((CVector2<TR>*)pc_buffer) = *this;
		return pc_buffer + sizeof(CVector2<TR>);
	}

	//*****************************************************************************************
	const char * pcLoad(const char * pc_buffer)
	// Same as the CInstance function of the same name...
	{
		*this = *((CVector2<TR>*)pc_buffer);
		return pc_buffer + sizeof(CVector2<TR>);
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
	CDir2<TR>& operator =(const CVector2<TR>& v2)
	{
		new(this) CDir2<TR>(v2);
		return *this;
	}

	// Copy the direction, then check for normalisation.
	CDir2<TR>& operator =(const CDir2<TR>& d2)
	{
		Assert(this != &d2);
		new(this) CDir2<TR>(d2);
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

//lint -save -e1401
// Yeah, we know that we aren't initializing tX, tY, and tZ.
	CVector3() 
	{
	}
//lint -restore

	forceinline CVector3(TR t_x, TR t_y, TR t_z)
		: tX(t_x), tY(t_y), tZ(t_z)
	{
	}


	// Copy from a CVector2, and specify Z.
	forceinline CVector3(const CVector2<TR>& v2, TR t_z = 0)
		: tX(v2.tX), tY(v2.tY), tZ(t_z)
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

	// Access.

	const TR& operator[](EAxis ea) const
	{
		Assert(bWithin(ea, eX, eZ));
		return (&tX)[(int)ea];
	}

	TR& operator[](EAxis ea)
	{
		Assert(bWithin(ea, eX, eZ));
		return (&tX)[(int)ea];
	}

	// Addition and subtraction.

	forceinline CVector3<TR>& operator +=(const CVector3<TR>& v3) 
	{
		tX += v3.tX;  
		tY += v3.tY;  
		tZ += v3.tZ;
		return *this;
	}

	forceinline CVector3<TR> operator +(const CVector3<TR>& v3) const
	{
		return CVector3<TR>(tX + v3.tX, tY + v3.tY, tZ + v3.tZ);
	}

	forceinline CVector3<TR>& operator -=(const CVector3<TR>& v3) 
	{
		tX -= v3.tX;  
		tY -= v3.tY;  
		tZ -= v3.tZ;
		return *this;
	}

	forceinline CVector3<TR> operator -(const CVector3<TR>& v3) const
	{
		return CVector3<TR>(tX - v3.tX, tY - v3.tY, tZ - v3.tZ);
	}

	// Negation, and scalar multiplication.

	forceinline CVector3<TR> operator -() const
	{
		return CVector3<TR>(-tX, -tY, -tZ);
	}

	forceinline CVector3<TR>& operator *=(TR t) 
	{
		tX *= t;  
		tY *= t;  
		tZ *= t;
		return *this;
	}

	forceinline CVector3<TR> operator *(TR t) const
	{
		return CVector3<TR>(tX * t, tY * t, tZ * t);
	}

	//
	// Divide operators multiply by reciprocal, under the assumption that this is
	// faster for all TR types.
	//

	forceinline CVector3<TR>& operator /=(TR t) 
	{
		Assert(t != 0);
		t = 1 / t;
		tX *= t;
		tY *= t;
		tZ *= t;
		return *this;
	}

	forceinline CVector3<TR> operator /(TR t) const
	{
		Assert(t != 0);
		t = 1 / t;
		return CVector3<TR>(tX * t, tY * t, tZ * t);
	}

	// The dot-product is performed by the * operator.
	forceinline TR operator *(const CVector3<TR>& v3) const
	{
		return tX * v3.tX  +  tY * v3.tY  +  tZ * v3.tZ;
	}

	//
	// The cross-product is performed by the ^ operator.
	// Note: Beware of precedence!  This has *lower* precedence than +!
	//
	forceinline CVector3<TR> operator ^(const CVector3<TR>& v3) const
	{
		return CVector3<TR>
		(
			tY * v3.tZ  -  tZ * v3.tY, 
			tZ * v3.tX  -  tX * v3.tZ, 
			tX * v3.tY  -  tY * v3.tX
		);
	}


	forceinline bool operator ==(const CVector3<TR>& v3) const
	{
//lint -save -e777
//  Yes, we really want to compare floats with == here.
		return tX == v3.tX && tY == v3.tY && tZ == v3.tZ;
//lint -restore
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	forceinline TR tLenSqr() const
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
	//		This calls fSqrt, taking and returning a float, regardless of the type TR.  
	//		That means that for a non-float TR, there will be a conversion to and from float.
	//
	//**********************************
	{
		return (TR) fSqrt((float)tLenSqr());
	}

	//******************************************************************************************
	//
	TR tInvLen() const
	//
	// Returns:
	//		Reciprocal magnitude of the vector.
	//
	//**********************************
	{
		return (TR) fInvSqrt((float)tLenSqr());
	}

	//******************************************************************************************
	//
	TR tManhattanDistance
	(
	) const
	//
	// Returns an approximation of the magnitude of the vector.
	//
	// Notes:
	//		The maximum error returned is + or - 8.22%.
	//
	//**********************************
	{
		uint32 u4_a = u4FromFloat(tX) & 0x7FFFFFFF;
		uint32 u4_b = u4FromFloat(tY) & 0x7FFFFFFF;
		uint32 u4_c = u4FromFloat(tZ) & 0x7FFFFFFF;

		if (u4_b > u4_a)
			Swap(u4_a, u4_b);
		if (u4_c > u4_b)
		{
			Swap(u4_b, u4_c);
			if (u4_b > u4_a)
				Swap(u4_a, u4_b);
		}

		return fToFloat(u4_a) + 0.3395f * fToFloat(u4_b) + 0.25f * fToFloat(u4_c);
	}

	//******************************************************************************************
	//
	void Normalise() 
	//
	// Turn this vector into a normal, by scaling the magnitude to 1.
	//
	//**********************************
	{
		*this *= tInvLen();
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

		*this *= t_magnitude * tInvLen();
	}

	//******************************************************************************************
	//
	void FastNormalise() 
	//
	// Turn this vector into a normal, by scaling the magnitude to 1.
	//
	//**********************************
	{
		// Calculate the length of the vector using the fast inverse square root function.
		float f_inv_len = fInvSqrtEst(tX * tX + tY * tY + tZ * tZ);

		// Normalize the vector.
		*this *= f_inv_len;
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

	//******************************************************************************************
	//
	void SetMin
	(
		const CVector3<TR>& v3
	)
	//
	// Sets the values of this vector to the minimum of the existing values or the values
	// passed as a parameter.
	//
	//**********************************
	{
		tX = Min(tX, v3.tX);
		tY = Min(tY, v3.tY);
		tZ = Min(tZ, v3.tZ);
	}

	//******************************************************************************************
	//
	void SetMax
	(
		const CVector3<TR>& v3
	)
	//
	// Sets the values of this vector to the maximum of the existing values or the values
	// passed as a parameter.
	//
	//**********************************
	{
		tX = Max(tX, v3.tX);
		tY = Max(tY, v3.tY);
		tZ = Max(tZ, v3.tZ);
	}

	
	//*****************************************************************************************
	char * pcSave(char *pc_buffer) const
	// Same as the CInstance function of the same name...
	{
		*((CVector3<TR>*)pc_buffer) = *this;
		return pc_buffer + sizeof(CVector3<TR>);
	}

	//*****************************************************************************************
	const char * pcLoad(const char* pc_buffer)
	// Same as the CInstance function of the same name...
	{
		*this = *((CVector3<TR>*)pc_buffer);
		return pc_buffer + sizeof(CVector3<TR>);
	}

};


//
// Fuzzy specialisation.
//

//**********************************************************************************************
inline TReal Difference(const CVector3<>& v3_a, const CVector3<>& v3_b)
//
// Specialise the Difference function used by CFuzzy<>.
//
// Note: this version does not allow ordering comparisions, just [in]equality.
//
//**************************************
{
	return Abs(v3_a.tX - v3_b.tX) + 
		   Abs(v3_a.tY - v3_b.tY) + 
		   Abs(v3_a.tZ - v3_b.tZ);
}

inline CFuzzy<CVector3<>, TReal> Fuzzy(const CVector3<>& v3_value, TReal r_tolerance = 0.001)
// Specialise the Fuzzy function for tolerance.
{
	return CFuzzy<CVector3<>, TReal>(v3_value, r_tolerance);
}

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

	forceinline CDir3()
		// The default CDir3 initialises to the Z axis, so that assertion for normalisation doesn't fail.
		: CVector3<TR>(0, 0, 1)
	{
	}

	// Initialise with arbitrary coordinates, optionally normalise.
	forceinline CDir3(TR t_x, TR t_y, TR t_z, bool b_is_normalised = false)
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
	forceinline CDir3(const CVector3<TR>& v3, bool b_is_normalised = false)
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
	forceinline CDir3(const CDir3<TR>& d3)
		: CVector3<TR>(d3)
	{
		Assert(bIsNormalised());
	}

	//******************************************************************************************
	//
	// Assignment operators.
	//

	// Copy the vector, then normalise.
	forceinline CDir3<TR>& operator =(const CVector3<TR>& v3)
	{
		new(this) CDir3<TR>(v3);
		return *this;
	}

	// Copy the direction, then check for normalisation.
	forceinline CDir3<TR>& operator =(const CDir3<TR>& d3)
	{
		new(this) CDir3<TR>(d3);
		return *this;
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Duplicate
	forceinline CDir3<TR> operator -() const
	{
		return CDir3<TR>(-tX, -tY, -tZ, true);
	}

#if 1 || _MSC_VER < 1100
	//
	// Duplicate all versions of a dot product between normal and vector, 
	// to avoid ambiguities with other transform operators (compiler bug re templates).
	//

	friend forceinline TR operator *(const CDir3<TR>& d3_a, const CDir3<TR>& d3_b)
	{
		return (const CVector3<TR>&) d3_a * (const CVector3<TR>&) d3_b;
	}

	friend forceinline TR operator *(const CVector3<TR>& v3_a, const CDir3<TR>& d3_b)
	{
		return v3_a * (const CVector3<TR>&) d3_b;
	}

	friend forceinline TR operator *(const CDir3<TR>& d3_a, const CVector3<TR>& v3_b)
	{
		return (const CVector3<TR>&) d3_a * (const CVector3<TR>&) v3_b;
	}
#endif
};

	//******************************************************************************************
	//
	// Constants.
	//

	// Special vectors.
	extern const CVector2<> v2Zero;
	extern const CVector3<> v3Zero;
	extern const CDir3<> d3XAxis, d3YAxis, d3ZAxis;
	extern const CDir3<> ad3Axes[3];


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

