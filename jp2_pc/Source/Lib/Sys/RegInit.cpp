/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Implementation of RegInit.hpp.
 *
 * Notes:
 *		This file may be used by applications outside of 'GUIApp' and even 'Trespasser.' As a
 *		means to simplify writing applications outside of these two environments, regular
 *		windows headers and asserts are used.
 *
 ***********************************************************************************************
 *
 * $Log: /JP2_PC/Source/Lib/Sys/RegInit.cpp $
 * 
 * 29    10/06/98 5:51a Pkeet
 * Added the page managed flag.
 * 
 * 28    9/09/98 3:52p Pkeet
 * Added the 'Auto Settings' registry flag.
 * 
 * 27    8/31/98 9:55p Pkeet
 * Turned dithering on by default.
 * 
 * 26    8/30/98 5:03p Asouth
 * explicit variable declaration
 * 
 * 25    8/29/98 3:40p Pkeet
 * Set triple buffering off by default.
 * 
 * 24    8/28/98 9:41p Pkeet
 * Added registry settings for getting and setting the dither flag.
 * 
 * 23    8/23/98 2:24a Pkeet
 * Added an option for triple buffering.
 * 
 * 22    8/18/98 3:11p Pkeet
 * Added in registry access functions for setting the recommended maximum
 * texture size for Direct3D.
 * 
 * 21    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 20    8/05/98 9:18p Pkeet
 * Added functions for getting and setting the Z buffer bitdepth.
 * 
 * 19    8/05/98 9:12p Pkeet
 * Removed device descriptions for DirectDraw and Direct3D.
 * 
 * 18    7/12/98 2:35p Pkeet
 * Set autoload to its correct default value.
 * 
 * 17    7/08/98 6:35p Pkeet
 * Added 'SafeMode.' Added auto load keys.
 * 
 * 16    6/29/98 9:59p Pkeet
 * Added a default parameter for 'bGetInitFlag' to allow GUIApp to work
 * correctly.
 * 
 * 15    6/29/98 9:14p Pkeet
 * Added a string for recording the video card name. Removed functions for
 * setting and getting video card types.
 * 
 * 14    6/15/98 12:11p Pkeet
 * Added a class for storing individual Direct3D parameters separately.
 * 
 * 13    6/08/98 4:13p Pkeet
 * Added registry functions for setting and getting video card types.
 * 
 * 12    3/19/98 5:18p Pkeet
 * Added 'Auto Save' registry values and functions.
 * 
 * 11    3/03/98 10:47a Mlange
 * Removed terrain texture size divisor and memory error flag.
 * 
 * 10    1/30/98 3:08p Pkeet
 * Increased default settings for partition building.
 * 
 * 9     1/29/98 6:51p Pkeet
 * Set defaults to build the partition quickly.
 * 
 * 8     1/29/98 4:44p Pkeet
 * Added keys for partition building.
 * 
 * 7     12/15/97 1:50p Pkeet
 * Added functions for setting and getting the terrain divisor.
 * 
 * 6     12/12/97 1:52p Shernd
 * moved #defines to the header file
 * 
 * 5     12/09/97 6:21p Pkeet
 * Added code to set and get D3D driver information.
 * 
 * 4     12/09/97 12:46p Pkeet
 * Added the registry init flag.
 * 
 * 3     12/09/97 12:23p Pkeet
 * Added a flag for system memory.
 * 
 * 2     12/08/97 6:38p Pkeet
 * Initial implementation of initialization functions.
 * 
 * 1     12/08/97 12:04p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include <assert.h>
#include <memory.h>
#include "Lib/W95/WinInclude.hpp"
#include "Reg.h"
#include "RegInit.hpp"

//
// Macros and constants.
//

//
// External variables.
//

extern BOOL bSafeModeReg;

//
// Function implementation.
//

//*********************************************************************************************
void WriteDDGUID(const GUID& guid)
{
	SetRegData(strDDDEVICE_GUID, (unsigned char*)&guid, sizeof(GUID));
}

//*********************************************************************************************
GUID ReadDDGUID()
{
	GUID guid;	// Device GUID.

	// Read in the GUID.
	if (GetRegData(strDDDEVICE_GUID, (unsigned char*)&guid, sizeof(GUID)) != sizeof(GUID))
	{
		// If the GUID was not read in, use a GUID filled with zero.
		memset(&guid, 0, sizeof(GUID));
	}
	return guid;
}

//*********************************************************************************************
GUID* pConvertGUID(GUID& guid)
{
	GUID guid_null;	// A GUID filled with zeroes.

	memset(&guid_null, 0, sizeof(GUID));
	return (memcmp(&guid, &guid_null, sizeof(GUID)) == 0) ? (0) : (&guid);
}

//*********************************************************************************************
void SetFullScreen(BOOL b_fullscreen)
{
	SetRegValue(strFLAG_FULLSCREEN, b_fullscreen);
}

//*********************************************************************************************
BOOL bGetFullScreen()
{
	return GetRegValue(strFLAG_FULLSCREEN, DEFAULT_FULLSCREEN) != 0;
}

//*********************************************************************************************
void SetD3D(BOOL b_d3d)
{
	SetRegValue(strFLAG_D3D, b_d3d);
}

//*********************************************************************************************
BOOL bGetD3D()
{
	return GetRegValue(strFLAG_D3D, DEFAULT_D3D) != 0;
}

//*********************************************************************************************
void SetDimensions(int i_width, int i_height)
{
	SetRegValue(strSIZE_WIDTH, i_width);
	SetRegValue(strSIZE_HEIGHT, i_height);
}

//*********************************************************************************************
BOOL bGetDimensions(int& ri_width, int& ri_height)
{
	ri_width  = GetRegValue(strSIZE_WIDTH, DEFAULT_SIZE_WIDTH);
	ri_height = GetRegValue(strSIZE_HEIGHT, DEFAULT_SIZE_HEIGHT);
	return TRUE;
}

//*********************************************************************************************
void bSetSystemMem(BOOL b_use_system_mem)
{
	SetRegValue(strFLAG_SYSTEMMEM, (b_use_system_mem) ? (1) : (0));
}

//*********************************************************************************************
BOOL bGetSystemMem()
{
	return GetRegValue(strFLAG_SYSTEMMEM, DEFAULT_SYSTEMMEM) != 0;
}


//*********************************************************************************************
void bSetInitFlag(BOOL b_init)
{
	SetRegValue(strFLAG_REGINIT, (b_init) ? (1) : (0));
}

//*********************************************************************************************
BOOL bGetInitFlag(BOOL b_default)
{
	return GetRegValue(strFLAG_REGINIT, b_default) != 0;
}

//*********************************************************************************************
void WriteD3DGUID(const GUID& guid)
{
	SetRegData(strD3DDEVICE_GUID, (unsigned char*)&guid, sizeof(GUID));
}

//*********************************************************************************************
GUID ReadD3DGUID()
{
	GUID guid;	// Device GUID.

	// Read in the GUID.
	if (GetRegData(strD3DDEVICE_GUID, (unsigned char*)&guid, sizeof(GUID)) != sizeof(GUID))
	{
		// If the GUID was not read in, use a GUID filled with zero.
		memset(&guid, 0, sizeof(GUID));
	}
	return guid;
}

//*********************************************************************************************
void SetPartitionBuildInfo(int i_subdivision, BOOL b_stuff)
{
	assert(i_subdivision > 2);

	SetRegValue(strPARTITION_SUBDIVISION,   i_subdivision);
	SetRegValue(strPARTITION_STUFFCHILDREN, (b_stuff) ? (1) : (0));
}

//*********************************************************************************************
void GetPartitionBuildInfo(int& ri_subdivision, BOOL& rb_stuff)
{
	ri_subdivision = GetRegValue(strPARTITION_SUBDIVISION,   200);
	rb_stuff       = GetRegValue(strPARTITION_STUFFCHILDREN, 1) != 0;
}

//*********************************************************************************************
void SetAutoSave(BOOL b_init)
{
	SetRegValue(REG_KEY_AUTOSAVE, b_init);
}

//*********************************************************************************************
BOOL bGetAutoSave()
{
	return GetRegValue(REG_KEY_AUTOSAVE, 0) != 0;
}

//*********************************************************************************************
void SetAutoLoad(BOOL b_init)
{
	SetRegValue(REG_KEY_AUTOLOAD, b_init);
}

//*********************************************************************************************
BOOL bGetAutoLoad()
{
	return GetRegValue(REG_KEY_AUTOLOAD, 0) != 0;
}

//*********************************************************************************************
BOOL bSafemode()
{
	return bSafeModeReg;
}

//*********************************************************************************************
void SetZBufferBitdepth(int i_bitdepth)
{
	SetRegValue(strZBUFFER_BITDEPTH, i_bitdepth);
}

//*********************************************************************************************
int iGetZBufferBitdepth()
{
	return GetRegValue (strZBUFFER_BITDEPTH, 0);
}

//*********************************************************************************************
void SetRecommendedTextureMaxDim(int i_dim)
{
	SetRegValue(strRECOMMENDEDTEXMAX, i_dim);
}

//*********************************************************************************************
int iGetRecommendedTextureMaxDim()
{
	return GetRegValue (strRECOMMENDEDTEXMAX, 128);
}

//*********************************************************************************************
bool bGetTripleBuffer()
{
	return GetRegValue(strTRIPLEBUFFER, 0) != 0;
}

//*********************************************************************************************
void SetTripleBuffer(BOOL b)
{
	SetRegValue(strTRIPLEBUFFER, b);
}

//*********************************************************************************************
bool bGetDither()
{
	return GetRegValue(strD3D_DITHER, 1) != 0;
}

//*********************************************************************************************
void SetDither(BOOL b)
{
	SetRegValue(strD3D_DITHER, b);
}

//*********************************************************************************************
bool bAutoSettings(BOOL b)
{
	bool b_retval = GetRegValue(strAUTOSETTINGS, 0) != 0;
	SetRegValue(strAUTOSETTINGS, b);
	return b_retval;
}

//*********************************************************************************************
bool bGetPageManaged()
{
	return GetRegValue(strPAGEMANAGED, 1) != 0;
}

//*********************************************************************************************
void SetPageManaged(BOOL b)
{
	SetRegValue(strPAGEMANAGED, b);
}