/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Implementation of RasterD3D.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/RasterD3D.cpp                                                $
 * 
 * 61    10/04/98 8:22a Pkeet
 * AGP or managed textures are flushed if the constructor fails to allocate a video raster.
 * 
 * 60    10/03/98 11:15p Pkeet
 * Safer allocation for video memory raster; also it gives more information when it fails.
 * 
 * 59    9/09/98 7:41p Pkeet
 * Added functions to show that the computer is working while it loads Direct3D surfaces.
 * 
 * 58    8/29/98 9:01p Pkeet
 * Added code to look for raster leaks.
 * 
 * 57    8/28/98 8:59p Pkeet
 * Incorrect dimensions are forcibly set to valid dimensions instead of asserting.
 * 
 * 56    8/28/98 8:26p Pkeet
 * Added an enumeration for hint type, and added code for the hint type.
 * 
 * 55    8/28/98 1:10p Asouth
 * changes required by different STL implementation
 * 
 * 54    8/21/98 7:41p Pkeet
 * Commented out unnecessary assert.
 * 
 * 53    8/21/98 4:32p Pkeet
 * Commented out the 'is busy' assert for uploads. Changed the system so that system memory
 * buffers are not ref counted if they are shared.
 * 
 * 52    8/21/98 12:05a Pkeet
 * Added debug code for region uploads.
 * 
 * 51    8/20/98 11:11p Pkeet
 * Added the 'ReleaseSharedSurfaces' static member function.
 * 
 * 50    8/18/98 10:36p Pkeet
 * Added preloaded rasters to the list of rasters to track.
 * 
 * 49    8/18/98 3:37p Pkeet
 * Remove the forced dimension parameter.
 * 
 * 48    8/18/98 1:48p Pkeet
 * Removed the alloc on load flag for video textures.
 * 
 * 47    8/11/98 9:46p Pkeet
 * Made all non-local memory textures managed.
 * 
 * 46    8/11/98 9:22p Pkeet
 * Removed inappropriate flags.
 * 
 * 45    8/08/98 11:00p Pkeet
 * Improved the managed texture scheme.
 * 
 * 44    8/08/98 6:58p Pkeet
 * Added a selection of allocation schemes.
 * 
 * 43    8/07/98 11:52a Pkeet
 * Added the 'bIsLocked' and 'bIsLockedAll' member functions.
 * 
 * 42    7/28/98 3:07p Pkeet
 * Tightened upload function somewhat.
 * 
 * 41    7/27/98 6:30p Pkeet
 * Enabled shared system memory buffers for discreet textures.
 * 
 * 40    7/27/98 3:45p Pkeet
 * Implemented shared system memory surfaces for discreetly allocated textures.
 * 
 * 39    7/27/98 12:52p Pkeet
 * Added a separate initialization class for page managed textures.
 * 
 * 38    7/27/98 12:28p Pkeet
 * Added an initialization class for 'CRasterD3D.'
 * 
 * 37    7/23/98 9:08p Pkeet
 * Added code to track the creation and deletion of Direct3D textures using a macro switch in
 * 'RasterD3D.hpp.'
 * 
 * 36    7/23/98 6:24p Pkeet
 * Added support for texturing in hardware out of system memory.
 * 
 * 35    7/20/98 10:32p Pkeet
 * Upgraded to Direct3D 4's interface.
 * 
 * 34    7/10/98 4:28p Pkeet
 * Added support for colour keying.
 * 
 * 33    7/09/98 8:47p Pkeet
 * Added code to support direct access to texture memory.
 * 
 * 32    6/13/98 5:05p Pkeet
 * Added test code to try small region uploads.
 * 
 * 31    6/12/98 9:12p Pkeet
 * Added parameter to constructor to force the Direct3D texture surface to be a given size.
 * 
 * 30    6/10/98 7:33p Pkeet
 * Added support for shared DirectDraw system memory surfaces.
 * 
 * 29    6/10/98 3:16p Mlange
 * Fix for NULL pointer in ReleasePObject.
 * 
 * 28    6/09/98 8:14p Pkeet
 * Separated the create system memory surface private member function.
 * 
 * 27    6/05/98 6:16p Pkeet
 * Fixed bug so that the right and bottom of the upload rectangle are exclusive.
 * 
 * 26    5/20/98 2:50p Pkeet
 * Added a call to upload the texture right away upon construction of the raster. Made the
 * texture upload function tolerant of having a null pointer for the video surface.
 * 
 * 25    5/14/98 11:49a Pkeet
 * Moved the interface objects for system and video surfaces back into the general 'bUpload'
 * call.
 * 
 * 24    5/13/98 10:54a Mlange
 * Region upload now first checks if D3D in use.
 * 
 * 23    5/12/98 6:44p Mlange
 * Renamed bUploadRegion() to bUpload() and changed its parameters.
 * 
 * 22    5/12/98 12:28p Pkeet
 * Added the 'bUploadRegion' member function.
 * 
 * 21    4/08/98 5:32p Pkeet
 * Fixed bug that caused the upload to crash the application if it failed.
 * 
 * 20    4/08/98 3:00p Pkeet
 * Made 'bSetHandle' a member of 'priv_self.' Added caching of com objects that provide texture
 * interfaces.
 * 
 * 19    4/02/98 4:57p Pkeet
 * Added the 'bVerifyConstruction' and 'u4MakeTypeIDD3D' member functions.
 * 
 * 18    4/02/98 2:03p Pkeet
 * Added the 'u4GetTypeID' member function. Removed the 'bMaintainBoth' flag.
 * 
 * 17    4/01/98 5:48p Pkeet
 * Added a private class for use by 'priv_self.'
 * 
 * 16    2/24/98 6:53p Rwyatt
 * Added an assert to the lock member to ensure the stride is still the same
 * 
 * 15    1/21/98 9:07p Pkeet
 * Added an assert to ensure that the size parameters passed to the raster constructor based on
 * capabilities determined by Direct3D.
 * 
 * 14    1/12/98 6:15p Pkeet
 * Added a lock and unlock toggle.
 * 
 * 13    1/10/98 3:01p Pkeet
 * Added the 'RemoveFromSystem' member function.
 * 
 * 12    98/01/06 15:40 Speter
 * Put in appropriate texture stats.  
 * 
 * 11    12/18/97 7:59p Pkeet
 * Fixed bug for 3Dfx.
 * 
 * 10    12/18/97 6:05p Pkeet
 * Fixed Riva 128 bug.
 * 
 * 9     12/17/97 5:39p Pkeet
 * Added error handling for Direct3D.
 * 
 * 8     12/16/97 3:43p Pkeet
 * Added structures for maintaining both a system and video memory raster.
 * 
 * 7     12/16/97 3:02p Pkeet
 * The constructor now sets the 'RasterSurface' data member.
 * 
 * 6     12/15/97 3:49p Pkeet
 * Turned off tiling.
 * 
 * 5     12/15/97 1:50p Pkeet
 * Improved error reporting for uploading textures to the video card.
 * 
 * 4     12/12/97 6:09p Pkeet
 * Added code to set the pixel format. Removed test code in anticipation of using the
 * 'CRasterD3D' class for terrain.
 * 
 * 3     12/12/97 12:06p Pkeet
 * Added generic functions for D3D textures.
 * 
 * 2     12/12/97 11:19a Pkeet
 * Added the 'RemoveFromVideo' and 'SetHandle' member functions.
 * 
 * 1     12/11/97 8:00p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "RasterD3D.hpp"

#include <map>
#include "Lib/W95/WinInclude.hpp"
#include "Lib/W95/DD.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/View/Direct3DRenderState.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"
#include "Lib/View/AGPTextureMemManager.hpp"

#include <memory.h>


//
// Profile Stats.
//

// Upload stats.
static CProfileStat psUploadCreateSys   ("Create Sys",  &proHardware.psUploads);
static CProfileStat psUploadCreateVid   ("Create Vid",  &proHardware.psUploads);
static CProfileStat psUploadLoad        ("Load",        &proHardware.psUploads);
static CProfileStat psUploadOther       ("Other",       &proHardware.psUploads);

// Other stats.
static CProfileStat psRelease("Release", &proHardware.psHardware);
static CProfileStat psLockTexture("Lock Tex.", &proHardware.psHardware);



#if bTRACK_D3D_RASTERS

	#include "list.h"
	#include "Lib/Sys/DebugConsole.hpp"

	char* strWN = "Water";
	char* strSN = "Sky";
	char* strTN = "Terrain";
	char* strCN = "Caches";
	char* strAN = "AGP Texturing";
	char* strPN = "Pool";
	char* strRN = "Preloaded textures";
	char* strUN = "Unknown";
	char* strTrack = strUN;
	ETrackD3DRas etrTrack = etrUnknown;

	void TrackSystem(ETrackD3DRas etr)
	{
		switch (etr)
		{
			case etrWater:
				strTrack = strWN;
				break;
			case etrSky:
				strTrack = strSN;
				break;
			case etrTerrain:
				strTrack = strTN;
				break;
			case etrCaches:
				strTrack = strCN;
				break;
			case etrAGP:
				strTrack = strAN;
				break;
			case etrPre:
				strTrack = strRN;
				break;
			case etrPool:
				strTrack = strPN;
				break;
			default:
				strTrack = strUN;
		};
		etrTrack = etr;
	}

	void SetupTrack()
	{
		TrackSystem(etrTrack);
	}

	class CRList
	{
	public:
		list<CRasterD3D*> lr;

		~CRList()
		{
			PrintLeftovers();
		}

		void Add(CRasterD3D* p)
		{
			Assert(p);
			p->strSource = strTrack;
			lr.push_back(p);
			strTrack = strUN;
		}

		void Remove(CRasterD3D* p)
		{
			lr.remove(p);
		}

		void PrintLeftovers()
		{
			int i = 0;
			dprintf("\n\nSearching for leaking D3D rasters:\n");
			list<CRasterD3D*>::iterator it = lr.begin();
			for (; it != lr.end(); ++it)
			{
				if ((*it)->strSource)
					dprintf("Leftover raster %s\n", (*it)->strSource);
				else
					dprintf("Leftover raster of unknown type\n");
				++i;
			}
			dprintf("%ld leaked D3D rasters found!\n\n\n", i);
		}
	};
	CRList rlist;

	void PrintLeftoverD3D()
	{
		rlist.PrintLeftovers();
	}

#endif // bTRACK_D3D_RASTERS


//
// Local functions.
//

//**********************************************************************************************
//
template<class T> void InitDXStruct
(
	T& t
)
//
// Initializes and DirectX structure.
//
//**************************************
{
	Clear(t);
	t.dwSize = sizeof(t);
}

//**********************************************************************************************
//
template<class T> T& Clear
(
	T& t
)
//
// Clears any structure.
//
//**************************************
{
	memset(&t, 0, sizeof(t));
	return t;
}

//**********************************************************************************************
//
template<class T> void ReleasePObject
(
	T& t
)
//
// Releases a DirectX pointer object.
//
//**************************************
{
	if (t)
		t->Release();
	t = 0;
}


//
// Class definitions.
//

//**********************************************************************************************
//
class CMapDDSurfaces : public std::map< uint32, LPDIRECTDRAWSURFACE4, std::less<uint32> >
//
// Encapsulates an STL map for DirectDraw surfaces.
//
// Prefix: mapdd
//
//**************************************
{

public:

	//******************************************************************************************
	//
	// Constructors and destructors.
	//

	// Default constructor.
	CMapDDSurfaces()
	{
	}

	// Destructor.
	~CMapDDSurfaces()
	{
		ReleaseAll();
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void ReleaseAll()
	{
		iterator it = begin();
		for (; it != end(); ++it)
		{
			int i_refs = (*it).second->Release();
			Assert(i_refs == 0);
		}
		erase(begin(), end());
	}
	//
	// Releases all shared surfaces and removes them from the STL map.
	//
	//**************************************

	//******************************************************************************************
	//
	LPDIRECTDRAWSURFACE4 pddsFind
	(
		uint32 u4_hash
	)
	//
	// Returns a pointer to a shared surface if it has one with the given hash value, otherwise
	// returns zero.
	//
	//**************************************
	{
		iterator it = find(u4_hash);
		if (it == end())
			return 0;
		return (*it).second;
	}

	//******************************************************************************************
	//
	void Add
	(
		uint32 u4_hash,
		LPDIRECTDRAWSURFACE4 pdds
	)
	//
	// Adds a shared surface of the given description.
	//
	//**************************************
	{
		Assert(u4_hash);
		Assert(!pddsFind(u4_hash));

	#ifdef __MWERKS__
		insert(pair<const uint32, LPDIRECTDRAWSURFACE4>(u4_hash, pdds));
	#else
		insert(std::pair<uint32, LPDIRECTDRAWSURFACE4>(u4_hash, pdds));
	#endif

		Assert(pddsFind(u4_hash));
	}

};
CMapDDSurfaces mapddSurfaces;


//**********************************************************************************************
//
class CRasterD3D::CPriv : public CRasterD3D
//
// Private member functions for class 'CScreenRenderAuxD3D.'
//
//**************************************
{
public:
	CPriv();

	//******************************************************************************************
	//
	void SetupPixelFormat
	(
		ED3DTextureType ed3dtex	// Texture format required.
	);
	//
	// Creates a system memory surface from specified attributes.
	//
	//**************************************

	//******************************************************************************************
	//
	LPDIRECTDRAWSURFACE4 pddsCreateSysMemSurface
	(
		ED3DTextureType ed3dtex	// Texture format required.
	);
	//
	// Creates a system memory surface from specified attributes.
	//
	//**************************************

	//******************************************************************************************
	//
	uint32 u4HashSurfaceType
	(
		ED3DTextureType ed3dtex	// Texture format required.
	)
	//
	// Returns a unique hash value for the specified surface type and size.
	//
	//**************************************
	{
		uint32 u4 = uint32(ed3dtex);
		u4 <<= 10;
		u4 |= iWidth;
		u4 <<= 10;
		u4 |= iHeight;
		return u4;
	}

	//******************************************************************************************
	//
	bool bCreateSysMemSurface
	(
		ED3DTextureType ed3dtex	// Texture format required.
	);
	//
	// Creates a system memory surface from specified attributes.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bCreateVidMemSurface
	(
	);
	//
	// Creates a video memory surface from the system memory surface.
	//
	//**************************************

	//******************************************************************************************
	//
	void SetColourKey
	(
	);
	//
	// Sets the colour key for the texture for rendering transparencies.
	//
	//**************************************

};


//
// Class implementation.
//

//**********************************************************************************************
//
// CRasterD3D::CInit and descendant implementations.
//

	//******************************************************************************************
	//
	// Constructors.
	//

	//******************************************************************************************
	CRasterD3D::CInit::CInit(int i_width, int i_height, ED3DTextureType ed3dtex, bool b_upload_imm)
	{
		iWidth        = i_width;
		iHeight       = i_height;
		ed3dtexType   = ed3dtex;
		bUploadImm    = b_upload_imm;
		bSharedSysMem = d3dDriver.bUseSharedSysBuffers();
		ehHint        = ehDynamic;

		switch (d3dDriver.easAllocations())
		{
			case easNonLocal:
				easAllocation = easNonLocal;
				break;

			case easManaged:
				easAllocation = easManaged;
				break;

			default:
				easAllocation = easFixed;
		}
	}

	//******************************************************************************************
	CRasterD3D::CInitPage::CInitPage(int i_width, int i_height, ED3DTextureType ed3dtex)
		: CRasterD3D::CInit(i_width, i_height, ed3dtex)
	{
		bUploadImm = true;
	}

	//******************************************************************************************
	CRasterD3D::CInitDiscreet::CInitDiscreet(int i_width, int i_height, ED3DTextureType ed3dtex)
		: CRasterD3D::CInit(i_width, i_height, ed3dtex)
	{
		bUploadImm    = false;
		bSharedSysMem = true;

		if (!bPowerOfTwo(iWidth))
			iWidth = NextPowerOfTwo(iWidth);
		if (!bPowerOfTwo(iHeight))
			iHeight = NextPowerOfTwo(iHeight);
	}

	//******************************************************************************************
	CRasterD3D::CInitDiscreetStatic::CInitDiscreetStatic(int i_width, int i_height, ED3DTextureType ed3dtex)
		: CInitDiscreet(i_width, i_height, ed3dtex)
	{
		ehHint = ehStatic;
	}

	//******************************************************************************************
	CRasterD3D::CInitDiscreetOpaque::CInitDiscreetOpaque(int i_width, int i_height, ED3DTextureType ed3dtex)
		: CInitDiscreet(i_width, i_height, ed3dtex)
	{
		ehHint = ehOpaque;
	}


//**********************************************************************************************
//
// CRasterD3D implementation.
//

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	//******************************************************************************************
	CRasterD3D::CRasterD3D(const CInit& init)
		: bLocked(false), pd3dtexTex(0), u4LastUsedFrame(CRasterD3D::u4FrameCount)
	{
		// Give the user a sign that something is happening if required.
		d3dDriver.AnimateLoadingScreen(init.iWidth, init.iHeight);

		// Set data members from the init class.
		iWidth        = init.iWidth;
		iHeight       = init.iHeight;
		ed3dtexType   = init.ed3dtexType;
		bSharedSysMem = init.bSharedSysMem;
		easAllocation = init.easAllocation;
		ehHint        = init.ehHint;

		// Set the correct dimensions.
		d3dDriver.SetTextureMinMax(iWidth, iHeight, false);

		Assert(DirectDraw::pdd4);

		iPixelBits       = 16;
		iLinePixels      = iWidth;
		pSurface         = 0;
		fWidth           = float(iWidth) - 2.0f * fTexEdgeTolerance;
		fHeight          = float(iHeight) - 2.0f * fTexEdgeTolerance;
		bNotTileable     = true;
		u4HeightTileMask = 0xFFFFFFFF;
		u4WidthTileMask  = 0xFFFFFFFF;
		pddsSurfaceSys   = 0;
		pddsSurfaceVid   = 0;

		// Construct our RasterSurface member.
		new(&RasterSurface) CRasterSurface(*this, iPixelBits);

		// Create a new DirectDraw surface for the system memory version of the texture.
		priv_self.bCreateSysMemSurface(ed3dtexType);

		// Create a new DirectDraw surface for the video version of the texture.
		priv_self.bCreateVidMemSurface();

		// Set the colour key for transparent textures.
	#if bCOLOUR_KEY
		if (ed3dtex == ed3dtexSCREEN_TRANSPARENT)
			priv_self.SetColourKey();
	#endif

		// Add to stats.
		proProfile.psTextures.Add(0, 1);
		proProfile.psTextureKB.Add(0, iTotalPixels() * iPixelBytes() / 1024);

		if (init.bUploadImm)
			CRasterD3D::bUpload();

	#if bTRACK_D3D_RASTERS
		rlist.Add(this);
	#endif // bTRACK_D3D_RASTERS
	}
	
	//******************************************************************************************
	CRasterD3D::~CRasterD3D()
	{
		if (prasLink)
			prasLink->prasLink = rptr0;
		prasLink = rptr0;

		CCycleTimer ctmr;

		ReleasePObject(pd3dtexTex);
		if (!bSharedSysMem)
		{
			// Release the system memory buffer if it is not shared.
			ReleasePObject(pddsSurfaceSys);
		}
		ReleasePObject(pddsSurfaceVid);

		proProfile.psTextures.Add(0, -1);
		proProfile.psTextureKB.Add(0, - iTotalPixels() * iPixelBytes() / 1024);

	#if bTRACK_D3D_RASTERS
		rlist.Remove(this);
	#endif // bTRACK_D3D_RASTERS
	}

	//******************************************************************************************
	//
	// Overrides.
	//
	
	//******************************************************************************************
	void CRasterD3D::Lock()
	{
		LPDIRECTDRAWSURFACE4 pdds = pddsSurfaceSys;

		Assert(pdds);
		//AlwaysAssert(!srd3dRenderer.bIsBusy());
		
		// If the surface is already locked, do nothing.
		if (bLocked)
			return;

		// Increment the lock count.
		++iLockCount;

		CCycleTimer ctmr;

		CDDSize<DDSURFACEDESC2> ddsd;
		DirectDraw::err = pdds->Lock(0, &ddsd, DDLOCK_WAIT, 0);
		Assert(ddsd.lpSurface);
		pSurface = ddsd.lpSurface;

		Assert(ddsd.lPitch == iLineBytes());

		// Indicated that the surface is locked.
		bLocked = true;

		psLockTexture.Add(ctmr(), 1);
	}

	//******************************************************************************************
	void CRasterD3D::Unlock()
	{
		LPDIRECTDRAWSURFACE4 pdds = pddsSurfaceSys;
		Assert(pdds);

		// If the surface is already unlocked, do nothing.
		if (!bLocked)
			return;

		// Decrement the lock count.
		--iLockCount;

		pdds->Unlock(0);

		// Indicated that the surface is locked.
		bLocked = false;
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	bool CRasterD3D::bUpload()
	{
		bool b_succeeded = true;

		// Do nothing if the texture is already uploaded or D3D is not being used.
		if (!d3dDriver.bUseD3D())
			return true;
		Assert(pddsSurfaceSys);
		//AlwaysAssert(!srd3dRenderer.bIsBusy());

		if (!pddsSurfaceVid)
			return false;

		CCycleTimer ctmr_total;
		LPDIRECT3DTEXTURE2 d3dtex_sys = 0;	// System memory texture.

		d3dDriver.err = pddsSurfaceSys->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&d3dtex_sys);

		// If the interface query failed, return immediately with a return value indicating an error.
		if (!d3dtex_sys)
		{				
			ReleasePObject(pd3dtexTex);
			proHardware.psUploads.Add(ctmr_total(), iTotalPixels());
			return false;
		}

		// Get the texture interface if there is not already one.
		if (!pd3dtexTex)
		{
			d3dDriver.err = pddsSurfaceVid->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pd3dtexTex);
		}

		// If the interface query failed, return immediately with a return value indicating an error.
		if (!pd3dtexTex)
		{				
			d3dtex_sys->Release();
			proHardware.psUploads.Add(ctmr_total(), iTotalPixels());
			return false;
		}

		AlwaysAssert(d3dtex_sys);
		AlwaysAssert(pd3dtexTex);


		//
		// Upload the texture.
		//
		{
			CCycleTimer ctmr;
			HRESULT hres = pd3dtexTex->Load(d3dtex_sys);
			psUploadLoad.Add(ctmr(), iTotalPixels());
			if (FAILED(hres))
				b_succeeded = false;
		}
		d3dtex_sys->Release();

		// If the upload failed, release current video local or non-local interface and memory.
		if (!b_succeeded)
		{				
			ReleasePObject(pd3dtexTex);
			proHardware.psUploads.Add(ctmr_total(), iTotalPixels());
			return false;
		}

		proHardware.psUploads.Add(ctmr_total(), iTotalPixels());

		// Return flag indicating success or error.
		b_succeeded = b_succeeded && pd3dtexGet() != 0;
		return b_succeeded;
	}

	//******************************************************************************************
	bool CRasterD3D::bUpload(int i_x, int i_y, int i_width, int i_height)
	{
		if (!d3dDriver.bUseD3D())
			return true;

		if (!pddsSurfaceVid)
			return false;

		CCycleTimer ctmr;

		Assert(pddsSurfaceSys);
		Assert(pddsSurfaceVid);
		AlwaysAssert(!srd3dRenderer.bIsBusy());
		
		RECT rc;
		SetRect(&rc, i_x, i_y, i_x + i_width, i_y + i_height);

		// Blit from system memory to video memory.
		HRESULT hres = pddsSurfaceVid->Blt(&rc, pddsSurfaceSys, &rc, DDBLT_WAIT, NULL);

	#if VER_DEBUG
		if (FAILED(hres))
		{
			PrintD3D("\n************Upload Failed**************\n");
			PrintD3D2("Error: %ld\n", hres & 0x0000FFFF);
			PrintD3D2("x1: %ld ", i_x);
			PrintD3D2("y1: %ld ", i_y);
			PrintD3D2("x2: %ld ", i_x + i_width);
			PrintD3D2("y2: %ld\n", i_y + i_height);

			{
				CDDSize<DDSURFACEDESC2> sd;
				hres = pddsSurfaceVid->GetSurfaceDesc(&sd);
				if (FAILED(hres))
				{
					PrintD3D("Failed to get vidmem surface desc.\n");
					PrintD3D2("Vidmem pointer: %ld\n", pddsSurfaceVid);
				}
				else
				{
					PrintD3D2("Vid mem width: %ld ", sd.dwWidth);
					PrintD3D2("height: %ld\n", sd.dwHeight);
				}
			}

			{
				CDDSize<DDSURFACEDESC2> sd;
				hres = pddsSurfaceSys->GetSurfaceDesc(&sd);
				if (FAILED(hres))
				{
					PrintD3D("Failed to get sysmem surface desc.\n");
					PrintD3D2("Sysmem pointer: %ld\n", pddsSurfaceSys);
				}
				else
				{
					PrintD3D2("Sys mem width: %ld ", sd.dwWidth);
					PrintD3D2("height: %ld\n", sd.dwHeight);
				}
			}
			PrintD3D("************Upload Failed**************\n");
			//__asm int 3
		}
		DirectDraw::err = hres;
	#endif

		bool b_succeeded = hres == DD_OK;

		proHardware.psUploads.Add(ctmr(), iTotalPixels());

		// Presumed to always succeed.
		return b_succeeded;
	}
	
	//******************************************************************************************
	void CRasterD3D::RemoveFromSystem()
	{
		ReleasePObject(pddsSurfaceSys);
	}
	
	//******************************************************************************************
	void CRasterD3D::RemoveFromVideo()
	{
		ReleasePObject(pddsSurfaceVid);
		ReleasePObject(pd3dtexTex);
	}

	//******************************************************************************************
	uint32 CRasterD3D::u4GetTypeID() const
	{
		return u4MakeTypeIDD3D(iWidth, iHeight, ed3dtexType);
	}

	//******************************************************************************************
	bool CRasterD3D::bRestore()
	{
		// Function not implemented yet.
		return true;
	}

	//**********************************************************************************************
	uint32 CRasterD3D::u4MakeTypeIDD3D(int i_width, int i_height, ED3DTextureType ed3dtex)
	{
		uint32 u4_retval = 0;	// Return value.
		
		u4_retval ^= uint32(i_width);
		u4_retval <<= 12;
		u4_retval ^= uint32(i_height);
		u4_retval <<= 4;
		u4_retval ^= uint32(ed3dtex);
		return u4_retval;
	}

	//******************************************************************************************
	bool CRasterD3D::bVerifyConstruction() const
	{
		//
		// To do:
		//		Special test for NVidia.
		//
		return (pddsSurfaceSys != 0) && (pddsSurfaceVid != 0);
	}

	//******************************************************************************************
	CRasterD3D* CRasterD3D::prasd3dGet()
	{
		return this;
	}

	//******************************************************************************************
	void CRasterD3D::ReleaseSharedSurfaces()
	{
		mapddSurfaces.ReleaseAll();
	}


//**********************************************************************************************
//
// CRasterD3D::CPriv implementation.
//

	//******************************************************************************************
	bool CRasterD3D::CPriv::bCreateVidMemSurface()
	{
		// Do nothing if Direct3D is not in use.
		if (!d3dDriver.bUseD3D())
			return false;

		// Do nothing if no system memory texture is specified.
		if (!pddsSurfaceSys)
			return false;

		CCycleTimer ctmr;				// Timer for this operation.
		CDDSize<DDSURFACEDESC2> ddsd;	// Surface description.

		// Copy the existing description for the system memory surface.
		DirectDraw::err = pddsSurfaceSys->GetSurfaceDesc(&ddsd);

		// Change flags for the video memory surface.
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;

		// Set allocation flags.
		switch (easAllocation)
		{
			case easNonLocal:
				ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;
				break;

			case easManaged:
			default:
				ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
				ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
				break;
		}

		switch (ehHint)
		{
			case ehDynamic:
				ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_HINTDYNAMIC;
				break;
			case ehStatic:
				ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_HINTSTATIC;
				break;
			case ehOpaque:
				ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_OPAQUE;
				break;
		}

		// Create the direct draw surface.
		HRESULT hres = DirectDraw::pdd4->CreateSurface(&ddsd, &pddsSurfaceVid, 0);

		// If the create succeeds, the function is finished.
		if (!FAILED(hres) && pddsSurfaceVid)
		{
			psUploadCreateVid.Add(ctmr(), iTotalPixels());
			return true;
		}

	#if VER_TEST

		if (hres == DDERR_OUTOFVIDEOMEMORY)
			PrintD3D("D3D texture allocation failed: out of memory\n");
		else
			PrintD3D("D3D texture allocation failed: unknown cause\n");

		// Set allocation flags.
		switch (easAllocation)
		{
			case easNonLocal:
				ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;
				break;

			case easManaged:
			default:
				ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
				ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
				break;
		}

		if (easAllocation == easNonLocal)
		{
			DDSCAPS2 ddscaps;
			DWORD    dw_total;
			DWORD    dw_free;

			// Query to get the available amount of local texture memory.
			memset(&ddscaps, 0, sizeof(ddscaps));
			ddscaps.dwCaps = DDSCAPS_TEXTURE;
			hres = DirectDraw::pdd4->GetAvailableVidMem(&ddscaps, &dw_total, &dw_free);
			if (!FAILED(hres))
			{
				dprintf("Total texture memory: %ld\n", dw_total);
				dprintf("Free texture memory:  %ld\n", dw_free);
			}
		}

	#endif

		// Soft landing.
		agpAGPTextureMemManager.FlushUnused();
		
		// Return failure.
		pddsSurfaceVid = 0;
		psUploadCreateVid.Add(ctmr(), iTotalPixels());
		return false;
	}

	//******************************************************************************************
	LPDIRECTDRAWSURFACE4 CRasterD3D::CPriv::pddsCreateSysMemSurface(ED3DTextureType ed3dtex)
	{
		CCycleTimer ctmr;					// Timer for this operation.
		CDDSize<DDSURFACEDESC2> ddsd;		// Surface description.
		LPDIRECTDRAWSURFACE4    pdds = 0;	// Pointer to the created DirectDraw surface.

		// Set the surface description.
		ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		ddsd.dwWidth        = iWidth;
		ddsd.dwHeight       = iHeight;

		ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;

		InitDXStruct(ddsd.ddpfPixelFormat);

		// Set the pixel format.
		if (d3dDriver.bUseD3D())
		{
			// Use Direct3D's format.
			ddsd.ddpfPixelFormat = d3dDriver.ddpfGetPixelFormat(ed3dtex);
		}
		else
		{
			// Use the screen format.
			CDDSize<DDSURFACEDESC2> ddsd_screen;
			InitDXStruct(ddsd_screen.ddpfPixelFormat);

			prasMainScreen->pddsDraw4->GetSurfaceDesc(&ddsd_screen);
			ddsd.ddpfPixelFormat = ddsd_screen.ddpfPixelFormat;
		}

		// Create the direct draw texture.
		Verify(DirectDraw::pdd4->CreateSurface(&ddsd, &pdds, 0) == DD_OK);
		Assert(pdds);

		// Return success.
		psUploadCreateSys.Add(ctmr(), iTotalPixels());
		return pdds;
	}

	//******************************************************************************************
	void CRasterD3D::CPriv::SetupPixelFormat(ED3DTextureType ed3dtex)
	{
		CDDSize<DDSURFACEDESC2> ddsd;		// Surface description.

		// Set the surface description.
		ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
		ddsd.dwWidth        = iWidth;
		ddsd.dwHeight       = iHeight;

		ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;

		InitDXStruct(ddsd.ddpfPixelFormat);

		// Set the pixel format.
		if (d3dDriver.bUseD3D())
		{
			// Use Direct3D's format.
			ddsd.ddpfPixelFormat = d3dDriver.ddpfGetPixelFormat(ed3dtex);

			// Set the raster's pixel format.
			pxf = CPixelFormat
			(
				(int)ddsd.ddpfPixelFormat.dwRGBBitCount,
				ddsd.ddpfPixelFormat.dwRBitMask, 
				ddsd.ddpfPixelFormat.dwGBitMask, 
				ddsd.ddpfPixelFormat.dwBBitMask
			);
		}
		else
		{
			// Use the screen format.
			CDDSize<DDSURFACEDESC2> ddsd_screen;
			InitDXStruct(ddsd_screen.ddpfPixelFormat);

			prasMainScreen->pddsDraw4->GetSurfaceDesc(&ddsd_screen);

			pxf = prasMainScreen->pxf;
		}
	}

	//******************************************************************************************
	bool CRasterD3D::CPriv::bCreateSysMemSurface(ED3DTextureType ed3dtex)
	{
		Assert(pddsSurfaceSys == 0);

		// Set up the pixel format for the raster.
		SetupPixelFormat(ed3dtex);

		if (bSharedSysMem)
		{
			uint32 u4_hash = u4HashSurfaceType(ed3dtex);
			pddsSurfaceSys = mapddSurfaces.pddsFind(u4_hash);
			if (!pddsSurfaceSys)
			{
				pddsSurfaceSys = pddsCreateSysMemSurface(ed3dtex);
				if (pddsSurfaceSys)
				{
					mapddSurfaces.Add(u4_hash, pddsSurfaceSys);
				}
			}
		}
		else
		{
			pddsSurfaceSys = pddsCreateSysMemSurface(ed3dtex);
		}
		return pddsSurfaceSys != 0;
	}

	//******************************************************************************************
	void CRasterD3D::CPriv::SetColourKey()
	{
		Assert(pddsSurfaceSys);
		Assert(pddsSurfaceVid);

		DDCOLORKEY colkeyinfo;
		colkeyinfo.dwColorSpaceLowValue  = 0x0000;
		colkeyinfo.dwColorSpaceHighValue = 0x0000;

		pddsSurfaceSys->SetColorKey(DDCKEY_SRCBLT, &colkeyinfo);
		pddsSurfaceVid->SetColorKey(DDCKEY_SRCBLT, &colkeyinfo);
	}


//
// Global static variables.
//
uint32 CRasterD3D::u4FrameCount = 0;
int    CRasterD3D::iLockCount   = 0;