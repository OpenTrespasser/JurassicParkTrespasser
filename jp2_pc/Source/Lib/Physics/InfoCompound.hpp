/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CPhysicsInfoCompound
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/InfoCompound.hpp                                          $
 * 
 * 24    98/10/01 16:17 Speter
 * New presence used for tighter fitting collide volumes.
 * 
 * 23    98/09/30 19:04 Speter
 * Set instances in SetupSuperBox.
 * 
 * 22    98/09/02 21:19 Speter
 * Removed i_activity param from Activate().
 * 
 * 21    98/08/27 0:52 Speter
 * Added bIsMoving(), made some args const.
 * 
 * 20    98/06/25 17:36 Speter
 * RayIntersect function changed to take explicity subobject index. 
 * 
 * 19    6/18/98 4:43p Agrant
 * mass takes a const instance pointer
 * 
 * 18    98/05/22 20:57 Speter
 * Changed raycast construction to let objects insert multiple intersections into list. Changed
 * ApplyImpulse to accept subobject index.
 * 
 * 17    98/05/20 18:55 Speter
 * Now picks up hinged compounds with correct orientation.
 * 
 * 16    98/05/18 19:38 Speter
 * Added per-element friction and sound.
 * 
 * 15    98/05/15 16:08 Speter
 * Compound boxes now have actual total bounding box.
 * 
 * 14    5/14/98 8:07p Agrant
 * Removed the defunct poval_physics arguemnt from physics info constructors
 * 
 * 13    98/05/09 15:13 Speter
 * Fix.
 * 
 * 12    98/05/09 14:36 Speter
 * Implemented fVolume(), fMass(), and ForceVelocity().
 * 
 * 11    4/17/98 2:18p Agrant
 * default constructor
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_INFOCOMPOUND_HPP
#define HEADER_LIB_PHYSICS_INFOCOMPOUND_HPP

#include "Lib/EntityDBase/PhysicsInfo.hpp"
struct SPhysicsData;

#define iMAX_SUBMODELS 10		// The max number of submodels.

//**********************************************************************************************
//
class CPhysicsInfoCompound: public CPhysicsInfo
//
// Prefix: phic
//
// Class holding physics info that has multiple parts.  Shared.
//
//**************************************
{
public:

	int iNumSubmodels;												// How many submodels?
	CMSArray<const CPhysicsInfo*, iMAX_SUBMODELS>	apphiSubmodels;			// An array of submodels, null terminated.
	CMSArray<CPresence3<>, iMAX_SUBMODELS>	apr3SubmodelPresences;	// An array of submodels, presences.

	int iCurrentSubmodel;				// A hack to allow submodels to find themselves in the submodel array.
										// When you call a submodel's function, set iCurrentSubmodel to the index 
										// of that submodel.  This is a huge, horrible hack.  
private:
	CBoundVolBox	bvbBound;			// The box encompassing all submodels bounding volumes.,
										// centred, in instance space.
	CBoundVolBox	bvbCollide;			// The box encompassing all submodels' collide volumes.
										// stored in the orientation of sub-model 0.
	CVector3<>		v3Collide;			// The centre of this box, in object space.

public:
	//*****************************************************************************************
	//
	CPhysicsInfoCompound
	(
		const CHandle&				h_obj,				// Handle to the object in the value table.
		const CGroffObjectName*		pgon,		// Object to load.
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				
	);

	CPhysicsInfoCompound
	(
	)
	{
		iNumSubmodels = 0;
	};

	static const CPhysicsInfoCompound* pphicFindShared
	(
		const CHandle&				h_obj,				// Handle to the object in the value table.
		const CGroffObjectName*	pgon,		// Object to load.
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				
	);

	//
	// Constructs a compound physics model with no sub-models.
	//
	//**********************************

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void AddSubmodel
	(
		const CPhysicsInfo* pphi,	// The model to add.
		const CPresence3<>& pr3		// Model presence, relative to compound info.
	);
	//
	// Adds a submodel to the info.
	//
	//**************************



	//*****************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual const CBoundVol* pbvGetBoundVol() const override
	{
		return &bvbBound;
	}

	//*****************************************************************************************
	virtual const CBoundVol* pbvGetCollideVol() const override
	{
		return &bvbCollide;
	}

	//*****************************************************************************************
	virtual TReal fVolume(CInstance* pins) const override;

	//*****************************************************************************************
	virtual TReal fMass(const CInstance* pins) const override;

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
	virtual bool bIsActive(const CInstance* pins) const override;

	//*****************************************************************************************
	virtual bool bIsMoving(const CInstance* pins) const override;

	//*****************************************************************************************
	virtual CPlacement3<> p3GetVelocity(const CInstance* pins) const override;

	//*****************************************************************************************
	virtual void UpdateWDBase(CInstance* pins, int i_index,	int	i_element) const override;

	//**********************************************************************************************
	virtual void RayIntersect(CInstance* pins, int i_subobj, CRayCast& rc,
							  const CPlacement3<>& p3, TReal r_length, TReal r_diameter) const override;

	//*****************************************************************************************
	virtual void ApplyImpulse(CInstance* pins, int i_subobj, const CVector3<>& v3_pos, const CVector3<>& v3_impulse) const override;

	//*****************************************************************************************
	virtual void ForceVelocity(CInstance* pins_target, const CVector3<>& v3_new_velocity) const override;

	//*****************************************************************************************
	virtual CPlacement3<> p3Base(CInstance* pins) const override;

	//*****************************************************************************************
	virtual CPresence3<> pr3Collide(CInstance* pins) const override;

	//*****************************************************************************************
	virtual void HandleMessage(const CMessagePhysicsReq& msgpr, CInstance *pins) const override;

	//*****************************************************************************************
	virtual void DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const override;

	//**********************************************************************************************
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
	) const override;
	//**************************


	virtual CPhysicsInfoCompound* pphicCast() override { return this; }
	virtual const CPhysicsInfoCompound* pphicCast() const override { return this; }
};

#endif
