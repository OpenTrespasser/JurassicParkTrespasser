/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of WaveletQuadTreeBase.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTreeBase.cpp                                 $
 * 
 * 32    9/23/98 7:41p Mlange
 * Wavelet quad tree nodes now have a single pointer to the first descendant and a pointer to
 * the next sibling, instead of four pointers for each of the descendants.
 * 
 * 31    8/25/98 8:39p Rvande
 * Loop variables re-scoped; 'class' keyword added to explicit template instantiations
 * 
 * 30    98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 29    5/15/98 6:24p Mlange
 * Instantiation of water quad tree on build version switch.
 * 
 * 28    5/05/98 4:31p Mlange
 * Reorganised data members among the quad tree class hierarchy.
 * 
 **********************************************************************************************/

#include "GblInc/Common.hpp"
#include "WaveletQuadTreeBase.hpp"

#include "Lib/Sys/ConIO.hpp"


namespace NMultiResolution
{

//**********************************************************************************************
//
// NMultiResolution::CQuadNodeBaseT<> implementation.
//

	//******************************************************************************************
	template<class TD, class TVT> CQuadNodeBaseT<TD, TVT>::CQuadNodeBaseT
	(
		const TD* ptqn_parent, int i_relation, TD* aptqn_subdiv_neighbours[4], TVT* apqvt_dsc[5]
	)
		: ptqnDescendant(0), ptqnSibling(0)
	{
		Assert(ptqn_parent != 0);
		Assert(i_relation == (i_relation & 3));

		ptqnParent = const_cast<TD*>(ptqn_parent);

		//
		// Set the vertex pointers for this node. Each descendant shares one of its corner vertices with the parent;
		// the remaining vertices are shared with the siblings. The parent - child relation is used to obtain the
		// index for each of the vertices.
		//
		csapqvtVertices[i_relation    ] = ptqn_parent->pqvtGetVertex(i_relation);
		csapqvtVertices[i_relation + 1] = apqvt_dsc[i_relation];
		csapqvtVertices[i_relation + 2] = apqvt_dsc[4];
		csapqvtVertices[i_relation + 3] = apqvt_dsc[(i_relation + 3) & 3];


		//
		// Set the status flags.
		//
		// First, set the boundary status flags for this descendant. The boundary status of a descendant is
		// determined by the parent's boundary condition and the descendant's (spatial) relation to the parent.
		//

		// Determine if this node's minimum x and y extents coincide with the parent's minimum x and y extents.
		// Then construct a mask to filter the boundary conditions of the parent that do not apply to this node.
		TState st_boundary_mask;
		st_boundary_mask[estBOUNDARY_MIN_X] = ((i_relation - 1) & 3) >= 2;
		st_boundary_mask[estBOUNDARY_MIN_Y] = i_relation < 2;
		st_boundary_mask[estBOUNDARY_MAX_X] = !st_boundary_mask[estBOUNDARY_MIN_X];
		st_boundary_mask[estBOUNDARY_MAX_Y] = !st_boundary_mask[estBOUNDARY_MIN_Y];

		stState = ptqn_parent->stState & st_boundary_mask;

		// Set the neighbouring flags. Each descendant is always a neighbour to its siblings, so we simply set these flags.
		stState[estGetNeighbourBit(i_relation * 2 + 2)][estGetNeighbourBit(i_relation * 2 + 4)] = true;

		// Set the neighbouring flags for the nodes other than the siblings.
		stState[estGetNeighbourBit(i_relation * 2 + 6)] = aptqn_subdiv_neighbours[(i_relation + 3) & 3] != 0;
		stState[estGetNeighbourBit(i_relation * 2    )] = aptqn_subdiv_neighbours[i_relation          ] != 0;

		stState[estDISC_DIAGONAL_1_3] = (i_relation & 1) != 0;

		// This is a leaf node.
		stState[estLEAF_COMBINE] = false;
		stState[estLEAF]         = true;

		//
		// Update the status flags of the neighbouring nodes (other than the siblings), if any.
		//
		if (aptqn_subdiv_neighbours[(i_relation + 3) & 3] != 0)
		{
			TD* ptqn_neighbour = aptqn_subdiv_neighbours[(i_relation + 3) & 3]->ptqnGetDescendant(i_relation + 1);

			// Check the neighbour's status bits.
			Assert(!ptqn_neighbour->stState[estGetNeighbourBit(i_relation * 2 + 6 + 4)]);

			ptqn_neighbour->stState[estGetNeighbourBit(i_relation * 2 + 6 + 4)] = true;
		}


		if (aptqn_subdiv_neighbours[i_relation] != 0)
		{
			TD* ptqn_neighbour = aptqn_subdiv_neighbours[i_relation]->ptqnGetDescendant(i_relation - 1);

			// Check the neighbour's status bits.
			Assert(!ptqn_neighbour->stState[estGetNeighbourBit(i_relation * 2 + 4)]);

			ptqn_neighbour->stState[estGetNeighbourBit(i_relation * 2 + 4)] = true;
		}

	}



	//******************************************************************************************
	template<class TD, class TVT> TD* CQuadNodeBaseT<TD, TVT>::ptqnGetNearestNeighbour(int i_n) const
	{
		// Ensure the enumerated constants match the index calculation.
		Assert(int(estBOUNDARY_MAX_X) - int(estBOUNDARY_MIN_Y) == 1 &&
		       int(estBOUNDARY_MAX_Y) - int(estBOUNDARY_MIN_Y) == 2 && int(estBOUNDARY_MIN_X) - int(estBOUNDARY_MIN_Y) == 3);

		// Avoid redundant searches for neighbours that would lie outside the bounds of the quad tree.
		if (stState[EState((((i_n + 1) >> 1) & 3) + estBOUNDARY_MIN_Y)][EState(((i_n >> 1) & 3) + estBOUNDARY_MIN_Y)])
			return 0;

		//
		// Calculate the base coordinates of the requested neighbour.
		//
		int i_base_x = iBaseX();

		if (((i_n - 5) & 7) <= 2)
			i_base_x -= iGetSize();
		else if (((i_n - 1) & 7) <= 2)
			i_base_x += iGetSize();

		int i_base_y = iBaseY();

		if (((i_n - 7) & 7) <= 2)
			i_base_y -= iGetSize();
		else if (((i_n - 3) & 7) <= 2)
			i_base_y += iGetSize();


		//
		// Starting at this node, search up the quad tree until a parent is found that envelopes both this
		// node and the requested neighbour.
		//
		TD* ptqn_search = ptqnGetParent();

		Assert(ptqn_search != 0);

		while (!ptqn_search->bContains(i_base_x, i_base_y))
		{
			ptqn_search = ptqn_search->ptqnGetParent();

			Assert(ptqn_search != 0);
		}


		//
		// From this 'common' parent we recurse down the tree, following the descendants that envelop the
		// requested neighbour.
		//
		do
		{
			Assert(ptqn_search->iGetSize() > iGetSize());
			Assert(ptqn_search->bContains(i_base_x, i_base_y))

			// Determine which descendant contains the requested neighbour.
			bool b_right = i_base_x >= ptqn_search->ptqnGetFirstDescendant()->pqvtGetVertex(2)->iX();
			bool b_up    = i_base_y >= ptqn_search->ptqnGetFirstDescendant()->pqvtGetVertex(2)->iY();

			int i_dsc_base = (int(b_up) << 1) | (int(b_right) ^ int(b_up));

			ptqn_search = ptqn_search->ptqnGetDescendant(i_dsc_base);
		}
		// We follow the tree until we have found the neighbour or until we've reached a leaf node.
		while (ptqn_search->iGetSize() != iGetSize() && ptqn_search->bHasDescendants());


		#if VER_DEBUG
			// If we have found the actual requested neighbour (and not just a nearest neighbour) we can
			// perform several assert checks.
			if (ptqn_search->iGetSize() == iGetSize())
			{
				i_n &= 7;

				// Make sure the found neighbour's base coordinates are an exact match.
				Assert(i_base_x == ptqn_search->iBaseX() && i_base_y == ptqn_search->iBaseY());

				// If a diagonal neighbour was requested, check if the single shared vertex is valid. Otherwise,
				// check if the two shared vertices are valid.
				Assert( (i_n & 1) == 0 || pqvtGetVertex((i_n + 1) / 2) == ptqn_search->pqvtGetVertex((i_n + 1) / 2 + 2) );
				Assert( (i_n & 1) != 0 || pqvtGetVertex(i_n / 2)       == ptqn_search->pqvtGetVertex(i_n / 2 + 3) &&
		                                  pqvtGetVertex(i_n / 2 + 1)   == ptqn_search->pqvtGetVertex(i_n / 2 + 2)    );

				// Ensure the neighbour status bits are valid.
				Assert( (i_n & 1) != 0 || (ptqn_search->stState[estGetNeighbourBit(i_n + 4)] != 0) ==
				                                       (stState[estGetNeighbourBit(i_n)    ] != 0)   );
			}
		#endif

		return ptqn_search;
	}


	//******************************************************************************************
	template<class TD, class TVT> void CQuadNodeBaseT<TD, TVT>::AccumulateWavelets(CCoef acf_wvlt[3], CCoef acf_corner[4]) const
	{
		// Accumulate the wavelet coeficients of this node as dictated by the definition of the 0-disc
		// wavelet.
		acf_corner[0] = acf_wvlt[0] + acf_wvlt[2];
		acf_corner[1] = acf_wvlt[0];
		acf_corner[2] = cfZERO;
		acf_corner[3] = acf_wvlt[2];

		if (stState[estDISC_DIAGONAL_1_3])
		{
			acf_corner[1] += acf_wvlt[1];
			acf_corner[3] += acf_wvlt[1];
		}
		else
		{
			acf_corner[0] += acf_wvlt[1];
			acf_corner[2] += acf_wvlt[1];
		}


		// The wavelet data is reflected at the borders. 
		if (stState[estBOUNDARY_MIN_X])
		{
			acf_corner[0] += acf_wvlt[0];

			if (stState[estDISC_DIAGONAL_1_3])
				acf_corner[3] += acf_wvlt[1];
			else
				acf_corner[0] += acf_wvlt[1];
		}

		if (stState[estBOUNDARY_MIN_Y])
		{
			acf_corner[0] += acf_wvlt[2];

			if (stState[estDISC_DIAGONAL_1_3])
				acf_corner[1] += acf_wvlt[1];
			else
				acf_corner[0] += acf_wvlt[1];
		}

		if (stState[estBOUNDARY_MIN_X] && stState[estBOUNDARY_MIN_Y])
			acf_corner[0] += acf_wvlt[1];
	}



	//******************************************************************************************
	template<class TD, class TVT> void CQuadNodeBaseT<TD, TVT>::AllocateDscVertices(TVT* apqvt_dsc[5], TD* aptqn_subdiv_neighbours[4]) const
	{
		// Make sure this node has not been subdivided already.
		Assert(!bHasDescendants() && iGetSize() > 1);

		// We iterate through the four the descendant numbers, and at each step we consider the
		// pair formed by the current and next descendant.
		for (int i_dsc = 0; i_dsc < 4; i_dsc++)
		{
			TD* ptqn_neighbour = ptqnGetNeighbour(i_dsc * 2);

			if (ptqn_neighbour != 0 && ptqn_neighbour->bHasDescendants())
			{
				// The neighbour exists and has descendants, so the required vertex already exists.
				apqvt_dsc[i_dsc] = ptqn_neighbour->ptqnGetDescendant(i_dsc - 1)->pqvtGetVertex(i_dsc + 2);

				aptqn_subdiv_neighbours[i_dsc] = ptqn_neighbour;
			}
			else
			{
				// Allocate a new vertex.
				apqvt_dsc[i_dsc] = new TVT(pqvtGetVertex(i_dsc), pqvtGetVertex(i_dsc + 1));

				aptqn_subdiv_neighbours[i_dsc] = 0;
			}
		}

		// Allocate the fifth new vertex at the midpoint of this node.
		if (stState[estDISC_DIAGONAL_1_3])
			apqvt_dsc[4] = new TVT(pqvtGetVertex(1), pqvtGetVertex(3));
		else
			apqvt_dsc[4] = new TVT(pqvtGetVertex(0), pqvtGetVertex(2));
	}


	//******************************************************************************************
	template<class TD, class TVT> void CQuadNodeBaseT<TD, TVT>::InitSubdivide(TD* ptqn_dsc_0, TD* ptqn_dsc_1, TD* ptqn_dsc_2, TD* ptqn_dsc_3)
	{
		// Make sure this node has not been subdivided already.
		Assert(!bHasDescendants() && iGetSize() > 1);
		Assert(ptqn_dsc_0 != 0 && ptqn_dsc_1 != 0 && ptqn_dsc_2 != 0 && ptqn_dsc_3 != 0);

		ptqnDescendant = ptqn_dsc_0;

		ptqn_dsc_0->SetSibling(ptqn_dsc_1);
		ptqn_dsc_1->SetSibling(ptqn_dsc_2);
		ptqn_dsc_2->SetSibling(ptqn_dsc_3);
		ptqn_dsc_3->SetSibling(ptqn_dsc_0);

		// This becomes a leaf combine node.
		stState[estLEAF_COMBINE] = true;
		stState[estLEAF]         = false;

		// The parent (if one exists) can no longer be a leaf combine node.
		if (ptqnGetParent() != 0)
		{
			Assert(!ptqnGetParent()->stState[estLEAF]);
			ptqnGetParent()->stState[estLEAF_COMBINE] = false;
		}
	}


	//******************************************************************************************
	template<class TD, class TVT> void CQuadNodeBaseT<TD, TVT>::Decimate()
	{
		Assert(stState[estLEAF_COMBINE]);
		Assert(bHasDescendants() && !ptqnGetDescendant(0)->bHasDescendants() && !ptqnGetDescendant(1)->bHasDescendants() &&
		                            !ptqnGetDescendant(2)->bHasDescendants() && !ptqnGetDescendant(3)->bHasDescendants()   );

		//
		// First delete the descendant nodes. At the same time we save references to the vertices along
		// the mid points of the edges so we can delete these later.
		//
		TVT* pqvt_mid_vertex = ptqnGetFirstDescendant()->pqvtGetVertex(2);

		TVT* apqvt_edge_vertices[4];

		TD* ptqn_curr = ptqnGetFirstDescendant();

		int i_dsc;
		for (i_dsc = 0; i_dsc < 4; i_dsc++)
		{
			apqvt_edge_vertices[i_dsc] = ptqn_curr->pqvtGetVertex(i_dsc + 1);

			TD* ptqn_prev = ptqn_curr;
			ptqn_curr = ptqn_curr->ptqnGetSibling();

			delete ptqn_prev;
		}

		ptqnDescendant = 0;

		//
		// Now update the (neighbouring) status flags of the neighbours' descendants if they exist. Also
		// delete the orphaned vertices.
		//
		for (i_dsc = 0; i_dsc < 4; i_dsc++)
		{
			int i_neighbour = i_dsc * 2;

			TD* ptqn_neighbour = ptqnGetNeighbour(i_neighbour);

			if (ptqn_neighbour != 0 && ptqn_neighbour->bHasDescendants())
			{
				Assert(ptqn_neighbour->ptqnGetDescendant(i_dsc - 1)->stState[estGetNeighbourBit(i_neighbour + 4)]);
				Assert(ptqn_neighbour->ptqnGetDescendant(i_dsc - 2)->stState[estGetNeighbourBit(i_neighbour + 4)]);

				ptqn_neighbour->ptqnGetDescendant(i_dsc - 1)->stState[estGetNeighbourBit(i_neighbour + 4)] = false;
				ptqn_neighbour->ptqnGetDescendant(i_dsc - 2)->stState[estGetNeighbourBit(i_neighbour + 4)] = false;
			}
			else
				// This vertex is no longer used by any nodes, so delete it.
				delete apqvt_edge_vertices[i_dsc];
		}

		delete pqvt_mid_vertex;


		// This is now a leaf node.
		stState[estLEAF_COMBINE] = false;
		stState[estLEAF]         = true;

		// The parent (if one exists) may have become a leaf combine node.
		if (ptqnGetParent() != 0)
		{
			Assert(!ptqnGetParent()->stState[estLEAF]);

			// Check the parent's descendants to see if any have descendants.
			ptqnGetParent()->stState[estLEAF_COMBINE] = true;

			TD* ptqn_parent_dsc = ptqnGetParent()->ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				if (ptqn_parent_dsc->bHasDescendants())
				{
					ptqnGetParent()->stState[estLEAF_COMBINE] = false;

					break;
				}

				ptqn_parent_dsc = ptqn_parent_dsc->ptqnGetSibling();
			}
		}
	}



//**********************************************************************************************
//
// NMultiResolution::CQuadRootBaseT<> implementation.
//

	//******************************************************************************************
	template<class TD, class TVT> CQuadRootBaseT<TD, TVT>::CQuadRootBaseT(TVT* pqvt_0, TVT* pqvt_1, TVT* pqvt_2, TVT* pqvt_3)
	{
		// The root node is always significant and defines the boundary of the quad tree.
		stState[estSIGNIFICANT][estBOUNDARY_MIN_X][estBOUNDARY_MIN_Y][estBOUNDARY_MAX_X][estBOUNDARY_MAX_Y] = true;

		// This is a leaf node.
		stState[estLEAF_COMBINE] = false;
		stState[estLEAF]         = true;

		// Initialise the corner vertex pointers.
		csapqvtVertices[0] = pqvt_0;
		csapqvtVertices[1] = pqvt_1;
		csapqvtVertices[2] = pqvt_2;
		csapqvtVertices[3] = pqvt_3;
 	}


	//******************************************************************************************
	template<class TD, class TVT> CQuadRootBaseT<TD, TVT>::~CQuadRootBaseT()
	{
		DecimateBranch();

		stState[estSIGNIFICANT] = false;

		for (int i_vt = 0; i_vt < 4; i_vt++)
			delete csapqvtVertices[i_vt];
	}


	//******************************************************************************************
	template<class TD, class TVT> TD* CQuadRootBaseT<TD, TVT>::ptqnFindLeaf(int i_x, int i_y) const
	{
		// Determine if the tree contains the point.
		if (!bContains(i_x, i_y))
			return 0;

		const TD* ptqn_search = static_cast<const TD*>(this);

		// Have we reached a leaf node?
		while (ptqn_search->bHasDescendants())
		{
			// Find the descendant that contains point.
			ptqn_search = ptqn_search->ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 3; i_dsc++)
			{
				if (ptqn_search->bContains(i_x, i_y))
					break;

				ptqn_search = ptqn_search->ptqnGetSibling();
			}
		}

		// Make sure we found the right node.
		Assert(ptqn_search->bContains(i_x, i_y));

		return const_cast<TD*>(ptqn_search);
	}


	//******************************************************************************************
	template<class TD, class TVT> void CQuadRootBaseT<TD, TVT>::PrintStats(CConsoleBuffer& con) const
	{
		con.Print
		(
			" Nodes        : %5d  %3dKB   %5d  %3dKB\n",
			TD::uNumAlloc(), (TD::uNumAlloc() * sizeof(TD) + 512) / 1024,
			TD::uMaxAlloc(), (TD::uMaxAlloc() * sizeof(TD) + 512) / 1024
		);

		con.Print
		(
			" Vertices     : %5d  %3dKB   %5d  %3dKB\n",
			TVT::uNumAlloc(), (TVT::uNumAlloc() * sizeof(TVT) + 512) / 1024,
			TVT::uMaxAlloc(), (TVT::uMaxAlloc() * sizeof(TVT) + 512) / 1024
		);
	}
};


//******************************************************************************************
//
// Explicit instantiation of used template types.
//
#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeQuery.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeTForm.hpp"

#if VER_MULTI_RES_WATER
	#include "Lib/GeomDBase/WaterQuadTree.hpp"
#endif

namespace NMultiResolution
{
	// Disable warning: template-class specialization 'xxx' is already instantiated.
	// If you instantiate these classes, VC4.2 complains.  If you don't, they don't link.  Lovely.
	#pragma warning(disable:4660)

	template class CQuadNodeBaseT<CQuadNodeTIN,   CQuadVertexTIN>;
	template class CQuadNodeBaseT<CQuadNodeQuery, CQuadVertexQuery>;
	template class CQuadNodeBaseT<CQuadNodeTForm, CQuadVertexTForm>;

	template class CQuadRootBaseT<CQuadNodeTIN,   CQuadVertexTIN>;
	template class CQuadRootBaseT<CQuadNodeQuery, CQuadVertexQuery>;
	template class CQuadRootBaseT<CQuadNodeTForm, CQuadVertexTForm>;

#if VER_MULTI_RES_WATER
	template class CQuadNodeBaseT<CQuadNodeWater, CQuadVertexWater>;
	template class CQuadRootBaseT<CQuadNodeWater, CQuadVertexWater>;
#endif
};
