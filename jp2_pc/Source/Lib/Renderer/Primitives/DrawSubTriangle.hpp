/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Defines a subtriangle drawing function.
 *
 * Bugs:
 *
 * Notes:
 *
 * To do:
 *		Produce specific assembly versions.
 *		Fix 'bModified' bug.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/DrawSubTriangle.hpp                           $
 * 
 * 55    98.07.17 6:36p Mmouni
 * Added  new alpha texture primitive.
 * 
 * 54    98.06.18 3:57p Mmouni
 * Added stippled flat shaded primitive.
 * 
 * 53    5/12/98 3:04p Pkeet
 * Added hack to fix crash bug.
 * 
 * 52    98.04.30 5:22p Mmouni
 * Added support for stippled texture.
 * 
 * 51    98.04.08 8:42p Mmouni
 * Added support for fogged dithered flat shaded primitive.
 * 
 * 50    3/05/98 3:38p Pkeet
 * Added the 'bCAT_PIXEL_STAT' macro switch.
 * 
 * 49    98.01.22 3:15p Mmouni
 * Changed bump-map depth conditional compilation.
 * 
 * 48    1/19/98 7:34p Pkeet
 * Added support for 16 bit bumpmaps.
 * 
 * 47    98/01/02 19:06 Speter
 * Avoided Assert by changing MAP param in TShadeTerrain.
 * 
 * 46    97.12.22 4:31p Mmouni
 * Changed prototype of ShadeTerrain to be correct.
 * 
 * 45    97.12.22 2:53p Mmouni
 * Added new terrain gouraud shading primitive.
 * 
 * 44    97.11.24 6:51p Mmouni
 * Changed CGouraudOff to CGouraudNone in TAlphaColor to avoid setting up values that are not
 * used from data that is not initialized.
 * 
 * 43    97/11/19 18:10 Speter
 * Shadow primitives use CGouraudNone rather than CGouraudOff.
 * 
 * 42    97.11.14 11:55p Mmouni
 * Added scanline types and function prototypes for filter copy primitive.
 * 
 * 41    97/11/10 11:26a Pkeet
 * Added water primitive types.
 * 
 * 40    97/11/08 3:40p Pkeet
 * Removed Z template parameters. Removed z buffer code from the C++ version of
 * 'DrawSubTriangle.'
 * 
 * 39    97/11/06 4:49p Pkeet
 * Added gouraud fog types.
 * 
 * 38    97.11.05 7:50p Mmouni
 * Added u4TerrainFogMask.
 * 
 * 37    97/11/05 2:49p Pkeet
 * Added 8 band alpha fog for terrain.
 * 
 * 36    97/11/03 5:52p Pkeet
 * Changed the terrain template types to use the 'CColLookupTerrain' class.
 * 
 * 35    9/30/97 9:29p Mmouni
 * Added seperate 16-bit clut routines for terrain.
 * 
 * 34    8/29/97 3:05p Pkeet
 * Added stats for transparencies.
 * 
 * 33    97/08/22 6:42p Pkeet
 * Employed the 'TBoolAsm' typedef.
 * 
 * 32    8/19/97 12:12a Rwyatt
 * Added 32 bit shadow primitive
 * 
 * 31    8/18/97 5:47p Bbell
 * Modified template parameters to match changes to MapT.hpp.
 * 
 * 30    8/15/97 8:43p Bbell
 * Added fogging for terrain texturing.
 * 
 * 29    97/08/14 9:49p Pkeet
 * Added the 'TAlphaColour,' 'TShadow' and 'TShadowTrans8' primitives.
 * 
 * 28    97/08/14 6:40p Pkeet
 * Debugged the 'Assign' function.
 * 
 * 27    97/08/14 15:45 Speter
 * New CMapShadow template, courtesy Raul.
 * 
 * 26    97/08/12 6:57p Pkeet
 * Added special primitives for terrain texturing.
 * 
 * 25    97/08/07 11:35a Pkeet
 * Added interface support for mipmapping.
 * 
 * 24    97/07/18 3:28p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 23    97/07/16 16:00 Speter
 * Reinstated VER_ASM switch for primitives.
 * 
 * 22    97/06/27 15:24 Speter
 * Changed CGouraudOff to new CGouraudNone where appropriate.
 * 
 * 21    97/06/26 11:27a Pkeet
 * Eliminated the DrawLoop template function.
 * 
 * 20    97/06/25 12:45p Pkeet
 * Support for a wireframe mode.
 * 
 * 19    97/06/14 7:00p Pkeet
 * Added template specializations.
 * 
 * 18    97/06/14 3:38p Pkeet
 * Removed the filter variable and the scanline pointer.
 * 
 * 17    97/06/12 5:56p Pkeet
 * Added capability to render only every other scanline and to double the number of scanlines.
 * 
 * 16    97/06/12 2:15p Pkeet
 * Added new primitives.
 * 
 * 15    97/06/11 4:58p Pkeet
 * Added opaque versions of the optimized copy primitives.
 * 
 * 14    97/06/11 4:07p Pkeet
 * Added texture mapping primitives that use a clut but do not shade.
 * 
 * 13    97/06/10 6:08p Pkeet
 * Added prototypes for fast 8 and 16 bit flat shaded primitives.
 * 
 * 12    97/06/10 1:20p Pkeet
 * Added initial assembly optimizations for bumpmap linear routines.
 * 
 * 11    97/06/08 4:16p Pkeet
 * Added perspective correction bump map primitives.
 * 
 * 10    97/06/06 8:51p Pkeet
 * Added linear.
 * 
 * 9     97/06/06 4:35p Pkeet
 * Should work now!
 * 
 * 8     97/06/04 6:46p Pkeet
 * Initial specialized version of DrawSubTriangle.
 * 
 * 7     97/05/25 18:05 Speter
 * Changed CDrawTriangle to CDrawPolygon.
 * 
 * 6     5/06/97 5:06p Cwalla
 * Added scanlineptr.
 * 
 * 5     4/30/97 6:59p Cwalla
 * Inlined fixed point calls.
 * 
 * 4     97-03-31 22:17 Speter
 * Moved timing here from multiple calls in CDrawTriangle.
 * Moved setting of pvTextureBitmap variable into INDEX::InitializeTriangleData().
 * Changed pdtriTriangle parameter to pdtri
 * 
 * 3     97/02/24 7:34p Pkeet
 * Fixed bug that prevented self-modifying code from being used in release builds.
 * 
 * 2     97/02/21 12:47p Pkeet
 * Changed type names for template parameters to meet coding standards.
 * 
 * 1     97/02/20 7:19p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_DRAWSUBTRIANGLE_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_DRAWSUBTRIANGLE_HPP

#include "DrawTriangle.hpp"

//
// Externally defined variables.
//

// Scanline direction value is +1 if the scanline moves left to right, otherwise it is -1.
extern int32 i4ScanlineDirection;

// Flag to render every other scanline only.
extern TBoolAsm bEvenScanlinesOnly;

// Terrain texturing hack value.
extern uint16*   pu2TerrainTextureFogClut;
extern CPalClut* ppceTerrainClut;

// Transparent and solid pixels counts.
extern int iNumPixelsIterated;
extern int iNumPixelsSolid;

// Mask for terrain fog alpha.
extern uint16 u2TerrainFogMask;
extern uint32 u4TerrainFogMask;


//
// Macros.
//

// Flag set to 'true' if the scanline moves from right to left.
#define bRIGHT_TO_LEFT (i4ScanlineDirection < 0)
#define bCOUNT_PIXELS_WRITTEN (1)

//
// Specialized DrawSubtriangle functions for assembly.
//
#if VER_ASM

// Types.

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapTexture<uint16>, CIndexPerspective, CColLookupOff> TCopyPersp;

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapTexture<uint16>, CIndexLinear, CColLookupOff>      TCopyLinear;

typedef CScanline<uint16, CGouraudNone, CTransparencyOn,
			      CMapTexture<uint16>, CIndexPerspective, CColLookupOff> TCopyPerspTrans;

typedef CScanline<uint16, CGouraudNone, CTransparencyOn,
			      CMapTexture<uint16>, CIndexLinear, CColLookupOff>      TCopyLinearTrans;

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapTexture<uint16>, CIndexPerspective, CColLookupTerrain> TCopyTerrainPersp;

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapTexture<uint16>, CIndexLinear, CColLookupTerrain>  TCopyTerrainLinear;

typedef CScanline<uint16, CGouraudFog, CTransparencyOff,
			      CMapTexture<uint16>, CIndexPerspective, CColLookupOff> TGFogPersp;

typedef CScanline<uint16, CGouraudFog, CTransparencyOff,
			      CMapTexture<uint16>, CIndexLinear, CColLookupOff>      TGFogLinear;

typedef CScanline<uint16, CGouraudFog, CTransparencyOff,
			      CMapFlat, CIndexNone, CColLookupOff>					TFlatGourFog;

#if (BILINEAR_FILTER)

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapTexture<uint16>, CIndexPerspectiveFilter, CColLookupOff>	TCopyPerspFilter;

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapTexture<uint16>, CIndexLinearFilter, CColLookupOff>		TCopyLinearFilter;

#endif

typedef CScanline<uint16, CGouraudOff, CTransparencyStipple, CMapTexture<uint16>,
                  CIndexPerspective, CColLookupOff>						TStippleTexPersp;

typedef CScanline<uint16, CGouraudOff, CTransparencyStipple, CMapTexture<uint16>,
                  CIndexLinear, CColLookupOff>							TStippleTexLinear;

typedef CScanline<uint16, CGouraudOff, CTransparencyOff, CMapTexture<uint16>,
                  CIndexPerspective, CColLookupAlphaTexture>			TAlphaTexPersp;

typedef CScanline<uint16, CGouraudOff, CTransparencyOff, CMapTexture<uint16>,
                  CIndexLinear, CColLookupAlphaTexture>					TAlphaTexLinear;

typedef CScanline<uint16, CGouraudOff, CTransparencyOff, CMapTexture<uint16>,
                  CIndexPerspective, CColLookupAlphaWater>               TWaterPersp;

typedef CScanline<uint16, CGouraudOff, CTransparencyOff, CMapTexture<uint16>,
                  CIndexLinear, CColLookupAlphaWater>                    TWaterLinear;

typedef CScanline<uint16, CGouraudNone, CTransparencyOn,
			      CMapBump, CIndexPerspective, CColLookupOn>             TBumpPerspTrans;

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapBump, CIndexPerspective, CColLookupOn>             TBumpPersp;

typedef CScanline<uint16, CGouraudNone, CTransparencyOn,
			      CMapBump, CIndexLinear, CColLookupOn>                  TBumpLinearTrans;

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapBump, CIndexLinear, CColLookupOn>                  TBumpLinear;

#if iBUMPMAP_RESOLUTION == 32

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapBumpTable, CIndexPerspective, CColLookupOn>        TBumpPerspTbl;

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapBumpTable, CIndexLinear, CColLookupOn>             TBumpLinearTbl;

#endif // iBUMPMAP_RESOLUTION == 32

typedef CScanline<uint16, CGouraudOff, CTransparencyOff,
			      CMapFlat, CIndexNone, CColLookupOff>                   TFlat;

typedef CScanline<uint16, CGouraudOff, CTransparencyStipple,
			      CMapFlat, CIndexNone, CColLookupOff>                   TFlatStipple;

typedef CScanline<uint8, CGouraudOff, CTransparencyOff,
			      CMapFlat, CIndexNone, CColLookupOff>                   TFlat8;

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapWire, CIndexNone, CColLookupOff>                   TWire;

typedef CScanline<uint16, CGouraudOff, CTransparencyOn,
			      CMapTexture<uint8>, CIndexPerspective, CColLookupOn>   TTexturePerspTrans;

typedef CScanline<uint16, CGouraudOff, CTransparencyOff,
			      CMapTexture<uint8>, CIndexPerspective, CColLookupOn>   TTexturePersp;

typedef CScanline<uint16, CGouraudOff, CTransparencyOn,
			      CMapTexture<uint8>, CIndexLinear, CColLookupOn>        TTextureLinearTrans;

typedef CScanline<uint16, CGouraudOff, CTransparencyOff,
			      CMapTexture<uint8>, CIndexLinear, CColLookupOn>        TTextureLinear;

typedef CScanline<uint16, CGouraudOn, CTransparencyOn,
			      CMapTexture<uint8>, CIndexPerspective, CColLookupOn>   TTexturePerspTransGour;

typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMapTexture<uint8>, CIndexPerspective, CColLookupOn>   TTexturePerspGour;

typedef CScanline<uint16, CGouraudOn, CTransparencyOn,
			      CMapTexture<uint8>, CIndexLinear, CColLookupOn>        TTextureLinearTransGour;

typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMapTexture<uint8>, CIndexLinear, CColLookupOn>        TTextureLinearGour;

typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMapFlat, CIndexNone, CColLookupOn>                    TGour;

//
// Special primitives for terrain texturing.
//

typedef CScanline<uint16, CGouraudNone, CTransparencyOn,
			      CMapTexture<uint8>, CIndexLinear, CColLookupOff>       TTexNoClutLinearTrans;

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapTexture<uint8>, CIndexLinear, CColLookupOff>       TTexNoClutLinear;

typedef CScanline<uint16, CGouraudNone, CTransparencyOn,
			      CMapShadow, CIndexLinear, CColLookupOff>               TShadowTrans8;

typedef CScanline<uint16, CGouraudNone, CTransparencyOn,
			      CMapShadow32, CIndexLinear, CColLookupOff>             TShadowTrans32;

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapShadow, CIndexNone, CColLookupOff>                 TShadow;

typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMap<uint8>, CIndexNone, CColLookupOff>				 TShadeTerrain;


//
// Special primitives for alpha blending.
//

typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapAlphaColour, CIndexNone, CColLookupOff>            TAlphaColour;


//
// Prototypes for assembly versions of subtriangle functions.
//

//*********************************************************************************************
void DrawSubtriangle
(
	TCopyPersp*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TCopyPersp>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TCopyLinear*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TCopyLinear>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TGFogPersp*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TGFogPersp>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TGFogLinear*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TGFogLinear>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TFlatGourFog*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TFlatGourFog>* pdtri	// CDrawPolygon object.
);

#if (BILINEAR_FILTER)

//*********************************************************************************************
void DrawSubtriangle
(
	TCopyPerspFilter*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TCopyPerspFilter>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TCopyLinearFilter*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TCopyLinearFilter>* pdtri	// CDrawPolygon object.
);

#endif

//*********************************************************************************************
void DrawSubtriangle
(
	TStippleTexPersp*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TStippleTexPersp>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TStippleTexLinear*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TStippleTexLinear>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TAlphaTexPersp*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TAlphaTexPersp>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TAlphaTexLinear*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TAlphaTexLinear>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TWaterPersp*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TWaterPersp>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TWaterLinear*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TWaterLinear>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TCopyPerspTrans*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TCopyPerspTrans>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TCopyLinearTrans*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TCopyLinearTrans>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TCopyTerrainPersp*					pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TCopyTerrainPersp>*	pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TCopyTerrainLinear*					pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TCopyTerrainLinear>*	pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TBumpPerspTrans*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TBumpPerspTrans>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TBumpPersp*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TBumpPersp>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TBumpLinearTrans*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TBumpLinearTrans>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TBumpLinear*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TBumpLinear>* pdtri	// CDrawPolygon object.
);

#if iBUMPMAP_RESOLUTION == 32

//*********************************************************************************************
void DrawSubtriangle
(
	TBumpPerspTbl*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TBumpPerspTbl>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TBumpLinearTbl*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TBumpLinearTbl>* pdtri		// CDrawPolygon object.
);

#endif // iBUMPMAP_RESOLUTION == 32

//*********************************************************************************************
void DrawSubtriangle
(
	TFlat*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TFlat>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TFlatStipple*				pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TFlatStipple>*	pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TWire*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TWire>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TFlat8*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TFlat8>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TTexturePerspTrans*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TTexturePerspTrans>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TTexturePersp*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TTexturePersp>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TTextureLinearTrans*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TTextureLinearTrans>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TTextureLinear*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TTextureLinear>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TTexturePerspTransGour*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TTexturePerspTransGour>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TTexturePerspGour*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TTexturePerspGour>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TTextureLinearTransGour*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TTextureLinearTransGour>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TTextureLinearGour*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TTextureLinearGour>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TGour*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TGour>* pdtri	// CDrawPolygon object.
);

//
// Prototypes for terrain.
//

//*********************************************************************************************
void DrawSubtriangle
(
	TTexNoClutLinearTrans*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TTexNoClutLinearTrans>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TTexNoClutLinear*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TTexNoClutLinear>* pdtri	// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TShadowTrans8*               pscan,		// Pointer to the CScanline base object.
	CDrawPolygon<TShadowTrans8>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TShadowTrans32*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<TShadowTrans32>* pdtri		// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TShadow*               pscan,			// Pointer to the CScanline base object.
	CDrawPolygon<TShadow>* pdtri			// CDrawPolygon object.
);

//*********************************************************************************************
void DrawSubtriangle
(
	TShadeTerrain*				 pscan,		// Pointer to the CScanline base object.
	CDrawPolygon<TShadeTerrain>* pdtri		// CDrawPolygon object.
);

//
// Prototypes for alpha blending.
//

//*********************************************************************************************
void DrawSubtriangle
(
	TAlphaColour*               pscan,		// Pointer to the CScanline base object.
	CDrawPolygon<TAlphaColour>* pdtri		// CDrawPolygon object.
);

#endif // VER_ASM


//
// Generalized template function.
//

extern bool bNoCPPSubtriangle;

//*********************************************************************************************
//
template
<
	class LINE	// CScanline template type.
>
void DrawSubtriangle
(
	LINE*               pscan,	// Pointer to the CScanline base object.
	CDrawPolygon<LINE>* pdtri	// CDrawPolygon object.
)
//
// Draws a subtriangle by drawing scanlines of pixels.
//
//**************************************
{
	if (bNoCPPSubtriangle)
		return;

	typedef LINE::TPixel TDest;

	Assert(pscan);
	Assert(pdtri);

	CCycleTimer ctmr;	// Timer object.

	int i_x_from;
	int i_x_to;
	int i_screen_index;
	int i_pixel;
	uint32 u4_colour;
	LINE::TMap::TSourcePixel spix = 0;
	TDest pix_screen = (TDest)LINE::TMap::u4GetConstColour();

	//
	// Iterate through the scanlines that intersect the subtriangle.
	//
	do
	{
		if (bRIGHT_TO_LEFT)
		{
			i_x_from = (pscan->fxX.i4Fx - pdtri->fxLineLength.i4Fx) >> 16;
			i_x_to   = pscan->fxX.i4Fx >> 16;
			i_screen_index = pdtri->iLineStartIndex + i_x_from - 1;
			i_pixel = i_x_to - i_x_from;
		}
		else
		{
			i_x_from = pscan->fxX.i4Fx >> 16;
			i_x_to   = (pscan->fxX.i4Fx + pdtri->fxLineLength.i4Fx) >> 16;
			i_screen_index = pdtri->iLineStartIndex + i_x_to;
			i_pixel = i_x_from - i_x_to;
		}

		// Draw if there are pixels to draw.
		if (i_x_to > i_x_from && !(bEvenScanlinesOnly && ((pdtri->iY & 1) == 0)))
		{
			// Debug range checking.
			Assert(i_x_from >= 0);
			Assert(i_x_to >= 0);
			Assert(i_x_from <= pdtri->prasScreen->iWidth);
			Assert(i_x_to <= pdtri->prasScreen->iWidth);
			pscan->indCoord.AssertRange(pdtri->ptexTexture, pdtri->prpolyPoly->iMipLevel);

			//
			// Call the scanline function.
			//
			// Note: this call should be replace with explicit code.
			//
			{
				LINE::TGouraud gour(pscan->gourIntensity);
				LINE::TIndex   index(pscan->indCoord, i_pixel);
				TDest*  apix_screen = ((TDest*)pdtri->prasScreen->pSurface) + i_screen_index;

				//
				// Iterate left or right along the destination scanline depending on the sign of
				// 'i4ScanlineDirection.'
				//
				for (;;)
				{
					// Do next subdivision.
					int i_pixel_inner = index.i4StartSubdivision(i_pixel);

					TDest* apix_screen_inner = apix_screen + i_pixel;

					for (;;)
					{
						#if bCOUNT_PIXELS_WRITTEN
							++iNumPixelsIterated;
						#endif // bCOUNT_PIXELS_WRITTEN
						
						// Get an indexed pixel.
						if (index.bIsIndexed())
							spix = ((LINE::TMap::TSourcePixel*)pvTextureBitmap)[index.iGetIndex()];

						++index;

						// Fast mode drawing functions.
						if (LINE::TMap::bUseConstColour())
						{
							// If the pixel is unindexed, we are writing a solid colour.
							// Otherwise, check transparency.
							if (!index.bIsIndexed() || LINE::TMap::u4GetColour(spix))
							{
								// Write the pixel.
								Assign(apix_screen_inner[i_pixel_inner], pix_screen, (LINE::TMap::TDummy*)0);
								#if bCOUNT_PIXELS_WRITTEN
									++iNumPixelsSolid;
								#endif // bCOUNT_PIXELS_WRITTEN
							}
						}
						else
						{
							// Get the colour value from the pixel.
							u4_colour = LINE::TMap::u4GetColour(spix);

							if (LINE::TTrans::bIsOpaquePixel(u4_colour))
							{
								// Copy the texture value to the destination raster.
								if (LINE::TClu::bUseClut())
								{
									// Use a clut.
									pix_screen = ((TDest*)pvClutConversion)
									[
										u4_colour +
										LINE::TMap::u4Offset(spix) +
										gour.u4Offset()
									];
									Assign(apix_screen_inner[i_pixel_inner], pix_screen, (LINE::TMap::TDummy*)0);
									#if bCOUNT_PIXELS_WRITTEN
										++iNumPixelsSolid;
									#endif // bCOUNT_PIXELS_WRITTEN
								}
								else
								{
									// Just copy the pixel value.
									Assign(apix_screen_inner[i_pixel_inner], u4_colour, (LINE::TMap::TDummy*)0);
									#if bCOUNT_PIXELS_WRITTEN
										++iNumPixelsSolid;
									#endif // bCOUNT_PIXELS_WRITTEN

								}
							}
						}

						// Increment values.
						i_pixel_inner += i4ScanlineDirection;

						// Break from subdivision loop if there are no more pixels to render.
						if (i_pixel_inner == 0)
							break;

						// Increment values.
						++gour;
					}

					// Exit scanline routine if there are no more pixels to render.
					if (i_pixel == 0)
						break;

					// Increment values.
					++gour;

					// Implement perspective correction.
					index.UpdatePerspective(0);
				}
			}
		}
		
		// Increment the base edge.
		++*pdtri->pedgeBase;

		// Set the new length of the line to be rasterized.
		pdtri->fxLineLength.i4Fx += pdtri->fxDeltaLineLength.i4Fx;

		// Get new starting pixel TIndex for the scanline.
		pdtri->iLineStartIndex += pdtri->prasScreen->iLinePixels;
	}
	while (++pdtri->iY < pdtri->iYTo);

	// Count timer stats.
#if bCAT_PIXEL_STAT
	psPixelsC.Add(ctmr(), 0);
#endif
}


#endif
