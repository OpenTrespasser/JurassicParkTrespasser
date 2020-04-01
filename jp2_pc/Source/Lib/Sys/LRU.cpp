/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:   Implementation of LRU.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/LRU.cpp                                                      $
 * 
 * 7     10/02/98 3:50p Mmouni
 * Changed include for metroworks STL.
 * 
 * 6     10/01/98 7:12p Pkeet
 * Fixed lurking bug in the LRU.
 * 
 * 5     9/08/98 8:56p Rwyatt
 * LRU memory is now properly destroyed
 * 
 * 4     8/25/98 4:39p Rwyatt
 * Reset heap
 * 
 * 3     4/15/98 5:56p Pkeet
 * Changed the maximum number of LRU items permitted.
 * 
 * 2     4/13/98 11:41a Pkeet
 * Fixed bug that failed to correctly delete items.
 * 
 * 1     4/09/98 4:45p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#include "Common.hpp"
#include "LRU.hpp"

#include "Lib/Std/PrivSelf.hpp"
#include "Lib/Sys/FastHeap.hpp"

#if defined(__MWERKS__)
	#include <algorithm>
#else
	#include <algorithm>
#endif

//
// Macros and constants.
//

// Maximum number of LRU objects that will be allowed.
const int iMaxNumLRUItems = 8192;


//
// Module variables.
//


//
// Internal class definitions.
//

//*********************************************************************************************
//
class CLRUItemCompare
//
// Compares two CSchedulerItem by priority.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	bool operator()
	(
		const CLRUItem* plrit_0,	// First LRU item.
		const CLRUItem* plrit_1		// Second LRU item.
	) const
	//
	// Returns 'true' if the priority of the first LRU object is larger than the priority
	// of the second LRU object.
	//
	//**************************************
	{
		Assert(plrit_0);
		Assert(plrit_1);

		// Return the results of the comparision.
		return plrit_0->iAge < plrit_1->iAge;
	}

};

//**********************************************************************************************
//
class CLRU::CPriv : public CLRU
//
// Private member functions for class 'CLRU.'
//
//**************************************
{
public:

	//******************************************************************************************
	//
	void Sort
	(
	)
	//
	// Sorts the LRU array if it is unsorted.
	//
	//**************************************
	{
		// Do nothing if the list is already sorted.
		if (bSorted)
			return;

		// Use the STL Quicksort routine.
		std::sort(papItems->atArray, papItems->atArray + papItems->uLen, CLRUItemCompare());

		// Indicate that the list is now sorted.
		bSorted = true;
	}

};


//
// Class definitions.
//

//
// Class implementation.
//

//*********************************************************************************************
//
// CLRU Implementation.
//

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	CLRU::CLRU()
		: bSorted(false), pfhHeap(new CFastHeap(iMaxNumLRUItems * sizeof(CLRUItem)))
	{
		pplritArray = new CLRUItem*[iMaxNumLRUItems];
		papItems = new CMArray<CLRUItem*>(iMaxNumLRUItems,pplritArray);
		pfhHeap->Align(8);
	}

	CLRU::~CLRU()
	{
		delete		pfhHeap;
		delete		papItems;
		delete		pplritArray;
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	void CLRU::AddItem(CLRUItem* plrit)
	{
		Assert(plrit);
		(*papItems) << plrit;

		// The list is now unsorted.
		bSorted = false;
	}

	//*****************************************************************************************
	bool CLRU::bDelete(int i_num_delete)
	{
		// Do nothing if there are no items that can be deleted.
		if (papItems->uLen == 0 || i_num_delete == 0)
			return false;

		// If the list is not sorted, sort it.
		priv_self.Sort();

		//
		// Remove the oldest elements.
		//
		for (int i = 0; i < i_num_delete; ++i)
		{
			if (papItems->uLen < 1)
				return false;
			(*papItems)[papItems->uLen - 1]->Delete();
			--papItems->uLen;
		}

		return true;
	}

	//*****************************************************************************************
	void CLRU::Reset()
	{
		// Reset the sort array.
		papItems->Reset();

		// Reset the fast heap used as memory for the LRU items.
		pfhHeap->Reset(0,0);
	}


//
// Global functions.
//

//*********************************************************************************************
void* operator new(uint u_size_type, CLRU& lru)
{
	return operator new(u_size_type, *lru.pfhHeap);
}


//
// Variable allocations.
//
CLRU lruTerrainCache;