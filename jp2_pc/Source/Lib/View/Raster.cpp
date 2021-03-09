/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Raster.hpp.
 *
 * Bugs:
 *
 * To do:
 *		Make CRasterMemT inherit from CRasterT.
 *		Replace SRect with CRectangle<int>, and/or separate params.
 *		Provide manual blitting functions for CRasterMem, when needed.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Raster.cpp                                                   $
 * 
 * 73    9/08/98 8:58p Rwyatt
 * Rasters now have a class specific delete so they can exist in either normal memory or fast
 * heap memory
 * 
 * 72    8/19/98 1:28p Rwyatt
 * Raster have a ZeroRefs function so they can delete themselves correctly when their ref count
 * gets to zero
 * 
 * 71    8/13/98 1:39p Rwyatt
 * Added memory logs for rasters and textures
 * 
 * 70    7/27/98 6:29p Pkeet
 * Added the 'iSurfaceMemBytes' member function.
 * 
 * 69    7/23/98 6:21p Pkeet
 * Added support for linked rasters.
 * 
 * 68    7/22/98 10:05p Agrant
 * Removed data daemon and pre-fetching
 * 
 * 67    6/29/98 9:48p Agrant
 * Verbose error message for going over the small texture limit
 * 
 * 66    6/10/98 7:32p Pkeet
 * CRasterMem unlocks the direct draw surface in the buffer.
 * 
 * 65    98.05.18 12:14p Mmouni
 * Removed special case to not pack 16-bit textures.
 * 
 * 64    4/21/98 3:32p Rwyatt
 * Fixed merge problem
 * 
 * 63    4/21/98 2:44p Rwyatt
 * Added a new memory type for virtual memory rasters. The new type is so we can have
 * nonpageable rasters.
 * 
 * 62    4/02/98 4:57p Pkeet
 * Added the 'bVerifyConstruction' and 'u4MakeTypeIDD3D' member functions.
 * 
 * 61    4/02/98 2:03p Pkeet
 * Added the 'u4GetTypeID' member function.
 * 
 * 60    98/03/12 11:11 Speter
 * Fixed Clear() bug; now properly clears only up to width, not stride. Consolidated and
 * simplified Clear() and Fill() functions. Removed old code: CShareRasterSurface, bLINE_TABLE.
 * Updated comments; updated to do's, and moved to .cpp file.
 * 
 * 59    2/24/98 6:52p Rwyatt
 * CMemRaster pack surface pointer has been changed for the new texture packer
 * 
 * 58    2/11/98 4:37p Rwyatt
 * Change raster can now handle transparent pixels
 * 
 * 57    1/29/98 7:50p Rwyatt
 * New memory type of emtNONE, and a new CRasterMem constructor to use exiting raw data.
 * 
 * 56    1/14/98 6:21p Rwyatt
 * New function to change an existing raster to look like another. Used by the bump packer
 * 
 * 55    1/10/98 3:01p Pkeet
 * Added the 'RemoveFromSystem' member function.
 * 
 * 54    98/01/06 15:40 Speter
 * Moved texture stats to proProfile.
 * 
 * 53    12/05/97 4:14p Agrant
 * Made OnPrefetch const.
 * Fixed bug in OnPrefetch()
 * 
 * 52    12/04/97 4:15p Rwyatt
 * The bool that controlled global or ficed heap memory as been changed into an enum.
 * A new memory type for rasters: emtVirtual.
 * 
 * 51    11/26/97 4:41p Pkeet
 * Added an argument to the basic constructor used by 'CRasterMem' to use memory allocated from
 * the fixed heap instead of the standard heap. Made the remaining constructors for 'CRasterMem'
 * turn the flag off.
 * 
 * 50    97.10.27 1:23p Mmouni
 * Changed format of tiling masks to support MMX inner loops.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/Sys/VirtualMem.hpp"
#include "Raster.hpp"
#include "Lib/Loader/TextureManager.hpp"
#include "Lib/Loader/ImageLoader.hpp"
#include "Lib/Loader/DataDaemon.hpp"
#include "Lib/Loader/TexturePackSurface.hpp"
#include "Lib/Renderer/Primitives/FastBump.hpp"
#include "Lib/Std/Mem.hpp"
#include "Lib/Sys/FixedHeap.hpp"
#include "Lib/Sys/Profile.hpp"

// 'this' : used in base member initializer list
#pragma warning(disable: 4355)


//**********************************************************************************************
//
// CRasterBase implementation.
//

	//******************************************************************************************
	//
	void CRasterBase::UpdateTilingMasks()
	//
	// The width,height or some other property of the texture has changed,
	// we need to update the tiling info.
	//
	//**********************************
	{
#if (VER_CLAMP_UV_TILE)
		//
		// Tiling is disabled in this build.
		//
		fWidth = (float)iWidth;
		fHeight = (float)iHeight;
		bNotTileable = true;
		u4HeightTileMask = 0xffffffff;
		u4WidthTileMask = 0xffffffff;
#else
		//
		// Tiling is enabled in this build, set parameter appropriately. 
		//
		if ( iLinePixels == 512 && iWidth <= 256  )
		{
			uint32	u4_width_mask  = iWidth - 1;
			uint32	u4_height_mask = iHeight - 1;

			// is the raster a power of 2 in U and V
			if ( (iWidth & u4_width_mask) == 0 && (iHeight & u4_height_mask) == 0 )
			{
				// packed (Power of 2) rasters can be tiled.
				bNotTileable = false;

				// This raster is a power of 2 in U and V so combine the tile 
				// masks for the width and the height.
				u4HeightTileMask = u4_height_mask;
				u4WidthTileMask = u4_width_mask;
			}
			else
			{
				// This raster although packed is not a power of 2 in width and height so 
				// it cannot be tiled. Set the mask to wrap acorss the whole raster width, 
				// if tiling is atempted no wrapping will occur.
				bNotTileable = true;
				u4HeightTileMask = 0xFFFFFFFF;
				u4WidthTileMask = 0xFFFFFFFF;
			}
		}
		else
		{
			// If not packed then create a mask that will have no effect 
			// and set the clamp to true..
			// Rasters not packed cannot be tiled.
			bNotTileable = true;
			u4HeightTileMask = 0xFFFFFFFF;
			u4WidthTileMask = 0xFFFFFFFF;
		}

		if (bNotTileable)
		{
			// FP width and height are adjusted in slightly to avoid going off
			// the edges of the raster.
			fWidth = (float)iWidth - (fTexEdgeTolerance * 2.0f);
			fHeight = (float)iHeight - (fTexEdgeTolerance * 2.0f);
		}
		else
		{
			fWidth = (float)iWidth;
			fHeight = (float)iHeight;
		}
#endif
	}

	//******************************************************************************************
	void CRasterBase::SetRaster(int i_width, int i_height, int i_bits, int i_stride)
	{
		Assert(i_bits > 0 && i_bits % 8 == 0);

		iWidth = i_width;  
		iHeight = i_height;  
		iPixelBits = i_bits;  
		pSurface = 0;

		// If i_stride given, use it.  Else calculate it, rounding to 4 bytes.		
 		int i_line_bytes = i_stride ? i_stride : (iWidth * iPixelBits + 31) / 32 * 4;
		iLinePixels = i_line_bytes / (iPixelBits/8);

		//  This assert often triggers when you run GUIApp in 24-bit color mode.  Try 16-bit color.
		Assert(i_line_bytes % (iPixelBits/8) == 0);

		// We need to updatet the info used for tiling the texture.
		UpdateTilingMasks();
	}

	//******************************************************************************************
	void CRasterBase::OnPrefetch(bool b_in_thread) const
	{
#ifdef USING_FETCH
		pddDataDaemon->Prefetch(pSurface, b_in_thread, iLineBytes() * iHeight);
#endif
	}

	//******************************************************************************************
	void CRasterBase::OnFetch()
	{
#ifdef USING_FETCH
		pddDataDaemon->Fetch(pSurface);
#endif
	}

	//******************************************************************************************
	void CRasterBase::OnUnfetch()
	{
		// Does nothing for now.
	}

	//******************************************************************************************
	void CRasterBase::RemoveFromSystem()
	{
		// Does nothing for now.
	}

	//******************************************************************************************
	uint32 CRasterBase::u4GetTypeID() const
	{
		uint32 u4_retval = 0;	// Return value.
		
		u4_retval ^= uint32(iWidth);
		u4_retval <<= 12;
		u4_retval ^= uint32(iHeight);
		return u4_retval;
	}

	//******************************************************************************************
	bool CRasterBase::bVerifyConstruction() const
	{
		return true;
	}

	//******************************************************************************************
	// If this raster is not in the image loader fast heap then delete its memory 
	void CRasterBase::operator delete(void* pv)
	{
		if (!CLoadImageDirectory::bLoadHeapAllocation(pv))
		{
			::delete pv;
		}
	}


//**********************************************************************************************
//
// CRaster implementation.
//

	//**********************************************************************************************
	//
	template<class PIX> class CRasterSurfaceT: public CRaster::CRasterSurface
	//
	// Prefix: rsf
	//
	// Implements CRasterSurface for various pixel types.
	//
	//**************************************
	{
	public:

		//******************************************************************************************
		//
		// Constructor.
		//

		CRasterSurfaceT
		(
			CRasterBase& rasb_owner
		)
			: CRasterSurface(rasb_owner)
		{
		}

		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		const PIX* ppixAddress
		(
			int i_index
		) const
		//
		// Returns:
		//		The address of the index.
		//
		//******************************
		{
			Assert(uint(i_index) < rasbOwner.iTotalPixels());
			Assert(rasbOwner.pSurface);
			return reinterpret_cast<PIX*>(rasbOwner.pSurface) + i_index;
		}

		//******************************************************************************************
		PIX* ppixAddress(int i_index)
		{
			Assert(uint(i_index) < rasbOwner.iTotalPixels());
			Assert(rasbOwner.pSurface);
			return reinterpret_cast<PIX*>(rasbOwner.pSurface) + i_index;
		}

		//******************************************************************************************
		//
		// Overrides.
		//

		//******************************************************************************************
		TPixel pixGet(int i_index) const override
		{
			return *ppixAddress(i_index);
		}

		//******************************************************************************************
		void PutPixel(int i_index, TPixel pix) override
		{
			*ppixAddress(i_index) = pix;
		}

		//******************************************************************************************
		virtual void Fill(int i_index, int i_count, TPixel pix) override
		{
			Assert(i_index + i_count <= rasbOwner.iTotalPixels());
			::Fill(ppixAddress(i_index), i_count, PIX(pix));
		}

		//******************************************************************************************
		virtual void Rect(const SRect& rect, TPixel pix) override
		{
			Assert(uint(rect.iY + rect.iHeight) <= rasbOwner.iHeight);

			// Find the index of the upper left pixel.
			int i_index = rasbOwner.iIndex(rect.iX, rect.iY);
			for (int i_y = 0; i_y < rect.iHeight; i_y++)
			{
				// Call this class's Fill().
				Fill(i_index, rect.iWidth, pix);
				i_index += rasbOwner.iLinePixels;
			}
		}
	};

	//**********************************************************************************************
	//
	// CRasterSurface implementation.
	//

		//******************************************************************************************
		CRaster::CRasterSurface::CRasterSurface(CRasterBase& ras)
			: rasbOwner(ras)
		{
		}

		//******************************************************************************************
		CRaster::CRasterSurface::CRasterSurface(CRasterBase& ras, int i_bits)
			: rasbOwner(ras)
		{
			// Construct ourselves as an appropriate derived type.
			switch (i_bits)
			{
				case 8:
					new(this) CRasterSurfaceT<uint8>(ras);
					break;
				case 16:
					new(this) CRasterSurfaceT<uint16>(ras);
					break;
				case 24:
					new(this) CRasterSurfaceT<uint24>(ras);
					break;
				case 32:
					new(this) CRasterSurfaceT<uint32>(ras);
					break;
				default:
					Assert(0);
			}
		}

	//******************************************************************************************
	CRaster::CRaster()
		: RasterSurface(*this)
	{
		Link();
	}

	//******************************************************************************************
	int CRaster::iSurfaceMemBytes()
	{
		return iWidth * iHeight * (iPixelBits >> 3);
	}

	//******************************************************************************************
	void CRaster::SetRaster(int i_width, int i_height, int i_bits, int i_stride, CPixelFormat* ppxf)
	{
		// This is more restrictive than CRasterBase, in that pixel size can only be 1..4 bytes.
		Assert(i_bits <= 32);
		CRasterBase::SetRaster(i_width, i_height, i_bits, i_stride);

		// Construct our RasterSurface member.
		new(&RasterSurface) CRasterSurface(*this, i_bits);

		// Set pixel format.  If not given, use default for pixel bits.
		pxf = ppxf ? *ppxf : CPixelFormat(i_bits);
	}

	//******************************************************************************************
	void CRaster::AttachPalette(CPal* ppal, CPixelFormat* ppxf)
	{
		if (iPixelBits == 8)
		{
			//
			// Set the pixel format info.
			// If a specific format was passed in (e.g. pxf332), assume the palette reflects 
			// that format, and set it.  Otherwise, set it to the empty pixel format, which 
			// implies a palette lookup for colour conversions.
			//
			pxf = ppxf ? *ppxf : CPixelFormat(8);
		}
		pxf.ppalAttached = ppal;
	}

	//******************************************************************************************
	void CRaster::Clear(TPixel pix)
	{
		Lock();
		if (iWidth == iLinePixels)
			// Can efficiently clear in one block.
			RasterSurface.Fill(0, iTotalPixels(), pix);
		else
			// Clear only the pixels we own.
			RasterSurface.Rect(SRect(0, 0, iWidth, iHeight), pix);
		Unlock();
	}

	//******************************************************************************************
	void CRaster::Link(rptr<CRaster> pras)
	{
		// Break the link if the raster pointer is null.
		if (!pras)
		{
			if (prasLink)
				prasLink->prasLink = rptr0;
			prasLink = rptr0;
			return;
		}

		// Establish the link.
		prasLink           = pras;
		prasLink->prasLink = rptr_this(this);
	}

	//******************************************************************************************
	CRasterD3D* CRaster::prasd3dGet()
	{
		return 0;
	}


//**********************************************************************************************
//
// class CRasterMem implementation.
//

	//******************************************************************************************
	CRasterMem::CRasterMem(int i_width, int i_height, int i_bits, int i_stride, 
						   CPixelFormat* ppxf, EMemType emt_heap_type)
		: emtHeapType(emt_heap_type)
	{
		// Set up the dimensions.
		SetRaster(i_width, i_height, i_bits, i_stride, ppxf);

		MEMLOG_ADD_COUNTER(emlCRaster, 1);

		// Now allocate the memory.
		switch (emtHeapType)
		{
		case emtNormal:
			pSurface = new uint8[iLineBytes() * iHeight];
			break;

		case emtFixed:
			pSurface = fxhHeap.pvMalloc(iLineBytes() * iHeight);
			break;

		case emtTexManVirtual:
			pSurface = gtxmTexMan.pvmeTextures->pvMalloc(iLineBytes() * iHeight);
			break;

		case emtTexManSubVirtual:
			// If there is a sub allocator for small textures then use it, otherwise
			// allocate them as normal virtual memory surfaces.
			if (gtxmTexMan.pvmsaSmallTextures)
			{
				pSurface = gtxmTexMan.pvmsaSmallTextures->pvMalloc(iLineBytes() * iHeight);

				//
				// If this asserts then there is not enough memory reserved for the small
				// textures. This can be changed from the texture manager dialog before a
				// file is loaded. The default can be changed in the texture manager source.
				//
#if VER_TEST
				if (!pSurface)
				{
					char str_buffer[512];
					sprintf(str_buffer, "%s\n\nOver small texture memory limit!\n", __FILE__);
					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}
#endif
			}
			else
			{
				// If we allocated normal virtual memory change the heap type of this
				// raster
				pSurface = gtxmTexMan.pvmeTextures->pvMalloc(iLineBytes() * iHeight);
				emtHeapType = emtTexManVirtual;
			}
			break;

		case emtNone:
			pSurface = NULL;
			break;
		}

		// Set the specified texture pack surface.
		ppckrParent = rptr0;

		// Add to stats.
		proProfile.psTextures.Add(0, 1);
		proProfile.psTextureKB.Add(0, iTotalPixels() * iPixelBytes() / 1024);
	}

	//******************************************************************************************
	CRasterMem::CRasterMem(rptr<CRaster> pras_source, const SRect& rc,
							rptr<CPackedRaster> ppckr,
							CPixelFormat* ppxf)
	{
		// sub rasters have no memory type as they are never freed.
		emtHeapType = emtNone;
		MEMLOG_ADD_COUNTER(emlCRaster, 1);

		// Set up the dimensions from rc, copy format info from pras_source if a alternate
		// ppxf is not passed in.
		SetRaster(rc.iWidth, rc.iHeight, pras_source->iPixelBits, pras_source->iLineBytes(), 
			ppxf==0?&pras_source->pxf:ppxf);

		// lock the source to ensure that the memory pointer is valid
		pras_source->Lock();

		// Now set the memory.
		pSurface = pras_source->pAddress(rc.iX, rc.iY);

		pras_source->Unlock();

		// Reference the last pixel as well, for range-checking asserts.
		pras_source->pAddress(rc.iX + rc.iWidth-1, rc.iY + rc.iHeight-1);

		// the referenced (source) is required for the delete function, it also keeps the rptr counter
		// correct.
		prasReferenced = rptr_cast(CRaster,pras_source);

		// set the pack surface
		ppckrParent = ppckr;
	}


	//******************************************************************************************
	CRasterMem::CRasterMem(void* pv, int i_width, int i_height, int i_bits, int i_stride,
						   CPixelFormat* ppxf,	EMemType emt) 
	{
		MEMLOG_ADD_COUNTER(emlCRaster, 1);

		// Set up the dimensions.
		SetRaster(i_width, i_height, i_bits, i_stride, ppxf);

		// Set the specified texture pack surface.
		ppckrParent = rptr0;

		//
		// fill in the missing elements of the class
		//
		pSurface = pv;
		emtHeapType = emt;

		// Add to stats.
		proProfile.psTextures.Add(0, 1);
		proProfile.psTextureKB.Add(0, iTotalPixels() * iPixelBytes() / 1024);
	}




	//******************************************************************************************
	CRasterMem::~CRasterMem() 
	{
		MEMLOG_SUB_COUNTER(emlCRaster, 1);

		// Remove any linked rasters.
		if (prasLink)
			prasLink->prasLink = rptr0;
		prasLink = rptr0;

		DeleteRasterMemory();

		//
		// Keep track of the raster profile counters
		//
		if (!prasReferenced)
		{
			proProfile.psTextures.Add(0, -1);
			proProfile.psTextureKB.Add(0, - iTotalPixels() * iPixelBytes() / 1024);
		}
	}

	//******************************************************************************************
	// This function will change class raster to be identical to the reference raster passed in
	// and the data will remain intact. This needs to be used when the raster needs to be 
	// changed but the CRaster class cannot change address. 'New'ing a different class over the
	// top of this class is not sufficient because the ref counters get out of step.
	//
	// NOTE:	The reference raster passed in will be corrupted so make sure it is not used
	//			elsewhere. The class will still exist but its surface and its local data will
	//			be invalid. This has to be so when the reference raster gets deleted it does
	//			not delete our data. It is best to use a local raster which gets deleted when
	//			its parent function exits.
	// 
	void CRasterMem::ChangeRaster
	(
		rptr<CRasterMem>	pras_ref,
		bool				b_transparent
	)
	//
	//*********************************
	{
		Assert(pras_ref->iWidth>=iWidth);
		Assert(pras_ref->iHeight>=iHeight);

		//
		// Copy the source data into the reference raster
		//
		uint8*	src;
		uint8*	dst;

		Lock();
		pras_ref->Lock();

		// copy the source raster into the reference raster
		src = (uint8*)pAddress(0);
		dst = (uint8*)pras_ref->pAddress(0);

		int x, y;

		switch (iPixelBits)
		{
		case 8:
			for (y = 0; y<iHeight; y++)
			{
				for (x = 0; x<iWidth*(iPixelBits/8); x++)
				{
					if (!(b_transparent && src[x] == 0))
						dst[x] = src[x];
				}
				src = ((uint8*)src) + iLineBytes();
				dst = ((uint8*)dst) + pras_ref->iLineBytes();
			}
			break;


		case iBUMPMAP_RESOLUTION:
			//
			// A CBumpAnglePair raster is a bump map so we need to extract the colour element,
			// this is usually the top byte but we will use the class just to be sure.
			// The resulting data is 8 bits per pixel and uses the palette from the
			// raster.
			//
			CBumpAnglePair	bang;

			for (y = 0; y<iHeight; y++)
			{
				for (x = 0; x<iWidth; x++)
				{
					bang.br = pixGet(x, y);

					if (!(b_transparent && bang.u1GetColour() == 0))
					{
						pras_ref->PutPixel(x,y,bang.br);
					}
				}
			}

			break;
		}

		Unlock();
		pras_ref->Unlock();

		//
		// Delete the memory of this raster
		//
		DeleteRasterMemory();

		//
		// Make this raster look like the reference raster
		//

		// width, height and colour are all correct
		iLinePixels = pras_ref->iLinePixels;
		pSurface = pras_ref->pSurface;
		UpdateTilingMasks();

		// copy the CRasterMem elements
		ppckrParent = pras_ref->ppckrParent;
		pras_ref->ppckrParent = rptr0;
		prasReferenced = pras_ref->prasReferenced;
		pras_ref->prasReferenced = rptr0;
		emtHeapType = pras_ref->emtHeapType;

		pras_ref->emtHeapType = emtNone;
		pras_ref->pSurface = NULL;
	}



	//******************************************************************************************
	// 
	void CRasterMem::DeleteRasterMemory
	(
	)
	//
	//*********************************
	{
		// if a texture pack surface is specifed then call it to say delete a texture
		if (ppckrParent)
		{
			// the address of the parent raster and the address of the sub raster,
			// from this its quad node location can be calculated.
			ppckrParent->Delete(pSurface);
		}

		// Delete a non-shared surface.
		if (!prasReferenced)
		{
			// Now deallocate the memory.
			switch (emtHeapType)
			{
			case emtNormal:
				delete[] static_cast<uint8*>(pSurface);
				break;

			case emtFixed:
				fxhHeap.Free(pSurface);
				break;

			case emtTexManVirtual:
				gtxmTexMan.pvmeTextures->Free(pSurface);
				break;

			case emtTexManSubVirtual:
				if (gtxmTexMan.pvmsaSmallTextures)
					gtxmTexMan.pvmsaSmallTextures->Free(pSurface);
				break;

			case emtNone:
				break;
			}

			pSurface = NULL;
		}
	}