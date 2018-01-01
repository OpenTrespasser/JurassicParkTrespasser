/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Class for efficient memory management of fixed sized blocks.
 *
 * Bugs:
 *
 * To do:
 *		The inheritance mechanism can be improved, esp wrt the static variable requirement in
 *		the derived class.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/BlockAllocator.hpp                                            $
 * 
 * 11    9/14/98 5:44p Mlange
 * Fixed bug: the maximum allocations counter was never set or reset.
 * 
 * 10    1/26/98 8:13p Mlange
 * Now (optionally) maintains a handle. Minor optimisations.
 * 
 * 9     10/15/97 10:57a Mlange
 * Re-instated destructor assert.
 * 
 * 8     10/08/97 7:03p Mlange
 * In debug mode, now also trashes the contents of blocks as they are allocated from the fast
 * heap.
 * 
 * 7     10/03/97 4:43p Mlange
 * The fast heap to use may now be omitted from the constructor call. The fast heap can be
 * specified later, just before actual allocations are made.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_BLOCKALLOCATOR_HPP
#define HEADER_LIB_STD_BLOCKALLOCATOR_HPP

#include <string.h>

#include "Lib/Sys/FastHeap.hpp"


//**********************************************************************************************
//
template<class T_TYPE> class CBlockAllocator
//
// A base class that efficiently manages memory in fixed size blocks.
//
// Prefix: ba
//
// Notes:
//		This class manages a sparse array of T_TYPEs that grows dynamically if new allocations
//		require it. Elements that are deleted leave a hole in this sparse array. These holes
//		are recycled for subsequent allocations.
//
//		Each class inherited from this must declare a static data member named 'stStore' of type
//		'CBlockAllocator<CDERIVED>::SStore'. This static data member must be accessable by this
//		base class.
//
// Example:
//		class CFoo : public CBlockAllocator<CFoo>
//		{
//			friend class CBlockAllocator<CFoo>;
//			static CBlockAllocator<CFoo>::SStore stStore; // Storage for the types.
//		};
//
//**************************************
{
	struct SFreeBlock
	// Prefix: fb
	// Maintains the linked list of free blocks.
	{
		SFreeBlock* pfbNextFree;	// Pointer to next free block, null if last.
		int iHandle;				// Unique 'index' handle assigned to the associated reserved memory block.
	};

public:
	struct SStore
	// Prefix: st
	// The storage for the types.
	{
		int iNumAlloc;				// Number of allocated types.
		int iMaxAlloc;				// Maximum number of types ever allocated.

		SFreeBlock* pfbFreeList;	// Pointer to the first element in the linked list of free types.
		CFastHeap*  pfhFreeStore;	// Storage for the types.

		// Constructor.
		SStore(CFastHeap* pfh = 0)
			: iNumAlloc(0), iMaxAlloc(0), pfbFreeList(0), pfhFreeStore(pfh)
		{
		}

		// Destructor.
		~SStore()
		{
			// Cannot free memory if there are still allocations.
			AlwaysAssert(iNumAlloc == 0);
		}
	};



	//******************************************************************************************
	//
	// Overloaded operators.
	//

	//******************************************************************************************
	void* operator new(size_t i_size)
	{
		Assert(sizeof(T_TYPE) >= sizeof(SFreeBlock));
		Assert(i_size == sizeof(T_TYPE));
		Assert(T_TYPE::stStore.iNumAlloc >= 0);
		Assert(T_TYPE::stStore.pfhFreeStore != 0);

		T_TYPE::stStore.iNumAlloc++;

		// Are there elements on the free list?
		if (T_TYPE::stStore.pfbFreeList != 0)
		{
			// Pop the first item from the free list.
			SFreeBlock* pfb_free = T_TYPE::stStore.pfbFreeList;
			T_TYPE::stStore.pfbFreeList = pfb_free->pfbNextFree;

			reinterpret_cast<T_TYPE*>(pfb_free)->SetHandle(pfb_free->iHandle);

			return pfb_free;
		}
		else
		{
			// Allocate a new element from the fast heap.
			T_TYPE::stStore.pfhFreeStore->Align(sizeof(int));

			void* pv_new = T_TYPE::stStore.pfhFreeStore->pvAllocate(i_size);

			// In debug mode, trash the contents of the returned block.
			#if VER_DEBUG
				memset(pv_new, 0xCD, sizeof(T_TYPE));
			#endif

			reinterpret_cast<T_TYPE*>(pv_new)->SetHandle(T_TYPE::stStore.iMaxAlloc);

			T_TYPE::stStore.iMaxAlloc++;

			return pv_new;
		}
	}

	//******************************************************************************************
	void operator delete(void* pv)
	{
		Assert(pv != 0);
		Assert(T_TYPE::stStore.iNumAlloc > 0);

		T_TYPE::stStore.iNumAlloc--;

		int i_handle = reinterpret_cast<T_TYPE*>(pv)->iGetHandle();

		// In debug mode, trash the old contents of this block.
		#if VER_DEBUG
			memset(pv, 0xCD, sizeof(T_TYPE));
		#endif

		// This is really a SFreeBlock.
		SFreeBlock* pfb = reinterpret_cast<SFreeBlock*>(pv);

		// Add the element to the start of the free list.
		pfb->pfbNextFree = T_TYPE::stStore.pfbFreeList;
		pfb->iHandle     = i_handle;
		T_TYPE::stStore.pfbFreeList = pfb;
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	int iGetHandle() const
	//
	// Returns:
	//		A positive integer associated with the memory block reserved for this type, or -1
	//		if not implemented by the derived class.
	//
	// Notes:
	//		The derived class is responsible for defining the storage for the handle, if this
	//		handle functionality is required. The derived class must overide this function in
	//		this case.
	//
	//		If implemented, a handle is an integer number in the range [0, T_TYPE::uMaxAlloc()].
	//
	//**********************************
	{
		return -1;
	}


	//******************************************************************************************
	//
	void SetHandle
	(
		int i_handle
	)
	//
	// Set the handle for this type.
	//
	// Notes:
	//		The derived class is responsible for defining the storage for the handle, if this
	//		handle functionality is required. The derived class must overide this function in
	//		this case.
	//
	//**********************************
	{
	}


	//******************************************************************************************
	//
	static void SetFastHeap
	(
		CFastHeap* pfh
	)
	//
	// Specify the fast heap to use for subsequent allocations.
	//
	//**********************************
	{
		Assert(T_TYPE::stStore.iNumAlloc == 0);

		T_TYPE::stStore.pfhFreeStore = pfh;
		T_TYPE::stStore.pfbFreeList  = 0;
		T_TYPE::stStore.iNumAlloc    = 0;
		T_TYPE::stStore.iMaxAlloc    = 0;
	}


	//******************************************************************************************
	//
	static CFastHeap* pfhGetFastHeap()
	//
	// Returns:
	//		The current fast heap in use, null if none yet specified.
	//
	//**********************************
	{
		return T_TYPE::stStore.pfhFreeStore;
	}


	//******************************************************************************************
	//
	static uint uNumAlloc()
	//
	// Returns:
	//		The number of allocated types.
	//
	//**********************************
	{
		return T_TYPE::stStore.iNumAlloc;
	}


	//******************************************************************************************
	//
	static uint uMaxAlloc()
	//
	// Returns:
	//		The largest number of types that were ever allocated.
	//
	//**********************************
	{
		return T_TYPE::stStore.iMaxAlloc;
	}
};


#endif
