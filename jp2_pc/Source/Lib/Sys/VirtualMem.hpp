/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CVirtualMem
 *
 * To do:
 *		Decommit pages
 *
 * Notes:
 *		Lib/W95/WinInclude.hpp needs to be included before this file, it cannot be included
 *		within this file has it causes compiler erros due to order dependencies.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/VirtualMem.hpp                                                $
 * 
 * 14    9/28/98 10:07p Pkeet
 * Added automatic allocations for the VM loader.
 * 
 * 13    8/25/98 2:33p Rvande
 * removed redundant class scope
 * 
 * 12    6/24/98 3:20p Rwyatt
 * VM can now be syncronously loaded without using the thread loader.
 * 
 * 11    4/28/98 5:00p Rwyatt
 * Added some asserts to verify we do not overrun the page table entries.
 * 
 * 10    4/22/98 7:50p Rwyatt
 * Modified some member variables
 * 
 * 9     4/22/98 4:51p Rwyatt
 * Remove reset and destroy members. If you want to reset the VM system then delete it and
 * re-creaet it.
 * 
 * 8     4/21/98 2:49p Rwyatt
 * Huge change.
 * This is the first implementation of the VM system that agnerates no faults. The loader thread
 * is now part of the VM system and not some external class.
 * 
 * 7     2/21/98 5:51p Rwyatt
 * Added Reset member
 * 
 * 6     2/18/98 1:08p Rwyatt
 * Virtual range mapped by the image file can be set before the file is opened. This was added
 * so we could thread the copy local procedure and allocate virtual memory from the foreground.
 * 
 * 5     1/29/98 7:48p Rwyatt
 * Big chnages:
 * You can now allocate while using a swap file
 * Functions to commit and decommit the whole file.
 * 
 * 4     12/15/97 4:56p Rwyatt
 * Added the VM touch function 
 * 
 * 3     12/11/97 1:40p Rwyatt
 * Protected the exdeption handler with a CriticalSection in case two seperate threads fault at
 * the same time.
 * 
 * 2     12/10/97 8:50p Rwyatt
 * Forground and background faults are counted if memory logging is enabled. This is done
 * regardless of the build mode.
 * 
 * 1     12/04/97 3:20p Rwyatt
 * Initial implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_SYS_VIRTUALMEM
#define HEADER_LIB_SYS_VIRTUALMEM

#include "Lib/Sys/MemoryLog.hpp"

//**********************************************************************************************
#define VM_PAGE_PRESENT		0x00000001		// Page is in memory and can be accessed
#define VM_PAGE_PAGEABLE	0x00000002		// page is pageable and may be removed
#define VM_PAGE_REQUESTED	0x80000000		// Page has been requested, loaded by the VM thread



//**********************************************************************************************
struct SPageTableEntry
// prefix: pte
{
	uint32				u4PageStatus;
	uint32				u4PageFileOffset;
	SPageTableEntry*	ppteYounger;		// Point to the next newest guy
	SPageTableEntry*	ppteOlder;			// Point to the next oldest guy
};


//**********************************************************************************************
//
class CVirtualMem
// prefix: vm
//*************************************
{
public:
	//******************************************************************************************
	class CVMSubAllocator
	{
	public:
		//**************************************************************************************
		CVMSubAllocator
		(
			CVirtualMem*	pvme,
			uint32			u4_length,
			uint32			u4_align = 0		// use parent alignent
		)
		//*****************************
		{
			Assert(pvme);
			Assert(u4_length>0);

			pvmeParent = pvme;
			pu1Base = pu1NextAllocAdr = (uint8*)pvme->pvMalloc(u4_length);
			pu1LastAvailAdr = pu1NextAllocAdr + u4_length;
			if (u4_align)
				u4Alignment = u4_align;
			else
				u4Alignment = pvme->u4GetAlignment();

			MEMLOG_ADD_COUNTER(emlVirtualSubBlocks,1);
			MEMLOG_ADD_COUNTER(emlVirtualSubMem,u4_length);			
		}

		//**************************************************************************************
		~CVMSubAllocator
		(
		)
		//*****************************
		{
			MEMLOG_SUB_COUNTER(emlVirtualSubBlocks,1);
			MEMLOG_SUB_COUNTER(emlVirtualSubMem, (pu1LastAvailAdr-pu1Base) );

			pvmeParent->Free(pu1Base);
		}

		//**************************************************************************************
		void* pvMalloc
		(
			uint32 u4_size
		)
		//*****************************
		{
			uint32	u4_aligned_size = ( (u4_size+(u4Alignment-1)) & ~(u4Alignment-1) );

			if (pu1NextAllocAdr>=pu1LastAvailAdr)
			{
				return NULL;
			}

			MEMLOG_ADD_COUNTER(emlVirtualSubUsed,u4_aligned_size);

			uint8*	pu1_allocate = pu1NextAllocAdr;
			pu1NextAllocAdr+=u4_aligned_size;

			return pu1_allocate;
		}

		//**************************************************************************************
		void Free
		(
			void*
		)
		//*****************************
		{
		}

		//**************************************************************************************
		uint32 u4AllocatorLength()
		{
			return (uint32)(pu1LastAvailAdr-pu1Base);
		}

		//**************************************************************************************
		uint32 u4MemoryAllocated()
		{
			return (uint32)(pu1NextAllocAdr - pu1Base);
		}


	protected:
		CVirtualMem*	pvmeParent;
		uint8*			pu1Base;
		uint8*			pu1NextAllocAdr;
		uint8*			pu1LastAvailAdr;
		uint32			u4Alignment;
	};

	//******************************************************************************************
	CVirtualMem
	(
		uint32	u4_mem_pool,
		uint32	u4_mem_page_size,
		uint32	u4_mem_max_physical,
		uint32	u4_alignment = 32
	);

	//******************************************************************************************
	~CVirtualMem
	(
	);

	//******************************************************************************************
	void CreatePageTable();

	//******************************************************************************************
	void DestroyPageTable();

	//******************************************************************************************
	// Get the virtual base address of this VM manager
	void* pvGetBase()
	{
		return pvBase;
	}


	//******************************************************************************************
	// returns true if the VM system is file based and paging, return false if in initial
	// allocation state (conventional, non-virtual using swap file)
	bool bFileBased()
	{
		return (hFile != NULL);
	}

	//******************************************************************************************
	// Returns true is the VM system has not allocated any memory elsewhere and therefore can
	// be file based.
	bool bAnythingAllocated()
	{
		return (u4AllocateCount != 0);
	}

	//******************************************************************************************
	// Sets the size and region of the file that is going to be demand paged later. 
	void SetPagingRegion
	(
		uint32	u4_file_size
	);


	//******************************************************************************************
	// returns true if the specified address is in the file image part of virtual memory, this
	// is the first part. There may or may not be commited allocations after the file.
	bool bImageAddressValid
	(
		void* pv
	)
	{
		return ((pv >= pvBase) && 
					((char*)pv < ((char*)pvBase + u4FileLength)) );
	}


	//******************************************************************************************
	// returns true if the specified address is in this virtual block
	bool bVirtualAddressValid
	(
		void* pv
	)
	{
		return ((pv >= pvBase) && 
					((char*)pv < ((char*)pvBase + u4Length)) );
	}


	//******************************************************************************************
	void SetLoadPageSize
	(
		uint32	u4_page_size
	);

	//******************************************************************************************
	void SetMaximumPhysical
	(
		uint32	u4_new_maximum
	);

	//******************************************************************************************
	//
	void AutoSetMemory
	(
	);
	//
	// Automatically sets the amount of memory for the VM based on quality, available memory
	// and whether or not Direct3D is in use.
	//
	//*********************************

	//******************************************************************************************
	uint32 u4GetLoadPageSize
	(
	)
	{
		return u4PageSize;
	}

	//******************************************************************************************
	uint32 u4GetMaxPhysicalMem
	(
	)
	{
		return u4MaxPhysical;
	}

	//******************************************************************************************
	bool bCommitToOffset
	(
		uint32	u4_commit_limit				// commit to at least this offset
	);

	//******************************************************************************************
	void DecommitAll();

	//******************************************************************************************
	void CommitAll();

	//******************************************************************************************
	uint32 u4AlignBlock
	(
		uint32	u4_block_size
	)
	{
		return ( (u4_block_size+(u4Alignment-1)) & ~(u4Alignment-1) );
	}

	//******************************************************************************************
	bool bOpenVMImage
	(
		const char*	str_fname,
		uint32	u4_pageable_offset,
		uint32	u4_nonpageable_count
	);

	//******************************************************************************************
	bool bCreateVMImage
	(
		const char*	str_fname
	);

	//******************************************************************************************
	void* pvMalloc
	(
		uint32	u4_block_size
	);

	//******************************************************************************************
	void Free
	(
		void* pv
	);

	//******************************************************************************************
	// Demand pages the specified file name as a virtual image, optionally it will save the
	// image from the currently allocated VM before it demand pages.
	//
	bool bBeginPaging
	(
		const char*		str_filename,
		uint32			u4_pageable_offset = 0,
		uint32			u4_nonpageable_count = 0,
		bool			b_save = false
	);

	//**********************************************************************************************
	uint32 u4GetAlignment()
	{
		return u4Alignment;
	}

	//**********************************************************************************************
	bool bAllPagesPresent()
	{
		return bAllPresent;
	}

	//******************************************************************************************
	void SetAlwaysLoad(bool b_state)
	{
		bAlwaysLoad = b_state;
	}

	//******************************************************************************************
	void SuspendVMLoadThread();

	//******************************************************************************************
	bool bPageFileLoadErrorRecovery();

	//******************************************************************************************
	bool bLoadPageRangeImmediate(uint32 u4_page, uint32 u4_count);

	//******************************************************************************************
	void ResumeVMLoadThread();

	bool bVMLoadThreadSuspended()
	{
		return bThreadSuspended;
	}

	//******************************************************************************************
	static uint32 u4AsyncLoadThread
	(
		uint32	u4_user
	);

	//******************************************************************************************
	// This function checks the page status flags to determine if a range of memory is
	// accessible or not:
	//		VM_PAGE_PRESENT		VM_PAGE_REQUEST				STATUS
	//		===============		===============				======
	//				0					0					FALSE - Page not present
	//				0					1					FALSE - Should never happen
	//				1					0					TRUE  - Page is OK to use
	//				1					1					FALSE - Page has not yet been loaded
	__inline void RequestMemory
	(
		void*		pv_base,
		uint32		u4_len
	)
	//*********************************
	{
		// Check if the request is invalid, it must be within the pageable section of the
		// swap file to be valid.
		Assert( ((uint32)pv_base>=(uint32)pvBase + u4PageableOffset) && ((uint32)pv_base<((uint32)pvFileEnd)) );

		if (bThreadSuspended)
			return;

		// Calculate the start page
		uint32	u4_page = (((uint32)pv_base)-((uint32)pvBase)) >> u4PageShift;
		uint32	u4_count = ( ((((uint32)pv_base)+u4_len-1)-((uint32)pvBase)) >> u4PageShift ) - u4_page + 1;

		// check all covered pages
		while ( u4_count > 0)
		{
			// If this assert fails we have used an invalid page entry, if we continue we shall use
			// an invalid memory address.
			Assert(u4_page<u4Pages);


			// For the logic within the if statement check the above truth table.
			if ((pptePages[u4_page].u4PageStatus & VM_PAGE_PRESENT) == 0)
			{
				Assert( (pptePages[u4_page].u4PageStatus & VM_PAGE_REQUESTED) == 0);

				if (u4PagesCommitted<u4MaxPages)
				{
					// we have not yet used our quota so lets commit another page
					u4PagesCommitted++;
					MEMLOG_ADD_COUNTER(emlVirtualPagedCommit, u4PageSize);
				}
				else
				{
					if (pteOldest.ppteYounger->u4PageStatus & VM_PAGE_REQUESTED)
						return;

					// We have used our quota so therefore we need to decommit the oldest page
					// then commit the required new page.
					VirtualFree(((uint8*)pvBase)+pteOldest.ppteYounger->u4PageFileOffset, u4PageSize, MEM_DECOMMIT);
					pteOldest.ppteYounger->u4PageStatus &= ~(VM_PAGE_PRESENT|VM_PAGE_REQUESTED);

					// Set the new oldest pointer
					pteOldest.ppteYounger = pteOldest.ppteYounger->ppteYounger;
					pteOldest.ppteYounger->ppteOlder = &pteOldest;
				}

				// set the fetch flag now that we have memory for the page that we require
				pptePages[u4_page].u4PageStatus |= (VM_PAGE_PRESENT|VM_PAGE_REQUESTED);

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
	}


	//******************************************************************************************
	__inline bool bIsMemoryPresent
	(
		void*		pv_base,
		uint32		u4_len
	)
	//*********************************
	{
		//
		// If the address is not in our PAGEABLE range we must assume that it exists
		//
		if ( ((uint32)pv_base<(uint32)pvBase+u4PageableOffset) || ((uint32)pv_base>=((uint32)pvFileEnd)) )
			return true;

		// Calculate the start page
		uint32	u4_start_page = (((uint32)pv_base)-((uint32)pvBase)) >> u4PageShift;
		uint32	u4_end_page = ((((uint32)pv_base) + u4_len-1)-((uint32)pvBase)) >> u4PageShift;

		// these are used in the second stage while we update the page links
		uint32	u4_page = u4_start_page;
		uint32	u4_count = u4_end_page - u4_start_page + 1;
		uint32	u4_pages = u4_count;

		// check all covered pages
		while (u4_count)
		{
			// If this assert fails we have used an invalid page entry, if we continue we shall use
			// an invalid memory address.
			Assert(u4_page<u4Pages);

			if ((pptePages[u4_page].u4PageStatus & (VM_PAGE_PRESENT|VM_PAGE_REQUESTED)) != VM_PAGE_PRESENT)
			{
				if (bAlwaysLoad)
				{
					// This will ensure that all the required pages are present and contain valid data. This function
					// will not return until all the requested data is in memory
					return bLoadPageRangeImmediate(u4_page,u4_count);
				}
				return false;
			}

			u4_count--;
			u4_page++;
		}

		//
		// If all the pages are present and we are going to use this page then update the page pointers
		// with in the LRU list.
		//

		while (u4_pages)
		{
			//
			// Detach the current page from the list, this will maintain a valid list
			//
			pptePages[u4_start_page].ppteOlder->ppteYounger = pptePages[u4_start_page].ppteYounger;
			pptePages[u4_start_page].ppteYounger->ppteOlder = pptePages[u4_start_page].ppteOlder;

			//
			// re-attach current page at the end of the list, so it becomes the youngest
			//
			pptePages[u4_start_page].ppteYounger = &pteYoungest;
			pptePages[u4_start_page].ppteOlder = pteYoungest.ppteOlder;
			pteYoungest.ppteOlder->ppteYounger = &pptePages[u4_start_page];
			pteYoungest.ppteOlder = &pptePages[u4_start_page];

			// maintain the page counters
			u4_start_page++;
			u4_pages--;
		}

		return true;
	}


	//******************************************************************************************
	//
	void*				pvBase;					// base address in virtual space
	uint32				u4Length;				// length of the virtual block
	uint32				u4MaxPhysical;			// maximum amount of physical memory that can be used
	uint32				u4MaxPages;				// maximum number of pageable VM pages that can be commited
	uint32				u4PagesCommitted;		// current number of commited pageable VM pages
	uint32				u4PageSize;				// VM page size
	uint32				u4PageShift;			// Shift amount to divide by page size
	uint8*				pu1AllocateBase;		// Next address to be allocated
	uint32				u4FixedCommitCount;		// Total bytes commited from start of VM block
	uint32				u4AllocateCount;		// Total bytes allocated from start of VM block (must be less than above)
	uint32				u4Alignment;			// Allocation alignmant

	HANDLE				hFile;					// handle of the image file
	uint32				u4FileLength;			// length of the image file, can be less than u4Length
	uint32				u4AdjustedFileLen;		// length of the file adjusted to a VM page boundary
	uint32				u4PageableOffset;		// offset in the file where pageable data begins
	uint32				u4RealPageableOffset;	// offset in the file where pageable data begins, this copy does not change
	uint32				u4NonPageableCount;		// number of bytes at the start of the file that is not pageable
	uint32				u4Pages;				// number of VM pages
	bool				bAllPresent;			// set to true if the whole swap file is present in memory;
	void*				pvFileEnd;				// Next address after the swap file.
	bool				bBuffered;				// File is open with buffered reads
	char				strPageFile[MAX_PATH];	// Filename of the page file

	SPageTableEntry*	pptePages;				// pointer to the virtual memory page table entries, one per VM page

	SPageTableEntry		pteOldest;				// oldest guys parent, this guy will get removed first
	SPageTableEntry		pteYoungest;			// youngest guys child, this guy has just beed added

	uint32				u4SysPageSize;			// system hardware page size, usually 4K
	uint32				u4ThreadExitCode;
	uint32				u4ThreadID;
	HANDLE				hThread;
	bool				bThreadSuspended;		// set to true is the thread is suspended
	uint32				u4SleepTime;		
	bool				bAlwaysLoad;			// set to true to load when requested
};


#endif