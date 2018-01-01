/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Implementation of Direct3DCards.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/Direct3DCards.cpp                                             $
 * 
 * 7     10/06/98 5:52a Pkeet
 * Added detection for the Voodoo 2.
 * 
 * 6     9/18/98 7:04p Pkeet
 * Added registry routines for correctly setting up the NVidia Riva 128.
 * 
 * 5     9/14/98 7:59p Pkeet
 * 512x384 screen resolution is suppressed for Rendition V1000 based cards.
 * 
 * 4     9/05/98 7:52p Pkeet
 * Added support for the Matrox G100.
 * 
 * 3     9/02/98 6:05p Pkeet
 * Added detection for Trident3D cards.
 * 
 * 2     8/29/98 3:25p Pkeet
 * Added the specific detection code.
 * 
 * 1     8/29/98 2:43p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/


//
// Necessary includes.
//
#include "Lib/W95/WinInclude.hpp"
#include "Direct3DCards.hpp"
#include "Lib/Sys/RegInit.hpp"
#include "Lib/Sys/reg.h"

#define REGNVIDIA_KEYPARENT       HKEY_LOCAL_MACHINE
#define REGNVIDIA_REGLOCATION     "SOFTWARE\\NV3D3D"
#define REGNVIDIA_REGLOCATION_ALT "SOFTWARE\\NVIDIA Corporation\\RIVA 128\\Direct3D"
#define REGNVIDIA_MIPMAPS         "MIPMAPLEVELS"
#define REGNVIDIA_SQUAREONLY      "SQUAREONLYENABLE"

const unsigned int u4MipmapLevels     = 0;
const unsigned int u4SquareOnlyEnable = 0;

bool  bAltNVidia = false;
DWORD dwMipType  = REG_DWORD;


//
// Function implementations.
//

//*********************************************************************************************
EVideoCard evcGetCard(LPDIRECTDRAW2 pdd_device)
{
	LPDIRECTDRAW4 pdd_4 = 0;

	if (!pdd_device)
		return evcUnknown;
	HRESULT hres = pdd_device->QueryInterface(IID_IDirectDraw4, (LPVOID*)&pdd_4);
	if (FAILED(hres))
		return evcUnknown;
	if (!pdd_4)
		return evcUnknown;
	EVideoCard evc = evcGetCard(pdd_4);
	pdd_4->Release();
	return evc;
}

//*********************************************************************************************
EVideoCard evcGetCard(LPDIRECTDRAW4 pdd4_device)
{
	DDDEVICEIDENTIFIER devid;	// Device identifier.

	// If the device is null, the card cannot be identified.
	if (!pdd4_device)
		return evcUnknown;

	// Get the device identifier.
	HRESULT hres = pdd4_device->GetDeviceIdentifier(&devid, 0);

	// If the identifier fails, the card cannot be identified.
	if (FAILED(hres))
		return evcUnknown;

	// Test for any ATI.
	if (devid.dwVendorId == 0x00001002)
		return evcATIRage;

	// Test for the Matrox G100.
	if (devid.dwVendorId == 0x0000102b && devid.dwDeviceId == 0x00001001)
		return evcMatroxG100;

	// Test for the NVidia Riva 128.
	if (devid.dwVendorId == 0x000012d2 && devid.dwDeviceId == 0x00000018)
		return evcNVidia128;

	// Test for the Permedia 2.
	if (devid.dwVendorId == 0x0000104C && devid.dwDeviceId == 0x00003D07)
		return evcPermedia2;

	// Test for a Rendition Verite 1000.
	if (devid.dwVendorId == 0x00001163 && devid.dwDeviceId == 0x00000001)
		return evcRenditionV1000;

	// Test for a Trident3D.
	if (devid.dwVendorId == 0x00001023)
		return evcTrident3D;

	// Test for the Voodoo2
	if (devid.dwVendorId == 0x0000121A && devid.dwDeviceId == 0x00000002)
		return evcVoodoo2;

	// No specific card detected.
	return evcUnknown;
}

//**********************************************************************************************
int GetRegValueNVidia(HKEY hkey, LPCSTR lpszVal, int nDefault)
{
	DWORD dwType,dwSize;
	DWORD nRes;
	dwSize = sizeof(nRes);
	if (RegQueryValueEx (hkey,
		lpszVal,
		NULL,
		&dwType,
		(LPBYTE)&nRes,
		&dwSize)==ERROR_SUCCESS)
	{
		if (dwType==REG_DWORD)
			return (int)nRes;
	}
	return nDefault;
}

//**********************************************************************************************
int GetRegDataNVidia(HKEY hkey, LPCSTR lpszVal, LPBYTE lpszData, int nSize)
{
	DWORD dwType,dwSize;
	dwSize = nSize;
	if (RegQueryValueEx(hkey,
		lpszVal,
		NULL,
		&dwType,
		lpszData,
		&dwSize)!=ERROR_SUCCESS)
		dwSize = 0;
	return dwSize;
}

//**********************************************************************************************
HKEY hkeyOpenNVidiaRegistry()
{
	HKEY hkey;

	// Open the registry for the new driver if possible.
	if (RegOpenKeyEx(REGNVIDIA_KEYPARENT, REGNVIDIA_REGLOCATION_ALT, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS)
	{
		bAltNVidia = true;
		dwMipType  = REG_BINARY;
		return hkey;
	}

	// Open or create the registry for the old driver.
	if (RegOpenKeyEx(REGNVIDIA_KEYPARENT, REGNVIDIA_REGLOCATION, 0, KEY_ALL_ACCESS, &hkey) != ERROR_SUCCESS)
		RegCreateKey(REGNVIDIA_KEYPARENT, REGNVIDIA_REGLOCATION, &hkey);
	bAltNVidia = false;
	dwMipType  = REG_DWORD;
	return hkey;
}

//**********************************************************************************************
BOOL bMustSetNVidiaRegistry()
{
	// Determine if the NVidia registry settings must be set.
	if (GetRegValue(strRESTORE_NVIDIA, 0) == 0)
		return FALSE;

	// Open the registry for NVidia settings.
	HKEY hkey = hkeyOpenNVidiaRegistry();

	// Test each value.
	unsigned int u_mipmaplevels     = GetRegValueNVidia(hkey, REGNVIDIA_MIPMAPS, 1);
	unsigned int u_squareonlyenable = GetRegValueNVidia(hkey, REGNVIDIA_SQUAREONLY, 1);

	// Close the registry.
	if (hkey)
		RegCloseKey(hkey);

	// Return the results of the test.
	return !(u_mipmaplevels == u4MipmapLevels && u_squareonlyenable == u4SquareOnlyEnable);
}

//**********************************************************************************************
void SetNVidiaRegistry()
{
	// Open the registry for NVidia settings.
	HKEY hkey = hkeyOpenNVidiaRegistry();

	// Save the old values for restoration.
	unsigned int u_mipmaplevels;
	
	if (bAltNVidia)
	{
		int i_datasize = GetRegDataNVidia(hkey, REGNVIDIA_MIPMAPS, (LPBYTE)&u_mipmaplevels, 4);
		if (i_datasize == 0)
			u_mipmaplevels = 0xFFFF;
	}
	else
	{
		u_mipmaplevels = GetRegValueNVidia(hkey, REGNVIDIA_MIPMAPS, 0xFFFF);
	}
	unsigned int u_squareonlyenable = GetRegValueNVidia(hkey, REGNVIDIA_SQUAREONLY, 0xFFFF);
	SetRegValue(strRESTORE_NVIDIAMIPMAPS, u_mipmaplevels);
	SetRegValue(strRESTORE_NVIDIASQUARE,  u_squareonlyenable);

	// Set the new values.
	RegSetValueEx(hkey, REGNVIDIA_MIPMAPS, NULL, dwMipType, (LPBYTE)&u4MipmapLevels, sizeof(u4MipmapLevels));
	RegSetValueEx(hkey, REGNVIDIA_SQUAREONLY, NULL, REG_DWORD, (LPBYTE)&u4SquareOnlyEnable, sizeof(u4SquareOnlyEnable));

	// Close the registry.
	RegCloseKey(hkey);
}

//**********************************************************************************************
void RestoreNVidiaRegistry()
{
	// Determine if the NVidia registry settings must be restored.
	if (GetRegValue(strRESTORE_NVIDIA, 0) == 0)
		return;

	// Open the registry for NVidia settings.
	HKEY hkey = hkeyOpenNVidiaRegistry();
	
	unsigned int u_mipmaplevels     = GetRegValue(strRESTORE_NVIDIAMIPMAPS, 0xFFFF);
	unsigned int u_squareonlyenable = GetRegValue(strRESTORE_NVIDIASQUARE,  0xFFFF);

	if (u_mipmaplevels == 0xFFFF)
	{
		RegDeleteValue(hkey, REGNVIDIA_MIPMAPS);
	}
	else
	{
		RegSetValueEx(hkey, REGNVIDIA_MIPMAPS, 0, dwMipType, (LPBYTE)&u_mipmaplevels, sizeof(u_mipmaplevels));
	}

	if (u_squareonlyenable == 0xFFFF)
	{
		RegDeleteValue(hkey, REGNVIDIA_SQUAREONLY);
	}
	else
	{
		RegSetValueEx(hkey, REGNVIDIA_SQUAREONLY, 0, REG_DWORD, (LPBYTE)&u_squareonlyenable, sizeof(u_squareonlyenable));
	}

	// Close the registry.
	RegCloseKey(hkey);
}
