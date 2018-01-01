/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of the async loading support functions and actions.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/AsyncLoader.cpp                                           $
 * 
 * 12    10/02/98 9:51p Rwyatt
 * #if 0 whole file, not required
 * 
 * 11    4/21/98 3:09p Rwyatt
 * Old async loader is no longer required
 * 
 * 10    2/19/98 2:21p Agrant
 * Keep the load list just a little smaller than the max to be extra safe.
 * 
 * 9     2/17/98 12:34p Agrant
 * Some protection in case the prefetcher tries to exceed its allowed prefetch queue.
 * 
 * 8     2/13/98 6:38p Agrant
 * Support for removing touch blocks before deleting the Data Daemon
 * 
 * 7     12/15/97 5:00p Rwyatt
 * Major change:
 * We have gone from a threaded action loader to a loader with its own implementation. The was
 * done to impreove the prefetch times.
 * All references to the TouchAction have been removed
 * 
 * 6     12/04/97 4:12p Rwyatt
 * Change in header files and the system page size is obtained by calling u4SystemPageSize()
 * 
 * 5     11/14/97 12:07a Rwyatt
 * Removed voice over loader
 * 
 * 4     10/13/97 10:37p Agrant
 * Touching Fetchables now uses the fetchable callback (OnPrefetch())
 * 
 * 3     10/03/97 5:19p Rwyatt
 * Implemented basic actions for audio voice overs and touching memory pages.
 * 
 * 2     10/02/97 8:11p Rwyatt
 * Checked in to fix the build..
 * 
 * 1     10/02/97 6:32p Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/

#if 0

#include "Common.hpp"
#include "AsyncLoader.hpp"

#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgAudio.hpp"
#include "Lib/Audio/AudioDaemon.hpp"
#include "Lib/Std/MemLimits.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/VirtualMem.hpp"



//**********************************************************************************************
// Create a global thread class that is going to do all of the loading for us. This will
// create the actual thread here and now but it will take no processing time until something
// is addedd to it.
// Maybe at sme time in the future this would be created in a more controlled way..
CAsyncLoad				aslLoader;


//**********************************************************************************************
//
// Implementation of CAsyncLoad
// 
// This is a specific implementation of the async loader, the old version used a ThreadAction
// and a priority thread. The problem with the old implementation is that a thread action
// was allocated and added to the process list for every memory block that had to be fetched.
// Most of the time of the old loader was in this process which involved allocating the action
// class, initializing it, and adding it to the linked list of actions. Which itself includes
// another new for the list node. The old class was not always thead safe, this would not of
// caused a crash but it would have cause a memory leak and block not to be touched.
// This loader has the memory for the touch blocks pre-allocated and always maintains a pointer
// to the next block so adding another block is trivial and really fast. This loader is also
// completely thread safe.
// The memory touch is now within the virtual memory class so it can log the LRU status.
//


	//******************************************************************************************
	//
	CAsyncLoad::CAsyncLoad
	(
	)
	//*************************************
	{
		//
		// Create the critical sections so we can keep the system stable
		//
		InitializeCriticalSection(&csAddBlock);
		InitializeCriticalSection(&csThread);

		//
		// Point the foreground at one list and the background thread at the other
		//
		pmemtAdd		= &memtList1;
		pmemtProcess	= &memtList2;

		//
		// Set this to force the thread to exit. Do not kill the thread by force, let it exit
		// in the normal way.
		//
		u4ThreadExitCode = 0;

		// Create the thread that will do the processing.....
		hThread = CreateThread( 
					NULL,							// no security attributes 
					0,								// use default stack size  
					(LPTHREAD_START_ROUTINE) u4AsyncLoadThread,
					(void*)this,					// argument to thread function, class this pointer
					0,								// create running
					(ULONG*)&u4ThreadID);			// returns the thread identifier

		dprintf("Thread Create: AsyncLoader [Thread handle 0x%x]\n", u4ThreadID);
		//
		// The thread above is created running, it will pause itself immediately. This has to be
		// done so the threads critical section is taken. A critical section is like a semaphore
		// in operation but is alot faster. A critical section is aware of the thread that has
		// entered it so unlike a semaphore we cannot enter it on beahlf of the thread,
		//

		//
		// wait for the thread to pause, once we leave the cirtical section below the async loader
		// is stable.
		//
		EnterCriticalSection(&csAddBlock);
		LeaveCriticalSection(&csAddBlock);
	}



	//******************************************************************************************
	//
	CAsyncLoad::~CAsyncLoad
	(
	)
	//*************************************
	{
		// set the exit code of the thread to force it to exit
		u4ThreadExitCode = 0xffffffff;

		// force resume the thread, by undoing any pauses that have been issused.
		while (ResumeThread(hThread)>1);
		bThreadRunning = false;

		//
		// Try to enter the thread critical section, we will only be able to do this once the
		// thread has finished. Leave once we have entered for then, onad only then is it
		// safe to exit.
		//
		EnterCriticalSection(&csThread);
		LeaveCriticalSection(&csThread);

		//
		// at this point memory is no longer being touched by the loader.
		//

		DeleteCriticalSection(&csAddBlock);
		DeleteCriticalSection(&csThread);
	}



	//******************************************************************************************
	// Add another block to be touched
	//
	void CAsyncLoad::AddTouchBlock
	(
		const void*	pv_base,		// base of the region to touch
		uint32		u4_length,		// size of the region to touch
		CFetchable*	pf_fetchable	// The address of the fetchable object to touch, or NULL		
	)
	//*************************************
	{
		Assert (pmemtAdd);

		// make sure the parameters passed in are valid
		Assert(pv_base);
		Assert(u4_length > 0);

		// Some emergency code to handle overflows.
		if (pmemtAdd->u4Count >= u4MAX_MEM_TOUCH_REQUESTS - 1)
			return;

		EnterCriticalSection(&csAddBlock);

//		dprintf("#### add block ####\n");

		// add the data to the current block...
		pmemtAdd->ptblkNext->pvBase		= pv_base;
		pmemtAdd->ptblkNext->u4Length	= u4_length;
		pmemtAdd->ptblkNext->pfFetchable= pf_fetchable;

		// keep the count of the numbr of blocks we have added to the foreground
		pmemtAdd->u4Count++;

		// This assert means that the prefetcher is fetching too many textures.
		AlwaysAssert(pmemtAdd->u4Count<u4MAX_MEM_TOUCH_REQUESTS);

		uint32	u4_pos = pmemtAdd->u4AllocPos;

		// mark this block as used in the bit array
		pmemtAdd->au4UseMask[u4_pos >> 5] |= (1<<(u4_pos & 31));

		//
		// now find the next free entry..
		// This should always be the next entry in the array because the loader thread
		// is always behind us. To be really safe we will search for it as it is no
		// overhead if we find the block on the first try.
		//
		do
		{
			// when we reach the end go back to the start, there must be a free block somewhere or the
			// assert above would have gone off.
			if ((++u4_pos) >= u4MAX_MEM_TOUCH_REQUESTS)
			{
				u4_pos = 0;
				pmemtAdd->ptblkNext = pmemtAdd->atblkList;
			}
			pmemtAdd->ptblkNext++;

			// keep looking for an empty bit
		} while ( pmemtAdd->au4UseMask[u4_pos >> 5] & (1<<(u4_pos & 31)) );

		pmemtAdd->u4AllocPos = u4_pos;

		// if count is 1 then the only block is theone we have just added
		if (pmemtAdd->u4Count == 1)
		{
			// check if the thread is awake....
			if (!bThreadRunning)
			{
				//
				// thread is not running (or should not be).
				//
//				dprintf("#### resume thread ####\n");
				bThreadRunning = true;
				ResumeThread(hThread);
			}
		}


		LeaveCriticalSection(&csAddBlock);
	}

	//******************************************************************************************
	void CAsyncLoad::RemoveTouchBlocks
	(
	)
	//*************************************
	{
		// Take the Add critical section.
		EnterCriticalSection(&csAddBlock);

		// Reset the Add list to null.
		new (pmemtProcess) SMemTouch();

		// Free the critical section.
		LeaveCriticalSection(&csAddBlock);

		// Wait for the thread to finish preloading the Process list.
		int i_wait = 0;
		while (bThreadRunning && i_wait < 100)
		{
			Sleep(0.01);
			i_wait++;
		}

		// Should never take a second to finish the pre-fetch.
		AlwaysAssert(i_wait < 100);

		Assert(pmemtAdd->u4Count == 0);
		Assert(pmemtProcess->u4Count == 0);
	}


	
	//******************************************************************************************
	// The actual async (threaded) loader.
	//
	// We do not actually load the data, we just virtual memory and our VM manager will ensure
	// that the pages are in memory.
	//
	uint32 CAsyncLoad::u4AsyncLoadThread
	(
		uint32 u4_user
	)
	//*************************************
	{
		CAsyncLoad*		pasl			= (CAsyncLoad*) u4_user;

		VM_PROTECT_BACKGROUND
		//
		// take the thread critical section so the destructor of the parent class cannot exit,
		// as this would be fatal.
		//
		EnterCriticalSection(&pasl->csThread);

		// while there is no return code...
		while (pasl->u4ThreadExitCode == 0)
		{
			//
			// do we have any blocks to process...
			//
			if (pasl->pmemtProcess->u4Count == 0)
			{
				//
				// swap the blocks around within the critical section; so we do not
				// screw up when both threads are using the same data.
				//
				EnterCriticalSection(&pasl->csAddBlock);

//				dprintf("#### switch blocks ####\n");

				SMemTouch*	pmemt_local = pasl->pmemtProcess;
				pasl->pmemtProcess = pasl->pmemtAdd;
				pasl->pmemtAdd = pmemt_local;

				// set the alloc pos of the new add block to the start.
				pasl->pmemtAdd->u4AllocPos = 0;
				pasl->pmemtAdd->ptblkNext = pasl->pmemtAdd->atblkList;

				// the new block also has no actions, pause the thread
				if (pasl->pmemtProcess->u4Count == 0)
				{
					pasl->bThreadRunning = false;
					LeaveCriticalSection(&pasl->csAddBlock);
//					dprintf("#### suspend thread ####\n");
					SuspendThread(pasl->hThread);
//					dprintf("#### continue thread ####\n");
					continue;
				}

				LeaveCriticalSection(&pasl->csAddBlock);
			}

			//
			// Process the next block which has its bit set, should usually be the next block
			//
			if (pasl->pmemtProcess->au4UseMask[pasl->pmemtProcess->u4AllocPos >> 5] 
					& (1<<(pasl->pmemtProcess->u4AllocPos & 31)) )
			{
				// get the next block
				STouchBlock* ptblk = pasl->pmemtProcess->ptblkNext;

				// Pass the memory block to the VM manager
				CVirtualMem::Touch(ptblk->pvBase, ptblk->u4Length);

//				dprintf("Block: ADR = %x   Len = %x\n", ptblk->pvBase, ptblk->u4Length);
//				ptblk->pvBase = NULL;
//				ptblk->u4Length = 0;

				//
				// if this block has a CFetchable call it so it can fetch its children.
				//
				if (ptblk->pfFetchable)
				{
					ptblk->pfFetchable->OnPrefetch(true);	// we are being called from a thread
				}
	
				// clear the bit of the block we have just processed.
				pasl->pmemtProcess->au4UseMask[pasl->pmemtProcess->u4AllocPos >> 5] &= ~(1<<(pasl->pmemtProcess->u4AllocPos & 31));
				pasl->pmemtProcess->u4Count--;
			}

			// increment to the next block
			pasl->pmemtProcess->u4AllocPos++;
			pasl->pmemtProcess->ptblkNext++;

			// if we haves passed the end, go back to the start
			if (pasl->pmemtProcess->u4AllocPos >= u4MAX_MEM_TOUCH_REQUESTS)
			{
				pasl->pmemtProcess->u4AllocPos = 0;
				pasl->pmemtProcess->ptblkNext = pasl->pmemtProcess->atblkList;
			}
		}

		VM_UNPROTECT_BACKGROUND

		uint32 u4_ret = pasl->u4ThreadExitCode;

		//
		// Once the call below returns, the parent class will get deleted and the
		// memory pointed to by pasl becomes invalid. That is why we take a copy
		// of the return code.
		//
		LeaveCriticalSection(&pasl->csThread);

		return u4_ret;
		return 0;
	}


#endif