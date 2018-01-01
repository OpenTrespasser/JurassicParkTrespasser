/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of CCollisionTrigger defined in Trigger.hpp
 *
 * Contents:
 *		CCollisionTrigger
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/CollisionTrigger.cpp                                     $
 * 
 * 10    10/01/98 6:49p Mlange
 * Improved collision message stats.
 * 
 * 9     98/08/21 18:03 Speter
 * Updated for new collision parameters.
 * 
 * 8     8/12/98 1:59p Mlange
 * Collision message type now requires registration from its intended recipients.
 * 
 * 7     7/20/98 11:26p Rwyatt
 * Added validation to all triggers
 * All changable data is now saved in the scene file
 * 
 * 6     7/02/98 7:06p Rwyatt
 * Default Max collision velocity is 1.0
 * 
 **********************************************************************************************/

#include "Trigger.hpp"
#include <algo.h>

#include "Action.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgCollision.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Lib/Sys/Profile.hpp"

//*********************************************************************************************
//
// CCollisionTrigger implementation.
//


//*********************************************************************************************
CCollisionTrigger::CCollisionTrigger
(
	const CGroffObjectName*	pgon,		// Object to load.
	CLoadWorld*				p_load,		// The loader.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
) : CTrigger(pgon, p_load, h_object, pvtable, pinfo)
{
	bSoundMaterial1 = false;
	bSoundMaterial2 = false;
	u4Handle1 = 0;
	u4Handle2 = 0;
	fMinVelocity = 0.0f;
	fMaxVelocity = 1.0f;

	SETUP_TEXT_PROCESSING(pvtable, p_load)
	SETUP_OBJECT_HANDLE(h_object)
	{
		const CEasyString* pestr = 0;

		// Read the status of the sound materials
		bFILL_BOOL(bSoundMaterial1, esSoundMaterial1 ); 
		bFILL_BOOL(bSoundMaterial2, esSoundMaterial2 ); 

		bFILL_FLOAT(fMinVelocity, esMinVelocity);
		bFILL_FLOAT(fMaxVelocity, esMaxVelocity);

		// look for an object/material for the first element of the collision
		if (bFILL_pEASYSTRING(pestr, esElement1))
		{
			if (bSoundMaterial1)
			{
				smat1 = matHashIdentifier(pestr->strData());
			}
			else
			{
				u4Handle1 = u4Hash(pestr->strData(),0,true);
			}
		}

		// look for an object/material for the second element of the collision
		if (bFILL_pEASYSTRING(pestr, esElement2))
		{
			if (bSoundMaterial2)
			{
				smat2 = matHashIdentifier(pestr->strData());
			}
			else
			{
				u4Handle2 = u4Hash(pestr->strData(),0,true);
			}
		}
	}
	END_OBJECT;
	END_TEXT_PROCESSING;

	// Both sound handles cannot be zero otherwise we are going to be firing on every collision
	Assert( (u4Handle1!=0) || (u4Handle2!=0) );

	Assert( fMinVelocity >=0.0f);
	Assert( fMaxVelocity >=0.0f);
	Assert( fMinVelocity <=1.0f);
	Assert( fMaxVelocity <=1.0f);

	// Make sur that the min is less than the max!
	Assert( fMinVelocity<fMaxVelocity);

	// Register this entity with the message types it needs to receive.
	CMessageCollision::RegisterRecipient(this);
}


//*********************************************************************************************
CCollisionTrigger::~CCollisionTrigger()
{
	CMessageCollision::UnregisterRecipient(this);
}


extern CProfileStat psCollisionMsgTrigger;

//*********************************************************************************************
// The collision trigger acts for a collision message
//
void CCollisionTrigger::Process(const CMessageCollision& msg)
{
	CTimeBlock tmb(&psCollisionMsgTrigger);


	bool	b_collision_element1 = false;
	bool	b_collision_element2 = false;


	// The simulation must be going before we can get a collision message, if this is not the
	// case then something is seriously wrong.
	Assert(CMessageSystem::bSimulationGoing());

	//
	// This trigger has zero probabilty so do nothing. Triggers that keep local state should no do this
	//
	if (u4_Probability == 0)
		return;

	float	f_nvel = msg.fEnergyMaxNormLog();
	//
	// If the force is not in range then do nothing...
	//
	if ( (f_nvel<fMinVelocity) || (f_nvel>fMaxVelocity) )
		return;

	if (u4Handle1)
	{
		// we have something in element 1 so lets see if this collision concerns us
		if (bSoundMaterial1)
		{
			// element 1 specifies a sound material
			b_collision_element1 =	smat1 == msg.smatSound1 ||
									smat1 == msg.smatSound2;
		}
		else
		{
			// element 1 specified an instance handle

			b_collision_element1 =	(msg.pins1 && u4Handle1 == msg.pins1->u4GetUniqueHandle()) ||
									(msg.pins2 && u4Handle1 == msg.pins2->u4GetUniqueHandle());
		}
	}
	else
	{
		// element 1 of this trigger is 0 so all collisions concern us
		b_collision_element1 = true;
	}

	if (u4Handle2)
	{
		// we have something in element 1 so lets see if this collision concerns us
		if (bSoundMaterial2)
		{
			// element 1 specifies a sound material
			b_collision_element2 =	smat2 == msg.smatSound2 ||
									smat2 == msg.smatSound1;
		}
		else
		{
			// element 1 specified an instance handle
			b_collision_element2 =	(msg.pins1 && u4Handle2 == msg.pins1->u4GetUniqueHandle()) ||
									(msg.pins2 && u4Handle2 == msg.pins2->u4GetUniqueHandle());
		}
	}
	else
	{
		// element 1 of this trigger is 0 so all collisions concern us
		b_collision_element2 = true;
	}

	// If both of collision elements 1 evaluated to true (code above) then we need
	// to fire the trigger.
	if (b_collision_element1 && b_collision_element2)
	{
		// Call the base class fire function
		AttemptTriggerFire();
	}
}



//*****************************************************************************************
char * CCollisionTrigger::pcSave(char *  pc) const
{
	return CTrigger::pcSave(pc);
}


//*****************************************************************************************
const char * CCollisionTrigger::pcLoad(const char *  pc)
{
	return CTrigger::pcLoad(pc);
}

#if VER_TEST
//*****************************************************************************************
int CCollisionTrigger::iGetDescription(char* pc_buffer, int i_buffer_len)
{
	CTrigger::iGetDescription(pc_buffer, i_buffer_len);

	char str_buffer[512];

	strcat(pc_buffer, "\nCCollisionTrigger:\n");

	if (bSoundMaterial1)
	{
		sprintf(str_buffer,"Sound Material 1\t%d\n", smat1);
		strcat(pc_buffer, str_buffer);
	}
	else
	{
		char* str_name = CInstance::strInstanceNameFromHandle(u4Handle1);
		sprintf(str_buffer,"Instance Handle 1\t%d (%s)\n", u4Handle1, str_name?str_name:"No name");
		strcat(pc_buffer, str_buffer);
	}

	if (bSoundMaterial2)
	{
		sprintf(str_buffer,"Sound Material 2\t%d\n", smat2);
		strcat(pc_buffer, str_buffer);
	}
	else
	{
		char* str_name = CInstance::strInstanceNameFromHandle(u4Handle2);
		sprintf(str_buffer,"Instance Handle 2\t%d (%s)\n", u4Handle2, str_name?str_name:"No name");
		strcat(pc_buffer, str_buffer);
	}

	sprintf(str_buffer,"Minimum Velocity 1\t%f\n", fMinVelocity);
	strcat(pc_buffer, str_buffer);

	sprintf(str_buffer,"Maximum Velocity 1\t%f\n", fMaxVelocity);
	strcat(pc_buffer, str_buffer);
	
	int i_len = strlen(pc_buffer);

	Assert(i_len < i_buffer_len);
	return i_len;
}
#endif //VER_TEST


//*****************************************************************************************
bool CCollisionTrigger::bValidateTriggerProperties
(
	const CGroffObjectName*	pgon,		// Object to load.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	CLoadWorld*				p_load		// The loader.
)
{
	return CTrigger::bValidateTriggerProperties(pgon, h_object,pvtable,p_load);
}
