/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 *	Implementation of HeadActivities.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/HeadActivities.cpp                                           $
 * 
 * 30    9/20/98 7:26p Agrant
 * use the node me flag
 * 
 * 29    9/16/98 9:19p Agrant
 * Less heavy-handed look around behavior
 * 
 * 28    9/12/98 1:09a Agrant
 * somewhat better sniffing
 * 
 * 27    9/08/98 7:40p Agrant
 * dont glare at somebody who isn't looking
 * 
 * 26    9/06/98 8:15p Agrant
 * cock head activity
 * 
 * 25    9/05/98 8:27p Agrant
 * some sniff tweaks
 * 
 * 24    9/02/98 5:21p Agrant
 * glare fixes
 * 
 * 23    8/31/98 6:36p Agrant
 * Greatly reduced LookAround probability.
 * 
 * 22    8/31/98 6:31p Agrant
 * massive dino resources reallocation
 * 
 * 21    8/31/98 5:09p Agrant
 * Sniff Target now uses all resources
 * 
 * 20    8/26/98 7:14p Agrant
 * Sniff rework
 * 
 * 19    8/24/98 10:03p Agrant
 * Prevent simultaneous glares with a random intializer
 * Reduce glare time somewhat
 * 
 * 18    8/23/98 2:22p Agrant
 * do not glare at something to close
 * 
 * 17    8/23/98 3:50a Agrant
 * improved lookaround and glare
 * 
 * 16    8/22/98 7:18p Agrant
 * lookaround responds to startle flag
 * 
 * 15    8/21/98 12:43a Agrant
 * sniff air implementation
 * 
 * 14    8/20/98 11:07p Agrant
 * sniff target no longer asserts
 * 
 * 13    7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 **********************************************************************************************/

#include "Common.hpp"

#include "HeadActivities.hpp"
#include "Influence.hpp"
#include "AIMain.hpp"
#include "Brain.hpp"

#include "Lib/Sys/DebugConsole.hpp"


#define RAND11 (gaiSystem.rnd(-1.0f, 1.0f))
#define PI		3.14159265f

//*********************************************************************************************
//
//	Class CActivityLookAt implementation.
//


	//*************************************************************************************
	CActivityLookAt::CActivityLookAt() : CActivity("LookAt")
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


		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = false;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;
	};


	//*************************************************************************************
	void CActivityLookAt::Act(CRating rt, CInfluence* pinf)
	{
		// Look at the target.

		// Tell physics to move head to target at debug speed.
		DoPointHead(rt, pinf->v3Location - gaiSystem.v3GetBodyLocation(), 1.0);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityLookAt::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nTurn head so we are looking at target.\n");

		strcat(buffer, "\n\nCAN USE, but not done.\nWaiting for head orientation, as differentiated from head position.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//	Class CActivityTaste implementation.
//

	//*************************************************************************************
	CActivityTaste::CActivityTaste() : CActivity("Taste")
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

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= true;
		sResourcesRequired[edrFORELEGS] = true;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= true;

		sResourcesUsed = sResourcesRequired;
	};


	//*************************************************************************************
	void CActivityTaste::Act(CRating rt, CInfluence* pinf)
	{	
		// Is this our first time?
		if (!bWasChosen())
		{	
			// We aren't ready to slurp just yet.
			sFlags[eafPHASEONE] = false;
		}

		if (sGiveUp < gaiSystem.sNow)
			DontContinue();

		CVector3<> v3_target_location = pinf->v3Location;

		// Are we close to our target?
		if (sFlags[eafPHASEONE] || rDISTANCE_SQR(v3_target_location , gaiSystem.v3GetHeadLocation()) < 0.1f * 0.1f)
		{
			// Did we just get here for the first time?
			if (!sFlags[eafPHASEONE])
			{
				sFlags[eafPHASEONE] = true;
				sGiveUp = gaiSystem.sNow + 1.0f;
			}

			// Yes.  Lick it and move head a tad.
			DebugSay("Begin Taste Test\n");

			// PLAY SLURP SOUND!
			// IF SOUND IS DONE, sCantAct sets in

			// Bob the head upwards a little bit.
			DoMoveHeadTo(rt, v3_target_location + CVector3<>(0.0f,0.0f,0.1f), 0.5f);
		}
		else
		{
			// No.   Get close.
			// Move head slowly to target
			DoMoveHeadTo(rt, v3_target_location, 0.3f);

			// Open mouth slightly.
			DoOpenMouth(rt, 0.2f, 1.0f);

			DebugSay("Approaching taste test\n");
		}
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityTaste::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove head to target slowly, open mouth slightly, and make a licking sound.\n");

		strcat(buffer, "\n\nCAN USE, not done.\nWaiting for precision head placement\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif

//*********************************************************************************************
//
//	Class CActivityLookAt implementation.
//


	//*************************************************************************************
	CActivitySniffTarget::CActivitySniffTarget() : CActivitySniffAir("Sniff Target")
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

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= true;
		sResourcesRequired[edrFORELEGS] = true;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= true;

		sResourcesUsed = sResourcesRequired;

		sWait			= 0.5f;
};


	//*************************************************************************************
	void CActivitySniffTarget::Act(CRating rt, CInfluence* pinf)
	{
		if (!bWasChosen())
		{
			sStartSniffing = -1.0f;
		}

		// Make sure we node our target if possible.
		pinf->setNodeFlags[ensfNODE_ME] = true;

		// Just walk toward target while sniffing.
		CActivitySniffAir::Act(rt, pinf);

		TReal r_close_enough = pbrBRAIN->rHeadReach * 1.5f;

		// Are we close enough for a good sniff?
		if (pinf->rDistanceTo > r_close_enough)
		{
			CVector3<> v3_target = pinf->v3ToTarget;
			v3_target.Normalise(pinf->rDistanceTo - r_close_enough);
			v3_target += gaiSystem.v3GetBodyLocation();

			// Walk up to target.
			DoMoveToLocation(rt, v3_target, 0.3f);
			DebugSay("Approaching sniff test\n");
		}
		else
		{
			CVector3<> v3_target = pinf->v3ClosestPointTo(gaiSystem.v3GetHeadLocation());

			// Walk up to target.
			DoMoveHeadTo(rt, v3_target, 0.3f);

			// Did we just start sniffing the target up close?
			if (sStartSniffing < 0)
			{
				// Yes!
				sStartSniffing = gaiSystem.sNow;
			}
			else
			{
				// Nope.  We've been sniffing for a short time now.

				// Reduce curiosity about target.
				pinf->feelAboutTarget[eptCURIOSITY] -= 0.1f * (gaiSystem.sNow - sStartSniffing);
				sStartSniffing = gaiSystem.sNow;

				DebugSay("Sniff reducing curiosity.\n");
			}
		}
	}

#if VER_TEST
	//*************************************************************************************
	int CActivitySniffTarget::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMove head to target slowly, close mouth, and make a sniffing sound.\n");

		strcat(buffer, "\n\nCAN USE.  Not done.\nWaiting for precision head placement.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityGlare implementation.
//
#define sGLARE_DURATION 2.0f

	//*************************************************************************************
	CActivityGlare::CActivityGlare() : CActivity("Glare")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0;
		feelRatingFeeling[eptLOVE]		= -2.0;
		feelRatingFeeling[eptANGER]		= 2.0;
		feelRatingFeeling[eptCURIOSITY] = 2.0;
		feelRatingFeeling[eptHUNGER]	= 0.0;
		feelRatingFeeling[eptTHIRST]	= 0.0;
		feelRatingFeeling[eptFATIGUE]	= -2.0;
		feelRatingFeeling[eptPAIN]		= -10.0;
		feelRatingFeeling[eptSOLIDITY]  = 0.0;

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= true;
		sResourcesRequired[edrFORELEGS] = true;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= true;

		sResourcesUsed = sResourcesRequired;

		sCanAct = gaiSystem.sNow + gaiSystem.rnd(sGLARE_DURATION, 3.0f * sGLARE_DURATION);
	};

	//*************************************************************************************
	CRating CActivityGlare::rtRate(const CFeeling& feel, CInfluence* pinf)
	{
		if (pinf->bFacingAway() || pinf->rDistanceTo < 2.0f + pbrBRAIN->rHeadReach + pinf->pinsTarget->fGetScale())
			return 0.0;

		CRating rt = CActivity::rtRate(feel, pinf);

		if (pbrBRAIN->bStartled)
			rt *= 3;

		return rt;	
	}


	//*************************************************************************************
	void CActivityGlare::Act(CRating rt, CInfluence* pinf)
	{
		if (!bWasChosen())
		{
			sGiveUp	= gaiSystem.sNow + gaiSystem.rnd(0.5f * sGLARE_DURATION, 1.5f * sGLARE_DURATION);
		}

		if (sGiveUp > gaiSystem.sNow)
		{
			TryToContinue();
		}
		else
		{
			DontContinue();
			sCanAct = gaiSystem.sNow + gaiSystem.rnd(6.0f, 15.0f);
		}

		// Look at target.
		DoPointHead(rt, pinf->v3Location - gaiSystem.v3GetBodyLocation(), 1.0);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityGlare::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLook at the target, but somehow make it seem more menacing.\n");

		strcat(buffer, "\n\nREADY.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivityCockHead implementation.
//

	//*************************************************************************************
	CActivityCockHead::CActivityCockHead() : CActivity("CockHead")
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
	

	//*************************************************************************************
	CRating CActivityCockHead::rtRate(const CFeeling& feel, CInfluence* pinf)
	{
		if (pinf->rDistanceTo < 2.0f + pbrBRAIN->rHeadReach + pinf->pinsTarget->fGetScale())
			return 0.0;

		CRating rt = CActivity::rtRate(feel, pinf);

		if (pbrBRAIN->bStartled)
			rt *= 3;

		return rt;	
	}


	//*************************************************************************************
	void CActivityCockHead::Act(CRating rt, CInfluence* pinf)
	{
		if (!bWasChosen())
		{
			sGiveUp	= gaiSystem.sNow + gaiSystem.rnd(0.5f * sGLARE_DURATION, 1.5f * sGLARE_DURATION);
			rHeadCock = gaiSystem.rnd(PI * 0.1f, PI * 0.25f);
			if (gaiSystem.rnd() & 0x00010000)
				rHeadCock = - rHeadCock;
		}

		if (sGiveUp > gaiSystem.sNow)
		{
			TryToContinue();
		}
		else
		{
			DontContinue();
			sCanAct = gaiSystem.sNow + gaiSystem.rnd(0.0f, 15.0f);
		}

		// Look at target.
		DoPointHead(rt, pinf->v3Location - gaiSystem.v3GetBodyLocation(), 1.0);
		DoCockHead(rt, rHeadCock, 1.0f);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityCockHead::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLook at target, but cock head.  Maybe twist it a bit to give one eye a good view.\n");

		strcat(buffer, "\n\nNOT READY.\nWaiting for precision head orientation, head cock.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


//*********************************************************************************************
//
//	Class CActivitySniffAir implementation.
//

	//*************************************************************************************
	CActivitySniffAir::CActivitySniffAir(char * pc_name) : CActivityVocal(pc_name)
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

	
		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = false;
		sResourcesRequired[edrVOICE]	= false;

		sResourcesUsed = sResourcesRequired;

		evtVocalType	= evtSNIFF;
		sWait			= 2.0f;
		rtMaxMouth		= 0.0f;
		rtMouthSpeed	= 0.3f;

		// Allow vocals at somewhat random points.
		sCanAct = gaiSystem.sNow + gaiSystem.rnd(0.0f, 10.0f);
	};


	//*************************************************************************************
	void CActivitySniffAir::Act(CRating rt_importance, CInfluence* pinf)
	{

		if (!bWasChosen())
		{
			v3StartPosition = gaiSystem.v3GetHeadPoint();

			rHeadCock = gaiSystem.rnd(-PI * 0.125, PI * 0.125f);
		}

		// Do the normal thing.
		CActivityVocal::Act(rt_importance, pinf);

		DoPointHead(rt_importance, v3StartPosition + CVector3<>(0,0,0.2f), 0.3);

		// Don't bite when you sniff.
		DoSetHeadDamage(rt_importance, 0.0);

		// Cock the head a teensy bit.
		DoCockHead(rt_importance, rHeadCock, 1.0f);

	}

#if VER_TEST
	//*************************************************************************************
	int CActivitySniffAir::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLift head into the air.  Play a sniff sound and jerk head back/up a tiny bit.  Repeat.\n");

		strcat(buffer, "\n\nCAN USE.  Not done.\nWaiting for precision head placement/orientation.  Ideally, differentiate between the two to allow a sigmoidish neck curve.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif



//*********************************************************************************************
//
//	Class CActivityLookAround implementation.
//

	//*************************************************************************************
	CActivityLookAround::CActivityLookAround() : CActivity("Look Around")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.1;
		feelRatingFeeling[eptLOVE]		= 0.0;
		feelRatingFeeling[eptANGER]		= 0.0;
		feelRatingFeeling[eptCURIOSITY] = 0.1;
		feelRatingFeeling[eptHUNGER]	= 0.0;
		feelRatingFeeling[eptTHIRST]	= 0.0;
		feelRatingFeeling[eptFATIGUE]	= 0.0;
		feelRatingFeeling[eptPAIN]		= 0.0;
		feelRatingFeeling[eptSOLIDITY]  = 0.0;
	};

	//*************************************************************************************
	CRating CActivityLookAround::rtRate(const CFeeling& feel, CInfluence* pinf)
	{
		if (pbrBRAIN->bStartled)
			sCanAct = gaiSystem.sNow - 1.0f;

		CRating rt = CActivity::rtRate(feel, pinf);

		if (pbrBRAIN->bStartled)
			rt *= 20.0f;

		return rt;	
	}

	//*************************************************************************************
	void CActivityLookAround::Act(CRating rt_importance, CInfluence* pinf)
	{
		CBrain* pbr = pbrBRAIN;
		CVector3<> v3_head = gaiSystem.v3GetHeadPoint();

		if (!bWasChosen())
		{
			if (pbr->bStartled)
			{
				v3Target = pbr->v3Startle - gaiSystem.v3GetBodyLocation();
			}
			else
			{
				// Make sure he looks in a direction other than the one he's already looking.

				// Only care about facing, not elevation.
				v3_head.tZ = 0.0f;
				v3Target	= v3_head * CRotate3<>(CVector3<>(0,0,1), gaiSystem.rnd(PI/4.0f, 7.0f*PI/4.0f));
			}
		
			sGiveUp		= gaiSystem.sNow + 3.0f;
		}

		// If we are already near our target, don't stare quite so long.
		CVector2<> v2_head = v3_head;
		v2_head.Normalise();
		CVector2<> v2_target = v3Target;
		v2_target.Normalise();
		if (rDISTANCE_SQR(v2_head, v2_target) < 0.1)
		{
			sGiveUp = Min(gaiSystem.sNow + 0.50f, sGiveUp);
		}

		if (sGiveUp < gaiSystem.sNow)
		{
			// Were we stratled?
			if (pbr->bStartled)
			{
				// Yes!  The glance has satisfied us.
				pbr->UnStartle(10.0f);
			}

			// And maybe disable look around for a while.
			sCanAct = gaiSystem.sNow + gaiSystem.rnd(-40.0f, 15.0f);
//			dout << sCanAct - gaiSystem.sNow << "\n";
		}
		else
		{
			// Dont try to continue, as that will make you Exclusive.
			// Set multiplier so you will keep beating Wander.
			SetMultiplier(3.0);
		}
		

		CRating rt_speed = 0.3;
		if (pbr->bStartled)
			rt_speed = 1.0f;
		DoPointHead(rt_importance, v3Target, 0.3);
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityLookAround::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nLook about to see if anyone is there.\n");

		strcat(buffer, "\n\nREADY.\nCould use tweaking.\n");

		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif


