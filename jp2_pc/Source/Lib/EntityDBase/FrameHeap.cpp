/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of "FrameHeap.hpp."
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/FrameHeap.cpp                                         $
 * 
 * 4     8/25/98 4:46p Rwyatt
 * Reset heap
 * 
 * 3     5/30/97 11:13a Agrant
 * LINT tidying
 * 
 * 2     96/10/23 6:43p Mlange
 * Fixed some Hungarian names.
 * 
 * 1     10/21/96 4:00p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "gblinc/common.hpp"
#include "FrameHeap.hpp"


	//*****************************************************************************************
	//
	// CFrameHeap constructor and destructor.
	//

	//*****************************************************************************************
	CFrameHeap::CFrameHeap()
	{
		// Allocate memory for heaps.
		pfhCurrentHeap = new CFastHeap(iDEFAULT_FRAMEHEAP_SIZE);   //lint !e1732 !e1733
		pfhNextHeap    = new CFastHeap(iDEFAULT_FRAMEHEAP_SIZE);   //lint !e1732 !e1733

		Assert(pfhCurrentHeap);
		Assert(pfhNextHeap);
	}

	//*****************************************************************************************
	CFrameHeap::~CFrameHeap()
	{
		Assert(pfhCurrentHeap);
		Assert(pfhNextHeap);

		// Delete memory associated with heaps.
		delete pfhCurrentHeap;
		delete pfhNextHeap;
	}


	//*****************************************************************************************
	//
	// CFrameHeap member functions.
	//

	//*****************************************************************************************
	void CFrameHeap::Flip()
	{
		Assert(pfhCurrentHeap);
		Assert(pfhNextHeap);

		//
		// Reset the current heap, then make the current heap the next heap, and
		// the next heap the current heap.
		//
		pfhCurrentHeap->Reset(0,0);
		Swap(pfhCurrentHeap, pfhNextHeap);
	}


//
// Module-specific variables.
//

CFrameHeap frhFrameHeap;
