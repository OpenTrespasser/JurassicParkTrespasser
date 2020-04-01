/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		A Direct3D driver.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/Direct3D.hpp                                                  $
 * 
 * 64    10/06/98 5:51a Pkeet
 * Added the page managed flag.
 * 
 * 63    9/22/98 10:39p Pkeet
 * Forced inline critical Direct3D functions.
 * 
 * 62    9/21/98 8:58p Pkeet
 * Added 'SetFlipClear' member function.
 * 
 * 61    9/09/98 7:40p Pkeet
 * Added functions to show that the computer is working while it loads Direct3D surfaces.
 * 
 * 60    9/05/98 7:53p Pkeet
 * Added support for the Matrox G100.
 * 
 * 59    8/30/98 11:14p Pkeet
 * Added the 'bSecondaryCard' and 'bLocalVideoMem' flags.  Added the 'PurgePrimary' member
 * function.
 * 
 * 58    8/29/98 8:13p Pkeet
 * Added a flag for fogging caches using hardware.
 * 
 * 57    8/29/98 2:48p Pkeet
 * Added in an enumeration for detecting the card type.
 * 
 * 56    8/28/98 9:40p Pkeet
 * Removed the 'bDitherAvailable' flag.
 * 
 * 55    8/25/98 11:54a Rvande
 * Removed redundant class scope within class declaration
 * 
 * 54    8/22/98 11:04p Pkeet
 * Added support for full texturing in 32 Mb.
 * 
 * 53    8/22/98 6:42p Pkeet
 * Added the 'bDeviceTested' flag. Added the 'ReleaseD3D' member function. Renamed the
 * 'InitializeD3D' function to use the correct prefix.
 * 
 * 52    8/20/98 4:43p Pkeet
 * Removed the Z buffer flag from the Direct3D object.
 * 
 * 51    8/20/98 2:21p Pkeet
 * Separated initialization of the Direct3D interface.
 * 
 * 50    8/18/98 3:10p Pkeet
 * Added the 'iRecommendedMaxDim' data member.
 * 
 * 49    8/11/98 6:18p Pkeet
 * Removed the settings structure.
 * 
 * 48    8/10/98 11:30p Pkeet
 * Added a test for finding out if a card lights the pixel based on its centre or on its corner.
 * 
 * 47    8/10/98 8:46p Pkeet
 * Added the 'bTransparentAlpha' flag and a device identifier.
 * 
 * 46    8/10/98 7:33a Kidsbuild
 * For release mode completely remove the PrintD3D and PrintD3D2 calls
 * 
 * 45    8/09/98 2:34p Pkeet
 * Added macros for printing information about Direct3D if the debug print macro switch is on.
 * 
 * 44    8/08/98 6:57p Pkeet
 * Added a selection of allocation schemes.
 * 
 * 43    8/06/98 7:27p Pkeet
 * Added the 'bHardwareWater' flag.
 * 
 * 42    8/06/98 6:16p Pkeet
 * Fixed bug with texture formats for clut conversions.
 * 
 * 41    8/06/98 5:10p Pkeet
 * Alpha is now reported on a different flag.
 * 
 * 40    7/30/98 4:06p Pkeet
 * Added the 'bTexCreatesAreSlow' member function.
 * 
 * 39    7/29/98 8:24p Pkeet
 * Added code for Z buffering.
 * 
 * 38    7/29/98 12:08p Pkeet
 * The device description is now stored.
 * 
 * 37    7/27/98 2:35p Pkeet
 * Added code to output texture memory allocations and usage to the debug window.
 * 
 * 36    7/23/98 6:19p Pkeet
 * Added code for detecting common formats for conversion.
 * 
 * 35    7/21/98 2:04p Pkeet
 * Added functions to account for free Direct3D memory.
 * 
 * 34    7/20/98 10:32p Pkeet
 * Upgraded to Direct3D 4's interface.
 * 
 * 33    7/10/98 4:28p Pkeet
 * Added support for colour keying.
 * 
 * 32    7/09/98 7:22p Pkeet
 * Added the 'bUseDirectTextureAccess' member function.
 * 
 * 31    7/02/98 4:25p Pkeet
 * Fixed bug that prevented proper restoring of Direct3D when alt tabbing.
 * 
 * 30    6/15/98 12:11p Pkeet
 * Added a class for storing individual Direct3D parameters separately.
 * 
 * 29    6/14/98 2:46p Pkeet
 * Added support for fogging render caches using hardware.
 * 
 * 28    6/12/98 9:14p Pkeet
 * Added in data members for optionally supporting the sky and for supporting memory allocations
 * less than the amount available on the card.
 * 
 * 27    6/10/98 7:32p Pkeet
 * Added a flag for using shared surfaces.
 * 
 * 26    6/09/98 5:39p Pkeet
 * Added the 'bD3DWater' member function.
 * 
 * 25    6/09/98 5:32p Pkeet
 * Added support for non-region uploads.
 * 
 * 24    6/08/98 4:26p Pkeet
 * Arranged data members for card specific tweaks together.
 * 
 * 23    5/13/98 6:28p Pkeet
 * Added code to support tranparencies correctly.
 * 
 * 22    4/15/98 12:08p Pkeet
 * Added a data member to store terrain memory.
 * 
 * 21    4/13/98 5:08p Pkeet
 * Added the 'AllocateTextureMemory' member function.
 * 
 * 20    4/13/98 3:37p Pkeet
 * Added a member function to get the amount of texture memory available. Used 'priv_self' to
 * hide private member functions.
 * 
 * 19    4/03/98 5:25p Pkeet
 * Optionally sets filtering for image caches.
 * 
 * 18    4/01/98 5:45p Pkeet
 * Added a parameter to the 'SetTextureMinMax' to truncate the values.
 * 
 * 17    3/27/98 5:09p Pkeet
 * Added flags and functions for using Direct3D for image caches.
 * 
 * 16    3/06/98 7:09p Pkeet
 * Added the 'bEnableInit' data member and the 'bInitEnabled,' 'SetInitEnable,' 'bD3DCapable'
 * member functions and an alternative 'bInitialize' member function. Added an assert to make
 * sure that Direct3D is never used when it is incapable of being initialized.
 * 
 * 15    1/22/98 1:39p Pkeet
 * Added an initial implementation of the 'Purge' and 'Restore' functions.
 * 
 * 14    1/21/98 9:05p Pkeet
 * Added functions to test and set texture sizes based on available attributes.
 * 
 * 13    1/15/98 5:40p Pkeet
 * Added in PowerVR support for fog.
 * 
 * 12    1/15/98 2:28p Pkeet
 * Added the 'Restore' and 'SetAlphaFormat' member functions.
 * 
 * 11    1/09/98 2:40p Pkeet
 * Added an explicit clip adjust value that can be changed from video card to video card.
 * 
 * 10    1/09/98 11:43a Pkeet
 * Added a screen coordinate translation value for software to Direct3D coordinate translations.
 * Added a flag to detect if square textures are reqired.
 * 
 * 9     1/06/98 6:26p Pkeet
 * Added member functions for setting fog.
 * 
 * 8     12/19/97 4:23p Pkeet
 * Added flags for testing alpha and dithering capabilities.
 * 
 * 7     12/17/97 5:39p Pkeet
 * Added error handling for Direct3D.
 * 
 * 6     12/16/97 5:25p Pkeet
 * Added a conversion function for alpha.
 * 
 * 5     12/16/97 3:03p Pkeet
 * Added the "WinInclude.hpp" include.
 * 
 * 4     12/12/97 6:07p Pkeet
 * Added a test to indicate if textures must be a power of two.
 * 
 * 3     12/11/97 8:01p Pkeet
 * Added types and functions for enumerated texture formats.
 * 
 * 2     12/10/97 5:32p Pkeet
 * Added a member function to return the device.
 * 
 * 1     12/10/97 10:28a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_DIRECT3D_HPP
#define HEADER_LIB_RENDERER_DIRECT3D_HPP


//
// Necessary includes.
//
#include "Lib/W95/Direct3DCards.hpp"
#include "dd.hpp"
#define D3D_OVERLOADS
#include <d3d.h>


//
// Forward declarations.
//
class CScreenTests;


//
// Macro switches.
//

// Switch for debugging Direct3D.
#define bDEBUG_DIRECT3D (1 && BUILDVER_MODE != MODE_FINAL)

// Macros for printing to the debug window.
#if bDEBUG_DIRECT3D
	#include "Lib/Sys/DebugConsole.hpp"
	#define PrintD3D(a)     dprintf(a)
	#define PrintD3D2(a, b) dprintf(a, b)
#else
	#define PrintD3D(a)
	#define PrintD3D2(a, b)
#endif // bDEBUG_DIRECT3D

// Switch to use colour keying in place of one bit alpha.
#define bCOLOUR_KEY (0)


//
// Enumerations.
//

enum EScreenFormats
// Common destination pixel formats.
{
	esf555,
	esf565,
	esfOther
};

enum EAllocationScheme
// Texture allocation scheme for Direct3D.
{
	easNonLocal,	// Non-local video memory (e.g., cards that texture from system memory using AGP).
	easManaged,		// Managed large memory (e.g., 12 Mb Voodoo 2).
	easFixed,		// Fixed memory (e.g., 32 Mb system RAM, NVidia 128 or a crappy card).
	easNone			// Direct3D should not be used.
};

// Type of D3D texture.
enum ED3DTextureType
{
	ed3dtexSCREEN_OPAQUE,
	ed3dtexSCREEN_TRANSPARENT,
	ed3dtexSCREEN_ALPHA,
	ed3dtexNUM_FORMATS
};

// Common D3D texture formats.
enum ED3DCommonFormat
{
	ed3dcom555_RGB,
	ed3dcom565_RGB,
	ed3dcom565_BGR,
	ed3dcom5551_BRGA,
	ed3dcom1555_ARGB,
	ed3dcom4444_ARGB,
	ed3dcom4444_BRGA,
	ed3dcomUnknown
};

// Memory model.
enum EMemoryModel
{
	emmSmall,	// 32 Mb memory or less.
	emmLarge	// 56 Mb memory or more.
};


//
// Class definitions.
//

//*********************************************************************************************
//
class CDirect3D
//
// Encapsulates D3D functionality
//
// Prefix: d3d
//
//**************************************
{
private:
	bool                bInitialized;		// Flag indicating that Direct3D has been initialized.
	bool                bUse;				// Flag indicating that Direct3D has been initialized.
	LPDIRECT3DDEVICE3   pDevice;			// Direct 3D device.
	LPDIRECT3D3         pD3D;				// Direct 3D object.
	DDPIXELFORMAT       ddpfFormats[ed3dtexNUM_FORMATS];	// Texture formats.
	ED3DCommonFormat    ad3dcomFormats[ed3dtexNUM_FORMATS];	// Common texture formats.
	bool                bDither;			// Flag indicates dithering is available.
	D3DCOLOR            d3dcolFogColour;	// Fog colour.
	bool                bEnableInit;		// Flag to enable initialization of Direct3D.
	bool                bPurged;			// Indicates Direct3D is purged.
	bool                bFullTexturing;	    // Flag indicates full rendering in hardware should be used.
	int                 iRecommendedMaxDim;	// Maximum recommended dimension.

	// Viewport variables.
	LPDIRECT3DVIEWPORT3 pd3dViewport;		// Viewport for the D3D surface.
	int                 iViewportWidth;		// Viewport width.
	int                 iViewportHeight;	// Viewport height;

	// Device capabilities.
	D3DDEVICEDESC       d3ddescHardware;	// Direct3D hardware device description.
	DDDEVICEIDENTIFIER  devidIdentifier;	// Device identifier.
	bool                bModulatedAlpha;	// Allows for the use of modulating alpha.
	EScreenFormats      esfScreenFormat;	// Format of destination screen pixels.
	bool                bHardwareWater;		// Flag to use hardware for rendering water.
	EAllocationScheme   easAllocation;		// Texture memory allocation scheme.
	bool                bTransparentAlpha;	// Flag to use alpha blending with transparencies.
	bool                bRegionUploads;		// Region upload flag.
	bool                bSharedSysBuffers;	// Shared buffers.
	bool                bFilterCaches;		// Filter caches.
	bool                bDeviceTested;		// Flag indicating the the current device has been tested.
	EMemoryModel        emmMemoryModel;		// Memory model based on available system RAM.
	EVideoCard          evcVideoCard;		// Video card type.
	bool                bD3DCacheFog;		// Flag to use hardware fogging for caches.
	bool                bSecondaryCard;		// Set to 'true' if the card is an add-on (e.g., a Voodoo 2).
	bool                bLocalVideoMem;		// Set to 'true' if there is local video memory for texturing.
	bool                bAlphaTransparency;	// Transparency requires alpha.
	bool                bShowLoadingScreen;	// Flag to display texture loading screen.
	int                 iAnimateCount;		// Animation screen count.
	bool                bFlipClear;			// Clear screen and z buffer immediately after a flip.
	bool                bPageManaged;		// Indicates if the app is page managed.

	// Texture dimension values.
	int                 iMinWidth;			// Minimum width of a texture.
	int                 iMaxWidth;			// Maximum width of a texture.
	int                 iMinHeight;			// Minimum height of a texture.
	int                 iMaxHeight;			// Maximum height of a texture.
	int                 iMinDim;			// Minimum dimension of a texture.
	int                 iMaxDim;			// Maximum dimension of a texture.
	bool                bPower2;			// Textures must be a power of two.
	bool                bSquareTextures;	// Flag indicating whether square textures are
											//required.
	int                 iTotalTextureMem;	// Total amount of texture memory available on the card.
	int                 iTerrainTextureMem;	// Amount of texture memory available for terrain.
	int                 iMaxTotalMemory;	// Maximum texture memory that can be allocated in bytes.
	int                 iTotalFreeMem;		// Total unaccounted for texture memory available on the card.

	// Conversion values for Alpha.
	uint32              u4AlphaRedMask;
	uint32              u4AlphaRedShift;
	uint32              u4AlphaGreenMask;
	uint32              u4AlphaGreenShift;
	uint32              u4AlphaBlueMask;
	uint32              u4AlphaBlueShift;
	uint32              u4AlphaAlphaMask;
	uint32              u4AlphaAlphaShift;

	// Conversion values for transparency.
	uint32              u4TransRedMask;
	uint32              u4TransRedShift;
	uint32              u4TransGreenMask;
	uint32              u4TransGreenShift;
	uint32              u4TransBlueMask;
	uint32              u4TransBlueShift;
	uint32              u4TransAlphaMask;
	uint32              u4TransAlphaShift;

public:

	//**********************************************************************************************
	//
	class CError
	//
	// An error handling class for DirectDraw errors.
	// Uses the assign operation to make error termination easy.
	// You only need the one that's declared below.
	//
	// Example:
	//		#include "dd.h"
	//
	//		IDirectDrawSurface*	pdds;
	//		DirectDraw::err = pdds->Flip();
	//		// If Flip returns anything other than 0, the program will show
	//		// an error dialog with the option to abort.
	//
	//**********************************
	{
	public:

		//**************************************************************************************
		//
		// Assignment operator.
		//

		//**************************************************************************************
		//
		HRESULT operator =
		(
			HRESULT hres_error	// An error code returned by a Direct3D function.
		)
		//
		// Displays an error message corresponding to hres_error if hres_error != D3D_OK. 
		//
		//******************************
		{
		#if bDEBUG_DIRECT3D

			// Process only error return codes.
			if (hres_error & 0xFFFF0000)
				ProcessError(hres_error);

		#endif // bDEBUG_DIRECT3D

			return hres_error;
		}

	private:

		//**************************************************************************************
		void ProcessError(HRESULT hres_error);

	};

	//**********************************************************************************************
	//
	class CCount
	//
	// An error handling class for DirectDraw errors.
	// Uses the assign operation to make error termination easy.
	// You only need the one that's declared below.
	//
	// Example:
	//		#include "dd.h"
	//
	//		IDirectDrawSurface*	pdds;
	//		DirectDraw::err = pdds->Flip();
	//		// If Flip returns anything other than 0, the program will show
	//		// an error dialog with the option to abort.
	//
	//**********************************
	{
	public:

		//**************************************************************************************
		//
		// Assignment operator.
		//

		//**************************************************************************************
		//
		void operator =
		(
			int i_count	// Reference count.
		);
		//
		// Displays an error message if the reference count is not zero. 
		//
		//******************************
	};

	CError err;		// Error variable.
	CCount count;	// Error variable.

public:

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	// Default constructor.
	CDirect3D
	(
	);

	// Destructor.
	~CDirect3D
	(
	);

	//*****************************************************************************************
	//
	// Member functions.
	//
	
	//*****************************************************************************************
	//
	forceinline LPDIRECT3DDEVICE3 pGetDevice
	(
	)
	//
	// Returns the D3D device pointer.
	//
	//**************************************
	{
		return pDevice;
	}
	
	//*****************************************************************************************
	//
	forceinline LPDIRECT3D3 pGetD3D
	(
	)
	//
	// Returns the D3D interface pointer.
	//
	//**************************************
	{
		AlwaysAssert(pD3D);
		return pD3D;
	}
	
	//*****************************************************************************************
	//
	forceinline EVideoCard evcGetVideoCard
	(
	) const
	//
	// Returns the video card type used for Direct3D.
	//
	//**************************************
	{
		return evcVideoCard;
	}          
	
	//*****************************************************************************************
	//
	void ReleaseD3D
	(
	);
	//
	// Releases the D3D interface pointer.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	bool bInitializeD3D
	(
	);
	//
	// Returns 'true' if the D3D interface pointer is successfully initialized.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	LPDIRECT3DVIEWPORT3 pd3dGetViewport
	(
	)
	//
	// Returns the viewport pointer.
	//
	//**************************************
	{
		return pd3dViewport;
	}
	
	//*****************************************************************************************
	//
	const D3DDEVICEDESC& d3ddescGet
	(
	) const
	//
	// Returns the description for the D3D device.
	//
	//**************************************
	{
		return d3ddescHardware;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bUseD3D
	(
	) const
	//
	// Returns 'true' if D3D should be used.
	//
	//**************************************
	{
		Assert((bUse && bEnableInit) || !bUse);
		return bUse;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bIsPageManaged
	(
	) const
	//
	// Returns 'true' if D3D should be used.
	//
	//**************************************
	{
		return bPageManaged;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bGetAlphaTransparency
	(
	) const
	//
	// Returns 'true' if turning alpha on is required for transparency.
	//
	//**************************************
	{
		return bAlphaTransparency;
	}
	
	//*****************************************************************************************
	//
	bool bGetFlipClear
	(
	) const
	//
	// Returns 'true' if a clear should be performed immediately after a flip.
	//
	//**************************************
	{
		return bFlipClear;
	}
	
	//*****************************************************************************************
	//
	void SetFlipClear
	(
		bool b_flipclear = true
	)
	//
	// Sets the 'bFlipClear' flag.
	//
	//**************************************
	{
		bFlipClear = b_flipclear;
	}
	
	//*****************************************************************************************
	//
	int iGetRecommendedMaxDim
	(
	) const
	//
	// Returns the recommended maximum dimension for textures.
	//
	//**************************************
	{
		Assert(bUse);
		return iRecommendedMaxDim;
	}
	
	//*****************************************************************************************
	//
	EAllocationScheme easAllocations
	(
	) const
	//
	// Returns the type of texture memory allocation scheme to be used by Direct3D.
	//
	//**************************************
	{
		return easAllocation;
	}
	
	//*****************************************************************************************
	//
	bool bUseD3DSky
	(
	) const
	//
	// Returns 'true' if D3D should be used.
	//
	//**************************************
	{
		return bUse;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bUseTransparentAlpha
	(
	) const
	//
	// Returns 'true' if alpha blending should be used with transparencies.
	//
	//**************************************
	{
		return bTransparentAlpha;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bUseCacheFog
	(
	) const
	//
	// Returns 'true' if D3D should be used for fogging render caches.
	//
	//**************************************
	{
		return bUse && bD3DCacheFog;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bFilterImageCaches
	(
	) const
	//
	// Returns 'true' if filtering for image caches should be used.
	//
	//**************************************
	{
		return bFilterCaches;
	}
	
	//*****************************************************************************************
	//
	void SetFilterImageCaches
	(
		bool b_filter_caches = true
	)
	//
	// Returns 'true' if filtering for image caches should be used.
	//
	//**************************************
	{
		bFilterCaches = b_filter_caches;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bUseD3DForImageCaches
	(
	) const
	//
	// Returns 'true' if D3D should be used for image caches.
	//
	//**************************************
	{
		Assert((bUse && bEnableInit) || !bUse);
		return bUse;
	}
	
	//*****************************************************************************************
	//
	bool bInitEnabled
	(
	) const;
	//
	// Returns 'true' if D3D should be used.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void SetInitEnable
	(
		bool b_enable
	);
	//
	// Sets the disable flag.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	forceinline bool bD3DWater
	(
	) const
	//
	// Returns 'true' if water is to be rendered in hardware.
	//
	//**************************************
	{
		return bUse && bModulatedAlpha && bHardwareWater;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bUseRegionUploads
	(
	) const
	//
	// Returns 'true' if regional uploads may be used.
	//
	//**************************************
	{
		return bRegionUploads;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bUseSharedSysBuffers
	(
	) const
	//
	// Returns 'true' if shared buffers may be used.
	//
	//**************************************
	{
		return bSharedSysBuffers;
	}
	
	//*****************************************************************************************
	//
	bool bTexCreatesAreSlow
	(
	) const
	//
	// Returns 'true' if texture create times are slow.
	//
	//**************************************
	{
		return false;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bUseSquareTextures
	(
	) const
	//
	// Returns 'true' if square textures must used.
	//
	//**************************************
	{
		return bSquareTextures;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bUseDithering
	(
	) const
	//
	// Returns 'true' dithering should be used.
	//
	//**************************************
	{
		return bDither;
	}
	
	//*****************************************************************************************
	//
	forceinline bool bMustBePowerOfTwo
	(
	) const
	//
	// Returns 'true' if D3D should be used.
	//
	//**************************************
	{
		// If D3D is not being used, textures can be any size.
		if (!bUse)
			return false;
		return bPower2;
	}
	
	//*****************************************************************************************
	//
	uint32 u4ConvertToAlpha
	(
		float f_red,
		float f_green,
		float f_blue,
		float f_alpha
	) const;
	//
	// Returns the alpha pixel for the given input values.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	uint32 u4ConvertToTranparency
	(
		float f_red,
		float f_green,
		float f_blue,
		bool  b_transparent = false
	) const;
	//
	// Returns the tranparent pixel for the given input values.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	static bool bD3DCapable
	(
	);
	//
	// Returns 'true' if D3D is successfully set up.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	bool bInitialize
	(
	);
	//
	// Returns 'true' if D3D is successfully set up.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	bool bInitialize
	(
		LPDIRECTDRAWSURFACE4 pdds_backbuffer,	// Pointer to the backbuffer currently being used.
		LPDIRECTDRAWSURFACE4 pdds_frontbuffer	// Pointer to the frontbuffer currently being used.
	);
	//
	// Returns 'true' if D3D is successfully set up.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void Uninitialize
	(
	);
	//
	// Undoes everything that 'bInitialize' does.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	DDPIXELFORMAT ddpfGetPixelFormat
	(
		ED3DTextureType ed3dtex
	) const;
	//
	// Returns the pixel format for the request type.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	forceinline D3DCOLOR d3dcolGetFogColour
	(
	) const
	//
	// Returns the fog colour in D3D format.
	//
	//**************************************
	{
		return d3dcolFogColour;
	}
	
	//*****************************************************************************************
	//
	void Purge
	(
	);
	//
	// Purges D3D surfaces.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void PurgePrimary
	(
	);
	//
	// Calls 'Purge' only if the card is not a secondary card.
	//
	// Notes:
	//		Secondary cards like the Voodoo 2 do not share memory with the primary display
	//		device, therefore textures and so on will not become corrupted if the primary
	//		display goes back to the desktop (i.e., when the application is alt-tabbed).
	//		Direct3D cards that are also the primary display cards must purge textures because
	//		the textures may become corrupted.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void Restore
	(
	);
	//
	// Restores D3D surfaces.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void SetTextureMinMax
	(
		int& ri_width,			// Width of texture
		int& ri_height,			// Height of texture.
		bool b_truncate = true	// Truncates width and height instead of rounding.
	) const;
	//
	// Clamps the texture coordinates to lay within the minimum and maximum allowed values.
	// Also sets the values to be square or power of two as required.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void SetTextureMinMaxSquare
	(
		int& ri_dim	// Width and height of texture
	) const;
	//
	// Clamps the texture coordinates to lay within the minimum and maximum allowed values.
	// Also sets the values to be square or power of two as required.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	bool bValidTextureDimensions
	(
		int i_width,	// Width of texture
		int i_height	// Height of texture.
	) const;
	//
	// Returns 'true' if the dimensions of the texture are in a valid range.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	int iGetTotalTextureMem
	(
	) const;
	//
	// Returns the total amount of texture memory available in the card.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void AllocateTextureMemory
	(
	);
	//
	// Allocates texture memory to sky, water, terrain and image caches.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	int iGetTotalFreeMem
	(
	) const
	//
	// Returns the total amount of free texture memory available.
	//
	//**************************************
	{
		return iTotalFreeMem;
	}
	
	//*****************************************************************************************
	//
	bool bUseFullTexturing
	(
	) const
	//
	// Returns 'true' if using the rendering for regular texture rendering.
	//
	//**************************************
	{
		return bFullTexturing && bUse;
	}
	
	//*****************************************************************************************
	//
	ED3DCommonFormat d3dcomGetCommonFormat
	(
		ED3DTextureType ed3dtex	// Texture format.
	) const
	//
	// Returns the common format for a given texture type.
	//
	// Notes:
	//		Optimized routines can be written for common format types.
	//
	//**************************************
	{
		return ad3dcomFormats[ed3dtex];
	}
	
	//*****************************************************************************************
	//
	void ReportPerFrameStats
	(
	) const;
	//
	// Outputs per frame statistics on Direct3D to the debug console if 'bDEBUG_DIRECT3D'
	// is enabled.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void ClearZBuffer
	(
	);
	//
	// Clears the Z buffer.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	forceinline bool bUseAlpha
	(
	) const
	//
	// Returns 'true' alpha textures may be used.
	//
	//**************************************
	{
		return bModulatedAlpha;
	}
	
	//*****************************************************************************************
	//
	forceinline EScreenFormats esfGetScreenFormat
	(
	) const
	//
	// Returns the screen format currently in use.
	//
	//**************************************
	{
		return esfScreenFormat;
	}
	
	//*****************************************************************************************
	//
	void AnimateLoadingScreen
	(
		int i_width,
		int i_height
	)
	//
	// Animates the loading screen if the 'bShowLoadingScreen' flag is set.
	//
	//**************************************
	{
		if (!bShowLoadingScreen)
			return;
		AnimateLoadingScreenPriv(i_width, i_height);
	}

private:

	class CPriv;
	friend class CPriv;
	friend class CScreenTests;

	//*****************************************************************************************
	void AnimateLoadingScreenPriv(int i_width, int i_height);

};


//
// Global variables.
//

// Global Direct3D object.
extern CDirect3D d3dDriver;


#endif // HEADER_LIB_RENDERER_DIRECT3D_HPP