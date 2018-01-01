/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Direct3D query functions.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/Direct3DQuery.hpp                                             $
 * 
 * 24    8/11/98 6:18p Pkeet
 * Removed the settings structure.
 * 
 * 23    8/10/98 11:30p Pkeet
 * Added a test for finding out if a card lights the pixel based on its centre or on its corner.
 * 
 * 22    8/10/98 5:51p Pkeet
 * Added the 'ed3dcom565_RGB' common format.
 * 
 * 21    8/06/98 5:10p Pkeet
 * Alpha is now reported on a different flag.
 * 
 * 20    7/30/98 4:06p Pkeet
 * Added the 'bSlowCreate' flag.
 * 
 * 19    7/23/98 6:19p Pkeet
 * Added code for detecting common formats for conversion.
 * 
 * 18    7/20/98 10:32p Pkeet
 * Upgraded to Direct3D 4's interface.
 * 
 * 17    7/09/98 7:22p Pkeet
 * Added comment.
 * 
 * 16    7/09/98 3:56p Pkeet
 * Added support for the direct texture access flag.
 * 
 * 15    7/01/98 3:27p Pkeet
 * Added data members for notes, chipset and driver information. Added code to dump a text file
 * of card settings.
 * 
 * 14    7/01/98 12:35p Pkeet
 * Added the 'DumpList' member function.
 * 
 * 13    6/30/98 2:37p Pkeet
 * Moved 'strD3DSettingsFileName' to the header and made it a macro.
 * 
 * 12    6/30/98 2:23p Pkeet
 * Added the 'RemoveD3DSetting' member function.
 * 
 * 11    6/30/98 1:46p Pkeet
 * Added the 'strDEFAULT_CARDNAME' macro.
 * 
 * 10    6/29/98 9:20p Pkeet
 * Changed query to rely on a configuration file instead of built-in settings.
 * 
 * 9     6/15/98 12:11p Pkeet
 * Added a class for storing individual Direct3D parameters separately.
 * 
 * 8     6/12/98 9:12p Pkeet
 * Added numerous cards.
 * 
 * 7     6/08/98 4:52p Pkeet
 * Fixed warnings.
 * 
 * 6     6/08/98 4:34p Pkeet
 * Added the 'ecardBegin' macro.
 * 
 * 5     6/08/98 4:25p Pkeet
 * Added the 'bUseSecondaryCard' global function.
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
 * 1     1/09/98 11:41a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_DIRECT3DQUERY_HPP
#define HEADER_LIB_RENDERER_DIRECT3DQUERY_HPP


//
// Macros.
//


//
// Forward declarations.
//


//
// Class definitions.
//

//*********************************************************************************************
//
//
// Encapsulates tweakable Direct3D settings.
//
// Prefix: d3dset
//
//**************************************

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	//
	// Saves all the settings into the registry.
	//
	//**************************************


//
// Global functions.
//

//*********************************************************************************************
//
HRESULT WINAPI D3DEnumTextureFormatCallback
(
	LPDDPIXELFORMAT ppix,
	LPVOID          lpUserArg
);
//
// Callback used to enumerate textures.
//
//**************************************

//*********************************************************************************************
//
inline int iCountBits
(
	unsigned int u4	// Value to count bits in.
)
//
// Returns the number of bits set in the value.
//
//**************************************
{
	int i_num_bits = 0;

	while (u4)
	{
		if (u4 & 1)
			++i_num_bits;
		u4 >>= 1;
	}
	return i_num_bits;
}

//*********************************************************************************************
//
void ScreenTests
(
	LPDIRECTDRAWSURFACE4 pdds_back,		// Pointer to the DirectDraw backbuffer.
	LPDIRECTDRAWSURFACE4 pdds_front		// Pointer to the DirectDraw frontbuffer.
);
//
// Sets operating parameters based on tests that read pixels back from the screen.
//
//**************************************


//
// External variables.
//


#endif // HEADER_LIB_RENDERER_DIRECT3DQUERY_HPP
