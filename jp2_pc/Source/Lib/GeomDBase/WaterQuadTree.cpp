/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of WaterQuadTree.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaterQuadTree.cpp                                       $
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
 * 1     5/04/98 12:58p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#include "GblInc/Common.hpp"

#if VER_MULTI_RES_WATER

#include "WaterQuadTree.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/View/LineDraw.hpp"


namespace NMultiResolution
{

//**********************************************************************************************
//
// NMultiResolution::CQuadVertexWater implementation.
//

	//******************************************************************************************
	CBlockAllocator<CQuadVertexWater>::SStore CQuadVertexWater::stStore;



//**********************************************************************************************
//
// NMultiResolution::CQuadNodeWater implementation.
//

	//**********************************************************************************************
	//
	// Definitions for NMultiResolution::SEvalInfoWater.
	//

	// World distance in meters to which the far distance settings are normalised.
	static const float fWorldFarNormaliseDist = 500;

	//**********************************************************************************************
	//
	struct SEvalInfoWater
	//
	// Simple POD containing pre-calculated data for quad tree evaluation.
	//
	// Prefix: ei
	//
	//**************************************
	{
		CVector3<> v3QuadSpaceCamPos;			// Position of the camera in quad tree units.

//		float fWorldToPixRatio;

//		float fNormSqrQuadDistFactor;			// Factor to normalise squared quad space distances to fWorldFarNormaliseDist.
//		float fPixTolToCoef;
//		float fPixTolDeltaToCoef;

		int iDistRatioMinNodeSize;
//		float fMaxDistRatioSqr;
//		float fDisableShadowDistSqr;
//		float fDisableTextureDistSqr;

//		float fFarViewDist;
	};


	//******************************************************************************************
	CBlockAllocator<CQuadNodeWater>::SStore CQuadNodeWater::stStore;


	//******************************************************************************************
	void CQuadNodeWater::Grow()
	{
		Assert(!bHasDescendants());

//ONLY NECESSARY FOR TRANSVERSE NEIGHBOURS?
		for (int i_neighbour = 0; i_neighbour < 8; i_neighbour++)
		{
			CQuadNodeWater* pqnw_neighbour = ptqnGetNearestNeighbour(i_neighbour);

			// Check the subdivision level of the neighbouring node and grow it if necessary.
			if (pqnw_neighbour != 0 && pqnw_neighbour->iGetSize() != iGetSize())
				pqnw_neighbour->Grow();
		}

		// The subdivision levels of the neighbouring nodes now match (or exceed by at most one) the subdivision
		// level of this node, so we can now subdivide it.
		CQuadVertexWater* apqvt_dsc[5];
		  CQuadNodeWater* aptqn_subdiv_neighbours[4];

		AllocateDscVertices(apqvt_dsc, aptqn_subdiv_neighbours);

		InitSubdivide
		(
			new CQuadNodeWater(this, 0, aptqn_subdiv_neighbours, apqvt_dsc),
			new CQuadNodeWater(this, 1, aptqn_subdiv_neighbours, apqvt_dsc),
			new CQuadNodeWater(this, 2, aptqn_subdiv_neighbours, apqvt_dsc),
			new CQuadNodeWater(this, 3, aptqn_subdiv_neighbours, apqvt_dsc)
		);
	}


	//******************************************************************************************
	void CQuadNodeWater::Reduce()
	{
		Assert(stState[estLEAF_COMBINE]);

		if (stState[estSIGNIFICANT])
			return;

//ONLY NECESSARY FOR TRANSVERSE NEIGHBOURS?
		bool b_can_decimate = true;

		for (int i_neighbour = 0; i_neighbour < 8; i_neighbour++)
		{
			CQuadNodeWater* pqnw_neighbour = ptqnGetNeighbour(i_neighbour);

			Assert(pqnw_neighbour || !ptqnGetNearestNeighbour(i_neighbour));

			// Check the subdivision level of the neighbouring node and reduce it if necessary and possible.
			if (pqnw_neighbour != 0 && !pqnw_neighbour->stState[estLEAF][estLEAF_COMBINE])
			{
				Assert(pqnw_neighbour->bHasDescendants());

				if ((i_neighbour & 1) == 0)
				{
					Assert(ptqnGetDescendant((i_neighbour >> 1) + 2)->stState[estLEAF][estLEAF_COMBINE] && ptqnGetDescendant((i_neighbour >> 1) + 3)->stState[estLEAF][estLEAF_COMBINE]);

					if (ptqnGetDescendant((i_neighbour >> 1) + 2)->stState[estLEAF_COMBINE])
						ptqnGetDescendant((i_neighbour >> 1) + 2)->Reduce();

					if (ptqnGetDescendant((i_neighbour >> 1) + 3)->stState[estLEAF_COMBINE])
						ptqnGetDescendant((i_neighbour >> 1) + 3)->Reduce();
				}
				else
				{
					Assert(ptqnGetDescendant((i_neighbour >> 1) + 3)->stState[estLEAF][estLEAF_COMBINE]);

					if (ptqnGetDescendant((i_neighbour >> 1) + 3)->stState[estLEAF_COMBINE])
						ptqnGetDescendant((i_neighbour >> 1) + 3)->Reduce();
				}
			}

			if (pqnw_neighbour != 0 && !pqnw_neighbour->stState[estLEAF][estLEAF_COMBINE])
				b_can_decimate = false;
		}

		Assert(!stState[estSIGNIFICANT]);
		Assert(stState[estLEAF_COMBINE]);

		if (b_can_decimate)
			Decimate();
	}


	//******************************************************************************************
	void CQuadNodeWater::EvaluateBranch(const CQuadRootWater* pqnw_root)
	{
//DO THIS ONLY IF PARENT WAS SIGNIFICANT? AT VERY LEAST SHOULD ASSERT FOR THAT
		CIntFloat if_dist_x_min = pqvtGetVertex(0)->iX() - pqnw_root->peiInfo->v3QuadSpaceCamPos.tX;
		if (if_dist_x_min.i4Int < 0)
		{
			if_dist_x_min = pqnw_root->peiInfo->v3QuadSpaceCamPos.tX - pqvtGetVertex(2)->iX();
			if (if_dist_x_min.i4Int < 0)
				if_dist_x_min = 0;
		}

		CIntFloat if_dist_y_min = pqvtGetVertex(0)->iY() - pqnw_root->peiInfo->v3QuadSpaceCamPos.tY;
		if (if_dist_y_min.i4Int < 0)
		{
			if_dist_y_min = pqnw_root->peiInfo->v3QuadSpaceCamPos.tY - pqvtGetVertex(2)->iY();
			if (if_dist_y_min.i4Int < 0)
				if_dist_y_min = 0;
		}

		float f_min_dist = CIntFloat(Max(if_dist_x_min.i4Int, if_dist_y_min.i4Int)).fFloat;
		float f_max_dist = f_min_dist + iGetSize();

		stState[estSIGNIFICANT] =
			iGetSize() > pqnw_root->peiInfo->iDistRatioMinNodeSize &&
			CIntFloat(f_max_dist).i4Int > CIntFloat(f_min_dist * CQuadRootWater::rvarMaxDistRatio).i4Int;


		if (stState[estSIGNIFICANT])
		{
			if (!bHasDescendants())
				Grow();
		}

		if (bHasDescendants())
			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
				ptqnGetDescendant(i_dsc)->EvaluateBranch(pqnw_root);

		if (!stState[estSIGNIFICANT])
		{
			if (stState[estLEAF_COMBINE])
				Reduce();
		}
	}


	//******************************************************************************************
	void CQuadNodeWater::DrawWireframeBranch(CDraw& draw, const CColour& clr) const
	{
		draw.MoveTo(iBaseX(),              iBaseY());
		draw.LineTo(iBaseX() + iGetSize(), iBaseY());
		draw.LineTo(iBaseX() + iGetSize(), iBaseY() + iGetSize());
		draw.LineTo(iBaseX(),              iBaseY() + iGetSize());
		draw.LineTo(iBaseX(),              iBaseY());

		if (bHasDescendants())
			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
				ptqnGetDescendant(i_dsc)->DrawWireframeBranch(draw, clr);
	}



//**********************************************************************************************
//
// NMultiResolution::CQuadRootWater implementation.
//

	static const int iQUAD_TREE_RESOLUTION = 1024;

	//******************************************************************************************
	CRangeVar<float> CQuadRootWater::rvarMaxDistRatio(2, 6, 2.8);
	CRangeVar<int>   CQuadRootWater::rvarMinDistRatioNodeSize(1, 16, 1);

	//******************************************************************************************
	CQuadRootWater::CQuadRootWater(const CInstance& ins)
		: CQuadRootBaseT<CQuadNodeWater, CQuadVertexWater>
		  (
			new CQuadVertexWater(0,                     0),
			new CQuadVertexWater(iQUAD_TREE_RESOLUTION, 0),
			new CQuadVertexWater(iQUAD_TREE_RESOLUTION, iQUAD_TREE_RESOLUTION),
			new CQuadVertexWater(0,                     iQUAD_TREE_RESOLUTION)
		  ),
		  peiInfo(new SEvalInfoWater)
	{
		Assert(bPowerOfTwo(iQUAD_TREE_RESOLUTION));
		Assert(CQuadNodeWater::pfhGetFastHeap() != 0 && CQuadVertexWater::pfhGetFastHeap() != 0);

		AlwaysAssert(ins.pbvBoundingVol()->pbvbCast());
		CBoundVolBox bvbox = *ins.pbvBoundingVol()->pbvbCast();

		CRectangle<> rc_obj_extent
		(
			bvbox[ebeMIN_X], bvbox[ebeMIN_Y],
			bvbox[ebeMAX_X] - bvbox[ebeMIN_X], bvbox[ebeMAX_Y] - bvbox[ebeMIN_Y]
		);

		CTransLinear2<> tlr2_obj_to_quad(rc_obj_extent, rcGetRectangle(), true);

		CPresence3<> pr3_obj_to_quad
		(
			CRotate3<>(),
			tlr2_obj_to_quad.tlrX.tScale,
			CVector3<>
			(
				tlr2_obj_to_quad.tlrX.tOffset,
				tlr2_obj_to_quad.tlrY.tOffset,
				0
			)
		);

		CPresence3<> pr3_world_to_obj = ~ins.pr3Presence();

		pr3WorldQuad = pr3_world_to_obj * pr3_obj_to_quad;
	}



	//******************************************************************************************
	CQuadRootWater::~CQuadRootWater()
	{
		DecimateBranch();

		delete peiInfo;
	}


	//******************************************************************************************
	void CQuadRootWater::Update(const CCamera& cam_view)
	{
		CVector3<> v3_cam_quad_pos = cam_view.v3Pos() * pr3WorldQuad;

		peiInfo->v3QuadSpaceCamPos     = v3_cam_quad_pos;
		peiInfo->iDistRatioMinNodeSize = rvarMinDistRatioNodeSize * pr3WorldQuad.rScale;

		EvaluateBranch(this);
	}


	//******************************************************************************************
	void CQuadRootWater::AllocMemory(uint u_max_nodes)
	{
		Assert(CQuadNodeWater::pfhGetFastHeap() == 0 && CQuadVertexWater::pfhGetFastHeap() == 0);

		// Allocate and set the fastheap used for allocations of the quad tree classes.
		uint u_max_node_bytes = sizeof(  CQuadNodeWater) * u_max_nodes;
		uint u_max_vert_bytes = sizeof(CQuadVertexWater) * u_max_nodes * dAVG_QUAD_NODE_TO_VERTEX_RATIO;

		CFastHeap* pfh = new CFastHeap(u_max_node_bytes + u_max_vert_bytes);

		  CQuadNodeWater::SetFastHeap(pfh);
		CQuadVertexWater::SetFastHeap(pfh);
	}


	//**********************************************************************************************
	void CQuadRootWater::FreeMemory()
	{
		// Delete the memory used for allocations of the quad tree classes.
		CFastHeap* pfh = CQuadNodeWater::pfhGetFastHeap();

		Assert(CQuadVertexWater::pfhGetFastHeap() == pfh);

		  CQuadNodeWater::SetFastHeap(0);
		CQuadVertexWater::SetFastHeap(0);

		delete pfh;
	}

	//******************************************************************************************
	CRectangle<> CQuadRootWater::rcDrawWireframe(CRasterWin* pras_win, const CColour& clr, const CVector2<>& v2_quad_pos, TReal r_zoom) const
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

		DrawWireframeBranch(draw, clr);

		return rc_quad;
	}


//**********************************************************************************************
//
// NMultiResolution::CQuadRootWaterShape implementation.
//


	//******************************************************************************************
	CQuadRootWaterShape::CQuadRootWaterShape(const CInstance& ins)
		: CQuadRootWater(ins)
	{
	}


	//******************************************************************************************
	CQuadRootWaterShape::~CQuadRootWaterShape()
	{
	}

	//******************************************************************************************
	TReal CQuadRootWaterShape::rPolyPlaneThickness() const
	{
//TODO: Determine required plane thickness
		if (d3dDriver.bUseD3D())
			return CShape::rPolyPlaneThicknessD3D;
		else
			return CShape::rPolyPlaneThicknessDefault;
	}


	//**********************************************************************************************
	//
	// CPolyIterator implementation.
	//

		#define uMAX_WATER_QUADS	uint(4000)		// The maximum number of quads in iteration.

		//******************************************************************************************
		CQuadRootWaterShape::CPolyIterator::CPolyIterator(const CQuadRootWaterShape& qnwsh, const CCamera* pcam)
			: qnwshShape(const_cast<CQuadRootWaterShape&>(qnwsh)),
			  dapqnwCurrentQuads(uMAX_WATER_QUADS)
		{
/*
			CClipRegion2D clip2d(*pcam);

			// Calculate the world to quad space transform.
			CPresence3<> pr3_world_to_quad
			(
				CRotate3<>(),
				qntinshShape.mpConversions.tlr2WorldToQuad.tlrX.tScale,
				CVector3<>(qntinshShape.mpConversions.tlr2WorldToQuad.tlrX.tOffset,
						   qntinshShape.mpConversions.tlr2WorldToQuad.tlrY.tOffset, 0)
			);

			clip2d *= pr3_world_to_quad;

			qntinshShape.InitTriangleList(daptriCurrentTris, clip2d, pcam->v3Pos() * pr3_world_to_quad, qntinshShape.rPolyPlaneThickness());
*/

			// Reset iterators.
			ppqnwCurrent = dapqnwCurrentQuads.begin() - 1;
			pqnwCurrent = 0;
		}


		//******************************************************************************************
		CQuadRootWaterShape::CPolyIterator::~CPolyIterator()
		{
			// Todo: handle failed uploads gracefully by i.e. killing the referenced terrain textures.
			// For now we'll assume the upload is always successful.
//TODO: Upload textures here
//			CTextureNode::ptexmTexturePages->bUploadPages();
		}


		//******************************************************************************************
		const CTexture* CQuadRootWaterShape::CPolyIterator::ptexTexture()
		{
			// Compute all the texture coords now.
//TODO: Compute texture coords and get water texture here
//			avtVertices[0].tcTex = ptxn->tcObjectToTexture(ptriCurrent->pqvtGetVertex(0)->iX(), ptriCurrent->pqvtGetVertex(0)->iY());
//			avtVertices[1].tcTex = ptxn->tcObjectToTexture(ptriCurrent->pqvtGetVertex(1)->iX(), ptriCurrent->pqvtGetVertex(1)->iY());
//			avtVertices[2].tcTex = ptxn->tcObjectToTexture(ptriCurrent->pqvtGetVertex(2)->iX(), ptriCurrent->pqvtGetVertex(2)->iY());
//			avtVertices[3].tcTex = ptxn->tcObjectToTexture(ptriCurrent->pqvtGetVertex(2)->iX(), ptriCurrent->pqvtGetVertex(2)->iY());

			return 0;
		}


		//******************************************************************************************
		void CQuadRootWaterShape::CPolyIterator::TransformPoints
		(
			const CTransform3<>&	tf3_shape_camera,
			const CCamera&			cam,
			CPArray<SClipPoint>		paclpt_points,
			bool					b_outcodes
		)
		{
/*
			// Init the entire array's seteocOut to Set(eocUNINIT).
			for (int i = 0; i < paclpt_points.uLen; i++)
				paclpt_points[i].seteocOut = Set(eocUNINIT);

			const CBoundVolCamera* pbvcam = cam.pbvcamClipVolume();


			// Get quad to camera transform, including negated lifted dequantisation scalar for Z axis.
			CTransform3<> tf3_shapequad_cam = CScale3<>(1, 1, -qntinshShape.mpConversions.rCoefToQuad / 8) *
			                                  tf3_shape_camera;


			for (CTriangleTIN** pptri_curr = daptriCurrentTris; pptri_curr < daptriCurrentTris.end(); pptri_curr++)
			{
				CTriangleTIN* ptri_curr = *pptri_curr;

				for (int i_v = 0; i_v < 3; i_v++)
				{
					const CQuadVertexTIN* pqvt = ptri_curr->pqvtGetVertex(i_v);
					int i_index = pqvt->iGetHandle();

					// Check if the vertex has already been transformed.
					if (paclpt_points[i_index].seteocOut[eocUNINIT])
					{
						// Transform point, and generate the outcode.
						CVector3<> v3_vt(pqvt->iX(), pqvt->iY(), pqvt->cfNegUnliftedScaling().iGet());

						paclpt_points[i_index].v3Point   = v3_vt * tf3_shapequad_cam;
						paclpt_points[i_index].seteocOut = pbvcam->seteocOutCodes(paclpt_points[i_index].v3Point);
					}
				}
			}
*/
		}
};

//#if VER_MULTI_RES_WATER
#endif