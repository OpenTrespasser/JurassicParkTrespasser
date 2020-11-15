/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 *		Implementation of Influence.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/Influence.cpp                                                $
 * 
 * 67    9/26/98 9:13p Agrant
 * player is not a solid object for pathfinding
 * 
 * 66    9/14/98 10:17p Agrant
 * allow dinos to move things with the same AI mass as their strength
 * 
 * 65    9/12/98 1:08a Agrant
 * The new living flag for influences
 * 
 * 64    9/08/98 7:41p Agrant
 * better facing away implementation
 * 
 * 63    9/08/98 4:43p Agrant
 * facing away query
 * 
 * 62    8/26/98 11:16a Agrant
 * now using the passable flag
 * 
 * 61    8/26/98 3:16a Agrant
 * better influence rating
 * 
 * 60    8/23/98 3:38a Agrant
 * fix assert in making surface nodes, increrase importance of influences that block the path
 * 
 * 59    8/20/98 11:06p Agrant
 * ignore depends somewhat on individual influences rather than just general frustration
 * 
 * 58    8/17/98 6:49p Agrant
 * save functions
 * 
 * 57    8/16/98 3:21p Agrant
 * Much tweaking to simplify activity balancing
 * 
 * 56    8/13/98 6:56p Agrant
 * code to allow dinos to ignore influences that are unattainable
 * 
 * 55    8/11/98 2:20p Agrant
 * added assert
 * 
 * 54    7/20/98 12:27p Agrant
 * width fixes
 * 
 * 53    7/14/98 9:43p Agrant
 * many pathfinding improvements
 * 
 * 52    6/09/98 9:47p Agrant
 * Better opinion generation
 * 
 * 51    6/08/98 5:48p Agrant
 * Code for finding the edge of an influence rather than its center
 * 
 * 50    6/07/98 4:03p Agrant
 * Use AIMass and MoveableMass for pathfinding.
 * 
 * 49    6/05/98 6:08p Agrant
 * Revised synthesizer to handle random-access acitivities.
 * Also handles exclusive ones (the old method).
 * 
 * 48    6/04/98 7:11p Agrant
 * major AI specification change
 * 
 * 47    5/18/98 4:36p Agrant
 * walk over short objects, walk under high ones
 * 
 * 46    5/16/98 10:00a Agrant
 * Pathfinding fixes for the AI bounding box vs the physics bounding box
 * Major pathfinding fix in general-  how did it ever work???
 * 
 * 45    5/10/98 6:26p Agrant
 * AI's now can ignore selected tangible objects.
 * misc. changes
 * 
 * 44    3/12/98 7:54p Agrant
 * New silhouette internals (array instead of vector)
 * 2d pathfinding handles compound physics objects
 * 
 * 43    3/09/98 5:48p Agrant
 * Another case where the dino not handle compound physics
 * 
 * 42    3/06/98 9:34p Agrant
 * use render bounding box for compound physics objects
 *
 **********************************************************************************************/

#include "common.hpp"
#include "Influence.hpp"

#include "AIMain.hpp"
#include "Brain.hpp"
#include "AIInfo.hpp"
#include "AIGraph.hpp"
#include "PathAStar.hpp"

#include "Lib/EntityDBase/Animal.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"

#include "Lib/Physics/InfoBox.hpp"
#include "Lib/Physics/InfoCompound.hpp"

#include "Lib/Renderer/RenderType.hpp"

#include "Lib/Loader/SaveBuffer.hpp"

// Used to add a little extra width around obstacles.
#define rSLOP TReal(.1)

 
//*********************************************************************************************
//
//	Class CInfluence
//

	//*********************************************************************************
	//
	CInfluence::CInfluence(CAnimal* pani_self, CInstance *pins_target)
		: feelAboutTarget(), pinsTarget(pins_target)
	{
		Assert(pins_target);
		Assert(pins_target->paiiGetAIInfo());
		Assert(pani_self->paiiGetAIInfo());
		Assert(pani_self->pbrBrain);
	
		v3Location = pins_target->v3Pos();

		gaiSystem.GetOpinion(pani_self, pins_target, &feelAboutTarget);

		// Set the node suitability.
		CalculateNodeSuitability();

		// Some high value to avoid flushing the influence right away.
		rImportance = 100000;

		// By default, the influence blocks all heights.
		rMinZ = -1.0f;
		rMaxZ = 1000.0f;

		if (ptCast<CPlayer>((CAnimate*)pins_target->paniGetOwner()))
		{
			feelAboutTarget[eptSOLIDITY] = 0.0f;
		}

		// Initialize the passable flag!
		setNodeFlags[ensfIS_PASSABLE] = feelAboutTarget[eptSOLIDITY] <= pani_self->pbrBrain->rMoveableMass;

		AlwaysAssert(!setNodeFlags[ensfIN_GRAPH]);


		// Is it alive?
		const CAnimate* panimate = pins_target->paniGetOwner();
		if (panimate && !panimate->bDead())
			setFlags[eifLIVING] = true;
		else
			setFlags[eifLIVING] = false;
	}

	//*********************************************************************************
	void CInfluence::UpdateTo(const CVector3<>&	v3_observer_location)
	{
		v3ToTarget	=	v3Location - 
						v3_observer_location;
		rDistanceTo	=	v3ToTarget.tLen();
		rDistanceToInverted = 1 / rDistanceTo;

		// Update facing flag.
		CVector2<> v2_to_target = v3ToTarget;
		CVector2<> v2_target_facing = CVector3<>(0,1,0) * pinsTarget->r3Rot();
		// Is the target facing away from us?
		setFlags[eifFACING_AWAY] = (v2_to_target * v2_target_facing > 0);
	}

	//*********************************************************************************
	void CInfluence::UpdatePositionalData(CAnimal* pani)
	{
		v3Location = pinsTarget->v3Pos();

		UpdateTo(pani->v3Pos());

		CVector3<> v3_max;
		CVector3<> v3_min;
		pinsTarget->paiiGetAIInfo()->pbvGetBoundVol(pinsTarget)->GetWorldExtents(pinsTarget->pr3Presence(), v3_min, v3_max);
		rMinZ = v3_min.tZ;
		rMaxZ = v3_max.tZ;


		const CPhysicsInfoCompound* pphic = pinsTarget->pphiGetPhysicsInfo()->pphicCast();

		// Does the target have a compound physics model?
		if (pphic)
		{
			int i_num_submodels = pphic->iNumSubmodels;

			// Loop through the submodels, and add each independently.
			for (int i = 0 ; i < i_num_submodels; ++i)
			{
				// Make sure we have a silhouette to initialize...
				MakeSilhouette(i);

				// Get the world transform for the submodel.
				CPresence3<> pr3 = pphic->apr3SubmodelPresences[i] * pinsTarget->pr3Presence();

				// Init the zero silhouette.
				psilGetSilhouette(i)->Init(	pphic->apphiSubmodels[i]->pbvGetBoundVol(),
											pr3,
											pani->pbrBrain->rWidth * 0.5f);
			}
		}
		else
		{
			// Make sure that we have a silhouette in slot zero.
			MakeSilhouette(0);

			const CBoundVol* pbv = pinsTarget->paiiGetAIInfo()->pbvGetBoundVol(pinsTarget);
			
			// Init the zero silhouette.
			psilGetSilhouette(0)->Init(	pbv,
										pinsTarget->pr3Presence(),
										pani->pbrBrain->rWidth * 0.5f);
		}
	}

		
	//*********************************************************************************
	void CInfluence::UpdateForMove
	(
		CAnimal*, //pani,	// Animal to which this influence belongs.
		TSec	 //s_when // When the update occurred.
	)
	{
		// For now, do nothing.  Depend on the sensory input to handle all influence
		// details.
	}

	//*********************************************************************************
	int CInfluence::iAddToGraph
	(
		CBrain* pbr		// Brain to which this influence belongs.
	)
	{
		Assert(!setNodeFlags[ensfIN_GRAPH]);

		const CBoundVol* pbv = pinsTarget->paiiGetAIInfo()->pbvGetBoundVol(pinsTarget);
		const CPhysicsInfoCompound* pphic = pinsTarget->pphiGetPhysicsInfo()->pphicCast();

		// Track the total node count added.
		int i_node_count = 0;

		// Do we have a compound physics model?
		if (pphic)
		{
			// Yes!  Add all the nodes.  
			CSilhouette sil;

			// No 3d pathfinding for compound guys yet!!!
			for (int i_sil = pphic->iNumSubmodels - 1; i_sil >= 0; i_sil--)
			{	
				// Get the world transform for the submodel.
				CPresence3<> pr3 = pphic->apr3SubmodelPresences[i_sil] * pinsTarget->pr3Presence();

				// Init the zero silhouette.
				sil.Init(pphic->apphiSubmodels[i_sil]->pbvGetBoundVol(),
  						 pr3,
						 rSLOP + pbr->rWidth * 0.5f);

				for (int i_node = sil.size() - 1; i_node >= 0; --i_node)
				{
					CVector3<> v3(sil[i_node]);
					CAIGraphNode aign(v3, gaiSystem.sNow);

					aign.SetHistoryInfluence(this);

					pbr->paigGraph->AddNode(aign);

					++i_node_count;
				}
			}
		}
		else
		{
			// No! Do the normal thing.

			// First, calculate a silhouette with some slop to make our new nodes be just outside
			// the corners of the actual silhouette.
			CSilhouette sil(pbv,
							pinsTarget->pr3Presence(), 
							rSLOP + pbr->rWidth * 0.5f);

			int i_sil_size = sil.size();

	#define rNODE_SPACING 2.5f


			// Does this influence have a goodly surface to walk upon?

			// If the scale is big enough compared to dino width, keep looking.
			TReal r_width = gaiSystem.pbrGetCurrentBrain()->rWidth;

			if (pinsTarget->fGetScale() > r_width * .5f &&			// Is it big enough?
				pinsTarget->paiiGetAIInfo()->bClimb())	// Is it something to walk on?
			{
				// It's big enough.  Are any surfaces close enough to horizontal to be of use?

				const CPhysicsInfo* pphi = pinsTarget->pphiGetPhysicsInfo();
				const CPhysicsInfoBox* ppib = pphi->ppibCast();

				if (ppib)
				{
					// Then we have a box, and can move on.

					// Take the axis vectors and transform by the rotation of the instance.  
					// We can then look at it to determine which faces are close to horizontal.

					// Could just use the Z vector and examine each coordinate, but let's do it the slower
					// way for now.

					
					//CVector3<> v3_corner = ppib->bvbBoundVol[0];
					CVector3<> v3_corner = (*((CBoundVolBox*)pinsTarget->paiiGetAIInfo()->pbvGetBoundVol(pinsTarget)))[0];

					CVector3<> v3_x = CVector3<>(v3_corner.tX,0,0) * pinsTarget->r3Rot();
					CVector3<> v3_y = CVector3<>(0,v3_corner.tY,0) * pinsTarget->r3Rot();
					CVector3<> v3_z = CVector3<>(0,0,v3_corner.tZ) * pinsTarget->r3Rot();

					CVector3<>	v3_axis1;
					CVector3<>	v3_axis2;
					CVector3<>	v3_axis3;
					TReal		r_height;



	#define rHORIZONTAL_THRESHOLD .8
					if ((r_height = abs(v3_z.tZ)) > rHORIZONTAL_THRESHOLD * v3_corner.tZ)
					{		
						v3_axis1 = v3_x;
						v3_axis2 = v3_y;
						v3_axis3 = v3_z;
					}
					else if ((r_height = abs(v3_y.tZ)) > rHORIZONTAL_THRESHOLD * v3_corner.tY)
					{		
						v3_axis1 = v3_x;
						v3_axis2 = v3_z;
						v3_axis3 = v3_y;
					}
					else if ((r_height = abs(v3_x.tZ)) > rHORIZONTAL_THRESHOLD * v3_corner.tX)
					{		
						v3_axis1 = v3_y;
						v3_axis2 = v3_z;
						v3_axis3 = v3_x;
					}
					else
					{
						// No horizontal faces.  Return.
						// We're now in the graph!
						setNodeFlags[ensfIN_GRAPH] = true;

						return i_node_count;
					}

					TReal r_scale = pinsTarget->fGetScale();


	#define rINSET 0.5f
					// Now we can munge the two axis vectors and the height to get four corner nodes.
					v3_axis1 *= r_scale;
					v3_axis2 *= r_scale;

					if (v3_axis3.tZ < 0)
						v3_axis3 = -v3_axis3;
					v3_axis3 *= r_scale;

					r_height *= r_scale;

					// Start adding nodes (+,+)
					CVector3<> v3_pos = pinsTarget->v3Pos();
					v3_pos.tZ += .01;	// Pretend it's a bit higher so nodes on its surface are not IN its surface.

					// A multiplier to move the nodes inward from the edge slightly.
					TReal r_inset_scale = 1.0f - (rINSET / (v3_axis1 + v3_axis2).tLen());


					TReal r_half_jump = pbr->rJumpDistance * 0.5f;
					TReal r_half_jump_sqr = r_half_jump * r_half_jump;

					// Set up the nodes to add on the surface.  All added nodes will copy this one,
					//  but at different positions.
					CAIGraphNode aign(CVector3<>(0,0,0), gaiSystem.sNow);
					aign.pinsSurface = pinsTarget;
					aign.SetHistoryInfluenceSurface(this);

					// Three cases.   
					if (v3_axis1.tLenSqr() > r_half_jump_sqr)
					{
						if (v3_axis2.tLenSqr() > r_half_jump_sqr)
						{
							// The surface is really big, and requires a node line along each edge.

							// Make four node lines.  

							v3_axis1 *= r_inset_scale;
							v3_axis2 *= r_inset_scale;

							// 2 along axis1
							TReal r_half_jump = pbr->rJumpDistance * 0.5f;

							CVector3<> v3_delta = v3_axis1;
							v3_delta.Normalise(r_half_jump);

							v3_pos += v3_axis3;

							i_node_count += iAddNodeLine(	v3_pos + v3_axis2 + v3_axis1 - v3_delta, 
															v3_pos + v3_axis2 - v3_axis1 + v3_delta, 
															pbr->rJumpDistance * rNODE_SPACING,
															pbr,
															aign);
							i_node_count += iAddNodeLine(	v3_pos - v3_axis2 + v3_axis1 - v3_delta, 
															v3_pos - v3_axis2 - v3_axis1 + v3_delta, 
															pbr->rJumpDistance * rNODE_SPACING,
															pbr,
															aign);

							// 2 along axis2
							v3_delta = v3_axis2;
							v3_delta.Normalise(r_half_jump);

							i_node_count += iAddNodeLine(	v3_pos + v3_axis1 + v3_axis2 - v3_delta, 
															v3_pos + v3_axis1 - v3_axis2 + v3_delta, 
															pbr->rJumpDistance * rNODE_SPACING,
															pbr,
															aign);
							i_node_count += iAddNodeLine(	v3_pos - v3_axis1 + v3_axis2 - v3_delta, 
															v3_pos - v3_axis1 - v3_axis2 + v3_delta, 
															pbr->rJumpDistance * rNODE_SPACING,
															pbr,
															aign);
						}
						else
						{
							// The surface only requires a node line parallel to axis1.
							v3_axis1 *= r_inset_scale;

							CVector3<> v3_delta = v3_axis1;
							v3_delta.Normalise(r_half_jump);

							v3_pos += v3_axis3;

							i_node_count += iAddNodeLine(	v3_pos + v3_axis1 - v3_delta, 
															v3_pos - v3_axis1 + v3_delta, 
															pbr->rJumpDistance * rNODE_SPACING,
															pbr,
															aign);
						}
					} 
					else
					{
						// Axis1 is too short to need node lines.

						if (v3_axis2.tLenSqr() > r_half_jump_sqr)
						{
							// The surface only requires a node line parallel to axis2.
							v3_axis2 *= r_inset_scale;

							CVector3<> v3_delta = v3_axis2;
							v3_delta.Normalise(r_half_jump);

							v3_pos += v3_axis3;

							i_node_count += iAddNodeLine(	v3_pos + v3_axis2 - v3_delta, 
															v3_pos - v3_axis2 + v3_delta, 
															pbr->rJumpDistance * rNODE_SPACING,
															pbr,
															aign);
						}
						else
						{
							// Both axes are short.  One centered node will do.
							aign.v3Pos = v3_pos + v3_axis3;
							pbr->paigGraph->AddNode(aign);
						}
					}


					// Already made some nodes on the surface of the influence.  
					// Must include supporting nodes around the edges.  
					// These guys are on the terrain, and are only needed if they connect to one or more 
					// of the nodes on the surface itself.

					sil.push_back(sil[0]);		// Make a circle.

					aign.pinsSurface = 0;
					aign.Invalidate();
					aign.SetHistoryInfluenceSurface(this);

					for (int i = sil.size() - 1; i >= 1; i--)
					{
						// This adds the corner nodes twice!
						i_node_count += iAddNodeLine(	sil[i], 
														sil[i-1], 
														pbr->rJumpDistance * rNODE_SPACING * 0.5f,
														pbr,
														aign,
														false);
					}


				}
			}

			// Now add nodes on the corners of the silhouette for ordinary pathfinding.
			{
				// No surface to walk on.  Just use the minimal nodes, please.
				// We've might have added an extra trailing node above, so use the original size.
				for (int i = i_sil_size - 1; i >= 0; i--)
				{
					CVector3<> v3(sil[i]);
					CAIGraphNode aign(v3, gaiSystem.sNow);

					aign.SetHistoryInfluence(this);

					pbr->paigGraph->AddNode(aign);

					++i_node_count;
				}
			}
		}

		// We're now in the graph!
		setNodeFlags[ensfIN_GRAPH] = true;
		return i_node_count;
	}

	//*********************************************************************************
	int CInfluence::iAddNodeLine(CVector3<> v3_start, CVector3<> v3_stop, TReal r_max_step, CBrain* pbr, const CAIGraphNode& aign_source,
		bool b_add_endpoints)
	{
		// Creates a set of nodes on the line between start and stop, with no more than max_step between nodes.
		// Includes the endpoints as nodes.

		CVector3<> v3_delta = v3_stop - v3_start;
		TReal r_distance = v3_delta.tLen();

		CAIGraphNode aign = aign_source;
		
		int i_num_midpoints = int((r_distance / r_max_step) + .01);

		int i_num_nodes = i_num_midpoints + 2;

		// Get the delta between consecutive nodes.
		v3_delta.Normalise(TReal(r_distance / (i_num_midpoints + 1)));

		aign.v3Pos = v3_start;

		int i_node_count = 0;

		int i = i_num_nodes - 1;
		int i_end = 0;
		if (!b_add_endpoints)
		{
			// Skip last point.
			++i_end;

			// Skip first point.
			--i;
			aign.v3Pos += v3_delta;
		}

		for ( ; i >= i_end; --i)
		{
			pbr->paigGraph->AddNode(aign);
			++i_node_count;
			
			// Set up for next node.
			aign.v3Pos += v3_delta;
		}

		return i_node_count;
	}



	//*********************************************************************************
	void CInfluence::ResetTemporaryFlags()
	{
		CNodeSource::ResetTemporaryFlags();
		setFlags[eifIS_VISIBLE]	= false;

		if (gaiSystem.sNow > sIgnoreTimeOut && setFlags[eifIGNORE])
		{
			setFlags[eifIGNORE] = false;

			// We were ignoring, now we aren't.  Set a new waypoint so we don't ignore again right away.
			gaiSystem.pbrGetCurrentBrain()->SetWayPoint();
		}
	}

	//*********************************************************************************
	void CInfluence::CalculateImportance()
	{
		rImportance = 0;

		for (int i = eptEND - 1; i >= 0; i--)
		{
			rImportance += feelAboutTarget[i];
		}

		// Did it ever block our path?
		if (setNodeFlags[ensfBLOCKED_PATH])
			// Yes!  Don't forget it so easily!
			rImportance += 10.0f;
	}

	//*********************************************************************************
	void CInfluence::CalculateNodeSuitability
	(
	)
	{
		CInstance*	pins			= pinsTarget;
		const CPhysicsInfo*	pphi	= pins->pphiGetPhysicsInfo();  // OK physics info usage

//		if (!pphi->bIsTangible())
//		{
//			rNodeSuitability = 0;
//			return;
//		}

		// Bigger obstacles have better nodes, and scale is a good measure of size.
		TReal r_suits = pins->fGetScale();

		// Animals move a lot, and are less good.
		if (ptCast<CAnimate>((CInstance*)pins))
			r_suits *= .5;
		else if (!pphi->bIsMovable())
		{
			// Immoveable objects are better.
			r_suits *= 2;
		}

		rNodeSuitability = r_suits;
	}


	//*********************************************************************************
	TReal CInfluence::rRateNodeSuitability() const
	{
		return CNodeSource::rRateNodeSuitability() - rDistanceTo;
	}


	bool CInfluence::bFacingAway() const
	{
		return setFlags[eifFACING_AWAY];
	}
	
	//*********************************************************************************
	CVector3<> CInfluence::v3ClosestPointTo(const CVector3<>& v3) const
	{
		const CBoundVol* pbv = pinsTarget->paiiGetAIInfo()->pbvGetBoundVol(pinsTarget);
		SVolumeLoc vl;

		if (pbv->bRayIntersect(&vl, pinsTarget->pr3Presence(), CPlacement3<>(v3), 0, 0))
		{
			return vl.v3Location;;
		}
		else
			return CVector3<>(0,0,0);
	}

	//*********************************************************************************
	void CInfluence::Ignore(bool b)
	{
		// Are we trying to ignore?
		if (b)
		{
			// Yes!  Are we allowed to ignore yet?
			if (sIgnoreTimeOut < gaiSystem.sNow)
			{
				// Yes!  Go ahead and ignore.
				sIgnoreTimeOut = gaiSystem.sNow + 15.0f;
				setFlags[eifIGNORE] = true;
			}
		}
		else
		{
			// No!  We want to stop ignoring.
			sIgnoreTimeOut = gaiSystem.sNow + 5.0f;
			setFlags[eifIGNORE] = false;
		}
	}
		
		
	//*********************************************************************************
	bool CInfluence::bBlocksLine(CLineSegment2<> *pls, CBrain* pbr) const
	{
		// Check to see if the influence is too high or too low.
		if (rMinZ > pbr->paniOwner->v3Pos().tZ + pbr->rWalkUnder || rMaxZ < pbr->paniOwner->v3Pos().tZ + pbr->rWalkOver)
			return false;

		// Can we jump it?
		if (pbr->bCanJump && pbr->rJumpOver > rMaxZ)
			return false;

		// Check each silhouette against the line segment.
		if (bEdgeIntersects(*pls))
		{
			return true;
		}

		return false;
	}

	//*****************************************************************************************
char *CInfluence::pcSave(char * pc) const
{
	// Save target pointer.
	pc = pcSaveInstancePointer(pc, pinsTarget);

	// Save feeling.
	pc = pcSaveT(pc, feelAboutTarget);

	return pc;
}

//*****************************************************************************************
const char *CInfluence::pcLoad(const char * pc)
{
	// Load target.
	CInstance* pins = 0;
	pc = pcLoadInstancePointer(pc, &pins);

	AlwaysAssert(pins);

	{
		new (this) CInfluence(pins);

		v3Location = pins->v3Pos();

		// Set the node suitability.
		CalculateNodeSuitability();

		// Some high value to avoid flushing the influence right away.
		rImportance = 100000;

		// By default, the influence blocks all heights.
		rMinZ = -1.0f;
		rMaxZ = 1000.0f;
	}

	// And the feeling.
	pc = pcLoadT(pc, &feelAboutTarget);

	return pc;
}
