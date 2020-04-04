/***********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive 1996-1997
 *
 * Contents: File for defining some of the common types.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/StandardTypes.hpp                                      $
 * 
 * 5     6/18/97 7:33p Gstull
 * Added changes to support fast exporting.
 * 
 **********************************************************************************************/

#ifndef HEADER_TOOLS_GROFFEXP_STANDARDTYPES_HPP
#define HEADER_TOOLS_GROFFEXP_STANDARDTYPES_HPP

#include "Lib/Groff/Groff.hpp"

//
// Define signed scalar types.
//

#ifndef int8
typedef signed char int8;
#endif

#ifndef int16
typedef short int int16;
#endif

#ifndef int32
typedef int int32;
#endif


//
// Define unsigned scalar types.
//

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef uint8
typedef unsigned char uint8;
#endif

#ifndef uint16
typedef unsigned short int uint16;
#endif

#ifndef uint32
typedef unsigned int uint32;
#endif



//
// Define the floating point quaternion type.
//

#ifndef quaternion
typedef struct
//
// Prefix: q
//
{
	float X;
	float Y;
	float Z;
	float W;
} quaternion;

#endif

#endif