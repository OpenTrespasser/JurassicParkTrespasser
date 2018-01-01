/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Global Loader Thread Class
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/AsyncLoader.hpp                                            $
 * 
 * 9     10/02/98 9:51p Rwyatt
 * #if 0 whole file, not required
 * 
 * 8     2/13/98 6:38p Agrant
 * Support for removing touch blocks before deleting the Data Daemon
 * 
 * 7     2/09/98 12:47p Agrant
 * Increase loader queue size
 * 
 * 6     12/15/97 5:00p Rwyatt
 * Major change:
 * We have gone from a threaded action loader to a loader with its own implementation. The was
 * done to impreove the prefetch times.
 * All references to the TouchAction have been removed
 * 
 * 5     12/05/97 4:18p Agrant
 * Added new asserts for invalid load requests.
 * 
 * 4     11/14/97 12:08a Rwyatt
 * remopved the async-load voice over action
 * 
 * 3     10/13/97 10:37p Agrant
 * Touching Fetchables now uses the fetchable callback (OnPrefetch())
 * 
 * 2     10/03/97 5:19p Rwyatt
 * Implemented basic actions for audio voice overs and touching memory pages.
 * 
 * 1     10/02/97 6:32p Rwyatt
 * Initial Implementation
 *
 **********************************************************************************************/

#ifndef HEADER_LOADER_ASYNC_LOADER_HPP
#define HEADER_LOADER_ASYNC_LOADER_HPP

#if 0

#include "Lib/W95/WinInclude.hpp"
#include "Lib/EntityDBase/Instance.hpp"


//**********************************************************************************************
// Maximum number of touch blocks in either of the add or process lists.
//
#define u4MAX_MEM_TOUCH_REQUESTS	8192



//**********************************************************************************************
//
class CAsyncLoad
// prefix: asl
{
public:

	//******************************************************************************************
	// The data of an individual touch block
	//
	struct STouchBlock
	// prefix: tblk
	{
		const void*	pvBase;				// base of the region to touch
		CFetchable*	pfFetchable;		// The address of the fetchable object to touch, or NULL
		uint32		u4Length;			// size of the region to touch
	};


	//******************************************************************************************
	// The structure of a touch heap, there are two heaps in the class
	//
	struct SMemTouch
	// prefix: memt
	{
		uint32			u4Count;		// number of elements in the array (somewhere)
		STouchBlock*	ptblkNext;		// the next element to allocate
		uint32			u4AllocPos;		// the index of the next allocation
		uint32			au4UseMask[u4MAX_MEM_TOUCH_REQUESTS / 32];		// 1 bit per element
		STouchBlock		atblkList[u4MAX_MEM_TOUCH_REQUESTS];			// array of elements

		//**************************************************************************************
		// Constructor for this structure
		//
		SMemTouch()
		{
			u4Count = 0;
			u4AllocPos = 0;
			ptblkNext = atblkList;
			memset(au4UseMask, 0, sizeof(uint32) * (u4MAX_MEM_TOUCH_REQUESTS / 32) );
		}
	};


	//******************************************************************************************
	//
	// Constructor and destructor
	//

	//******************************************************************************************
	CAsyncLoad();

	//******************************************************************************************
	~CAsyncLoad();

	//******************************************************************************************
	// Add another block to be touched
	void AddTouchBlock
	(
		const void*	pv_base,			// base of the region to touch
		uint32		u4_length,			// size of the region to touch
		CFetchable*	pf_fetchable = NULL	// The address of the fetchable object to touch, or NULL		
	);

	//******************************************************************************************
	// Remove all touch blocks.
	void RemoveTouchBlocks
	(
	);

	//******************************************************************************************
	void Suspend()
	{
		SuspendThread(hThread);
	}


	//******************************************************************************************
	void Resume()
	{
		ResumeThread(hThread);
	}


	//******************************************************************************************
	// The thread function, MUST BE STATIC.
	// The paramter passed into this function is the this pointer back to the class
	//
	static uint32 u4AsyncLoadThread
	(
		uint32 u4_user
	);


	//******************************************************************************************
	//
	//	MEMBER VARIABLES
	//
	CRITICAL_SECTION	csAddBlock;
	CRITICAL_SECTION	csThread;
	HANDLE				hThread;
	uint32				u4ThreadID;
	uint32				u4ThreadExitCode;
	bool				bThreadRunning;

	SMemTouch*			pmemtAdd;				// allocate from this heap
	SMemTouch*			pmemtProcess;			// process this heap
	SMemTouch			memtList1;
	SMemTouch			memtList2;
};


//******************************************************************************************
//
// Global functions, classes and data elements
//

extern CAsyncLoad				aslLoader;

#endif


#endif