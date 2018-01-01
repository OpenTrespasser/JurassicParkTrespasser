/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: The implementation of WaterDisturbance.hpp.
 *
 * Bugs:
 *
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/WaterDisturbance.cpp                                     $
 * 
 * 19    9/18/98 3:10p Agrant
 * save/load water disturbances properly
 * 
 * 18    8/13/98 1:41p Mlange
 * The step message now requires registration of their recipients.
 * 
 * 17    98/07/30 22:38 Speter
 * CWDbQueryWater -> CWDbQueryWaterHeight.
 * 
 * 16    7/20/98 10:14p Rwyatt
 * Removed description text in final mode
 * 
 * 15    98/03/09 21:55 Speter
 * Removed test of bIsRunning...water knows what to do.
 * 
 * 14    12/15/97 3:03p Agrant
 * Water Disturbance descriptions
 * 
 *********************************************************************************************/

#include "Common.hpp"
#include "WaterDisturbance.hpp"

#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Query/QWater.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Lib/Loader/SaveFile.hpp"


CRandom rndDisturb(22);

//*********************************************************************************************
//
// CWaterDisturbance implementation.
//
	
	//******************************************************************************************
	CWaterDisturbance::CWaterDisturbance
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CEntity(pgon, pload, h_object, pvtable, pinfo)
	{
		// Should interpret text props.  For now, will simply use defaults.

		// No water object yet.
		pewWater = 0;

		rSize = fGetScale();
		rStrength = 0.5f;
		sInterval = 0.25f;

		SETUP_TEXT_PROCESSING(pvtable, pload)
		IF_SETUP_OBJECT_HANDLE(h_object)
		{
			bFILL_FLOAT(rStrength, esStrength);
			bFILL_FLOAT(sInterval, esInterval);
			bFILL_FLOAT(rSize, esRadius);
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		// Register this entity with the message types it needs to receive.
		CMessageStep::RegisterRecipient(this);
 	}

	//******************************************************************************************
	CWaterDisturbance::~CWaterDisturbance()
	{
		CMessageStep::UnregisterRecipient(this);
	}

	//******************************************************************************************
	void CWaterDisturbance::Process(const CMessageStep&)
	{
		if (pewWater == 0)
		{
			// Find a water overlapping this.
			CWDbQueryWaterHeight wqwtrh(v3Pos());

			pewWater = wqwtrh.petWater;

			sNextDisturbance = CMessageStep::sStaticTotal + rndDisturb(0.0, sInterval);
		}
		else
		{
			while (CMessageStep::sStaticTotal > sNextDisturbance)
			{
				// Yes, it is somewhat frame rate dependent, but it is safer.
				sNextDisturbance = CMessageStep::sStaticTotal + sInterval;
				// sNextDisturbance += sInterval;

				pewWater->CreateDisturbance
				(
					v3Pos(),
					rSize,
					rStrength,
					true
				);
			}
		}
	}


	//*****************************************************************************************
	char * CWaterDisturbance::pcSave(char *  pc) const
	{
		pc = pcSaveT(pc, sNextDisturbance);
		return pc;
	}

	//*****************************************************************************************
	const char * CWaterDisturbance::pcLoad(const char *  pc)
	{
		if (CSaveFile::iCurrentVersion >= 16)
		{
			pc = pcLoadT(pc, &sNextDisturbance);
		}
		return pc;
	}


#if VER_TEST	
 	//*****************************************************************************************
	int CWaterDisturbance::iGetDescription(char* pc_buffer, int i_buffer_len)
	{
		CInstance::iGetDescription(pc_buffer, i_buffer_len);

		char str_buffer[512];

		strcat(pc_buffer, "\nCWaterDisturbance:\n");

		sprintf(str_buffer,"rSize\t%f\n", rSize);
		strcat(pc_buffer, str_buffer);
	
		sprintf(str_buffer,"rStrength\t%f\n", rStrength);
		strcat(pc_buffer, str_buffer);

		sprintf(str_buffer,"sInterval\t%f\n", sInterval);
		strcat(pc_buffer, str_buffer);

		int i_len = strlen(pc_buffer);
		Assert(i_len < i_buffer_len);
		return i_len;
	}
#endif
