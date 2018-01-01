/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CSimClock for AI Test.  CSimClock handles simulation time based
 *		on real time.  It can pause or flow at varying rates.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/SimClock.hpp                                                  $
 * 
 * 3     9/27/96 5:44p Agrant
 * Added Trees
 * Added some notion of acceleration to dino model.
 * 
 * 2     8/29/96 5:31p Agrant
 * Major revision
 * Mostly changed code to match coding spec
 * Turned the object list into a class
 * Incorporated a timer
 * Now using TDefReal instead of float
 * 
 * 1     8/28/96 4:11p Agrant
 * initial revision
 *
 *********************************************************************************************/

#ifndef HEADER_TEST_AI_SIMCLOCK_HPP
#define HEADER_TEST_AI_SIMCLOCK_HPP

#include "Common.hpp"
#include "Lib/Sys/Timer.hpp"

#include "TestTypes.hpp"


//********************************************************************************************
//
class CSimClock
//
//	Prefix: clock
//
//	The CSimClock class is a stopwatch.  Time passes in periods called
//		"instants"
//
//	Example:
//		none
//
//********************************************************************************************
{

	//
	//  Variable declarations
	//

public:
	float	fTimeRatio;			// In seconds/second, how fast does
								//		this clock run?
	
	TMSec	msSimInstant;		// The Sim time elapsed in the last
								//		instant.
	TMSec	msRealInstant;		// The Real time elapsed in the last
								//		instant.
	CTimer  tmr;				// Provides windows clock access

	bool	bStopped;			// True when the clock is stopped.
	
	//
	//  Member function definitions
	//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	CSimClock
	(
		float f_ratio = 1   // fTimeRatio in sim-seconds per real-second
	) 
	:	fTimeRatio(f_ratio), tmr(), bStopped(false)
	{		
		// The last instant was short.
		msSimInstant = 0;
		msRealInstant = 0;
	}

	//*****************************************************************************************
	//
	//	Member functions
	//

		//*********************************************************************************
		//
		TMSec msTick
		(
		)
		//
		//	Advances the simulation clock and marks the passing of a new instant.
		//
		//	Returns:
		//		The duration of the instant in simulated milliseceonds.
		//
		//******************************
		{
			//  calculate length of new instant in real msec
			msRealInstant = tmr.msElapsed();

			//  calculate the length of the new instant in sim time
			msSimInstant = (TMSec)(msRealInstant * fTimeRatio);

			return msSimInstant;
		}

		//*********************************************************************************
		//
		void Stop
		(
		)
		//
		//	Stops the clock, freezing time.
		//
		//******************************
		{
			if (!bStopped)
			{
				tmr.Pause(true);
				bStopped = true;
			}
		}

		//*********************************************************************************
		//
		void Start
		(
		)
		//
		//	Starts the clock, unfreezing time.
		//
		//******************************
		{
			if (bStopped)
			{
				bStopped = false;
				tmr.Pause(false);
			}

		}


};

// #ifndef HEADER_TEST_AI_SIMCLOCK_HPP
#endif  
