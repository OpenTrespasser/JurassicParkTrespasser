/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: The implementation of Animal.hpp.
 *
 * Bugs:
 *
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Animal.cpp                                           $
 * 
 * 52    10/01/98 6:49p Mlange
 * Improved collision message stats.
 * 
 * 51    10/01/98 4:05p Mlange
 * Improved move message stat reporting.
 * 
 * 50    9/08/98 7:37p Agrant
 * delay visual wakeup by using a flag
 * 
 * 49    8/31/98 5:06p Agrant
 * Double wake up distance for dinos in view cone
 * 
 * 48    98/08/25 19:22 Speter
 *  Removed unused HandleColliision; added pins_me to HandleDamage. 
 * 
 *********************************************************************************************/

#include "Common.hpp"
#include "Animal.hpp"

#include "Lib/EntityDBase/MessageLog.hpp"
#include "Game/AI/Brain.hpp"
#include "Game/AI/AIInfo.hpp"
#include "PhysicsInfo.hpp"
#include "Game/AI/MentalState.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgCollision.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgTrigger.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"

#include "Game/DesignDaemon/Player.hpp"

#include "Lib/Sys/Profile.hpp"

extern CProfileStat psMoveMsgBrain;
extern CProfileStat psCollisionMsgBrain;


//*********************************************************************************************
//
// CAnimal implementation.
//
	
	//******************************************************************************************
	CAnimal::CAnimal()
		: pbrBrain(0)
	{
		pbrBrain = new CBrain(this, earCARNIVORE, strGetInstanceName() );////  //lint !e1732 !e1733

		apbbBoundaryBoxes.Fill(0);

		gaiSystem.AddAnimal(this);

		// Register this entity with the message types it needs to receive.
		CMessagePhysicsReq::RegisterRecipient(this);
		      CMessageMove::RegisterRecipient(this);
		 CMessageCollision::RegisterRecipient(this);
		     CMessageDeath::RegisterRecipient(this);
	}

	//******************************************************************************************
	CAnimal::CAnimal
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CAnimate(pgon, pload, h_object, pvtable, pinfo)
	{
		pbrBrain = new CBrain(this, pgon, pload, h_object, pvtable, pinfo);


		// And let the AI system know about us.
		gaiSystem.AddAnimal(this);

		// Register this entity with the message types it needs to receive.
		      CMessageMove::RegisterRecipient(this);
		 CMessageCollision::RegisterRecipient(this);
		     CMessageDeath::RegisterRecipient(this);
		CMessagePhysicsReq::RegisterRecipient(this);
	}

	//******************************************************************************************
	CAnimal::~CAnimal()
	{
		CMessagePhysicsReq::UnregisterRecipient(this);
		     CMessageDeath::UnregisterRecipient(this);
		 CMessageCollision::UnregisterRecipient(this);
		      CMessageMove::UnregisterRecipient(this);

		// Do not remove the animal, since things only get deleted when we purge the WDBase.  
		//  It will get removed when the AI system itself is deleted.
		//gaiSystem.RemoveAnimal(this);

		delete pbrBrain;
	}

	//******************************************************************************************
	void CAnimal::SetEmotions(const CFeeling& feel)
	{
		Assert(pbrBrain);

		for (int i = eptEND - 1; i >= 0; --i)
		{
			if (feel[i] >= 0)
				pbrBrain->pmsState->feelEmotions[i] = feel[i];
		}
	}

	//******************************************************************************************
	CVector3<> CAnimal::v3GetHeadPos() const
	{
		// Lame, but functional.  
		const CPhysicsInfo* pphi = pphiGetPhysicsInfo();

		if (pphi)
		{
			return pphi->v3GetHeadPosition(this);
		}
		else
			return v3Pos();
	}

	//******************************************************************************************
	CVector3<> CAnimal::v3GetTailPos() const
	{
		// Lame, but functional.  
		const CPhysicsInfo* pphi = pphiGetPhysicsInfo();

		if (pphi)
		{
			return pphi->v3GetTailPosition(this);
		}
		else
			return v3Pos();
	}

	//*****************************************************************************************
	void CAnimal::PreRender(CRenderContext& renc)
	{
		// Let this dino know that he should wake up as as soon as possible.
		pbrBrain->bWakeMe = true;
	}

	//******************************************************************************************
	void CAnimal::Process(const CMessagePhysicsReq& msgpr)
	{
		// Pass it on to the physics model.
		Assert(pphiGetPhysicsInfo());
		pphiGetPhysicsInfo()->HandleMessage(msgpr, this);
	}

	//******************************************************************************************
	void CAnimal::Process(const CMessageCollision& msgcoll)
	{
		CTimeBlock tmb(&psCollisionMsgBrain);

		// Ignore collisions involving immovable objects.
		if (msgcoll.pins1 && msgcoll.pins1->pphiGetPhysicsInfo() && !msgcoll.pins1->pphiGetPhysicsInfo()->bIsMovable())
			return;
		if (msgcoll.pins2 && msgcoll.pins2->pphiGetPhysicsInfo() && !msgcoll.pins2->pphiGetPhysicsInfo()->bIsMovable())
			return;
				
		CAnimate::Process(msgcoll);
	}

	//******************************************************************************************
	void CAnimal::Process(const CMessageMove& msgmv)
	{
		// Not necessary at this time.
		Assert(pbrBrain);

		CTimeBlock tmb(&psMoveMsgBrain);
		pbrBrain->HandleMessage(msgmv);
	}

	//******************************************************************************************
	void CAnimal::Process(const CMessageDelete& msgdel)
	{
		Assert(pbrBrain);
		pbrBrain->HandleMessage(msgdel);
	}

	//******************************************************************************************
	void CAnimal::Process(const CMessageDeath& msgdeath)
	{
		Assert(pbrBrain);
		pbrBrain->HandleMessage(msgdeath);
	}

	//******************************************************************************************
	void CAnimal::HandleDamage(float f_damage, const CInstance* pins_aggressor, const CInstance* pins_me)
	{
		if (bDead())
			return;

		Assert(pbrBrain);
		pbrBrain->HandleDamage(f_damage, pins_aggressor);

		// Temporarily spit out a message saying that the animal has been hit.
		conMessageLog.Print("An animal is hit and has %1.1f hit points left\n", fGetHitPoints());
	}

	//*****************************************************************************************
	CInstance* CAnimal::pinsCopy() const
	{
//		AlwaysAssert(false);
		return 0;
	}

#if VER_TEST
	//*****************************************************************************************
	int CAnimal::iGetDescription(char *buffer, int i_buffer_length)
	{
		int i_len = CAnimate::iGetDescription(buffer, i_buffer_length);

		int i_brain_len = pbrBrain->iGetDescription(buffer + i_len, i_buffer_length - i_len);

		Assert(i_len + i_brain_len < i_buffer_length);
		return i_len + i_brain_len;
	}
#endif
	
	//*****************************************************************************************
	char *CAnimal::pcSave(char * pc_buffer) const
	{
		pc_buffer = CAnimate::pcSave(pc_buffer);


		pc_buffer = pbrBrain->pcSave(pc_buffer);
		return pc_buffer;
	}

	//*****************************************************************************************
	const char* CAnimal::pcLoad(const char* pc_buffer)
	{
		// Animal version four files use CAnimate.  Else use CInstance
		extern int iAnimalVersion;
		if (iAnimalVersion >= 4)
			pc_buffer = CAnimate::pcLoad(pc_buffer);
		else
			pc_buffer = CInstance::pcLoad(pc_buffer);

		pc_buffer = pbrBrain->pcLoad(pc_buffer);
		return pc_buffer;
	}
