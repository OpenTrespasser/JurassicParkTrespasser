/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Classes for using, or not using, transparencies in raster primitives.
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/TransparencyT.hpp                             $
 * 
 * 2     98.04.30 5:22p Mmouni
 * Added support for stippled texture.
 * 
 * 1     8/22/96 11:00a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_TRANSPARENCYT_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_TRANSPARENCYT_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
class CTransparencyOn
//
// Defines a class for rasterizing primitives with transparency.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CTransparencyOn member functions.
	//

	//*****************************************************************************************
	//
	static bool bIsOpaquePixel
	(
		uint32 u4
	)
	//
	// Returns 'true' if the pixel is opaque and should be copied, otherwise returns 'false.'
	//
	//**************************************
	{
		return u4 != 0;
	}
};


//*********************************************************************************************
//
class CTransparencyOff
//
// Defines a class for rasterizing primitives without transparency.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CTransparencyOff member functions.
	//

	//*****************************************************************************************
	//
	static bool bIsOpaquePixel
	(
		uint32 u4
	)
	//
	// Returns 'true' if the pixel is opaque and should be copied, otherwise returns 'false.'
	//
	//**************************************
	{
		return true;
	}

};


//*********************************************************************************************
//
class CTransparencyStipple
//
// Defines a class for rasterizing primitives with stippled transparency.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CTransparencyOff member functions.
	//

	//*****************************************************************************************
	//
	static bool bIsOpaquePixel
	(
		uint32 u4
	)
	//
	// Returns 'true' if the pixel is opaque and should be copied, otherwise returns 'false.'
	//
	//**************************************
	{
		return true;
	}

};


#endif
