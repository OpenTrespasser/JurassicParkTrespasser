/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		class CRasterPool.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/RasterPool.hpp                                               $
 * 
 * 5     7/27/98 6:30p Pkeet
 * Added support for working in fixed sizes.
 * 
 * 4     4/09/98 10:57a Pkeet
 * Added comment.
 * 
 * 3     4/02/98 4:56p Pkeet
 * Added LRU and map classes.
 * 
 * 2     4/02/98 2:04p Pkeet
 * Removed the 'bMaintainBoth' flag.
 * 
 * 1     4/02/98 1:20p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_RASTERPOOL_HPP
#define HEADER_LIB_VIEW_RASTERPOOL_HPP


//
// Necessary includes.
//
#include "RasterD3D.hpp"


//
// Forward declarations.
//
class CPoolLRU;
class CPoolMap;


//
// Class definitions.
//

//**********************************************************************************************
//
class CRasterPool
//
// Prefix: pool
//
// A texture raster pool manager.
//
// Notes:
//		Currently the pool manager is hardwired to use 'CRasterD3D.'
//
//**************************************
{
private:

	CPoolLRU*  plruPool;
	CPoolMap*  pmapPool;
	static int iBytesAvailable;
	static int iBytesUsed;

public:

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	// Constructor.
	CRasterPool
	(
	);

	// Destructor.
	virtual ~CRasterPool
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
	//		The parameters taken by this function match the constructor parameters for
	//		CRasterD3D.
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
	void FlushAll
	(
	);
	//
	// Flushes all rasters from the global pool.
	//
	//**************************************

	//******************************************************************************************
	//
	static void SetTextureMemSize
	(
		int i_bytes_available	// Number of bytes that the system may use.
	);
	//
	// Flushes all rasters from the global pool.
	//
	//**************************************

	//******************************************************************************************
	//
	void OutputAllocations
	(
	) const;
	//
	// Outputs texture allocations to the debug window.
	//
	//**************************************

private:

	class CPriv;
	friend class CPriv;
	friend class CPoolLRU;
	friend class CPoolMap;

};


//
// Global pool variables.
//

// D3D raster pool.
extern CRasterPool poolD3DRaster;


#endif // HEADER_LIB_VIEW_RASTERPOOL_HPP

