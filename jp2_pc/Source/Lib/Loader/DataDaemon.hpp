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
 *
 * Notes:
 *  Prefetches could be done through messages, but since Fetches cannot easily be done that way,
 *  we might as well keep the interface uniform and have all fetch, prefetch, and unfetch requests
 *  work directly through function calls.  In addition, there is no pressing need for the 
 *  data daemon to inherit from CInstance or CSubsystem.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/DataDaemon.hpp                                            $
 * 
 * 11    7/22/98 10:05p Agrant
 * Removed data daemon and pre-fetching
 * 
 * 10    2/19/98 8:49p Agrant
 * SLightly more clever fetching, max 100 meters
 * Warning message when fetching takes too long
 * 
 * 9     12/17/97 3:39p Agrant
 * Removed the wasted mem option, as it is now not needed due to the virtual texture system
 * 
 * 8     12/15/97 4:58p Rwyatt
 * Changed to use the new specific async loader instead of the threaded action loader.
 * No priortiy is passed to the load functions has it can no longer be supported.
 * 
 * 7     12/11/97 5:45p Agrant
 * Preload optimization- never load the same mesh twice in a preload session
 * 
 * 6     12/10/97 4:07p Agrant
 * Preload queries are now part of the scheduler to avoid jerky framerate.
 * Still needs speeding up.
 * 
 * 5     12/05/97 4:17p Agrant
 * Removed the bInPhysicalMemory call.  Assumes that everything needs fetching.
 * 
 * 4     11/11/97 2:57p Agrant
 * Save/Load functions
 * 
 * 3     10/13/97 10:36p Agrant
 * Numerous chages, moved implementations into .cpp 
 * 
 * 2     10/08/97 12:54a Agrant
 * An actual implementation of the loader.
 * 
 * 1     10/02/97 5:47p Agrant
 * initial rev
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_LOADER_DATAMASTER_HPP
#define HEADER_GUIAPP_LOADER_DATAMASTER_HPP


#include "Fetchable.hpp"

#ifdef USING_FETCH

//#include "Lib/Loader/ASyncLoader.hpp"
#include "Lib/EntityDBase/SubSystem.hpp"
#include "Lib/Sys/Timer.hpp"
#include "Lib/Sys/Scheduler.hpp"

// The handle used to refer to a piece of data.  For now, just a pointer to the data.
typedef const void* TDataHandle;

// The handle used to refer to a Fetchable piece of data, with functions to handle special loading issues.
typedef const CFetchable* TFetchableHandle;

// The other handle used to refer to a Fetchable piece of data, with functions to handle special loading issues.
typedef rptr<CFetchable> TFetchableHandle2;

//*********************************************************************************************
//
class CDataDaemon : public CSubsystem, public CSchedulerItem
//
// Manages data movement and storage- HD vs. RAM
//
// Prefix: dd
//
//	Notes:
//		There are two implementations for each function in the Data Daemon.  One deals with generic 
//		data, such as bitmaps and the like, and the second deals with CFetchable's.  A CFetchable has
//		additional overhead, and handles loading sub-objects after it is itself loaded.
//
//**************************************
{
public:
	CVector3<>	v3LastPrefetchPosition;	// Where was the camera at the last prefetch?
	TReal		rUpdateDistSqr;			// How far can we move before we must update again? (squared)
	TReal		rDistancePastFarClip;	// How far past the far clipping plane do we prefetch?
	TSec		sLastPrefetchTime;		// When was our last prefetch update?
	TSec		sPrefetchInterval;		// The interval at which we prefetch even without movement.
	bool		bThreadedLoad;			// Use a thread to load?  true if yes.
	bool		bHaveActionListLock;	// True when the application cont
	bool		bPrefetchWastedMem;		// True when the application prefetches the wasted memory.
	char		cLastPrefetch;			// An index of the prefetch, changed each time it runs.  A bad value here
										// doesn't really matter.


	int			iFetchHits;				// Number of times that fetched memory is already available.
	int			iFetchMisses;			// Number of times that fetched memory is not already available.
//	char*		pcWastedMem;			// A pointer to the wasted memory.
//	int			iWastedMem;				// The Amount of Memory to waste (in bytes)

	//*****************************************************************************************
	//
	// Constructor .
	//
	CDataDaemon();

	~CDataDaemon();
	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	TReal rFetchDistance
	(
		TReal r_clip_plane_distance		// distance to the clipping plane
	);
	//
	// Returns the distance to which the world DBase should go in prefetching.
	//
	//**************************


	//*****************************************************************************************
	//
	void Prefetch
	(
		TDataHandle	dh_data,
		bool b_in_thread,
		int			i_size = 1
	);
	//
	// Prefetch the thing pointed to by pv_data.
	//
	// Notes:
	//		i_size is used for large objects that are likely to cross page boundaries, and thus need
	//		a bit more attention on the part of the threaded loader.  In bytes.
	//
	//**************************

	//*****************************************************************************************
	//
	void Prefetch
	(
		TFetchableHandle	fh_data,
		bool b_in_thread
	);
	//
	// Prefetch the fetchable pointed to by fh_data.
	//
	// Notes:
	//		i_size is used for large objects that are likely to cross page boundaries, and thus need
	//		a bit more attention on the part of the threaded loader.  In bytes.
	//
	//**************************

	//*****************************************************************************************
	//
	void Prefetch
	(
		TFetchableHandle2	fh_data,
		bool b_in_thread
	);
	//
	// Prefetch the fetchable pointed to by fh_data.
	//
	// Notes:
	//		i_size is used for large objects that are likely to cross page boundaries, and thus need
	//		a bit more attention on the part of the threaded loader.  In bytes.
	//		
	//**************************
	
	//*****************************************************************************************
	//
	void PrefetchNext
	(
		TDataHandle	dh_data,
		int			i_size = 1
	);
	//
	// Prefetch the thing pointed to by pv_data, but put it first in line.
	//
	// Notes:
	//		FETCH NEXT CANNOT BE IMPLEMENTED
	//
	//**************************

	//*****************************************************************************************
	//
	void PrefetchNext
	(
		TFetchableHandle	fh_data
	);
	//
	// Prefetch the fetchable pointed to by fh_data, but put it first in line.
	//
	// Notes:
	//		FETCH NEXT CANNOT BE IMPLEMENTED
	//
	//**************************

	//*****************************************************************************************
	//
	void Fetch
	(
		TDataHandle	dh_data,
		int			i_size = 1
	)
	//
	// Fetch the thing pointed to by pv_data immediately.
	//
	// Notes:
	//		Performs a blocking read to get dh_data if necesssary, and returns the pointer immediately.
	//
	//**************************
	{
		// Needs no implementation, as Virtual Memory will do it for us in a blocking manner.
	}

	//*****************************************************************************************
	//
	void Fetch
	(
		TFetchableHandle	fh_data,
		int			i_size = 1
	)
	//
	// Fetch the fetchable pointed to by pv_data immediately.
	//
	// Notes:
	//		Performs a blocking read to get dh_data if necesssary, and returns the pointer immediately.
	//
	//**************************
	{
//		Touch(dh_data, i_size);
		((CFetchable*)fh_data)->OnFetch();
//		return (CFetchable*)fh_data;
	}

	//*****************************************************************************************
	//
	void Fetch
	(
		TFetchableHandle2	fh_data,
		int			i_size = 1
	)
	//
	// Fetch the fetchable pointed to by pv_data immediately.
	//
	// Notes:
	//		Performs a blocking read to get dh_data if necesssary, and returns the pointer immediately.
	//
	//**************************
	{
//		Touch(dh_data, i_size);
		fh_data->OnFetch();
//		return fh_data;
	}

	//*****************************************************************************************
	//
	void Unfetch
	(
		TDataHandle	dh_data,
		int			i_size = 1
	)
	//
	// Lets the DataDaemon know that the data is no longer necessary.
	//
	// Notes:
	//		
	//
	//**************************
	{
		// Do nothing.
	}

	//*****************************************************************************************
	//
	void Unfetch
	(
		TFetchableHandle	fh_data,
		int					i_size = 1
	)
	//
	// Lets the DataDaemon know that the data is no longer necessary.
	//
	// Notes:
	//		
	//
	//**************************
	{
		((CFetchable*)fh_data)->OnUnfetch();
	}

	//*****************************************************************************************
	//
	void PrefetchWorld
	(
	);
	//
	// Performs a threaded prefetch operation on any instances close enough to the camera to matter.
	//
	//**************************

	//
	//	Message Processing
	//
	virtual void Process(const CMessageStep& msgstep);

	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

	//*****************************************************************************************
	virtual void Execute();

	//*****************************************************************************************
	virtual void PostScheduleExecute();

};


//
// Global variables.
//


// The global data daemon.
extern CDataDaemon* pddDataDaemon;

#endif	// USING_FETCH
#endif  // include wrapper
