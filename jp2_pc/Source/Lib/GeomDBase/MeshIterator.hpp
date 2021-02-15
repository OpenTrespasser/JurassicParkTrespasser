/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CMesh
 *		CMeshRect
 *
 * Bugs:
 *
 * To do:
 *		Move the definition of CShapeRect to the water files. It is only ever used there.
 *
 *		Implement the copy function.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/MeshIterator.hpp                                        $
 * 
 * 5     9/24/98 5:41p Asouth
 * changed inline asm ; comments to //
 * 
 * 4     98/09/15 0:45 Speter
 * Added count functions to PolyIterator; helps terrain be more accurate.
 * 
 * 3     7/10/98 5:10p Mlange
 * Poly iterators now take a render context instead of a camera.
 * 
 * 2     98/06/29 16:07 Speter
 * Rendering functions now take the CInstance* parameter.
 * 
 * 1     98/06/01 19:05 Speter
 * Separated from corresponding shape file.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_MESH_ITERATOR_HPP
#define HEADER_LIB_GEOMDBASE_MESH_ITERATOR_HPP

#include "Mesh.hpp"
#include "Config.hpp"
#include "AsmSupport.hpp"

//******************************************************************************************
//

	//******************************************************************************************
	//
	class CMesh::CPolyIterator: public CShape::CPolyIterator
	//
	// Implementation of CShape::CPolyIterator.
	//
	//**************************************
	{
	protected:
		CPArray<SVertex*> papmvVertices;	// A copy of the array for the current polygon.
		const SPolygon	*pmpCurrent;		// The current polygon in iteration.
		const CMesh		&mshMesh;			// Mesh we're iterating through.
		const CCamera	*pcamCamera;		// Camera used to render view, or null if none specified.

	public:

		//******************************************************************************************
		//
		// Constructor and destructor.
		//

		CPolyIterator(const CMesh& msh, const CInstance* pins, const CRenderContext* prenc);

		~CPolyIterator();

		//******************************************************************************************
		//
		// Non-virtual overrides.
		//

		void Reset()
		{
			pmpCurrent = mshMesh.pampPolygons - 1;
		}

		//******************************************************************************************
		int iNumPoints() const
		{
			return mshMesh.pav3Points.uLen;
		}

		//******************************************************************************************
		int iNumShapeVertices() const
		{
			return mshMesh.pamvVertices.uLen;
		}

		//******************************************************************************************
		int iNumVertexPointers() const
		{
			return mshMesh.papmvVertices.uLen;
		}

		//******************************************************************************************
		int iNumPolygons() const
		{
			return mshMesh.pampPolygons.uLen;
		}

		//******************************************************************************************
		bool bNext()
		{
			if (++pmpCurrent >= mshMesh.pampPolygons.end())
				return false;

			// Copy vertex array pointer, for faster access.
			papmvVertices = pmpCurrent->papmvVertices;
			return true;
		}

		//******************************************************************************************
		bool bBackface(const CVector3<>& v3_cam)
		{
			// Back face cull. Determine if the camera lies to the negative side of the polygon's plane,
			// adjusted by some value for numerical accuracy.

		#if (TARGET_PROCESSOR == PROCESSOR_K6_3D && VER_ASM)

			TReal r_plane_adj_dist;

			const CPlane* ppl = &plPlane();
			TReal r_thickness = rPolyPlaneThicknessDefault;

			typedef CVector3<> tdCVector3;

			Assert((char *)&ppl->d3Normal.tX - (char *)&ppl->d3Normal == 0);
			Assert((char *)&ppl->d3Normal.tY - (char *)&ppl->d3Normal == 4);
			Assert((char *)&ppl->d3Normal.tZ - (char *)&ppl->d3Normal == 8);

			__asm
			{
				mov		eax,[ppl]						// get ptr to plane of polygon

				femms									// ensure fast switch

				mov		ebx,[v3_cam]					// get ptr to camera vector
				jmp		StartAsm3

				align 16
			StartAsm3:
				movq	mm0,[eax+0]CPlaneDef.d3Normal	// m0=   pl.Y   | pl.X

				movq	mm1,[ebx]tdCVector3.tX			// m1=   v3.Y   | v3.X

				movd	mm2,[eax+8]CPlaneDef.d3Normal	// m2=     0    | pl.Z
				test	ebx,ebx							// 2-byte NOOP to avoid degraded predecode

				movd	mm3,[ebx]tdCVector3.tZ			// m3=     0    | v3.Z
				pfmul	(m0,m1)							// m0= pl.Y*v.Y | pl.X*v.X

				movd	mm4,[eax]CPlaneDef.rD			// m4= rD
				movd	mm5,[r_thickness]				// m5= rPOLYGON_PLANE_THICKNESS

				pfmul	(m2,m3)							// m2=     0    | pl.Z*v3.Z
				pfacc	(m0,m0)							// m0= pl.Y*v3.Y + pl.X*v3.X

				pfadd	(m0,m2)							// m0= pl.Y*v3.Y + pl.X*v3.X + pl.Z*v3.Z
				pfsub	(m4,m5)							// m0= rD - rPOLYGON_PLANE_THICKNESS

				pfadd	(m0,m4)							// m0= v3*d3Normal + rD - rPOLYGON_PLANE_THICKNESS
													//   = rDistance - rPOLYGON_PLANE_THICKNESS
				movd	[r_plane_adj_dist],mm0

				femms									// empty MMX state and ensure fast switch
			}

		#else // if (TARGET_PROCESSOR == PROCESSOR_K6_3D && VER_ASM)

			TReal r_plane_adj_dist = plPlane().rDistance(v3_cam) - rPolyPlaneThicknessDefault;

		#endif // else

			return CIntFloat(r_plane_adj_dist).bSign();
		}

		const CPlane& plPlane()
		{
			Assert(pmpCurrent);
			return pmpCurrent->plPlane;
		}

		const CVector3<>& v3Point()
		{
			// Just return first point.
			return v3Point(0);
		}

		const CDir3<>& d3Normal()
		{
			return plPlane().d3Normal;
		}

		bool bCurved()
		{
			Assert(pmpCurrent);
			return pmpCurrent->bCurved;
		}

		const CMatrix3<>& mx3ObjToTexture()
		{
			Assert(pmpCurrent);
			return *pmpCurrent->pmx3ObjToTexture;
		}

		const CTexture* ptexTexture()
		{
			// Convert rptr to raw pointer.
			Assert(pmpCurrent);
			Assert(pmpCurrent->pSurface);
			Assert(pmpCurrent->pSurface->ptexTexture);
			return pmpCurrent->pSurface->ptexTexture.ptGet();
		}

		int iNumVertices()
		{
			return papmvVertices.uLen;
		}

		const CVector3<>& v3Point(int i_poly_vertex)
		{
			return *papmvVertices[i_poly_vertex]->pv3Point;
		}

		const CDir3<>& d3Normal(int i_poly_vertex)
		{
			return papmvVertices[i_poly_vertex]->d3Normal;
		}

		const CTexCoord& tcTexCoord(int i_poly_vertex)
		{
			return papmvVertices[i_poly_vertex]->tcTex;
		}

		int iShapeVertex(int i_poly_vertex)
		{
			// Make sure the index indicates the actual position in the vertex array.
			Assert(pmvVertex(i_poly_vertex)->u4ShapeVertex == pmvVertex(i_poly_vertex) - mshMesh.pamvVertices);
			return papmvVertices[i_poly_vertex]->u4ShapeVertex;
		}

		int iShapePoint(int i_poly_vertex)
		{
			// Make sure the index indicates the actual position in the point array.
			Assert(pmvVertex(i_poly_vertex)->u4ShapePoint == pmvVertex(i_poly_vertex)->pv3Point - mshMesh.pav3Points);
			return papmvVertices[i_poly_vertex]->u4ShapePoint;
		}

		bool bOcclude()
		{
			Assert(pmpCurrent);
			return pmpCurrent->bOcclude;
		}

		bool bCache()
		{
			Assert(pmpCurrent);
			return pmpCurrent->bCache;
		}

		//******************************************************************************************
		//
		// Member functions.
		//

	public:

		//******************************************************************************************
		SPolygon* pmpPolygon()
		//
		// Returns:
		//		A writable pointer to the current iterated polygon.
		//
		// Notes:
		//		Sort of violates encapsulation, but needed by various utilities which manipulate
		//		the mesh.
		//
		//******************************
		{
			return const_cast<SPolygon*>(pmpCurrent);
		}

		//******************************************************************************************
		SVertex* pmvVertex(int i_poly_vertex) const
		//
		// Returns:
		//		A pointer to the actual mesh vertex structure.
		//
		// Notes:
		//		Sort of violates encapsulation, but needed by various utilities which manipulate
		//		the mesh.
		//
		//******************************
		{
			Assert(pmpCurrent);
			return const_cast<SVertex*>(papmvVertices[i_poly_vertex]);
		}

		//******************************************************************************************
		const SVertex** papmvVertices_3DX() const
		//
		// Returns:
		//		A pointer to the actual array of mesh vertex structure pointers.
		//
		// Notes:
		//		Sort of violates encapsulation, but needed by the 3DX version of CShapeCache::Render 
		//		which must loop through the vertices of a polygon.
		//
		//******************************
		{
			Assert(pmpCurrent);
			return (const SVertex**)papmvVertices.atArray;
		}

		//******************************************************************************************
		//
		// Virtual overrides.
		//

		//******************************************************************************************
		void TransformPoints(const CTransform3<>& tf3_shape_camera, const CCamera& cam,
			CPArray<SClipPoint> paclpt_points, bool b_outcodes) override;

		void Reset_() override
		{
			Reset();
		}

		bool bNext_() override
		{
			return bNext();
		}

		bool bBackface_(const CVector3<>& v3_cam) override
		{
			return bBackface(v3_cam);
		}

		CPlane plPlane_() override
		{
			return plPlane();
		}

		CVector3<> v3Point_() override
		{
			return v3Point();
		}

		bool bCurved_() override
		{
			return bCurved();
		}

		CMatrix3<> mx3ObjToTexture_() override
		{
			return mx3ObjToTexture();
		}

		const CTexture* ptexTexture_() override
		{
			return ptexTexture();
		}

		int iNumVertices_() override
		{
			return iNumVertices();
		}

		CVector3<> v3Point_(int i_poly_vertex) override
		{
			return v3Point(i_poly_vertex);
		}

		CDir3<> d3Normal_(int i_poly_vertex) override
		{
			return d3Normal(i_poly_vertex);
		}

		CTexCoord tcTexCoord_(int i_poly_vertex) override
		{
			return tcTexCoord(i_poly_vertex);
		}

		int iShapeVertex_(int i_poly_vertex) override
		{
			return iShapeVertex(i_poly_vertex);
		}

		int iShapePoint_(int i_poly_vertex) override
		{
			return iShapePoint(i_poly_vertex);
		}

		friend class CMesh;
	};


#endif
