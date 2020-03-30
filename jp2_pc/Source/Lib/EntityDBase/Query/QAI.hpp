/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		AI Query classes for the world database.
 *
 * Bugs:
 *
 * To do:
 *		Change the query base class to allow non-container query objects to lock and unlock the
 *		world database.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QAI.hpp                                         $
 * 
 * 3     5/10/98 6:24p Agrant
 * query for objects with interesting AI props
 * 
 * 2     97/03/04 5:11p Pkeet
 * Changed query interface to use partition objects instead of discrete presences and bounding
 * volumes.
 * 
 * 1     2/03/97 10:39p Agrant
 * Split off from Query.hpp
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QAI_HPP
#define HEADER_LIB_ENTITYDBASE_QUERY_QAI_HPP

#include "Lib/EntityDBase/Query.hpp"
#include "Lib/EntityDBase/Animal.hpp"


//*********************************************************************************************
//
class CWDbQueryAnimal : public CWDbQuery< std::list<CAnimal*> >
//
// World database query container.
//
// Prefix: wqani
//
// Notes:
//		Calling the constructor for this object will instantiate it with a list of CAnimal
//		types that intersect the given bounding volume.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryAnimal(const CPartition* ppart = 0, const CWorld& w = wWorld);
};


//*********************************************************************************************
//
class CWDbQueryAI : public CWDbQuery< std::list<CInstance*> >
//
// World database query container.
//
// Prefix: wqai
//
// Notes:
//		Calling the constructor for this object will instantiate it with a list of CInstance
//		types in which the AI system is interested within the specified bounding volume.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

//	CWDbQueryAI(const CPartition* ppart = 0, const CWorld& w = wWorld);
	CWDbQueryAI(const CBoundVol& bv, const CPresence3<>& pr3_boundvol, const CWorld& w = wWorld);

	void BuildList(const CPartition* ppart_volume, const CPartition* ppart_data);

};


#endif
