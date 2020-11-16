/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Implementation of AGPTextureMemManager.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/AGPTextureMemManager.cpp                                     $
 * 
 * 33    10/07/98 6:16a Pkeet
 * Added a retirement age.
 * 
 * 32    10/05/98 6:02a Pkeet
 * Upped threshold.
 * 
 * 31    10/04/98 8:18a Pkeet
 * Added the function 'FlushUnused.' Fixed bug that left rasters laying around.
 * 
 * 30    10/02/98 4:05p Pkeet
 * Made failure to allocate a D3D raster for a mip level more robust.
 * 
 * 29    9/15/98 7:03p Pkeet
 * More rigorous purge.
 * 
 * 28    8/31/98 10:32p Pkeet
 * Removed the purge from the texture manager's destructor.
 * 
 * 27    8/30/98 5:35p Pkeet
 * Removed the 'ClearLinks' private member function entirely.
 * 
 * 26    8/29/98 10:53p Pkeet
 * Raster links are now cleared using the texture stl map object.
 * 
 * 25    8/28/98 8:57p Pkeet
 * Made preloaded textures opaque.
 * 
 * 24    8/25/98 4:09p Rvande
 * Fixed a "for" scope thing.
 * 
 * 23    8/23/98 3:05a Pkeet
 * Direct3D water textures are now purged when switching resolutions.
 * 
 * 22    8/22/98 11:04p Pkeet
 * Added support for full texturing in 32 Mb.
 * 
 * 21    8/21/98 7:42p Pkeet
 * Added texture tracking for preloaded textures.
 * 
 * 20    8/09/98 7:36p Pkeet
 * Reduced the threshold for regular texturing.
 * 
 * 19    8/09/98 2:35p Pkeet
 * Activated the texture manager.
 * 
 * 18    8/08/98 11:01p Pkeet
 * Removed the divide by two for memory allocations.
 * 
 * 17    8/08/98 9:32p Pkeet
 * Fixed bug in the deallocation scheme.
 * 
 * 16    8/07/98 5:36p Pkeet
 * Fixed minor bug.
 * 
 * 15    8/01/98 4:44p Pkeet
 * Added a background colour for transparent textures.
 * 
 * 14    7/30/98 4:08p Pkeet
 * Large textures are disabled in hardware if texture create times are slower.
 * 
 * 13    7/30/98 1:27p Pkeet
 * Enabled animating textures in hardware.
 * 
 * 12    7/29/98 4:52p Pkeet
 * Fixed major cracking bug between polygons in the same mesh.
 * 
 * 11    7/29/98 11:46a Pkeet
 * The device changes addressing mode to wrap or clamp as required.
 * 
 * 10    7/28/98 8:25p Pkeet
 * Added code to set tiling flags.
 * 
 * 9     7/28/98 4:24p Pkeet
 * Added but disabled the ability to load higher level mip maps as non-pageable textures.
 * 
 * 8     7/28/98 3:13p Pkeet
 * Added a member function to restore non-pageable textures. The 'Purge' member function now
 * also purges non-pageable textures.
 * 
 * 7     7/27/98 6:32p Pkeet
 * Direct3D rasters are now allocated and deallocated from the pool manager.
 * 
 * 6     7/27/98 3:45p Pkeet
 * Implemented shared system memory surfaces for discreetly allocated textures.
 * 
 * 5     7/27/98 2:35p Pkeet
 * Added code to output texture memory allocations and usage to the debug window.
 * 
 * 4     7/27/98 12:29p Pkeet
 * Added an initialization class for 'CRasterD3D.'
 * 
 * 3     7/26/98 6:10p Pkeet
 * Added code to track memory. Added required functions to clean up unused allocations if memory
 * starts to run low.
 * 
 * 2     7/23/98 9:08p Pkeet
 * Added code to track the creation and deletion of Direct3D textures using a macro switch in
 * 'RasterD3D.hpp.'
 * 
 * 1     7/23/98 6:18p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include <list>
#include "common.hpp"
#include "Lib/View/AGPTextureMemManager.hpp"

#include "Lib/W95/WinInclude.hpp"
#include "Lib/W95/DD.hpp"
#include "Raster.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/View/RasterPool.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/Loader/TextureManager.hpp"
#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/View/RasterConvertD3D.hpp"
#include "Lib/EntityDBase/RenderDB.hpp"
#include "Lib/Loader/LoadTexture.hpp"

#if bDEBUG_DIRECT3D
	#include "Lib/Sys/DebugConsole.hpp"
#endif // bDEBUG_DIRECT3D


//
// Macros.
//

// Use a higher-level map for non-pageable textures.
#define bHIGHER_LEVEL (0)

// Age to potentially retire texture.
const int iRetireAge = 8;


//
// Class definitions.
//

//**********************************************************************************************
//
class CRasterWrap
//
// 
//
//**************************************
{
public:

	rptr<CRaster> prasRaster;

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	// Default constructor.
	CRasterWrap()
	{
	}

	// Constructor.
	CRasterWrap(rptr<CRaster> pras)
	{
		prasRaster = pras;
	}

	// Constructor.
	~CRasterWrap()
	{
		// Return the raster to the global pool.
		if (prasRaster)
		{
			if (prasRaster->uNumRefs() == 1)
			{
				poolD3DRaster.Delete(prasRaster);
				AlwaysAssert(!prasRaster);
			}
			else
			{
				prasRaster = rptr0;
			}
		}
	}

};


//**********************************************************************************************
//
class CAGPTextureMemManager::CPriv : public CAGPTextureMemManager
//
// Private member functions for class 'CAGPTextureMemManager.'
//
//**************************************
{
public:

	//******************************************************************************************
	//
	bool bReducedUnused
	(
		int i_num_reduce = 10	// Number of textures to reduce the unused amount by.
	);
	//
	// Reduces the amount of unused D3D rasters by a fixed amount.
	//
	//**************************************

	//******************************************************************************************
	//
	void SetAlloc
	(
		rptr<CRaster> pras,			// Raster to get the size of allocation for.
		int           i_sign = 1	// Positive one to increment the allocation, negative one to
									// subtract it.
	);
	//
	// Returns the size of the allocation in bytes.
	//
	//**************************************

	//******************************************************************************************
	//
	void LoadNonPageable
	(
		CPartition* ppart // Partition to load maps from.
	);
	//
	// Loads non-pageable mipmaps into Direct3D textures.
	//
	// Notes:
	//		This function is recursive.
	//
	//**************************************

	//******************************************************************************************
	//
	void LoadNonPageable
	(
		rptr<CMesh> pmsh // Mesh to load maps from.
	);
	//
	// Loads non-pageable mipmaps into Direct3D textures.
	//
	//**************************************

	//******************************************************************************************
	//
	void LoadNonPageable
	(
		rptr<CTexture> ptex // Texture to load rasters from.
	);
	//
	// Loads non-pageable mipmaps into Direct3D textures.
	//
	//**************************************

	//******************************************************************************************
	//
	void SetTilingFlags
	(
		rptr<CMesh> pmsh // Mesh to load maps from.
	);
	//
	// Loads non-pageable mipmaps into Direct3D textures.
	//
	//**************************************

};


//
// Type definitions.
//

// Type for maintaining a list of currently active textures.
typedef std::list<CRasterWrap> TRasD3DList;


//
// Module specific variables.
//
TRasD3DList rlActive;
TRasD3DList rlUnused;


//
// Class implementation.
//

//**********************************************************************************************
//
// CAGPTextureMemManager implementation.
//

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	// Constructor.
	CAGPTextureMemManager::CAGPTextureMemManager()
	{
		iBytesTotal         = 0;
		iBytesInUse         = 0;
		iBytesAllocated     = 0;
		iBytesLowWaterMark  = 0;
		iBytesHighWaterMark = 0;
	}

	// Destructor.
	CAGPTextureMemManager::~CAGPTextureMemManager()
	{
	#if iTRACK_D3D_MEM_USE
		dprintf("\nMaximum D3D Textures allocated     : %ld Kb\n", iMaxBytesAllocated >> 10);
		dprintf("Maximum D3D Textures used per frame: %ld Kb\n\n", iMaxBytesUsedPerFrame >> 10);
	#endif // iTRACK_D3D_MEM_USE
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************

	//******************************************************************************************

	//******************************************************************************************
	void CAGPTextureMemManager::BeginFrame()
	{
	}

	//******************************************************************************************
	void CAGPTextureMemManager::EndFrame()
	{
	}

	//******************************************************************************************
	void CAGPTextureMemManager::MakeCurrentList()
	{
		TRasD3DList::iterator it;

		//
		// Move inactive textures in the active list to the inactive list.
		//
		it = rlActive.begin();
		while (it != rlActive.end())
		{
			Assert((*it).prasRaster);
			Assert((*it).prasRaster->prasd3dGet());

			// Move the texture if required.
			if ((*it).prasRaster->prasd3dGet()->u4Age() > iRetireAge)
			{
				// Keep the current iterator valid.
				TRasD3DList::iterator it_move = it;
				++it;

				// Add the item.
				rlUnused.push_back(CRasterWrap((*it_move).prasRaster));

				// Remove the item from the active list.
				rlActive.erase(it_move);

			}
			else
			{
				++it;
			}
		}

		//
		// Move active textures in the inactive list to the active list.
		//
		it = rlUnused.begin();
		while (it != rlUnused.end())
		{
			Assert((*it).prasRaster);
			Assert((*it).prasRaster->prasd3dGet());

			// Move the texture if required.
			if ((*it).prasRaster->prasd3dGet()->u4Age() <= iRetireAge)
			{
				// Keep the current iterator valid.
				TRasD3DList::iterator it_move = it;
				++it;

				// Add the item.
				rlActive.push_back(CRasterWrap((*it_move).prasRaster));

				// Remove the item from the active list.
				rlUnused.erase(it_move);

			}
			else
			{
				++it;
			}
		}

		// Count active and unused textures.
		iBytesTotal = 0;
		for (it = rlActive.begin(); it != rlActive.end(); ++it)
			iBytesTotal += (*it).prasRaster->iSurfaceMemBytes();
		for (it = rlUnused.begin(); it != rlUnused.end(); ++it)
			iBytesTotal += (*it).prasRaster->iSurfaceMemBytes();

		// If texture memory is above the high water mark, reduce it.
		while (iBytesTotal > iBytesHighWaterMark)
		{
			if (!priv_self.bReducedUnused())
				break;
		}
		//PrintD3D2("***Total: %ld\n", iBytesTotal >> 10);

		// Increment the frame count mechanism.
		++CRasterD3D::u4FrameCount;
	}

	//******************************************************************************************
	void CAGPTextureMemManager::AllocateMemory(int i_bytes_allocated)
	{
		Assert(pwWorld);

		Purge();

		iBytesAllocated     = i_bytes_allocated;
		iBytesLowWaterMark  = (i_bytes_allocated * 85) / 100;
		iBytesHighWaterMark = (i_bytes_allocated * 95) / 100;

		// If the amount is not enough, turn off allocations.
		if (!bIsActive())
		{
			iBytesAllocated     = 0;
			iBytesLowWaterMark  = 0;
			iBytesHighWaterMark = 0;
		}

	#if 0
		dprintf("\nD3D Free Texture memory available   : %ld Kb\n", iBytesAllocated >> 10);
		dprintf("Non-pageable textures allocated     : %ld Kb\n", iTotalNonPageable >> 10);
	#endif // bDEBUG_DIRECT3D
	}
	
	//******************************************************************************************
	void CAGPTextureMemManager::RestoreNonpageable()
	{
		// Do nothing if the system is not in use.
		if (!bIsActive())
			return;
		AlwaysAssert(d3dDriver.bUseD3D());

		//
		// Preload lowest mip levels.
		//
	#if bHIGHER_LEVEL
		CRenderDB::SetNoPageFrames(2);
	#endif // bHIGHE
		priv_self.LoadNonPageable(pwWorld->ppartPartitionList());
		priv_self.LoadNonPageable(pwWorld->ppartBackdropsList());

	#if bDEBUG_DIRECT3D
		dprintf("\nNon-pageable textures allocated     : %ld Kb\n\n", iTotalNonPageable >> 10);
	#endif // bDEBUG_DIRECT3D
	}

	//******************************************************************************************
	bool CAGPTextureMemManager::bIsActive()
	{
		// There must be at least quarter a megabyte of texture memory available.
		return iBytesLowWaterMark >= 256 * 1024;
	}

	//******************************************************************************************
	void CAGPTextureMemManager::Purge()
	{
		TRasD3DList::iterator it;

		BreakAllTextureLinks();

		// Purge non-pageable textures.
		iTotalNonPageable = 0;

		// Iterate throught the active and unused rasters and remove as required.
		for (it = rlActive.begin(); it != rlActive.end(); ++it)
			(*it).prasRaster->Link();
		rlActive.erase(rlActive.begin(), rlActive.end());
		for (it = rlUnused.begin(); it != rlUnused.end(); ++it)
			(*it).prasRaster->Link();
		rlUnused.erase(rlUnused.begin(), rlUnused.end());
		poolD3DRaster.FlushAll();

		// Reset all variables.
		iBytesTotal         = 0;
		iBytesInUse         = 0;
		iBytesAllocated     = 0;
		iBytesLowWaterMark  = 0;
		iBytesHighWaterMark = 0;
	}
	
	//*****************************************************************************************
	void CAGPTextureMemManager::FlushUnused()
	{
		priv_self.bReducedUnused(Min(10, rlUnused.size()));
	}

	//******************************************************************************************
	rptr<CRaster> CAGPTextureMemManager::prasCreate(int i_width, int i_height, ED3DTextureType ed3dtex)
	{
		// If texture memory is above the high water mark, reduce it.
		while (iBytesTotal > iBytesHighWaterMark)
		{
			if (!priv_self.bReducedUnused())
				break;
		}
		return poolD3DRaster.prasCreate(i_width, i_height, ed3dtex);
	}

	//******************************************************************************************
	void CAGPTextureMemManager::Delete(rptr<CRaster>& rpras)
	{
		poolD3DRaster.Delete(rpras);
	}

	//******************************************************************************************
	bool CAGPTextureMemManager::bAdd(rptr<CRaster>& rpras)
	{
		// Fail if no raster is present.
		if (!rpras)
			return false;

		// Attempt to upload the texture.
		if (!rpras->bUpload())
		{
			rpras = rptr0;
			return false;
		}

		// Add the raster to the manager.
		priv_self.SetAlloc(rpras);
		rlActive.push_back(CRasterWrap(rpras));

		return true;
	}
	
	//*****************************************************************************************
	void CAGPTextureMemManager::OutputAllocations() const
	{
	#if bDEBUG_DIRECT3D

		dprintf("Full texture memory available : %ld Kb\n", iBytesAllocated >> 10);
		dprintf("Non-pageable textures         : %ld Kb\n", iTotalNonPageable >> 10);

	#if iTRACK_D3D_MEM_USE
		dprintf("Maximum full allocated        : %ld Kb\n", iMaxBytesAllocated >> 10);
		dprintf("Maximum full used per frame   : %ld Kb\n", iMaxBytesUsedPerFrame >> 10);
	#endif // iTRACK_D3D_MEM_USE
		
	#endif // bDEBUG_DIRECT3D
	}


//**********************************************************************************************
//
// CAGPTextureMemManager::CPriv Implementation.
//

	//******************************************************************************************
	bool CAGPTextureMemManager::CPriv::bReducedUnused(int i_num_reduce)
	{
		TRasD3DList::iterator it;

		// Determine if a reduction is possible.
		if (rlUnused.size() < 1)
		{
		#if 0
			dprintf("Unable to reduce unused textures!\n");
		#endif // bDEBUG_DIRECT3D
			return false;
		}

	#if 0
		dprintf("Reducing unused textures!\n");
	#endif // bDEBUG_DIRECT3D

		// Delete the entire list if that is all that remains.
		if (i_num_reduce > rlUnused.size())
		{
			for (it = rlUnused.begin(); it != rlUnused.end(); ++it)
			{
				rptr<CRaster> pras = (*it).prasRaster;
				priv_self.SetAlloc(pras, -1);
				pras->Link();
			}
			rlUnused.erase(rlUnused.begin(), rlUnused.end());
			return true;
		}

		// Remove the first n elements.
		for (it = rlUnused.begin();;)
		{
			rptr<CRaster> pras = (*it).prasRaster;
			priv_self.SetAlloc(pras, -1);
			pras->Link();
			
			if (--i_num_reduce < 1)
				break;
			++it;
			Assert(it != rlUnused.end());
		}
		rlUnused.erase(rlUnused.begin(), it);

		// Some have been deleted at least.
		return true;
	}

	//******************************************************************************************
	void CAGPTextureMemManager::CPriv::SetAlloc(rptr<CRaster> pras, int i_sign)
	{
		Assert(i_sign == 1 || i_sign == -1);

		int i_alloc_size = pras->iSurfaceMemBytes() * i_sign;
		iBytesTotal += i_alloc_size;
		iBytesInUse += i_alloc_size;
		
	#if iTRACK_D3D_MEM_USE
		// Record maximum memory use.
		iMaxBytesAllocated = Max(iMaxBytesAllocated, iBytesTotal);
	#endif // iTRACK_D3D_MEM_USE
	}

	//******************************************************************************************
	void CAGPTextureMemManager::CPriv::LoadNonPageable(CPartition* ppart)
	{
		Assert(ppart);

		//
		// If this partition has a mesh that is a candidate for hardware, preload the lowest
		// mip levels.
		//
		if (ppart->pdGetData().bHardwareAble)
		{
			// Load the mesh.
			LoadNonPageable(ppart->pmshGetMesh());
		}

		//
		// Recursively call this function on children.
		//
		{
			CPartition* ppartc = (CPartition*)ppart->ppartChildren();
			if (ppartc)
				for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
					LoadNonPageable(*it);
		}
	}

	//******************************************************************************************
	void CAGPTextureMemManager::CPriv::LoadNonPageable(rptr<CTexture> ptex)
	{
		if (!ptex)
			return;

		// Ignore if this surface has no rasters.
		if (ptex->iGetNumMipLevels() <= 0)
			return;
		
		// Find the largest non-pageable raster.
		int i_nopage;
		for (i_nopage = 0; i_nopage < ptex->iGetNumMipLevels(); ++i_nopage)
		{
			// Determine if the raster is non-pageable.
			rptr<CRaster> pras = ptex->prasGetTexture(i_nopage);
			if ((pras->iWidth < 32 && pras->iHeight < 32) || (pras->iWidth < 16 || pras->iHeight < 16))
				break;
		}

		// If no suitable mip level is available, find the smallest mip level and load it.
		if (i_nopage >= ptex->iGetNumMipLevels())
			i_nopage = ptex->iGetNumMipLevels() - 1;

		// If no suitable mip level is yet available, blow this polygon off.
		if (i_nopage < 0 || i_nopage >= ptex->iGetNumMipLevels())
			return;
		
	#if bHIGHER_LEVEL
		if (i_nopage > 1)
			--i_nopage;
			//i_nopage = 1;
	#endif // bHIGHER_LEVEL

		// Get the candidate raster.
		rptr<CRaster> pras = ptex->prasGetTexture(i_nopage);

		// If the polygon already has a link, forget about it.
		if (pras->prasLink)
			return;

		// If the mip level is not available, load it.
		while (!gtxmTexMan.bIsAvailable(pras->pSurface, pras->iByteSpan()))
		{
		#if bHIGHER_LEVEL
			++i_nopage;
			AlwaysAssert(i_nopage >= 0 && i_nopage < ptex->iGetNumMipLevels());
			pras = ptex->prasGetTexture(i_nopage);
		#else
			return;
		#endif // bHIGHER_LEVEL
		}

		// Get the D3D texture type.
		ED3DTextureType ed3dt;

		if (ptex->seterfFeatures[erfALPHA_COLOUR])
		{
			ed3dt = ed3dtexSCREEN_ALPHA;
		}
		else if (ptex->seterfFeatures[erfTRANSPARENT])
		{
			ed3dt = ed3dtexSCREEN_TRANSPARENT;
		}
		else
		{
			ed3dt = ed3dtexSCREEN_OPAQUE;
		}

		// Create a linked D3D raster.
	#if bTRACK_D3D_RASTERS
		TrackSystem(etrPre);
	#endif // bTRACK_D3D_RASTERS

		rptr<CRaster> prasd3d = rptr_cast(CRaster, rptr_new CRasterD3D
		(
			CRasterD3D::CInitDiscreetOpaque
			(
				pras->iWidth,
				pras->iHeight,
				ed3dt
			)
		));

		// Load and link the newly created Direct3D raster.
		ConvertRaster(prasd3d->prasd3dGet(), pras, CColour(ptex->d3dpixColour, 0));
		if (prasd3d->bUpload())
		{
			AlwaysAssert(prasd3d->pd3dtexGet());
			pras->Link(prasd3d);
			iTotalNonPageable += prasd3d->iSurfaceMemBytes();
		}
		else
		{
		#if bDEBUG_DIRECT3D
			dprintf("D3D texture upload failed!\n");
		#endif // bDEBUG_DIRECT3D
		}

		//
		// Determine if the highest resolution raster is large.
		//
		if (d3dDriver.bTexCreatesAreSlow())
		{
			pras = ptex->prasGetTexture(0);
			if (pras->iWidth > 128 || pras->iHeight > 128)
			{
				ptex->bLargeSizes = true;
			}
		}
	}

	//******************************************************************************************
	void CAGPTextureMemManager::CPriv::LoadNonPageable(rptr<CMesh> pmsh)
	{
		Assert(pmsh);

		// Convenient place to do this.
		SetTilingFlags(pmsh);

		// Iterate through the list of polygons.
		for (uint u = 0; u < pmsh->pasfSurfaces.uLen; ++u)
			LoadNonPageable(pmsh->pasfSurfaces[u].ptexTexture);
		if (pmsh->bIsAnimated())
		{
			// Force tiling on.
			uint u;
			for (u = 0; u < pmsh->pasfSurfaces.uLen; ++u)
				pmsh->pasfSurfaces[u].ptexTexture->seterfFeatures += erfTILE_UV;

			rptr<CMeshAnimating> pmshanim = rptr_static_cast(CMeshAnimating, pmsh);
			for (u = 0; u < pmshanim->aptexTextures.uLen; ++u)
			{
				LoadNonPageable(pmshanim->aptexTextures[u]);
				pmshanim->aptexTextures[u]->seterfFeatures += erfTILE_UV;
			}
		}

		// Do not recurse if there are no more detailed reduced meshes.
		if (!pmsh->pshCoarser)
			return;
		
		pmsh = rptr_static_cast(CMesh, pmsh->pshCoarser);
		LoadNonPageable(pmsh);
	}

	//******************************************************************************************
	void CAGPTextureMemManager::CPriv::SetTilingFlags(rptr<CMesh> pmsh)
	{
		Assert(pmsh);

		// Iterate through the list of polygons.
		for (uint u = 0; u < pmsh->pampPolygons.uLen; ++u)
		{
			CMesh::SPolygon& poly = pmsh->pampPolygons[u];

			// Ignore the polygon if it has no surface.
			if (!poly.pSurface)
				continue;
			if (!poly.pSurface->ptexTexture)
				continue;

			// Ignore the polygon if it is used for occlusion.
			if (poly.pSurface->ptexTexture->seterfFeatures[erfOCCLUDE])
				continue;

			//
			// Go through each vertex of the polygon looking for out of range vertices.
			//
			// To do:
			//		Make sure it is not the case that Trespasser's 'U' is Direct3D's 'V,'
			//		and visa-versa.
			//
			bool b_tile_u = false;
			bool b_tile_v = false;
			for (int i_vert = 0; i_vert < poly.papmvVertices.uLen; ++i_vert)
			{
				CMesh::SVertex& vert = *poly.papmvVertices[i_vert];
				if (!b_tile_u && !bWithin(vert.tcTex.tX, -0.0025f, 1.0025f))
				{
					b_tile_u = true;
				}
				if (!b_tile_v && !bWithin(vert.tcTex.tY, -0.0025f, 1.0025f))
				{
					b_tile_v = true;
				}
			}

			//
			// Set the tiling flags.
			//
			// To do:
			//		Using separate wrapping behaviour for U and V should depend on a caps bit
			//		or a test.
			//
			/*
			if (b_tile_u)
				poly.pSurface->ptexTexture->seterfFeatures += erfTILE_U;
			if (b_tile_v)
				poly.pSurface->ptexTexture->seterfFeatures += erfTILE_V;
			if (b_tile_u && b_tile_v)
				poly.pSurface->ptexTexture->seterfFeatures += erfTILE_UV;
			*/
			if (b_tile_u || b_tile_v)
				poly.pSurface->ptexTexture->seterfFeatures += erfTILE_UV;
		}
	}


//
// Global variables.
//
CAGPTextureMemManager agpAGPTextureMemManager;

#if iTRACK_D3D_MEM_USE
	int CAGPTextureMemManager::iMaxBytesAllocated    = 0;
	int CAGPTextureMemManager::iMaxBytesUsedPerFrame = 0;
#endif // iTRACK_D3D_MEM_USE