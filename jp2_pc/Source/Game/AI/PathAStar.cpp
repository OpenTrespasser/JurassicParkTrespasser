/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of PathAStar.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/PathAStar.cpp                                                $
 * 
 * 38    9/23/98 10:24p Agrant
 * debug info
 * 
 * 37    9/20/98 7:29p Agrant
 * better approximate paths
 * 
 * 36    9/19/98 1:41a Agrant
 * better pathfinding versus solid targets
 * 
 * 35    9/13/98 8:03p Agrant
 * allow pathfinding to cut short if within a certain distance of the stop node
 * 
 * 34    9/11/98 12:39p Agrant
 * added asserts
 * 
 * 33    8/27/98 5:32p Agrant
 * Forget nodes more easily
 * 
 * 32    8/25/98 11:24a Rvande
 * Checking in for Andrew S.
 * 
 * 31    8/24/98 10:02p Agrant
 * Do not limit path by distance-  it doesn't really speed things up since we limit on number
 * of segments anyway.
 * 
 * 30    8/23/98 3:34a Agrant
 * Random pathfinding nodes.
 * 
 * 29    8/17/98 6:48p Agrant
 * Safety assert to verify that you only pathfind once.
 * 
 * 28    8/13/98 6:53p Agrant
 * more ambitious node removal
 * 
 **********************************************************************************************/


#include "Common.hpp"
#include "PathAStar.hpp"
#include "Lib/EntityDBase/Animal.hpp"

#include <stdio.h>
//#include "Lib\Sys\Profile.cpp"

#include "Brain.hpp"
#include "AIGraph.hpp"
#include "AIMain.hpp"
#include "WorldView.hpp"



//#include "Test/AI/TestAnimal.hpp"

//*********************************************************************************************
//
//	Class AStar implementation.
//



//*********************************************************************************************
//
//	Class CPathAStar implementation.
//



	//*********************************************************************************
	bool CPathAStar::bFindPath
	(
		CPath*	ppath,		// The path to be improved by the pathfinder.
		CRating rt_solidity,
		TReal	r_close_enough
	)
	{
#if VER_TEST
		static TSec sLastPathfind = -1000;

		AlwaysAssert(gaiSystem.sNow != sLastPathfind);
		sLastPathfind = gaiSystem.sNow;
#endif


		// Okay, this is kind of goofy to pass the path as a CPath instead of just two points.

		Assert(pbrBrain);

		CAIGraph * paig = pbrBrain->paigGraph;
		Assert(paig);
		AlwaysAssert(paig->paniAnimal);

		Assert(ppath->size() > 1);

		// Set the start and end points of the AIGraph in the worldview.
		int i_start = 0;
		int i_stop = ppath->size() - 1;
		paig->SetStart((*ppath)[i_start]);
		paig->SetStop((*ppath)[i_stop]);


		// Add some nodes to our graph if our brain is not full.
		(void) bMaybeAddNodes();
				
		// Remove some nodes.
		(void) bMaybeRemoveNodes();

		// Create an AStar search engine.
		CAStarAIGraph astar(paig);
		astar.rCloseEnough = r_close_enough;

		// Set the solidity threshhold.
		paig->SetSolidityThreshhold(rt_solidity);

		// Tell the astar when to give up.
		astar.dTooFar	= pbrBrain->rMaxPathLength;
		astar.iTooHigh	= pbrBrain->iMaxAStarSteps;

		// Find the path.
		astar.Search();

		// Now for debug update.
		{
			pbrBrain->gpTestPath.Clear();
			astar.GetPath(&pbrBrain->gpTestPath);
		}

		// If successful
		if (astar.bHasValidPath)
		{
			// Update the ppath
			astar.GetCPath(ppath, i_stop);
			pbrBrain->MaybeDebugSay("Pathfind Succeeded!\n");

			return true;
		}
		else
		{
			// Otherwise
			pbrBrain->MaybeDebugSay("Pathfind Failed: \n");

			// Find the attainable node closest to the destination.
			int i_best_attainable = astar.iFindBestAttainableNode();

			if (i_best_attainable < 0)
			{
				pbrBrain->MaybeDebugSay("hopeless!\n");

				// For now, make the animal stand still.
				//(*ppath)[1] = (*ppath)[0];

				return false;				
			}
			else
			{
				pbrBrain->MaybeDebugSay("taking bad path.\n");
	
				// Get the path to that attainable node.
				astar.GetCPath(ppath, i_best_attainable);

#if VER_TEST
				// Now for debug update.
				{
					pbrBrain->gpTestPath.Clear();
					astar.GetPathUpToIndex(&pbrBrain->gpTestPath, i_best_attainable);
				}
#endif
		
				return false;
			}
		}
	}

	//*********************************************************************************
	bool CPathAStar::bFindDirectionPath
	(
		CVector2<>			v2_location,	// Starting point.
		CVector2<>			v2_direction,	// Direction of travel.
		CPath*				ppath,			// The path to be filled by the pathfinder.
		CRating				rt_solidity
	)
	{
		// Not yet implemented!!!
		Assert(0);
		return false;
	}

	//*********************************************************************************
	bool CPathAStar::bMaybeAddNodes
	(
	)
	{
		// First, determine if we can learn some nodes right now.
		if (pbrBrain->sWhenToLearn < gaiSystem.sNow)
		{
			// Then we can learn!
			
			// If brain is full, do not learn.
			if (pbrBrain->paigGraph->iNumNodes() >= pbrBrain->iMaxNodes)
				return false;
			
			// Find the best new influence!
			TReal				r_best		= 0;
			CNodeSource*		pns_best	= 0;
			
			// For now, just grab an influence that is not up to date.
			CInfluenceList& infl = pbrBrain->pwvWorldView->inflInfluences;
			CInfluenceList::iterator pinf = infl.begin();

			// Check the influences.
			for( ; pinf != infl.end(); pinf++)
			{
				if (!(*pinf).setNodeFlags[ensfIN_GRAPH])
				{
					// Check it versus best so far.
					TReal r = (*pinf).rRateNodeSuitability();

					if (r > r_best)
						pns_best = (CInfluence*)&(*pinf);
				}
			}

			// Check the terrain polys.
			list<CTerrainKnowledge>* pltk = &pbrBrain->pwvWorldView->ltkKnowledge;
			list<CTerrainKnowledge>::iterator ptk = pltk->begin();

			for (; ptk != pltk->end(); ++ptk)
			{
				if (!(*ptk).setNodeFlags[ensfIN_GRAPH])
				{
					// Check it versus best so far.
					TReal r = (*ptk).rRateNodeSuitability();

					if (r > r_best)
						pns_best = &(*ptk);
				}
			}

			// How many nodes are we adding?
			int i_num_nodes = 0;

			// Has it been a while since we were able to learn?
			if (pbrBrain->sWhenToLearn + 0.3f < gaiSystem.sNow)
			{
				// Yes, it's been a while.  // Pick a random node location.
				CVector3<> v3 = CVector3<>(	gaiSystem.rnd(pbrBrain->rObjectSensoryRange * -1.0f, pbrBrain->rObjectSensoryRange * 1.0f), 
											gaiSystem.rnd(pbrBrain->rObjectSensoryRange * -1.0f, pbrBrain->rObjectSensoryRange * 1.0f), 
											0);
				v3 += pbrBrain->paniOwner->v3Pos();
					
				CAIGraphNode aign(v3, gaiSystem.sNow);
				aign.SetHistoryRandom();
				pbrBrain->paigGraph->AddNode(aign);
				++i_num_nodes;
			}

			if (pns_best)
			{
				// Make sure that we have room for all corners of the influence's silhouette.
				if (pns_best->iNumNodes() + pbrBrain->paigGraph->iNumNodes() >= pbrBrain->iMaxNodes)  //lint !e574
				{
					// Do nothing.
				}
				else
				{
					i_num_nodes += pns_best->iAddToGraph(pbrBrain);
				}
			}

			if (i_num_nodes > 0)
			{
				// Increment the learn time.
				// Take up some brainpower.
				if (pbrBrain->sWhenToLearn < gaiSystem.sNow)
					pbrBrain->sWhenToLearn = gaiSystem.sNow;
				pbrBrain->sWhenToLearn += TReal(i_num_nodes) / pbrBrain->rNodesPerSec;
				return true;
			}
		}

		return false;
	}

	//*********************************************************************************
	bool CPathAStar::bMaybeRemoveNodes
	(
	)
	{
		// Don't bother garbage collecting nodes if there aren't very many.
		// For now, garbage collect always to test the system. HACK HACK HACK
//		if (pbrBrain->paigGraph->iNumNodes() <= pbrBrain->iMaxNodes / 4)
//			return false;

		int		i_worst_index	= -1;
		float	f_worst_rating	= 0;
		bool	b_removed_one	= false;

		// If our brain is full, lower the threshhold.
		int i_num_nodes_to_kill = pbrBrain->paigGraph->iNumNodes() - pbrBrain->iMaxNodes;


		if (i_num_nodes_to_kill <= 0)
		{
			f_worst_rating = pbrBrain->sTimeToForget;
		}
	
		CAIGraph *pGraph = pbrBrain->paigGraph;
		AlwaysAssert(pGraph->paniAnimal);

		// Find the worst node.
		for (int i = pGraph->iMaxNodeIndex() - 1; i >= 0; i--)
		{
			if (pGraph->nNode(i).bIsValid())
			{
				CNodeHistory*	pnh = pGraph->nNode(i).pnhGetNodeHistory();

				if (pnh->setFlags[enfDISCARDABLE])
				{
					// This is a discardable node.  Discard it!
					Remove(i);
					--i_num_nodes_to_kill;
					b_removed_one = true;
					continue;
				}

				float f_rating = pnh->fRate(pGraph, i);

				// Is this node really bad?
				if (f_rating >= pbrBrain->sTimeToForget * 1.0f)
				{
					// Yes!  Get rid of it!
					Remove(i);
					--i_num_nodes_to_kill;
					b_removed_one = true;
					continue;
				}

				if (f_rating > f_worst_rating)
				{
					f_worst_rating = f_rating;
					i_worst_index = i;
				}
			}
		}

		if (i_num_nodes_to_kill > 0)
			pbrBrain->MaybeDebugSay("Too many nodes left!\n");

		char ac[256];
		sprintf(ac, "got %d nodes\n", pbrBrain->paigGraph->iNumNodes());
		pbrBrain->MaybeDebugSay(ac);


		// No bad nodes found.
		if (i_worst_index != -1)
		{
			Remove(i_worst_index);
			return true;
		}
		else return b_removed_one;
	}


	//*********************************************************************************
	void CPathAStar::Remove(int i_node)
	{
		CAIGraph *pGraph = pbrBrain->paigGraph;

		// Remove the bad node.
		CNodeHistory* pnh = pGraph->nNode(i_node).pnhGetNodeHistory();

		// Call the specialized delete code for the node history, because they
		// are never actually destructed.
		pnh->Delete(pGraph, i_node);
	}


//*********************************************************************************************
//
//	Class CAStarAIGraph implementation.
//

//  Compatability with template code!
#define DISTANCE TReal


	//*********************************************************************************
	void CAStarAIGraph::GetCPath
	(
		CPath* pp,	// The path to fill.
		int i_stop_index
	)
	{
		CGraphPath gp;

		// Get the path even if invalid.
//		if (bHasValidPath)
		{
			GetPathUpToIndex(&gp, i_stop_index);

			pp->Clear();

			int i_size = gp.size();
			for (int i = 0; i < i_size; i++)
			{
				CAIGraphNode* paign = pGraph->pnNode(gp[i]);

				Assert(paign->bIsValid());

				CVector3<> v3_ret = paign->v3Pos;
				pp->push_back(v3_ret);

				// Mark each node as used in a path recently.
				paign->pnhGetNodeHistory()->WasUsed(paign, gaiSystem.sNow);

			}
		}
	}

		//*********************************************************************************
		//
		void CAStarAIGraph::Search
		(
		)
		//
		//	Performs a search.
		//
		//	Notes:
		//
		//******************************
		{
			Assert(pGraph->bIsValidIndex(iStartNode));
			Assert(pGraph->bIsValidIndex(iStopNode));

			pGraph->ClearDistanceCache();

			aasniInfo[iStartNode].dEstRemaining = pGraph->nNode(iStartNode).dEstimatedDistanceTo(pGraph->nNode(iStopNode));
			aasniInfo[iStartNode].dSoFar		= 0;   // No distance from start to start!
			aasniInfo[iStartNode].dEstTotal     = aasniInfo[iStartNode].dEstRemaining +
													aasniInfo[iStartNode].dSoFar;
			aasniInfo[iStartNode].bIsUpToDate	= false;   // Must expand first node!

			int i_current_node = iStartNode;

			int i_num_nodes = aasniInfo.uLen;


			// Before we start, check connectivity to stop node.  If some node connects the stop node, 
			// we'll zero the close enough distance.
			{
				//  Check connectivity with stop node...
				int i_node = i_num_nodes - 1;
				for ( ; i_node >= 0; --i_node)
				{
					if (pGraph->nNode(i_node).bIsValid())
					{
						bool b_successor = pGraph->bIsSuccessor(i_node, iStopNode);

						if (b_successor)
						{
							// Somebody can reach it!  Do not settle for an approximation.
							rCloseEnough = 0.0f;
							break;
						}
					}
				}
			}


			// Use a for loop instead of while(1) for safety.
			int i_high_count = iTooHigh;
			int i_count;
			for (i_count = 0; i_count < i_high_count; )
			{
				// We should not fail in finding a low estimate.
				if (i_current_node == -1)
				{
					// Failed.
					bHasValidPath = false;
					// dout << i_count << " tries!  FAIL\n";
					return;
				}

				if (i_current_node == iStopNode)
				{
					bHasValidPath = true;
					return;
				}

				if (pGraph->nNode(i_current_node).dEstimatedDistanceTo(pGraph->nNode(iStopNode)) <= rCloseEnough)
				{
					// We are close.  Fake a good path!
					aasniInfo[iStopNode].iNodeBefore = i_current_node;
					bHasValidPath = true;
					return;
				}


				Assert(pGraph->nNode(i_current_node).bIsValid());


				// Since we have a segment cap, it seems silly to enforce the broken notion of 
				// a path length limit.
				/*
				if (aasniInfo[i_current_node].dEstTotal > dTooFar)
				{
					// The path is too long.
					// Failed.
					bHasValidPath = false;
					// dout << i_count << " tries!  FAIL\n";
					return;
				}
				*/

				bool b_already_tried_stop_node = false;

				int i_successor = iStopNode;
				while(i_successor >= 0)
				{
					if (pGraph->nNode(i_successor).bIsValid())
					{
						i_count++;

						bool b_successor = pGraph->bIsSuccessor(i_current_node, i_successor);

						if (b_successor)
						{
							Assert(pGraph->nNode(i_successor).bIsValid());


							DISTANCE d = pGraph->dDistanceTo(i_current_node, i_successor);

							Assert(d > 0);

							d += dSoFar(i_current_node);

							if (d < dSoFar(i_successor) || dSoFar(i_successor) < 0)
							{
								// d is shorter, or this node has not yet been examined.
							
								aasniInfo[i_successor].dSoFar = d;
								aasniInfo[i_successor].iNodeBefore = i_current_node;

								// Node must now be expanded again, as our distance has improved.
								aasniInfo[i_successor].bIsUpToDate = false;

								if (dEstRemaining(i_successor) < 0)
								{
									// Have not yet estimated remaining distance.	
									if (i_successor == iStopNode)
										aasniInfo[i_successor].dEstRemaining = 0;
									else aasniInfo[i_successor].dEstRemaining = 
										pGraph->nNode(i_successor).dEstimatedDistanceTo(pGraph->nNode(iStopNode));
								}

								aasniInfo[i_successor].dEstTotal = dSoFar(i_successor) + dEstRemaining(i_successor);
							}
						}
						// Ends bIsSuccessor
					}
					// End bIsValid

					if (b_already_tried_stop_node)
					{
						i_successor--;
					}
					else
					{
						// We've now tried for the straight shot.
						b_already_tried_stop_node = true;
	
						// Loop through the rest.
						i_successor = i_num_nodes - 1;

						// How can we test to see if this is the best we can do?
						if (iFindLowestEstTotal() == iStopNode)
						{
							// The stop node has a lower total than all other nodes, so ship it.

							bHasValidPath = true;
							// We're done!
							return;
						}
					}

					// Already did the stop node.  Decrement again!
					if (i_successor == iStopNode)
						i_successor--;
				}

				// Don't expand this node again!
				aasniInfo[i_current_node].bIsUpToDate = true;

				i_current_node = iFindLowestEstTotal();
			}

			// Failed to find path in a timely fashion.
			bHasValidPath = false;
			dout << i_count << " tries!  FAIL\n";
			return;


		};


		//*********************************************************************************
		//
		int CAStarAIGraph::iFindLowestEstTotal
		(
		)
		//
		//	Gets the index of the node with the lowest total estimated distance.
		//
		//	Notes:
		//		Ignores -1 estimate total, as those nodes have not yet been reached.
		//		Returns -1 if none found.
		//
		//******************************
		{
			int i_best = - 1;

			// Pick a large number.
			DISTANCE d_best = DISTANCE(LARGE);
						
			for (int i = aasniInfo.uLen - 1; i >= 0; i--)
			{
				// Make sure the node needs expanding before considering it.
				if (!aasniInfo[i].bIsUpToDate)
				{
					DISTANCE d = dEstTotal(i);
	
					// Make sure we've been initialized.
					// Use dNEGATIVE / 2 to avoid floating point errors near zero.
					if (d >= dNEGATIVE / 2 && d < d_best)
					{
						d_best = d;
						i_best = i;
					}
				}
			}
			return i_best;
		}


		//*********************************************************************************
		//
		void CAStarAIGraph::Init
		(
			int i_start,
			int i_stop
		)
		//
		//	Resets the graph and prepares for a search.
		//
		//	Notes:
		//
		//******************************
		{
//			Assert(g->bIsValidIndex(i_start));
//			Assert(g->bIsValidIndex(i_stop));

			bHasValidPath = false;
			iStartNode  = i_start;
			iStopNode	= i_stop;
			dTooFar		= LARGE;

			// Init all node info!

			// Create new info.
			CAStarNodeInfo<DISTANCE> asni;
			#define dNEGATIVE DISTANCE(-1000)
			asni.dSoFar			= dNEGATIVE;
			asni.dEstRemaining	= dNEGATIVE;
			asni.dEstTotal		= asni.dEstRemaining;
			asni.iNodeBefore	= -1;
			asni.bIsUpToDate	= true;
//			asni.dEstRemaining	= g[i]->dEstimatedDistanceTo(g[stop]);

			// Create new info.
			for (int i = 0; i < aasniInfo.uLen; ++i)
			{
				aasniInfo[i] = asni;
			}
		}


#undef DISTANCE
