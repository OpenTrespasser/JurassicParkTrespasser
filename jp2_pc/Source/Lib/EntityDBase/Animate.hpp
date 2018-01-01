/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CAnimate
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Animate.hpp                                          $
 * 
 * 52    98/10/04 2:34 Speter
 * Added UpdatePhysicsHealth().
 * 
 * 51    98/09/23 15:05 Speter
 * Added f_tranq to AddWound.
 * 
 * 50    9/16/98 12:33a Agrant
 * reworked collision damage system
 * 
 * 49    98/09/12 0:56 Speter
 * Added enum alias for forearm.
 * 
 * 48    9/08/98 2:20p Sblack
 * 
 * 47    98/09/06 3:17 Speter
 * Added swing damage adjustment.
 * 
 * 46    98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 45    98/08/28 19:52 Speter
 * Added iGoreLevel. 
 * 
 * 44    98/08/25 19:18 Speter
 * New collision damage handling; now uses total energy, integrated and buffered. Changed
 * papWounds to paWounds. Removed unused HandleColliision; added pins_me to HandleDamage.
 * 
 * 43    8/23/98 3:17a Agrant
 * New time-based animal damage
 * 
 * 42    8/17/98 7:38p Pkeet
 * Changed 'papWounds' from a CMArray to a CPArray so it would compile under 4.2.
 * 
 * 41    98/08/17 17:21 Speter
 * Added wound support.
 * 
 * 40    8/14/98 11:35a Agrant
 * record who killed you
 * 
 * 39    8/12/98 1:57p Mlange
 * Added CAnimate destructor.
 * 
 * 38    8/11/98 8:25p Mlange
 * 
 * 37    98/08/02 16:43 Speter
 * Added Move() handler to CAnimate, to move subsidiary boxes.
 * 
 * 36    7/29/98 3:05p Agrant
 * tranq and team tweaks
 * 
 * 35    7/20/98 10:13p Rwyatt
 * Removed description text in final mode
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_ANIMATE_HPP
#define HEADER_LIB_ENTITYDBASE_ANIMATE_HPP

//
// Includes.
//
#include "Entity.hpp"
#include "Lib/Renderer/GeomTypes.hpp"

typedef uint32 TSoundMaterial;

//
// Class definitions.
//

class CBoundaryBox;
class CWound;
class CGun;

enum EBoundaryBoxes
{
	ebbBODY,
	ebbFOOT,
	ebbRIGHT_FOOT = ebbFOOT,
	ebbHAND,
	ebbHEAD,
	ebbTAIL,
	ebbLEFT_FOOT,
	ebbARM = ebbLEFT_FOOT,		// Don't ask.
	ebbFRIGHT_FOOT,
	ebbFLEFT_FOOT,
	ebbEND
};

//**********************************************************************************************
//
class CSkeletonRenderInfo
//
// Prefix: sri
//
//	A class that contains bone/joint info for biomodel renderinfo structures.  
//
//	Notes:
//		This class is where physics and renderer meet, and hopefully it is the only place.
//
//		Physics stuffs the joint transforms, the renderer reads them.
//
//**************************************
{
public:
	CAArray< CTransform3<> >	patf3JointTransforms;	//  An array of transforms describing each
														//  joint/bones placement relative to the 
														//  biomodel's local coordinate frame.
	CBoundVolBox				bvbVolume;				// The bounding box of the mesh for the 
														// current joint configuration.

	CSkeletonRenderInfo() 
	{
	}

	CSkeletonRenderInfo(CPArray< CTransform3<> > patf3_joints) 
		: patf3JointTransforms(patf3_joints)
	{
	}

	CSkeletonRenderInfo(uint u_num_joints) 
		: patf3JointTransforms(u_num_joints)
	{
	}

	//******************************************************************************************
	//
	const CVector3<>& v3JointPos
	(
		int i_joint
	) const
	//
	// Gets the position of joint "i_joint" in object space.
	//
	//**********************************
	{
		return patf3JointTransforms[i_joint].v3Pos;
	}

	//******************************************************************************************
	//
	TReal rJointLen
	(
		int i_joint
	) const
	//
	// Gets the length of joint "i_joint" in object space, which is the distance to the next joint.
	//
	//**********************************
	{
		return (v3JointPos(i_joint) - v3JointPos(i_joint+1)).tLen();
	}

	//******************************************************************************************
	//
	CVector3<>	v3GetJointWorldPosition
	(
		int i_joint, 
		CInstance *pins
	) const;
	//
	// Gets the position of joint "i_joint" in world coords.
	//
	//**********************************
};


//*********************************************************************************************
//
class CAnimate: public CEntity
//
// Prefix: ant
//
// Notes:
//		An entity that has a biomesh attached.
//
//**************************************
{
public:

	float fHitPoints;	// Number of hit points an animate creature can sustain.  At zero, creature stops being a threat.
	float fMaxHitPoints;// Number of hit points a critter has when at full.
	float fRegenerationRate;	// Number of hit points a creature regains per second.
	float fReallyDead;	// Hit point level at which we actually realy and truly die.
	float fDieRate;		// Hit points lost each second that we are below zero.
	float fCriticalHit;	// More than this amount of hit points constitutes a critical hit on this guy.
	float fTranqPoints;			// Like hit points, but for tranquilizers.
	float fMaxTranqPoints;
	float fTranqRegenerationRate;

	//
	// The collide point system provides both the ability to integrate collision damage over
	// several frames (because that's how it happens in the physics system), and to filter
	// out small collisions.
	//
	float fCollidePoints;		// Number of points that can absorb collisions before damage.
	float fCollideResetPoints;
	float fMaxCollidePoints;	// Number of collide points when full (typically == fMaxHitPoints).
	float fCollideRegenerationRate;	// Rate at which they regenerate (very fast).
	float fCollideDamageThisFrame;	// The amount of energy damage taken this frame.  Used to make
									// Sure you don't take damage twice from the same event.

	int iTeam;			// What team am I on?

	CSkeletonRenderInfo	sriBones;			// The joint placements for this creature.
	bool				bSkeletonUpdated;	// Whether skeleton has been updated from physics.

	CSArray<CBoundaryBox*, ebbEND>	apbbBoundaryBoxes;	// The boxes that describe parts of the animal.  Zero if none.
	CPArray<CWound> paWounds;			// The animate's visible wounds.
	int iNumWounds;						// This should be part of a CMArray, but the MS compiler team has shit for brains.

	CInstance*	pinsKiller;		// The instance that killed me, zero if none.

	static int iGoreLevel;			// Controls how much animates bleed.
									// 0 = Tipper Gore level.
									// 1 = Realistic Gore level.
									// 2 = AL (A Lot) Gore level.

	//******************************************************************************************
	//
	// Constructors.
	//

	CAnimate();

	CAnimate
	(
		const SInit& initins
	);

	CAnimate
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);
	
	~CAnimate();

	//******************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual void ParseProps
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);
	//
	//	Looks at the props and sets up internal variables based on them.
	//
	//**************************

	//*****************************************************************************************
	//
	void AddBoundaryBox
	(
		EBoundaryBoxes ebb,					// Which box to add.
		const CVector3<>& v3_size,			// Size of box (half dimensions).
		const CVector3<>& v3_offset,		// Offset from joint.
		float f_mass,						// Mass.
		TSoundMaterial smat,				// Sound material.
		float f_damage = 1.0,				// Offense multiplier.
		float f_armour = 1.0,				// Defense multiplier.
		const char* str_name = 0			// Optional box instance name.
	);
	//
	//	Adds a box of the given type, if one does not already exist.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bCanHaveChildren
	(
	);
	//
	// Returns 'false.'
	//
	//**************************

	//*****************************************************************************************
	//
	virtual CPlacement3<> p3HeadPlacement
	(
	) const;
	//
	// Returns:
	//		The current placement of the head (relative to the body).
	//
	//**************************************

	//*****************************************************************************************
	//
	float fGetHitPoints
	(
	) const
	//
	// Returns the number of hit points left.
	//
	//**************************************
	{
		return fHitPoints;
	}

	//*****************************************************************************************
	//
	virtual bool bDead
	(
	) const
	//
	// Returns 'true' if the critter is dying, otherwise returns 'false.'
	//
	//**************************************
	{
		return fHitPoints <= 0.0f;
	}
	
	//*****************************************************************************************
	//
	virtual bool bReallyDead
	(
	) const
	//
	// Returns 'true' if the critter is actually dead, otherwise returns 'false.'
	//
	//**************************************
	{
		return fHitPoints <= fReallyDead;
	}

	//*****************************************************************************************
	//
	virtual float fCalculateHitPoints
	(
		const CInstance* pins,				// The thing getting hit.
		const CMessageCollision& msgcoll	// A collision event.
	);
	//
	// Code to handle a collision after hit points have been subtracted.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual float fWieldDamage
	(
		const CInstance* pins_weapon,		// The object wielded.
		float f_coll_damage					// Normal collision damage this weapon would do.
	) const;
	//
	// Returns the damage done by this animate by wielding this weapon.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void HandleDamage
	(
		float f_damage, const CInstance* pins_aggressor = 0, const CInstance* pins_me = 0
	)
	//
	// what happens when an animate takes damage
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	void AddWound
	(
		const CVector3<>& v3_pos,			// World location of wound.
		CGun* pgun,							// Gun causing wound.
		float f_damage,						// Amount of damage it did.
		float f_tranq,						// Amount of tranquilisation it did.
		CBoundaryBox* pbb					// Which box was hit.
	);
	//
	// Adds a visible, bleeding wound to the animate.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void Substitute
	(
		int i_sub = 0						// Substitution index.
	);
	//
	// Switch to an alternate pre-defined shape.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void UpdatePhysicsHealth();
	//
	// Inform physics of current health.
	//
	//**********************************

	//*****************************************************************************************
	//
	// Overides.
	//


	//*****************************************************************************************
	virtual int iGetTeam() const;

	//*****************************************************************************************
	virtual const CAnimate* paniGetOwner() const;

	//*****************************************************************************************
	virtual const CBoundVol* pbvBoundingVol() const;

	//*****************************************************************************************
	virtual CInstance* pinsCopy() const;

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

	//
	// Message processing overrides.
	//

	//******************************************************************************************
	virtual void Move
	(
		const CPlacement3<>& p3_new,
		CEntity* pet_sender = 0
	);

	//******************************************************************************************
	virtual void Process
	(
		const CMessagePhysicsReq& msgpr
	);

	//*****************************************************************************************
	virtual void Process
	(
		const CMessageCollision& msgcoll	// A collision event.
	);

	//*****************************************************************************************
	virtual void PreRender
	(
		CRenderContext& renc			// Target, camera, settings, etc.
	);

	//******************************************************************************************
	virtual void Process
	(
		const CMessageStep& msgstep
	);

	//
	// Identifier functions.
	//

	//*****************************************************************************************
	//lint -save -e1411
	virtual void Cast(CAnimate** ppant)
	{
		*ppant = this;
	}
	//lint -restore

protected:

	//*****************************************************************************************
	//
	void Init();
	//
	// Aids in construction.
	//
	//**********************************
};


class CPhysicsInfoBox;

//*********************************************************************************************
//
class CBoundaryBox: public CInstance
//
// Prefix: bb
//
// Notes:
//		An entity that is attached to a CAnimate as a boundary condition
//
//**************************************
{
public:

	CAnimate*		paniAnimate;		// The animate object this is linked to.
	EBoundaryBoxes	ebbElement;			// Which box this is.

	float			fDamagePerSecond;	// How much damage can this box inflict per second of contact?
	float			fDamageMultiplier;	// 0-1 value, settable by the AI system to allow varying damage
										// depending on the animal's current activity.  Usually 1.


	//******************************************************************************************
	//
	// Constructors.
	//

	//******************************************************************************************
	CBoundaryBox
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	//******************************************************************************************
	CBoundaryBox
	(
		CAnimate* pani,						// Owning animate object.
		EBoundaryBoxes ebb,					// Which box.
		const CVector3<>& v3_offset,		// The offset from joint.
		const CPhysicsInfoBox& phib,		// Physics data for box.
		const char* str_name				// Instance name.
	);

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	int iPhysicsElem() const;
	//
	// Returns:
	//		The physics-model element corresponding to this box.
	//
	//******************************************************************************************

	//******************************************************************************************
	//
	CVector3<> v3GetOffset() const;
	//
	// Returns:
	//		The offset of this box away from its joint, in world-scale joint space.
	//
	//******************************************************************************************


	//******************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual int iGetTeam() const;

	//*****************************************************************************************
	virtual const CAnimate* paniGetOwner() const;

	//*****************************************************************************************
	virtual CInstance* pinsCopy() const;

	virtual void Cast(CBoundaryBox** ppbb)
	{
		*ppbb = this;
	}
};





// HEADER_LIB_ENTITYDBASE_ANIMATE_HPP
#endif
