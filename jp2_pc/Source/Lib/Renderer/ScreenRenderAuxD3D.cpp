/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ScreenRenderAuxD3D.cpp                                   $
 * 
 * 99    10/09/98 1:37a Pkeet
 * Caches must have three or more vertices to render.
 * 
 * 98    10/08/98 3:19a Pkeet
 * Fixed bug that caused D3D textures to be loaded multiple times.
 * 
 * 97    10/06/98 11:28p Pkeet
 * Set texture scheduler to 7 ms.
 * 
 * 96    10/05/98 5:46a Pkeet
 * Tweaked z buffer.
 * 
 * 95    10/02/98 4:05p Pkeet
 * Made failure to allocate a D3D raster for a mip level more robust.
 * 
 * 94    9/30/98 3:04p Pkeet
 * Hack for fog and water on the ATI Rage Pro.
 * 
 * 93    9/29/98 4:19p Pkeet
 * Added paranoia code for TLVertex buffers.
 * 
 * 92    9/24/98 1:46a Pkeet
 * Changed the blood specular colour hack to a correct test.
 * 
 * 91    9/24/98 12:30a Pkeet
 * Batch optimized fluid particles. Batched optimized fill particles.
 * 
 * 90    9/22/98 10:40p Pkeet
 * Forced inline critical Direct3D functions.
 * 
 * 89    9/15/98 4:19p Pkeet
 * Reduced problem with objects flashing white after loading a new level.
 * 
 * 88    9/05/98 8:35p Pkeet
 * Used constant flags.
 * 
 * 87    9/05/98 7:53p Pkeet
 * Added support for the Matrox G100.
 * 
 * 86    9/01/98 1:05a Pkeet
 * The mode can now only be changed if the system is active.
 * 
 * 85    9/01/98 12:48a Pkeet
 * Replaced the local screen raster target with the global screen raster target. Uploads of
 * regular textures no longer block.
 * 
 * 84    8/31/98 4:53p Pkeet
 * Implemented the 'FlushBatch' member function.
 * 
 * 83    8/31/98 2:58p Pkeet
 * Added the 'FlushBatch' call.
 * 
 * 82    8/21/98 2:37p Pkeet
 * Screen pointer always nulled when endscene is called.
 * 
 * 81    8/19/98 1:32p Rwyatt
 * VC6.0 Warning fixes
 * 
 * 80    8/19/98 12:37a Pkeet
 * Fixed bug that caused a crash when changing screen resolutions.
 * 
 * 79    8/17/98 4:52p Pkeet
 * Added a specialized hardware primitive for fluid particles that sets them to use alpha and
 * specular.
 * 
 * 78    8/16/98 3:09p Pkeet
 * Activated non-textured fills in 'SetD3DFlagForPolygons.'
 * 
 * 77    8/14/98 1:28p Pkeet
 * Added the 'RenderDummy' member function.
 * 
 * 76    8/10/98 11:29p Pkeet
 * Added the base offsets. Removed the clumsy file hack adjust value.
 * 
 * 75    8/10/98 8:46p Pkeet
 * Made alpha with transparency dependent on the state of the 'bTransparentAlpha' flag.
 * 
 * 74    8/07/98 11:47a Pkeet
 * Added flags for tracking whether the hardware is busy or not. Added asserts to ensure that
 * hardware is not used while the lock is active.
 * 
 * 73    8/06/98 7:36p Pkeet
 * Enabled the use of software water in hardware.
 * 
 * 72    8/06/98 5:10p Pkeet
 * Alpha is now reported on a different flag.
 * 
 * 71    8/05/98 3:14p Pkeet
 * Added the 'fZBiasPerPrimitive' data member.
 * 
 * 70    8/04/98 8:55p Pkeet
 * Fixed compile bug.
 * 
 * 69    8/04/98 6:16p Pkeet
 * Made setting variables at the beginning of 'bBeginScene' occur even if the member function
 * fails. The main screen is now locked and unlocked if textures are to be uploaded.
 * 
 * 68    8/03/98 4:26p Pkeet
 * All polygons receive a Z bias for sorting with prerasterized polygons.
 * 
 * 67    8/02/98 4:51p Pkeet
 * Changed flags.
 * 
 * 66    8/02/98 3:41p Pkeet
 * TLVertices now use a 'w' already in the vertex structure.
 * 
 * 65    8/02/98 3:12p Pkeet
 * Moved several functions to the utilities header.
 * 
 * 64    8/01/98 11:09p Pkeet
 * Moved the z buffer clear to occur just after the flip.
 * 
 * 63    8/01/98 4:47p Pkeet
 * Set a background colour for transparencies. Set transparencies to use alpha and the alpha
 * compare function with filtering.
 * 
 * 62    7/31/98 3:11p Pkeet
 * Changed the 'fInvScale' constant again.
 * 
 * 61    7/31/98 2:34p Pkeet
 * Increased the near range accuracy of the Z buffer to fix bug that allowed the camera to look
 * through objects.
 * 
 * 60    7/31/98 12:27p Pkeet
 * Fixed problem of the flickering line at the top of the screen.
 * 
 * 59    7/29/98 8:25p Pkeet
 * Added code for Z buffering.
 * 
 * 58    7/29/98 5:00p Pkeet
 * Fixed bug that prevented hardware and software screen extents from matching.
 * 
 * 57    7/29/98 4:53p Pkeet
 * Fixed major cracking bug between polygons in the same mesh.
 * 
 * 56    7/29/98 1:34p Pkeet
 * Hardware polygons are now sorted by largest screen area first when attempting uploads.
 * 
 * 55    7/29/98 11:46a Pkeet
 * The device changes addressing mode to wrap or clamp as required.
 * 
 * 54    7/27/98 8:44p Pkeet
 * Added code for scheduling uploads.
 * 
 * 53    7/26/98 7:32p Pkeet
 * Removed unnecessary primitives. Added primitives for lighting non-textured polygons. Added
 * 'const' as appropriate.
 * 
 * 52    7/26/98 6:08p Pkeet
 * Deferred uploads are now used.
 * 
 * 51    7/25/98 3:21p Pkeet
 * Added support for flat-shaded polygons.
 * 
 * 50    7/24/98 5:09p Pkeet
 * Added functions to correctly get lighting values for Gouraud shaded polygons.
 * 
 * 49    7/24/98 3:07p Pkeet
 * Moved fogging here from the pipeline. Fixed crash bug.
 * 
 * 48    7/23/98 10:28p Pkeet
 * Added the specular state.
 * 
 * 47    7/23/98 6:23p Pkeet
 * Added more hardware polygon types.
 *
 **********************************************************************************************/


//
// Includes.
//
#include <algorithm>
#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/View/RasterD3D.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "ScreenRenderAuxD3D.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Fog.hpp"
#include "Lib/View/Direct3DRenderState.hpp"
#include "Lib/Renderer/Sky.hpp"
#include "Lib/View/AGPTextureMemManager.hpp"
#include "Lib/View/RasterConvertD3D.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "ScreenRenderAuxD3DBatch.hpp"
#include "ScreenRenderAuxD3DUtilities.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Renderer/Particles.hpp"

#if bDEBUG_DIRECT3D
	#include "Lib/Sys/DebugConsole.hpp"
#endif // bDEBUG_DIRECT3D


//
// Constants.
//

// White Direct3D colour constants.
D3DCOLOR d3drgbPlainA = D3DRGBA(1, 1, 1, 1);


//
// Macros.
//

// Macros for using a switch..case statement with polygon types.
#define TERRAIN_SET  (1 << erfSOURCE_TERRAIN)
#define WATER_SET    (1 << erfSOURCE_WATER)
#define ALPHACOL_SET (1 << erfALPHA_COLOUR)
#define FILL_SET	 (1 << erfTEXTURE)
#define CACHE_SET    (1 << erfD3D_CACHE)


//
// Module specific variables.
//

// Switch for debugging non-textured polygons.
bool bNoTextures = false;


//
// Class definitions.
//

//******************************************************************************************
//
class CPolyArea
//
// Less than comparison class for CRenderPolygon pointers.
//
//**************************************
{
public:

	//**************************************************************************************
	//
	bool operator()(const CRenderPolygon* prpoly_0, const CRenderPolygon* prpoly_1) const
	//
	// Returns 'true' if the first polygon is larger than the second polygon.
	//
	//**************************************
	{
		Assert(prpoly_0->fArea >= 0.0f);
		Assert(prpoly_1->fArea >= 0.0f);

		// Return the results of the comparision.
		return u4FromFloat(prpoly_0->fArea) > u4FromFloat(prpoly_1->fArea);
	}
};

//**********************************************************************************************
//
class CScreenRenderAuxD3D::CPriv : public CScreenRenderAuxD3D
//
// Private member functions for class 'CScreenRenderAuxD3D.'
//
//**************************************
{
public:
	
	//******************************************************************************************
	//
	void SetAlternateTexture
	(
		CRenderPolygon& rp	// Polygon to draw.
	) const
	//
	// Sets the current texture to be an alternate.
	//
	//**************************************
	{
		Assert(rp.ptexTexture);
		Assert(rp.ptexTexture->prasGetTexture(rp.iMipLevel));
		Assert(rp.ptexTexture->prasGetTexture(rp.iMipLevel)->prasLink.ptGet());

		d3dstState.SetTexture(rp.ptexTexture->prasGetTexture(rp.iMipLevel)->prasLink.ptGet());
	}

	//******************************************************************************************
	//
	forceinline CClut* pclutGet
	(
		CRenderPolygon& rp	// Polygon to extract clut from.
	) const
	//
	// Returns a pointer to the texture's clut for the polygon.
	//
	//**************************************
	{
		AlwaysAssert(rp.ptexTexture->ppcePalClut);
		AlwaysAssert(rp.ptexTexture->ppcePalClut->pclutClut);

		return rp.ptexTexture->ppcePalClut->pclutClut;
	}

	//******************************************************************************************
	//
	forceinline D3DCOLOR d3dcolGetFogVal
	(
		float f_intensity	// Fog intensity.
	) const
	//
	// Returns the fog intensity level between 0 and 1.
	//
	//**************************************
	{
		// If no fog is being used, return the specular constant.
		if (!d3dstState.bFogState())
		{
			return d3drgbPlainA;
		}

		/*
		if (f_intensity < 0.0f)
			f_intensity = 0.0f;
		else
			f_intensity /= 8.0f;
		if (f_intensity > 1.0f)
			f_intensity = 1.0f;
		float f_fog = 1.0f - f_intensity;
		*/
		f_intensity = f_intensity - 0.5f;
		if (f_intensity < 0.0f)
			f_intensity = 0.0f;

		else
			f_intensity /= 6.5f;
		if (f_intensity > 1.0f)
			f_intensity = 1.0f;
		float f_fog = 1.0f - f_intensity;
		return D3DRGBA(0, 0, 0, f_fog);


	}


	//
	// Non-textured D3D polygons.
	//

	//******************************************************************************************
	//
	bool bDrawPolygonFluid
	(
		const CRenderPolygon& rp	// Polygon to draw.
	) const;
	//
	// Renders a polygon with alpha and no texture, the alpha is modulated to give a 'tail.'
	//
	//**************************************

	//******************************************************************************************
	//
	bool bDrawPolygonFoggedNoTexture
	(
		const CRenderPolygon& rp,			// Polygon to draw.
		bool                  b_clip = true	// Flag to clip the polygon if required.
	) const;
	//
	// Renders a polygon with fog but without applying any modulating colour.
	//
	// Notes:
	//		The option to turn clipping off for this polygon is for use by alpha primitives.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bDrawPolygonFlatFoggedNoTexture
	(
		CRenderPolygon& rp	// Polygon to draw.
	) const;
	//
	// Renders a flat shaded and fogged polygon using Direct3D.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bDrawPolygonGouraudFoggedNoTexture
	(
		CRenderPolygon& rp	// Polygon to draw.
	) const;
	//
	// Renders a gouraud shaded and fogged polygon using Direct3D.
	//
	//**************************************

	//
	// Textured D3D polygons.
	//

	//******************************************************************************************
	//
	bool bDrawPolygonFogged
	(
		const CRenderPolygon& rp,			// Polygon to draw.
		bool                  b_clip = true	// Flag to clip the polygon if required.
	) const;
	//
	// Renders a polygon with fog but without applying any modulating colour.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bDrawPolygonWater
	(
		const CRenderPolygon& rp,			// Polygon to draw.
		bool                  b_clip = true	// Flag to clip the polygon if required.
	) const;
	//
	// Renders a polygon with fog but without applying any modulating colour.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bDrawPolygonFlatFogged
	(
		CRenderPolygon& rp	// Polygon to draw.
	) const;
	//
	// Renders a flat shaded and fogged polygon using Direct3D.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bDrawPolygonGouraudFogged
	(
		CRenderPolygon& rp	// Polygon to draw.
	) const;
	//
	// Renders a gouraud shaded and fogged polygon using Direct3D.
	//
	//**************************************

	//
	// Render cache primitives.
	//

	//******************************************************************************************
	//
	bool bDrawPolygonCacheFog
	(
		const CRenderPolygon& rp	// Polygon to draw.
	) const;
	//
	// Renders a render cache polygon using Direct3D and Direct3D fog.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bDrawPolygonCache
	(
		const CRenderPolygon& rp	// Polygon to draw.
	) const;
	//
	// Renders a pre-fogged render cache polygon using Direct3D.
	//
	//**************************************


	//******************************************************************************************
	//
	forceinline void SetTexture
	(
		CRenderPolygon& rp	// Polygon to draw.
	)
	//
	// Sets the current texture.
	//
	//**************************************
	{
		Assert(rp.ptexTexture);
		Assert(rp.ptexTexture->prasGetTexture(0).ptGet());

		d3dstState.SetTexture(rp.ptexTexture->prasGetTexture(0).ptGet());
	}

	//******************************************************************************************
	//
	forceinline ED3DTextureType ed3dtSetPolygonFlags
	(
		CRenderPolygon& rp
	) const
	//
	// Selects type of D3D texture.
	//
	//**************************************
	{
		if (rp.ehwHardwareFlags == ehwFluid)
		{
			return ed3dtexSCREEN_ALPHA;
		}
		if (rp.seterfFace[erfALPHA_COLOUR])
		{
			rp.ehwHardwareFlags = ehwRegularAlpha;
			return ed3dtexSCREEN_ALPHA;
		}
		if (rp.seterfFace[erfTRANSPARENT])
		{
			rp.ehwHardwareFlags = ehwRegularTransparent;
			return ed3dtexSCREEN_TRANSPARENT;
		}
		rp.ehwHardwareFlags = ehwRegularOpaque;
		return ed3dtexSCREEN_OPAQUE;
	};

	//******************************************************************************************
	//
	forceinline void UploadPolygon
	(
		CRenderPolygon& rp
	) const
	//
	// Uploads textures to the card as required.
	//
	//**************************************
	{
		// Select type of D3D texture.
		ED3DTextureType ed3dtex = ed3dtSetPolygonFlags(rp);

		rptr<CRaster> pras = rp.ptexTexture->prasGetTexture(rp.iMipLevel);

		// Test if a link was already established.
		if (pras->prasLink)
		{
			// If there is a valid D3D texture available, there is no need to load another one.
			if (pras->prasLink->pd3dtexGet())
				return;

			// Something is wrong with this texture, clear the link.
			pras->Link();
		}

		rptr<CRaster> prasd3d = agpAGPTextureMemManager.prasCreate
		(
			pras->iWidth,
			pras->iHeight,
			ed3dtex
		);

		// Copy data to the new raster and upload.
		if (prasd3d)
		{
			ConvertRaster(prasd3d->prasd3dGet(), pras, CColour(rp.ptexTexture->d3dpixColour, 0));
			if (!agpAGPTextureMemManager.bAdd(prasd3d))
			{
				// Make sure the link is cleared.
				pras->Link();
				return;
			}
			pras->Link(prasd3d);
		}
		else
		{
			// Make sure the link is cleared.
			pras->Link();
		}
	}

	//******************************************************************************************
	//
	void UploadPolygonArray
	(
		CPArray<CRenderPolygon*> aprp	// Array of polygons to upload.
	);
	//
	// Uploads textures to the card as required.
	//
	//**************************************

	//******************************************************************************************
	//
	void GetBestMipLevels
	(
		CPArray<CRenderPolygon*> aprp,	// Array of polygons to upload.
		int                      i		// Iterator position in the array.
	) const;
	//
	// Selects the best mip levels for textures that do not have time to be uploaded.
	//
	//**************************************

};


//
// Class implementations.
//

//**********************************************************************************************
//
// Implementation of CScreenRenderAuxD3D
//

	//******************************************************************************************
	//
	// Constructor.
	//

	//******************************************************************************************
	CScreenRenderAuxD3D::CScreenRenderAuxD3D()
		: ed3drMode(ed3drSOFTWARE_LOCK), bActive(false), bBusy(false)
	{
		SetScheduler();
	}

	//******************************************************************************************
	CScreenRenderAuxD3D::~CScreenRenderAuxD3D()
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//
	
	//******************************************************************************************
	void CScreenRenderAuxD3D::Reset()
	{
		AlwaysAssert(ed3drMode == ed3drSOFTWARE_LOCK);

		ed3drMode  = ed3drSOFTWARE_LOCK;
		bActive    = false;
		bBusy      = false;
	}

	//******************************************************************************************
	bool CScreenRenderAuxD3D::bBeginScene()
	{
		fOffsetX = fOffsetXBase;
		fOffsetY = fOffsetYBase;

		// Set the active flag.
		bActive = bOutputDisplay && d3dDriver.bUseD3D();

		// If the output is not the output video display raster, do nothing.
		if (!bActive)
			return false;
		AlwaysAssert(!bIsBusy());

		// Set viewport offsets.
		fOffsetX += float(prasMainScreen->iOffsetX);
		fOffsetY += float(prasMainScreen->iOffsetY);

		// Set the base fog colour.
		{
			// Get the RGB values for fog.
			float f_red   = float(clrDefEndDepth.u1Red)   / 255.0f;
			float f_green = float(clrDefEndDepth.u1Green) / 255.0f;
			float f_blue  = float(clrDefEndDepth.u1Blue)  / 255.0f;

			d3dcolFog = D3DRGBA(f_red, f_green, f_blue, 0.0f);
		}

		// Always start with a software lock.
		Assert(ed3drMode == ed3drSOFTWARE_LOCK);
		if (prasMainScreen)
			prasMainScreen->Unlock();
		ed3drMode = ed3drSOFTWARE_LOCK;

		return true;
	}

	//******************************************************************************************
	void CScreenRenderAuxD3D::EndScene()
	{
		// If the output is not the output video display raster, do nothing.
		if (!bActive)
			return;

		// Unlock hardware or software.
		switch (ed3drMode)
		{
			case ed3drSOFTWARE_LOCK:
				if (prasMainScreen)
					prasMainScreen->Unlock();
				break;
			case ed3drHARDWARE_LOCK:
				d3dstState.SetTextureNull();
				d3dDriver.err = d3dDriver.pGetDevice()->EndScene();
				bBusy = false;
				break;
			default:
				Assert(0);
		}
		ed3drMode = ed3drSOFTWARE_LOCK;
		AlwaysAssert(!bBusy);
	}

	//******************************************************************************************
	void CScreenRenderAuxD3D::FlushBatch()
	{
		bool b_hardware = ed3drMode == ed3drHARDWARE_LOCK;

		// Toggle hardware off.
		if (b_hardware)
			SetD3DMode(ed3drSOFTWARE_LOCK);

		// Calling the lock blocks hardware until it has finished rendering.
		prasMainScreen->Lock();
		prasMainScreen->Unlock();

		// Toggle hardware back on.
		if (b_hardware)
			SetD3DMode(ed3drHARDWARE_LOCK);
	}

	//******************************************************************************************
	//
	void CScreenRenderAuxD3D::SetD3DModePriv
	(
		ED3DRenderMode ed3dr
	)
	//
	// Toggles between hardware and software rendering modes.
	//
	//**************************************
	{
		Assert(ed3drMode != ed3dr);
		Assert(d3dDriver.pGetDevice());

		// Do nothing if the system is not active.
		if (!bActive)
			return;

		// Lock and unlock hardware and software.
		switch (ed3dr)
		{
			case ed3drSOFTWARE_LOCK:
				AlwaysAssert(bBusy);
				d3dstState.SetTextureNull();
				Verify(d3dDriver.pGetDevice()->EndScene() == DD_OK);
				bBusy = false;
				break;

			case ed3drHARDWARE_LOCK:
				AlwaysAssert(!bBusy);
				if (prasMainScreen)
					prasMainScreen->Unlock();
				Verify(d3dDriver.pGetDevice()->BeginScene() == DD_OK);
				bBusy = true;

				// Turn Z buffering on for rendering regular polygons with hardware always.
				d3dstState.SetZBuffer(true);
				break;

			default:
				Assert(0);
		}

		// Set the mode variable.
		ed3drMode = ed3dr;
	}

	//******************************************************************************************
	void CScreenRenderAuxD3D::AssertEndScene() const
	{
		if (!d3dDriver.bUseD3D())
			return;
		Assert(ed3drMode != ed3drHARDWARE_LOCK);
	}

	//******************************************************************************************
	//
	bool CScreenRenderAuxD3D::bDrawPolygonD3D
	(
		CRenderPolygon& rp	// Polygon to render.
	)
	//
	// Returns 'true' if the polygon is successfully rendered using Direct3D.
	//
	// Notes: Currently only the following types of polygons are accepted:
	//
	//		Terrain polygons.
	//		Flat shaded polygons.
	//		Alpha coloured polygons.
	//		Alpha water.
	//
	//**************************************
	{
		Assert(rp.ptexTexture);
		Assert(bActive);

		// Start the hardware lock.
		SetD3DMode(ed3drHARDWARE_LOCK);

		// Set the Z bias value.
		fZBiasPerPrimitive = (rp.bPrerasterized) ? (fZBias) : (1.0f);

		//
		// Set up the polygon state and render.
		//
		switch (rp.ehwHardwareFlags)
		{
			// Render regular opaque polgyons.
			case ehwRegularOpaque:
			{
				d3dstState.SetTexture(prasGetBestD3DRaster(rp));
				d3dstState.SetTransparency();
				d3dstState.SetFiltering(true);
				d3dstState.SetAlpha();
				break;
			}

			// Render regular transparent polygons.
			case ehwRegularTransparent:
			{
				d3dstState.SetTexture(prasGetBestD3DRaster(rp));
				d3dstState.SetAlpha(d3dDriver.bUseTransparentAlpha());
				d3dstState.SetTransparency(true);
				d3dstState.SetFiltering(true);
				break;
			}

			// Render regular alpha polygons.
			case ehwRegularAlpha:
			{
				d3dstState.SetTexture(prasGetBestD3DRaster(rp));
				d3dstState.SetTransparency();
				d3dstState.SetFiltering(true);
				d3dstState.SetAlpha(true);
				d3dstState.SetFog(true);
				d3dstState.SetShading();
				d3dstState.SetSpecular();

				return priv_self.bDrawPolygonFogged(rp, false);
			}

			// Terrain rendering.
			case ehwTerrain:
			{
				priv_self.SetTexture(rp);
				d3dstState.SetTransparency();
				d3dstState.SetFiltering(true);
				d3dstState.SetAlpha();

				// Terrain polygons have no shading.
				return priv_self.bDrawPolygonFogged(rp);
			}

			// Set up for image caches.
			case ehwCache:
			{
				priv_self.SetTexture(rp);
				d3dstState.SetAlpha(d3dDriver.bGetAlphaTransparency());
				d3dstState.SetTransparency(true);
				d3dstState.SetFiltering();
				d3dstState.SetShading();
				d3dstState.SetSpecular();

				if (d3dDriver.bUseCacheFog())
				{
					d3dstState.SetFog(true);
					return priv_self.bDrawPolygonCacheFog(rp);
				}
				else
				{
					d3dstState.SetFog(false);
					return priv_self.bDrawPolygonCache(rp);
				}
			}

			// Set up for water.
			case ehwWater:
			{
				priv_self.SetTexture(rp);
				d3dstState.SetTransparency();
				d3dstState.SetFiltering(true);
				d3dstState.SetAlpha(true);
				d3dstState.SetSpecular();

				return priv_self.bDrawPolygonWater(rp, false);
			}

			// Alpha coloured polygons.
			case ehwAlphaCol:
			{
				if (!d3dDriver.bUseAlpha())
					return true;

				d3dstState.SetAlpha(true);
				d3dstState.SetFog(false);
				return priv_self.bDrawPolygonFoggedNoTexture(rp, false);
			}

			// Untextured polygons.
			case ehwFill:
			{
				d3dstState.SetAlpha();
				d3dstState.SetSpecular();
				
				// Non-textured polgyons that may require a variety of options for lighting.
				if (!rp.seterfFace[erfLIGHT] || rp.seterfFace[erfSOURCE_TERRAIN])
				{
					return priv_self.bDrawPolygonFoggedNoTexture(rp);
				}
				else
				{
					if (rp.seterfFace[erfLIGHT_SHADE])
						return priv_self.bDrawPolygonGouraudFoggedNoTexture(rp);
					else
						return priv_self.bDrawPolygonFlatFoggedNoTexture(rp);
				}
				return priv_self.bDrawPolygonFluid(rp);
			}

			// Blood and water drops.
			case ehwFluid:
			{
				return priv_self.bDrawPolygonFluid(rp);
			}

			default:
				Assert(0);
				return false;
		}

		//
		// Regular polgyons that may require a variety of options for lighting.
		//
		if (rp.seterfFace[erfLIGHT])
		{
			if (rp.seterfFace[erfLIGHT_SHADE])
				return priv_self.bDrawPolygonGouraudFogged(rp);
			else
				return priv_self.bDrawPolygonFlatFogged(rp);
		}
		else
			return priv_self.bDrawPolygonFogged(rp);
	}

	//******************************************************************************************
	void CScreenRenderAuxD3D::SetD3DFlagForPolygon(CRenderPolygon& rpoly)
	{
		if (!bActive)
		{
			rpoly.ehwHardwareFlags = ehwSoftware;
			return;
		}

		//
		// If the polygon has already been rasterized, it must already have the appropriate
		// flags and texture.
		//
		if (rpoly.bPrerasterized || rpoly.ehwHardwareFlags != ehwSoftware)
			return;

		if (rpoly.seterfFace[erfSOURCE_TERRAIN])
		{
			if (!rpoly.seterfFace[erfTEXTURE])
			{
				rpoly.ehwHardwareFlags = ehwFill;
				return;
			}
			rpoly.ehwHardwareFlags = ehwTerrain;
			return;
		}

		if (rpoly.seterfFace[erfD3D_CACHE])
		{
			rpoly.ehwHardwareFlags = ehwCache;
			return;
		}

		// Select alpha or non-alpha versions.
		if (d3dDriver.bUseAlpha())
		{
			if (rpoly.seterfFace[erfSOURCE_WATER])
			{
				rpoly.ehwHardwareFlags = ehwWater;
				return;
			}

			if (rpoly.seterfFace[erfALPHA_COLOUR])
			{
				rpoly.ehwHardwareFlags = ehwAlphaCol;
				return;
			}
		}
		rpoly.ehwHardwareFlags = ehwSoftware;
	}

	//******************************************************************************************
	void CScreenRenderAuxD3D::SetD3DFlagForPolygons(CPArray<CRenderPolygon> paprpoly)
	{
		if (!bActive)
		{
			for (uint u_poly = 0; u_poly < paprpoly.uLen; u_poly++)
			{
				paprpoly[u_poly].bFullHardware = false;
				paprpoly[u_poly].ehwHardwareFlags = ehwSoftware;
			}
			return;
		}

		// Select alpha or non-alpha versions.
		if (d3dDriver.bUseAlpha())
		{
			for (uint u_poly = 0; u_poly < paprpoly.uLen; u_poly++)
			{
				CRenderPolygon& rp = paprpoly[u_poly];
				Assert(!(rp.seterfFace[erfDRAW_CLIP] && rp.seterfFace[erfSOURCE_TERRAIN]));

				//
				// If the polygon has already been rasterized, it must already have the appropriate
				// flags and texture.
				//
				if (rp.bPrerasterized || rp.ehwHardwareFlags != ehwSoftware)
					continue;

				if (rp.seterfFace[erfSOURCE_TERRAIN])
				{
					rp.bFullHardware = false;
					if (!rp.seterfFace[erfTEXTURE])
					{
						rp.ehwHardwareFlags = ehwFill;
						continue;
					}
					rp.ehwHardwareFlags = ehwTerrain;
					continue;
				}

				if (rp.seterfFace[erfD3D_CACHE])
				{
					rp.bFullHardware = false;
					rp.ehwHardwareFlags = ehwCache;
					continue;
				}

				if (rp.seterfFace[erfSOURCE_WATER])
				{
					if (d3dDriver.bD3DWater())
					{
						rp.bFullHardware = false;
						rp.ehwHardwareFlags = ehwWater;
						continue;
					}
				}

				if (rp.seterfFace[erfALPHA_COLOUR] && !rp.seterfFace[erfTEXTURE])
				{
					rp.bFullHardware = false;
					rp.ehwHardwareFlags = ehwAlphaCol;
					continue;
				}

				if (!rp.seterfFace[erfTEXTURE])
				{
					rp.ehwHardwareFlags = ehwFill;
					continue;
				}

				rp.ehwHardwareFlags = ehwSoftware;
			}
		}
		else
		{
			for (uint u_poly = 0; u_poly < paprpoly.uLen; u_poly++)
			{
				CRenderPolygon& rp = paprpoly[u_poly];
				Assert(!(rp.seterfFace[erfDRAW_CLIP] && rp.seterfFace[erfSOURCE_TERRAIN]));

				//
				// If the polygon has already been rasterized, it must already have the appropriate
				// flags and texture.
				//
				if (rp.bPrerasterized || rp.ehwHardwareFlags != ehwSoftware)
					continue;

				if (rp.seterfFace[erfSOURCE_TERRAIN])
				{
					rp.bFullHardware = false;
					if (!rp.seterfFace[erfTEXTURE])
					{
						rp.bFullHardware = false;
						continue;
					}
					rp.ehwHardwareFlags = ehwTerrain;
					continue;
				}

				if (rp.seterfFace[erfD3D_CACHE])
				{
					rp.bFullHardware = false;
					rp.ehwHardwareFlags = ehwCache;
					continue;
				}
				
				if (!rp.seterfFace[erfTEXTURE])
				{
					rp.ehwHardwareFlags = ehwFill;
					continue;
				}

				rp.ehwHardwareFlags = ehwSoftware;
			}
		}
	}

	//******************************************************************************************
	void CScreenRenderAuxD3D::LoadHardwareTextures(CPArray<CRenderPolygon> paprpoly,
		                                           bool b_target_hardware)
	{
		b_target_hardware = b_target_hardware && d3dDriver.bUseFullTexturing();

		// Do nothing if full hardware texturing is not supported.
		if (!b_target_hardware)
		{
			for (uint u_poly = 0; u_poly < paprpoly.uLen; u_poly++)
				paprpoly[u_poly].bFullHardware = false;
			return;
		}

		// Create a list of polygons for uploading.
		CMLArray(CRenderPolygon*, arp, paprpoly.uLen);

		for (uint u_poly = 0; u_poly < paprpoly.uLen; u_poly++)
		{
			CRenderPolygon& rp = paprpoly[u_poly];

			//
			// If the polygon has already been rasterized, it must already have the appropriate
			// flags and texture.
			//
			if (rp.bPrerasterized)
				continue;

			// Do nothing if the polygon is of another hardware type.
			if (rp.ehwHardwareFlags != ehwSoftware)
			{
				rp.bFullHardware = false;
				continue;
			}

			// Do nothing if the polygon is to be rejected.
			if (!rp.bAccept || !rp.bFullHardware)
			{
				rp.ehwHardwareFlags = ehwSoftware;
				continue;
			}

			if (rp.bFullHardware && rp.seterfFace[erfTEXTURE])
			{
				// Get the software raster.
				rptr<CRaster> pras = rp.ptexTexture->prasGetTexture(rp.iMipLevel);

				// Confirm the presence of a valid raster.
				if (!pras || bNoTextures)
				{
					rp.seterfFace -= erfTEXTURE;
					rp.ehwHardwareFlags = ehwFill;
					continue;
				}

				// If the polygon has a d3d raster, reference it to make it current.
				if (pras->prasLink)
				{
					priv_self.ed3dtSetPolygonFlags(rp);
					pras->prasLink->pd3dtexGet();
					continue;
				}

				// Record the polygon for future uploading.
				arp << (paprpoly.atArray + u_poly);
			}
		}

		// If there is nothing in the list, do nothing.
		if (!arp.uLen)
			return;

		// Implement the LRU.
		agpAGPTextureMemManager.MakeCurrentList();

		// Sort the polygons by largest area first.
		std::sort(arp.atArray, arp.atArray + arp.uLen, CPolyArea());

		// Upload textures.
		priv_self.UploadPolygonArray(arp);
	}


//**********************************************************************************************
//
//  CScreenRenderAuxD3D::CPriv Implementation.
//

	//******************************************************************************************
	//
	// Non-textured polygons.
	//

	//******************************************************************************************
	bool CScreenRenderAuxD3D::CPriv::bDrawPolygonFluid(const CRenderPolygon& rp) const
	{
		Assert(d3dDriver.pGetDevice());
		Assert(ed3drMode == ed3drHARDWARE_LOCK);
		Assert(rp.ptexTexture);
		Assert(rp.paprvPolyVertices.uLen == 4);
		if (rp.paprvPolyVertices.uLen > 4)
			return true;

		// Necessary states.
		d3dstState.SetAlpha(true);
		d3dstState.SetTextureNull();
		d3dstState.SetTransparency();
		d3dstState.SetSpecular(true);
		d3dstState.SetShading(true);

		// Get the Direct3D colour.
		D3DCOLOR d3dcol      = rp.ptexTexture->d3dpixColour;
		D3DCOLOR d3dcol_spec = d3dcol;

		// Hack for blood.
		if (d3dcol_spec == clrBlood)
		{
			d3dcol_spec = clrBloodSpecular.d3dcolGetD3DColour();
		}
		else if (d3dcol_spec == clrWater)
		{
			d3dcol_spec = clrWaterSpecular.d3dcolGetD3DColour();
		}
		

		// Build a local list of TL vertices.
		D3DTLVERTEX atlv[4];

		// Set the fog level.
		D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(rp.paprvPolyVertices[0]->v3Cam.tY));
		d3dstState.SetFog(d3d_fog != 0xFF000000);

		// Set the first vertex.
		SRenderVertex* prv = rp.paprvPolyVertices[0];
		atlv[0].sz       = fGetScreenZ(prv) * fZBiasPerPrimitive;
		atlv[0].rhw      = prv->v3Screen.tZ;
		atlv[0].color    = (d3dcol & 0x00FFFFFF) | d3dcolFluidAlpha;
		atlv[0].specular = d3d_fog;
		atlv[0].tu       = 0.0f;
		atlv[0].tv       = 0.0f;

		// Set the next three vertices.
		atlv[1] = atlv[0];
		atlv[2] = atlv[0];
		atlv[3] = atlv[0];

		// Make the tail of the particle fully translucent.
		atlv[2].color = d3dcol;

		// Make the head of the particle specular.
		atlv[0].specular = d3d_fog | d3dcol_spec;

		// Add vertices.
		for (int i_vert = 3; i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			prv  = rp.paprvPolyVertices[i_vert];

			// Copy the data to the TLVertex.
			atlv[i_vert].sx = prv->v3Screen.tX + fOffsetX;
			atlv[i_vert].sy = prv->v3Screen.tY + fOffsetY;
		}

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlv,
			4,
			u4DrawPrimFlags
		);
		return true;
	}

	//******************************************************************************************
	bool CScreenRenderAuxD3D::CPriv::bDrawPolygonFoggedNoTexture(const CRenderPolygon& rp, bool b_clip) const
	{
		Assert(d3dDriver.pGetDevice());
		Assert(ed3drMode == ed3drHARDWARE_LOCK);
		Assert(rp.ptexTexture);

		// Necessary states.
		d3dstState.SetTextureNull();
		d3dstState.SetTransparency();
		d3dstState.SetSpecular();

		// Set flag for toggling fogging.
		bool b_fog = false;

		// Get the Direct3D colour.
		D3DCOLOR d3dcol = rp.ptexTexture->d3dpixColour;

		// Build a local list of TL vertices.
		CLArray(D3DTLVERTEX, atlv, rp.paprvPolyVertices.uLen);

		// Add vertices.
		for (int i_vert = int(rp.paprvPolyVertices.uLen - 1); i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = rp.paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlv[i_vert];

			// Set the fog level.
			D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prv->v3Cam.tY));
			//if (d3d_fog != 0xFF000000)
				b_fog = true;

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + fOffsetY;
			ptlv->sz       = fGetScreenZ(prv) * fZBiasPerPrimitive;
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3dcol;
			ptlv->specular = d3d_fog;
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
		}

		// Toggle states as required.
		if (b_fog)
		{
			d3dstState.SetFog(true);
			d3dstState.SetShading(true);
		}
		else
		{
			d3dstState.SetFog();
		}

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlv.atArray,
			rp.paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
		return true;
	}

	//******************************************************************************************
	bool CScreenRenderAuxD3D::CPriv::bDrawPolygonFlatFoggedNoTexture(CRenderPolygon& rp) const
	{
		Assert(d3dDriver.pGetDevice());
		Assert(ed3drMode == ed3drHARDWARE_LOCK);
		Assert(rp.ptexTexture);

		// Necessary states.
		d3dstState.SetTextureNull();
		d3dstState.SetTransparency();

		// Set flag for toggling fogging.
		bool b_fog = false;

		// Get the clut.
		CClut* pclut = pclutGet(rp);

		// Get the Direct3D colour.
		D3DCOLOR d3dcol = rp.ptexTexture->d3dpixColour;

		// Set colour levels.
		SD3DTable d3dt;
		pclut->ConvertToD3D(d3dt, rp.cvFace);

		// Modulate the colour.
		d3dcol = d3dcolModulate(d3dt.u4Colour, d3dcol);

		// Build a local list of TL vertices.
		CLArray(D3DTLVERTEX, atlv, rp.paprvPolyVertices.uLen);

		// Add vertices.
		for (int i_vert = int(rp.paprvPolyVertices.uLen - 1); i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = rp.paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlv[i_vert];

			// Set the fog level.
			D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prv->v3Cam.tY));
			//if (d3d_fog != 0xFF000000)
				b_fog = true;

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + fOffsetY;
			ptlv->sz       = fGetScreenZ(prv) * fZBiasPerPrimitive;
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3dcol;
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
			ptlv->specular = d3dt.u4Specular | d3d_fog;
		}

		// Toggle states as required.
		d3dstState.SetFog(b_fog);
		d3dstState.SetShading(b_fog);
		d3dstState.SetSpecular();

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlv.atArray,
			rp.paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
		return true;
	}

	//******************************************************************************************
	bool CScreenRenderAuxD3D::CPriv::bDrawPolygonGouraudFoggedNoTexture(CRenderPolygon& rp) const
	{
		Assert(d3dDriver.pGetDevice());
		Assert(ed3drMode == ed3drHARDWARE_LOCK);
		Assert(rp.ptexTexture);

		// Necessary states.
		d3dstState.SetTextureNull();
		d3dstState.SetTransparency();

		// Set flag for toggling fogging.
		bool b_fog = false;

		// Get the clut.
		CClut* pclut = pclutGet(rp);

		// Get the Direct3D colour.
		D3DCOLOR d3dcol = rp.ptexTexture->d3dpixColour;

		// Build a local list of TL vertices.
		CLArray(D3DTLVERTEX, atlv, rp.paprvPolyVertices.uLen);

		// Add vertices.
		for (int i_vert = int(rp.paprvPolyVertices.uLen - 1); i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = rp.paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlv[i_vert];

			// Set the fog level.
			D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prv->v3Cam.tY));
			//if (d3d_fog != 0xFF000000)
				b_fog = true;

			// Set colour levels.
			SD3DTable d3dt;
			pclut->ConvertToD3D(d3dt, prv->cvIntensity);

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + fOffsetY;
			ptlv->sz       = fGetScreenZ(prv) * fZBiasPerPrimitive;
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3dcolModulate(d3dt.u4Colour, d3dcol);
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
			ptlv->specular = d3dt.u4Specular | d3d_fog;
		}

		// Toggle states as required.
		d3dstState.SetFog(b_fog);
		d3dstState.SetShading(true);
		d3dstState.SetSpecular();

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlv.atArray,
			rp.paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
		return true;
	}


	//******************************************************************************************
	//
	// Textured polygons.
	//

	//******************************************************************************************
	bool CScreenRenderAuxD3D::CPriv::bDrawPolygonFogged(const CRenderPolygon& rp, bool b_clip) const
	{
		Assert(d3dDriver.pGetDevice());
		Assert(ed3drMode == ed3drHARDWARE_LOCK);

		// Necessary states.
		d3dstState.SetAddressing(rp.eamAddressMode);
		d3dstState.SetSpecular();

		// Set flag for toggling fogging.
		bool b_fog = false;

		// Build a local list of TL vertices.
		CLArray(D3DTLVERTEX, atlv, rp.paprvPolyVertices.uLen);

		// Add vertices.
		for (int i_vert = int(rp.paprvPolyVertices.uLen - 1); i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = rp.paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlv[i_vert];

			// Set the fog level.
			D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prv->v3Cam.tY));
			//if (d3d_fog != 0xFF000000)
				b_fog = true;

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + fOffsetY;
			ptlv->sz       = fGetScreenZ(prv) * fZBiasPerPrimitive;
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3drgbPlain;
			ptlv->specular = d3d_fog;
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
		}

		// Toggle states as required.
		if (b_fog)
		{
			d3dstState.SetFog(true);
			d3dstState.SetShading(true);
		}
		else
		{
			d3dstState.SetFog();
		}

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlv.atArray,
			rp.paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
		return true;
	}

	//******************************************************************************************
	bool CScreenRenderAuxD3D::CPriv::bDrawPolygonFlatFogged(CRenderPolygon& rp) const
	{
		Assert(d3dDriver.pGetDevice());
		Assert(ed3drMode == ed3drHARDWARE_LOCK);

		// Necessary states.
		d3dstState.SetAddressing(rp.eamAddressMode);

		// Set flag for toggling fogging.
		bool b_fog = false;

		// Get the clut.
		CClut* pclut = pclutGet(rp);

		// Set colour levels.
		SD3DTable d3dt;
		pclut->ConvertToD3D(d3dt, rp.cvFace);

		// Build a local list of TL vertices.
		CLArray(D3DTLVERTEX, atlv, rp.paprvPolyVertices.uLen);

		// Add vertices.
		for (int i_vert = int(rp.paprvPolyVertices.uLen - 1); i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = rp.paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlv[i_vert];

			// Set the fog level.
			D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prv->v3Cam.tY));
			//if (d3d_fog != 0xFF000000)
				b_fog = true;

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + fOffsetY;
			ptlv->sz       = fGetScreenZ(prv) * fZBiasPerPrimitive;
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3dt.u4Colour;
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
			ptlv->specular = d3dt.u4Specular | d3d_fog;
		}

		// Toggle states as required.
		d3dstState.SetFog(b_fog);
		d3dstState.SetShading(b_fog);
		d3dstState.SetSpecular();

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlv.atArray,
			rp.paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
		return true;
	}

	//******************************************************************************************
	bool CScreenRenderAuxD3D::CPriv::bDrawPolygonGouraudFogged(CRenderPolygon& rp) const
	{
		Assert(d3dDriver.pGetDevice());
		Assert(ed3drMode == ed3drHARDWARE_LOCK);

		// Necessary states.
		d3dstState.SetAddressing(rp.eamAddressMode);

		// Set flag for toggling fogging.
		bool b_fog = false;

		// Get the clut.
		CClut* pclut = pclutGet(rp);

		// Build a local list of TL vertices.
		CLArray(D3DTLVERTEX, atlv, rp.paprvPolyVertices.uLen);

		// Add vertices.
		for (int i_vert = int(rp.paprvPolyVertices.uLen - 1); i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = rp.paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlv[i_vert];

			// Set the fog level.
			D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prv->v3Cam.tY));
			//if (d3d_fog != 0xFF000000)
				b_fog = true;

			// Set colour levels.
			SD3DTable d3dt;
			pclut->ConvertToD3D(d3dt, prv->cvIntensity);

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + fOffsetY;
			ptlv->sz       = fGetScreenZ(prv) * fZBiasPerPrimitive;
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3dt.u4Colour;
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
			ptlv->specular = d3dt.u4Specular | d3d_fog;
		}

		// Toggle states as required.
		d3dstState.SetFog(b_fog);
		d3dstState.SetShading(true);
		d3dstState.SetSpecular();

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlv.atArray,
			rp.paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
		return true;
	}


	//******************************************************************************************
	//
	// Render cache polygons.
	//

	//******************************************************************************************
	bool CScreenRenderAuxD3D::CPriv::bDrawPolygonCache(const CRenderPolygon& rp) const
	{
		Assert(d3dDriver.pGetDevice());
		Assert(ed3drMode == ed3drHARDWARE_LOCK);

		// Necessary states.
		d3dstState.SetAddressing(rp.eamAddressMode);

		// Skip this polygon if it doesn't have enough vertices.
		if (rp.paprvPolyVertices.uLen < 3)
			return true;

		// Build a local list of TL vertices.
		CLArray(D3DTLVERTEX, atlv, rp.paprvPolyVertices.uLen);

		// Add vertices.
		for (int i_vert = int(rp.paprvPolyVertices.uLen - 1); i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = rp.paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlv[i_vert];

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + fOffsetY;
			ptlv->sz       = fGetScreenZ(prv) * fZBiasPerPrimitive;
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3drgbPlain;
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
			ptlv->specular = d3dcolGetFogVal(prv->cvIntensity);
		}

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlv.atArray,
			rp.paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
		return true;
	}

	//******************************************************************************************
	bool CScreenRenderAuxD3D::CPriv::bDrawPolygonCacheFog(const CRenderPolygon& rp) const
	{
		Assert(d3dDriver.pGetDevice());
		Assert(ed3drMode == ed3drHARDWARE_LOCK);

		// Necessary states.
		d3dstState.SetAddressing(rp.eamAddressMode);

		// Build a local list of TL vertices.
		CLArray(D3DTLVERTEX, atlv, rp.paprvPolyVertices.uLen);

		// Set fog as required.
		D3DCOLOR specular = d3dcolGetFogVal
		(
			(d3dstState.bFogState()) ? (fogTerrainFog.fGetFogLevel(rp.fGetFarthestZ())) : (0.0f)
		);

		// Add vertices.
		for (int i_vert = int(rp.paprvPolyVertices.uLen - 1); i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = rp.paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlv[i_vert];

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + fOffsetY;
			ptlv->sz       = fGetScreenZ(prv) * fZBiasPerPrimitive;
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3drgbPlain;
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
			ptlv->specular = specular;
		}

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlv.atArray,
			rp.paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
		return true;
	}

	//******************************************************************************************
	bool CScreenRenderAuxD3D::CPriv::bDrawPolygonWater(const CRenderPolygon& rp, bool b_clip) const
	{
		Assert(d3dDriver.pGetDevice());
		Assert(ed3drMode == ed3drHARDWARE_LOCK);

		// Necessary states.
		d3dstState.SetAddressing(rp.eamAddressMode);
		d3dstState.SetSpecular();

		// Set flag for toggling fogging.
		bool b_fog = d3dDriver.evcGetVideoCard() != evcATIRage;

		// Build a local list of TL vertices.
		CLArray(D3DTLVERTEX, atlv, rp.paprvPolyVertices.uLen);

		// Add vertices.
		for (int i_vert = int(rp.paprvPolyVertices.uLen - 1); i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = rp.paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlv[i_vert];

			// Set the fog level.
			D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prv->v3Cam.tY));

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + fOffsetY;
			ptlv->sz       = fGetScreenZ(prv) * fZBiasPerPrimitive;
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3drgbPlain;
			ptlv->specular = d3d_fog;
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
		}

		// Toggle states as required.
		if (b_fog)
		{
			d3dstState.SetFog(true);
			d3dstState.SetShading(true);
		}
		else
		{
			d3dstState.SetFog();
		}

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlv.atArray,
			rp.paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
		return true;
	}

	//******************************************************************************************
	void CScreenRenderAuxD3D::CPriv::UploadPolygonArray(CPArray<CRenderPolygon*> aprp)
	{
		// Do nothing if no polygons are present.
		if (!aprp.uLen)
			return;

		// Function without the scheduler.
		if (iDisabledCount > 0)
		{
			// Count down to re-enable scheduling.
			--iDisabledCount;

			// Upload everything.
			for (int i = 0; i < aprp.uLen; ++i)
				UploadPolygon(*aprp[i]);
			return;
		}

		// Start the scheduler.
		uint32 u4_time_ms = GetTickCount();
		for (int i = 0;;)
		{
			// Upload polygon.
			UploadPolygon(*aprp[i]);
			++i;

			// Break if out of time.
			if (i >= aprp.uLen || GetTickCount() - u4_time_ms > 7)
			{
				GetBestMipLevels(aprp, i);
				return;
			}
		}
	}

	//******************************************************************************************
	void CScreenRenderAuxD3D::CPriv::GetBestMipLevels(CPArray<CRenderPolygon*> aprp, int i) const
	{
		for (; i < aprp.uLen; ++i)
		{
			CRenderPolygon& rp = *aprp[i];
			rp.iMipLevel = rp.ptexTexture->iGetBestLinkedMipLevel(rp.iMipLevel);

			// If a mip level is not found, turn this polygon into a straight colour fill.
			// Set the hardware flags so this polygon will be rendered.
			if (rp.iMipLevel < 0)
			{
				rp.seterfFace -= erfTEXTURE;
				rp.ehwHardwareFlags = ehwFill;
			}
			else
			{
				ed3dtSetPolygonFlags(rp);
			}
		}
	}

	//******************************************************************************************
	void CScreenRenderAuxD3D::RenderDummy(CRasterD3D* prasd3d)
	{
		const float fSizeSide = 20.0f;

		if (!prasd3d)
			return;

		D3DTLVERTEX atlv[3];
		D3DTLVERTEX atlv_main;

		d3dDriver.pGetDevice()->BeginScene();
		d3dDriver.pGetDevice()->SetTexture(0, (LPDIRECT3DTEXTURE2)prasd3d->pd3dtexGet());

		d3dstState.SetTransparency();
		d3dstState.SetFiltering();
		d3dstState.SetAlpha();
		d3dstState.SetFog();
		d3dstState.SetShading();
		d3dstState.SetSpecular();
		d3dstState.SetZBuffer();

		// Set up vertices.
		atlv_main.sx       = 1.0f;
		atlv_main.sy       = 1.0f;
		atlv_main.sz       = 0.0;
		atlv_main.rhw      = 1.0 / 0.5;
		atlv_main.color    = d3drgbPlain;
		atlv_main.tu       = 0.0;
		atlv_main.tv       = 0.0;
		atlv_main.specular = d3drgbPlain;

		atlv[0] = atlv_main;
		atlv[1] = atlv_main;
		atlv[2] = atlv_main;

		atlv[1].sx += fSizeSide;
		atlv[1].tu = 1.0f;

		atlv[2].sx = atlv[1].sx;
		atlv[2].sy += fSizeSide;
		atlv[2].tu = 1.0f;
		atlv[2].tv = 1.0f;

		// Draw the rectangle.
		d3dDriver.pGetDevice()->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, (LPVOID)atlv, 3, u4DrawPrimFlags);
		d3dDriver.pGetDevice()->SetTexture(0, 0);
		d3dDriver.pGetDevice()->EndScene();
	}


//
// Global variables.
//
CScreenRenderAuxD3D srd3dRenderer;

float CScreenRenderAuxD3D::fOffsetX     = 0.0f;
float CScreenRenderAuxD3D::fOffsetY     = 0.0f;
float CScreenRenderAuxD3D::fOffsetXBase = 0.0f;
float CScreenRenderAuxD3D::fOffsetYBase = 0.0f;