/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	CAction
 *		CVoiceOverAction
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Trigger/Action.hpp                                               $
 * 
 * 62    9/21/98 1:03a Agrant
 * enum includes water disturbance trigger
 * 
 * 61    9/18/98 2:03a Rwyatt
 * Text Action has a time period.
 * Texr Action default colour is now white
 * 
 * 60    9/17/98 3:52p Rwyatt
 * New action for text overlay
 * 
 * 59    9/10/98 12:03a Agrant
 * added player comtrol action and set ai system action
 * 
 * 58    98.09.03 5:03p Mmouni
 * Added end game action.
 * 
 * 57    9/02/98 2:22p Rwyatt
 * Fixed effect action and added loop support
 * 
 * 56    9/01/98 2:55p Rwyatt
 * Sound effect action can now attach to instances
 * 
 * 55    8/25/98 11:48a Rvande
 * Removed redundant scoping
 * 
 * 54    8/19/98 5:41p Mlange
 * Added substitute AI action.
 * 
 * 53    8/06/98 7:00p Mlange
 * Updated terrain actions.
 * 
 * 52    8/05/98 3:16p Mlange
 * Updated for changes to anim script manager interface.
 * 
 * 51    8/05/98 12:03p Rwyatt
 * Added EAX action
 * 
 * 50    98.08.04 9:38p Mmouni
 * Added "set variable trigger" action.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRIGGER_ACTION_HPP
#define HEADER_LIB_TRIGGER_ACTION_HPP

#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/View/Colour.hpp"
#include "Lib/Audio/SoundDefs.hpp"


//*********************************************************************************************
//
class CTrigger;




//*********************************************************************************************
// enums for types of actions
//
// NOTE: THIS ENUM MUST MATCH THE IDS OF THE ACTION TYPES IN THE TEXT PROPS
//
enum EActionType
// prefix: ea
//**************************************
{
	eaACTION_VOICEOVER = 0,					//0
	eaACTION_AMBIENT,						//1
	eaACTION_MUSIC,							//2
	eaACTION_FADE_MUSIC,					//3
	eaACTION_SHOW_OVERLAY,					//4
	eaACTION_SET_FOG,						//5
	eaACTION_SET_RENDERER,					//6
	eaACTION_SET_TERRAIN,					//7
	eaACTION_SET_IMAGECACHE,				//8
	eaACTION_SET_AI,						//9
	eaACTION_SET_PHYSICS,					//10
	eaACTION_SUBSTITUTE_MESH,				//11
	eaACTION_SET_DEPTHSORT,					//12
	eaACTION_SET_SKY,						//13
	eaACTION_SET_ALPHA_WATER,				//14
	eaACTION_ENABLE_WATER,					//15
	eaACTION_LOAD_LEVEL,					//16
	eaACTION_SET_ANIMATE_PROPERTIES,		//17
	eaACTION_TELEPORT,						//18
	eaACTION_SAVE_LEVEL,					//19
	eaACTION_MAGNET,						//20
	eaACTION_SET_ANIMATE_TEXTURE,			//21
	eaACTION_HIDESHOW,						//22
	eaACTION_SOUND_EFFECT,					//23
	eaACTION_WAKE_AI,						//24
	eaACTION_DELAY,							//25
	eaACTION_SCRIPTED_ANIMATION,			//26
	eaACTION_SET_VARIABLE_TRIGGER,			//27
	eaACTION_SET_HINT,						//28
	eaACTION_AUDIO_ENVIRONMENT,				//29
	eaACTION_SUBSTITUTE_AI,					//30
	eaACTION_END_GAME,						//31
	eaACTION_CONTROL_PLAYER,				//32
	eaACTION_AI_SYSTEM,						//33
	eaACTION_TEXT,							//34
	eaACTION_WATER_DISTURBANCE,				//35
	eaEND
};



//*********************************************************************************************
class CAction : public CRefObj
// Prefix: ac
//**************************************
{
public:
	//*****************************************************************************************
	//
	virtual ~CAction()
	{
	}


	//*****************************************************************************************
	// Start is pure because there is no default start action that can be provided.
	//
	virtual void Start() = 0;	// Start the action.


	//*****************************************************************************************
	// Default stop action is to do nothing, most actions canot be stopped one they have been
	// started because critical game infomration may be missed. If the particular action can
	// be stopped then override this function.
	//
	virtual void Stop()
	{
	}


	//*****************************************************************************************
	// Get the time that an action stakes to perform. By default this returns zero. Override 
	// this function to implement something different.
	// This memeber is called by a trigger for the sequenced actions.
	virtual TSec sGetActionTime()
	{
		return TSec(0.0f);
	}

	//*****************************************************************************************
	// Some actions have local state that needs to be saved and restored. This trigger base
	// class will iterate through all the actions that it contains and call its load and
	// save functions.
	// By default these funcitons save nothing.
	virtual const char* pcLoad(const char* pc)
	{
		return pc;
	}

	//*****************************************************************************************
	virtual char* pcSave(char* pc) const
	{
		return pc;
	}

	//*****************************************************************************************
	//
	static void CreateTriggerAction
	(
		CTrigger*			ptr_trig,
		const CGroffObjectName*	pgon,			// Object to load.
		CObjectValue*		poval_action,		// Pointer to the actual action class
		CValueTable*		pvtable,			// Pointer to the value table.
		CLoadWorld*			pload				// the loader.
	);

	//*****************************************************************************************
	//
	static bool bValidateTriggerAction
	(
		const CGroffObjectName*	pgon,			// Object to load.
		CObjectValue*		poval_action,		// Pointer to the actual action class
		CValueTable*		pvtable,			// Pointer to the value table.
		CLoadWorld*			pload				// the loader.
	);
};





//*********************************************************************************************
class CVoiceOverAction : public CAction
// Prefix: vo
// A voice over action.
//**************************************
{
public:
	CVoiceOverAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;

protected:
	// No data needs to be saved...
	TSoundHandle	sndhndVoiceSample;
	float			fVolume;
	float			fAtten;
	float			fOutsideVolume;
	float			fFrustumAngle;
	uint32			u4SpatialType;
	CInstance*		pinsLocation;
};
	



//*********************************************************************************************
class CAmbientAction : public CAction
// Prefix: am
// an ambient sound
//**************************************
{
public:
	CAmbientAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
	// override the action time function and return the time in seconds that the voiceover
	// takes to play.
	virtual TSec sGetActionTime() override
	{
		return sActionLength;
	}

protected:

	// Protected constructor to copy itself and set the snd handle
	CAmbientAction
	(
		CAmbientAction&		ambact,
		TSoundHandle		sndhnd
	);

	// No data needs to be saved
	TSoundHandle	sndhndAmbientSample;
	float			fVolume;
	float			fAtten;
	float			fOutsideVolume;
	float			fFrustumAngle;
	uint32			u4SpatialType;
	CInstance*		pinsLocation;
	bool			bLooped;
	int32			i4LoopCount;
	float			fMaxDistance;
	float			fMaxVolDistance;
	TSec			sActionLength;
	float			fMasterVolumeMin;
	float			fMasterVolumeMax;
	bool			bMute;
};




//*********************************************************************************************
class CMusicAction : public CAction
// Prefix: mus
// stream a piece of music
//**************************************
{
public:
	CMusicAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No action needs to be saved
	TSoundHandle	sndhndMusicSample;
	float			fVolume;
	float			fAtten;
	float			fOutsideVolume;
	float			fFrustumAngle;
	uint32			u4SpatialType;
	CInstance*		pinsLocation;
};



//*********************************************************************************************
class CFadeMusicAction : public CAction
// Prefix: fmus
// fade the current piece of music
//**************************************
{
public:
	CFadeMusicAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No data needs to be saved
	float	fDeltaVolume;
	bool	bStop;
};



//*********************************************************************************************
class CShowOverlayAction : public CAction
// Prefix: sol
// overlay a 2D decal
//**************************************
{
public:
	CShowOverlayAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No data needs to be saved
	CEasyString		estrBitmap;
	bool			bCenter;
	bool			bDiscard;
	int32			i4XPos;
	int32			i4YPos;
};



//*********************************************************************************************
//
#define ACTION_FOG_FLAG_SET_COLOUR		0x00000001
#define ACTION_FOG_FLAG_SET_TYPE		0x00000002
#define ACTION_FOG_FLAG_SET_POWER		0x00000004
#define ACTION_FOG_FLAG_SET_HALF		0x00000008
#define ACTION_FOG_FLAG_SET_LINEAR		0x00000010
#define ACTION_FOG_FLAG_SET_EXPONENTIAL	0x00000020

#define ACTION_FOG_TYPE_LINEAR			0
#define ACTION_FOG_TYPE_EXPO			1


//*********************************************************************************************
class CSetFogAction : public CAction
// Prefix: sfa
// set the fog colour and range
//**************************************
{
public:
	CSetFogAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;

protected:
	// No data needs to be saved
	uint32			u4SetFlags;
	CColour			clrFog;
	float			fFogPower;
	float			fFogHalf;
};



//*********************************************************************************************
//
#define ACTION_TERRAIN_SET_TrrPixelTol				0x00000001
#define ACTION_TERRAIN_SET_TrrPixelTolFar			0x00000002
#define ACTION_TERRAIN_SET_TrrNoShadowDist			0x00000004
#define ACTION_TERRAIN_SET_TrrNoTextureDist			0x00000008
#define ACTION_TERRAIN_SET_TrrNoDynTextureDist		0x00000010
#define ACTION_TERRAIN_SET_TrrMovingShadows			0x00000020


//*********************************************************************************************
class CSetTerrainAction : public CAction
// Prefix: sta
// set the terrain parameters
//**************************************
{
public:
	CSetTerrainAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No data needs to be saved
	uint32	u4SetFlags;
	float	fPixelTol;
	float	fPixelTolFar;
	float	fTrrNoShadowDist;	
	float	fTrrNoTextureDist;	
	float	fTrrNoDynTextureDist;	
	bool	bTrrMovingShadows;	
};



//*********************************************************************************************
//
#define ACTION_SORT_SET_NearTolerance		0x00000001
#define ACTION_SORT_SET_FarTolerance		0x00000002
#define ACTION_SORT_SET_NearZ				0x00000004
#define ACTION_SORT_SET_FarZ				0x00000008
#define ACTION_SORT_SET_FarZNo				0x00000010
#define ACTION_SORT_SET_SortPixelTol		0x00000020
#define ACTION_SORT_SET_MaxNumToSort		0x00000040
#define ACTION_SORT_SET_Sort2PartAt			0x00000080
#define ACTION_SORT_SET_Sort4PartAt			0x00000100
#define ACTION_SORT_SET_TerrNearTolerance	0x00000200
#define ACTION_SORT_SET_TerrFarTolerance	0x00000400
#define ACTION_SORT_SET_TerrNearZ			0x00000800
#define ACTION_SORT_SET_TerrFarZ			0x00001000
#define ACTION_SORT_SET_UseSeperateTol		0x00002000


//*********************************************************************************************
class CSetSortAction : public CAction
// Prefix: ssa
// set the depth sort parameters
//**************************************
{
public:
	CSetSortAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No data needs to be saved
	uint32	u4SetFlags;
	float	fNearTolerance;
	float	fFarTolerance;
	float	fNearZ;
	float	fFarZ;
	float	fFarZNo;
	float	fSortPixelTol;
	int		iMaxNumToSort;
	int		iSort2PartAt;
	int		iSort4PartAt;
	float	fTerrNearTolerance;
	float	fTerrFarTolerance;
	float	fTerrNearZ;
	float	fTerrFarZ;
	bool	bUseSeperateTol;
};



//*********************************************************************************************
//
#define ACTION_RENDERER_SET_AMBIENT				0x00000001
#define ACTION_RENDERER_SET_CAMERA_FOV			0x00000002
#define ACTION_RENDERER_SET_FAR_CLIP			0x00000004
#define ACTION_RENDERER_SET_NEAR_CLIP			0x00000008

// Distance culling macros.
#define ACTION_RENDERER_SET_CULLMAXDIST          (0x00000010)
#define ACTION_RENDERER_SET_CULLMAXRADIUS        (0x00000020)
#define ACTION_RENDERER_SET_CULLMAXDISTSHADOW    (0x00000040)
#define ACTION_RENDERER_SET_CULLMAXRADIUSSHADOW  (0x00000080)

// Perspective correction
#define ACTION_RENDERER_SET_PixelError			0x00000100
#define ACTION_RENDERER_SET_SubdivisionLen		0x00000200
#define ACTION_RENDERER_SET_AltPixelError		0x00000400
#define ACTION_RENDERER_SET_AltSubdivisionLen	0x00000800


//*********************************************************************************************
class CSetRendererAction : public CAction
// Prefix: sra
// set the renderer parameters
//**************************************
{
public:
	CSetRendererAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No data needs to be saved
	uint32	u4SetFlags;

	// Lighting & camera settings.
	float	fAmbientLight;
	float   fCameraFOV;
	float	fNearClipPlane;
	float	fFarClipPlane;

	// Distance culling stuff.
	float	fCullMaxDist;
	float	fCullMaxRadius;
	float	fCullMaxDistShadow;
	float	fCullMaxRadiusShadow;

	// Perspective correction settings.
	float	fPixelError;
	int		iSubdivisionLen;
	float	fAltPixelError;
	int		iAltSubdivisionLen;
};


//*********************************************************************************************
#define ACTION_IMGCACHE_SET_PIXEL_RATIO		0x00000001
#define ACTION_IMGCACHE_SET_MIN_PIXELS		0x00000002
#define ACTION_IMGCACHE_SET_CACHE_AGE		0x00000004
#define ACTION_IMGCACHE_SET_CACHE_ON		0x00000008
#define ACTION_IMGCACHE_SET_CACHE_OFF		0x00000010
#define ACTION_IMGCACHE_SET_INTERSECT_ON	0x00000020
#define ACTION_IMGCACHE_SET_INTERSECT_OFF	0x00000040


//*********************************************************************************************
class CSetImageCacheAction : public CAction
// Prefix: sica
// set the image cache parameters
//**************************************
{
public:
	CSetImageCacheAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No data needs to be saved
	uint32	u4SetFlags;
	float	fPixelRatio;
	int32	i4MinPixels;
	int32	i4CacheAge;
};



//*********************************************************************************************
#define ACTION_SKY_SET_SCALE			0x00000001
#define ACTION_SKY_SET_HEIGHT			0x00000002
#define ACTION_SKY_SET_NEAR_FOG			0x00000004
#define ACTION_SKY_SET_FAR_FOG			0x00000008
#define ACTION_SKY_SET_WIND_SPEED_X		0x00000010
#define ACTION_SKY_SET_WIND_SPEED_Y		0x00000020
#define ACTION_SKY_SET_SUBDIVISIONS		0x00000040
#define ACTION_SKY_SET_TEXTURE_ON		0x00000080
#define ACTION_SKY_SET_TEXTURE_OFF		0x00000100
#define ACTION_SKY_SET_FILLSCREEN_ON	0x00000200
#define ACTION_SKY_SET_FILLSCREEN_OFF	0x00000400

//*********************************************************************************************
class CSetSkyAction : public CAction
// Prefix: skya
// set the sky parameters
//**************************************
{
public:
	CSetSkyAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No data needs to be saved
	uint32	u4SetFlags;

	float	fScale;
	float	fHeight;
	float	fNearFog;
	float	fFarFog;
	float	fWindX;
	float	fWindY;

	int32	i4Division;
};


//*********************************************************************************************
#define ACTION_WATER_SET_ALHPA_VALUES		0x00000001
#define ACTION_WATER_SET_NONALHPA_VALUES	0x00000002

//*********************************************************************************************
class CSetAlphaWaterAction : public CAction
// Prefix: waa
// set the water alpha setttings.
//**************************************
{
public:
	CSetAlphaWaterAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No data needs to be saved
	uint32	u4SetFlags;

	CColour	clrColour[16];			// Colour values.
	float	fAlpha[16];				// Alpha values.

	CColour	clrAltColour[32];		// Colour values.
	float	fAltAlpha[32];			// Alpha values.
};


//*********************************************************************************************
class CWaterOnOffAction : public CAction
// Prefix: woo
// set the water alpha setttings.
//**************************************
{
public:
	CWaterOnOffAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No data needs to be saved
	uint32	u4Handle;		// Hash of the name of the water object.
	bool	bEnable;		// Enable or disable flag.
};


//*********************************************************************************************
class CLoadLevelAction : public CAction
//
// Prefix: lla
//
// Load a new level.
//**************************************
{
public:
	CLoadLevelAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No data needs to be saved
	CEasyString estrLevelName;						// Name of the level to load.
};


//*********************************************************************************************
class CSaveLevelAction : public CAction
//
// Prefix: lla
//
// Load a new level.
//**************************************
{
public:
	CSaveLevelAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;
	
protected:
	// No data needs to be saved
	CEasyString estrLevelName;						// Name of the level to save.
};


//*********************************************************************************************
#define ACTION_ANIMATE_PROP_SET_HITPOINTS			0x00000001
#define ACTION_ANIMATE_PROP_SET_MAX_HITPOINTS		0x00000002
#define ACTION_ANIMATE_PROP_SET_REGENERATION_RATE	0x00000004
#define ACTION_ANIMATE_PROP_SET_REALLY_DEAD			0x00000008
#define ACTION_ANIMATE_PROP_SET_DIE_RATE			0x00000010
#define ACTION_ANIMATE_PROP_SET_CRITICAL_HIT		0x00000020
#define ACTION_ANIMATE_PROP_SET_DAMAGE				0x00000040


//*********************************************************************************************
//
class CSetAnimatePropertiesAction : public CAction
//
// Prefix: sapa
//
// Set properties on a target CAnimate.
//
//**************************************
{
public:
	CSetAnimatePropertiesAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;

protected:
	// No data needs to be saved
	uint32	u4Handle;			// Hash of the name of the CAnimate.

	uint32	u4SetFlags;

	float fHitPoints;			// Number of hit points an animate creature can sustain.  At zero, creature stops being a threat.
	float fMaxHitPoints;		// Number of hit points a critter has when at full.
	float fRegenerationRate;	// Number of hit points a creature regains per second.
	float fReallyDead;			// Hit point level at which we actually realy and truly die.
	float fDieRate;				// Hit points lost each second that we are below zero.
	float fCriticalHit;			// More than this amount of hit points constitutes a critical hit on this guy.
	float fDamage;				// A hit point delta, applied as if the critter was actually hit.
};



//*********************************************************************************************
//
class CTeleportAction : public CAction
//
// Prefix: ta
//
// Teleport an instance.
//
//**************************************
{
public:
	CTeleportAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;

protected:
	// No data needs to be saved
	uint32 u4Handle;			// Hash of the name of the CInstance.

	CPlacement3<> p3Destination;// Target destination and orientation of teleport.

	bool bHeightRelative;		// Whether the Z component of the destination is specified relative to the highest
								// physics object and terrain height in the region.
	bool bOnTerrain;			// 

	bool bSetPosition;			// Whether the position must be set on teleport.
	bool bSetOrientation;		// Whether the orientation must be set on teleport.
};



//*********************************************************************************************
//
class CSoundEffectAction : public CAction
//
// Prefix: sfa
//
// Sound effect action...
//
//**************************************
{
public:
	CSoundEffectAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;

protected:
	// No data needs to be saved
	TSoundHandle	sndhndEffectSample;
	uint32			u4Handle;
	CVector3<>		v3Location;
	float			fVolume;
	float			fAtten;
	bool			bAttach;
	float			fFrustum;
	float			fOutsideVol;
	bool			bLoop;
};



class CAnimationScript;

//*********************************************************************************************
//
class CScriptedAnimationAction : public CAction
//
// Prefix: saa
//
// Control scripted animations.
//
//**************************************
{
public:
	CScriptedAnimationAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual ~CScriptedAnimationAction();

	virtual void Start() override;

protected:
	// No data needs to be saved
	CAnimationScript* pansAnim;
};


//*********************************************************************************************
//
class CDelayAction : public CAction
//
// Prefix: sfa
//
// Sound effect action...
//
//**************************************
{
public:
	CDelayAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	//*****************************************************************************************
	// The start function of this action does nothing at all
	virtual void Start() override
	{
	}

	//*****************************************************************************************
	// Return the preset delay
	virtual TSec sGetActionTime() override
	{
		return sDelay;
	}

protected:
	// No data needs to be saved
	TSec	sDelay;
};



class CVariableTrigger;

//*********************************************************************************************
//
class CSetVariableTriggerAction : public CAction
//
// Prefix: svt
//
// Sets (true or false) or toggles the value of a variable trigger.
//
//**************************************
{
public:
	CSetVariableTriggerAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	virtual void Start() override;

protected:
	uint32	u4Handle;								// Trigger to affect.
	bool	bToggle;								// Toggle or set?
	bool	bValue;									// Value to set.
};

//*********************************************************************************************
//
class CAudioEnvironmentAction : public CAction
//
// Prefix: aea
//
// EAX audio environment action
//
//**************************************
{
public:
	CAudioEnvironmentAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);


	//*****************************************************************************************
	// The start function of this action does nothing at all
	virtual void Start() override;


protected:
	int32	i4Environment;
	float	fVolume;
	float	fDecay;
	float	fDamping;
};



//*********************************************************************************************
//
class CTextAction : public CAction
//
// Prefix: aea
//
// EAX audio environment action
//
//**************************************
{
public:
	CTextAction
	(
		CTrigger*				ptr_trig,
		const CGroffObjectName*	pgon,				// Object to load.
		CObjectValue*			poval_action,		// Pointer to the actual action class
		CValueTable*			pvtable,			// Pointer to the value table.
		CLoadWorld*				pload				// the loader.
	);

	//*****************************************************************************************
	virtual ~CTextAction()
	{
		delete strRawData;
	}

	//*****************************************************************************************
	// The start function of this action does nothing at all
	virtual void Start() override;

	//*****************************************************************************************
	// The length of a text action is the display period
	virtual TSec sGetActionTime() override
	{
		return TSec(fTime);
	}


protected:
	uint32			u4Flags;
	float			fTime;
	CColour			clrText;
	char*			strRawData;
	int32			i4ResID;			// resource ID
};



#endif // HEADER_LIB_TRIGGER_ACTION_HPP
