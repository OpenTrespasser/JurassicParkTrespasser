/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Definition of the fixed point type class. The fixed point type is a signed 32 bit
 *		quantity with 16 bit of integer and 16 bit of fractional precision.
 *
 *		See also the file: Lib/Types/P5/FixedPEx.hpp.
 *
 * Bugs:
 *
 * To do:
 *		Overload the multiply and divide operators for mixed integer and fixed point operations.
 *		Add member functions for multiply-accumulate operations if and when they are required.
 *      Make the class a template. Make assembly versions of 'FromPosFloat' and 'FromFloat.'
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Types/FixedP.hpp                                                  $
 * 
 * 31    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 30    8/25/98 4:07p Rvande
 * added global-scope qualifier to fixed type
 * 
 * 29    8/25/98 2:39p Rvande
 * added global-scope qualifier to fixed data type
 * 
 * 28    8/16/98 8:38p Mmouni
 * Fixed error in bIntEquals().
 * 
 * 27    6/26/97 9:59a Mlange
 * Fixed point assignment from double now uses the (fast) float to int conversion function.
 * 
 * 26    6/17/97 5:27p Mlange
 * Assembly versions of fixed::fxFromPosFloat() and fixed::fxFromFloat().
 * 
 * 25    6/16/97 8:33p Mlange
 * The named member functions for initialisation with a floating point value now use the fast
 * float to integer conversions. They now also return a reference to (modified) this. Removed
 * the fixed::FromDouble() function. It was never used. Removed the fixed::Round() function. It
 * was never used and very slow. Removed the now redundant fFixedMultiplier constant.
 * 
 * 24    6/15/97 4:55p Rwyatt
 * Added a macro so the assembler can use fixed point
 * 
 * 23    97/05/30 3:04p Pkeet
 * Now uses explicit fpu asm conversions.
 * 
 * 22    97/05/21 17:29 Speter
 * Commented out Fuzzy capability for fixed, due to new Fuzzy implementation.  Will be restored
 * if every needed.
 * 
 * 21    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 20    96/09/16 11:56 Speter
 * Removed bFuzzyEquals, replaced with specialisations for CFuzzy.
 * 
 * 19    96/06/20 6:24p Mlange
 * Updated for changes to CIntFloat.
 * 
 * 18    6/18/96 10:42a Pkeet
 * Added 'FromPosFloat' and 'FromFloat' functions. Deleted 'FromPosDouble' and 'FromDouble'
 * functions
 * 
 * 17    6/13/96 10:26a Pkeet
 * Gave up on trying to make 'FromDouble' work and changed it to an interface only.
 * 
 * 16    6/13/96 10:00a Pkeet
 * Fixed bug in 'FromDouble.'
 * 
 * 15    6/13/96 9:58a Pkeet
 * Added 'FromDouble' interface. Added assembly work to the 'to do' list for fast conversion
 * operations.
 * 
 * 14    5/29/96 4:35p Pkeet
 * Added 'dDOUBLE_TO_FIXED_TRUNC' for 'FromPosDouble' member function.
 * 
 * 13    96/05/23 16:07 Speter
 * Added conversion to float to avoid ambiguity errors.
 * 
 * 12    5/21/96 9:40a Pkeet
 * Made the default assignment operator that converted from doubles truncate, and added a member
 * function that would round.
 * 
 * 11    5/17/96 9:50a Pkeet
 * Made data member public.
 * 
 * 10    14-05-96 8:26p Mlange
 * Added a member function for fast conversion from double floating point to fixed point.
 * Rewrote bIntEquals as a C++ function instead of inline assembly. Renamed the fuzzy equality
 * function and made it a non-member function. Removed casts from the uses of BitMask and
 * BitMaskRange where they are no longer needed. Changes from code review: Updated some
 * comments. The negation operator now asserts for taking the negative of the negative minimum.
 * Added some spaces around operators. Added the Hungarian prefix to member functions where
 * applicable. The bValueInFixedRange() member function is now excluded from the release build.
 * 
 * 9     5/13/96 5:59p Pkeet
 * Added 'bIntEquals' function.
 * 
 * 8     5/08/96 7:45p Mlange
 * Updated to do list.
 * 
 * 7     5/06/96 2:04p Mlange
 * All parameters of all uses of BitMaskRange are now cast to int32 to avoid compiler errors.
 * 
 * 6     5/05/96 2:07p Mlange
 * Removed #include for StdLibEx.hpp. Added overloaded /= operator. Changes from code review.
 * Removed Abs member function because the template Abs function operates on the fixed point
 * type anyway.  The const keyword for constant member functions is now on the same line as the
 * closing parenthesis. All the overloaded operators are now implemented as member functions
 * instead of friend global functions. Declared overloaded operator functions const where
 * applicable. Changed the default behaviour of the overloaded operators to truncation. Named
 * member functions must now be used to implement rounding. Added additional Assert parameter
 * checking. Updated some comments.
 * 
 * 5     5/03/96 11:45a Pkeet
 * Added overloaded *= operator.
 * 
 * 4     5/01/96 7:51p Mlange
 * Updated the 'to do' list.
 * 
 * 3     5/01/96 2:40p Mlange
 * Now includes <function.h> which contains inline functions that define all equality and
 * relational operators in terms of == and <. Removed the now redundant overloaded !=, >, <= and
 * >= operators.
 * 
 * 2     5/01/96 9:54a Mlange
 * Added some comments. Updated the 'to do' list.
 * 
 * 1     4/30/96 10:32a Mlange
 * Fixed point type class.
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_TYPES_FIXEDP_HPP
#define HEADER_LIB_TYPES_FIXEDP_HPP

#include <functional>
#include "Lib/Math/FloatDef.hpp"


//**********************************************************************************************
//
// Constants for class fixed.
//

// The size of the fixed point type and the position of the decimal point.
#define iFIXED_SIZE_IN_BITS		32
#define iFIXED_PT_POSITION		16

// a macro for the inline assembler so we do not have to use the above defines
// we cannot use the define in this macro because the assembler thinks it is a variable, and gives
// a non constant shift error
#define FIXED(x) (x << 16)

// The default tolerance for a 'fuzzy' compare between two fixed point types.
#define fxTOLERANCE_DEFAULT		((::fixed)1e-4)

//
// The maximum and minimum values that define the numerical range of the fixed point type.
// Note that these constants define a half-open interval, e.g. the fixed point type can hold a
// value up to, but not including, the maximum defined below.
//
#define dFIXED_MAX_OPEN		    ((double)BitMaskRange(0, iFIXED_SIZE_IN_BITS - iFIXED_PT_POSITION - 1))
#define dFIXED_MIN			    (-dFIXED_MAX_OPEN)

// Multiplication factor for floating point representations of the fixed point type.
#define dFIXED_PT_MULTIPLIER	((double)BitMask(iFIXED_PT_POSITION))
#define fFIXED_PT_MULTIPLIER	((float)BitMask(iFIXED_PT_POSITION))


//**********************************************************************************************
//
class fixed
//
// Definition of the fixed point type and associated overloaded operators and member functions.
//
// Notes:
//		This class is intended as an extension of the family of built-in types. It therefore
//		does not follow the naming rules of conventional classes (e.g. CFixed).
//
//		Where applicable, the overloaded operators truncate the result to the nearest fixed
//		point representation by default. Named member functions are provided that perform the
//		same operations, but round to the nearest fixed point value instead.
//
// Prefix: fx
//
//**************************************
{

public:
	int32 i4Fx;		// The current value of the fixed point type.


public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// The default constructor.
	forceinline fixed()
	{
	}

	//
	// Constructor for initialising with a floating point value.
	// Note that this (implicitly) uses the overloaded assignment operator defined below.
	//
	forceinline fixed(double d)
	{
		*this = d;
	}
	
	//
	// Constructor for initialising with an integer value.
	// Note that this (implicitly) uses the overloaded assignment operator defined below.
	//
	forceinline fixed(int i)
	{
		*this = i;
	}
	

	//******************************************************************************************
	//
	// Assignment operators.
	//
	forceinline fixed& operator =(double d)
	{
		Assert(bValueInFixedRange(d));

		// Initialise with the nearest possible fixed point representation of the floating point value.
		i4Fx = iFloatCast(float(d * dFIXED_PT_MULTIPLIER));
		return *this;
	}

	forceinline fixed& operator =(int i)
	{
		Assert(bValueInFixedRange((double)i));

		i4Fx = (int32)i << iFIXED_PT_POSITION;
		return *this;
	}


	//******************************************************************************************
	//
	// Conversion operators.
	//

	//
	// Note: both the assignment and conversion operators are defined for the fixed point class.
	// This will cause ambiguities when using mixed types with binary operators. For example:
	// fx_a + 2.5. In this case the compiler cannot determine whether the double should be
	// converted to a fixed or the fixed to a double. This is easily resolved by explicitly
	// specifing the conversion: fx_a + (fixed)2.5 for example.
	//
	// These conflicts could have been avoided by not overloading the conversion operators, but
	// instead defining named member functions for the conversions. However, it was observed
	// that the compiler generates far inferior code when relying on implicit conversions in
	// binary operations. Explicity specifying conversions gives the compiler additional hints
	// for optimisation. Thus, even though having to specify the explicit conversion in
	// expressions with mixed types may seem a nuisance, it forces the user to add these
	// additional optimisation hints.
	//

	forceinline operator double() const
	{
		return (double)i4Fx / dFIXED_PT_MULTIPLIER;
	}

	forceinline operator float() const
	{
		return (float) ( (double)i4Fx / dFIXED_PT_MULTIPLIER );
	}

	forceinline operator int() const
	{
		// Truncate the fixed point value to the nearest integer.
		return (int)(i4Fx >> iFIXED_PT_POSITION);
	}
	

	//******************************************************************************************
	//
	// Operators.
	//

	//
	// Relational and equality operators.
	// Note that all the other relational operators (!=, >, etc.) are defined in terms of the
	// equality and less than operators by <function.h>.
	//

	// Relational and equality operators.
	forceinline bool operator ==(fixed fx) const
	{
		return i4Fx == fx.i4Fx;
	}


	forceinline bool operator <(fixed fx) const
	{
		return i4Fx < fx.i4Fx;
	}


	// Shift operators.
	forceinline fixed operator <<(int i) const
	{
		Assert(i >= 0 && i < iFIXED_SIZE_IN_BITS);

		fixed fx_temp;

		fx_temp.i4Fx = i4Fx << i;
		return fx_temp;
	}


	forceinline fixed operator >>(int i) const
	{
		Assert(i >= 0 && i < iFIXED_SIZE_IN_BITS);

		fixed fx_temp;

		fx_temp.i4Fx = i4Fx >> i;
		return fx_temp;
	}


	forceinline fixed& operator <<=(int i)
	{
		Assert(i >= 0 && i < iFIXED_SIZE_IN_BITS);

		i4Fx <<= i;
		
		return *this;
	}


	forceinline fixed& operator >>=(int i)
	{
		Assert(i >= 0 && i < iFIXED_SIZE_IN_BITS);

		i4Fx >>= i;
		
		return *this;
	}


	// Negation operator.
	forceinline fixed operator -() const
	{
		// The negative of the negative minimum remains negative.
		Assert(i4Fx != BitMask(iFIXED_SIZE_IN_BITS - 1));

		fixed fx_temp;

		fx_temp.i4Fx = -i4Fx;
		return fx_temp;
	}


	// Additive operators.
	forceinline fixed operator +(fixed fx) const
	{
		fixed fx_temp;

		// Note that the (already overloaded) += operator is not used to implement this because it generates inferior code.
		fx_temp.i4Fx = i4Fx + fx.i4Fx;
		return fx_temp;
	}


	forceinline fixed operator -(fixed fx) const
	{
		fixed fx_temp;

		// Note that the (already overloaded) -= operator is not used to implement this because it generates inferior code.
		fx_temp.i4Fx = i4Fx - fx.i4Fx;
		return fx_temp;
	}


	// Additive assignment operators.
	forceinline fixed& operator +=(fixed fx)
	{
		i4Fx += fx.i4Fx;
		
		return *this;
	}

	forceinline fixed& operator -=(fixed fx)
	{
		i4Fx -= fx.i4Fx;
		
		return *this;
	}


	// Multiplication operator. The result is truncated.
	friend fixed operator *(fixed fx_a, fixed fx_b);

	// Multiplication assignment operator. The result is truncated.
	forceinline fixed& operator *=(fixed fx)
	{
		*this = *this * fx;

		return *this;
	}

	// Division operator. The result is truncated.
	friend fixed operator /(fixed fx_numerator, fixed fx_denominator);

	// Division assignment operator. The result is truncated.
	forceinline fixed& operator /=(fixed fx)
	{
		*this = *this / fx;

		return *this;
	}



	//******************************************************************************************
	//
	// Member functions.
	//


	//******************************************************************************************
	//
	forceinline fixed fxShiftRightR
	(
		int i
	) const
	//
	// Rounded fixed point shift right.
	//
	// Returns:
	//		This fixed point value shifted right by the given amount.
	//
	// Notes:
	//		Because it performs rounding, this member function executes (slightly) slower than
	//		the overloaded operator that truncates. See above.
	//
	//**************************************
	{
		Assert(i >= 1);

		fixed fx_temp;

		fx_temp.i4Fx = (i4Fx + BitMask((int32)(i - 1))) >> i;
		return fx_temp;
	}



	//******************************************************************************************
	//
	forceinline int iToIntR
	(
	) const
	//
	// Rounded fixed point to integer conversion.
	//
	// Returns:
	//		This fixed point value rounded to the nearest integer representation.
	//
	//**************************************
	{
		return (int)((i4Fx + BitMask((int32)iFIXED_PT_POSITION - 1)) >> iFIXED_PT_POSITION);
	}


	//******************************************************************************************
	//
	forceinline fixed& fxFromPosFloat
	(
		float f
	)
	//
	// Fast initialisation of this fixed point with a positive truncated floating point value.
	//
	// Returns:
	//		Reference to this fixed point.
	//
	//**************************************
	{
		Assert(bValueInFixedRange(f) && f >= 0);

#if VER_ASM
		int32 i4_temp_ret;

		__asm mov eax, f
		__asm add eax, iFIXED_PT_POSITION << iFLOAT_EXP_BIT_START

		ASM_POS_FLOAT_CAST

		__asm mov i4_temp_ret, eax

		i4Fx = i4_temp_ret;
#else

		CIntFloat if_val(f);

		// Adjust the floating point's exponent for the position of the fixed point. Essentialy,
		// we multiply the floating point value by 'dFIXED_PT_MULTIPLIER' here.
		if_val.i4Int += iFIXED_PT_POSITION << iFLOAT_EXP_BIT_START;

		// And then convert to integer.
		i4Fx = if_val.u4CastPos();
#endif

		Assert(i4Fx == uint32(f * dFIXED_PT_MULTIPLIER));

		return *this;
	}


	//******************************************************************************************
	//
	forceinline fixed& fxFromFloat
	(
		float f
	)
	//
	// Fast initialisation of this fixed point with a truncated floating point value.
	//
	// Returns:
	//		Reference to this fixed point.
	//
	//**************************************
	{
		Assert(bValueInFixedRange(f));

#if VER_ASM
		int32 i4_temp_ret;

		__asm mov eax, f
		__asm add eax, iFIXED_PT_POSITION << iFLOAT_EXP_BIT_START

		ASM_FLOAT_CAST

		__asm mov i4_temp_ret, eax

		i4Fx = i4_temp_ret;
#else

		CIntFloat if_val(f);

		// Adjust the floating point's exponent for the position of the fixed point. Essentialy,
		// we multiply the floating point value by 'dFIXED_PT_MULTIPLIER' here.
		if_val.i4Int += iFIXED_PT_POSITION << iFLOAT_EXP_BIT_START;

		// And then convert to integer.
		i4Fx = if_val.i4Cast();
#endif

		Assert(i4Fx == int32(f * dFIXED_PT_MULTIPLIER));

		return *this;
	}


	//******************************************************************************************
	//
	forceinline fixed fxWhole
	(
	) const
	//
	// Get the integer part of this fixed point value.
	//
	// Returns:
	//		The integer part of this fixed point value expressed as a fixed point value also.
	//
	//**************************************
	{
		fixed fx_temp;

		if (i4Fx >= 0)
			fx_temp.i4Fx =    i4Fx & BitMaskRange((int32)iFIXED_PT_POSITION, (int32)(iFIXED_SIZE_IN_BITS - iFIXED_PT_POSITION));
		else
			fx_temp.i4Fx = -(-i4Fx & BitMaskRange((int32)iFIXED_PT_POSITION, (int32)(iFIXED_SIZE_IN_BITS - iFIXED_PT_POSITION)));

		return fx_temp;
	}


	//******************************************************************************************
	//
	forceinline fixed fxFractional
	(
	) const
	//
	// Get the fractional part of this fixed point value.
	//
	// Returns:
	//		The fractional part of this fixed point value expressed as a fixed point value also.
	//
	//**************************************
	{
		fixed fx_temp;

		if (i4Fx >= 0)
			fx_temp.i4Fx =    i4Fx & BitMaskRange((int32)0, (int32)iFIXED_PT_POSITION);
		else
			fx_temp.i4Fx = -(-i4Fx & BitMaskRange((int32)0, (int32)iFIXED_PT_POSITION));

		return fx_temp;
	}



	//******************************************************************************************
	//
	// Friend global functions.
	//
	friend inline fixed fxMulR(fixed fx_a, fixed fx_b);
	friend inline fixed fxDivR(fixed fx_numerator, fixed fx_denominator);
	friend inline fixed fxMulDiv( fixed fx_multiplier_a, fixed fx_multiplier_b, fixed fx_denominator);
	friend inline fixed fxMulDivR(fixed fx_multiplier_a, fixed fx_multiplier_b, fixed fx_denominator);

	friend inline bool bIntEquals(fixed fx_a, fixed fx_b);




private:
	//******************************************************************************************
	//
	// Member functions.
	//

#if VER_DEBUG

	//******************************************************************************************
	//
	static bool bValueInFixedRange
	(
		double d
	)
	//
	// Determine if a floating point value can be represented within the fixed point numerical
	// range.
	//
	// Returns:
	//		'true' if the floating point value can be represented in the fixed point type,
	//		'false' otherwise.
	//
	// Notes:
	//		This function is used for debugging purposes only. It is not included in the release
	//		build.
	//
	//**************************************
	{
		return (d >= dFIXED_MIN && d < dFIXED_MAX_OPEN);
	}

//#if VER_DEBUG
#endif

};





//**********************************************************************************************
//
// Global functions for class fixed.
//

//******************************************************************************************
//
inline bool bFuzzyEquals
(
	::fixed fx_a, ::fixed fx_b,						// The fixed point values to compare.
	::fixed fx_tolerance = fxTOLERANCE_DEFAULT	// The tolerance for equality.
)
//
// Equality comparision with tolerance.
//
// Returns:
//		'true' if the fixed point values are equal to within some range, 'false' otherwise.
//
//**************************************
{
	// The casts to an unsigned type are to prevent errors when the difference is exactly 0x80000000.
	return (uint32)Abs(fx_a.i4Fx - fx_b.i4Fx) <= (uint32)fx_tolerance.i4Fx;
}


//******************************************************************************************
//
// Class CFuzzy specialisation.
//

/*
	int32 CFuzzy<fixed>::operator ==(fixed t)
	{
		// We specialise the comparisons for fixed point in order to cast to an unsigned type.
		// This prevents errors when the difference is exactly 0x80000000.
		return (uint32)Abs(tValue.i4Fx - t.i4Fx) <= (uint32)tTolerance.i4Fx;
	}

	//
	// A specialised version of Fuzzy for fixed, which has its own default tolerances.
	//

	inline CFuzzy<fixed> Fuzzy(fixed fx_value, fixed fx_tolerance = fxTOLERANCE_DEFAULT)
	{
		return CFuzzy<fixed>(fx_value, fx_tolerance);
	}
*/
//******************************************************************************************
//
inline bool bIntEquals
(
	::fixed fx_a,
	::fixed fx_b
)
//
// Compares the integer components of two fixed point numbers. Equivalent to:
//
//		return (int)fx_a == (int)fx_b;
//
// Returns:
//		Boolean result of the comparison.
//
//**************************************
{
	return (fx_a.i4Fx & BitMaskRange((int32)iFIXED_PT_POSITION, (int32)(iFIXED_SIZE_IN_BITS - iFIXED_PT_POSITION))) ==
	       (fx_b.i4Fx & BitMaskRange((int32)iFIXED_PT_POSITION, (int32)(iFIXED_SIZE_IN_BITS - iFIXED_PT_POSITION)));
}
	


// Include the inline assembly versions of overloaded operators and global functions.
#include "Lib/Types/P5/FixedPEx.hpp"



// Undefine constants that are no longer needed.
#undef iFIXED_SIZE_IN_BITS
#undef iFIXED_PT_POSITION
#undef dFIXED_PT_MULTIPLIER
#undef dFIXED_MAX_OPEN
#undef dFIXED_MIN


#endif
