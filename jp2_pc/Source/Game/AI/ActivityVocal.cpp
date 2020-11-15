/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 *	Implementation of ActivityVocal.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/ActivityVocal.cpp                                            $
 * 
 * 39    9/30/98 8:00p Agrant
 * Dying vocal repairs
 * 
 * 38    9/19/98 1:43a Agrant
 * better rating
 * 
 * 37    9/17/98 9:15p Agrant
 * spend less effort vocalizing so's we can eat our player
 * 
 * 36    9/06/98 7:49p Agrant
 * added some head cock 
 * 
 * 35    9/02/98 5:24p Agrant
 * Vocalizations no longer require audio handle
 * 
 * 34    8/24/98 10:05p Agrant
 * parameter tweaks
 * 
 * 33    8/22/98 7:22p Agrant
 * use startle flag
 * 
 * 32    8/20/98 11:10p Agrant
 * only howl when howl is active
 * 
 * 31    8/14/98 11:36a Agrant
 * Howl when you are in a mood to celebrate.
 * 
 * 30    7/25/98 8:04p Agrant
 * better vocal parameters
 * 
 * 29    7/23/98 9:35p Agrant
 * open and close the mouth according to times set in text props
 * 
 **********************************************************************************************/

#include "common.hpp"

#include "ActivityVocal.hpp"
#include "AIMain.hpp"
#include "Brain.hpp"

#include "Lib/Sys/DebugConsole.hpp"
#include "Game/DesignDaemon/Player.hpp"

#include "Lib/EntityDBase/Animal.hpp"

#include <stdio.h>

#define PI 3.14159265f


//*********************************************************************************************
//
//	Class CActivityVocal implementation.
//

	CActivityVocal::CActivityVocal
	(
		char *ac_name
	) : CActivity(ac_name)
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 0.0f;
		feelRatingFeeling[eptCURIOSITY] = 0.0f;
		feelRatingFeeling[eptHUNGER]	= 0.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= 0.0f;
		feelRatingFeeling[eptPAIN]		= 0.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = false;
		sResourcesRequired[edrVOICE]	= true;

		sResourcesUsed = sResourcesRequired;

		// Default to OUCH, but MUST override in other vocals.
		evtVocalType	= evtOUCH;
		sWait			= 0.0f;
		rtMaxMouth		= 0.5f;
		rtMouthSpeed	= 1.0f;
		rHeadCock		= 0.0f;

		// Allow vocals at somewhat random points.
		sCanAct = gaiSystem.sNow + gaiSystem.rnd(0.0f, 10.0f);
	};


	//*********************************************************************************
	void CActivityVocal::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		CBrain* pbr = gaiSystem.pbrGetCurrentBrain();
		Assert(pbr);


#if VER_TEST
		char buffer[256];
		sprintf(buffer, "Vocalizing %s\n", acName);

		DebugSay(buffer);
#endif

		TryToContinue();

		// Is the brain already playing a sample?
		if (pbr->bIsVocalizing())
		{
			// Yes!
			Assert(pbr->iVocalizingActivity >= 0);

			// Is it ours?
			if (pbr->sapactActivities[pbr->iVocalizingActivity] == this)
			{
			//	dout << "Now: " << gaiSystem.sNow << ", Then: " << pbr->sSampleStart << ", Duration: " << pbr->sSampleLength << "\n";

				// Yes!  Is it over?
				if (gaiSystem.sNow > pbr->sSampleLength + pbr->sSampleStart)
				{
					// Yes, it's over.
					DoStopVocalizing();

					// Close the mouth.
					DoOpenMouth(rt_importance, 0.0f, rtMouthSpeed);

					DontContinue();

					sCanAct = gaiSystem.sNow + gaiSystem.rnd(0.5f * sWait, 1.5f* sWait);

					// If startled, completing this vocal clears it.
					if (pbrBRAIN->bStartled)
					{
						pbrBRAIN->UnStartle(15.0f);
					}

				}
				else
				{
					// No.  It's still going.

					// Should we have our mouth shut?
					if (gaiSystem.sNow < pbr->sOpenMouth || gaiSystem.sNow > pbr->sCloseMouth)
					{
						// Yes!
						rtMouthTarget = 0;
					}
					else
					{
						// It's ours, and it's ongoing.  Move it to new head position.
						if (sNextMouthPosition < gaiSystem.sNow)
						{
							// Pick a new mouth target!
							rtMouthTarget = gaiSystem.rnd(rtMaxMouth * 0.5f, rtMaxMouth);

							sNextMouthPosition = gaiSystem.sNow + gaiSystem.rnd(0.2f, 0.5f);
						}
					}

					// HACK HACK HACK

					// Open the mouth.
					DoOpenMouth(rt_importance, rtMouthTarget, rtMouthSpeed);
					DoCockHead(rt_importance, rHeadCock, 1.0f);

				}
					
				// Our work here is done.  Return to avoid playing our sample again.
				return;
			}
			else
			{
				// Sample is not ours.  Kill it.
				DoStopVocalizing();
			}
		}

		// Start the vocal.
		if (!bDoVocal(evtVocalType))
			DontContinue();

		// Open the mouth.
		sNextMouthPosition = gaiSystem.sNow;
		rtMouthTarget = rtMaxMouth;
//		DoOpenMouth(rt_importance, rtMaxMouth, rtMouthSpeed);

	}

	//*************************************************************************************
	CRating CActivityVocal::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	//******************************
	{
		CBrain* pbr = gaiSystem.pbrGetCurrentBrain();
		Assert(pbr);

		if (!bIsActive() || pbr->paniOwner->bDead())
			return 0.0f;

		float f_ret = CActivity::rtRate(feel,pinf);

		// If we are playing, rate high.
		if (bIsActive() && pbr->bIsVocalizing() && pbr->sapactActivities[pbr->iVocalizingActivity] == this)
		{
			// If we are already making this sound, it ought to be more likely to continue it.
			f_ret += rtThreshold + 0.001f;
			if (f_ret > 1.0f)
				f_ret = 1.0f;
		}

		return f_ret;
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityVocal::iGetDescription(char *buffer, int i_buffer_length)
	{
		CActivity::iGetDescription(buffer, i_buffer_length);
		
		strcat(buffer, "\n\nMake a sound.\n");

		strcat(buffer, "\n\nREADY.\n");
		Assert(strlen(buffer) < i_buffer_length);
		return strlen(buffer);
	}
#endif




//*********************************************************************************************
//
//	Class CActivityVocalOuch implementation.
//

	CActivityVocalOuch::CActivityVocalOuch()
		: CActivityVocal("Vocal Ouch")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 0.0f;
		feelRatingFeeling[eptCURIOSITY] = 0.0f;
		feelRatingFeeling[eptHUNGER]	= 0.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= 0.0f;
		feelRatingFeeling[eptPAIN]		= 5.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;


		evtVocalType	= evtOUCH;
		sWait			= 0.01f;
		rtMaxMouth		= 0.85f;

		sResourcesRequired[edrHEAD] = true;
		sResourcesRequired[edrTAIL] = false;
		sResourcesRequired[edrFORELEGS] = false;
		sResourcesRequired[edrHINDLEGS] = false;
		sResourcesRequired[edrVOICE] = true;

		// The activity two resources, and uses all resources.
		// This way, we get a stun effect when you hurt him bad enough to 
		// make him squeal.
		sResourcesUsed[edrHEAD] = true;
		sResourcesUsed[edrTAIL] = true;
		sResourcesUsed[edrFORELEGS] = true;
		sResourcesUsed[edrHINDLEGS] = true;
		sResourcesUsed[edrVOICE] = true;

		// Ouch can override anything.
		sFlags[eafEXCLUSIVE] = true;
	};


	//*************************************************************************************
	CRating CActivityVocalOuch::rtRate(const CFeeling& feel, CInfluence* pinf)
	{
		CRating rt = CActivity::rtRate(feel, pinf);

		if (pbrBRAIN->bStartled)
			rt *= 2;

		return rt;
	}

	//*********************************************************************************
	void CActivityVocalOuch::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		if (!bWasChosen())
			rHeadCock = gaiSystem.rnd(-PI * 0.125, PI * 0.125);

		// Do the normal thing.
		CActivityVocal::Act(rt_importance, pinf);

		// Create a good cringe vector.
		CVector3<> v3 = CVector3<>(0,1,0) * gaiSystem.paniGetCurrentAnimal()->r3Rot();

		// Perturb it to one side, and slightly downwards.
		v3 *= CRotate3<>(CDir3<>(1,0,0), 3.14159 * 3.0f / 32.0f);
		v3 *= CRotate3<>(CDir3<>(0,0,1), 3.14159 * 3.0f / 32.0f);

		// And cringe..
		DoPointHead(rt_importance, v3, 0.8f);
	}



//*********************************************************************************************
//
//	Class CActivityVocalHowl implementation.
//

	CActivityVocalHowl::CActivityVocalHowl() : CActivityVocal("Vocal Howl")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -3.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 0.0f;
		feelRatingFeeling[eptCURIOSITY] = 1.0f;
		feelRatingFeeling[eptHUNGER]	= 2.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= -3.0f;
		feelRatingFeeling[eptPAIN]		= 0.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;

		sResourcesRequired[edrHEAD]		= true;
		sResourcesRequired[edrTAIL]		= true;
		sResourcesRequired[edrFORELEGS] = true;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE]	= true;

		sResourcesUsed = sResourcesRequired;

		evtVocalType = evtHOWL;
		sWait			= 8.5f;
		rtMaxMouth		= 0.85f;
		rtMouthSpeed	= 0.3f;
	};

	//*************************************************************************************
	CRating CActivityVocalHowl::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	//******************************
	{
		CRating rt = 0.0f;
		rt = CActivityVocal::rtRate(feel, pinf);

		if (bIsActive() && pbrBRAIN->bTriumph)
		{
			rt.fVal += 0.5;
		}

		return rt;
	}

	//*********************************************************************************
	void CActivityVocalHowl::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		if (!bWasChosen())
		{
			sNextHeadPoint = 0.0f;
		}

		// Do the normal thing.
		CActivityVocal::Act(rt_importance, pinf);

		// Is anyone watching?
		CPresence3<> pr3_player = gpPlayer->pr3Presence();
		CVector2<> v2_to_target = pr3_player.v3Pos - gaiSystem.v3GetBodyLocation();
		CVector2<> v2_target_facing = CVector3<>(0,1,0) * pr3_player.r3Rot;

		// Is the player looking?
		bool b_audience = !(v2_to_target * v2_target_facing > 0);

		if (b_audience)
		{
			// We have an audience.  Stop so they can admire you.
			sResourcesUsed = sResourcesRequired;

			if (sNextHeadPoint < gaiSystem.sNow)
			{
				v3HeadPoint.tZ = 1.0f;
	#define rHEAD_TRAVEL 0.1f
				v3HeadPoint.tX = gaiSystem.rnd(-rHEAD_TRAVEL, rHEAD_TRAVEL);
				v3HeadPoint.tY = gaiSystem.rnd(-rHEAD_TRAVEL, rHEAD_TRAVEL);

				rHeadCock = gaiSystem.rnd(-PI * 0.25f, PI * 0.25f);

				sNextHeadPoint = gaiSystem.sNow + 0.5;
			}

			// And lift your head up high.
			DoPointHead(rt_importance, v3HeadPoint, 0.3);

			DebugSay("Howl majestic!\n");
		}
		else
		{
			// Allow dino to move around.  Do not move the head.
			sResourcesUsed[edrHEAD]		= false;
			sResourcesUsed[edrTAIL]		= false;
			sResourcesUsed[edrFORELEGS] = false;
			sResourcesUsed[edrHINDLEGS] = false;
			sResourcesUsed[edrVOICE]	= true;

			DebugSay("Howl sound ONLY!\n");
		}

		// Is it over?
		CBrain* pbr = pbrBRAIN;
		if (gaiSystem.sNow > pbr->sSampleLength + pbr->sSampleStart)
		{
			// Yes, it's over.
			// Clear the triumph flag now that we are howling.
			pbr->bTriumph = false;
		}
	}



//*********************************************************************************************
//
//	Class CActivityVocalSnarl implementation.
//

	CActivityVocalSnarl::CActivityVocalSnarl() : CActivityVocal("Vocal Snarl")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 0.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 3.0f;
		feelRatingFeeling[eptCURIOSITY] = 0.0f;
		feelRatingFeeling[eptHUNGER]	= -5.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= -2.0f;
		feelRatingFeeling[eptPAIN]		= 0.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;

		evtVocalType = evtSNARL;
		sWait			= 12.0f;
		rtMaxMouth		= 0.35f;

	};

	//*********************************************************************************
	void CActivityVocalSnarl::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		if (!bWasChosen())
			rHeadCock = gaiSystem.rnd(-PI * 0.25f, PI * 0.25f);

		// Do the normal thing.
		CActivityVocal::Act(rt_importance, pinf);

		// And look at your foe.
		DoPointHead(rt_importance, pinf->v3ToTarget, 0.3);
	}


//*********************************************************************************************
//
//	Class CActivityVocalHelp implementation.
//

	CActivityVocalHelp::CActivityVocalHelp(): CActivityVocal("Vocal Help")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= 1.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= 0.0f;
		feelRatingFeeling[eptCURIOSITY] = 2.0f;
		feelRatingFeeling[eptHUNGER]	= 0.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= 1.0f;
		feelRatingFeeling[eptPAIN]		= 0.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;

		evtVocalType = evtHELP;
		sWait			= 12.0f;
		rtMaxMouth		= 0.65f;
	};

	//*************************************************************************************
	CRating CActivityVocalHelp::rtRate(const CFeeling& feel, CInfluence* pinf)
	{
		CRating rt = CActivity::rtRate(feel, pinf);

		if (pbrBRAIN->bStartled)
			rt *= 2;

		return rt;	
	}

	//*********************************************************************************
	void CActivityVocalHelp::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		if (!bWasChosen())
			rHeadCock = gaiSystem.rnd(-PI * 0.25f, PI * 0.25f);

		// Do the normal thing.
		CActivityVocal::Act(rt_importance, pinf);

/*
		// Create a good cringe vector.
		CVector3<> v3 = CVector3<>(0,1,0) * gaiSystem.paniGetCurrentAnimal()->r3Rot();

		// Perturb it to one side, and slightly downwards.
		v3 *= CRotate3<>(CDir3<>(1,0,0), 3.14159 * 3.0f / 32.0f);
		v3 *= CRotate3<>(CDir3<>(0,0,1), 3.14159 * 3.0f / 32.0f);

		// And cringe..
		DoPointHead(rt_importance, v3, 0.8f);
  */
	}


//*********************************************************************************************
//
//	Class CActivityVocalCroon implementation.
//

	
	CActivityVocalCroon::CActivityVocalCroon() : CActivityVocal("Vocal Croon")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFEAR]		= -3.0f;
		feelRatingFeeling[eptLOVE]		= 0.0f;
		feelRatingFeeling[eptANGER]		= -2.0f;
		feelRatingFeeling[eptCURIOSITY] = 0.0f;
		feelRatingFeeling[eptHUNGER]	= 5.0f;
		feelRatingFeeling[eptTHIRST]	= 0.0f;
		feelRatingFeeling[eptFATIGUE]	= -8.0f;
		feelRatingFeeling[eptPAIN]		= 0.0f;
		feelRatingFeeling[eptSOLIDITY]  = 0.0f;

		evtVocalType = evtCROON;
		sWait			= 8.5f;
		rtMaxMouth		= 0.25f;
		rtMouthSpeed	= 0.75f;

	};

	//*********************************************************************************
	void CActivityVocalCroon::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		// Do the normal thing.
		CActivityVocal::Act(rt_importance, pinf);

		// And look at your foe.
		DoPointHead(rt_importance, pinf->v3ToTarget, 0.3);
	}


//*********************************************************************************************
//
//	Class CActivityVocalDie implementation.
//

	
	CActivityVocalDie::CActivityVocalDie() : CActivityVocal("Vocal Dying")
	{
		// Set up the rating feeling.
		feelRatingFeeling[eptFATIGUE]	= 1.0f;

		evtVocalType = evtDIE;
		sWait			= 2.5f;
		rtMaxMouth		= 0.35f;
		rtMouthSpeed	= 0.75f;

	};

	//*********************************************************************************
	void CActivityVocalDie::Act
	(
		CRating			rt_importance,
		CInfluence*		pinf
	)
	{
		if (!bWasChosen())
			rHeadCock = gaiSystem.rnd(-PI * 0.25f, PI * 0.25f);

		// Do the normal thing.
		CActivityVocal::Act(rt_importance, pinf);
	}

	//*********************************************************************************
	CRating CActivityVocalDie::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	{
		// if we are not dead, do nothing.
		if (!bIsActive() || sCanAct > gaiSystem.sNow)
			return 0;

		if (gaiSystem.paniGetCurrentAnimal()->bDead())
		{
			CRating rt = rtRateWithRatingFeeling(feel);
			rt *= fMultiplier;
			return rt;
		}
		else
			return 0;
	}
