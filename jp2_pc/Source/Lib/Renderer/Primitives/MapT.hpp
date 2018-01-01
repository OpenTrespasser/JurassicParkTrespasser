/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Classes for using different mapping techniques in raster primitives.
 *
 * Notes:
 *		Class hierarchy:
 *
 *			CMap<>
 *				CMapShadow
 *				CMapFlat
 *				CMapTexture<>
 *					CMapBump
 *						CMapBumpNoTex
 *						CMapBumpTable
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/MapT.hpp                                      $
 * 
 * 31    98/02/26 13:47 Speter
 * Added seterfFeatures to CTexture, removing redundant flags.
 * 
 * 30    1/19/98 7:30p Pkeet
 * Added support for 16 bit bumpmaps by adding lower colour resolution and a smaller size for
 * 'CBumpAnglePair.'
 * 
 * 29    97/10/23 10:57a Pkeet
 * Added a K6 3D switch.
 * 
 * 28    97/10/12 20:45 Speter
 * Replaced BumpLookup with CBumpTable.  Replaced au1CosToIntensity with au1IntensityTrans.
 * 
 * 27    8/19/97 12:12a Rwyatt
 * Added 32 bit shadow primitive
 * 
 * 26    8/18/97 5:45p Bbell
 * Eliminated the 'CMapFlatConstColour' class. Made the 'CMapFlat' class non-templated. Added
 * the 'bSetConstColour' member function.
 * 
 * 25    97/08/14 6:40p Pkeet
 * Debugged the 'Assign' function. Added a CMapAlphaColour class.
 * 
 * 24    97/08/14 15:45 Speter
 * New CMapShadow template, courtesy Raul.
 * 
 * 23    97/08/11 17:58 Speter
 * Adjustments for bumpmap tpSolid now in CBumpAnglePair format.
 * 
 * 22    7/07/97 11:53p Rwyatt
 * No includes headers that contain assembly code from the processor specific directories
 * 
 * 21    97/06/27 15:32 Speter
 * Removed SetBaseIntensity function; now handled by CGouraudNone template.
 * 
 * 20    97/06/25 12:45p Pkeet
 * Support for a wireframe mode.
 * 
 * 19    97-04-23 17:53 Speter
 * Removed specular functions by compile flag.
 * 
 * 18    97-04-23 14:29 Speter
 * Moved bangLight and bangSpecular variables to CBumpLookup.
 * 
 * 17    4/13/97 7:02p Cwalla
 * Made u4ConstColour global,
 * 
 * 16    97/02/07 3:25p Pkeet
 * Added the 'CMapFlatConstColour' template.
 * 
 * 15    97/01/07 12:05 Speter
 * Updated for ptr_const.
 * 
 * 14    96/12/31 17:05 Speter
 * Changed some pointers to ptr<>.
 * 
 * 13    96/12/05 14:32 Speter
 * Added bREFLECTION_PRIMARY flag to control method of reflection bump-mapping.  Moved
 * au1BumpFirst pointer into CBumpLookup, set automatically.
 * 
 * 12    96/12/04 11:47 Speter
 * Changed for reorganised bump table class.
 * Now use au1BumpFirst table.
 * 
 * 11    96/11/25 13:44 Speter
 * Changed Light variable to bangLight, added bangSpecular.
 * Implemented CMapBumpSpec::u1GetIntensity().
 * 
 * 10    11/22/96 12:44p Pkeet
 * Added the CMapBumpSpec class and globals.
 * 
 * 9     96/11/18 20:55 Speter
 * Changed au1BumpToArcCos name to au1BumpToCos.
 * 
 * 8     11/07/96 3:26p Pkeet
 * Added the 'CMapBumpNoTex' class.
 * 
 * 7     11/05/96 5:32p Pkeet
 * Fixed 'GetColour' bug for CMapBump.
 * 
 * 6     10/14/96 10:09a Pkeet
 * Added the 'SPIX' template parameter to classes inherited from 'CMap.'
 * 
 * 5     10/11/96 12:26p Pkeet
 * Made the pointer to the texture bitmap a global variable.
 * 
 * 4     10/11/96 10:31a Pkeet
 * Made the lighting globals externally defined.
 * 
 * 3     10/10/96 2:44p Pkeet
 * Changed 'SetBaseIntensity' for bumpmaps to set the base intensity to 0.
 * 
 * 2     10/04/96 5:35p Pkeet
 * Changed 'SourcePixel' to 'TSourcePixel.' Removed the 'bIsIndexed' member functions.
 * 
 * 1     10/03/96 6:10p Pkeet
 * Moved mapping implementations here from 'BumpMapT.hpp.' Added the CMapFlat and CMapTexture
 * member functions.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_MAPT_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_MAPT_HPP


//
// Includes.
//
#include "Config.hpp"
#include "Lib/Renderer/Primitives/FastBump.hpp"
#include "Lib/Renderer/Primitives/FastBumpTable.hpp"
//**********************************************************************************************
#if TARGET_PROCESSOR == PROCESSOR_PENTIUM
	#include "Lib/Renderer/Primitives/P5/FastBumpEx.hpp"
#elif TARGET_PROCESSOR == PROCESSOR_PENTIUMPRO
	#include "Lib/Renderer/Primitives/P6/FastBumpEx.hpp"
#elif TARGET_PROCESSOR == PROCESSOR_K6_3D
	#include "Lib/Renderer/Primitives/AMDK6/FastBumpEx.hpp"
#else
	#error Invalid [No] target processor specified
#endif
//**********************************************************************************************


//
// Externally defined global variables.
//

// Theta constant of current light relative to the polygon's texturemap.
extern uint32 u4LightTheta;
extern uint32 u4SpecularTheta;

// Pointer to the location in the bumpmap to intensity lookup table based on the light's phi.
extern uint8* au1SubBumpToIntensity;
extern uint8* au1IntensityTrans;

#if !bFOLD_SPECULAR
extern uint8* au1SubBumpToCosSpec;
#endif

// Pointer to the base address of a bitmap associated with the currently rendered texture.
extern const void* pvTextureBitmap;

// Constant colour for flat shading.
extern uint32 u4ConstColour;

// Alpha colour values.
extern uint16  u2AlphaColourMask;
extern uint16  u2AlphaColourReference;
extern uint16* au2AlphaTable;

// Shadow lighting intensity.
extern uint8 u1ShadowIntensity;


//
// Class definitions.
//

//*****************************************************************************************
//
template<class DESTTYPE, class SOURCETYPE, class MAPTYPE> inline void Assign
(
	DESTTYPE& rdest,
	SOURCETYPE spix_source,
	MAPTYPE* pmt
)
//
// Copies the pixel value over.
//
//**************************************
{
	rdest = spix_source;
}

//*********************************************************************************************
//
template<class SPIX> class CMap
//
// Defines a class for rasterizing primitives without bumpmaps.
//
//**************************************
{
public:

	typedef SPIX TSourcePixel;
	typedef int TDummy;

public:

	//*****************************************************************************************
	//
	// CMap member functions.
	//

	//*****************************************************************************************
	//
	static uint32 u4GetClutBaseColour
	(
		ptr_const<CTexture> ptex
	)
	//
	// Returns the colour value for flat shaded polygons.
	//
	//**************************************
	{
		return 0;
	}

	//*****************************************************************************************
	//
	static uint32 u4GetColour
	(
		TSourcePixel spix
	)
	//
	// Returns the colour value referenced by the map.
	//
	//**************************************
	{
		return uint32(spix);
	}

	//*****************************************************************************************
	//
	static uint8 u1GetIntensity
	(
		TSourcePixel spix
	)
	//
	// Returns the intensity value referenced by the "bump."
	//
	//**************************************
	{
		return 0;
	}

	//*****************************************************************************************
	//
	static uint32 u4Offset
	(
		TSourcePixel spix
	)
	//
	// Returns the offset into the clut for a given bumpmap value based on the intensity.
	//
	//**************************************
	{
		return 0;
	}

	//*****************************************************************************************
	//
	static bool bSetConstColour
	(
		ptr_const<CTexture> ptex,
		int i_ramp = iDefNumRampValues - 1,
		int i_fog = iDefNumDepthValues - 1
	)
	//
	// Sets a constant colour for the triangle.
	//
	//**************************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	static bool bUseConstColour
	(
	)
	//
	// Returns true if a constant colour is required for the surface.
	//
	//**************************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	static uint32 u4GetConstColour
	(
	)
	//
	// Returns constant colour for the triangle.
	//
	//**************************************
	{
		return 0;
	}

};


//*********************************************************************************************
//
class CMapFlat : public CMap<uint8>
//
// Defines a class for rasterizing primitives with bumpmaps using secondary lookup tables.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	static uint32 u4GetClutBaseColour
	(
		ptr_const<CTexture> ptex
	)
	//
	// Returns the colour value for flat shaded polygons.
	//
	//**************************************
	{
		if (ptex->seterfFeatures[erfBUMP])
			return ((CBumpAnglePair&)ptex->tpSolid).u1GetColour();
		else
			return ptex->tpSolid;
	}

	//*****************************************************************************************
	//
	static bool bSetConstColour
	(
		ptr_const<CTexture> ptex,
		int i_ramp = iDefNumRampValues - 1,
		int i_fog = iDefNumDepthValues - 1
	)
	//
	// Sets a constant colour for the triangle.
	//
	//**************************************
	{
		Assert(ptex);

		u4ConstColour = ptex->u4GetConstColour(i_ramp, i_fog);
		return true;
	}

	//*****************************************************************************************
	//
	static uint32 u4GetConstColour
	(
	)
	//
	// Returns constant colour for the triangle.
	//
	//**************************************
	{
		return u4ConstColour;
	}

	//*****************************************************************************************
	//
	static bool bUseConstColour
	(
	)
	//
	// Returns true if a constant colour is required for the surface.
	//
	//**************************************
	{
		return true;
	}

};


//*********************************************************************************************
//
class CMapWire : public CMapFlat
//
// Defines a class for rasterizing primitives without bumpmaps.
//
//**************************************
{
public:
};


//*********************************************************************************************
//
class CMapShadow : public CMap<uint8>
//
// Defines a class for rasterizing primitives as shadows to a special composite map.
//
// Notes:
//		Refer to "TerrainTexture.cpp."
//
//**************************************
{
public:
	typedef char* TDummy;

	//*****************************************************************************************
	//
	static bool bUseConstColour
	(
	)
	//
	// Returns true if a constant colour is required for the surface.
	//
	//**************************************
	{
		return true;
	}
};


//*********************************************************************************************
//
class CMapShadow32 : public CMapShadow
//
// Defines a class for rasterizing primitives as shadows to a special composite map.
//
// Notes:
//		Refer to "TerrainTexture.cpp."
//
//**************************************
{
public:
	typedef float* TDummy2;
};

//*****************************************************************************************
//
inline void Assign
(
	uint16& ru2,
	uint16,
	CMapShadow::TDummy* pmt
)
//
// Specialisation for shadow rendering.  Sets the light value of the composite pixel to a constant.
//
//**************************************
{
	*(((uint8*)(&ru2)) + 1) = u1ShadowIntensity;
}

//*****************************************************************************************
//
inline void Assign
(
	uint16& ru2,
	uint16,
	CMapShadow32::TDummy2* pmt
)
//
// Specialisation for shadow rendering.  Sets the light value of the composite pixel to a constant.
//
//**************************************
{
	*(((uint8*)(&ru2)) + 1) = u1ShadowIntensity;
}


//*********************************************************************************************
//
class CMapAlphaColour : public CMap<uint8>
//
// Defines a class for rasterizing primitives as shadows to a special composite map.
//
// Notes:
//		Refer to "TerrainTexture.cpp."
//
//**************************************
{
public:
	typedef float TDummy;

	//*****************************************************************************************
	//
	static bool bUseConstColour
	(
	)
	//
	// Returns true if a constant colour is required for the surface.
	//
	//**************************************
	{
		return true;
	}
};

//*****************************************************************************************
//
inline void Assign
(
	uint16& ru2,
	uint16,
	CMapAlphaColour::TDummy* pmt
)
//
// Specialisation for applying a constant alpha colour.
//
//**************************************
{
	Assert(au2AlphaTable);

	ru2 &= u2AlphaColourMask;
	ru2 |= u2AlphaColourReference;
	ru2  = au2AlphaTable[ru2];
}


//*********************************************************************************************
//
template<class SPIX> class CMapTexture : public CMap<SPIX>
//
// Defines a class for rasterizing primitives with texture maps.
//
// Notes:
//		Although this class has no unique member variables or unique member functions, this
//		class and it descendants are distinct.
//
//**************************************
{
public:
};


//*********************************************************************************************
//
class CMapBump : public CMapTexture<CBumpAnglePair>
//
// Defines a class for rasterizing primitives with bumpmaps using secondary lookup tables.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CBumpMapOn member functions.
	//

	//*****************************************************************************************
	//
	static uint32 u4GetClutBaseColour
	(
		ptr_const<CTexture> ptex
	)
	//
	// Returns the colour value for flat shaded polygons.
	//
	//**************************************
	{
		// Get the base address for bumpmapping.
		CBumpTable* pbmtbl = ptex->ppcePalClut->pBumpTable;
		Assert(pbmtbl);
		au1SubBumpToIntensity = CBumpTable::bangLight.pu1GetSubBumpTable(pbmtbl->au1BumpToIntensity);
		u4LightTheta          = CBumpTable::bangLight.brIsolateTheta();

		return 0;
	}

	//*****************************************************************************************
	//
	static uint32 u4GetColour
	(
		TSourcePixel spix
	)
	//
	// Returns the colour value referenced by the map.
	//
	//**************************************
	{
		return spix.u1GetColour();
	}

	//*****************************************************************************************
	//
	static uint8 u1GetIntensity
	(
		TSourcePixel spix
	)
	//
	// Returns the intensity value referenced by the "bump."
	//
	//**************************************
	{
		// Get intensity from bumpmap.
		return u1GetIntensityFromBumpmap
		(
			spix,
			u4LightTheta,
			au1SubBumpToIntensity
		);
	}

	//*****************************************************************************************
	//
	static uint32 u4Offset
	(
		TSourcePixel spix
	)
	//
	// Returns the offset into the clut for a given bumpmap value based on the intensity.
	//
	//**************************************
	{
		return uint32(u1GetIntensity(spix)) << iBUMPMAP_COLOURBITS;
	}

};


//*********************************************************************************************
//
class CMapBumpNoTex : public CMapBump
//
// Defines a class for rasterizing primitives with bumpmaps using secondary lookup tables.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CMapBumpNoTex member functions.
	//

	//*****************************************************************************************
	//
	static uint32 u4GetClutBaseColour
	(
		ptr_const<CTexture> ptex
	)
	//
	// Returns the colour value for flat shaded polygons.
	//
	//**************************************
	{
		Assert(ptex->tpSolid);

		// Call base class member function.
		CMapBump::u4GetClutBaseColour(ptex);

		return ((CBumpAnglePair&)ptex->tpSolid).u1GetColour() - 1;
	}

	//*****************************************************************************************
	//
	static uint32 u4GetColour
	(
		TSourcePixel spix
	)
	//
	// Returns the colour value referenced by the map.
	//
	//**************************************
	{
		if (spix.u1GetColour())
			return 1;
		
		return 0;
	}

};


//*********************************************************************************************
//
class CMapBumpTable : public CMapBump
//
// Defines a class for rasterizing primitives with bumpmaps using a secondary table.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CBumpMapTableOn member functions.
	//

	//*****************************************************************************************
	//
	static uint8 u1GetIntensity
	(
		TSourcePixel spix
	)
	//
	// Returns the intensity value referenced by the "bump."
	//
	//**************************************
	{
		// Get intensity from bumpmap.
		uint8 u1_intensity = u1GetIntensityFromBumpmap
		(
			spix,
			u4LightTheta,
			au1SubBumpToIntensity
		);

		// Return the intensity value.
		return au1IntensityTrans[u1_intensity];
	}

	//*****************************************************************************************
	//
	static uint32 u4Offset
	(
		CBumpAnglePair bang
	)
	//
	// Returns the offset into the clut for a given bumpmap value based on the intensity.
	//
	//**************************************
	{
		return uint32(u1GetIntensity(bang)) << iBUMPMAP_COLOURBITS;
	}

	//*****************************************************************************************
	//
	static uint32 u4GetClutBaseColour
	(
		ptr_const<CTexture> ptex
	)
	//
	// Returns the colour value for flat shaded polygons.
	//
	//**************************************
	{
		// Get the base address for bumpmapping.
		CBumpTable* pbmtbl = ptex->ppcePalClut->pBumpTable;
		Assert(pbmtbl);
		au1SubBumpToIntensity = CBumpTable::bangLight.pu1GetSubBumpTable(pbmtbl->au1BumpToIntensity);
		u4LightTheta          = CBumpTable::bangLight.brIsolateTheta();
		au1IntensityTrans     = pbmtbl->pau1IntensityTrans;

		return 0;
	}

};

#if !bFOLD_SPECULAR

//*********************************************************************************************
//
class CMapBumpSpec : public CMapTexture<CBumpAnglePair>
//
// Defines a class for rasterizing primitives with bumpmaps using secondary lookup tables.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CBumpMapOn member functions.
	//

	//*****************************************************************************************
	//
	static uint32 u4GetClutBaseColour
	(
		ptr_const<CTexture> ptex
	)
	//
	// Returns the colour value for flat shaded polygons.
	//
	//**************************************
	{
		// Get the base address for bumpmapping.
		Assert(BumpLookup.au1BumpFirst);
		au1SubBumpToIntensity = BumpLookup.bangLight.pu1GetSubBumpTable(BumpLookup.au1BumpFirst);
		u4LightTheta          = BumpLookup.bangLight.u2IsolateTheta();

		// Specular components.
		au1SubBumpToCosSpec = BumpLookup.bangSpecular.pu1GetSubBumpTable(BumpLookup.au1BumpToCos);
		u4SpecularTheta     = BumpLookup.bangSpecular.u2IsolateTheta();

		return 0;
	}

	//*****************************************************************************************
	//
	static uint32 u4GetColour
	(
		CBumpAnglePair spix
	)
	//
	// Returns the colour value referenced by the map.
	//
	//**************************************
	{
		return spix.u1GetColour();
	}

	//*****************************************************************************************
	//
	static uint8 u1GetIntensity
	(
		TSourcePixel spix
	)
	//
	// Returns the intensity value referenced by the "bump."
	//
	//**************************************
	{
		// Get cos from bumpmap for light.
		uint u_light = u1GetIntensityFromBumpmap
		(
			spix,
			u4LightTheta,
			au1SubBumpToIntensity
		);

		// Get cos from bumpmap for specular.
		uint u_specular = u1GetIntensityFromBumpmap
		(
			spix,
			u4SpecularTheta,
			au1SubBumpToCosSpec
		);

		// Transform here.
		return BumpLookup.aau1CosToSpecularIntensity[u_light][u_specular];
	}

	//*****************************************************************************************
	//
	static uint32 u4Offset
	(
		TSourcePixel spix
	)
	//
	// Returns the offset into the clut for a given bumpmap value based on the intensity.
	//
	//**************************************
	{
		return uint32(u1GetIntensity(spix)) << iBUMPMAP_COLOURBITS;
	}

};

#endif

#endif
