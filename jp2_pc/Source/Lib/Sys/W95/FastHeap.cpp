/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of FastHeap.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/W95/FastHeap.cpp                                              $
 * 
 * 17    98/09/15 0:42 Speter
 * Made Commit/Decommit functions AlwaysAssert.
 * 
 * 16    8/25/98 4:37p Rwyatt
 * Added a mem counter to decommit function
 * 
 * 15    12/21/97 10:52p Rwyatt
 * Added decommit function to decommit virtual memory
 * 
 * 14    12/16/97 3:39p Mlange
 * CDArray now commits memory as it is allocated.
 * 
 * 13    12/04/97 4:17p Rwyatt
 * Fast heap allocator can either reserve or reserve and commit virtual blocks.
 * A new function to commit to an offset in a virtual block has been added.
 * 
 * 12    10/03/97 5:17p Rwyatt
 * Removed global memory functions from this file and put the in std/mem.cpp
 * 
 * 11    10/02/97 5:41p Rwyatt
 * Added global function to give the virtual status of a block of memory
 * 
 * 10    9/18/97 3:47p Rwyatt
 * Added global functions to return system memory info/state
 * 
 * 9     97/03/18 19:19 Speter
 * Re-organised CDArray and CFastHeap classes.  CDArray is now simpler.  CFastHeap is now
 * derived from CDArray, and is also simpler.
 * 
 * 8     97/03/17 14:29 Speter
 * Changed AppendString to default to no case conversion (as that's the only way it was ever
 * used).
 * 
 * 7     96/07/31 15:24 Speter
 * Changed <windows.h> to WinInclude.
 * Efficientised calculation of uMaxSizeUsed.
 * 
 * 6     96/07/22 15:25 Speter
 * Added Reset() function taking position parameter.
 * Now initialise pu1CurrentAlloc in constructor.
 * 
 * 5     7/16/96 11:29a Mlange
 * Moved several files to different directories, updated includes.
 * 
 * 4     7/01/96 6:28p Mlange
 * Changed pointers to uint8* instead of void*. Fixed implementation of get current alloc
 * location function.
 * 
 * 3     7/01/96 6:16p Pkeet
 * Added member functions to access protected member values. Changed char*'s to void*'s.
 * 
 * 2     6/12/96 6:59p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "GblInc/Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/Std/MemLimits.hpp"
#include "../FastHeap.hpp"
#include "Lib/Std/StringEx.hpp"
#include <stdlib.h>
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/Sys/DebugConsole.hpp"


//**********************************************************************************************
//
// Global functions.
//

	//*****************************************************************************************
	void* pHeapAlloc
	(
		uint	u_heap_size
	)
	{
		//
		// Get the base address of the allocation region, reserve the region and commit the
		// region for use. Committing the region will only use a minimal amount of physical
		// memory according to the following formula:
		//
		//		Physical Memory Used in Kilobytes = 4 * Region allocated in megabytes
		//
		// So, for example, a 1 Mb region would use 4Kb of physical memory at this step.
		// The first page of memory committed will also be allocated in physical memory at this
		// time. Additional physical memory will subsequently be allocated to the region as
		// pages of the region are first accessed on a page-by-page basis. The normal page size
		// for a pv is 4Kb.
		//
		void* p_heap = VirtualAlloc
		(
			0,							// Address of region to reserve or commit.
			u_heap_size,				// Size of region.
			MEM_RESERVE,
			PAGE_READWRITE 				// Type of access protection.
		);

		//
		// Check for a heap allocation error. Terminate the application with the appropriate
		// message if an error is found.
		//
		if (p_heap == 0)
		{
			char str_num[16];
			char str_out[64] = { "System error reference #" };

			//
			// Supplement error message with the internal Windows error number. AppendString
			// will never write past the end of the memory allocated for the string.
			//
			itoa(GetLastError(), str_num, 10);
			AppendString(str_out, str_num, sizeof(str_out));
			AppendString(str_out, ".", sizeof(str_out));

			// Terminate application.
			TerminalError(ERROR_FASTHEAP_UNABLETOCREATE, false, str_out);
		}

		MEMLOG_ADD_COUNTER(emlHeap, u_heap_size);
//		dprintf("Virtual Heap base = %X\n", (uint32)p_heap);

		return p_heap;
	}

	//*****************************************************************************************
	void HeapFree(void* p_heap, uint u_heap_size)
	{
		//
		// Release the region of reserved and committed pages.
		//
		Verify(VirtualFree(p_heap, u_heap_size, MEM_DECOMMIT));
		Verify(VirtualFree(p_heap, 0, MEM_RELEASE));

		MEMLOG_SUB_COUNTER(emlHeap, u_heap_size);
	}


	//**********************************************************************************************
	void* pvCommitMem(void* pv_start, uint32 u4_amount)
	{
		u4_amount = (u4_amount + u4SystemPageSize() - 1) & ~(u4SystemPageSize()-1);
		AlwaysVerify(VirtualAlloc(pv_start, u4_amount, MEM_COMMIT, PAGE_READWRITE) != 0);

		MEMLOG_ADD_COUNTER(emlCFastHeapCommit, u4_amount);

		return (uint8*)pv_start + u4_amount;
	}


	//**********************************************************************************************
	// NOTE: Decommiting memory has some stange caveats, we could just pass the numbers to
	// VirtualFree which will do the rounding but we need to know what the base address of the
	// decommited block is, or if no memory is decommited what the end of the block is.
	void* pvDecommitMem(void* pv_start, uint32 u4_amount)
	{
		// round pv_start UP to the next page boundary
		void*	pv_start_page = (void*) (((uint32)pv_start + u4SystemPageSize() - 1) & ~(u4SystemPageSize()-1));
		// the number of bytes different between the start address and the page address
		uint32	u4_diff = (uint32)pv_start_page - (uint32)pv_start;

		uint32	u4_decommit = (u4_amount - u4_diff) & ~(u4SystemPageSize()-1);

		// the decommit address should be on a system page boundary
		Assert (((uint32)pv_start_page & (u4SystemPageSize()-1)) == 0);

		if (u4_decommit == 0)
		{
			// we are not going to decommit any physical memory. Either because we have decommited
			// less that a system page or our decommit block straddles two physical pages so neither
			// of them can be decommited.
			// Return the address at the end of the block
			return (uint8*)pv_start + u4_amount;
		}

		MEMLOG_SUB_COUNTER(emlCFastHeapCommit, u4_decommit);
		AlwaysVerify(VirtualFree(pv_start_page,u4_decommit,MEM_DECOMMIT));

		return pv_start_page;		// the beginning of the block actually decommited
	}



