/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Renderer Query classes for the world database.
 *
 * Bugs:
 *
 * To do:
 *		Change the query base class to allow non-container query objects to lock and unlock the
 *		world database.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QRenderer.hpp                                   $
 * 
 * 12    98.09.12 12:18a Mmouni
 * Changed shape query to render type query.
 * 
 * 11    98/05/08 14:01 Speter
 * Added CWDbQueryAllChildren.
 * 
 * 10    98/02/05 18:26 Speter
 * Added CWDbQueryShapesAndMagnets.
 * 
 * 9     9/29/97 6:05p Agrant
 * Allocate memory to hold the queried presence in the camera presence query.
 * 
 * 8     97/08/01 16:09 Speter
 * Added param to CWdbQueryShapes that includes terrain.
 * 
 * 7     6/14/97 5:52p Agrant
 * Faster light query support
 * 
 * 6     97/06/06 12:59 Speter
 * Added CWDbQueryActiveCameraPlacement.
 * 
 * 5     97/06/05 3:50p Pkeet
 * Added a query to get all objects in the spatial partitioning system.
 * 
 * 4     4/30/97 9:17p Rwyatt
 * Moved esfView member variable from CPartition and put it into the list that the partition
 * functions return. These functions used to return a list of CPartition* now they return a list
 * of structures that contain a CPartition* and esfSideOf.
 * 
 * 3     97/03/24 15:16 Speter
 * Replaced CWDbQueryActiveCamera::pinsCamera with pcamActive; made constructor inline.
 * 
 * 2     97/03/04 5:14p Pkeet
 * Changed query interface to use a partition object instead of discrete presences and bounding
 * volumes.
 * 
 * 1     2/03/97 10:39p Agrant
 * Split off from Query.hpp
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QRENDERER_HPP
#define HEADER_LIB_ENTITYDBASE_QUERY_QRENDERER_HPP

#include "Lib/EntityDBase/Query.hpp"
#include "Lib/EntityDBase/Instance.hpp"


//*********************************************************************************************
//
class CWDbQueryRenderTypes : public CWDbQuery< TPartitionList >
//
// World database query container.
//
// Prefix: wqsh
//
// Notes:
//		Calling the constructor for this object will instantiate it with a list of CInstance
//		types whose renderer type is a 3d shape that intersects the given bounding volume.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryRenderTypes
	(
		const CPartition* ppart = 0, 
		bool b_include_terrain = true, 
		const CWorld& w = wWorld
	);
};


//*********************************************************************************************
//
class CWDbQueryRenderTypesAndMagnets : public CWDbQueryRenderTypes
//
// Prefix: wqshmag
//
// Finds all RenderTypes (as in CWDbQueryRenderTypes) as well as magnets.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryRenderTypesAndMagnets
	(
		const CPartition* ppart = 0, 
		const CWorld& w = wWorld
	);
};



//*********************************************************************************************
//
class CWDbQueryAllParts : public CWDbQuery< TPartitionList >
//
// World database query container.
//
// Prefix: wqp
//
// Notes:
//		Calling the constructor for this object will instantiate it with a list of CInstance
//		types whose renderer type is a 3d shape that intersects the given bounding volume.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryAllParts(const CPartition* ppart = 0, const CWorld& w = wWorld);
};



//*********************************************************************************************
//
class CWDbQueryAllChildren: public CContainer<TPartitionList>
//
// World database query container.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryAllChildren(CPartition* ppart_root);
};




//*********************************************************************************************
//
class CWDbQueryActiveCamera
//
// World database query container.
//
// Prefix: wqcam
//
// Notes:
//		Calling the constructor for this object will instantiate it with a CInstance type
//		whose renderer type refers to the current active camera in the world. This camera's
//		properties are up to date.
//
//		There is only ever one active camera, so this query class does not need to inherit
//		from the CContainer type. This class does contain the tGet() member function, so in
//		use it behaves similarly to the other query container classes.
//
//**************************************
{
	CCamera* pcamCamera;

public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryActiveCamera(const CWorld& w = wWorld)
	{
		pcamCamera = w.pcamActive;
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	CCamera* tGet()
	//
	// Obtain a value from this container.
	//
	// Returns:
	//		The value from this container.
	//
	//**************************************
	{
		Assert(pcamCamera != 0);

		// Return the value.
		return pcamCamera;
	}
};


//*********************************************************************************************
//
class CWDbQueryActiveCameraPlacement
//
// World database query container.
//
// Prefix: wqcamp3
//
// Notes:
//		This class queries the active camera, then simply returns its placement.
//		This is useful so clients who merely want the placement do not need the CCamera
//		definition.
//
//**************************************
{
	CPlacement3<>* pp3Camera;
	CPlacement3<>  p3Camera;

public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryActiveCameraPlacement(const CWorld& w = wWorld);

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	CPlacement3<>& tGet()
	//
	// Obtain a value from this container.
	//
	// Returns:
	//		The value from this container.
	//
	//**************************************
	{
		Assert(pp3Camera != 0);

		// Return the value.
		return *pp3Camera;
	}
};



//*********************************************************************************************
//
class CWDbQueryLights : public CWDbQuery< list<CInstance*> >
//
// World database query container.
//
// Prefix: wqlt
//
// Notes:
//		Calling the constructor for this object will instantiate it with a list of CInstance
//		types whose renderer type is a light whose influence intersects the given bounding
//		volume.
//
//**************************************
{
public:

	static list<CInstance*> lpinsActiveLights;		// The active light list, maintained by the world dbase.

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryLights(const CPartition* ppart = 0, const CWorld& w = wWorld);
};





#endif
