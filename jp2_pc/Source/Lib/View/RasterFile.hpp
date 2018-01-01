/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Functions for saving and loading rasters.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/RasterFile.hpp                                               $
 * 
 * 5     6/23/98 11:27a Shernd
 * Changed SetMipDirectory to return a true/false depending if the directory was successfully
 * created during the set operation
 * 
 * 4     98.06.19 8:31p Mmouni
 * Changed SetMipDirectory().
 * 
 * 3     1/29/98 7:51p Rwyatt
 * Mip maps no longer scan the raster data to generate a filename.
 * 
 * 2     97/08/13 12:29p Pkeet
 * Added the 'SetMipDirectory' and 'DeleteMipDirectoryString' functions. Removed the
 * 'MakeMipDirectory' function.
 * 
 * 1     97/08/08 4:33p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_RASTERFILE_HPP
#define HEADER_LIB_VIEW_RASTERFILE_HPP


//
// Forward declarations.
//
class CRaster;


//
// Global function prototypes.
//

//*********************************************************************************************
//
uint64 u8GenerateHashValue
(
	rptr<CRaster> pras	// Raster to generate a hash value from.
);
//
// Returns a seven byte hash value generated from the surface of the raster. The additional
// (lowest) byte is left empty for the addition of extra data.
//
//**************************************

//*********************************************************************************************
//
bool bLoadRasterMip
(
	const char*		str_filename,
	rptr<CRaster>	pras		// Raster to load file to.
);
//
// Function generates a filename based on the hash value and the raster, then loads that
// file from the bindata\mip directory into the raster's surface if it can.
//
// Returns 'true' if the raster was successfully loaded.
//
//**************************************

//*********************************************************************************************
//
bool bSaveRasterMip
(
	const char*		str_filename,
	rptr<CRaster> pras		// Raster to save to file.
);
//
// Function generates a filename based on the hash value and the raster, then saves that
// file from the bindata\mip directory from the raster's surface if it can.
//
// Returns 'true' if the raster was successfully saved.
//
//**************************************

//*********************************************************************************************
//
bool SetMipDirectory
(
	const char* str_filename	// Groff file name (with extenstion stripped off).
);
//
// Sets the mipmap directory.
//
// Returns 'true' if the directory was created
//
//**************************************

//*********************************************************************************************
//
void DeleteMipDirectoryString
(
);
//
// Deletes the string associated with the mipmap directory.
//
//**************************************


#endif // HEADER_LIB_VIEW_RASTERFILE_HPP
