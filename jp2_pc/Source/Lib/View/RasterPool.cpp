/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Implementation of RasterPool.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/RasterPool.cpp                                               $
 * 
 * 13    10/04/98 8:20a Pkeet
 * Made 'Delete' always perform cleanup. Added an assert.
 * 
 * 12    10/03/98 11:20p Pkeet
 * Fixed problem with LRU not matching the STL map of texture sizes and types.
 * 
 * 11    10/02/98 4:05p Pkeet
 * Made failure to allocate a D3D raster for a mip level more robust.
 * 
 * 10    9/15/98 7:03p Pkeet
 * More rigorous purge.
 * 
 * 9     8/28/98 1:10p Asouth
 * change required by different STL implementation
 * 
 * 8     7/27/98 6:30p Pkeet
 * Added support for working in fixed sizes.
 * 
 * 7     7/27/98 12:28p Pkeet
 * Added an initialization class for 'CRasterD3D.'
 * 
 * 6     7/23/98 9:08p Pkeet
 * Added code to track the creation and deletion of Direct3D textures using a macro switch in
 * 'RasterD3D.hpp.'
 * 
 * 5     4/08/98 5:31p Pkeet
 * Made the 'Delete' member function handle both Direct3D and non-Direct3D rasters.
 * 
 * 4     4/02/98 8:03p Pkeet
 * Fixed bug in LRU system.
 * 
 * 3     4/02/98 4:56p Pkeet
 * Added LRU and map classes. Implemented an LRU list.
 * 
 * 2     4/02/98 2:05p Pkeet
 * Removed the 'bMaintainBoth' flag.
 * 
 * 1     4/02/98 1:20p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include <list>
#include <map>
#include "common.hpp"
#include "RasterPool.hpp"

#include "Lib/W95/WinInclude.hpp"
#include "Lib/W95/DD.hpp"
#include "Raster.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/View/RasterPool.hpp"

#if bDEBUG_DIRECT3D
	#include "Lib/Sys/DebugConsole.hpp"
#endif // bDEBUG_DIRECT3D


//
// Class definitions.
//

//**********************************************************************************************
//
class CPoolMap : public std::multimap< uint32, rptr<CRaster>, std::less<uint32> >
//
// 
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	//******************************************************************************************
	//
	rptr<CRaster> prasGet
	(
		int i_width,
		int i_height,
		ED3DTextureType ed3dtex
	)
	//
	// 
	//
	//**************************************
	{
		// Generate an ID.
		uint32 u4_id = CRasterD3D::u4MakeTypeIDD3D(i_width, i_height, ed3dtex);

		// Attempt to find a raster.
		iterator it = find(u4_id);

		// Return a null pointer if a raster cannot be found.
		if (it == end())
			return rptr0;

		// Return a reference to the existing raster after removing it from the pool.
		rptr<CRaster> pras = (*it).second;
		Erase(it);

		// Return the found or created raster.
		Assert(pras);
		return pras;
	}

	//******************************************************************************************
	//
	void Remove
	(
		uint32 u4_id
	)
	//
	// 
	//
	//**************************************
	{
		// Attempt to find a raster.
		iterator it = find(u4_id);

		// Remove the raster if one with the same ID was found.
		if (it != end())
			Erase(it);
	}

	//******************************************************************************************
	//
	void Return
	(
		rptr<CRaster> pras	// Raster to add.
	)
	//
	// Returns the raster to the global pool of rasters.
	//
	//**************************************
	{
	#ifdef __MWERKS__
		insert( pair< const uint32, rptr<CRaster> >(pras->u4GetTypeID(), pras));
	#else
		insert(std::pair< uint32, rptr<CRaster> >(pras->u4GetTypeID(), pras));
	#endif
		CRasterPool::iBytesUsed += pras->iSurfaceMemBytes();
	}

	//******************************************************************************************
	//
	void Flush
	(
	)
	//
	// Removes all rasters from the global pool.
	//
	//**************************************
	{
		erase(begin(), end());
		CRasterPool::iBytesUsed = 0;
	}

	//******************************************************************************************
	//
	void Erase
	(
		iterator& it	// Iterator to erase.
	)
	//
	// 
	//
	//**************************************
	{
		CRasterPool::iBytesUsed -= (*it).second->iSurfaceMemBytes();
		erase(it);
	}

};


//**********************************************************************************************
//
class CPoolLRU : public std::list<uint32>
//
// Object to maintain a least recently used (LRU) list.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	void Remove
	(
		CPoolMap* pmap_pool,	// Map pool.
		int i_num = 5			// Number to remove.
	)
	//
	// Removes the specified number of textures from the unused list.
	//
	//**************************************
	{
		Assert(pmap_pool);

		int i = 0;
		while (!empty() && i < i_num)
		{
			pmap_pool->Remove(*begin());
			pop_front();
		}
	}

	//******************************************************************************************
	//
	void RemoveID
	(
		uint32 u4_id
	)
	//
	// 
	//
	//**************************************
	{
		iterator it = begin();
		for (; it != end(); ++it)
			if (*it == u4_id)
			{
				erase(it);
				return;
			}
	}

	//******************************************************************************************
	//
	void Return
	(
		rptr<CRaster> pras	// Raster to add.
	)
	//
	// 
	//
	//**************************************
	{
		push_back(pras->u4GetTypeID());
	}

	//******************************************************************************************
	//
	void Flush
	(
	)
	//
	// 
	//
	//**************************************
	{
		erase(begin(), end());
	}

};


//**********************************************************************************************
//
class CRasterPool::CPriv : public CRasterPool
//
// Private member functions for class 'CRasterPool.'
//
//**************************************
{
public:

	//******************************************************************************************
	//
	bool bMakeRaster
	(
		rptr<CRaster>& rpras,
		int i_width,
		int i_height,
		ED3DTextureType ed3dtex
	);
	//
	// Creates a raster of the specified dimensions and type. Returns 'true' if the raster
	// was successfully allocated.
	//
	//**************************************

};


//
// Class implementation.
//

//**********************************************************************************************
//
// CRasterPool implementation.
//

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	// Constructor.
	CRasterPool::CRasterPool()
		: plruPool(new CPoolLRU()), pmapPool(new CPoolMap())
	{
		iBytesUsed      = 0;
		iBytesAvailable = 0;
	}

	// Destructor.
	CRasterPool::~CRasterPool()
	{
		delete plruPool;
		delete pmapPool;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	rptr<CRaster> CRasterPool::prasCreate(int i_width, int i_height, ED3DTextureType ed3dtex)
	{
		Assert(plruPool);
		Assert(pmapPool);
		AlwaysAssert(plruPool->size() == pmapPool->size());

		// Attempt to find a raster of the request size and type.
		rptr<CRaster> pras = pmapPool->prasGet(i_width, i_height, ed3dtex);

		// If one was found, simply return it.
		if (pras)
		{
			plruPool->RemoveID(pras->u4GetTypeID());
			return pras;
		}

		// Create a new one.
		while (!priv_self.bMakeRaster(pras, i_width, i_height, ed3dtex))
		{
			if (plruPool->empty())
				break;
			plruPool->Remove(pmapPool);
		}

		// Return the newly-created raster.
		return pras;
	}

	//******************************************************************************************
	void CRasterPool::Delete(rptr<CRaster>& rpras)
	{
		// Do nothing if the raster is already destroyed.
		if (rpras)
		{
			// Return D3D rasters to the memory pool, but delete regular memory rasters.
			if (rpras->bD3DRaster())
			{
				// Return memory to the pool.
				pmapPool->Return(rpras);
				plruPool->Return(rpras);
			}
			rpras = rptr0;
		}

		//
		// Start removing rasters until the amount of memory used is less than the amount of
		// memory allocated for the system.
		//
		while (iBytesUsed > iBytesAvailable && !plruPool->empty())
			plruPool->Remove(pmapPool, 1);
		AlwaysAssert(plruPool->size() == pmapPool->size());
	}

	//******************************************************************************************
	void CRasterPool::FlushAll()
	{
		delete plruPool;
		delete pmapPool;
		iBytesUsed = 0;

		plruPool = new CPoolLRU();
		pmapPool = new CPoolMap();
	}

	//******************************************************************************************
	void CRasterPool::SetTextureMemSize(int i_bytes_available)
	{
		// Remove existing allocations.
		poolD3DRaster.FlushAll();

		// Set a new limit.
		iBytesAvailable = i_bytes_available;
	}

	//******************************************************************************************
	void CRasterPool::OutputAllocations() const
	{
	#if bDEBUG_DIRECT3D

		dprintf("Pool texture memory available : %ld Kb\n", iBytesAvailable >> 10);
		dprintf("Pool texture memory used      : %ld Kb\n", iBytesUsed >> 10);
		dprintf("Number of textures in pool    : %ld\n", plruPool->size());
		dprintf("Number of textures in map     : %ld\n", pmapPool->size());
		
	#endif // bDEBUG_DIRECT3D
	}


//**********************************************************************************************
//
// class CRasterPool::CPriv implementation.
//

	//******************************************************************************************
	bool CRasterPool::CPriv::bMakeRaster(rptr<CRaster>& rpras, int i_width, int i_height,
		                                 ED3DTextureType ed3dtex)
	{
	#if bTRACK_D3D_RASTERS
		TrackSystem(etrPool);
	#endif

		// Make sure that there is enough memory in the system.
		int i_requested_bytes = i_width * i_height * 2;
		if (i_requested_bytes + iBytesUsed > iBytesAvailable)
			return false;

		// Create the raster.
		rpras = rptr_cast(CRaster, rptr_new CRasterD3D(CRasterD3D::CInitDiscreet
		(
			i_width,
			i_height,
			ed3dtex
		)));

		// Make sure that the raster has successfully been allocated.
		if (!rpras)
			return false;
		if (!rpras->bVerifyConstruction())
		{
			rpras = rptr0;
			return false;
		}			

		return true;
	}


//
// Global pool variables.
//
CRasterPool poolD3DRaster;
int CRasterPool::iBytesAvailable = 0;
int CRasterPool::iBytesUsed      = 0;