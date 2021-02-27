/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Path failure modes for AI library.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/PathFailure.hpp                                               $
 * 
 * 2     5/19/98 9:16p Agrant
 * pathfailure and path obstacle and pathavoider all gone
 * 
 * 1     1/28/97 6:44p Agrant
 * pathfinding files
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_PATHFAILURE_HPP
#define HEADER_GAME_AI_PATHFAILURE_HPP
#if 0

class CFailureObstacle;
#include "Pathfinder.hpp"


//*********************************************************************************************
//
class CPathFailure 
//
//	Prefix: pf
//
//	Base class for all pathfinding failure mode descriptions.
//
//*********************************************************************************************
{
//
//  Variable declarations
//
public:
	int iPathSegment;	// The segment of the path that had the problem.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//
	CPathFailure(int i_segment) : iPathSegment(i_segment)
	{};

	
	//*****************************************************************************************
	//
	//	Member functions.
	//

		//*********************************************************************************
		//
		virtual bool bFix
		(
			CPathfinder*	pfind, 	// The pathfinder used to repair the path.
			CPath*			ppath	// The path to repair.
		) = 0;
		//
		//	Repairs the path.
		//
		//******************************

	//
	// Cast functions.
	//

	virtual CFailureObstacle* pfoCast()
	{ return 0; }
};



//*********************************************************************************************
//
class CFailureObstacle : public CPathFailure
//
//	Prefix: fo
//
//	Class for failures due to simple obstacles.
//
//*********************************************************************************************
{
//
//  Variable declarations
//
public:
	const CInfluence* pinfObstacle;	// The obstacle causing the failure.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//
	
	CFailureObstacle(int i_segment, const CInfluence* pinf) :
	  CPathFailure(i_segment), pinfObstacle(pinf)
	  {};



	//*****************************************************************************************
	//
	//	Member functions.
	//
	
	//*****************************************************************************************
	//
	//	Overrrides.
	//

		//*********************************************************************************
		//
		virtual bool bFix
		(
			CPathfinder*	pfind, 	// The pathfinder used to repair the path.
			CPath*			ppath	// The path to repair.
		)
		//
		//	Repairs the path.
		//
		//******************************
		{
			return pfind->bFix(ppath, this);
		}

	//
	// Cast functions.
	//

	virtual CFailureObstacle* pfoCast()
	{ return this; }
};




//#ifndef HEADER_GAME_AI_PATHFAILURE_HPP
#endif 
#endif