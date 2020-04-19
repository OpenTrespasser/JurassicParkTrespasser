/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		A specialized heap for fast allocations and deallocations.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *		The fast heap can be used by creating a CFastHeap object and calling the overloaded
 *		"new" operator as per the following example:
 *
 *			CFastHeap fh(1024, 8);
 *
 *			void FastHeapExample()
 *			{
 *				int* pi = new(&fhFastHeap) int[64];	// Allocate memory with the fast heap.
 *				...			// Do something with pi.
 *				fh.Reset();	// Resets the heap getting of any allocations, the actual memory
 *							// pages stay behind so the heap can be reused without having to
 *							// recommit memory.
 *			}
 *
 *		It is important to note that memory allocated with the fast heap object SHOULD NOT use
 *		"delete" to deallocate memory. Destructor members of classes cannot therefore be
 *		invoked when the class object was constructed with the fast heap.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/FastHeap.hpp                                                  $
 * 
 * 41    9/24/98 5:45p Asouth
 * new operator new [] for MW build
 * 
 * 40    98/09/14 1:43 Speter
 * CDArray now has fast allocation functions, that assert the memory is pre-committed. Added
 * bCommit() to commit additional memory. Changed pvCommitEnd pointer to uCommit index, and made
 * public, like uLen.
 * 
 * 39    8/25/98 4:39p Rwyatt
 * Adjusted fast heap reset function
 * 
 * 38    8/25/98 2:13p Rwyatt
 * Added always assert in none final mode to detect allocations at the end of a heap
 * 
 * 37    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 36    98/05/01 15:51 Speter
 * Concatentation operator now performs placement new rather than assign. Added array
 * concatenation operator.
 * 
 * 35    12/21/97 10:55p Rwyatt
 * Reset member functions of CDArray and CFastHeap now take a maximum commited parameter, any
 * memory is the heap above this limit is decommitted and returned to the free pool
 * 
 * 34    12/17/97 12:16p Mlange
 * CDArray now does not commit memory by default.
 * 
 * 33    12/16/97 3:39p Mlange
 * CDArray now commits memory as it is allocated.
 * 
 * 32    12/04/97 4:09p Rwyatt
 * Heap allocate function now takes a bool to control commiting of the memory.
 * CDArrays by default commit all of their memory, CFastArray does not commit memory until it is
 * newed
 * 
 * 31    10/03/97 5:17p Rwyatt
 * Removed global memory functions from this file and put the in std/mem.hpp
 * 
 * 30    10/02/97 5:41p Rwyatt
 * Added global function to give the virtual status of a block of memory
 * 
 * 29    9/18/97 3:47p Rwyatt
 * Added global functions to return system memory info/state
 * 
 * 28    97/07/23 17:52 Speter
 * Heap again has default alignment of 'int', enforced through rounding up allocation requests;
 * Align() still rounds to larger boundaries.  Made some new funcs const.  Added new() operator
 * with alignment argument.  Added paAlloc() macro.
 * 
 * 27    7/22/97 1:53p Mlange
 * Added CFastHeap::uNumBytesUsed() and CFastHeap::uNumBytesFree().
 * 
 * 26    7/09/97 1:54p Mlange
 * Added CFastHeap::uGetSize().
 * 
 * 25    97/05/25 17:52 Speter
 * Changed constructor to work around compiler bug with nested types.
 * Removed commented-out code.
 * 
 * 24    97/05/21 17:33 Speter
 * Updated for new CMArray argument order.  Removed commented-out stuff.
 * 
 * 23    97-04-09 17:11 Speter
 * Made CDArray inherit from CMArray; commented out pdaOwner capability, since no one needs it
 * currently.
 * 
 * 22    97-04-03 18:53 Speter
 * Oops.
 * 
 * 21    97-04-03 17:18 Speter
 * Added capability for CDArray to be constructed based on an existing one.
 * 
 * 20    3/19/97 1:16p Mlange
 * Reinstated CFastheap::pvAllocate().
 * 
 * 19    97/03/18 19:35 Speter
 * Fixed some errors.
 * 
 * 18    97/03/18 19:19 Speter
 * Re-organised CDArray and CFastHeap classes.  CDArray is now simpler.  CFastHeap is now
 * derived from CDArray, and is also simpler.
 * 
 * 17    97/02/03 11:03 Speter
 * In CFastHeap, separated Align from pvAllocate, to allow unallocated align.  Added Deallocate
 * function.
 * In CDArray, made allocate unaligned memory from heap (faster, better).  Added -= operator.
 * 
 * 16    97/01/29 18:33 Speter
 * Replaced CFastHeap += operator with pvAllocate(), which returns aligned memory.  Made
 * new(CFastHeap) and new(CDArray) call this function.  Moved new(CDArray) definition out of
 * class.
 * 
 * 15    97/01/20 11:31 Speter
 * Changed CDArray::Reset() function to take index parameter.
 * 
 * 14    96/12/31 16:39 Speter
 * Disabled CDArray += operator (see comment), made new(CDArray) operator call heap's new rather
 * than heap's array new.
 * 
 * 13    96/10/04 16:59 Speter
 * Extended CDArray, so that it can be placed on a pre-existing heap.
 * 
 * 12    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 11    96/08/02 11:23 Speter
 * Made CDArray inherit from CPArray, moved from Array.hpp to FastHeap.hpp.  Removed #include
 * "FastHeap.hpp" from Array.hpp.
 * 
 * 10    96/07/22 15:25 Speter
 * Added Reset() function taking position parameter.
 * Now initialise pu1CurrentAlloc in constructor.
 * 
 * 9     7/17/96 3:56p Mlange
 * The Reset member function is now no longer virtual. The member functions pvGetHeapBase() and
 * pvGetCurrentAlloc() are now public.
 * 
 * 8     7/16/96 6:01p Mlange
 * Made the Reset member function virtual because CFastHeapArray needs to overide it.
 * 
 * 7     7/15/96 7:04p Mlange
 * Made the member functions pvGetHeapBase() and pvGetCurrentAlloc() const and protected.
 * Updated the todo list.
 * 
 * 6     96/07/03 12:58 Speter
 * Moved many files to new directories, changed include statements.
 * 
 * 5     7/01/96 6:28p Mlange
 * Changed pointers to uint8* instead of void*. Fixed implementation of get current alloc
 * location function.
 * 
 * 4     7/01/96 6:16p Pkeet
 * Added member functions to access protected member values. Changed char*'s to void*'s.
 * 
 * 3     6/26/96 12:29p Mlange
 * Removed assert checking for zero size allocations.
 * 
 * 2     6/12/96 6:59p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_SYS_FASTHEAP_HPP
#define HEADER_LIB_SYS_FASTHEAP_HPP

//
// Includes.
//
#include "Lib/Sys/Errors.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/Std/MemLimits.hpp"


//**********************************************************************************************
//
// Global functions used by CDArray below.  Should not be used by others.
//

//**********************************************************************************************
//
void* pHeapAlloc
(
	uint u_heap_size					// Number of bytes to allocate.
);
//
// Returns:
//		Address of RESERVED virtual memory block. Use u4CommitMem() function to commit memory
//		before use, otherwise an access violation will occur.
//
//**************************************


//**********************************************************************************************
//
void HeapFree
(
	void* p_heap, 
	uint u_heap_size
);
//
// Frees and decommits memory allocated with pHeapAlloc.
//
//**************************************


//**********************************************************************************************
//
void* pvCommitMem
(
	void* pv_start,		// Start of block to commit.
	uint32 u4_bytes		// Number of bytes to commit.
);
//
// Commits memory reserved by pHeapAlloc.
//
// Returns:
//		Pointer to end of memory block that was actually commited, rounded by the page size.
//
//**************************************


//**********************************************************************************************
//
void* pvDecommitMem
(
	void* pv_start,		// Start of block to decommit.
	uint32 u4_bytes		// Number of bytes to decommit.
);
//
// Decommits memory reserved by pHeapAlloc.
//
// Returns:
//		Pointer to end of new committed memory block, rounded by the page size.
//
//**************************************



//**********************************************************************************************
//
template<class T> class CDArray : public CPArray<T>
//
// Prefix: da
//
// A dynamic array that is almost infinitely growable, but does not actually take up memory
// until it is accessed.
//
// Notes:
//		The total number of elements allocated can not exceed 'u_max_elements', as the heap does
//		not grow! Users are expected to allocate much more heap memory than they will actually
//		need. Because physical memory is not actually used until a page of heap memory is
//		first commited, this array will not consume unnecessary physical memory,
//		and is therefore as efficient as a growing heap in allocating memory but faster.
//
//**************************************
{
public:
	uint uMax;					// The maximum size this can grow to.
	uint uCommit;				// The current committed size.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CDArray(uint u_max_elements, bool b_commit = false)
		: uMax(u_max_elements)
	{
		static_cast<CPArray<T>&>(*this) = CPArray<T>(u_max_elements, (T*)pHeapAlloc(u_max_elements * sizeof(T)));
		uLen = 0;
		uCommit = 0;

		if (b_commit)
			CommitToOffset(uMax);

		MEMLOG_ADD_COUNTER(emlCDArray, u_max_elements * sizeof(T) );
	}


	virtual ~CDArray()
	{
		HeapFree(atArray, uMax * sizeof(T));

		MEMLOG_SUB_COUNTER(emlCDArray, uMax * sizeof(T) );
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Add an element with << operator.  Allow chaining together.
	// Visual C++ 5.0sp3 doesn't like it when this is used for some unknowable reason.
	CDArray<T>& operator <<(const T& t)
	{
		Grow(1);

		// Use placement new rather than simple assignment, as we're assigning to raw memory.
		new(&atArray[uLen - 1]) T(t);

		return *this;
	}

	// Add an element with << operator.  Allow chaining together.
	CDArray<T>& daAdd(const T& t)
	{
		Grow(1);

		// Use placement new rather than simple assignment, as we're assigning to raw memory.
		new(&atArray[uLen - 1]) T(t);

		return *this;
	}

	// Add an array with << operator.  Allow chaining together.
	CDArray<T>& operator <<(CPArray<T> pa)
	{
		Grow(pa.uLen);

		for (int i = 0; i < pa.uLen; i++)
			new(&atArray[uLen - pa.uLen + i]) T(pa[i]);

		return *this;
	}

	CDArray<T>& operator -=(uint u_elems)
	// Remove u_elems elements from the end of the array.
	{
		Assert(u_elems <= uLen);

		uLen -= u_elems;

		return *this;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Grow(uint u_elems)
	//
	// Add u_elems elements to the end of the array.
	//
	//**********************************
	{
		uLen += u_elems;
#if VER_TEST
		AlwaysAssert(uLen <= uMax);
#endif
		CommitToOffset(uLen);
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
	// Can be used in place of array new. However, note that it returns raw memory.
	//
	//**********************************
	{
		T* at_array = atArray + uLen;

		Grow(u_len);

		return CPArray<T>(u_len, at_array);
	}

	//******************************************************************************************
	//
	T* ptFastAlloc()
	//
	// Returns:
	//		A pointer to a new element in this array.
	//
	// Returns raw memory. Does not call CommitToOffset; sufficient memory must already have been committed.
	//
	//**********************************
	{
		uLen ++;
		Assert(uLen <= uMax);
		Assert(uLen <= uCommit);

		return atArray + uLen - 1;
	}

	//******************************************************************************************
	//
	CPArray<T> paFastAlloc
	(
		uint u_len						// Length of sub-array to allocate.
	)
	//
	// Returns:
	//		A CPArray<T>, referencing a new segment of this array of the specified length.
	//
	// Returns raw memory. Does not call CommitToOffset; sufficient memory must already have been committed.
	//
	//**********************************
	{
		T* at_array = atArray + uLen;

		uLen += u_len;
		Assert(uLen <= uMax);
		Assert(uLen <= uCommit);

		return CPArray<T>(u_len, at_array);
	}

	//******************************************************************************************
	//
	void Reset
	(
		uint	u_len = 0,					// New length to reset array to.
		bool	b_destruct = false,			// Whether to call destructors for reset elements.
		uint	u_decommit = 0xffffffff		// number of elements to leave committed memory
	)
	//
	// Reset the array, removing the elements beyond u_len.
	//
	// Memory allocated with this object will be invalid after this call. 
	// This function should be used instead of using "delete" for every object allocated.
	//
	//**********************************
	{
		Assert(u_len <= uLen);
		Assert(u_decommit>=u_len);

		if (b_destruct)
		{
			while (uLen > u_len)
			{
				uLen--;
				destruct(&atArray[uLen]);
			}
		}

		uLen = u_len;

		// if the default parameter is passed, do not alter the amount of commited memory.
		if (u_decommit == 0xffffffff)
			return;

		// if we get to here our current committed elements is above the specified maximum
		// so we need to remove some memory pages.
		Assert(u_decommit<uMax);
		Assert(u_decommit>=u_len);
		DecommitAboveOffset(u_decommit);
	}


	//*****************************************************************************************
	//
	bool bCommit
	(
		uint u_len
	)
	//
	// Commit given # of additional elements at end of array.
	//
	// Returns:
	//		Whether successful.
	//
	//**********************************
	{
		CommitToOffset(uLen + u_len);
		return uLen + u_len <= uMax;
	}

private:

	//*****************************************************************************************
	//
	void CommitToOffset
	(
		uint u_offset
	)
	//
	// Commit given # of elements at end of array.
	//
	//**********************************
	{
		if (u_offset <= uCommit)
			return;

		void* pv = pvCommitMem(atArray + uCommit, (u_offset - uCommit) * sizeof(T));
		uCommit = (T*)pv - atArray;
	}

	//*****************************************************************************************
	//
	void DecommitAboveOffset
	(
		uint u_offset
	)
	//
	// Decommit above given # of elements at end of array.
	//
	//**********************************
	{
		// if we do not currently have enough memory commited to meet the request then do
		// nothing as there is no memory to decommit
		if (u_offset >= uCommit)
			return;

		void* pv = pvDecommitMem(atArray + u_offset, (uCommit+1 - u_offset) * sizeof(T) - 1);
		uCommit = (T*)pv - atArray;
	}

};

// Global new operator function based on CDArray.
template<class T> inline void* operator new(uint u_size, CDArray<T>& ma)
{
	// Make sure we're allocating an object of the proper type.
	Assert(u_size == sizeof(T));

	ma.Grow(1);

	return ma.atArray + (ma.uLen - 1);
}



// The default heap alignment used.
#define uHEAP_ALIGNMENT	sizeof(int)

//*********************************************************************************************
//
class CFastHeap: private CDArray<char>
//
// Prefix: fh
//
// An object to make fast memory allocations and deallocations for temporary use. 
//
// Notes:
//		This heap uses an uncommited CDArray<char> to implement its functionality.
//		It provides its own	overloaded "new" operator, which can allocate objects of any type, 
//		and which aligns memory first. Memory is commited as it is allocated so the amount of
//		physical memory used is detatched from the size of the heap.
//
//		Do not use the delete operator on these objects. Instead, all memory allocated by 
//		the heap is removed when the heap is destroyed, calling the Reset() member only sets
//		the allocation pointers back the start and the memory is not decommited.
//
//		The total amount of memory allocated can not exceed 'uFastHeapSize' as the heap does
//		not grow! Users are expected to allocate much more heap memory than they will actually
//		need. Because physical memory is not actually allocated used until a block of heap 
//		memory is newed, this heap allocator will not consume unnecessary physical memory,
//		and is therefore as efficient as a growing heap in allocating memory but faster.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	//*****************************************************************************************
	CFastHeap
	(
		uint u_heap_size
	)
		// do not commit the memory in the CDArray - it will get commited as it is allocated
		: CDArray<char>(RoundUp(u_heap_size, uHEAP_ALIGNMENT), false)
	{
		Assert((int)atArray % uHEAP_ALIGNMENT == 0);

		MEMLOG_ADD_COUNTER(emlCFastHeap, u_heap_size );
		MEMLOG_SUB_COUNTER(emlCDArray, u_heap_size );
	}


	//*****************************************************************************************
	// destructor only tracks the memory usage
	~CFastHeap
	(
	)
	{
		MEMLOG_SUB_COUNTER(emlCFastHeap, uGetSize() );
		MEMLOG_ADD_COUNTER(emlCDArray, uGetSize() );
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void* pvGetBase() const
	//
	// Gets the address of the beginning of the heap.
	//
	//**************************************
	{
		return atArray;
	}

	//*****************************************************************************************
	//
	void* pvGetCurrentAlloc() const
	//
	// Gets the current value of the heap allocation pointer.
	//
	//**************************************
	{
		return atArray + uLen;
	}

	//*****************************************************************************************
	//
	uint uGetSize() const
	//
	// Returns:
	//		The total size of the heap, in bytes.
	//
	//**************************************
	{
		return uMax;
	}

	//******************************************************************************************
	//
	uint uNumBytesUsed() const
	//
	// Returns:
	//		The total number of bytes reserved from the fast heap.
	//
	//**********************************
	{
		return uLen;
	}

	//******************************************************************************************
	//
	uint uNumBytesFree() const
	//
	// Returns:
	//		The total number of bytes available on the fast heap.
	//
	//**********************************
	{
		return uMax - uLen;
	}

	//*****************************************************************************************
	//
	void Reset
	(
		void* p_reset_position,				// Address to reset to or NULL for start
		uint32 u4_decommit = 0xffffffff		// number of bytes to leave committed
	)
	//
	// Resets the current allocator pointer to p_reset_position.
	//
	//**************************************
	{
		Assert(atArray);

		// If zero is passed for the address get the heap base address
		if (p_reset_position == 0)
			p_reset_position = pvGetBase();

		Assert(bWithin((char*)p_reset_position, atArray, atArray + uLen));
		Assert((int)p_reset_position % uHEAP_ALIGNMENT == 0);

		CDArray<char>::Reset((char*)p_reset_position - atArray,false,u4_decommit);
	}


	//*****************************************************************************************
	//
	void Reset
	(
		uint32	u4_decommit = 0xffffffff		// the number of bytes to leave commited
	)
	//
	// Reset heap to beginning.
	//
	//**************************************
	{
		CDArray<char>::Reset(0,false,u4_decommit);
	}


	//*****************************************************************************************
	//
	void* pvAllocate
	(
		uint u_size_type
	)
	//
	// Allocates the requested size.
	//
	// Returns:
	//		A pointer to the piece of memory.
	//
	// Notes:
	//		Rounds the requested size up to keep the heap aligned to uHEAP_ALIGNMENT.
	//		Caller must call Align() first if he wants memory aligned beyond this.
	//
	//**************************************
	{
		// Remember the current position.
		void* pv_return_value = reinterpret_cast<void*>(atArray + uLen);

		Assert((int)pv_return_value % uHEAP_ALIGNMENT == 0);

		Grow(RoundUp(u_size_type, uHEAP_ALIGNMENT));

		return pv_return_value;
	};


	//*****************************************************************************************
	//
	void Align
	(
		uint u_alignment					// The quantum to align up to.
	)
	//
	// Rounds the current allocator pointer up to the next alignment boundary.
	//
	//**************************************
	{
		//
		// Align current allocator to the next available block of memory.
		//
		Assert(bPowerOfTwo(u_alignment));
		Assert(u_alignment % uHEAP_ALIGNMENT == 0);

		uint u_len_curr = uLen;
		u_len_curr += u_alignment - 1;
		u_len_curr &= ~(u_alignment - 1);
		Grow(u_len_curr - uLen);
	};

	//Hides the operator from the base class
	//but CFastHeap has private inheritance from that class
	//and the operator is needed often
	operator char* () const
	{
		return CDArray<char>::operator char*();
	}

	friend void* operator new(uint u_size_type, CFastHeap& fh_heap);
	friend void* operator new(uint u_size_type, CFastHeap& fh_heap, uint u_alignment);

	friend void* operator new [] (uint u_size_type,	CFastHeap& fh_heap);
	friend void* operator new [] (uint u_size_type, CFastHeap& fh_heap, uint u_alignment);
};


//
// Global function based on CFastHeap.
//

//*********************************************************************************************
//
inline void* operator new
(
	uint       u_size_type,	// Size of object to be allocated in bytes.
	CFastHeap& fh_heap		// Pointer to the fast heap object making the memory allocation.
)
//
// Overloaded new operator to use the CFastHeap object.
//
// Returns:
//		Returns the address of the memory block associated with the created object.
//
// Notes:
//		This function will get the next available block of memory from the associated CFastHeap
//		object and then increment the pointer to the next available block of memory. The
//		pointer to the next available block of memory will then be rounded up using the
//		CFastHeap::Align() function so that it aligns on a specified alignment boundary.
//
//**************************************
{
	return fh_heap.pvAllocate(u_size_type);
}



//*********************************************************************************************
//
inline void* operator new
(
	uint		u_size_type,	// Size of object to be allocated in bytes.
	CFastHeap&	fh_heap,		// Pointer to the fast heap object making the memory allocation.
	uint		u_alignment		// Byte alignment of new memory.
)
//
// Similar to new operator above, but aligns memory first.
//
//**************************************
{
	fh_heap.Align(u_alignment);
	return fh_heap.pvAllocate(u_size_type);
}


inline void* operator new []
(
	uint       u_size_type,	// Size of object to be allocated in bytes.
	CFastHeap& fh_heap		// Pointer to the fast heap object making the memory allocation.
)
{
	return fh_heap.pvAllocate(u_size_type);
}


inline void* operator new []
(
	uint		u_size_type,	// Size of object to be allocated in bytes.
	CFastHeap&	fh_heap,		// Pointer to the fast heap object making the memory allocation.
	uint		u_alignment		// Byte alignment of new memory.
)
{
	fh_heap.Align(u_alignment);
	return fh_heap.pvAllocate(u_size_type);
}





#endif
