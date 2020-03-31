/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Water.hpp.
 *
 *	Bugs:
 *		Determine water limits based on original object polygon, not bounding rectangle.
 *		Allow nodes to determine at creation time whether they lie outside all limits, and thus
 *		 need no data.
 *
 * To do:
 *		Implement a full-featured quad-tree for the water.
 *			Currently, much of the quad-tree structure is here, but the entity simply subdivides
 *			 the root once, and performs no further subdivisions.
 *			In PreRender, execute node-subdivision tests as well as node-resolution tests.
 *			Extend the Integrate function to handle all nodes.
 *			Extend the node "neighbouring" code to handle many-to-one borders.
 *			Optional: Determine visibility per node rather than per entity; 
 *			 execute integration and texture creation only on visible nodes; 
 *			 wake floating objects only in active nodes;
 *			 purge nodes not viewed for a while individually.
 *		Create a custom CPolyIterator rather than the hacky 4-polygon mesh.
 *		
 *		Remove ptexOriginal and its associated clut. Create some other way of determining non-alpha
 *		 pixel values.
 *		Replace pa2mDepth with a lower-density, 8-bit version.
 *
 *		Implement FillTextureMap and FillIntensityMapInterp functions in ASM.
 *		Reduce iLIMIT_EXTEND; instead, increase world extent of water to compensate for sorting errors.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Water.cpp                                             $
 * 
 * 214   98/10/09 0:33 Speter
 * Max water size, varying by quality setting.
 * 
 * 213   98/10/06 18:57 Speter
 * Upped impulse scale for bullets. No longer check mMaxAmplitude before waking up floaters
 * (it's gone anyway). 
 * 
 * 212   98/10/03 22:37 Speter
 * Only wake floating objects in water. Set partition flag to avoid calling PreRender unless
 * actually viewed.
 * 
 * 211   9/28/98 12:46p Pkeet
 * Handle D3D texture allocation failure gracefully. Removed the upload on create.
 * 
 * 210   9/17/98 5:33p Pkeet
 * Added the 'ResetLuts' member function.
 * 
 * 209   98.09.11 8:58p Mmouni
 * Water now creates a default alpha texture for safety if a regular texture is not created.
 * 
 * 208   9/10/98 4:51p Pkeet
 * Made water safer.
 * 
 * 207   98.09.09 5:55p Mmouni
 * Enabled/Added intensity clamps and put them on a switch.
 * 
 * 206   98.09.08 3:46p Mmouni
 * Now fills water texture with gray when creating it so that any edge pixels that show will
 * look ok.
 * 
 * 205   98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 204   9/02/98 5:17p Asouth
 * added some specifiers to ensure the MW compiler is happy
 * 
 * 203   98/08/31 1:19 Speter
 * Set water damage to 0.
 * 
 * 202   8/30/98 5:04p Pkeet
 * Added the 'PurgeWaterTextures' global function. Cleaned up the water node destructor.
 * 
 **********************************************************************************************/

#include <list>
#include "Common.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/View/RasterD3D.hpp"
#include "Water.hpp"

#include "Lib/Std/LocalArray.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"

#if VER_MULTI_RES_WATER
	#include "Lib/GeomDBase/WaterQuadTree.hpp"
#endif

#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/Renderer/Pipeline.hpp"
#include "Lib/Physics/Waves.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPaint.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/EntityDBase/Query/QPhysics.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/EntityDBase/Subsystem.hpp"
#include "Lib/GeomDBase/Terrain.hpp"
#include "Lib/GeomDBase/RayCast.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/View/LineDraw.hpp"
#include "Lib/Math/FloatTable.hpp"
#include "Lib/Renderer/LightBlend.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"
#include "Lib/EntityDBase/QualitySettings.hpp"

//#include "Config.hpp"
//#define TARGET_PROCESSOR PROCESSOR_PENTIUM
//#define VER_ASM			0

#include "AsmSupport.hpp"
#include "Lib/Std/PrivSelf.hpp"

#include <malloc.h>

extern CProfileStat		psWaterStep;


//
// Keep a list of currently active water.
//
typedef std::list<CEntityWater*> TWaterList;
TWaterList wlWaters;


//*****************************************************************************************
//
namespace
//
// Private namespace for CEntityWater implementation.
//
//**************************************
{
	//
	// Constants.
	//

	#define bCAM_PIXEL					1			// Per-pixel camera angles calculated.
	#define bREAL_SPECULAR				(0 && bCAM_PIXEL)	
													// Correct sun-specular reflections calculated.
	#define bPOWER_OF_2					0			// Force power-of-2 textures.

	#define INTENSITY_CLAMP				1			// Clamp calculated dot product.

	#define iLIMIT_EXTEND				4			// Number of water elements to display beyond
													// the integration limits (should be even, for 3DX).

	const float fTEX_ADJUST				= 0.001;	// Adjust tex coords inward for safety.

	const int iMAX_WATER_SIZE			= 256;		// Max we allow any water to be.

	const int iBONES_INCR				= 4;		// Default divisor for number of water elements drawn.
	const int iBONES_MAX				= 32;		// Maximum number of water elements drawn.

	// Parameters controlling how impulses are applied.
	const TReal rIMPULSE_SCALE			= 0.04;
	const TMetres mMAX_DISTURB			= 0.25;
	const TMetres mIMPULSE_RADIUS		= 0.25;

	const TSec sPURGE_TIME				= 10.0;		// Time after which water data is destroyed.

	const TAngleWidth angwSUN			= angwFromAngle(3 * dDEGREES);
													// Larger than real sun, but looks better.
	const TReal rMIN_CAMERA_Z			= 0.008;	// Value used to clamp the effective angle
													// at which water is viewed.  Empirically
													// determined to prevent all-white water.
	//
	// Variables.
	//

	int      iMemUsedTotal = 0;				// Amount of memory in use for water.
	int      iManagedMemSize = iDEFAULT_WATER_TEXTURE_MEMORY_KB;				
											// Amount of memory allocated for use by water.

	int 		iAlphaState = -1;			// Not initialized yet.
	bool		bDirect3DClut = false;		// Indicates that the clut was built for Direct3D.
	aptr<CClut>	pClutNonAlpha;				// Pointer to the non-alpha conversion clut.

	//
	// Internal class definitions.
	//

	#define iREFLECT_OUT	32

	//*********************************************************************************************
	//
	class CReflectTable: public CFloatTableRound<int, 512, -1, +1>
	//
	// Prefix: rfltab
	//
	//**************************************
	{
	public:
		CReflectTable()
		{
			//
			// Initialise table.
			//

			// Declare local material, so as not to depend on static initialisation of matWATER.
			CMaterial mat_water(1.0, 1.80, angwZERO, true, true, 1.33);

			for (int i = 0; i < satTable.size(); ++i)
			{
				float f_in = fInputFull(i);
				satTable[i] = (f_in < 0? 1.0 : mat_water.fReflected(f_in)) * (float(iREFLECT_OUT) - 0.1);
			}
		}

		float fGetAdd()
		{
			return fAdd;
		}

		float fGetScale()
		{
			return fScale;
		}

		int* piGetTable()
		{
			return satTable.atArray;
		}
	};

	CReflectTable	ReflectTable;

	// Conversion table of reflection intensity to pixels.
	CSArray<uint16, 32> sau2PixelTable;

	//*********************************************************************************************
	//
	class CPixelReflectTable: public CFloatTableRound<uint16, 512, -1, +1>
	//
	// Prefix: rfltaba
	//
	// Direct conversion from dot-product to screen pixel.
	//
	//**************************************
	{
	public:

		CPixelReflectTable()
		{
			Assert(sau2PixelTable.size() == iREFLECT_OUT);

			//
			// Initialise table, with alpha values corresponding to intensities in ReflectTable.
			//

			for (int i = 0; i < satTable.size(); ++i)
			{
				int i_reflect = ReflectTable[i];
				satTable[i] = sau2PixelTable[i_reflect];
			}
		}

		float fGetAdd()
		{
			return fAdd;
		}

		float fGetScale()
		{
			return fScale;
		}

		uint16* pu2GetTable()
		{
			return satTable.atArray;
		}
	};

	CPixelReflectTable	PixelReflectTable;


	//*********************************************************************************************
	//
	inline float fWaterResAdjFactor()
	//
	// Obtain the water resolution adjustment factor based on the overall quality setting.
	//
	//**************************************
	{
		return qdQualitySettings[iGetQualitySetting()].fWaterResScale;
	}
};

//**********************************************************************************************
//
class CPhysicsInfoWater: public CPhysicsInfo
//
// A special physics info for water which handles ApplyImpulse, and other things.
//
//**************************************
{
public:

	//**********************************************************************************************
	CPhysicsInfoWater()
		// HACK: Set movable so I can save it in different positions!
		: CPhysicsInfo(SPhysicsData(Set(epfTANGIBLE) + epfMOVEABLE, matHashIdentifier("WATER")))
	{
		// Water does not do no damage to nothing. 
		// May change in future if we get realistic energy from collisions.
		fDamage = 0.0f;
	}

	//**********************************************************************************************
	//
	// Overrides.
	//

	//**********************************************************************************************
	virtual void RayIntersect(CInstance* pins, int i_subobj, CRayCast& rc,
							  const CPlacement3<>& p3, TReal r_length, TReal r_diameter) const
	{
		// Collide with rendering bounding box.
		SObjectLoc obl;
		if (pins->pbvBoundingVol()->bRayIntersect(&obl, pins->pr3GetPresence(), p3, r_length, r_diameter))
		{
			obl.pinsObject = pins;
			obl.iSubObject = i_subobj;
			rc.InsertIntersection(obl);
		}
	}

	//**********************************************************************************************
	virtual void ApplyImpulse(CInstance* pins, int i_subobj, const CVector3<>& v3_pos, const CVector3<>& v3_impulse) const
	{
		CEntityWater* petw = dynamic_cast<CEntityWater*>(pins);
		if (!petw)
			return;

		TMetres m_depth = MinMax(v3_impulse.tZ * rIMPULSE_SCALE, -mMAX_DISTURB, +mMAX_DISTURB);

		petw->CreateDisturbance
		(
			v3_pos,							// World position.
			mIMPULSE_RADIUS,				// Radius of depression.
			m_depth,						// Depth of depression.
			true							// Additive.
 		);
	}

	//*****************************************************************************************
	virtual void DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const;
};

//*********************************************************************************************
//
class CWaveNode
//
// Prefix: wvn
//
// Contains all info for simulating and rendering a uniform-density rectangle of water.
//
//**************************************
{
public:
	ptr<CSimWater2D> pWater2;			// The water model.
	CRectangle<>	rcObject;			// Object-space extents of this node.
	rptr<CRaster>	prasTexture;		// The (destination) water raster.
	rptr<CTexture>	ptexTexture;		// The (destination) water texture.
	CRectangle<>	rcTexExtents;		// Extents of texture coords (may be inward of [0..1]
										// to account for texture over-allocation.
	int				iMipLevel;			// The current mip level running.

protected:
	CTransLinear2<>	tlr2ModelObject,	// Translations between object (metre) and model 
					tlr2ObjectModel;	// (element) coordinates.
	uint			bInterpState: 1,	// Cache interp state for this object.
					bInterpActual: 1;	// Whether we are really interpolating.

	int             iMemUsed;			// Amount of memory used for this water node.

public:

	//******************************************************************************************
	CWaveNode
	(
		const CRectangle<>& rc_object	// Object-space extents.
	);

	//******************************************************************************************
	~CWaveNode();

	//******************************************************************************************
	//
	static CVector2<int> v2iWaterSize
	(
		const CEntityWater* petw_owner,
		const CVector2<>& v2_world_size,
		int i_mip_level,
		bool b_add_borders = true
	);
	//
	// Returns:
	//		The texture size needed for the given mip level, as a 2D int vector.
	//
	//**********************************

	//******************************************************************************************
	//
	void UpdateRes
	(
		const CEntityWater* petw_owner,
		const CCamera& cam, 
		const CVector3<> v3_cam_obj,
		CPArray2<TMetres> pa2m_depth, 
		const CTransLinear2<>& tlr2_depth
	);
	//
	// Determines desired resolution for water, and recreates if necessary.
	//
	//**********************************

	//*****************************************************************************************
	//
	void FillWaterTexture
	(
		const CCamera& cam,					// Current camera to render from.
		const CPlacement3<>& p3_cam_obj,	// The object-space camera placement.
		const CDir3<>& d3_sun				// Object-space sun direction.
	);
	//
	// Fill the texture owned by this with the values corresponding to the current wave sim.
	// If bInterpState, interpolate extra elements between each pair, in both x and y.
	//
	//**********************************

	//******************************************************************************************
	//
	void SetNeighbours
	(
		CWaveNode* pwn_0, CWaveNode* pwn_1, CWaveNode* pwn_2, CWaveNode* pwn_3
	);
	//
	//**********************************

	//******************************************************************************************
	//
	TReal rWaterHeight
	(
		const CVector2<>& v2_object		// Object-space location; must be within the node.
	) const;
	//
	// Returns:
	//		The water height at the given object location, in object space.
	//
	//**********************************

	//******************************************************************************************
	//
	void CreateDisturbance
	(
		CVector2<> v2_obj, 
		TReal r_radius, TReal r_height,
		bool b_add
	);
	//
	//**********************************

	//*****************************************************************************************
	void DrawPhysics(CDraw& draw, CCamera& cam, const CTransform3<>& tf3_shape_screen) const;

private:

	//******************************************************************************************
	int iRowStart() const
	{
		return Max(pWater2->lmRows.iStart - iLIMIT_EXTEND, 1);
	}

	//******************************************************************************************
	int iRowEnd() const
	{
		return Min(pWater2->lmRows.iEnd   + iLIMIT_EXTEND, pWater2->iHeight-2);
	}

	//******************************************************************************************
	int iColStart(int i_row) const
	{
		return Max(pWater2->palmColLimits[i_row].iStart - iLIMIT_EXTEND, 1);
	}

	//******************************************************************************************
	int iColEnd(int i_row) const
	{
		return Min(pWater2->palmColLimits[i_row].iEnd   + iLIMIT_EXTEND, pWater2->iWidth-2);
	}

	//******************************************************************************************
	//
	int iGetMipLevel
	(
		const CEntityWater* petw_owner,
		const CCamera& cam,				// Rendering camera.
		const CVector3<> v3_cam_obj		// Camera position in object space.
	) const;
	//
	// Returns:
	//		The desired mip level for the water, depending on camera.
	//
	//**********************************

	//*****************************************************************************************
	//
	void CreateTexture
	(
		CVector2<int> v2i_size			// Desired size for texture.
	);
	//
	// Creates/replaces the output texture at a resolution derived from the current pWater2.
	// This resolution is in general double that of pWater2 if bInterpState is on.
	//
	//**********************************

	//*****************************************************************************************
	//
	void FillIntensityMapInterp
	(
		CPArray<uint8> pau1,			// Array of intensities to fill.
		int i_y,						// Water row to convert.
		CVector3<> v3_cam,				// Water-space camera position.
		CDir3<> d3_sun					// Sun direction for optional specular.
	);
	//
	// Fill the array with reflection intensity values derived from the water row, with
	// an additional intensity interpolated between each pair.
	//
	//**********************************

	//*****************************************************************************************
	//
	void FillTextureMap
	(
		CPArray<uint16> pau2,			// Array of alpha pixels to fill.
		int i_y,						// Water row to convert.
		CVector3<> v3_cam,				// Water-space camera position.
		CDir3<> d3_sun					// Sun direction for optional specular.
	);
	//
	// Fill the array with texels derived from the water row.
	//
	//**********************************
};

//*********************************************************************************************
//
class CWaterNode: public CWaveNode
//
// Prefix: wn
//
// A quad-tree node, possibly containing water data.
// Handles recursive descent for several water functions.
//
// In this system, only leaf nodes contain water data, but all nodes are the same structure
// for simplicity; the memory hit should be minor.
//
//**************************************
{
public:
	CWaterNode*			pwnParent;
	aptr<CWaterNode>	apwnChildren[4];	// Child nodes, if any.

public:

	//******************************************************************************************
	~CWaterNode();

public:

	//******************************************************************************************
	CWaterNode
	(
		CWaterNode* pwn_parent,		// Parent node.
		CRectangle<> rc_object			// The object-space rectangle used by this node.
	);

	//******************************************************************************************
	//
	bool bHasChildren() const
	//
	// Returns:
	//		Whether this node contains child nodes; it's all 4 or none..
	//
	//**********************************
	{
		return apwnChildren[0] != 0;
	}

	//******************************************************************************************
	//
	bool bHasWater() const
	//
	// Returns:
	//		Whether this node contains wave data.
	//
	//**********************************
	{
		if (pWater2 != 0)
		{
			Assert(!bHasChildren());
		}
		return pWater2 != 0;
	}

	//******************************************************************************************
	//
	CWaveNode* pwnFindNode
	(
		const CVector2<>& v2_object		// Object-space point. Must be within the node.
	);
	//
	// Returns:
	//		The water node containing this point.
	//
	//**********************************

	//******************************************************************************************
	//
	void Subdivide();
	//
	// Does it, creating 4 children without waternodes.
	//
	//**********************************

	//******************************************************************************************
	//
	void Decimate();
	//
	// Does it, removing all children.
	//
	//**********************************

	//******************************************************************************************
	//
	void UpdateRes
	(
		const CEntityWater* petw_owner,
		const CCamera& cam, 
		const CVector3<>& v3_cam_obj,
		CPArray2<TMetres>& pa2m_depth, 
		const CTransLinear2<>& tlr2_depth
	);
	//
	//**********************************

	//*****************************************************************************************
	//
	void FillWaterTexture
	(
		const CCamera& cam,					// Current camera to render from.
		const CPlacement3<>& p3_cam_obj,	// Object-space camera placement.
		const CDir3<>& d3_sun				// Object-space sun direction.
	);
	//
	// Fill all water textures in this node.
	//
	//**********************************

	//*****************************************************************************************
	//
	void DrawPhysics(CDraw& draw, CCamera& cam, const CTransform3<>& tf3_shape_screen) const;
	//
	// Dispatches command to all water nodes.
	//
	//**********************************

	//******************************************************************************************
	//
	void CreateDisturbance
	(
		CVector2<> v2_object,			// Centre of disturbance to make, in world coords.
		TReal r_radius,					// Radius of the disturbance.
		TReal r_height,					// Height of the disturbance (negative for depressions).
		bool b_add = false				// Whether to add the disturbance (else set it).
	);
	//
	// Makes a hemispherical disturbance in the water surface.
	//
	//**********************************

};

//******************************************************************************************
//
// class CWaveNode implementation.
//

	//******************************************************************************************
	CWaveNode::CWaveNode(const CRectangle<>& rc_object)
		: rcObject(rc_object)
	{
		// Force update first time.
		iMipLevel		= -1;
		bInterpState	= -1;
	}

	//******************************************************************************************
	CWaveNode::~CWaveNode()
	{
		iMemUsedTotal -= iMemUsed;
		delete pWater2.ptGet();
	}

	//******************************************************************************************
	int CWaveNode::iGetMipLevel(const CEntityWater* petw_owner, const CCamera& cam, const CVector3<> v3_cam_obj) const
	{
		// Do nothing if underwater.
		if (v3_cam_obj.tZ < 0)
			return -1;

		// Find minimum distance to object, clamped to an object axis.
		TReal r_min_dist_x = rcObject.tX0() - v3_cam_obj.tX;
		if (r_min_dist_x < 0)
			r_min_dist_x = v3_cam_obj.tX - rcObject.tX1();

		TReal r_min_dist_y = rcObject.tY0() - v3_cam_obj.tY;
		if (r_min_dist_y < 0)
			r_min_dist_y = v3_cam_obj.tY - rcObject.tY1();

		TReal r_min_dist = Max(Max(r_min_dist_x, r_min_dist_y), v3_cam_obj.tZ);


		// Calculate desired world res, adjusted by the overall quality setting.
		float f_world_res = CEntityWater::rvarMaxScreenRes * fWaterResAdjFactor() * petw_owner->fResFactor *
			                cam.campropGetProperties().fViewWidthRatio * 0.5 / r_min_dist;

		//
		// Calculate mip level desired for current water.
		//

		// Get maximum possible texture size, and compute corresponding world water density.
		CVector2<int> v2i_size = v2iWaterSize(petw_owner, rcObject.v2Extent(), 0);
		float f_max_water_res = Max(v2i_size.tX / rcObject.tWidth(), v2i_size.tY / rcObject.tHeight());

		int i_mip_level = 0;
		while (f_world_res < f_max_water_res)
		{
			// Break when we've hit maximum mip level.
			if (v2i_size.tX < 16 || v2i_size.tY < 16)
				break;

			i_mip_level++;
			v2i_size.tX >>= 1;
			v2i_size.tY >>= 1;
			f_max_water_res *= 0.5;
		}

		return i_mip_level;
	}

	//******************************************************************************************
	void CWaveNode::UpdateRes(const CEntityWater* petw_owner, const CCamera& cam, const CVector3<> v3_cam_obj, 
		CPArray2<TMetres> pa2m_depth, const CTransLinear2<>& tlr2_depth)
	{
		int i_mip_level = iGetMipLevel(petw_owner, cam, v3_cam_obj);
		if (i_mip_level < 0)
		{
			if (!ptexTexture)
			{
				// Create default alpha texture just in case we need it.
				ptexTexture = rptr_new CTexture(128,128,128,128);
			}
			return;
		}

		bool b_do_interp = CEntityWater::bInterp && !d3dDriver.bD3DWater();
		if (pWater2 && iMipLevel == i_mip_level && 
			bInterpState == b_do_interp &&
			iAlphaState == (int)CEntityWater::bAlpha)
			// Everything's fine.
			return;

		iMipLevel		= i_mip_level;
		iAlphaState		= CEntityWater::bAlpha;
		bInterpState	= b_do_interp;
		bInterpActual	= b_do_interp;

		Assert(i_mip_level >= 0);

		// Determine water size from mip level.
		CVector2<int> v2i_size = v2iWaterSize(petw_owner, rcObject.v2Extent(), i_mip_level);

		// Calculate the world-water translations, moving 1 inward for borders.
		tlr2ObjectModel = CTransLinear2<>
		(
			rcObject,
			CRectangle<>(1, 1, v2i_size.tX-2, v2i_size.tY-2)	// Model rectangle (adjust in for borders).
		);
		tlr2ModelObject = ~tlr2ObjectModel;

		// We can round these indices to avoid small floating-point errors.
		int i_x0 = iPosRound(rcObject.tX0() * tlr2_depth.tlrX);
		int i_y0 = iPosRound(rcObject.tY0() * tlr2_depth.tlrY);
		int i_x1 = iPosRound(rcObject.tX1() * tlr2_depth.tlrX);
		int i_y1 = iPosRound(rcObject.tY1() * tlr2_depth.tlrY);

		CPArray2<TMetres> pa2m_depth_sub = pa2m_depth.pa2Rect(SRect(i_x0, i_y0, i_x1 - i_x0, i_y1 - i_y0));

		//
		// Create the water object.
		//

		if (pWater2)
		{
			// Convert wave info from old to new water.
			CSimWater2D* pwat2 = pWater2;
			pWater2 = new CSimWater2D
			(
				v2i_size.tY, v2i_size.tX, *pWater2, pa2m_depth_sub
			);
			delete pwat2;
		}
		else
		{
			// Create water object first time.
			pWater2 = new CSimWater2D
			(
				v2i_size.tY, v2i_size.tX, rcObject.tHeight(), rcObject.tWidth(), pa2m_depth_sub
			);
		}

		//
		// Compute the texture size from water size.
		// The outer elements of each water are for boundary conditions, not actual water.
		// For uninterpolated textures, a texel is derived from each pair of water elements, so
		//
		//		tx = tw - 3
		//
		// For interpolated textures, the same process is performed, then an additional element
		// interpolated between each pair, so
		//
		//		tx = 2*tw - 7
		//
		CVector2<int> v2i_tex_size;

		if (bInterpActual)
		{
			// Test whether we can make a texture large enough for interpolation.
			v2i_tex_size.tX = v2i_size.tX * 2;
			v2i_tex_size.tY = v2i_size.tY * 2;

			if (d3dDriver.bD3DWater())
				d3dDriver.SetTextureMinMax(v2i_tex_size.tX, v2i_tex_size.tY, false);

			if (v2i_tex_size.tX < v2i_size.tX*2 || v2i_tex_size.tY < v2i_size.tY*2)
				// Too large for interp.
				bInterpActual = false;
		}

		if (bInterpActual)
		{
			v2i_tex_size.tX = v2i_size.tX * 2 - 7;
			v2i_tex_size.tY = v2i_size.tY * 2 - 7;
		}
		else
		{
			v2i_tex_size.tX = v2i_size.tX - 3;
			v2i_tex_size.tY = v2i_size.tY - 3;
		}

		// Create the texture.
		CreateTexture(v2i_tex_size);
	}

	//******************************************************************************************
	CVector2<int> CWaveNode::v2iWaterSize(const CEntityWater* petw_owner, const CVector2<>& v2_world_size, int i_mip_level, bool b_add_borders)
	{
		Assert(i_mip_level >= 0);

		CVector2<int> v2i_size;

		// Determine resolution from world extents, and add 2 for borders.
		float f_adj = fWaterResAdjFactor();
		v2i_size.tX = iPosFloatCast(v2_world_size.tX * CEntityWater::rvarMaxWorldRes * f_adj * petw_owner->fResFactor) >> i_mip_level;
		v2i_size.tY = iPosFloatCast(v2_world_size.tY * CEntityWater::rvarMaxWorldRes * f_adj * petw_owner->fResFactor) >> i_mip_level;

		if (b_add_borders)
		{
			v2i_size.tX += 2;
			v2i_size.tY += 2;
		}

		// Enforce absolute maximum water size.
		CVector2<int> v2i_actual
		(
			Min(v2i_size.tX, iMAX_WATER_SIZE * f_adj),
			Min(v2i_size.tY, iMAX_WATER_SIZE * f_adj)
		);

		if (d3dDriver.bD3DWater() || bPOWER_OF_2)
		{
			// Round this down to nearest power of 2, and clamp to maximum size.
			v2i_actual.tX = NextPowerOfTwo(v2i_actual.tX / 2);
			v2i_actual.tY = NextPowerOfTwo(v2i_actual.tY / 2);
			if (d3dDriver.bD3DWater())
				d3dDriver.SetTextureMinMax(v2i_actual.tX, v2i_actual.tY, false);
		}

		// Now adjust one dimension down to maintain aspect ratio.
		if ((float)v2i_actual.tX / v2i_size.tX < (float)v2i_actual.tY / v2i_size.tY)
		{
			v2i_size.tY *= (float)v2i_actual.tX / v2i_size.tX;
			v2i_size.tX = v2i_actual.tX;
		}
		else
		{
			v2i_size.tX *= (float)v2i_actual.tY / v2i_size.tY;
			v2i_size.tY = v2i_actual.tY;
		}

		// Round down to even. (Needed for integration).
		v2i_size.tX &= ~1;
		v2i_size.tY &= ~1;

		// Dude, we can't make the water too small.
		SetMax(v2i_size.tX, 8);
		SetMax(v2i_size.tY, 8);

		return v2i_size;
	}

	//******************************************************************************************
	void CWaveNode::CreateTexture(CVector2<int> v2i_size)
	{
		// Track the amount of memory in use by water.
		iMemUsedTotal -= iMemUsed;

		int i_width  = v2i_size.tX,
			i_height = v2i_size.tY;

		// Construct a new 16 bit raster.
		if (d3dDriver.bD3DWater() || bPOWER_OF_2)
		{
			// Adjust resolution.
			i_width  = NextPowerOfTwo(v2i_size.tX);
			i_height = NextPowerOfTwo(v2i_size.tY);

			if (d3dDriver.bUseSquareTextures())
				i_width = i_height = Max(i_width, i_height);

		}

		if (d3dDriver.bD3DWater())
			d3dDriver.SetTextureMinMax(i_width, i_height, false);

		// Make sure the allocation is null.
		prasTexture = rptr0;

		// Attempt to allocate a D3D texture buffer.
		if (d3dDriver.bD3DWater())
		{
		#if bTRACK_D3D_RASTERS
			TrackSystem(etrWater);
		#endif
			prasTexture = rptr_cast(CRaster, rptr_new CRasterD3D(CRasterD3D::CInit
			(
				i_width,
				i_height,
				ed3dtexSCREEN_ALPHA
			)));

			// Make sure only valid rasters can be used.
			if (prasTexture)
				if (!prasTexture->bVerifyConstruction())
					prasTexture = rptr0;
		}

		//
		// If the D3D allocation was unsuccessful, or if D3D water is not in use, create a system
		// memory buffer.
		//
		if (!prasTexture) 
		{
			// Create ratser.
			prasTexture = rptr_cast(CRaster, rptr_new CRasterMem
			(
				i_width, i_height,
				16,
				0,
				&prasMainScreen->pxf	// Use the screen pixel formal.
			));

			// Fill with grey so that any edges showing will look ok.
			prasTexture->Clear(prasTexture->pixFromColour(CColour(80,80,80)));
		}
		Assert(prasTexture);

		// Tract the amount of memory in use by water.
		iMemUsed = prasTexture->iWidth * prasTexture->iHeight * (prasTexture->iPixelBits / 8);
		iMemUsedTotal += iMemUsed;

		//
		// Assign it to the mesh's surface, replacing any existing texture and rasters.
		//
		CSet<ERenderFeature> seterf = Set(erfPERSPECTIVE) + erfTRAPEZOIDS + erfTEXTURE + erfFILTER;

		if (iAlphaState)
		{
			// Turn on alpha blending; wacky flag, I know.
			seterf += erfSOURCE_WATER;
		}
		else
		{
			// Turn on dithered texture; even wackier flag.
			seterf += Set(erfSOURCE_WATER) + erfDITHER;
		}

		ptexTexture = rptr_new CTexture(prasTexture, &matWATER, seterf);

		//
		// Scale all vertex uv values to account for a larger destination raster.
		// And subtract 1 texel to account for any filtering.
		//
		CVector2<> v2_max = CVector2<>
		(
			float(v2i_size.tX) / float(prasTexture->iWidth),
			float(v2i_size.tY) / float(prasTexture->iHeight)
		);

		// Set texture extents, used to generate texture coords.
		// Move them inward slightly for safety.
		rcTexExtents = CRectangle<>(fTEX_ADJUST, fTEX_ADJUST, v2_max.tX - 2*fTEX_ADJUST, v2_max.tY - 2*fTEX_ADJUST);
	}

#if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D && bCAM_PIXEL && !bREAL_SPECULAR)

	//*****************************************************************************************
	void CWaveNode::FillIntensityMapInterp
	(
		CPArray<uint8> pau1,
		int i_y,
		CVector3<> v3_cam,
		CDir3<> d3_sun
	)
	{
		// Since we read one row and column ahead, i_y cannot be the last row.
		Assert(i_y >= 0);
		Assert(i_y < pWater2->iHeight-1);

		//
		// Start and end points for x.  Since we access both the current and next elements,
		// we go only up to, but not including, iEnd.
		// Since this row will be interpolated with adjacent rows, make sure to include all
		// relevant elements from those rows.
		//
		int i_x_start = iColStart(i_y);
		if (i_y > 0)
			SetMin(i_x_start, iColStart(i_y-1));
		SetMin(i_x_start, iColStart(i_y+1));

		int i_x_end   = iColEnd(i_y);
		if (i_y > 0)
			SetMax(i_x_end, iColEnd(i_y-1));
		SetMax(i_x_end, iColEnd(i_y+1));

		// Base address of the water amplitude array.
		TMetres* am_amp = &pWater2->pa2mAmplitude[i_y][0];
		const int i_amp_dy = pWater2->iStride;

		// Copies of items that can't be directly accessed from inline ASM.
		float mIntervalX = pWater2->mIntervalX;
		float mInterval = pWater2->mInterval;
		float fTableAdd = ReflectTable.fGetAdd();
		float fTableScale = ReflectTable.fGetScale();
		int* piTable = ReflectTable.piGetTable();
		uint8* pu1 = pau1.atArray;

		// Constants we need.
		const __int64 pfNegateLow = 0x0000000080000000;

#if (INTENSITY_CLAMP)
		const float fOne = 1.0f;
		const float fNegOne = -1.0f;
#endif

		// Temps because we are out of registers.
		float r_len_add;
		float r_dot_add;

		// Typedefs so that we can access stuff from inline ASM.
		typedef CVector3<> tdCVector3;

		__asm
		{
			mov			ecx,[i_x_start]

			cmp			ecx,[i_x_end]
			jge			SKIP_LOOP

			femms									// Only needed for debug build.

			//
			// Setup for loop.
			//
			mov			edx,[i_amp_dy]
			mov			esi,[am_amp]
			mov			edi,[pu1]
			mov			ebx,[piTable]				// Base of lookup table

			lea			edx,[esi + edx*4]			// am_amp + i_amp_dy
			movd		mm0,[esi + ecx*4]			// am_amp[i_x]

			movq		mm2,v3_cam.tX				// v3_cam.tY | v3_cam.tX

			movd		mm3,[mIntervalX]

			mov			eax,ecx						// copy i_x_start

			dec			eax							// -1

			movd		mm4,eax						// i_x_start

			pi2fd		(m4,m4)						// Convert to float

			pfmul		(m4,m3)						// mIntervalX * (i_x_start-1)

			pxor		mm1,mm1						// Clear mm1

			punpckldq	mm4,mm1						// Zero high word of mm4

			pfsub		(m2,m4)						// v3_cam.tX -= mIntervalX * (i_x_start-1)

			pxor		mm3,[pfNegateLow]			// r_dx = -mIntervalX

			movd		mm6,v3_cam.tZ				// v3_cam.tZ

			movq		mm7,mm2						// v3_cam.tY | v3_cam.tX

			psrlq		mm7,32						// v3_cam.tY

			punpckldq	mm7,mm6						// v3_cam.tZ | v3_cam.tY

			movd		mm4,[mInterval]

			pfmul		(m6,m4)						// v3_cam.tZ * mInterval

			movd		[r_dot_add],mm6				// Save for later

			pfmul		(m4,m4)						// mInterval^2

			movd		[r_len_add],mm4				// Save for later

			pfmul		(m7,m7)						// v3_cam.tY^2 | v3_cam.tZ^2

			pfacc		(m7,m7)						// r_cam_len_sqr

			//
			// Loop for each element in x.
			//
			// eax = temp
			// ebx = ReflectTable.satTable
			// ecx = i_x
			// edx = am_amp + i_amp_dy
			// esi = am_amp
			// edi = pau1
			//
			// mm0 = temp, initially = am_amp[i_x]
			// mm1 = temp
			// mm2 = v3_cam.tY | v3_cam.tX
			// mm3 = 0 | r_dx
			// mm4 = temp
			// mm5 = temp
			// mm6 = temp 
			// mm7 = 0 | r_cam_len_sqr
			//
		X_LOOP:
			movq		mm1,mm0						// am_amp[i_x] from previous am_amp[i_x + 1]
			movd		mm0,[esi + ecx*4 + 4]		// am_amp[i_x + 1]

			// Incrment camera x.
			pfadd		(m2,m3)						// camy | camx
			
			punpckldq	mm1,mm1						// am_amp[i_x] | am_amp[i_x]
			punpckldq	mm0,[edx + ecx*4]			// am_amp[i_x + i_amp_dy] | am_amp[i_x + 1]

			pfsub		(m1,m0)						// r_wy | r_wx

			// Calculate inverse of normal length.
			movq		mm4,mm1
			movd		mm5,[r_len_add]

			pfmul		(m4,m4)						// r_wy^2 | r_wx^2
			movq		mm6,mm2						// camy | camx

			pfacc		(m4,m4)						// r_wy^2 + r_wx^2
			pfmul		(m6,m6)						// camx^2

			pfadd		(m4,m5)						// r_wy^2 + r_wx^2 + r_len_add

			pfadd		(m6,m7)						// camx^2 + r_cam_len_sqr

			pfmul		(m4,m6)						// r_len_sqr

			pfrsqrt		(m4,m4)						// r_inv_len
			movd		mm5,[r_dot_add]

			// Dot product of camera and normal.
			pfmul		(m1,m2)						// r_wy * camy | r_wx * camx

#if (INTENSITY_CLAMP)
			movd		mm6,[fOne]
#endif

			pfacc		(m1,m1)						// r_wy * camy + r_wx * camx

			pfadd		(m1,m5)						// r_wy * camy + r_wx * camx + r_dot_add
			movd		mm5,[fTableAdd]

			pfmul		(m1,m4)						// * f_inv_len
			movd		mm4,[fTableScale]

#if (INTENSITY_CLAMP)
			pfmin		(m1,m6)						// Don't go over one.
			movd		mm6,[fNegOne]

			pfmax		(m1,m6)						// Don't go below negative one.
#endif

			// Lookup value in the table.
			pfadd		(m1,m5)						// r_dot + ReflectTable.fAdd

			pfmul		(m1,m4)						// * ReflectTable.fScale

			pf2id		(m1,m1)

			movd		eax,mm1

			mov			eax,[ebx + eax*4]			// Table lookup.
			mov			[edi + ecx*2],al			// Store even pixel.

			cmp			ecx,[i_x_start]
			je			SKIP_AVERAGE

			movzx		ebx,byte ptr[edi + ecx*2 - 2]	// Get previous even value.

			add			eax,ebx							// Average.

			shr			eax,1
			mov			ebx,[piTable]				// Base of lookup table

			mov			[edi + ecx*2 - 1],al		// Store odd value.

SKIP_AVERAGE:
			inc			ecx
			
			cmp			ecx,[i_x_end]
			jl			X_LOOP	

			femms									// Only needed for debug build.

SKIP_LOOP:
		}
	}

	//*****************************************************************************************
	void CWaveNode::FillTextureMap
	(
		CPArray<uint16> pau2,
		int i_y,
		CVector3<> v3_cam,
		CDir3<> d3_sun
	)
	{
		// Since we read one row and column ahead, i_y cannot be the last row.
		Assert(i_y >= 0);
		Assert(i_y < pWater2->iHeight-1);

		// Texel n comes from water elems n+1 and n+2.
		// So decrement the texture array, and increment the count to avoid asserts.
		pau2.atArray--;
		pau2.uLen++;

		//
		// Start and end points for x.  Since we access both the current and next elements,
		// we go only up to, but not including, iEnd.
		//
		int i_x_start = iColStart(i_y);
		int i_x_end   = iColEnd(i_y);

		// Base address of the water amplitude array.
		TMetres* am_amp = &pWater2->pa2mAmplitude[i_y][0];
		const int i_amp_dy = pWater2->iStride;

		// Copies of items that can't be directly accessed from inline ASM.
		float mIntervalX = pWater2->mIntervalX;
		float mInterval = pWater2->mInterval;
		float fTableAdd = PixelReflectTable.fGetAdd();
		float fTableScale = PixelReflectTable.fGetScale();
		uint16* pu2Table = PixelReflectTable.pu2GetTable();
		uint16* pu2 = pau2.atArray;

		// Constants we need.
		const __int64 pfNegateLow = 0x80000000;

#if (INTENSITY_CLAMP)
		const float fOne = 1.0f;
		const float fNegOne = -1.0f;
#endif

		// Temps because we are out of registers.
		float r_len_add;
		float r_dot_add;

		// Typedefs so that we can access stuff from inline ASM.
		typedef CVector3<> tdCVector3;

		__asm
		{
			mov			ecx,[i_x_start]

			cmp			ecx,[i_x_end]
			jge			SKIP_LOOP

			femms									// Only needed for debug build.

			//
			// Setup for loop.
			//
			mov			edx,[i_amp_dy]
			mov			esi,[am_amp]
			mov			edi,[pu2]
			mov			ebx,[pu2Table]				// Base of lookup table

			lea			edx,[esi + edx*4]			// am_amp + i_amp_dy
			movd		mm0,[esi + ecx*4]			// am_amp[i_x]

			movq		mm2,v3_cam.tX				// v3_cam.tY | v3_cam.tX

			movd		mm3,[mIntervalX]

			mov			eax,ecx						// copy i_x_start

			dec			eax							// -1

			movd		mm4,eax						// i_x_start

			pi2fd		(m4,m4)						// Convert to float

			pfmul		(m4,m3)						// mIntervalX * (i_x_start-1)

			pxor		mm1,mm1						// Clear mm1

			punpckldq	mm4,mm1						// Zero high word of mm4

			pfsub		(m2,m4)						// v3_cam.tX -= mIntervalX * (i_x_start-1)

			pxor		mm3,[pfNegateLow]			// r_dx = -mIntervalX

			movd		mm6,v3_cam.tZ				// v3_cam.tZ

			movq		mm7,mm2						// v3_cam.tY | v3_cam.tX

			psrlq		mm7,32						// v3_cam.tY

			punpckldq	mm7,mm6						// v3_cam.tZ | v3_cam.tY

			movd		mm4,[mInterval]

			pfmul		(m6,m4)						// v3_cam.tZ * mInterval

			movd		[r_dot_add],mm6				// Save for later

			pfmul		(m4,m4)						// mInterval^2

			movd		[r_len_add],mm4				// Save for later

			pfmul		(m7,m7)						// v3_cam.tY^2 | v3_cam.tZ^2

			pfacc		(m7,m7)						// r_cam_len_sqr

			//
			// Loop for each element in x.
			//
			// eax = temp
			// ebx = ReflectTable.satTable
			// ecx = i_x
			// edx = am_amp + i_amp_dy
			// esi = am_amp
			// edi = pau1
			//
			// mm0 = temp, initially = am_amp[i_x]
			// mm1 = temp
			// mm2 = v3_cam.tY | v3_cam.tX
			// mm3 = 0 | r_dx
			// mm4 = temp
			// mm5 = temp
			// mm6 = temp 
			// mm7 = 0 | r_cam_len_sqr
			//
		X_LOOP:
			movq		mm1,mm0						// am_amp[i_x] from previous am_amp[i_x + 1]
			movd		mm0,[esi + ecx*4 + 4]		// am_amp[i_x + 1]

			// Incrment camera x.
			pfadd		(m2,m3)						// camy | camx
			
			punpckldq	mm1,mm1						// am_amp[i_x] | am_amp[i_x]
			punpckldq	mm0,[edx + ecx*4]			// am_amp[i_x + i_amp_dy] | am_amp[i_x + 1]

			pfsub		(m1,m0)						// r_wy | r_wx

			// Calculate inverse of normal length.
			movq		mm4,mm1
			movd		mm5,[r_len_add]

			pfmul		(m4,m4)						// r_wy^2 | r_wx^2
			movq		mm6,mm2						// camy | camx

			pfacc		(m4,m4)						// r_wy^2 + r_wx^2
			pfmul		(m6,m6)						// camx^2

			pfadd		(m4,m5)						// r_wy^2 + r_wx^2 + r_len_add

			pfadd		(m6,m7)						// camx^2 + r_cam_len_sqr

			pfmul		(m4,m6)						// r_len_sqr

			pfrsqrt		(m4,m4)						// r_inv_len
			movd		mm5,[r_dot_add]

			// Dot product of camera and normal.
			pfmul		(m1,m2)						// r_wy * camy | r_wx * camx

#if (INTENSITY_CLAMP)
			movd		mm6,[fOne]
#endif

			pfacc		(m1,m1)						// r_wy * camy + r_wx * camx

			pfadd		(m1,m5)						// r_wy * camy + r_wx * camx + r_dot_add
			movd		mm5,[fTableAdd]

			pfmul		(m1,m4)						// * f_inv_len
			movd		mm4,[fTableScale]

#if (INTENSITY_CLAMP)
			pfmin		(m1,m6)						// Don't go over one.
			movd		mm6,[fNegOne]

			pfmax		(m1,m6)						// Don't go below negative one.
#endif

			// Lookup value in the table.
			pfadd		(m1,m5)						// r_dot + PixelReflectTable.fAdd
			inc			ecx

			pfmul		(m1,m4)						// * PixelReflectTable.fScale

			pf2id		(m1,m1)

			movd		eax,mm1

			mov			ax,[ebx + eax*2]			// Table lookup.
			mov			[edi + ecx*2 - 2],ax		// Store value.
			
			cmp			ecx,[i_x_end]
			jl			X_LOOP	

			femms									// Only needed for debug build.

SKIP_LOOP:
		}
	}

#elif (VER_ASM && TARGET_PROCESSOR != PROCESSOR_K6_3D && bCAM_PIXEL && !bREAL_SPECULAR)

	//*****************************************************************************************
	void CWaveNode::FillIntensityMapInterp
	(
		CPArray<uint8> pau1,
		int i_y,
		CVector3<> v3_cam,
		CDir3<> d3_sun
	)
	{
		// Since we read one row and column ahead, i_y cannot be the last row.
		Assert(i_y >= 0);
		Assert(i_y < pWater2->iHeight-1);

		//
		// Start and end points for x.  Since we access both the current and next elements,
		// we go only up to, but not including, iEnd.
		// Since this row will be interpolated with adjacent rows, make sure to include all
		// relevant elements from those rows.
		//
		int i_x_start = iColStart(i_y);
		if (i_y > 0)
			SetMin(i_x_start, iColStart(i_y-1));
		SetMin(i_x_start, iColStart(i_y+1));

		int i_x_end   = iColEnd(i_y);
		if (i_y > 0)
			SetMax(i_x_end, iColEnd(i_y-1));
		SetMax(i_x_end, iColEnd(i_y+1));

		// Base address of the water amplitude array.
		TMetres* am_amp = &pWater2->pa2mAmplitude[i_y][0];
		const int i_amp_dy = pWater2->iStride;

		// Copies of items that can't be directly accessed from inline ASM.
		float mIntervalX = pWater2->mIntervalX;
		float mInterval = pWater2->mInterval;
		float fTableAdd = ReflectTable.fGetAdd();
		float fTableScale = ReflectTable.fGetScale();
		int* piTable = ReflectTable.piGetTable();
		uint8* pu1 = pau1.atArray;

		// Temps because we are out of registers.
		float r_cam_len_sqr, r_len_add, r_dot_add, r_len_sqr, r_inv_len;
		int i_temp;

		// Typedefs so that we can access stuff from inline ASM.
		typedef CVector3<> tdCVector3;

#if (INTENSITY_CLAMP)
		const float fOne = 1.0f;
		float r_dot;
#endif

		__asm
		{
			mov		ecx,[i_x_start]
			mov		edx,[i_x_end]

			cmp		ecx,edx
			jge		SKIP_LOOP

			//
			// Setup for loop.
			//
			mov		edx,[i_amp_dy]
			mov		esi,[am_amp]

			mov		edi,[pu1]

			mov		ebx,[piTable]			// Base of lookup table

			lea		edx,[esi + edx*4]		// am_amp + i_amp_dy

			fild	[i_x_start]				// Load i_x_start
			fld		float ptr [v3_cam.tY]	// v3_cam.tY
			fmul	st(0),st(0)				// v3_cam.tY^2
			fld		float ptr [v3_cam.tZ]	// v3_cam.tZ
			fmul	st(0),st(0)				// v3_cam.tZ^2
			fld		[mIntervalX]			// mIntervalX v3_cam.tZ^2 v3_cam.tY^2 i_x_start
			fmulp	st(3),st(0)				// v3_cam.tZ^2 v3_cam.tY^2 i_x_start*mIntervalX
			fld		float ptr [v3_cam.tX]	// v3_cam.tX v3_cam.tZ^2 v3_cam.tY^2 i_x_start*mIntervalX
			fsubrp	st(3),st(0)				// v3_cam.tZ^2 v3_cam.tY^2 v3_cam.tX-i_x_start*mIntervalX
			faddp	st(1),st(0)				// v3_cam.tZ^2+v3_cam.tY^2 v3_cam.tX-i_x_start*mIntervalX
			fld		[mInterval]				// mInterval r_cam_len_sqr v3_cam.tX
			fmul	st(0),st(0)				// mInterval*mInterval r_cam_len_sqr v3_cam.tX
			fld		[mInterval]				// mInterval r_len_add r_cam_len_sqr v3_cam.tX
			fmul	float ptr [v3_cam.tZ]	// r_dot_add r_len_add r_cam_len_sqr v3_cam.tX
			fxch	st(2)					// r_cam_len_sqr r_len_add r_dot_add v3_cam.tX
			fstp	[r_cam_len_sqr]			// r_len_add r_dot_add v3_cam.tX
			fstp	[r_len_add]				// r_dot_add v3_cam.tX
			fstp	[r_dot_add]				// v3_cam.tX

			//
			// Loop for each element in x.
			//
			// eax = temp
			// ebx = ReflectTable.satTable
			// ecx = i_x
			// edx = am_amp + i_amp_dy
			// esi = am_amp
			// edi = pau1
			//
		X_LOOP:
			// Get the normal components from gradient.
			fld		dword ptr[esi + ecx*4]		// am_amp[i_x]
			fsub	dword ptr[esi + ecx*4 + 4]	// am_amp[i_x + 1]
			fld		dword ptr[esi + ecx*4]		// am_amp[i_x]
			fsub	dword ptr[edx + ecx*4]		// am_amp[i_x + i_amp_dy]

			fld		st(1)						// r_wx r_wy r_wx cam.tX
			fmul	st(0),st(0)					// r_wx^2 r_wy r_wx cam.tX
			fld		st(1)						// r_wy r_wx^2 r_wy r_wx cam.tX
			fmul	st(0),st(0)					// r_wy^2 r_wx^2 r_wy r_wx cam.tX
			fld		st(4)						// cam.tX r_wy^2 r_wx^2 r_wy r_wx cam.tX
			fmul	st(0),st(0)					// tX^2 r_wy^2 r_wx^2 r_wy r_wx cam.tX
			fxch	st(2)						// r_wx^2 r_wy^2 tX^2 r_wy r_wx cam.tX
			faddp	st(1),st(0)					// r_wx^2+r_wy^2 tX^2 r_wy r_wx cam.tX
			fxch	st(1)						// tX^2 r_wx^2+r_wy^2 r_wy r_wx cam.tX
			fadd	[r_cam_len_sqr]				// r_cam_len_sqr+tX^2 r_wx^2+r_wy^2 r_wy r_wx cam.tX
			fxch	st(3)						// r_wx r_wx^2+r_wy^2 r_wy r_cam_len_sqr+tX^2 cam.tX
			fmul	st(0),st(4)						// r_wx*tX r_wx^2+r_wy^2 r_wy r_cam_len_sqr+tX^2 cam.tX
			fxch	st(1)						// r_wx^2+r_wy^2 r_wx*tX r_wy r_cam_len_sqr+tX^2 cam.tX
			fadd	[r_len_add]					// r_len_sqr r_wx*tX r_wy r_cam_len_sqr+tX^2 cam.tX
			fxch	st(2)						// r_wy r_wx*tX r_len_sqr r_cam_len_sqr+tX^2 cam.tX
			fmul	float ptr [v3_cam.tY]		// r_wy*tY r_wx*tX r_len_sqr r_cam_len_sqr+tX^2 cam.tX
			fxch	st(4)						// cam.tX r_wx*tX r_len_sqr r_cam_len_sqr+tX^2 r_wy*tY
			fsub	[mIntervalX]				// cam.tX r_wx*tX r_len_sqr r_cam_len_sqr+tX^2 r_wy*tY
			fxch	st(2)						// r_len_sqr r_wx*tX cam.tX r_cam_len_sqr+tX^2 r_wy*tY
			fmulp	st(3),st(0)					// r_wx*tX cam.tX r_len_sqr r_wy*tY
			faddp	st(3),st(0)					// cam.tX r_len_sqr r_wx*tX+r_wy*tY
			fxch	st(2)						// r_wx*tX+r_wy*tY r_len_sqr cam.tX
			fxch	st(1)						// r_len_sqr r_wx*tX+r_wy*tY cam.tX
			// stall
			fstp	[r_len_sqr]					// r_wx*tX+r_wy*tY cam.tX
			fadd	[r_dot_add]					// r_wx*tX+r_wy*tY+r_dot_add cam.tX

			mov		ebx,[r_len_sqr]

			// i4_table_index   = if_val & (i4FLOAT_MANT_MASK | BitMask(iFLOAT_EXP_BIT_START));
			mov		eax,ebx
			and		ebx,0x00ffffff

			// i4_table_index >>= iFLOAT_MANT_BIT_WIDTH - iFAST_INV_SQRT_TABLE_SIZE_BITS + 1;
			shr		ebx,iFLOAT_MANT_BIT_WIDTH-iFAST_INV_SQRT_TABLE_SIZE_BITS+1

			// if_val.i4Int &= i4FLOAT_EXP_MASK;
			and		eax,0x7f800000

			// if_val = ((3 * iFLOAT_EXP_BIAS - 1) << (iFLOAT_EXP_BIT_START - 1)) - (if_val >> 1);
			shr		eax,1
			mov		esi,380 << 22

			sub		esi,eax

			// if_val.i4Int &= i4FLOAT_EXP_MASK;
			and		esi,0x7f800000

			// if_val.i4Int |= ai4InvSqrtMantissaTable[i4_table_index];
			mov		ebx,ai4InvSqrtMantissaTable[ebx*4]
			or		esi,ebx

			mov		[r_inv_len],esi
	
			fmul	[r_inv_len]					// (r_wx*tX+r_wy*tY+r_dot_add)*r_inv_len cam.tX

#if (INTENSITY_CLAMP)
			fld		[fTableAdd]					// fTableAdd r_dot cam.tX
			fxch	st(1)						// r_dot fTableAdd cam.tX

			mov		ebx,[fOne]

			fstp	[r_dot]						// fTableAdd cam.tX

			mov		eax,[r_dot]

			and		eax,0x7fffffff				// fabs(r_dot)

			cmp		eax,ebx						// fabs(r_dot) > 1 ?
			jle		NO_CLAMP

			mov		eax,[r_dot]
			and		eax,0x80000000				// Sign of r_dot
			or		ebx,eax						// Combine with maximum.
			mov		[r_dot],ebx

NO_CLAMP:
			fadd	[r_dot]						// r_dot + ReflectTable.fAdd
#else
			// stall
			fadd	[fTableAdd]					// r_dot + ReflectTable.fAdd
#endif
			// stall
			fmul	[fTableScale]				// * ReflectTable.fScale

			mov		esi,[am_amp]				// Re-load am_amp poniter.
			mov		ebx,[piTable]				// Re-load Base of lookup table

			fistp	[i_temp]					// FPU is set to truncate.

			mov		eax,[i_temp]				// Load table index.

			mov		eax,[ebx + eax*4]			// Table lookup.

			mov		[edi + ecx*2],al			// Store even value.
			xor		ebx,ebx

			cmp		ecx,[i_x_start]
			je		SKIP_AVERAGE

			mov		bl,[edi + ecx*2 - 2]		// Get previous even value.

			add		eax,ebx						// Average.

			shr		eax,1

			mov		[edi + ecx*2 - 1],al		// Store odd value.

SKIP_AVERAGE:
			inc		ecx							// Increment i_x
			mov		eax,[i_x_end]

			cmp		ecx,eax
			jl		X_LOOP

			fcomp	st(0)						// Get rid of v3_cam.tX
SKIP_LOOP:
		}
	}

	//*****************************************************************************************
	void CWaveNode::FillTextureMap
	(
		CPArray<uint16> pau2,
		int i_y,
		CVector3<> v3_cam,
		CDir3<> d3_sun
	)
	{
		// Since we read one row and column ahead, i_y cannot be the last row.
		Assert(i_y >= 0);
		Assert(i_y < pWater2->iHeight-1);

		// Texel n comes from water elems n+1 and n+2.
		// So decrement the texture array, and increment the count to avoid asserts.
		pau2.atArray--;
		pau2.uLen++;

		//
		// Start and end points for x.  Since we access both the current and next elements,
		// we go only up to, but not including, iEnd.
		//
		int i_x_start = iColStart(i_y);
		int i_x_end   = iColEnd(i_y);

		// Base address of the water amplitude array.
		TMetres* am_amp = &pWater2->pa2mAmplitude[i_y][0];
		const int i_amp_dy = pWater2->iStride;

		// Copies of items that can't be directly accessed from inline ASM.
		float mIntervalX = pWater2->mIntervalX;
		float mInterval = pWater2->mInterval;
		float fTableAdd = PixelReflectTable.fGetAdd();
		float fTableScale = PixelReflectTable.fGetScale();
		uint16* pu2Table = PixelReflectTable.pu2GetTable();
		uint16* pu2 = pau2.atArray;

		// Temps because we are out of registers.
		float r_cam_len_sqr, r_len_add, r_dot_add, r_len_sqr, r_inv_len;
		int i_temp;

		// Typedefs so that we can access stuff from inline ASM.
		typedef CVector3<> tdCVector3;

#if (INTENSITY_CLAMP)
		const float fOne = 1.0f;
		float r_dot;
#endif

		__asm
		{
			mov		ecx,[i_x_start]
			mov		edx,[i_x_end]

			cmp		ecx,edx
			jge		SKIP_LOOP

			//
			// Setup for loop.
			//
			mov		edx,[i_amp_dy]
			mov		esi,[am_amp]

			mov		edi,[pu2]

			mov		ebx,[pu2Table]			// Base of lookup table

			lea		edx,[esi + edx*4]		// am_amp + i_amp_dy

			fild	[i_x_start]				// Load i_x_start
			fld		float ptr [v3_cam.tY]	// v3_cam.tY
			fmul	st(0),st(0)				// v3_cam.tY^2
			fld		float ptr [v3_cam.tZ]	// v3_cam.tZ
			fmul	st(0),st(0)				// v3_cam.tZ^2
			fld		[mIntervalX]			// mIntervalX v3_cam.tZ^2 v3_cam.tY^2 i_x_start
			fmulp	st(3),st(0)				// v3_cam.tZ^2 v3_cam.tY^2 i_x_start*mIntervalX
			fld		float ptr [v3_cam.tX]	// v3_cam.tX v3_cam.tZ^2 v3_cam.tY^2 i_x_start*mIntervalX
			fsubrp	st(3),st(0)				// v3_cam.tZ^2 v3_cam.tY^2 v3_cam.tX-i_x_start*mIntervalX
			faddp	st(1),st(0)				// v3_cam.tZ^2+v3_cam.tY^2 v3_cam.tX-i_x_start*mIntervalX
			fld		[mInterval]				// mInterval r_cam_len_sqr v3_cam.tX
			fmul	st(0),st(0)				// mInterval*mInterval r_cam_len_sqr v3_cam.tX
			fld		[mInterval]				// mInterval r_len_add r_cam_len_sqr v3_cam.tX
			fmul	float ptr [v3_cam.tZ]	// r_dot_add r_len_add r_cam_len_sqr v3_cam.tX
			fxch	st(2)					// r_cam_len_sqr r_len_add r_dot_add v3_cam.tX
			fstp	[r_cam_len_sqr]			// r_len_add r_dot_add v3_cam.tX
			fstp	[r_len_add]				// r_dot_add v3_cam.tX
			fstp	[r_dot_add]				// v3_cam.tX

			//
			// Loop for each element in x.
			//
			// eax = temp
			// ebx = ReflectTable.satTable
			// ecx = i_x
			// edx = am_amp + i_amp_dy
			// esi = am_amp
			// edi = pau1
			//
		X_LOOP:
			// Get the normal components from gradient.
			fld		dword ptr[esi + ecx*4]		// am_amp[i_x]
			fsub	dword ptr[esi + ecx*4 + 4]	// am_amp[i_x + 1]
			fld		dword ptr[esi + ecx*4]		// am_amp[i_x]
			fsub	dword ptr[edx + ecx*4]		// am_amp[i_x + i_amp_dy]

			fld		st(1)						// r_wx r_wy r_wx cam.tX
			fmul	st(0),st(0)					// r_wx^2 r_wy r_wx cam.tX
			fld		st(1)						// r_wy r_wx^2 r_wy r_wx cam.tX
			fmul	st(0),st(0)					// r_wy^2 r_wx^2 r_wy r_wx cam.tX
			fld		st(4)						// cam.tX r_wy^2 r_wx^2 r_wy r_wx cam.tX
			fmul	st(0),st(0)					// tX^2 r_wy^2 r_wx^2 r_wy r_wx cam.tX
			fxch	st(2)						// r_wx^2 r_wy^2 tX^2 r_wy r_wx cam.tX
			faddp	st(1),st(0)					// r_wx^2+r_wy^2 tX^2 r_wy r_wx cam.tX
			fxch	st(1)						// tX^2 r_wx^2+r_wy^2 r_wy r_wx cam.tX
			fadd	[r_cam_len_sqr]				// r_cam_len_sqr+tX^2 r_wx^2+r_wy^2 r_wy r_wx cam.tX
			fxch	st(3)						// r_wx r_wx^2+r_wy^2 r_wy r_cam_len_sqr+tX^2 cam.tX
			fmul	st(0),st(4)					// r_wx*tX r_wx^2+r_wy^2 r_wy r_cam_len_sqr+tX^2 cam.tX
			fxch	st(1)						// r_wx^2+r_wy^2 r_wx*tX r_wy r_cam_len_sqr+tX^2 cam.tX
			fadd	[r_len_add]					// r_len_sqr r_wx*tX r_wy r_cam_len_sqr+tX^2 cam.tX
			fxch	st(2)						// r_wy r_wx*tX r_len_sqr r_cam_len_sqr+tX^2 cam.tX
			fmul	float ptr [v3_cam.tY]		// r_wy*tY r_wx*tX r_len_sqr r_cam_len_sqr+tX^2 cam.tX
			fxch	st(4)						// cam.tX r_wx*tX r_len_sqr r_cam_len_sqr+tX^2 r_wy*tY
			fsub	[mIntervalX]				// cam.tX r_wx*tX r_len_sqr r_cam_len_sqr+tX^2 r_wy*tY
			fxch	st(2)						// r_len_sqr r_wx*tX cam.tX r_cam_len_sqr+tX^2 r_wy*tY
			fmulp	st(3),st(0)					// r_wx*tX cam.tX r_len_sqr r_wy*tY
			faddp	st(3),st(0)					// cam.tX r_len_sqr r_wx*tX+r_wy*tY
			fxch	st(2)						// r_wx*tX+r_wy*tY r_len_sqr cam.tX
			fxch	st(1)						// r_len_sqr r_wx*tX+r_wy*tY cam.tX
			// stall
			fstp	[r_len_sqr]					// r_wx*tX+r_wy*tY cam.tX
			fadd	[r_dot_add]					// r_wx*tX+r_wy*tY+r_dot_add cam.tX

			mov		ebx,[r_len_sqr]

			// i4_table_index   = if_val & (i4FLOAT_MANT_MASK | BitMask(iFLOAT_EXP_BIT_START));
			mov		eax,ebx
			and		ebx,0x00ffffff

			// i4_table_index >>= iFLOAT_MANT_BIT_WIDTH - iFAST_INV_SQRT_TABLE_SIZE_BITS + 1;
			shr		ebx,iFLOAT_MANT_BIT_WIDTH-iFAST_INV_SQRT_TABLE_SIZE_BITS+1

			// if_val.i4Int &= i4FLOAT_EXP_MASK;
			and		eax,0x7f800000

			// if_val = ((3 * iFLOAT_EXP_BIAS - 1) << (iFLOAT_EXP_BIT_START - 1)) - (if_val >> 1);
			shr		eax,1
			mov		esi,380 << 22

			sub		esi,eax

			// if_val.i4Int &= i4FLOAT_EXP_MASK;
			and		esi,0x7f800000

			// if_val.i4Int |= ai4InvSqrtMantissaTable[i4_table_index];
			mov		ebx,ai4InvSqrtMantissaTable[ebx*4]
			or		esi,ebx

			mov		[r_inv_len],esi
	
			fmul	[r_inv_len]					// (r_wx*tX+r_wy*tY+r_dot_add)*r_inv_len cam.tX

#if (INTENSITY_CLAMP)
			fld		[fTableAdd]					// fTableAdd r_dot cam.tX
			fxch	st(1)						// r_dot fTableAdd cam.tX

			mov		ebx,[fOne]

			fstp	[r_dot]						// fTableAdd cam.tX

			mov		eax,[r_dot]

			and		eax,0x7fffffff				// fabs(r_dot)

			cmp		eax,ebx						// fabs(r_dot) > 1 ?
			jle		NO_CLAMP

			mov		eax,[r_dot]
			and		eax,0x80000000				// Sign of r_dot
			or		ebx,eax						// Combine with maximum.
			mov		[r_dot],ebx

NO_CLAMP:
			fadd	[r_dot]						// r_dot + ReflectTable.fAdd
#else
			// stall
			fadd	[fTableAdd]					// r_dot + ReflectTable.fAdd
#endif
			// stall
			fmul	[fTableScale]				// * ReflectTable.fScale

			mov		esi,[am_amp]				// Re-load am_amp poniter.

			mov		ebx,[pu2Table]				// Re-load Base of lookup table
			inc		ecx							// Increment i_x

			fistp	[i_temp]					// FPU is set to truncate.

			mov		eax,[i_temp]

			mov		ax,[ebx + eax*2]			// Table lookup.

			mov		[edi + ecx*2 - 2],ax		// Store value.
			
			cmp		ecx,[i_x_end]
			jl		X_LOOP

			fcomp	st(0)						// Get rid of v3_cam.tX
SKIP_LOOP:
		}
	}

#else

	//*****************************************************************************************
	void CWaveNode::FillIntensityMapInterp
	(
		CPArray<uint8> pau1,
		int i_y,
		CVector3<> v3_cam,
		CDir3<> d3_sun
	)
	{
		// Since we read one row and column ahead, i_y cannot be the last row.
		Assert(i_y >= 0);
		Assert(i_y < pWater2->iHeight-1);

		//
		// Start and end points for x.  Since we access both the current and next elements,
		// we go only up to, but not including, iEnd.
		// Since this row will be interpolated with adjacent rows, make sure to include all
		// relevant elements from those rows.
		//
		int i_x_start = iColStart(i_y);
		if (i_y > 0)
			SetMin(i_x_start, iColStart(i_y-1));
		SetMin(i_x_start, iColStart(i_y+1));

		int i_x_end   = iColEnd(i_y);
		if (i_y > 0)
			SetMax(i_x_end, iColEnd(i_y-1));
		SetMax(i_x_end, iColEnd(i_y+1));

		// Base address of the water amplitude array.
		TMetres* am_amp = &pWater2->pa2mAmplitude[i_y][0];
		const int i_amp_dy = pWater2->iStride;

		#if bCAM_PIXEL
			v3_cam.tX -= pWater2->mIntervalX * (i_x_start-1);

			// Find length squared, excluding X component.
			TReal r_cam_len_sqr = v3_cam.tY * v3_cam.tY + v3_cam.tZ * v3_cam.tZ;
			TReal r_dx = -pWater2->mIntervalX;
		#endif

		TReal r_len_add	= Square(pWater2->mInterval);
		TReal r_dot_add	= pWater2->mInterval * v3_cam.tZ;

		// Calculate the row of intensities.
		for (int i_x = i_x_start; i_x < i_x_end; ++i_x)
		{
			// Get the normal components from gradient.
			TReal r_wx = am_amp[i_x] - am_amp[i_x + 1];
			TReal r_wy = am_amp[i_x] - am_amp[i_x + i_amp_dy];

			// Calculate inverse length of the corresponding normal.
			TReal r_len_sqr = r_wx*r_wx + r_wy*r_wy + r_len_add;
			#if bCAM_PIXEL
				v3_cam.tX += r_dx;
				r_len_sqr *= r_cam_len_sqr + v3_cam.tX * v3_cam.tX;
			#endif
			TReal r_inv_len = fInvSqrtEst(r_len_sqr);

			// Take dot-product of this normal with the camera normal.
			TReal r_dot = (r_wx*v3_cam.tX + r_wy*v3_cam.tY + r_dot_add) * r_inv_len;

			#if bREAL_SPECULAR
				//
				// Phong model: produces correct spherical sun reflections.
				// Reflection R of C about normal N is:
				//		R = 2 (N*C) N - C
				//
				CDir3<> d3_bump(r_wx, r_wy, r_interval);
				CDir3<> d3_cam = v3_cam;
				CDir3<> d3_reflect(d3_bump * (d3_bump * d3_cam * 2.0) - d3_cam, true);

				// Get the intensity value for the sun, max with reflective value.
				if (d3_sun * d3_reflect >= angwSUN)
					// Set minimum angle, which produces maximum reflection value.
					r_dot = 0.0;
			#endif

			#if (INTENSITY_CLAMP)
				// Clamp out of range values.
				if (r_dot > 1.0f) r_dot = 1.0f;
				if (r_dot < -1.0f) r_dot = -1.0f;
			#endif

			pau1[i_x * 2] = ReflectTable(r_dot);
			Assert(bWithin(pau1[i_x*2], 0, 31));
		}

		// Do every odd element relative to the first calculated element.
		// Interpolate water in x.
		for (i_x = i_x_start; i_x < i_x_end - 1; ++i_x)
		{
			// Get the interpolated intensity value.
			pau1[i_x * 2 + 1] = (pau1[i_x * 2] + pau1[i_x * 2 + 2]) >> 1;
		}
	}

	//*****************************************************************************************
	void CWaveNode::FillTextureMap
	(
		CPArray<uint16> pau2,
		int i_y,
		CVector3<> v3_cam,
		CDir3<> d3_sun
	)
	{
		// Since we read one row and column ahead, i_y cannot be the last row.
		Assert(i_y >= 0);
		Assert(i_y < pWater2->iHeight-1);

		// Texel n comes from water elems n+1 and n+2.
		// So decrement the texture array, and increment the count to avoid asserts.
		pau2.atArray--;
		pau2.uLen++;

		//
		// Start and end points for x.  Since we access both the current and next elements,
		// we go only up to, but not including, iEnd.
		//
		int i_x_start = iColStart(i_y);
		int i_x_end   = iColEnd(i_y);

		// Base address of the water amplitude array.
		TMetres* am_amp = &pWater2->pa2mAmplitude[i_y][0];
		const int i_amp_dy = pWater2->iStride;

		#if bCAM_PIXEL
			v3_cam.tX -= pWater2->mIntervalX * (i_x_start-1);

			// Find length squared, excluding X component.
			TReal r_cam_len_sqr = v3_cam.tY * v3_cam.tY + v3_cam.tZ * v3_cam.tZ;
			TReal r_dx = -pWater2->mIntervalX;
		#endif

		TReal r_len_add	= Square(pWater2->mInterval);
		TReal r_dot_add	= pWater2->mInterval * v3_cam.tZ;

		// Calculate the row of intensities.
		for (int i_x = i_x_start; i_x < i_x_end; ++i_x)
		{
			// Get the normal components from gradient.
			TReal r_wx = am_amp[i_x] - am_amp[i_x + 1];
			TReal r_wy = am_amp[i_x] - am_amp[i_x + i_amp_dy];

			// Calculate inverse length of the corresponding normal.
			TReal r_len_sqr = r_wx*r_wx + r_wy*r_wy + r_len_add;
			#if bCAM_PIXEL
				v3_cam.tX += r_dx;
				r_len_sqr *= r_cam_len_sqr + v3_cam.tX * v3_cam.tX;
			#endif
			TReal r_inv_len = fInvSqrtEst(r_len_sqr);

			// Take dot-product of this normal with the camera normal.
			TReal r_dot = (r_wx*v3_cam.tX + r_wy*v3_cam.tY + r_dot_add) * r_inv_len;

			#if bREAL_SPECULAR
				//
				// Phong model: produces correct spherical sun reflections.
				// Reflection R of C about normal N is:
				//		R = 2 (N*C) N - C
				//
				CDir3<> d3_bump(r_wx, r_wy, r_interval);
				CDir3<> d3_cam = v3_cam;
				CDir3<> d3_reflect(d3_bump * (d3_bump * d3_cam * 2.0) - d3_cam, true);

				// Get the intensity value for the sun, max with reflective value.
				if (d3_sun * d3_reflect >= angwSUN)
					// Set minimum angle, which produces maximum reflection value.
					r_dot = 0.0;
			#endif

			#if (INTENSITY_CLAMP)
				// Clamp out of range values.
				if (r_dot > 1.0f) r_dot = 1.0f;
				if (r_dot < -1.0f) r_dot = -1.0f;
			#endif

			pau2[i_x] = PixelReflectTable(r_dot);
		}
	}

#endif

	//*****************************************************************************************
	void CWaveNode::FillWaterTexture(const CCamera& cam, const CPlacement3<>& p3_cam_obj, const CDir3<>& d3_sun)
	{
		if (!pWater2)
			return;

		//
		// Water to texture correspondence:
		// Each texel is derived from an adjacent pair of water elements.
		// Without interpolation:
		//		Texture(n) <-- Water(n+1, n+2)
		// With interpolation:
		//		Texture(2*n) <-- Water(n+1, n+2)
		//		Texture(2*n+1) <-- Texture(2*n, 2*n+2)
		//

		// All index variables herein refer to water elements.

		// Get the current raster used for the texture. Currently there can be only one.
		rptr<CRaster> pras = prasTexture;
		Assert(pras);

		//
		// Get the camera to texture conversion information.
		//

		// Find vector of water centre to camera, in water's frame.
		CVector3<> v3_cam = p3_cam_obj.v3Pos;

		//
		// Adjust the normal to ensure the viewer/water angle is not too small.
		// This prevents still water from appearing entirely white.
		//
		SetMax(v3_cam.tZ, rMIN_CAMERA_Z * v3_cam.tLen());

		#if bCAM_PIXEL
			// Offset to water's corner. Do not normalise, as that is done per pixel.
			v3_cam.tX -= rcObject.tX0();
			v3_cam.tY -= rcObject.tY0();
		#else
			// Normalise for dot-product.
			v3_cam.Normalise();
		#endif

		// Get the starting and ending rows.  Since we access both current and next row/column,
		// we go up to but not including iRowEnd.
		int i_y		= iRowStart();
		int i_y_end	= iRowEnd();
		if (i_y >= i_y_end)
			return;

		// Calculate intensity values for the first row into the buffer.
		#if bCAM_PIXEL
			v3_cam.tY -= pWater2->mIntervalY * (i_y-1);
		#endif

		#if (VER_ASM)
			#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)
				float mIntervalY = pWater2->mIntervalY;
				__asm femms								// Clear MMX state and ensure fast switch
			#else
				CFPUState fpus;
				fpus.SetTruncate();						// Set the FPU state to truncate.
			#endif
		#endif // if (VER_ASM)

		if (bInterpActual)
		{
			//
			// Create buffers to output the intermediate intensity values.
			//
			int i_buffer_width	= pras->iWidth + 2;
			uint8* pu1_buffer_0	= (uint8*)_alloca(i_buffer_width + 16);

			// Ensure eight byte alignment and at least one byte of left-side padding.
			pu1_buffer_0 = ((uint8*)(uint32(pu1_buffer_0 + 8) & 0xFFFFFFF8));

			uint8* pu1_buffer_1 = (uint8*)_alloca(i_buffer_width + 16);
			pu1_buffer_1 = ((uint8*)(uint32(pu1_buffer_1 + 8) & 0xFFFFFFF8));

			// Get the row pointer for the first row of the output raster.
			Assert(i_y > 0);
			uint16* pu2 = (uint16*)pras->pAddress(0, (i_y-1) * 2) - 2;

			FillIntensityMapInterp(CPArray<uint8>(i_buffer_width, pu1_buffer_0), i_y, v3_cam, d3_sun);

			#if bCAM_PIXEL
				#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)
					// Avoid x87 code.
					__asm
					{
						femms						// Only needed for debug build.

						movd	mm0,v3_cam.tY
						movd	mm1,mIntervalY

						pfsub	(m0,m1)

						movd	v3_cam.tY,mm0

						femms						// Only needed for debug build.
					}
				#else
					v3_cam.tY -= pWater2->mIntervalY;
				#endif
			#endif

			// Convert to final format for every row.
			for (;;)
			{
				// Start and end points for x.
				int i_x_start = Min(iColStart(i_y), iColStart(i_y+1)) * 2;
				int i_x_end   = Max(iColEnd  (i_y), iColEnd  (i_y+1)) * 2 - 1;

				uint16* pu2_IntensityToAlpha = sau2PixelTable.atArray;
				uint8*  pu1_buffer_0_start = &pu1_buffer_0[i_x_start];
				uint16* pu2_buffer_start = &pu2[i_x_start];

				//
				// Copy and convert the first row.
				//
				//
				// Translate intensities to alpha pixels.
				//
				#if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)
				{

					__asm 
					{
					mov		ecx,[i_x_end]
					sub		ecx,[i_x_start]					;ecx= i_x_end - i_x_start

					mov		esi,[pu1_buffer_0_start]
					mov		eax,[pu2_IntensityToAlpha]
					mov		ebx,[pu2_buffer_start]

					jle		SkipLoop1						;skip if empty row

					movzx	edx,BYTE PTR[esi + ecx - 1]		;edx= first intensity of row
					jmp		Loop1

					align	16
				Loop1:
					mov		di,WORD PTR[eax + edx*2]		;di= alpha of current intensity

					movzx	edx,BYTE PTR[esi + ecx - 2]		;edx= next intensity of row

					mov		WORD PTR[ebx + ecx*2 - 2],di	;write out alpha for current intensity

					loop	Loop1							;iterate from end to beginning of 
															;   row buffer
				SkipLoop1:
					}
				}
				// elif (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)
				#else 
				{
					for (int i_x = i_x_start; i_x < i_x_end; ++i_x)
						pu2[i_x] = sau2PixelTable[ (int)pu1_buffer_0[i_x] ];
				}
				#endif

				//
				// Get the next row of intensities.
				//
				++i_y;

				// Break if there are no more rows.
				if (i_y >= i_y_end)
					break;

				//
				// Interpolate intensities with next row.
				//
				FillIntensityMapInterp(CPArray<uint8>(i_buffer_width, pu1_buffer_1), i_y, v3_cam, d3_sun);

				// Set to the next raster line.
				pu2 += pras->iLinePixels;

				#if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)
				{
					uint8* pu1_buffer_1_start = &pu1_buffer_1[i_x_start];
						   pu2_buffer_start = &pu2[i_x_start];

					__asm
					{
					mov		ecx,[i_x_end]
					sub		ecx,[i_x_start]					;ecx= i_x_end - i_x_start

					mov		esi,[pu1_buffer_0_start]
					mov		edi,[pu1_buffer_1_start]
					mov		eax,[pu2_IntensityToAlpha]
					mov		ebx,[pu2_buffer_start]

					jle		SkipLoop2						;skip if empty row

					mov		dl,BYTE PTR[esi + ecx - 1]
					add		dl,BYTE PTR[edi + ecx - 1]		;dl= 2 * first interpolated intensity
					jmp		Loop2

					align	16
				Loop2:
					movzx	edx,dl

					and		edx,-2							;edx= 2 * current interpolated intensity

					mov		dx,WORD PTR[eax + edx*1]		;dx= alpha of current intensity

					mov		WORD PTR[ebx + ecx*2 - 2],dx	;write out this alpha

					nop										;1-byte NOOP to avoid degraded predecode
					mov		dl,BYTE PTR[esi + ecx - 2]

					add		dl,BYTE PTR[edi + ecx - 2]		;dl= 2 * next interpolated intensity
					loop	Loop2							;iterate from end to beginning of 
															;   row buffer
				SkipLoop2:
					}
				}
				// if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)
				#elif (VER_ASM && TARGET_PROCESSOR == PROCESSOR_PENTIUM_PRO)
				{
					__asm
					{
					mov		ecx,i_x_start
					mov		edi,pu2

					mov		esi,pu1_buffer_0
					mov		edx,pu1_buffer_1

					mov		ebx,i_x_end

					jmp		ENTER_AWATER_INTERP
					ALIGN	16

				AWATER_INTERP:
					xor		eax,eax
					xor		ebx,ebx
					mov		al,[esi + ecx]

					mov		bl,[edx + ecx]
					mov		esi,pu2_IntensityToAlpha
					add		eax,ebx							// Sum intensities.

					shr		eax,1							// Get rid of fractional bit.
					mov		ebx,i_x_end
					mov		ax,[esi + eax*2]

					mov		[edi + ecx*2],ax
					mov		esi,pu1_buffer_0
					inc		ecx

				ENTER_AWATER_INTERP:
					cmp		ecx,ebx
					jl		AWATER_INTERP
					}
				}
				// #elif (VER_ASM && TARGET_PROCESSOR == PROCESSOR_PENTIUM_PRO)
				#elif (VER_ASM)		// Pentium ASM here temporarily broken.
				{
					__asm
					{
					mov		ecx,i_x_start
					mov		edi,pu2

					mov		esi,pu1_buffer_0
					mov		edx,pu1_buffer_1

					lea		edi,[edi + ecx*2]
					mov		ebx,i_x_end

					jmp		ENTER_AWATER_INTERP

				AWATER_INTERP:
					xor		eax,eax
					xor		ebx,ebx

					mov		al,[esi + ecx]
					mov		bl,[edx + ecx]

					mov		esi,pu2_IntensityToAlpha
					add		eax,ebx							// Sum intensities.

					and		eax,0xfffffffe					// Get rid of fractional bit.
					mov		ebx,i_x_end

					add		esi,eax

					movsw									// Move a word.

					mov		esi,pu1_buffer_0
					inc		ecx

				ENTER_AWATER_INTERP:
					cmp		ecx,ebx
					jl		AWATER_INTERP
					}
				}
				// #elif (VER_ASM) 
				#else
				{
					for (int i_x = i_x_start; i_x < i_x_end; ++i_x)
						pu2[i_x] = sau2PixelTable[(pu1_buffer_0[i_x] + pu1_buffer_1[i_x]) >> 1];
				}
				#endif

				//
				// Exchange buffer pointers, effectively saving the newly-generated row of
				// intensities.
				//
				Swap(pu1_buffer_0, pu1_buffer_1);

				#if bCAM_PIXEL
					#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)
						// Avoid x87 code.
						__asm
						{
							femms						// Only needed for debug build.

							movd	mm0,v3_cam.tY
							movd	mm1,mIntervalY

							pfsub	(m0,m1)

							movd	v3_cam.tY,mm0

							femms						// Only needed for debug build.
						}
					#else
						v3_cam.tY -= pWater2->mIntervalY;
					#endif
				#endif

				// Set to the next raster line.
				pu2 += pras->iLinePixels;
			}
		} // if (bInterpActual)
		else
		{
			// Texture elem n comes from water elems n+1 and n+2.
			Assert(i_y > 0);
			uint16* pu2 = (uint16*)pras->pAddress(0, i_y-1);

			// Convert to final format for every even row.
			for (; i_y < i_y_end; i_y++)
			{
				FillTextureMap(CPArray<uint16>(pras->iWidth, pu2), i_y, v3_cam, d3_sun);

				#if bCAM_PIXEL
					v3_cam.tY -= pWater2->mIntervalY;
				#endif

				// Set to the next raster line.
				pu2 += pras->iLinePixels;
			}
		}

#if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)
		__asm femms										// clear MMX state
#endif

		// Upload the new texture.
		if (d3dDriver.bD3DWater())
		{
			srd3dRenderer.SetD3DMode(ed3drSOFTWARE_LOCK);
			prasMainScreen->Lock();
			prasMainScreen->Unlock();
			Verify(prasTexture->bUpload());
		}
	}
	
	//******************************************************************************************
	void CWaveNode::SetNeighbours(CWaveNode* pwn_0, CWaveNode* pwn_1, CWaveNode* pwn_2, CWaveNode* pwn_3)
	{
		// Handle any re-grouping for water.
		if (pWater2)
		{
			if (pwn_0 && pwn_0->pWater2)
				pWater2->SetNeighbour(0, pwn_0->pWater2);
			else
				pWater2->SetNeighbour(0, 0);

			if (pwn_1 && pwn_1->pWater2)
				pWater2->SetNeighbour(1, pwn_1->pWater2);
			else
				pWater2->SetNeighbour(1, 0);

			if (pwn_2 && pwn_2->pWater2)
				pWater2->SetNeighbour(2, pwn_2->pWater2);
			else
				pWater2->SetNeighbour(2, 0);

			if (pwn_3 && pwn_3->pWater2)
				pWater2->SetNeighbour(3, pwn_3->pWater2);
			else
				pWater2->SetNeighbour(3, 0);
		}
	}

	//******************************************************************************************
	TReal CWaveNode::rWaterHeight(const CVector2<>& v2_object) const
	{
		if (!pWater2)
			// Not made yet.
			return rWATER_NONE;

		Assert(rcObject.bContains(v2_object));

		// Convert to water elements.
		CVector2<> v2_model = v2_object * tlr2ObjectModel;

		int i_x = iTrunc(v2_model.tX);
		int i_y = iTrunc(v2_model.tY);

		if (pWater2->CSimWaveVar2D::bElementActive(i_y, i_x))
		{
			// The point is within the rectangle, and above the terrain.
			return pWater2->pa2mAmplitude[i_y][i_x];
		}
		else
			return rWATER_NONE;
	}

	//******************************************************************************************
	void CWaveNode::CreateDisturbance(CVector2<> v2_obj, TReal r_radius, TReal r_height,
		bool b_add)
	{
		if (!pWater2)
			return;

		// Translate to model coords.
		v2_obj *= tlr2ObjectModel;

		// Now scale back to world size, because that's what CreateDisturbance expects.
		pWater2->CreateDisturbance
		(
			v2_obj.tY * pWater2->mIntervalX, v2_obj.tX * pWater2->mIntervalY, 
			r_radius, r_height, b_add
		);
	}

	//*****************************************************************************************
	void CWaveNode::DrawPhysics(CDraw& draw, CCamera& cam, const CTransform3<>& tf3_shape_screen) const
	{
#if bVER_BONES()
		//
		// Draw a wireframe of the wave heights.
		//
		if (!pWater2)
			return;

		// Override colour to something watery.
		draw.Colour(CColour(0.0, 0.3, 0.8));

		int i_incr = Max(pWater2->iWidth / iBONES_MAX, iBONES_INCR);

		// Draw only at integrating limits.
		for (int i_y = iRowStart(); i_y < iRowEnd(); i_y += i_incr)
		{
			for (int i_x = iColStart(i_y); i_x < iColEnd(i_y); i_x += i_incr)
			{
				// Calculate the object position of this point.
				CVector3<> v3_obj = CVector2<>(i_x + 0.5, i_y + 0.5) * tlr2ModelObject;

				// Add wave height.
				v3_obj.tZ += pWater2->pa2mAmplitude[i_y][i_x];

//				draw.Point3D(v3_obj * tf3_shape_screen);

				if (i_x == iColStart(i_y))
					draw.MoveTo3D(v3_obj * tf3_shape_screen);
				else
					draw.LineTo3D(v3_obj * tf3_shape_screen);
			}
		}
#endif
	}

//******************************************************************************************
//
// class CWaterNode implementation.
//

	//******************************************************************************************
	CWaterNode::CWaterNode(CWaterNode* pwn_parent, CRectangle<> rc_object)
		: pwnParent(pwn_parent), CWaveNode(rc_object)
	{
	}

	//******************************************************************************************
	CWaterNode::~CWaterNode()
	{
	}

	//******************************************************************************************
	CWaveNode* CWaterNode::pwnFindNode(const CVector2<>& v2_object)
	{
		Assert(rcObject.bContains(v2_object));

		if (!bHasChildren())
			// Leaf node, return water node.
			return this;

		// Determine which sub-branch it is in.
		if (v2_object.tX >= rcObject.tXMid())
		{
			if (v2_object.tY >= rcObject.tYMid())
				return apwnChildren[2]->pwnFindNode(v2_object);
			else
				return apwnChildren[1]->pwnFindNode(v2_object);
		}
		else
		{
			if (v2_object.tY >= rcObject.tYMid())
				return apwnChildren[3]->pwnFindNode(v2_object);
			else
				return apwnChildren[0]->pwnFindNode(v2_object);
		}
	}

	//******************************************************************************************
	void CWaterNode::Subdivide()
	{
		if (bHasChildren())
			return;

		//
		// Create 4 children, with appropriate extent rectangles.
		// They do not have water nodes yet.
		//
		apwnChildren[0] = new CWaterNode
		(
			this,
			CRectangle<>(rcObject.tX0(), rcObject.tY0(), rcObject.tWidth()/2, rcObject.tHeight()/2)
		);
		apwnChildren[1] = new CWaterNode
		(
			this,
			CRectangle<>(rcObject.tXMid(), rcObject.tY0(), rcObject.tWidth()/2, rcObject.tHeight()/2)
		);
		apwnChildren[2] = new CWaterNode
		(
			this,
			CRectangle<>(rcObject.tXMid(), rcObject.tYMid(), rcObject.tWidth()/2, rcObject.tHeight()/2)
		);
		apwnChildren[3] = new CWaterNode
		(
			this,
			CRectangle<>(rcObject.tX0(), rcObject.tYMid(), rcObject.tWidth()/2, rcObject.tHeight()/2)
		);
	}

	//******************************************************************************************
	void CWaterNode::Decimate()
	{
		apwnChildren[0] = 0;
		apwnChildren[1] = 0;
		apwnChildren[2] = 0;
		apwnChildren[3] = 0;
	}

	//******************************************************************************************
	void CWaterNode::UpdateRes(const CEntityWater* petw_owner, const CCamera& cam, const CVector3<>& v3_cam_obj, 
		CPArray2<TMetres>& pa2m_depth, const CTransLinear2<>& tlr2_depth)
	{
		if (!bHasChildren())
		{
			// Update actual water node.
			CWaveNode::UpdateRes(petw_owner, cam, v3_cam_obj, pa2m_depth, tlr2_depth);
		}
		else
		{
			// Dispatch to children.
			for (int i = 0; i < 4; i++)
				apwnChildren[i]->UpdateRes(petw_owner, cam, v3_cam_obj, pa2m_depth, tlr2_depth);

			// Re-group the neighbourhood.
			// To do: handle general quad-tree structure.
			apwnChildren[0]->SetNeighbours(0,    apwnChildren[1],       apwnChildren[3], 0);
			apwnChildren[1]->SetNeighbours(0, 0, apwnChildren[2],       apwnChildren[0]);
			apwnChildren[2]->SetNeighbours(      apwnChildren[1], 0, 0, apwnChildren[3]);
			apwnChildren[3]->SetNeighbours(      apwnChildren[0],       apwnChildren[2], 0, 0);
		}
	}

	//*****************************************************************************************
	void CWaterNode::FillWaterTexture(const CCamera& cam, const CPlacement3<>& p3_cam_obj, const CDir3<>& d3_sun)
	{
		if (!bHasChildren()) 
			CWaveNode::FillWaterTexture(cam, p3_cam_obj, d3_sun);
		else
			// Dispatch to children.
			for (int i = 0; i < 4; i++)
				apwnChildren[i]->FillWaterTexture(cam, p3_cam_obj, d3_sun);
	}

	//******************************************************************************************
	void CWaterNode::CreateDisturbance(CVector2<> v2_obj, TReal r_radius, TReal r_height,
		bool b_add)
	{
		if (!bWithin(v2_obj.tX, rcObject.tX0() - r_radius, rcObject.tX1() + r_radius) ||
			!bWithin(v2_obj.tY, rcObject.tY0() - r_radius, rcObject.tY1() + r_radius))
			return;

		if (!bHasChildren()) 
			CWaveNode::CreateDisturbance(v2_obj, r_radius, r_height, b_add);
		else
			// Dispatch to kids.
			for (int i = 0; i < 4; i++)
				apwnChildren[i]->CreateDisturbance(v2_obj, r_radius, r_height, b_add);
	}

	//*****************************************************************************************
	void CWaterNode::DrawPhysics(CDraw& draw, CCamera& cam, const CTransform3<>& tf3_shape_screen) const
	{
#if bVER_BONES()
		if (!bHasChildren()) 
			// Tell water nodes to draw it.
			CWaveNode::DrawPhysics(draw, cam, tf3_shape_screen);
		else
			for (int i = 0; i < 4; i++)
				apwnChildren[i]->DrawPhysics(draw, cam, tf3_shape_screen);
#endif
	}



#if VER_MULTI_RES_WATER
//******************************************************************************************
//
class CSubWater : public CSubsystem
//
//
//
//**********************************
{
public:
	CSubWater()
		: CSubsystem(SInit(rptr0, 0, 0, "WaterSubs"))
	{
	}

	virtual void Process(const CMessagePaint& msgpaint);
};
//#if VER_MULTI_RES_WATER
#endif

//******************************************************************************************
//
class CEntityWater::CPriv: CEntityWater
//
// Private implementation of CEntityWater.
//
//**********************************
{
public:

#ifdef __MWERKS__
	// there is no default constructor for the base class,
	// hence this class cannot be constructed without an
	// explicit construction of the base
	CPriv(const SInit& initins) : CEntityWater(initins) {};
#endif

#if VER_MULTI_RES_WATER
	static int iNumInstances;
	static CSubWater* psubWater;
	static CEntityWater* petwLastRendered;		// The water entity that was last rendered.
#endif

	//*****************************************************************************************
	//
	void Init();
	// 
	// Initialises the entity. Called by all constructors.
	//
	//**********************************

	//*****************************************************************************************
	//
	void MakeTranslations();
	//
	// Create fast 2D transformations from world-to-object.
	//
	//**********************************

	//*****************************************************************************************
	//
	void GetDepthFromWorld();
	//
	// Fill pa2mDepth array with world depth values, at maximum resolution.
	// If pWater2 has been created, update it with new depth.
	//
	//**********************************

	//******************************************************************************************
	//
	void Integrate();
	//
	// Integrate all water nodes for the current step.
	//
	//**********************************

	//******************************************************************************************
	//
	void Purge();
	//
	// Removes all water data and memory.
	//
	//**********************************

	//*****************************************************************************************
	//
	uint16* au2GetConversionTable
	(
	) const;
	//
	// Returns the conversion table for use by water.
	//
	//**************************
};

//**********************************************************************************************
//
// CEntityWater::CPriv implementation.
//

#if VER_MULTI_RES_WATER
	int           CEntityWater::CPriv::iNumInstances    = 0;
	CSubWater*    CEntityWater::CPriv::psubWater        = 0;
	CEntityWater* CEntityWater::CPriv::petwLastRendered = 0;
#endif

	//******************************************************************************************
	void CEntityWater::CPriv::Init()
	{
#if VER_MULTI_RES_WATER
		Assert(CPriv::iNumInstances >= 0);

		if (CPriv::iNumInstances == 0)
		{
			NMultiResolution::CQuadRootWater::AllocMemory();

			Assert(CPriv::psubWater == 0);
			CPriv::psubWater = new CSubWater();
			pwWorld->Add(CPriv::psubWater);
		}

		CPriv::iNumInstances++;
#endif

		fResFactor = 1;
		bEnabled   = true;

		// Set partition properties.
		iIsRunning = 0;
		SetFlagShadow(false);

		// Set the custom CPhysicsInfo.
		SetPhysicsInfo(new CPhysicsInfoWater());

		// Get and save the original mesh texture.
		rptr_const<CMesh> pmsh = rptr_const_dynamic_cast(CMesh, prdtGetRenderInfo());
		AlwaysAssert(pmsh);
		AlwaysAssert(pmsh->pasfSurfaces.uLen == 1);
		ptexOriginal = pmsh->pasfSurfaces[0].ptexTexture;

		//
		// Obtain the water physical dimension from the partition box size.
		// For now, let the water be tilted if that's the way it comes in.
		//

		AlwaysAssert(pbvBoundingVol()->pbvbCast());
		CBoundVolBox bvbox = *pbvBoundingVol()->pbvbCast();
		bvbox *= fGetScale();

		pwnRoot = new CWaterNode(0, CRectangle<>
		(
			bvbox[ebeMIN_X], bvbox[ebeMIN_Y],
			bvbox[ebeMAX_X] - bvbox[ebeMIN_X], bvbox[ebeMAX_Y] - bvbox[ebeMIN_Y]
		));
		AlwaysAssert(pwnRoot->rcObject.tWidth() > 0 && pwnRoot->rcObject.tHeight() > 0);


		// Create our fast transforms.
		MakeTranslations();

#if VER_MULTI_RES_WATER
		pqnwshRoot = new NMultiResolution::CQuadRootWaterShape(*this);

		//
		// Now create a new mesh for the master entity, with 4 polygons.
		// Do do: replace this with a special CPolyIterator.
		//
		CMesh::CHeap& mh = CMesh::mhGetBuildHeap();

		// 4 surfaces. Copy the initial surface in, because if the surfaces don't have a texture,
		// the tex coords get zeroed.
		mh.masfSurfaces << pmsh->pasfSurfaces[0]
						<< pmsh->pasfSurfaces[0]
						<< pmsh->pasfSurfaces[0]
						<< pmsh->pasfSurfaces[0];

		// Create points.
		CVector2<> v2_extent = pwnRoot->rcObject.v2Extent() * (0.5 / fGetScale());

		// 9 points.
		mh.mav3Points	<< CVector3<>(-v2_extent.tX, -v2_extent.tY, 0)
						<< CVector3<>(0,             -v2_extent.tY, 0)
						<< CVector3<>(+v2_extent.tX, -v2_extent.tY, 0)

						<< CVector3<>(-v2_extent.tX, 0, 0)
						<< CVector3<>(0,             0, 0)
						<< CVector3<>(+v2_extent.tX, 0, 0)

						<< CVector3<>(-v2_extent.tX, +v2_extent.tY, 0)
						<< CVector3<>(0,             +v2_extent.tY, 0)
						<< CVector3<>(+v2_extent.tX, +v2_extent.tY, 0);
						

		// 16 vertices.
		mh.mamvVertices	<< CMesh::SVertex(mh, 0, CTexCoord(0, 0))
						<< CMesh::SVertex(mh, 1, CTexCoord(1, 0))
						<< CMesh::SVertex(mh, 4, CTexCoord(1, 1))
						<< CMesh::SVertex(mh, 3, CTexCoord(0, 1))

						<< CMesh::SVertex(mh, 1, CTexCoord(0, 0))
						<< CMesh::SVertex(mh, 2, CTexCoord(1, 0))
						<< CMesh::SVertex(mh, 5, CTexCoord(1, 1))
						<< CMesh::SVertex(mh, 4, CTexCoord(0, 1))

						<< CMesh::SVertex(mh, 4, CTexCoord(0, 0))
						<< CMesh::SVertex(mh, 5, CTexCoord(1, 0))
						<< CMesh::SVertex(mh, 8, CTexCoord(1, 1))
						<< CMesh::SVertex(mh, 7, CTexCoord(0, 1))

						<< CMesh::SVertex(mh, 3, CTexCoord(0, 0))
						<< CMesh::SVertex(mh, 4, CTexCoord(1, 0))
						<< CMesh::SVertex(mh, 7, CTexCoord(1, 1))
						<< CMesh::SVertex(mh, 6, CTexCoord(0, 1));

		// 4 polygons.
		mh.mampPolygons << CMesh::SPolygon(mh, 0, 4)
						<< CMesh::SPolygon(mh, 4, 4)
						<< CMesh::SPolygon(mh, 8, 4)
						<< CMesh::SPolygon(mh, 12, 4);

		for (int i = 0; i < 4; i++)
			mh.mampPolygons[i].pSurface = &mh.masfSurfaces[i];

		rptr<CMesh> pmsh_new = rptr_new CMesh(mh, enlFLAT, false, false, false);
		mh_build.Reset(0,0);

		SetRenderInfo(rptr_cast(CRenderType, pmsh_new));
#endif
	}

	//*****************************************************************************************
	uint16* CEntityWater::CPriv::au2GetConversionTable() const
	{
		// Return a null pointer if no clut is needed.
		if (bAlpha)
			return 0;

		// Create a clut if required.
		if (!pClutNonAlpha)
		{
			Assert(ptexOriginal->ppcePalClut->pclutClut);
			pClutNonAlpha = new CClut(ptexOriginal->ppcePalClut->pclutClut, true);
		}

		//
		// Update clut to accomodate on-the-fly switching between hardware and software
		// rasterizers.
		//
		pClutNonAlpha->SetAsWaterClut();

		// Return the non-alpha clut.
		return (uint16*)pClutNonAlpha->pvGetConversionAddress(0, 0, 0);
	}

	//*****************************************************************************************
	void CEntityWater::CPriv::MakeTranslations()
	{
		// Construct fast world-to-object 2D transforms.
		tf2ObjectWorld = CTransform3<>(p3GetPlacement());
		tf2WorldObject = ~tf2ObjectWorld;
	}

	//******************************************************************************************
	void CEntityWater::CPriv::GetDepthFromWorld()
	{
		if (pa2mDepth)
			// Already done.
			return;

		// Allocate array at max world res.
		CVector2<int> v2i_size = CWaveNode::v2iWaterSize(this, pwnRoot->rcObject.v2Extent(), 0, false);
		SetMax(v2i_size.tX, 16);
		SetMax(v2i_size.tY, 16);
		pa2mDepth = CPArray2<TMetres>(v2i_size.tY, v2i_size.tX);

		// Force water to be horizontal.  This is the real world, you know.
		CDir3<> d3_water_up = d3ZAxis * pr3GetPresence();

		// Rotate water's presence to convert current up to up.
		CPresence3<> pr3 = pr3GetPresence();
		pr3 *= CRotate3<>(d3_water_up, d3ZAxis);
		SetPresence(pr3);

		// Create a model-to-world transformation.
		CTransLinear2<> tlr2_to_world = CTransLinear2<>
		(
			CRectangle<>(0, 0, v2i_size.tX, v2i_size.tY),		// Model rectangle.
			pwnRoot->rcObject									// Object rectangle (centred, world scale).
		);

		// Find world location of object corner, and steps in each axis.
		CVector2<> v2_world_0	= CVector3<>(CVector2<>(0.5, 0.5) * tlr2_to_world) * p3GetPlacement();
		CVector2<> v2_dx		= CVector3<>(tlr2_to_world.tlrX.tScale, 0, 0) * p3GetPlacement().r3Rot;
		CVector2<> v2_dy		= CVector3<>(0, tlr2_to_world.tlrY.tScale, 0) * p3GetPlacement().r3Rot;

		// We must first check if there's terrain before we invoke the query class.
		if (CWDbQueryTerrain().tGet())
		{
			// Retrieve the terrain heights.
			CWDbQueryTerrainTopology wqttop(*pbvBoundingVol(), pr3);

			// For each element, calculate world position, and query height.
			for (int i_y = 0; i_y < pa2mDepth.iHeight; i_y++)
			{
				CVector2<> v2_world = v2_world_0;
				for (int i_x = 0; i_x < pa2mDepth.iWidth; i_x++)
				{
					// Find the terrain height at this xy location.
					// Set the depth to the difference between the terrain and water height.
					TMetres m_depth = p3GetPlacement().v3Pos.tZ - wqttop.rHeight(v2_world.tX, v2_world.tY);
					pa2mDepth[i_y][i_x] = m_depth;

					v2_world += v2_dx;
				}
				v2_world_0 += v2_dy;
			}
		}

		// To do: re-create all water objs when depth changes.
//		if (pWater2)
//			pWater2->UpdateDepth(pa2mDepth);
	}

	//******************************************************************************************
	void CEntityWater::CPriv::Integrate()
	{
		// Integrate as a whole.
		if (sRequireStep <= 0)
			return;

#if VER_MULTI_RES_WATER
		//
		// Build a list of nodes to integrate.
		// To do: incorporate nodes of entire quad-tree in this list.
		//
		CMLArray(CWaveData*, mapwvd, 4);

		if (pwnRoot->bHasChildren())
			for (int i = 0; i < 4; i++)
			{
				if (pwnRoot->apwnChildren[i]->pWater2)
					mapwvd << pwnRoot->apwnChildren[i]->pWater2;
			}

		CWaveData::Step(mapwvd, sRequireStep);
#else
		if (pwnRoot->pWater2)
		{
			pwnRoot->pWater2->Step(sRequireStep);
			iIsRunning = 2;
		}
#endif
		sRequireStep = 0;
	}

	//******************************************************************************************
	void CEntityWater::CPriv::Purge()
	{
		rptr<CMesh> pmsh = rptr_dynamic_cast(CMesh, rptr_nonconst(prdtGetRenderInfo()));
		if (pmsh)
			for (int i = 0; i < pmsh->pasfSurfaces.uLen; ++i)
				pmsh->pasfSurfaces[i].ptexTexture = rptr0;

		if (pwnRoot)
		{
			pwnRoot->prasTexture = rptr0;
			pwnRoot->ptexTexture = rptr0;

			// Delete root, and re-create with no water data or children.
			CRectangle<> rc_obj = pwnRoot->rcObject;
			delete pwnRoot;
			pwnRoot = new CWaterNode(0, rc_obj);
		}
	}

//**********************************************************************************************
//
// CPhysicsInfoWater implementation.
//

	//*****************************************************************************************
	void CPhysicsInfoWater::DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const
	{
#if bVER_BONES()
		if (!setedfMain[edfWATER])
			return;

		//
		// Draw a wireframe of the wave heights.
		//

		CEntityWater* petw = dynamic_cast<CEntityWater*>(pins);
		if (!petw)
			return;

		CTransform3<> tf3_shape_screen = petw->p3GetPlacement() * cam.tf3ToHomogeneousScreen();
		petw->pwnRoot->DrawPhysics(draw, cam, tf3_shape_screen);
#endif
	}

//**********************************************************************************************
//
// CEntityWater implementation.
//

	//******************************************************************************************
#if VER_MULTI_RES_WATER
	CRangeVar<float> CEntityWater::rvarWireZoom(.1, 20, 1);
	bool	CEntityWater::bShowQuadTree		= false;
#endif

	bool	CEntityWater::bAlpha  = false;
	bool	CEntityWater::bInterp = true;
	CRangeVar<float> CEntityWater::rvarMaxScreenRes(.01, .5, .2);
	CRangeVar<float> CEntityWater::rvarMaxWorldRes(2, 20, 8);

	//******************************************************************************************
	CEntityWater::CEntityWater(const SInit& initins)
		: CEntity(initins)
	{
		priv_self.Init();

		// Register this entity with the message types it needs to receive.
		  CMessageStep::RegisterRecipient(this);
		CMessageSystem::RegisterRecipient(this);
	}

	//******************************************************************************************
	CEntityWater::CEntityWater
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo
	) 
		: CEntity(pgon, pload, h_object, pvtable, pinfo)
	{
		priv_self.Init();

		wlWaters.push_back(this);

		SETUP_TEXT_PROCESSING(pvtable, pload)
		SETUP_OBJECT_HANDLE(h_object)
		{
			// Read the resolution multiplier.
			bFILL_FLOAT(fResFactor, esResolutionRatio); 
		}
		END_OBJECT;
		END_TEXT_PROCESSING;

		// Register this entity with the message types it needs to receive.
		  CMessageStep::RegisterRecipient(this);
		CMessageSystem::RegisterRecipient(this);
 	}

	//*********************************************************************************************
	CEntityWater::~CEntityWater()
	{
		priv_self.Purge();
		wlWaters.remove(this);

		CMessageSystem::UnregisterRecipient(this);
		  CMessageStep::UnregisterRecipient(this);

#if VER_MULTI_RES_WATER
		delete pqnwshRoot;

		--CPriv::iNumInstances;

		Assert(CPriv::iNumInstances >= 0);

		if (CPriv::iNumInstances == 0)
		{
			NMultiResolution::CQuadRootWater::FreeMemory();

			CPriv::psubWater = 0;
		}
#endif
	}

	//*****************************************************************************************
	void CEntityWater::ResetLuts()
	{
		iAlphaState   = -1;
		pClutNonAlpha = 0;

		// Setup the default alpha light values.
		lbAlphaConstant.Setup(prasMainScreen.ptPtrRaw());
		lbAlphaTerrain.CreateBlend(prasMainScreen.ptPtrRaw(), clrDefEndDepth);
		lbAlphaWater.CreateBlendForWater(prasMainScreen.ptPtrRaw());
		abAlphaTexture.Setup(prasMainScreen.ptPtrRaw());	
	}

	//*****************************************************************************************
	bool CEntityWater::bIsMoving() const
	{
		return true;
	}

	//*****************************************************************************************
	void CEntityWater::Enable(bool b_enabled)
	{
		bEnabled = b_enabled;
	}

	//******************************************************************************************
	TReal CEntityWater::rWaterHeight(const CVector2<>& v2_world)
	{
		// Transform world x and y to object space.
		CVector2<> v2 = v2_world * tf2WorldObject;

		if (pwnRoot->rcObject.bContains(v2))
		{
			// Determine which node it is in.
			CWaveNode* pwn = pwnRoot->pwnFindNode(v2);
			if (pwn)
			{
				TReal r_height = pwn->rWaterHeight(v2);
				if (r_height > rWATER_NONE)
					// Return height plus world Z position.
					return r_height + p3GetPlacement().v3Pos.tZ;
			}
		}
		return rWATER_NONE;
	}

	//******************************************************************************************
	void CEntityWater::CreateDisturbance(CVector3<> v3_world_centre, TReal r_radius, TReal r_height,
		bool b_add)
	{
		// Only add impulses to running water.
		if (!iIsRunning)
			return;

		// Transform the world coords to model coords.
		CVector2<> v2_obj = CVector2<>(v3_world_centre) * tf2WorldObject;

		pwnRoot->CreateDisturbance(v2_obj, r_radius, r_height, b_add);
	}

	//******************************************************************************************
	void CEntityWater::Process(const CMessageStep& msgstep)
	{
		//
		// For optimisation purposes, we want to integrate and do texture conversion only when
		// the water is viewed.  So in the step, we only record that a step is necessary,
		// and do the real step and conversion in Render().
		//

		// Don't worry, actual integration time is clamped, so sRequireStep can grow arbitrarily.
		sRequireStep += msgstep.sStep;

		// Keep track of whether we're currently running.  Render() will set this flag to 2,
		// so if we haven't rendered for 1 or 2 steps, this flag will become false.
		if (iIsRunning > 0)
		{
			iIsRunning--;

			//
			// Wake up any floating objects. 
			// To do: Perform on a per-node basis. Restore test for significant ampltitude. 
			// We do not need to do an amplitude significance test.
			// Currently, very few objects float, so we can afford to keep them awake while
			// the water is running; they will sleep when the water isn't looked at.
			//

			{
				// Find all floatable physics objects intersecting the water.
				CWDbQueryPhysicsMovable wqp(this);
				foreach (wqp)
				{
					// Activate floating objects only.
					if (wqp.tGet() != this && 
						wqp.tGet()->pphiGetPhysicsInfo()->bFloats())
						wqp.tGet()->PhysicsActivate();
				}
			}
		}
		else
		{
			//
			// To do: restore the purging code, on a per-node basis. Let each node track sRequireStep.
			// 

			// Dump the water if it is not in use.
			if (sRequireStep > sPURGE_TIME)
			{
				priv_self.Purge();
				sRequireStep = 0;
			}
		}
	}

	//*****************************************************************************************
	void CEntityWater::InitializeDataStatic()
	{
		CPartition::InitializeDataStatic();

		// This turns off the bSimpleObject flag, so that the water is always tested
		// with the camera volume. This prevents calling PreRender() when the water is not visible.
		SetFlagNoCacheAlone(false);
	}

	//******************************************************************************************
	void CEntityWater::PreRender(CRenderContext& renc)
	{
		// Do nothing if not enabled.
		if (!bEnabled)
			return;

		// Step if necessary, and perform conversion.
		CCycleTimer ctmr;

		if (iAlphaState != (int)bAlpha)
		{
			// Rebuild alpha table here.
			// To do: make this somehow source-texture independent.
			if (bAlpha)
			{
				// Use alpha conversion table.
				for (int i = 0; i < 32; i++)
					sau2PixelTable[i] = au2IntensityToAlpha[i];
			}
			else
			{
				// Use alpha conversion table.
				for (int i = 0; i < 32; i++)
					sau2PixelTable[i] = au2IntensityToAlpha[i];

				/*
				// Create an array containing the clut-converted values for the solid colour.
				uint16* au2_clut = priv_selfc.au2GetConversionTable();
				for (int i = 0; i < 32; i++)
				{
					int i_clut_index = (i << iBitsInIndex) | ptexOriginal->tpSolid;
					sau2PixelTable[i] = au2_clut[i_clut_index];
				}
				*/
			}

			// Re-construct the direct pixel table from current settings.
			new(&PixelReflectTable) CPixelReflectTable;
		}

		// Adjust effective water resolution by the overall quality setting.
		float f_adj = fWaterResAdjFactor();

		float f_adj_max_screen_res = rvarMaxScreenRes * fResFactor * f_adj;
		float f_adj_max_world_res  = rvarMaxWorldRes  * fResFactor * f_adj;

		if (f_adj_max_screen_res != rMaxScreenRes ||
			f_adj_max_world_res  != rMaxWorldRes    )
		{
			rMaxScreenRes = f_adj_max_screen_res;
			rMaxWorldRes  = f_adj_max_world_res;

			// Change of resolution requires killing and re-creating water and depth arrays.
			pa2mDepth = CPArray2<TMetres>();
			delete pwnRoot->pWater2.ptGet();
			pwnRoot->pWater2 = 0;
			pwnRoot->iMipLevel = -1;
		}

		// Get the depth values if not gotten already.
		priv_self.GetDepthFromWorld();

		// Create object-to-depth array translation.
		CTransLinear2<> tlr2_depth
		(
			pwnRoot->rcObject,
			CRectangle<>(0, 0, pa2mDepth.iWidth, pa2mDepth.iHeight)
		);

#if VER_MULTI_RES_WATER
		pqnwshRoot->Update(renc.Camera);
#endif

		// Get object-space camera position.
		CVector3<> v3_cam_obj = renc.Camera.v3Pos() / p3GetPlacement();

		// Create/adjust any new water objs.
		// Check the resolution for each node.

		//
		// For this root object, always subdivide into 4 children.
		// These children will not themselves subdivide.
		// To do: replace this with actual quad-tree management.
		//
//		pwnRoot->Subdivide();

		pwnRoot->UpdateRes(this, renc.Camera, v3_cam_obj, pa2mDepth, tlr2_depth);

		// Integrate the water.
		priv_self.Integrate();

		// Now render.
		#if bREAL_SPECULAR
			// Get primary directional light info.
			aptr<CLightList> pltl = new CLightList(CWDbQueryLights(&cam));
			pltl->SetViewingContext(pr3GetPresence(), cam.pr3GetPresence() / pr3GetPresence());
			SBumpLighting blt = pltl->bltGetBumpLighting(CVector3<>(0, 0, 0), d3ZAxis);
			CDir3<> d3_sun = blt.d3Light;
		#else
			// Dummy value.
			CDir3<> d3_sun;
		#endif

		pwnRoot->FillWaterTexture
		(
			renc.Camera, 
			renc.Camera.p3GetPlacement() / p3GetPlacement(),
			d3_sun
		);

#if VER_MULTI_RES_WATER
		if (pwnRoot->bHasChildren())
		{
			//
			// Copy each child's texture to a root mesh polygon surface,
			// and get any new texture coords as well.
			// To do: forget this once special CPolyIterator is written.
			//
			rptr<CMesh> pmsh = rptr_dynamic_cast(CMesh, rptr_nonconst(prdtGetRenderInfo()));
			Assert(pmsh);

			for (int i = 0; i < 4; i++)
			{
				CWaterNode* pwn = pwnRoot->apwnChildren[i];
				if (pwn->ptexTexture)
				{
					// Only do this if the texture's been created already.
					pmsh->pampPolygons[i].pSurface->ptexTexture = pwn->ptexTexture;

					pmsh->pamvVertices[4*i    ].tcTex = CTexCoord(pwn->rcTexExtents.tX0(), pwn->rcTexExtents.tY0());
					pmsh->pamvVertices[4*i + 1].tcTex = CTexCoord(pwn->rcTexExtents.tX1(), pwn->rcTexExtents.tY0());
					pmsh->pamvVertices[4*i + 2].tcTex = CTexCoord(pwn->rcTexExtents.tX1(), pwn->rcTexExtents.tY1());
					pmsh->pamvVertices[4*i + 3].tcTex = CTexCoord(pwn->rcTexExtents.tX0(), pwn->rcTexExtents.tY1());
				}
			}
		}
#else
		if (pwnRoot->ptexTexture)
		{
			//
			// Stick the texture back in the mesh.
			//
			// HACK ALERT: We should use prdtGetInfoWritable() here, to obtain a unique, possibly 
			// duplicated mesh, since we are changing its texture and material.  
			// But both instancing and CRenderType copy functions are currently unimplemented.
			// Thus we do const_casts below.
			//
			rptr<CMesh> pmsh = rptr_dynamic_cast(CMesh, rptr_nonconst(prdtGetRenderInfo()));
			AlwaysAssert(pmsh);
			AlwaysAssert(pmsh->pasfSurfaces.uLen == 1);

			pmsh->pasfSurfaces[0] = pwnRoot->ptexTexture;

			// Create a translation between object coords and texture coords.
			CTransLinear2<> tlr2_obj_tex(pwnRoot->rcObject * (1.0 / pr3GetPresence().rScale), pwnRoot->rcTexExtents);

			// Reassign all texture coords.
			for (int i = 0; i < pmsh->pamvVertices.size(); ++i)
			{
				pmsh->pamvVertices[i].tcTex = CVector2<>(*pmsh->pamvVertices[i].pv3Point) * tlr2_obj_tex;
			}
		}
#endif

		psWaterStep.Add(ctmr(), 1);

		CEntity::PreRender(renc);

#if VER_MULTI_RES_WATER
		CPriv::petwLastRendered = this;
#endif
	}

	//******************************************************************************************
	void CEntityWater::Process(const CMessageSystem& msgsys)
	{
		// Whenever the sim is started, we query the world depth values if needed.
		if (msgsys.escCode == escSTART_SIM)
		{
			priv_self.GetDepthFromWorld();
		}
	}

	//*****************************************************************************************
	void CEntityWater::Move(const CPlacement3<>& p3_new, CEntity* pet_sender)
	{
		CEntity::Move(p3_new, pet_sender);

		// Need to update our transforms.
		priv_self.MakeTranslations();

		// Kill depth array, so it gets re-made for new position.
		pa2mDepth = CPArray2<TMetres>();
	}

	//*****************************************************************************************
	bool CEntityWater::bContainsCacheableMeshes()
	{
		// The object must be cacheable.
		return false;
	}


	//******************************************************************************************
	int CEntityWater::iGetManagedMemUsed()
	{
		return iMemUsedTotal;
	}

	//******************************************************************************************
	int CEntityWater::iGetManagedMemSize()
	{
		return iManagedMemSize;
	}

	//******************************************************************************************
	void CEntityWater::SetTextureMemSize(int i_mem_size_kb)
	{
		Assert(i_mem_size_kb >= 0);
		iManagedMemSize = i_mem_size_kb * 1024;
	}

	//*****************************************************************************************
	void CEntityWater::PurgeD3DTextures()
	{
		priv_self.Purge();
	}


//**********************************************************************************************
//
// CSubWater implementation.
//

#if VER_MULTI_RES_WATER
	//******************************************************************************************
	void CSubWater::Process(const CMessagePaint& msgpaint)
	{
		if (CEntityWater::CPriv::petwLastRendered && CEntityWater::bShowQuadTree)
		{
			// Attempt to cast to a raster win.
			CRasterWin* pras_win = dynamic_cast<CRasterWin*>(msgpaint.renContext.pScreenRender->prasScreen);

			if (pras_win != 0)
			{
				// Get camera origin, in quad space.
				const CCamera& cam = *CWDbQueryActiveCamera().tGet();

				CVector2<> v2_quad_pos = CVector2<>(cam.v3Pos() * CEntityWater::CPriv::petwLastRendered->pqnwshRoot->pr3WorldQuad);

				CRectangle<> rc_map = CEntityWater::CPriv::petwLastRendered->pqnwshRoot->rcDrawWireframe
				(
					pras_win,
					CColour(0.8, 0.0, 0.0),
					v2_quad_pos,
					CEntityWater::rvarWireZoom
				);

				CDraw draw(rptr_this(pras_win), rc_map, true);

				// Draw camera cone on top.
				draw.Colour(CColour(1.0, 0.0, 1.0));

				// Get camera origin, and bottom far points, in world space.
				CVector3<> v3_left   = CVector3<>(-.1, .1, -.1) * ~cam.tf3ToNormalisedCamera() * CEntityWater::CPriv::petwLastRendered->pqnwshRoot->pr3WorldQuad;
				CVector3<> v3_right  = CVector3<>(+.1, .1, -.1) * ~cam.tf3ToNormalisedCamera() * CEntityWater::CPriv::petwLastRendered->pqnwshRoot->pr3WorldQuad;

				draw.Line(CVector2<>(v3_left), v2_quad_pos);
				draw.LineTo(CVector2<>(v3_right));
			}
		}

		CEntityWater::CPriv::petwLastRendered = 0;
	}
//#if VER_MULTI_RES_WATER
#endif


//
// Global function implementations.
//

//*****************************************************************************************
void PurgeWaterTextures()
{
	// Do nothing if no water objects are present.
	if (wlWaters.size() == 0)
		return;

	//
	// Iterate through the list of water objects and force them to remove their Direct3D
	// rasters.
	//
	TWaterList::iterator it = wlWaters.begin();
	for (; it != wlWaters.end(); ++it)
		(*it)->PurgeD3DTextures();
}