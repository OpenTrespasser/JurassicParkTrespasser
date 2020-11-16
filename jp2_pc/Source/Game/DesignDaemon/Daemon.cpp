/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: The implementation of Daemon.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/Daemon.cpp                                         $
 * 
 * 27    8/13/98 6:10p Mlange
 * Paint message now requires registration of the recipients.
 * 
 * 26    8/13/98 1:49p Mlange
 * Removed unused step message process function.
 * 
 * 25    3/22/98 5:01p Rwyatt
 * New binary audio collisions
 * New binary instance hashing
 * 
 * 24    3/18/98 4:07p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 * 23    11/16/97 4:15a Agrant
 * Add new overlays at the front rather than the rear so that new ones show up immediately.
 * 
 *********************************************************************************************/

//
// Includes.
//

#include "gblinc/common.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Daemon.hpp"

#include "Lib/Sys/Textout.hpp"
#include "Lib/EntityDBase/MessageLog.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPaint.hpp"
#include "Lib/Sys/W95/Render.hpp"
	
//
// Module functions.
//

//*********************************************************************************************
//
bool bExecuteScriptFunction
(
	bool (*bFunction)()
)
//
// Returns the result of a function execution.
//
//**************************************
{
	return bFunction();
}


//
// Class implementations.
//

//*********************************************************************************************
//
// CDaemon implementation.
//


	//*****************************************************************************************
	//
	// CDaemon constructor.
	//

	//*****************************************************************************************
	CDaemon::CDaemon()
		: CSubsystem()
	{
		SetInstanceName("CDaemon");
	}


	CDaemon::~CDaemon()
	{
	}


	//*****************************************************************************************
	//
	// CDaemon protected member functions.
	//

	//*****************************************************************************************
	//
	void CDaemon::Dump
	(
	)
	//
	// Dumps the contents of the design daemon into a text file.
	//
	//**************************************
	{
		// Open file for debug printout.
		CConsoleBuffer conDaemon(80, 25);
		conDaemon.OpenFileSession("Daemon.txt");
		conDaemon.Print("\nFunctions Registered with the Design Daemon:");
		conDaemon.Print("\n--------------------------------------------\n\n");
		// Close the file for debug printout.
		conDaemon.Print("\nFinished list of registered functions.\n");
		conDaemon.CloseFileSession();
	}


//
// Externally defined functions.
//

//*********************************************************************************************
void Say(const char* str)
{
	Assert(str);

	conMessageLog.Print("\n");
	conMessageLog.Print(str);
	conMessageLog.Print("\n\n");
}

//*********************************************************************************************
void VoiceOver(const char* str)
{
	Assert(0);
	// not yet implmented
}

//*********************************************************************************************
float fRandom(float f_a, float f_b)
{
	float f_difference = f_b - f_a;

	return f_difference * float(rand() & 1023) / 1023.0f + f_a;
}

//*********************************************************************************************
int iRandom(int i_a, int i_b)
{
	int i_difference = i_b - i_a + 1;

	return (rand() % i_difference) + i_a;
}

//*********************************************************************************************
//
// CVideoOverlay implementation.
//

	//*****************************************************************************************
	CVideoOverlay::CVideoOverlay()
	{
		msPeriod = 0;
		tmrCycle.Reset();
		bEnabled_ = true;

		// Register this entity with the message types it needs to receive.
		CMessagePaint::RegisterRecipient(this);
	}

	//*****************************************************************************************
	CVideoOverlay::~CVideoOverlay()
	{
		CMessagePaint::UnregisterRecipient(this);
	}

	//*****************************************************************************************
	void CVideoOverlay::AddOverlay(const SOverlay& ovl)
	{
		lsovlOverlays.push_front(ovl);
		lsovlOverlays.Begin();
		tmrCycle.Reset();
	}

	//*****************************************************************************************
	void CVideoOverlay::SetPeriod(TMSec ms_period)
	{
		Assert(ms_period >= 0);
		msPeriod = ms_period;
		tmrCycle.Reset();
	}

	//*****************************************************************************************
	void CVideoOverlay::Process(const CMessagePaint& msgpaint)
	{
		if (!bEnabled_ || lsovlOverlays.empty())
			return;

		SOverlay& ovl = *lsovlOverlays;

		// Blit this puppy on the screen.
		CVector2<int> v2i_pos = ovl.v2iOverlayPos;
		if (v2i_pos.tX < 0)
			v2i_pos.tX += prasMainScreen->iWidth - ovl.prasOverlay->iWidth + 1;
		if (v2i_pos.tY < 0)
			v2i_pos.tY += prasMainScreen->iHeight - ovl.prasOverlay->iHeight + 1;

		if (ovl.bCentered)
		{
			v2i_pos.tX = (prasMainScreen->iWidth - ovl.prasOverlay->iWidth) / 2;
			v2i_pos.tY = (prasMainScreen->iHeight - ovl.prasOverlay->iHeight) / 2;
		}


		prasMainScreen->Blit
		(
			v2i_pos.tX, v2i_pos.tY,			// Position.
			*ovl.prasOverlay,				// Source raster.
			0,								// Entire rectangle.
			true,							// G'ahead and clip.
			true,							// Transparent blit.
			0								// Transparent pixel.
		);

		if (tmrCycle.msTime() >= msPeriod)
		{
			// Discard this overlay?
			if (ovl.bThrowaway)
			{
				// Yes!  erase it.
				lsovlOverlays.erase(lsovlOverlays.itContainer);
				lsovlOverlays.Begin();
			}
			else
			{
				// No! Advance to next overlay.
				++lsovlOverlays;
				if (!lsovlOverlays.bIsNotEnd())			// That means it's the end.
				{
					lsovlOverlays.Begin();
				}				
			}
			tmrCycle.Reset();
		}
	}

	CVideoOverlay* pVideoOverlay;
