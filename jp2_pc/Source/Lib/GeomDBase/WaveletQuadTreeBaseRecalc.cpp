/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of WaveletQuadTreeBaseRecalc.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTreeBaseRecalc.cpp                           $
 * 
 * 11    9/23/98 7:41p Mlange
 * Wavelet quad tree nodes now have a single pointer to the first descendant and a pointer to
 * the next sibling, instead of four pointers for each of the descendants.
 * 
 * 10    8/25/98 8:40p Rvande
 * Loop variable re-scoped; 'class' keyword added to explicit template instantiation
 * 
 * 9     5/05/98 4:31p Mlange
 * Reorganised data members among the quad tree class hierarchy.
 * 
 * 8     1/21/98 1:35p Mlange
 * Minor optimisation: removed redundant reflecting of scaling coeficients at the max quad tree
 * boundaries in the Subdivide() function.
 * 
 * 7     98/01/15 14:11 Speter
 * Removed Assert for non-zero coefficient in Subdivide, because texturing has its own reasons
 * for subdividing.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "WaveletQuadTreeBaseRecalc.hpp"

namespace NMultiResolution
{


//**********************************************************************************************
//
// NMultiResolution::CQuadNodeBaseRecalcT<> implementation.
//

	//******************************************************************************************
	template<class TD, class TVT> CQuadNodeBaseRecalcT<TD, TVT>::CQuadNodeBaseRecalcT
	(
		const CQuadRootBaseRecalcT<TD, TVT>* pqnr_root, const TD* ptqn_parent, int i_relation, TD* aptqn_subdiv_neighbours[4], TVT* apqvt_dsc[5]
	)
		: CQuadNodeBaseT<TD, TVT>(ptqn_parent, i_relation, aptqn_subdiv_neighbours, apqvt_dsc),
		  tdData(ptqn_parent->tdData, i_relation, pqnr_root->ptdhData->bbGetData())
	{
	}


	//******************************************************************************************
	template<class TD, class TVT> void CQuadNodeBaseRecalcT<TD, TVT>::Subdivide(const CQuadRootBaseRecalcT<TD, TVT>* pqnr_root)
	{
		Assert(iGetSize() > 1);

		// Make sure the corner vertices are of the same level as the vertices for the descendant nodes.
		uint u_next_level = Max(pqvtGetVertex(0)->uGetRootLevel(), pqvtGetVertex(1)->uGetRootLevel()) + 1;

		pqvtGetVertex(0)->MakeLevel(u_next_level);
		pqvtGetVertex(1)->MakeLevel(u_next_level);
		pqvtGetVertex(2)->MakeLevel(u_next_level);
		pqvtGetVertex(3)->MakeLevel(u_next_level);


		TVT* apqvt_dsc[5];
		TD*  aptqn_subdiv_neighbours[4];

		AllocateDscVertices(apqvt_dsc, aptqn_subdiv_neighbours);

		InitSubdivide
		(
			new TD(pqnr_root, static_cast<TD*>(this), 0, aptqn_subdiv_neighbours, apqvt_dsc),
			new TD(pqnr_root, static_cast<TD*>(this), 1, aptqn_subdiv_neighbours, apqvt_dsc),
			new TD(pqnr_root, static_cast<TD*>(this), 2, aptqn_subdiv_neighbours, apqvt_dsc),
			new TD(pqnr_root, static_cast<TD*>(this), 3, aptqn_subdiv_neighbours, apqvt_dsc)
		);

		// Set the vertex wavelet coeficients for this node.
		CCoef acf_wvlts[3];
		tdData.GetWavelets(acf_wvlts, pqnr_root->ptdhData->bbGetData());

		ptqnGetFirstDescendant()->pqvtGetVertex(1)->SetWavelet(acf_wvlts[0]);
		ptqnGetFirstDescendant()->pqvtGetVertex(2)->SetWavelet(acf_wvlts[1]);
		ptqnGetFirstDescendant()->pqvtGetVertex(3)->SetWavelet(acf_wvlts[2]);

		Recalculate(acf_wvlts);
	}


	//******************************************************************************************
	template<class TD, class TVT> void CQuadNodeBaseRecalcT<TD, TVT>::Recalculate(CCoef acf_wvlts[3])
	{
		// Calculate the (unlifted) amount the corner vertex scaling coeficients change by as a result
		// of the new wavelet coeficient values.
		CCoef acf_corners[4];
		AccumulateWavelets(acf_wvlts, acf_corners);

		uint u_level = ptqnGetFirstDescendant()->pqvtGetVertex(2)->uGetRootLevel();

		CSCArray<CCoef, 4> csacf_diff;

		// Recalculate the scaling coeficients of the corner vertices of this node.
		for (int i_corner = 0; i_corner < 4; i_corner++)
			csacf_diff[i_corner] = pqvtGetVertex(i_corner)->cfUpdate(acf_corners[i_corner], u_level);

		// Recalculate the scaling coeficients of the descendant nodes.
		RecalcDscScalingCoef();


		TD* aptqn_modified_neighbours[8];

		// Recalculate the scaling coeficients of the diagonal neighbours.
		int i_n;
		for (i_n = 1; i_n < 8; i_n += 2)
		{
			aptqn_modified_neighbours[i_n] = 0;

			// A diagonal neighbour shares one vertex with this node. Only when this
			// shared vertex' scaling coeficients has changed, do we need to update that
			// neighbouring node.
			if (csacf_diff[(i_n + 1) >> 1] != cfZERO)
			{
				aptqn_modified_neighbours[i_n] = ptqnGetNearestNeighbour(i_n);

				if (aptqn_modified_neighbours[i_n] != 0)
					aptqn_modified_neighbours[i_n]->RecalcDscScalingCoef();
			}
		}


		// Recalculate the scaling coeficients of the transverse neighbours.
		for (i_n = 0; i_n < 8; i_n += 2)
		{
			// Note that we do not check if any of the two shared vertices' scaling coeficients have
			// changed because transverse neighbours must virtually always be updated.
			aptqn_modified_neighbours[i_n] = ptqnGetNearestNeighbour(i_n);

			if (aptqn_modified_neighbours[i_n] != 0)
				aptqn_modified_neighbours[i_n]->RecalcDscScalingCoef();
		}


		// Call the update function for all modified branches of the quad tree.
		static_cast<TD*>(this)->ModifiedVerticesBranch();

		for (i_n = 0; i_n < 8; i_n++)
			if (aptqn_modified_neighbours[i_n] != 0)
				aptqn_modified_neighbours[i_n]->ModifiedVerticesBranch();
	}


	//******************************************************************************************
	template<class TD, class TVT> void CQuadNodeBaseRecalcT<TD, TVT>::RecalcDscScalingCoef()
	{
		if (bHasDescendants())
		{
			// At the boundary of the quad tree, make sure the descendant scaling coeficients are
			// calculated from this node's reflected scaling coeficients.
			if (stState[estBOUNDARY_MAX_X][estBOUNDARY_MAX_Y])
			{
				uint u_level = ptqnGetFirstDescendant()->pqvtGetVertex(2)->uGetRootLevel();

				if (stState[estBOUNDARY_MAX_X])
				{
					pqvtGetVertex(1)->DupFrom(pqvtGetVertex(0), u_level);
					pqvtGetVertex(2)->DupFrom(pqvtGetVertex(3), u_level);
				}

				if (stState[estBOUNDARY_MAX_Y])
				{
					pqvtGetVertex(2)->DupFrom(pqvtGetVertex(1), u_level);
					pqvtGetVertex(3)->DupFrom(pqvtGetVertex(0), u_level);
				}
			}


			TD* ptqn_dsc = ptqnGetFirstDescendant();

			int i_dsc;
			for (i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				ptqn_dsc->pqvtGetVertex(i_dsc + 1)->SetRootScaling(pqvtGetVertex(i_dsc), pqvtGetVertex(i_dsc + 1));

				ptqn_dsc = ptqn_dsc->ptqnGetSibling();
			}


			if (stState[estDISC_DIAGONAL_1_3])
				ptqnGetFirstDescendant()->pqvtGetVertex(2)->SetRootScaling(pqvtGetVertex(1), pqvtGetVertex(3));
			else
				ptqnGetFirstDescendant()->pqvtGetVertex(2)->SetRootScaling(pqvtGetVertex(0), pqvtGetVertex(2));


			ptqn_dsc = ptqnGetFirstDescendant();

			for (i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				ptqn_dsc->RecalcDscScalingCoef();

				ptqn_dsc = ptqn_dsc->ptqnGetSibling();
			}
		}
	}


	//******************************************************************************************
	template<class TD, class TVT> void CQuadNodeBaseRecalcT<TD, TVT>::Decimate()
	{
		Assert(bHasDescendants());

		CCoef acf_wvlts[3] = 
		{
			-ptqnGetFirstDescendant()->pqvtGetVertex(1)->cfWavelet(),
			-ptqnGetFirstDescendant()->pqvtGetVertex(2)->cfWavelet(),
			-ptqnGetFirstDescendant()->pqvtGetVertex(3)->cfWavelet()
		};

		// Zero the vertex wavelet coeficients for this node.
		ptqnGetFirstDescendant()->pqvtGetVertex(1)->SetWavelet(cfZERO);
		ptqnGetFirstDescendant()->pqvtGetVertex(2)->SetWavelet(cfZERO);
		ptqnGetFirstDescendant()->pqvtGetVertex(3)->SetWavelet(cfZERO);

		Recalculate(acf_wvlts);

		CQuadNodeBaseT<TD, TVT>::Decimate();
	}


//**********************************************************************************************
//
// NMultiResolution::CQuadRootBaseRecalcT<> implementation.
//

	//******************************************************************************************
	template<class TD, class TVT> CQuadRootBaseRecalcT<TD, TVT>::CQuadRootBaseRecalcT(const CTransformedDataHeader* ptdh, TVT* pqvt_0, TVT* pqvt_1, TVT* pqvt_2, TVT* pqvt_3)
		: CQuadRootBaseT<TD, TVT>(pqvt_0, pqvt_1, pqvt_2, pqvt_3), ptdhData(ptdh)
	{
		Assert(bPowerOfTwo(ptdh->mpConversions.rcQuadSpaceExtents.v2Extent().tX));
		Assert(ptdh->mpConversions.rcQuadSpaceExtents.v2Extent().tX == ptdh->mpConversions.rcQuadSpaceExtents.v2Extent().tY);

		tdData = CTransformedData(ptdhData->bbGetData());
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
	CArrayAllocator<CCoef, iMAX_TRANSFORM_LEVELS> CQuadVertexRecalc::aaScalingAlloc;

	// Disable warning: template-class specialization 'xxx' is already instantiated.
	// If you instantiate these classes, VC4.2 complains.  If you don't, they don't link.  Lovely.
	#pragma warning(disable:4660)

	template class CQuadNodeBaseRecalcT<CQuadNodeTIN,   CQuadVertexTIN>;
	template class CQuadNodeBaseRecalcT<CQuadNodeQuery, CQuadVertexQuery>;

	template class CQuadRootBaseRecalcT<CQuadNodeTIN,   CQuadVertexTIN>;
	template class CQuadRootBaseRecalcT<CQuadNodeQuery, CQuadVertexQuery>;
};
