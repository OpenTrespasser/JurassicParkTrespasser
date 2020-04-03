/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CPhysicsInfoBox
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/InfoBox.hpp                                               $
 * 
 * 37    98/09/30 19:04 Speter
 * Set instances in SetupSuperBox.
 * 
 * 36    98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 35    98/09/02 21:19 Speter
 * Removed i_activity param from Activate().
 * 
 * 34    98/08/27 0:52 Speter
 * Added bIsMoving(), made some args const.
 * 
 * 33    98/07/27 18:09 Speter
 * Added once-per-frame UpdateQueries(), for faster terrain query refresh.
 * Moved fBoxTerrain declaration to PhysImport.
 * 
 * 32    98/07/24 14:07 Speter
 * Moved some box-specific stuff here from CPhysicsSystem.
 * 
 * 31    98/07/20 13:17 Speter
 * New un-ignore code.
 * 
 * 30    98/07/09 16:43 Speter
 * Pivot points now stored in CPhysicsInfoBox. Added handler for object create/delete, which
 * purges cached queries.
 * 
 * 29    98/06/25 17:32 Speter
 * Implemented several suboptions for DrawPhysics (bones). RayIntersect function changed to take
 * explicity subobject index. Removed old MotileBox code.
 * 
 * 28    6/18/98 4:43p Agrant
 * mass takes a const instance pointer
 * 
 * 27    98/06/02 23:28 Speter
 * Removed old code.
 * 
 * 26    98/05/25 17:30 Speter
 * Enabled the physics terrain query.
 * 
 * 25    98/05/25 15:35 Speter
 * Added but disabled support for proper terrain query.
 * 
 * 24    98/05/22 20:56 Speter
 * Changed raycast construction to let objects insert multiple intersections into list. Changed
 * ApplyImpulse to accept subobject index.
 * 
 * 23    98/05/22 13:14 Speter
 * Made bvbBoundBox private.
 * 
 * 22    98/05/22 1:38 Speter
 * Made all box activation utility functions private.
 * 
 * 21    98/05/21 19:17 Speter
 * Added several functions facilitating fast wake-up code.
 * 
 * 20    98/05/18 19:38 Speter
 * Added per-element friction and sound.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_PHYSICSINFOBOX_HPP
#define HEADER_LIB_ENTITYDBASE_PHYSICSINFOBOX_HPP

#include "Lib/EntityDBase/PhysicsInfo.hpp"

//**********************************************************************************************
//
class CPhysicsInfoBox : public CPhysicsInfo
//
// Prefix: phib
//
// Class holding shared physics info.
//
//**************************************
{
public:

	float			fDensity;		// The density of the object, what units?
	float			fFriction;		// The frictive quality of the object, 0 to 1.
	float			fElasticity;	// The elastic quality of the object (for collisions) 0 to 1.
	
	CBoundVolBox	bvbBoundVol;	
	CBoundVolBox	bvbCollideVol;	// Same volume, with minimum sizes enforced.
	CVector3<>		v3Pivot;		// Pivot point for special physics purposes, such as joint attachment.
	
	//******************************************************************************************
	//
	// Constructors.
	//

	CPhysicsInfoBox();
	
	CPhysicsInfoBox
	(
		const CBoundVolBox &bvb,		// Dimensions.
		const SPhysicsData& phd,		// General material params, including density.
		float f_mass_override = 0.0,	// Pass this to set mass directly instead of via density.
		float f_scale = 1.0,			// Scale of object, needed for mass override.
		const CVector3<>& v3_pivot = v3Zero	// Pivot point.
	);

	CPhysicsInfoBox
	(
		const rptr<CRenderType>&	prdt,				// The rendering shape.
		const ::CHandle&				h_obj,				// Handle to the object in the value table.
		const CGroffObjectName*		pgon,
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				// the loader.
	);


	//******************************************************************************************
	bool operator< 
	(
		const CPhysicsInfoBox& pib
	) const;

	
	//******************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	//
	virtual CPhysicsInfo* pphiCopy
	(
	);
	//
	// Copies this, returning a unique, non-instanced object outside of the instancing system.
	//
	//**************************

	//*****************************************************************************************
	//
	static const CPhysicsInfoBox* pphibFindShared
	(
		const CPhysicsInfoBox& phib
	);
	//
	// Obtain a CPhysicsInfoBox class that has the requested data.
	//
	// Notes:
	//		Gets info that looks like the one requested. This function will search for an
	//		existing info with the same properties. If one does not exist, this function will
	//		create a new info. Therefore, it may be either shared or be unique.
	//
	//		Sets the epfMANAGED flag.  Cannot be deleted through "delete"- let the instancer
	//		handle it, please!
	//
	//**************************

	//******************************************************************************************
	//
	virtual void ZeroRefs();
	// 
	//	Overridden to allow pphibFindShared to handle its own memory.
	//
	//**********************************

	//*****************************************************************************************
	virtual const CBoundVol* pbvGetBoundVol() const
	{
		return &bvbBoundVol;
	}

	//*****************************************************************************************
	virtual const CBoundVol* pbvGetCollideVol() const
	{
		return &bvbCollideVol;
	}

	//*****************************************************************************************
	virtual void Activate
	(
		CInstance *pins, 
		bool b_just_update = false, 
		const CPlacement3<>& p3_vel = p3VELOCITY_ZERO
	) const;

	//*****************************************************************************************
	virtual void Deactivate(CInstance *pins) const;

	//*****************************************************************************************
	virtual bool bIsActive(const CInstance* pins) const;

	//*****************************************************************************************
	virtual bool bIsMoving(const CInstance* pins) const;

	//*****************************************************************************************
	virtual CPlacement3<> p3GetVelocity(const CInstance* pins) const;

	//*****************************************************************************************
	static void UpdateWDBase(int i_index);

	//*****************************************************************************************
	virtual void UpdateWDBase(CInstance* pins, int i_index, int i_element) const;

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
	) const;
	//
	//  Initializes arrays for call to CreateBoxModel.
	//
	//**************************

	//**********************************************************************************************
	virtual void ApplyImpulse(CInstance* pins, int i_subobj, const CVector3<>& v3_pos, const CVector3<>& v3_impulse) const;

	//*****************************************************************************************
	virtual void ForceVelocity(CInstance* pins_target, const CVector3<>& v3_new_velocity) const;
	
	//*****************************************************************************************
	virtual void DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const;

	virtual CPhysicsInfoBox* ppibCast()
	{ return this; }

	virtual CPhysicsInfoBox const * ppibCast() const
	{ return this; }


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	virtual TReal fVolume(CInstance* pins) const;

	//*****************************************************************************************
	virtual TReal fMass(const CInstance* pins) const;

	//*****************************************************************************************
	//
	static void UpdateQueries();
	//
	//**********************************

	//*****************************************************************************************
	//
	static void ActivateIntersecting();
	//
	//**********************************

	//*****************************************************************************************
	//
	static int bIntersect
	(
		int i_me, int i_you				// Active box indices.
	);
	//
	// Returns:
	//		Whether these boxes intersect.
	//
	//**********************************

	//*****************************************************************************************
	//
	static void Reset();
	//
	// Resets boxes, clearing all data.
	//
	//**********************************

	//*****************************************************************************************
	//
	static void ProcessCreateDelete();
	//
	// React to instance creation/deletion.
	//
	//**********************************

	//*****************************************************************************************
	//
	static void Purge();
	//
	// Clean up any data for physics system destruction.
	//
	//**********************************

	//*****************************************************************************************
	//
	static CBoundVolBox bvbBoundBox
	(
		int i_index, int i_elem
	);
	//
	// Returns the box physics used for intersections (approximately).
	//
	//**********************************

	//*****************************************************************************************
	//
	static CPlacement3<> p3BoxElement
	(
		int i_index, int i_elem
	);
	//
	// Returns the world placement of the given element.
	//
	//**********************************
};

#endif