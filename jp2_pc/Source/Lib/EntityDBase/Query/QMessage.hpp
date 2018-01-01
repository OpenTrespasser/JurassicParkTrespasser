/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Message recipient Query classes for the world database.
 *
 * Bugs:
 *
 * To do:
 *		Change the query base class to allow non-container query objects to lock and unlock the
 *		world database.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QMessage.hpp                                    $
 * 
 * 3     6/13/97 5:28p Agrant
 * Keep an active entity list so the query can go faster.
 * 
 * 2     5/08/97 4:09p Mlange
 * Added forward declaration for CPlayer.
 * 
 * 1     2/03/97 10:39p Agrant
 * Split off from Query.hpp
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QMESSAGE_HPP
#define HEADER_LIB_ENTITYDBASE_QUERY_QMESSAGE_HPP

#include "Lib/EntityDBase/Query.hpp"

class CDaemon;

//*********************************************************************************************
//
class CWDbQueryActiveEntities : public CWDbQuery< list<CEntity*> >
//
// World database query container.
//
// Prefix: wqet
//
// Notes:
//		Calling the constructor for this object will instantiate it with a list of current
//		active CEntity types in the world.
//
//**************************************
{
public:

	static bool bAlreadyExists;					// There can be only one of these instantiated at any one time.
	static list<CEntity*>	lpetActiveEntities;	// A list of active entities, maintained by the world database.

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryActiveEntities(const CWorld& w = wWorld);
};


//*********************************************************************************************
//
class CWDbQueryActiveDaemon
//
// World database query container.
//
// Prefix: wqdam
//
// Notes:
//		Locates the design daemon subsystem.
//
//**************************************
{
	CDaemon* pdemDaemon;

public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryActiveDaemon(const CWorld& w = wWorld);


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	CDaemon* pdemGet()
	//
	// Obtain a value from this container.
	//
	// Returns:
	//		The value from this container.
	//
	//**************************************
	{
		Assert(pdemDaemon);

		// Return the value.
		return pdemDaemon;
	}

};


class CPlayer;

//*********************************************************************************************
//
class CWDbQueryActivePlayer
//
// World database query container.
//
// Prefix: wqplay
//
// Notes:
//		Locates the design daemon subsystem.
//
//**************************************
{
	CPlayer* pplayPlayer;

public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryActivePlayer(const CWorld& w = wWorld);


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	CPlayer* pplayGet()
	//
	// Obtain a value from this container.
	//
	// Returns:
	//		The value from this container.
	//
	//**************************************
	{
		Assert(pplayPlayer);

		// Return the value.
		return pplayPlayer;
	}

};


#endif
