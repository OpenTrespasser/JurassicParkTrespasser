/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of WaveletQuadTreeTForm.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTreeTForm.cpp                                $
 * 
 * 17    9/23/98 7:41p Mlange
 * Wavelet quad tree nodes now have a single pointer to the first descendant and a pointer to
 * the next sibling, instead of four pointers for each of the descendants.
 * 
 * 16    8/25/98 8:41p Rvande
 * Iterator moved out of loop scope
 * 
 * 15    5/05/98 4:31p Mlange
 * Reorganised data members among the quad tree class hierarchy.
 * 
 * 14    3/02/98 7:36p Mlange
 * Added CQuadRootTForm::iGetNumSignificantNodes() and CQuadRootTForm::iGetNumVertices().
 * 
 * 13    1/21/98 2:58p Mlange
 * Updated stat printing.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "WaveletQuadTreeTForm.hpp"

#include "Lib/Sys/Textout.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "Lib/Std/Random.hpp"
#include "Lib/View/Raster.hpp"

#include "Lib/GeomDBase/TerrainLoad.hpp"
#include "Lib/GeomDBase/WaveletDataFormat.hpp"


namespace NMultiResolution
{

//**********************************************************************************************
//
// NMultiResolution::CQuadVertexTForm implementation.
//

	//******************************************************************************************
	CBlockAllocator<CQuadVertexTForm>::SStore CQuadVertexTForm::stStore;



//**********************************************************************************************
//
// NMultiResolution::CQuadNodeTForm implementation.
//

	//******************************************************************************************
	CBlockAllocator<CQuadNodeTForm>::SStore CQuadNodeTForm::stStore;


	//******************************************************************************************
	CQuadNodeTForm::CQuadNodeTForm(const CQuadNodeTForm* ptqn_parent, int i_relation, CQuadNodeTForm* aptqn_subdiv_neighbours[4], CQuadVertexTForm* apqvt_dsc[5])
		: CQuadNodeBaseT<CQuadNodeTForm, CQuadVertexTForm>(ptqn_parent, i_relation, aptqn_subdiv_neighbours, apqvt_dsc),
		  cfMaxRegion(cfZERO), uBranchDataBitSize(TypeMax(int))
	{
	}


	//******************************************************************************************
	void CQuadNodeTForm::CalcBranchDataSize()
	{
		// The size of the wavelet data for a node depends on the data size of its parent, so we need to
		// calculate the data size 'bottom up'.
		if (stState[estSIGNIFICANT])
		{
			Assert(bHasDescendants() && cfMaxRegion != cfZERO);

			CQuadNodeTForm* pqnt_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				pqnt_dsc->CalcBranchDataSize();

				pqnt_dsc = pqnt_dsc->ptqnGetSibling();
			}
		}

		// Calculate the data size for this node.
		uBranchDataBitSize = CTransformedData::uEncodedBitSize(*this);

		// Add the data size of the descendant nodes, if any.
		if (stState[estSIGNIFICANT])
		{
			CQuadNodeTForm* pqnt_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				uBranchDataBitSize += pqnt_dsc->uBranchDataBitSize;

				pqnt_dsc = pqnt_dsc->ptqnGetSibling();
			}
		}
	}


	//******************************************************************************************
	void CQuadNodeTForm::Grow()
	{
		for (int i_neighbour = 0; i_neighbour < 8; i_neighbour++)
		{
			CQuadNodeTForm* pqnt_neighbour = ptqnGetNearestNeighbour(i_neighbour);

			// Check the subdivision level of the neighbouring node and grow it if necessary.
			if (pqnt_neighbour != 0 && pqnt_neighbour->iGetSize() != iGetSize())
				pqnt_neighbour->Grow();
		}

		// The subdivision levels of the neighbouring nodes now match (or exceed by at most one) the subdivision
		// level of this node, so we can now subdivide it.
		CQuadVertexTForm* apqvt_dsc[5];
		  CQuadNodeTForm* aptqn_subdiv_neighbours[4];

		AllocateDscVertices(apqvt_dsc, aptqn_subdiv_neighbours);

		InitSubdivide
		(
			new CQuadNodeTForm(this, 0, aptqn_subdiv_neighbours, apqvt_dsc),
			new CQuadNodeTForm(this, 1, aptqn_subdiv_neighbours, apqvt_dsc),
			new CQuadNodeTForm(this, 2, aptqn_subdiv_neighbours, apqvt_dsc),
			new CQuadNodeTForm(this, 3, aptqn_subdiv_neighbours, apqvt_dsc)
		);
	}


	//******************************************************************************************
	void CQuadNodeTForm::InterpolateUndefinedPoints(CConsoleBuffer& rcon_text_out, bool b_recursed)
	{
		const int iUPDATE_DELAY = 547;	// Specifies the console buffer update rate.

		//
		// Print progress stats.
		//
		static int iNumNodesVisited;

		if (!b_recursed)
		{
			rcon_text_out.Print("Interpolating undefined points\n");

			iNumNodesVisited = 0;
		}

		iNumNodesVisited++;

		rcon_text_out.Print("\r Nodes checked: %d", iNumNodesVisited);

		if ((iNumNodesVisited - 1) % iUPDATE_DELAY == 0)
			rcon_text_out.Show();


		//
		// Interpolate the unspecified scaling coeficients.
		//
		// Unspecified scaling coeficients are interpolated by 'predicting' them from their neighbouring
		// coeficients. Note that this is identical to behaviour of the wavelet transform predict stage.
		//
		if (bHasDescendants())
		{
			CQuadNodeTForm* pqnt_dsc = ptqnGetFirstDescendant();

			for (int i_dsc_vt = 0; i_dsc_vt < 4; i_dsc_vt++)
			{
				// Interpolate any missing scaling coeficients at the midpoint of this node's edges.
				CQuadVertexTForm* pqvtq = pqnt_dsc->pqvtGetVertex(i_dsc_vt + 1);

				if (!pqvtq->bHasScaling())
					pqvtq->SetScaling((cfScalingReflect(i_dsc_vt) + cfScalingReflect(i_dsc_vt + 1)).cfPredict());

				pqnt_dsc = pqnt_dsc->ptqnGetSibling();
			}

			// Interpolate the scaling coeficient at the centre of this node, if necessary.
			CQuadVertexTForm* pqvtq = ptqnGetFirstDescendant()->pqvtGetVertex(2);

			if (!pqvtq->bHasScaling())
				if (stState[estDISC_DIAGONAL_1_3])
					pqvtq->SetScaling((cfScalingReflect(1) + cfScalingReflect(3)).cfPredict());
				else
					pqvtq->SetScaling((cfScalingReflect(0) + cfScalingReflect(2)).cfPredict());

			#if VER_DEBUG
				// Invalidate scaling coeficients that lie on the max X/Y boundaries of the quad tree because they do not
				// participate in the wavelet transform calculation.
				if (stState[estBOUNDARY_MAX_X])
					ptqnGetFirstDescendant()->ptqnGetSibling()->pqvtGetVertex(2)->SetScaling(cfNAN);

				if (stState[estBOUNDARY_MAX_Y])
					ptqnGetFirstDescendant()->ptqnGetSibling()->ptqnGetSibling()->pqvtGetVertex(3)->SetScaling(cfNAN);
			#endif

			// Interpolate the unspecified scaling coeficients for the remaining nodes in this branch of the tree.
			pqnt_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				pqnt_dsc->InterpolateUndefinedPoints(rcon_text_out, true);

				pqnt_dsc = pqnt_dsc->ptqnGetSibling();
			}
		}
	}


	//******************************************************************************************
	void CQuadNodeTForm::AnalyseStep(int i_req_node_size)
	{
		Assert(iGetSize() >= i_req_node_size);

		// Search down quad tree for quad nodes whose size matches the required decomposition level.
		if (iGetSize() == i_req_node_size)
		{
			if (bHasDescendants())
			{
				//
				// Calculate the three wavelet coeficients in this node.
				//
				// Predict each wavelet from the scaling coeficients of the endpoint vertices of the edge on
				// which it lies.
				//
				CCoef acf_wvlt[3];
				acf_wvlt[0] = (cfScalingReflect(0) + cfScalingReflect(1)).cfPredict();
				acf_wvlt[2] = (cfScalingReflect(0) + cfScalingReflect(3)).cfPredict();

				if (stState[estDISC_DIAGONAL_1_3])
					acf_wvlt[1] = (cfScalingReflect(1) + cfScalingReflect(3)).cfPredict();
				else
					acf_wvlt[1] = (cfScalingReflect(0) + cfScalingReflect(2)).cfPredict();

				for (int i_wvlt = 0; i_wvlt < 3; i_wvlt++)
				{
					CQuadVertexTForm* pqvtq_wvlt = ptqnGetFirstDescendant()->pqvtGetVertex(i_wvlt + 1);

					acf_wvlt[i_wvlt] = pqvtq_wvlt->cfScaling() - acf_wvlt[i_wvlt];

					pqvtq_wvlt->InitWavelet(acf_wvlt[i_wvlt]);

					// Update the maximum wavelet coeficient value if this wavelet coeficient value is greater.
					cfMaxRegion = cfMaxRegion.cfMaxAbs(pqvtq_wvlt->cfWavelet());
				}

				// Update the maximum wavelet coeficient value if a descendant's max wavelet coeficient value is greater.
				CQuadNodeTForm* pqnt_dsc = ptqnGetFirstDescendant();

				for (int i_dsc = 0; i_dsc < 4; i_dsc++)
				{
					cfMaxRegion = cfMaxRegion.cfMaxAbs(pqnt_dsc->cfMaxRegion);

					pqnt_dsc = pqnt_dsc->ptqnGetSibling();
				}

				// If all the wavelet coeficients in this branch of the quad tree have a zero value, this
				// node is insignificant e.g. contains no relevant wavelet transformed data.
				stState[estSIGNIFICANT] = cfMaxRegion != cfZERO;

				CCoef acf_corners[4];
				AccumulateWavelets(acf_wvlt, acf_corners);

				for (int i_vt = 0; i_vt < 4; i_vt++)
					pqvtGetVertex(i_vt)->SumWavelet(acf_corners[i_vt]);

				// If the neighbours to the right of or up from this node contain no wavelet coeficients, we need
				// to calculate the wavelet coeficient values of the vertices along the edge shared with that
				// neighbour now. We set these wavelet coeficient values to zero, because we know that these
				// vertices have been interpolated by the InterpolateUndefinedPoints() function.
				if (ptqnGetNeighbour(2) == 0 || !ptqnGetNeighbour(2)->bHasDescendants())
					ptqnGetFirstDescendant()->ptqnGetSibling()->pqvtGetVertex(2)->InitWavelet(cfZERO);

				if (ptqnGetNeighbour(4) == 0 || !ptqnGetNeighbour(4)->bHasDescendants())
					ptqnGetFirstDescendant()->ptqnGetSibling()->ptqnGetSibling()->pqvtGetVertex(3)->InitWavelet(cfZERO);

				// Decimate this branch of the quad tree if it is insignificant, to ensure the count of allocated
				// quad tree nodes is an accurate representation of the number of significant nodes.
				if (!stState[estSIGNIFICANT])
					Decimate();
			}

			// Because of the order in which the quad tree is traversed (see below), at this point, all the wavelet
			// coeficients that influence the base vertex have been calculated and accumulated in that vertex.
			pqvtGetVertex(0)->Lift();
		}
		else
		{
			if (bHasDescendants())
			{
				// The order in which the wavelet quad tree is traversed is significant. Note that this is NOT
				// in sequential order.
				ptqnGetDescendant(0)->AnalyseStep(i_req_node_size);
				ptqnGetDescendant(1)->AnalyseStep(i_req_node_size);
				ptqnGetDescendant(3)->AnalyseStep(i_req_node_size);
				ptqnGetDescendant(2)->AnalyseStep(i_req_node_size);
			}
		}
	}


	//******************************************************************************************
	void CQuadNodeTForm::FillRaster(CRasterT<CCoef>* pras) const
	{
		pras->tPix(iBaseX(), iBaseY()) = pqvtGetVertex(0)->cfScaling();

		if (bHasDescendants())
		{
			CQuadNodeTForm* pqnt_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				pqnt_dsc->FillRaster(pras);

				pqnt_dsc = pqnt_dsc->ptqnGetSibling();
			}
		}
	}


	//******************************************************************************************
	void CQuadNodeTForm::CheckWavelets(CRasterT<CCoef>* pras) const
	{
		if (bHasDescendants())
		{
			for (int i_wvlt = 0; i_wvlt < 3; i_wvlt++)
			{
				CQuadVertexTForm* pqvtq_wvlt = ptqnGetFirstDescendant()->pqvtGetVertex(i_wvlt + 1);

				AlwaysAssert(pqvtq_wvlt->cfWavelet() == pras->tPix(pqvtq_wvlt->iX(), pqvtq_wvlt->iY()));

				// Indicate that this wavelet coeficient has been checked.
				pras->tPix(pqvtq_wvlt->iX(), pqvtq_wvlt->iY()) = cfZERO;
			}

			CQuadNodeTForm* pqnt_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				pqnt_dsc->CheckWavelets(pras);

				pqnt_dsc = pqnt_dsc->ptqnGetSibling();
			}
		}
	}


	//******************************************************************************************
	void CQuadNodeTForm::CheckDataBranch(const CTransformedData& td, const CTransformedDataHeader& tdh) const
	{
		AlwaysAssert(td.cfGetMaxRegion() == cfGetMaxRegion());

		if (stState[estSIGNIFICANT])
		{
			Assert(bHasDescendants() && cfGetMaxRegion() != cfZERO);

			CCoef acf_wvlts[3];
			td.GetWavelets(acf_wvlts, tdh.bbGetData());

			for (int i_wvlt = 0; i_wvlt < 3; i_wvlt++)
				AlwaysAssert(acf_wvlts[i_wvlt] == ptqnGetFirstDescendant()->pqvtGetVertex(i_wvlt + 1)->cfWavelet());


			CQuadNodeTForm* pqnt_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				pqnt_dsc->CheckDataBranch(CTransformedData(td, i_dsc, tdh.bbGetData()), tdh);

				pqnt_dsc = pqnt_dsc->ptqnGetSibling();
			}
		}
	}



//**********************************************************************************************
//
// NMultiResolution::CQuadRootTForm implementation.
//

	//******************************************************************************************
	CQuadRootTForm::CQuadRootTForm(const SExportDataInfo& edi, CConsoleBuffer& rcon_text_out)
		: CQuadRootBaseT<CQuadNodeTForm, CQuadVertexTForm>
		  (
			new CQuadVertexTForm(0,               0),
			new CQuadVertexTForm(edi.iMaxIndices, 0),
			new CQuadVertexTForm(edi.iMaxIndices, edi.iMaxIndices),
			new CQuadVertexTForm(0,               edi.iMaxIndices)
		  ),
		  iNumPointsAdded(0), rMaxQuantisationError(0), iLevel(0), rconTextOut(rcon_text_out), mpConversions(edi)
	{
		Assert(bPowerOfTwo(mpConversions.rcQuadSpaceExtents.v2Extent().tX));
		Assert(mpConversions.rcQuadSpaceExtents.v2Extent().tX == mpConversions.rcQuadSpaceExtents.v2Extent().tY);
		Assert(CQuadNodeTForm::pfhGetFastHeap() != 0 && CQuadVertexTForm::pfhGetFastHeap() != 0);

		rconTextOut.SetActive(true);
	}


	//******************************************************************************************
	CQuadRootTForm::~CQuadRootTForm()
	{
		// Decimate entire quad tree,
		DecimateBranch();

		Assert(CQuadNodeTForm::pfhGetFastHeap() == CQuadVertexTForm::pfhGetFastHeap());
	}


	//******************************************************************************************
	void CQuadRootTForm::AllocMemory()
	{
		Assert(CQuadNodeTForm::pfhGetFastHeap() == 0 && CQuadVertexTForm::pfhGetFastHeap() == 0);

		// Allocate and set the fastheap used for allocations of the transform quad tree classes.
		CFastHeap* pfh = ::new CFastHeap(iTRANSFORM_BUFFER_SIZE_MB * 1024 * 1024);
		  CQuadNodeTForm::SetFastHeap(pfh);
		CQuadVertexTForm::SetFastHeap(pfh);
	}


	//**********************************************************************************************
	void CQuadRootTForm::FreeMemory()
	{
		// Delete the memory used for allocations of the transform quad tree classes.
		CFastHeap* pfh = CQuadNodeTForm::pfhGetFastHeap();

		Assert(CQuadVertexTForm::pfhGetFastHeap() == pfh);

		  CQuadNodeTForm::SetFastHeap(0);
		CQuadVertexTForm::SetFastHeap(0);

		delete pfh;
	}


	//******************************************************************************************
	int CQuadRootTForm::iGetNumSignificantNodes() const
	{
		return CQuadNodeTForm::uNumAlloc() + 1;
	}


	//******************************************************************************************
	int CQuadRootTForm::iGetNumVertices() const
	{
		return CQuadVertexTForm::uNumAlloc();
	}


	//******************************************************************************************
	void CQuadRootTForm::AddPoints(CTerrainExportedData& rted)
	{
		// Make sure points are added only once.
		Assert(!bHasDescendants());

		rted.Reset();

		for (int i_pt = 0; i_pt < rted.ediGetInfo().iNumDataPoints; i_pt++)
			AddPoint(rted.v3Read(), i_pt, rted.ediGetInfo().iNumDataPoints);

		Assert(iNumPointsAdded <= rted.ediGetInfo().iNumDataPoints);

		InterpolateUndefinedPoints(rconTextOut);
		rconTextOut.Print("\n\n");
		rconTextOut.Show();
	}


	//******************************************************************************************
	void CQuadRootTForm::RandomisePoints(const SExportDataInfo& edi, CRandom& rrnd)
	{
		// Make sure points are added only once.
		Assert(!bHasDescendants());

		// Ensure the random data set includes the root node base vertex.
		AddPoint(CVector3<>(edi.v2Min.tX, edi.v2Min.tY, rrnd(edi.rMinHeight, edi.rMaxHeight)), 0, edi.iNumDataPoints);

		for (int i_pt = 1; i_pt < edi.iNumDataPoints; i_pt++)
		{
			CVector3<> v3_rnd_world_pt;

			v3_rnd_world_pt = CVector2<>
							  (
								rrnd(uint32(mpConversions.rcQuadSpaceExtents.tX0()), mpConversions.rcQuadSpaceExtents.tX1()),
								rrnd(uint32(mpConversions.rcQuadSpaceExtents.tY0()), mpConversions.rcQuadSpaceExtents.tY1())
							  ) * mpConversions.tlr2QuadToWorld;

			v3_rnd_world_pt.tZ = rrnd(edi.rMinHeight, edi.rMaxHeight);

			AddPoint(v3_rnd_world_pt, i_pt, edi.iNumDataPoints);
		}

		InterpolateUndefinedPoints(rconTextOut);
		rconTextOut.Print("\n\n");
		rconTextOut.Show();
	}


	//******************************************************************************************
	void CQuadRootTForm::Analyse()
	{
		Assert(iLevel == 0);

		//
		// Perform analysing wavelet transform at each decomposition level.
		//
		rconTextOut.Print("Analysing data\n");

		// Calculate the number of decomposition levels required.
		int i_num_levels = uLog2(iGetSize());

		Assert(i_num_levels < iMAX_TRANSFORM_LEVELS);

		int i_req_size;

		do
		{
			iLevel++;

			i_req_size = 1 << iLevel;

			AnalyseStep(i_req_size);

			rconTextOut.Print("\r Levels remaining: %d   ", i_num_levels - iLevel);
			rconTextOut.Show();
		}
		while (i_req_size != iGetSize());

		rconTextOut.Print("\n\n");
		rconTextOut.Show();


		//
		// Calculate the sizes of the encoded wavelet transform data for all nodes in the quad tree.
		//
		// The encoded data size of a particular node depends on the data size of its parent. But, in
		// turn, that parent's encoded data size depends on the data size of the descendant. Hence, the
		// data size of a node cannot be determined directly. Instead, we must calculate the data sizes
		// with several passes through the quad tree. In each pass,.we calculate the data sizes 'bottom
		// up' (e.g. starting at the leaf nodes and going up the tree). For the first pass we simply
		// assume the parent's data size is at its theoretical maximum. With each subsequent pass, the
		// data sizes will converge to their correct value; we stop once the data sizes remain unchanged
		// between each pass.
		//
		uint u_prev_size;

		do
		{
			u_prev_size = uGetBranchDataBitSize();

			// Calculate the data sizes for all the nodes.
			CalcBranchDataSize();
		}
		while (u_prev_size != uGetBranchDataBitSize());

		// Make sure the root node data is within the valid range.
		AlwaysAssert(uGetBranchDataBitSize() < (1 << iMAX_ENCODE_SIG_BITS) && cfGetMaxRegion() < CCoef(1 << iMAX_ENCODE_SIG_BITS));

		rconTextOut.Print("Num significant nodes  : %u\n",      iGetNumSignificantNodes());
		rconTextOut.Print("Avg. data bits per node: %4.1f\n\n", double(uGetBranchDataBitSize()) / iGetNumSignificantNodes());
		rconTextOut.Show();
	}


	//******************************************************************************************
	CRasterT<CCoef>* CQuadRootTForm::prasGetData() const
	{
		Assert(iLevel == 0);

		CRasterT<CCoef>* pras = new CRasterT<CCoef>(iGetSize(), iGetSize());

		pras->Fill(cfNAN);

		FillRaster(pras);

		return pras;
	}


	//******************************************************************************************
	void CQuadRootTForm::CheckData(const CRasterT<CCoef>* pras_cmp) const
	{
		Assert(iLevel != 0);
		Assert(pras_cmp->iWidth == iGetSize() && pras_cmp->iHeight == iGetSize());

		// The raster will be modified by the CheckWavelets() function, so make a copy first.
		CRasterT<CCoef>* pras_copy = new CRasterT<CCoef>(iGetSize(), iGetSize());
		memcpy(*pras_copy, *pras_cmp, pras_cmp->iSize() * sizeof(CCoef));

		// Check the root scaling coeficient.
		AlwaysAssert(pqvtGetVertex(0)->cfScaling() == pras_copy->tPix(0, 0));

		pras_copy->tPix(0, 0) = cfZERO;
		CheckWavelets(pras_copy);

		// All the wavelet coeficients have been checked and the raster must therefore contain all
		// zeroes at this point.
		for (int i_y = 0; i_y < iGetSize(); i_y++)
			for (int i_x = 0; i_x < iGetSize(); i_x++)
				AlwaysAssert(pras_copy->tPix(i_x, i_y) == cfZERO);

		delete pras_copy;
	}



	//******************************************************************************************
	void CQuadRootTForm::CheckData(const CTransformedDataHeader& tdh) const
	{
		Assert(iLevel != 0);

		// Check the contents of the header.
		AlwaysAssert(pqvtGetVertex(0)->cfScaling() == tdh.cfRoot);
		AlwaysAssert(memcmp(&mpConversions, &tdh.mpConversions, sizeof(SMapping)) == 0);

		CheckDataBranch(CTransformedData(tdh.bbGetData()), tdh);
	}



	//******************************************************************************************
	void CQuadRootTForm::AddPoint(const CVector3<>& v3_world_pos, int i_pt_num, int i_total_num_points)
	{
		const int iUPDATE_DELAY = 547;	// Specifies the console buffer update rate.

		// Convert point's XY world location to quad tree indices.
		int i_quad_x = iPosFloatCast(v3_world_pos.tX * mpConversions.tlr2WorldToQuad.tlrX);
		int i_quad_y = iPosFloatCast(v3_world_pos.tY * mpConversions.tlr2WorldToQuad.tlrY);

		// Make sure the point lies exactly on a grid location.
		Assert(i_quad_x == v3_world_pos.tX * mpConversions.tlr2WorldToQuad.tlrX &&
		       i_quad_y == v3_world_pos.tY * mpConversions.tlr2WorldToQuad.tlrY   );

		//
		// Find the leaf node that contains the point, if any. If no leaf node can be found, this point lies
		// outside the bounds of the quad tree and is ignored.
		//
		CQuadNodeTForm* pqnt_containing = ptqnFindLeaf(i_quad_x, i_quad_y);

		if (pqnt_containing != 0)
		{
			// Each data point must define the base vertex of a quad node, so refine the quad tree if a base
			// vertex does not exist yet.
			while (!(pqnt_containing->iBaseX() == i_quad_x && pqnt_containing->iBaseY() == i_quad_y))
			{
				pqnt_containing->Grow();

				// Find the descendant that contains point.
				CQuadNodeTForm* pqnt_dsc = pqnt_containing->ptqnGetFirstDescendant();

				for (int i_dsc = 0; i_dsc < 3; i_dsc++)
					if (pqnt_dsc->bContains(i_quad_x, i_quad_y))
						break;
					else
						pqnt_dsc = pqnt_dsc->ptqnGetSibling();

				pqnt_containing = pqnt_dsc;

				Assert(pqnt_containing->bContains(i_quad_x, i_quad_y));
			}

			// Has a data point already been added at this location?
			if (!pqnt_containing->pqvtGetVertex(0)->bHasScaling())
				iNumPointsAdded++;

			// Quantise the point's world Z value.
			CCoef cf_coef(v3_world_pos.tZ, mpConversions.rWorldToCoef);

			pqnt_containing->pqvtGetVertex(0)->SetScaling(cf_coef);

			rMaxQuantisationError = Max(rMaxQuantisationError, Abs(v3_world_pos.tZ - cf_coef.rGet(mpConversions.rCoefToWorld)));
		}


		//
		// Print progress stats.
		//
		if ((i_pt_num % iUPDATE_DELAY == 0) || (i_pt_num == i_total_num_points - 1))
		{
			if (i_pt_num != 0)
				rconTextOut.MoveCursor(0, -10);

			rconTextOut.Print("Adding points\n");
			rconTextOut.Print(" Actual num points added: %d\n",   iNumPointsAdded);
			rconTextOut.Print(" Actual quantise error  : %2.2fcm   \n\n", rMaxQuantisationError * 100.0);
			rconTextOut.Print("Quad tree info\n");
			rconTextOut.Print(" Nodes     : %u\n", iGetNumSignificantNodes());
			rconTextOut.Print(" Vertices  : %u\n", iGetNumVertices());


			int i_bytes_commit = CQuadVertexTForm::pfhGetFastHeap()->uNumBytesUsed();
			int i_bytes_free   = CQuadVertexTForm::pfhGetFastHeap()->uNumBytesFree();

			rconTextOut.Print(" Mem commit: %4.1f MB  \n",   i_bytes_commit / (1024.0 * 1024.0));
			rconTextOut.Print(" Mem free  : %4.1f MB  \n\n", i_bytes_free   / (1024.0 * 1024.0));
			rconTextOut.Show();
		}
	}
};
