/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Terrain.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/Terrain.cpp                                             $
 * 
 * 139   10/01/98 4:05p Mlange
 * Improved move message stat reporting.
 * 
 * 138   9/27/98 8:35p Mlange
 * Added clrGetBaseTextureColour() function.
 * 
 * 137   9/09/98 11:31a Pkeet
 * Needed to add an include to be compatible with a debug switch in 'Texture.hpp.'
 * 
 * 136   8/13/98 6:10p Mlange
 * Paint message now requires registration of the recipients.
 * 
 * 135   8/11/98 8:29p Mlange
 * Entities must now register themselves with each message type in which they are interested.
 * 
 * 134   98/07/30 22:16 Speter
 * FrameBegin now updates terrain based on CMessageStep frame count, rather than paint frame
 * count, which makes for consistent replays. Added move process stat.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Terrain.hpp"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/GeomDBase/TerrainLoad.hpp"
#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/GeomDBase/WaveletQuadTreeQuery.hpp"
#include "Lib/GeomDBase/WaveletStaticData.hpp"
#include "Lib/GeomDBase/TerrainTexture.hpp"
#include "Lib/GeomDBase/TerrainObj.hpp"
#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/GeomDBase/RayCast.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPaint.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/View/LineDraw.hpp"
#include "Lib/Loader/SaveFile.hpp"


//**********************************************************************************************
//
class CPhysicsInfoTerrain: public CPhysicsInfo
//
// A physics info for terrain which specialises sound material extraction and ray intersection.
//
//**************************************
{
public:

	//**********************************************************************************************
	CPhysicsInfoTerrain()
		: CPhysicsInfo(SPhysicsData(Set(epfTANGIBLE)), matHashIdentifier("TERRAIN"))
	{
	}

	//**********************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual TSoundMaterial smatGetMaterialType() const override
	{
		return tmatSoundMaterial;
	}

	//*****************************************************************************************
	virtual TSoundMaterial smatGetMaterialType(const CVector3<>& v3_world) const override
	{
		// Get the sound material at this location.
		const CTerrainObj* ptobj = wWorld.ptobjGetTopTerrainObjAt(v3_world.tX, v3_world.tY);

		if (ptobj)
			return ptobj->matSoundMaterial;
		else
			// Only data driven sound materials, please.
			//return tmatSoundMaterial;
			return 0;
	}

	//**********************************************************************************************
	virtual void RayIntersect(CInstance* pins, int i_subobj, CRayCast& rc,
							  const CPlacement3<>& p3, TReal r_length, TReal r_diameter) const override
	{
		CTerrain* ptrr = ptCast<CTerrain>(pins);
		AlwaysAssert(ptrr);

		// Extract start and end points.
		CVector3<> v3_start = p3.v3Pos;
		CVector3<> v3_end = CVector3<>(0, r_length, 0) * p3;
		CVector3<> v3_loc;

		SObjectLoc obl;

		// Get the intersection from the terrain.
		if (ptrr->pqnqGetQueryRoot()->bRayIntersects(&obl, v3_start, v3_end))
		{
			obl.pinsObject = pins;
			obl.iSubObject = i_subobj;

			rc.InsertIntersection(obl);
		}
	}
};

//**********************************************************************************************
//
// CTerrain implementation.
//
	//******************************************************************************************
	CRangeVar<float> CTerrain::rvarWireZoom(.5, 20, 1);

	bool CTerrain::bShowWireframe = false;
	bool CTerrain::bShowQuadtree  = false;

	//******************************************************************************************
	CTerrain::CTerrain(const SInit& initins)
		: CSubsystem(SInit(rptr0, 0, 0, "Terrain"))
	{
		// There can be only one terrain!
		Assert(CWDbQueryTerrain().tGet() == 0);

		ptdhData = ptdhLoadTerrainData
		(
			// Get the terrain file name from the instance's name.
			initins.strName.substr(initins.strName.find('_') + 1)
		);


		// The following sets up a default terrain texture.
		rptr<CMesh> pmsh = ptCastRenderType<CMesh>(initins.prdtRenderInfo);

		if (pmsh == 0 || pmsh->pasfSurfaces[0].ptexTexture == 0)
			TerminalError(ERROR_ASSERTFAIL, true, "Terrain placement object has no mesh and/or texture!");

		rptr<CTexture> ptex = pmsh->pasfSurfaces[0].ptexTexture;

		// Pass any default texture to the texturing system.
		NMultiResolution::CTextureNode::SetBaseTexture(ptex);

	    NMultiResolution::CTextureNode::SetTextureMemSize();
		NMultiResolution::AllocMemory();

		pqntinRoot = rptr_new NMultiResolution::CQuadRootTINShape(ptdhData);
		pqnqRoot   =      new NMultiResolution::CQuadRootQuery(ptdhData);

		// Do first quad tree update.
		pqntinRoot->Update(CWDbQueryActiveCamera().tGet());

		Assert(pqntinRoot->mpConversions.tlr2QuadToWorld.tlrX.tScale ==
		       pqntinRoot->mpConversions.tlr2QuadToWorld.tlrY.tScale   );

		// Set the terrain's presence according to the quad to world space transformation.
		SetPresence
		(
			CPresence3<>
			(
				CRotate3<>(),
				pqntinRoot->mpConversions.tlr2QuadToWorld.tlrX.tScale,
				CVector3<>
				(
					pqntinRoot->mpConversions.tlr2QuadToWorld.tlrX.tOffset,
					pqntinRoot->mpConversions.tlr2QuadToWorld.tlrY.tOffset,
					0
				)
			)
		);

		//
		// The quadtree root serves as the rendering shape.
		//
		SetRenderInfo(rptr_static_cast(CRenderType, pqntinRoot));

		// Set the custom CPhysicsInfo.
		SetPhysicsInfo(new CPhysicsInfoTerrain());

		// Save the default settings.
		pc_defaults = 0;
		SaveDefaults();

		// Register this entity with the message types it needs to receive.
		CMessagePaint::RegisterRecipient(this);
		 CMessageMove::RegisterRecipient(this);
	}

	//******************************************************************************************
	CTerrain::~CTerrain()
	{
		 CMessageMove::UnregisterRecipient(this);
		CMessagePaint::UnregisterRecipient(this);

		SetRenderInfo(rptr0);

		pqntinRoot = rptr0;
		delete pqnqRoot;

		NMultiResolution::CTextureNode::SetTextureMemSize(0);
		NMultiResolution::FreeMemory();
		NMultiResolution::CTextureNode::SetBaseTexture(rptr0);

		delete ptdhData;

		delete pc_defaults;
	}

	//*****************************************************************************************
	bool CTerrain::bGetWorldExtents(CVector3<>& v3_min, CVector3<>& v3_max) const
	{
		CRectangle<> rc_world = pqnqRoot->rcWorldRectangle(pqnqRoot->mpConversions);

		v3_min = rc_world.v2Start();
		v3_max = rc_world.v2End();

		return true;
	}
	
	//******************************************************************************************
	TReal CTerrain::rHeight(TReal r_x, TReal r_y)
	{
		// Query terrain with a very small bounding volume containing the requested point.
		CVector3<> v3_terrain = CVector3<>(r_x, r_y, 0);

		NMultiResolution::CQueryRect qr
		(
			pqnqRoot,
			CBoundVolSphere(FLT_EPSILON * 10),
			CPresence3<>(v3_terrain)
		);

		return qr.rHeight(v3_terrain.tX, v3_terrain.tY);
	}


	//******************************************************************************************
	TReal CTerrain::rHeightTIN(TReal r_x, TReal r_y)
	{
		// Simply query the current TIN quad tree.
		CVector2<> v2_quad = CVector2<>(r_x, r_y) * pqntinRoot->mpConversions.tlr2WorldToQuad;

		return pqntinRoot->rGetWorldZ(v2_quad.tX, v2_quad.tY);
	}


	//******************************************************************************************
	CColour CTerrain::clrGetBaseTextureColour(TReal r_world_x, TReal r_world_y) const
	{
		return NMultiResolution::CTextureNode::clrGetBaseTextureColour(pqntinRoot.ptPtrRaw(), r_world_x, r_world_y);
	}


	extern CProfileStat psMoveMsgTerrain;
	
	//*****************************************************************************************
	void CTerrain::Process(const CMessageMove& msgmv)
	{
		CTimeBlock tmb(&psMoveMsgTerrain);

		// Update terrain when any light or CTerrainObj moves.
		if (ptCastRenderType<CLight>(msgmv.pinsMover->prdtGetRenderInfo()) != 0 ||
			ptCast<CTerrainObj>(msgmv.pinsMover) != 0)
		{
			// Destroy textures for moving light or terrain texture object.
			pqntinRoot->UpdateTextures(msgmv.pinsMover, msgmv.p3Prev);
		}
		else if (msgmv.pinsMover->pdGetData().bCastShadow)
		{
			// Update textures for moving shadows.
			pqntinRoot->UpdateMovingObjShadows(msgmv);
		}
	}


	//******************************************************************************************
	void CTerrain::FrameBegin(const CCamera* pcam_view)
	{
		// Update the quad trees every few steps.
		// Base it on step count, so replays are consistent.
		if (CMessageStep::u4Frame % 16 == 0)
			// Time for update.
			pqnqRoot->Update();

		pqntinRoot->Update(pcam_view, false);
	}


	//******************************************************************************************
	void CTerrain::FrameEnd()
	{
		NMultiResolution::CTextureNode::FrameEnd();
	}


	//******************************************************************************************
	void CTerrain::Rebuild(bool b_eval_geometry)
	{
		if (b_eval_geometry)
			pqntinRoot->Update(CWDbQueryActiveCamera().tGet(), true);

		NMultiResolution::CTextureNode::PurgeAllTextures();
	}


	//*****************************************************************************************
	void CTerrain::Process(const CMessagePaint& msgpaint)
	{
		if (CTerrain::bShowWireframe || CTerrain::bShowQuadtree)
		{
			// Attempt to cast to a raster win.
			CRasterWin* pras_win = dynamic_cast<CRasterWin*>(msgpaint.renContext.pScreenRender->prasScreen);

			if (pras_win != 0)
			{
				// Get camera origin, in quad space.
				const CCamera& cam = *CWDbQueryActiveCamera().tGet();

				CVector2<> v2_quad_pos = CVector2<>(cam.v3Pos()) * pqnqRoot->mpConversions.tlr2WorldToQuad;


				CRectangle<> rc_map = pqnqRoot->rcDrawWireframe
				(
					pras_win,
					CColour(0.8, 0.0, 0.0),
					CTerrain::bShowWireframe,
					CTerrain::bShowQuadtree,
					v2_quad_pos,
					CTerrain::rvarWireZoom
				);

				CDraw draw(rptr_this(pras_win), rc_map, true);

				// Draw camera cone on top.
				draw.Colour(CColour(1.0, 0.0, 1.0));

				// Get camera origin, and bottom far points, in world space.
				CVector2<> v2_origin = CVector2<>(cam.v3Pos()) * pqnqRoot->mpConversions.tlr2WorldToQuad;
				CVector2<> v2_left   = CVector2<>(CVector3<>(-1, 1, -1) * ~cam.tf3ToNormalisedCamera()) * pqnqRoot->mpConversions.tlr2WorldToQuad;
				CVector2<> v2_right  = CVector2<>(CVector3<>(+1, 1, -1) * ~cam.tf3ToNormalisedCamera()) * pqnqRoot->mpConversions.tlr2WorldToQuad;

				draw.Line(v2_left, v2_origin);
				draw.LineTo(v2_right);
			}
		}

		// Update terrain console.
		conTerrain.ClearScreen();

//*******************
/*
		const CCamera& cam = *CWDbQueryActiveCamera().tGet();

		CPresence3<> pr3_shape = pr3Presence();
		CTransform3<> tf3_norm_cam = cam.tf3ToNormalisedCamera();

//		CTransform3<> tf3_shape_to_norm_cam = ~pr3_shape * tf3_norm_cam;
		CTransform3<> tf3_shape_to_norm_cam = tf3_norm_cam;

		CVector3<> v3_cam_world_pos = cam.v3Pos();



		TReal r_z_min  = -18;
		TReal r_z_max  = 18;
		TReal r_size   = 64;
		TReal r_height = r_z_max - r_z_min;

		CVector3<> av3_corners[8];
		
		av3_corners[0] = CVector3<>(-32, 40, r_z_min);

		CVector3<> v3_mid(av3_corners[0].tX + r_size / 2, av3_corners[0].tY + r_size / 2, av3_corners[0].tZ + r_height / 2);
		CVector3<> v3_mid_xy(av3_corners[0].tX + r_size / 2, av3_corners[0].tY + r_size / 2, av3_corners[0].tZ);

		av3_corners[1] = av3_corners[0] + CVector3<>(r_size,      0, 0);
		av3_corners[2] = av3_corners[0] + CVector3<>(r_size, r_size, 0);
		av3_corners[3] = av3_corners[0] + CVector3<>(     0, r_size, 0);
		av3_corners[4] = av3_corners[0] + CVector3<>(     0,      0, r_height);
		av3_corners[5] = av3_corners[0] + CVector3<>(r_size,      0, r_height);
		av3_corners[6] = av3_corners[0] + CVector3<>(r_size, r_size, r_height);
		av3_corners[7] = av3_corners[0] + CVector3<>(     0, r_size, r_height);


		float f_world_to_pixel_ratio = cam.rGetProjectPlaneDist() *
		                               cam.tlr2GetProjectPlaneToScreen().tlrX.tScale;


		CVector3<> v3_min_dist_sqr;
		CVector3<> v3_max_dist_sqr;
		CVector3<> v3_observer_pos = cam.v3Pos();
		TReal r_bb_min_z = r_z_min;
		TReal r_bb_max_z = r_z_max;

		CVector3<> v3_rel_mid_sqr = v3_mid - cam.v3Pos();
		v3_rel_mid_sqr.tX *= v3_rel_mid_sqr.tX;
		v3_rel_mid_sqr.tY *= v3_rel_mid_sqr.tY;
		v3_rel_mid_sqr.tZ *= v3_rel_mid_sqr.tZ;


		// Determine minimum and maximum distance in X.
		float f_dist_x_min_sqr = av3_corners[0].tX - v3_observer_pos.tX;
		float f_dist_x_max_sqr = av3_corners[2].tX - v3_observer_pos.tX;

		float f_dist_x_prod = f_dist_x_min_sqr * f_dist_x_max_sqr;

		f_dist_x_min_sqr *= f_dist_x_min_sqr;
		f_dist_x_max_sqr *= f_dist_x_max_sqr;

		if (CIntFloat(f_dist_x_min_sqr).i4Int > CIntFloat(f_dist_x_max_sqr).i4Int)
			Swap(f_dist_x_min_sqr, f_dist_x_max_sqr);

		if (CIntFloat(f_dist_x_prod).i4Int < 0)
			f_dist_x_min_sqr = 0;

		v3_min_dist_sqr.tX = f_dist_x_min_sqr;
		v3_max_dist_sqr.tX = f_dist_x_max_sqr;


		// Determine minimum and maximum distance in Y.
		float f_dist_y_min_sqr = av3_corners[0].tY - v3_observer_pos.tY;
		float f_dist_y_max_sqr = av3_corners[2].tY - v3_observer_pos.tY;

		float f_dist_y_prod = f_dist_y_min_sqr * f_dist_y_max_sqr;

		f_dist_y_min_sqr *= f_dist_y_min_sqr;
		f_dist_y_max_sqr *= f_dist_y_max_sqr;

		if (CIntFloat(f_dist_y_min_sqr).i4Int > CIntFloat(f_dist_y_max_sqr).i4Int)
			Swap(f_dist_y_min_sqr, f_dist_y_max_sqr);

		if (CIntFloat(f_dist_y_prod).i4Int < 0)
			f_dist_y_min_sqr = 0;

		v3_min_dist_sqr.tY = f_dist_y_min_sqr;
		v3_max_dist_sqr.tY = f_dist_y_max_sqr;


		// Determine minimum and maximum distance in Z.
		float f_dist_z_min_sqr = r_bb_min_z - v3_observer_pos.tZ;
		float f_dist_z_max_sqr = r_bb_max_z - v3_observer_pos.tZ;

		float f_dist_z_prod = f_dist_z_min_sqr * f_dist_z_max_sqr;

		f_dist_z_min_sqr *= f_dist_z_min_sqr;
		f_dist_z_max_sqr *= f_dist_z_max_sqr;

		if (CIntFloat(f_dist_z_min_sqr).i4Int > CIntFloat(f_dist_z_max_sqr).i4Int)
			Swap(f_dist_z_min_sqr, f_dist_z_max_sqr);

		if (CIntFloat(f_dist_z_prod).i4Int < 0)
			f_dist_z_min_sqr = 0;

		v3_min_dist_sqr.tZ = f_dist_z_min_sqr;
		v3_max_dist_sqr.tZ = f_dist_z_max_sqr;


		float f_min_h_proj_dist_sqr = float(CIntFloat(MinMax
		(
			CIntFloat(v3_rel_mid_sqr.tZ).i4Int,
			CIntFloat(v3_min_dist_sqr.tX + v3_min_dist_sqr.tY).i4Int,
			CIntFloat(v3_max_dist_sqr.tX + v3_max_dist_sqr.tY).i4Int))
		);

		TReal r_h_proj_sqr = Sqr((r_z_max - r_z_min) * f_world_to_pixel_ratio);
		      r_h_proj_sqr *= f_min_h_proj_dist_sqr / Sqr(f_min_h_proj_dist_sqr + v3_rel_mid_sqr.tZ);


		float f_min_u_proj_dist_sqr = float(CIntFloat(MinMax
		(
			CIntFloat(v3_rel_mid_sqr.tX).i4Int,
			CIntFloat(v3_min_dist_sqr.tY + v3_min_dist_sqr.tZ).i4Int,
			CIntFloat(v3_max_dist_sqr.tY + v3_max_dist_sqr.tZ).i4Int))
		);

		TReal r_u_proj_sqr = Sqr(r_size * f_world_to_pixel_ratio);
		      r_u_proj_sqr *= f_min_u_proj_dist_sqr / Sqr(f_min_u_proj_dist_sqr + v3_rel_mid_sqr.tX);


		float f_min_v_proj_dist_sqr = float(CIntFloat(MinMax
		(
			CIntFloat(v3_rel_mid_sqr.tY).i4Int,
			CIntFloat(v3_min_dist_sqr.tX + v3_min_dist_sqr.tZ).i4Int,
			CIntFloat(v3_max_dist_sqr.tX + v3_max_dist_sqr.tZ).i4Int))
		);

		TReal r_v_proj_sqr = Sqr(r_size * f_world_to_pixel_ratio);
		      r_v_proj_sqr *= f_min_v_proj_dist_sqr / Sqr(f_min_v_proj_dist_sqr + v3_rel_mid_sqr.tY);


		TReal r_ratio_angle = double(angArctan2(v3_rel_mid_sqr.tY, v3_rel_mid_sqr.tX)) / dPI_2;
//		r_tex_u_size += r_height * (1 - r_ratio_angle);
//		r_tex_v_size += r_height * r_ratio_angle;


		TReal r_h_proj = sqrt(r_h_proj_sqr);
		TReal r_u_proj = sqrt(r_u_proj_sqr);
		TReal r_v_proj = sqrt(r_v_proj_sqr);

		CVector3<> v3_rel_pos = cam.v3Pos() - v3_mid;

		for (int i = 0; i < 8; i++)
		{
			av3_corners[i] *= tf3_shape_to_norm_cam;
			av3_corners[i] = cam.ProjectPoint(av3_corners[i]);
		}


		float f_hlen = 0;
		for (i = 0; i < 4; i++)
			f_hlen = Max(f_hlen, CVector2<>(av3_corners[i] - av3_corners[i + 4]).tLen());

		float f_ulen = 0;
		f_ulen = Max(f_ulen, CVector2<>(av3_corners[0] - av3_corners[1]).tLen());
		f_ulen = Max(f_ulen, CVector2<>(av3_corners[2] - av3_corners[3]).tLen());
		f_ulen = Max(f_ulen, CVector2<>(av3_corners[4] - av3_corners[5]).tLen());
		f_ulen = Max(f_ulen, CVector2<>(av3_corners[6] - av3_corners[7]).tLen());

		float f_vlen = 0;
		f_vlen = Max(f_vlen, CVector2<>(av3_corners[1] - av3_corners[2]).tLen());
		f_vlen = Max(f_vlen, CVector2<>(av3_corners[3] - av3_corners[0]).tLen());
		f_vlen = Max(f_vlen, CVector2<>(av3_corners[5] - av3_corners[6]).tLen());
		f_vlen = Max(f_vlen, CVector2<>(av3_corners[7] - av3_corners[4]).tLen());

		// Attempt to cast to a raster win.
		CRasterWin* pras_win = dynamic_cast<CRasterWin*>(msgpaint.renContext.pScreenRender->prasScreen);

		if (pras_win != 0)
		{
			CDraw draw(rptr_this(pras_win));

			for (i = 0; i < 4; i++)
			{
				if ((i & 1) != 0)
					draw.Colour(CColour(0.0, 1.0, 0.0));
				else
					draw.Colour(CColour(1.0, 0.0, 0.0));

				draw.Line(CVector2<>(av3_corners[i    ]), CVector2<>(av3_corners[ (i + 1) & 3     ]));
				draw.Line(CVector2<>(av3_corners[i + 4]), CVector2<>(av3_corners[((i + 1) & 3) + 4]));
			}
		}


		float f_pct_hdiff = (r_h_proj - f_hlen) / r_h_proj * 100;
		float f_pct_udiff = (r_u_proj - f_ulen) / r_u_proj * 100;
		float f_pct_vdiff = (r_v_proj - f_vlen) / r_v_proj * 100;
		conTerrain.Print("H: %f, %f, %f\n", f_hlen, r_h_proj, f_pct_hdiff);
		conTerrain.Print("U: %f, %f, %f\n", f_ulen, r_u_proj, f_pct_udiff);
		conTerrain.Print("V: %f, %f, %f\n", f_vlen, r_v_proj, f_pct_vdiff);

		conTerrain.Print("\nMin : %f, %f, %f\n", v3_min_dist_sqr.tX, v3_min_dist_sqr.tY, v3_min_dist_sqr.tZ);
		conTerrain.Print("Max : %f, %f, %f\n", v3_max_dist_sqr.tX, v3_max_dist_sqr.tY, v3_max_dist_sqr.tZ);
		conTerrain.Print("Proj: %f, %f, %f\n", r_h_proj_sqr, r_u_proj_sqr, r_v_proj_sqr);
		conTerrain.Print("Ratio: %f\n", r_ratio_angle);
*/
//*************
/*
		const CCamera& cam = *CWDbQueryActiveCamera().tGet();

		CVector2<> v2_quad = CVector2<>(cam.v3Pos()) * pqntinRoot->mpConversions.tlr2WorldToQuad;
		NMultiResolution::CQuadNodeTIN* pqntin_tri = pqntinRoot->ptqnFindLeaf(iTrunc(v2_quad.tX), iTrunc(v2_quad.tY));
		NMultiResolution::CQuadNodeQuery* pqnq_tri =   pqnqRoot->ptqnFindLeaf(iTrunc(v2_quad.tX), iTrunc(v2_quad.tY));

		int i_tin_size = pqntin_tri->ptqnGetTriangulate()->iGetSize();
		int i_q_size   =   pqnq_tri->ptqnGetTriangulate()->iGetSize();

		conTerrain.Print("%f\n%f\n", rHeight(cam.v3Pos().tX, cam.v3Pos().tY), rHeightTIN(cam.v3Pos().tX, cam.v3Pos().tY));
		conTerrain.Print("%f\n%f\n",   i_q_size * pqntinRoot->mpConversions.tlr2QuadToWorld.tlrX.tScale,
		                             i_tin_size * pqntinRoot->mpConversions.tlr2QuadToWorld.tlrX.tScale );
*/
//*************

		NMultiResolution::PrintProfileStats(conTerrain, pqntinRoot.ptPtrRaw(), pqnqRoot);
	}


	//*****************************************************************************************
	char *CTerrain::pcSave
	(
		char* pc
	) const
	{
		// Version number.
		pc = pcSaveT(pc, 1);

		// Settings.
		pc = pcSaveT(pc, NMultiResolution::CQuadRootTIN::rvarPixelTolerance.tGet());
		pc = pcSaveT(pc, NMultiResolution::CQuadRootTIN::rvarPixelToleranceFar.tGet());
		pc = pcSaveT(pc, NMultiResolution::CQuadRootTIN::rvarDeferMultiplier.tGet());
		pc = pcSaveT(pc, NMultiResolution::CQuadRootTIN::rvarMaxDistRatio.tGet());
		pc = pcSaveT(pc, NMultiResolution::CQuadRootTIN::rvarMinDistRatioNodeSize.tGet());
		pc = pcSaveT(pc, NMultiResolution::CQuadRootTIN::rvarSigNodeDiffShift.tGet());
		pc = pcSaveT(pc, NMultiResolution::CQuadRootTIN::rvarTexelScale.tGet());
		pc = pcSaveT(pc, NMultiResolution::CQuadRootTIN::rvarTexelScaleFar.tGet());
		pc = pcSaveT(pc, NMultiResolution::CQuadRootTIN::rvarDisableShadowDistance.tGet());
		pc = pcSaveT(pc, NMultiResolution::CQuadRootTIN::rvarDisableDynamicTextureDistance.tGet());
		pc = pcSaveT(pc, NMultiResolution::CQuadRootTIN::rvarDisableTextureDistance.tGet());

		pc = pcSaveT(pc, NMultiResolution::CTextureNode::bEnableShadows);
		pc = pcSaveT(pc, NMultiResolution::CTextureNode::bEnableMovingShadows);
		pc = pcSaveT(pc, NMultiResolution::CTextureNode::bDisableTextures);
		
		pc = pcSaveT(pc, CTerrain::bShowWireframe);
		pc = pcSaveT(pc, CTerrain::bShowQuadtree);
		pc = pcSaveT(pc, CTerrain::rvarWireZoom.tGet());

		return pc;
	}

	//*****************************************************************************************
	const char *CTerrain::pcLoad
	(
		const char* pc
	)
	{
		int iVersion;
		float f_temp;
		int i_temp;

		pc = pcLoadT(pc, &iVersion);

		if (iVersion == 1)
		{
			// Settings.
			pc = pcLoadT(pc, &f_temp);
			NMultiResolution::CQuadRootTIN::rvarPixelTolerance.Set(f_temp);

			pc = pcLoadT(pc, &f_temp);
			NMultiResolution::CQuadRootTIN::rvarPixelToleranceFar.Set(f_temp);

			pc = pcLoadT(pc, &f_temp);
			NMultiResolution::CQuadRootTIN::rvarDeferMultiplier.Set(f_temp);

			pc = pcLoadT(pc, &f_temp);
			NMultiResolution::CQuadRootTIN::rvarMaxDistRatio.Set(f_temp);

			pc = pcLoadT(pc, &i_temp);
			NMultiResolution::CQuadRootTIN::rvarMinDistRatioNodeSize.Set(i_temp);

			pc = pcLoadT(pc, &i_temp);
			NMultiResolution::CQuadRootTIN::rvarSigNodeDiffShift.Set(i_temp);

			pc = pcLoadT(pc, &f_temp);
			NMultiResolution::CQuadRootTIN::rvarTexelScale.Set(f_temp);

			pc = pcLoadT(pc, &f_temp);
			NMultiResolution::CQuadRootTIN::rvarTexelScaleFar.Set(f_temp);

			pc = pcLoadT(pc, &f_temp);
			NMultiResolution::CQuadRootTIN::rvarDisableShadowDistance.Set(f_temp);

			pc = pcLoadT(pc, &f_temp);
			NMultiResolution::CQuadRootTIN::rvarDisableDynamicTextureDistance.Set(f_temp);

			pc = pcLoadT(pc, &f_temp);
			NMultiResolution::CQuadRootTIN::rvarDisableTextureDistance.Set(f_temp);

			pc = pcLoadT(pc, &NMultiResolution::CTextureNode::bEnableShadows);
			pc = pcLoadT(pc, &NMultiResolution::CTextureNode::bEnableMovingShadows);

			pc = pcLoadT(pc, &NMultiResolution::CTextureNode::bDisableTextures);
			
			pc = pcLoadT(pc, &CTerrain::bShowWireframe);
			pc = pcLoadT(pc, &CTerrain::bShowQuadtree);

			pc = pcLoadT(pc, &f_temp);
			CTerrain::rvarWireZoom.Set(f_temp);
		}
		else
		{
			AlwaysAssert("Unknown version of terrain settings");
		}

		return pc;
	}

	//*****************************************************************************************
	void CTerrain::SaveDefaults()
	{
		char ac_buffer[4096];
		char *pc_end;

		pc_end = pcSave(ac_buffer);

		if (pc_defaults)
			delete pc_defaults; 

		int i_len = pc_end - ac_buffer;
		pc_defaults = new char[i_len];

		if (pc_defaults)
			memcpy(pc_defaults, ac_buffer, i_len);
	}


	//*****************************************************************************************
	void CTerrain::RestoreDefaults()
	{
		if (pc_defaults)
			pcLoad(pc_defaults);
	}
