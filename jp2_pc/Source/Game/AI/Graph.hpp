/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Node Graph templates for AI library.
 *		
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/Graph.hpp                                                     $
 * 
 * 15    6/23/98 1:56p Agrant
 * Bogus assert check removed
 * 
 * 14    6/13/98 8:02p Agrant
 * Always assert that we have room for the new nodes
 * 
 * 13    6/11/98 2:58p Agrant
 * Remove STL vector from CPath
 * 
 * 12    5/10/98 6:26p Agrant
 * AI's now can ignore selected tangible objects.
 * misc. changes
 * 
 * 11    9/11/97 1:53p Agrant
 * Added asserts
 * 
 * 10    9/09/97 9:03p Agrant
 * RemoveNode function for general graphs
 * 
 * 9     8/12/97 6:35p Agrant
 * Virtualized AddNode()
 * 
 * 8     7/31/97 4:41p Agrant
 * Clearer variable names
 * 
 * 7     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 6     97-04-24 16:06 Speter
 * Fixed silly syntax error that messed up VC5.
 * 
 * 5     3/26/97 5:29p Agrant
 * Switched from STL vector to CSparseArray to implement graphs.
 * 
 * 4     2/11/97 7:11p Agrant
 * Another pathfinding improvement.
 * 
 * 3     2/10/97 4:15p Agrant
 * Intermediate pathfinding check-in
 * 
 * 2     2/09/97 8:20p Agrant
 * Fix to CVecNode3<>
 * random otherness
 * 
 * 1     2/05/97 1:07p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_GRAPH_HPP
#define HEADER_GAME_AI_GRAPH_HPP

//#include <vector.h>

#include "Lib\Std\SparseArray.hpp"
#include "Lib\Std\Array2.hpp"
#include "Lib\Transform\Vector.hpp"

//lint -save -e1509   // vector<int> destructor not virtual.
//*********************************************************************************************
//
class CGraphPath : public CMSArray< int, 10 >
//
//	Prefix: gp
//
//	A class that describes a path through a graph (by indexing the nodes)
//
//	Notes:
//
//*********************************************************************************************
{

//
//  Variable declarations
//

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//
//	CGraphPath()
//	{
//	}


	//*****************************************************************************************
	//
	//	Member functions.
	//

		//*********************************************************************************
		//
		void Clear
		(
		)
		//
		//	Clears the path of all nodes.
		//
		//******************************
		{
			uLen = 0;
			//erase(begin(), end());
		}

		//*********************************************************************************
		//
		void push_back
		(
			int i
		)
		//
		//	Adds an element to the end of the path.
		//
		//******************************
		{
			*this << i;
		}
};
//lint -restore


//*********************************************************************************************
//
template <class NODE, class DISTANCE>
class CGraph //: protected vector< NODE >
//
//	Prefix: graph
//
//	A class that describes a graph as a collection of nodes.
//
//	Notes:
//		A simple CGraph<> is a node graph.
//		There are some requirements on a NODE.
//			NODE->dDistance(const& NODE) const must return DISTANCE
//
//		NODE must also define as a static function the following:
//		bool bUnknownDistance(DISTANCE d) which returns true for unknown
//			distance values, for distances that have not been calculated.
//		bInfiniteDistance(DISTANCE d) which returns true for infinite
//			distance values or distances between unconnected nodes.
//		DISTANCE dUnknown(DISTANCE& d) which returns an
//			"unknown" distance rating.
//		DISTANCE dInfinite(DISTANCE& d) which returns an
//			"infinite" distance rating.
//
//*********************************************************************************************
{
public:
//
//  Variable declarations
//

	CSparseArray<NODE>	aNodes;			// An array of nodes.

	CAArray2<DISTANCE>	a2dDistances;	// A 2d array of distances.  
										// a2dDistances[x][y] gives distance from x to y.

	int			iDivideInit;			// How many times do we have to init to fully reset the distance array?
	int			iCurrentDivideInit;		// Which number in our init cycle are we on?
										// (reset every Nth piece of our connectivity dbase)

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//
	CGraph(int i_divide_init, int i_max_nodes) 
		: aNodes(i_max_nodes), a2dDistances(i_max_nodes,i_max_nodes)
	{
		iDivideInit = i_divide_init;
		iCurrentDivideInit = 0;
	}

	virtual ~CGraph(){}

	//*****************************************************************************************
	//
	//	Member functions.
	//
	
		//*********************************************************************************
		//
		virtual void AddNode
		(
			NODE& node
		)
		//
		//	Adds a node to the graph.
		//
		//******************************
		{
			//push_back(node);
			AlwaysAssert(aNodes.iNumElements < aNodes.uMax);
			NODE *n = new(aNodes) NODE(node);

			int i_index = aNodes.iIndexOf(n);

			for (int i = aNodes.uLen-1; i >= 0; i--)
			{
				DISTANCE d_unknown = NODE::dUnknown();

				// Make all node info unknown.
				a2dDistances(i_index,i) = d_unknown;
				a2dDistances(i,i_index) = d_unknown;
			}
//			erase(begin() + i_index);
		}

		//*********************************************************************************
		//
		virtual void RemoveNode
		(
			int i_node_index
		)
		//
		//	Adds a node to the graph.
		//
		//******************************
		{
#if VER_DEBUG
			int i_num = iNumNodes();
#endif
			aNodes.Invalidate(i_node_index);
#if VER_DEBUG
			Assert(i_num - 1 == iNumNodes());
#endif
		}

		//*********************************************************************************
		//
		const NODE& nNode
		(
			int i_index
		)
		//
		//	Gets the specified node.
		//
		//******************************
		{
//			Assert(size() > i_index && i_index >= 0);

//			return (*this)[i_index];
			return aNodes[i_index];
		}

		//*********************************************************************************
		//
		NODE* pnNode
		(
			int i_index
		)
		//
		//	Gets the specified node.
		//
		//******************************
		{
//			Assert(size() > i_index && i_index >= 0);

//			return &(*this)[i_index];
			return &aNodes[i_index];
		}

		//*********************************************************************************
		//
		virtual DISTANCE dDistanceTo
		(
			int i_index1,
			int i_index2
		)
		//
		//	Gets the distance between the specified nodes.
		//
		//******************************
		{
			DISTANCE d = a2dDistances(i_index1,i_index2);

			if (!NODE::bIsUnknown(d))
				return d;

			d = nNode(i_index1).dDistanceTo(nNode(i_index2));
			a2dDistances(i_index1,i_index2) = d;
			return d;
		}

		//*********************************************************************************
		//
		bool bIsValidIndex
		(
			int i_index
		)
		//
		//	Returns true if the index indexes a valid node.
		//
		//******************************
		{
			return i_index >= 0 && i_index < aNodes.uLen;  //lint !e574 
		}

		//*********************************************************************************
		//
		int iNumNodes
		(
		) const
		//
		//	Returns the number of nodes in the graph.
		//
		//******************************
		{
			// Not implemented in sparse array version
//			return size();
			return aNodes.iNumElements;
		}

		//*********************************************************************************
		//
		int iMaxNodeIndex
		(
		) const
		//
		//	Returns 1+highest node index in the graph.
		//
		//******************************
		{
			// Not implemented in sparse array version
//			return size();
			return aNodes.uLen;
		}


		//*********************************************************************************
		//
		virtual bool bIsSuccessor
		(
			int i_parent,
			int i_successor_maybe
		)
		//
		//	Returns true if the second node is a successor of the first.
		//
		//	Notes:
		//
		//******************************
		{
			return i_parent != i_successor_maybe;
		}


		//*********************************************************************************
		//
		void ClearDistanceCache
		(
		)
		//
		//	Sets all distance and connectivity to UNKNOWN
		//
		//	Notes:
		//
		//******************************
		{
			// Only clears every Nth piece of connectivity info!

			DISTANCE d_unknown = NODE::dUnknown();

			int i_counter = iCurrentDivideInit;

			for (int i = aNodes.uLen - 1; i >= 0; i--)
				for (int i2 = aNodes.uLen - 1; i2 >= 0; i2--)
				{
					if (i_counter % iDivideInit == 0)
					{
						a2dDistances(i,i2) = d_unknown;				
					}

					i_counter++;
				}

			iCurrentDivideInit++;
			iCurrentDivideInit = iCurrentDivideInit % iDivideInit;
		}


};


//*********************************************************************************************
//
template <class TREAL = TReal>
class CVecNode3 : public CVector3<TREAL>
//
//	Prefix: vn3
//
//	A sample node type.
//
//	Notes:
//		CVecNode3<> represents nodes in 3d space.
//
//*********************************************************************************************
{


//
//  Variable declarations
//

	bool bValid;		// True when valid, else false.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	CVecNode3() : bValid(false)
	{}


	CVecNode3(TREAL r_x, TREAL r_y = 0, TREAL r_z = 0) :
	  CVector3<TREAL>(r_x, r_y, r_z), bValid(true)
	{
//		tX = r_x;
//		tY = r_y;
//		tZ = r_z;
	}


	CVecNode3(CVector3<TREAL>& v3) : CVector3<>(v3.tX, v3.tY, v3.tZ), bValid(true)
	{
	}

	//*****************************************************************************************
	//
	//	Member functions.
	//
	
		//*********************************************************************************
		//
		TREAL dDistanceTo
		(
			const CVecNode3<TREAL>& vn3		// The node to which we are travelling.
		) const
		//
		//	Returns the distance between the points.
		//
		//******************************
		{
			return ((*this) - vn3).tLen();
		}

		//*********************************************************************************
		//
		TREAL dEstimatedDistanceTo
		(
			const CVecNode3<TREAL>& vn3		// The node to which we are travelling.
		) const
		//
		//	Returns the estimated distance to the goal.
		//
		//******************************
		{
			return dDistanceTo(vn3);
		}

		//*********************************************************************************
		//
		bool bIsValid
		(
		) const
		//
		//	Always valid.
		//
		//******************************
		{
			return bValid;
		}

		//*********************************************************************************
		//
		void Invalidate
		(
		) 
		//
		//	Does nothing.
		//
		//******************************
		{
			bValid = false;
		}

		//*********************************************************************************
		//
		static bool bIsUnknown
		(
			TREAL r
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
			TREAL r
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
		inline static TREAL dUnknown
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
		inline static TREAL dInfinite
		(
		) 
		//
		//	An infinite value.
		//
		//******************************
		{
			return 0;
		}

};


template <class DUMMY>
class GOOF {};
		
//#ifndef HEADER_GAME_AI_GRAPH_HPP
#endif
