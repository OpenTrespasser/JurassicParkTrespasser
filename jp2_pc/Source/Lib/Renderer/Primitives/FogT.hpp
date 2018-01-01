/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Classes for using, or not using, fogging in raster primitives.
 *
 * Notes:
 *
 * To Do:
 *		Find a faster way to fall through the 'SetFlatColour' member function if the
 *		'CLineBumpMake' line primitive is being used.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/FogT.hpp                                      $
 * 
 * 14    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 13    12/20/96 12:10p Mlange
 * Moved fog source files to 'Renderer' directory.
 * 
 * 12    12/15/96 2:44p Mlange
 * CFogOn::u4Offset() now always returns 0.
 * 
 * 11    10/14/96 10:06a Pkeet
 * Made 'iDefaultFog' extern instead of static.
 * 
 * 10    10/09/96 7:18p Pkeet
 * Moved the 'iDefaultFog' global variable here.
 * 
 * 9     10/03/96 3:15p Pkeet
 * Removed the 'pvGetClutAddress' global function.
 * 
 * 8     9/27/96 4:31p Pkeet
 * Removed the 'SetFlatColour' member function.
 * 
 * 7     96/09/25 15:40 Speter
 * Changed pvAddFog to u4GetClutOffset.
 * 
 * 6     9/24/96 2:52p Pkeet
 * Added checks to ensure that the 'SetFlatColour' member function only sets a colour if a clut
 * is present.
 * 
 * 5     9/18/96 3:06p Pkeet
 * Made the 'pvGetClutAddress' global function static. Disabled the 'SetFlatColour' member
 * function for the 'CFogOff' class.
 * 
 * 4     96/09/12 16:24 Speter
 * Incorporated new TReflectVal usage.
 * 
 * 3     96/09/09 18:30 Speter
 * Made compatible with change of fIntensity to lvIntensity.
 * 
 * 2     8/22/96 2:07p Pkeet
 * Changed member functions to static member functions.
 * 
 * 1     8/21/96 4:45p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_FOGT_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_FOGT_HPP


//
// Global includes.
//
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"
#include "Lib/Renderer/Fog.hpp"


//
// Global variables.
//

// Default fogging value.
extern int iDefaultFog;


//
// Externally defined variables.
//
extern CFog fogFog;


//
// Class definitions.
//

//*********************************************************************************************
//
class CFogOn
//
// Defines a class for rasterizing primitives with fog.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	static forceinline uint32 u4Offset
	(
		int32 i4_invz
	)
	//
	// Returns the offset into the clut for a given fog level.
	//
	//**************************************
	{
		return 0;
	}

};


//*********************************************************************************************
//
class CFogOff
//
// Defines a class for rasterizing primitives without fog.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	static forceinline uint32 u4Offset
	(
		int32 i4_invz
	)
	//
	// Returns zero.
	//
	//**************************************
	{
		return 0;
	}

};


#endif
