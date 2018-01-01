/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of MentalState.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/MentalState.cpp                                              $
 * 
 * 15    8/28/98 5:05p Agrant
 * fixed anger restoring force (and a call to abs, which should have been Abs)
 * 
 * 14    8/25/98 2:12p Agrant
 * wander fix
 * 
 * 13    6/05/98 6:08p Agrant
 * Revised synthesizer to handle random-access acitivities.
 * Also handles exclusive ones (the old method).
 * 
 * 12    3/25/98 9:23p Agrant
 * Anger is at a low, but non-zero point by default
 * 
 * 11    3/17/98 9:59p Agrant
 * save AI emotional state
 * 
 * 10    2/23/98 5:07p Agrant
 * AI save/load
 * 
 * 9     12/22/97 6:45p Agrant
 * Make the dino try to go the direction it faces by default by setting the LastDirection var
 * to the forward along Y axis.
 * 
 * 8     7/31/97 4:40p Agrant
 * better starting emotional state
 * 
 * 7     7/19/97 1:24p Agrant
 * Emotions now change over time, going back to their default values.
 * 
 * 6     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 5     4/07/97 4:03p Agrant
 * Closer to True Vision(tm) technology!
 * 
 * 4     3/28/97 2:00p Agrant
 * Brain now points to graph, mentalstate, and worldview
 * Pathfinding more efficient
 * 
 * 3     2/16/97 5:37p Agrant
 * Even better pathfinding.
 * 
 * 2     11/20/96 1:26p Agrant
 * Now using world database queries for perception.
 * Now using archetypes for default personality behaviors.
 * 
 * 1     11/18/96 2:49p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "MentalState.hpp"

#include "AIInfo.hpp"
#include "Lib/Loader/SaveBuffer.hpp"

//*********************************************************************************************
//
//	Class CMentalState implementation.
//

	//*********************************************************************************
	CMentalState::CMentalState() : v2LastDirection(0,0)
	{

		feelDefault[eptFEAR]		= 0.2f;
		feelDefault[eptLOVE]		= 0.2f;
		feelDefault[eptCURIOSITY]	= 0.2f;
		feelDefault[eptANGER]		= 0.2f;
		feelDefault[eptHUNGER]		= 1.0f;
		feelDefault[eptTHIRST]		= 1.0f;
		feelDefault[eptFATIGUE]		= 0.0f;
		feelDefault[eptPAIN]		= 0.0f;
		feelDefault[eptSOLIDITY]	= 1.0f;

		feelEmotions = feelDefault;

		feelEmotions[eptHUNGER]		= 0.5f;
		feelEmotions[eptTHIRST]		= 0.5f;



		// The value in restoring force to get the maximum  restoring time to be "s" seconds.
#define STIME(s) (1.0f/s)	
		// The value in restoring force to get the maximum  restoring time to be "m" minutes.
#define MTIME(m) (1.0f/(m*60.0f))	

		feelRestoringForce[eptFEAR]			= 0;//MTIME(5);
		feelRestoringForce[eptLOVE]			= 0;//MTIME(30);
		feelRestoringForce[eptANGER]		= 0;//MTIME(30);
		feelRestoringForce[eptCURIOSITY]	= 0;//MTIME(1);
		feelRestoringForce[eptHUNGER]		= 0;//MTIME(60);
		feelRestoringForce[eptTHIRST]		= 0;//MTIME(15);
		feelRestoringForce[eptFATIGUE]		= 0;//MTIME(5);
		feelRestoringForce[eptPAIN]			= STIME(2);
		feelRestoringForce[eptSOLIDITY]		= 0.0f;
	}




	//*********************************************************************************
	void CMentalState::RestoreEmotions
	(
		TSec		s_time	// How long since the last restoration.
	)
	{
		// How much to restore this time?
		CFeeling feel_restore = feelRestoringForce * s_time;

		for (int i = eptEND - 1; i >= 0; --i)
		{
			// Our restoring force had better be positive.
			Assert(feel_restore[i] >= 0);

			// Renormalize 0 to 1.
			if (feelEmotions[i] > 1.0f)
				feelEmotions[i] = 1.0f;
			else if (feelEmotions[i] < 0)
				feelEmotions[i] = 0;

			TReal r_off = feelEmotions[i] - feelDefault[i];

			if (Abs(r_off) <= feel_restore[i])
			{
				// Close enough to go back to default.
				feelEmotions[i] = feelDefault[i];
			}
			else if (r_off <= 0)
			{
				// Go upwards as far as we need to.
				feelEmotions[i] += feel_restore[i];
			}
			else
			{
				// Must go downwards to restore.
				feelEmotions[i] -= feel_restore[i];
			}
		}
	}






	//*****************************************************************************************
	char *CMentalState::pcSave(char * pc) const
	{
		pc = pcSaveT(pc, feelEmotions);

		return pc;
	}

	//*****************************************************************************************
	const char *CMentalState::pcLoad(const char * pc)
	{
		pc = pcLoadT(pc, &feelEmotions);

		return pc;
	}








