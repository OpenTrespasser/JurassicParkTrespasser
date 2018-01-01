/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Decl's for functions and variables used to implement the platonic instancer (see notes)
 *
 * Bugs:
 *
 * To do:
 *		Replace this module with a more permanent solution.
 *
 * Notes:
 *		The platonic instancer is designed with the idea that every object has an ideal version
 *		version of itself, and all other versions are pale copies of the one ideal version.
 *
 *		In our case, all other versions are indentical to the original in terms of physics, 
 *		mesh, and AI opinions.
 *
 *		The platonic instancer keeps a list of all ideal objects (used as the original) and provides
 *      information to the loader to allow it to take advantage of the ideal objects.
 *
 *		At this time, an ideal object is an object whose name is of the form "XXXXXXX-00"
 *		Only one dash is allowed in the name of the object.  All objects of the form "XXXXXXX-XX"
 *      are considered to be copies of "XXXXXXX-00", regardless of any differences between the two.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Loader/PlatonicInstance.hpp                                      $
 * 
 * 4     8/19/98 1:36p Rwyatt
 * Platonic instances now use hash values instead of names
 * 
 * 3     10/27/97 4:35p Agrant
 * Added the bIsPlatonicIdealName function 
 * 
 * 2     10/16/97 1:56p Agrant
 * Now instancing all CInfo on object name.  Object-00 is always the base object from which we
 * instance.
 * 
 * 1     10/14/97 9:12p Agrant
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_LOADER_PLATONICINSTANCE_HPP
#define HEADER_GUIAPP_LOADER_PLATONICINSTANCE_HPP


#include <map.h>
#include <bstring.h>

class CInstance;

//*****************************************************************************************
//
bool bGetPlatonicIdealName
(
   const char* ac_source,			// The name to be analyzed.
   char* ac_ideal_dest		// The return buffer to be filled with an ideal name (zero if not desired).
);
//
// Gets the ideal name.
//
//	Returns:
//		true if the source is an ideal name, otherwise false.
//		If no ideal name exists, ac_ideal_dest[0] is set to '\0'
//
//**************************************

//*****************************************************************************************
//
bool bIsPlatonicIdealName
(
   const char* ac_source			// The name to be analyzed.
);
//
// Returns:
//		true if the source is an ideal name, otherwise false.
//
//**************************************

//*****************************************************************************************
//
void AddToIdealList
(
   const uint32 u4_hash,	// The hash of the name of the ideal instance.
   const CInstance *pins	// The ideal instance.
);
//
// Remembers the ideal instance for future instancing.
//
//**************************************

//*****************************************************************************************
//
void RemoveFromIdealList
(
	const uint32 u4_hash			// The hash of the name of the ideal instance to remove
);
//
// Remembers the ideal instance for future instancing.
//
//**************************************

typedef map<uint32, const CInstance*, less<uint32> >	TMapHashPins;


//
//  Globals
//
extern TMapHashPins  tmPlatonicIdeal;


#endif