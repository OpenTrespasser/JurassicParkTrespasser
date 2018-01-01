/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		TMSec typedef
 *		TSec typedef
 *		class CTimer
 *
 * Bugs:
 *
 * To do:
 *		Replace with higher resolution timer.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/Timer.hpp                                                     $
 * 
 * 5     96/11/20 17:07 Speter
 * Added msTime() functions to timer.  Rearranged implementation somewhat.
 * 
 * 4     96/11/05 16:14 Speter
 * Added calls to CTimer which return time in TSec (float).
 * 
 * 3     11/01/96 11:20a Agrant
 * Added TSec for time in seconds.
 * Added Conversion functions between TSec and TMSec
 * 
 * 2     10/15/96 9:15p Agrant
 * Added CTimerLocal which can pause independently of other timers.
 * 
 * 1     96/07/31 14:40 Speter
 * New module.
 * 
 * 3     96/05/23 17:05 Speter
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_TIMER_HPP
#define HEADER_LIB_STD_TIMER_HPP

//**********************************************************************************************
//
// Declarations for CTimer.
//

//**********************************************************************************************
typedef int32	TMSec;
// Prefix: ms
// A number of milliseconds.
//**************************

//**********************************************************************************************
typedef float	TSec;
// Prefix: s
// A number of seconds, fractional values possible.
//
//	Notes: 
//		TSec is not explicitly used in CTimer, but it is heavily related to TMSec, which is.
//
//**************************
#define fMILLISECONDS_PER_SECOND 1000.0

//******************************************************************************************
//
inline TSec sSeconds(TMSec ms) 
//
// Returns:
//		ms converted to seconds.
//
//**************************
{
	return ms / (TSec) fMILLISECONDS_PER_SECOND;
}

//******************************************************************************************
//
inline TMSec msTMSec(TSec s)
//
// Returns:
//		s converted to milliseconds.
//
//**************************
{
	return (TMSec) s * fMILLISECONDS_PER_SECOND;
}


//**********************************************************************************************
//
class CTimer
//
// Prefix: tmr
//
// Allows timing of events.
//
// Every time .msElapsed() is called, it returns the number of milliseconds since it was last
// called.  Therefore, sequences of events can be timed using only a single CTimer object.
//
// Example:
//		CTimer tmr;								// Initialise, remember current time.
//		PerformActivity();
//		TMSec ms_activity = tmr.msElapsed();	// Time since initialisation.
//		PerformFunction();
//		TMSec ms_function = tmr.msElapsed();	// Time since last msElapsed().
//
//**************************
{
protected:
	TMSec		msStart;					// Keep track of the last time called.

public:

	//******************************************************************************************
	//
	// Constructor.
	//

	CTimer();
	
	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	//
	void Reset();
	//
	// Resets the timer to 0.
	//
	//**************************

	//******************************************************************************************
	//
	TMSec msTime();
	//
	// Returns:
	//		Milliseconds since last reset.
	//
	//**************************

	//******************************************************************************************
	//
	TMSec msElapsed();
	//
	// Returns:
	//		Milliseconds since last call to this function.
	//
	// Notes:
	//		This function effectively returns the value of msTime(), then calls Reset().
	//
	//**************************

/*
	//******************************************************************************************
	//
	static TMSec msCurrent();
	//
	// Returns:
	//		The current time (relative) in milliseconds.
	//
	//**************************
*/

	//******************************************************************************************
	//
	TSec sTime()
	//
	// Returns:
	//		Seconds since last reset.
	//
	//**************************
	{
		return msTime() * 0.001;
	}

	//******************************************************************************************
	//
	TSec sElapsed()
	//
	// Returns:
	//		Seconds since last call to this.
	//
	//**************************
	{
		return msElapsed() * 0.001;
	}

/*
	//******************************************************************************************
	//
	static TSec sCurrent()
	//
	// Returns:
	//		The current time (relative) in seconds.
	//
	//**************************
	{
		return msCurrent() * 0.001;
	}
*/

	//******************************************************************************************
	//
	static void Pause
	(
		bool b_paused					// Whether app is paused.
	);
	//
	// Pauses or unpauses all timers.
	//
	// Notes:
	//		Call this when the app is paused (e.g. loses focus), and unpaused.  
	//		All subsequent timings for all timers will be adjusted by the amount of time the 
	//		app is paused.
	//
	//**************************

};


//**********************************************************************************************
//
class CTimerLocal : public CTimer
//
// Prefix: tmrl
//
// Allows timing of events, but each TimerLocal pauses or unpauses by itself and it
//	unaffected by the pause state of other timers.
//
//**************************
{
protected:
	bool	bPaused;					// true when paused.
	TMSec	msMyPauseAdjust;			
	TMSec	msMyPaused;

public:

	//******************************************************************************************
	//
	// Constructor.
	//

	CTimerLocal();
	
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual TMSec msElapsed();
	//
	// Returns:
	//		Milliseconds since last call.
	//
	//**************************

	//******************************************************************************************
	//
	virtual void Pause
	(
		bool b_paused					// Whether timer is paused.
	);
	//
	// Pauses or unpauses this timer only.  
	//
	// Notes:
	//		Hopefully, this will hide the Pause() function that pauses all timers.
	//
	//**************************


	//******************************************************************************************
	//
	void PauseMe
	(
		bool b_paused					// Whether timer is paused.
	);
	//
	// Pauses or unpauses this timer only.  
	//
	//**************************

};

#endif
