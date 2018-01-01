/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CPixelFormat
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/View/Pixel.hpp                                                    $
 * 
 * 8     97/06/12 3:13p Pkeet
 * Made colour component formats public instead of protected.
 * 
 * 7     97/01/10 17:46 Speter
 * Added const specifier.
 * 
 * 6     96/10/14 15:19 Speter
 * Added CPal pointer to CPixelFormat, so it now contains all info necessary for pixel/colour
 * conversion.
 * Made conversion functions virtual, so that subclasses can perform optimal conversion.
 * Implemented subclasses.
 * 
 * 
 * 5     96/08/15 18:44 Speter
 * Now must call CColour() explicitly.
 * 
 * 4     96/07/18 18:16 Speter
 * Added mono pixel format.
 * 
 * 3     96/07/01 21:35 Speter
 * Code review: Moved uint24 from Raster.
 * 
 * 2     96/06/12 20:53 Speter
 * Code review changes:
 * Fixed up comments.
 * Added function headers to CPixelFormat members (you guys didn't catch that).
 * Changed TPixel from int32 back to uint32.
 * Moved Prefix fields to top of class.
 * Made member functions const where appropriate.
 * Changed CCompMask::u4PixelFromColour to take a CColour argument.
 * 
 * 1     96/06/04 16:13 Speter
 * First version.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_PIXEL_HPP
#define HEADER_LIB_VIEW_PIXEL_HPP

#include "Colour.hpp"
#include "Palette.hpp"

#include <memory.h>

// A pixel value in the actual format written to a raster surface.
// Prefix: pix
typedef uint32	TPixel;

//**********************************************************************************************
//
class CPixelFormat
//
// Contains info on converting an RGB pixel type to and from a CColour.
// Contains both a palette pointer for paletted formats, and component bit info for RGB formats.
//
// The conversion functions are virtual so that optimised subclasses can be implemented.
// This base class will handle generic RGB formats.  Subclasses handle paletted formats, and
// optimised code for specific RGB formats.
//
// CPixelFormat itself is a "virtual" class.  When constructed, it turns itself into one of
// its optimised subclasses depending on the pixel format.
//
// Prefix: pxf
//
//**************************************
{
public:

	//******************************************************************************************
	//
	class CCompPos
	//
	// Prefix: cpos
	//
	// A class which describes the bit format of a single component of an RGB pixel.  An RGB pixel
	// is not a palette index, but a pixel with specific bit ranges allocated to each RGB component.
	//
	// This is used for pixel conversion in the base CPixelFormat class, but not in optimised
	// subclasses.
	//
	//**************************************
	{
	public:
		uint8	u1Pos;				// Starting bit position of component.
		uint8	u1WidthDiff;		// 8 minus number of bits in component.

		//******************************************************************************************
		//
		// Constructors.
		//

		CCompPos()
		{
		}

		CCompPos(uint8 u1_pos, uint8 u1_width) :
			u1Pos(u1_pos), u1WidthDiff(8 - u1_width)
		{
			Assert(u1_width <= 8);
		}

		// Convert a bitmask into bit position info.
		CCompPos(uint32 u4_mask);

		//******************************************************************************************
		//
		// Operators.
		//

		bool operator ==(CCompPos cpos) const
		{
			return u1Pos == cpos.u1Pos && u1WidthDiff == cpos.u1WidthDiff;
		}

		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		uint8 u1ColourMask
		(
			TPixel	pix					// A pixel to extract colour info from.
		) const
		//
		// Returns:
		//		The 8-bit colour value corresponding to this pixel component.
		//
		//******************************
		{
			return (pix >> u1Pos) << u1WidthDiff;
		}

		//******************************************************************************************
		//
		uint32 u4PixelMask
		(
			uint8	u1_colour			// An 8-bit colour component value.
		) const
		//
		// Returns:
		//		The mask containing the u1_colour value in this pixel component format.
		//
		//******************************
		{
			return (u1_colour >> u1WidthDiff) << u1Pos;
		}
	};

public:
	CPal*		ppalAttached;			// Palette attached to this raster, if any.
	uint8		u1Bits;					// Number of bits per pixel.

	CCompPos	cposR, cposG, cposB;	// Position information for each component.

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	CPixelFormat()
		: ppalAttached(0), u1Bits(0)
	{
	}

	// Set default pixel format for a given bit depth.
	CPixelFormat
	(
		int i_bits
	);

	// Set pixel format info for a bit depth, and explicit bit masks.
	CPixelFormat
	(
		int i_bits,
		uint32 u4_rmask, uint32 u4_gmask, uint32 u4_bmask
	);

	// Copy constructor calls assignment operator.
	CPixelFormat(const CPixelFormat& pxf)
	{
		*this = pxf;
	}

	//******************************************************************************************
	//
	// Conversion operators.
	//

	operator int() const
	{
		// Return whether pixel format is initialised.
		return u1Bits;
	}

	//******************************************************************************************
	//
	// Operators.
	//

	CPixelFormat& operator =(const CPixelFormat& pxf)
	{
		// Memcpy the entire object.  This is because CPixelFormat is a virtual class,
		// and we want the vtables to be copied with the object.
		memcpy(this, &pxf, sizeof *this);
		return *this;
	}

	bool operator ==(const CPixelFormat& pxf) const
	{
		return u1Bits == pxf.u1Bits &&
			   cposR  == pxf.cposR  && 
			   cposG  == pxf.cposG  && 
			   cposB  == pxf.cposB;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual TPixel pixFromColour
	(
		CColour clr
	) const;
	//
	// Returns:
	//		The pixel corresponding to the colour, according to this pixel format.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual CColour clrFromPixel
	(
		TPixel pix
	) const;
	//
	// Returns:
	//		The colour represented by this pixel, according to this pixel format.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual TPixel pixScale
	(
		TPixel pix,						// A pixel in this pixel format.
		int i_intensity,				// An intensity value to scale by.
		int i_bits						// The number of bits in the intensity.
	) const;
	//
	// Returns:
	//		pix, scaled by the value i_intensity, and shifted right by i_bits.
	//
	//**********************************

protected:

	// Constructor used internally by public constructors of this and derived classes.
	CPixelFormat
	(
		int i_bits,
		int i_rbits, int i_gbits, int i_bbits,
		bool b_dummy					// Used to resolve overload ambiguity.
	);

};

//**********************************************************************************************
//
class uint24
//
// Prefix: u3
//
// This fabulous class can be used to access 24-bit rasters.  Simply treat the raster as an 
// array of type uint24.
//
// The class is simply a 3-byte array, with conversion to and from uint32.
// 
// This relies on sizeof(uint24) == 3, which is true in MS VC++ 4.0.
//
//**************************************
{
protected:
	uint8	u1Bytes[3];

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	uint24()
	{
		Assert(sizeof(uint24) == 3);
	}

	uint24(uint32 u4)
	{
		*this = u4;
	}

	//******************************************************************************************
	//
	// Assignment operator.
	//

	uint24& operator =(uint32 u4)
	{
		// This relies on little-firstian bit order.
		uint8*	pu1 = (uint8*) &u4;

		u1Bytes[0] = pu1[0];
		u1Bytes[1] = pu1[1];
		u1Bytes[2] = pu1[2];

		return *this;
	}

	//******************************************************************************************
	//
	// Conversion.
	//

	operator uint32() const
	{
		uint32	u4 = 0;
		uint8*	pu1 = (uint8*) &u4;

		pu1[0] = u1Bytes[0];
		pu1[1] = u1Bytes[1];
		pu1[2] = u1Bytes[2];

		return u4;
	}
};

#endif
