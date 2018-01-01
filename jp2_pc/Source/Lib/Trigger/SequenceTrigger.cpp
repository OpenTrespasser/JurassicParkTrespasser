/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of CSequenceTrigger defined in Trigger.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/SequenceTrigger.cpp                                      $
 * 
 * 7     8/25/98 6:26p Rvande
 * Fixed numerous mis-scoped loop variables
 * 
 * 6     8/12/98 3:40p Mlange
 * The CMessageTrigger and CMessageMoveTriggerTo message types now use the new registration
 * scheme.
 * 
 * 5     7/20/98 11:27p Rwyatt
 * Added validation to all triggers
 * All changable data is now saved in the scene file
 * 
 * 4     7/14/98 1:45p Mlange
 * Now uses text prop array syntax to specify sequence trigger names.
 * 
 * 3     7/06/98 1:34p Mlange
 * Additional sequence trigger features.
 * 
 * 2     7/03/98 8:34p Agrant
 * Triggers need a different lookup function
 * 
 **********************************************************************************************/

#include "Trigger.hpp"

#include <string.h>
#include <vector.h>
#include "Lib/EntityDBase/MessageTypes/MsgTrigger.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Lib/Std/Hash.hpp"


namespace
{
	void ParseTextProps
	(
		vector<string>& rvcstr_listen,
		vector<string>& rvcstr_order,
		vector<string>& rvcstr_eval_now,
		string& rstr_eval_false,
		const CHandle& h_object,
		CValueTable* pvtable,
		CLoadWorld* p_load
	)
	{
		SETUP_TEXT_PROCESSING(pvtable, p_load)
		SETUP_OBJECT_HANDLE(h_object)
		{
			IF_SETUP_OBJECT_SYMBOL(esSequenceListenNames)
			{
				// Now loop through the array entries
				for (int i_index = 0; true; i_index++)
				{
					const CEasyString* pestr = 0;
					if (bFILL_pEASYSTRING(pestr, ESymbol(esA00 + i_index)))
						rvcstr_listen.push_back(string(pestr->strData()));
					else
						break;
				}
			}
			END_OBJECT;

			IF_SETUP_OBJECT_SYMBOL(esSequenceOrderNames)
			{
				// Now loop through the array entries
				for (int i_index = 0; true; i_index++)
				{
					const CEasyString* pestr = 0;
					if (bFILL_pEASYSTRING(pestr, ESymbol(esA00 + i_index)))
						rvcstr_order.push_back(string(pestr->strData()));
					else
						break;
				}
			}
			END_OBJECT;

			IF_SETUP_OBJECT_SYMBOL(esSequenceEvalNowNames)
			{
				// Now loop through the array entries
				for (int i_index = 0; true; i_index++)
				{
					const CEasyString* pestr = 0;
					if (bFILL_pEASYSTRING(pestr, ESymbol(esA00 + i_index)))
						rvcstr_eval_now.push_back(string(pestr->strData()));
					else
						break;
				}
			}
			END_OBJECT;

			const CEasyString* pestr_false = 0;
			if (bFILL_pEASYSTRING(pestr_false, esSequenceFalseTriggerName))
				rstr_eval_false = string(pestr_false->strData());
		}
		END_OBJECT;
		END_TEXT_PROCESSING;
	}
}


//*********************************************************************************************
//
// CSequenceTrigger implementation.
//

	//*********************************************************************************************
	CSequenceTrigger::CSequenceTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CTrigger(pgon, p_load, h_object, pvtable, pinfo)
	{
		vector<string> vcstr_listen;
		vector<string> vcstr_order;
		vector<string> vcstr_eval_now;
		string str_false;

		ParseTextProps(vcstr_listen, vcstr_order, vcstr_eval_now, str_false, h_object, pvtable, p_load);


		ptrSequenceFalse = 0;

		if (str_false.length() != 0)
			ptrSequenceFalse = ptCast<CTrigger>(pwWorld->ppartTriggerPartitionList()->pinsFindInstance(u4Hash(str_false.c_str())));

		paptrListen = CPArray<const CTrigger*>(vcstr_listen.size());

		int i = 0;
		vector<string>::const_iterator it = vcstr_listen.begin();
		for (; it != vcstr_listen.end(); ++it)
		{
			CTrigger* ptr = ptCast<CTrigger>(pwWorld->ppartTriggerPartitionList()->pinsFindInstance(u4Hash((*it).c_str())));
			Assert(ptr);
			paptrListen[i++] = ptr;
		}

		paptrOrder = CPArray<const CTrigger*>(vcstr_order.size());

		i = 0;
		for (it = vcstr_order.begin(); it != vcstr_order.end(); ++it)
		{
			CTrigger* ptr = ptCast<CTrigger>(pwWorld->ppartTriggerPartitionList()->pinsFindInstance(u4Hash((*it).c_str())));
			Assert(ptr);
			paptrOrder[i++] = ptr;
		}

		if (vcstr_eval_now.size() != 0)
		{
			paptrEvalNow = CPArray<const CTrigger*>(vcstr_eval_now.size());

			i = 0;
			for (it = vcstr_eval_now.begin(); it != vcstr_eval_now.end(); ++it)
			{
				CTrigger* ptr = ptCast<CTrigger>(pwWorld->ppartTriggerPartitionList()->pinsFindInstance(u4Hash((*it).c_str())));
				Assert(ptr);
				paptrEvalNow[i++] = ptr;
			}
		}
		else
			paptrEvalNow = CPArray<const CTrigger*>();

		maptrCurrentOrder = CMArray<const CTrigger*>(vcstr_order.size());

		// Register this entity with the message types it needs to receive.
		CMessageTrigger::RegisterRecipient(this);
	}


	//*********************************************************************************************
	CSequenceTrigger::~CSequenceTrigger()
	{
		CMessageTrigger::UnregisterRecipient(this);

		delete[] paptrListen.atArray;
		delete[] paptrOrder.atArray;
		delete[] paptrEvalNow.atArray;
		delete[] maptrCurrentOrder.atArray;

		paptrListen       = CPArray<const CTrigger*>();
		paptrOrder        = CPArray<const CTrigger*>();
		paptrEvalNow      = CPArray<const CTrigger*>();
		maptrCurrentOrder = CMArray<const CTrigger*>();
	}


	//*********************************************************************************************
	void CSequenceTrigger::Process(const CMessageTrigger& trigmsg)
	{
		CTrigger::Process(trigmsg);

		// Is this trigger one we listen to?
		int i;
		for (i = 0; i < paptrListen.uLen; i++)
			if (paptrListen[i] == trigmsg.ptrGetActivatedTrigger())
				break;

		if (i != paptrListen.uLen)
		{
			// Add it to the current sequence.
			maptrCurrentOrder << trigmsg.ptrGetActivatedTrigger();

			// Determine if this trigger signifies a forced evaluation.
			int i_eval_now;
			for (i_eval_now = 0; i_eval_now < paptrEvalNow.uLen; i_eval_now++)
				if (paptrEvalNow[i_eval_now] == trigmsg.ptrGetActivatedTrigger())
					break;

			bool b_eval_now = i_eval_now != paptrEvalNow.uLen;

			// If this is a forced evaluation or if the sequence is full, we evaluate its order.
			if (b_eval_now || maptrCurrentOrder.uLen == paptrOrder.uLen)
			{
				int i_order;
				for (i_order = 0; i_order < maptrCurrentOrder.uLen; i_order++)
					if (maptrCurrentOrder[i_order] != paptrOrder[i_order])
						break;

				// If the entire sequence is in correct order, fire.
				if (i_order == paptrOrder.uLen)
					AttemptTriggerFire();
				else if (ptrSequenceFalse)
					ptrSequenceFalse->AttemptTriggerFire();

				// Reset sequence.
				maptrCurrentOrder.Reset();
			}
		}
	}


	//*****************************************************************************************
	char * CSequenceTrigger::pcSave(char *  pc) const
	{
		// Save the length of the current input sequence
		pc = pcSaveT(pc, maptrCurrentOrder.uLen);

		// Save out the elements of the current sequence
		for (int i_order = 0; i_order < maptrCurrentOrder.uLen; i_order++)
			pc = pcSaveInstancePointer(pc, maptrCurrentOrder[i_order]);

		return CTrigger::pcSave(pc);
	}


	//*****************************************************************************************
	const char * CSequenceTrigger::pcLoad(const char *  pc)
	{
		int i_len;

		// Remove all current entries from
		maptrCurrentOrder.Reset();

		// Get the length of the saved sequence.
		pc = pcLoadT(pc, &i_len);

		// Add the saved elements back to the current order sequence
		while (i_len>0)
		{
			CTrigger*	ptr_local;
			pc = pcLoadInstancePointer(pc, (CInstance**)&ptr_local);
			maptrCurrentOrder << ptr_local;
			i_len--;
		}

		return CTrigger::pcLoad(pc);
	}


#if VER_TEST
	//*****************************************************************************************
	int CSequenceTrigger::iGetDescription(char* pc_buffer, int i_buffer_len)
	{
		CTrigger::iGetDescription(pc_buffer, i_buffer_len);

		strcat(pc_buffer, "\nCSequenceTrigger:\n");

		int i_len = strlen(pc_buffer);

		Assert(i_len < i_buffer_len);
		return i_len;
	}
#endif


	//*****************************************************************************************
	bool CSequenceTrigger::bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	)
	{
		vector<string> vcstr_listen;
		vector<string> vcstr_order;
		vector<string> vcstr_eval_now;
		string str_false;

		ParseTextProps(vcstr_listen, vcstr_order, vcstr_eval_now, str_false, h_object, pvtable, p_load);


#if VER_DEBUG
		for (vector<string>::const_iterator it_order = vcstr_order.begin(); it_order != vcstr_order.end(); ++it_order)
		{
			vector<string>::const_iterator it_listen = vcstr_listen.begin();
			for (; it_listen != vcstr_listen.end(); ++it_listen)
				if ((*it_order) == (*it_listen))
					break;

			// Each trigger in the sequence must also be specified in the list of triggers listened to.
			Assert(it_listen != vcstr_listen.end());
		}

		for (vector<string>::const_iterator it_eval = vcstr_eval_now.begin(); it_eval != vcstr_eval_now.end(); ++it_eval)
		{
			vector<string>::const_iterator it_listen = vcstr_listen.begin();
			for (; it_listen != vcstr_listen.end(); ++it_listen)
				if ((*it_eval) == (*it_listen))
					break;

			// Each trigger in the evaluate now sequence must also be specified in the list of triggers listened to.
			Assert(it_listen != vcstr_listen.end());
		}

		for (vector<string>::const_iterator it_ver = vcstr_listen.begin(); it_ver != vcstr_listen.end(); ++it_ver)
		{
			// Make sure every trigger in the sequence exists in the GROFF file.
			CGroffObjectName* pgon = p_load->goiInfo.pgonFindObject((*it_ver).c_str());

			Assert(pgon != 0);
		}

		// Make sure the evaluate false trigger exists in the GROFF file.
		if (str_false.length() != 0)
		{
			Assert(p_load->goiInfo.pgonFindObject(str_false.c_str()));
		}
#endif

		// Determine if each trigger in the listen sequence has been loaded.
		for (vector<string>::const_iterator it = vcstr_listen.begin(); it != vcstr_listen.end(); ++it)
		{
			CInstance* pins = pwWorld->ppartTriggerPartitionList()->pinsFindInstance(u4Hash((*it).c_str()));

			if (!pins)
				return false;

			// Make sure it is actually a trigger.
			Assert(ptCast<CTrigger>(pins));
		}

		// Determine if the evaluate false trigger has been loaded.
		if (str_false.length() != 0)
		{
			CInstance* pins = pwWorld->ppartTriggerPartitionList()->pinsFindInstance(u4Hash(str_false.c_str()));

			if (!pins)
				return false;

			// Make sure it is actually a trigger.
			Assert(ptCast<CTrigger>(pins));
		}

		// Validate the base class
		return CTrigger::bValidateTriggerProperties(pgon, h_object,pvtable,p_load);
	}
