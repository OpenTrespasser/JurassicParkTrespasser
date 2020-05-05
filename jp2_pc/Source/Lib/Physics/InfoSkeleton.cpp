/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of InfoBio.hpp
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 * 
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Physics/InfoSkeleton.cpp                                         $
 * 
 * 144   98/10/07 6:53 Speter
 * Restore quad foot positions from animal state.
 * 
 * 143   98/10/04 2:33 Speter
 * Set current health on activation. Removed unused functions.
 * 
 * 142   10/03/98 4:56a Sblack
 * Scaled gun impulses to animal mass.
 * 
 * 141   9/25/98 2:39a Sblack
 * 
 * 140   98/09/25 1:38 Speter
 * Only check living animals for undergroundness. Check just their body box, not their entire
 * volume.
 * 
 * 139   98/09/24 1:49 Speter
 * Made an itty bitty change to the Xob system.
 * 
 * 138   98/09/12 16:37 Speter
 * Fixed underground move code.
 * 
 * 137   98/09/12 1:05 Speter
 * Added Init() function to create boundary boxes at animal creation time, rather than during
 * CreatePhysics.
 * 
 * 136   98/09/10 0:39 Speter
 * Removed specified box names from biped and quad, letting AddBoundaryBox generate unique
 * names.
 * 
 * 135   9/08/98 2:36p Sblack
 * Added foot boxes.
 * 
 * 134   98/09/05 23:21 Speter
 * Update for new biomodel stuff.
 * 
 * 133   9/05/98 11:11p Agrant
 * handle position and rotation differently for double joints
 * 
 * 132   9/05/98 4:40p Agrant
 * Go limp support
 * 
 * 131   98/09/04 21:58 Speter
 * Added sender to Move().
 * 
 * 130   9/04/98 4:23p Sblack
 * 
 * 129   98/09/03 18:53 Speter
 * Catch PhysicsMessage in inactive dino.
 * 
 * 128   98/09/02 21:21 Speter
 * Removed i_activity param from Activate().
 * 
 * 127   8/31/98 5:04p Agrant
 * Don't pass tail commands
 * 
 * 126   98/08/29 18:11 Speter
 * In underground adjustment code, now use shape GetExtents() to find closer min Z, rather than
 * centred physics volume. Changed BioMesh single rMaxRelativeDist to one per joint, for more
 * efficient volumes.
 * 
 * 125   98/08/27 0:53 Speter
 * Added bIsMoving(), made some args const.
 * 
 * 124   98/08/26 21:21 Speter
 * Animal hip height params.
 * 
 * 123   8/26/98 11:17a Agrant
 * skeleton velocity query
 * 
 * 122   8/23/98 12:49p Sblack
 * 
 *********************************************************************************************/


//
// Includes.
//

#include "Common.hpp"
#include "InfoSkeleton.hpp"

#include "PhysicsSystem.hpp"
#include "PhysicsHelp.hpp"
#include "PhysicsStats.hpp"
#include "human.h"
#include "dino_biped.h"
#include "pelvis_def.h"
#include "pelvis.h"
#include "futil.h"
#include "Arms.h"
#include "InfoBox.hpp"
#include "Xob_bc.hpp"

#include "Lib/GeomDBase/Skeleton.hpp"
#include "Lib/GeomDBase/RayCast.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPhysicsReq.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Lib/Std/LocalArray.hpp"

// Debugging rendering stuff.
#include "Lib/Renderer/Camera.hpp"
#include "Lib/View/LineDraw.hpp"

#include "Lib/Sys/ConIO.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include "Lib/EntityDBase/Animal.hpp"
#include "Game/AI/Brain.hpp"

#include "Lib/Groff/VTParse.hpp"

#define bVERIFY_MATRICES	VER_DEBUG

static CColour clrBONES(1.0, 0.0, 0.0);
static CColour clrINACTIVE(0.5, 0.0, 0.5);
static CColour clrATTACH(0.3, 0.8, 0.0);


TSoundMaterial mat_trike_head = matHashIdentifier("TRIKE-HEAD");
TSoundMaterial mat_trike_body = matHashIdentifier("TRIKE-BODY");
TSoundMaterial mat_trike_tail = matHashIdentifier("TRIKE-TAIL");

TSoundMaterial mat_raptor_head = matHashIdentifier("RAPTOR-HEAD");
TSoundMaterial mat_raptor_body = matHashIdentifier("RAPTOR-BODY");
TSoundMaterial mat_raptor_tail = matHashIdentifier("RAPTOR-TAIL");


//
// Array mapping boundary box ids to physics elements.
//

static int aiPhysicsElements[] =
{
	BODY, RIGHT_FOOT, RIGHT_HAND, HEAD_BC, TAIL_BC, LEFT_FOOT, FRIGHT_FOOT, FLEFT_FOOT
};
		
//**********************************************************************************************
//
//  CPhysicsInfoSkeleton implementation
//

	CSArray<CAnimate*, NUM_PELVISES>	apaniActiveSkeletons;
	CSArray<bool, NUM_PELVISES>			abUpdatingSkeleton;


	//*****************************************************************************************
	CPhysicsInfoSkeleton::CPhysicsInfoSkeleton(const CPhysicsInfoBox& phib)
		: phibBounding(phib)
	{
		setFlags[epfTANGIBLE] = true;
		setFlags[epfMOVEABLE] = true;
		tmatSoundMaterial = phib.smatGetMaterialType();
		rSpeedMultiplier = 1.0f;
		rMouthMultiplier = 1.0f;
	}

	//*****************************************************************************************
	CPhysicsInfoSkeleton::~CPhysicsInfoSkeleton()
	{
	}

	//*****************************************************************************************
	void CPhysicsInfoSkeleton::ParseProps
	(
		const CHandle&				h_obj,				// Handle to the object in the value table.
		const CGroffObjectName*		pgon,
		CValueTable*				pvtable,			// Pointer to the value table.
		CLoadWorld*					pload				// the loader.
	)
	{
		SETUP_TEXT_PROCESSING(pvtable, pload);
		SETUP_OBJECT_HANDLE(h_obj)
		{
			bFILL_FLOAT(rSpeedMultiplier,	esSpeed);
			bFILL_FLOAT(rMouthMultiplier,	esMouth);
		}
		END_OBJECT;
		END_TEXT_PROCESSING;
	}

	//*****************************************************************************************
	void CPhysicsInfoSkeleton::Activate
	(
		CInstance *pins, bool b_just_update, const CPlacement3<>& p3_vel
	) const
	{
		CTimeBlock tmb(&psActivate);

		Assert(pphSystem);

		// Only wake up tangible objects!
		if (!bIsTangible())
			return;

		CAnimate* pani = ptCast<CAnimate>(pins);
		AlwaysAssert(pani);

		int i_index = pphSystem->iGetSkeletonIndex(pins);
		if (i_index >= 0)
		{
			if (b_just_update)
			{
				if (abUpdatingSkeleton[i_index])
					// This comes from physics move command, so needn't update physics.
					return;

				// Moving the object outside of physics.  
				// Accomplish by destroying/recreating skeleton in same spot.
				Remove_Pelvis(i_index);
				apaniActiveSkeletons[i_index] = 0;
			}
			else
				return;
		}
		else
		{
			if (b_just_update)
				return;

			// Add object to integrator.
			for (i_index = 0; i_index < apaniActiveSkeletons.size(); i_index++)
			{
				if (!apaniActiveSkeletons[i_index])
					break;
			}

			if (i_index == apaniActiveSkeletons.size())
			{
				Assert(false);
				return;
			}

			// Sound a wake-up call for newly activated object. It's always movable.
			CMessageMove msgmv(CMessageMove::etAWOKE, pins, pins->p3GetPlacement());
			msgmv.Dispatch();
		}
		
		conPhysics << "\nWaking up skeleton.";

		// Init the biomesh, getting it ready for animation.
		rptr<CRenderType> prdt	= pins->prdtGetRenderInfoWritable();
		rptr<CBioMesh>	pbm		= rptr_dynamic_cast(CBioMesh, prdt);
		Assert(pbm);

		// Perform a sanity check on the mass.
		if (phibBounding.fDensity >= fDEFAULT_DENSITY)
		{
			// Mass wasn't set, and is far too large.
			// Just set mass to 20, which is a usable default for both bipeds and quads.
			const_cast<CPhysicsInfoBox&>(phibBounding).fDensity *= 20 / phibBounding.fMass(pins);
		}

		if (!pani->bReallyDead())
		{
			//
			// Check for underground animals.
			//

			// Find body box minimum Z, in world space.
			if (pani->apbbBoundaryBoxes[(int)ebbBODY])
			{
				CVector3<> v3_min, v3_max;
				AlwaysVerify(pani->apbbBoundaryBoxes[(int)ebbBODY]->bGetWorldExtents(v3_min, v3_max));

				// Get terrain height at box centre.
				CPlacement3<> p3 = pani->apbbBoundaryBoxes[(int)ebbBODY]->p3GetPlacement();
				CWDbQueryTerrain wqtrr;
				TReal r_trr = wqtrr.tGet() ? wqtrr.tGet()->rHeight(p3.v3Pos.tX, p3.v3Pos.tY) : 0;

				const TReal r_slack = 0.2f;
				TReal r_over = v3_min.tZ - r_trr + r_slack;
				if (r_over < 0.0f)
				{
					//
					// Move the animal to be above the terrain.
					// Since animal hasn't yet been placed in the apaniActiveSkeletons array,
					// it will be considered inactive, and this function won't attempt to update it
					// recursively.
					//
					dout <<"WARNING! Skeleton " <<pins->strGetInstanceName() <<" is "
						 <<-r_over <<" m underground!\n";
					p3.v3Pos.tZ -= r_over;
					pins->Move(p3);
				}
			}
		}

		// Put the instance in the list.
		apaniActiveSkeletons[i_index] = pani;

		// Convert state to arrays.
		float aaf_state[7][3];

		Copy(aaf_state, pins->p3GetPlacement(), 0);
		Copy(aaf_state, p3_vel, 1);

		CreatePhysics(pins, i_index, aaf_state);

		// Set current health before next physics step.
		pani->UpdatePhysicsHealth();
	}

	//*****************************************************************************************
	void CPhysicsInfoSkeleton::Deactivate
	(
		CInstance *pins
	) const
	{
		int i_index = pphSystem->iGetSkeletonIndex(pins);
		if (i_index < 0)
			return;

		// Update before removing.
		UpdateWDBase(pins, i_index);

		// Also, update joints a final time before leaving physics.
		UpdateSkeleton(pins);

		// Do we have specified boundary boxes?
		CAnimate* pani = ptCast<CAnimate>(pins);
		if (pani)
		{
			// Deactivate them.
			for (int i = 0; i < ebbEND; i++)
			{
				if (pani->apbbBoundaryBoxes[i])
					pani->apbbBoundaryBoxes[i]->PhysicsDeactivate();
			}
		}

		apaniActiveSkeletons[i_index] = 0;
		Remove_Pelvis(i_index);
		conStd << "\nB: " << i_index;

		// Send a sleep message. It's always movable.
		CMessageMove msgmv(CMessageMove::etSLEPT, pins, pins->p3GetPlacement());
		msgmv.Dispatch();
	}

	//*****************************************************************************************
	CAnimate* CPhysicsInfoSkeleton::paniSkeleton(int i_index)
	{
		if (i_index < 0)
			return 0;
		return apaniActiveSkeletons[i_index];
	}
															// associated with the active objects.
	//*****************************************************************************************
	void CPhysicsInfoSkeleton::Reset()
	{
		// Clear our internal arrays.
		apaniActiveSkeletons.Fill(0);

		// Clear out physics system stuff.
		PelReset();
	}

	//*****************************************************************************************
	bool CPhysicsInfoSkeleton::bIsActive(const CInstance* pins) const
	{
		return pphSystem->iGetSkeletonIndex(pins) >= 0;
	}

	//*****************************************************************************************
	bool CPhysicsInfoSkeleton::bIsMoving(const CInstance* pins) const
	{
		return pphSystem->iGetSkeletonIndex(pins) >= 0;
	}

	//*****************************************************************************************
	CPlacement3<> CPhysicsInfoSkeleton::p3GetVelocity(const CInstance* pins) const
	{
		int i_index = pphSystem->iGetSkeletonIndex(pins);
		if (i_index >= 0)
		{
			// Get the velocity from State; it's the same for all sub-elements.
			return p3Convert(Pel[i_index], 1);
		}
		else
			return p3Null;
	}

	//*****************************************************************************************
	void CPhysicsInfoSkeleton::HandleMessage
	(
		const CMessagePhysicsReq& msgpr,
		CInstance *pins
	) const
	{
		// Make sure it's active.
		int i_index = pphSystem->iGetSkeletonIndex(pins);
		AlwaysAssert(i_index >= 0);

		// Send pelvis control always, so model knows when to stop moving.  Initialise params.
		CDir3<> d3_move, d3_turn;
		float f_move_sp = 0, f_turn_sp = 0;

		if (msgpr.subMoveBody.rtUrgency != 0 && msgpr.subMoveBody.rtSpeed != 0)
		{
			d3_move = msgpr.subMoveBody.dData - pins->v3Pos();
			if (!d3_move.bIsZero())
				d3_move.Normalise();
			f_move_sp = 12.0 * 5 * msgpr.subMoveBody.rtSpeed * rSpeedMultiplier;
			//f_move_sp = msgpr.subMoveBody.rtSpeed;
		}

		if (msgpr.subOrientBody.rtUrgency != 0)
		{
			d3_turn = d3YAxis * msgpr.subOrientBody.dData;
			f_turn_sp = msgpr.subOrientBody.rtSpeed;
		}

		Pelvis_Control(i_index, afConvert(d3_move), f_move_sp, afConvert(d3_turn), f_turn_sp);

		
extern bool	 bKontrol_Krouch[NUM_PELVISES];
extern int	 iKontrol_Jump[NUM_PELVISES];
		// Now, crouch and Jump!!	Remove Globals later when this works!!!!!	
		if (msgpr.subCrouch.rtUrgency)
			// Crouch or end-crouch.
			bKontrol_Krouch[i_index] = msgpr.subCrouch.dData;
		else
			bKontrol_Krouch[i_index] = false;

		if (msgpr.subJump.rtUrgency)
		{
			Pelvis_Jump[0] = Pelvis_Jump[1] = 0;
			Pelvis_Jump[2] = 1;
			Pelvis_Jump_Voluntary = true;
		}
		else if (!Pelvis_Jump_Voluntary) /* If-condition part of an incomplete workaround for the jump bug */
		{
			// Always stop jumping if no urgency.
			Pelvis_Jump[0] = Pelvis_Jump[1] = Pelvis_Jump[2] = 0;
		}
	}


	//*****************************************************************************************
	void CPhysicsInfoSkeleton::UpdateWDBase
	(
		CInstance*	pins,		// The instance to update
		int			i_index		// The index of that instance in the appropriate physics
								// system array.
	) const
	{
		// Prevent recursion, e.g. from Deactivate.
		if (abUpdatingSkeleton[i_index])
			return;
		abUpdatingSkeleton[i_index] = true;

		// Move the object in the world partitioning structure.
		pins->Move(p3Convert(Pel[i_index], 0), pphSystem);

		CAnimate* pani = ptCast<CAnimate>(pins);
		Assert(pani);

		// Mark mesh as requiring update.
		pani->bSkeletonUpdated = false;
		UpdateSkeleton(pins);

		// Remove if dead (unless player).
		if (!ptCast<CPlayer>(pani) && pani->bReallyDead())
		{
			bool b_moving = false;

			// Animal is dead. See if all its boxes are still moving.
			for (int i = 0; i < iMAX_PHYSICS_OBJECTS; i++)
			{
				if (Xob[i].PelvisModel == i_index)
				{
					// This is our box.
					if (Xob[i].Info > 2)
					{
						b_moving = true;
						break;
					}
				}
			}

			if (!b_moving)
				Deactivate(pins);
		}

		abUpdatingSkeleton[i_index] = false;
	}

	//*****************************************************************************************
	void CPhysicsInfoSkeleton::UpdateSkeleton
	(
		CInstance*	pins									// The instance to update
	) const
	{
		CAnimate* pani = ptCast<CAnimate>(pins);
		rptr<CBioMesh> rpbm = rptr_nonconst(pins->prdtGetRenderInfoWritable()->rpbmCast());
		if (!pani || !rpbm || pani->bSkeletonUpdated)
			return;

		pani->bSkeletonUpdated = true;

		CSkeletonRenderInfo& sri = pani->sriBones;

		TReal	loc[7];										// Dino placement.
		TReal	points[iMAX_SKELETAL_ELEMENTS][3];			// Joint positions (world space)
		TReal	matrices[iMAX_SKELETAL_ELEMENTS][3][3];		// Joint orientations (world space)
		int		Am_I_Supported[iMAX_SKELETAL_ELEMENTS];		// Non-zero if that joint is updated by the physics system

		int i_index = pphSystem->iGetSkeletonIndex(pins);
		if (i_index < 0)
			// Not active!
			return;

		// Init the flags, just in case.
		memset(Am_I_Supported, 0, sizeof(Am_I_Supported));

		// Snarf the data!
		GetData(i_index, loc, points, matrices, Am_I_Supported);

		const CPlacement3<>& p3_local_world = p3Convert(loc);
		CTransform3<> tf3_world_local = ~p3_local_world;
		TReal	r_inverse_scale = 1 / pins->fGetScale();

		Assert(iMAX_SKELETAL_ELEMENTS >= rpbm->iJoints);

		// Update the joints. Also compute max extent from centre.
		CVector3<> v3_max(0, 0, 0);

		int j;
		for (j = rpbm->iJoints - 1; j >= 0; j--)
		{
			if (Am_I_Supported[j])
			{
				CMatrix3<> mx3 = mx3Convert(matrices[j]);

#if 0
				// Verify that we have an orientation matrix.
				const float fTOL = .01;
				Assert(Fuzzy(mx3.v3X * mx3.v3Y, fTOL) == 0);
				Assert(Fuzzy(mx3.v3X * mx3.v3Z, fTOL) == 0);				
				Assert(Fuzzy(mx3.v3Z * mx3.v3Y, fTOL) == 0);

				Assert(Fuzzy(mx3.v3X * mx3.v3X, fTOL) == 1);
				Assert(Fuzzy(mx3.v3Y * mx3.v3Y, fTOL) == 1);
				Assert(Fuzzy(mx3.v3Z * mx3.v3Z, fTOL) == 1);

				CVector3<> v3_x = mx3.v3Y ^ mx3.v3Z;
				Assert(Fuzzy(v3_x,.01) == mx3.v3X);
#endif

				CVector3<> v3_world_pos = v3Convert(points[j]);

				CTransform3<> tf3_world
				(
					mx3,
					v3_world_pos
				);

				sri.patf3JointTransforms[j] = tf3_world * tf3_world_local;

				sri.patf3JointTransforms[j].v3Pos *= r_inverse_scale;
			}

			// Set maximum joint extent from centre, adding the maximum relative vertex distance.
			SetMax(v3_max.tX, Abs(sri.patf3JointTransforms[j].v3Pos.tX) + rpbm->arMaxRelativeDist[j]);
			SetMax(v3_max.tY, Abs(sri.patf3JointTransforms[j].v3Pos.tY) + rpbm->arMaxRelativeDist[j]);
			SetMax(v3_max.tZ, Abs(sri.patf3JointTransforms[j].v3Pos.tZ) + rpbm->arMaxRelativeDist[j]);
		}

		sri.bvbVolume = CBoundVolBox(v3_max);

		// And now loop through the double joints.
		for (j = 0; j < rpbm->afPositionRatios.uLen; ++j)
		{
			// Want:  Weighted average of two transforms.
			CTransform3<> tf3_primary	= sri.patf3JointTransforms[rpbm->aiDoubleJointsA[j]];
			CTransform3<> tf3_secondary	= sri.patf3JointTransforms[rpbm->aiDoubleJointsB[j]];

			CTransform3<> tf3;

			// Handle position with the position ratio.
			float f_primary = rpbm->afPositionRatios[j];
			float f_secondary = 1 - f_primary;

			tf3.v3Pos  = tf3_primary.v3Pos * f_primary
				+ tf3_secondary.v3Pos * f_secondary;

			// Handle rotation with rotation ratio.
			f_primary = rpbm->afRotationRatios[j];
			f_secondary = 1 - f_primary;

			tf3_primary *= CScaleI3<>(f_primary);
			tf3_secondary *= CScaleI3<>(f_secondary);

			tf3.mx3Mat.v3X = tf3_primary.mx3Mat.v3X + tf3_secondary.mx3Mat.v3X;
			tf3.mx3Mat.v3Y = tf3_primary.mx3Mat.v3Y + tf3_secondary.mx3Mat.v3Y;
			tf3.mx3Mat.v3Z = tf3_primary.mx3Mat.v3Z + tf3_secondary.mx3Mat.v3Z;

			// And set the joint transform.
			sri.patf3JointTransforms[j + rpbm->iJoints] = tf3;
		}
	}

	//**********************************************************************************************
	void CPhysicsInfoSkeleton::RayIntersect(CInstance* pins, int i_subobj, CRayCast& rc,
							   const CPlacement3<>& p3, TReal r_length, TReal r_diameter) const
	{
		// THIS IS REDUNDANT, AS ALL THE BOUNDARY BOXES ARE INSTANCES IN THE WORLD DB.
/*
		// To do: wake up animals with gun.
		CAnimate* pani = ptCast<CAnimate>(pins);
		if (!pani)
			return;

		// Use boundary-box instances.
		for (int i = 0; i < ebbEND; i++)
		{
			if (pani->apbbBoundaryBoxes[i])
			{
				// Intersect with this box, passing BC as subobject.
				pani->apbbBoundaryBoxes[i]->pphiGetPhysicsInfo()->RayIntersect
				(
					pani->apbbBoundaryBoxes[i], i,
					rc, p3, r_length, r_diameter
				);
			}
		}
*/
	}

	//*****************************************************************************************
	void CPhysicsInfoSkeleton::ApplyImpulse(CInstance* pins, int i_subobj, const CVector3<>& v3_pos, const CVector3<>& v3_impulse) const
	{
		int i_pel = pphSystem->iGetSkeletonIndex(pins);
		if (i_pel >= 0)
		{
			// i_subobj refers to the EBoundaryBox type. Make sure it refers to an active box.
			int i_elem = iPhysicsElem(EBoundaryBoxes(i_subobj));
			Assert(bWithin(i_elem, 0, PELVIS_DOF));
			int i_index = Pel_Box_BC[i_pel][i_elem];
			Assert(i_index >= 0);
			Assert(Xob[i_index].Instances[0]);

			// Copy the vectors, as they're modified.
			float af_pos[3], af_impulse[3];

			//conPhysics << "Hello from pelvis " << i_pel << "with mass " << 1.0f/Pel_Data[i_pel][31] << "\n";
			Copy(af_pos, v3_pos);
			Copy(af_impulse, v3_impulse);

			af_impulse[0] *= 20.0f * Pel_Data[i_pel][31];		//Rescale to Dino Mass!
			af_impulse[1] *= 20.0f * Pel_Data[i_pel][31];
			af_impulse[2] *= 20.0f * Pel_Data[i_pel][31];

			// Call the raw physics box function.
			Xob[i_index].ApplyImpulse(af_pos, af_impulse);
		}
	}

	//*****************************************************************************************
	void CPhysicsInfoSkeleton::ActivateBoundaryBoxes(int, CAnimate* pani) const
	{
		AlwaysAssert(pani);

		// Do we have specified boundary boxes?
		for (int i = 0; i < ebbEND; i++)
		{
			if (pani->apbbBoundaryBoxes[i])
				pani->apbbBoundaryBoxes[i]->PhysicsActivate();
		}
	}

	//*****************************************************************************************
	void CPhysicsInfoSkeleton::Init(CAnimate* pani) const
	{
	}

	//*****************************************************************************************
	int CPhysicsInfoSkeleton::iPhysicsElem(int i_index)
	{
		// Pretty simple.
		Assert(i_index < (int)ebbEND);
		return aiPhysicsElements[i_index];
	}

	//*****************************************************************************************
	void CPhysicsInfoSkeleton::DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const
	{
	}

	//*****************************************************************************************
	static void DrawBones(CInstance* pins, CPArray<int> pai_attach, CDraw& draw, CCamera& cam)
	{
#if bVER_BONES()

		// Get this info's biomesh (if it exists).
		CAnimate* pani = ptCast<CAnimate>(pins);
		rptr_const<CBioMesh> pbm = ptCastRenderType<CBioMesh>(pins->prdtGetRenderInfo());
		if (!pani || !pbm)
			return;

		CSkeletonRenderInfo& sri = pani->sriBones;

		CTransform3<> tf3_shape_screen = pins->pr3GetPresence() * cam.tf3ToHomogeneousScreen();

		if (CPhysicsInfo::setedfMain[edfATTACHMENTS])
		{
			// Draw attached points.
			draw.Colour(clrATTACH);
			for (int i_p = 0; i_p < pbm->pav3Points.uLen; i_p++)
			{
				int i_j = pbm->pauJoints[i_p];
				draw.Line3D
				(
					// Attached joint.
					sri.patf3JointTransforms[i_j].v3Pos * tf3_shape_screen,

					// Point.
					pbm->pav3Points[i_p] * sri.patf3JointTransforms[i_j] * tf3_shape_screen
				);
			}
		}

		if (CPhysicsInfo::setedfMain[edfSKELETONS])
		{
			// Draw joints.
			for (int i = 0; i < sri.patf3JointTransforms.size(); i++)
			{
				if (i < pai_attach.size() && pai_attach[i] != i)
				{
					CVector3<> v3_a = sri.patf3JointTransforms[i].v3Pos;
					CVector3<> v3_b = sri.patf3JointTransforms[pai_attach[i]].v3Pos;

					// Transform and project these to camera space, then draw.
					// Use preset colour.
					draw.Colour(CColour(128,255,255));
					draw.Line3D
					(
						v3_a * tf3_shape_screen,
						v3_b * tf3_shape_screen
					);

					// Draw coordinate frames.
					draw.CoordinateFrame3D
					(
						sri.patf3JointTransforms[i] * tf3_shape_screen,
						1.0,		// Brightness.
						.04			// Size
					);
				}
			}
		}
#endif
	}

//**********************************************************************************************
//
//  CPhysicsInfoBiped implementation
//

	//*****************************************************************************************
	CVector3<> CPhysicsInfoBiped::v3GetHeadPosition
	(
		const CInstance* pins
	) const
	{
		const CAnimate* pani = ptCast<CAnimate>(const_cast<CInstance*>(pins));
		if (!pani)
			return pins->v3Pos();

		// Return the position of joint 9.
		return pani->sriBones.patf3JointTransforms[8].v3Pos * pins->pr3GetPresence();
	}

	//*****************************************************************************************
	CVector3<> CPhysicsInfoBiped::v3GetTailPosition
	(
		const CInstance* pins
	) const
	{
		const CAnimate* pani = ptCast<CAnimate>(const_cast<CInstance*>(pins));
		if (!pani)
			return pins->v3Pos();

		// Return the position of joint 9.
		return pani->sriBones.patf3JointTransforms[4].v3Pos * pins->pr3GetPresence();
	}

	//*****************************************************************************************
	inline TReal rLength
	(
		CPArray< CTransform3<> > patf3,
		int i_start, int i_end
	)
	{
		return (patf3[i_end].v3Pos - patf3[i_start].v3Pos).tLen();
	}

	//*****************************************************************************************
	inline TReal rLength
	(
		CPArray< CTransform3<> > patf3,
		int i_start
	)
	{
		return rLength(patf3, i_start, i_start+1);
	}

	//*****************************************************************************************
	void GetBipedParams(CInstance* pins, Biped_Params* pparams)
	{
		Biped_Params& params = *pparams;

		// Use biomesh's ORIGINAL joint geometry (not instance's current geometry).
		rptr<CBioMesh> pbm = rptr_nonconst(pins->prdtGetRenderInfoWritable()->rpbmCast());
		Assert(pbm);

		TReal r_scale = pins->fGetScale();

		CPArray< CTransform3<> > patf3 = pbm->patf3JointsOrig;

		params.hip_radius		= rLength(patf3, 12, 16) * 0.5 * r_scale;
		params.hip_height		= patf3[12].v3Pos.tZ * r_scale;
		params.leg_length		= (rLength(patf3, 12) + 
								  rLength(patf3, 13) + 
								  rLength(patf3, 14)) * r_scale;

		Copy(params.tail_offset, patf3[0].v3Pos * r_scale);
		params.tail_length		= (rLength(patf3, 0) + 
								  rLength(patf3, 1) + 
								  rLength(patf3, 2) +
								  rLength(patf3, 3)) * r_scale;

		Copy(params.neck_offset, patf3[5].v3Pos * r_scale);
		params.neck_length		= (rLength(patf3, 5) +
								  rLength(patf3, 6) +
								  rLength(patf3, 7)) * r_scale;

		// Neck orientation vector (normalised).
		CDir3<> d3_neck = patf3[8].v3Pos - patf3[5].v3Pos;
		Copy(params.neck_dir, d3_neck);
	}

	//*****************************************************************************************
	void CPhysicsInfoBiped::Init(CAnimate* pani) const
	{
		// Create non-loaded boundary boxes.
		pani->AddBoundaryBox(ebbBODY, CVector3<>(.1, .3, .1), v3Zero, 100, mat_raptor_body, 1.0, 1.0);
		pani->AddBoundaryBox(ebbHEAD, CVector3<>(.05, .1, .05), v3Zero, 100, mat_raptor_head, 1.0, 1.0);
		pani->AddBoundaryBox(ebbTAIL, CVector3<>(.05, .2, .05), v3Zero, 100, mat_raptor_tail, 1.0, 1.0);

		pani->AddBoundaryBox(ebbRIGHT_FOOT, CVector3<>(.1, .2, .1), v3Zero, 100, 0, 0.0, 0.0);
		pani->AddBoundaryBox(ebbLEFT_FOOT,  CVector3<>(.1, .2, .1), v3Zero, 100, 0, 0.0, 0.0);
	}

	//*****************************************************************************************
	void CPhysicsInfoBiped::CreatePhysics(CInstance* pins, int i_index, float aaf_state[7][3]) const
	{
		Assert(i_index >= 0);

		// Get params from biomesh.
		Biped_Params params;
		GetBipedParams(pins, &params);

		params.mass = fMass(pins);

		// Well, MAKE A RAPTOR!
		Make_a_Raptor(pins, i_index, params, aaf_state);

		CAnimate* pani = ptCast<CAnimate>(pins);
		ActivateBoundaryBoxes(i_index, pani);
	}

	//*****************************************************************************************
	void CPhysicsInfoBiped::GetData
	( 
		int model,
		float loc[7],
		float points[iMAX_SKELETAL_ELEMENTS][3], float matrices[iMAX_SKELETAL_ELEMENTS][3][3],
		int Am_I_Supported[iMAX_SKELETAL_ELEMENTS]
	) const
	{
		// Call the right guy.
		Get_Raptor_Data(model, loc, points, matrices, Am_I_Supported);
	}

	//*****************************************************************************************
	void CPhysicsInfoBiped::DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const
	{
#if bVER_BONES()
		// Well, for now let's use hard-coded joint connection info.
		// Later, this might be split into a connection data structure in this class, and a
		// general-purpose drawing function.

		static int ai_attach[] =
		{
			0,				// 0: Base of tail.
			0, 1, 2, 3,		// 1: Tail.
			5,				// 5: Base of torso.
			5, 6, 7, 8, 	// Torso, head.
			6,				// 10: Right arm.
			6,				// 11: Left arm.
			12,				// 12: Base of right leg.
			12, 13, 14,		// Right leg.
			16,				// 16: Base of left leg.
			16, 17, 18		// Left leg.
		};

		DrawBones(pins, CPArray<int>(ArrayData(ai_attach)), draw, cam);

		if (!setedfMain[edfSKELETONS])
			return;

		CAnimal* pani = ptCast<CAnimal>(pins);
		Assert(pani);
	
		// Draw target head location!
		if (pani->pbrBrain->msgprPhysicsControl.subMoveHead.rtUrgency.fVal > 0.0f)
		{
			// Draw a 3-cross at the target position.
			CPlacement3<> p3(pani->pbrBrain->msgprPhysicsControl.subMoveHead.dData);
			CTransform3<> tf3_shape_screen = p3 * cam.tf3ToHomogeneousScreen();
			draw.CoordinateFrame3D(tf3_shape_screen, 0.75f);
		}

		CTransform3<> tf3_shape_camera = pins->pr3GetPresence() * cam.tf3ToNormalisedCamera();

		// Draw target head orientation!
		if (pani->pbrBrain->msgprPhysicsControl.subPointHead.rtUrgency.fVal > 0.0f)
		{
			draw.Colour(CColour(255,0,0));
			// Draw a 2 meter line from pelvis in direction of orientation
			CVector3<> v3 = ((pani->pbrBrain->msgprPhysicsControl.subPointHead.dData) * 2.0f + pani->v3Pos());

			v3 = v3 * ~pani->pr3Presence();

			//CPlacement3<> p3(v3);

			//CTransform3<> tf3_shape_screen = p3 * cam.tf3ToHomogeneousScreen();

			draw.Line
			(
				// Attached joint.
				cam.ProjectPoint(v3 * tf3_shape_camera),
				//CVector3<>(0,0,0) * tf3_shape_screen,

				// Axis.
				cam.ProjectPoint(CVector3<>(0,0,0) * tf3_shape_camera)
			);
		}

		// Call parent.
		CPhysicsInfoSkeleton::DrawPhysics(pins, draw, cam);
#endif
	}

	//*****************************************************************************************
	void CPhysicsInfoBiped::HandleMessage
	(
		const CMessagePhysicsReq& msgpr,
		CInstance *pins
	) const
	{
		// Make sure it's active.
		int i_index = pphSystem->iGetSkeletonIndex(pins);
		if (i_index < 0)
			return;

		//
		// Default values.
		//

		// Head does not tilt
		float f_head_cock		= 0;				

		// No tail offset
		float f_wag_amount		= 0;				
		
		// No tail motion
		float f_wag_frequency	= 0;				

		// Mouth closed.
		float f_mouth_open		= 0.0f;

		// Body does not move to head position
		float f_head_move_speed	= 0.0f;

		// Am I pointing or moving...
		float f_headup			= 1.0f;	//pointing...

		// Nose facing same as desired body direction.
		CVector3<> v3_nose		= CVector3<>(0,1,0) * pins->r3Rot();  //  Y axis for now- ha!

		// Send pelvis control always, so model knows when to stop moving.  Initialise params.
		CDir3<> d3_move, d3_turn;
		float f_move_sp = 0, f_turn_sp = 0;

		if (msgpr.subMoveBody.rtUrgency != 0 && msgpr.subMoveBody.rtSpeed != 0)
		{
			d3_move = msgpr.subMoveBody.dData - pins->v3Pos();
//			d3_move.tZ = 0;
			if (!d3_move.bIsZero())
				d3_move.Normalise();
			//f_move_sp = 12.0 * 4 * msgpr.subMoveBody.rtSpeed;
			f_move_sp = msgpr.subMoveBody.rtSpeed * rSpeedMultiplier;
		}

		if (msgpr.subOrientBody.rtUrgency != 0)
		{
			d3_turn = d3YAxis * msgpr.subOrientBody.dData;
			f_turn_sp = msgpr.subOrientBody.rtSpeed;
		}

		if (msgpr.subOpenMouth.rtUrgency != 0.0f)
		{
			// Then open the mouth.	
			f_mouth_open = msgpr.subOpenMouth.dData * rMouthMultiplier;
		}

		if (msgpr.subPointHead.rtUrgency != 0.0f)
		{
			// Then point the head.	
			v3_nose = msgpr.subPointHead.dData;
		}

		if (msgpr.subMoveHead.rtUrgency != 0.0f)
		{
			// Then point the head.	
			v3_nose = msgpr.subMoveHead.dData - pins->v3Pos();
			//f_head_move_speed = 5 * msgpr.subMoveHead.rtSpeed;
			f_head_move_speed = msgpr.subMoveHead.rtSpeed;
			f_headup = 100.0f;
		}

		if (msgpr.subCockHead.rtUrgency != 0.0f)
		{
			// Then cock the head.	
			f_head_cock = msgpr.subCockHead.dData;
		}
/*
		if (msgpr.subTailWagAmplitude.rtUrgency != 0.0f)
		{
			// Then wag the tail.

			// Must have a frequency command as well....
			Assert(msgpr.subTailWagFrequency.rtUrgency != 0.0f);
			
			f_wag_amount = msgpr.subTailWagAmplitude.dData;
			f_wag_frequency = msgpr.subTailWagFrequency.dData;
		}
		else
		{
			// Must only give a tail frequency command when the amplitude command is given.
			Assert(msgpr.subTailWagFrequency.rtUrgency == 0.0f);
		}
*/


		
extern bool	 bKontrol_Krouch[NUM_PELVISES];
extern int	 iKontrol_Jump[NUM_PELVISES];
		// Now, crouch and Jump!!	Remove Globals later when this works!!!!!	
		if (msgpr.subCrouch.rtUrgency)
			// Crouch or end-crouch.
			bKontrol_Krouch[i_index] = msgpr.subCrouch.dData;
		else
			bKontrol_Krouch[i_index] = false;

		if (msgpr.subJump.rtUrgency)
			iKontrol_Jump[i_index] = 1;//msgpr.subJump.dData;
		else
		{
			// Always stop jumping if no urgency.
			//iKontrol_Jump[i_index] = 0;
		}

		extern bool bIsLimp[NUM_PELVISES];
		if (msgpr.subGoLimp.rtUrgency)
		{
			// Go Limp!
			Assert(msgpr.subGoLimp.dData == true);
			bIsLimp[i_index] = true;
		}
		else
		{
			bIsLimp[i_index] = false;
			// Stop going limp!
		}


		// Bring nose pointing vector into model space.
		v3_nose = v3_nose * ~pins->r3Rot();

		float nose[3];


		nose[0] = v3_nose.tX;
		nose[1] = v3_nose.tY;
		nose[2] = v3_nose.tZ;

		//	First random stab at skeletal AI control...
		// DREW -- THE 0 BELOW IS FOR HEADMOVETO //

		Pelvis_Control(i_index, afConvert(d3_move), f_move_sp, afConvert(d3_turn), f_turn_sp);
		RaptorBodyControl(i_index, nose, f_head_cock, f_head_move_speed, f_wag_amount, f_wag_frequency, f_mouth_open, f_headup );

	}

	
	//*****************************************************************************************
	float CPhysicsInfoBiped::fDamageMultiplier
	(
		const CInstance* pins ,
		TSoundMaterial tsmat
	) const
	{
		Assert(pins);

		if (tsmat == mat_raptor_head)
		{
			return 4 * fDamage;
		}
		else if (tsmat == mat_raptor_body)
		{
			return 0.1 * fDamage;
		} 
		else if (tsmat == mat_raptor_tail)
		{
			return 0.25 * fDamage;
		}
		else
		{
			// Unknown quadruped sound material.
			Assert(false);
			return 0;
		}	
	}

	//*****************************************************************************************
	float CPhysicsInfoBiped::fArmourMultiplier
	(
		const CInstance* pins,
		TSoundMaterial tsmat 
	) const
	{
		Assert(pins);

		if (tsmat == mat_raptor_head)
		{
			return 0.1 * fArmour;
		}
		else if (tsmat == mat_raptor_body)
		{
			return 1 * fArmour;
		} 
		else if (tsmat == mat_raptor_tail)
		{
			return 1 * fArmour;
		}
		else
		{
			// Unknown quadruped sound material.
//			Assert(false);
			return 2;
		}	
	}


//**********************************************************************************************
//
//  CPhysicsInfoQuadruped implementation
//

	//*****************************************************************************************
	void GetQuadParams(CInstance* pins, Quad_Params* pparams)
	{
		GetBipedParams(pins, pparams);

		Quad_Params& params = *pparams;

		// Use biomesh's ORIGINAL joint geometry (not instance's current geometry).
		rptr<CBioMesh> pbm = rptr_nonconst(pins->prdtGetRenderInfoWritable()->rpbmCast());
		Assert(pbm);
		CPArray< CTransform3<> > patf3 = pbm->patf3JointsOrig;

		TReal r_scale = pins->fGetScale();

		params.front_hip_radius	= rLength(patf3, 20, 24) * 0.5 * r_scale;
		params.front_hip_height = patf3[20].v3Pos.tZ * r_scale;

		params.front_leg_length = (rLength(patf3, 20) +
								  rLength(patf3, 21) +
								  rLength(patf3, 22)) * r_scale;

		// Half-distance between centres of front-back hips.
		params.leg_offset		= (patf3[12].v3Pos + patf3[16].v3Pos - 
								   patf3[20].v3Pos - patf3[24].v3Pos).tLen() * 0.25 * r_scale;
	}

	//*****************************************************************************************
	void CPhysicsInfoQuadruped::Init(CAnimate* pani) const
	{
		// Construct any non-loaded boundary boxes.
		pani->AddBoundaryBox(ebbBODY, CVector3<>(.15, .3, .2), v3Zero, 100, mat_trike_body, 1.0, 1.0);
		pani->AddBoundaryBox(ebbHEAD, CVector3<>(.1, .2, .1), v3Zero, 100, mat_trike_head, 1.0, 1.0);
		pani->AddBoundaryBox(ebbTAIL, CVector3<>(.05, .2, .05), v3Zero, 100, mat_trike_tail, 1.0, 1.0);
	}

	//*****************************************************************************************
	void StuffPelState
	(
		CAnimate* pani, 
		int i_joint,
		int i_pel, 
		int i_elem
	)
	{
		Assert(i_pel >= 0);
		Assert(i_elem >= 0);
		Assert(i_joint >= 0);

		// Get the current relative joint transform, convert to placement.
		CTransform3<>& tf3_joint = pani->sriBones.patf3JointTransforms[i_joint];
		CPlacement3<> p3_joint(tf3_joint.mx3Mat, tf3_joint.v3Pos * pani->pr3GetPresence().rScale);

		// Convert to world space, and stuff it in the pelvis.
		CPlacement3<> p3_world = p3_joint * pani->p3GetPlacement();
		Copy(&Pel[i_pel][i_elem], p3_world, 0);
	}

	//*****************************************************************************************
	void CPhysicsInfoQuadruped::CreatePhysics(CInstance* pins, int i_index, float aaf_state[7][3]) const
	{
		Assert(i_index >= 0);

		// Get params from biomesh.
		Quad_Params params;
		GetQuadParams(pins, &params);

		params.mass = fMass(pins);
/*
		// Fill with data for quad.
		params.mass = 20;

		params.leg_length = 2.2;//1.8;
		params.hip_radius = .791;
 
		params.front_hip_radius = .51;
		params.leg_offset = 1.16;

		params.tail_length = 2;
		params.neck_length = 1;
		params.neck_offset[0] = 0;
		params.neck_offset[1] = 2;
		params.neck_offset[2] = 0;

		params.tail_offset[0] = 0;
		params.tail_offset[1] =-1.17;
		params.tail_offset[2] = 0;
*/
		//  Make the quad.
		Make_a_Quad(pins, i_index, params, aaf_state);

		CAnimate* pani = ptCast<CAnimate>(pins);

		// Stuff selected physics state from current joint data.
		StuffPelState(pani, 15, i_index, RIGHT_FOOT);
		StuffPelState(pani, 19, i_index, LEFT_FOOT);
		StuffPelState(pani, 23, i_index, FRIGHT_FOOT);
		StuffPelState(pani, 27, i_index, FLEFT_FOOT);

		//
		// Now apply the boundary conditions.
		//

		ActivateBoundaryBoxes(i_index, pani);
	}

	//*****************************************************************************************
	void CPhysicsInfoQuadruped::GetData
	( 
		int model,
		float loc[7],
		float points[iMAX_SKELETAL_ELEMENTS][3], float matrices[iMAX_SKELETAL_ELEMENTS][3][3],
		int Am_I_Supported[iMAX_SKELETAL_ELEMENTS]
	) const
	{
		// Call the right guy.
		Get_Quad_Data( model, loc, points, matrices, Am_I_Supported);
	}

	//*****************************************************************************************
	void CPhysicsInfoQuadruped::DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const
	{
#if bVER_BONES()
		// Since we re-use the biped joints, we call its bones routine.
		// Here, we just code the extra legs.
		static int ai_attach_extra[] =
		{
			0, 1, 2, 3, 4,	// 0: 20 empty values.
			5, 6, 7, 8, 9,
			10, 11, 12, 13, 14,
			15, 16, 17, 18, 19,
			20,				// 20: Base of right front leg.
			20, 21, 22,		// Right front leg.
			24,				// 24: Base of left front leg.
			24, 25, 26,		// Left front leg.
			0,				// 29: Static joint.
		};

		// Draw the biped stuff.
		CPhysicsInfoBiped::DrawPhysics(pins, draw, cam);

		// Draw the front legs.
		DrawBones(pins, CPArray<int>(ArrayData(ai_attach_extra)), draw, cam);
#endif
	}

	//*****************************************************************************************
	void CPhysicsInfoQuadruped::HandleMessage
	(
		const CMessagePhysicsReq& msgpr,
		CInstance *pins
	) const
	{
		// Call parent.
		CPhysicsInfoBiped::HandleMessage(msgpr, pins);
	}
	

	//*****************************************************************************************
	float CPhysicsInfoQuadruped::fDamageMultiplier
	(
		const CInstance* pins ,
		TSoundMaterial tsmat
	) const
	{
		Assert(pins);

		if (tsmat == mat_trike_head)
		{
			return 4 * fDamage;
		}
		else if (tsmat == mat_trike_body)
		{
			return 0.1 * fDamage;
		} 
		else if (tsmat == mat_trike_tail)
		{
			return 1 * fDamage;
		}
		else
		{
			// Unknown quadruped sound material.
			Assert(false);
			return 0;
		}
	}

	//*****************************************************************************************
	float CPhysicsInfoQuadruped::fArmourMultiplier
	(
		const CInstance* pins,
		TSoundMaterial tsmat 
	) const
	{
		Assert(pins);

		if (tsmat == mat_trike_head)
		{
			return 0.1 * fArmour;
		}
		else if (tsmat == mat_trike_body)
		{
			return 1 * fArmour;
		} 
		else if (tsmat == mat_trike_tail)
		{
			return 0.5 * fArmour;
		}
		else
		{
			// Unknown quadruped sound material.
//			Assert(false);
			return 2.0;
		}	
	}


//**********************************************************************************************
//
//  CPhysicsInfoHuman implementation
//

	//*****************************************************************************************
	float CPhysicsInfoHuman::fDamageMultiplier
	(
		const CInstance* pins ,
		TSoundMaterial tsmat
	) const
	{
		Assert(pins);
		Assert(tsmat);

		return fDamage;
	}

	//*****************************************************************************************
	float CPhysicsInfoHuman::fArmourMultiplier
	(
		const CInstance* pins,
		TSoundMaterial tsmat 
	) const
	{
		Assert(pins);
		Assert(tsmat);

		return fArmour;
	}

	//*****************************************************************************************
	void CPhysicsInfoHuman::UpdateWDBase
	(
		CInstance *	pins,		// The instance to update
		int			i_index		// The index of that instance in the appropriate physics
								// system array.
	) const
	{
		CPhysicsInfoSkeleton::UpdateWDBase(pins, i_index);
	}

	//*****************************************************************************************
	void CPhysicsInfoHuman::Init(CAnimate* pani) const
	{
		// Construct any non-loaded boundary boxes.
		pani->AddBoundaryBox(ebbBODY, CVector3<>(.17, .22, .25), v3Zero, 100, matHashIdentifier("Anne"), 1.0, 1.0, "Anne");
		pani->AddBoundaryBox(ebbFOOT, CVector3<>(.25, .25, .25), v3Zero, 100, matHashIdentifier("Anne-Foot"), 1.0, 1.0, "Anne-Foot");
		pani->AddBoundaryBox(ebbHAND, CVector3<>(.05, .075, .025), CVector3<>(0, .09, 0), 1, 0, 0.0, 0.0, "Hand");

		// Anne shouldn't have a head, so let's decapitate her if she came with one.
		// Just let the damn thing leak, since apparently "delete" is too much for our
		// system to handle.
		pani->apbbBoundaryBoxes[(int)ebbHEAD] = 0;
	}

	//*****************************************************************************************
	void CPhysicsInfoHuman::CreatePhysics(CInstance* pins, int i_index, float aaf_state[7][3]) const
	{
		Assert(i_index >= 0);

		// The real magic.
		Make_a_Human(pins, i_index, aaf_state);


		CAnimate* pani = ptCast<CAnimate>(pins);
		ActivateBoundaryBoxes(i_index, pani);
	}

	//*****************************************************************************************
	void CPhysicsInfoHuman::GetData
	( 
		int model,
		float loc[7],
		float points[iMAX_SKELETAL_ELEMENTS][3], float matrices[iMAX_SKELETAL_ELEMENTS][3][3],
		int Am_I_Supported[iMAX_SKELETAL_ELEMENTS] 
	) const
	{
		Assert(HUMAN_SKELETAL_ELEMENTS <= iMAX_SKELETAL_ELEMENTS);

		// Call the right guy.
		Get_Human_Data( model, loc, points, matrices, Am_I_Supported);

		// HACK HACK HACK--  only get arm data, please.
		for (int i = iMAX_SKELETAL_ELEMENTS - 1; i >= 0; --i)
		{
			if (i < 10 || i > 13)
				Am_I_Supported[i] = 0;
		}
	}

	//*****************************************************************************************
	void CPhysicsInfoHuman::DrawPhysics(CInstance* pins, CDraw& draw, CCamera& cam) const
	{
#if bVER_BONES()
		// Well, for now let's use hard-coded joint connection info.
		// Later, this might be split into a connection data structure in this class, and a
		// general-purpose drawing function.

		static int ai_attach[] =
		{
			0,					// 0: Pelvis.
			0, 1, 2,			// 1: Right leg.
			4,				
			0, 5, 6,			// 5: Left leg.
			8, 9, 10, 
			10, 11, 12,			// 11: Right arm.
			14, 15, 16, 17, 18,	// 14: Empty
			9, 19,				// 19: Left mamm.
			9, 21				// 21: Right mamm.
		};

		DrawBones(pins, CPArray<int>(ArrayData(ai_attach)), draw, cam);

		// Call parent.
		CPhysicsInfoSkeleton::DrawPhysics(pins, draw, cam);
#endif
	}


	//*****************************************************************************************
	void CPhysicsInfoHuman::HandleMessage
	(
		const CMessagePhysicsReq& msgpr,
		CInstance *pins
	) const
	{
		CPhysicsInfoSkeleton::HandleMessage(msgpr, pins);

		// Make sure it's active.
		int i_index = pphSystem->iGetSkeletonIndex(pins);
		AlwaysAssert(i_index >= 0);

		// Handle arms.
		SControlArm cta;
		if (msgpr.subUserMoveHand.rtUrgency != 0)
		{
			cta.fPosUrgency = 1;
			Copy(cta.afPosition, msgpr.subUserMoveHand.dData);
		}

		if (msgpr.subUserOrientHand.rtUrgency != 0)
		{
			cta.fOrientUrgency = 1;

			// Convert the rotation to a matrix, and copy to the array.
			Copy(cta.aafOrient, CMatrix3<>(msgpr.subUserOrientHand.dData));
		}
		else
		{
			cta.fOrientUrgency = 0;

			// Set a unit matrix, just for good measure.
			Copy(cta.aafOrient, CMatrix3<>());
		}

		// If currently grasping an object, curl the hand.
		if (msgpr.subPickup.rtUrgency != 0 && msgpr.subPickup.dData)
			cta.fFingerCurl = -0.9;

		cta.bSwing = msgpr.subSwing.rtUrgency != 0 && msgpr.subSwing.dData;

		Control_Arms(i_index, cta);
	}

