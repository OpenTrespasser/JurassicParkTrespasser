/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		A* search class that operates on graphs.
 *		
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/AStar.hpp                                                     $
 * 
 * 13    10/07/98 10:41p Jpaque
 * fixed crash where our path got too long
 * 
 * 12    6/10/98 6:32p Agrant
 * Give up on pathfinding set in data
 * 
 * 11    5/29/98 10:08p Agrant
 * Fixed AStar memory leak
 * 
 * 10    5/16/98 10:00a Agrant
 * Pathfinding fixes for the AI bounding box vs the physics bounding box
 * Major pathfinding fix in general-  how did it ever work???
 * 
 * 9     5/10/98 6:26p Agrant
 * AI's now can ignore selected tangible objects.
 * misc. changes
 * 
 * 8     9/09/97 9:02p Agrant
 * Give up pathfinding when the path is too long.
 * 
 * 7     7/31/97 4:45p Agrant
 * Support for finding best path if successful not found
 * 
 * 6     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 5     3/28/97 2:00p Agrant
 * Brain now points to graph, mentalstate, and worldview
 * Pathfinding more efficient
 * 
 * 4     3/26/97 5:30p Agrant
 * switched over to sparse array to hold graph nodes.
 * 
 * 3     2/10/97 4:15p Agrant
 * Intermediate pathfinding check-in
 * 
 * 2     2/05/97 8:07p Agrant
 * Allows algorithm to fail
 * Allows connectivity to be determined by a child class.
 * 
 * 1     2/05/97 1:07p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_ASTAR_HPP
#define HEADER_GAME_AI_ASTAR_HPP

#include "Graph.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#define LARGE (65536)

//*********************************************************************************************
//
template <class DISTANCE>
class CAStarNodeInfo
//
//	Prefix: asni
//
//	A class that holds important data for each node of a graph.
//
//	Notes:
//
//*********************************************************************************************
{
//
//  Variable declarations
//
public:
	DISTANCE dEstRemaining;		// Estimated distance remaining to stop.
	DISTANCE dSoFar;			// Shortest distance to here from start.
	DISTANCE dEstTotal;			// Estimated total distance from start to stop.

	int		iNodeBefore;		// The node before this one on the best path to this point.
	bool	bIsUpToDate;		// True when the node is current and need not be examined further.
};


//*********************************************************************************************
//
//  OLDNODE is the type of node used in the graph to search through
template <class OLDNODE, class DISTANCE>
class CAStar
//
//	Prefix: as
//
//	A class that performs an A* search through a graph with nodes in 3-space.
//
//	Notes:
//
//*********************************************************************************************
{
//
//  Variable declarations
//
public:
	int iStartNode;		// The first node of the path.
	int iStopNode;		// The last node in the path.

	CAArray< CAStarNodeInfo< DISTANCE > > aasniInfo;   // Info for search.

	CGraph<OLDNODE, DISTANCE>* pGraph;	// The graph to search.

	bool bHasValidPath;					// True when we have a valid solution.

	DISTANCE		dTooFar;			// A distance beyond which we will not pathfind.
	int				iTooHigh;			// The number of successor tests beyond which we give up.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//
	CAStar(CGraph<OLDNODE, DISTANCE>* g, int i_start, int i_stop) : aasniInfo(g->aNodes.uLen)
	{
		pGraph = g;
		

		Assert(g);

		Init(i_start, i_stop);
	}

	virtual ~CAStar(){};


	//*****************************************************************************************
	//
	//	Member functions.
	//
	
		//*********************************************************************************
		//
		void Init
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
			iTooHigh	= 400;

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

		//*********************************************************************************
		//
		void Search
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

			// Use a for loop instead of while(1) for safety.
			for (int i_count = i_num_nodes * i_num_nodes; i_count >= 0; i_count--)
			{

				// We should not fail in finding a low estimate.
				if (i_current_node == -1)
				{
					// Failed.
					bHasValidPath = false;
					return;
				}

				if (i_current_node == iStopNode)
				{
					bHasValidPath = true;
					// We're done!
					return;
				}

				Assert(pGraph->nNode(i_current_node).bIsValid());


				if (aasniInfo[i_current_node].dEstTotal > dTooFar)
				{
					// The path is too long.
					// Failed.
					bHasValidPath = false;
					return;
				}



				bool b_already_tried_stop_node = false;

				int i_successor = iStopNode;
				while(i_successor >= 0)
				{
					if (pGraph->nNode(i_successor).bIsValid() && pGraph->bIsSuccessor(i_current_node, i_successor))
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
			Assert(0);
		}


		//*********************************************************************************
		//
		void GetPathUpToIndex
		(
			CGraphPath *pgp,	// Path to fill.
			int i_index			// The node at the end of the path
		)
		//
		//	Gets the search path from start to i_index.
		//
		//	Notes:
		//
		//******************************
		{
			if (i_index == iStartNode)
			{
				pgp->push_back(i_index);
			}
			else
			{
				Assert(pGraph->bIsValidIndex(i_index));

				GetPathUpToIndex(pgp,aasniInfo[i_index].iNodeBefore);

				if (pgp->size() >= 10)
				{
					// We've filled up the path.
					return;
				}

				pgp->push_back(i_index);
			}
		}


		//*********************************************************************************
		//
		void GetPath
		(
			CGraphPath* pgp				// The path to fill.
		)
		//
		//	Gets the final search path.
		//
		//	Notes:
		//
		//******************************
		{
			if (bHasValidPath)
				GetPathUpToIndex(pgp, iStopNode);
		}

		//*********************************************************************************
		//
		DISTANCE dSoFar
		(
			int i_index
		)
		//
		//	Gets the distance to the node from start.
		//
		//	Notes:
		//
		//******************************
		{
			return aasniInfo[i_index].dSoFar;
		}

		//*********************************************************************************
		//
		DISTANCE dEstRemaining
		(
			int i_index
		)
		//
		//	Gets the estimated distance from i_index to stop.
		//
		//	Notes:
		//
		//******************************
		{
			return aasniInfo[i_index].dEstRemaining;
		}

		//*********************************************************************************
		//
		DISTANCE dEstTotal
		(
			int i_index
		)
		//
		//	Gets the estimated distance from start to stop through i_index.
		//
		//	Notes:
		//
		//******************************
		{
			return aasniInfo[i_index].dEstTotal;
		}

		//*********************************************************************************
		//
		int iFindLowestEstTotal
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
		int iFindBestAttainableNode
		(
		)
		//
		//	Gets the index of the attainable node with the lowest total estimated remaining distance.
		//
		//	Notes:
		//		A node is attainable if a path exists from the start node to it.
		//
		//	Returns:
		//		Index of the best node, or -1 if none found.
		//
		//******************************
		{
			DISTANCE d_smallest_remaining = DISTANCE(LARGE);
			int i_best = -1;

			// Loop through all nodes.
			for (int i = aasniInfo.uLen - 1; i >= 0; i--)
			{
				if (aasniInfo[i].iNodeBefore >= 0)
				{
					// Node is attainable!

					if (aasniInfo[i].dEstRemaining < d_smallest_remaining)
					{
						// Save the info.
						d_smallest_remaining = aasniInfo[i].dEstRemaining;
						i_best = i;
					}
				}
			}

			return i_best;
		}



};









		
//#ifndef HEADER_GAME_AI_AIGRAPH_HPP
#endif
