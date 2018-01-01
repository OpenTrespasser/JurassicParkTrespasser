/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Optimal sort routine.
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/Std/Sort.hpp                                                      $
 * 
 * 1     97/04/02 6:47p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_SORT_HPP
#define HEADER_LIB_STD_SORT_HPP


//
// Sort functions.
//

//**********************************************************************************************
//
template<class T, class C>
void SortLargeList
(
	T    apt_items,		// An array of pointers to items of type 'T.'
	uint u_num_items,	// Number of items in the array.
	C*					// Comparison function that returns a 'bool.'
)
//
// Sorts the array of type 'T' by using the comparison function 'bCompare.'
//
//**************************************
{
	//
	// Hacky insertion sort to demonstrate.
	//
	for(uint u_i = 1; u_i < u_num_items; u_i++)
	{
		uint u_j = u_i;

		while(u_j > 0 && C::bCompare(apt_items[u_j - 1], apt_items[u_j]))
		{
			Swap(apt_items[u_j], apt_items[u_j - 1]);
			u_j--;
		}
	}
}


#endif // HEADER_LIB_STD_SORT_HPP

