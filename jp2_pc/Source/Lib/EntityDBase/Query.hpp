/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
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
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query.hpp                                             $
 * 
 * 19    97/10/15 22:13 Speter
 * Removed locking and pwWorld member in CWDbQuery base class; class is now largely obsolete.
 * 
 * 18    5/30/97 11:13a Agrant
 * LINT tidying
 * 
 * 17    2/03/97 10:35p Agrant
 * Query.hpp and MessageTypes.hpp have been split into
 * myriad parts so that they may have friends.
 * Or rather, so compile times go down.
 * Look for your favorite query in Lib/EntityDBase/Query/
 * Look for messages in                Lib/EntityDBase/MessageTypes/
 * 
 * 16    12/17/96 4:10p Pkeet
 * Added a base class for query containers that locks and unlocks the world database.
 * 
 * 15    12/14/96 4:53p Pkeet
 * Added 'CWDbQueryActivePlayer' class.
 * 
 * 14    12/14/96 3:29p Pkeet
 * Added the 'CWDbQueryActiveDaemon' class.
 * 
 * 13    12/11/96 3:59p Mlange
 * Updated for changes to the terrain query function.
 * 
 * 12    12/09/96 1:43p Mlange
 * Updated for changes to the CCamera interface.
 * 
 * 11    12/05/96 3:42p Mlange
 * Added a CPresence3<>* parameter to the constructor of several query classes so that the
 * orientation, position and scale of the bounding volume can be specified.
 * 
 * 10    11/27/96 12:37p Mlange
 * Updated the lights query class. Removed some #includes.
 * 
 * 9     11/23/96 5:41p Mlange
 * Renamed the query function for shapes. Updated the active camera query class.
 * 
 * 8     11/19/96 3:50p Mlange
 * Updated for CPlane name change. Removed dummy class definition.
 * 
 * 7     11/16/96 4:21p Mlange
 * Rewrite.
 * 
 * 6     11/14/96 12:11p Agrant
 * Animal query now contains CAnimal* instead of CInstance*
 * 
 * 5     11/13/96 9:04p Agrant
 * Added a CAnimal query function.
 * 
 * 4     11/13/96 5:01p Pkeet
 * Replaced CEntityCamera and its include with CCamera and its include.
 * 
 * 3     11/13/96 12:41p Agrant
 * world db now based off of CInstance
 * Added query for instances with physics info-Not yet fully tested.
 * 
 * 2     11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 1     96/11/09 10:04p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUERY_HPP
#define HEADER_LIB_ENTITYDBASE_QUERY_HPP


#include <list.h>
#include "Lib/EntityDBase/Container.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"

//*********************************************************************************************
//
template<class TCont> class CWDbQuery : public CContainer<TCont>
//
// The base class for all world database query containers.
//
// Prefix: wq
//
// Notes: Locking the world should be done around actual traversal code, not during the
// entire lifetime of a query object.
//
// This class currently has no functionality, and should probably be removed as a base.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQuery(const CWorld& w)
	{
	}

	virtual ~CWDbQuery()
	{
	}
};




#endif
