/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	 	Contains standard hashing functions.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/Hash.hpp                                                     $
 * 
 * 4     7/08/98 12:43a Rwyatt
 * New function to enable hashing across multiple blocks/strings
 * 
 * 3     3/09/98 10:48p Rwyatt
 * New function to CRC a string without first getting its length
 * 
 * 2     96/12/15 18:30 Speter
 * Moved u4Hash function to .cpp file.
 * 
 * 1     11/17/96 3:09p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_STD_HASH_HPP
#define HEADER_LIB_STD_HASH_HPP


//
// Includes.
//

#include "Lib/Std/StringEx.hpp"


//
// Global functions.
//

//**********************************************************************************************
//
uint32 u4Hash
(
	const void* pv,					// Pointer to the data to derive a hash value from.
	int         i_size_bytes,		// Size of data.
	bool        b_string = false	// Set to 'true' if the data is a string. True will invoke
									// string conversion to lower case, false will ignore
									// conversions.
);
//
// Returns a unique hash value given a block of data.
//
// Notes:
//		There is a one in 4294967296 that two random strings or blocks of data four or more
//		bytes long will be the same.
//
//**************************************


//**********************************************************************************************
//
inline uint32 u4Hash
(
	const char* str	// Pointer to the string to derive a hash value from.
)
//
// Returns a unique hash value given a string.
//
// Notes:
//		This function is case insensitive.
//
//		There is a one in 4294967296 that two random strings or blocks of data four or more
//		bytes long will be the same.
//
//**************************************
{
	Assert(str);
	Assert(uStrLen(str) > 0);

	return u4Hash((const void*)str, 0, true);
}



//**********************************************************************************************
//
uint32 u4HashContinue
(
	const void* pv,					// Pointer to the data to derive a hash value from.
	int         i_size_bytes,		// Size of data.
	bool        b_string = false	// Set to 'true' if the data is a string. True will invoke
);
//
// Called after u4Hash to combine more data into the previous hash value.
//
//**************************************


#endif
