/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of CTimerTrigger defined in Trigger.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/TimerTrigger.cpp                                         $
 * 
 * 4     8/13/98 1:43p Mlange
 * The step message now requires registration of its recipients.
 * 
 * 3     7/20/98 11:27p Rwyatt
 * Added validation to all triggers
 * All changable data is now saved in the scene file
 * 
 * 2     7/06/98 3:04a Rwyatt
 * Added suitable defaults for the timer periods
 * 
 * 1     7/06/98 2:48a Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/

#include "Trigger.hpp"

#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Loader/SaveBuffer.hpp"


//*********************************************************************************************
//
// CTimerTrigger implementation.
//

	//*********************************************************************************************
	CTimerTrigger::CTimerTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CTrigger(pgon, p_load, h_object, pvtable, pinfo)
	{
		bool	b_initial = false;		// start of in a low state

		// Set the defaults to give a 1 second period for high and low. A fixed timer of 0.5Hz
		fMinLowTime = 1.0f;
		fMaxLowTime = 1.0f;
		fMinHighTime = 1.0f;
		fMaxHighTime = 1.0f;

		SETUP_TEXT_PROCESSING(pvtable, p_load)
		SETUP_OBJECT_HANDLE(h_object)
		{
			// get the initial state of the trigger
			bFILL_BOOL(b_initial, esInitialState);

			// read the minimum and maximum low and high periods
			bFILL_FLOAT(fMinHighTime, esMinHighTime);
			bFILL_FLOAT(fMaxHighTime, esMaxHighTime);
			bFILL_FLOAT(fMinLowTime, esMinLowTime);
			bFILL_FLOAT(fMaxLowTime, esMaxLowTime);

			Assert(fMinHighTime > 0.0f);
			Assert(fMaxHighTime > 0.0f);
			Assert(fMinLowTime > 0.0f);
			Assert(fMaxLowTime > 0.0f);

			Assert(fMinLowTime<=fMaxLowTime);
			Assert(fMinHighTime<=fMaxHighTime);
		}
		END_OBJECT;
		END_TEXT_PROCESSING;


		bState = false;
		if (b_initial)
		{
			// we want to start in a high state so set the state remain time to zero, the trigger
			// will switch to a high state on the first frame..
			fStateRemain = 0.0f;
		}
		else
		{
			// Pick a random time period for the trigger to be in a low state.
			if (fMinLowTime == fMaxLowTime)
			{
				fStateRemain = fMinLowTime;
			}
			else
			{
				// Pick a random time
				fStateRemain = CTrigger::rndRand(fMinLowTime, fMaxLowTime);
			}
		}

		// Register this entity with the message types it needs to receive.
		CMessageStep::RegisterRecipient(this);
	}


	//*********************************************************************************************
	CTimerTrigger::~CTimerTrigger()
	{
		CMessageStep::UnregisterRecipient(this);
	}


	//*********************************************************************************************
	void CTimerTrigger::Process(const CMessageStep& msg)
	{
		// The simulation must be going before we can get a collision message, if this is not the
		// case then something is seriously wrong.
		Assert(CMessageSystem::bSimulationGoing());

		// If this trigger is still active
		if (u4_Probability)
		{
			fStateRemain -= msg.sStaticStep;
			if (fStateRemain<=0.0f)
			{
				// time to switch state
				if (bState)
				{
					// currently high going to low
					bState = false;
					if (fMinLowTime == fMaxLowTime)
					{
						fStateRemain = fMinLowTime;
					}
					else
					{
						// Pick a random time
						fStateRemain = CTrigger::rndRand(fMinLowTime, fMaxLowTime);
					}
				}
				else
				{
					// currently low going to high
					bState = true;
					if (fMinHighTime == fMaxHighTime)
					{
						fStateRemain = fMinHighTime;
					}
					else
					{
						// Pick a random time
						fStateRemain = CTrigger::rndRand(fMinHighTime, fMaxHighTime);
					}
					AttemptTriggerFire();
				}
			}
		}

		// Call the base class step handler so it can do the fire delays etc etc
		CTrigger::Process(msg);
	}


	//*****************************************************************************************
	char* CTimerTrigger::pcSave
	(
		char*  pc_buffer
	) const
	//*********************************
	{
		pc_buffer = CTrigger::pcSave(pc_buffer);

		pc_buffer = pcSaveT(pc_buffer, fStateRemain);
		pc_buffer = pcSaveT(pc_buffer, bState);

		return pc_buffer;
	}


	//*****************************************************************************************
	const char* CTimerTrigger::pcLoad
	(
		const char*  pc_buffer
	)
	//*********************************
	{
		pc_buffer = CTrigger::pcLoad(pc_buffer);

		pc_buffer = pcLoadT(pc_buffer, &fStateRemain);
		pc_buffer = pcLoadT(pc_buffer, &bState);

		return pc_buffer;
	}

#if VER_TEST
	//*****************************************************************************************
	int CTimerTrigger::iGetDescription(char* pc_buffer, int i_buffer_len)
	{
		CTrigger::iGetDescription(pc_buffer, i_buffer_len);

		char str_buffer[512];

		strcat(pc_buffer, "\nCTimerTrigger:\n");

		sprintf(str_buffer,"Min High Time\t%f\n", fMinHighTime);
		strcat(pc_buffer, str_buffer);
		sprintf(str_buffer,"Max High Time\t%f\n", fMaxHighTime);
		strcat(pc_buffer, str_buffer);

		sprintf(str_buffer,"Min Low Time\t%f\n", fMinLowTime);
		strcat(pc_buffer, str_buffer);
		sprintf(str_buffer,"Max Low Time\t%f\n", fMaxLowTime);
		strcat(pc_buffer, str_buffer);

		sprintf(str_buffer,"Current State\t%s\n", bState?"High":"Low");
		strcat(pc_buffer, str_buffer);
		sprintf(str_buffer,"Current State Time Remain\t%f\n", fStateRemain);
		strcat(pc_buffer, str_buffer);

		int i_len = strlen(pc_buffer);
		Assert(i_len < i_buffer_len);
		return i_len;
	}
#endif


	//*****************************************************************************************
	bool CTimerTrigger::bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	)
	{
		return CTrigger::bValidateTriggerProperties(pgon, h_object,pvtable,p_load);
	}
