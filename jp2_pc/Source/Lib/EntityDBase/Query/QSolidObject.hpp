/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Solid Object querey
 *
 * Bugs:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QSolidObject.hpp                                $
 * 
 * 2     5/09/97 3:20p Rwyatt
 * Added a new query for SolidObjects which is used by the ray caster
 * 
 * 1     5/08/97 2:32p Rwyatt
 * Initial implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QSOLIDOBJ_HPP
#define HEADER_LIB_ENTITYDBASE_QUERY_QSOLIDOBJ_HPP

#include "Lib/EntityDBase/Query.hpp"
#include "Lib/EntityDBase/Instance.hpp"



//*********************************************************************************************
//
class CWDbQuerySolidObject : public CWDbQuery< TPartitionList >
//
// Solid object query container.
//
// Prefix: wqsolobj
//
// Notes:
// Constructing one of these will create a list of solid objects that interset the partition
// passed in. The world parameter defaults to the world but can be any world you may choose..
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQuerySolidObject(const CPartition* ppart, const CWorld& w = wWorld);
};


#endif //#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QSOLIDNOBJ_HPP
