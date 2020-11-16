/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of RasterFile.hpp.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/RasterFile.cpp                                               $
 * 
 * 8     98.06.25 1:46p Mmouni
 * Fixed problem with mip-map directory not being created.
 * 
 * 7     6/23/98 11:27a Shernd
 * Changed SetMipDirectory to return a true/false depending if the directory was successfully
 * created during the set operation
 * 
 * 6     98.06.19 8:31p Mmouni
 * Changed SetMipDirectory().
 * 
 * 5     1/29/98 7:51p Rwyatt
 * Mip maps no longer scan the raster data to generate a filename.
 * 
 * 4     97/09/11 15:30 Speter
 * Now create map directory as well, if not there.
 * 
 * 3     9/02/97 7:52p Agrant
 * Moved mipmaps to a "Mip" subdirectory.
 * 
 * 2     97/08/13 12:29p Pkeet
 * Added the 'SetMipDirectory' and 'DeleteMipDirectoryString' functions. Removed the
 * 'MakeMipDirectory' function.
 * 
 * 1     97/08/08 4:33p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <direct.h>
#include "common.hpp"
#include "Raster.hpp"
#include "Lib/Sys/FileEx.hpp"
#include "RasterFile.hpp"


//
// Constants.
//
char* strMipFileCache = 0;

//
// Function implementations.
//

/*//*********************************************************************************************
uint64 u8GenerateHashValue(rptr<CRaster> pras)
{
	// Initialize the hash value.
	uint64 u8_hash = 0;
	int    i_count = 8;	// Rolling hash shift counter.

	// Get a pointer to the surface.
	uint8* pu1_surface = (uint8*)pras->pSurface;

	// Calculate the number of bytes in a line.
	int i_line_bytes = pras->iLinePixels * (pras->iPixelBits >> 3);

	for (int i_line = 0; i_line < pras->iHeight; ++i_line)
	{
		// Get the start address of the line.
		uint8* pu1_line = pu1_surface + i_line * i_line_bytes;

		// Scan through the horizontal row of pixels.
		for (int i_pixel = 0; i_pixel < pras->iWidth; ++i_pixel)
		{
			// Generate a value to xor into the hash value.
			uint64 u8 = uint64(pu1_line[i_pixel]);
			u8 <<= i_count;

			// Xor the new value into the hash value.
			u8_hash ^= u8;

			// Increment the hash counter.
			++i_count;
			if (i_count > 55)
				i_count = 8;
		}
	}

	// Return the generated hash value.
	return u8_hash;
}*/

//*********************************************************************************************
bool bLoadRasterMip(const char* str_filename, rptr<CRaster> pras)
{
	Assert(pras);

	int  i_size = pras->iHeight * pras->iLinePixels * (pras->iPixelBits >> 3);	

	// Set the file paths to the local BinData directory.
	CPushDir pshd(strMipFileCache);

	// Save cache file.
	if (bFileExists(str_filename))
	{
		ReadImage(str_filename, pras->pSurface, i_size);
		return true;
	}
	return false;
}

//*********************************************************************************************
bool bSaveRasterMip(const char* str_filename, rptr<CRaster> pras)
{
	Assert(pras);

	int  i_size = pras->iHeight * pras->iLinePixels * (pras->iPixelBits >> 3);	

	// Set the file paths to the local BinData directory.
	CPushDir pshd(strMipFileCache);

	// Save cache file.
	SaveImage(str_filename, pras->pSurface, i_size);
	return true;
}

/*//*********************************************************************************************
void CreateFileName(char* str_filename, uint64 u8_hash, int32 i4_width)
{
	Assert(str_filename);
	Assert(u8_hash);
	Assert(i4_width);

	// Incorporate the raster's width into the hash value.
	u8_hash ^= i4_width;

	// Separate the top and bottom parts of the hash value.
	uint32 u4_top    = u8_hash >> 32;
	uint32 u4_bottom = u8_hash & 0xFFFFFFFF;

	// Generate the string.
	sprintf(str_filename, "Mip/%x%x.mip", u4_top, u4_bottom);
}*/

//*********************************************************************************************
void DeleteMipDirectoryString()
{
	delete[] strMipFileCache;
	strMipFileCache = 0;
}

//*********************************************************************************************
bool SetMipDirectory(const char* str_filename)
{
	// Get rid of the old mip directory string.
	DeleteMipDirectoryString();

	// Allocate temporary buffer for map directory path.
	char *str_temp = (char *)_alloca(strlen(str_filename) + 32);

	// Append the file name.
	strcpy(str_temp, "Map\\");			// 4
	strcat(str_temp, str_filename);
	strcat(str_temp, "\\");				// 1

	// Allocate buffer for map directory path.
	strMipFileCache = new char[strlen(str_temp) + 1];
	strcpy(strMipFileCache, str_temp);

	// Create the map (should already be there), and the MIP sub-directory.
	_mkdir(str_temp);

	// Add on the mip sub-directory and attempt to make it.
	strcat(str_temp, "Mip");
	if (_mkdir(str_temp) == -1)
    {
        return false;
    }

    return true;
}
