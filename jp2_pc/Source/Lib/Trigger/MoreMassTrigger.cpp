/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of CMoreMassTrigger defined in Trigger.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/MoreMassTrigger.cpp                                      $
 * 
 * 4     9/06/98 4:00p Agrant
 * more mass triggers never have actions, so we need only validate their fire expressions
 * 
 * 3     8/31/98 4:38p Agrant
 * When ignoring an animate, also ignore its boundary boxes
 * 
 * 2     8/30/98 12:22p Agrant
 * implemented more mass trigger
 * 
 * 1     8/29/98 9:09p Agrant
 * initial rev
 * 
 **********************************************************************************************/


#include "Common.hpp"
#include "Trigger.hpp"


#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/EntityDBase/Query/QPhysics.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/EntityDBase/Animate.hpp"
#include "Lib/Std/Hash.hpp"


//*********************************************************************************************
//
// CMoreMassTrigger implementation.
//


	//*********************************************************************************************
	CMoreMassTrigger::CMoreMassTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CTrigger(pgon, p_load, h_object, pvtable, pinfo)
	{
		u4Compare = 0;
		pmmtCompare = 0;
		fMassCompare = 0;


		SETUP_TEXT_PROCESSING(pvtable, p_load)
		SETUP_OBJECT_HANDLE(h_object)
		{
			const CEasyString* pestr = 0;
			if (bFILL_pEASYSTRING(pestr, esTarget))
			{
				AlwaysAssert(bInGroff(p_load, pestr->strData()));
				u4Compare = u4Hash(pestr->strData());
			}

			bFILL_FLOAT(fMass, esMass);

			int i = 0;
			for ( ; i < iNUM_IGNORE; ++i)
			{
				if (bFILL_pEASYSTRING(pestr, ESymbol(esA00+i)))
				{
					au4Ignore << u4Hash(pestr->strData());
					AlwaysAssert(bInGroff(p_load, pestr->strData()) || pwWorld->pinsFindInstance(au4Ignore[i]));
				}
				else
					break;
			}
		}
		END_OBJECT_HANDLE;
		END_TEXT_PROCESSING;
	}


	//*********************************************************************************************
	CMoreMassTrigger::~CMoreMassTrigger()
	{
	}


	//*********************************************************************************************
	bool CMoreMassTrigger::bEvaluateNow()
	{
		if (peeFireExpression)
		{
			if (!peeFireExpression->EvaluateExpression())
				return false;
		}

		float f_mass = fGetMass();
		float f_compare = fMassCompare;

		if (u4Compare)
		{
			if (!pmmtCompare)
			{
				CInstance* pins = pwWorld->ppartTriggerPartitionList()->pinsFindInstance(u4Compare);
				AlwaysAssert(dynamic_cast<CMoreMassTrigger*>(pins));
				pmmtCompare = (CMoreMassTrigger*)pins;
			}
		
			f_compare = pmmtCompare->fGetMass();
		}

		return f_mass > f_compare;
	}

	//*********************************************************************************************
	float CMoreMassTrigger::fGetMass()
	{
		// Are we current?
		if (sValidMass != CMessageStep::sStaticTotal)
		{
			// No!  Calculate mass!
			fMass = 0;

			// Query world database for all physics objects in the volume.
			CWDbQueryPhysics wqph(*pbvBoundingVol(), pr3Pres);

			int i;
			foreach(wqph)
			{
				const CInstance* pins_phys = wqph.tGet();

				uint32 u4_pins = pins_phys->u4GetUniqueHandle();
				bool b_add_mass = true;

				for (i = 0; i < au4Ignore.uLen; ++i)
				{
					if (au4Ignore[i] == u4_pins)
					{
						b_add_mass = false;
						break;
					}

					const CAnimate* pani = pins_phys->paniGetOwner();
					if (pani && au4Ignore[i] == pani->u4GetUniqueHandle())
					{
						b_add_mass = false;
						break;
					}
				}

				if (b_add_mass)
					fMass += pins_phys->pphiGetPhysicsInfo()->fMass(pins_phys);
			}
		}

		return fMass;
	}

		
		
	//*****************************************************************************************
	bool CMoreMassTrigger::bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	)
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

		}
		END_OBJECT;
		END_TEXT_PROCESSING

		// return true if we made it through all the tests
		return true;
	}

	
//*****************************************************************************************
char * CMoreMassTrigger::pcSave(char *  pc) const
{
	return CTrigger::pcSave(pc);
}


//*****************************************************************************************
const char * CMoreMassTrigger::pcLoad(const char *  pc)
{
	return CTrigger::pcLoad(pc);
}


#if VER_TEST
//*****************************************************************************************
int CMoreMassTrigger::iGetDescription(char* pc_buffer, int i_buffer_len)
{
//	char str_buffer[1024];
	CTrigger::iGetDescription(pc_buffer, i_buffer_len);

	strcat(pc_buffer, "\nCMoreMassTrigger:\n");

	int i_len;

	i_len = strlen(pc_buffer);
	Assert(i_len < i_buffer_len);
	return i_len;
}
#endif