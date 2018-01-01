/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaterQuadTree.hpp                                       $
 * 
 * 6     5/15/98 6:25p Mlange
 * Definition of water quad tree on build version switch.
 * 
 * 5     5/11/98 1:01p Mlange
 * Water quad tree work in progress.
 * 
 * 4     5/06/98 11:43a Mlange
 * Fixed broken check-in.
 * 
 * 3     5/06/98 10:34a Mlange
 * Work in progress.
 * 
 * 2     5/05/98 4:31p Mlange
 * Reorganised data members among the quad tree class hierarchy.
 * 
 * 1     5/04/98 1:00p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_WATERQUADTREE_HPP
#define HEADER_LIB_GEOMDBASE_WATERQUADTREE_HPP

#if VER_MULTI_RES_WATER

#include "Lib/Std/RangeVar.hpp"
#include "Lib/Std/BlockAllocator.hpp"
#include "Lib/Transform/Matrix2.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeBase.hpp"
#include "Lib/GeomDBase/Shape.hpp"


class CCamera;
class CInstance;
class CRasterWin;
class CColour;
class CDraw;

namespace NMultiResolution
{
	//**********************************************************************************************
	//
	class CQuadVertexWater : public CQuadVertex,
	                         public CBlockAllocator<CQuadVertexWater>
	//
	// Vertex type for the water quad tree.
	//
	// Prefix: qvtw
	//
	//**************************************
	{
		friend class CBlockAllocator<CQuadVertexWater>;
		static CBlockAllocator<CQuadVertexWater>::SStore stStore; // Storage for the types.

		int iDummyPad;

	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//
		CQuadVertexWater(int i_x, int i_y)
			: CQuadVertex(i_x, i_y)
		{
		}

		// Construct a new vertex at the midpoint of an edge.
		CQuadVertexWater(const CQuadVertexWater* pqvt_a, const CQuadVertexWater* pqvt_b)
			: CQuadVertex(pqvt_a, pqvt_b)
		{
		}

	};


	class CQuadRootWater;

	//**********************************************************************************************
	//
	class CQuadNodeWater : public CQuadNodeBaseT<CQuadNodeWater, CQuadVertexWater>,
	                       public CBlockAllocator<CQuadNodeWater>
	//
	// Definition of a single node in the water quad tree.
	//
	// Prefix: qnw
	//
	//**************************************
	{
		friend class CBlockAllocator<CQuadNodeWater>;
		static CBlockAllocator<CQuadNodeWater>::SStore stStore; // Storage for the types.

	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		CQuadNodeWater()
		{
		}


		// Construct a new descendant.
		CQuadNodeWater
		(
			const CQuadNodeWater* ptqn_parent, int i_relation,
			CQuadNodeWater* aptqn_subdiv_neighbours[4],	CQuadVertexWater* apqvt_dsc[5]
		)
			: CQuadNodeBaseT<CQuadNodeWater, CQuadVertexWater>(ptqn_parent, i_relation, aptqn_subdiv_neighbours, apqvt_dsc)
		{
		}


		// Destructor.
		~CQuadNodeWater()
		{
		}


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		void Grow();
		//
		// Grow this node e.g. add four descendants.
		//
		// Notes:
		//		This function ensures that the subdivision level of the neighbouring nodes never
		//		differs by more than one relative to the subdivision level of this node.
		//
		//**************************************


		//******************************************************************************************
		//
		void Reduce();
		//
		// Reduce this node e.g. remove its four descendants.
		//
		// Notes:
		//		This function ensures that the subdivision level of the neighbouring nodes never
		//		differs by more than one relative to the subdivision level of this node.
		//
		//**************************************


	protected:
		//******************************************************************************************
		//
		void EvaluateBranch
		(
			const CQuadRootWater* pqnw_root
		);
		//
		//**************************************


		//******************************************************************************************
		//
		void DrawWireframeBranch
		(
			CDraw& draw,
			const CColour& clr
		) const;
		//
		//******************************
	};



	//**********************************************************************************************
	//
	// Declarations for CQuadRootTIN.
	//
	#define uDEFAULT_ALLOC_WATER_NODES	2000u

	struct SEvalInfoWater;

	//**********************************************************************************************
	//
	class CQuadRootWater : public CQuadRootBaseT<CQuadNodeWater, CQuadVertexWater>
	//
	// Definition of the root quad node for the water quad tree.
	//
	// Prefix: qnw
	//
	// Notes:
	//
	//**************************************
	{
	public:
		static CRangeVar<float> rvarMaxDistRatio;
		static CRangeVar<int>   rvarMinDistRatioNodeSize;

		CPresence3<> pr3WorldQuad;		// Transforms between world and quad space.

	private:
		SEvalInfoWater* peiInfo;

		friend class CQuadNodeWater;

	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		// Construct the root node for the tree.
		CQuadRootWater(const CInstance& ins);

		~CQuadRootWater();

		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		void Update
		(
			const CCamera& cam_view		// Camera specifying the observer.
		);
		//
		// Update the quad tree for geometry.
		//
		//**************************************


		//******************************************************************************************
		//
		static void AllocMemory
		(
			uint u_max_nodes = uDEFAULT_ALLOC_WATER_NODES
		);
		//
		// Allocate fastheaps used to store water quad tree types.
		//
		// Notes:
		//		Must be called *before* any instances of this class are created.
		//
		//**************************************


		//******************************************************************************************
		//
		static void FreeMemory();
		//
		// Free fastheaps used to store water quad tree types.
		//
		// Notes:
		//		Must be called *after* all instances of this class are destructed.
		//
		//**************************************

		//******************************************************************************************
		//
		CRectangle<> rcDrawWireframe
		(
			CRasterWin* pras_win,
			const CColour& clr,
			const CVector2<>& v2_quad_pos,
			TReal r_zoom = 1
		) const;
		//
		// Draw a wireframe representation of the current quad tree.
		//
		// Returns:
		//		The rectangle used to construct a CDraw class.
		//
		//******************************
	};


	//**********************************************************************************************
	//
	class CQuadRootWaterShape : public CShape, public CQuadRootWater
	//
	// Prefix:
	//
	// Extends CQuadRootWater by providing a CShape implementation, to render the water.
	//
	//**************************************
	{
	public:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//
		CQuadRootWaterShape(const CInstance& ins);

		~CQuadRootWaterShape();

		// Resolve which version of delete we inherit.
		void operator delete(void* pv)
		{
			CQuadRootWater::operator delete(pv);
		}

		//******************************************************************************************
		//
		// Overrides.
		//

		//******************************************************************************************
		virtual void Render
		(
			CRenderContext&		      renc,				// The rendering context.
			const CPresence3<>&	      pr3_shape_world,	// The shape-to-world transform.
			const CTransform3<>&      tf3_shape_camera,	// The shape to camera transform.
			const CPArray<COcclude*>& papoc,			// Array of occluding objects.
			ESideOf				      esf_view			// Shape's relation to the view volume
														// (for trivial acceptance).
		) const;
		//
		// Defined in Pipeline.cpp.
		//

		virtual const CBoundVol& bvGet() const
		{
//TODO: Add real bound vol code here.
			static CBoundVolInfinite bvi;
			return bvi;
		}

		virtual rptr<CRenderType> prdtCopy()
		{
			Assert(false);
			return rptr0;
		}

		int iNumPolygons() const
		{
//TODO: Make this more accurate.
			// Return an upper bound on the triangle count.
			return iNumPoints() * 2;
		}

		int iNumTriangles() const
		{
			return iNumPolygons() * 2;
		}

		int iNumPoints() const
		{
			return CQuadVertexWater::uMaxAlloc();
		}

		int iNumVertices() const
		{
			// We currently do not share vertices.
			return 0;
		}

		virtual TReal rPolyPlaneThickness() const;


		//******************************************************************************************
		bool bBumpmap() const
		{
			return false;
		}

		//******************************************************************************************
		bool bSharedVertices() const
		{
			return false;
		}

		//******************************************************************************************
		bool bIsCacheable() const
		{
			return false;
		}
		
		//******************************************************************************************
		bool bTerrain() const
		{
			return false;
		}
		

		//******************************************************************************************
		//
		class CPolyIterator: public CShape::CPolyIterator
		//
		// Implementation of CShape::CPolyIterator for CQuadRootTINShape.
		//
		//**************************************
		{
		protected:
			CQuadRootWaterShape& qnwshShape;	// Shape we're iterating through.

			CDArray<CQuadNodeWater*> dapqnwCurrentQuads;// Holds nodes visible in current view.

			CQuadNodeWater** ppqnwCurrent;		// Position in iterator.
			CQuadNodeWater*  pqnwCurrent;		// The current pointer in iteration.

			struct SVertex
			// Prefix: vt
			{
				CVector3<>	v3Point;
				CTexCoord	tcTex;
			};

			SVertex avtVertices[4];				// Stored vertices for the quad.

		public:

			//******************************************************************************************
			//
			// Constructor and destructor.
			//

			CPolyIterator(const CQuadRootWaterShape& qnwsh, const CCamera* pcam = 0);

			~CPolyIterator();

			//******************************************************************************************
			//
			// Overrides.
			//

			//******************************************************************************************
			void TransformPoints
			(
				const CTransform3<>&	tf3_shape_camera,
				const CCamera&			cam,
				CPArray<SClipPoint>		paclpt_points,
				bool					b_outcodes
			);

			//******************************************************************************************
			bool bNext()
			{
				ppqnwCurrent++;
				if (ppqnwCurrent >= dapqnwCurrentQuads.end())
					return false;

				pqnwCurrent = *ppqnwCurrent;
				return true;
			}

			//******************************************************************************************
			const CPlane& plPlane()
			{
//TODO: Return proper quad of current plane here.
				return CPlane();
			}

			//******************************************************************************************
			const CVector3<>& v3Point()
			{
				return v3Point(0);
			}

			//******************************************************************************************
			const CDir3<>& d3Normal()
			{
				return plPlane().d3Normal;
			}

			//******************************************************************************************
			bool bCurved()
			{
//TODO: Determine if this is the right value to return.
				return false;
			}

			//******************************************************************************************
			const CMatrix3<>& mx3ObjToTexture()
			{
				Assert(0);
				return CMatrix3<>();
			}

			//******************************************************************************************
			const CTexture* ptexTexture();

			//******************************************************************************************
			int iNumVertices()
			{
				return 4;
			}

			//******************************************************************************************
			const CVector3<>& v3Point(int i_vertex)
			{
				// Calculate, copy to storage, then return reference.
				Assert(i_vertex < 4);
				Assert(pqnwCurrent);

//TODO: Initialise with proper corner vertex here
//				avtVertices[i_vertex].v3Point = pqnwCurrent->pqvtGetVertex(i_vertex)->
				return avtVertices[i_vertex].v3Point;
			}

			//******************************************************************************************
			const CDir3<>& d3Normal(int i_vertex)
			{
				// Calculate, copy to storage, then return reference.
				Assert(i_vertex < 4);
				Assert(pqnwCurrent);

//TODO: Determine if this will even get called because we return 'bCurved' as false.
//      If it is called, return dummy vertex normal here.
				return CDir3<>();
			}

			//******************************************************************************************
			const CTexCoord& tcTexCoord(int i_vertex)
			{
				Assert(i_vertex < 4);
				return avtVertices[i_vertex].tcTex;
			}

			//******************************************************************************************
			int iShapePoint(int i_vertex)
			{
				Assert(pqnwCurrent);

				const CQuadVertexWater* pqvt = pqnwCurrent->pqvtGetVertex(i_vertex);
				return pqvt->iGetHandle();
			}

			//******************************************************************************************
			int iShapeVertex(int i_vertex)
			{
				// Should not be called, because pipeline was told we had unique vertices.
				return Verify(0);
			}

		protected:

			//******************************************************************************************
			//
			// Virtual overrides.
			//

			void Reset_()
			{
				Reset();
			}

			bool bNext_()
			{
				return bNext();
			}

			const CPlane& plPlane_()
			{
				return plPlane();
			}

			const CVector3<>& v3Point_()
			{
				return v3Point();
			}

			const CDir3<>& d3Normal_()
			{
				return d3Normal();
			}

			bool bCurved_()
			{
				return bCurved();
			}

			const CMatrix3<>& mx3ObjToTexture_()
			{
				return mx3ObjToTexture();
			}

			const CTexture* ptexTexture_()
			{
				return ptexTexture();
			}

			int iNumVertices_()
			{
				return iNumVertices();
			}

			const CVector3<>& v3Point_(int i_poly_vertex)
			{
				return v3Point(i_poly_vertex);
			}

			const CDir3<>& d3Normal_(int i_poly_vertex)
			{
				return d3Normal(i_poly_vertex);
			}

			const CTexCoord& tcTexCoord_(int i_poly_vertex)
			{
				return tcTexCoord(i_poly_vertex);
			}

			int iShapeVertex_(int i_poly_vertex)
			{
				return iShapeVertex(i_poly_vertex);
			}

			int iShapePoint_(int i_poly_vertex)
			{
				return iShapePoint(i_poly_vertex);
			}
		};

		//******************************************************************************************
		CShape::CPolyIterator* pPolyIterator(const CCamera* pcam = 0) const
		{
			return new CPolyIterator(*this, pcam);
		}

		friend class CPolyIterator;
	};
};

//#if VER_MULTI_RES_WATER
#endif

#endif
