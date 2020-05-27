/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of texture packing.
 *
 * This will pack square and rectangular textures in to a texture page 256x256. Each texture
 * surface is 2 texture pages which are interleaved on a pixel byte interval and therefore the
 * stride of each texture surface is 512 pixels. The stride of 512 is VERY important because
 * it is what gives us the capability of tiling textures, these textures are created by having
 * an empty bit at bit 8 which is ignored by the texture walking but is used when the texel
 * address is calculated. This results in 256 texels and then a gap of 256 texels, this leads
 * to a width of 256, a stride of 512 and a second texture page that starts 256 bytes after
 * the first.
 *
 * The structures used by this packer, mainly the quad tree, are aligned to a bytes boundary.
 * This is so memory overhead in minimal and performance is not essential here although it
 * still has to be very fast. From an individual model there will only be about 10 textures
 * that are loaded so the unaligned access will not cause any noticable decrease in speed.
 *
 * NOTE: The data in the packed surface is not affected by any operation within CPackedRaster
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/TexturePackSurface.cpp                                     $
 * 
 * 18    8/26/98 5:20p Asouth
 * Fixed loop-scoping error
 * 
 * 17    4/25/98 8:07p Agrant
 * Do not print out for every texture
 * 
 * 16    4/24/98 4:08p Rwyatt
 * Fixed assert if a 8x256 map was packed first in a page.
 * 
 * 15    4/21/98 3:12p Rwyatt
 * New pack surface type for smallest mip maps.
 * New logic to set the smallest mip maps
 * 
 * 14    3/30/98 11:13p Rwyatt
 * Modified to allow a parent surface that is not 256x256. This is still themaximum size but now
 * any power of 2 and sqaure raster can be used,
 * There is also a minimum now a minimum size that all smaller allocations are rounded upto.
 * 
 * 13    3/17/98 7:59p Rwyatt
 * Added a few asserts
 * 
 * 12    3/17/98 7:53p Rwyatt
 * Added new functions that allow textures to be packed at a specified location.
 * 
 * 11    3/11/98 3:50p Rwyatt
 * Fixed a serious bug in the delete function.
 * 
 * 10    3/06/98 8:41p Rwyatt
 * The check for less than 256 is now an always assert because larger maps cause all sorts of
 * problems.
 * 
 * 9     2/24/98 6:55p Rwyatt
 * CTexturePackSurface has been split into two seperate classes. The new class CPackedRaster
 * does the packing on a single raster and the old class has been modifed to suit.
 * 
 * 8     1/29/98 7:41p Rwyatt
 * New mip settings for special cases such as curved bump map parents
 * 
 * 7     1/14/98 6:25p Rwyatt
 * Modified very incorrect comment
 * 
 * 6     1/09/98 7:04p Rwyatt
 * Each pack surface has a base mip level which is used to determine what can be packed in it.
 * The default is don't care
 * 
 * 5     12/04/97 4:14p Rwyatt
 * Packed rasters are now allocated from virtual memory
 * Removed some memory counters
 * 
 * 4     9/01/97 7:57p Rwyatt
 * Delete member can now handle any surfcaes of any bit depth
 * 
 * 3     8/19/97 7:03p Rwyatt
 * Sub allocated mem rasters (such as textures) now have an rpt back to the parent Texture Pack
 * surface. This is so the packer can be infomred when the texture is deleted, it is an rptr to
 * avoid static destructor dependencies.
 * 
 * 2     7/29/97 2:01a Rwyatt
 * Latest packer, this version has no knowledege about the thing that represents the sub region.
 * No texture pointer is stored in the quad node - just a 0/1 used flag which is now a byte to
 * keep the node size minimum.
 * Packing is set 1
 * 
 * 1     7/29/97 1:00a Rwyatt
 * Initial implementation
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/View/Raster.hpp"
#include "TexturePackSurface.hpp"
#include "Lib/sys/MemoryLog.hpp"
#include "Lib/sys/DebugConsole.hpp"


//*********************************************************************************************
//
#if (VER_DEBUG == TRUE)
static bool AssertEmptyTree(STextureQuadTree* ptqt)
{
	// is the root texture used??
	if (ptqt->tqnRoot.u1Texture)
		return false;

	// are any of the children nodes used??
	if (ptqt->tqnRoot.ptqnSubNode[0])
		return false;

	if (ptqt->tqnRoot.ptqnSubNode[1])
		return false;

	if (ptqt->tqnRoot.ptqnSubNode[2])
		return false;

	if (ptqt->tqnRoot.ptqnSubNode[3])
		return false;

	// tree is free, so return true
	return true;
}
#else
_inline static bool AssertEmptyTree(STextureQuadTree* ptqt)
{
	return true;
}
#endif




//*********************************************************************************************
//
CPackedRaster::CPackedRaster
(
	rptr<CRaster>	pras_parent,
	uint32			u4_smallest
)
//*************************************
{
	// The raster passed in must be valid and must have a valid width and height (256x256).
	// NOTE:	The parent raster does require a valid surface pointer. The surface is never
	//			accessed because it may be in virtual memory. All packed rasters have a surface
	//			pointer relative to the base address of the parent.
	Assert( pras_parent );
	Assert( u4_smallest>0);

	Assert( pras_parent->iWidth >= u4_smallest);
	Assert( pras_parent->iHeight >= u4_smallest);
	Assert( pras_parent->iWidth <= 256);
	Assert( pras_parent->iHeight <= 256);
	Assert( pras_parent->iHeight == pras_parent->iWidth);

	u4SmallestAllocation = u4_smallest;		// all allocations are rounded upto this size
	prasPackedParent = pras_parent;

	InitQuadTree(pras_parent->iWidth, u4_smallest);

	// we do not know what the mip level or texture type of this surface is.
	eptMipLevel = eptDONT_CARE;

	//
	// A little memory logging....
	//
#ifdef LOG_MEM
	switch (pras_parent->iPixelBits)
	{
	case 8:
		MEMLOG_ADD_COUNTER(emlTexturePages8,1);
		break;

	case 16:
		MEMLOG_ADD_COUNTER(emlTexturePages16,1);
		break;

	case 32:
		MEMLOG_ADD_COUNTER(emlTexturePages32,1);
		break;
	}
#endif
}



//*********************************************************************************************
//
CPackedRaster::~CPackedRaster
(
)
//*************************************
{
	if (!AssertEmptyTree(&tqtPackQuadTree))
	{
		dprintf("Textures remain in surface\n");
	}

	//
	// A little memory logging....
	//
#ifdef LOG_MEM
	switch (prasPackedParent->iPixelBits)
	{
	case 8:
		MEMLOG_SUB_COUNTER(emlTexturePages8,1);
		break;

	case 16:
		MEMLOG_SUB_COUNTER(emlTexturePages16,1);
		break;

	case 32:
		MEMLOG_SUB_COUNTER(emlTexturePages32,1);
		break;
	}
#endif

	//
	// Delete the free lists, note the first 3 lists are statically allocated so do not
	// try to delete them.
	//
	for (int i = tqtPackQuadTree.u4ParentSize-3; i>=0; i--)
	{
		delete tqtPackQuadTree.anltFree[ i ].aptqn;
	}
}



//*********************************************************************************************
// Init a quad tree withe an empty root node. This must be called before the tree is used.
//
void CPackedRaster::InitQuadTree
(
	uint32	u4_parent,
	uint32	u4_smallest
)
//*************************************
{
	int	i_parent_size = iGetSize(u4_parent);
	int i_small_size = iGetSize(u4_smallest);

	// setup the root node
	tqtPackQuadTree.u4ParentSize		= (uint32)i_parent_size;
	tqtPackQuadTree.tqnRoot.u1Size		= (uint8)u4_parent-1;
	tqtPackQuadTree.tqnRoot.u1XOrg		= 0;
	tqtPackQuadTree.tqnRoot.u1YOrg		= 0;

	tqtPackQuadTree.tqnRoot.u1Texture	= 0;
	tqtPackQuadTree.tqnRoot.ptqnLink	= NULL;

	// no children, the children of a node are not allocated until the node needs to be split
	// and at that point all 4 children are created.
	tqtPackQuadTree.tqnRoot.ptqnSubNode[0]	= NULL;
	tqtPackQuadTree.tqnRoot.ptqnSubNode[1]	= NULL;
	tqtPackQuadTree.tqnRoot.ptqnSubNode[2]	= NULL;
	tqtPackQuadTree.tqnRoot.ptqnSubNode[3]	= NULL;

	// Empty every free list and null the pointers, all lists are now invalid and no size
	// can be allocated
	for (int k = 0 ; k<=8 ; k++)
	{
		tqtPackQuadTree.anltFree[ k ].u1Count = 0;
		tqtPackQuadTree.u4LostNodeCount[ k ] = 0;
		tqtPackQuadTree.anltFree[ k ].aptqn = NULL;
	}

	// allocate and initialize all free lists that are within the valid size range
	for (int i = i_parent_size, j=0 ; i>=i_small_size ; i--,j++)
	{
		uint8	u1_max = 0;
		tqtPackQuadTree.anltFree[ i ].u1Count = 0;
		tqtPackQuadTree.u4LostNodeCount[ i ] = 0;

		switch (j)
		{
		case 0:
			u1_max = u4FREE_LIST_ENTRIES_PARENT;
			tqtPackQuadTree.anltFree[ i ].aptqn = atqnSizeParent;
			break;

		case 1:
			u1_max = u4FREE_LIST_ENTRIES_DIVIDE1;
			tqtPackQuadTree.anltFree[ i ].aptqn = atqnSizeDivide1;
			break;

		case 2:
			u1_max = u4FREE_LIST_ENTRIES_DIVIDE2;
			tqtPackQuadTree.anltFree[ i ].aptqn = atqnSizeDivide2;
			break;

		case 3:
			u1_max = u4FREE_LIST_ENTRIES_DIVIDE3;
			tqtPackQuadTree.anltFree[ i ].aptqn = (STextureQuadNode**) new char[sizeof(STextureQuadNode*)*u1_max];
			break;

		case 4:
			u1_max = u4FREE_LIST_ENTRIES_DIVIDE4;
			tqtPackQuadTree.anltFree[ i ].aptqn = (STextureQuadNode**) new char[sizeof(STextureQuadNode*)*u1_max];
			break;

		case 5:
			u1_max = u4FREE_LIST_ENTRIES_DIVIDE5;
			tqtPackQuadTree.anltFree[ i ].aptqn = (STextureQuadNode**) new char[sizeof(STextureQuadNode*)*u1_max];
			break;

		case 6:
			u1_max = u4FREE_LIST_ENTRIES_DIVIDE6;
			tqtPackQuadTree.anltFree[ i ].aptqn = (STextureQuadNode**) new char[sizeof(STextureQuadNode*)*u1_max];
			break;

		case 7:
			u1_max = u4FREE_LIST_ENTRIES_DIVIDE7;
			tqtPackQuadTree.anltFree[ i ].aptqn = (STextureQuadNode**) new char[sizeof(STextureQuadNode*)*u1_max];
			break;

		case 8:
			u1_max = u4FREE_LIST_ENTRIES_DIVIDE8;
			tqtPackQuadTree.anltFree[ i ].aptqn = (STextureQuadNode**) new char[sizeof(STextureQuadNode*)*u1_max];
			break;
		}

		tqtPackQuadTree.anltFree[ i ].u1MaxCount = u1_max;

		// zero the new free list
		for (uint32 k=0; k<u1_max; k++)
		{
			tqtPackQuadTree.anltFree[i].aptqn[k] = 0;
		}
	}

	// setup the parent list
	tqtPackQuadTree.anltFree[i_parent_size].u1Count = 1;
	tqtPackQuadTree.anltFree[i_parent_size].aptqn[0] = &tqtPackQuadTree.tqnRoot;
}


//*********************************************************************************************
//
rptr<CRasterMem> CPackedRaster::prasAllocateRaster
(
	uint32			u4_xsize,
	uint32			u4_ysize,
	CPixelFormat*	pxf
)
//*************************************
{
	uint32 u4_powx = NextPowerOfTwo(u4_xsize);
	uint32 u4_powy = NextPowerOfTwo(u4_ysize);

	//
	// So lets try to add the map to this surface.
	// (find a block of the correct size in the quad tree
	// Sizes are rounded up to the next power of two)
	//
	STextureQuadNode*	ptqn = ptqnAddRaster(u4_powx,u4_powy);

	// failed to allocate, return a NULL rptr
	if (ptqn == NULL)
	{
		return rptr0;
	}

	// log the rounding waste
	MEMLOG_ADD_COUNTER(emlRoundingWaste,((u4_powx*u4_powy)- (u4_xsize*u4_ysize)) * 
											prasGetParentRaster()->iPixelBytes() );

	// make a rectangle from the info returned in the quad tree and use this rectange
	// to allocate a section of the parent.
	// NOTE: The texture quad node returned above is the square in the top left hand
	// corner of the rectangle that was requested. For example, if a 256x128 rectangle
	// is requested the reselting node is 128x128. This is not a bug, the packer knows
	// that the neighbouring 128x128 is also used.
	SRect rc(ptqn->u1XOrg, ptqn->u1YOrg, u4_xsize, u4_ysize);

	return rptr_cast(CRasterMem, rptr_new CRasterMem(	prasGetParentRaster(), 
														rc,
														rptr_this(this),
														pxf
													));
}


//*********************************************************************************************
//
bool CPackedRaster::bFindNextHorizontal
(	
	STextureQuadNode*	aptqn[],
	uint32				au4[],
	uint32				u4_count,
	uint32*				pu4_adjcount,
	uint32*				pu4_amin,
	uint32*				pu4_amax,
	uint32				i_pixels
)
//*************************************
{
	if (*pu4_adjcount == 0)
	{
		//
		// Find the first good node and insert it into the index array
		//

		// while there is entries in the list, find the first good node thatwe can use.
		while (u4_count)
		{
			u4_count--;
			// if this node set??
			if (aptqn[u4_count])
			{
				au4[0] = u4_count;			// index of the first good one
				*pu4_adjcount = 1;			// number in list
				*pu4_amin = u4_count;		// index in index array of the minimum point
				*pu4_amax = u4_count;		// index in index array of the maximum point

				return true;
			}
		}

		return false;
	}
	else
	{

		// starting block, this will get decremented
		// this will get decremented so we actaully start 1 before it.
		u4_count = au4[0];

		// while there are entries in the list, find the first good node that we can use.
		while (u4_count)
		{
			u4_count--;

			// if this node set??
			if (aptqn[u4_count])
			{
				if ( (aptqn[u4_count]->u1YOrg) == (aptqn[ au4[0] ]->u1YOrg) )
				{
					if ( (uint32)aptqn[u4_count]->u1XOrg == ((uint32)aptqn[ *pu4_amax ]->u1XOrg + i_pixels) )
					{
						// this is our neighbour, 1 to the right
						au4[ *pu4_adjcount ] = u4_count;
						(*pu4_amax) = u4_count;
						(*pu4_adjcount)++;

						return true;
					}

					if ( (uint32)aptqn[u4_count]->u1XOrg == ((uint32)aptqn[ *pu4_amin ]->u1XOrg - i_pixels) )
					{
						// this is our neighbour, 1 to the right
						au4[ *pu4_adjcount ] = u4_count;
						(*pu4_amin) = u4_count;
						(*pu4_adjcount)++;

						return true;
					}
				}
			}
		}
	}
	return false;
}




//*********************************************************************************************
//
bool CPackedRaster::bFindNextVertical
(	
	STextureQuadNode*	aptqn[],
	uint32				au4[],
	uint32				u4_count,
	uint32*				pu4_adjcount,
	uint32*				pu4_amin,
	uint32*				pu4_amax,
	uint32				i_pixels
)
//*************************************
{
	if (*pu4_adjcount == 0)
	{
		//
		// Find the first good node and insert it into the index array
		//

		// while there is entries in the list, find the first good node thatwe can use.
		while (u4_count)
		{
			u4_count--;
			// if this node set??
			if (aptqn[u4_count])
			{
				au4[0] = u4_count;			// index of the first good one
				*pu4_adjcount = 1;			// number in list
				*pu4_amin = u4_count;		// index in index array of the minimum point
				*pu4_amax = u4_count;		// index in index array of the maximum point

				return true;
			}
		}
		return false;
	}
	else
	{
		// starting block, this will get decremented
		// this will get decremented so we actaully start 1 before it.
		u4_count = au4[0];

		// while there are entries in the list, find the first good node that we can use.
		while (u4_count)
		{
			u4_count--;
			// if this node set??
			if (aptqn[u4_count])
			{
				if ( (aptqn[u4_count]->u1XOrg) == (aptqn[ au4[0] ]->u1XOrg) )
				{
					if ( (uint32)aptqn[u4_count]->u1YOrg == ((uint32)aptqn[  *pu4_amax ]->u1YOrg + i_pixels) )
					{
						// this is our neighbour, 1 below
						au4[ *pu4_adjcount ] = u4_count;
						(*pu4_amax) = u4_count;
						(*pu4_adjcount)++;

						return true;
					}

					if ( (uint32)aptqn[u4_count]->u1YOrg == ((uint32)aptqn[ *pu4_amin ]->u1YOrg - i_pixels) )
					{
						// this is our neighbour, 1 above
						au4[ *pu4_adjcount ] = u4_count;
						(*pu4_amin) = u4_count;
						(*pu4_adjcount)++;

						return true;
					}
				}
			}
		}
	}
	return false;
}


//*********************************************************************************************
// Add the specified texture to this texture page
// NOTE: The node that this returns is smallest square that fits into the top left hand corner
// of the area requested. This is not a bug is is down to the way rectangles are broken into
// squares with sides equal to the minor side of the rectangle.
//
STextureQuadNode* CPackedRaster::ptqnAddRaster
(
	int i_width, 
	int i_height
)
//*************************************
{
	int		i_size;
	int		i_big;
	int		i_small;
	int		i_blocks;
	int		i_pixels;
	bool	b_vertical;

	Assert(bPowerOfTwo(i_width));
	Assert(bPowerOfTwo(i_height));

//	dprintf("WIDTH = %d\nHEIGHT = %d\n\n", i_width, i_height);

	if (i_width<(int)u4SmallestAllocation)
	{
		i_width = (int)u4SmallestAllocation;
	}

	if (i_height<(int)u4SmallestAllocation)
	{
		i_height = (int)u4SmallestAllocation;
	}

	// which free list are we to use first (the list of the biggest size)
	i_size = iGetSize(i_width>i_height?i_width:i_height);

	// if the width and height are equal then allocate directly...
	if (i_width == i_height)
	{
		STextureQuadNode*	ptqn = ptqnFindQuadNode( iGetSize(i_width) );

		if (ptqn == NULL)
			return NULL;

		// fill in the info within the quad node
		ptqn->u1Texture = 1;
		ptqn->ptqnLink = NULL;

		return ptqn;
	}

	// get the dimensions and number of sub blocks that we need to allocate....
	if (i_width<i_height)
	{
		i_blocks = i_height/i_width;		// number of neighboring blocks
		i_size = iGetSize(i_width);			// at this size
		i_small = i_size;
		i_big = iGetSize(i_height);
		i_pixels = i_width;
		b_vertical = true;
	}
	else
	{
		i_blocks = i_width/i_height;		// number of neighboring blocks
		i_size = iGetSize(i_height);		// at this size
		i_small = i_size;
		i_big = iGetSize(i_width);			
		i_pixels = i_height;
		b_vertical = false;
	}

	return ptqnAddSubTexture
	(
		i_width,
		i_height,
		i_blocks,
		i_size,
		i_small,
		i_big,
		i_pixels,
		b_vertical
	);
}



//*********************************************************************************************
//
STextureQuadNode* CPackedRaster::ptqnAddSubTexture
(
	int					i_width, 
	int					i_height,
	int					i_blocks,
	int					i_size,
	int					i_small,
	int					i_big,
	int					i_pixels,
	bool				b_vertical
)
//*************************************
{
	STextureQuadNode*	aptqn_adjcopy[u4LOCAL_FREE_LIST_SIZE];
	uint32				u4_list[u4LOCAL_FREE_LIST_SIZE];

	// Make sure we have a free list for this size, if there is no free list then this size
	// should have not been used as it is less than the smallest allocation size.
	Assert(tqtPackQuadTree.anltFree[i_size].aptqn);

	// while there is still a search that is not the size of the master square
	while (i_blocks>1)
	{
		uint32	u4_count = tqtPackQuadTree.anltFree[i_size].u1Count;		// number of blocks

		if (u4_count >= (uint32)i_blocks)
		{
			uint32	u4_adjcount = 0;	
			uint32	u4_amin		= 0;
			uint32  u4_amax		= 0;
			bool	b_res;

			// make sure we do not overrun the local free lists obove..
			Assert(u4_count<=u4LOCAL_FREE_LIST_SIZE);

			// make a local copy of the free list
			for (uint32 u4 = 0; u4< u4_count; u4++)
			{
				aptqn_adjcopy[u4] = tqtPackQuadTree.anltFree[i_size].aptqn[u4];
			}

			while (1)
			{

				if (b_vertical)
				{
					b_res = bFindNextVertical(aptqn_adjcopy,u4_list,u4_count,&u4_adjcount,&u4_amin,&u4_amax,i_pixels);
				}
				else
				{
					b_res = bFindNextHorizontal(aptqn_adjcopy,u4_list,u4_count,&u4_adjcount,&u4_amin,&u4_amax,i_pixels);
				}

				if (!b_res)
				{
					// we have not returned any blocks this time so the ones currently 
					// in the list are not valid. If we never inserted a start block
					// then we are at the end and have not found what we are looking for.
					//
					if (u4_adjcount == 0)
						break;

					// go through the list and remove any that cannot be used
					for (uint32 i = 0; i<u4_adjcount; i++)
					{
						aptqn_adjcopy[ u4_list[i] ] = NULL;
					}

					// we have nothing in our list now which is the same as when we started.
					// Go back to the loop start and re-iterate and we will ignore the ones
					// that we have just deleted.
					u4_adjcount = 0;	
					continue;
				}

				// is our new adjacent list big enough??
				if ((int)u4_adjcount >= i_blocks)
				{
					int i;

					if (i_size == i_small)
					{
						//
						// we have a suitable set of blocks that are the same size in one
						// dimension as the source block, therefore no subdivision is required.
						// delete the N nodes from the free list and set the texture pointer
						//
						// THIS IS THE ONLY PLACE WHERE THE LINK IS SET TO NON-NULL
						//

						// copy the first element to the last just so we can point to it!
						u4_list[i_blocks] = u4_list[0];

						for (i=0; i<i_blocks; i++)
						{
							KillNode(aptqn_adjcopy[ u4_list[i] ], i_size ,false);
							aptqn_adjcopy[ u4_list[i] ]->u1Texture = 1;
							aptqn_adjcopy[ u4_list[i] ]->ptqnLink = aptqn_adjcopy[ u4_list[i+1] ];
						}
					}
					else
					{
						// we have a set of blocks that make up an area of the correct dimensions
						// but it needs to be subdivided, so lets do that....
						for (i=0; i<i_blocks; i++)
						{
							KillNode(aptqn_adjcopy[ u4_list[i] ], i_size ,false);
							SplitQuadNodeForRectangle(aptqn_adjcopy[ u4_list[i] ], b_vertical, i_small, i_size-1);
						}

						// now add the texture again and it will fit!
						return ptqnAddRaster(i_width, i_height);
					}

					return aptqn_adjcopy[ u4_amin ];
				}
			}		
		}
		i_size++;							// go up a block size
		i_pixels <<= 1;
		i_blocks >>= 1;						// half the number of blocks
	}

	// if we get to here things are still hopeful..
	// we are down to one block so allocate it with the normal allocator and divide it down
	// we need to allocate a block at the master (bigger) size
	STextureQuadNode*	ptqn = ptqnFindQuadNode(i_big );
	if (ptqn == NULL)
		return NULL;

	SplitQuadNodeForRectangle(ptqn, b_vertical, i_small, i_size-1);

	return ptqnAddRaster(i_width, i_height);
}



//*********************************************************************************************
// The node passed in is not in the free list for that node size
//
// This will keep dividing until it is the size of the smaller of the two dimensions, at this
// point our rectangle can be added
//
void CPackedRaster::SplitQuadNodeForRectangle
(
	STextureQuadNode* ptqn, 
	bool b_vertical,		// set to true to split vertically (width<height)
	int i_small, 
	int i_size
)
//*************************************
{
	// splits into 4 children...
	DivideNode(ptqn, i_size);

	if (i_size != i_small)
	{
		// we are still not there so lets recurse
		if (b_vertical)
		{
			KillNode(ptqn->ptqnSubNode[3], i_size ,false);
			KillNode(ptqn->ptqnSubNode[1], i_size ,false);

			SplitQuadNodeForRectangle(ptqn->ptqnSubNode[3], b_vertical, i_small, i_size-1);
			SplitQuadNodeForRectangle(ptqn->ptqnSubNode[1], b_vertical, i_small, i_size-1);
		}
		else
		{
			KillNode( ptqn->ptqnSubNode[3], i_size ,false);
			KillNode(ptqn->ptqnSubNode[2], i_size ,false);

			SplitQuadNodeForRectangle(ptqn->ptqnSubNode[3], b_vertical, i_small, i_size-1);
			SplitQuadNodeForRectangle(ptqn->ptqnSubNode[2], b_vertical, i_small, i_size-1);
		}
	}
}



//*********************************************************************************************
// Divides  the specified node and adds the children to the free list
// This function has to handle running out of space in the free lists
//
void CPackedRaster::DivideNode
(
	STextureQuadNode* ptqn, 
	int i_size
)
//*************************************
{
	// set the parent sub pointers
	ptqn->ptqnSubNode[0] = new STextureQuadNode;
	ptqn->ptqnSubNode[1] = new STextureQuadNode;
	ptqn->ptqnSubNode[2] = new STextureQuadNode;
	ptqn->ptqnSubNode[3] = new STextureQuadNode;

	MEMLOG_ADD_ADRSIZE(emlTextureManQuad,ptqn->ptqnSubNode[0]);
	MEMLOG_ADD_ADRSIZE(emlTextureManQuad,ptqn->ptqnSubNode[1]);
	MEMLOG_ADD_ADRSIZE(emlTextureManQuad,ptqn->ptqnSubNode[2]);
	MEMLOG_ADD_ADRSIZE(emlTextureManQuad,ptqn->ptqnSubNode[3]);

	// set the free list  pointers
	for (int i = 0; i<4 ; i++)
	{
		if (tqtPackQuadTree.anltFree[i_size].u1Count < tqtPackQuadTree.anltFree[i_size].u1MaxCount )
		{
			tqtPackQuadTree.anltFree[i_size].aptqn[ tqtPackQuadTree.anltFree[i_size].u1Count ] = ptqn->ptqnSubNode[i];
			tqtPackQuadTree.anltFree[i_size].u1Count++;
		}
		else
		{
			break;
		}
	}

	// zero all the texture pointers
	ptqn->ptqnSubNode[0]->u1Texture = 0;
	ptqn->ptqnSubNode[1]->u1Texture = 0;
	ptqn->ptqnSubNode[2]->u1Texture = 0;
	ptqn->ptqnSubNode[3]->u1Texture = 0;

	// zero all the texture links
	ptqn->ptqnSubNode[0]->ptqnLink = 0;
	ptqn->ptqnSubNode[1]->ptqnLink = 0;
	ptqn->ptqnSubNode[2]->ptqnLink = 0;
	ptqn->ptqnSubNode[3]->ptqnLink = 0;

	// set the width of all the new nodes
	ptqn->ptqnSubNode[0]->u1Size = ((ptqn->u1Size+1)>>1)-1;
	ptqn->ptqnSubNode[1]->u1Size = ((ptqn->u1Size+1)>>1)-1;
	ptqn->ptqnSubNode[2]->u1Size = ((ptqn->u1Size+1)>>1)-1;
	ptqn->ptqnSubNode[3]->u1Size = ((ptqn->u1Size+1)>>1)-1;

	// node 0 is top left
	ptqn->ptqnSubNode[3]->u1XOrg = ptqn->u1XOrg;
	ptqn->ptqnSubNode[3]->u1YOrg = ptqn->u1YOrg;

	// node 1 is top right
	ptqn->ptqnSubNode[2]->u1XOrg = ptqn->u1XOrg + ((ptqn->u1Size+1)>>1);
	ptqn->ptqnSubNode[2]->u1YOrg = ptqn->u1YOrg;

	// node 2 is bottom left
	ptqn->ptqnSubNode[1]->u1XOrg = ptqn->u1XOrg;
	ptqn->ptqnSubNode[1]->u1YOrg = ptqn->u1YOrg + ((ptqn->u1Size+1)>>1);

	// node 3 is bottom right
	ptqn->ptqnSubNode[0]->u1XOrg = ptqn->u1XOrg + ((ptqn->u1Size+1)>>1);
	ptqn->ptqnSubNode[0]->u1YOrg = ptqn->u1YOrg + ((ptqn->u1Size+1)>>1);
}


//*********************************************************************************************
// Add the specified texture to the specified tree
//
STextureQuadNode* CPackedRaster::ptqnFindQuadNode
(
	int i_size 
)
//*************************************
{
	STextureQuadNode*	ptqn = NULL;

	// Make sure we have a free list for this size, if there is no free list then this size
	// should have not been used as it is less than the smallest allocation size.
	Assert(tqtPackQuadTree.anltFree[i_size].aptqn);

	// first move is to allocate from the cahce of free nodes...
	if (tqtPackQuadTree.anltFree[i_size].u1Count>0)
	{
		// we have a slot of this size that we can use.
		tqtPackQuadTree.anltFree[i_size].u1Count--;

		// return the address that we have found, remember to null the element
		ptqn = tqtPackQuadTree.anltFree[i_size].aptqn[ tqtPackQuadTree.anltFree[i_size].u1Count ];
		tqtPackQuadTree.anltFree[i_size].aptqn[ tqtPackQuadTree.anltFree[i_size].u1Count ] = NULL;
	}
	// second move is to see if there are any lost nodes of the requested size...
	else if (tqtPackQuadTree.u4LostNodeCount[i_size]>0)
	{
		ptqn = ptqnSearchFreeNode(&tqtPackQuadTree.tqnRoot, tqtPackQuadTree.u4ParentSize,i_size);
		if (ptqn)
		{
			KillNode(ptqn,i_size,false);
		}
	}
	// the final move is to search for a bigger node and sub divide it.
	else
	{
		// are we looking for the biggest size??
		if (i_size<(int)tqtPackQuadTree.u4ParentSize)
		{
			// it is time to up one a size and sub divide
			// zero is the texture handle so that we mark the parent as unused
			ptqn = ptqnFindQuadNode(i_size+1);

			// ptqn is the pointer to our parent node, or NULL if our request cannot be made
			if (ptqn != NULL)
			{
				DivideNode(ptqn, i_size);

				// allocate one of the quads allocated above
				ptqn = ptqnFindQuadNode(i_size);
			}
		}
	}

	return ptqn;
}


//*********************************************************************************************
// this will walk the tree looking for a free node, if it finds other free nodes that are not
// in the list it will add them - even for different sizes.
//
STextureQuadNode* CPackedRaster::ptqnSearchFreeNode
(
	STextureQuadNode*	ptqn,
	int					i_size,
	int					i_searchsize 
)
//*************************************
{
	STextureQuadNode*	ptqn_local = NULL;
	STextureQuadNode*	ptqn_temp = NULL;

	if ((ptqn->ptqnSubNode[0] == NULL) && (ptqn->ptqnSubNode[1] == NULL) && 
		(ptqn->ptqnSubNode[2] == NULL) && (ptqn->ptqnSubNode[3] == NULL) && (ptqn->u1Texture == 0))
	{
		bool b_res = false;

		// this node is free, check if it is in the free list
		for (uint32 u4=0; u4<(uint32)tqtPackQuadTree.anltFree[i_size].u1Count; u4++)
		{
			if (tqtPackQuadTree.anltFree[i_size].aptqn[u4] == ptqn)
			{
				Assert(b_res);			// if we are in the list twice we are screwed...
				b_res = true;
			}
		}

		// did we find it??
		if (!b_res)
		{
			// the node is not in the free list.
			Assert(tqtPackQuadTree.u4LostNodeCount[i_size]>0);

			// now check if there is room in the free list...
			if (tqtPackQuadTree.anltFree[i_size].u1Count<tqtPackQuadTree.anltFree[i_size].u1MaxCount)
			{
				// add us to the free list and decrement the counter.
				tqtPackQuadTree.anltFree[i_size].aptqn[ tqtPackQuadTree.anltFree[i_size].u1Count ] = ptqn;
				tqtPackQuadTree.anltFree[i_size].u1Count++;
				tqtPackQuadTree.u4LostNodeCount[i_size]--;
			}

			// if this is the size that we are looking for then return it
			if (i_size == i_searchsize)
			{
				ptqn_local = ptqn;
			}
		}
	}
	else
	{
		if (i_size>i_searchsize)
		{
			ptqn_temp = ptqnSearchFreeNode(ptqn->ptqnSubNode[0],i_size-1,i_searchsize);
			if (ptqn_local == NULL)
			{
				ptqn_local = ptqn_temp;
			}

			ptqn_temp = ptqnSearchFreeNode(ptqn->ptqnSubNode[1],i_size-1,i_searchsize);
			if (ptqn_local == NULL)
			{
				ptqn_local = ptqn_temp;
			}

			ptqn_temp = ptqnSearchFreeNode(ptqn->ptqnSubNode[2],i_size-1,i_searchsize);
			if (ptqn_local == NULL)
			{
				ptqn_local = ptqn_temp;
			}

			ptqn_temp = ptqnSearchFreeNode(ptqn->ptqnSubNode[3],i_size-1,i_searchsize);
			if (ptqn_local == NULL)
			{
				ptqn_local = ptqn_temp;
			}
		}
	}

	return ptqn_local;
}



//*********************************************************************************************
// Delete Texture from current page
// All this requires is the address of the map within the page
//
void CPackedRaster::Delete
(
	void* p_tex
)
//*************************************
{
	STextureQuadNode* ptqn_last;

	Assert(p_tex);

	// get the byte offset from the start of the texture..
	uint32	p_texture = ((uint8*)p_tex) - ((uint8*)(prasPackedParent->pSurface));
	uint32	u4_ypos;
	uint32	u4_xpos;

	p_texture /= (prasPackedParent->iPixelBits / 8);

	u4_ypos = p_texture / prasPackedParent->iLinePixels;		// pixel Y pos of texture in page
	u4_xpos = p_texture % prasPackedParent->iLinePixels;		// pixel X pos

	Assert (u4_xpos<256);
	Assert (u4_ypos<256);

	ptqnDel = ptqn_last = NULL;
	ptqnStart = NULL;

	bRemoveQuadNodeAndCompact(&tqtPackQuadTree.tqnRoot,u4_xpos,u4_ypos,tqtPackQuadTree.u4ParentSize);

	while ((ptqnDel!=ptqnStart)  && (ptqnDel!=NULL))
	{
		bRemoveQuadNodeAndCompact(&tqtPackQuadTree.tqnRoot,u1DelXPos,u1DelYPos,tqtPackQuadTree.u4ParentSize);
	}
}




//*********************************************************************************************
// This will return true if the specifed node is in the size list
//
bool CPackedRaster::bFindInNodeList
(
	STextureQuadNode* ptqn,
	int i_size
)
//*************************************
{
	// A node with no chldren and no texture is free. a node gets to this state when it cannot
	// be entered into a free list. When its siblings are deleted it can still be freed dur to
	// this check.
	if ((ptqn->ptqnSubNode[0] == NULL) && (ptqn->ptqnSubNode[1] == NULL) && 
		(ptqn->ptqnSubNode[2] == NULL) && (ptqn->ptqnSubNode[3] == NULL) && (ptqn->u1Texture == 0))
		return true;

	for (uint32 u4=0; u4<(uint32)tqtPackQuadTree.anltFree[i_size].u1Count; u4++)
	{
		if (tqtPackQuadTree.anltFree[i_size].aptqn[u4] == ptqn)
			return true;
	}
	return false;
}




//*********************************************************************************************
// Kill the specified node and keep the free list consecutive.
//
void CPackedRaster::KillNode
(
	STextureQuadNode* ptqn,
	int i_size,
	bool b_delete
)
//*************************************
{
	uint32	u4_count = (uint32)tqtPackQuadTree.anltFree[i_size].u1Count;
	bool	b_found = false;

	for (uint32 u4=0; u4<u4_count; u4++)
	{
		if (tqtPackQuadTree.anltFree[i_size].aptqn[u4] == ptqn)
		{
			b_found = true;

			tqtPackQuadTree.anltFree[i_size].u1Count--;

			if (b_delete)
			{
				MEMLOG_SUB_ADRSIZE(emlTextureManQuad,tqtPackQuadTree.anltFree[i_size].aptqn[u4]);
				deletedNodes.insert(tqtPackQuadTree.anltFree[i_size].aptqn[u4]);
				delete tqtPackQuadTree.anltFree[i_size].aptqn[u4];
			}

			tqtPackQuadTree.anltFree[i_size].aptqn[u4] = NULL;

			uint32	u4_insert = 0;

			for (uint32 u4_read=0; u4_read<u4_count; u4_read++)
			{
				if (tqtPackQuadTree.anltFree[i_size].aptqn[u4_read] != NULL)
				{
					tqtPackQuadTree.anltFree[i_size].aptqn[u4_insert] = tqtPackQuadTree.anltFree[i_size].aptqn[u4_read];
					u4_insert++;
				}
			}
		}
	}

	// we did not find the node in the current free list, this must be one of the ones that we have
	// lost.
	if (!b_found)
	{
		Assert( tqtPackQuadTree.u4LostNodeCount[i_size] >0 );

		tqtPackQuadTree.u4LostNodeCount[i_size]--;

		if (b_delete)
		{
			MEMLOG_SUB_ADRSIZE(emlTextureManQuad,ptqn);
			delete ptqn;
		}
	}
}




//*********************************************************************************************
// Do the actual delete and delete your parent all the way to the root if all your siblings
// have also been removed. This is essential if you are going to prevent global framgmentation
//
bool CPackedRaster::bRemoveQuadNodeAndCompact
(
	STextureQuadNode* ptqn,
	uint32 u4_xp,
	uint32 u4_yp, 
	int i_size
)
{
	Assert(u4_xp<256);
	Assert(u4_yp<256);

	Assert(ptqn);

	if  ( ((ptqn->u1XOrg<=u4_xp) && (((ptqn->u1XOrg)+ptqn->u1Size)>=(uint8)u4_xp) ) && //Is X within the node
		  ((ptqn->u1YOrg<=u4_yp) && (((ptqn->u1YOrg)+ptqn->u1Size)>=(uint8)u4_yp)) )	//Is Y within the node
	{

		if (ptqn->u1Texture == 0)
		{			
			// if there is no texture then we much check the children
			bool b_ret = false;

			for (int i=0; i<4; i++)
			{
				if (ptqn->ptqnSubNode[i])
				{
					if (bRemoveQuadNodeAndCompact(ptqn->ptqnSubNode[i], u4_xp, u4_yp, i_size-1))
					{
						b_ret = true;
					}
				}
			}

			// if we deleted a child, it could mean that we can delete ourselves, Check the free list at the
			// relevent size to see if all children are present.
			if (b_ret)
			{
				// TO PREVENT GLOBAL FRAGMENTATION
				// if all child nodes are free, delete them and return true so our parent will delete us
				if (bFindInNodeList(ptqn->ptqnSubNode[0],i_size-1) && bFindInNodeList(ptqn->ptqnSubNode[1],i_size-1) &&
					bFindInNodeList(ptqn->ptqnSubNode[2],i_size-1) && bFindInNodeList(ptqn->ptqnSubNode[3],i_size-1))
				{
					// all of the children are in the size list

					// Kill the node, this will free its memory and remove it from the free list of the associted size
					KillNode(ptqn->ptqnSubNode[0],i_size-1, true);
					KillNode(ptqn->ptqnSubNode[1],i_size-1, true);
					KillNode(ptqn->ptqnSubNode[2],i_size-1, true);
					KillNode(ptqn->ptqnSubNode[3],i_size-1, true);
					
					ptqn->ptqnSubNode[0] = NULL;
					ptqn->ptqnSubNode[1] = NULL;
					ptqn->ptqnSubNode[2] = NULL;
					ptqn->ptqnSubNode[3] = NULL;

					// make this node free
					ptqn->u1Texture = 0;
					ptqn->ptqnLink = NULL;			// just to be safe!

					// add us to the free list if there is room in the current free list
					if (tqtPackQuadTree.anltFree[i_size].u1Count < tqtPackQuadTree.anltFree[i_size].u1MaxCount)
					{
						tqtPackQuadTree.anltFree[i_size].aptqn[ tqtPackQuadTree.anltFree[i_size].u1Count ] = ptqn;
						tqtPackQuadTree.anltFree[i_size].u1Count++;
					}
					else
					{
						// we do not have room to add the node, so keep a counter of it as a
						// lost node.
						tqtPackQuadTree.u4LostNodeCount[i_size]++;
					}
					return true;
				}
				else
				{
					return false;
				}
			}

			return false;
		}
		else
		{
			// yep, there is a texture pointer and there should not be any children

			//
			// THIS IS THE ONLY PLACE THAT A TEXTURED NODE CAN GET REMOVED. SO IT IS
			// OK TO REMEMBER THE LINK POINTER OF THE DELETED ITEM SO WE DO NOT HAVE
			// TO PASS IT AROUND THE RECURSION.
			//
			Assert(ptqn->ptqnSubNode[0] == NULL);
			Assert(ptqn->ptqnSubNode[1] == NULL);
			Assert(ptqn->ptqnSubNode[2] == NULL);
			Assert(ptqn->ptqnSubNode[3] == NULL);		


			// make this node free
			ptqn->u1Texture = 0;
			ptqnDel = ptqn->ptqnLink;

			if (ptqnStart == NULL)
			{
				ptqnStart = ptqn;
			}

			// if there is a link no only do we have to remember the link address but we 
			// must remember the position. we cannot get the position from the link because
			// the last node may point to the first node if it is a rectangle and the block
			// for the first node may have been deleted and paged out when the last node
			// references it.
			if (deletedNodes.find(ptqnDel) != deletedNodes.end())
			{
				//Pointer is known to have been deleted, do not use
				dout << "applied dead texture node link correction" << std::endl;
				ptqnDel = nullptr;
			}
			else if (ptqnDel)
			{
				u1DelXPos = ptqnDel->u1XOrg;
				u1DelYPos = ptqnDel->u1YOrg;
			}
			ptqn->ptqnLink = NULL;


			// add us to the free list if there is room
			if (tqtPackQuadTree.anltFree[i_size].u1Count < tqtPackQuadTree.anltFree[i_size].u1MaxCount)
			{
				tqtPackQuadTree.anltFree[i_size].aptqn[ tqtPackQuadTree.anltFree[i_size].u1Count ] = ptqn;
				tqtPackQuadTree.anltFree[i_size].u1Count++;
			}
			else
			{
				// we do not have room to add the node, so keep a counter of it as a
				// lost node.
				tqtPackQuadTree.u4LostNodeCount[i_size]++;
			}

			return true;
		}
	}

	return false;
}


//*********************************************************************************************
// Try to allocate a texture of the given size at the position requested.
// Return a texture quad node like the other allocators if successful otherwise return NULL.
//
STextureQuadNode* CPackedRaster::ptqnAddTextureAtPosition
(
	int i_width, 
	int i_height,
	int i_xp,
	int i_yp
)
//*************************************
{
	Assert(bPowerOfTwo(i_width));
	Assert(bPowerOfTwo(i_height));

	if (i_width<(int)u4SmallestAllocation)
	{
		i_width = (int)u4SmallestAllocation;
	}

	if (i_height<(int)u4SmallestAllocation)
	{
		i_height = (int)u4SmallestAllocation;
	}

	// we must be aligned to a multiple of the size
	Assert( (i_xp%i_width) == 0);
	Assert( (i_yp%i_height) == 0);

	STextureQuadNode* ptqn = &tqtPackQuadTree.tqnRoot;
	int i_node_size = (int)(tqtPackQuadTree.u4ParentSize);

	// if it is sqyare then it is trivial to add.
	if (i_width == i_height)
	{
		// which free list are we to use first (the list of the biggest size)
		int i_size	= iGetSize(i_width>i_height?i_width:i_height);

		return ptqnInsertNodeAtPosition(ptqn, i_node_size, i_size, i_xp, i_yp);
	}

	//
	// We are allocating a rectangular block, split it up into many squares whose size
	// are that of the minor edge of the rectangle.
	//
	int i_blocks;
	int i_small;
	int i_big;
	int i_xstep;
	int i_ystep;

	if (i_width<i_height)
	{
		i_blocks = i_height/i_width;		// number of neighboring blocks
		i_small = iGetSize(i_width);
		i_big = iGetSize(i_height);
		i_xstep = 0;
		i_ystep = i_width;
	}
	else
	{
		i_blocks = i_width/i_height;		// number of neighboring blocks
		i_small = iGetSize(i_height);
		i_big = iGetSize(i_width);			
		i_xstep = i_height;
		i_ystep = 0;
	}
	
	// A local array of 32 entries
	STextureQuadNode*	ptqn_sub[256/8];
	int i_count = 0;
	
	//
	// starting at the top left hand corner walk across the map to be allocated and
	// allocate a sqaure at each stop. The individual squares are added to a list
	// and are hooked up into linked list when all the allocations have been made.
	// If an element of the rectangle fails to allocate then the individual squares
	// are removed and the quad tree is compacted. In this case NULL is returned.
	//
	while (i_blocks)
	{
		ptqn_sub[i_count] = ptqnInsertNodeAtPosition(ptqn, i_node_size, i_small, i_xp, i_yp);
		if (!ptqn_sub[i_count])
		{
			// We have failed, free all allocations made so far.
			for (int i=0;i<i_count;i++)
			{
				bRemoveQuadNodeAndCompact(&tqtPackQuadTree.tqnRoot,ptqn_sub[i]->u1XOrg,ptqn_sub[i]->u1YOrg,tqtPackQuadTree.u4ParentSize);
			}

			return NULL;
		}

		// co-ords of the next square
		i_xp+=i_xstep;
		i_yp+=i_ystep;
		i_blocks--;
		i_count++;
	}

	//
	// Link the seperate individual sqaures into a list
	//
	int i;
	for (i = 0; i<i_count-1; i++)
	{
		ptqn_sub[i]->ptqnLink = ptqn_sub[i+1];
	}
	ptqn_sub[i]->ptqnLink = ptqn_sub[0];

	return ptqn;
}


//*********************************************************************************************
// Divides tree and allocates a sqaure region.
// Function will return NULL is the request sqaure cannot be allocated.
STextureQuadNode* CPackedRaster::ptqnInsertNodeAtPosition
(
	STextureQuadNode* ptqn,
	int i_current_size,
	int i_target_size, 
	int i_xp,
	int i_yp
)
//*************************************
{
	while (i_current_size!=i_target_size)
	{
		// if this node is allocated then we must fail, as we cannot allocate it and we cannot
		// sub divide it.
		if (ptqn->u1Texture)
		{
			return NULL;
		}

		i_current_size--;

		// If this node has no children then remove it from the free list and divide it,
		// which will add 4 elements to the next smallest free list. If this node has
		// already been divided, ie, has children then do nothing.
		if ((ptqn->ptqnSubNode[0]==NULL) && (ptqn->ptqnSubNode[1]==NULL) && 
			(ptqn->ptqnSubNode[2]==NULL) && (ptqn->ptqnSubNode[3]==NULL) )
		{
			RemoveFromFreeList(ptqn,i_current_size+1);
			DivideNode(ptqn, i_current_size);
		}

		//
		// Logically figure out which of the 4 nodes we need to work on next based on the
		// requested co-ordinates passed in.
		//
		int i_node = 0;

		if (i_xp < ptqn->ptqnSubNode[0]->u1XOrg)
		{
			i_node |= 1;
		}

		if (i_yp < ptqn->ptqnSubNode[0]->u1YOrg)
		{
			i_node |= 2;
		}

		ptqn = ptqn->ptqnSubNode[i_node];
	}

	// We have divided down to the requested size

	if (ptqn->u1Texture || ptqn->ptqnSubNode[0] || ptqn->ptqnSubNode[1] ||
		ptqn->ptqnSubNode[2] || ptqn->ptqnSubNode[3])
	{
		// The specified co-ordinate in the pack surface is already being used by a texture
		// or is a parent of smaller textures. Either way it cannot be used.
		return NULL;
	}

	// we have found the node we are looking for, so remove it from the free list of the
	// packer and mark it as used.
	RemoveFromFreeList(ptqn,i_current_size);
	ptqn->u1Texture = (uint8)true;;
	ptqn->ptqnLink = NULL;

	return ptqn;
}





//*********************************************************************************************
//
CRenderTexturePackSurface::CRenderTexturePackSurface
(
	uint32				u4_bits,
	ETexturePackTypes	ept
)
{
	//*****************************************************************************************
	// First create two interleaved rasters of the required bit depth
	//*****************************************************************************************

	// Create a virtual memory raster that is 256x256 pixels with a stride of 512 pixels.
	// Packed surfaces have no pixel format..
	// NOTE:	If the texture pack type is 'smallest mip' then the memory comes fom the sub
	//			block allocator.
	rptr<CRaster>	pras_a = rptr_cast(CRaster, rptr_new CRasterMem(
										256,
										256,
										u4_bits,
										512*(u4_bits/8),
										NULL,
										(ept == eptSMALLEST)?emtTexManSubVirtual:emtTexManVirtual) );

	// the second surface is created as a sub rectangle of the first, initially using the
	// same address space but the pAddress element is shifted on by 256 texels.
	SRect			rc(0,0,256,256);
	rptr<CRaster>	pras_b = rptr_cast(CRaster, rptr_new CRasterMem(pras_a, rc) );

	// surface B will use the memory that is unused within the interleave on surface A.
	// This is essential to the tiling primitives.....
	pras_b->pSurface = ((char*)pras_b->pSurface) 
									+ ((pras_b->iWidth) * (pras_b->iPixelBits/8));

	// Create the pack classes for the two surfaces
	ppckPackA = rptr_cast(CPackedRaster, rptr_new CPackedRaster(pras_a) );
	ppckPackB = rptr_cast(CPackedRaster, rptr_new CPackedRaster(pras_b) );

	SetType(ept);
}



//*********************************************************************************************
//
CRenderTexturePackSurface::~CRenderTexturePackSurface()
{
	//
	// Remove our reference to the packed surfaces
	//
	ppckPackA = rptr0;
	ppckPackB = rptr0;
}


//*********************************************************************************************
// Try to allocate a raster of the specified size in one of the two rasters that this
// class contains.
rptr<CRasterMem> CRenderTexturePackSurface::prasAllocateRaster
(
	uint32			u4_xsize,
	uint32			u4_ysize,
	CPixelFormat*	pxf
)
//*************************************
{
	rptr<CRasterMem> pras = ppckPackA->prasAllocateRaster(u4_xsize,u4_ysize,pxf);

	if (!pras)
	{
		// try surface B
		pras = ppckPackB->prasAllocateRaster(u4_xsize,u4_ysize,pxf);
		if (!pras)
			return rptr0;
	}

	return pras;
}


//*********************************************************************************************
// Try to allocate a raster of the same size as the raster passed in. The raster passed in is
// just used as a reference for the parameters. No data is copied into the new raster and the
// raster passed in is not midified in anyway.
rptr<CRasterMem> CRenderTexturePackSurface::prasAllocateRaster
(
	rptr<CRaster>	pras_src
)
//*************************************
{
	return prasAllocateRaster(pras_src->iWidth, pras_src->iHeight, &pras_src->pxf);
}
