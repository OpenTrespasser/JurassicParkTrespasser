/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of FastInverse.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Math/FastInverse.cpp                                              $
 * 
 * 7     98.09.25 5:15p Mmouni
 * Combined positive and negative inverse int tables.
 * 
 * 6     9/25/98 1:51a Pkeet
 * Reverted to an older working version.
 * 
 * 5     98/09/24 23:09 Speter
 * Fixed inverse table implementation so ASM rasteriser won't crash horribly.
 * 
 * 4     9/24/98 5:44p Asouth
 * fixed dependency on in-order storage of array tables
 * 
 * 3     1/09/97 8:07p Pkeet
 * Changed 'true' to 1 and 'false' to 0 for #defines used in #if statements.
 * 
 * 2     12/11/96 11:50a Pkeet
 * Assembly version of the fInverse function completed.
 * 
 * 1     12/10/96 3:34p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/


//
// Includes.
//
#include "GblInc/Common.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "FastInverse.hpp"


//
// Module specific defines.
//

// Switch to automatically run a test on the inverse math at startup.
#define bRUN_INVERSE_TEST 0


//
// Additional includes.
//
#if bRUN_INVERSE_TEST
	#include "Lib/Sys/TextOut.hpp"
#endif


//
// Module specific functions.
//

//**********************************************************************************************
//
inline void GreatestDiffRatio
(
	float& f_greatest_diff_ratio,
	float  f_approximation,
	float  f_target
)
//
// Returns an approximation of the inverse of the floating point parameter.
//
//**************************************
{
	Assert(Sign(f_approximation) == Sign(f_target));

	if (f_target == 0.0f)
		return;

	f_greatest_diff_ratio = Max
	(
		f_greatest_diff_ratio,
		Abs(Abs(f_approximation / f_target) - 1.0f)
	);
}


//
// Class definitions.
//

//**********************************************************************************************
//
class CInitInverseTables
//
// Class for initialising the inverse tables.
//
// Prefix: inv
//
// Notes:
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructor.
	//

	// The default constructor.
	CInitInverseTables()
	{
		InitializeInverseIntTable();
		InitializeInverseTable();
		TestInverse();
	}

private:

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void InitializeInverseIntTable
	(
	)
	//
	// Initializes the inverse integer table.
	//
	//**************************************
	{
		for (int i_int = -iNUM_ENTRIES_INVERSEINT + 1; i_int < iNUM_ENTRIES_INVERSEINT; i_int++)
		{
			// Find the floating point inverse of the target integer.
			float f = (i_int == 0) ? (1.0f) : (1.0f / float(Abs(i_int)));

			// Put the floating point value in a lookup table.
			fInverseIntTable[i_int] = f;
		}
	}

	//******************************************************************************************
	//
	void InitializeInverseTable
	(
	)
	//
	// Initializes the floating point inverse table.
	//
	//**************************************
	{
		const int32 i4_one      = 127 << iFLOAT_EXP_BIT_START;
		const int32 i4_one_plus = 128 << iFLOAT_EXP_BIT_START;
		CIntFloat   if_entry;

		for (int i_mantissa = 0; i_mantissa < iINVERSE_TABLE_SIZE; i_mantissa++)
		{
			// Make a floating point value from the mantissa.
			if_entry.i4Int = (i_mantissa << iSHIFT_MANTISSA) | i4_one;

			// Add a half for greater accuracy.
			if_entry.i4Int += 1 << (iSHIFT_MANTISSA - 1);

			// Invert the floating point value.
			if_entry.fFloat = 1.0f / if_entry.fFloat;
 
			// Insert the mantissa in the lookup table.
			i4InverseMantissa[i_mantissa] = if_entry.i4Int - i4_one_plus;
		}
	}

	//******************************************************************************************
	//
	void TestInverse
	(
	)
	//
	// Tests the fast floating point inverse function.
	//
	//**************************************
	{
#if bRUN_INVERSE_TEST
		const float f_begin = float(1.0e-34);
		const float f_end   = float(1.0e35);
		const int   i_step  = 1 << 8;

		CIntFloat if_entry;
		float     f_greatest_diff_ratio;
		int       i_count;

		CConsoleBuffer con(120, 40);

		// Open dump file.
		con.OpenFileSession("Inverse.txt");
		con.Print("Starting inverse test...\n\n");

		// Set initial parameters.
		if_entry.fFloat       = f_begin;
		f_greatest_diff_ratio = 0.0f;
		i_count               = 0;

		// Perform positive floating point divide test.
		do
		{
			GreatestDiffRatio
			(
				f_greatest_diff_ratio,
				fInverse(if_entry.fFloat),
				1.0f / if_entry.fFloat
			);
			if_entry.i4Int += i_step;
			i_count++;
		}
		while (if_entry.fFloat < f_end);

		// Perform negative floating point divide test.
		if_entry.fFloat = -f_end;
		do
		{
			GreatestDiffRatio
			(
				f_greatest_diff_ratio,
				fInverse(if_entry.fFloat),
				1.0f / if_entry.fFloat
			);
			if_entry.i4Int -= i_step;
			i_count++;
		}
		while (if_entry.fFloat < -f_begin);

		con.Print("Number of tests (x1000)           : %ld\n",   i_count / 1000);
		con.Print("Maximum percent difference        : %1.3f\n", f_greatest_diff_ratio * 100.0f);
		con.Print("Inverse of 1.0                    : %e\n",    fInverse(1.0f));
		con.Print("Inverse of -1.0                   : %e\n",    fInverse(-1.0f));
		con.Print("Maximum percent difference of 1.0 : %1.3f\n", Abs(1.0f - fInverse(1.0f)) * 100.0f);
		con.Print("Table size (kb)                   : %ld\n",   ((iNUM_ENTRIES_INVERSEINT * 2 + iINVERSE_TABLE_SIZE) * 4) >> 10);

		// Close dump file.
		con.Print("\nFinished inverse test!");
		con.CloseFileSession();
#endif
	}

};


//
// Module variables.
//

// The lookup tables for Inverse int. Note: these two tables must be together!
SCombinedInverseTable citInverseIntTable;

// The lookup table for the fast floating point inverse.
int32 i4InverseMantissa[iINVERSE_TABLE_SIZE];

// Fast inverse initialization class.
CInitInverseTables invInitInverseTables;


//
// Additional comments.
//

/***********************************************************************************************
*
* 'C' version of the inverse function.
*
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
	CIntFloat if_val;		// Union of integer and floating point.
	int32     i4_signexp;	// Sign bit and exponent bits.
	int32     i4_mantissa;	// Mantissa bits.

	// Get sign, exponent and mantissa portions of the float.
	if_val.fFloat = f;
	i4_signexp    = if_val.i4Int & iFI_MASK_SIGN_EXPONENT;
	i4_mantissa   = (if_val.i4Int & iFI_MASK_MANTISSA) >> iSHIFT_MANTISSA;

	// Invert exponent.
	i4_signexp = iFI_MASK_EXPONENT - i4_signexp;

	// Get inverted mantissa.
	i4_mantissa = i4InverseMantissa[i4_mantissa];
	
	// Assemble components.
	if_val.i4Int = i4_signexp + i4_mantissa;

	// Return the inverse approximate value.
	return if_val.fFloat;
}
*
***********************************************************************************************/
