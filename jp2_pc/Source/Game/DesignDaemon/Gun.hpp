/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/Gun.hpp                                             $
 * 
 * 28    9/17/98 10:47p Pkeet
 * Put in a fix for guns disappearing when muzzle flashes appear.
 * 
 * 27    98/08/17 17:17 Speter
 * CGun now passed to pinsFire, and to collision message.
 * 
 * 26    8/11/98 8:24p Mlange
 * Entities must now register themselves with each message type in which they are interested.
 * 
 * 25    98/08/05 20:41 Speter
 * Changed Use() to bUse(). 
 * 
 * 24    7/29/98 3:12p Agrant
 * added tranq damage
 * 
 * 23    98.07.24 1:46p Mmouni
 * Added approximate ammo counting and pickup counts.
 * 
 * 22    7/21/98 4:53p Agrant
 * Gunfire now alerts wakeful animals within rWakeUp distance
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_GUN_HPP
#define HEADER_LIB_GEOMDBASE_GUN_HPP

#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Sys/Timer.hpp"
#include "Lib/Audio/SoundDefs.hpp"
#include <list>

//
// Class definitions.
//
class CMuzzleFlash;
class CGun;

//*********************************************************************************************
//
class CGunData
//
// Data for a gun object.
//
// Prefix: gundata
//
//**************************************
{
public:
	float fExtension;
	float fImpulse;
	float fExtensionPull;
	float fImpulsePull;
	float fDamage;			// Damage caused by the gun, in hit points.
	float fTranqDamage;		// Damage caused by the gun, in tranq points.

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CGunData();

	//*********************************************************************************************
	//
	CInstance* pinsFire
	(
		CGun* pgun,							// The gun firing the shot.
		const CPresence3<>& pr3_gun,		// Presence of the gun.
		bool b_pull,						// If 'true' apply a negative impulse.
		const CInstance* pins_immune = 0	// An optional instance that is invisible to the gunshot (like the CGun object)
	);
	//
	// Command to fire the global gun.
	//
	//	Returns:
	//		Pointer to the struck instance, or NULL if none.
	//
	//**************************************
};


//*********************************************************************************************
//
class CGun : public CEntity
//
// The actual gun 
//
// Prefix: gundata
//
//**************************************
{
public:
	CGunData		gdGunData;			// Gun specs.
	int				iMaxAmmo;			// How much ammo the gun could hold, were it full of bullets.
	int				iAmmo;				// Remaining ammo.
	bool			bAutoFire;			// Is this an autofire weapon (used for sound effects)
	bool			bAltAmmoCount;		// Use alternate ammo counting (for full auto and burst weapons).
	CVector3<>		v3BarrelOffset;		// Barrel offset from center in local space.
	TSec			sTimeBetweenShots;	// The amount of time between successive shots, in seconds.
	TSec			sCanShoot;			// The next time the gun can shoot.
	TReal			rWakeUp;			// Distance at which dinos "hear" the gunshot.
	TSoundHandle	sndhndGunSample;	// sound handle for the gun fire sample
	TSoundHandle	sndhndEmptySample;	// sound handle for the empty clip sample.
	TSoundHandle	sndhndRingSample;	// sound handle of the release sample after a loop
	TSoundHandle	sndhndLastSample;	// sound handle of the sample that we played last

	// Recoil and muzzle flash info
	CVector3<>		v3RecoilOffset;	// Offset from origin in local space, where the recoil impulse is applied
	CVector3<>		v3RecoilDir;	// Direction vector of the recoil
	float			fRecoilForce;	// force of the recoil, if zero there is no recoil

	CVector3<>		v3FlashOffset;	// Offset from origin in local space, where the muzzle flash object appears
	TSec			sFlashDuration;	// Duration in seconds of the flash, default to 0.0 for a single frame
	TSec			sFlashRemove;	// time after which the flash should be removed
	bool			bRandomRotate;	// set to true to randomly rotate flash mesh around the Y axis
	uint32			u4MeshCount;	// Number of flash meshes that this gun contains, if zero no muzzle flash
	uint32			u4NextMesh;		// Index of the next mesh to use
	CMuzzleFlash*	pmuzflMeshes[iMAX_MUZZLE_FLASH];	// A gun can have a maximum of 10 meshes attached to it.
	CMuzzleFlash*	pmuzflCurrentFlash;


	//*****************************************************************************************
	//
	// Constructors.
	//
	CGun();

	CGun
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CGun();

	//
	//	Overrides.
	//

	//*****************************************************************************************
	virtual bool bUse(bool b_repeat);

	//*****************************************************************************************
	virtual void PickedUp();

	//*****************************************************************************************
	virtual void Process(const CMessageMove& msg);

	//*****************************************************************************************
	virtual void Process(const CMessageStep& msg);

	//*****************************************************************************************
	virtual CInstance* pinsCopy() const;

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

	//*****************************************************************************************
	virtual void Cast(CGun** ppgun)
	{
		*ppgun = this;
	}

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer);

	//*****************************************************************************************
	// Return true if all referenced names are loaded, otherwise return false to prevent the
	// current object being loaded until the next pass.
	static bool bValidateGunProperties
	(
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		CLoadWorld*				pload		// The loader.
	);
};


//*********************************************************************************************
//
typedef std::list<CMuzzleFlash*> TMuzzleFlashList;
// prefix: mf
//**************************************

//*********************************************************************************************
//
class CMuzzleFlash : public CInstance
//
// The muzzle flash
//
// Prefix: muzfl
//
//**************************************
{
public:
	CMuzzleFlash
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
	);

	~CMuzzleFlash();

	bool	bVisible;

	static CMuzzleFlash* pmuzflFindNamedMuzzleFlash(const char* str);
	static void ClearMuzzleFlashList();
	static void ResetMuzzleFlash();

	//*****************************************************************************************
	virtual bool bCanHaveChildren();

//	CInstance* pinsCopy() const;

protected:
	static TMuzzleFlashList		mfMeshList;
};



//
// Global variables.
//

// Global gun variable.
extern CGunData gdGlobalGunData;


#endif // HEADER_LIB_GEOMDBASE_GUN_HPP
