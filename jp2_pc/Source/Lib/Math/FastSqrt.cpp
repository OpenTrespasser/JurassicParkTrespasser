/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of FastSqrt.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Math/FastSqrt.cpp                                                 $
 * 
 * 9     98/08/25 19:09 Speter
 * Changed test code.
 * 
 * 8     98/08/24 21:52 Speter
 * More VC 6.0 carnage.
 * 
 * 7     97.12.10 2:59p Mmouni
 * Added quick test for fSqrt(), fInvSqrt().
 * 
 * 6     7/15/96 12:08p Mlange
 * Changed the name of the table initialisation class.
 * 
 * 5     96/06/20 6:26p Mlange
 * Updated for changes to the CIntFloat class. Indented definition of CSqrtTables. 
 * 
 * 4     3-06-96 3:41p Mlange
 * Added assert for table size as a power of two. Changes from code review: Now uses 32 bit
 * tables.
 * 
 * 3     29-05-96 2:35p Mlange
 * Updated so that initialisation of the lookup tables will always take place before any global
 * constructors that may depend on it.
 * 
 * 2     14-05-96 8:17p Mlange
 * Updated for changes to float def constants.
 * 
 * 1     5/08/96 12:11p Mlange
 * Initialise square root tables.
 *
 **********************************************************************************************/

#include <math.h>

#include "GblInc/Common.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/Profile.hpp"

#include "FastSqrt.hpp"


// The lookup tables.
int32 ai4SqrtMantissaTable[iFAST_SQRT_TABLE_SIZE];
int32 ai4InvSqrtMantissaTable[iFAST_INV_SQRT_TABLE_SIZE];


//**********************************************************************************************
//
// CInitSqrtTables implementation.
//

// MORE VC 6.0 CARNAGE.
#pragma optimize("g", off)

	static void TestFastSqrt();

	//******************************************************************************************
	CInitSqrtTables::CInitSqrtTables()
	{
		static bool b_initialised = false;

		CIntFloat if_curr_val;
		CIntFloat if_next_val;
		CIntFloat if_tbl_entry;
		CIntFloat if_temp;
		int i;

		// Ensure the table sizes are a power of two.
		Assert(bPowerOfTwo(iFAST_SQRT_TABLE_SIZE));
		Assert(bPowerOfTwo(iFAST_INV_SQRT_TABLE_SIZE));

		// Ensure the tables are only initialised once.
		if (b_initialised)
			return;

		//
		// Initialise the square root mantissa lookup table.
		//

		// The values in the table are in the range [sqrt(1.0), sqrt(4.0)).
		if_curr_val.SetExponent(0);

		for(i = 0; i < iFAST_SQRT_TABLE_SIZE; i++)
		{
			// The table value is the square root of the average of the range for that entry, so first calculate the next
			// entry for the table.
			if_temp.SetMantissa(1 << (iFLOAT_MANT_BIT_WIDTH - (iFAST_SQRT_TABLE_SIZE_BITS - 1)));

			if_next_val = if_curr_val.i4Int + if_temp.i4Int;

			// Then take the average of the two values.
			if_tbl_entry = (if_next_val.fFloat - if_curr_val.fFloat) * .5f + if_curr_val.fFloat;

			// Calculate the accurate square root for the table.
			if_tbl_entry = (float)sqrt(if_tbl_entry);

			ai4SqrtMantissaTable[i] = if_tbl_entry.i4GetMantissa();

			// Include the exponent' bias value in the table entry as required by the square root function to bias the
			// exponent efficiently.
			if_temp.SetExponent(0);

			ai4SqrtMantissaTable[i] |= if_temp.i4Int;

			if_curr_val = if_next_val;
		}



		//
		// Initialise the inverse square root mantissa lookup table.
		//

		// The values in the table are in the range [1.0 / sqrt(0.5), 1.0 / sqrt(2.0)).
		if_curr_val.SetExponent(-1);

		for(i = 0; i < iFAST_INV_SQRT_TABLE_SIZE; i++)
		{
			// The table value is the inverse square root of the average of the range for that entry, so first
			// calculate the next entry for the table.
			if_temp.SetMantissa(1 << (iFLOAT_MANT_BIT_WIDTH - (iFAST_INV_SQRT_TABLE_SIZE_BITS - 1)));

			if_next_val = if_curr_val.i4Int + if_temp.i4Int;

			// Then take the average of the two values.
			if_tbl_entry = (if_next_val.fFloat - if_curr_val.fFloat) * .5f + if_curr_val.fFloat;

			// Calculate the accurate inverse square root for the table.
			if_tbl_entry = 1.0f / (float)sqrt(if_tbl_entry);
			
			ai4InvSqrtMantissaTable[i] = if_tbl_entry.i4GetMantissa();

			if_curr_val = if_next_val;
		}

		
		// Indicate tables are initialised.
		b_initialised = true;

		TestFastSqrt();
	}

#pragma optimize("g", on)

	//******************************************************************************************
	void Dummy(float f)
	{
	}

	//******************************************************************************************
	static void TestFastSqrt()
	{
#if (0)
		// Quick test of fSqrt, fInvSqrt.
		for (float v = 0.0f; v < 10.0f; v += 0.1f)
		{
			volatile float f1 = 1.0f/sqrt(v);
			volatile float f2 = fInvSqrt(v);

			if (f1 != f2)
			{
				dprintf("fInvSqrt Diff: %f, %f %f, %08x %08x\n", 
					v, f1, f2, *(int *)&f1, *(int *)&f2);
			}

			f1 = sqrt(v);
			f2 = fSqrt(v);

			if (f1 != f2)
			{
				dprintf("fSqrt Diff: %f, %f %f, %08x %08x\n", 
					v, f1, f2, *(int *)&f1, *(int *)&f2);
			}
		}

		// Now time it.

		CCycleTimer ctmr;
		for (int i = 0; i < 1000000; i++)
		{
			Dummy((float)i);
		}
		TCycles cy_base = ctmr();

		for (i = 0; i < 1000000; i++)
		{
			Dummy(sqrt((float)i));
		}
		dout << "sqrt: " <<(ctmr() - cy_base) * CCycleTimer::fSecondsPerCycle() <<endl;

		for (i = 0; i < 1000000; i++)
		{
			Dummy(1.0f / sqrt((float)i));
		}
		dout << "1/sqrt: " <<(ctmr()  - cy_base) * CCycleTimer::fSecondsPerCycle() <<endl;

		for (i = 0; i < 1000000; i++)
		{
			Dummy(fSqrt(float(i)));
		}
		dout << "fSqrt: " <<(ctmr()  - cy_base) * CCycleTimer::fSecondsPerCycle() <<endl;

		for (i = 0; i < 1000000; i++)
		{
			Dummy(fInvSqrt(float(i)));
		}
		dout << "fInvSqrt: " <<(ctmr()  - cy_base) * CCycleTimer::fSecondsPerCycle() <<endl;

		for (i = 0; i < 1000000; i++)
		{
			Dummy(fSqrtEst(float(i)));
		}
		dout << "fSqrtEst: " <<(ctmr()  - cy_base) * CCycleTimer::fSecondsPerCycle() <<endl;

		for (i = 0; i < 1000000; i++)
		{
			Dummy(fInvSqrtEst(float(i)));
		}
		dout << "fInvSqrtEst: " <<(ctmr()  - cy_base) * CCycleTimer::fSecondsPerCycle() <<endl;
#endif
	}
