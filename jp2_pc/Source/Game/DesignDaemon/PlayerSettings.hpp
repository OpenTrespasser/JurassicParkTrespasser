/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		SPlayerSettings
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/PlayerSettings.hpp                                  $
 * 
 * 9     98/10/04 23:24 Speter
 * Added fHandReachGrab; removed unused param.
 * 
 * 8     98/10/03 4:42 Speter
 * New player settings.
 * 
 * 7     98/10/01 4:05 Speter
 * Added swing size and mass limits. Made wrist drop use more liberal limits; only check it once
 * a frame, as it's kinda slow; length drop still checked every physics step.
 * 
 * 6     98/09/25 1:31 Speter
 * Added things.
 * 
 * 5     98/09/22 23:53 Speter
 * More stow intermediates.
 * 
 * 4     98/09/14 13:43 Speter
 * Moved some constants into SPlayerSettings.
 * 
 * 3     98/09/10 1:10 Speter
 * Better wrist rotation stuff.
 * 
 * 2     98/09/06 3:16 Speter
 * Updated/renamed hand orientation functions for new player command space.
 * 
 * 1     98/09/04 22:06 Speter
 * Moved from InfoPlayer.
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_DESIGNDAEMON_PLAYERSETTINGS_HPP
#define HEADER_GAME_DESIGNDAEMON_PLAYERSETTINGS_HPP

#include "Lib/Transform/Transform.hpp"
#include "Lib/Sys/Timer.hpp"

//******************************************************************************************
typedef CVector2<> CAngles2;
// Prefix: ang2

//******************************************************************************************
typedef CVector3<> CAngles3;
// Prefix: ang3

//**********************************************************************************************
//
struct SPlayerSettings
//
// Prefix: playset
//
//**************************************
{
	// Hand settings.
	CVector3<>	v3WristToPalmNormal;	// Translation from physics wrist joint to actual palm centre.
	CVector3<>	v3WristToPalmSpecial;	// Slightly tweaked version used for pickup magnet positioning,
										// to maintain compatibility with data.

	float		fHandDistMin;			// Default distance hand is extended.
	float		fHandDistMax;			// Max distance hand is extended.
	float		fHandReachSpecial;		// Distance hand can reach to pick up objects with pickup magnets.
	float		fHandReachGrab;			// Distance hand can reach when already touching objects with pickup magnets.
	float		fHandReachWidth;		// Raycast width for reaching.
	float		fHandDistPickup;		// Distance at which special pickup activates (per second).
	TSec		sPickupTime;			// Maximum time we must wait for pickup.
	TSec		sSwingTime;				// Time-out for swing cycle.
	TReal		rHandRotPickup;			// Angular divergence (of cosine) at which pickup activates (per second).
	TSec		sRaiseTime;				// Max time to wait for hand to be raised.
	TSec		sPickupNoDrop;			// Time after pickup to ignore drop.
	TSec		sThrowTime;				// Time for automatic throw "cock".

	float		fMaxMassPickup;			// Heaviest object she can pick up.
	float		fMaxMassSwing;			// Heaviest object she can swing.
	float		fSwingDamageMul;		// Ratio of damage increase during swinging.

	bool		bHandFollowHead;		// Alternate hand movement scheme.
	bool		bMoveHandActual;		// Hand target position based on delta from actual position.
										// Else based on delta from current target position.
	bool		bActivateCentreEmpty;	// Centre hand on activation when empty.
	bool		bActivateCentreHolding;	// Centre hand on activation when full.
	bool		bAllowDrop;				// Pay attention to physics drop commands.
	bool		bAutoCrouchNormal,		// Crouch automatically to pick up objects (normal or special).
				bAutoCrouchSpecial;
	TSec		sCrouchDelay;			// Time we expect crouching to take.
	TReal		fCrouchDist;			// Distance we crouch (should match physics value).
	TReal		fAutoCrouchDist;		// Distance object should be below shoulder to enable auto-crouch.
	bool		bHoldRelativeShoulder;	// Holding orientation is relative to shoulder; else head.

	CAngles2	ang2HandMax;			// Max angles that hand can move from body.
	CAngles2	ang2HandTurnMax;		// Max angles that hand can move from body before turning.
	CAngles2	ang2HandViewMin;		// Max angles that hand can roam from head before head turns.
	CAngles2	ang2HandViewMax;
	CAngles3	ang3HandRotMin;			// Min angles that hand can rotate.
	CAngles3	ang3HandRotMax;			// Max angles that hand can rotate.
	CAngles3	ang3HandDropMin;		// Min angles before hand drops.
	CAngles3	ang3HandDropMax;		// Max angles before hand drops.
	float		angHandRotPickup;		// Max angle hand can rotate to pickup.

	// Throw stuff.
	float		angThrowLob;			// Angle to adjust throw upward when lobbing.
	float		fThrowMomentumScale;	// Momentum magnitude (kg m/s) per input radian.
										// Also serves as maximum.
	float		fThrowVelocityMax;		// Maximum magnitude of throw velocity.
	float		fThrowTorque;			// Scale the torque applied when throwing off-centre.
	bool		bThrowReverse;			// Throw in opposite direction (horizontal) of aim.

	// Special hand placements.
	CPlacement3<>	p3Throw;			// Placement for throw cock.
	CPlacement3<>	aap3Stow[2][3];		// Player-relative placements for stowed object 
										// (2 slots, each with 3 intermediate points).
	CVector3<>	av3StowMin[2],			// Limits of stowable object min extents (relative to hand).
				av3StowMax[2];			// Same for max extents.
	CPlacement3<>	p3SafeHand;			// Safe place to put hand inside body.
	float		angSwingPull;			// Angle to pull back from screen centre for swing.

	// Head settings.
	CVector3<>	v3HeadOffset;			// Position of head relative to body (default forward position).
	CVector3<>	v3NeckOffset;			// Point about which head pivots when turning.
	bool		bHeadFollowActual;		// Head tracks actual hand position; else target position.
	float		fHeadMoveFrac;			// Fraction of adjustment head makes per frame to keep hand in view.
										// 1 makes tracking instant. Yes, this is frame-rate dependent.
	CAngles2	ang2HeadViewMax;		// Limits on head motion.
	float		fAngHeadTurnX;			// Angle at which head turn starts body turn.

	//******************************************************************************************
	SPlayerSettings();
};

extern SPlayerSettings	PlayerSettings;


#endif
