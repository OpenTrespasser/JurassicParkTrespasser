/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Subsystem query class for the world database.
 *
 * Bugs:
 *
 * To do:
 *		Change the query base class to allow non-container query objects to lock and unlock the
 *		world database.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QSubsystem.hpp                                  $
 * 
 * 2     3/14/97 5:43p Mlange
 * Moved implementation to cpp file.
 * 
 * 1     3/14/97 3:13p Mlange
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QAI_HPP
#define HEADER_LIB_ENTITYDBASE_QUERY_QAI_HPP

#include "Lib/EntityDBase/Query.hpp"
#include "Lib/EntityDBase/Instance.hpp"


//*********************************************************************************************
//
class CWDbQuerySubsystem : public CWDbQuery< list<CSubsystem*> >
//
// World database query container.
//
// Prefix: wqsubs
//
// Notes:
//		Calling the constructor for this object will instantiate it with a list of the
//		CSubsystems in the world database.
//		Note that, unlike the other query classes, a bounding volume need not be specified
//		because the subsystems have no position in the world.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQuerySubsystem(const CWorld& w = wWorld);
};


#endif
