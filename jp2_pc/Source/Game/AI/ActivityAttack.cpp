/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 *	Implementation of ActivityAttack.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/ActivityAttack.cpp                                           $
 * 
 * 55    9/26/98 9:11p Agrant
 * Eat better
 * 
 * 54    9/22/98 1:04p Agrant
 * better tearing stage for Eat
 * 
 * 53    9/20/98 7:25p Agrant
 * use the node me flag
 * mouth open for tear
 * 
 * 52    9/19/98 1:44a Agrant
 * cooler eating
 * 
 * 51    9/17/98 9:15p Agrant
 * Let's be really vicious about biting
 * 
 * 50    9/17/98 2:51p Agrant
 * Lead your target for bites
 * 
 * 49    9/16/98 9:21p Agrant
 * Better ram and bite
 * 
 * 48    9/16/98 12:28a Agrant
 * better debug
 * 
 * 47    9/14/98 8:21p Agrant
 * Better debug info
 * 
 * 46    9/12/98 1:08a Agrant
 * The new living flag for influences
 * 
 * 45    9/08/98 7:40p Agrant
 * Different behavior when target faces away from you
 * 
 * 44    9/06/98 7:49p Agrant
 * added some head cock 
 * 
 * 43    9/05/98 8:28p Agrant
 * Make attack cause damage
 * 
 * 42    9/02/98 5:25p Agrant
 * vocalizations do not require audio handle
 * 
 * 41    8/31/98 6:31p Agrant
 * massive dino resources reallocation
 * 
 * 40    8/31/98 5:08p Agrant
 * Don't eat living targets
 * 
 * 39    8/27/98 1:44a Agrant
 * eat & drink tweak
 * 
 * 38    8/26/98 7:13p Agrant
 * Major EAT DRINK rework
 * 
 * 37    8/22/98 7:22p Agrant
 * attacks unstartle
 * 
 * 36    8/20/98 11:11p Agrant
 * implemented tail swipe attack
 * 
 * 35    8/16/98 3:21p Agrant
 * Much tweaking to simplify activity balancing
 * 
 * 34    8/11/98 2:22p Agrant
 * minor bite improvements
 * 
 **********************************************************************************************/

#include "Common.hpp"

#include "ActivityAttack.hpp"
#include "AIMain.hpp"
#include "Brain.hpp"

#include "Lib/EntityDBase/PhysicsInfo.hpp"

#include <stdio.h>

#define RAND11 (gaiSystem.rnd(-1.0f, 1.0f))
#define PI 3.14159265f

//*********************************************************************************************
//
//	Class CActivityBite implementation.
//

	//*********************************************************************************
	CActivityBite::CActivityBite() : CActivity("Bite")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0f;
		feelRatingFeeling[eptLOVE]		= -4.0f;
		feelRatingFeeling[eptANGER]		= 5.0f;
		feelRatingFeeling[eptCURIOSITY] = 0.0f;
		feelRatingFeeling[eptHUNGER]	= 5.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= -3.0f;
		feelRatingFeeling[eptPAIN]		= 0.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;

		rLockDistance = 1.0f;
		rLeadFactor = 1.0f;
	};

	//*********************************************************************************
	void CActivityBite::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		Assert(pinf);

		// Make sure we node our target if possible.
		pinf->setNodeFlags[ensfNODE_ME] = true;

		// Start making the mouth cause damage.
		DoSetHeadDamage(rt_importance, 1);

		// Don't get startled while attacking.
		pbrBRAIN->UnStartle(1.0f);		

		// Were we chosen last time?
		if (!bWasChosen() || sGiveUp == -100.0f)
		{
			// No.  Clear target lock.
			sFlags[eafPHASEONE] =	false;
			sGiveUp = gaiSystem.sNow + 5.0f;
			rHeadCock = gaiSystem.rnd(-PI * 0.25f, PI * 0.25f);

			bDoVocal(evtATTACK);
		}

		// Check timeout.
		if (gaiSystem.sNow > sGiveUp)
		{
			// Don't keep up this bite, but feel free to start a new one.
			// Replaces DontContinue()
			sGiveUp = -100.0f;

		}
		else
		{
			TryToContinue(1.3);
		}

		// Have we acquired target lock?
		if (sFlags[eafPHASEONE])
		{
			// Yes!  
			CBrain* pbr = gaiSystem.pbrGetCurrentBrain();

			// Cock the head.
			DoCockHead(rt_importance, rHeadCock, 1.0f);

			// Are we close enough to chomp down?
			if (rDISTANCE_SQR(v3Target, gaiSystem.v3GetHeadLocation()) < 
					0.3f * 0.3f)
			{
				//  Yes!	Chomp down.

				// Tell physics to close the mouth.
				DoOpenMouth(rt_importance, 0.0f, 1.0f);

#ifdef VER_TEST
				char buffer[256];
				sprintf(buffer, "Biting %s    Chomp!\n", pinf->pinsTarget->strGetInstanceName());
				DebugSay(buffer);
#endif

				// Stop biting now!
				TSec s_chomp_time = 0.5f;
				sGiveUp = Min(sGiveUp, gaiSystem.sNow + s_chomp_time);
			}
			else
			{
				// Make sure we don't keep going for a futile lock.
				sGiveUp = Min(sGiveUp, gaiSystem.sNow + 1.0f);

				//  No!		Move to target.
				// Tell physics to open the mouth wide.
				DoOpenMouth(rt_importance, 1.0f, 1.0f);

				// Tell physics to move head to target.
				DoMoveHeadTo(rt_importance, v3Target, 1.0f);

#if VER_TEST
			CVector3<> v3_head = gaiSystem.v3GetHeadLocation();
			char buffer[256];
			sprintf(buffer, "Locked on %f %f %f\n", 
				v3Target.tX, v3Target.tY, v3Target.tZ);
			DebugSay(buffer);
#endif
			}
		}
		else
		{
			// Prepare a temp target.
			v3Target = pinf->v3Location;

			// Leading code.
			if (rLeadFactor != 0.0f)
			{
				// How fast can I move my head?
				TReal r_head_speed = 3.0f;

				// WHere is my head?
				CVector3<> v3_head = gaiSystem.v3GetHeadLocation();
				
				// Where is my target?
				//v3Target;

				// Where is my target going?
				CVector3<> v3_target_velocity = pinf->pinsTarget->pphiGetPhysicsInfo()->p3GetVelocity(pinf->pinsTarget).v3Pos;

				// If my target is moving toward me, stop now....
				//////////

				// How long will it take to get there?
				TReal r_arrival_time = rDISTANCE(v3_head, v3Target) / r_head_speed;

				// What is the offset then as opposed to now?
				CVector3<> v3_lead = v3_target_velocity * (r_arrival_time * rLeadFactor);

#if VER_TEST
				char buffer[256];
				sprintf(buffer, "Leading %f %f %f\n", v3_lead.tX, v3_lead.tY, v3_lead.tZ);
				DebugSay(buffer);
#endif

				// Where will it be when I get there?
				v3Target += v3_lead;
			}
	

			
			// Prepare the lock, if appropriate.
			if (!pinf->bFacingAway())
				MaybeSetTargetLock(pinf);
			else
			{
				DebugSay("BITE: Prey is Facing Away.\n");
			}

			// Tell physics to open the mouth wide.
			DoOpenMouth(rt_importance, 1.0f, 1.0f);

			// Tell physics to move head to target.
			DoMoveHeadTo(rt_importance, v3Target, 1.0f);

#if VER_TEST
			CVector3<> v3_head = gaiSystem.v3GetHeadLocation();
			char buffer[256];
			sprintf(buffer, "Trying to acquire lock \n\tTargeting %f %f %f\n\t Head At: %f %f %f\n", 
				pinf->v3Location.tX, pinf->v3Location.tY, pinf->v3Location.tZ,
				v3_head.tX, v3_head.tY, v3_head.tZ);
			DebugSay(buffer);
#endif
		}
	}

	//*************************************************************************************
	CRating CActivityBite::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	//******************************
	{
		if (!pinf->bLiving())
			return 0;
		else if (pinf->rDistanceTo > 2.0f * pbrBRAIN->rHeadReach + rLockDistance)
			return 0;
//		else if (!bWasChosen() && pinf->rDistanceTo < 0.5f * rATTACK_RANGE)
//			return 0;
		else
			return CActivity::rtRate(feel, pinf);
	}

	//*************************************************************************************
	void CActivityBite::MaybeSetTargetLock(CInfluence* pinf)
	{
		// Already locked?
		Assert(!sFlags[eafPHASEONE]);

		CVector3<> v3_target = pinf->v3ClosestPointTo(gaiSystem.v3GetBodyLocation());

		// Should we acquire target lock?
		if (rDISTANCE_SQR(gaiSystem.v3GetHeadLocation(), v3_target) < 
				rLockDistance * rLockDistance)
		{
			// Yes!  Acquire target lock and move to target.
			v3Target = v3_target;

			sFlags[eafPHASEONE] = true;

			DebugSay("Locking target.\n");

			bDoVocal(evtBITE);
		}
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityBite::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);

		char local_buffer[256];
		sprintf(local_buffer, "Target Lock: %f\n", rLockDistance);
		strcat(buffer, local_buffer);
		
		strcat(buffer, "\n\nAttempts to bite target.  Moves head to target.  Opens mouth when close, closes mouth when on target.\n");

		strcat(buffer, "\n\nREADY.\nSome tweaks remain, but mostly done.\n");
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityClaw implementation.
//

	//*********************************************************************************
	CActivityClaw::CActivityClaw()	: CActivity("Claw")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -.5f;
		feelRatingFeeling[eptLOVE]		= -1.5f;
		feelRatingFeeling[eptANGER]		= 2.5f;
		feelRatingFeeling[eptCURIOSITY] = 0.0f;
		feelRatingFeeling[eptHUNGER]	= 1.5f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= 0.0f;
		feelRatingFeeling[eptPAIN]		= 0.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;
	};
	
	//*********************************************************************************
	void CActivityClaw::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		Assert(pinf);

		// Don't get startled while attacking.
		pbrBRAIN->UnStartle(1.0f);		


		CVector3<>	v3_target_location = pinf->v3Location;

		{
			// Tell physics to move head to target.
			DoMoveHeadTo(rt_importance, v3_target_location, 1);

			DebugSay("Claw of Doom\n");

			//  We should try to keep clawing next cycle.
			SetMultiplier(1.3);
		}
	}


	//*************************************************************************************
	CRating CActivityClaw::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	//******************************
	{
		if (pinf->rDistanceTo > pbrBRAIN->rClawReach)
			return 0;
		else
			return CActivity::rtRate(feel, pinf);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityClaw::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nAttempts to claw target.\n");

		strcat(buffer, "\n\nCUT.\n");
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//	Class CActivityFeint implementation.
//

	//*********************************************************************************
	CActivityFeint::CActivityFeint() : CActivityBite()
	{
		// Set the name.
		sprintf(acName, "Feint");

		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 2.0f;
		feelRatingFeeling[eptCURIOSITY] = 0.0f;
		feelRatingFeeling[eptHUNGER]	= 2.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= -4.0f;
		feelRatingFeeling[eptPAIN]		= 0.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;

		rLockDistance = 3.0f;	// The distance at which we acquire target lock.
								// (Head to target distance)
	};

	//*************************************************************************************
	CRating CActivityFeint::rtRate(const CFeeling&	feel,	CInfluence*		pinf)
	{
		if (pinf->bFacingAway())
			return 0;
		else 
			return CActivityBite::rtRate(feel, pinf);
	}

	//*************************************************************************************
	void CActivityFeint::MaybeSetTargetLock(CInfluence* pinf)
	{
		// Already locked?
		Assert(!sFlags[eafPHASEONE]);

		// Should we acquire target lock?
		if (rDISTANCE_SQR(gaiSystem.v3GetHeadLocation(), pinf->v3Location) < 
				rLockDistance * rLockDistance)
		{
			// Yes!  Acquire target lock and move to target.
			v3Target = pinf->v3Location + gaiSystem.v3GetBodyLocation();
			v3Target *= 0.5;
			sFlags[eafPHASEONE] = true;

			DebugSay("Locking target.\n");
		}
	}


#if VER_TEST
	//*************************************************************************************
	int CActivityFeint::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nA fake bite.  Stops just short of the target.\n");

		strcat(buffer, "\n\nREADY.\nRequires Jump Back to be cool.  Some tweaks remain.\n");
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityRam implementation.
//

	//*********************************************************************************
	CActivityRam::CActivityRam()
		: CActivity("Ram")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -2.0f;
		feelRatingFeeling[eptLOVE]		= -1.0f;
		feelRatingFeeling[eptANGER]		= 3.0f;
		feelRatingFeeling[eptCURIOSITY] = 0.0f;
		feelRatingFeeling[eptHUNGER]	= 0.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= 0.0f;
		feelRatingFeeling[eptPAIN]		= -.5f;
		feelRatingFeeling[eptSOLIDITY]  = -.2f;
	};

	//*********************************************************************************
	void CActivityRam::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		Assert(pinf);

		// Start making the mouth cause damage.
		DoSetHeadDamage(rt_importance, 1);
		DoSetBodyDamage(rt_importance, 1);

		// Make sure we node our target if possible.
		pinf->setNodeFlags[ensfNODE_ME] = true;

		// Don't get startled while attacking.
		pbrBRAIN->UnStartle(1.0f);		

		// Are we continuing a ram command?
		if (!bWasChosen())
		{
			// No!  We are just starting.

			// Pick a point on the far side of the target.
			v3Target = pinf->v3ToTarget;
			v3Target.Normalise(pbrBRAIN->rWidth * 5.0f);
			v3Target = pinf->v3Location + v3Target;

			// Keep head level with center of target as best you can.
			v3Target.tZ = pinf->v3Location.tZ;

			// Point the head in the right direction.
			SetHeadPoint(pinf);
			sGiveUp  = gaiSystem.sNow + 10.0f;

			rHeadCock = gaiSystem.rnd(-PI * 0.125f, PI * 0.125f);

			bDoVocal(evtATTACK);
		}

		// Charge at full speed to target location.
		DoMoveToLocation(rt_importance, v3Target, 1.0f);

		// Cock the head.
		DoCockHead(rt_importance, rHeadCock, 1.0f);

		// If we are near our target, look aside.
		if (rDISTANCE_SQR(gaiSystem.v3GetBodyLocation(), pinf->pinsTarget->v3Pos()) < Sqr(pbrBRAIN->rHeadReach * 3.0f))
		{ 
			DoPointHead(rt_importance, v3HeadPoint, 1.0f);
		}
		else
		{
			DoPointHead(rt_importance, v3Target - gaiSystem.v3GetBodyLocation(), 1.0f);
		}



#if VER_TEST
		char buffer[256];
		sprintf(buffer, "Ramming speed!\n  Distance to follow-through point: %f\n", rDISTANCE(v3Target, gaiSystem.v3GetBodyLocation()));
		DebugSay(buffer);
#endif

		// Are we near our target location or too late?
		if (gaiSystem.sNow > sGiveUp ||
			(gaiSystem.v2GetBodyLocation() - CVector2<>(v3Target)).tLenSqr() < 9.0f)
		{
			// Yes.  Stop ramming.
			DontContinue();
		}
		else
		{
			// No.
			//  We should try to keep ramming next cycle.
			TryToContinue();
		}
	}


	//*************************************************************************************
	CRating CActivityRam::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	//******************************
	{
		// Don't start charging unless we are far enough away to really start something.
		if (!bWasChosen() && pinf->rDistanceTo < 0.5f * rATTACK_RANGE)
			return 0;
		else
			return CActivity::rtRate(feel,pinf);
	}

	//*************************************************************************************
	void CActivityRam::SetHeadPoint
	(
		CInfluence*		pinf		// The direct object of the action.
	)
	{
		// Point at 30 degrees down.
		v3HeadPoint = pinf->v3ToTarget;
		v3HeadPoint.tZ = - 0.5f * sqrt(Sqr(v3HeadPoint.tX) + Sqr(v3HeadPoint.tY));
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityRam::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nPut head down and run through target.  Should try to keep running past a bit.\n");

		strcat(buffer, "\n\nREADY.\n");
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityEat implementation.
//

	//*********************************************************************************
	CActivityEat::CActivityEat() 
		: CActivityPhased("Eat")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -10.0f;
		feelRatingFeeling[eptLOVE]		= -10.0f;
		feelRatingFeeling[eptANGER]		= -10.0f;
		feelRatingFeeling[eptCURIOSITY] = 0.0f;
		feelRatingFeeling[eptHUNGER]	= 3.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= -10.0f;
		feelRatingFeeling[eptPAIN]		= -10.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;
	};


void CActivityEat::Start(CInfluence* pinf)
{
	// Set phase one
	GotoPhaseOne();

	sGiveUp = gaiSystem.sNow + 10.0f;

	v3Target = pinf->v3ClosestPointTo(pinf->v3Location);

	rHeadCock = gaiSystem.rnd(-PI * 0.25f, PI * 0.25f);
}

void CActivityEat::DoPhaseOne(CRating rt_importance, CInfluence* pinf)
{
	//
	//  Move head to food.
	//

	// In this phase, we head moves to the target location, opening the mouth slightly.
	DoSetHeadDamage(rt_importance, 1);
	
	// Tell physics to open the mouth wide.
	DoOpenMouth(rt_importance, 0.60f, 0.5f);

	CVector3<> v3_head = gaiSystem.v3GetHeadLocation();

	// Tell physics to move head to target.
	v3Target = pinf->v3ClosestPointTo(v3_head);
	DoMoveHeadTo(rt_importance, v3Target, 0.5f);

	// Cock the head.
	DoCockHead(rt_importance, rHeadCock, 1.0f);

	DebugSay("Move Mouth to Food\n");

	// Are we close?
	if (rDISTANCE_SQR(v3Target, v3_head) < pbrBRAIN->rHeadReach * pbrBRAIN->rHeadReach * 0.5f * 0.5f)
	{
		// Yes!  Start phase two.
		GotoPhaseTwo();

		// Yes!  Set target and give up time.
		v3Target = pinf->v3Location;
		v3TearTarget = pinf->v3Location;

		// Reset give up time.
		sChewTime = gaiSystem.sNow + gaiSystem.rnd(2.0f, 8.0f);
		sGiveUp = gaiSystem.sNow + 10.0f;
		
		rHeadCock = gaiSystem.rnd(-PI * 0.25f, PI * 0.25f);

		// Play tear vocal.
		bDoVocal(evtTEAR);
	}
}

void CActivityEat::DoPhaseTwo(CRating rt_importance, CInfluence* pinf)
{
	// Tear the meat/grass off.
	DebugSay("Rip meat off\n");
	DoSetHeadDamage(rt_importance, 1);

	// Tell physics to open the mouth.
	DoOpenMouth(rt_importance, 0.7f, 1.0f);

	// Tell physics to move head sideways.
	DoMoveHeadToBlind(rt_importance, v3TearTarget, 1.0f);

	// Cock the head.
	DoCockHead(rt_importance, rHeadCock, 1.0f);


	// Are we done tearing meat?
	CBrain *pbr = pbrBRAIN;
	if (pbr->bIsVocalizing() && gaiSystem.sNow > pbr->sSampleLength + pbr->sSampleStart)
	{
		// Yes!  End phase two.  
		GotoPhaseThree();
		
		// Play eat vocal.
		bDoVocal(evtEAT);

		// Reset give up time.
		sChewTime = gaiSystem.sNow + gaiSystem.rnd(2.0f, 4.0f);
		sGiveUp = gaiSystem.sNow + 20.0f;

		sMouthTime = -10000;
		v3Target = CVector3<>(RAND11, RAND11, 0);
		if (v3Target.bIsZero())
		{
			v3Target.tX += 1.0f;
		}
	}
}

void CActivityEat::DoPhaseThree(CRating rt_importance, CInfluence* pinf)
{
	//  CHEW!
	// Chew happily.
	DebugSay("Chewing.\n");
	DoSetHeadDamage(rt_importance, 1);

	DoPointHead(rt_importance, v3Target, 0.2);

	// Are we done opening or closing the mouth?
	if (sMouthTime < gaiSystem.sNow)
	{
		bOpening = !bOpening;
		sMouthTime  = gaiSystem.sNow + 0.4f;
	}

	if (bOpening)
	{
		// Opening.
		DoOpenMouth(rt_importance, 0.2f, 0.2f);
	}
	else
	{
		// Closing.
		DoOpenMouth(rt_importance, 0.0f, 0.2f);
	}

	if (gaiSystem.sNow > sChewTime)
	{
		GotoPhaseFour();

		// Make swallow sound.
		bDoVocal(evtSWALLOW);

		// Look up to swallow.
		v3Target = gaiSystem.v3GetHeadPoint();
		v3Target.tZ = 1.5f * v3Target.tLen();
	}
	else
	{
		CBrain* pbr = pbrBRAIN;

		// Is the brain still playing a sample?
		if (pbr->bIsVocalizing())
		{
			// Is it over?
			if (gaiSystem.sNow > pbr->sSampleLength + pbr->sSampleStart)
			{
				// Done chewing.  Start again.
				// Play eat vocal.
				bDoVocal(evtEAT);
			}
		}
	}
}

void CActivityEat::DoPhaseFour(CRating rt_importance, CInfluence* pinf)
{
	DebugSay("Swallowing.\n");


	// Tell physics to close the mouth.
	DoOpenMouth(rt_importance, 0.0f, 1.0f);

	// Look up.
	DoPointHead(rt_importance, v3Target, 0.2);

	// Give up when the sample is done.
	sGiveUp = pbrBRAIN->sSampleLength + pbrBRAIN->sSampleStart;
}

	//*************************************************************************************
	CRating CActivityEat::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	//******************************
	{
		if (pinf->bLiving() || pinf->rDistanceTo > rATTACK_RANGE)
			return 0;
		else
			return CActivity::rtRate(feel,pinf);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityEat::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove head to target, chew noisily.  Maybe lift head while chewing.\n");

		strcat(buffer, "\n\nNOT READY.\nWaiting for precision head placement.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif



//*********************************************************************************************
//
//	Class CActivityDrink implementation.
//

	//*********************************************************************************
	CActivityDrink::CActivityDrink() : CActivityPhased("Drink")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -5.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 0.0f;
		feelRatingFeeling[eptCURIOSITY] = 0.0f;
		feelRatingFeeling[eptHUNGER]	= 0.0f;
		feelRatingFeeling[eptTHIRST]	= 5.0f;
		feelRatingFeeling[eptFATIGUE]	= 0.0f;
		feelRatingFeeling[eptPAIN]		= -5.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;
	};


void CActivityDrink::Start(CInfluence* pinf)
{
	// Assign targets.
	v3TargetOpen	= pinf->v3Location;
	v3TargetOpen.tZ -= 0.25;

	v3TargetClosed	= v3TargetOpen;
	v3TargetClosed.tZ += 0.5f;

	// Only drink for a short time.
	sGiveUp = gaiSystem.sNow + gaiSystem.rnd(3.0f, 7.0f);
}

void CActivityDrink::DoPhaseOne(CRating rt_importance, CInfluence* pinf)
{
	// Are we far from our targets?
	if (rDISTANCE_SQR(v3TargetClosed, gaiSystem.v3GetHeadLocation()) > pbrBRAIN->rHeadReach * pbrBRAIN->rHeadReach * 0.5f * 0.5f)
	{
		// Yes!  Head towards targets.
		// Tell physics to move head to target.
		DoMoveHeadTo(rt_importance, v3TargetClosed, 0.5f);

		DebugSay("Move Mouth to Water\n");
	}
	else
		GotoPhaseTwo();
}

void CActivityDrink::DoPhaseTwo(CRating rt_importance, CInfluence* pinf)
{
	// Have we gotten there?
	bOpening = (rDISTANCE_SQR(v3TargetClosed, gaiSystem.v3GetHeadLocation()) < pbrBRAIN->rHeadReach * pbrBRAIN->rHeadReach * 0.5f * 0.5f);

	if (!bOpening)
	{
		// Closed mouth position.
		DoMoveHeadTo(rt_importance, v3TargetClosed, 0.1f);
		DoOpenMouth(rt_importance, 0.0f, 0.1);
		DebugSay("Drink Close Mouth\n");

		// Have we gotten there?
//		if (rDISTANCE_SQR(v3TargetClosed, gaiSystem.v3GetHeadLocation()) < pbrBRAIN->rHeadReach * pbrBRAIN->rHeadReach * 0.5f * 0.5f)
//		{
//			bOpening = true;
//		}
	}
	else
	{
		// Open mouth position.
		// Closed mouth position.
		DoMoveHeadTo(rt_importance, v3TargetOpen, 0.1f);
		DoOpenMouth(rt_importance, 0.2f, 0.2);
		DebugSay("Drink Open Mouth\n");

		// Have we gotten there?
//		if (rDISTANCE_SQR(v3TargetClosed, gaiSystem.v3GetHeadLocation()) < 0.2f * 0.2f)
//		{
//			bOpening = false;
//		}
	}


	CBrain* pbr = pbrBRAIN;
	// Is the brain still playing a sample?
	// Is it over?
	if (gaiSystem.sNow > pbr->sSampleLength + pbr->sSampleStart)
	{
		// Done with drink sample.  Start again.
		// Play eat vocal.
		bDoVocal(evtDRINK);
	}
}

void CActivityDrink::DoPhaseThree(CRating rt_importance, CInfluence* pinf)
{
}

void CActivityDrink::DoPhaseFour(CRating rt_importance, CInfluence* pinf)
{
}

void CActivityDrink::Stop(CInfluence* pinf)
{
	sCanAct = gaiSystem.sNow + 10.0f;
}


	//*************************************************************************************
	CRating CActivityDrink::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	//******************************
	{
		if (pinf->bLiving() || pinf->rDistanceTo > rATTACK_RANGE)
			return 0;
		else
			return CActivity::rtRate(feel,pinf);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityDrink::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove head to target, slurp noisily.  Jiggle the head up and down a tad.\n");

		strcat(buffer, "\n\nNOT READY.\nWaiting for precision head placement.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityGrab implementation.
//

	CActivityGrab::CActivityGrab() : CActivity("Grab")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -5.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 0.0f;
		feelRatingFeeling[eptCURIOSITY] = 2.0f;
		feelRatingFeeling[eptHUNGER]	= 2.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= 0.0f;
		feelRatingFeeling[eptPAIN]		= -5.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;
	};
	


	//*********************************************************************************
	void CActivityGrab::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		CVector3<>	v3_head_location	= gaiSystem.v3GetHeadLocation();
		CVector3<>	v3_target_location  = pinf->v3Location;
		CBrain*		pbr		= gaiSystem.pbrGetCurrentBrain();

		// We're close!
		if ((v3_head_location - v3_target_location).tLen() < pbr->rHeadReach * 0.1f)
		{
			// Tell physics to close the mouth.
			DoOpenMouth(rt_importance, 0, 1);

			DebugSay("Grabbing!\n");
		}
		else
		{
			// Tell physics to open the mouth wide.
			DoOpenMouth(rt_importance, 1, 1);

			// Tell physics to move head to target.
			DoMoveHeadTo(rt_importance, v3_target_location, 1);

			DebugSay("Mv Mouth to Grab Target\n");

			//  We should try to keep biting next cycle.
			SetMultiplier(1.3);
		}
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityGrab::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove head to target, clamp down with mouth and don't let go.\n");

		strcat(buffer, "\n\nCUT.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityThrow implementation.
//


	//*********************************************************************************
	CActivityThrow::CActivityThrow() : CActivity("Throw")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -5.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 0.0f;
		feelRatingFeeling[eptCURIOSITY] = 1.0f;
		feelRatingFeeling[eptHUNGER]	= 0.0f;
		feelRatingFeeling[eptTHIRST]	= 5.0f;
		feelRatingFeeling[eptFATIGUE]	= 0.0f;
		feelRatingFeeling[eptPAIN]		= -5.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;
	};
	
	//*********************************************************************************
	void CActivityThrow::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		// When we get here, we should already have the target in our mouth.
		DebugSay("Throwing!");

		/*   PSEUDOCODE!
		if (we no longer have a hold on the target)
		{
			mark this activity as a bad choice for next time;
		} 
		else if (we have given our target enough acceleration)
		{
			let go of target
		}
		else
		{
			accelerate the target;
		}
*/
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityThrow::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nAfter a grab, swing your head wildly and let go of target.  Should fling the target.\n");

		strcat(buffer, "\n\nCUT.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif



//*********************************************************************************************
//
//	Class CActivityShake implementation.
//

	CActivityShake::CActivityShake() : CActivity("Shake")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -5.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 0.0f;
		feelRatingFeeling[eptCURIOSITY] = 1.0f;
		feelRatingFeeling[eptHUNGER]	= 0.0f;
		feelRatingFeeling[eptTHIRST]	= 5.0f;
		feelRatingFeeling[eptFATIGUE]	= 0.0f;
		feelRatingFeeling[eptPAIN]		= -5.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;
	};


	//*********************************************************************************
	void CActivityShake::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		// When we get here, we should already have the target in our mouth.
		DebugSay("Shake!");

		/*   PSEUDOCODE!
		if (we no longer have a hold on the target)
		{
			mark this activity as a bad choice for next time;
		} 
		else if (we are not yet at our target)
		{
			keep going for the head target;
		}
		else
		{
			if (we have shaken enough)
			{
				drop target;
			}
			else
			{
				change shake direction;
			}
		}
*/
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityShake::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nAfter grab, move head back and forth violently.  Then drop target.\n");

		strcat(buffer, "\n\nCUT.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityTailSwipe implementation.
//

	
	CActivityTailSwipe::CActivityTailSwipe() : CActivityPhased("Tail Swipe")
	{
		// Set up the rating feeling.
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -2.0f;
		feelRatingFeeling[eptLOVE]		= -1.0f;
		feelRatingFeeling[eptANGER]		= 3.0f;
		feelRatingFeeling[eptCURIOSITY] = 0.0f;
		feelRatingFeeling[eptHUNGER]	= 0.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= 0.0f;
		feelRatingFeeling[eptPAIN]		= -.5f;
		feelRatingFeeling[eptSOLIDITY]  = -.2f;
	};
	
	//*************************************************************************************
	CRating CActivityTailSwipe::rtRate(const CFeeling&	feel,	CInfluence*		pinf)
	{
		if (pinf->rDistanceTo > pbrBRAIN->rTailReach * 2.0f)
			return 0;
		else 
			return CActivityPhased::rtRate(feel, pinf);
	}


	void CActivityTailSwipe::Start()
	{
		sGiveUp = gaiSystem.sNow + 5.0f;

		bDoVocal(evtATTACK);
	}


	void CActivityTailSwipe::DoPhaseOne(CRating rt_importance, CInfluence* pinf)
	{
		//
		// Move body to stand within tail swipe distance.
		//
		DoSetTailDamage(rt_importance, 1);

		DebugSay("Closing for Tail\n");

		CVector3<> v3_target = pinf->v3ClosestPointTo(gaiSystem.v3GetBodyLocation());

		// If the body is within tail reach of the target, commence swing.
		if (rDISTANCE_SQR(v3_target, gaiSystem.v3GetBodyLocation()) < pbrBRAIN->rTailReach * pbrBRAIN->rTailReach)
		{
			// Close enough!
			GotoPhaseTwo();
		}
		else
		{
			// Not close enough!
			DoMoveTo(rt_importance, pinf, 1.0);
		}

	}

	void CActivityTailSwipe::DoPhaseTwo(CRating rt_importance, CInfluence* pinf)
	{
		//
		// Pick a target tail orientation.
		//

		DebugSay("Picking Tail Target\n");
		DoSetTailDamage(rt_importance, 1);

		// Where is my tail?
		CVector2<> v2_tail_position = gaiSystem.v3GetTailPoint();

		// Where is my target?
		CVector2<> v2_foe_position = pinf->v3ToTarget;

		CVector2<> v2_head = gaiSystem.v3GetHeadPoint();
		v2_head.Normalise(2);
		v2_foe_position.Normalise(1);
		v2TailTargetIntermediate = v2_head + v2_foe_position;
		v2TailTargetIntermediate.Normalise();

		v2TailTarget = - v2_foe_position;
		v2TailTarget.Normalise();

		// Swing tail.
		DoPointHead(rt_importance, - v2TailTargetIntermediate, 1.0);

		// And then goto the next phase.
		GotoPhaseThree();
	}

	void CActivityTailSwipe::DoPhaseThree(CRating rt_importance, CInfluence* pinf)
	{
		DoSetTailDamage(rt_importance, 1);

		CVector2<> v2_target = v2TailTargetIntermediate;

	#if VER_TEST
		char buffer[256];
		sprintf(buffer, "Swiping tail to %f %f\n", - v2_target.tX, - v2_target.tY);
		DebugSay(buffer);
	#endif

		// Swing body until tail is in target orientation.
		DoPointHead(rt_importance, - v2_target, 1.0);

		// Have we finished the swing?
		CVector2<> v2_head = gaiSystem.v3GetHeadPoint();
		if (!v2_head.bIsZero())
		{
			v2_head.Normalise();
		}

		if (rDISTANCE_SQR(v2_head, (- v2_target)) < .1)
		{
			// Yes!
			GotoPhaseFour();
		}
	}

	void CActivityTailSwipe::DoPhaseFour(CRating rt_importance, CInfluence* pinf)
	{
		DoSetTailDamage(rt_importance, 1);
		CVector2<> v2_target = v2TailTarget;

	#if VER_TEST
		char buffer[256];
		sprintf(buffer, "Swiping tail to %f %f\n", - v2_target.tX, - v2_target.tY);
		DebugSay(buffer);
	#endif

		// Swing body until tail is in target orientation.
		DoPointHead(rt_importance, - v2_target, 1.0);

		// Have we finished the swing?
		CVector2<> v2_head = gaiSystem.v3GetHeadPoint();
		if (!v2_head.bIsZero())
		{
			v2_head.Normalise();
		}

		if (rDISTANCE_SQR(v2_head, (- v2_target)) < .1)
		{
			// Yes!
			DontContinue();
		}
	}





#if VER_TEST
	//*************************************************************************************
	int CActivityTailSwipe::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLash tail at target.\n");

		strcat(buffer, "\n\nNot done.\nWaiting for Andrew.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityJumpAndClaw implementation.
//

	CActivityJumpAndClaw::CActivityJumpAndClaw() : CActivity("Jump And Claw")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -5.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 0.0f;
		feelRatingFeeling[eptCURIOSITY] = 1.0f;
		feelRatingFeeling[eptHUNGER]	= 0.0f;
		feelRatingFeeling[eptTHIRST]	= 5.0f;
		feelRatingFeeling[eptFATIGUE]	= 0.0f;
		feelRatingFeeling[eptPAIN]		= -5.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;
	};
	
	//*********************************************************************************
	void CActivityJumpAndClaw::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		// When we get here, we should already have the target in our mouth.
		DebugSay("Jump and Claw!");

		/*   PSEUDOCODE!
		if (we are in the air)
		{
			move head to foe
		} 
		else if (we are far from foe)
		{
			jump to foe
		}
*/
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityJumpAndClaw::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLeap into the air and deliver a flying kick, or just plain land on the target.\n");

		strcat(buffer, "\n\nCUT\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif



//*********************************************************************************************
//
//	Class CActivityJumpAndBite implementation.
//


	CActivityJumpAndBite::CActivityJumpAndBite() : CActivity("Jump and Bite")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -5.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 0.0f;
		feelRatingFeeling[eptCURIOSITY] = 1.0f;
		feelRatingFeeling[eptHUNGER]	= 0.0f;
		feelRatingFeeling[eptTHIRST]	= 5.0f;
		feelRatingFeeling[eptFATIGUE]	= 0.0f;
		feelRatingFeeling[eptPAIN]		= -5.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;
	}



	//*********************************************************************************
	void CActivityJumpAndBite::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		DoSetHeadDamage(rt_importance, 1);

		// When we get here, we should already have the target in our mouth.
		DebugSay("Jump And Bite!");

		/*   PSEUDOCODE!
		if (we are in the air)
		{
			Move claws to foe
		} 
		else if (we are far from foe)
		{
			jump to foe
		}
*/
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityJumpAndBite::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLeap into the air and bite the target.  Not useful, but very cinematic.\n");

		strcat(buffer, "\n\nNOT READY.\nWaiting for precision head placement and jump controls.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityShoulderCharge implementation.
//

	CActivityShoulderCharge::CActivityShoulderCharge() : CActivityRam()
	{
		sprintf(acName, "Shoulder Charge");
	};
	
	
	//*************************************************************************************
	void CActivityShoulderCharge::SetHeadPoint
	(
		CInfluence*		pinf		// The direct object of the action.
	)
	{
		// Point 45 degrees off target.
		v3HeadPoint = pinf->v3ToTarget;
		int i_sign = gaiSystem.rnd() & 0x2;
		--i_sign;
		CRotate3<> r3(CDir3<>(0,0,1), i_sign * 3.14159 * .25);
		v3HeadPoint = v3HeadPoint * r3;
	}


#if VER_TEST	
	//*************************************************************************************
	int CActivityShoulderCharge::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLike Ram, but instead of hitting with the head we try to hit with the side of the neck (or shoulder, if we are lucky).\n");

		strcat(buffer, "\n\nREADY.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

