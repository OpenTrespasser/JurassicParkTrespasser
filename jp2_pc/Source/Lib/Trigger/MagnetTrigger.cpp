/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of CMagnetTrigger defined in Trigger.hpp
 *
 * Contents:
 *		CMagnetTrigger
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
 * $Log:: /JP2_PC/Source/Lib/Trigger/MagnetTrigger.cpp                                        $
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
 * 2     12/03/97 12:02p Agrant
 * Trigger load/save
 * 
 * 1     11/13/97 11:24p Rwyatt
 * Initial Implementation, Split off from the old trigger.cpp
 * 
 **********************************************************************************************/

#include "Trigger.hpp"
#include <algo.h>

#include "Action.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgTrigger.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMagnet.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Groff/VTParse.hpp"



//*********************************************************************************************
//
// CMagnetTrigger implementation.
//




//*********************************************************************************************
//
CMagnetTrigger::CMagnetTrigger
(
	const CGroffObjectName*	pgon,		// Object to load.
	CLoadWorld*				p_load,		// The loader.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
) : CTrigger(pgon, p_load, h_object, pvtable, pinfo)
//
//**************************************
{
}


//*********************************************************************************************
//
CMagnetTrigger::~CMagnetTrigger()
//
//**************************************
{
}



//*********************************************************************************************
//
void CMagnetTrigger::Process
(
	const CMessageMagnetBreak& msgmagb
)
//
//**************************************
{
	if (u4_Probability == 0)
		return;

}



//*********************************************************************************************
//
void CMagnetTrigger::Process
(
	const CMessageMagnetMove& msgmagm
)
//
//**************************************
{
	if (u4_Probability == 0)
		return;

}


//*****************************************************************************************
char * CMagnetTrigger::pcSave(char *  pc) const
{
	return CTrigger::pcSave(pc);
}

//*****************************************************************************************
const char * CMagnetTrigger::pcLoad(const char *  pc)
{
	return CTrigger::pcLoad(pc);
}


#if VER_TEST
//*****************************************************************************************
int CMagnetTrigger::iGetDescription(char* pc_buffer, int i_buffer_len)
{
	CTrigger::iGetDescription(pc_buffer, i_buffer_len);

	strcat(pc_buffer, "\nCMagnetTrigger:\n");

	int i_len = strlen(pc_buffer);

//	iGetConditionDescription(tco_TriggerConditions, pc_buffer + i_len, i_buffer_len - i_len);

	i_len = strlen(pc_buffer);
	Assert(i_len < i_buffer_len);
	return i_len;
}
#endif


//*****************************************************************************************
bool CMagnetTrigger::bValidateTriggerProperties
(
	const CGroffObjectName*	pgon,		// Object to load.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	CLoadWorld*				p_load		// The loader.
)
{
	return CTrigger::bValidateTriggerProperties(pgon, h_object,pvtable,p_load);
}
