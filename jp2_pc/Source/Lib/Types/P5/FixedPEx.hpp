/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Additional header file for the fixed point type class.
 *		Implements the overloaded operators and member functions that require inline assembly.
 *
 *		This file is included by the 'FixedP.hpp' header file. NEVER include this file directly!
 *
 * Bugs:
 *
 * To do:
 *		Check all the code for optimal Pentium pipeline utilisation.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Types/P5/FixedPEx.hpp                                             $
 * 
 * 6     98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 5     8/25/98 4:07p Rvande
 * added global scope qualifier to references to the fixed type
 * 
 * 4     12/20/96 6:28p Mlange
 * Surrounded all __asm blocks with an #if conditional because they are Microsoft specific.
 * 
 * 3     14-05-96 8:27p Mlange
 * Changes from code review: All functions in this header file are now implemented as global
 * functions to avoid the use of temporary local variables. Prefixed function names with the
 * Hungarian return type. Removed dependancy on the fixed point decimal position of 16.
 * Reordered instructions for better pipeline utilisation. Added assert checking for divisions
 * by zero.
 * 
 * 2     5/05/96 2:08p Mlange
 * Updated for change in default behaviour to truncation.
 * 
 * 1     4/30/96 10:33a Mlange
 * Additional inline assembly overloaded operators and member functions for the fixed point type
 * class.
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_TYPES_P5_FIXEDPEX_HPP
#define HEADER_LIB_TYPES_P5_FIXEDPEX_HPP

// Make sure this file is being included from 'FixedP.hpp'.
#ifndef HEADER_LIB_TYPES_FIXEDP_HPP
	#error The header file FixedP.hpp has not been included!
#endif


//**********************************************************************************************
//
// Overloaded operator functions for class fixed.
//

// Multiplication operator.
forceinline  ::fixed operator *(::fixed fx_a, ::fixed fx_b)
{
	::fixed fx_temp;

	#if defined(_MSC_VER)
		__asm
		{
			mov		eax, fx_a.i4Fx
			imul	fx_b.i4Fx						// This results in a 64 bit fixed point value with 32 integer and 32 fractional bits.
		
			shrd	eax, edx, iFIXED_PT_POSITION	// Shift back down to the 32 bit fixed point (overflows are not detected).
			mov		fx_temp.i4Fx, eax
		}

	#else
		Assert(false);

	#endif

	return fx_temp;
}


// Division operator.
forceinline  ::fixed operator /(::fixed fx_numerator, ::fixed fx_denominator)
{
	// Trap divisions by zero.
	Assert(fx_denominator.i4Fx != 0);

	::fixed fx_temp;

	#if defined(_MSC_VER)
		__asm
		{
			mov		eax, fx_numerator.i4Fx			// Multiply the numerator by the fixed point multiplier and sign extend it to 64 bits.
			mov		edx, eax
			shl		eax, iFIXED_PT_POSITION
			sar		edx, 32 - iFIXED_PT_POSITION

			idiv	fx_denominator.i4Fx

			mov		fx_temp.i4Fx, eax
		}

	#else
		Assert(false);

	#endif

	return fx_temp;
}




//**********************************************************************************************
//
// Global functions for class fixed.
//


//******************************************************************************************
//
forceinline  ::fixed fxMulR
(
	::fixed fx_a,
	::fixed fx_b
)
//
// Rounded fixed point multiplication.
//
// Returns:
//		A fixed point value that is the result of multiplying 'fx_a' by 'fx_b'. The result
//		is rounded to the nearest fixed point representation.
//
// Notes:
//		Because it performs rounding, this function executes (slightly) slower than the
//		overloaded multiplication operator.
//
//**************************************
{
	::fixed fx_temp;

	#if defined(_MSC_VER)
		__asm
		{
			mov		eax, fx_a.i4Fx
			imul	fx_b.i4Fx						// This results in a 64 bit fixed point value with 32 integer and 32 fractional bits.
		
			shrd	eax, edx, iFIXED_PT_POSITION	// Shift back down to the 32 bit fixed point (overflows are not detected).
			adc		eax, 0							// If the last bit shifted out was set, we need to add one for rounding.

			mov		fx_temp.i4Fx, eax
		}

	#else
		Assert(false);

	#endif

	return fx_temp;
}


//******************************************************************************************
//
forceinline  ::fixed fxDivR
(
	::fixed fx_numerator,
	::fixed fx_denominator
)
//
// Rounded fixed point division.
//
// Returns:
//		A fixed point value that is the result of dividing 'fx_numerator' by
//		'fx_denominator'. The result is rounded to the nearest fixed point representation.
//
// Notes:
//		Because it performs rounding, this function executes (slightly) slower than the
//		overloaded division operator.
//
//**************************************
{
	// Trap divisions by zero.
	Assert(fx_denominator.i4Fx != 0);

	::fixed fx_temp;

	#if defined(_MSC_VER)
		__asm
		{
			//
			// Fixed point division is represented by:
			//
			//         n * m^2
			// q * m = -------, where m is the fixed point multiplier.
			//          d * m
			//
			// To round to the nearest fixed point value we need to add 1/2 to the quotient when it
			// is positive, or subtract 1/2 when it is negative, and then truncate. This is equivalent
			// to adding or subtracting d/2 to the numerator before the division and then truncating.
			//

			mov		eax, fx_numerator.i4Fx			// Multiply the numerator by the fixed point multiplier and sign extend it to 64 bits.
			mov		edx, eax
			shl		eax, iFIXED_PT_POSITION
			sar		edx, 32 - iFIXED_PT_POSITION

			mov		ebx, fx_denominator.i4Fx
			sar		ebx, 1

			mov		ecx, ebx
			xor		ecx, edx						// Bit 31 is set if the signs of the numerator and denominator are different.
			sar		ecx, 31							// 0 if the signs are the same, -1 if they are different.
			xor		ebx, ecx
			sub		ebx, ecx						// denominator / 2 if the signs are the same, -denominator / 2 if they are different.

			mov		ecx, ebx						// Sign extend denominator / 2 to a 64 bit quantity into ecx:ebx.
			sar		ecx, 31
			add		eax, ebx						// Add it to the 64 bit numerator.
			adc		edx, ecx

			idiv	fx_denominator.i4Fx

			mov		fx_temp.i4Fx, eax
		}

	#else
		Assert(false);

	#endif

	return fx_temp;
}



//******************************************************************************************
//
forceinline  ::fixed fxMulDiv
(
	::fixed fx_multiplier_a,
	::fixed fx_multiplier_b,
	::fixed fx_denominator
)
//
// Fixed point multiply-divide.
//
// Returns:
//		A fixed point value that is the result of multiplying 'fx_multiplier_a' by
//		'fx_multiplier_b' and then dividing it by 'fx_denominator'. The result is truncated
//		to the nearest fixed point representation.
//
//**************************************
{
	// Trap divisions by zero.
	Assert(fx_denominator.i4Fx != 0);

	::fixed fx_temp;

	#if defined(_MSC_VER)
		__asm
		{
			mov		eax, fx_multiplier_a.i4Fx
			imul	fx_multiplier_b.i4Fx			// This results in a 64 bit fixed point numerator for the division.

			idiv	fx_denominator.i4Fx

			mov		fx_temp.i4Fx, eax
		}

	#else
		Assert(false);

	#endif

	return fx_temp;
}



//******************************************************************************************
//
forceinline  ::fixed fxMulDivR
(
	::fixed fx_multiplier_a,
	::fixed fx_multiplier_b,
	::fixed fx_denominator
)
//
// Rounded fixed point multiply-divide.
//
// Returns:
//		A fixed point value that is the result of multiplying 'fx_multiplier_a' by
//		'fx_multiplier_b' and then dividing it by 'fx_denominator'. The result is rounded
//		to the nearest fixed point representation.
//
// Notes:
//		Because it performs rounding, this member function executes (slightly) slower than
//		the one that truncates. See above.
//
//**************************************
{
	// Trap divisions by zero.
	Assert(fx_denominator.i4Fx != 0);

	::fixed fx_temp;

	#if defined(_MSC_VER)
		__asm
		{
			mov		eax, fx_multiplier_a.i4Fx
			imul	fx_multiplier_b.i4Fx			// This results in a 64 bit fixed point numerator for the division.

			mov		ebx, fx_denominator.i4Fx
			sar		ebx, 1

			mov		ecx, ebx
			xor		ecx, edx						// Bit 31 is set if the signs of the numerator and denominator are different.
			sar		ecx, 31							// 0 if the signs are the same, -1 if they are different.
			xor		ebx, ecx
			sub		ebx, ecx						// denominator / 2 if the signs are the same, -denominator / 2 if they are different.

			mov		ecx, ebx						// Sign extend denominator / 2 to a 64 bit quantity into ecx:ebx.
			sar		ecx, 31
			add		eax, ebx						// Add it to the 64 bit numerator.
			adc		edx, ecx

			idiv	fx_denominator.i4Fx

			mov		fx_temp.i4Fx, eax
		}

	#else
		Assert(false);

	#endif

	return fx_temp;
}


#endif
