/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/FrameHeap.hpp                                         $
 * 
 * 5     9/30/98 9:37p Rwyatt
 * Reset function to clear out queue fast heap
 * 
 * 4     5/30/97 11:13a Agrant
 * LINT tidying
 * 
 * 3     1/08/97 7:36p Pkeet
 * Fixed bug in the 'new' operator that prevented this module from being compiled under Visual
 * C++ 5.0.
 * 
 * 2     96/10/23 6:43p Mlange
 * Added Hungarian prefix to the global instance of the frame heap. Fixed some Hungarian
 * prefixes.
 * 
 * 1     10/21/96 4:00p Pkeet
 * Initial implementation.
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_EVENT_FRAMEHEAP_HPP
#define HEADER_LIB_EVENT_FRAMEHEAP_HPP


//
// Includes.
//
#include "Lib/Sys/FastHeap.hpp"


//
// Constants.
//

// Default size for the temporary heaps for the current and next frames.
const int iDEFAULT_FRAMEHEAP_SIZE = 1 << 16;


//
// Classes.
//

//*********************************************************************************************
//
class CFrameHeap
//
// An object to maintain two fast heap objects. The fast heap objects allocate memory for use
// by the current frame and the next frame. When a "flip" occurs, the current frame's fast heap
// is reset and then assigned as the next frame's temporary heap, while the next frame's
// heap is used as the current frame's heap.
//
// Prefix: frh
//
//**************************************
{
protected:

	CFastHeap* pfhCurrentHeap;	// Temporary fast heap associated with the current frame.
	CFastHeap* pfhNextHeap;		// Temporary fast heap associated with the next frame.

public:

	//*****************************************************************************************
	//
	// CFrameHeap constructor and destructor.
	//

	CFrameHeap();

	~CFrameHeap();


	//*****************************************************************************************
	//
	// CFrameHeap member functions.
	//

	//*****************************************************************************************
	//
	CFastHeap* pfhGetCurrentHeap
	(
	)
	//
	// Returns the temporary heap associated with the current frame.
	//
	//**************************************
	{
		Assert(pfhCurrentHeap);

		return pfhCurrentHeap;
	};


	//*****************************************************************************************
	//
	void Reset
	(
	)
	//
	// Resets both fast heaps within the frame heap
	//
	//**************************************
	{
		pfhCurrentHeap->Reset(0,0);
		pfhNextHeap->Reset(0,0);
	};


	//*****************************************************************************************
	//
	CFastHeap* pfhGetNextHeap
	(
	) const
	//
	// Returns the temporary heap associated with the next frame.
	//
	//**************************************
	{
		Assert(pfhNextHeap);

		return pfhNextHeap;
	};

	//*****************************************************************************************
	//
	void Flip
	(
	);
	//
	// Swaps the current and next temporary heaps after clearing the current heap.
	//
	//**************************************
};


//
// Global functions.
//

//*********************************************************************************************
//
inline void* operator new
(
	uint  u_size_type,			// Amount of memory to allocate in bytes.
	const CFrameHeap& frh_heap	// Frame heap to allocate from.
)
//
// Returns pointer to memory allocated from the next frame's temporary heap.
//
//**************************************
{
	//
	// Call the 'new' operator associated with the fast heap module using the next frame's fast
	// heap.
	//
	return new (*frh_heap.pfhGetNextHeap()) uint8[u_size_type];   //lint !e119
};


//
// External declarations of global functions.
//

// Object to maintain temporary heaps for the current and the next game frame.
extern CFrameHeap frhFrameHeap;


#endif
