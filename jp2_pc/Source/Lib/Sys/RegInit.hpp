/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Implementation of RegInit.hpp.
 *
 ***********************************************************************************************
 *
 * $Log: /JP2_PC/Source/Lib/Sys/RegInit.hpp $
 * 
 * 43    10/06/98 5:18p Ksherr
 * Set default gore level to 1
 * 
 * 42    10/06/98 5:51a Pkeet
 * Added the page managed flag.
 * 
 * 41    9/18/98 7:06p Pkeet
 * Added code to setup the registry for the NVidia Riva 128.
 * 
 * 40    9/15/98 8:40p Shernd
 * audio volume changes
 * 
 * 39    9/10/98 4:39p Shernd
 * Changing default audio volume from 10 to 9.  it is a 0-9 based system
 * 
 * 38    9/09/98 3:52p Pkeet
 * Added the 'Auto Settings' registry flag.
 * 
 * 37    9/08/98 8:22a Shernd
 * 
 * 36    9/04/98 8:59p Shernd
 * Added viewport registry keys
 * 
 * 35    9/04/98 7:39a Shernd
 * Enable/Disable Audio and Audio3D support
 * 
 * 34    9/04/98 2:54a Rwyatt
 * Added new keys for audio
 * 
 * 33    8/31/98 1:47p Shernd
 * Added Gore and Invert Mouse options
 * 
 * 32    8/28/98 9:41p Pkeet
 * Added registry settings for getting and setting the dither flag.
 * 
 * 31    8/23/98 3:30p Pkeet
 * Application defaults to 320x240 resolution. If that resolution is not
 * available, it chooses the next lowest resolution. If there are no lower
 * resolutions, it selects the next highest resolution.
 * 
 * 1     12/08/97 12:52p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef LIB_SYS_REGINIT_HPP
#define LIB_SYS_REGINIT_HPP

//
// Add the bare minimum for this interface without require the entire GUIApp inclusion.
//
#include <windows.h>
#include <ddraw.h>

// Registry entries for the DirectDraw video device name, description and GUID.
#define strDDDEVICE_NAME			"Video Name"
#define strDDDEVICE_DESCRIPTION		"Video Description"
#define strDDDEVICE_GUID			"Video GUID"
#define strD3DDEVICE_NAME			"D3D Name"
#define strD3DDEVICE_DESCRIPTION	"D3D Description"
#define strD3DDEVICE_GUID			"D3D GUID"
#define strFLAG_FULLSCREEN          "Full Screen"
#define strFLAG_D3D                 "Use D3D"
#define strFLAG_SYSTEMMEM           "System Memory"
#define strFLAG_REGINIT				"Registry Init"
#define strSIZE_WIDTH               "Width"
#define strSIZE_HEIGHT              "Height"
#define strPARTITION_SUBDIVISION    "Partition Subdivision"
#define strPARTITION_STUFFCHILDREN  "Partition Stuff Children"
#define strAUTOSETTINGS             "Auto Settings"


#define REG_KEY_PID                 "PID"
#define REG_KEY_DATA_DRIVE          "Data Drive"
#define REG_KEY_INSTALLED           "Installed"
#define REG_KEY_INSTALLED_DIR       "Installed Directory"
#define REG_KEY_NOVIDEO             "NoVideo"
#define REG_KEY_AUDIO_LEVEL         "AudioLevel"
#define REG_KEY_AUDIO_EFFECT        "PlaySFX"
#define REG_KEY_AUDIO_AMBIENT       "PlayAmbient"
#define REG_KEY_AUDIO_VOICEOVER     "PlayVoiceOver"
#define REG_KEY_AUDIO_MUSIC         "PlayMusic"
#define REG_KEY_AUDIO_SUBTITLES     "UseSubTitles"
#define REG_KEY_AUDIO_ENABLE		"AudioEnable"
#define REG_KEY_AUDIO_ENABLE3D		"AudioEnable3D"
#define REG_KEY_GAMMA               "Gamma"
#define REG_KEY_DSOUND_IGNORE       "DSoundIgnore"
#define REG_KEY_DDRAW_CERT_IGNORE   "DDrawCertIgnore"
#define REG_KEY_DDRAW_HARD_IGNORE   "DDrawHardIgnore"
#define REG_KEY_VIEWPORT_X          "XView"
#define REG_KEY_VIEWPORT_Y          "YView"

#define REG_KEY_RENDERING_QUALITY   "RenderQuality"
#define REG_KEY_AUTOLOAD            "AutoLoad Temp"
#define REG_KEY_SAFEMODE            "Safemode"
#define REG_KEY_KEYMAP              "KeyMap"
#define REG_KEY_GORE                "Gore"
#define REG_KEY_INVERTMOUSE         "InvertMouse"

// Registry entries for error handling.
#define REG_KEY_AUTOSAVE            "Auto Save"

// Setup the default values with #defines for the necessary items
#define DEFAULT_REG_NOVIDEO         FALSE
#define DEFAULT_SIZE_WIDTH          640
#define DEFAULT_SIZE_HEIGHT         480
#define DEFAULT_FULLSCREEN          TRUE
#define DEFAULT_D3D                 FALSE
#define DEFAULT_SYSTEMMEM           FALSE
#define DEFAULT_AUDIO_EFFECT        TRUE
#define DEFAULT_AUDIO_AMBIENT       TRUE
#define DEFAULT_AUDIO_VOICEOVER     TRUE
#define DEFAULT_AUDIO_MUSIC         TRUE
#define DEFAULT_AUDIO_SUBTITLES     FALSE
#define DEFAULT_AUDIO_ENABLE3D      TRUE
#define DEFAULT_RENDERING_QUALITY   3
#define DEFAULT_GAMMA               5
#define DEFAULT_GORE                1
#define DEFAULT_INVERTMOUSE         FALSE
#define DEFAULT_VIEWPORT_X          0
#define DEFAULT_VIEWPORT_Y          0

// Direct3D Card Settings.
#define strD3D_FILTERCACHES         "D3D Filter Caches"
#define strD3D_DITHER               "Dither"
#define strVIDEOCARD_TYPE			"Video Card Type"
#define strVIDEOCARD_NAME			"Video Card Name"
#define strRECOMMENDEDTEXMAX        "Max Recommended Tex Dim"
#define strTRIPLEBUFFER             "Triple Buffer"
#define strRESTORE_NVIDIA       	"Restore NVidia"
#define strRESTORE_NVIDIAMIPMAPS	"Restore NVidia Mipmap"
#define strRESTORE_NVIDIASQUARE     "Restore NVidia Square"
#define strPAGEMANAGED              "Page Managed"

#define strD3D_TITLE                "Video Driver"
#define strZBUFFER_BITDEPTH			"Z Buffer Bitdepth"
#define strHARDWARE_WATER           "Hardware Water"


//*********************************************************************************************
//
void WriteDDGUID
(
	const GUID& guid
);
//
// Writes the direct draw GUID description to the registry.
//
//**************************************

//*********************************************************************************************
//
GUID ReadDDGUID
(
);
//
// Reads the direct draw GUID description from the registry.
//
//**************************************

//*********************************************************************************************
//
GUID* pConvertGUID
(
	GUID& guid
);
//
// Returns a null pointer if the GUID is filled with zeroes, otherwise returns a pointer to
// the supplied GUID.
//
//**************************************

//*********************************************************************************************
//
void SetFullScreen
(
	BOOL b_fullscreen	// Full screen flag.
);
//
// Sets the full screen flag into the registry.
//
//**************************************

//*********************************************************************************************
//
BOOL bGetFullScreen
(
);
//
// Gets the full screen flag from the registry.
//
//**************************************

//*********************************************************************************************
//
void SetD3D
(
	BOOL b_d3d	// D3D flag.
);
//
// Sets the D3D flag into the registry.
//
//**************************************

//*********************************************************************************************
//
BOOL bGetD3D
(
);
//
// Gets the D3D flag from the registry.
//
//**************************************

//*********************************************************************************************
//
void SetDimensions
(
	int i_width,	// Width of raster.
	int i_height	// Height of raster.
);
//
// Sets the height and width of the raster into the registry.
//
//**************************************

//*********************************************************************************************
//
BOOL bGetDimensions
(
	int& ri_width,	// Width of raster.
	int& ri_height	// Height of raster.
);
//
// Gets the height and width of the raster from the registry.
//
//**************************************

//*********************************************************************************************
//
void bSetSystemMem
(
	BOOL b_use_system_mem	// Flag to use system memory.
);
//
// Sets the system memory flag in the registry.
//
//**************************************

//*********************************************************************************************
//
BOOL bGetSystemMem
(
);
//
// Returns the system memory flag from the registry.
//
//**************************************

//*********************************************************************************************
//
void bSetInitFlag
(
	BOOL b_init	// Flag indicating initialization through the registry.
);
//
// Sets the initialization flag in the registry.
//
//**************************************

//*********************************************************************************************
//
BOOL bGetInitFlag
(
	BOOL b_default = TRUE
);
//
// Returns the initialization flag from the registry.
//
//**************************************

//*********************************************************************************************
//
void WriteD3DGUID
(
	const GUID& guid
);
//
// Writes the direct draw GUID description to the registry.
//
//**************************************

//*********************************************************************************************
//
GUID ReadD3DGUID
(
);
//
// Reads the direct draw GUID description from the registry.
//
//**************************************

//*********************************************************************************************
//
void SetPartitionBuildInfo
(
	int  i_subdivision,	// Number of objects per subdivision.
	BOOL b_stuff		// Flag indicates if children should be stuffed.
);
//
// Writes partition build settings to the registry.
//
//**************************************

//*********************************************************************************************
//
void GetPartitionBuildInfo
(
	int&  ri_subdivision,	// Number of objects per subdivision.
	BOOL& rb_stuff			// Flag indicates if children should be stuffed.
);
//
// Reads partition build settings from the registry.
//
//**************************************

//*********************************************************************************************
//
void SetAutoSave
(
	BOOL b_init	// Flag indicating initialization through the registry.
);
//
// Sets the auto save flag in the registry.
//
//**************************************

//*********************************************************************************************
//
BOOL bGetAutoSave
(
);
//
// Returns the initialization flag from the registry.
//
//**************************************

//*********************************************************************************************
//
void SetAutoLoad
(
	BOOL b_init	= FALSE	// Flag indicating automatic loading.
);
//
// Sets the auto load flag in the registry.
//
//**************************************

//*********************************************************************************************
//
BOOL bGetAutoLoad
(
);
//
// Returns the auto load flag from the registry.
//
//**************************************

//*********************************************************************************************
//
BOOL bSafemode
(
);
//
// Returns 'true' if the application is in safemode.
//
//**************************************

//*********************************************************************************************
//
void SetZBufferBitdepth
(
	int i_bitdepth
);
//
// Sets the number of bits per pixel for the Z buffer.
//
//**************************************

//*********************************************************************************************
//
int iGetZBufferBitdepth
(
);
//
// Returns the number of bits per pixel for the Z buffer.
//
//**************************************

//*********************************************************************************************
//
void SetRecommendedTextureMaxDim
(
	int i_dim
);
//
// Sets the recommended maximum texture dimension for Direct3D.
//
//**************************************

//*********************************************************************************************
//
int iGetRecommendedTextureMaxDim
(
);
//
// Returns the recommended maximum texture dimension for Direct3D.
//
//**************************************

//*********************************************************************************************
//
bool bGetTripleBuffer
(
);
//
// Returns 'true' if the application should triple buffer in hardware.
//
//**************************************

//*********************************************************************************************
//
void SetTripleBuffer
(
	BOOL b	// Flag 
);
//
// Sets the triple buffer flag for hardware.
//
//**************************************

//*********************************************************************************************
//
bool bGetDither
(
);
//
// Returns 'true' if the application should dither in hardware.
//
//**************************************

//*********************************************************************************************
//
void SetDither
(
	BOOL b	// Flag 
);
//
// Sets the dither flag for hardware.
//
//**************************************

//*********************************************************************************************
//
bool bAutoSettings
(
	BOOL b	// New state. 
);
//
// Sets the auto settings flag in the registry and returns the old value.
//
//**************************************

//*********************************************************************************************
//
bool bGetPageManaged
(
);
//
// Returns 'true' if hardware should use page management.
//
//**************************************

//*********************************************************************************************
//
void SetPageManaged
(
	BOOL b	// Flag 
);
//
// Sets the page management flag.
//
//**************************************

void SetSettingToDefault(const char* setting);

void SetAllSettingsToDefault();

void SetKeyMappingToDefault();

#endif // LIB_SYS_REGINIT_HPP
