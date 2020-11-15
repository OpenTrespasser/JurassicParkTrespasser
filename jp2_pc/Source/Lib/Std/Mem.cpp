/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Mem.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/Mem.cpp                                                       $
 * 
 * 4     12/21/97 10:58p Rwyatt
 * Moved SetupSystemPageSize from the MemLimits.hpp header file
 * 
 * 3     12/04/97 4:16p Rwyatt
 * Reordered header files to accomodate MemLimits.hpp
 * 
 * 2     10/03/97 5:15p Rwyatt
 * Moved global memory function to here from FastHeap.cpp
 * 
 * 1     96/05/23 16:51 Speter
 * New memset functions.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "MemLimits.hpp"
#include "Mem.hpp"


uint32 gu4SystemPageSize = 4096;


//**********************************************************************************************
// Global functions that return info about free memory
//
//

	//******************************************************************************************
	void SetupSystemPageSize
	(
	)
	//*************************************
	{
		SYSTEM_INFO		si;
		GetSystemInfo(&si);

		gu4SystemPageSize = si.dwPageSize;

		// Page size is not a power of two!!, this is going to cause big trouble elsewhere.
		Assert(bPowerOfTwo(gu4SystemPageSize));
	}


	//******************************************************************************************
	// Return the amount of physical memory in the machine
	//
	uint64 u4TotalPhysicalMemory()
	{
		MEMORYSTATUSEX	ms = {0};
		ms.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&ms);

		return ms.ullTotalPhys;
	}


	//******************************************************************************************
	// Return the amount free of physical memory in the machine
	//
	uint64 u4FreePhysicalMemory()
	{
		MEMORYSTATUSEX	ms = {0};
		ms.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&ms);

		return ms.ullAvailPhys;
	}


	//******************************************************************************************
	// Return the amount space of memory in the page file
	//
	uint64 u4FreePagefileMemory()
	{
		MEMORYSTATUSEX	ms = {0};
		ms.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&ms);

		return ms.ullAvailPageFile;
	}


	//******************************************************************************************
	// Return the amount of free (un-commited and un-reserved) virtual address space
	//
	uint64 u4FreeVirtualMemory()
	{
		MEMORYSTATUSEX	ms = {0};
		ms.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&ms);

		return ms.ullAvailVirtual;
	}


	//******************************************************************************************
	// Return that commit state of a block of memory.
	// This will return one of the following:
	//		HEAP_BLOCK_COMMITED			(0)				Whole block in physical memory
	//		HEAP_BLOCK_PART_COMMITED	(1)				Part of block is in physical memory
	//		HEAP_BLOCK_NOT_COMMITED		(2)				Whole block is not in physical memory
	//		HEAP_BLOCK_FAIL				(0xffffffff)	virtual query fail
	//
	// The pointers passed to this function can be NULL if the information is not required.
	//
	uint32	u4HeapBlockStatus
	(
		void*	pv_adr,				// pointer to the block to query
		uint32	u4_length,			// size of the block
		void**	ppv_base,			// pointer to base of region pointer to be filled, (can be NULL)
		uint32*	pu4_region_size		// pointer to size of region to be filled (can be NULL)
	)
	{
		MEMORY_BASIC_INFORMATION	mbi_query;
		uint32						u4_ret;

		// query the address passed in, the OS will round the address down to the previous
		// page boundary so we do not have to.
		if (VirtualQuery(pv_adr,&mbi_query,sizeof(mbi_query)) < sizeof(mbi_query))
		{
			// less than the buffer was returned so we have failed..
			return HEAP_BLOCK_FAIL;
		}

		if (mbi_query.AllocationProtect == PAGE_NOACCESS)
		{
			// the block is marked as no access, this could be for a number of reasons but
			// will probably because windows has moved it into the swap file. Whatever the
			// reason that it is marked as no access it is going to fault when we touch it.
			if (mbi_query.RegionSize< u4_length)
			{
				// the region is less than the size we requested so the block must have a
				// mixed access type.
				u4_ret = HEAP_BLOCK_PART_COMMITED;
			}
			else
			{
				// all the block is marked as no access.
				u4_ret = HEAP_BLOCK_NOT_COMMITED;
			}
		}
		else
		{
			// the block should be read write, if it is not then we have modified it so
			// should be aware.
			if (mbi_query.RegionSize< u4_length)
			{
				// the region is less than the size we requested so the block must have a
				// mixed access type.
				u4_ret = HEAP_BLOCK_PART_COMMITED;
			}
			else
			{
				// all the block is marked as accessible in some form.
				u4_ret = HEAP_BLOCK_COMMITED;
			}
		}

		// fill in the pointers passed in if they are valid...

		// region base...
		if (ppv_base!=NULL)
		{
			*ppv_base = mbi_query.BaseAddress;
		}

		// region size..
		if (pu4_region_size!=NULL)
		{
			*pu4_region_size = mbi_query.RegionSize;
		}

		return u4_ret;
	}



//
// Include the processor-specific source code.
// Currently, only P5 version exists.
//
#include "P5/Mem.cpp"

