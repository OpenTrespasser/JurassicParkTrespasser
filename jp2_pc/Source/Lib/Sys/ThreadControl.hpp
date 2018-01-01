/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Definition of the thread control class and associated constants
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/ThreadControl.hpp                                            $
 * 
 * 6     10/02/98 9:50p Rwyatt
 * #if 0 whole file, not required
 * 
 * 5     10/13/97 10:33p Agrant
 * Multiple list version for reducing the number of Semaphore calls.
 * 
 * 4     10/09/97 7:40p Rwyatt
 * blah
 * 
 * 3     10/03/97 5:18p Rwyatt
 * Added the TF_NEXT flag
 * 
 * 2     10/02/97 8:37p Rwyatt
 * checked in to fix build
 * 
 * 1     9/20/97 8:55p Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_THREAD_CONTROL_HPP
#define HEADER_THREAD_CONTROL_HPP

#if 0

#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "ThreadActionBase.hpp"


//**********************************************************************************************
// flags that control the action of threads.
// This type is a set of bits which is defned below
//
typedef uint32	TThreadFlags;
typedef void*	TThreadAction;


//**********************************************************************************************
// THREAD PRIORITY FLAGS FOR ACTION LISTS
//**********************************************************************************************
// Idle priortiy functions are added to the end of the list in the order they are received
// unless a list order is specified. Idle priority is very low priority and functions running
// at this priority may take a while to complete. Functions in the list after the idle function
// will have to wait for it to complete. The priortity of an idle process may be boosted if a
// hight priority action is added.
#define TF_PRIORITY_IDLE	0x00000001

//**********************************************************************************************
// A low priority action is set to a thread priority 2 lower than the main application so will
// take about 25% of the processing time of the main application. 
// This priority is good for caching files from CD to the hard drive.
#define TF_PRIORITY_LOW		0x00000002

//**********************************************************************************************
// Threads at this priority are given the same amount of processig time as the main application
// thread.
#define TF_PRIORITY_NORMAL	0x00000003

//**********************************************************************************************
// Threads at this priority are given more processing time that the main application thread,
// CPU intensive actions at this priority will result in a performance hit. 
#define TF_PRIORITY_HIGH	0x00000004

//**********************************************************************************************
// Threads at this priority are given alot more processing time that the main application thread,
// CPU intensive actions at this priority will result in a performance hit. 
#define TF_PRIORITY_HIGHEST	0x00000005

//**********************************************************************************************
// Thread at this priority will get most of the processor time and will in effect block other
// threads and processes.
#define TF_PRIORITY_CRITICAL 0x00000006

//**********************************************************************************************
// if blocking priority is used then the function to be threaded is called by the add function
// within the main thread so the application will be stopped while the function finishes.
// Thread functions with blocking priority will ignore flags that control list order, the
// function is always called immediately
#define TF_PRIORITY_BLOCK	0x00000007

#define TF_PRIORITY_MASK	0x00000007

//**********************************************************************************************
// If the next priority if specified then the action is placed in the queue after the action
// that is currently active. The priority if also speicfed is still used but only to set the
// thread priority and has no effect on the order of the list. The action specifed with this
// flag will always be done next regardless of what is already in the queue.
#define TF_NEXT				0x00000100







//**********************************************************************************************
// Structure that makes the linked list of actions
struct SThreadListElement
{
	SThreadListElement*	ptlNext;
	CThreadActionBase*	ptabAction;
	uint32				u4Position;

	SThreadListElement()
	{
		ptlNext		= NULL;
		ptabAction	= NULL;
		u4Position	= 0;		// zero is lower than any real action priority
	}
};





//**********************************************************************************************
// This is the base class of the thread handling system
//
class CPriorityThreadControl
// prefix ptc
{
public:
	//******************************************************************************************
	//	
	CPriorityThreadControl();

	//******************************************************************************************
	//
	~CPriorityThreadControl();

	//******************************************************************************************
	//
	TThreadAction taAddAction(CThreadActionBase* tab_new_action, TThreadFlags tf_flags);
//	TThreadAction taAddToAddList(CThreadActionBase* tab_new_action, TThreadFlags tf_flags);

	void MergeLists();	// Merges Add and Action lists into the Action list.  Called from Thread.

	//******************************************************************************************
	void LockAddList();

	//******************************************************************************************
	void UnlockAddList();

	//******************************************************************************************
	// Suspend the whole thread control so no actions will be processed
	//
	void Suspend();


	//******************************************************************************************
	// Resume the whole thread control so no actions will be processed
	//
	void Resume();


	//******************************************************************************************
	// data members
	//
	SThreadListElement*				ptlActionList;		// list of actions in this thread class
	SThreadListElement*				ptlAddList;			// list of actions to add to action list
		// The Action List is not accessible from the main program- only from within the thread.
		// The Add List is accessible from both, and it is the list that gets locked and unlocked.
    uint32							u4ThreadId;			// thread ID
    HANDLE							hThread;			// thread object handle
	HANDLE							hFuncSema;			// semaphore handle to syncronize thread
	HANDLE							hListSema;			// semaphore to syncronize action list access
	uint32							u4Exit;				// Exit code for the thread

private:
	//******************************************************************************************
	//
	void AddToList
	(
		SThreadListElement*		ptl_start,				// pos to start looking in list
		SThreadListElement*		ptl_new,				// the new element
		uint32					u4_pos					// ordinal for priority ( > higher priority)
	);

};


#endif

#endif