/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CPathAStar class for AI library.
 *		
 *
 * Bugs:
 *
 * To do:
 *		Better new node selection, which could include:
 *			Influences between start and stop nodes
 *			Influences near stop
 *			Influences whose silhouettes block important connections 
 *			Influences which have never been added
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/PathAStar.hpp                                                 $
 * 
 * 10    9/13/98 8:03p Agrant
 * allow pathfinding to cut short if within a certain distance of the stop node
 * 
 * 9     5/21/98 11:45a Agrant
 * .cpp file implementation of AStar for smaller compile times
 * 
 * 8     9/09/97 9:06p Agrant
 * Better node removal
 * 
 * 7     7/31/97 4:42p Agrant
 * Better handling of pathfinding failure
 * 
 * 6     7/14/97 12:55a Agrant
 * Influences can calculate their own node suitability now.
 * 
 * 5     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 4     2/16/97 5:37p Agrant
 * Even better pathfinding.
 * 
 * 3     2/12/97 7:32p Agrant
 * Major pathfinding revision complete- animals construct a set of 
 * nodes which they use for pathfinding, and update the set interactively
 * based on the physical geometry of the objects around them.
 * 
 * 2     2/11/97 7:11p Agrant
 * Another pathfinding improvement.
 * 
 * 1     2/09/97 8:16p Agrant
 * initial rev
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_PATHASTAR_HPP
#define HEADER_GAME_AI_PATHASTAR_HPP

#include "Pathfinder.hpp"
#include "AStar.hpp"
#include "AIGraph.hpp"

//*********************************************************************************************
//
class CPathAStar : public CPathfinder
//
//	Prefix: pastar
//
//	A pathfinding class that performs an A* search on a graph.
//
//	Notes:
//		The class also adds and removes nodes to graph as part of its natural process.
//
//*********************************************************************************************
{
//
//  Variable declarations
//
public:

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//
	
		CPathAStar
		(
			CBrain*	pbr
		) : CPathfinder(pbr)
		{}



	//*****************************************************************************************
	//
	//	Overrides.
	//

		//*********************************************************************************
		//
		virtual bool bFindPath
		(
			CPath*				ppath,
			CRating				rt_solidity, // The max solidity through which the dino will go
			TReal				r_close_enough	// Accept a path bringing you this close to destination
		);
		//
		//	Notes:
		//		Assumes that the influence list is up to date, including target distaces,
		//		and that the subject viewing the influences is at the origin of the path.
		//
		//	Returns:
		//		true is successful, else false.
		//
		//******************************


		//*********************************************************************************
		//
		virtual bool bFindDirectionPath
		(
			CVector2<>			v2_location,	// Starting point.
			CVector2<>			v2_direction,	// Direction of travel.
			CPath*				ppath,			// The path to be filled by the pathfinder.
			CRating				rt_solidity // The max solidity through which the dino will go
		);
		//
		//	Fills "ppath" with a path leading in the general direction "v2_direction".
		//
		//	Returns:
		//		true if successful, else false. The definition of successful varies with
		//		the type of pathfinder.  An "avoidance" pathfinder may only guarantee 
		//		an obstacle-free path for the first segment of the path.
		//
		//******************************

		//*********************************************************************************
		//
		virtual bool bMaybeAddNodes
		(
		);
		//
		//	Adds some nodes to the pathfinding graph if appropriate.
		//
		//	Returns:
		//		true if graph nodes have been changed, else false. 
		//
		//	Notes:
		//		Modifies the brain and the pathfinding graph in the worldview.
		//
		//******************************

		//*********************************************************************************
		//
		virtual bool bMaybeRemoveNodes
		(
		);
		//
		//	Removes some nodes from the pathfinding graph if appropriate.
		//
		//	Returns:
		//		true if graph nodes have been changed, else false. 
		//
		//	Notes:
		//		Modifies the brain and the pathfinding graph in the worldview.
		//
		//******************************

		//*********************************************************************************
		//
		void Remove
		(
			int i_node_index
		);
		//
		//	Removes the node from the graph.
		//
		//******************************


	//*****************************************************************************************
	//
	//	Static Member functions.
	//

		//*********************************************************************************
		//
//		static TReal rCalculateNodeSuitability
//		(
//			const CInfluence* pinf	// The influence to decide.
//		);
		//
		//	Gets a suitability of the influence for creating nodes based on it.
		//
		//	Returns:
		//		A suitability, higher is more suitable.
		//
		//	Notes:
		//		The suitability returned by this function ought to be saved, as it
		//		will not change over the life of the influence.
		//
		//******************************

		//*********************************************************************************
		//
//		static TReal rRateNodeSuitability
//		(
//			const CInfluence* pinf
//		);
		//
		//	Gets a suitability of the influence for creating nodes based on it.
		//
		//	Returns:
		//		A suitability, higher is more suitable.
		//
		//	Notes:
		//		The suitability returned by this function varies over the life of
		//		the influence, and takes into account the rating stored in the influence
		//		itself that was previous determined by a call to
		//			static TReal rCalculateNodeSuitability
		//
		//******************************

};


//*********************************************************************************************
//
class CAStarAIGraph : public CAStar< CAIGraphNode, TReal >
//
//	Prefix: astar
//
//	A pathfinding class that performs an A* search on a CAIGraph.
//
//	Notes:
//		The class also adds and removes nodes to graph as part of its natural process.
//
//*********************************************************************************************
{
//
//  Variable declarations
//
public:
	TReal rCloseEnough;		// If within this distance of stop node, that's good enough.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//
	
		CAStarAIGraph
		(
			CGraph< CAIGraphNode, TReal> *g
		) : CAStar< CAIGraphNode, TReal >(g,0,1)
		{
			rCloseEnough = 0.0f;
		}



	//*****************************************************************************************
	//
	//	Member functions.
	//

		//*********************************************************************************
		//
		void GetCPath
		(
			CPath* pp,	// The path to fill.
			int i_index	// The node at the end of the path
		);
		//
		//	Gets the final search path.
		//
		//	Notes:
		//		Also flags nodes as having been used in a path.
		//
		//******************************


		// 
		//  Overrides
		//

		//*********************************************************************************
		void Search();

		//*********************************************************************************
		int iFindLowestEstTotal();
		
		//*********************************************************************************
		void Init(int i_start, int i_stop);

};


//#ifndef HEADER_GAME_AI_PATHASTAR_HPP
#endif
