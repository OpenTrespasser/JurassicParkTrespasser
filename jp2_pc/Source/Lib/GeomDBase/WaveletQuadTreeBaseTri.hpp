/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Quad tree base classes for triangulating the synthesising wavelet transform.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTreeBaseTri.hpp                              $
 * 
 * 20    98/08/10 12:50 Speter
 * Added CTriangle::bContains()
 * 
 * 19    7/21/98 7:10p Mlange
 * Added functions to detect whether a node subdivision would result in a triangulation that
 * conforms to the original data set.
 * 
 * 18    5/05/98 4:31p Mlange
 * Reorganised data members among the quad tree class hierarchy.
 * 
 * 17    1/30/98 1:10p Mlange
 * Draw wireframe function now takes optional zoom parameter.
 * 
 * 16    1/26/98 8:12p Mlange
 * CTriangle<> vertex pointers are now non-const.
 * 
 * 15    1/21/98 5:27p Mlange
 * Added constants.
 * 
 * 14    1/20/98 2:42p Mlange
 * Added CQuadRootBaseTriT<>::CheckTriangulation().
 * 
 * 13    98/01/19 21:03 Speter
 * Changed DrawWireframe interface and functionality.
 * 
 * 12    1/15/98 11:36a Mlange
 * Added CTriNodeInfo type support.
 * 
 * 11    1/13/98 1:53p Mlange
 * Can now overide Triangulate() function. Renamed
 * CQuadNodeBaseRecalcT<>::UpdateDscScalingCoef() to RecalcDscScalingCoef(). Renamed
 * CQuadNodeBaseRecalcT<>::UpdatedVertices() to ModifiedVerticesBranch().
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_WAVELETQUADTREEBASETRI_HPP
#define HEADER_LIB_GEOMDBASE_WAVELETQUADTREEBASETRI_HPP

#include <pair.h>
#include "Lib/Std/BlockAllocator.hpp"
#include "Lib/Renderer/ClipRegion2D.hpp"
#include "Lib/GeomDBase/Plane.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeBaseRecalc.hpp"


#define dAVG_QUAD_NODE_TO_TRIANGLE_RATIO		1.1		// Empirically derived constants, used only to determine the
#define dAVG_QUAD_NODE_TO_TRIANGLE_INFO_RATIO	.33		// upper limit of allocated memory buffers sizes.


class CRasterWin;
class CColour;
class CDraw;

namespace NMultiResolution
{
	//**********************************************************************************************
	//
	template<class TD, class TVT> class CTriangleT
	//
	//
	// Prefix: tri
	//
	//**************************************
	{
	protected:
		TVT* apqvtVertices[3];	// The corner vertices of this triangle.

		TD* ptriNext;			// Next in quad node's linked list of triangles, null if this triangle
								// marks the end of the list.

		//******************************************************************************************
		//
		// Constructors and destructor.
		//

	public:
		CTriangleT()
			: ptriNext(0)
		{
		}


		CTriangleT(TVT* pqvt_a, TVT* pqvt_b, TVT* pqvt_c, TD* pptri_link_after);


		//******************************************************************************************
		//
		// Member functions.
		//

		//******************************************************************************************
		//
		TD* ptriGetNext() const
		//
		// Returns:
		//		Pointer to the next triangle in the linked list, null if this is the last triangle.
		//
		//**************************************
		{
			return ptriNext;
		}


		//******************************************************************************************
		//
		void UpdatedVertices()
		//
		//
		//**************************************
		{
		}


		//******************************************************************************************
		//
		TVT* pqvtGetVertex
		(
			uint u_vt_num
		) const
		//
		//
		//**************************************
		{
			Assert(u_vt_num < 3);
			return apqvtVertices[u_vt_num];
		}


		//******************************************************************************************
		//
		bool bLeftOfEdge
		(
			uint u_edge,
			TReal r_quad_x, TReal r_quad_y,
			TReal r_tolerance = 0.01
		) const
		//
		//
		//**************************************
		{
			Assert(u_edge < 3);

			uint u_vt_a = u_edge;
			uint u_vt_b = u_edge != 2 ? u_edge + 1 : 0;

			const TVT* pqvt_a = pqvtGetVertex(u_vt_a);
			const TVT* pqvt_b = pqvtGetVertex(u_vt_b);

			// Adapted from the FAQ at: http://www.cis.ohio-state.edu/hypertext/faq/usenet/graphics/algorithms-faq/faq.html.
			float f_d = (r_quad_x - pqvt_a->iX()) * (pqvt_b->iY() - pqvt_a->iY()) - (r_quad_y - pqvt_a->iY()) * (pqvt_b->iX() - pqvt_a->iX());

			return CIntFloat(f_d - r_tolerance).bSign();
		}

		//******************************************************************************************
		//
		bool bContains
		(
			TReal r_quad_x, TReal r_quad_y,
			TReal r_tolerance = 0.01
		) const;
		//
		// Returns:
		//		Whether the triangle contains the point to within r_tolerance.
		//
		//**************************************
	};



	//**********************************************************************************************
	//
	template<class TTRI, class TVT> class CTriNodeInfoT
	//
	// Prefix: tinf
	//
	//**************************************
	{
	public:
		TTRI* ptriTriangles;

		//******************************************************************************************
		//
		// Constructors and destructor.
		//
	public:
		CTriNodeInfoT()
			: ptriTriangles(0)
		{
		}


		~CTriNodeInfoT();


		//******************************************************************************************
		//
		void ModifiedVertices();
		//
		// Notes:
		//		This function can be overiden by the derived class.
		//
		//**************************************
	};



	template<class TD, class TVT, class TTINF, class TTRI> class CQuadRootBaseTriT;

	//**********************************************************************************************
	//
	template<class TD, class TVT, class TTINF, class TTRI> class CQuadNodeBaseTriT : public CQuadNodeBaseRecalcT<TD, TVT>
	//
	//
	// Prefix: qnt
	//
	//**************************************
	{
	protected:
		TTINF* ptinfInfo;


		//******************************************************************************************
		//
		// Constructors and destructor.
		//
		CQuadNodeBaseTriT()
		{
		}

		// Construct a new descendant.
		CQuadNodeBaseTriT
		(
			const CQuadRootBaseRecalcT<TD, TVT>* pqnr_root,
			const TD* ptqn_parent, int i_relation, TD* aptqn_subdiv_neighbours[4], TVT* apqvt_dsc[5]
		)
			: CQuadNodeBaseRecalcT<TD, TVT>(pqnr_root, ptqn_parent, i_relation, aptqn_subdiv_neighbours, apqvt_dsc),
			  ptinfInfo(0)
		{
		}

		~CQuadNodeBaseTriT()
		{
			Assert(ptinfInfo == 0);
		}

		//******************************************************************************************
		//
		// Member functions.
		//

	public:
		//******************************************************************************************
		//
		TD* ptqnGetTriangulate()
		//
		// Returns:
		//
		//**************************************
		{
			if (ptqnGetParent() != 0 && ptqnGetParent()->stState[estLEAF_COMBINE])
				return ptqnGetParent();
			else
				return static_cast<TD*>(this);
		}


		//******************************************************************************************
		//
		TTRI* ptriGetFirst()
		//
		// Returns:
		//		Pointer to the first triangle in the linked list.
		//
		// Notes:
		//		Triangulates this node if it currently has no triangles.
		//
		//**************************************
		{
			Assert(stState[estLEAF][estLEAF_COMBINE]);
			Assert(ptqnGetTriangulate() == static_cast<TD*>(this));

			if (ptinfGetTriangleInfo()->ptriTriangles == 0)
				static_cast<TD*>(this)->Triangulate();

			return ptinfGetTriangleInfo()->ptriTriangles;
		}


		//******************************************************************************************
		//
		TTINF* ptinfGetTriangleInfo()
		//
		// Returns:
		//		Pointer to the triangulation info of this node.
		//
		//**************************************
		{
			Assert(stState[estLEAF][estLEAF_COMBINE]);
			Assert(ptqnGetTriangulate() == static_cast<TD*>(this));

			if (ptinfInfo == 0)
				ptinfInfo = new TTINF;

			return ptinfInfo;
		}


		//******************************************************************************************
		//
		TTRI* ptriGetEnclosingTriangle
		(
			TReal r_quad_x, TReal r_quad_y		// Quad-space coordinates to query at.
		);
		//
		// Returns:
		//
		//******************************


		//******************************************************************************************
		//
		bool bSubdivideConforms() const;
		//
		// Returns:
		//
		//******************************


	protected:
		//******************************************************************************************
		//
		void SetWaveletStates
		(
			bool b_wvlt_0, bool b_wvlt_1, bool b_wvlt_2
		);
		//
		//
		//
		//**************************************


		//******************************************************************************************
		//
		void Triangulate();
		//
		// Notes:
		//		This function can be overiden by the derived class.
		//
		//**************************************


		//******************************************************************************************
		//
		TTRI* ptriTriangulateCorner
		(
			const CDArray<TVT*>& sapqvt_left,
			const CDArray<TVT*>& sapqvt_right,
			TTRI* ptri_list_end
		) const;
		//
		//
		//
		//**************************************


		//******************************************************************************************
		//
		void GetEdgeSubdiv
		(
			CDArray<TVT*>& rsapqvt_verts,
			int i_edge
		) const;
		//
		//
		//
		//**************************************


		//******************************************************************************************
		//
		void GetEdgeSigWavelets
		(
			CDArray<TVT*>& rsapqvt_verts,
			EState est_wvlt
		) const;
		//
		//
		//
		//**************************************


		//******************************************************************************************
		//
		bool bIsContinousEdge
		(
			int i_edge		// Edge to check, may be 0 or 3.
		) const;
		//
		// Returns:
		//		'true' if the edge is continous, e.g. has no 'kinks'.
		//
		//******************************


		//******************************************************************************************
		//
		bool bIsCrossContinous() const
		//
		// Returns:
		//
		//******************************
		{
			if (!ptqnGetParent()->stState[estWAVELET_1_SIGNIFICANT] &&
				 ptqnGetParent()->stState[estWAVELET_0_SIGNIFICANT][estWAVELET_2_SIGNIFICANT])
				return false;
			else
				return true;
		}


		//******************************************************************************************
		//
		void RemoveTrianglesBranch();
		//
		//
		//
		//**************************************


		//******************************************************************************************
		//
		void RemoveNeighbouringTriangles
		(
			int i_n
		) const
		//
		//
		//
		//**************************************
		{
			Assert((i_n & 1) == 0);

			TD* ptqn_neighbour = ptqnGetNearestNeighbour(i_n);

			if (ptqn_neighbour != 0)
				ptqn_neighbour->ptqnGetTriangulate()->RemoveTrianglesBranch();
		}


		//******************************************************************************************
		//
		void DrawWireframeBranch
		(
			CDraw& draw,
			CColour clr,
			bool b_draw_tri,
			bool b_draw_quad
		) const;
		//
		//******************************


		//******************************************************************************************
		//
		void DrawQuadNode
		(
			CDraw& draw
		) const
		//
		//******************************
		{
		}


		//******************************************************************************************
		//
		void SetColour
		(
			CDraw& draw,
			const TTRI* pttri			// Triangle contained by this.
		) const
		//
		// Sets a colour appropriate to this triangle, for debug drawing.
		//
		//******************************
		{
			// Base version does nothing, using default colour.
		}



#if VER_DEBUG
		//******************************************************************************************
		//
		void CheckTriangulationBranch
		(
			const CQuadRootBaseTriT<TD, TVT, TTINF, TTRI>* pqnr_root
		);
		//
		//
		//******************************


		//******************************************************************************************
		//
		int iCountMatchingEdgesBranch
		(
			const TVT* pqvt_edge_a,
			const TVT* pqvt_edge_b
		);
		//
		//
		//******************************
#endif


		//******************************************************************************************
		//
		// Overides.
		//

	public:
		//******************************************************************************************
		void Subdivide(const CQuadRootBaseRecalcT<TD, TVT>* pqnr_root);

		//******************************************************************************************
		void Decimate();

		//******************************************************************************************
		void ModifiedVerticesBranch();
	};


	//**********************************************************************************************
	//
	template<class TD, class TVT, class TTINF, class TTRI> class CQuadRootBaseTriT : public CQuadRootBaseRecalcT<TD, TVT>
	//
	//
	// Prefix: qnrt
	//
	//**************************************
	{
		friend class CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>;

	protected:
		//******************************************************************************************
		//
		// Constructors and destructor.
		//

		CQuadRootBaseTriT(const CTransformedDataHeader* ptdh, TVT* pqvt_0, TVT* pqvt_1, TVT* pqvt_2, TVT* pqvt_3);


		//******************************************************************************************
		//
		// Member functions.
		//

	public:
		//******************************************************************************************
		//
		TTRI* ptriFindEnclosingTriangle
		(
			TReal r_quad_x, TReal r_quad_y		// Quad-space coordinates to query at.
		) const;
		//
		// Find the enclosing triangle of the given point in the quad tree.
		//
		// Returns:
		//		Uses current quad-tree refinement, does not refine further.
		//		Returns 0 if the point is not contained in the tree.
		//
		//******************************


		//******************************************************************************************
		//
		CRectangle<> rcDrawWireframe
		(
			CRasterWin* pras_win,
			CColour clr,
			bool b_draw_tri,
			bool b_draw_quad,
			const CVector2<>& v2_quad_pos,
			TReal r_zoom = 1
		) const;
		//
		// Draw a wireframe representation of the current triangulation.
		//
		// Returns:
		//		The rectangle used to construct a CDraw class.
		//
		// Notes:
		//		Draws existing triangles only, e.g. does not trianglulate nodes that currently have
		//		no triangles.
		//
		//******************************


		//******************************************************************************************
		//
		void CheckTriangulation();
		//
		// Check the triangulation of the wavelet data and assert in case of an error.
		//
		// Notes:
		//		This function does nothing if not a debug build.
		//
		//		Calling this function will cause the entire quad tree to be triangulated.
		//
		//******************************


		//******************************************************************************************
		//
		// Overides.
		//

		//******************************************************************************************
		void PrintStats(CConsoleBuffer& con) const;
	};
};


#endif
