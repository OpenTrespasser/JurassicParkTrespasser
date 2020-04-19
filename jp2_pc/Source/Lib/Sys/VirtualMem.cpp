/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Implementation of CVirtualMem (VirtualMem.hpp)
 *
 * To do:
 *		Decommit pages
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/VirtualMem.cpp                                                $
 * 
 * 21    10/02/98 5:17p Pkeet
 * Changed allocation for Direct3D.
 * 
 * 20    9/28/98 10:07p Pkeet
 * Added automatic allocations for the VM loader.
 * 
 * 19    9/25/98 7:21p Rwyatt
 * Set Maximum physical does nothing if the new size is the same as the current size
 * 
 * 18    8/28/98 12:00p Asouth
 * loop variable re-scoped
 * 
 * 17    6/24/98 3:20p Rwyatt
 * VM can now be syncronously loaded without using the thread loader.
 * 
 * 16    5/27/98 2:50a Rwyatt
 * Fixed any anything allocated assert in bOpenVMImage. This used to fire even if you had moved
 * the allocate base.
 * 
 * 15    4/23/98 7:56p Rwyatt
 * Added error support for all non buffered reads.
 * 
 * 14    4/22/98 7:49p Rwyatt
 * Max memory no considers the non pageable section. Non-buffered files are back again with a
 * backup that falls back to buffered files.
 * 
 * 13    4/22/98 7:36p Rwyatt
 * Fixed up the suspend and resume functions so they do not rely on the OS to keep the suspend
 * count. Win95/98 get very upset!
 * 
 * 12    4/22/98 4:51p Rwyatt
 * Remove reset and destroy members. If you want to reset the VM system then delete it and
 * re-creaet it.
 * 
 * 11    4/22/98 2:54p Rwyatt
 * Changed the destroy code so it destroys the thread, Reset will recreate the thread. This is
 * safer for when we start to load multiple GRF files.
 * 
 * 10    4/22/98 12:50p Rwyatt
 * Removed the dprintf calls from the thread because OutputDebugString is not thread safe on
 * Win95
 * 
 * 9     4/21/98 2:49p Rwyatt
 * Huge change.
 * This is the first implementation of the VM system that agnerates no faults. The loader thread
 * is now part of the VM system and not some external class.
 * 
 * 8     98.03.22 4:33p Mmouni
 * Disabled FILE_FLAG_NO_BUFFERING until we figure out why commit all read is failing.
 * 
 * 7     2/21/98 5:51p Rwyatt
 * Added Reset member
 * 
 * 6     1/30/98 4:26p Rwyatt
 * Make swap file sharable incase it is being used from the network
 * 
 * 5     1/29/98 7:48p Rwyatt
 * Big chnages:
 * You can now allocate while using a swap file
 * Functions to commit and decommit the whole file.
 * 
 * 4     12/15/97 4:56p Rwyatt
 * VM system has its own copy of the system page size in case the get function does not inline
 * as it is referenced quite often.
 * 
 * 3     12/11/97 1:40p Rwyatt
 * Protected the exdeption handler with a CriticalSection in case two seperate threads fault at
 * the same time.
 * 
 * 2     12/10/97 8:49p Rwyatt
 * Fixed minimum page size bug, it was set to 512 megs when it should have been 512K
 * Put total fault counter and commit counter in exception handler.
 * 
 * 1     12/04/97 3:20p Rwyatt
 * Initial implementation
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "VirtualMem.hpp"
#include "Lib/Std/MemLimits.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "lib/sys/RegInit.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"



//**********************************************************************************************
// This constructor will create a virtual pool of the specified size, this pool will get commited
// as memory gets allocated. no relocation of objects referencing this memory is required.
//
// The alignment parameter controls the byte alignment of every allocated block, by default this
// is 32 bytes to keep all blocks cache aligned, generally this will not waste memory as most
// allocations through will be huge.
//
CVirtualMem::CVirtualMem
(
	uint32	u4_mem_pool,				// total size of virtual memory pool
	uint32	u4_mem_page_size,			// load page size
	uint32	u4_mem_max_physical,		// maximum physical memory
	uint32	u4_alignment				// alignment of allocations
)
//*************************************
{
	Assert (bPowerOfTwo(u4_alignment));

	u4Alignment = u4_alignment;

	//
	// We have our own copy of the system page size, just in case the call does not inline.
	// This would be really bad as the system page size is used all of the time
	//
	u4SysPageSize = u4SystemPageSize();

	hFile					= NULL;
	hThread					= NULL;
	bThreadSuspended		= true;
	bAlwaysLoad				= false;
	pptePages				= NULL;
	u4NonPageableCount		= 0;
	u4FixedCommitCount		= 0;
	u4AllocateCount			= 0;

	u4FileLength			= 0;
	u4PageableOffset		= u4_mem_pool;	// We start in a state that assumes all memory is present
	u4RealPageableOffset	= 0;
	u4AdjustedFileLen		= 0;
	pptePages				= NULL;
	u4Pages					= 0;
	bAllPresent				= true;
	u4PagesCommitted		= 0;
	u4SleepTime				= 75;	// initially we do not sleep

	// set the inital page load sizes and the amount of physical memory that can be used
	// for commited memory.
	SetLoadPageSize(u4_mem_page_size);
	//SetMaximumPhysical(u4_mem_max_physical);
	AutoSetMemory();

	// reserve a block of VM of the specified size
	//pu1AllocateBase = (uint8*)VirtualAlloc(NULL,u4_mem_pool,MEM_RESERVE,PAGE_READWRITE);

	//TODO using new[] is a quickfix until VirtualMem is repaired and VirtualAlloc can be used again
	//TODO also see destructor below
	pu1AllocateBase = new uint8[u4_mem_pool];
	Assert(pu1AllocateBase != NULL);

	MEMLOG_ADD_COUNTER(emlVirtualPool,u4_mem_pool);

	pvBase		= pu1AllocateBase;
	u4Length	= u4_mem_pool;


	bThreadSuspended		= true;
	pu1AllocateBase			= (uint8*)pvBase;
	pvFileEnd				= pvBase;

	//
	// Set this to force the thread to exit. Do not kill the thread by force, let it exit
	// in the normal way.
	//
	u4ThreadExitCode	= 0;
	u4ThreadID			= 0;

	// Create the thread that will do the processing, initially it is suspended
	hThread = CreateThread( 
				NULL,							// no security attributes 
				0,								// use default stack size  
				(LPTHREAD_START_ROUTINE) u4AsyncLoadThread,
				(void*)this,					// argument to thread function, class this pointer
				CREATE_SUSPENDED,				// create running
				(ULONG*)&u4ThreadID);			// returns the thread identifier

	Verify(hThread);
	dprintf("Thread Create: CVirtualMem [Thread handle 0x%x]\n", u4ThreadID);
}



//**********************************************************************************************
//
CVirtualMem::~CVirtualMem
(
)
//*************************************
{
	// Kill a thread if one is present
	if (hThread)
	{
		// Terminate the thread.
		u4ThreadExitCode = 0xffffffff;
		ResumeVMLoadThread();

		//
		// Wait for the thread to become signalled, ie, when it finishes
		//
		WaitForSingleObject(hThread, 1000);
		CloseHandle(hThread);
		hThread = NULL;
	}

	bThreadSuspended = true;

	// kill the paging file
	if (hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}

	// remove any committed memory
	if (pvBase)
	{
		//Verify( VirtualFree(pvBase, u4Length, MEM_DECOMMIT) );

		//TODO using delete[] is a quickfix until VirtualMem is repaired and VirtualFree can be used again
		//TODO also see constructor above
		delete[] static_cast<uint8*>(pvBase);
		pvBase = nullptr;
	}

	// there is no commited memory any more, and the virtual pages require no memory
	MEMLOG_SUB_COUNTER(emlVirtualFixedCommit, u4FixedCommitCount);
	MEMLOG_SUB_COUNTER(emlVirtualPagedCommit, u4PagesCommitted*u4PageSize);

	// destroy any memory used by the page tables and set the LRU pointers to NULL.
	DestroyPageTable();

	if (pvBase)
	{
		Verify( VirtualFree(pvBase, 0, MEM_RELEASE) );
		MEMLOG_SUB_COUNTER(emlVirtualPool,u4Length);
		pvBase = NULL;
	}
}


//**********************************************************************************************
// Sets the size of the file that is going to be demand paged later. 
// Once this has been called virtual memory can be allocated using the vpMalloc function.
void CVirtualMem::SetPagingRegion
(
	uint32	u4_file_size
)
//*************************************
{
	Assert(bAnythingAllocated() == false);
	Assert(bThreadSuspended);

	if (u4_file_size<512*1024)
	{
		u4_file_size = 512*1024;
	}

	u4FileLength		= u4_file_size;
	u4AdjustedFileLen	= ((u4_file_size + (u4PageSize-1)) & ~(u4PageSize-1));
	// the new base for allocations is after the file.
	pu1AllocateBase = (uint8*)(pvBase) + u4AdjustedFileLen;

	// the last address backed by the swap file
	pvFileEnd = (char*)(((uint32)pvBase) + u4_file_size);
}


//**********************************************************************************************
// Create the page table entries for the paging area of the VM and create the LRU tables for the
// pageable section. This function also commits and loads the nonpageable section.
//
// NOTE:
//		The memory (if any) between the end of the nonpageable section and the start of the 
//		pageable section is not commited and therefore cannot be accessed.
void CVirtualMem::CreatePageTable
(
)
//*************************************
{
	// There is already a set of page tables, lets delete them
	if (pptePages)
	{
		DestroyPageTable();
	}

	Assert(bThreadSuspended);
	Assert(u4PagesCommitted == 0);

	Assert((u4AdjustedFileLen % u4PageSize) == 0 );
	u4Pages = u4AdjustedFileLen / u4PageSize;

	uint32 u4_deduct = ((u4NonPageableCount + (u4PageSize-1)) & ~(u4PageSize-1));
	Assert( (u4_deduct%u4PageSize) == 0);

	//
	// The maximum number of pageable physical pages is the max allowed physical memory minus
	// the non-pageable section size. At worst case a single VM page of memory will be wasted.
	//
	u4MaxPages	= (u4MaxPhysical-u4_deduct) / u4PageSize;

	pptePages = new SPageTableEntry[u4Pages];
	MEMLOG_ADD_COUNTER(emlVirtualPages,u4Pages*sizeof(SPageTableEntry));

	memset(pptePages,0,u4Pages*sizeof(SPageTableEntry));

	// fill i the fileoffsets for each page
	uint32 u4;
	for (u4=0;u4<u4Pages;u4++)
	{
		pptePages[u4].u4PageFileOffset = u4*u4PageSize;
	}

	//
	// Now we have to commit the first part of the file which is no pageable
	//

	bAllPresent	= false;
	u4PageableOffset = u4RealPageableOffset;

	Assert(hFile);
	uint32 u4_fixed_pages = 0;

	if (u4NonPageableCount>0)
	{
		// Number of vm pages that be commited
		u4_fixed_pages = ((u4NonPageableCount + (u4PageSize-1)) & ~(u4PageSize-1)) / u4PageSize;

		// allocate the memory
		Verify(VirtualAlloc(pvBase, u4_fixed_pages*u4PageSize, MEM_COMMIT, PAGE_READWRITE));

		dprintf("Committing non-pageable section of swap file\n");
		// Read the first section of the file
		uint32 u4_read = 0;

		//
		// Read the non-pageable data
		//
		SetFilePointer(hFile,0,NULL,FILE_BEGIN);
		if (ReadFile(hFile, pvBase, u4_fixed_pages*u4PageSize, (unsigned long*)&u4_read, NULL) == false)
		{
			// Read file has failed, try to recover and try again
			if (!bPageFileLoadErrorRecovery())
			{
				AlwaysAssert(0);
			}

			// try the read file again, this time it should work because all optimizations have been removed
			// from the file handle. hFile is now a standard file handle.
			SetFilePointer(hFile,0,NULL,FILE_BEGIN);
			if (ReadFile(hFile, pvBase, u4_fixed_pages*u4PageSize, (unsigned long*)&u4_read, NULL) == false)
			{
				AlwaysAssert(0);
			}
		}

		AlwaysAssert(u4_read==u4_fixed_pages*u4PageSize);

		for (u4 = 0; u4<u4_fixed_pages; u4++)
		{
			pptePages[u4].u4PageStatus |= VM_PAGE_PRESENT;
		}
	}

	uint32 u4_pageable_start = ((u4PageableOffset + (u4PageSize-1)) & ~(u4PageSize-1)) / u4PageSize;

	// If the first pageable byte is in a non-pageable page then we must start on the next page
	// as it would not be good to decommit a non-pageable page!.
	if (u4_pageable_start<u4_fixed_pages)
		u4_pageable_start = u4_fixed_pages;

	// Mark all pageable pages
	for (u4 = u4_pageable_start; u4<u4Pages; u4++)
	{
		pptePages[u4].u4PageStatus |= VM_PAGE_PAGEABLE;
	}

	//
	// Setup the initial LRU
	//
	pteOldest.ppteYounger	= &pteYoungest;
	pteOldest.ppteOlder		= NULL;
	pteYoungest.ppteYounger	= NULL;
	pteYoungest.ppteOlder	= &pteOldest;
}



//**********************************************************************************************
//Deletes the page table
void CVirtualMem::DestroyPageTable
(
)
//*************************************
{
	Assert(bThreadSuspended);

	MEMLOG_SUB_COUNTER(emlVirtualPages,u4Pages*sizeof(SPageTableEntry));
	delete[] pptePages;
	pptePages = NULL;

	//
	// Invalidate the LRU in case it is used
	//
	pteOldest.ppteYounger	= NULL;
	pteOldest.ppteOlder		= NULL;
	pteYoungest.ppteYounger	= NULL;
	pteYoungest.ppteOlder	= NULL;
}



//**********************************************************************************************
// This will set the load page size to the specified amount.
// The amount must be a power of 2 and it must be larger than the sector size of the drive that
// the app is running from.
//
void CVirtualMem::SetLoadPageSize
(
	uint32	u4_page_size
)
//*************************************
{
	Assert(bPowerOfTwo(u4_page_size));
	Assert(u4_page_size>0);

	bool b_suspend = bVMLoadThreadSuspended();
	SuspendVMLoadThread();

	uint32 u4_shift = 0;
	uint32 u4_count = u4_page_size;

	// calcualte the shift factor for this page size
	while (u4_count)
	{
		u4_shift++;
		Assert(u4_shift<32);
		u4_count>>=1;
	}

	u4PageShift = u4_shift-1;			// u4_shift is the number of bits we shifted to give zero, ie 1 too many
	u4PageSize	= u4_page_size;
	u4AdjustedFileLen = ((u4FileLength + (u4PageSize-1)) & ~(u4PageSize-1));

	if (pptePages)
	{
		DecommitAll();
		CreatePageTable();
	}

	// If the thread was going then start it again
	if (!b_suspend)
		ResumeVMLoadThread();
}



//**********************************************************************************************
// This sets the amount of physical memory that the VM system can use. The amount of memory that
// cam be used for pageable data is the total minus the non-pageable section size.
//
void CVirtualMem::SetMaximumPhysical
(
	uint32	u4_new_maximum
)
//*************************************
{
	// Do nothing if we are not changing the amount of physical memory.
	if (u4_new_maximum == u4MaxPhysical)
		return;

	bool b_suspend = bVMLoadThreadSuspended();
	SuspendVMLoadThread();

	uint32 u4_deduct = ((u4NonPageableCount + (u4PageSize-1)) & ~(u4PageSize-1));
	u4MaxPhysical = u4_new_maximum;

	Assert( (u4_deduct%u4PageSize) == 0);
	Assert(u4_new_maximum>u4_deduct);
	Assert( (u4MaxPhysical%u4PageSize) == 0);

	if (pptePages)
	{
		DecommitAll();
		// maximum number of pageable physical pages
		u4MaxPages	= (u4MaxPhysical-u4_deduct) / u4PageSize;
	}

	// If the thread was going then start it again
	if (!b_suspend)
		ResumeVMLoadThread();

	dprintf("Setting VM max physical mem: %ldMb\n", u4MaxPhysical >> 20);
}



//**********************************************************************************************
// Memory while being allocated can be commited in units of a memory page, this is different
// from commited memory from an image file.
//
bool CVirtualMem::bCommitToOffset
(
	uint32	u4_commit_limit				// commit to at least this offset
)
//*************************************
{
	// the page size must be set...
	Assert(u4SysPageSize);

	// we have already commited enough memory to satisfy this request,,,
	if (u4_commit_limit<=u4FixedCommitCount)
		return true;

	// the commited range at the moment must be a multiple of the system page size
	Assert((u4FixedCommitCount % u4SysPageSize) == 0);

	uint32	u4_amount = u4_commit_limit - u4FixedCommitCount;

	u4_amount = (u4_amount + u4SysPageSize - 1) & ~(u4SysPageSize-1);

	// virtualAlloc would round address and byte count to page boundaries, but this does not
	// matter as we pass in rounded addresses.
	if (VirtualAlloc(pu1AllocateBase + u4FixedCommitCount, u4_amount, MEM_COMMIT, PAGE_READWRITE) == NULL)
	{
		return false;
	}

	MEMLOG_ADD_COUNTER(emlVirtualFixedCommit,u4_amount);
	u4FixedCommitCount+=u4_amount;

	return true;
}



//**********************************************************************************************
// This will decommit all the memory used within this class.
// This memory is either thememory that has been allocated while we are not pageing which is
// continous and all commited. Otherwise, it is the memory that is present while we are paging,
// this memory is memory is continous but is not all present. If we decommit the whole virtual
// block the kernel does the rest.
// If the paging system is active this will flush the logical page tables and rest them to their
// initial state.
//
void CVirtualMem::DecommitAll
(
)
//*************************************
{
	// if we do not have an image file we cannot commit, allocated virtual memory is always commited
	Assert (hFile);

	uint32 u4_pages = 0;

	// we must have logical pages.
	Assert(pptePages != NULL);

	// While we decommit the memory we need to stop the loader thread
	bool b_suspend = bVMLoadThreadSuspended();
	SuspendVMLoadThread();

	bAllPresent		= false;
	u4PageableOffset = u4RealPageableOffset;

	//
	// Decommit the pageable section of the virtual image
	//

	// go through all the pages
	for (uint32 u4=0;u4<u4Pages;u4++)
	{
		// is this page pageable??
		if (pptePages[u4].u4PageStatus & VM_PAGE_PAGEABLE)
		{
			// is this page present??
			if (pptePages[u4].u4PageStatus & (VM_PAGE_PRESENT|VM_PAGE_REQUESTED))
			{
				u4_pages++;

				// page is present so remove it
				MEMLOG_SUB_COUNTER(emlVirtualPagedCommit, u4PageSize);

				VirtualFree(((uint8*)pvBase)+pptePages[u4].u4PageFileOffset, u4PageSize, MEM_DECOMMIT);
				u4PagesCommitted--;

				// page is now not present
				pptePages[u4].u4PageStatus&=~(VM_PAGE_PRESENT|VM_PAGE_REQUESTED);
				pptePages[u4].ppteYounger = NULL;
				pptePages[u4].ppteOlder = NULL;
			}
		}
	}

	if (u4PagesCommitted !=0)
	{
		dprintf("PROBLEM\n");
	}

	u4PagesCommitted = 0;
	//
	// Setup the initial LRU
	//
	pteOldest.ppteYounger	= &pteYoungest;
	pteOldest.ppteOlder		= NULL;
	pteYoungest.ppteYounger	= NULL;
	pteYoungest.ppteOlder	= &pteOldest;


	dprintf("%d VM pages decommited (%d bytes).\n", u4_pages, u4_pages*u4PageSize);
}


//**********************************************************************************************
// This will commit the whole virtual page file, if we are not paging this function does
// nothing.
//
void CVirtualMem::CommitAll
(
)
//*************************************
{
	// if we do not have an image file we cannot commit, allocated virtual memory is always commited
	Assert (hFile);

	// we must have logical pages.
	Assert(pptePages != NULL);

	uint32 u4_commit = 0;
	uint32 u4_read;

	// If we are commiting all the memory then we may as well stop the loader thread
	SuspendVMLoadThread();

	dprintf("Commiting pageable swap swap file..\n");

//	SetMaximumPhysical(u4_length);
//	What about the commit count???????
		
	// go through all the pages
	for (uint32 u4=0;u4<u4Pages;u4++)
	{
		// is this page pageable??
		if (pptePages[u4].u4PageStatus & VM_PAGE_PAGEABLE)
		{
			// is this page present??
			if (!(pptePages[u4].u4PageStatus & VM_PAGE_PRESENT))
			{
				// page is not present so load it....
				MEMLOG_ADD_COUNTER(emlVirtualPagedCommit, u4PageSize);
				u4_commit++;

				// commit this page
				VirtualAlloc(((uint8*)pvBase)+pptePages[u4].u4PageFileOffset, u4PageSize, MEM_COMMIT, PAGE_READWRITE);
				u4PagesCommitted++;

				// load this page from the swap file
				SetFilePointer(hFile,pptePages[u4].u4PageFileOffset,NULL,FILE_BEGIN);
				if (ReadFile(hFile, ((uint8*)pvBase)+pptePages[u4].u4PageFileOffset, u4PageSize, (unsigned long*)&u4_read, NULL) == false)
				{
					// Read file has failed, try to recover and try again
					if (!bPageFileLoadErrorRecovery())
					{
						AlwaysAssert(0);
					}

					// Load the same page again from the new file handle, this time is should not fail.
					SetFilePointer(hFile,pptePages[u4].u4PageFileOffset,NULL,FILE_BEGIN);
					if (ReadFile(hFile, ((uint8*)pvBase)+pptePages[u4].u4PageFileOffset, u4PageSize, (unsigned long*)&u4_read, NULL) == false)
					{
						AlwaysAssert(0);
					}
				}

				// page is now present
				pptePages[u4].u4PageStatus|=VM_PAGE_PRESENT;
			}
		}

		// While all is committed there is no need to have the requested flag set, also the thread is stopped
		pptePages[u4].u4PageStatus&=~VM_PAGE_REQUESTED;
	}

	// If all the memory is present then fiddle the pageable offset so there is not one, this helps
	// out the renderer because it does not have to explicitally check if the VM has all the pages
	// present.
	bAllPresent	= true;
	u4PageableOffset = u4Length;

	dprintf("%d VM pages commited (%d bytes).\n", u4_commit, u4_commit*u4PageSize);
}

//**********************************************************************************************
//
void* CVirtualMem::pvMalloc
(
	uint32 u4_size						// size of the allocation
)
//*************************************
{
	Assert(u4_size>0);
	Assert(pu1AllocateBase != NULL);

	uint32	u4_aligned_size = u4AlignBlock(u4_size);

	// make sure there is enough commited memory for this allocation
	if (!bCommitToOffset(u4AllocateCount+u4_aligned_size))
		return NULL;

	uint8*	pu1_allocate = pu1AllocateBase+u4AllocateCount;
	u4AllocateCount+=u4_aligned_size;

	return pu1_allocate;
}

//**********************************************************************************************
// We cannot free a block allocated with the the VM malloc above. 
//
void CVirtualMem::Free
(
	void*	pv
)
//*************************************
{
}


//**********************************************************************************************
// This is the place where the virtual demand paging system starts,,,
// This will save the currently allocated virtual memory map as a binary file to the hard disk
// and then decommit all of our virtual allocated memory.
// NOTE: This can only be called if we constructed a virtual class with an allocator, if we 
// started with the file allocator this has alrady been done and this function will return
// false.
//
bool CVirtualMem::bBeginPaging
(
	const char*			str_filename,		// file name to use for image
	uint32				u4_pageable_offset,
	uint32				u4_nonpageable_count,
	bool				b_save				// should we save the existing image
)
//*************************************
{
	//
	// do we have to save the image before we page it?
	//
	if (b_save)
	{
		if (!bCreateVMImage(str_filename))
		{
			return false;
		}

		// decommit any memory that we may have had loaded, this will cause any access to a texture
		// page to cause a fault.
		VirtualFree(pvBase, u4Length, MEM_DECOMMIT);
		u4AllocateCount = 0;
	}

	if (bOpenVMImage
		(
			str_filename,
			u4_pageable_offset,
			u4_nonpageable_count
		) == false)
	{
		return false;
	}

	return true;
}


//**********************************************************************************************
//
bool CVirtualMem::bCreateVMImage
(
	const char*	str_fname
)
//*************************************
{
	HANDLE	h_save;

	dprintf("Creating Swap file....\n");

	// if we have not allocated an VM then we cannot save.
	if (u4AllocateCount == 0)
		return false;

	// if we do not have an allocate base then we cannot save
	Assert(pu1AllocateBase != NULL)

	h_save=CreateFile(str_fname,GENERIC_WRITE,0,NULL,
				CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,NULL);

	if (h_save==INVALID_HANDLE_VALUE)
	{
		dprintf("Cannot open VM image file for saving..\n");
		return(false);
	}

	uint32	u4_bytes_saved;

	if (WriteFile(h_save,pu1AllocateBase,u4FixedCommitCount,(DWORD*)&u4_bytes_saved,NULL) == false)
	{
		dprintf("VM Image WriteFile has failed..\n");
		return false;
	}

	if (u4_bytes_saved != u4FixedCommitCount)
	{
		dprintf("VM Image WriteFile did not write the correct number of bytes\n");
		return false;
	}

	// we now have a file image of our virtual memory
	CloseHandle(h_save);

	dprintf("Swap file Created. (%d bytes)\n", u4FixedCommitCount);

	return true;
}


//**********************************************************************************************
//
bool CVirtualMem::bOpenVMImage
(
	const char*	str_fname,
	uint32	u4_pageable_offset,
	uint32	u4_nonpageable_count
)
//*************************************
{
	uint32	u4_file_size;

	Assert(bThreadSuspended);

	//
	// No allocatiosn can have been made before we go to an image file,
	// allocations can be made after. There can be no existing image file
	//
	Assert (hFile == NULL);

	// If we have not moved the allocate base then we must not have allocated any VM.
	if (pvBase == pu1AllocateBase)
		Assert (u4AllocateCount == 0);


	//
	// Get the sector size of the drive holding the current directoy
	//
	uint32	u4_spc;		// Sectors per cluster
	uint32	u4_bps;		// bytes per sector
	uint32	u4_free;	// free clusters
	uint32	u4_total;	// total clusters
	bool	b_sector = GetDiskFreeSpace(NULL, (DWORD*)&u4_spc, (DWORD*)&u4_bps, (DWORD*)&u4_free, (DWORD*)&u4_total);

	// Keep a copy of the filename incase the thread needs to re-open it
	strcpy(strPageFile, str_fname);

	//
	// If the sector size of the current drive is larger that the load size
	// we cannot use non buffered reads as the disk address must be a multiple
	// of the sector size. Our default load size is 64K so this is unlikely.
	//	
	if ( (b_sector) && ((u4PageSize & u4_bps)==0) )
	{
		//
		// We successfully got the sector size and it is valid for our load block
		// size, open with no caching or buffering...
		//
		hFile = CreateFile
				(
					str_fname,
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_READONLY | FILE_FLAG_NO_BUFFERING,
					NULL
				);
		bBuffered = false;
	}
	else
	{
		//
		// we have either failed to get the sector size of the size is too big,
		// in either case open the file with buffering
		//
		hFile = CreateFile
				(
					str_fname,
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_READONLY,
					NULL
				);
		bBuffered = true;
	}

	if (hFile==INVALID_HANDLE_VALUE)
	{
		hFile = NULL;

		//
		// try to recover from the open file error if we opened the file as buffered
		//
		if ((bBuffered) || bPageFileLoadErrorRecovery() == false)
		{
			dprintf("Cannot open VM image file for loaing..\n");
			return(false);
		}
	}

	u4_file_size = GetFileSize(hFile,NULL);

	//
	// If we have not adjusted the allocation pointer past the end of the pageable
	// section do it now. SetPagingRegion can be called earlier if allocations
	// are required before the page file can be setup.
	//
	if (pu1AllocateBase == pvBase)
	{
		SetPagingRegion(u4_file_size);
	}

	u4PageableOffset = u4_pageable_offset;
	u4RealPageableOffset = u4_pageable_offset;
	u4NonPageableCount = u4_nonpageable_count;

	CreatePageTable();

	return true;
}


//**********************************************************************************************
// This function will try to recover from a page file load error
bool CVirtualMem::bPageFileLoadErrorRecovery
(
)
//*************************************
{
	// read file has failed....
	if (bBuffered)
	{
		// buffered reads should never fail, this is a serious error. If we just ignore it
		// (Assert in non final builds) it will render the texture that it covers usesless
		// and the next best texture will be used. This is better than crashing.
		AlwaysAssert(0);
		return false;
	}
	else
	{
		dprintf("Unbuffered page file read has failed, adjusting file parameters\n");
		// Unbuffered reads sometimes fail, lets close the unbuffered file and open it again
		// buffered,

		if (hFile)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}

		bBuffered = true;

		//
		// Open the file again as a normal cached file
		//
		hFile = CreateFile
		(
			strPageFile,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_READONLY,
			NULL
		);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			//
			// We have failed to re-open the file so terminate the thread.
			//
			hFile = NULL;
			return false;
		}
	}

	return true;
}


//**********************************************************************************************
void CVirtualMem::SuspendVMLoadThread
(
)
//*************************************
{
	if ((hThread) && (!bThreadSuspended))
	{
		// suspend the loader thread so it is safe to remove its data.
		bThreadSuspended = true;
		SuspendThread(hThread);

		dprintf("VM load thread suspended.\n");
	}
}


//**********************************************************************************************
void CVirtualMem::ResumeVMLoadThread
(
)
//*************************************
{
	if ((hThread) && (bThreadSuspended))
	{
		// Keep resuming the thread until it is running
		ResumeThread(hThread);
		bThreadSuspended = false;

		dprintf("VM load thread resumed.\n");
	}
}


//**********************************************************************************************
// This does almost the same work as the threaded loader but synchronously. This will also
// maintain this linked list.
// It is safe for the background thread to be going at the same time as this function is
// going.
bool CVirtualMem::bLoadPageRangeImmediate
(
	uint32	u4_page, 
	uint32	u4_count
)
//*************************************
{
	uint32 u4_read;

	// check all covered pages
	while ( u4_count > 0)
	{
		// If this assert fails we have used an invalid page entry, if we continue we shall use
		// an invalid memory address.
		Assert(u4_page<u4Pages);

		if ((pptePages[u4_page].u4PageStatus & VM_PAGE_PRESENT) == 0)
		{
			if (u4PagesCommitted<u4MaxPages)
			{
				// we have not yet used our quota so lets commit another page
				u4PagesCommitted++;
				MEMLOG_ADD_COUNTER(emlVirtualPagedCommit, u4PageSize);
			}
			else
			{
				// The oldest guy in our linked list is requested so therefore the
				// whole list must be requested, therefore we have more that our
				// pageable quota in a given scene.
				if (pteOldest.ppteYounger->u4PageStatus & VM_PAGE_REQUESTED)
					return false;

				// We have used our quota so therefore we need to decommit the oldest page
				// then commit the required new page.
				VirtualFree(((uint8*)pvBase)+pteOldest.ppteYounger->u4PageFileOffset, u4PageSize, MEM_DECOMMIT);
				pteOldest.ppteYounger->u4PageStatus &= ~(VM_PAGE_PRESENT|VM_PAGE_REQUESTED);

				// Set the new oldest pointer
				pteOldest.ppteYounger = pteOldest.ppteYounger->ppteYounger;
				pteOldest.ppteYounger->ppteOlder = &pteOldest;
			}

			pptePages[u4_page].u4PageStatus &= ~VM_PAGE_REQUESTED;
			pptePages[u4_page].u4PageStatus |= VM_PAGE_PRESENT;

			VirtualAlloc(((uint8*)pvBase)+pptePages[u4_page].u4PageFileOffset, 
				u4PageSize, MEM_COMMIT, PAGE_READWRITE);

			while (1)
			{
				// load this block of pages from the swap file
				SetFilePointer(hFile,pptePages[u4_page].u4PageFileOffset,NULL,FILE_BEGIN);

				if (ReadFile(hFile, ((uint8*)pvBase)+pptePages[u4_page].u4PageFileOffset, 
					u4PageSize, (unsigned long*)&u4_read, NULL) == false)
				{
					if (!bPageFileLoadErrorRecovery())
					{
						// If we fail to recover then there is nothing that we can do as the swap file is
						// now probably closed. If we just stop the thread the game will continue to work
						// using the non-pageable data.
						AlwaysAssert(0);
						return false;
					}
				}
				else
				{
					// If the read succeeded then break out of the loop
					break;
				}
			}

			// Insert this page at the end of the LRU
			pptePages[u4_page].ppteYounger = &pteYoungest;
			pptePages[u4_page].ppteOlder = pteYoungest.ppteOlder;

			// Point the younest to us.
			pteYoungest.ppteOlder->ppteYounger = &pptePages[u4_page];
			pteYoungest.ppteOlder = &pptePages[u4_page];
		}
		else
		{
			//
			// This page was already present so move it to the end of the LRU
			//
			pptePages[u4_page].ppteOlder->ppteYounger = pptePages[u4_page].ppteYounger;
			pptePages[u4_page].ppteYounger->ppteOlder = pptePages[u4_page].ppteOlder;
			pptePages[u4_page].ppteYounger = &pteYoungest;
			pptePages[u4_page].ppteOlder = pteYoungest.ppteOlder;
			pteYoungest.ppteOlder->ppteYounger = &pptePages[u4_page];
			pteYoungest.ppteOlder = &pptePages[u4_page];
		}

		u4_count--;
		u4_page++;
	}

	return true;
}


//**********************************************************************************************
// The actual async (threaded) loader.
//
// This will walk through all the pages in the VM system and load any that have the requested
// bit set in the pagetbales.
//
uint32 CVirtualMem::u4AsyncLoadThread
(
	uint32	u4_user
)
//*************************************
{
	uint32			u4_page;
	uint32			u4_base;
	uint32			u4_loaded;
	uint32			u4_read;
	uint32			u4_count;
	uint32			u4_set;

	CVirtualMem*	pvme = (CVirtualMem*)u4_user;

	dprintf("CVirtualMem Thread Enter\n");

	while (pvme->u4ThreadExitCode == 0)
	{
		u4_page = 0;
		u4_loaded = 0;
		while ((u4_page<pvme->u4Pages) && (pvme->u4ThreadExitCode == 0))
		{
			if (pvme->pptePages[u4_page].u4PageStatus & VM_PAGE_REQUESTED)
			{
				u4_base = u4_page;			// remember the base page
				u4_count = 1;
				u4_page++;

				// Before we load this page check the following pages to see if they also need loading.
				// If they do we can load them all in one go.
				while ((u4_page<pvme->u4Pages) && (pvme->pptePages[u4_page].u4PageStatus & VM_PAGE_REQUESTED))
				{
					u4_count++;
					u4_page++;
				}

				//dprintf("Load %d pages..\n",u4_count);
				// Commit required number of pages
				VirtualAlloc(((uint8*)pvme->pvBase)+pvme->pptePages[u4_base].u4PageFileOffset, 
					pvme->u4PageSize*u4_count, MEM_COMMIT, PAGE_READWRITE);

				// load this page from the swap file
				SetFilePointer(pvme->hFile,pvme->pptePages[u4_base].u4PageFileOffset,NULL,FILE_BEGIN);

				if (ReadFile(pvme->hFile, ((uint8*)pvme->pvBase)+pvme->pptePages[u4_base].u4PageFileOffset, 
					pvme->u4PageSize*u4_count, (unsigned long*)&u4_read, NULL) == false)
				{
					if (!pvme->bPageFileLoadErrorRecovery())
					{
						// If we fail to recover then there is nothing that we can do as the swap file is
						// now probably closed. If we just stop the thread the game will continue to work
						// using the non-pageable data.
						return 0xffffffff;
					}
					// try these pages again
					u4_page = u4_base;
					continue;
				}

				// For all pages loaded clear the requested flag
				for (u4_set = u4_base; u4_set<u4_page; u4_set++)
				{
					// clear the requested flag to make the page valid
					pvme->pptePages[u4_set].u4PageStatus &= ~VM_PAGE_REQUESTED;
				}

				u4_loaded+=u4_count;
				continue;
			}

			u4_page++;
		}

		//	We have been all the way through the cache and not loaded anything so it is time to
		//	sleep for u4SleepTime (75ms). By then more data should have been requested.
		if ((u4_loaded == 0) && (pvme->u4ThreadExitCode == 0))
			Sleep(pvme->u4SleepTime);
	}

	dprintf("CVirtualMem Thread Leave\n");
	return pvme->u4ThreadExitCode;
}

//******************************************************************************************
void CVirtualMem::AutoSetMemory()
{
	int i_mb = 0;	// Amount of memory to allocate in megabytes.

	// Set available memory based on quality.
	switch (iGetQualitySetting())
	{
		case 0:
			i_mb += 8;
			break;

		case 1:
			i_mb += 10;
			break;

		case 2:
			i_mb += 10;
			break;

		case 3:
			i_mb += 12;
			break;

		default:
			AlwaysAssert(0);
		case 4:
			i_mb += 16;
			break;
	}

	uint32 u4_mb_phys = u4TotalPhysicalMemory() >> 20;

	// Do nothing more if memory is limited.
	if (u4_mb_phys < 56)
	{
		SetMaximumPhysical(i_mb << 20);
		return;
	}

	// Add for large memory support for systems without hardware.
	if (!bGetD3D())
	{
		i_mb += (u4_mb_phys - 32) / 2;
		SetMaximumPhysical(i_mb << 20);
		return;
	}

	// Add memory for systems with hardware and 96Mb memory or more.
	if (u4_mb_phys < 90)
	{
		SetMaximumPhysical(i_mb << 20);
		return;
	}

	i_mb += (u4_mb_phys - 64) / 2;
	SetMaximumPhysical(i_mb << 20);
}
