/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Pathinder classes for AI library.
 *		
 *
 * Bugs:
 *
 * To do:
 *		Maybe make CPathFailure a crefobj?
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/Pathfinder.hpp                                                $
 * 
 * 11    9/13/98 8:03p Agrant
 * allow pathfinding to cut short if within a certain distance of the stop node
 * 
 * 10    6/11/98 2:58p Agrant
 * Remove STL vector from CPath
 * 
 * 9     5/19/98 9:16p Agrant
 * pathfailure and path obstacle and pathavoider all gone
 * 
 * 8     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 7     4/29/97 6:42p Agrant
 * CFeeling now float-based instead of CRating-based.
 * Activities now rate/act based on a list of influences.
 * Better rating functions for activities.
 * Debugging tools for activity processing.
 * 
 * 6     2/11/97 7:11p Agrant
 * Another pathfinding improvement.
 * 
 * 5     2/09/97 8:18p Agrant
 * bFix now has a default Assert behavior.
 * 
 * 4     2/06/97 7:18p Agrant
 * Now uses CVector2<>  "^" cross product operator
 * 
 * 3     1/30/97 2:39p Agrant
 * Silhouettes added and pathfinder revision.
 * 
 * 2     1/28/97 6:05p Agrant
 * Intermediate pathfinding revision check in
 * 
 * 1     11/01/96 11:37a Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_PATHFINDER_HPP
#define HEADER_GAME_AI_PATHFINDER_HPP

//#include <vector.h>

#include "Lib/Transform/Vector.hpp"
#include "Lib/Std/Array.hpp"

#include "Influence.hpp"
#include "Rating.hpp"


class CPath;
//class CPathFailure;
//class CFailureObstacle;


//lint -save -e1509
//*********************************************************************************************
//
class CPath : public CMSArray< CVector3<>, 10 >
//
//	Prefix: path
//
//	A class that describes a path in terms of a series of destinations.
//
//
//	Notes:
//		A path can be thought of as a line along the surface upon which the dino walks.
//		The line is a sort of tightrope that the dino follows.
//		Pathfinders ought to take into account that dinos tend to occupy space above the path
//		bounded roughly by:
//			The dino's height above the line
//			Half the dino's width to the left and right of the line
//
//		At some point, CPath will be extended to record the surface upon which the line is drawn
//			(whether that be terrain or an object).
//
//		CPath uses CVector2<> to describe its points.  If other types of points are needed, 
//		CPath can be rewritten as a template.
//
//		CPath uses the standard STL iterator and push_back functinality, and will continue to do
//		so if its implementation changes.
//
//		STL stuff supported by CPath:
//			iterator
//			push_back()
//			begin()
//			end()
//			rbegin()
//			rend();
//			reverse_iterator
//
//*********************************************************************************************
{

//
//  Variable declarations
//
private:
//	CPathFailure*	pfFailure;	// Used to describe why the path has most recently failed.
								// 0 if path is successful.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//
	CPath()
	{
//		pfFailure = 0;
	}


	//*****************************************************************************************
	//
	//	Member functions.
	//
	
		//*********************************************************************************
		//
		const CVector2<> v2GetFirstDestination
		(
		)
		//
		//	Gets the first destination of the path.
		//
		//	Notes:
		//		Requires that the path has at least 2 points- start and end.
		//
		//******************************
		{
			Assert(size() >= 2);

			iterator pv2 = begin();
			pv2++;
			return *pv2;
		}

		//*********************************************************************************
		//
		inline CVector2<> v2GetOrigin
		(
		)
		//
		//	Gets the start point of the path.
		//
		//	Notes:
		//		Requires that the path has at least 1 points- start.
		//
		//******************************
		{
			Assert(size() >= 1);

			iterator pv2 = begin();
			return *pv2; 
		}

		//*********************************************************************************
		//
		inline void push_back
		(
			const CVector3<>& v3
		)
		//
		//	Add a point to the path
		//
		//******************************
		{
			*this << v3;
		}

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


};
//lint -restore


//*********************************************************************************************
//
class CPathfinder
//
//	Prefix: pathf
//
//	A virtual base class that finds out how to get from point A to point B.
//
//	Notes:
//		CPathfinder children will use different algorithms for pathfinding.  The pathfinder
//		base class merely defines the framework and allows various pathfinders to substituted 
//		easily.
//
//*********************************************************************************************
{


//
//  Variable declarations
//
public:
	CBrain*	pbrBrain;		// The brain of the animal doing the pathfinding.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//
	CPathfinder(CBrain* pbr) : pbrBrain(pbr)
	{}

	CPathfinder()
	{  Assert(false); }

	//*****************************************************************************************
	//
	//	Member functions.
	//
	
		//*********************************************************************************
		//
		virtual bool bFindPath
		(
			CPath*				ppath,		// The path to be improved by the pathfinder.
			CRating				rt_solidity, // The max solidity through which the dino will go
			TReal				r_close_enough
		) = 0;
		//
		//	Fills "ppath" with a path from its first element to its second element.
		//
		//	Notes:
		//		"ppath" must have 2 elements, "from" and "to".
		//
		//	Returns:
		//		true if successful, else false. The definition of successful varies with
		//		the type of pathfinder.  An "avoidance" pathfinder may only guarantee 
		//		an obstacle-free path for the first segment of the path.
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
		) = 0;
		//
		//	Fills "ppath" with a path leading in the general direction "v2_direction".
		//
		//	Returns:
		//		true if successful, else false. The definition of successful varies with
		//		the type of pathfinder.  An "avoidance" pathfinder may only guarantee 
		//		an obstacle-free path for the first segment of the path.
		//
		//******************************

};


//#ifndef HEADER_GAME_AI_PATHFINDER_HPP
#endif
