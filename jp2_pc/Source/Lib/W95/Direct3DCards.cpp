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

//
// Function implementations.
//


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
