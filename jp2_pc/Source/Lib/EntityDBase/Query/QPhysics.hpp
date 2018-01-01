/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Query classes for the world database.
 *
 * Bugs:
 *
 * To do:
 *		Change the query base class to allow non-container query objects to lock and unlock the
 *		world database.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QPhysics.hpp                                    $
 * 
 * 7     98/09/12 1:10 Speter
 * Added Partition-based query for QueryPhysicsBoxFast.
 * 
 * 6     98/05/15 16:11 Speter
 * Added physics queries for movable objects only, and fast query for box instances.
 * 
 * 5     97/09/29 16:31 Speter
 * Changed CWDbQueryPhysics to take CPresence3<>& rather than pointer.
 * 
 * 4     6/07/97 1:44a Agrant
 * Sped up the physics query.  We can use a similar method for other queries.
 * 
 * 3     97/03/08 6:13p Pkeet
 * Added the old query format constructor.
 * 
 * 2     97/03/04 5:12p Pkeet
 * Changed query interface to use partitions instead of discrete presences and bounding volumes.
 * 
 * 1     2/03/97 10:39p Agrant
 * Split off from Query.hpp
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QPHYSICS_HPP
#define HEADER_LIB_ENTITYDBASE_QUERY_QPHYSICS_HPP

#include "Lib/EntityDBase/Query.hpp"


//*********************************************************************************************
//
class CWDbQueryPhysics : public CWDbQuery< list<CInstance*> >
//
// Prefix: wqph
//
// Finds all physics objects in the region.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryPhysics(const CPartition* ppart = 0, const CWorld& w = wWorld);

	CWDbQueryPhysics
	(
		const CBoundVol& bv, const CPresence3<>& pr3_boundvol, 
		const CWorld& w = wWorld
	);
};


//*********************************************************************************************
//
class CWDbQueryPhysicsMovable : public CWDbQuery< list<CInstance*> >
//
// Prefix: wqphm
//
// Finds all movable physics objects in the region.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryPhysicsMovable(const CPartition* ppart = 0, const CWorld& w = wWorld);
};


//*********************************************************************************************
//
class CWDbQueryPhysicsBoxFast : public CWDbQuery< list<CInstance*> >
//
// Prefix: wqph
//
// Finds all box physics objects in the region. Performs a fast-and-loose intersection
// test, perhaps returning objects a bit outside the region.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryPhysicsBoxFast
	(
		const CBoundVol& bv, const CPresence3<>& pr3_boundvol, 
		const CWorld& w = wWorld
	);

	CWDbQueryPhysicsBoxFast
	(
		const CPartition* ppart = 0, 
		const CWorld& w = wWorld
	);
};




#endif
