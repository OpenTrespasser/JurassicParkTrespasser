/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of Activity.hpp and ActivityCompound.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/Activity.cpp                                                 $
 * 
 * 85    10/09/98 12:29a Agrant
 * White knuckle ride to the finish-  hack the head posits for large bipeds
 * 
 * 84    9/23/98 11:09p Agrant
 * dinos make dino vocals instead of positioned effect
 * 
 * 83    9/20/98 7:24p Agrant
 * use the node me flag
 * 
 * 82    9/19/98 1:44a Agrant
 * blind head move for cooler eating
 * 
 * 81    9/13/98 8:05p Agrant
 * Allow some slop when body pathfinding
 * 
 * 80    9/10/98 4:13p Agrant
 * completely new activity save function
 * 
 * 79    9/08/98 4:42p Agrant
 * point head along path when taking complex path
 * 
 * 78    9/05/98 8:28p Agrant
 * allow AI to set body and tail damage
 * 
 * 77    9/03/98 4:15p Agrant
 * Okay, now really a dout for Brady.
 * 
 * 76    9/03/98 12:38p Agrant
 * dout for identifying animal samples (commented out)
 * 
 * 75    9/02/98 11:19p Agrant
 * When failing to head pathfind, go ahead and move your body as best you can.
 * 
 * 74    9/02/98 5:25p Agrant
 * Activities allocated from heap
 * Notify nearby dinos about vocals
 * 
 * 73    8/26/98 7:12p Agrant
 * setting head damage, better head pathfinding
 * 
 * 72    8/26/98 11:15a Agrant
 * temp flag rework
 * 
 * 71    8/25/98 10:52a Rvande
 * Checking in on behalf of Andrew S.  Fixing some for ( int i... type problems.
 * 
 * 70    8/16/98 3:21p Agrant
 * Much tweaking to simplify activity balancing
 * 
 * 69    8/13/98 6:56p Agrant
 * code to allow dinos to ignore influences that are unattainable
 * 
 * 68    8/11/98 2:23p Agrant
 * change flee distance to avoid goofy bug
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/Audio/AudioDaemon.hpp"
#include "Lib/Audio/Audio.hpp"

//#include "Lib/W95/WinAlias.hpp"
#include <stdio.h>

#include "Lib/EntityDBase/Instance.hpp"

#include "Activity.hpp"
//#include "ActivityCompound.hpp"

#include "AIMain.hpp"
#include "AIInfo.hpp"
#include "Brain.hpp"
#include "MentalState.hpp"
#include "Synthesizer.hpp"
#include "Lib\EntityDBase\Animal.hpp"

#include "Lib/EntityDBase/MessageTypes/MsgAudio.hpp"
#include "Lib/Audio/Sounddefs.hpp"
#include "Lib/Audio/AudioLoader.hpp"

#include "Lib/Loader/SaveFile.hpp"
#include "Lib/Sys/Profile.hpp"

#include "PathAStar.hpp"

#include <string>

// The target location distance when fleeing.
#define rFLEE_DISTANCE				50.0

class CAudioDaemon;
extern CAudioDaemon* padAudioDaemon;

extern CProfileStat psPathfinder;


//*********************************************************************************************
//
//	Class CActivity implementation.
//

	//*************************************************************************************
	CActivity::CActivity(const char *ac_name) : rtThreshold(0.0001f), fMultiplier(1.0), pinfTarget(0),
		sResourcesUsed(0,1), sResourcesRequired(0,1), sCanAct(0.0f)
	{

		// Make sure that the name is not too long.
		char ac_buffer[iMAX_ACTIVITY_NAME_LENGTH];
		if (strlen(ac_name) >= iMAX_ACTIVITY_NAME_LENGTH)
		{
			for (int i = 0; i < iMAX_ACTIVITY_NAME_LENGTH - 1; ++i)
			{
				ac_buffer[i] = ac_name[i];
			}
			
			ac_buffer[iMAX_ACTIVITY_NAME_LENGTH-1] = '\0';
			ac_name = ac_buffer;
		}

		strcpy(acName, ac_name);

		// By default, the activity requires all resources, and uses all resources.
		sResourcesRequired[edrHEAD] = true;
		sResourcesRequired[edrTAIL] = true;
		sResourcesRequired[edrFORELEGS] = true;
		sResourcesRequired[edrHINDLEGS] = true;
		sResourcesRequired[edrVOICE] = true;

		sResourcesUsed = sResourcesRequired;

		SetMultiplier(1.0f);
	};


	//*****************************************************************************************
	void* CActivity::operator new(uint  u_size_type)
	{
#ifdef LOG_MEM

		// if we are logging memory allocate 8 bytes more that required, the first DWORD  is
		// a check value so we can verify that a freed block was allocated by this operator.
		// The second DWORD is the size of the block so we can log memory usage.
		void* pv = new (fhAI) uint8[u_size_type+8];
		*((uint32*)pv)	= MEM_LOG_INSTANCE_CHECKWORD;
		*(((uint32*)pv)+1) = u_size_type;

		MEMLOG_ADD_COUNTER(emlAI, u_size_type);

		// return 8 bytes after the allocated address to the caller.
		return (void*) (((uint8*)pv)+8);
#else
		return new (fhAI) uint8[u_size_type];
#endif
	}


#if VER_TEST
	//*************************************************************************************
	int CActivity::iGetDescription(char *buffer, int i_max_len)
	{
		char local_buffer[256];

		sprintf(buffer, "%s", acName);

		strcat(buffer, "\n\nThreshold:\t");

		sprintf(local_buffer, "%f\nMultiplier:\t%f", rtThreshold.fVal, fMultiplier);
		strcat(buffer, local_buffer);

		// Current rating feeling.
		sprintf(local_buffer, "\n\nRating feeling:\n");
		strcat(buffer, local_buffer);

		int i;
		for (i = 0; i < eptEND; ++i)
		{
			sprintf(local_buffer, "%s\t\t%f\n", CFeeling::pcRatingNames[i], feelRatingFeeling[i]);
			strcat(buffer, local_buffer);
		}

		// Dino resource report.
		strcat(buffer, "\n\nResource\tNeeded\tUsed\n");

		static char* apc_resources[] = { "\tHead", "\tTail", "\tFLegs", "\tRLegs", "\tVoice" };

		for (i = 0; i < edrEND; ++i)
		{
			char *pc_needed = 0;
			char *pc_used	= 0;

			if (sResourcesRequired[EDinoResource(i)])
				pc_needed = "Yes";
			else
				pc_needed = "No";

			if (sResourcesUsed[EDinoResource(i)])
				pc_used = "Yes";
			else
				pc_used = "No";

			sprintf(local_buffer, "%s\t%s\t%s\n", apc_resources[i], pc_needed, pc_used);
			strcat(buffer, local_buffer);
		}
		return strlen(buffer);
	
	}
#endif


	//*************************************************************************************
	void CActivity::ResetTempFlags
	(
	)
	{
		sFlags[eafCONTINUE]				= sFlags[eafCONTINUENEXTTIME];
		sFlags[eafCONTINUENEXTTIME]		= false;
		sFlags[eafWASCHOSENLASTTIME]	= sFlags[eafWASCHOSENTHISTIME];
		sFlags[eafWASCHOSENTHISTIME]	= false;
		fMultiplier = fNextMultiplier;
		fNextMultiplier = 1.0f;
	}

	//*************************************************************************************
	CRating CActivity::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	{
		// Rate self zero if inactive or can't go yet.
		if (!bIsActive() || sCanAct > gaiSystem.sNow || gaiSystem.paniGetCurrentAnimal()->bDead())
			return 0;
		

		CRating rt = rtRateWithRatingFeeling(feel);
		rt *= fMultiplier;

#if VER_TEST
		if (rt > 0.0f)
		{
			if (pinf)
			{
				char ac[128];

				sprintf(ac, "%s:[%s]\t%.2f\n", acName, pinf->pinsTarget->strGetInstanceName(), 1024 * rt.fVal);

				gaiSystem.pbrGetCurrentBrain()->MaybeDebugSay(ac);
			}
			else
			{
				char ac[128];
				sprintf(ac, "%s:[NA]\t%.2f\n", acName, 1024 * rt.fVal);
				gaiSystem.pbrGetCurrentBrain()->MaybeDebugSay(ac);
			}
		}
#endif


		return rt;
	}

	//*************************************************************************************
	CRating CActivity::rtRateList
	(
		const CFeeling&	feel,		// an emotional state
		CInfluenceList*	pinfl		
	)
	{
		CRating rt_max = 0;
		CInfluenceList::iterator pinf;

		// Analyze each influence in turn with all sub-brains.
		for (pinf = pinfl->begin(); pinf != pinfl->end(); pinf++)
		{
			// Speed and convenience hack-
			// Only rate if we have a know AI Type.
			if ((*pinf).setFlags[eifIGNORE] || (*pinf).pinsTarget->paiiGetAIInfo()->eaiRefType == eaiUNKNOWN)
				continue;

			// SLOW SLOW SLOW
			// We'll do this same calculation for every activity!
			// We can pre-calc them instead.
			CFeeling	feel_rater = (*pinf).feelAboutTarget * feel;

			// Casting away const, because it isn't supposed to be const.
			CRating rt = rtRate(feel_rater,(CInfluence*)&(*pinf));

			if (rt > rt_max)
			{
				rt_max = rt;
				pinfTarget = &(*pinf);
			}
		}
		
		return rt_max;
	}

	//*************************************************************************************
	void CActivity::Register
	(
		CRating			rt_importance,	// The importance of the action.
		CInfluence*		pinf			// The direct object of the action.
	)
	{
//		AlwaysAssert(!gaiSystem.paniGetCurrentAnimal()->bDead());
		gaiSystem.psynGetCurrentSynthesizer()->Register
		(
			rt_importance,
			this,
			pinf
		);
	}

	//*************************************************************************************
	void CActivity::RegisterList
	(
		const CRating rt,			// The rating at which to register.
		CInfluenceList*	pinfl		// All influences known.
	)
	{
		if (!bIsActive() || gaiSystem.paniGetCurrentAnimal()->bDead())
			return;

		// Make sure the influence is in the list.
		Assert(pinfTarget);
		Assert(pinfl->find(*pinfTarget) != pinfl->end());

		Register(rt, (CInfluence*)pinfTarget);
	}

	//*************************************************************************************
	CRating CActivity::rtRateAndRegister
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	{
		CRating rt_importance = rtRate(feel, pinf);

    
		if (rt_importance > rtThreshold)
			Register(rt_importance, pinf);
		return rt_importance;
	}

	//*************************************************************************************
	CRating CActivity::rtRateAndRegisterList
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluenceList*	pinfl		// All influences known.
	)
	{
		if (!bIsActive() || gaiSystem.paniGetCurrentAnimal()->bDead())
			return 0.0f;

		CRating rt = rtRateList(feel, pinfl);

		if (rt > rtThreshold)
			RegisterList(rt, pinfl);

		return rt;
	}

	
	//*************************************************************************************
	CRating CActivity::rtRateAndRegisterAll
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluenceList*	pinfl		// All influences known.
	)
	{
		// Default behavior here is to rate and register each influence seperately.

		CRating rt_max = 0;
		CInfluenceList::iterator pinf;

		// Analyze each influence in turn with all sub-brains.
		for (pinf = pinfl->begin(); pinf != pinfl->end(); pinf++)
		{
			// Casting away const, because it isn't supposed to be const.
			CRating rt = rtRateAndRegister(feel,(CInfluence*)&(*pinf));

			if (rt > rt_max)
			{
				rt_max = rt;
				pinfTarget = &(*pinf);
			}
		}
		
		return rt_max;
	}

		
	//
	//  Functions common to all activities (not virtual)
	//
		
		
		
	//*********************************************************************************
	void CActivity::RegisterHerdHint
	(
		const CRating		rt_importance,	// The importance of the hint.
		const CVector2<>&	v2_direction	// The direction of the hint.
	)
	{
		gaiSystem.psynGetCurrentSynthesizer()->RegisterHerdHint(rt_importance, v2_direction);
	}

	//*********************************************************************************
	void CActivity::RegisterFleeHint
	(
		const CRating		rt_importance,	// The importance of the hint.
		const CVector2<>&	v2_direction	// The direction of the hint.
	)
	{
		gaiSystem.psynGetCurrentSynthesizer()->RegisterFleeHint(rt_importance, v2_direction);
	}
	
	//*********************************************************************************
	void CActivity::DoSetHeadDamage
	(
		CRating			rt_importance,	// How important is this command?
		CRating			rt_damage		// How much damage?
	)
	{
		CBoundaryBox* pbb = gaiSystem.paniGetCurrentAnimal()->apbbBoundaryBoxes[ebbHEAD];

		AlwaysAssert(pbb);
		pbb->fDamageMultiplier = rt_damage;
	}

	//*********************************************************************************
	void CActivity::DoSetBodyDamage
	(
		CRating			rt_importance,	// How important is this command?
		CRating			rt_damage		// How much damage?
	)
	{
		CBoundaryBox* pbb = gaiSystem.paniGetCurrentAnimal()->apbbBoundaryBoxes[ebbBODY];

		AlwaysAssert(pbb);
		pbb->fDamageMultiplier = rt_damage;
	}

	//*********************************************************************************
	void CActivity::DoSetTailDamage
	(
		CRating			rt_importance,	// How important is this command?
		CRating			rt_damage		// How much damage?
	)
	{
		CBoundaryBox* pbb = gaiSystem.paniGetCurrentAnimal()->apbbBoundaryBoxes[ebbTAIL];

		AlwaysAssert(pbb);
		pbb->fDamageMultiplier = rt_damage;
	}

	//*********************************************************************************
	void CActivity::DoMoveTo
	(
		CRating			rt_importance,	// How important is this command?
		CInfluence*		pinf_target,	// Which influence?
		CRating			rt_speed		// How quickly should I move?
	)
	{
		Assert(pinf_target);

		// Make sure the node our target if possible.
		pinf_target->setNodeFlags[ensfNODE_ME] = true;

		DoMoveToLocation
		(
			rt_importance, 
			pinf_target->v3Location,
			rt_speed
		);			
	}


	//*********************************************************************************
	//
	void CActivity::DoMoveToLocationBlind
	(
		const CRating		rt_importance,	// The importance of the move command.
		const CVector2<>&	v2_location,	// Where to go.
		const CRating		rt_speed		// How quicly to move there.
	)
	{
		// Travel the first leg of the path, please.
		CBrain* pbr = gaiSystem.pbrGetCurrentBrain();
		Assert(pbr);
		pbr->TellPhysicsMoveTo
		(
			rt_importance,
			v2_location,
			rt_speed
		);
	}

	//*************************************************************************************
	void CActivity::DoMoveToLocation
	(
		const CRating		rt_importance,	// The importance of the move command.
		const CVector2<>&	v2_location,	// Where to go.
		const CRating		rt_speed		// How quicly to move there.
	)
	{
		CCycleTimer ctmr;

		CBrain *pbr = gaiSystem.pbrGetCurrentBrain();

		// Set the LastDirection variable.
		pbr->pmsState->v2LastDirection = v2_location - pbr->v2GetLocation();

		// Get a pathfinder to avoid simple obstacles.
		CPathAStar		pathas(pbr);
		CPathfinder*	ppathf = &pathas;
		CPath			path;
	
		// Find the path.
		path.push_back(gaiSystem.v2GetBodyLocation());
		path.push_back(v2_location);

		Assert(pbr->rMoveableMass <= 1.0f);
		Assert(pbr->rMoveableMass >= 0.0f);

		if (ppathf->bFindPath(&path, pbr->rMoveableMass, pbr->rHeadReach))
		{
			pbr->bLastPathSucceeded = true;
			pbr->sLastSuccessfulPath = gaiSystem.sNow;
		}
		else
		{
			// Pathfind failed.
			pbr->bLastPathSucceeded = false;
		}

		// Travel the first leg of the path, please.
		// If we fail, the first leg is our best guess.
		pbr->TellPhysicsMoveTo
		(
			rt_importance,
			path.v2GetFirstDestination(),
			rt_speed
		);

		if (!pbr->bLastPathSucceeded || path.size() > 2)
		{
			// Complicated path.  Point head at intermediate location.
			DoPointHead(rt_importance, path.v2GetFirstDestination() - gaiSystem.v2GetBodyLocation(), rt_speed);
		}


		psPathfinder.Add(ctmr());
	}

	//*********************************************************************************
	void CActivity::DoMoveInDirection
	(
		const CRating		rt_importance,	// The importance of the move command.
		const CVector2<>&	v2_direction,	// Which direction to move.
		const CRating		rt_speed		// How quicly to move there.
	)
	{
		CVector2<> v2_dir = v2_direction;
		if (!v2_dir.bIsZero())
			v2_dir.Normalise(rFLEE_DISTANCE);
		DoMoveToLocation
		(
			rt_importance,
			v2_dir + gaiSystem.v2GetBodyLocation(),
			rt_speed
		);
	}

	//*********************************************************************************
	void CActivity::DoMoveHeadTo
	(
		const CRating		rt_importance,	// The importance of the move command.
		const CVector3<>&	v3_location,	// Target location of head.
		const CRating		rt_speed		// How quicly to move there.
	)
	{
		CCycleTimer ctmr;

		CBrain *pbr = gaiSystem.pbrGetCurrentBrain();
		bool b_path_ok = false;

		// Set the LastDirection variable.
		CVector2<> v2_location = v3_location;
		pbr->pmsState->v2LastDirection = v2_location - pbr->v2GetLocation();

		//  Pathfinding target is a little short of head location to allow for head reaching into solid foes.
		CVector2<> v2_short_location = pbr->pmsState->v2LastDirection;

		// Create the path.
		CPath			path;
		path.push_back(gaiSystem.v2GetBodyLocation());
		path.push_back(v2_location);

		if (v2_short_location.tLenSqr() < pbr->rHeadReach * pbr->rHeadReach)
		{
			// We're already close enough.
			b_path_ok = true;
		}
		else
		{
			v2_short_location.Normalise(pbr->rHeadReach);
			v2_short_location = v2_location - v2_short_location;

			// Get a pathfinder to avoid simple obstacles.
			CPathAStar		pathas(pbr);
			CPathfinder*	ppathf = &pathas;

			Assert(pbr->rMoveableMass <= 1.0f);
			Assert(pbr->rMoveableMass >= 0.0f);

			// See if we can make it work.
			b_path_ok = ppathf->bFindPath(&path, pbr->rMoveableMass, 0.0f);
		}

		if (b_path_ok)
		{
			// we found a path...

			// Is there only one segment?
			if (path.size() == 2)
			{
				// Yes!  Go ahead and move your head there!
				pbr->TellPhysicsMoveHeadTo
				(
					rt_importance,
					v3_location,
					rt_speed
				);
			}
			else
			{
				// No.  Travel the first leg of the path, please.
				pbr->TellPhysicsMoveTo
				(
					rt_importance,
					path.v2GetFirstDestination(),
					rt_speed
				);
		
				DoPointHead(rt_importance, v3_location - gaiSystem.v3GetBodyLocation(), rt_speed);


			}

			pbr->bLastPathSucceeded = true;
			pbr->sLastSuccessfulPath = gaiSystem.sNow;
		}
		else
		{
			// Pathfind failed.
			pbr->bLastPathSucceeded = false;

			pbr->TellPhysicsMoveTo
			(
				rt_importance,
				path.v2GetFirstDestination(),
				rt_speed
			);
		}
		psPathfinder.Add(ctmr());
	}

	//*********************************************************************************
	void CActivity::DoMoveHeadToBlind
	(
		const CRating		rt_importance,	// The importance of the move command.
		const CVector3<>&	v3_location,	// Target location of head.
		const CRating		rt_speed		// How quicly to move there.
	)
	{
		pbrBRAIN->TellPhysicsMoveHeadTo
		(
			rt_importance,
			v3_location,
			rt_speed
		);
	}

	//*********************************************************************************
	void CActivity::DoOpenMouth
	(
		const CRating		rt_importance,	// The importance of the move command.
		const CRating		rt_open_amount,	// Target location of head.
		const CRating		rt_speed		// How quicly to move there.
	)
	{
		CBrain *pbr = gaiSystem.pbrGetCurrentBrain();
		pbr->TellPhysicsOpenMouth
		(
			rt_importance,
			rt_open_amount,
			rt_speed
		);
	}

	//*********************************************************************************
	void CActivity::DoStartVocalizing
	(
		const SVocal& sv
	)
	{
		// Okay, here begins some hacking.
		bool b_played_sound = false;
		SSoundEffectResult ser;

		if (bQuerySoundEffect(sv.sndhnd, &ser))
		{

			CAnimal* pani = gaiSystem.paniGetCurrentAnimal();
			CBoundaryBox* pbb = pani->apbbBoundaryBoxes[(int)ebbHEAD];
			CInstance* pins_source = pbb ? pbb : (CInstance*)pani;

			// Send a message with my animal as the sender.
			CMessageAudio ma_voice
			(
				sv.sndhnd,								// Which sound to play
				eamDINO_VOCAL_EFFECT,				// Kind of effect
//				eamPOSITIONED_EFFECT,			// To avoid mysterious audio crash
				(CEntity*)padAudioDaemon,			// Recipient
				gaiSystem.paniGetCurrentAnimal(),	// Sender
				pins_source,				// parent of sound
				ser.fMasterVolume,			// volume 0dBs
				ser.fAttenuation,			// attenuation (only for pseudo/real 3D samples)
				AU_SPATIAL3D,				// spatial type
				280.0f,						// fustrum angle (real 3D only)
				-2.00f,						// outside volume (real 3D only)
				false,						// looped
				0,							// loop count
				10000.0f					// distance before sound is stopped (pseudo/real 3D only)
			);

			b_played_sound = bImmediateSoundEffect(ma_voice, &ser);
		}

		// Did we play a sound?
		if (b_played_sound)
		{	
			// Success!  Remember that it was us who started it.
			CBrain* pbr = gaiSystem.pbrGetCurrentBrain();

			// Record the ID.
			pbr->u4AudioID = ser.u4SoundID;

			// Record the sample length.
			pbr->sSampleLength = ser.fPlayTime;
			pbr->sSampleStart = gaiSystem.sNow;
			pbr->sOpenMouth = pbr->sSampleStart + sv.sOpen;
			pbr->sCloseMouth = pbr->sSampleStart + pbr->sSampleLength - sv.sClose;

			// Which activity started the current sample.
			pbr->iVocalizingActivity = pbr->iGetActivityIndex(this);

			// And let all of the dinos within 60m know about us.
			gaiSystem.AlertAnimals(pbr->paniOwner->apbbBoundaryBoxes[ebbHEAD], 60.0f);

#if VER_TEST
			// BRADY   change this to "true" to get your printouts!
			if (false)
			{
				char buffer[32];
				sprintf(buffer, "%x\n", sv.sndhnd);

				dout << pbr->paniOwner->strGetInstanceName() << " played " << buffer;
			}
#endif
		}
		else
		{
			// Dino sound failed to happen!
			// Assert(false);
		}

	}

	//*********************************************************************************
	void CActivity::DoStopVocalizing
	(
	)
	{

		CBrain* pbr = gaiSystem.pbrGetCurrentBrain();
		
		if (pbr->u4AudioID)
		{		
			// Send the stop message.
			StopSoundEffect(pbr->u4AudioID);
		}
		
		pbr->u4AudioID = 0;
		pbr->iVocalizingActivity = -1;
	}

	//*********************************************************************************
	bool CActivity::bDoVocal(EVocalType evt)
	{
		// Since we are locking, go ahead and play an attack vocal.
		DoStopVocalizing();

		// Our sample is not playing yet.  Play our sample.
		CBrain* pbr = gaiSystem.pbrGetCurrentBrain();
		Assert(pbr->u4AudioID == 0);
		Assert(pbr->iVocalizingActivity == -1);

		// Get the sample set.
		int i_num_samples = gaiSystem.iNumVocalHandles(pbr->edtDinoType, evt);
		TVocalSet*	pvs = gaiSystem.pvsGetVocalSet(pbr->edtDinoType, evt);

//		return false;

		if (i_num_samples > 0)
		{
			// Play sample.
			DoStartVocalizing((*pvs)[gaiSystem.rnd() % i_num_samples]);
			return true;
		}
		else
		{
			return false;
		}
	}

	//*********************************************************************************
	void CActivity::DoCockHead(const CRating rt_importance, float f_angle, const CRating rt_speed)
	{
		CBrain *pbr = gaiSystem.pbrGetCurrentBrain();

		pbr->TellPhysicsCockHead
		(
			rt_importance,
			f_angle,
			rt_speed
		);
	}

	//*********************************************************************************
	void CActivity::DoPointHead(const CRating rt_importance, const CVector3<>& v3_direction, const CRating rt_speed)
	{
		CBrain *pbr = gaiSystem.pbrGetCurrentBrain();
		CVector3<> v3_safe_direction = v3_direction;

		///  HACK HACK HACK
		// Do we have a raptor?
		if (pbr->edtDinoType == edtTREX || pbr->edtDinoType == edtPARA || pbr->edtDinoType == edtALBERT)
		{
			CVector2<> v2_head = gaiSystem.v3GetHeadPoint();
			CVector2<> v2_direction = v3_direction;

			if (!v2_head.bIsZero() && !v2_direction.bIsZero())
			{
				v2_head.Normalise();
				v2_direction.Normalise();

				TReal r_dot_product = v2_head * v2_direction;

				// Is it close to 90 degrees?
	#define rZERO_DOT  0.15f
				if (r_dot_product < rZERO_DOT && r_dot_product > - rZERO_DOT)
				{
					// We are close to 90 degrees!
					v3_safe_direction.tX = - 3.0f * v2_head.tX + v2_direction.tX * 1.0f;
					v3_safe_direction.tY = - 3.0f * v2_head.tY + v2_direction.tY * 1.0f;
				}
			}
		}

		pbr->TellPhysicsPointHead
		(
			rt_importance,
			v3_safe_direction,
			rt_speed
		);
	}
		
	//*********************************************************************************
	void CActivity::DoWagTail(const CRating rt_importance, float f_amplitude, float f_frequency)
	{
		CBrain *pbr = gaiSystem.pbrGetCurrentBrain();

		pbr->TellPhysicsWagTail
		(
			rt_importance,
			f_amplitude,
			f_frequency
		);
	}

	//*********************************************************************************
	void CActivity::DoJump(const CRating rt_importance, const CRating rt_speed)
	{
		CBrain *pbr = gaiSystem.pbrGetCurrentBrain();

		pbr->TellPhysicsJump
		(
			rt_importance,
			rt_speed
		);
	}

 	//*********************************************************************************
	void CActivity::DoCrouch(const CRating rt_importance, const CRating rt_speed)
	{
		CBrain *pbr = gaiSystem.pbrGetCurrentBrain();

		pbr->TellPhysicsCrouch
		(
			rt_importance,
			rt_speed
		);
	}

	//*********************************************************************************
	void CActivity::DebugSay
	(
			const char *ac		// what to say
	)
	{
		CBrain *pbr = gaiSystem.pbrGetCurrentBrain();
		pbr->MaybeDebugSay
		(
			ac
		);
	}



	//*****************************************************************************************
	char *CActivity::pcSave(char * pc) const
	{
		Assert(rtThreshold.fVal >= 0.0f);
		Assert(fMultiplier >= 0.0f);

		pc = pcSaveT(pc, rtThreshold);
		pc = pcSaveT(pc, sFlags);
		pc = pcSaveT(pc, sCanAct);
		pc = pcSaveT(pc, sGiveUp);

		return pc;
	}

	//*****************************************************************************************
	const char *CActivity::pcLoad(const char * pc)
	{
		extern int iAnimalVersion;
		if (iAnimalVersion >= 8)
		{
			pc = pcLoadT(pc, &rtThreshold);
			pc = pcLoadT(pc, &sFlags);
			pc = pcLoadT(pc, &sCanAct);
			pc = pcLoadT(pc, &sGiveUp);
			return pc;
		}
		else
		{
			CFeeling feel_ignore;
			int i_ignore;
			CRating rt_ignore;
			float f_ignore;

			// Don't load rating feeling changes.  Please use code or text prop defaults!
	//		pc = pcLoadT(pc, &feelRatingFeeling);
			pc = pcLoadT(pc, &feel_ignore);
			pc = pcLoadT(pc, &rt_ignore);
			pc = pcLoadT(pc, &f_ignore);
			pc = pcLoadT(pc, &sFlags);


			pc = pcLoadT(pc, &i_ignore);
			pc = pcLoadT(pc, &i_ignore);

			pc = pcLoadT(pc, &sCanAct);
			pc = pcLoadT(pc, &sGiveUp);

			// Save some space for future data.
			pc += 32;

			Assert(rtThreshold.fVal >= 0.0f);
			Assert(fMultiplier >= 0.0f);

			return pc;
		}
	}



//*********************************************************************************************
//
//	Class CActivityDistance implementation.
//

	CActivityDistance::CActivityDistance(char *ac_name) : CActivity(ac_name)
	{
		rStartFalloff = 3.0f;
		rFalloffFactor = 0.50f;
	};

	CActivityDistance::CActivityDistance()
	{
		rStartFalloff = 3.0f;
		rFalloffFactor = 0.50f;
	};



	//*************************************************************************************
	CRating CActivityDistance::rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	)
	{
		// Rate self zero if inactive.
		if (!bIsActive() || sCanAct > gaiSystem.sNow || gaiSystem.paniGetCurrentAnimal()->bDead())
			return 0;

		CRating rt;

		if (pinf->rDistanceTo > rStartFalloff)
		{
			Assert(rFalloffFactor	> 0);
			Assert(rStartFalloff	> 0);

			rt = rtRateWithRatingFeeling(feel);
			
			TReal r_multiplier = 1.0f / (1.0f + rFalloffFactor * (pinf->rDistanceTo - rStartFalloff));

			rt *= r_multiplier;
		}
		else
			rt = rtRateWithRatingFeeling(feel);

#if VER_TEST
		if (rt.fVal > 0.0f)
		{
			if (pinf)
			{
				char ac[128];
				sprintf(ac, "%s:[%s]\t%.2f\n", acName, pinf->pinsTarget->strGetInstanceName(), 1024 * rt.fVal);
				gaiSystem.pbrGetCurrentBrain()->MaybeDebugSay(ac);
			}
			else
			{
				char ac[128];
				sprintf(ac, "%s:[NA]\t%.2f\n", acName, 1024 * rt.fVal);
				gaiSystem.pbrGetCurrentBrain()->MaybeDebugSay(ac);
			}
		}
#endif

		return rt;
	}

#if VER_TEST
	//*************************************************************************************
	int CActivityDistance::iGetDescription(char *buffer, int i_max_len)
	{
		char local_buffer[256];

		// Get the base description.
		CActivity::iGetDescription(buffer, i_max_len);

		// Add distance specific details.
		sprintf(local_buffer, "\nDistance attenuated-\nrFalloffFactor\t%f\nrStartFalloff\t%f\n\n", 
					rFalloffFactor, 
					rStartFalloff);
		strcat(buffer, local_buffer);

		AlwaysAssert(strlen(buffer) < i_max_len);
		return strlen(buffer);
	}
#endif