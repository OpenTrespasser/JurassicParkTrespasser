/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		Trigger query classes for the world database.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QTriggers.hpp                                   $
 * 
 * 2     9/18/98 10:49a Mlange
 * Location trigger query now returns CLocationTrigger types only, instead of CTrigger base
 * class types.
 * 
 * 1     8/21/98 7:55p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QTRIGGERS_HPP
#define HEADER_LIB_ENTITYDBASE_QUERY_QTRIGGERS_HPP

#include "Lib/EntityDBase/Query.hpp"

class CLocationTrigger;

//*********************************************************************************************
//
class CWDbQueryLocationTrigger : public CWDbQuery< std::list<CLocationTrigger*> >
//
// Prefix: wqlt
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//
	CWDbQueryLocationTrigger
	(
		const CBoundVol& bv, const CPresence3<>& pr3_boundvol, 
		const CWorld& w = wWorld
	);
};


#endif
