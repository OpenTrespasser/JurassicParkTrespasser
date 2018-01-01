/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Test class for A* search class.
 *		
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/AI/AStarTest.hpp                                                 $
 * 
 * 1     2/05/97 8:08p Agrant
 * A test class for A*
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_AI_ASTARTEST_HPP
#define HEADER_TEST_AI_ASTARTEST_HPP

#include "Game/AI/AStar.hpp"

class CWDbQueryTestWall;

//*********************************************************************************************
//
class CAStarTest : public CAStar<CVecNode3<TReal>, TReal>
//
//	Prefix: ast
//
//	A class that performs an A* search through a graph with nodes in 3-space.
//
//
//	Notes:
//		The test A* class uses walls to determine connectivity.
//
//*********************************************************************************************
{
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
	CAStarTest(CGraph<CVecNode3<TReal>, TReal>* g, int i_start, int i_stop) :
 		CAStar<CVecNode3<TReal>, TReal>(g, i_start, i_stop)
	{
		pqtwWalls = 0;
	}

	//*****************************************************************************************
	//
	//	Member functions.
	//
	
	//*****************************************************************************************
	//
	//	Overrides.
	//
		//*********************************************************************************
		//
		void Search
		(
		);
		//
		//	Performs a search.
		//
		//	Notes:
		//
		//******************************

		//*********************************************************************************
		//
		virtual bool bIsSuccessor
		(
			int i_parent,
			int i_successor_maybe
		);
		//
		//	Returns true if the second arg is a successor of the first.
		//
		//	Notes:
		//
		//******************************

};





//#ifndef HEADER_TEST_AI_ASTARTEST_HPP
#endif
