/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of Trigger.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/TriggerBase.cpp                                          $
 * 
 * 40    9/14/98 11:09p Pkeet
 * supplied second argument to dprintf
 * 
 * 39    8/20/98 7:47p Mlange
 * Now registers trigger base class to receive trigger messages only if it has an expression.
 * 
 * 38    8/13/98 1:43p Mlange
 * The step message now requires registration of its recipients.
 * 
 * 37    8/12/98 6:50p Agrant
 * Share spherical trigger CInfos
 * 
 * 36    8/12/98 3:41p Mlange
 * The CMessageTrigger and CMessageMoveTriggerTo message types now use the new registration
 * scheme.
 * 
 * 35    98.08.04 9:40p Mmouni
 * Info now shows value of expression (if it exists).
 * 
 * 34    8/04/98 9:29a Agrant
 * helper function 
 * 
 * 33    8/03/98 9:14p Agrant
 * load no triggers before load pass 2.
 * This allows player data to finish loading.
 * 
 * 32    7/29/98 10:51a Rwyatt
 * Moved the default implementation of EvaluateNow() to the CPP file, this function now prints
 * a warning so you know the default is being used,
 * 
 **********************************************************************************************/
#include "Common.hpp"
#include "Trigger.hpp"
#include <algorithm>
#include "Action.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgTrigger.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/GeomDBase/Partition.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Lib/sys/DebugConsole.hpp"


//*********************************************************************************************
// Define a static uint bounding sphere that all spherical triggers use.
//
CBoundVolSphere	CTrigger::bvsTriggerUnitSphere(1.0f);
CRandom	 CTrigger::rndRand;

// Shared by all spherical triggers.
extern const CInfo* pinfoSphericalTrigger;

//*********************************************************************************************
// Static function to check if the given object name is loaded.
// This function will assert if the string does not reference an object that exists in the
// groff file...
//
bool bGroffObjectLoaded
(
	CLoadWorld*					pload,
	const char*					str
)
{
	// assert on a valid string pointer and valid string (not NULL)
	Assert( str );
	Assert( *str );

	// get the named object from the world
	CInstance* pins = wWorld.ppartPartitionList()->
		pinsFindNamedInstance(str);

	// return true if we found the object...
	if (pins)
		return true;

	// if we are in test build check that the specified symbol actually exists in the GRF file
#if VER_TEST
	CGroffObjectName* pgon = pload->goiInfo.pgonFindObject( str );

	if (pgon == 0)
	{
		char str_buffer[1024];
		sprintf(str_buffer, 
				"%s\n\nSymbol '%s' not present in groff file.\n", 
				__FILE__, 
				str );

		if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
			DebuggerBreak();
	}			
#endif

	// The object exists but has not been loaded yet, return false....
	return false;
}

//*********************************************************************************************
// Static function to check if the given object name is present in the GROFF.
//
bool bInGroff
(
	CLoadWorld*					pload,
	const char*					str
)
{
	if (pload->goiInfo.pgonFindObject( str ))
		return true;
	else
		return false;
}



//*********************************************************************************************
//
// CTrigger implementation.
//

//*********************************************************************************************
CTrigger::CTrigger()
{
	// Register this entity with the message types it needs to receive.
	         CMessageStep::RegisterRecipient(this);
	      CMessageTrigger::RegisterRecipient(this);
	CMessageMoveTriggerTo::RegisterRecipient(this);
}

//*********************************************************************************************
CTrigger::CTrigger
(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.

) 
	// To make a trigger invisible, just set the Visible prop to false.
	: CEntity( pgon, p_load, h_object, pvtable, pinfo )
{
	Assert( h_object != hNULL_HANDLE);

	// Set up the shared mesh


	int				i_firecount		= 0xffffffff;
	int				i_prob_percent	= 100;								// probability as a percentage.
	uint32			u4_prob			= TRIGGER_PROBABILITY_ALWAYS;		// 0xffffffff
	TSec			s_firedelay		= 0.0f;								// fire now
	bool			b_firezero		= false;
	bool			b_resetfire		= false;
	int				i_action_process= (int)eapALL;
	int32			i4_bv_type		= BOUND_SPHERE;

	// by default a trigger has a NULL expression which means 'true'
	peeFireExpression = NULL;
	u4ActionCount = 0;

	// Sequences are not delayed by default.
	fMinSequenceDelay	= 0.0f;
	fMaxSequenceDelay	= 0.0f;
	sNextFireTime		= 0.0f;
	sFireDelayPeriod	= 0.0f;

	IF_SETUP_TEXT_PROCESSING(pvtable, p_load)
	SETUP_OBJECT_HANDLE(h_object)
	{
		// now we have a valid trigger class, get the elements out of it.
		// Only the action is essential, the others have suitable defaults.

		const CEasyString*	pestr_exp = 0;

		// Do we have an expression
		if (bFILL_pEASYSTRING(pestr_exp, esTFireExpression))
		{
			// an expression is present and it must be valid if we got to here.
			// make an expression class and convert the infix string to reverse polish
			peeFireExpression = new CExpressionEvaluate;
			if (peeFireExpression)
			{
				peeFireExpression->SetInfixExpression(pestr_exp->strData());
			}

			CMessageTrigger::RegisterRecipient(this);
		}

		// trigger fire count
		bFILL_INT(i_firecount, esTFireCount);
		Assert(i_firecount!=0);

		// fire probability
		bFILL_INT(i_prob_percent, esTProb);
		Assert ((i_prob_percent>=0.0f) && (i_prob_percent<=100.0f));

		// convert the percentage probability into an unsigned 32bit int
		u4_prob = i_prob_percent * (TRIGGER_PROBABILITY_ALWAYS / 100);
		if (i_prob_percent == 100)
			u4_prob = TRIGGER_PROBABILITY_ALWAYS;

		// action process style
		bFILL_INT(i_action_process, esTProcessStyle);
		Assert( (i_action_process>=0) && (i_action_process<(int)eapEND) );

		// fire delay
		bFILL_FLOAT(s_firedelay, esTFireDelay);
		Assert(s_firedelay>=0.0f);

		// fire repeat delay
		bFILL_FLOAT(sFireDelayPeriod, esTRepeatPeriod);
		Assert(sFireDelayPeriod>=0.0f);

		// fire at zero flag
		bFILL_BOOL(b_firezero, esTFireZero);

		// reset after fire at zero flag
		bFILL_BOOL(b_resetfire, esTResetFire);

		// if reset fire is set and firezero is not then assert.
		Assert (!((!b_firezero) && (b_resetfire)))

		// get the bounding volume type.
		bFILL_INT(i4_bv_type, esTBoundVol);

		// Get the sequence delay parameters
		bFILL_FLOAT(fMinSequenceDelay, esTSequenceDelayMin);
		bFILL_FLOAT(fMaxSequenceDelay, esTSequenceDelayMax);

		// Ensure that the sequence delays are valid
		Assert(fMinSequenceDelay <= fMaxSequenceDelay);
		Assert(fMaxSequenceDelay >= 0.0f);
		Assert(fMinSequenceDelay >= 0.0f);

		//
		// THIS SHOULD BE A LOOP THAT GOES THROUGH AN ARRAY OF ACTIONS. THIS CANNOT
		// BE DONE UNTIL NESTED OBJECTS ARE WORKING.....
		//

		// Sample value to fill, with defaults.
		CObjectValue* poval_action = 0;
		uint32	u4_act_count = 0;
	
		while ( bFILL_OBJECT(poval_action, (ESymbol)((uint32)esTAction00+u4_act_count)))
		{
			CAction::CreateTriggerAction(this, pgon, poval_action, pvtable, p_load);
			u4_act_count++;
		}

		if (u4_act_count == 0)
		{
			// HACK HACK HACK
			// To handle un-nested text props.
			// Do not use this code as an example!
			if (!poval_action)
				poval_action = __pov;

			// create and add the action to the current trigger
			CAction::CreateTriggerAction(this, pgon, poval_action, pvtable, p_load);
		}
	}
	END_OBJECT;
	END_TEXT_PROCESSING;

	// set the parameters.....
	u4_TriggerLife	= (uint32)i_firecount;
	u4_Probability	= u4_prob;
	s_FireDelay		= s_firedelay;

	s_FireTime		=-1.0F;								// negative time means no delay

	// process style of the action list
	eapList				= (EActionProcess)i_action_process;
	u4NextAction		= 0;
	bSequenceActions	= false;

	// fire at zero/reset fire count flags
	bFireZero		= b_firezero;

	if ( (bFireZero) && (b_resetfire) )
	{
		u4ResetLife	= u4_TriggerLife;
	}
	else
	{
		// reset life is only used while bFireZero is active
		u4ResetLife	= 0;
	}

	// create a presence from the groff object. we cannot use the object presence at this 
	// point because it has not been created
	CPresence3<> pr3_trigger = ::pr3Presence(*pgon);

	switch (i4_bv_type)
	{
	case BOUND_SPHERE:
		// bounding volume is the unit sphere
		bv_BoundingVolume = &bvsTriggerUnitSphere;

		// Do we already have a global spherical trigger info?
		if (!pinfoSphericalTrigger)
			// No!   Use ours.
			pinfoSphericalTrigger = pinfoProperties;
		
		// This can be bad if the mesh in question extends outside the specified sphere.
#if VER_DEBUG
		{
			rptr_const<CRenderType> prdt = prdtGetRenderInfo();
			CMesh* pmsh = dynamic_cast<CMesh*>((CRenderType*)prdt.ptGet());

			if (pmsh)
			{
				int i = 0;
				for (i = 0; i < pmsh->pav3Points.size(); ++i)
				{
					if (pmsh->pav3Points[i].tLenSqr() > 1.001f)
					{
						// Trigger mesh extends beyond the unit sphere!w  Use a box trigger, or the MAX sphere primitive.
						char str_buffer[256];
						sprintf(str_buffer, 
								"%s\n\nSpherical trigger %s extends beyond unit sphere!:\n", 
								__FILE__, 
								pgon->strObjectName);

						if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
							DebuggerBreak();

						break;
					}
				}
			}
		}
#endif


		break;

	case BOUND_CUBE:
		// This looks bad, but bvGet returns a reference, so it ought to be fine.
		bv_BoundingVolume = &prdtGetRenderInfo()->bvGet();
		break;

	default:
		// unknown bounding volume type..
		Assert(0);
		break;
	}

	//
	// Make sure that the trigger mesh does not have lots of polygons...
	// 
#if VER_DEBUG
	{
		rptr_const<CRenderType> prdt = prdtGetRenderInfo();
		CMesh* pmsh = dynamic_cast<CMesh*>((CRenderType*)prdt.ptGet());

		if (pmsh)
		{
			if (pmsh->pav3Points.size()>20)
			{
				dprintf("Trigger %s has more than 20 polygons in mesh\n", pgon->strObjectName);
			}
		}
	}
#endif

	// Register this entity with the message types it needs to receive.
	         CMessageStep::RegisterRecipient(this);
	CMessageMoveTriggerTo::RegisterRecipient(this);
}



//*********************************************************************************************
// Base class destructor, other triggers should virtually overide this function...
CTrigger::~CTrigger()
{
	CMessageMoveTriggerTo::UnregisterRecipient(this);
	      CMessageTrigger::UnregisterRecipient(this);
	         CMessageStep::UnregisterRecipient(this);

//
//	for (uint32 u4=0 ; u4<u4ActionCount; u4++)
//		delete acList[u4];

	// delete the expression
	delete peeFireExpression;
}



//*********************************************************************************************
// Attempt to fire the trigger by evaluating any expression and checking all other conditions
//
void CTrigger::AttemptTriggerFire
(
)
//
//**************************************
{
	// if we are not in play mode then do not fire any triggers. Dervided classes may require
	// to also check this variable so to keep their data in step.
	if (!CMessageSystem::bSimulationGoing())
		return;

	// If this trigger has a fire repeat delay: make sure enough time has elapsed
	if (CMessageStep::sStaticTotal<sNextFireTime)
		return;

	// if this trigger is sequencing an action then make sure it does nothing else.
	if (bSequenceActions)
		return;

	// if we have an expression then evaluate it, no expression assumes always true.
	if (peeFireExpression)
	{
		if (!peeFireExpression->EvaluateExpression())
		{
			// the expression has come out to be false so we cannot fire
			return;
		}
	}

	// now do the random thing.
	if (rndRand()<=u4_Probability)
	{
		// we are going to fire the trigger, lets do the time thing
		if (s_FireDelay==0.0F)
		{
			// there is no delay
			Trigger();
		}
		else
		{
			// this message is delayed
			s_FireTime=CMessageStep::sStaticTotal+s_FireDelay;
		}
	}
}


//*********************************************************************************************
// Cacluate an additional sequence delay for this sequence step. Normally this is zero seconds
// and there is no gap betwen consecutive sequences. fSequenceMin and fSequenceMax specifiy a
// random time in seconds.
//
TSec CTrigger::sSequenceDelay
(
)
//
//**************************************
{
	return rndRand((double)fMinSequenceDelay,(double)fMaxSequenceDelay);
}


//*********************************************************************************************
// Process in the list of actions within this trigger in the specified way
//
void CTrigger::ProcessActionList
(
)
//
//**************************************
{
	uint32 u4;

	if (u4ActionCount == 0)
		return;

	Assert(bSequenceActions == false);

	switch (eapList)
	{

	// do all of the actions for this trigger, if any
	case eapALL:
		for (u4=0 ; u4<u4ActionCount; u4++)
			acList[u4]->Start();
		break;

	// do the actions in sequence, going back to the start when the end is reached
	case eapSTEPORDER:
		if (u4ActionCount>0)
		{
			acList[u4NextAction]->Start();
			u4NextAction++;
			if (u4NextAction >= u4ActionCount)
				u4NextAction = 0;
		}
		break;

	// do the list in a random order, the same action may be done on consecutive fires.
	case eapSTEPRANDOM:
		acList[ rndRand(0,u4ActionCount) ]->Start();
		break;

	case eapSEQUENCEORDER:
	case eapSEQUENCEORDERLOOP:
		bSequenceActions = true;
		acList[0]->Start();
		u4NextAction = 1;
		sNextActionTime = CMessageStep::sStaticTotal + acList[0]->sGetActionTime() + sSequenceDelay();
		break;

	case eapSEQUENCERANDOM:
	case eapSEQUENCERANDOMLOOP:
		bSequenceActions = true;
		u4 = rndRand(0,u4ActionCount);
		acList[u4]->Start();
		sNextActionTime = CMessageStep::sStaticTotal + acList[u4]->sGetActionTime() + sSequenceDelay();
		u4NextAction = 1;		// Action counter for random actions
		break;

	default:
		// Should not be able to get to here
		Assert(0);
	}
}



//*********************************************************************************************
// Maintain the fire state and call the action processor. Disables the trigger if it is a one
// hit and its life has expired.
// Also send a fire message so all other boolean expressions can be updated. The trigger does
// not have to stay around after this function.
//
void CTrigger::Trigger()
//
//**************************************
{
	// if trigger life is 1 and fire zero is set to true then it is time to fire.
	// the counter gets decremented to zero at the bottom of this function.
	// Always fire if bFireZero is false.
	if ( (!bFireZero) || (u4_TriggerLife == 1) )
	{
		ProcessActionList();

		// fire off a trigger message so everybody else knows what is going on, including other
		// triggers especially the boolean triggers.
		// we must specify a source for these message types
		CMessageTrigger msgtrig(this);
		msgtrig.Queue();

		// Set the repeat time, this is the time before we can fire again...
		sNextFireTime = CMessageStep::sStaticTotal + sFireDelayPeriod;
	}

	// If we are a permanent trigger then stop here
	if (u4_TriggerLife==0xffffffff)
		return;

	// decrement the counter and if it is zero kill the trigger
	if (--u4_TriggerLife==0)
	{
		if (bFireZero)
		{
			if (u4ResetLife)
			{
				u4_TriggerLife = u4ResetLife;
			}
		}
		else
		{
			// if we are not set to fire at zero the trigger is dead.
			u4_Probability = 0;
		}
	}
}




//*********************************************************************************************
// Sets the current time for the Trigger so that the delayed trigger time can be calculated, 
// It then checks the current time against the delayed time for this trigger to see if it 
// requires firing,
//
void CTrigger::Process(const CMessageStep& msgstep)
{
	// if there is a negative fire time then this is not a delayed trigger
	// if there is a positve fire time check if it has expired
	if ( (s_FireTime>0.0F) && (msgstep.sTotal>s_FireTime) )
	{
		Trigger();
		s_FireTime = -1.0f;
	}

	// Is this trigger currently processing a sequenced action??
	if ((bSequenceActions) && (msgstep.sStaticTotal>sNextActionTime))
	{
		uint32 u4;

		// It is time to do another action but what do we want to do??
		switch (eapList)
		{
			case eapSEQUENCEORDER:
				if (u4NextAction<u4ActionCount)
				{
					// There is another action in the list so process it
					acList[u4NextAction]->Start();
					sNextActionTime = msgstep.sStaticTotal + acList[u4NextAction]->sGetActionTime() + sSequenceDelay();
					u4NextAction++;
				}
				else
				{
					// No more actions so stop the sequencer
					bSequenceActions = false;
				}
				break;

			case eapSEQUENCEORDERLOOP:
				if (u4NextAction>=u4ActionCount)
					u4NextAction = 0;

				acList[u4NextAction]->Start();
				sNextActionTime = msgstep.sStaticTotal + acList[u4NextAction]->sGetActionTime() + sSequenceDelay();
				u4NextAction++;
				break;

			case eapSEQUENCERANDOM:
				// Pick a random element rather than the current element.
				if (u4NextAction<u4ActionCount)
				{
					u4 = rndRand(0,u4ActionCount);
					acList[u4]->Start();
					sNextActionTime = msgstep.sStaticTotal + acList[u4]->sGetActionTime() + sSequenceDelay();
					u4NextAction++;		// Action counter for random actions
				}
				else
				{
					bSequenceActions = false;
				}
				break;

			case eapSEQUENCERANDOMLOOP:
				u4 = rndRand(0,u4ActionCount);
				acList[u4]->Start();
				sNextActionTime = msgstep.sStaticTotal + acList[u4]->sGetActionTime() + sSequenceDelay();
				break;

			default:
				// Should not be able to get to here
				Assert(0);
		}

	}
}



//*********************************************************************************************
// Every trigger can have its location in the world changed. 
// For triggers other than location triggers the location has no meaning.
void CTrigger::Process(const CMessageMoveTriggerTo& msgmtrigto)
{
	if (u4_Probability == 0)
		return;

	Move(msgmtrigto.p3NewPosition);
}



//*********************************************************************************************
// A trigger has fired message, update any boolean expression
void CTrigger::Process(const CMessageTrigger& trigmsg)
{
	if (u4_Probability == 0)
		return;

	// if we generated this message then ignore it, otherwise we are liable to get stuck
	// in a loop.
	// Boolean expressions can not depend on themselves.
	if (trigmsg.ptrGetActivatedTrigger() == this)
	{
		return;
	}

	// this trigger has no expression so we do not care about other triggers firing
	if (peeFireExpression == NULL)
		return;

	peeFireExpression->bUpdateSymbolState( trigmsg.ptrGetActivatedTrigger() );
}



//*********************************************************************************************
// This function is a hack and it assume that the trigger text props are all at one level.
// When sub objects are inplemented this function will have to be chnaged to reflect that,
// when object pointers are implemented in text properties this function will probably not be
// required and will have to be completely re-written anyway.
//
bool CTrigger::bValidateTriggerProperties
(
	const CGroffObjectName*	pgon,		// Object to load.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	CLoadWorld*				p_load		// The loader.
)
//
//**************************************
{
	if (p_load->iLoadPass < 2)
		return false;

	//
	// Go through all instance references in the trigger and make sure they are present
	// also, assert on anything that seems incorrect.
	// If a missing instance is found the return false there and then, if this function
	// makes it to the end true is returned.
	//

	SETUP_TEXT_PROCESSING(pvtable, p_load)
	SETUP_OBJECT_HANDLE(h_object)
	{
		const CEasyString* pestr = 0;

		// look for a fire expression string in the trigger text props...
		if (bFILL_pEASYSTRING(pestr, esTFireExpression))
		{
			if (!CExpressionEvaluate::bValidateExpression(p_load, pestr->strData()))
				return false;
		}

		// look for an activate object
		if (bFILL_pEASYSTRING(pestr, esLTTriggerActivate))
		{
			if (!bGroffObjectLoaded(p_load,pestr->strData()))
				return false;
		}

		//
		// Go through all the actions one by one and validate they are valid
		//
		CObjectValue* poval_action = 0;
		uint32	u4_act_count = 0;
	
		while ( bFILL_OBJECT(poval_action, (ESymbol)((uint32)esTAction00+u4_act_count)))
		{
			if (CAction::bValidateTriggerAction(pgon, poval_action, pvtable, p_load) == false)
				return false;
			u4_act_count++;
		}

		if (u4_act_count == 0)
		{
			if (!poval_action)
				poval_action = __pov;

			// create and add the action to the current trigger
			if (CAction::bValidateTriggerAction(pgon, poval_action, pvtable, p_load) == false)
				return false;
		}
	}
	END_OBJECT;
	END_TEXT_PROCESSING

	// return true if we made it through all the tests
	return true;
}



/*
// Fields saved for CTrigger

	CExpressionEvaluate*	peeFireExpression;
	uint32					u4NextAction;
	uint32					u4_TriggerLife;
	uint32					u4_Probability;
	TSec					s_FireTime;
	bool					bSequenceActions;
	TSec					sNextActionTime;
	TSec					sNextFireTime;
*/

//*****************************************************************************************
void CTrigger::Move(const CPlacement3<>& p3_new)
{
	// The world database really knows how to deal with moved partitions.
	wWorld.Move(this, p3_new, wWorld.ppartTriggers);
}

//*****************************************************************************************
char * CTrigger::pcSave(char *  pc) const
{
	pc = CEntity::pcSave(pc);

	if (peeFireExpression)
	{
		pc = peeFireExpression->pcSave(pc);
	}

	pc = pcSaveT(pc, u4NextAction);
	pc = pcSaveT(pc, u4_TriggerLife);	
	pc = pcSaveT(pc, u4_Probability);	
	pc = pcSaveT(pc, s_FireTime);
	pc = pcSaveT(pc, bSequenceActions);
	pc = pcSaveT(pc, sNextActionTime);
	pc = pcSaveT(pc, sNextFireTime);

	// Save any volatile data that this triggers actions may have
	for (uint u4=0 ; u4<u4ActionCount; u4++)
		pc = acList[u4]->pcSave(pc);

	return pc;
}

//*****************************************************************************************
const char * CTrigger::pcLoad(const char *  pc)
{
	pc = CEntity::pcLoad(pc);

	if (peeFireExpression)
	{
		pc = peeFireExpression->pcLoad(pc);
	}

	pc = pcLoadT(pc, &u4NextAction);
	pc = pcLoadT(pc, &u4_TriggerLife);	
	pc = pcLoadT(pc, &u4_Probability);	
	pc = pcLoadT(pc, &s_FireTime);
	pc = pcLoadT(pc, &bSequenceActions);
	pc = pcLoadT(pc, &sNextActionTime);
	pc = pcLoadT(pc, &sNextFireTime);

	// Reload trigger action data.
	for (uint u4=0 ; u4<u4ActionCount; u4++)
		pc = acList[u4]->pcLoad(pc);

	return pc;
}



//*****************************************************************************************
bool CTrigger::bEvaluateNow()
{
	dprintf("Trigger '%s' has no Evaluate Now function, returning false\n",strGetInstanceName());
	return false;
}



#if VER_TEST
//*****************************************************************************************
int CTrigger::iGetDescription(char* pc_buffer, int i_buffer_len)
{
	CInstance::iGetDescription(pc_buffer, i_buffer_len);

	char str_buffer[512];

	strcat(pc_buffer, "\nTRIGGER:\n");

	if (peeFireExpression)
	{
		sprintf(str_buffer,"peeFireExpression\t%s\n", peeFireExpression->EvaluateExpression() ? "true" : "false");
		strcat(pc_buffer, str_buffer);
	}
	else
	{
		sprintf(str_buffer,"peeFireExpression\t%s\n", "none (true)");
		strcat(pc_buffer, str_buffer);
	}

	sprintf(str_buffer,"u4ActionCount\t%d\n", u4ActionCount);
	strcat(pc_buffer, str_buffer);

	sprintf(str_buffer,"u4NextAction\t%d\n", u4NextAction);
	strcat(pc_buffer, str_buffer);

	// The type of action list process
	//EActionProcess			eapList;

	sprintf(str_buffer,"u4_TriggerLife\t%d\n", u4_TriggerLife);
	strcat(pc_buffer, str_buffer);

	sprintf(str_buffer,"u4_Probability\t%d (%d%%)\n", u4_Probability, int(100.0f * (float(u4_Probability) / float(0xffffffff)) + 0.5f));
	strcat(pc_buffer, str_buffer);

	sprintf(str_buffer,"s_FireDelay\t%f\n", s_FireDelay);
	strcat(pc_buffer, str_buffer);

	sprintf(str_buffer,"s_FireTime\t%f\n", s_FireTime);
	strcat(pc_buffer, str_buffer);
	
	sprintf(str_buffer,"bFireZero\t%s\n", bFireZero ? "true" : "false");
	strcat(pc_buffer, str_buffer);

	sprintf(str_buffer,"u4ResetLife\t%d\n", u4ResetLife);
	strcat(pc_buffer, str_buffer);

	int i_len = strlen(pc_buffer);
	Assert(i_len < i_buffer_len);
	return i_len;
}




// Gets a description of the condition in human readable form.
int iGetConditionDescription(TCondition tco, char *pc_buffer, int i_buffer_len)
{
	char str_add_buffer[512];

	sprintf(str_add_buffer, "\nCondition (value %d):\n", tco);
	strcat(pc_buffer, str_add_buffer);


	if (tco & TRIGGER_COND_PLAYER_MOVE_INSIDE)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_PLAYER_MOVE_INSIDE\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_PLAYER_MOVE_OUTSIDE)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_PLAYER_MOVE_OUTSIDE\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_CREATURE_MOVE_INSIDE)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_CREATURE_MOVE_INSIDE\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_CREATURE_MOVE_OUTSIDE)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_CREATURE_MOVE_OUTSIDE\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_PLAYER_ALWAYS)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_PLAYER_ALWAYS\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_CREATURE_ALWAYS)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_CREATURE_ALWAYS\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_CREATURE_ENTER_COUNT)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_CREATURE_ENTER_COUNT\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_CREATURE_LEAVE_COUNT)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_CREATURE_LEAVE_COUNT\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_OBJECT_MOVE_INSIDE	)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_OBJECT_MOVE_INSIDE\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_OBJECT_MOVE_OUTSIDE)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_OBJECT_MOVE_OUTSIDE\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_OBJECT_ALWAYS)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_OBJECT_ALWAYS\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_CREATURE_GROUP)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_CREATURE_GROUP\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_CREATURE_DIED		)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_CREATURE_DIED\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_CREATURE_WOKEN		)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_CREATURE_WOKEN\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_CREATURE_DAMAGE	)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_CREATURE_DAMAGE\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_CREATURE_CRITDAMAGE)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_CREATURE_CRITDAMAGE\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_OBJECT_GROUP		)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_OBJECT_GROUP\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_OBJECT_PICKUP		)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_OBJECT_PICKUP\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_OBJECT_PUTDOWN		)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_OBJECT_PUTDOWN\n");
		strcat(pc_buffer, str_add_buffer);
	}

	if (tco & TRIGGER_COND_OBJECT_USED		)
	{
		sprintf(str_add_buffer, "TRIGGER_COND_OBJECT_USED\n");
		strcat(pc_buffer, str_add_buffer);
	}

	int i_len = strlen(pc_buffer);
	Assert(i_len < i_buffer_len);
	return i_len;
}
#endif  //VER_TEST
