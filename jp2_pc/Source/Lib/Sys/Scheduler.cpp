/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:   Implementation of Scheduler.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/Scheduler.cpp                                                $
 * 
 * 21    10/02/98 2:40a Pkeet
 * Added a member function to set the time slice.
 * 
 * 20    9/19/98 3:03p Pkeet
 * Changed scheduler stat name.
 * 
 * 19    9/02/98 2:39p Rwyatt
 * Chnaged some VER_FINAL to VER_PROFILE_STATS. This was cauing errors in non final modes with
 * stats disabled.
 * 
 * 18    8/28/98 11:58a Asouth
 * loop var re-scoped
 * 
 * 17    6/21/98 3:32p Pkeet
 * Removed schedule disabling based on camera movement.
 * 
 * 16    6/01/98 2:37p Mlange
 * Fixed bug in CScheduler::uGetTime(). Scheduled terrain textures are now always evaluated,
 * even if the camera hasn't moved.
 * 
 * 15    5/10/98 2:03p Pkeet
 * Changed enumeration interator.
 * 
 * 14    4/29/98 2:42p Pkeet
 * Added code for determining if the main camera has moved on a per frame basis using a
 * threshold.
 * 
 * 13    4/21/98 7:23p Mlange
 * Made some data members non-static. Added terrain texture scheduler object.
 * 
 * 12    3/11/98 2:47p Breed
 * 
 * 11    98.03.04 12:12p Mmouni
 * Changed performance counters for more consistent results.
 * 
 * 10    1/27/98 11:42a Pkeet
 * Increased the number of objects permitted in the scheduler.
 * 
 * 9     12/05/97 3:49p Pkeet
 * Added the 'fAlwaysExecute' and 'fNeverExecute' constants.
 * 
 * 8     12/01/97 3:56p Pkeet
 * Reduced default time assigned to the scheduler.
 * 
 * 7     11/13/97 12:11a Gfavor
 * Added a timer.
 * 
 * 6     97/10/28 5:33p Pkeet
 * Replaced the callback for execution with a scheduler item base class and virtual function
 * calls. Added a post scheduler execution call in addition to the execution call. Replaced the
 * 'pfhExecuteHeap' heap with a 'CDArray' templated to pointers to the scheduler item base
 * class.
 * 
 * 5     97/10/27 6:22p Pkeet
 * Added the 'fGetPriority' member function to the scheduler element object. Added the
 * 'LogItemCompletion' function. Added timer code and code to execute scheduled items based on
 * priority and time available.
 * 
 * 4     97/10/24 11:55a Pkeet
 * Added a log for cache schedule items.
 * 
 * 3     97/10/22 5:42p Pkeet
 * Added the time remaining flag for the execute command.
 * 
 * 2     97/10/22 3:15p Pkeet
 * Implemented user and execution element heaps. Implemented an interface to add elements.
 * Added sorted execution.
 * 
 * 1     97/10/21 8:20p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/Sys/FastHeap.hpp"
#include <algorithm>
#include "Lib/Sys/Textout.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Scheduler.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"


//
// Macros and constants.
//

// Flag to compile a schedule log.
#define bLOG_SCHEDULE (0)

// Maximum permitted scheduled items.
const int iMaxNumSchedulerItems = 25000;


//
// Module variables associated with a schedule log.
//

//
// Additional includes.
//
#if bLOG_SCHEDULE
	#include "Lib/Renderer/RenderCache.hpp"
#endif // bLOG_CACHE_SCHEDULE

//
// Module variables.
//
#if bLOG_SCHEDULE
	CConsoleBuffer conSchedule;	// Schedule log.
#endif // bLOG_CACHE_SCHEDULE

static CProfileStat	psRenderSchedule("Scheduled", &proProfile.psRenderShape);
static CProfileStat	psRenderSchedulePriority("Priority", &psRenderSchedule);
static CProfileStat	psRenderScheduleOptional("Optional", &psRenderSchedule);
static CProfileStat	psRenderSchedulePost("Rndr to Scn", &psRenderSchedule);


//
// Internal class definitions.
//


//
// Class definitions.
//

//*********************************************************************************************
//
class CSchedulerItemCompare
//
// Compares two CSchedulerItem by priority.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	bool operator()
	(
		const CSchedulerItem* pscit_0,	// First execute object.
		const CSchedulerItem* pscit_1	// Second execute object.
	) const
	//
	// Returns 'true' if the priority of the first execute object is larger than the priority
	// of the second execute object.
	//
	//**************************************
	{
		Assert(pscit_0);
		Assert(pscit_1);
		Assert(pscit_0->fPriority >= 0.0f);
		Assert(pscit_1->fPriority >= 0.0f);

		// Return the results of the comparision.
		return u4FromFloat(pscit_0->fPriority) > u4FromFloat(pscit_1->fPriority);
	}

};


//
// Module specific function prototypes.
//

#if bLOG_SCHEDULE

	//*****************************************************************************************
	//
	void LogScheduledItems
	(
	);
	//
	// Outputs information about this item to a log file.
	//
	//**************************************

	//*****************************************************************************************
	//
	void LogItemCompletion
	(
		int i_num_done,
		int i_num_total
	);
	//
	// Outputs information about this item to a log file.
	//
	//**************************************

#endif // bLOG_SCHEDULE


//
// Class implementation.
//

//*********************************************************************************************
//
// CSchedulerItem implementation.
//

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	void CSchedulerItem::LogItem(CConsoleBuffer& con) const
	{
		con.Print("\n%1.3f\t", fPriority);
	}


//*********************************************************************************************
//
// CScheduler implementation.
//

#if (VER_TIMING_STATS)
	TCycles CScheduler::cyAccountedScheduleCycles;
#endif

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	//*****************************************************************************************
	CScheduler::CScheduler(uint u_ms_slice)
		: pfhHeap(new CFastHeap(iMaxNumSchedulerItems * 16)),
		  dapscitItems(iMaxNumSchedulerItems), uMSSlice(u_ms_slice)
	{
		// Initialize heaps for use.
		Clear();
	}

	//*****************************************************************************************
	CScheduler::~CScheduler()
	{
		delete pfhHeap;
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	void CScheduler::SetTimeSlice(uint u_ms_slice)
	{
		uMSSlice = u_ms_slice;
	}

	//*****************************************************************************************
	void CScheduler::AddExecution(CSchedulerItem* pscit)
	{
		dapscitItems << pscit;
	}

	//*****************************************************************************************
	void CScheduler::Clear()
	{
		pfhHeap->Reset();
		dapscitItems.Reset();
	}

	//*****************************************************************************************
	void CScheduler::Execute()
	{
		CCycleTimer ctmr_scheduler;
		CCycleTimer ctmr;
		uint        u;

#if (VER_TIMING_STATS)
		// Avoid counting time twice.
		cyAccountedScheduleCycles = 0;
#endif

		// Begin counting the timeslice.
		StartTimer();

		// Sort by priority using STL's QSort routine.
		std::sort(dapscitItems.atArray, dapscitItems.atArray + dapscitItems.uLen,
			 CSchedulerItemCompare());

	#if bLOG_SCHEDULE

		// Log Scheduled items.
		LogScheduledItems();

	#endif // bLOG_SCHEDULE
		
		// Execute everything that has a maximum priority.
		int i_item;
		for (i_item = 0; i_item < dapscitItems.uLen; ++i_item)
		{
			// Break if the priority is less that the maximum.
			if (dapscitItems[i_item]->fGetPriority() < fAlwaysExecute)
				break;
			Assert(dapscitItems[i_item]);

			// Execute the task.
			AlwaysAssert(i_item >= 0);
			AlwaysAssert(i_item < 10000);
			dapscitItems[i_item]->Execute();

		#if bLOG_SCHEDULE

			// Debug log.
			dapscitItems[i_item]->LogItem(conSchedule);
			conSchedule.Print(" *");

		#endif // bLOG_SCHEDULE
		}

		psRenderSchedulePriority.Add(ctmr());

		// Initialize performance flag.
		int i_num_done = i_item;
		u = i_item;

		// Execute remaining tasks.
		for (; u < dapscitItems.uLen; ++u)
		{
			// Test if there is still time remaining.
			if (uGetTime() >= uMSSlice || (dapscitItems[u]->fGetPriority() <= fNeverExecute))
				break;
			Assert(dapscitItems[u]);

			// Increment stats.
			++i_num_done;

			// Execute the task.
			dapscitItems[u]->Execute();

		#if bLOG_SCHEDULE

			// Debug log.
			dapscitItems[u]->LogItem(conSchedule);
			conSchedule.Print(" *");

		#endif // bLOG_SCHEDULE
		}

		psRenderScheduleOptional.Add(ctmr());
		
	#if bLOG_SCHEDULE

		// Log remaining tasks.
		for (; u < dapscitItems.uLen; ++u)
		{
			Assert(dapscitItems[u]);
			dapscitItems[u]->LogItem(conSchedule);
		}

	#endif // bLOG_SCHEDULE

		// Non-optional execution tasks.
		u = 0;
		for (; u < dapscitItems.uLen; ++u)
		{
			Assert(dapscitItems[u]);
			dapscitItems[u]->PostScheduleExecute();
		}

	#if bLOG_SCHEDULE

		// Log Scheduled items.
		LogItemCompletion(i_num_done, dapscitItems.uLen);

	#endif // bLOG_SCHEDULE

		// Clear the heaps for reuse.
		Clear();
		psRenderSchedulePost.Add(ctmr());

#if (VER_TIMING_STATS)
		// Avoid counting time twice.
		TCycles cy_scheduler = ctmr_scheduler();

		// Remove rendering & rasterizing time.
		cy_scheduler -= cyAccountedScheduleCycles;

		psRenderSchedule.Add(cy_scheduler, dapscitItems.uLen);
#endif
	}

	//*****************************************************************************************
	//
	void CScheduler::StartTimer
	(
	)
	//
	// Sets the time stamp.
	//
	//**************************************
	{
		uTimeMS = GetTickCount();
	}

	//*****************************************************************************************
	//
	uint CScheduler::uGetTime
	(
	) const
	//
	// Returns the difference in time between the time stamp time and the current time.
	//
	//**************************************
	{
		return GetTickCount() - uTimeMS;
	}


//
// Module specific function implementations.
//

#if bLOG_SCHEDULE

	//*************************************************************************************
	void LogScheduledItems()
	{
		//
		// Make sure the cache schedule file is open and active.
		//
		static bool b_file_open = false;

		if (!conSchedule.bIsActive())
			conSchedule.SetActive();

		if (!b_file_open)
		{
			b_file_open = true;
			conSchedule.OpenFileSession("Schedule.txt");
		}

		//
		// Output header for this frame.
		//
		static int i_frame = 0;
		conSchedule.Print("\n\n\nFrame: %ld *******************************\n\n", i_frame++);
	}

	//*****************************************************************************************
	void LogItemCompletion(int i_num_done, int i_num_total)
	{
		conSchedule.Print("\nNumber done:  %ld", i_num_done);
		conSchedule.Print("\nNumber total: %ld", i_num_total);
		if (i_num_total)
			conSchedule.Print("\nPercent done: %ld", (i_num_done * 100) / i_num_total);
	}

#endif // bLOG_SCHEDULE


//
// Global function implementations.
//

//*********************************************************************************************
void* operator new(uint u_size_type, CScheduler& sch, uint u_alignment)
{
	sch.pfhHeap->Align(u_alignment);
	return operator new(u_size_type, *sch.pfhHeap);
}


//
// Variable allocations.
//
bool CScheduler::bUseScheduler = true;

CScheduler shcScheduler(10);
CScheduler shcSchedulerTerrainTextures(5);

