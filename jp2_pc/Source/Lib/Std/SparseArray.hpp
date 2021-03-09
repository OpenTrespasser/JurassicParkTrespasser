#pragma once

/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Class for maintaining a static sparse array.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/SparseArray.hpp                                               $
 * 
 * 3     6/11/98 2:57p Agrant
 * fixed reset function
 * 
 * 2     97/05/21 17:32 Speter
 * Updated for new CMArray argument order.
 * 
 * 1     3/26/97 5:35p Agrant
 * Initial revision
 * 
 **********************************************************************************************/

#include "Lib/Std/Array.hpp"

//**********************************************************************************************
//
template<class T> class CSparseArray: public CMAArray<T>
//
// A sparse CMAArray.
//
// Prefix: sa
//
//
//	Comments:
//		The CSparseArray is used exactly like a sparse array.  If you want to create a class
//		that handles its own memory allocation in a non-fragmentary way, but do not necessarily
//		want an actual array, check out CTINAllocator in GeomDBase\TIN.hpp.
//
//		The CSparseArray requires more effort to use, but allows multiple arrays of the same type
//		with simple array indexing.
//
//	Requires:
//		T must have the following functions:
//			bool bIsValid();		true when "this" is a valid object, else false.
//			void Invalidate();		invalidates "this" and handles all cleanup that would
//										normally be found in a destructor.
//
//		T's destructor is never called by the Sparse Array.
//
//**************************************
{
public:

	int iFirstFreeIndex;	// The first slot which is likely available for use.
	int iNumElements;		// Total number of elements in the array.

	//
	//	Member functions
	//


	// Allocate with a specified size.
	CSparseArray(uint u_max) : CMAArray<T>(u_max), iFirstFreeIndex(0), iNumElements(0)
	{
	}

	// Initialise with a newly created array, and size.
	CSparseArray(T* at_array, uint u_max, uint u_len = 0)
		: CMAArray<T>(u_max, at_array), iFirstFreeIndex(u_len), iNumElements(u_len)
	{
		uLen = u_len;
	}


	// The right way to add an element to the array is to new it into the array.
	inline friend void* operator new(uint u_size, CSparseArray<T>& sa)
	{
		Assert(u_size <= sizeof(T));

		Assert(sa.iFirstFreeIndex < sa.uMax);
		Assert(!(sa.atArray)[(sa.iFirstFreeIndex)].bIsValid());
		void *pv = &sa.atArray[sa.iFirstFreeIndex];

		sa.iNumElements++;


		// Update the free index and the length.
		if (sa.uLen <= sa.iFirstFreeIndex)
		{
			// The first free was at the end of the array.
			sa.uLen++;
			sa.iFirstFreeIndex = sa.uLen;
			return pv;
		}
		else
		{
			// There was a hole in the array.
			for (sa.iFirstFreeIndex++; sa.iFirstFreeIndex < sa.uMax; sa.iFirstFreeIndex++)
			{
				if (!(sa.atArray)[sa.iFirstFreeIndex].bIsValid())
				{
					// Found the next free slot!
					return pv;
				}
			}

			return pv;
		}
	}


	//******************************************************************************************
	//
	// Operators.
	//

	void operator <<(const T& t)
	{
		// Disable chaining, for now.
		Assert(false);
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Reset()
	//
	// Reset the array count to 0.
	//
	//**************************************
	{
		T t;
		t.Invalidate();

		Fill(t);
		uLen = 0;
		iFirstFreeIndex = 0;
		iNumElements = 0;
	}

	//******************************************************************************************
	//
	int iIndexOf(T* pt)
	//
	// Returns the index of pt in the array.  
	//
	//  Comments:
	//		Returns an invalid index if pt is out of the array bounds.
	//
	//**************************************
	{
		return (pt - atArray) / sizeof(T);
//				return int(static_cast<const T_TYPE*>(this) - ptFirst());
	}

	//******************************************************************************************
	//
	void Invalidate(int i_index)
	//
	// Informs the array that i_index is now available.
	//
	//**************************************
	{
		Assert(i_index >= 0 && i_index < uMax);
	
		iNumElements--;
		atArray[i_index].Invalidate();
		if (i_index < iFirstFreeIndex)
			iFirstFreeIndex = i_index;
	}

};