/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CPhysicsInfoPlayer
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/InfoPlayer.hpp                                            $
 * 
 * 35    98/09/04 21:58 Speter
 * Moved PlayerSettings to separate file.
 * 
 * 34    98/09/02 21:19 Speter
 * Removed i_activity param from Activate().
 * 
 * 33    98/08/30 21:19 Speter
 * Added quick-throw.
 * 
 * 32    98/08/22 21:46 Speter
 * Support for the inhandgible. 
 * 
 * 31    98/08/16 0:07 Speter
 * Added back-stow, and some 2-stage stow motions.
 * 
 * 30    98/08/13 17:03 Speter
 * Added params for back-stow.
 * 
 * 29    98/08/13 14:45 Speter
 * Added inventory.
 * 
 * 28    98/08/09 19:49 Speter
 * Added ang2HandTurnMax.
 * 
 * 27    98/08/05 20:42 Speter
 * Added v3NeckOffset, for head pivot.
 * 
 * 26    98/07/02 16:08 Speter
 * Made max angle ortation for pickup a separate field.
 * 
 * 25    98/06/25 17:37 Speter
 * Removed v3ShoulderOffset; now get from skeleton.
 * 
 * 24    98/06/19 17:48 Speter
 * Made UI functions part of CPlayer class.
 * 
 * 23    98/06/11 23:24 Speter
 * Added more options, removed unused ones.
 * 
 * 22    98/06/09 21:26 Speter
 * Added needed include, opaque types.
 * 
 * 21    98/05/22 13:12 Speter
 * Added support for flail mode.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_INFOPLAYER_HPP
#define HEADER_LIB_PHYSICS_INFOPLAYER_HPP

#include "InfoSkeleton.hpp"
//**********************************************************************************************
//
class CPhysicsInfoPlayer: public CPhysicsInfoHuman
//
// Prefix: phip
//
// Class holding physics info unique to the player.  Not shared.
//
//**************************************
{
public:
	typedef CPhysicsInfoHuman	TParent;

	//*****************************************************************************************
	//
	CPhysicsInfoPlayer
	(
		CPlayer* pplay					// Player instance.
	);
	//
	// Constructs a player physics model, and initialises player with appropriate info.
	//
	//**********************************

	//*****************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual void Activate
	(
		CInstance *pins, 
		bool b_just_update = false, 
		const CPlacement3<>& p3_vel = p3VELOCITY_ZERO
	) const override;

	//*****************************************************************************************
	virtual void Deactivate(CInstance *pins) const override;

	//*****************************************************************************************
	virtual void UpdateWDBase(CInstance* pins, int i_index) const override;

	//*****************************************************************************************
	virtual void DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const override;
};

#endif
