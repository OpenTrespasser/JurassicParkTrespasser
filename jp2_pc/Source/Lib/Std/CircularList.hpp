/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Classes associated with the CCircularList container type.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *		This container type uses an interface based on STL, and can be used in place of an
 *		STL map or set with minimal changes. Some STL functions are not implemented.
 *
 *		Many variables are public rather private; this is to allow for hand inlining when the
 *		compiler fails.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/CircularList.hpp                                              $
 * 
 * 5     97/10/30 7:03p Pkeet
 * Added the 'SetBegin' member function.
 * 
 * 4     97/10/30 11:13a Pkeet
 * Added 'const' to member functions of the node and list classes that should be const.
 * 
 * 3     97/10/29 4:00p Pkeet
 * Added the 'Clear' member function to the circular list class.
 * 
 * 2     8/28/97 4:06p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 3     8/22/97 7:19p Pkeet
 * Removed static variable initialization from the constructor.
 * 
 * 2     97/08/22 6:38p Pkeet
 * Added forward declarations.
 * 
 * 1     97/07/16 7:05p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_STD_CIRCULARBUFFER_HPP
#define HEADER_STD_CIRCULARBUFFER_HPP


//
// Forward declarations.
//
template<class N> class CCircularNodeIterator;
template<class T> class CCircularList;


//**********************************************************************************************
//
template<class T> class CCircularNode
//
// Prefix: node
//
// A node in the binary tree.
//
//**************************************
{
public:

	// Public type definitions.
	typedef T TData;
	typedef CCircularNode<T> TNode;
	typedef CCircularNodeIterator< CCircularNode<T> > iterator;
	typedef CCircularList<T> circular_list;

public:

	T Data;				// Data associated with the key.
	TNode* pnodeNext;	// Next link.
	TNode* pnodeLast;	// Last link.

private:

	//******************************************************************************************
	//
	// Private member functions.
	//

	//******************************************************************************************
	//
	void bind()
	//
	// Binds the pointers for the node.
	//
	//**************************************
	{
		Assert(pnodeNext);
		Assert(pnodeLast);

		pnodeLast->pnodeNext = this;
		pnodeNext->pnodeLast = this;
	}

	//******************************************************************************************
	//
	// Friend classes.
	//
	friend iterator;
	friend circular_list;

};


//**********************************************************************************************
//
template<class N> class CCircularNodeIterator
//
// Prefix: binit
//
// An iterator to move forward through the binary tree list. Uses an STL-style interface.
//
//**************************************
{
public:

	N* pnodeCurrent;	// Node the iterator is currently pointing to.

public:
	
	//******************************************************************************************
	//
	// Constructor.
	//

	// Default constructor.
	CCircularNodeIterator(N* pnode = 0)
		: pnodeCurrent(pnode)
	{
	}

	
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	typename N::TData operator*()
	//
	// Dereferences the data associated with the iterator.
	//
	//**************************************
	{
		return pnodeCurrent->Data;
    }

	//******************************************************************************************
	//
	typename N::TData operator*() const
	//
	// Dereferences the data associated with the iterator.
	//
	//**************************************
	{
		return pnodeCurrent->Data;
    }

	//******************************************************************************************
	//
	operator bool() const
	//
	// Returns 'true' if the current position of the iterator is valid.
	//
	//**************************************
	{
		return pnodeCurrent != 0;
    }

	//******************************************************************************************
	//
	bool operator ==(const CCircularNodeIterator<N>& it_compare) const
	//
	// Returns 'true' if the two iterators are equal.
	//
	//**************************************
	{
		return pnodeCurrent == it_compare.pnodeCurrent;
	}

	//******************************************************************************************
	//
	bool operator !=(const CCircularNodeIterator<N>& it_compare) const
	//
	// Returns 'true' if the two iterators are not equal.
	//
	//**************************************
	{
		return pnodeCurrent != it_compare.pnodeCurrent;
	}

	//******************************************************************************************
	//
	CCircularNodeIterator<N> operator ++()
	//
	// Incremements iterator to the next element in the list.
	//
	//**************************************
	{
		Assert(pnodeCurrent);

		pnodeCurrent = pnodeCurrent->pnodeNext;
		return *this;
	}

	//******************************************************************************************
	//
	CCircularNodeIterator<N> operator --()
	//
	// Decremements iterator to the last element in the list.
	//
	//**************************************
	{
		Assert(pnodeCurrent);

		pnodeCurrent = pnodeCurrent->pnodeLast;
		return *this;
	}

	//******************************************************************************************
	//
	CCircularNodeIterator<N> itNext() const
	//
	// Returns an iterator to the next element in the list.
	//
	//**************************************
	{
		Assert(pnodeCurrent);

		return CCircularNodeIterator<N>(pnodeCurrent->pnodeNext);
	}
};


//**********************************************************************************************
//
template<class T> class CCircularList
//
// Prefix: circ
//
// A binary tree container class with an STL-style interface.
//
//**************************************
{
public:

	// Public type definitions.
	typedef T TData;
	typedef CCircularNode<T> TNode;
	typedef CCircularNodeIterator< CCircularNode<T> > iterator;

private:

	// Object variables.
	iterator itBegin;				// Iterator representing the entry point into the circular
									// list.

	// Class variables.
	static TNode* pAllocator;		// Current allocation position in memory.
	static TNode* pBeginAllocation;	// Start allocation position in memory.
	static TNode* pMaxAllocation;	// Last available allocation position in memory.

public:

	int iNumNodes;			// The number of elements in the tree.

public:
	
	//******************************************************************************************
	//
	// Constructor.
	//

	// Default constructor.
	CCircularList()
		: iNumNodes(0)
	{
		pAllocator       = 0;
		pBeginAllocation = 0;
		pMaxAllocation   = 0;
	}

	// Default constructor.
	CCircularList(void* pv_allocator, int i_max_size)
		: iNumNodes(0)
	{
		pAllocator       = (TNode*)pv_allocator;
		pBeginAllocation = pAllocator;
		pMaxAllocation   = pAllocator + i_max_size;
	}

	
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	static uint uSizeOfElement()
	//
	// Returns an iterator to the first element in the list.
	//
	//**************************************
	{
		return sizeof(TNode);
	}

	//******************************************************************************************
	//
	iterator begin() const
	//
	// Returns an iterator to the first element in the list.
	//
	//**************************************
	{
		return itBegin;
	}

	//******************************************************************************************
	//
	iterator end() const
	//
	// Returns an iterator to the first element in the list.
	//
	//**************************************
	{
		return itBegin;
	}

	//******************************************************************************************
	//
	void push_back(const T& t)
	//
	// Inserts an element into the front of the circular list (behind the current begin
	// iterator).
	//
	//**************************************
	{
		insert(itBegin, t);
	}

	//******************************************************************************************
	//
	void push_front(const T& t)
	//
	// Inserts an element into the front of the circular list (ahead of the current begin
	// iterator).
	//
	//**************************************
	{
		itBegin = insert(itBegin, t);
	}

	//******************************************************************************************
	//
	iterator insert(const iterator& it, const T& t)
	//
	// Inserts an element after the iterated node on the list.
	//
	//**************************************
	{
		// Create a new node from the allocator memory.
		TNode* pnode = NewNode(t);
		Assert(pnode);

		// Is this the first node?
		if (iNumNodes++ < 1)
		{
			// Make this node the beginning node.
			itBegin.pnodeCurrent = pnode;
			pnode->pnodeNext = pnode;
			pnode->pnodeLast = pnode;
			return iterator(pnode);
		}

		//
		// Bind the node.
		//
		Assert(itBegin.pnodeCurrent);

		// Get the last and the next node.
		pnode->pnodeLast = it.pnodeCurrent->pnodeLast;
		pnode->pnodeNext = it.pnodeCurrent;

		// Assign the pointers for the last and next node.
		pnode->bind();

		return iterator(pnode);
	}

	//******************************************************************************************
	//
	void erase(iterator it)
	//
	// Erases and element in the list.
	//
	//**************************************
	{
		Assert(it.pnodeCurrent);
		Assert(it.pnodeCurrent->pnodeLast);
		Assert(it.pnodeCurrent->pnodeNext);
		Assert(iNumNodes > 1);

		// If the node is the beginning node, move the beginning node forward.
		if (itBegin == it)
			++itBegin;

		// Remove the node from its current position.
		it.pnodeCurrent->pnodeLast->pnodeNext = it.pnodeCurrent->pnodeNext;
		it.pnodeCurrent->pnodeNext->pnodeLast = it.pnodeCurrent->pnodeLast;

		// Decrement the node counter.
		iNumNodes--;
	}

	//******************************************************************************************
	//
	void SetBegin(iterator it)
	//
	// Sets the beginning of the list to the iterator value.
	//
	//**************************************
	{
		itBegin = it;
	}

	//******************************************************************************************
	//
	int size() const
	//
	// Returns true if there are no elements associated with the container.
	//
	//**************************************
	{
		return iNumNodes;
	}

	//******************************************************************************************
	//
	TNode* NewNode
	(
		const T& t
	)
	//
	// Allocates memory for a new node and initializes the node with default values.
	//
	//**************************************
	{
		Assert(pAllocator < pMaxAllocation);

		TNode* pnode;

		// Allocate memory.
		pnode = pAllocator++;

		// Initialize the node.
		pnode->pnodeNext     = 0;
		pnode->pnodeLast = 0;
		pnode->Data          = t;
		return pnode;
	}
	
	//******************************************************************************************
	//
	void Clear
	(
	)
	//
	// Clears list.
	//
	//**************************************
	{
		iNumNodes  = 0;
		pAllocator = pBeginAllocation;
	}

};

//**********************************************************************************************
//
// Static variables for memory allocation.
//
template<class T> typename CCircularList<T>::TNode* CCircularList<T>::pAllocator;
template<class T> typename CCircularList<T>::TNode* CCircularList<T>::pBeginAllocation;
template<class T> typename CCircularList<T>::TNode* CCircularList<T>::pMaxAllocation;


#endif // HEADER_STD_CIRCULARBUFFER_HPP
