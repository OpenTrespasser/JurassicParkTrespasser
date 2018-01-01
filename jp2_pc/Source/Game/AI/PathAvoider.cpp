/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of PathAvoid.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/PathAvoider.cpp                                              $
 * 
 * 15    5/16/98 10:00a Agrant
 * Pathfinding fixes for the AI bounding box vs the physics bounding box
 * Major pathfinding fix in general-  how did it ever work???
 * 
 * 14    1/30/98 6:22p Agrant
 * Remove dead code.
 * 
 * 13    97/10/30 15:14 Speter
 * CPhysicsInfo::pbvGetBoundVol() now returns const CBoundVol*.
 * 
 * 12    9/09/97 9:05p Agrant
 * Using MaybeDebugSay
 * 
 * 11    5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 10    3/28/97 2:00p Agrant
 * Brain now points to graph, mentalstate, and worldview
 * Pathfinding more efficient
 * 
 * 9     2/11/97 7:11p Agrant
 * Another pathfinding improvement.
 * 
 * 8     2/09/97 8:18p Agrant
 * Minor fix to pathavoider.  If no path correction necessary, does nothing.
 * 
 * 7     2/06/97 7:18p Agrant
 * Now uses CVector2<>  "^" cross product operator
 * 
 * 6     1/30/97 2:39p Agrant
 * Silhouettes added and pathfinder revision.
 * 
 * 5     1/28/97 6:05p Agrant
 * Intermediate pathfinding revision check in
 * 
 * 4     11/20/96 1:26p Agrant
 * Now using world database queries for perception.
 * Now using archetypes for default personality behaviors.
 * 
 * 3     10/31/96 7:27p Agrant
 * changed over to game object system and world database
 * 
 * 2     10/23/96 7:38p Agrant
 * more debugging info
 * first pass at the tree avoiding pathfinder
 * 
 * 1     10/22/96 7:49p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#if 0





#include "Common.hpp"
#include "PathAvoider.hpp"

#include "PathFailure.hpp"

#include "WorldView.hpp"
#include "Brain.hpp"
#include "Silhouette.hpp"

#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/Renderer/GeomTypes.hpp"

//#define rSAFE_OBSTACLE_DISTANCE  30.0;

//*********************************************************************************************
//
//	Class CPathAvoider implementation.
//



	//*********************************************************************************
	bool CPathAvoider::FindPath
	(
		CPath*	ppath,		// The path to be improved by the pathfinder.
		CRating rt_solidity
	)
	{
		// Only works on one segment paths.
		Assert(ppath->size() == 2);

		// Analyze the path.
		if (pbrBrain->pwvWorldView->bIsPathValid(ppath, rt_solidity))
			return true;
		else
		{
			// Call the correct fix function for the failure mode.

			pbrBrain->MaybeDebugSay("Fixing Path- Avoiding.\n");
			return ppath->pfGetFailure()->bFix(this, ppath);
		}

/*		
		// Find the most important obstacle.
		CInfluence *pinf_obstacle = pobsGetMostImportantObstacle(ppath);

		// Avoid it, or ignore it if none found.
		if (pinf_obstacle)
			pinf_obstacle->bFixPath(ppath);

		return true;
		*/
	}

	//*********************************************************************************
	bool CPathAvoider::FindDirectionPath
	(
		CVector2<>,//			v2_location,	// Starting point.
		CVector2<>,//			v2_direction,	// Direction of travel.
		CPath*	,//			ppath,			// The path to be filled by the pathfinder.
		CRating	//			rt_solidity
	)
	{
		// Not yet implemented!!!
		Assert(0);
		return false;
	}

#if 0
	//*********************************************************************************
	void CPathAvoider::Reset
	(
	)
	{
		// Not yet implemented!!!
		Assert(0);
	}

	//*********************************************************************************
	void CPathAvoider::SetObstacleList
	(
		CInfluenceList *infl
	)
	{
		pinflObstacles = infl;
	}
#endif // 0

	//*********************************************************************************
	CInfluence* CPathAvoider::pobsGetMostImportantObstacle
	(
		CPath*				//ppath
	)
	{

		Assert(0);

		/*
		CVector2<> v2_direction = ppath->v2GetFirstDestination() -  ppath->v2GetOrigin();

		// Keep track of the closest.
		TReal			r_nearest = v2_direction.tLen();

		// Keep track of the closest blocker.
		CInfluence*		pinf_best = 0;


		CInfluenceList::iterator	ili;
		CInfluence *				pinf;

		for( ili = pinflObstacles->begin(); ili != pinflObstacles->end(); ili++)
		{
			pinf = (CInfluence*)&(*ili);

			// If closer...
			if (r_nearest > (*pinf).rDistanceTo)
			{
				// and blocking the path...
				if ((*pinf).bBlocks(ppath->v2GetOrigin(), ppath->v2GetFirstDestination(), v2_direction))
				{
					// Mark the object as being considered as a path blocker.
					pinf->AddDebug('B');

					// Then the obstacle is more important.
					r_nearest = (*pinf).rDistanceTo;
					pinf_best = pinf;
				}
				else
					pinf->AddDebug('N');
		
			}
			else
				pinf->AddDebug('D');
		}

		if (pinf_best)
			// Mark the object as being the most important blocker.
			pinf_best->AddDebug('*');

		return pinf_best;
*/
		return 0;
	}

	//*********************************************************************************
	bool CPathAvoider::bFix
	(
		CPath*				ppath,
		CFailureObstacle*	pfo
	)
	{
		// PathAvoider only promises that the first segment is clear.
		if (pfo->iPathSegment > 0)
			return true;

		// Okay, what we have here is an obstacle in the first segment.  Our job is to 
		// insert a point between 0 and 1 such that the new path circumvents the obstacle.

		// Grab the instance that is the obstacle.
 		const CInstance* pins = pfo->pinfObstacle->pinsTarget;

		// One way to solve this problem would be to project the obstacle into a 2-d silhouette
		// polygon.  Then, any simple piecewise linear path around the obstacle goes through the
		// vertices of the projected silhouette polygon.  We find the easiest path around on a linear
		// basis, and then add slop for dino width.  Future implementations can take Z into account
		// when calculating the projection, and do fancy stuff like chop off a box when higher
		// vertices are above dino head height.

		const CBoundVol* pbv = pins->paiiGetAIInfo()->pbvGetBoundVol();
		Assert(pbv);

		// Can't handle non-boxes yet!!!
		Assert(pbv->pbvbCast());

//		CBoundVolBox* pbvb = pbv->pbvbCast();

//		TReal r_dino_width = gaiSystem.pbrGetCurrentBrain()->rWidth;

		// Construct a silhouette for the obstacle.
		const CSilhouette& sil = pfo->pinfObstacle->silSilhouette; 

		
		// Okay, what we have now is a path segment that travels through the silhouette.
		// The silhouette has been expanded to take dino width into account, so we only 
		// need to find a good path around it.  Since the silhouette is a nice concave polygon, 
		// the best path around it will go through one or more corners of the polygon.

		// The plan is this:  For each corner, we'll determine the angle between [the path from
		// where we are now to the corner] and [the path between where we are now and our original
		// destination].  We'll find the largest leftward angle and the largest rightward angle, 
		// both of which indicate paths that do not intersect the silhouette.  We'll then choose
		// the smaller of the two angles, as the smaller will deflect our original path the least.

		
		TReal r_sin_theta;  // The sin of the angle between the bad path and the current possibly
							// correct path.

		// Save the highest and lowest values for r_sin_theta so far.
		TReal	r_minZ = 0;		// Minimum sin value.
		int		i_minZ = -1;	// Corner which has minimum sin value.
		TReal	r_maxZ = 0;		// Maximum sin value.
		int		i_maxZ = -1;	// Corner which has maximum sin value.
		
		CVector2<> v2_start = (*ppath)[0];						// Point from which our travels start.
		CVector2<> v2_original_path = ((CVector2<>)(*ppath)[1]) - v2_start;	// Vector pointing to destination from start.
		v2_original_path.Normalise();						// Normalise it.

		CVector2<> v2_test_path;							// The path to the point to be tested.

		for (int i = sil.size() - 1; i >= 0; i--)
		{
			// Get each corner in turn.
			v2_test_path = sil[i] - v2_start;
			v2_test_path.Normalise();

			// Calculate sin
			r_sin_theta = v2_original_path ^ v2_test_path;

			//  Make sure that the point is not behind us.
//			Assert(v2_original_path * v2_test_path > 0);

			if (r_sin_theta > r_maxZ)
			{
				r_maxZ = r_sin_theta;
				i_maxZ = i;
			}
			else if (r_sin_theta < r_minZ)
			{
				r_minZ = r_sin_theta;
				i_minZ = i;
			}
		}

		// We'd better have points to both left and right!
		// if not, do not correct path.
		if (r_minZ == 0 || r_maxZ == 0)
			return true;

		if (r_maxZ < - r_minZ)
		{
			// The positive side corner deflects the path the least.
			(*ppath)[1] = sil[i_maxZ];
		}
		else
		{
			// The negative side corner deflects the path the least.
			(*ppath)[1] = sil[i_minZ];
		}

		return true;
	}

#endif