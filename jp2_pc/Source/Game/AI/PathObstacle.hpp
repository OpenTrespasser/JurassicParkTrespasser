/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Pathfinder obstacle geometries for AI library.
 *		
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/PathObstacle.hpp                                              $
 * 
 * 3     1/28/97 6:05p Agrant
 * Intermediate pathfinding revision check in
 * 
 * 2     11/02/96 7:19p Agrant
 * made base obstacle class non-virtual to it can be used as a null obstacle.
 * 
 * 1     10/23/96 7:32p Agrant
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_PATHOBSTACLE_HPP
#define HEADER_GAME_AI_PATHOBSTACLE_HPP

// Try to make pathfinding work without path obstacles.
#if 0


#include "Lib/Transform/Vector.hpp"

#include "Classes.hpp"


//*********************************************************************************************
//
class CPathObsGeometry
//
//	Prefix: pgeo
//
//	A class that describes an obstacle's shape in the pathfinder world model.
//
//	Notes:
//		This class has no shape and never interferes with movement.
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

	//*****************************************************************************************
	//
	//	Member functions.
	//
	
		//*********************************************************************************
		//
		virtual bool bBlocks
		(
			const CVector2<>&		v2_from,		// Starting point.
			const CVector2<>&		v2_to,			// Ending point.
			const CVector2<>&		v2_direction,	// Direction of travel
			const CInfluence*		pinf			// The obstacle itself
		) const 
		//
		//	true if the influence with this geometry blocks the path segment.
		//
		//	Notes:
		//		v2_to = v2_from + v2_direction
		//
		//	Returns:
		//		true if the obstacle intersects the line segment from v2_from to v2_to.
		//		Otherwise, false.
		//
		//******************************
		{
			return false;
		}

		//*********************************************************************************
		//
		virtual bool bFixPath
		(
			CPath*				ppath,		// A path to be altered.
			const CInfluence*	pinf		// The obstacle itself
		) const
		//
		//	Modifies ppath so that it does not intersect this obstacle.
		//
		//	Notes:
		//		This functionality is really only useful in pathfinders that have a 
		//		relatively small number of obstacles or a simple/stupid pathfinding 
		//		algorithm.
		//
		//	Returns:
		//		true if successful, else false.
		//
		//******************************
		{
			return true;
		}

};



//*********************************************************************************************
//
class CPathObsCylinder : public CPathObsGeometry
//
//	Prefix: obcyl
//
//	A cylindrical obstacle.
//
//	The cylinder is infinitely tall, and goes far into the ground.
//
//*********************************************************************************************
{

//
//  Variable declarations
//
public:
	TReal		rRadius;		// The radius of the cylinder.

//
//  Member function definitions
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//
	CPathObsCylinder
	(
		TReal		r_radius	// The radius of the cylinder.
	)
	{
		rRadius		= r_radius;
	}

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
	//*********************************************************************************
	//
	virtual bool bBlocks
	(
		const CVector2<>&		v2_from,		// Starting point.
		const CVector2<>&		v2_to,			// Ending point.
		const CVector2<>&		v2_direction,	// Direction of travel
		const CInfluence*		pinf			// The obstacle itself
	) const;
	//
	//
	//******************************

		//*********************************************************************************
		//
		virtual bool bFixPath
		(
			CPath*				ppath,		// A path to be altered.
			const CInfluence*	pinf		// The influence to avoid.
		) const;
		//
		//	Notes:
		//		Sets the path so that the first segment is tangent to the cylinder.
		//
		//
		//******************************

};



#endif // 0

//#ifndef HEADER_GAME_AI_PATHOBSTACLE_HPP
#endif
