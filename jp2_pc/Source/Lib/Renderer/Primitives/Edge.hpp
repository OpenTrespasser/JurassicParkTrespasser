/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The CEdge class describes generic behaviour for triangle edges as used for rasterizing.
 *		This class is designed to work with any scan line type.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *		The base edge of the triangle is defined as the edge which is used to find the starting
 *		point of the scanline for each scanline.
 *
 *		Access to data associated with the line type (e.g. CLineFlatZ) is performed through a
 *		pointer so that classes with different structure sizes can be cast to the parent class
 *		without generating an error.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/Edge.hpp                                      $
 * 
 * 16    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 15    97.10.15 7:40p Mmouni
 * Remove unnecessary max clamp of delta y.
 * 
 * 14    97/07/07 14:02 Speter
 * Renamed iY to iYScr, as it's now a union with v3Cam, and set in InitializePolygonData.
 * 
 * 13    97/06/17 11:39a Pkeet
 * Removed the default constructor.
 * 
 * 12    4/30/97 2:10p Cwalla
 * Inlined SetMinMax and improved code.
 * 
 * 11    97-03-31 22:20 Speter
 * Removed LINE pointer members from CEdge<>, now just use actuals.
 * 
 * 10    97/03/25 2:31p Pkeet
 * Fix for undefined subpixel y position bug.
 * 
 * 9     10/09/96 7:39p Pkeet
 * Removed the 'InitializeAsBaseEdge' member function. Removed the 'protected' line description
 * member variables.
 * 
 * 8     9/26/96 1:45p Pkeet
 * Always calculate fInvDY in InitializeEdge.
 * 
 * 7     96/07/18 17:19 Speter
 * Changes affecting zillions of files:
 * Changed SRenderCoord to SRenderVertex.
 * Added SRenderTriangle type, changed drawing primitives to accept it.
 * 
 * 6     7/17/96 12:06p Pkeet
 * Moved the drawing primitives up by 1 so that they clip correctly.
 * 
 * 5     7/08/96 3:29p Pkeet
 * Added additional parameter to 'InitializeAsBase' function.
 * 
 * 4     6/25/96 2:32p Pkeet
 * Added 'plineStart' and 'plineIncrement.'
 * 
 * 3     6/13/96 3:09p Pkeet
 * Pass *this to line types.
 * 
 * 2     6/12/96 7:23p Pkeet
 * Added notes. Added asserts. Removed iY + 1 references. Renamed 'fInverseDY' to 'fInvDY.'
 * Renamed 'fSubpixelY' to 'fStartYDiff.'
 * 
 * 1     6/06/96 3:44p Pkeet
 * Initial implementation. Based on the CEdge class in the deleted file 'PolygonEdge.hpp.'
 * 
 **********************************************************************************************/

#ifndef HEADER_VIEW_RENDER_PRIMITIVES_EDGE_HPP
#define HEADER_VIEW_RENDER_PRIMITIVES_EDGE_HPP


//*********************************************************************************************
//
template<class LINE> class CEdge
//
// Describes a rasterizing edge for a triangle. Performs setups and increments for left and
// right edges.
//
// Prefix: edge
//
// Notes:
//		This object contains data about the vertical extent of the edge, scanline starting
//		data, increment data and the slope of the edge's line on the screen. Subpixel positions
//		will be calculated for the start of each edge.
//
//**************************************
{

public:
	LINE          lineStart;		// Scanline starting data.
	LINE          lineIncrement;	// Scanline increment data.
	float         fInvDY;			// Inverse of the vertical distance of the edge.
	float         fStartYDiff;		// Distance from the start y position to the first
									// horizontal line.
	SRenderVertex* prvFrom;			// Pointer to the start render coordinate structure.
	SRenderVertex* prvTo;			// Pointer to the last render coordinate structure.

	// CEdge template access types.
	typedef LINE  TScanline;		// Scanline type.

public:

	// Force default constructor inline so it is no called.
	forceinline CEdge()
	{
	}

	//*****************************************************************************************
	//
	// CEdge member functions.
	//

	//*****************************************************************************************
	//
	forceinline void InitializeEdge
	(
		SRenderVertex* prv_from,	// Starting coordinate of edge.
		SRenderVertex* prv_to		// End coordinate of edge.
	)
	//
	// Prepares edge for general use. Base edges will require further setup by calling
	// 'InitializeAsBaseEdge.' This function contains common setup behaviour for both left
	// and right edges.
	//
	// This member function will calculate the X slope of the line with respect to the
	// Y axis of the screen, and will calculate the subpixel distance between the Y starting
	// coordinate and the first rasterized horizontal scanline.
	//
	//**************************************
	{
		Assert(prv_from);
		Assert(prv_to);

		// Store pointers.
		prvFrom = prv_from;
		prvTo   = prv_to;

		float f_dy = prv_to->v3Screen.tY - prv_from->v3Screen.tY;

		// SetMinMax(f_dy, 0.01f, 1024.0f);
		if (f_dy < 0.01f)
			fInvDY = (1.0f / 0.01f);
		else
		 	fInvDY = 1.0f / f_dy;

		//
		// Get the subpixel position in Y.
		//
		fStartYDiff = (float)(prv_from->iYScr + 1) - prv_from->v3Screen.tY;

		//
		// Don't bother initializing the edge if the edge is not rendered; ie., it does not
		// cross a horizontal scanline.
		//
		if (prv_from->iYScr == prv_to->iYScr)
			return;

		//
		// Calculate the X increment for each scanline, and move the starting X coordinate
		// to where the edge intersects the first scanline.
		//
		lineStart.Initialize(this);
	}

	//*****************************************************************************************
	//
	forceinline void operator ++()
	//
	// Increment the edge data for each scanline along the edge.
	//
	//**************************************
	{
		lineStart += lineIncrement;
	}

};


#endif
