/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 *	Implementation of AStarTest.hpp.
 *
 *	Bugs:
 *
 *	To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/AStarTest.cpp                                                $
 * 
 * 4     5/09/98 11:17a Agrant
 * #include commander
 * 
 * 3     3/15/97 5:05p Agrant
 * Updated for spatial partition changes
 * 
 * 2     2/11/97 7:11p Agrant
 * Another pathfinding improvement.
 * 
 * 1     2/05/97 8:08p Agrant
 * A test class for A*
 * 
 **********************************************************************************************/

#include "common.hpp"

//#include "Lib/EntityDBase/Instance.hpp"
//#include "Lib/Renderer/GeomTypes.hpp"
#include "AStarTest.hpp"

//#include "Lib/Transform/Vector.hpp"
#include "QueryTest.hpp"

//#include "Game/AI/AIGraph.hpp"
//#include "AITest.hpp"
//#include "TestTree.hpp"

//#include "Lib/GeomDBase/LineSegment.hpp"


//**********************************************************************************************
//
//	Class CAStarTest.
//

	//*********************************************************************************
	bool CAStarTest::bIsSuccessor
	(
		int i_parent,
		int i_successor
	)
	{
		return false;
		/*
		if (i_parent == i_successor)
			return false;

		Assert(pqtwWalls);

		CWDbQueryTestWall& qtw = *pqtwWalls;

		CLineSegment2<> seg(pGraph->nNode(i_parent), pGraph->nNode(i_successor));

		// for each wall, make sure that it does not intersect the graph edge.
		foreach(qtw)
		{
			if (qtw.tGet()->bIntersects(seg))
				return false;
		}

		return true;
		*/
	}

	//*********************************************************************************
	void CAStarTest::Search
	(
	)
	{
		/*
		// Update wall list.
//		CWDbQueryTestWall qtw(bvsBigSphere);
		CWDbQueryTestWall qtw;
		pqtwWalls = &qtw;

		((CSpatialGraph3*)pGraph)->pqtwWalls = &qtw;

		// Call parent search algorithm.
		CAStar<CVecNode3<>, TReal>::Search();

		// Free wall list.
		pqtwWalls = 0;
		((CSpatialGraph3*)pGraph)->pqtwWalls = 0;
*/
	}