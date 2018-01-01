/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Direct3D query functions.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/Direct3DQuery.cpp                                             $
 * 
 * 38    9/23/98 12:19p Pkeet
 * Added code to get rid of the test pattern.
 * 
 * 37    9/04/98 9:54p Pkeet
 * Fixed bug in code to find the centre of a pixel.
 * 
 * 36    9/04/98 7:36p Pkeet
 * Fixed bug in corner selection.
 * 
 * 35    9/04/98 7:12p Pkeet
 * Added a more robust screen offset function.
 * 
 * 34    8/22/98 8:52p Pkeet
 * Textures for the conformance test now do not specify which memory to get textures from. This
 * fixes the gruesome bug that caused a crash or undefined behaviour when switching from one
 * hardware card to another.
 * 
 * 33    8/18/98 10:34p Pkeet
 * Fixed problem with the incorrect interface being released.
 * 
 * 32    8/12/98 10:04p Pkeet
 * Changed the shared buffer test so that shared buffers would not be enabled unless region
 * uploads are enabled. This fixed a bug on the Fire GL 1000 Pro.
 * 
 * 31    8/11/98 10:09p Pkeet
 * Finished region upload test.
 * 
 * 30    8/11/98 8:24p Pkeet
 * Added code to build test textures.
 * 
 * 29    8/11/98 7:16p Pkeet
 * Improved the pixel centre finding code.
 * 
 * 28    8/11/98 6:17p Pkeet
 * Removed the settings structure.
 * 
 * 27    8/10/98 11:30p Pkeet
 * Added a test for finding out if a card lights the pixel based on its centre or on its corner.
 * 
 * 26    8/06/98 5:11p Pkeet
 * Alpha is now reported on a different flag.
 * 
 * 25    7/30/98 4:06p Pkeet
 * Added the 'bSlowCreate' flag.
 * 
 * 24    7/23/98 6:20p Pkeet
 * Added code for detecting common formats for conversion.
 * 
 * 23    7/20/98 10:32p Pkeet
 * Upgraded to Direct3D 4's interface.
 * 
 * 22    7/09/98 7:22p Pkeet
 * Fixed direct texture bug.
 * 
 * 21    7/09/98 3:57p Pkeet
 * Added support for the direct texture access flag.
 * 
 * 20    7/02/98 1:28p Pkeet
 * Set reserved data members to zero.
 * 
 * 19    7/01/98 3:27p Pkeet
 * Added data members for notes, chipset and driver information. Added code to dump a text file
 * of card settings.
 * 
 * 18    7/01/98 12:35p Pkeet
 * Added the 'DumpList' member function. Added code to sort the settings list.
 * 
 * 17    6/30/98 2:37p Pkeet
 * Moved 'strD3DSettingsFileName' to the header and made it a macro.
 * 
 * 16    6/30/98 2:23p Pkeet
 * Added the 'RemoveD3DSetting' member function.
 * 
 * 15    6/30/98 1:46p Pkeet
 * Added the 'strDEFAULT_CARDNAME' macro. Added a default value for 'bDither.'
 * 
 * 14    6/29/98 9:20p Pkeet
 * Changed query to rely on a configuration file instead of built-in settings.
 * 
 * 13    6/18/98 6:31p Pkeet
 * Set the minimum clip adjust value to a smaller value.
 * 
 * 12    6/16/98 1:43p Pkeet
 * Disabled cache fogging for all cards.
 * 
 * 11    6/16/98 12:59p Pkeet
 * Added regional uploads for the Voodoo 2.
 * 
 * 10    6/15/98 7:38p Pkeet
 * Increased the clip adjust value.
 * 
 * 9     6/15/98 12:45p Pkeet
 * Tweaked values.
 * 
 * 8     6/15/98 12:11p Pkeet
 * Added a class for storing individual Direct3D parameters separately.
 * 
 * 7     6/12/98 9:13p Pkeet
 * Added numerous cards.
 * 
 * 6     6/08/98 4:52p Pkeet
 * Fixed warnings.
 * 
 * 5     6/08/98 4:26p Pkeet
 * Added the 'bUseSecondaryCard' function implementation.
 * 
 * 4     6/08/98 4:16p Pkeet
 * Changed to include an enumeration for card types and strings based on the enumeration.
 * 
 * 3     1/15/98 2:30p Pkeet
 * Added a function to detect the PowerVR.
 * 
 * 2     1/09/98 6:25p Pkeet
 * Added a function to detect a 3DFX card.
 * 
 * 1     1/09/98 11:42a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/


//
// Necessary includes.
//
#include <math.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>
#include <DirectX/DDraw.h>
#define D3D_OVERLOADS
#include <DirectX/d3d.h>
#include "Common.hpp"
#include "Direct3DQuery.hpp"

#include "Lib/Std/Ptr.hpp"
#include "Lib/Sys/RegInit.hpp"
#include "Lib/Sys/Reg.h"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"


//
// Forward declarations.
//
class CTestTextures;
class CScreenTests;


//
// Constants.
//

// Direct3D White Colour.
const D3DCOLOR d3drgbWhite = D3DRGB(1, 1, 1);

// Default DrawPrimitive flags.
const uint32 u4DefaultFlags = D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTLIGHT | D3DDP_DONOTCLIP;


//
// Macros.
//

// Switch to display test pattern.
#define bDISPLAY_TEST (0)


//
// Local functions.
//


//
// Class definitions.
//

//*********************************************************************************************
//
class CTestTextures
//
// Object encapsulates all the functions required for testing D3D textures.
//
//**************************************
{
public:

	int iWidth;
	int iHeight;
	int iStride;
	bool bHardware;

	LPDIRECT3DTEXTURE2   d3dtexVid;
	LPDIRECTDRAWSURFACE4 pddsSurfaceVid;	// DirectDraw surface.
	LPDIRECTDRAWSURFACE4 pddsSurfaceSys;	// DirectDraw surface.

	CTestTextures(CScreenTests& st, bool b_hardware)
		: bHardware(b_hardware)
	{
		pddsSurfaceSys = 0;
		pddsSurfaceVid = 0;
		d3dtexVid      = 0;
		CDDSize<DDSURFACEDESC2> ddsd;		// Surface description.
		HRESULT hres;

		iWidth  = 128;
		iHeight = 128;

		// Set the surface description.
		ddsd.dwFlags  = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		ddsd.dwWidth  = iWidth;
		ddsd.dwHeight = iHeight;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
		ddsd.ddsCaps.dwCaps |= (b_hardware) ? (DDSCAPS_VIDEOMEMORY) : (DDSCAPS_SYSTEMMEMORY);

		// Set the pixel format.
		ddsd.ddpfPixelFormat = d3dDriver.ddpfGetPixelFormat(ed3dtexSCREEN_OPAQUE);

		// Create the direct draw texture.
		if (b_hardware)
		{
			hres = DirectDraw::pdd4->CreateSurface(&ddsd, &pddsSurfaceVid, 0);
			if (FAILED(hres))
			{
				PrintD3D2("DirectDrawError: Vid surface creation failed (%ld)\n", hres & 0x0000FFFF);
				AlwaysAssert(0);
			}
			else
			{
				PrintD3D2("Vid Surface: %x\n", (void*)pddsSurfaceVid);
			}
		}
		else
		{
			hres = DirectDraw::pdd4->CreateSurface(&ddsd, &pddsSurfaceSys, 0);
			if (FAILED(hres))
			{
				PrintD3D2("DirectDrawError: Sys surface creation failed (%ld)\n", hres & 0x0000FFFF);
				AlwaysAssert(0);
			}
			else
			{
				PrintD3D2("Vid Surface: %x\n", (void*)pddsSurfaceSys);
			}
		}
	}

	~CTestTextures()
	{
		if (pddsSurfaceSys)
			pddsSurfaceSys->Release();
		if (pddsSurfaceVid)
			pddsSurfaceVid->Release();
		if (d3dtexVid)
			d3dtexVid->Release();
	}

	//*****************************************************************************************
	//
	uint16* pu2Lock
	(
	)
	//
	// Returns a pointer to the locked front buffer.
	//
	//**************************************
	{
		AlwaysAssert(pddsSurfaceSys);

		CDDSize<DDSURFACEDESC2> sd;

		// Lock the front buffer and get a pointer to it.
		pddsSurfaceSys->Lock(0, &sd, DDLOCK_WAIT, 0);
		iStride = sd.lPitch / 2;
		return (uint16*)sd.lpSurface;
	}

	//*****************************************************************************************
	//
	void Unlock
	(
	)
	//
	// Unlocks the front buffer.
	//
	//**************************************
	{
		AlwaysAssert(pddsSurfaceSys);
		pddsSurfaceSys->Unlock(0);
	}

	//*****************************************************************************************
	//
	void LoadFrom
	(
		CTestTextures& testtexture
	)
	//
	// Loads from the sytem memory buffer to the video memory buffer.
	//
	//**************************************
	{
		AlwaysAssert(testtexture.pddsSurfaceSys);
		AlwaysAssert(pddsSurfaceVid);

		LPDIRECT3DTEXTURE2 d3dtex_sys = 0;

		// Get the texture interfaces.
		testtexture.pddsSurfaceSys->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&d3dtex_sys);
		if (!d3dtex_sys)
			return;
		if (!d3dtexVid)
			pddsSurfaceVid->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&d3dtexVid);

		// Upload.
		d3dtexVid->Load(d3dtex_sys);

		// Release unused interfaces.
		d3dtex_sys->Release();
	}

	//*****************************************************************************************
	//
	void Set
	(
	)
	//
	// Sets the texture to texture stage 0.
	//
	//**************************************
	{
		d3dDriver.pGetDevice()->SetTexture(0, d3dtexVid);
	}

	//*****************************************************************************************
	//
	void DrawRectangle
	(
		int    i_x,
		int    i_y,
		int    i_width,
		int    i_height,
		uint16 u2
	)
	//
	// Draws a rectangle of the given dimensions and fill colour.
	//
	//**************************************
	{
		uint16* pu2 = pu2Lock();
		pu2 += i_x + i_y * iStride;

		for (int a = i_y; a < i_y + i_height; ++a, pu2 += iStride)
			for (int b = 0; b < i_width; ++b)
				pu2[b] = u2;

		Unlock();
	}

	//*****************************************************************************************
	//
	void DrawRectangle
	(
		int    i_x,
		int    i_y,
		int    i_width,
		int    i_height,
		CTestTextures& testtexture
	)
	//
	// Loads from the sytem memory buffer to the video memory buffer.
	//
	//**************************************
	{
		LPDIRECTDRAWSURFACE4 pdds_sys = testtexture.pddsSurfaceSys;
		AlwaysAssert(pdds_sys);
		AlwaysAssert(pddsSurfaceVid);

		
		RECT rc;
		SetRect(&rc, i_x, i_y, i_x + i_width, i_y + i_height);

		// Blit from system memory to video memory.
		pddsSurfaceVid->Blt(&rc, pdds_sys, &rc, DDBLT_WAIT, NULL);
	}

};

//*********************************************************************************************
//
class CScreenTests
//
// Object encapsulates all the functions required for conducting screen based tests.
//
//**************************************
{
private:

	int                  iWidth;
	int                  iHeight;
	int                  iStride;
	EScreenFormats       esfScreenFormat;
	LPDIRECT3DDEVICE3    pDevice;
	LPDIRECT3DVIEWPORT3  pViewport;
	LPDIRECTDRAWSURFACE4 pddsBack;			// Pointer to the DirectDraw backbuffer.
	LPDIRECTDRAWSURFACE4 pddsFront;			// Pointer to the DirectDraw frontbuffer.

	uint16 u2TexOpRed;		// Opaque Red.
	uint16 u2TexOpGreen;	// Opaque Green.
	uint16 u2TexOpBlue;		// Opaque Blue.

	uint16 u2ScreenRed;		// Screen Red.
	uint16 u2ScreenGreen;	// Screen Green.
	uint16 u2ScreenBlue;	// Screen Blue.

public:

	//*****************************************************************************************
	//
	// CScreenTests Constructor and Destructor.
	//

	//*****************************************************************************************
	//
	CScreenTests
	(
		LPDIRECTDRAWSURFACE4 pdds_back,		// Pointer to the DirectDraw backbuffer.
		LPDIRECTDRAWSURFACE4 pdds_front		// Pointer to the DirectDraw frontbuffer.
	)
	//
	// Creates the necessary structures for the tests.
	//
	//**************************************
	{
		pddsBack  = pdds_back;
		pddsFront = pdds_front;

		AlwaysAssert(pddsBack);
		AlwaysAssert(pddsFront);
		AlwaysAssert(d3dDriver.pGetDevice());
		AlwaysAssert(d3dDriver.pd3dGetViewport());

		CDDSize<DDSURFACEDESC2> sd;

		// Retrieve the surface description.
		pddsBack->GetSurfaceDesc(&sd);

		pDevice         = d3dDriver.pGetDevice();
		pViewport       = d3dDriver.pd3dGetViewport();
		esfScreenFormat = d3dDriver.esfGetScreenFormat();
		iWidth          = sd.dwWidth;
		iHeight         = sd.dwHeight;
		iStride         = sd.lPitch / 2;

		PrintD3D2("Pitch: %ld bytes\n", sd.lPitch);

	#if bDISPLAY_TEST
		Clear();
		pddsFront->Flip(pddsBack, DDFLIP_WAIT);
	#endif
		Clear();

		SetColours();
		SetDefaults();
	}

	//*****************************************************************************************
	//
	~CScreenTests
	(
	)
	//
	// Deletes the previously created structures used for the tests.
	//
	//**************************************
	{
	#if !bDEBUG_DIRECT3D
		// For cleanliness.
		Clear();
	#if bDISPLAY_TEST
		pddsFront->Flip(pddsBack, DDFLIP_WAIT);
	#endif
	#endif // bDEBUG_DIRECT3D
	}


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void SetDefaults
	(
	)
	//
	// Sets defaults for testing.
	//
	//**************************************
	{
		pDevice->SetTexture(0, 0);
		pDevice->SetTextureStageState(0, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);
		pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
		pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFG_POINT);

		pDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
		pDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
		pDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
		pDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);
		pDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
		pDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,  0x00001000);
		pDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER);
		pDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);
		pDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_GREATEREQUAL);
		pDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
		pDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRENDERSTATE_ZBIAS, 0);
		pDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, FALSE);
		pDevice->SetRenderState(D3DRENDERSTATE_FILLMODE,        D3DFILL_SOLID);
		pDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,        D3DCULL_NONE);
		pDevice->SetRenderState(D3DRENDERSTATE_SUBPIXEL,        TRUE);
		pDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
		pDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);
	}

	//*****************************************************************************************
	//
	void SetColours
	(
	)
	//
	// Sets colours for testing.
	//
	//**************************************
	{
		// Set texture colours.
		switch (d3dDriver.d3dcomGetCommonFormat(ed3dtexSCREEN_OPAQUE))
		{
			case ed3dcom555_RGB:
				u2TexOpRed   = 0x7C00;
				u2TexOpGreen = 0x03E0;
				u2TexOpBlue  = 0x001F;
				break;

			case ed3dcom565_BGR:
				u2TexOpRed   = 0x001F;
				u2TexOpGreen = 0x07E0;
				u2TexOpBlue  = 0xF800;
				break;

			case ed3dcom565_RGB:
			default:
				u2TexOpRed   = 0xF800;
				u2TexOpGreen = 0x07E0;
				u2TexOpBlue  = 0x001F;
	}

		// Set screen colours.
		switch (d3dDriver.esfGetScreenFormat())
		{
			case esf555:
				u2ScreenRed   = 0x7C00;
				u2ScreenGreen = 0x03E0;
				u2ScreenBlue  = 0x001F;
				break;

			case esf565:
			default:
				u2ScreenRed   = 0xF800;
				u2ScreenGreen = 0x07E0;
				u2ScreenBlue  = 0x001F;
		}
	}

	//*****************************************************************************************
	//
	void Clear
	(
	)
	//
	// Clears the backbuffer and the z buffer.
	//
	//**************************************
	{
		D3DRECT d3drect;

		// Clear the surface.
		d3drect.lX1 = 0; 
		d3drect.lX2 = iWidth; 
		d3drect.lY1 = 0; 
		d3drect.lY2 = iHeight;
		pViewport->Clear2(1, &d3drect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 0.0f, 0);
	}

	//*****************************************************************************************
	//
	void DrawSquare
	(
		float f_corner_x,	// X and Y coordinates for the top left corner.
		float f_corner_y,	// X and Y coordinates for the top left corner.
		float f_size	// Width and height.
	)
	//
	// Used 'DrawPrimitive' to draw a rectangle of the given size.
	//
	//**************************************
	{
		D3DTLVERTEX atlv[4];
		D3DTLVERTEX atlv_main;

		// Set up vertices.
		atlv_main.sx       = f_corner_x;
		atlv_main.sy       = f_corner_y;
		atlv_main.sz       = 0.5;
		atlv_main.rhw      = 1.0 / 0.5;
		atlv_main.color    = d3drgbWhite;
		atlv_main.tu       = 0.0;
		atlv_main.tv       = 0.0;
		atlv_main.specular = d3drgbWhite;

		atlv[0] = atlv_main;
		atlv[1] = atlv_main;
		atlv[2] = atlv_main;
		atlv[3] = atlv_main;

		atlv[1].sx += f_size;
		atlv[1].tu = 1.0f;

		atlv[2].sx = atlv[1].sx;
		atlv[2].sy += f_size;
		atlv[2].tu = 1.0f;
		atlv[2].tv = 1.0f;

		atlv[3].sy = atlv[2].sy;
		atlv[3].tv = 1.0f;

		// Draw the rectangle.
		pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, (LPVOID)atlv, 4, u4DefaultFlags);
	}

	//*****************************************************************************************
	//
	uint16* pu2Lock
	(
	)
	//
	// Returns a pointer to the locked front buffer.
	//
	//**************************************
	{
		CDDSize<DDSURFACEDESC2> sd;

		// Lock the front buffer and get a pointer to it.
	#if bDISPLAY_TEST
		pddsFront->Lock(0, &sd, DDLOCK_WAIT, 0);
	#else
		pddsBack->Lock(0, &sd, DDLOCK_WAIT, 0);
	#endif
		return (uint16*)sd.lpSurface;
	}

	//*****************************************************************************************
	//
	void Unlock
	(
	)
	//
	// Unlocks the front buffer.
	//
	//**************************************
	{
	#if bDISPLAY_TEST
		pddsFront->Unlock(0);
	#else
		pddsBack->Unlock(0);
	#endif
	}

	//*****************************************************************************************
	//
	bool bScanForColour
	(
		int i_x,
		int i_y,
		int i_width,
		int i_height,
		uint16 u2_col
	)
	//
	// Returns 'true' if the colour is found within the region.
	//
	//**************************************
	{
		uint16* pu2 = pu2Lock();
		pu2 += i_x + i_y * iStride;

		for (int j = 0; j < i_height; ++j, pu2 += iStride)
			for (int i = 0; i < i_width; ++i)
				if (pu2[i] & u2_col)
				{
					Unlock();
					return true;
				}

		Unlock();
		return false;
	}

	//*****************************************************************************************
	//
	bool bScanForNotColour
	(
		int i_x,
		int i_y,
		int i_width,
		int i_height,
		uint16 u2_col
	)
	//
	// Returns 'true' if any pixels without the desired colour are within the region.
	//
	//**************************************
	{
		uint16* pu2 = pu2Lock();
		pu2 += i_x + i_y * iStride;

		for (int j = 0; j < i_height; ++j, pu2 += iStride)
			for (int i = 0; i < i_width; ++i)
				if ((pu2[i] & u2_col) == 0)
				{
					Unlock();
					return true;
				}

		Unlock();
		return false;
	}

	//*****************************************************************************************
	//
	void TestOffset
	(
		bool&  rb_x,
		bool&  rb_y,
		int    i_start,
		int    i_midpoint,
		float  f_offset,
		uint16 u2_mask
	);
	//
	// Sets the rb_x and rb_y flags to false if a pixel is found. Sets the screen offset
	// accordingly.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetBaseScreenOffset
	(
	);
	//
	// Use to set the 'fOffsetXBase' and 'fOffsetYBase' values of 'CScreenRenderAuxD3D.'
	//
	// Notes:
	//		Most cards sample the centre of a screen pixel to light the screen pixel; these cards
	//		require an offset of 0.0. Some cards, despite convention, sample the top left corner
	//		of the pixel to light the pixel. These cards require an offset of 0.5.
	//
	//		This function determines the offset by writing a precisely positioned polygon on the
	//		screen and then reading back the pixels.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetRegionUploads
	(
	);
	//
	// Determines if region uploads and shared buffers may be used for page-managed textures.
	//
	//**************************************

};


//
// Global functions.
//

//*********************************************************************************************
HRESULT WINAPI D3DEnumTextureFormatCallback(LPDDPIXELFORMAT ppix, LPVOID lpUserArg)
{
	// Do nothing if no surface description is present.
	if (!ppix)
		return D3DENUMRET_OK;

	DDPIXELFORMAT* pddpf = (DDPIXELFORMAT*)lpUserArg;
	
	// 16 bit format without alpha.
	if (ppix->dwRGBBitCount == 16 && 
		(ppix->dwFlags & DDPF_RGB) &&
		!(ppix->dwFlags & DDPF_ALPHAPIXELS))
	{
		pddpf[ed3dtexSCREEN_OPAQUE] = *ppix;
		//pddpf[ed3dtexSCREEN_TRANSPARENT] = *ppix;
	}
	
	// 16 bit format with alpha.
	if (ppix->dwRGBBitCount == 16 && 
		(ppix->dwFlags & DDPF_RGB) &&
		(ppix->dwFlags & DDPF_ALPHAPIXELS))
	{
		// If this pixel format has previously been found, select the best one.
		if (pddpf[ed3dtexSCREEN_ALPHA].dwSize)
		{
			// The best format has the most alpha bit depth.
			if (iCountBits(ppix->dwRGBAlphaBitMask) >
				iCountBits(pddpf[ed3dtexSCREEN_ALPHA].dwRGBAlphaBitMask))
			{
				pddpf[ed3dtexSCREEN_ALPHA] = *ppix;
			}
		}
		else
		{
			pddpf[ed3dtexSCREEN_ALPHA] = *ppix;
		}
	}
	
	// 16 bit format with transparency, or 1 bit alpha.
	if (ppix->dwRGBBitCount == 16 && 
		(ppix->dwFlags & DDPF_RGB) &&
		(ppix->dwFlags & DDPF_ALPHAPIXELS))
	{
		// The best format has the most alpha bit depth.
		if (iCountBits(ppix->dwRGBAlphaBitMask) == 1)
		{
			pddpf[ed3dtexSCREEN_TRANSPARENT] = *ppix;
		}
	}

	return D3DENUMRET_OK;
}

//*********************************************************************************************
void CScreenTests::TestOffset(bool& rb_x, bool&  rb_y, int i_start, int i_midpoint,
							  float f_offset, uint16 u2_mask)
{
	uint16* pu2_left;
	uint16* pu2_top;
	uint16* pu2 = pu2Lock();
	AlwaysAssert(pu2);

	// Get a left side pixel.
	pu2_left = pu2 + i_start + i_midpoint * iStride;

	// Get a topside pixel.
	pu2_top = pu2 + i_midpoint + i_start * iStride;

	// Set the side values.
	if (!rb_x && (*pu2_left & u2_mask) == 0)
	{
		rb_x = true;
		CScreenRenderAuxD3D::fOffsetXBase = f_offset - 1.0f;
	}

	// Set the side values.
	if (!rb_y && (*pu2_top & u2_mask) == 0)
	{
		rb_y = true;
		CScreenRenderAuxD3D::fOffsetYBase = f_offset - 1.0f;
	}

	// Unlock the front buffer.
	Unlock();
}

//*********************************************************************************************
void CScreenTests::SetBaseScreenOffset()
{
	//
	// Notes:
	//
	//		The software rasterizer assumes that a bias value of 0.5 has been applied to the x
	//		and y screen values of the coordinate so that it can light or not light the centre
	//		of the pixel by truncating each value. This has the effect of making the pixel
	//		light or not light if its top left corner is inside or outside of the rendering
	//		triangle. To match the card's pixel picking strategy to the real pixel picking
	//		strategy of the software renderer, an offset must be found to make the card light
	//		the top left corner of the pixel in the same way.
	//
	//		This is done by applying an increment until a lit pixel is no longer lit. This
	//		value is then shifted over by one pixel.
	//
#if bDISPLAY_TEST
	pddsFront->Flip(pddsBack, DDFLIP_WAIT);
	Clear();
	pddsFront->Flip(pddsBack, DDFLIP_WAIT);
	Clear();
	pddsFront->Flip(pddsBack, DDFLIP_WAIT);
#else
	Clear();
#endif
	bool b_side_x = false;
	bool b_side_y = false;

	CTestTextures tex_sys(*this, false);
	CTestTextures tex_vid0(*this, true);

	tex_sys.DrawRectangle(0, 0, 128, 128, u2TexOpRed);
	tex_vid0.LoadFrom(tex_sys);

	uint16 u2_mask = (u2ScreenRed << 2) & u2ScreenRed;

	const float fCornerStart     = 16.0f;
	const float fCornerIncrement = 0.05f;
	const int   iCornerStart     = int(fCornerStart);

	float f_offset  = -1.6f;
	int   iRectSize = 32;

	// Set default values.
	CScreenRenderAuxD3D::fOffsetXBase = 0.0f;
	CScreenRenderAuxD3D::fOffsetYBase = 0.0f;

	//
	// Make sure no pixels are lit to begin with.
	//
	Clear();
#if bDISPLAY_TEST
	pddsFront->Flip(pddsBack, DDFLIP_WAIT);
#endif
	TestOffset(b_side_x, b_side_y, iCornerStart, iCornerStart + iRectSize / 2, f_offset, u2_mask);
	if (!(b_side_x && b_side_y))
	{
		AlwaysAssert(0);
		CScreenRenderAuxD3D::fOffsetXBase = 0.0f;
		CScreenRenderAuxD3D::fOffsetYBase = 0.0f;
		return;
	}

	//
	// Make sure pixels will light.
	//
	// To do:
	//		Debug this routine.
	//
	/*
	Clear();
	pDevice->BeginScene();
	tex_vid0.Set();
	DrawSquare(fCornerStart - 4.0f, fCornerStart - 4.0f, float(iRectSize));
	pDevice->EndScene();
	pddsFront->Flip(pddsBack, DDFLIP_WAIT);
	TestOffset(b_side_x, b_side_y, iCornerStart, iCornerStart + iRectSize / 2, f_offset, u2_mask);
	if (b_side_x || b_side_y)
	{
		AlwaysAssert(0);
		CScreenRenderAuxD3D::fOffsetXBase = 0.0f;
		CScreenRenderAuxD3D::fOffsetYBase = 0.0f;
		return;
	}
	*/

	//
	// Draw a rectangle.
	//
	b_side_x = false;
	b_side_y = false;
	while (!b_side_x || !b_side_y)
	{
		// Prevent infinite looping.
		if (f_offset > 1.6f)
			return;

		float fCorner = fCornerStart + f_offset;
		f_offset += fCornerIncrement;

		int iCorner   = int(fCorner);
		int iMidPoint = iCorner + iRectSize / 2; 

		Clear();
		pDevice->BeginScene();
		tex_vid0.Set();
		DrawSquare(fCorner, fCorner, float(iRectSize));
		pDevice->EndScene();

		// Flip.
	#if bDISPLAY_TEST
		pddsFront->Flip(pddsBack, DDFLIP_WAIT);
	#endif

		// Test for pixels no longer being lit.
		TestOffset(b_side_x, b_side_y, iCornerStart, iMidPoint, f_offset, u2_mask);
	}

	// Debug print.
	PrintD3D2("X Centre %1.2f\n", CScreenRenderAuxD3D::fOffsetXBase);
	PrintD3D2("Y Centre %1.2f\n", CScreenRenderAuxD3D::fOffsetYBase);
}

//*********************************************************************************************
void CScreenTests::SetRegionUploads()
{
	CTestTextures tex_sys(*this, false);
	CTestTextures tex_vid0(*this, true);

	tex_sys.DrawRectangle(0, 0, 128, 128, u2TexOpRed);
	tex_vid0.LoadFrom(tex_sys);
	tex_sys.DrawRectangle(0, 0, 128, 128, u2TexOpGreen);
	tex_sys.DrawRectangle(32, 32, 32, 64, u2TexOpBlue);
	tex_vid0.DrawRectangle(32, 32, 32, 64, tex_sys);

	pDevice->BeginScene();

	tex_vid0.Set();
	DrawSquare(0, 0, 128);

	pDevice->EndScene();
#if bDISPLAY_TEST
	pddsFront->Flip(pddsBack, DDFLIP_WAIT);
#endif

	d3dDriver.bRegionUploads    = !bScanForNotColour(32, 32, 32, 64, u2ScreenBlue);
	d3dDriver.bSharedSysBuffers = !bScanForColour(0, 0, 128, 128, u2ScreenGreen) && d3dDriver.bRegionUploads;

#if bDEBUG_DIRECT3D
	if (d3dDriver.bRegionUploads)
		PrintD3D("\nRegion Uploads Supported\n");
	else
		PrintD3D("\nRegion Uploads Unupported\n");
	if (d3dDriver.bSharedSysBuffers)
		PrintD3D("Shared buffers Supported\n");
	else
		PrintD3D("Shared buffers Unupported\n");
#endif // bDEBUG_DIRECT3D
}

//*********************************************************************************************
void ScreenTests(LPDIRECTDRAWSURFACE4 pdds_back, LPDIRECTDRAWSURFACE4 pdds_front)
{
	CScreenTests screentests(pdds_back, pdds_front);
	PrintD3D("\n\n");

	//
	// Tests.
	//
	screentests.SetBaseScreenOffset();
	screentests.SetRegionUploads();
	
	PrintD3D("\n\n");
}