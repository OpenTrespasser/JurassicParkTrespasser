/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CSet<I>, a class that abstracts a bitset.
 *
 * Bugs:
 *
 * To do:
 *		Base the type of all data members on the template parameter.
 *
 *		Overload the equality (==) operator for the set class. Added include for <function.h> to
 *		define != operator.
 *
 *		Use the template parameter as the type for parameters to the various operators where
 *		appropriate.
 *
 *		Added assert checking where appropriate to validate the range of passed parameters etc.
 *
 *		Take the commented out code at the end of this file and include it in the 'Example:'
 *		field in the class comment header. Update the example code for coding standards.
 *
 *		Comment how the value of an enum determines the bit position in the set and how CSet
 *		differs from bitfields wrt to the fact that positions of bits are determined and not
 *		implementation defined.
 *
 *		Should the CSetHelper class be private to CSet?
 *
 *		Consider whether it would make any sense to overload the shift operators for CSet.
 *
 *		Currently, chaining for CSet (ex  if (set[FOO][BAR]) {}) will not work when the set
 *		is const.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/Set.hpp                                                       $
 * 
 * 24    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 23    8/27/98 12:38a Asouth
 * made comparison operators into const functions
 * 
 * 22    97.12.08 5:49p Mmouni
 * Added ConditionalAdd function to conditionally add a bit to the set without using any
 * comparisons.
 * 
 * 21    97/08/18 10:22 Speter
 * Added comparison operators, and bAll() function.
 * 
 * 20    97/07/23 17:59 Speter
 * Made unnecessary default parameters exist in all build modes, as the crappy compiler now
 * complains in them.
 * 
 * 19    6/06/97 2:12p Agrant
 * Brutal hack to get FINAL version to work in spite of odd compiler bug.
 * 
 * 18    5/30/97 11:06a Agrant
 * Allow construction of CSet with a uint initializer.
 * 
 * 17    5/26/97 1:33p Agrant
 * LINT fixes.
 * 
 * 16    97-05-02 12:03 Speter
 * Converted NonConst function to const_cast (5.0 compatibility).
 * 
 * 15    97-04-14 20:19 Speter
 * Added bAll member to check all referenced bits.
 * 
 * 14    97-03-31 22:34 Speter
 * Redid CSet<> class, to allow all read functions on const objects.  
 * Removed direct initialisation from integers; use set0 object to init to 0.  Fixed constness
 * in members.  Fixed coding standard errors.
 * 
 * 13    1/09/97 5:43p Agrant
 * New [] operator for CSet to allow accessing its data when the CSet is const.
 * 
 * 12    96/09/05 11:52 Speter
 * Added negation operator, which complements set.
 * 
 * 11    96/08/15 18:56 Speter
 * Added conversion from integer.
 * Added cute "multiply" operator.
 * 
 * 10    96/06/27 18:21 Speter
 * Added +, -, and ^ operators taking arguments of member type.
 * Added Set() function.
 * 
 * 9     96/06/24 4:17p Mlange
 * Fixed bugs in binary operators.
 * 
 * 8     96/06/24 11:26a Mlange
 * Declared operator functions 'const' where appropriate.
 * 
 * 7     96/06/19 5:27p Mlange
 * Updated todo list.
 * 
 * 6     96/06/19 11:48a Mlange
 * Some more updates to the todo list.
 * 
 * 5     96/06/18 7:54p Mlange
 * Updated the todo list.
 * 
 * 4     96/06/18 7:50p Mlange
 * Restored from archive. Added some comments.
 * 
 * 3     96/06/04 16:53 Speter
 * Improved method of accessing multiple bits at once.
 * 
 * 2     96/04/22 19:55 Speter
 * Cleaned up comments based on new standards.
 * 
 * 1     96/04/19 18:03 Speter
 * Not yet conforming to our coding standards.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_SET_HPP
#define HEADER_LIB_STD_SET_HPP


//**********************************************************************************************
//
class CSet0
//
// A dummy class used for concisely representing empty sets.
{
public:
	CSet0(){};
};

const CSet0 set0;

//**********************************************************************************************
//
template<class I> class CSet
//
// A class to abstract a bitset.  A CSet<I> object can contain up to 32 elements
// of type I, where I is an integral type such as int or an enum.  Operators provide
// simple adding, subtracting, and testing of elements or other sets with the set.
//
// Example:
//		enum EStooge {eMOE, eLARRY, eCURLY, eCURLY_JOE, eSHEMP};
//		CSet<EStooge> setst;			// Set starts out empty.
//
//		setst += eMOE;					// Set now contains element eMOE.
//		setst[eCURLY] = 1;				// Another way of adding an element.
//		setst += eSHEMP -= eCURLY;		// Add and subtract at once.
//		setst[eMOE][eSHEMP] = f;		// Conditionally set/clear multiple elements.
//		if (setst[eLARRY])				// Test for element eLARRY.
//			...
//
//		CSet<EStooge> setst2;
//		setst ^= setst2;				// You can operate on sets as well.
//
// Prefix: set
//
//**************************************
{
protected:
	uint32	u4Bits;						// Contains the bitset.

public:

	//******************************************************************************************
	//
	class CSetHelperConst
	//
	// A class private to CSet that implements the [] operation for const sets.
	//
	// The CSetHelper has just 3 operations:
	//	Construction: 	
	//		The [] operation on a CSet returns a CSetHelper that contains the address 
	//		of the CSet bitset, and a bitmask.
	//
	//		The [] operation on a CSetHelper adds a bit to the bitmask.
	//	Read:
	//		The read operation (operator int) returns the bits of the CSet that are
	//		set in the bitmask.
	//
	//**********************************
	{
	protected:
		const uint32&	ru4Var;			//lint !e1725
										// Reference points to a bitset variable.
		
		uint32			u4Mask;			// Mask indicates which bits to read or write.

	public:
		//**************************************************************************************
		//
		// Constructor.
		//

		// Straight member specification.  Sets up which things to operate on.
#if 1
		// For some reason, the compiler wants default params for these two args.
		// It used to be only when BUILDVER_MODE == MODE_FINAL, now it's always.
		forceinline CSetHelperConst(const uint32& ru4_var = 0, int32 u4_mask = 0)
			: ru4Var(ru4_var), u4Mask(u4_mask) {}
#else
		forceinline CSetHelperConst(const uint32& ru4_var, uint32 u4_mask)
			: ru4Var(ru4_var), u4Mask(u4_mask) {}
#endif


		//**************************************************************************************
		//
		// Conversion operator.
		//

		forceinline operator uint32 () const
		//
		// Returns:
		//		The bits asked for from the CSet bitfield.
		//		
		// Example:
		//		int i = seti[3][4];		// Extract bits 3 and 4 from seti.
		//								// Equivalent to (seti.u4Mask & 12);
		//
		{
			return ru4Var & u4Mask;
		}

		//**************************************************************************************
		//
		// Operator functions.
		//

		forceinline CSetHelperConst& operator [](I i)
		// Allow chaining of bits to construct a CSetHelperConst containing multiple bits.
		{
			u4Mask |= 1<<i;
			return *this;
		}

		//**************************************************************************************
		//
		// Member functions.
		//

		forceinline bool bAll() const
		//
		// Returns:
		//		Whether all the bits asked for are set.
		//
		// Example:
		//		if (seti[3][4].bAll())				// Test whether both 3 and 4 bits are set.
		//
		{
			return (ru4Var & u4Mask) == u4Mask;
		}
	};

	//******************************************************************************************
	//
	class CSetHelper: public CSetHelperConst
	//
	// A version of CSetHelperConst that allows writes as well as reads.
	// Casts away the const of the inherited reference when needed.
	//
	//**********************************
	{
	public:
		//**************************************************************************************
		//
		// Constructor.
		//

		// Straight member specification.  Sets up which things to operate on.
		forceinline CSetHelper(uint32& ru4_var, uint32 u4_mask)
			: CSetHelperConst(ru4_var, u4_mask)
		{
		}

		//**************************************************************************************
		//
		// Assignment operator.
		//

		forceinline  void operator =(const int i) const
		// Write operator.  This is called whenever an expression like this occurs:
		//		seti[3] = 0;
		// It sets the CSet bitfield according to the bitmask.
		{
			if (i)
				const_cast<uint32&>(ru4Var) |= u4Mask;
			else
				const_cast<uint32&>(ru4Var) &= ~u4Mask;
		}

		//**************************************************************************************
		//
		// Operator functions.
		//

		forceinline CSetHelper& operator [](I i)
		// Allow chaining of bits to construct a CSetHelper containing multiple bits.
		{
			u4Mask |= 1<<i;
			return *this;
		}
	};

	//******************************************************************************************
	//
	// Constructors.
	//

	forceinline CSet()
		: u4Bits(0) 
	{
	}

	forceinline CSet(CSet0)
		: u4Bits(0) 
	{
	}

	//******************************************************************************************
	//
	friend forceinline CSet<I> Set(I i)
	{
		return CSet<I>() + i;
	}
	//
	// Acts like a constructor for a temporary object, constructing a CSet<I> object with one member.
	//
	// Returns:
	//		A set object containing just the member i.
	//
	// Example:
	//		Particularly useful when initialising CSets.
	//
	//		CSet<EStooge> setst = Set(eMOE) + eLARRY;
	//
	//		The advantage is that the type of the set is inferred from the argument, rather than
	//		having to be redundantly specified, as in:
	//
	//		CSet<EStooge> setst = CSet<EStooge>(eMOE) + eLARRY;
	//
	//**********************************

	//******************************************************************************************
	//
	// Conversion operators.
	//

	// Allow conversion to the bit value, for testing empty elements, or switch statements.
	forceinline operator uint32 () const
	{
		return u4Bits;
	}

	//******************************************************************************************
	//
	// Operators.
	//

	// Unary operator.  Unary minus is complement, analogous to subtraction operator below.
	forceinline bool operator !() const
	{
		return u4Bits == 0;
	}

	// Unary operator.  Unary minus is complement, analogous to subtraction operator below.
	forceinline CSet<I> operator -() const
	{
		return CSet<I>(~u4Bits, 0);
	}

	//
	// Operations with elements.
	//

	// Set, clear, and toggle individual elements.

	forceinline CSet<I>& operator +=(I i) 
	{ 
		u4Bits |= (1<<i); 
		return *this; 
	}

	forceinline CSet<I> operator +(I i) const
	{ 
		return CSet<I>(u4Bits | (1<<i), 0);
	}

	forceinline CSet<I>& operator -=(I i) 
	{ 
		u4Bits &= ~(1<<i); 
		return *this; 
	}

	forceinline CSet<I> operator -(I i) const
	{ 
		return CSet<I>(u4Bits & ~(1<<i), 0);
	}

	forceinline CSet<I>& operator ^=(I i) 
	{ 
		u4Bits ^= (1<<i); 
		return *this; 
	}

	forceinline CSet<I> operator ^(I i) const
	{ 
		return CSet<I>(u4Bits ^ (1<<i), 0);
	}

	// Allow reading/writing a single element, via CSetHelper[Const].

	forceinline CSetHelper operator [](I i)
	{ 
		return CSetHelper(u4Bits, 1<<i); 
	}

	forceinline CSetHelperConst operator [](I i) const
	{ 
		return CSetHelperConst(u4Bits, 1<<i);
	}

	//
	// Operations with sets.
	//

	// Comparison
	forceinline bool operator ==(CSet<I> set) const
	{ 
		return u4Bits == set.u4Bits;
	}

	forceinline bool operator !=(CSet<I> set) const
	{ 
		return u4Bits != set.u4Bits;
	}

	// Union.
	forceinline CSet<I>& operator +=(CSet<I> set) 
	{ 
		u4Bits |= set.u4Bits; 
		return *this; 
	}

	forceinline CSet<I> operator +(CSet<I> set) const
	{
		return CSet<I>(u4Bits | set.u4Bits, 0);
	}

	// Difference.
	forceinline CSet<I>& operator -=(CSet<I> set) 
	{ 
		u4Bits &= ~set.u4Bits; 
		return *this; 
	}

	forceinline CSet<I> operator -(CSet<I> set) const
	{
		return CSet<I>(u4Bits & ~set.u4Bits, 0);
	}

	// Exclusive or.
	forceinline CSet<I>& operator ^=(CSet<I> set) 
	{ 
		u4Bits ^= set.u4Bits; 
		return *this; 
	}

	forceinline CSet<I> operator ^(CSet<I> set) const
	{
		return CSet<I>(u4Bits ^ set.u4Bits, 0);
	}

	// Intersection.
	forceinline CSet<I>& operator &=(CSet<I> set) 
	{ 
		u4Bits &= set.u4Bits; 
		return *this; 
	}

	forceinline CSet<I> operator &(CSet<I> set) const
	{
		return CSet<I>(u4Bits & set.u4Bits, 0);
	}

	// "Multiply" set by a boolean value.
	forceinline CSet<I> operator *(bool b) const
	{
		return b ? *this : CSet<I>();
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	// Test for all set
	forceinline bool bAll(CSet<I> set) const
	{ 
		return (u4Bits & set.u4Bits) == set.u4Bits;
	}

	// Conditionally set one of the flags of the set.
	forceinline void ConditionalAdd(bool condition, I i)
	{
		u4Bits |= (condition<<i); 
	}

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	//
	// A private constructor taking a dummy second parameter, 
	// which allows directly setting the bit value.
	//
	CSet(uint32 u4_mask, int)
		: u4Bits(u4_mask) {}
};

#endif


/*
		enum EStooge {										union SStooge {
																int iMask;
																struct {
			eMOE, eLARRY, eCURLY, eCURLY_JOE, eSHEMP				int bMoe: 1, bLarry: 1, bCurly: 1, bCurlyJoe: 1, bShemp: 1;
																}
		};													};
		CSet<EStooge> setst;								SStooge setst;

		setst += eMOE;										setst.bMoe = 1;
		setst -= eLARRY;									setst.bLarry = 0;
		setst ^= eCURLY;									setst.bCurly ^= 1;
		setst[eCURLY] = f;									setst.bCurly = f;
		if (setst[eLARRY])									if (setst.bCurly)

		setst += eMOE += eSHEMP;							setst.bMoe = setst.bShemp = 1;
		setst -= eLARRY -= eCURLY_JOE;						setst.bLarry = setst.bCurlyJoe = 0;
		setst += eSHEMP -= eCURLY ^= eLARRY;				setst.bShemp = 1;
															setst.bCurly = 0;
															setst.bLarry ^= 1;
		setst[eSHEMP][eLARRY] = f;							setst.bShemp = setst.bLarry = f;
		if (setst[eSHEMP][eLARRY])							if (setst.bShemp || setst.bLarry)

		setst += setst2;									setst.iMask |= setst2.iMask;
		setst -= setst2;									setst.iMask &= ~setst2.iMask;
		setst ^= setst2;									setst.iMask ^= setst2.iMask;
*/
