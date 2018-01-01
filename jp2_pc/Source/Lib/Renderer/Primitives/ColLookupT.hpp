/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Classes for using, or not using, colour lookup tables in raster primitives.
 *
 * Notes:
 *
 * To Do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/ColLookupT.hpp                               $
 * 
 * 6     98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 5     98.07.17 6:36p Mmouni
 * Added  new alpha texture primitive.
 * 
 * 4     97/11/10 11:27a Pkeet
 * Added 'CColLookupAlphaWater.'
 * 
 * 3     97/11/03 5:51p Pkeet
 * Added the 'CColLookupTerrain' class.
 * 
 * 2     10/11/96 10:31a Pkeet
 * Added 'pvClutConversion' as a global variable instead of a member of 'CDrawTriangle.' No
 * clut pointer is therefore passed to the 'DrawLoop' functions.
 * 
 * 1     10/09/96 7:18p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_COLLOOKUPT_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_COLLOOKUPT_HPP


//
// Includes.
//
#include "Lib/Renderer/LightBlend.hpp"


//
// Externally defined global variables.
//

// Pointer to a clut location for source to destination pixel conversions.
extern void* pvClutConversion;


//
// Class definitions.
//

//*********************************************************************************************
//
class CColLookup
//
// Defines a base class for using cluts.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CColLookup member functions.
	//

	//*****************************************************************************************
	//
	static forceinline bool bUseClut
	(
	)
	//
	// Returns 'false.'
	//
	//**************************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	static forceinline bool bUseAlphaClut
	(
	)
	//
	// Returns 'false.'
	//
	//**************************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	static forceinline void AssertClut
	(
		void* pv_clut
	)
	//
	// Does nothing in the base class.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	static forceinline uint16* SetAlphaClutPointer
	(
		int i_fogband = 0
	)
	//
	// Returns a pointer to the alpha clut.
	//
	//**************************************
	{
		return 0;
	}

};


//*********************************************************************************************
//
class CColLookupOn : public CColLookup
//
// Defines a class for rasterizing primitives with a clut.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	static forceinline bool bUseClut
	(
	)
	//
	// Returns 'true.'
	//
	//**************************************
	{
		return true;
	}

	//*****************************************************************************************
	//
	static void AssertClut
	(
		void* pv_clut
	)
	//
	// Causes an assert to fail if a clut is not present.
	//
	//**************************************
	{
		Assert(pv_clut);
	}

};


//*********************************************************************************************
//
class CColLookupOff : public CColLookup
//
// Defines a class for rasterizing primitives without using a clut.
//
//**************************************
{
};


//*********************************************************************************************
//
class CColLookupTerrain : public CColLookup
//
// Defines a class for rasterizing primitives with a special alpha (for fog) terrain clut.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	static forceinline bool bUseAlphaClut
	(
	)
	//
	// Returns 'false.'
	//
	//**************************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	static forceinline uint16* SetAlphaClutPointer
	(
		int i_fogband = 0
	)
	//
	// Returns a pointer to the alpha clut.
	//
	//**************************************
	{
		// Get the index into the alpha table.
		uint32 u4_bits_index = uint32(lbAlphaTerrain.u2GetAlphaReference(i_fogband));

		// Get a pointer to the alpha table position.
		uint16* pu2 = lbAlphaTerrain.au2Colour + u4_bits_index;

		// Return the assembled pointer.
		return pu2;
	}
};


//*********************************************************************************************
//
class CColLookupAlphaWater : public CColLookup
//
// Defines a class for rasterizing primitives with a special alhpa water clut.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	static forceinline bool bUseAlphaClut
	(
	)
	//
	// Returns 'false.'
	//
	//**************************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	static forceinline uint16* SetAlphaClutPointer
	(
	)
	//
	// Returns a pointer to the alpha clut.
	//
	//**************************************
	{
		// Return the assembled pointer.
		return lbAlphaTerrain.au2Colour;
	}
};


//*********************************************************************************************
//
class CColLookupAlphaTexture : public CColLookup
//
// Defines a class for rasterizing primitives with a special alhpa texture clut.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	static forceinline bool bUseAlphaClut
	(
	)
	//
	// Returns 'false.'
	//
	//**************************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	static forceinline uint16* SetAlphaClutPointer
	(
	)
	//
	// Returns a pointer to the alpha clut.
	//
	//**************************************
	{
		// Return the assembled pointer.
		return abAlphaTexture.u2ColorToAlpha;
	}
};


#endif
