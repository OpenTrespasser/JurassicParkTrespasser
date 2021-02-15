/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Node Graph classes for AI library.
 *		
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/AIGraph.hpp                                                   $
 * 
 * 23    9/23/98 10:31p Agrant
 * Use 2d distances for pathfinding
 * 
 * 22    8/23/98 3:51a Agrant
 * Set HistoryRandom
 * 
 * 21    8/11/98 2:22p Agrant
 * many new asserts
 * 
 * 20    6/11/98 2:58p Agrant
 * Remove STL vector from CPath
 * 
 * 19    6/08/98 9:46p Agrant
 * Handle influence motion gracefully by moving nodes with the influence
 * 
 * 18    5/10/98 6:26p Agrant
 * AI's now can ignore selected tangible objects.
 * misc. changes
 * 
 * 17    2/23/98 5:07p Agrant
 * AI save/load
 * 
 * 16    9/11/97 1:52p Agrant
 * AIGraph now uses flags to determine different drawing styles
 * Removing an influence now removes any nodes associated with it.
 * 
 * 15    9/09/97 9:06p Agrant
 * removed old influence nodes
 * 
 * 14    8/12/97 6:34p Agrant
 * AddNode function to support placing nodes on terrain automatically.
 * 
 * 13    7/31/97 4:46p Agrant
 * Object pathfinding
 * 
 * 12    7/27/97 2:24p Agrant
 * Rudimentary pathfinding over objects
 * 
 * 11    7/14/97 1:00a Agrant
 * Supports NodeSource and Unknown node types.
 * 
 * 10    5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 9     5/06/97 7:54p Agrant
 * delete those pointers
 * 
 * 8     3/28/97 2:00p Agrant
 * Brain now points to graph, mentalstate, and worldview
 * Pathfinding more efficient
 * 
 * 7     3/26/97 5:35p Agrant
 * Switched graph over to sparse array instead of vector.
 * Graph now caches distance data to speed up A*
 * 
 * 6     2/16/97 5:37p Agrant
 * Even better pathfinding.
 * 
 * 5     2/12/97 7:32p Agrant
 * Major pathfinding revision complete- animals construct a set of 
 * nodes which they use for pathfinding, and update the set interactively
 * based on the physical geometry of the objects around them.
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

#ifndef HEADER_GAME_AI_AIGRAPH_HPP
#define HEADER_GAME_AI_AIGRAPH_HPP

#include "Graph.hpp"
#include "Rating.hpp"
#include "NodeHistory.hpp"

#include "Lib/Sys/Timer.hpp"

class CAnimal;
class CInstance;
class CWDbQueryTestWall;
class CMessageMove;

//*********************************************************************************************
//
class CSpatialGraph3 : public CGraph<CVecNode3<TReal>, TReal >
//
//	Prefix: sg3
//
//	A class that describes a set of points on 3-space.
//
//	Notes:
//
//*********************************************************************************************
{
public:
//
//  Variable declarations
//

	CWDbQueryTestWall* pqtwWalls;  // A set of walls used to determine graph connectivity.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//
	
	CSpatialGraph3() : CGraph<CVecNode3<TReal>, TReal >(3, 50)
	{
		pqtwWalls = 0;
	}



	//*****************************************************************************************
	//
	//	Member functions.
	//
	
		//*********************************************************************************
		//
		void Draw
		(
			bool b_draw_edges		// true if edges are to be drawn.
		);
		//
		//	Draws the graph.
		//
		//	Notes:
		//
		//******************************

		//*********************************************************************************
		//
		void HighlightNode
		(
			int i_index   // The index of the node to highlight.
		);
		//
		//	Highlights the specified node with the current line color.
		//
		//	Notes:
		//		If index is out of range, does nothing.
		//
		//******************************

		//*********************************************************************************
		//
		int iNearestNode
		(
			CVector3<> v3
		);
		//
		//	Returns the index of the node nearest the specified point.
		//
		//******************************

		//*********************************************************************************
		//
		void DrawPath
		(
			const CGraphPath& gp	// The path to draw.
		);
		//
		//	Draws the path.
		//
		//******************************

		//*********************************************************************************
		//
		virtual bool bIsSuccessor
		(
			int i_parent,
			int i_successor_maybe
		) override;
		//
		//	Returns true if the second arg is a successor of the first.
		//
		//	Notes:
		//
		//******************************
		

};


//typedef CVecNode3<TReal> TAIGraphNode;


//*********************************************************************************************
//
class CAIGraphNode
//
//	Prefix: aign
//
//	A class that describes a point used for pathfinding.
//
//	Notes:
//
//*********************************************************************************************
{
public:
	
//
//  Variable declarations
//

	CVector3<>	v3Pos;		// The location of the node.
	TSec		sLastUsed;	// The time the node was last used in a solution path.
	CInstance*	pinsSurface;// The object upon which the node rests, or 0 if terrain.

protected:
	UNodeHistory	unhData;	// The memory allocated for the history.



//
//  Constructors.
//
public:

	CAIGraphNode()
	{
		SetHistoryInvalid();
	};

	CAIGraphNode(const CVector3<>& v3_pos, TSec s_now, CInstance* pins_surface = 0);


//
//  Member function definitions
//

		//*********************************************************************************
		//
		CNodeHistory* pnhGetNodeHistory
		(
		) const
		//
		//	Returns the distance between the points.
		//
		//******************************
		{
			return (CNodeHistory*)&unhData;
		}


		//*********************************************************************************
		//
		TReal dDistanceTo
		(
			const CAIGraphNode& aign		// The node to which we are travelling.
		) const
		//
		//	Returns the distance between the points.
		//
		//******************************
		{
			CVector3<> v3_diff = v3Pos - aign.v3Pos;
			v3_diff.tZ = 0;
			
			TReal r = v3_diff.tLen();

			// A jump is more complicated than a run, so it seems longer.
			if (pinsSurface != aign.pinsSurface)
				r += 1.0f;

			if (r == 0)
				return dInfinite();
			else
				return r;
		}

		//*********************************************************************************
		//
		TReal dEstimatedDistanceTo
		(
			const CAIGraphNode& aign		// The node to which we are travelling.
		) const
		//
		//	Returns the estimated distance to the goal.
		//
		//******************************
		{
			return dDistanceTo(aign);
		}

		
		//*********************************************************************************
		//
		bool bIsValid
		(
		) const
		//
		//	Returns true if the node is a valid node.
		//
		//******************************
		{
			return !pnhGetNodeHistory()->pCastNHInvalid();
		}

		//*********************************************************************************
		//
		bool bIsStart
		(
		) const
		//
		//	Returns true if the node is a start node.
		//
		//******************************
		{
			return 0 != pnhGetNodeHistory()->pCastNHStart();
		}

		//*********************************************************************************
		//
		bool bIsStop
		(
		) const
		//
		//	Returns true if the node is a stop node.
		//
		//******************************
		{
			return 0 != pnhGetNodeHistory()->pCastNHStop();
		}

		//*********************************************************************************
		//
		void Invalidate
		(
		)
		//
		//	Invalidates the node.
		//
		//******************************
		{
			SetHistoryInvalid();			
		}

		//*********************************************************************************
		//
		static bool bIsUnknown
		(
			TReal r
		) 
		//
		//	true if r represents an unknown distance value.
		//
		//******************************
		{
			return r < 0;
		}

		//*********************************************************************************
		//
		static bool bIsInfinite
		(
			TReal r
		) 
		//
		//	true if r represents an infinite distance value.
		//
		//******************************
		{
			return r == 0;
		}

		//*********************************************************************************
		//
		inline static TReal dUnknown
		(
		) 
		//
		//	An unknown value.
		//
		//******************************
		{
			return -1;
		}

		//*********************************************************************************
		//
		inline static TReal dInfinite
		(
		) 
		//
		//	An infinite value.
		//
		//******************************
		{
			return 0;
		}



	//
	//  History functions.
	//

		//*********************************************************************************
		void SetHistoryStart
		(
		)
		//	Sets the history of the node.
		//******************************
		{
			AlwaysAssert(!bIsValid());

			// Cast to void to indicate to LINT that we mean to ignore the return value.
			(void)  new (&unhData) CNHStart();
		}

		//*********************************************************************************
		void SetHistoryStop
		(
		)
		//	Sets the history of the node.
		//******************************
		{
			AlwaysAssert(!bIsValid());
			(void) new (&unhData) CNHStop();
		}

		//*********************************************************************************
		void SetHistoryUnknown
		(
		)
		//	Sets the history of the node.
		//******************************
		{
			AlwaysAssert(!bIsValid());
			(void) new (&unhData) CNHUnknown();
		}

		//*********************************************************************************
		void SetHistoryInfluence
		(
			CInfluence*	pinf
		)
		//	Sets the history of the node.
		//******************************
		{
			AlwaysAssert(!bIsValid());
			(void) new (&unhData) CNHInfluence(pinf);
		}

		//*********************************************************************************
		void SetHistoryInfluenceSurface
		(
			CInfluence*	pinf
		)
		//	Sets the history of the node.
		//******************************
		{
			AlwaysAssert(!bIsValid());
			(void) new (&unhData) CNHInfluenceSurface(pinf);
		}

		//*********************************************************************************
		void SetHistoryNodeSource
		(
			CNodeSource*	pns
		)
		//	Sets the history of the node.
		//******************************
		{
			AlwaysAssert(!bIsValid());
			(void) new (&unhData) CNHNodeSource(pns);
		}

		//*********************************************************************************
		void SetHistoryRandom
		(
		)
		//	Sets the history of the node.
		//******************************
		{
			AlwaysAssert(!bIsValid());
			(void) new (&unhData) CNHRandom();
		}

#ifdef TRACK_OLD_NODES
		//*********************************************************************************
		void SetHistoryNodeSourceOld
		(
			CNodeSource*	pns
		)
		//	Sets the history of the node.
		//******************************
		{
			AlwaysAssert(!bIsValid());
			(void) new (&unhData) CNHNodeSourceOld(pns);
		}

		//*********************************************************************************
		void SetHistoryInfluenceOld
		(
			CInfluence*	pinf
		)
		//	Sets the history of the node.
		//******************************
		{
			AlwaysAssert(!bIsValid());
			(void) new (&unhData) CNHInfluenceOld(pinf);
		}
#endif  // TRACK_OLD_NODES

		//*********************************************************************************
		void SetHistoryInvalid
		(
		)
		//	Sets the history of the node.
		//		Only used by the sparse array containing the node.  Use pGraph->RemoveNode() instead!!!
		//******************************
		{
			(void) new (&unhData) CNHInvalid();
		}

};



//*********************************************************************************************
//
class CAIGraph : public CGraph<CAIGraphNode, TReal >
//
//	Prefix: aig
//
//	A class that describes a set of points used for pathfinding.
//
//	Notes:
//		Named diffferently from SpatialGraph3 to allow extra AI code.
//
//*********************************************************************************************
{
//
//  Variable declarations
//
public:
	CAnimal*	paniAnimal;	// The animal using this graph.

	CRating		rtSolidityThreshhold;	// The solidity beyond which an obstacle must be
										// reckoned with.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	CAIGraph(CAnimal* pbr, int i_max_nodes);
	~CAIGraph()
	{
		paniAnimal = 0;
	};

	//*****************************************************************************************
	//
	//	Member functions.
	//
	
		//*********************************************************************************
		//
		void RemoveNodes
		(
			int i_first,
			int i_last
		)
		//
		//	Deletes nodes "i_first" through "i_last", inclusive.
		//
		//	Notes:
		//
		//******************************
		{
			Assert(bIsValidIndex(i_first) && bIsValidIndex(i_last));
			Assert(i_first <= i_last);

//			erase(begin() + i_first, begin() + i_last + 1);
			for (int i = i_first; i <= i_last; ++i)
			{
				RemoveNode(i);
			}

			// Shorten the array if appropriate.
			if (aNodes.uLen <= i_last)   //lint !e574
				aNodes.uLen = i_first;

		}

		//*********************************************************************************
		//
		void Reset
		(
		);
		//
		//	Clear graph to its original state.
		//
		//******************************

		//*********************************************************************************
		//
		void ClearReferences
		(
			const CInstance* pins
		);
		//
		//	Makes nodes referencing pins safe (unknown)
		//
		//******************************

		//*********************************************************************************
		//
		void ClearReferences
		(
			const CNodeSource* pns
		);
		//
		//	Makes nodes referencing pns safe (unknown)
		//
		//******************************

		//*********************************************************************************
		//
//		void RemoveNode
//		(
//			int i_index
//		)
		//
		//	Deletes node i_index
		//
		//	Notes:
		//
		//******************************
//		{
//			aNodes.Invalidate(i_index);
//		}

		//*********************************************************************************
		//
		void InfluenceMoved
		(
			CInfluence *pinf,
			const CMessageMove& msgmv
		);
		//
		//	Updates the graph as appropriate for an influence moving.
		//
		//	Notes:
		//		Modifies pinf flags associated with the graph.
		//
		//******************************

		//*********************************************************************************
		//
		void SetStart
		(
			CVector3<> v3		// New start location, replacing old one.
		);
		//
		//	Sets the start location.
		//
		//	Notes:
		//
		//******************************

		//*********************************************************************************
		//
		void SetStop
		(
			CVector3<> v3		// New stop location, replacing old one.
		);
		//
		//	Sets the stop location.
		//
		//	Notes:
		//
		//******************************

		//*********************************************************************************
		//
		void SetSolidityThreshhold
		(
			CRating rt		// New solidity theshhold.
		)
		//
		//	Sets solidity threshhold.
		//
		//	Notes:
		//
		//******************************
		{
			rtSolidityThreshhold = rt;
		}

		//*********************************************************************************
		//
		CRating rtGetSolidityThreshhold
		(
		)
		//
		//	Gets solidity threshhold.
		//
		//	Notes:
		//
		//******************************
		{
			return rtSolidityThreshhold;
		}



		//
	//	Overrides.
	//

		//*********************************************************************************
		virtual void RemoveNode(int i_node_index) override;

		//*********************************************************************************
		//
		virtual bool bIsSuccessor
		(
			int i_parent,
			int i_successor_maybe
		) override;
		//
		//	Returns true if the second arg is a successor of the first.
		//
		//	Notes:
		//
		//******************************

		//*********************************************************************************
		//
		virtual void AddNode
		(
			CAIGraphNode& node
		) override;
		//
		//	Adds a node to the graph.
		//
		//******************************

		//*********************************************************************************
		//
		virtual bool bCanSurfaceTransfer

		(
			int i_parent,
			int i_successor_maybe
		);
		//
		//	Returns true if the second arg is a successor of the first.
		//
		//	Notes:
		//		Assumes that the nodes are on different surfaces, thus requiring a jump or somesuch.
		//	
		//
		//******************************

	//
	//  Debug functions.
	//

		//*********************************************************************************
		//
		void Draw
		(
			bool b_draw_edges,				// true if edges are to be drawn.
			bool b_draw_jump_edges = false	// true if surface transfers are to be drawn.
		);
		//
		//	Draws the graph.
		//
		//	Notes:
		//		jump edges flag only used when draw_edges flag is false
		//
		//******************************

		//*********************************************************************************
		//
		void Draw
		(
		);
		//
		//	Draws the graph.
		//
		//	Notes:
		//		Uses ai system debug flags to determine whether or not to draw edges.
		//
		//******************************

		//*********************************************************************************
		//
		void HighlightNode
		(
			int i_index   // The index of the node to highlight.
		);
		//
		//	Highlights the specified node with the current line color.
		//
		//	Notes:
		//		If index is out of range, does nothing.
		//
		//******************************

		//*********************************************************************************
		//
		void DrawPath
		(
			const CGraphPath& gp	// The path to draw.
		);
		//
		//	Draws the path.
		//
		//******************************


		//*****************************************************************************************
		virtual char * pcSave(char *  pc_buffer) const;

		//*****************************************************************************************
		virtual const char * pcLoad(const char *  pc_buffer);

};

		
//#ifndef HEADER_GAME_AI_AIGRAPH_HPP
#endif
