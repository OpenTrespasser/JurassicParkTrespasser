/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Terrain texture system types.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/TerrainTexture.hpp                                      $
 * 
 * 63    10/01/98 12:23a Pkeet
 * Made a function to automatically set the texture resolution.
 * 
 * 62    9/27/98 8:35p Mlange
 * Added clrGetBaseTextureColour() function.
 * 
 * 61    9/01/98 1:55p Mlange
 * Removed bSignificantMove data member.
 * 
 * 60    7/23/98 6:32p Mlange
 * Terrain texture moving shadow buffers are now managed by an instance of the texture page
 * manager, this to limit the amount of memory used for moving shadow buffers. Some minor
 * optimisations to moving shadows for insignificant object moves.
 * 
 * 59    7/22/98 2:36p Mlange
 * Now only purges terrain texture on wakeup/sleep when movement was significant. Now uses
 * lists<> to store dirty rects.
 * 
 * 58    7/09/98 7:56p Mlange
 * Made moving shadows much more efficient: now evaluates descendant and ancestor texture nodes
 * to determine if moving shadows must be enabled, instead of blindly assuming each newly
 * created texture will have moving shadows. Now only updates the sub-regions of the texture
 * that are modified by moving shadows, instead of the entire texture. Fixed bug in texture
 * purge on wake-up/sleep of objects. Now uses default clut for uninitialised textures again, to
 * avoid referencing the terrain texture clut when it has been deleted.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_TERRAINTEXTURE_HPP
#define HEADER_LIB_GEOMDBASE_TERRAINTEXTURE_HPP

#include <list>
#include "Lib/Std/BlockAllocator.hpp"
#include "Lib/Transform/TransLinear.hpp"
#include "Lib/GeomDBase/Shape.hpp"
#include "Lib/GeomDBase/TexturePageManager.hpp"


// Opaque declarations.
class CTexture;
class CConsoleBuffer;
class CMessageMove;
class CRasterMem;
class CConsoleBuffer;

namespace NMultiResolution
{
	class CQuadNodeTIN;
	class CQuadRootTIN;

	//Default amount memory to allocate for terrain textures, in KB.
	#define iDEFAULT_TERRAIN_TEXTURE_MEMORY_KB					1200
	#define iMAX_TERRAIN_TEXTURE_MEMORY_KB					    2500
	#define iDEFAULT_TERRAIN_TEXTURE_MOVING_SHADOW_MEMORY_KB	512

	//**********************************************************************************************
	//
	class CTextureNode : public CBlockAllocator<CTextureNode>
	//
	// Prefix: txn
	//
	// Contains information for a single terrain texture and its mapping to the world.
	//
	//**************************************
	{
	public:
		static bool bDisableTextures;		// Disable dynamic textures.
		static bool bClears;				// Whether to clear dynamic textures before rendering them.
		static bool bShowConstrained;		// Whether to clear show the textures whose subdivision was constrained.
		static bool bOutlineNodes;			// Debugging feature which shows quad-node borders in texture.
		static bool bEnableShadows;			// Whether to place shadows on terrain.
		static bool bEnableMovingShadows;	// Whether to update shadows for moving objects.

		static rptr<CTexturePageManager> ptexmTexturePages;
		static rptr<CTexturePageManager> ptexmTextureStaticShadowPages;

	private:
		class CPriv;
		friend class CPriv;

		friend class CBlockAllocator<CTextureNode>;
		static CBlockAllocator<CTextureNode>::SStore stStore; // Storage for the types.

		static CDir3<> d3ShadowingLight_;	// See below.


		uint8 bInit;						// Whether this texture is initialised.
		uint8 bShadow;						// Whether this texture is shadowed (both static and moving).
		uint8 bStaticTexture;				// Whether this texture references a sub-region of the static base terrain texture.
		uint8 bSolidColour;					// Whether this texture is has a solid colour and no raster.
		uint8 bShadowRequired;				// Whether this texture should, or should not, be shadowed next update.

		int16 iLastScheduled;				// Count how long ago this texture was scheduled for an update.


		CVector2<int> v2DimRequired;		// Required dimensions of texture for next update. May not match actual size of
											// allocated raster.

		const CQuadNodeTIN* pqntOwner;		// Owning quad node.

		rptr<CTexture> ptexTexture;			// The texture associated with this node.

		CTransLinear2<> tlr2ObjectTexture;	// Mapping from mesh to texture coords for this node.

		CTexturePageManager::CRegionHandle rhAlloc;
											// Allocation handle for the texture's raster.

		CTexturePageManager::CRegionHandle rhAllocShadow;
											// Allocation handle for the texture and static shadows.

		std::list< CRectangle<> >    ltrcDirtyRects;
		std::list< CRectangle<int> > ltrcPrevDirtyRects;

		//******************************************************************************************
		//
		// Constructors and destructor.
		//
	public:
		CTextureNode(const CQuadNodeTIN* pqnt_owner);


		CTextureNode
		(
			const CTextureNode& rtxn_ancestor,
			const CQuadNodeTIN* pqnt_owner,
			const CQuadRootTIN* pqntin_root
		);


		~CTextureNode();


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		static void SetTextureMemSize
		(
		);
		//
		// Allocate texture memory using defaults.
		//
		//**************************************

		//******************************************************************************************
		//
		static void SetTextureMemSize
		(
			int i_mem_size_kb
		);
		//
		// Allocate texture memory.
		//
		//**************************************


		//******************************************************************************************
		//
		static void SetBaseTexture
		(
			rptr<CTexture> ptex
		);
		//
		// Set the base texture for the terrain.
		//
		//**************************************


		//******************************************************************************************
		//
		static CColour clrGetBaseTextureColour
		(
			const CQuadRootTIN* pqntin_root,
			TReal r_world_x,
			TReal r_world_y
		);
		//
		// Returns:
		//		The colour of the base terrain texture at the given world x/y location, or black
		//		(colour 0, 0, 0) if given location is outside of bounds of the terrain.
		//
		//**************************************


		//******************************************************************************************
		//
		static void PurgeAllTextures();
		//
		// Purges all the terrain textures.
		//
		//**************************************


		//******************************************************************************************
		//
		static void FrameEnd();
		//
		// Must be called after the frame has rendered.
		//
		//**************************************


		//******************************************************************************************
		//
		static void PrintStats
		(
			CConsoleBuffer& con
		);
		//
		// Print some stats about the terrain textures.
		//
		//**************************************


		//******************************************************************************************
		//
		static CDir3<> d3ShadowingLight()
		//
		// Returns:
		//		The direction of the shadowing light, as of last rendering.
		//
		//**********************************
		{
			return CTextureNode::d3ShadowingLight_;
		}


		//******************************************************************************************
		//
		bool bTextureInitialised() const
		//
		// Returns:
		//		'true' if an actual texture has been created (e.g. not the dummy texture).
		//
		//**********************************
		{
			return bInit;
		}


		//******************************************************************************************
		//
		int iGetLastScheduled() const
		//
		// Returns:
		//		How long ago this texture was scheduled for an update.
		//
		//**********************************
		{
			return iLastScheduled;
		}


		//******************************************************************************************
		//
		rptr_const<CTexture> ptexGetTexture() const
		//
		// Returns:
		//		The texture associated with this node.
		//
		//**********************************
		{
			return ptexTexture;
		}


		//******************************************************************************************
		//
		CTexCoord tcObjectToTexture
		(
			int i_quad_x,
			int i_quad_y
		) const
		//
		// Returns:
		//		Texture coordinates corresponding to given quad space coordinates.
		//
		//**********************************
		{
			return CVector2<>(i_quad_x, i_quad_y) * tlr2ObjectTexture;
		}


		//******************************************************************************************
		//
		void SetMovingObjShadows
		(
			const CRectangle<>& rc_quad_proj,
			const CMessageMove& msgmv
		);
		//
		// Updates this texture for moving shadows.
		//
		//**********************************


		//******************************************************************************************
		//
		void PurgeTexture();
		//
		// Destroys the texture, causing it to be re-rendered next access.
		//
		//**********************************


		//******************************************************************************************
		//
		void ScheduleUpdate
		(
			const CQuadRootTIN* pqntin_root,
			CVector2<> v2_dim_req,			// Requested texture size, in floats.
			bool b_shadow_req
		);
		//
		// Schedule this texture for an update if required.
		//
		// Notes:
		//		Resets the last scheduled counter.
		//
		//**********************************


		//******************************************************************************************
		//
		void SetStaticTexture
		(
			const CQuadRootTIN* pqntin_root
		);
		//
		// Maps this texture node to the static, pre-lit base terrain texture.
		//
		// Notes:
		//		Resets the last scheduled counter.
		//
		//**********************************


		//******************************************************************************************
		//
		void SetSolidTexture
		(
			const CQuadRootTIN* pqntin_root
		);
		//
		// Creates a fogged, solid colour (e.g. no raster) texture.
		//
		// Notes:
		//		Resets the last scheduled counter.
		//
		//**********************************

	};
};


#endif
