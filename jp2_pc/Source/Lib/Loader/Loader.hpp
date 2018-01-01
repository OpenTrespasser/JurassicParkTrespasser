/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Groff loader for the GUIApp.
 *
 * Bugs:
 *
 * To do:
 *		Replace this module with a more permanent solution.
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/Loader.hpp                                                $
 * 
 * 174   9/17/98 3:52p Rwyatt
 * Removed GRF load filename from the audio system
 * Added text props for the text action
 * 
 * 173   9/11/98 12:38p Agrant
 * new symbols
 * 
 * 172   9/08/98 10:24p Pkeet
 * Added a text property for dust.
 * 
 * 171   98.09.07 5:25p Mmouni
 * Changed pmshLoadObject to prdtLoadObject and changed instancing to use CRenderType instead
 * of CMesh.
 * 
 * 
 * 170   9/05/98 11:10p Agrant
 * rotation ratio symbol
 * 
 * 169   9/05/98 4:39p Agrant
 * terrain pathfinding, minor activity changes
 * 
 * 168   9/04/98 4:35p Agrant
 * added the floats text prop
 * 
 * 167   9/03/98 4:30p Shernd
 * Checking for out of disk space
 * 
 * 166   9/01/98 2:54p Rwyatt
 * Text props for sound effect action
 * 
 * 165   8/31/98 1:05p Mmouni
 * Added esAmmoReallyFull text prop.
 * 
 * 164   8/28/98 6:09p Agrant
 * Query symbol
 * 
 * 163   8/26/98 3:19a Agrant
 * gender text prop for sockets
 * 
 * 162   8/23/98 3:04a Agrant
 * new text prop
 * 
 * 161   98/08/22 21:43 Speter
 * Added translational magnet params.
 * 
 * 160   98/08/21 18:02 Speter
 * Text props for particles.
 * 
 * 159   8/20/98 5:35p Mmouni
 * Added new text props for clut control stuff.
 * 
 * 158   8/20/98 12:59p Agrant
 * More symbols
 * 
 * 157   8/18/98 10:01p Agrant
 * impulse text prop
 * 
 * 156   8/18/98 6:48p Mlange
 * Added 'Volume' text prop.
 * 
 * 155   8/18/98 6:09p Pkeet
 * Added text properties for hardware flags.
 * 
 * 154   8/12/98 5:37p Agrant
 * added esFall text prop
 * 
 * 153   98.08.10 4:18p Mmouni
 * Added esAnimSubMaterial text prop.
 * 
 * 152   8/06/98 7:00p Mlange
 * Updated terrain actions.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_LOADER_LOADER_HPP
#define HEADER_GUIAPP_LOADER_LOADER_HPP


//
// Includes.
//
#include "Lib/Sys/Textout.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Groff/ObjectHandle.hpp"
#include "Lib/Groff/ValueTable.hpp"
#include "Game/AI/AITypes.hpp"


#define pinsINVALID		((CInstance*)1)

#define iMAX_ANIMATION_FRAMES	30
#define iMAX_VERTICES			512
#define iMAX_MODELS				12
#define iMAX_A					200
#define iMAX_ACTIONS			100
#define iMAX_ACTIVITIES			55
#define iMAX_MUZZLE_FLASH		10		// Number of muzzle flashes per gun.
#define iMAX_SUBMATERIALS		20		// Maximum number of sub-materials.


#ifndef PFNWORLDLOADNOTIFY
typedef uint32 (__stdcall * PFNWORLDLOADNOTIFY)(uint32 dwContext, uint32 dwParam1, uint32 dwParam2, uint32 dwParam3);
#endif

//**********************************************************************************************
//
enum ESymbol
//
// Prefix: es
//
//	Each value here coresponds with a matching symbol string.
//	The strings are case-sensitive.  Make the text match exactly, so we don't have to 
//	look it up.
//
{
	esClass	 = 0,		// "Class"
	esMesh,
	esPhysics,

	esMin,
	esMax,

	esInitialState,
	esMinHighTime,
	esMaxHighTime,
	esMinLowTime,
	esMaxLowTime,

	esClose,
	esDecay,
	esToggle,	       //"Toggle"
	esSpangCount,
	esArmour,
	esDamageMultiplier,
	esDamagePerSecond,
	esMass,
	esDensity,
	esElasticity,
	esFile,
	esFloats,
	esFriction,
	esFrozen,
	esImpulse,
	esLocation,
	esMaterial,
	esMoveable,
	esOpen,
	esPlanted,
	esRatio,
	esRotationRatio,
	esReset,
	esShadow,
	esSmall,
	esSoundMaterial,
	esSubstitute,
	esTangible,
	esTarget,
	esType,
	esValue,
	esHintID,
	esVolume,

	// Some animal combat stuff.
	esCriticalHit,
	esHitPoints,
	esMaxHitPoints,
	esRegeneration,
	esReallyDie,
	esDieRate,
	esOuch,
	esFall,

	//
	// Distance Culling Parameters.
	//
	esCulling,					// float "Culling"
	esShadowCulling,			// float "ShadowCulling"
	esCullingMaxDist,			// float "CullMaxDist"
	esCullingMaxRadius,			// float "CullMaxRadius"
	esCullingMaxDistShadow,		// float "CullMaxDistShadow"
	esCullingMaxRadiusShadow,	// float "CullMaxRadiusShadow"
	esPriority,					// int "Priority"

	//
	// Rendering paramaters.
	//
	esCameraFOV,				// float "CameraFOV"
	esPixelError,
	esSubdivisionLen,
	esAltPixelError,
	esAltSubdivisionLen,

	//
	// Direct3D parameters.
	//
	esD3DHardwareOnly,			// Object will be rendered in hardware only.
	esD3DSoftwareOnly,			// Object will be rendered in software only.
	esD3DNoLowRes,				// Object will not allow textures to be rendered in low resolution.

	//
	// Object rendering parameters.
	//
	esAlwaysFace,				// bool  "AlwaysFace"

	esScale,
	esMinScale,
	esMaxScale,
	esScaleLimit,

	esFogNear,
	esFogFar,
	esWindSpeedX,
	esWindSpeedY,
	esSubDivision,
	esFillScreen,

	esElement1,
	esElement2,
	esSoundMaterial1,
	esSoundMaterial2,
	esMinVelocity,
	esMaxVelocity,

	// Creature trigger flags
	esCreatureDie,
	esCreatureWake,
	esCreatureSleep,
	esCreatureCriticalDamage,
	esCreatureDamagePoints,
	esEvaluateAll,

	esA00,				// Array entries
	esAPLACEHOLDER = esA00 + iMAX_A,	

	
	// Trigger base class elements
//	esCTrigger,			// "CTrigger"
	esTFireDelay,		// "FireDelay"
	esTFireCount,		// "FireCount"
	esTFireExpression,	// "FireExpression"
	esTProb,			// "Probability
	esTProcessStyle,	// "ProcessStyle"
	esTRepeatPeriod,	// "RepeatPeriod"
	esTFireZero,		// "FireAtZero"
	esTResetFire,		// "ResetFire"
	esTBoundVol,		// "BoundVol"
	esTSequenceDelayMin,
	esTSequenceDelayMax,

	esTMasterVolumeMin,
	esTMasterVolumeMax,

	esTAction00,		// "Action00"
	esTActionEND = esTAction00 + iMAX_ACTIONS - 1,

	// action elements
	esTActionType,		// "ActionType"
	esTActionSample,	// "Sample"
	esTActionSpatial,	// "SpatialType"
	esTActionVolume,	// "Volume"
	esTActionStopAfterFade, //"StopAfterFade"
	esTActionAtten,		// "Attenuation"
	esTActionBoundaryVol, //"BoundaryVolume"
	esTActionMaxVolDist,  //"MaxVolDistance"
	esTActionEmitter,	// "Emitter"
	esTActionFrustum,	// "Frustum"
	esTActionMute,		// Mute
	esTActionOutsideVol,// "OutsideVolume"
	esTActionSampleLoop,// "Looped"
	esTActionLoopCount,	// "LoopCount"
	esTActionMaxDist,	// "MaximumDistance"
	esTActionVolFade,	// "VolumeFader"
	esTActionAttach,	// "Attach"

	esTActionBitmap,	// "Bitmap"
	esTActionCenter,	// "Center"
	esTActionDiscard,	// "Discard"
	esTActionXP,		// "XPos"
	esTActionYP,		// "YPos"

	esTActionFogType,	// "FogType"
	esTActionFogPower,	// "FogPower"
	esTActionFogHalf,	// "FogHalf"

	esTActionAmbientLight,  // "AmbientLight"
	esTActionFarClipPlane,  // "FarClipPlane"
	esTActionNearClipPlane, // "NearClipPlane"

	// Terrain parameters.
	esTActionTrrPixelTol,			// "TrrPixelTol"
	esTActionTrrPixelTolFar,		// "TrrPixelTolFar"
	esTrrNoShadowDist,
	esTrrNoTextureDist,
	esTrrNoDynTextureDist,
	esTrrMovingShadows,

	// Depth Sort parameters.
	esTActionNearTolerance,		// "NearTolerance"
	esTActionFarTolerance,		// "FarTolerance"
	esTActionNearZ,				// "NearZ"
	esTActionFarZ,				// "FarZ"
	esTActionFarZNo,			// "FarZNo"
	esTActionSortPixelTol,		// "SortPixelTol"
	esTActionMaxNumToSort,		// "MaxNumToSort"
	esTActionSort2PartAt,		// "Sort2PartAt"
	esTActionSort4PartAt,		// "Sort4PartAt"
	esTActionTerrNearTolerance,	// "TerrNearTolerance"
	esTActionTerrFarTolerance,	// "TerrFarTolerance"
	esTActionTerrNearZ,			// "TerrNearZ"
	esTActionTerrFarZ,			// "TerrFarZ"
	esTActionUseSeperateTol,	// "UseSeperateTol"


	esUseObject,				// "UseObject"
	esPickUpObject,				// "PickUpObject"
	esPutDownObject,			// "PutDownObject"
	esTriggerName,				// "TriggerName"

	//
	// Image cache parameters.
	//
	esTActionPixelRatio,
	esTActionMinPixels,
	esTActionCacheAge,
	esTActionCacheActive,
	esTActionCacheIntersect,

	esNoCache,			// "NoCache"
	esNoCacheAlone,		// "NoCacheAlone"
	esCacheMultiplier,	// "CacheMul"


	//
	// Teleport trigger parameters.
	//
	esHeightRelative,
	esOnTerrain,
	esSetPosition,
	esSetOrientation,
	esTeleportDestObjectName,
	


	// specific trigger symbols
//	esCLocationTrigger,	// "CLocationTrigger"
	esLTEnterCount,		// "CreatureEnterCount"
	esLTLeaveCount,		// "CreatureLeaveCount"
	esLTPlayerIn,		// "PlayerIn"
	esLTPlayerLeave,	// "PlayerLeave"
	esLTPlayerEnter,	// "PlayerEnter"
	esLTObjectIn,		// "ObjectIn"
	esLTObjectLeave,	// "ObjectLeave"
	esLTObjectEnter,	// "ObjectEnter"
	esLTCreatureIn,		// "CreatureIn"
	esLTCreatureLeave,	// "CreatureLeave"
	esLTCreatureEnter,	// "CreatureEnter"
	esLTTriggerActivate,// "TriggerActivate"
	esLTPointTrigger,	// "PointTrigger"

	// Stuff need for game triggers
	esObjectName,
	esEnable,
	esQuery,
	esLevelName,
	esAnimationName,

	// For sequence trigger.
	esSequenceListenNames,
	esSequenceOrderNames,
	esSequenceEvalNowNames,
	esSequenceFalseTriggerName,

	// Coordinate symbols
	esX,
	esY,
	esZ,

	// Magnet symbols.
	esBreakable,
	esBreakStrength,
	esDelta,
	esDrive,
	esDelay,
	esAngleMin,
	esAngleMax,
	esCMagnet,
	esInstance,
	esGender,
	esMagnet,
	esMasterObject,
	esRestoreStrength,
	esSlaveMagnet,
	esSlaveObject,
	esXTFree,
	esYTFree,
	esZTFree,
	esXFree,
	esYFree,
	esZFree,
	esHandPickup,
	esShoulderHold,

	// Water object symbols.
	esResolutionRatio,

	// Water disturbance symbols.
	esStrength,
	esInterval,
	esRadius,

	// Animating texture symbols.
	esAnim00,	// Animating textures.
				// All id's in between are for successive frames.
	esAnimB00 = esAnim00 + iMAX_ANIMATION_FRAMES,			//  Animating bumpmaps.
	esPLACEHOLDER = esAnimB00 + iMAX_ANIMATION_FRAMES,
	esFrame,
	esFreezeFrame,
	esTrackTwo,
	esDeltaX,
	esDeltaY,
	esAnimSubMaterial,

	esAudioEnvironment,
	esReverbVolume,	
	esReverbDecay,	
	esReverbDamping,

	// Mesh symbols!
	esBumpiness,
	esBumpmaps,
	esCacheIntersecting,
	esDetail1,
	esDetail2,
	esDetail3,
	esDetail4,
	esDetailShadow,
	esDiffuse,
	esEmissive,
	esMerge,
	esNormals,
	esCurved,
	esNumDoubleJoints,
	esNumJoints,
	esPRadius,
	esPVA,
	esReflect,
	esRefract,
	esRefractIndex,
	esSpecular,
	esSpecularAngle,
	esSplit,
	esTexture,
	esVisible,
	esWrap,
	esUnlit,

	// Clut control.
	esClutStartColor,
	esClutStopColor,
	esClutReflectionColor,
	esDefaultClutStartColor,
	esDefaultClutStopColor,
	esDefaultClutReflectionColor,

	// Alpha water clut values.
	esAlphaWaterProperties,
	esNonAlphaWaterProperties,

	// Dino helpers   // MUST MATCH ebb ORDER IN Animate.hpp!
	esBody,
	esFoot,
	esHand,
	esHead,
	esTail,

	esLeftFoot,
	esRightFoot,
	esLeftRearFoot,
	esRightRearFoot,
	esNeck,

	esReverseKnees,

	// Sub-material objects.
	esSubMaterial01,
	esSubMaterialEnd = esSubMaterial01 + iMAX_SUBMATERIALS,

	// Alpha symbols
	esAlphaChannel,
	esR,
	esG,
	esB,
	esAlpha,

	// Sky Flat colour
	esSkyFlatColour,

	// Mip maps symbols
	esMipMap,

	// Light symbols
	esIntensity,

	// Joint assignment symbols.
	esVertex000,// Vertexes.
				// All id's in between are for successive vertices.
	esVERTEX_PLACEHOLDER = esVertex000 + iMAX_VERTICES,

	// Joint assignment symbols.
	esModel00,// Sub-models
				// All id's in between are for successive vertices.
	esMODEL_PLACEHOLDER = esModel00 + iMAX_MODELS,

	// Terrain object symbols
	esCTerrainObj,
	esHeight,

	// Gun symbols
	esAmmo,
	esMaxAmmo,
	esROF,
	esAutoFire,
	esAltAmmoCount,
	esAmmoPickup,
	esAmmoAlmostEmpty,
	esAmmoHalfFull,
	esAmmoFull,
	esAmmoReallyFull,
	esDamage,
	esTranqDamage,
	esRange,
	esKick,
	esPush,
	esBarrel,
	esRecoilOrg,
	esEmptyClipSample,
	esRingSample,
	esRecoilForce,
	esMFlashOrg,
	esMFlashDuration,
	esRandomRotate,
	esMFlashObject0,
	esMUZZLE_PLACEHOLDER = esMFlashObject0 + iMAX_MUZZLE_FLASH,

	//
	// AI symbols.
	//

	// For objects
	esAI,
	esClimb,
	esPathfind,
	esAIMass,
	esDanger,

	// For animals
	esParameter00,		// Fear
	esParameterEND = esParameter00 + eptEND - 1,
	esHumanParameter00,		// Fear
	esHumanParameterEND = esHumanParameter00 + eptEND - 1,
	esDamageParameter00,		// Fear
	esDamageParameterEND = esDamageParameter00 + eptEND - 1,
	esBravery,
	esTeam,

	esSpeed,
	esMouth,
	
	esDinosaur,
	esVocal,
	esSenseTerrain,
	esSenseObjects,
	esSenseAnimates,
	esTerrainPathfinding,
	esWakeUp,
	esSleep,
	esStayNearMax,
	esStayNearOK,
	esStayNearTarget,
	esStayAwayMin,
	esStayAwayOK,
	esStayAwayTarget,
	esBiteTargetDistance,
	esFeintTargetDistance,

	esWidth,
	esHeadReach,
	esTailReach,
	esClawReach,
	esJumpDistance,
	esJumpUp,
	esJumpDown,
	esWalkOver,
	esWalkUnder,
	esJumpOver,
	esCrouchUnder,
	esMoveableMass,
	esUpAngle,
	esDownAngle,
	
	// AI Smarts 
	esNodesPerSec,
	esMaxNodes,
	esTimeToForgetNode,
	esTimeToForgetInfluence,
	esMaxPathLength,
	esMaxAStarSteps,



	// Activity
	esActBASE,
	esActEND = esActBASE + iMAX_ACTIVITIES - 1,

	esActRATINGS_BASE,
	esActRATINGS_END = esActRATINGS_BASE + iMAX_ACTIVITIES * eptEND - 1,

	esArchetype,
	esAIType,

	// Particle props.
	esSolid,
	esDust,
	esLog,
	esNumber,
	esVelocity,
	esLifespan,
	esSize,
	esLength,
	esGravity,
	esStuckGravity,
	esSpread,
	esThreshold,

	esTextPosition,
	esTextDisplayTime,
	esTextAtTop,
	esOverlayText,
	esResourceID,

	esEND
};




//
// Global functions.
//

//*********************************************************************************************
//
class CLoadWorld
//
// Loads a 'Groff' file into the world database.
//
// Prefix: lw
//
//**************************************
{
public:

	CGroffIO			gioFile;		// Groff file.
	CGroffObjectInfo	goiInfo;		// Info in the file.
    int32               i4Error;        // Error value

	static bool			bTextures;		// true if we use textures.
	static bool			bBumps;			// true if we use bump maps.
	static bool			bCreateWrap;	// Flag to indicate if optimal wraps are created
										// at load time.
	static bool			bRebuildPartitions;	// true if we rebuild partitions at load time.
	static bool			bVerbose;		// True if we should dout lots of info
	static CGroffObjectName* pgonCurrentObject;	// The object we are loading right now.

public:
	CValueTable			vtableTable;	// The value table for the loading experience.


	///   Symbol handles!
	// Each handle references a symbol with the same name is the enum used specify it.
	//
	//	Ex.      ahSymbols[esClass]  references the string "Class" in the value table,
	//									or is NULL if "Class" does not appear in the table.
	
	CHandle ahSymbols[esEND];


	int iNumSelected;					// The number of objects that have been selected by the loader. (UI)
	int iLoadPass;						// Indicates the current loading pass, so objects like
										// magnets can load after instances.

#if VER_TEST
	bool	abSymbolInit[esEND];
#endif


	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	// Constructor requiring a filename.
	CLoadWorld(const char* str_filename, PFNWORLDLOADNOTIFY pfnWorldNotify = NULL, uint32 ui32_NotifyParam = 0);

	// Destructor.
	~CLoadWorld();

	rptr<CMesh> pmshLoadMesh
	(
		const CGroffObjectName& gon,
		CPresence3<>* ppr3_adjust,
		bool b_animating = false
	);


	inline const CHandle& hSymbol(ESymbol es)
	{
		return ahSymbols[es];
	}

	// Returns index of symbol with string equal to "estr" or -1 if not found.
	int iFindSymbol(const CEasyString& estr);
	
	//*****************************************************************************************
	//
	static void PurgeMeshes
	(
	);
	//
	// Removes all the meshes from the mesh instancer.
	//
	//**************************************

protected:

	//*****************************************************************************************
	//
	// Protected member functions.
	//
	
	void LoadObjectToWorld(CGroffObjectName& gon);

	void InitHandleTable();
	void InitHandle(ESymbol es, char *str_symbol_name);
};



// Instancing information for objects from the 'Groff' system.
struct SMeshInstance
// Prefix: mshi
{
	rptr<CRenderType>	prdtMesh;
	CPresence3<>		pr3Adjust;

	SMeshInstance()
	{
	}

	SMeshInstance(rptr<CRenderType> prdt, CPresence3<> pr3)
		: prdtMesh(prdt), pr3Adjust(pr3)
	{
	}
};


//
// Global function.
//

//*****************************************************************************************
//
void RemoveMeshReferences
(
);
//
// Removes all references to meshes.
//
//**************************************

//
// Global variables.
//

// The loader's console buffer for logging activity.
extern CConsoleBuffer conLoadLog;

extern float fDefaultBumpiness;




#endif
