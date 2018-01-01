/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of WaveletQuadTreeBaseTri.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTreeBaseTri.cpp                              $
 * 
 * 22    9/23/98 7:41p Mlange
 * Wavelet quad tree nodes now have a single pointer to the first descendant and a pointer to
 * the next sibling, instead of four pointers for each of the descendants.
 * 
 * 21    8/25/98 8:40p Rvande
 * 'class' keyword added to explicit template instantiations
 * 
 * 20    98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 19    98/08/10 12:50 Speter
 * Added CTriangle::bContains()
 * 
 * 18    7/21/98 7:10p Mlange
 * Added functions to detect whether a node subdivision would result in a triangulation that
 * conforms to the original data set.
 * 
 **********************************************************************************************/

#include "GblInc/Common.hpp"
#include "WaveletQuadTreeBaseTri.hpp"

#include "Lib/View/LineDraw.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "Lib/Sys/ConIO.hpp"

namespace NMultiResolution
{

//**********************************************************************************************
//
// NMultiResolution::CTriangleT implementation.
//

	//******************************************************************************************
	template<class TD, class TVT> CTriangleT<TD, TVT>::CTriangleT(TVT* pqvt_a, TVT* pqvt_b, TVT* pqvt_c, TD* ptri_link_after)
	{
		Assert(pqvt_a != 0 && pqvt_b != 0 && pqvt_c != 0 && pqvt_a != pqvt_b && pqvt_a != pqvt_c && pqvt_b != pqvt_c);

		apqvtVertices[0] = pqvt_a;
		apqvtVertices[1] = pqvt_b;
		apqvtVertices[2] = pqvt_c;

		ptriNext = ptri_link_after->ptriNext;
		ptri_link_after->ptriNext = static_cast<TD*>(this);
	}

	//******************************************************************************************
	template<class TD, class TVT> bool CTriangleT<TD, TVT>::bContains
	(
		TReal r_quad_x, TReal r_quad_y,
		TReal r_tolerance
	) const
	{
		return bLeftOfEdge(0, r_quad_x, r_quad_y, r_tolerance) &&
			   bLeftOfEdge(1, r_quad_x, r_quad_y, r_tolerance) && 
			   bLeftOfEdge(2, r_quad_x, r_quad_y, r_tolerance);
	}

//**********************************************************************************************
//
// NMultiResolution::CTriNodeInfoT implementation.
//

	//******************************************************************************************
	template<class TTRI, class TVT> CTriNodeInfoT<TTRI, TVT>::~CTriNodeInfoT()
	{
		// Delete the triangles in the linked list.
		TTRI* ptri_curr = ptriTriangles;

		while (ptri_curr != 0)
		{
			TTRI* ptri_next = ptri_curr->ptriGetNext();

			delete ptri_curr;

			ptri_curr = ptri_next;
		}
	}


	//******************************************************************************************
	template<class TTRI, class TVT> void CTriNodeInfoT<TTRI, TVT>::ModifiedVertices()
	{
		TTRI* ptri_curr = ptriTriangles;

		while (ptri_curr != 0)
		{
			ptri_curr->UpdatedVertices();
			ptri_curr = ptri_curr->ptriGetNext();
		}
	}



//**********************************************************************************************
//
// NMultiResolution::CQuadNodeBaseTriT<> implementation.
//

	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	TTRI* CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::ptriGetEnclosingTriangle(TReal r_quad_x, TReal r_quad_y)
	{
		Assert(bContains(r_quad_x, r_quad_y));

		TTRI* ptri_enc;
		TTRI* ptri_curr = ptriGetFirst();

		if (stState[estWAVELET_1_SIGNIFICANT])
		{
			bool b_left_of_0;
			bool b_left_of_2 = !ptri_curr->bLeftOfEdge(0, r_quad_x, r_quad_y);

			do
			{
				Assert(ptri_curr != 0);

				b_left_of_0 = !b_left_of_2;
				b_left_of_2 = ptri_curr->bLeftOfEdge(2, r_quad_x, r_quad_y);

				ptri_enc  = ptri_curr;
				ptri_curr = ptri_curr->ptriGetNext();
			}
			while (!b_left_of_0 || !b_left_of_2);
		}
		else
		{
			do
			{
				Assert(ptri_curr != 0);

				ptri_enc  = ptri_curr;
				ptri_curr = ptri_curr->ptriGetNext();
			}
			while (!ptri_enc->bLeftOfEdge(2, r_quad_x, r_quad_y));
		}

		Assert(ptri_enc->bContains(r_quad_x, r_quad_y, .05));

		return ptri_enc;
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	bool CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::bSubdivideConforms() const
	{
		if (tdData.cfGetMaxRegion() != cfZERO)
			return true;

		TD* ptqn_nn2 = ptqnGetNearestNeighbour(2);
		TD* ptqn_nn4 = ptqnGetNearestNeighbour(4);

		TD* ptqn_n2  = ptqn_nn2 ? ptqnGetNeighbour(2) : 0;
		TD* ptqn_n4  = ptqn_nn4 ? ptqnGetNeighbour(4) : 0;

		return bIsCrossContinous() &&
			   (ptqn_nn2 == 0 || ptqn_nn2->bIsCrossContinous()) && (ptqn_n2 == 0 || ptqn_n2->bIsContinousEdge(3)) &&
			   (ptqn_nn4 == 0 || ptqn_nn4->bIsCrossContinous()) && (ptqn_n4 == 0 || ptqn_n4->bIsContinousEdge(0));
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::SetWaveletStates(bool b_wvlt_0, bool b_wvlt_1, bool b_wvlt_2)
	{
		TState st_old = stState;

		stState[estWAVELET_0_SIGNIFICANT] = b_wvlt_0;
		stState[estWAVELET_1_SIGNIFICANT] = b_wvlt_1;
		stState[estWAVELET_2_SIGNIFICANT] = b_wvlt_2;

		if (st_old != stState)
			RemoveTrianglesBranch();

		if (st_old[estWAVELET_0_SIGNIFICANT] != stState[estWAVELET_0_SIGNIFICANT])
			RemoveNeighbouringTriangles(0);

		if (st_old[estWAVELET_2_SIGNIFICANT] != stState[estWAVELET_2_SIGNIFICANT])
			RemoveNeighbouringTriangles(6);
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::Triangulate()
	{
		Assert(stState[estLEAF][estLEAF_COMBINE]);
		Assert(ptqnGetTriangulate() == static_cast<TD*>(this));

		static CDArray<TVT*> adapqvt_edges[4] = {iMAX_TRANSFORM_INDICES / 2, iMAX_TRANSFORM_INDICES / 2,
		                                         iMAX_TRANSFORM_INDICES / 2, iMAX_TRANSFORM_INDICES / 2};

		adapqvt_edges[0].Reset();
		adapqvt_edges[1].Reset();
		adapqvt_edges[2].Reset();
		adapqvt_edges[3].Reset();

		adapqvt_edges[0] << pqvtGetVertex(0);
		adapqvt_edges[1] << pqvtGetVertex(1);
		adapqvt_edges[2] << pqvtGetVertex(2);
		adapqvt_edges[3] << pqvtGetVertex(3);

		if (ptqnGetNeighbour(0) != 0)
			ptqnGetNeighbour(0)->GetEdgeSubdiv(adapqvt_edges[0], 0);

		if (adapqvt_edges[0].uLen == 1 && stState[estWAVELET_0_SIGNIFICANT])
			adapqvt_edges[0] << ptqnGetFirstDescendant()->pqvtGetVertex(1);

		if (ptqnGetNeighbour(6) != 0)
			ptqnGetNeighbour(6)->GetEdgeSubdiv(adapqvt_edges[3], 3);

		if (adapqvt_edges[3].uLen == 1 && stState[estWAVELET_2_SIGNIFICANT])
			adapqvt_edges[3] << ptqnGetFirstDescendant()->pqvtGetVertex(3);

		if (ptqnGetNeighbour(2) != 0)
			ptqnGetNeighbour(2)->GetEdgeSigWavelets(adapqvt_edges[1], estWAVELET_2_SIGNIFICANT);

		if (ptqnGetNeighbour(4) != 0)
			ptqnGetNeighbour(4)->GetEdgeSigWavelets(adapqvt_edges[2], estWAVELET_0_SIGNIFICANT);

		adapqvt_edges[0] << pqvtGetVertex(1);
		adapqvt_edges[1] << pqvtGetVertex(2);
		adapqvt_edges[2] << pqvtGetVertex(3);
		adapqvt_edges[3] << pqvtGetVertex(0);


		TTINF* ptinf = ptinfGetTriangleInfo();

		Assert(ptinf->ptriTriangles == 0);

		TTRI tri_dummy_head;

		if (stState[estWAVELET_1_SIGNIFICANT])
		{
			TVT* pqvt_centre = ptqnGetFirstDescendant()->pqvtGetVertex(2);

			TTRI* ptri_list_end = &tri_dummy_head;

			for (int i_edge = 0; i_edge < 4; i_edge++)
				for (int i_vt = 0; i_vt < adapqvt_edges[i_edge].uLen - 1; i_vt++)
					ptri_list_end = new TTRI(pqvt_centre, adapqvt_edges[i_edge][i_vt], adapqvt_edges[i_edge][i_vt + 1], ptri_list_end);
		}
		else
		{
			if (!stState[estDISC_DIAGONAL_1_3])
			{
				TTRI* ptri_last = ptriTriangulateCorner(adapqvt_edges[0], adapqvt_edges[1], &tri_dummy_head);
								  ptriTriangulateCorner(adapqvt_edges[2], adapqvt_edges[3], ptri_last);
			}
			else
			{
				TTRI* ptri_last = ptriTriangulateCorner(adapqvt_edges[3], adapqvt_edges[0], &tri_dummy_head);
								  ptriTriangulateCorner(adapqvt_edges[1], adapqvt_edges[2], ptri_last);
			}
		}

		ptinf->ptriTriangles = tri_dummy_head.ptriGetNext();
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	TTRI* CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::ptriTriangulateCorner(const CDArray<TVT*>& sapqvt_left, const CDArray<TVT*>& sapqvt_right, TTRI* ptri_list_end) const
	{
		int i_vt_l = sapqvt_left.uLen - 2;
		int i_vt_r = 0;

		bool b_advance_r = true;

		do
		{
			if (b_advance_r)
			{
				ptri_list_end = new TTRI(sapqvt_left[i_vt_l], sapqvt_right[i_vt_r], sapqvt_right[i_vt_r + 1], ptri_list_end);
				i_vt_r++;
			}
			else
			{
				ptri_list_end = new TTRI(sapqvt_left[i_vt_l - 1], sapqvt_left[i_vt_l], sapqvt_right[i_vt_r], ptri_list_end);
				i_vt_l--;
			}


			b_advance_r = !b_advance_r;

			if (i_vt_r == sapqvt_right.uLen - 1)
				b_advance_r = false;
			else if (i_vt_l == 0)
				b_advance_r = true;
		}
		while (i_vt_l != 0 || i_vt_r != sapqvt_right.uLen - 1);

		return ptri_list_end;
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::GetEdgeSubdiv(CDArray<TVT*>& rsapqvt_verts, int i_edge) const
	{
		if (!stState[estLEAF][estLEAF_COMBINE])
		{
			ptqnGetDescendant(i_edge + 3)->GetEdgeSubdiv(rsapqvt_verts, i_edge);

			rsapqvt_verts << ptqnGetDescendant(i_edge + 2)->pqvtGetVertex(i_edge + 3);

			ptqnGetDescendant(i_edge + 2)->GetEdgeSubdiv(rsapqvt_verts, i_edge);
		}
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::GetEdgeSigWavelets(CDArray<TVT*>& rsapqvt_verts, EState est_wvlt) const
	{
		Assert(est_wvlt == estWAVELET_0_SIGNIFICANT || est_wvlt == estWAVELET_2_SIGNIFICANT);

		if (!stState[estLEAF][estLEAF_COMBINE] || stState[est_wvlt])
		{
			int i_wvlt = int(est_wvlt - estWAVELET_0_SIGNIFICANT);
			int i_dsc  = 1 - (i_wvlt >> 1);

			ptqnGetDescendant(i_dsc)->GetEdgeSigWavelets(rsapqvt_verts, est_wvlt);

			rsapqvt_verts << ptqnGetFirstDescendant()->pqvtGetVertex(i_wvlt + 1);

			ptqnGetDescendant(i_dsc - 1)->GetEdgeSigWavelets(rsapqvt_verts, est_wvlt);
		}
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	bool CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::bIsContinousEdge(int i_edge) const
	{
		Assert(i_edge == 0 || i_edge == 3);

		if (!bHasDescendants() || tdData.cfGetMaxRegion() == cfZERO)
			return true;
		else
		{
			if (ptqnGetDescendant(i_edge)->pqvtGetVertex(i_edge + 1)->cfWavelet() != cfZERO)
				return false;
			else
			{
				return ptqnGetDescendant(i_edge    )->bIsContinousEdge(i_edge) &&
				       ptqnGetDescendant(i_edge + 1)->bIsContinousEdge(i_edge);
			}
		}
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::RemoveTrianglesBranch()
	{
		if (!stState[estLEAF][estLEAF_COMBINE])
		{
			TD* ptqn_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				ptqn_dsc->RemoveTrianglesBranch();

				ptqn_dsc = ptqn_dsc->ptqnGetSibling();
			}
		}
		else
		{
			if (ptinfInfo != 0)
			{
				delete ptinfInfo;
				ptinfInfo = 0;
			}
		}
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::DrawWireframeBranch(CDraw& draw, CColour clr, bool b_draw_tri, bool b_draw_quad) const
	{
		if (bHasDescendants())
		{
			TD* ptqn_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				ptqn_dsc->DrawWireframeBranch(draw, clr, b_draw_tri, b_draw_quad);

				ptqn_dsc = ptqn_dsc->ptqnGetSibling();
			}
		}

		if (b_draw_tri && stState[estLEAF][estLEAF_COMBINE])
		{
			draw.Colour(clr);

			if (ptinfInfo != 0 && ptinfInfo->ptriTriangles != 0)
			{
				const TTRI* ptri_curr = ptinfInfo->ptriTriangles;

				do
				{
					// Call the custom SetColour function.
//					static_cast<const TD*>(this)->SetColour(draw, ptri_curr);

					// Draw each of the edges of the triangle.
					draw.MoveTo(ptri_curr->pqvtGetVertex(0)->iX(), ptri_curr->pqvtGetVertex(0)->iY());

					draw.LineTo(ptri_curr->pqvtGetVertex(1)->iX(), ptri_curr->pqvtGetVertex(1)->iY());
					draw.LineTo(ptri_curr->pqvtGetVertex(2)->iX(), ptri_curr->pqvtGetVertex(2)->iY());
					draw.LineTo(ptri_curr->pqvtGetVertex(0)->iX(), ptri_curr->pqvtGetVertex(0)->iY());

					ptri_curr = ptri_curr->ptriGetNext();
				}
				while (ptri_curr != 0);
			}
		}

		if (b_draw_quad)
			// Call the custom quad-tree function.
			static_cast<const TD*>(this)->DrawQuadNode(draw);
	}

#if VER_DEBUG
	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::CheckTriangulationBranch(const CQuadRootBaseTriT<TD, TVT, TTINF, TTRI>* pqnr_root)
	{
		if (!stState[estLEAF][estLEAF_COMBINE])
		{
			Assert(ptinfInfo == 0);

			TD* ptqn_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				ptqn_dsc->CheckTriangulationBranch(pqnr_root);

				ptqn_dsc = ptqn_dsc->ptqnGetSibling();
			}
		}
		else
		{
			// Ensure there is no ancestor with triangles too.
			TD* ptqn_search = ptqnGetParent();

			while (ptqn_search != 0)
			{
				Assert(ptqn_search->ptinfInfo == 0);
				ptqn_search = ptqn_search->ptqnGetParent();
			}

			// Ensure the descendants (if any) have no triangles.
			if (stState[estLEAF_COMBINE])
				for (int i_dsc = 0; i_dsc < 4; i_dsc++)
					Assert(ptqnGetDescendant(i_dsc)->ptinfInfo == 0);

			const TTRI* ptri_curr = ptriGetFirst();

			do
			{
				for (int i_curr_edge = 0; i_curr_edge < 3; i_curr_edge++)
				{
					const TVT* pqvt_edge_a = ptri_curr->pqvtGetVertex(i_curr_edge);
					const TVT* pqvt_edge_b = ptri_curr->pqvtGetVertex((i_curr_edge + 1) % 3);

					int i_num_matching_edges = iCountMatchingEdgesBranch(pqvt_edge_a, pqvt_edge_b);


					bool b_node_boundary_vert_a = pqvt_edge_a->iX() == iBaseX() || pqvt_edge_a->iX() == iBaseX() + iGetSize() ||
												  pqvt_edge_a->iY() == iBaseY() || pqvt_edge_a->iY() == iBaseY() + iGetSize();

					bool b_node_boundary_vert_b = pqvt_edge_b->iX() == iBaseX() || pqvt_edge_b->iX() == iBaseX() + iGetSize() ||
												  pqvt_edge_b->iY() == iBaseY() || pqvt_edge_b->iY() == iBaseY() + iGetSize();

					bool b_node_boundary_edge = b_node_boundary_vert_a && b_node_boundary_vert_b &&
												(pqvt_edge_a->iX() == pqvt_edge_b->iX() || pqvt_edge_a->iY() == pqvt_edge_b->iY());

					if (!b_node_boundary_edge)
					{
						Assert(i_num_matching_edges == 1);
					}
					else
					{
						Assert(i_num_matching_edges == 0);

						for (int i_n = 0; i_n < 8; i_n += 2)
						{
							TD* ptqn_neighbour = ptqnGetNearestNeighbour(i_n);

							if (ptqn_neighbour != 0)
								i_num_matching_edges += ptqn_neighbour->ptqnGetTriangulate()->iCountMatchingEdgesBranch(pqvt_edge_a, pqvt_edge_b);
						}

						bool b_tree_boundary_vert_a = pqvt_edge_a->iX() == pqnr_root->iBaseX() || pqvt_edge_a->iX() == pqnr_root->iBaseX() + pqnr_root->iGetSize() ||
													  pqvt_edge_a->iY() == pqnr_root->iBaseY() || pqvt_edge_a->iY() == pqnr_root->iBaseY() + pqnr_root->iGetSize();

						bool b_tree_boundary_vert_b = pqvt_edge_b->iX() == pqnr_root->iBaseX() || pqvt_edge_b->iX() == pqnr_root->iBaseX() + pqnr_root->iGetSize() ||
													  pqvt_edge_b->iY() == pqnr_root->iBaseY() || pqvt_edge_b->iY() == pqnr_root->iBaseY() + pqnr_root->iGetSize();

						if (b_tree_boundary_vert_a && b_tree_boundary_vert_b)
						{
							Assert(i_num_matching_edges == 0);
						}
						else
						{
							Assert(i_num_matching_edges == 1);
						}
					}
				}

				ptri_curr = ptri_curr->ptriGetNext();
			}
			while (ptri_curr != 0);
		}
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	int CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::iCountMatchingEdgesBranch(const TVT* pqvt_edge_a, const TVT* pqvt_edge_b)
	{
		int i_num_matching_edges = 0;

		if (!stState[estLEAF][estLEAF_COMBINE])
		{
			Assert(ptinfInfo == 0);

			TD* ptqn_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				i_num_matching_edges += ptqn_dsc->iCountMatchingEdgesBranch(pqvt_edge_a, pqvt_edge_b);

				ptqn_dsc = ptqn_dsc->ptqnGetSibling();
			}
		}
		else
		{
			const TTRI* ptri_cmp = ptriGetFirst();

			do
			{
				for (int i_cmp_edge = 0; i_cmp_edge < 3; i_cmp_edge++)
				{
					if (ptri_cmp->pqvtGetVertex(i_cmp_edge)           == pqvt_edge_b &&
						ptri_cmp->pqvtGetVertex((i_cmp_edge + 1) % 3) == pqvt_edge_a   )
					{
						i_num_matching_edges++;
					}
				}

				ptri_cmp = ptri_cmp->ptriGetNext();
			}
			while (ptri_cmp != 0);
		}

		return i_num_matching_edges;
	}
#endif

	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::Subdivide(const CQuadRootBaseRecalcT<TD, TVT>* pqnr_root)
	{
		if (ptqnGetParent() != 0)
			ptqnGetParent()->SetWaveletStates(true, true, true);

		CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>* pqtn = this;

		if (ptqnGetParent() != 0 && ptqnGetParent()->stState[estLEAF_COMBINE])
		{
			pqtn = ptqnGetParent();
			pqtn->RemoveTrianglesBranch();
		}

		pqtn->RemoveNeighbouringTriangles(2);
		pqtn->RemoveNeighbouringTriangles(4);

		CQuadNodeBaseRecalcT<TD, TVT>::Subdivide(pqnr_root);
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::Decimate()
	{
		SetWaveletStates(false, false, false);

		CQuadNodeBaseRecalcT<TD, TVT>::Decimate();

		CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>* pqtn = this;

		if (ptqnGetParent() != 0 && ptqnGetParent()->stState[estLEAF_COMBINE])
		{
			pqtn = ptqnGetParent();

			TD* ptqn_dsc = pqtn->ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				ptqn_dsc->RemoveTrianglesBranch();

				ptqn_dsc = ptqn_dsc->ptqnGetSibling();
			}
		}

		pqtn->RemoveNeighbouringTriangles(2);
		pqtn->RemoveNeighbouringTriangles(4);
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadNodeBaseTriT<TD, TVT, TTINF, TTRI>::ModifiedVerticesBranch()
	{
		if (!stState[estLEAF][estLEAF_COMBINE])
		{
			TD* ptqn_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				ptqn_dsc->ModifiedVerticesBranch();

				ptqn_dsc = ptqn_dsc->ptqnGetSibling();
			}
		}
		else
		{
			TD* ptqn_mod = ptqnGetTriangulate();

			if (ptqn_mod->ptinfInfo != 0)
				ptqn_mod->ptinfInfo->ModifiedVertices();
		}
	}


//**********************************************************************************************
//
// NMultiResolution::CQuadRootBaseTriT<> implementation.
//

	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	CQuadRootBaseTriT<TD, TVT, TTINF, TTRI>::CQuadRootBaseTriT(const CTransformedDataHeader* ptdh, TVT* pqvt_0, TVT* pqvt_1, TVT* pqvt_2, TVT* pqvt_3)
		: CQuadRootBaseRecalcT<TD, TVT>(ptdh, pqvt_0, pqvt_1, pqvt_2, pqvt_3)
	{
		ptinfInfo = 0;
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	TTRI* CQuadRootBaseTriT<TD, TVT, TTINF, TTRI>::ptriFindEnclosingTriangle(TReal r_quad_x, TReal r_quad_y) const
	{
		// Find leaf node that contains the point.
		TD* ptqn_tri = ptqnFindLeaf(iTrunc(r_quad_x), iTrunc(r_quad_y));

		// If no leaf node could be found then is a height query outside the bounds of the node.
		if (ptqn_tri == 0)
			return 0;

		return ptqn_tri->ptqnGetTriangulate()->ptriGetEnclosingTriangle(r_quad_x, r_quad_y);
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadRootBaseTriT<TD, TVT, TTINF, TTRI>::CheckTriangulation()
	{
		#if VER_DEBUG
			CheckTriangulationBranch(this);
		#endif
	}

	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	CRectangle<> CQuadRootBaseTriT<TD, TVT, TTINF, TTRI>::rcDrawWireframe(CRasterWin* pras_win, CColour clr, bool b_draw_tri, bool b_draw_quad, const CVector2<>& v2_quad_pos, TReal r_zoom) const
	{
		// Zoom in around position.
		TReal r_size_scaled = (iGetSize() * 1.01) / r_zoom;

		CRectangle<> rc_quad
		(
			v2_quad_pos.tX - r_size_scaled / 2,
			v2_quad_pos.tY + r_size_scaled / 2,
			 r_size_scaled,
			-r_size_scaled
		);

		CDraw draw(rptr_this(pras_win), rc_quad, true);

		DrawWireframeBranch(draw, clr, b_draw_tri, b_draw_quad);

		return rc_quad;
	}


	//******************************************************************************************
	template<class TD, class TVT, class TTINF, class TTRI>
	void CQuadRootBaseTriT<TD, TVT, TTINF, TTRI>::PrintStats(CConsoleBuffer& con) const
	{
		CQuadRootBaseRecalcT<TD, TVT>::PrintStats(con);

		con.Print
		(
			" Triangles    : %5d  %3dKB   %5d  %3dKB\n",
			TTRI::uNumAlloc(), (TTRI::uNumAlloc() * sizeof(TTRI) + 512) / 1024,
			TTRI::uMaxAlloc(), (TTRI::uMaxAlloc() * sizeof(TTRI) + 512) / 1024
		);

		con.Print
		(
			" Triangle info: %5d  %3dKB   %5d  %3dKB\n",
			TTINF::uNumAlloc(), (TTINF::uNumAlloc() * sizeof(TTINF) + 512) / 1024,
			TTINF::uMaxAlloc(), (TTINF::uMaxAlloc() * sizeof(TTINF) + 512) / 1024
		);
	}
};


//******************************************************************************************
//
// Explicit instantiation of used template types.
//

#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeQuery.hpp"

namespace NMultiResolution
{
	// Disable warning: template-class specialization 'xxx' is already instantiated.
	// If you instantiate these classes, VC4.2 complains.  If you don't, they don't link.  Lovely.
	#pragma warning(disable:4660)

	template class CTriangleT<CTriangleTIN,   CQuadVertexTIN>;
	template class CTriangleT<CTriangleQuery, CQuadVertexQuery>;

	template class CTriNodeInfoT<CTriangleTIN,   CQuadVertexTIN>;
	template class CTriNodeInfoT<CTriangleQuery, CQuadVertexQuery>;

	template class CQuadNodeBaseTriT<CQuadNodeTIN,   CQuadVertexTIN,   CTriNodeInfoTIN,   CTriangleTIN>;
	template class CQuadNodeBaseTriT<CQuadNodeQuery, CQuadVertexQuery, CTriNodeInfoQuery, CTriangleQuery>;

	template class CQuadRootBaseTriT<CQuadNodeTIN,   CQuadVertexTIN,   CTriNodeInfoTIN,   CTriangleTIN>;
	template class CQuadRootBaseTriT<CQuadNodeQuery, CQuadVertexQuery, CTriNodeInfoQuery, CTriangleQuery>;
};

