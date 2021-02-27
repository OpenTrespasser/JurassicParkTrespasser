#pragma once

/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CClipPolygon
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/ClipMesh.hpp                                            $
 * 
 * 1     97/07/23 18:57 Speter
 * Special-purpose polygon for clipping mesh polygons.
 * 
 **********************************************************************************************/

#include "Mesh.hpp"

// Special outcode for this class; indicates a polygon is to the side of another polygon's edges.
#define esfSIDE (esfOUTSIDE << 1)

//**********************************************************************************************
//
class CClipPolygon
//
// Polygon structure for clipping mesh polygons.  Can be constructed from a mesh polygon.
//
// Prefix: clpg
//
//**************************************
{
protected:
	CPlane	plPlane;					// The plane of the polygon.
	CAArray<CPlane>	paplEdges;			// Planes for the polygon edges.

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Initialise with an array of points; the edge planes are calculated automatically.
	CClipPolygon
	(
		const CMesh::SPolygon& mp			// Mesh polygon to clip from.
	);

	//******************************************************************************************
	//
	ESideOf esfIntersects
	(
		CMesh::SPolygon& mp,				// The mesh polygon to intersect.
		CMesh::CHeap& mh,					// Where to allocate new (temporary) data.
		ESideOf* pesf_edges					// Return intersection with polygon edges.
	) const;
	//
	// Returns:
	//		The relationship of mp to this polygon's plane.
	//
	//**********************************

	//******************************************************************************************
	//
	ESideOf esfClipPolygon
	(
		CMesh::SPolygon& mp,				// The mesh polygon to clip.
		CMesh::CHeap& mh,					// Where to allocate new data.
		ESideOf esf_sides					// esfINSIDE to clip to inside of this polygon;
											// esfOUTSIDE to clip to outside;
											// both to split polygon, creating outside one on heap.
	) const;
	//
	// Clips mp against this polygon, if they intersect.
	//
	// Returns:
	//		The relationship of mp to this polygon's plane (before clipping).
	//
	//**********************************
};
