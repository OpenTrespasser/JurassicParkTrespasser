/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of DDDevice.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Trespass/DDDevice.cpp                                                 $
 * 
 * 31    9/19/98 1:12a Pkeet
 * Fixed bug that was incorrectly detecting small memory cards.
 * 
 * 30    9/19/98 1:06a Pkeet
 * Changed criteria for small memory.
 * 
 * 29    9/14/98 7:59p Pkeet
 * 512x384 screen resolution is suppressed for Rendition V1000 based cards.
 * 
 * 28    9/07/98 7:44p Shernd
 * fix for wrong driver name in video dialog
 * 
 * 27    9/05/98 7:53p Pkeet
 * Added support for the Matrox G100.
 * 
 * 26    9/02/98 6:27p Pkeet
 * Trident3D cards fail to be detected.
 * 
 * 25    8/29/98 3:52p Pkeet
 * Added data members to detect cards that should use smaller textures.
 * 
 * 24    8/28/98 8:25p Pkeet
 * Added code to specifically recognize a card based on the Permedia 2 chipset and allow it to
 * ignore the alpha compare test.
 * 
 * 23    8/24/98 10:24p Pkeet
 * Application now prints out additional information during card and Direct3D enumerations.
 * 
 * 22    8/23/98 2:24a Pkeet
 * Upped the enumerated resolutions to 800x600.
 * 
 * 21    8/22/98 11:55p Pkeet
 * Filters out non-supported cards.
 * 
 * 20    8/22/98 6:38p Pkeet
 * Added dprintf calls as a diagnostic tool.
 * 
 * 19    8/12/98 10:11p Pkeet
 * Disabled requiring the blend source alpha caps bit be set for hardware water to work.
 * 
 * 18    8/06/98 7:28p Pkeet
 * Added the 'bModulatedAlpha' flag. Added the 'bGetModulatedAlpha' function.
 * 
 * 17    8/05/98 10:10p Pkeet
 * Changed the way cards are listed.
 * 
 * 16    8/05/98 9:40p Pkeet
 * The Z buffer bit depth is now determined and stored in the registry. Moved the
 * 'iZBufferBitDepth' data member to the Direct3D structure. Removed unnecessary code.
 * 
 * 15    8/05/98 9:09p Pkeet
 * 
 * 14    7/20/98 10:42p Pkeet
 * Changed devices to work with new DirectX 6.0 interfaces.
 * 
 * 13    7/20/98 10:24p Pkeet
 * 
 * 12    7/05/98 8:05p Pkeet
 * Added code to prevent the enumeration of display modes greater than 640x480.
 * 
 * 11    7/05/98 7:00p Pkeet
 * Added a release to complement a DirectDraw device creation.
 * 
 * 10    6/29/98 9:19p Pkeet
 * Simplified code to find GUIDs for DirectDraw and Direct3D.
 * 
 * 9     6/15/98 7:48p Shernd
 * Make builds for machines that don't have c:\jp2_pc as the build environment
 * 
 * 8     6/15/98 12:10p Pkeet
 * Added includes for compiling with 'InitGUIApp.'
 * 
 * 7     6/09/98 9:22a Shernd
 * Removing Refresh Rates for EnumDisplayMode
 * 
 * 6     6/08/98 4:16p Pkeet
 * Added commented out includes.
 * 
 * 5     1/26/98 1:45p Shernd
 * Adding setup for precompiled headers
 * 
 * 4     12/15/97 2:29p Shernd
 * Don't GPF when d3d can't be initialized.
 * 
 * 3     12/09/97 6:48p Pkeet
 * Added a query to get the direct draw 2 interface.
 * 
 * 2     12/09/97 6:21p Pkeet
 * Added code to set and get D3D driver information.
 * 
 * 1     12/08/97 6:36p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef TRESPASS
#include <windows.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#else
#include "precomp.h"
#include "resource.h"
#pragma hdrstop
#include <assert.h>
#endif
#include "../Lib/Sys/RegInit.hpp"
#include "../Lib/Sys/Reg.h"

#if (1)
	#include "/jp2_pc/inc/DirectX/ddraw.h"
	#define D3D_OVERLOADS
	#include "/jp2_pc/inc/DirectX/d3d.h"
#endif

#include "Lib/Sys/DebugConsole.hpp"
#include "DDDevice.hpp"

BOOL bSecondaryCard = FALSE;
BOOL bDeviceFound   = FALSE;
BOOL bIgnoreAlpha   = FALSE;
bool bSuppress512   = false;

#ifdef TRESPASS
extern HINSTANCE    g_hInst;
#endif


//
// Function prototypes.
//

//*********************************************************************************************
//
HRESULT WINAPI D3DEnumCallback
(
	LPGUID          lpGUID, 
	LPSTR           lpszDeviceDesc, 
	LPSTR           lpszDeviceName, 
	LPD3DDEVICEDESC lpd3dHWDeviceDesc, 
	LPD3DDEVICEDESC lpd3dSWDeviceDesc, 
	LPVOID          lpUserArg                 
);
//
// Enumeration callback for direct3D drivers.
//
//**************************************

//**********************************************************************************************
//
BOOL WINAPI DDEnumCallbackAuto
(
	GUID FAR* lpGUID,
	LPSTR     lpDriverDescription,
	LPSTR     lpDriverName,
	LPVOID    lpContext
);
//
// Callback used to enumerate DirectDraw devices.
//
//**************************************

//**********************************************************************************************
//
static void PrintGUID
(
	LPGUID lpGUID
)
//
// Outputs the GUID as a string to the debug window.
//
//**************************************
{
	if (!lpGUID)
	{
		dprintf("GUID: 0\n");
		return;
	}
	dprintf("GUID: %8x-%4x-%4x-", lpGUID->Data1, lpGUID->Data2, lpGUID->Data3);
	for (int i = 0; i < 8; ++i)
		dprintf("%2x", lpGUID->Data4[i]);
	dprintf("\n");
}



//
// Function implementations.
//

//**********************************************************************************************
//
static HRESULT CALLBACK EnumDisplayMode
(
	DDSURFACEDESC* pddsd,		// Surface descriptor for a screen mode.
	void*          pv_context	// Resolution array.
)
// 
// Callback for IDirectDraw::EnumDisplayModes().  Adds the display mode 
// in ascrmdList via AddScreenMode().
//
// Returns DDENUMRET_OK to indicate success.
//
//**********************************
{
	assert(pddsd);
	assert(pv_context);

	// If the mode is not 16 bits, ignore it.
	if (pddsd->ddpfPixelFormat.dwRGBBitCount != 16)
		return DDENUMRET_OK;

	// Suppress resolutions if required.
	if (bSuppress512 && pddsd->dwWidth == 512)
		return DDENUMRET_OK;

	// Do not enumerate screen resolutions higher than 640x480.
	if (pddsd->dwWidth > 800)
		return DDENUMRET_OK;

	// Get the next available element.
	SResolution* pr = &((SResolution*)pv_context)[SResolution::iResolutionsCount++];
	assert(pr);

	// Set the width, height and refresh rate of the element.
	pr->iWidth     = int(pddsd->dwWidth);
	pr->iHeight    = int(pddsd->dwHeight);
	pr->iRefreshHz = int(pddsd->dwRefreshRate);
	return DDENUMRET_OK;
}

//**********************************************************************************************
BOOL bFindResolutions(GUID& guid, SResolution* ar, int& ri_num_resolutions)
{
	LPDIRECTDRAW pdd = 0;	// DirectDraw device.
	BOOL         b_created;
	EVideoCard   evc = evcUnknown;

	// Initialize the device count.
	SResolution::iResolutionsCount = 0;

	// Create the direct draw device.
	b_created = DirectDrawCreate(pConvertGUID(guid), &pdd, 0) == DD_OK;
	assert(b_created);
	assert(pdd);


	// Get the video card type.
	AlwaysAssert(pdd);

	LPDIRECTDRAW4 pdd_4 = 0;

	HRESULT hres = pdd->QueryInterface(IID_IDirectDraw4, (LPVOID*)&pdd_4);
	if (pdd_4 && !FAILED(hres))
	{
		evc = evcGetCard(pdd_4);
		pdd_4->Release();
	}

	// Suppress resolutions based on the video card type.
	bSuppress512 = false;
	switch (evc)
	{
		case evcRenditionV1000:
			bSuppress512 = true;
			break;
	}

	// Enumerate resolutions.
	pdd->EnumDisplayModes(0, 0, (void*)ar, EnumDisplayMode);

	// Release the direct draw device.
	pdd->Release();

	//
	// To do:
	//		Verify there is enough video memory for the selected video mode and for textures
	//		that may be used by direct draw.
	//

	// Copy the resolutions count.
	ri_num_resolutions = SResolution::iResolutionsCount;
	return TRUE;
}






//**********************************************************************************************
DWORD FlagsToBitDepth(DWORD dwFlags) 
{ 
    if (dwFlags & DDBD_1) 
        return 1; 
    else if (dwFlags & DDBD_2) 
        return 2; 
    else if (dwFlags & DDBD_4) 
        return 4; 
    else if (dwFlags & DDBD_8) 
        return 8; 
    else if (dwFlags & DDBD_16) 
        return 16; 
    else if (dwFlags & DDBD_24) 
        return 24; 
    else if (dwFlags & DDBD_32) 
        return 32; 
    else 
        return 0; 
} 

	
//*****************************************************************************************
//
bool bGetModulatedAlpha(LPD3DDEVICEDESC lpd3dHWDeviceDesc)
//
// Returns 'true' if modulated alpha is supported.
//
// To do:
//		Add in a test based on the pixel format.
//
//**************************************
{
	assert(lpd3dHWDeviceDesc);

	// Test the modulate caps bit.
	if (!(lpd3dHWDeviceDesc->dwTextureOpCaps & D3DTEXOPCAPS_MODULATE))
		return false;

	// Test to ensure a 4444 texture format is supported.
	//DDPIXELFORMAT pixform = ddpfFormats[ed3dtexSCREEN_ALPHA];
	//if (iCountBits(pixform.dwRGBAlphaBitMask) != 4 || !(pixform.dwFlags & DDPF_ALPHAPIXELS))
		//return false;

	// Test if source alpha blending is supported.
	//if (!(lpd3dHWDeviceDesc->dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_SRCALPHA))
		//return false;

	// This feature is supported.
	return true;
}


//**********************************************************************************************
HRESULT WINAPI D3DEnumCallback(LPGUID lpGUID, LPSTR lpszDeviceDesc, LPSTR lpszDeviceName,
							   LPD3DDEVICEDESC lpd3dHWDeviceDesc, LPD3DDEVICEDESC lpd3dSWDeviceDesc,
							   LPVOID lpUserArg)
{
	assert(lpszDeviceName);
	assert(lpszDeviceDesc);
	assert(lpUserArg);

	// Cast to get the device slot.
	SD3DDevice* pd3d = (SD3DDevice*)lpUserArg;

	dprintf("******D3D Device found...\n");
	PrintGUID(lpGUID);
	if (lpszDeviceName)
		dprintf("%s\n", lpszDeviceName);
	if (lpszDeviceDesc)
		dprintf("%s\n", lpszDeviceDesc);

	// If the device is not in hardware, ignore it.
	if (!lpd3dHWDeviceDesc)
	{
		dprintf("Enumeration failed: device does not support hardware\n");
		return D3DENUMRET_OK;
	}
		
	// If the device does not support RGB lighting, ignore it.
	if (lpd3dHWDeviceDesc->dcmColorModel != D3DCOLOR_RGB)
	{
		dprintf("Enumeration failed: device does not support rgb\n");
		return D3DENUMRET_OK;
	}

	// If the device does not support a 16 bit bitdepth, ignore it.
	if ((lpd3dHWDeviceDesc->dwDeviceRenderBitDepth & DDBD_16) == 0)
	{
		dprintf("Enumeration failed: device does not support 16 bit\n");
		return D3DENUMRET_OK;
	}

	// If the device does not support Gouraud shading, ignore it.
	if ((lpd3dHWDeviceDesc->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB) == 0)
	{
		dprintf("Enumeration failed: device does not support gouraud rgb\n");
		return D3DENUMRET_OK;
	}

	// If the device does not support a z buffer, ignore it.
	if (!lpd3dHWDeviceDesc->dwDeviceZBufferBitDepth)
	{
		dprintf("Enumeration failed: device does not support z buffering\n");
		return D3DENUMRET_OK;
	}

	if (!bIgnoreAlpha)
	{
		// If the device does not support alpha compares, ignore it.
		if ((lpd3dHWDeviceDesc->dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_GREATER) == 0)
		{
			dprintf("Enumeration failed: device does not support alpha compares\n");
			return D3DENUMRET_OK;
		}
	}

	// Get and store the bit depth of the Z buffer.
	int i_zbuffer_bitdepth = FlagsToBitDepth(lpd3dHWDeviceDesc->dwDeviceZBufferBitDepth);
	if (!i_zbuffer_bitdepth)
	{
		dprintf("Enumeration failed: device does not support z buffering\n");
		return D3DENUMRET_OK;
	}

	// If the device does not support greater than z buffer compares, ignore it.
	if ((lpd3dHWDeviceDesc->dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_GREATEREQUAL) == 0)
	{
		dprintf("Enumeration failed: device does not support z buffer compares\n");
		return D3DENUMRET_OK;
	}

	// Copy the GUID.
	if (lpGUID)
	{
		pd3d->guidID = *lpGUID;
	}
	else
	{
		memset(&pd3d->guidID, 0, sizeof(pd3d->guidID));
	}

	// Copy the Z buffer bitdepth.
	pd3d->iZBufferBitDepth = i_zbuffer_bitdepth;

	// Set the modulated alpha flag.
	pd3d->bModulatedAlpha = bGetModulatedAlpha(lpd3dHWDeviceDesc);

	// Copy the name and device description and return.
	strcpy(pd3d->strName, lpszDeviceName);
	strcpy(pd3d->strDescription, lpszDeviceDesc);
	pd3d->bSupported = TRUE;
	dprintf("...D3D Device successful!");
	return D3DENUMRET_OK; 
}

//**********************************************************************************************
BOOL WINAPI DDEnumCallbackAuto(GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName,
							   LPVOID lpContext)
{
	assert(lpContext);
	assert(lpDriverDescription);
	assert(lpDriverName);

	SDDDevice         ddd;											// Device for this callback.
	CEnumerateDevices *penumdev = (CEnumerateDevices*)lpContext;	// Enumeration of devices.

	// Set the GUID.
	if (lpGUID)
	{
		// Just copy it.
		ddd.guidID = *lpGUID;
	}
	else
	{
		// Set it to null.
		memset(&ddd.guidID, 0, sizeof(GUID));
	}

	// Copy the name and device description.
	strcpy(ddd.strName, lpDriverName);
	strcpy(ddd.strDescription, lpDriverDescription);

	// Add the device to the list.
	penumdev->AddDDDevice(ddd);

	return DDENUMRET_OK;
}

//**********************************************************************************************
BOOL bFindD3D(GUID& guid, SD3DDevice* pd3d_device)
{
	LPDIRECTDRAW  pdd  = 0;	// DirectDraw device.
	LPDIRECTDRAW4 pdd2 = 0;	// DirectDraw2 device.
	LPDIRECT3D3   pd3d = 0;	// Direct3D3 device.
	BOOL          b_retval = FALSE;
	HRESULT       hres;

	dprintf("\n\n\n\n****************************************************************\n");
	dprintf("**************************Testing new card**********************\n");
	dprintf("****************************************************************\n");
	PrintGUID(&guid);

	// Create the direct draw device.
	hres = DirectDrawCreate(pConvertGUID(guid), &pdd, 0);
	if (FAILED(hres) || !pdd)
		return FALSE;
	dprintf(">>>>>>New direct draw created\n");

	// Get the direct draw 2 interface.
    hres = pdd->QueryInterface(IID_IDirectDraw4, (LPVOID*)&pdd2);
	if (FAILED(hres) || !pdd2)
	{
		pdd->Release();
		dprintf(">>>>>>New direct draw released\n");
		return FALSE;
	}
	dprintf(">>>>>>New direct draw 4 interface created\n");

	// Query the DirectDraw object to get the D3D object
	hres = pdd2->QueryInterface(IID_IDirect3D3, (void**)&pd3d);
	if (FAILED(hres) || !pd3d)
	{
		pdd->Release();
		pdd2->Release();
		dprintf(">>>>>>New direct draw released\n");
		dprintf(">>>>>>New direct draw 4 interface released\n");
		return FALSE;
	}
	dprintf(">>>>>>New direct 3d object created\n");

	// Restore special case flags.
	bIgnoreAlpha = FALSE;

	// Determine the card type if known.
	pd3d_device->evcVideoCard = evcGetCard(pdd2);

	// Fail some really bad cards outright.
	if (pd3d_device->evcVideoCard == evcTrident3D)
		goto FINISH_RELEASE;

	// Make exceptions for reporting for known devices.
	if (pd3d_device->evcVideoCard == evcPermedia2)
		bIgnoreAlpha = TRUE;
	if (pd3d_device->evcVideoCard == evcMatroxG100)
		bIgnoreAlpha = TRUE;

	// Enumerate d3d drivers.
	pd3d->EnumDevices(D3DEnumCallback, (void*)pd3d_device); 

	// If the enumeration failed to find a supported device, fail this driver.
	if (!pd3d_device->bSupported)
		goto FINISH_RELEASE;

	// All tests have succeeded.
	b_retval = TRUE;

	//
	// Set the small memory flag.
	//
	pd3d_device->bSmallMemory = false;
	switch (pd3d_device->evcVideoCard)
	{
		case evcATIRage:
			pd3d_device->bSmallMemory = true;
			break;
		case evcNVidia128:
			pd3d_device->bSmallMemory = true;
			break;
	};

	// Detect available texture memory.
	if (!pd3d_device->bSmallMemory)
	{
		DDSCAPS2 ddscaps;
		DWORD    dw_total;
		DWORD    dw_free;
		HRESULT  hres;

		// If nonlocal video memory is supported, assume the best.
		memset(&ddscaps, 0, sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_NONLOCALVIDMEM | DDSCAPS_TEXTURE;
		hres = pdd2->GetAvailableVidMem(&ddscaps, &dw_total, &dw_free);
		if (FAILED(hres))
			goto FINISH_RELEASE;
		if (dw_total)
			goto FINISH_RELEASE;

		// If the free texture memory is less than or equal to two megabytes, use small memory.
		memset(&ddscaps, 0, sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_TEXTURE;
		hres = pdd2->GetAvailableVidMem(&ddscaps, &dw_total, &dw_free);
		if (FAILED(hres))
			goto FINISH_RELEASE;
		if (dw_free > 2 * 1024 * 1024)
			goto FINISH_RELEASE;

		// Assume the worst.
		pd3d_device->bSmallMemory = true;
	}

FINISH_RELEASE:
	// Release the various interfaces and return.
	pd3d->Release();
	pdd2->Release();
	pdd->Release();
	dprintf(">>>>>>New direct draw released\n");
	dprintf(">>>>>>New direct draw 4 interface released\n");
	dprintf(">>>>>>New direct 3d object interface released\n");
	dprintf("****************************************************************\n");
	dprintf("*********************Finished testing new card******************\n");
	dprintf("****************************************************************\n\n\n\n");
	return b_retval;
}

//**********************************************************************************************
void CEnumerateDevices::RemoveDevice(int i)
{
	for (int j = i + 1; j < iNumDevices; ++j)
		Devices[j - 1] = Devices[j];
	--iNumDevices;
}

//**********************************************************************************************
void CEnumerateDevices::AddDDDevice(const SDDDevice& ddd)
{
	// Stop adding devices if the array is full.
	if (iNumDevices >= iMaxDevices - 1)
		return;

	// Copy the device information.
	Devices[iNumDevices].ddDevice        = ddd;
	Devices[iNumDevices].bD3DDeviceFound = false;

	// Clear the Direct3D device information.
	strcpy(Devices[iNumDevices].d3dDevice.strName, "None");
	strcpy(Devices[iNumDevices].d3dDevice.strDescription, "None");

	// Defaults.
	Devices[iNumDevices].d3dDevice.iZBufferBitDepth = 0;
	Devices[iNumDevices].d3dDevice.bSupported       = FALSE;
	Devices[iNumDevices].d3dDevice.bModulatedAlpha  = FALSE;

	++iNumDevices;
}

//**********************************************************************************************
void CEnumerateDevices::GetTitle(int i, char* str) const
{
	assert(i >= 0);
	assert(i < iNumDevices);

	CEnumerateDevices::SDevice dev = devGetDevice(i);
	strcpy(str, dev.ddDevice.strDescription);
	strcat(str, " (");
	strcat(str, dev.ddDevice.strName);
	strcat(str, ")");
}

//**********************************************************************************************
void CEnumerateDevices::SelectDevice(char* str)
{
	char str_title[2048];

	for (int i = 0; i < iNumDevices; ++i)
	{
		GetTitle(i, str_title);
		if (strcmpi(str, str_title) == 0)
		{
			iSelectedDevice = i;
			return;
		}
	}
	assert(0);
}

//**********************************************************************************************
CEnumerateDevices::CEnumerateDevices()
	: iNumDevices(0), iSelectedDevice(0)
{
	// Enumerate DirectDraw devices.
	DirectDrawEnumerate(DDEnumCallbackAuto, (void*)this);

	//
	// Attempt to find Direct3D support for each of the devices.
	//
	for (int i = 0; i < iNumDevices; ++i)
	{
		Devices[i].d3dDevice.evcVideoCard = evcUnknown;
		Devices[i].bD3DDeviceFound = bFindD3D
		(
			Devices[i].ddDevice.guidID,
			&Devices[i].d3dDevice
		);
	}
	for (i = iNumDevices - 1; i >= 0; --i)
	{
		if (!Devices[i].bD3DDeviceFound)
			RemoveDevice(i);
	}

	//
	// The last device should be Trespasser's software renderer.
	//
	{
		SDDDevice   ddd;
        char        sz[100];

		memset(&ddd.guidID, 0, sizeof(GUID));
#ifdef TRESPASS
        LoadString(g_hInst, IDS_SOFTWARE_DRIVER, sz, sizeof(sz));
#else
        strcpy(sz, "Trespasser's software driver");
#endif
		strcpy(ddd.strDescription, sz);
#ifdef TRESPASS
        LoadString(g_hInst, IDS_DISPLAY, sz, sizeof(sz));
#else
        strcpy(sz, "display");
#endif
		strcpy(ddd.strName, sz);

		Devices[iNumDevices].ddDevice        = ddd;
		Devices[iNumDevices].bD3DDeviceFound = FALSE;

#ifdef TRESPASS
        LoadString(g_hInst, IDS_TITLE, sz, sizeof(sz));
#else
        strcpy(sz, "Trespasser");
#endif
		strcpy(Devices[iNumDevices].d3dDevice.strName, sz);
#ifdef TRESPASS
        LoadString(g_hInst, IDS_SOFTWARE, sz, sizeof(sz));
#else
        strcpy(sz, "software");
#endif
		strcpy(Devices[iNumDevices].d3dDevice.strDescription, sz);

		Devices[iNumDevices].d3dDevice.iZBufferBitDepth = 0;
		Devices[iNumDevices].d3dDevice.bModulatedAlpha  = FALSE;

		iSelectedDevice = iNumDevices;
		++iNumDevices;
	}
}

void CEnumerateDevices::WriteSelectedDeviceToRegistry()
{
	SDevice dev = devGetSelectedDevice();

	if (!dev.bD3DDeviceFound)
	{
		GUID guid;

		// Write software version to the registry.
		SetD3D(FALSE);
		memset(&guid, 0, sizeof(GUID));
		WriteDDGUID(guid);
		WriteD3DGUID(guid);
		
	}
	else
	{
		// Write the hardware version to the registry.
		SetD3D(TRUE);
		WriteDDGUID(dev.ddDevice.guidID);
		WriteD3DGUID(dev.d3dDevice.guidID);
	}

	// Write the common information to the registry.
	bSetInitFlag(TRUE);
	{
		// Set the title.
		char str_title[2048];

		GetTitle(iSelectedDevice, str_title);
		SetRegString(strD3D_TITLE, str_title);
	}
	SetZBufferBitdepth(dev.d3dDevice.iZBufferBitDepth);
}


//
// Global variables.
//
int SResolution::iResolutionsCount;
CEnumerateDevices *penumdevDevices = 0;