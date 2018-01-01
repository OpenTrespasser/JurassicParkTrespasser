/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Filter templates for accessing types of pointers to objects in heterogenous stl
 *		containers.
 *
 * Bugs:
 *
 * To do:
 *		Remove this header file! Its functionality has been superseded by the identifier
 *		functions declared in CInstance.
 *
 * Notes:
 *		Requires that the class 'C' refer to an stl container templated from pointers to
 *		objects.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/FilterIterator.hpp                                    $
 * 
 * 2     11/16/96 4:20p Mlange
 * Updated todo list.
 * 
 * 1     10/22/96 7:15p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_ITERATORFILTER_HPP
#define HEADER_LIB_ENTITYDBASE_ITERATORFILTER_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
template<class C, class T> class CIteratorFilter
//
// The iterator filter class is designed to iterate only through objects in a given STL
// container (specified by class 'C') of an object type (specified by 'T').
//
// Prefix: itf
//
// Notes:
//		This template will only work with STL containers templated from pointers to objects
//		because it uses a 'dynamic_cast' to check the pointer type.
//
//		An example of using this class is as follows:
//
//			for (CIteratorFilter<list(CA*), CAA> itf_ca(list_pca)); itf_ca.bIsNotEnd();
//			     itf_ca++)
//			{
//				itf_ca.pGet()->DoSomething(); // Does something specific to class CAA.
//			}
//
//		Where CAA is a child class of class CA, and list_pca is defined as
//		'list<CA*> list_pca.'
//
//**************************************
{
public:

	C* pContainer;				// Pointer to the STL container.
	C::iterator itContainer;	// Iterator for the STL container.

public:

	//*****************************************************************************************
	//
	// CIteratorFilter constructor.
	//

	CIteratorFilter(const C& container)
		: pContainer((C*)&container)
	{
		//
		// Set the iterator to the first pointer specified by the template type 'T.'
		//
		itContainer = pContainer->begin();

		// Skip objects of the wrong kind.
		SkipMiscasts();
	}

	//*****************************************************************************************
	//
	// CIteratorFilter member functions.
	//

	//*****************************************************************************************
	//
	bool bIsNotEnd()
	//
	// Returns 'true' if there are still elements in the container to be iterated.
	//
	//**************************************
	{
		return itContainer != pContainer->end();
	}

	//*****************************************************************************************
	//
	void operator ++(int)
	//
	// Moves the iterator to the next valid object in the container.
	//
	//**************************************
	{
		itContainer++;

		// Skip objects of the wrong kind.
		SkipMiscasts();
	}

	//*****************************************************************************************
	//
	T* pGet()
	//
	// Returns a pointer of the filter object type.
	//
	//**************************************
	{
		T* pt = (T*)(*itContainer);

		Assert(pt);

		return pt;
	}

private:

	//*****************************************************************************************
	//
	void SkipMiscasts()
	//
	// Skips elements in the STL container that do not cast to the template type.
	//
	//**************************************
	{
		for (;;)
		{
			// Make sure the iterator does not move beyond the container.
			if (!bIsNotEnd())
				return;

			// Check to see if the contained object is of the right type.
			if (dynamic_cast<T*>(*itContainer))
				return;
	
			// If the object isn't of the right type, iterate to the next one.
			itContainer++;
		}
	}

};


#endif
