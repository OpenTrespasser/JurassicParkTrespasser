/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * CEntityWater
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Water.hpp                                             $
 * 
 * 66    98/10/03 22:37 Speter
 * Set partition flag to avoid calling PreRender unless actually viewed.
 * 
 * 65    9/17/98 5:33p Pkeet
 * Added the 'ResetLuts' member function.
 * 
 * 64    98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 63    8/30/98 5:04p Pkeet
 * Added the 'PurgeWaterTextures' global function
 * 
 * 62    8/23/98 3:06a Pkeet
 * Direct3D water textures are now purged when switching resolutions.
 * 
 * 61    98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 60    7/20/98 12:30p Mlange
 * Water resolution on text prop.
 * 
 * 59    98.06.12 3:24p Mmouni
 * Added enable/disable support.
 * 
 * 58    98/05/21 15:47 Speter
 * Now actually updates when resolution settings are changed.
 * 
 * 57    5/15/98 6:25p Mlange
 * Multiresolution water on build version switch.
 * 
 * 56    5/11/98 2:24p Mlange
 * Made water resolution range variable types.
 * 
 * 55    5/11/98 1:01p Mlange
 * Water quad tree work in progress.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_WATER_HPP
#define HEADER_LIB_ENTITYDBASE_WATER_HPP


//
// Required includes.
//
#include "Entity.hpp"
#include "Message.hpp"
#include "Lib/Transform/TransLinear.hpp"
#include "Lib/Transform/Matrix2.hpp"
#include "Lib/Sys/Timer.hpp"
#include "Lib/Std/Array2.hpp"
#include "Lib/Std/RangeVar.hpp"


//
// Opaque type declarations for CEntityWater.
//
class CSimWater2D;
class CRaster;
class CTexture;
class CDraw;
class CCamera;
typedef float TMetres;

//
// Macros.
//

// The default amount of memory assigned to the system.
#define iDEFAULT_WATER_TEXTURE_MEMORY_KB (384 * 1024)

//
// Class definitions.
//

class CWaterNode;

#if VER_MULTI_RES_WATER
	namespace NMultiResolution {class CQuadRootWaterShape;};
#endif

//*********************************************************************************************
//
class CEntityWater: public CEntity
//
//**************************************
{
private:
	CWaterNode*		pwnRoot;			// The root of the water tree.
	CAArray2<TMetres> pa2mDepth;		// Stored array of depth values per element, max res.
	rptr<CTexture>	ptexOriginal;		// The source texture, if any.  Used only for solid colour.
	CTransform2<>	tf2WorldObject,		// 2D transforms between world and object space.
					tf2ObjectWorld;
	TSec			sRequireStep;		// Remember step size till just before rendering.
	int				iIsRunning;			// Non-zero when the water updated last step message.
	bool			bEnabled;			// True when water movement is enabled.

	float			fResFactor;			// User defined multiplier for water resolution.

public:

#if VER_MULTI_RES_WATER
	NMultiResolution::CQuadRootWaterShape* pqnwshRoot;

	static CRangeVar<float> rvarWireZoom;
	static bool bShowQuadTree;			// Render water quad tree.
#endif

	static bool bAlpha;					// Indicates that water should be rendered using alpha.
	static bool bInterp;				// Interpolate values (only applies in software rendering).
	static CRangeVar<float> rvarMaxScreenRes;			// Target number of water elements per pixel.
	static CRangeVar<float> rvarMaxWorldRes;			// Max number of elements per metre.

	TReal			rMaxScreenRes,		// Values this node is currently using.
					rMaxWorldRes;		// (Stored to detect updates).

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	// Standard instance constructor.
	CEntityWater(const SInit& initins);

	// Standard GROFF constructor.
	CEntityWater
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const ::CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo
	);

	~CEntityWater();


	//******************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual bool bContainsCacheableMeshes();

	//*****************************************************************************************
	virtual void InitializeDataStatic();

	//*****************************************************************************************
	virtual void PreRender(CRenderContext& renc);

	//*****************************************************************************************
	virtual void Process(const CMessageStep& msgstep);

	//*****************************************************************************************
	virtual void Process(const CMessageSystem& msgsys);

	//*****************************************************************************************
	virtual void Move(const CPlacement3<>& p3_new, CEntity* pet_sender = 0);

	//*****************************************************************************************
	virtual void Cast(CEntityWater** ppetw)
	{
		*ppetw = this;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	#define rWATER_NONE		-1e6		// A very large negative number.

	//******************************************************************************************
	//
	TReal rWaterHeight
	(
		const CVector2<>& v2_world		// World position.
	);
	//
	// Returns:
	//		The height of the water surface at the given world coordinates, taking account
	//		of both water world location, and wave height.
	//
	//		If the coords are outside the water surface, or under the terrain, returns rWATER_NONE.
	//
	//**********************************

	//******************************************************************************************
	//
	void CreateDisturbance
	(
		CVector3<> v3_world_centre,		// Centre of disturbance to make, in world coords.
		TReal r_radius,					// Radius of the disturbance.
		TReal r_height,					// Height of the disturbance (negative for depressions).
		bool b_add = false				// Whether to add the disturbance (else set it).
	);
	//
	// Makes a hemispherical disturbance in the water surface.
	//
	//**********************************

	//*****************************************************************************************
	//
	virtual bool bIsMoving
	(
	) const;
	//
	// Returns 'true' always, as water is always dynamic.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void PurgeD3DTextures
	(
	);
	//
	// Purge Direct3D textures.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void Enable
	(
		bool b_enabled
	);
	//
	// Enable or disable this water entity.
	//
	//**************************

	//******************************************************************************************
	//
	static int iGetManagedMemUsed();
	//
	// Returns:
	//		The amount of memory allocated for water, in bytes.
	//
	//**************************************

	//******************************************************************************************
	//
	static int iGetManagedMemSize();
	//
	// Returns:
	//		The amount of memory managed for water, in bytes.
	//
	//**************************************

	//******************************************************************************************
	//
	static void SetTextureMemSize
	(
		int i_mem_size_kb = iDEFAULT_WATER_TEXTURE_MEMORY_KB
	);
	//
	// Allocate texture memory.
	//
	//**************************************

	//******************************************************************************************
	//
	static void ResetLuts
	(
	);
	//
	// Resets lookup tables used for water.
	//
	//**************************************

	class CPriv;
	friend CPriv;

	friend CWaterNode;
	friend class CWaveNode;
	friend class CPhysicsInfoWater;
};


//
// Global functions.
//

//*****************************************************************************************
//
void PurgeWaterTextures
(
);
//
// Purges water textures. This function is particularly useful for Direct3D.
//
//**************************************

#endif

