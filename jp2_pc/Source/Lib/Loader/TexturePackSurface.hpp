/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of texture packing.
 *
 * Contents:
 *			CPackedRender
 *			CRenderTexturePackSurface
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/TexturePackSurface.hpp                                     $
 * 
 * 12    8/25/98 2:36p Rvande
 * removed redundant class scope
 * 
 * 11    4/24/98 4:08p Rwyatt
 * Fixed assert if a 8x256 map was packed first in a page.
 * 
 * 10    4/21/98 3:12p Rwyatt
 * New pack surface type for smallest mip maps.
 * New logic to set the smallest mip maps
 * 
 * 9     3/30/98 11:13p Rwyatt
 * Modified to allow a parent surface that is not 256x256. This is still themaximum size but now
 * any power of 2 and sqaure raster can be used,
 * There is also a minimum now a minimum size that all smaller allocations are rounded upto.
 * 
 * 8     3/17/98 7:53p Rwyatt
 * Added new functions that allow textures to be packed at a specified location.
 * 
 * 7     2/24/98 6:55p Rwyatt
 * CTexturePackSurface has been split into two seperate classes. The new class CPackedRaster
 * does the packing on a single raster and the old class has been modifed to suit.
 * 
 * 6     1/29/98 7:42p Rwyatt
 * New mip settings for special cases such as curved bump map parents
 * 
 * 5     1/09/98 7:03p Rwyatt
 * Each pack surface has a base mip level which is used to determine what can be packed in it.
 * 
 * 4     9/01/97 7:57p Rwyatt
 * Delete member can now handle any surfcaes of any bit depth
 * 
 * 3     8/19/97 7:03p Rwyatt
 * Sub allocated mem rasters (such as textures) now have an rptr back to the parent Texture Pack
 * surface. This is so the packer can be infomred when the texture is deleted, it is an rptr to
 * avoid static destructor dependencies.
 * 
 * 2     7/29/97 2:01a Rwyatt
 * Latest packer, this version has no knowledege about the thing that represents the sub region.
 * No texture pointer is stored in the quad node - just a 0/1 used flag which is now a byte to
 * keep the node size minimum.
 * Packing is set 1
 * 
 * 1     7/29/97 1:01a Rwyatt
 * Initial implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_TEXTUREPACKSURFACE_HPP
#define HEADER_TEXTUREPACKSURFACE_HPP

#include "Lib/View/Raster.hpp"
#include <unordered_set>

#pragma pack(push,1)


//**********************************************************************************************
// Each pack surface has one of the following associated with it and it governs what can and
// cannot be packed with it.
enum ETexturePackTypes
// prefix: ept
{
	eptCURVED_PARENT		=	-5,
	eptUSER_PACK			=	-4,
	eptDONT_CARE			=	-3,
	eptSMALLEST_USER_PACK	=	-2,
	eptSMALLEST				=	-1,
	eptTOP_LEVEL			=	0,
	eptMIP1,
	eptMIP2,
	eptMIP3,
	eptMIP4,
	eptMIP5,
	eptMIP6,
	eptMIP7,
	eptMIP8,
	eptMIP9,
	eptMIP10,
};



//**********************************************************************************************
// A single node of the packed raster quad tree
//
struct STextureQuadNode
// prefix: tqn
{
	uint8				u1Size;
	uint8				u1XOrg;
	uint8				u1YOrg;
	uint8				u1Texture;

	STextureQuadNode*	ptqnLink;
	STextureQuadNode*	ptqnSubNode[4];

	STextureQuadNode()
	{
		ptqnSubNode[0] = NULL;
		ptqnSubNode[1] = NULL;
		ptqnSubNode[2] = NULL;
		ptqnSubNode[3] = NULL;
		ptqnLink = NULL;
		u1Texture = 0;
	};
};



//**********************************************************************************************
// Each packed raster has a free node list so it can locate quad nodes of a specific size
// quickly without having to search.
//
struct SNodeList
// prefix: nlt
{
	uint8				u1Count;		// number of elements at this size
	uint8				u1MaxCount;		// max number of elements in the list for this size
	STextureQuadNode**	aptqn;			// the array of free nodes at this size
};



//**********************************************************************************************
// The root structure of the quad tree that the nodes are contained within
// This also contains pointers to the rasters that hold the data that the tree represents.
//
struct STextureQuadTree
// prefix: tqt
{
	SNodeList			anltFree[9];		// an array of free nodes at each size
	uint32				u4LostNodeCount[9];	// an array of lost nodes conters, 1 for each size
	uint32				u4ParentSize;		// ordinal for size of parent (power of parent)
	STextureQuadNode	tqnRoot;
};



//*********************************************************************************************
// Maximum number of quad node pointers in the per size free lists.
//
#define u4FREE_LIST_ENTRIES_PARENT	1
#define u4FREE_LIST_ENTRIES_DIVIDE1	4
#define u4FREE_LIST_ENTRIES_DIVIDE2	16


#define u4FREE_LIST_ENTRIES_DIVIDE3	64
#define u4FREE_LIST_ENTRIES_DIVIDE4	64
#define u4FREE_LIST_ENTRIES_DIVIDE5	64
#define u4FREE_LIST_ENTRIES_DIVIDE6	64
#define u4FREE_LIST_ENTRIES_DIVIDE7	64
#define u4FREE_LIST_ENTRIES_DIVIDE8	64

//
// This must be set to the biggest of the above values
//
#define u4LOCAL_FREE_LIST_SIZE		64

//*********************************************************************************************
//
class CPackedRaster : public CRefObj
// prefix: pckr
{
public:
	//*****************************************************************************************
	CPackedRaster::CPackedRaster
	(
		rptr<CRaster>	pras_parent,
		uint32			u4_smallest = 8
	);

	//*****************************************************************************************
	CPackedRaster::~CPackedRaster
	(
	);

	//*****************************************************************************************
	rptr<CRaster> prasGetParentRaster
	(
	)
	//*********************************
	{
		return prasPackedParent;
	}

	//*****************************************************************************************
	rptr<CRasterMem> prasAllocateRaster
	(
		uint32			u4_xsize,
		uint32			u4_ysize,
		CPixelFormat*	pxf = 0
	);

	//*****************************************************************************************
	void Delete
	(
		void* p_tex
	);

	//*****************************************************************************************
	void SetType
	(
		ETexturePackTypes	ept
	)
	//*********************************
	{
		eptMipLevel = ept;
	}

	//******************************************************************************************
	ETexturePackTypes eptGetType
	(
	)
	//*********************************
	{
		return eptMipLevel;
	}

	//******************************************************************************************
	STextureQuadTree* ptqtGetPackQuadTree
	(
	)
	//*********************************
	{
		return &tqtPackQuadTree;
	}


	//******************************************************************************************
	//
	STextureQuadNode* ptqnAddTextureAtPosition
	(
		int i_width, 
		int i_height,
		int i_xp,
		int i_yp
	);


protected:
	rptr<CRaster>			prasPackedParent;
	STextureQuadTree		tqtPackQuadTree;
	ETexturePackTypes		eptMipLevel;

	STextureQuadNode*		atqnSizeParent[u4FREE_LIST_ENTRIES_PARENT];
	STextureQuadNode*		atqnSizeDivide1[u4FREE_LIST_ENTRIES_DIVIDE1];
	STextureQuadNode*		atqnSizeDivide2[u4FREE_LIST_ENTRIES_DIVIDE2];

	STextureQuadNode*		ptqnDel;
	STextureQuadNode*		ptqnStart;
	uint32					u4SmallestAllocation;
	uint8					u1DelXPos;
	uint8					u1DelYPos;

	std::unordered_set<STextureQuadNode*> deletedNodes;

	//*****************************************************************************************
	void InitQuadTree
	(
		uint32				u4_parent_size,
		uint32				u4_smallest_size
	);

	//*****************************************************************************************
	bool bFindNextHorizontal
	(	
		STextureQuadNode*	aptqn[],
		uint32				au4[],
		uint32				u4_count,
		uint32*				pu4_adjcount,
		uint32*				pu4_amin,
		uint32*				pu4_amax,
		uint32				i_pixels
	);

	//*****************************************************************************************
	bool bFindNextVertical
	(	
		STextureQuadNode*	aptqn[],
		uint32				au4[],
		uint32				u4_count,
		uint32*				pu4_adjcount,
		uint32*				pu4_amin,
		uint32*				pu4_amax,
		uint32				i_pixels
	);

	//*****************************************************************************************
	STextureQuadNode* ptqnAddRaster
	(
		int i_width, 
		int i_height
	);


	//*****************************************************************************************
	STextureQuadNode* ptqnAddSubTexture
	(
		int					i_width, 
		int					i_height,
		int					i_blocks,
		int					i_size,
		int					i_small,
		int					i_big,
		int					i_pixels,
		bool				b_vertical
	);

	//*****************************************************************************************
	STextureQuadNode* ptqnFindQuadNode
	(
		int i_size 
	);

	//*****************************************************************************************
	void SplitQuadNodeForRectangle
	(
		STextureQuadNode* ptqn, 
		bool b_vertical,		// set to true to split vertically (width<height)
		int i_small, 
		int i_size
	);

	//*****************************************************************************************
	void KillNode
	(
		STextureQuadNode* ptqn,
		int i_size,
		bool b_delete
	);

	//*****************************************************************************************
	void DivideNode
	(
		STextureQuadNode* ptqn, 
		int i_size
	);

	//*****************************************************************************************
	STextureQuadNode* ptqnSearchFreeNode
	(
		STextureQuadNode*	ptqn,
		int					i_size,
		int					i_searchsize 
	);

	//*****************************************************************************************
	bool bFindInNodeList
	(
		STextureQuadNode* ptqn,
		int i_size
	);

	//*****************************************************************************************
	bool bRemoveQuadNodeAndCompact
	(
		STextureQuadNode* ptqn,
		uint32 u4_xp,
		uint32 u4_yp, 
		int i_size
	);


	//******************************************************************************************
	//
	STextureQuadNode* ptqnInsertNodeAtPosition
	(
		STextureQuadNode*	ptqn,
		int					i_node_size,
		int					i_target_size, 
		int					i_xp,
		int					i_yp
	);


	//******************************************************************************************
	void RemoveFromFreeList
	(
		STextureQuadNode* ptqn, 
		int i_current_size
	)
	//*************************************
	{
		SNodeList*	pnlt = &tqtPackQuadTree.anltFree[i_current_size];

		for (int i = 0; i<pnlt->u1Count; i++)
		{
			if (pnlt->aptqn[i] == ptqn)
			{
				// if there are none more enteries then there is no list to adjust
				pnlt->u1Count--;
				if (pnlt->u1Count == 0)
					return;

				// go through the remaining list and shift it down by 1
				for (int j = i+1; j<pnlt->u1Count+1; j++)
				{
					pnlt->aptqn[j-1] = pnlt->aptqn[j];
				}

				return;
			}
		}
	}


	//*********************************************************************************************
	__inline static int iGetSize
	(
		int i_size
	)
	//*************************************
	{
		Assert (bPowerOfTwo(i_size));

		AlwaysAssert (i_size>=1);
		AlwaysAssert (i_size<=256);

		switch (i_size)
		{
		case 1:
			return 0;

		case 2:
			return 1;

		case 4:
			return 2;

		case 8:
			return 3;

		case 16:
			return 4;

		case 32:
			return 5;

		case 64:
			return 6;

		case 128:
			return 7;

		case 256:
			return 8;
		}

		// If we get to here it is broke!
		Assert(0);
		return 0;
	}
};







//**********************************************************************************************
// The class for a TexturePackSurface controls 2 256x256 surfaces. This is due to the 256 byte
// interleave that is required for the tiling primitives.
//
class CRenderTexturePackSurface
// prefix: tps
{
public:
	//******************************************************************************************
	//
	CRenderTexturePackSurface
	(
		uint32				u4_bits,				// number of bits per pixel in the base rasters...
		ETexturePackTypes	ept = eptDONT_CARE		// Mip type (also controls memory allocation)
	);

	~CRenderTexturePackSurface();

	//******************************************************************************************
	ETexturePackTypes eptGetType()
	{
		return eptMipLevel;
	}

	//******************************************************************************************
	// Set the type of both pack surfaces and our local copy
	void SetType(ETexturePackTypes	ept)
	{
		ppckPackA->SetType(ept);
		ppckPackB->SetType(ept);
		eptMipLevel = ept;
	}

	//******************************************************************************************
	int iGetPixelBits()
	{
		return ppckPackA->prasGetParentRaster()->iPixelBits;
	}

	//******************************************************************************************
	rptr<CRaster> prasGetRaster
	(
		uint32	u4_surface
	)
	{
		Assert(u4_surface<=1);

		if (u4_surface == 0)
			return ppckPackA->prasGetParentRaster();
		else
			return ppckPackB->prasGetParentRaster();
	}

	//*********************************************************************************************
	rptr<CPackedRaster> ppckPackedRaster
	(
		uint32 u4_surface
	)
	{
		Assert(u4_surface<=1);

		if (u4_surface == 0)
			return ppckPackA;
		else
			return ppckPackB;
	}


	//*********************************************************************************************
	rptr<CRasterMem> prasAllocateRaster
	(
		uint32			u4_xsize,
		uint32			u4_ysize,
		CPixelFormat*	pxf
	);

	//*********************************************************************************************
	rptr<CRasterMem> prasAllocateRaster
	(
		rptr<CRaster>	pras_src
	);

protected:
	//******************************************************************************************
	// Pointers to the raster data
	//******************************************************************************************
	rptr<CPackedRaster>			ppckPackA;
	rptr<CPackedRaster>			ppckPackB;
	ETexturePackTypes			eptMipLevel;
};

#pragma pack(pop)

#endif //#ifndef HEADER_TEXTUREPACKSURFACE_HPP