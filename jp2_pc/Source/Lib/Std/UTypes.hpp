/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Global header file defining portable synonyms for the base C++ types, and macros to
 *		obtain the numeric range of these types.
 *
 *		This header file is automatically included by "Common.hpp".
 *
 *		The following additional types anf constants are made available by this file:
 *
 *			uint				Unsigned integer.
 *			int8,   uint8		Signed and unsigned 8 bit integer (a byte).
 *			int16,  uint16		Signed and unsigned 16 bit integer.
 *			int32,  uint32		Signed and unsigned 32 bit integer.
 *			int64,  uint64		Signed and unsigned 64 bit integer.
 *			bool				Boolean
 *			true,   false		Boolean true and false.
 *
 *			ushort, ulong		Unsigned short and unsigned long integer.
 *
 *		Note:
 *		Avoid the use of the short, ushort, long and ulong types! Wherever possible use
 *		portable equivalents, such as int16.
 *
 * Bugs:
 *
 * To do:
 *		Remove the definitions of bool, true and false once the compiler supports these
 *		constructs.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/UTypes.hpp                                                    $
 * 
 * 12    9/04/97 5:48p Mlange
 * Fixed TypeMin macro to avoid compiler warnings.
 * 
 * 11    8/28/97 4:06p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 11    97/08/22 6:38p Pkeet
 * Added the 'TBoolAsm' type.
 * 
 * 10    97-03-28 16:38 Speter
 * Fixed UTypeMax macro, explicitly cast it to return type.
 * 
 * 
 * 
 * 9     12/20/96 4:34p Mlange
 * Now works with GNU as well.
 * 
 * 8     5/07/96 2:07p Mlange
 * The 16 and 32 bit user types are now defined by the numerical range set in limits.h.
 * 
 * 7     96/05/07 11:04 Speter
 * Changed definition of int32 from __int32 to int to workaround compiler bug.
 * 
 * 6     5/03/96 11:42a Mlange
 * Added some comments.
 * 
 * 5     5/03/96 11:13a Mlange
 * Changes from code review. Now no longer defines bool, true and false in this header file but
 * instead imports header file from STL. Removed the constants for the limits of the types.
 * Added generic macros for obtaining these limits instead.
 * 
 * 4     4/30/96 1:47p Mlange
 * Updated for changes to the coding standards.
 * 
 * 3     4/18/96 4:59p Mlange
 * Updated for changes to the coding standards.
 * 
 * 2     4/16/96 10:49a Mlange
 * Added definitions for the true and false constants.
 * 
 * 1     4/12/96 3:23p Mlange
 * Defines portable synonyms for the base types.
 *
 **********************************************************************************************/

#ifndef HEADER_GBLINC_UTYPES_HPP
#define HEADER_GBLINC_UTYPES_HPP


// Get limits of built-in types needed to define the user types.
#include <limits.h>


//
// Type definitions.
//
// Compiler bug: We define int32 and uint32 in terms of "int" rather than __int32,
// because the latter causes a spurious compiler error in conjunction with
// the template function Abs(), in MS VC++.
//

typedef signed   char  int8;
typedef unsigned char  uint8;

#if UINT_MAX == 0xffffU
	// The compiler has 16 bit integers.
	typedef 		 int  int16;
	typedef unsigned int  uint16;

#elif USHRT_MAX == 0xffffU
	// The compiler has 16 bit short integers.
	typedef 		 short int  int16;
	typedef unsigned short int  uint16;

#else
#	error Cannot define types: int16, uint16.

#endif


#if UINT_MAX == 0xffffffffUL
	// The compiler has 32 bit integers.
	typedef			 int  int32;
	typedef unsigned int  uint32;

#elif ULONG_MAX == 0xffffffffUL
	// The compiler has 32 bit long integers.
	typedef			 long int  int32;
	typedef unsigned long int  uint32;

#else
#	error Cannot define types: int32, uint32.

#endif



#if defined(_MSC_VER)
	typedef          __int64  int64;
	typedef unsigned __int64  uint64;

#elif defined(__GNUC__)
	typedef          long long  int64;
	typedef unsigned long long  uint64;
#endif


typedef unsigned short ushort;
typedef unsigned long  ulong;
typedef unsigned int   uint;


//
// Macros to obtain the minimum and maximum value that can be stored in an integer type.
// The result returned by these macros is of the same type as the parameter.
//

// Signed minimum and maximum.
#define TypeMin(T)  T( T(1) << (sizeof(T) * 8 - 1) )
#define TypeMax(T)  (~T(0) ^ TypeMin(T))

// Unsigned maximum.
#define UTypeMax(T) T(~T(0))

//
// Types for ASM use.
//
typedef int TBoolAsm;


#endif
