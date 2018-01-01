/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Helper functions and classes for the pipeline.
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/PipeLineHelp.hpp                                         $
 * 
 * 10    97/07/07 14:19 Speter
 * Removed bvboxEnclosing().
 * 
 * 9     97/07/07 14:17 Speter
 * Fixed include file order.
 * 
 * 8     4/30/97 9:19p Rwyatt
 * Moved esfView member variable from CPartition and put it into the list that the partition
 * functions return. These functions used to return a list of CPartition* now they return a list
 * of structures that contain a CPartition* and esfSideOf.
 * 
 * 7     97/04/05 2:48p Pkeet
 * Changed interface to use the pipeline heap.
 * 
 * 6     97-04-04 12:38 Speter
 * A big change: now pipeline uses CRenderPolygon rather than SRenderTriangle (currently always
 * as a triangle).  Changed associated variable names and comments.
 * 
 * 5     97/04/03 7:52p Pkeet
 * Added the bounding box enclosing function.
 * 
 * 4     97/04/02 12:14p Pkeet
 * Made 'SetCompareValues' internal to the pipeline help module.
 * 
 * 3     97/04/01 6:36p Pkeet
 * Made inline functions out-of-line.
 * 
 * 2     97/04/01 4:35p Pkeet
 * Improved 'MakePointerList' slightly.
 * 
 * 1     97/04/01 4:18p Pkeet
 * Change ScreenRender interface to use an array of pointers to render triangles instead of an
 * array of render triangles.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PIPELINEHELP_HPP
#define HEADER_LIB_RENDERER_PIPELINEHELP_HPP

#include "ScreenRender.hpp"
#include "PipeLineHeap.hpp"
#include "GeomTypes.hpp"
#include "Lib/GeomDBase/Partition.hpp"

//
// Global functions.
//

//**********************************************************************************************
//
void MakePointerList
(
	CPArray<CRenderPolygon> parpoly,	// Pointer to an array of render triangles.
	CPipelineHeap&          rplh		// Pointer to an array of pointers to render triangles.
);
//
// Fills the array of pointers.
//
//**************************************

//**********************************************************************************************
//
void MakeSortedPointerList
(
	CPArray<CRenderPolygon> parpoly,		// Pointer to an array of render triangles.
	CPipelineHeap&          rplh,			// Pointer to an array of pointers to render triangles.
	bool                    bFrontToBack	// If 'true' sort triangle pointers into a front-
											// to-back order, otherwise sort into a back-to-
											// front order.
);
//
// Fills the array of pointers.
//
//**************************************

#endif // HEADER_LIB_RENDERER_PIPELINEHELP_HPP

