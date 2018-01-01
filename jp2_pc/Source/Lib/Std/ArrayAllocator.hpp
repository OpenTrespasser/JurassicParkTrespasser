/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Class for efficient memory management of varying length arrays of same type.
 *
 * Bugs:
 *
 * To do:
 *		Add overloaded new and delete operators based on CArrayAllocator.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/ArrayAllocator.hpp                                            $
 * 
 * 2     1/12/98 5:04p Mlange
 * Fixed bug. CArrayAllocator::SetFastHeap() was not reseting the free lists.
 * 
 * 1     12/02/97 1:38p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_ARRAYALLOCATOR_HPP
#define HEADER_LIB_STD_ARRAYALLOCATOR_HPP

#include <string.h>

#include "Lib/Sys/FastHeap.hpp"


//**********************************************************************************************
//
template<class T, uint uMaxLength> class CArrayAllocator
//
// A class that efficiently manages memory for arrays of same type but of varying length.
//
// Prefix: aa
//
// Notes:
//		This class manages a sparse array of arrays of type T that grows dynamically if new
//		allocations	require it. Arrays that are deleted leave a hole in this sparse array. These
//		holes are recycled for subsequent allocations.
//
//		Template parameter 'T' specifies the type of the array elements and template parameter
//		'uMaxLength' specifies the maximum length (# of elements) of an array that can be
//		allocated with this class.
//
//**************************************
{
	struct SFreeBlock
	// Prefix: fb
	// Maintains the linked list of free blocks.
	{
		SFreeBlock* pfbNextFree;
	};


	CFastHeap* pfhFreeStore;	// Fast heap to allocate from.

	int iNumAllocBytes_;		// Current number of allocated bytes.
	int iMaxAllocBytes_;		// Max number of bytes ever allocated.


	SFreeBlock* apfbFreeBlocks[uMaxLength];
								// Array of pointers to free lists, one for each specific
								// size of T array or null if it doesn't exist. Index 0 into this
								// free list array contains a pointer to the free list for T arrays
								// that are one element in length, index 1 contains a pointer to the
								// free list for arrays that are of length 2, etc.


	//******************************************************************************************
	//
	// Constructors and destructor.
	//
public:
	CArrayAllocator(CFastHeap* pfh = 0)
		: iNumAllocBytes_(0), iMaxAllocBytes_(0)
	{
		SetFastHeap(pfh);
	}

	// Destructor.
	~CArrayAllocator()
	{
		AlwaysAssert(iNumAllocBytes_ == 0);
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SetFastHeap
	(
		CFastHeap* pfh
	)
	//
	// Specify the fast heap to use for subsequent allocations.
	//
	//**********************************
	{
		Assert(iNumAllocBytes_ == 0);
		pfhFreeStore = pfh;

		iMaxAllocBytes_ = 0;

		// Initialise the free lists.
		for (int i = 0; i < uMaxLength; i++)
			apfbFreeBlocks[i] = 0;
	}


	//******************************************************************************************
	//
	CFastHeap* pfhGetFastHeap() const
	//
	// Returns:
	//		The current fast heap in use, null if none yet specified.
	//
	//**********************************
	{
		return pfhFreeStore;
	}


	//******************************************************************************************
	//
	T* patAlloc
	(
		uint u_size		// Size of array to allocate.
	)
	//
	// Allocate an array of T of the specified size.
	//
	// Notes:
	//		Does NOT call the default constructor on the elements in the array!
	//
	//**********************************
	{
		Assert(pfhFreeStore != 0);
		Assert(u_size > 0 && u_size <= uMaxLength);

		iNumAllocBytes_ += u_size * sizeof(T);
		iMaxAllocBytes_ = Max(iNumAllocBytes_, iMaxAllocBytes_);

		uint u_index = u_size - 1;

		// Are there elements on the free list?
		if (apfbFreeBlocks[u_index] != 0)
		{
			// Pop the first item from the free list.
			SFreeBlock* pfb_new = apfbFreeBlocks[u_index];
			apfbFreeBlocks[u_index] = pfb_new->pfbNextFree;

			return reinterpret_cast<T*>(pfb_new);
		}
		else
		{
			// Allocate a new element from the fast heap.
			pfhFreeStore->Align(sizeof(int));

			void* pv_new = pfhFreeStore->pvAllocate(u_size * sizeof(T));

			// In debug mode, trash the contents of the returned block.
			#if VER_DEBUG
				memset(pv_new, 0xCD, u_size * sizeof(T));
			#endif

			return reinterpret_cast<T*>(pv_new);
		}
	}


	//******************************************************************************************
	//
	void Free
	(
		T* pat,			// Array to delete.
		uint u_size		// Size of this array.
	)
	//
	// Free the memory for an array of T.
	//
	// Notes:
	//		Does NOT call the destructor on the elements in the array!
	//
	//**********************************
	{
		Assert(pfhFreeStore != 0);
		Assert(u_size > 0 && u_size <= uMaxLength);
		Assert(pat != 0);

		iNumAllocBytes_ -= u_size * sizeof(T);
		Assert(iNumAllocBytes_ >= 0);

		// In debug mode, trash the old contents of this block.
		#if VER_DEBUG
			memset(pat, 0xCD, u_size * sizeof(T));
		#endif

		// This is really a SFreeBlock.
		SFreeBlock* pfb = reinterpret_cast<SFreeBlock*>(pat);

		uint u_index = u_size - 1;

		// Add the element to the start of the free list.
		pfb->pfbNextFree = apfbFreeBlocks[u_index];
		apfbFreeBlocks[u_index] = pfb;
	}


	//******************************************************************************************
	//
	uint uNumAllocBytes() const
	//
	// Returns:
	//		The number of allocated bytes.
	//
	//**********************************
	{
		return iNumAllocBytes_;
	}


	//******************************************************************************************
	//
	uint uMaxAllocBytes() const
	//
	// Returns:
	//		The largest number of bytes that were ever allocated.
	//
	//**********************************
	{
		return iMaxAllocBytes_;
	}
};


#endif
