/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Raycasting class(es)
 *
 *		CRayCast
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/RayCast.hpp                                             $
 * 
 * 12    98/05/22 20:54 Speter
 * Changed raycast construction to let objects insert multiple intersections into list.
 * 
 * 11    98/05/06 23:18 Speter
 * If f_length == 0, find nearest point in any direction.
 * 
 * 10    98/04/16 14:22 Speter
 * Now returns SObjectLoc info with precise collision information. Moved container classes into
 * private member pointer. 
 * 
 * 9     98/03/06 10:52 Speter
 * Now store results in vector<>, so it can be sorted. Added DrawPhysics() to show last raycast.
 * 
 * 8     98/02/04 14:46 Speter
 * CRayCast now stores and retrieves the querying volume it uses (for debugging).
 * 
 * 7     98/01/12 16:37 Speter
 * Fixed bug in ResetCollisionList(); a function call without parentheses is a null expression!
 * Made member variables protected.
 * 
 * 6     97/12/15 18:52 Speter
 * Changed ppartNextBoundingCollision() to CInstance* pins...  Made DistanceSort() protected;
 * now always sorts.
 * 
 * 5     97/10/15 1:28a Pkeet
 * Changed protected data members to public.
 * 
 * 4     10/14/97 10:38p Rwyatt
 * 
 * 3     97/10/14 10:11p Pkeet
 * Changed basic query to a physics query because physics objects are required and the solid
 * object query is broken.
 * 
 * 2     5/09/97 3:21p Rwyatt
 * Now can sort items in the ray cast list baed on distance from the ray start
 * 
 * 1     4/30/97 12:19p Rwyatt
 * Initial implementation of ray cast class
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_RAYCAST_HPP
#define HEADER_LIB_GEOMDBASE_RAYCAST_HPP


#include "Partition.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"

class CDraw;
class CCamera;


//**********************************************************************************************
//
struct SObjectLoc: public SVolumeLoc
//
// Prefix: obl
//
// Describes a collision location on an object.
// Adds world object to the SVolumeLoc info.
//
//**************************************
{
	CInstance*		pinsObject;			// The object that has been hit.
};


//**********************************************************************************************
//
class CRayCast
//
// Base class for ray casting
//
// Prefix: rc
//
//**************************************
{
protected:

	// Private information.
	class CPrivate;
	rptr<CPrivate> ppriv;

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	//******************************************************************************************
	CRayCast
	(
		const	CPresence3<>& pr3,		// presence describing the origin, direction and scale
		float	f_diameter=0.1,			// diameter of the ray (default is a narrow ray)
		float	f_length=10000			// length of the ray to cast (default bigger than island)
		// NOTE: the radius is actually the size of a square around the ray and not the radius
		// of the cylinder....
	);
	//
	// Cast a ray from the position of the presense in the direction of the presence.
	// If f_length == 0, it construct a box of diameter f_diamter, and find the nearest point on
	// an object in any direction.
	//
	//**********************************


	//******************************************************************************************
	~CRayCast();

	//******************************************************************************************
	//
	void InsertIntersection
	(
		SObjectLoc& obl
	);
	//
	// Inserts a collision record into the raycast list.
	//
	// Notes:
	//		This is called by helper functions of specific volumes, which are called by the
	//		CRayCast() constructor. It is not used once the constructor is completed.
	//
	//**********************************

	//******************************************************************************************
	//
	void ResetIntersectionList();
	//
	//**********************************

	//******************************************************************************************
	//
	CInstance* pinsNextObject();
	//
	// Returns:
	//		Next object intersected.
	//
	//**********************************

	//******************************************************************************************
	//
	SObjectLoc* poblNextIntersection();
	//
	// Returns:
	//		Description of next object intersected, and where.
	//
	//**********************************

	//******************************************************************************************
	//
	static void DrawPhysics(CDraw& draw, CCamera& cam);
	//
	// Draws the volume of the last raycast performed.
	//
	//**********************************
};


//**********************************************************************************************
// Cast a ray out from a screen pixel co-ordinate
CRayCast* rcCastScreenRay
(
	float	f_xpos,			// normalized screen co-ord, 0,0 top left -> 1,1 bottom right
	float	f_ypos,
	float	f_ray_length,	// length of ray
	float	f_ray_diam		// diameter of ray
);



#endif //#ifndef HEADER_LIB_GEOMDBASE_RAYCAST_HPP
