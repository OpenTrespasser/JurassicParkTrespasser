/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Detects Direct3D video cards.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/Direct3DCards.hpp                                             $
 * 
 * 6     10/06/98 5:52a Pkeet
 * Added detection for the Voodoo 2.
 * 
 * 5     9/18/98 7:05p Pkeet
 * Added registry routines for correctly setting up the NVidia Riva 128.
 * 
 * 4     9/14/98 7:59p Pkeet
 * 512x384 screen resolution is suppressed for Rendition V1000 based cards.
 * 
 * 3     9/05/98 7:53p Pkeet
 * Added support for the Matrox G100.
 * 
 * 2     9/02/98 6:05p Pkeet
 * Added detection for Trident3D cards.
 * 
 * 1     8/29/98 2:51p Pkeet
 * Initial implemenation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_DIRECT3DCARDS_HPP
#define HEADER_LIB_RENDERER_DIRECT3DCARDS_HPP


//
// Necessary includes.
//
#include "Lib/W95/WinInclude.hpp"
#include "DDraw.h"


//
// Enumerations.
//
enum EVideoCard
{
	evcATIRage,
	evcMatroxG100,
	evcNVidia128,
	evcPermedia2,
	evcRenditionV1000,
	evcTrident3D,
	evcVoodoo2,
	evcUnknown
};


//
// Global functions.
//

//*********************************************************************************************
//
EVideoCard evcGetCard
(
	LPDIRECTDRAW2 pdd_device
);
//
// Returns the video card associated with the DirectDraw device.
//
//**************************************

//*********************************************************************************************
//
EVideoCard evcGetCard
(
	LPDIRECTDRAW4 pdd4_device
);
//
// Returns the video card associated with the DirectDraw device.
//
//**************************************

//**********************************************************************************************
//
BOOL bMustSetNVidiaRegistry
(
);
//
// Returns 'TRUE' if the registry must still be set for using the NVidia Riva 128.
//
//**************************************

//**********************************************************************************************
//
void SetNVidiaRegistry
(
);
//
// Sets the registry for use with the NVidia Riva 128.
//
//**************************************

//**********************************************************************************************
//
void RestoreNVidiaRegistry
(
);
//
// Restores the original NVidia Riva 128 settings.
//
//**************************************


#endif // HEADER_LIB_RENDERER_DIRECT3DCARDS_HPP
