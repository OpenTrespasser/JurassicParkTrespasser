/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CBioMesh::CPolyIterator
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/SkeletonIterator.hpp                                    $
 * 
 * 9     98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 8     98/09/14 1:40 Speter
 * Added iNumVertexPointers(), for pipeline heap. bBackface() now called, because of compiler's
 * abominable inlining.
 * 
 * 7     7/10/98 5:10p Mlange
 * Poly iterators now take a render context instead of a camera.
 * 
 * 6     98/07/01 0:44 Speter
 * Fixed egregious bug affecting lighting and wire displays.
 * 
 * 5     98/06/29 16:16 Speter
 * Moved CSkeletonRenderInfo out of CBioMesh and into CAnimate. Rendering functions now take the
 * CInstance* parameter, and use CAnimate's joints.
 * 
 * 4     98/06/26 0:22 Speter
 * Made v3Point return the correct object-space point; fixes wireframe, vertex bones, and
 * doesn't slow down significantly.
 * 
 * 3     6/24/98 4:51p Agrant
 * Renormalize normals for double joint verts
 * 
 * 2     98/06/07 19:03 Speter
 * Bio-iterator now always used. Removed relative point and normal arrays; now stuff original
 * mesh with relative values.
 * 
 * 1     98/06/01 19:32 Speter
 * Separated from Skeleton.hpp.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_SKELETON_ITERATOR_HPP
#define HEADER_LIB_GEOMDBASE_SKELETON_ITERATOR_HPP

#include "Skeleton.hpp"
#include "MeshIterator.hpp"

//**********************************************************************************************
//
// class CBioMesh
//

	//
	// Const variables.
	//
	const TReal rPolyPlaneThicknessCam = 0.0002 / 500;
	
	//******************************************************************************************
	//
	class CBioMesh::CPolyIterator: public CMesh::CPolyIterator
	//
	// Extension of CMesh::CPolyIterator.
	//
	//**************************************
	{
	protected:
		const CBioMesh& bmshMesh;				// The referenced mesh.
		CPArray< CTransform3<> > patf3Joints;	// The bones of the instance.
		CPArray<SClipPoint> paclptPoints;		// Copied pointer to clip-point array.

	public:

		//******************************************************************************************
		CPolyIterator(const CBioMesh& msh, const CInstance* pins, const CRenderContext* prenc);

		//**************************************************************************************
		//
		// Overrides.
		//

		//******************************************************************************************
		forceinline bool bBackface(const CVector3<>&)
		{
			// Ignore camera position, not needed.
			return bBackface();
		}

		//
		// The following function is slow, as it computes the polygon's normal.
		// However, there doesn't seem to be a faster way of culling in an animating mesh.
		//
		//******************************************************************************************
		forceinline bool bBackface()
		{
			// Compute normal in camera space, using already transformed points.
			const CVector3<>& v3_a = paclptPoints[iShapePoint(0)].v3Point;
			const CVector3<>& v3_b = paclptPoints[iShapePoint(1)].v3Point;
			const CVector3<>& v3_c = paclptPoints[iShapePoint(2)].v3Point;

			TReal r_dist = ((v3_b - v3_c) ^ (v3_a - v3_b)) * v3_b;
			return CIntFloat(r_dist).bSign();
		}

		//
		// The following 2 functions are slow, and should not be used in pipeline rendering.
		// bBackface() is used instead of plPlane().
		// d3Normal() is only used for flat-shading, which will not occur because all polygons
		// will indicate bCurved() = true.
		//

		CPlane plPlane();

		CDir3<> d3Normal()
		{
			return plPlane().d3Normal;
		}


		CVector3<> v3Point()
		{
			// Just return first point.
			return v3Point(0);
		}

		CVector3<> v3Point(int i_poly_vertex)
		{
			// Object-space point. Cannot take advantage of pre-transformed camera-space point.
			int i_point = iShapePoint(i_poly_vertex);
			int i_joint = bmshMesh.pauJoints[i_point];
			return bmshMesh.pav3Points[i_point] * patf3Joints[i_joint];
		}

		CDir3<> d3Normal(int i_poly_vertex)
		{
			// Transform the normal into object space with the current joint transform.
			int i_vertex = iShapeVertex(i_poly_vertex);
			int i_joint = bmshMesh.pauJoints[ iShapePoint(i_poly_vertex) ];

			// Are we using a double joint?
			if (i_joint >= bmshMesh.iJoints)
			{
				// Yes!  Renormalize before returning.
				CVector3<> v3 = ((CVector3<>&)bmshMesh.pamvVertices[i_vertex].d3Normal) * patf3Joints[i_joint].mx3Mat;
				//v3.Normalise();
				return v3;
			}
			else
				// No!  Length is preserved, so go ahead and return.
				return bmshMesh.pamvVertices[i_vertex].d3Normal * patf3Joints[i_joint].mx3Mat;
		}

		bool bOcclude()
		{
			return false;
		}

		bool bCurved()
		{
			return true;
		}

		//******************************************************************************************
		//
		// Virtual overrides.
		//

		//******************************************************************************************
		void TransformPoints(const CTransform3<>& tf3_shape_camera, const CCamera& cam,
			CPArray<SClipPoint> paclpt_points, bool b_outcodes) override;

		bool bBackface_(const CVector3<>& v3_cam) override
		{
			return bBackface();
		}

		CPlane plPlane_() override
		{
			return plPlane();
		}

		bool bCurved_() override
		{
			return bCurved();
		}

		CVector3<> v3Point_() override
		{
			return v3Point();
		}

		CVector3<> v3Point_(int i_poly_vertex) override
		{
			return v3Point(i_poly_vertex);
		}

		CDir3<> d3Normal_(int i_poly_vertex) override
		{
			return d3Normal(i_poly_vertex);
		}

	};

#endif
