/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of GameLoop.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/GameLoop.cpp                                         $
 * 
 * 24    98/04/09 20:34 Speter
 * Added ability to replay with in specified step increments.
 * 
 * 23    3/18/98 4:07p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 * 22    98/03/06 10:55 Speter
 * Added CCamera& arg to CMessagePaint.
 * 
 * 21    11/11/97 9:29p Agrant
 * Empty the Trash list on deleting the GameLoop
 * 
 * 20    9/18/97 12:25p Rwyatt
 * Memory dump has been removed and put onto a dialog
 * 
 * 19    97/09/11 20:55 Speter
 * .Step() now optionally take a step time.  
 * 
 * 18    7/23/97 6:17p Rwyatt
 * Added the memory log key
 * 
 * 17    6/24/97 3:11p Rwyatt
 * Press B in debug build for a global heap status report
 * 
 * 16    5/30/97 11:13a Agrant
 * LINT tidying
 * 
 * 15    4/09/97 11:15p Rwyatt
 * Replay and Step members are now called at the correct times depending on the replay state.
 * 
 * 14    4/09/97 12:08p Rwyatt
 * Added log to file
 * 
 *********************************************************************************************/


#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "GameLoop.hpp"
#include "WorldDBase.hpp"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/Replay.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPaint.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/FrameHeap.hpp"
#include "Lib/EntityDBase/QueueMessage.hpp"
#include "Lib/EntityDBase/MessageLog.hpp"
#include "Lib/Renderer/Pipeline.hpp"

#include "Lib/sys/MemoryLog.hpp"
//
// Class implementations.
//


//*********************************************************************************************
//
// CGameLoop implementation.
//

	//*****************************************************************************************
	//
	// CGameLoop constructor.
	//

	//*****************************************************************************************
	CGameLoop::CGameLoop()
	{
		// Set default values.
		egmGameMode = egmDEBUG;
		bPauseGame  = false;
	}

	//*****************************************************************************************
	CGameLoop::~CGameLoop()
	{
		EmptyTrash();
	}

	//*****************************************************************************************
	//
	// CGameLoop member functions.
	//

	//*****************************************************************************************
	bool CGameLoop::bCanStep()
	{
		return (egmGameMode == egmPLAY) && (!bPauseGame);
	}

	//*****************************************************************************************
	void CGameLoop::Step(TSec s_step)
	{
		qmQueueMessage.Play(true, s_step);
		qmQueueMessage.Flip();

		// Delete old instances.
		EmptyTrash();
	}


	//*****************************************************************************************
	//
	// Extract one frame from replay file and then processes the resulting queue (if any) much
	// the same way as the above Step function does. A frame in the replay file is considered
	// to be from one step message to the next.
	//
	// NOTE: that play is called with a false parameter to prevent it generating its own step
	// message.
	//
	void CGameLoop::Replay(TSec s_step)
	{
		crpReplay.RunFrame(s_step);

		qmQueueMessage.Play(FALSE);
		qmQueueMessage.Flip();

		// Delete old instances.
		EmptyTrash();
	}


	//*****************************************************************************************
	void CGameLoop::Paint() const
	{
		// Specify main renderer and camera.
		CMessagePaint msgpaint(*prenMain, *CWDbQueryActiveCamera().tGet());
		msgpaint.Send();
	}

	//*****************************************************************************************
	void CGameLoop::EmptyTrash
	(
	)
	{
		// If CInstances were ref counted, this code might work:
//		foreach(lpinsTrash)
//		{
//			// This ought to be the last reference to the object.
//			Assert(lpinsTrash.tGet()->uNumRefs() == 1);
//		}

//		lpinsTrash.erase(lpinsTrash.Begin(), lpinsTrash.end());

		// If CInstances were ref counted, this code might work:
		foreach(lpinsTrash)
		{
			// This ought to be the last reference to the object.
			CInstance *pins = lpinsTrash.tGet();
			
			// Remove from world dbase.
			wWorld.Remove(pins);

			// Finally, delete the instance.
			delete pins;
		}

		lpinsTrash.erase(lpinsTrash.begin(), lpinsTrash.end());
	}

//
// Global Variables.
//
CGameLoop gmlGameLoop;
