/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *	CAction
 *		CSetAIAction
 *		CSetPhysicsAction
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/GameActions.hpp                                          $
 * 
 * 20    9/21/98 1:06a Agrant
 * added water disturbance action
 * 
 * 19    9/10/98 12:03a Agrant
 * added player comtrol action and set ai system action
 * 
 * 18    98.09.03 5:03p Mmouni
 * Added end game action.
 * 
 * 17    8/28/98 6:10p Agrant
 * Allow set magnet trigger to base drive off of another object's offset.
 * 
 * 16    8/19/98 5:41p Mlange
 * Added substitute AI action.
 * 
 * 15    8/18/98 10:02p Agrant
 * Added impulse style to the physics action.
 * 
 * 14    8/18/98 6:49p Mlange
 * Hide/show action can now optionally be volume based.
 * 
 * 13    8/11/98 7:41p Agrant
 * activate/deactivate activities in Set AI trigger
 * 
 * 12    98.08.04 9:38p Mmouni
 * Added "set hint" action.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRIGGER_GAMEACTION_HPP
#define HEADER_LIB_TRIGGER_GAMEACTION_HPP

#include "Action.hpp"
#include "Game/AI/Feeling.hpp"
#include "Game/AI/ActivityEnum.hpp"
class CWaterDisturbance;


//*********************************************************************************************
#define ACTION_IMGCACHE_SET_PIXEL_RATIO		0x00000001
#define ACTION_IMGCACHE_SET_MIN_PIXELS		0x00000002
#define ACTION_IMGCACHE_SET_CACHE_AGE		0x00000004
#define ACTION_IMGCACHE_SET_CACHE_ON		0x00000008
#define ACTION_IMGCACHE_SET_CACHE_OFF		0x00000010
#define ACTION_IMGCACHE_SET_INTERSECT_ON	0x00000020
#define ACTION_IMGCACHE_SET_INTERSECT_OFF	0x00000040


//*********************************************************************************************
class CSetAIAction : public CAction
// Prefix: saia
// set an AI's parameters
//**************************************
{
public:
	CSetAIAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start();
	
protected:
	CFeeling	feelNewEmotions;	// The new emotional state of the animal.  If an entry is negative, do not change it.
	CAnimal*	paniTarget;			// The animal to affect.
	CInstance*	pinsStayNearTarget;	// The thing to stay near.
	CInstance*	pinsStayAwayTarget; // The thing to stay away from.
	uint8		au1ActivityStates[eatEND];	// 0 for ignore, 1 for On, 2 for Off
};


//*********************************************************************************************
class CWakeAIAction : public CAction
// Prefix: swai
// set an AI's parameters
//**************************************
{
public:
	CWakeAIAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start();
	
protected:
	CAnimal*	paniTarget;			// The animal to affect.  Zero if none.
	bool		bWakeUp;			// true if waking, false if putting to sleep
	TReal		rRadius;			// Radius of area to affect dinos in
	CVector3<>	v3Center;			// Center of area to affect dinos in
};

//*********************************************************************************************
class CSetPhysicsAction : public CAction
// Prefix: saia
// set a physics object's parameters
//**************************************
{
public:
	CSetPhysicsAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start();
	
protected:
	// No data needs to be saved
	CVector3<>	v3Velocity;		// The object's velocity when we are done.
	bool		bFreeze;		// Whether to freeze or unfreeze the target.
	CInstance*	pinsTarget;		// The object to affect.
	
	bool		bImpulse;			// True if we are to apply an impulse.
	CVector3<>	v3ImpulseOrigin;	// The point of origin of the impulse.
	CVector3<>	v3ImpulseDirection;	// The direction the impulse goes.
	TReal		rImpulse;			// The amount of impulse.
	CInstance*	pinsImpulseSource;	// An optional instance to use as impulse source.
									// If present, overrides the Origin and Direction.
};


//*********************************************************************************************
class CSubstituteMeshAction : public CAction
// Prefix: saia
// set a physics object's parameters
//**************************************
{
public:
	CSubstituteMeshAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start();
	virtual void Stop();
	
protected:
	// No data needs to be saved
	bool bReset;				// Do we reset the mesh to its original state on exiting the trigger?
	int	iSubstitute;			// Which substitute do we use?
	CInstance* pinsTarget;		// The object to affect.
};


class CMagnet;

//*********************************************************************************************
//
class CMagnetAction : public CAction
//
// Prefix: ma
//
// Create and destroy magnets.
//
//**************************************
{
public:
	CMagnetAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start();

protected:
	// No data to save
	uint32 u4MasterHandle;			// Hash of the name of the master object to magnet.
	uint32 u4SlaveHandle;			// Hash of the name of the slave object to magnet, 0 if master is magneted to world.

	const CMagnet* pmagMagnet;		// Magnet to use.

	bool bEnable;					// Whether this action should create or destroy the magnet.
	bool bDelta;					// True when the fields should be ADDED to an existing magnet.
	bool bMerge;					// True when merging in non-zero values from pmagMagnet into existing magnet
	const CInstance* pinsQuery;		// An optional instance to query for a drive parameter multiplier.  Zero if none.
};


//*********************************************************************************************
class CAnimateTextureAction : public CAction
// Prefix: saia
// set a physics object's parameters
//**************************************
{
public:
	CAnimateTextureAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start();
	//virtual void Stop();
	
protected:
	int iFrame;			// Frame to select, -1 if not set.
	int iFreezeFrame;	// Freeze frame, -2 if not set.  -1 means clear freeze frame.
	int iTrackTwo;		// New track two start.  -1 if not set.
	TSec sInterval;		// New time between frames.  Zero if not set.
	CInstance* pinsTarget;		// The object to affect.
};



//*********************************************************************************************
//
class CHideShowAction : public CAction
//
// Prefix: hsa
//
// hide or show an object
//
//**************************************
{
public:
	CHideShowAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start();
	virtual const char* pcLoad(const char* pc);
	virtual char* pcSave(char* pc) const;
	
protected:
	// bVisible needs to be saved
	uint32	u4Handle;								// handle of the target object to hide or show
	bool	bVolume;
	bool	bVisible;
	bool	bToggle;
};


//*********************************************************************************************
class CSetHintAction : public CAction
// Prefix: sha
// Set the current hint.
//**************************************
{
public:
	CSetHintAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start();
	virtual const char* pcLoad(const char* pc);
	virtual char* pcSave(char* pc) const;
	
	static int iCurrentHint;						// Global hint ID (need to save).

protected:
	int	iHintID;									// ID to set the current hint to.
};


//*********************************************************************************************
//
class CSubstituteAIAction : public CAction
//
// Prefix: saa
//
// Substitute AI info of an instance.
//
//**************************************
{
public:
	CSubstituteAIAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual ~CSubstituteAIAction();

	virtual void Start();
	virtual const char* pcLoad(const char* pc);
	virtual char* pcSave(char* pc) const;

protected:
	uint32 u4HashTarget;		// Hash of names of instances to modify.
	uint32 u4HashSubstitute;

	CAIInfo* paiiTargetOld;		// Original AI infos of instances (before substitution took place).
	CAIInfo* paiiSubstituteOld;

	bool bSubstituted;			// True if AI info of 'target' has been substituted for 'substitute'.
};


//*********************************************************************************************
//
class CEndGameAction : public CAction
//
// Prefix: end
//
// End game action.
//
//**************************************
{
public:
	CEndGameAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	//*****************************************************************************************
	// The start function of this action tells the world the game is over.
	virtual void Start();
};

//*********************************************************************************************
//
class CControlPlayerAction : public CAction
//
// Prefix: caa
//
// End game action.
//
//**************************************
{
public:
	CControlPlayerAction 
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	//*****************************************************************************************
	// The start function of this action tells the world the game is over.
	virtual void Start();

protected:
	bool bDropHeldItem;		// True if player should drop the thing she's holding
};

//*********************************************************************************************
//
class CAISystemAction : public CAction
//
// Prefix: asa
//
// Control the entire AI system action.
//
//**************************************
{
public:
	CAISystemAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	//*****************************************************************************************
	// The start function of this action tells the world the game is over.
	virtual void Start();

protected:
	bool bWakeUp;			// True if AI system should wake up, false if AI system should go to sleep.
};



//*********************************************************************************************
//
class CWaterDisturbanceAction : public CAction
//
// Prefix: wda
//
// Force a water disturbance to fire right now
//
//**************************************
{
public:
	CWaterDisturbanceAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual ~CWaterDisturbanceAction ();

	virtual void Start();

protected:
	uint32 u4Target;		// Hash of names of instances to modify.
	CWaterDisturbance* pwdTarget;
};



#endif // HEADER_LIB_TRIGGER_GAMEACTION_HPP
