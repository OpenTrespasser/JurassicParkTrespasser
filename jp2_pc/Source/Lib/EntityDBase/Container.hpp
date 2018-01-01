/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		General purpose container class.
 *
 * Bugs:
 *
 * To do:
 *		Add STL extensions to the 'CDArray' and test it with the CContainer class.
 *
 * Notes:
 *		The CContainer class is designed to function as an extension of STL containers; it
 *		combines the iterator into the container and simplifies use of STL. Currently it
 *		is tested to work with the vector, list, deque and set classes.
 *
 *		Containers inherited from container adapters use the "push_back" member function to
 *		add data; associative containers use the "insert" member function.
 *
 *		The following example shows its use:
 *
 *			void TestList()
 *			{
 *				CContainer< list<int> > intlist;	// Create the empty container.
 *
 *				// Add some data to the container.
 *				intlist.push_back(10);
 *				intlist.push_back(99);
 *				intlist.push_back(5);
 *
 *				// Iterate throught the container and write elements to disk.
 *				for (intlist.Begin(); intlist.bIsNotEnd(); intlist++)
 *				{
 *					conMessageLog << "Contains value: " << intlist.rtGet() << "\n";
 *				}
 *			}
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Container.hpp                                         $
 * 
 * 21    98.08.14 3:35p Mmouni
 * Made change becuase I was getting an internal compiler error in 4.2
 * 
 * 20    98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 19    3/22/98 5:02p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 18    3/19/98 5:21p Pkeet
 * Everybody now has internal compiler errors; it is time to try the alternative iterator
 * declaration.
 * 
 * 17    1/29/98 3:33p Gstull
 * Added a #ifdef to change the iterator definition to be compatible with version 4.1 of the
 * VC++  compiler.
 * 
 * 16    11/15/97 10:20p Agrant
 * Let us get at STL
 * 
 * 15    8/28/97 4:02p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 15    97/08/22 1:12p Pkeet
 * Fixed iterator template problem.
 * 
 * 14    97/06/13 14:31 Speter
 * Changed all ++ operations to pre-increment the actual STL iterator.
 * 
 * 13    97/06/10 15:34 Speter
 * Took out unnecessary T:: in iterator declaration, fixing a compiler internal error.
 * 
 * 12    5/30/97 11:08a Agrant
 * LINT tidying.
 * 
 * 11    97-05-02 11:56 Speter
 * Added convenient -> operator.
 * 
 * 10    97/03/08 6:12p Pkeet
 * Temporarily disabled the 'EraseAll' command in the destructor.
 * 
 * 9     97/03/08 1:29p Pkeet
 * Added the 'EraseAll' member function, and added it to the virtual destructor.
 * 
 * 8     97/01/27 11:54 Speter
 * Removed const& operator *.
 * 
 * 7     97/01/26 19:21 Speter
 * Added fun operators.
 * 
 * 6     1/08/97 7:35p Pkeet
 * Added scope information for uses of 'value_type.'
 * 
 * 5     12/17/96 4:11p Pkeet
 * Added a virtual destructor.
 * 
 * 4     11/22/96 5:12p Pkeet
 * Added the 'rtGet' member function.
 * 
 * 3     11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 2     96/11/05 3:48p Mlange
 * Some code style changes. Fixed some comments.
 * 
 **********************************************************************************************/

#ifndef HEADER_ENTITYDBASE_CONTAINER_HPP
#define HEADER_ENTITYDBASE_CONTAINER_HPP

//
// Class definitions.
//

//*********************************************************************************************
//
template<class T> class CContainer : public T
//
// Generalized container class for building lists or arrays of objects.
//
// Prefix: cont
//
//**************************
{
public:

// Modify the definition of this iterator since an internal compoler error is generated in VC++ 4.1.
#if _MSC_VER < 1100
	iterator itContainer;		// Iterator for the container.
#else
	T::iterator itContainer;	// Iterator for the container.
#endif

public:
	
	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	CContainer()
	{
		// Initialize the iterator.
		Begin();
	};

	virtual ~CContainer()   //lint !e1509
	{
		//EraseAll();
	}
	
	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Begin()
	//
	// Performs the initialisation required before iterating the contents of this container.
	//
	//**************************
	{
		// Set the iterator to the first element in the array.
		itContainer = begin();
	}

	//*****************************************************************************************
	//
	void operator ++()
	//
	// Moves the iterator to the next valid object in the container.
	//
	//**************************************
	{
		// Increment the iterator.
		++itContainer;
	}

	//*****************************************************************************************
	//
	void operator ++(int)
	//
	// Moves the iterator to the next valid object in the container.
	// Note: This actually performs pre-increment on the iterator, because it is usually
	// much faster.
	//
	//**************************************
	{
		// Increment the iterator.
		++itContainer;
	}

	//*****************************************************************************************
	//
	bool bIsNotEnd()
	//
	// Determine if the last element in the container has been reached.
	//
	// Returns:
	//		'true' if there are still elements in this container.
	//
	//**************************************
	{
		// Return false if the iterator equals the end object.
		return itContainer != end();
	}

	//*****************************************************************************************
	//
	const T::value_type& tGet()
	//
	// Obtain a value from this container.
	//
	// Returns:
	//		The current value referenced by the iterator.
	//
	//**************************************
	{
		// Ensure there are still some elements left.
		Assert(bIsNotEnd());

		// Return the value pointed to by the iterator.
		return (const value_type&)(*itContainer);
	}

	//*****************************************************************************************
	//
	T::value_type& rtGet()
	//
	// Obtain a value from this container.
	//
	// Returns:
	//		The current value referenced by the iterator.
	//
	//**************************************
	{
		// Ensure there are still some elements left.
		Assert(bIsNotEnd());

		// Return the value pointed to by the iterator.
		return (value_type&)(*itContainer);
	}

	//*****************************************************************************************
	//
	// Operators.
	//

	//*****************************************************************************************
	//
	operator bool()
	//
	// Duplicate bIsNotEnd();
	//
	//**************************************
	{
		return itContainer != end();
	}

	//*****************************************************************************************
	//
	T::value_type& operator *()
	//
	// Duplicate rtGet().
	//
	//**************************************
	{
		// Ensure there are still some elements left.
		Assert(bIsNotEnd());

		// Return the value pointed to by the iterator.
		return (value_type&)(*itContainer);
	}

	//*****************************************************************************************
	//
	T::value_type* operator ->()
	//
	// Return address of rtGet().
	//
	//**************************************
	{
		// Ensure there are still some elements left.
		Assert(bIsNotEnd());

		// Return the value pointed to by the iterator.
		return (value_type*)(&*itContainer);
	}

	//*****************************************************************************************
	//
	virtual void EraseAll
	(
	)
	//
	// Erases the entire container list.
	//
	//**************************
	{
		erase(begin(), end());
	}

};


	//*****************************************************************************************
	//
#define foreach(CONTAINER)\
	for (CONTAINER.Begin(); CONTAINER.bIsNotEnd(); CONTAINER++)
	//
	// A "for" construct that only works for CContainer or child classes of CContainer.
	//
	// Notes:
	//		In the statement following the "foreach" command, CONTAINER's internal 
	//		iterator points to successive values until all have been represented.
	//
	//	Example:
	//		CContainer<list<int> > conint;
	//		conint.push_back(15);
	//		conint.push_back(16);
	//		conint.push_back(3);
	//
	//		foreach(conint)
	//		{
	//			// {} optional, just like "for" or "while"
	//			cout << conint.tGet << "--";
	//		}
	//
	//		// Should result in:
	//		15--16--3
	//
	//
	//**************************************



#endif
