/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of FastTrig.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Math/FastTrig.cpp                                                 $
 * 
 * 4     7/15/96 12:18p Mlange
 * Changed the name of the table initialisation class.
 * 
 * 3     96/06/24 10:00a Mlange
 * Indented the implementation of the constructor by a single tab.
 * 
 * 2     3-06-96 5:56p Mlange
 * Added assert for table size as a power of two. Updated so that initialisation of the lookup
 * tables  will always take place before any global constructors that may depend on it.
 * 
 * 1     5/09/96 1:50p Mlange
 * Fast trig functions.
 * 
 **********************************************************************************************/

#include <math.h>

#include "GblInc/Common.hpp"

#include "FastTrig.hpp"


// The lookup tables.
float fSinTable[iSIN_TABLE_SIZE];


//**********************************************************************************************
//
// CInitTrigTables implementation.
//

	//******************************************************************************************
	CInitTrigTables::CInitTrigTables()
	{
		static bool b_initialised = false;

		// Ensure the table sizes are a power of two.
		Assert(bPowerOfTwo(iSIN_TABLE_SIZE));

		// Ensure the tables are only initialised once.
		if (b_initialised)
			return;


		//
		// Initialise the sine/cosine lookup table.
		//
		
		// Indicate tables are initialised.
		b_initialised = true;
	}
