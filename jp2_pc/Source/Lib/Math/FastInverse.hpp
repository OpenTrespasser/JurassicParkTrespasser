/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		A fast approximation for floating point division (through inversion). A lookup table for
 *		converting integer numbers to their inverse floating point values.
 *
 * Bugs:
 *
 * Notes:
 *		To assist in the understanding of how the inline fast floating point works, a fully
 *		operational, commented 'C' version of the function is included in the implementation
 *		module.
 *
 *		Accuracy of the fast floating point inverse function:
 *
 *			Bits	Max %Error	Table Size (kb)
 *
 *			 16		   0.001	   256
 *			 15		   0.001	   128
 *			 14		   0.003		64
 *			 13		   0.006		32
 *			 12		   0.012		16
 *			 11		   0.024		 8
 *			 10		   0.049		 4
 *			  9		   0.098		 2
 *			  8		   0.195		 1
 *	
 *		The accuracy was tested against the fdiv instruction with 120 million samples spread
 *		through the floating point number range using both positive and negative values.
 *
 * To do:
 *		Remove floating point masks when those in "FloatDef.hpp" work correctly with inline
 *		assembly.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Math/FastInverse.hpp                                              $
 * 
 * 10    98.09.25 5:15p Mmouni
 * Combined positive and negative inverse int tables.
 * 
 * 9     9/25/98 1:51a Pkeet
 * Reverted to an older working version.
 * 
 * 8     98/09/24 23:08 Speter
 * Fixed inverse table implementation so ASM rasteriser won't crash horribly.
 * 
 * 7     9/24/98 5:44p Asouth
 * changed tables to use positive indices only (fixed extern of same)
 * 
 * 6     97/11/15 10:52p Pkeet
 * Added the 'fInverseLow' function.
 * 
 * 5     97.11.11 9:39p Mmouni
 * Inverse table is now only 12-bits, but function does one interation of Newton-Raphson.
 * 
 * 4     97/06/27 2:02p Pkeet
 * Changed size of the fast inverse table.
 * 
 * 3     12/11/96 5:06p Pkeet
 * Increased accuracy of the floating point inverse table.
 * 
 * 2     12/11/96 11:50a Pkeet
 * Assembly version of the fInverse function completed.
 * 
 * 1     12/10/96 3:34p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_MATH_FASTINVERSE_HPP
#define HEADER_LIB_MATH_FASTINVERSE_HPP


//
// Constants and defines.
//

// Number of entries in the inverse integer table.
#define iNUM_ENTRIES_INVERSEINT (128)

// Floating point inverse constants.
#define iINVERSE_TABLE_BITS (12)
#define iINVERSE_TABLE_SIZE (1 << iINVERSE_TABLE_BITS)
#define iSHIFT_MANTISSA     (23 - iINVERSE_TABLE_BITS)

// Define to use one interation of Newton-Raphson.
#define NEWTON_RAPHSON (1)


//
// Floating point manipulation masks. Note that currently these masks replicate definitions
// in "FloatDef.hpp," however the versions in FloatDef don't work correctly with inline
// assembly.
//
#define iFI_MASK_EXPONENT      0x7F800000
#define iFI_MASK_SIGN_EXPONENT 0xFF800000
#define iFI_MASK_MANTISSA      0x007FFFFF
#define iFI_SIGN_EXPONENT_SUB  0x7FFFFFFF


//
// Externally defined global variables.
//

//**********************************************************************************************
//
struct SCombinedInverseTable
//
// Prefix: cit
//
// Structure that combines the positive and negative inverse int tables.
//
//**************************************
{
	float fInverseNegIntTable[iNUM_ENTRIES_INVERSEINT];
	float fInversePosIntTable[iNUM_ENTRIES_INVERSEINT];
};

// The lookup tables for integer to inverse floating point.
extern SCombinedInverseTable citInverseIntTable;

// A #define so that legacy code will work.
#define fInverseIntTable citInverseIntTable.fInversePosIntTable

// The lookup table for fast floating point inverses.
extern int32 i4InverseMantissa[iINVERSE_TABLE_SIZE];


//
// Global functions.
//

//**********************************************************************************************
//
inline float fInverse
(
	float f	// Floating point value to find the inverse for.
)
//
// Returns an approximation of the inverse of the floating point parameter.
//
//**************************************
{
	float r;
	float fTWO = 2.0f;

	__asm
	{
		mov		ebx,dword ptr[f]
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

#if (NEWTON_RAPHSON)
		// One iteration of Newton-Raphson doubles the number of bits of accuary.
		fld		[f]

		add		eax,dword ptr[i4InverseMantissa + ebx*4]

		mov		dword ptr[r],eax

		fmul	[r]							// (v*r)
		
		fsubr	[fTWO]						// (2.0 - v*r)

		fmul	[r]							// (2.0 - v*r)*r

		fstp	[r]
#else
		add		eax,dword ptr[i4InverseMantissa + ebx*4]
	
		mov		dword ptr[r],eax
#endif
	}

	return r;
}

//**********************************************************************************************
//
inline float fInverseLow
(
	float f	// Floating point value to find the inverse for.
)
//
// Returns an approximation of the inverse of the floating point parameter.
//
//**************************************
{
	__asm
	{
		mov ebx, dword ptr[f]
		mov eax, iFI_SIGN_EXPONENT_SUB

		and ebx, iFI_MASK_MANTISSA
		sub eax, dword ptr[f]

		sar ebx, iSHIFT_MANTISSA
		and eax, iFI_MASK_SIGN_EXPONENT

		add eax, dword ptr[i4InverseMantissa + ebx*4]

		mov dword ptr[f], eax
	}

	return f;
}

//**********************************************************************************************
//
inline float fUnsignedInverseInt
(
	int i	// Integer to find the floating point inverse value for.
)
//
// Returns the unsigned floating point inverse of a signed integer value.
//
//**************************************
{
	Assert(i != 0);
	Assert(i > -iNUM_ENTRIES_INVERSEINT);
	Assert(i < iNUM_ENTRIES_INVERSEINT);

	return fInverseIntTable[i];
}


#endif
