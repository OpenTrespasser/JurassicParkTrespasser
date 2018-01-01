/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		class CAGPTextureMemManager.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/View/AGPTextureMemManager.hpp                                     $
 * 
 * 5     10/04/98 8:14a Pkeet
 * Added the function 'FlushUnused.'
 * 
 * 4     7/28/98 3:12p Pkeet
 * Added a member function to restore non-pageable textures.
 * 
 * 3     7/27/98 2:35p Pkeet
 * Added code to output texture memory allocations and usage to the debug window.
 * 
 * 2     7/26/98 6:08p Pkeet
 * Added code to track Direct3D memory use.
 * 
 * 1     7/23/98 6:19p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_AGPTEXTUREMEMMANAGER_HPP
#define HEADER_LIB_VIEW_AGPTEXTUREMEMMANAGER_HPP


//
// Necessary includes.
//
#include "RasterD3D.hpp"


//
// Forward declarations.
//


//
// Macros.
//

// Macro to track use of memory for Direct3D.
#define iTRACK_D3D_MEM_USE (1 && BUILDVER_MODE != MODE_FINAL)


//
// Class definitions.
//

//**********************************************************************************************
//
class CAGPTextureMemManager
//
// Prefix: agp
//
// A texture manager for cards texturing out of AGP memory.
//
// Notes:
//		Currently the manager is hardwired to use 'CRasterD3D.'
//
//**************************************
{
private:

	int iBytesInUse;			// Number of bytes in use for the current frame.
	int iBytesTotal;			// Total number of bytes in actual allocated textures.
	int iBytesAllocated;		// Number of bytes allocated from Direct3D for AGP texturing.
	int iBytesLowWaterMark;		// Desired number of bytes allocated.
	int iBytesHighWaterMark;	// Maximum number of bytes that can be allocated before the
								// system is in danger of failing.
	int iTotalNonPageable;		// Total amount of memory used for non-pageable textures in bytes.
	
#if iTRACK_D3D_MEM_USE
	// The maximum number of bytes used for any frame and for the entire system.
	static int iMaxBytesAllocated;
	static int iMaxBytesUsedPerFrame;
#endif // iTRACK_D3D_MEM_USE

public:

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	// Constructor.
	CAGPTextureMemManager
	(
	);

	// Destructor.
	~CAGPTextureMemManager
	(
	);

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	rptr<CRaster> prasCreate
	(
		int             i_width,						// Desired width of raster.
		int             i_height,						// Desired height of raster.
		ED3DTextureType ed3dtex = ed3dtexSCREEN_OPAQUE	// Type of raster to create.
	);
	//
	// Returns a raster either of a matching size from the pool, or one generated.
	//
	// Notes:
	//		The caller of this member function should use the 'Add' member function to upload
	//		this texture.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bAdd
	(
		rptr<CRaster>& rpras	// Raster to delete.
	);
	//
	// Returns the raster to the global pool if the raster is a Direct3D raster. If it is not,
	// the raster is simply deleted in the same fashion as the regular memory rasters.
	//
	//**************************************

	//******************************************************************************************
	//
	void Delete
	(
		rptr<CRaster>& rpras	// Raster to delete.
	);
	//
	// Returns the raster to the global pool if the raster is a Direct3D raster. If it is not,
	// the raster is simply deleted in the same fashion as the regular memory rasters.
	//
	//**************************************

	//******************************************************************************************
	//
	void BeginFrame
	(
	);
	//
	// Prepares the memory manager for the current frame.
	//
	//**************************************

	//******************************************************************************************
	//
	void EndFrame
	(
	);
	//
	// Cleans up the memory manager from work of the current frame.
	//
	//**************************************

	//******************************************************************************************
	//
	void MakeCurrentList
	(
	);
	//
	// Moves unused textures to the used list, and moves textures in use for the current frame
	// to the current active list.
	//
	//**************************************

	//******************************************************************************************
	//
	void AllocateMemory
	(
		int i_bytes_allocated
	);
	//
	// Allocates memory for use by the texture memory manager.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bIsActive
	(
	);
	//
	// Returns 'true' if the system is in use.
	//
	//**************************************

	//******************************************************************************************
	//
	void Purge
	(
	);
	//
	// Purges all memory managed textures from Direct3D.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void OutputAllocations
	(
	) const;
	//
	// Outputs texture use statistics to the debug window.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void RestoreNonpageable
	(
	);
	//
	// Restores non-pageable textures.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void FlushUnused
	(
	);
	//
	// Flushes all unused textures.
	//
	//**************************************

private:

	class CPriv;
	friend class CPriv;

};


//
// Global pool variables.
//

// D3D raster pool.
extern CAGPTextureMemManager agpAGPTextureMemManager;


#endif // HEADER_LIB_VIEW_AGPTEXTUREMEMMANAGER_HPP

