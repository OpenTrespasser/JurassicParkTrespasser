/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Texture management... This basically controls the allocation of texture pages
 *
 * Contents:
 *		CTextureManager
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/TextureManager.hpp                                         $
 * 
 * 15    8/25/98 2:36p Rvande
 * removed redundant class scope
 * 
 * 14    4/21/98 3:13p Rwyatt
 * Significantly changed to support the new VM system.
 * 
 * 13    98/02/26 14:01 Speter
 * Moved seterfFeatures to CTexture from SSurface, removing redundant flags.
 * 
 * 12    2/24/98 6:54p Rwyatt
 * Use the new texture packer
 * 
 * 11    2/18/98 1:06p Rwyatt
 * Removed the image loader into its own file
 * 
 * 10    2/09/98 12:24p Rwyatt
 * Fixed palette instancing bug while using virtual loader
 * 
 * 9     1/30/98 4:25p Rwyatt
 * Conventional loader is the default loader
 * 
 * 8     1/29/98 7:41p Rwyatt
 * New STL members to log packed textures, curved bump map parents and bump maps.
 * This is to aid the construction of virtual images.
 * 
 * 7     1/19/98 7:30p Pkeet
 * Added support for 16 bit bumpmaps by adding lower colour resolution and a smaller size for
 * 'CBumpAnglePair.'
 * 
 * 6     1/14/98 6:27p Rwyatt
 * New inline functions for returning and setting the pack curved status
 * 
 * 5     1/09/98 7:07p Rwyatt
 * The pack function now takes paramters that specify the curved state and mip level of a
 * texture/bump map.
 * Bump maps are kept in a list and not packed, they will be packed later after loading is
 * complete.
 * 
 * 4     9/01/97 7:56p Rwyatt
 * Will only pack surfaces that have not already been packed.
 * Has a member function to return the current list of packed surfaces, this is mainly used by
 * the texture pack dialog.
 * 
 * 3     8/19/97 7:03p Rwyatt
 * Sub allocated mem rasters (such as textures) now have an rpt back to the parent Texture Pack
 * surface. This is so the packer can be infomred when the texture is deleted, it is an rptr to
 * avoid static destructor dependencies.
 * 
 * 2     7/29/97 1:58a Rwyatt
 * Very basic texture manager but works for the time being. At the moment the upper limit on
 * texture pages is 2048 (4096 256x256 surfaces).
 * Logs everything to the LoadLog.txt console file.
 * 
 * 1     7/29/97 1:01a Rwyatt
 * Initial implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_LOADER_TEXTUREMANAGER_HPP
#define HEADER_LIB_LOADER_TEXTUREMANAGER_HPP

#include "Common.hpp"
#include "Lib/Sys/VirtualMem.hpp"
#include "TexturePackSurface.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/Renderer/Texture.hpp"
#include "Lib/View/Colour.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "vector.h"
#include "map.h"
#include "set.h"


#define MAX_TEXTURE_PAGES			1024			//64 megs of source textures for the time being..
#define u4MINIMUM_TEXTURE_PAGING	2*1024*1024		//2 megs minimum for dynamic texture paging
#define u4NONPAGEABLE_MIPS			(3)				//Number of mip levels that cannot be paged
#define	u4SMALL_TEXTURE_SIZE		(1<<(u4NONPAGEABLE_MIPS+2))			// Bottom 3 mips


//**********************************************************************************************
// The structure that references all loaded bump maps so they can be processed and packed
// later
//
struct SBumpMapListElement
// prefix ble
{
	//******************************************************************************************
	rptr<CRaster>		prasBumpMap;		// the bump maps raster
	bool				bCurved;			// is this surface curved, and therefore unique
	bool				bPacked;			// is this raster already packed
	ETexturePackTypes	eptMipLevel;		// mip level of this map or -1 for don't know

	//******************************************************************************************
	SBumpMapListElement
	(
		rptr<CRaster>		pras,
		bool				b_curved,
		bool				b_packed,
		ETexturePackTypes	ept
	)
	//
	//*********************************
	{
		prasBumpMap = pras;
		bCurved		= b_curved;
		bPacked		= b_packed;
		eptMipLevel	= ept;
	}

	//******************************************************************************************
	SBumpMapListElement();
};


//**********************************************************************************************
// This structure references all loaded rasters, these then form the directory for the packed
// image file.
//
struct SVirtualImageElement
// prefix vie
{
	//******************************************************************************************
	uint64			u8HashValue;
	rptr<CRaster>	prasTexture;
	rptr<CTexture>	ptexTexture;

	//******************************************************************************************
	SVirtualImageElement
	(
		rptr<CTexture>	ptex,
		rptr<CRaster>	pras,
		uint64			u8_hash
	)
	//
	//*********************************
	{
		// keep a copy of the raster so we can get to the address, width, height,
		// stride and palette

		ptexTexture = ptex;
		prasTexture	= pras;
		u8HashValue = u8_hash;
	}

	//******************************************************************************************
	SVirtualImageElement();
};


//**********************************************************************************************
struct SCurvedParentElement
{
	rptr<CRaster>	prasBumpMap;
	uint32			u4HashValue;
	bool			bTransparent;
	bool			bPacked;
};


//**********************************************************************************************
class test_curved_parent
{
public:
	bool operator()(const SCurvedParentElement& a, const SCurvedParentElement& b) const
	{
		return a.u4HashValue < b.u4HashValue;
	}
};


//**********************************************************************************************
//
typedef vector<SBumpMapListElement>		TBumpList;

//**********************************************************************************************
//
typedef vector<SVirtualImageElement>	TVirtualImageList;

//**********************************************************************************************
//
typedef set<SCurvedParentElement, test_curved_parent >	TCurvedParentList;


//**********************************************************************************************
// Texture manager packs source textures into texture pack surfaces in Virtual memory. Each
// texture pack surfcae contains two 256x256 texture pages interleaved on a scan line bases.
// This interleaving enables packed textures to be tiled.
// The texture manager is responsible for packing similar textures with each other so to minimize
// any loading that needs to be done, a texture pack surface must be loaded in one go,
// The ETexturePackType varaible in each page controls the type of the page and can be set to
// one of the enum values defined in TexturePackSurface.hpp.
// The rules for packing textures are:
//	1)	Bit depth must match, this is because all packed rasters share the same parent and the
//		parent must have the same bit depth.
//
//	2)	If the ETexturePackTypes variable is less than zero the texture is a special type or
//		it is a type of don't care. All negative types are packed with the same type only.
//
//	3)	Mip level 0 is always packed by itself, no matter what. This is to ensure no remains
//		of mip level 0 are in memory should it not be used. Mip level 0 consumes 3/4 of the
//		total memory so not loading it unless it is required is a big win.
//
//	4)	Other mip levels can be packed with each other by a distance specifed in the variable
//		i4MipLevelDifference. IE, if this 1 and the surface is a mip level 2, then mip level 1
//		or mip level 3 can be packed,
//
class CTextureManager
//prefix: txm
{
public:
	//******************************************************************************************
	CVirtualMem*					pvmeTextures;
	CVirtualMem::CVMSubAllocator*	pvmsaSmallTextures;

	//******************************************************************************************
	CTextureManager();
	~CTextureManager();

	//******************************************************************************************
	void CreateSmallTextureAllocator();

	//******************************************************************************************
	void DestroySmallTextureAllocator();

	//******************************************************************************************
	void Reset();

	//******************************************************************************************
	void Destroy();

	//******************************************************************************************
	uint32 u4MinimumTextureMemory
	(
	)
	//*********************************
	{
		return u4SmallTextureSubAllocateSize + u4MINIMUM_TEXTURE_PAGING;
	}

	//******************************************************************************************
	void SetSmallTextureSize
	(
		uint32 u4_small
	)
	{
		u4SmallTextureSubAllocateSize = u4_small;
	}

	//******************************************************************************************
	uint32 u4SmallTexturesize
	(
	)
	{
		return u4SmallTextureSubAllocateSize;
	}

	//******************************************************************************************
	// Check if a given memory range is available
	bool bIsAvailable
	(
		void*		pv_base,
		uint32		u4_len
	)
	//*********************************
	{
		return pvmeTextures->bIsMemoryPresent(pv_base,u4_len);
	}

	//******************************************************************************************
	// Check if a given memory range is available
	void RequestMemory
	(
		void*		pv_base,
		uint32		u4_len
	)
	//*********************************
	{
		pvmeTextures->RequestMemory(pv_base,u4_len);
	}

	//******************************************************************************************
	bool bAllPresent()
	{
		return pvmeTextures->bAllPagesPresent();
	}

	//******************************************************************************************
	// pack the specifed raster into a texture sheet, this will have to search for the most
	// intelligent place to put it.
	rptr<CRaster> prasPackTexture
	(
		rptr<CRaster>		pras_src, 
		bool				b_curved = false,
		ETexturePackTypes	ept = eptDONT_CARE
	);

	//******************************************************************************************
	// returns a pointer to the array of rptr's to the texture surfaces.
	CRenderTexturePackSurface** aptpsGetPageList()
	{
		return	aptpsTexturePageList;
	}

	//******************************************************************************************
	void CopyRasterData
	(
		rptr<CRaster>	pras_src,			// original raster
		rptr<CRaster>	pras_copy			// new raster to be filled in
	);


	//******************************************************************************************
	// Set the mip level packing difference
	void SetMipLevelPackingDifference
	(
		int32	i4_mip
	)
	{
		Assert(i4_mip>=0);

		i4MipLevelDifference = i4_mip;
	}

	//******************************************************************************************
	int32 i4GetMipLevelPackingDifference()
	{
		return i4MipLevelDifference;
	}


	//******************************************************************************************
	//******************************************************************************************
	//
	// Bump map logging.
	// =================
	//
	// The function below all reference a TBumpList. This STL vector is used to hold all
	// bump maps while loading. Bump maps are pack while loading in the same way as textures,
	// but they cannot be actually packed until the load step is complete. The reason bumps
	// cannot be packed is that once packed they lose their matrix which is required for
	// the curvature process. So if a bump is packed early in the load stage and then later
	// a polygon wishes to curve that packed bump map and make it unique, it cannot be done.
	// So to fix this all bump maps are kept in system memory within their original rasters
	// until the load step is complete.
	// Once the load step is complete CLoadWorld calls the PackBumpMaps member function which
	// interates through ths TBumpList and packs the bumps, because all the meshes now point
	// to these unpacked bump maps the maps are modifed in place to save having to go and find
	// all the references and relocate them. Curved maps can optionally be packed at this stage
	// although it is not advised because of the huge memory waste.
	// Bump maps are also logged in the packed image list which is the list that is used to make
	// the PID file, see below.
	//
	//******************************************************************************************
	//******************************************************************************************

	//******************************************************************************************
	// Allocate the STL vector that will hold all the loaded bump maps
	void AllocateBumpMapVector()
	{
		if (pblBumpMaps == NULL)
		{
			pblBumpMaps = new TBumpList;
		}
	}

	//******************************************************************************************
	// delete the STL vector that contains the list of loaded bump maps
	void RemoveBumpMapVector()
	{
		delete pblBumpMaps;
		pblBumpMaps = NULL;
	}

	//******************************************************************************************
	// Add the specified surface to the bump map list.
	//
	void TrackBumpMap
	(
		rptr<CRaster>		pras,
		bool				b_packed,
		bool				b_curved,
		ETexturePackTypes	ept
	)
	{
		// must be abump map		
		Assert(pras->iPixelBits==iBUMPMAP_RESOLUTION);

		(*pblBumpMaps).push_back( SBumpMapListElement(pras,b_packed,b_curved,ept) );
	}

	//******************************************************************************************
	// Return the list of loaded bump maps
	TBumpList*	pblGetBumpMaps()
	{
		return pblBumpMaps;
	}

	//******************************************************************************************
	// Call this member after a loading a GROFF file. All the new bump maps will not be packed
	// but will have been curved. Now it is safe to pack the none curved maps, the curved maps
	// are packed depending on the bPackCurved flag.
	void PackBumpMaps();


	//******************************************************************************************
	// Create an empty new bump map page (16 bit) and insert it into the database
	int32 i4CreateNewBumpPage()
	{
		for (int i = 0; i<MAX_TEXTURE_PAGES; i++)
		{
			if	(aptpsTexturePageList[i] == NULL)
			{
				aptpsTexturePageList[i] = new CRenderTexturePackSurface(iBUMPMAP_RESOLUTION);
				return i;
			}
		}

		// no more texture pages
		Assert(0);
		return -1;
	}

	//******************************************************************************************
	// Controls if curved maps should be packed as normal bump maps, rounded to the next power
	// of two.
	// This wastes huge amounts of memory so by default is off, curved mump maps should be
	// packed with the bump map packing dialog. 
	// This option is useful for testing as you can quickly generate the VM image.
	//
	void PackCurvedBumpMaps
	(
		bool	b_pack
	)
	{
		bPackCurved = b_pack;
	}

	//******************************************************************************************
	// Returns the pack curved status
	bool bPackCurvedBumpMaps()
	{
		return bPackCurved;
	}


	//******************************************************************************************
	//******************************************************************************************
	//
	// Packed Texture Logging.
	// =======================
	//
	// Every texture that is in virtual memory should be logged in the pack image log. This
	// applies to texture maps, bump maps, curved maps and non-packed textures.
	// When the virtual image is saved out this log makes the directory file which enables the
	// loader to quickly relocate textures within the virtual image.
	// Before the direcrtory file is saved this log should be scanned because unpacked, curved
	// bump maps will be in this log but will have system memory pointers which are invalid.
	//
	//******************************************************************************************
	//******************************************************************************************


	//******************************************************************************************
	//
	void AddToPackLog
	(
		rptr<CTexture>	ptex,				// source texture holding the raster
		rptr<CRaster>	pras_texture,		// Resulting raster
		uint64			u8_hash				// 64 bit value for a unique ID
	)
	//*********************************
	{
		// if we have no virtual image list, create a new one.
		if (pvilVirtualImageMaps == NULL)
		{
			pvilVirtualImageMaps = new TVirtualImageList;
		}

		//
		// Assert if the hash valus is already in the list
		//
/*		TVirtualImageList::iterator i;

		for (i = (*pvilVirtualImageMaps).begin(); i != (*pvilVirtualImageMaps).end() ; i++)
		{
			if ( (*i).u8HashValue == u8_hash )
			{
				Assert(0);
			}
		}*/

		(*pvilVirtualImageMaps).push_back( SVirtualImageElement
										   (
												ptex,
												pras_texture,
												u8_hash
											) );
	}



	//******************************************************************************************
	//
	SVirtualImageElement*	vieFindInPackedLog
	(
		uint64	u8_hash, 
		bool	b_bump,
		uint32	u4_const_col
	)
	{
		// we have no packed texture log so could not have found the source
		if (pvilVirtualImageMaps == NULL)
			return NULL;

		TVirtualImageList::iterator i;

		for (i = (*pvilVirtualImageMaps).begin(); i != (*pvilVirtualImageMaps).end() ; i++)
		{
			if ( (*i).u8HashValue == u8_hash )
			{
				// we have found the hash value check the other parameters
				if ( (!!(*i).ptexTexture->seterfFeatures[erfBUMP] == b_bump) && 
					 ((*i).ptexTexture->tpSolid == u4_const_col) )
					return &(*i);

				//
				// Hash value matches but not all other data does.
				// this is a genuine hash collision.
				//
				Assert(0);
				return NULL;
			}
		}

		return NULL;
	}



	//******************************************************************************************
	// Delete the memory (and all of the elements) used by the image pack list
	void RemovePackImageVector()
	{
		delete pvilVirtualImageMaps;
		pvilVirtualImageMaps = NULL;
	}

	//******************************************************************************************
	void CreatePackedImageDirectory
	(
		const char* str_dir_fname
	);


	//******************************************************************************************
	//******************************************************************************************
	//
	// Curved bump parent map logging
	// ==============================
	//
	//******************************************************************************************
	//******************************************************************************************

	//******************************************************************************************
	// Delete the memory (and all of the elements) used by the curved bump parent list
	void RemoveCurvedBumpParentList
	(
	)
	//*********************************
	{
		delete pcplParentBumpMaps;
		pcplParentBumpMaps = NULL;
	}

	//******************************************************************************************
	// Adds this texture as a parent to a curved bump maps, which will be a section of itself.
	void AddCurvedBumpParent
	(
		rptr<CRaster>		pras,
		uint32				u4_hash,
		bool				b_transparent
	)
	//*********************************
	{
		if (pcplParentBumpMaps == NULL)
			pcplParentBumpMaps = new TCurvedParentList;

		SCurvedParentElement	a;

		a.prasBumpMap	= pras;
		a.u4HashValue	= u4_hash;
		a.bTransparent	= b_transparent;
		a.bPacked		= false;

		pcplParentBumpMaps->insert( a );
	}

protected:
	// The arrays holding pointers to surfaces.
	CRenderTexturePackSurface*	aptpsTexturePageList[MAX_TEXTURE_PAGES];

	// The arrays holding pointers to loaded bump maps, initialized when the first bump map
	// is loaded
	TBumpList*					pblBumpMaps;


	// STL vector holding all of the information about the VM pack file, this vector is
	// saved as the directory for the image file, enabling us to load a scene and not
	// load the textures.
	TVirtualImageList*			pvilVirtualImageMaps;


	TCurvedParentList*			pcplParentBumpMaps;


	// Current mip pack difference.
	// This control what mip levels can be packed with aach other. If the difference in the
	// mip levels is less than this number then the maps can be packed together
	int32						i4MipLevelDifference;

	// Controls if PackBumpMaps should pack curved maps as ordinary maps. By default this
	// is false and curved maps are not packed.
	bool						bPackCurved;

	uint32						u4SmallTextureSubAllocateSize;

	//******************************************************************************************
	// Find a raster in a packed surface that is suitable for packing the source.
	// The returned raster contains no valid data and is always of type CRasterMem
	rptr<CRasterMem> prasFindPackLocation
	(
		rptr<CRaster>		pras_src,
		ETexturePackTypes	ept
	);
};




//**********************************************************************************************
// Globals.....
//**********************************************************************************************
// The global texture manager, there can be only one
//
extern CTextureManager	gtxmTexMan;


#endif //HEADER_LIB_LOADER_TEXTUREMANAGER_HPP

