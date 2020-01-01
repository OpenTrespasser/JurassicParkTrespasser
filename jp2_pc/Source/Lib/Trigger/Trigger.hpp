/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	CTrigger
 *		CLocationTrigger
 *		CObjectTrigger
 *		CCreatureTrigger
 *		CMagnetTrigger
 *
 * Bugs:
 *
 * To do:
 * Look at optimizing conditions with a "player only" location trigger that doesn't
 * go through an entity list but only checks for the player; this can be implemented within
 * the current interface since it's not interface (just implementation) specific.
 *
 * The button triggers (part of the object group) could have additional flags so that the
 * trigger only fires in the button/lever is in a certain state. Ie, just fire if you have
 * switch the button on,
 *
 * Magnet triggers
 *
 * Fill the relevent trigger messages with data, this data has not yet been decided...
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/Trigger.hpp                                              $
 * 
 * 55    98.09.18 12:29p Mmouni
 * The start trigger now listens to the CMessageSystem GROFF_LOADED message.
 * 
 * 54    9/18/98 10:47a Mlange
 * Added cast function for location trigger.
 * 
 * 53    8/30/98 12:22p Agrant
 * added more mass trigger
 * 
 * 52    8/23/98 3:26p Rwyatt
 * Added evaluate now for boolean triggers
 * 
 * 51    8/21/98 5:12p Mlange
 * Location triggers now participate in an optimised partition tree.
 * 
 * 50    8/18/98 8:12p Mmouni
 * Changed CLocationTrigger so that if an object is not specified only tangible moveable
 * objects will trigger the trigger.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRIGGER_TRIGGER_HPP
#define HEADER_LIB_TRIGGER_TRIGGER_HPP


#include "common.hpp"

#include <list>

#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/std/random.hpp"
#include "Lib/sys/timer.hpp"
#include "Lib/Groff/GroffIO.hpp"

#include "Action.hpp"
#include "ExpressionEvaluate.hpp"


//*********************************************************************************************
// use this for always
#define TRIGGER_PROBABILITY_ALWAYS	0xffffffff


//*********************************************************************************************
// maximum number of actions that any trigger can hold
#define u4TRIGGER_MAX_ACTIONS			16

// maximum number of instances that can be in a trigger
#define u4TRIGGER_MAX_CONTAINMENT		16

// number of individual creatures that can be attached to a creature trigger
#define	u4TRIGGER_MAX_CREATURES			4

// number of individual objects that can be attached to an object trigger
#define	u4TRIGGER_MAX_OBJECTS			4


//*********************************************************************************************
// Type of bounding volumes that a trigger can have
//
#define BOUND_SPHERE					0		//default
#define BOUND_CUBE						1



//*********************************************************************************************
// This enum controls how the action list within a trigger is processed when the trigger fires.
//
enum EActionProcess
// prefix: eap
//**************************************
{
	eapALL=0,				// do all the actions in the list
	eapSTEPORDER,			// do one action each fire, do the list in sequence
	eapSTEPRANDOM,			// do one action each fire, do the list random
	eapSEQUENCEORDER,		// Sequence the actions in the list one after the other
	eapSEQUENCERANDOM,		// Sequence the actions in the list in a random order
	eapSEQUENCEORDERLOOP,	// Sequence the actions in the list one after the other and loop at the end
	eapSEQUENCERANDOMLOOP,	// Sequence the actions in the list in a random order continuosly

	eapEND
};


//*********************************************************************************************
enum EInstanceType
//prefix: eit
{
	eINSTANCE_PLAYER,		// The player object.
	eINSTANCE_CREATURE,		// A creature object.
	eINSTANCE_OBJECT,		// Any tangible movable object.
	eINSTANCE_IGNORE		// Intangible or immovable object.
};


//*********************************************************************************************
// Global helper function....
bool bGroffObjectLoaded
(
	CLoadWorld*		pload,
	const char*		str
);

//*********************************************************************************************
bool bInGroff
(
	CLoadWorld*					pload,
	const char*					str
);


//*********************************************************************************************
//
class CTrigger : public CEntity
// Prefix: tr
// The abstraction for all triggers: one-shot and a list of actions to fire (Trigger()) when
// conditions are met.  There is no pure virtual function denoting the evaluate-condition than
// trigger - that's pushed down to inherited classes.  Trigger() will always fire all actions.
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
protected:
	CTrigger();  // Only for pinsCopy.

public:
	CTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	//******************************************************************************************
	virtual ~CTrigger();


	//******************************************************************************************
	// Add an action to this trigger
	void Add(rptr<CAction> paction)			
	{ 
		Assert(u4ActionCount < u4TRIGGER_MAX_ACTIONS);

		acList[u4ActionCount] = paction;
		u4ActionCount++;
	}


	//******************************************************************************************
	// Set the life of a trigger (use this instead of the above)
	void SetLife(uint32 u4_life)
	{
		Assert(u4_life>=1);

		u4_TriggerLife=u4_life;
	}


	//******************************************************************************************
	// Set the probability of the trigger firing...
	void SetProbability(uint32 u4_chance)
	{
		u4_Probability=u4_chance;
	}


	//******************************************************************************************
	// set the fire delay period
	void SetTriggerFireDelay(TSec s_time)
	{
		Assert(s_time>=0.0F);
		s_FireDelay=s_time;
	}


	//******************************************************************************************
	// Caclulate a random delay between sequenced actions.
	TSec sSequenceDelay
	(
	);


	//******************************************************************************************
	// process the contained actions in the specified style
	void ProcessActionList
	(
	);


	//******************************************************************************************
	// this will atempt to fire the trigger by checking any probablity or conditions that
	// govern the use of this trigger. The member also takes care of any time delayed
	// triggers.
	void AttemptTriggerFire();


	//******************************************************************************************
	// Process the list and decrement the life counter, send a fire message
	void Trigger();


	//******************************************************************************************
	//
	// Overides.
	//	
	virtual const CBoundVol* pbvBoundingVol() const
	{ 
		return bv_BoundingVolume; 
	}


	//*****************************************************************************************
	// This is the base Trigger class which handles step messages, if a certain type of
	// trigger requires step messages it may overload this function, but this must be 
	// called when the ewn function has finished so that delayed messages still get fired
	//
	virtual void Process(const CMessageStep& msgstep);


	//*****************************************************************************************
	// the base class can process the move message because generally this just updates
	// the entities postion, for some triggers the state of the trigger may have to
	// changed so it will need to overload this function.
	// The defaul implemntation of this virtual function just calls the Move member
	// of the instance base class.
	//
	virtual void Process(const CMessageMoveTriggerTo& msgmtrigto);


	//*****************************************************************************************
	// the base class handles all boolean expressions through this message.
	//
	virtual void Process(const CMessageTrigger& trigmsg);


	//*****************************************************************************************
	virtual bool bIncludeInBuildPart() const
	{
		return false;
	}


	//******************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual void Move(const CPlacement3<>& p3_new);

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	//*****************************************************************************************
	// override the default implementation in CPartition which assigns NULL.
	//
	virtual void Cast(CTrigger** pptr)
	{
		*pptr=this;
	}


	//*****************************************************************************************
	// function will return true or false with regards to all the object references contained
	// within the trigger being present.
	// In debug mode this function asserts on every possible parameter error.
	//
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);


	//*****************************************************************************************
	// This function is called by the expression evaluator to determine if the trigger is
	// currently firing. Triggers then can implement such a feature should overload this function
	// and return true if the trigger is currently active.
	virtual bool bEvaluateNow();

	static CRandom			rndRand;

protected:
	// Expression evaluator to determine fire logic or NULL for always fire
	CExpressionEvaluate*	peeFireExpression;				// Has its own save function

	// List of actions for this trigger to take
	rptr<CAction>			acList[u4TRIGGER_MAX_ACTIONS];

	// For sequenced actions these control the delay between sequenced actions.
	float					fMinSequenceDelay;				// Not saved
	float					fMaxSequenceDelay;				// Not saved

	// The number of actions in this trigger
	uint32					u4ActionCount;					// Not saved

	// The next action to be processed in the action sequence. Only used while the action
	// list process style is eapSEQUENCE;
	uint32					u4NextAction;					// Saved

	// The type of action list process
	EActionProcess			eapList;						// Not saved

	// The number of times the trigger can be fired before it dies.
	// set to 0xffffffff to have an indefinate life, (this is handled
	// as a special case and is not just a big counter)
	uint32					u4_TriggerLife;					// Save

	// The probability that the trigger will fire, bigger is more probable
	// this could be a float but then we will have trouble with explicit
	// compares. 0xffffffff means always without regard for the probable
	// case..
	// when the trigger is fired this is set to zero. This is a special case
	// which is checked in all trigger entry points
	uint32					u4_Probability;					// Save

	// The delay is fractional seconds to when the trigger fires, this time
	// is relative to the start of this frame.
	TSec					s_FireDelay;					// Not saved

	// When this time passes, fire the trigger.
	// This time is negative if it has not been set
	TSec					s_FireTime;						// Save

	// If true only fire when the trigger count is zero
	bool					bFireZero;						// Not Saved

	// If this is set the the trigger life counter is set to this when it fires
	// This is only set if the bFireZero flag is set to true.
	uint32					u4ResetLife;					// Not saved

	// Set to true if this trigger is sequencing actions.
	bool					bSequenceActions;				// Save

	// Time at which the next action should start
	TSec					sNextActionTime;				// Save

	// Next time the trigger fires, should the sequenxe be reset??
	bool					bResetSequence;					// Not saved


	// Control the fire rate of a trigger...
	TSec					sNextFireTime;					// Save
	TSec					sFireDelayPeriod;				// Not saved

	// this points to the required bounding volume, either the static below or the a bounding
	// volume that is loaded.
	const CBoundVol*		bv_BoundingVolume;

	static CBoundVolSphere	bvsTriggerUnitSphere;
};



//*********************************************************************************************
//
typedef uint32 TCondition;
// Prefix: tco
//
// These are all of the possible trigger conditions that can be applied to a location trigger



//*********************************************************************************************
// these conditions need a mask so individual trgger types cannot get the wrong flags
// location trigger types

// object location triggers only have the following two states, we cannot do group objects
// because the trigger only as mechanisms for all objects or specific objects and having
// a trigger where it actiavtes when any 3 objects are within it is pointless. It could be
// useful if we could specify which 3 objects

// IF THE ACTIVATION INSTANCE IS SET FOR THE TRIGGER PLEASE ENSURE THAT THE FLAGS ARE SET
// CORRECTLY. FOR EXAMPLE IF YOU HAVE A TRIGGER WHICH HAS ITS FLAGS SET TO TRIGGER_COND_
// PLAYER_MOVE_INSIDE | TRIGGER_COND_CREATURE_MOVE_INSIDE IT WILL TRIGGER IF EITHER ANNE
// OR ANY CREATURE ENTERS THE AREA. IF YOU SET THE TRIGGER ACTIVATION INSTANCE TO BE COME
// CREATURE THEN ONLY THAT CREATURE WILL TRIGGER IT, ANNE OR ANY OTHER CREATURES WILL NOT.
// THIS IS ESPECIALLY IMPORTANT IF THE TRIGGER IS JUST SET TO A PLAYER.

#define TRIGGER_COND_LOCATION_GROUP			0x00000FFF
#define TRIGGER_COND_NEVER					0x00000000
#define TRIGGER_COND_PLAYER_MOVE_INSIDE		0x00000001	// Player has moved in to the bv
#define TRIGGER_COND_PLAYER_MOVE_OUTSIDE	0x00000002	// Player has moved out of the bv
#define TRIGGER_COND_CREATURE_MOVE_INSIDE	0x00000004	// Creature has moved in to the bv
#define TRIGGER_COND_CREATURE_MOVE_OUTSIDE	0x00000008	// Creature has moved out of the bv
#define TRIGGER_COND_PLAYER_ALWAYS			0x00000010	// fire trigger if player is in bv
#define TRIGGER_COND_CREATURE_ALWAYS		0x00000020	// fire trigger if creature is in bv
#define TRIGGER_COND_CREATURE_ENTER_COUNT	0x00000040	// fire trigger if n creatures enter
#define TRIGGER_COND_CREATURE_LEAVE_COUNT	0x00000040	// fire trigger if n creatures leave
#define TRIGGER_COND_OBJECT_MOVE_INSIDE		0x00000080	// an object has moved in to the bv
#define TRIGGER_COND_OBJECT_MOVE_OUTSIDE	0x00000100  // an object has moved out of the bv
#define TRIGGER_COND_OBJECT_ALWAYS			0x00000200	// an obect is in the bv


// creature trigger types
#define TRIGGER_COND_CREATURE_GROUP			0x0001F000
#define TRIGGER_COND_CREATURE_DIED			0x00001000	// a creature/player has died
#define TRIGGER_COND_CREATURE_WOKEN			0x00002000	// a creature/player has been woken up
#define TRIGGER_COND_CREATURE_DAMAGE		0x00004000	// sustained non-critical damage
#define TRIGGER_COND_CREATURE_CRITDAMAGE	0x00008000	// sustained critical damage
#define TRIGGER_COND_CREATURE_SLEEP			0x00010000	// a creature has been woken up

// object trigger types
#define TRIGGER_COND_OBJECT_GROUP			0x00700000
#define	TRIGGER_COND_OBJECT_PICKUP			0x00100000	// object has been picked up
#define TRIGGER_COND_OBJECT_PUTDOWN			0x00200000	// object has been put down
#define TRIGGER_COND_OBJECT_USED			0x00400000	// object has been used


// Gets a description of the condition in human readable form.
int iGetConditionDescription(TCondition tco, char *pc_buffer, int i_buffer_len);





//*********************************************************************************************
//
class CLocationTrigger : public CTrigger
// Prefix: lo
// A location trigger is a trigger that fires when players/non-players (creatures) cross
// in-to-out or out-to-in the location boundry (spherical).
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CLocationTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,		// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

private:
	CLocationTrigger();  // Only used for pinsCopy()

public:
	~CLocationTrigger();



	void SetCondition(TCondition tco_conditions)
	{ 
		// only set a condtion from the location group, see above
		Assert( (tco_conditions & ~TRIGGER_COND_LOCATION_GROUP) == 0);
		tco_TriggerConditions = tco_conditions;
	}


	//******************************************************************************************
	// set the object that this trigger will respond to, if this is set it will ONLY respond
	// to the specifed object/creature. Set to NULL for any object/creature
	//
	void SetActivation(CInstance* pins_act)
	{
		pinsActivate=pins_act;
	}

	//******************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual bool bIncludeInBuildPart() const
	{
		return true;
	}

	//*****************************************************************************************
	virtual void Process(const CMessageStep& msgstep);

	//*****************************************************************************************
	virtual bool bEvaluateNow();

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	//*****************************************************************************************
	virtual CInstance* pinsCopy() const;
	
	const char* strPartType() const
	{ 
		return "CLocationTrigger"; 
	}

	//*****************************************************************************************
	// override the default implementation in CPartition which assigns NULL.
	//
	virtual void Cast(CLocationTrigger** pptr)
	{
		*pptr=this;
	}


	// set the number of creatures that have to enter the trigger area
	void SetCreatureEnterTrigger(uint32 u4_count)
	{
		u4_CreatureEnterTriggerCount=u4_count;
	}

	// set the number of creatures that have to leave the trigger area
	void SetCreatureLeaveTrigger(uint32 u4_count)
	{
		u4_CreatureLeaveTriggerCount=u4_count;
	}

	// Messages to process.
	//void Process(const CMessageHere& msg);
	void Process(const CMessageMove& msg);
	virtual void Process(const CMessageMoveTriggerTo& msgmtrigto);

	//*****************************************************************************************
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);

private:

	//******************************************************************************************
	// Private functions that manage the containment of the location trigger.
	//
	bool bContained(const CInstance* pins);

	//******************************************************************************************
	void SetContained(const CInstance* pins);


	//******************************************************************************************
	void RemoveContained(const CInstance* pins);

	//******************************************************************************************
	// Evaluate trigger wrt the specified object
	bool Evaluate(const CInstance* pins);

	//******************************************************************************************
	// Returns an enum based on the type of the instance passed in.
	EInstanceType eitInstanceType(const CInstance* pins);

	//******************************************************************************************
	// List of entities currently inside the trigger. We do not dereference the
	// entities directly so if the enitity was killed than it would be okay -
	// except for the memory it consumes.  If this is a problem than we can do a
	// timer mechanism where we purge old entities but this shouldn't be necessary
	// since location triggers are usually temporary anyway.
	const CInstance*	apinsContained[u4TRIGGER_MAX_CONTAINMENT];							// save

	// Conditions in which we trigger.
	TCondition	tco_TriggerConditions;		

	// these are only used by the creature limit condition
	uint32		u4_CreatureEnterCount;			// number of CREATURES that have entered	// save
	uint32		u4_CreatureEnterTriggerCount;	// number of enteries before we trigger		// not saved
	uint32		u4_CreatureLeaveCount;			// number of CREATURES that have left		// save
	uint32		u4_CreatureLeaveTriggerCount;	// number to leave before we trigger		// not saved

	// the object or creature that can activate this trigger, if this is set to NULL
	// then any object or trigger can activate it. This is the defalt case.
	CInstance*	pinsActivate;																// not saved

	// Set to true to fire on the center point of an object entering this trigger, if set
	// to false then the trigger will fire as soon as the moving obejcts bounding volume
	// intersects.
	bool		bPointTrigger;																// not saved
};





//*********************************************************************************************
//
class CCreatureTrigger : public CTrigger
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CCreatureTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);


	~CCreatureTrigger();


	//******************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

	//*****************************************************************************************
	virtual bool bEvaluateNow();

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	const char* strPartType() const
	{ 
		return "CCreatureTrigger"; 
	}

	//******************************************************************************************
	// List control functions
	void AddCreature(uint32 u4_handle);
	bool bInList(uint32 u4_handle);

	//******************************************************************************************
	// Messages to process.
	void Process(const CMessageDeath& msg);
	void Process(const CMessageDamage& msg);
	void Process(const CMessageMove& msg);

	//*****************************************************************************************
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);
protected:
	TCondition			tcoTriggerConditions;					// Conditions in which we trigger.
	uint32				au4Creatures[u4TRIGGER_MAX_CREATURES];
	float				fDamagePoints;
	bool				bEvaluateAll;		// should evaluate look for all creatures being dead
};





//*********************************************************************************************
//
//
class CObjectTrigger : public CTrigger
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CObjectTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CObjectTrigger();


	//******************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	const char* strPartType() const
	{ 
		return "CObjectTrigger"; 
	}

	//******************************************************************************************
	// these are the messages that this trigger will respond to..
	void Process(const CMessagePickUp& msgpu);
	void Process(const CMessageUse& msguse);

	//******************************************************************************************
	// List control functions
	void AddObject(uint32 u4_hash);
	bool bInList(uint32 u4_hash);

	//*****************************************************************************************
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);

protected:
	uint32				au4Objects[u4TRIGGER_MAX_OBJECTS];
	TCondition			tcoTriggerConditions;
};




//*********************************************************************************************
//
//
class CMagnetTrigger : public CTrigger
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CMagnetTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CMagnetTrigger();


	//******************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	const char* strPartType() const
	{ 
		return "CMagnetTrigger"; 
	}

	//******************************************************************************************
	// these are the messages that this trigger will respond to..
	void Process(const CMessageMagnetMove& msgmagm);
	void Process(const CMessageMagnetBreak& msgmagb);

	//*****************************************************************************************
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);

protected:

};


//*********************************************************************************************
//
//
class CStartTrigger : public CTrigger
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CStartTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CStartTrigger();

	//******************************************************************************************
	static void ResetStartTriggers
	(
	);
	//
	// Reset all the start triggers so that they will fire again.
	//	
	//**************************************

	//******************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual char* pcSave(char* pc_buffer) const;

	//*****************************************************************************************
	virtual const char* pcLoad(const char*  pc_buffer);

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char* buffer, int i_buffer_length);
#endif

	const char* strPartType() const
	{ 
		return "CStartTrigger";
	}

	//******************************************************************************************
	// these are the messages that this trigger will respond to..
	void Process(const CMessageStep& msgstep);

	void Process(const CMessageSystem& msgsys);

	//*****************************************************************************************
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);
protected:
	bool bFired;		// Have we fired?

	static std::list<CStartTrigger*> listStartTriggers;
						// A list of all the start triggers.
};



//*********************************************************************************************
//
//
class CCollisionTrigger : public CTrigger
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CCollisionTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CCollisionTrigger();


	//******************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	const char* strPartType() const
	{ 
		return "CCollisionTrigger"; 
	}

	//******************************************************************************************
	// these are the messages that this trigger will respond to..
	void Process(const CMessageCollision& msg);

	//*****************************************************************************************
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);
protected:
	union
	{
		TSoundMaterial	smat1;
		uint32			u4Handle1;		// Note instance handle not pointer, so it can be generated without the instance being present
	};

	union
	{
		TSoundMaterial	smat2;
		uint32			u4Handle2;
	};

	bool	bSoundMaterial1;		// set to true if the element 1 is a sound material rather than an instance
	bool	bSoundMaterial2;		// set to true if the element 2 is a sound material rather than an instance
	float	fMinVelocity;
	float	fMaxVelocity;			// Minimum and maximum velocities (normalized 0.0 to 1.0)
};




//*********************************************************************************************
//
//
class CSequenceTrigger : public CTrigger
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CSequenceTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CSequenceTrigger();


	//******************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	const char* strPartType() const
	{ 
		return "CSequenceTrigger"; 
	}

	//******************************************************************************************
	// these are the messages that this trigger will respond to..
	void Process(const CMessageTrigger& trigmsg);

	//*****************************************************************************************
	// function will return true or false with regards to all the object references contained
	// within the trigger being present.
	// In debug mode this function asserts on every possible parameter error.
	//
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);


private:
	CPArray<const CTrigger*> paptrListen;
	CPArray<const CTrigger*> paptrOrder;
	CPArray<const CTrigger*> paptrEvalNow;

	CTrigger* ptrSequenceFalse;

	CMArray<const CTrigger*> maptrCurrentOrder;
};



//*********************************************************************************************
// Boolean triggers are basically an instance of the CTrigger base class excpet that the
// boolean trigger will attempt to fire on every step message
//
class CBooleanTrigger : public CTrigger
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CBooleanTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CBooleanTrigger();

	//******************************************************************************************
	// these are the messages that this trigger will respond to..
	void Process(const CMessageStep& msg);

	//*****************************************************************************************
	virtual bool bEvaluateNow();
	//
	// Simply returns the current evaluate state of the trigger.
	//
	//*********************************

	//******************************************************************************************
	const char* strPartType() const
	{ 
		return "CBooleanTrigger"; 
	}

	//*****************************************************************************************
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);
};




//*********************************************************************************************
// A timer trigger has a high and low period. These periods are only accurate to the next
// frame so in effect are minimum periods.
// The trigger fires once at the start of the high period but its evaluate now function will
// return true throughout the high period.
//
class CTimerTrigger : public CTrigger
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CTimerTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CTimerTrigger();

	//******************************************************************************************
	void Process(const CMessageStep& msg);

	//******************************************************************************************
	const char* strPartType() const
	{ 
		return "CTimerTrigger"; 
	}

	//*****************************************************************************************
	virtual char* pcSave(char*  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	//*****************************************************************************************
	virtual bool bEvaluateNow()
	{
		return bState;
	}

	//*****************************************************************************************
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);

protected:
	float	fMinHighTime;		// High period
	float	fMaxHighTime;		// High period
	float	fMinLowTime;		// Low period
	float	fMaxLowTime;		// Low period
	bool	bState;				// Current state
	float	fStateRemain;		// Time of the current state remaining
};



//*********************************************************************************************
//
// This trigger hold a true/false value for use in trigger expressions.
// It never fires.
//
class CVariableTrigger : public CTrigger
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CVariableTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CVariableTrigger();

	//******************************************************************************************
	const char* strPartType() const
	{ 
		return "CVariableTrigger"; 
	}

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	//*****************************************************************************************
	virtual bool bEvaluateNow
	(
	);
	//
	// Simply returns the current value of the trigger.
	//
	//*********************************

	//*********************************************************************************************
	void Set
	(
		bool b_value
	);
	//
	// Sets the value of the trigger to b_value.
	//
	//*********************************
	
	//*********************************************************************************************
	void Invert
	(
	);
	//
	// Inverts the value of the trigger.
	//
	//*********************************

	//*****************************************************************************************
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);

protected:
	bool bValue;							// Value of the trigger.
};


//*********************************************************************************************
//
class CMoreMassTrigger : public CTrigger
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//
	CMoreMassTrigger
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				p_load,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);


	~CMoreMassTrigger();

	
	//*****************************************************************************************
	float fGetMass();		// Gets the current intersecting this

	//******************************************************************************************
	//
	// Overides.
	//

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

	//*****************************************************************************************
	virtual bool bEvaluateNow();

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	const char* strPartType() const
	{ 
		return "CMoreMassTrigger"; 
	}

	//*****************************************************************************************
	static bool bValidateTriggerProperties
	(
		const CGroffObjectName*	pgon,		// Object to load.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				p_load		// The loader.
	);
protected:
	float				fMass;
	TSec				sValidMass;
	
	float				fMassCompare;
	CMoreMassTrigger*	pmmtCompare;
	uint32				u4Compare;

#define iNUM_IGNORE 16
	CMSArray<uint32,iNUM_IGNORE>	au4Ignore;
};


#endif // HEADER_LIB_TRIGGER_TRIGGER_HPP
