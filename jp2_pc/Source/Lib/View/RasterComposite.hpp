/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Functions for using composite rasters, e.g., rasters with interlaced palette index values
 * and lighting values.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/RasterComposite.hpp                                          $
 * 
 * 3     7/09/98 7:58p Mlange
 * Apply clut function now has optional sub-rect parameter.
 * 
 * 2     97/11/03 5:51p Pkeet
 * Added in a mask for the apply clut functions.
 * 
 * 1     97/08/12 1:37p Pkeet
 * Initial (untested) implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_RASTERCOMPOSITE
#define HEADER_LIB_VIEW_RASTERCOMPOSITE


//
// Function prototypes.
//

//**********************************************************************************************
//
void ApplyClut
(
	rptr<CRaster> pras,		// Interlaced raster.
	const CClut&  clut,		// Clut to use for conversion.
	int           i_fog,	// Constant fog value to apply.
	uint16        u2_mask = 0xFFFF,	// Bit mask for alpha.
	const SRect* prect = 0	// Optional sub-rect within raster.
);
//
// Applies the given clut to the raster. Function assumes the raster is in an interlaced format
// based on the clut format, with the index value as the bottom byte.
//
//**********************************


#endif // HEADER_LIB_VIEW_RASTERCOMPOSITE
