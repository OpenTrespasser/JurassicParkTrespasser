/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ScreenRenderDWI.cpp                                      $
 * 
 * 231   10/03/98 4:38a Rwyatt
 * Translate bump primitive is never used.
 * 
 * 230   9/07/98 4:55p Pkeet
 * Fixed bug clearing the background with a viewport smaller than the screen dimensions.
 * 
 * 229   9/01/98 1:05a Pkeet
 * Made 'BeginFrame' safer for hardware.
 * 
 * 228   9/01/98 12:46a Pkeet
 * Removed the hardware switch from 'BeginFrame.'
 * 
 * 227   98.08.31 9:35p Mmouni
 * Added support for directly specified alpha.
 * 
 * 226   8/20/98 4:44p Pkeet
 * Changed the 'bTargetHardware' test.
 * 
 * 225   8/16/98 3:09p Pkeet
 * Added the 'bTargetMainScreen' member function.
 * 
 * 224   8/07/98 11:15a Pkeet
 * Removed the stalling lock.
 * 
 * 223   8/02/98 8:37p Pkeet
 * Added the 'SetHardwareOut' member function.
 * 
 * 222   8/01/98 4:42p Pkeet
 * Added the 'pixSetBackground' member function.
 * 
 * 221   7/24/98 5:07p Pkeet
 * Added a lock for D3D testing.
 * 
 * 220   7/23/98 6:22p Pkeet
 * Added a member function to detect if the destination target is hardware. Added more hardware
 * polygon types.
 * 
 * 219   98.07.17 6:41p Mmouni
 * Changed to support new alpha texture primitive.
 * 
 * 218   6/21/98 8:02p Pkeet
 * Added the 'bTEST_HARDWARE_MATCH' macro and test code.
 * 
 * 217   98.06.18 3:56p Mmouni
 * Added support for stippled alpha.
 * 
 * 216   98.05.23 8:10p Mmouni
 * Made bi-linear filter default to on in K63D build.
 * 
 * 215   98.04.30 5:24p Mmouni
 * Added dispatching for stippled texture primitives.
 * 
 * 214   98.04.08 8:05p Mmouni
 * Added code to use new dithered flat fogged primitive.
 * 
 * 213   4/01/98 5:42p Pkeet
 * Added the 'erfD3D_CACHE' render type.
 * 
 * 212   3/18/98 4:06p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 * 211   3/06/98 2:09p Pkeet
 * Direct3D now sets the texture filtering flag on.
 * 
 * 210   3/04/98 1:29p Pkeet
 * Removed the call to the hardware endscene from the screen render to the pipeline.
 * 
 * 209   98.03.04 12:13p Mmouni
 * Changed performance counters for more consistent results.
 * 
 * 208   98/02/26 15:38 Speter
 * Removed erfALPHA_SHADE and erfALPHA_TEXTURE.
 * 
 * 207   98/02/26 13:51 Speter
 * Added seterfFeatures to CTexture, removing redundant flags.
 * 
 * 206   98/02/10 13:17 Speter
 * Moved CClut::gcfMain to CClu.
 * 
 * 205   1/25/98 3:42p Pkeet
 * Polygon feature reduction code has been moved to ScreenRender.cpp.
 * 
 * 204   1/21/98 3:21p Pkeet
 * Added an assert to ensure clip polygons are not also marked as terrain polygons.
 * 
 * 203   1/16/98 6:40p Pkeet
 * Added an assert that a polygon material cannot both be a bumpmap and a copy type.
 * 
 * 202   1/13/98 7:44p Pkeet
 * The screen raster pointer is now stored instead of being passed by each polygon.
 * 
 * 201   1/13/98 1:51p Pkeet
 * The screen is unlocked when rendering begins.
 * 
 * 200   12/15/97 5:38p Rwyatt
 * Fixed header files
 * 
 * 199   12/15/97 1:51p Pkeet
 * Moved the D3D entry point and set timers to measure D3D performance.
 * 
 * 198   12/11/97 1:35p Pkeet
 * Added in the basic functionality for a mixed mode renderer.
 * 
 * 197   1/23/96 6:27p Pkeet
 * Added the 'USE_REGULAR_TERRAIN_FOG' switch.
 * 
 * 196   97.11.19 4:38p Mmouni
 * Added in primitive counts for new terrain dispatch.
 * 
 **********************************************************************************************/

// Calculate timings and frequency for each primitive type.  (Uncomment following line to enable.)
// #define bPRIM_COUNTS	VER_TIMING_STATS
#define bPRIM_COUNTS	(0)


//
// Includes.
//
#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "D3DTypes.h"
#include "ScreenRenderDWI.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/View/Viewport.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Renderer/LightBlend.hpp"
#include <crtdbg.h>
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"
#include "Lib/Renderer/Sky.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "ScreenRenderAuxD3D.hpp"
#include "Lib/Sys/W95/Render.hpp"


//
// Defines.
//

// Switch for compiling 24 and 32-bit primitives (takes twice as long).
#define b8_BIT	(1)
#define b16_BIT	(1)
#define b24_BIT	(1)
#define b32_BIT	(1)

// Switch for compiling an optional wireframe view.
#define bALLOW_WIREFRAME (1)

// Whether to impose size threshold for texturing.
static bool bCullTexture = false;

// Switch to use regular fogging for terrain.
#define USE_REGULAR_TERRAIN_FOG (0)

// Debug testing for hardware implementation.
#define bTEST_HARDWARE_MATCH (0)

//******************************************************************************************
//
class CScreenRenderDWI : public CScreenRender
//
// The base of an implementation of CScreenRender.  Provides common functions not
// specialised for pixel type.
//
//**************************************
{
public:

	bool bHardwareOut;		// Flag indicates hardware is being use for the output device.

	// Settings derived from pSettings.
	TPixel pixBackground;	// The pixel corresponding to clrBackground.


	//******************************************************************************************
	CScreenRenderDWI(SSettings* pscrenset, rptr<CRaster> pras_screen)
		: CScreenRender(pscrenset, pras_screen), bHardwareOut(false)
	{
		// Construct the render context.
		UpdateSettings();
	}

	//******************************************************************************************
	~CScreenRenderDWI()
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual CSet<ERenderFeature> seterfModify()
	{
		static const CSet<ERenderFeature> seterf_modify = Set
		     (erfLIGHT)
			+ erfLIGHT_SHADE
			+ erfFOG
			+ erfFOG_SHADE
			+ erfSPECULAR
			+ erfCOPY
			+ erfTEXTURE
			+ erfTRANSPARENT
			+ erfBUMP
			+ erfWIRE
			+ erfPERSPECTIVE
			+ erfOCCLUDE
			+ erfTRAPEZOIDS
			+ erfSOURCE_TERRAIN
			+ erfSOURCE_WATER
			+ erfDRAW_CLIP
			+ erfMIPMAP
			+ erfALPHA_COLOUR
			+ erfD3D_CACHE
#if (BILINEAR_FILTER)
			+ erfFILTER
#endif
			;

		CSet<ERenderFeature> seterf = seterf_modify;

		// Force filtering on for Direct3D.
		if (d3dDriver.bUseD3D())
			seterf += erfFILTER;

		return seterf;
	}

	//******************************************************************************************
	virtual CSet<ERenderFeature> seterfDefault()
	{
		static const CSet<ERenderFeature> seterf_default = Set
		     (erfZ_BUFFER)
			+ erfLIGHT
			+ erfLIGHT_SHADE
			+ erfFOG
			+ erfFOG_SHADE
			+ erfSPECULAR
			+ erfCOPY
			+ erfTEXTURE
			+ erfTRANSPARENT
			+ erfBUMP
			+ erfSUBPIXEL
			+ erfPERSPECTIVE
			+ erfTRAPEZOIDS
			+ erfMIPMAP
			+ erfALPHA_COLOUR
			+ erfD3D_CACHE
			+ erfDITHER
#if (BILINEAR_FILTER)
			+ erfFILTER
#endif
		;

		CSet<ERenderFeature> seterf = seterf_default;

		// Force filtering on for Direct3D.
		if (d3dDriver.bUseD3D())
			seterf += erfFILTER;

		return seterf;
	}

	//******************************************************************************************
	virtual void UpdateSettings()
	{
		CorrectRenderState(pSettings->seterfState);
		Assert(prasScreen);
		if (pSettings->bClearBackground)
			pixBackground = prasScreen->pixFromColour(pSettings->clrBackground);
	}

	//******************************************************************************************
	virtual bool bTargetHardware() const
	{
		return prasScreen == prasMainScreen.ptGet() && d3dDriver.bUseD3D();
	}

	//******************************************************************************************
	virtual bool bTargetMainScreen() const
	{
		return prasScreen == prasMainScreen.ptGet();
	}

	//******************************************************************************************
	virtual TPixel pixSetBackground(TPixel pix_background)
	{
		TPixel pix_old = pixBackground;
		pixBackground  = pix_background;
		return pix_old;
	}

	//******************************************************************************************
	virtual void SetHardwareOut(bool b_allow_hardware)
	{
		bHardwareOut = false;

		// If Direct3D is not initialized, hardware cannot be used.
		if (!d3dDriver.bUseD3D())
			return;

		// If hardware is not allow, the hardware flag should not be reset.
		if (!b_allow_hardware)
			return;

		// If this screen is not the main screen it cannot support hardware.
		if (prasScreen != prasMainScreen.ptGet())
			return;

		// No reason not to use hardware.
		bHardwareOut = true;
	}

	//******************************************************************************************
	virtual void BeginFrame()
	{
		Assert(prasScreen);

		// Make sure the rasterizer is in software mode before attempting the lock.
		if (d3dDriver.bUseD3D())
		{
			srd3dRenderer.SetD3DMode(ed3drSOFTWARE_LOCK);
		}

		// Copy global screen information.
		prasScreen->Lock();
		gsGlobals.pvScreen     = prasScreen->pSurface;
		gsGlobals.u4LinePixels = prasScreen->iLinePixels;
		gsGlobals.iBits = (prasScreen->pxf.cposG == 0x03E0) ? (15) : (16);
		prasScreen->Unlock();
	}

	//******************************************************************************************
	virtual void ClearMemSurfaces()
	{
		if (pSettings->bClearBackground)
		{
			if (pSettings->bTestGamma)
			{
				GammaBackground();
			}
			else
			{
				// 
				// If there is a sky and this renderer is set to use it, it can only do so
				// in 16 bit modes.
				//
				if ((gpskyRender!=NULL) && (pSettings->bDrawSky) && (prasScreen->iPixelBits == 16))
				{
					gpskyRender->DrawSkyToHorizon();
				}
				else
				{
					if (prasScreen == prasMainScreen.ptGet())
						prasMainScreen->ClearSubRect(pixBackground);
					else
						prasScreen->Clear(pixBackground);
				}
			}
		}
	}

	//******************************************************************************************
	virtual void EndFrame()
	{
		// Double the lines on the screen.
		if (pSettings->bDoubleVertical)
			DoubleVertical();

		// Unlock the backbuffer.
		prasScreen->Unlock();
	}

	//******************************************************************************************
	virtual void DrawPolygons
	(
		CPArray<CRenderPolygon*> paprpoly
	)
	{
		// Set even scanlines only flag.
		bEvenScanlinesOnly = pSettings->bHalfScanlines;

		CScreenRender::DrawPolygons(paprpoly);
	}

protected:

	//******************************************************************************************
	virtual void DrawPalette()
	{
		if (prasScreen->iPixelBits != 8)
			return;

		const int i_pal_pixel_size_bits = 3;
		const int i_line_size           = (1 << i_pal_pixel_size_bits) * 16;
		uint8*    pu1_screen            = (uint8*)prasScreen->pSurface;
		int       i_x, i_y, i_index;

		//
		// Ensure that there is enough space to draw palette give the 'i_pal_pixel_size.'
		//
		if (i_line_size >= prasScreen->iWidth)
			return;
		if (i_line_size >= prasScreen->iHeight)
			return;

		// Count through palette entries and draw.
		for (i_index = i_y = 0; i_y < i_line_size; i_y++)
		{
			for (i_x = 0; i_x < i_line_size; i_x++)
			{
				pu1_screen[i_x] = (i_x >> i_pal_pixel_size_bits) + i_index;
			}
			pu1_screen += prasScreen->iLinePixels;
			i_index     = 16 * (i_y >> i_pal_pixel_size_bits);
		}
	}

	//******************************************************************************************
	//
	void GammaBackground()
	//
	// Draw a gamma test pattern on the background.
	//
	//**********************************
	{
		float f_grey = pow(0.5f, CClu::gcfMain.fGamma);

		// The left side is solid grey.
		prasScreen->Rect
		(
			SRect(0, 0, prasScreen->iWidth/2, prasScreen->iHeight),
			prasScreen->pixFromColour(CColour(f_grey, f_grey, f_grey))
		);

		// The right side alternates black with white.
		TPixel apix[2] = 
		{ 
			prasScreen->pixFromColour(CColour(0, 0, 0)),
			prasScreen->pixFromColour(CColour(1.0, 1.0, 1.0))
		};

		for (int i_y = 0; i_y < prasScreen->iHeight; i_y++)
		{
			int i_index = prasScreen->iIndex(prasScreen->iWidth/2, i_y);
			for (int i_x = 0; i_x < prasScreen->iWidth/2; i_x++)
				prasScreen->PutPixel(i_index + i_x, apix[(i_x+i_y)%2]);
		}
	}

	//*********************************************************************************************
	//
	void DoubleVertical()
	//
	// Replicates every second scanline.
	//
	//**************************************
	{
		int i_width_bytes = prasScreen->iLineBytes();
		int i_height = prasScreen->iHeight;
		i_height -= (i_height & 1);

		uint8* pu1_screen = (uint8*)prasScreen->pSurface;

		// Duplicate every second scanline.
		for (int i_scanline = 0; i_scanline < i_height; i_scanline += 2)
		{
			void* pv_dest = pu1_screen;
			pu1_screen += i_width_bytes;
			void* pv_source = pu1_screen;
			pu1_screen += i_width_bytes;
			MemCopy32(pv_dest, pv_source, i_width_bytes);
		}
	}

};

//******************************************************************************************
//
template<class TPIX> class CScreenRenderDWIT : public CScreenRenderDWI
//
// A class which implements CScreenRender::DrawPolygon, specialised for pixel type TPIX.
// Invoked by newCScreenRenderDWI.
//
//**************************************
{
public:
	//******************************************************************************************
	CScreenRenderDWIT(SSettings* pscrenset, rptr<CRaster> pras_screen)
		: CScreenRenderDWI(pscrenset, pras_screen)
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual CScreenRender* psrCreateCompatible(rptr<CRaster> pras_screen)
	{
		Assert(pras_screen->iPixelBits == prasScreen->iPixelBits);
		return new CScreenRenderDWIT<TPIX>(pSettings, pras_screen);
	}

	//******************************************************************************************
	virtual void DrawPolygon(CRenderPolygon& rp);
};


//
// Counts for primitives.
//

#if bPRIM_COUNTS

#define iFEATURE_COUNT	(erfFILTER+1)
#define iPRIM_COUNT		(1<<iFEATURE_COUNT)

#define dis	(*this)

//**********************************************************************************************
class CPrimCounts: public CAArray<CProfileStat>
{
protected:
	static char* astrFeatures[];
	static char* astrFeatureAbbrevs[];

	CProfileStat psTotal;
	char astrNames[iPRIM_COUNT][2*iFEATURE_COUNT+3];

public:

	//******************************************************************************************
	CPrimCounts() :
		CAArray<CProfileStat>(iPRIM_COUNT+1),
		psTotal("Primitives", &proProfile.psDrawPolygon)
	{
		// Init all the stats with the correct data.
		for (int i = 0; i < uLen-1; i++)
		{
			// Construct name from features.
			*astrNames[i] = 0;
			for (int i_f = 0; i_f < iFEATURE_COUNT; i_f++)
			{
				if (i & (1<<i_f))
					strcat(astrNames[i], astrFeatureAbbrevs[i_f]);
			}

			new(&dis[i]) CProfileStat(astrNames[i], &psTotal);
		}

		new(&dis[uLen-1]) CProfileStat("Alpha", &psTotal);
	}
};

char* CPrimCounts::astrFeatureAbbrevs[] = 
{
	"Cp",
	"Sh",
	"Tx",
	"Tr",
	"Bm",
	"Dc",
	"Pr",
	"Fi"
};

static CPrimCounts aPrimCounts;

#endif


//**********************************************************************************************
template<class TPIX>
void CScreenRenderDWIT<TPIX>::DrawPolygon(CRenderPolygon& rp)
{
	Assert(rp.bAccept);

//
// A space-saving macro.
//
#define DrawPolygon(GOUR, TRANS, MAP, INDEX, CLU)				\
	CDrawPolygon< CScanline<TPIX, GOUR, TRANS, MAP, INDEX, CLU> >(prasScreen, rp)

	CCycleTimer ctmr;

	proProfile.psDrawPolygon.Add(0, 1);

	//
	// Render using the auxilary renderer if possible.
	//
	if (srd3dRenderer.bDrawPolygon(rp))
	{
		psPixels.Add(ctmr());
		return;
	}

	//
	// Hardware is not being use for this primitive, make sure the render is in software mode.
	//
	srd3dRenderer.SetD3DMode(ed3drSOFTWARE_LOCK);

#if bALLOW_WIREFRAME
	Assert(pSettings);
	if (pSettings->seterfState[erfWIRE])
	{
		DrawPolygon(CGouraudOff, CTransparencyOff, CMapWire, CIndexNone, CColLookupOff);
		return;
	}
#endif // bALLOW_WIREFRAME

	CSet<ERenderFeature> seterf = rp.seterfFace;

	//
	// Are you, or have you ever been, a member of the alpha type.
	//
	if (seterf[erfALPHA_COLOUR])
	{
		if (seterf[erfTEXTURE])
		{
			// Get the lookup table.
			au2AlphaTable = abAlphaTexture.u2ColorToAlpha;

			// Textured alpha.
			if (seterf[erfPERSPECTIVE])
				DrawPolygon(CGouraudOff, CTransparencyOff, CMapTexture<uint16>, CIndexPerspective, CColLookupAlphaTexture);
			else
				DrawPolygon(CGouraudOff, CTransparencyOff, CMapTexture<uint16>, CIndexLinear, CColLookupAlphaTexture);
		}
		else if (pSettings->bSoftwareAlpha)
		{
			// Get the alpha colour.
			u2AlphaColourMask      = lbAlphaConstant.u2GetAlphaMask();
			u2AlphaColourReference = lbAlphaConstant.u2GetAlphaReference(rp.ptexTexture->iAlphaColour);
			au2AlphaTable          = lbAlphaConstant.au2Colour;

			// Draw the alpha polygon.
			CDrawPolygon< TAlphaColour >(prasScreen, rp);
		}
		else
		{
			if (rp.ptexTexture->bDirectAlpha)
				u2AlphaColourReference = rp.ptexTexture->tpSolid;
			else
				u2AlphaColourReference = lbAlphaConstant.u2GetAlphaSolidColour(rp.ptexTexture->iAlphaColour);

			// Draw the stippled polygon.
			CDrawPolygon< TFlatStipple >(prasScreen, rp);
		}

#if bPRIM_COUNTS
		// Count time taken by alpha polygon.
		aPrimCounts[iPRIM_COUNT].Add(ctmr(), 1);
#endif

		return;
	}

	iDefaultFog = rp.iFogBand;

	psDrawPolygonInit.Add(ctmr(), 1);

	//
	// Act on the bumpmapping if the texture is a bumpmap.
	//
	if (rp.ptexTexture->seterfFeatures[erfBUMP] && seterf[erfBUMP][erfTEXTURE])
	{
		if (seterf[erfBUMP])
		{
			// Bump mapping is turned on.
			Assert(rp.ptexTexture->ppcePalClut->pBumpTable);

			rp.ptexTexture->ppcePalClut->pBumpTable->bSetupBump(pSettings->bltPrimary, rp.Bump);

			//
			// bSetupbump will always return false because translated lighting is disabled.
			// 

/*			bool b_translate = rp.ptexTexture->ppcePalClut->pBumpTable->bSetupBump(pSettings->bltPrimary, rp.Bump);
			// (Re)Use erfLIGHT_SHADE to indicate translated bump mapping.
			if (b_translate && seterf[erfTEXTURE])
				seterf[erfLIGHT_SHADE] = 1;*/
		}
		else
		{
			// Draw bump map without bump mapping.
			bool b_translate = rp.ptexTexture->ppcePalClut->pBumpTable->bSetupBump(pSettings->bltPrimary, rp.Bump);

			seterf += erfBUMP;

			// (Re)Use erfLIGHT_SHADE to indicate translated bump mapping.
			if (b_translate && seterf[erfTEXTURE])
				seterf[erfLIGHT_SHADE] = 1;
		}

		psDrawPolygonBump.Add(ctmr(), 1);
	}

	//
	// If the terrain has not become flat shaded, use special primitives.
	//
#if USE_REGULAR_TERRAIN_FOG
	//
	// Hack. Set the fog clut value for the terrain.
	//
	if (seterf[erfSOURCE_TERRAIN])
	{
		/*
		if (seterf[erfCOPY])
		{
			Assert(ppceTerrainClut)
			Assert(ppceTerrainClut->pclutClut);

			if (pSettings->seterfState[erfFOG])
			{
				pu2TerrainTextureFogClut = (uint16*)ppceTerrainClut->pclutClut->pvGetConversionAddress(0, 0, rp.iFogBand);
			}
			else
			{
				pu2TerrainTextureFogClut = (uint16*)ppceTerrainClut->pclutClut->pvGetConversionAddress(0, 0, 0);
			}

			Assert(pu2TerrainTextureFogClut);
		}
		else
			seterf -= erfSOURCE_TERRAIN;
		*/


		// If the terrain has not become flat shaded, use special primitives.
		if (seterf[erfTEXTURE])
		{
			// Use no clut if it is the first fog band.
			if (rp.iFogBand == 0)
			{
				if (seterf[erfPERSPECTIVE])
				{
					DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<uint16>, CIndexPerspective, CColLookupOff);
				}
				else
				{
					DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<uint16>, CIndexLinear, CColLookupOff);
				}

#if bPRIM_COUNTS
				int i_flags = seterf[erfCOPY][erfPERSPECTIVE][erfTEXTURE];

				// Count time taken by terrain polygon.
				aPrimCounts[i_flags].Add(ctmr(), 1);
#endif

				// Terrain is in its own grouping.
				return;
			}
			Assert(pSettings->seterfState[erfFOG]);

			//
			// Use an alpha clut.
			//

			// Get the alpha clut settings.
			pu2TerrainTextureFogClut = (uint16*)CColLookupTerrain::SetAlphaClutPointer(rp.iFogBand);

			if (seterf[erfPERSPECTIVE])
			{
				DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<uint16>, CIndexPerspective, CColLookupTerrain);
			}
			else
			{
				DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<uint16>, CIndexLinear, CColLookupTerrain);
			}

#if bPRIM_COUNTS
			int i_flags = seterf[erfCOPY][erfPERSPECTIVE][erfTEXTURE] | (1 << erfLIGHT_SHADE);

			// Count time taken by terrain polygon.
			aPrimCounts[i_flags].Add(ctmr(), 1);
#endif

			// Terrain is in its own grouping.
			return;
		}
	}

#else // USE_REGULAR_TERRAIN_FOG

	if (seterf[erfSOURCE_TERRAIN])
	{
		if (seterf[erfTEXTURE])
		{
			if (seterf[erfCOPY])
			{
				// Use no clut if it is the first fog band.
				if (rp.iFogBand == 0)
				{
					if (seterf[erfPERSPECTIVE])
					{
	#if (BILINEAR_FILTER)
						if (seterf[erfFILTER])
							DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<uint16>, CIndexPerspectiveFilter, CColLookupOff);
						else
	#endif
							DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<uint16>, CIndexPerspective, CColLookupOff);
					}
					else
					{
	#if (BILINEAR_FILTER)
						if (seterf[erfFILTER])
							DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<uint16>, CIndexLinearFilter, CColLookupOff);
						else
	#endif
							DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<uint16>, CIndexLinear, CColLookupOff);
					}

	#if (bPRIM_COUNTS)
					int i_flags = seterf[erfCOPY][erfPERSPECTIVE][erfTEXTURE][erfFILTER];

					// Count time taken by terrain polygon.
					aPrimCounts[i_flags].Add(ctmr(), 1);
	#endif

					// Terrain is in its own grouping.
					return;
				}
				Assert(pSettings->seterfState[erfFOG]);

				//
				// Use an alpha clut.
				//
				if (seterf[erfPERSPECTIVE])
				{
					DrawPolygon(CGouraudFog, CTransparencyOff, CMapTexture<uint16>, CIndexPerspective, CColLookupOff);
				}
				else
				{
					DrawPolygon(CGouraudFog, CTransparencyOff, CMapTexture<uint16>, CIndexLinear, CColLookupOff);
				}

	#if (bPRIM_COUNTS)
				int i_flags = seterf[erfCOPY][erfPERSPECTIVE][erfTEXTURE] | (1 << erfLIGHT_SHADE);

				// Count time taken by terrain polygon.
				aPrimCounts[i_flags].Add(ctmr(), 1);
	#endif

				// Terrain is in its own grouping.
				return;
			}
			else
			{
				seterf = Set(erfLIGHT_SHADE);
			}
		}
		else
		{
			if (rp.iFogBand != 0)
			{
				// Dither fog the flat shaded poly.
				iDefaultFog = 0;
				DrawPolygon(CGouraudFog, CTransparencyOff, CMapFlat, CIndexNone, CColLookupOff);
				return;
			}
			else
			{
				seterf -= erfSOURCE_TERRAIN;
			}
		}
	}

#endif // USE_REGULAR_TERRAIN_FOG

	// Do water.
	if (seterf[erfSOURCE_WATER])
	{
		if (seterf[erfPERSPECTIVE])
		{
			if (seterf[erfDITHER])
				DrawPolygon(CGouraudOff, CTransparencyStipple, CMapTexture<uint16>, CIndexPerspective, CColLookupOff);
			else
				DrawPolygon(CGouraudOff, CTransparencyOff, CMapTexture<uint16>, CIndexPerspective, CColLookupAlphaWater);
		}
		else
		{
			if (seterf[erfDITHER])
				DrawPolygon(CGouraudOff, CTransparencyStipple, CMapTexture<uint16>, CIndexLinear, CColLookupOff);
			else
				DrawPolygon(CGouraudOff, CTransparencyOff, CMapTexture<uint16>, CIndexLinear, CColLookupAlphaWater);
		}
		return;
	}
	
	// Check for incompatible features.
	Assert(!(seterf[erfCOPY] && seterf[erfBUMP]));
	Assert(!(seterf[erfDRAW_CLIP] && seterf[erfSOURCE_TERRAIN]));

	int i_flags = seterf[erfCOPY][erfTRANSPARENT][erfPERSPECTIVE][erfTEXTURE][erfLIGHT_SHADE][erfBUMP][erfDRAW_CLIP][erfSOURCE_TERRAIN];

	switch (i_flags)
	{
		// Ignore perspective when no texture.
		case 0:
		case (1 << erfDRAW_CLIP):
		case (1 << erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudOff, CTransparencyOff, CMapFlat, CIndexNone, CColLookupOff);
			break;
		}

		case (1<<erfLIGHT_SHADE) :
		case (1<<erfLIGHT_SHADE) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudOn, CTransparencyOff, CMapFlat, CIndexNone, CColLookupOn);
			break;
		}

		case (1<<erfTEXTURE) :
		{
			DrawPolygon(CGouraudOff, CTransparencyOff, CMapTexture<uint8>, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfLIGHT_SHADE) | (1<<erfTEXTURE) :
		{
			DrawPolygon(CGouraudOn, CTransparencyOff, CMapTexture<uint8>, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfTEXTURE) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudOff, CTransparencyOff, CMapTexture<uint8>, CIndexPerspective, CColLookupOn);
			break;
		}

		case (1<<erfLIGHT_SHADE) | (1<<erfTEXTURE) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudOn, CTransparencyOff, CMapTexture<uint8>, CIndexPerspective, CColLookupOn);
			break;
		}

		case (1<<erfTRANSPARENT) :
		case (1<<erfTRANSPARENT) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudOff, CTransparencyOn, CMapFlat, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfLIGHT_SHADE) | (1<<erfTRANSPARENT) :
		case (1<<erfLIGHT_SHADE) | (1<<erfTRANSPARENT) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudOn, CTransparencyOn, CMapFlat, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfTEXTURE) | (1<<erfTRANSPARENT) :
		{
			DrawPolygon(CGouraudOff, CTransparencyOn, CMapTexture<uint8>, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfLIGHT_SHADE) | (1<<erfTEXTURE) | (1<<erfTRANSPARENT) :
		{
			DrawPolygon(CGouraudOn, CTransparencyOn, CMapTexture<uint8>, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfTEXTURE) | (1<<erfPERSPECTIVE) | (1<<erfTRANSPARENT) :
		{
			DrawPolygon(CGouraudOff, CTransparencyOn, CMapTexture<uint8>, CIndexPerspective, CColLookupOn);
			break;
		}

		case (1<<erfLIGHT_SHADE) | (1<<erfTEXTURE) | (1<<erfPERSPECTIVE) | (1<<erfTRANSPARENT) :
		{
			DrawPolygon(CGouraudOn, CTransparencyOn, CMapTexture<uint8>, CIndexPerspective, CColLookupOn);
			break;
		}


		// Primary non-translated bump-mapping.
		// Allow turning off texture for debugging purposes.

		case (1<<erfBUMP) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapBumpNoTex, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfBUMP) | (1<<erfTRANSPARENT) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOn, CMapBumpNoTex, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfBUMP) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapBumpNoTex, CIndexPerspective, CColLookupOn);
			break;
		}

		case (1<<erfBUMP) | (1<<erfTRANSPARENT) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOn, CMapBumpNoTex, CIndexPerspective, CColLookupOn);
			break;
		}

		case (1<<erfBUMP) | (1<<erfTEXTURE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapBump, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfBUMP) | (1<<erfTEXTURE) | (1<<erfTRANSPARENT) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOn, CMapBump, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfBUMP) | (1<<erfTEXTURE) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapBump, CIndexPerspective, CColLookupOn);
			break;
		}

		case (1<<erfBUMP) | (1<<erfTEXTURE) | (1<<erfTRANSPARENT) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOn, CMapBump, CIndexPerspective, CColLookupOn);
			break;
		}

		/*
		// Regular texturing from bump map.
		case (1<<erfBUMPFLAT) | (1<<erfTEXTURE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapBump, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfBUMPFLAT) | (1<<erfTEXTURE) | (1<<erfTRANSPARENT) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOn, CMapBump, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfBUMPFLAT) | (1<<erfTEXTURE) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapBump, CIndexPerspective, CColLookupOn);
			break;
		}

		case (1<<erfBUMPFLAT) | (1<<erfTEXTURE) | (1<<erfTRANSPARENT) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOn, CMapBump, CIndexPerspective, CColLookupOn);
			break;
		}
		*/

		// Bump mapping with extra translation lookup.
		case (1<<erfBUMP) | (1<<erfLIGHT_SHADE) | (1<<erfTEXTURE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapBumpTable, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfBUMP) | (1<<erfLIGHT_SHADE) | (1<<erfTEXTURE) | (1<<erfTRANSPARENT) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOn, CMapBumpTable, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfBUMP) | (1<<erfLIGHT_SHADE) | (1<<erfTEXTURE) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapBumpTable, CIndexPerspective, CColLookupOn);
			break;
		}

		case (1<<erfBUMP) | (1<<erfLIGHT_SHADE) | (1<<erfTEXTURE) | (1<<erfTRANSPARENT) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOn, CMapBumpTable, CIndexPerspective, CColLookupOn);
			break;
		}

		case (1<<erfCOPY) | (1<<erfTEXTURE) | (1<<erfTRANSPARENT) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOn, CMapTexture<TPIX>, CIndexPerspective, CColLookupOff);
			break;
		}

		case (1<<erfCOPY) | (1<<erfTEXTURE) | (1<<erfTRANSPARENT) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOn, CMapTexture<TPIX>, CIndexLinear, CColLookupOff);
			break;
		}

		case (1<<erfCOPY) | (1<<erfTEXTURE) | (1<<erfPERSPECTIVE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<TPIX>, CIndexPerspective, CColLookupOff);
			break;
		}

		case (1<<erfCOPY) | (1<<erfTEXTURE) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<TPIX>, CIndexLinear, CColLookupOff);
			break;
		}

		case (1<<erfCOPY) | (1<<erfTEXTURE) | (1<<erfPERSPECTIVE) | (1<<erfSOURCE_TERRAIN) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<TPIX>, CIndexPerspective, CColLookupOn);
			break;
		}

		case (1<<erfCOPY) | (1<<erfTEXTURE) | (1<<erfSOURCE_TERRAIN) :
		{
			DrawPolygon(CGouraudNone, CTransparencyOff, CMapTexture<TPIX>, CIndexLinear, CColLookupOn);
			break;
		}

		case (1<<erfCOPY) :
		{
			DrawPolygon(CGouraudOff, CTransparencyOff, CMapFlat, CIndexNone, CColLookupOff);
			break;
		}

		default:
			// Please leave this assert in.  Its purpose is to detect polygons that don't render, 
			// and helps us debug such situations.
			Assert(false);
			break;
	}

#if bPRIM_COUNTS
	// Ignore perspective when no texture.
	if (i_flags & (1 << erfPERSPECTIVE) && !(i_flags & (1 << erfTEXTURE)))
		i_flags &= ~(1 << erfPERSPECTIVE);

	// Ignore source_terrain flag.
	i_flags &= ~(1 << erfSOURCE_TERRAIN);
	
	aPrimCounts[i_flags].Add(ctmr(), 1);
#endif
}

//**********************************************************************************************
//
// CRenderDescDWI implementation.
//

	//**********************************************************************************************
	CScreenRender* CRenderDescDWI::pScreenRenderCreate(CScreenRender::SSettings* pscrenset, 
		rptr<CRaster> pras_screen)
	{
		// Create a new CScreenRenderT depending on raster type.
		switch (pras_screen->iPixelBits)
		{
		#if b8_BIT
			case 8:
				return new CScreenRenderDWIT<uint8>(pscrenset, pras_screen);
		#endif
		#if b16_BIT
			case 16:
				return new CScreenRenderDWIT<uint16>(pscrenset, pras_screen);
		#endif
		#if b24_BIT
			case 24:
				return new CScreenRenderDWIT<uint24>(pscrenset, pras_screen);
		#endif
		#if b32_BIT
			case 32:
				return new CScreenRenderDWIT<uint32>(pscrenset, pras_screen);
		#endif
			default:
				Assert(false);
				return 0;
		}
	}


// Global instance of this render descriptor.
CRenderDescDWI screndescDWI;
