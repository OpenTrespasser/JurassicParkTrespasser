/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		Texture page manager for square, power of two texture pages.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/TexturePageManager.hpp                                  $
 * 
 * 18    9/20/98 5:50p Pkeet
 * Added the 'SetToNull' member function to the region handle object.
 * 
 * 17    8/16/98 8:35p Mmouni
 * Fixed VC6 warning.
 * 
 * 16    7/23/98 6:28p Mlange
 * Can now be used to manage mem rasters.
 * 
 * 15    6/10/98 6:51p Mlange
 * Added iGetMaxRegionSize().
 * 
 * 14    5/20/98 2:55p Pkeet
 * Added a parameter to set the Direct3D raster type to the 'AllocPages' call. Added a data
 * member to the page manager to store the Direct3D raster type.
 * 
 * 13    5/14/98 11:31a Mlange
 * Texture pages are now properly reference counted. Improved the texture allocation scheme wrt
 * memory fragmentation.
 * 
 * 12    5/12/98 6:45p Mlange
 * Added support for uploading of regions.
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_TEXTUREPAGEMANAGER_HPP
#define HEADER_LIB_GEOMDBASE_TEXTUREPAGEMANAGER_HPP

#include "Lib/W95/Direct3D.hpp"
#include "Lib/Transform/TransLinear.hpp"

class CRaster;
class CConsoleBuffer;

//**********************************************************************************************
//
class CTexturePageManager : public CRefObj
//
// Manager of multiple, square texture pages of various sizes and allocations of sub-regions in
// those pages.
//
// Prefix: texm
//
// Notes:
//		Enables the use of square, power of two textures whilst allowing allocations of
//		non-square, power of two sub-regions within those textures.
//
//		The texture is in screen format!
//
//**************************************
{
	struct SThisData;
	SThisData* this_d;

public:
	class CRegionHandle;
	friend class CRegionHandle;

	//******************************************************************************************
	//
	class CRegionHandle
	//
	// Handle object for allocations of page sub-regions.
	//
	// Prefix: rh
	//
	//**************************************
	{
		friend class CTexturePageManager;

		rptr<CTexturePageManager> ptexmOwner;	// Reference to page manager, or null if this is an invalid handle.
		int iPage;								// Page number this is a region of.
		CVector2<int> v2Origin;					// Origin of this region in the page, in pixel coordinates.
		CVector2<int> v2Size;					// Size of this region, in pixel coordinates.

		//******************************************************************************************
		//
		// Constructors and destructor.
		//
	public:
		CRegionHandle()
			: ptexmOwner(rptr0)
		{
		}

		~CRegionHandle()
		{
			// Automagically delete this region if it is a valid allocation.
			SetToNull();
		}

		//******************************************************************************************
		//
		// Conversions.
		//

		// Conversion to a bool for testing the validity of this handle (e.g. whether this refers to a valid
		// allocation). Returns 'true' if valid, 'false' otherwise.
		operator bool() const
		{
			return ptexmOwner != 0;
		}


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		int iGetPage() const
		//
		// Returns:
		//		The parent page number this is a sub-region of.
		//
		//**************************************
		{
			return iPage;
		}


		//******************************************************************************************
		//
		CVector2<int> v2GetOrigin() const
		//
		// Returns:
		//		The origin of this region in the page, in pixel coordinates.
		//
		//**************************************
		{
			return v2Origin;
		}


		//******************************************************************************************
		//
		CVector2<int> v2GetSize() const
		//
		// Returns:
		//		The dimensions of this region in the page, in pixel coordinates.
		//
		//**************************************
		{
			return v2Size;
		}


		//******************************************************************************************
		//
		CTransLinear2<> tlr2ToPage() const;
		//
		// Returns:
		//		The mapping of coordinates in the region to coordinates in the page.
		//
		// Notes:
		//		This handle must be valid.
		//
		//**************************************


		//******************************************************************************************
		//
		void SetToNull
		(
		)
		//
		// Sets the owner pointer to null, effectively making the region handle invalid.
		//
		//**************************************
		{
			if (ptexmOwner)
				ptexmOwner->DeleteRegion(*this);
		}


	private:
		// Private constructor used by the texture page manager.
		CRegionHandle(rptr<CTexturePageManager> ptexm, int i_page, const CVector2<int>& v2_origin, const CVector2<int>& v2_size)
			: ptexmOwner(ptexm), iPage(i_page), v2Origin(v2_origin), v2Size(v2_size)
		{
		}


		// Not implemented.
		CRegionHandle(const CRegionHandle& rh);
		CRegionHandle& operator=(const CRegionHandle& rh);
	};


	// Maximum allowable pages sizes.
	enum
	{
		iMIN_PAGE_SIZE_LOG2 = 5,
		iMAX_PAGE_SIZE_LOG2 = 8,
		iMIN_PAGE_SIZE = 1 << iMIN_PAGE_SIZE_LOG2,
		iMAX_PAGE_SIZE = 1 << iMAX_PAGE_SIZE_LOG2,
		iMAX_PAGE_SIZE_RANGE = 3
	};


	//******************************************************************************************
	//
	// Constructors and destructor.
	//
public:
	CTexturePageManager();

	CTexturePageManager
	(
		int   i_mem_size,
		int   i_page_size_range   = 0,
		float f_page_count_factor = 3,
		int   i_max_page_size     = iMAX_PAGE_SIZE,
		bool  b_d3d_rasters       = true
	);

	~CTexturePageManager();


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	int iGetManagedMemUsed() const;
	//
	// Returns:
	//		The amount of memory allocated by this texture page manager, in bytes.
	//
	//**************************************

	//******************************************************************************************
	//
	int iGetManagedMemSize() const;
	//
	// Returns:
	//		The amount of memory managed by this texture page manager, in bytes.
	//
	//**************************************


	//******************************************************************************************
	//
	void AllocPagesD3D
	(
		int i_mem_size,					// Size of memory block to allocate, in bytes.
		int i_page_size_range = 0,		// Range of page sizes, as a shift factor of the max page
										// size.
		float f_page_count_factor = 3,	// Multiplier for the page count of the next size in the
										// range.
		int i_max_page_size = iMAX_PAGE_SIZE,
										// Dimensions of the largest page.
		ED3DTextureType ed3d = ed3dtexSCREEN_OPAQUE	// Direct3D raster type.
	);
	//
	// Allocate the texture pages, as Direct3d rasters.
	//
	// Notes:
	//		The actual amount of memory allocated is rounded down from the multiple of the smallest
	//		page. The page size is clamped to be within the allowable range as defined by Direct3D.
	//
	//		If there are currently allocated pages, they are deleted by calling FreePages().
	//
	//**************************************


	//******************************************************************************************
	//
	void AllocPages
	(
		int i_mem_size,					// Size of memory block to allocate, in bytes.
		int i_page_size_range = 0,		// Range of page sizes, as a shift factor of the max page
										// size.
		float f_page_count_factor = 3,	// Multiplier for the page count of the next size in the
										// range.
		int i_max_page_size = iMAX_PAGE_SIZE,
		bool b_d3d_rasters = true
	);
	//
	// Allocate the texture pages.
	//
	// Notes:
	//		The actual amount of memory allocated is rounded down from the multiple of the smallest
	//		page. If allocating D3D rasters, the page size is clamped to be within the allowable
	//		range as defined by Direct3D.
	//
	//		If there are currently allocated pages, they are deleted by calling FreePages().
	//
	//**************************************


	//******************************************************************************************
	//
	void FreePages();
	//
	// Free the allocated texture pages, if any.
	//
	// Notes:
	//		This function will assert if there are any outstanding sub-region allocations.
	//
	//**************************************


	//******************************************************************************************
	//
	int iGetNumPages() const;
	//
	// Returns:
	//		The number of managed texture pages.
	//
	//**************************************


	//******************************************************************************************
	//
	int iGetMinRegionSize() const;
	//
	// Returns:
	//		The (power of two) minimum allowable dimension of an allocation of a sub-region in
	//		a page.
	//
	//**************************************


	//******************************************************************************************
	//
	int iGetMaxRegionSize() const;
	//
	// Returns:
	//		The (power of two) maximum allowable dimension of an allocation of a sub-region in
	//		a page.
	//
	//**************************************


	//******************************************************************************************
	//
	void ClampRegionSize
	(
		CVector2<int>& rv2_dim
	) const;
	//
	// Clamps the given sub-region dimensions to be within the allowable range.
	//
	// Notes:
	//		The given dimensions must be a non-zero power of two.
	//
	//**************************************


	//******************************************************************************************
	//
	int iGetPageSize
	(
		int i_page
	) const;
	//
	// Returns:
	//		The size of the given page.
	//
	//**************************************


	//******************************************************************************************
	//
	rptr<CRaster> prasGetPage
	(
		int i_page
	) const;
	//
	// Returns:
	//		The raster of the given page.
	//
	//**************************************


	//******************************************************************************************
	//
	void AllocateRegion
	(
		CRegionHandle& rrh_init,	// Handle to initialise.
		const CVector2<int>& v2_dim,// Dimensions of sub-region to allocate.
		bool b_modified				// Determines if the modified flag for the parent page
									// must be set.
	);
	//
	// Allocate a region in a page.
	//
	// Returns:
	//		The given handle is initialised.
	//
	// Notes:
	//		The dimensions of the region must be a power of two, but do not have to be square.
	//		They are clamped to be within the allowable range.
	//
	//		The given handle must not refer to a valid allocation. If the allocation is
	//		successful, the given handle will be initialised. Otherwise, it will remain invalid.
	//
	//**************************************


	//******************************************************************************************
	//
	void AllocateRegion
	(
		CRegionHandle& rrh_init,		// Handle to initialise.
		int i_page,
		const CVector2<int>& v2_origin,	// Origin of sub-region to allocate.
		const CVector2<int>& v2_dim,	// Dimensions of sub-region to allocate.
		bool b_modified					// Determines if the modified flag for the parent page
										// must be set.
	);
	//
	// Allocate a region at a specified location in a specified page.
	//
	// Returns:
	//		The given handle is initialised.
	//
	// Notes:
	//		The dimensions of the region must be a power of two, but do not have to be square.
	//		They are clamped to be within the allowable range.
	//
	//		The given handle must not refer to a valid allocation. If the allocation is
	//		successful, the given handle will be initialised. Otherwise, it will remain invalid.
	//
	//**************************************


	//******************************************************************************************
	//
	void DeleteRegion
	(
		CRegionHandle& rrh
	);
	//
	// Delete a region from a page.
	//
	// Notes:
	//		The destructor of the handle will invoke this function if it refers to a valid
	//		allocation.
	//
	//		This function unlocks the page's raster.
	//
	//**************************************


	//******************************************************************************************
	//
	bool bUploadRegion
	(
		const CRegionHandle& rh		// Handle specifying region to upload.
	) const;
	//
	// Upload a region of a page.
	//
	// Returns:
	//		'true' if the upload was successful.
	//
	// Notes:
	//		This function unlocks all the page's raster.
	//
	//**************************************


	//******************************************************************************************
	//
	bool bUploadPages
	(
		bool b_upload_all = false	// If set, upload all texture pages even if not modified.
	) const;
	//
	// Upload all modified texture pages.
	//
	// Returns:
	//		'true' if the upload was successful.
	//
	// Notes:
	//		The modified flags for the successfully uploaded pages are cleared.
	//
	//		This function unlocks all the pages' rasters.
	//
	//**************************************


	//******************************************************************************************
	//
	bool bIsModified
	(
		int i_page_num
	) const;
	//
	// Returns:
	//		'true' if the modified flag of the given page is set.
	//
	//**************************************


	//******************************************************************************************
	//
	void SetModified
	(
		int i_page_num,
		bool b_modified
	);
	//
	// Sets the modified flag of the requested page to the given state.
	//
	//**************************************


	//******************************************************************************************
	//
	void PrintStats
	(
		CConsoleBuffer& con
	) const;
	//
	// Print stats to given console.
	//
	//**************************************

private:
	// Not implemented.
	CTexturePageManager(const CTexturePageManager& texm);
	CTexturePageManager& operator=(const CTexturePageManager& texm);
};


#endif
