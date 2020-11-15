/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 *	Implementation of MoveActivities.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/MoveActivities.cpp                                           $
 * 
 * 70    10/08/98 2:45p Agrant
 * better GetOut
 * 
 * 69    10/07/98 7:00a Agrant
 * better downhill behavior in most cases
 * 
 * 68    10/02/98 10:06p Agrant
 * don't look up or down while getting out
 * 
 * 67    10/01/98 2:53p Agrant
 * fixed GetOut for large dinos
 * 
 * 66    9/27/98 11:40p Agrant
 * numerous pathfinding assistance tweaks
 * 
 * 65    9/23/98 12:49a Agrant
 * Wander within the stay near distance, please
 * 
 * 64    9/21/98 11:40p Agrant
 * Get Out of terrain even when you think you are pathfinding okay
 * 
 * 63    9/20/98 7:26p Agrant
 * use the node me flag
 * 
 * 62    9/19/98 9:00p Agrant
 * jump back better
 * 
 * 61    9/19/98 1:42a Agrant
 * better jump back, maybe
 * 
 * 60    9/14/98 8:20p Agrant
 * Do damage while charging aggressively
 * 
 * 59    9/13/98 8:04p Agrant
 * Save and load stay near and away targets
 * 
 * 58    8/29/98 2:21p Jpaque
 * Fixed drew's get out when dead bug because he was too lame to do it
 * 
 * 57    9/12/98 1:08a Agrant
 * The new living flag for influences
 * 
 * 56    9/11/98 3:18p Agrant
 * Dont stay near or away when dead
 * 
 * 55    9/11/98 12:41p Agrant
 * get out of steep slopes
 * 
 * 54    9/09/98 10:01p Agrant
 * don't get out if you are over or under
 * 
 * 53    9/08/98 7:41p Agrant
 * if hungry, stalk should beat pursue if the foe is facing you
 * 
 * 52    9/08/98 4:44p Agrant
 * much parameter tweaking
 * approach just sprints when target faces away
 * 
 * 51    9/05/98 10:33p Agrant
 * stay near and stay away now think only in 2d
 * 
 * 50    9/05/98 4:39p Agrant
 * better stalk
 * 
 * 49    9/05/98 11:16a Agrant
 * flee does not cause ignore, but can cancel flee
 * 
 * 48    8/31/98 6:31p Agrant
 * massive dino resources reallocation
 * 
 * 47    8/31/98 5:09p Agrant
 * tweak circle, moveby
 * 
 * 46    8/26/98 7:15p Agrant
 * removed crouch from the jump command
 * 
 * 45    8/26/98 11:16a Agrant
 * jump activity first pass
 * 
 * 44    8/25/98 2:12p Agrant
 * wander fix and other tweaks
 * 
 * 43    8/23/98 2:21p Agrant
 * Wander based off head point
 * Circle distance based
 * Balancing moveby vs pursue
 * 
 * 42    8/20/98 11:07p Agrant
 * jump back times out
 * 
 * 41    8/16/98 3:21p Agrant
 * Much tweaking to simplify activity balancing
 * 
 * 40    8/13/98 6:56p Agrant
 * code to allow dinos to ignore influences that are unattainable
 * 
 * 39    8/11/98 2:20p Agrant
 * minor improvements
 * 
 * 38    7/31/98 7:50p Agrant
 * complete circle rework
 * 
 * 37    7/31/98 4:46p Agrant
 * better jumpback.  Removed dout
 * 
 * 36    7/31/98 3:17p Agrant
 * better jumpback and moveby
 * 
 * 35    7/29/98 3:07p Agrant
 * added move by, cleaned up
 * 
 * 34    7/25/98 8:03p Agrant
 * better movement defaults
 * 
 * 33    7/24/98 6:30p Agrant
 * stay near and stay away now handle based to avoid load order problems
 * 
 * 32    7/23/98 9:36p Agrant
 * StayNear and StayAway now exclusive 
 * 
 * 31    7/22/98 10:06p Agrant
 * don't circle when you are far away
 * 
 * 30    7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 29    7/20/98 12:27p Agrant
 * Better GetOut implementation
 *
 **********************************************************************************************/

#include "common.hpp"

#include "MoveActivities.hpp"
#include "Lib/Std/Random.hpp"

#include "Activity.hpp"
#include "Synthesizer.hpp"
#include "WorldView.hpp"

#include "AIMain.hpp"
#include "AIInfo.hpp"
#include "Brain.hpp"

#include "Lib/EntityDBase/Animal.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"

#include "Lib/Std/Hash.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include "Lib/Renderer/GeomTypes.hpp"


#include "Lib/EntityDBase/Query/QTerrain.hpp"

#include <stdio.h>


// The amount by which you can multiply an importance rating to get a decent speed.
#define fSPEED_SCALE_FACTOR (1.0f / 0.02f)
#define PI (3.14159265f)


//*********************************************************************************************
//
//	Class CActivityMoveCommandToward implementation
//

	CActivityMoveCommandToward::CActivityMoveCommandToward()
		: CActivityDistance("MoveToward")
	{

		 // Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -3.0;
		feelRatingFeeling[eptLOVE]		= 0.0;
		feelRatingFeeling[eptANGER]		= 0.0;
		feelRatingFeeling[eptCURIOSITY] = 4.0;
		feelRatingFeeling[eptHUNGER]	= 2.0;
		feelRatingFeeling[eptTHIRST]	= 2.0;
		feelRatingFeeling[eptFATIGUE]	= -3.0;
		feelRatingFeeling[eptPAIN]		= -4.0;
		feelRatingFeeling[eptSOLIDITY]  = 0;

		sResourcesRequired[edrHEAD]		= false;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;

	};
	
	//*********************************************************************************
	CRating CActivityMoveCommandToward::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	{
		// Are we already really close?
		if (pinf->rDistanceTo < pbrBRAIN->rHeadReach)
			return 0;
		
		CRating rt_ret = CActivityDistance::rtRate(feel, pinf);

		if (pinf->rDistanceTo < 2.0f * pbrBRAIN->rHeadReach)
		{
			return rt_ret.fVal * pinf->rDistanceTo / (2.0f * pbrBRAIN->rHeadReach);
		}
		else
			return rt_ret;
	}

	//*********************************************************************************
	void CActivityMoveCommandToward::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		Assert(pinf);

		// Handle the case where it's time to give up.  We've been chosen, so if it's time, it's prolly our fault.
		if (pbrBRAIN->bFrustrated())
		{
			// Ignore the culprit!
			pinf->Ignore(true);
			
			pbrBRAIN->SetWayPoint();
		}

		// Move toward the influence with importance based on rating,
		// speed relative to importance.

		/*
		rtSpeed.Clamp(gaiSystem.rnd(rtSpeed - 0.1, rtSpeed + 0.1));
		if (rtSpeed < 0.3)
			rtSpeed = 0.3;
		else if (rtSpeed > 0.7)
			rtSpeed = 0.7;
			*/

		rtSpeed = 0.5;

#if VER_TEST
		char buffer[256];
		sprintf(buffer, "Move Toward, speed %f\n", rtSpeed.fVal);
		DebugSay(buffer);
#endif

		DoMoveTo(rt_importance, pinf, rtSpeed);
		DoPointHead(rt_importance, pinf->v3ToTarget, rtSpeed);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityMoveCommandToward::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove your body closer to the target.\n\nREADY.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//  class CActivityMoveHerd implementation
//

	CActivityMoveHerd::CActivityMoveHerd
	(
	) : CActivity("MoveHerd")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -3;
		feelRatingFeeling[eptLOVE]		= 2;
		feelRatingFeeling[eptANGER]		= 0;
		feelRatingFeeling[eptCURIOSITY] = 0;
		feelRatingFeeling[eptHUNGER]	= 0;
		feelRatingFeeling[eptTHIRST]	= 0;
		feelRatingFeeling[eptFATIGUE]	= 0;
		feelRatingFeeling[eptPAIN]		= 0;
		feelRatingFeeling[eptSOLIDITY]  = 0;
	};
	
	//*********************************************************************************
	void CActivityMoveHerd::Act
	(
		CRating	rt_importance,
		CInfluence*		pinf
	)
	{
		Assert(pinf);
		
		// Move toward the influence with importance based on rating.
		DoMoveTo
		(
			rt_importance, 
			pinf,
			rt_importance
		);
	}

	//*********************************************************************************
	CRating CActivityMoveHerd::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf
	)
	{
		Assert(pinf);

		CRating rt	= CActivity::rtRate(feel, pinf);

		// Move toward the influence with importance based on rating.
		RegisterHerdHint
		(
			rt, 
			pinf->v3ToTarget
		);

		return rt;
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityMoveHerd::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove your body closer to the target.  This may never be implemented, as MoveToward may suffice, and we may never have a herd.\n");
		strcat(buffer, "\n\nNOT READY. (CUT)\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//  class CActivityMoveCommandAway implementation
//

	CActivityMoveCommandAway::CActivityMoveCommandAway 
	(
	) : CActivityDistance("MoveAway")
	{
		// Set up the rating feeling.

		// Only Fear affects fleeing.

		feelRatingFeeling[eptFEAR]		= 4.0;
		feelRatingFeeling[eptLOVE]		= 0;
		feelRatingFeeling[eptANGER]		= -2;
		feelRatingFeeling[eptCURIOSITY] = 0;
		feelRatingFeeling[eptHUNGER]	= -1;
		feelRatingFeeling[eptTHIRST]	= 0;
		feelRatingFeeling[eptFATIGUE]	= 2.0;
		feelRatingFeeling[eptPAIN]		= -1;
		feelRatingFeeling[eptSOLIDITY]  = 0;

		sResourcesRequired[edrHEAD]		= false;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;

	};
	
	
	//*********************************************************************************
	CRating CActivityMoveCommandAway::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	{
		CRating rt = CActivityDistance::rtRate(feel, pinf);

		// Hint away from the threat.
		RegisterFleeHint
		(
			rt, 
			- pinf->v3ToTarget
		);

		return rt;
	}

	//*********************************************************************************
	void CActivityMoveCommandAway::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		Assert(pinf);
		
		// Handle the case where it's time to give up.  We've been chosen, so if it's time, it's prolly our fault.
		if (pbrBRAIN->bFrustrated())
		{
			// Ignore the culprit!
			pinf->Ignore(true);
			
			pbrBRAIN->SetWayPoint();
		}

		DebugSay("Move Away.\n");

		CVector2<> v2_flee = gaiSystem.psynGetCurrentSynthesizer()->v2FleeHint;
		v2_flee.Normalise();

		v2_flee += gaiSystem.v2GetBodyFacing() * 2.0f;

		// Move away from all fearful things!
		DoMoveInDirection
		(
			rt_importance, 
			v2_flee,
			0.5f
		);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityMoveCommandAway::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove your body farther from the target.\n\n\nREADY.");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//  class CActivityMoveCommandDontTouch implementation
//

	//*********************************************************************************
	CActivityMoveCommandDontTouch::CActivityMoveCommandDontTouch
	(
	) : CActivity("DontTouch")
	{
		// Set up the rating feeling.
		//  These initializations will be replaced by data files.

		// This goofy guy has been replaced by pathfinding.
		// Maybe its day will come again.
//			Assert(0);

		// Dont touch solid objects.
		feelRatingFeeling[eptSOLIDITY]	= .75;

		// Really dont touch fearsome objects.
		feelRatingFeeling[eptFEAR]		= .75;

		// Only avoid things if they are nearby or very influential.
		rtThreshold						= .3f;
	};


	//*********************************************************************************
	CRating CActivityMoveCommandDontTouch::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf
	)
	{
		if (!bIsActive())
			return 0;

		Assert(pinf);

		CRating rt			= rtRateWithRatingFeeling(feel);

		TReal rDistanceSQ	= (gaiSystem.v3GetBodyLocation() - 
								pinf->v3Location).tLenSqr();

		// Reduce to zero when the distance is above a 
		// threshold value.
		if (rDistanceSQ > 4.0)
			rt				= 0;
		else if (rDistanceSQ > 1.0)
			rt				*= 1.0 / rDistanceSQ;
		return rt;
	}

	//*********************************************************************************
	void CActivityMoveCommandDontTouch::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		// Move away from the influence with importance based on rating.
		DoMoveInDirection
		(
			rt_importance, 
			- pinf->v3ToTarget,
			1
		);
	}


#if VER_TEST
	//*************************************************************************************
	int CActivityMoveCommandDontTouch::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMake sure that you don't touch target, but you can get close.  From old pathfinding hack days.\n");

		strcat(buffer, "\n\nNOT READY.\nCould implement, but why?\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//  class CActivityMoveHintWander implementation
//

	CActivityMoveHintWander::CActivityMoveHintWander
	(
	) : CActivity("Wander")
	{
		// Set up the rating feeling.
		//  These initializations will be replaced by data files.

		// Wander if hungry, thirsty, or curious.
		feelRatingFeeling[eptCURIOSITY]		= .005;
		feelRatingFeeling[eptHUNGER]		= .005;
		feelRatingFeeling[eptTHIRST]		= .005;

		// Always wander.
		rtThreshold						= 0;

		sResourcesRequired[edrHEAD]		= false;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;

	
	};

		
	//*********************************************************************************
	void CActivityMoveHintWander::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
#define rWANDERAMOUNT	1.0
#define rWANDERDISTANCE	10.0
		
		CVector3<> v3_body = gaiSystem.v3GetBodyLocation();

		// Get desired velocity.
		CVector2<>	v2_rand = gaiSystem.v2GetLastDesiredDirection();

		// Get the stay near distance, if it exists.
		CBrain* pbr = pbrBRAIN;
		TReal r_stay_near = 100000.0f;
		Assert(dynamic_cast<CActivityMoveCommandStayNear*>(pbr->sapactActivities[eatSTAY_NEAR]));
		CActivityMoveCommandStayNear* pasn = (CActivityMoveCommandStayNear*)  pbr->sapactActivities[eatSTAY_NEAR];

		if (pasn && pasn->bIsActive())
		{
			r_stay_near = pasn->rTooFarAway;

			CVector2<> v2_target	= pasn->pinsTarget->v3Pos();
			CVector2<> v2_me		= v3_body;
			CVector2<> v2_to_target = v2_target - v2_me;

			if (v2_to_target.tLenSqr() > r_stay_near * r_stay_near * 0.75f * 0.75f)
			{
				v2_rand = v2_to_target;
			}
		}

		else if (v2_rand.bIsZero())
			v2_rand = gaiSystem.v3GetHeadPoint();

		// Turn it into a direction of motion.
		if (!v2_rand.bIsZero())
			v2_rand.Normalise(rWANDERDISTANCE);

		// Have we had troubles pathfinding lately?
		if (gaiSystem.sNow - pbrBRAIN->sLastSuccessfulPath > 3.0f)
		{
			// Rotate the random vector.
			Swap(v2_rand.tX, v2_rand.tY);
			if (gaiSystem.rnd() & (1 << 3))
				v2_rand.tX = - v2_rand.tX;
			else
				v2_rand.tY = - v2_rand.tY;
		}


		// Perturb it randomly.
		v2_rand += CVector2<>(  (TReal)gaiSystem.rnd(- rWANDERAMOUNT, rWANDERAMOUNT), 
								(TReal)gaiSystem.rnd(- rWANDERAMOUNT, rWANDERAMOUNT));

		// Perturb wander vector by herding hints.
		CSynthesizer *psyn	= gaiSystem.psynGetCurrentSynthesizer();
		CVector2<>	v2_hint = psyn->v2HerdHint;
		if (v2_hint.tX != 0 || v2_hint.tY != 0)
		{
			v2_hint.Normalise(psyn->rtHerdImportance.fVal * rWANDERAMOUNT);
			v2_rand += v2_hint;
		}

		// Perturb wander vector by fleeing hints.
		v2_hint				= psyn->v2FleeHint;
		if (v2_hint.tX != 0 || v2_hint.tY != 0)
		{
			v2_hint.Normalise(psyn->rtFleeImportance.fVal * rWANDERAMOUNT);
			v2_rand += v2_hint;
		}

		// Set speed to imporance averaged with 1.
		CRating rt_speed = (1 + rt_importance.fVal) * .5f;
		
		// Finally, move.
		DoMoveInDirection(rt_importance, v2_rand, rt_speed);
		DoPointHead(rt_importance, v2_rand, rt_speed);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityMoveHintWander::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove about randomly.\n\n\nREADY.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif



//*********************************************************************************************
//
//  class CActivityMoveCommandStalk implementation
//

	CActivityMoveCommandStalk::CActivityMoveCommandStalk
	(
	) : CActivity("Stalk")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -3.0;
		feelRatingFeeling[eptLOVE]		= -1.0;
		feelRatingFeeling[eptANGER]		= 2.5;
		feelRatingFeeling[eptHUNGER]	= 6.0;
		feelRatingFeeling[eptFATIGUE]	= -4.0;
		feelRatingFeeling[eptPAIN]		= -4.0;
		feelRatingFeeling[eptSOLIDITY]  = 0;

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;
	};

	//*********************************************************************************
	CRating CActivityMoveCommandStalk::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf
	)
	{
		// Is the bad guy looking the other way?
		if (pinf->bFacingAway())
			// Yes!  Don't stalk- move faster than that.
			return 0;
		else
			// No!  He might see us.  Stalk.
			return CActivity::rtRate(feel, pinf);
	}


	//*********************************************************************************
	void CActivityMoveCommandStalk::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{	
		// Handle the case where it's time to give up.  We've been chosen, so if it's time, it's prolly our fault.
//		if (pbrBRAIN->bFrustrated())
//		{
			// Ignore the culprit!
//			pinf->Ignore(true);
//			
//			pbrBRAIN->SetWayPoint();
//		}

		DoMoveHeadTo(rt_importance, pinf->v3Location, 0.4f);
//		DoPointHead(rt_importance, pinf->v3ToTarget, 0.2);

		// And crouch, too!
		DoCrouch(rt_importance);
		return;
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityMoveCommandStalk::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove your body closer to the target stealthily.  Move slow, crouch a bit.\n");

		strcat(buffer, "\n\nNOT READY.\nWaiting for crouch.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//  class CActivityMoveCommandCircle implementation
//

	CActivityMoveCommandCircle::CActivityMoveCommandCircle
	(
	) : CActivityDistance("Circle")
	{
		// Set up the rating feeling.
		//  These initializations will be replaced by data files.

		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -1.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 1.0f;
		feelRatingFeeling[eptCURIOSITY] = 1.0f;
		feelRatingFeeling[eptHUNGER]	= 1.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= -2.0f;
		feelRatingFeeling[eptPAIN]		= -3.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;


		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= true;
		sResourcesRequired[edrFORELEGS] = true;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;

	};

	//*********************************************************************************
	CRating CActivityMoveCommandCircle::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf
	)
	{
		if (!pinf->bLiving())
			return 0;

		if (pinf->rDistanceTo < 10.0f && pinf->rDistanceTo > pbrBRAIN->rHeadReach + pinf->pinsTarget->fGetScale())
			return CActivityDistance::rtRate(feel, pinf);
		else 
			return 0;
	}


	//*********************************************************************************
	void CActivityMoveCommandCircle::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{	
		CVector3<> v3_loc = gaiSystem.v3GetBodyLocation();

		if (!bWasChosen())
		{
//			rtSpeed = gaiSystem.rnd(0.2, 1.0);
			rtSpeed = 1.0f;
			sGiveUp = gaiSystem.sNow + gaiSystem.rnd(2.0f, 5.0f);

			v3Start			= 	v3_loc;

			rNearDistance	= gaiSystem.rnd(pbrBRAIN->rWidth + 0.5, 7.0f);

			if (gaiSystem.rnd() & 1 != 0)
				rNearDistance = -rNearDistance;

			// Don't be frustrated right away.
			pbrBRAIN->SetWayPoint();
		}
		else
		{
			// Have we been having pathfinding difficulties?
			if (pbrBRAIN->sLastSuccessfulPath < gaiSystem.sNow - 1.0)
			{
				// Yes!  Stop circling for a bit.
				sCanAct = gaiSystem.sNow + 5.0f;
			}
		}

		if (sGiveUp < gaiSystem.sNow || pbrBRAIN->bFrustrated())
			DontContinue();
		else
			TryToContinue(3);

		// Which direction should we run?
		CVector2<> v2_direction;
		CVector2<> v2_head_direction;

		// Are we inside the near distance?
		if (abs(rNearDistance) >= pinf->rDistanceTo)
		{
			// Yes!  Just keep running straight.
			v2_direction = gaiSystem.v3GetHeadPoint();
			v2_head_direction = v2_direction;
			

			if (pinf->rDistanceTo > 7.0f)
			{
				DontContinue();
				sCanAct = gaiSystem.sNow + 0.5f;
			}
		}
		else
		{
			// We are sure to have a starting point and a closest approach distance.
			TReal r_theta = asin(rNearDistance / pinf->rDistanceTo);

			v2_direction = pinf->v3ToTarget * CRotate3<>(CVector3<>(0,0,1), r_theta);

			v2_head_direction = pinf->v3ToTarget;
		}

		DoMoveInDirection(
			rt_importance, 
			v2_direction,
			rtSpeed);

		DoPointHead(
			rt_importance,
			v2_head_direction,
			rtSpeed);

#if VER_TEST
		char buffer[256];
		sprintf(buffer, "Circling, speed %f\n", rtSpeed.fVal);
		DebugSay(buffer);
#endif
		return;
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityMoveCommandCircle::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove your body in an orbit around the target.\n\n\nREADY.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//  class CActivityMoveCommandStayNear implementation
//

	CActivityMoveCommandStayNear::CActivityMoveCommandStayNear
	(
		CInstance*	pins_target,
		TReal		r_too_far,
		TReal		r_close_enough
	) : CActivity("Stay Near"), pinsTarget(pins_target), rTooFarAway(r_too_far), rCloseEnough(r_close_enough)
	{
		// Default the target to null!
		pinsTarget	= 0;
		u4Target	= u4Hash("Player");

		// Set up the rating feeling.
		//  These initializations will be replaced by data files.

		feelRatingFeeling[eptLOVE]			= 4.0f;

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;

		sFlags[eafEXCLUSIVE] = true;

	};

	//*********************************************************************************
	CRating CActivityMoveCommandStayNear::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		
	)
	{
		if (!bIsActive() || gaiSystem.paniGetCurrentAnimal()->bDead())
			return 0.0f;

		// Make sure we have a valid target.
		if (!pinsTarget)
		{
			pinsTarget = wWorld.ppartPartitionList()->pinsFindInstance(u4Target);
			AlwaysAssert(pinsTarget);
		}

		// Ignore the influence-  only pay attention to the target!
		CVector2<> v2_body = gaiSystem.v3GetBodyLocation();
		CVector2<> v2_target = pinsTarget->v3Pos();
		TReal r_dist_sqr = (v2_target - v2_body).tLenSqr();

		// If we are comfortably close, then return 0.
		if (!sFlags[eafACTIVE] || r_dist_sqr < rCloseEnough * rCloseEnough)
		{
			return 0;
		}

		if (r_dist_sqr > rTooFarAway * rTooFarAway)
			return 1;

		if (sFlags[eafWASCHOSENLASTTIME])
			return 1;
		else return 0;
	}


	//*********************************************************************************
	void CActivityMoveCommandStayNear::Act
	(
		CRating			rt_importance,
		CInfluence*		
	)
	{	

		CVector3<> v3_to_target = pinsTarget->v3Pos() - gaiSystem.v3GetBodyLocation();
		v3_to_target.tZ = 0.0f;

		// Go to target.
		DebugSay("Stay near that!\n");

		DoMoveInDirection(rt_importance, v3_to_target, 1);
		DoPointHead(rt_importance, v3_to_target, 1);
		return;
	}


	//*****************************************************************************************
	char *CActivityMoveCommandStayNear::pcSave(char * pc) const
	{
		pc = pcSaveInstancePointer(pc, pinsTarget);

		pc = CActivity::pcSave(pc);
		return pc;
	}

	//*****************************************************************************************
	const char *CActivityMoveCommandStayNear::pcLoad(const char * pc)
	{
		extern int iAnimalVersion;
		if (iAnimalVersion >= 9)
		{
			pc = pcLoadInstancePointer(pc, &pinsTarget);
		}

		pc = CActivity::pcLoad(pc);
		return pc;
	}


#if VER_TEST
	//*************************************************************************************
	int CActivityMoveCommandStayNear::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nWhen you get far enough away, run back to ");
		char local_buffer[256];
		sprintf(local_buffer, "%s.\nToo far: %f\nClose enough: %f\n", 
			pinsTarget->strGetInstanceName(), 
			rTooFarAway, 
			rCloseEnough);

		strcat(buffer, local_buffer);
		strcat(buffer, "\n\nREADY.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//  class CActivityMoveCommandStayAway implementation
//

	CActivityMoveCommandStayAway::CActivityMoveCommandStayAway
	(
		CInstance* pins_target,
		TReal		r_too_close,
		TReal		r_far_enough
	) : CActivity("Stay Away"), pinsTarget(pins_target), rTooClose(r_too_close), rFarEnough(r_far_enough)
	{
		// Default the target to null!
		pinsTarget	= 0;
		u4Target	= u4Hash("Player");

		// Set up the rating feeling.
		//  These initializations will be replaced by data files.

		// Wander if hungry, thirsty, or curious.
		feelRatingFeeling[eptFEAR]			= 4.0f;

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;

		sFlags[eafEXCLUSIVE] = true;

	};

	//*********************************************************************************
	CRating CActivityMoveCommandStayAway::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		
	)
	{
		if (!bIsActive() || gaiSystem.paniGetCurrentAnimal()->bDead())
			return 0.0f;

		// Make sure we have a valid target.
		if (!pinsTarget)
		{
			pinsTarget = wWorld.ppartPartitionList()->pinsFindInstance(u4Target);
			AlwaysAssert(pinsTarget);
		}

		// Ignore the influence-  only pay attention to the target!
		CVector2<> v2_body = gaiSystem.v3GetBodyLocation();
		CVector2<> v2_target = pinsTarget->v3Pos();
		TReal r_dist_sqr = (v2_target - v2_body).tLenSqr();

		// If we are comfortably far away, then return 0.
		if (!sFlags[eafACTIVE] || r_dist_sqr > rFarEnough * rFarEnough)
		{
			return 0.0f;
		}

		// Are we close enough to alarm us?
		if (r_dist_sqr < rTooClose * rTooClose)
		{
			// Yes.  Make this the priority.
			return 1;
		}

		// In the intermediate range.
		// Are we in the process of staying away?
		if (sFlags[eafWASCHOSENLASTTIME])
			return 1;
		else
			return 0;
	}


	//*********************************************************************************
	void CActivityMoveCommandStayAway::Act
	(
		CRating			rt_importance,
		CInfluence*		
	)
	{	
		CVector3<> v3_to_target = pinsTarget->v3Pos() - gaiSystem.v3GetBodyLocation();
		v3_to_target.tZ = 0.0f;

		DebugSay("Stay away from that!\n");

		DoMoveInDirection(rt_importance, - v3_to_target, 1);
		DoPointHead(rt_importance, - v3_to_target, 1);


		return;
	}

	//*****************************************************************************************
	char *CActivityMoveCommandStayAway::pcSave(char * pc) const
	{
		pc = pcSaveInstancePointer(pc, pinsTarget);

		pc = CActivity::pcSave(pc);
		return pc;
	}

	//*****************************************************************************************
	const char *CActivityMoveCommandStayAway::pcLoad(const char * pc)
	{
		extern int iAnimalVersion;
		if (iAnimalVersion >= 9)
		{
			pc = pcLoadInstancePointer(pc, &pinsTarget);
		}

		pc = CActivity::pcLoad(pc);
		return pc;
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityMoveCommandStayAway::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		char local_buffer[256];
		sprintf(local_buffer, "Stay away from %s.\n  %f is too close.\n  %f is far enough away.\n",
			pinsTarget->strGetInstanceName(), rTooClose, rFarEnough);
		strcat(buffer, local_buffer);
		strcat(buffer, "\n\nREADY.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//  class CActivityPursue implementation
//

	CActivityPursue::CActivityPursue
	(
	) : CActivityDistance("Pursue")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -3.0;
		feelRatingFeeling[eptLOVE]		= -1.0;
		feelRatingFeeling[eptANGER]		= 4.5;
		feelRatingFeeling[eptHUNGER]	= 4.0;
		feelRatingFeeling[eptFATIGUE]	= -4.0;
		feelRatingFeeling[eptPAIN]		= -4.0;
		feelRatingFeeling[eptSOLIDITY]  = 0;

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;

		// ActivityDistance parameters
//		rStartFalloff	= 5.0;
//		rFalloffFactor	= 1.0;
	};

	//*********************************************************************************
	CRating CActivityPursue::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf
	)
	{
   		CRating rt = CActivityDistance::rtRate(feel, pinf);
		return rt;
	}


	//*********************************************************************************
	void CActivityPursue::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{	
		if (!bWasChosen())
			pbrBRAIN->SetWayPoint();

		// Handle the case where it's time to give up.  We've been chosen, so if it's time, it's prolly our fault.
		if (pbrBRAIN->bFrustrated())
		{
			// Ignore the culprit!
			pinf->Ignore(true);
			
			pbrBRAIN->SetWayPoint();
		}

		// Start making the mouth cause damage.
		DoSetHeadDamage(rt_importance, 1);

		DebugSay("Chasing!\n");
		DoMoveTo(rt_importance, pinf, 1.0f);
		DoPointHead(rt_importance, pinf->v3ToTarget, 1.0f);
		return;
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityPursue::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove towards the target at top speed!\n");
		strcat(buffer, "\n\nREADY.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//  class CActivityFlee implementation
//

	CActivityFlee::CActivityFlee
	(
	) : CActivityDistance("Flee")
	{
		// Set up the rating feeling.
		//  These initializations will be replaced by data files.

		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 8.0;
		feelRatingFeeling[eptLOVE]		= -8.0;
		feelRatingFeeling[eptANGER]		= -8.0;
		feelRatingFeeling[eptHUNGER]	= 0.0;
		feelRatingFeeling[eptFATIGUE]	= 8.0;
		feelRatingFeeling[eptPAIN]		= 8.0;
		feelRatingFeeling[eptSOLIDITY]  = 0;

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;
	};

	//*********************************************************************************
	CRating CActivityFlee::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf
	)
	{
		CRating rt = CActivityDistance::rtRate(feel, pinf);

		RegisterFleeHint
		(
			rt, 
			- pinf->v3ToTarget
		);

		return rt;
	}


	//*********************************************************************************
	void CActivityFlee::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{	
		// Handle the case where it's time to give up.  We've been chosen, so if it's time, it's prolly our fault.
		if (pbrBRAIN->bFrustrated())
		{
			// Give up on fleeing.  It's hopeless.
			sCanAct = gaiSystem.sNow + 30.0f;

			pbrBRAIN->SetWayPoint();
		}

		DebugSay("Run Away!\n");

		CVector2<> v2_flee = gaiSystem.psynGetCurrentSynthesizer()->v2FleeHint;
		if (!v2_flee.bIsZero())
			v2_flee.Normalise();

		v2_flee += gaiSystem.v2GetBodyFacing() * 2.0f;

		DoMoveInDirection(rt_importance, v2_flee, 1.0f);
		DoPointHead(rt_importance, v2_flee, 1.0f);
		return;
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityFlee::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove your body away from target at top speed!\n");
		strcat(buffer, "\n\nREADY.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//  class CActivityFlee implementation
//

	CActivityJumpBack::CActivityJumpBack
	(
	) : CActivity("Jump Back")
	{
		// Set up the rating feeling.
		//  These initializations will be replaced by data files.

		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 4.0;
		feelRatingFeeling[eptLOVE]		= -3.0;
		feelRatingFeeling[eptANGER]		= 2.0;
		feelRatingFeeling[eptHUNGER]	= 2.0;
		feelRatingFeeling[eptFATIGUE]	= 4.0;
		feelRatingFeeling[eptPAIN]		= 4.0;
		feelRatingFeeling[eptSOLIDITY]  = 0;

		sResourcesRequired[edrHEAD]		= false;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;

		rSafeDistance	= 2.0f;
		rUnsafeDistance	= 1.0f;
	};

	//*********************************************************************************
	CRating CActivityJumpBack::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf
	)
	{
		if (!pinf->bLiving())
			return 0;

		TReal r_head_reach = pbrBRAIN->rHeadReach;
		if (pinf->rDistanceTo < rUnsafeDistance + r_head_reach)
		{
			CRating rt = CActivity::rtRate(feel, pinf);
			return rt;
		}

		return 0;
	}


	//*********************************************************************************
	void CActivityJumpBack::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{	
		if (!bWasChosen())
		{
			rCinematicAngle = gaiSystem.rnd(-PI/4, PI/4);
			sGiveUp	= gaiSystem.sNow + 5.0f;
		}

		DebugSay("Jump Back!\n");

		CVector3<> v3 = - (pinf->v3ToTarget * CRotate3<>(CVector3<>(0,0,1), rCinematicAngle));
		DoMoveInDirection(rt_importance, v3, 1.0f);

		// If we are looking in a direction close to opposite our direction of travel, look at target.
		CVector2<> v2_side_vector	= CVector2<>(pinf->v3ToTarget.tY, - pinf->v3ToTarget.tX);
		CVector2<> v2_head			= gaiSystem.v3GetHeadPoint();

		TReal r_cross = v2_side_vector ^ v2_head;

		// Are we facing target?
		if (r_cross > 0)
		{
			// Yes!  Face target fully.
			DoPointHead(rt_importance, pinf->v3ToTarget, 0.5f);
		}
		else
		{
			// No.  Face away fully.
			DoPointHead(rt_importance, - pinf->v3ToTarget, 0.5f);
		}

		// Try to keep doing this as long as we are too close.
		TryToContinue(3.0);

		return;
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityJumpBack::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		

		strcat(buffer, "\n\nA quick retreat.\n\n\nREADY.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//  class CActivityGetOut implementation
//

	CActivityGetOut::CActivityGetOut 
	(
	) : CActivity("Get Out")
	{
		// Set up the rating feeling.
		//  These initializations will be replaced by data files.

		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 4.0;
		feelRatingFeeling[eptLOVE]		= -1.0;
		feelRatingFeeling[eptANGER]		= 2.0;
		feelRatingFeeling[eptHUNGER]	= 2.0;
		feelRatingFeeling[eptFATIGUE]	= 0.0;
		feelRatingFeeling[eptPAIN]		= 4.0;
		feelRatingFeeling[eptSOLIDITY]  = 0;

		sResourcesRequired[edrHEAD]		= false;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		// The GetOut activity is exclusive. 
		sFlags[eafEXCLUSIVE] = true;

		sResourcesUsed = sResourcesRequired;
	};

	//*********************************************************************************
	CRating CActivityGetOut::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf
	)
	{
		// Get Out takes no direct object, because most of the things it cares about are the things
		// that we ignore.
		Assert(pinf == 0);

		// Clear the culprits.
		pinfInside= 0;

		CBrain* pbr = gaiSystem.pbrGetCurrentBrain();

		if (!bIsActive() || pbr->paniOwner->bDead())
			return 0.0f;

		//
		//  Check the slope we have first.
		//

		// Do we have terrain?
		CTerrain* ptrr = CWDbQueryTerrain().tGet();
		if (ptrr)
		{
			//  Yes!  Get the slope.
			CVector3<>  v3_body = gaiSystem.v3GetBodyLocation();
			TReal		r_terrain_height = 0;
			const CPlane* ppl = 0;

			r_terrain_height = ptrr->pqnqGetQueryRoot()->rHeight(v3_body.tX, v3_body.tY, &ppl);
			
			// If 2 meters under our walkover height is still over the terrain, then we are on an object or jumping
			// and can skip the whole deal.
			if (v3_body.tZ + pbr->rWalkOver - 2.0f > r_terrain_height)
				// Skip the whole deal.
				ppl = 0;

			if (ppl)
			{
				// Is the slope to high?
				if (pbr->bTooSteepDownHill(*ppl))
				{
					// Yes.  Save the desired direction.
					v2Direction = ppl->d3Normal;
					return 1;
				}
			}
		}

		//
		//	Check the influences.
		//

//		if (pbr->bLastPathSucceeded)
//			return 0;

//		TSec s_fail_time = gaiSystem.sNow - pbr->sLastSuccessfulPath;
//		if (s_fail_time > 0.5f)
		{
			// Loop through all influences.  Find if any intersect the dino.
			CInfluenceList* pinfl = &pbr->pwvWorldView->inflInfluences;
			CInfluenceList::iterator pinf = pinfl->begin();

			// Assume a spherical dinosaur.
			CBoundVolSphere bvs(pbr->rWidth * 0.4f);

			CPresence3<> pr3_animal		= gaiSystem.paniGetCurrentAnimal()->pr3Presence();
			pr3_animal.rScale = 1.0f;

			for ( ; pinf != pinfl->end(); ++pinf)
			{
				if ((*pinf).setNodeFlags[ensfIS_PASSABLE])
					continue;

				if ((*pinf).rMinZ > pbr->paniOwner->v3Pos().tZ + pbr->rWalkUnder || 
					(*pinf).rMaxZ < pbr->paniOwner->v3Pos().tZ + pbr->rWalkOver)
					continue;

				// Passed the trivial rejections.  
				{
					// Okay, was this influence within reach? (1.5 because unit cube is bigger than unit sphere)
					if ((*pinf).rDistanceTo <= 1.5f * (*pinf).pinsTarget->fGetScale() + pbr->rWidth * 0.5f)
					{
						// Maybe....  Okay, do the proper intersect test.

						CPresence3<>*  ppr3_influence	= &(*pinf).pinsTarget->pr3Presence();

						const CBoundVol* pbv = (*pinf).pinsTarget->paiiGetAIInfo()->pbvGetBoundVol((*pinf).pinsTarget);
						if (pbv->bIntersects(bvs, ppr3_influence, &pr3_animal))
						{
							// We are intersecting this guy!  Get Out!
							pinfInside = &(*pinf);
							return 1.0;
						}
					}
				}
			}
		}

		pinfInside = 0;
		return 0.0f;
	}


	//*******************************************************************************
	void CActivityGetOut::Act
	(
		CRating			rt_importance,
		CInfluence*		
	)
	{	
		// In a box?
		if (pinfInside)
		{
			// Yes!  Get out.
			DebugSay("Get Out of object!!!\n");

			CVector3<> v3_point = pinfInside->v3ClosestPointTo(gaiSystem.v3GetBodyLocation());
			v3_point = v3_point - gaiSystem.v3GetBodyLocation();
			v3_point.tZ = 0.0f;

			if (v3_point.bIsZero())
			{
				v3_point.tX = 1.0f;
			}

			DoPointHead(rt_importance, v3_point, 1.0);

			v3_point.Normalise(10.0f);
			DoMoveToLocationBlind(rt_importance, gaiSystem.v3GetBodyLocation() + v3_point, 1.0f);
			return;
		}
		else
		{
			// No!  On a hill.  Go down.
			DebugSay("Get Down the Hill!!!\n");
			v2Direction.Normalise(30.0f);

			// Cheat.   HACK HACK HACK
			CBrain* pbr = pbrBRAIN;

			// Avoid objects, but not terrain.
			DoMoveToLocationBlind(rt_importance, 
				CVector2<>(gaiSystem.v3GetBodyLocation()) + v2Direction, 
				1.0f);

			return;
		}
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityGetOut::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);

		strcat(buffer, "\n\nMove your body out of the thing you are inside.  Pathfinding fix.\n");
		strcat(buffer, "\n\nREADY.\nThis will help dinos get unstuck.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//  class CActivityDash implementation
//

	CActivityDash::CActivityDash
	(
	) : CActivity("Dash")
	{
		// Set up the rating feeling.
		//  These initializations will be replaced by data files.

		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 1.5;
		feelRatingFeeling[eptLOVE]		= 0.0;
		feelRatingFeeling[eptANGER]		= 1.5;
		feelRatingFeeling[eptHUNGER]	= 1.5;
		feelRatingFeeling[eptFATIGUE]	= 0.0;
		feelRatingFeeling[eptPAIN]		= 4.5;
		feelRatingFeeling[eptSOLIDITY]  = 0;

		sResourcesRequired[edrHEAD]		= false;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;
	};

	//*********************************************************************************
	CRating CActivityDash::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf
	)
	{
		return CActivity::rtRate(feel, pinf);
	}


	//*******************************************************************************
	void CActivityDash::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{	
		if (!bWasChosen())
		{
			// Set up time limit.
			sGiveUp = gaiSystem.sNow + gaiSystem.rnd(1.0f, 3.0f);

			// Set up random location 10 meters away.
			v3Target = CVector3<>(10.0f,0,0) * CRotate3<>(CVector3<>(0,0,1), gaiSystem.rnd(0, 2.0f * 3.14159f));
			v3Target += gaiSystem.v3GetBodyLocation();
		}

		// If the time is up,		 OR we are within a meter of the target
		if (gaiSystem.sNow > sGiveUp || rDISTANCE_SQR(gaiSystem.v3GetBodyLocation(), v3Target) < 1.0)
		{
			// We are done.
			DontContinue();
		}
		else
		{
			// Otherwise, keep trying.
			TryToContinue();
		}

		DoMoveToLocation(rt_importance, v3Target, 1.0f);
		DoPointHead(rt_importance, v3Target - gaiSystem.v3GetBodyLocation(), 1.0f);
		return;
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityDash::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);

		strcat(buffer, "\n\nMove your body in a random direction for ten meters.\n");
		strcat(buffer, "\n\nREADY.\nGood for compy's and frantic dinos.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//  class CActivityApproach implementation
//

	//*************************************************************************************
	CActivityApproach::CActivityApproach
	(
	) : CActivityDistance("Approach")
	{
				// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -1.0;
		feelRatingFeeling[eptLOVE]		= -1.0;
		feelRatingFeeling[eptANGER]		= 4.5;
		feelRatingFeeling[eptHUNGER]	= 4.0;
		feelRatingFeeling[eptFATIGUE]	= -4.0;
		feelRatingFeeling[eptPAIN]		= -4.0;
		feelRatingFeeling[eptSOLIDITY]  = 0;

		sResourcesRequired[edrHEAD]		= false;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;

		// ActivityDistance parameters
//		rStartFalloff	= 5.0;
//		rFalloffFactor	= 1.0;
	};

	//*********************************************************************************
	CRating CActivityApproach::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf
	)
	{
		CRating rt = CActivityDistance::rtRate(feel, pinf);
		return rt;
	}


	//*******************************************************************************
	void CActivityApproach::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		// Make sure we node our target if possible.
		pinf->setNodeFlags[ensfNODE_ME] = true;

		// Handle the case where it's time to give up.  We've been chosen, so if it's time, it's prolly our fault.
		if (pbrBRAIN->bFrustrated())
		{
			// Ignore the culprit!
			pinf->Ignore(true);
			
			pbrBRAIN->SetWayPoint();
		}

		if (!bWasChosen())
		{
			// Make sure that we get a new angle if we've 
			sGiveUp = -1000.0f;
		}


		if (sGiveUp < gaiSystem.sNow)
		{
			// Give up on the current cinematic angle, and choose another one.
			sGiveUp = gaiSystem.sNow + gaiSystem.rnd(1.0f, 6.0f);
#define rMAX_CINEMATIC_ANGLE 0.8f
			rCinematicAngle = gaiSystem.rnd(-rMAX_CINEMATIC_ANGLE, rMAX_CINEMATIC_ANGLE);
		}

		// Are we heading on a straight path?
		if (rCinematicAngle != 0.0f)
		{
			// Is the target facing away from us?
			if (pinf->bFacingAway())
			{
				// Yes!  Facing away.  Just make it a flat out pursue.
				rCinematicAngle = 0.0f;
			}
		}

		// Have we been failing in finding a path?
		if (rCinematicAngle == 0.0f || pbrBRAIN->sLastSuccessfulPath < gaiSystem.sNow - 1.0 ||
			pinf->rDistanceTo < 5.0f)
		{
			// Then take the straight path.
			rCinematicAngle = 0.0f;
			DoMoveToLocation(rt_importance, pinf->v3Location, 1.0f);
			DebugSay("Simple approach!\n");
		}
		else
		{
			DoMoveInDirection(rt_importance, pinf->v3ToTarget * CRotate3<>(CVector3<>(0,0,1), rCinematicAngle), 1.0f);
			DebugSay("Cinematic approach!\n");
		}

		// Start making the mouth cause damage.
		DoSetHeadDamage(rt_importance, 1);

		DoPointHead(rt_importance, pinf->v3ToTarget, 0.5f);
		return;
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityApproach::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivityDistance::iGetDescription(buffer, i_buffer_length);

		strcat(buffer, "\n\nPerform a cinematic approach.  Zig, zag, and show your profile.  Be artistic.\nNeeds some work.\n");

		strcat(buffer, "\n\nREADY.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//  class CActivityMoveBy implementation
//
	
	//*************************************************************************************
	CActivityMoveBy::CActivityMoveBy
	(
	) : CActivityDistance("MoveBy")
	{
		// Set up the rating feeling.
		//  These initializations will be replaced by data files.
		feelRatingFeeling[eptFEAR]		= -2.0;
		feelRatingFeeling[eptLOVE]		= -1.0;
		feelRatingFeeling[eptANGER]		= 3.0;
		feelRatingFeeling[eptHUNGER]	= 4.0;
		feelRatingFeeling[eptFATIGUE]	= -3.0;
		feelRatingFeeling[eptPAIN]		= -3.0;
		feelRatingFeeling[eptSOLIDITY]  = 0;

		sResourcesRequired[edrHEAD]		= false;
		sResourcesRequired[edrTAIL]		= true;
		sResourcesRequired[edrFORELEGS] = true;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;
	};

	//*********************************************************************************
	CRating CActivityMoveBy::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf
	)
	{
		if (!pinf->bLiving())
			return 0;

		CRating rt = CActivityDistance::rtRate(feel, pinf);
		if (pinf->bFacingAway())
			return rt * 0.2;
		else
			return rt;
	}


	//*******************************************************************************
	void CActivityMoveBy::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		// Make sure we node our target if possible.
		pinf->setNodeFlags[ensfNODE_ME] = true;

		// Handle the case where it's time to give up.  We've been chosen, so if it's time, it's prolly our fault.
		if (pbrBRAIN->bFrustrated())
		{
			// Ignore the culprit!
			pinf->Ignore(true);
			
			pbrBRAIN->SetWayPoint();
		}

		CVector3<> v3_loc = gaiSystem.v3GetBodyLocation();

		if (!bWasChosen())
		{
			v3Start			= 	v3_loc;

			if (pbrBRAIN->rWidth < 5.0)
				rNearDistance	= gaiSystem.rnd(pbrBRAIN->rWidth + 0.5, 5.0f);
			else
				rNearDistance	= pbrBRAIN->rWidth + 0.5;

			if (gaiSystem.rnd() & 1 != 0)
				rNearDistance = -rNearDistance;

			sGiveUp = 15.0f;
		}

		// Which direction should we run?
		CVector2<> v2_direction;

		// Are we past the target?
		if (abs(rNearDistance) >= pinf->rDistanceTo || rDISTANCE_SQR(v3Start, pinf->v3Location) < rDISTANCE_SQR(v3Start, v3_loc))
		{
			// Yes!  We are past.  Just keep running straight.
			v2_direction = gaiSystem.v3GetHeadPoint();

			sGiveUp = Min(sGiveUp, gaiSystem.sNow + 1.0f);

			if (pinf->rDistanceTo > 7.0f || sGiveUp < gaiSystem.sNow)
			{
				DontContinue();
				sCanAct = gaiSystem.sNow + 0.5f;
			}
		}
		else
		{
			// We are sure to have a starting point and a closest approach distance.
			TReal r_theta = asin(rNearDistance / pinf->rDistanceTo);

			v2_direction = pinf->v3ToTarget * CRotate3<>(CVector3<>(0,0,1), r_theta);
		}

		// Start making the mouth cause damage.
		DoSetHeadDamage(rt_importance, 1);

		DoMoveInDirection(
			rt_importance, 
			v2_direction,
			1.0f);

		DoPointHead(
			rt_importance,
			v2_direction,
			0.9);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityMoveBy::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivityDistance::iGetDescription(buffer, i_buffer_length);

		strcat(buffer, "\n\nPerform a run-by.\nNeeds some work.\n");

		strcat(buffer, "\n\nREADY.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif




//*********************************************************************************************
//
//  class CActivityJump implementation
//
	
	//*************************************************************************************
	CActivityJump::CActivityJump
	(
	) : CActivityDistance("Jump")
	{
		// Set up the rating feeling.
		//  These initializations will be replaced by data files.
		feelRatingFeeling[eptFEAR]		= 1;
		feelRatingFeeling[eptLOVE]		= 1;
		feelRatingFeeling[eptANGER]		= 1.0;
		feelRatingFeeling[eptHUNGER]	= 1.0;
		feelRatingFeeling[eptFATIGUE]	= 1.0;
		feelRatingFeeling[eptPAIN]		= 1.0;
		feelRatingFeeling[eptSOLIDITY]  = 1;

		sResourcesRequired[edrHEAD]		= false;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;
		sResourcesUsed[edrHINDLEGS] = false;

		sFlags[eafEXCLUSIVE] = true;
	};

	//*********************************************************************************
	CRating CActivityJump::rtRateSingleInfluence
	(
		const CFeeling&	feel,
		const CInfluence*		pinf
	)
	{
		CBrain* pbr = pbrBRAIN;

		// Are we close enough to the influence to jump it?
		CVector3<> v3_me		= gaiSystem.v3GetBodyLocation();
		CVector3<> v3_closest	= pinf->v3ClosestPointTo(v3_me);
		CVector3<> v3_to_closest= v3_closest - v3_me;
		TReal r_distance_sqr	= v3_to_closest.tLenSqr();


		TReal r_jump_distance = gaiSystem.rGetBodySpeed();
		TReal r_jump_distance_sqr = r_jump_distance * r_jump_distance;

		// Are we too far away?
		if (r_distance_sqr > r_jump_distance_sqr * 0.5f)
		{
			// Yes.
			return 0;
		}

		// Are me moving towards the influence?
		CVector3<> v3_velocity = gaiSystem.v3GetBodyVelocity();
		
		// Are we going roughly toward the target?
		if (v3_velocity * pinf->v3ToTarget < 0)
			// No!
			return 0;

		// Are we too close?
		if (r_distance_sqr < r_jump_distance_sqr * 0.5f * 0.5f)
		{
			// Yes!  Do not jump now, and stop counting on it for pathfinding.
			pbr->bCanJump = false;
			return 0;
		}


		// Then jump!
		return 1;
	}

	//*********************************************************************************
	CRating CActivityJump::rtRate
	(
		const CFeeling&	feel,
		CInfluence*		pinf_arg
	)
	{
		// Get Out takes no direct object, because most of the things it cares about are the things
		// that we ignore.
		Assert(pinf_arg == 0);

		CBrain* pbr = pbrBRAIN;

		// Only jump if this activity is active.
		pbr->bJumpEnabled	= bIsActive();
		pbr->bCanJump	  	= pbr->bJumpEnabled;
		if (!bIsActive() || sCanAct > gaiSystem.sNow)
			return 0;

		// Were we chosen last time?
		if (bWasChosen())
			// Yes!  Follow through on the crouch, please.
			return 1;

		// Are we moving quickly?
		TReal r_speed = gaiSystem.rGetBodySpeed();
		pbr->bCanJump = r_speed > 2.0f;
		if (!pbr->bCanJump)
			return 0;

		// Loop through all influences.  Find if any intersect the dino.
		CInfluenceList* pinfl = &pbr->pwvWorldView->inflInfluences;
		CInfluenceList::iterator pinf = pinfl->begin();

		for ( ; pinf != pinfl->end(); ++pinf)
		{
		// Can we push through the influence?
			if ((*pinf).setNodeFlags[ensfIS_PASSABLE])
				continue;

			CRating rt = rtRateSingleInfluence(feel, &(*pinf));

			if (rt > 0.0f)
				return rt;
		}

		return 0.0f;
	}

	//*******************************************************************************
	void CActivityJump::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		// Were we chosen last time?
		if (!bWasChosen())
		{
			sGiveUp = gaiSystem.sNow + 0.4f;
		}

		if (sGiveUp > gaiSystem.sNow)
		{
			// Waiting to crouch.
//			DoCrouch(rt_importance, 1.0f);
			DebugSay("Jump\n");
		}
		else
		{
			// Yes!  Follow through on the crouch, please.
			DoJump(rt_importance, 1.0f);
			sCanAct = gaiSystem.sNow + 0.5f;
			DontContinue();
		}
	}

	//*************************************************************************************
	void CActivityJump::Activate(bool b_active_state)
	{
		CActivity::Activate(b_active_state);
	}


#if VER_TEST
	//*************************************************************************************
	int CActivityJump::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivityDistance::iGetDescription(buffer, i_buffer_length);

		strcat(buffer, "\n\nPerform a jump.\nNeeds some work.\n");

		strcat(buffer, "\n\nREADY.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif
