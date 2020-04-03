/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CPhysicsInfo
 *
 * Bugs:
 *
 * To do:
 *		Implement proper instancing.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/PhysicsInfo.hpp                                       $
 * 
 * 74    98/10/01 16:15 Speter
 * New presence used for collide volumes.
 * 
 * 73    98/09/30 19:04 Speter
 * Set instances in SetupSuperBox.
 * 
 * 72    98/09/19 14:37 Speter
 * Removed CPhysicsInfo::HandleMessage for collisions; now processed in canonical manner by
 * CPhysicsSystem.
 * 
 * 71    9/04/98 4:35p Agrant
 * added the floats flag
 * 
 * 70    98/09/02 21:18 Speter
 * Removed i_activity param from Activate().
 * 
 * 69    98/08/27 0:52 Speter
 * Added bIsMoving(), made some args const.
 * 
 * 68    8/20/98 11:04p Agrant
 * A tail position accessor function
 * 
 * 67    98/08/19 2:32 Speter
 * Added HandleMessage for collision.
 * 
 * 66    8/10/98 5:23p Rwyatt
 * Removed iTotal and replaced it with a memory counter
 * 
 * 65    98/06/29 14:04 Speter
 * Added locational smatGetMaterialType(), useful for terrain.
 * 
 * 64    98/06/25 17:30 Speter
 * Implemented several suboptions for DrawPhysics (bones). RayIntersect function changed to take
 * explicity subobject index.
 * 
 * 63    6/18/98 4:42p Agrant
 * fMass takes a const instance pointer
 * 
 * 62    6/13/98 8:01p Agrant
 * added the reverse knees boolean, for reversing quadruped legs
 * 
 * 61    6/11/98 2:57p Agrant
 * added SMALL flag
 * 
 * 60    98/06/09 21:32 Speter
 * Moved default physics params here. Removed include of "PhysicsSystem.hpp", added other
 * includes and opaque types.
 * 
 * 59    98/05/22 20:55 Speter
 * Changed raycast construction to let objects insert multiple intersections into list. Changed
 * ApplyImpulse to accept subobject index.
 * 
 * 58    5/21/98 11:46a Agrant
 * Added damage and armour multipliers to allow different objects to inflict different amounts
 * of damage.
 * 
 * 57    98/05/20 18:55 Speter
 * Now picks up hinged compounds with correct orientation.
 * 
 * 56    98/05/18 19:43 Speter
 * Added per-element friction and sound. Changed GetMaterialType to smatGetMaterialType.
 * 
 * 55    98/05/15 16:12 Speter
 * Added separate volume for collisions, queries, etc. (takes account of minimum sizes).
 * 
 * 54    5/14/98 8:08p Agrant
 * removed poval_physics
 * 
 * 53    98/05/09 15:17 Speter
 * fVolume() now virtual.
 * 
 * 52    5/09/98 11:19a Agrant
 * don't assert for bad mass, just return -1
 * 
 * 51    98/04/16 14:15 Speter
 * Added bRayIntersect() to CPhysicsInfo; utilise equivalent CBoundVol functions.
 * 
 * 50    2/05/98 5:06p Agrant
 * Head position query
 * 
 * 49    1/20/98 3:21p Agrant
 * prototype for ForceVelocity function, for later use in triggers
 * 
 * 48    97/12/15 18:45 Speter
 * Moved world magneting code to NMagnetSystem.
 * 
 * 47    97/12/11 14:19 Speter
 * Reduced include bloat, removed unneeded dimensions from array args.
 * 
 * 46    97/12/03 17:59 Speter
 * Objects can now be activated with velocity.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_PHYSICSINFO_HPP
#define HEADER_LIB_ENTITYDBASE_PHYSICSINFO_HPP

#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/Audio/SoundDefs.hpp"
#include "Lib/Loader/Fetchable.hpp"
#include "Lib/Sys/MemoryLog.hpp"

class CMesh;
class CRenderType;
class CDraw;
class CCamera;
class CPhysicsInfoBox;
class CPhysicsInfoCompound;
class CPhysicsInfoSkeleton;
class CMagnetPair;
class CRayCast;
class CMessagePhysicsReq;

class CGroffObjectName;
class CLoadWorld;
class CHandle;
class CObjectValue;
class CValueTable;

enum EPhysicsFlags
{
	epfTANGIBLE = 0,		// True if the object has a physical reality and can be touched
	epfMOVEABLE,			// True if the object can be physically moved.
	epfHACK_UNIQUE,			// True if the info is unique and never shared.
							// This is a hack that will go away when we do the 
							// correct thing about instancing/ref counting.
	epfMANAGED,				// True if the class has been instantiated by a memory manager
							// or instancer and requires special treatment.  
	epfBIOMANAGED,			// Object is part of a bio-model (handled specially).
	epfSMALL,				// Object is smaller than the normal minimum size.
	epfREVERSE_KNEES,		// True when the knees go the other way.
	epfFLOATS,				// True when the object floats in water.
	epfEND		
};

#define fDEFAULT_DENSITY	0.10f
#define fDEFAULT_FRICTION	5.0f
#define fDEFAULT_ELASTICITY	0.2f

enum EDrawPhysics
{
	edfBOXES,
	edfBOXES_PHYSICS,
	edfBOXES_WAKE,
	edfBOXES_QUERY,
	edfBOX_CENTRES,
	edfMAGNETS,
	edfSKELETONS,
	edfATTACHMENTS,
	edfWATER,
	edfRAYCASTS
};

//**********************************************************************************************
//
struct SPhysicsData
//
// Prefix: phd
//
// Initialising values for CPhysicsInfo.
//
// Notes:
//		This class is somewhat temporary, and is related to the SInstanceData hack.
//		Ultimately, each type of CInstance, and each type of CPhysicsInfo will need to procure
//		its own initialising values from the GROFF file.
//
//**************************************
{
	CSet<EPhysicsFlags>	setFlags;			// Various physics flags.
	TSoundMaterial	matMaterial;
	bool			bIsBox;
	float			fDensity;
	float			fFriction;
	float			fElasticity;

	// Constructor.  Currently, only a constructor for box models exists.
	SPhysicsData
	(
		TSoundMaterial	mat			= 0,
		float			f_density	= fDEFAULT_DENSITY,
		float			f_friction	= fDEFAULT_FRICTION,
		float			f_elasticity= fDEFAULT_ELASTICITY
	) :
		matMaterial	(mat),
		bIsBox		(true),
		fDensity	(f_density),
		fFriction	(f_friction),
		fElasticity	(f_elasticity)
	{
	}

	SPhysicsData
	(
		CSet<EPhysicsFlags>	set_flags,
		TSoundMaterial	mat				= 0,
		float			f_density		= fDEFAULT_DENSITY,
		float			f_friction		= fDEFAULT_FRICTION,
		float			f_elasticity	= fDEFAULT_ELASTICITY
	) :
		setFlags	(set_flags),
		matMaterial	(mat),
		bIsBox		(true),
		fDensity	(f_density),
		fFriction	(f_friction),
		fElasticity	(f_elasticity)
	{
	}

	SPhysicsData
	(
		bool			b_is_movable,
		bool			b_is_tangible	= true,
		TSoundMaterial	mat				= 0,
		float			f_density		= fDEFAULT_DENSITY,
		float			f_friction		= fDEFAULT_FRICTION,
		float			f_elasticity	= fDEFAULT_ELASTICITY
	) :
		matMaterial	(mat),
		bIsBox		(true),
		fDensity	(f_density),
		fFriction	(f_friction),
		fElasticity	(f_elasticity)
	{
		setFlags[epfTANGIBLE] = b_is_tangible;
		setFlags[epfMOVEABLE] = b_is_movable;
	}
};

extern const CPlacement3<> p3VELOCITY_ZERO;

//**********************************************************************************************
//
class CPhysicsInfo : public CRefObj, public CFetchable
//
// Prefix: phi
//
// Class holding shared physics info.
//
//	Notes:
//		Abstract base class only.
//
//**************************************
{
public:
	static CSet<EDrawPhysics> setedfMain;	// The global set of flags specifying which physics to draw.

	CSet<EPhysicsFlags>	setFlags;		// Various physics flags.
	float				fDamage;		// Damage inflicted multiplier (applied to force/velocity).
	float				fArmour;		// Damage taken multiplier.

protected:
	 TSoundMaterial tmatSoundMaterial;	// The material used by the sound system to determine the collision
										// sounds generated.

	
	//******************************************************************************************
	//
	// Constructors.
	//
public:

	CPhysicsInfo(bool b_hack_unique = true)
		: tmatSoundMaterial(0)
	{
		setFlags[epfHACK_UNIQUE] = b_hack_unique;
		fDamage = 1.0f;
		fArmour = 1.0f;

		MEMLOG_ADD_COUNTER(emlTotalPhysicsInfo,1);
	}

	CPhysicsInfo(const SPhysicsData& phd, bool b_hack_unique = true)
		: tmatSoundMaterial(phd.matMaterial), setFlags(phd.setFlags)
	{
		setFlags[epfHACK_UNIQUE] = b_hack_unique;
		fDamage = 1.0f;
		fArmour = 1.0f;
		MEMLOG_ADD_COUNTER(emlTotalPhysicsInfo,1);
	}

	CPhysicsInfo
	(
		const CGroffObjectName*		pgon,
		const ::CHandle&				h_obj,				// Handle to the object in the value table.
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				// the loader.
	);

	~CPhysicsInfo()
	{
		MEMLOG_SUB_COUNTER(emlTotalPhysicsInfo,1);
	}
	
	// Dummy copy constructor. Use the virtual copy function below instead.
//	CPhysicsInfo(const CPhysicsInfo& phi)
//	{
//		Assert(false);
//	}
	
	//******************************************************************************************
	//
	// Member functions.
	//


	//*****************************************************************************************
	//
	virtual CPhysicsInfo* pphiCopy
	(
	)
	//
	// Copies this, returning a unique, non-instanced object outside of the instancing system.
	//
	//**************************
	{
		return new CPhysicsInfo(*this);
	}

	//*****************************************************************************************
	//
	static const CPhysicsInfo* pphiFindShared
	(
		const rptr<CRenderType>&	prdt,		// The mesh of the object needing the physics data.
		const CGroffObjectName*	pgon,				// Object using the physics info- needed for compound info construction
		const ::CHandle&		h_obj,				// Handle to the object in the value table.
		CValueTable*		pvtable,			// Pointer to the value table.
		CLoadWorld*			pload				// the loader.
	);
	// Obtain a CPhysicsInfo class that has the requested data.
	//
	// Notes:
	//		Uses value table to the right kind of physics info.  If the 
	//		epfMANAGED flag is set, DO NOT DELETE the returned data.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void HandleMessage
	(
		const CMessagePhysicsReq&, //msgpr,
		CInstance *                //pins
	) const
	//
	// Updates the instance based on message content and internal physics model info.
	//
	// Notes:
	//		Called when an instance does not know how to deal with a CMessagePhysicsReq.
	//
	//**************************
	{
		// Only biomodels should get requests.
		Assert(0);
	}

	//*****************************************************************************************
	//
	virtual const CBoundVol* pbvGetBoundVol
	(
	) const
	//
	// Returns a pointer to a bounding volume that describes the object when transformed by
	//	the CInstance's CPresence.
	//
	//**************************
	{
		static CBoundVolInfinite bvi;
		return &bvi;
	}

	//*****************************************************************************************
	//
	virtual const CBoundVol* pbvGetCollideVol
	(
	) const
	//
	// Returns the volume effectively used for collision tests, queries, etc.
	// This may be larger than the actual bounding volume, as a mimimum size is enforced.
	//
	//**************************
	{
		return pbvGetBoundVol();
	}

	//*****************************************************************************************
	//
	bool bContains
	(
		const CVector3<> &v3_point,			// The point to test.
		const CPresence3<> &pr3_instance	// The instance's presence
	) const
	//
	// Returns true if this physics info contains the point.
	//
	//	Notes:
	//		pr3_instance take the instance from local to world coords.  Point is in local coords.
	//
	//**************************
	{
		// A default implementation.
		CVector3<> v3 = v3_point * ~pr3_instance;
		return pbvGetBoundVol()->bContains(v3);
	}

	//*****************************************************************************************
	//
	virtual void Activate
	(
		CInstance *pins,			// Instance owning this physics info.
		bool b_just_update = false,	// If true, just updates physics to current world position
									// if already active, does nothing if inactive.
		const CPlacement3<>& p3_vel	= p3VELOCITY_ZERO
									// Linear and rotational velocity; defaults to all 0.
	) const
	//
	// Wakes up the instance and places it in the physics system with the physics model described 
	//	by the CPhysicsInfo class.
	//
	//**************************
	{
	}

	//*****************************************************************************************
	//
	virtual void Deactivate
	(
		CInstance *pins
	) const
	//
	// Wakes up the instance and places it in the physics system with the physics model described 
	//	by the CPhysicsInfo class.
	//
	//**************************
	{
	}

	//*****************************************************************************************
	//
	virtual bool bIsActive
	(
		const CInstance* pins
	) const
	//
	// Returns:
	//		Whether the object is currently active in the physics system.
	//
	//**********************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	virtual bool bIsMoving
	(
		const CInstance* pins
	) const
	//
	// Returns:
	//		Whether the object is currently active and moving in the physics system.
	//
	//**********************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	virtual CPlacement3<> p3GetVelocity
	(
		const CInstance* pins
	) const
	//
	// Returns:
	//		The current physics velocity, if active (else 0).
	//
	//**********************************
	{
		// Default version returns no velocity.
		return CPlacement3<>();
	}

	//*****************************************************************************************
	//
	virtual CVector3<> v3GetHeadPosition
	(
		const CInstance* pins
	) const
	//
	// Returns:
	//		The current head position if that has meaning, otherwise the current position,
	//
	//**********************************
	{
		return pins->v3Pos();
	}

	//*****************************************************************************************
	//
	virtual CVector3<> v3GetTailPosition
	(
		const CInstance* pins
	) const
	//
	// Returns:
	//		The current tail position if that has meaning, otherwise the current position,
	//
	//**********************************
	{
		return pins->v3Pos();
	}

	//*****************************************************************************************
	//
	virtual void UpdateWDBase
	(
		CInstance *	pins,		// The instance to update
		int			i_index		// The index of that instance in the appropriate physics
								// system array.
	) const
	//
	// Updates the world database for changes to this instance in the physics system.
	//
	//**************************
	{
	}

	//*****************************************************************************************
	//
	virtual void UpdateWDBase
	(
		CInstance *	pins,		// The instance to update
		int			i_index,	// The index of that instance in the appropriate physics
								// system array.
		int			i_element	// The element index in the superbox array.
	) const
	//
	// Updates the world database for changes to this instance in the physics system.
	//
	//**************************
	{
	}

	//**********************************************************************************************
	//
	virtual void RayIntersect
	(
		CInstance* pins,				// Owning instance.
		int i_subobj,					// Subobject this corresponds to; 0 if n.a.
		CRayCast& rc,					// The raycast to add the intersections to.
		const CPlacement3<>& p3,		// Origin and direction of ray.
		TReal r_length,					// Length of ray.
		TReal r_diameter				// Width of ray.
	) const;
	//
	// Adds zero or more SObjectLoc intersection records to the raycast.
	//
	//**********************************

	//**********************************************************************************************
	//
	virtual void ApplyImpulse
	(
		CInstance* pins,				// Owning instance.
		int i_subobj,					// Index of subobject, if applicable (use 0 if not).
		const CVector3<>& v3_pos,		// World coordinate of impulse.
		const CVector3<>& v3_impulse	// Vector impulse to apply (kg m/s).
	) const
	//
	// Activates the object if necessary, and gives it a kick.
	//
	//**********************************
	{
	}

	//*****************************************************************************************
	//
	virtual void ForceVelocity
	(
		CInstance* pins_target,				// object whose velocity we will modify
		const CVector3<>& v3_new_velocity	// velocity to set
	) const
	//
	// Wakes up the target.  Slams its velocity to v3_new_velocity, in world coords (m/sec)
	//
	//**************************
	{
		Assert(false);
	}


	//*****************************************************************************************
	//
	virtual TSoundMaterial smatGetMaterialType() const
	//
	// Gets the material type.
	//
	//**************************
	{
		return tmatSoundMaterial;
	}

	//*****************************************************************************************
	//
	virtual TSoundMaterial smatGetMaterialType
	(
		const CVector3<>& v3_world			// World location to check.
	) const
	//
	// Returns the material type at the given location.
	//
	//**************************
	{
		return tmatSoundMaterial;
	}

	//*****************************************************************************************
	//
	virtual void SetMaterialType
	(
		TSoundMaterial tmat
	)
	//
	// Gets the material type.
	//
	//**************************
	{
		tmatSoundMaterial = tmat;
	}


	//*****************************************************************************************
	//
	bool bIsMovable
	(
	) const 
	//
	// True if the physics system can move the object.
	//
	//**************************
	{
		return setFlags[epfMOVEABLE];
	}

	//*****************************************************************************************
	//
	bool bIsTangible
	(
	) const
	//
	// True if the physics system can move the object.
	//
	//**************************
	{
		return setFlags[epfTANGIBLE];
	}

	//*****************************************************************************************
	//
	bool bFloats
	(
	) const 
	//
	// True if the physics system can move the object.
	//
	//**************************
	{
		return setFlags[epfFLOATS];
	}

	//*****************************************************************************************
	//
	virtual CSet<EPhysicsFlags> epfPhysicsFlags
	(
	) const
	//
	// Returns the physics flags.
	//
	//**************************
	{
		return setFlags;
	}

	//*****************************************************************************************
	//
	virtual CPlacement3<> p3Base
	(
		CInstance* pins
	) const
	//
	// Returns the base placement of the physics object.
	// This is used as the frame for hinge magnets, for example.
	//
	//**************************
	{
		// Default returns instance placement.
		return pins->p3GetPlacement();
	}

	//*****************************************************************************************
	//
	virtual CPresence3<> pr3Collide
	(
		CInstance* pins
	) const
	//
	// Returns the presence used for the collide volume.
	//
	//**************************
	{
		// Default returns instance presence.
		return pins->pr3GetPresence();
	}

	//*****************************************************************************************
	//
	virtual void DrawPhysics
	(
		CInstance* pins, 		// Owning instance.
		CDraw& draw,			// Line draw object.
		CCamera& cam			// Current view camera.
	) const
	//
	// Draws the physics info in wireframe, using the line drawing object (for debugging).
	//
	// Notes:
	//		The draw colour is preset.
	//
	//**************************
	{
		// Default does nothing.
	}


	//**********************************************************************************************
	//
	virtual void SetupSuperBox
	(
		const CPresence3<>& pr3_master,
		const CPresence3<>& pr3_slave,
		CMagnetPair* pmp,
		int* pi_elem,
		CInstance* apins[],					// Instances.
		float aaf_extents[][6],				// Offsets and sizes.
		float aaf_orient[][3][3],			// Orientation matrices.
		float af_mass[],					// Masses.
		float af_friction[],				// Frictions.
		int ai_sound[],						// Sounds.
		float aaf_magnet_pos[][3],			// Magnet positions.
		float af_breaking[]					// Magnet strengths.
	) const
	//
	//  Initializes arrays for call to CreateBoxModel.
	//
	//**************************
	{
		Assert(false);
	}

	//*****************************************************************************************
	//
	virtual TReal fVolume
	(
		CInstance* pins					// Owning instance.
	) const
	//
	// Returns:
	//		The volume of this object.
	//
	//**********************************
	{
		return -1;
	}

	//*****************************************************************************************
	//
	virtual TReal fMass
	(
		const CInstance* pins					// Owning instance.
	) const
	//
	// Returns:
	//		The mass of this object.  Currently calculated from volume and density.
	//
	//**********************************
	{
		return -1;
	}

	//*****************************************************************************************
	//
	virtual float fDamageMultiplier
	(
		const CInstance* pins = 0,
		TSoundMaterial tsmat = 0
	) const
	//
	// Returns:
	//		The amount by which to multiply damage inflicted by this object.  Takes in a sound 
	//		material as a hack for biomodels.
	//
	//**********************************
	{
		return fDamage;
	}

	//*****************************************************************************************
	//
	virtual float fArmourMultiplier
	(
		const CInstance* pins = 0,
		TSoundMaterial tsmat = 0
	) const
	//
	// Returns:
	//		The amount by which to multiply damage taken by this object.  Takes in sound 
	//		material as a hack for biomodels.
	//
	//**********************************
	{
		return fArmour;
	}

	


	virtual CPhysicsInfoBox* ppibCast()
	{ return 0; }

	virtual CPhysicsInfoBox const * ppibCast() const
	{ return 0; }

	virtual CPhysicsInfoSkeleton* ppisCast()
	{ return 0; }

	virtual CPhysicsInfoSkeleton const * ppisCast() const
	{ return 0; }

	virtual CPhysicsInfoCompound* pphicCast() 	{ return 0; }
	virtual const CPhysicsInfoCompound* pphicCast() const  	{ return 0; }


};


//*****************************************************************************************
//
// Global variables.
//

	// A master copy of default physics info, to use for objects not requiring physics.
	extern CPhysicsInfo phiDEFAULT;



//#ifndef HEADER_LIB_ENTITYDBASE_PHYSICSINFO_HPP
#endif
