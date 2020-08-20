/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of RasterVid.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/W95/RasterVid.cpp                                            $
 * 
 * 125   10/03/98 4:54a Pkeet
 * Fixed possible lurking bug.
 * 
 * 124   10/01/98 1:28a Pkeet
 * Moved terrain and cache texture allocation from the raster constructor to the general purpose
 * create member function.
 * 
 * 123   10/01/98 12:24a Pkeet
 * Terrain and cache memory are automatically set.
 * 
 * 122   9/30/98 5:55p Pkeet
 * Added default memory initialization for caches and terrain if Direct3D initialization fails.
 * 
 * 121   9/19/98 9:16p Pkeet
 * Constructor now sets the 'bFullScreen' flag before constructing a D3D compatible raster.
 * 
 * 120   98/09/17 16:42 Speter
 * Update due to DebugConsole.hpp.
 * 
 * 119   9/16/98 2:12p Pkeet
 * Fixed crash bug on a SiS motherboard/video card combination.
 * 
 * 118   9/11/98 3:29p Pkeet
 * Disabled crash bug when switching resolutions.
 * 
 * 117   9/10/98 8:53p Pkeet
 * Fixed GUIApp bug.
 * 
 * 116   9/10/98 4:55p Pkeet
 * DirectDraw is reinitialized in the 'CRasterWin' destructor.
 * 
 * 115   9/10/98 2:54p Shernd
 * Added flag to ClearBorder to use back buffer
 * 
 * 114   9/10/98 8:40a Shernd
 * allowing clear border to happen if the surface if flippable.
 * 
 * 113   9/09/98 7:41p Pkeet
 * Added functions to show that the computer is working while it loads Direct3D surfaces.
 * 
 * 112   9/09/98 6:08p Pkeet
 * Fixed problem going into hardware mode.
 * 
 * 111   98.09.07 5:23p Mmouni
 * GammaFlash is now disabled on Permedia 2 cards.
 * 
 * 110   9/07/98 4:55p Pkeet
 * Fixed bug clearing the background with a viewport smaller than the screen dimensions.
 * 
 * 109   98.09.02 10:03p Mmouni
 * Now checks the return value of GetGammaRamp() just to be extra safe.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "../RasterVid.hpp"
#include "Lib/W95/Dd.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/FileEx.hpp"
#include "Lib/Sys/debugConsole.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Sys/RegInit.hpp"
#include "Lib/Sys/DWSizeStruct.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"
#include "Lib/W95/Direct3DCards.hpp"
#include "Lib/View/DisplayMode.hpp"

//
// D3D inclusion is needed in order to set certain flags on structures.
// DD Surfaces need the DDSD_3DDEVICE flag set if they are to be D3D rendering surfaces
// (but must *not* be set if D3D isn't installed!).
// And unused palette entries must have the D3DPAL_RESERVED flag set.
//
#include <d3d.h>

#include <string.h>
// Include stdio for prasReadBMP only.
#include <stdio.h>
#include <crtdbg.h>

//
// Flags.
//

// Use DirectDraw clears only.
#define bUSE_DD_CLEARS_ONLY (1)


#define u4BORDER_COLOUR		0x00000000

extern  bool bIsTrespasser;

//**********************************************************************************************
//
static HRESULT WINAPI EnumZBufferFormats
(
	DDPIXELFORMAT* pddpf,
	VOID*          pddpfDesired
)
//
// Enumeration function to find Direct3D compatible Z buffer formats.
//
// Notes:
//		The structure pointed to by 'pddpfDesired' is filled with the pixel format for a
//		z buffer.
//
//**************************************
{
	DDPIXELFORMAT* pddpf_desired = ((DDPIXELFORMAT*)pddpfDesired);
	if (!pddpf || !pddpf_desired)
		return D3DENUMRET_CANCEL;
	
	// Copy the best available format.
	if (pddpf->dwFlags == pddpf_desired->dwFlags)
		if (pddpf->dwZBufferBitDepth > pddpf_desired->dwZBufferBitDepth)
			memcpy(pddpfDesired, pddpf, sizeof(DDPIXELFORMAT));
	return D3DENUMRET_OK;
}


//
// Private class definitions.
//

//**********************************************************************************************
//
class CRasterWin::CPriv: public CRasterWin 
//
// Private member functions for 'CRasterWin.'
//
//**************************************
{
private:

	//******************************************************************************************
	//
	void DestroyDD4Surfaces
	(
	)
	//
	// Invalidates all DirectDraw4 surfaces.
	//
	//**************************************
	{
		pddsZBuffer.SafeRelease();
		pddsDraw4.SafeRelease();
		pddsPrimary4.SafeRelease();
	}

	//******************************************************************************************
	//
	bool bConstructSoftwareVidRam
	(
		int i_width,	// The desired dimensions of the raster.
		int i_height,
		int i_bits,		// How many bits deep for full-screen, 0 for windowed.
		int i_buffers	// How many buffers to construct.
	)
	//
	// Creates a screen raster for use with the software rasterizer in video memory.
	//
	//**************************************
	{
		HRESULT hres;
		CDDSize<DDSURFACEDESC2> sd;

		//
		// We want a flipping (double-buffered) surface in video memory.
		// Note: We add the DDSCAPS_VIDEOMEMORY flag, because if the flipping
		// surface isn't in video mem, we don't want it.
		//
		sd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY |
					        DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		sd.dwBackBufferCount = i_buffers-1;

		// Create the front buffer.
		hres = DirectDraw::pdd4->CreateSurface(&sd, &pddsPrimary4, 0);

		//
		// If the front buffer could not be created, fail trying to construct a video raster
		// with DirectDraw hardware.
		//
        if (FAILED(hres) || !pddsPrimary4)
        {
            return false;
        }

		// Retrieve back buffer.
		sd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
		hres = pddsPrimary4->GetAttachedSurface(&sd.ddsCaps, &pddsDraw4);

		if (FAILED(hres) || !pddsDraw4)
		{
			// Failed to create a flipping chain, try to create a double buffer.
			sd.dwBackBufferCount = 1;
			sd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
			hres = pddsPrimary4->GetAttachedSurface(&sd.ddsCaps, &pddsDraw4);
			if (FAILED(hres) || !pddsDraw4)
			{
				pddsPrimary4.SafeRelease();
				return false;
			}
		}

		// Clear all the new surfaces.
		for (uint u = 0; u <= sd.dwBackBufferCount; u++)
		{
			CDDSize<DDBLTFX> ddbltfx;
			ddbltfx.dwFillColor = u4BORDER_COLOUR;

			// Clear the next backbuffer.
			while (bRestore(pddsDraw4->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx)));
			
			// Flip to the next in the chain.
			while (bRestore(pddsPrimary4->Flip(pddsDraw4, DDFLIP_WAIT)));
		}

		return true;
	}

	//******************************************************************************************
	//
	bool bConstructSoftwareSysRam
	(
		int i_width,				// The desired dimensions of the raster.
		int i_height,
		int i_bits,					// How many bits deep for full-screen, 0 for windowed.
		int i_buffers,				// How many buffers to construct.
		CSet<ERasterFlag> seteras	// Any special flags for the raster.
	)
	//
	// Creates a screen raster for use with the software rasterizer in video memory.
	//
	//**************************************
	{
		CDDSize<DDSURFACEDESC2> sd;

		sd.dwFlags = DDSD_CAPS;
		sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		DirectDraw::err = DirectDraw::pdd4->CreateSurface(&sd, &pddsPrimary4, 0);

		if (i_buffers > 1)
		{
			sd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;

			//
			// Round up width to ensure that stride is a whole number of pixels.  This will
			// not affect the raster's iWidth field, but will affect its iLinePixels.
			// This is only strictly necessary for 24-bit modes, but at this point, we don't
			// know which bit depth the screen is.  In full-screen mode, this should have no
			// effect, because the width will surely already be a multiple of 8.
			//
			sd.dwWidth	= RoundUp(i_width, 8);
			sd.dwHeight = i_height;
			sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

			if (!seteras[erasVideoMem])
				// Video mem not wanted, force system memory.
				sd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
			DirectDraw::err = DirectDraw::pdd4->CreateSurface(&sd, &pddsDraw4, 0);

			//
			// If we wanted video memory, but the suface didn't end up there, then
			// recreate the surface without the videomem flag.  Otherwise, we have the Win16 lock
			// without even the benefit of video memory.
			//

			DirectDraw::err = pddsDraw4->GetSurfaceDesc(&sd);
			if (!(sd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY))
			{
				pddsDraw4->Release();
				sd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
				DirectDraw::err = DirectDraw::pdd4->CreateSurface(&sd, &pddsDraw4, 0);
			}
		}
		else 
		{
			// Just a single buffer.
			// Copy the COM pointer.
			pddsDraw4 = pddsPrimary4;
		}

		return true;
	}

	//******************************************************************************************
	//
	void ConstructSoftware
	(
		HWND hwnd,					// A previously constructed Windows HWND.
		int i_width,				// The desired dimensions of the raster.
		int i_height,
		int i_bits,					// How many bits deep for full-screen, 0 for windowed.
		int i_buffers,				// How many buffers to construct.
		CSet<ERasterFlag> seteras	// Any special flags for the raster.
	)
	//
	// Creates a screen raster for use with the software rasterizer.
	//
	//**************************************
	{
		Assert(hwnd);
		Assert(bWithin(i_buffers, 1, 3));

		iBuffers    = i_buffers;
		bFullScreen = i_bits != 0;
		iWidthFront  = i_width;
		iHeightFront = i_height;

		if (i_bits && GetWindowModeConfigured() == WindowMode::EXCLUSIVE)
		{		
			// Go fullscreen.  We need to call 2 DD functions to do this.
			DirectDraw::err = DirectDraw::pdd4->SetCooperativeLevel(hwnd, 
				DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
			DirectDraw::err = DirectDraw::pdd4->SetDisplayMode(i_width, i_height, i_bits, 0, 0);
		}
		else 
		{			
			// Go windowed in current screen mode.
			// Return to Windows screen if necessary.
			DirectDraw::err = DirectDraw::pdd4->SetCooperativeLevel(hwnd, DDSCL_NORMAL);
		}

		// Try to construct the backbuffer in video memory.
		if (i_bits && i_buffers > 1 && seteras[erasVideoMem])
		{
			if (bConstructSoftwareVidRam(i_width, i_height, i_bits, i_buffers))
			{
				bFlippable = true;
				return;
			}
		}

		// Creating a buffer in video memory failed. Create one in system memory.
		bConstructSoftwareSysRam(i_width, i_height, i_bits, i_buffers, seteras);
		bFlippable = false;
	}

	//******************************************************************************************
	//
	bool bConstructD3D
	(
		HWND hwnd,							// A previously constructed Windows HWND.
		int i_width, int i_height,			// The desired dimensions of the raster.
		int i_bits = 16
	);
	//
	// Creates a screen raster for use with Direct3D.
	//
	//**************************************

	bool bConstructD3DExclusive
	(
		HWND hwnd,							// A previously constructed Windows HWND.
		int i_width, int i_height,			// The desired dimensions of the raster.
		int i_bits = 16
	);

	bool bConstructD3DWindowed
	(
		HWND hwnd,							// A previously constructed Windows HWND.
		int i_width, int i_height,			// The desired dimensions of the raster.
		int i_bits = 16
	);
	
	//******************************************************************************************
	//
	bool bFindZBufferFormat
	(
		DDPIXELFORMAT& rddpf_zbuffer	// Pixel format for the Z buffer if a Z buffer is found.
	);
	//
	// Returns 'true' if a valid Z buffer format was found.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bConstructZBuffer
	(
		const DDPIXELFORMAT& ddpf_zbuffer	// Pixel format for the Z buffer if a Z buffer is found.
	);
	//
	// Creates a z buffer raster for use with Direct3D.
	//
	//**************************************

	friend class CRasterWin;
};


//
// Class implementations.
//

//**********************************************************************************************
//
// CRasterVid implementation.
//
// Notes:
//		CRasterVid and descendents are implemented with DirectDraw, which provides surface
//		management, flipping, and blitting.
//
//		Most DirectDraw function return codes are assigned to DirectDraw::err to provide
//		error handling.
//

//**********************************************************************************************
//
// Private globals.
//

	char* astrDDSCaps[] = {
		"3D", "Alpha", "Back", "Complex", "Flip", "Front", "OffScreen", "Overlay",
		"Palette", "Primary", "Left", "SysMem", "Texture", "3D", "VidMem", "Visible",
		"Write-only", "ZBuffer", "OwnDC", "Video", "HWCodec", "ModeX", "Mip", "AllocLoad",
	0};

	void PrintNames(char str_dest[], char* astr_names[], uint u_bits)
	{
		str_dest[0] = 0;
		for (uint u = 0; astr_names[u] && (1<<u); u++)
			if (u_bits & (1<<u))
			{
				strcat(str_dest, " ");
				strcat(str_dest, astr_names[u]);
			}
	}

	//******************************************************************************************
	CRasterVid::CRasterVid(int i_width, int i_height, int i_bits, CSet<ERasterFlag> seteras)
	{
		pddsDraw4 = 0;

		// No default bit depth.
		Assert(i_bits);
		int i_realbits = i_bits == 15 ? 16 : i_bits;

		if (DirectDraw::pdd4)
		{
			CDDSize<DDSURFACEDESC2> sd;

			// Fill out the sd structure for the CreateSurface call.
			sd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
			sd.dwFlags |= DDSD_PIXELFORMAT;
			// Initialise the structure.
			new(&sd.ddpfPixelFormat) CDDSize<DDPIXELFORMAT>;

			// Specify the bit count.
			sd.ddpfPixelFormat.dwRGBBitCount = i_realbits;

			// Specify that RGB masks are filled in.
			sd.ddpfPixelFormat.dwFlags = DDPF_RGB;
			switch (i_bits)
			{
				case 8:
					sd.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;
					break;
				case 15:
					// A modified 16-bit raster.
					sd.ddpfPixelFormat.dwRBitMask = 0x7C00;
					sd.ddpfPixelFormat.dwGBitMask = 0x03E0;
					sd.ddpfPixelFormat.dwBBitMask = 0x001F;
					break;
				case 16:
					sd.ddpfPixelFormat.dwRBitMask = 0xF800;
					sd.ddpfPixelFormat.dwGBitMask = 0x07E0;
					sd.ddpfPixelFormat.dwBBitMask = 0x001F;
					break;
				case 32:
					sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
					// No break.
				case 24:
					sd.ddpfPixelFormat.dwRBitMask = 0xFF0000;
					sd.ddpfPixelFormat.dwGBitMask = 0x00FF00;
					sd.ddpfPixelFormat.dwBBitMask = 0x0000FF;
					break;
				default:
					Assert(0);
			}

			if (seteras[erasTexture])
			{
				sd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
				if (seteras[erasVideoMem])
					// Video textures might be loaded specially.
					sd.ddsCaps.dwCaps |= DDSCAPS_ALLOCONLOAD;
			}
			else
			{
				sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

				// Add the Direct3D flag if required.
				if (bGetD3D())
					sd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
			}

			if (!seteras[erasVideoMem])
				// Force surfaces to be in system memory.
				sd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

			if (seteras[erasTexture])
			{
				//
				// All texture dimensions must be power of two.
				// So we create the DirectDraw surface with dimensions moved up to the next power of 2,
				// and leave the raster dimensions as they are.
				//
				sd.dwWidth	= NextPowerOfTwo(i_width);
				sd.dwHeight	= NextPowerOfTwo(i_height);
			}
			else
			{
				sd.dwWidth	= RoundUp(i_width, 8);
				sd.dwHeight	= i_height;
			}

			DirectDraw::err = DirectDraw::pdd4->CreateSurface(&sd, &pddsDraw4, 0);
			AlwaysAssert(pddsDraw4);

			// Retrieve the surface format info.
			DirectDraw::err = pddsDraw4->GetSurfaceDesc(&sd);
			u4DDSFlags = sd.ddsCaps.dwCaps;
			bVideoMem = (sd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;

			// Create a CPixelFormat structure from sd info.
			CPixelFormat pxf
			(
				(int) sd.ddpfPixelFormat.dwRGBBitCount,
				sd.ddpfPixelFormat.dwRBitMask, 
				sd.ddpfPixelFormat.dwGBitMask, 
				sd.ddpfPixelFormat.dwBBitMask
			);

			SetRaster(i_width, i_height, (int) sd.ddpfPixelFormat.dwRGBBitCount, (int) sd.lPitch, &pxf);

			bLocked = 0;
			
			eClearMethod = ecmTEST;
			i4ClearTiming = 0;
		}
		else
		{
			CDDSize<DDSURFACEDESC2> sd;

			// Fill out the sd structure for the CreateSurface call.
			sd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
			sd.dwFlags |= DDSD_PIXELFORMAT;
			// Initialise the structure.
			new(&sd.ddpfPixelFormat) CDDSize<DDPIXELFORMAT>;

			// Specify the bit count.
			sd.ddpfPixelFormat.dwRGBBitCount = i_realbits;

			// Specify that RGB masks are filled in.
			sd.ddpfPixelFormat.dwFlags = DDPF_RGB;
			switch (i_bits)
			{
				case 8:
					sd.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;
					break;
				case 15:
					// A modified 16-bit raster.
					sd.ddpfPixelFormat.dwRBitMask = 0x7C00;
					sd.ddpfPixelFormat.dwGBitMask = 0x03E0;
					sd.ddpfPixelFormat.dwBBitMask = 0x001F;
					break;
				case 16:
					sd.ddpfPixelFormat.dwRBitMask = 0xF800;
					sd.ddpfPixelFormat.dwGBitMask = 0x07E0;
					sd.ddpfPixelFormat.dwBBitMask = 0x001F;
					break;
				case 32:
					sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
					// No break.
				case 24:
					sd.ddpfPixelFormat.dwRBitMask = 0xFF0000;
					sd.ddpfPixelFormat.dwGBitMask = 0x00FF00;
					sd.ddpfPixelFormat.dwBBitMask = 0x0000FF;
					break;
				default:
					Assert(0);
			}

			if (seteras[erasTexture])
			{
				sd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
				if (seteras[erasVideoMem])
					// Video textures might be loaded specially.
					sd.ddsCaps.dwCaps |= DDSCAPS_ALLOCONLOAD;
			}
			else
			{
				sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

				// Add the Direct3D flag if required.
				if (bGetD3D())
					sd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
			}

			if (!seteras[erasVideoMem])
				// Force surfaces to be in system memory.
				sd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

			if (seteras[erasTexture])
			{
				//
				// All texture dimensions must be power of two.
				// So we create the DirectDraw surface with dimensions moved up to the next power of 2,
				// and leave the raster dimensions as they are.
				//
				sd.dwWidth	= NextPowerOfTwo(i_width);
				sd.dwHeight	= NextPowerOfTwo(i_height);
			}
			else
			{
				sd.dwWidth	= RoundUp(i_width, 8);
				sd.dwHeight	= i_height;
			}

			DirectDraw::err = DirectDraw::pdd4->CreateSurface(&sd, &pddsDraw4, 0);
			AlwaysAssert(pddsDraw4);

			// Retrieve the surface format info.
			DirectDraw::err = pddsDraw4->GetSurfaceDesc(&sd);
			u4DDSFlags = sd.ddsCaps.dwCaps;
			bVideoMem = (sd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;

			// Create a CPixelFormat structure from sd info.
			CPixelFormat pxf
			(
				(int) sd.ddpfPixelFormat.dwRGBBitCount,
				sd.ddpfPixelFormat.dwRBitMask, 
				sd.ddpfPixelFormat.dwGBitMask, 
				sd.ddpfPixelFormat.dwBBitMask
			);

			SetRaster(i_width, i_height, (int) sd.ddpfPixelFormat.dwRGBBitCount, (int) sd.lPitch, &pxf);

			bLocked = 0;
			
			eClearMethod = ecmTEST;
			i4ClearTiming = 0;
		}

		AlwaysAssert(pddsDraw4);
	}

	//******************************************************************************************
	CRasterVid::~CRasterVid() 
	{
		if (prasLink)
			prasLink->prasLink = rptr0;
		prasLink = rptr0;
	}

	//******************************************************************************************
	bool CRasterVid::bRestore(int i_err)
	{
		if (i_err == DDERR_SURFACELOST)
		{
			DirectDraw::err = pddsDraw4->Restore();
		}
		else if (i_err != DDERR_WASSTILLDRAWING)
		{
			DirectDraw::err = i_err;
		}
		return i_err != DD_OK;
	}

	//******************************************************************************************
	void CRasterVid::Lock() 
	{
		if (bLocked)
			return;

		//
		// If back surfaces are in system memory,
		// we don't have to worry about Lock hanging the system.
		//
		CDDSize<DDSURFACEDESC2> sd;
		while (bRestore(pddsDraw4->Lock(0, &sd, DDLOCK_WAIT, 0)));
		pSurface = sd.lpSurface;
		bLocked = 1;
	}

	//******************************************************************************************
	void CRasterVid::Unlock() 
	{
		if (!bLocked)
			return;
		DirectDraw::err = pddsDraw4->Unlock(0);
		bLocked = 0;
	}

	//******************************************************************************************
	void CRasterVid::AttachPalette(CPal* ppal, CPixelFormat* ppxf) 
	{
		CRaster::AttachPalette(ppal, ppxf);

		if (iPixelBits == 8) 
		{
			// Palettes only work in this mode.

			// Convert colours to PALETTEENTRY format, by swapping R and B.
			PALETTEENTRY ape[256];
			memset(ape, 0, sizeof(ape));

			if (ppal)
			{
				// Initialise the palette entries with ppal
				uint u;

				for (u = 0; u < ppal->aclrPalette.uLen; u++)
				{
					ape[u].peRed   = ppal->aclrPalette[u].u1Red;
					ape[u].peGreen = ppal->aclrPalette[u].u1Green;
					ape[u].peBlue  = ppal->aclrPalette[u].u1Blue;
				}
				// Add the Direct3D flag if required.
				bool b_d3d = bGetD3D();

				for (; u < 256; u++)
				{
					ape[u].peFlags = (b_d3d) ? (D3DPAL_RESERVED) : (0);
				}
			}

			if (!pddpal)
			{
				// Create a custom palette, initialise it with ppal, and attach it to DD surface.
				DirectDraw::err = DirectDraw::pdd4->CreatePalette(
					DDPCAPS_8BIT | DDPCAPS_ALLOW256 | (ppal? DDPCAPS_INITIALIZE : 0), ape, &pddpal, NULL);
				DirectDraw::err = pddsDraw4->SetPalette(pddpal);
			}
			else if (ppal)
			{
				// Palette already created.  Change the entries.
				DirectDraw::err = pddpal->SetEntries(0, 0, ppal->aclrPalette.uLen, ape);
			}
		}
	}

	//******************************************************************************************
	void CRasterVid::Clear(TPixel pix)
	{
		// Clear the Z buffer if required.
		if (d3dDriver.bGetFlipClear())
			d3dDriver.ClearZBuffer();

		if (!(bUSE_DD_CLEARS_ONLY && bVideoMem))
		{
			if (eClearMethod >= ecmTEST)
			{
				//
				// Still testing clear times.
				//

				// Remember the current flag.
				EClearMethod ecm = eClearMethod;

				// Time the clear by setting the flag to a value below ecmTEST,
				// and calling ourselves recursively.
				eClearMethod = EClearMethod( int(eClearMethod) % int(ecmTEST) );

				CCycleTimer ctmr;
				Clear(pix);
				int32 i4_time = int32(ctmr());

				if (eClearMethod == ecmDD)
					i4ClearTiming += i4_time;
				else
					i4ClearTiming -= i4_time;

				// Restore current flag, and increment.
				eClearMethod = EClearMethod( int(ecm) + 1);
				if (eClearMethod >= ecmTEST + 12)
				{
					// Done timing, come to a decision.
					if (i4ClearTiming > 0)
						eClearMethod = ecmDWI;
					else
						eClearMethod = ecmDD;
				}

				return;
			}
			else if (eClearMethod == ecmDWI)
			{
				CRasterWin* pras_win = dynamic_cast<CRasterWin*>(this);

				if (pras_win)
				{
					pras_win->LockAll();
					int i_width  = pras_win->iWidthFront;
					int i_height = pras_win->iHeightFront;
					uint16* pu2  = (uint16*)pSurface;

					for (int i_y = 0; i_y < i_height; i_y++, pu2 += iLinePixels)
					{
						::Fill(pu2, i_width, uint16(pix));
					}
					Unlock();
				}
				else
				{
					// Call the base class Clear(), which does 8 bytes at a time.
					CRaster::Clear(pix);
				}
				return;
			}
		}

		//
		// DirectDraw cannot Clear or Blit when the surface is locked.
		// So we need to detect when the surface is locked, unlock it, do the blit, then
		// relock it if necessary.  What a waste.
		//
		bool b_waslocked = bLocked;
		Unlock();

		// Use DirectDraw blitting.
		// Set up a blit fx struct to hold the fill colour.
		CDDSize<DDBLTFX> fx;
		fx.dwFillColor = pix;

		// Call Blt, with the COLORFILL operation.
		while (bRestore(pddsDraw4->Blt
			(
				0,										// Dest rectangle (0 means whole surface.)
				0,										// Source surface (none).
				0,										// Source rectangle (none).
				DDBLT_WAIT | DDBLT_COLORFILL,			// Blit type.
				&fx										// Blit params.
			)
		));

		// Relock the surface.
		if (b_waslocked)
			Lock();
	}

	//******************************************************************************************
	void CRasterVid::Blit(int i_dx, int i_dy, CRaster& ras_src, SRect* prect_src, 
		bool b_clip, bool b_colour_key, TPixel pix_colour_key)
	{
		//
		// Interesting issue regarding virtual functions.
		// CRaster::Blit is a virtual function that takes another CRaster& as an argument.
		// We want to do a DirectDraw blit if both rasters are CRasterVids or descendents,
		// and punt otherwise.
		//
		// Since we are in CRasterVid::Blit, we know that the first raster is a CRasterVid.
		// Now we just need to check that the second one is, otherwise punt back to CRaster::Blit.
		//

		CRasterVid* prasv_src;

		if (!(prasv_src = dynamic_cast<CRasterVid*>(&ras_src)))
		{
			// ras_src is not a CRasterVid, so call default blitting function.
			CRaster::Blit(i_dx, i_dy, ras_src, prect_src, b_clip, b_colour_key, pix_colour_key);
			return;
		}

		//
		// Use DirectDraw to blit.
		// Unlock if necessary.  See Clear above.
		//
		bool b_waslocked = bLocked;
		Unlock();

		//
		// Declare a RECT variable, required by the DD Blit routine to encode the source 
		// blitting rectangle.
		//
		RECT rc_src;

		// Set rc_src values based on prect_src values.
		if (prect_src)
			SetRect(&rc_src, prect_src->iX, prect_src->iY, prect_src->iWidth, prect_src->iHeight);
		else
			// Set to entire surface.
			SetRect(&rc_src, 0, 0, prasv_src->iWidth, prasv_src->iHeight);

		if (b_clip)
		{
			// Clip all coords against raster limits.
			if (i_dx < 0) 
			{
				rc_src.left -= i_dx;
				i_dx = 0;
			}
			else if (i_dx >= iWidth) 
				return;
			if (i_dy < 0) 
			{
				rc_src.top -= i_dy;
				i_dy = 0;
			}
			else if (i_dy >= iHeight) 
				return;

			SetMax(rc_src.left, 0);
			SetMax(rc_src.top,  0);
			SetMin(rc_src.right,  Min(prasv_src->iWidth,  rc_src.left + iWidth  - i_dx));
			SetMin(rc_src.bottom, Min(prasv_src->iHeight, rc_src.top  + iHeight - i_dy));

			// Abort if there is nothing to blit.
			if (rc_src.right <= rc_src.left || rc_src.bottom <= rc_src.top)
				return;
		}

		if (b_colour_key) 
		{
			// Set pras_src's colour key to pix_colour_key, if given.
			DDCOLORKEY ddck;
			ddck.dwColorSpaceLowValue = ddck.dwColorSpaceHighValue = pix_colour_key;
			prasv_src->pddsDraw4->SetColorKey(DDCKEY_SRCBLT, &ddck);
		}

		int i_err;
		while ((i_err = pddsDraw4->BltFast
			(
				i_dx, i_dy,						// Dest position.
				prasv_src->pddsDraw4,			// Source surface.
				&rc_src,						// Source rectangle.
				DDBLTFAST_WAIT | (b_colour_key ? DDBLTFAST_SRCCOLORKEY : 0)
			)
		) == DDERR_SURFACELOST)
		{
			// One of the surfaces was lost; restore them both.
			DirectDraw::err = pddsDraw4->Restore();
			DirectDraw::err = prasv_src->pddsDraw4->Restore();
		}
		DirectDraw::err = i_err;

		if (b_waslocked)
			Lock();
	}

	//******************************************************************************************
	HDC CRasterVid::hdcGet() 
	{
		// Get the DC via DirectDraw's cumbersome interface.
		HDC hdc = 0;
		while (bRestore(pddsDraw4->GetDC(&hdc)));
		return hdc;
	}

	//******************************************************************************************
	void CRasterVid::ReleaseDC(HDC hdc) 
	{
		DirectDraw::err = pddsDraw4->ReleaseDC(hdc);
	}

	//******************************************************************************************
	void CRasterVid::GetDDSDesc(char str_dest[]) 
	{
		PrintNames(str_dest, astrDDSCaps, u4DDSFlags);
	}


//*****************************************************************************************
rptr<CRaster> prasReadBMP(const char* str_bitmap_name, bool b_vid)
{
	BITMAPFILEHEADER*	pbfh;
	BITMAPINFOHEADER*	pbih;
	HANDLE				hfile;
	uint32				u4_byte_count;
	uint32				u4_length;
	char*				pu1_bitmap_buffer;

	hfile = CreateFile
	(
		(LPCTSTR)str_bitmap_name,	// Pointer to name of the uncompressed file.
		GENERIC_READ,				// Access mode.
		FILE_SHARE_READ,			// Share mode.
		0,							// Pointer to security descriptor.
		OPEN_EXISTING,				// How to create.
		FILE_ATTRIBUTE_NORMAL,		// File attributes.
		0 							// Handle to file with attributes to copy.
	);

	//
	// If the handle after opening with Win32 is invalid then the file
	// probably does not exit, return a NULL raster.
	//
	if (hfile == INVALID_HANDLE_VALUE)
	{
		return rptr<CRaster>();
	}

	u4_length = GetFileSize(hfile, NULL);
	pu1_bitmap_buffer = (char*)VirtualAlloc(NULL, u4_length,
		MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);	

	if (pu1_bitmap_buffer == NULL)
	{
		dprintf("Virtual Alloc failed..\n");
		return rptr<CRaster>();
	}

	Verify(ReadFile(hfile,pu1_bitmap_buffer,u4_length,(DWORD*)&u4_byte_count, NULL));
	CloseHandle(hfile);

	//
	// file has now been read into memory and decompressed if requird
	//

	pbfh = (BITMAPFILEHEADER*)pu1_bitmap_buffer;

	if (pbfh->bfType != 'MB')
	{
		_RPT1(_CRT_ASSERT,"ERROR: File %s has zero length\n",str_bitmap_name);
		AlwaysAssert(0);
	}

	pbih = (BITMAPINFOHEADER*) (pu1_bitmap_buffer+sizeof(BITMAPFILEHEADER));

	pbih->biWidth    = Abs(pbih->biWidth);
	pbih->biHeight   = Abs(pbih->biHeight);

	Assert(pbih->biWidth > 0);
	Assert(pbih->biHeight > 0);

	// Create ourselves.
	rptr<CRaster> pras;

	if (b_vid)
	{
		// Bit of a hack here; if we're going to convert the 16-bit raster to 15-bit,
		// we must also tell the DirectDraw surface to be in 15-bit format.
		int i_bits = pbih->biBitCount;
		if (i_bits == 16 && prasMainScreen->pxf.cposG.u1WidthDiff == 3)
			i_bits = 15;
		pras = rptr_cast(CRaster, rptr_new CRasterVid(pbih->biWidth, pbih->biHeight, i_bits));
	}
	else
	{
		// Create a RasterMem cause that's all we need.
		pras = rptr_cast(CRaster, rptr_new CRasterMem(pbih->biWidth, pbih->biHeight, pbih->biBitCount, 0));
	}

	CPal* ppal = 0;

	// Create a CPal, if palettised.
	if (pbih->biBitCount == 8)
	{
		if (pbih->biClrUsed == 0)
			pbih->biClrUsed = 256;

		// Create palette of appropriate size.
		ppal = new CPal(256);

		void* pv = ppal->aclrPalette;

		memcpy(pv, pu1_bitmap_buffer+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER), 
			sizeof(CColour)*pbih->biClrUsed );

#if bSTIPPLE_MIP_LEVELS
		ppal->aclrPalette[255] = CColour(255,0,0);			//MIP 0 = RED
		ppal->aclrPalette[254] = CColour(0,255,0);			//MIP 1 = GREEN
		ppal->aclrPalette[253] = CColour(0,0,255);			//MIP 2 = BLUE
		ppal->aclrPalette[252] = CColour(255,255,0);		//MIP 3 = YELLOW
		ppal->aclrPalette[251] = CColour(255,0,255);		//MIP 4 = MAGENTA
		ppal->aclrPalette[250] = CColour(0,255,255);		//MIP 5 = CYAN
#endif
	}
	else
		pbih->biClrUsed = 0;


	char* pu1_bits = pu1_bitmap_buffer + pbfh->bfOffBits;

	pras->Lock();

	uint u_file_line_bytes = RoundUp((long)(pbih->biWidth * pbih->biBitCount / 8), (long)4);

	// We want the top lines first, which is the reverse of the order they come in.
	// We must read last lines first.
	for (int i_line = pras->iHeight-1; i_line >= 0; i_line--)
	{
		memcpy(pras->pAddress(0, i_line), pu1_bits, u_file_line_bytes);
		pu1_bits += u_file_line_bytes;

#if VER_DEBUG
		if (pbih->biBitCount == 8 && pbih->biClrUsed < 256)
		{
			// Check for bogus pixels.
			uint8* pu1 = (uint8*)pras->pAddress(0, i_line);
			for (int i_pix = 0; i_pix < pras->iWidth; i_pix++)
			{
				Assert(*pu1 < pbih->biClrUsed);
				pu1++;
			}
		}
#endif
	}

	pras->Unlock();

	if (ppal)
	{
		// Add the palette to the database, and attach it to the raster.
		ppal->aclrPalette.uLen = pbih->biClrUsed;

/*
		// Make sure that the palette is added to the database.
		// Sometimes, palettes that get associated with rasters are not then 
		// later added to the database in the CTexture constructor.
		// This database call makes sure that such evilness does not happen.
		ppal = pcdbMain.ppalAddPalette(ppal, false);
*/
		pras->AttachPalette(ppal);
	}

	Assert(pras->iWidth  > 0);
	Assert(pras->iHeight > 0);

	VirtualFree(pu1_bitmap_buffer, u4_length, MEM_DECOMMIT);
	VirtualFree(pu1_bitmap_buffer, 0, MEM_RELEASE);
	
	return pras;
}



//**********************************************************************************************
//
// Globals for CRasterWin.
//

	//************************************************************************
	//
	static void ClientToScreen
	(
		HWND hwnd,						// A valid Windows window.
		RECT* prc						// The rectangle to convert.
	)
	//
	// Add the screen location of hwnd to the dimensions of prc.
	//
	// Cross references:
	//		Used by raster functions below.
	//
	//**************************
	{
		POINT pt = {0, 0};

		::ClientToScreen(hwnd, &pt);
		prc->top    += pt.y;
		prc->bottom += pt.y;
		prc->left   += pt.x;
		prc->right  += pt.x;
	}

//**********************************************************************************************
//
// CRasterWin implementation.
//

	//******************************************************************************************
	CRasterWin::CRasterWin(HWND hwnd, int i_width, int i_height, int i_bits, int i_buffers, 
		                   CSet<ERasterFlag> seteras)
	{
		pddsDraw4    = 0;
		pddsPrimary4 = 0;
		pddsZBuffer  = 0;

		iOffsetX       = 0;
		iOffsetY       = 0;
		iOffsetPointer = 0;

		bGammaAvailable = false;
		bGammaFlash		= false;

		bFullScreen = i_bits != 0;

		// Use separate creation code for Direct3D.
		if (priv_self.bConstructD3D(hwnd, i_width, i_height, 16))
		{
			AlwaysAssert(pddsDraw4);
			AlwaysAssert(pddsPrimary4);
		}
		else
		{
			// Make sure that Direct3D is unitialized.
			d3dDriver.Uninitialize();

			// Construct a regular DirectDraw interface.
			priv_self.ConstructSoftware(hwnd, i_width, i_height, i_bits, i_buffers, seteras);
		}
		ConstructClipper(hwnd);
		AlwaysAssert(pddsDraw4);
		AlwaysAssert(pddsPrimary4);

		//
		// Common setup.
		//

		if (DirectDraw::pdd4)
		{
			// Check the card type.
			EVideoCard evc_type = evcGetCard(DirectDraw::pdd4);

			// Ignore the Permedia 2, gamma is broken on it.
			if (evc_type != evcPermedia2)
			{
				// Check for primary gamma capability.
				CDDSize<DDCAPS> ddcaps;

				HRESULT ddrval = DirectDraw::pdd4->GetCaps(&ddcaps, NULL);

				if (bFullScreen && (ddrval == DD_OK) && (ddcaps.dwCaps2 & DDCAPS2_PRIMARYGAMMA))
				{
					// Get & save the current gamma settings.
					LPDIRECTDRAWGAMMACONTROL pddgc = 0;
					pddsPrimary4->QueryInterface(IID_IDirectDrawGammaControl, (void**)&pddgc);
					if (pddgc)
					{
						// Only activate gamma if we successfully get the original ramp.
						if (pddgc->GetGammaRamp(0, (LPDDGAMMARAMP)&grSaved) == DD_OK)
							bGammaAvailable = true;

						pddgc->Release();
					}
				}
			}
		}

		CDDSize<DDSURFACEDESC2> sd;

		//
		// Figure out the screen aspect ratio by getting the screen pixel dimensions,
		// and dividing by the assumed monitor aspect ratio.
		//
		DirectDraw::err = pddsPrimary4->GetSurfaceDesc(&sd);
		u4DDSFlagsFront = sd.ddsCaps.dwCaps;
		
		eClearMethod  = ecmTEST;
		i4ClearTiming = 0;
		bLocked       = 0;

		// Retrieve the surface format info.
		DirectDraw::err = pddsDraw4->GetSurfaceDesc(&sd);

		// Set up the info.
		CPixelFormat pxf
		(
			(int) sd.ddpfPixelFormat.dwRGBBitCount,
			sd.ddpfPixelFormat.dwRBitMask, 
			sd.ddpfPixelFormat.dwGBitMask, 
			sd.ddpfPixelFormat.dwBBitMask
		);
		SetRaster(i_width, i_height, (int) sd.ddpfPixelFormat.dwRGBBitCount, (int) sd.lPitch, &pxf);

		u4DDSFlags	= sd.ddsCaps.dwCaps;
		bVideoMem	= (sd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;

		//
		// Clear any border than may be visible due to a size difference between
		// the screen size and the render size.'
		//
		ClearBorder();

		// Set up Direct3D.
		if (d3dDriver.bInitEnabled() && pddsDraw4)
		{
			Verify(d3dDriver.bInitialize(pddsDraw4, pddsPrimary4));
		}
	}

	//******************************************************************************************
	CRasterWin::~CRasterWin() 
	{
		if (prasLink)
			prasLink->prasLink = rptr0;
		prasLink = rptr0;

		// Release Direct3D before doing anything.
		d3dDriver.Uninitialize();

		if (bFullScreen)
		{
			// Return to Windows screen if necessary.
			if (DirectDraw::pdd4)
			{
				if (GetWindowModeConfigured() == WindowMode::EXCLUSIVE)
					DirectDraw::err = DirectDraw::pdd4->RestoreDisplayMode();
				DirectDraw::err = DirectDraw::pdd4->SetCooperativeLevel(0, DDSCL_NORMAL);
			}
		}


		if (bFlippable)
		{
			//
			// bFlippable implies that pddsDraw is an attached surface to pddsPrimary.
			// So when this destructor destroys pddsPrimary, pddsDraw will be automatically
			// destroyed.  So we must prevent ~CRasterVid from trying to destroy pddsDraw.
			// We do this by re-initialising it to 0.
			//
			pddsDraw4.SafeRelease();
			pddsZBuffer.SafeRelease();
			pddsPrimary4.SafeRelease();
		/*
			if (DirectDraw::pdd4)
				new(&pddsDraw4) CCom<IDirectDrawSurface4>;
			else
				new(&pddsDraw) CCom<IDirectDrawSurface>;
		*/
		}

		/*
		if (bIsTrespasser)
		{
			// Really brutal, but clean: kill DirectDraw entirely and resurect it.
			g_initDD.ReleaseAll();
			g_initDD.BaseInit();
		}
		*/
	}

	void CRasterWin::ConstructClipper(HWND hwnd)
	{
		// Create a DirectDrawClipper object, needed for the window.
		if (!bFullScreen)
		{
			DirectDraw::err = DirectDraw::pdd4->CreateClipper(0, &pddclip, 0);
			DirectDraw::err = pddclip->SetHWnd(0, hwnd);
			DirectDraw::err = pddsPrimary4->SetClipper(pddclip);
		}
		AlwaysAssert(bFullScreen || pddclip);
	}

	//******************************************************************************************
	bool CRasterWin::bRestore(int i_err)
	{
		// This differs from CRasterVid::bRestore in that it restores the primary rather than 
		// the drawing surface.
		if (i_err == DDERR_SURFACELOST)
		{
			DirectDraw::err = pddsPrimary4->Restore();
		}
		else if (i_err != DDERR_WASSTILLDRAWING)
		{
			// only set the error code if it is not a busy/still drawing error
			DirectDraw::err = i_err;
		}
		return i_err != DD_OK;
	}

	//******************************************************************************************
	bool CRasterWin::bRestoreLostSurfaces()
	{
		// Restore the primary surface if required.
		if (pddsPrimary4)
			if (pddsPrimary4->IsLost())
				pddsPrimary4->Restore();

		// Restore the backbuffer surface if required.
		if (pddsDraw4)
			if (pddsDraw4->IsLost())
				pddsDraw4->Restore();

		// Restore the primary surface if required.
		if (pddsZBuffer)
			if (pddsZBuffer->IsLost())
				pddsZBuffer->Restore();

		// Indicate that all surfaces have been restored.
		return true;
	}

	//******************************************************************************************
	void CRasterWin::AttachPalette(CPal* ppal, CPixelFormat* ppxf) 
	{
		CRasterVid::AttachPalette(ppal, ppxf);
		if (iPixelBits == 8) 
		{
			// Palettes only work in this mode.
			// Attach palette to primary screen as well.  This realises it on screen.
			Assert(pddpal);
			DirectDraw::err = pddsPrimary4->SetPalette(pddpal);
		}
	}

	//******************************************************************************************
	void CRasterWin::Flip() 
	{
		// If single surface, return.
		if (pddsPrimary4 == pddsDraw4)
			return;

		// Force an unlock.
		Unlock();

		RECT rc_src, rc_dest;
		SetRect(&rc_src, 0, 0, iWidthFront, iHeightFront);

		if (!bFullScreen)
		{
			//
			// Windowed Mode - cannot flip, must do a blit!
			// NOTE: Cannot use a fast blit even if the surfaces sizes match
			//		 because there is a clipper attached to the surface. This
			//		 will cause the BltFast call to fail.
			//


			HWND hwnd;
			pddclip->GetHWnd(&hwnd);

			SetRect(&rc_dest, 0, 0, iWidthFront, iHeightFront);
			// Offset the rc_dest rect by the window's screen location.
			::ClientToScreen(hwnd, &rc_dest);
			rc_dest.left	+=((rc_dest.right - rc_dest.left) - rc_src.right)>>1;
			rc_dest.top		+=((rc_dest.bottom - rc_dest.top) - rc_src.bottom)>>1;
			rc_dest.right	= rc_dest.left + rc_src.right;
			rc_dest.bottom	= rc_dest.top  + rc_src.bottom;

			// do a stretch blit to fill the window
			while (bRestore(pddsPrimary4->Blt(&rc_dest, pddsDraw4, &rc_src, DDBLT_WAIT, NULL)));
		}
		else
		{
			if (bFlippable) 
			{  
				while (bRestore(pddsPrimary4->Flip(pddsDraw4, DDFLIP_WAIT)));
			}
			else if (iWidth == iWidthFront && iHeight == iHeightFront)
			{
				//
				// Call BltFast function, because no clipping or stretching needed.
				//

				while (bRestore(pddsPrimary4->BltFast(0, 0, pddsDraw4, &rc_src, DDBLTFAST_WAIT)));
			}
			else
			{
				//
				// Call Blt function, which can clip and stretch. Offset the destination rect
				// by the size of the source window so the view port is centered.
				//
				SetRect(&rc_dest, 0, 0, iWidthFront, iHeightFront);
				rc_dest.left	+=((rc_dest.right - rc_dest.left) - rc_src.right)>>1;
				rc_dest.top		+=((rc_dest.bottom - rc_dest.top) - rc_src.bottom)>>1;
				rc_dest.right	= rc_dest.left + rc_src.right;
				rc_dest.bottom	= rc_dest.top  + rc_src.bottom;

				while (bRestore(pddsPrimary4->Blt(&rc_dest, pddsDraw4, &rc_src, DDBLT_WAIT, NULL)));
			}
		}

		// Clear the Z buffer if required.
		if (d3dDriver.bGetFlipClear())
			d3dDriver.ClearZBuffer();
	}

	//******************************************************************************************
	void CRasterWin::GammaFlash(float fAmount)
	{
		// Only works in full screen mode.
		if (!bGammaAvailable)
			return;

		// Clamp range from 0 to 1
		if (fAmount < 0.0f)
			fAmount = 0.0f;
		else if (fAmount > 1.0f)
			fAmount = 1.0f;

		// Get the gamma control interface.
		LPDIRECTDRAWGAMMACONTROL pddgc = 0;
		pddsPrimary4->QueryInterface(IID_IDirectDrawGammaControl, (void**)&pddgc);
		if (pddgc)
		{
			DDGAMMARAMP gr;

			int i_base = int(fAmount * 32767.5f);
			int i_range = 65535 - i_base*2;

			for (int i = 0; i < 256; i++)
			{
				WORD wGammaVal = WORD(i_base + (i * i_range) / 255);

				gr.red[i] = wGammaVal;
				gr.green[i] = wGammaVal;
				gr.blue[i] = wGammaVal;
			}

			pddgc->SetGammaRamp(0, &gr);

			pddgc->Release();

			// Set flag indicating gamma flash is active.
			bGammaFlash = true;
		}
	}

	//******************************************************************************************
	void CRasterWin::RestoreGamma()
	{
		// Only works in full screen mode.
		if (!bGammaAvailable || !bGammaFlash)
			return;

		// Get the gamma control interface.
		LPDIRECTDRAWGAMMACONTROL pddgc = 0;
		pddsPrimary4->QueryInterface(IID_IDirectDrawGammaControl, (void**)&pddgc);
		if (pddgc)
		{
			bGammaFlash = false;
			pddgc->SetGammaRamp(0, (LPDDGAMMARAMP)&grSaved);
			pddgc->Release();
		}
	}

	//******************************************************************************************
	void CRasterWin::HandleActivate(bool b_active)
	{
		// Currently, just restores the palette when activated.
		if (pddpal && !bFullScreen)
		{
			if (b_active)
			{
				// Make sure the palette is realized.
				DirectDraw::err = pddsPrimary4->SetPalette(pddpal);
			}
			//
			// There is no corresponding action when the window is deactivated.
			//
			// In a normal Windows program, one would call the UpdateColors function when
			// deactivated in order to set a palette translation that would match the window's
			// palette as closely as possible to the system palette.
			//
			// But this doesn't work in DirectDraw, because it has no palette translation
			// during blits.  That's good, but it would be nice for there to be an equivalent 
			// function.  For now, we just have to live with wrong colours when a DD window is 
			// deactivated.
			//
		}
	}

	//******************************************************************************************
	void CRasterWin::FlipToGDISurface() 
	{
		// Do something only if the surface is a flipping surface.
		if (!bFlippable)
			return;

		// Just call the DD function.  
		DirectDraw::err = DirectDraw::pdd4->FlipToGDISurface();
	}

	//******************************************************************************************
	void CRasterWin::GetDDSDescFront(char str_dest[]) 
	{
		PrintNames(str_dest, astrDDSCaps, u4DDSFlagsFront);
	}


	//******************************************************************************************
	// Clear the border that is visible between the render surface and the visible window
	// surface
	void CRasterWin::ClearBorder(bool bBackBuffer /* = false */)
	{
        IDirectDrawSurface4 *    pdds;

		// if the render surface and screen surface are the same size then we do not need to
		// do anything because there is no border.
		// We do not need to do anything if we are stretching the render surface to fit the
		// screen
		if (iWidthFront == iWidth && iHeightFront == iHeight)
			return;

		// Now clear the visible part of the primary buffer
		RECT rc_src, rc_dest, rc_win;
		SetRect(&rc_src, 0, 0, iWidth, iHeight);
		SetRect(&rc_dest, 0, 0, iWidthFront, iHeightFront);

		if (!bFullScreen)
		{
			// Offset the rc_dest rect by the window's screen location.
			HWND hwnd;
			pddclip->GetHWnd(&hwnd);
			::ClientToScreen(hwnd, &rc_dest);
		}

		// offset the destination rect by the size of the source window
/*		rc_dest.left	+=(((rc_dest.right - rc_dest.left) - rc_src.right)>>1);
		rc_dest.top		+=((rc_dest.bottom - rc_dest.top) - rc_src.bottom)>>1;
		rc_dest.right	= rc_dest.left + rc_src.right;
		rc_dest.bottom	= rc_dest.top  + rc_src.bottom;*/

		// clear the borders on the primary surface
		CDDSize<DDBLTFX> ddbltfx;
		ddbltfx.dwFillColor = u4BORDER_COLOUR;

        if (bBackBuffer)
        {
            pdds = pddsDraw4;
        }
        else
        {
            pdds = pddsPrimary4;
        }

		//
		// left hand border..
		//
		rc_win = rc_dest;
		rc_win.right = rc_dest.left+(((rc_dest.right - rc_dest.left) - rc_src.right)>>1);
		// if there is a left border clear it.
		if (rc_win.right > rc_win.left)
		{
			while (bRestore(pdds->Blt(&rc_win, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx)));
		}

		//
		// top border..
		//
		rc_win = rc_dest;
		rc_win.bottom = rc_dest.top + (((rc_dest.bottom - rc_dest.top) - rc_src.bottom)>>1);
		// if there is a top border clear it.
		if (rc_win.bottom > rc_win.top)
		{
			while (bRestore(pdds->Blt(&rc_win, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx)));
		}

		//
		// right border..
		//
		rc_win = rc_dest;
		rc_win.left= rc_dest.left+rc_src.right+(((rc_dest.right - rc_dest.left) - rc_src.right)>>1);
		// if there is a top border clear it.
		if (rc_win.right > rc_win.left)
		{
			while (bRestore(pdds->Blt(&rc_win, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx)));
		}

		//
		// bottom border..
		//
		rc_win = rc_dest;
		rc_win.top = rc_dest.top + rc_src.bottom + (((rc_dest.bottom - rc_dest.top) - rc_src.bottom)>>1);

		// if there is a top border clear it.
		if (rc_win.bottom > rc_win.top)
		{
			while (bRestore(pdds->Blt(&rc_win, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx)));
		}
	}


	//******************************************************************************************
	void CRasterWin::SetViewportDim(int i_width, int i_height, HWND hwnd)
	{
		int i_width_full  = iWidthFront;
		int i_height_full = iHeightFront;

		// Only care about the window handle when the application is not in full screen mode.
		//if (bFullScreen)
			hwnd = 0;

		if (hwnd)
		{
			RECT rc;
			GetClientRect(hwnd, &rc);
			i_width_full  = rc.right - rc.left;
			i_height_full = rc.top - rc.bottom;
		}

		// Set the pixel and pointer offsets for the raster.
		iOffsetX = (i_width_full  - i_width) / 2;
		iOffsetY = (i_height_full - i_height) / 2;
		iOffsetPointer = iOffsetX * iPixelBytes() + iOffsetY * iLinePixels * iPixelBytes();

		// Set the new width and height of the raster.
		iWidth  = i_width;
		iHeight = i_height;
	}
	
	//******************************************************************************************
	void CRasterWin::Lock()
	{
		AlwaysAssert(!srd3dRenderer.bIsBusy());
		CRasterVid::Lock();
		pSurface = (void*)(((uint8*)pSurface) + iOffsetPointer);
	}
	
	//******************************************************************************************
	void CRasterWin::LockAll()
	{
		AlwaysAssert(!srd3dRenderer.bIsBusy());
		CRasterVid::Lock();
	}

	//******************************************************************************************
	bool CRasterWin::CPriv::bConstructD3D(HWND hwnd, int i_width, int i_height, int i_bits)
	{
		if (GetWindowModeConfigured() == WindowMode::EXCLUSIVE)
			return bConstructD3DExclusive(hwnd, i_width, i_height, i_bits);
		else
			return bConstructD3DWindowed(hwnd, i_width, i_height, i_bits);
	}

	bool CRasterWin::CPriv::bConstructD3DExclusive(HWND hwnd, int i_width, int i_height, int i_bits)
	{
		AlwaysAssert(hwnd);
		AlwaysAssert(i_width > 0);
		AlwaysAssert(i_height > 0);
		AlwaysAssert(i_bits == 16);

		DDPIXELFORMAT ddpf_zbuffer;

		// Fail this function if Direct3D use is not set in the registry.
		if (!bGetD3D() || !bFullScreen || !DirectDraw::pdd4)
		{
			PrintD3D("\n***D3D Error***: Registry entries incorrect.\n");
			return false;
		}

		// Find the z buffer format.
		if (!priv_self.bFindZBufferFormat(ddpf_zbuffer))
		{
			PrintD3D("\n***D3D Error***: Z buffer format not found.\n");
			return false;
		}

		CDDSize<DDSURFACEDESC2> ddsd;
		CDDSize<DDSURFACEDESC2> sd;
		HRESULT hres;

		DWORD dw_flags = DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE;
		DDDEVICEIDENTIFIER dddevid;
		bool b_identifier_found = true;

		// Get the type of device and the driver.
		hres = DirectDraw::pdd4->GetDeviceIdentifier(&dddevid, 0);
		if (FAILED(hres))
			b_identifier_found = false;

		hres = DirectDraw::pdd4->SetCooperativeLevel(hwnd, dw_flags);
		if (FAILED(hres))
		{
			PrintD3D("\n***D3D Error***: Could not set cooperative level.\n");
			AlwaysAssert(0);
			return false;
		}

		hres = DirectDraw::pdd4->SetDisplayMode(i_width, i_height, i_bits, 0, 0);
		if (FAILED(hres))
		{
			PrintD3D("\n***D3D Error***: Could not set display mode.\n");
			AlwaysAssert(0);
			return false;
		}

		//
		// Currently triple buffering does not appear to work on the Voodoo 2.
		//
		bool b_triple = bGetTripleBuffer();

		if (b_triple && b_identifier_found)
		{
			GUID guid_dd = ReadDDGUID();
			GUID guid_zero;

			memset(&guid_zero, 0, sizeof(GUID));
			if (memcmp(&guid_dd, &guid_zero, sizeof(GUID)) != 0)
			{
				//
				// The device has a non-zero guid, it could be a Voodoo or Voodoo 2.
				//
				// Notes:
				//		Use the device ID to find a string containing the letters '3dfx.'
				//
				b_triple = false;
			}
		}

		if (b_triple)
		{
			PrintD3D("Triple buffering is on.");
		}
		else
		{
			PrintD3D("Triple buffering is off.");
		}

		// Get the primary display surface.
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.dwBackBufferCount = (b_triple) ? (2) : (1);
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;

		hres = DirectDraw::pdd4->CreateSurface(&ddsd, &pddsPrimary4, NULL);

		// If the triple buffering fails, attempt to create a double buffered chain.
		if (FAILED(hres))
		{
			ddsd.dwBackBufferCount = 1;
			hres = DirectDraw::pdd4->CreateSurface(&ddsd, &pddsPrimary4, NULL);
			if (FAILED(hres))
			{
				PrintD3D2("Buffer creation failed %x\n", hres);
				AlwaysAssert(0);
				return false;
			}
		}

		// Create a backbuffer.
		{
			DDSCAPS2 ddsCaps;
			ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
			hres = pddsPrimary4->GetAttachedSurface(&ddsCaps, &pddsDraw4);
			if (FAILED(hres))
			{
				PrintD3D2("Buffer creation failed %x\n", hres);
				AlwaysAssert(0);
				return false;
			}
		}

		// Create the Z buffer.
		if (!priv_self.bConstructZBuffer(ddpf_zbuffer))
		{
			PrintD3D("\n***D3D Error***: Could not create the Z buffer.\n");
			AlwaysAssert(0);
			return false;
		}

		// Set required data members.
		iWidthFront = i_width;
		iHeightFront = i_height;
		iBuffers = 2;
		bFullScreen = true;
		bFlippable = true;

		// Indicate success.
		AlwaysAssert(pddsDraw4 && pddsPrimary4);
		return pddsDraw4 && pddsPrimary4;
	}

	bool CRasterWin::CPriv::bConstructD3DWindowed(HWND hwnd, int i_width, int i_height, int i_bits)
	{
		AlwaysAssert(hwnd);
		AlwaysAssert(i_width > 0);
		AlwaysAssert(i_height > 0);
		AlwaysAssert(i_bits == 16);

		DDPIXELFORMAT ddpf_zbuffer;

		// Fail this function if Direct3D use is not set in the registry.
		if (!bGetD3D() || !bFullScreen || !DirectDraw::pdd4)
		{
			PrintD3D("\n***D3D Error***: Registry entries incorrect.\n");
			return false;
		}

		// Find the z buffer format.
		if (!priv_self.bFindZBufferFormat(ddpf_zbuffer))
		{
			PrintD3D("\n***D3D Error***: Z buffer format not found.\n");
			return false;
		}

		CDDSize<DDSURFACEDESC2> ddsd;
		HRESULT hres;

		DWORD dw_flags = DDSCL_NORMAL;

		hres = DirectDraw::pdd4->SetCooperativeLevel(hwnd, dw_flags);
		if (FAILED(hres))
		{
			PrintD3D("\n***D3D Error***: Could not set cooperative level.\n");
			AlwaysAssert(0);
			return false;
		}

		// Get the primary display surface.
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		hres = DirectDraw::pdd4->CreateSurface(&ddsd, &pddsPrimary4, NULL);
		if (FAILED(hres))
		{
			PrintD3D2("Buffer creation failed %x\n", hres);
			AlwaysAssert(0);
			return false;
		}

		// Create a backbuffer.
		{
			CDDSize<DDSURFACEDESC2> backbufferddsd;
			backbufferddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
			backbufferddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

			backbufferddsd.dwWidth = i_width;
			backbufferddsd.dwHeight = i_height;
			
			hres = DirectDraw::pdd4->CreateSurface(&backbufferddsd, &pddsDraw4, nullptr);
			if (FAILED(hres))
			{
				PrintD3D2("Buffer creation failed %x\n", hres);
				AlwaysAssert(0);
				return false;
			}
		}

		// Create the Z buffer.
		if (!priv_self.bConstructZBuffer(ddpf_zbuffer))
		{
			PrintD3D("\n***D3D Error***: Could not create the Z buffer.\n");
			AlwaysAssert(0);
			return false;
		}

		// Set required data members.
		iWidthFront  = i_width;
		iHeightFront = i_height;
		iBuffers     = 2;
		bFullScreen  = false;
		bFlippable   = false;

		// Indicate success.
		AlwaysAssert(pddsDraw4 && pddsPrimary4);
		return pddsDraw4 && pddsPrimary4;
	}

	//******************************************************************************************
	bool CRasterWin::CPriv::bFindZBufferFormat(DDPIXELFORMAT& rddpf_zbuffer)
	{
		GUID guid = ReadD3DGUID();

		// Get a valid Z buffer format.
		ZeroMemory(&rddpf_zbuffer, sizeof(DDPIXELFORMAT));
		rddpf_zbuffer.dwFlags = DDPF_ZBUFFER;
		d3dDriver.pGetD3D()->EnumZBufferFormats(guid, EnumZBufferFormats, (VOID*)&rddpf_zbuffer);
		if (!rddpf_zbuffer.dwSize)
		{
			ZeroMemory(&rddpf_zbuffer, sizeof(DDPIXELFORMAT));
			return false;
		}
		return true;
	}

	//******************************************************************************************
	bool CRasterWin::CPriv::bConstructZBuffer(const DDPIXELFORMAT& ddpf_zbuffer)
	{
		CDDSize<DDSURFACEDESC2> sd_back;
		CDDSize<DDSURFACEDESC2> sd_z;
		HRESULT hRes;

		// Get a description of the backbuffer.
		DirectDraw::err = pddsDraw4->GetSurfaceDesc(&sd_back);
 
        // Create the z-buffer. 
        sd_z.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT; 
        sd_z.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY; 
        sd_z.dwWidth        = sd_back.dwWidth; 
        sd_z.dwHeight       = sd_back.dwHeight;

		// Set the pixel information.
		memcpy(&sd_z.ddpfPixelFormat, &ddpf_zbuffer, sizeof(DDPIXELFORMAT));

		// Create the Z buffer surface.
        hRes = DirectDraw::pdd4->CreateSurface(&sd_z, &pddsZBuffer, NULL); 
        if (FAILED(hRes) || !pddsZBuffer) 
            return false; 
 
        // Attach it to the rendering target. 
        hRes = pddsDraw4->AddAttachedSurface(pddsZBuffer);
        if (FAILED(hRes)) 
            return false; 

		// Indicate success.
		return true;
	}

	//******************************************************************************************
	void CRasterWin::ClearSubRect(TPixel pix)
	{
		bool b_waslocked = bLocked;

		// Clear the Z buffer if required.
		d3dDriver.ClearZBuffer();

		if (eClearMethod == ecmDWI)
		{
			if (!b_waslocked)
				Lock();
			uint16* pu2  = (uint16*)pSurface;

			for (int i_y = 0; i_y < iHeight; i_y++, pu2 += iLinePixels)
			{
				::Fill(pu2, iWidth, uint16(pix));
			}
			if (!b_waslocked)
				Unlock();
			return;
		}

		//
		// DirectDraw cannot Clear or Blit when the surface is locked.
		// So we need to detect when the surface is locked, unlock it, do the blit, then
		// relock it if necessary.  What a waste.
		//
		Unlock();

		// Use DirectDraw blitting.
		// Set up a blit fx struct to hold the fill colour.
		CDDSize<DDBLTFX> fx;
		fx.dwFillColor = pix;

		RECT rect;
		
		rect.left   = iOffsetX;
		rect.top    = iOffsetY;
		rect.right  = iOffsetX + iWidth;
		rect.bottom = iOffsetY + iHeight;

		// Call Blt, with the COLORFILL operation.
		while (bRestore(pddsDraw4->Blt
			(
				&rect,							// Dest rectangle (0 means whole surface.)
				0,								// Source surface (none).
				0,								// Source rectangle (none).
				DDBLT_WAIT | DDBLT_COLORFILL,	// Blit type.
				&fx								// Blit params.
			)
		));

		// Relock the surface.
		if (b_waslocked)
			Lock();
	}
