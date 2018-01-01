/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:   Schedules operations based on priorities and time slices available.
 *
 * Notes:      Instantiates an LRU object for terrain textures and image caches.
 *
 * To do:      
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/LRU.hpp                                                      $
 * 
 * 2     9/08/98 8:56p Rwyatt
 * LRU memory is now properly destroyed
 * 
 * 1     4/09/98 4:45p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_SYS_LRU_HPP
#define HEADER_LIB_SYS_LRU_HPP


//
// Required includes.
//


//
// Forward declarations.
//
class CLRU;
class CFastHeap;
class CLRUItemCompare;
template<class T> class CMArray;


//
// Constants.
//

// The default number of items to be deleted should the LRU delete be called.
#define iDEFAULT_NUM_DELETE (5)


//
// Class definitions.
//

//*********************************************************************************************
//
class CLRUItem
//
// An item for maintaining in an LRU list.
//
// Prefix: lrit
//
//**************************************
{
private:
	int iAge;	// Age of the object in frames.

public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CLRUItem(int i_age = 0)
		: iAge(i_age)
	{
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual void Delete
	(
	) = 0;
	//
	// Executes the element.
	//
	//**************************************

private:

	friend class CLRU;
	friend class CLRUItemCompare;

};


//*********************************************************************************************
//
class CLRU
//
// Maintains an LRU list.
//
// Prefix: lru
//
//**************************************
{
private:
	bool                bSorted;	// Set to 'true' if the list is sorted.
	CFastHeap*          pfhHeap;	// Heap for allocating LRU items.
	CMArray<CLRUItem*>* papItems;	// Array of pointers to items.
	CLRUItem**			pplritArray;// Memory used by the CMArray above

public:

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	// Default constructor.
	CLRU();

	// Destructor.
	~CLRU();

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void AddItem
	(
		CLRUItem* plrit
	);
	//
	// Adds an lru item to the list.
	//
	// Notes:
	//		Will automatically set the 'bSorted' flag to false.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bDelete
	(
		int i_num_delete = iDEFAULT_NUM_DELETE
	);
	//
	// Deletes the specified number of items from the LRU list.
	//
	// Returns 'true' if one or more items is deleted.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Reset
	(
	);
	//
	// Clears the LRU list and sets the 'bSorted' flag to false.
	//
	//**************************************

	//*****************************************************************************************
	//
	// Friend functions.
	//

	//*****************************************************************************************
	//
	friend void* operator new
	(
		uint	u_size_type,	// Size of object to be allocated in bytes.
		CLRU&	lru				// Reference to the scheduler making the memory allocation.
	);
	//
	// Allocates memory from the heap associated with the LRU object.
	//
	//**************************************

private:

	class CPriv;
	friend class CPriv;

};


//
// Global variables.
//

// The lru object for terrain textures and image caches.
extern CLRU lruTerrainCache;


#endif // HEADER_LIB_SYS_LRU_HPP
