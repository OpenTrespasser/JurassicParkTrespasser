/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		class CRasterD3D.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/RasterD3D.hpp                                                $
 * 
 * 30    10/07/98 6:12a Pkeet
 * Added an assert.
 * 
 * 29    8/28/98 8:26p Pkeet
 * Removed the TTextureAccess enumeration. Added an enumeration for hint type, and added code
 * for the hint type.
 * 
 * 28    8/21/98 7:41p Pkeet
 * Added preloaded enum type to texture tracker.
 * 
 * 27    8/20/98 11:11p Pkeet
 * Added the 'ReleaseSharedSurfaces' static member function.
 * 
 * 26    8/18/98 3:37p Pkeet
 * Remove the forced dimension parameter.
 * 
 * 25    8/08/98 6:57p Pkeet
 * Added a selection of allocation schemes.
 * 
 * 24    8/07/98 11:52a Pkeet
 * Added the 'bIsLocked' and 'bIsLockedAll' member functions.
 * 
 * 23    7/27/98 3:45p Pkeet
 * Implemented shared system memory surfaces for discreetly allocated textures.
 * 
 * 22    7/27/98 12:52p Pkeet
 * Added a separate initialization class for page managed textures.
 * 
 * 21    7/27/98 12:28p Pkeet
 * Added an initialization class for 'CRasterD3D.'
 * 
 * 20    7/23/98 9:08p Pkeet
 * Added code to track the creation and deletion of Direct3D textures using a macro switch in
 * 'RasterD3D.hpp.'
 * 
 * 19    7/23/98 6:23p Pkeet
 * Added support for texturing in hardware out of system memory.
 * 
 * 18    7/20/98 10:32p Pkeet
 * Upgraded to Direct3D 4's interface.
 * 
 * 17    7/09/98 8:47p Pkeet
 * Added code to support direct access to texture memory.
 * 
 * 16    6/12/98 9:11p Pkeet
 * Added parameter to constructor to force the Direct3D texture surface to be a given size.
 * 
 * 15    5/14/98 11:49a Pkeet
 * Moved the interface objects for system and video surfaces back into the general 'bUpload'
 * call.
 * 
 * 14    5/12/98 6:44p Mlange
 * Renamed bUploadRegion() to bUpload() and changed its parameters.
 * 
 * 13    5/12/98 12:28p Pkeet
 * Added the 'bUploadRegion' member function.
 * 
 * 12    4/08/98 5:30p Pkeet
 * Added the 'bD3DRaster' member function.
 * 
 * 11    4/08/98 3:00p Pkeet
 * Made 'bSetHandle' a member of 'priv_self.' Added caching of com objects that provide texture
 * interfaces.
 * 
 * 10    4/02/98 4:57p Pkeet
 * Added the 'bVerifyConstruction' and 'u4MakeTypeIDD3D' member functions.
 * 
 * 9     4/02/98 2:03p Pkeet
 * Added the 'u4GetTypeID' member function. Removed the 'bMaintainBoth' flag.
 * 
 * 8     4/01/98 5:47p Pkeet
 * Added a private class for use by 'priv_self.'
 * 
 * 7     1/12/98 6:15p Pkeet
 * Added a lock and unlock toggle.
 * 
 * 6     1/10/98 3:01p Pkeet
 * Added the 'RemoveFromSystem' member function.
 * 
 * 5     12/18/97 6:05p Pkeet
 * Fixed Riva 128 bug.
 * 
 * 4     12/16/97 3:43p Pkeet
 * Added structures for maintaining both a system and video memory raster.
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

#ifndef HEADER_LIB_VIEW_RASTERD3D_HPP
#define HEADER_LIB_VIEW_RASTERD3D_HPP


//
// Necessary includes.
//
#include "Raster.hpp"
#include "Lib/W95/Direct3D.hpp"


//
// Enumerations.
//
enum TD3DHint
{
	ehDynamic,
	ehStatic,
	ehOpaque,
	ehNone
};




//
// Forward declarations.
//
class CAGPTextureMemManager;


//
// Macros.
//

// Switch to track leaked rasters.
#define bTRACK_D3D_RASTERS (0)


//
// Class definitions.
//

//**********************************************************************************************
//
class CRasterD3D: public CRaster
//
// Prefix: rasd3d
//
// A D3D texture raster.
//
//**************************************
{
private:

	ED3DTextureType      ed3dtexType;		// Type of texture.
	LPDIRECTDRAWSURFACE4 pddsSurfaceSys;	// DirectDraw surface.
	LPDIRECTDRAWSURFACE4 pddsSurfaceVid;	// DirectDraw surface.
	bool                 bLocked;			// Indicates if surface is locked.
	LPDIRECT3DTEXTURE2   pd3dtexTex;		// Direct3D texture pointer.
	uint32               u4LastUsedFrame;	// Last frame the raster was used.
	bool                 bSharedSysMem;		// Flag for using a shared system memory surface.
	EAllocationScheme    easAllocation;		// Texture memory allocation scheme.
	TD3DHint             ehHint;			// D3D hint flag.

	// Static global variables.
	static uint32        u4FrameCount;		// Frame counter for an LRU mechanism.
	static int           iLockCount;		// Number of locked D3D rasters.

#if bTRACK_D3D_RASTERS
public:
	char* strSource;
#endif // bTRACK_D3D_RASTERS

public:

	//******************************************************************************************
	//
	// Initialization classes for CRaster.
	//

	//******************************************************************************************
	//
	class CInit
	//
	// Default initialization.
	//
	//**************************************
	{
	public:
		int               iWidth;			// Width of texture to allocate.
		int               iHeight;			// Height of texture to allocate.
		ED3DTextureType   ed3dtexType;		// Direct3D texture type.
		bool              bUploadImm;		// Upload on allocation. Used to ensure allocation.
		bool              bSharedSysMem;	// Flag for using a shared system memory surface.
		EAllocationScheme easAllocation;	// Texture memory allocation scheme.
		TD3DHint          ehHint;			// D3D hint flag.

		// Default.
		CInit
		(
			int             i_width      = 256,
			int             i_height     = 256,
			ED3DTextureType ed3dtex      = ed3dtexSCREEN_OPAQUE,
			bool            b_upload_imm = true
		);

	};

	//******************************************************************************************
	//
	class CInitPage : public CInit
	//
	// Initialization for page managed systems.
	//
	//**************************************
	{
	public:

		// Default.
		CInitPage
		(
			int             i_width  = 256,
			int             i_height = 256,
			ED3DTextureType ed3dtex  = ed3dtexSCREEN_OPAQUE
		);

	};

	//******************************************************************************************
	//
	class CInitDiscreet : public CInit
	//
	// Initialization for discreet texture allocations.
	//
	//**************************************
	{
	public:

		// Default.
		CInitDiscreet
		(
			int             i_width  = 256,
			int             i_height = 256,
			ED3DTextureType ed3dtex  = ed3dtexSCREEN_OPAQUE
		);

	};

	//******************************************************************************************
	//
	class CInitDiscreetStatic : public CInitDiscreet
	//
	// Initialization for discreet texture allocations.
	//
	//**************************************
	{
	public:

		// Default.
		CInitDiscreetStatic
		(
			int             i_width  = 256,
			int             i_height = 256,
			ED3DTextureType ed3dtex  = ed3dtexSCREEN_OPAQUE
		);

	};

	//******************************************************************************************
	//
	class CInitDiscreetOpaque : public CInitDiscreet
	//
	// Initialization for discreet texture allocations.
	//
	//**************************************
	{
	public:

		// Default.
		CInitDiscreetOpaque
		(
			int             i_width  = 256,
			int             i_height = 256,
			ED3DTextureType ed3dtex  = ed3dtexSCREEN_OPAQUE
		);

	};


	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	// Constructor using a format determined by Direct3D.
	CRasterD3D
	(
		const CInit& init
	);

	// Destructor.
	virtual ~CRasterD3D();

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void Lock() override;

	//******************************************************************************************
	static bool bIsLockedAll()
	{
		return iLockCount != 0;
	}

	//******************************************************************************************
	virtual bool bIsLocked() override
	{
		return bLocked;
	}

	//******************************************************************************************
	virtual void Unlock() override;

	//******************************************************************************************
	//
	// Overloaded Member functions.
	//

	//******************************************************************************************
	virtual void* pd3dtexGet() override
	{
		Assert(pd3dtexTex);

		// Account for frame for the LRU.
		u4LastUsedFrame = u4FrameCount;
		return pd3dtexTex;
	}

	//******************************************************************************************
	virtual bool bUpload() override;

	//******************************************************************************************
	virtual bool bUpload(int i_x, int i_y, int i_width, int i_height) override;

	//******************************************************************************************
	virtual void RemoveFromVideo() override;

	//******************************************************************************************
	virtual void RemoveFromSystem() override;

	//******************************************************************************************
	virtual uint32 u4GetTypeID() const override;

	//******************************************************************************************
	virtual bool bRestore() override;

	//******************************************************************************************
	virtual bool bVerifyConstruction() const override;

	//******************************************************************************************
	virtual bool bD3DRaster() const override
	{
		return true;
	}

	//******************************************************************************************
	virtual CRasterD3D* prasd3dGet() override;

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	static uint32 u4MakeTypeIDD3D
	(
		int i_width,				// Desired width of raster.
		int i_height,				// Desired height of raster.
		ED3DTextureType ed3dtex		// Type of raster to create.
	);
	//
	// Returns a type ID.
	//
	//**************************************

	//******************************************************************************************
	//
	ED3DTextureType ed3dtexGet
	(
	) const
	//
	// Returns the type of Direct3D texture.
	//
	//**************************************
	{
		return ed3dtexType;
	}

	//******************************************************************************************
	//
	uint32 u4Age
	(
	) const
	//
	// Returns the number of frames this raster remains unused for.
	//
	//**************************************
	{
		AlwaysAssert((int(u4FrameCount) - int(u4LastUsedFrame)) >= 0);
		return u4FrameCount - u4LastUsedFrame;
	}

	//******************************************************************************************
	//
	static void ReleaseSharedSurfaces
	(
	);
	//
	// Releases shared DirectDraw system memory buffers.
	//
	//**************************************

private:

	class CPriv;
	friend class CPriv;
	friend class CAGPTextureMemManager;

};


// Tracking stuff.
#if bTRACK_D3D_RASTERS

	enum ETrackD3DRas { etrWater, etrSky, etrTerrain, etrCaches, etrAGP, etrPre, etrPool, etrUnknown };
	void TrackSystem(ETrackD3DRas etr);
	void SetupTrack();

#endif // bTRACK_D3D_RASTERS


#endif // HEADER_LIB_VIEW_RASTERD3D_HPP

