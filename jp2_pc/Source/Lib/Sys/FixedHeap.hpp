/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Fixed size heap object that provides better performance than the Windows heap.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/Sys/FixedHeap.hpp                                                 $
 * 
 * 4     10/01/98 12:25a Pkeet
 * Upped the default memory allocation.
 * 
 * 3     12/01/97 12:27p Pkeet
 * Added member function for returning info about the current state of the heap.
 * 
 * 2     11/26/97 12:04p Pkeet
 * Added the 'DumpState' member function and the global variable.
 * 
 * 1     11/26/97 11:29a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_SYS_FIXEDHEAP_HPP
#define HEADER_LIB_SYS_FIXEDHEAP_HPP


//
// Forward declarations for opaque pointers.
//
class THeapMem;
class THeapSize;


//
// Class definitions.
//

//*********************************************************************************************
//
class CFixedHeap
//
// Prefix: fxh
//
// Fixed size heap.
//
//**************************************
{
	void*      pvHeap;			// Pointer to heap memory.
	int        iSize;			// Size of heap memory.
	int        iGranularity;	// Granularity of allocations.
	THeapMem*  phsetAllocated;	// List of allocations made.
	THeapSize* phsetFreeSize;	// List of free blocks based on size.
	THeapMem*  phsetFreeMem;	// List of free blocks based on address.
public:

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	// Default constructor.
	CFixedHeap();

	// Destructor.
	~CFixedHeap();

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void* pvMalloc
	(
		int i_size
	);
	//
	// Allocates memory from this heap. If this heap cannot allocate the requested memory, the
	// memory is allocated from the Windows heap. This object will track where the memory came
	// from, so that 'Free' can always be called on memory allocated from here.
	//
	// Returns a pointer to the allocated memory.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Free
	(
		void* pv
	);
	//
	// Frees memory allocated from 'pvMalloc.'
	//
	//**************************************

	//*****************************************************************************************
	//
	void Reset
	(
	);
	//
	// Resets the fixed heap structure.
	//
	//**************************************

	//*****************************************************************************************
	//
	int iGetSizeKB
	(
	) const
	//
	// Returns the size of memory allocated.
	//
	//**************************************
	{
		return iSize >> 10;
	}

	//*****************************************************************************************
	//
	bool bConglomerate();
	//
	// Merges free blocks together.
	//
	// Returns 'true' if blocks were merged.
	//
	// Notes:
	//		During a pass this function will merge consecutive blocks of memory together;
	//		however this does not guarantee that all blocks that can be merged will be merged.
	//		Calling this function more that once will guarantee all blocks that can be merged
	//		are merged.
	//
	//**************************************

	//*****************************************************************************************
	//
	void DumpState
	(
	) const;
	//
	// Dumps the state of the heap into a file called 'FixedHeap.txt.'
	//
	//**************************************

	//*****************************************************************************************
	//
	void GetInfo
	(
		int& ri_num_nodes,	// Number of memory allocations.
		int& ri_mem_used	// Amount of memory allocated.
	) const;
	//
	// Returns info on the current state of the heap.
	//
	//**************************************

private:

	//*****************************************************************************************
	void Alloc(int i_size, int i_granularity);

	//*****************************************************************************************
	void Dealloc();


};


//
// Global variables.
//
extern CFixedHeap fxhHeap;


#endif // HEADER_LIB_SYS_FIXEDHEAP_HPP