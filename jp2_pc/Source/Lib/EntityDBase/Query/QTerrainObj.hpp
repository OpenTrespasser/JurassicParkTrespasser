/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Terrain object query class
 *
 * Bugs:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query/QTerrainObj.hpp                                 $
 * 
 * 3     98/10/02 15:39 Speter
 * Added sort option to terrain object query.
 * 
 * 2     5/01/97 3:47p Rwyatt
 * Renamed calls to ptCastInstance to ptCast. This is because it no longer casts from an
 * instance but from a partition. Renaming the function to ptCastPartition would have caused
 * more work later if another class is added under cPartition. All casts have base
 * implementations in CPartition rather than CInstance
 * 
 * 1     5/01/97 3:44p Rwyatt
 * Initial implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QTERRAINOBJ_HPP
#define HEADER_LIB_ENTITYDBASE_QUERY_QTERRAINOBJ_HPP

#include "Lib/EntityDBase/Query.hpp"
#include "Lib/EntityDBase/Instance.hpp"


//*********************************************************************************************
//
class CWDbQueryTerrainObj : public CWDbQuery< TPartitionList >
//
// Terrain object query container.
//
// Prefix: wqtrrobj
//
// Notes:
// Constructing one of these will create a list of terrain objects that interset the partition
// passed in. The world parameter defaults to the world but can be any world you may choose..
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryTerrainObj
	(
		const CPartition* ppart, 
		bool b_sort = false,		// Whether to sort the objects by ascending height.
		const CWorld& w = wWorld
	);
};


#endif //#ifndef HEADER_LIB_ENTITYDBASE_QUERY_QTERRAINOBJ_HPP
