/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of WaveletQuadTree.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletQuadTree.cpp                                     $
 * 
 * 179   10/01/98 8:28p Mlange
 * Optimised move message processing.
 * 
 * 178   98.09.24 1:38a Mmouni
 * Added multiplier for terrain mesh pixel error.
 * 
 * 177   9/23/98 7:41p Mlange
 * Wavelet quad tree nodes now have a single pointer to the first descendant and a pointer to
 * the next sibling, instead of four pointers for each of the descendants.
 * 
 * 176   98/09/19 14:46 Speter
 * Changed default settings to increase texturing distance, maintain screen resolution in
 * distance, and eliminate static texturing...goes faster.
 * 
 * 175   9/15/98 8:55p Mmouni
 * Added code to set area on terrain gouraud polygons.
 * 
 **********************************************************************************************/

#include "GblInc/Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "WaveletQuadTree.hpp"

#include "Lib/GeomDBase/WaveletStaticData.hpp"
#include "Lib/GeomDBase/TerrainTexture.hpp"

#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/QualitySettings.hpp"

#include "Lib/Renderer/Light.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"
#include "Lib/Renderer/PipeLine.hpp"

#include "Lib/View/LineDraw.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Std/Set.hpp"

#include "AsmSupport.hpp"

#pragma warning(disable: 4786)

namespace NMultiResolution
{


//**********************************************************************************************
//
// NMultiResolution::CQuadVertexTIN implementation.
//

	//******************************************************************************************
	CBlockAllocator<CQuadVertexTIN>::SStore CQuadVertexTIN::stStore;

	//******************************************************************************************
	TClutVal CQuadVertexTIN::cvGetLighting(const CQuadNodeTIN* pqntin, CLightList& ltl_lighting, const CClut& clut, const CQuadRootTIN* pqntin_root)
	{
		if (i2LightVal < 0)
		{
			TClutVal cv_lightval = ltl_lighting.cvGetLighting
			(
				v3Quad(pqntin_root->mpConversions), 
				pqntin_root->d3GetNormal(this, pqntin),
				clut
			);

			Assert(cv_lightval < 256.0f);

			i2LightVal = iPosFloatCast(cv_lightval * 256.0f);
		}

		return TClutVal(i2LightVal * (1.0f / 256.0f));
	}


//**********************************************************************************************
//
// NMultiResolution::CTriangleTIN implementation.
//

	//******************************************************************************************
	CBlockAllocator<CTriangleTIN>::SStore CTriangleTIN::stStore;


	//******************************************************************************************
	void CTriangleTIN::CalcPlane(const SMapping& mp)
	{
		if (!bHasValidPlane())
			plPlane = CPlane(apqvtVertices[0]->v3Quad(mp), apqvtVertices[1]->v3Quad(mp), apqvtVertices[2]->v3Quad(mp));

		#if VER_DEBUG
			CPlane pl_cmp(apqvtVertices[0]->v3Quad(mp), apqvtVertices[1]->v3Quad(mp), apqvtVertices[2]->v3Quad(mp));
			Assert(Fuzzy(pl_cmp.d3Normal) == plPlane.d3Normal && bFurryEquals(pl_cmp.rD, plPlane.rD));
		#endif
	}


//**********************************************************************************************
//
// NMultiResolution::CTriNodeInfoTIN implementation.
//

	//******************************************************************************************
	CBlockAllocator<CTriNodeInfoTIN>::SStore CTriNodeInfoTIN::stStore;


	//******************************************************************************************
	void CTriNodeInfoTIN::Validate(const CQuadRootTIN* pqntin_root, CQuadNodeTIN* pqntin_owner)
	{
		Assert(pqntin_owner != 0 && pqntin_owner->ptinfGetTriangleInfo() == this);

		if (!bValid)
		{
			CTriangleTIN* ptri_curr = pqntin_owner->ptriGetFirst();

			do
			{
				// If the triangle's plane is invalid, calculate it now.
				if (!ptri_curr->bHasValidPlane())
					ptri_curr->CalcPlane(pqntin_root->mpConversions);

				if (ptri_curr->pqntinGetTextureNode() == 0)
				{
					// If the owning node does not contain a texture node, search for an ancestor that has.
					CQuadNodeTIN* pqntin_tex_tri = pqntin_owner;

					while (pqntin_tex_tri->ptxnGetTextureNode() == 0)
					{
						pqntin_tex_tri = pqntin_tex_tri->ptqnGetParent();
						Assert(pqntin_tex_tri != 0);
					}

					// Store texture reference in triangle.
					ptri_curr->SetTextureReference(pqntin_tex_tri);
				}

				ptri_curr = ptri_curr->ptriGetNext();
			}
			while (ptri_curr != 0);

			bValid = true;
		}

	}

	//******************************************************************************************
	void CTriNodeInfoTIN::InvalidateTextureReferences()
	{
		CTriangleTIN* ptri_curr = ptriTriangles;

		while (ptri_curr != 0)
		{
			ptri_curr->SetTextureReference();
			ptri_curr = ptri_curr->ptriGetNext();
		}

		bValid = false;
	}


	//**********************************************************************************************
	//
	// Definitions for NMultiResolution::SEvalInfo.
	//

	// World distance in meters to which the far distance settings are normalised.
	static const float fWorldFarNormaliseDist = 500;

	//**********************************************************************************************
	//
	struct SEvalInfo
	//
	// Simple POD containing pre-calculated data for quad tree evaluation.
	//
	// Prefix: ei
	//
	//**************************************
	{
		CVector3<> v3QuadSpaceCamPos;			// Position of the camera in quad tree units.

		float fWorldToPixRatio;

		float fNormSqrQuadDistFactor;			// Factor to normalise squared quad space distances to fWorldFarNormaliseDist.
		float fPixTolToCoef;
		float fPixTolDeltaToCoef;
		float fTexTolToCoef;

		int iDistRatioMinNodeSize;
		float fMaxDistRatioSqr;
		float fDisableShadowDistSqr;
		float fDisableDynamicTextureDistSqr;
		float fDisableTextureDistSqr;

		float fFarViewDist;
	};


//**********************************************************************************************
//
// NMultiResolution::CQuadNodeTIN implementation.
//

	//**********************************************************************************************
	//
	class CQuadNodeTIN::CPriv: CQuadNodeTIN
	//
	// Private implementation of CQuadNodeTIN.
	//
	//**************************************
	{
		friend class CQuadNodeTIN;
		friend class CQuadRootTIN;

		//******************************************************************************************
		//
		int iEvaluate
		(
			const CQuadRootTIN* pqntin_root
		);
		//
		// Evaluate the geometry and texturing of this node.
		//
		// Returns:
		//		The smallest size (in quad tree units) of all the nodes in this branch of the quad
		//		tree that can contain triangles.
		//
		// Notes:
		//		Called by iEvaluateBranch().
		//
		//**************************************


		//******************************************************************************************
		//
		void RemoveTexturesAndRefsBranch();
		//
		// Remove any textures and invalidate triangle texture references in this branch of the
		// quad tree.
		//
		//******************************


		//******************************************************************************************
		//
		void RecalcCoefLimitsBranch();
		//
		// Recalculates the coeficient 'Z' limits for this branch of the quad tree.
		//
		//**************************************


		//******************************************************************************************
		//
		std::pair<CCoef, CCoef> prcfCheckCoefLimitsBranch() const;
		//
		// Checks the coeficient 'Z' limits for this branch of the quad tree, in debug builds only.
		//
		//**************************************


		//******************************************************************************************
		//
		void GetObserverMinMaxDistSqr
		(
			CVector3<>& v3_min_dist,		// Vectors to initialise.
			CVector3<>& v3_max_dist,
			const CQuadRootTIN* pqntin_root
		);
		//
		// Determines the nearest and furthest distances from the observer (specified in the root
		// quad node) to a point in or on the bounding box of this node.
		//
		//**************************************


		//******************************************************************************************
		//
		void GenerateLighting
		(
			rptr<CRaster> pras_dest,		// Composite 16-bit raster to light.
			CLightList& ltl_lighting,		// Lighting context for current scene.
			const CClut& clut,				// Clut for raster output.
			const CTransLinear2<>& tlr2_ras,// Translation from quad-to-raster space for vertices.
			const CQuadRootTIN* pqntin_root	// Owning context.
		) const;
		//
		// Fill the lighting values (high byte) of the composite raster.
		// This overload is recursively called by the public overload above.
		//
		//******************************
	};



	//******************************************************************************************
	CBlockAllocator<CQuadNodeTIN>::SStore CQuadNodeTIN::stStore;


	//******************************************************************************************
	CQuadNodeTIN::~CQuadNodeTIN()
	{
		if (ptxnTexture != 0)
			delete ptxnTexture;
	}


	//******************************************************************************************
	void CQuadNodeTIN::EvaluateTexture(const CQuadRootTIN* pqntin_root)
	{
		Assert(ptxnTexture != 0 && pqntin_root != 0);

		// Determine the minimum and maximum distances from the camera to the bounding box of this node.
		CVector3<> v3_min_sqr;
		CVector3<> v3_max_sqr;
		priv_self.GetObserverMinMaxDistSqr(v3_min_sqr, v3_max_sqr, pqntin_root);

		float f_min_node_dist_sqr = v3_min_sqr.tX + v3_min_sqr.tY + v3_min_sqr.tZ;
		
		if (!d3dDriver.bUseD3D() &&
		    CIntFloat(f_min_node_dist_sqr).i4Int > CIntFloat(pqntin_root->peiInfo->fDisableTextureDistSqr).i4Int)
		{
			ptxnTexture->SetSolidTexture(pqntin_root);
			return;
		}


		if (CIntFloat(f_min_node_dist_sqr).i4Int > CIntFloat(pqntin_root->peiInfo->fDisableDynamicTextureDistSqr).i4Int)
		{
			ptxnTexture->SetStaticTexture(pqntin_root);
			return;
		}


		//
		// Calculate the required texture sizes along U and V for a texture asssigned to this node.
		// The U/V texture sizes are a power of two.
		//
		Assert(cfMin != cfNAN && cfMax != cfNAN);

		TReal r_z_min = cfMin.rGet(pqntin_root->mpConversions.rCoefToQuad);
		TReal r_z_max = cfMax.rGet(pqntin_root->mpConversions.rCoefToQuad);

		TReal r_u_proj_sqr;
		TReal r_v_proj_sqr;
		TReal r_h_proj_sqr;

		CVector2<> v2_tex_size;

		// Get the vector to base projection calculations on.
		CVector3<> v3_proj_dist_sqr;

		if (CQuadRootTIN::bTexelProjNearest)
		{
			// Projection is based on minimum distances.
			v3_proj_dist_sqr = v3_min_sqr;
		}
		else
		{
			// Projection is based on distances to midpoint.
			CVector3<> v3_mid
			(
				pqvtGetVertex(0)->iX() + iGetSize() / 2,
				pqvtGetVertex(0)->iY() + iGetSize() / 2,
				(r_z_min + r_z_max) / 2
			);

			v3_proj_dist_sqr = pqntin_root->peiInfo->v3QuadSpaceCamPos - v3_mid;
			v3_proj_dist_sqr.tX *= v3_proj_dist_sqr.tX;
			v3_proj_dist_sqr.tY *= v3_proj_dist_sqr.tY;
			v3_proj_dist_sqr.tZ *= v3_proj_dist_sqr.tZ;
		}

		if (f_min_node_dist_sqr == 0.0f)
		{
			// Inside the node.
			v2_tex_size.tX = v2_tex_size.tY = 256;
		}
		else
		{
			// Determine the distance to the location on the bounding box of the quad node that maximises the projection.
			float f_min_h_proj_dist_sqr = float(CIntFloat(MinMax
			(
				CIntFloat(v3_proj_dist_sqr.tZ).i4Int,
				CIntFloat(v3_min_sqr.tX + v3_min_sqr.tY).i4Int,
				CIntFloat(v3_max_sqr.tX + v3_max_sqr.tY).i4Int))
			);

			r_h_proj_sqr = Sqr((r_z_max - r_z_min) * pqntin_root->peiInfo->fWorldToPixRatio);
			r_h_proj_sqr *= f_min_h_proj_dist_sqr / Sqr(f_min_h_proj_dist_sqr + v3_proj_dist_sqr.tZ);


			float f_min_u_proj_dist_sqr = float(CIntFloat(MinMax
			(
				CIntFloat(v3_proj_dist_sqr.tX).i4Int,
				CIntFloat(v3_min_sqr.tY + v3_min_sqr.tZ).i4Int,
				CIntFloat(v3_max_sqr.tY + v3_max_sqr.tZ).i4Int))
			);

			r_u_proj_sqr = Sqr(iGetSize() * pqntin_root->peiInfo->fWorldToPixRatio);
			r_u_proj_sqr *= f_min_u_proj_dist_sqr / Sqr(f_min_u_proj_dist_sqr + v3_proj_dist_sqr.tX);


			float f_min_v_proj_dist_sqr = float(CIntFloat(MinMax
			(
				CIntFloat(v3_proj_dist_sqr.tY).i4Int,
				CIntFloat(v3_min_sqr.tX + v3_min_sqr.tZ).i4Int,
				CIntFloat(v3_max_sqr.tX + v3_max_sqr.tZ).i4Int))
			);

			r_v_proj_sqr = Sqr(iGetSize() * pqntin_root->peiInfo->fWorldToPixRatio);
			r_v_proj_sqr *= f_min_v_proj_dist_sqr / Sqr(f_min_v_proj_dist_sqr + v3_proj_dist_sqr.tY);

			float f_tex_u_size = fSqrtEst(r_u_proj_sqr);
			float f_tex_v_size = fSqrtEst(r_v_proj_sqr);
			float f_height     = fSqrtEst(r_h_proj_sqr);

			CIntFloat if_rel_mid_x = pqntin_root->peiInfo->v3QuadSpaceCamPos.tX - (pqvtGetVertex(0)->iX() + iGetSize() / 2);
			CIntFloat if_rel_mid_y = pqntin_root->peiInfo->v3QuadSpaceCamPos.tY - (pqvtGetVertex(0)->iY() + iGetSize() / 2);

			float f_ratio_angle = float(angArctan2(if_rel_mid_y.ifAbs().fFloat, if_rel_mid_x.ifAbs().fFloat)) / float(dPI_2);
			f_tex_u_size += f_height * (1 - f_ratio_angle);
			f_tex_v_size += f_height * f_ratio_angle;

			// Normalise the minimum distance to this node wrt the static texture distance, and use it to interpolate
			// between the near and far texel scale parameters.
			float f_norm_node_dist = fSqrtEst(f_min_node_dist_sqr / pqntin_root->peiInfo->fDisableDynamicTextureDistSqr);
			float f_tex_scale      = CQuadRootTIN::rvarTexelScale + (CQuadRootTIN::rvarTexelScaleFar -
																	 CQuadRootTIN::rvarTexelScale     ) * f_norm_node_dist;

			if (f_tex_scale < 0)
			{
				// Set smallest possible texture size.
				v2_tex_size = CVector2<>(1, 1);
			}
			else
			{
				// Adjust effective texel resolution by the quality setting.
				int i_curr_quality = iGetQualitySetting();
				f_tex_scale *= qdQualitySettings[i_curr_quality].fTerrainTexelScale;

				// Scale the texture.
				f_tex_u_size *= f_tex_scale;
				f_tex_v_size *= f_tex_scale;

				Assert(f_tex_u_size >= 0 && f_tex_v_size >= 0);

				v2_tex_size.tX = f_tex_u_size;
				v2_tex_size.tY = f_tex_v_size;
			}
		}

		//
		// Determine if this node needs to be shadowed. Compare the minimum squared distance to this node to
		// the squared distance at which to disable shadows.
		//
		bool b_shadow = CIntFloat(f_min_node_dist_sqr).i4Int <= CIntFloat(pqntin_root->peiInfo->fDisableShadowDistSqr).i4Int;

		// At low quality settings, disable shadows.
		if (!qdQualitySettings[iGetQualitySetting()].bShadows)
			b_shadow = false;

		ptxnTexture->ScheduleUpdate(pqntin_root, v2_tex_size, b_shadow);
	}


	//******************************************************************************************
	void CQuadNodeTIN::GenerateLighting
	(
		rptr<CRaster> pras_dest, CLightList& ltl_lighting, const CClut& clut, const CQuadRootTIN* pqntin_root
	) const
	{
		if (pras_dest->iWidth <= 0 || pras_dest->iHeight <= 0)
			return;

		// Create the translation from quad to raster coords, with Y axis flipped.
		CRectangle<> rc_quad = rcGetRectangle();
		CTransLinear2<> tlr2_quad_ras
		(
			CRectangle<>(rc_quad.tX0(), rc_quad.tY1(), rc_quad.tWidth(), -rc_quad.tHeight()),
			CRectangle<>(0.5, 0.5, pras_dest->iWidth, pras_dest->iHeight)
		);

		// Call recursive member.
		priv_selfc.GenerateLighting(pras_dest, ltl_lighting, clut, tlr2_quad_ras, pqntin_root);
	}



	//******************************************************************************************
	void CQuadNodeTIN::ResetDeferredEvalBranch()
	{
		if (stState[estLEAF][estLEAF_COMBINE])
		{
			Assert(ptqnGetTriangulate() == this);
			
			ptinfGetTriangleInfo()->iDeferEvalCount = 0;
		}
		else
		{
			CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				pqntin_dsc->ResetDeferredEvalBranch();

				pqntin_dsc = pqntin_dsc->ptqnGetSibling();
			}
		}
	}


	// Constant used in expressions to avoid crummy code generation by the fucked-up compiler.
	CQuadNodeTIN::TState createSTMASKTRI()
	{
		CQuadNodeTIN::TState result;
		result += CQuadNodeTIN::estLEAF;
		result += CQuadNodeTIN::estLEAF_COMBINE;
		return result;
	}
	static const CQuadNodeTIN::TState stMASK_TRI = createSTMASKTRI();
	//******************************************************************************************
	int CQuadNodeTIN::iEvaluateBranch(const CQuadRootTIN* pqntin_root)
	{
		int i_min_sig_size = TypeMax(int);

		if (!(stState & stMASK_TRI))
		{
			CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				i_min_sig_size = Min(i_min_sig_size, pqntin_dsc->iEvaluateBranch(pqntin_root));

				pqntin_dsc = pqntin_dsc->ptqnGetSibling();
			}

			stState[estDIFF_SIG_LEVEL_EXCEEDED] = uint(i_min_sig_size << (int)CQuadRootTIN::rvarSigNodeDiffShift) < uint(iGetSize());
		}


		// Note the use of an intermediate constant to avoid bad inline code generation by the crummy compiler.
		if (stState & stMASK_TRI)
		{
			CQuadNodeTIN* pqntin_tri = ptqnGetTriangulate();
			CTriNodeInfoTIN* ptinf   = pqntin_tri->ptinfGetTriangleInfo();

			if (ptinf->iDeferEvalCount != 0)
			{
				// Determine if the observer is within the influence of this node.
				CIntFloat if_dist_x_min = (pqvtGetVertex(0)->iX() - iGetSize()) - pqntin_root->peiInfo->v3QuadSpaceCamPos.tX;
				CIntFloat if_dist_x_max = (pqvtGetVertex(2)->iX() + iGetSize()) - pqntin_root->peiInfo->v3QuadSpaceCamPos.tX;

				CIntFloat if_dist_y_min = (pqvtGetVertex(0)->iY() - iGetSize()) - pqntin_root->peiInfo->v3QuadSpaceCamPos.tY;
				CIntFloat if_dist_y_max = (pqvtGetVertex(2)->iY() + iGetSize()) - pqntin_root->peiInfo->v3QuadSpaceCamPos.tY;

				if (CIntFloat((if_dist_x_min.i4Int ^ if_dist_x_max.i4Int) & (if_dist_y_min.i4Int ^ if_dist_y_max.i4Int)).bSign())
					ptinf->iDeferEvalCount = 0;
			}


			if (ptinf->iDeferEvalCount != 0)
			{
				ptinf->iDeferEvalCount--;

				if (stState[estSIGNIFICANT])
					i_min_sig_size = iGetSize();
			}
			else
			{
				CCycleTimer ctmr;

				i_min_sig_size = priv_self.iEvaluate(pqntin_root);

				CQuadNodeTIN::psEvaluate.Add(ctmr(), 1);
			}
		}

		return i_min_sig_size;
	}


	//******************************************************************************************
	void CQuadNodeTIN::EvaluateTextureBranch(const CQuadRootTIN* pqntin_root)
	{
		// Determine if this node should be subdivided for texture node placement.
		if (stState[estDIFF_SIG_LEVEL_EXCEEDED])
		{
			Assert(stState[estSIGNIFICANT]);
			stState += estTEXTURE_SUBDIVIDE;
		}
		else
		{
			// Determine minimum distance to the camera in x.
			CIntFloat if_dist_x_min = pqvtGetVertex(0)->iX() - pqntin_root->peiInfo->v3QuadSpaceCamPos.tX;
			if (if_dist_x_min.i4Int < 0)
			{
				if_dist_x_min = pqntin_root->peiInfo->v3QuadSpaceCamPos.tX - pqvtGetVertex(2)->iX();
				if (if_dist_x_min.i4Int < 0)
					if_dist_x_min = 0;
			}

			// Determine minimum distance to the camera in y.
			CIntFloat if_dist_y_min = pqvtGetVertex(0)->iY() - pqntin_root->peiInfo->v3QuadSpaceCamPos.tY;
			if (if_dist_y_min.i4Int < 0)
			{
				if_dist_y_min = pqntin_root->peiInfo->v3QuadSpaceCamPos.tY - pqvtGetVertex(2)->iY();
				if (if_dist_y_min.i4Int < 0)
					if_dist_y_min = 0;
			}


			// Determine the minimum and maximum distances to use in x or y for the distance ratio evaluation.
			CIntFloat if_min_dist = Max(if_dist_x_min.i4Int, if_dist_y_min.i4Int);
			CIntFloat if_max_dist = if_min_dist.fFloat + iGetSize();

			// Determine if distance ratio is exceeded in xy.
			bool b_dist_ratio_exceeded = if_max_dist.i4Int > CIntFloat(if_min_dist.fFloat * CQuadRootTIN::rvarMaxDistRatio).i4Int;

			// If not, determine if distance ratio is exceeded in z.
			if (!b_dist_ratio_exceeded)
			{
				// Determine XY distance from midpoint of node to the camera.
				CVector2<> v2_mid(iBaseX() + (iGetSize() >> 1), iBaseY() + (iGetSize() >> 1));

				CVector2<> v2_diff_sqr = CVector2<>(pqntin_root->peiInfo->v3QuadSpaceCamPos) - v2_mid;
				v2_diff_sqr.tX *= v2_diff_sqr.tX;
				v2_diff_sqr.tY *= v2_diff_sqr.tY;

				if (cfMin == cfNAN)
					priv_self.RecalcCoefLimitsBranch();

				CCoef cf_delta = cfMax - cfMin;

				CIntFloat if_proj = Sqr(float(cf_delta.iGet()));
				CIntFloat if_dist = Sqr(pqntin_root->peiInfo->fTexTolToCoef) * (v2_diff_sqr.tX + v2_diff_sqr.tY);

				b_dist_ratio_exceeded = if_proj.i4Int > if_dist.i4Int;
			}


			int i_size_cmp;

			if (stState[estSIGNIFICANT])
				i_size_cmp = ptqnGetTriangulate()->iGetSize();
			else
				i_size_cmp = iGetSize() << 1;

			stState[estTEXTURE_SUBDIVIDE] =
				iGetSize() > 1 &&
				i_size_cmp > pqntin_root->peiInfo->iDistRatioMinNodeSize &&
				b_dist_ratio_exceeded;
		}

		Assert(!stState[estTEXTURE_SUBDIVIDE] || iGetSize() > 1);


		// If we are subdividing a quad node that has no wavelet data, make sure we will not create a
		// triangulation with a topology that differs from the underlying wavelet data at its highest
		// resolution.
		stState -= estTEX_SUBDIV_CONSTRAINED;

		if (CQuadRootTIN::bConform && stState[estTEXTURE_SUBDIVIDE])
		{
			if (!bSubdivideConforms())
			{
				stState -= estTEXTURE_SUBDIVIDE;
				stState += estTEX_SUBDIV_CONSTRAINED;
			}
		}

		if (stState[estTEXTURE_SUBDIVIDE])
		{
			if (!bHasDescendants())
			{
				CCycleTimer ctmr;

				Subdivide(pqntin_root);

				CQuadNodeTIN::psRefine.Add(ctmr(), 1);
			}


			CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				pqntin_dsc->EvaluateTextureBranch(pqntin_root);

				pqntin_dsc = pqntin_dsc->ptqnGetSibling();
			}
		}
	}


	//******************************************************************************************
	void CQuadNodeTIN::AssignTexturesBranch(const CQuadRootTIN* pqntin_root, CTextureNode* ptxn_ancestor)
	{
		Assert(!(ptxnTexture != 0 && ptxn_ancestor != 0));

		if (!stState[estTEXTURE_SUBDIVIDE] || stState[estLEAF][estLEAF_COMBINE])
		{
			// This node can contain a texture. Does it already contain a texture node?
			if (ptxnTexture == 0)
			{
				// Kill any textures below this node and create a new texture.
				CTextureNode* ptxn_new;

				if (ptxn_ancestor != 0)
					ptxn_new = new CTextureNode(*ptxn_ancestor, this, pqntin_root);
				else
					ptxn_new = new CTextureNode(this);

				bool b_old_subdiv = stState[estTEXTURE_SUBDIVIDE];
				priv_self.RemoveTexturesAndRefsBranch();
				stState[estTEXTURE_SUBDIVIDE] = b_old_subdiv;

				ptxnTexture = ptxn_new;
			}
		}
		else
		{
			// This node can not contain a texture.
			if (ptxnTexture != 0)
			{
				if (bHasDescendants())
				{
					CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

					for (int i_dsc = 0; i_dsc < 4; i_dsc++)
					{
						pqntin_dsc->AssignTexturesBranch(pqntin_root, ptxnTexture);

						pqntin_dsc = pqntin_dsc->ptqnGetSibling();
					}
				}

				delete ptxnTexture;
				ptxnTexture = 0;
			}
			else
			{
				if (bHasDescendants())
				{
					CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

					for (int i_dsc = 0; i_dsc < 4; i_dsc++)
					{
						pqntin_dsc->AssignTexturesBranch(pqntin_root, ptxn_ancestor);

						pqntin_dsc = pqntin_dsc->ptqnGetSibling();
					}
				}
			}
		}
	}


	//******************************************************************************************
	void CQuadNodeTIN::PurgeTexturesBranch(const CRectangle<>& rc_region)
	{
		// Recurse down the tree until we've reached a textured node.
		if (ptxnTexture == 0)
		{
			Assert(bHasDescendants());

			CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				pqntin_dsc->PurgeTexturesBranch(rc_region);

				pqntin_dsc = pqntin_dsc->ptqnGetSibling();
			}
		}
		else
		{
			// Intersect with this quad.
			if (rc_region.bIntersects(rcGetRectangle()))
				// Delete the actual texture, causing it to be regenerated when next looked at.
				ptxnTexture->PurgeTexture();
		}
	}


	//******************************************************************************************
	void CQuadNodeTIN::MovingObjShadowsBranch(const CRectangle<int>& rc_region_int, const CRectangle<>& rc_region, const CMessageMove& msgmv)
	{
		int i_accum  = (rc_region_int.tX1() - pqvtGetVertex(0)->iX()) | (pqvtGetVertex(2)->iX() - rc_region_int.tX0());
			i_accum |= (rc_region_int.tY1() - pqvtGetVertex(0)->iY()) | (pqvtGetVertex(2)->iY() - rc_region_int.tY0());

		bool b_intersects = i_accum > 0;

		if (b_intersects)
		{
			// Recurse down the tree until we've reached a textured node.
			if (ptxnTexture == 0)
			{
				Assert(bHasDescendants());

				CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

				for (int i_dsc = 0; i_dsc < 4; i_dsc++)
				{
					pqntin_dsc->MovingObjShadowsBranch(rc_region_int, rc_region, msgmv);

					pqntin_dsc = pqntin_dsc->ptqnGetSibling();
				}
			}
			else
			{
				// Intersect with this quad.
				if (rc_region.bIntersects(rcGetRectangle()))
					ptxnTexture->SetMovingObjShadows(rc_region, msgmv);
			}
		}
	}


	//******************************************************************************************
	void CQuadNodeTIN::CheckTexturesBranch() const
	{
#if VER_DEBUG
		if (!stState[estLEAF][estLEAF_COMBINE])
		{
			const CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				pqntin_dsc->CheckTexturesBranch();

				pqntin_dsc = pqntin_dsc->ptqnGetSibling();
			}
		}
		else
		{
			// If this is a leaf combine node, make sure the descendants have no texture.
			if (stState[estLEAF_COMBINE])
			{
				const CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

				for (int i_dsc = 0; i_dsc < 4; i_dsc++)
				{
					Assert(pqntin_dsc->ptxnTexture == 0);
					Assert(!pqntin_dsc->stState[estSIGNIFICANT][estTEXTURE_SUBDIVIDE]);

					pqntin_dsc = pqntin_dsc->ptqnGetSibling();
				}
			}

			// Walk up the quad tree and find the first node containing a texture.
			const CQuadNodeTIN* pqntin_tex = this;
			for (; pqntin_tex->ptxnTexture == 0; pqntin_tex = pqntin_tex->ptqnGetParent())
				Assert(pqntin_tex != 0);

			// From the node that contains the texture, walk up the tree and make sure no ancestor has a texture.
			const CQuadNodeTIN* pqntin_curr = pqntin_tex->ptqnGetParent();
			for (; pqntin_curr != 0; pqntin_curr = pqntin_curr->ptqnGetParent())
				Assert(pqntin_curr->ptxnTexture == 0);


			// Check state flags are consistent all the way up the tree.
			const CQuadNodeTIN* pqntin_sig = this;
			while (pqntin_sig != 0 && !pqntin_sig->stState[estSIGNIFICANT])
				pqntin_sig = pqntin_sig->ptqnGetParent();

			for (pqntin_curr = pqntin_sig; pqntin_curr != 0; pqntin_curr = pqntin_curr->ptqnGetParent())
				Assert(pqntin_curr->stState[estSIGNIFICANT]);

			const CQuadNodeTIN* pqntin_tex_subdiv = this;
			while (pqntin_tex_subdiv != 0 && !pqntin_tex_subdiv->stState[estTEXTURE_SUBDIVIDE])
				pqntin_tex_subdiv = pqntin_tex_subdiv->ptqnGetParent();

			for (pqntin_curr = pqntin_tex_subdiv; pqntin_curr != 0; pqntin_curr = pqntin_curr->ptqnGetParent())
				Assert(pqntin_curr->stState[estTEXTURE_SUBDIVIDE]);

			const CQuadNodeTIN* pqntin_sig_dist = this;
			while (pqntin_sig_dist != 0 && !pqntin_sig_dist->stState[estDIFF_SIG_LEVEL_EXCEEDED])
				pqntin_sig_dist = pqntin_sig_dist->ptqnGetParent();

			for (pqntin_curr = pqntin_sig_dist; pqntin_curr != 0; pqntin_curr = pqntin_curr->ptqnGetParent())
				Assert(pqntin_curr->stState[estDIFF_SIG_LEVEL_EXCEEDED]);


			// Make sure that the triangles (if any) of this node reference the correct texture.
			if (ptinfInfo != 0 && ptinfInfo->ptriTriangles != 0)
			{
				const CTriangleTIN* ptri_curr = NonConst(*this).ptriGetFirst();

				do
				{
					if (ptri_curr->pqntinGetTextureNode() != 0)
						Assert(ptri_curr->pqntinGetTextureNode() == pqntin_tex);

					ptri_curr = ptri_curr->ptriGetNext();
				}
				while (ptri_curr != 0);
			}
		}
#endif
	}


	//******************************************************************************************
	//
	struct CQuadNodeTIN::SIterateContext
	//
	// Prefix: itc
	//
	// Used in the recursive InitTriangleListBranch function, for efficiency of recursion.
	//
	//******************************
	{
		CDArray<CTriangleTIN*>& daptriTris;		// Array of triangle pointers to fill.
		const CQuadRootTIN* pqntinRoot;			// Root node of the quad tree.
		const CClipRegion2D& clip2dCam;			// Clipping region for camera.
		const CVector3<>& v3CamPos;				// Position of camera in object space.
		TReal rPlaneThickness;					// Thickness of plane through a triangle, used in back face culling.

		SIterateContext
		(
			CDArray<CTriangleTIN*>& daptri,
			const CQuadRootTIN* pqntin_root,
			const CClipRegion2D& clip2d,
			const CVector3<>& v3_cam_shape,
			TReal r_pl_thickness
		) : daptriTris(daptri), pqntinRoot(pqntin_root), clip2dCam(clip2d), v3CamPos(v3_cam_shape), rPlaneThickness(r_pl_thickness)
		{
		}
	};


	//******************************************************************************************
	void CQuadNodeTIN::InitTriangleListBranch(const SIterateContext& itc, CClipRegion2D::CClipInfo cli_vis)
	{
		Assert(cli_vis != esfOUTSIDE);

	#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
		__asm femms									//ensure fast switch for below
	#endif

		// Only if the parent node intersected the view volume do we need to calculate the visibility
		// of this node. Otherwise, the parent was visible, and therefore so is this node.
		if (cli_vis == esfINTERSECT)
		{
		#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

			struct UVec2
			{
				uint iX;
				uint iY;
			} u_v2 = {pqvtGetVertex(0)->iX(), pqvtGetVertex(0)->iY()};

			uint u_size = iGetSize();

			const float fOneHalf = 0.5f;

			CVector2<> f_v2;
			TReal r_radius_sqr;
	
			__asm
			{
				// femms							;initial FEMMS is above

				movd	mm0,[u_size]				;m0= u_size
				jmp		StartAsm1

				align	16
			StartAsm1:
				movq	mm1,[u_v2]					;m1= iY | iX
				test	eax,eax						;2-byte NOOP to avoid degraded predecode

				pi2fd	(m4,m0)						;m4= f_size
				punpckldq mm0,mm0					;m0= u_size | u_size

				movd	mm5,[fOneHalf]				;m5= 0.5f
				psrad	mm0,1						;m0= (size >> 1) | (size >> 1)

				pfmul	(m4,m4)						;m4= f_size*f_size
				paddd	mm1,mm0						;m1= iY + (size >> 1) | iX + (size >> 1)

				pi2fd	(m1,m1)						;m1= fY | fX
				movq	[f_v2],mm1

				pfmul	(m4,m5)						;m4= f_size*f_size * 0.5f
				movd	[r_radius_sqr],mm4

				// femms							;final FEMMS is below
			}
	
			cli_vis = itc.clip2dCam.cliIntersects(f_v2, r_radius_sqr, cli_vis);

		#else // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

			cli_vis = itc.clip2dCam.cliIntersects(CVector2<>(pqvtGetVertex(0)->iX() + (iGetSize() >> 1), pqvtGetVertex(0)->iY() + (iGetSize() >> 1)), Sqr(iGetSize() * 0.5f) * 2, cli_vis);

		#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
		}

		if (cli_vis != esfOUTSIDE)
		{
			// Recurse down the quad tree until we arrive at a leaf node or a leaf combine node. These are the
			// nodes that contain the triangles.
			if (!stState[estLEAF][estLEAF_COMBINE])
			{
				CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

				for (int i_dsc = 0; i_dsc < 4; i_dsc++)
				{
					pqntin_dsc->InitTriangleListBranch(itc, cli_vis);

					pqntin_dsc = pqntin_dsc->ptqnGetSibling();
				}
			}
			else
			{
				CTriNodeInfoTIN* ptinf = ptinfGetTriangleInfo();

				if (!ptinf->bIsValid())
				{
					#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
						__asm femms								//empty MMX state and ensure fast switch
						ptinf->Validate(itc.pqntinRoot, this);
						__asm femms								//ensure fast switch
					#else
						ptinf->Validate(itc.pqntinRoot, this);
					#endif
				}


				// Add a reference to each of the triangles in this node to the pointer array.
				CTriangleTIN* ptri_curr = ptinfInfo->ptriTriangles;

				do
				{
					const CPlane& pl = ptri_curr->plGetPlaneQuad();

				#if (TARGET_PROCESSOR == PROCESSOR_K6_3D && VER_ASM)

					TReal r_plane_adj_dist;

					typedef SIterateContext tdSIterateContext;
					typedef CVector3<> tdCVector3;

					Assert((char *)&pl.d3Normal.tX - (char *)&pl.d3Normal == 0);
					Assert((char *)&pl.d3Normal.tY - (char *)&pl.d3Normal == 4);
					Assert((char *)&pl.d3Normal.tZ - (char *)&pl.d3Normal == 8);

					__asm
					{
						// femms								;initial FEMMS is above

						mov		ebx,[itc]						;get ptr to iteration context
						jmp		StartAsm2

						align 16
					StartAsm2:
						mov		eax,[pl]						;get ptr to plane of polygon
						
						mov		edx,[ebx]tdSIterateContext.v3CamPos ;get ptr to camera vector

						movq	mm0,[eax+0]CPlaneDef.d3Normal	;m0=   pl.Y   | pl.X

						movq	mm1,[edx]tdCVector3.tX			;m1=   v3.Y   | v3.X

						movd	mm2,[eax+8]CPlaneDef.d3Normal	;m2=     0    | pl.Z

						movd	mm3,[edx]tdCVector3.tZ			;m3=     0    | v3.Z
						pfmul	(m0,m1)							;m0= pl.Y*v.Y | pl.X*v.X

						movd	mm4,[eax]CPlaneDef.rD			;m4= rD
						movd	mm5,[ebx]tdSIterateContext.rPlaneThickness
																;m5= rPlaneThickness
						pfmul	(m2,m3)							;m2=     0    | pl.Z*v3.Z
						pfacc	(m0,m0)							;m0= pl.Y*v3.Y + pl.X*v3.X

						pfadd	(m0,m2)							;m0= pl.Y*v3.Y + pl.X*v3.X + pl.Z*v3.Z
						pfsub	(m4,m5)							;m0= rD - rPlaneThickness

						pfadd	(m0,m4)							;m0= v3*d3Normal + rD - rPlaneThickness
																;  = rDistance - rPlaneThickness
						movd	[r_plane_adj_dist],mm0

						// femms								;final FEMMS is below
					}

				#else // if (TARGET_PROCESSOR == PROCESSOR_K6_3D && VER_ASM)

					TReal r_plane_adj_dist = pl.rDistance(itc.v3CamPos) - itc.rPlaneThickness;

				#endif // else

					if (!CIntFloat(r_plane_adj_dist).bSign())
					{
						// Add triangle to list.
						itc.daptriTris << ptri_curr;
					}

					ptri_curr = ptri_curr->ptriGetNext();
				}
				while (ptri_curr != 0);
			}
		}
	#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
		__asm femms						//empty MMX state and ensure fast switch
	#endif
	}


	//******************************************************************************************
	void CQuadNodeTIN::ModifiedVerticesBranch()
	{
		priv_self.RecalcCoefLimitsBranch();

		CQuadNodeTIN* pqntin_curr = this;

		do
		{
			pqntin_curr->cfMin = cfNAN;
			pqntin_curr->cfMax = cfNAN;

			pqntin_curr = pqntin_curr->ptqnGetParent();
		}
		while (pqntin_curr != 0 && pqntin_curr->cfMin != cfNAN);

		CQuadNodeBaseTriT<CQuadNodeTIN, CQuadVertexTIN, CTriNodeInfoTIN, CTriangleTIN>::ModifiedVerticesBranch();
	}


//**********************************************************************************************
//
// NMultiResolution::CQuadNodeTIN::CPriv implementation.
//

	//******************************************************************************************
	int CQuadNodeTIN::CPriv::iEvaluate(const CQuadRootTIN* pqntin_root)
	{
		Assert(stState[estLEAF][estLEAF_COMBINE]);
		Assert(pqntin_root != 0);

		CVector3<> v3_min_sqr;
		CVector3<> v3_max_sqr;
		GetObserverMinMaxDistSqr(v3_min_sqr, v3_max_sqr, pqntin_root);

		const SEvalInfo* pei_data = pqntin_root->peiInfo;

		bool b_in_influence;
		CCoef cf_min_delta_proj;

		// Is the influence of this node outside of the observer's far view range?
		if (CIntFloat(v3_min_sqr.tX + v3_min_sqr.tY).i4Int > CIntFloat(Sqr(pei_data->fFarViewDist + iGetSize())).i4Int)
		{
			b_in_influence    = false;
			cf_min_delta_proj = cfZERO;

			stState[estSIGNIFICANT] = false;
		}
		else
		{
			b_in_influence = (CIntFloat(v3_min_sqr.tX - Sqr(TReal(iGetSize()))).i4Int &
							  CIntFloat(v3_min_sqr.tY - Sqr(TReal(iGetSize()))).i4Int  ) < 0;

			if (!b_in_influence)
			{
				CCycleTimer ctmr;

				// Determine the distance to the location on the bounding box of the quad node that maximises the projection.
				float f_min_proj_dist_sqr = float(CIntFloat(MinMax
				(
					CIntFloat(v3_min_sqr.tZ).i4Int,
					CIntFloat(v3_min_sqr.tX + v3_min_sqr.tY).i4Int,
					CIntFloat(v3_max_sqr.tX + v3_max_sqr.tY).i4Int))
				);


				// Normalise the minimum distance to this node, and use it to interpolate between the near and far
				// pixel tolerance.
				float f_norm_node_dist = fSqrtEst((v3_min_sqr.tX + v3_min_sqr.tY + v3_min_sqr.tZ) * pei_data->fNormSqrQuadDistFactor);
				float f_pix_tol        = pei_data->fPixTolToCoef + pei_data->fPixTolDeltaToCoef * f_norm_node_dist;

				// Calculate the magnitude of the smallest squared wavelet coeficient value located anywhere in this node, that,
				// when projected, can exceed the tolerance.
				float f_coef_sqr = Sqr(f_pix_tol) * Sqr(f_min_proj_dist_sqr + v3_min_sqr.tZ) / f_min_proj_dist_sqr;

				// Make sure we don't exceed the numerical range of the CCoef type.
				const float f_max_coef_sqr = Sqr(float(cfMAX.iGet()));

				if (CIntFloat(f_coef_sqr).i4Int < CIntFloat(f_max_coef_sqr).i4Int)
					cf_min_delta_proj = CCoef(iPosFloatCast(fSqrtEst(f_coef_sqr)));
				else
					cf_min_delta_proj = cfMAX;

				CQuadNodeTIN::psProject.Add(ctmr(), 1);
			}
			else
			{
				cf_min_delta_proj = cfZERO;
			}

			// If the max coeficient in this node's region is greater than the smallest coeficient value that exceeds the
			// current pixel tolerance, this node MAY contain wavelet coeficients are significant. We mark this node as
			// significant and evaluate it.
			stState[estSIGNIFICANT] = tdData.cfGetMaxRegion() > cf_min_delta_proj;
		}


		CTriNodeInfoTIN* ptinf = ptqnGetTriangulate()->ptinfGetTriangleInfo();
		TReal r_tol_abs_diff = (tdData.cfGetMaxRegion() - cf_min_delta_proj).cfAbs().rGet(pqntin_root->mpConversions.rCoefToQuad);
		ptinf->iDeferEvalCount = iPosFloatCast(r_tol_abs_diff * CQuadRootTIN::rvarDeferMultiplier);


		int i_min_sig_size = TypeMax(int);
		stState -= estDIFF_SIG_LEVEL_EXCEEDED;

		if (stState[estSIGNIFICANT])
		{
			// Make sure significance flag is consistent all the way up the tree.
			for (CQuadNodeTIN* pqntin_curr = ptqnGetParent(); pqntin_curr != 0 && pqntin_curr->stState[estSIGNIFICANT]; pqntin_curr = pqntin_curr->ptqnGetParent())
				pqntin_curr->stState += estSIGNIFICANT;

			if (!bHasDescendants())
			{
				CCycleTimer ctmr;

				ptinf->iDeferEvalCount = 0;

				Subdivide(pqntin_root);

				CQuadNodeTIN::psRefine.Add(ctmr(), 1);
			}

			i_min_sig_size = iGetSize();


			CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				i_min_sig_size = Min(i_min_sig_size, static_cast<CPriv*>(pqntin_dsc)->iEvaluate(pqntin_root));

				pqntin_dsc = pqntin_dsc->ptqnGetSibling();
			}


			stState[estDIFF_SIG_LEVEL_EXCEEDED] = uint(i_min_sig_size << CQuadRootTIN::rvarSigNodeDiffShift) < uint(iGetSize());

			if (stState[estLEAF_COMBINE])
			{
				CCycleTimer ctmr;

				bool ab_wvlt_sig[3];

				if (!b_in_influence)
				{
					for (int i_wvlt = 0; i_wvlt < 3; i_wvlt++)
					{
						const CQuadVertexTIN* pqvt_wvlt = ptqnGetFirstDescendant()->pqvtGetVertex(i_wvlt + 1);

						// First do the quick conservative significance calculation, if this fails perform an
						// accurate projection calculation.
						if (pqvt_wvlt->cfWavelet().cfAbs() > cf_min_delta_proj)
						{
							// Calculate the squared distance from the observer to the wavelet coeficient.
							CVector3<> v3_diff_sqr = pei_data->v3QuadSpaceCamPos - pqvt_wvlt->v3Quad(pqntin_root->mpConversions);
							v3_diff_sqr.tX *= v3_diff_sqr.tX;
							v3_diff_sqr.tY *= v3_diff_sqr.tY;
							v3_diff_sqr.tZ *= v3_diff_sqr.tZ;

							float f_wvlt_dist_sqr = v3_diff_sqr.tX + v3_diff_sqr.tY + v3_diff_sqr.tZ;

							// Determine the pixel tolerance for the wavelet coeficient projection.
							float f_norm_wvlt_dist = fSqrtEst(f_wvlt_dist_sqr * pei_data->fNormSqrQuadDistFactor);
							float f_pix_tol_wvlt   = pei_data->fPixTolToCoef + pei_data->fPixTolDeltaToCoef * f_norm_wvlt_dist;

							CIntFloat if_proj = Sqr(float(pqvt_wvlt->cfWavelet().iGet())) * (v3_diff_sqr.tX + v3_diff_sqr.tY);
							CIntFloat if_dist = Sqr(f_pix_tol_wvlt) * Sqr(f_wvlt_dist_sqr);

							ab_wvlt_sig[i_wvlt] = if_proj.i4Int > if_dist.i4Int;
						}
						else
							ab_wvlt_sig[i_wvlt] = false;
					}
				}
				else
				{
					// The observer is in the influence of this node. Set the significance flag for all wavelet
					// coeficients that are not zero.
					ab_wvlt_sig[0] = ptqnGetFirstDescendant()->pqvtGetVertex(1)->cfWavelet() != cfZERO;
					ab_wvlt_sig[1] = ptqnGetFirstDescendant()->pqvtGetVertex(2)->cfWavelet() != cfZERO;
					ab_wvlt_sig[2] = ptqnGetFirstDescendant()->pqvtGetVertex(3)->cfWavelet() != cfZERO;
				}

				SetWaveletStates(ab_wvlt_sig[0], ab_wvlt_sig[1], ab_wvlt_sig[2]);

				CQuadNodeTIN::psEvalWavelets.Add(ctmr(), 1);
			}
		}
		else if (!stState[estTEXTURE_SUBDIVIDE])
		{
			if (bHasDescendants())
			{
				CCycleTimer ctmr;

				ptinf->iDeferEvalCount = 0;

				Decimate();

				CQuadNodeTIN::psDecimate.Add(ctmr(), 1);
			}
		}

		return i_min_sig_size;
	}


	//******************************************************************************************
	void CQuadNodeTIN::CPriv::RemoveTexturesAndRefsBranch()
	{
		stState -= estTEXTURE_SUBDIVIDE;

		// Kill any texture.
		if (ptxnTexture != 0)
		{
			delete ptxnTexture;
			ptxnTexture = 0;
		}

		// Recurse until we reach a quad node that may contain triangles.
		if (!stState[estLEAF][estLEAF_COMBINE])
		{
			CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				static_cast<CPriv*>(pqntin_dsc)->RemoveTexturesAndRefsBranch();

				pqntin_dsc = pqntin_dsc->ptqnGetSibling();
			}
		}
		else
		{
			// Invalidate the referenced texture for any triangles.
			if (ptinfInfo != 0)
			{
				Assert(ptqnGetTriangulate() == this);
				ptinfInfo->InvalidateTextureReferences();
			}

			if (stState[estLEAF_COMBINE])
			{
				// Kill any texture and reset the evaluate flag for the remaining descendants.
				CQuadNodeTIN* ptqn_dsc = ptqnGetFirstDescendant();

				for (int i_dsc = 0; i_dsc < 4; i_dsc++)
				{
					ptqn_dsc->stState -= estTEXTURE_SUBDIVIDE;

					if (ptqn_dsc->ptxnTexture != 0)
					{
						delete ptqn_dsc->ptxnTexture;
						ptqn_dsc->ptxnTexture = 0;
					}

					ptqn_dsc = ptqn_dsc->ptqnGetSibling();
				}
			}
		}
	}


	//******************************************************************************************
	void CQuadNodeTIN::CPriv::RecalcCoefLimitsBranch()
	{
		if (bHasDescendants())
		{
			CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

			static_cast<CPriv*>(pqntin_dsc)->RecalcCoefLimitsBranch();

			cfMin = pqntin_dsc->cfMin;
			cfMax = pqntin_dsc->cfMax;

			for (int i_dsc = 1; i_dsc < 4; i_dsc++)
			{
				pqntin_dsc = pqntin_dsc->ptqnGetSibling();

				static_cast<CPriv*>(pqntin_dsc)->RecalcCoefLimitsBranch();

				cfMin = Min(cfMin, pqntin_dsc->cfMin);
				cfMax = Max(cfMax, pqntin_dsc->cfMax);
			}
		}
		else
		{
			cfMin = pqvtGetVertex(0)->cfScaling();
			cfMax = pqvtGetVertex(0)->cfScaling();

			for (int i_vt = 1; i_vt < 4; i_vt++)
			{
				cfMin = Min(cfMin, pqvtGetVertex(i_vt)->cfScaling());
				cfMax = Max(cfMax, pqvtGetVertex(i_vt)->cfScaling());
			}
		}
	}


	//******************************************************************************************
	std::pair<CCoef, CCoef> CQuadNodeTIN::CPriv::prcfCheckCoefLimitsBranch() const
	{
		std::pair<CCoef, CCoef> prcf_lim(cfMAX, -cfMAX);

		#if VER_DEBUG
			if (bHasDescendants())
			{
				CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

				for (int i_dsc = 0; i_dsc < 4; i_dsc++)
				{
					std::pair<CCoef, CCoef> prcf_ret = static_cast<CPriv*>(pqntin_dsc)->prcfCheckCoefLimitsBranch();

					prcf_lim.first  = Min(prcf_ret.first,  prcf_lim.first);
					prcf_lim.second = Max(prcf_ret.second, prcf_lim.second);

					pqntin_dsc = pqntin_dsc->ptqnGetSibling();
				}
			}
			else
			{
				prcf_lim.first  = pqvtGetVertex(0)->cfScaling();
				prcf_lim.second = pqvtGetVertex(0)->cfScaling();

				for (int i_vt = 1; i_vt < 4; i_vt++)
				{
					prcf_lim.first  = Min(prcf_lim.first,  pqvtGetVertex(i_vt)->cfScaling());
					prcf_lim.second = Max(prcf_lim.second, pqvtGetVertex(i_vt)->cfScaling());
				}
			}

			Assert((cfMin == cfNAN) || (cfMin == prcf_lim.first));
			Assert((cfMin == cfNAN) || (cfMax == prcf_lim.second));
		#endif

		return prcf_lim;
	}


	//******************************************************************************************
	void CQuadNodeTIN::CPriv::GetObserverMinMaxDistSqr(CVector3<>& v3_min_dist, CVector3<>& v3_max_dist, const CQuadRootTIN* pqntin_root)
	{
		Assert(pqntin_root != 0);

		// Determine minimum and maximum distance in X.
		float f_dist_x_min_sqr = pqvtGetVertex(0)->iX() - pqntin_root->peiInfo->v3QuadSpaceCamPos.tX;
		float f_dist_x_max_sqr = pqvtGetVertex(2)->iX() - pqntin_root->peiInfo->v3QuadSpaceCamPos.tX;

		float f_dist_x_prod = f_dist_x_min_sqr * f_dist_x_max_sqr;

		f_dist_x_min_sqr *= f_dist_x_min_sqr;
		f_dist_x_max_sqr *= f_dist_x_max_sqr;

		if (CIntFloat(f_dist_x_min_sqr).i4Int > CIntFloat(f_dist_x_max_sqr).i4Int)
			Swap(f_dist_x_min_sqr, f_dist_x_max_sqr);

		if (CIntFloat(f_dist_x_prod).i4Int < 0)
			f_dist_x_min_sqr = 0;

		v3_min_dist.tX = f_dist_x_min_sqr;
		v3_max_dist.tX = f_dist_x_max_sqr;


		// Determine minimum and maximum distance in Y.
		float f_dist_y_min_sqr = pqvtGetVertex(0)->iY() - pqntin_root->peiInfo->v3QuadSpaceCamPos.tY;
		float f_dist_y_max_sqr = pqvtGetVertex(2)->iY() - pqntin_root->peiInfo->v3QuadSpaceCamPos.tY;

		float f_dist_y_prod = f_dist_y_min_sqr * f_dist_y_max_sqr;

		f_dist_y_min_sqr *= f_dist_y_min_sqr;
		f_dist_y_max_sqr *= f_dist_y_max_sqr;

		if (CIntFloat(f_dist_y_min_sqr).i4Int > CIntFloat(f_dist_y_max_sqr).i4Int)
			Swap(f_dist_y_min_sqr, f_dist_y_max_sqr);

		if (CIntFloat(f_dist_y_prod).i4Int < 0)
			f_dist_y_min_sqr = 0;

		v3_min_dist.tY = f_dist_y_min_sqr;
		v3_max_dist.tY = f_dist_y_max_sqr;


		// Determine minimum and maximum distance in Z.
		if (cfMin == cfNAN)
			priv_self.RecalcCoefLimitsBranch();

		float f_dist_z_min_sqr = cfMin.rGet(pqntin_root->mpConversions.rCoefToQuad) - pqntin_root->peiInfo->v3QuadSpaceCamPos.tZ;
		float f_dist_z_max_sqr = cfMax.rGet(pqntin_root->mpConversions.rCoefToQuad) - pqntin_root->peiInfo->v3QuadSpaceCamPos.tZ;

		float f_dist_z_prod = f_dist_z_min_sqr * f_dist_z_max_sqr;

		f_dist_z_min_sqr *= f_dist_z_min_sqr;
		f_dist_z_max_sqr *= f_dist_z_max_sqr;

		if (CIntFloat(f_dist_z_min_sqr).i4Int > CIntFloat(f_dist_z_max_sqr).i4Int)
			Swap(f_dist_z_min_sqr, f_dist_z_max_sqr);

		if (CIntFloat(f_dist_z_prod).i4Int < 0)
			f_dist_z_min_sqr = 0;

		v3_min_dist.tZ = f_dist_z_min_sqr;
		v3_max_dist.tZ = f_dist_z_max_sqr;
	}


	//******************************************************************************************
	void CQuadNodeTIN::CPriv::GenerateLighting
	(
		rptr<CRaster> pras_dest, CLightList& ltl_lighting, const CClut& clut,
		const CTransLinear2<>& tlr2_ras, const CQuadRootTIN* pqntin_root
	) const
	{
		if (!stState[estLEAF][estLEAF_COMBINE])
		{
			// Descend to a leaf-combine node.
			CQuadNodeTIN* pqntin_dsc = ptqnGetFirstDescendant();

			for (int i_dsc = 0; i_dsc < 4; i_dsc++)
			{
				static_cast<const CPriv*>(pqntin_dsc)->GenerateLighting(pras_dest, ltl_lighting, clut, tlr2_ras, pqntin_root);

				pqntin_dsc = pqntin_dsc->ptqnGetSibling();
			}
		}
		else
		{
			//
			// Light this map directly, by triangulating and Gouraud shading.
			//

			// Iteration has 'logical' constness.
			for (CTriangleTIN* ptri = NonConst(*this).ptriGetFirst(); ptri; ptri = ptri->ptriGetNext())
			{
				// Construct and render a CRenderPolygon to the raster.
				CLArray(SRenderVertex, arv, 3);
				SRenderVertex* aprv[3] = {&arv[0], &arv[1], &arv[2]};
				CRenderPolygon rp;

				rp.paprvPolyVertices = CPArray<SRenderVertex*>(3, aprv);

				for (int i_v = 0; i_v < 3; i_v++)
				{
					CQuadVertexTIN* pqvt = ptri->pqvtGetVertex(i_v);
					
					// Calculate the raster-space coord for this vertex using tlr2_ras.
					arv[i_v].v3Screen    = CVector2<>(pqvt->iX(), pqvt->iY()) * tlr2_ras;
					arv[i_v].cvIntensity = pqvt->cvGetLighting(0, ltl_lighting, clut, pqntin_root);
				}

				#if (VER_TEST)
					// Set area (used for stats only).
					rp.SetArea();
				#endif

				CDrawPolygon<TShadeTerrain>(pras_dest.ptGet(), rp);
			}
		}
	}


	//******************************************************************************************
	void CQuadNodeTIN::DrawQuadNode(CDraw& draw) const
	{
		if (ptxnTexture != 0)
		{
			if (ptxnTexture->bTextureInitialised())
				draw.Colour(CColour(0.0, 1.0, 1.0));
			else
				draw.Colour(CColour(0.0, 0.6, 0.6));

 			// Draw a texture node as a solid square.
			draw.MoveTo(iBaseX(),              iBaseY());
			draw.LineTo(iBaseX() + iGetSize(), iBaseY());
			draw.LineTo(iBaseX() + iGetSize(), iBaseY() + iGetSize());
			draw.LineTo(iBaseX(),              iBaseY() + iGetSize());
			draw.LineTo(iBaseX(),              iBaseY());
		}
	}


	//******************************************************************************************
	void CQuadNodeTIN::SetColour(CDraw& draw, const CTriangleTIN* pttri) const
	{
		// Set colour depending on whether this triangle has an assigned texture, and on which level.
		if (pttri->pqntinGetTextureNode() != 0)
		{
			draw.Colour(CColour(0.8, 0.0, 0.0));
		}
		else
			draw.Colour(CColour(0.4, 0.4, 0.4));
	}



//**********************************************************************************************
//
// NMultiResolution::CQuadRootTIN implementation.
//

	//******************************************************************************************
	CRangeVar<float> CQuadRootTIN::rvarPixelTolerance(1, 10, 8.0f);
	CRangeVar<float> CQuadRootTIN::rvarPixelToleranceFar(5, 50, 20.0f);
	CRangeVar<float> CQuadRootTIN::rvarDeferMultiplier(0, 20, 8);
	CRangeVar<float> CQuadRootTIN::rvarMaxDistRatio(2, 6, 2.0);
	CRangeVar<int>   CQuadRootTIN::rvarMinDistRatioNodeSize(1, 16, 8);
	CRangeVar<int>   CQuadRootTIN::rvarSigNodeDiffShift(0, 8, 3);
	CRangeVar<float> CQuadRootTIN::rvarTexelScale(.1, 1, .20);
	CRangeVar<float> CQuadRootTIN::rvarTexelScaleFar(.01, .3, .20);
	CRangeVar<float> CQuadRootTIN::rvarTextureTolerance(128, 512, 256);
	CRangeVar<float> CQuadRootTIN::rvarDisableShadowDistance(10, 500, 32.0f);
	CRangeVar<float> CQuadRootTIN::rvarDisableDynamicTextureDistance(10, 1000, 230.0f);
	CRangeVar<float> CQuadRootTIN::rvarDisableTextureDistance(10, 1000, 230.0f);
	bool             CQuadRootTIN::bFreeze(false);
	bool             CQuadRootTIN::bTexelProjNearest(true);
	bool             CQuadRootTIN::bConform(true);

	//******************************************************************************************
	CQuadRootTIN::CQuadRootTIN(const CTransformedDataHeader* ptdh)
		: CQuadRootBaseTriT<CQuadNodeTIN, CQuadVertexTIN, CTriNodeInfoTIN, CTriangleTIN>
		  (
			ptdh,
			new CQuadVertexTIN(ptdh->mpConversions.rcQuadSpaceExtents.tX0(), ptdh->mpConversions.rcQuadSpaceExtents.tY0(), ptdh->mpConversions.rcQuadSpaceExtents.tWidth(), ptdh->cfRoot),
			new CQuadVertexTIN(ptdh->mpConversions.rcQuadSpaceExtents.tX1(), ptdh->mpConversions.rcQuadSpaceExtents.tY0(), ptdh->mpConversions.rcQuadSpaceExtents.tWidth(), ptdh->cfRoot),
			new CQuadVertexTIN(ptdh->mpConversions.rcQuadSpaceExtents.tX1(), ptdh->mpConversions.rcQuadSpaceExtents.tY1(), ptdh->mpConversions.rcQuadSpaceExtents.tWidth(), ptdh->cfRoot),
			new CQuadVertexTIN(ptdh->mpConversions.rcQuadSpaceExtents.tX0(), ptdh->mpConversions.rcQuadSpaceExtents.tY1(), ptdh->mpConversions.rcQuadSpaceExtents.tWidth(), ptdh->cfRoot)
		  ),
		  peiInfo(new SEvalInfo), iLastQualitySetting(0), mpConversions(ptdh->mpConversions)
	{
		Assert(CQuadNodeTIN::pfhGetFastHeap() != 0 &&  CQuadVertexTIN::pfhGetFastHeap() != 0);
		Assert(CTriangleTIN::pfhGetFastHeap() != 0 && CTriNodeInfoTIN::pfhGetFastHeap() != 0);

		Assert(CTextureNode::ptexmTexturePages->iGetNumPages() != 0);

		cfMin = cfNAN;
		cfMax = cfNAN;

		// Assign the initial dummy texture for the root node.
		ptxnTexture = new CTextureNode(this);
	}


	//******************************************************************************************
	CQuadRootTIN::~CQuadRootTIN()
	{
		DecimateBranch();

		delete peiInfo;
	}


	//******************************************************************************************
	void CQuadRootTIN::AllocMemory(uint u_max_nodes)
	{
		Assert(CQuadNodeTIN::pfhGetFastHeap() == 0 &&  CQuadVertexTIN::pfhGetFastHeap() == 0);
		Assert(CTriangleTIN::pfhGetFastHeap() == 0 && CTriNodeInfoTIN::pfhGetFastHeap() == 0);

		// Allocate and set the fastheap used for allocations of the tin quad tree classes.
		uint u_max_node_bytes     = sizeof(   CQuadNodeTIN) * u_max_nodes;
		uint u_max_vert_bytes     = sizeof( CQuadVertexTIN) * u_max_nodes * dAVG_QUAD_NODE_TO_VERTEX_RATIO;
		uint u_max_tri_bytes      = sizeof(   CTriangleTIN) * u_max_nodes * dAVG_QUAD_NODE_TO_TRIANGLE_RATIO;
		uint u_max_tri_info_bytes = sizeof(CTriNodeInfoTIN) * u_max_nodes * dAVG_QUAD_NODE_TO_TRIANGLE_INFO_RATIO;

		CFastHeap* pfh = new CFastHeap(u_max_node_bytes + u_max_vert_bytes + u_max_tri_bytes + u_max_tri_info_bytes);

		   CQuadNodeTIN::SetFastHeap(pfh);
		 CQuadVertexTIN::SetFastHeap(pfh);
		   CTriangleTIN::SetFastHeap(pfh);
	    CTriNodeInfoTIN::SetFastHeap(pfh);

	    CTextureNode::SetFastHeap(pfh);
	}


	//**********************************************************************************************
	void CQuadRootTIN::FreeMemory()
	{
		// Delete the memory used for allocations of the tin quad tree classes.
		CFastHeap* pfh  =   CQuadNodeTIN::pfhGetFastHeap();

		Assert(CTriangleTIN::pfhGetFastHeap() == pfh && CTriNodeInfoTIN::pfhGetFastHeap() == pfh && CQuadVertexTIN::pfhGetFastHeap() == pfh);

		   CQuadNodeTIN::SetFastHeap(0);
		 CQuadVertexTIN::SetFastHeap(0);
		   CTriangleTIN::SetFastHeap(0);
	    CTriNodeInfoTIN::SetFastHeap(0);

	    CTextureNode::SetFastHeap(0);

		delete pfh;
	}

	//******************************************************************************************
	void CQuadRootTIN::Update(const CCamera* pcam_view, bool b_force_eval)
	{
		Assert(pcam_view != 0);

		if (!bAverageStats)
			psWaveletTIN.Reset();

		if (CQuadRootTIN::bFreeze)
			return;

		CCycleTimer ctmr_update;

		// If the quality setting has changed, force all the terrain textures and geometry to update.
		int i_curr_quality = iGetQualitySetting();

		if (i_curr_quality != iLastQualitySetting)
		{
			iLastQualitySetting = i_curr_quality;

			b_force_eval = true;
			CTextureNode::PurgeAllTextures();
		}

		CVector3<> v3_cam_world_pos = pcam_view->v3Pos();

		float f_world_to_pixel_ratio = pcam_view->rGetProjectPlaneDist() *
		                               pcam_view->tlr2GetProjectPlaneToScreen().tlrX.tScale;

		// Determine the radius of the bounding sphere of the camera volume.
		CSArray<CVector3<>, 8> sav3_cam_volume;
		pcam_view->WorldExtents(sav3_cam_volume);

		TReal r_view_radius_sqr = 0;

		for (int i_pt = 0; i_pt < 4; i_pt++)
			r_view_radius_sqr = Max(r_view_radius_sqr, (sav3_cam_volume[i_pt] - v3_cam_world_pos).tLenSqr());

		// Mapping constants.
		float f_world_to_quad = mpConversions.tlr2WorldToQuad.tlrX.tScale;
		float f_quad_to_coef  = mpConversions.tlr2QuadToWorld.tlrX.tScale * mpConversions.rWorldToCoef;

		// Adjust distance cut-outs by the quality setting.
		float f_dist_adj = qdQualitySettings[i_curr_quality].fTerrainDistanceScale;
		float f_pixtol_adj = qdQualitySettings[i_curr_quality].fPixelTolAdj;

		// Initialise evaluation info structure.
		peiInfo->v3QuadSpaceCamPos             = CVector2<>(v3_cam_world_pos) * mpConversions.tlr2WorldToQuad;
		peiInfo->v3QuadSpaceCamPos.tZ          = v3_cam_world_pos.tZ * f_world_to_quad;
		peiInfo->fWorldToPixRatio              = f_world_to_pixel_ratio;
		peiInfo->fNormSqrQuadDistFactor        = Sqr(1.0f / (fWorldFarNormaliseDist * f_world_to_quad));

		peiInfo->fPixTolToCoef                 =  (CQuadRootTIN::rvarPixelTolerance * f_pixtol_adj) / 
												   f_world_to_pixel_ratio * f_quad_to_coef;

		peiInfo->fPixTolDeltaToCoef            = ((CQuadRootTIN::rvarPixelToleranceFar - 
												  CQuadRootTIN::rvarPixelTolerance ) * f_pixtol_adj) / 
												  f_world_to_pixel_ratio * f_quad_to_coef;

		peiInfo->fTexTolToCoef                 =  CQuadRootTIN::rvarTextureTolerance      / f_world_to_pixel_ratio * f_quad_to_coef;

		peiInfo->iDistRatioMinNodeSize         = Max(1, iPosFloatCast(CQuadRootTIN::rvarMinDistRatioNodeSize * f_world_to_quad));
		peiInfo->fMaxDistRatioSqr              = Sqr(float(CQuadRootTIN::rvarMaxDistRatio));

		peiInfo->fDisableShadowDistSqr         = Sqr(CQuadRootTIN::rvarDisableShadowDistance         * f_dist_adj * f_world_to_quad);
		peiInfo->fDisableDynamicTextureDistSqr = Sqr(CQuadRootTIN::rvarDisableDynamicTextureDistance * f_dist_adj * f_world_to_quad);
		peiInfo->fDisableTextureDistSqr        = Sqr(CQuadRootTIN::rvarDisableTextureDistance        * f_dist_adj * f_world_to_quad);

		peiInfo->fFarViewDist                  = sqrt(r_view_radius_sqr) * f_world_to_quad;


		// Evaluate geometry.
		CCycleTimer ctmr_eval;

		if (b_force_eval)
			ResetDeferredEvalBranch();

		iEvaluateBranch(this);

		CQuadRootTIN::psEvaluateBranch.Add(ctmr_eval(), 1);


		// Do texturing;
		CCycleTimer ctmr_tex;

		EvaluateTextureBranch(this);

		AssignTexturesBranch(this);

		CheckTexturesBranch();

		CQuadRootTIN::psTextureBranch.Add(ctmr_tex(), 1);

		#if VER_DEBUG
			priv_selfc.prcfCheckCoefLimitsBranch();
		#endif

		TCycles cy = ctmr_update();
		CQuadRootTIN::psUpdate.Add(cy, 1);
		proProfile.psTerrainUpdate.Add(cy, 1);
	}


	//******************************************************************************************
	void CQuadRootTIN::UpdateMovingObjShadows(const CMessageMove& msgmv)
	{
		Assert(msgmv.pinsMover != 0 && msgmv.pinsMover->pdGetData().bCastShadow);

		if (msgmv.pinsMover->bNoSpatialInfo() || !msgmv.pinsMover->pshGetShape())
			return;

		// To do: make this faster, along with other bounding volume code.  Use shear/matrix instead of rotate.

		// There is a region with a finite volume. Construct a 2D bounding rectangle for it.
		CTransform3<> tf3_to_terrain = msgmv.pinsMover->pr3Presence();

		// Move Z value relative to terrain.
		CVector2<> v2_quad = CVector2<>(tf3_to_terrain.v3Pos) * mpConversions.tlr2WorldToQuad;

		TReal r_height = rGetWorldZ(v2_quad.tX, v2_quad.tY);

		tf3_to_terrain.v3Pos.tZ -= r_height;

		// Adjust transform for shadowing.
		// Shear object in opposite direction of shadow, relative to Z height.
		CDir3<> d3_shadow = -CTextureNode::d3ShadowingLight();

		if (d3_shadow.tZ)
		{
			TReal r_inv_z_shadow = -1.0 / d3_shadow.tZ;
			CShear3<> sh3_shadow_adjust('z', d3_shadow.tX * r_inv_z_shadow, d3_shadow.tY * r_inv_z_shadow);
			tf3_to_terrain *= sh3_shadow_adjust;
		}

		// Get the extents of the current volume in this space.
		CVector3<> v3_min, v3_max;
		msgmv.pinsMover->pshGetShape()->GetExtents(msgmv.pinsMover, tf3_to_terrain, v3_min, v3_max);

		// Construct rectangle containing XY of region.
		CRectangle<> rc_region(v3_min, v3_max - v3_min);

		// Convert to quad space.
		rc_region *= mpConversions.tlr2WorldToQuad;

		// Convert this rectangle to integer coordinates, rounding to ensure the rectangle bounds
		// the volume in quad tree space.
		int i_x0 = iTrunc(rc_region.tX0());
		int i_y0 = iTrunc(rc_region.tY0());
		int i_x1 = iTrunc(rc_region.tX1() + 1.0);
		int i_y1 = iTrunc(rc_region.tY1() + 1.0);

		CRectangle<int> rc_region_int(i_x0, i_y0, i_x1 - i_x0, i_y1 - i_y0);

		MovingObjShadowsBranch(rc_region_int, rc_region, msgmv);
	}


	//******************************************************************************************
	void CQuadRootTIN::UpdateTextures(const CPartition* ppart_region, const CPlacement3<>& p3_prev)
	{
		if (ppart_region->bNoSpatialInfo())
		{
			// Update everything.
			PurgeTexturesBranch(rcGetRectangle());
		}
		else
		{
			// To do: make this faster, along with other bounding volume code.  Use shear/matrix instead of rotate.

			// There is a region with a finite volume. Construct a 2D bounding rectangle for it.
			CTransform3<> tf3_to_terrain = ppart_region->pr3Presence();

			// Get the extents of the current volume in this space.
			CVector3<> v3_min, v3_max;
			ppart_region->pbvBoundingVol()->GetWorldExtents(tf3_to_terrain, v3_min, v3_max);

			// Construct rectangle containing XY of region.
			CRectangle<> rc_region(v3_min, v3_max - v3_min);

			// Convert to quad space.
			rc_region *= mpConversions.tlr2WorldToQuad;

			PurgeTexturesBranch(rc_region);

			{
				// Construct presence from previous placement and current scale.
				tf3_to_terrain = CPresence3<>(p3_prev, ppart_region->fGetScale());

				ppart_region->pbvBoundingVol()->GetWorldExtents(tf3_to_terrain, v3_min, v3_max);
	
				rc_region = CRectangle<>(v3_min, v3_max - v3_min);

				// Convert to quad space.
				rc_region *= mpConversions.tlr2WorldToQuad;

				PurgeTexturesBranch(rc_region);
			}
		}
	}


	//******************************************************************************************
	TReal CQuadRootTIN::rGetWorldZ(TReal r_quad_x, TReal r_quad_y) const
	{
		// Find the enclosing triangle.
		CTriangleTIN* ptri_enc = ptriFindEnclosingTriangle(r_quad_x, r_quad_y);

		if (ptri_enc == 0)
			return 0;

		// Interpolate Z across triangle.
		if (!ptri_enc->bHasValidPlane())
			ptri_enc->CalcPlane(mpConversions);

		CPlane pl_enc = ptri_enc->plGetPlaneQuad();

		TReal r_quad_z = -(pl_enc.d3Normal.tX * r_quad_x + pl_enc.d3Normal.tY * r_quad_y + pl_enc.rD) / pl_enc.d3Normal.tZ;

		// Convert quad space Z to world space.
		return r_quad_z * mpConversions.tlr2QuadToWorld.tlrX.tScale;
	}


	//******************************************************************************************
	CDir3<> CQuadRootTIN::d3GetNormal(const CQuadVertexTIN* pqvt, const CQuadNodeTIN* ptqn) const
	{
		Assert(pqvt);

		//
		// For each axis, find the slopes on each side of the vertex, and average them.
		//
		TReal r_dx = 0, r_dy = 0;

		TReal r_zv = pqvt->rZQuad(mpConversions);

		// Find node containing pqvt.
		if (!ptqn)
			ptqn = ptqnFindLeaf(pqvt->iX(), pqvt->iY());
		if (ptqn)
		{
			TReal r_inv_size = 1.0 / ptqn->iGetSize();

			// For +x slope, get interpolated Z value on right edge at pqtv->iY().
			TReal r_z = (ptqn->pqvtGetVertex(1)->rZQuad(mpConversions) * 
						 (ptqn->pqvtGetVertex(2)->iY() - pqvt->iY()) +
						 ptqn->pqvtGetVertex(2)->rZQuad(mpConversions) * 
						 (pqvt->iY() - ptqn->pqvtGetVertex(1)->iY())) * 
						 r_inv_size;

			// Calculate slope for +x by dividing by dx.
			r_dx += (r_z - r_zv) * r_inv_size;

			// For +y slope, get interpolated Z value on top edge at pqtv->iX().
			// but omit final divide by dx.
			      r_z = (ptqn->pqvtGetVertex(3)->rZQuad(mpConversions) * 
						 (ptqn->pqvtGetVertex(2)->iX() - pqvt->iX()) +
						 ptqn->pqvtGetVertex(2)->rZQuad(mpConversions) * 
						 (pqvt->iX() - ptqn->pqvtGetVertex(3)->iX())) * 
						 r_inv_size;

			// Calculate slope for +y by dividing by dy.
			r_dy += (r_z - r_zv) * r_inv_size;
		}

		// For -x slope, find nearest node in -x.
		if (ptqn = ptqnFindLeaf(pqvt->iX() - 1, pqvt->iY()))
		{
			TReal r_inv_size = 1.0 / ptqn->iGetSize();

			// Get interpolated Z value on left edge at pqtv->iY().
			TReal r_z = (ptqn->pqvtGetVertex(0)->rZQuad(mpConversions) * 
						 (ptqn->pqvtGetVertex(3)->iY() - pqvt->iY()) +
						 ptqn->pqvtGetVertex(3)->rZQuad(mpConversions) * 
						 (pqvt->iY() - ptqn->pqvtGetVertex(0)->iY())) * 
						 r_inv_size;

			// Calculate slope for -y by dividing by dx.
			r_dy += (r_zv - r_z) * r_inv_size;
		}

		// For -y slope, find nearest node in -y.
		if (ptqn = ptqnFindLeaf(pqvt->iX(), pqvt->iY() - 1))
		{
			TReal r_inv_size = 1.0 / ptqn->iGetSize();

			// Get interpolated Z value on bottom edge at pqtv->iX().
			TReal r_z = (ptqn->pqvtGetVertex(0)->rZQuad(mpConversions) * 
						 (ptqn->pqvtGetVertex(1)->iX() - pqvt->iX()) +
						 ptqn->pqvtGetVertex(1)->rZQuad(mpConversions) * 
						 (pqvt->iX() - ptqn->pqvtGetVertex(0)->iX())) * 
						 r_inv_size;

			// Calculate slope for -x by dividing by dy.
			r_dy += (r_zv - r_z) * r_inv_size;
		}

		// From these slopes, construct the normal.
		return CDir3<>(-r_dx, -r_dy, 2.0);
	}


	//******************************************************************************************
	void CQuadRootTIN::InitTriangleList(CDArray<CTriangleTIN*>& daptri, const CClipRegion2D& clip2d, const CVector3<>& v3_pos, TReal r_pl_thickness)
	{
		// Make sure the quad tree has been evaluated at least once before we iterate its triangles.
		if (bHasDescendants())
		{
			SIterateContext itc(daptri, this, clip2d, v3_pos, r_pl_thickness);

			InitTriangleListBranch(itc);
		}
	}


	//******************************************************************************************
	void CQuadRootTIN::PrintStats(CConsoleBuffer& con) const
	{
		con.Print("TIN quad tree    Curr            Max\n");

		CQuadRootBaseTriT<CQuadNodeTIN, CQuadVertexTIN, CTriNodeInfoTIN, CTriangleTIN>::PrintStats(con);

		con.Print
		(
			" Texture nodes: %5d  %3dKB   %5d  %3dKB\n",
			CTextureNode::uNumAlloc(), (CTextureNode::uNumAlloc() * sizeof(CTextureNode) + 512) / 1024,
			CTextureNode::uMaxAlloc(), (CTextureNode::uMaxAlloc() * sizeof(CTextureNode) + 512) / 1024
		);

		con.Print
		(
			"Mem commit: %dKB, free: %dKB, reserved: %dKB\n",
			(CQuadNodeTIN::pfhGetFastHeap()->uNumBytesUsed() + 512) / 1024,
		    (CQuadNodeTIN::pfhGetFastHeap()->uNumBytesFree() + 512) / 1024,
			(CQuadNodeTIN::pfhGetFastHeap()->uGetSize()      + 512) / 1024
		);
	}



//**********************************************************************************************
//
// NMultiResolution::CQuadRootTINShape implementation.
//


	//******************************************************************************************
	CQuadRootTINShape::CQuadRootTINShape(const CTransformedDataHeader* ptdh)
		: CQuadRootTIN(ptdh)
	{
	}


	//******************************************************************************************
	CQuadRootTINShape::~CQuadRootTINShape()
	{
	}

	//******************************************************************************************
	TReal CQuadRootTINShape::rPolyPlaneThickness() const
	{
		if (d3dDriver.bUseD3D())
			return CShape::rPolyPlaneThicknessD3D;
		else
			return CShape::rPolyPlaneThicknessDefault;
	}


	//**********************************************************************************************
	//
	// CPolyIterator implementation.
	//

		#define uMAX_TIN_TRIANGLES	uint(40000)		// The maximum number of triangles in iteration.

		//******************************************************************************************
		CQuadRootTINShape::CPolyIterator::CPolyIterator(const CQuadRootTINShape& qntinsh, const CInstance*, const CRenderContext* prenc)
			: qntinshShape(const_cast<CQuadRootTINShape&>(qntinsh)), prencContext(prenc),
			  daptriCurrentTris(uMAX_TIN_TRIANGLES)
		{
			CClipRegion2D clip2d(prenc->Camera);

			// Calculate the world to quad space transform.
			CPresence3<> pr3_world_to_quad
			(
				CRotate3<>(),
				qntinshShape.mpConversions.tlr2WorldToQuad.tlrX.tScale,
				CVector3<>(qntinshShape.mpConversions.tlr2WorldToQuad.tlrX.tOffset,
						   qntinshShape.mpConversions.tlr2WorldToQuad.tlrY.tOffset, 0)
			);

			clip2d *= pr3_world_to_quad;

			qntinshShape.InitTriangleList(daptriCurrentTris, clip2d, prenc->Camera.v3Pos() * pr3_world_to_quad, qntinshShape.rPolyPlaneThickness());

			// Reset iterators.
			pptriCurrent = daptriCurrentTris.begin() - 1;
			ptriCurrent = 0;
		}


		//******************************************************************************************
		CQuadRootTINShape::CPolyIterator::~CPolyIterator()
		{
			// Todo: handle failed uploads gracefully by i.e. killing the referenced terrain textures.
			// For now we'll assume the upload is always successful.
			CTextureNode::ptexmTexturePages->bUploadPages();
		}


		//******************************************************************************************
		const CTexture* CQuadRootTINShape::CPolyIterator::ptexTexture()
		{
			CQuadNodeTIN* pqntin_tex = ptriCurrent->pqntinGetTextureNode();

			Assert(pqntin_tex != 0);

			const CTextureNode* ptxn = pqntin_tex->ptxnGetTextureNode();

			Assert(ptxn != 0);

			if (!prencContext->Renderer.pSettings->bTargetCache && ptxn->iGetLastScheduled() != 0 && !CQuadRootTIN::bFreeze)
			{
				CCycleTimer ctmr;

				pqntin_tex->EvaluateTexture(&qntinshShape);

				CQuadRootTINShape::psEvaluateTexture.Add(ctmr(), 1);
			}

			const CTexture* ptex = ptxn->ptexGetTexture().ptGet();

			// Compute all the texture coords now.
			atcCoords[0] = ptxn->tcObjectToTexture(ptriCurrent->pqvtGetVertex(0)->iX(), ptriCurrent->pqvtGetVertex(0)->iY());
			atcCoords[1] = ptxn->tcObjectToTexture(ptriCurrent->pqvtGetVertex(1)->iX(), ptriCurrent->pqvtGetVertex(1)->iY());
			atcCoords[2] = ptxn->tcObjectToTexture(ptriCurrent->pqvtGetVertex(2)->iX(), ptriCurrent->pqvtGetVertex(2)->iY());

			return ptex;
		}


		//******************************************************************************************
		void CQuadRootTINShape::CPolyIterator::TransformPoints
		(
			const CTransform3<>&	tf3_shape_camera,
			const CCamera&			cam,
			CPArray<SClipPoint>		paclpt_points,
			bool					b_outcodes
		)
		{
			// Init the entire array's seteocOut to Set(eocUNINIT).
			for (int i = 0; i < paclpt_points.uLen; i++)
				paclpt_points[i].seteocOut = Set(eocUNINIT);

			const CBoundVolCamera* pbvcam = cam.pbvcamClipVolume();


			#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
				float fNegUnliftCoefToQuad = -qntinshShape.mpConversions.rCoefToQuad / 8;
				const CTransform3<>* tf3 = &tf3_shape_camera;

				const uint u_down_left_masks[2] = {1<<eocDOWN, 1<<eocLEFT};
				const uint u_up_right_masks[2]  = {1<<eocUP,   1<<eocRIGHT};
				const uint u_near_far_masks[2]  = {1<<eocNEAR, 1<<eocFAR};

				#define fCAMERA_PLANE_TOLERANCE	1e-4
				const float f_tolerance   = fCAMERA_PLANE_TOLERANCE;
				const float f_toleranceP1 = fCAMERA_PLANE_TOLERANCE + 1.0f;

				__asm femms							//ensure fast switch for below
			#else

				// Get quad to camera transform, including negated lifted dequantisation scalar for Z axis.
				CTransform3<> tf3_shapequad_cam = CScale3<>(1, 1, -qntinshShape.mpConversions.rCoefToQuad / 8) *
				                                  tf3_shape_camera;

			#endif

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

					#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

						// Following transform code does:
						//   paclpt_points[i_index].v3Point = v3_quad * tf3_shape_camera;
						// where:
						//   CVector3<> v3_quad = pqvt->v3Quad(qntinshShape.mpConversions);
						// which is the same as:
						//   CVector3<> v3_quad(pqvt->iX(), 
						//                      pqvt->iY(), 
						//                      pqvt->cfScaling().iGet() * qntinshShape.mpConversions.rCoefToQuad
						//                     );
						// which is the same as:
						//   CVector3<> v3_quad(pqvt->iX(), 
						//                      pqvt->iY(), 
						//                      -pqvt->cfNegUnliftedScaling().iGet()/8 * qntinshShape.mpConversions.rCoefToQuad
						//                     );

						// Assumptions made by this code:
						// This code only works if pbvcam points to a CCameraDefPerspectiveNorm!!!
						//   (since seteocOutCodes differs for each type of CBoundVolCamera that 
						//    pbvcam can point to)
						// ((char *)&pqvt->u2Y - (char *)&pqvt->u2X == 2);
						// (fCAMERA_PLANE_TOLERANCE	== 1e-4) to match #define in GeomTypesCamera.cpp
						// ((char *)&pbvcam->CCameraDefPerspectiveNorm.clpNear.rPos - (char *)&pbvcam == 24);
						// ((char *)&pbvcam->CCameraDefPerspectiveNorm.clpFar.rPos - (char *)&pbvcam == 32);
						//   (these last three really need to be replaced by proper symbolic references)

						typedef CTransform3<> tdCTransform3;

						int i_scale = pqvt->cfNegUnliftedScaling().iGet();

						SClipPoint* pclpt = &paclpt_points[i_index];

						__asm
						{
							mov		eax,[pqvt]
							mov		edx,[pbvcam]

							movd	mm2,[i_scale]
							jmp		StartAsm

							align	16
						StartAsm:
							movd	mm0,[eax]CQuadVertex.u2X	;m0=  0  | u2Y:u2X
							pxor	mm1,mm1						;m1=  0  |  0

							movd	mm3,[fNegUnliftCoefToQuad]
							pi2fd	(m2,m2)						;m2= f_scale

							mov		ebx,[tf3]
							punpcklwd mm0,mm1					;m0= u2Y | u2X

							pi2fd	(m0,m0)						;m0=  fY |  fX
							pfmul	(m2,m3)						;m2=   0 |  fZ

							mov		ecx,[pclpt]					;ecx= ptr to current point
							dec		ebx							;force non-zero disp's in following uses

							movq	mm1,mm0						;m1= Y | X
							punpckldq mm0,mm0					;m0= X | X

							movd	mm3,[ebx+1]tdCTransform3.mx3Mat.v3X.tZ ;m3= m02
							punpckhdq mm1,mm1							;m1= Y | Y

							movd	mm4,[ebx+1]tdCTransform3.mx3Mat.v3Y.tZ ;m4= m12
							punpckldq mm2,mm2							;m2= Z | Z

							movd	mm5,[ebx+1]tdCTransform3.mx3Mat.v3Z.tZ ;m5= m22
							pfmul	(m3,m0)								;m3= m02*X

							movd	mm6,[ebx+1]tdCTransform3.v3Pos.tZ	;m6= m32
							pfmul	(m4,m1)								;m4= m12*Y

							movq	mm7,[ebx+1]tdCTransform3.mx3Mat.v3X.tX ;m7= m01 | m00
							pfmul	(m5,m2)								;m5= m22*Z

							pfadd	(m4,m3)								;m4= m02*X + m12*Y
							movq	mm3,[ebx+1]tdCTransform3.mx3Mat.v3Y.tX ;m3= m11 | m10

							pfadd	(m6,m5)								;m6= m22*Z + m32
							movq	mm5,[ebx+1]tdCTransform3.mx3Mat.v3Z.tX ;m5= m21 | m20

							pfmul	(m7,m0)								;m7= m01*X | m00*X
							movd	mm0,[f_tolerance]			;m0= f_tolerance

							pfadd	(m6,m4)								;m6= resultZ
							pfmul	(m3,m1)								;m3= m11*Y | m10*Y

							movq	mm4,[ebx+1]tdCTransform3.v3Pos.tX	;m4= m31 | m30
							pfmul	(m5,m2)								;m5= m21*Z | m20*Z

							movd	[ecx]SClipPoint.v3Point.tZ,mm6
							pfadd	(m7,m3)								;m7= m01*X + m11*Y | m00*X + m10*Y

							movd	mm2,[edx+24]				;m2= CCameraDefPerspectiveNorm.clpNear.rPos
							pfadd	(m5,m4)								;m5= m21*Z + m31   | m20*Z + m30

							movd	mm3,[edx+32]				;m3= CCameraDefPerspectiveNorm.clpFar.rPos
							nop									;maintain decode pairing

							pfadd	(m7,m5)								;m7=    resultY    | resultX
							movq	[ecx]SClipPoint.v3Point.tX,mm7

							movq	mm1,mm0						;m1= f_tolerance
							pfmul	(m0,m2)						;m0= clpNear.rPos * f_tolerance

							pfmul	(m1,m3)						;m1= clpFar.rPos * f_tolerance
							movq	mm5,[u_near_far_masks]		;m5=  eocFAR | eocNEAR

							pfsub	(m0,m2)						;m0= clpNear.rPos * f_tolerance - clpNear.rPos
							movq	mm4,[u_down_left_masks]		;m4= eocLEFT | eocDOWN

							pfadd	(m1,m3)						;m1= clpFar.rPos * f_tolerance + clpFar.rPos
							nop									;1-byte NOOP to avoid degraded predecode

							punpckldq mm6,mm7					;m6= X | Z
							psrlq	mm7,32						;m7= 0 | Y

							movd	mm3,[f_toleranceP1]			;m3= f_tolerance + 1
							test	eax,eax						;2-byte NOOP to avoid degraded predecode

							movq	mm2,mm7						;m2= 0 | Y
							psllq	mm7,32						;m7= Y | 0

							pfsubr	(m7,m2)						;m7= -Y | Y
							pfmul	(m2,m3)						;m2= Y*f_toleranceP1

							punpckldq mm0,mm1					;m0= clipFar_const | clipNear_const
							nop									;1-byte NOOP to avoid degraded predecode
							movq	mm1,[u_up_right_masks]		;m1= eocRIGHT | eocUP
							test	eax,eax						;2-byte NOOP to avoid degraded predecode

							pfadd	(m7,m0)						;m7= clipFar_const-Y | clipNear_const+Y
							punpckldq mm2,mm2					;m2= Y*f_toleranceP1 | Y*f_toleranceP1

							movq	mm3,mm2						;m3= Y*f_toleranceP1 | Y*f_toleranceP1
							pfadd	(m2,m6)						;m2= Y*f_toleranceP1+X | Y*f_toleranceP1+Z

							psrad	mm7,31						;m7= sign(fFar) | sign(fNear)
							pfsub	(m3,m6)						;m3= Y*f_toleranceP1-X | Y*f_toleranceP1-Z

							psrad	mm2,31						;m2= sign(fLeft) | sign(fDown)
							pand	mm7,mm5						;m7= eocFAR? | eocNEAR?

							psrad	mm3,31						;m3= sign(fRight) | sign(fUp)
							pand	mm2,mm4						;m2= eocLEFT? | eocDOWN?

							por		mm7,mm2						;m7= accumulate eoc's
							pand	mm3,mm1						;m3= eocRIGHT? | eocUP?

							por		mm7,mm3						;m7= accumulate eoc's

							movq	mm6,mm7						;m6= copy of low eoc accumulation
							punpckhdq mm7,mm7					;m7= copy of hight eoc accumulation

							por		mm7,mm6						;m7= accumulation of all six eoc's
							movd	[ecx]SClipPoint.seteocOut,mm7
						}

					#else // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

						CVector3<> v3_vt(pqvt->iX(), pqvt->iY(), pqvt->cfNegUnliftedScaling().iGet());

						paclpt_points[i_index].v3Point   = v3_vt * tf3_shapequad_cam;
						paclpt_points[i_index].seteocOut = pbvcam->seteocOutCodes(paclpt_points[i_index].v3Point);

					#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
					}
				}
			}

			#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
				__asm femms						//clear MMX state and ensure fast switch
			#endif
		}

};

