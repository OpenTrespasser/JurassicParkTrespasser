/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Entity.hpp
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Entity.cpp                                            $
 * 
 * 23    10/02/98 6:06p Agrant
 * Save location of entity attached, even if CInstance does not wish to do so
 * 
 * 22    10/02/98 1:14p Agrant
 * fixed relative placement of text prop loaded entity attached
 * 
 * 21    10/01/98 4:05p Mlange
 * Improved move message stat reporting.
 * 
 * 20    9/26/98 9:14p Agrant
 * bump up player head a little more to avoid going under terrain
 * 
 * 19    9/24/98 7:49p Mlange
 * Now uses TIN height query to keep camera above terrain.
 * 
 * 18    9/23/98 10:23p Agrant
 * safety feature for Anne's head, again
 * 
 * 17    9/23/98 6:50p Agrant
 * Anne's head above the terrain please
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Entity.hpp"
#include "Animate.hpp"
#include "MessageTypes/MsgMove.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Std/Hash.hpp"
#include "Lib/Physics/PhysicsImport.hpp"

#include "WorldDBase.hpp"
#include "Query/QTerrain.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "Game/DesignDaemon/Player.hpp"

//*********************************************************************************************
//
// CEntityAttached implementation.
//

	//*****************************************************************************************
	const char* CEntity::strPartType() const
	{
		return "Entity";
	}


//*********************************************************************************************
//
// CEntityAttached implementation.
//

	CEntityAttached::CEntityAttached
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) : CEntity(pgon, pload, h_object, pvtable,pinfo)
	{
		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_object)
		{
			const CEasyString* pestr = 0;

			bFILL_pEASYSTRING(pestr, esTarget);
			AlwaysAssert(pestr);
			CInstance* pins = pwWorld->pinsFindInstance(u4Hash(pestr->strData()));
			AlwaysAssert(pins);

			pinsAttached_ = pins;
			paniAttached_ = 0;
		}
		END_OBJECT_HANDLE;
		END_TEXT_PROCESSING;

		CPlacement3<> p3_this = pr3Presence();
		CPlacement3<> p3_attached = pinsAttached_->pr3Presence();
		p3Relative_ = CPlacement3<>(p3_this * ~p3_attached);

		UpdateFromAttached();

		CMessageMove::RegisterRecipient(this);

		// HACK HACK HACK
		if (pinsAttached_ == gpPlayer)
		{
			AlwaysAssert(gpPlayer->pinsAttached == 0);
			gpPlayer->pinsAttached = this;
		}
	}

	//*****************************************************************************************
	CEntityAttached::CEntityAttached()
	{
		pinsAttached_ = 0;
		paniAttached_ = 0;

		CMessageMove::RegisterRecipient(this);
	}

	//*****************************************************************************************
	CEntityAttached::CEntityAttached(const SInit& initins, CInstance* pins_attached, bool b_head, const CPlacement3<>& p3)
		: CEntity(initins), pinsAttached_(pins_attached), paniAttached_(0), p3Relative_(p3)
	{
		if (pins_attached)
		{
			char buf[128];
			pins_attached->strGetUniqueName(buf);
			SetInstanceName(buf);

			if (b_head)
				paniAttached_ = ptCast<CAnimate>(const_cast<CInstance*>(pinsAttached_));
		}

		UpdateFromAttached();

		CMessageMove::RegisterRecipient(this);
	}

	//******************************************************************************************
	CEntityAttached::~CEntityAttached()
	{
		CMessageMove::UnregisterRecipient(this);
	}

	//******************************************************************************************
	void CEntityAttached::SetAttached(CInstance* pins_attached, bool b_head)
	{
		pinsAttached_ = pins_attached;

		// If attaching to head, store the CAnimate pointer, if it is one.
		if (b_head)
			paniAttached_ = ptCast<CAnimate>(const_cast<CInstance*>(pinsAttached_));
		else
			paniAttached_ = 0;

		UpdateFromAttached();
	}

	//******************************************************************************************
	void CEntityAttached::SetRelative(const CPlacement3<>& p3_relative)
	{
		p3Relative_ = p3_relative;
		UpdateFromAttached();
	}

	//*****************************************************************************************
	void CEntityAttached::Move(const CPlacement3<>& p3_new, CEntity* pet_sender)
	{
		// Control goes both ways.  Move my attached (which will then move me).
		if (paniAttached_)
		{
			// Attached to head.
			// Modify by inverse head placement and relative placement.
			pinsAttached_->Move(~p3Relative_ * ~paniAttached_->p3HeadPlacement() * p3_new, this);
		}
		else if (pinsAttached_)
		{
			// Modify by relative placement.
			pinsAttached_->Move(~p3Relative_ * p3_new, this);
		}
		else
		{
			// G'ahead and move myself.
			CEntity::Move(p3_new, pet_sender);
		}
	}

	extern CProfileStat psMoveMsgEntityAttached;
	
	//*****************************************************************************************
	void CEntityAttached::Process(const CMessageMove& msgmove)
	{
		CTimeBlock tmb(&psMoveMsgEntityAttached);

		// Intercept all objects' move messages.  If it's my attached, update myself.
		if (msgmove.pinsMover == pinsAttached_ && pinsAttached_ != this 
			&& msgmove.etType == CMessageMove::etMOVED)
			UpdateFromAttached();
	}

	//*****************************************************************************************
	void CEntityAttached::UpdateFromAttached()
	{
		if (pinsAttached_)
		{
			CPlacement3<> p3 = pinsAttached_->pr3Presence();

			if (paniAttached_ && ptCast<CCamera>(this))
			{
				// Pre-concatenate head placement.
				p3 = paniAttached_->p3HeadPlacement() * p3;
			}

			CPlacement3<> p3_world = p3Relative_ * p3;

			if (paniAttached_ && ptCast<CCamera>(this) && ptCast<CPlayer>(paniAttached_))
			{
				// HACK HACK HACK
				//  Do not allow the player's head to drop below the terrain in run mode.

				// We were attached to the player.  Make sure that the camera is at least a bit
				//  above the terrain.
				TReal r_floor = 0.05f;
				
 				if (CWDbQueryTerrain().tGet() != 0)
					r_floor += CWDbQueryTerrain().tGet()->rHeightTIN(p3_world.v3Pos.tX, p3_world.v3Pos.tY);

				if (r_floor > p3_world.v3Pos.tZ)
					p3_world.v3Pos.tZ = r_floor;
			}

			// Call base class to must move entity, without moving parent.
			CEntity::Move(p3_world, this);
		}
	}

	//*****************************************************************************************
	const char* CEntityAttached::strPartType() const
	{
		return "EntityAttached";
	}

	//*****************************************************************************************
	char *CEntityAttached::pcSave(char * pc) const
	{
		pc = CInstance::pcSave(pc);
		pc = pr3Pres.pcSave(pc);
		pc = p3Relative_.pcSave(pc);
		pc = pcSaveInstancePointer(pc, pinsAttached_);

		return pc;
	}

	//*****************************************************************************************
	const char *CEntityAttached::pcLoad(const char * pc)
	{
		pc = CInstance::pcLoad(pc);

		// Override the CInstance loaded position-  this will make us load even if 
		//  we are intangible.
		if (CSaveFile::iCurrentVersion >= 20)
		{
			pc = pr3Pres.pcLoad(pc);
		}

		pc = p3Relative_.pcLoad(pc);
		pc = pcLoadInstancePointer(pc, &pinsAttached_);

		return pc;
	}
