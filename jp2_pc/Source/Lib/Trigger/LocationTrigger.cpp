/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of CLocationTrigger defined in Trigger.hpp
 *
 * Contents:
 *		CLocationTrigger
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/LocationTrigger.cpp                                      $
 * 
 * 36    10/04/98 8:00p Rwyatt
 * None looped sequenced actions will now continue until the end of the sequence even if the
 * player leaves the trigger. Looped sequences are the same as they used to be.
 * 
 * 35    10/01/98 4:05p Mlange
 * Improved move message stat reporting.
 * 
 * 34    9/23/98 11:24p Agrant
 * Player still triggers location triggers if the player is dead.
 * 
 * 33    9/14/98 12:26a Rwyatt
 * Player will ignore location triggers while dead
 * 
 * 32    8/22/98 5:39p Mlange
 * Location triggers must now not register themselves to receive move messages. Move messages
 * are delivered to location triggers as a special case.
 * 
 * 31    8/18/98 8:12p Mmouni
 * Changed CLocationTrigger so that if an object is not specified only tangible moveable
 * objects will trigger the trigger.
 * 
 **********************************************************************************************/

#include "Trigger.hpp"
#include <algo.h>

#include "Action.hpp"
#include "Lib/Physics/InfoBox.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgTrigger.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/GeomDBase/Partition.hpp"
#include "Lib/GeomDBase/Plane.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Game\DesignDaemon\Player.hpp"

#include "Lib/Sys/Profile.hpp"

extern CProfileStat psMoveMsgTrigger;

//*********************************************************************************************
//
// CLocationTrigger implementation.
//

struct STAList
{
	ESymbol	esHandle;
	uint32	u4TriggerFlag;
};

//*********************************************************************************************
CLocationTrigger::CLocationTrigger()
{
	// Register this entity with the message types it needs to receive.
	// Note that move messages are handled separately and do not need to be registered.
		     CMessageStep::RegisterRecipient(this);
	CMessageMoveTriggerTo::RegisterRecipient(this);
}

//*********************************************************************************************
CLocationTrigger::CLocationTrigger
(
	const CGroffObjectName*	pgon,		// Object to load.
	CLoadWorld*				p_load,		// The loader.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
) : CTrigger(pgon, p_load, h_object, pvtable, pinfo)
{
	Assert( h_object != hNULL_HANDLE);

	TCondition		tco_trigger		= 0;
	int				i_enter_count	= 0;
	int				i_leave_count	= 0;
	bool			b_cflag;
	CInstance*		pins_activate = NULL;

	// a static list of handles and actions
	static STAList	ta_list[]=
	{
		{esLTPlayerIn,		TRIGGER_COND_PLAYER_ALWAYS},
		{esLTPlayerEnter,	TRIGGER_COND_PLAYER_MOVE_INSIDE},
		{esLTPlayerLeave,	TRIGGER_COND_PLAYER_MOVE_OUTSIDE},

		{esLTObjectIn,		TRIGGER_COND_OBJECT_ALWAYS},
		{esLTObjectEnter,	TRIGGER_COND_OBJECT_MOVE_INSIDE},
		{esLTObjectLeave,	TRIGGER_COND_OBJECT_MOVE_OUTSIDE},

		{esLTCreatureIn,	TRIGGER_COND_CREATURE_ALWAYS},
		{esLTCreatureEnter,	TRIGGER_COND_CREATURE_MOVE_INSIDE},
		{esLTCreatureLeave,	TRIGGER_COND_CREATURE_MOVE_OUTSIDE},
		{esEND,				0}
	};

	// by default we are a point trigger
	bPointTrigger = true;

	SETUP_TEXT_PROCESSING(pvtable, p_load)
	SETUP_OBJECT_HANDLE(h_object)
	{
		// Process all text props.

		// Enter/Leave coutns.
		bFILL_INT(i_enter_count,esLTEnterCount);
		bFILL_INT(i_leave_count,esLTLeaveCount);

		// Are we a point (contains) or volume (intersect) trigger
		bFILL_BOOL(bPointTrigger,esLTPointTrigger);

		if (i_enter_count > 0)
		{
			// if an enter count is specifed then set the enter flag
			tco_trigger |= TRIGGER_COND_CREATURE_ENTER_COUNT;
		}

		if (i_leave_count > 0)
		{
			// if an leave count is specifed then set the enter flag
			tco_trigger |= TRIGGER_COND_CREATURE_LEAVE_COUNT;
		}

		// scan through our local table and set the flags that handles are present for...
		int u4_count=0;

		while ( ta_list[u4_count].esHandle != esEND )
		{
			b_cflag=FALSE;
			bFILL_BOOL(b_cflag, ta_list[u4_count].esHandle ); 
			if (b_cflag)
				tco_trigger |= ta_list[u4_count].u4TriggerFlag;

			u4_count++;
		}


		// If no condition was set, default to player enter.
		if (tco_trigger == 0)
		{
			tco_trigger |= TRIGGER_COND_PLAYER_MOVE_INSIDE;
		}


		const CEasyString* pestr = 0;

		// look for an activate object
		if (bFILL_pEASYSTRING(pestr, esLTTriggerActivate))
		{
			// assert on a valid string pointer and valid string (not NULL)
			Assert( pestr->strData() );
			Assert( *(pestr->strData()) );

			// get the named object
			pins_activate = wWorld.ppartPartitionList()->
				pinsFindNamedInstance(pestr->strData());
		}
	}
	END_OBJECT;
	END_TEXT_PROCESSING;

	// we must have a trigger condition set.
	Assert( tco_trigger != TRIGGER_COND_NEVER);

	tco_TriggerConditions = tco_trigger;
	u4_CreatureEnterTriggerCount=i_enter_count;
	u4_CreatureLeaveTriggerCount=i_leave_count;

	u4_CreatureEnterCount=0;
	u4_CreatureLeaveCount=0;

	// we originall contain no instances
	for (uint32 u4 = 0; u4<u4TRIGGER_MAX_CONTAINMENT; u4++)
	{
		apinsContained[u4] = NULL;
	}

	// we cannot get the active object at the moment
	pinsActivate = pins_activate;


#if VER_TEST

	if (eapList>=eapSEQUENCEORDER)
	{
		if ( ((tco_TriggerConditions & (TRIGGER_COND_PLAYER_MOVE_INSIDE|TRIGGER_COND_PLAYER_MOVE_OUTSIDE|TRIGGER_COND_PLAYER_ALWAYS)) == 0) ^ 
			(pinsActivate!=NULL) )
		{
			AlwaysAssert(0);
		}
	}

#endif

	// Register this entity with the message types it needs to receive.
	// Note that move messages are handled separately and do not need to be registered.
	         CMessageStep::RegisterRecipient(this);
	CMessageMoveTriggerTo::RegisterRecipient(this);
}


//*********************************************************************************************
CInstance* CLocationTrigger::pinsCopy() const
{
	AlwaysAssert(!pinfoProperties->setUnique[CInfo::eitTHIS]);

	CLocationTrigger* plt = new CLocationTrigger();

	*plt = *this;
	return plt;
}

//*********************************************************************************************
CLocationTrigger::~CLocationTrigger()
{
	CMessageMoveTriggerTo::UnregisterRecipient(this);
	         CMessageStep::UnregisterRecipient(this);
}


//*********************************************************************************************
// Process the CMessageStep message for this location trigger, remember that the base class
// must be called
//
void CLocationTrigger::Process(const CMessageStep& msg)
{
	// If this trigger has any of the 'always' fire conditions then we need to process a little
	// more information.
	if (tco_TriggerConditions & (TRIGGER_COND_PLAYER_ALWAYS|TRIGGER_COND_OBJECT_ALWAYS|TRIGGER_COND_CREATURE_ALWAYS))
	{
		if (pinsActivate)
		{
			if (bContained(pinsActivate))
				AttemptTriggerFire();
		}
		else
		{
			TCondition	tco_fire = 0;

			// we need to go through the list and determine what each object is
			for (uint32 u4 = 0; u4 < u4TRIGGER_MAX_CONTAINMENT; u4++)
			{
				if (apinsContained[u4])
				{
					EInstanceType eit_type = eitInstanceType(apinsContained[u4]);

					if (eit_type == eINSTANCE_CREATURE)
					{
						tco_fire |= TRIGGER_COND_CREATURE_ALWAYS;
					}
					else if (eit_type == eINSTANCE_PLAYER)
					{
						tco_fire |= TRIGGER_COND_PLAYER_ALWAYS;
					}
					else if (eit_type == eINSTANCE_OBJECT)
					{
						tco_fire |= TRIGGER_COND_OBJECT_ALWAYS;
					}
				}
			}

			if (tco_TriggerConditions & tco_fire)
				AttemptTriggerFire();
		}
	}

	// Call the base class
	CTrigger::Process(msg);
}


//*********************************************************************************************
void CLocationTrigger::Process(const CMessageMove& msg)
{
	CTimeBlock tmb(&psMoveMsgTrigger);

	// if we are not in play mode then do not fire any triggers. Dervided classes may require
	// to also check this variable so to keep their data in step.
	if (!CMessageSystem::bSimulationGoing())
		return;

	//
	// this function only checks flags that are unique to this trigger
	// Trigger base condtitions like timers and probabilities are handlied
	// inside the trigger base class function AttemptTriggerFire()
	//

	if (msg.etType != CMessageMove::etMOVED)
		return;

	CInstance*	pins=msg.pinsMover;

	// do we have a specific activation instance (creature/object/player)?????
	if (pinsActivate != NULL)
	{
		// yes, check if it generated this message
		if (pins != pinsActivate )
		{
			return;
		}
	}

	if (Evaluate(pins))
		AttemptTriggerFire();
}


//*********************************************************************************************
//
/*void CLocationTrigger::Process(const CMessageHere& msg)
{
	//
	// this function only checks flags that are unique to this trigger
	// trigger wide condtitions like timers and probabilities are handlied
	// inside the trigger base class function AttemptTriggerFire()
	//

	if (u4_Probability == 0)
		return;

	CEntity*	petSender=msg.petGetSender();

	// do we have a specific activation instance (creature/object/player)?????
	if (pinsActivate != NULL)
	{
		// yes, check if it generated this message
		if ((CInstance*)petSender != pinsActivate )
		{
			return;
		}
	}

	if (Evaluate(petSender))
		AttemptTriggerFire();
}*/


//******************************************************************************************
// Location trigger functions to control what is contained within the trigger
//
bool CLocationTrigger::bContained
(
	const CInstance* pins
)
//
//**************************************
{
	for (uint32 u4=0; u4<u4TRIGGER_MAX_CONTAINMENT; u4++)
	{
		if (apinsContained[u4] == pins)
			return TRUE;
	}
	return false;
}


//******************************************************************************************
//
void CLocationTrigger::SetContained
(
	const CInstance* pins
)
//
//**************************************
{
	for (uint32 u4=0; u4<u4TRIGGER_MAX_CONTAINMENT; u4++)
	{
		if (apinsContained[u4] == NULL)
		{
			apinsContained[u4] = pins;
			return;
		}
	}

	// failed to find an empty slot in the trigger containment list
	AlwaysAssert(0);
}


//******************************************************************************************
//
void CLocationTrigger::RemoveContained
(
	const CInstance* pins
)
//
//**************************************
{
	for (uint32 u4=0; u4<u4TRIGGER_MAX_CONTAINMENT; u4++)
	{
		if (apinsContained[u4] == pins)
		{
			apinsContained[u4] = NULL;
			return;
		}
	}

	// failed to find the instance in the trigger containment list
	AlwaysAssert(0);
}


//*********************************************************************************************
// Determine weather the trigger is currently in a firing state
//
bool CLocationTrigger::bEvaluateNow()
{
	// If the simulation is not going then we must never evaluate to true
	if (!CMessageSystem::bSimulationGoing())
		return false;

	if (pinsActivate)
	{
		// if the activate object is set then we only fire if that object is contained within
		// the trigger.
		return bContained(pinsActivate);
	}
	else
	{
		//
		// At the moment we can query for a creature being in a trigger but we cannoy query
		// for a number of creatures being in the trigger.
		//

		// Go through the triggers containment list to see what we contain.
		for (uint32 u4 = 0; u4 < u4TRIGGER_MAX_CONTAINMENT; u4++)
		{
			if (apinsContained[u4])
			{
				EInstanceType eit_type = eitInstanceType(apinsContained[u4]);

				// the activate object is not set so we fire depending on the state of the other
				// trigger variables.
				if (tco_TriggerConditions & (TRIGGER_COND_PLAYER_MOVE_INSIDE|TRIGGER_COND_PLAYER_MOVE_OUTSIDE|TRIGGER_COND_PLAYER_ALWAYS))
				{
					// we are looking for the player in this trigger
					if (eit_type == eINSTANCE_PLAYER)
					{
						return true;
					}
				}

				if (tco_TriggerConditions & (TRIGGER_COND_CREATURE_MOVE_INSIDE|TRIGGER_COND_CREATURE_MOVE_OUTSIDE|TRIGGER_COND_CREATURE_ALWAYS))
				{
					// we are looking for a creature in this trigger
					if (eit_type == eINSTANCE_CREATURE)
					{
						return true;
					}
				}

				if (tco_TriggerConditions & (TRIGGER_COND_OBJECT_MOVE_INSIDE|TRIGGER_COND_OBJECT_MOVE_OUTSIDE|TRIGGER_COND_OBJECT_ALWAYS))
				{
					// we are looking for an object, so if it is not a player and not an animal then it must be an object.
					if (eit_type == eINSTANCE_OBJECT)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


//*********************************************************************************************
EInstanceType CLocationTrigger::eitInstanceType(const CInstance* pins)
{
	if (ptCast<CAnimal>((CInstance*)pins))
	{
		return eINSTANCE_CREATURE;
	}
	else if (ptCast<CPlayer>((CInstance*)pins))
	{
		return eINSTANCE_PLAYER;
	}

	// Get the physics info.
	const CPhysicsInfo* pphi = pins->pphiGetPhysicsInfo();

	// Check for tangible moveable objects.
	if (pphi && pphi->epfPhysicsFlags()[epfTANGIBLE] && pphi->epfPhysicsFlags()[epfMOVEABLE])
	{
		return eINSTANCE_OBJECT;
	}
	else
	{
		return eINSTANCE_IGNORE;
	}
}


//*********************************************************************************************
// Evaluate whether any of the trigger conditions are met
//
bool CLocationTrigger::Evaluate
(
	const CInstance* pins
)
//
//**************************************
{
	Assert( CMessageSystem::bSimulationGoing() );

	// Before we evaluate the trigger lets check what the type of object is that is moving, if
	// it is something that we do not care about then we need not do nay more. This will save us
	// filling the containment list with junk and having to search through it every time.
	EInstanceType eit_type = eitInstanceType(pins);

	if (eit_type == eINSTANCE_CREATURE)
	{
		// The type of instance is a creature. If this trigger does not care about creatures then
		// do nothing.
		if ((tco_TriggerConditions & (TRIGGER_COND_CREATURE_MOVE_INSIDE|TRIGGER_COND_CREATURE_MOVE_OUTSIDE|TRIGGER_COND_CREATURE_ALWAYS|TRIGGER_COND_CREATURE_ENTER_COUNT|TRIGGER_COND_CREATURE_LEAVE_COUNT)) == 0)
			return false;
	}
	else if (eit_type == eINSTANCE_PLAYER)
	{
		// The type of instance is a player. If this trigger does not care about players then
		// do nothing.
		if ((tco_TriggerConditions & (TRIGGER_COND_PLAYER_MOVE_INSIDE|TRIGGER_COND_PLAYER_MOVE_OUTSIDE|TRIGGER_COND_PLAYER_ALWAYS)) == 0)
			return false;
	}
	else
	{
		// The type of instance is an object. If this trigger does not care about objects then
		// do nothing.
		if ((tco_TriggerConditions & (TRIGGER_COND_OBJECT_MOVE_INSIDE|TRIGGER_COND_OBJECT_MOVE_OUTSIDE|TRIGGER_COND_OBJECT_ALWAYS)) == 0)
			return false;
	}


	CVector3<>	pos = pins->v3Pos();
	
#ifdef EXTRA_CHECK		// Actually, this safety check is unnecessary, because BV intersections always do a sphere test, first.
						// Also, if we trivially reject here, we miss some essential logic below.
	//
	// Before we evaluate the trigger make sure we are close enough to it, this is to save
	// the evaluate function calculating the bounding volume containment for triggers we
	// are not close to.
	//

	CVector3<>	mypos = v3Pos();

	// the radius of the bounding sphere, if we are closer than 3 times this distance we will
	// consider the trigger. This distance has to be bigger than  the trigger otherwise we
	// cannot leave the trigger.
	TReal		r_radius2 = rGetCylinderRadiusSqr() * 3.0f;
	TReal		r_dist2   = ((mypos.tX - pos.tX) * (mypos.tX - pos.tX)) +
							((mypos.tY - pos.tY) * (mypos.tY - pos.tY)) +
							((mypos.tZ - pos.tZ) * (mypos.tZ - pos.tZ));

	// we are not close to the trigger so do not process it.
	if (r_dist2>r_radius2)
		return false;
#endif  // EXTRA_CHECK

	bool b_inside = bContained(pins);
	bool b_in;

	// If this is a point trigger we must fire on the origin of the moving object, once this
	// enters the trigger is fires.
	// If we are not a point trigger then as soon as the bounding volume of the moving object
	// intersects the trigger it fires.
	if (bPointTrigger)
	{		
		pos *= ~pr3Presence();


		// Are we in or out?
		// Transform the object's position into trigger's coord frame.
		//CVector3<> pos = pins->v3Pos() - v3Pos();
		b_in = bv_BoundingVolume->bContains(pos);
	}
	else
	{
		Assert(pins->pphiGetPhysicsInfo());
		b_in = bv_BoundingVolume->bIntersects( *(pins->pphiGetPhysicsInfo()->pbvGetBoundVol()), &pr3Presence(), &pins->pr3Presence() );
	}

	// if both are 0 we have stayed outside so no conditions are met.
	// if both are 1 we have stayed inside... we can only match one of the always conditions.
	// The always conditions are handled by the step message because when objects go to sleep then
	// do not send move messages so the trigger will stop firing if this case is handled here.
	if ((b_inside ^ b_in) == false)
	{
		return false;
	}

	TCondition	tco_result = TRIGGER_COND_NEVER;

	// If come inside than we must add it to our list ...
	if (!b_inside && b_in)
	{
		SetContained(pins);
		if (eit_type == eINSTANCE_CREATURE)
		{
			// maintain the creature counters for the trigger and
			// trigger if required.
			u4_CreatureEnterCount++;
			if (u4_CreatureEnterCount >= u4_CreatureEnterTriggerCount)
			{
				tco_result|=TRIGGER_COND_CREATURE_ENTER_COUNT;
				u4_CreatureEnterCount=0;				// reset the counter
			}
		}
	}
	else
	{
		// ... else if it went outside than we must take it off our list.
		RemoveContained(pins);
		if (eit_type == eINSTANCE_CREATURE)
		{
			// it is a creature, check the neumber that have left
			u4_CreatureLeaveCount++;
			if (u4_CreatureLeaveCount >= u4_CreatureLeaveTriggerCount)
			{
				tco_result|=TRIGGER_COND_CREATURE_LEAVE_COUNT;
				u4_CreatureLeaveCount=0;				// reset the counter
			}
		}

		// On triggers that can be activated by multiple objects this flag will always be false
		// as it will assert on load.
		// On triggers that are either player triggers or triggers that are attached to an object, it
		// must have been this object that has just left the trigger so it is safe to blindy clear the
		// sequenced flag.

		//
		// We only clear the sequence flag for looped sequences. Sequcnes which are not looped will
		// continue being sequenced by the base trigger until the end of the sequence.
		//
		if ((eapList == eapSEQUENCEORDERLOOP) || (eapList == eapSEQUENCERANDOMLOOP))
		{
			bSequenceActions = false;
		}		
	}

	// check what has moved across the boundary of the trigger
	switch (eit_type)
	{
	case eINSTANCE_CREATURE:
			if (b_in) 
			{
				return (tco_result | TRIGGER_COND_CREATURE_MOVE_INSIDE) & tco_TriggerConditions;
			}
			else
			{
				return (tco_result | TRIGGER_COND_CREATURE_MOVE_OUTSIDE) & tco_TriggerConditions;
			}
		break;

	case eINSTANCE_PLAYER:
			if (b_in) 
			{
				return (tco_result | TRIGGER_COND_PLAYER_MOVE_INSIDE) & tco_TriggerConditions;
			}
			else
			{
				return (tco_result | TRIGGER_COND_PLAYER_MOVE_OUTSIDE) & tco_TriggerConditions;
			}
		break;

	default:
			if (b_in) 
			{
				return (tco_result | TRIGGER_COND_OBJECT_MOVE_INSIDE) & tco_TriggerConditions;
			}
			else
			{
				return (tco_result | TRIGGER_COND_OBJECT_MOVE_OUTSIDE) & tco_TriggerConditions;
			}
		break;
	}

	// if we get to here something is horribly wrong!!!!!!
	Assert(0);
	return false;
}




//*********************************************************************************************
// Location triggers need to override this message from the base class because a location
// trigger needs to recalculate its contents before it moves the trigger. Doing this could
// cause the trigger to fire (obejcts would in effect be leaving or entering).
void CLocationTrigger::Process(const CMessageMoveTriggerTo& msgmtrigto)
{
	if (u4_Probability == 0)
		return;

	// TODO:
	// Implement the move trigger code.....

	// must call the base class so that the trigger really gets moved and the database gets
	// updated...
	CTrigger::Move(msgmtrigto.p3NewPosition);
}


/*
// Fields saved for CLocationTrigger.
	const CInstance*	apinsContained[u4TRIGGER_MAX_CONTAINMENT];
	uint32		u4_CreatureEnterCount;			// number of CREATURES that have entered
	uint32		u4_CreatureLeaveCount;			// number of CREATURES that have left
  */

//*****************************************************************************************
char * CLocationTrigger::pcSave(char *  pc) const
{
	pc = CTrigger::pcSave(pc);

	pc = pcSaveT(pc, u4_CreatureEnterCount);
	pc = pcSaveT(pc, u4_CreatureLeaveCount);

	for (int i = 0; i < u4TRIGGER_MAX_CONTAINMENT; ++i)
	{
		pc = pcSaveInstancePointer(pc, apinsContained[i]);
	}	

	return pc;
}

//*****************************************************************************************
const char * CLocationTrigger::pcLoad(const char *  pc)
{
	pc = CTrigger::pcLoad(pc);

	pc = pcLoadT(pc, &u4_CreatureEnterCount);
	pc = pcLoadT(pc, &u4_CreatureLeaveCount);

	for (int i = 0; i < u4TRIGGER_MAX_CONTAINMENT; ++i)
	{
		pc = pcLoadInstancePointer(pc, (CInstance**)&apinsContained[i]);
	}	

	return pc;
}

#if VER_TEST
//*****************************************************************************************
int CLocationTrigger::iGetDescription(char* pc_buffer, int i_buffer_len)
{
	CTrigger::iGetDescription(pc_buffer, i_buffer_len);

	char str_buffer[512];

	strcat(pc_buffer, "\nCLocationTrigger:\n");

	sprintf(str_buffer,"u4_CreatureEnterCount\t%d\n", u4_CreatureEnterCount);
	strcat(pc_buffer, str_buffer);

	sprintf(str_buffer,"u4_CreatureEnterTriggerCount\t%d\n", u4_CreatureEnterTriggerCount);
	strcat(pc_buffer, str_buffer);

	sprintf(str_buffer,"u4_CreatureLeaveCount\t%d\n", u4_CreatureLeaveCount);
	strcat(pc_buffer, str_buffer);

	sprintf(str_buffer,"u4_CreatureLeaveTriggerCount\t%d\n", u4_CreatureLeaveTriggerCount);
	strcat(pc_buffer, str_buffer);

	sprintf(str_buffer,"pinsActivate\t%s\n", pinsActivate ? pinsActivate->strGetInstanceName() : "Any");
	strcat(pc_buffer, str_buffer);

	int i_len = strlen(pc_buffer);

	iGetConditionDescription(tco_TriggerConditions, pc_buffer + i_len, i_buffer_len - i_len);

	i_len = strlen(pc_buffer);
	Assert(i_len < i_buffer_len);
	return i_len;
}
#endif //VER_TEST



//*****************************************************************************************
bool CLocationTrigger::bValidateTriggerProperties
(
	const CGroffObjectName*	pgon,		// Object to load.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	CLoadWorld*				p_load		// The loader.
)
{
	return CTrigger::bValidateTriggerProperties(pgon, h_object,pvtable,p_load);
}
