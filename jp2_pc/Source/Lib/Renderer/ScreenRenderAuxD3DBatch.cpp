/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ScreenRenderAuxD3DBatch.cpp                              $
 * 
 * 27    10/09/98 1:38a Pkeet
 * Polygons must have three or more vertices to render for caches.
 * 
 * 26    10/07/98 7:55a Pkeet
 * Sorting caches by texture.
 * 
 * 25    10/05/98 5:46a Pkeet
 * Tweaked z buffer.
 * 
 * 24    9/29/98 4:19p Pkeet
 * Added paranoia code for TLVertex buffers.
 * 
 * 23    9/24/98 1:46a Pkeet
 * Changed the blood specular colour hack to a correct test.
 * 
 * 22    9/24/98 12:30a Pkeet
 * Batch optimized fluid particles. Batched optimized fill particles.
 * 
 * 21    9/15/98 4:19p Pkeet
 * Reduced problem with objects flashing white after loading a new level.
 * 
 * 20    9/05/98 7:53p Pkeet
 * Added support for the Matrox G100.
 * 
 * 19    9/01/98 12:47a Pkeet
 * Removed unnecessary default state changes. Replaced calls to 'BeginScene' and 'EndScene' with
 * calls to toggle the hardware state on and off.
 * 
 * 18    8/29/98 7:26p Pkeet
 * Implemented cache fog option.
 * 
 * 17    8/27/98 9:15p Asouth
 * loop variable scope
 * 
 * 16    8/23/98 4:21p Pkeet
 * Fixed compiler bug.
 * 
 * 15    8/19/98 1:32p Rwyatt
 * VC6.0 Warning fixes
 * 
 * 14    8/10/98 11:29p Pkeet
 * Added the base offsets. Removed the clumsy file hack adjust value.
 * 
 * 13    8/09/98 10:26p Pkeet
 * Moved batch rasterization stats to the pipeline.
 * 
 * 12    8/07/98 11:48a Pkeet
 * Replaced 'SetD3DModePriv' with 'SetD3DMode.'
 * 
 * 11    8/05/98 3:15p Pkeet
 * Added selective use of the Z bias value.
 * 
 * 10    8/05/98 2:27p Pkeet
 * Added in more optimized batching for opaque texture maps.
 * 
 * 9     8/04/98 6:14p Pkeet
 * Added batch rasterization for caches and for ordinary polygons.
 * 
 * 8     8/03/98 6:12p Pkeet
 * Fixed general sorting bug by setting the area of a prerasterized polygon. Apparently the area
 * is required by the depth sort; this should be investigated further if time permits.
 * 
 * 7     8/03/98 6:07p Pkeet
 * Fixed bug that caused all prerasterized polygons to be accepted when there were no software
 * polygons.
 * 
 * 6     8/03/98 4:25p Pkeet
 * Added the 'RemovePrerasterized' function.
 * 
 * 5     8/03/98 7:47a Kidsbuild
 * Bug Fix for "Final" Build mode
 * 
 * 4     8/02/98 8:38p Pkeet
 * Further optimized the batch processing by eliminating the CLArray used for D3D vertices.
 * 
 * 3     8/02/98 4:50p Pkeet
 * Changed interface to take a list of polygons instead of polygon pointers.
 * 
 * 2     8/02/98 3:41p Pkeet
 * TLVertices now use a 'w' already in the vertex structure.
 * 
 * 1     8/02/98 3:11p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/


//
// Includes.
//
#include <algorithm>
#include "Common.hpp"
#include "ScreenRenderAuxD3DBatch.hpp"

#include "Lib/W95/WinInclude.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/View/RasterD3D.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "ScreenRenderAuxD3D.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/View/Direct3DRenderState.hpp"
#include "Lib/Renderer/Sky.hpp"
#include "Lib/View/AGPTextureMemManager.hpp"
#include "Lib/View/RasterConvertD3D.hpp"
#include "Lib\Std\LocalArray.hpp"
#include "ScreenRenderAuxD3DUtilities.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Renderer/DepthSortTools.hpp"
#include "Lib/Renderer/Particles.hpp"

#if bDEBUG_DIRECT3D
	#include "Lib/Sys/DebugConsole.hpp"
#endif // bDEBUG_DIRECT3D


//
// Module specific constants.
//

// Maximum number of vertices for a Direct3D polygon.
const int iMaxD3DVertices = 128;

// Vertex buffer.
D3DTLVERTEX atlvBuffer[iMaxD3DVertices];


//
// Module specific variables.
//
static CProfileStat psBatchTerrain("Batch Terrain", &proProfile.psRender);
static CProfileStat psRemovePrerendered("Remove Prerendered", &proProfile.psRender);


//
// Internal class definitions.
//

//******************************************************************************************
//
class CSortByBaseTexture
//
// Less than comparison class for CRenderPolygon pointers using their Direct3D interfaces.
//
//**************************************
{
public:

	//**************************************************************************************
	//
	bool operator()(const CRenderPolygon* prpoly_0, const CRenderPolygon* prpoly_1) const
	//
	//
	//
	//**************************************
	{
		// Return the results of the comparision.
		return prpoly_0->ptexTexture->prasGetTexture(0).ptGet() <
			   prpoly_1->ptexTexture->prasGetTexture(0).ptGet();
		
	}

};

//******************************************************************************************
//
class CSortByD3DTexture
//
// Less than comparison class for CRenderPolygon pointers using their Direct3D interfaces.
//
//**************************************
{
public:

	//**************************************************************************************
	//
	bool operator()(const CRenderPolygon* prpoly_0, const CRenderPolygon* prpoly_1) const
	//
	//
	//
	//**************************************
	{
		CRaster* pras_0 = prpoly_0->ptexTexture->prasGetTexture(prpoly_0->iMipLevel)->prasLink.ptGet();
		CRaster* pras_1 = prpoly_1->ptexTexture->prasGetTexture(prpoly_1->iMipLevel)->prasLink.ptGet();

		// Return the results of the comparision.
		return pras_0 < pras_1;
		
	}

};


//
// Module specific function prototypes.
//


//
// Function implementations.
//

//******************************************************************************************
void RasterizeOpaque(CMArray<CRenderPolygon*>& appoly)
{
	// Necessary states.
	d3dstState.SetTransparency();
	d3dstState.SetFiltering(true);
	d3dstState.SetAlpha();
	d3dstState.SetSpecular();

	// Rasterize list.
	for (uint u_poly = 0; u_poly < appoly.uLen; ++u_poly)
	{
		CRenderPolygon* prp = appoly[u_poly];

		// Necessary states.
		d3dstState.SetAddressing(prp->eamAddressMode);
		d3dstState.SetTexture(prasGetBestD3DRaster(*prp));

		// Set flag for toggling fogging.
		bool b_fog = false;

		// Render based on lighting.
		if (prp->seterfFace[erfLIGHT] && prp->seterfFace[erfLIGHT_SHADE])
		{
			//
			// bDrawPolygonGouraudFogged equivalent.
			//

			// Get the clut.
			CClut* pclut = prp->ptexTexture->ppcePalClut->pclutClut;

			// Add vertices.
			int i_last_vert = Min(iMaxD3DVertices, int(prp->paprvPolyVertices.uLen)) - 1;
			for (int i_vert = i_last_vert; i_vert >= 0; --i_vert)
			{
				// Get a pointer to the vertex.
				SRenderVertex* prv  = prp->paprvPolyVertices[i_vert];
				D3DTLVERTEX*   ptlv = &atlvBuffer[i_vert];

				// Set the fog level.
				D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prv->v3Cam.tY));
				//if (d3d_fog != 0xFF000000)
					b_fog = true;

				// Set colour levels.
				SD3DTable d3dt;
				pclut->ConvertToD3D(d3dt, prv->cvIntensity);

				// Copy the data to the TLVertex.
				ptlv->sx       = prv->v3Screen.tX + CScreenRenderAuxD3D::fOffsetX;
				ptlv->sy       = prv->v3Screen.tY + CScreenRenderAuxD3D::fOffsetY;
				ptlv->sz       = fGetScreenZ(prv);
				ptlv->rhw      = prv->v3Screen.tZ;
				ptlv->color    = d3dt.u4Colour;
				ptlv->tu       = prv->tcTex.tX;
				ptlv->tv       = prv->tcTex.tY;
				ptlv->specular = d3dt.u4Specular | d3d_fog;
			}
			d3dstState.SetShading(true);
			d3dstState.SetSpecular();
		}
		else if (prp->seterfFace[erfLIGHT])
		{
			//
			// bDrawPolygonFlatFogged equivalent.
			//

			// Get the clut.
			CClut* pclut = prp->ptexTexture->ppcePalClut->pclutClut;

			// Set colour levels.
			SD3DTable d3dt;
			pclut->ConvertToD3D(d3dt, prp->cvFace);

			// Add vertices.
			int i_last_vert = Min(iMaxD3DVertices, int(prp->paprvPolyVertices.uLen)) - 1;
			for (int i_vert = i_last_vert; i_vert >= 0; --i_vert)
			{
				// Get a pointer to the vertex.
				SRenderVertex* prv  = prp->paprvPolyVertices[i_vert];
				D3DTLVERTEX*   ptlv = &atlvBuffer[i_vert];

				// Set the fog level.
				D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prv->v3Cam.tY));
				//if (d3d_fog != 0xFF000000)
					b_fog = true;

				// Copy the data to the TLVertex.
				ptlv->sx       = prv->v3Screen.tX + CScreenRenderAuxD3D::fOffsetX;
				ptlv->sy       = prv->v3Screen.tY + CScreenRenderAuxD3D::fOffsetY;
				ptlv->sz       = fGetScreenZ(prv);
				ptlv->rhw      = prv->v3Screen.tZ;
				ptlv->color    = d3dt.u4Colour;
				ptlv->tu       = prv->tcTex.tX;
				ptlv->tv       = prv->tcTex.tY;
				ptlv->specular = d3dt.u4Specular | d3d_fog;
			}
			d3dstState.SetSpecular();
		}
		else
		{
			//
			// bDrawPolygonFogged equivalent.
			//

			// Add vertices.
			int i_last_vert = Min(iMaxD3DVertices, int(prp->paprvPolyVertices.uLen)) - 1;
			for (int i_vert = i_last_vert; i_vert >= 0; --i_vert)
			{
				// Get a pointer to the vertex.
				SRenderVertex* prv  = prp->paprvPolyVertices[i_vert];
				D3DTLVERTEX*   ptlv = &atlvBuffer[i_vert];

				// Set the fog level.
				D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prv->v3Cam.tY));
				//if (d3d_fog != 0xFF000000)
					b_fog = true;

				// Copy the data to the TLVertex.
				ptlv->sx       = prv->v3Screen.tX + CScreenRenderAuxD3D::fOffsetX;
				ptlv->sy       = prv->v3Screen.tY + CScreenRenderAuxD3D::fOffsetY;
				ptlv->sz       = fGetScreenZ(prv);
				ptlv->rhw      = prv->v3Screen.tZ;
				ptlv->color    = d3drgbPlain;
				ptlv->specular = d3d_fog;
				ptlv->tu       = prv->tcTex.tX;
				ptlv->tv       = prv->tcTex.tY;
			}
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
			(LPVOID)atlvBuffer,
			prp->paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
	}
}

//******************************************************************************************
void RasterizeFill(CMArray<CRenderPolygon*>& appoly)
{
	// Necessary states.
	d3dstState.SetTextureNull();
	d3dstState.SetTransparency();
	d3dstState.SetFiltering();
	d3dstState.SetAlpha();
	d3dstState.SetSpecular();
	d3dstState.SetFog(true);
	d3dstState.SetShading();
	d3dstState.SetSpecular();

	// Rasterize list.
	for (uint u_poly = 0; u_poly < appoly.uLen; ++u_poly)
	{
		CRenderPolygon* prp = appoly[u_poly];

	#if (0)
		// Get the clut.
		Assert(prp->ptexTexture);
		Assert(prp->ptexTexture->ppcePalClut);
		CClut* pclut = prp->ptexTexture->ppcePalClut->pclutClut;
		Assert(pclut);

		// Get the Direct3D colour.
		D3DCOLOR d3dcol = prp->ptexTexture->d3dpixColour;

		// Set colour levels.
		SD3DTable d3dt;
		pclut->ConvertToD3D(d3dt, prp->cvFace);

		// Modulate the colour.
		d3dcol = d3dcolModulate(d3dt.u4Colour, d3dcol);

		// Set the fog level.
		D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prp->paprvPolyVertices[0]->v3Cam.tY));
		d3d_fog |= d3dt.u4Specular;
	#else
		D3DCOLOR d3dcol  = prp->ptexTexture->d3dpixColour;
		D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prp->paprvPolyVertices[0]->v3Cam.tY));
	#endif

		// Add vertices.
		int i_last_vert = Min(iMaxD3DVertices, int(prp->paprvPolyVertices.uLen)) - 1;
		for (int i_vert = i_last_vert; i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = prp->paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlvBuffer[i_vert];

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + CScreenRenderAuxD3D::fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + CScreenRenderAuxD3D::fOffsetY;
			ptlv->sz       = fGetScreenZ(prv);
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3dcol;
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
			ptlv->specular = d3d_fog;
		}

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlvBuffer,
			prp->paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
	}
}

//******************************************************************************************
void RasterizeFluid(CMArray<CRenderPolygon*>& appoly)
{
	// Necessary states.
	d3dstState.SetAlpha(true);
	d3dstState.SetTextureNull();
	d3dstState.SetTransparency();
	d3dstState.SetSpecular(true);
	d3dstState.SetShading(true);

	for (uint u_poly = 0; u_poly < appoly.uLen; ++u_poly)
	{
		CRenderPolygon* prp = appoly[u_poly];

		// Get the Direct3D colour.
		D3DCOLOR d3dcol      = prp->ptexTexture->d3dpixColour;
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

		// Set the fog level.
		D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prp->paprvPolyVertices[0]->v3Cam.tY));
		d3dstState.SetFog(d3d_fog != 0xFF000000);

		// Set the first vertex.
		SRenderVertex* prv = prp->paprvPolyVertices[0];
		atlvBuffer[0].sz       = fGetScreenZ(prv);
		atlvBuffer[0].rhw      = prv->v3Screen.tZ;
		atlvBuffer[0].color    = (d3dcol & 0x00FFFFFF) | d3dcolFluidAlpha;
		atlvBuffer[0].specular = d3d_fog;
		atlvBuffer[0].tu       = 0.0f;
		atlvBuffer[0].tv       = 0.0f;

		// Set the next three vertices.
		atlvBuffer[1] = atlvBuffer[0];
		atlvBuffer[2] = atlvBuffer[0];
		atlvBuffer[3] = atlvBuffer[0];

		// Make the tail of the particle fully translucent.
		atlvBuffer[2].color = d3dcol;

		// Make the head of the particle specular.
		atlvBuffer[0].specular = d3d_fog | d3dcol_spec;

		// Add vertices.
		for (int i_vert = 3; i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			prv  = prp->paprvPolyVertices[i_vert];

			// Copy the data to the TLVertex.
			atlvBuffer[i_vert].sx = prv->v3Screen.tX + CScreenRenderAuxD3D::fOffsetX;
			atlvBuffer[i_vert].sy = prv->v3Screen.tY + CScreenRenderAuxD3D::fOffsetY;
		}

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlvBuffer,
			4,
			u4DrawPrimFlags
		);
	}
}

//******************************************************************************************
void RasterizeBatch(CPArray<CRenderPolygon>& parpoly)
{
	uint u_poly;

	// Create a pointer array.
	CMLArray(CRenderPolygon*, appoly_mixed,  parpoly.uLen);
	CMLArray(CRenderPolygon*, appoly_opaque, parpoly.uLen);
	CMLArray(CRenderPolygon*, appoly_fluid,  parpoly.uLen);
	
	// Iterate through the polygons, initialize them and push them onto a pointer array.
	for (u_poly = 0; u_poly < parpoly.uLen; ++u_poly)
	{
		CRenderPolygon* ppoly = &parpoly[u_poly];

		// Push to appropriate list.
		switch (ppoly->ehwHardwareFlags)
		{

			case ehwRegularOpaque:
			{
				ppoly->bFullHardware    = false;
				ppoly->bAccept          = true;
				ppoly->bPrerasterized   = true;
				appoly_opaque << ppoly;
				break;	
			}

			case ehwFill:
			{
				ppoly->bFullHardware    = false;
				ppoly->bAccept          = true;
				ppoly->bPrerasterized   = true;
				appoly_mixed << ppoly;
				break;	
			}

			case ehwFluid:
			{
				ppoly->bFullHardware    = false;
				ppoly->bAccept          = true;
				ppoly->bPrerasterized   = true;
				appoly_fluid << ppoly;
				break;	
			}

			default:;
				// Leave the polygon for the depthsort.

		};
	}

	// Sort all the opaque textures by texture.
	std::sort(appoly_opaque.atArray, appoly_opaque.atArray + appoly_opaque.uLen, CSortByD3DTexture());

	// Initialize for hardware rasterizing.
	srd3dRenderer.SetD3DMode(ed3drHARDWARE_LOCK);

	RasterizeOpaque(appoly_opaque);
	RasterizeFill(appoly_mixed);
	RasterizeFluid(appoly_fluid);

	// Finish hardware renderering.
	srd3dRenderer.SetD3DMode(ed3drSOFTWARE_LOCK);
}

//******************************************************************************************
void RasterizeCacheBatch(CPArray<CRenderPolygon>& parpoly)
{
	uint u_poly;

	// Create a pointer array.
	CMLArray(CRenderPolygon*, appoly, parpoly.uLen);
	
	// Iterate through the polygons, initialize them and push them onto a pointer array.
	for (u_poly = 0; u_poly < parpoly.uLen; ++u_poly)
	{
		CRenderPolygon* ppoly = &parpoly[u_poly];

		// If the polygon is not a cache, skip it.
		if (!ppoly->ptexTexture->seterfFeatures[erfD3D_CACHE])
			continue;

		ppoly->bCalculateScreenExtents(false);
		ppoly->ehwHardwareFlags = ehwCache;
		ppoly->bFullHardware    = false;
		ppoly->iMipLevel        = 0;
		ppoly->bAccept          = true;
		ppoly->bPrerasterized   = true;

		appoly << ppoly;
	}

	if (d3dDriver.bIsPageManaged())
	{
		// Sort the list by Direct3D texture address.
		std::sort(appoly.atArray, appoly.atArray + appoly.uLen, CSortByBaseTexture());
	}

	// Initialize for hardware rasterizing.
	srd3dRenderer.SetD3DMode(ed3drHARDWARE_LOCK);
	d3dstState.SetAddressing(eamTileNone);
	d3dstState.SetAlpha(d3dDriver.bGetAlphaTransparency());
	d3dstState.SetFiltering();
	d3dstState.SetSpecular();
	d3dstState.SetShading();
	d3dstState.SetTransparency(true);
	d3dstState.SetFog(d3dDriver.bUseCacheFog());

	D3DCOLOR d3d_fog = d3drgbPlain;

	// Rasterize list.
	for (u_poly = 0; u_poly < appoly.uLen; ++u_poly)
	{
		CRenderPolygon* prp = appoly[u_poly];

		// Skip this polygon if it doesn't have enough vertices.
		if (prp->paprvPolyVertices.uLen < 3)
			continue;

		// Set the Direct3D interface for the texture.
		d3dstState.SetTexture(prp->ptexTexture->prasGetTexture(0).ptGet());

		// Set fog as required for the entire polygon.
		if (d3dDriver.bUseCacheFog())
		{
			d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prp->fGetFarthestZ()));
		}

		// Add vertices.
		int i_last_vert = Min(iMaxD3DVertices, int(prp->paprvPolyVertices.uLen)) - 1;
		for (int i_vert = i_last_vert; i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = prp->paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlvBuffer[i_vert];

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + CScreenRenderAuxD3D::fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + CScreenRenderAuxD3D::fOffsetY;
			ptlv->sz       = fGetScreenZ(prv);
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3drgbPlain;
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
			ptlv->specular = d3d_fog;
		}

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlvBuffer,
			prp->paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
	}

	// Finish hardware renderering.
	srd3dRenderer.SetD3DMode(ed3drSOFTWARE_LOCK);
}

//******************************************************************************************
void RasterizeTerrainBatch(CPArray<CRenderPolygon>& parpoly)
{
	CCycleTimer ctmr;

	// Create a pointer array.
	CLArray(CRenderPolygon*, appoly, parpoly.uLen);
	
	// Iterate through the polygons, initialize them and push them onto a pointer array.
	uint u_poly;
	for (u_poly = 0; u_poly < parpoly.uLen; ++u_poly)
	{
		CRenderPolygon* ppoly = &parpoly[u_poly];

		ppoly->bCalculateScreenExtents(false);
		ppoly->ehwHardwareFlags = ehwTerrain;
		ppoly->bFullHardware    = false;
		ppoly->iMipLevel        = 0;
		ppoly->bAccept          = true;
		ppoly->bPrerasterized   = true;

		appoly[u_poly] = ppoly;
	}

	// Sort the list by Direct3D texture address.
	std::sort(appoly.atArray, appoly.atArray + appoly.uLen, CSortByBaseTexture());

	// Calculate offset.
	CScreenRenderAuxD3D::fOffsetX = float(prasMainScreen->iOffsetX) + CScreenRenderAuxD3D::fOffsetXBase;
	CScreenRenderAuxD3D::fOffsetY = float(prasMainScreen->iOffsetY) + CScreenRenderAuxD3D::fOffsetYBase;

	// Initialize for hardware rasterizing.
	srd3dRenderer.SetD3DMode(ed3drHARDWARE_LOCK);
	d3dstState.SetTransparency();
	d3dstState.SetFiltering(true);
	d3dstState.SetAlpha();
	d3dstState.SetAddressing(eamTileNone);
	d3dstState.SetSpecular();
	d3dstState.SetFog(true);
	d3dstState.SetShading(true);
	d3dstState.SetZBuffer(true);

	// Turn off texture modulation as an optimization.
	d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECAL);

	// Rasterize list.
	for (u_poly = 0; u_poly < appoly.uLen; ++u_poly)
	{
		CRenderPolygon* prp = appoly[u_poly];

		// Set the Direct3D interface for the texture.
		d3dstState.SetTexture(prp->ptexTexture->prasGetTexture(0).ptGet());

		// Add vertices.
		int i_last_vert = Min(iMaxD3DVertices, int(prp->paprvPolyVertices.uLen)) - 1;
		for (int i_vert = i_last_vert; i_vert >= 0; --i_vert)
		{
			// Get a pointer to the vertex.
			SRenderVertex* prv  = prp->paprvPolyVertices[i_vert];
			D3DTLVERTEX*   ptlv = &atlvBuffer[i_vert];

			// Set the fog level.
			D3DCOLOR d3d_fog = d3dGetFog(fogTerrainFog.fGetFogLevel(prv->v3Cam.tY));

			// Copy the data to the TLVertex.
			ptlv->sx       = prv->v3Screen.tX + CScreenRenderAuxD3D::fOffsetX;
			ptlv->sy       = prv->v3Screen.tY + CScreenRenderAuxD3D::fOffsetY;
			ptlv->sz       = fGetScreenZ(prv);
			ptlv->rhw      = prv->v3Screen.tZ;
			ptlv->color    = d3drgbPlain;
			ptlv->specular = d3d_fog;
			ptlv->tu       = prv->tcTex.tX;
			ptlv->tv       = prv->tcTex.tY;
		}

		// Call DrawPrimitive.
		d3dDriver.err = d3dDriver.pGetDevice()->DrawPrimitive
		(
			D3DPT_TRIANGLEFAN,
			D3DFVF_TLVERTEX,
			(LPVOID)atlvBuffer,
			prp->paprvPolyVertices.uLen,
			u4DrawPrimFlags
		);
	}

	// Turn texture modulation back on.
	d3dDriver.err = d3dDriver.pGetDevice()->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);

	// Finish hardware renderering.
	srd3dRenderer.SetD3DMode(ed3drSOFTWARE_LOCK);

	// Update stats.
	psBatchTerrain.Add(ctmr(), appoly.uLen);
}

//******************************************************************************************
void RemovePrerasterized(CPArray<CRenderPolygon>& parpoly)
{
	CCycleTimer ctmr;
	int         i_poly;
	int         i_poly_soft;
	int         i_poly_pre;
	int         i_removed = 0;

	// Do nothing if no polyons are present.
	if (!parpoly.uLen)
		return;

	CMLArray(CRenderPolygon*, appoly_prerast, parpoly.uLen);
	CMLArray(CRenderPolygon*, appoly_software, parpoly.uLen);

	// Iterate through the polygons, initialize them and push them onto a pointer array.
	for (i_poly = 0; i_poly < parpoly.uLen; ++i_poly)
	{
		CRenderPolygon* ppoly = &parpoly[i_poly];
		if (ppoly->bAccept)
		{
			if (ppoly->bPrerasterized)
			{
				ppoly->bAccept = false;
				appoly_prerast << ppoly;
				++i_removed;
			}
			else if (ppoly->ehwHardwareFlags == ehwSoftware)
				appoly_software << ppoly;
		}
	}

	// Do nothing if no polyons are present.
	if (!appoly_prerast.uLen || !appoly_software.uLen)
	{
		psRemovePrerendered.Add(ctmr(), i_removed);
		return;
	}

	//
	// Now all the polygons that are already rendered and are at the end of the list may be
	// culled. First sort the list with the polygons with the largest screen z values (camera
	// 'y' values) first. Then go through each polygon and copy it to the next list only if it
	// does not overlap a software polygon in 'y.' Stop when the first software polygon is
	// reached and copy the remainder of the list.
	//

	// Sort by maximum z.
	std::sort(appoly_software.atArray, appoly_software.atArray + appoly_software.uLen, CPolyFarZ());

	// Cull as many polygons as possible.
	for (i_poly_pre = 0; i_poly_pre < int(appoly_prerast.uLen); ++i_poly_pre)
	{
		CRenderPolygon* ppoly_pre = appoly_prerast[i_poly_pre];
		
		//for (i_poly_soft = 0; i_poly_soft < int(appoly_software.uLen); ++i_poly_soft)
		for (i_poly_soft = int(appoly_software.uLen) - 1; i_poly_soft >= 0; --i_poly_soft)
		{
			CRenderPolygon* ppoly_soft = appoly_software[i_poly_soft];
			
			// If the polygons overlap, the prerendered polygon must be accepted.
			if (!ppoly_pre->bScreenNoOverlap(ppoly_soft))
			{
				ppoly_pre->bAccept = true;
				ppoly_pre->SetArea();
				--i_removed;
				break;
			}

			//
			// Break from the inner loop if the prerasterized polygon is behind all remaining
			// software polygons.
			//
			if (u4FromFloat(ppoly_pre->fMinZ) >= u4FromFloat(ppoly_soft->fMaxZ))
				break;
		}
	}

	// Update stats.
	psRemovePrerendered.Add(ctmr(), i_removed);
}