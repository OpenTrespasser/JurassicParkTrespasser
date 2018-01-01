/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Functions to test for 2D screen containment and intersection.
 *
 * Bugs:
 *
 * To do:
 *		Utilize "side-of" definitions from GeomDefs when it comes into being.
 *		Integrate functions here with the CLineSegment class wherever appropriate.
 *		Add a function to determine if a polygon is wholly contained by another polygon.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Line.hpp                                                 $
 * 
 * 4     98.02.05 4:49p Mmouni
 * Added bFastIntersect2D().
 * 
 * 3     97/04/28 5:51p Pkeet
 * Removed 'iSIDE' macro definitions.
 * 
 * 2     97/04/23 10:28a Pkeet
 * Added the 'fAreaIntersection' function.
 * 
 * 1     97/04/13 1:54p Pkeet
 * Initial implementation. Code moved here from the depth sort module.
 * 
 **********************************************************************************************/

#ifndef LIB_RENDERER_LINE_HPP
#define LIB_RENDERER_LINE_HPP


//
// Necessary includes.
//
#include "ScreenRender.hpp"


//
// Global function prototypes.
//

//**********************************************************************************************
//
bool bIntersect2D
(
	const CRenderPolygon* prpoly_a,	// Pointer to the first render polygon.
	const CRenderPolygon* prpoly_b	// Pointer to the second render polygon.
);
//
// Returns 'true' if the 2D projections of the polygon intersect in screen space.
//
//**********************************

//**********************************************************************************************
//
float fAreaIntersection
(
	const CRenderPolygon* prpoly_a,	// Pointer to the first render polygon.
	const CRenderPolygon* prpoly_b	// Pointer to the second render polygon.
);
//
// Returns the area of intersection between the 2D projections of the polygons in screen space.
//
//**********************************

//**********************************************************************************************
//
bool bFastIntersect2D
(
	const CRenderPolygon* prpoly_p,		// Pointer to the first render polygon.
	const CRenderPolygon* prpoly_q		// Pointer to the second render polygon.
);
//
// Returns 'true' if the 2D projections of the polygon intersect in screen space.
//
// Notes:	The polygons must be in the standard winding order.
//			Polygons with more than 32 vertices may not work correctly.
//
//**********************************

#endif // LIB_RENDERER_LINE_HPP
