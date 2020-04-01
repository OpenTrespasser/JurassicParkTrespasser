/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of CVariableTrigger defined in Trigger.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/VariableTrigger.cpp                                       $
 * 
 * 1     98.08.04 9:34p Mmouni
 * Created.
 * 
 **********************************************************************************************/

#include "Trigger.hpp"

#include <string.h>
#include <vector>
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"


//*********************************************************************************************
//
// CVariableTrigger implementation.
//

	//*********************************************************************************************
	CVariableTrigger::CVariableTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CTrigger(pgon, p_load, h_object, pvtable, pinfo)
	//*********************************
	{
		bValue = false;

		// Get values from text-props.
		IF_SETUP_TEXT_PROCESSING(pvtable, p_load)
		SETUP_OBJECT_HANDLE(h_object)
		{
			bFILL_BOOL(bValue, esValue);
		}
		END_OBJECT;
		END_TEXT_PROCESSING;
	}

	//*********************************************************************************************
	CVariableTrigger::~CVariableTrigger()
	//*********************************
	{
	}

#if VER_TEST
	//*****************************************************************************************
	int CVariableTrigger::iGetDescription(char *buffer, int i_buffer_length)
	{
		CTrigger::iGetDescription(buffer, i_buffer_length);

		strcat(buffer, "\nCCollisionTrigger:\n");

		strcat(buffer, "Value: ");

		if (bValue)
			strcat(buffer, "true");
		else
			strcat(buffer, "false");

		int i_len = strlen(buffer);

		Assert(i_len < i_buffer_length);
		return i_len;
	}
#endif


	//*****************************************************************************************
	bool CVariableTrigger::bEvaluateNow()
	//*********************************
	{
		return bValue;
	}

	//*********************************************************************************************
	void CVariableTrigger::Set(bool b_value)
	//*********************************
	{
		bValue = b_value;
	}
	
	//*********************************************************************************************
	void CVariableTrigger::Invert()
	//*********************************
	{
		bValue = !bValue;
	}

	//*****************************************************************************************
	bool CVariableTrigger::bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	)
	{
		return CTrigger::bValidateTriggerProperties(pgon, h_object,pvtable,p_load);
	}
