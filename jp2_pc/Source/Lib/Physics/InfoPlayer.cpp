/**********************************************************************************************
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of InfoBio.hpp
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/InfoPlayer.cpp                                           $
 * 
 * 105   98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 104   98/09/15 4:01 Speter
 * Moved some functionality to new pure CPlayer.
 * 
 * 103   98/09/04 21:58 Speter
 * Moved PlayerSettings to separate file.
 * 
 * 102   98/09/02 21:19 Speter
 * Removed i_activity param from Activate().
 * 
 * 101   98/08/31 19:02 Speter
 * Moved hand tracking code from UpdateWDBase and CPlayer::AdjustHead to CPlayer::Move.
 * 
 * 100   98/08/05 20:47 Speter
 * Fixed bug in teleport.
 * 
 * 99    98/07/24 14:07 Speter
 * New bUpdating() function.
 * 
 * 98    98/07/20 13:19 Speter
 * No longer drop on teleport.
 * 
 * 97    98/07/14 20:17 Speter
 * Removed "Tilted" messages.
 * 
 * 96    98/07/08 18:34 Speter
 * Added bVER_BONES() switch.
 * 
 * 95    98/06/25 17:38 Speter
 * Implemented several suboptions for DrawPhysics (bones). RayIntersect function changed to
 * take explicity subobject index.
 * 
 * 94    98/06/19 17:50 Speter
 * Made UI functions part of CPlayer class.
 * 
 * 93    98/06/16 20:55 Speter
 * Restored old methods of tracking hand, and allowing max rotation.
 * 
 *********************************************************************************************/

//
// Includes.
//

#include "common.hpp"
#include "InfoPlayer.hpp"

#include "VerBones.hpp"
#include "PhysicsHelp.hpp"
#include "PhysicsSystem.hpp"
#include "pelvis.h"

#include "Lib/GeomDBase/Shape.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Lib/Sys/DebugConsole.hpp"

//**********************************************************************************************
//
//  CPhysicsInfoPlayer implementation
//

	//*****************************************************************************************
	CPhysicsInfoPlayer::CPhysicsInfoPlayer(CPlayer* pplay)
		// Just pass the rendering box for now; mass doesn't need to be set for player model.
		: CPhysicsInfoHuman
		(
			pplay->prdtGetRenderInfo() ? 
				CPhysicsInfoBox(*pplay->prdtGetRenderInfo()->bvGet().pbvbCast(), SPhysicsData(true, true))
				: CPhysicsInfoBox()
		)
	{
		tmatSoundMaterial = matHashIdentifier("Anne");
	}

	//*****************************************************************************************
	void CPhysicsInfoPlayer::Activate
	(
		CInstance *pins, bool b_just_update, const CPlacement3<>&
	) const
	{
		// Don't wake up the player's physics is turned off.
		CPlayer* pplay = ptCast<CPlayer>(pins);
		if (pplay && !pplay->bPhysics)
			return;

		TParent::Activate(pins, b_just_update);
	}

	//*****************************************************************************************
	void CPhysicsInfoPlayer::Deactivate(CInstance *pins) const
	{
		// Don't do it if the player's physics is turned off.
		CPlayer* pplay = ptCast<CPlayer>(pins);
		if (pplay && !pplay->bPhysics)
			return;

		TParent::Deactivate(pins);
	}

	//*****************************************************************************************
	void CPhysicsInfoPlayer::UpdateWDBase(CInstance* pins, int i_index) const
	{
		// Perform parent's update.
		TParent::UpdateWDBase(pins, i_index);
	}

	//*****************************************************************************************
	void CPhysicsInfoPlayer::DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const
	{
#if bVER_BONES()
		// Call parent.
		TParent::DrawPhysics(pins, draw, cam);

		if (setedfMain[edfSKELETONS])
		{
			// Let player do the specialised drawing.
			CPlayer* pplay = ptCast<CPlayer>(pins);
			Assert(pplay);
			if (!pplay->bPhysics)
				return;

			pplay->DrawPhysics(draw, cam);
		}
#endif
	}
