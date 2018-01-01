/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Pixel.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Pixel.cpp                                                    $
 * 
 * 8     98/07/22 14:41 Speter
 * Added macro for pixFromColour calculations, allowing optional rounding (disabled).
 * 
 * 7     98.07.17 6:35p Mmouni
 * Added 4444 pixel format.  It is now the default for unspecified 16-bit textures.
 * 
 * 6     96/10/18 19:03 Speter
 * Took out include of <new.h>, now that it's in Common.hpp.
 * 
 * 5     96/10/14 15:19 Speter
 * Added CPal pointer to CPixelFormat, so it now contains all info necessary for pixel/colour
 * conversion.
 * Made conversion functions virtual, so that subclasses can perform optimal conversion.
 * Implemented subclasses.
 * 
 * 
 * 4     96/07/18 18:16 Speter
 * Added mono pixel format.
 * 
 * 3     96/06/12 20:53 Speter
 * Code review changes:
 * Changed comments.
 * 
 * 2     96/06/06 16:52 Speter
 * Added comments and an Assert.
 * 
 * 1     96/06/04 16:13 Speter
 * First version.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Pixel.hpp"

//**********************************************************************************************
//
inline uint uShift1R
(
	uint u_val,
	uint8 u1_shift
)
//
//**************************************
{
	// Round it up.
//	u_val += 1 << (u1_shift-1);
//	SetMin(u_val, 255);
	return u_val >> u1_shift;
}

//**********************************************************************************************
//
class CPixelFormatPalette: public CPixelFormat
//
// Subclass of CPixelFormat which handles formats with a palette.
//
//**************************************
{
public:

	//******************************************************************************************
	CPixelFormatPalette()
		: CPixelFormat(8, 0, 0, 0, true)
	{
		// Make sure it's the same size as its base class, because we copy different version
		// of CPixelFormat around to each other.
		Assert(sizeof(*this) == sizeof(CPixelFormat));
	}

	//******************************************************************************************
	TPixel pixFromColour(CColour clr) const
	{
		// Do a palette search to find best match to clr.
		Assert(ppalAttached);
		return ppalAttached->u1MatchEntry(clr);
	}

	//******************************************************************************************
	CColour clrFromPixel(TPixel pix) const
	{
		// Just look up the palette colour.
		Assert(ppalAttached);
		Assert(pix < 256);
		return ppalAttached->aclrPalette[pix];
	}
};

//**********************************************************************************************
//
class CPixelFormat888: public CPixelFormat
//
// A CPixelFormat subclass optimised for 888 RGB format; pixels are identical to CColour.
//
//**************************************
{
public:

	//******************************************************************************************
	CPixelFormat888(int i_bits) :
		CPixelFormat(i_bits, 8, 8, 8, true)
	{
		Assert(sizeof(*this) == sizeof(CPixelFormat));
	}

	//******************************************************************************************
	TPixel pixFromColour(CColour clr) const
	{
		return clr.u4Value;
	}

	//******************************************************************************************
	CColour clrFromPixel(TPixel pix) const
	{
		return CColour(pix, 0);
	}
};

//**********************************************************************************************
//
class CPixelFormat565: public CPixelFormat
//
// A CPixelFormat subclass hard-coded for 565 format.
//
//**************************************
{
public:

	//******************************************************************************************
	CPixelFormat565() :
		CPixelFormat(16, 5, 6, 5, true)
	{
		Assert(sizeof(*this) == sizeof(CPixelFormat));
	}

	//******************************************************************************************
	TPixel pixFromColour(CColour clr) const
	{
		return (uShift1R(clr.u1Red,   3) << 11) |
			   (uShift1R(clr.u1Green, 2) <<  5) |
			    uShift1R(clr.u1Blue,  3);
	}

	//******************************************************************************************
	CColour clrFromPixel(TPixel pix) const
	{
		return CColour
		(
			uint8((pix >> 11) << 3),
			uint8((pix >>  5) << 2),
			uint8( pix        << 3)
		);
	}
};

//**********************************************************************************************
//
class CPixelFormat555: public CPixelFormat
//
// A CPixelFormat subclass hard-coded for 555 format.
//
//**************************************
{
public:

	//******************************************************************************************
	CPixelFormat555() :
		CPixelFormat(16, 5, 5, 5, true)
	{
		Assert(sizeof(*this) == sizeof(CPixelFormat));
	}

	//******************************************************************************************
	TPixel pixFromColour(CColour clr) const
	{
		return (uShift1R(clr.u1Red,   3) << 10) |
			   (uShift1R(clr.u1Green, 3) <<  5) |
			    uShift1R(clr.u1Blue,  3);
	}

	//******************************************************************************************
	CColour clrFromPixel(TPixel pix) const
	{
		return CColour
		(
			uint8((pix >> 10) << 3),
			uint8((pix >>  5) << 3),
			uint8( pix        << 3)
		);
	}
};

//**********************************************************************************************
//
class CPixelFormat4444: public CPixelFormat
//
// A CPixelFormat subclass hard-coded for 4444 format.
//
//**************************************
{
public:

	//******************************************************************************************
	CPixelFormat4444() :
		CPixelFormat(16, 4, 4, 4, true)
	{
		Assert(sizeof(*this) == sizeof(CPixelFormat));
	}

	//******************************************************************************************
	TPixel pixFromColour(CColour clr) const
	{
		return (uShift1R(clr.u1Red,   4) << 8) |
			   (uShift1R(clr.u1Green, 4) << 4) |
			    uShift1R(clr.u1Blue,  4);
	}

	//******************************************************************************************
	CColour clrFromPixel(TPixel pix) const
	{
		return CColour
		(
			uint8((pix >> 8) << 4),
			uint8((pix >> 4) << 4),
			uint8( pix       << 4)
		);
	}
};

//**********************************************************************************************
//
// CPixelFormat implementation.
//

	//**********************************************************************************************
	//
	// CPixelFormat::CCompPos implementation.
	//

		//******************************************************************************************
		CPixelFormat::CCompPos::CCompPos(uint32 u4_mask) 
		{
			// Calculate bit positions from u4_mask.
			u1Pos = 0;
			int i_width_diff = 8;

			for (; u4_mask && !(u4_mask & 1); u4_mask >>= 1)
				u1Pos++;
			for (; u4_mask; u4_mask >>= 1)
			{
				Assert(u4_mask & 1);
				i_width_diff--;
			}

			Assert(i_width_diff >= 0);
			u1WidthDiff = i_width_diff;
		}


	//******************************************************************************************
	CPixelFormat::CPixelFormat(int i_bits)
		: ppalAttached(0), u1Bits(i_bits)
	{
		// Construct self according to the default format for each bit depth.
		if (i_bits == 8)
			new(this) CPixelFormatPalette();
		else if (i_bits == 16)
			new(this) CPixelFormat4444();
		else if (i_bits == 24 || i_bits == 32)
			new(this) CPixelFormat888(i_bits);
		else
			Assert(0);
	}

	//******************************************************************************************
	CPixelFormat::CPixelFormat
	(
		int i_bits,
		uint32 u4_rmask, uint32 u4_gmask, uint32 u4_bmask
	) :
		ppalAttached(0), u1Bits(i_bits),
		cposR(u4_rmask), cposG(u4_gmask), cposB(u4_bmask)
	{
		Assert(i_bits % 8 == 0 && bWithin(i_bits, 8, 32));
		if (i_bits < 32)
			Assert((u4_rmask | u4_bmask | u4_gmask) < (1<<i_bits));
		Assert((u4_rmask & u4_gmask) == 0);
		Assert((u4_gmask & u4_bmask) == 0);
		Assert((u4_bmask & u4_rmask) == 0);

		//
		// Compare the bit formats with specific subclasses, and reconstruct ourselves
		// as a subclass if we match.
		//
		if (i_bits == 8)
			new(this) CPixelFormatPalette();
		else if (*this == CPixelFormat888(32))
			new(this) CPixelFormat888(32);
		else if (*this == CPixelFormat888(24))
			new(this) CPixelFormat888(24);
		else if (*this == CPixelFormat565())
			new(this) CPixelFormat565();
		else if (*this == CPixelFormat555())
			new(this) CPixelFormat555();
	}

	//******************************************************************************************
	TPixel CPixelFormat::pixFromColour(CColour clr) const
	{
		return cposR.u4PixelMask(clr.u1Red)   |
			   cposG.u4PixelMask(clr.u1Green) |
			   cposB.u4PixelMask(clr.u1Blue);
	}

	//******************************************************************************************
	CColour CPixelFormat::clrFromPixel(TPixel pix) const
	{
		return CColour(cposR.u1ColourMask(pix),
					   cposG.u1ColourMask(pix),
					   cposB.u1ColourMask(pix));
	}

	//******************************************************************************************
	TPixel CPixelFormat::pixScale(TPixel pix, int i_intensity, int i_bits) const
	{
		return pixFromColour( clrFromPixel(pix).clrScale(i_intensity, i_bits) );
	}

	//******************************************************************************************
	CPixelFormat::CPixelFormat
	(
		int i_bits,
		int i_rbits, int i_gbits, int i_bbits,
		bool b_dummy
	) :
		// An internal non-virtual constructor,
		// used by descendents to initialise cpos fields without invoking virtual construciton.
		ppalAttached(0),
		u1Bits(i_bits),
		cposR(i_bbits + i_gbits, i_rbits), 
		cposG(i_bbits,           i_gbits), 
		cposB(0,                 i_bbits)
	{
	}

