/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 *	Implementation of TestActivities.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/TestActivities.cpp                                           $
 * 
 * 21    9/06/98 2:32p Agrant
 * Another variant for testing jumps
 * 
 * 20    9/05/98 6:00p Agrant
 * constrain test head cock to a reasonable range
 * 
 * 19    8/26/98 7:15p Agrant
 * removed crouch and move from jump test
 * 
 * 18    8/26/98 11:16a Agrant
 * yet more jump test tweaks
 * 
 * 17    8/25/98 2:12p Agrant
 * better wag tail hack test
 * 
 * 16    8/24/98 11:09p Agrant
 * Use WagTail as a temporary jump test
 * 
 * 15    8/20/98 11:06p Agrant
 * somewhat better raptor head tests
 * 
 * 14    7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 13    6/27/98 9:28p Agrant
 * Updated descriptions or activities
 * 
 * 12    6/27/98 6:13p Agrant
 * Please put head test somewhere where raptors can reach it
 * 
 * 11    6/26/98 6:57p Agrant
 * remove bogus pinf asserts
 * 
 * 10    6/14/98 8:39p Agrant
 * Better mouth test
 * 
 * 9     5/16/98 8:29p Agrant
 * fixed distance printout in head posit
 * 
 * 8     5/16/98 7:03p Agrant
 * print distance from desired location for test head move to
 * 
 * 7     5/01/98 1:37p Agrant
 * test head position with random speed
 * 
 * 6     2/13/98 1:53p Agrant
 * Rework multipliers, continues, and was used last time data
 * 
 * 5     2/11/98 12:57p Agrant
 * More complete descriptions for test activities.
 * 
 * 4     2/06/98 12:55p Agrant
 * Fixed typo
 * 
 * 3     2/06/98 12:53p Agrant
 * Better head position test debug info
 * 
 * 2     12/18/97 7:35p Agrant
 * Better grouped behavior through the CONTINUE flag.
 * 
 * 1     12/17/97 9:28p Agrant
 * Initial revision
 * 
 **********************************************************************************************/

#include "Common.hpp"

#include "TestActivities.hpp"
#include "AIMain.hpp"

#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"

#include "Game/DesignDaemon/DaemonScript.hpp"

#include <stdio.h>


#define RAND01 (gaiSystem.rnd(0.0f, 1.0f))
#define RAND11 (gaiSystem.rnd(-1.0f, 1.0f))
#define RAND1010 (10.0f*RAND11)

#define PI 3.14159265f


//*********************************************************************************************
//
//	Class CActivityTest implementation.
//

	CActivityTest::CActivityTest
	(
		const char* str_name
	) : CActivity(str_name)
	{
		// Set up the rating feeling to be generically high.
		feelRatingFeeling[eptFEAR]		= 0.5f;
		feelRatingFeeling[eptLOVE]		= 0.5f;
		feelRatingFeeling[eptANGER]		= 0.5f;
		feelRatingFeeling[eptCURIOSITY] = 0.5f;
		feelRatingFeeling[eptHUNGER]	= 0.5f;
		feelRatingFeeling[eptTHIRST]	= 0.5f;
		feelRatingFeeling[eptFATIGUE]	= 0.5f;
		feelRatingFeeling[eptPAIN]		= 0.5f;
		feelRatingFeeling[eptSOLIDITY]  = 0.5f;

		// Set up the timer.
		sDuration	= 15.0f;
		sStart		= -1000.0f;
		sStop		= -1000.0f;

	};


#if VER_TEST
	int CActivityTest::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);

		char local_buffer[512];

		sprintf(local_buffer, "\n\nTest Duration: %f sec\n", sDuration);
		strcat(buffer, local_buffer);
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//	Class CActivityTestHeadCock implementation.
//


	//*********************************************************************************
	void CActivityTestHeadCock::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		if (sStart == sStop)
		{
			// Then we are just beginning!
			sStart	= CMessageStep::sStaticTotal;
			sStop	= sStart + sDuration;

			fAngle = fRandom(-PI * 0.35f, PI * 0.35f);
		}

		// Tell physics to move head to target at debug speed.
		DoCockHead(rt_importance, fAngle, fAIDebug1);

		// Should we continue this action?
		if (CMessageStep::sStaticTotal < sStop)
		{
			// Yes!
			//  We should try to keep testing next cycle.
			TryToContinue();			
			char buffer[256];
			sprintf(buffer, "Head Cock:%f\n", fAngle);
			DebugSay(buffer);
		}
		else
		{
			// No!  Stop this nonsense.
			sStart = sStop;
		}
	}

#if VER_TEST
	int CActivityTestHeadCock::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivityTest::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nCocks the dino's head around the nose vector.\n");

		strcat(buffer, "\n\nNot supported by physics.\n");
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityTestHeadOrient implementation.
//

	//*********************************************************************************
	void CActivityTestHeadOrient::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		if (sStart == sStop)
		{
			// Then we are just beginning!
			sStart	= CMessageStep::sStaticTotal;
			sStop	= sStart + sDuration;

			v3Target = CVector3<>(RAND11, RAND11, RAND11);
		}

		// Tell physics to move head to target at debug speed.
		DoPointHead(rt_importance, v3Target, fAIDebug1);

		// Should we continue this action?
		if (CMessageStep::sStaticTotal < sStop)
		{
			// Yes!
			//  We should try to keep testing next cycle.
			TryToContinue();
			char buffer[256];
			sprintf(buffer, "Head Orient:%f, %f, %f\n", v3Target.tX, v3Target.tY, v3Target.tZ);
			DebugSay(buffer);
		}
		else
		{
			// No!  Stop this nonsense.
			sStart = sStop;
		}
	}

#if VER_TEST
	int CActivityTestHeadOrient::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivityTest::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nPoints the dino nose vector in a random.\n");

		strcat(buffer, "\n\nREADY.\nWhen directed to look behind self, body turns very slowly.  Not fully supported by physics.\n");
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityTestHeadPosition implementation.
//

	//*********************************************************************************
	void CActivityTestHeadPosition::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		if (sStart == sStop)
		{
			// Then we are just beginning!
			sStart	= CMessageStep::sStaticTotal;
			sStop	= sStart + sDuration;

			v3Target = CVector3<>(RAND1010, RAND1010, RAND01 + 0.25f);

			rtSpeed = RAND01;
			if (rtSpeed < 0.25f)
				rtSpeed = 1.0f;
		}

		// Tell physics to move head to target at debug speed.
		DoMoveHeadTo(rt_importance, v3Target, rtSpeed);

		// Should we continue this action?
		if (CMessageStep::sStaticTotal < sStop)
		{
			// Yes!
			//  We should try to keep testing next cycle.
			TryToContinue();
			char buffer[256];
			sprintf(buffer, "Target Head Pos:%f, %f, %f\n", v3Target.tX, v3Target.tY, v3Target.tZ);
			DebugSay(buffer);
			CVector3<> v3_head = gaiSystem.v3GetHeadLocation();
			sprintf(buffer, "Real   Head Pos:%f, %f, %f\n", v3_head.tX, v3_head.tY, v3_head.tZ);
			DebugSay(buffer);
			CVector3<> v3_body = gaiSystem.v3GetBodyLocation();
			sprintf(buffer, "Real   Body Pos:%f, %f, %f\n", v3_body.tX, v3_body.tY, v3_body.tZ);
			DebugSay(buffer);
			sprintf(buffer, "Speed: %f\tDistance: %f\n", rtSpeed.fVal, rDISTANCE(v3Target, gaiSystem.v3GetHeadLocation()));
			DebugSay(buffer);
		}
		else
		{
			// No!  Stop this nonsense.
			sStart = sStop;
		}
	}


#if VER_TEST
	int CActivityTestHeadPosition::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivityTest::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nDino puts its head in a random spot in the world (near the origin).\n\n");

		strcat(buffer, "\n\nREADY.\nPhysics misses the target a bit.\nAlmost supported by physics.\n");
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityRam implementation.
//

	//*********************************************************************************
	void CActivityTestWagTail::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		if (!bWasChosen())
		{
			static TSec s = 0.0f;
			sStop = 2.0;
//			s += 0.05;
			sStart = sStop + gaiSystem.sNow;
			sFlags[eafPHASEONE] = false;
			sGiveUp = sStart + 2.0f;

			// Alternate between always moving forward and only moving forward before the jump.
			sFlags[eafPHASETWO] = !sFlags[eafPHASETWO];

			// When true, always moves forward.
//			sFlags[eafPHASETWO] = true;
		}

		if (sGiveUp < gaiSystem.sNow)
		{
			DontContinue();
		}

		if (sStart < gaiSystem.sNow)
		{
			if (!sFlags[eafPHASEONE])
			{
				DoJump(rt_importance, 1.0f);
				DebugSay("Jump\n");
				sFlags[eafPHASEONE] = true;
			}

			// If flag is set, always move forward.
			if (sFlags[eafPHASETWO])
			{
				DoMoveInDirection(rt_importance, CVector2<>(0,1), 1.0);
				DebugSay("Moving forward\n");
			}
			else
			{
				DebugSay("No move command\n");
			}

		}
		else
		{
//			DoCrouch(rt_importance, 1.0f);
//			DebugSay("Crouch\n");
			DoMoveInDirection(rt_importance, CVector2<>(0,1), 1.0);

		}
	}


#if VER_TEST
	int CActivityTestWagTail::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivityTest::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nWags the tail with a random amplitude and frequency.\n");

		strcat(buffer, "\n\nNot supported by physics.\n");
		return strlen(buffer);
	}
#endif



//*********************************************************************************************
//
//	Class CActivityTestMouth implementation.
//

	//*********************************************************************************
	void CActivityTestMouth::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		sDuration = 3.0f;

		if (sStart == sStop)
		{
			// Then we are just beginning!
			sStart	= CMessageStep::sStaticTotal;
			sStop	= sStart + sDuration;

			rtOpen = RAND01;
			rtSpeed = RAND01;
		}

		// Tell physics to move head to target at debug speed.
		DoOpenMouth(rt_importance, rtOpen, rtSpeed);

		// Should we continue this action?
		if (CMessageStep::sStaticTotal < sStop)
		{
			// Yes!
			//  We should try to keep testing next cycle.
			TryToContinue();
			char buffer[256];
			sprintf(buffer, "Mouth Open: %f\tMouth Speed: %f", rtOpen.fVal, rtSpeed.fVal);
			DebugSay(buffer);
		}
		else
		{
			// No!  Stop this nonsense.
			sStart = sStop;
		}
	}


#if VER_TEST
	int CActivityTestMouth::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivityTest::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMoves the jaw to a random openness with random speed.\n");

		strcat(buffer, "\n\nREADY.\nSpeed ignored.\n");
		return strlen(buffer);
	}
#endif
