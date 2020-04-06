 /***********************************************************************************************
 *
 * Copyright (c) 1996 DreamWorks Interactive, 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/Bitmap.cpp                                             $
 * 
 * 5     12/16/96 11:22a Gstull
 * Made changes to support quantization to a single palette.
 * 
 * 4     11/15/96 11:08a Gstull
 * Added substantial changes to the GroffExporter including for support of mutiple section
 * files.
 * 
 * 3     11/06/96 7:28p Gstull
 * Added a Copy function to the bitmap IO classes.
 * 
 * 2     11/04/96 8:34p Gstull
 * Latest version of the new bitmap exporter stuff.
 *
 **********************************************************************************************/

#include <assert.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <fstream>

#include "max.h"
#include "bmmlib.h"

#include "StandardTypes.hpp"
#include "Lib/Sys/SmartBuffer.hpp"
#include "Bitmap.hpp"


//**********************************************************************************************
//
CBitmapInfo::CBitmapInfo()
{
	// Initialize the text fields to null.
	strBitmapName[0]     = 0;
	strBitmapDescription = 0;

	// Call the generic initialization function.
	Initialize();
}


//**********************************************************************************************
//
CBitmapInfo::~CBitmapInfo()
{
	// Call the generic initialize function.
	Initialize();
}


//**********************************************************************************************
//
void CBitmapInfo::Initialize()
{
	// Set the bitmap file format to undefined,
 	fmtFileFormat = EFMT_UNDEFINED;

	// Set the generic bitmap parameters to some initial values.
	uBitmapWidth  = 0;
	uBitmapHeight = 0;
	uBitmapDepth  = 0;
	fBitmapAspect = 1.0f;
	fBitmapGamma  = 1.0f;

	// Initialize the palette information to empty values.
	uBitmapPaletteSize	  = 0;
	uBitmapPaletteEntries = 0;

	// Initialize the bitmap name to null.
 	strBitmapName[0] = 0;

	// If there is any memory allocated to the strings, return it.  Is there any?
	if (strBitmapDescription != 0)
	{	
		// There is memory here so deallocate it.
		delete [] strBitmapDescription;

		// Initialize the pointers to null.
		strBitmapDescription = 0;
	}
}


//**********************************************************************************************
//
bool CBitmapInfo::bIsValidDefinition() const
{
	//
	// First check for error conditions, then check for warning conditions.
	// 

	// Is the width reasonable?
	if (uBitmapWidth < 1 || uBitmapWidth > MAX_BITMAP_WIDTH)
	{
		// Return an error.
		return false;
	}

	// Is the height reasonable?
	if (uBitmapHeight < 1 || uBitmapHeight > MAX_BITMAP_HEIGHT)
	{
		// Return an error.
		return false;
	}

	// Is the bitmap depth reasonable?
	switch (uBitmapDepth)
	{
		case 1:
			// Is the palette size correct?
			if (uBitmapPaletteSize != 2)
			{
				// No!  Return an error.
				return false;
			}

			// Is the palette entries in range.?
			if (uBitmapPaletteEntries > 2)
			{
				// No!  Return an error.
				return false;
			}

			break;

		case 4:
			// Is the palette size correct?
			if (uBitmapPaletteSize != 16)
			{
				// No!  Return an error.
				return false;
			}

			// Is the palette entries in range.?
			if (uBitmapPaletteEntries > 16)
			{
				// No!  Return an error.
				return false;
			}

			break;

		case 8:
			// Is the palette size correct?
			if (uBitmapPaletteSize != 256)
			{
				// No!  Return an error.
				return false;
			}

			// Is the palette entries in range.?
			if (uBitmapPaletteEntries > 256)
			{
				// No!  Return an error.
				return false;
			}


			break;

		case 24:
			break;
		default:
			// Return an error.
			return false;
	}

	// We have enough correct information to build a valid bitmap.
	return true;
}


//**********************************************************************************************
//
bool CBitmapInfo::bIsValidRepresentation() const
{
	//
	// First check for error conditions, then check for warning conditions.
	// 

	// Are the basic parameters valid?
	if (!bIsValidDefinition())
	{
		// No!  Return an error.
		return false;
	}

	// Is the bitmap depth reasonable?
	switch (uBitmapDepth)
	{
		case 1:
			// Is the palette size correct?
			if (uBitmapPaletteSize != 2)
			{
				// No!  Return an error.
				return false;
			}

			// Is the palette entries in range.?
			if (uBitmapPaletteEntries > 2)
			{
				// No!  Return an error.
				return false;
			}

			break;

		case 4:
			// Is the palette size correct?
			if (uBitmapPaletteSize != 16)
			{
				// No!  Return an error.
				return false;
			}

			// Is the palette entries in range.?
			if (uBitmapPaletteEntries > 16)
			{
				// No!  Return an error.
				return false;
			}

			break;

		case 8:
			// Is the palette size correct?
			if (uBitmapPaletteSize != 256)
			{
				// No!  Return an error.
				return false;
			}

			// Is the palette entries in range.?
			if (uBitmapPaletteEntries > 256)
			{
				// No!  Return an error.
				return false;
			}


			break;

		case 24:
			break;
		default:
			// Return an error.
			return false;
	}

	// We have enough correct information to build a valid bitmap.
	return true;
}


//**********************************************************************************************
//
void CBitmapInfo::Name(const char* str_bitmap_name)
{
	// Is the user string in range?
	if (strlen(str_bitmap_name) >= MAX_NAME_LENGTH)
		return;

	// Copy the string.
	strcpy(&strBitmapName[0], str_bitmap_name);
}


//**********************************************************************************************
//
bool CBitmapInfo::bDescription(const char* str_description)
{
	// Is there already memory allocated to the string?
	if (strBitmapDescription != 0)
	{
		// Yes! Deallocate it.
		delete [] strBitmapDescription;
	}

	// Did the user supply a valid string?
	if (str_description != 0)
	{
		// Allocate memory for the new string.
		strBitmapDescription = new char[strlen(str_description)+1];

		// Were we successful?
		if (strBitmapDescription == 0)
		{
			// No!  Return a failure result.
			return false;
		}
		
		// Now copy the string.
		strcpy(strBitmapDescription, str_description);
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
void CBitmapInfo::BitmapFormat(EBitmapFormat fmt_file_format)
{
	// Assign the file format.
	fmtFileFormat = fmt_file_format;
}


//**********************************************************************************************
//
void CBitmapInfo::Width(uint u_width)
{
	// Is the new width within a reasonable range?
	if (u_width > 0 && u_width < MAX_BITMAP_WIDTH)
	{
		// Yes!  Then assign it to the member variable.
		uBitmapWidth = u_width;
	}
}


//**********************************************************************************************
//
void CBitmapInfo::Height(uint u_height)
{
	// Is the new width within a reasonable range?
	if (u_height > 0 && u_height < MAX_BITMAP_HEIGHT)
	{
		// Yes!  Then assign it to the member variable.
		uBitmapHeight = u_height;
	}
}

//**********************************************************************************************
//
void CBitmapInfo::Depth(uint u_depth)
{
	// Is this a valid supported bitmap depth?
	switch (u_depth)
	{
		case 1:
			uBitmapDepth = 1;
			uBitmapPaletteSize = 2;
			break;

		case 4:
			uBitmapDepth = 4;
			uBitmapPaletteSize = 16;
			break;

		case 8:
			uBitmapDepth = 8;
			uBitmapPaletteSize = 256;
			break;

		case 24:
			// Yes!  Then assign it to the member variable.
			uBitmapDepth = u_depth;
	}

	// Make sure the new depth doesn't create an illegal condition regarding the maximum number 
	// of palette entries.
	switch (u_depth)
	{
		case 1:
		case 4:
		case 8:
			uBitmapPaletteSize = 256;
			break;

			// Is the bitmap palette entry count greater than the size?
			if (uBitmapPaletteEntries > uBitmapPaletteSize)
			{
				// Yes!  Then truncate the entry count.
				uBitmapPaletteEntries = uBitmapPaletteSize;
			}
	}

}


//**********************************************************************************************
//
void CBitmapInfo::PaletteEntries(uint u_palette_entries)
{
	// Is the requested number of palette entries in range?
	if (u_palette_entries <= uBitmapPaletteSize)
	{
		// Yes!  Then assign it to the member variable.
		uBitmapPaletteEntries = u_palette_entries;
	}
}


//**********************************************************************************************
//
void CBitmapInfo::Gamma(float f_gamma)
{
	// Is the requested gamma value in range?
	if (f_gamma > 0.1f && f_gamma < 10.0f)
 	{
		// Yes!  Then assign it to the member variable.
		fBitmapGamma = f_gamma;
 	}
}


//**********************************************************************************************
//
void CBitmapInfo::Aspect(float f_aspect)
{
	// Is the requested gamma value in range?
	if (f_aspect > 0.5f && f_aspect < 2.0f)
 	{
		// Yes!  Then assign it to the member variable.
		fBitmapAspect = f_aspect;
 	}
}


//**********************************************************************************************
//
const char* CBitmapInfo::strName() const
{
	// Return a pointer to the string.
	return &strBitmapName[0];
}


//**********************************************************************************************
//
const char* CBitmapInfo::strDescription() const
{
	// Return a pointer to the string.
	return &strBitmapDescription[0];
}


//**********************************************************************************************
//
EBitmapFormat CBitmapInfo::fmtBitmapFormat() const
{
	// Return the current file type.
	return fmtFileFormat;
}


//**********************************************************************************************
//
uint CBitmapInfo::uWidth() const
{
	// Return the bitmap width.
	return uBitmapWidth;
}


//**********************************************************************************************
//
uint CBitmapInfo::uHeight() const
{
	// Return the bitmap height.
	return uBitmapHeight;
}


//**********************************************************************************************
//
uint CBitmapInfo::uDepth() const
{
	// Return the bitmap depth.
	return uBitmapDepth;
}


//**********************************************************************************************
//
uint CBitmapInfo::uPaletteSize() const 
{
	// Return the size of the palette.
	return uBitmapPaletteSize;
}


//**********************************************************************************************
//
uint CBitmapInfo::uPaletteEntries() const 
{
	// Return the number of entries in the palette.
	return uBitmapPaletteEntries;
}


//**********************************************************************************************
//
float CBitmapInfo::fGamma() const
{
	// Return the current gamma value.
	return fBitmapGamma;
}


//**********************************************************************************************
//
float CBitmapInfo::fAspect() const
{
	// Return the current aspect ratio.
	return fBitmapAspect;
}


//**********************************************************************************************
//
CBitmapInfo& CBitmapInfo::operator =(CBitmapInfo& bi_src)
{
	// Make sure we are not assigning to ourself.
	if (this != &bi_src)
	{
		// Duplicate all the members in the structure.
		fmtFileFormat		  = bi_src.fmtFileFormat;

		uBitmapWidth		  = bi_src.uBitmapWidth;
		uBitmapHeight         = bi_src.uBitmapHeight;
		uBitmapDepth          = bi_src.uBitmapDepth;
		fBitmapAspect         = bi_src.fBitmapAspect;
		fBitmapGamma          = bi_src.fBitmapGamma;
		
		uBitmapPaletteSize    = bi_src.uBitmapPaletteSize;
		uBitmapPaletteEntries = bi_src.uBitmapPaletteEntries;

		// Copy the bitmap name to the target.
		strcpy(&strBitmapName[0], bi_src.strBitmapName);

		// Does a bitmap name exist in the dest?
		if (strBitmapDescription != 0)
		{
			// Deallocate it.
			delete [] strBitmapDescription;

			// Set the pointer to null.
			strBitmapDescription = 0;
		}

		// Does a bitmap name exist in the source?
		if (bi_src.strBitmapDescription != 0)
		{
			// Allocate the memory in the destination.
			strBitmapDescription = new char[strlen(bi_src.strBitmapDescription)+1];

			// Copy the bitmap name to the destination.
			strcpy(strBitmapDescription, bi_src.strBitmapDescription);
		}
	}

	return *this;
}


//**********************************************************************************************
//
CBitmapImage::CBitmapImage()
{
	// Initialize the image storage pointer.
	ac24TruePixels = 0;

	// Initialize the paletted pixel buffer.
	au1PalettedPixels = 0;

	// Set the buffer size to 0;
	uBufferSize = 0;
}


//**********************************************************************************************
//
CBitmapImage::~CBitmapImage()
{
	// Call the generic initialization routine.
	Initialize();
}


//**********************************************************************************************
//
void CBitmapImage::Initialize()
{
	// Was there any memory used for the 24 bit storage?
	if (ac24TruePixels != 0)
	{
		// Yes!  Then deallocate the memory.
		delete [] ac24TruePixels;

		// Set the pointer to null.
		ac24TruePixels = 0;
	}

	// Was there any memory used for indexed pixel storage?
	if (au1PalettedPixels != 0)
	{
		// Yes!  Then deallocate the memory.
		delete [] au1PalettedPixels;

		// Set the pointer to null.
		au1PalettedPixels = 0;
	}

	// Set the buffer size to 0.
	uBufferSize = 0;

	// Initialize the bitmap info structure.
	biImageBitmapInfo.Initialize();
}


//**********************************************************************************************
//
bool CBitmapImage::bCreate(CBitmapInfo& bi_src, bool b_initialize = false)
{
	// Do we have a valid bitmap definition?
	if (!bi_src.bIsValidDefinition())
	{
		// No! Something is wrong so return an error.
		return false;
	}

	// Make sure we are starting out with a clean structure.
	Initialize();
 
	// Start out by determining how much memory we need.
	int i_pixels = bi_src.uWidth() * bi_src.uHeight();

	// 
	// We are all ready to start filling in the bitmap info structure and allocating 
	// storage for our new bitmap.
	//
	biImageBitmapInfo = bi_src;

	// Setup the width and height fields sice
	// Is this a paletted or true color bitmap?
	if (bi_src.uDepth() > 8)
	{
		// Must be true color.  Attempt to allocate the memory to the bitmap buffer.
		ac24TruePixels = new SColor24[i_pixels];

		// Were we successful?
		if (ac24TruePixels == 0)
		{
			// No!  Then return an error.
			return false;
		}

		// Setup the size of the buffer.
		uBufferSize = (uint) i_pixels * sizeof(SColor24);

		// Did the user request for the bitmap storage to be cleared?
		if (b_initialize)
		{
			// Initialize the bitmap and palette storage.
			memset(ac24TruePixels, 0, i_pixels * sizeof(SColor24));
		}
	}
	else
	{
		// Must be paletted.  Attempt to allocate the memory for the bitmap buffer.
		au1PalettedPixels = new uint8[i_pixels];

		// Were we successful?
		if (au1PalettedPixels == 0)
		{
			// No!  Then return an error.
			return false;
		}

		// Setup the size of the buffer.
		uBufferSize = (uint) i_pixels;

		// Set the palette entry count to 0 since this is a new palette.
		biImageBitmapInfo.PaletteEntries(0);

		// Did the user request the bitmap and palette storage to be cleared?
		if (b_initialize)
		{
			// Initialize the bitmap and palette storage.
			memset(au1PalettedPixels, 0, i_pixels);
			memset(ac24Palette, 0, MAX_PALETTE_SIZE * sizeof(SColor24));
		}
	}

	// We were successful so return an error.
	return true;
}	


//**********************************************************************************************
//
void CBitmapImage::bDestroy()
{
	// Make sure we are starting out with a clean structure.
	Initialize();
}


//**********************************************************************************************
//
bool CBitmapImage::bRangeCheckAndClip(uint u_width, uint u_x, uint& u_length)
{
	// Is the starting point in the range?
	if (u_x >= u_width)
	{
		// No!  The starting point is out of range.  Nothing can be done. Return an error.
		return false;
	}

	// Is the length out of range?
	if (u_x + u_length > u_width)
	{
		// Adjust the length to it's maximum, legal value.
		u_length = (u_width - u_x);
	}

	// Return a successful result, since we now have useable coordinates.
	return true;
}


//**********************************************************************************************
//
uint CBitmapImage::uGetTruePixels(uint u_x, uint u_y, uint u_count, SColor24* ac24_pixels)
{
	// Has a true color bitmap buffer been allocated for the bitmap?
	if (ac24TruePixels == 0)
	{
		// No!  Return a count of 0.
		return 0;
	}

	// Do we have good parameters?
	if (!bRangeCheckAndClip(biImageBitmapInfo.uWidth(), u_x, u_count))
	{
		// No!  Return a count of 0.
		return 0;
	}

	// Setup the offset into the bitmap.
	uint u_offset = u_y * biImageBitmapInfo.uWidth() + u_x;

	// Copy the pixels into the users buffer.
	memcpy(ac24_pixels, &ac24TruePixels[u_offset], u_count * sizeof(SColor24));

	// Return the number of pixels read.
	return u_count;
}


//**********************************************************************************************
//
uint CBitmapImage::uSetTruePixels(uint u_x, uint u_y, uint u_count, SColor24* ac24_pixels)
{
	// Has a true color bitmap buffer been allocated for the bitmap?
	if (ac24TruePixels == 0)
	{
		// No!  Return a count of 0.
		return 0;
	}

	// Do we have good parameters?
	if (!bRangeCheckAndClip(biImageBitmapInfo.uWidth(), u_x, u_count))
	{
		// No!  Return a count of 0.
		return 0;
	}

	// Setup the offset into the bitmap.
	uint u_offset = u_y * biImageBitmapInfo.uWidth() + u_x;

	// Copy the pixels from the users buffer.
	memcpy(&ac24TruePixels[u_offset], ac24_pixels, u_count * sizeof(SColor24));

	// Return the number of pixels set.
	return u_count;
}


//**********************************************************************************************
//
uint CBitmapImage::uGetPalettedPixels(uint u_x, uint u_y, uint u_count, uint8* au1_pixels)
{
	// Has a true color bitmap buffer been allocated for the bitmap?
	if (au1PalettedPixels == 0)
	{
		// No!  Return a count of 0.
		return 0;
	}
	
	// Do we have good parameters?
	if (!bRangeCheckAndClip(biImageBitmapInfo.uWidth(), u_x, u_count))
	{
		// No!  Return a count of 0.
		return 0;
	}

	// Setup the offset into the bitmap.
	uint u_offset = u_y * biImageBitmapInfo.uWidth() + u_x;

	// Copy the pixels into the users buffer.
	memcpy(au1_pixels, &au1PalettedPixels[u_offset], u_count);

	// Return the number of pixels read.
	return u_count;
}


//**********************************************************************************************
//
uint CBitmapImage::uSetPalettedPixels(uint u_x, uint u_y, uint u_count, uint8* au1_pixels)
{
	// Do we have good parameters?
	if (!bRangeCheckAndClip(biImageBitmapInfo.uWidth(), u_x, u_count))
	{
		// No!  Return a count of 0.
		return 0;
	}

	// Setup the offset into the bitmap.
	uint u_offset = u_y * biImageBitmapInfo.uWidth() + u_x;

	// Copy the pixels into the users buffer.
	memcpy(&au1PalettedPixels[u_offset], au1_pixels, u_count);

	// Return the number of pixels read.
	return u_count;
}


//**********************************************************************************************
//
uint CBitmapImage::uGetPaletteEntries(uint u_start, uint u_count, SColor24* ac24_user_palette)
{
	// Do we have good parameters?
	if (!bRangeCheckAndClip(biImageBitmapInfo.uPaletteSize(), u_start, u_count))
	{
		// No!  Return a count of 0.
		return 0;
	}

	// Copy the pixels into the users buffer.
	memcpy(ac24_user_palette, &ac24Palette[u_start], u_count * sizeof(SColor24));

	// Return the number of palette entries read.
	return u_count;
}


//**********************************************************************************************
//
uint CBitmapImage::uSetPaletteEntries(uint u_start, uint u_count, SColor24* ac24_user_palette)
{
	// Has a palette been allocated for this bitmap?
	if (ac24Palette == 0)
	{
		// No!  Return a count of 0.
		return 0;
	}

	// Do we have good parameters?
	if (!bRangeCheckAndClip(biImageBitmapInfo.uPaletteSize(), u_start, u_count))
	{
		// No!  Return a count of 0.
		return 0;
	}

	// Copy the palette entries out of the users buffer.
	memcpy(&ac24Palette[u_start], ac24_user_palette, u_count * sizeof(SColor24));

	//
	// Set the palette entries field to reflect the last written pixel.  This is sort of a
	// hack because we really don't know if all of these palette entries are being used in
	// the actual image.  For now it will cork just fine.
	//
	biImageBitmapInfo.PaletteEntries(u_start+u_count);

	// Return the number of palette entries set.
	return u_count;
}


//**********************************************************************************************
//
bool CBitmapImage::bBitmapInfo(CBitmapInfo& bi_bitmap_info)
{
	// Only allow the user to modify a couple bitmap parameters (Name, Description, Format).
	biImageBitmapInfo.BitmapFormat(bi_bitmap_info.fmtBitmapFormat());
	biImageBitmapInfo.Name(bi_bitmap_info.strName());

	return biImageBitmapInfo.bDescription(bi_bitmap_info.strDescription());
}


//**********************************************************************************************
//
CBitmapInfo& CBitmapImage::biBitmapInfo()
{
	return biImageBitmapInfo;
}


//**********************************************************************************************
//
bool CBitmapImage::bIsPaletted() const
{
	// All paletted bitmaps have a depth which is less than or equal to 8 bits.
	return biImageBitmapInfo.uDepth() <= 8;
}


//**********************************************************************************************
//
bool CBitmapImage::bIsValidImage()
{
	// First check the bitmap information for reasonable values.
	if (!biImageBitmapInfo.bIsValidDefinition())
	{
		// No!  Either the width, height or depth is unreasonable, so return an error.
		return false;
	}

	// Make sure we have at least a string to represent our bitmap.
	if (!biImageBitmapInfo.strName())
	{
		// No!  The user has not requested a name for the bitmap.
		return false;
	}

	// Check to make sure the bitmap image is of the right dimensions.
	uint u_buff_size = biImageBitmapInfo.uWidth() * biImageBitmapInfo.uHeight();

	// Determine the depth of the bitmap and perform any depth related checks.
	switch (biImageBitmapInfo.uDepth())
	{
		// Later add code to handle 1 and 4 bit paletted bitmaps.
	case 1:
	case 4:
	case 8:
		// Make sure we have an storage allocated to the bitmap buffer.
		if (au1PalettedPixels == 0)
		{
			// We don't have any memory associated with the bitmap so return an error.
			return false;
		}

		// Make sure we have an storage allocated to the bitmap palette.
		if (ac24Palette == 0)
		{
			// We don't have any memory associated with the bitmap palette, so return an error.
			return false;
		}

		break;
	case 24:
		// Adjust the size of the pixel buffer for 24 bit pixels.
		u_buff_size  *= 3;

		// Make sure we have an storage allocated to the bitmap buffer.
		if (ac24TruePixels == 0)
		{
			// We don't have any memory associated with bitmap so return an error.
			return false;
		}
		
		break;
	}

	// Check to determine if the pixel buffer is the correct size.  Is it correct
	if (u_buff_size != uBufferSize)
	{
		// No!  Return an error.
		return false;
	}
	
	// This bitmap can be successfully written to disk. 
	return true;
}


//**********************************************************************************************
//
void* CBitmapImage::Data() const
{
	// Are we in paletted mode?
	if (bIsPaletted())
	{
		// Yes!  We are int paletted mode.
		return (void *) &au1PalettedPixels[0];
	}
	else
	{
		// No!  We are in true color mode.
		return (void *) &ac24TruePixels[0];
	}
}


//**********************************************************************************************
//
CBitmapImage& CBitmapImage::operator =(CBitmapImage& bmi_src)
{
	// Make sure we are not assigning to outself.
	if (this != &bmi_src)
	{
		// Start out by making a duplicate of the bitmap image.
		if (!bCreate(bmi_src.biBitmapInfo()))
		{
			// Bad situation has occurred.  We are out of heap memory.  Blow up for now.
			Assert(0);
		}

		// Get a copy of the bitmap info for the host.
		CBitmapInfo bi_bitmap_info = bmi_src.biBitmapInfo();

		// Is this bitmap true color or paletted?
		if (bi_bitmap_info.uDepth() > 8)
		{
			// This is a true color bitmap.  So copy it.
			memcpy(ac24TruePixels, bmi_src.ac24TruePixels, 
				bi_bitmap_info.uWidth() * bi_bitmap_info.uHeight() * sizeof(SColor24));
		}
		else
		{
			// This is a paletted bitmap.  So copy the bitmap and the palette.
			memcpy(ac24TruePixels, bmi_src.ac24TruePixels, bi_bitmap_info.uWidth() * bi_bitmap_info.uHeight());
			memcpy(ac24Palette, bmi_src.ac24Palette, bi_bitmap_info.uPaletteSize() * sizeof(SColor24));
		}
	}


	return *this;
}


//**********************************************************************************************
//
bool CBitmapIO::bLoad(const char* str_bitmap_name, CBitmapImage& bmi_bitmap_image)
{
	CBmpBitmap  bmp_bitmap;
	CGrfBitmap	groff_bitmap;
	//
	// Invoke a series of bitmap sniffers to determine the bitmap type.
	//

	// Is this bitmap a windows BMP bitmap?
	if (bmp_bitmap.bDetectBitmap(str_bitmap_name))
	{
		// Yes!  Then attempt to load it, and return the result.
		return bmp_bitmap.bRead(str_bitmap_name, bmi_bitmap_image);
	}

	// Is it a groff file bitmap
	if (groff_bitmap.bDetectBitmap(str_bitmap_name))
	{
		// Yes!  Then attempt to load it, and return the result.
		return groff_bitmap.bRead(str_bitmap_name, bmi_bitmap_image);
	}

	// The bitmap was unable to be loaded so return an error.
	return false;
}


//**********************************************************************************************
//
bool CBitmapIO::bSave(CBitmapImage& bmi_bitmap_image)
{
	CBmpBitmap		bmp_bitmap;
	CGrfBitmap		groff_bitmap;
	bool			b_result = false;
    CBitmapInfo& 	bi_bitmap_info = bmi_bitmap_image.biBitmapInfo();


	// Determine the type of file we are being requested to create.
	switch (bi_bitmap_info.fmtBitmapFormat())
	{
	case EFMT_UNDEFINED:
		// Bummer!  A file format must be specified for now.
		break;

	case EFMT_BMP:
		// Attempt to write it out.
	    b_result =  bmp_bitmap.bWrite(bmi_bitmap_image);

		break;

	case EFMT_GROFF:
		// Attempt to write it out.
	    b_result =  groff_bitmap.bWrite(bmi_bitmap_image);

		break;
	}
	
	// Return the result.
	return b_result;
}


//**********************************************************************************************
//
bool CBitmapIO::bCopy(const char* str_dst, const char* str_src)
{
	CBitmapImage bmi_bitmap_image;

	// Attempt to load the bitmap.  Were we successful?
	if (!bLoad(str_src, bmi_bitmap_image))
	{
		// No!  Return an error.
		return false;
	}

	// Change the bitmap name.
	CBitmapInfo bi_bitmap_info;
	bi_bitmap_info = bmi_bitmap_image.biBitmapInfo();

	// Change the name in the bitmap image.  Were we successful?
	bi_bitmap_info.Name(str_dst);
	if (!bmi_bitmap_image.bBitmapInfo(bi_bitmap_info))
	{
		// No!  Perhaps the destination name is invalid???  Return an unsuccessful result.
		return false;
	}

	// Attempt to save the bitmap and return the result.
	return bSave(bmi_bitmap_image);
}


/*
//**********************************************************************************************
//
CBitmapUtil::CBitmapUtil()
{
	// Set the quantizer structure to null.
	pqQuantizer = 0;

	// Set the size of the palette to 0.
	uPaletteSize = 0;

	// Set the color packer to null.
	pcpColorPacker = 0;

	// Set the transparency flag to null.
	bTransparency = false;
}


//**********************************************************************************************
//
CBitmapUtil::~CBitmapUtil()
{
	// Was a quantizer allocated?
	if (pqQuantizer != 0)
	{
		// Yes! Delete the quantizer.
		pqQuantizer->DeleteThis();

		// Set the quantizer to null.
		pqQuantizer = 0;
	}

	// Set the palette size to 0.
	uPaletteSize = 0;

	// Was a color packer allocated?
	if (pcpColorPacker != 0)
	{
		// Yes! Delete the color packer.
		pcpColorPacker->DeleteThis();

		// Set the color packer structure to null.
		pcpColorPacker = 0;
	}
}


//**********************************************************************************************
//
bool CBitmapUtil::bExists(const char* str_bitmap_name)
{
	//
	// Attempt to open the file to make sure it exists.
	//

	// Attempt to open the file and see if it exists.
	ifstream ifile(str_bitmap_name, ios::binary | ios::nocreate);

	// Were we able to open the file?
	if (!ifile)
	{
		// No!  Return false.
		return false;
	}

	// Yes!  So close the file and return true.
	ifile.close();

	return true;
}


//**********************************************************************************************
//
bool CBitmapUtil::bCopy(const char* str_dst, const char* str_src)
{
	CBitmapImage bmi_bitmap_image;

	// Attempt to load the bitmap.  Were we successful?
	if (!bLoad(str_src, bmi_bitmap_image))
	{
		// No!  Return an error.
		return false;
	}

	// Change the bitmap name.
	CBitmapInfo bi_bitmap_info;
	bi_bitmap_info = bmi_bitmap_image.biBitmapInfo();

	// Change the name in the bitmap image.  Were we successful?
	bi_bitmap_info.Name(str_dst);
	if (!bmi_bitmap_image.bBitmapInfo(bi_bitmap_info))
	{
		// No!  Perhaps the destination name is invalid???  Return an unsuccessful result.
		return false;
	}

	// Attempt to save the bitmap and return the result.
	return bSave(bmi_bitmap_image);
}


//**********************************************************************************************
//
bool CBitmapUtil::bConfigureQuantizer(uint u_maximum_palette_entries)
{
	// Is the requested palette size in range?
	if (u_maximum_palette_entries < 2 || u_maximum_palette_entries > 256)
	{
		// No!  Return an error.
		return false;
	}

	// Setup the palette entry count.
	uPaletteSize = u_maximum_palette_entries;

	// Does a palette exist? If so, deallocate it.
	if (ac24Palette != 0)
	{
		// Yes!  Delete the palette.
		delete [] ac24Palette;

		// Set the palette pointer to 0.
		ac24Palette = 0;
	}

	// Does a quantizer already exist?
	if (pqQuantizer != 0)
	{
		// Yes!  Deallocate it.
		pqQuantizer->DeleteThis();
	}

	// Attempt to allocate a histogram for the quantizer.  Were we successful?
	if (!pqQuantizer->AllocHistogram())
	{
		// No!  Deallocate the quantizer and return an error.
		pqQuantizer->DeleteThis();

		return false;
	}

	// Does a color packer exist?  If so delete it.
	if (pcpColorPacker != 0)
	{
		// Yes!  Deallocate it.
		pcpColorPacker->DeleteThis();

		// Set the pointer to 0.
		pcpColorPacker = 0;
	}


	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CBitmapUtil::bQuantizePixels(CBitmapImage& bmi_bitmap_image)
{
	// Does a quantizer exist?
	if (pqQuantizer == 0)
	{
		// No!  Return an error.
		return false;
	}

	// Do we have a valid bitmap image?
	if (!bmi_bitmap_image.bIsValidImage())
	{
		// No! Return an error.
		return false;
	}

	// Is this a paletted bitmap?
	if (bmi_bitmap_image.bIsPaletted())
	{
		// Yes! Return an error since we don't support quantization of paletted bitmaps.
		return false;
	}

	//
	// We now know that we have a valid bitmap which is true color.  We can then feed the individual
	// pixels in the bitmap into the quantizer.  
	//

	// Get a copy of the bitmap info structure.
	CBitmapInfo bi_bitmap_info = bmi_bitmap_image.biBitmapInfo();

	// Setup a pixel line buffer.
	SColor24* ac24_scanline = new SColor24[bi_bitmap_info.uWidth()];

	// Were we successful?
	if (ac24_scanline == 0)
	{
		// No! Return an error.
		return false;
	}

	// Loop through all the rows in the bitmap.
	for (uint u_i = 0; u_i < bi_bitmap_info.uWidth(); u_i++)
	{
	}

	// Delete the scanline buffer.
	delete [] ac24_scanline;

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CBitmapUtil::bPalettizeBitmap(CBitmapImage& bmi_dst, CBitmapImage& bmi_dst& u1_palette_size)
{
}


//**********************************************************************************************
//
bool CBitmapUtil::bMerge(CBitmapImage& bmi_dst, CBitmapImage& bmi_texture, CBitmap bmi_opacity)
{
}


//**********************************************************************************************
//
bool CBitmapUtil::bFlip(CBitmapImage& bmi_bitmap_image, bool b_vertical, bool b_horizontal)
{
}


//**********************************************************************************************
//
bool CBitmapUtil::bMeanColor(CBitmapImage& bmi_bitmap_image, SColor24& c24_mean_color)
{
}
*/


//**********************************************************************************************
//
bool CBmpBitmap::bDetectBitmap(const char* str_bitmap_name)
{
	// Attempt to open the file and see if it exists.
	std::ifstream ifile(str_bitmap_name, std::ios::binary | std::ios::_Nocreate);

	// Were we able to open the file?
	if (!ifile)
	{
		// No!  So return an error.
		return false;
	}

	// Determine if we have a Windows BMP bitmap file.
	SFileHeader fh_file_header;

	// Read in the file header.
	ifile.read((char *) &fh_file_header, sizeof(SFileHeader));

	ifile.close();

	// Is this a Windows bitmap?
	return fh_file_header.u2FileType == WINDOWS_BMP_MAGIC_NUMBER;
}


//**********************************************************************************************
//
bool CBmpBitmap::bRead(const char* str_bitmap_name, CBitmapImage& bmi_bitmap_image)
{
	// Do we have a possible string?
	if (str_bitmap_name == 0)
	{
		// No!  This is a null string so return a failure result.
		return false;
	}

	// Attempt to open the bitmap file.
	std::ifstream ifile(str_bitmap_name, std::ios::binary | std::ios::_Nocreate);

	// Were we successful?
	if (!ifile)
	{
		// No!  Return a failure result.
		return false;
	}

	// Determine if we have a Windows BMP bitmap file.
	SFileHeader fh_file_header;

	// Read in the file header.
	ifile.read((char *) &fh_file_header, sizeof(SFileHeader));

	// Is this a Windows bitmap?
	if (fh_file_header.u2FileType != WINDOWS_BMP_MAGIC_NUMBER)
	{
		// No!  CLose the file and return a failure result.
		ifile.close();

		return false;
	}

	// Read in the bitmap header and verify we recognize the version.
	SBitmapHeader bmh_bitmap_header;

	// Read in the file header.
	ifile.read((char *) &bmh_bitmap_header, sizeof(SBitmapHeader));

	// Is this a Windows bitmap?
	if (bmh_bitmap_header.uHeaderSize != sizeof(SBitmapHeader))
	{
		// No!  CLose the file and return a failure result.
		ifile.close();

		return false;
	}

	// Make sure this is an uncompressed BMP file since this is all we can handle now.
	if (bmh_bitmap_header.uCompression != 0 || bmh_bitmap_header.u2Planes != 1)
	{
		// This type of bitmap is not yet supported, so close the file and return an error.
		ifile.close();

		return false;
	}

	// Reset the file pointer to the beginning and read the entire bitmap into a smart buffer.
	CSmartBuffer sb_buffer;
	TBufferHandle bh_handle = sb_buffer.bhCreate(fh_file_header.uFileSize);

	// Were we successful?
	if (bh_handle == 0)
	{
		// No!  Close the file and return an error.
		ifile.close();

		return false;
	}

	// Setup the smart buffer to reflect an entire buffer full of data.
	if (sb_buffer.iLast(bh_handle, fh_file_header.uFileSize-1) == -1)
	{
		// This did not work so close the file and return an error.
		ifile.close();

		return false;
	}
	
	// Get the buffer address.
	uint8* u1_buffer = (uint8 *) sb_buffer.Address(bh_handle);

	// Were we successful?
	if (u1_buffer == 0)
	{
		// No!  Close the file and return an error.
		ifile.close();

		return false;
	}

	// Seek to the start of the file.
	ifile.seekg(0);

	// Read in the entire file.
	ifile.read((char*)u1_buffer, fh_file_header.uFileSize);

	// Close the file since the image is now entirely located in the smart buffer.
	ifile.close();

	// Verify the file size matches the smart buffer count.
	if (sb_buffer.iCount(bh_handle) != (int) fh_file_header.uFileSize)
	{
		// Return an error.
		return false;
	}

	//
	// We now have the entire bitmap in a smartbuffer, so start out by constructing a bitmap
	// info structure which can then be used to build the bitmap image.  Fortunately, this
	// only requires a couple of fields out of the file header.
	//

	CBitmapInfo bi_bitmap_info;

	bi_bitmap_info.Width(bmh_bitmap_header.lWidth);
	bi_bitmap_info.Height(bmh_bitmap_header.lHeight);
	bi_bitmap_info.Depth(bmh_bitmap_header.u2BitsPerPixel);
	bi_bitmap_info.BitmapFormat(EFMT_BMP);
	bi_bitmap_info.Name(str_bitmap_name);

	// Attempt to allocate the storage for the bitmap.  Were we successful?
	if (!bmi_bitmap_image.bCreate(bi_bitmap_info, true))
	{
		// No!  Return an error.
		return false;
	}

	//
	// We now have a valid bitmap image to store our bitmap in, so we now need to load the data
	// from the smart buffer into the image.  This will largely depend on whether this bitmap
	// is a true color bitmap or a paletted bitmap.
	//

	// Is this a paletted bitmap?
	if (bmh_bitmap_header.u2BitsPerPixel <= 8)
	{
		// Make sure the number of colors field is properly setup.
		if (bmh_bitmap_header.uColorsUsed == 0)
		{
			switch (bmh_bitmap_header.u2BitsPerPixel)
			{
				case 1:
					bmh_bitmap_header.uColorsUsed = 2;
					bmh_bitmap_header.uColorsReqd = 2;
					break;
				case 4:
					bmh_bitmap_header.uColorsUsed = 16;
					bmh_bitmap_header.uColorsReqd = 16;
					break;
				case 8:
					bmh_bitmap_header.uColorsUsed = 256;
					bmh_bitmap_header.uColorsReqd = 256;
					break;
			}
		};

		// This is a paletted bitmap.
		SPaletteEntry*	pbple_palette = (SPaletteEntry *) (u1_buffer + sizeof(SFileHeader) + sizeof(SBitmapHeader));
		SColor24		ac24_palette[256];

		// Convert the Windows BMP palette entries into the internal format.
		for (uint u_i = 0; u_i < bmh_bitmap_header.uColorsUsed; u_i++)
		{
			ac24_palette[u_i].u1Red   = pbple_palette[u_i].u1Red;
			ac24_palette[u_i].u1Green = pbple_palette[u_i].u1Green;
			ac24_palette[u_i].u1Blue  = pbple_palette[u_i].u1Blue;
		}

		// Give the palette to the bitmap image.  Were we successful?
		if (bmi_bitmap_image.uSetPaletteEntries(0, bmh_bitmap_header.uColorsUsed, &ac24_palette[0]) != bmh_bitmap_header.uColorsUsed)
		{
			// No!  Return an error.
			return false;
		}
		
		// Determine how large the read chunks need to be.
		const int i_scanline_offset = (int) (bmh_bitmap_header.lWidth+3) >> 2;
		int* pi_buffer = (int *) (u1_buffer + fh_file_header.uBitmapOffset);
		
		// Loop through the scanlines and load them into the image data.
		for (uint u_i = 0; u_i < (uint) bmh_bitmap_header.lHeight; u_i++)
		{
			// Attempt to write the data into the bitmap image buffer.  Were we successful?
			if (bmi_bitmap_image.uSetPalettedPixels(0, u_i, bmh_bitmap_header.lWidth, (uint8 *) pi_buffer) != (uint) bmh_bitmap_header.lWidth)
			{
				// No!  Return an error;
				return false;
			}

			// Adavance the pointer to the next scanline.
			pi_buffer += i_scanline_offset;
		}
	}
	else
	{
		// Determine how large the read chunks need to be.
		const int i_scanline_offset = (int) ((bmh_bitmap_header.lWidth * sizeof(SPixelEntry))+3) >> 2;
		int* pi_buffer = (int *) (u1_buffer + fh_file_header.uBitmapOffset);
		
		// Loop through the scanlines and load them into the image data.
		for (uint u_i = 0; u_i < (uint) bmh_bitmap_header.lHeight; u_i++)
		{
			// Attempt to write the data into the bitmap image buffer.  Were we successful?
			if (bmi_bitmap_image.uSetTruePixels(0, u_i, bmh_bitmap_header.lWidth, (SColor24 *) pi_buffer) != (uint) bmh_bitmap_header.lWidth)
			{
				// No!  Return an error;
				return false;
			}

			// Adavance the pointer to the next scanline.
			pi_buffer += i_scanline_offset;
		}
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CBmpBitmap::bWrite(CBitmapImage& bmi_bitmap_image)
{
	// Get a copy of the bitmap info record.
	CBitmapInfo bi_bitmap_info;
	bi_bitmap_info = bmi_bitmap_image.biBitmapInfo();

	// Check the dimensions of the bitmap for correct ranges.
	if (!bi_bitmap_info.bIsValidDefinition())
	{
		// No!  Return an error.
		return false;
	}

	// Start out with the bitmap width in pixels.
	const uint u_bitmap_depth    = bi_bitmap_info.uDepth();
	const uint u_width           = bi_bitmap_info.uWidth();
	const uint u_height          = bi_bitmap_info.uHeight();
	const uint u_palette_entries = bi_bitmap_info.uPaletteEntries();
	const uint u_palette_size	 = u_palette_entries * sizeof(SPaletteEntry);

	uint       u_scanline_pad;
	uint       u_image_size;

	// Determine the correction factor based upon the bitmap depth.
	switch(u_bitmap_depth)
	{
	case 1:
	case 4:
		// These are not handled for now, so store 2 or 16 color bitmaps as 256 color bitmaps.
	case 8:
		u_scanline_pad = ((u_width+3) & 0xFFFFFFFC) - u_width;

		// Determine the size of the image.
		u_image_size = u_height * (u_width + u_scanline_pad);

		break;
	case 24:
		// Calculate the padding factor required to 32 bit align the row.
		u_scanline_pad = (((u_width * sizeof(SColor24))+3) & 0xFFFFFFFC) - (u_width * sizeof(SColor24));

		// Determine the size of the image.
		u_image_size = u_height * ((u_width * sizeof(SColor24)) + u_scanline_pad);

		break;
	}

	// Construct the DIB file image.
	SFileHeader fh_file_header;

	fh_file_header.u2FileType    = WINDOWS_BMP_MAGIC_NUMBER;
	fh_file_header.uFileSize     = sizeof(SFileHeader) + sizeof(SBitmapHeader) + u_palette_size + u_image_size;
	fh_file_header.uReserved1    = 0;
	fh_file_header.uReserved2    = 0;
	fh_file_header.uBitmapOffset = sizeof(SFileHeader) + sizeof(SBitmapHeader) + u_palette_size;

	// Construct the DIB bitmap header.
	SBitmapHeader bmh_bitmap_header;


	bmh_bitmap_header.uHeaderSize	 = sizeof(SBitmapHeader);
	bmh_bitmap_header.lWidth		 = u_width;
	bmh_bitmap_header.lHeight		 = u_height;
	bmh_bitmap_header.u2Planes		 = 1;
	bmh_bitmap_header.u2BitsPerPixel = u_bitmap_depth;
	bmh_bitmap_header.uCompression	 = 0;
	bmh_bitmap_header.uBitmapSize	 = 0;
	bmh_bitmap_header.uHorizRes		 = 0xAED;
	bmh_bitmap_header.uVerticalRes	 = 0xAED;
	bmh_bitmap_header.uColorsUsed	 = u_palette_entries;
	bmh_bitmap_header.uColorsReqd	 = u_palette_entries;

	//
	// We now have the headers constructed so we can write the bitmap to the file.
	//
	CSmartBuffer sb_buffer;
	
	// Construct a buffer to buildWrite out the header.
	TBufferHandle bh_handle = sb_buffer.bhCreate(fh_file_header.uFileSize);

	// Were we able to allocate a smart buffer?
	if (bh_handle == 0)
	{
		// No!  Return an error.
		return false;
	}

	// Write out the file header.
	if (sb_buffer.iWrite(bh_handle, &fh_file_header, sizeof(SFileHeader)) != sizeof(SFileHeader))
	{
		// No!  Return an error.
		return false;
	}

	// Write out the bitmap header.  Were we successful? 
	if (sb_buffer.iWrite(bh_handle, &bmh_bitmap_header, sizeof(SBitmapHeader)) != sizeof(SBitmapHeader))
	{
		// No!  Return an error.
		return false;
	}

	// If this is a bitmap with a palette, write out the palette entries.
	if (u_palette_entries != 0)
	{
		// Get the palette in a local buffer.
		SColor24 c24_palette[256];
		
		// Are we able to read the palette?
		if (bmi_bitmap_image.uGetPaletteEntries(0, u_palette_entries, &c24_palette[0]) != u_palette_entries)
		{
			// No!  Return an error.
			return false;
		}

		// Convert each of the palette entries from a SColor24 value to a BMP palette entry.
		SPaletteEntry bple_palette[256];
		for (uint u_i = 0; u_i < u_palette_entries; u_i++)
		{
			// Copy the values from one representation to another.
			bple_palette[u_i].u1Red    = c24_palette[u_i].u1Red;
			bple_palette[u_i].u1Green  = c24_palette[u_i].u1Green;
			bple_palette[u_i].u1Blue   = c24_palette[u_i].u1Blue;
			bple_palette[u_i].u1Reserved = 0;
		}

		// Attempt to write the palette out to the file.  Were we successful?
		if (sb_buffer.iWrite(bh_handle, &bple_palette[0], u_palette_size) != (int) u_palette_size)
		{
			// No!  Return an error.
			return false;
		}
	}

	//
	// Write out the actual image data.
	//
	int i_zero = 0;

	// Is this a paletted or unpaletted bitmap?
	switch (u_bitmap_depth)
	{
		case 1:
		case 4:
		case 8:
			// For each row in the bitmap, write out the row and pad it to the smart buffer.			
			uint u_i;
			for (u_i = 0; u_i < u_height; u_i++)
			{
				// Read the index pixels directly into the smart buffer.  Were all the pixels read?
				uint8 u1_scanline[MAX_BITMAP_WIDTH];

				// Were we able to read the indexed pixels?
				if (bmi_bitmap_image.uGetPalettedPixels(0, u_i, u_width, &u1_scanline[0]) != u_width)
				{
					// No!  Return an error.
					return false;
				}

				// Write them to the bitmap file.  Were we successful?
				if (sb_buffer.iWrite(bh_handle, &u1_scanline[0], u_width) != (int) u_width)
				{
					// No!  Return an error.
				}

				// If the row is not properly aligned to a 32 bit boundry, adjust it.
				if (sb_buffer.iWrite(bh_handle, &i_zero, u_scanline_pad) != (int) u_scanline_pad)
				{
					// No!  Return an error.
					return false;
				}
			}

			break;
		case 24:
			// For each row in the bitmap, write out the row and pad it to the smart buffer.
			for (u_i = 0; u_i < u_height; u_i++)
			{
				SColor24 ac24_scanline[MAX_BITMAP_WIDTH];

				// Read the index pixels directly into the smart buffer.  Were all the pixels read?
				if (bmi_bitmap_image.uGetTruePixels(0, u_i, u_width, &ac24_scanline[0]) != u_width)
				{
					// No!  Return an error.
					return false;
				}

				// Write them to the bitmap file.  Were we successful?
				if (sb_buffer.iWrite(bh_handle, &ac24_scanline[0], u_width * sizeof(SPixelEntry)) != (int) (u_width * sizeof(SPixelEntry)))
				{
					// No!  Return an error.
					return false;
				}


				// If the row is not properly aligned to a 32 bit boundry, adjust it.
				if (sb_buffer.iWrite(bh_handle, &i_zero, u_scanline_pad) != (int) u_scanline_pad)
				{
					// No!  Return an error.
					return false;
				}
			}

			break;
	}

	// Seek to the beginning of the smart buffer so it can be written to disk.
	if (sb_buffer.iSeek(bh_handle, 0) != 0)
	{
		// We cannot seek to the beginning of the smart buffer so return an error.
		return false;
	}

	// Get the address of the start of the buffer.
	void* buffer = sb_buffer.Address(bh_handle);

	// Were we successful?
	if (buffer == 0)
	{
		// No!  Return and error.
		return false;
	}

	int i_count = sb_buffer.iCount(bh_handle);

	// Is this buffer the correct size?

	// Attempt to open the output file.
	std::ofstream ofile(bi_bitmap_info.strName(), std::ios::binary | std::ios::trunc);

	// Were we able to open the file?
	if (!ofile)
	{
		// No!  Return an error.
		return false;
	}

	// Write the buffer contents to a file.
	ofile.write((const char *) buffer, i_count);

	// Close the file.
	ofile.close();

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
bool CGrfBitmap::bDetectBitmap(const char* str_bitmap_name)
{
	// Return an error for now.
	return false;
}


//**********************************************************************************************
//
bool CGrfBitmap::bRead(const char* str_bitmap_name, CBitmapImage& bmi_bitmap_image)
{
	// Return an error for now.
	return false;
}


//**********************************************************************************************
//
bool CGrfBitmap::bWrite(CBitmapImage& bmi_bitmap_image)
{
	// Return an error for now.
	return false;
}