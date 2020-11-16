/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * Notes:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/DrawTriangle.cpp                              $
 * 
 * 67    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 66    98.04.30 5:22p Mmouni
 * Added support for stippled texture.
 * 
 * 65    3/05/98 3:38p Pkeet
 * Added the 'bCAT_PIXEL_STAT' macro switch. Removed unused stats.
 * 
 * 64    1/15/98 7:03p Pkeet
 * Added the polygon lock stat.
 * 
 * 63    98.01.14 9:07p Mmouni
 * Made changes for K6 3D polygon setup optimizations.
 * 
 * 62    97.11.04 10:57p Mmouni
 * Made changes for K6-3D specific CIndexLinear.
 * 
 * 61    97/10/31 9:20 Speter
 * Commented out some less useful stats.
 * 
 * 60    97.10.27 1:27p Mmouni
 * Made changes to support the K6-3D.
 * 
 * 59    97.10.15 7:37p Mmouni
 * Now uses bPOLY_STATS define.
 * 
 * 58    97/10/12 20:35 Speter
 * Removed fClutRampScale, as cvIntensity is now pre-scaled.  Made iNumIntensities a debug-only
 * param.  Replaced au1SubBumpToCosSpec with au1IntensityTrans.
 * 
 * 57    9/29/97 11:10a Mmouni
 * Added some extra globals to support alignment in texture correction spans.
 * 
 * 56    9/15/97 2:01p Mmouni
 * Added u4TextureTileMaskStepU global.
 * 
 * 55    9/01/97 7:58p Rwyatt
 * bClampUV controls clamping at run timw within the perspective primitives
 * 
 * 54    8/29/97 3:05p Pkeet
 * Added stats for transparencies.
 * 
 * 53    8/28/97 4:07p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 53    97/08/22 6:42p Pkeet
 * Employed the 'TBoolAsm' typedef.
 * 
 * 52    8/15/97 8:44p Bbell
 * Added fogging for terrain texturing.
 * 
 * 51    8/15/97 12:47a Rwyatt
 * Added new global for texture tiling mask and moved some floating point constants to here.
 * 
 * 50    97/08/14 6:40p Pkeet
 * Changed the static member of the map type.
 * 
 * 49    97/08/14 15:45 Speter
 * New CMapShadow template, courtesy Raul.
 * 
 * 48    97/08/11 12:24 Speter
 * Commented out DrawPolygon sub-stats.
 * 
 * 47    97/08/05 11:33 Speter
 * Added psDrawPolygonBegin stat.
 * 
 * 46    97/07/23 18:01 Speter
 * Added psDrawPolygonSolid stat.
 * 
 * 45    97/07/16 15:59 Speter
 * Added iNumRasterVertices global, and moved u4ClutIndex global here.  Added additional
 * DrawPolygon sub-stats.  
 * 
 * 44    97/07/08 20:23 Speter
 * Moved bDoubleVertical and bHalfScanlines to CScreenRender::SSettings.
 * Renamed KPixels stat to Pixels.
 * 
 * 43    97/07/07 14:02 Speter
 * Now copy vertices for current polygon locally to arvRasterVertices.
 * 
 * 42    97/06/27 15:24 Speter
 * Updated primitive stats (disabled) for trapezoids.  Removed CalculateTrapStats().  Added
 * fD[U|V]InvEdge globals (needed for multiple base edges per polygon).
 * 
 * 41    97/06/26 11:27a Pkeet
 * Removed unnecessary globals.
 * 
 * 40    97/06/24 1:59p Pkeet
 * Added adaptive subdivision.
 * 
 * 39    97/06/23 20:31 Speter
 * Made gcfScreen a static member of CScreenRender.
 * 
 * 38    97/06/15 17:27 Speter
 * Put trapezoid stat code inside bPRIM_STATS compile flag.
 * 
 * 37    97/06/14 3:37p Pkeet
 * Removed the filter variable and the scanline pointer.
 * 
 * 36    97/06/14 14:51 Speter
 * Updated stat constructors.
 * 
 * 35    6/15/97 1:43a Bbell
 * Fixed final mode build problem.
 * 
 * 34    97/06/13 7:31p Pkeet
 * Added CDrawPolygonBase and a global raster structure.
 * 
 * 33    97/06/12 5:56p Pkeet
 * Added capability to render only every other scanline and to double the number of scanlines.
 * 
 * 32    97/06/12 15:29 Speter
 * CalculateTrapStats is now iffed out.
 * 
 * 31    97/06/10 21:56 Speter
 * Fixed PixelsC++ stat.
 * 
 * 30    97/06/10 15:39 Speter
 * Updated stats.
 * 
 * 29    97/06/05 17:03 Speter
 * Updated for stats.
 * 
 * 28    97/06/03 18:50 Speter
 * Moved polygon stats here from Profile.cpp, added perspective stats.  Commented out polygon
 * element stats.
 * 
 * 27    97/06/02 13:52 Speter
 * Added primitive element stats for both triangles and trapezoids.
 * 
 * 26    5/06/97 5:06p Cwalla
 * Added pScaneLinPtr, renamed filter, made some functions global.
 * 
 * 25    97-04-23 14:29 Speter
 * Moved bangLight and bangSpecular variables to CBumpLookup.
 * 
 * 24    4/13/97 7:04p Cwalla
 * Moved u4ConstColor to global.
 * 
 * 23    97-03-31 22:18 Speter
 * Moved filter variable here, so it's defined only once.
 * 
 * 22    97-03-28 16:26 Speter
 * Moved CZBuffer static variables to ZBufferT.hpp (now that they're templates).
 * 
 * 21    97/02/10 5:23p Pkeet
 * Added support for the reduced Z buffer clear algorithm.
 * 
 * 20    97/02/07 3:21p Pkeet
 * Added the 'u4ConstColour' static variable.
 * 
 * 19    97/01/20 11:51 Speter
 * Moved gcfScreen from CScreenRender to a global var for this rasteriser.
 * 
 * 18    97/01/07 12:03 Speter
 * Changed pvTextureBitmap to const.
 * 
 * 17    1/03/97 4:41p Pkeet
 * Added the 'iSubdivideLen' and 'fInvSubdivideLen' globals.
 * 
 * 16    12/12/96 11:56a Pkeet
 * Added more globals for perspective correction.
 * 
 * 15    12/11/96 2:21p Pkeet
 * Added more globals for perspective correction.
 * 
 * 14    12/10/96 10:39a Pkeet
 * Added more globals for perspective correction.
 * 
 * 13    12/09/96 4:11p Pkeet
 * Added more globals for perspective correction.
 * 
 * 12    12/09/96 3:49p Pkeet
 * Added global values for perspective-correction.
 * 
 * 11    12/06/96 3:50p Pkeet
 * Added the bModified global flag.
 * 
 * 10    12/06/96 2:28p Pkeet
 * Added pointers to the Zbuffer and the screen.
 * 
 * 9     96/12/05 14:32 Speter
 * Added bREFLECTION_PRIMARY flag to control method of reflection bump-mapping.  Moved
 * au1BumpFirst pointer into CBumpLookup, set automatically.
 * 
 * 8     96/12/04 11:46 Speter
 * Added au1BumpFirst table for bumpmapping.
 * 
 * 7     96/11/25 13:41 Speter
 * Changed Light variable to bangLight, added bangSpecular.
 * 
 * 6     11/22/96 12:44p Pkeet
 * Added the CMapBumpSpec class and globals.
 * 
 * 5     10/18/96 10:25a Pkeet
 * Removed unnecessary includes. Reordered global variables according to those needed by the
 * scanline loop and then those needed generically by triangles.
 * 
 * 4     10/14/96 11:49a Pkeet
 * Added externally declared global variables from the 'GouraudT' module.
 * 
 * 3     10/11/96 12:26p Pkeet
 * Made the pointer to the texture bitmap a global variable.
 * 
 * 2     10/11/96 10:31a Pkeet
 * Added 'pvClutConversion' as a global variable instead of a member of 'CDrawTriangle.' No clut
 * pointer is therefore passed to the 'DrawLoop' functions.
 * 
 * 1     10/10/96 7:20p Pkeet
 * Initial implementation. Moved variables previously declared as globals here.
 * 
 **********************************************************************************************/


//
// Includes.
//
#include "common.hpp"
#include "DrawTriangle.hpp"
#include "DrawSubTriangle.hpp"


//
// Global variables called often by the rasterizer.
//

//
// Alpha colour variables.
//
uint16  u2AlphaColourMask      = 0;
uint16  u2AlphaColourReference = 0;
uint16* au2AlphaTable          = 0;

uint16* pu2TerrainTextureFogClut = 0;
CPalClut* ppceTerrainClut = 0;

// Copy of vertices for current polygon, with texture and intensity values scaled
// to final ranges.
SRenderVertex arvRasterVertices[iMAX_RASTER_VERTICES];
int iNumRasterVertices;

// Scanline direction value is +1 if the scanline moves left to right, otherwise it is -1.
int32 i4ScanlineDirection;

// Pointer to a clut location for source to destination pixel conversions.
void* pvClutConversion;

// Increment for InvZ for each pixel in the Z direction.
int32 i4DInvZ;

// Fixed point representation of intensity step values with respect to x.
fixed fxDeltaXIntensity;


#if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)

// 2D walk version of the u and v step values over the horizontal axis.
PackedFixedUV pfxDeltaTex;

#endif

// 2D walk version of the u and v step values over the horizontal axis.
CWalk2D w2dTex;
CWalk2D w2dDeltaTex;

void* pvZBuffer;

// Theta constant of current light relative to the polygon's texturemap.
uint32 u4LightTheta;
uint32 u4SpecularTheta;

// For clut lookup values.
uint32 u4ClutIndex;

// Pointer to the location in the bumpmap to intensity lookup table based on the light's phi.
uint8* au1SubBumpToIntensity;

// Pointer to the intermediate translation table between bump-intensity and clut.
uint8* au1IntensityTrans;

// Intensity of shadow.
uint8 u1ShadowIntensity = 0;

bool bModified;

//
// Variables used in perspective correction using scanline subdivision.
//

#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

TexVals tvDeltas;			// U,V,Z slopes.
TexVals tvDEdge;			// U,V,Z slopes * subdivision length.
TexVals tvNegAdj;			// U,V,Z for subpixel correction.

ClampVals cvMaxCoords;

#else

float fDUInvZ;				// U,V,Z slopes.
float fDVInvZ;
float fDInvZ;

float fDUInvZEdge;			// U,V,Z slopes * subdivision length.
float fDVInvZEdge;
float fDInvZEdge;

float fNegUInvZ;			// U,V,Z for subpixel correction.
float fNegVInvZ;
float fNegInvZ;

float fTexWidth;
float fTexHeight;

#endif


float fDUInvZEdgeMinusOne;	// U,V,Z slopes * (subdivision length - 1).
float fDVInvZEdgeMinusOne;
float fDInvZEdgeMinusOne;

float fDUInvZScanline;		// U,V,Z slopes * length of current subdivision.
float fDVInvZScanline;
float fDInvZScanline;

float fGUInvZ;
float fGVInvZ;
float fGInvZ;

float fU, fNextU;
float fV, fNextV;
const float fFixed16Scale	= 65536.0f;
const float fOne			= 1.0f;

int   iNextSubdivide;
int   iSubdivideLen       = iDEFAULT_SUBDIVIDELEN;
float fInvSubdivideLen    = 1.0f / float(iDEFAULT_SUBDIVIDELEN);
int   iCacheNextSubdivide;

int   iTexWidth;

CPerspectiveSettings persetSettings;

SGlobalRas gsGlobals;
uint32 u4ConstColour;

//
// Global variables.
//


// The mask to AND texture offsets and force them to tile.
// this should be set to (width-1) | (height<<9-1). Texture
// sizes must be a power of 2 and packed on a 512 byte stride,
// hence the shift by 9.
// the default value of this mask will leave the offset unaltered
uint32 u4TextureTileMask = 0xffffffff;


// The mask to AND the U integer step with to ensure that negative
// U slopes won't result in the texture shifting each time it is tiled.
// For non-tiled texture this must be set to 0xffffffff.
uint32 u4TextureTileMaskStepU = 0xffffffff;

//
// Combined U,V mask for MMX/3DX in the form (V,U,V,U).
//
uint64 qUVMasks = 0xffffffffffffffff;


// this clamp variable controls if the UV co-ords of the current polygon
// are clamped to the size of the source raster.
//
// If a raster is packed, it can be tiled. Therefore the clamp is set
// to false and the tile mask is set according to the width of the texture.
// 
// If a raster is not packed, either because it is bigger than 256x256 or
// it is a unique curved bump map, then it cannot be tiled. In this case
// the UVs need to be clamped so the perspective primitives do not walk
// outside of the texture. For non-tiled rasters the tile mask is set to
// 0xffffffff so it will have no effect.
int bClampUV = 0;


// Floating point representation of inverse z step values with respect to x.
float fDeltaZ;	

// Value for modulus subtract.
int32 i4ZModDiv;

// Floating point version of the u and v step values over the horizontal axis.
float fDU, fDV;


#if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)

// Negative versions of the u and v step values over the horizontal axis.
PackedFloatUV pfNegD;

#else

// The seperate version of the u and v step values over the horizontal axis.
UBigFixed bfDeltaU;
CWalk1D   w1dDeltaV;

// Negative versions of the u and v step values over the horizontal axis.
UBigFixed bfNegDU;
CWalk1D   w1dNegDV;

#endif


#if VER_DEBUG
	int iTexSize;	// Variable used in debugging only.
#endif

// Pointer to the base address of a bitmap associated with the currently rendered texture.
const void* pvTextureBitmap;

// Floating point representation of intensity step values with respect to x.
float fDeltaXIntensity;

// Value for modulus subtract.
fixed fxIModDiv;

#if VER_DEBUG
	int iNumIntensities;
#endif

// Base intensity value for the triangle.
int iBaseIntensity;

// Default fogging value.
int iDefaultFog;

TBoolAsm bEvenScanlinesOnly = 0;

//
// Counts of pixels iterated and pixels rendered. Used for finding the amount of tranparency
// rendered in proportion to the number of solid pixels rendered.
//
int iNumPixelsIterated = 0;
int iNumPixelsSolid = 0;

CProfileStat		psDrawPolygonLock			("Lock", &proProfile.psDrawPolygon);

#if bPOLY_STATS

CProfileStat		psDrawPolygonInit			("Setup", &proProfile.psDrawPolygon);
CProfileStat		psDrawPolygonBump			("Bump Setup", &proProfile.psDrawPolygon);
CProfileStat		psDrawPolygonBegin			("Begin", &proProfile.psDrawPolygon);
CProfileStat		psDrawPolygonData			("Data", &proProfile.psDrawPolygon);
CProfileStat		psDrawPolygonEdges			("Edges", &proProfile.psDrawPolygon);
CProfileStat		psDrawPolygonWalk			("Walk", &proProfile.psDrawPolygon);

#else

// Disable.
CProfileStat		psDrawPolygonInit;
CProfileStat		psDrawPolygonBump;

#endif

CProfileStatParent	psPixels					("Pixels", &proProfile.psDrawPolygon, Set(epfSEPARATE));
#if bCAT_PIXEL_STAT
CProfileStat		psPixelsPerspective			("Perspective", &psPixels, Set(epfRELATIVE));
#endif
#if bPRIM_STATS

CProfileStat psSubpolygons("Subpolygons", &proProfile.psDrawPolygon);
CProfileStat psTrapezoids("Trapezoids", &proProfile.psDrawPolygon);
CProfileStat psBaseEdges("Base Edges", &proProfile.psDrawPolygon);
CProfileStat psEdges("Edges", &proProfile.psDrawPolygon);
CProfileStat psLines("Lines", &proProfile.psDrawPolygon);

#endif

