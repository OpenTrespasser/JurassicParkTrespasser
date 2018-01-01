/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		The data daemon class, used to keep the right bits of data in memory at the right times.
 *
 * Bugs:
 *
 * To do:
 *		
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/Fetchable.hpp                                             $
 * 
 * 5     7/22/98 10:05p Agrant
 * Removed data daemon and pre-fetching
 * 
 * 4     12/05/97 4:13p Agrant
 * Made the OnPrefetch() function const
 * 
 * 3     10/13/97 10:35p Agrant
 * Fetchables now require a boolean for OnPrefetch to describe whether we are in the thread or
 * in the main app.
 * 
 * 2     10/08/97 12:49a Agrant
 * Added iSize virtual function.
 * 
 * 1     10/02/97 5:47p Agrant
 * initial rev
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_LOADER_FETCHABLE_HPP
#define HEADER_GUIAPP_LOADER_FETCHABLE_HPP

//#define USING_FETCH 1


//*********************************************************************************************
//
class CFetchable
//
// A base class for all DataDaemon-Fetchable classes that can have special loading instructions.
//
// Prefix: f
//
//	Notes:
//		In the pure virtual memory implementation of the dynamic loader, all of these functions
//		default to null operations.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor .
	//

	//*****************************************************************************************
	//
	// Member functions.
	//

#ifdef USING_FETCH
	//*****************************************************************************************
	//
	virtual void OnPrefetch
	(
		bool b_in_thread
	) const
	//
	// Called when the object has been prefetched.
	//
	//	b_in_thread must be FALSE when called from the main program, and TRUE when called from
	//	the thread.
	//
	//**************************************
	{}

	//*****************************************************************************************
	//
	virtual void OnFetch
	(
	)
	//
	// Called when the object has been fetched.
	//
	//**************************************
	{}

	//*****************************************************************************************
	//
	virtual void OnUnfetch
	(
	)
	//
	// Called when the object is about to be unfetched.
	//
	//**************************************
	{}

	//*****************************************************************************************
	//
	virtual int iSize
	(
	) const
	//
	// Returns:  
	//		Size in bytes of object.
	//
	//**************************************
	{
		return sizeof(CFetchable);
	}


#endif  // USING_FETCH
};


#endif
