/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Math/FloatTable.hpp                                               $
 * 
 * 4     8/25/98 11:41a Rvande
 * Float values are not valid as template parameters; changed to ints
 * 
 * 3     98/04/26 20:16 Speter
 * Added some useful functions.
 * 
 * 2     98/03/31 17:10 Speter
 * Disabled Assert.
 * 
 * 1     98/03/24 14:02 Speter
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_MATH_FLOATTABLE_HPP
#define HEADER_LIB_MATH_FLOATTABLE_HPP

#include "FloatDef.hpp"

//*********************************************************************************************
//
template<class TOut, int iSIZE, int iIN_MIN, int iIN_MAX> class CFloatTable
//
// Prefix: ftb
//
// A base class which maps a float input value to an output of type TOut, via a lookup table.
// Standardises the floating-point manipulation necessary to do this quickly.
// Derived classes are responsible for actually filling the lookup table in their constructors.
//
// The size of the table is iSIZE. The input values range from iIN_MIN to iIN_MAX inclusive, 
// and are mapped with equal ranges to the table entries.  
//
// (For alternative mapping schemes, see derived classes.)
//
//**************************************
{
protected:
	float fAdd, fScale;					// The parameters converting input values to table entries.
										// Float variables should be as fast as inline constants.

	// Lookup table.  This must be filled in the derived class constructor.
	CSArray<TOut, iSIZE> satTable;

public:

	//*********************************************************************************************
	//
	// Operators.
	//

	//*********************************************************************************************
	//
	inline TOut operator ()
	(
		float f_in
	) const
	//
	// Returns the value of the appropriate table entry.
	//
	//**************************************
	{
		return satTable[iIndex(f_in)];
	}

	//*********************************************************************************************
	//
	inline TOut operator []
	(
		int i_index
	) const
	//
	// Returns the value of the appropriate table entry.
	//
	//**************************************
	{
		return satTable[i_index];
	}

	//*********************************************************************************************
	//
	// Member functions.
	//

	//*********************************************************************************************
	//
	inline int iIndex
	(
		float f_in
	) const
	//
	// Returns the index of the appropriate table entry.
	//
	//**************************************
	{
		//
		// For practical reasons, allow the input value to be slightly out of range, as long
		// as the truncation process yields a valid entry.
		//
		//Assert(bWithin(f_in, iIN_MIN, iIN_MAX));

		return iPosFloatCast((f_in + fAdd) * fScale);
	}

protected:

	//*********************************************************************************************
	//
	// Constructor.
	//

	//*********************************************************************************************
	//
	CFloatTable()
	//
	// Initialises and validates some compile-time parameters.  
	// Derived classes must init the table.
	//
	//**********************************
	{
		Assert(iSIZE > 1);
		Assert(bPowerOfTwo(iSIZE));
		Assert(iIN_MAX > iIN_MIN);

		// Scale up to just under max value plus 1, so that truncation yields valid range.
		fScale	= (float(iSIZE) - 0.01) / fRange();
		fAdd	= -iIN_MIN;
	}

	//
	// Utility functions usable in derived constructor for creating table.
	//

	// Trivial functions to return template params.

	inline int iSize() const
	{
		return iSIZE;
	}

	inline float fInMin() const
	{
		return iIN_MIN;
	}

	inline float fInMax() const
	{
		return iIN_MAX;
	}


	//*********************************************************************************************
	//
	inline float fRange() const
	//
	// Returns:
	//		The range of input values.
	//
	//**********************************
	{
		return iIN_MAX - iIN_MIN;
	}

	//*********************************************************************************************
	//
	inline float fStep() const
	//
	// Returns:
	//		The input amount stepped per entry.
	//
	//**********************************
	{
		return fRange() / iSIZE;
	}

	//*********************************************************************************************
	//
	inline float fStepFull() const
	//
	// Returns:
	//		The input amount stepped per entry.
	//
	//**********************************
	{
		return fRange() / (iSIZE-1);
	}

	//*********************************************************************************************
	//
	inline float fInputLow
	(
		int i_entry
	) const
	//
	// Returns:
	//		The float value corresponding to the low end of the table entry.
	//
	//**********************************
	{
		return iIN_MIN + float(i_entry) * fStep();
	}

	//*********************************************************************************************
	//
	inline float fInputHigh
	(
		int i_entry
	) const
	//
	// Returns:
	//		The float value corresponding to the high end of the table entry.
	//
	//**********************************
	{
		return iIN_MIN + float(i_entry+1) * fStep();
	}

	//*********************************************************************************************
	//
	inline float fInputMid
	(
		int i_entry
	) const
	//
	// Returns:
	//		The float value corresponding to the middle of the table entry.
	//
	//**********************************
	{
		return iIN_MIN + (float(i_entry) + 0.5) * fStep();
	}

	//*********************************************************************************************
	//
	inline float fInputFull
	(
		int i_entry
	) const
	//
	// Returns:
	//		The float value corresponding to a range of numbers stepping evenly through the
	//		full range of inputs.
	//
	//**********************************
	{
		return iIN_MIN + float(i_entry) * fStepFull();
	}
};

//*********************************************************************************************
//
template<class TOut, int iSIZE, int iIN_MIN, int iIN_MAX> class CFloatTableRound :
	public CFloatTable<TOut, iSIZE, iIN_MIN, iIN_MAX>
//
// Just like parent, but input values are rounded to the nearest slot, rather than being mapped evenly.
//
//**************************************
{
protected:

	//*********************************************************************************************
	//
	// Constructor.
	//

	//*********************************************************************************************
	//
	CFloatTableRound()
	//
	// Initialises and validates some compile-time parameters.  
	// Derived classes must init the table.
	//
	//**********************************
	{
		// Override fScale to scale exactly up to max value.
		fScale	= (float(iSIZE) - 1.01) / fRange();

		// Adjust fAdd to effect rounding.
		fAdd	= -iIN_MIN + 0.5 / fScale;
	}
};

#endif
