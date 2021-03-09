/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CPhysicsInfoBio
 *			CPhysicsInfoSkeleton
 *				CPhysicsInfoBiped
 *				CPhysicsInfoHuman
 *				CPhysicsInfoQuadruped
 *
 * Bugs:
 *
 * To do:
 *		Implement proper instancing.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/InfoSkeleton.hpp                                          $
 * 
 * 37    98/10/04 2:33 Speter
 * Removed unused functions.
 * 
 * 36    98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 35    98/09/12 1:04 Speter
 * Added Init() function to create boundary boxes at animal creation time.
 * 
 * 34    98/09/02 21:19 Speter
 * Removed i_activity param from Activate().
 * 
 * 33    98/08/27 0:52 Speter
 * Added bIsMoving(), made some args const.
 * 
 * 32    8/26/98 11:17a Agrant
 * skeleton velocity query
 * 
 * 31    8/20/98 11:04p Agrant
 * A tail position accessor function
 * 
 * 30    8/20/98 1:00p Agrant
 * mouth multiplier
 * mouth and speed multiplier loading
 * 
 * 29    8/17/98 7:41p Pkeet
 * Removed Animate.hpp dependency.
 * 
 * 28    8/17/98 6:46p Agrant
 * added speed multiplier to allow data control of dino speeds
 * 
 * 27    98/08/02 16:48 Speter
 * Removed AddBox().
 * 
 * 26    98/07/27 18:10 Speter
 * Moved fBioTerrain declaration to PhysImport. Added hack to prevent creation of old Anne head
 * box.
 * 
 * 25    98/07/24 14:05 Speter
 * Moved some skeleton stuff here from CPhysicsSystem.
 * 
 * 24    98/07/08 1:13 Speter
 * Added iPhysicsElem(); takes over boundary-box-to-physics connection from CAnimate.
 * 
 * 23    98/06/25 17:38 Speter
 * Implemented several suboptions for DrawPhysics (bones). RayIntersect function changed to take
 * explicity subobject index.
 * 
 * 22    98/06/19 16:58 Speter
 * Removed box_init from AddBox().
 * 
 * 21    6/18/98 4:43p Agrant
 * mass takes a const instance pointer
 * 
 * 20    98/06/14 17:29 Speter
 * Removed af_box_init from ActivateBoundaryBoxes. Made AddBox and ActivateBoundaryBoxes
 * protected.
 * 
 * 19    6/13/98 8:02p Agrant
 * Boundary box hacking
 * 
 * 18    98/06/07 18:59 Speter
 * Skeletons now have a PhysicsInfoBox, for mass, etc. UpdateSkeleton() now gets data from
 * physics, during rendering. Changed CPhysicsInfoRaptor to CPhysicsInfoBiped.
 * 
 * 17    6/03/98 3:02p Agrant
 * data driven dino boundary conditions
 * 
 * 16    6/01/98 6:08p Agrant
 * get and set internal physics health parameter
 * 
 * 15    98/05/25 15:36 Speter
 * Added but disabled support for proper terrain query.
 * 
 * 14    98/05/22 20:57 Speter
 * Changed raycast construction to let objects insert multiple intersections into list. Changed
 * ApplyImpulse to accept subobject index.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_PHYSICS_INFOSKELETON_HPP
#define HEADER_LIB_PHYSICS_INFOSKELETON_HPP

#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "InfoBox.hpp"

class CAnimate;

#define iMAX_SKELETAL_ELEMENTS	32

//**********************************************************************************************
//
class CPhysicsInfoSkeleton : public CPhysicsInfo
//
// Prefix: phis
//
// Class holding shared physics skeleton info.
//
//**************************************
{
protected:
	CPhysicsInfoBox	phibBounding;		// Bounding physics box; provides mass, volume, etc.
										// Contained rather than inherited to prevent undesirable
										// inherited physics functionality.

public:
	TReal	rSpeedMultiplier;			// Multiply an AI body/head move request by this number before
										// passing into the physics system.
	TReal	rMouthMultiplier;			// Multiply an AI mouth request by this number before
										// passing into the physics system.

	//******************************************************************************************
	//
	// Constructors.
	//

	CPhysicsInfoSkeleton(const CPhysicsInfoBox& phib);

	~CPhysicsInfoSkeleton();

	//******************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void ParseProps
	(
		const ::CHandle&				h_obj,				// Handle to the object in the value table.
		const CGroffObjectName*		pgon,
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				// the loader.
	);
	//
	//	Update the info for some extra props after creation via the box method.
	//
	//*****************

	//******************************************************************************************
	//
	// Overrides.
	//


	//*****************************************************************************************
	//
	virtual CPhysicsInfo* pphiCopy
	(
	) override
	//
	// Copies this, returning a unique, non-instanced object outside of the instancing system.
	//
	//**************************
	{
		Assert(0);
//		CPhysicsInfoBox* pphib_ret = new CPhysicsInfoBox();
//		*pphib_ret = *this;
//		return pphib_ret;
		return 0;
	}

	//*****************************************************************************************
	//
	virtual const CBoundVol* pbvGetBoundVol() const override
	{
		return &phibBounding.bvbBoundVol;
	}

	//*****************************************************************************************
	virtual TReal fVolume(CInstance* pins) const override
	{
		return phibBounding.fVolume(pins);
	}

	//*****************************************************************************************
	virtual TReal fMass(const CInstance* pins) const override
	{
		return phibBounding.fMass(pins);
	}

	//*****************************************************************************************
	//
	virtual void Activate
	(
		CInstance *pins,
		bool b_just_update = false,
		const CPlacement3<>& p3_vel = p3VELOCITY_ZERO
	) const override;
	//
	//**************************

	//*****************************************************************************************
	virtual void Deactivate(CInstance *pins) const override;

	//*****************************************************************************************
	virtual bool bIsActive(const CInstance* pins) const override;

	//*****************************************************************************************
	virtual bool bIsMoving(const CInstance* pins) const override;

	//*****************************************************************************************
	virtual CPlacement3<> p3GetVelocity(const CInstance* pins) const override;

	//*****************************************************************************************
	virtual void HandleMessage(const CMessagePhysicsReq& msgpr,	CInstance *pins) const override;

	//*****************************************************************************************
	virtual void UpdateWDBase(CInstance * pins,	int	i_index) const override;

	//**********************************************************************************************
	virtual void RayIntersect(CInstance* pins, int i_subobj, CRayCast& rc,
							  const CPlacement3<>& p3, TReal r_length, TReal r_diameter) const override;

	//*****************************************************************************************
	virtual void ApplyImpulse(CInstance* pins, int i_subobj, const CVector3<>& v3_pos, const CVector3<>& v3_impulse) const override;

	//*****************************************************************************************
	virtual void DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const override;

	virtual CPhysicsInfoSkeleton* ppisCast() override
	{ return this; }

	virtual const CPhysicsInfoSkeleton* ppisCast() const override
	{ return this; }

	//******************************************************************************************
	//
	// Member functions.
	//

	//
	// Interface to CAnimate and CBioMesh classes.
	//

	//*****************************************************************************************
	//
	virtual void UpdateSkeleton
	( 
		CInstance* pins								// World object.
	) const;
	//
	// Updates the object's joint positions, for rendering.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void Init
	( 
		CAnimate* pani							// World object.
	) const;
	//
	// Initialise any physics info in the animal.
	//
	// Should be called at animal creation time, after loading.
	//
	//**************************

	//*****************************************************************************************
	//
	static CAnimate* paniSkeleton
	(
		int i_index
	);
	//
	// Returns:
	//		The skeleton for this index, if any.
	//
	//**********************************

	//*****************************************************************************************
	//
	static void Reset();
	//
	// Resets the skeleton list, clearing all data.
	//
	//**********************************

	//*****************************************************************************************
	//
	static int iPhysicsElem
	(
		int i_index
	);
	//
	//	Returns:
	//		The physics-model element corresponding to this boundary box.
	//
	//**********************************

protected:

	//******************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual void CreatePhysics
	( 
		CInstance* pins,					// Owning instance.
		int i_index,						// Physics skeleton index.
		float aaf_state[7][3]				// Initial state.
	) const = 0;
	//
	// Enters info for skeleton into physics system at i_index.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void GetData
	( 
		int model,										// Physics system object index.
		float loc[7],									// Object world position and orientation.
		float points[iMAX_SKELETAL_ELEMENTS][3],		// Joint world positions. 
		float matrices[iMAX_SKELETAL_ELEMENTS][3][3],	// Joint world orientations.
		int Am_I_Supported[iMAX_SKELETAL_ELEMENTS]		// Non-zero if that joint is updated by the physics system.
	) const = 0;
	//
	// Provides data from the core physics system.
	//
	//**************************

	//*****************************************************************************************
	//
	void ActivateBoundaryBoxes
	(
		int i_index, 
		CAnimate* pani
	) const;
	//
	//	Adds all bio boundary condition boxes.
	//
	//*****************

private:

	// Disable copy constructor. Use pphiCopy() instead.
	CPhysicsInfoSkeleton(const CPhysicsInfoSkeleton& phib);
};

//**********************************************************************************************
//
class CPhysicsInfoBiped : public CPhysicsInfoSkeleton
//
// Prefix: phir
//
// Class holding shared physics skeleton info.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	//  Constructor.
	//

	CPhysicsInfoBiped(const CPhysicsInfoBox& phib)
		: CPhysicsInfoSkeleton(phib)
	{
	}

	//*****************************************************************************************
	//
	//  Overrides.
	//

	//*****************************************************************************************
	virtual float fDamageMultiplier
	(
		const CInstance* pins = 0,
		TSoundMaterial tsmat = 0
	) const override;

	//*****************************************************************************************
	virtual float fArmourMultiplier
	(
		const CInstance* pins = 0,
		TSoundMaterial tsmat = 0
	) const override;

	//*****************************************************************************************
	virtual void Init(CAnimate* pani) const override;

	//*****************************************************************************************
	virtual void HandleMessage
	(
		const CMessagePhysicsReq& msgpr,
		CInstance *pins
	) const override;

	//*****************************************************************************************
	virtual void DrawPhysics
	(
		CInstance* pins, 		// Owning instance.
		CDraw& draw,			// Line draw object.
		CCamera& cam			// Current view camera.
	) const override;

	//*****************************************************************************************
	virtual CVector3<> v3GetHeadPosition
	(
		const CInstance* pins
	) const override;

	//*****************************************************************************************
	virtual   CVector3<> v3GetTailPosition
	(
		const CInstance* pins
	) const override;

protected:

	//*****************************************************************************************
	virtual void CreatePhysics(CInstance* pins, int i_index, float aaf_state[7][3]) const override;

	//*****************************************************************************************
	virtual void GetData
	( 
		int model,
		float loc[7],
		float points[iMAX_SKELETAL_ELEMENTS][3], float matrices[iMAX_SKELETAL_ELEMENTS][3][3],
		int Am_I_Supported[iMAX_SKELETAL_ELEMENTS]
	) const override;
};

//**********************************************************************************************
//
class CPhysicsInfoQuadruped: public CPhysicsInfoBiped
//
// Prefix: phiq
//
// Extend biped model with 2 front legs.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	//  Constructor.
	//

	CPhysicsInfoQuadruped(const CPhysicsInfoBox& phib)
		: CPhysicsInfoBiped(phib)
	{
	}

	//*****************************************************************************************
	//
	//  Overrides.
	//

	//*****************************************************************************************
	virtual float fDamageMultiplier
	(
		const CInstance* pins = 0,
		TSoundMaterial tsmat = 0
	) const override;

	//*****************************************************************************************
	virtual float fArmourMultiplier
	(
		const CInstance* pins = 0,
		TSoundMaterial tsmat = 0
	) const override;

	//*****************************************************************************************
	virtual void HandleMessage
	(
		const CMessagePhysicsReq& msgpr,
		CInstance *pins
	) const override;

	//*****************************************************************************************
	virtual void Init(CAnimate* pani) const override;

	//*****************************************************************************************
	virtual void DrawPhysics
	(
		CInstance* pins, 		// Owning instance.
		CDraw& draw,			// Line draw object.
		CCamera& cam			// Current view camera.
	) const override;

protected:

	//*****************************************************************************************
	virtual void CreatePhysics(CInstance* pins, int i_index, float aaf_state[7][3]) const override;

	//*****************************************************************************************
	virtual void GetData
	( 
		int model,
		float loc[7],
		float points[iMAX_SKELETAL_ELEMENTS][3], float matrices[iMAX_SKELETAL_ELEMENTS][3][3],
		int Am_I_Supported[iMAX_SKELETAL_ELEMENTS]
	) const override;
};

//**********************************************************************************************
//
class CPhysicsInfoHuman : public CPhysicsInfoSkeleton
//
// Prefix: phih
//
// Class holding shared physics skeleton info.
//
//**************************************
{
public:

	
	//*****************************************************************************************
	//
	//  Constructor.
	//

	CPhysicsInfoHuman(const CPhysicsInfoBox& phib)
		: CPhysicsInfoSkeleton(phib)
	{
	}

	//*****************************************************************************************
	//
	virtual void HandleMessage
	(
		const CMessagePhysicsReq& msgpr,
		CInstance *pins
	) const override;
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void UpdateWDBase
	(
		CInstance *	pins,		// The instance to update
		int			i_index		// The index of that instance in the appropriate physics
								// system array.
	) const override;
	//
	//**************************

	//*****************************************************************************************
	virtual float fDamageMultiplier
	(
		const CInstance* pins = 0,
		TSoundMaterial tsmat = 0
	) const override;

	//*****************************************************************************************
	virtual float fArmourMultiplier
	(
		const CInstance* pins = 0,
		TSoundMaterial tsmat = 0
	) const override;

	//*****************************************************************************************
	virtual void Init(CAnimate* pani) const override;

	//*****************************************************************************************
	virtual void DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const override;

protected:
	//*****************************************************************************************
	virtual void CreatePhysics(CInstance* pins, int i_index, float aaf_state[7][3]) const override;

	//*****************************************************************************************
	//
	virtual void GetData
	( 
		int model,
		float loc[7],
		float points[iMAX_SKELETAL_ELEMENTS][3], float matrices[iMAX_SKELETAL_ELEMENTS][3][3],
		int Am_I_Supported[iMAX_SKELETAL_ELEMENTS] 
	) const override;
};

#endif
