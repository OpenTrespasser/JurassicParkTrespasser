/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of Random.hpp.
 *
 * Bugs:
 *
 * To do:
 *		Remove dependance on ANSI rand() in the seed function. Write a true 32 bit linear
 *		congruential random number generator to seed the table instead.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/Random.cpp                                                    $
 * 
 * 2     98/08/19 2:30 Speter
 * Added a static randMain member.
 * 
 * 1     98/01/15 14:24 Speter
 * New file, contains code moved from .hpp.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Random.hpp"

#include <stdlib.h>

//**********************************************************************************************
//
// class CRandom implementation.
//

	CRandom CRandom::randMain;

	//******************************************************************************************
	void CRandom::Seed(uint32 u4_seed)
	{
		srand(u4_seed);

		// Initialise the table.
		for (uint u_index = 0; u_index < iRANDOM_TABLE_SIZE; u_index++)
		{
			u4RandomTable[u_index] = ((uint32)rand() << 16) ^ (uint32)rand();
		}

		iIndexJ = 0;
		iIndexK = iRANDOM_INDEX_DIFF;
	}

	//******************************************************************************************
	uint32 CRandom::u4Next()
	{
		uint32 u4_random = u4RandomTable[iIndexJ] + u4RandomTable[iIndexK];

		u4RandomTable[iIndexK] = u4_random;

		// Update table indices, wrap on underflow.
		iIndexJ--;
		iIndexK--;

		if (iIndexJ < 0)
			iIndexJ = iRANDOM_TABLE_SIZE - 1;

		if (iIndexK < 0)
			iIndexK = iRANDOM_TABLE_SIZE - 1;

		return u4_random;
	}

	//*****************************************************************************************
	char* CRandom::pcSave(char* pc) const
	{
		// Well, we could save the entire table and make sure that our random numbers are ALWAYS THE SAME.
		// Instead, I'll just save a seed so that we have consistent random numbers (!) on restore.
		*((uint32*)pc) = u4RandomTable[iIndexJ];
		pc += sizeof(uint32);

		return pc;
	}

	//*****************************************************************************************
	const char* CRandom::pcLoad(const char* pc)
	{
		Seed(*((uint32*)pc));
		pc += sizeof(uint32);

		return pc;
	}
