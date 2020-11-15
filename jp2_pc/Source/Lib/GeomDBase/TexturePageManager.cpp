/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of TexturePageManager.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/TexturePageManager.cpp                                  $
 * 
 * 31    10/07/98 3:13a Pkeet
 * Fixed crash bugs.
 * 
 * 30    8/25/98 8:39p Rvande
 * Loop variables re-scoped
 * 
 * 29    98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 28    8/06/98 5:21p Mlange
 * Added extra checks for outstanding references in FreePages().
 * 
 * 27    7/27/98 12:53p Pkeet
 * Added a separate initialization class for page managed textures.
 * 
 * 26    7/27/98 12:29p Pkeet
 * Added an initialization class for 'CRasterD3D.'
 * 
 * 25    7/23/98 9:08p Pkeet
 * Added code to track the creation and deletion of Direct3D textures using a macro switch in
 * 'RasterD3D.hpp.'
 * 
 * 24    7/23/98 6:28p Mlange
 * Can now be used to manage mem rasters.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "TexturePageManager.hpp"

#include "Lib/View/RasterD3D.hpp"
#include "Lib/Transform/Vector.hpp"
#include "Lib/Loader/TexturePackSurface.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/Sys/W95/Render.hpp"


static CProfileStat psUploadTexures("Tex pg upload", &proProfile.psRenderShape, Set(epfSEPARATE));

static const int iMIN_REGION_SIZE = 8;

//**********************************************************************************************
//
class CPage : public CPackedRaster
//
// A single managed page.
//
// Prefix: pg
//
//**************************************
{
	friend class CTexturePageManager;
	friend class CTexturePageManager::CRegionHandle;

	bool bModified;

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Construct D3D raster.
	CPage(int i_size, ED3DTextureType ed3d)
		: CPackedRaster(((const IDirectDraw4*)DirectDraw::pdd4 != NULL) ? rptr_cast(CRaster, rptr_new CRasterD3D(CRasterD3D::CInitPage(i_size, i_size, ed3d))) :
		                                   rptr_cast(CRaster, rptr_new CRasterMem(i_size, i_size, 16, i_size * 2, &prasMainScreen->pxf))),
		  bModified(true)
	{
	}

	// Construct mem raster.
	CPage(int i_size)
		: CPackedRaster(rptr_cast(CRaster, rptr_new CRasterMem(i_size, i_size, 16, i_size * 2, &prasMainScreen->pxf))),
		  bModified(true)
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	int iGetSize() const
	//
	// Returns:
	//		The size along one axis for this (square) page.
	//
	//**************************************
	{
		return prasPackedParent->iWidth;
	}


	//******************************************************************************************
	//
	int iNumFreeRegions
	(
		int i_size_log2
	) const
	//
	// Returns:
	//		The number of free regions of the specified size, in this page.
	//
	//**************************************
	{
		Assert(i_size_log2 >= 0 && i_size_log2 <= CTexturePageManager::iMAX_PAGE_SIZE_LOG2);

		if ((1 << i_size_log2) > iGetSize())
			return 0;

		return tqtPackQuadTree.anltFree[i_size_log2].u1Count;
	}


	//******************************************************************************************
	//
	bool bReserveRegion
	(
		CVector2<int>* pv2_origin,	// Vector to initialise with the origin of the allocated region.
		const CVector2<int>& v2_dim	// Dimensions of the region to allocate.
	);
	//
	// Reserve a region in this page.
	//
	// Returns:
	//		'true' if successful.
	//
	// Notes:
	//		The given dimensions must be within the valid range.
	//
	//**************************************


	//******************************************************************************************
	//
	bool bReserveRegion
	(
		const CVector2<int>& rv2_origin,// Vector specifying the origin of the region to allocate.
		const CVector2<int>& v2_dim		// Dimensions of the region to allocate.
	);
	//
	// Reserve a region in this page at a specified location.
	//
	// Returns:
	//		'true' if successful.
	//
	// Notes:
	//		The given dimensions must be within the valid range.
	//
	//**************************************


	//******************************************************************************************
	//
	void FreeRegion
	(
		const CVector2<int>& v2_origin	// Origin of the region.
	);
	//
	// De-commit a previously reserved a region in this page.
	//
	//**************************************
};



//**********************************************************************************************
//
// CTexturePageManager implementation.
//

	// Private member data.
	struct CTexturePageManager::SThisData
	{
		ED3DTextureType ed3dTextureType;

		int iMaxPageSize;			// Size of the largest page.
		int iMinPageSize;			// Size of the smallest page.
		int iBytesPerPixel;			// Size of a pixel in a page's raster.
		int iAllocBytes;			// Total number of bytes currently allocated from the pages.

		CPArray< rptr<CPage> > pappgPages;
									// The pages, sorted from largest to smallest size.

		CMSArray<int, iMAX_PAGE_SIZE_RANGE + 1> msaiPageCounts;
									// Counts for the number of pages of each specific size, starting with
									// the largest.
	};


	//******************************************************************************************
	CTexturePageManager::CTexturePageManager()
		: this_d(new SThisData)
	{
		this_d->ed3dTextureType = ed3dtexSCREEN_OPAQUE;
		this_d->iAllocBytes     = 0;
		this_d->iBytesPerPixel  = 0;
	}

	//******************************************************************************************
	CTexturePageManager::CTexturePageManager(int i_mem_size, int i_page_size_range, float f_page_count_factor, int i_max_page_size, bool b_d3d_rasters)
		: this_d(new SThisData)
	{
		this_d->ed3dTextureType = ed3dtexSCREEN_OPAQUE;
		this_d->iAllocBytes     = 0;
		this_d->iBytesPerPixel  = 0;

		if (b_d3d_rasters)
			AllocPagesD3D(i_mem_size, i_page_size_range, f_page_count_factor, i_max_page_size);
		else
			AllocPages(i_mem_size, i_page_size_range, f_page_count_factor, i_max_page_size, false);
	}

	//******************************************************************************************
	CTexturePageManager::~CTexturePageManager()
	{
		FreePages();

		delete this_d;
	}


	//******************************************************************************************
	int CTexturePageManager::iGetManagedMemUsed() const
	{
		return this_d->iAllocBytes;
	}


	//******************************************************************************************
	int CTexturePageManager::iGetManagedMemSize() const
	{
		int i_total_mem = 0;

		for (int i_page = 0; i_page < this_d->pappgPages.uLen; i_page++)
			i_total_mem += Sqr(this_d->pappgPages[i_page]->iGetSize()) * this_d->iBytesPerPixel;

		return i_total_mem;
	}


	//******************************************************************************************
	void CTexturePageManager::AllocPagesD3D(int i_mem_size, int i_page_size_range, float f_page_count_factor,
		                                    int i_max_page_size, ED3DTextureType ed3d)
	{
		// Set the page type.
		this_d->ed3dTextureType = ed3d;

		AllocPages(i_mem_size, i_page_size_range, f_page_count_factor, i_max_page_size, d3dDriver.bUseD3D());
	}


	//******************************************************************************************
	void CTexturePageManager::AllocPages(int i_mem_size, int i_page_size_range, float f_page_count_factor, int i_max_page_size, bool b_d3d_rasters)
	{
		Assert(i_mem_size > 0 && i_page_size_range >= 0 && i_page_size_range <= iMAX_PAGE_SIZE_RANGE && i_max_page_size > 0);
		Assert(f_page_count_factor >= 1 && f_page_count_factor <= 4);

		// Free current texture pages, if any.
		FreePages();

		this_d->iMaxPageSize = i_max_page_size;
		this_d->iMinPageSize = Max(i_max_page_size >> i_page_size_range, iMIN_PAGE_SIZE);

		// Clamp the default page sizes by the allowed texture sizes.
		if (d3dDriver.bUseD3D() && b_d3d_rasters)
		{
			d3dDriver.SetTextureMinMaxSquare(this_d->iMaxPageSize);
			d3dDriver.SetTextureMinMaxSquare(this_d->iMinPageSize);
		}

		// Now, having clamped the page sizes, determine the actual page size range.
		i_page_size_range = uLog2(this_d->iMaxPageSize) - uLog2(this_d->iMinPageSize);

		// Allocate a dummy page, to determine the pixel size.
	#if bTRACK_D3D_RASTERS
		SetupTrack();
	#endif
		CPage* ppg_temp = b_d3d_rasters ? new CPage(this_d->iMaxPageSize, this_d->ed3dTextureType) : new CPage(this_d->iMaxPageSize);
		this_d->iBytesPerPixel = ppg_temp->prasGetParentRaster()->iPixelBytes();
		delete ppg_temp;


		float f_mem_req_factor;
		
		if (f_page_count_factor != 4)
			f_mem_req_factor = (1 - pow(f_page_count_factor / 4, float(i_page_size_range + 1))) /
			                   (1 - f_page_count_factor / 4);
		else
			f_mem_req_factor = i_page_size_range;

		// Determine the memory taken by a page of the largest size.
		int i_mem_pages_0 = Sqr(this_d->iMaxPageSize) * this_d->iBytesPerPixel;

		float f_num_pages_0 = float(i_mem_size) / (i_mem_pages_0 * f_mem_req_factor);


		int i_total_pages = 0;

		float f_mem_exceeded_prev_level = 0;
		int i_curr_page_mem = i_mem_pages_0;

		int i_page_counts;
		for (i_page_counts = 0; i_page_counts <= i_page_size_range; i_page_counts++)
		{
			// Determine the number of pages to allocate of the current size.
			float f_num_pages = f_num_pages_0 * pow(f_page_count_factor, i_page_counts);

			// Adjust the page count according to the page memory budget deviation for the previous page size.
			f_num_pages -= f_mem_exceeded_prev_level / float(i_curr_page_mem);

			this_d->msaiPageCounts << iRound(f_num_pages);

			i_total_pages += this_d->msaiPageCounts(-1);

			// Because the actual page count was rounded to the nearest integer value, we likely have not used
			// the exact amount of memory reserved for the current page size.
			f_mem_exceeded_prev_level = (iRound(f_num_pages) - f_num_pages) * i_curr_page_mem;

			i_curr_page_mem >>= 2;
		}


		// If we exceeded the amount of memory that we're supposed to allocate, remove one of the smallest pages.
		if (f_mem_exceeded_prev_level > 0)
		{
			Assert(this_d->msaiPageCounts[i_page_size_range] != 0);

			this_d->msaiPageCounts[i_page_size_range] -= 1;
			i_total_pages -= 1;
		}


		//
		// Allocate the pages.
		//
		this_d->pappgPages = CPArray< rptr<CPage> >(i_total_pages);

		int i_curr_page_size = this_d->iMaxPageSize;

		int i_page = 0;
		for (i_page_counts = 0; i_page_counts <= i_page_size_range; i_page_counts++)
		{
			int i_pages = this_d->msaiPageCounts[i_page_counts];
			for (int i = 0; i < i_pages; i++)
			{
			#if bTRACK_D3D_RASTERS
				SetupTrack();
			#endif
				// Allocate a page and attempt to upload it.
				rptr<CPage> ppg_new = b_d3d_rasters ? rptr_new CPage(i_curr_page_size, this_d->ed3dTextureType) : rptr_new CPage(i_curr_page_size);

				if (ppg_new->prasGetParentRaster()->bUpload())
				{
					ppg_new->bModified = false;

					this_d->pappgPages[i_page++] = ppg_new;
				}
				else
				{
					// Upload failed. Do not add this page to the list of managed pages.
					ppg_new = rptr0;

					this_d->msaiPageCounts[i_page_counts] -= 1;

					if (i_page_counts == i_page_size_range && i_page_size_range < iMAX_PAGE_SIZE_RANGE)
					{
						this_d->msaiPageCounts << 0;
						i_page_size_range++;
					}

					// Attempt to request an extra page of the next size down, to replace the one that failed to upload.
					if (i_page_counts < i_page_size_range)
						this_d->msaiPageCounts[i_page_counts + 1] += 1;
					else
						this_d->pappgPages.uLen--;
				}
			}

			i_curr_page_size >>= 1;
		}

		Assert(i_page == this_d->pappgPages.uLen);
		Assert(iGetManagedMemSize() <= i_mem_size);
	}


	//******************************************************************************************
	void CTexturePageManager::FreePages()
	{
		// At this point, there should be no outstanding references to the texture pages.
		#if VER_DEBUG
		{
			for (int i = 0; i < this_d->pappgPages.uLen; i++)
			{
				Assert(this_d->pappgPages[i]->uNumRefs() == 1);
				Assert(this_d->pappgPages[i]->prasGetParentRaster()->uNumRefs() == 2);
			}
		}
		#endif


		// Delete the pages.
		for (int i = 0; i < this_d->pappgPages.uLen; i++)
			this_d->pappgPages[i] = rptr0;

		delete[] this_d->pappgPages.atArray;

		this_d->pappgPages.uLen    = 0;
		this_d->pappgPages.atArray = 0;


		// Reset the page count array.
		this_d->msaiPageCounts.Reset();


		this_d->iAllocBytes    = 0;
		this_d->iBytesPerPixel = 0;
	}


	//******************************************************************************************
	int CTexturePageManager::iGetNumPages() const
	{
		return this_d->pappgPages.uLen;
	}

	//******************************************************************************************
	int CTexturePageManager::iGetMinRegionSize() const
	{
		Assert(this_d->pappgPages.uLen != 0);
		return iMIN_REGION_SIZE;
	}

	//******************************************************************************************
	int CTexturePageManager::iGetMaxRegionSize() const
	{
		Assert(this_d->pappgPages.uLen != 0);
		return this_d->iMaxPageSize;
	}

	//******************************************************************************************
	void CTexturePageManager::ClampRegionSize(CVector2<int>& rv2_dim) const
	{
		Assert(this_d->pappgPages.uLen != 0);
		Assert(rv2_dim.tX >= 0 && bPowerOfTwo(rv2_dim.tX));
		Assert(rv2_dim.tY >= 0 && bPowerOfTwo(rv2_dim.tY));

		// Clamp the dimension to be within range.
		SetMinMax(rv2_dim.tX, iMIN_REGION_SIZE, this_d->iMaxPageSize);
		SetMinMax(rv2_dim.tY, iMIN_REGION_SIZE, this_d->iMaxPageSize);
	}


	//******************************************************************************************
	int CTexturePageManager::iGetPageSize(int i_page) const
	{
		Assert(this_d->pappgPages.uLen != 0);
		return this_d->pappgPages[i_page]->iGetSize();
	}


	//******************************************************************************************
	rptr<CRaster> CTexturePageManager::prasGetPage(int i_page) const
	{
		Assert(this_d->pappgPages.uLen != 0);
		return this_d->pappgPages[i_page]->prasGetParentRaster();
	}


	//******************************************************************************************
	void CTexturePageManager::AllocateRegion(CRegionHandle& rrh_init, const CVector2<int>& v2_dim, bool b_modified)
	{
		Assert(!rrh_init);

		// Do nothing if there no allocated pages.
		if (this_d->pappgPages.uLen == 0)
			return;

		CVector2<int> v2_dim_clamp = v2_dim;
		ClampRegionSize(v2_dim_clamp);

		// Find the index of the last page in the array that is large enough to allocate the region from.
		int i_page_count_index = uLog2(this_d->iMaxPageSize) - uLog2(Max(v2_dim_clamp.tX, v2_dim_clamp.tY));
		SetMin(i_page_count_index, this_d->msaiPageCounts.uLen - 1);

		int i_start_page_index = 0;
		for ( ; i_page_count_index >= 0; i_page_count_index--)
			i_start_page_index += this_d->msaiPageCounts[i_page_count_index];

		i_start_page_index--;

		// From this page, iterate in reverse order through the pages until we successfully reserve a region or
		// until we run out of pages.
		CVector2<int> v2_origin;


		// First, attempt the allocation only from those pages that have a free list entry whose dimension
		// closely matches the minor axis' dimension of the allocation. This reduces memory fragmentation.
		int i_page     = -1;
		int i_free_dim = uLog2(Min(v2_dim_clamp.tX, v2_dim_clamp.tY));

		do
		{
			for (i_page = i_start_page_index; i_page >= 0; i_page--)
			{
				if (this_d->pappgPages[i_page]->iNumFreeRegions(i_free_dim) > 0)
				{
					if (this_d->pappgPages[i_page]->bReserveRegion(&v2_origin, v2_dim_clamp))
						break;
				}
			}

			// If we could not find a page to allocate from, search the pages again, this time with a larger
			// free list entry dimension.
			i_free_dim++;
		}
		while (i_page < 0 && i_free_dim <= iMAX_PAGE_SIZE_LOG2);

		// If the allocation failed, we do a last allocation attempt without regard of any free list entries,
		// to catch a possible allocation that we missed in the first pass due to 'lost' free list entries.
		if (i_page < 0)
		{
			for (i_page = i_start_page_index; i_page >= 0; i_page--)
				if (this_d->pappgPages[i_page]->bReserveRegion(&v2_origin, v2_dim_clamp))
					break;
		}


		if (i_page >= 0)
		{
			this_d->pappgPages[i_page]->bModified = this_d->pappgPages[i_page]->bModified || b_modified;

			new(&rrh_init) CRegionHandle(rptr_this(this), i_page, v2_origin, v2_dim_clamp);

			this_d->iAllocBytes += v2_dim_clamp.tX * v2_dim_clamp.tY * this_d->iBytesPerPixel;
		}
	}


	//******************************************************************************************
	void CTexturePageManager::AllocateRegion(CRegionHandle& rrh_init, int i_page, const CVector2<int>& v2_origin, const CVector2<int>& v2_dim, bool b_modified)
	{
		Assert(!rrh_init);

		// Do nothing if there no allocated pages.
		if (this_d->pappgPages.uLen == 0)
			return;

		CVector2<int> v2_dim_clamp = v2_dim;
		ClampRegionSize(v2_dim_clamp);

		// Attempt to allocate from the given page.
		if (this_d->pappgPages[i_page]->bReserveRegion(v2_origin, v2_dim_clamp))
		{
			this_d->pappgPages[i_page]->bModified = this_d->pappgPages[i_page]->bModified || b_modified;

			new(&rrh_init) CRegionHandle(rptr_this(this), i_page, v2_origin, v2_dim_clamp);

			this_d->iAllocBytes += v2_dim_clamp.tX * v2_dim_clamp.tY * this_d->iBytesPerPixel;
		}
	}


	//******************************************************************************************
	void CTexturePageManager::DeleteRegion(CRegionHandle& rrh)
	{
		if (!this_d)
			return;

		// Do nothing if this doesn't refer to a valid allocation.
		if(!rrh)
			return;

		Assert(rrh.ptexmOwner.ptGet() == this);

		this_d->iAllocBytes -= rrh.v2Size.tX * rrh.v2Size.tY * this_d->iBytesPerPixel;

		this_d->pappgPages[rrh.iPage]->FreeRegion(rrh.v2Origin);

		// Invalidate this handle.
		rrh.ptexmOwner = rptr0;
	}


	//******************************************************************************************
	bool CTexturePageManager::bUploadRegion(const CRegionHandle& rh) const
	{
		// Do nothing if this doesn't refer to a valid allocation.
		if(!rh)
			return false;

		prasGetPage(rh.iGetPage())->Unlock();

		return prasGetPage(rh.iGetPage())->bUpload
		(
			rh.v2GetOrigin().tX,
			rh.v2GetOrigin().tY,
			rh.v2GetSize().tX,
			rh.v2GetSize().tY
		);
	}

	//******************************************************************************************
	bool CTexturePageManager::bUploadPages(bool b_upload_all) const
	{
		CCycleTimer ctmr;

		int i_num_pages_uploaded = 0;
		bool b_uploads_ok = true;

		// Loop through the pages.
		for (int i_page = 0; i_page < this_d->pappgPages.uLen; i_page++)
		{
			prasGetPage(i_page)->Unlock();

			if (this_d->pappgPages[i_page]->bModified || b_upload_all)
			{
				if (prasGetPage(i_page)->bUpload())
				{
					++i_num_pages_uploaded;
					this_d->pappgPages[i_page]->bModified = false;
				}
				else
					b_uploads_ok = false;
			}
		}

		psUploadTexures.Add(ctmr(), i_num_pages_uploaded);

		return b_uploads_ok;
	}


	//******************************************************************************************
	bool CTexturePageManager::bIsModified(int i_page_num) const
	{
		Assert(this_d->pappgPages.uLen != 0);
		return this_d->pappgPages[i_page_num]->bModified;
	}


	//******************************************************************************************
	void CTexturePageManager::SetModified(int i_page_num, bool b_modified)
	{
		Assert(this_d->pappgPages.uLen != 0);
		this_d->pappgPages[i_page_num]->bModified = b_modified;
	}


	//******************************************************************************************
	void CTexturePageManager::PrintStats(CConsoleBuffer& con) const
	{
		con.Print("Texture pages: ");

		int i_page_size = this_d->iMaxPageSize;
		int i;
		for (i = 0; i < this_d->msaiPageCounts.uLen; i++)
		{
			con.Print("%3d^2 ", i_page_size, i_page_size);
			i_page_size >>= 1;
		}

		con.Print("\n Count       : ");

		for (i = 0; i < this_d->msaiPageCounts.uLen; i++)
		{
			con.Print(" %3d  ", this_d->msaiPageCounts[i]);
		}

		int i_total_mem = iGetManagedMemSize();

		con.Print
		(
			"\nMem commit: %dKB, free: %dKB, reserved: %dKB\n",
			(this_d->iAllocBytes + 512) / 1024,
			((i_total_mem - this_d->iAllocBytes) + 512) / 1024,
			(i_total_mem + 512) / 1024
		);
	}


//**********************************************************************************************
//
// CTexturePageManager::CRegionHandle implementation.
//


	//******************************************************************************************
	CTransLinear2<> CTexturePageManager::CRegionHandle::tlr2ToPage() const
	{
		Assert(ptexmOwner != 0);

		float f_inv_size = 1.0f / ptexmOwner->this_d->pappgPages[iPage]->iGetSize();

		return CTransLinear2<>
		(
			CTransLinear<>(v2Size.tX * f_inv_size, v2Origin.tX * f_inv_size),
			CTransLinear<>(v2Size.tY * f_inv_size, v2Origin.tY * f_inv_size)
		);
	}


//**********************************************************************************************
//
// CPage implementation.
//

	//******************************************************************************************
	bool CPage::bReserveRegion(CVector2<int>* pv2_origin, const CVector2<int>& v2_dim)
	{
		STextureQuadNode* ptqn = ptqnAddRaster(v2_dim.tX, v2_dim.tY);

		if (ptqn != 0)
		{
			pv2_origin->tX = ptqn->u1XOrg;
			pv2_origin->tY = ptqn->u1YOrg;

			return true;
		}
		else
			return false;
	}


	//******************************************************************************************
	bool CPage::bReserveRegion(const CVector2<int>& v2_origin, const CVector2<int>& v2_dim)
	{
		STextureQuadNode* ptqn = ptqnAddTextureAtPosition(v2_dim.tX, v2_dim.tY, v2_origin.tX, v2_origin.tY);

		return ptqn != 0;
	}


	//******************************************************************************************
	void CPage::FreeRegion(const CVector2<int>& v2_origin)
	{
		prasGetParentRaster()->Lock();
		Delete(prasGetParentRaster()->pAddress(v2_origin.tX, v2_origin.tY));
		prasGetParentRaster()->Unlock();
	}


