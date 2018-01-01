/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ScreenRenderAuxD3DBatch.hpp                              $
 * 
 * 4     8/04/98 6:14p Pkeet
 * Added batch rasterization for caches and for ordinary polygons.
 * 
 * 3     8/03/98 4:25p Pkeet
 * Added the 'RemovePrerasterized' function.
 * 
 * 2     8/02/98 4:50p Pkeet
 * Changed interface to take a list of polygons instead of polygon pointers.
 * 
 * 1     8/02/98 3:11p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef LIB_RENDERER_SCREENRENDERAUXD3DBATCH_HPP
#define LIB_RENDERER_SCREENRENDERAUXD3DBATCH_HPP


//
// Includes.
//
#include "Lib/W95/WinInclude.hpp"
#include "Lib/Renderer/ScreenRender.hpp"


//
// Function prototypes.
//

//******************************************************************************************
//
void RasterizeBatch
(
	CPArray<CRenderPolygon>& parpoly	// Polygons to rasterize.
);
//
// Rasterizes a batch of polygons using Direct3D.
//
//**************************************

//******************************************************************************************
//
void RasterizeCacheBatch
(
	CPArray<CRenderPolygon>& parpoly	// Polygons to rasterize.
);
//
// Rasterizes a batch of terrain polygons using Direct3D.
//
//**************************************

//******************************************************************************************
//
void RasterizeTerrainBatch
(
	CPArray<CRenderPolygon>& parpoly	// Polygons to rasterize.
);
//
// Rasterizes a batch of terrain polygons using Direct3D.
//
//**************************************

//******************************************************************************************
//
void RemovePrerasterized
(
	CPArray<CRenderPolygon>& parpoly	// Polygons to rasterize.
);
//
// Sets the 'bAccept' flag for prerendered polygons that do not overlap software polygons
// to 'false.'
//
//**************************************


#endif // LIB_RENDERER_SCREENRENDERAUXD3DBATCH_HPP