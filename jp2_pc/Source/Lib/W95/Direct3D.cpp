/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of Direct3D.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/W95/Direct3D.cpp                                                  $
 * 
 * 134   10/08/98 10:58p Pkeet
 * Changed memory allocation slightly.
 * 
 * 133   10/07/98 7:52a Pkeet
 * 
 * 132   10/07/98 6:10a Pkeet
 * Made a minimum texture allocation.
 * 
 * 131   10/06/98 5:51a Pkeet
 * Added the page managed flag.
 * 
 * 130   10/05/98 11:41p Pkeet
 * Tweaked numbers.
 * 
 * 129   10/05/98 5:45a Pkeet
 * Increased memory for terrain.
 * 
 * 128   10/02/98 5:18p Pkeet
 * Allocation is less for non-shared buffers.
 * 
 * 127   10/01/98 8:39p Pkeet
 * Reserved more memory for Direct3D.
 * 
 * 126   10/01/98 7:10p Pkeet
 * Paranoid setting of the 'bUse' flag.
 * 
 * 125   9/29/98 5:41p Pkeet
 * Tweaked memory allocations.
 * 
 * 124   9/27/98 10:02p Pkeet
 * Added a global function to reset conversion tables.
 * 
 * 123   9/27/98 2:08a Mmouni
 * Changed d3d surface load progress.
 * 
 * 122   9/17/98 11:12p Shernd
 * First pass on updating during d3d surface setup
 * 
 * 121   9/15/98 5:34p Pkeet
 * Removed unnecessary code in 'Uninitialize.'
 * 
 * 120   9/11/98 3:28p Pkeet
 * Disabled crash bug when switching resolutions.
 * 
 * 119   9/09/98 7:41p Pkeet
 * Added functions to show that the computer is working while it loads Direct3D surfaces.
 * 
 * 118   9/05/98 7:52p Pkeet
 * Added support for the Matrox G100.
 * 
 * 117   9/02/98 6:05p Pkeet
 * Added the 'EvictManagedTextures' call before purging.
 * 
 * 116   9/01/98 12:44a Pkeet
 * The default render state is now set only once in the 'Restore' member function. Fixed bug
 * clearing the viewport when it is shrunk.
 * 
  * 
 **********************************************************************************************/

#include <memory.h>
#include "common.hpp"
#include "Direct3D.hpp"

#include "Lib/View/Clut.hpp"
#include "Lib/Sys/RegInit.hpp"
#include "Lib/GeomDBase/Terrain.hpp"
#include "Lib/GeomDBase/TerrainTexture.hpp"
#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/Renderer/Sky.hpp"
#include "Lib/EntityDBase/Water.hpp"
#include "Lib/Renderer/PipeLine.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Renderer/RenderCache.hpp"
#include "Lib/Std/MemLimits.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/View/Direct3DRenderState.hpp"
#include "Lib/View/AGPTextureMemManager.hpp"
#include "Lib/View/RasterPool.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"
#include "Lib/Sys/reg.h"
#include "Lib/View/Direct3DRenderState.hpp"
#include "Lib/W95/Direct3DQuery.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/View/ColourBase.hpp"
#include "Lib/View/RasterConvertD3D.hpp"
#include "Lib/Sys/DWSizeStruct.hpp"
#include "Lib/Sys/DebugConsole.hpp"


//
// Macros.
//

// Convert kilobytes to bytes.
#define iBytesKB(X) (X << 10)

// Convert megabytes to bytes.
#define iBytesMB(X) (X << 20)

// Convert bytes to kilobytes.
#define iKB(X) (X >> 10)

// Convert bytes to megabytes.
#define iMB(X) (X >> 20)

#ifndef PFNWORLDLOADNOTIFY
typedef uint32 (__stdcall * PFNWORLDLOADNOTIFY)(uint32 dwContext, uint32 dwParam1, uint32 dwParam2, uint32 dwParam3);
#endif

//
// Constants.
//

// Minimum and maximum allowed texture memory for terrain.
const int iMinTerrain = iBytesKB(1280);
const int iMaxTerrain = iBytesKB(1920);

// Minimum amount of memory for AGP texturing.
const int iMinTexture = iBytesKB(2560);

// Fixed texture memory allocations.
const int iAllocSky   = iBytesKB(128);
const int iAllocWater = iBytesKB(384);

// Threshold for using a large memory model.
static const uint u64MbThreshold = iBytesMB(56);

// Threshold for using a very large memory model.
static const uint u96MbThreshold = iBytesMB(92);

// Percentage for caches.
const int iCachePercent = 40;


//
// External declaractions.
//
extern rptr<CTexturePageManager>    ptexmCacheTextures;
extern PFNWORLDLOADNOTIFY           g_pfnWorldLoadNotify;
extern uint32                       g_u4NotifyParam;
extern bool                         bIsTrespasser;

#if bTRACK_D3D_RASTERS
	void PrintLeftoverD3D();
#endif // bTRACK_D3D_RASTERS

static CProfileStat psExecuteHardwareClear("HW Clear", &proProfile.psRender);


//
// Function prototypes and simple functions.
//

//*********************************************************************************************
//
uint uGetMaskBits
(
	uint32 u4	// Value to count bits in.
)
//
// Returns the number of bits set in the value, or zero if no bits are set.
//
//**************************************
{
	uint u_num_bits = 0;

	while ((u4 & 1) == 0 && u4 != 0)
	{
		++u_num_bits;
		u4 >>= 1;
	}
	return u_num_bits;
}

//*****************************************************************************************
//
inline bool bRedLow
(
	const DDPIXELFORMAT& ddpf
)
//
// Returns 'true' if red is low.
//
//**********************************
{
	return ddpf.dwRBitMask == 0x001F;
}

static DWORD 
FlagsToBitDepth(DWORD dwFlags) 
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



//
// Class definitions.
//

//**********************************************************************************************
//
class CDirect3D::CPriv : public CDirect3D
//
// Private member functions for class 'CDirect3D.'
//
//**************************************
{
public:
	
	//*****************************************************************************************
	//
	void SetMemoryModel
	(
	);
	//
	// Sets the 'emmMemoryModel' data member based on available system memory.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	uint uLargePool
	(
	)
	//
	// Returns the amount of memory to use for a pool based on the 'emmMemoryModel' data member.
	//
	//**************************************
	{
		// Small memory allocatation.
		if (iTotalFreeMem < iBytesMB(6))
			return iBytesKB(512);

		// Non-shared texture allocation.
		if (iTotalFreeMem < iBytesMB(18))
			return iBytesKB(1152);

		// Default normal memory model allocation.
		return iBytesKB(1792);
	}
	
	//*****************************************************************************************
	//
	bool bUseNonLocalMemory
	(
	);
	//
	// Returns 'true' if nonlocal (AGP system) memory is to be used for texturing.
	//
	// Notes:
	//		If this function is successful, the 'easAllocation' data member should be set to
	//		'easNonLocal.'
	//
	//**************************************
	
	//*****************************************************************************************
	//
	bool bUseManagedMemory
	(
	);
	//
	// Returns 'true' if managed memory is to be used for texturing.
	//
	// Notes:
	//		If this function is successful, the 'easAllocation' data member should be set to
	//		'easManaged.'
	//
	//**************************************
	
	//*****************************************************************************************
	//
	bool bAllocateManagedMemory
	(
	);
	//
	// Returns 'true' if managed memory is to be used for texturing.
	//
	// Notes:
	//		If this function is successful, the 'easAllocation' data member should be set to
	//		'easManaged.'
	//
	//**************************************
	
	//*****************************************************************************************
	//
	bool bAllocateNonLocalMemory
	(
	);
	//
	// Returns 'true' if non-local (agp system) memory is to be used for texturing.
	//
	// Notes:
	//		If this function is successful, the 'easAllocation' data member should be set to
	//		'easNonLocal.'
	//
	//**************************************
	
	//*****************************************************************************************
	//
	int iGetMaxTextureRam
	(
	);
	//
	// Returns the maximum amount of texture memory that may be used.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	int iGetWaterAlloc
	(
	);
	//
	// Returns the amount of memory in bytes allocated for water.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void SetupLoadingScreens
	(
	);
	//
	// Prepares the animating loading screens for use.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void WriteTextToLoadScreen
	(
		const char* str	// String to write.
	);
	//
	// Clears the load screen and writes text to it.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void SetModulatedAlpha
	(
	);
	//
	// Sets the modulate alpha flag if modulated alpha can be supported.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	bool bSetupViewport
	(
		LPDIRECTDRAWSURFACE4 pdds_backbuffer
	);
	//
	// Adds a viewport to the D3D object.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bEnumerateTextureFormats
	(
	);
	//
	// Builds a list of valid texture formats for the D3D device.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SpecialCaseSetup
	(
	);
	//
	// Performs awkward special case code for specific video cards.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetAlphaFormat
	(
	);
	//
	// Sets the alpha format.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetScreenFormat
	(
		LPDIRECTDRAWSURFACE4 pdds_backbuffer	// Pointer to the backbuffer.
	);
	//
	// Sets the screen pixel format.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetTransparentFormat
	(
	);
	//
	// Sets the transparent format.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetTotalTextureMem
	(
	);
	//
	// Sets the total amount of texture memory available.
	//
	// Notes:
	//		This function must take into account the amount of memory allocated to the front
	//		and back buffers in cards that have unified memory architectures.
	//
	// To do:
	//		Ensure that allocations for cards that can have system memory texturing reflects
	//		the amount available to it.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void SetFog
	(
	);
	//
	// Sets the necessary flags in D3D for fogging.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetD3DFlag
	(
	);
	//
	// Sets the 'bUse' flag.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetCommonFormats
	(
	);
	//
	// Sets common formats for Direct3D textures.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetDeviceDescriptions
	(
	);
	//
	// Sets Direct3D descriptions and capabilites structures.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetLocalVideoMemFlag
	(
	);
	//
	// Set the 'bLocalVideoMem' flag.
	//
	//**************************************

	//*****************************************************************************************
	//
	int iLargeMemAllocMB
	(
	);
	//
	// Returns the recommended allocation in megabytes.
	//
	//**************************************

};


//
// Class implementations.
//

//*********************************************************************************************
//
// Implementation of CDirect3D.
//

	//******************************************************************************************
	//
	// class CError implementation.
	//

		//**************************************************************************************
		void CDirect3D::CError::ProcessError(HRESULT hres_error)
		{
			//
			// Convert i_err to a resource symbol, and call the standard TerminalError function.
			// The app resources contain strings for all DirectDraw error codes.
			// The conversion consists of taking the low 16 bits of the error code, and adding
			// it to ERROR_DD_BASE.
			//
			if (srd3dRenderer.bIsBusy())
				srd3dRenderer.SetD3DMode(ed3drSOFTWARE_LOCK);
			TerminalError(ERROR_D3D_BASE + (hres_error & 0xFFFF), true, "Direct3D Error.");
		}

	//******************************************************************************************
	//
	// class CCount implementation.
	//

		//**************************************************************************************
		void CDirect3D::CCount::operator =(int i_count)
		{
			//
			// Convert i_err to a resource symbol, and call the standard TerminalError function.
			// The app resources contain strings for all DirectDraw error codes.
			// The conversion consists of taking the low 16 bits of the error code, and adding
			// it to ERROR_DD_BASE.
			//
			if (i_count == 0)
				return;
			TerminalError(ERROR_D3D_REFERENCECOUNT, true, "Direct3D Error.");
		}

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	//*****************************************************************************************
	CDirect3D::CDirect3D()
		: bInitialized(false), pD3D(0), pDevice(0), pd3dViewport(0), bPower2(true),
		  bSquareTextures(true), iTotalTextureMem(-1), iTerrainTextureMem(iMinTerrain),
		  bPurged(true), iTotalFreeMem(0), bFullTexturing(false),
		  bModulatedAlpha(false), bHardwareWater(false), easAllocation(easNone),
		  bTransparentAlpha(true), iRecommendedMaxDim(128), bDeviceTested(false),
		  emmMemoryModel(emmSmall), evcVideoCard(evcUnknown), bD3DCacheFog(true),
		  bSecondaryCard(false), bLocalVideoMem(true), bAlphaTransparency(false),
		  bShowLoadingScreen(false), bFlipClear(false)
	{
		// Test if D3D should be used.
		priv_self.SetD3DFlag();
		SetInitEnable(true);

		bRegionUploads    = true;
		bSharedSysBuffers = true;
		bFilterCaches     = false;

		// Set the memory model.
		priv_self.SetMemoryModel();
	}

	//*****************************************************************************************
	CDirect3D::~CDirect3D()
	{
		// Put Direct3D away.
		Uninitialize();
	}

	//*****************************************************************************************
	//
	// Member functions.
	//
	
	//*****************************************************************************************
	bool CDirect3D::bInitEnabled() const
	{
		return bEnableInit && bGetD3D();
	}
	
	//*****************************************************************************************
	void CDirect3D::SetInitEnable(bool b_enable)
	{
		bEnableInit = b_enable;
		Assert(!(bUse && !bEnableInit));
	}	

	//*****************************************************************************************
	bool CDirect3D::bInitializeD3D()
	{
		AlwaysAssert(pD3D == 0);

		HRESULT hres;

		// If Direct3D is already initialized, uninitialize it.
		if (bInitialized)
			return true;

		// Do nothing if Direct3D is not is use.
		priv_self.SetD3DFlag();
		if (!bUse)
		{
			return false;
		}

		// If Direct3D does not have its initialization flag enabled, do nothing.
		if (!bInitEnabled())
		{
			SetD3D(false);
			bUse = false;
			return false;

		}

		//
		// Set up Direct3D.
		//
		AlwaysAssert(DirectDraw::pdd4);

		// Query the DirectDraw object to get the D3D object.
		hres = DirectDraw::pdd4->QueryInterface(IID_IDirect3D3, (void**)&pD3D);
		if (FAILED(hres))
		{
			SetD3D(false);
			bUse = false;
			return false;
		}
		Assert(pD3D);
		PrintD3D(">>>>>>New direct 3d object created\n");

		SetInitEnable(true);
		bDeviceTested = false;
		return true;
	}
	
	//*****************************************************************************************
	void CDirect3D::ReleaseD3D()
	{
		if (pD3D)
		{
			int i_ref = pD3D->Release();
			pD3D = 0;
			PrintD3D2(">>>>>>Direct3D object released (%ld).\n", i_ref);
		};
		bUse = false;
		bDeviceTested = false;
	}

	//*****************************************************************************************
	bool CDirect3D::bInitialize(LPDIRECTDRAWSURFACE4 pdds_backbuffer, LPDIRECTDRAWSURFACE4 pdds_frontbuffer)
	{
		// If Direct3D is already initialized, uninitialize it.
		if (bInitialized)
			return true;

		// Determine the video card type.
		evcVideoCard = evcGetCard(DirectDraw::pdd4);

	#if bDEBUG_DIRECT3D

		// Output card name if specifically detected.
		switch (evcVideoCard)
		{
			case evcMatroxG100:
				dprintf("Video card: Matrox MGA-G100 Productiva\n");
				break;
			case evcATIRage:
				dprintf("Video card: ATI\n");
				break;
			case evcNVidia128:
				dprintf("Video card: NVidia Riva 128\n");
				break;
			case evcPermedia2:
				dprintf("Video card: Permedia 2\n");
				break;
			default:
				dprintf("Video card unknown\n");
				break;
		}

	#endif // bDEBUG_DIRECT3D

		// Set defaults.
		iMaxTotalMemory = priv_self.iGetMaxTextureRam();
		iRecommendedMaxDim = iGetRecommendedTextureMaxDim();

		PrintD3D2("\nRecommended D3D max texture dimension: %ld\n", iRecommendedMaxDim);

		// Set the screen pixel format as 555 or 565.
		priv_self.SetScreenFormat(pdds_backbuffer);

		//
		// Create the D3D device.
		//

		// Get the GUID for the specific Direct3D driver.
		GUID guid = ReadD3DGUID();
		if (!pConvertGUID(guid))
		{
			Uninitialize();
			bUse = false;
			return false;
		}

		// Create the device using the guid.
#if VER_DEBUG
		err = pD3D->CreateDevice(guid, pdds_backbuffer, &pDevice, 0);
		Assert(pDevice);
#else // VER_DEBUG
		if (pD3D->CreateDevice(guid, pdds_backbuffer, &pDevice, 0) != D3D_OK)
		{
			SetD3D(false);
			Uninitialize();
			bUse = false;
			return false;
		}
		AlwaysAssert(pDevice);
#endif // VER_DEBUG
		PrintD3D(">>>>>>New direct 3d device created\n");

		//3D->EvictManagedTextures();

		// Set device description and capability structures.
		priv_self.SetDeviceDescriptions();

		// Set up the viewport.
		if (!priv_self.bSetupViewport(pdds_backbuffer))
		{
			SetD3D(false);
			Uninitialize();
			bUse = false;
			return false;
		}
		PrintD3D(">>>>>>New direct 3d viewport created\n");

		//
		// Get texture formats.
		//
		// Notes:
		//		This enumeration call will set the following private member functions:
		//
		//			Set capabilities.
		//
		if (!priv_self.bEnumerateTextureFormats())
		{
			SetD3D(false);
			Uninitialize();
			bUse = false;
			return false;
		}

		// Perform card specific setup.
		priv_self.SpecialCaseSetup();

		// Set filtering on by default for Direct3D.
		Assert(prnshMain);
		prnshMain->rensetSettings.seterfState += erfFILTER;

		CEntityWater::bInterp = false;

		// Enable fog.
		priv_self.SetFog();

		// Set common formats.
		priv_self.SetCommonFormats();

		// Run conformance tests.
		if (!bDeviceTested)
		{
			ScreenTests(pdds_backbuffer, pdds_frontbuffer);
			bDeviceTested = true;
		}

		if (!bSharedSysBuffers && emmMemoryModel == emmSmall)
			iMaxTotalMemory = Min(iMaxTotalMemory, 5500 * 1024);

		// Set the dithering flag.
		bDither = bGetDither();

		// Set the various flags.
		bD3DCacheFog       = true;
		bAlphaTransparency = false;
		switch (evcVideoCard)
		{
			case evcMatroxG100:
				bAlphaTransparency = true;
				bD3DCacheFog = true;
				break;

			case evcATIRage:
				bD3DCacheFog = false;
				break;
		}

		// Set the secondary card flag.
		{
			GUID guid = ReadDDGUID();
			bSecondaryCard = pConvertGUID(guid) != 0;
		}

		// Set the 'bLocalVideoMem' flag.
		priv_self.SetLocalVideoMemFlag();

		ResetD3DConversions();

		// Return a flag indicating success.
		bUse = true;
		bInitialized = true;
		return true;
	}
	
	//*****************************************************************************************
	bool CDirect3D::bInitialize()
	{
		if (!prasMainScreen)
		{
			Uninitialize();
			return false;
		}
		if (!bD3DCapable())
		{
			Uninitialize();
			return false;
		}
		LPDIRECTDRAWSURFACE4 pdds_back  = prasMainScreen->pddsDraw4;
		LPDIRECTDRAWSURFACE4 pdds_front = prasMainScreen->GetPrimarySurface4();
		return bInitialize(pdds_back, pdds_front);
	}
	
	//*****************************************************************************************
	bool CDirect3D::bD3DCapable()
	{
		bool b_vid_raster = true;
		if (prasMainScreen && prasMainScreen->pddsDraw4)
		{
			CDDSize<DDSURFACEDESC2> sd;

			// Test to ensure that the backbuffer is in video memory.
			LPDIRECTDRAWSURFACE4 pdds = prasMainScreen->pddsDraw4;
			Assert(pdds);

			// Retrieve the surface format info (for pitch only).
			DirectDraw::err = pdds->GetSurfaceDesc(&sd);
			//b_vid_raster = b_vid_raster && (sd.ddsCaps.dwCaps & DDSCAPS_3DDEVICE);
			//b_vid_raster = b_vid_raster && (sd.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER);
			b_vid_raster = b_vid_raster && (sd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY);
			b_vid_raster = b_vid_raster && bGetD3D();
		}
		return bGetD3D() && b_vid_raster;
	}
	
	//*****************************************************************************************
	void CDirect3D::Uninitialize()
	{
		// If Direct3D is already uninitialized, do nothing.
		if (!bInitialized)
		{
			bUse = false;
			return;
		}

		// Remove all dependant textures.
		Purge();

		// Release the viewport.
		if (pd3dViewport)
		{
			int i_ref = pd3dViewport->Release();
			pd3dViewport = 0;
			PrintD3D2(">>>>>>New direct 3d viewport released (%ld).\n", i_ref);
		}

		// Release the device.
		if (pDevice)
		{
			int i_ref = pDevice->Release();
			pDevice = 0;
			PrintD3D2(">>>>>>New direct 3d device released (%ld).\n", i_ref);
		}

		// Turn off water alpha.
		CEntityWater::bAlpha = false;

		// Turn on water interpolation.
		CEntityWater::bInterp = true;

	#if !BILINEAR_FILTER
		// Switch filtering off by if required for the software rasterizer.
		Assert(prnshMain);
		prnshMain->rensetSettings.seterfState -= erfFILTER;
	#endif // !BILINEAR_FILTER

		// Indicated that the Direct3D object is uninitialized.
		bUse = false;
		bInitialized = false;
	}

	//*****************************************************************************************
	DDPIXELFORMAT CDirect3D::ddpfGetPixelFormat(ED3DTextureType ed3dtex) const
	{
		Assert(ddpfFormats[ed3dtex].dwSize);

		return ddpfFormats[ed3dtex];
	}
	
	//*****************************************************************************************
	uint32 CDirect3D::u4ConvertToAlpha(float f_red, float f_green, float f_blue, float f_alpha) const
	{
		uint32 u4_red   = uint32(f_red   * float(u4AlphaRedMask)   + 0.5f);
		uint32 u4_green = uint32(f_green * float(u4AlphaGreenMask) + 0.5f);
		uint32 u4_blue  = uint32(f_blue  * float(u4AlphaBlueMask)  + 0.5f);
		uint32 u4_retval;

		if (u4_red > u4AlphaRedMask)
			u4_red = u4AlphaRedMask;

		if (u4_green > u4AlphaGreenMask)
			u4_green = u4AlphaGreenMask;

		if (u4_blue > u4AlphaBlueMask)
			u4_blue = u4AlphaBlueMask;

		u4_retval = (u4_red << u4AlphaRedShift) | (u4_green << u4AlphaGreenShift) |
			        (u4_blue << u4AlphaBlueShift);

		// Add alpha only if it is available.
		if (bModulatedAlpha)
		{
			uint32 u4_alpha = uint32(f_alpha * float(u4AlphaAlphaMask) + 0.5f);
			if (u4_alpha > u4AlphaAlphaMask)
				u4_alpha = u4AlphaAlphaMask;
			u4_retval |= u4_alpha << u4AlphaAlphaShift;
		}

		return u4_retval;
	}
	
	//*****************************************************************************************
	uint32 CDirect3D::u4ConvertToTranparency(float f_red, float f_green, float f_blue, bool b_transparent) const
	{
		uint32 u4_red   = uint32(f_red   * float(u4TransRedMask)   + 0.5f);
		uint32 u4_green = uint32(f_green * float(u4TransGreenMask) + 0.5f);
		uint32 u4_blue  = uint32(f_blue  * float(u4TransBlueMask)  + 0.5f);
		uint32 u4_retval;

		if (u4_red > u4TransRedMask)
			u4_red = u4TransRedMask;

		if (u4_green > u4TransGreenMask)
			u4_green = u4TransGreenMask;

		if (u4_blue > u4TransBlueMask)
			u4_blue = u4TransBlueMask;

		u4_retval = (u4_red << u4TransRedShift) | (u4_green << u4TransGreenShift) |
			        (u4_blue << u4TransBlueShift);

		// Add Trans only if it is available.
		if (b_transparent)
		{
			u4_retval |= u4TransAlphaMask << u4TransAlphaShift;
		}

		return u4_retval;
	}
	
	//*****************************************************************************************
	void CDirect3D::SetTextureMinMax(int& ri_width, int& ri_height, bool b_truncate) const
	{
		// Do nothing if Direct3D is not in use.
		if (!bUse)
			return;

		if (b_truncate)
		{
			ri_width  = (ri_width * 2) / 3;
			ri_height = (ri_height * 2) / 3;
			if (bSquareTextures)
			{
				int i_dim = Max(ri_width, ri_height);
				if (bPower2)
				{
					if (!bPowerOfTwo(i_dim))
						i_dim = NextPowerOfTwo(i_dim) >> 1;
				}
				SetTextureMinMaxSquare(i_dim);
				ri_width  = i_dim;
				ri_height = i_dim;
			}
			else
			{
				if (bPower2)
				{
					if (!bPowerOfTwo(ri_width))
						ri_width = NextPowerOfTwo(ri_width) >> 1;
					if (!bPowerOfTwo(ri_height))
						ri_height = NextPowerOfTwo(ri_height) >> 1;
				}
			}
		}
		else
		{
			if (bSquareTextures)
			{
				int i_dim = Max(ri_width, ri_height);
				if (bPower2)
					i_dim = NextPowerOfTwo(i_dim);
				SetTextureMinMaxSquare(i_dim);
				ri_width  = i_dim;
				ri_height = i_dim;
			}
			else
			{
				if (bPower2)
				{
					ri_width  = NextPowerOfTwo(ri_width);
					ri_height = NextPowerOfTwo(ri_height);
				}
			}
		}
		SetMinMax(ri_width,  iMinWidth,  iMaxWidth);
		SetMinMax(ri_height, iMinHeight, iMaxHeight);
	}
	
	//*****************************************************************************************
	void CDirect3D::SetTextureMinMaxSquare(int& ri_dim) const
	{
		// Do nothing if Direct3D is not in use.
		if (!bUse)
			return;

		if (bPower2)
		{
			ri_dim = NextPowerOfTwo(ri_dim);
		}
		SetMinMax(ri_dim, iMinDim, iMaxDim);
	}
	
	//*****************************************************************************************
	bool CDirect3D::bValidTextureDimensions(int i_width, int i_height) const
	{
		// Check only that the dimensions are positive if Direct3D is not being used.
		if (!bUse)
		{
			return i_width > 0 && i_height > 0;
		}

		// Test that the texture is square if required.
		if (bSquareTextures)
			if (i_width != i_height)
				return false;

		// Test that the texture dimensions are a power of two if required.
		if (bPower2)
			if (!bPowerOfTwo(i_width) || !bPowerOfTwo(i_height))
				return false;

		// Test that the dimensions are within the specified minimum and maximum.
		if (!bWithin(i_width, iMinWidth,  iMaxWidth) || !bWithin(i_height, iMinHeight, iMaxHeight))
			return false;

		// Success.
		return true;
	}
	
	//*****************************************************************************************
	void CDirect3D::PurgePrimary()
	{
		// Do nothing if Direct3D has already been purged.
		if (bPurged)
			return;

		// If the card has no local texture memory, do nothing.
		if (!bLocalVideoMem)
			return;

		// If the card is a secondary card, do nothing.
		if (bSecondaryCard)
			return;

		// All else has failed. Purge it.
		Purge();
	}
	
	//*****************************************************************************************
	void CDirect3D::Purge()
	{
		// Do nothing if Direct3D has already been purged.
		if (bPurged)
			return;

		if (bUse)
			srd3dRenderer.SetD3DMode(ed3drSOFTWARE_LOCK);
		bFlipClear = false;

		PrintD3D("Purging D3D\n");

		// Delete existing D3D rasters.
		if (easAllocation == easManaged)
			pD3D->EvictManagedTextures();

		// Remove all image caches.
		PurgeRenderCaches();
		
		// Remove all water textures.
		PurgeWaterTextures();

		// Reset the Direct3D auxilary screen renderer.
		srd3dRenderer.Reset();
		agpAGPTextureMemManager.Purge();
		if (gpskyRender)
			gpskyRender->PurgeD3D();
		CEntityWater::SetTextureMemSize(0);
		NMultiResolution::CTextureNode::SetTextureMemSize(0);
		CRenderCache::SetTextureMemSize(0, 0);

		// Turn scheduler off for a while.
		srd3dRenderer.SetScheduler();

		// Remove shared system memory DirectDraw surfaces used by d3d rasters for uploading.
		CRasterD3D::ReleaseSharedSurfaces();

	#if bTRACK_D3D_RASTERS
		// Print leftover d3d rasters to the debugger.
		PrintLeftoverD3D();
	#endif // bTRACK_D3D_RASTERS

		// Set the purge flag.
		bPurged = true;
	}
	
	//*****************************************************************************************
	void CDirect3D::Restore()
	{
		// Do nothing if Direct3D is not is use or Direct3D is not purged.
		if (!bUse || !bPurged)
			return;

		srd3dRenderer.SetD3DMode(ed3drSOFTWARE_LOCK);
		d3dstState.SetDefault();

		// Give the user a sign that stuff is happening.
		priv_self.SetupLoadingScreens();
		bShowLoadingScreen = true;
		iAnimateCount = 0;
		bFlipClear = false;

		// Assign texture memory to the various subsystems.
		AllocateTextureMemory();

		// Restore sky.
		if (gpskyRender)
		{
			gpskyRender->InitializeForD3D();
		}
		bPurged = false;

		// Restore non-pageable textures for full agp texturing.
		agpAGPTextureMemManager.RestoreNonpageable();

		// Clear screens.
		bShowLoadingScreen = false;
		bFlipClear = true;
	}
	
	//*****************************************************************************************
	int CDirect3D::iGetTotalTextureMem() const
	{
		return iTotalTextureMem;
	}
	
	//*****************************************************************************************
	void CDirect3D::AllocateTextureMemory()
	{
		// Do nothing if Direct3D is not in use.
		if (!bUse)
			return;

		Purge();

		// Set the page managed flag.
		bPageManaged = bGetPageManaged();

	#if VER_TEST

		if (bPageManaged)
			PrintD3D("D3D using page manager\n");
		else
			PrintD3D("D3D not using page manager\n");

	#endif

		// Attempt to implement non-local (agp sytem) memory for large memory cards/machines.
		if (priv_self.bUseNonLocalMemory())
			if (priv_self.bAllocateNonLocalMemory())
				return;

		// Attempt to implement the managed memory scheme.
		if (priv_self.bUseManagedMemory())
			if (priv_self.bAllocateManagedMemory())
				return;

		// If no scheme is successful, forget Direct3D.
		bUse = false;
		Uninitialize();
	}
	
	//*****************************************************************************************
	void CDirect3D::ReportPerFrameStats() const
	{
	#if bDEBUG_DIRECT3D
		DDSCAPS2 ddscaps;
		DWORD    dw_total;
		DWORD    dw_free;

		// Query to get the available amount of texture memory.
		memset(&ddscaps, 0, sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_TEXTURE;
		HRESULT hres = d3dDriver.err = DirectDraw::pdd4->GetAvailableVidMem(&ddscaps, &dw_total, &dw_free);
		if (FAILED(hres))
		{
			PrintD3D("\nNo texture memory available!\n\n");
		}
		else
		{
			PrintD3D2("\nTotal texture memory: %ld Kb\n", iKB(dw_total));
			PrintD3D2(  "Free texture memory : %ld Kb\n", iKB(dw_free));
		}
	#endif // bDEBUG_DIRECT3D
	}



//*********************************************************************************************
//
// Implementation of CDirect3D::CPriv.
//

	//*****************************************************************************************
	void CDirect3D::CPriv::SetTotalTextureMem()
	{
		DDSCAPS2 ddscaps;
		DWORD    dw_total;
		DWORD    dw_free;

		// Query to get the available amount of texture memory.
		memset(&ddscaps, 0, sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_TEXTURE;
		HRESULT hres = d3dDriver.err = DirectDraw::pdd4->GetAvailableVidMem(&ddscaps, &dw_total, &dw_free);
		if (FAILED(hres))
		{
			iTotalTextureMem = 0;
			PrintD3D("\nNo texture memory available!\n\n");
			return;
		}

		//
		// Calculated the amount of available texture memory. This assumes that front and
		// backbuffers have been allocated, but that there are currently no textures allocated.
		//
		iTotalTextureMem = Min(iMaxTotalMemory, dw_free);

	#if bDEBUG_DIRECT3D
		PrintD3D2("D3D Systems Texture memory available: %ld Kb\n", iKB(iTotalTextureMem));
		if (bFullTexturing)
		{
			PrintD3D("Full hardware texturing mode enabled.\n");
		}
		PrintD3D("\n");
	#endif // bDEBUG_DIRECT3D
	}
	
	//*****************************************************************************************
	void CDirect3D::CPriv::SetFog()
	{
		// Get the RGB values for fog.
		float f_red   = float(clrDefEndDepth.u1Red)   / 255.0f;
		float f_green = float(clrDefEndDepth.u1Green) / 255.0f;
		float f_blue  = float(clrDefEndDepth.u1Blue)  / 255.0f;

		// Set the appropriate renderstates for fogging.
		d3dDriver.err = pDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_NONE);
		d3dDriver.err = pDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR,     D3DRGB(f_red, f_green, f_blue));

		// Set the constant fog colour.
		d3dcolFogColour = D3DRGBA(f_red, f_green, f_blue, 1);
	}
	
	//*****************************************************************************************
	int CDirect3D::CPriv::iGetMaxTextureRam()
	{
		// Maximum amount of texture memory that will be considered.
		static const int iMaxD3DTextureRam = 8192 * 1024;

		if (emmMemoryModel == emmSmall)
			return (iMaxD3DTextureRam * 3) / 2;
		return iMaxD3DTextureRam;
	}
	
	//*****************************************************************************************
	int CDirect3D::CPriv::iGetWaterAlloc()
	{
		return (bD3DWater()) ? (iAllocWater) : (0);
	}
	
	//*****************************************************************************************
	bool CDirect3D::CPriv::bSetupViewport(LPDIRECTDRAWSURFACE4 pdds_backbuffer)
	{
		Assert(pdds_backbuffer);
		Assert(pD3D);
		Assert(pDevice);
		Assert(!pd3dViewport);
		
		// Get a description of the backbuffer.
		CDDSize<DDSURFACEDESC2> sd;	// Backbuffer surface information.
		DirectDraw::err = pdds_backbuffer->GetSurfaceDesc(&sd);

		iViewportWidth  = int(sd.dwWidth);
		iViewportHeight = int(sd.dwHeight);

		// Intialize viewport values.
		CDDSize<D3DVIEWPORT2> d3d_viewport;	// Viewport information..
		d3d_viewport.dwX          = 0;
		d3d_viewport.dwY          = 0;
		d3d_viewport.dwWidth      = iViewportWidth;
		d3d_viewport.dwHeight     = iViewportHeight;
		d3d_viewport.dvClipX      = D3DVAL(0);
		d3d_viewport.dvClipY      = D3DVAL(0);
		d3d_viewport.dvClipWidth  = D3DVAL(d3d_viewport.dwWidth);
		d3d_viewport.dvClipHeight = D3DVAL(d3d_viewport.dwHeight);
		d3d_viewport.dvMinZ       = D3DVAL(0);
		d3d_viewport.dvMaxZ       = D3DVAL(1);

		// Create and add the viewport.
		if (pD3D->CreateViewport(&pd3dViewport, NULL) != D3D_OK)
			return false;
		if (pDevice->AddViewport(pd3dViewport) != D3D_OK)
			return false;
		if (pd3dViewport->SetViewport2(&d3d_viewport) != D3D_OK)
			return false;
		if (pDevice->SetCurrentViewport(pd3dViewport) != D3D_OK)
			return false;

		// Success.
		return true;
	}

	//*****************************************************************************************
	bool CDirect3D::CPriv::bEnumerateTextureFormats()
	{
		// Clear the texture formats.
		memset(ddpfFormats, 0, sizeof(ddpfFormats));

		if (pDevice->EnumTextureFormats(D3DEnumTextureFormatCallback, (void*)ddpfFormats)
			!= D3D_OK)
			return false;

		// Set alpha flag.
		SetModulatedAlpha();

		if (!ddpfFormats[ed3dtexSCREEN_OPAQUE].dwSize)
			return false;

		// Alpha textures are not supported, use the existing opaque 16 bit format.
		if (!bModulatedAlpha)
		{
			ddpfFormats[ed3dtexSCREEN_ALPHA] = ddpfFormats[ed3dtexSCREEN_OPAQUE];
		}
		priv_self.SetAlphaFormat();

	#if bCOLOUR_KEY
		ddpfFormats[ed3dtexSCREEN_TRANSPARENT] = ddpfFormats[ed3dtexSCREEN_OPAQUE];
	#else
		// Use the alpha format for transparencies if no one bit alpha was found.
		if (ddpfFormats[ed3dtexSCREEN_TRANSPARENT].dwSize == 0)
			ddpfFormats[ed3dtexSCREEN_TRANSPARENT] = ddpfFormats[ed3dtexSCREEN_ALPHA];
		priv_self.SetTransparentFormat();
	#endif

		// Return success.
		return true;
	}

	//*****************************************************************************************
	void CDirect3D::CPriv::SpecialCaseSetup()
	{
		//
		// Set texture capabilities.
		//

		// Test if square textures are required.
		bSquareTextures = (d3ddescHardware.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) != 0;
		//if (b_riva_128)
			//bSquareTextures = true;

		// Test if the textures must be a power of two.
		bPower2 = (d3ddescHardware.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2) != 0;

		// Get the minimum and maximum dimensions of a texture.
		iMinWidth  = int(d3ddescHardware.dwMinTextureWidth);
		iMaxWidth  = int(d3ddescHardware.dwMaxTextureWidth);
		iMinHeight = int(d3ddescHardware.dwMinTextureHeight);
		iMaxHeight = int(d3ddescHardware.dwMaxTextureHeight);

		// Alter settings if no information has been provided.
		if (iMinWidth == 0)
			iMinWidth = 16;
		if (iMaxWidth == 0)
			iMaxWidth = 256;
		if (iMinHeight == 0)
			iMinHeight = 16;
		if (iMaxHeight == 0)
			iMaxHeight = 256;

		// Set the dimension values.
		iMinDim = Max(iMinWidth, iMinHeight);
		iMaxDim = Min(iMaxWidth, iMaxHeight);

		// Make sure the dimensions are square if only square textures are allowed.
		if (bSquareTextures)
		{
			iMinWidth  = iMinDim;
			iMaxWidth  = iMaxDim;
			iMinHeight = iMinDim;
			iMaxHeight = iMaxDim;
		}

		//
		// If alpha textures with destination blends are not supported, use the existing
		// opaque 16 bit format.
		//
		if (!bModulatedAlpha)
		{
			ddpfFormats[ed3dtexSCREEN_ALPHA] = ddpfFormats[ed3dtexSCREEN_OPAQUE];
			priv_self.SetAlphaFormat();
		}
		CEntityWater::bAlpha = bD3DWater();

		//
		// If shared system memory surfaces are not supported this also means that the entire
		// surface is being uploaded to the card regardless if region uploads are on or not.
		// To prevent slow times in uploading, make the maximum page size much smaller. Also
		// limit the maximum amount of memory that can be allocated.
		//

		//
		// Determine if alpha should be used for transparencies.
		//
		{
			// On by default.
			bTransparentAlpha = true;

			// Detect NVidia Riva 128's.
			if (evcGetVideoCard() == evcNVidia128)
				bTransparentAlpha = false;
		}
	}

	//*****************************************************************************************
	void CDirect3D::CPriv::SetAlphaFormat()
	{
		//
		// Set conversion values for Alpha.
		//
		
		// Get the raw masks.
		u4AlphaRedMask   = ddpfFormats[ed3dtexSCREEN_ALPHA].dwRBitMask;
		u4AlphaGreenMask = ddpfFormats[ed3dtexSCREEN_ALPHA].dwGBitMask;
		u4AlphaBlueMask  = ddpfFormats[ed3dtexSCREEN_ALPHA].dwBBitMask;
		u4AlphaAlphaMask = ddpfFormats[ed3dtexSCREEN_ALPHA].dwRGBAlphaBitMask;

		// Get the shift.
		u4AlphaRedShift   = uGetMaskBits(u4AlphaRedMask);
		u4AlphaGreenShift = uGetMaskBits(u4AlphaGreenMask);
		u4AlphaBlueShift  = uGetMaskBits(u4AlphaBlueMask);
		u4AlphaAlphaShift = uGetMaskBits(u4AlphaAlphaMask);

		// Shift masks.
		u4AlphaRedMask   >>= u4AlphaRedShift;
		u4AlphaGreenMask >>= u4AlphaGreenShift;
		u4AlphaBlueMask  >>= u4AlphaBlueShift;
		u4AlphaAlphaMask >>= u4AlphaAlphaShift;
	}


	//*****************************************************************************************
	void CDirect3D::CPriv::SetTransparentFormat()
	{
		// If no 5551 or 1555 format is found, use 4444.
		//if (ddpfFormats[ed3dtexSCREEN_TRANSPARENT].dwSize == 0)
			//ddpfFormats[ed3dtexSCREEN_TRANSPARENT] = ddpfFormats[ed3dtexSCREEN_ALPHA];

		//
		// Set conversion values for transparency.
		//
		
		// Get the raw masks.
		u4TransRedMask   = ddpfFormats[ed3dtexSCREEN_TRANSPARENT].dwRBitMask;
		u4TransGreenMask = ddpfFormats[ed3dtexSCREEN_TRANSPARENT].dwGBitMask;
		u4TransBlueMask  = ddpfFormats[ed3dtexSCREEN_TRANSPARENT].dwBBitMask;
		u4TransAlphaMask = ddpfFormats[ed3dtexSCREEN_TRANSPARENT].dwRGBAlphaBitMask;

		// Get the shift.
		u4TransRedShift   = uGetMaskBits(u4TransRedMask);
		u4TransGreenShift = uGetMaskBits(u4TransGreenMask);
		u4TransBlueShift  = uGetMaskBits(u4TransBlueMask);
		u4TransAlphaShift = uGetMaskBits(u4TransAlphaMask);

		// Shift masks.
		u4TransRedMask   >>= u4TransRedShift;
		u4TransGreenMask >>= u4TransGreenShift;
		u4TransBlueMask  >>= u4TransBlueShift;
		u4TransAlphaMask >>= u4TransAlphaShift;
	}

	//*****************************************************************************************
	void CDirect3D::CPriv::SetD3DFlag()
	{
		if (!DirectDraw::pdd4)
		{
			bUse = false;
			return;
		}

		bUse = bGetD3D();
		if (bUse)
		{
			GUID guid_d3d = ReadD3DGUID();
			GUID guid_null;
			memset(&guid_null, 0, sizeof(guid_null));
			if (memcmp((void*)&guid_d3d, (void*)&guid_null, sizeof(GUID)) == 0)
				bUse = false;
		}
	}

	//*****************************************************************************************
	void CDirect3D::CPriv::SetCommonFormats()
	{
		// Set format for 'ed3dtexSCREEN_OPAQUE.'
		{
			DDPIXELFORMAT ddpf = ddpfFormats[ed3dtexSCREEN_OPAQUE];

			// Count bits in the green mask.
			int i_green_bits = iCountBits(ddpf.dwGBitMask);

			switch (i_green_bits)
			{
				case 5:
					AlwaysAssert(!bRedLow(ddpf));
					ad3dcomFormats[ed3dtexSCREEN_OPAQUE] = ed3dcom555_RGB;
					break;
				case 6:
				{
					// Now select between RGB and BGR formats.
					if (bRedLow(ddpf))
						ad3dcomFormats[ed3dtexSCREEN_OPAQUE] = ed3dcom565_BGR;
					else
						ad3dcomFormats[ed3dtexSCREEN_OPAQUE] = ed3dcom565_RGB;
					break;
				}
				default:
					ad3dcomFormats[ed3dtexSCREEN_OPAQUE] = ed3dcomUnknown;
					break;
			}
		}

		// Set format for 'ed3dtexSCREEN_TRANSPARENT.'
		{
			DDPIXELFORMAT ddpf = ddpfFormats[ed3dtexSCREEN_TRANSPARENT];
			AlwaysAssert(!bRedLow(ddpf));

			// Look for a particular mask.
			if (ddpf.dwRGBAlphaBitMask == 0x8000)
				ad3dcomFormats[ed3dtexSCREEN_TRANSPARENT] = ed3dcom1555_ARGB;
			else
				ad3dcomFormats[ed3dtexSCREEN_TRANSPARENT] = ed3dcom5551_BRGA;
		}

		// Set format for 'ed3dtexSCREEN_ALPHA.'
		{
			DDPIXELFORMAT ddpf = ddpfFormats[ed3dtexSCREEN_ALPHA];
			AlwaysAssert(!bRedLow(ddpf));

			// Look for a particular mask.
			if (ddpf.dwRGBAlphaBitMask == 0xF000)
				ad3dcomFormats[ed3dtexSCREEN_ALPHA] = ed3dcom4444_ARGB;
			else
				ad3dcomFormats[ed3dtexSCREEN_ALPHA] = ed3dcom4444_BRGA;
		}
	}
	
	//*****************************************************************************************
	void CDirect3D::CPriv::SetDeviceDescriptions()
	{
		Assert(DirectDraw::pdd4);

		// Get the device description.
		{
			CDDSize<D3DDEVICEDESC> d3ddesc_hard;	// Hardware device information.
			CDDSize<D3DDEVICEDESC> d3ddesc_soft;	// Software device information.
			err = pDevice->GetCaps(&d3ddesc_hard, &d3ddesc_soft);
			
			// Copy the secription over.
			d3ddescHardware = d3ddesc_hard;
		}

		// Get the device identifier.
		HRESULT hres = DirectDraw::pdd4->GetDeviceIdentifier(&devidIdentifier, 0);
		if (FAILED(hres))
		{
			memset(&devidIdentifier, 0, sizeof(devidIdentifier));
		}

	#if bDEBUG_DIRECT3D
		{
			CConsoleBuffer con;

			con.OpenFileSession("3DDevice.txt");
			con.Print("\n\nDirectDraw 3D Device Identifier.\n\n");
			con.Print("Driver        : %s\n", devidIdentifier.szDriver);
			con.Print("Description   : %s\n", devidIdentifier.szDescription);
			con.Print("Driver Version: %8x, %8x\n", devidIdentifier.liDriverVersion.HighPart,
				      devidIdentifier.liDriverVersion.LowPart);
			con.Print("Vendor ID     : %8x\n", devidIdentifier.dwVendorId);
			con.Print("Device ID     : %8x\n", devidIdentifier.dwDeviceId);
			con.Print("SubSystem ID  : %8x\n", devidIdentifier.dwSubSysId);
			con.Print("Revision ID   : %8x\n", devidIdentifier.dwRevision);
			con.Print("GUID          : %8x", devidIdentifier.guidDeviceIdentifier.Data1);
			con.Print(" %4x", devidIdentifier.guidDeviceIdentifier.Data2);
			con.Print(" %4x", devidIdentifier.guidDeviceIdentifier.Data3);
			for (uint u = 0; u < 8; ++u)
				con.Print(" %2x", devidIdentifier.guidDeviceIdentifier.Data4[u]);
			con.Print("\n\n\n");
			con.CloseFileSession();
		}
	#endif // bDEBUG_DIRECT3D
	}
	
	//*****************************************************************************************
	void CDirect3D::ClearZBuffer()
	{
		// Do nothing if Direct3D is not in use.
		if (!bUse)
			return;

		CCycleTimer cmtr;
		D3DRECT d3dRect;
		uint32 u4_flags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;

		d3dRect.lX1 = prasMainScreen->iOffsetX; 
		d3dRect.lX2 = prasMainScreen->iOffsetX + prasMainScreen->iWidth; 
		d3dRect.lY1 = prasMainScreen->iOffsetY; 
		d3dRect.lY2 = prasMainScreen->iOffsetY + prasMainScreen->iHeight;
		
		d3dDriver.err = pd3dViewport->Clear2(1, &d3dRect, u4_flags, d3dDriver.d3dcolGetFogColour(), 0.0f, 0);
		psExecuteHardwareClear.Add(cmtr(), 1);
	}
	
	//*****************************************************************************************
	void CDirect3D::CPriv::SetModulatedAlpha()
	{
		// Assume that the feature is not supported until otherwise established.
		bModulatedAlpha = false;
		bHardwareWater  = false;

		// Test the modulate caps bit.
		if (!(d3ddescHardware.dwTextureOpCaps & D3DTEXOPCAPS_MODULATE))
			return;

		// Test to ensure a 4444 texture format is supported.
		DDPIXELFORMAT pixform = ddpfFormats[ed3dtexSCREEN_ALPHA];
		if (iCountBits(pixform.dwRGBAlphaBitMask) != 4 || !(pixform.dwFlags & DDPF_ALPHAPIXELS))
			return;

		// Test if source alpha blending is supported.
		//if (!(d3ddescHardware.dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_SRCALPHA))
			//return;

		PrintD3D("Modulated alpha textures are supported.\n");

		// Get the registry setting for hardware accelerated water.
		bHardwareWater = GetRegValue(strHARDWARE_WATER, bModulatedAlpha);

		// This feature is supported.
		bModulatedAlpha = true;
	}

	//*****************************************************************************************
	void CDirect3D::CPriv::SetScreenFormat(LPDIRECTDRAWSURFACE4 pdds_backbuffer)
	{
		HRESULT hres;

		// Set to unknown format, assumed to be 565 because it is the most common format.
		esfScreenFormat = esfOther;
		
		// Get a description of the pixel format.
		CDDSize<DDSURFACEDESC2> sd;	// Backbuffer surface information.
		hres = pdds_backbuffer->GetSurfaceDesc(&sd);
		if (FAILED(hres))
		{
			Assert(0);
			return;
		}

		// Test for 555 format.
		if (sd.ddpfPixelFormat.dwGBitMask == 0x03E0)
		{
			esfScreenFormat = esf555;
			return;
		}

		// Test for 565 format.
		if (sd.ddpfPixelFormat.dwGBitMask == 0x07E0)
		{
			esfScreenFormat = esf565;
			return;
		}

		Assert(0);
		return;
	}
	
	//*****************************************************************************************
	void CDirect3D::CPriv::SetMemoryModel()
	{
		if (u4TotalPhysicalMemory() > u64MbThreshold)
		{
			emmMemoryModel = emmLarge;
			return;
		}
		emmMemoryModel = emmSmall;
	}

	//*****************************************************************************************
	bool CDirect3D::CPriv::bUseManagedMemory()
	{
		//
		// The card must have a sufficient amount of available local texture memory.
		//
		// Notes:
		//		Managed memory seems to work on a variety of cards regardless of available texture
		//		memory, therefore the code that tested for available texture memory has been removed.
		//

		// The card cannot be a NVidia Riva 128.
		// To do: test the device ID for this.

		//
		// Get the total amount of managed memory.
		//
		// Notes:
		//
		//		What it should do is use 20 Mb for every 32 Mb over 32 Mb:
		//
		//			iTotalFreeMem = ((u4_system - iBytesMB(32)) * 20) / 32;
		//
		//		Sadly, for a demo hack (given that memory usage has not yet been reigned in),
		//		the system will be given 20 Mb and no more.
		//
		switch (emmMemoryModel)
		{
			case emmSmall:
				iTotalFreeMem = iBytesMB(5);
				break;

			case emmLarge:
				iTotalFreeMem = iBytesMB(priv_self.iLargeMemAllocMB());
				break;
		}
		iTotalFreeMem -= uLargePool();

		iTotalTextureMem = iTotalFreeMem;
		AlwaysAssert(iTotalFreeMem >= 0);

		// Use managed memory.
		PrintD3D("\n\nTexturing will be done using Direct3D managed memory.\n");
		PrintD3D2("Managed texture memory        : %ld Kb\n", iKB(iTotalFreeMem));
		return true;
	}

	//*****************************************************************************************
	bool CDirect3D::CPriv::bAllocateManagedMemory()
	{
		//
		// Calculate system allocations.
		//
		int i_running_total = iTotalTextureMem;

		// Allocate memory for the pool manager.
		poolD3DRaster.SetTextureMemSize(uLargePool());
		i_running_total -= uLargePool();
		PrintD3D2("Pooled free textures          : %ld Kb\n", iKB(uLargePool()));

		// Allocate sky. Sky uses the fixed amount of 128 Kb.
		i_running_total -= iAllocSky;
		PrintD3D2("Sky                           : %ld Kb\n", iKB(iAllocSky));

		// Allocate caches. Use 1/3 of available memory.
		int i_cache = (iTotalTextureMem * iCachePercent) / 100;
		CRenderCache::SetTextureMemSize(0, i_cache / 1024);
		i_running_total -= i_cache;
		PrintD3D2("Render Caches                 : %ld Kb\n", iKB(i_cache));

		// Allocate water.
		int i_water = iTotalFreeMem / 40;
		SetMinMax(i_water, iBytesKB(384), iBytesKB(1024));
		CEntityWater::SetTextureMemSize(i_water / 1024);
		i_water = CEntityWater::iGetManagedMemSize();
		i_running_total -= i_water;
		PrintD3D2("Water                         : %ld Kb\n", iKB(i_water));

		// Allocate terrain.
		iTerrainTextureMem = iTotalTextureMem / 9;
		SetMinMax(iTerrainTextureMem, iMinTerrain, iMaxTerrain);
		NMultiResolution::CTextureNode::SetTextureMemSize(iTerrainTextureMem / 1024);
		iTerrainTextureMem = NMultiResolution::CTextureNode::ptexmTexturePages->iGetManagedMemSize();
		i_running_total -= iTerrainTextureMem;
		PrintD3D2("Terrain                       : %ld Kb\n", iKB(iTerrainTextureMem));

		// Attempt to use the agp memory allocator.
		agpAGPTextureMemManager.AllocateMemory(Max(iMinTexture, i_running_total));
		if (!agpAGPTextureMemManager.bIsActive())
		{
			agpAGPTextureMemManager.AllocateMemory(0);
			Assert(0);
			Purge();
			return false;
		}
		PrintD3D2("Memory for regular texturing  : %ld Kb\n", iKB(i_running_total));

		// Set appropriate flags and return.
		bFullTexturing = true;
		easAllocation  = easManaged;
		PrintD3D("\n");
		return true;
	}
	
	//*****************************************************************************************
	bool CDirect3D::CPriv::bUseNonLocalMemory()
	{
		const uint uMinLocal  = iBytesKB(12000);
											// Minimum amount of local texture memory required
											// for using memory management.
		uint32 u4_available   = 0;			// Available texture memory.

		// Get the amount of texture memory available.
		{
			DDSCAPS2 ddscaps;
			DWORD    dw_total;
			DWORD    dw_free;

			// Query to get the available amount of local texture memory.
			memset(&ddscaps, 0, sizeof(ddscaps));
			ddscaps.dwCaps = DDSCAPS_NONLOCALVIDMEM | DDSCAPS_TEXTURE;
			HRESULT hres = d3dDriver.err = DirectDraw::pdd4->GetAvailableVidMem(&ddscaps, &dw_total, &dw_free);
			if (!FAILED(hres))
			{
				u4_available = dw_free;
			}
		}
		// The card must have a sufficient amount of available local texture memory.
		if (u4_available < uMinLocal)
			return false;

		// The card cannot be a NVidia Riva 128.
		// To do: test the device ID for this.

		//
		// Get the total amount of managed memory.
		//
		// Notes:
		//
		//		What it should do is use 20 Mb for every 32 Mb over 32 Mb:
		//
		//			iTotalFreeMem = ((u4_system - iBytesMB(32)) * 20) / 32;
		//
		//		Sadly, for a demo hack (given that memory usage has not yet been reigned in),
		//		the system will be given 20 Mb and no more.
		//
		switch (emmMemoryModel)
		{
			case emmSmall:
				iTotalFreeMem  = Min(iBytesMB(5), u4_available);
				break;

			case emmLarge:
				iTotalFreeMem  = Min(iBytesMB(priv_self.iLargeMemAllocMB()), u4_available);
				break;
		}
		iTotalFreeMem -= uLargePool();

		iTotalTextureMem = iTotalFreeMem;
		AlwaysAssert(iTotalFreeMem >= 0);

		// Use managed memory.
		PrintD3D("\n\nTexturing will be done using Direct3D nonlocal video memory.\n");
		PrintD3D2("Available texture memory      : %ld Kb\n", iKB(iTotalFreeMem));
		return true;
	}
	
	//*****************************************************************************************
	bool CDirect3D::CPriv::bAllocateNonLocalMemory()
	{
		//
		// Calculate system allocations.
		//
		int i_running_total = iTotalTextureMem;

		// Allocate memory for the pool manager.
		poolD3DRaster.SetTextureMemSize(uLargePool());
		i_running_total -= uLargePool();
		PrintD3D2("Pooled free textures          : %ld Kb\n", iKB(uLargePool()));

		// Allocate sky. Sky uses the fixed amount of 128 Kb.
		i_running_total -= iAllocSky;
		PrintD3D2("Sky                           : %ld Kb\n", iKB(iAllocSky));

		// Allocate caches. Use 1/3 of available memory.
		int i_cache = (iTotalTextureMem * iCachePercent) / 100;
		CRenderCache::SetTextureMemSize(0, i_cache / 1024);
		i_running_total -= i_cache;
		PrintD3D2("Render Caches                 : %ld Kb\n", iKB(i_cache));

		// Allocate water.
		int i_water = iTotalFreeMem / 40;
		SetMinMax(i_water, iBytesKB(384), iBytesKB(1024));
		CEntityWater::SetTextureMemSize(i_water / 1024);
		i_water = CEntityWater::iGetManagedMemSize();
		i_running_total -= i_water;
		PrintD3D2("Water                         : %ld Kb\n", iKB(i_water));

		// Allocate terrain.
		iTerrainTextureMem = iTotalTextureMem / 9;
		SetMinMax(iTerrainTextureMem, iMinTerrain, iMaxTerrain);
		NMultiResolution::CTextureNode::SetTextureMemSize(iTerrainTextureMem / 1024);
		iTerrainTextureMem = NMultiResolution::CTextureNode::ptexmTexturePages->iGetManagedMemSize();
		i_running_total -= iTerrainTextureMem;
		PrintD3D2("Terrain                       : %ld Kb\n", iKB(iTerrainTextureMem));

		// Attempt to use the agp memory allocator.
		agpAGPTextureMemManager.AllocateMemory(Max(iMinTexture, i_running_total));
		if (!agpAGPTextureMemManager.bIsActive())
		{
			agpAGPTextureMemManager.AllocateMemory(0);
			Assert(0);
			Purge();
			return false;
		}
		PrintD3D2("Memory for regular texturing  : %ld Kb\n", iKB(i_running_total));

		// Set appropriate flags and return.
		easAllocation = easNonLocal;
		bFullTexturing = true;
		return true;
	}
	
	//*****************************************************************************************
	void CDirect3D::CPriv::SetLocalVideoMemFlag()
	{
		DDSCAPS2 ddscaps;
		DWORD    dw_total;
		DWORD    dw_free;

		// Query to get the available amount of local texture memory.
		memset(&ddscaps, 0, sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_TEXTURE;
		HRESULT hres = d3dDriver.err = DirectDraw::pdd4->GetAvailableVidMem(&ddscaps, &dw_total, &dw_free);
		if (FAILED(hres))
		{
			// Assume the worst.
			bLocalVideoMem = true;
			return;
		}
		bLocalVideoMem = dw_total != 0 || dw_free != 0;
	}
	
	//*****************************************************************************************
	//
	void CDirect3D::AnimateLoadingScreenPriv
	(
		int i_width,
		int i_height
	)
	//
	// Animates the loading screen every eight calls.
	//
	//**************************************
	{
        if (g_pfnWorldLoadNotify)
        {
			// Use the main loading bar call every N textures.
			// N = approximate number of surfaces / 5
			if ((++iAnimateCount & 511) == 0)
			{
				// Increment loading percentage.
				(g_pfnWorldLoadNotify)(g_u4NotifyParam, 5, 0, 0);
			}
        }
        else if (!bIsTrespasser)
        {
		    int i_size = i_width * i_height;
		    i_size >>= 12;
		    if (i_size < 1)
			    i_size = 1;
		    iAnimateCount += i_size;
		    if (iAnimateCount < 16)
			    return;
		    iAnimateCount = 0;
		    if (!prasMainScreen)
			    return;
		    prasMainScreen->Flip();
        }
	}

	//*****************************************************************************************
	void CDirect3D::CPriv::WriteTextToLoadScreen(const char* str)
	{
		prasMainScreen->Unlock();
		prasMainScreen->Clear(0);
		if (!str || !*str)
		{
			prasMainScreen->Flip();
			return;
		}

		COLORREF col = 0x00FFFFFF;

		HDC hdc = prasMainScreen->hdcGet();

		int i_bk_mode     = SetBkMode(hdc, TRANSPARENT);
		col               = SetTextColor(hdc, col);
		uint u_text_align = SetTextAlign(hdc, TA_TOP | TA_LEFT);

		TextOut(hdc, 16, 16, str, strlen(str));

		SetTextAlign(hdc, u_text_align);
		SetTextColor(hdc, col);
		SetBkMode(hdc, i_bk_mode);

		prasMainScreen->ReleaseDC(hdc);
		prasMainScreen->Flip();
	}

	//*****************************************************************************************
	void CDirect3D::CPriv::SetupLoadingScreens()
	{
		if (!prasMainScreen)
			return;

        if (g_pfnWorldLoadNotify)
        {
            (g_pfnWorldLoadNotify)(g_u4NotifyParam, 4, 0, 0);
        }
        else if (!bIsTrespasser)
        {
		    WriteTextToLoadScreen("Loading surfaces -");
		    WriteTextToLoadScreen("Loading surfaces \\");
		    WriteTextToLoadScreen("Loading surfaces /");
        }
	}

	//*****************************************************************************************
	int CDirect3D::CPriv::iLargeMemAllocMB()
	{
		// If a very large amount of memory is available, use everything.
		if (u4TotalPhysicalMemory() > u96MbThreshold)
			return 24;

		// Shared buffers reduce the memory load.
		if (bSharedSysBuffers)
			return 18;

		// Non-shared buffers eat more memory.
		return 16;
	}


//
// Global variables.
//
CDirect3D d3dDriver;