/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of Clip.hpp
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Clip.cpp                                                 $
 * 
 * 16    98/09/16 12:15 Speter
 * Much more efficient use of vertex pointer heap.
 * 
 * 15    8/27/98 1:49p Asouth
 * loop variables fixed
 * 
 * 14    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 13    2/03/98 6:35p Mlange
 * Moved assertion failure avoidance to CRenderPolygon::InitFast().
 * 
 * 12    1/29/98 3:39p Mlange
 * Fixed assertion failure.
 * 
 * 11    1/07/98 4:05p Pkeet
 * Added the clip region function and globals.
 * 
 * 10    12/23/97 1:56p Pkeet
 * Added a parameter to use when splitting polygons to move the coordinate slightly outwards to
 * avoid a crack due to T-junctions.
 * 
 * 9     12/21/97 10:58p Rwyatt
 * Changed CMArray to CDArray
 * 
 * 8     12/16/97 2:57p Mlange
 * Changed CMArray<> reference to a CDArray<>.
 * 
 * 7     97/07/25 16:58 Speter
 * Increased iMAX_VERTICES_CLIP from 32 to 100 (image cache polygons!)
 * 
 * 6     97/07/23 18:04 Speter
 * Replaced CCamera argument to clipping functions with b_perspective flag.  Added similar
 * clipping functions for mesh polygons.
 * 
 * 5     97/06/10 15:47 Speter
 * Now takes camera as an argument (for new vertex projection).
 * 
 * 4     97/05/21 17:39 Speter
 * Updated for new CMArray argument order.
 * 
 * 3     97-04-21 17:26 Speter
 * Totally rewrote CClipPlane interface, based more cleanly on templates, and inherited from
 * CPlane.  Removed InterpolateVertex(), now use SRenderVertex constructor.
 * 
 * 1     97-04-14 20:41 Speter
 * Contains code moved from GeomTypes.hpp.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Clip.hpp"

//
// Global clipping values.
//
// Notes:
//		Currently these values are used only where polygons are "grown" by the depthsort.
//
float fClipXStart = 0.0f;
float fClipYStart = 0.0f;
float fClipXEnd   = 0.0f;
float fClipYEnd   = 0.0f;


//
// Class implementations.
//

//**********************************************************************************************
//
// CClipPlane implementation.
//

	// A large polygon vertex limit, used for local arrays.
	#define iMAX_VERTICES_CLIP	100

	//******************************************************************************************
	ESideOf CClipPlane::esfClipPolygonInside(CRenderPolygon& rpoly, CPipelineHeap& plh, 
		bool b_perspective) const
	{
		Assert(rpoly.paprvPolyVertices.uLen >= 3);
		Assert(rpoly.paprvPolyVertices.uLen <= iMAX_VERTICES_CLIP);

		// Calculate and store all side codes first.
		ESideOf aesf_poly[iMAX_VERTICES_CLIP];
		ESideOf esf_total = esfIntersectPolygon(rpoly.paprvPolyVertices, aesf_poly);

		//
		// Perform trivial acceptance and rejection.
		//
		if (esf_total != esfINTERSECT)
			return esf_total;

		//
		// The polygon intersects the plane, so we must clip it.
		//

		CMSArray<SRenderVertex*, iMAX_VERTICES_CLIP> maprv_new;

		for (uint u_curr = 0; u_curr < rpoly.paprvPolyVertices.uLen; u_curr++)
		{
			uint u_next = u_curr + 1;
			if (u_next == rpoly.paprvPolyVertices.uLen)
				u_next = 0;

			// If the current vertex is inside or on the plane, add it.
			if (aesf_poly[u_curr] != esfOUTSIDE)
				maprv_new << rpoly.paprvPolyVertices[u_curr];

			// Find the intersection of the current edge with this plane.
			ESideOf esf_edge = aesf_poly[u_curr] | aesf_poly[u_next];

			// If the edge crosses the plane, we must add a new intersecting vertex.
			if (esf_edge == esfINTERSECT)
			{
				// Create a new vertex on the dynamic array, with interpolated values.
				SRenderVertex* prv_intersection = new(plh.darvVertices) SRenderVertex
				(
					*rpoly.paprvPolyVertices[u_curr], *rpoly.paprvPolyVertices[u_next],
					rEdgeT(rpoly.paprvPolyVertices[u_curr]->v3Cam, rpoly.paprvPolyVertices[u_next]->v3Cam),
					b_perspective
				);

				// Insert the vertex pointer in the output list.
				maprv_new << prv_intersection;
			}
		}

		// Copy the polygon vertex pointer array back onto the heap.
		if (rpoly.paprvPolyVertices.end() == plh.daprvVertices.end())
		{
			// These vertex pointers are at the end of the heap, so we can eliminate them.
			plh.daprvVertices -= rpoly.paprvPolyVertices.uLen;
		}

		// Copy them all to the end of the heap.
		rpoly.paprvPolyVertices.atArray = plh.daprvVertices.end();
		rpoly.paprvPolyVertices.uLen = maprv_new.uLen;
		plh.daprvVertices << maprv_new;

		//
		// This should be a complete polygon.  If only a point or edge lay on the plane,
		// it should have been rejected.
		//
		Assert(rpoly.paprvPolyVertices.uLen >= 3);

		return esfINTERSECT;
	}

	//******************************************************************************************
	ESideOf CClipPlane::esfSplitPolygon(CRenderPolygon& rpoly, CPipelineHeap& plh,
		bool b_perspective, CRenderPolygon** pprpoly_out, float f_adjust_out) const
	{
		Assert(rpoly.paprvPolyVertices.uLen >= 3);
		Assert(rpoly.paprvPolyVertices.uLen <= iMAX_VERTICES_CLIP);

		// Calculate and store all side codes first.
		ESideOf aesf_poly[iMAX_VERTICES_CLIP];
		ESideOf esf_total = esfIntersectPolygon(rpoly.paprvPolyVertices, aesf_poly);

		if (esf_total != esfINTERSECT)
			// The polygon does not cross this plane.
			return esf_total;

		//
		// The polygon intersects the plane, so we must split it.
		//

		// Create a new polygon on the heap, copied from the original.
		CRenderPolygon& rpoly_in  = rpoly;
		CRenderPolygon& rpoly_out = *new(plh.darpolyPolygons) CRenderPolygon(rpoly);

		// Build the first polygon's vertex pointer list on the pipeline heap.  Remember where we start.
		uint u_vertices_start = plh.daprvVertices.uLen;
		CDArray<SRenderVertex*>& maprv_vertices_in = plh.daprvVertices;

		// Build the second polygon's vertex pointer list on a local array.
		SRenderVertex* aprv_vertices_out[iMAX_VERTICES_CLIP];
		CMArray<SRenderVertex*> maprv_vertices_out(iMAX_VERTICES_CLIP, aprv_vertices_out);

		uint u_curr;
		for (u_curr = 0; u_curr < rpoly.paprvPolyVertices.uLen; u_curr++)
		{
			uint u_next = u_curr + 1;
			if (u_next == rpoly.paprvPolyVertices.uLen)
				u_next = 0;

			// If the current vertex is inside or on the plane, add it to the inside polygon.
			if (aesf_poly[u_curr] != esfOUTSIDE)
				maprv_vertices_in << rpoly.paprvPolyVertices[u_curr];

			// Likewise with the outside polygon.
			if (aesf_poly[u_curr] != esfINSIDE)
				maprv_vertices_out << rpoly.paprvPolyVertices[u_curr];

			// Find the intersection of the current edge with this plane.
			ESideOf esf_edge = aesf_poly[u_curr] | aesf_poly[u_next];

			// If the edge crosses the plane, we must add a new intersecting vertex to both polygons.
			if (esf_edge == esfINTERSECT)
			{
				// Create a new vertex on the dynamic array, with interpolated values.
				SRenderVertex* prv_intersection = new(plh.darvVertices) SRenderVertex
				(
					*rpoly.paprvPolyVertices[u_curr], *rpoly.paprvPolyVertices[u_next],
					rEdgeT(rpoly.paprvPolyVertices[u_curr]->v3Cam, rpoly.paprvPolyVertices[u_next]->v3Cam),
					b_perspective
				);

				// Move the vertex outward as required.
				if (f_adjust_out)
				{
					// Find the 2D screen-space edge vector.
					CVector2<> v2_edge = (const CVector2<>&)rpoly.paprvPolyVertices[u_curr]->v3Screen -
										 (const CVector2<>&)rpoly.paprvPolyVertices[u_next]->v3Screen;

					// Generate its perpendicular, of the desired length.
					CVector2<> v2_adjust(v2_edge.tY, -v2_edge.tX);
					v2_adjust.Normalise(f_adjust_out);

					// Get the new position.
					float f_x = prv_intersection->v3Screen.tX + v2_adjust.tX;
					float f_y = prv_intersection->v3Screen.tY + v2_adjust.tY;

					// Add the new position if it is not outside the clip region.
					if (f_x >= fClipXStart && f_x < fClipXEnd)
						prv_intersection->v3Screen.tX = f_x;
					if (f_y >= fClipYStart && f_y < fClipYEnd)
						prv_intersection->v3Screen.tY = f_y;
				}

				// Add the vertex pointer to both lists.
				maprv_vertices_in  << prv_intersection;
				maprv_vertices_out << prv_intersection;
			}
		}

		// Set the first polygon's vertex array to the one we just built on the heap.
		rpoly_in.paprvPolyVertices = plh.daprvVertices.paSegment(u_vertices_start);

		// Copy the second polygon's vertices to the heap.
		u_vertices_start = plh.daprvVertices.uLen;
		for (u_curr = 0; u_curr < maprv_vertices_out.uLen; u_curr++)
			plh.daprvVertices << maprv_vertices_out[u_curr];

		// Set the polygon's vertices to the ones we just built.
		rpoly_out.paprvPolyVertices = plh.daprvVertices.paSegment(u_vertices_start);

		//
		// These should be complete polygons.
		//
		Assert(rpoly_in.paprvPolyVertices.uLen >= 3);
		Assert(rpoly_out.paprvPolyVertices.uLen >= 3);

		// Return the pointer to the new polygon.
		if (pprpoly_out)
			*pprpoly_out = &rpoly_out;

		return esfINTERSECT;
	}


	//******************************************************************************************
	ESideOf CClipPlane::esfClipPolygon(CMesh::SPolygon& mp, CMesh::CHeap& mh, ESideOf esf_sides) const
	{
		Assert(mp.papmvVertices.uLen >= 3);
		Assert(mp.papmvVertices.uLen <= iMAX_VERTICES_CLIP);

		// Calculate and store all side codes first.
		ESideOf aesf_poly[iMAX_VERTICES_CLIP];
		ESideOf esf_total = esfIntersectPolygonMesh(mp.papmvVertices, aesf_poly);

		//
		// Perform trivial acceptance and rejection.
		//
		if (esf_total != esfINTERSECT)
			return esf_total;

		//
		// The polygon intersects the plane, so we must clip it.
		//

		// An array of vertex pointers for the new polygon, if splitting.
		//CMLArray(CMesh::SVertex*, mapmv_vertices_new, iMAX_VERTICES_CLIP);
		CDArray<CMesh::SVertex*> mapmv_vertices_new(iMAX_VERTICES_CLIP);

		// Remember where we start building the polygon's new vertex array.
		uint u_vertices_start = mh.mapmvVertices.uLen;

		// If we keep the inside polygon, it always creates its vertices on the heap.
		CDArray<CMesh::SVertex*>& mapmv_vertices_in = mh.mapmvVertices;

		// If the keep only the outside polygon, it creates its vertices on the heap.
		// If we keep both, it creates its vertices on the local array.
		CDArray<CMesh::SVertex*>& mapmv_vertices_out = 
			esf_sides == esfOUTSIDE ? mh.mapmvVertices
			: mapmv_vertices_new;

		uint u_curr;
		for (u_curr = 0; u_curr < mp.papmvVertices.uLen; u_curr++)
		{
			uint u_next = u_curr + 1;
			if (u_next == mp.papmvVertices.uLen)
				u_next = 0;

			// If the current vertex is inside or on the plane, add it to the inside poly.
			if (esf_sides & esfINSIDE)
				if (aesf_poly[u_curr] != esfOUTSIDE)
					mapmv_vertices_in << mp.papmvVertices[u_curr];

			// Likewise for the outside.
			if (esf_sides & esfOUTSIDE)
				if (aesf_poly[u_curr] != esfINSIDE)
					mapmv_vertices_out << mp.papmvVertices[u_curr];

			// Find the intersection of the current edge with this plane.
			ESideOf esf_edge = aesf_poly[u_curr] | aesf_poly[u_next];

			// If the edge crosses the plane, we must add a new intersecting vertex.
			if (esf_edge == esfINTERSECT)
			{
				// Create a new vertex on the dynamic array, with interpolated values.
				CMesh::SVertex* pmv_intersection = new(mh.mamvVertices) CMesh::SVertex
				(
					*mp.papmvVertices[u_curr], *mp.papmvVertices[u_next],
					rEdgeT(*mp.papmvVertices[u_curr]->pv3Point, *mp.papmvVertices[u_next]->pv3Point),
					mh
				);

				// Add the vertex pointer to the output list(s).
				if (esf_sides & esfINSIDE)
					mapmv_vertices_in << pmv_intersection;
				if (esf_sides & esfOUTSIDE)
					mapmv_vertices_out << pmv_intersection;
			}
		}

		// Set the first polygon's vertices to the ones we just built (inside or outside).
		mp.papmvVertices = mh.mapmvVertices.paSegment(u_vertices_start);

		//
		// This should be a complete polygon.  If only a point or edge lay on the plane,
		// it should have been rejected.
		//
		Assert(mp.papmvVertices.uLen >= 3);

		if (esf_sides == esfINTERSECT)
		{
			// We are splitting, and must create a new polygon for the outside.
			// Copy it from the first polygon.
			CMesh::SPolygon& mp_out = *new(mh.mampPolygons) CMesh::SPolygon(mp);

			// Copy the outside vertices to the heap.
			u_vertices_start = mh.mapmvVertices.uLen;
			for (u_curr = 0; u_curr < mapmv_vertices_out.uLen; u_curr++)
				mh.mapmvVertices << mapmv_vertices_out[u_curr];

			// Set the polygon's vertices to the ones we just built.
			mp_out.papmvVertices = mh.mapmvVertices.paSegment(u_vertices_start);
			Assert(mp_out.papmvVertices.uLen >= 3);
		}

		return esfINTERSECT;
	}

//**********************************************************************************************
void SetClipRegion(float f_x_0, float f_y_0, float f_x_1, float f_y_1)
{
	fClipXStart = f_x_0;
	fClipYStart = f_y_0;
	fClipXEnd   = f_x_1;
	fClipYEnd   = f_y_1;
}
