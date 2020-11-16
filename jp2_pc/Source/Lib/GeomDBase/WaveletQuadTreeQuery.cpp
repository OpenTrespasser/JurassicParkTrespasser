/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of WaveletQuadTreeQuery.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTreeQuery.cpp                                $
 * 
 * 46    9/25/98 2:22p Mlange
 * Now rejects very short ray casts.
 * 
 * 45    9/23/98 7:41p Mlange
 * Wavelet quad tree nodes now have a single pointer to the first descendant and a pointer to
 * the next sibling, instead of four pointers for each of the descendants.
 * 
 * 44    98/09/10 1:13 Speter
 * Fixed bug in height query; using iFloatCast instead of proper iTrunc caused query failure.
 * 
 * 43    98/09/08 13:08 Speter
 * Added esfEdgeIntersects().
 * 
 * 42    9/07/98 5:16p Mlange
 * Fixed bug - abs() was used where fabs() should have been instead.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "WaveletQuadTreeQuery.hpp"

#include "Lib/GeomDBase/WaveletStaticData.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/Sys/Textout.hpp"

#include "Lib/Physics/PhysicsStats.hpp"

namespace NMultiResolution
{


//**********************************************************************************************
//
// NMultiResolution::CQuadVertexQuery implementation.
//

	//******************************************************************************************
	CBlockAllocator<CQuadVertexQuery>::SStore CQuadVertexQuery::stStore;


//**********************************************************************************************
//
// NMultiResolution::CTriangleQuery implementation.
//

	//******************************************************************************************
	CBlockAllocator<CTriangleQuery>::SStore CTriangleQuery::stStore;

	//******************************************************************************************
	float CTriangleQuery::fRayIntersects(CVector3<>& rv3_i, const CVector3<>& v3_ray_world_start, const CVector3<>& v3_ray_world_diff, const SMapping& mp)
	{
		Init(mp);

		float f_denom = v3_ray_world_diff * plPlane.d3Normal;

		// If the denominator is (close to) zero, the ray and plane are parallel.
		if (fabs(f_denom) < .001)
			return -1;

		float f_numer = -(v3_ray_world_start * plPlane.d3Normal + plPlane.rD);

		if (f_denom < 0)
		{
			f_numer = -f_numer;
			f_denom = -f_denom;
		}

		// Determine if the fraction is within the range [0..1].
		if (f_numer < 0 || CIntFloat(f_numer).i4Int > CIntFloat(f_denom).i4Int)
			return -1;

		float f_t = f_numer / f_denom;

		// Calculate the point of intersection and determine if it lies in this triangle.
		rv3_i.tX = v3_ray_world_diff.tX * f_t + v3_ray_world_start.tX;
		rv3_i.tY = v3_ray_world_diff.tY * f_t + v3_ray_world_start.tY;
		rv3_i.tZ = v3_ray_world_diff.tZ * f_t + v3_ray_world_start.tZ;

		const float f_tol = .01f;

		if (!CIntFloat((rv3_i.tX - av3Corners[0].tX) * (av3Corners[1].tY - av3Corners[0].tY) -
		               (rv3_i.tY - av3Corners[0].tY) * (av3Corners[1].tX - av3Corners[0].tX) - f_tol).bSign())
			return -1;

		if (!CIntFloat((rv3_i.tX - av3Corners[1].tX) * (av3Corners[2].tY - av3Corners[1].tY) -
		               (rv3_i.tY - av3Corners[1].tY) * (av3Corners[2].tX - av3Corners[1].tX) - f_tol).bSign())
			return -1;

		if (!CIntFloat((rv3_i.tX - av3Corners[2].tX) * (av3Corners[0].tY - av3Corners[2].tY) -
		               (rv3_i.tY - av3Corners[2].tY) * (av3Corners[0].tX - av3Corners[2].tX) - f_tol).bSign())
			return -1;

		return f_t;
	}


//**********************************************************************************************
//
// NMultiResolution::CTriNodeInfoQuery implementation.
//

	//******************************************************************************************
	CBlockAllocator<CTriNodeInfoQuery>::SStore CTriNodeInfoQuery::stStore;



//**********************************************************************************************
//
// NMultiResolution::CQuadNodeQuery implementation.
//

	//******************************************************************************************
	CBlockAllocator<CQuadNodeQuery>::SStore CQuadNodeQuery::stStore;


	//******************************************************************************************
	std::pair<TReal, TReal> CQuadNodeQuery::prrGetWorldZLimits(const CQuadRootQuery* pqnq_root)
	{
		CTriNodeInfoQuery* ptinf = ptinfGetTriangleInfo();

		if (ptinf->rMinZ == FLT_MAX)
		{
			CCoef cf_min, cf_max;

			CTriangleQuery* ptri_curr = ptriGetFirst();

			if (stState[estWAVELET_1_SIGNIFICANT])
			{
				cf_min = ptri_curr->pqvtGetVertex(0)->cfScaling();
				cf_max = cf_min;

				do
				{
					cf_min = Min(cf_min, ptri_curr->pqvtGetVertex(1)->cfScaling());
					cf_max = Max(cf_max, ptri_curr->pqvtGetVertex(1)->cfScaling());

					ptri_curr = ptri_curr->ptriGetNext();
				}
				while (ptri_curr != 0);
			}
			else
			{
				if (stState[estDISC_DIAGONAL_1_3])
				{
					cf_min = Min(pqvtGetVertex(0)->cfScaling(), pqvtGetVertex(2)->cfScaling());
					cf_max = Max(pqvtGetVertex(0)->cfScaling(), pqvtGetVertex(2)->cfScaling());
				}
				else
				{
					cf_min = Min(pqvtGetVertex(1)->cfScaling(), pqvtGetVertex(3)->cfScaling());
					cf_max = Max(pqvtGetVertex(1)->cfScaling(), pqvtGetVertex(3)->cfScaling());
				}

				do
				{
					cf_min = Min(cf_min, ptri_curr->pqvtGetVertex(0)->cfScaling());
					cf_min = Min(cf_min, ptri_curr->pqvtGetVertex(2)->cfScaling());
					cf_max = Max(cf_max, ptri_curr->pqvtGetVertex(0)->cfScaling());
					cf_max = Max(cf_max, ptri_curr->pqvtGetVertex(2)->cfScaling());

					ptri_curr = ptri_curr->ptriGetNext();
				}
				while (ptri_curr != 0);
			}

			ptinf->rMinZ = cf_min.rGet(pqnq_root->mpConversions.rCoefToWorld);
			ptinf->rMaxZ = cf_max.rGet(pqnq_root->mpConversions.rCoefToWorld);
		}

		return std::pair<TReal, TReal>(ptinf->rMinZ, ptinf->rMaxZ);
	}

	//******************************************************************************************
	//
	struct CQuadNodeQuery::SRayIterateContext
	//
	// Prefix: ric
	//
	// Used in the recursive ray intersect functions, for efficiency of recursion.
	//
	//******************************
	{
		SVolumeLoc* pvlInit;				// Structure to init with intersection info.

		const CQuadRootQuery* pqnqRoot;		// Root node of the quad tree.

		CVector3<> v3RayWorldStart;
		CVector3<> v3RayWorldDelta;

		CVector2<> v2RayQuadA;
		CVector2<> v2RayQuadB;
		CVector2<> v2RayQuadD;

		float fRayWorldLength;
		float fRayQuadLength;
		float fRayQuadInvLength;

		CVector2<int> v2RayAdjQuadA;
		CVector2<int> v2RayAdjQuadB;

		CVector2<int> v2RayAdjQuadMin;
		CVector2<int> v2RayAdjQuadMax;

		CCoef cfMaxRayHighPass;


		//******************************************************************************************
		SRayIterateContext
		(
			const CQuadRootQuery* pqnq_root,
			const CVector3<>& v3_ray_world_start,
			const CVector3<>& v3_ray_world_end,
			SVolumeLoc* pvl_init,
			CCoef cf_max_ray_high_pass
		) :
			pqnqRoot(pqnq_root),
			v3RayWorldStart(v3_ray_world_start),
			v3RayWorldDelta(v3_ray_world_end - v3_ray_world_start),
			pvlInit(pvl_init),
			cfMaxRayHighPass(cf_max_ray_high_pass)
		{
			v2RayQuadA = CVector2<>(v3_ray_world_start) * pqnq_root->mpConversions.tlr2WorldToQuad;
			v2RayQuadB = CVector2<>(v3_ray_world_end)   * pqnq_root->mpConversions.tlr2WorldToQuad;
			v2RayQuadD = v2RayQuadB - v2RayQuadA;

			fRayWorldLength   = v3RayWorldDelta.tLen();
			fRayQuadLength    = v2RayQuadD.tLen();
			fRayQuadInvLength = 1.0f / fRayQuadLength;

			if (v2RayQuadA.tX < v2RayQuadB.tX)
			{
				v2RayAdjQuadA.tX = int(v2RayQuadA.tX - .5f);
				v2RayAdjQuadB.tX = int(v2RayQuadB.tX + .5f);
			}
			else
			{
				v2RayAdjQuadA.tX = int(v2RayQuadA.tX + .5f);
				v2RayAdjQuadB.tX = int(v2RayQuadB.tX - .5f);
			}

			if (v2RayQuadA.tY < v2RayQuadB.tY)
			{
				v2RayAdjQuadA.tY = int(v2RayQuadA.tY - .5f);
				v2RayAdjQuadB.tY = int(v2RayQuadB.tY + .5f);
			}
			else
			{
				v2RayAdjQuadA.tY = int(v2RayQuadA.tY + .5f);
				v2RayAdjQuadB.tY = int(v2RayQuadB.tY - .5f);
			}

			v2RayAdjQuadMin.tX = Min(v2RayAdjQuadA.tX, v2RayAdjQuadB.tX);
			v2RayAdjQuadMin.tY = Min(v2RayAdjQuadA.tY, v2RayAdjQuadB.tY);
			v2RayAdjQuadMax.tX = Max(v2RayAdjQuadA.tX, v2RayAdjQuadB.tX);
			v2RayAdjQuadMax.tY = Max(v2RayAdjQuadA.tY, v2RayAdjQuadB.tY);
		}
	};


	//******************************************************************************************
	void CQuadNodeQuery::RayEvaluateBranch(const SRayIterateContext& ric)
	{
		// Determine if the influence rectangle of this node intersects the bounding rectangle of the ray.
		int i_accum  = (ric.v2RayAdjQuadMax.tX - (pqvtGetVertex(0)->iX() - iGetSize()));
		    i_accum |= (ric.v2RayAdjQuadMax.tY - (pqvtGetVertex(0)->iY() - iGetSize()));
		    i_accum |= ((pqvtGetVertex(2)->iX() + iGetSize()) - ric.v2RayAdjQuadMin.tX);
			i_accum |= ((pqvtGetVertex(2)->iY() + iGetSize()) - ric.v2RayAdjQuadMin.tY);

		bool b_in_influence = false;

		if (i_accum >= 0)
		{
			CIntFloat if_side_a, if_side_b;

			float f_size_adj = iGetSize() + .01f;

			// Determine if the ray intersects the influence rectangle of this node. Depending on the orientation
			// of the ray, take two corners opposite a diagonal and check to which side of the ray they lie.
			if (int(uint(CIntFloat(ric.v2RayQuadD.tX).i4Int) ^ uint(CIntFloat(ric.v2RayQuadD.tY).i4Int)) < 0)
			{
				// Check if the node's influence rectangle's bottom left and top right corners lie to
				// different sides of the ray.
				if_side_a = ((pqvtGetVertex(0)->iX() - f_size_adj) - ric.v2RayQuadA.tX) * ric.v2RayQuadD.tY -
				            ((pqvtGetVertex(0)->iY() - f_size_adj) - ric.v2RayQuadA.tY) * ric.v2RayQuadD.tX;

				if_side_b = ((pqvtGetVertex(2)->iX() + f_size_adj) - ric.v2RayQuadA.tX) * ric.v2RayQuadD.tY -
				            ((pqvtGetVertex(2)->iY() + f_size_adj) - ric.v2RayQuadA.tY) * ric.v2RayQuadD.tX;
			}
			else
			{
				// Check if the node's influence rectangle's bottom right and top left corners lie to
				// different sides of the ray.
				if_side_a = ((pqvtGetVertex(3)->iX() - f_size_adj) - ric.v2RayQuadA.tX) * ric.v2RayQuadD.tY -
				            ((pqvtGetVertex(3)->iY() + f_size_adj) - ric.v2RayQuadA.tY) * ric.v2RayQuadD.tX;

				if_side_b = ((pqvtGetVertex(1)->iX() + f_size_adj) - ric.v2RayQuadA.tX) * ric.v2RayQuadD.tY -
				            ((pqvtGetVertex(1)->iY() - f_size_adj) - ric.v2RayQuadA.tY) * ric.v2RayQuadD.tX;
			}

			b_in_influence = int(uint(if_side_a.i4Int) ^ uint(if_side_b.i4Int)) < 0;
		}

		stState[estSIGNIFICANT] = stState[estSIGNIFICANT] || b_in_influence;

		if (b_in_influence)
		{
			if (!bHasDescendants() && tdData.cfGetMaxRegion() > ric.cfMaxRayHighPass)
			{
				Subdivide(ric.pqnqRoot);

				// Set the wavelet coeficient significance flag for all wavelet coeficients exceeding the cutoff frequency.
				SetWaveletStates
				(
					ptqnGetFirstDescendant()->pqvtGetVertex(1)->cfWavelet().cfAbs() > ric.cfMaxRayHighPass,
					ptqnGetFirstDescendant()->pqvtGetVertex(2)->cfWavelet().cfAbs() > ric.cfMaxRayHighPass,
					ptqnGetFirstDescendant()->pqvtGetVertex(3)->cfWavelet().cfAbs() > ric.cfMaxRayHighPass
				);
			}


			CQuadNodeQuery* pqnq_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				if (pqnq_dsc->tdData.cfGetMaxRegion() > ric.cfMaxRayHighPass)
				{
					pqnq_dsc->RayEvaluateBranch(ric);
				}

				pqnq_dsc = pqnq_dsc->ptqnGetSibling();
			}
		}
	}

	//******************************************************************************************
	bool CQuadNodeQuery::bRayIntersectsBranch(SRayIterateContext& rric)
	{
		if (!stState[estLEAF][estLEAF_COMBINE])
		{
			//
			// This node contains no triangles. Determine which of its descendants the ray intersects, and
			// recurse these in order, from the start toward the end point of the ray.
			//
			// First, we determine the descendants the ray *potentially* intersects, based on its intersection
			// with the node's infinite middle horizontal and vertical.
			//

			// Get the midpoint of this node.
			CVector2<int> v2_mid(iBaseX() + (iGetSize() >> 1), iBaseY() + (iGetSize() >> 1));

			// Obtain some flags that determine to which side of the infinite middle vertical and
			// horizontal the endpoints of the ray lie.
			bool b_ray_a_rt_vert = uint(v2_mid.tX - rric.v2RayAdjQuadA.tX) >> (sizeof(int) * 8 - 1);
			bool b_ray_b_rt_vert = uint(v2_mid.tX - rric.v2RayAdjQuadB.tX) >> (sizeof(int) * 8 - 1);

			bool b_ray_a_up_horz = uint(v2_mid.tY - rric.v2RayAdjQuadA.tY) >> (sizeof(int) * 8 - 1);
			bool b_ray_b_up_horz = uint(v2_mid.tY - rric.v2RayAdjQuadB.tY) >> (sizeof(int) * 8 - 1);

			// Determine if the midpoint of this node lies to left or right of the ray. This determines if we
			// need to recurse the descendants in clock- or counter-clockwise order.
			float f_d_mid = (v2_mid.tX - rric.v2RayQuadA.tX) * rric.v2RayQuadD.tY -
							(v2_mid.tY - rric.v2RayQuadA.tY) * rric.v2RayQuadD.tX;

			int i_ray_left_mid = CIntFloat(f_d_mid).bSign();

			int i_dsc_incr  = -1 + i_ray_left_mid * 2;

			// Handle the special cases where the ray start point lies on the (infinite) middle vertical or horizontal.
			if (rric.v2RayAdjQuadA.tX == v2_mid.tX || rric.v2RayAdjQuadA.tY == v2_mid.tY)
			{
				float f_d_mid_adj = float(v2_mid.tX - rric.v2RayAdjQuadA.tX) * (rric.v2RayAdjQuadB.tY - rric.v2RayAdjQuadA.tY) -
									float(v2_mid.tY - rric.v2RayAdjQuadA.tY) * (rric.v2RayAdjQuadB.tX - rric.v2RayAdjQuadA.tX);

				i_dsc_incr = -1 + CIntFloat(f_d_mid_adj).bSign() * 2;

				// To avoid any accuracy problems, make sure we recurse all descendants that the ray potentially
				// intersects.
				i_ray_left_mid = -1;

				// Always consider the ray to intersect in these special cases.
				if (rric.v2RayAdjQuadA.tX == v2_mid.tX)
					b_ray_a_rt_vert = !b_ray_b_rt_vert;

				if (rric.v2RayAdjQuadA.tY == v2_mid.tY)
					b_ray_a_up_horz = !b_ray_b_up_horz;
			}

			// Determine the first and subsequent descendants the ray (potentially) intersects.
			int i_dsc       = (b_ray_a_up_horz << 1) | (b_ray_a_rt_vert ^ b_ray_a_up_horz);
			int i_dsc_count = 1;

			if (b_ray_a_rt_vert != b_ray_b_rt_vert)
				i_dsc_count++;

			if (b_ray_a_up_horz != b_ray_b_up_horz)
				i_dsc_count++;

			const float f_tol = .01f;

			float f_dist_ray_mid = fabs(f_d_mid * rric.fRayQuadInvLength);

			// Handle special case where the ray intersects the midpoint of node, in which case
			// we recurse all the descendants.
			if (i_dsc_count == 3 && CIntFloat(f_dist_ray_mid).i4Int < CIntFloat(f_tol).i4Int)
			{
				int i_dsc_incr_incr = i_dsc_incr;

				for (int i = 0; i < 4; i++)
				{
					if (ptqnGetDescendant(i_dsc)->bRayIntersectsBranch(rric))
						return true;

					i_dsc      += i_dsc_incr;
					i_dsc_incr += i_dsc_incr_incr;
				}
			}
			else
			{
				// For each descendant, accurately determine if the ray intersects it.
				float f_adj = f_tol * rric.fRayQuadLength;

				if (i_ray_left_mid)
					f_adj = -f_adj;

				if (CIntFloat(f_dist_ray_mid).i4Int < CIntFloat(f_tol).i4Int)
					i_ray_left_mid = -1;

				const CQuadVertexQuery* pqvtq = ptqnGetDescendant(i_dsc)->pqvtGetVertex(i_dsc - i_dsc_incr);
				int i_ray_left_prev = CIntFloat((pqvtq->iX() - rric.v2RayQuadA.tX) * rric.v2RayQuadD.tY -
				                                (pqvtq->iY() - rric.v2RayQuadA.tY) * rric.v2RayQuadD.tX - f_adj).bSign();

				for (int i = 0; i < i_dsc_count; i++)
				{
					pqvtq = ptqnGetDescendant(i_dsc)->pqvtGetVertex(i_dsc + i_dsc_incr);
					int i_ray_left_next = CIntFloat((pqvtq->iX() - rric.v2RayQuadA.tX) * rric.v2RayQuadD.tY -
					                                (pqvtq->iY() - rric.v2RayQuadA.tY) * rric.v2RayQuadD.tX - f_adj).bSign();

					bool b_recurse = (i_ray_left_prev != i_ray_left_mid) || (i_ray_left_next != i_ray_left_mid);

					if (!b_recurse)
						b_recurse = (int)CIntFloat((pqvtGetVertex(i_dsc)->iX() - rric.v2RayQuadA.tX) * rric.v2RayQuadD.tY -
											       (pqvtGetVertex(i_dsc)->iY() - rric.v2RayQuadA.tY) * rric.v2RayQuadD.tX - f_adj).bSign() != i_ray_left_mid;

					if (b_recurse)
						if (ptqnGetDescendant(i_dsc)->bRayIntersectsBranch(rric))
							return true;

					i_dsc += i_dsc_incr;

					i_ray_left_prev = i_ray_left_next;
				}
			}

			return false;
		}
		else
		{
			CTriangleQuery* ptri_curr = ptriGetFirst();

			Assert(ptri_curr != 0);

			CIntFloat if_t_nearest = FLT_MAX;

			do
			{
				CVector3<> v3_i;
				CIntFloat if_t_ray = ptri_curr->fRayIntersects(v3_i, rric.v3RayWorldStart, rric.v3RayWorldDelta, rric.pqnqRoot->mpConversions);

				if (if_t_ray.i4Int != CIntFloat(-1.0f).i4Int && if_t_ray.i4Int < if_t_nearest.i4Int)
				{
					if_t_nearest = if_t_ray;

					rric.pvlInit->v3Location = v3_i;
					rric.pvlInit->d3Face     = ptri_curr->plGetPlaneWorld().d3Normal;
					rric.pvlInit->rDist      = if_t_ray.fFloat * rric.fRayWorldLength;
				}

				ptri_curr = ptri_curr->ptriGetNext();
			}
			while (ptri_curr != 0);

			return if_t_nearest.i4Int != CIntFloat(FLT_MAX).i4Int;
		}
	}


	//******************************************************************************************
	void CQuadNodeQuery::AlignQueryExtent(CQueryRect& qr)
	{
		int i_accum  = (qr.rcExtent.tX1() - pqvtGetVertex(0)->iX()) | (pqvtGetVertex(2)->iX() - qr.rcExtent.tX0());
		    i_accum |= (qr.rcExtent.tY1() - pqvtGetVertex(0)->iY()) | (pqvtGetVertex(2)->iY() - qr.rcExtent.tY0());

		bool b_intersects = i_accum > 0;

		stState[estSIGNIFICANT] = stState[estSIGNIFICANT] || b_intersects;

		if (b_intersects)
		{
			if (!bHasDescendants())
			{
				CCoef cf_high_pass = qr.cfMaxRegionHighPass;

				if (qr.bFilterAsRatio)
				{
					Assert(int64(qr.cfMaxRegionHighPass.iGet()) * iGetSize() <= TypeMax(int));

					cf_high_pass = qr.cfMaxRegionHighPass.iGet() * iGetSize();
				}

				if (tdData.cfGetMaxRegion() > cf_high_pass)
				{
					Subdivide(qr.pqnqRoot);

					// Set the wavelet coeficient significance flag for all wavelet coeficients exceeding the cutoff frequency.
					SetWaveletStates
					(
						ptqnGetFirstDescendant()->pqvtGetVertex(1)->cfWavelet().cfAbs() > cf_high_pass,
						ptqnGetFirstDescendant()->pqvtGetVertex(2)->cfWavelet().cfAbs() > cf_high_pass,
						ptqnGetFirstDescendant()->pqvtGetVertex(3)->cfWavelet().cfAbs() > cf_high_pass
					);
				}
			}

			if (bHasDescendants())
			{
				CQuadNodeQuery* pqnq_dsc = ptqnGetFirstDescendant();

				for (int i_dsc = 0; i_dsc < 4; i_dsc++)
				{
					pqnq_dsc->AlignQueryExtent(qr);

					pqnq_dsc = pqnq_dsc->ptqnGetSibling();
				}
			}

			if (stState[estLEAF][estLEAF_COMBINE])
				qr.rcRefinedExtent |= CRectangle<int>(iBaseX(), iBaseY(), iGetSize(), iGetSize());
		}
	}


	//******************************************************************************************
	void CQuadNodeQuery::EvaluateQueryBranch(const CQueryRect& qr)
	{
		int i_accum  = (qr.rcRefinedExtent.tX1() - (pqvtGetVertex(0)->iX() - iGetSize()));
		    i_accum |= (qr.rcRefinedExtent.tY1() - (pqvtGetVertex(0)->iY() - iGetSize()));
		    i_accum |= ((pqvtGetVertex(2)->iX() + iGetSize()) - qr.rcRefinedExtent.tX0());
			i_accum |= ((pqvtGetVertex(2)->iY() + iGetSize()) - qr.rcRefinedExtent.tY0());

		bool b_in_influence = i_accum >= 0;

		stState[estSIGNIFICANT] = stState[estSIGNIFICANT] || b_in_influence;

		if (b_in_influence)
		{
			CCoef cf_high_pass = qr.cfMaxRegionHighPass;

			if (qr.bFilterAsRatio)
			{
				Assert(int64(qr.cfMaxRegionHighPass.iGet()) * iGetSize() <= TypeMax(int));

				cf_high_pass = qr.cfMaxRegionHighPass.iGet() * iGetSize();
			}

			Assert(tdData.cfGetMaxRegion() > cf_high_pass);

			if (!bHasDescendants())
			{
				Subdivide(qr.pqnqRoot);

 				// Set the wavelet coeficient significance flag for all wavelet coeficients exceeding the cutoff frequency.
				SetWaveletStates
				(
					ptqnGetFirstDescendant()->pqvtGetVertex(1)->cfWavelet().cfAbs() > cf_high_pass,
					ptqnGetFirstDescendant()->pqvtGetVertex(2)->cfWavelet().cfAbs() > cf_high_pass,
					ptqnGetFirstDescendant()->pqvtGetVertex(3)->cfWavelet().cfAbs() > cf_high_pass
				);
			}


			CQuadNodeQuery* pqnq_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				if (pqnq_dsc->tdData.cfGetMaxRegion() > cf_high_pass)
				{
					pqnq_dsc->EvaluateQueryBranch(qr);
				}

				pqnq_dsc = pqnq_dsc->ptqnGetSibling();
			}
		}
	}


	//******************************************************************************************
	void CQuadNodeQuery::IterateBranch(CQueryRect& qr) const
	{
		int i_accum  = (qr.rcExtent.tX1() - pqvtGetVertex(0)->iX()) | (pqvtGetVertex(2)->iX() - qr.rcExtent.tX0());
		    i_accum |= (qr.rcExtent.tY1() - pqvtGetVertex(0)->iY()) | (pqvtGetVertex(2)->iY() - qr.rcExtent.tY0());

		bool b_intersects_inclusive = i_accum >= 0;

		if (b_intersects_inclusive)
		{
			if (!stState[estLEAF][estLEAF_COMBINE])
			{
				CQuadNodeQuery* pqnq_dsc = ptqnGetFirstDescendant();

				for (int i_dsc = 0; i_dsc < 4; i_dsc++)
				{
					pqnq_dsc->IterateBranch(qr);

					pqnq_dsc = pqnq_dsc->ptqnGetSibling();
				}
			}
			else
			{
				Assert(stState[estSIGNIFICANT]);
				Assert(qr.rcRefinedExtent.bContains(CRectangle<int>(iBaseX(), iBaseY(), iGetSize(), iGetSize())));

				qr.dapqnqIterateStore << const_cast<CQuadNodeQuery*>(this);
			}
		}
	}


	//******************************************************************************************
	bool CQuadNodeQuery::bDecimateTree()
	{
		bool b_decimated = false;

		if (bHasDescendants())
		{
			//
			// Determine the significance of this node. If it is not significant, it is no longer needed
			// and we can delete this branch of the tree. If it is significant, we continue by recursing
			// down the descendants of this node.
			//
			if (!stState[estSIGNIFICANT])
			{
				DecimateBranch();
				b_decimated = true;
			}
			else
			{
				CQuadNodeQuery* pqnq_dsc = ptqnGetFirstDescendant();

				for (int i_dsc = 0; i_dsc < 4; i_dsc++)
				{
					b_decimated |= pqnq_dsc->bDecimateTree();

					pqnq_dsc = pqnq_dsc->ptqnGetSibling();
				}
			}
		}

		// We have processed this branch of the tree. Force this node to be insignificant so that it may
		// be deleted in the next iteration of this function.
		stState[estSIGNIFICANT] = false;

		return b_decimated;
	}



//**********************************************************************************************
//
// NMultiResolution::CQuadRootQuery implementation.
//

	//******************************************************************************************
	CQuadRootQuery::CQuadRootQuery(const CTransformedDataHeader* ptdh)
		: CQuadRootBaseTriT<CQuadNodeQuery, CQuadVertexQuery, CTriNodeInfoQuery, CTriangleQuery>
		  (
			ptdh,
			new CQuadVertexQuery(ptdh->mpConversions.rcQuadSpaceExtents.tX0(), ptdh->mpConversions.rcQuadSpaceExtents.tY0(), ptdh->mpConversions.rcQuadSpaceExtents.tWidth(), ptdh->cfRoot),
			new CQuadVertexQuery(ptdh->mpConversions.rcQuadSpaceExtents.tX1(), ptdh->mpConversions.rcQuadSpaceExtents.tY0(), ptdh->mpConversions.rcQuadSpaceExtents.tWidth(), ptdh->cfRoot),
			new CQuadVertexQuery(ptdh->mpConversions.rcQuadSpaceExtents.tX1(), ptdh->mpConversions.rcQuadSpaceExtents.tY1(), ptdh->mpConversions.rcQuadSpaceExtents.tWidth(), ptdh->cfRoot),
			new CQuadVertexQuery(ptdh->mpConversions.rcQuadSpaceExtents.tX0(), ptdh->mpConversions.rcQuadSpaceExtents.tY1(), ptdh->mpConversions.rcQuadSpaceExtents.tWidth(), ptdh->cfRoot)
		  ),
		  mpConversions(ptdh->mpConversions)
	{
		Assert(CQuadNodeQuery::pfhGetFastHeap() != 0 && CQuadVertexQuery::pfhGetFastHeap() != 0);
		Assert(CTriangleQuery::pfhGetFastHeap() != 0 && CTriNodeInfoQuery::pfhGetFastHeap() != 0);
	}


	//******************************************************************************************
	CQuadRootQuery::~CQuadRootQuery()
	{
		// Make sure there are no query regions using this tree.
		AlwaysAssert(ltpqrQueries.size() == 0);

		// Call the decimate function twice to delete all nodes in the tree.
		bDecimateTree();
		bDecimateTree();
	}


	//******************************************************************************************
	void CQuadRootQuery::AllocMemory(uint u_max_nodes)
	{
		Assert(CQuadNodeQuery::pfhGetFastHeap() == 0 &&  CQuadVertexQuery::pfhGetFastHeap() == 0);
		Assert(CTriangleQuery::pfhGetFastHeap() == 0 && CTriNodeInfoQuery::pfhGetFastHeap() == 0);

		// Allocate and set the fastheap used for allocations of the query quad tree classes.
		uint u_max_node_bytes     = sizeof(   CQuadNodeQuery) * u_max_nodes;
		uint u_max_vert_bytes     = sizeof( CQuadVertexQuery) * u_max_nodes * dAVG_QUAD_NODE_TO_VERTEX_RATIO;
		uint u_max_tri_bytes      = sizeof(   CTriangleQuery) * u_max_nodes * dAVG_QUAD_NODE_TO_TRIANGLE_RATIO;
		uint u_max_tri_info_bytes = sizeof(CTriNodeInfoQuery) * u_max_nodes * dAVG_QUAD_NODE_TO_TRIANGLE_INFO_RATIO;

		CFastHeap* pfh = ::new CFastHeap(u_max_node_bytes + u_max_vert_bytes + u_max_tri_bytes + u_max_tri_info_bytes);

		   CQuadNodeQuery::SetFastHeap(pfh);
		 CQuadVertexQuery::SetFastHeap(pfh);
		   CTriangleQuery::SetFastHeap(pfh);
	    CTriNodeInfoQuery::SetFastHeap(pfh);
	}


	//**********************************************************************************************
	void CQuadRootQuery::FreeMemory()
	{
		// Delete the memory used for allocations of the query quad tree classes.
		CFastHeap* pfh = CQuadNodeQuery::pfhGetFastHeap();

		Assert(CQuadVertexQuery::pfhGetFastHeap() == pfh);
		Assert(  CTriangleQuery::pfhGetFastHeap() == pfh && CTriNodeInfoQuery::pfhGetFastHeap() == pfh);

		   CQuadNodeQuery::SetFastHeap(0);
		 CQuadVertexQuery::SetFastHeap(0);
		   CTriangleQuery::SetFastHeap(0);
	    CTriNodeInfoQuery::SetFastHeap(0);

		delete pfh;
	}


	//******************************************************************************************
	void CQuadRootQuery::Update()
	{
		if (!bAverageStats)
			psWaveletQuery.Reset();

		CCycleTimer ctmr;

		if (bDecimateTree())
		{
			for (std::list<CQueryRect*>::iterator it = ltpqrQueries.begin(); it != ltpqrQueries.end(); ++it)
				(*it)->SetEvaluateReq();
		}

		CQuadRootQuery::psUpdate.Add(ctmr(), 1);
	}

	//******************************************************************************************
	TReal CQuadRootQuery::rHeight(TReal r_x, TReal r_y, const CPlane** pppl) const
	{
		CVector2<> v2_quad = CVector2<>(r_x, r_y) * mpConversions.tlr2WorldToQuad;

		CTriangleQuery* ptri_enc = ptriFindEnclosingTriangle(v2_quad.tX, v2_quad.tY);

		// Trap height queries outside the bounds of the terrain.
		if (ptri_enc == 0)
			return 0;

		ptri_enc->Init(mpConversions);
		*pppl = &ptri_enc->plGetPlaneWorld();

		return ptri_enc->rHeight(r_x, r_y);
	}



	//******************************************************************************************
	bool CQuadRootQuery::bRayIntersects(SVolumeLoc* pvl_init, const CVector3<>& v3_ray_world_start, const CVector3<>& v3_ray_world_end)
	{
		CCycleTimer ctmr;

		// Reject very short rays.
		const float fRAY_MIN_LENGTH = .001f;

		if ((v3_ray_world_end - v3_ray_world_start).tLenSqr() < Sqr(fRAY_MIN_LENGTH))
			return false;

		CQuadNodeQuery::SRayIterateContext ric(this, v3_ray_world_start, v3_ray_world_end, pvl_init, cfZERO);

		CCycleTimer ctmr2;
		RayEvaluateBranch(ric);
		psRayRefine.Add(ctmr2());

		CCycleTimer ctmr3;
		bool b_intersects = bRayIntersectsBranch(ric);
		psRayRefine.Add(ctmr3());

		psRayEvaluate.Add(ctmr());

		return b_intersects;
	}


	//******************************************************************************************
	void CQuadRootQuery::PrintStats(CConsoleBuffer& con) const
	{
		con.Print("Query quad tree  Curr            Max\n");

		CQuadRootBaseTriT<CQuadNodeQuery, CQuadVertexQuery, CTriNodeInfoQuery, CTriangleQuery>::PrintStats(con);

		con.Print
		(
			"Mem commit: %dKB, free: %dKB, reserved: %dKB\n",
			(CQuadNodeQuery::pfhGetFastHeap()->uNumBytesUsed() + 512) / 1024,
		    (CQuadNodeQuery::pfhGetFastHeap()->uNumBytesFree() + 512) / 1024,
			(CQuadNodeQuery::pfhGetFastHeap()->uGetSize()      + 512) / 1024
		);
	}



//**********************************************************************************************
//
// NMultiResolution::CQueryRect::CEdgeIterator implementation.
//

	//******************************************************************************************
	void CQueryRect::CEdgeIterator::Reset()
	{
		uCurrNodeIndex = 0;
		uCurrEdgeIndex = 2;

		bInternal    = true;
		bTriFan      = pqrContainer->dapqnqIterateStore[0]->stState[CQuadNodeQuery::estWAVELET_1_SIGNIFICANT];
		bNodeDisc1_3 = pqrContainer->dapqnqIterateStore[0]->stState[CQuadNodeQuery::estDISC_DIAGONAL_1_3];
		ptriCurr     = pqrContainer->dapqnqIterateStore[0]->ptriGetFirst();
	}

	//*****************************************************************************************
	void CQueryRect::CEdgeIterator::NextQuadNode()
	{
		Assert(uCurrNodeIndex < pqrContainer->dapqnqIterateStore.uLen);

		uCurrNodeIndex++;

		if (uCurrNodeIndex < pqrContainer->dapqnqIterateStore.uLen)
		{
			uCurrEdgeIndex = 2;

			bInternal    = true;
			bTriFan      = pqrContainer->dapqnqIterateStore[uCurrNodeIndex]->stState[CQuadNodeQuery::estWAVELET_1_SIGNIFICANT];
			bNodeDisc1_3 = pqrContainer->dapqnqIterateStore[uCurrNodeIndex]->stState[CQuadNodeQuery::estDISC_DIAGONAL_1_3];
			ptriCurr     = pqrContainer->dapqnqIterateStore[uCurrNodeIndex]->ptriGetFirst();
		}
		else
			ptriCurr = 0;
	}

	//*****************************************************************************************
	void CQueryRect::CEdgeIterator::operator ++()
	{
		Assert(ptriCurr != 0 && uCurrNodeIndex < pqrContainer->dapqnqIterateStore.uLen);

		if (bInternal)
		{
			// Iterate internal edges.
			ptriCurr = ptriCurr->ptriGetNext();

			if (ptriCurr == 0 || (ptriCurr->ptriGetNext() == 0 && !bTriFan))
			{
				// If done, reset to first triangle in node and iterate boundary edges.
				ptriCurr  = pqrContainer->dapqnqIterateStore[uCurrNodeIndex]->ptriGetFirst();
				bInternal = false;

				if (!bTriFan && !bNodeDisc1_3)
					uCurrEdgeIndex = 0;
				else
					uCurrEdgeIndex = 1;
			}
		}
		else
		{
			if (bTriFan)
			{
				ptriCurr = ptriCurr->ptriGetNext();

				// Iterate bottom and right boundary edges only.
				if (ptriCurr->pqvtGetVertex(1) == pqrContainer->dapqnqIterateStore[uCurrNodeIndex]->pqvtGetVertex(2))
					NextQuadNode();
			}
			else
			{
				if (!bNodeDisc1_3)
				{
					do
					{
						uCurrEdgeIndex ^= 1;

						if (uCurrEdgeIndex == 0)
						{
							ptriCurr = ptriCurr->ptriGetNext();

							if (ptriCurr->pqvtGetVertex(1) == pqrContainer->dapqnqIterateStore[uCurrNodeIndex]->pqvtGetVertex(3))
							{
								NextQuadNode();
								break;
							}
						}
					}
					while ((uCurrEdgeIndex == 0 && ptriCurr->pqvtGetVertex(0)->iY() != ptriCurr->pqvtGetVertex(1)->iY()) ||
					       (uCurrEdgeIndex == 1 && ptriCurr->pqvtGetVertex(1)->iX() != ptriCurr->pqvtGetVertex(2)->iX())   );
				}
				else
				{
					if (uCurrEdgeIndex == 1)
					{
						do
						{
							ptriCurr = ptriCurr->ptriGetNext();

							if (ptriCurr->pqvtGetVertex(1) == pqrContainer->dapqnqIterateStore[uCurrNodeIndex]->pqvtGetVertex(2))
							{
								uCurrEdgeIndex = 0;
								break;
							}
						}
						while (ptriCurr->pqvtGetVertex(1)->iY() != ptriCurr->pqvtGetVertex(2)->iY());
					}
					else
					{
						do
						{
							ptriCurr = ptriCurr->ptriGetNext();

							if (ptriCurr == 0)
							{
								NextQuadNode();
								break;
							}
						}
						while (ptriCurr->pqvtGetVertex(0)->iX() != ptriCurr->pqvtGetVertex(1)->iX());
					}
				}
			}
		}
	}

	//******************************************************************************************
	ESideOf esfEdgeRectangle(const CRectangle<>& rc, const CVector2<>& v2_a, const CVector2<>& v2_b)
	{
		if (v2_a.tX < rc.tX0())
		{
			if (v2_b.tX < rc.tX0())
				return esfOUTSIDE;
		}
		else if (v2_a.tX > rc.tX1())
		{
			if (v2_b.tX > rc.tX1())
				return esfOUTSIDE;
		}

		if (v2_a.tY < rc.tY0())
		{
			if (v2_b.tY < rc.tY0())
				return esfOUTSIDE;
		}
		else if (v2_a.tY > rc.tY1())
		{
			if (v2_b.tY > rc.tY1())
				return esfOUTSIDE;
		}

		return esfINTERSECT;
	}

	//*****************************************************************************************
	ESideOf CQueryRect::CEdgeIterator::esfEdgeIntersects() const
	{
		CVector3<> v3_a, v3_b;
		GetEdge(v3_a, v3_b);

		// Test intersection.
		return esfEdgeRectangle(pqrContainer->rcExtentQuery, v3_a, v3_b);
	}

//**********************************************************************************************
//
// NMultiResolution::CQueryRect implementation.
//

	//******************************************************************************************
	CQueryRect::CQueryRect(CQuadRootQuery* pqnq_root, const CBoundVol& bv, const CPresence3<>& pr3, TReal r_freq_highpass, bool b_filter_as_ratio, uint u_max_nodes)
		: pqnqRoot(pqnq_root), dapqnqIterateStore(u_max_nodes), iIterateLockCount(0)
	{
		Assert(pqnq_root != 0);

		// Max has not been computed yet.
		rZMax = -FLT_MAX;

		pqnqRoot->RegisterQuery(this);

		SetEvaluateReq();

		SetVolume(bv, pr3, r_freq_highpass, b_filter_as_ratio);
	}


	//******************************************************************************************
	CQueryRect::~CQueryRect()
	{
		Assert(iIterateLockCount == 0);

		pqnqRoot->UnregisterQuery(this);
	}


	//******************************************************************************************
	void CQueryRect::SetVolume(const CBoundVol& bv, const CPresence3<>& pr3, TReal r_freq_highpass, bool b_filter_as_ratio)
	{
		cfMaxRegionHighPass = CCoef(r_freq_highpass, pqnqRoot->mpConversions.rWorldToCoef);
		bFilterAsRatio = b_filter_as_ratio;

		Assert(cfMaxRegionHighPass >= cfZERO);

		// Obtain world space extents of given volume.
		CVector3<> v3_world_min;
		CVector3<> v3_world_max;

		bv.GetWorldExtents(pr3, v3_world_min, v3_world_max);

		// Convert the bounding volume's 3d world space extents to a quad tree space 2d rectangle on the xy plane.
		rcExtentQuery = CRectangle<>(v3_world_min, v3_world_max - v3_world_min);
		CRectangle<> rc_quad_extent = rcExtentQuery * pqnqRoot->mpConversions.tlr2WorldToQuad;

		// Convert this rectangle to integer coordinates, rounding to ensure the rectangle bounds
		// the volume in quad tree space.
		int i_x0 = iTrunc(rc_quad_extent.tX0());
		int i_y0 = iTrunc(rc_quad_extent.tY0());
		int i_x1 = iTrunc(rc_quad_extent.tX1() + 1.0);
		int i_y1 = iTrunc(rc_quad_extent.tY1() + 1.0);

		rcExtent = CRectangle<int>(i_x0, i_y0, i_x1 - i_x0, i_y1 - i_y0);

		// Clip the rectangle against the extents of the wavelet transformed data.
		CRectangle<int> rc_quad_space_extent(pqnqRoot->mpConversions.rcQuadSpaceExtents.v2Start(),
		                                     pqnqRoot->mpConversions.rcQuadSpaceExtents.v2Extent());

		rcExtent &= rc_quad_space_extent;

		// If the rectangle lies outside the extents of the wavelet transformed data, set it to a 'null' rectangle.
		if (rcExtent.tWidth() == 0 || rcExtent.tHeight() == 0)
			rcExtent = CRectangle<int>(0, 0, 0, 0);

		rcRefinedExtent = rcExtent;

		// Make sure the quad tree is re-evaluated at the next iteration.
		SetEvaluateReq();
	}


	//******************************************************************************************
	CTriangleQuery* CQueryRect::ptriFind(TReal r_x, TReal r_y)
	{
		CVector2<> v2_quad = CVector2<>(r_x, r_y) * pqnqRoot->mpConversions.tlr2WorldToQuad;
		CVector2<int> v2i_quad(iTrunc(v2_quad.tX), iTrunc(v2_quad.tY));

		// If within the last quad node, use it.
		if (pqnqLastNode && pqnqLastNode->bContains(v2i_quad.tX, v2i_quad.tY))
		{
			//
			// THIS DOES NOT CURRENTLY WORK. 
			// In rare cases, ptriLastTri points to invalid data.
			//
#if 0
			// If within the last triangle, use it.
			if (ptriLastTri)
			{
				if (pqnqLastNode->ptinfGetTriangleInfo()->ptriTriangles)
				{
					// Triangulation info is valid.
					if (ptriLastTri->bContains(v2_quad.tX, v2_quad.tY))
						return ptriLastTri;
				}
			}
#endif
		}
		else
		{
			// Iterate through quad-nodes.
			CIterator it = itBegin();
			for (; it; it.NextQuadNode())
			{
				if (it.pqnqGetNode()->bContains(v2i_quad.tX, v2i_quad.tY))
				{
					pqnqLastNode = it.pqnqGetNode();
					break;
				}
			}

			if (!it)
			{
				// We finished the loop, so the point was not contained in the query rect.
				// Only if the point is outside the quad-tree is this valid
				// (because the query-rect will have been clamped to the quad-tree).
				Assert(!pqnqRoot->bContains(v2i_quad.tX, v2i_quad.tY));

				// In either case, the caller must trap this.
				return 0;
			}
		}

		// Point is contained in pqnqLastNode. Find triangle.
		CTriangleQuery* ptri_enc = pqnqLastNode->ptriGetEnclosingTriangle(v2_quad.tX, v2_quad.tY);
#if 0
		ptriLastTri = ptri_enc;
#endif
		Assert(ptri_enc);

		return ptri_enc;
	}

	//******************************************************************************************
	TReal CQueryRect::rHeight(TReal r_x, TReal r_y, const CPlane** pppl)
	{
		CTriangleQuery* ptri_enc = ptriFind(r_x, r_y);

		// Trap height queries outside the bounds of the terrain.
		if (!ptri_enc)
		{
			static CPlane pl_default(d3ZAxis, 0);
			*pppl = &pl_default;
			return 0;
		}

		ptri_enc->Init(pqnqRoot->mpConversions);
		*pppl = &ptri_enc->plGetPlaneWorld();

		return ptri_enc->rHeight(r_x, r_y);
	}


	//******************************************************************************************
	void CQueryRect::Evaluate()
	{
		// Make sure we only evaluate the quad tree for queries once between each update step.
		if (bEvaluateReq)
		{
			CCycleTimer ctmr_eval;

			Assert(iIterateLockCount == 0 && dapqnqIterateStore.uLen == 0);

			CCycleTimer ctmr;

			pqnqRoot->AlignQueryExtent(*this);
			pqnqRoot->EvaluateQueryBranch(*this);

			CQueryRect::psRefine.Add(ctmr(), 1);


			ctmr.Reset();

			pqnqRoot->IterateBranch(*this);

			CQueryRect::psIterate.Add(ctmr(), 1);

			bEvaluateReq = false;

			//
			// Cache the Z limits in the rect as well.
			//

			rZMax = -FLT_MAX;

			// Iterate through nodes.
			for (CIterator it = itBegin(); it; it.NextQuadNode())
			{
				SetMax(rZMax, it.pqnqGetNode()->prrGetWorldZLimits(pqnqRoot).second);
			}

			CQueryRect::psEvaluate.Add(ctmr_eval(), 1);
		}
	}
};
