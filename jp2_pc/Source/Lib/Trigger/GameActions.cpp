/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		Implementation of GameAction.hpp
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/GameActions.cpp                                          $
 * 
 * 36    9/21/98 6:41p Mlange
 * Fixed bug in hide/show action - critical code was executed on an assert, which meant that it
 * did not get executed in a release build.
 * 
 * 35    9/21/98 1:06a Agrant
 * added water disturbance action
 * 
 * 34    9/12/98 2:03p Kmckis
 * fixed load problem for hide/show
 * 
 * 33    9/10/98 12:03a Agrant
 * added player comtrol action and set ai system action
 * 
 * 32    98.09.03 5:03p Mmouni
 * Added end game action.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "GameActions.hpp"
#include "Trigger.hpp"

#include "Lib/GROFF/VTParse.hpp"
#include "Lib/EntityDBase/Animal.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/Physics/Magnet.hpp"
#include "Lib/GeomDBase/Skeleton.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Lib/Std/Hash.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/Physics/WaterDisturbance.hpp"
#include "Game/AI/MoveActivities.hpp"
#include "Game/AI/Brain.hpp"
#include "Game/AI/AIMain.hpp"
#include "Game/DesignDaemon/Player.hpp"



//*********************************************************************************************
// SET AI PARAMETERS ACTION
//*********************************************************************************************
//
CSetAIAction::CSetAIAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	//
	//  Defaults!
	//

	// Mark all parameters in the feeling as not updating.
	{
		for (int i = eptEND - 1; i >= 0; --i)
			feelNewEmotions[i] = -1.0f;
	}

	pinsStayNearTarget = 0;
	pinsStayAwayTarget = 0;

	{
		for (int i = eatEND - 1; i >= 0; --i)
			au1ActivityStates[i] = 0;	// 0 for ignore, 1 for On, 2 for Off
	}
	
	//
	//  Loading from props....
	//

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		float f = -1;
		const CEasyString* pestr = 0;

		bFILL_pEASYSTRING(pestr, esTarget);

		// Verify that we have a valid target.
		Assert(pestr);

		// Get the target.
		CInstance* pins = wWorld.ppartPartitionList()->pinsFindNamedInstance(pestr->strData());

		// Did we find it?
		Assert(pins);

		paniTarget = ptCast<CAnimal>(pins);

		// Was it an animal?
		Assert(paniTarget);

		// Load stay near target!
		if (bFILL_pEASYSTRING(pestr, esStayNearTarget))
		{
			// Get the target.
			pinsStayNearTarget = wWorld.ppartPartitionList()->pinsFindNamedInstance(pestr->strData());
			Assert(pinsStayNearTarget);
		}
	
		// Load stay near target!
		if (bFILL_pEASYSTRING(pestr, esStayAwayTarget))
		{
			// Get the target.
			pinsStayAwayTarget = wWorld.ppartPartitionList()->pinsFindNamedInstance(pestr->strData());
			Assert(pinsStayAwayTarget);
		}
		
		{
			// Load in the feeling.
			for (int i = 0; i < eptEND; ++i)
				if (bFILL_FLOAT(f, ESymbol(esParameter00 + i)))
				{
					Assert(f >= 0.0f && f <= 1.0f);
					feelNewEmotions[i] = f;
				}
		}

		// Load in the activity states.
		{
			// Search for each prop in the value table.  
			for (int i = 0; i < eatEND; ++i)
			{
				bool b_active = false;
				if (bFILL_BOOL(b_active, ESymbol(esActBASE + i)))
				{
					if (b_active)
						au1ActivityStates[i] = 1;
					else
						au1ActivityStates[i] = 2;
				}
			}
		}
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;
}



//*********************************************************************************************
void CSetAIAction::Start
(
)
//	
//**************************************
{
	//dprintf("ACTION: Set AI \n");
	// Set the feeling.
	Assert(paniTarget);
	paniTarget->SetEmotions(feelNewEmotions);

	if (pinsStayNearTarget)
	{
		((CActivityMoveCommandStayNear*)paniTarget->pbrBrain->sapactActivities[(int)eatSTAY_NEAR])->pinsTarget = pinsStayNearTarget;
	}

	if (pinsStayAwayTarget)
	{
		((CActivityMoveCommandStayAway*)paniTarget->pbrBrain->sapactActivities[(int)eatSTAY_AWAY])->pinsTarget = pinsStayAwayTarget;
	}

	// Search for each prop in the value table.  
	for (int i = 0; i < eatEND; ++i)
	{
		if (au1ActivityStates[i])
		{
			if (au1ActivityStates[i] == 1)
				paniTarget->pbrBrain->sapactActivities[i]->Activate(true);
			else if (au1ActivityStates[i] == 2)
				paniTarget->pbrBrain->sapactActivities[i]->Activate(false);
			else
			{
				// Invalid activity state!
				AlwaysAssert(false);
			}
		}
	}
}


//*********************************************************************************************
// WAKE AI ACTION
//*********************************************************************************************
//
CWakeAIAction::CWakeAIAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	bWakeUp		= true;			// true if waking, false if putting to sleep
	paniTarget	= 0;			// default to no target
	v3Center	= ptr_trig->v3Pos();	// default to position of trigger
	rRadius		= ptr_trig->fGetScale();// default to scale of trigger
	
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		float f = -1;
		const CEasyString* pestr = 0;

		if (bFILL_pEASYSTRING(pestr, esTarget))
		{
			// Verify that we have a valid target.
			Assert(pestr);

			// Get the target.
			CInstance* pins = wWorld.ppartPartitionList()->pinsFindNamedInstance(pestr->strData());

			// Did we find it?
			Assert(pins);

			paniTarget = ptCast<CAnimal>(pins);

			// Was it an animal?
			AlwaysAssert(paniTarget);
		}

		if (bFILL_pEASYSTRING(pestr, esLocation))
		{
			CGroffObjectName* pgon = pload->goiInfo.pgonFindObject(pestr->strData());

			// Are we missing the location object?
			AlwaysAssert(pgon);
		
			v3Center = ::pr3Presence(*pgon).v3Pos;
		}


		bFILL_FLOAT(rRadius, esRadius);
		bFILL_BOOL(bWakeUp, esWakeUp);
		bFILL_FLOAT(v3Center.tX, esX);
		bFILL_FLOAT(v3Center.tY, esY);
		bFILL_FLOAT(v3Center.tZ, esZ);

	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;
}



//*********************************************************************************************
void CWakeAIAction::Start
(
)
//	
//**************************************
{
	if (bWakeUp)
	{
		if (paniTarget)
			gaiSystem.ActivateAnimal(paniTarget);
		else
			gaiSystem.ActivateAnimals(v3Center, rRadius);
	}
	else
	{
		if (paniTarget)
			gaiSystem.DeactivateAnimal(paniTarget);
		else
			gaiSystem.DeactivateAnimals(v3Center, rRadius);
	}
}


//*********************************************************************************************
// SET PHYSICS PARAMETERS ACTION
//*********************************************************************************************
//
CSetPhysicsAction::CSetPhysicsAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{

	v3Velocity		= CVector3<>(0,0,0);
	bFreeze			= false;

	// Impulse variables.
	bImpulse			= false;
	v3ImpulseOrigin		= CVector3<>(0,0,0);
	v3ImpulseDirection	= CVector3<>(0,0,0);
	rImpulse			= 0.0f;
	pinsImpulseSource	= 0;

	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		const CEasyString* pestr = 0;

		bFILL_pEASYSTRING(pestr, esTarget);

		// Verify that we have a valid target.
		AlwaysAssert(pestr);

		// Get the target.
		pinsTarget = wWorld.ppartPartitionList()->pinsFindNamedInstance(pestr->strData());

		// Did we find it?
		AlwaysAssert(pinsTarget);

		bFILL_BOOL(bFreeze, esFrozen);

		// Impulse?
		bFILL_BOOL(bImpulse, esImpulse);

		// Are we making an impulse action?
		if (bImpulse)
		{
			// Yes!  Get the impulse magnitude.
			bFILL_FLOAT(rImpulse, esPush);			
			
			// Grab the impulse source object.
			pestr = 0;
			bFILL_pEASYSTRING(pestr, esTActionEmitter);

			// Verify that we have a valid source.
			AlwaysAssert(pestr);

			// Get the source.
			pinsImpulseSource = wWorld.ppartPartitionList()->pinsFindNamedInstance(pestr->strData());

			// Did we find an actual source object?
			if (!pinsImpulseSource)
			{
				//  No!  Just use the position/rotation of the GROFF object.
				CGroffObjectName* pgon = pload->goiInfo.pgonFindObject(pestr->strData());

				CPresence3<> pr3 = ::pr3Presence(*pgon);

				// Locate impulse source at center of source object.
				v3ImpulseOrigin = pr3.v3Pos;

				// Point impulse down source object's Y axis.
				v3ImpulseDirection = CVector3<>(0,1,0) * pr3.r3Rot;
			}
		}
		else
		{
			// No impulse.  Just get a velocity.
			bFILL_FLOAT(v3Velocity.tX, esX);
			bFILL_FLOAT(v3Velocity.tY, esY);
			bFILL_FLOAT(v3Velocity.tZ, esZ);
		}

	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

}



//*********************************************************************************************
void CSetPhysicsAction::Start
(
)
//	
//**************************************
{
	//dprintf("ACTION: Set AI \n");
	// Set the physics.

	Assert(pinsTarget);

	const CPhysicsInfo* pphi = pinsTarget->pphiGetPhysicsInfo();
	Assert(pphi);

	NMagnetSystem::SetFrozen(pinsTarget, bFreeze);

	// Did we freeze it?
	if (!bFreeze)
	{
		// No!  Go ahead perform the rest.

		// Is this an impulse?
		if (bImpulse)
		{
			// Yes!  Apply the impulse.

			if (pinsImpulseSource)
			{
				v3ImpulseOrigin = pinsImpulseSource->v3Pos();
				v3ImpulseDirection = CVector3<>(0,1,0) * pinsImpulseSource->r3Rot();
			}

			pphi->ApplyImpulse
			(
				pinsTarget, 0,
				v3ImpulseOrigin,
				v3ImpulseDirection * rImpulse
			);


		}
		else
		{
			//  No!  Set velocity.
			pinsTarget->pphiGetPhysicsInfo()->ForceVelocity(pinsTarget, v3Velocity);
		}
	}
}


//*********************************************************************************************
// Substitute Mesh ACTION
//*********************************************************************************************
//
CSubstituteMeshAction::CSubstituteMeshAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		const CEasyString* pestr = 0;

		bFILL_pEASYSTRING(pestr, esTarget);

		// Verify that we have a valid target.
		AlwaysAssert(pestr);

		// Get the target.
		pinsTarget = wWorld.ppartPartitionList()->pinsFindNamedInstance(pestr->strData());

		// Did we find it?
		Assert(pinsTarget);

		// Do we return the mesh to its previous state on exit?
		bReset = true;
		bFILL_BOOL(bReset, esReset);

		// Which substitute do we use?
		bFILL_INT(iSubstitute, esSubstitute);

	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;
}



//*********************************************************************************************
void CSubstituteMeshAction::Start
(
)
//	
//**************************************
{
	Assert(pinsTarget);
	CAnimate* pani = ptCast<CAnimate>(pinsTarget);
	Assert(pani);
	pani->Substitute(iSubstitute);
}


//*********************************************************************************************
void CSubstituteMeshAction::Stop
(
)
//	
//**************************************
{
	Assert(pinsTarget);
	CAnimate* pani = ptCast<CAnimate>(pinsTarget);
	Assert(pani);
	pani->Substitute(0);
}



//*********************************************************************************************
//
// CMagnetAction implementation.
//

	//*********************************************************************************************
	CMagnetAction::CMagnetAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	)
	{
		u4MasterHandle	= 0;
		u4SlaveHandle	= 0;
		pmagMagnet		= 0;
		bEnable			= true;
		bDelta			= false;
		bMerge			= false;
		pinsQuery		= 0;

		// Get values from text-props.
		SETUP_TEXT_PROCESSING(pvtable, pload);
		IF_SETUP_OBJECT_POINTER(poval_action)
		{
			const CEasyString* pestr_object = 0;
			if (bFILL_pEASYSTRING(pestr_object, esMasterObject))
			{
				u4MasterHandle = u4Hash((const char *)pestr_object->strData());
			}

			if (bFILL_pEASYSTRING(pestr_object, esSlaveObject))
			{
				u4SlaveHandle = u4Hash((const char *)pestr_object->strData());
			}

			// Load Query instance!
			if (bFILL_pEASYSTRING(pestr_object, esQuery))
			{
				// Get the target.
				pinsQuery = wWorld.ppartPartitionList()->pinsFindNamedInstance(pestr_object->strData());
				Assert(pinsQuery);
			}

			Verify(bFILL_BOOL(bEnable,	esEnable));
			bFILL_BOOL(bDelta,	esDelta);
			bFILL_BOOL(bMerge,	esMerge);

			if (bEnable)
				pmagMagnet = CMagnet::pmagFindShared(pgon->hAttributeHandle, poval_action, pvtable, pload);
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		// Must at least have a master object and a magnet.
		Assert(u4MasterHandle && (!bEnable || pmagMagnet));
	}

	//*********************************************************************************************
	void CMagnetAction::Start()
	{
		CInstance* pins_master = pwWorld->pinsFindInstance(u4MasterHandle);
		AlwaysAssert(pins_master);

		CInstance* pins_slave = 0;

		if (u4SlaveHandle)
		{
			pins_slave = pwWorld->pinsFindInstance(u4SlaveHandle);
			AlwaysAssert(pins_slave);
		}

		// Drop anything if we are magnetting it.
		CVector3<> v3_drop_velocity(0,0,0);
		if (pins_master == gpPlayer->pinsHeldObject())
		{
			gpPlayer->Drop(v3_drop_velocity);
		}
		else if (pins_slave == gpPlayer->pinsHeldObject())
		{
			gpPlayer->Drop(v3_drop_velocity);
		}

		// Twiddle the physics so we don't get floating objects.
		pins_master->PhysicsDeactivate();
		if (pins_slave)
			pins_slave->PhysicsDeactivate();

		// Is this an "add magnet" trigger?
		if (bEnable)
		{
			// Do we already have the magnet pair?
			const CMagnetPair* pmp = NMagnetSystem::pmpFindMagnet(pins_master, pins_slave, set0);
			if (pmp)
			{
				// Yes!  Already exists.  Change its CMagnet.

				// Merging or deltaing?
				if (!bDelta && !bMerge)
				{
					// No!  Just set the data.
					((CMagnetPair*)pmp)->pmagData = pmagMagnet;
				}
				else
				{
					// Yes!  Construct the new magnet.
					CMagnet mag = *pmp->pmagData;
					float f_drive_multiple = 1.0f;

					// If we have a query instance, query it.
					if (pinsQuery)
					{
						f_drive_multiple = NMagnetSystem::fAngleRotated(pinsQuery);
					}

					// Can't do both!
					AlwaysAssert(bDelta != bMerge);


					if (bDelta)
					{
						//  Doing a delta.

						// Ignore flags.
						// mag.setFlags	

						mag.fBreakStrength += pmagMagnet->fBreakStrength;

						// Ignore substitute mesh.
						// mag.u1SubstituteMesh

						mag.fRestore += pmagMagnet->fRestore;
				
						mag.fFriction += pmagMagnet->fFriction;
				
						mag.fDrive += pmagMagnet->fDrive * f_drive_multiple;
				
						mag.fAngleMin += pmagMagnet->fAngleMin;
					
						mag.fAngleMax += pmagMagnet->fAngleMax;
					}
					else
					{
						// Doing a merge.
						//  Doing a delta.

						// Ignore flags.
						// mag.setFlags	

						if (pmagMagnet->fBreakStrength)
							mag.fBreakStrength = pmagMagnet->fBreakStrength;

						if (pmagMagnet->u1SubstituteMesh)
							mag.u1SubstituteMesh = pmagMagnet->u1SubstituteMesh;

						if (pmagMagnet->fRestore)
							mag.fRestore = pmagMagnet->fRestore;
				
						if (pmagMagnet->fFriction)
							mag.fFriction = pmagMagnet->fFriction;
				
						if (pmagMagnet->fDrive)
							mag.fDrive = pmagMagnet->fDrive * f_drive_multiple;
				
						if (pmagMagnet->fAngleMin)
							mag.fAngleMin = pmagMagnet->fAngleMin;
					
						if (pmagMagnet->fAngleMax)
							mag.fAngleMax = pmagMagnet->fAngleMax;
					}

					// Now get a magnet pointer.
					const CMagnet* pmag = CMagnet::pmagFindShared(mag);

					// Set the data.
					((CMagnetPair*)pmp)->pmagData = pmag;
				}
			}
			else
			{
				// Make sure we aren't creating a delta of merge magnet-
				AlwaysAssert(!bDelta);
				AlwaysAssert(!bMerge);

				// Doesn't exist.  Create it.
				NMagnetSystem::AddMagnetPair(pins_master, pins_slave, pmagMagnet);
			}
		}
		else
		{
			// Remove the specified magnet pair.
			NMagnetSystem::RemoveMagnetPair(pins_master, pins_slave, Set(emfFREEZE) + emfHINGE + emfSLIDE);
		}

		// Turn physics back on.
		pins_master->PhysicsActivate();
		if (pins_slave)
			pins_slave->PhysicsActivate();
	}



//*********************************************************************************************
// Substitute Mesh ACTION
//*********************************************************************************************
//
CAnimateTextureAction::CAnimateTextureAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//	
//**************************************
{
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		const CEasyString* pestr = 0;

		bFILL_pEASYSTRING(pestr, esTarget);

		// Verify that we have a valid target.
		AlwaysAssert(pestr);

		// Get the target.
		pinsTarget = wWorld.ppartPartitionList()->pinsFindNamedInstance(pestr->strData());

		// Did we find it?
		Assert(pinsTarget);

		// What is the new interval?
		sInterval = 0.0f;
		bFILL_FLOAT(sInterval, esInterval);
		if (sInterval < 0)
			sInterval = FLT_MAX;

		// Which frame do we set it to?
		iFrame = -1;
		bFILL_INT(iFrame, esFrame);

		iFreezeFrame = -2;	// Freeze frame, -2 if not set.  -1 means clear freeze frame.
		bFILL_INT(iFreezeFrame, esFreezeFrame);

		iTrackTwo = -1;		// New track two start.  -1 if not set.
		bFILL_INT(iTrackTwo, esTrackTwo);
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;
}



//*********************************************************************************************
void CAnimateTextureAction::Start
(
)
//	
//**************************************
{
	Assert(pinsTarget);

	rptr_const<CRenderType> prdt = pinsTarget->prdtGetRenderInfo();

	CMesh* pmsh = (CMesh*)prdt.ptGet();

	Assert(pmsh);
	AlwaysAssert(pmsh->bIsAnimated());

	CMeshAnimating* pma = (CMeshAnimating*)pmsh;

	if (iFreezeFrame > -2)
	{
		pma->SetFreezeFrame(iFreezeFrame);
	}

	if (iTrackTwo > -1)
	{
		pma->SetTrackTwo(iTrackTwo);
	}

	// Set the current frame.
	if (iFrame >= 0)
	{
		// Where the magic happens!
		pma->SetFrame(iFrame);
	}

	// Set the interval.
	if (sInterval > 0.0f)
	{
		pma->SetInterval(sInterval);
	}
}



//*********************************************************************************************
//
// CHideShowAction implementation.
//
namespace
{
	//*********************************************************************************************
	//
	CPartition* ppartFindContainedIn
	(
		CPartition* ppart,
		const CVector3<>& v3_world_min,		// Extent of axis aligned bounding box, in world space.
		const CVector3<>& v3_world_max
	)
	//
	// Find the first partition (i.e. closest to given) that is fully contained in the given axis
	// aligned bounding box.
	//
	// Returns:
	//		The found partition, or null if none.
	//
	//**************************************
	{
		Assert(ppart != 0);

		// Local scope to cut down on stack usage.
		{
			CVector3<> v3_part_min;
			CVector3<> v3_part_max;

			if (ppart->bGetWorldExtents(v3_part_min, v3_part_max))
			{
				// Early out if the volumes do not overlap.
				if (v3_part_max.tX < v3_world_min.tX || v3_part_min.tX > v3_world_max.tX ||
					v3_part_max.tY < v3_world_min.tY || v3_part_min.tY > v3_world_max.tY ||
					v3_part_max.tZ < v3_world_min.tZ || v3_part_min.tZ > v3_world_max.tZ   )
					return 0;

				// Determine if this partition's bounding volume is contained within the given world volume.
				if (v3_part_min.tX >= v3_world_min.tX && v3_part_max.tX <= v3_world_max.tX &&
					v3_part_min.tY >= v3_world_min.tY && v3_part_max.tY <= v3_world_max.tY &&
					v3_part_min.tZ >= v3_world_min.tZ && v3_part_max.tZ <= v3_world_max.tZ   )
					return ppart;
			}
		}


		// Recursively apply containment test to child partitions.
		ppart = ppart->ppartChildren();

		if (ppart != 0)
		{
			for (CPartition::iterator it = ppart->begin(); it != ppart->end(); ++it)
			{
				CPartition* ppart_found = ppartFindContainedIn(*it, v3_world_min, v3_world_max);

				if (ppart_found != 0)
					return ppart_found;
			}
		}

		return 0;
	}
}

//*********************************************************************************************
CHideShowAction::CHideShowAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
{
	u4Handle        = 0;
	bVolume			= false;
	bVisible		= true;
	bToggle			= false;

	// Get values from text-props.
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		const CEasyString* pestr_object = 0;
		if (bFILL_pEASYSTRING(pestr_object, esObjectName))
		{
			u4Handle = u4Hash((const char *)pestr_object->strData());
		}

		bFILL_BOOL(bVolume, esVolume);
		bFILL_BOOL(bVisible, esVisible);
		bFILL_BOOL(bToggle, esToggle);
	}
	END_OBJECT;
	END_TEXT_PROCESSING;

	// Must have an object to work on.
	Assert(u4Handle != 0);
}


//*********************************************************************************************
void CHideShowAction::Start()
{
	CInstance* pins = pwWorld->pinsFindInstance(u4Handle);
	Assert(pins != 0);
	if (pins == 0)
		return;

	if (bVolume)
	{
		CVector3<> v3_volume_min;
		CVector3<> v3_volume_max;
		AlwaysAssert(pins->bGetWorldExtents(v3_volume_min, v3_volume_max));

		CPartition* ppart = ppartFindContainedIn(pwWorld->ppartPartitionList(), v3_volume_min, v3_volume_max);

		if (ppart != 0)
			ppart->SetVisible(bVisible);
	}
	else
		pins->SetVisible(bVisible);

	// toggle the state if we have to.
	if (bToggle)
		bVisible = !bVisible;
}


//*********************************************************************************************
const char* CHideShowAction::pcLoad(const char* pc)
{
	pc = pcLoadT(pc, &bVisible);

	return pc;
}


//*********************************************************************************************
char* CHideShowAction::pcSave(char* pc) const
{
	pc = pcSaveT(pc, bVisible);

	return pc;
}


//*********************************************************************************************
// Action to set the current hint id.
//*********************************************************************************************
//
CSetHintAction::CSetHintAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
//*************************************
{
	iHintID = 0;

	// Get values from text-props.
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		bFILL_INT(iHintID, esHintID);
	}
	END_OBJECT;
	END_TEXT_PROCESSING;
}


//*********************************************************************************************
void CSetHintAction::Start()
{
	// The the hint to use.
	iCurrentHint = iHintID;
}


//*********************************************************************************************
const char* CSetHintAction::pcLoad(const char* pc)
{
	// Save current hint (gets saved multipled times, but who cares).
	pc = pcLoadT(pc, &iCurrentHint);

	return pc;
}


//*********************************************************************************************
char* CSetHintAction::pcSave(char* pc) const
{
	// Load current hint (gets saved multipled times, but who cares).
	pc = pcSaveT(pc, iCurrentHint);

	return pc;
}


//*********************************************************************************************
int CSetHintAction::iCurrentHint = 0;




//*********************************************************************************************
//
// CSubstituteAIAction implementation.
//

	//*********************************************************************************************
	CSubstituteAIAction::CSubstituteAIAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	)
	{
		paiiTargetOld     = 0;
		paiiSubstituteOld = 0;

		bSubstituted = false;

		// Get values from text-props.
		SETUP_TEXT_PROCESSING(pvtable, pload);
		IF_SETUP_OBJECT_POINTER(poval_action)
		{
			const CEasyString* pestr_target = 0;

			// Target object must be specified.
			AlwaysAssert(bFILL_pEASYSTRING(pestr_target, esTarget));
			u4HashTarget = u4Hash((const char *)pestr_target->strData());

			const CEasyString* pestr_substitute = 0;

			// Substitute object must be specified.
			AlwaysAssert(bFILL_pEASYSTRING(pestr_substitute, esSubstitute));
			u4HashSubstitute = u4Hash((const char *)pestr_substitute->strData());
		}
		END_OBJECT;
		END_TEXT_PROCESSING;
	}

	//*********************************************************************************************
	CSubstituteAIAction::~CSubstituteAIAction()
	{
	}

	//*********************************************************************************************
	void CSubstituteAIAction::Start()
	{
		bSubstituted = !bSubstituted;

		CInstance* pins_target     = pwWorld->pinsFindInstance(u4HashTarget);
		CInstance* pins_substitute = pwWorld->pinsFindInstance(u4HashSubstitute);

		AlwaysAssert(pins_target && pins_substitute);

		// Is this the first time the AI properties are substituted?
		if (paiiTargetOld == 0)
		{
			Assert(!bSubstituted);
			paiiTargetOld     =     pins_target->paiiGetAIInfoWritable();
			paiiSubstituteOld = pins_substitute->paiiGetAIInfoWritable();
		}

		if (!bSubstituted)
		{
			// Substitute AI properties.
				pins_target->SetAIInfo(paiiSubstituteOld);
			pins_substitute->SetAIInfo(paiiTargetOld);
		}
		else
		{
			// Restore original AI properties.
				pins_target->SetAIInfo(paiiTargetOld);
			pins_substitute->SetAIInfo(paiiSubstituteOld);
		}

		bSubstituted = !bSubstituted;
	}

	//*********************************************************************************************
	const char* CSubstituteAIAction::pcLoad(const char* pc)
	{
		bool b_old_subst = bSubstituted;
		pc = pcLoadT(pc, &bSubstituted);

		// If the loaded substitution state is different from the current, toggle it.
		if (bSubstituted != b_old_subst)
			Start();

		return pc;
	}

	//*********************************************************************************************
	char* CSubstituteAIAction::pcSave(char* pc) const
	{
		pc = pcSaveT(pc, bSubstituted);
		return pc;
	}


//*********************************************************************************************
//
// End game action.
//
CEndGameAction::CEndGameAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
{
	// Don't need to do anything.
}


//*********************************************************************************************
void CEndGameAction::Start
(
)
{
	// Tell the world the game is over.
	pwWorld->GameIsOver();
}



//*********************************************************************************************
//
// CControlPlayerAction 
//
CControlPlayerAction::CControlPlayerAction 
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
{
	bDropHeldItem = false;

	// Get values from text-props.
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		bFILL_BOOL(bDropHeldItem, esPutDownObject);
	}
	END_OBJECT;
	END_TEXT_PROCESSING;
}


//*********************************************************************************************
void CControlPlayerAction::Start()
{
	if (bDropHeldItem)
		gpPlayer->Drop(CVector3<>(0,0,0));
}

//*********************************************************************************************
//
// CAISystemAction
//
CAISystemAction::CAISystemAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
{
	bWakeUp = true;

	// Get values from text-props.
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		bFILL_BOOL(bWakeUp, esWakeUp);
	}
	END_OBJECT;
	END_TEXT_PROCESSING;
}


//*********************************************************************************************
void CAISystemAction::Start()
{
	gaiSystem.bSleep = !bWakeUp;
}



//*********************************************************************************************
//
// CWaterDisturbanceAction
//
CWaterDisturbanceAction::CWaterDisturbanceAction
(
	CTrigger*				ptr_trig,
	const CGroffObjectName*	pgon,				// Object to load.
	CObjectValue*			poval_action,		// Pointer to the actual action class
	CValueTable*			pvtable,			// Pointer to the value table.
	CLoadWorld*				pload				// the loader.
)
{
	u4Target = 0;
	pwdTarget = 0;

	// Get values from text-props.
	SETUP_TEXT_PROCESSING(pvtable, pload);
	IF_SETUP_OBJECT_POINTER(poval_action)
	{
		const CEasyString* pestr_object = 0;
		if (bFILL_pEASYSTRING(pestr_object, esTarget))
		{
			u4Target = u4Hash((const char *)pestr_object->strData());
			AlwaysAssert(bInGroff(pload, pestr_object->strData()));
		}
	}
	END_OBJECT;
	END_TEXT_PROCESSING;
}


//*********************************************************************************************
CWaterDisturbanceAction::~CWaterDisturbanceAction()
{}


//*********************************************************************************************
void CWaterDisturbanceAction::Start()
{
	if (!pwdTarget)
	{
		pwdTarget = ptCast<CWaterDisturbance>(pwWorld->pinsFindInstance(u4Target));
	}

	AlwaysAssert(pwdTarget);

	// Disturb the next chance you get.
	pwdTarget->sNextDisturbance = CMessageStep::sStaticTotal - 0.0001f;
}
