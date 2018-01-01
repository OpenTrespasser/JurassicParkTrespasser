/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	 	Contains several simple and useful inline and template functions and pre-processor
 *		macros. This module can be seen as to implement 'extensions' to the ANSI standard
 *		library.
 *
 *		This header file is automatically included by "Common.hpp".
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/StdLibEx.hpp                                                  $
 * 
 * 51    8/26/98 2:44p Rvande
 * Changed a constant double to a float.
 * 
 * 50    6/03/98 8:06p Pkeet
 * Added the 'fToFloat' macro.
 * 
 * 49    10/21/97 1:38p Mlange
 * Improved uLog2() function.
 * 
 * 48    97/09/16 15:30 Speter
 * Fixed behaviour of bFurryEquals.
 * 
 * 47    97/09/04 18:46 Speter
 * Added bWithin to CFuzzy class.
 * 
 * 46    9/03/97 7:53p Pkeet
 * Added the 'pvGetVTable' and 'PokeVTable' functions.
 * 
 * 45    97/09/03 18:03 Speter
 * Added erase_all() template function (sorry).
 * 
 * 44    8/28/97 4:06p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 45    8/22/97 6:56p Pkeet
 * Added a macro for a 5.0 build.
 * 
 * 44    97/08/22 6:39p Pkeet
 * Fuzzy uses only specialized versions of Difference.
 * 
 * 43    97/07/23 17:52 Speter
 * bFurryEquals now uses relative tolerance as minimum absolute tolerance as well.
 * 
 * 42    97/06/23 18:47 Speter
 * Moved iRound to FloatDef.hpp, utilising its functions.
 * 
 * 41    97/05/21 17:33 Speter
 * Redid CFuzzy class to take second template parameter, and use global overloaded Difference()
 * function.
 * 
 * 40    97-05-01 19:04 Speter
 * Added slight optimisation to CFuzzy.
 * 
 * 39    97/04/04 4:33p Pkeet
 * Added the 'u4FromFloat' macro.
 * 
 * 38    3/07/97 10:50a Mlange
 * Added Fuzzy function for integer types.
 * 
 * 37    97/02/13 14:24 Speter
 * Added cool <= and >= operators for CFuzzy.  Added forall and forall_const macros.
 * 
 * 36    97/02/03 11:03 Speter
 * Added iRound function.
 * 
 * 35    1/25/97 1:53p Pkeet
 * Added the 'Sqr' function.
 * 
 * 34    1/08/97 7:41p Pkeet
 * Removed the 'Fuzzy' template version of the function.
 * 
 * 33    97/01/02 16:36 Speter
 * Commented out CDelete class, removed Set.h include.
 * 
 * 32    96/12/31 16:31 Speter
 * Changed rAssign to Assign.  Added NonConst and pNonConst functions.
 * 
 * 31    12/11/96 3:49p Mlange
 * Now ensures that the tolerance parameter for the CFuzzy class is always positive.
 * 
 * 30    96/12/04 20:39 Speter
 * Took out minimum tolerance in both bFurryEquals functions.
 * 
 * 29    11/17/96 3:10p Pkeet
 * Move the hashing functions and the 'uStrLen' function to separate modules.
 * 
 * 28    11/14/96 7:32p Pkeet
 * Added the 'u4Hash' and 'uStrLen' functions.
 * 
 * 27    11/11/96 8:59p Pkeet
 * Added the 'CDelete' class.
 * 
 * 26    96/10/25 14:38 Speter
 * Removed Assert in bWithin, because it's useful to allow hi to be less than lo, and it still
 * works.
 * 
 * 25    96/09/18 14:44 Speter
 * Added handy rAssign template.
 * 
 * 24    96/09/16 11:56 Speter
 * Removed bFuzzyEquals, now that we have CFuzzy.
 * 
 * 23    96/09/10 18:27 Speter
 * Added new CFuzzy class and Fuzzy() function.  It's beautiful, man.
 * 
 * 22    96/09/09 18:28 Speter
 * Changed MinMax and SetMinMax to make their third parameter a separate template type.
 * Changed prefixes of all parameters to match their template types.
 * Made bFurryEquals behave the same as bFuzzyEquals when its parameters are less than 1.
 * 
 * 21    96/08/15 18:57 Speter
 * Added NextPowerOfTwo function.
 * Changed header star lengths.
 * 
 * 20    8/12/96 7:14p Pkeet
 * Changed 'KeepInRange' to 'Clamp.'
 * 
 * 19    7/31/96 2:36p Pkeet
 * Added 'KeepInRange' template function.
 * 
 * 18    96/07/26 18:23 Speter
 * Added RoundUp and RoundDown templates.
 * 
 * 17    96/06/24 21:12 Speter
 * Added FurryEquals.
 * 
 * 16    6/19/96 5:03p Pkeet
 * Added the 'uLog2' template function.
 * 
 * 15    13-06-96 4:32p Mlange
 * Added macro to determine the number of elements in an array. Updated the todo list.
 * 
 * 14    12-06-96 1:06p Mlange
 * Removed definition of overloaded new operator. The Microsoft standard header file new.h also
 * contains that construct, so use that header file instead.
 * 
 * 13    6/03/96 4:43p Pkeet
 * Added the 'Offset' function.
 * 
 * 12    3-06-96 3:40p Mlange
 * Added bPowerOfTwo() function.
 * 
 * 11    30-05-96 11:25a Mlange
 * Added fuzzy equality functions.
 * 
 * 10    96/05/13 18:44 Speter
 * Guarded overloaded operator new against <new.h> include file.
 * 
 * 9     96/05/13 14:20 Speter
 * Added new bBetween function to check betweenness regardless of argument order.
 * 
 * 8     10-05-96 3:15p Mlange
 * Fixed assertion problem in BitMaskRange.
 * 
 * 7     5/09/96 10:36a Pkeet
 * Fixed assertion problem in Abs.
 * 
 * 6     5/08/96 2:03p Mlange
 * Added some comments. The BitMask and BitMaskRange functions now validate their parameters.
 * Added additional casts to Abs and Sign template functions to avoid ambiguities. Added Assert
 * checking to various other functions. Added correct function comment header to all functions.
 * 
 * 5     96/05/07 11:05 Speter
 * Changed MinMax and SetMinMax to be general on all 3 parameters.
 * 
 * 4     96/05/03 10:55 Speter
 * Added ifdef around operator new
 * 
 * 3     4/30/96 3:02p Mlange
 * Updated for changes to the coding standards. Moved the overloaded placement new operator from
 * UDefs.hpp to this file.
 * 
 * 2     96/04/19 17:59 Speter
 * Added standard Min, Max, Abs, Sign, etc. templates.
 * 
 * 1     4/18/96 5:09p Mlange
 * Defines extensions to stdlib.h.
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_STDLIBEX_HPP
#define HEADER_LIB_STD_STDLIBEX_HPP


//
// Includes.
//


//
// Constants.
//

// The default tolerance for fuzzy equality of floating point and integer values.
#define dTOLERANCE_DEFAULT	0.00000001
#define fTOLERANCE_DEFAULT	0.0001
#define iTOLERANCE_DEFAULT	0


//******************************************************************************************
//
template <class T, class U> inline T Min
(
	T t_a, U t_b
)
//
// Find the smaller of t_a and t_b.
//
// Returns:
//		t_a when t_a <= t_b
//		t_b when t_a >  t_b
//		Note that the return type is identical to the type of the first parameter.
//
//**************************************
{
	return t_a <= t_b ? t_a : t_b;
}

		 
//******************************************************************************************
//
template <class T, class U> inline T Max
(
	T t_a, U t_b
)
//
// Find the greater of t_a and t_b.
//
// Returns:
//		t_a when t_a >= t_b
//		t_b when t_a <  t_b
//		Note that the return type is identical to the type of the first parameter.
//
//**************************************
{
	return t_a >= t_b ? t_a : t_b;
}



//******************************************************************************************
//
template <class T, class U, class V> inline T MinMax
(
	T t, U u_lo, V v_hi
)
//
// Find t within the range limit of u_lo and v_hi.
//
// Returns:
//		u_lo when t <  u_lo
//		t    when t >= u_lo && t <= v_hi
//		v_hi when t >  v_hi
//		Note that the return type is identical to the type of the first parameter.
//
//**************************************
{
	Assert(u_lo <= v_hi);

	return t < u_lo ? u_lo : t > v_hi ? v_hi : t;
}



//******************************************************************************************
//
template <class T, class U> inline void SetMin
(
	T& t, U u_hi
)
//
// Upper limit t to the value specified by u_hi. If t is greater than this limit it is
// clamped to this limit.
//
// Side effects:
//		The first parameter is passed by reference and may be modified.
//
//**************************************
{
	if (t > u_hi)
		t = u_hi;
}
		 

//******************************************************************************************
//
template <class T, class U> inline void SetMax
(
	T& t, U u_lo
)
//
// Lower limit t to the value specified by u_lo. If t is less than this limit it is set to
// this limit.
//
// Side effects:
//		The first parameter is passed by reference and may be modified.
//
//**************************************
{
	if (t < u_lo)
		t = u_lo;
}


//******************************************************************************************
//
template <class T, class U, class V> inline void SetMinMax
(
	T& t, U u_lo, V v_hi
)
//
// Limit t to the range specified by u_lo and v_hi. If t is less than the lower limit or
// greater than the upper limit, it is set to this limit.
//
// Side effects:
//		The first parameter is passed by reference and may be modified.
//
//**************************************
{
	if (t < u_lo)
		t = u_lo;
	
	else if (t > v_hi)
		t = v_hi;
}


//******************************************************************************************
// 
template <class T, class U, class V> inline bool bWithin
(
	T t, U u_lo, V v_hi
)
//
// Determine whether t is between u_lo and v_hi (inclusive).
//
// Returns:
//		true when t is between u_lo and v_hi, false otherwise.
//
// Notes:
//		u_lo is assumed to be less than or equal to v_hi.
//
//**************************************
{
	return t >= u_lo && t <= v_hi;
}
		 
//******************************************************************************************
// 
template <class T, class U> inline bool bBetween
(
	T t, U u1, U u2
)
//
// Determine whether t is between u1 and u2 (inclusive).
//
// Returns:
//		true when t is between u1 and u2, false otherwise.
//
// Notes:
//		u1 and u2 can be in any order.
//
//**************************************
{
	if (u1 < u2)
		return t >= u1 && t <= u2;
	else
		return t >= u2 && t <= u1;
}
		 


//******************************************************************************************
// 
template <class T> inline bool bPowerOfTwo
(
	T t
)
//
// Determine whether t is a power of two.
//
// Returns:
//		true when t is a power of two, false otherwise.
//
//**************************************
{
	return (t & (t - 1)) == 0;
}
		 

//******************************************************************************************
// 
template <class T> inline T NextPowerOfTwo
(
	T t
)
//
// Returns:
//		The next highest-or-equal power of two.
//
//**************************************
{
	if (bPowerOfTwo(t))
		return t;

	int i_bits = 0;					// The number of bits in t.

	while (t)
	{
		// Count the bits in t.
		i_bits++;
		t >>= 1;
	}
	return (T)1 << i_bits;
}
		 


//******************************************************************************************
//
template <class T> inline T Abs
(
	T t
)
//
// Take the absolute of the given parameter.
//
// Returns:
//		t when t >= 0, -t when t < 0.
//
//**************************************
{
	T t_temp;

	t_temp = (t < (T)0 ? -t : t);

	// Ensure we didn't take the absolute of an integer negative minimum.
	Assert(t_temp >= (T)0);
	
	return t_temp;
}

//******************************************************************************************
//
template
<
	class T,							// Value type to compare.  
	class D								// Difference type.
> 
class CFuzzy
//
// Prefix: Not used.
//
// An abstract class to facilitate "fuzzy" equality tests, or tests with tolerance.
// This class contains a T value and tolerance.  The only operations allowed
// on the class are equality comparisons.
//
//**************************************
{
protected:
	const T tValue;						// The value to compare.  
										// It would be nice to use a reference, but that will cause
										// errors, as this class is used in temporary expressions.
	D dTolerance;						// The tolerance to compare with.

public:
	CFuzzy(const T& t_value, D d_tolerance)
		: tValue(t_value), dTolerance(d_tolerance)
	{
		Assert(d_tolerance >= D(0));
	}

	bool operator ==(const T& t) const
	{
		return Abs(Difference(tValue, t)) <= dTolerance;
	}

	bool operator !=(const T& t) const
	{
		return Abs(Difference(tValue, t)) > dTolerance;
	}

	bool operator <=(const T& t) const
	{
		return Difference(tValue, t) <= dTolerance;
	}

	bool operator >=(const T& t) const
	{
		return Difference(tValue, t) >= - dTolerance;
	}

	bool bWithin(const T& t_lo, const T& t_hi)
	{
		return *this >= t_lo && *this <= t_hi;
	}
};

#if _MSC_VER >= 1100

//******************************************************************************************
//
inline double Difference
(
	double t_a, double t_b
)
//
//**************************************
{
	return t_a - t_b;
}

//******************************************************************************************
//
inline float Difference
(
	float t_a, float t_b
)
//
//**************************************
{
	return t_a - t_b;
}

#else

//******************************************************************************************
//
template<class T> inline T Difference
(
	T t_a, T t_b
)
//
//**************************************
{
	return t_a - t_b;
}

#endif

//
// The following are specialised versions of Fuzzy for various types, which have their
// own default tolerances.
//

inline CFuzzy<int, int> Fuzzy(int i_value, int i_tolerance = iTOLERANCE_DEFAULT)
{
	return CFuzzy<int, int>(i_value, i_tolerance);
}

inline CFuzzy<float, float> Fuzzy(float f_value, float f_tolerance = fTOLERANCE_DEFAULT)
{
	return CFuzzy<float, float>(f_value, f_tolerance);
}

inline CFuzzy<double, double> Fuzzy(double d_value, double d_tolerance = dTOLERANCE_DEFAULT)
{
	return CFuzzy<double, double>(d_value, d_tolerance);
}

//******************************************************************************************
//
inline bool bFurryEquals
(
	double d_a, double d_b,					// The values to compare.
	double d_tolerance = dTOLERANCE_DEFAULT	// The tolerance for equality.
)
//
// Equality comparision with relative tolerance.
//
// Returns:
//		'true' if the values are equal to within some relative precision, 'false' otherwise.
//
//**************************************
{
	return Abs(d_a - d_b) <= d_tolerance * Max(Abs(d_a), 1.0);
}


//******************************************************************************************
//
inline bool bFurryEquals
(
	float f_a, float f_b,					// The values to compare.
	float f_tolerance = fTOLERANCE_DEFAULT	// The tolerance for equality.
)
//
// Equality comparision with relative tolerance.
//
// Returns:
//		'true' if the values are equal to within some relative precision, 'false' otherwise.
//
//**************************************
{
	return Abs(f_a - f_b) <= f_tolerance * Max(Abs(f_a), 1.0f);
}

//******************************************************************************************
//
template <class T> inline T Sign
(
	T t
)
//
// Determine the sign of the given parameter.
//
// Returns:
//		-1 when t <  0
//		 0 when t == 0
//		 1 when t >  0
//		Note that the return type is identical to the type of the parameter.
//
//**************************************
{
	return t < (T)0 ? -1 : t > 0? 1 : 0;
}



//******************************************************************************************
//
template <class T> inline T* Offset
(
	T*  p_array,
	int i_offset
)
//
// Takes a pointer and an offset and returns a pointer to the element in the array being
// referred to.
//
// Returns:
//		A typed pointer to the member of the array referred to by the offset.
//
//**************************************
{
	return (T *)(((char *)p_array) + i_offset);
}



//******************************************************************************************
//
template<class T> inline void Swap
(
	T& rt_a,	// References to the parameters to swap.
	T& rt_b
)
//
// Function to swap two variables of any type.
//
// Side effects:
//		The parameters are passed by reference and are modified.
//
//**************************************
{
	T t_temp;

	t_temp = rt_a;
	rt_a   = rt_b;
	rt_b   = t_temp;
}

//******************************************************************************************
//
template<class T, class U> void Assign(T& t, const U& u)
//
// Standard assignment operation, which first destroys t, then reconstructs it as u.
//
// Cross-references:
//		Convenient to call in canonical assignment operators.
//
// Notes:
//		In general, it is essential to check for self-assignment before calling Assign.
//		It would be wrong to destroy t before copying it to itself.  
//
// Example:
//		class CWidget
//		{
//			...
//			CWidget& operator =(const CGadget& g)
//			{
//				if (*this != g)
//					Assign(*this, g);
//				return *this;
//			}
//		};
//
//**************************************
{
	// First call t's destructor.
	t.~T();

	// Then call its constructor for u.
	new(&t) T(u);
}

//******************************************************************************************
//
template<class T> inline T* pNonConst
(
	const T* pt
)
//
// Returns:
//		The pointer with constness cast away.
//
// Notes:
//		As every C++ book will tell you, casting away const is a Bad Thing.  Never, EVER,
//		do it under any circumstance.  And when you do, use this function, which lets
//		you avoid respecifying the type when the compiler already knows it.
//
// Example:
//		const Monster* pmon_c;
//		Monster* pmon;
//
//		// Various ways of casting away const.
//		pmon = pmon_c;					// Error, conversion of const to non-const.
//		pmon = (Monster*)pmon_c;		// Valid, but unsafe, because see below.
//		pmun = (Munster*)pmon_c;		// Uncaught error.  You intended to cast away const,
//										// but actually cast to another type.  The compiler
//										// doesn't catch this.
//		pmon = const_cast<Monster*>(pmon_c);	// Valid, and safe, but verbose.
//		pmon = pNonConst(pmon_c);		// The best way in the world to do it.
//
//**************************************
{
	return const_cast<T*>(pt);
}

//******************************************************************************************
//
template<class T> inline T& NonConst
(
	const T& pt
)
//
// Returns:
//		The reference with constness cast away.
//
// Notes:
//		See function above.
//
//**************************************
{
	return const_cast<T&>(pt);
}

//******************************************************************************************
//
template<class T> inline T BitMask
(
	T t_bit_num		// The bit number.
)
//
// Function to convert a bit number into a mask (i.e. a value that can be used in an AND
// operation to isolate that bit).
//
// Returns:
//		The mask equivalent of 't_bit_num'. Note that the return type is identical to the
//		type of the parameter.
//
//**************************************
{
	// Ensure the parameter is within the range of the type.
	Assert(t_bit_num >= (T)0 && t_bit_num < (T)(sizeof(T) * 8));

	return ((T)1 << t_bit_num);
}



//******************************************************************************************
//
template<class T> inline T BitMaskRange
(
	T t_bit_num_start,	// The starting bit number.
	T t_width			// The number of bits following 't_bit_num_start'.
)
//
// Function to convert several consecutive bits into a mask.
//
// Returns:
//		The mask equivalent. Note that the return type is identical to the type of the
//		parameters.
//
// Notes:
//		The current implementation cannot generate a mask that includes the MSB of the given
//		type.
//
//**************************************
{
	// Ensure the parameter is within the range of the type.
	Assert(t_bit_num_start >= (T)0);
	Assert(t_width         >= (T)0);
	Assert(t_bit_num_start + t_width < (T)(sizeof(T) * 8));

	return ( (((T)1 << (T)t_width) - (T)1) << (T)t_bit_num_start );
}


//******************************************************************************************
//
// int iNumArrayElements
// (
//		a_array
// )
//
// Macro to determine the number of elements in a static array.
//
// Returns:
//		The number of elements in the array.
//
//**************************
#define iNumArrayElements(a_array)	(int)(sizeof(a_array) / sizeof(a_array[0]))
		 


//******************************************************************************************
// 
inline uint uLog2
(
	uint32 u4_val
)
//
// Returns:
//		The highest power of two in the value, or zero in case of a zero parameter.
//
//**************************
{
	uint u_msb16 = ((u4_val & 0xFFFF0000u) != 0) << 4;
	u4_val >>= u_msb16;

	uint u_msb8 = ((u4_val & 0xFF00u) != 0) << 3;
	u4_val >>= u_msb8;

	uint u_msb4 = ((u4_val & 0xF0u) != 0) << 2;
	u4_val >>= u_msb4;

	uint u_msb2 = ((u4_val & 0xCu) != 0) << 1;
	u4_val >>= u_msb2;

	uint u_msb1 = (u4_val >> 1) & 1;

	uint u_msb = u_msb16 + u_msb8 + u_msb4 + u_msb2 + u_msb1;

	return u_msb;
}



//******************************************************************************************
// 
template <class T> inline T RoundUp
(
	T t_orig,							// The number to round up.
	T t_round							// The multiple to round to.
)
//
// Returns:
//		The next multiple of t_round above or equal to t_orig.
//
//**************************
{
	t_orig += t_round - (T)1;
	return t_orig - (t_orig % t_round);
}

//******************************************************************************************
// 
template <class T> inline T RoundDown
(
	T t_orig,							// The number to round up.
	T t_round							// The multiple to round to.
)
//
// Returns:
//		The next multiple of t_round below or equal to t_orig.
//
//**************************
{
	return t_orig - (t_orig % t_round);
}

//*****************************************************************************************
//
template<class T> inline void Clamp
(
	T& value,	// Value to keep in range.
	T  range
)
//
// Keeps f_value in the range -range <= value <= range.
//
//**************************************
{
	if (value < (T)0)
	{
		if (value < -range)
			value = -range;
	}
	else
	{
		if (value > range)
			value = range;
	}
}

//******************************************************************************************
//
template<class T> inline T Sqr
(
	const T t
)
//
// Returns the square of the value.
//
//**************************************
{
	return t * t;
}

//*****************************************************************************************
//
// forall
// (
//		container,						// An STL container object.
//		type,							// The type of the container.
//		it								// A variable name for an iterator.
//	)
//
//  Expands to a for loop iterating through all elements of the container.
//
//**************************************
#define forall(container, type, it)		\
	for (type::iterator it = container.begin(); it != container.end(); it++)

//*****************************************************************************************
//
// forall_const(container, type, it)
//
// Like forall, but uses a const_iterator.
//
//**************************************
#define forall_const(container, type, it)		\
	for (type::const_iterator it = container.begin(); it != container.end(); it++)

//*****************************************************************************************
//
template<class T> void erase_all
(
	T& t_container
)
//
//**************************************
{
	t_container.erase(t_container.begin(), t_container.end());
}

//*********************************************************************************************
//
inline void* pvGetVTable
(
	void* pv_object
)
//
// Returns the object's virtual table.
//
// Notes:
//		This may be considered a hack because the position and size of the V table may be
//		dependent on machine an compiler types. However this should work for all common x86,
//		32-bit Windows compilers.
//
//**************************************
{
	Assert(pv_object);
	Assert(sizeof(void*) == sizeof(uint32));

	//
	// Recast as an array of 32 bit values, get the top value and cast it as a pointer to
	// a type void.
	//
	return (void*)(*((uint32*)pv_object));
};

//*********************************************************************************************
//
inline void PokeVTable
(
	void* pv_object,
	void* pv_table
)
//
// Sets the object's V table with the specified value.
//
// Notes:
//		This may be considered a hack because the position and size of the virtual table may be
//		dependent on machine an compiler types. However this should work for all common x86,
//		32-bit Windows compilers.
//
//**************************************
{
	Assert(pv_object);
	Assert(pv_table);
	Assert(sizeof(void*) == sizeof(uint32));

	// Recast as an array of 32 bit values and poke the top value.
	*((uint32*)pv_object) = uint32(pv_table);
};

//
// Some useful macros.
//

// Macro to alias a float as an int.
#define u4FromFloat(f) (*(uint32*)&(f))

// Macro to alias an int as a float.
#define fToFloat(u)    (*(float*)&(u))


/*
//*********************************************************************************************
//
template<class T> class CDelete
//
// Keeps track of pointers to be deleted. Pointers to be deleted are added with the
// overloaded '<<' operator and actually deleted when the destructor for the class is
// called. Only one pointer per address will be deleted.
//
// Notes:
//		This class is only intended to prevent multiple deletions of the same pointer
//		until reference counting for pointers is implemented.
//
//**************************************
{
private:

	set<T, less<T> > setT;	// STL container to allow only one pointer per key.

public:

	//*****************************************************************************************
	//
	// Destructor.
	//

	//*****************************************************************************************
	~CDelete()
	{
		// Iterate through set and delete pointers.
		for (set<T, less<T> >::iterator it = setT.begin(); it != setT.end(); it++)
		{
			T t = *it;
			delete t;
		}
	}

	//*****************************************************************************************
	//
	// Overloaded operators.
	//

	//*****************************************************************************************
	void Delete(T pt)
	{
		// Insert the pointer into the set.
		setT.insert(pt);
	}

};
*/

#endif
