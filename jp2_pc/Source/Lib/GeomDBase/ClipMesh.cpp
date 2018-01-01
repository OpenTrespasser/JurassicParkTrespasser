/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of ClipMesh.hpp.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/ClipMesh.cpp                                            $
 * 
 * 2     8/25/98 4:41p Rwyatt
 * Reset heap
 * 
 * 1     97/07/23 18:57 Speter
 * Special-purpose polygon for clipping mesh polygons.
 * 
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "ClipMesh.hpp"
#include "Lib/Renderer/Clip.hpp"

//**********************************************************************************************
//
// CClipPolygon implementation.
//

	//******************************************************************************************
	CClipPolygon::CClipPolygon(const CMesh::SPolygon& mp)
		: plPlane(mp.plPlane)
	{
		//
		// Construct the planes for each edge.
		//

		paplEdges = CPArray<CPlane>(mp.papmvVertices.uLen);

		for (int i = 0; i < paplEdges.uLen; i++)
		{
			int i_2 = i == paplEdges.uLen-1 ? 0 : i+1;

			// For edge i..i_2, construct a plane whose normal is perpendicular to this edge,
			// and facing outward the polygon.  This is the cross-product of the edge and the
			// polygon plane normal.

			paplEdges[i] = CPlane(mp.v3Point(i), mp.v3Point(i_2), mp.v3Point(i) + plPlane.d3Normal);
		}
	}

	//******************************************************************************************
	ESideOf CClipPolygon::esfIntersects(CMesh::SPolygon& mp, CMesh::CHeap& mh, ESideOf* pesf_edges) const
	{
		// Remember, and restore the heap while doing this.
		uint u_pointer_start = mh.mapmvVertices.uLen;
		uint u_vertex_start = mh.mamvVertices.uLen;
		uint u_point_start = mh.mav3Points.uLen;

		// Make a copy of mp, and clip it against this polygon's edges.
		CMesh::SPolygon mp_copy = mp;

		*pesf_edges = 0;

		forall_const (paplEdges, CPArray<CPlane>, it_pl)
		{
			ESideOf esf_plane = CClipPlaneGeneral(*it_pl).esfClipPolygon(mp_copy, mh, esfINSIDE);
			if (!(esf_plane & esfINSIDE))
			{
				// It's outside or on the plane.
				*pesf_edges = esf_plane;
				break;
			}
			*pesf_edges |= esf_plane;
		}

		ESideOf esf;

		if (*pesf_edges & esfINSIDE)
		{
			// Return the intersection of the polygon with this polygon's plane.
			esf = 0;
			for (int i = 0; i < mp_copy.iNumVertices(); i++)
				esf |= plPlane.esfSideOf(mp_copy.v3Point(i));
		}
		else
			esf = esfSIDE;

		// Restore the heap to its previous values.
		mh.mapmvVertices.Reset(u_pointer_start,u_pointer_start);
		mh.mamvVertices.Reset(u_vertex_start,u_vertex_start);
		mh.mav3Points.Reset(u_point_start,u_point_start);

		return esf;
	}

	//******************************************************************************************
	ESideOf CClipPolygon::esfClipPolygon(CMesh::SPolygon& mp, CMesh::CHeap& mh, ESideOf esf_sides) const
	{
		ESideOf esf_edges;
		ESideOf esf = esfIntersects(mp, mh, &esf_edges);
		if (esf == esfINTERSECT)
			// The polygons intersect, so split mp.
			return CClipPlaneGeneral(plPlane).esfClipPolygon(mp, mh, esf_sides);
		else
			return esf;
	}
