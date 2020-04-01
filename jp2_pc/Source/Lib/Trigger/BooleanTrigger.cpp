/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of CBooleanTrigger defined in Trigger.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/BooleanTrigger.cpp                                       $
 * 
 * 5     8/23/98 3:24p Rwyatt
 * Added evaluate now for boolean triggers
 * 
 * 4     8/13/98 1:42p Mlange
 * The step message now requires registration of its recipients.
 * 
 * 3     7/20/98 11:26p Rwyatt
 * Added validation to all triggers
 * All changable data is now saved in the scene file
 * 
 * 2     7/06/98 2:50a Rwyatt
 * Boolean triggers now fire from the step message so they can handle negative logic.
 * 
 * 1     7/04/98 8:12p Rwyatt
 * Initial implementation
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
// CBooleanTrigger implementation.
//

	//*********************************************************************************************
	CBooleanTrigger::CBooleanTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CTrigger(pgon, p_load, h_object, pvtable, pinfo)
	{
		// Register this entity with the message types it needs to receive.
		CMessageStep::RegisterRecipient(this);
	}


	//*********************************************************************************************
	CBooleanTrigger::~CBooleanTrigger()
	{
		CMessageStep::UnregisterRecipient(this);
	}


	//*********************************************************************************************
	void CBooleanTrigger::Process(const CMessageStep& msg)
	{
		AttemptTriggerFire();
	}


	//*********************************************************************************************
	bool CBooleanTrigger::bEvaluateNow()
	{
		if (peeFireExpression)
		{
			return peeFireExpression->EvaluateExpression();
		}

		return true;
	}

	//*****************************************************************************************
	bool CBooleanTrigger::bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	)
	{
		return CTrigger::bValidateTriggerProperties(pgon, h_object,pvtable,p_load);
	}
