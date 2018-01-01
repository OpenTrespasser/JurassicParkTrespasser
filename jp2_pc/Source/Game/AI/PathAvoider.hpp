/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CPathAvoider class for AI library.
 *		
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/PathAvoider.hpp                                               $
 * 
 * 3     5/19/98 9:16p Agrant
 * pathfailure and path obstacle and pathavoider all gone
 * 
 * 2     1/28/97 6:05p Agrant
 * Intermediate pathfinding revision check in
 * 
 * 1     11/01/96 11:37a Agrant
 * initial revision
 * 
 **********************************************************************************************/

#if 0
#ifndef HEADER_GAME_AI_PATHAVOIDER_HPP
#define HEADER_GAME_AI_PATHAVOIDER_HPP

#include "Pathfinder.hpp"

//*********************************************************************************************
//
class CPathAvoider : public CPathfinder
//
//	Prefix: avoid
//
//	A pathfinding class that is fast but only looks at objects near the beginning of the path.
//
//	Notes:
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
	
		CPathAvoider
		(
			CBrain*	pbr
		) : CPathfinder(pbr)
		{}


	//*****************************************************************************************
	//
	//	Member functions.
	//
		//*********************************************************************************
		//
		CInfluence* pobsGetMostImportantObstacle
		(
			CPath*				ppath
		);
		//
		//	Returns:
		//		The obstacle most immediately relevent to the path.
		//
		//	Notes:
		//		Assumes that the influence list is up to date, including target distaces,
		//		and that the subject viewing the influences is at the origin of the path.
		//
		//		The most important obstacle is the closest obstacle that intersects the path.
		//
		//******************************

	//*****************************************************************************************
	//
	//	Overrides.
	//



		//*********************************************************************************
		//
		virtual bool FindPath
		(
			CPath*				ppath,
			CRating				rt_solidity // The max solidity through which the dino will go
		);
		//
		//	Notes:
		//		Only pathfinds for the first leg of the path.  Very good at avoiding lots
		//		of independent, free-standing obstacles.
		//
		//		Assumes that the influence list is up to date, including target distaces,
		//		and that the subject viewing the influences is at the origin of the path.
		//
		//	Returns:
		//		true
		//
		//******************************


		//*********************************************************************************
		//
		virtual bool FindDirectionPath
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

#if 0
		//*********************************************************************************
		//
		virtual void Reset
		(
		);
		//
		//	Clears the pathfinder's obstacle list.
		//
		//******************************

		//*********************************************************************************
		//
		virtual void SetObstacleList
		(
			CInfluenceList *infl
		);
		//
		//	Sets the pathfinder's object list, converting data as needed.
		//
		//******************************
#endif

		//*********************************************************************************
		//
		virtual bool bFix
		(
			CPath* ppath,	 			// The path to repair.
			CFailureObstacle* pfo// The way in which the path failed.
		);
		//
		//	Repairs the path.
		//
		//******************************
};

//#ifndef HEADER_GAME_AI_PATHAVOIDER_HPP
#endif
#endif