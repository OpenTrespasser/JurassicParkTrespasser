/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CArrayT<T, A>			Template base class for defining arrays
 *			CSArray<T, iSize>		Simple array with static size.
 *			CPArray<T>				Simple pointer to array, with run-time length.
 *				CAArray<T>				Variant which owns and automatically deletes storage.
 *				CLArray(T, iSize)	Array with run-time size allocated on stack.
 *			CMArrayT<T, A>			Template base class for defining growable arrays.
 *				CMSArray<T, iMax>	Static array that is growable to a maximum.
 *				CMArray<T>			Growable array with run-time pre-allocated maximum.
 *					CMAArray<T>			Variant which owns and automatically deletes storage.
 *					CMLArray(T, iSize)	CLArray that is growable to a maximum.
 *
 * Bugs:
 *
 * To do:
 *		Change array size and index types from unsigned to signed.
 *		Add any other needed STL compatibility members.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/Array.hpp                                                     $
 * 
 * 46    9/08/98 8:55p Rwyatt
 * Added a new pdDup function which will duplicate the array to the specified address.
 * 
 * 45    8/26/98 3:27p Asouth
 * Removed const conversion operator; made non-const operator a const function
 * 
 * 44    98/05/01 15:51 Speter
 * Concatentation operator now performs placement new rather than assign. Added array
 * concatenation operator.
 * 
 * 43    3/10/98 1:19p Pkeet
 * Added include to "LocalArray.hpp."
 * 
 * 42    2/10/98 6:47p Agrant
 * Magic mojo according to Scott
 * 
 * 41    98/02/10 12:59 Speter
 * Re-organised array classes to allow new CSArray, which has constant static size.  Old
 * growable CSArray is now CMSArray.
 * 
 * 40    12/10/97 4:01p Mlange
 * Added assert.
 * 
 * 39    97/11/24 16:49 Speter
 * Changed CLArray and CMLArray to *not* initialise elements.  Added CLArrayInit which
 * initialises, like old CLArray.
 * 
 * 38    10/29/97 3:53p Mlange
 * The CMArray<T>::operator <<() function now no longer uses the global overloaded new operator
 * based on CMArray<T>, because it caused the compiler to generate slow and bloated code.
 * 
 * 37    9/03/97 7:00p Mlange
 * Added CSArray constructor for specifying initial length. Removed CSArray constructor taking
 * variable argument list because it clashed with this new constructor - but it wasn't used
 * anyway.
 * 
 * 36    97/07/23 17:54 Speter
 * Added uMemSize() function.  Removed Set() functions, as regular assignment works as well.
 * Added b_destruct parameter to Reset(), which destroys removed members.  Revamped CLArray
 * macros to invoke array operator new (for initialisation)
 * 
 * 35    97/07/15 12:55p Pkeet
 * Added 'CLMAArray' macro.
 * 
 * 34    7/09/97 3:22p Mlange
 * Fixed bug due to lifetime of temporary: CSCArray::operator[](int) const now returns a const
 * reference instead of a temporary.
 * 
 * 33    97/07/03 2:15p Pkeet
 * Added the 'CLArray' macro which uses CPArray in conjunction with _alloca to produce an array
 * that is destroyed when the function using it is destroyed.
 * 
 * 32    97/06/10 15:36 Speter
 * Changed Fill() function to be more efficient.  While here, finally added STL compatibility
 * members to CPArray.  Made CPArray and CSArray derive from a common parent.
 * 
 * 31    97/05/21 17:32 Speter
 * Added PArray convenience function.  Made order of arguments to CMArray constructor match that
 * of CPArray; added auto-alloc CMArray constructor.  Added CMArray::paAlloc for allocating
 * arrays.  Fixed some var names and comments.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_ARRAY_HPP
#define HEADER_LIB_STD_ARRAY_HPP

#include "Mem.hpp"
#include <stdarg.h>
#include <malloc.h>


//******************************************************************************************
//
// Global functions.
//

	//******************************************************************************************
	//
	template<class T> inline void Fill
	(
		T* pt_start,				// Starting address to fill.
		int i_count,				// Number of values.
		T t							// Value to write.
	)
	//
	// Fill the address with the given number of elements.
	//
	// Cross references:
	//		Called by various CPArray Fill() functions.
	//
	//**********************************
	{
		// Note: if/else causes VC4.2 to optimise out the test, switch/case doesn't.
		if (sizeof(T) == 1)
			memset8((uint8*)pt_start, (const uint8&)t, i_count);
		else if (sizeof(T) == 2)
			memset16((uint16*)pt_start, (const uint16&)t, i_count);
		else if (sizeof(T) == 4)
			memset32((uint32*)pt_start, (const uint32&)t, i_count);
		else
		{
			// Default slow implementation just writes values in a loop.
			while (--i_count >= 0)
				pt_start[i_count] = t;
		}
	}


// Forward declaration, for conversion operations.
template<class T> class CPArray;

//**********************************************************************************************
//
template<class T, class A> class CArrayT: public A
//
// Serves as a base class for various array classes.  Provides common element access functionality,
// while allowing the base class A to define atArray, size(), and capacity() as it chooses.  
//
// See CSArray and CPArray for examples.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Conversion operators.
	//

	operator T* () const // note: can break const-ness, use with caution
	{
		return atArray;
	}

	/* ARS 98.08.26: Removed to reduce errors with MW compiler
	operator const T* () const
	{
		return atArray;
	} */

	//******************************************************************************************
	//
	// Operators.
	//

	// Access the elements, with checking.
	T& operator [](int i)
	{
		Assert(atArray && i >= 0 && i < size());
		return atArray[i];
	}

	const T& operator [](int i) const
	{
		Assert(atArray && i >= 0 && i < size());
		return atArray[i];
	}

	// Provide uint versions, due to silly compiler errors.
	T& operator [](uint u)
	{
		Assert(atArray && u >= 0 && u < size());
		return atArray[u];
	}

	const T& operator [](uint u) const
	{
		Assert(atArray && u >= 0 && u < size());
		return atArray[u];
	}

	// Parenthesis operator indexes from end of array.
	T& operator ()(int i)
	{
		return operator [](size()+i);
	}

	const T& operator ()(int i) const
	{
		return operator [](size()+i);
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//
	// The following functions provide some compatibility with STL.
	//

	//******************************************************************************************
	//
	bool empty() const
	//
	// Returns:
	//		Whether the array has 0 elements.
	//
	//**********************************
	{
		return size() == 0;
	}

	typedef T value_type;
	typedef T* iterator;
	typedef const T* const_iterator;

	//******************************************************************************************
	//
	const T* begin() const
	//
	// Returns:
	//		The address of the beginning of the array.
	//
	//**********************************
	{
		return atArray;
	}

	//******************************************************************************************
	//
	T* begin()
	//
	// Returns:
	//		The address of the beginning of the array.
	//
	//**********************************
	{
		return atArray;
	}

	//******************************************************************************************
	//
	const T* end() const
	//
	// Returns:
	//		The address of the end of the array (after last element).
	//
	//**********************************
	{
		return atArray + size();
	}

	//******************************************************************************************
	//
	T* end()
	//
	// Returns:
	//		The address of the end of the array (after last element).
	//
	//**********************************
	{
		return atArray + size();
	}

	//******************************************************************************************
	//
	void Fill
	(
		int i_start,					// First element to fill.
		int i_count,					// Number of elements to fill
		T t								// Value.
	)
	//
	// Sets the specified elements to value t.
	//
	//**********************************
	{
		Assert(bWithin(i_start, 0, size()-1));
		Assert(bWithin(i_count, 0, size() - i_start));
		::Fill(atArray + i_start, i_count, t);
	}

	//******************************************************************************************
	//
	void Fill
	(
		T t
	)
	//
	// Sets every element to value t.
	//
	//**********************************
	{
		::Fill(atArray, int(size()), t);
	}

	//******************************************************************************************
	//
	uint uMemSize() const
	//
	// Returns:
	//		The total size of the array in bytes.
	//
	//**********************************
	{
		return size() * sizeof(T);
	}

	//******************************************************************************************
	//
	CPArray<T> paDup() const;
	//
	// Returns:
	//		Another array, with a new copy of the data.
	//
	//**********************************

	//******************************************************************************************
	//
	CPArray<T> paDup(T*) const;
	//
	// Returns:
	//		Another array, with a new copy of the data at the specified address
	//
	//**********************************
	
	//******************************************************************************************
	//
	CPArray<T> paSegment
	(
		int i_start,
		int i_len
	);
	//
	// Returns:
	//		Another array, referencing the given segment of this array.
	//
	//**********************************

	//******************************************************************************************
	//
	CPArray<T> paSegment
	(
		int i_start
	);
	//
	// Returns:
	//		Another array, referencing this array from u_start to the end.
	//
	//**********************************
};

//**********************************************************************************************
//
template<class T> class CPArrayBase
//
// Defines storage configuration for CPArray, an array with run-time allocation of length and
// storage.
//
//**************************************
{
public:
	uint	uLen;
	T*		atArray;

	int size() const
	{
		return uLen;
	}

	int capacity() const
	{
		return uLen;
	}
};

//**********************************************************************************************
//
template<class T> class CPArray: public CArrayT< T, CPArrayBase<T> >
//
// Prefix: pa
//
// A simple template class that basically just allows convenient declaration of a pointer to
// an array, and its length.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	CPArray()
	{
		atArray = 0;
		uLen = 0;
	}

	// Allocate with a specified size.
	CPArray(int i_len)
	{
		atArray = new T[i_len];
		uLen = i_len;
	}

	// Initialise with preallocated storage, and size.
	CPArray(int i_len, T* at_array)
	{
		atArray = at_array;
		uLen = i_len;
	}
};

//*********************************************************************************************
//
// CArrayT<> implementation.
// (These functions required CPArray<T> to be defined.
//

	//******************************************************************************************
	template<class T, class A> CPArray<T> CArrayT<T, A>::paDup() const
	{
		T* pt = new T[size()];

		return paDup(pt);
/*		// Copy the data.
		for (uint u = 0; u < size(); u++)
			pt[u] = atArray[u];

		return CPArray<T>(size(), pt);*/
	}

	//******************************************************************************************
	template<class T, class A> CPArray<T> CArrayT<T, A>::paDup(T* pt) const
	{
		// Copy the data.
		for (uint u = 0; u < size(); u++)
			pt[u] = atArray[u];

		return CPArray<T>(size(), pt);
	}

	//******************************************************************************************
	template<class T, class A> CPArray<T> CArrayT<T, A>::paSegment(int i_start, int i_len)
	{
		Assert(i_start >= 0 && i_start <= size());
		Assert(i_len >= 0 && i_start + i_len <= size());
		return CPArray<T>(i_len, atArray + i_start);
	}

	//******************************************************************************************
	template<class T, class A> CPArray<T> CArrayT<T, A>::paSegment(int i_start)
	{
		Assert(i_start >= 0 && i_start <= size());
		return CPArray<T>(size() - i_start, atArray + i_start);
	}

//******************************************************************************************
//
template<class T> inline CPArray<T> PArray
(
	int i_len,							// Number of elements in array.
	T* at_array							// Array data.
)
//
// Useful function which infers the array type, allowing you to avoid specifying it.
//
// Returns:
//		A CPArray<T> containing the given data.
//
//**************************************
{
	return CPArray<T>(i_len, at_array);
}

//******************************************************************************************
//
// ArrayData
// (
//		a_array
// )
//
// Macro which specifies both the number of array elements, and the array.
// Useful for initialising CPArray above.
//
//**************************
#define ArrayData(a_array)		iNumArrayElements(a_array), a_array


//**********************************************************************************************
//
template<class T> class CAArray: public CPArray<T>
//
// A CPArray that is owned by its created.  It is allocated upon initialisation, 
// and deleted upon destruction.
//
// Prefix: pa
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CAArray()
	{
	}

	// Allocate with a specified size.
	CAArray(int i_len)
		: CPArray<T>(i_len)
	{
	}

	// Initialise with a newly created array, and size.
	CAArray(uint i_len, T* at_array)
		: CPArray<T>(i_len, at_array)
	{
	}

	// Copy from a CPArray.  Should be newly created.
	CAArray(const CPArray<T>& pa)
		: CPArray<T>(pa)
	{
	}

	~CAArray()
	{
		delete[] atArray;
	}

	//******************************************************************************************
	//
	// Assignment operators.
	//

	CAArray<T>& operator =(const CPArray<T>& pa)
	{
		if (atArray != pa.atArray)
		{
			delete[] atArray;
			new(this) CAArray<T>(pa);
		}
		return *this;
	}

private:

	// Prevent copying CAArrays, because we don't want to give an array owned by one object
	// to another.

	CAArray(const CAArray<T>&);

	CAArray<T>& operator =(const CAArray<T>&);
};


//**********************************************************************************************
//
template<class T, int iSIZE> class CSArrayBase
//
// Defines storage configuration for CSArray.
//
//**************************************
{
public:
	T	atArray[iSIZE];

	int size() const
	{
		return iSIZE;
	}

	int capacity() const
	{
		return iSIZE;
	}

	//
	// Only default constructor.
	//
};

//**********************************************************************************************
//
template<class T, int iSIZE> class CSArray: public CArrayT<T, CSArrayBase<T, iSIZE> >
//
// Prefix: sa
//
// An array with statically declared size and storage.  Very similar to standard C array.
//
//**************************************
{
public:
	//
	// Only default constructor.
	//

	//******************************************************************************************
	//
	// Operators.
	//

	//******************************************************************************************
	//
	operator CPArray<T> ()
	//
	// Returns:
	//		A CPArray<T> referencing this entire array.
	//
	//**********************************
	{
		// Convert array to a CPArray.
		return CPArray<T>(size(), atArray);
	}
};

//**********************************************************************************************
//
// Various macros to simplify multi-dimensional CSArray declaration.
//

#define CSArray2(T, iSIZE1, iSIZE2) \
	CSArray<CSArray<T, iSIZE2>, iSIZE1>

#define CSArray3(T, iSIZE1, iSIZE2, iSIZE3) \
	CSArray<CSArray<CSArray<T, iSIZE3>, iSIZE2>, iSIZE1>

#define CSArray4(T, iSIZE1, iSIZE2, iSIZE3, iSIZE4)	\
	CSArray<CSArray<CSArray<CSArray<T, iSIZE4>, iSIZE3>, iSIZE2>, iSIZE1>

//
// Growable arrays.
//

//**********************************************************************************************
//
template<class T> inline void destruct
(
	T* pt
)
//
//**************************************
{
	pt->~T();
}

//**********************************************************************************************
//
template<class T, class A> class CMArrayT: public CArrayT<T, A>
//
// A templated base class for arrays which can grow up to a pre-allocated maximum.
// A should be a full array class such as CSArray<T> or CPArray<T>
//
//**************************************
{
public:
	uint		uMaxUsed;				// Maximum length attained, for informational purposes.

public:

	//******************************************************************************************
	//
	// Operators for growing.
	//

	//******************************************************************************************
	CMArrayT<T, A>& operator <<(const T& t)
	// Add an element with << operator.  Allow chaining together.
	{
		Assert(size() < capacity());
		new(&atArray[uLen]) T(t);
		uLen++;
		return *this;
	}

	//******************************************************************************************
	CMArrayT<T, A>& operator <<(CPArray<T> pa)
	// Add an array with << operator.  Allow chaining together.
	{
		Assert(size() + pa.size() <= capacity());
		for (int i = 0; i < pa.size(); i++)
			new(&atArray[uLen + i]) T(pa[i]);
		uLen += pa.size();
		return *this;
	}

	//******************************************************************************************
	CMArrayT<T, A>& operator -=(int i_elems)
	// Remove i_elems elements from the end of the array.
	{
		Assert(i_elems <= uLen);
		uLen -= i_elems;
		return *this;
	}

	//******************************************************************************************
	inline friend void* operator new(uint u_size, CMArrayT<T, A>& ma)
	{
		// Allocate one element at a time only, and only of type T.
		// Unfortunately, this is enforced only by checking u_size.
		Assert(u_size == sizeof(T));
		Assert(ma.size() < ma.capacity());
		ma.uLen++;
		return &ma.atArray[ma.uLen-1];
	}

	//******************************************************************************************
	//
	operator CPArray<T> ()
	//
	// Returns:
	//		A CPArray<T> referencing this entire array.
	//
	//**********************************
	{
		// Convert array to a CPArray.
		return CPArray<T>(size(), atArray);
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Reset
	(
		int i_len = 0,					// New length to reset array to.
		bool b_destruct = false			// Whether to call destructors for reset elements.
	)
	//
	// Reset the array, removing the elements beyond u_len.
	//
	// Compares the amount of memory previously used to set the 'uMaxUsed' member variable.
	// Memory allocated with this object will be invalid after this call. 
	// This function should be used instead of using "delete" for every object allocated.
	//
	//**********************************
	{
		//
		// Remember largest total allocation.
		//
		SetMax(uMaxUsed, uLen);

		Assert(i_len >= 0 && i_len <= uLen);
		if (b_destruct)
		{
			while (uLen > i_len)
			{
				uLen--;
				destruct(&atArray[uLen]);
			}
		}
		uLen = i_len;
	}

	//******************************************************************************************
	//
	CPArray<T> paAlloc
	(
		uint u_len = 0					// Length of sub-array to allocate.
	)
	//
	// Returns:
	//		A CPArray<T>, referencing a segment of this array of the specified length.
	//
	// Can be used in place of array new.
	//
	//**********************************
	{
		T* at_array = atArray + uLen;
		uLen += u_len;
		Assert(uLen <= capacity());
		return CPArray<T>(u_len, at_array);
	}

protected:

	//******************************************************************************************
	//
	// Member functions.
	//

	void SetLen(int i_len = 0)
	{
		Assert(i_len <= capacity());
		uLen = uMaxUsed = i_len;
	}

};

//**********************************************************************************************
//
template<class T, int iSIZE> class CMSArrayBase: public CSArrayBase<T, iSIZE>
//
// Defines storage configuration for CSArray.
//
//**************************************
{
public:
	uint		uLen;					// Current filled size.

	//******************************************************************************************
	//
	// Overrides.
	//

	int size() const
	{
		return uLen;
	}
};

//**********************************************************************************************
//
template<class T, int iMAX> class CMSArray: public CMArrayT<T, CMSArrayBase<T, iMAX> >
//
// Prefix: msa
//
// A CSArray whose size() can grow dynamically up to capacity().
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	CMSArray(int i_len = 0)
	{
		// Initial length.
		SetLen(i_len);
	}
};


//**********************************************************************************************
//
template<class T> class CMArrayBase: public CPArrayBase<T>
//
// Defines storage configuration for CMArray, an array with run-time allocation of capacity and
// storage.
//
//**************************************
{
public:
	uint	uMax;						// Add a maximum, in addition to the current size.

	//******************************************************************************************
	//
	// Overrides.
	//

	int capacity() const
	{
		return uMax;
	}
};

//**********************************************************************************************
//
template<class T> class CMArray: public CMArrayT< T, CMArrayBase<T> >
//
// A CPArray that is allowed to grow, up to a predefined maximum.
//
// Prefix: ma
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CMArray()
	{
		// No storage.
		atArray = 0;
		uMax = 0;
		SetLen(0);
	}

	// Initialise with a max, allocate array.
	CMArray(int i_max)
	{
		atArray = new T[i_max];
		uMax = i_max;
		SetLen(0);
	}

	// Initialise with a size and preallocated array.
	CMArray(int i_max, T* at_array)
	{
		atArray = at_array;
		uMax = i_max;
		SetLen(0);
	}
};

//**********************************************************************************************
//
template<class T> class CMAArray: public CMArray<T>
//
// A CMArray that is allocated upon initialisation, and deleted upon destruction.
//
// Prefix: pa
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Duplicate CMArray<> constructors.
	CMAArray()
	{
	}

	// Duplicate CMArray<> constructors.
	CMAArray(int i_max)
		: CMArray<T>(i_max)
	{
	}

	CMAArray(int i_max, T* at_array)
		: CMArray<T>(i_max, at_array)
	{
	}

	~CMAArray()
	{
		delete[] atArray;
	}

private:

	// Prevent copying CMArrays, because we don't want to give an array owned by one object
	// to another.

	CMAArray(const CMAArray<T>&);

	CMAArray<T>& operator =(const CMAArray<T>&);
};


//**********************************************************************************************
//
template<class T, uint uSize> class CSCArray
//
// A static circular array of type 'T' and size 'uSize'.
//
// Prefix: csa
//
// Notes:
//		This is a minimal implementation of a static circular buffer. Its size is constrained
//		to a power of two for efficiency and simplicity.
//
//		Note that this class requires no storage beyond that required to hold the data.
//
//**************************************
{
	T atArray[uSize];		// The data.

public:
	//******************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CSCArray()
	{
		Assert(bPowerOfTwo(uSize));
	}

	// Initialiser constructor.
	CSCArray(const T& t_init)
	{
		Assert(bPowerOfTwo(uSize));

		for (int u_index = 0; u_index < uSize; u_index++)
			atArray[u_index] = t_init;
	}


	//******************************************************************************************
	//
	// Operators.
	//

	// Access the elements.
	const T& operator[](int i_index) const
	{
		return atArray[i_index & (uSize - 1)];
	}

	T& operator[](int i_index)
	{
		return atArray[i_index & (uSize - 1)];
	}
};

#endif
