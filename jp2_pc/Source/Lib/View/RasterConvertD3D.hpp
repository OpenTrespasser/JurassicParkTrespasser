/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Functions for converting rasters from memory format to Direct3D texture formats.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/View/RasterConvertD3D.hpp                                         $
 * 
 * 3     9/27/98 10:02p Pkeet
 * Added a global function to reset conversion tables.
 * 
 * 2     8/01/98 4:44p Pkeet
 * Added a background colour for transparent textures.
 * 
 * 1     7/23/98 6:18p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_RASTERCONVERTD3D_HPP
#define HEADER_LIB_VIEW_RASTERCONVERTD3D_HPP


//
// Forward declarations.
//
class CRaster;
class CRasterD3D;


//
// Required includes.
//
#include "Lib/View/Colour.hpp"


//
// Function prototypes.
//

//**********************************************************************************************
//
void ConvertRaster
(
	CRasterD3D*    pras_d3d,		// Direct3D texture to write to.
	rptr<CRaster>  pras_mem,		// Memory raster to get texture from.
	CColour        clr = CColour()	// Average colour of raster, used for transparent/textures.
);
//
// Copies and converts pixels from the memory raster to the D3D texture raster.
//
//**************************************
 
//**********************************************************************************************
//
void ResetD3DConversions
(
);
//
// Resets lookup tables used for converting regular rasters to Direct3D textures.
//
//**************************************


#endif // HEADER_LIB_VIEW_RASTERCONVERTD3D_HPP

