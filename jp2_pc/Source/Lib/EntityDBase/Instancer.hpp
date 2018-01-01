/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *		
 *
 *
 *
 **********************************************************************************************
 *
 *                                                                                            $
 * 
 *********************************************************************************************/

#ifndef HEADER_ENTITYDBASE_INTSTANCER_HPP
#define HEADER_ENTITYDBASE_INTSTANCER_HPP


//
// Includes.
//
#include "set.h"
#include "Container.hpp"


//
// Class definitions.
//

//*********************************************************************************************
//
class CInstancer
//
// Class defining an object in the instancer database.
//
// Prefix: inst
//
//**************************
{
public:

	int   iHandle;	// Handle of object.
	int   iType;	// Type id of object.
	void* pvData;	// Object's data.

public:
	
	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	// Default constructor.
	CInstancer()
		: iHandle(0), iType(0), pvData(0)
	{
	}

	// Constructor to take a symbol and a type id.
	CInstancer(int i_symbol, int i_type)
		: iHandle(i_symbol), iType(i_type), pvData(0)
	{
	}

	~CInstancer()
	{
		// Delete data associated with object.
		delete pvData;
	}
	
	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	bool operator()
	(
		const CInstancer& inst_a,
		const CInstancer& inst_b
	) const
	//
	// Returns 'true' if the symbol for instance 'a' is less than instance 'b', otherwise
	// returns false.
	//
	// Notes:
	//		This member function could be contained in a seperate class (e.g.,
	//		'CLessThanInstance'), but is placed in this class for convenience.
	//
	//**************************
	{
		return inst_a.iHandle < inst_b.iHandle;
	}

};


//*********************************************************************************************
//
class CInstanceCont : public CContainer < set<CInstancer, CInstancer> >
//
// Container class for CInstance.
//
// Prefix: incont
//
// Notes:
//		For convenience, this class opens and closes files. This allows recursive calling of
//		the 'GetInstance' function.
//
//**************************
{
public:

	//*****************************************************************************************
	//
	// CInstanceCont member functions.
	//

	//*****************************************************************************************
	//
	void InsertFind
	(
		const CInstancer& inst
	)
	//
	// Points the container iterator to a copy of the object in the container. If no object
	// is found, one is created.
	//
	//**************************
	{
		pair<iterator, bool> pair_inst = insert(inst);
		itContainer = pair_inst.first;

		Assert(bIsNotEnd());
	}

};


//
// Global functions.
//

//*********************************************************************************************
//
template<class T> void GetInstance
(
	int i_handle,
	int i_type,
	T*& rpt
)
//
// Loads an instance of an object from the instance database, 'intconInstDBase.' If there is
// no instance of the required object, one will be loaded from file.
//
//**************************
{
	// Locate the entry containing the object. If there is no entry, on will be created.
	intconInstDBase.InsertFind(CInstance(i_handle, i_type));

	// Determine whether data has already been loaded for the object.
	if (intconInstDBase.rGet().pvData == 0)
	{
		// No data has been loaded already, so load!
		intconInstDBase.rGet().pvData = (void*)new T();
	}

	// Cast the void* pointer to the object type.
	rpt = (T*)intconInstDBase.rGet().pvData;

	Assert(rpt);
}


#endif
