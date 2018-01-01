/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CPlayer definition.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/Player.hpp                                         $
 * 
 * 78    98/09/28 2:50 Speter
 * Added CheckForDrop().
 * 
 * 77    9/26/98 8:59p Agrant
 * added bcontrol flag
 * 
 * 76    98/09/15 3:57 Speter
 * CPlayer is now a pure interface class, and this file is much smaller.
 * 
 * 75    9/06/98 5:57p Agrant
 * support for handling effects samples better (ouch, ammo)
 * 
 * 74    98/09/06 3:13 Speter
 * Added swing damage adjustment.
 * 
 * 73    98/09/04 22:03 Speter
 * Moved PlayerSettings to separate file. Initial auto-swing implementation.
 * 
 * 72    98/08/31 18:56 Speter
 * Gun aim lock added. Modified hand tracking a lot, some done in Move() now; removed
 * AdjustHead().
 * 
 * 71    8/31/98 1:06p Mmouni
 * Added new ammo pickup sound for 100%.
 * 
 * 70    98/08/30 21:10 Speter
 * Added quick throw. Removed flail support. Hand no longer interacts with foot when empty.
 * 
 * 69    8/27/98 12:23a Agrant
 * really lame hack to get the tatto to work with an instance attached to the player
 * 
 * 68    98/08/25 19:22 Speter
 *  Removed unused HandleColliision; added pins_me to HandleDamage. Handle all player damage
 * under HandleDamage.
 * 
 * 67    8/25/98 1:36p Shernd
 * Removed the local classes "bDead" call and flag.  It now uses the parent's bDead function
 * 
 * 66    98/08/22 21:41 Speter
 * Added bHandActive().
 * 
 * 65    8/18/98 8:28p Pkeet
 * Added the 'InitializeDataStatic' virtual override to fix bug that prevented the player's
 * body from being rendered in hardware.
 * 
 * 64    98.08.18 7:44p Mmouni
 * Added support for flashing the sceen when player is injured.
 * 
 *********************************************************************************************/

#ifndef HEADER_GAME_DESIGNDAEMON_PLAYER_HPP
#define HEADER_GAME_DESIGNDAEMON_PLAYER_HPP


//
// Includes.
//

#include "Lib/EntityDBase/Animate.hpp"

class CDraw;

//
// Class definitions.
//

//**********************************************************************************************
//
class CPlayer : public CAnimate
//
// Prefix: play
//
// Abstract interface class for the player object.
//
//**************************************
{
public:
	bool		bPhysics;		// Quick hack to toggle whether the player can have active physics.
	bool		bInvulnerable;	// Is the player invulnerable and immune to death?
	bool		bControl;		// Is the player controllable?

	CInstance*	pinsAttached;	// The EntityAttached that needs heart updates, or zero if none.

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	// Constructor function.
	static CPlayer* pplayNew();

	virtual ~CPlayer();

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual void Update
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) = 0;
	//
	// Changes the player to match the specified object found in the GROFF file.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void UpdatePlayerSettings
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	) = 0;
	//
	// Updates any player setting as specified in the given text props.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual CInstance* pinsHeldObject
	(
	) const = 0;
	//
	// Returns:
	//		The held object, if any.
	//
	//**********************************
	
	//*****************************************************************************************
	//
	virtual bool bHandInteract
	(
		const CInstance* pins			// Object to test.
	) const = 0;
	//
	// Returns:
	//		Whether the hand should physically interact with pins.
	//
	//**********************************

	//*****************************************************************************************
	//
	virtual void MaybeTalkAboutAmmo
	(
		int i_current_ammo_count,
		int i_maximum_ammo_count
	) = 0;
	//
	// Talks about the ammo situation, if applicable.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void TalkAboutApproximateAmmo
	(
		int i_sample
	) = 0;
	//
	// Say 0 = "almost emtpy", 1 = "half full", or 2 = "full".
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void TalkAboutAmmoOnPickup
	(
		int		i_current_ammo_count,
		int		i_maximum_ammo_count,
		bool	b_approximate
	) = 0;
	//
	// When pickup up a gun say how much ammo it has.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual bool bCanTalk
	(
	) = 0;
	//
	// True if we can talk, else false.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void StopTalking
	(
	) = 0;
	//
	// Shuts Anne up.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void JumpSound
	(
	) = 0;
	//
	// Make jump sound.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void Pickup
	(
		CInstance* pins_obj
	) = 0;
	//
	// Picks up pins_obj in player's hand, at current positions.
	//
	//**********************************

	//*****************************************************************************************
	//
	virtual void Drop
	(
		CVector3<> v3_throw = v3Zero		// Throw momentum vector, in world space.
	) = 0;
	//
	// Drops any held object.
	//
	//**********************************

	//*****************************************************************************************
	//
	virtual void CheckForDrop() = 0;
	//
	// Checks to see if arm has been stressed, and must drop.
	//
	// Called every physics frame.
	//
	//**********************************

	//*****************************************************************************************
	//
	virtual void DrawPhysics
	(
		CDraw& draw, 
		CCamera& cam
	) const = 0;
	//
	//**********************************
};


extern CPlayer* gpPlayer;

#endif
