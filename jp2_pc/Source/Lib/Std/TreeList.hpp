/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Classes associated with the CTreeList container type.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *		This container type uses an interface based on STL, and can be used in place of an
 *		STL map or set with minimal changes. Some STL functions are not implemented.
 *
 *		The advantage of this container over an associative STL container is that it provides
 *		for faster allocations, inserts, deletions and iteration; this comes at the expense
 *		of STL's more flexible memory management scheme and ability to maintain balanced
 *		red-black trees.
 *
 *		Many variables are public rather private; this is to allow for hand inlining when the
 *		compiler fails.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/TreeList.hpp                                                  $
 * 
 * 11    9/22/98 10:40p Pkeet
 * Forced inline all member functions.
 * 
 * 10    98.01.05 5:15p Mmouni
 * Added seperate node allocation routine.
 * 
 * 9     12/21/97 10:56p Rwyatt
 * The SetAllocator function now takes a CFastHeap pointer as well as the block of memory the
 * list is to use. If the allocator runs out and a fast heap is specified then the local memory
 * within the allocator will be extended.
 * 
 * 8     8/28/97 4:06p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 8     97/08/22 6:38p Pkeet
 * Added forward declarations.
 * 
 * 7     97/08/13 2:40p Pkeet
 * Provided a temporary fix for the mystery infinite loop bug.
 * 
 * 6     97/05/28 2:54p Pkeet
 * Added offsets into structures for use with assembly.
 * 
 * 5     97/05/27 3:57p Pkeet
 * Changed some private functions and variables to public ones.
 * 
 * 4     97/05/16 7:02p Pkeet
 * Added the insert_fast member function.
 * 
 * 3     97/05/15 5:04p Pkeet
 * Now uses placement information for iterators.
 * 
 * 2     97/05/14 8:13p Pkeet
 * Fixed bug in multiple list use.
 * 
 * 1     97/05/14 4:02p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_STD_TREELIST_HPP
#define HEADER_STD_TREELIST_HPP

#include "Lib/Sys/FastHeap.hpp"

#define iDATA_OFFSET (4)
#define iNEXT_OFFSET (20)


//
// Forward declarations.
//
template<class N> class CTreeListIterator;
template<class N> class CTreeListReverseIterator;
template<class N> class CTreeListBinaryIterator;
template<class K, class T> class CTreeList;


//**********************************************************************************************
//
template<class K, class T> class CTreeListNode
//
// Prefix: node
//
// A node in the binary tree.
//
//**************************************
{
public:

	// Public type definitions.
	typedef K TKey;
	typedef T TData;
	typedef CTreeListNode<K, T> TNode;
	typedef CTreeListIterator<TNode> iterator;
	typedef CTreeListReverseIterator<TNode> reverse_iterator;
	typedef CTreeListBinaryIterator<TNode> binary_iterator;
	typedef CTreeList<K, T> tree_list;

public:

	K Key;					// Key for the node.
	T Data;					// Data associated with the key.
	TNode* pnodeLess;		// Child with a lesser key value.
	TNode* pnodeGreater;	// Child with a greater key value.
	TNode* pnodeParent;		// Parent to node.
	TNode* pnodeNext;		// Next link.
	TNode* pnodePrevious;	// Previous link.
	bool   bSkip;			// Flag to pretend this node is empty.

public:

	//******************************************************************************************
	//
	// Public member functions.
	//

	//******************************************************************************************
	//
	forceinline void SetNextPointer
	(
		TNode* pnode_next	// Value to set the pointer to.
	)
	//
	// Sets the 'pnodeNext' member variable.
	//
	// Notes:
	//		This function was put into place to try and find a mystery bug in debug mode, and
	//		preventing the mystery bug from hanging the machine in release mode.
	//
	//**************************************
	{
		Assert(pnode_next != this);

		// Make the pointer null if an infinite loop condition will occur.
		if (pnode_next == this)
			pnodeNext = 0;
		else
			pnodeNext = pnode_next;
	}

private:

	//******************************************************************************************
	//
	// Private member functions.
	//

	//******************************************************************************************
	//
	forceinline void Set
	(
		iterator&         it_begin,
		reverse_iterator& revit_begin
	)
	//
	// Inserts a new node into the tree and list.
	//
	//**************************************
	{
		for (binary_iterator bit(this);;)
		{
			++bit;

			// Break if there are no more nodes to check.
			if (!bit.pnodeCurrent)
			{
				revit_begin.SetAsBeginning(this);
				break;
			}

			// Do nothing with this node if it is marked invisible.
			if (bit.pnodeCurrent->bSkip)
				continue;

			// Valid node found.
			if (!bit.pnodeCurrent->pnodePrevious)
			{
				//
				// If there is no previous node to this one, it must be the first node in the
				// list.
				//
				it_begin.SetAsBeginning(this);
			}
			else
			{
				bit.pnodeCurrent->InsertBefore(this);
			}
			if (!pnodeNext)
			{
				// If there are no nodes left, this must be the last node in the list.
				revit_begin.SetAsBeginning(this);
			}
			break;
		}
		Assert(bValidateOrder());
	}

	//******************************************************************************************
	//
	forceinline void InsertBefore
	(
		TNode* pnode	// Node to insert before the current node.
	)
	//
	// Inserts a new node into the list before the current node.
	//
	//**************************************
	{
		Assert(pnode);

		// Set the node's pointers.
		pnode->SetNextPointer(this);
		pnode->pnodePrevious = pnodePrevious;
		if (pnodePrevious)
			pnodePrevious->SetNextPointer(pnode);
		pnodePrevious = pnode;

		Assert(bValidateOrder());
		Assert(pnode->bValidateOrder());
	}

	//******************************************************************************************
	//
	forceinline iterator InsertNode
	(
		CTreeListNode<K, T>* pnode,
		iterator&            it_begin,
		reverse_iterator&    revit_begin,
		bool&                b_less,
		bool&                b_gt
	)
	//
	// Inserts a new node into the tree and list.
	//
	// Notes:
	//		This function is to be commented and optimized.
	//
	//**************************************
	{
		Assert(pnode);

		if (Key(pnode->Key, Key))
		{
			b_gt = false;
			if (pnodeLess)
				return pnodeLess->InsertNode(pnode, it_begin, revit_begin, b_less, b_gt);
			else
			{
				pnodeLess = pnode;
				pnode->pnodeParent = this;

				if (b_less)
				{
					it_begin.SetAsBeginning(pnode);
				}
				else
					pnode->Set(it_begin, revit_begin);
				return iterator(pnode);
			}
		}

		if (Key(Key, pnode->Key))
		{
			b_less = false;
			if (pnodeGreater)
				return pnodeGreater->InsertNode(pnode, it_begin, revit_begin, b_less, b_gt);
			else
			{
				pnodeGreater = pnode;
				pnode->pnodeParent = this;

				if (b_gt)
				{
					revit_begin.SetAsBeginning(pnode);
				}
				else
					pnode->Set(it_begin, revit_begin);
				return iterator(pnode);
			}
		}

		Assert(bSkip);
		Data = pnode->Data;
		bSkip = false;
		Set(it_begin, revit_begin);
		return iterator(this);
	}

	//******************************************************************************************
	//
	forceinline iterator itFind
	(
		const K& k	// Key to find element associated with.
	)
	//
	// Recursively searches for the element that matches the key.
	//
	//**************************************
	{
		// If the key is less than the current key, explore the less than path.
		if (Key(k, Key))
		{
			if (pnodeLess)
				return pnodeLess->itFind(k);
			return iterator();
		}

		// If the key is greater than the current key, explore the greater than path.
		if (Key(Key, k))
		{
			if (pnodeGreater)
				return pnodeGreater->itFind(k);
			return iterator();
		}

		// The key equals this node...
		if (bSkip)
			return iterator();
		return iterator(this);
	}

	//******************************************************************************************
	//
	forceinline void DeleteNode
	(
		iterator& it_begin,
		reverse_iterator& revit_begin
	)
	//
	// Removes this node from the list.
	//
	//**************************************
	{
		Assert(bValidateOrder());

		// Bind the next pointer.
		if (pnodeNext)
		{
			pnodeNext->pnodePrevious = pnodePrevious;
		}
		else
			revit_begin = reverse_iterator(pnodePrevious);

		// Bind the previous pointer.
		if (pnodePrevious)
		{
			pnodePrevious->SetNextPointer(pnodeNext);
		}
		else
			it_begin = iterator(pnodeNext);

		// Invalidate the node.
		bSkip = true;
		SetNextPointer(0);
		pnodePrevious = 0;

		Assert((pnodeNext && pnodeNext->bValidateOrder()) || !pnodeNext);
		Assert((pnodePrevious && pnodePrevious->bValidateOrder()) || !pnodePrevious);

		//
		// Remove node if unnecessary.
		//
		/*
		if (pnodeParent)
		{
			if (!(pnodeLess || pnodeGreater))
			{
				if (pnodeParent->pnodeLess == this)
					pnodeParent->pnodeLess = 0;
				if (pnodeParent->pnodeGreater == this)
					pnodeParent->pnodeGreater = 0;
				return;
			}

			TNode* pnode = pnodeLess;
			if (pnode)
			{
				if (pnodeGreater)
					return;
			}
			else
				pnode = pnodeGreater;
			if (pnodeParent->pnodeLess == this)
				pnodeParent->pnodeLess = pnode;
			else
				pnodeParent->pnodeGreater = pnode;
		}
		*/
	}

	//******************************************************************************************
	//
	forceinline bool bValidateOrder()
	//
	// Returns 'true' if the linked list links are valid at this node.
	//
	// Notes:
	//		This function is for debugging purposes only.
	//
	//**************************************
	{
		bool b_retval = !bSkip;

		if (pnodeNext)
			b_retval = b_retval && !pnodeNext->bSkip;

		if (pnodePrevious)
			b_retval = b_retval && !pnodePrevious->bSkip;

		b_retval = b_retval && this != pnodePrevious;
		b_retval = b_retval && this != pnodeNext;

		if (pnodePrevious)
			b_retval = b_retval && pnodePrevious != pnodeNext;

		// Return the results.
		return b_retval;
	}
	
	//******************************************************************************************
	//
	forceinline TNode* pnodeGetNextBinary
	(
		const K& key	// Key to find the node after.
	)
	//
	// Returns the next node after the the given key.
	//
	//**************************************
	{
		if (pnodeLess)
			if (Key(key, pnodeLess->Key)) // key < pnodeLess->Key
				return pnodeLess->pnodeGetNextBinary(key);

		if (Key(key, Key)) // key < Key
			return this;

		if (pnodeGreater)
			if (Key(key, pnodeGreater->Key)) // key < pnodeGreater->Key
				return pnodeGreater->pnodeGetNextBinary(key);

		if (pnodeParent)
			return pnodeParent->pnodeGetNextBinary(key);

		return 0;
	}
	
	//******************************************************************************************
	//
	forceinline TNode* pnodeGetLastBinary
	(
		const K& key		// Key to find the node before.
	)
	//
	// Returns the next node before the the given key.
	//
	//**************************************
	{
		if (pnodeGreater)
			if (Key(pnodeGreater->Key, key))
				return pnodeGreater->pnodeGetLastBinary(key);

		if (Key(Key, key))
			return this;

		if (pnodeLess)
			if (Key(pnodeLess->Key, key))
				return pnodeLess->pnodeGetLastBinary(key);

		if (pnodeParent)
			return pnodeParent->pnodeGetLastBinary(key);

		return 0;
	}

	//******************************************************************************************
	//
	// Friend classes.
	//
	friend iterator;
	friend reverse_iterator;
	friend tree_list;
	friend binary_iterator;

};


//**********************************************************************************************
//
template<class N> class CTreeListBinaryIterator
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
	forceinline CTreeListBinaryIterator(N* pnode = 0)
		: pnodeCurrent(pnode)
	{
	}

	
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	forceinline typename N::TData operator*()
	//
	// Dereferences the data associated with the iterator.
	//
	//**************************************
	{
		if (pnodeCurrent)
			return pnodeCurrent->Data;
		return 0;
    }

	//******************************************************************************************
	//
	forceinline operator bool()
	//
	// Returns 'true' if the current position of the iterator is valid.
	//
	//**************************************
	{
		return pnodeCurrent != 0;
    }

	//******************************************************************************************
	//
	forceinline bool operator ==(const CTreeListBinaryIterator<N>& it_compare) const
	//
	// Returns 'true' if the two iterators are equal.
	//
	//**************************************
	{
		return pnodeCurrent == it_compare.pnodeCurrent;
	}

	//******************************************************************************************
	//
	forceinline bool operator !=(const CTreeListBinaryIterator<N>& it_compare) const
	//
	// Returns 'true' if the two iterators are not equal.
	//
	//**************************************
	{
		return pnodeCurrent != it_compare.pnodeCurrent;
	}

	//******************************************************************************************
	//
	forceinline CTreeListBinaryIterator<N> operator ++()
	//
	// Incremements iterator to the next element in the list.
	//
	//**************************************
	{
		Assert(pnodeCurrent);

		pnodeCurrent = pnodeCurrent->pnodeGetNextBinary(pnodeCurrent->Key);
		return *this;
	}

	//******************************************************************************************
	//
	forceinline CTreeListBinaryIterator<N> operator --()
	//
	// Decremements iterator to the last element in the list.
	//
	//**************************************
	{
		Assert(pnodeCurrent);

		pnodeCurrent = pnodeCurrent->pnodeGetLastBinary(pnodeCurrent->Key);
		return *this;
	}
};


//**********************************************************************************************
//
template<class N> class CTreeListIterator
//
// Prefix: it
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
	forceinline CTreeListIterator(N* pnode = 0)
		: pnodeCurrent(pnode)
	{
	}

	
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	forceinline typename N::TData operator*()
	//
	// Dereferences the data associated with the iterator.
	//
	//**************************************
	{
		Assert(pnodeCurrent);
		return pnodeCurrent->Data;
    }

	//******************************************************************************************
	//
	forceinline operator bool()
	//
	// Returns 'true' if the current position of the iterator is valid.
	//
	//**************************************
	{
		return pnodeCurrent != 0;
    }

	//******************************************************************************************
	//
	forceinline bool operator ==(const CTreeListIterator<N>& it_compare) const
	//
	// Returns 'true' if the two iterators are equal.
	//
	//**************************************
	{
		return pnodeCurrent == it_compare.pnodeCurrent;
	}

	//******************************************************************************************
	//
	forceinline bool operator !=(const CTreeListIterator<N>& it_compare) const
	//
	// Returns 'true' if the two iterators are not equal.
	//
	//**************************************
	{
		return pnodeCurrent != it_compare.pnodeCurrent;
	}

	//******************************************************************************************
	//
	forceinline CTreeListIterator<N> operator ++()
	//
	// Incremements iterator to the next element in the list.
	//
	//**************************************
	{
		Assert(pnodeCurrent);
		pnodeCurrent = pnodeCurrent->pnodeNext;
		Assert((pnodeCurrent && !pnodeCurrent->bSkip) || !pnodeCurrent);
		return *this;
	}

	//******************************************************************************************
	//
	forceinline CTreeListIterator<N> operator --()
	//
	// Decremements iterator to the last element in the list.
	//
	//**************************************
	{
		Assert(pnodeCurrent);
		pnodeCurrent = pnodeCurrent->pnodePrevious;
		return *this;
	}

	//******************************************************************************************
	//
	forceinline void SetAsBeginning(N* pnode)
	//
	// Decremements iterator to the last element in the list.
	//
	//**************************************
	{
		Assert(pnode);
		Assert(!pnode->bSkip);

		pnode->pnodePrevious = 0;
		pnode->SetNextPointer(pnodeCurrent);
		if (pnodeCurrent)
			pnodeCurrent->pnodePrevious = pnode;

		// Set the current node (the beginning node) to be the node passed as a parameter.
		pnodeCurrent = pnode;
		Assert(pnodeCurrent->bValidateOrder());
	}
};


//**********************************************************************************************
//
template<class N> class CTreeListReverseIterator
//
// Prefix: rit
//
// An iterator to move backward through the binary tree list. Uses an STL-style interface.
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
	forceinline CTreeListReverseIterator(N* pnode = 0)
		: pnodeCurrent(pnode)
	{
	}

	
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	forceinline typename N::TData& operator*()
	//
	// Dereferences the data associated with the iterator.
	//
	//**************************************
	{
		Assert(pnodeCurrent);
		return pnodeCurrent->Data;
    }

	//******************************************************************************************
	//
	forceinline operator bool()
	//
	// Returns 'true' if the current position of the iterator is valid.
	//
	//**************************************
	{
		return pnodeCurrent != 0;
    }

	//******************************************************************************************
	//
	forceinline bool operator ==(const CTreeListReverseIterator<N>& it_compare) const
	//
	// Returns 'true' if the two iterators are equal.
	//
	//**************************************
	{
		return pnodeCurrent == it_compare.pnodeCurrent;
	}

	//******************************************************************************************
	//
	forceinline bool operator !=(const CTreeListReverseIterator<N>& it_compare) const
	//
	// Returns 'true' if the two iterators are not equal.
	//
	//**************************************
	{
		return pnodeCurrent != it_compare.pnodeCurrent;
	}

	//******************************************************************************************
	//
	forceinline CTreeListReverseIterator<N> operator ++()
	//
	// Decremements iterator to the last element in the list.
	//
	//**************************************
	{
		Assert(pnodeCurrent);
		pnodeCurrent = pnodeCurrent->pnodePrevious;
		return *this;
	}

	//******************************************************************************************
	//
	forceinline CTreeListReverseIterator<N> operator --()
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
	forceinline void SetAsBeginning(N* pnode)
	//
	// Decremements iterator to the last element in the list.
	//
	//**************************************
	{
		Assert(pnode);
		Assert(!pnode->bSkip);

		// The current node will become the next node.
		pnode->pnodePrevious = pnodeCurrent;
		if (pnodeCurrent)
			pnodeCurrent->SetNextPointer(pnode);

		// Set the current node (the beginning node) to be the node passed as a parameter.
		pnodeCurrent = pnode;
		Assert(pnodeCurrent->bValidateOrder());
	}
};


//**********************************************************************************************
//
template<class K, class T> class CTreeList : public CTreeListNode<K, T>
//
// Prefix: tree
//
// A binary tree container class with an STL-style interface.
//
//**************************************
{
protected:

	// Object variables.
	iterator itBegin;					// Iterator representing the smallest tree member.
	reverse_iterator revitBegin;		// Iterator representing the largest tree member.

	// Class variables.
	static TNode* ptreeAllocator;		// Start allocation position in memory.
	static TNode* ptreeMaxAllocation;	// End allocation position in memory.
	static CFastHeap* pfhAllocator;		// The fast heap that owns the alloated memory or NULL.

public:

	int iNumNodes;						// The number of elements in the tree.

public:
	
	//******************************************************************************************
	//
	// Constructor.
	//

	// Default constructor.
	forceinline CTreeList()
		: iNumNodes(0) 
	{
		pnodeLess     = 0;
		pnodeGreater  = 0;
		pnodeParent   = 0;
		SetNextPointer(0);
		pnodePrevious = 0;
		bSkip         = true;
	}

	
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	forceinline static void SetAllocator
	(
		void*		pv_allocator,	// base of allocated memory
		uint		u_size,			// Maximum number of items that can be allocated.
		CFastHeap*	pfh = NULL		// fast heap used to allocate memory or NULL is not a fast heap
	)
	//
	// Sets the allocator to be used by the container class.
	//
	//**************************************
	{
		Assert(pv_allocator);
		Assert(u_size);

		ptreeAllocator = (TNode*)pv_allocator;
		ptreeMaxAllocation = ptreeAllocator + u_size - 1;
		pfhAllocator = pfh;
	}

	//******************************************************************************************
	//
	forceinline iterator begin()
	//
	// Returns an iterator to the first element in the list.
	//
	//**************************************
	{
		return itBegin;
	}

	//******************************************************************************************
	//
	forceinline iterator end()
	//
	// Returns an iterator to a null element.
	//
	//**************************************
	{
		return iterator();
	}

	//******************************************************************************************
	//
	forceinline binary_iterator binary_begin()
	//
	// Returns an iterator to the first element in the list.
	//
	//**************************************
	{
		TNode* pnode = this;

		while (pnode->pnodeLess)
			pnode = pnode->pnodeLess;
		return binary_iterator(pnode);
	}

	//******************************************************************************************
	//
	forceinline binary_iterator binary_rbegin()
	//
	// Returns an iterator to the first element in the list.
	//
	//**************************************
	{
		TNode* pnode = this;

		while (pnode->pnodeGreater)
			pnode = pnode->pnodeGreater;
		return binary_iterator(pnode);
	}

	//******************************************************************************************
	//
	forceinline binary_iterator binary_end()
	//
	// Returns an iterator to a null element.
	//
	//**************************************
	{
		return binary_iterator();
	}

	//******************************************************************************************
	//
	forceinline reverse_iterator rbegin()
	//
	// Returns an iterator to the last element in the list.
	//
	//**************************************
	{
		return revitBegin;
	}

	//******************************************************************************************
	//
	forceinline reverse_iterator rend()
	//
	// Returns an iterator to a null element.
	//
	//**************************************
	{
		return reverse_iterator();
	}

	//******************************************************************************************
	//
	forceinline T pop()
	//
	// Pops the first element in the list and reforms the tree at that location.
	//
	//**************************************
	{
		if (!itBegin.pnodeCurrent)
			return 0;

		iNumNodes--;

		TNode* pnode = itBegin.pnodeCurrent;
	
		pnode->DeleteNode(itBegin, revitBegin);

		if (pnode->pnodeParent)
		{
			if (pnode == pnode->pnodeParent->pnodeLess)
				pnode->pnodeParent->pnodeLess = pnode->pnodeGreater;
			else
				pnode->pnodeParent->pnodeGreater = pnode->pnodeGreater;
		}

		return pnode->Data;
	}

	//******************************************************************************************
	//
	forceinline iterator push_front(const K& k, const T& t)
	//
	// Inserts an element into the tree and the list in the first position.
	//
	//**************************************
	{
		// If the tree is empty, just use the ordinary insertion.
		if (!itBegin.pnodeCurrent)
			return insert(k, t);

		// Find the push point.
		TNode* pnode = this;//binary_begin().pnodeCurrent;
		while (pnode->pnodeLess)
			pnode = pnode->pnodeLess;

		// Create a node at the beginning of the list.
		TNode* pnode_new = NewNode(k, t);
		pnode->pnodeLess = pnode_new;
		pnode_new->pnodeParent = pnode;
		pnode_new->SetNextPointer(itBegin.pnodeCurrent);
		itBegin.pnodeCurrent->pnodePrevious = pnode_new;
		itBegin.pnodeCurrent = pnode_new;

		// Increment the count of nodes.
		iNumNodes++;
		Assert(pnode_new->bValidateOrder());

		return itBegin;
	}

	//******************************************************************************************
	//
	forceinline iterator insert_fast(const K& k, const T& t, const iterator& it)
	//
	// Inserts an element into the tree and the list.
	//
	//**************************************
	{
		// Create a new node from the allocator memory.
		TNode* pnode = NewNode(k, t);

		// Increment the count of nodes.
		iNumNodes++;

		pnode->pnodeParent = it.pnodeCurrent;
		if (k(k, it.pnodeCurrent->Key))
			it.pnodeCurrent->pnodeLess = pnode;
		else
			it.pnodeCurrent->pnodeGreater = pnode;
		return iterator(pnode);
	}

	//******************************************************************************************
	//
	forceinline iterator insert(const K& k, const T& t)
	//
	// Inserts an element into the tree and the list.
	//
	//**************************************
	{
		// Create a new node from the allocator memory.
		TNode* pnode = NewNode(k, t);

		// Increment the count of nodes.
		iNumNodes++;

		// If there is a tree, find the position to insert it.
		if (iNumNodes > 1)
		{
			bool b_less = true;
			bool b_gt = true;
			return InsertNode(pnode, itBegin, revitBegin, b_less, b_gt);
		}
		else
		{
			// Start a tree.
			itBegin = iterator(pnode);
			revitBegin = reverse_iterator(pnode);

			// Return an iterator to the new node.
			return iterator(pnode);
		}
	}

	//******************************************************************************************
	//
	forceinline void erase(iterator it)
	//
	// Erases and element in the tree and list.
	//
	//**************************************
	{
		if (it.pnodeCurrent)
		{
			// Remove the node from its current position.
			it.pnodeCurrent->DeleteNode(itBegin, revitBegin);

			// Decrement the node counter.
			iNumNodes--;
		}
	}

	//******************************************************************************************
	//
	forceinline void erase(const K& k)
	//
	// Erases and element in the tree and list.
	//
	//**************************************
	{
		erase(find(k));
	}

	//******************************************************************************************
	//
	forceinline iterator find(const K& k)
	//
	// Returns an iterator to the element associated with the key. If the element is not found,
	// the iterator is null (it will equal the iterator returned from the end() call).
	//
	//**************************************
	{
		return itFind(k);
	}

 	//******************************************************************************************
	//
	forceinline T operator[](const K& k)
	//
	// Returns a reference to the data associated with the key.
	//
	//**************************************
	{
		iterator it = itFind(k);
		if (it == end())
			return 0;
		return *it;
	}

	//******************************************************************************************
	//
	forceinline bool empty() const
	//
	// Returns true if there are no elements associated with the container.
	//
	//**************************************
	{
		return iNumNodes == 0;
	}

	//******************************************************************************************
	//
	forceinline int size()
	//
	// Returns true if there are no elements associated with the container.
	//
	//**************************************
	{
		return iNumNodes;
	}

	//******************************************************************************************
	//
	forceinline void SetBeginEnd()
	//
	// Sets the begin and end iterators for the container. For use with insert_fast.
	//
	//**************************************
	{
		itBegin.pnodeCurrent = binary_begin().pnodeCurrent;
		revitBegin.pnodeCurrent = binary_rbegin().pnodeCurrent;
	}

	//******************************************************************************************
	//
	forceinline TNode *AllocateNode()
	//
	// Allocates memory for a new node.
	//
	//**************************************
	{
		// we have allocated the last node, take drastic action
		if (ptreeAllocator >= ptreeMaxAllocation)
		{
			// if we have a fast heap allocator then we can extend the size of the original
			// allocation upto the size of the fast heap, if the fast heap is completely
			// allocated then the new operator will assert or fail.
			if (pfhAllocator)
			{
				// we do not need the return address of new operator, the new call will
				// extend the commited section of the fast heap by enough to accodate
				// the requested nodes.

				new (*pfhAllocator) TNode[1024];
				ptreeMaxAllocation += 1024;
			}

			// if we do not have an allocator then we must fail.
			AlwaysAssert(0);
		}

		return ptreeAllocator++;
	}

	//******************************************************************************************
	//
	forceinline TNode* NewNode
	(
		const K& k,
		const T& t
	)
	//
	// Allocates memory for a new node and initializes the node with default values.
	//
	//**************************************
	{
		TNode* pnode;

		// Allocate node.
		if (iNumNodes == 0)
			pnode = this;				// Special case for root node.
		else
			pnode = AllocateNode();

		// Initialize the node.
		pnode->pnodeLess     = 0;
		pnode->pnodeGreater  = 0;
		pnode->SetNextPointer(0);
		pnode->pnodePrevious = 0;
		pnode->pnodeParent   = 0;
		pnode->bSkip         = false;
		pnode->Key           = k;
		pnode->Data          = t;
		return pnode;
	}

};


//**********************************************************************************************
//
// Static variables for memory allocation.
//
template<class K, class T> typename CTreeList<K, T>::TNode* CTreeList<K, T>::ptreeAllocator;
template<class K, class T> typename CTreeList<K, T>::TNode* CTreeList<K, T>::ptreeMaxAllocation;
template<class K, class T> CFastHeap* CTreeList<K, T>::pfhAllocator;


#endif // HEADER_STD_TREELIST_HPP
