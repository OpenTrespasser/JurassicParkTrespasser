/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of TerrainTexture.hpp.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/TerrainTexture.cpp                                      $
 * 
 * 186   10/05/98 5:22p Mlange
 * Commented-out optimisation that removed dirty rects that were not visible. This particular
 * implementation of that optimisation does not work.
 * 
 * 185   98/10/02 15:41 Speter
 * Added sort option to terrain object query.
 * 
 * 184   10/01/98 12:32a Asouth
 * explicit cast
 * 
 * 183   10/01/98 12:23a Pkeet
 * Made a function to automatically set the texture resolution.
 * 
 * 182   9/27/98 8:35p Mlange
 * Added clrGetBaseTextureColour() function.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "TerrainTexture.hpp"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/GeomDBase/TerrainObj.hpp"
#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/Renderer/Texture.hpp"
#include "Lib/Renderer/PipeLine.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"
#include "Lib/EntityDBase/Query/QTerrainObj.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/QualitySettings.hpp"
#include "Lib/View/RasterComposite.hpp"
#include "Lib/View/RasterD3D.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Transform/TransformIO.hpp"
#include "Lib/Sys/Scheduler.hpp"
#include "Lib/Std/Mem.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/Std/MemLimits.hpp"

#include <list>

//
// Static constants.
//

static float fAspectDivergenceThreshold = 0.5;
static float fIncreaseThreshold	= 1.414;
static float fDecreaseThreshold	= 0.5;

//
// Helper functions.
//

//******************************************************************************************
inline int iNearestPower2(float f)
{
	//
	// The nearest power of two of a number can be efficiently derived from the bit pattern of
	// an IEEE floating point number. Observe that a floating point number is represented as 2^e * m.
	// Thus, the truncated power of two of the number is equal to the exponent. The value of the
	// mantissa then determines if we need to add 1 to the exponent value to obtain the rounded
	// power of two. From: 2^e * 2^r == 2^(e+r), we note that we need to add 1 if the mantissa's
	// value exceeds 2^(1/2).
	//

	// The mantissa is stored as: (m - 1) * 2^iFLOAT_MANT_BIT_WIDTH.
	static const int iMANT_ROUND = int((sqrt(2.0) - 1) * (1 << iFLOAT_MANT_BIT_WIDTH));

	int i_log2 =   CIntFloat(f).i4GetExponent() -
				   ((iMANT_ROUND - CIntFloat(f).i4GetMantissa()) >> (sizeof(int) * 8 - 1));

	return 1 << MinMax(i_log2, 0, sizeof(int) * 8 - 2);
}

//******************************************************************************************
inline float fAbsRatio(float f)
{
	// Return this number, or its reciprocal if less than 1.
	return f < 1.0 ? 1.0 / f : f;
}

//**********************************************************************************************
//
struct SProfileTex
{
	CProfileStat psTerrainTex;
		CProfileStat psAlloc;
		CProfileStat psQuery;
		CProfileStat psTexture;
		CProfileStat psLight;
		CProfileStat psShadow;
		CProfileStat psCopy;
		CProfileStat psClut;
	CProfileStat psShadowMoving;

	SProfileTex(const char* str_name, CProfileStat* pps_parent, CSet<EProfileFlag> setepf = set0)
		: psTerrainTex(str_name, pps_parent, setepf),
			psAlloc(  "Alloc",   &psTerrainTex, Set(epfHIDDEN)),		// Also denotes alloc texture KB.
			psQuery(  "Query",   &psTerrainTex, Set(epfHIDDEN)),
			psTexture("Texture", &psTerrainTex, Set(epfHIDDEN)),		// Also denotes # texture objects.
			psLight(  "Light",   &psTerrainTex, Set(epfHIDDEN)),		// Also denotes total current textures.
			psShadow( "Shadow",  &psTerrainTex),
			psCopy(   "Copy",    &psTerrainTex, Set(epfHIDDEN)),		// Also denotes # of texels copied.
			psClut(   "Clut",    &psTerrainTex, Set(epfHIDDEN)),		// Also denotes # of texel lookups.
		psShadowMoving("Shadow Mov", pps_parent, setepf)
	{
	}
};

static SProfileTex proTex("Terrain Tex", &proProfile.psRenderShape, Set(epfSEPARATE));


//******************************************************************************************
//
class CScreenRenderTerrain : public CScreenRender
//
// A screen rendering implementation used for the 8-bit terrain texture copy primitive.
//
//**************************************
{
public:

	//******************************************************************************************
	CScreenRenderTerrain(SSettings* pscrenset, rptr<CRaster> pras_screen)
		: CScreenRender(pscrenset, pras_screen)
	{
		Assert(pras_screen->iPixelBytes() == 2);
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void BeginFrame() override
	{
		prasScreen->Lock();
	}

	//******************************************************************************************
	virtual void EndFrame() override
	{
		prasScreen->Unlock();
	}

	//******************************************************************************************
	virtual void DrawPolygon(CRenderPolygon& rp) override
	{
		// Invoke the CDrawPolygon code, using the screen as both screen and Z buffer.
		if (rp.seterfFace[erfTRANSPARENT])
			CDrawPolygon<TTexNoClutLinearTrans>(prasScreen, rp);
		else
			CDrawPolygon<TTexNoClutLinear>(prasScreen, rp);
	}
};


//******************************************************************************************
//
struct SRenderSettingsTerrain: public CRenderer::SSettings
//
// Customise settings for terrain pre-texturing rendering.  
//
// Prefix: rensettrr
//
//**************************************
{
	SRenderSettingsTerrain()
	{
		// Set to transparent, linear copy mode.
		seterfState = Set(erfCOPY) + erfTEXTURE + erfTRANSPARENT;

		seterfState[erfMIPMAP]     = (int)(prenMain->pSettings->seterfState[erfMIPMAP]);
//		seterfState[erfTRAPEZOIDS] = prenMain->pSettings->seterfState[erfTRAPEZOIDS];

		// We clear it ourselves.
		bClearBackground = false;

		// Disable object shadowing, render chaching, depth sorting, and culling.
		bShadow			= false;
		bRenderCache	= false;
		esSortMethod	= esNone;
		bBackfaceCull	= false;
		bDrawSky		= false;

		// Indicate that scheduled operations should not be executed.
		bExecuteScheduler = false;
	}
};


//******************************************************************************************
//
// Definitions for CTextureNode::RenderTexture().
//

//******************************************************************************************
//
inline bool operator <
(
	ptr<NMultiResolution::CTextureNode>& ptxn1,
	ptr<NMultiResolution::CTextureNode>& ptxn2
)
//
// Comparison function for sorting texture nodes according to their last scheduled counter,
// from oldest to most recent.
//
//**********************************
{
	return ptxn1->iGetLastScheduled() > ptxn2->iGetLastScheduled();
};



//**********************************************************************************************
//
// CTextureNode implementation.
//
static int   iTexturePageSizeRange   = 2;
static const float fTexturePageCountFactor = 2.5;
static int   iMaxTexturePageSize     = 256;

namespace NMultiResolution
{
	//**********************************************************************************************
	//
	class CTextureNode::CPriv : public CTextureNode
	//
	// Private implementation of CTextureNode.
	//
	//**************************************
	{
		static CClut* pclutSoftware;
		static CClut* pclutDirect3D;

		static CPalClut* ppcePalClut;		// Palette/Clut used for renderering. Currently only used with Direct3D.

		static int iShadowLightVal;			// Light value to set in the map for shadowed texels.

		static CClipRegion2D clip2dCam;

	public:
		static bool bClipRegionInit;

		// Base class has no default constructor, hence this class
		// can have no default constructor; the following ensures
		// that this class can be constructed.
		CPriv(const CQuadNodeTIN* pqnt_owner) : CTextureNode(pqnt_owner) {};

		static rptr<CRaster> prasBaseTexture;
		static CTexturePageManager::CRegionHandle rhBaseTexture;

		typedef std::list< ptr<NMultiResolution::CTextureNode> > TList;
		static TList lptxnTextures;

		static CClut* pclutCurrent;


		//******************************************************************************************
		//
		static void GatherDscDirtyRects
		(
			CTextureNode* ptxn_ancestor,
			const CQuadNodeTIN* pqntin_node
		);
		//
		// Search given quad tree branch for texture nodes and add any of their dirty rects to
		// the given texture node.
		//
		//**********************************


		//******************************************************************************************
		//
		static const CClipRegion2D& clip2dGet
		(
			const CQuadRootTIN* pqntin_root
		);
		//
		// Returns:
		//		The 2d projected clipping region of the camera.
		//
		//**********************************


		//******************************************************************************************
		//
		void DoTextureUpdate
		(
			const CQuadRootTIN* pqntin_root
		);
		//
		// Update, and if required, render this terrain texture.
		//
		//**********************************


		//******************************************************************************************
		//
		void AllocRegion();
		//
		// Allocate a region for the texture's raster.
		//
		// Notes:
		//		Initialises rhAlloc and, if required, rhAllocShadow if successful.
		//
		//**********************************


		//******************************************************************************************
		//
		bool bReAllocRegion();
		//
		// Attempt to (re)allocate a region for a new texture raster at a higher resolution.
		//
		// Returns:
		//		'true' if the reallocation was successful, 'false' if the current allocated region
		//		remains in use.
		//
		// Notes:
		//		Initialises rhAlloc and, if required, rhAllocShadow if successful.
		//
		//**********************************


		//******************************************************************************************
		//
		void InitTexture
		(
			const CQuadRootTIN* pqntin_root
		);
		//
		// Initialise the CTexture for this node.
		//
		//**********************************


		//******************************************************************************************
		//
		void RenderTexture
		(
			const CQuadRootTIN* pqntin_root
		);
		//
		// Render this terrain texture.
		//
		//**********************************


		//******************************************************************************************
		//
		void RenderMovingShadows
		(
			const CQuadRootTIN* pqntin_root
		);
		//
		// Render the moving shadows for this terrain texture.
		//
		//**********************************


		//******************************************************************************************
		//
		void ClutConvert
		(
			rptr<CRaster> pras_region,
			const CRectangle<int>* prc_rect = 0
		);
		//
		// Do clut conversion and upload modified areas of texture.
		//
		//**********************************


		//******************************************************************************************
		//
		static void MergeInList
		(
			std::list< CRectangle<int> >& lrc_merged,
			CRectangle<int> rc_new
		);
		//
		// Adds the given rectangle into the list such that there are no overlapping rectangles in
		// the list afterwards. Any existing rectangles in the list may be grown to accomodate the
		// new rectangle.
		//
		//**********************************


		//******************************************************************************************
		//
		static void CopyRaster
		(
			rptr<CRaster> pras_dest,
			rptr<CRaster> pras_src,
			const CRectangle<int>* prc_rect = 0
		);
		//
		//
		//**********************************


		//******************************************************************************************
		//
		static void UpdateCluts();
		//
		//
		//**********************************


		//******************************************************************************************
		//
		static void InitBaseTexture();
		//
		//
		//**********************************
	};



	//*********************************************************************************************
	//
	class CScheduleTerrainTextureItem : public CSchedulerItem
	//
	// Schedules function operations.
	//
	// Prefix: sctx
	//
	//**************************************
	{
		CTextureNode* ptxnItem;
		const CQuadRootTIN* pqntinRoot;

	public:
		CScheduleTerrainTextureItem(CTextureNode* ptxn, const CQuadRootTIN* pqntin_root, float f_priority)
			: CSchedulerItem(f_priority), ptxnItem(ptxn), pqntinRoot(pqntin_root)
		{
		}


		//*****************************************************************************************
		virtual void Execute() override
		{
			static_cast<CTextureNode::CPriv*>(ptxnItem)->DoTextureUpdate(pqntinRoot);
		}

		//*****************************************************************************************
		virtual void PostScheduleExecute() override
		{
		}
	};


	//******************************************************************************************
	bool CTextureNode::bDisableTextures      = false;
	bool CTextureNode::bClears               = false;
	bool CTextureNode::bShowConstrained      = false;
	bool CTextureNode::bOutlineNodes         = false;
	bool CTextureNode::bEnableShadows        = true;
	bool CTextureNode::bEnableMovingShadows  = true;

	rptr<CTexturePageManager> CTextureNode::ptexmTexturePages             = rptr_new CTexturePageManager();
	rptr<CTexturePageManager> CTextureNode::ptexmTextureStaticShadowPages = rptr_new CTexturePageManager();

	CBlockAllocator<CTextureNode>::SStore CTextureNode::stStore;

	CDir3<> CTextureNode::d3ShadowingLight_;

	static const CColour clrNotInit = CColour(1.0, 0.0, 0.0);


	//******************************************************************************************
	CTextureNode::CTextureNode(const CQuadNodeTIN* pqnt_owner)
		: pqntOwner(pqnt_owner)
	{
		Assert(pqnt_owner != 0);

		// Initialise this.
		ptexTexture = rptr_new CTexture(clrNotInit, &matDEFAULT);
		ptexTexture->seterfFeatures = seterfDEFAULT - erfLIGHT_SHADE;

		bInit           = false;
		iLastScheduled  = -1;

		Assert(pqnt_owner->ptxnGetTextureNode() == 0);

		CPriv::GatherDscDirtyRects(this, pqnt_owner);

		CPriv::lptxnTextures.push_back(this);
	}


	//******************************************************************************************
	CTextureNode::CTextureNode(const CTextureNode& rtxn_ancestor, const CQuadNodeTIN* pqnt_owner, const CQuadRootTIN* pqntin_root)
		: pqntOwner(pqnt_owner)
	{
		Assert(pqnt_owner != 0);

		// Initialise this.
		ptexTexture = rptr_new CTexture(clrNotInit, &matDEFAULT);
		ptexTexture->seterfFeatures = seterfDEFAULT - erfLIGHT_SHADE;

		bInit           = false;
		iLastScheduled  = -1;

		// Copy the ancestor's moving shadows' dirty rects if they intersect this descendant.
		if (!rtxn_ancestor.ltrcDirtyRects.empty())
		{
			std::list< CRectangle<> >::const_iterator it;
			for (it = rtxn_ancestor.ltrcDirtyRects.begin(); it != rtxn_ancestor.ltrcDirtyRects.end(); ++it)
				if ((*it).bIntersects(pqnt_owner->rcGetRectangle()))
					ltrcDirtyRects.push_back(*it);
		}

		CPriv::lptxnTextures.push_back(this);


		// If the ancestor wasn't initialised or if we want to see outlined nodes we simply leave this texture uninitialised.
		if (!rtxn_ancestor.bInit || rtxn_ancestor.bSolidColour || rtxn_ancestor.bStaticTexture || CTextureNode::bOutlineNodes)
			return;

		// For simplicity of the implementation, we also leave this uninitialised if the ancestor has moving shadows.
		// But we could make this work and reuse the ancestor texture and moving shadow buffer if performance requires it.
		if (rtxn_ancestor.rhAllocShadow)
			return;

		// Calculate the size divisor (shift factor) mapping from the ancestor to this node.
		int i_log2_ancestor_node_size = uLog2(rtxn_ancestor.pqntOwner->iGetSize());
		int i_shift                   = i_log2_ancestor_node_size - uLog2(pqntOwner->iGetSize());

		Assert(i_shift > 0);

		// Set the required size of this texture node as a fraction of the ancestor's texture size.
		v2DimRequired.tX = rtxn_ancestor.v2DimRequired.tX >> i_shift;
		v2DimRequired.tY = rtxn_ancestor.v2DimRequired.tY >> i_shift;
		CTextureNode::ptexmTexturePages->ClampRegionSize(v2DimRequired);

		// Set the actual size of this texture node as a sub-region of the ancestor texture.
		// Note! Even though the ancestor's texture region may have been deleted, we assume the allocation
		// handle remains valid.
		CVector2<int> v2_ancestor_alloc_size = rtxn_ancestor.rhAlloc.v2GetSize();

		CVector2<int> v2_alloc_size(v2_ancestor_alloc_size.tX >> i_shift,
		                            v2_ancestor_alloc_size.tY >> i_shift );

		// If the actual size of this texture is too small, we cannot create this texture as a sub-region of the ancestor
		// texture. We must leave it uninitialised.
		int i_min_tex_size = CTextureNode::ptexmTexturePages->iGetMinRegionSize();

		if (v2_alloc_size.tX < i_min_tex_size || v2_alloc_size.tY < i_min_tex_size)
			return;

		// Copy flags from ancestor.
		bShadow        = rtxn_ancestor.bShadow;
		bSolidColour   = false;
		bStaticTexture = false;
		iLastScheduled = rtxn_ancestor.iLastScheduled;

		// Calculate the offset of this texture into the ancestor texture.
		CVector2<int> v2_offset(pqntOwner->pqvtGetVertex(3)->iX() - rtxn_ancestor.pqntOwner->pqvtGetVertex(3)->iX(),
		                        rtxn_ancestor.pqntOwner->pqvtGetVertex(3)->iY() - pqntOwner->pqvtGetVertex(3)->iY() );

		Assert(fmod(double(v2_offset.tX) * v2_ancestor_alloc_size.tX / rtxn_ancestor.pqntOwner->iGetSize(), 1) == 0);
		Assert(fmod(double(v2_offset.tY) * v2_ancestor_alloc_size.tY / rtxn_ancestor.pqntOwner->iGetSize(), 1) == 0);

		v2_offset.tX = (v2_offset.tX * v2_ancestor_alloc_size.tX) >> i_log2_ancestor_node_size;
		v2_offset.tY = (v2_offset.tY * v2_ancestor_alloc_size.tY) >> i_log2_ancestor_node_size;

		// Free the region in the page occupied by the ancestor texture. NOTE: we assume the ancestor allocation
		// handle remains valid!
		CCycleTimer ctmr;

		CTextureNode::ptexmTexturePages->DeleteRegion(NonConst(rtxn_ancestor).rhAlloc);

		if (rtxn_ancestor.ptexTexture->iGetNumMipLevels() != 0)
			NonConst(rtxn_ancestor).ptexTexture->ReassignMipLevel(rptr0);

		CTextureNode::ptexmTexturePages->AllocateRegion
		(
			rhAlloc,
			rtxn_ancestor.rhAlloc.iGetPage(),
			rtxn_ancestor.rhAlloc.v2GetOrigin() + v2_offset,
			v2_alloc_size,
			false			// Reusing ancestor texture, so no need to modify page.
		);

		if (rhAlloc)
		{
			priv_self.InitTexture(pqntin_root);

			tlr2ObjectTexture = rtxn_ancestor.tlr2ObjectTexture;
		}

		// Add KB count.
		proTex.psAlloc.Add(ctmr(), (v2_alloc_size.tX * v2_alloc_size.tY) / 1024.0);
	}


	//******************************************************************************************
	CTextureNode::~CTextureNode()
	{
		PurgeTexture();

		CPriv::lptxnTextures.remove(this);

		ptexTexture = rptr0;
	}


	//******************************************************************************************
	void CTextureNode::SetTextureMemSize()
	{
		// Normal allocation for 32 Mb machines.
		if (u4TotalPhysicalMemory() < (1 << 20) * 60)
			SetTextureMemSize(iDEFAULT_TERRAIN_TEXTURE_MEMORY_KB);
		
		// Large allocation for 64 Mb machines.
		SetTextureMemSize(iMAX_TERRAIN_TEXTURE_MEMORY_KB);
	}


	//******************************************************************************************
	void CTextureNode::SetTextureMemSize(int i_mem_size_kb)
	{
	#if bTRACK_D3D_RASTERS
		TrackSystem(etrTerrain);
	#endif
		CTextureNode::PurgeAllTextures();
		CTextureNode::ptexmTexturePages->DeleteRegion(CPriv::rhBaseTexture);

		CTextureNode::ptexmTexturePages->FreePages();
		CTextureNode::ptexmTextureStaticShadowPages->FreePages();

		// Set the maximum page size.
		if (d3dDriver.bUseD3D())
		{
			d3dDriver.SetTextureMinMaxSquare(iMaxTexturePageSize);
			iMaxTexturePageSize = d3dDriver.iGetRecommendedMaxDim();
		}
		else
			iMaxTexturePageSize = 256;

		if (i_mem_size_kb != 0)
		{
			// If hardware accelerated rendering is not in use or if we do region uploads, allocate all texture
			// pages at the maximum page size.
			if (!d3dDriver.bUseD3D() || d3dDriver.bUseRegionUploads())
			{
				CTextureNode::ptexmTexturePages->AllocPagesD3D(i_mem_size_kb * 1024, 0, 1, iMaxTexturePageSize);
			}
			else
			{
				iTexturePageSizeRange = 0;

				CTextureNode::ptexmTexturePages->AllocPagesD3D
				(
					i_mem_size_kb * 1024, iTexturePageSizeRange, fTexturePageCountFactor, iMaxTexturePageSize
				);
			}

			CTextureNode::ptexmTextureStaticShadowPages->AllocPages(iDEFAULT_TERRAIN_TEXTURE_MOVING_SHADOW_MEMORY_KB * 1024, 0, 1, iMaxTexturePageSize, false);

			Assert(CTextureNode::ptexmTexturePages->iGetMinRegionSize() == CTextureNode::ptexmTextureStaticShadowPages->iGetMinRegionSize());
			Assert(CTextureNode::ptexmTexturePages->iGetMaxRegionSize() == CTextureNode::ptexmTextureStaticShadowPages->iGetMaxRegionSize());

			// Make sure the existing clut will work for Direct3D.
			CPriv::UpdateCluts();
		}
	}


	//******************************************************************************************
	void CTextureNode::SetBaseTexture(rptr<CTexture> ptex)
	{
		CTextureNode::ptexmTexturePages->DeleteRegion(CPriv::rhBaseTexture);

		if (ptex)
			CPriv::prasBaseTexture = ptex->prasGetTexture();
		else
			CPriv::prasBaseTexture = rptr0;
	}

	//******************************************************************************************
	CColour CTextureNode::clrGetBaseTextureColour(const CQuadRootTIN* pqntin_root, TReal r_world_x, TReal r_world_y)
	{
		Assert(CPriv::prasBaseTexture != 0);

		// Map world coordinates to quad space coordinates.
		CVector2<> v2_quad = CVector2<>(r_world_x, r_world_y) * pqntin_root->mpConversions.tlr2WorldToQuad;

		if (!pqntin_root->bContains(v2_quad.tX, v2_quad.tY))
			return CColour(0, 0, 0);

		float f_scale_x = CPriv::prasBaseTexture->iWidth  * (1.0f / pqntin_root->iGetSize());
		float f_scale_y = CPriv::prasBaseTexture->iHeight * (1.0f / pqntin_root->iGetSize());

		TPixel pix_clr = CPriv::prasBaseTexture->pixGet(iPosFloatCast(f_scale_x * v2_quad.tX),
		                                                iPosFloatCast(f_scale_y * v2_quad.tY) );
 
		return CPriv::pclutCurrent->ppalPalette->aclrPalette[pix_clr];
	}

	//******************************************************************************************
	void CTextureNode::PurgeAllTextures()
	{
		for (CPriv::TList::iterator it = CPriv::lptxnTextures.begin(); it != CPriv::lptxnTextures.end(); ++it)
			(*it)->PurgeTexture();

		// We should not have any outstanding references at this point.
		#if VER_DEBUG
			int i_page;
			for (i_page = 0; i_page < CTextureNode::ptexmTexturePages->iGetNumPages(); i_page++)
			{
				Assert(CTextureNode::ptexmTexturePages->prasGetPage(i_page)->uNumRefs() == 2);
			}

			for (i_page = 0; i_page < CTextureNode::ptexmTextureStaticShadowPages->iGetNumPages(); i_page++)
			{
				Assert(CTextureNode::ptexmTextureStaticShadowPages->prasGetPage(i_page)->uNumRefs() == 2);
			}
		#endif
	}

	//******************************************************************************************
	void CTextureNode::FrameEnd()
	{
		for (CPriv::TList::iterator it = CPriv::lptxnTextures.begin(); it != CPriv::lptxnTextures.end(); ++it)
		{
			Assert((*it)->bInit || (*it)->iLastScheduled == -1);

			if ((*it)->bInit)
				(*it)->iLastScheduled++;

			// At every frame, each moving object adds its moving shadow rect to the terrain texture
			// nodes that intersect its shadow. We clear these here (at the end of this frame).
			(*it)->ltrcDirtyRects.erase((*it)->ltrcDirtyRects.begin(), (*it)->ltrcDirtyRects.end());
		}

		CPriv::bClipRegionInit = false;
	}

	//******************************************************************************************
	void CTextureNode::PrintStats(CConsoleBuffer& con)
	{
#if VER_TEST
		int i_num_init_tex_nodes             = 0;
		int i_num_init_tex_nodes_for_view    = 0;
		int i_num_dynamic_tex_nodes          = 0;
		int i_num_dynamic_tex_nodes_for_view = 0;
		int i_num_static_tex_nodes           = 0;
		int i_num_static_tex_nodes_for_view  = 0;
		int i_num_solid_tex_nodes            = 0;
		int i_num_solid_tex_nodes_for_view   = 0;

		int i_total_dynamic_texels = 0;
		int i_dynamic_texels_for_view = 0;

		for (CPriv::TList::iterator it = CPriv::lptxnTextures.begin(); it != CPriv::lptxnTextures.end(); ++it)
		{
			if ((*it)->bInit)
			{
				i_num_init_tex_nodes++;

				if ((*it)->iLastScheduled == 1)
					i_num_init_tex_nodes_for_view++;

				// Is this a dynamic texture?
				if (!(*it)->bStaticTexture && !(*it)->bSolidColour)
				{
					Assert((*it)->rhAlloc);

					i_num_dynamic_tex_nodes++;
 					i_total_dynamic_texels += (*it)->rhAlloc.v2GetSize().tX * (*it)->rhAlloc.v2GetSize().tY;

					if ((*it)->iLastScheduled == 1)
					{
						i_num_dynamic_tex_nodes_for_view++;
 						i_dynamic_texels_for_view += (*it)->rhAlloc.v2GetSize().tX * (*it)->rhAlloc.v2GetSize().tY;
					}
				}
				else if ((*it)->bStaticTexture)
				{
					Assert(!(*it)->bSolidColour && !(*it)->rhAlloc);

					i_num_static_tex_nodes++;

					if ((*it)->iLastScheduled == 1)
						i_num_static_tex_nodes_for_view++;
				}
				else
				{
					Assert((*it)->bSolidColour && !(*it)->bStaticTexture && !(*it)->rhAlloc);

					i_num_solid_tex_nodes++;

					if ((*it)->iLastScheduled == 1)
						i_num_solid_tex_nodes_for_view++;
				}
			}
 		}

		con.Print("Terrain tex     dyn  static  solid  total\n");
		con.Print(" Total # init : %3d    %3d    %3d    %3d\n", i_num_dynamic_tex_nodes, i_num_static_tex_nodes,
		                                                        i_num_solid_tex_nodes,   i_num_init_tex_nodes);
		con.Print(" Num visible  : %3d    %3d    %3d    %3d\n", i_num_dynamic_tex_nodes_for_view, i_num_static_tex_nodes_for_view,
		                                                        i_num_solid_tex_nodes_for_view,   i_num_init_tex_nodes_for_view);

		int i_avg_node_res = 0;

		if (i_total_dynamic_texels != 0 && i_num_dynamic_tex_nodes != 0)
			i_avg_node_res = iRound(sqrt(float(i_total_dynamic_texels) / i_num_dynamic_tex_nodes));
		
		con.Print(" Avg node res : %d^2\n", i_avg_node_res);
		con.Print(" Min mem req  : %dKB\n", (i_dynamic_texels_for_view * 2 + 512) / 1024);
#endif
	}

	//******************************************************************************************
	void CTextureNode::SetMovingObjShadows(const CRectangle<>& rc_quad_proj, const CMessageMove& msgmv)
	{
		if (bInit && bShadow)
		{
			switch (msgmv.etType)
			{
				case CMessageMove::etACTIVE :
				case CMessageMove::etMOVED :
 					break;

				case CMessageMove::etAWOKE :
					// This texture intersected a projected shadow from a static object that has been woken up.
					// We must kill this texture so that the object's old static shadow will be removed.
					PurgeTexture();
 					break;

				case CMessageMove::etSLEPT :
					// This texture intersected a projected shadow from a moving object that has gone to sleep.
					// We must kill this texture so that the object's new static shadow will be rendered.
					PurgeTexture();
					break;

				default :
					Assert(false);
					break;
			}
		}

		// Add the moving shadows rect for this object only if moving shadows are enabled.
		if (CTextureNode::bEnableMovingShadows)
			ltrcDirtyRects.push_back(rc_quad_proj);
	}

	//******************************************************************************************
	void CTextureNode::PurgeTexture()
	{
		CTextureNode::ptexmTexturePages->DeleteRegion(rhAlloc);
		CTextureNode::ptexmTextureStaticShadowPages->DeleteRegion(rhAllocShadow);

		ltrcPrevDirtyRects.erase(ltrcPrevDirtyRects.begin(), ltrcPrevDirtyRects.end());

		if (ptexTexture->iGetNumMipLevels() != 0)
			ptexTexture->ReassignMipLevel(rptr0);

		uint u_curr_ref = ptexTexture->uRefs;
		new (ptexTexture.ptPtrRaw()) CTexture(clrNotInit, &matDEFAULT);
		ptexTexture->uRefs = u_curr_ref;

		ptexTexture->seterfFeatures = seterfDEFAULT - erfLIGHT_SHADE;

		bInit           = false;
		iLastScheduled  = -1;
	}

	//******************************************************************************************
	void CTextureNode::ScheduleUpdate(const CQuadRootTIN* pqntin_root, CVector2<> v2_dim_req, bool b_shadow_req)
	{
		// Initialise base texture if required.
		if (!CPriv::rhBaseTexture)
			CPriv::InitBaseTexture();

		if (CTextureNode::bDisableTextures || CPriv::pclutCurrent == 0)
		{
			SetSolidTexture(pqntin_root);
			return;
		}

		Assert(iLastScheduled != 0);

		iLastScheduled = 0;

		// Validate the new properties and size for this texture.
		bShadowRequired = b_shadow_req && CTextureNode::bEnableShadows;

		//
		// Iterate through the list of dirty rects and remove any that do not intersect the (2d projection of)
		// the camera. This avoids rendering of moving shadows that are not visible.
		//
/*
		if (!ltrcDirtyRects.empty())
		{
			list< CRectangle<> >::iterator it = ltrcDirtyRects.begin();

			do
			{
				// Calculate the centre and squared radius of the bounding circle of this rectangle.
				CVector2<> v2_mid = (*it).v2Mid();
				TReal r_radius_sqr = Sqr((*it).tWidth() * .5f) + Sqr((*it).tHeight() * .5f);

				CClipRegion2D::CClipInfo cli_vis = CPriv::clip2dGet(pqntin_root).cliIntersects(v2_mid, r_radius_sqr);

				if (cli_vis == esfOUTSIDE)
				{
					// The bounding circle of the dirty rect is outside of the view volume, so
					// remove it from the list.
					list< CRectangle<> >::iterator it_curr = it;
					++it;

					ltrcDirtyRects.erase(it_curr);
				}
				else
					++it;
			}
			while (it != ltrcDirtyRects.end());
		}
*/

		// Determine if this texture intersects a shadow cast by a moving object. If it currently
		// has no buffer for moving shadows and moving shadows are enabled, kill this texture.
		if (!ltrcDirtyRects.empty() && !rhAllocShadow && bShadowRequired)
			PurgeTexture();

		//
		// Correlate the two dimensions to bias toward square textures, while keeping
		// the requested are the same. Effectively take the square root of the aspect ratio.
		// This tends to make the two dimensions update more often together, for fewer updates.
		//
		float f_dim_avg = iNearestPower2(fSqrtEst(v2_dim_req.tX * v2_dim_req.tY));
		v2_dim_req.tX = fSqrtEst(v2_dim_req.tX * f_dim_avg);
		v2_dim_req.tY = fSqrtEst(v2_dim_req.tY * f_dim_avg);

		//
		// Round texture dimensions to nearest power of two, and clamp to max size.
		//
		CVector2<int> v2_dim_new(iNearestPower2(v2_dim_req.tX), iNearestPower2(v2_dim_req.tY));

		int i_min_size = CTextureNode::ptexmTexturePages->iGetMinRegionSize();
		int i_max_size = CTextureNode::ptexmTexturePages->iGetMaxRegionSize();

		// Adjust max texture dimension by quality setting.
		i_max_size = Max
					(
						i_max_size >> (qdQualitySettings[iGetQualitySetting()].iMaxSizeReduction),
			            iMaxTexturePageSize >> (iTexturePageSizeRange + 1)        
					);

		SetMinMax(v2_dim_new.tX, i_min_size, i_max_size);
		SetMinMax(v2_dim_new.tY, i_min_size, i_max_size);

		// Always update uninitialised texures.
		if (!bInit)
		{
			v2DimRequired = v2_dim_new;
			priv_self.DoTextureUpdate(pqntin_root);
		}
		else
		{
			// Calculate the priority of the required rebuild of this texture.
			CVector2<int> v2_dim_cur;

			if (rhAlloc)
				v2_dim_cur = rhAlloc.v2GetSize();
			else if (bStaticTexture)
				v2_dim_cur = v2DimRequired;
			else
				v2_dim_cur = CVector2<int>(1, 1);

			//
			// Calculate the priority for this update.
			// In so doing, apply separate thresholds for increasing or decreasing resolution.
			// This tends to conserve textures, and reduce the frequency of updates.
			//

			CVector2<> v2_dim_diff;

			if (v2_dim_req.tX > v2_dim_cur.tX)
				v2_dim_diff.tX = v2_dim_req.tX - v2_dim_cur.tX * fIncreaseThreshold;
			else
				v2_dim_diff.tX = v2_dim_cur.tX * fDecreaseThreshold - v2_dim_req.tX;
			if (v2_dim_diff.tX <= 0)
				v2_dim_new.tX = v2_dim_cur.tX;

			if (v2_dim_req.tY > v2_dim_cur.tY)
				v2_dim_diff.tY = v2_dim_req.tY - v2_dim_cur.tY * fIncreaseThreshold;
			else
				v2_dim_diff.tY = v2_dim_cur.tY * fDecreaseThreshold - v2_dim_req.tY;
			if (v2_dim_diff.tY <= 0)
				v2_dim_new.tY = v2_dim_cur.tY;

			if (!rhAllocShadow && v2_dim_new == v2_dim_cur)
				return;

			if (rhAllocShadow)
			{
				// Always update moving shadows.
				v2DimRequired = v2_dim_new;
				priv_self.DoTextureUpdate(pqntin_root);
				return;
			}

			//
			// Calculate the priority for this update.
			//

			float f_priority = (Max(v2_dim_diff.tX, 0) + Max(v2_dim_diff.tY, 0)) * 
								.5f / i_max_size;

			// Clamp the value, as we may have requested a texture size larger than the max allowed.
			SetMin(f_priority, 1);
			Assert(f_priority >= 0);

			if (f_priority != 0)
			{
				//dout <<"Update " <<pqntOwner->iBaseX() <<"x" <<pqntOwner->iBaseY() <<"[" <<pqntOwner->iGetSize() <<"]: "
				//	<<v2_dim_cur <<" -> " <<v2_dim_req <<" -> " <<v2_dim_new <<", Pri " <<f_priority <<endl;

				v2DimRequired = v2_dim_new;
				CTextureNode::ptexmTexturePages->ClampRegionSize(v2DimRequired);

				shcSchedulerTerrainTextures.AddExecution
				(
					new (shcSchedulerTerrainTextures) CScheduleTerrainTextureItem(this, pqntin_root, f_priority)
				);
			}
		}
	}


	//******************************************************************************************
	void CTextureNode::SetStaticTexture(const CQuadRootTIN* pqntin_root)
	{
		// Initialise base texture if required.
		if (!CPriv::rhBaseTexture)
			CPriv::InitBaseTexture();

		if (CTextureNode::bDisableTextures || !CPriv::rhBaseTexture)
		{
			SetSolidTexture(pqntin_root);
			return;
		}

		if (!bInit || !bStaticTexture)
		{
			PurgeTexture();

			bShadow        = false;
			bStaticTexture = true;
			bSolidColour   = false;


			int i_log2_root_node_size = uLog2(pqntin_root->iGetSize());
			int i_shift               = i_log2_root_node_size - uLog2(pqntOwner->iGetSize());

			Assert(i_shift > 0);

			// Set the required size of this texture node as a fraction of the base's texture size.
			v2DimRequired.tX = CPriv::prasBaseTexture->iWidth  >> i_shift;
			v2DimRequired.tY = CPriv::prasBaseTexture->iHeight >> i_shift;

			if (v2DimRequired.tX == 0 || v2DimRequired.tY == 0)
			{
				SetSolidTexture(pqntin_root);
				return;
			}

			priv_self.InitTexture(pqntin_root);


			// Map quad space coordinates to a unit rectangle.
			float f_inv_node_size = 1.0f / pqntin_root->iGetSize();

			tlr2ObjectTexture = CTransLinear2<>
			(
				CTransLinear<>( f_inv_node_size, -pqntin_root->iBaseX() * f_inv_node_size),
				CTransLinear<>(f_inv_node_size,  -pqntin_root->iBaseY() * f_inv_node_size)
			);

			// Map texture coordinates from the base texture's allocated region to the page.
			tlr2ObjectTexture *= CPriv::rhBaseTexture.tlr2ToPage();
		}

		iLastScheduled = 0;
	}


	//******************************************************************************************
	void CTextureNode::SetSolidTexture(const CQuadRootTIN* pqntin_root)
	{
		if (!bInit || !bSolidColour)
		{
			PurgeTexture();

			bShadow        = false;
			bStaticTexture = false;
			bSolidColour   = true;

			priv_self.InitTexture(pqntin_root);
		}

		iLastScheduled = 0;
	}


//**********************************************************************************************
//
// CTextureNode::CPriv implementation.
//

	//******************************************************************************************
	CClut* CTextureNode::CPriv::pclutCurrent  = 0;
	CClut* CTextureNode::CPriv::pclutSoftware = 0;
	CClut* CTextureNode::CPriv::pclutDirect3D = 0;

	CPalClut* CTextureNode::CPriv::ppcePalClut = 0;

	int CTextureNode::CPriv::iShadowLightVal = 0;

	CClipRegion2D CTextureNode::CPriv::clip2dCam;
	bool CTextureNode::CPriv::bClipRegionInit = false;

	rptr<CRaster> CTextureNode::CPriv::prasBaseTexture = rptr0;
	CTexturePageManager::CRegionHandle CTextureNode::CPriv::rhBaseTexture;


	CTextureNode::CPriv::TList CTextureNode::CPriv::lptxnTextures;


	//******************************************************************************************
	void CTextureNode::CPriv::GatherDscDirtyRects(CTextureNode* ptxn_ancestor, const CQuadNodeTIN* pqntin_node)
	{
		if (pqntin_node->ptxnGetTextureNode() != 0)
		{
			std::list< CRectangle<> >::const_iterator it;
			for (it = pqntin_node->ptxnGetTextureNode()->ltrcDirtyRects.begin(); it != pqntin_node->ptxnGetTextureNode()->ltrcDirtyRects.end(); ++it)
				ptxn_ancestor->ltrcDirtyRects.push_back(*it);
		}
		else if (pqntin_node->bHasDescendants())
		{
			const CQuadNodeTIN* pqntin_dsc = pqntin_node->ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				CPriv::GatherDscDirtyRects(ptxn_ancestor, pqntin_dsc);

				pqntin_dsc = pqntin_dsc->ptqnGetSibling();
			}
		}
	}

	//******************************************************************************************
	const CClipRegion2D& CTextureNode::CPriv::clip2dGet(const CQuadRootTIN* pqntin_root)
	{
		if (!CPriv::bClipRegionInit)
		{
			CPriv::clip2dCam.Init(*pwWorld->pcamGetActiveCamera());

			// Calculate the world to quad space transform.
			CPresence3<> pr3_world_to_quad
			(
				CRotate3<>(),
				pqntin_root->mpConversions.tlr2WorldToQuad.tlrX.tScale,
				CVector3<>(pqntin_root->mpConversions.tlr2WorldToQuad.tlrX.tOffset,
						   pqntin_root->mpConversions.tlr2WorldToQuad.tlrY.tOffset, 0)
			);

			clip2dCam *= pr3_world_to_quad;

			CPriv::bClipRegionInit = true;
		}

		return CPriv::clip2dCam;
	}

	//******************************************************************************************
	void CTextureNode::CPriv::DoTextureUpdate(const CQuadRootTIN* pqntin_root)
	{
		// Make sure the owning quad node refers to this texture node.
		Assert(pqntOwner != 0 && pqntOwner->ptxnGetTextureNode() == this);

		// Determine if this texture needs to be re-rendered, based on the new properties and required size.
		if (bInit && !bSolidColour && !bStaticTexture &&
		    (bShadow == bShadowRequired) && (v2DimRequired == rhAlloc.v2GetSize()))
		{
			Assert(rhAlloc);

			// This texture does not need to be re-rendered. Determine if we need to update it for
			// moving shadows.
			if (rhAllocShadow)
				RenderMovingShadows(pqntin_root);
		}
		else
		{
			CCycleTimer ctmr;

			if (rhAlloc && (bShadow == bShadowRequired) &&
			    (v2DimRequired.tX >= rhAlloc.v2GetSize().tX || v2DimRequired.tY >= rhAlloc.v2GetSize().tY))
			{
				// We are attempting to re-render an existing texture at a higher resolution. If the allocation fails,
				// we re-use the current texture (no re-rendering is necessary).
				if (!bReAllocRegion())
					return;
			}
			else
			{
				PurgeTexture();

				bShadow        = bShadowRequired;
				bSolidColour   = false;
				bStaticTexture = false;

				// Attempt to allocate a page region for the new texture.
				AllocRegion();
			}

			// Add KB count.
			proTex.psAlloc.Add(ctmr(), (v2DimRequired.tX * v2DimRequired.tY) / 1024.0);

			if (!rhAlloc)
			{
				// Set up static texture.
				SetStaticTexture(pqntin_root);
				return;
			}

			InitTexture(pqntin_root);
			RenderTexture(pqntin_root);
		}
	}


	//******************************************************************************************
	void CTextureNode::CPriv::AllocRegion()
	{
		Assert(!rhAlloc && !rhAllocShadow && !bSolidColour && !bStaticTexture);
		Assert(bWithin(v2DimRequired.tX, CTextureNode::ptexmTexturePages->iGetMinRegionSize(), CTextureNode::ptexmTexturePages->iGetMaxRegionSize()));
		Assert(bWithin(v2DimRequired.tY, CTextureNode::ptexmTexturePages->iGetMinRegionSize(), CTextureNode::ptexmTexturePages->iGetMaxRegionSize()));

		// Do not allocate a texture if we don't have a valid clut.
		if (CPriv::pclutCurrent == 0)
			return;

		CTextureNode::ptexmTexturePages->AllocateRegion(rhAlloc, v2DimRequired, !d3dDriver.bUseRegionUploads());

		// If required, allocate the pre-clut textured and static shadow raster copy.
		bool b_shadow_buf_req = bShadow && !ltrcDirtyRects.empty();

		if (b_shadow_buf_req)
			CTextureNode::ptexmTextureStaticShadowPages->AllocateRegion(rhAllocShadow, v2DimRequired, false);

		// Were the allocations successful?
		if (rhAlloc && (!b_shadow_buf_req || rhAllocShadow))
			return;


		// One of the allocations failed, we may need to purge an old texture until the allocation succeeds.
		// Sort the texture nodes so that the first element is the oldest (has the highest iLastScheduled count)..
		CPriv::lptxnTextures.sort();

		do
		{
			if (!rhAlloc || (b_shadow_buf_req && !rhAllocShadow))
			{
				int i_max_req_dim = Max(v2DimRequired.tX, v2DimRequired.tY);

				// Go through this list of textures, deleting the oldest, until we reach the end of the list
				// or we hit a texture that was accessed this frame.
				for (TList::iterator it = CPriv::lptxnTextures.begin(); it != CPriv::lptxnTextures.end() && (*it)->iLastScheduled > 0; ++it)
				{
					bool b_purge_curr = false;

					// Determine if the current texture was allocated from a page that would be large enough
					// to hold the new texture that we are trying to allocate. If it is not, we do not purge
					// that texture.
					b_purge_curr |= !rhAlloc && (*it)->rhAlloc && CTextureNode::ptexmTexturePages->iGetPageSize((*it)->rhAlloc.iGetPage()) >= i_max_req_dim;

					// Always purge if we need a shadow buffer and the current texture has a shadow buffer.
					b_purge_curr |= b_shadow_buf_req && !rhAllocShadow && (*it)->rhAllocShadow;

					if (b_purge_curr)
					{
						(*it)->PurgeTexture();

						if (!rhAlloc)
							CTextureNode::ptexmTexturePages->AllocateRegion(rhAlloc, v2DimRequired, !d3dDriver.bUseRegionUploads());

						if (b_shadow_buf_req && !rhAllocShadow)
							CTextureNode::ptexmTextureStaticShadowPages->AllocateRegion(rhAllocShadow, v2DimRequired, false);

						if (rhAlloc && (!b_shadow_buf_req || rhAllocShadow))
							break;
					}
				}
			}

			// If the allocation is still unsuccessful, we decimate the required sizes and attempt
			// to allocate again.
			if (!rhAlloc || (b_shadow_buf_req && !rhAllocShadow))
			{
				int i_min_tex_size = CTextureNode::ptexmTexturePages->iGetMinRegionSize();

				int* pi_major = v2DimRequired.tX >= v2DimRequired.tY ? &v2DimRequired.tX : &v2DimRequired.tY;

				// Is the texture size already at the lower limit?
				if (*pi_major <= i_min_tex_size)
					break;

				// Decimate the major axis.
				*pi_major >>= 1;

				CTextureNode::ptexmTexturePages->ClampRegionSize(v2DimRequired);

				CTextureNode::ptexmTexturePages->DeleteRegion(rhAlloc);
				CTextureNode::ptexmTexturePages->AllocateRegion(rhAlloc, v2DimRequired, !d3dDriver.bUseRegionUploads());

				if (b_shadow_buf_req)
				{
					CTextureNode::ptexmTextureStaticShadowPages->DeleteRegion(rhAllocShadow);
					CTextureNode::ptexmTextureStaticShadowPages->AllocateRegion(rhAllocShadow, v2DimRequired, false);
				}
			}
		}
		while (!rhAlloc || (b_shadow_buf_req && !rhAllocShadow));
	}


	//******************************************************************************************
	bool CTextureNode::CPriv::bReAllocRegion()
	{
		Assert(rhAlloc && bInit && !bSolidColour && !bStaticTexture);
		Assert(!rhAllocShadow || (rhAlloc.v2GetSize() == rhAllocShadow.v2GetSize()));

		// We are attempting to re-allocate an existing texture at a higher resolution. In case the
		// allocation of the new texture size fails, we reuse the existing one.
		int i_old_page = rhAlloc.iGetPage();
		CVector2<int> v2_old_origin = rhAlloc.v2GetOrigin();
		CVector2<int> v2_old_size   = rhAlloc.v2GetSize();

		int i_old_page_shadow;
		CVector2<int> v2_old_origin_shadow;

		if (rhAllocShadow)
		{
			i_old_page_shadow    = rhAllocShadow.iGetPage();
			v2_old_origin_shadow = rhAllocShadow.v2GetOrigin();
		}

		// Do not realloc the shadow buffer if there are no dirty rects.
		bool b_had_shadow_buf = rhAllocShadow && !ltrcDirtyRects.empty();

		// Free the region in the page occupied by the existing texture and shadow buffer. NOTE: we assume the
		// rasters remain valid!
		CTextureNode::ptexmTexturePages->DeleteRegion(rhAlloc);
		CTextureNode::ptexmTextureStaticShadowPages->DeleteRegion(rhAllocShadow);

		// Attempt to allocate a page region for the new texture size.
		AllocRegion();

		// The allocation must always be successful, because at the very least we should be able to
		// allocate a texture and shadow buffer of the current size.
		Assert(rhAlloc && (!b_had_shadow_buf || rhAllocShadow));

		// If the allocation at the new size failed, restore the original texture and shadow buffer.
		if (rhAlloc && rhAlloc.v2GetSize() == v2_old_size)
		{
			// Reuse the original texture raster.
			CTextureNode::ptexmTexturePages->DeleteRegion(rhAlloc);
			CTextureNode::ptexmTexturePages->AllocateRegion(rhAlloc, i_old_page, v2_old_origin, v2_old_size, false);

			if (b_had_shadow_buf)
			{
				// Restore shadow buffer.
				CTextureNode::ptexmTextureStaticShadowPages->DeleteRegion(rhAllocShadow);
				CTextureNode::ptexmTextureStaticShadowPages->AllocateRegion(rhAllocShadow, i_old_page_shadow, v2_old_origin_shadow, v2_old_size, false);
			}

			Assert(rhAlloc && v2DimRequired == rhAlloc.v2GetSize());
			return false;
		}
		else
		{
			Assert(ptexTexture->iGetNumMipLevels() == 1);

			ptexTexture->ReassignMipLevel(rptr0);

			bInit = false;

			return true;
		}
	}


	//******************************************************************************************
	void CTextureNode::CPriv::InitTexture(const CQuadRootTIN* pqntin_root)
	{
		Assert(!bInit);

		bInit = true;

		if (CPriv::pclutCurrent == 0)
			return;

		if (bSolidColour)
		{
			Assert(!rhAlloc && !bShadow && !bStaticTexture);
			Assert(ptexTexture->iGetNumMipLevels() == 0);
			Assert(CPriv::prasBaseTexture != 0);

			// Determine colour for solid texture.
			// Map midpoint of this node into the base texture space.
			int i_mid_x = pqntOwner->iBaseX() + (pqntOwner->iGetSize() >> 1);
			int i_mid_y = pqntOwner->iBaseY() + (pqntOwner->iGetSize() >> 1);

			float f_scale_x = CPriv::prasBaseTexture->iWidth  * (1.0f / pqntin_root->iGetSize());
			float f_scale_y = CPriv::prasBaseTexture->iHeight * (1.0f / pqntin_root->iGetSize());

			TPixel pix_clr = CPriv::prasBaseTexture->pixGet(iPosFloatCast(f_scale_x * i_mid_x), iPosFloatCast(f_scale_y * i_mid_y));
			CColour clr    = CPriv::pclutCurrent->ppalPalette->aclrPalette[pix_clr];

			uint u_curr_ref = ptexTexture->uRefs;
			new (ptexTexture.ptPtrRaw()) CTexture
			(
				const_cast<CPal*>(CPriv::pclutCurrent->ppalPalette),
				pix_clr,
				clr,
				CPriv::pclutCurrent->pmatMaterial
			);
			ptexTexture->uRefs = u_curr_ref;

			ptexTexture->seterfFeatures = seterfDEFAULT - erfLIGHT_SHADE + erfSOURCE_TERRAIN + erfTRAPEZOIDS;
		}
		else
		{
			Assert( bStaticTexture || rhAlloc);
			Assert(!bStaticTexture || (!rhAlloc && !bShadow && !bSolidColour));
			Assert(ptexTexture->iGetNumMipLevels() < 2);

			if (ptexTexture->iGetNumMipLevels() != 0)
				ptexTexture->ReassignMipLevel(rptr0);

			// Get the raster.
			rptr<CRaster> pras;

			if (bStaticTexture)
				pras = CTextureNode::ptexmTexturePages->prasGetPage(CPriv::rhBaseTexture.iGetPage());
			else
				pras = CTextureNode::ptexmTexturePages->prasGetPage(rhAlloc.iGetPage());

			uint u_curr_ref = ptexTexture->uRefs;
			new (ptexTexture.ptPtrRaw()) CTexture
			(
				pras,
				CPriv::pclutCurrent->pmatMaterial,
				pras->pixFromColour(CTerrainObj::clrAverage)
			);
			ptexTexture->uRefs = u_curr_ref;

			ptexTexture->seterfFeatures = seterfDEFAULT - erfLIGHT - erfLIGHT_SHADE - erfSPECULAR + erfTRAPEZOIDS + 
										  erfTEXTURE + erfCOPY + erfSOURCE_TERRAIN + erfFILTER;
		}

		// Use a special clut for Direct3D.
		if (d3dDriver.bUseD3D())
			ptexTexture->ppcePalClut = CPriv::ppcePalClut;

		// HACK: Set global variable needed for terrain fogging.
		ppceTerrainClut = ptexTexture->ppcePalClut;
		Assert(ppceTerrainClut != 0);
	}


	//******************************************************************************************
	void CTextureNode::CPriv::RenderTexture(const CQuadRootTIN* pqntin_root)
	{
		// Make sure the owning quad node refers to this texture node.
		Assert(pqntOwner != 0 && pqntOwner->ptxnGetTextureNode() == this);
		Assert(bInit && !bSolidColour && !bStaticTexture);
		Assert(rhAlloc && v2DimRequired == rhAlloc.v2GetSize());

		CCycleTimer	ctmr;
		CCycleTimer ctmr2;

		//
		// Calculate the world space extents of this texture node and calculate the mapping from
		// (quad space) node coordinates to texture coordinates.
		//
		CVector2<int> v2_size = rhAlloc.v2GetSize();
		Assert(v2_size.tX > 2 && v2_size.tY > 2);

		float f_inv_u_size = 1.0 / v2_size.tX;
		float f_inv_v_size = 1.0 / v2_size.tY;

		CRectangle<> rc_world_node = pqntOwner->rcWorldRectangle(pqntin_root->mpConversions);

		// Calculate the dimensions of a texel in world space.
		float f_texel_world_width  = rc_world_node.tWidth()  * f_inv_u_size;
		float f_texel_world_height = rc_world_node.tHeight() * f_inv_v_size;

		// Extent world coverage of this texture by one texel in all directions (e.g. -u, +u, -v, +v) to avoid
		// artefacts at the texture seams due to bilinear filtering. We extend in all directions instead of,
		// say, +u, +v only, because Direct3D does not specify the texel picking algorithm wrt to filtering.
		rc_world_node = CRectangle<>
		(
			rc_world_node.tX0()     - f_texel_world_width,
			rc_world_node.tY0()     - f_texel_world_height,
			rc_world_node.tWidth()  + f_texel_world_width  * 2,
			rc_world_node.tHeight() + f_texel_world_height * 2
		);


		// Map quad space coordinates to a unit rectangle shrunk in all dimension by one texel, with
		// the Y axis flipped.
		float f_inv_node_size = 1.0f / pqntOwner->iGetSize();
		float f_map_u_scale = f_inv_node_size * ( 1 - 2 * f_inv_u_size);
		float f_map_v_scale = f_inv_node_size * (-1 + 2 * f_inv_v_size);

		tlr2ObjectTexture = CTransLinear2<>
		(
			CTransLinear<>(f_map_u_scale, f_inv_u_size - pqntOwner->iBaseX() * f_map_u_scale),
			CTransLinear<>(f_map_v_scale, (1 - f_inv_v_size) - pqntOwner->iBaseY() * f_map_v_scale)
		);

		// Map texture coordinates from the allocated region to the page.
		tlr2ObjectTexture *= rhAlloc.tlr2ToPage();


		// Un-adjust for the texture coordinate adjustment that happens inside the software rasteriser, so to avoid
		// seams between neighbouring textures and texture 'swimming' artefacts when combining or decimating textures.
		// The rasteriser adjusts texture coordinates to avoid walking outside of the bounds of the texture when
		// rendering. We do not have to worry about this when rendering terrain textures because these have a
		// single texel border around them.

#if (!BILINEAR_FILTER)	/* temporary HACK to prevent crashes with software filtering */

		if (!d3dDriver.bUseD3D())
		{
			float f_inv_page_size = 1.0f / CTextureNode::ptexmTexturePages->iGetPageSize(rhAlloc.iGetPage());

			float f_unit_adj_u = fTexEdgeTolerance * f_inv_page_size;
			float f_unit_adj_v = fTexEdgeTolerance * f_inv_page_size;

			CTransLinear2<> tlr2_adj
			(
				CTransLinear<>(1.0f + f_unit_adj_u * 2, -f_unit_adj_u),
				CTransLinear<>(1.0f + f_unit_adj_v * 2, -f_unit_adj_v)
			);

			tlr2ObjectTexture *= tlr2_adj;
		}

#endif

		//
		// Create and position a parallel camera overhead this node.
		//

		// Move the camera up from the world, and point it downward.
		const float fCAM_HEIGHT = 1000;

		// Fudge the camera's world position by some amount. In many cases (depending on the resolution and positions
		// of the terrain texture objects) this will avoid sampling texels from the terrain texture objects (exactly) at
		// their top-left corner. This makes the rasterisation of the terrain texture objects highly sensitive to
		// the slightest roundof errors. In turn, this can cause artefacts at the seams between terrain texture objects.
		const float fCAM_XY_ADJUST = .015f;

		CVector3<> v3_cam_pos(rc_world_node.tXMid() + fCAM_XY_ADJUST, rc_world_node.tYMid() + fCAM_XY_ADJUST, fCAM_HEIGHT);
		CPlacement3<> p3_camera(CRotate3<>(d3YAxis, -d3ZAxis), v3_cam_pos);

		// Create the parallel overhead terrain camera properties.
		CCamera::SProperties camprop;
		camprop.bPerspective       = false;
		camprop.bClipNearFar       = false;
		camprop.rViewWidth         = rc_world_node.tWidth() / 2;
		camprop.fAspectRatio       = rc_world_node.tWidth() / rc_world_node.tHeight();
		camprop.rNearClipPlaneDist = 0;
		camprop.rFarClipPlaneDist  = fCAM_HEIGHT * 2;

		// Construct a temporary raster for the region in the page.
		CVector2<int> v2_origin = rhAlloc.v2GetOrigin();

		rptr<CRaster> pras_region = rptr_cast(CRaster, rptr_new CRasterMem
		(
			CTextureNode::ptexmTexturePages->prasGetPage(rhAlloc.iGetPage()),
			SRect(v2_origin.tX, v2_origin.tY, v2_size.tX, v2_size.tY)
		));

		// Set the viewport screen width to correspond to the raster size.
		camprop.vpViewport.SetSize(pras_region->iWidth, pras_region->iHeight);

		// Create the camera.
		CCamera cam(p3_camera, camprop);


		// Construct a terrain screen renderer and renderer with this raster (and no Z buffer).
		SRenderSettingsTerrain rnsettrr;
		ptr<CScreenRender> pscren = new CScreenRenderTerrain(&rnsettrr, pras_region);
		CRenderer ren(pscren, &rnsettrr);


		//
		// Query for the terrain texture objects that intersect the extent of this node and render
		// these into the terrain texture.
		//

		// Find terrain objects in this region. Sort and cull them.
		CWDbQueryTerrainObj wqtrrobj(&cam, true);

		proTex.psQuery.Add(ctmr2(), wqtrrobj.size());

		pscren->BeginFrame();

		if (CTextureNode::bClears)
			pras_region->Clear(pras_region->pixFromColour(CTerrainObj::clrAverage));

		ren.RenderScene(cam, wqtrrobj);

		proTex.psTexture.Add(ctmr2(), 1);


		//
		// Add the lighting to the composite map.
		//

		// Get the lights whose influence intersects the camera's bounding volume,
		// and construct the light list.
		aptr<CLightList> pltl = new CLightList(CWDbQueryLights(&cam));

		// Set lighting relative to terrain geometry.  
		// To do: get actual terrain presence, rather than assuming identity.
		pltl->SetViewingContext(CPresence3<>(), CPresence3<>());

		// Extract the clut from the texture.
		const CClut& clut = *ptexTexture->ppcePalClut->pclutClut;

		pqntOwner->GenerateLighting(pras_region, *pltl, clut, pqntin_root);

		pscren->EndFrame();

#if VER_TEST
		// Debugging feature: draw node outline.
		if (CTextureNode::bOutlineNodes)
		{
			// For texture nodes, draw border in bright blue (colour 0).
			for (int i_x = 2; i_x < pras_region->iWidth - 2; i_x++)
			{
				pras_region->PutPixel(i_x, 2,                        30 << 8);
				pras_region->PutPixel(i_x, pras_region->iHeight - 3, 30 << 8);
			}
			for (int i_y = 3; i_y < pras_region->iHeight - 3; i_y++)
			{
				pras_region->PutPixel(2,                       i_y, 30 << 8);
				pras_region->PutPixel(pras_region->iWidth - 3, i_y, 30 << 8);
			}

			if (pqntOwner->bHasDescendants())
			{
				// Draw subdivisions in black (light value 0).
				int i_xmid = pras_region->iWidth  / 2;
				int i_ymid = pras_region->iHeight / 2;
				for (int i_x = 2; i_x < pras_region->iWidth - 2; i_x++)
					pras_region->PutPixel(i_x, i_ymid, 0);
				for (int i_y = 3; i_y < pras_region->iHeight - 3; i_y++)
					pras_region->PutPixel(i_xmid, i_y, 0);
			}


			// For nodes with static shadow buffers, draw bright blue cross (colour 0).
			if (rhAllocShadow)
			{
				int i_cx = pras_region->iWidth / 2;
				int i_cy = pras_region->iHeight / 2;

				for (int i = 0; i < Min(pras_region->iWidth, pras_region->iHeight) / 2 - 4; i++)
				{
					pras_region->PutPixel(i_cx + i, i_cy + i, 30 << 8);
					pras_region->PutPixel(i_cx - i, i_cy + i, 30 << 8);
					pras_region->PutPixel(i_cx + i, i_cy - i, 30 << 8);
					pras_region->PutPixel(i_cx - i, i_cy - i, 30 << 8);
				}
			}
		}

		if (CTextureNode::bShowConstrained && pqntOwner->stState[CQuadRootTIN::estTEX_SUBDIV_CONSTRAINED])
		{
			for (int i_y = 0; i_y < pras_region->iHeight - 1; i_y += 2)
				for (int i_x = 0; i_x < pras_region->iWidth - 1; i_x += 2)
					pras_region->PutPixel(i_x, i_y, 30 << 8);
		}
//VER_TEST
#endif

		proTex.psLight.Add(ctmr2(), 1);

		if (bShadow)
		{
			//
			// Shadow the map.  
			// Light direction is taken from blt.drdPrimary (which points *toward* light), and negated.
			// Shadow value is determined by blt.rvBase (basically, ambient light),
			// and the clut range of the texture.
			//

			// Extract the primary and ambient light information, indirectly via bltGetBumpLighting.
			SBumpLighting blt = pltl->bltGetBumpLighting();
			CTextureNode::d3ShadowingLight_ = -blt.d3Light;

			CPriv::iShadowLightVal = clut.cvFromReflect(clut.pmatMaterial->rvCombined(blt.lvAmbient, 0));

			// Do the static shadows.
			ShadowLighting
			(
				pras_region, 
				rc_world_node,
				CTextureNode::d3ShadowingLight_,
				CPriv::iShadowLightVal,
				false,			// Do not render moving shadows.
				true			// Render static shadows.
			);

			proTex.psShadow.Add(ctmr2(), 1);

			// Make a copy of the current pre-clut converted, textured with static shadows raster.
			if (rhAllocShadow)
			{
				Assert(rhAlloc.v2GetSize() == rhAllocShadow.v2GetSize());

				rptr<CRaster> pras_shadow = rptr_cast(CRaster, rptr_new CRasterMem
				(
					CTextureNode::ptexmTextureStaticShadowPages->prasGetPage(rhAllocShadow.iGetPage()),
					SRect(rhAllocShadow.v2GetOrigin().tX, rhAllocShadow.v2GetOrigin().tY, v2_size.tX, v2_size.tY)
				));

				CopyRaster(pras_shadow, pras_region);

				ltrcPrevDirtyRects.erase(ltrcPrevDirtyRects.begin(), ltrcPrevDirtyRects.end());
			}
		}

		priv_self.ClutConvert(pras_region);

		// Get elapsed cycles.		
		TCycles cy_terrain_tex = ctmr();
		proTex.psTerrainTex.Add(cy_terrain_tex, 1);

		// Render moving shadows.
		if (rhAllocShadow)
			RenderMovingShadows(pqntin_root);

#if (VER_TIMING_STATS)
		// This may be scheduled, so don't count it in the scheduler time.
		CScheduler::cyAccountedScheduleCycles += cy_terrain_tex;
#endif
	}


	//******************************************************************************************
	void CTextureNode::CPriv::RenderMovingShadows(const CQuadRootTIN* pqntin_root)
	{
		Assert(bInit && bShadow && !bSolidColour && !bStaticTexture);
		Assert(rhAllocShadow);
		Assert(rhAlloc.v2GetSize() == rhAllocShadow.v2GetSize());

		CTimeBlock tmb(&proTex.psShadowMoving);

		//
		// Build a list of rectangle areas in this texture that need to be updated for moving shadows.
		// This list must include the dirty rectangles of the previous update and the dirty rectangles
		// of the current frame.
		//
		std::list< CRectangle<int> > lrc_this_frame;

		// Add the dirty rects from the previous update.
		std::list< CRectangle<int> >::iterator it_prev;
		for (it_prev = ltrcPrevDirtyRects.begin(); it_prev != ltrcPrevDirtyRects.end(); ++it_prev)
			lrc_this_frame.push_back(*it_prev);

		ltrcPrevDirtyRects.erase(ltrcPrevDirtyRects.begin(), ltrcPrevDirtyRects.end());


		CVector2<int> v2_origin = rhAlloc.v2GetOrigin();
		CVector2<int> v2_size   = rhAlloc.v2GetSize();

		Assert(v2_size.tX > 2 && v2_size.tY > 2);

		// Add the dirty rects for the current frame.
		if (!ltrcDirtyRects.empty())
		{
			CTransLinear2<> tlr2_quad_inv_raster(pqntOwner->rcGetRectangle(), CRectangle<>(0, 0, v2_size.tX, v2_size.tY));

			std::list< CRectangle<int> > lrc_next_frame;

			std::list< CRectangle<> >::iterator it_curr;
			for (it_curr = ltrcDirtyRects.begin(); it_curr != ltrcDirtyRects.end(); ++it_curr)
			{
				// Convert the dirty rect from quad space to texture raster coords. Add an extra few pixels in all
				// dimensions for safety and clip against the extends of the raster.
				CRectangle<> rc = *it_curr * tlr2_quad_inv_raster;

				int i_xa = Max(int(rc.v2Start().tX) - 2 & ~1, 0);
				int i_xb = Min(int(  rc.v2End().tX) + 3 & ~1, v2_size.tX);
				int i_ya = Max(v2_size.tY - (int(  rc.v2End().tY) + 3), 0);
				int i_yb = Min(v2_size.tY - (int(rc.v2Start().tY) - 2), v2_size.tY);
				CRectangle<int> rc_clipped(i_xa, i_ya, i_xb - i_xa, i_yb - i_ya);

				// Ignore this rect if it is entirely clipped away.
				if (rc_clipped.tWidth() > 0 && rc_clipped.tHeight() > 0)
				{
					// Merge the rect into the list for the current frame and also for the next.
					CPriv::MergeInList(lrc_this_frame, rc_clipped);
					CPriv::MergeInList(lrc_next_frame, rc_clipped);
				}
			}

			for (std::list< CRectangle<int> >::iterator it_next = lrc_next_frame.begin(); it_next != lrc_next_frame.end(); ++it_next)
				ltrcPrevDirtyRects.push_back(*it_next);
		}


		// Construct temporary rasters for the regions in the pages.
		rptr<CRaster> pras_region = rptr_cast(CRaster, rptr_new CRasterMem
		(
			CTextureNode::ptexmTexturePages->prasGetPage(rhAlloc.iGetPage()),
			SRect(v2_origin.tX, v2_origin.tY, v2_size.tX, v2_size.tY)
		));

		rptr<CRaster> pras_shadow = rptr_cast(CRaster, rptr_new CRasterMem
		(
			CTextureNode::ptexmTextureStaticShadowPages->prasGetPage(rhAllocShadow.iGetPage()),
			SRect(rhAllocShadow.v2GetOrigin().tX, rhAllocShadow.v2GetOrigin().tY, v2_size.tX, v2_size.tY)
		));

		// Copy regions from the buffer containing the non-clut converted texture and static shadows.
		std::list< CRectangle<int> >::iterator it_this = lrc_this_frame.begin();
		for (; it_this != lrc_this_frame.end(); ++it_this)
			CPriv::CopyRaster(pras_region, pras_shadow, &(*it_this));

		if (ltrcPrevDirtyRects.empty())
		{
			// This texture node is set up for moving shadows, but there are no moving shadows intersecting
			// it anymore. We can delete the raster containing the pre-clut texture and static shadows.
			CTextureNode::ptexmTextureStaticShadowPages->DeleteRegion(rhAllocShadow);
		}
		else
		{
			std::list< CRectangle<int> >::iterator it_new;
			for (it_new = ltrcPrevDirtyRects.begin(); it_new != ltrcPrevDirtyRects.end(); ++it_new)
			{
				#if VER_TEST
					// Debugging feature: draw moving shadow rectangle outline.
					if (CTextureNode::bOutlineNodes)
					{
						for (int i_x = (*it_new).tX0(); i_x < (*it_new).tX1(); i_x++)
						{
							pras_region->PutPixel(i_x, (*it_new).tY0(),     30 << 8);
							pras_region->PutPixel(i_x, (*it_new).tY1() - 1, 30 << 8);
						}
						for (int i_y = (*it_new).tY0(); i_y < (*it_new).tY1(); i_y++)
						{
							pras_region->PutPixel((*it_new).tX0(),     i_y, 30 << 8);
							pras_region->PutPixel((*it_new).tX1() - 1, i_y, 30 << 8);
						}
					}
				#endif
			}


			CRectangle<> rc_world_node = pqntOwner->rcWorldRectangle(pqntin_root->mpConversions);

			// Calculate the dimensions of a texel in world space.
			float f_texel_world_width  = rc_world_node.tWidth()  / v2_size.tX;
			float f_texel_world_height = rc_world_node.tHeight() / v2_size.tY;

			// Extent world coverage of this texture by one texel in all directions (e.g. -u, +u, -v, +v) to avoid
			// artefacts at the texture seams due to bilinear filtering. We extend in all directions instead of,
			// say, +u, +v only, because Direct3D does not specify the texel picking algorithm wrt to filtering.
			rc_world_node = CRectangle<>
			(
				rc_world_node.tX0()     - f_texel_world_width,
				rc_world_node.tY0()     - f_texel_world_height,
				rc_world_node.tWidth()  + f_texel_world_width  * 2,
				rc_world_node.tHeight() + f_texel_world_height * 2
			);

			// Do the moving shadows.
			ShadowLighting(pras_region, rc_world_node, CTextureNode::d3ShadowingLight_, CPriv::iShadowLightVal, true, false);
		}

		// Do clut conversion in affected areas.
		for (it_this = lrc_this_frame.begin(); it_this != lrc_this_frame.end(); ++it_this)
			ClutConvert(pras_region, &(*it_this));
	}


	//******************************************************************************************
	void CTextureNode::CPriv::ClutConvert(rptr<CRaster> pras_region, const CRectangle<int>* prc_rect)
	{
		CCycleTimer ctmr;

		//
		// Convert the composite map to screen format, utilising the texture's clut.
		//
		pras_region->Lock();

		if (prc_rect)
		{
			SRect rec(prc_rect->tX0(), prc_rect->tY0(), prc_rect->tWidth(), prc_rect->tHeight());
			ApplyClut(pras_region, *ptexTexture->ppcePalClut->pclutClut, 0, lbAlphaTerrain.u2GetAlphaMask(), &rec);
		}
		else
			ApplyClut(pras_region, *ptexTexture->ppcePalClut->pclutClut, 0, lbAlphaTerrain.u2GetAlphaMask());

		pras_region->Unlock();


		if (!d3dDriver.bUseRegionUploads())
		{
			// Mark the entire texture page as modified so it will get uploaded.
			CTextureNode::ptexmTexturePages->SetModified(rhAlloc.iGetPage(), true);
		}
		else
		{
			if (!prc_rect)
 				// Upload the entire texture.
				CTextureNode::ptexmTexturePages->bUploadRegion(rhAlloc);
			else
			{
				// Upload rectangle area.
				CTextureNode::ptexmTexturePages->prasGetPage(rhAlloc.iGetPage())->Unlock();
				CTextureNode::ptexmTexturePages->prasGetPage(rhAlloc.iGetPage())->bUpload
				(
					rhAlloc.v2GetOrigin().tX + prc_rect->tX0(),
					rhAlloc.v2GetOrigin().tY + prc_rect->tY0(),
					prc_rect->tWidth(),
					prc_rect->tHeight()
				);
			}
		}

		proTex.psClut.Add(ctmr(), pras_region->iWidth * pras_region->iHeight);
	}


	//******************************************************************************************
	void CTextureNode::CPriv::MergeInList(std::list< CRectangle<int> >& lrc_merged, CRectangle<int> rc_new)
	{
		for (;;)
		{
			std::list< CRectangle<int> >::iterator it_merge;
			for (it_merge = lrc_merged.begin(); it_merge != lrc_merged.end(); ++it_merge)
				if (rc_new.bIntersects(*it_merge))
					break;

			if (it_merge != lrc_merged.end())
			{
				rc_new |= *it_merge;
				lrc_merged.erase(it_merge);
			}
			else
				break;
		}

		lrc_merged.push_back(rc_new);
	}


	//******************************************************************************************
	void CTextureNode::CPriv::CopyRaster(rptr<CRaster> pras_dest, rptr<CRaster> pras_src, const CRectangle<int>* prc_rect)
	{
		Assert(pras_dest->pSurface != pras_src->pSurface);
		Assert(pras_dest->iWidth == pras_src->iWidth && pras_dest->iHeight == pras_src->iHeight);

		CCycleTimer ctmr;

		int i_base_x = prc_rect ? prc_rect->tX0()     : 0;
		int i_base_y = prc_rect ? prc_rect->tY0()     : 0;
		int i_width  = prc_rect ? prc_rect->tWidth()  : pras_dest->iWidth;
		int i_height = prc_rect ? prc_rect->tHeight() : pras_dest->iHeight;

		Assert(i_base_x >= 0 && i_base_y >= 0);
		Assert((i_base_x + i_width) <= pras_dest->iWidth && (i_base_y + i_height) <= pras_dest->iHeight);
		Assert((i_base_x & 1) == 0 && (i_width & 1) == 0);

		pras_dest->Lock();
		 pras_src->Lock();

		for (int i_y = 0; i_y < i_height; i_y++)
		{
			void* pv_dest = pras_dest->pAddress(i_base_x, i_base_y + i_y);
			void* pv_src  =  pras_src->pAddress(i_base_x, i_base_y + i_y);

			MemCopy32(pv_dest, pv_src, i_width * pras_dest->iPixelBytes());
		}

		pras_dest->Unlock();
		 pras_src->Unlock();

		proTex.psCopy.Add(ctmr(), i_width * i_height);
	}


	//******************************************************************************************
	void CTextureNode::CPriv::UpdateCluts()
	{
		// Do nothing if no terrain texture objects' clut exists.
		if ((const CClut*)CTerrainObj::pclutTerrain == 0)
		{
			Assert(CPriv::pclutCurrent == 0 && CPriv::pclutDirect3D == 0);
			return;
		}

		CPriv::pclutSoftware = CTerrainObj::pclutTerrain;

		// If Direct3D is being used, build a clut for it if one doesn't already exist.
		if (d3dDriver.bUseD3D())
		{
			//
			// Build a Direct3D clut.
			//

			// Get rid of the old one, if there is one.
			if (CPriv::pclutDirect3D)
			{
				if (CPriv::ppcePalClut->pclutClut == CPriv::pclutDirect3D)
				{
					CPriv::ppcePalClut->pclutClut = 0;
				}
				delete CPriv::pclutDirect3D;
				CPriv::pclutDirect3D = 0;
			}
			if (CPriv::ppcePalClut)
			{
				delete CPriv::ppcePalClut;
				CPriv::ppcePalClut = 0;
			}

			// Get the pixel format for the D3D terrain texture.
			DDPIXELFORMAT ddpf = d3dDriver.ddpfGetPixelFormat(ed3dtexSCREEN_OPAQUE);

			// Get the pixel format for the current renderer.
			CPixelFormat pxf((int)ddpf.dwRGBBitCount, ddpf.dwRBitMask, ddpf.dwGBitMask, ddpf.dwBBitMask);

			// Copy the existing clut.
			// Use the default clut values with a few overrides.
			CClu clu(*CTerrainObj::pclutTerrain);
			clu.iNumDepthValues = 1;	// Number of depth values.

			CPriv::pclutDirect3D = new CClut(clu, CTerrainObj::pclutTerrain->ppalPalette, pxf);

			// Create a palette/clut object.
//DOES THIS LEAK MEMORY AT APPLICATION EXIT?
			// Yes!
			CPriv::ppcePalClut = new CPalClut
			(
				(CPal*)CPriv::pclutDirect3D->ppalPalette,
				       CPriv::pclutDirect3D->pmatMaterial
			);

			CPriv::ppcePalClut->pclutClut = CPriv::pclutDirect3D;
		}

		if (!d3dDriver.bUseD3D())
			CPriv::pclutCurrent = CPriv::pclutSoftware;
		else
			CPriv::pclutCurrent = CPriv::pclutDirect3D;


		Assert(CTextureNode::ptexmTexturePages->iGetNumPages() != 0);

		for (int i_page = 0; i_page < CTextureNode::ptexmTexturePages->iGetNumPages(); i_page++)
		{
			// Get raster page.
			rptr<CRaster> pras = CTextureNode::ptexmTexturePages->prasGetPage(i_page);

			// Currently, terrain works only with 16-bit screens!
			Assert(pras->iPixelBits == 16);

			// Attach the terrain palette to this.  Even though this is a screen-format raster,
			// the palette is used for special rendering.
			pras->AttachPalette(const_cast<CPal*>(CPriv::pclutCurrent->ppalPalette));
		}
	}

	//******************************************************************************************
	void CTextureNode::CPriv::InitBaseTexture()
	{
		//
		// Set up the base terrain texture.
		//
		if (CPriv::prasBaseTexture == 0 || CPriv::rhBaseTexture || CPriv::pclutCurrent == 0)
			return;

		// Allocate the base terrain texture;
		CTextureNode::ptexmTexturePages->AllocateRegion
		(
			CPriv::rhBaseTexture,
			CVector2<int>(CPriv::prasBaseTexture->iWidth, CPriv::prasBaseTexture->iHeight),
			true
		);

		if (!CPriv::rhBaseTexture)
			return;

		// Construct a temporary raster for the region in the page.
		CVector2<int> v2_origin = CPriv::rhBaseTexture.v2GetOrigin();
		CVector2<int> v2_size   = CPriv::rhBaseTexture.v2GetSize();

		rptr<CRaster> pras_region = rptr_cast(CRaster, rptr_new CRasterMem
		(
			CTextureNode::ptexmTexturePages->prasGetPage(CPriv::rhBaseTexture.iGetPage()),
			SRect(v2_origin.tX, v2_origin.tY, v2_size.tX, v2_size.tY)
		));

		pras_region->Lock();

		// Construct a CPArray2 out of the raster.
		CPArray2<uint16> pa2u2
		(
			(uint16*)pras_region->pSurface, pras_region->iHeight, pras_region->iWidth, pras_region->iLinePixels
		);

		int i_lv = int(CPriv::pclutCurrent->cvFromReflect(1.0)) << 8;

		for (int i_y = 0; i_y < pa2u2.iHeight; i_y++)
		{
			for (int i_x = 0; i_x < pa2u2.iWidth; i_x++)
			{
				pa2u2[i_y][i_x] = CPriv::prasBaseTexture->pixGet(i_x, i_y) | i_lv;
			}
		}

		ApplyClut(pras_region, CPriv::pclutCurrent, 0, lbAlphaTerrain.u2GetAlphaMask());

		pras_region->Unlock();
	}
};
