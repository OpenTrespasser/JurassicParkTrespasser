/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of texture manager
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/TextureManager.cpp                                         $
 * 
 * 28    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 27    98.07.27 7:11p Mmouni
 * Checks for bump-maps now include a check of the pixel format as well as checking the bit
 * depth.
 * 
 * 26    98.07.08 12:10p Mmouni
 * Now saves the palette for parent bump maps.
 * 
 * 25    7/02/98 7:11p Rwyatt
 * New VM allocation stratergy for curved bump maps
 * 
 * 24    4/29/98 7:05p Rwyatt
 * Texture manager now resets properly
 * 
 * 23    4/22/98 12:35p Rwyatt
 * Added VER_TEST to the always assert on textures larger than 256
 * 
 * 22    4/21/98 8:50p Rwyatt
 * Terminal error for maps larger than 256
 * 
 * 21    4/21/98 3:13p Rwyatt
 * Significantly changed to support the new VM system.
 * 
 * 20    3/06/98 8:40p Rwyatt
 * When generating the swap file, if curved parent bump maps are above 256x256 in size it now
 * allocates virtual memory rather than trying to pack them which caused a stack fault.
 * 
 * 19    98/02/26 14:02 Speter
 * Moved seterfFeatures to CTexture from SSurface, removing redundant flags.
 * 
 * 18    2/24/98 6:54p Rwyatt
 * Use the new texture packer
 * 
 * 17    2/18/98 1:05p Rwyatt
 * Removed the image loader into its own file
 * 
 * 16    2/09/98 12:24p Rwyatt
 * Fixed palette instancing bug while using virtual loader
 * 
 * 15    2/03/98 5:35p Rwyatt
 * Curved bump maps are handled in a special way because they can be less than 8x8 in size.
 * 
 * 14    2/03/98 2:40p Rwyatt
 * Now files are shared the virtual loader can be used by default.
 * 
 * 13    2/03/98 2:24p Rwyatt
 * Removed AlwaysAssert so if a file is not found in the image it will be loaded via the
 * conventional method.
 * 
 * 12    1/30/98 4:35p Rwyatt
 * Opps
 * 
 * 11    1/30/98 4:25p Rwyatt
 * Conventional loader is the default loader
 * 
 * 10    1/29/98 7:39p Rwyatt
 * Mip 0 is packed alone to aid the loading process when it is not used,
 * Create a virtiual image directory so the textures in the image can be relocated on load.
 * Textures that are too big to pack just get copied into virtual memory, so do curved bump maps
 * when the automatic packer is active.
 * All allocations within the texture manager are logged so the image directory can be built and
 * verified.
 * 
 * 9     1/19/98 7:30p Pkeet
 * Added support for 16 bit bumpmaps by adding lower colour resolution and a smaller size for
 * 'CBumpAnglePair.'
 * 
 * 8     1/14/98 6:26p Rwyatt
 * Has a callback function that will pack bump maps after they have all been loaded, and
 * optionally pack curved maps.
 * The pack search function has been isolated so it can be used by the deferred bump packing
 * 
 * 7     1/09/98 7:07p Rwyatt
 * The pack function now takes paramters that specify the curved state and mip level of a
 * texture/bump map.
 * Bump maps are kept in a list and not packed, they will be packed later after loading is
 * complete.
 * 
 * 6     12/04/97 4:13p Rwyatt
 * New header file order
 * Texture manager now create a VM handler with a default memory pool of 256Megs. Texture memory
 * is commited to this pool as it is used by the VM manager, upto the prescribed limit.
 * 
 * 5     10/10/97 1:36p Mmouni
 * Now updates tiling masks when copying rasters.
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
 * 1     7/29/97 1:00a Rwyatt
 * Initial implementation
 * 
 **********************************************************************************************/


#include "common.hpp"
#include "stdio.h"
#include "stdlib.h"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/Sys/VirtualMem.hpp"
#include "TextureManager.hpp"
#include "ImageLoader.hpp"
#include "TexturePackSurface.hpp"
#include "Loader.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/Std/StdLibEx.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/FileEx.hpp"


//*********************************************************************************************
// This is the size of the virtual block that VM system will allocate while loading textures..
//
#define		u4TEXTURE_MEMORY_POOL_SIZE			(128*1024*1024)
#define		u4TEXTURE_MEMORY_PAGE_SIZE			(64*1024)
#define		u4TEXTURE_MEMORY_MAX_PHYSICAL		(16*1024*1024)
#define		u4SMALL_TEXTURE_MAX_MEM				(6*1024*1024)


//*********************************************************************************************
// Global texture manager.
//
CTextureManager	gtxmTexMan;


//*********************************************************************************************
//
CTextureManager::CTextureManager()
{
	// this class does not have to be stored, the allocator address is stored within a static
	// inside itself. This way here can be only one.

	// Create an allocate VM manager, this will just keep allocating memory until it is
	// purged. At that point it will start to demand load texture pages.
	pvmeTextures = NULL;
	
	// Allocator for smallest textures, these textures are never decommited so are never
	// required to be reloaded.
	pvmsaSmallTextures = NULL;

	// Set the amount of memory that is to be reserved
	u4SmallTextureSubAllocateSize = u4SMALL_TEXTURE_MAX_MEM;

	// no STL vector until a bump map is loaded
	pblBumpMaps = NULL;
	pvilVirtualImageMaps = NULL;
	pcplParentBumpMaps = NULL;

	// Pack curved bump maps
	bPackCurved = true;

	// a mip can only be packed with a neighbouring sized mip.
	// This only applies to mips that are above level 0.
	SetMipLevelPackingDifference(2);

	// delete the render texture packed pages.
	for (int i=0; i<MAX_TEXTURE_PAGES; i++)
	{
		aptpsTexturePageList[i] = NULL;
	}
}


//*********************************************************************************************
// Simply call the destroy function...
CTextureManager::~CTextureManager()
{
	Destroy();
}


//**********************************************************************************************
// Destroys all the structures and classes owned by th texture manager.
// Go through the arrays and delete any texture surfcaes that we have allocated.
// This will free all the memory used by the quad trees and other packing structures, but it
// may not free the surface itself because they are referenced counted and will be delelted
// when the last reference to them is removed.
void CTextureManager::Destroy
(
)
//*************************************
{
	RemoveBumpMapVector();
	RemovePackImageVector();
	RemoveCurvedBumpParentList();

	// delete the render texture packed pages.
	for (int i=0; i<MAX_TEXTURE_PAGES; i++)
	{
		delete aptpsTexturePageList[i];
		aptpsTexturePageList[i] = NULL;
	}

	DestroySmallTextureAllocator();
	delete pvmeTextures;
}


//**********************************************************************************************
// Create the initial allocator for the small textures which are at the start of the virtual
// memory and are not paged by the VM system.
void CTextureManager::CreateSmallTextureAllocator
(
)
//*************************************
{
	Assert(pvmsaSmallTextures == NULL);				// No current allocator
	Assert(!pvmeTextures->bFileBased());			// VM is not paging
	Assert(!pvmeTextures->bAnythingAllocated());	// VM has nothing allocated

	pvmsaSmallTextures = new CVirtualMem::CVMSubAllocator(pvmeTextures,u4SmallTextureSubAllocateSize);
}



//*********************************************************************************************
// Remove the sub allocator that we was using for small textures. Once this is done small
// textures are allocated as normal.
void CTextureManager::DestroySmallTextureAllocator
(
)
//*************************************
{
	if (pvmsaSmallTextures)
	{
		delete pvmsaSmallTextures;
		pvmsaSmallTextures = NULL;
	}
}


//*********************************************************************************************
// Reset/init the texture manager
// The texture manager cannot be used until this function is called.
// The world database calls this function every time it is reset.
void CTextureManager::Reset
(
)
//*************************************
{
	uint32	u4_max_phys;
	uint32	u4_load_page;

	if (pvmeTextures)
	{
		u4_max_phys	= pvmeTextures->u4GetMaxPhysicalMem();
		u4_load_page = pvmeTextures->u4GetLoadPageSize();

		Destroy();
	}
	else
	{
		u4_load_page = u4TEXTURE_MEMORY_PAGE_SIZE;
		u4_max_phys = u4TEXTURE_MEMORY_MAX_PHYSICAL;
	}

	pvmeTextures = new CVirtualMem(u4TEXTURE_MEMORY_POOL_SIZE,
							u4_load_page,u4_max_phys);
}






//*********************************************************************************************
rptr<CRaster> CTextureManager::prasPackTexture
(
	rptr<CRaster>		pras_src,
	bool				b_curved,
	ETexturePackTypes	ept
)
//*************************************
{
	rptr<CRaster>		pras_copy;

	if (b_curved)
	{
		if ((pras_src->iWidth<16) || (pras_src->iHeight<16) )
		{
			ept = eptSMALLEST;
		}
	}
	else
	{
		// If this texture has maps that are less than u4SMALL_TEXTURE_SIZE in both directions
		// then it is a small texture...
		if ((pras_src->iWidth<u4SMALL_TEXTURE_SIZE) && (pras_src->iHeight<u4SMALL_TEXTURE_SIZE) )
		{
			ept = eptSMALLEST;
		}
	}

	// If either the width or height is less than 16 then regardless of the other
	// dimension this texture is non pageable as in this case no more mip maps
	// will be generated...
	if ((pras_src->iWidth<16) || (pras_src->iHeight<16) )
	{
		ept = eptSMALLEST;
	}

	if ( (pras_src->iWidth>256) || (pras_src->iHeight>256) )
	{

		// the source texture is bigger than a pack surface therefore it cannot be packed.
		conLoadLog.Print("  Texture/Bump too big to pack: w=%d, h=%d\n", 
							pras_src->iWidth, 
							pras_src->iHeight);

		//
		// we cannot pack this texture or bump but we can just allocate it in Virtual Memory
		//
		if (pras_src->iPixelBits == iBUMPMAP_RESOLUTION && !(pras_src->pxf.cposG == 0x00F0))
		{
			if (b_curved)
			{
				//
				// curved maps can be bigger than 256 x 256 because they are grown by 5 pixels.
				// There is an assert elsewhere to ensure that the curved bump map is not
				// tiled.
				//
				MEMLOG_ADD_COUNTER(emlBumpNoPack,1);
				MEMLOG_ADD_COUNTER(emlBumpNoPackMem,pras_src->iLineBytes()*pras_src->iHeight);

				AllocateBumpMapVector();
				TrackBumpMap(pras_src, b_curved, false, ept);

				return pras_src;
			}
		}

		//
		// maps cannot be bigger than 256x256, unless they are curved, see above
		//
#if (VER_TEST == TRUE)
		char str_buffer[1024];

		sprintf(str_buffer, "A map on the current object is greater than 256 (%dx%d)",pras_src->iWidth,pras_src->iHeight);

		if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
			DebuggerBreak();
#endif

		return rptr0;
	}


	if ( (pras_src->iWidth<8) || (pras_src->iHeight<8) )
	{
		// source texture map is too small to pack, adjust the counters and log it in the
		// file..

		if (pras_src->iPixelBits == iBUMPMAP_RESOLUTION && !(pras_src->pxf.cposG == 0x00F0))
		{
			// we can only have curved bump maps that are less than 8x8
			AlwaysAssert(b_curved);

			// we have not packed the bump map yet.
			MEMLOG_ADD_COUNTER(emlBumpNoPack,1);
			MEMLOG_ADD_COUNTER(emlBumpNoPackMem,pras_src->iLineBytes()*pras_src->iHeight);

			AllocateBumpMapVector();
			TrackBumpMap(pras_src, b_curved, false, ept);

			return pras_src;
		}


		MEMLOG_ADD_COUNTER(emlTextureNoPack,1);
		MEMLOG_ADD_COUNTER(emlTextureNoPackMem,pras_src->iLineBytes()*pras_src->iHeight);

		conLoadLog.Print("  Texture too small to pack: w=%d, h=%d\n", 
						pras_src->iWidth, 
						pras_src->iHeight);

		// we should never get a texture that is this small...
		// This must be an always assert because the Virtual loader will fail.
		AlwaysAssert(0);

		return pras_src;
	}



	//
	// this surface has already been packed just return and don't log anything....
	//
	if (pras_src->iLinePixels == 512)
	{
		// just enaure that this texture is packed in virtual memory
		Assert( gtxmTexMan.pvmeTextures->bVirtualAddressValid(pras_src->pSurface) );
		return pras_src;
	}


	//
	// If we have a map which is not a curve and it is not a power of 2, report it
	// in the log file.
	//
	if (!b_curved)
	{
		// if the source is not a power of two then log it in the LoadLog file
		if ( (!bPowerOfTwo(pras_src->iWidth)) || (!bPowerOfTwo(pras_src->iHeight)) )
		{
			conLoadLog.Print("  Not power of 2: w=%d, h=%d\n", pras_src->iWidth, pras_src->iHeight);
			conLoadLog.Print("  Rounded up to:  w=%d, h=%d (wastage = %d bytes)\n", 
							NextPowerOfTwo(pras_src->iWidth), 
							NextPowerOfTwo(pras_src->iHeight),
							((NextPowerOfTwo(pras_src->iWidth)*NextPowerOfTwo(pras_src->iHeight)) 
							- (pras_src->iWidth*pras_src->iHeight)) * pras_src->iPixelBytes() );
		}
	}


	//
	// are we packing a bump map, if so just log it because we cannot pack bumps until all
	// meshes have been loaded.
	//
	if (pras_src->iPixelBits == iBUMPMAP_RESOLUTION && !(pras_src->pxf.cposG == 0x00F0))
	{
		AllocateBumpMapVector();
		TrackBumpMap(pras_src, b_curved, false, ept);

		// return the source
		MEMLOG_ADD_COUNTER(emlBumpNoPack,1);
		MEMLOG_ADD_COUNTER(emlBumpNoPackMem,pras_src->iLineBytes()*pras_src->iHeight);
		return pras_src;
	}
	else
	{
		// we cannot have a curved texture...
		Assert(b_curved == false);
	}

	//
	// Create a packed raster that we can use to hold the source data, cast it from
	// a CRasterMem to the base class CRaster
	//
	pras_copy = rptr_cast(CRaster, prasFindPackLocation(pras_src, ept) );

	AlwaysAssert(pras_copy);

	CopyRasterData(pras_src, pras_copy);

	//
	// Return the new raster as the one to use
	//
	return pras_copy;
}


//*********************************************************************************************
// Function to copy 1 raster into another. The destination raster must be at least the same
// size as the source and must be the same bit depth
//
void CTextureManager::CopyRasterData
(
	rptr<CRaster>	pras_src,			// original raster
	rptr<CRaster>	pras_copy			// new raster to be filled in
)
//
//*************************************
{
	//
	// Copy the source data into the new raster
	//
	uint8*	src;
	uint8*	dst;

	// ensure that the new raster is big enough to take the source raster
	Assert(pras_copy->iWidth>=pras_src->iWidth);
	Assert(pras_copy->iHeight>=pras_src->iHeight);
	Assert(pras_copy->iPixelBits>=pras_src->iPixelBits);

	pras_src->Lock();
	pras_copy->Lock();
	// copy the source raster into the new raster which is a sub region of a pack
	// surface.
	src = (uint8*)pras_src->pAddress(0);
	dst = (uint8*)pras_copy->pAddress(0);

	for (int y = 0; y<pras_src->iHeight; y++)
	{
		for (int x = 0; x<pras_src->iWidth*(pras_src->iPixelBits/8); x++)
		{
			dst[x] = src[x];
		}
		src = ((uint8*)src) + pras_src->iLineBytes();
		dst = ((uint8*)dst) + pras_copy->iLineBytes();
	}

	pras_src->Unlock();
	pras_copy->Unlock();


	// set the width and height of the new raster to be exaclty the same a the source, this
	// is done because the new raster may have been bigger than the source.
	pras_copy->iWidth = pras_src->iWidth;
	pras_copy->iHeight = pras_src->iHeight;

	// Since we have messed with the width and height, adjust the tiling info.
	pras_copy->UpdateTilingMasks();
}


//*********************************************************************************************
//
rptr<CRasterMem> CTextureManager::prasFindPackLocation
(
	rptr<CRaster>		pras_src,
	ETexturePackTypes	ept
)
//
//*************************************
{
	rptr<CRasterMem>	pras_pack;
	int32				i4_empty = -1;
	int32				i4_start = 0;

	while (1)
	{
		for (int i = i4_start; i<MAX_TEXTURE_PAGES; i++)
		{
			if	(aptpsTexturePageList[i])
			{
				// we have found a surface pointer.
				if (aptpsTexturePageList[i]->iGetPixelBits() == pras_src->iPixelBits)
				{
					// The bit depths match so check the mip level
					ETexturePackTypes ept_surface= aptpsTexturePageList[i]->eptGetType();

					if (ept>eptTOP_LEVEL)
					{
						// we specified a mip level that is not 0 so obey the rule of packing
						// mip maps. Currently this is the mip level has to be within
						// some range of the packed mip level
						if ( (ept_surface <= eptTOP_LEVEL) || 
								( abs((int32)ept_surface - (int32)ept) > i4MipLevelDifference) )
							continue;
					}
					else
					{
						// mip type  0 and all negative types are packed alone no matter what the
						// mip packing range is. This is so we can remove the top mip level and
						// no of trace of them will have to be loaded.
						if (ept_surface != ept)
							continue;
					}

					//
					// So lets try to add the map to this surface.
					// (find a block of the correct size in the quad tree
					// Sizes are rounded up to the next power of two)
					//
					pras_pack = aptpsTexturePageList[i]->prasAllocateRaster(pras_src);

					if (!pras_pack)
					{
						// cannot add the texture, go onto the next page
						continue;
					}

					return pras_pack;
				}
			}
			else
			{
				// we have a NULL pointer, so rememebr where it is in case we have to a page.
				if (i4_empty == -1)
				{
					i4_empty = i;
				}
			}
		}

		// at this point we have failed to add a texture to an existing page, either there
		// is not one at the correct bit depth or the source is too big to fit into a space
		// on an existing surface....
		// i4_empty is the location in the array of an empty slot that we can allocate a page
		// into, if i4_empty is -1 then all available pages have been alloctaed and drastic
		// measures now need to be taken....
		if (i4_empty == -1)
		{
			conLoadLog.Print("  Out of texture pages\n");
			return rptr0;
		}

		//
		// allocate a new texture page in the array ay i4_empty, this page should be the same
		// bit depth as the source texture.
		// The mip level of a new texture page is that of the first texture to be packed in it,
		//
		aptpsTexturePageList[i4_empty] = new CRenderTexturePackSurface(pras_src->iPixelBits,ept);

		// start looking on the next pass from the element we have just added
		i4_start = i4_empty;
	}

	// we can never get to here but we have to keep the compiler happy.
	return rptr0;
}




//*********************************************************************************************
// This should be called when loading is complete.
// This function will go through all loaded bump maps and pack them, this cannot be done while
// the bumps are being loaded in case they are curved after being packed.
//
void CTextureManager::PackBumpMaps
(
)
//
//*************************************
{
	// if there is no bump list then there is nothing to pack
	if (pblBumpMaps)
	{
		// go through all of the packed bump maps and check for ones that are not packed
		for (TBumpList::iterator i = (*pblBumpMaps).begin(); i != (*pblBumpMaps).end() ; i++)
		{
			// if we are not packing curves and this map is curved, ignroe it and go on to the next
			if ( (!bPackCurved) && ((*i).bCurved) )
				continue;

			// if the map is not already packed, process it
			if ((*i).bPacked == false)
			{
				rptr<CRasterMem> pras_packed;

				// pack the source bump map
				if ( ((*i).prasBumpMap->iWidth>256) || ((*i).prasBumpMap->iHeight>256) || (*i).bCurved )
				{
					//
					// allocate a raster in virtual memory, it is too big to allocate from a
					// pack surface or if it is curved.
					//

					EMemType	emt;
					if (((*i).prasBumpMap->iWidth<u4SMALL_TEXTURE_SIZE) && ((*i).prasBumpMap->iHeight<u4SMALL_TEXTURE_SIZE) )
					{
						emt = emtTexManSubVirtual;
					}
					else
					{
						emt = emtTexManVirtual;
					}

					if (((*i).prasBumpMap->iWidth<16) || ((*i).prasBumpMap->iHeight<16) )
					{
						emt = emtTexManSubVirtual;
					}

					pras_packed = rptr_cast(CRasterMem, rptr_new CRasterMem((*i).prasBumpMap->iWidth, 
																			(*i).prasBumpMap->iHeight,
																			(*i).prasBumpMap->iPixelBits,
																			0,
																			&(*i).prasBumpMap->pxf,
																			emt) );
				}
				else
				{
					if (((*i).prasBumpMap->iWidth<16) || ((*i).prasBumpMap->iHeight<16) )
					{
						Assert( (*i).eptMipLevel == eptSMALLEST);
					}

					//
					// allocate a raster in an existing pack surface...
					//
					pras_packed = prasFindPackLocation( (*i).prasBumpMap, (*i).eptMipLevel);
				}

				// if we get a packed raster we have succeded, so mark the surface as packed
				if (pras_packed)
				{
					MEMLOG_SUB_COUNTER(emlBumpNoPack,1);
					MEMLOG_SUB_COUNTER(emlBumpNoPackMem,
								(*i).prasBumpMap->iLineBytes()*(*i).prasBumpMap->iHeight);

					// change the bump raster to be like the copy
					// pras_copy if going to get corrupted by this call but that does not matter
					// because it is a local and will be destroyed before the end of this function.
					(*i).prasBumpMap->ChangeRaster( pras_packed );

					(*i).prasBumpMap->UpdateTilingMasks();
					// WE DO NOT NEED TO MODIFY THE IMAGE LOG FOR RASTERS THAT ARE CHANGED BECAUSE THE
					// RASTER ADDRESS DOES NOT CHANGE, ONLY THE CLASS CONTENTS WHICH ARE REFERENCED WHEN
					// WE SAVE THE IMAGE FILE.

					// mark this element as packed.
					(*i).bPacked = true;
				}
			}
		}
	}
}




//**********************************************************************************************
// Create Image directory file
//
#define MAX_PAL		128
//
void CTextureManager::CreatePackedImageDirectory
(
	const char* str_dir_name
)
//*************************************
{
	uint32	u4_palid[MAX_PAL];
	CPal*	ppal[MAX_PAL];
	uint32	u4_next_pal = 0;

	uint32	u4_texture = 0;
	uint32	u4_bump_map = 0;
	uint32	u4_parent_bump = 0;

	TVirtualImageList::iterator i;
	TCurvedParentList::iterator j;

	memset(u4_palid, 0, sizeof(uint32)*MAX_PAL);
	memset(ppal, 0, sizeof(CPal*)*MAX_PAL);

	dprintf("Creating Packed Image Directory\n");
	dprintf("********************************************************\n");


	// if there is image log list then return
	if (pvilVirtualImageMaps == NULL)
	{
		dprintf("ABORT: No textures\n");
		return;
	}


	//******************************************************************************************
	//
	// Remove parent bump maps that are also used as normal bump maps. After this operation the
	// parent list should contain textures that are never seen. See Below
	//
	dprintf("Validating parent bump maps...\n");

	if (pcplParentBumpMaps)
	{
		for (i = (*pvilVirtualImageMaps).begin(); i != (*pvilVirtualImageMaps).end() ; i++)
		{
			// we have to be a bump map....
			if ( (*i).ptexTexture->seterfFeatures[erfBUMP])
			{
				// the top 32 bits of our hash value must be zero, otherwise we cannot be a parent map..
				if ( (uint32)(((*i).u8HashValue)>>32) == 0 )
				{
					bool	b_found;

					while (1)
					{
						b_found = false;

						// go through all of the parent maps in the SET....
						for (j = (*pcplParentBumpMaps).begin(); j != (*pcplParentBumpMaps).end() ; ++j)
						{
							// if we find a matching hash value, remove the parent. Break out and go through the list again
							// keep doing this until we do not find a parent.
							if ( (*j).u4HashValue == (uint32)((*i).u8HashValue & 0xffffffff) )
							{
								dprintf("WARNING: Parent of curved bump map also used as a non curved bump map, updating directory\n");
								(*pcplParentBumpMaps).erase(j);
								b_found = true;
								break;
							}
						}
						
						if (!b_found)
							break;
					}
				}
			}
		}
	}


	//******************************************************************************************
	//
	// make a unique list of palette IDs
	//
	dprintf("Creating palette ID array...\n");

	for (i = (*pvilVirtualImageMaps).begin(); i != (*pvilVirtualImageMaps).end() ; i++)
	{
		if ((*i).prasTexture->pxf)
		{
			if ((*i).prasTexture->pxf.ppalAttached)
			{
				bool b_found = false;

				for (int32 i4 = 0; i4<MAX_PAL; i4++)
				{
					// zero is a not used entry
					if (u4_palid[i4] == 0)
						continue;

					// the palette IDs and the address has to match
					if ( u4_palid[i4] == (*i).prasTexture->pxf.ppalAttached->u4GetHashValue() && 
						 ppal[i4] == (*i).prasTexture->pxf.ppalAttached )
					{
						b_found = true;
						break;
					}
				}

				if (!b_found)
				{
					u4_palid[u4_next_pal] = (*i).prasTexture->pxf.ppalAttached->u4GetHashValue();
					ppal[u4_next_pal] = (*i).prasTexture->pxf.ppalAttached;
					u4_next_pal++;
				}
			}
		}
	}


	//******************************************************************************************
	//
	// Go through the list of parents of curved bump maps and pack them. These maps are going 
	// to be at the end of the VM image because nothing can get packed after here. Also nothing
	// can get loaded after this.
	//
	// Parent bump maps are bump maps that are the source of curved/unique bump maps. These
	// maps although packed should never be referenced or loaded. They are only here to keep
	// the loader happy.
	//
	dprintf("Packing parent bump maps...\n");

	// if there is no parent bump list then there is nothing to pack
	if (pcplParentBumpMaps)
	{
		// go through all of the packed bump maps and check for ones that are not packed
		for (j = (*pcplParentBumpMaps).begin(); j != (*pcplParentBumpMaps).end() ; j++)
		{
			// if the map is not already packed, process it.
			// NOTE:	The mapp cannot be already packed because we only call this function once and duplicates have
			//			been removed.
			if ((*j).bPacked == false)
			{
				// pack the source bump map in a surface type of a curved parent
				rptr<CRasterMem> pras_packed;// = prasFindPackLocation( (*j).prasBumpMap, eptCURVED_PARENT);


				if ( ((*j).prasBumpMap->iWidth>256) || ((*j).prasBumpMap->iHeight>256) )
				{
					//
					// allocate a raster in virtual memory, it is too big to allocate from a
					// pack surface or if it is curved.
					//
					pras_packed = rptr_cast(CRasterMem, rptr_new CRasterMem((*j).prasBumpMap->iWidth, 
																			(*j).prasBumpMap->iHeight,
																			(*j).prasBumpMap->iPixelBits,
																			0,
																			(CPixelFormat*)&(*j).prasBumpMap->pxf,
																			emtTexManVirtual) );
				}
				else
				{
					//
					// allocate a raster in an existing pack surface...
					//
					pras_packed = prasFindPackLocation( (*j).prasBumpMap, eptCURVED_PARENT);
				}


				// if we get a packed raster we have succeded, so mark the surface as packed
				if (pras_packed)
				{
					// cast away the STL const on the SCurvedParentElement structure.
					SCurvedParentElement* cpe = (SCurvedParentElement*)&(*j);

					// change the packed raster to be like pras_packed
					cpe->prasBumpMap->ChangeRaster( pras_packed );

					// we have chnaged the raster so update the tiling masks
					cpe->prasBumpMap->UpdateTilingMasks();

					// mark this element as packed.
					cpe->bPacked = true;
				}
				else
				{
					AlwaysAssert(0);
				}
			}
			else
			{
				// we cannot have already packed this map
				Assert(0);
			}

			u4_parent_bump++;
		}
	}


	//******************************************************************************************
	//
	// Save out the directory file
	//
	int32					i4_count;
	uint32					u4_bytes;
	HANDLE					h_save;
	SDirectoryFileHeader	dfh;
	SDirectoryFileChunk*	pdfc = NULL;

	dprintf("Creating PID file...\n");
	// Create the save file
	h_save=CreateFile(str_dir_name,GENERIC_WRITE,0,NULL,
				CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,NULL);

	if (h_save==INVALID_HANDLE_VALUE)
	{
		AlwaysAssert(0);
		return;
	}

	//******************************************************************************************
	//
	// Check how many of the logged textures are still in system memory, this should
	// only apply to bump maps. You cannot get an entry in this list for a texture that
	// is in system memory.
	//
	uint32	u4_tex_valid = 0;
	for (i = (*pvilVirtualImageMaps).begin(); i != (*pvilVirtualImageMaps).end() ; i++)
	{
		if (!gtxmTexMan.pvmeTextures->bVirtualAddressValid( (*i).prasTexture->pSurface ) )
		{
			// address is invalid, this has got to be a bump map...
			AlwaysAssert( (*i).ptexTexture->seterfFeatures[erfBUMP] );
			continue;
		}

		u4_tex_valid++;
	}

	// we should have counted all the elements in the list
	if (u4_tex_valid != ((*pvilVirtualImageMaps).size()) )
	{
		dprintf("ERROR: SWP file invalid - unpacked bump maps\n");
	}


	dprintf("Creating PID directory header...\n");
	//******************************************************************************************
	//
	// Directory file header...
	//
	dfh.u4Version				= u4DIRECTORY_FILE_VERSION;
	dfh.u4BumpMapBitDepth		= iBUMPMAP_RESOLUTION;
	dfh.u4RasterChunkOffset		= 0;
	if (pcplParentBumpMaps)
		dfh.u4RasterChunkCount	= u4_tex_valid + ((*pcplParentBumpMaps).size());
	else
		dfh.u4RasterChunkCount		= u4_tex_valid;
	dfh.u4PaletteOffset			= sizeof(dfh);
	dfh.u4PaletteCount			= u4_next_pal;
	dfh.u4PageableOffset		= pvmsaSmallTextures->u4AllocatorLength();
	dfh.u4NonPageableCount		= pvmsaSmallTextures->u4MemoryAllocated();


	//******************************************************************************************
	//
	// Write out the partial header, the offset to the rasters in not complete
	//
	if (WriteFile(h_save,&dfh,sizeof(dfh),(DWORD*)&u4_bytes,NULL) == false)
	{
		CloseHandle(h_save);
		AlwaysAssert(0);
		return;
	}
	AlwaysAssert(u4_bytes == sizeof(dfh));


	//******************************************************************************************
	//
	// Write out all of the palettes
	//
	dprintf("Writing ordered/unique palettes...\n");

	for (uint32 u4_pal = 0; u4_pal<u4_next_pal; u4_pal++)
	{
		Assert(ppal[u4_pal]);

		uint32					u4_size = sizeof(SDirectoryPaletteChunk) + 
											(ppal[u4_pal]->aclrPalette.uLen * sizeof(CColour));
		SDirectoryPaletteChunk*	pdpc = (SDirectoryPaletteChunk*) new char[u4_size];

		pdpc->u4Size		= u4_size;
		pdpc->u4ColoursUsed	= ppal[u4_pal]->aclrPalette.uLen;
		pdpc->u4HashValue = ppal[u4_pal]->u4GetHashValue();
		memcpy(&pdpc->clr[0], &ppal[u4_pal]->aclrPalette[0], ppal[u4_pal]->aclrPalette.uLen * sizeof(CColour) );

		//
		// Write out the palette chunk
		//
		if (WriteFile(h_save,pdpc,u4_size,(DWORD*)&u4_bytes,NULL) == false)
		{
			CloseHandle(h_save);
			AlwaysAssert(0);
			return;
		}
		AlwaysAssert(u4_bytes == u4_size);

		delete pdpc;
	}


	//******************************************************************************************
	//
	//Fix up the file header so the offset to the raster chunks is correct
	// 
	dprintf("Updating directory header...\n");

	uint32	u4_current_pos = SetFilePointer(h_save,0,NULL,FILE_CURRENT);

	SetFilePointer(h_save,0,NULL,FILE_BEGIN);
	dfh.u4RasterChunkOffset = u4_current_pos;

	if (WriteFile(h_save,&dfh,sizeof(dfh),(DWORD*)&u4_bytes,NULL) == false)
	{
		CloseHandle(h_save);
		AlwaysAssert(0);
		return;
	}
	AlwaysAssert(u4_bytes == sizeof(dfh));

	// put the file pointer back to where it was.
	SetFilePointer(h_save,u4_current_pos,NULL,FILE_BEGIN);


	//******************************************************************************************
	//
	//Write out the raster chunks
	//
	dprintf("Writing texture directory entries...\n");

	for (i = (*pvilVirtualImageMaps).begin(); i != (*pvilVirtualImageMaps).end() ; i++)
	{
		if (!gtxmTexMan.pvmeTextures->bVirtualAddressValid( (*i).prasTexture->pSurface ) )
			continue;

		if ( (*i).ptexTexture->seterfFeatures[erfBUMP] )
		{
			u4_bump_map++;
		}
		else
		{
			u4_texture++;
		}

		uint32 u4_size = sizeof(SDirectoryFileChunk);
		pdfc = (SDirectoryFileChunk*) new char[u4_size];

		// fill in the allocated file chunk
		pdfc->u4Size		= u4_size;
		pdfc->u4VMOffset	= (uint32)(*i).prasTexture->pSurface - (uint32)gtxmTexMan.pvmeTextures->pvGetBase();
		pdfc->u4Width		= (*i).prasTexture->iWidth;
		pdfc->u4Height		= (*i).prasTexture->iHeight;
		pdfc->u4Stride		= (*i).prasTexture->iLineBytes();
		pdfc->u4Bits		= (*i).prasTexture->iPixelBits;
		pdfc->clrConstCol	= (*i).prasTexture->clrFromPixel( (*i).ptexTexture->tpSolid );
		pdfc->iTransparent	= (*i).ptexTexture->seterfFeatures[erfTRANSPARENT];
		pdfc->iBumpMap		= (*i).ptexTexture->seterfFeatures[erfBUMP];
		pdfc->iOcclusion	= (*i).ptexTexture->seterfFeatures[erfOCCLUDE];
		pdfc->u8HashValue	= (*i).u8HashValue;

		//
		// set the palette ID
		//
		if ((*i).prasTexture->pxf)
		{
			if ((*i).prasTexture->pxf.ppalAttached)
			{
				bool b_found = false;

				for (i4_count = 0; i4_count<(int32)u4_next_pal; i4_count++)
				{
					if ( u4_palid[i4_count] == (*i).prasTexture->pxf.ppalAttached->u4GetHashValue() &&  
						ppal[i4_count] == (*i).prasTexture->pxf.ppalAttached )
					{
						b_found = true;
						break;
					}
				}

				// we must always find the palette
				AlwaysAssert(b_found);

				pdfc->u4Palette = (uint32)i4_count;
			}
			else
			{
				pdfc->u4Palette	= 0xffffffff;
			}
		}
		else
		{
			pdfc->u4Palette	= 0xffffffff;
		}

		//
		// Write out the chunk
		//
		if (WriteFile(h_save,pdfc,u4_size,(DWORD*)&u4_bytes,NULL) == false)
		{
			CloseHandle(h_save);
			AlwaysAssert(0);
			return;
		}
		AlwaysAssert(u4_bytes == u4_size);

		// delete and go around to the next
		delete pdfc;
		pdfc = NULL;
	}

	//******************************************************************************************
	//
	// The bumps maps below are the maps that are the source or parent of a curved map. These
	// source maps themselves are neve referenced or used. The parent bump maps that are used
	// as flat bump maps on other faces/meshes should have been removed from this list.
	// The hash value of these parent faces is the same as any other top level texture map,
	// the bottom 32 bits being the hash of the name and the top 32 bits being zero.
	// These are packed as normal rasters, which are packed at the end of the VM image so 
	// they never get loaded and never have to be seeked over. These textures have an invalid
	// constant colour and the default palette. These textures are never drawn and are only
	// present so the loader does not fail.
	//
	// NOTE:	CURVED TEXTURES THAT ARE USED ELSEWHERE SHOULD NOT BE IN THIS LIST
	//
	dprintf("Writing parent bump map directory entries...\n");

	if (pcplParentBumpMaps)
	{
		for (j = (*pcplParentBumpMaps).begin(); j != (*pcplParentBumpMaps).end() ; j++)
		{
			uint32 u4_size = sizeof(SDirectoryFileChunk);
			pdfc = (SDirectoryFileChunk*) new char[u4_size];

			// fill in the allocated file chunk
			pdfc->u4Size		= u4_size;
			pdfc->u4VMOffset	= (uint32)(*j).prasBumpMap->pSurface - (uint32)gtxmTexMan.pvmeTextures->pvGetBase();
			pdfc->u4Width		= (*j).prasBumpMap->iWidth;
			pdfc->u4Height		= (*j).prasBumpMap->iHeight;
			pdfc->u4Stride		= (*j).prasBumpMap->iLineBytes();
			pdfc->u4Bits		= (*j).prasBumpMap->iPixelBits;
			pdfc->clrConstCol	= 0xffffffff;
			pdfc->iBumpMap		= 1;
			pdfc->iTransparent	= (*j).bTransparent;
			pdfc->u8HashValue	= (uint64) ((*j).u4HashValue);
			pdfc->iOcclusion	= 0;

			//
			// set the palette ID
			//
			if ((*j).prasBumpMap->pxf)
			{
				if ((*j).prasBumpMap->pxf.ppalAttached)
				{
					bool b_found = false;

					for (i4_count = 0; i4_count<(int32)u4_next_pal; i4_count++)
					{
						if ( u4_palid[i4_count] == (*j).prasBumpMap->pxf.ppalAttached->u4GetHashValue() &&  
							ppal[i4_count] == (*j).prasBumpMap->pxf.ppalAttached )
						{
							b_found = true;
							break;
						}
					}

					// we must always find the palette
					AlwaysAssert(b_found);

					pdfc->u4Palette = (uint32)i4_count;
				}
				else
				{
					pdfc->u4Palette	= 0xffffffff;
				}
			}
			else
			{
				pdfc->u4Palette	= 0xffffffff;
			}

			//
			// Write out the chunk
			//
			if (WriteFile(h_save,pdfc,u4_size,(DWORD*)&u4_bytes,NULL) == false)
			{
				CloseHandle(h_save);
				AlwaysAssert(0);
				return;
			}
			AlwaysAssert(u4_bytes == u4_size);

			// delete and go around to the next
			delete pdfc;
			pdfc = NULL;
		}
	}

	CloseHandle(h_save);

	dprintf("********************************************************\n");
	dprintf("Packed Image Directory Created\n");
	dprintf("%d Palettes\n",u4_next_pal);
	dprintf("%d Texture maps\n",u4_texture);
	dprintf("%d Bump maps\n",u4_bump_map);
	dprintf("%d Curved bump map parents\n",u4_parent_bump);
}

