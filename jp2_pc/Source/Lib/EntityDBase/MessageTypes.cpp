/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of MessageTypes.hpp.
 *
 * To Do:
 *		Remove the mmsystem calls and header for the sound message.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes.cpp                                     $
 * 
 * 83    10/02/98 6:41p Bbell
 * Changed slide normalization value from 100 to 1000
 * 
 * 82    10/01/98 8:49p Pkeet
 * Changed max timestep to 0.1 seconds.
 * 
 * 81    10/01/98 6:49p Mlange
 * Improved collision message stats.
 * 
 * 80    10/01/98 4:05p Mlange
 * Improved move message stat reporting.
 * 
 * 79    9/25/98 12:47p Agrant
 * simulation speed multiplier, mostly for SLOMO
 * 
 * 78    9/24/98 5:06p Mlange
 * Move message processing now uses a special purpose world database query for the location
 * triggers, with greatly increased performance.
 * 
 * 77    9/21/98 4:19p Mlange
 * Fixed move message stat reporting.
 * 
 * 76    98/09/19 14:37 Speter
 * Removed CPhysicsInfo::HandleMessage for collisions; now processed in canonical manner by
 * CPhysicsSystem.
 * 
 *********************************************************************************************/

//
// Includes.
//

#include "gblinc/common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Message.hpp"
#include "MessageTypes/MsgStep.hpp"
#include "MessageTypes/MsgTrigger.hpp"
#include "MessageTypes/MsgCollision.hpp"
#include "MessageTypes/MsgMove.hpp"
#include "MessageTypes/MsgAudio.hpp"
#include "MessageTypes/MsgDelete.hpp"
#include "MessageTypes/MsgControl.hpp"
#include "MessageTypes/MsgPaint.hpp"
#include "MessageTypes/MsgSystem.hpp"
#include "MessageTypes/MsgPhysicsReq.hpp"

#include "Lib/EntityDBase/MessageLog.hpp"
#include "Lib/EntityDBase/QueueMessage.hpp"
#include "Lib/EntityDBase/FrameHeap.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Replay.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/EntityDBase/Subsystem.hpp"
#include "Lib/EntityDBase/Animate.hpp"
#include "Lib/EntityDBase/Query/QMessage.hpp"
#include "Lib/EntityDBase/Query/QSubsystem.hpp"
#include "Lib/EntityDBase/Query/QTriggers.hpp"
#include "Lib/GeomDBase/TerrainObj.hpp"

#include "Lib/Std/StringEx.hpp"
#include "Lib/Std/Hash.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Trigger/Trigger.hpp"

#include <string.h>
#include <stdio.h>

extern CProfileStat psMsgSubscribe, psMoveMsgQuery;


//*********************************************************************************************
//
// CMessage implementation.
//

	//******************************************************************************************
	void CMessage::PrintStats(CConsoleBuffer& con)
	{
		#if VER_TEST
			int i_num_registered, i_num_created;

			i_num_registered = CMessageMove::iGetNumRegisteredRecipients();
			i_num_created    = CMessageMove::iGetNumCreated();
			con.Print("Move          : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessageCollision::iGetNumRegisteredRecipients();
			i_num_created    = CMessageCollision::iGetNumCreated();
			con.Print("Collision     : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessageTrigger::iGetNumRegisteredRecipients();
			i_num_created    = CMessageTrigger::iGetNumCreated();
			con.Print("Trigger       : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessageMoveTriggerTo::iGetNumRegisteredRecipients();
			i_num_created    = CMessageMoveTriggerTo::iGetNumCreated();
			con.Print("MoveTriggerTo : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessagePickUp::iGetNumRegisteredRecipients();
			i_num_created    = CMessagePickUp::iGetNumCreated();
			con.Print("PickUp        : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessageUse::iGetNumRegisteredRecipients();
			i_num_created    = CMessageUse::iGetNumCreated();
			con.Print("Use           : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessageDeath::iGetNumRegisteredRecipients();
			i_num_created    = CMessageDeath::iGetNumCreated();
			con.Print("Death         : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessageDamage::iGetNumRegisteredRecipients();
			i_num_created    = CMessageDamage::iGetNumCreated();
			con.Print("Damage        : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessageSystem::iGetNumRegisteredRecipients();
			i_num_created    = CMessageSystem::iGetNumCreated();
			con.Print("System        : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessageStep::iGetNumRegisteredRecipients();
			i_num_created    = CMessageStep::iGetNumCreated();
			con.Print("Step          : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessagePaint::iGetNumRegisteredRecipients();
			i_num_created    = CMessagePaint::iGetNumCreated();
			con.Print("Paint         : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessagePhysicsReq::iGetNumRegisteredRecipients();
			i_num_created    = CMessagePhysicsReq::iGetNumCreated();
			con.Print("PhysicsReq    : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessageControl::iGetNumRegisteredRecipients();
			i_num_created    = CMessageControl::iGetNumCreated();
			con.Print("Control       : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);

			i_num_registered = CMessageAudio::iGetNumRegisteredRecipients();
			i_num_created    = CMessageAudio::iGetNumCreated();
			con.Print("Audio         : %.3d * %.3d = %d\n", i_num_registered, i_num_created, i_num_registered * i_num_created);
		#endif
	}


//
//	Any message that is required to be saved in the reply file must overload the Send and
//	ExtractReplayData members because the default implmentation in the base class does
//	nothing
//

//*********************************************************************************************
//
// CMessageStep implementation.
//

	// Static variables
	SRecipientsInfo CMessageStep::riInfo;

	TSec CMessageStep::sStaticStep		= 0.001f;
	TSec CMessageStep::sStaticTotal		= 0.0f;
	TSec CMessageStep::sElapsedRealTime	= 0.0f;
	TSec CMessageStep::sMultiplier		= 1.0f;
	uint32 CMessageStep::u4Frame        = 0;

	static const TSec sSTEP_MAX = 0.1f;	// Maximum time step.

	//*****************************************************************************************
	CMessageStep::CMessageStep(TSec s_step)
		: CMessage(&riInfo.rcRecipients)
	{
		static CTimer tmrStep;				// The simulation timer.

		// The first step time will probably be very large, since it's the time elapsed since
		// creation.  However, we clamp the step value, so we'll be all right.

		// Find the real elapsed time.
		TSec s_elapsed = tmrStep.sElapsed();

		// If no step value given, use real-time value.
		if (s_step < 0.0f)
			s_step = s_elapsed;

		// Clever detection of reset.
		if (sStaticTotal == 0.0f)
			u4Frame = 0;

		sElapsedRealTime	+=	s_elapsed;					// The unclamped time.
		sStep				=	Min(s_step, sSTEP_MAX) * sMultiplier;
		sStaticStep			=	sStep;
		sStaticTotal		+=	sStep;
		sTotal				=	sStaticTotal;

		u4Frame++;
	}


	//*****************************************************************************************
	void CMessageStep::Send() const
	{
		// Log this message in the replay file
		ExtractReplayData();

		// Deliver the message.
		CMessage::Send();
	}


	//*****************************************************************************************
	void CMessageStep::ExtractReplayData() const
	{
		// if we are playing back a replay then don't save for now
		if (crpReplay.bSaveActive())
		{
			SReplayChunkStep	chs_chunk={sStep,sTotal};		// create a chunk

			// write it to the file
			(void) crpReplay.ulWriteReplayChunk(ectReplayChunkStep,
										(uint8*)&chs_chunk,
										sizeof(SReplayChunkStep));
		}
	}


	//*****************************************************************************************
	const char* CMessageStep::strData() const
	{
		static char str_data[10];
		sprintf(str_data, "%.3f", sStep);
		return str_data;
	}


//*********************************************************************************************
//
// CMessageControl implementation.
//

	SRecipientsInfo CMessageControl::riInfo;

	//*****************************************************************************************
	void CMessageControl::Send() const
	{
		// Log this message in the replay file
		ExtractReplayData();

		// Deliver the message.
		CMessage::Send();
	}


	//*****************************************************************************************
	void CMessageControl::ExtractReplayData() const
	{
		// if we are playing back a replay then don't save for now
		if (crpReplay.bSaveActive())		
		{
			crpReplay.ulWriteReplayChunk(ectReplayChunkControl,
										 (uint8*)(static_cast<const SInput*>(this)),
										 sizeof(SInput));
		}
	}



//*********************************************************************************************
//
// CMessageTrigger implementation.
//

	SRecipientsInfo CMessageTrigger::riInfo;

	//*****************************************************************************************
	void CMessageTrigger::Queue() const
	{
		CMessageTrigger* pmsg_new = new(frhFrameHeap) CMessageTrigger(ptrGetActivatedTrigger());
		*pmsg_new = *this;
		qmQueueMessage.pdqwmNextMessages->push({ pmsg_new });
	}



//*********************************************************************************************
//
// CMessageMoveTriggerTo implementation.
//

	SRecipientsInfo CMessageMoveTriggerTo::riInfo;


//**********************************************************************************************
//
// CMessagePickUp implementation.
//

	SRecipientsInfo CMessagePickUp::riInfo;


//**********************************************************************************************
//
// CMessageUse implementation.
//

	SRecipientsInfo CMessageUse::riInfo;


//**********************************************************************************************
//
// CMessageDeath implementation.
//

	SRecipientsInfo CMessageDeath::riInfo;


//**********************************************************************************************
//
// CMessageDamage implementation.
//

	SRecipientsInfo CMessageDamage::riInfo;



//*********************************************************************************************
//
// CMessageCollision implementation.
//

	SRecipientsInfo CMessageCollision::riInfo;

	//
	// Collision energies ranges.
	//

	// These are the inverses of the max energies.
	const float CMessageCollision::fENERGY_MAX_NORM		= 1 / 100.0;
	const float CMessageCollision::fENERGY_HIT_NORM		= 1 / 100.0;
	const float CMessageCollision::fENERGY_SLIDE_NORM	= 1 / 1000.0;

	// These are the inverses of the min energies.
	const float fINV_ENERGY_MAX_MIN		= 1 / 0.1;
	const float fINV_ENERGY_HIT_MIN		= 1 / 0.1;
	const float fINV_ENERGY_SLIDE_MIN	= 1 / 0.1;

	extern CProfileStat psCollisionMsgConstruct;

	//******************************************************************************************
	CMessageCollision::CMessageCollision
	(
		CInstance*	pins_one, 
		CInstance*	pins_two, 
		TSoundMaterial smat_one,
		TSoundMaterial smat_two,
		float		f_energy_max,
		float		f_energy_hit,
		float		f_energy_slide,
		const CVector3<>& v3,
		float		f_energy_delta_1,
		float		f_energy_delta_2,
		CEntity*	pet_sender,
		CRayCast*   prc_gun_hits
	)
		:	CMessage(&riInfo.rcRecipients, pet_sender), 
			pins1(pins_one),
			pins2(pins_two),
			smatSound1(smat_one),
			smatSound2(smat_two),
			fEnergyMax(f_energy_max),
			fEnergyHit(f_energy_hit),
			fEnergySlide(f_energy_slide),
			v3Position(v3),
			fEnergyDelta1(f_energy_delta_1),
			fEnergyDelta2(f_energy_delta_2),
			prcGunHits(prc_gun_hits)
	{
		CTimeBlock tmb(&psCollisionMsgConstruct);

		AlwaysAssert(pins_one);
		//Assert(!ptCast<CTerrain>(pins_one));

		// Infer sound materials if not specified.
		if (!smatSound1)
			smatSound1 = pins_one->pphiGetPhysicsInfo()->smatGetMaterialType(v3Position);

		if (!smatSound2)
		{
			if (pins_two)
				smatSound2 = pins_two->pphiGetPhysicsInfo()->smatGetMaterialType(v3Position);
			else
				// If instance is 0, we assume it's a bullet.
				smatSound2 = matHashIdentifier("BULLET");
		}
	}

	//******************************************************************************************
	CMessageCollision::CMessageCollision
	(
		CInstance*	pins_one, 
		CInstance*	pins_two, 
		float		f_energy_max,
		float		f_energy_hit,
		float		f_energy_slide,
		const CVector3<>& v3,
		float		f_energy_delta_1,
		float		f_energy_delta_2,
		CEntity*	pet_sender,
		CRayCast*   prc_gun_hits
	)
	{
		new(this) CMessageCollision(pins_one, pins_two, 0, 0, 
									f_energy_max, f_energy_hit, f_energy_slide, 
									v3, 
									f_energy_delta_1,
									f_energy_delta_2,
									pet_sender, prc_gun_hits);
	}

	//******************************************************************************************
	void CMessageCollision::Send() const
	{
		CMessage::Send();
	}

	//*********************************************************************************************
	float CMessageCollision::fEnergyMaxNormLog() const
	{
		static const float fINV_LOG_MAX		= -1.0f / log(fENERGY_MAX_NORM / fINV_ENERGY_MAX_MIN);

		// Logarithmic conversion.  Scale min..max to 0..1.
		// Clamp to usable range.
		if (fEnergyMax <= 0.0f)
			return 0.0f;
		float f_rel = fEnergyMax * fINV_ENERGY_MAX_MIN;
		float f_norm = log(f_rel) * fINV_LOG_MAX;
		return MinMax(f_norm, 0.0f, 1.0f);
	}

	//*********************************************************************************************
	float CMessageCollision::fEnergyHitNormLog() const
	{
		static const float fINV_LOG_HIT		= -1.0f / log(fENERGY_HIT_NORM / fINV_ENERGY_HIT_MIN);

		// Logarithmic conversion.  Scale min..max to 0..1.
		// Clamp to usable range.
		if (fEnergyHit <= 0.0f)
			return 0.0f;
		float f_rel = fEnergyHit * fINV_ENERGY_HIT_MIN;
		float f_norm = log(f_rel) * fINV_LOG_HIT;
		return MinMax(f_norm, 0.0f, 1.0f);
	}

	//*********************************************************************************************
	float CMessageCollision::fEnergySlideNormLog() const
	{
		static const float fINV_LOG_SLIDE		= -1.0f / log(fENERGY_SLIDE_NORM / fINV_ENERGY_SLIDE_MIN);

		if (fEnergySlide <= 0.0f)
			return 0.0f;
		float f_rel = fEnergySlide * fINV_ENERGY_SLIDE_MIN;
		float f_norm = log(f_rel) * fINV_LOG_SLIDE;
		return MinMax(f_norm, 0.0f, 1.0f);
	}


//*********************************************************************************************
//
// CMessageMove implementation.
//

namespace
{
	//******************************************************************************************
	//
	void DeliverMoveMsgToLocationTriggers
	(
		CPartition* ppart,
		const CVector3<>& v3_query_min,
		const CVector3<>& v3_query_max,
		const CMessageMove& msgmv
	)
	//
	// Helper function for move message class.
	//
	// Recursively descend the partition hierarchy and deliver the move message to the location
	// triggers that intersect the given volume.
	//
	//**************************
	{
		bool b_recurse = true;

		// Have we found pure spatial partition?
		if (ppart->bIsPureSpatial())
		{
			CCycleTimer ctmr;

			// Is its volume a bounding box (e.g. is it not infinite) ?
			const CBoundVolBox* pbb_part = ppart->pbvBoundingVol()->pbvbCast();

			if (pbb_part)
			{
				// Get the world min and max extents of the partition's bounding box.
				CVector3<> v3_part_min = (*pbb_part)[7] + ppart->v3Pos();
				CVector3<> v3_part_max = (*pbb_part)[0] + ppart->v3Pos();

				// Only recurse descendants if the query and partition bounding box overlap.
				b_recurse = v3_part_max.tX >= v3_query_min.tX && v3_part_min.tX <= v3_query_max.tX &&
				            v3_part_max.tY >= v3_query_min.tY && v3_part_min.tY <= v3_query_max.tY &&
				            v3_part_max.tZ >= v3_query_min.tZ && v3_part_min.tZ <= v3_query_max.tZ;
			}

			psMoveMsgQuery.Add(ctmr(), 1);
		}
		else
		{
			// Have we found a location trigger?
			CLocationTrigger* ploctrig = ptCast<CLocationTrigger>(ppart);

			if (ploctrig)
			{
				// Deliver move message to the location trigger.
				CCycleTimer ctmr;

				ploctrig->Process(msgmv);

				psMsgSubscribe.Add(ctmr(), 1);
			}
		}

		if (b_recurse)
		{
			ppart = ppart->ppartChildren();

			if (ppart != 0)
				for (CPartition::iterator it = ppart->begin(); it != ppart->end(); ++it)
					DeliverMoveMsgToLocationTriggers(*it, v3_query_min, v3_query_max, msgmv);
		}
	}
}

	//******************************************************************************************
	SRecipientsInfo CMessageMove::riInfo;

	const TReal CMessageMove::rTHRESHOLD_DEFAULT = 0.001;		// 1 mm.

	//******************************************************************************************
	bool CMessageMove::bSignificant(TReal r_threshold) const
	{
		// For now, living animating objects are automatically significant.
		// They may be animating, even if their placement doesn't change.
		CAnimate* pani = ptCast<CAnimate>(pinsMover);
		if (pani && !pani->bReallyDead())
			return true;

		// First test positional movement.
		if ((p3Prev.v3Pos - pinsMover->p3GetPlacement().v3Pos).tLenSqr() >= Sqr(r_threshold))
			return true;

		//
		// Then test rotation.  
		//

		// Get the delta rotation.
		CRotate3<> r3_rot = pinsMover->p3GetPlacement().r3Rot / p3Prev.r3Rot;

		//
		// Calculate the amount a point moves which is on the radius of the object.
		// This is equal to the angle in radians of rotation times the radius.
		// The angle is about equal to the sin for small angles, and we can get the 
		// sin from the cosine, which is the tC component of the rotation.
		//
		TReal r_rot_sqr = Sqr(pinsMover->pr3GetPresence().rScale) * (1.0 - Sqr(r3_rot.tC));

		// Arbitrarily reduce sensitivity *10, because floating-point inaccuracies
		// cause significance even on trivial rotations.
		return r_rot_sqr >= Sqr(r_threshold * 10.0);
	}

	//******************************************************************************************
	void CMessageMove::Send() const
	{
		//
 		// For efficiency, the move message dispatching to location triggers is handled as a special case.
		//
		// Location triggers are not registered as recipients for the move message type. Instead, we query the
		// trigger spatial partitions hierarchy for location triggers with a volume that includes the
		// current and previous presence of the mover.
		//
		if (!CMessage::bIgnoreRegisteredRecipients && !pinsMover->bNoSpatialInfo())
		{
			// Construct a world axis aligned bounding box that includes the current and previous presence of the
			// mover.
			CVector3<> v3_curr_min, v3_curr_max;
			pinsMover->pbvBoundingVol()->GetWorldExtents(pinsMover->pr3GetPresence(), v3_curr_min, v3_curr_max);

			// Assume scale remains constant for mover.
			CVector3<> v3_prev_min, v3_prev_max;
			pinsMover->pbvBoundingVol()->GetWorldExtents(CPresence3<>(p3Prev, pinsMover->pr3GetPresence().rScale), v3_prev_min, v3_prev_max);

			CVector3<> v3_min(Min(v3_prev_min.tX, v3_curr_min.tX), Min(v3_prev_min.tY, v3_curr_min.tY), Min(v3_prev_min.tZ, v3_curr_min.tZ));
			CVector3<> v3_max(Max(v3_prev_max.tX, v3_curr_max.tX), Max(v3_prev_max.tY, v3_curr_max.tY), Max(v3_prev_max.tZ, v3_curr_max.tZ));

			DeliverMoveMsgToLocationTriggers(pwWorld->ppartTriggerPartitionList(), v3_min, v3_max, *this);
		}

		// Default move message handling.
		CMessage::Send();
	}


//*********************************************************************************************
//
// CMessageDelete implementation.
//

	//******************************************************************************************
	CMessageDelete::CMessageDelete(CInstance* pins)
		: CMessage(), pinsDeleteMe(pins)
	{
	}

	//*****************************************************************************************
	void CMessageDelete::Queue() const
	{
		Send();
		//qmQueueMessage << (CMessageDelete)*this;
	}
	
	//******************************************************************************************
	void CMessageDelete::DeliverTo(CEntity* pet) const
	{
		Assert(pet);

		pet->Process(*this);
	}

	//*****************************************************************************************
	void CMessageDelete::Send() const
	{
		if (petGetRecipient() != 0)
			// Dispatch this message to the single recipient.
			DeliverTo(petGetRecipient());
		else
		{
			//
			// Dispatch this message to all the recipients.
			//
			for (CWDbQueryActiveEntities wqaet; wqaet.bIsNotEnd(); wqaet++)
			{
				Assert(wqaet.tGet());

				DeliverTo(wqaet.tGet());
			}
		}
	}


//*********************************************************************************************
//
// CMessageNewRaster implementation.
//

	//*****************************************************************************************
	void CMessageNewRaster::Send() const
	{
		// Write message to log.
		#if VER_LOG_MESSAGES
			conMessageLog.Print("Sending: ");
			WriteToLog();
		#endif

		// Log this message in the replay file
		ExtractReplayData();

		// Deliver the message.
		if (petGetRecipient() != 0)
			// Dispatch this message to the single recipient.
			DeliverTo(petGetRecipient());
		else
		{
			//
			// Dispatch this message to all the recipients that are shells.
			//
			Assert(wWorld.petShell);
			DeliverTo(wWorld.petShell);
		}
	}


	//*****************************************************************************************
	void CMessageNewRaster::ExtractReplayData() const
	{
		// Not saved for replay.
	}


//**********************************************************************************************
//
// CMessagePaint implementation.
//

	SRecipientsInfo CMessagePaint::riInfo;

	//******************************************************************************************
	void CMessagePaint::Send() const
	{
		CMessage::Send();
	}



//**********************************************************************************************
//
// CMessageSystem implementation.
//

	SRecipientsInfo CMessageSystem::riInfo;

	//*****************************************************************************************
	bool CMessageSystem::bSimGoing = false;

	//*****************************************************************************************
	void CMessageSystem::Send() const
	{
		// System messages should always be broadcast.
		Assert(petGetRecipient() == 0);

		// Update the static bSimGoing before we send the message
		if (escCode == escSTART_SIM)
		{
			CTimer::Pause(false);
			bSimGoing = true;
		}
		else if (escCode == escSTOP_SIM)
		{
			CTimer::Pause(true);
			bSimGoing = false;
		}

		// Call the base class send function
		CMessage::Send();
	}


//*********************************************************************************************
//
// CMessagePhysicsReq implementation.
//

	SRecipientsInfo CMessagePhysicsReq::riInfo;


//*********************************************************************************************
//
// CMessageAudio implementation.
//

	SRecipientsInfo CMessageAudio::riInfo;

	//*****************************************************************************************
	void CMessageAudio::Queue() const
	{
		CMessageAudio* pmsg_new = new(frhFrameHeap) CMessageAudio();
		*pmsg_new = *this;
		qmQueueMessage.pdqwmNextMessages->push({ pmsg_new });
	}
