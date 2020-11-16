/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of the thread system defined in ThreadContro.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/ThreadControl.cpp                                            $
 * 
 * 10    10/02/98 9:50p Rwyatt
 * #if 0 whole file, not required
 * 
 * 9     4/21/98 2:46p Rwyatt
 * Removed traces of the old virtual memory
 * 
 * 8     12/15/97 7:49p Rwyatt
 * fixed threadID variable
 * 
 * 7     12/15/97 4:54p Rwyatt
 * Add a Thread ID log to debug window
 * 
 * 6     12/05/97 4:15p Agrant
 * Added virtual memory exception handling to Threaded loader.
 * 
 * 5     10/13/97 10:33p Agrant
 * Multiple list version for reducing the number of Semaphore calls.
 * 
 * 4     10/09/97 7:39p Rwyatt
 * 
 * 3     10/03/97 5:17p Rwyatt
 * Added the TF_NEXT flag to force insert an item next in the thread list.
 * 
 * 2     10/02/97 8:37p Rwyatt
 * Checked in to fix build...
 * 
 * 1     9/20/97 8:55p Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/

#if 0

#include "common.hpp"
#include "ThreadControl.hpp"

#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/VirtualMem.hpp"

//**********************************************************************************************
// Thread function is a C function which is passed a 'this' pointer as a parameter...
//
static uint32 u4ThreadFunction(uint32 u4_user);

CProfileStat psAdd;
CProfileStat psListAdd;


//**********************************************************************************************
//
CPriorityThreadControl::CPriorityThreadControl()
{
	char	str_name[128];

	ptlActionList	= NULL;
	ptlAddList		= NULL;
	u4Exit = 0;

	// Create the thread that will do the processing.....
    hThread = CreateThread( 
				NULL,							// no security attributes 
				0,								// use default stack size  
				(LPTHREAD_START_ROUTINE) u4ThreadFunction, // thread function 
				(void*)this,					// argument to thread function, class this pointer
				CREATE_SUSPENDED,				// create the flag in a suspended state
				(ULONG*)&u4ThreadId);			// returns the thread identifier 


	dprintf("Thread Create: ThreadControl [Thread handle 0x%x]\n", u4ThreadId);

	// make a unique name based on the this pointer so if two classes are created they
	// will still have unique names..
	wsprintf(str_name,"__ptcThreadFunction%x",(uint32)this);

	// create a semaphore for the whole thread function, this is used to make sure that the
	// thread function exits before its associated destructor exits. This will ensure
	// linear execution of critical code when the application is closed.
	// The semphore is created as TAKEN (signalled)
	hFuncSema = CreateSemaphore(NULL,0,1,str_name);

	// make another unique name for the list semaphore
	// will still have unique names..
	wsprintf(str_name,"__ThreadList%x",(uint32)this);

	hListSema = CreateSemaphore(NULL,1,1,str_name);
}




//**********************************************************************************************
//
CPriorityThreadControl::~CPriorityThreadControl()
{
	SThreadListElement*		ptl_this = ptlActionList;
	SThreadListElement*		ptl_next;

	// set the exit code for the thread
	u4Exit = 0xffffffff;

	// there are actions in the list, go and delete them..
	while (ptl_this)
	{
		// delete the action class
		delete ptl_this->ptabAction;
		ptl_next = ptl_this->ptlNext;

		// delete the structure from this list
		delete ptl_this;

		// set this to be the next in the list
		ptl_this = ptl_next;
	}

	ptlActionList=NULL;
	
	// Lock the add list so the thread does not access it
	LockAddList();
	ptl_this = ptlAddList;

	// there are actions in the list, go and delete them..
	while (ptl_this)
	{
		// delete the action class
		delete ptl_this->ptabAction;
		ptl_next = ptl_this->ptlNext;

		// delete the structure from this list
		delete ptl_this;

		// set this to be the next in the list
		ptl_this = ptl_next;
	}

	ptlAddList=NULL;
	UnlockAddList();

	
	// now resume the thread while there are no actions in the list;
	// There is now an exit code set so the thread will wxit, normally if the thread
	// is resumed and there are no actions it will re-suspend itself.
	// We must continually resume the thread because it may have been suspended from
	// elsewhere witin the code.
	while (ResumeThread(hThread)>1);

	// wait for the thread to finish...
	WaitForSingleObject(hFuncSema,INFINITE);

	// release the semaphore
	CloseHandle(hFuncSema);
	CloseHandle(hListSema);
}


//**********************************************************************************************
// Add to the new action to the list at the correct position. So long as the TF_NEXT flag has
// a higher numerical value than any real action priority; actions with this bit set will 
// autmatically be added to the start of the list. Multiple next requests will be processed
// in the order they are received.
//
// ACTION LIST MUST BE LOCKED BEFORE THIS FUNCTION IS CALLED
//
void CPriorityThreadControl::AddToList
(
	SThreadListElement*		ptl_start,				// pos to start looking in list
	SThreadListElement*		ptl_new,				// the new element
	uint32					u4_pos					// ordinal for priority ( > higher priority)
)
{
	while (ptl_start->ptlNext)
	{
		if ((ptl_start->ptlNext->u4Position<u4_pos))
		{
			// standard linked list to add the item...
			ptl_new->ptlNext = ptl_start->ptlNext;
			ptl_start->ptlNext = ptl_new;

			// we must return otherwise we will add the action again at the end of the list.....
			return;
		}
		ptl_start = ptl_start->ptlNext;
	}

	// there was no next pointer so add the element to the end of the list
	ptl_new->ptlNext = NULL;;
	ptl_start->ptlNext = ptl_new;
}



//**********************************************************************************************
// If this is called with the blocking flag then the action is called and not threaded off.
// it will return an action representing the action added


//	This function only adds to the Add list, and you must Lock the list before using it!

// This function can only add an item after the current one, in this way the first item of the
// list is always safe.


//
TThreadAction CPriorityThreadControl::taAddAction(CThreadActionBase* tab_new_action, TThreadFlags tf_flags)
{
	// if we are doing a blocking action then call the DoAction member of the class passed in...
	if ( (tf_flags & TF_PRIORITY_MASK) == TF_PRIORITY_BLOCK )
	{
		// Blocking reads not supported through Action Lists.
		Assert(false);

/*
		if (tab_new_action->hSyncObjectStart)
		{
			// if a start syncronization object was specified then act on it, the event type
			// can be any of the Win32 objects, event, Semaphore, Pipe, Mutex, Critical Section etc.
			// The effect of this call depends on the object type, its creation parameters and its
			// current signalled state.
			WaitForSingleObject(tab_new_action->hSyncObjectStart, INFINITE);
		}

		// if the action returns true do the callback....
		if (tab_new_action->DoAction())
		{
			// the action has returned true so do the callback if one is specified...
			if (tab_new_action->pfnCallback)
			{
				tab_new_action->pfnCallback(tab_new_action);
			}
		}

		delete tab_new_action;
		*/
		return NULL;
	}
	else
	{
		SThreadListElement*		ptl_new;

		ptl_new = new SThreadListElement;

		// if we fail to allocate the memory for the list then we must fail the operation...
		if (ptl_new == NULL)
			return NULL;

		ptl_new->ptabAction = tab_new_action;
		ptl_new->u4Position = tf_flags & (TF_PRIORITY_MASK|TF_NEXT);

		if (ptlAddList == NULL)
		{
			// if there is nothing in the list then add it to the start............
			// in this acase we can ignore the TF_NEXT flag because we will do any
			// action next...
			ptl_new->ptlNext = ptlAddList;
			ptlAddList = ptl_new;

			// make sure we have set the list up before we resume the thread
			ResumeThread(hThread);
		}
		else
		{
			AddToList(ptlAddList, ptl_new, tf_flags & (TF_PRIORITY_MASK|TF_NEXT));
		}

		return ptl_new;
	}

	// we should never get to here
	Assert(0);
	return NULL;
}



//**********************************************************************************************
// Lock the action list so only 1 thread at a time is accessing it. This is essential to ensure
// actions get executed in the correct order and actions do not get skipped.
// This function will block if another thread is accessing the list and will return when the
// other thread releases access to the list.
//	
void CPriorityThreadControl::LockAddList()
{
	Assert(hListSema);

	WaitForSingleObject(hListSema,INFINITE);
}



//**********************************************************************************************
// Unlock the action list...
//
void CPriorityThreadControl::UnlockAddList()
{
	Assert(hListSema);

	ReleaseSemaphore(hListSema,1,NULL);
}



//**********************************************************************************************
// Suspend the whole thread control so no actions will be processed
//
void CPriorityThreadControl::Suspend()
{
	Assert(hThread);

	SuspendThread(hThread);
}


//**********************************************************************************************
// Resume the whole thread control so no actions will be processed
//
void CPriorityThreadControl::Resume()
{
	Assert(hThread);

	ResumeThread(hThread);
}


//**********************************************************************************************
// the function that is threaded off must either be a C function of a static member, otherwise
// a this pointer is required and Win32 does not provide one.
// Thread functions do have a user DWORD, this DWORD will be set to the this pointer of the
// class, therefore we can access private and derived members of the class without any hacking.
// THIS FUNCTION IS USED FOR ALL THREAD CONTROLS, THEREFORE IT COULD BE EXECUTING MULTIPLE
// TIMES AT ONCE.....BECASUE OF THIS DO NOT USE ANY GLOBAL VARIABLES.
//
static uint32 u4ThreadFunction(uint32 u4_user)
{
	static	au4ThreadPriority[] = { THREAD_PRIORITY_NORMAL,				// (0)
									THREAD_PRIORITY_IDLE,				// (1)
									THREAD_PRIORITY_BELOW_NORMAL,		// (2)
									THREAD_PRIORITY_NORMAL,				// (3)
									THREAD_PRIORITY_ABOVE_NORMAL,		// (4)
									THREAD_PRIORITY_HIGHEST,			// (5)
									THREAD_PRIORITY_TIME_CRITICAL };	// (6)

	CPriorityThreadControl*	ptc = (CPriorityThreadControl*) u4_user;
	SThreadListElement*		ptl_this;

	// Set up the virtual memory exception handler.
	while(1)
	{
		// If there is nothing in the list suspend the thread so that it consumes no
		// processing time. The add member function will wake the thread up when an
		// action is added to the list.
		if (ptc->ptlActionList == NULL)
		{
			// if an exit code is set, terminate the thread with the specified exit code.
			if (ptc->u4Exit!=0)
			{
				// take a copy of the exit code before we release the semaphore
				uint32 u4_ret = ptc->u4Exit;
				// release the semaphore that syncronizes the thread with its class
				ReleaseSemaphore(ptc->hFuncSema,1,NULL);
				return u4_ret;
			}

			// We have no actions left in the action list.
			// Do we have any in the Add list?
			if (ptc->ptlAddList)
			{
				// Move the add list over to the Action list.
				ptc->LockAddList();
				ptc->ptlActionList	= ptc->ptlAddList;
				ptc->ptlAddList		= NULL;
				ptc->UnlockAddList();
				continue;
			}
			else
			{
				// Nothing left to load.  Suspend self.

				// this increments the suspend count, while non-zero the thread is suspended
				// THIS WILL NOT RETURN
				SuspendThread(ptc->hThread);
				continue;
			}
		}
		else
		{
			if (ptc->ptlActionList->ptabAction->hSyncObjectStart)
			{
				// if a start syncronization object was specified then act on it, the event type
				// can be any of the Win32 objects, event, Semaphore, Pipe, Mutex, 
				// Critical Section etc. The effect of this call depends on the object type,
				// its creation parameters and its current signalled state.
				WaitForSingleObject(ptc->ptlActionList->ptabAction->hSyncObjectStart, INFINITE);
			}

			// change the priority of the thread to relfect the prioriry of the action.
			SetThreadPriority(ptc->hThread, au4ThreadPriority[ptc->ptlActionList->u4Position]);

			// do the action of this class
			if (ptc->ptlActionList->ptabAction->DoAction())
			{
				// the action has returned true so do the callback if one is specifed..
				if (ptc->ptlActionList->ptabAction->pfnCallback != NULL)
				{
					ptc->ptlActionList->ptabAction->pfnCallback(ptc->ptlActionList->ptabAction);
				}
			}
		}

		// set the next action and remove the current one from the list, deleting the
		// action class.
		ptl_this = ptc->ptlActionList;

		// just check the pointer is not NULL....
		if (ptl_this)
		{
			// set the next action...
			ptc->ptlActionList = ptl_this->ptlNext;
		}

		// remove the element from the list and delete the action class
		delete ptl_this->ptabAction;
		delete ptl_this;
	}

	// Unset the virtual memory exception handler.
}


/*
		OLD ADD ACTION CODE

		SThreadListElement*		ptl_new;

		ptl_new = new SThreadListElement;

		// if we fail to allocate the memory for the list then we must fail the operation...
		if (ptl_new == NULL)
			return NULL;

		ptl_new->ptabAction = tab_new_action;
		ptl_new->u4Position = tf_flags & (TF_PRIORITY_MASK|TF_NEXT);

		LockActionList();


		if (ptlList == NULL)
		{
			// if there is nothing in the list then add it to the start............
			// in this acase we can ignore the TF_NEXT flag because we will do any
			// action next...
			ptl_new->ptlNext = ptlList;
			ptlList = ptl_new;

			// make sure we have set the list up before we resume the thread
			ResumeThread(hThread);
		}
		else
		{
			AddToList(ptlList, ptl_new, tf_flags & (TF_PRIORITY_MASK|TF_NEXT));
		}

		UnlockActionList();

		return ptl_new;
*/

#endif