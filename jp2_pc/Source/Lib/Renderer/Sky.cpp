/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Implementation of CSkyRender
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *		The bitmap for the texture must contain less than iNUM_SKY_COLOURS otherwise it will
 *		assert in the constructor. This define can be set to any number of colours but care
 *		should taken when chaning this value because the fog CLUT for the sky also changes
 *		size. At the moment we are set to 64 fog bands and 128 colours which creates a CLUT
 *		with 8192 entries at 2 bytes each - thus consuming 16384 bytes of memory. A full clut
 *		of 256 colours with 256 fog bands will consume 128K.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Sky.cpp                                                  $
 * 
 * 61    98.10.03 11:09p Mmouni
 * Put self moding code in code segment "SelfMod".
 * 
 * 60    9/26/98 9:26p Pkeet
 * Sky matches fill.
 * 
 * 59    9/23/98 12:39p Pkeet
 * Turned off Z buffering for the sky.
 * 
 * 58    9/22/98 10:40p Pkeet
 * Changed the overloaded '*' operator for CColour to use floats instead of doubles.
 * 
 * 57    9/22/98 12:37a Pkeet
 * Hardware states handled explicitly.
 * 
 * 56    9/07/98 4:55p Pkeet
 * Fixed bug clearing the background with a viewport smaller than the screen dimensions.
 * 
 * 55    9/04/98 7:38p Agrant
 * disable the long symbol warning
 * 
 * 54    9/02/98 10:52p Mmouni
 * Fixed a problem with the K6-3D scanline fill filling an extra 8 pixles if the starting
 * address of the scanline was not 8 byte aligned.
 * 
 * 53    9/02/98 5:48p Asouth
 * added some specific 'float ptr' qualifiers in inline assembly
 * 
 * 52    9/01/98 12:49a Pkeet
 * Sky no longer sets the default render state.
 * 
 * 51    8/31/98 2:59p Pkeet
 * Changed the Direct3D rendering to use more typical Direct3D code.
 * 
 * 50    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 49    8/10/98 11:29p Pkeet
 * Added the base offsets. Removed the clumsy file hack adjust value.
 * 
 * 48    8/04/98 6:13p Pkeet
 * Made sky work with the Z buffer on a Voodoo 2.
 * 
 * 47    8/02/98 9:05p Pkeet
 * Disabled filling the non-sky portion of the backdrop using a Direct3D primitive.
 * 
 * 46    7/29/98 8:25p Pkeet
 * Added code for Z buffering.
 * 
 * 45    7/29/98 11:47a Pkeet
 * The addressing mode for sky is made to wrap.
 * 
 * 44    7/27/98 12:29p Pkeet
 * Added an initialization class for 'CRasterD3D.'
 * 
 * 43    7/23/98 9:08p Pkeet
 * Added code to track the creation and deletion of Direct3D textures using a macro switch in
 * 'RasterD3D.hpp.'
 * 
 * 42    7/21/98 5:21p Rwyatt
 * Set sky to use 128 colours.
 * 
 * 41    7/20/98 10:27p Pkeet
 * Upgraded to Direct3D 4's interface.
 * 
 * 40    7/13/98 12:07a Pkeet
 * Added the 'ResetScreenSize' member function. Fixed a crash bug associated with the sky. Added
 * an offset to allow Direct3D to place its polygons in the right place when the size of the
 * viewport is changed.
 * 
 * 39    98/06/29 16:25 Speter
 * Rendering functions now take CInstance*.
 * 
 * 38    98.06.26 7:40p Mmouni
 * Added sold fill mode to sky.
 * 
 * 37    98.06.25 8:46p Mmouni
 * No longer loads the bTextured flag.
 * 
 * 36    6/22/98 12:17p Pkeet
 * Fixed the sky flashing bug under Direct3D for certain configurations.
 * 
 * 35    6/12/98 9:16p Pkeet
 * Sky now uses a Direct3D parameter to optionally support software rendered sky under Direct3D.
 * Sky forces its Direct3D raster to always be its required size.
 * 
 * 34    98.05.26 4:29p Mmouni
 * Fixed sky co-ordinates overflow due to honking big step time.
 * 
 * 33    5/15/98 2:22a Rwyatt
 * Adjusted the UV co-ords for when the sky wraps
 * 
 * 32    5/14/98 8:06p Agrant
 * Removed the defunct poval_renderer argument form  rendertype constructor
 * 
 * 31    5/01/98 5:13p Rwyatt
 * Sky texture offsets now wrap instead of getting bigger  and bigger.
 * 
 * 30    4/22/98 12:36p Rwyatt
 * New text prop to set the flat shade colour
 * 
 * 29    4/21/98 2:55p Rwyatt
 * Sky must have text props which specifiy no mip maps.
 * Old VM system has been removed.
 * 
 * 28    4/03/98 5:26p Pkeet
 * Fixed bug that was causing sky to randomly switch between a filtered state and a non-filtered
 * state.
 * 
 * 27    98.04.03 1:01p Mmouni
 * Added save/load methods to CSky.
 * 
 * 26    98.03.31 6:26p Mmouni
 * K63D optimized sky is now drawn in 64x64 blocks.
 * 
 * 25    3/30/98 12:41p Pkeet
 * Fixed Direct3D bug.
 * 
 * 24    98.03.27 6:18p Mmouni
 * Sky (on Pentium) is now drawn in 32x32 blocks.
 * 
 * 23    3/27/98 5:30p Pkeet
 * Fixed bug where the sky texture is not available under Direct3D.
 * 
 * 22    98/03/27 16:15 Speter
 * Fixed warnings.
 * 
 * 21    98.03.19 5:07p Mmouni
 * Optimized linear textured routine for 3dx.
 * 
 * 20    3/18/98 4:06p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 * 19    98/02/26 13:45 Speter
 * Replaced CPolyIterator::sfSurface() with ptexTexture().
 * 
 * 18    1/29/98 7:43p Rwyatt
 * The Sky constructor no longer scans the bitmap as this causes the map to be loaded in virtual
 * mode.
 * 
 * 17    1/26/98 11:37a Pkeet
 * Set the perspective flag on for Direct3D.
 * 
 * 16    1/22/98 3:17p Pkeet
 * Added a purge function for Direct3D.
 * 
 * 15    98/01/16 12:34 Speter
 * Turned on screen fill by default.
 * 
 * 14    98.01.07 1:05p Mmouni
 * Optimized Pentium version of linear sky drawing.
 * 
 * 13    1/06/98 7:02p Pkeet
 * Always enable fog for sky.
 * 
 * 12    1/06/98 6:26p Pkeet
 * Added fogging for sky.
 * 
 * 11    1/05/98 7:29p Pkeet
 * Fixed bug involving the render state being set after "Begin."
 * 
 * 10    1/05/98 6:27p Pkeet
 * Made the sky render correctly with Direct3D. Added the 'FillD3D' member function.
 * 
 * 9     1/02/98 2:36p Pkeet
 * Added structures and preliminary code to support Direct3D.
 * 
 * 8     12/04/97 4:15p Rwyatt
 * Inserted the protect macros around the sky renderer
 * 
 * 7     97.11.11 10:47p Mmouni
 * General assembly sky drawing optimizations.
 * 
 * 6     10/22/97 5:07p Rwyatt
 * Added perspective textured and flat for non horizontal skies.
 * Optimized math and replaced constant expressions expressions
 * Made fog linear.
 * frustum vectors are now normalised direction vectors instead of the full length of the
 * frustum,
 * Sub division is variable.
 * 
 * 5     10/16/97 2:04p Agrant
 * Value table based constructor now allows instancing of CInfo-
 * not relevant for Sky, but here for uniformity.
 * 
 * 4     10/14/97 7:01p Rwyatt
 * First Check in
 * 
 * 3     10/14/97 4:26a Rwyatt
 * Added sky/clouds move speed in m/s
 * 
 * 2     10/13/97 8:46p Rwyatt
 * Fixed assert in debug mode
 * 
 * 1     10/13/97 1:22p Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "lib/W95/WinInclude.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Sky.hpp"

#include <algorithm>

#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Sys/VirtualMem.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"
#include "Lib/View/Clut.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "SkyPoly.hpp"
#include "AsmSupport.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/View/Direct3DRenderState.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3DUtilities.hpp"

#pragma warning(disable:4786)

#undef VER_ASM
//**********************************************************************************************
// Global sky render class this should ALWAYS be NULL if there is no sky present.
//
CSkyRender*	gpskyRender = NULL;
//
//**************************************


//**********************************************************************************************
// static array initialized by the constructor to contain the normailzed co-ords of the frustum
// at the back clip plane.
//
CVector3<>	CSkyRender::av3Frustum[u4NUM_SKY_TRANSFORM_VERT];


//**********************************************************************************************
// The transformed fustrum as unit vectors
//
CVector3<>	CSkyRender::av3FrustTran[u4NUM_SKY_TRANSFORM_VERT];


//**********************************************************************************************
// The Camera in world space
//
CVector3<>	CSkyRender::v3Camera;



//**********************************************************************************************
// Constructor requires a source sky bitmap and a destination render surface,
//
CSkyRender::CSkyRender(rptr<CRaster> pras_sky, rptr<CRaster> pras_screen)
//
//**************************************
{
	// the width and height of the sky texture must be 256x256
	Assert (pras_sky->iWidth == 256);
	Assert (pras_sky->iHeight == 256);
	// and the texture must be packed...
	Assert (pras_sky->iLinePixels == 512);

	// reference the sky texture so it remians when the mesh is deleted
	prasSkyTexture		= pras_sky;
	prasRenderSurface	= pras_screen;


	// Setup the static array representing the back of the frustum in normalized
	// camera space.
	av3Frustum[u4FRUST_TL]		= CVector3<>( -1.0f,  1.0f,  1.0f);  //tl
	av3Frustum[u4FRUST_TR]		= CVector3<>(  1.0f,  1.0f,  1.0f);  //tr
	av3Frustum[u4FRUST_BL]		= CVector3<>( -1.0f,  1.0f, -1.0f);  //bl
	av3Frustum[u4FRUST_BR]		= CVector3<>(  1.0f,  1.0f, -1.0f);  //br
	av3Frustum[u4FRUST_TOP]		= CVector3<>(  0.0f,  0.0f,  1.0f);  //top
	av3Frustum[u4FRUST_RIGHT]	= CVector3<>(  1.0f,  0.0f,  0.0f);  //right


	// initial sky offsets
	fSkyOffsetU = 0;
	fSkyOffsetV = 0;

	// Default flat shaded colour is the last in the palette
	u4FlatColourIndex = iNUM_SKY_COLOURS - 1;

	//
	// Setup the working default sky paramaters.
	//
	SetWorkingConstants
	(
		fDEFAULT_SKY_PIXEL_SCALE, 
		fDEFAULT_SKY_HEIGHT, 
		fDEFAULT_SKY_NEAR_FOG,
		fDEFAULT_SKY_FAR_FOG,
		u4SCREEN_DIVIDE_PIXELS
	);


	//
	// Set the default wind speed
	//
	SetSkyWind(fDEFAULT_SKY_WIND_U,fDEFAULT_SKY_WIND_V);

	// fog colour = clrDefEndDepth;
	SetSkyFogTable(clrDefEndDepth);

	// sky is textured by default..
	SetDrawMode(sdmTextured);

	// the sky does fill by default
	SetFilled(true);

	// set the frame time so the clould movement can be calculated
	sFrameTime = CMessageStep::sStaticTotal;

	// Determine if Direct3D is to be used, and perform setup for it if it is.
	//InitializeForD3D();
}


//**********************************************************************************************
// This calculates a linear fade from 0 to the back fog colour.
//
void CSkyRender::SetSkyFogTable
//
//**************************************
(
	CColour	clr_fog
)
{
	for (uint32 u4_fog = 0; u4_fog<iNUM_SKY_FOG_BANDS; u4_fog++)
	{
		float	f_fog_weight = (float)u4_fog / (float)(iNUM_SKY_FOG_BANDS-1);

		for (uint32 u4_col = 0; u4_col<iNUM_SKY_COLOURS; u4_col++)
		{
			CColour		clr = prasSkyTexture->clrFromPixel(u4_col);
			CColour		clr_local;

			clr_local = (clr * (1.0f - f_fog_weight)) + (clr_fog * f_fog_weight);
		
			u2FogTable[u4_fog][u4_col] = (uint16)prasRenderSurface->pixFromColour(clr_local);
		}
	}

	// make a DWORD with the fog colour so we can set a scan line fast.
	u4Fog = prasRenderSurface->pixFromColour(clr_fog);
	u4Fog = u4Fog | (u4Fog<<16);
}



//**********************************************************************************************
// All the working constants for the screen are calculated here. Most of them are 1/x functions
// to save the divides.
//
void CSkyRender::SetWorkingConstants
//
//**************************************
(
	float			f_pixels_per_meter,
	float			f_sky_height,
	float			f_fog_near,
	float			f_fog_far,
	uint32			u4_sub_division
)
{
	//
	// Do most of the asserts here, after here not much can go wrong.
	//
	Assert(prasRenderSurface);
	Assert(prasRenderSurface->iWidth);
	Assert(prasRenderSurface->iHeight);
	Assert(f_sky_height>0);
	Assert( (f_fog_near>=0) && (f_fog_near<=1.0) );
	Assert( (f_fog_far>=0) && (f_fog_far<=1.0) );

	// sub division must be an even number....
	Assert((u4_sub_division & 1) == 0);

	u4DivisionLength	= u4_sub_division;
	fDivisionsPerScanRep = 1.0f / ((float)prasRenderSurface->iWidth / (float)u4DivisionLength);
	fPixelsPerMeter		= f_pixels_per_meter;
	fSkyHeight			= f_sky_height;
	fFogFar				= f_fog_far;
	fFogNear			= f_fog_near;
	fFogScale			= (1.0f/(fFogFar-fFogNear)) * (iNUM_SKY_FOG_BANDS-1)*(1<<u4DEFAULT_SKY_FOG_BITS);
	fScale				= f_pixels_per_meter * 65536.0f;
	fScreenWidthRep		= 1.0f / (float)prasRenderSurface->iWidth;
	fScreenHeightRep	= 1.0f / (float)prasRenderSurface->iHeight;
	fScreenWidthScaleRep = (1.0f / (float)prasRenderSurface->iWidth)*fScale;
	fDivisionLengthRepScale	= (1.0f/(float)u4DivisionLength)*fScale;

	u4ScreenWidth		= prasRenderSurface->iWidth;
	u4ScreenHeight		= prasRenderSurface->iHeight;
}

//**********************************************************************************************
void CSkyRender::ResetScreenSize()
{
	Assert(prasRenderSurface);

	NewRenderSurface(prasRenderSurface);
	SetWorkingConstants
	(
		fPixelsPerMeter,
		fSkyHeight,
		fFogNear,
		fFogFar,
		u4DivisionLength
	);
}


//**********************************************************************************************
// This is called when the destination raster changes. This happens when the screen is resized
// or the mode is changed.
//
void CSkyRender::NewRenderSurface(rptr<CRaster> pras_screen)
//
//**************************************
{
	Assert(pras_screen);
	Assert(pras_screen->iWidth);
	Assert(pras_screen->iHeight);

	prasRenderSurface = pras_screen;
	fScreenHeightRep	= 1.0f / (float)prasRenderSurface->iHeight;
	fScreenWidthRep		= 1.0f / (float)prasRenderSurface->iWidth;
	fScreenWidthScaleRep = (1.0f / (float)prasRenderSurface->iWidth)*fScale;
	fDivisionsPerScanRep = 1.0f / ((float)prasRenderSurface->iWidth / (float)u4DivisionLength);

	u4ScreenWidth		= prasRenderSurface->iWidth;
	u4ScreenHeight		= prasRenderSurface->iHeight;

	SetSkyFogTable(clrDefEndDepth);
}

//**********************************************************************************************
// Entry point for drawing the sky.
//
// This will first decide if to use the linear or perspective sky primitve, the it checks to
// see if the sky is to be textured or shaded.
//
void CSkyRender::DrawSkyToHorizon()
//
//**************************************
{
	// Get the vectors representing the orientation of the view frustum.
	SetTransformedCameraCorners();

	// Set the D3D use flag.
	bUseD3D = d3dDriver.bUseD3DSky();

	// Use Direct3D if possible.
	if (bUseD3D)
	{
		// adjust the texture positions to simulate the wind moving the clouds
		TSec s_elapsed = CMessageStep::sStaticTotal - sFrameTime;
		sFrameTime = CMessageStep::sStaticTotal;

		// adjust the sky origin...
		fSkyOffsetU += (fSkyOffsetdU*s_elapsed * 0.000000025f);
		fSkyOffsetV += (fSkyOffsetdV*s_elapsed * 0.000000025f);

		// Keep the U and V offsets in the range 0..1.
		fSkyOffsetU -= int(fSkyOffsetU);
		fSkyOffsetV -= int(fSkyOffsetV);
		DrawD3D();
		return;
	}

	// get a pointer to the rendering surface.
	prasRenderSurface->Lock();
	pu2Raster = (uint16*)prasRenderSurface->pSurface;
	i4Pitch	= prasRenderSurface->iLinePixels;

	if ( fabs(av3FrustTran[0].tZ - av3FrustTran[1].tZ) < fSKY_HORIZONTAL)
	{
		// Draw a Linear Sky....
		if (sdmMode == sdmTextured)
		{
			DrawSkyTexturedLinear();
		}
		else if (sdmMode == sdmGradient)
		{
			DrawSkyFlatLinear();
		}
		else
		{
			// Color fill.
			AlwaysAssert(prasRenderSurface.ptGet() == prasMainScreen.ptGet());
			//prasRenderSurface->Clear(u4Fog & 0x0000ffff);
			prasMainScreen->ClearSubRect(u4Fog & 0x0000ffff);
		}
	}
	else
	{
		// Draw a perspective sky
		if (sdmMode == sdmTextured)
		{
			DrawSkyTexturedPerspective();
		}
		else if (sdmMode == sdmGradient)
		{
			DrawSkyFlatPerspective();
		}
		else
		{
			// Color fill.
			AlwaysAssert(prasRenderSurface.ptGet() == prasMainScreen.ptGet());
			//prasRenderSurface->Clear(u4Fog & 0x0000ffff);
			prasMainScreen->ClearSubRect(u4Fog & 0x0000ffff);
		}
	}

	// Unlock the render surface now we have finished with it
	prasRenderSurface->Unlock();

	{
		// adjust the texture positions to simulate the wind moving the clouds
		TSec s_elapsed = CMessageStep::sStaticTotal - sFrameTime;
		sFrameTime = CMessageStep::sStaticTotal;

		// Clamp the frame time.
		if (s_elapsed > 1.0f)
			s_elapsed = 1.0f;

		// adjust the sky origin...
		fSkyOffsetU += (fSkyOffsetdU * s_elapsed);
		fSkyOffsetV += (fSkyOffsetdV * s_elapsed);

		//
		// Keep the sky co-ords to something sensible, otherwise the floating point offsets become
		// too big and we get streaks.
		//
		if (fSkyOffsetU > 512.0f)
			fSkyOffsetU -= 512.0f;
		if (fSkyOffsetU < -512.0f)
			fSkyOffsetU += 512.0f;

		if (fSkyOffsetV > 512.0f)
			fSkyOffsetV -= 512.0f;
		if (fSkyOffsetV < -512.0f)
			fSkyOffsetV += 512.0f;
	}
}



//**********************************************************************************************
//
//  Protected functions
//

//**********************************************************************************************
//
void CSkyRender::DrawSkyFlatLinear()
//
//**************************************
{
	CVector3<>	v3_delta;							// frustum step vector for left edge of screen
	CVector3<>*	pv3;
	float		f_clip;
	int32		i4_fog;
	uint32		u4_lines = 0;


	if (av3FrustTran[0].tZ<0)
	{
		// we are drawing from bottom to top
		// calculate the deltas..
		v3_delta = (av3FrustTran[0] - av3FrustTran[2]) * fScreenHeightRep;

		// point to the base vector we are using (bottom in this case)
		pv3	= &av3FrustTran[2];

		// adjust the screen pointer to point to the left edge of the last scanline
		pu2Raster = pu2Raster + (i4Pitch*((int32)u4ScreenHeight-1));
		i4Pitch = - i4Pitch;
	}
	else
	{
		// we are drawing from bottom to top
		// calculate the deltas..
		v3_delta = (av3FrustTran[2] - av3FrustTran[0]) * fScreenHeightRep;

		// point to the base vector we are using (bottom in this case)
		pv3	= &av3FrustTran[0];
	}

	// get the parametric clip point for the vector and sky plane, relative to the player.
	// This is the initial clip point used for the first scan line, the next clip point
	// is calculated while the first scan line is being drawn..	
	f_clip = fSkyHeight / pv3->tZ;

	while ( (pv3->tZ>fSKY_HORIZON_Z)  && (u4_lines<u4ScreenHeight))
	{
		i4_fog = (int32) ((1.0f - fabs(pv3->tZ) - fFogNear) * fFogScale)>>u4DEFAULT_SKY_FOG_BITS;

		// clamp the fog bands to the definded range...
		if (i4_fog>iNUM_SKY_FOG_BANDS-1)
		{
			i4_fog = iNUM_SKY_FOG_BANDS-1;
		}
		else if (i4_fog<0)
		{
			i4_fog = 0;
		}

		// calculate the ray cast vectors for the next scan line and the next clip point
		*pv3+=v3_delta;
		f_clip = fSkyHeight / pv3->tZ;

		// fast fill the scan line
		FillSky
		(
			pu2Raster,
			u4ScreenWidth,
			0,
			1, 
			u2FogTable[ i4_fog ][u4FlatColourIndex] | (u2FogTable[ i4_fog ][u4FlatColourIndex]<<16)
		);

		// the next scan line
		pu2Raster = pu2Raster + i4Pitch;
		u4_lines++;
	}


	// if the fill flag is set and the screen was not filled by the sky then fill it with the
	// back fog colour.
	if ((bFill) && (u4_lines<u4ScreenHeight))
	{
		FillSky
		(
			pu2Raster,
			u4ScreenWidth,
			((i4Pitch-(int32)u4ScreenWidth)*prasRenderSurface->iPixelBits/8),
			u4ScreenHeight - u4_lines, 
			u4Fog
		);
	}
}


#if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_PENTIUM)

//**********************************************************************************************
//
struct SaveValues
//
// Values saved between scanlines.
//
//**************************************
{
	int		i_uvint;
	int		i_ufrac;
	int		i_vfrac;
	int		ai_uvdelta[2];
	int		i_ufrac_delta;
	int		i_vfrac_delta;
	void*	pu2_dest;
	void*	pv_fog;
};

#define iRENDER_BLOCK_SIZE 32

#pragma optimize("g", off)
#pragma code_seg("SelfMod")

//**********************************************************************************************
//
void CSkyRender::DrawSkyTexturedLinear()
//
//**************************************
{
	CVector3<>	v3_delta_left;
	CVector3<>	v3_delta_right;
	CVector3<>*	pv3_left;
	CVector3<>*	pv3_right;
	float		f_clip;
	int32		i4_fog;
	uint32		fp_u;
	uint32		fp_v;
	uint32		fp_delta_u;
	uint32		fp_delta_v;
	uint32		u4_lines = 0;
	uint8*		pu1_tex;
	SaveValues	asv_saved[iRENDER_BLOCK_SIZE];

	static void*  pvFogPointer;
	static void*  pvBaseOfLine;
	static int32 i_ufrac_delta;
	static int32 i_vfrac_delta;
	static int32 ai_uvdelta[2];

	typedef CVector3<> CVector3_float;
	typedef CSkyRender myCSkyRender;

	//Assert(iNUM_SKY_COLOURS == 64);

	pu1_tex = (uint8*)prasSkyTexture->pSurface;

	//
	// If the World space Z component of the frustum top left vector is negative then we are
	// drawing from bottom to top of the screen.
	//
	if (av3FrustTran[0].tZ < fSKY_HORIZON_Z)
	{
		// we are drawing from bottom to top
		// calculate the per scan line deltas..
		v3_delta_left	= (av3FrustTran[0] - av3FrustTran[2]) * fScreenHeightRep;
		v3_delta_right	= (av3FrustTran[1] - av3FrustTran[3]) * fScreenHeightRep;

		// point to the base vector we are using (bottom in this case)
		pv3_left	= &av3FrustTran[2];
		pv3_right	= &av3FrustTran[3];

		// adjust the screen pointer to point to the left edge of the last scanline
		pu2Raster = pu2Raster + (i4Pitch*((int)u4ScreenHeight-1));
		i4Pitch = - i4Pitch;
	}
	else
	{
		// we are drawing from top to bottom
		// calculate the per scan line deltas..
		v3_delta_left	= (av3FrustTran[2] - av3FrustTran[0]) * fScreenHeightRep;
		v3_delta_right	= (av3FrustTran[3] - av3FrustTran[1]) * fScreenHeightRep;

		// point to the base vector we are using (bottom in this case)
		pv3_left	= &av3FrustTran[0];
		pv3_right	= &av3FrustTran[1];
	}

	// get the parametric clip point for the vector and sky plane, relative to the origin.
	// This is the initial clip point used for the first scan line, the next clip point
	// is calculated while the first scan line is being drawn..	
	f_clip = fSkyHeight / pv3_left->tZ;

	__asm
	{
		mov		ecx,[pu1_tex]						// Texture pointer.

		lea		eax,MODIFY_FOR_TEXTUREPOINTER_A
		mov		[eax-4],ecx

		lea		eax,MODIFY_FOR_TEXTUREPOINTER_B
		mov		[eax-4],ecx

		lea		eax,MODIFY_FOR_TEXTUREPOINTER_C
		mov		[eax-4],ecx

		lea		eax,MODIFY_FOR_TEXTUREPOINTER_D
		mov		[eax-4],ecx

		lea		eax,MODIFY_FOR_TEXTUREPOINTER_E
		mov		[eax-4],ecx

		lea		eax,MODIFY_FOR_TEXTUREPOINTER_F
		mov		[eax-4],ecx

		lea		eax,MODIFY_FOR_TEXTUREPOINTER_G
		mov		[eax-4],ecx

		lea		eax,MODIFY_FOR_TEXTUREPOINTER_H
		mov		[eax-4],ecx
	}

	int i_line;
	int i_lines_drawn;

	// while not at the horizion and there are scan lines left.
	bool b_done = false;
	while (!b_done)
	{
		SaveValues* psv_saved = &asv_saved[0];

		i_lines_drawn = iRENDER_BLOCK_SIZE;

		// Do first block.
		for (i_line = 0; i_line < iRENDER_BLOCK_SIZE; i_line++)
		{
			// Early out if necessary.
			if (pv3_left->tZ <= fSKY_HORIZON_Z || u4_lines >= u4ScreenHeight)
			{
				b_done = true;
				i_lines_drawn = i_lines_drawn + i_line - iRENDER_BLOCK_SIZE;
				break;
			}

			// calculate the fog band based on the Z component the ray vectors for this scan line,
			// we need to shift down u4DEFAULT_SKY_FOG_BITS because the linear version does not 
			// use the fog fraction.
			i4_fog = (int32)((1.0f - fabs(pv3_left->tZ) - fFogNear) * fFogScale) >> u4DEFAULT_SKY_FOG_BITS;

			//
			// clamp the fog bands to the definded range...
			// If we above or equal to the last fog band then we can set the whole scan line to
			// a flat shaded colour.
			//
			if (i4_fog < 0)
			{
				i4_fog = 0;
			}
			else if (i4_fog >= iNUM_SKY_FOG_BANDS-1)
			{
				// next ray cast vectors...
				*pv3_left += v3_delta_left;
				*pv3_right += v3_delta_right;

				// fire off the divide
				f_clip = fSkyHeight / (pv3_left->tZ);

				// fill 1 scan line
				FillSky(pu2Raster, u4ScreenWidth, 0, 1, u4Fog);

				// Deduct this line from the number to texture.
				i_lines_drawn--;

				// goto the next address
				pu2Raster = pu2Raster + i4Pitch;
				u4_lines++;
				continue;
			}

			// calculate the intersection points with the sky plane, scale it and convert it to
			// 16.16 fixed point.
			fp_u = (uint32)(((v3Camera.tX + (pv3_left->tX * f_clip)) * fScale) + fSkyOffsetU);
			fp_v = (uint32)(((v3Camera.tY + (pv3_left->tY * f_clip)) * fScale) + fSkyOffsetV);

			// scale the f_clip by the screen width scale so the U,V deltas come out from below
			// ready scaled.
			// fScreenWidthScaleRep contains a scale of 65536.0 to adjust for the fixed point
			f_clip *= fScreenWidthScaleRep;

			// vec_right and vec_left contain the intersection points of the frustum vectors for
			// this scan line and the sky plane (in world space, with constant height).
			// The UVs are calculated as the difference between these positions divided by the
			// number of steps/pixels in the screen width.
			// These deltas are then scaled by the pixel/world size and the fixed point constant
			// 65536.0f
			fp_delta_u = (uint32)(((pv3_right->tX) - (pv3_left->tX)) * f_clip);
			fp_delta_v = (uint32)(((pv3_right->tY) - (pv3_left->tY)) * f_clip);

			// calculate the ray cast vectors for the next scan line
			*pv3_left  += v3_delta_left;
			*pv3_right += v3_delta_right;

			__asm
			{
			mov		ecx,this						// Load object pointer.
			mov		eax,[pv3_left]					// Load vertex pointer.

			fld		[ecx]myCSkyRender.fSkyHeight	// Load sky height. 
			fdiv	[eax]CVector3_float.tZ			// fSkyHeight / (pv3_left->tZ)
			// Overlap division with scanline.

			//
			// Per scanline loop.
			//
			lea		esi,[ecx]myCSkyRender.u2FogTable	// Pointer to fog table.
			mov		eax,[i4_fog]						// Load fog value.

			shl		eax,8								// Shift for width of table.
			mov		ebx,[ecx]myCSkyRender.pu2Raster		// Pointer to destination.

			mov		edi,iRENDER_BLOCK_SIZE				// Load width of scanline.
			add		esi,eax								// Adjust fog table base for i4_fog.

			push	ebp							// Save ebp.
			mov		[pvFogPointer],esi			// Global fog pointer.

			lea		ecx,[ebx+edi*2]				// Start + width of scanline.
			mov		eax,[fp_u]					// Load fixed point U.

			neg		edi							// Negative width.

			mov		[pvBaseOfLine],ecx			// One past end of scanline pointer.
			mov		ebx,[fp_v]					// Load fixed point V.

			//
			// eax = fp_u
			// ebx = fp_v
			// ecx = pvBaseOfLine
			// edi = -width
			//
			sar		eax,16						// integral u
			mov		esi,[fp_delta_u]			// u step

			sar		ebx,(16 - 9)				// integral v
			and		eax,0xff					// mask u for tiling

			and		ebx,(0xff << 9)				// mask v for tiling
			mov		ecx,[fp_delta_v]			// v step

			sar		esi,16						// shift for integral u
			add		eax,ebx						// integer start

			sar		ecx,(16 - 9)				// shift for integral v
			mov		edx,eax						// copy to edx

			and		ecx,(0xffffffff << 9)		// mask fractiontal bits
			mov		eax,[fp_delta_u]			// u step

			add		ecx,esi						// add integer and  frational parts
			mov		ebx,[fp_delta_v]			// v step

			shl		eax,16
			mov		[ai_uvdelta+4],ecx

			shl		ebx,16
			add		ecx,(1 << 9)				// Add one line to step.

			mov		[ai_uvdelta],ecx
			mov		[i_ufrac_delta],eax

			mov		[i_vfrac_delta],ebx
			mov		esi,[fp_u]					// get copy of u

			shl		esi,16						// u fraction
			mov		ecx,[fp_v]					// get copy of v

			shl		ecx,16						// v fraction
			mov		ebp,[pvBaseOfLine]
		
			//
			// eax = 0
			// ecx = vfrac
			// edx = integer offset
			// esi = ufrac
			//
			xor		eax,eax
			lea		ebp,[ebp+edi*2]

			and		ebp,3
			jz		ALIGNED

			// Check for one pixel before starting.
			inc		edi
			jz		FINISH_REMAINDER

			// Do one pixel to align?
			mov		al,[edx + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_A:
			mov		ebx,[i_vfrac_delta]					// Load V fraction.

			add		ecx,ebx								// Step V fraction.
			mov		ebx,[pvFogPointer]					// Load base of fog table.

			sbb		ebp,ebp								// Get borrow from V fraction step.
			mov		cl,[eax*2 + ebx]					// Do CLUT lookup (low).

			mov		ch,[eax*2 + ebx + 1]				// Do CLUT lookup (high).
			mov		ebx,[i_ufrac_delta]					// Load U fraction.

			add		esi,ebx								// Step U fraction.
			/*
			mov		ebp,[pvBaseOfLine]					// Load pointer to destination.

			adc		edx,ai_uvdelta[ebp*4 + 4]			// integer step.
			*/
			adc		edx,ai_uvdelta[ebp*4 + 4]			// integer step.
			mov		ebp,[pvBaseOfLine]					// Load pointer to destination.

			mov		[ebp + edi*2 - 2],cx				// Store pixel.
			and		edx,((0xff << 9) | 0xff)			// Mask for tiling.

ALIGNED:
			add		edi,2
			jg		FINISH_REMAINDER

X_LOOP:
			mov		al,[edx + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_B:
			mov		ebx,[i_vfrac_delta]					// Load V fraction delta.

			add		ecx,ebx								// Step V fraction.
			mov		ebx,[i_ufrac_delta]					// Load U fraction delta.
			
			sbb		ebp,ebp								// Get carry from V fraction step.
			add		esi,ebx								// Step U fraction.

			adc		edx,ai_uvdelta[ebp*4 + 4]			// Integer step.
			add		ecx,[i_vfrac_delta]					// Step V fraction.

			sbb		ebp,ebp								// Get carry from V fraction step.
			mov		ebx,[pvFogPointer]					// Load base of fog table.

			and		edx,((0xff << 9) | 0xff)			// Mask for tiling.
			nop

			mov		ch,[eax*2 + ebx]					// Do CLUT lookup (low).
			mov		ebp,ai_uvdelta[ebp*4 + 4]			// Integer step based on carry.

			mov		cl,[eax*2 + ebx + 1]				// Do CLUT lookup (high).
			mov		ebx,[i_ufrac_delta]					// Load U fraction delta.

			add		esi,ebx								// Step U fraction.
			mov		al,[edx + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_C:

			adc		edx,ebp								// Integer step + u carry.
			mov		ebp,[pvFogPointer]					// Load base of fog table.

			and		edx,((0xff << 9) | 0xff)			// Mask for tiling.
			mov		ebx,ecx								// Copy first pixel.

			shl		ebx,16								// Shift first pixel up.

			mov		bh,[eax*2 + ebp]					// Do CLUT lookup (low).
			add		edi,2								// Step count.

			mov		bl,[eax*2 + ebp + 1]				// Do CLUT lookup (high).
			mov		ebp,[pvBaseOfLine]					// Load pointer to destination.

			bswap	ebx									// Reverse pixels.

			mov		[ebp + edi*2 - 8],ebx				// Store pixel.
			jle		X_LOOP

FINISH_REMAINDER:
			cmp		edi,1
			jne		LINE_DONE

			// Do left over pixel.
			mov		al,[edx + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_D:
			mov		ebx,[pvFogPointer]					// Load base of fog table.

			mov		cl,[eax*2 + ebx]					// Do CLUT lookup (low).
			mov		ebp,[pvBaseOfLine]					// Load pointer to destination.

			mov		ch,[eax*2 + ebx + 1]				// Do CLUT lookup (high).

			mov		[ebp - 2],cx						// Store pixel.

LINE_DONE:
			pop		ebp							// Restore ebp.

			//
			// Save updated values.
			//
			mov		eax,[pvFogPointer]
			mov		ebx,[pvBaseOfLine]

			mov		edi,[psv_saved]

			fstp	[f_clip]					// Store f_clip

			mov		[edi]SaveValues.i_uvint,edx
			mov		[edi]SaveValues.i_ufrac,esi

			mov		[edi]SaveValues.i_vfrac,ecx

			mov		[edi]SaveValues.pu2_dest,ebx
			mov		[edi]SaveValues.pv_fog,eax

			mov		eax,ai_uvdelta[0]
			mov		ebx,ai_uvdelta[4]

			mov		[edi]SaveValues.ai_uvdelta[0],eax
			mov		[edi]SaveValues.ai_uvdelta[4],ebx

			mov		eax,[i_ufrac_delta]
			mov		ebx,[i_vfrac_delta]

			mov		[edi]SaveValues.i_ufrac_delta,eax
			mov		[edi]SaveValues.i_vfrac_delta,ebx
			}

			psv_saved++;

			// we have done a scan line so adjust the start pointer to the next
			pu2Raster = pu2Raster + i4Pitch;
			u4_lines++;
		}

		// Do subsequent blocks.
		for (int i_pixel = iRENDER_BLOCK_SIZE; i_pixel < u4ScreenWidth; i_pixel += iRENDER_BLOCK_SIZE)
		{
			psv_saved = &asv_saved[0];
			int i4_width = std::min(iRENDER_BLOCK_SIZE, (int)u4ScreenWidth - i_pixel);

			for (i_line = 0; i_line < i_lines_drawn; i_line++)
			{
				__asm
				{
				//
				// Load saved values.
				//
				mov		edi,[psv_saved]

				mov		edx,[edi]SaveValues.i_uvint
				mov		esi,[edi]SaveValues.i_ufrac

				mov		ecx,[edi]SaveValues.i_vfrac

				mov		eax,[edi]SaveValues.ai_uvdelta[0]
				mov		ebx,[edi]SaveValues.ai_uvdelta[4]

				mov		ai_uvdelta[0],eax
				mov		ai_uvdelta[4],ebx

				mov		eax,[edi]SaveValues.i_ufrac_delta
				mov		ebx,[edi]SaveValues.i_vfrac_delta

				mov		[i_ufrac_delta],eax
				mov		[i_vfrac_delta],ebx

				mov		ebx,[edi]SaveValues.pu2_dest
				mov		eax,[edi]SaveValues.pv_fog

				mov		edi,[i4_width]
				push	ebp

				mov		[pvFogPointer],eax			// Global fog pointer.

				lea		ebp,[ebx+edi*2]				// Start + width of scanline.

				neg		edi

				mov		[pvBaseOfLine],ebp			// End of line pointer.

				//
				// ecx = vfrac
				// edx = integer uv
				// esi = ufrac
				// edi = -width
				//
				// pvBaseOfLine, pvFogPointer set.
				//
				xor		eax,eax
				and		ebx,3

				jz		ALIGNED2

				// Check for one pixel before starting.
				inc		edi
				jz		FINISH_REMAINDER2

				// Do one pixel to align?
				mov		al,[edx + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_E:
				mov		ebx,[i_vfrac_delta]					// Load V fraction.

				add		ecx,ebx								// Step V fraction.
				mov		ebx,[pvFogPointer]					// Load base of fog table.

				sbb		ebp,ebp								// Get borrow from V fraction step.
				mov		cl,[eax*2 + ebx]					// Do CLUT lookup (low).

				mov		ch,[eax*2 + ebx + 1]				// Do CLUT lookup (high).
				mov		ebx,[i_ufrac_delta]					// Load U fraction.

				add		esi,ebx								// Step U fraction.
				/*
				mov		ebp,[pvBaseOfLine]					// Load pointer to destination.

				adc		edx,ai_uvdelta[ebp*4 + 4]			// integer step.
				*/
				adc		edx,ai_uvdelta[ebp*4 + 4]			// integer step.
				mov		ebp,[pvBaseOfLine]					// Load pointer to destination.

				mov		[ebp + edi*2 - 2],cx				// Store pixel.
				and		edx,((0xff << 9) | 0xff)			// Mask for tiling.

ALIGNED2:
				add		edi,2
				jg		FINISH_REMAINDER2

X_LOOP2:
				mov		al,[edx + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_F:
				mov		ebx,[i_vfrac_delta]					// Load V fraction delta.

				add		ecx,ebx								// Step V fraction.
				mov		ebx,[i_ufrac_delta]					// Load U fraction delta.
				
				sbb		ebp,ebp								// Get carry from V fraction step.
				add		esi,ebx								// Step U fraction.

				adc		edx,ai_uvdelta[ebp*4 + 4]			// Integer step.
				add		ecx,[i_vfrac_delta]					// Step V fraction.

				sbb		ebp,ebp								// Get carry from V fraction step.
				mov		ebx,[pvFogPointer]					// Load base of fog table.

				and		edx,((0xff << 9) | 0xff)			// Mask for tiling.
				nop

				mov		ch,[eax*2 + ebx]					// Do CLUT lookup (low).
				mov		ebp,ai_uvdelta[ebp*4 + 4]			// Integer step based on carry.

				mov		cl,[eax*2 + ebx + 1]				// Do CLUT lookup (high).
				mov		ebx,[i_ufrac_delta]					// Load U fraction delta.

				add		esi,ebx								// Step U fraction.
				mov		al,[edx + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_G:

				adc		edx,ebp								// Integer step + u carry.
				mov		ebp,[pvFogPointer]					// Load base of fog table.

				and		edx,((0xff << 9) | 0xff)			// Mask for tiling.
				mov		ebx,ecx								// Copy first pixel.

				shl		ebx,16								// Shift first pixel up.

				mov		bh,[eax*2 + ebp]					// Do CLUT lookup (low).
				add		edi,2								// Step count.

				mov		bl,[eax*2 + ebp + 1]				// Do CLUT lookup (high).
				mov		ebp,[pvBaseOfLine]					// Load pointer to destination.

				bswap	ebx									// Reverse pixels.

				mov		[ebp + edi*2 - 8],ebx				// Store pixel.
				jle		X_LOOP2

FINISH_REMAINDER2:
				cmp		edi,1
				jne		LINE_DONE2

				// Do left over pixel.
				mov		al,[edx + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_H:
				mov		ebx,[pvFogPointer]					// Load base of fog table.

				mov		cl,[eax*2 + ebx]					// Do CLUT lookup (low).
				mov		ebp,[pvBaseOfLine]					// Load pointer to destination.

				mov		ch,[eax*2 + ebx + 1]				// Do CLUT lookup (high).

				mov		[ebp - 2],cx						// Store pixel.

LINE_DONE2:
				pop		ebp							// Restore ebp.

				//
				// Save updated values.
				//
				mov		edi,[psv_saved]
				mov		ebx,[pvBaseOfLine]

				mov		[edi]SaveValues.i_uvint,edx
				mov		[edi]SaveValues.i_ufrac,esi

				mov		[edi]SaveValues.i_vfrac,ecx
				mov		[edi]SaveValues.pu2_dest,ebx
				}

				psv_saved++;
			}
		}
	}

	// if the fill flag is set and the screen was not filled by the sky then fill it with the
	// back fog colour.
	if (bFill && u4_lines < u4ScreenHeight)
	{
		FillSky
		(
			pu2Raster,
			u4ScreenWidth,
			((i4Pitch-(int32)u4ScreenWidth)*prasRenderSurface->iPixelBits/8),
			u4ScreenHeight - u4_lines, 
			u4Fog
		);
	}
}

#pragma code_seg()
#pragma optimize("", on)

#elif (VER_ASM && TARGET_PROCESSOR == PROCESSOR_PENTIUMPRO)

//**********************************************************************************************
//
void CSkyRender::DrawSkyTexturedLinear()
//
//**************************************
{
	CVector3<>	v3_delta_left;
	CVector3<>	v3_delta_right;
	CVector3<>*	pv3_left;
	CVector3<>*	pv3_right;
	float		f_clip;
	int32		i4_fog;
	uint32		fp_u;
	uint32		fp_v;
	uint32		u4_lines = 0;

	static uint8* pu1_tex;
	static uint32 fp_delta_u;
	static uint32 fp_delta_v;
	static void*  pvFogPointer;
	static void*  pvBaseOfLine;

	pu1_tex = (uint8*)prasSkyTexture->pSurface;

	//
	// If the World space Z component of the frustum top left vector is negative then we are
	// drawing from bottom to top of the screen.
	//
	if (av3FrustTran[0].tZ < fSKY_HORIZON_Z)
	{
		// we are drawing from bottom to top
		// calculate the per scan line deltas..
		v3_delta_left	= (av3FrustTran[0] - av3FrustTran[2]) * fScreenHeightRep;
		v3_delta_right	= (av3FrustTran[1] - av3FrustTran[3]) * fScreenHeightRep;

		// point to the base vector we are using (bottom in this case)
		pv3_left	= &av3FrustTran[2];
		pv3_right	= &av3FrustTran[3];

		// adjust the screen pointer to point to the left edge of the last scanline
		pu2Raster = pu2Raster + (i4Pitch*((int)u4ScreenHeight-1));
		i4Pitch = - i4Pitch;
	}
	else
	{
		// we are drawing from top to bottom
		// calculate the per scan line deltas..
		v3_delta_left	= (av3FrustTran[2] - av3FrustTran[0]) * fScreenHeightRep;
		v3_delta_right	= (av3FrustTran[3] - av3FrustTran[1]) * fScreenHeightRep;

		// point to the base vector we are using (bottom in this case)
		pv3_left	= &av3FrustTran[0];
		pv3_right	= &av3FrustTran[1];
	}

	//Assert(iNUM_SKY_COLOURS == 64);

	typedef CVector3<> CVector3_float;
	typedef CSkyRender myCSkyRender;
	static int32 i_ufrac_delta;
	static int32 i_vfrac_delta;
	static int32 ai_uvdelta[2];

	// get the parametric clip point for the vector and sky plane, relative to the origin.
	// This is the initial clip point used for the first scan line, the next clip point
	// is calculated while the first scan line is being drawn..	
	f_clip = fSkyHeight / pv3_left->tZ;

	// while not at the horizion and there are scan lines left.
	while ( (pv3_left->tZ>fSKY_HORIZON_Z)  && (u4_lines<u4ScreenHeight) )
	{
		// calculate the fog band based on the Z component the ray vectors for this scan line,
		// we need to shift down u4DEFAULT_SKY_FOG_BITS because the linear version does not 
		// use the fog fraction.
		i4_fog = (int32) ((1.0f - fabs(pv3_left->tZ) - fFogNear) * fFogScale)>>u4DEFAULT_SKY_FOG_BITS;

		//
		// clamp the fog bands to the definded range...
		// If we above or equal to the last fog band then we can set the whole scan line to
		// a flat shaded colour.
		//
		if (i4_fog<0)
		{
			i4_fog = 0;
		}
		else if (i4_fog>=iNUM_SKY_FOG_BANDS-1)
		{
			// next ray cast vectors...
			*pv3_left+=v3_delta_left;
			*pv3_right+=v3_delta_right;

			// fire off the divide
			f_clip = fSkyHeight / (pv3_left->tZ);

			// fill 1 scan line
			FillSky(pu2Raster,u4ScreenWidth,0,1, u4Fog);

			// goto the next address
			pu2Raster = pu2Raster + i4Pitch;
			u4_lines++;
			continue;
		}

		// calculate the intersection points with the sky plane, scale it and convert it to
		// 16.16 fixed point.
		fp_u = (uint32) ( ((v3Camera.tX+(pv3_left->tX*f_clip))*fScale) + fSkyOffsetU);
		fp_v = (uint32) ( ((v3Camera.tY+(pv3_left->tY*f_clip))*fScale) + fSkyOffsetV);


		// scale the f_clip by the screen width scale so the U,V deltas come out from below
		// ready scaled.
		// fScreenWidthScaleRep contains a scale of 65536.0 to adjust for the fixed point
		f_clip *= fScreenWidthScaleRep;

		// vec_right and vec_left contain the intersection points of the frustum vectors for
		// this scan line and the sky plane (in world space, with constant height).
		// The UVs are calculated as the difference between these positions divided by the
		// number of steps/pixels in the screen width.
		// These deltas are then scaled by the pixel/world size and the fixed point constant
		// 65536.0f
		fp_delta_u = (uint32) ( ((pv3_right->tX) - (pv3_left->tX) ) * f_clip);
		fp_delta_v = (uint32) ( ((pv3_right->tY) - (pv3_left->tY) ) * f_clip);

		// calculate the ray cast vectors for the next scan line
		*pv3_left+=v3_delta_left;
		*pv3_right+=v3_delta_right;

		__asm
		{
		mov		ecx,this						// Load object pointer.
		mov		eax,[pv3_left]					// Load vertex pointer.

		fld		[ecx]myCSkyRender.fSkyHeight	// Load sky height. 
		fdiv	[eax]CVector3_float.tZ			// fSkyHeight / (pv3_left->tZ)
		// Overlap division with scanline.

		//
		// Per scanline loop.
		//
		lea		esi,[ecx]myCSkyRender.u2FogTable	// Pointer to fog table.
		mov		eax,[i4_fog]						// Load fog value.

		shl		eax,8								// Shift for width of table.
		mov		ebx,[ecx]myCSkyRender.pu2Raster		// Pointer to destination.

		mov		edi,[ecx]myCSkyRender.u4ScreenWidth	// Load width of scanline.
		add		esi,eax								// Adjust fog table base for i4_fog.

		push	ebp							// Save ebp.
		mov		[pvFogPointer],esi			// Global fog pointer.

		lea		ecx,[ebx+edi*2]				// Start + width of scanline.
		mov		eax,[fp_u]					// Load fixed point U.

		neg		edi							// Negative width.

		mov		[pvBaseOfLine],ecx			// One past end of scanline pointer.
		mov		ebx,[fp_v]					// Load fixed point V.

X_LOOP:
		mov		ebp,eax						// get copy of u
		mov		ecx,ebx						// get copy of v

		sar		ebp,16						// integral u
		mov		esi,[pu1_tex]				// load base of texture.

		sar		ecx,(16 - 9)				// integral v
		and		ebp,0xff					// mask u for tiling

		and		ecx,(0xff << 9)				// mask v for tiling
		add		ebp,esi						// add texture base to u.

		xor		edx,edx						// Zero edx.
		mov		esi,[pvFogPointer]			// load base of fog table.

		mov		dl,[ebp+ecx]				// fetch pixel
		mov		ebp,[pvBaseOfLine]			// base of end of line.

		add		eax,[fp_delta_u]			// Step U.
		add		ebx,[fp_delta_v]			// Step V.

		mov		dx,[esi + edx*2]			// Lookup fogged pixel.

		mov		[ebp + edi*2],dx			// Store pixel.

		inc		edi
		jnz		X_LOOP

		pop		ebp							// Restore ebp.

		fstp	[f_clip]					// Store f_clip
		}

		// we have done a scan line so adjust the start pointer to the next
		pu2Raster = pu2Raster + i4Pitch;
		u4_lines++;
	}

	// if the fill flag is set and the screen was not filled by the sky then fill it with the
	// back fog colour.
	if (bFill && u4_lines<u4ScreenHeight)
	{
		FillSky
		(
			pu2Raster,
			u4ScreenWidth,
			((i4Pitch-(int32)u4ScreenWidth)*prasRenderSurface->iPixelBits/8),
			u4ScreenHeight - u4_lines, 
			u4Fog
		);
	}
}

#elif (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)

//**********************************************************************************************
//
struct SaveValues
//
// Values saved between scanlines.
//
//**************************************
{
	uint64	pd_uv;
	uint64	pd_uvstep;
	void*	pu2_dest;
	void*	pv_fog;
};

#define iRENDER_BLOCK_SIZE 64

#define ieeeSKY_HORIZON_Z 0x3d4ccccd

//**********************************************************************************************
//
void CSkyRender::DrawSkyTexturedLinear()
//
//**************************************
{
	CVector3<>	v3_delta_left;
	CVector3<>	v3_delta_right;
	CVector3<>*	pv3_left;
	CVector3<>*	pv3_right;
	uint32		u4_lines;
	uint8*		pu1_tex;
	SaveValues	asv_saved[iRENDER_BLOCK_SIZE];

	const float fOne = 1.0f;
	const uint64 pdMFactor = 0x0200000102000001;
	const uint64 pwUVMasks = 0x00ff00ff00ff00ff;

	static uint64 pdIndexTemp;

	typedef CVector3<> CVector3_float;
	typedef CSkyRender myCSkyRender;

#if (VER_DEBUG)
	//Assert(iNUM_SKY_COLOURS == 64);
	float f_temp = fSKY_HORIZON_Z;
	Assert(*(int *)&f_temp == ieeeSKY_HORIZON_Z);
#endif

	u4_lines = 0;
	pu1_tex = (uint8*)prasSkyTexture->pSurface;

	//
	// If the World space Z component of the frustum top left vector is negative then we are
	// drawing from bottom to top of the screen.
	//
	if (av3FrustTran[0].tZ < fSKY_HORIZON_Z)
	{
		// we are drawing from bottom to top
		// calculate the per scan line deltas..
		v3_delta_left	= (av3FrustTran[0] - av3FrustTran[2]) * fScreenHeightRep;
		v3_delta_right	= (av3FrustTran[1] - av3FrustTran[3]) * fScreenHeightRep;

		// point to the base vector we are using (bottom in this case)
		pv3_left	= &av3FrustTran[2];
		pv3_right	= &av3FrustTran[3];

		// adjust the screen pointer to point to the left edge of the last scanline
		pu2Raster = pu2Raster + (i4Pitch*((int)u4ScreenHeight-1));
		i4Pitch = - i4Pitch;
	}
	else
	{
		// we are drawing from top to bottom
		// calculate the per scan line deltas..
		v3_delta_left	= (av3FrustTran[2] - av3FrustTran[0]) * fScreenHeightRep;
		v3_delta_right	= (av3FrustTran[3] - av3FrustTran[1]) * fScreenHeightRep;

		// point to the base vector we are using (bottom in this case)
		pv3_left	= &av3FrustTran[0];
		pv3_right	= &av3FrustTran[1];
	}

	CVector3_float v3CameraCopy = v3Camera;

	int b_done;
	int i_line;
	int i_lines_drawn;
	int i_pixel;
	int i_width;
	SaveValues *psv_saved;

	__asm
	{
		femms										// ensure fast switch.

		mov			[b_done],0						// b_done = false

//
// while (!b_done)
//
SCANLINE_LOOP:
		lea			eax,[asv_saved]
		mov			ecx,iRENDER_BLOCK_SIZE

		mov			[psv_saved],eax					// psv_saved = &asv_saved[0]

		mov			[i_lines_drawn],ecx				// i_lines_drawn = iRENDER_BLOCK_SIZE
		mov			eax,[pv3_left]					// left vertex.

		mov			ebx,0
		mov			esi,this						// this pointer.

		mov			[i_line],ebx					// i_line = 0
		mov			edx,[u4_lines]					// number of lines to draw.

//
// for (i_line = 0; i_line < iRENDER_BLOCK_SIZE; i_line++)
//
// eax = pv3_left
// ebx = i_line
// edx = u4_lines
// esi = this
//
FIRST_BLOCK_LINE_LOOP:
		cmp			ebx,iRENDER_BLOCK_SIZE
		jge			EXIT_FB_LINE_LOOP

		//
		// Early out of loop if needed.
		//
		mov			ebx,[eax]CVector3_float.tZ

		mov			ecx,[esi]myCSkyRender.u4ScreenHeight

		cmp			ebx,ieeeSKY_HORIZON_Z			// pv3_left->tZ > fSKY_HORIZON_Z
		jle			EXIT_FB_LINE_LOOP_FINISHED

		movd		mm1,ebx							// pv3_left->tZ
		and			ebx,0x7fffffff					// fabs(pv3_left->tZ)

		cmp			edx,ecx							// u4_lines < u4ScreenHeight
		jge			EXIT_FB_LINE_LOOP_FINISHED

		// i4_fog = (int32) ((1.0f - fabs(pv3_left->tZ) - fFogNear) * fFogScale)>>u4DEFAULT_SKY_FOG_BITS;
		movd		mm2,ebx

		movd		mm3,[fOne]

		movd		mm4,[esi]myCSkyRender.fFogNear

		movd		mm5,[esi]myCSkyRender.fFogScale
		pfsubr		(m2,m3)

		pfsub		(m2,m4)

		pfmul		(m2,m5)

		pf2id		(m2,m2)

		movd		ebx,mm2

		sar			ebx,u4DEFAULT_SKY_FOG_BITS

		cmp			ebx,0
		jge			I4FOG_POSITIVE

		mov			ebx,0
		jmp			short I4FOG_IN_RANGE

I4FOG_POSITIVE:
		cmp			ebx,iNUM_SKY_FOG_BANDS-1
		jl			I4FOG_IN_RANGE

		// FillSky(pu2Raster, u4ScreenWidth, 0, 1, u4Fog);
		mov			ebx,[esi]myCSkyRender.u4Fog

		mov			eax,[esi]myCSkyRender.u4ScreenWidth

		movd		mm0,ebx

		mov			edi,[esi]myCSkyRender.pu2Raster
		punpckldq	mm0,mm0

		// align to a 64 bit boundardry
ALIGN_64:
		test	edi,7						// are we aligned to a 64 bit (8 byte) boundary??
		jz		ALIGNED_64
		mov		[edi],bx
		add		edi,2
		dec		eax
		jnz		ALIGN_64
		jmp		short FILLSKY_DONE

ALIGNED_64:
		sub		eax,8						// can we do 8 pixels (16 bytes)
		jl		FINISH_X

LOOPX:
		movq	[edi],mm0

		movq	[edi+8],mm0
		add		edi,16

		sub		eax,8						// can we do 8 pixels (16 bytes)
		jge		LOOPX

FINISH_X:
		add		eax,8
		jz		FILLSKY_DONE

LOOP_FINISH_X:
		mov		[edi],bx
		add		edi,2
		dec		eax
		jnz		LOOP_FINISH_X

FILLSKY_DONE:
		dec		[i_lines_drawn]				// i_lines_drawn--
		jmp		END_OF_SCANLINE_NO_SAVE

I4FOG_IN_RANGE:
		// f_clip = fSkyHeight / pv3_left->tZ;
		// fp_u = (uint32) (((v3Camera.tX + (pv3_left->tX * f_clip)) * fScale) + fSkyOffsetU);
		// fp_v = (uint32) (((v3Camera.tY + (pv3_left->tY * f_clip)) * fScale) + fSkyOffsetV);
		// fp_delta_u = (uint32) (((pv3_right->tX) - (pv3_left->tX)) * f_clip * fScreenWidthScaleRep);
		// fp_delta_v = (uint32) (((pv3_right->tY) - (pv3_left->tY)) * f_clip * fScreenWidthScaleRep);
		pfrcp		(m1,m1)
		movd		mm0,[esi]myCSkyRender.fSkyHeight

		mov			ecx,[pv3_right]

		pfmul		(m0,m1)

		movq		mm7,[eax]CVector3_float.tX

		movq		mm6,[ecx]CVector3_float.tX
		punpckldq	mm0,mm0

		movq		mm1,[v3CameraCopy.tX]

		pfsub		(m6,m7)
		pfmul		(m7,m0)

		pfadd		(m7,m1)
		movq		mm2,[esi]myCSkyRender.fScale

		punpckldq	mm2,mm2

		pfmul		(m7,m2)

		movd		mm3,[esi]myCSkyRender.fSkyOffsetU

		punpckldq	mm3,[esi]myCSkyRender.fSkyOffsetV

		pfadd		(m7,m3)

		pfmul		(m6,m0)

		movd		mm4,[esi]myCSkyRender.fScreenWidthScaleRep

		punpckldq	mm4,mm4

		pfmul		(m6,m4)

		pf2id		(m7,m7)							// Convert texture values to 16.16 fixed.

		pf2id		(m6,m6)							// Convert texture values to 16.16 fixed.

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index1
		// ecx = fog_table_ptr		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor (512,1)	mm3 = uvmask (0x00ff, 0x00ff, 0x00ff, 0x00ff)
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		lea			ecx,[esi]myCSkyRender.u2FogTable	// Pointer to fog table.

		shl			ebx,8								// Shift for width of table.

		add			ecx,ebx								// Adjust fog table base for i4_fog.

		sub			ecx,4								// Hack to force SIB + offset in loop

		mov			edx,[esi]myCSkyRender.pu2Raster		// Pointer to destination.

		mov			edi,iRENDER_BLOCK_SIZE				// Load width of scanline.

		lea			edx,[edx+edi*2]						// Start + width of scanline.

		neg			edi									// Negative width.

		movq		mm2,[pdMFactor]						// Load [width,1]

		movq		mm3,[pwUVMasks]						// Load masks.

		movq		mm0,mm7								// Copy U1,V1
		paddd		mm7,mm6								// Step U,V

		psrad		mm0,16								// Shift for integer U1,V1
		movq		mm1,mm7								// Copy U2,V2

		psrad		mm1,16								// Shift for integer U2,V2

		packssdw	mm0,mm1								// Pack integer texture values

		pand		mm0,mm3								// Mask for tiling

		pmaddwd		mm0,mm2								// Compute texture indicies.

		mov			esi,[pu1_tex]						// load base of texture.

		sub			esi,4								// Hack to force SIB + offset in loop

		//
		// Align to a quadword boundry.
		//
		lea			eax,[edx + edi*2]

		and			eax,3
		jz			ALIGNED2_A

		inc			edi								// One pixel left?
		jz			ONE_PIXEL_LEFT_A

		//
		// Do one pixel for alignment.
		//
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		psrad		mm0,16							// Shift for integer U1,V1
		packssdw	mm1,mm0							// Pack integer texture values.

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm0,mm1							// Put values back into mm0

		pand		mm0,mm3							// Mask for tiling
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		lea			eax,[edx + edi*2]

		mov			[edx + edi*2 - 2],bx			// Store pixel

ALIGNED2_A:
		and			eax,7
		jz			ALIGNED4_A

		add			edi,2							// Two pixels left?
		jge			FINISH_REMAINDER2_A

		//
		// Do two pixels for alignment.
		//
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 2
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx + edi*2 - 4],ebx			// Store pixels

ALIGNED4_A:
		add		edi,4		
		jl		INNER_LOOP_4P_A						// Four pixles left?

		jmp		FINISH_REMAINDER4_A

		// Align start of loop to 0 past the beginning of a cache line.
		ALIGN	16

		//
		// Do four pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_4P_A:
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift second pixel up.
		movq		[pdIndexTemp],mm0				// Save texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm4,ebx							// Put in MMX register.

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		movq		mm1,mm7							// Copy U2,V2
		psrad		mm1,16							// Shift for integer U2,V2

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		shl			eax,16							// Shift second pixel up.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm1,ebx							// Put in MMX register.

		punpckldq	mm4,mm1							// Combine all four pixels.
		movq		[edx + edi*2 - 8],mm4			// Store 4 pixels

		add			edi,4							// Inrcement index by 4
		jl			INNER_LOOP_4P_A					// Loop (while >4 pixels)

FINISH_REMAINDER4_A:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		jnz			THREE_OR_LESS_A

		//
		// Four pixels left, step co-ordinates 3 times.
		//
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift second pixel up.
		movq		[pdIndexTemp],mm0				// Save texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm4,ebx							// Put in MMX register.

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1

		shl			eax,16							// Shift second pixel up.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm1,ebx							// Put in MMX register.

		punpckldq	mm4,mm1							// Combine all four pixels.
		movq		[edx - 8],mm4					// Store 4 pixels

		jmp			END_OF_SCANLINE_A

THREE_OR_LESS_A:
		sub			edi,2
		jge			FINISH_REMAINDER2_A

		//
		// Three pixels left, step co-ordinates twice.
		//
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 2
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx - 6],ebx					// Store pixels

		movd		ebx,mm0							// Get texture index 1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		mov			bx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p1

		mov			[edx - 2],bx					// Store pixel 1

		jmp			short END_OF_SCANLINE_A

FINISH_REMAINDER2_A:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			ONE_PIXEL_LEFT_A

		//
		// Two pixels left, only step texture co-ordinates once.
		//
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx - 4],ebx					// Store pixels

		jmp			short END_OF_SCANLINE_A

ONE_PIXEL_LEFT_A:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movd		ebx,mm0							// Get texture index 1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		mov			bx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p1

		mov			[edx - 2],bx					// Store pixel 1

END_OF_SCANLINE_A:
		//
		// Save values.
		//
		mov			edi,[psv_saved]

		movq		[edi]SaveValues.pd_uv,mm7
		movq		[edi]SaveValues.pd_uvstep,mm6
		mov			[edi]SaveValues.pu2_dest,edx
		mov			[edi]SaveValues.pv_fog,ecx

		// psv_saved++;
		add			edi,SIZE SaveValues
		mov			[psv_saved],edi

END_OF_SCANLINE_NO_SAVE:
		//
		// *pv3_left += v3_delta_left;
		// *pv3_right += v3_delta_right;
		//
		mov			eax,[pv3_left]

		mov			ebx,[pv3_right]

		movq		mm0,[eax]CVector3_float.tX

		movd		mm1,[eax]CVector3_float.tZ

		movq		mm2,v3_delta_left.tX

		movd		mm3,v3_delta_left.tZ

		pfadd		(m0,m2)

		pfadd		(m1,m3)

		movq		[eax]CVector3_float.tX,mm0

		movd		[eax]CVector3_float.tZ,mm1

		movq		mm0,[ebx]CVector3_float.tX

		movd		mm1,[ebx]CVector3_float.tZ

		movq		mm2,v3_delta_left.tX

		movd		mm3,v3_delta_left.tZ

		pfadd		(m0,m2)

		pfadd		(m1,m3)

		movq		[ebx]CVector3_float.tX,mm0

		movd		[ebx]CVector3_float.tZ,mm1

		//
		// pu2Raster = pu2Raster + i4Pitch;
		// u4_lines++;
		// i_line++;
		//
		mov			esi,this

		mov			ecx,[esi]myCSkyRender.pu2Raster

		mov			ebx,[esi]myCSkyRender.i4Pitch

		lea			ecx,[ecx + ebx*2]
		mov			[esi]myCSkyRender.pu2Raster,ecx

		mov			edx,[u4_lines]

		inc			edx
		mov			[u4_lines],edx

		mov			ebx,[i_line]

		inc			ebx
		mov			[i_line],ebx

		jmp			FIRST_BLOCK_LINE_LOOP

EXIT_FB_LINE_LOOP_FINISHED:
		mov			[b_done],1						// set b_done flag.

		//
		// i_lines_drawn = i_lines_drawn + i_line - iRENDER_BLOCK_SIZE;
		//
		mov			eax,[i_lines_drawn]
		mov			ebx,[i_line]
		sub			eax,iRENDER_BLOCK_SIZE
		add			eax,ebx
		mov			[i_lines_drawn],eax

EXIT_FB_LINE_LOOP:
		mov			ebx,iRENDER_BLOCK_SIZE
		mov			[i_pixel],ebx

		mov			esi,this
			
//
// for (int i_pixel = iRENDER_BLOCK_SIZE; i_pixel < u4ScreenWidth; i_pixel += iRENDER_BLOCK_SIZE)
//
// ebx = i_pixel
// esi = this
//
BLOCK_LOOP:
		// psv_saved = &asv_saved[0];
		lea			edi,[asv_saved]

		mov			[psv_saved],edi					// psv_saved = &asv_saved[0]

		// i_width = min(iRENDER_BLOCK_SIZE, (int)u4ScreenWidth - i_pixel)
		mov			ecx,[esi]myCSkyRender.u4ScreenWidth

		sub			ecx,ebx

		cmp			ecx,iRENDER_BLOCK_SIZE
		jle			ECX_IS_MIN

		mov			ecx,iRENDER_BLOCK_SIZE

ECX_IS_MIN:
		mov			[i_width],ecx
		mov			eax,0

		mov			[i_line],eax
		jmp			ENTER_BLOCK_LINE_LOOP

//
// for (i_line = 0; i_line < i_lines_drawn; i_line++)
//
// esi = this
// edi = psv_saved
//
BLOCK_LINE_LOOP:
		//
		// Load saved values.
		//
		movq		mm7,[edi]SaveValues.pd_uv
		movq		mm6,[edi]SaveValues.pd_uvstep
		mov			edx,[edi]SaveValues.pu2_dest
		mov			ecx,[edi]SaveValues.pv_fog

		mov			edi,[i_width]

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index1
		// ecx = fog_table_ptr		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor (512,1)	mm3 = uvmask (0x00ff, 0x00ff, 0x00ff, 0x00ff)
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		lea			edx,[edx+edi*2]						// Start + width of scanline.

		neg			edi									// Negative width.

		movq		mm2,[pdMFactor]						// Load [width,1]

		movq		mm3,[pwUVMasks]						// Load masks.

		movq		mm0,mm7								// Copy U1,V1
		paddd		mm7,mm6								// Step U,V

		psrad		mm0,16								// Shift for integer U1,V1
		movq		mm1,mm7								// Copy U2,V2

		psrad		mm1,16								// Shift for integer U2,V2

		packssdw	mm0,mm1								// Pack integer texture values

		pand		mm0,mm3								// Mask for tiling

		pmaddwd		mm0,mm2								// Compute texture indicies.

		mov			esi,[pu1_tex]						// load base of texture.

		sub			esi,4								// Hack to force SIB + offset in loop

		//
		// Align to a quadword boundry.
		//
		lea			eax,[edx + edi*2]

		and			eax,3
		jz			ALIGNED2_B

		inc			edi								// One pixel left?
		jz			ONE_PIXEL_LEFT_B

		//
		// Do one pixel for alignment.
		//
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		psrad		mm0,16							// Shift for integer U1,V1
		packssdw	mm1,mm0							// Pack integer texture values.

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm0,mm1							// Put values back into mm0

		pand		mm0,mm3							// Mask for tiling
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		lea			eax,[edx + edi*2]

		mov			[edx + edi*2 - 2],bx			// Store pixel

ALIGNED2_B:
		and			eax,7
		jz			ALIGNED4_B

		add			edi,2							// Two pixels left?
		jge			FINISH_REMAINDER2_B

		//
		// Do two pixels for alignment.
		//
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 2
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx + edi*2 - 4],ebx			// Store pixels

ALIGNED4_B:
		add		edi,4
		jl		INNER_LOOP_4P_B						// Four pixles left?

		jmp		FINISH_REMAINDER4_B

		// Align start of loop to 0 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16

		//
		// Do four pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_4P_B:
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift second pixel up.
		movq		[pdIndexTemp],mm0				// Save texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm4,ebx							// Put in MMX register.

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		movq		mm1,mm7							// Copy U2,V2
		psrad		mm1,16							// Shift for integer U2,V2

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		shl			eax,16							// Shift second pixel up.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm1,ebx							// Put in MMX register.

		punpckldq	mm4,mm1							// Combine all four pixels.
		movq		[edx + edi*2 - 8],mm4			// Store 4 pixels

		add			edi,4							// Inrcement index by 4
		jl			INNER_LOOP_4P_B					// Loop (while >4 pixels)

FINISH_REMAINDER4_B:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		jnz			THREE_OR_LESS_B

		//
		// Four pixels left, step co-ordinates 3 times.
		//
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift second pixel up.
		movq		[pdIndexTemp],mm0				// Save texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm4,ebx							// Put in MMX register.

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1

		shl			eax,16							// Shift second pixel up.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm1,ebx							// Put in MMX register.

		punpckldq	mm4,mm1							// Combine all four pixels.
		movq		[edx - 8],mm4					// Store 4 pixels

		jmp			END_OF_SCANLINE_B

THREE_OR_LESS_B:
		sub			edi,2
		jge			FINISH_REMAINDER2_B

		//
		// Three pixels left, step co-ordinates twice.
		//
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 2
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx - 6],ebx					// Store pixels

		movd		ebx,mm0							// Get texture index 1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		mov			bx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p1

		mov			[edx - 2],bx					// Store pixel 1

		jmp			short END_OF_SCANLINE_B

FINISH_REMAINDER2_B:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			ONE_PIXEL_LEFT_B

		//
		// Two pixels left, only step texture co-ordinates once.
		//
		movq		[pdIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[pdIndexTemp + 4]	// Get texture index 2

		mov			ebx,dword ptr[pdIndexTemp]		// Get texture index 1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx - 4],ebx					// Store pixels

		jmp			short END_OF_SCANLINE_B

ONE_PIXEL_LEFT_B:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movd		ebx,mm0							// Get texture index 1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		mov			bx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p1

		mov			[edx - 2],bx					// Store pixel 1

END_OF_SCANLINE_B:
		//
		// Save updated values.
		//
		mov			edi,[psv_saved]

		movq		[edi]SaveValues.pd_uv,mm7
		movq		[edi]SaveValues.pd_uvstep,mm6
		mov			[edi]SaveValues.pu2_dest,edx

		// psv_saved++;
		// i_line++;
		add			edi,SIZE SaveValues
		mov			eax,[i_line]

		mov			[psv_saved],edi
		inc			eax

		mov			esi,this
		mov			[i_line],eax

ENTER_BLOCK_LINE_LOOP:
		cmp			eax,[i_lines_drawn]
		jl			BLOCK_LINE_LOOP

		mov			ebx,[i_pixel]

//ENTER_BLOCK_LOOP:
		add			ebx,iRENDER_BLOCK_SIZE
		mov			[i_pixel],ebx

		cmp			ebx,[esi]myCSkyRender.u4ScreenWidth
		jl			BLOCK_LOOP

		mov			eax,[b_done]

		test		eax,eax
		jz			SCANLINE_LOOP

		femms										// ensure fast switch.
	}

	// if the fill flag is set and the screen was not filled by the sky then fill it with the
	// back fog colour.
	if (bFill && u4_lines < u4ScreenHeight)
	{
		FillSky
		(
			pu2Raster,
			u4ScreenWidth,
			((i4Pitch-(int32)u4ScreenWidth)*prasRenderSurface->iPixelBits/8),
			u4ScreenHeight - u4_lines, 
			u4Fog
		);
	}
}

#else

//**********************************************************************************************
//
void CSkyRender::DrawSkyTexturedLinear()
//
//**************************************
{
	CVector3<>	v3_delta_left;
	CVector3<>	v3_delta_right;
	CVector3<>*	pv3_left;
	CVector3<>*	pv3_right;
	float		f_clip;
	int32		i4_fog;
	int32		fp_u;
	int32		fp_v;
	uint32		u4_lines = 0;
	uint8* pu1_tex;
	uint32 u4_x;
	uint32 u4_adr;
	int32 fp_delta_u;
	int32 fp_delta_v;

	pu1_tex = (uint8*)prasSkyTexture->pSurface;

	//
	// If the World space Z component of the frustum top left vector is negative then we are
	// drawing from bottom to top of the screen.
	//
	if (av3FrustTran[0].tZ < fSKY_HORIZON_Z)
	{
		// we are drawing from bottom to top
		// calculate the per scan line deltas..
		v3_delta_left	= (av3FrustTran[0] - av3FrustTran[2]) * fScreenHeightRep;
		v3_delta_right	= (av3FrustTran[1] - av3FrustTran[3]) * fScreenHeightRep;

		// point to the base vector we are using (bottom in this case)
		pv3_left	= &av3FrustTran[2];
		pv3_right	= &av3FrustTran[3];

		// adjust the screen pointer to point to the left edge of the last scanline
		pu2Raster = pu2Raster + (i4Pitch*((int)u4ScreenHeight-1));
		i4Pitch = - i4Pitch;
	}
	else
	{
		// we are drawing from top to bottom
		// calculate the per scan line deltas..
		v3_delta_left	= (av3FrustTran[2] - av3FrustTran[0]) * fScreenHeightRep;
		v3_delta_right	= (av3FrustTran[3] - av3FrustTran[1]) * fScreenHeightRep;

		// point to the base vector we are using (bottom in this case)
		pv3_left	= &av3FrustTran[0];
		pv3_right	= &av3FrustTran[1];
	}

	// get the parametric clip point for the vector and sky plane, relative to the origin.
	// This is the initial clip point used for the first scan line, the next clip point
	// is calculated while the first scan line is being drawn..	
	f_clip = fSkyHeight / pv3_left->tZ;

	// while not at the horizion and there are scan lines left.
	while ( (pv3_left->tZ>fSKY_HORIZON_Z)  && (u4_lines<u4ScreenHeight) )
	{
		// calculate the fog band based on the Z component the ray vectors for this scan line,
		// we need to shift down u4DEFAULT_SKY_FOG_BITS because the linear version does not 
		// use the fog fraction.
		i4_fog = (int32) ((1.0f - fabs(pv3_left->tZ) - fFogNear) * fFogScale)>>u4DEFAULT_SKY_FOG_BITS;

		//
		// clamp the fog bands to the definded range...
		// If we above or equal to the last fog band then we can set the whole scan line to
		// a flat shaded colour.
		//
		if (i4_fog<0)
		{
			i4_fog = 0;
		}
		else if (i4_fog>=iNUM_SKY_FOG_BANDS-1)
		{
			// next ray cast vectors...
			*pv3_left+=v3_delta_left;
			*pv3_right+=v3_delta_right;

			// fire off the divide
			f_clip = fSkyHeight / (pv3_left->tZ);

			// fill 1 scan line
			FillSky(pu2Raster,u4ScreenWidth,0,1, u4Fog);

			// goto the next address
			pu2Raster = pu2Raster + i4Pitch;
			u4_lines++;
			continue;
		}

		// calculate the intersection points with the sky plane, scale it and convert it to
		// 16.16 fixed point.
		fp_u = static_cast<int32>( ((v3Camera.tX+(pv3_left->tX*f_clip))*fScale) + fSkyOffsetU);
		fp_v = static_cast<int32>( ((v3Camera.tY+(pv3_left->tY*f_clip))*fScale) + fSkyOffsetV);


		// scale the f_clip by the screen width scale so the U,V deltas come out from below
		// ready scaled.
		// fScreenWidthScaleRep contains a scale of 65536.0 to adjust for the fixed point
		f_clip *= fScreenWidthScaleRep;

		// vec_right and vec_left contain the intersection points of the frustum vectors for
		// this scan line and the sky plane (in world space, with constant height).
		// The UVs are calculated as the difference between these positions divided by the
		// number of steps/pixels in the screen width.
		// These deltas are then scaled by the pixel/world size and the fixed point constant
		// 65536.0f
		fp_delta_u = static_cast<int32>( ((pv3_right->tX) - (pv3_left->tX) ) * f_clip);
		fp_delta_v = static_cast<int32>( ((pv3_right->tY) - (pv3_left->tY) ) * f_clip);

		// calculate the ray cast vectors for the next scan line
		*pv3_left+=v3_delta_left;
		*pv3_right+=v3_delta_right;

		//
		// get the parametric clip point for the vector and sky plane, relative to the player.
		// This is for the next scan line, the floating point divide will overlap the drawing
		// code of this scan line which is all fixed point...
		//
		f_clip = fSkyHeight / (pv3_left->tZ);

		// while the divide is going draw the scan line with all integer code
		for (u4_x = 0; u4_x<u4ScreenWidth; u4_x++)
		{
			u4_adr = (((fp_v>>16) & 0xff)<<9) | ((fp_u>>16) & 0xff);

			pu2Raster[u4_x] = u2FogTable[ i4_fog ][ pu1_tex[u4_adr] ];

			fp_u += fp_delta_u;
			fp_v += fp_delta_v;
		}

		// we have done a scan line so adjust the start pointer to the next
		pu2Raster = pu2Raster + i4Pitch;
		u4_lines++;
	}

	// if the fill flag is set and the screen was not filled by the sky then fill it with the
	// back fog colour.
	if ((bFill) && (u4_lines<u4ScreenHeight))
	{
		FillSky
		(
			pu2Raster,
			u4ScreenWidth,
			((i4Pitch-(int32)u4ScreenWidth)*prasRenderSurface->iPixelBits/8),
			u4ScreenHeight - u4_lines, 
			u4Fog
		);
	}
}

#endif


//**********************************************************************************************
// Draws a flat shaded sky which is not horizontal. The edge walking is identical to the textured
// primitive but no sub-division are made. The fog start and end colour is calculated once per
// scan line and then interpolated at every pixel.
//
void CSkyRender::DrawSkyFlatPerspective()
//
//**************************************
{
	CVector3<>	v3_delta_start;
	CVector3<>	v3_delta_end;
	CVector3<>*	pv3_start;
	CVector3<>*	pv3_end;

	int32		i4_fog_s;
	int32		i4_fog_e;
	int32		i4_fog_inc;
	uint16*		pu2_pixel;
	int32		i4_pixels;
	int32		i4_dir;
	float		f_pixels;
	uint32		u4_lines = 0;


	//
	// fast fill the whole screen if the fill flag is switched on. This is quicker than
	// trying to fill just the pixels that the sky does not cover.
	//
	if (bFill)
	{
		FillSky
		(
			pu2Raster,
			u4ScreenWidth,
			(i4Pitch-(int32)u4ScreenWidth)*2,
			u4ScreenHeight,
			u4Fog
		);
	}


	//
	// Determine which of the corners of the screen is closest to the sky. This is done
	// by checking the sign of the up and right vectors. The drawing direction is always
	// away from the closest point.
	//

	if (av3FrustTran[4].tZ>0.0f)
	{
		//
		// Top vector is facing up (+ve Z) so the screen is the correct way around..
		//
		if (av3FrustTran[5].tZ<=0.0f)
		{
			// Right vector Z is 0 or less so we are drawing in the normal direction
			v3_delta_start	= (av3FrustTran[2] - av3FrustTran[0]) * fScreenHeightRep;
			v3_delta_end	= (av3FrustTran[3] - av3FrustTran[1]) * fScreenHeightRep;

			pv3_start	= &av3FrustTran[0];
			pv3_end		= &av3FrustTran[1];

			// Draw direction is from left to right. top to bottom
			i4_dir			= 1;
		}
		else
		{
			// Right vector X is begative so we draw from the right
			v3_delta_start	= (av3FrustTran[3] - av3FrustTran[1]) * fScreenHeightRep;
			v3_delta_end	= (av3FrustTran[2] - av3FrustTran[0]) * fScreenHeightRep;

			pv3_start		= &av3FrustTran[1];
			pv3_end			= &av3FrustTran[0];

			// Draw direction is from right to left, top to bottom
			i4_dir		= -1;
			pu2Raster = pu2Raster + u4ScreenWidth-1;
		}
	}
	else
	{
		//
		// The top vector is facing down so the screen is upside down
		//
		if (av3FrustTran[5].tZ<=0.0f)
		{
			// Right vector Z is 0 or less so we are drawing in the normal direction
			v3_delta_start	= (av3FrustTran[0] - av3FrustTran[2]) * fScreenHeightRep;
			v3_delta_end	= (av3FrustTran[1] - av3FrustTran[3]) * fScreenHeightRep;

			pv3_start		= &av3FrustTran[2];
			pv3_end			= &av3FrustTran[3];

			// draw direction is from left to right, bottom to top
			i4_dir		= 1;
			pu2Raster = pu2Raster + (i4Pitch*((int32)u4ScreenHeight-1));
			i4Pitch = - i4Pitch;
		}
		else
		{
			// Right vector X is begative so we draw from the right
			v3_delta_start	= (av3FrustTran[1] - av3FrustTran[3]) * fScreenHeightRep;
			v3_delta_end	= (av3FrustTran[0] - av3FrustTran[2]) * fScreenHeightRep;

			pv3_start		= &av3FrustTran[3];
			pv3_end			= &av3FrustTran[2];

			// draw direction is from right to left, bottom to top
			i4_dir		= -1;
			pu2Raster = pu2Raster + (i4Pitch*((int32)u4ScreenHeight-1));
			pu2Raster = pu2Raster + u4ScreenWidth-1;
			i4Pitch = - i4Pitch;
		}
	}

	//
	// The primitive just calculates the fog colour at the start and end of the scan line. (This
	// is done in an identical method to the perspective function). 
	// This function does nothing else, once it has the start and end fog colours it goes into
	// the main loop.
	//
	while ( (pv3_start->tZ>fSKY_HORIZON_Z)  && (u4_lines<u4ScreenHeight))
	{
		// calculate the fog colour at the start of the scan line, fog calculations are only
		// done once per scan line.
		// The start vector is definiatly on the screen so we have no need to check, this is
		// assured because we start drawing at the closest point. Leave the fixed point bits
		// in with the fog colour.
		i4_fog_s = ((1.0 - fabs(pv3_start->tZ) - fFogNear) * fFogScale);

		// clamp the fog bands to the definded range...
		if (i4_fog_s>(iNUM_SKY_FOG_BANDS-1)*(1<<u4DEFAULT_SKY_FOG_BITS))
			i4_fog_s = (iNUM_SKY_FOG_BANDS-1)*(1<<u4DEFAULT_SKY_FOG_BITS);
		if (i4_fog_s<0)
			i4_fog_s = 0;

		// if the end of the scan line points towards the sky then the fog can be calculated
		// the same as the above the the scan line length can be set to the screen width as we
		// are going to fill the whole line.
		if (pv3_end->tZ>fSKY_HORIZON_Z)
		{
			// end fog colour
			i4_fog_e = (int32)((1.0 - fabs(pv3_end->tZ) - fFogNear) * fFogScale);

			// clamp to range.
			if (i4_fog_e> (iNUM_SKY_FOG_BANDS-1)*(1<<u4DEFAULT_SKY_FOG_BITS))
				i4_fog_e = (iNUM_SKY_FOG_BANDS-1)*(1<<u4DEFAULT_SKY_FOG_BITS);
			if (i4_fog_e<0)
				i4_fog_e = 0;

			// calculate the fog band increment per pixel along the scan line
			i4_fog_inc = (int32) ((float)(i4_fog_e - i4_fog_s) * fScreenWidthRep);

			// number of pixels to draw is the whole scan line
			i4_pixels = (int32)u4ScreenWidth;
		}
		else
		{
			//
			// If the end of the scan line is not pointing towrds the sky we need to find the horizon.
			// after finding the paramterix horizon we calculate how many pixles are to be drawn and
			// assume the fog colour at this point is maximum.
			f_pixels = (float)u4ScreenWidth * (fSKY_HORIZON_Z-pv3_start->tZ)/(pv3_end->tZ - pv3_start->tZ);
			i4_fog_inc = (int32)(((float)(((iNUM_SKY_FOG_BANDS-1) * (1<<u4DEFAULT_SKY_FOG_BITS)) - i4_fog_s)) / f_pixels);
			i4_pixels = (int32)f_pixels;
		}

		// make a copy of the scan line address
		pu2_pixel = pu2Raster;


		//
		// Main drawing loop
		//
		// draw pixels of this scan line (i4_pixels) in one go.
		//
		while (i4_pixels)
		{
			// The last colour in the map is the flat shaded colour
			*pu2_pixel = u2FogTable[i4_fog_s>>u4DEFAULT_SKY_FOG_BITS][u4FlatColourIndex];
			pu2_pixel+=i4_dir;

			// new fog colour
			i4_fog_s += i4_fog_inc;

			i4_pixels -= 1;
		}

		// calculate the edge vectors for the next scan line
		*pv3_start	+= v3_delta_start;
		*pv3_end	+= v3_delta_end;

		// move to the next scan line
		pu2Raster = pu2Raster + i4Pitch;

		// we have done a scan line..
		u4_lines++;
	}
}



//**********************************************************************************************
// Draws a non horizontal sky. To do this perspective correction is requird if we are going to
// draw a scan line at a time. This will draw a horizontal sky but is not efficient and the above
// routine should be used.
// In operation this is similar to the linear routine but instead of casting out 1 ray per scan
// line it casts out many, 1 every X pixels. This forms a grid when done on every scan line and
// the perspective is correct at the grid points so accraute UV co-ords can be calculated. Linear
// Interpolation is used on a per pixel basis between the grid points.
// An alternate method would be to draw down the lines of constant Z but it is difficult to stop
// cracking.
//
void CSkyRender::DrawSkyTexturedPerspective()
//
//**************************************
{
	CVector3<>	v3_delta_start;
	CVector3<>	v3_delta_end;
	CVector3<>*	pv3_start;
	CVector3<>*	pv3_end;
	CVector3<>	v3_step_delta;
	CVector3<>	v3_step;
	CVector3<>	v3_next_step;

	float		f_clipl;
	float		f_clipr;

	int32		i4_fog_e;

	uint32		fp_u;
	uint32		fp_v;

	int32		i4_x_limit;
	int32		i4_x_limit1;
	int32		i4_x_limit2;
	int32		i4_pixels;
	float		f_pixels;

	uint32		u4_lines = 0;

#if (VER_ASM)
	static void*	pvFogPointer;
	static uint8*	pu1_tex;
	static uint16*	pu2_pixel;
	static int32	i4_fog_s;
	static int32	i4_fog_inc;
	static uint32	fp_delta_u;
	static uint32	fp_delta_v;
	static int32	i4_dir;
#else
	uint8*		pu1_tex;
	uint16*		pu2_pixel;
	int32		i4_fog_s;
	int32		i4_fog_inc;
	uint32		fp_delta_u;
	uint32		fp_delta_v;
	int32		i4_dir;
	int32		i4_x;
	uint32		u4_adr;
#endif

	pu1_tex = (uint8*)prasSkyTexture->pSurface;

	//
	// fast fill the whole screen if the fill flag is switched on. This is quicker than
	// trying to fill just the pixels that the sky does not cover.
	//
	if (bFill)
	{
		FillSky
		(
			pu2Raster,
			u4ScreenWidth,
			(i4Pitch-(int32)u4ScreenWidth)*2,
			u4ScreenHeight,
			u4Fog
		);
	}


	//
	// Determine which of the corners of the screen is closest to the sky. This is done
	// by checking the sign of the up and right vectors. The drawing direction is always
	// away from the closest point.
	//

	if (av3FrustTran[4].tZ>0.0f)
	{
		//
		// Top vector is facing up (+ve Z) so the screen is the correct way around..
		//
		if (av3FrustTran[5].tZ<=0.0f)
		{
			// Right vector Z is 0 or less so we are drawing in the normal direction
			v3_delta_start	= (av3FrustTran[2] - av3FrustTran[0]) * fScreenHeightRep;
			v3_delta_end	= (av3FrustTran[3] - av3FrustTran[1]) * fScreenHeightRep;

			pv3_start	= &av3FrustTran[0];
			pv3_end		= &av3FrustTran[1];

			// Draw direction is from left to right. top to bottom
			i4_dir			= 1;
		}
		else
		{
			// Right vector X is begative so we draw from the right
			v3_delta_start	= (av3FrustTran[3] - av3FrustTran[1]) * fScreenHeightRep;
			v3_delta_end	= (av3FrustTran[2] - av3FrustTran[0]) * fScreenHeightRep;

			pv3_start		= &av3FrustTran[1];
			pv3_end			= &av3FrustTran[0];

			// Draw direction is from right to left, top to bottom
			i4_dir		= -1;
			pu2Raster = pu2Raster + u4ScreenWidth-1;
		}
	}
	else
	{
		//
		// The top vector is facing down so the screen is upside down
		//
		if (av3FrustTran[5].tZ<=0.0f)
		{
			// Right vector Z is 0 or less so we are drawing in the normal direction
			v3_delta_start	= (av3FrustTran[0] - av3FrustTran[2]) * fScreenHeightRep;
			v3_delta_end	= (av3FrustTran[1] - av3FrustTran[3]) * fScreenHeightRep;

			pv3_start		= &av3FrustTran[2];
			pv3_end			= &av3FrustTran[3];

			// draw direction is from left to right, bottom to top
			i4_dir		= 1;
			pu2Raster = pu2Raster + (i4Pitch*((int32)u4ScreenHeight-1));
			i4Pitch = - i4Pitch;
		}
		else
		{
			// Right vector X is begative so we draw from the right
			v3_delta_start	= (av3FrustTran[1] - av3FrustTran[3]) * fScreenHeightRep;
			v3_delta_end	= (av3FrustTran[0] - av3FrustTran[2]) * fScreenHeightRep;

			pv3_start		= &av3FrustTran[3];
			pv3_end			= &av3FrustTran[2];

			// draw direction is from right to left, bottom to top
			i4_dir		= -1;
			pu2Raster = pu2Raster + (i4Pitch*((int32)u4ScreenHeight-1));
			pu2Raster = pu2Raster + u4ScreenWidth-1;
			i4Pitch = - i4Pitch;
		}
	}

	//
	// pv3_start and pv3_end point to the start and end vectors for the scan lines.
	// v3_delta_start and v3_delta_end is what we adjust these vectors by on each scan line.
	// 
	// No we have the start,end and delta vectors we are ready to start drawing the scan lines
	//
	while ( (pv3_start->tZ>fSKY_HORIZON_Z)  && (u4_lines<u4ScreenHeight))
	{
		//
		// First we need a step delta vector for this scan line, This will step from the start
		// to the end in x increments based on the division length. This vector is applied to
		// the current position at each subdivision.
		//
		v3_step_delta = ((*pv3_end) - (*pv3_start)) * fDivisionsPerScanRep;

		// v3_step is the current position and originally is the start position.
		v3_step = *pv3_start;

		// v3_next_step is the next sub division which is were the linear run of pixels is
		// going to head for.
		v3_next_step = *pv3_start + v3_step_delta;

		// Calculate 2 parametric clip values, 1 for the each of the two current positions.
		// from these the UV co-ords in the sky map will be calculated.
		f_clipl = (fSkyHeight / v3_step.tZ);
		f_clipr = (fSkyHeight / v3_next_step.tZ);

		// calculate the fog colour at the start of the scan line, fog calculations are only
		// done once per scan line.
		// The start vector is definiatly on the screen so we have no need to check, this is
		// assured because we start drawing at the closest point. Leave the fixed point bits
		// in with the fog colour.
		i4_fog_s = ((1.0 - fabs(pv3_start->tZ) - fFogNear) * fFogScale);

		// clamp the fog bands to the definded range...
		if (i4_fog_s>(iNUM_SKY_FOG_BANDS-1)*(1<<u4DEFAULT_SKY_FOG_BITS))
			i4_fog_s = (iNUM_SKY_FOG_BANDS-1)*(1<<u4DEFAULT_SKY_FOG_BITS);
		if (i4_fog_s<0)
			i4_fog_s = 0;

		// if the end of the scan line points towards the sky then the fog can be calculated
		// the same as the above the the scan line length can be set to the screen width as we
		// are going to fill the whole line.
		if (pv3_end->tZ>fSKY_HORIZON_Z)
		{
			// end fog colour
			i4_fog_e = (int32)((1.0 - fabs(pv3_end->tZ) - fFogNear) * fFogScale);

			// clamp to range.
			if (i4_fog_e> (iNUM_SKY_FOG_BANDS-1)*(1<<u4DEFAULT_SKY_FOG_BITS))
				i4_fog_e = (iNUM_SKY_FOG_BANDS-1)*(1<<u4DEFAULT_SKY_FOG_BITS);
			if (i4_fog_e<0)
				i4_fog_e = 0;

			// calculate the fog band increment per pixel along the scan line
			i4_fog_inc = (int32) ((float)(i4_fog_e - i4_fog_s) * fScreenWidthRep);

			// number of pixels to draw is the whole scan line
			i4_pixels = (int32)u4ScreenWidth;
		}
		else
		{
			//
			// If the end of the scan line is not pointing towrds the sky we need to find the horizon.
			// after finding the paramterix horizon we calculate how many pixles are to be drawn and
			// assume the fog colour at this point is maximum.
			f_pixels = (float)u4ScreenWidth * (fSKY_HORIZON_Z-pv3_start->tZ)/(pv3_end->tZ - pv3_start->tZ);
			i4_fog_inc = (int32)(((float)(((iNUM_SKY_FOG_BANDS-1) * (1<<u4DEFAULT_SKY_FOG_BITS)) - i4_fog_s)) / f_pixels);
			i4_pixels = (int32)f_pixels;
		}

		// make a copy of the scan line address
		pu2_pixel = pu2Raster;

		// number of pixels per divide and the half values.
		i4_x_limit = u4DivisionLength;
		i4_x_limit1 = i4_x_limit2 = (i4_x_limit>>1);

		// while there are pixels left in this division
		while (i4_pixels>=i4_x_limit)
		{
			// calculate the UV co-ord at the start division position
			fp_u = (uint32) ( fSkyOffsetU + ((v3Camera.tX+(v3_step.tX*f_clipl))*fScale) );
			fp_v = (uint32) ( fSkyOffsetV + ((v3Camera.tY+(v3_step.tY*f_clipl))*fScale) );

			// calculate the UV per pixel delta based on the end division poisiton
			fp_delta_u = (uint32) (( ((v3_next_step.tX*f_clipr) - (v3_step.tX*f_clipl)) ) * fDivisionLengthRepScale);
			fp_delta_v = (uint32) (( ((v3_next_step.tY*f_clipr) - (v3_step.tY*f_clipl)) ) * fDivisionLengthRepScale);

			// the new start division is the previous end,
			v3_step = v3_next_step;

			// the next end division is the previous + the delta.
			v3_next_step += v3_step_delta;

#if (VER_ASM)
			//Assert(iNUM_SKY_COLOURS == 64);

			typedef CSkyRender myCSkyRender;

			__asm
			{
			mov		ecx,this						// Load object pointer.

			fld		[ecx]myCSkyRender.fSkyHeight	// Load sky height. 
			fdiv	float ptr [v3_step.tZ]			// fSkyHeight / (pv3_left->tZ)
			// Overlap division with scanline.

			//
			// Per 1/2 span loop.
			//
			lea		esi,[ecx]myCSkyRender.u2FogTable	// Pointer to fog table.
			mov		eax,[i4_dir]						// Load scan direction.

			push	ebp							// Save ebp.
			mov		[pvFogPointer],esi			// Global fog pointer.

			mov		ecx,[pu2_pixel]				// Pointer to destination.
			mov		edi,[i4_x_limit1]			// Load width of span.

			cmp		eax,0
			jl		short NEG_SCAN1

			// Scanning left to right.
			neg		edi							// Negate width.

NEG_SCAN1:
			sub		ecx,edi						// Start - negative width.
			mov		eax,[fp_u]					// Load fixed point U.

			sub		ecx,edi						// Start - negative width*2.
			mov		ebx,[fp_v]					// Load fixed point V.

			mov		edx,[i4_fog_s]				// Load fog value.
			mov		[pu2_pixel],ecx				// Save pixel after last pixel.

X_LOOP1:
			mov		ebp,eax						// get copy of u
			mov		ecx,ebx						// get copy of v

			sar		ebp,16						// integral u
			mov		esi,[pu1_tex]				// load base of texture.

			sar		ecx,(16 - 9)				// integral v
			and		ebp,0xff					// mask u for tiling

			and		ecx,(0xff << 9)				// mask v for tiling
			add		ebp,esi						// add texture base to u.

			add		ebp,ecx						// Add U and V.
			mov		ecx,edx						// Copy fog value.

			sar		ecx,u4DEFAULT_SKY_FOG_BITS	// Shift off fractional portion.

			shl		ecx,8						// Shift for width of table.
			mov		esi,[pvFogPointer]			// Load base of fog table.

			add		esi,ecx						// Add base of fog table.
			xor		ecx,ecx						// Zero ecx.

			mov		cl,[ebp]					// fetch pixel
			mov		ebp,[pu2_pixel]				// base of end of line.

			add		eax,[fp_delta_u]			// Step U.
			add		ebx,[fp_delta_v]			// Step V.

			mov		cx,[esi + ecx*2]			// Lookup fogged pixel.

			add		edx,[i4_fog_inc]			// Step fog value.
			mov		esi,[i4_dir]				// Load step direction.

			mov		[ebp + edi*2],cx			// Store pixel.

			add		edi,esi						// Step destination.
			jnz		short X_LOOP1

			mov		[i4_fog_s],edx				// Save updated fog value.
			pop		ebp							// Restore ebp.

			fstp	[f_clipl]					// Store f_clipl

			mov		ecx,this						// Load object pointer.

			fld		[ecx]myCSkyRender.fSkyHeight	// Load sky height. 
			fdiv	float ptr [v3_next_step.tZ]		// fSkyHeight / (pv3_left->tZ)
			// Overlap division with scanline.

			//
			// Per 1/2 span loop.
			//
			// eax = fp_u
			// ebx = fp_v
			// edx = i4_fog_s
			//
			mov		esi,[i4_dir]				// Load scan direction.
			push	ebp							// Save ebp.

			mov		ecx,[pu2_pixel]				// Pointer to destination.
			mov		edi,[i4_x_limit2]			// Load width of span.

			cmp		esi,0
			jl		short NEG_SCAN2

			// S canning left to right.
			neg		edi							// Negate width.

NEG_SCAN2:
			sub		ecx,edi						// Start - negative width.

			sub		ecx,edi						// Start - negative width*2.
			mov		[pu2_pixel],ecx				// Save pixel after last pixel.

X_LOOP2:
			mov		ebp,eax						// get copy of u
			mov		ecx,ebx						// get copy of v

			sar		ebp,16						// integral u
			mov		esi,[pu1_tex]				// load base of texture.

			sar		ecx,(16 - 9)				// integral v
			and		ebp,0xff					// mask u for tiling

			and		ecx,(0xff << 9)				// mask v for tiling
			add		ebp,esi						// add texture base to u.

			add		ebp,ecx						// Add U and V.
			mov		ecx,edx						// Copy fog value.

			sar		ecx,u4DEFAULT_SKY_FOG_BITS	// Shift off fractional portion.

			shl		ecx,8						// Shift for width of table.
			mov		esi,[pvFogPointer]			// Load base of fog table.

			add		esi,ecx						// Add base of fog table.
			xor		ecx,ecx						// Zero ecx.

			mov		cl,[ebp]					// fetch pixel
			mov		ebp,[pu2_pixel]				// base of end of line.

			add		eax,[fp_delta_u]			// Step U.
			add		ebx,[fp_delta_v]			// Step V.

			mov		cx,[esi + ecx*2]			// Lookup fogged pixel.

			add		edx,[i4_fog_inc]			// Step fog value.
			mov		esi,[i4_dir]				// Load step direction.

			mov		[ebp + edi*2],cx			// Store pixel.

			add		edi,esi						// Step destination.
			jnz		short X_LOOP2

			pop		ebp							// Restore ebp.
			mov		[i4_fog_s],edx				// Save updated fog value.

			fstp	[f_clipr]					// Store f_clipr
			}
#else
			// start the division for the next start division clip point
			f_clipl = fSkyHeight / v3_step.tZ;

			// draw just half of the pixels of this run while that divide is going.
			for (i4_x = 0; i4_x<i4_x_limit1; i4_x++)
			{
				// linear address from UV
				u4_adr = (((fp_v>>16) & 0xff)<<9) | ((fp_u>>16) & 0xff);

				// put pixel on screen and adjust screen pointer
				*pu2_pixel = u2FogTable[i4_fog_s>>u4DEFAULT_SKY_FOG_BITS][ pu1_tex[u4_adr] ];
				pu2_pixel+=i4_dir;

				// new UV
				fp_u += fp_delta_u;
				fp_v += fp_delta_v;

				// new fog colour
				i4_fog_s += i4_fog_inc;
			}

			// start the second divide for the end division
			f_clipr = fSkyHeight / v3_next_step.tZ;

			// do the second half of the pixels, this loop is identical to the above
			for (i4_x = 0; i4_x<i4_x_limit2; i4_x++)
			{
				u4_adr = (((fp_v>>16) & 0xff)<<9) | ((fp_u>>16) & 0xff);

				*pu2_pixel = u2FogTable[i4_fog_s>>u4DEFAULT_SKY_FOG_BITS][ pu1_tex[u4_adr] ];
				pu2_pixel+=i4_dir;

				fp_u += fp_delta_u;
				fp_v += fp_delta_v;

				i4_fog_s += i4_fog_inc;
			}
#endif

			// this run is finished, decrement the pixel count and go again.
			i4_pixels -= i4_x_limit;
		}

		// at this point we have the remainder of the pixels that did not fit into a whole number
		// of divisions, or 0.
		if (i4_pixels>0)
		{
			// This loop is identical to the above loops but no next division is calculated and
			// there is only 1 loop.
			fp_u = (uint32) ( fSkyOffsetU + ((v3Camera.tX+(v3_step.tX*f_clipl))*fScale) );
			fp_v = (uint32) ( fSkyOffsetV + ((v3Camera.tY+(v3_step.tY*f_clipl))*fScale) );

			fp_delta_u = (uint32) (( ((v3_next_step.tX*f_clipr) - (v3_step.tX*f_clipl)) ) * fDivisionLengthRepScale);
			fp_delta_v = (uint32) (( ((v3_next_step.tY*f_clipr) - (v3_step.tY*f_clipl)) ) * fDivisionLengthRepScale);

#if (VER_ASM)
			typedef CSkyRender myCSkyRender;

			__asm
			{
			mov		ecx,this

			//
			// Span loop.
			//
			lea		esi,[ecx]myCSkyRender.u2FogTable	// Pointer to fog table.
			mov		eax,[i4_dir]						// Load scan direction.

			push	ebp							// Save ebp.
			mov		[pvFogPointer],esi			// Global fog pointer.

			mov		ecx,[pu2_pixel]				// Pointer to destination.
			mov		edi,[i4_pixels]				// Load width of span.

			cmp		eax,0
			jl		short NEG_SCAN3

			// Scanning left to right.
			neg		edi							// Negate width.

NEG_SCAN3:
			sub		ecx,edi						// Start - negative width.
			mov		eax,[fp_u]					// Load fixed point U.

			sub		ecx,edi						// Start - negative width*2.
			mov		ebx,[fp_v]					// Load fixed point V.

			mov		edx,[i4_fog_s]				// Load fog value.
			mov		[pu2_pixel],ecx				// Save pixel after last pixel.

X_LOOP3:
			mov		ebp,eax						// get copy of u
			mov		ecx,ebx						// get copy of v

			sar		ebp,16						// integral u
			mov		esi,[pu1_tex]				// load base of texture.

			sar		ecx,(16 - 9)				// integral v
			and		ebp,0xff					// mask u for tiling

			and		ecx,(0xff << 9)				// mask v for tiling
			add		ebp,esi						// add texture base to u.

			add		ebp,ecx						// Add U and V.
			mov		ecx,edx						// Copy fog value.

			sar		ecx,u4DEFAULT_SKY_FOG_BITS	// Shift off fractional portion.

			shl		ecx,8						// Shift for width of table.
			mov		esi,[pvFogPointer]			// Load base of fog table.

			add		esi,ecx						// Add base of fog table.
			xor		ecx,ecx						// Zero ecx.

			mov		cl,[ebp]					// fetch pixel
			mov		ebp,[pu2_pixel]				// base of end of line.

			add		eax,[fp_delta_u]			// Step U.
			add		ebx,[fp_delta_v]			// Step V.

			mov		cx,[esi + ecx*2]			// Lookup fogged pixel.

			add		edx,[i4_fog_inc]			// Step fog value.
			mov		esi,[i4_dir]				// Load step direction.

			mov		[ebp + edi*2],cx			// Store pixel.

			add		edi,esi						// Step destination.
			jnz		short X_LOOP3

			mov		[i4_fog_s],edx				// Save updated fog value.
			pop		ebp							// Restore ebp.
			}
#else		
			for (i4_x = 0; i4_x<i4_pixels; i4_x++)
			{
				u4_adr = (((fp_v>>16) & 0xff)<<9) | ((fp_u>>16) & 0xff);

				*pu2_pixel = u2FogTable[i4_fog_s>>u4DEFAULT_SKY_FOG_BITS][ pu1_tex[u4_adr] ];
				pu2_pixel += i4_dir;

				fp_u += fp_delta_u;
				fp_v += fp_delta_v;
				
				i4_fog_s += i4_fog_inc;
			}
#endif
		}

		// calculate the edge vectors for the next scan line
		*pv3_start	+= v3_delta_start;
		*pv3_end	+= v3_delta_end;

		pu2Raster = pu2Raster + i4Pitch;

		// we have done a scan line..
		u4_lines++;
	}
}



//**********************************************************************************************
// Fast fill a scan line with the given 32bit value. This code is generally optimal and will
// function on all processors. No processor specific version is required.
//
void CSkyRender::FillSky
//
//**************************************
(
	uint16* pu2_dst,			// pointer to the dst surface
	int32 i4_width,				// width in pixels to store
	int32 i4_pitch_adj,			// value to add to pointer at end of scan line to get tp the next
	uint32 u4_lines,			// number of lines to do
	uint32	u4_col				// colour packed into a DWORD
)
{
	Assert (u4_lines>0);
	Assert (i4_width>0);
	Assert ( (((uint32)pu2_dst) & 1) == 0 );	// address must be 16 bit aligned

	_asm
	{
			mov		edi,pu2_dst
			mov		eax,i4_width
			mov		ebx,u4_col
			mov		esi,eax
			mov		ecx,u4_lines
			mov		edx,i4_pitch_adj

LOOPY:
			// align to a 64 bit boundardry
ALIGN_64:
			test	edi,7				// are we aligned to a 64 bit (8 byte) boundary??
			jz		short LOOPX
			mov		[edi],bx
			add		edi,2
			dec		eax
			jnz		short ALIGN_64
			jmp		short DONE

LOOPX:
			sub		eax,8				// can we do 8 pixels (16 bytes)
			jl		FINISH_X

			mov		[edi],ebx
			mov		[edi+4],ebx
			mov		[edi+8],ebx
			mov		[edi+12],ebx
			add		edi,16
			jmp		short LOOPX	

FINISH_X:
			add		eax,8
			jz		short NEXT_LINE
LOOP_FINISH_X:
			mov		[edi],bx
			add		edi,2
			dec		eax
			jnz		short LOOP_FINISH_X

NEXT_LINE:
			add		edi,edx
			mov		eax,esi
			dec		ecx
			jnz		short LOOPY
DONE:
	}
}



//**********************************************************************************************
//
// Function returns the four corners of the far clipping plane's intersection with the
// view frustum. These vectors are calculated as direction vectors in the array CSkyRender::av3FrustTran. 
// They are returned as CVector's and not CDir's because operations need to be performed on them,
//
// The world position of the camera is returned in 'CSkyRender::v3Camera'
// 
void CSkyRender::SetTransformedCameraCorners()
//
//**************************************
{
	// Query the world database to get the current active camera.
	CWDbQueryActiveCamera wqcam(wWorld);
	CCamera* pcam = wqcam.tGet();
	Assert(pcam);

	// Construct an inverse transform for the camera.
	CTransform3<> tf3_invcam = ~pcam->tf3ToNormalisedCamera();

	// Set the position of the camera.
	v3Camera = pcam->pr3Presence().v3Pos;

	// Transform these points, base at the origin and normalize..
	for (uint u = 0; u < u4NUM_SKY_TRANSFORM_VERT; ++u)
	{
		av3FrustTran[u] = (av3Frustum[u] * tf3_invcam) - v3Camera;
		av3FrustTran[u].FastNormalise();
	}
}





//**********************************************************************************************
//
// Static helper functions to create and destroy the global sky
//


//**********************************************************************************************
//
void CSkyRender::CreateSky
(
	const CGroffObjectName*	pgon,		// Object to load.
	CLoadWorld*				pload,		// The loader.
	const CHandle&			h_object,	// handle of object in value table.
	CValueTable*			pvtable,	// Value Table.
	const CInfo*			pinfo		// The info to copy.  Create a new one if 0.
)
//
//**************************************
{
	// remove any existing sky.
	RemoveSky();

	int32 i4_shade_index = iNUM_SKY_COLOURS - 1;

	IF_SETUP_TEXT_PROCESSING(pvtable, pload)
	SETUP_OBJECT_HANDLE(h_object)
	{
#if (VER_TEST == TRUE)

	//
	// The sky must have no mip maps and be packed in the no pageable region of the texture
	// memory.
	//
		bool b_mips = true;
		bFILL_BOOL(b_mips, esMipMap);

		if (b_mips)
		{
			char str_buffer[1024];

			sprintf(str_buffer, "Cannot Load: The sky does not have the 'MipMap = false' text property. (%s)", __FILE__);

			if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
				DebuggerBreak();
		}
#endif
		bFILL_INT(i4_shade_index, esSkyFlatColour);
		Assert( i4_shade_index <= iNUM_SKY_COLOURS);
	}
	END_OBJECT;
	END_TEXT_PROCESSING;

	//
	// we must make a permenant reference to the texture before this function goes
	// out of scope and the mesh gets deleted.
	//

	// Load the mesh as a render type, then cast this to a CShape
	rptr<CRenderType>  prdt = CRenderType::prdtFindShared(pgon, pload, h_object, pvtable);
	rptr<CShape> psh = ptCastRenderType<CShape>(prdt);

	// Make a polygon iterator to go through the polygons of the mesh.
	// OK to use 0 for mesh instance, as it doesn't need the CInstance*.
	aptr<CShape::CPolyIterator> ppi = psh->pPolyIterator(0);
	Verify(ppi->bNext());

	//get the texture of the first polygon and use it for the sky.
	const CTexture* ptex = ppi->ptexTexture();

	// Create a new sky renderer class, the destination of which is the Main Screens raster
	gpskyRender = new CSkyRender(ptex->prasGetTexture(), rptr_cast(CRaster,prasMainScreen) );

	gpskyRender->SetFlatShadeColour(i4_shade_index);
}



//**********************************************************************************************
//
void CSkyRender::RemoveSky()
//
//**************************************
{
	delete gpskyRender;
	gpskyRender = NULL;
}

//******************************************************************************************
void CSkyRender::PurgeD3D()
{
	// Remove the existing D3D sky.
	prasD3DSky = rptr0;
}

//******************************************************************************************
void CSkyRender::InitializeForD3D()
{
	// Remove the existing D3D sky.
	PurgeD3D();

	// Do nothing if the software-only renderer is to be used.
	if (!bUseD3D)
		return;
	Assert (prasSkyTexture->iWidth == 256);
	Assert (prasSkyTexture->iHeight == 256);

	//
	// Create, copy and upload a sky raster.
	//
	AlwaysAssert(bWithin(prasSkyTexture->iWidth, 2, 256));
	AlwaysAssert(bWithin(prasSkyTexture->iHeight, 2, 256));
	#if bTRACK_D3D_RASTERS
		TrackSystem(etrSky);
	#endif
	prasD3DSky = rptr_cast(CRaster, rptr_new CRasterD3D(CRasterD3D::CInit
	(
		prasSkyTexture->iWidth,
		prasSkyTexture->iHeight,
		ed3dtexSCREEN_OPAQUE,
		true
	)));

	prasSkyTexture->Lock();
	prasD3DSky->Lock();
	for (int i_x = 0; i_x < prasSkyTexture->iWidth; ++i_x)
		for (int i_y = 0; i_y < prasSkyTexture->iHeight; ++i_y)
		{
			uint32 u4_pix = prasSkyTexture->pixGet(i_x, i_y);
			u4_pix = prasD3DSky->pixFromColour(prasSkyTexture->clrFromPixel(u4_pix));
			prasD3DSky->PutPixel(i_x, i_y, u4_pix);
		}
	prasD3DSky->Unlock();
	prasSkyTexture->Unlock();

	// Upload the D3D raster.
	Verify(prasD3DSky->bUpload());
}

//******************************************************************************************
//
void CSkyRender::FillD3D
(
	float f_offset_x,
	float f_offset_y
)
//
// Fills the Direct3D version of the sky.
//
//**************************************
{
	// Do nothing if no fill is required.
	if (!bFill)
		return;

	// Get the screen width and height.
	float f_screen_w = float(u4ScreenWidth);
	float f_screen_h = float(u4ScreenHeight);

	// Direct3D colour constants.
	D3DCOLOR d3drgb_plain  = d3dDriver.d3dcolGetFogColour();
	D3DCOLOR d3drgb_plaina = D3DRGBA(1, 1, 1, 1);

	// Setup Direct3D for this polygon.
	LPDIRECT3DDEVICE3 pdevice = d3dDriver.pGetDevice();	// Copy of the device pointer.
	Assert(pdevice);

	d3dDriver.err = pdevice->Begin
	(
		D3DPT_TRIANGLEFAN,
		D3DFVF_TLVERTEX,
		D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS
	);

	d3dDriver.err = pdevice->Vertex(&D3DTLVERTEX (D3DVECTOR(      0.0f + f_offset_x,       0.0f + f_offset_y, 1.0f), 0.5f, d3drgb_plain, d3drgb_plaina, 0.0f, 0.0f));
	d3dDriver.err = pdevice->Vertex(&D3DTLVERTEX (D3DVECTOR(f_screen_w + f_offset_x,       0.0f + f_offset_y, 1.0f), 0.5f, d3drgb_plain, d3drgb_plaina, 0.0f, 0.0f));
	d3dDriver.err = pdevice->Vertex(&D3DTLVERTEX (D3DVECTOR(f_screen_w + f_offset_x, f_screen_h + f_offset_y, 1.0f), 0.5f, d3drgb_plain, d3drgb_plaina, 0.0f, 0.0f));
	d3dDriver.err = pdevice->Vertex(&D3DTLVERTEX (D3DVECTOR(      0.0f + f_offset_x, f_screen_h + f_offset_y, 1.0f), 0.5f, d3drgb_plain, d3drgb_plaina, 0.0f, 0.0f));

	d3dDriver.err = pdevice->End(0);
}

//******************************************************************************************
//
void CSkyRender::DrawD3D
(
)
//
// Draws the Direct3D version of the sky.
//
//**************************************
{
	Assert(prasRenderSurface);

	float f_offset_x = 0.0f;
	float f_offset_y = 0.0f;
	LPDIRECT3DDEVICE3 pdevice = d3dDriver.pGetDevice();	// Copy of the device pointer.

	//
	// Initialize the sky for Direct3D if it has not already been initialized.
	//
	static bool b_initialized_sky = false;
	if (!b_initialized_sky)
	{
		InitializeForD3D();
		b_initialized_sky = true;
	}

	// Query the world database to get the current active camera.
	CWDbQueryActiveCamera wqcam(wWorld);
	CCamera* pcam = wqcam.tGet();
	Assert(pcam);

	// Make sure the screen is a CRasterWin.
	CRasterWin* prasw = dynamic_cast<CRasterWin*>(prasRenderSurface.ptGet());
	if (prasw)
	{
		f_offset_x = float(prasw->iOffsetX) + CScreenRenderAuxD3D::fOffsetXBase;
		f_offset_y = float(prasw->iOffsetY) + CScreenRenderAuxD3D::fOffsetYBase;
	}

	// Fill the sky with the fog colour if required.
	//FillD3D(f_offset_x, f_offset_y);

	const float fFarClipPlane = 50000.0f;
	const float fSkySize      = 1.0f / 10000.0f;
	float       f_screen_w    = float(u4ScreenWidth);
	float       f_screen_h    = float(u4ScreenHeight);
	float       f_cam_scale   = pcam->campropGetProperties().rFarClipPlaneDist / fFarClipPlane;

	SSkyClip skc[8];
	CVector3<> av3[u4NUM_SKY_TRANSFORM_VERT];
	uint u;

	// Create a world view frustum.
	av3[0] = av3FrustTran[u4FRUST_TL] * fFarClipPlane + v3Camera;
	av3[1] = av3FrustTran[u4FRUST_TR] * fFarClipPlane + v3Camera;
	av3[2] = av3FrustTran[u4FRUST_BR] * fFarClipPlane + v3Camera;
	av3[3] = av3FrustTran[u4FRUST_BL] * fFarClipPlane + v3Camera;

	// Initialize variables.
	skc[0].Set(&v3Camera, &av3[0], 0, 0);
	skc[1].Set(&av3[0],   &av3[1], f_screen_w, f_screen_h);
	skc[2].Set(&v3Camera, &av3[1], f_screen_w, 0);
	skc[3].Set(&av3[1],   &av3[2], f_screen_w, f_screen_h);
	skc[4].Set(&v3Camera, &av3[2], f_screen_w, f_screen_h);
	skc[5].Set(&av3[2],   &av3[3], f_screen_w, f_screen_h);
	skc[6].Set(&v3Camera, &av3[3], 0, f_screen_h);
	skc[7].Set(&av3[3],   &av3[0], f_screen_w, f_screen_h);

	// Generate outcodes.
	for (u = 0; u < 4; ++u)
		skc[u * 2].bValid = av3[u].tZ > fSkyHeight;
	skc[1].SetValid(&skc[0], &skc[2]);
	skc[3].SetValid(&skc[2], &skc[4]);
	skc[5].SetValid(&skc[4], &skc[6]);
	skc[7].SetValid(&skc[6], &skc[0]);

	// Count the outcodes, and only render if there are three or more.
	{
		int i_num_inview = 0;
		for (u = 0; u < 8; ++u)
			if (skc[u].bValid)
				++i_num_inview;
		if (i_num_inview < 3)
		{
			return;
		}
	}

	//
	// Find the worldspace point of intersection for every edge of the frustum that
	// intersects the sky.
	//
	for (u = 0; u < 8; ++u)
		if (skc[u].bValid)
			skc[u].SetIntersection(fSkyHeight, fSkySize);

	skc[1].SetScreenTop();
	skc[3].SetScreenRight();
	skc[5].SetScreenBottom();
	skc[7].SetScreenLeft();

	{
		// Construct an inverse transform for the camera.
		CTransform3<> tf3_invcam = pcam->tf3ToNormalisedCamera();

		// Transform these points, base at the origin and normalize..
		for (u = 0; u < 8; ++u)
		{
			CVector3<> v3 = skc[u].v3IntersectSky * tf3_invcam;
			skc[u].fZ = v3.tY;
		}
	}

	// Setup Direct3D for this polygon.
	srd3dRenderer.SetD3DMode(ed3drSOFTWARE_LOCK);
	srd3dRenderer.FlushBatch();
	d3dDriver.err = d3dDriver.pGetDevice()->BeginScene();
	d3dstState.SetDefault();
	d3dstState.SetFog(true);
	d3dstState.SetFiltering(true);
	d3dstState.SetTexture(prasD3DSky.ptGet());
	d3dstState.SetShading(true);
	d3dstState.SetAddressing(eamTileUV);

	// Turn Z buffering off.
	d3dstState.SetAllowZBuffer(false);

	// Set up the vertices and feed them to DrawPrimitive.
	int i_num_vertices = 0;
	D3DTLVERTEX atlv[8];

	for (u = 0; u < 8; ++u)
	{
		// Add a vertex.
		if (skc[u].bValid)
		{
			float f_z   = skc[u].fZ * f_cam_scale;
			float f_fog = 1.0f - f_z * 1.5f;
			SetMinMax(f_fog, 0.0f, 1.0f);
			D3DTLVERTEX* ptlv = &atlv[i_num_vertices];

			// Set the fog colour.
			D3DCOLOR d3d_fog = (uint32(f_fog * 255.0f) << 24);

			// Copy the data to the TLVertex.
			ptlv->sx       = skc[u].v2Screen.tX + f_offset_x;
			ptlv->sy       = skc[u].v2Screen.tY + f_offset_y;
			ptlv->sz       = fInvScale / f_z;
			ptlv->rhw      = ptlv->sz;
			ptlv->color    = d3drgbPlain;
			ptlv->specular = d3d_fog;
			ptlv->tu       = skc[u].v2UVSky.tX + fSkyOffsetU;
			ptlv->tv       = skc[u].v2UVSky.tY + fSkyOffsetV;

			++i_num_vertices;
		}
	}

	// Call DrawPrimitive.
	d3dDriver.err = pdevice->DrawPrimitive
	(
		D3DPT_TRIANGLEFAN,
		D3DFVF_TLVERTEX,
		(LPVOID)atlv,
		i_num_vertices,
		u4DrawPrimFlags
	);

	// Render the scene.
	d3dDriver.err = d3dDriver.pGetDevice()->EndScene();
}

//*****************************************************************************************
//
char* CSkyRender::pcSave
(
	char* pc
) const
//
// Saves the sky settings to a buffer.
//
//**************************************
{
	if (this)
	{
		// Save version number.
		pc = pcSaveT(pc, 1);

		pc = pcSaveT(pc, u4DivisionLength);
		pc = pcSaveT(pc, fPixelsPerMeter);
		pc = pcSaveT(pc, fFogFar);
		pc = pcSaveT(pc, fFogNear);
		pc = pcSaveT(pc, fSkyHeight);
		pc = pcSaveT(pc, fSkyOffsetU);
		pc = pcSaveT(pc, fSkyOffsetV);
		pc = pcSaveT(pc, fSkyOffsetdU);
		pc = pcSaveT(pc, fSkyOffsetdV);
		pc = pcSaveT(pc, true);				// No longer used.
		pc = pcSaveT(pc, bFill);
	}
	else
	{
		// No sky to save.
		pc = pcSaveT(pc, 0);
	}

	return pc;
}

//*****************************************************************************************
//
const char* CSkyRender::pcLoad
(
	const char* pc
)
//
// Loads the sky settings from a buffer.
//
//**************************************
{
	int iVersion;

	pc = pcLoadT(pc, &iVersion);

	if (iVersion == 1)
	{
		if (this)
		{
			pc = pcLoadT(pc, &u4DivisionLength);
			pc = pcLoadT(pc, &fPixelsPerMeter);
			pc = pcLoadT(pc, &fFogFar);
			pc = pcLoadT(pc, &fFogNear);
			pc = pcLoadT(pc, &fSkyHeight);
			pc = pcLoadT(pc, &fSkyOffsetU);
			pc = pcLoadT(pc, &fSkyOffsetV);
			pc = pcLoadT(pc, &fSkyOffsetdU);
			pc = pcLoadT(pc, &fSkyOffsetdV);

			// Ignore this settings, it is set by the global quality slider.
			bool b_textured;
			pc = pcLoadT(pc, &b_textured);

			pc = pcLoadT(pc, &bFill);

			// Update dependant values.
			SetWorkingConstants
			(
				fPixelsPerMeter,
				fSkyHeight,
				fFogNear,
				fFogFar,
				u4DivisionLength
			);
		}
		else
		{
			// Skip this data.
			pc += sizeof(uint32) + sizeof(float)* 8 + sizeof(bool) * 2;
		}
	}
	else if (iVersion != 0)
	{
		AlwaysAssert("Unknown version of sky settings");
	}

	return pc;
}
