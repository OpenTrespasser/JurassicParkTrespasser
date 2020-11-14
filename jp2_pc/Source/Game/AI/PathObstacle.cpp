/**********************************************************************************************
 *
 * $Source::																				  $
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of PathObstacle.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/PathObstacle.cpp                                             $
 * 
 * 5     12/11/96 11:51a Agrant
 * v2.bIsZero  ==>> v2.bIsZero()
 * bug fix for pathfinding
 * 
 * 4     11/14/96 4:50p Agrant
 * AI subsystem now in tune with the revised object hierarchy
 * 
 * 3     11/02/96 7:24p Agrant
 * Pathfinder now uses brain debug func, to let us know which animal is pathfinding
 * 
 * 2     10/23/96 7:38p Agrant
 * more debugging info
 * first pass at the tree avoiding pathfinder
 * 
 * 1     10/22/96 9:25p Agrant
 * intial rev
 *
 *********************************************************************************************/

#include "Common.hpp"
#include "PathObstacle.hpp"

#include "Lib/Sys/Textout.hpp"

#include "Influence.hpp"
#include "Pathfinder.hpp"
#include "AIMain.hpp"

//*********************************************************************************************
//
//	Class CPathObsCylinder implementation.
//

	//*********************************************************************************
	//
	bool CPathObsCylinder::bBlocks
	(
		const CVector2<>&		v2_from,		// Starting point.
		const CVector2<>&		v2_to,			// Ending point.
		const CVector2<>&		v2_direction,	// Direction of travel
		const CInfluence*		pinf			// The obstacle itself
	) const
	//
	//
	//******************************
	{
		// Function essentially returns true if the path intersects the circle.

		Assert(Fuzzy(v2_to.tX) == (v2_from + v2_direction).tX);

		TReal	r_dot_product = v2_direction * pinf->v2ToTarget;

		if (r_dot_product < 0)
			// Path leads away from pinf
			return false;
		else
		{
			// Not heading away, so check to see if line intersects circle.
			
			// Using algorithm on p. 5-6 Graphics Gems I

			CVector2<> v2_g		= v2_from - pinf->v2Location;
			TReal	r_a			= v2_direction * v2_direction;
			TReal	r_b			= 2.0 * (v2_direction * v2_g);
			TReal	r_c			= (v2_g * v2_g) - (rRadius * rRadius);
			TReal	r_d			= r_b * r_b - 4.0 * r_a * r_c;

			return (r_d >= 0);

/*
			TReal r_distance = v2_direction.tLen();

			if (r_distance == 0)
				return false;
			else if (r_distance + rRadius < pinf->rDistanceTo)
				return false;


			TReal r_distance_to_path =  r_dot_product / r_distance;
			//  r_dot_product / r_distance is the distance between the path and 
			//	center of the cylinder.
			return (rRadius > r_div);
			*/
		}
	}


	//*********************************************************************************
	bool CPathObsCylinder::bFixPath
	(
		CPath*				ppath,		// A path to be altered.
		const CInfluence*	pinf		// The influence to avoid.
	) const
	{
		// Actually, right now it isn't quite that elegant.
		
		// Get a vector perpendicular to the one to the obstacle.
		CVector2<> v2_perp(pinf->v2ToTarget.tY, - pinf->v2ToTarget.tX);
		if (v2_perp.bIsZero())
			return false;

		
		// Set its length to the radius plus 10%.
		v2_perp.Normalise(rRadius * 1.1);
		
		// Now pick the closer of left or right.
		// Try one side.
		if (v2_perp * (ppath->v2GetFirstDestination() - ppath->v2GetOrigin()) < 0)
			// Obtuse angle, so we've picked the wrong side.
			v2_perp = - v2_perp;

		// Insert the point between the first and second points in the path.
		CPath::iterator pv2 = ppath->begin();
		pv2++;
		ppath->insert(pv2, v2_perp + pinf->v2Location);

		gaiSystem.pbrGetCurrentBrain()->DebugSay("Fixing path.\n");

		return true;
	}
