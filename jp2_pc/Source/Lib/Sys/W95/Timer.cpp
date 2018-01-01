/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Timer.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/W95/timer.cpp                                                 $
 * 
 * 7     96/11/20 17:07 Speter
 * Added msTime() functions to timer.  Rearranged implementation somewhat.
 * 
 * 6     96/11/11 15:27 Speter
 * Fixed (for sure this time!) erroneous behaviour of Pause function.
 * 
 * 5     96/11/08 18:41 Speter
 * Oops.  Added bPaused code to wrong timer class.
 * 
 * 4     96/11/04 17:00 Speter
 * Added code to make sure multiple Pause() calls pause only once.
 * 
 * 3     10/15/96 9:15p Agrant
 * Added CTimerLocal which can pause independently of other timers.
 * 
 * 2     10/08/96 9:12p Agrant
 * Now includes WinInclude.h
 * 
 * 1     96/08/14 10:33 Speter
 * Now in W95 subdirectory.  Uses Windows GetCurrentTime() function, which is actually
 * millisecond-accurate, as opposed to clock().
 * 
 * 1     96/07/31 14:39 Speter
 * New module.
 * 
 * 3     96/05/23 17:01 Speter
 * Moved memset functions to new Lib/Std/Mem module.
 * 
 * 2     96/04/24 12:55 Speter
 * Test files for RasterTest project now converted to work in our common code base.  Still not
 * style-compliant.
 * 
 * 1     96/04/23 16:22 Speter
 * These are all test files to help test raster, geom, etc.
 * They are not JP2PC-style compatible.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "../Timer.hpp"

// Uses the Windows GetCurrentTime() function.
#include <Lib\W95\WinInclude.hpp>

//**********************************************************************************************
//
// CTimer implementation.
//

	static TMSec msPauseAdjust;

	//******************************************************************************************
	static inline TMSec msCurrent()
	{
		return GetCurrentTime() - msPauseAdjust;
	}

	//******************************************************************************************
	CTimer::CTimer()
	{ 
		Reset();
	}

	//******************************************************************************************
	void CTimer::Reset()
	{ 
		// Initialise to current time.
		msStart = msCurrent();
	}

	//******************************************************************************************
	TMSec CTimer::msTime()
	{ 
		return msCurrent() - msStart;
	}

	//******************************************************************************************
	TMSec CTimer::msElapsed()
	{ 
		TMSec ms = msCurrent() - msStart;

		// Bump msStart to reflect current time.
		msStart += ms;
		return ms;
	}

	//******************************************************************************************
	void CTimer::Pause(bool b_paused)
	{
		static bool bPaused = false;
		static CTimer tmrPause;

		if (bPaused == b_paused)
			return;
		bPaused = b_paused;
				
		if (b_paused)
			// Remember at what time we paused, so we can adjust later.
			tmrPause.msElapsed();
		else
			// Store the amount of time we paused, for adjustment.
			msPauseAdjust += tmrPause.msElapsed();
	}

//**********************************************************************************************
//
// CTimerLocal implementation.
//

	//******************************************************************************************
	CTimerLocal::CTimerLocal() : CTimer()
	{ 
		// We have not yet been paused.
		msMyPauseAdjust = 0;
	}

	//******************************************************************************************
	TMSec CTimerLocal::msElapsed()
	{ 
		// Get current time from clock().  Subtract last time saved, and also subtract any
		// pause adjustment.
		TMSec ms = msCurrent() - msStart - msMyPauseAdjust;

		// Bump msStart to reflect current time.
		msStart += ms;
		return ms;
	}

	//******************************************************************************************
	void CTimerLocal::Pause(bool b_paused)
	{
		if (b_paused)
			// Remember at what time we paused, so we can adjust later.
			msMyPaused = msCurrent();	
		else
			// Store the amount of time we paused, for adjustment.
			msMyPauseAdjust += msCurrent() - msMyPaused;
	}
