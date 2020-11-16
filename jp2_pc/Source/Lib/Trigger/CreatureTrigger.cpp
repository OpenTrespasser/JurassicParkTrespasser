/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of CCreatureTrigger defined in Trigger.hpp
 *
 * Contents:
 *		CCreatureTrigger
 *
 * Bugs:
 *
 * To do:
 *		Implement text prop reading when we know what they are
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/CreatureTrigger.cpp                                      $
 * 
 * 12    10/01/98 4:05p Mlange
 * Improved move message stat reporting.
 * 
 * 11    8/12/98 7:00p Mlange
 * CMessageDeath and CMessageDamage now require subscription of the recipients.
 * 
 * 10    8/11/98 8:29p Mlange
 * Entities must now register themselves with each message type in which they are interested.
 * 
 * 9     8/04/98 9:31a Agrant
 * don't assert on SimulationGoing,  just return
 * 
 * 8     98/07/30 22:21 Speter
 * Added stats.
 * 
 **********************************************************************************************/

#include "Trigger.hpp"
#include <algorithm>

#include "Action.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgTrigger.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Animate.hpp"
#include "Lib/Loader/SaveBuffer.hpp"

#include "Lib/Sys/Profile.hpp"

extern CProfileStat psMoveMsgTrigger;


//*********************************************************************************************
//
// CCreatureTrigger implementation.
//


//******************************************************************************************
CCreatureTrigger::CCreatureTrigger
(
	const CGroffObjectName*	pgon,		// Object to load.
	CLoadWorld*				p_load,		// The loader.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
) : CTrigger(pgon, p_load, h_object, pvtable, pinfo)
{
	const CEasyString*	pestr = 0;

	tcoTriggerConditions=TRIGGER_COND_NEVER;
	fDamagePoints = 0;

	// we make sure the creature list is empty
	for (uint32 u4 = 0; u4<u4TRIGGER_MAX_CREATURES; u4++)
	{
		au4Creatures[u4] = 0;
	}

	SETUP_TEXT_PROCESSING(pvtable, p_load)
	SETUP_OBJECT_HANDLE(h_object)
	{
		uint32			u4_count = 0;

		// Look for the dino array
		while ( bFILL_pEASYSTRING(pestr, (ESymbol)((uint32)esA00+u4_count)) )
		{
			// Hash the name to an instance handle and add it to the array
			if (pestr)
				AddCreature(u4Hash(pestr->strData(),0,true));

			u4_count++;
		}

		// Did we get an array element??
		Assert(u4_count>0);
	}

	bool b_local;

	if (bFILL_BOOL(b_local, esCreatureDie))
	{
		if (b_local)
		{
			tcoTriggerConditions |= TRIGGER_COND_CREATURE_DIED;
		}
	}

	if (bFILL_BOOL(b_local, esCreatureWake))
	{
		if (b_local)
		{
			tcoTriggerConditions |= TRIGGER_COND_CREATURE_WOKEN;
		}
	}

	if (bFILL_BOOL(b_local, esCreatureSleep))
	{
		if (b_local)
		{
			tcoTriggerConditions |= TRIGGER_COND_CREATURE_SLEEP;
		}
	}

	if (bFILL_FLOAT(fDamagePoints, esCreatureDamagePoints))
	{
		tcoTriggerConditions |= TRIGGER_COND_CREATURE_DAMAGE;
	}

	if (bFILL_BOOL(b_local, esCreatureCriticalDamage))
	{
		if (b_local)
		{
			// If we are looking for critical damage then ignore normal damage
			tcoTriggerConditions |= TRIGGER_COND_CREATURE_CRITDAMAGE;
			tcoTriggerConditions &= ~TRIGGER_COND_CREATURE_DAMAGE;
		}
	}

	bEvaluateAll = false;
	bFILL_BOOL(bEvaluateAll, esEvaluateAll);

	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	// We must have some condition set
	Assert(tcoTriggerConditions!=TRIGGER_COND_NEVER);

	// if we have a damage condition then we must have set th hit points
	if (tcoTriggerConditions & (TRIGGER_COND_CREATURE_CRITDAMAGE|TRIGGER_COND_CREATURE_DAMAGE))
	{
		Assert(fDamagePoints>=0.0f);
	}

	  CMessageMove::RegisterRecipient(this);
	 CMessageDeath::RegisterRecipient(this);
	CMessageDamage::RegisterRecipient(this);
}



//*********************************************************************************************
CCreatureTrigger::~CCreatureTrigger()
{
	CMessageDamage::UnregisterRecipient(this);
	 CMessageDeath::UnregisterRecipient(this);
	  CMessageMove::UnregisterRecipient(this);
}


//*********************************************************************************************
void CCreatureTrigger::Process(const CMessageDeath& msg)
{
	// The simulation must be going before we can get a collision message, if this is not the
	// case then something is seriously wrong.
	Assert(CMessageSystem::bSimulationGoing());

	//
	// This trigger has zero probabilty so do nothing. Triggers that keep local state should no do this
	//
	if (u4_Probability == 0)
		return;

	// if we have the death bit set then check the list to see if we are in it
	if (tcoTriggerConditions & TRIGGER_COND_CREATURE_DIED)
	{
		if (bInList( msg.paniDyingThing->u4GetUniqueHandle() ))
		{
			AttemptTriggerFire();
		}
	}
}


//*********************************************************************************************
void CCreatureTrigger::Process(const CMessageMove& msg)
{
	// The simulation must be going before we can get a move message, if this is not the
	// case then we should not trigger!
	if (!CMessageSystem::bSimulationGoing())
		return;

	
	CTimeBlock tmb(&psMoveMsgTrigger);

	TCondition	tco_type = 0;


	//
	// This trigger has zero probabilty so do nothing. Triggers that keep local state should no do this
	//
	if (u4_Probability == 0)
		return;

	// what type of message is this, critical or not
	if (msg.etType == CMessageMove::etAWOKE )
	{
		tco_type=TRIGGER_COND_CREATURE_WOKEN;
	}
	else if (msg.etType == CMessageMove::etSLEPT )
	{
		tco_type=TRIGGER_COND_CREATURE_SLEEP;
	}

	// if we have the wake/sleep bit set then check the creature list
	if (tcoTriggerConditions & tco_type)
	{
		if (bInList( msg.pinsMover->u4GetUniqueHandle() ))
		{
			AttemptTriggerFire();
		}
	}
}


//*********************************************************************************************
void CCreatureTrigger::Process(const CMessageDamage& msg)
{
	CInstance*	pins_damaged = msg.pinsGetDamaged();
	TCondition	tco_type;

	// The simulation must be going before we can get a collision message, if this is not the
	// case then something is seriously wrong.
	Assert(CMessageSystem::bSimulationGoing());

	//
	// This trigger has zero probabilty so do nothing. Triggers that keep local state should no do this
	//
	if (u4_Probability == 0)
		return;

	// what type of message is this, critical or not
	if (msg.bGetDamageType())
	{
		tco_type=TRIGGER_COND_CREATURE_CRITDAMAGE;
	}
	else
	{
		tco_type=TRIGGER_COND_CREATURE_DAMAGE;
	}

	// if we have any damage bits set bit set then fire the trigger
	if (tcoTriggerConditions & tco_type)
	{
		// have we suffered enough damage??
		if (msg.fGetDamage()>fDamagePoints)
		{
			// are we in this triggers fire list
			if (bInList( pins_damaged->u4GetUniqueHandle() ))
			{
				AttemptTriggerFire();
			}
		}
	}
}


//*********************************************************************************************
// adds a creature to this trigger, the trigger will fire if the sender of
// a received message is in the list and the conditions are met.
//
void CCreatureTrigger::AddCreature
(
	uint32 u4_creature
)
//
//**************************************
{
	// add the creature to the current list for this trigger
	for (uint32 u4=0; u4<u4TRIGGER_MAX_CREATURES; u4++)
	{
		if (au4Creatures[u4] == 0)
		{
			au4Creatures[u4] = u4_creature;
			return;
		}
		
	}

	// failed to find an empty slot in the trigger containment list
	Assert(0);
}



//*********************************************************************************************
// remove a creature from the list for this trigger
//
bool CCreatureTrigger::bInList
(
	uint32 u4_creature
)
//
//**************************************
{
	// add the creature to the current list for this trigger
	for (uint32 u4=0; u4<u4TRIGGER_MAX_CREATURES; u4++)
	{
		if (au4Creatures[u4] == u4_creature)
			return true;
	}
	return false;
}



//*****************************************************************************************
char * CCreatureTrigger::pcSave(char *  pc) const
{
	return CTrigger::pcSave(pc);
}


//*****************************************************************************************
const char * CCreatureTrigger::pcLoad(const char *  pc)
{
	return CTrigger::pcLoad(pc);
}


#if VER_TEST
//*****************************************************************************************
int CCreatureTrigger::iGetDescription(char* pc_buffer, int i_buffer_len)
{
	char str_buffer[1024];
	CTrigger::iGetDescription(pc_buffer, i_buffer_len);

	strcat(pc_buffer, "\nCCreatureTrigger:\n");

	strcat(pc_buffer, "Active Creatures\n\n");
	for (int i = 0; i < u4TRIGGER_MAX_CREATURES; ++i)
	{
		if (au4Creatures[i])
		{
			char* str_name = CInstance::strInstanceNameFromHandle(au4Creatures[i]);
			sprintf(str_buffer,"Creature Handle 2\t%d (%s)\n", au4Creatures[i], str_name?str_name:"No name");
			strcat(pc_buffer, str_buffer);
		}
	}

	sprintf(str_buffer,"Damage points = %f\n",fDamagePoints);
	strcat(pc_buffer,str_buffer);

	int i_len = strlen(pc_buffer);
	iGetConditionDescription(tcoTriggerConditions, pc_buffer + i_len, i_buffer_len - i_len);

	i_len = strlen(pc_buffer);
	Assert(i_len < i_buffer_len);
	return i_len;
}
#endif //VER_TEST



//*****************************************************************************************
bool CCreatureTrigger::bValidateTriggerProperties
(
	const CGroffObjectName*	pgon,		// Object to load.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	CLoadWorld*				p_load		// The loader.
)
{
	return CTrigger::bValidateTriggerProperties(pgon, h_object,pvtable,p_load);
}



//*****************************************************************************************
bool CCreatureTrigger::bEvaluateNow()
{
	// You can only evaluate a creature trigger that is a die trigger.
	Assert( (tcoTriggerConditions & TRIGGER_COND_CREATURE_DIED) == TRIGGER_COND_CREATURE_DIED );

	CInstance*	pins;
	bool		b_res = false;

	for (uint32 u4=0; u4<u4TRIGGER_MAX_CREATURES; u4++)
	{
		if (au4Creatures[u4])
		{
			pins = pwWorld->ppartTriggerPartitionList()->pinsFindInstance( au4Creatures[u4] );
			
			// we must find the creature...
			Assert(pins);

			CAnimate*	pani = ptCast<CAnimate>(pins);
			Assert(pani);

			if (pani->bDead())
			{
				b_res = true;
			}
			else
			{
				// this creature is not dead so the trigger cannot evaluate to true if it is
				// set to evaluate all creatures.
				if (bEvaluateAll)
					return false;
			}
		}
		else
		{
			// once we find an empty element stop, creatures can never be removed so there
			// can never be any gaps in the sequence...
			break;
		}
	}	
	
	return b_res;
}

