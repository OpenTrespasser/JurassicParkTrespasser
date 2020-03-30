/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Water query classes for the world database.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QWater.hpp                                      $
 * 
 * 2     98/07/30 22:15 Speter
 * QueryWater now queries water entities. QueryWaterHeight returns height, given a list of water
 * entities.
 * 
 * 1     97/10/02 12:39 Speter
 * New query function, replaces functionality in COLDTerrain.
 * 
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QWATER_HPP
#define HEADER_LIB_ENTITYDBASE_QUERY_QWATER_HPP

#include "Lib/EntityDBase/Query.hpp"
#include "Lib/EntityDBase/Water.hpp"

//*********************************************************************************************
//
class CWDbQueryWater: public CWDbQuery< std::list<CEntityWater*> >
//
// Water object query.
//
// Prefix: wqwtr
//
//**************************************
{
public:
	static std::list<CEntityWater*> lspetWater;	// List of all water objects in world.

public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryWater
	(
		const CBoundVol& bv, const CPresence3<>& pr3_boundvol,
		const CWorld& w = wWorld
	);
};

//*********************************************************************************************
//
class CWDbQueryWaterHeight
//
// Water object and height query.
//
// Prefix: wqwtr
//
//**************************************
{
public:

	CEntityWater* petWater;				// Water object returned, if any.
	CVector3<> v3Water;					// XY position queried, and height at that point.

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	//*****************************************************************************************
	CWDbQueryWaterHeight
	(
		const CVector2<>& v2_world,		// World point to test water height at.
		const std::list<CEntityWater*>& lspetw = CWDbQueryWater::lspetWater
										// Water list to use; default is world list.
	);
};


#endif
