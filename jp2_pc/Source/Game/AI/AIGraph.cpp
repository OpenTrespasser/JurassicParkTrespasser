/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 *	Implementation of AIGraph.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/AIGraph.cpp                                                  $
 * 
 * 37    9/27/98 11:34p Agrant
 * Better sphere rejection for paths
 * 
 * 36    9/17/98 6:47p Jpaque
 * safety assert
 * 
 * 35    9/11/98 12:43p Agrant
 * allow dinos to go downhill more than up hill
 * 
 * 34    9/02/98 5:24p Agrant
 * circle rejection for terrain knowledge
 * 
 * 33    9/02/98 3:58p Agrant
 * Now sharing graphs for all dinos in a level.
 * 
 * 32    8/31/98 5:08p Agrant
 * Asserts for bad node counts
 * 
 * 31    8/27/98 5:35p Agrant
 * fixed wacky pathfinding bug by disabling explicit surface transfers.  Will need to
 * re-address to allow surface transfers on or off sloped surfaces
 * 
 * 30    8/26/98 3:16a Agrant
 * node count paranoia
 * 
 * 29    8/26/98 1:04a Agrant
 * using the passable flag
 * 
 * 28    8/11/98 2:22p Agrant
 * fixed pathfinding bug
 * 
 * 27    7/14/98 9:43p Agrant
 * many pathfinding improvements
 * 
 * 26    6/13/98 8:03p Agrant
 * If we have too many nods, just stop adding them
 * 
 * 25    6/11/98 2:58p Agrant
 * Remove STL vector from CPath
 * 
 * 24    6/09/98 9:49p Agrant
 * Preliminary circle test to speed up path connectivity calculation
 * 
 * 23    6/08/98 9:46p Agrant
 * Handle influence motion gracefully by moving nodes with the influence
 * 
 * 22    5/18/98 4:36p Agrant
 * walk over short objects, walk under high ones
 * 
 * 21    5/16/98 10:00a Agrant
 * Pathfinding fixes for the AI bounding box vs the physics bounding box
 * Major pathfinding fix in general-  how did it ever work???
 * 
 * 20    5/10/98 6:26p Agrant
 * AI's now can ignore selected tangible objects.
 * misc. changes
 * 
 * 19    2/23/98 5:07p Agrant
 * AI save/load
 * 
 * 18    12/20/97 7:03p Agrant
 * Cleaning up AI includes
 * 
 * 17    9/11/97 1:52p Agrant
 * AIGraph now uses flags to determine different drawing styles
 * Removing an influence now removes any nodes associated with it.
 * 
 * 16    9/09/97 9:06p Agrant
 * removed old influence nodes
 * 
 * 15    8/29/97 5:59p Agrant
 * using port.hpp to reduce dependencies on AI Test files.
 * 
 * 14    8/28/97 4:00p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 14    8/22/97 6:33p Agrant
 * Remove all references to AI Test classes.
 * 
 * 13    8/12/97 6:34p Agrant
 * AddNode function to support placing nodes on terrain automatically.
 * 
 * 12    7/31/97 4:46p Agrant
 * Object pathfinding
 * 
 * 11    7/27/97 2:24p Agrant
 * Rudimentary pathfinding over objects
 * 
 * 10    7/14/97 1:00a Agrant
 * Supports NodeSource and Unknown node types.
 * 
 * 9     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 8     4/01/97 1:14p Agrant
 * Better influence forgetting code
 * 
 * 7     3/28/97 2:00p Agrant
 * Brain now points to graph, mentalstate, and worldview
 * Pathfinding more efficient
 * 
 * 6     3/26/97 5:35p Agrant
 * Switched graph over to sparse array instead of vector.
 * Graph now caches distance data to speed up A*
 * 
 * 5     2/16/97 5:37p Agrant
 * Even better pathfinding.
 * 
 * 4     2/11/97 7:11p Agrant
 * Another pathfinding improvement.
 * 
 * 3     2/10/97 4:15p Agrant
 * Intermediate pathfinding check-in
 * 
 * 2     2/09/97 8:21p Agrant
 * Added an AIGraph class to avoid template clutter in pathfinding code.
 * 
 * 1     2/05/97 1:07p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "AIGraph.hpp"

#include "WorldView.hpp"
//#include "Test/AI/DebugArray.hpp"
#include "AIMain.hpp"
#include "Brain.hpp"
#include "Influence.hpp"
#include "Lib\EntityDBase\Animal.hpp"

#include "Lib/GeomDBase/LineSegment.hpp"


//#include "Test/AI/QueryTest.hpp"

#include "Lib\EntityDBase\Query\QTerrain.hpp"

#include "Port.hpp"
#include "Lib/Loader/SaveFile.hpp"

#include "Lib/Sys/DebugConsole.hpp"

#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"

//*********************************************************************************************
//
//	Class CSpatialGraph3 implementation.
//

#define rBOX_SIZE .5
#define rPATH_MAX TReal(1000000)

//*********************************************************************************************
void CSpatialGraph3::Draw(bool b_draw_edges)
{

	if (b_draw_edges)
	{
		LineColour(0,255,0);

		for (int i = iMaxNodeIndex() - 1; i >= 0; i--)
			for (int i2 = iMaxNodeIndex() - 1; i2 >=0; i2--)
		{
			if (bIsSuccessor(i,i2))
			{
				Line(nNode(i), nNode(i2));
			}
		}
	}

	// Nodes are blue!
	LineColour(0,0,255);

	for (int i = iMaxNodeIndex() - 1; i >= 0; i--)
	{
		if (nNode(i).bIsValid())
		{
			// Draw each node as a diamond.
			CVector2<> v2	= nNode(i);
			CVector2<> v2_up(0,rBOX_SIZE);
			CVector2<> v2_right(rBOX_SIZE,0);

			Line(v2 + v2_up, v2 + v2_right);
			Line(v2 - v2_up, v2 + v2_right);
			Line(v2 + v2_up, v2 - v2_right);
			Line(v2 - v2_up, v2 - v2_right);
		}
	}
}

//*********************************************************************************************
void CSpatialGraph3::HighlightNode(int i_index)
{
	// Highlights the specified node with the current line colour.

	// If out of range, does nothing.
	if (i_index < 0 || i_index >= iMaxNodeIndex())
		return;

	// Draw each node as a diamond.
	CVector2<> v2		= nNode(i_index);
	CVector2<> v2_up(0,rBOX_SIZE*2);
	CVector2<> v2_right(rBOX_SIZE*2,0);

	Line(v2 + v2_up, v2 - v2_up);
	Line(v2 + v2_right, v2 - v2_right);
}

//*********************************************************************************************
int CSpatialGraph3::iNearestNode(CVector3<> v3)
{
	TReal r_min_dist_sq = rPATH_MAX * rPATH_MAX;
	TReal r_dist_sq;
	int i_best			= -1;
	
	for (int i = iMaxNodeIndex() - 1; i >= 0; i--)
	{
		r_dist_sq = (v3 - nNode(i)).tLenSqr();
		if (r_dist_sq < r_min_dist_sq)
		{
			r_min_dist_sq	= r_dist_sq;
			i_best			= i;
		}
	}
	return i_best;
}

//*********************************************************************************************
void CSpatialGraph3::DrawPath(const CGraphPath& gp)
{
	// Edges are white!
	LineColour(255,255,255);

	for (int i = gp.size() - 1; i > 0; i--)
	{
		// Draw each line.
		Line(nNode(gp[i]), nNode(gp[i - 1]));

	}
}


//*********************************************************************************
bool CSpatialGraph3::bIsSuccessor
(
	int i_parent,
	int i_successor
)
{
	Assert(nNode(i_parent).bIsValid());
	Assert(nNode(i_successor).bIsValid());

	if (i_parent == i_successor)
		return false;

	if (pqtwWalls == 0)
		return true;

#ifdef VER_AI_TEST
	// This doesn't work, since AI Lib doesn't have the VER_AI_TEST defined.


	CWDbQueryTestWall& qtw = *pqtwWalls;

	CLineSegment2<> seg(nNode(i_parent), nNode(i_successor));

	// for each wall, make sure that it does not intersect the graph edge.
	foreach(qtw)
	{
		if (qtw.tGet()->bIntersects(seg))
			return false;
	}
#endif

	return true;
}


//*********************************************************************************************
//
//	Class CAIGraph implementation.
//

//*********************************************************************************************
CAIGraph::CAIGraph(CAnimal* pani, int i_max_nodes) : paniAnimal(pani), CGraph<CAIGraphNode, TReal >(1, i_max_nodes)
{
	// Do not reset on construction-  that will happen when the graph is assigned to a brain.
//	Reset();
}


//*********************************************************************************************
void CAIGraph::Reset()
{
	aNodes.Reset();

	CVector3<> v3(10,11,12);

	// Creating current location node.
	CAIGraphNode aign(v3, gaiSystem.sNow);
	aign.SetHistoryStart();
	AddNode(aign);

	// Creating destination node.
	aign.SetHistoryInvalid();
	aign.SetHistoryStop();
	AddNode(aign);

	Assert(aNodes.iNumElements == 2);
}

//*********************************************************************************************
void CAIGraph::InfluenceMoved(CInfluence *pinf, const CMessageMove& msgmv)
{
#if VER_TEST 
	bool b_has_nodes = false;
#endif

	// Change all nodes based on this influence to Old Influence histories.
	for (int i = iMaxNodeIndex() - 1; i > 0; i--)
	{
		CNHInfluence* nhi = nNode(i).pnhGetNodeHistory()->pCastNHInfluence();

		if (nhi)
		{
			if (nhi->pinfInfluence == pinf)
			{
#ifdef TRACK_OLD_NODES
				pnNode(i)->SetHistoryInfluenceOld(pinf);
#else
//				pnNode(i)->SetHistoryUnknown();
//				RemoveNode(i);

				//  Let's just move this node to match the new object position.
				CVector3<> v3_node_relative = pnNode(i)->v3Pos * ~msgmv.p3Prev;
				pnNode(i)->v3Pos = v3_node_relative * msgmv.pinsMover->pr3Presence().p3Placement();
#endif	

#if VER_TEST
				b_has_nodes = true;
#endif

			}
		}
	}

#if VER_TEST
	AlwaysAssert(b_has_nodes);
#endif
}

//*********************************************************************************************
void CAIGraph::SetStart(CVector3<> v3)
{
	pnNode(0)->v3Pos = v3;
}

//*********************************************************************************************
void CAIGraph::SetStop(CVector3<> v3)
{
	pnNode(1)->v3Pos = v3;
}


//*********************************************************************************************
bool CAIGraph::bIsSuccessor(int i_from, int i_to)
{
	// This really asks, can I get from "from" to "to".
	Assert(bIsValidIndex(i_from));
	Assert(bIsValidIndex(i_to));

	Assert(nNode(i_from).bIsValid());
	Assert(nNode(i_to).bIsValid());


	///////////////////
#define CACHE_AND_RETURN_FALSE 	{								\
		a2dDistances(i_from,i_to) = CAIGraphNode::dInfinite();	\
		a2dDistances(i_to,i_from) = CAIGraphNode::dInfinite();	\
		return false;											\
	}
	///////////////////

	if (i_from == i_to)
		//|| !nNode(i_to).bIsValid())
	{
		return false;
	}

	const CAIGraphNode & n_from = nNode(i_from);
	const CAIGraphNode & n_to	= nNode(i_to);

	// Never search beyond the stop node.
	if (n_from.bIsStop())
		return false;


	TReal r = a2dDistances(i_from,i_to);

	// Have we already asked this question?
	if (!CAIGraphNode::bIsUnknown(r))
	{
		// If infinite, return false, else true.
		return !CAIGraphNode::bIsInfinite(r);
	}


	bool b_from_is_start	= nNode(i_from).bIsStart();
	bool b_to_is_stop		= nNode(i_to).bIsStop();

	// For now ignore surface restrictions for start and stop.
	if (b_from_is_start == false && b_to_is_stop == false) 
	{
		// Are we on the same surface?
		if (n_from.pinsSurface != n_to.pinsSurface)
		{
			// Different surfaces!  This needs special treatment.
			if (bCanSurfaceTransfer(i_from, i_to))
			{
				// Save distance!
				TReal r_d = dDistanceTo(i_from, i_to);

				Assert(r_d > 0);

				a2dDistances(i_from, i_to) = r_d;
				a2dDistances(i_to, i_from) = r_d;

				return true;
			}
			else
			{
				CACHE_AND_RETURN_FALSE;
			}
		}
	}
	else
	{
		// We have a start or stop.
		if (bCanSurfaceTransfer(i_from, i_to))
		{
			// Save distance!
			TReal r_d = dDistanceTo(i_from, i_to);

			Assert(r_d > 0);

			a2dDistances(i_from, i_to) = r_d;
			a2dDistances(i_to, i_from) = r_d;

			return true;
		}

		// If we get here, then we have a start or stop node of a different surface than the other node.  In order
		// for this to work correctly, in the long run we'll need an accurate surface for start and stop nodes.
		// For now, we get some false positives, ie node connections which are not valid.

	}

	// HACK HACK HACK
	// Check to see if the node is in the same place as another node
	// This could be better done at node creation time.
	// Ignore Z.  If tow are vertically displaced, it's not really on option to
	// move directly from one to another.
	if ((CVector2<>(n_from.v3Pos) - CVector2<>(n_to.v3Pos)).tLenSqr() <= .01f)
	{
		// Cache the connectivity.
		a2dDistances(i_from,i_to) = CAIGraphNode::dInfinite();
		
		// Same for the other direction...
		a2dDistances(i_to,i_from) = CAIGraphNode::dInfinite();
		return false;
	}


	// We could cache the answers to these questions for a speed boost.

	CInfluenceList* pinfl = &paniAnimal->pbrBrain->pwvWorldView->inflInfluences;
	CInfluenceList::iterator pinf = pinfl->begin();

	CLineSegment2<> ls(nNode(i_from).v3Pos, nNode(i_to).v3Pos);
	CVector2<>  v2_line_center = (ls.v2From + ls.v2To) * 0.5f;
	TReal		v2_line_radius = fSqrtEst((ls.v2From - v2_line_center).tLenSqr());

	for( ; pinf != pinfl->end(); ++pinf)
	{	
		// Ignore the influence if it's solidity is small emaough.
		if (!(*pinf).setNodeFlags[ensfIS_PASSABLE])
		{
			// Circle rejection first.
			TReal r_dist_between_centers_sqr = (CVector2<>((*pinf).v3Location) - v2_line_center).tLenSqr();
			if (r_dist_between_centers_sqr < Sqr(v2_line_radius + (*pinf).pinsTarget->fGetScale() * 1.7f))
			{
				if ((*pinf).bBlocksLine(&ls, paniAnimal->pbrBrain))
				{
					// Cache the connectivity.
					a2dDistances(i_from,i_to) = CAIGraphNode::dInfinite();

					// Same for the other direction...
					a2dDistances(i_to,i_from) = CAIGraphNode::dInfinite();

					return false;
				}
			}
		}
	}

	// Tested all inlfluences.

	// Assume success both forward and backward.
	bool b_can_go_forward = true;
	bool b_can_go_backward = true;

	CVector2<> v2_direction = ls.v2To - ls.v2From;

	// Now test all steep terrain polygons.
	if (paniAnimal->pbrBrain->bUseTerrainPathfinding)
	{
		TReal r_min_downhill_z = paniAnimal->pbrBrain->rMinDownHillNormalZ;

		list<CTerrainKnowledge>* pltk = &paniAnimal->pbrBrain->pwvWorldView->ltkKnowledge;
		list<CTerrainKnowledge>::iterator ptk = pltk->begin();

		for ( ; ptk != pltk->end(); ++ptk)
		{
			// Circle rejection first.
			TReal r_dist_between_centers_sqr = ((*ptk).v2Center - v2_line_center).tLenSqr();
			if (r_dist_between_centers_sqr < Sqr(v2_line_radius + (*ptk).rRadius * 1.5))
			{
				if ((*ptk).bEdgeIntersects(ls))
				{
					// If you want it speedy.
					//  CACHE_AND_RETURN_FALSE;


					// An edge intersects-  time to go back to the original data and check to see if we can go this direction
					// on this terrain poly.
					Assert((*ptk).ad3Normals.uLen == (*ptk).asilOriginalSilhouettes.uLen);

					// Iterate through all normals.
					int i = (*ptk).ad3Normals.uLen - 1;
					for ( ; i >= 0; --i)
					{
						// Does the line hit this silhouette?
						if ((*ptk).asilOriginalSilhouettes[i].bEdgeIntersects(ls))
						{
							// Yes.  Now see if we can navigate this guy.
							
							// Do we have a slope that's too high even to go down?
							if (r_min_downhill_z > (*ptk).ad3Normals[i].tZ)
							{
								// Yes.  Too steep.
								// Done searching.
								CACHE_AND_RETURN_FALSE;
							}

							// Now we have a slope we can go down, but not up.

							CVector2<> v2_normal = (*ptk).ad3Normals[i];

							// Are they going the same direction?
							if (v2_normal * v2_direction > 0.0f)
							{
								// Yes!  We can go down hill this way.  But not the other way.
								b_can_go_backward = false;
							}
							else
							{
								// No.  We cannot go forward.
								b_can_go_forward  = false;
							}
						}

						if (!b_can_go_backward && !b_can_go_forward)
						{
							// Done searching.
							CACHE_AND_RETURN_FALSE;
						}
					}
				}
			}
		}
	}

	// All tests negative, go ahead with the path.

	// Save distance!
	TReal r_d = dDistanceTo(i_from, i_to);

	Assert(r_d > 0);

	if (b_can_go_backward)
		a2dDistances(i_to, i_from) = r_d;
	else
		a2dDistances(i_to, i_from) = CAIGraphNode::dInfinite();

	if (b_can_go_forward)
	{
		a2dDistances(i_from, i_to) = r_d;
		return true;
	}
	else
	{
		return false;
		a2dDistances(i_from, i_to) = CAIGraphNode::dInfinite();
	}

#undef CACHE_AND_RETURN_FALSE

}

//*********************************************************************************************
bool CAIGraph::bCanSurfaceTransfer(int i_from, int i_to)
{
	// WalkOver parameter should take care of this for us, except for the case of a sloped surface.
	return false;


	// Better not be on the same surface!
	Assert(nNode(i_from).pinsSurface != nNode(i_to).pinsSurface || nNode(i_from).bIsStart() || nNode(i_to).bIsStop());



	CBrain* pbr = gaiSystem.pbrGetCurrentBrain();

	// First, check to see if the nodes are close enough for a jump.
	CVector3<> v3_jump = (nNode(i_from).v3Pos - nNode(i_to).v3Pos);

	if (CVector2<>(v3_jump).tLenSqr() >= pbr->rJumpDistanceSqr)
	{
		// Too far to jump!
		return false;
	}

	// Are we jumping up or down?
	if (v3_jump.tZ >= 0)
	{
		// Jumping up.
		if (v3_jump.tZ > pbr->rJumpUp)
		{
			return false;
		}
	}
	else
	{
		// Jumping down.
		if (v3_jump.tZ < - pbr->rJumpDown)
		{
			return false;
		}
	}

	// If we've gotten this far, the jump is possible.  Need to check intervening obstacles.

	// For now, just say OK.

	// HACK HACK HACK
	
	return true;
}

//*********************************************************************************
void CAIGraph::AddNode(	CAIGraphNode& node )
{
	if (aNodes.iNumElements >= aNodes.uMax)
	{
		// We have too many nodes!  We can't have another one!
		dout << "WARNING:  Node limit exceeded!\n";
		return;
	}

	if (node.pinsSurface == 0)
	{
		// The node is on the terrain.
		// Set the Z accordingly.

		CWDbQueryTerrain wqt;

		CTerrain* pterr = wqt.tGet();
		if (pterr)
		{
			node.v3Pos.tZ = pterr->rHeight(node.v3Pos.tX, node.v3Pos.tY);
		}
		else
			node.v3Pos.tZ = 0.0f;
	}

	CNodeSource* pns = node.pnhGetNodeHistory()->pnsReferences();
	if (pns)
	{
		Assert(pns->cNodeCount >= 0);
		pns->cNodeCount++;
	}

	CGraph<CAIGraphNode,TReal>::AddNode(node);
}


//*********************************************************************************
void CAIGraph::ClearReferences(const CInstance* pins)
{
	Assert(pins);
	AlwaysAssert(this);

	// Invalidate all nodes referencing pins
	for (int i = iMaxNodeIndex() - 1; i > 0; i--)
	{
		if (nNode(i).pnhGetNodeHistory()->bReferences(pins))
		{
			// May as well keep it, but get rid of any history.
//			pnNode(i)->SetHistoryUnknown();
			RemoveNode(i);
		}
	}
}

//*********************************************************************************
void CAIGraph::ClearReferences(const CNodeSource* pns)
{
	Assert(pns);

	CNHNodeSource* nhpns = 0;
	CNHInfluence*  pnhi = 0;


	// Invalidate all nodes referencing pins
	for (int i = iMaxNodeIndex() - 1; i > 0; i--)
	{
		nhpns = nNode(i).pnhGetNodeHistory()->pCastNHNodeSource();

		if (nhpns && nhpns->pnsNodeSource == pns)
		{
			// May as well keep it, but get rid of any history.
//			pnNode(i)->SetHistoryUnknown();
			RemoveNode(i);
		}
		else
		{
			// Check for influence history!
			pnhi = nNode(i).pnhGetNodeHistory()->pCastNHInfluence();
			if (pnhi && (CNodeSource*)pnhi->pinfInfluence == pns)
			{
				RemoveNode(i);
			}
		}
	}
}

//*********************************************************************************
void CAIGraph::RemoveNode(int i_node_index)
{
#if VER_DEBUG
	int i_num = iNumNodes();
#endif

	CNodeSource* pns = nNode(i_node_index).pnhGetNodeHistory()->pnsReferences();

#if VER_TEST
	{
		// Verify that the node count is correct
		char c_num_pns_nodes = 0;
		if (pns)
		{
			// Invalidate all nodes referencing pins
			for (int i = iMaxNodeIndex() - 1; i > 0; i--)
			{
				if (pns == nNode(i).pnhGetNodeHistory()->pnsReferences())
				{
					c_num_pns_nodes++;
				}
			}

			AlwaysAssert(c_num_pns_nodes == pns->cNodeCount);
		}
	}
#endif

	if (pns)
	{
		AlwaysAssert(pns->setNodeFlags[ensfIN_GRAPH]);
		AlwaysAssert(pns->cNodeCount > 0);
		pns->cNodeCount--;
		if (pns->cNodeCount == 0)
		{
			pns->setNodeFlags[ensfIN_GRAPH] = false;
		}
	}
		

	aNodes.Invalidate(i_node_index);

#if VER_TEST
	{
		// Verify that the node count is correct
		char c_num_pns_nodes = 0;
		if (pns)
		{
			// Invalidate all nodes referencing pins
			for (int i = iMaxNodeIndex() - 1; i > 0; i--)
			{
				if (pns == nNode(i).pnhGetNodeHistory()->pnsReferences())
				{
					c_num_pns_nodes++;
				}
			}

			AlwaysAssert(c_num_pns_nodes == pns->cNodeCount);
		}
	}
#endif


#if VER_DEBUG
	Assert(i_num - 1 == iNumNodes());
#endif
}


//*********************************************************************************************
void CAIGraph::Draw(bool b_draw_edges, bool b_draw_jumps)
{
	// Nodes are blue!
	LineColour(0,0,255);

	for (int i = iMaxNodeIndex() - 1; i >= 0; i--)
	{
		if (nNode(i).bIsValid())
		{
			// Draw each node as a diamond.
			CVector2<> v2	= nNode(i).v3Pos;
			CVector2<> v2_up(0,rBOX_SIZE);
			CVector2<> v2_right(rBOX_SIZE,0);

			Line(v2 + v2_up, v2 + v2_right);
			Line(v2 - v2_up, v2 + v2_right);
			Line(v2 + v2_up, v2 - v2_right);
			Line(v2 - v2_up, v2 - v2_right);
		}
	}

	if (b_draw_edges || b_draw_jumps)
	{
		LineColour(0,255,0);

		for (int i = iMaxNodeIndex() - 1; i >= 0; i--)
		{
			if (nNode(i).bIsValid())
			{
				for (int i2 = i - 1; i2 >=0; i2--)
				{
					if (nNode(i2).bIsValid())
					{
						if (b_draw_edges || (b_draw_jumps && nNode(i).pinsSurface != nNode(i2).pinsSurface))
						{
							if (bIsSuccessor(i,i2))
							{
								Line(nNode(i).v3Pos, nNode(i2).v3Pos);
							}	// if successor
						}	// if drawing all or would be a surface transfer
					}  // if node2 is valid
				}  // for all nodes
			}  // if node1 is valid
		} // for all nodes
	}  // if drawing some edges
}

//*********************************************************************************************
void CAIGraph::Draw()
{
	Draw(gaiSystem.bDrawAllGraphLinks, gaiSystem.bDrawJumpGraphLinks);
}

//*********************************************************************************************
void CAIGraph::HighlightNode(int i_index)
{
	// Highlights the specified node with the current line colour.

	// If out of range, does nothing.
	if (i_index < 0 || i_index >= iMaxNodeIndex())
		return;

	if (nNode(i_index).bIsValid())
	{
		
		// Draw each node as a diamond.
		CVector2<> v2		= nNode(i_index).v3Pos;
		CVector2<> v2_up(0,rBOX_SIZE*2);
		CVector2<> v2_right(rBOX_SIZE*2,0);

		Line(v2 + v2_up, v2 - v2_up);
		Line(v2 + v2_right, v2 - v2_right);
	}
}


//*********************************************************************************************
void CAIGraph::DrawPath(const CGraphPath& gp)
{
	// Edges are white!
	LineColour(255,255,255);

	for (int i = gp.size() - 1; i > 0; i--)
	{
		// Draw each line.
		Line(nNode(gp[i]).v3Pos, nNode(gp[i - 1]).v3Pos);

	}
}



//*****************************************************************************************
char *CAIGraph::pcSave(char * pc) const
{
//	Assert(rtThreshold.fVal >= 0.0f);
//	Assert(fMultiplier >= 0.0f);

//	pc = pcSaveT(pc, feelRatingFeeling);
//	pc = pcSaveT(pc, rtThreshold);
//	pc = pcSaveT(pc, fMultiplier);
//	pc = pcSaveT(pc, sFlags);

	// Clear the graph.  No telling if the old nodes are any good.

	return pc;
}

//*****************************************************************************************
const char *CAIGraph::pcLoad(const char * pc)
{
//	pc = pcLoadT(pc, &feelRatingFeeling);
//	pc = pcLoadT(pc, &rtThreshold);
//	pc = pcLoadT(pc, &fMultiplier);
//	pc = pcLoadT(pc, &sFlags);

//	Assert(rtThreshold.fVal >= 0.0f);
//	Assert(fMultiplier >= 0.0f);

	Reset();

	return pc;
}


//*********************************************************************************************
//
//	Class CAIGraphNode implementation.
//

CAIGraphNode::CAIGraphNode(const CVector3<>& v3, TSec s_now, CInstance* pins_surface) :
	v3Pos(v3), sLastUsed(s_now), pinsSurface(pins_surface)
{
	SetHistoryInvalid();
}



