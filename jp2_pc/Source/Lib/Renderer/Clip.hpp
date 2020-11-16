/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CClipPlane
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Clip.hpp                                                 $
 * 
 * 9     98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 8     1/07/98 4:05p Pkeet
 * Added the clip region function and globals.
 * 
 * 7     12/23/97 1:55p Pkeet
 * Added a parameter to use when splitting polygons to move the coordinate slightly outwards to
 * avoid a crack due to T-junctions.
 * 
 * 6     97/07/23 18:03 Speter
 * Replaced CCamera argument to clipping functions with b_perspective flag.  Added similar
 * clipping functions for mesh polygons.
 * 
 * 5     97/06/10 15:47 Speter
 * Now takes camera as an argument (for new vertex projection).
 * 
 * 4     97/05/23 6:24p Pkeet
 * Untemplatized CPlaneDef.
 * 
 * 3     97-04-21 17:26 Speter
 * Totally rewrote CClipPlane interface, based more cleanly on templates, and inherited from
 * CPlane.  Removed InterpolateVertex(), now use SRenderVertex constructor.
 * 
 * 1     97-04-14 20:41 Speter
 * Contains code moved from GeomTypes.hpp.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_CLIP_HPP
#define HEADER_LIB_RENDERER_CLIP_HPP

#include "ScreenRender.hpp"
#include "PipeLineHeap.hpp"
#include "Lib/GeomDBase/Plane.hpp"
#include "Lib/GeomDBase/Mesh.hpp"

//**********************************************************************************************
//
class CClipPlane
//
// Abstract class which provides plane clipping functions.
//
// Prefix: clp
//
//**************************************
{
public:

	//******************************************************************************************
	//
	virtual TReal rEdgeT
	(
		const CVector3<>& v3_0,			// Vectors describing edge to intersect.
		const CVector3<>& v3_1
	) const = 0;
	//
	// Calculate the point of intersection between an edge and this plane.
	//
	// Returns:
	//		The parameter of the point of intersection between the edge and the bounding
	//		volume. (e.g this value will be 0 if the point of intersection falls at the
	//		starting point of the edge and 1 if it falls at the end point.)
	//
	// Cross-references:
	//		Invoked by bClipPolygon... functions, to create new vertices on the plane when
	//		clipping.  Must be implemented by derived class.
	//
	//**********************************

	//
	// Clipping functions for CRenderPolygons.
	//

	//******************************************************************************************
	//
	virtual ESideOf esfIntersectPolygon
	(
		CPArray<SRenderVertex*> paprv_poly,	// Polygon to test, in this volume's space.
		ESideOf aesf_sides[]				// Array of ESideOf codes to set.
	) const = 0;
	//
	// Calculates the side codes of each vertex, and stores them in the array.
	//
	// Returns:
	//		Where this polygon is in relation to the volume (the combination of the side codes
	//		of the vertices).
	//
	// Cross-references:
	//		Invoked by bClipPolygon... functions, where the array of side codes is used to clip 
	//		the polygon if it intersects the plane.  Must be implemented by derived class.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual ESideOf esfClipPolygonInside
	(
		CRenderPolygon& rpoly,			// The render polygon to clip.
		CPipelineHeap& plh,				// The heap object to allocate new vertices on.
		bool b_perspective = true		// Whether to interpolate the screen coords as projected.
	) const;
	//
	// Clips the polygon against this volume.  If the polygon intersects the boundary of the
	// volume, this function converts it to a new polygon lying entirely inside the volume.
	// Otherwise (it is entirely inside or outside the volume), it leaves it unchanged.
	//
	// Returns:
	//		The side code of the original polygon.  If esfOUTSIDE or esfINSIDE, the polygon 
	//		is unchanged; if esfINTERSECT, it was clipped, and is now inside the plane.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual ESideOf esfSplitPolygon
	(
		CRenderPolygon& rpoly,				// The render polygon to split.
		CPipelineHeap& plh,					// The heap object to allocate new objects on.
		bool b_perspective = true,			// Whether to interpolate the screen coords as projected.
		CRenderPolygon** pprpoly_out = 0,	// Returns the outside polygon, if split.
		float f_adjust_out = 0.0f
	) const;
	//
	// Splits the polygon by this plane, if it intersects: the original polygon is converted
	// to one lying inside the plane; a new polygon is created on the pipeline heap, lying 
	// outside the plane.  If the polygon does not intersect the plane, nothing happens.
	//
	// Returns:
	//		The intersection code of the polygon; if esfINTERSECT, it was split.
	// 
	//**********************************

	//
	// Similar functions for CMesh::SPolygons.
	//

	//******************************************************************************************
	//
	virtual ESideOf esfIntersectPolygonMesh
	(
		CPArray<CMesh::SVertex*> papmv_poly,	// Polygon to test, in this volume's space.
		ESideOf aesf_sides[]					// Array of ESideOf codes to set.
	) const = 0;
	//
	//**********************************

	//******************************************************************************************
	//
	virtual ESideOf esfClipPolygon
	(
		CMesh::SPolygon& mp,			// The mesh polygon to clip.
		CMesh::CHeap& mh,				// Where to allocate new mesh data.
		ESideOf esf_sides				// Which sides to keep: esfINSIDE or esfOUTSIDE;
										// if both, creates the outside polygon on the heap.
	) const;
	//
	//**********************************
};

//**********************************************************************************************
//
template<class P> class CClipPlaneT : public CClipPlane, public CPlaneT<P>
//
// Implements CClipPlane by extending a CPlaneT<P>.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	// Use plane's default constructor.
	CClipPlaneT()
	{
	}

	// Initialise with a plane object.
	CClipPlaneT(const P& p)
		: CPlaneT<P>(p)
	{
	}

	//******************************************************************************************
	TReal rEdgeT(const CVector3<>& v3_0, const CVector3<>& v3_1) const
	{
		// Implemented with rDistance().
		TReal r_0 = rDistance(v3_0);
		TReal r_1 = rDistance(v3_1);
		return r_0 / (r_0 - r_1);
	}

	//******************************************************************************************
	virtual ESideOf esfIntersectPolygon(CPArray<SRenderVertex*> paprv_poly, ESideOf aesf_sides[]) const
	{
		// Return combination of all points.  Use esfSideOf, to return esfON when appropriate,
		// for storage in array.
		ESideOf esf = esfON;

		for (uint u = 0; u < paprv_poly.uLen; u++)
		{
			aesf_sides[u] = esfSideOf(paprv_poly[u]->v3Cam);
			esf |= aesf_sides[u];
		}
		return esf;
	}

	//******************************************************************************************
	virtual ESideOf esfIntersectPolygonMesh(CPArray<CMesh::SVertex*> papmv_poly, ESideOf aesf_sides[]) const
	{
		// Return combination of all points.  Use esfSideOf, to return esfON when appropriate,
		// for storage in array.
		ESideOf esf = esfON;

		for (uint u = 0; u < papmv_poly.uLen; u++)
		{
			aesf_sides[u] = esfSideOf(*papmv_poly[u]->pv3Point);
			esf |= aesf_sides[u];
		}
		return esf;
	}
};


//
// Typedefs for general purpose plane classes
//

typedef CClipPlaneT<CPlaneDef> CClipPlaneGeneral;

//**********************************************************************************************
//
class CClipPlaneTolerance: public CClipPlaneT<CPlaneDefTolerance>
//
// Inherit rather than typedef, to add a convenient constructor.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	// Use plane's default constructor.
	CClipPlaneTolerance(const CPlane& pl, TReal r_tolerance = fPLANE_TOLERANCE)
		: CClipPlaneT<CPlaneDefTolerance>(CPlaneTolerance(pl, r_tolerance))
	{
	}
};


//**********************************************************************************************
//
void SetClipRegion
(
	float f_x_0,
	float f_y_0,
	float f_x_1,
	float f_y_1
);
//
// Sets the clip region globally. Currently the clip region is only used for polygons "grown" by
// the depth sort.
//
//**************************************

#endif
