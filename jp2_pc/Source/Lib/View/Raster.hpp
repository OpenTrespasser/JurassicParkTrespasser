/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		class CRaster and many descendents.
 *
 *		CRasterT<PIX>: CAArray2<PIX>
 *		CRasterBase
 *			CRaster
 *				CRasterMem
 *		CRasterBaseT<BASE, PIX>
 *			CRasterMemT<PIX> = CRasterBaseT<CRasterMem, PIX>
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Raster.hpp                                                   $
 * 
 * 82    9/08/98 8:58p Rwyatt
 * Rasters now have a class specific delete so they can exist in either normal memory or fast
 * heap memory
 * 
 * 81    8/19/98 1:28p Rwyatt
 * Raster have a ZeroRefs function so they can delete themselves correctly when their ref count
 * gets to zero
 * 
 * 80    98.08.13 4:27p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 79    8/07/98 11:42a Pkeet
 * Added the 'bIsLocked' member function.
 * 
 * 78    7/27/98 6:29p Pkeet
 * Added the 'iSurfaceMemBytes' member function.
 * 
 * 77    7/23/98 6:20p Pkeet
 * Added support for linked rasters.
 * 
 * 76    7/20/98 10:32p Pkeet
 * Changed the 'd3dhGetHandle' member function to 'pd3dtexGet.' Made uploads succeed by default.
 * 
 * 75    5/12/98 6:44p Mlange
 * Renamed bUploadRegion() to bUpload() and changed its parameters.
 * 
 * 74    5/12/98 12:28p Pkeet
 * Added the 'bUploadRegion' member function.
 * 
 * 73    4/28/98 5:02p Rwyatt
 * Added a member to the base class to give the number of bytes that the raster covers. This is
 * the number of bytes between the top left and bottom right corners.
 * 
 * 72    4/21/98 3:21p Rwyatt
 * Fixed merge problems
 * 
 * 71    4/21/98 2:44p Rwyatt
 * Added a new memory type for virtual memory rasters. The new type is so we can have
 * nonpageable rasters.
 * 
 * 70    4/08/98 5:30p Pkeet
 * Added the 'bD3DRaster' member function.
 * 
 * 69    4/02/98 4:57p Pkeet
 * Added the 'bVerifyConstruction' and 'u4MakeTypeIDD3D' member functions.
 * 
 * 68    4/02/98 2:03p Pkeet
 * Added the 'u4GetTypeID' member function.
 * 
 * 67    98/03/12 11:10 Speter
 * Fixed Clear() bug; now properly clears only up to width, not stride. Consolidated and
 * simplified Clear() and Fill() functions. Removed old code: CShareRasterSurface, bLINE_TABLE.
 * Updated comments; updated to do's, and moved to .cpp file.
 * 
 * 66    3/02/98 8:19p Rwyatt
 * Mip level colouring abd controlling define
 * 
 * 65    2/24/98 6:52p Rwyatt
 * CMemRaster pack surface pointer has been changed for the new texture packer
 * 
 * 64    2/11/98 4:37p Rwyatt
 * Change raster can now handle transparent pixels
 * 
 * 63    1/29/98 7:50p Rwyatt
 * New CRasterMem constructor that will use existing raw data.
 * 
 * 62    1/14/98 6:21p Rwyatt
 * New function to change an existing raster to look like another. Used by the bump packer
 * 
 * 61    1/10/98 3:01p Pkeet
 * Added the 'RemoveFromSystem' member function.
 * 
 * 60    12/12/97 12:06p Pkeet
 * Added generic functions for D3D textures.
 * 
 * 59    12/05/97 4:13p Agrant
 * Made the OnPrefetch() function const
 * 
 * 58    12/04/97 4:11p Rwyatt
 * Rasters can now allocate memory from various memory heaps.
 * 
 * 57    11/26/97 4:40p Pkeet
 * Added an argument to the basic constructor used by 'CRasterMem' to use memory allocated from
 * the fixed heap instead of the standard heap. Made the destructor for 'CRasterMem' virtual.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_RASTER_HPP
#define HEADER_LIB_VIEW_RASTER_HPP

#include "Lib/Std/Ptr.hpp"
#include "Lib/Std/Array2.hpp"
#include "Lib/View/Pixel.hpp"
#include "Lib/View/Palette.hpp"
#include "Lib/Loader/Fetchable.hpp"
#include <memory.h>


//
// Constants.
//

// Set the margin for texture maps in pixels.
const float fTexEdgeTolerance = 0.2f;

#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

// Two constant in a QWORD for 3DX.
const float pfTexEdgeTolerance[2] = { fTexEdgeTolerance, fTexEdgeTolerance };

#endif


//
// Forward Delclaration
//
class CPackedRaster;
class CRasterMem;
class CRasterD3D;


//
// Types used in CRaster classes.
//

//**********************************************************************************************
//
template<class PIX> class CRasterT: public CRefObj, public CAArray2<PIX>
//
// Prefix: rast
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	CRasterT(int i_width, int i_height)
		: CAArray2<PIX>(i_height, i_width)
	{
	}

	CRasterT(const CPArray2<PIX>& pa2)
		: CAArray2<PIX>(pa2)
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	PIX& tPix(int i_x, int i_y)
	{
		return (*this)[i_y][i_x];
	}

	const PIX& tPix(int i_x, int i_y) const
	{
		return (*this)[i_y][i_x];
	}

	void Clear(PIX pix)
	{
		Fill(pix);
	}
};


//
// Class definitions.
//

//**********************************************************************************************
//
class CRasterBase: public CRefObj, public CFetchable
//
// Prefix: ras
//
// A base class for raster objects; not instantiable.
// Contains raster dimensions, and basic access functions.
//
//**************************************
{
public:
	int			iWidth, iHeight;		// Raster dimensions.
	int			iPixelBits;				// Bits per pixel.
	int			iLinePixels;			// Pixels per line (may be greater than iWidth).
	void*		pSurface;				// The address of the video surface.

	float		fWidth, fHeight;		// Floating point raster dimensions.
	bool		bNotTileable;			// Flag indicating it is not possible to tile this raster.
	uint32		u4HeightTileMask;		// Mask to insure a vertical offset is < iHeight.
	uint32		u4WidthTileMask;		// Mask to insure a horizontal offset is < iWidth.

public:

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void operator delete(void* pv);

	//******************************************************************************************
	//
	int iPixelBytes() const
	//
	// Returns:
	//		The number of bytes per pixel in the raster.
	//
	//**********************************
	{
		Assert(iPixelBits % 8 == 0);
		return iPixelBits / 8;
	}

	//******************************************************************************************
	//
	int iLineBytes() const
	//
	// Returns:
	//		The number of bytes per line in the raster.
	//
	//**********************************
	{
		return iLinePixels * iPixelBytes();
	}

	//******************************************************************************************
	//
	int iTotalPixels() const
	//
	// Returns:
	//		The total number of pixels in the raster.
	//
	//**********************************
	{
		return iLinePixels * iHeight;
	}


	//******************************************************************************************
	//
	int iByteSpan() const
	//
	// Returns:
	//		Returns the number of bytes from the first pixel to the last, this is different from
	//		iTotalPixels()*iPixelBytes() if the stride is not the same as the width. For example
	//		if the stride is 2048 and a texture is 64x64 then the number of bytes between the
	//		first and last pixel is (63*2048) + 64. We do not include the whole stride on the
	//		last scan line. This is very important if the rasters bottom left pixel is the last
	//		byte of legal memory.
	//
	//		Pixel Span = ((Height-1)*Stride) + Width
	//
	//**********************************
	{
		return (((iHeight-1)*iLinePixels) + iWidth) * iPixelBytes();
	}

	//******************************************************************************************
	//
	int iIndex
	(
		int i_x, int i_y				// Coordinates of pixel.
	) const
	//
	// Returns:
	//		The integer index of the pixel location.
	//
	//**********************************
	{
		Assert((uint)i_x < (uint)iWidth && (uint)i_y < (uint)iHeight);
		return i_y * iLinePixels + i_x;
	}

	//******************************************************************************************
	//
	void* pAddress
	(
		int i_index						// Index of pixel to retrieve.
	) const
	//
	// Returns:
	//		The address of the pixel location, cast to void*.
	//
	//**********************************
	{
		Assert(uint(i_index) < iTotalPixels());
		Assert(pSurface);
		return (uint8*)pSurface + i_index * iPixelBytes();
	}

	//******************************************************************************************
	//
	void* pAddress
	(
		int i_x, int i_y				// Pixel location.
	) const
	//
	// Returns:
	//		The address of the pixel location, cast to void*.
	//
	//**********************************
	{
		return pAddress(iIndex(i_x, i_y));
	}

	//******************************************************************************************
	//
	virtual void Lock()
	//
	// Lock the surface if needed, and set the pSurface pointer so clients can write to it.
	//
	// Notes:
	//		Lock() *must* be called before a surface can be accessed.  For some descendents of
	//		CRasterBase, system requirements make this so.  For the versions defined in this file,
	//		Lock() does nothing, and hopefully the virtual function call won't be too expensive.
	//
	//		Call Unlock() when you are done accessing the surface.
	//
	//		Important: When locking a video memory surface (CRasterVid or CRasterWin), Windows 
	//		is *locked up*.  So keep your drawing as quick as possible between Lock and Unlock.
	//
	//		Note: When debugging, this can be a real problem.  Therefore, it is advisable in
	//		debug mode to create all video surfaces in system memory.  See constructors for
	//		CRasterVid and CRasterWin.
	//
	// Example:
	//		extern CRaster* pras;
	//		...
	//		pras->Lock();
	//		for (int i = 0; i < 100; i++)
	//			pras->PutPixel(i, i, 0);
	//		pras->Unlock();
	//
	//**********************************
	{
		// Default version does nothing.
	}

	//******************************************************************************************
	//
	virtual bool bIsLocked()
	//
	// Returns 'true' if the raster's surface is locked.
	//
	//**********************************
	{
		return false;
	}

	//******************************************************************************************
	//
	virtual void Unlock()
	//
	// Unlocks the surface.  See notes for Lock() above.
	//
	//**********************************
	{
		// Default version does nothing.
	}

	//******************************************************************************************
	//
	virtual void* pd3dtexGet
	(
	)
	//
	// Returns the Direct3D texture pointer.
	//
	//**************************************
	{
		// The default version does nothing.
		return 0;
	}

	//******************************************************************************************
	//
	virtual bool bUpload
	(
	)
	//
	// Uploads a Direct3D texture to the video card.
	//
	// Returns 'true' if the upload was successful.
	//
	// Notes:
	//		The upload may not be successful if there is not enough memory on the card for the
	//		texture.
	//
	//**************************************
	{
		// The default version does nothing.
		return true;
	}

	//******************************************************************************************
	//
	virtual bool bUpload
	(
		int i_x,     int i_y,				// Rectangular region.
		int i_width, int i_height
	)
	//
	// Uploads a rectangular region of a Direct3D texture to the video card.
	//
	// Returns 'true' if the upload was successful.
	//
	//**************************************
	{
		// The default version does nothing.
		return true;
	}

	//******************************************************************************************
	//
	virtual void RemoveFromVideo
	(
	)
	//
	// Removes the Direct3D texture from the video card.
	//
	//**************************************
	{
		// The default version does nothing.
	}

	//******************************************************************************************
	//
	virtual void RemoveFromSystem
	(
	);
	//
	// Removes the Direct3D system memory texture.
	//
	//**************************************

	//******************************************************************************************
	//
	virtual uint32 u4GetTypeID
	(
	) const;
	//
	// Returns a type ID that is unique for a specific texture width, height and, in the case
	// of Direct3D, pixel format (e.g., alpha format).
	//
	//**************************************

	//******************************************************************************************
	//
	virtual bool bRestore
	(
	)
	//
	// Restores the Direct3D texture to the video card.
	//
	// Returns 'true' if the restore operation was successful.
	//
	//**************************************
	{
		// The default version does nothing.
		return true;
	}

	//******************************************************************************************
	//
	virtual bool bVerifyConstruction
	(
	) const;
	//
	// Returns 'true' if this raster was constructed successfully.
	//
	//**************************************

	//******************************************************************************************
	//
	void UpdateTilingMasks();
	//
	// The width,height or some other property of the texture has changed,
	// we need to update the tiling info.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual bool bD3DRaster
	(
	) const
	//
	// Returns 'true' if the raster is a D3D texture.
	//
	//**********************************
	{
		return false;
	}

	//
	// CFetchable overrides.  (for prefetching)
	//
	//*****************************************************************************************
	virtual void OnPrefetch(bool b_in_thread) const;

	//*****************************************************************************************
	virtual void OnFetch();

	//*****************************************************************************************
	virtual void OnUnfetch();

	//*****************************************************************************************
	virtual int iSize() const
	{
		return sizeof(CRasterBase);
	}


protected:

	//******************************************************************************************
	//
	// Constructor (prevents instantiation).
	//

	CRasterBase()
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SetRaster
	(
		int i_width, int i_height, int i_bits, 	// Dimensions of the raster.
		int i_stride = 0						// Number of bytes between lines.
												// If 0, determined from i_width and i_bits.
	);
	//
	// Sets up internal info.
	//
	//**********************************
};

//**********************************************************************************************
//
template<class BASE, class PIX> class CRasterBaseT: public BASE
//
// Prefix: rastb
//
// A raster class that has inline access functions for the pixel type PIX.
// BASE must be CRasterBase or a descendent.
//
// Serves as a base class for instantiable rasters.
//
// The pixels can be accessed in 3 ways:
//		1: Random access by (x, y):
//				ras1.tPix(i_x, i_y) = ras2.tPix(i_x, i_y);
//		2: Sequential access by index (i):
//				int i_index = ras.iIndex(i_x, i_y);
//				for (int i = 0; i < ras.iWidth; i++)
//					ras.tPix(i_index + i) = pix_val;
//		3: Sequential access by pointer:
//				PIX* ppix = &ras.tPix(i_x, i_y);
//				for (int i = 0; i < ras.iWidth; i++)
//					*ppix++ = pix_val;
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	PIX* ppixSurface() const
	//
	// Returns:
	//		The address of the raster surface, as a PIX*.
	//
	//**********************************
	{
		Assert(pSurface);
		return reinterpret_cast<PIX*>(pSurface);
	}

	//******************************************************************************************
	//
	const PIX& tPix
	(
		int i_index						// Index into the raster surface.
	) const
	//
	// Returns:
	//		A reference to the pixel location.
	//
	//**********************************
	{
		Assert(uint(i_index) < iTotalPixels());
		return ppixSurface()[i_index];
	}

	//******************************************************************************************
	//
	PIX& tPix
	(
		int i_index						// Index into the raster surface.
	)
	//
	// Returns:
	//		A reference to the pixel location.
	//
	//**********************************
	{
		Assert(uint(i_index) < iTotalPixels());
		return ppixSurface()[i_index];
	}

	//******************************************************************************************
	//
	const PIX& tPix
	(
		int i_x, int i_y				// Pixel location.
	) const
	//
	// Returns:
	//		The address of the pixel location.
	//
	//**********************************
	{
		return tPix(iIndex(i_x, i_y));
	}

	//******************************************************************************************
	//
	PIX& tPix
	(
		int i_x, int i_y				// Pixel location.
	)
	//
	// Returns:
	//		The address of the pixel location.
	//
	//**********************************
	{
		return tPix(iIndex(i_x, i_y));
	}

protected:
	
	CRasterBaseT()
	{
	}

};

/*
//**********************************************************************************************
//
template<class PIX> class pRasterTCast
//
// A class which is used like a cast operator, to return a CRasterT<PIX>*, 
// which provides an optimised interface to any CRasterBase of the correct pixel type.
// Note: the pointer returned just provides a different interface to the same raster.
//
// Example:
//		CRaster*	ras;				// We know this is an 8-bit raster.
//
//		// Obtain a CRasterT<uint8> interface to the raster.  Now we go faster.
//		CRasterT<uint8>* pras8 = pRasterTCast<uint8>(pras);
//		Assert(pras8);
//
//**************************************
{
protected:
	CRasterT<PIX>*	pRasterT;		// Interface to a raster.

public:

	//******************************************************************************************
	pRasterTCast
	(
		CRasterBase* prasb			// Raster to convert.
	)
		: pRasterT(reinterpret_cast< CRasterT<PIX>* >(prasb))
	{
		AlwaysAssert(sizeof(PIX) == prasb->iPixelBytes());
	}

	//******************************************************************************************
	operator CRasterT<PIX>*()
	{
		return pRasterT;
	}
};
*/

//**********************************************************************************************
//
class CRaster: public CRasterBase
//
// Prefix: ras
//
// A generic raster that can have a surface of type uint8, uint16, uint24, or uint32.
// Its functions read and write values of type TPixel, a 32-bit int that is converted to
// whatever type the raster contains.
//
// This class is not instantiable, as it has no constructors.
//
// The pixels can be accessed in 2 ways:
//		1: Random access by (x, y):
//				ras1.PutPixel(i_x, i_y, ras2.pixGet(i_x, i_y));
//		2: Sequential access by index (i):
//				int i_index = ras.iIndex(i_x, i_y);
//				for (int i = 0; i < ras.iWidth; i++)
//					ras.PutPixel(i_index + i, pix_val);
//
//**************************************
{
public:

	//**********************************************************************************************
	//
	class CRasterSurface
	//
	// Prefix: rsf
	//
	// Class utilised by CRaster to virtually provide access to any surface type.
	//
	//**************************************
	{
	protected:
		CRasterBase&	rasbOwner;

	public:

		//******************************************************************************************
		//
		// Constructor.
		//

		CRasterSurface
		(
			CRasterBase& rasb_owner
		);

		CRasterSurface
		(
			CRasterBase& rasb_owner,
			int i_bits
		);

		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		virtual TPixel pixGet
		(
			int i_index						// Pixel location.
		) const
		//
		// Returns:
		//		The pixel value at the given location.
		//
		//**********************************
		{
			Assert(0);
			return 0;
		}

		//******************************************************************************************
		//
		virtual void PutPixel
		(
			int i_index, 					// Pixel location.
			TPixel pix						// Pixel to write.
		)
		//
		// Writes the pixel to the given location.
		//
		//**********************************
		{
			Assert(0);
		}

		//******************************************************************************************
		//
		virtual void Fill
		(
			int i_index,					// Starting index to write.
			int i_count,					// Number of pixels.
			TPixel pix						// Pixel value to write.
		)
		//
		// Writes the pixel to the given locations.
		//
		//**********************************
		{
			Assert(0);
		}

		//******************************************************************************************
		//
		virtual void Rect
		(
			const SRect& rect,
			TPixel pix
		)
		//
		//**********************************
		{
			Assert(0);
		}
	};

protected:

	CRasterSurface	RasterSurface;		// Used to access the surface virtually.

public:

	CPixelFormat	pxf;				// Pixel and palette information.
	rptr<CRaster>   prasLink;			// Linked raster.

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual void Link
	(
		rptr<CRaster> pras = rptr0	// Pointer to the raster to create the link with.
	);
	//
	// Creates a double link between this raster and another. If the parameter is a null
	// pointer, any current links are broken.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual CRasterD3D* prasd3dGet
	(
	);
	//
	// Returns a pointer to this raster as a Direct3D raster if it is one, otherwise returns
	// a null pointer.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual int iSurfaceMemBytes
	(
	);
	//
	// Returns the number of bytes allocated for the surface memory for the raster.
	//
	//**********************************

	//******************************************************************************************
	//
	TPixel pixGet
	(
		int i_index						// Pixel location.
	) const
	//
	// Returns:
	//		The pixel value at the given location.
	//
	//**********************************
	{
		return RasterSurface.pixGet(i_index);
	}

	//******************************************************************************************
	//
	TPixel pixGet
	(
		int i_x, int i_y				// Pixel location.
	) const
	//
	// Returns:
	//		The pixel value at the given location.
	//
	//**********************************
	{
		return pixGet(iIndex(i_x, i_y));
	}

	//******************************************************************************************
	//
	void PutPixel
	(
		int i_index, 					// Pixel location.
		TPixel pix						// Pixel to write.
	)
	//
	// Writes the pixel to the given location.
	//
	//**********************************
	{
		RasterSurface.PutPixel(i_index, pix);
	}

	//******************************************************************************************
	//
	void PutPixel
	(
		int i_x, int i_y, 				// Pixel location.
		TPixel pix						// Pixel to write.
	)
	//
	// Writes the pixel to the given location.
	//
	//**********************************
	{
		PutPixel(iIndex(i_x, i_y), pix);
	}

	//******************************************************************************************
	//
	virtual void Rect
	(
		const SRect& rect,
		TPixel pix
	)
	//
	//**********************************
	{
		RasterSurface.Rect(rect, pix);
	}

	//******************************************************************************************
	//
	virtual void Clear
	(
		TPixel pix = 0					// The colour to clear to.
	);
	//
	// Clear the entire raster to value pix.
	//
	//**********************************

	//
	// Delegate pixel/colour conversion to CPixelFormat member.
	//

	//******************************************************************************************
	//
	TPixel pixFromColour
	(
		CColour clr
	) const
	//
	// Returns:
	//		The pixel corresponding to the colour, according to this pixel format.
	//
	//**********************************
	{
		return pxf.pixFromColour(clr);
	}

	//******************************************************************************************
	//
	CColour clrFromPixel
	(
		TPixel pix
	) const
	//
	// Returns:
	//		The colour represented by this pixel, according to this pixel format.
	//
	//**********************************
	{
		return pxf.clrFromPixel(pix);
	}

	//******************************************************************************************
	//
	TPixel pixScale
	(
		TPixel pix,						// A pixel in the raster's pixel format.
		int i_intensity,				// An intensity value to scale by.
		int i_bits						// The number of bits for the scale.
	) const
	//
	// Returns:
	//		The colour represented by this pixel, according to this pixel format.
	//
	//**********************************
	{
		return pxf.pixScale(pix, i_intensity, i_bits);
	}

	//******************************************************************************************
	//
	virtual void AttachPalette
	(
		CPal* ppal,						// Palette to attach.
		CPixelFormat* ppxf = 0			// Pixel format this palette corresponds to; 0 if none.
	);
	//
	// Attach the palette (and pixel format if specified) to this raster, for proper colour
	// conversion.  Realise the palette on the screen if this is a displayed raster.
	//
	// Notes:
	//		This sets the ppalAttached member to point to the palette.  Both internal and
	//		external routines can use this member for colour conversion.
	//
	//		This only has effect for 8-bit rasters, but can be called harmlessly for other
	//		rasters.
	//
	//		The ppxf argument enables fast pixFromColour conversions.  If you want to set this
	//		raster's palette to a colour cube, where each RGB component has a bit-range in an
	//		8-bit pixel, then create the palette with the correct colours, and pass it in along
	//		with the corresponding CPixelFormat object.  Typically, the format will be &pxf332.
	//
	//		The pixFromColour routine for 8-bit palettes checks to see whether a pixel format
	//		has been set, and if so, uses it for the conversion, which is fairly fast.  Otherwise,
	//		it must do a slow palette search to find the nearest colour.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual void Blit
	(
		int i_dx, int i_dy,				// The destination of the blit in this.
		CRaster& ras_src,				// The source raster to blit from.
		SRect* prect_src = 0,			// The source rectangle, or 0 for the entire surface.
		bool b_clip = true,				// Whether to clip the coordinates to valid values.
		bool b_color_key = 0,			// Whether to do a colorkey (transparent) blit.
		TPixel pix_color_key = 0		// The source pixel value which is transparent.
	)
	//
	// Copy a rectangle of pixels from ras_src to this.
	//
	//**********************************
	{
		Assert(0);
	}

	//******************************************************************************************
	// Default implementation of the change raster member. A base raster cannot be changed
	// so assert. 
	// If a raster derived from this class can have its contents changed by using a CRasterMem 
	// as a reference then it should implement this member.
	//
	// Other function may be required to change rasters with other references this one is primarily
	// used by the second stage of the bump map packing
	//
	virtual void ChangeRaster(rptr<CRasterMem> pras_reference, bool b_transparent = false)
	{
		Assert(0);
	}

protected:

	//******************************************************************************************
	//
	// Constructor (prevents instantiation).
	//

	CRaster();

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void SetRaster
	(
		int i_width, int i_height, int i_bits, 	// Dimensions of the raster.
		int i_stride = 0,						// Stride is in texels
												// If 0, determined from i_width and i_bits.
		CPixelFormat* ppxf = 0					// Pixel format.
	);
	//
	// Sets up internal info.
	//
	// Calls CRasterBase::SetRaster, and also processes pixel format info.
	//
	//**********************************
};


enum EMemType
// prefix: emt
{
	emtNormal = 0,			// memory is allocated with new
	emtFixed,				// memory is allocated from a fixed heap
	emtTexManVirtual,		// memory is allocated from a virtual fixed heap within the texture manager
	emtTexManSubVirtual,	// memory is allocated from a virtual sub allocator within the texture manager
	emtNone					// has no memory
};


//**********************************************************************************************
//
class CRasterMem: public CRaster
//
// Prefix: rasm
//
// An instantiable version of CRaster that creates a plain memory surface.
//
// Blitting is currently unimplemented.
//
//**************************************
{
public:
	rptr<CRaster>				prasReferenced;	// Hold a pointer to the shared raster if any
												// (this maintains a reference, so the shared raster
												// is not deleted).

	rptr<CPackedRaster>			ppckrParent;	// texture manager who is to be notified when the raster
												// is deleted, or NULL for don't care.

	EMemType					emtHeapType;	// type of allocated memory of this raster

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	// Constructor using a unique surface.
	// Stride must now be specified, but 0 can be passed to force it to be calculated
	// Stride is in texels NOT bytes
	CRasterMem
	(
		int i_width, int i_height, int i_bits,	int i_stride, // Dimensions of the raster to create.
		CPixelFormat* ppxf = 0,
		EMemType emt = emtNormal		// Use the global run time heap for memory allocations.
	);

	// Constructor using a shared surface from an existing memory raster raster.
	CRasterMem
	(
		rptr<CRaster> pras_source,				// Raster to reference.
		const SRect& rc,						// Rectangle in raster to reference.
		rptr<CPackedRaster> ppckr = rptr0,		// Class that packed the sub raster
		CPixelFormat* ppxf = 0					// if this is 0 then it will use the pxf of pras_source
	);

	// Constructor using existing raw data
	CRasterMem
	(
		void* pv_adr,							// address of the data
		int	i_width,
		int i_height,					
		int i_bits, 
		int i_stride,
		CPixelFormat* ppxf,
		EMemType emt							// type of memory that the data is in
	);

	virtual ~CRasterMem();

	//******************************************************************************************
	virtual void ChangeRaster
	(
		rptr<CRasterMem>	pras_ref,
		bool				b_transparent = false
	);

	//******************************************************************************************
	void DeleteRasterMemory
	(
	);

protected:
	
	CRasterMem()
		: emtHeapType(emtNormal)
	{
	}
};

//**********************************************************************************************
//
template<class PIX> class CRasterMemT: public CRasterBaseT<CRasterMem, PIX>
//
// Prefix: rasmt
//
// A slightly different form of CRasterT<PIX>.  This version is derived from CRaster, so PIX
// can only be of integral type.  It thus provides both a generic pixel access interface (via
// CRaster), and an inline specific access interface (via CRasterBaseT).
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructor.
	//

	//******************************************************************************************
	CRasterMemT
	(
		int i_width, int i_height,		// Dimensions of the raster to create.
		CPixelFormat* ppxf = 0			// Optional pixel format to use.
	)
	//
	//**********************************
	{
		new(static_cast<CRasterMem*>(this)) CRasterMem(i_width, i_height, 8 * sizeof(PIX), 0, ppxf);
	}

protected:
	
	CRasterMemT()
	{
	}
};


//**********************************************************************************************
// Set the define below to 'true' if you want rasters to be stippled with the mip colour.
// Mip colours are defined to be:
//
//			RED		-		LEVEL 0 (top level)
//			GREEN	-		LEVEL 1
//			BLUE	-		LEVEL 2
//			YELLOW	-		LEVEL 3
//			MAGENTA	-		LEVEL 4
//			CYAN	-		LEVEL 5 (smallest)
//
// You can see what is the smallest mip level by forcing the renderer to use only the smallest
// mip. The colours that you see represent the smallest mip, this can be useful when you wonder
// why a polygon is staying red - it is probably because red is the only map.
#define bSTIPPLE_MIP_LEVELS (0)

#endif

