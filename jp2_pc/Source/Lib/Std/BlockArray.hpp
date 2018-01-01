/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		CBArray		A block allocated array.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/BlockArray.hpp                                                $
 * 
 * 3     98/02/11 16:49 Speter
 * Oops.  Allocated memory twice.  Now derive from CAArray, and do not new/delete myself.
 * 
 * 2     98/02/10 12:59 Speter
 * Update for new CArray template.
 * 
 * 1     97/09/29 5:04p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_BLOCKARRAY_HPP
#define HEADER_LIB_STD_BLOCKARRAY_HPP


//
// Required includes.
//
// #include "Array.hpp"


//
// Defines.
//

// Value representing a non-element.
#define iNO_FREE_ELEMENTS (-1)


//
// Class definitions.
//

//**********************************************************************************************
//
template<class T> class CBArray : public CAArray<T>
//
// A block allocated array.
//
// Prefix: ba
//
// Notes:
//		This array allow memory to be allocated as a block for a number of discrete elements,
//		an then is able to allow these elements to be accessed through an index value.
//		The use of an index value can allow a savings in memory compared with the block
//		allocator class which is pointer based -- indexes can be less than 32 bit values.
//
//		This class should be employed with caution: random access to the array without using
//		the 'iNewElement' and 'DeleteElement' functions will cause the array to become invalid.
//
//**************************************
{
	int  iNextFree;	// Index to the next free element.
	uint uNumFree;	// Number of free elements in the array.
public:
	
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Construct and array of a given maximum.
	CBArray(uint u_len)
		: CAArray<T>(u_len), iNextFree(0), uNumFree(u_len)
	{
		Assert(uLen > 0);

		// Initialize array.
		Reset();
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	int iNewElement()
	//
	// Allocates memory for a new data element from the array.
	//
	// Returns:
	//		The index of the new data value or returns 'iNO_FREE_ELEMENTS' to indicate that no
	//		free memory was available.
	//
	//**********************************
	{
		// If there are no free elements, return an error message.
		if (uNumFree == 0)
			return iNO_FREE_ELEMENTS;

		// Decrement the free element count.
		--uNumFree;

		// If there is only one free element, return it.
		if (uNumFree == 0)
		{
			return iNextFree;
		}

		// If there is more than one free element, use the element after the next free element.
		int i_retval = riIndex(iNextFree);
		riIndex(iNextFree) = riIndex(i_retval);

		// Return the element's index value.
		return i_retval;
	}

	//******************************************************************************************
	//
	int iNewElement(const T& t)
	//
	// Allocates memory for a new data element from the array and copies a value into the new
	// element.
	//
	// Returns:
	//		The index of the new data value or returns 'iNO_FREE_ELEMENTS' to indicate that no
	//		free memory was available.
	//
	//**********************************
	{
		// Get a new element.
		int i_new_element = iNewElement();

		// Do nothing if the new element is not valid.
		if (i_new_element == iNO_FREE_ELEMENTS)
			return iNO_FREE_ELEMENTS;

		// Copy the data.
		atArray[i_new_element] = t;

		// Return the element's index value.
		return i_new_element;
	}

	//******************************************************************************************
	//
	void DeleteElement(int i_element)
	//
	// Removes an element from the array and adds it to the free list.
	//
	//**********************************
	{
		Assert(i_element != iNO_FREE_ELEMENTS);
		Assert(i_element >= 0);
		Assert(i_element < (int)uLen);

		// If there are no free elements, make this the sole free element.
		if (uNumFree == 0)
		{
			++uNumFree;
			iNextFree = i_element;
			riIndex(i_element) = iNO_FREE_ELEMENTS;
			return;
		}

		// If there is one free element, chain the two elements together.
		if (uNumFree == 1)
		{
			++uNumFree;
			riIndex(i_element) = iNextFree;
			riIndex(iNextFree) = i_element;
			return;
		}

		// Indicate that the element is released.
		++uNumFree;

		// Insert the freed element between the next free element and the element after it.
		riIndex(i_element) = riIndex(iNextFree);
		riIndex(iNextFree) = i_element;
	}

	//******************************************************************************************
	//
	void Reset()
	//
	// Adds all array elements to the free list.
	//
	//**********************************
	{
		Assert(atArray);

		// Initialize array with index values indicating an empty array.
		atArray[uLen - 1] = reinterpret_cast<T>(0);
		for (uint u = 0; u < uLen - 1; ++u)
		{
			atArray[u] = reinterpret_cast<T>(u + 1);
		}
	}

private:

	//******************************************************************************************
	//
	int& riIndex(int i)
	//
	// Returns a reference to the array element in the form of a reference to an integer.
	//
	//**********************************
	{
		Assert(i >= 0);
		Assert(i < (int)uLen);
		Assert(atArray);

		return *(reinterpret_cast<int*>(&atArray[i]));
	}
};


#endif // HEADER_LIB_STD_BLOCKARRAY_HPP