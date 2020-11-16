/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 *	Implementation of EmotionActivities.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/EmotionActivities.cpp                                        $
 * 
 * 17    9/19/98 1:43a Agrant
 * better rating for cower and rear back
 * 
 * 16    9/08/98 7:40p Agrant
 * dont cower when your foe is not looking
 * 
 * 15    9/06/98 7:49p Agrant
 * added some head cock 
 * 
 * 14    9/05/98 4:38p Agrant
 * better cower, nothing
 * 
 * 13    9/04/98 4:34p Agrant
 * fixed nothing name
 * 
 * 12    8/31/98 6:31p Agrant
 * massive dino resources reallocation
 * 
 * 11    8/31/98 5:09p Agrant
 * Make Nothing exclusive
 * 
 * 10    8/26/98 7:14p Agrant
 * Rear back and cower improvements
 * 
 * 9     8/22/98 7:20p Agrant
 * rearback responds to startle flag
 * 
 * 8     7/29/98 3:08p Agrant
 * added nothing activity, cleaned up
 * 
 * 7     7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 6     6/27/98 9:28p Agrant
 * Updated descriptions or activities
 * 
 * 5     5/16/98 11:50p Agrant
 * many tweaks
 * 
 * 4     2/12/98 1:11p Agrant
 * Added descriptions.
 * 
 * 3     1/20/98 3:23p Agrant
 * Removed bogus comments
 * 
 * 2     1/08/98 6:07p Agrant
 * Pseudocode implementations of emotional activities
 * 
 * 1     1/07/98 6:47p Agrant
 * intitial revision
 * 
 **********************************************************************************************/

#include "common.hpp"

#include "EmotionActivities.hpp"
#include "Influence.hpp"
#include "AIMain.hpp"
#include "Lib/EntityDBase/Animal.hpp"
#include "Brain.hpp"

#define rPI 3.14159265f
#define PI rPI

//*********************************************************************************************
//
//	Class CActivityLiftTail implementation.
//

	//*********************************************************************************
	CActivityLiftTail::CActivityLiftTail() : CActivity("LiftTail")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0;
		feelRatingFeeling[eptLOVE]		= 0.0;
		feelRatingFeeling[eptANGER]		= 0.0;
		feelRatingFeeling[eptCURIOSITY] = 2.0;
		feelRatingFeeling[eptHUNGER]	= 0.0;
		feelRatingFeeling[eptTHIRST]	= 0.0;
		feelRatingFeeling[eptFATIGUE]	= 0.0;
		feelRatingFeeling[eptPAIN]		= 0.0;
		feelRatingFeeling[eptSOLIDITY]  = 0.0;
	};

	//*********************************************************************************
	void CActivityLiftTail::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		DebugSay("Lift Tail!");

		//	lift the tail

		//  stop tail wagging
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityLiftTail::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLift the tail into the air, as a cat might.\n");

		strcat(buffer, "\n\nNOT READY.\nWaiting for precision tail controls.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityRearBack implementation.
//

	//*********************************************************************************
	CActivityRearBack::CActivityRearBack()	: CActivity("Rear Back")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0;
		feelRatingFeeling[eptLOVE]		= 0.0;
		feelRatingFeeling[eptANGER]		= -2.0;
		feelRatingFeeling[eptCURIOSITY] = 0.0;
		feelRatingFeeling[eptHUNGER]	= 0.0;
		feelRatingFeeling[eptTHIRST]	= 0.0;
		feelRatingFeeling[eptFATIGUE]	= 0.0;
		feelRatingFeeling[eptPAIN]		= 5.0;
		feelRatingFeeling[eptSOLIDITY]  = 0.0;

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= true;
		sResourcesRequired[edrFORELEGS] = true;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;
	};

	//*************************************************************************************
	CRating CActivityRearBack::rtRate(const CFeeling& feel, CInfluence* pinf)
	{
		CRating rt = CActivity::rtRate(feel, pinf);

		if (pbrBRAIN->bStartled)
			rt *= 2;

		return rt;
	}

	//*********************************************************************************
	void CActivityRearBack::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		if (!bWasChosen())
		{
			sGiveUp = gaiSystem.sNow + 1.0f;

			rHeadCock = gaiSystem.rnd(-PI * 0.25, PI * 0.25);
		}

		if (gaiSystem.sNow > sGiveUp)
		{
			DontContinue();
			sCanAct = gaiSystem.sNow + 3.0f;

			if (pbrBRAIN->bStartled)
			{
				pbrBRAIN->UnStartle(15.0f);
			}
		}
		else
		{
			TryToContinue();
		}

		DebugSay("Rear back!");

		//	lift the head way up (dragging the body with it)
		DoMoveHeadTo(rt_importance, gaiSystem.v3GetBodyLocation() + CVector3<>(0,0,pbrBRAIN->rHeadReach * 0.3f), 1);
		
		//  look at target
		DoPointHead(rt_importance, pinf->v3ToTarget, 1);

		// Cock the head for effect.
		DoCockHead(rt_importance, rHeadCock, 1.0f);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityRearBack::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLift head up high, but keep looking at target.  Indicates fear or surprise.\n");

		strcat(buffer, "\n\nNOT READY.\nWaiting for head orientation and head position working at the same time with conflicting orders.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


	
//*********************************************************************************************
//
//	Class CActivityBackAway implementation.
//

	//*********************************************************************************
	CActivityBackAway::CActivityBackAway() : CActivity("Back Away")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0;
		feelRatingFeeling[eptLOVE]		= 0.0;
		feelRatingFeeling[eptANGER]		= 0.0;
		feelRatingFeeling[eptCURIOSITY] = 2.0;
		feelRatingFeeling[eptHUNGER]	= 0.0;
		feelRatingFeeling[eptTHIRST]	= 0.0;
		feelRatingFeeling[eptFATIGUE]	= 0.0;
		feelRatingFeeling[eptPAIN]		= 0.0;
		feelRatingFeeling[eptSOLIDITY]  = 0.0;
	};

	//*********************************************************************************
	void CActivityBackAway::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		DebugSay("Back Away!");

		//	look at target

		//  move away from target
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityBackAway::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nBack up slowly away from target.\n");

		strcat(buffer, "\n\nNOT READY.\nSame as JumpBack.  Cut.");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


	
//*********************************************************************************************
//
//	Class CActivityCower implementation.
//

	//*********************************************************************************
	CActivityCower::CActivityCower() : CActivity("Cower")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 3.0;
		feelRatingFeeling[eptLOVE]		= 0.0;
		feelRatingFeeling[eptANGER]		= -2.0;
		feelRatingFeeling[eptCURIOSITY] = 0.0;
		feelRatingFeeling[eptHUNGER]	= 0.0;
		feelRatingFeeling[eptTHIRST]	= 0.0;
		feelRatingFeeling[eptFATIGUE]	= 4.0;
		feelRatingFeeling[eptPAIN]		= 0.0;
		feelRatingFeeling[eptSOLIDITY]  = 0.0;
	};
	
	//*************************************************************************************
	CRating CActivityCower::rtRate(const CFeeling& feel, CInfluence* pinf)
	{
		if (pinf->bFacingAway())
			return 0;

		// Don't cower unless the bad guy is close.
		if (pinf->rDistanceTo > 4.0f)
			return 0;

		CRating rt = CActivity::rtRate(feel, pinf);
		return rt;
	}


	//*********************************************************************************
	void CActivityCower::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		if (!bWasChosen())
		{
			bDoVocal(evtWHIMPER);
			rHeadCock = gaiSystem.rnd(-PI * 0.25, PI * 0.25);
		}

		DebugSay("Cower!");

		// No state info to this one....
		// May need a better rating function that really ramps up when near target
		DoCrouch(rt_importance);

		CVector3<> v3_target = pinf->v3ToTarget;
		v3_target.tZ = -.4f * v3_target.tLen();
		CVector2<> v2_target	= v3_target;
		CVector2<> v2_body		= gaiSystem.v2GetBodyFacing();

		TReal r_angle = rPI * gaiSystem.rnd(0.20f, 0.30f);
		if ((v2_target ^ v2_body) < 0)
			r_angle = - r_angle;
		
		// Tilt head to one side.
		v3_target *= CRotate3<>(CDir3<>(0,0,1), r_angle);
		DoPointHead(rt_importance, v3_target, 0.3f);

		// Lower the head in fear.
		CVector3<> v3_head = gaiSystem.v3GetHeadLocation();
		v3_head.tZ = gaiSystem.v3GetBodyLocation().tZ - (pbrBRAIN->rHeadReach * 0.25f);
		DoMoveHeadTo(rt_importance, v3_head, 0.3f);

		DoCockHead(rt_importance, rHeadCock, 1.0f);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityCower::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nCrouch down, lower the head, look at target.  Maybe back away slowly.  Indicates fear, submission.\n");

		strcat(buffer, "\n\nNOT READY.\nWaiting for head orientation, head position, crouch, movement speed controls, and conflict resolution between all of the above.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


	
//*********************************************************************************************
//
//	Class CActivityLashTail implementation.
//

	//*********************************************************************************
	CActivityLashTail::CActivityLashTail() : CActivity("Lash Tail")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0;
		feelRatingFeeling[eptLOVE]		= 0.0;
		feelRatingFeeling[eptANGER]		= 0.0;
		feelRatingFeeling[eptCURIOSITY] = 2.0;
		feelRatingFeeling[eptHUNGER]	= 0.0;
		feelRatingFeeling[eptTHIRST]	= 0.0;
		feelRatingFeeling[eptFATIGUE]	= 0.0;
		feelRatingFeeling[eptPAIN]		= 0.0;
		feelRatingFeeling[eptSOLIDITY]  = 0.0;
	};

	//*********************************************************************************
	void CActivityLashTail::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		DebugSay("lash tail!");

		//	wag tail slowly, big arcs.  Curve it as much as possible

		// If we have better tail controls, it should slow down at the larger displacements and 
		//    speed up when it is close to straight back

	}

#if VER_TEST
	//*************************************************************************************
	int CActivityLashTail::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLash tail impatiently.\n");

		strcat(buffer, "\n\nNOT READY.\nWaiting for tail controls.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif




	
//*********************************************************************************************
//
//	Class CActivitySleep implementation.
//

	//*********************************************************************************
	CActivitySleep::CActivitySleep() : CActivity("Sleep")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0;
		feelRatingFeeling[eptLOVE]		= 0.0;
		feelRatingFeeling[eptANGER]		= 0.0;
		feelRatingFeeling[eptCURIOSITY] = 2.0;
		feelRatingFeeling[eptHUNGER]	= 0.0;
		feelRatingFeeling[eptTHIRST]	= 0.0;
		feelRatingFeeling[eptFATIGUE]	= 0.0;
		feelRatingFeeling[eptPAIN]		= 0.0;
		feelRatingFeeling[eptSOLIDITY]  = 0.0;
	};

	//*********************************************************************************
	void CActivitySleep::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		DebugSay("ZZZZZZZZ!");

		// Lie down

		// Increase the liklihood on continuing to sleep by a lot.
	}

#if VER_TEST
	//*************************************************************************************
	int CActivitySleep::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLie down and snore.\n");

		strcat(buffer, "\n\nNOT READY.\nWaiting for lie down, stand up, sound.  This one likely gets cut.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif




	
//*********************************************************************************************
//
//	Class CActivityLiftTail implementation.
//

	//*********************************************************************************
	CActivityLieDown::CActivityLieDown() : CActivity("Lie Down")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0;
		feelRatingFeeling[eptLOVE]		= 0.0;
		feelRatingFeeling[eptANGER]		= 0.0;
		feelRatingFeeling[eptCURIOSITY] = 2.0;
		feelRatingFeeling[eptHUNGER]	= 0.0;
		feelRatingFeeling[eptTHIRST]	= 0.0;
		feelRatingFeeling[eptFATIGUE]	= 0.0;
		feelRatingFeeling[eptPAIN]		= 0.0;
		feelRatingFeeling[eptSOLIDITY]  = 0.0;
	};

	//*********************************************************************************
	void CActivityLieDown::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		DebugSay("Lie Down!");

		// Lie DOwn.
	
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityLieDown::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLie down.\n");

		strcat(buffer, "\n\nNOT READY.\nWaiting for lie down, stand up.  This one likely gets cut.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif



//*********************************************************************************************
//
//	Class CActivityScratch implementation.
//

	//*********************************************************************************
	CActivityScratch::CActivityScratch() : CActivity("Scratch")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0;
		feelRatingFeeling[eptLOVE]		= 0.0;
		feelRatingFeeling[eptANGER]		= 0.0;
		feelRatingFeeling[eptCURIOSITY] = 2.0;
		feelRatingFeeling[eptHUNGER]	= 0.0;
		feelRatingFeeling[eptTHIRST]	= 0.0;
		feelRatingFeeling[eptFATIGUE]	= 0.0;
		feelRatingFeeling[eptPAIN]		= 0.0;
		feelRatingFeeling[eptSOLIDITY]  = 0.0;
	};

	//*********************************************************************************
	void CActivityScratch::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		DebugSay("Scratch!");

		// We don't have control inputs for this one yet.
	
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityScratch::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nScratch like a dog might.\n");

		strcat(buffer, "\n\nNOT READY (CUT)\nWaiting for advanced foot controls.  Most likely cut this one.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//	Class CActivityNothing implementation.
//

	//*********************************************************************************
	CActivityNothing::CActivityNothing() : CActivity("Nothing")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 1.0;
		feelRatingFeeling[eptLOVE]		= 1.0;
		feelRatingFeeling[eptANGER]		= 1.0;
		feelRatingFeeling[eptCURIOSITY] = 1.0;
		feelRatingFeeling[eptHUNGER]	= 1.0;
		feelRatingFeeling[eptTHIRST]	= 1.0;
		feelRatingFeeling[eptFATIGUE]	= 1.0;
		feelRatingFeeling[eptPAIN]		= 1.0;
		feelRatingFeeling[eptSOLIDITY]  = 1.0;

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= true;
		sResourcesRequired[edrFORELEGS] = true;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= true;

		sResourcesUsed = sResourcesRequired;
		
		sFlags[eafEXCLUSIVE] = true;
	};

	//*********************************************************************************
	void CActivityNothing::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		DebugSay("Nothing!\n");
		pbrBRAIN->msgprPhysicsControl.subGoLimp.Set(true, rt_importance, 1);
	}

	//*********************************************************************************
	CRating CActivityNothing::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	{
		// If we are tranq'ed, rate 1.
		CAnimal* pani = gaiSystem.paniGetCurrentAnimal();

		if (pani->fTranqPoints < 0)
			return 1;
		else
			return 0;
	}


#if VER_TEST
	//*************************************************************************************
	int CActivityNothing::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nDo absolutely nothing.\n");

		strcat(buffer, "\n\nREADY\nUsed for tranquilized dinos..\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

