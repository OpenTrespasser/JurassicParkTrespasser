/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Definition of a thread action classes (and the reference base class)
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/ThreadActionBase.hpp                                         $
 * 
 * 3     10/02/98 9:50p Rwyatt
 * #if 0 whole file, not required
 * 
 * 2     10/03/97 5:18p Rwyatt
 * changed the default constructor of the base class to take both a callback and a sync event
 * handle
 * 
 * 1     10/02/97 6:34p Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_THREAD_ACTION_BASE_HPP
#define HEADER_THREAD_ACTION_BASE_HPP

#if 0

#include "Common.hpp"
#include "Lib/Sys/DebugConsole.hpp"


//**********************************************************************************************
// Forward declaration...
//
class CThreadActionBase;


typedef void (_stdcall *callback)(CThreadActionBase*);



//**********************************************************************************************
// Every thread action derived from this base must provide an implementation of the following
// functions:
//		DoAction	-	perform the required action of the class
//
class CThreadActionBase
// all thread actions should have a prefix prefix ta--
// prefix: tab
{
public:

	//******************************************************************************************
	//
	// Constructors and deconstructors
	//

	//******************************************************************************************
	//
	// Constructor to set the callback and the event, with suitable defaults.
	//
	CThreadActionBase(callback pfn, HANDLE h_event)
	{
		// This Assert will fail if the class is statically alloctaed. Action classes must be
		// alloctated with the new operator.
		Assert( _CrtIsValidHeapPointer(this) );

		// default constructor sets the sync object and the callback to be null
		hSyncObjectStart	= h_event;
		pfnCallback			= pfn;
	}



	//******************************************************************************************
	//
	// Default destructor does nothing
	//
	~CThreadActionBase()
	{
	}

	//******************************************************************************************
	//
	// Pure definition, every derived class must implement this member.
	// If this returns true then the callback is performed. If fasle is returned then the action
	// is just deleted an no futher processing is perfomed.
	//
	virtual bool DoAction() = 0;


	HANDLE		hSyncObjectStart;					// the win32 object to wait on...
	callback	pfnCallback;						// the callback function pointer
};



#endif

#endif