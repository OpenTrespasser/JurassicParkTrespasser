/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:   Schedules operations based on priorities and time slices available.
 *
 * Notes:      
 *
 * To do:      
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/Sys/Scheduler.hpp                                                $
 * 
 * 11    10/02/98 2:40a Pkeet
 * Added a member function to set the time slice.
 * 
 * 10    6/21/98 3:32p Pkeet
 * Removed schedule disabling based on camera movement.
 * 
 * 9     4/29/98 2:42p Pkeet
 * Added code for determining if the main camera has moved on a per frame basis using a
 * threshold.
 * 
 * 8     4/21/98 7:23p Mlange
 * Made some data members non-static. Added terrain texture scheduler object.
 * 
 * 7     98.03.04 12:12p Mmouni
 * Changed performance counters for more consistent results.
 * 
 * 6     12/05/97 3:49p Pkeet
 * Added the 'fAlwaysExecute' and 'fNeverExecute' constants.
 * 
 * 5     97/10/28 5:33p Pkeet
 * Replaced the callback for execution with a scheduler item base class and virtual function
 * calls. Added a post scheduler execution call in addition to the execution call. Replaced the
 * 'pfhExecuteHeap' heap with a 'CDArray' templated to pointers to the scheduler item base
 * class.
 * 
 * 4     97/10/27 6:20p Pkeet
 * Changed the callback to pass the priority parameter. Added data and function members to the
 * scheduler object to allow for timing.
 * 
 * 3     97/10/22 5:42p Pkeet
 * Added the time remaining flag for the execute command.
 * 
 * 2     97/10/22 3:14p Pkeet
 * Implemented user and execution element heaps. Implemented an interface to add elements.
 * 
 * 1     97/10/21 8:20p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_SYS_SCHEDULER_HPP
#define HEADER_LIB_SYS_SCHEDULER_HPP


//
// Required includes.
//
#include "Lib/Sys/FastHeap.hpp"
#include "Lib/Sys/Profile.hpp"


//
// Forward declarations.
//
class CFastHeap;
class CConsoleBuffer;
class CSchedulerItemCompare;


//
// Constants.
//

// Value indicating that the scheduled item must be executed.
const float fAlwaysExecute = 1000000000.0f;

// Value indicating that the scheduled item should never be executed.
const float fNeverExecute  = 0.0f;


//
// Class definitions.
//

//*********************************************************************************************
//
class CSchedulerItem
//
// Schedules function operations.
//
// Prefix: scit
//
//**************************************
{
	float fPriority;	// Priority for this schedule item.
public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CSchedulerItem()
		: fPriority(0.0f)
	{
	}

	// Construct with a priority value.
	CSchedulerItem(float f_priority)
		: fPriority(f_priority)
	{
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	float fGetPriority
	(
	) const
	//
	// Returns the priority of the scheduled item.
	//
	//**************************************
	{
		return fPriority;
	}

	//*****************************************************************************************
	//
	virtual void Execute
	(
	)
	//
	// Executes the element.
	//
	//**************************************
	{
		Assert(0);
	}

	//*****************************************************************************************
	//
	virtual void PostScheduleExecute
	(
	)
	//
	// Executes the element.
	//
	//**************************************
	{
		Assert(0);
	}

	//*****************************************************************************************
	//
	virtual void LogItem
	(
		CConsoleBuffer& con	// Buffer to write to.
	) const;
	//
	// Outputs information about this item to a log file.
	//
	//**************************************

protected:

	//*****************************************************************************************
	//
	void SetPriority
	(
		float f_priority
	)
	//
	// Returns the priority of the scheduled item.
	//
	//**************************************
	{
		fPriority = f_priority;
	}

private:

	friend class CSchedulerItemCompare;

};


//*********************************************************************************************
//
class CScheduler
//
// Schedules function operations.
//
// Prefix: sch
//
//**************************************
{
private:
	CFastHeap* pfhHeap;							// User allocation object.

	CDArray<CSchedulerItem*> dapscitItems;		// Array of pointers to the scheduled items.

	uint       uTimeMS;							// Time at the point 'StartTimer' was called.

public:
	uint uMSSlice;				// Maximum desired timeslice in milliseconds.

	static bool bUseScheduler;	// Flag indicates if scheduling is active or disabled.

#if (VER_TIMING_STATS)
	// Time that is accounted for seperately.
	static TCycles cyAccountedScheduleCycles;
#endif

public:

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	// Default constructor.
	CScheduler
	(
		uint u_ms_slice	// Time slice allotted per frame.
	);

	// Destructor.
	~CScheduler();

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void SetTimeSlice
	(
		uint u_ms_slice
	);
	//
	// Sets the timeslice for the object.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddExecution
	(
		CSchedulerItem* pscit
	);
	//
	// Adds an execution object.
	//
	// Notes:
	//		The priority value sorts by the highest value first.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Execute
	(
	);
	//
	// Executes as many operations in the timeslice allowed.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Clear
	(
	);
	//
	// Clears the user data and execution heaps.
	//
	//**************************************

	//*****************************************************************************************
	//
	// Friend functions.
	//

	//*****************************************************************************************
	//
	friend void* operator new
	(
		uint		u_size_type,	// Size of object to be allocated in bytes.
		CScheduler&	sch,			// Reference to the scheduler making the memory allocation.
		uint		u_alignment= 4	// Byte alignment of new memory.
	);
	//
	// Allocates memory from the heap associated with the scheduler.
	//
	//**************************************

private:

	//*****************************************************************************************
	void StartTimer();

	//*****************************************************************************************
	uint uGetTime() const;


};


//
// Global variables.
//

// The main scheduler object.
extern CScheduler shcScheduler;
extern CScheduler shcSchedulerTerrainTextures;


#endif // HEADER_LIB_SYS_SCHEDULER_HPP
