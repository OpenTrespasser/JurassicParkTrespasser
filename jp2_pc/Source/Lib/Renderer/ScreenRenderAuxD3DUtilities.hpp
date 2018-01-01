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
 * $Log:: /JP2_PC/Source/Lib/Renderer/ScreenRenderAuxD3DUtilities.hpp                          $
 * 
 * 9     10/05/98 5:46a Pkeet
 * Tweaked z buffer.
 * 
 * 8     10/02/98 4:05p Pkeet
 * Made failure to allocate a D3D raster for a mip level more robust.
 * 
 * 7     9/24/98 12:30a Pkeet
 * Batch optimized fluid particles. Batched optimized fill particles.
 * 
 * 6     9/23/98 2:15a Pkeet
 * Optimized the 'd3dGetFog' function.
 * 
 * 5     9/22/98 10:40p Pkeet
 * Forced inline critical Direct3D functions.
 * 
 * 4     9/15/98 4:19p Pkeet
 * Reduced problem with objects flashing white after loading a new level.
 * 
 * 3     8/03/98 4:31p Pkeet
 * Changed the inverse scale value.
 * 
 * 2     8/02/98 4:50p Pkeet
 * Added a constant for flags.
 * 
 * 1     8/02/98 3:11p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef LIB_RENDERER_SCREENRENDERAUXD3DUTILITIES_HPP
#define LIB_RENDERER_SCREENRENDERAUXD3DUTILITIES_HPP

//
// Includes.
//
#include "Lib/W95/WinInclude.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Renderer/Texture.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "Fog.hpp"


//
// Useful constants.
//

// White Direct3D colour constants.
const D3DCOLOR d3drgbPlain = D3DRGB(1, 1, 1);

// Standard DrawPrimitive flags.
const uint32 u4DrawPrimFlags = D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTLIGHT;

// Value for sky and overlay z.
const float fInvScale = 0.00005f;

// Value to move polygons forward by when using the Z buffer.
const float fZBias       = 1.0025f;

// Maximum possible z value.
const float fMaxInvZValue = 0.99f;

const float fOneCompare  = 1.0f;
const float f255         = 255.0f;
const float fScaleFogD3D = 1.0f / 6.5f;

#define fGetScreenZ(X) ( (u4FromFloat(X->v3Screen.tZ) > u4FromFloat(fMaxInvZValue)) ? (fMaxInvZValue) : (X->v3Screen.tZ) )

// Alpha value for the least translucent vertices of the polygon.
const uint32 d3dcolFluidAlpha = 176 << 24;


//
// Inline functions.
//

//******************************************************************************************
//
forceinline D3DCOLOR d3dGetFog
(
	float f_fog	// Fog intensity between 0 and 1.
)
//
// Returns the fog intensity level in a form that can be use by Direct3D.
//
//**************************************
{
	f_fog  = (1.0f + 0.5f * fScaleFogD3D) - f_fog * fScaleFogD3D;

	if (u4FromFloat(f_fog) & 0x80000000)
		return 0x00000000;

	if (u4FromFloat(f_fog) >= u4FromFloat(fOneCompare))
		return 0xFF000000;

#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

	// Convert to Direct3D colour form.
	return D3DCOLOR(f_fog * 255.0f) << 24;

#else

	D3DCOLOR d3d_col;

	// Convert to Direct3D colour form.
	__asm
	{
		fld   [f_fog]
		fmul  [f255]
		fistp [d3d_col]
		mov   eax, [d3d_col]
		shl   eax, 24
		mov   [d3d_col], eax
	}
	return d3d_col;

#endif
}

//******************************************************************************************
//
forceinline bool bAvailableD3DRaster
(
	CTexture* ptex,
	int       i_desired_mip
)
//
// Returns 'true' if a linked valid D3D raster is available.
//
//**********************************
{
	if (!ptex)
		return false;
	CRaster* pras = ptex->prasGetTexture(i_desired_mip).ptGet();
	if (!pras)
		return false;
	if (!pras->prasLink)
		return false;
	if (!pras->prasLink->pd3dtexGet())
	{
		pras->Link();
		return false;
	}
	return true;
}

//******************************************************************************************
//
forceinline CRaster* prasGetBestD3DRaster
(
	CRenderPolygon& rp
)
//
// Returns 'true' if a linked valid D3D raster is available.
//
//**********************************
{
	CTexture* ptex    = (CTexture*)rp.ptexTexture.ptGet();
	int i_desired_mip = rp.iMipLevel;

	// If no texture is available, no raster is available.
	if (!ptex)
		return 0;

	// If the miplevel is correct, do nothing more.
	if (bAvailableD3DRaster(ptex, i_desired_mip))
		return ptex->prasGetTexture(i_desired_mip)->prasLink.ptGet();

	// Search all mip levels for the largest available.
	for (i_desired_mip = 0; i_desired_mip < ptex->iGetNumMipLevels(); ++i_desired_mip)
		if (bAvailableD3DRaster(ptex, i_desired_mip))
		{
			// Set the mip level to the available raster.
			rp.iMipLevel = i_desired_mip;
			return ptex->prasGetTexture(i_desired_mip)->prasLink.ptGet();
		}

	// Search failed. Return a null pointer.
	return 0;
}

//******************************************************************************************
//
forceinline D3DCOLOR d3dcolModulate
(
	D3DCOLOR d3dcol_0,	// Colour to modulate.
	D3DCOLOR d3dcol_1	// Colour to modulate by.
)
//
// Returns a Direct3D colour modulated by another Direct3D colour.
//
// Notes:
//		Alpha is not modulated.
//
//**************************************
{
	// Red.
	uint32 u4_r0 = (d3dcol_0 & 0x00FF0000) >> 16;
	uint32 u4_r1 = (d3dcol_1 & 0x00FF0000) >> 16;
	uint32 u4_r  = (u4_r0 * u4_r1) >> 8;

	// Green.
	uint32 u4_g0 = d3dcol_0 & 0x0000FF00;
	uint32 u4_g1 = d3dcol_1 & 0x0000FF00;
	uint32 u4_g  = ((u4_g0 * u4_g1) >> 16) & 0x0000FF00;

	// Blue.
	uint32 u4_b0 = d3dcol_0 & 0x000000FF;
	uint32 u4_b1 = d3dcol_1 & 0x000000FF;
	uint32 u4_b  = (u4_b0 * u4_b1) >> 8;

	// Return the recomposited colours.
	return (u4_r << 16) | (u4_g) | (u4_b);
}


#endif LIB_RENDERER_SCREENRENDERAUXD3DUTILITIES_HPP