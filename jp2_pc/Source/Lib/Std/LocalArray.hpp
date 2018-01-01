/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/Std/LocalArray.hpp                                                $
 * 
 * 1     3/10/98 11:19a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_STD_LOCALARRAY_HPP
#define HEADER_LIB_STD_LOCALARRAY_HPP


//
// Required includes.
//
#include "Array.hpp"


//
// The following macros allocate and declare an array on the stack.  The array can be used
// just like a CPArray.  
//

// A CPArray using memory allocated by _alloca.  Does NOT initialise its elements.
#define CLArray(Type, Name, u_size) \
	CPArray< Type > Name(u_size, (Type*)_alloca((u_size) * sizeof(Type)) )

//
// A version of CLArray which initialises its elements, calling the default constructor for each.
// This is accomplished by invoking the array new operator.  We must allocate one word more 
// than needed, because the MSVC implementation of 'array new' stores the array length there.
//
#define CLArrayInit(Type, Name, u_size) \
	CPArray< Type > Name(u_size, new(_alloca((u_size) * sizeof(Type) + sizeof(int))) Type[u_size] )

// A CMAArray using memory allocated by _alloca.
// Since the used length is initialised to 0, this macro also does not initialise its elements.
#define CMLArray(Type, Name, u_size) \
	CMArray< Type > Name(u_size, (Type*)_alloca((u_size) * sizeof(Type)) )


#endif // HEADER_LIB_STD_LOCALARRAY_HPP
