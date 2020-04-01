/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of CStartTrigger defined in Trigger.hpp
 *
 * Contents:
 *		CStartTrigger
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/StartTrigger.cpp                                       $
 * 
 * 10    9/25/98 11:54a Mlange
 * Now listens for scene file loaded message instead of GROFF file loaded.
 * 
 * 9     98.09.19 1:24a Mmouni
 * Now Unregisters itself as well.
 * 
 * 8     9/19/98 12:09a Mmouni
 * Now registers for CMessageSystem.
 * 
 * 7     98.09.18 12:28p Mmouni
 * The start trigger now listens to the CMessageSystem GROFF_LOADED message.
 * 
 * 6     8/13/98 1:43p Mlange
 * The step message now requires registration of its recipients.
 * 
 **********************************************************************************************/

#include "Trigger.hpp"
#include <algorithm>

#include "Action.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgTrigger.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/GeomDBase/Partition.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Loader/SaveBuffer.hpp"


//*********************************************************************************************
//
// CStartTrigger implementation.
//
std::list<CStartTrigger*> CStartTrigger::listStartTriggers;


//*********************************************************************************************
// Constructs a CStartTrigger directly from the GROFF info.
//
CStartTrigger::CStartTrigger
(
	const CGroffObjectName*	pgon,		// Object to load.
	CLoadWorld*				p_load,		// The loader.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
) : CTrigger(pgon, p_load, h_object, pvtable, pinfo)
{
	Assert( h_object != hNULL_HANDLE);

	// Clear fired flag.
	bFired = false;

	// Add to list.
	listStartTriggers.push_front(this);

	// Register this entity with the message types it needs to receive.
	CMessageStep::RegisterRecipient(this);
	CMessageSystem::RegisterRecipient(this);
}


//*********************************************************************************************
CStartTrigger::~CStartTrigger()
{
	CMessageStep::UnregisterRecipient(this);
	CMessageSystem::UnregisterRecipient(this);

	std::list<CStartTrigger*>::iterator it_me = find(listStartTriggers.begin(),
											listStartTriggers.end(), this);

	Assert(it_me != listStartTriggers.end());

	listStartTriggers.erase(it_me);
}


//******************************************************************************************
void CStartTrigger::ResetStartTriggers()
{
	for (std::list<CStartTrigger*>::iterator it = listStartTriggers.begin();
		 it != listStartTriggers.end(); it++)
	{
		// Cleare fired flag.
		(*it)->bFired = false;
	}
}


//*********************************************************************************************
// Process the CMessageStep message for this trigger entity
void CStartTrigger::Process(const CMessageStep& msgstep)
{
	if (u4_Probability == 0)
		return;

	if (!CMessageSystem::bSimulationGoing())
		return;

	if (!bFired)
	{
		bFired = true;
		Trigger();
	}
}

//*********************************************************************************************
// Process the CMessageSystem message for this trigger entity
void CStartTrigger::Process(const CMessageSystem& msgsys)
{
	if (u4_Probability == 0)
		return;
	
	if (msgsys.escCode == escSCENE_FILE_LOADED)
	{
		if (!bFired)
		{
			bFired = true;
			Trigger();
		}
	}
}


//*****************************************************************************************
char* CStartTrigger::pcSave(char*  pc) const
{
	pc = CTrigger::pcSave(pc);

	pc = pcSaveT(pc, bFired);

	return pc;
}


//*****************************************************************************************
const char* CStartTrigger::pcLoad(const char*  pc)
{
	pc = CTrigger::pcLoad(pc);

	pc = pcLoadT(pc, &bFired);

	return pc;
}


#if VER_TEST
//*****************************************************************************************
int CStartTrigger::iGetDescription(char* pc_buffer, int i_buffer_len)
{
	CTrigger::iGetDescription(pc_buffer, i_buffer_len);

	char str_buffer[32];

	strcat(pc_buffer, "\nCStartTrigger:\n");

	sprintf(str_buffer, "Fired\t%d\n", bFired);
	strcat(pc_buffer, str_buffer);

	int i_len = strlen(pc_buffer);

	Assert(i_len < i_buffer_len);
	return i_len;
}
#endif


//*****************************************************************************************
bool CStartTrigger::bValidateTriggerProperties
(
	const CGroffObjectName*	pgon,		// Object to load.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	CLoadWorld*				p_load		// The loader.
)
{
	return CTrigger::bValidateTriggerProperties(pgon, h_object,pvtable,p_load);
}
