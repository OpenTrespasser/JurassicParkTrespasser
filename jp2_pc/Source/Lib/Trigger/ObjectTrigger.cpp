/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of CObjectTrigger defined in Trigger.hpp
 *
 * Contents:
 *		CObjectTrigger
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
 * $Log:: /JP2_PC/Source/Lib/Trigger/ObjectTrigger.cpp                                        $
 * 
 * 8     98/08/13 17:02 Speter
 * Updated for CMessagePickUp action types.
 * 
 * 7     8/12/98 6:02p Mlange
 * CMessagePickUp and CMessageUse now require registration of the listeners.
 * 
 * 6     7/24/98 12:32a Rwyatt
 * First functional object trigger
 * 
 * 5     7/20/98 11:27p Rwyatt
 * Added validation to all triggers
 * All changable data is now saved in the scene file
 * 
 * 4     98/02/26 15:40 Speter
 * Removed Hack.hpp.
 * 
 * 3     12/15/97 3:04p Agrant
 * Trigger descriptions
 * 
 **********************************************************************************************/

#include "Trigger.hpp"
#include <algorithm>

#include "Action.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgTrigger.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Groff/VTParse.hpp"



//*********************************************************************************************
//
// CObjectTrigger implementation.
//


//*********************************************************************************************
CObjectTrigger::CObjectTrigger
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

	// we make sure the object list is empty
	for (uint32 u4 = 0; u4<u4TRIGGER_MAX_OBJECTS; u4++)
	{
		au4Objects[u4] = 0;
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
				AddObject(u4Hash(pestr->strData(),0,true));

			u4_count++;
		}

		// Did we get an array element??
		Assert(u4_count>0);
	}

	bool b_local;

	if (bFILL_BOOL(b_local, esUseObject))
	{
		if (b_local)
		{
			tcoTriggerConditions |= TRIGGER_COND_OBJECT_USED;
		}
	}

	if (bFILL_BOOL(b_local, esPickUpObject))
	{
		if (b_local)
		{
			tcoTriggerConditions |= TRIGGER_COND_OBJECT_PICKUP;
		}
	}

	if (bFILL_BOOL(b_local, esPutDownObject))
	{
		if (b_local)
		{
			tcoTriggerConditions |= TRIGGER_COND_OBJECT_PUTDOWN;
		}
	}
	END_OBJECT;    //poval_action
	END_TEXT_PROCESSING;

	// We must have some condition set
	Assert(tcoTriggerConditions!=TRIGGER_COND_NEVER);

	// Register this entity with the message types it needs to receive.
	CMessagePickUp::RegisterRecipient(this);
	   CMessageUse::RegisterRecipient(this);
}


//*********************************************************************************************
CObjectTrigger::~CObjectTrigger()
{
	   CMessageUse::UnregisterRecipient(this);
	CMessagePickUp::UnregisterRecipient(this);
}


//*********************************************************************************************
// The object trigger must process the pickup message.
void CObjectTrigger::Process(const CMessagePickUp& msgpu)
{
	TCondition	tco_action;

	if (u4_Probability == 0)
		return;

	switch (msgpu.epuAction())
	{
		case epuPICKUP:
		case epuRETRIEVE:
			tco_action = TRIGGER_COND_OBJECT_PICKUP;
			break;
		case epuDROP:
		case epuSTOW:
			tco_action = TRIGGER_COND_OBJECT_PUTDOWN;
			break;
	}

	// if we have the death bit set then fire the trigger
	if (tcoTriggerConditions & tco_action)
	{
		if (bInList( msgpu.pinsGetMessageObject()->u4GetUniqueHandle() ))
		{
			AttemptTriggerFire();
		}
	}
}



//*********************************************************************************************
void CObjectTrigger::Process(const CMessageUse& msguse)
{
	if (u4_Probability == 0)
		return;

	// if we have the death bit set then fire the trigger
	if (tcoTriggerConditions & TRIGGER_COND_OBJECT_USED)
	{
		if (bInList( msguse.pinsGetMessageObject()->u4GetUniqueHandle() ))
		{
			AttemptTriggerFire();
		}
	}
}



//*********************************************************************************************
// add an objct to the list that this trigger processes
//
void CObjectTrigger::AddObject
(
	uint32	u4_hash
)
//
//**************************************
{
	// add the creature to the current list for this trigger
	for (uint32 u4=0; u4<u4TRIGGER_MAX_OBJECTS; u4++)
	{
		if (au4Objects[u4] == 0)
		{
			au4Objects[u4] = u4_hash;
			return;
		}
		
	}

	// failed to find an empty slot in the trigger containment list
	Assert(0);
}



//*********************************************************************************************
// remove a creature from the list for this trigger
//
bool CObjectTrigger::bInList
(
	uint32 u4_hash
)
//
//**************************************
{
	// add the creature to the current list for this trigger
	for (uint32 u4=0; u4<u4TRIGGER_MAX_OBJECTS; u4++)
	{
		if (au4Objects[u4] == u4_hash)
			return true;
	}
	return false;
}


//*****************************************************************************************
char * CObjectTrigger::pcSave(char *  pc) const
{
	return CTrigger::pcSave(pc);
}


//*****************************************************************************************
const char * CObjectTrigger::pcLoad(const char *  pc)
{
	return CTrigger::pcLoad(pc);
}


#if VER_TEST
//*****************************************************************************************
int CObjectTrigger::iGetDescription(char* pc_buffer, int i_buffer_len)
{
	char str_buffer[1024];
	CTrigger::iGetDescription(pc_buffer, i_buffer_len);

	strcat(pc_buffer, "\nCObjectTrigger:\n");

	strcat(pc_buffer, "Activatee Objects\n\n");
	for (int i = 0; i < u4TRIGGER_MAX_OBJECTS; ++i)
	{
		if (au4Objects[i])
		{
			char* str_name = CInstance::strInstanceNameFromHandle(au4Objects[i]);
			sprintf(str_buffer,"Object Handle 2\t%d (%s)\n", au4Objects[i], str_name?str_name:"No name");
			strcat(pc_buffer, str_buffer);
		}
	}

	int i_len = strlen(pc_buffer);

	iGetConditionDescription(tcoTriggerConditions, pc_buffer + i_len, i_buffer_len - i_len);

	i_len = strlen(pc_buffer);
	Assert(i_len < i_buffer_len);
	return i_len;
}
#endif


//*****************************************************************************************
bool CObjectTrigger::bValidateTriggerProperties
(
	const CGroffObjectName*	pgon,		// Object to load.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	CLoadWorld*				p_load		// The loader.
)
{
	return CTrigger::bValidateTriggerProperties(pgon, h_object,pvtable,p_load);
}
