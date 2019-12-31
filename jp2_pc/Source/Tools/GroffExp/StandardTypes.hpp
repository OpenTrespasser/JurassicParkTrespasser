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

//
// Define boolean type, if it hasn't been defined.
//
/*
#ifndef bool
#define bool int
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif
*/
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
// Define the floating point 2 and 3 element vector types.
//

#ifndef fvector2
typedef struct 
//
// Prefix: fv2
//
{
	float X;
	float Y;
} fvector2;
#endif

#ifndef fvector3
typedef struct 
//
// Prefix: fv3
//
{
	float X;
	float Y;
	float Z;
} fvector3;
#endif


//
// Define the unsigned integer 2 and 3 element vector types.
//

#ifndef uvector2
typedef struct 
//
// Prefix: uv2
//
{
	uint	X;
	uint	Y;
} uvector2;
#endif

#ifndef uvector3
typedef struct 
//
// Prefix: uv3
//
{
	uint	X;
	uint	Y;
	uint	Z;
} uvector3;
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