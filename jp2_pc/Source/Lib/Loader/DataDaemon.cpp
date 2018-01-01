/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Implementation of 'Loader.hpp.'
 *
 * Bugs:
 *
 * To do:
 *		
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/DataDaemon.cpp                                            $
 * 
 * 27    8/13/98 1:40p Mlange
 * The step and system messages now require registration of their recipients.
 * 
 * 26    7/22/98 10:05p Agrant
 * Removed data daemon and pre-fetching
 * 
 * 25    4/21/98 3:11p Rwyatt
 * Remove the page fault stats that were based on the old VM system.
 * 
 * 24    3/22/98 5:03p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 23    3/18/98 4:07p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 *********************************************************************************************/

#if 0
//
// Includes.
//
#include "Common.hpp"
#include "DataDaemon.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"

#include "Lib/Loader/ASyncLoader.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/Renderer/Camera.hpp"

#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/Profile.hpp"

#include "Lib/Loader/SaveBuffer.hpp"


//
// Global variables.
//

// The data manager for the app.
CDataDaemon* pddDataDaemon = 0;


//*********************************************************************************************
//
// CDataDaemon implementation.
//

#define rDIST 5.0f

// defined when we are using threaded preloading.
#define PRELOAD

	//*********************************************************************************************
	CDataDaemon::CDataDaemon()
	{
		SetInstanceName("Data Daemon");


		v3LastPrefetchPosition	= CVector3<>(0,0,-10000);	// Where was the camera at the last prefetch?
		rUpdateDistSqr			= rDIST * rDIST;				// How far can we move before we must update again? (squared)
		rDistancePastFarClip	= rDIST * 2.0;				// How far past the far clipping plane do we prefetch?
		sLastPrefetchTime		= -100;						// When was our last prefetch update?
		sPrefetchInterval		= 5.0f;
		bThreadedLoad			= false;

//		pcWastedMem = 0;
//		iWastedMem	= 0;


		//  Scheduler priority.
		SetPriority(FLT_MAX);

		Assert(pddDataDaemon == 0);
		pddDataDaemon = this;
		
		shcScheduler.AddExecution(this);

		// Register this entity with the message types it needs to receive.
		CMessageStep::RegisterRecipient(this);
	}


	//*********************************************************************************************
	CDataDaemon::~CDataDaemon()
	{
		CMessageStep::UnregisterRecipient(this);

		aslLoader.RemoveTouchBlocks();
		pddDataDaemon = 0;
//		if (pcWastedMem)
//		{
//			VirtualFree(pcWastedMem, 0, MEM_RELEASE);
//		}
	}


	//*****************************************************************************************
	TReal CDataDaemon::rFetchDistance
	(
		TReal r_clip_plane_distance		// distance to the clipping plane
	)
	{
		TReal r_dist = r_clip_plane_distance + rDistancePastFarClip;	
		r_dist = min(r_dist, 100.0f);
		return r_dist;
	}


	//*********************************************************************************************
	void CDataDaemon::Process(const CMessageStep& msgstep)
	{
		if (!bThreadedLoad)
			return;

		// If we have moved far enough since the last prefetch, fetch again!
		if (CMessageStep::sStaticTotal > sLastPrefetchTime + sPrefetchInterval ||		// it's been a while OR
			(CWDbQueryActiveCamera().tGet()->v3Pos() - v3LastPrefetchPosition).tLenSqr() > rUpdateDistSqr)  // We've moved a ways
			shcScheduler.AddExecution(this);
	}

	//*****************************************************************************************
	void CDataDaemon::Execute()
	{
		// Are we preloading?
		if (!bThreadedLoad)
			// No!  Return.
			return;

		// If we have moved far enough since the last prefetch, fetch again!
		if (CMessageStep::sStaticTotal > sLastPrefetchTime + sPrefetchInterval ||		// it's been a while OR
			(CWDbQueryActiveCamera().tGet()->v3Pos() - v3LastPrefetchPosition).tLenSqr() > rUpdateDistSqr)  // We've moved a ways
		{
			PrefetchWorld();
		}
	}

	//*****************************************************************************************
	void CDataDaemon::PostScheduleExecute()
	{
	}

	//*********************************************************************************************
	void CDataDaemon::PrefetchWorld()
	{
		Assert(bThreadedLoad);

		// Increment the prefetch counter.
		++cLastPrefetch;
		
		v3LastPrefetchPosition	= CWDbQueryActiveCamera().tGet()->v3Pos();
		sLastPrefetchTime		= CMessageStep::sStaticTotal;


		CCycleTimer ctr;


		wWorld.Preload();
//		if (pcWastedMem)
//			Prefetch(pcWastedMem, false, iWastedMem);		

		uint32 u4 = ctr();

		if (u4 * ctr.fSecondsPerCycle() > 0.10f)
		{
			dout << "Long Prefetch query, " << u4 * ctr.fSecondsPerCycle() << "seconds taken.\n";

		}
//		dout << "Prefetch query, " << ctr() * 1000.0f * ctr.fSecondsPerCycle() << "ms taken.\n";
//		dout << "Hits: " << iFetchHits << "\t\tMisses: " << iFetchMisses << '\n';
		iFetchMisses = 0;
		iFetchHits = 0;
	}

	//*****************************************************************************************
	void CDataDaemon::Prefetch
	(
		TDataHandle	dh_data,
		bool		b_in_thread,
		int			i_size
	)
	{
#ifdef PRELOAD
		// Tell the loader thread to prefetch dh_data.
		if (bThreadedLoad && dh_data)
		{
			++iFetchMisses;

			// Prefetch the data pointed to by pv_data.
			aslLoader.AddTouchBlock
			(
				(void*)dh_data, 
				i_size
			);
		}
#endif
	}

	//*****************************************************************************************
	void CDataDaemon::Prefetch
	(
		TFetchableHandle	fh_data,
		bool				b_in_thread
	)
	{
#ifdef PRELOAD
		// Tell the loader thread to prefetch dh_data.
		if (bThreadedLoad && fh_data)
		{
			// Fetch the CFetchable pointed to by fh_data
			aslLoader.AddTouchBlock
			(
				(void*)fh_data,
				fh_data->iSize(), 
				(CFetchable *)fh_data
			);

			++iFetchMisses;
		}
#endif
	}

	//*****************************************************************************************
	void CDataDaemon::Prefetch
	(
		TFetchableHandle2	fh_data,
		bool				b_in_thread
	)
	{
#ifdef PRELOAD
		// Tell the loader thread to prefetch dh_data.
		if (bThreadedLoad && fh_data)
		{
			++iFetchMisses;

			// fetch the CFetchable pointed to by fh_data
			aslLoader.AddTouchBlock
			(
				(void*)fh_data.ptPtrRaw(),
				fh_data->iSize(), 
				(CFetchable *)fh_data.ptPtrRaw()
			);
		}
#endif
	}

	//*****************************************************************************************
	//
	void CDataDaemon::PrefetchNext
	(
		TDataHandle	dh_data,
		int			i_size
	)
	{
		// This implementation will not work.
		Assert(false);
	}

	//*****************************************************************************************
	void CDataDaemon::PrefetchNext
	(
		TFetchableHandle	fh_data
	)
	{
		// This implementation will not work.
		Assert(false);
	}




	//*****************************************************************************************
	char *CDataDaemon::pcSave
	(
		char * pc
	) const
	{
		pc = v3LastPrefetchPosition.pcSave(pc);
		pc = pcSaveT(pc, rUpdateDistSqr);
		pc = pcSaveT(pc, rDistancePastFarClip);
		pc = pcSaveT(pc, sLastPrefetchTime);
		pc = pcSaveT(pc, sPrefetchInterval);
		pc = pcSaveT(pc, bThreadedLoad);

		return pc;
	}

	//*****************************************************************************************
	const char *CDataDaemon::pcLoad
	(
		const char * pc
	)
	{
		pc = v3LastPrefetchPosition.pcLoad(pc);
		pc = pcLoadT(pc, &rUpdateDistSqr);
		pc = pcLoadT(pc, &rDistancePastFarClip);
		pc = pcLoadT(pc, &sLastPrefetchTime);
		pc = pcLoadT(pc, &sPrefetchInterval);
		pc = pcLoadT(pc, &bThreadedLoad);

		return pc;
	}

#endif  // 0
