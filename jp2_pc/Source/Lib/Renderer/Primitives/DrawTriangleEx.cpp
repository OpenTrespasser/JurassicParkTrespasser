/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Functions that impelment parts of DrawTriangle in assembly.
 *
 * Bugs:
 *
 * Notes:
 *		Some of these may be faster inlined, but in general it's not worthwhile.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/DrawTriangleEx.cpp                            $
 * 
 * 30    10/01/98 12:37a Asouth
 * Added config to ensure proper configuration
 * 
 * 29    9/03/98 1:52p Asouth
 * removed 'short' from a couple of long jumps
 * 
 * 28    98.08.26 6:55p Mmouni
 * Removed gouraud gradient clamping code for degenerate edges becuase polygons with degenerate
 * edges are now drawn by triangulation.
 * 
 * 27    98.08.18 11:37p Mmouni
 * Added special case code to prevent gouraud primitive from walking outside texture.
 * 
 * 26    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 25    98.07.21 8:22p Mmouni
 * Added 3DX routine to get the perspective subdivision without using an x86 code.
 * 
 * 24    98.07.17 6:36p Mmouni
 * Added  new alpha texture primitive.
 * 
 * 23    98.06.17 6:45p Mmouni
 * Removed pixel counting (it was broken for planar N-sided polygons)
 * 
 * 22    98.06.12 3:36p Mmouni
 * Added optimized versions of planar polygon gradient routines.
 * 
 * 21    3/25/98 11:55a Kmckis
 * Added missing comma.
 * 
 * 20    98.03.24 8:17p Mmouni
 * Made changes to support alternate perspective correction settings.
 * 
 * 19    3/05/98 3:37p Pkeet
 * Added the 'bCAT_PIXEL_STAT' macro switch.
 * 
 * 18    2/26/98 2:54p Mmouni
 * DrawTriangle routines now only do femms at the start and end.
 * 
 * 17    2/17/98 11:10p Mmouni
 * Fixed problem with K6-3D GenericInitTriangleDataGour() routine.
 * 
 * 16    98.02.17 10:20p Mmouni
 * Gouraud shaded gradient is now zeroed if gradients from any two subtriangles have different
 * signs.
 * 
 * 15    98.01.22 3:14p Mmouni
 * Removed bump-map depth conditional compilation.
 * 
 * 14    1/19/98 7:34p Pkeet
 * Added support for 16 bit bumpmaps.
 * 
 * 13    98.01.14 9:08p Mmouni
 * Made changes for K6 3D polygon setup optimizations.
 * 
 * 12    97.12.17 7:05p Mmouni
 * Replaced fdivs with reciprocal lookups.
 * 
 * 11    97.12.11 7:10p Mmouni
 * Fixed error in edge setup.
 * 
 * 10    97/12/08 17:44 Speter
 * Updated offsets for new non-virtual CRenderPoly.
 * 
 * 9     97.11.25 9:23p Mmouni
 * Fixed error in bInitTriangleData for terrain fog primitives.
 * 
 * 8     97.11.25 7:02p Mmouni
 * Finished Pentium/Pentium Pro triangle setup optimization.
 * 
 * 7     97.11.14 11:59p Mmouni
 * Added specialized versions of InitializePolygonData for terrain texture that normalize the
 * texture co-ordinates for tiled textures.
 * 
 * 6     10/30/97 3:41p Mlange
 * Fixed broken build.
 * 
 * 5     97.10.30 1:41p Mmouni
 * Added Asserts for assumptions made by assembly routines.
 * 
 * 
 * 4     97.10.27 1:27p Mmouni
 * Made changes to support the K6-3D.
 * 
 * 3     97.10.16 2:15p Mmouni
 * Added in some missing debug version only setup.
 * 
 * 2     97.10.15 7:39p Mmouni
 * Added more optimized stuff.
 * 
 * 1     10/13/97 7:11p Mmouni
 * 
 **********************************************************************************************/

// Includes.
#include "Common.hpp"
#include "Config.hpp"
#include "DrawTriangle.hpp"
#include "AsmSupport.hpp"


// Make sure we have the best optimization.
#pragma optimize("tag", on)


#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

// Disable useless warning about no EMMS instruction in a function.
#pragma warning(disable: 4799)

// Stupid assembler complains about the size of an element of a struct,
// but assembles the instruction with the correct size anyway.
#pragma warning(disable: 4410)

#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D)


// Floating point constatnts.
const float fONE = 1.0f;
const float fTWO = 2.0f;
const float fPOINT_ZERO_ONE = 0.01f;
const float fRECIP_POINT_ZERO_ONE = 1.0f / 0.01f;
const float fSIXTEEN = 16.0f;
const float fNegativeFifty = -50.0f;

// Magic numbers for floating point conversions.
const double dFloatToInt = 6.755399441055744E15;
const double dFloatToFixed16 = 103079215104.0;
const double dFloatToFixed32 = 1572864.0;

// Temps.
static double d_temp_a;
static double d_temp_b;


//
// Class members we can't get to using normal means.
//
#define CRenderPolygon_paprvPolyVertices_uLen		0
#define CRenderPolygon_paprvPolyVertices_atArray	4


//*****************************************************************************************
//
// Generic functions and macros that implement the funcionality of the routines.
//
//*****************************************************************************************

#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)


// Packed constants.
const float pfFixed16Scale[2]	= { 65536.0f, 65536.0f };
const int pf_abs_mask[2]		= { 0x7fffffff, 0x7fffffff };
const int64 qFpSignBits			= 0x8000000080000000;

const float fInverseTwoToTheSixteenth = 1.0f / 65536.0f;


//*****************************************************************************************
//
inline void GenericInitializePolygonData(CRenderPolygon *rpoly)
//
// Scanline data initialization for un-shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	qUVMasks = ((uint64)pras_texture->u4HeightTileMask << 48) |
			   ((uint64)pras_texture->u4WidthTileMask << 32)  |
			   ((uint64)pras_texture->u4HeightTileMask << 16) |
			   ((uint64)pras_texture->u4WidthTileMask);

	__asm
	{
		femms

		mov		eax,[pras_texture]					// Pointer to texture.

		mov		ecx,[rpoly]							// Pointer to polygon.
		lea		edi,[arvRasterVertices]				// Destination vertices.

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]

		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		movq	mm1,[eax]CRasterBase.fWidth			// fWidth,fHeight
		movq	[cvMaxCoords],mm1					// fTexWidth,Height  = pras_texture->fWidth,Height

		mov		ebx,[bClampUV]						// Clamp flag.

		test	ebx,ebx
		jz		VTXLOOP2

		movq	mm2,[pfTexEdgeTolerance]			// fTexEdgeTolerance,fTexEdgeTolerance

		//
		// Copy vertices & scale texture values with clamping.
		//
VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX *
		//								 fTexWidth  + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//								 fTexHeight + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm3,[eax]SRenderCoord.v3Screen.tZ	// Screen Z.

		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.
		pfmul		(m0,m1)								// Scaled texture co-ordinates.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		punpckldq	mm3,mm3								// Duplicate Z across quadword.

		pf2id		(m4,m4)								// Truncate to integer.

		pfadd		(m0,m2)								// Add edge tolerance to tX,tY.
		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		pfmul		(m0,m3)								// Multiply by screen z.
		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y

		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y
		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP1

		jmp			VTXDONE

		//
		// Copy vertices & scale texture values, no clamping.
		//
VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm3,[eax]SRenderCoord.v3Screen.tZ	// Screen Z.
		pf2id		(m4,m4)								// Truncate to integer.

		pfmul		(m0,m1)								// Scaled texture co-ordinates.
		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		punpckldq	mm3,mm3								// Duplicate Z across quadword.

		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X
		pfmul		(m0,m3)								// Multiply by screen z.

		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y
		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y

		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z
		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP2

VTXDONE:
		//femms
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataFlat(CRenderPolygon *rpoly)
//
// Scanline data initialization for flat shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	qUVMasks = ((uint64)pras_texture->u4HeightTileMask << 48) |
			   ((uint64)pras_texture->u4WidthTileMask << 32)  |
			   ((uint64)pras_texture->u4HeightTileMask << 16) |
			   ((uint64)pras_texture->u4WidthTileMask);

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = iPosFloatCast(rpoly->cvFace);
	u4ConstColour = 0;

#if VER_DEBUG
	iNumIntensities = 0;
	if (rpoly->ptexTexture && rpoly->ptexTexture->ppcePalClut && rpoly->ptexTexture->ppcePalClut->pclutClut)
	{
		iNumIntensities = rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues;
		Assert(iBaseIntensity < rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues);
	}
#endif

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapTexture<uint8>::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		femms

		mov		eax,[pras_texture]					// Pointer to texture.

		mov		ecx,[rpoly]							// Pointer to polygon.
		lea		edi,[arvRasterVertices]				// Destination vertices.

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]

		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		movq	mm1,[eax]CRasterBase.fWidth			// fWidth,fHeight
		movq	[cvMaxCoords],mm1					// fTexWidth,Height  = pras_texture->fWidth,Height

		mov		ebx,[bClampUV]						// Clamp flag.

		test	ebx,ebx
		jz		VTXLOOP2

		movq	mm2,[pfTexEdgeTolerance]			// fTexEdgeTolerance,fTexEdgeTolerance

		//
		// Copy vertices & scale texture values with clamping.
		//
VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX *
		//								 fTexWidth  + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//								 fTexHeight + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm3,[eax]SRenderCoord.v3Screen.tZ	// Screen Z.

		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.
		pfmul		(m0,m1)								// Scaled texture co-ordinates.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		punpckldq	mm3,mm3								// Duplicate Z across quadword.

		pf2id		(m4,m4)								// Truncate to integer.
		pfadd		(m0,m2)								// Add edge tolerance to tX,tY.

		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		pfmul		(m0,m3)								// Multiply by screen z.
		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y

		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y
		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP1

		jmp			VTXDONE

		//
		// Copy vertices & scale texture values, no clamping.
		//
VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm3,[eax]SRenderCoord.v3Screen.tZ	// Screen Z.
		pf2id		(m4,m4)								// Truncate to integer.

		pfmul		(m0,m1)								// Scaled texture co-ordinates.
		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		punpckldq	mm3,mm3								// Duplicate Z across quadword.

		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X
		pfmul		(m0,m3)								// Multiply by screen z.

		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y
		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y

		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z
		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP2

VTXDONE:
		//femms
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataGour(CRenderPolygon *rpoly)
//
// Scanline data initialization for gouraud shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	qUVMasks = ((uint64)pras_texture->u4HeightTileMask << 48) |
			   ((uint64)pras_texture->u4WidthTileMask << 32)  |
			   ((uint64)pras_texture->u4HeightTileMask << 16) |
			   ((uint64)pras_texture->u4WidthTileMask);

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = 0;
	u4ConstColour = 0;

#if VER_DEBUG
	iNumIntensities = rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues;
#endif

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapTexture<uint8>::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		femms

		mov		eax,[pras_texture]					// Pointer to texture.

		mov		ecx,[rpoly]							// Pointer to polygon.
		lea		edi,[arvRasterVertices]				// Destination vertices.

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]

		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		movq	mm1,[eax]CRasterBase.fWidth			// fWidth,fHeight
		movq	[cvMaxCoords],mm1					// fTexWidth,Height  = pras_texture->fWidth,Height

		mov		ebx,[bClampUV]						// Clamp flag.

		test	ebx,ebx
		jz		VTXLOOP2

		movq	mm2,[pfTexEdgeTolerance]			// fTexEdgeTolerance,fTexEdgeTolerance

		//
		// Copy vertices & scale texture values with clamping.
		//
VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX *
		//								 fTexWidth  + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//								 fTexHeight + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm3,[eax]SRenderCoord.v3Screen.tZ	// Screen Z.

		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.
		pfmul		(m0,m1)								// Scaled texture co-ordinates.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		punpckldq	mm3,mm3								// Duplicate Z across quadword.

		pf2id		(m4,m4)								// Truncate to integer.

		pfadd		(m0,m2)								// Add edge tolerance to tX,tY.
		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		pfmul		(m0,m3)								// Multiply by screen z.
		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y

		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y
		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z
		mov			ebx,[eax]SRenderVertex.cvIntensity	// Load intenstity.

		mov			[edi]SRenderVertex.cvIntensity,ebx	// Store intensity.
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP1

		jmp			VTXDONE

		//
		// Copy vertices & scale texture values, no clamping.
		//
VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm3,[eax]SRenderCoord.v3Screen.tZ	// Screen Z.
		pf2id		(m4,m4)								// Truncate to integer.

		pfmul		(m0,m1)								// Scaled texture co-ordinates.
		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		punpckldq	mm3,mm3								// Duplicate Z across quadword.

		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X
		pfmul		(m0,m3)								// Multiply by screen z.

		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y
		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y

		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z
		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z

		mov			ebx,[eax]SRenderVertex.cvIntensity	// Load intenstity.
		mov			[edi]SRenderVertex.cvIntensity,ebx	// Store intensity.

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP2

VTXDONE:
		//femms
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataBump(CRenderPolygon *rpoly)
//
// Scanline data initialization for bump shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	qUVMasks = ((uint64)pras_texture->u4HeightTileMask << 48) |
			   ((uint64)pras_texture->u4WidthTileMask << 32)  |
			   ((uint64)pras_texture->u4HeightTileMask << 16) |
			   ((uint64)pras_texture->u4WidthTileMask);

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = 0;
	u4ConstColour = 0;

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapBump::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		femms

		mov		eax,[pras_texture]					// Pointer to texture.

		mov		ecx,[rpoly]							// Pointer to polygon.
		lea		edi,[arvRasterVertices]				// Destination vertices.

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]

		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		movq	mm1,[eax]CRasterBase.fWidth			// fWidth,fHeight
		movq	[cvMaxCoords],mm1					// fTexWidth,Height  = pras_texture->fWidth,Height

		mov		ebx,[bClampUV]						// Clamp flag.

		test	ebx,ebx
		jz		VTXLOOP2

		movq	mm2,[pfTexEdgeTolerance]			// fTexEdgeTolerance,fTexEdgeTolerance

		//
		// Copy vertices & scale texture values with clamping.
		//
VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX *
		//								 fTexWidth  + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//								 fTexHeight + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm3,[eax]SRenderCoord.v3Screen.tZ	// Screen Z.

		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.
		pfmul		(m0,m1)								// Scaled texture co-ordinates.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		punpckldq	mm3,mm3								// Duplicate Z across quadword.

		pf2id		(m4,m4)								// Truncate to integer.
		pfadd		(m0,m2)								// Add edge tolerance to tX,tY.

		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		pfmul		(m0,m3)								// Multiply by screen z.
		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y

		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y
		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP1

		jmp			VTXDONE

		//
		// Copy vertices & scale texture values, no clamping.
		//
VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm3,[eax]SRenderCoord.v3Screen.tZ	// Screen Z.
		pf2id		(m4,m4)								// Truncate to integer.

		pfmul		(m0,m1)								// Scaled texture co-ordinates.
		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		punpckldq	mm3,mm3								// Duplicate Z across quadword.

		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X
		pfmul		(m0,m3)								// Multiply by screen z.

		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y
		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y

		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z
		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP2

VTXDONE:
		//femms
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataLinear(CRenderPolygon *rpoly)
//
// Scanline data initialization for un-shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	qUVMasks = ((uint64)pras_texture->u4HeightTileMask << 48) |
			   ((uint64)pras_texture->u4WidthTileMask << 32)  |
			   ((uint64)pras_texture->u4HeightTileMask << 16) |
			   ((uint64)pras_texture->u4WidthTileMask);

	__asm
	{
		femms

		mov		eax,[pras_texture]					// Pointer to texture.

		mov		ecx,[rpoly]							// Pointer to polygon.
		lea		edi,[arvRasterVertices]				// Destination vertices.

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]

		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		movq	mm1,[eax]CRasterBase.fWidth			// fWidth,fHeight
		movq	[cvMaxCoords],mm1					// fTexWidth,Height  = pras_texture->fWidth,Height

		mov		ebx,[bClampUV]						// Clamp flag.

		test	ebx,ebx
		jz		VTXLOOP2

		movq	mm2,[pfTexEdgeTolerance]			// fTexEdgeTolerance,fTexEdgeTolerance

		//
		// Copy vertices & scale texture values with clamping.
		//
VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX *
		//									fTexWidth + fTexEdgeTolerance
		//
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//									fTexHeight + fTexEdgeTolerance
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		pfmul		(m0,m1)								// Scaled texture co-ordinates.

		pf2id		(m4,m4)								// Truncate to integer.
		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X

		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.
		pfadd		(m0,m2)								// Add edge tolerance to tX,tY.

		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y
		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y

		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z
		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP1

		jmp			VTXDONE

		//
		// Copy vertices & scale texture values, no clamping.
		//
VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		pf2id		(m4,m4)								// Truncate to integer.

		pfmul		(m0,m1)								// Scaled texture co-ordinates.
		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X
		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y

		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y
		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP2

VTXDONE:
		//femms
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataLinearFlat(CRenderPolygon *rpoly)
//
// Scanline data initialization for flat shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	qUVMasks = ((uint64)pras_texture->u4HeightTileMask << 48) |
			   ((uint64)pras_texture->u4WidthTileMask << 32)  |
			   ((uint64)pras_texture->u4HeightTileMask << 16) |
			   ((uint64)pras_texture->u4WidthTileMask);

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = iPosFloatCast(rpoly->cvFace);
	u4ConstColour = 0;

#if VER_DEBUG
	iNumIntensities = 0;
	if (rpoly->ptexTexture && rpoly->ptexTexture->ppcePalClut && rpoly->ptexTexture->ppcePalClut->pclutClut)
	{
		iNumIntensities = rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues;
		Assert(iBaseIntensity < rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues);
	}
#endif

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapTexture<uint8>::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		femms

		mov		eax,[pras_texture]					// Pointer to texture.

		mov		ecx,[rpoly]							// Pointer to polygon.
		lea		edi,[arvRasterVertices]				// Destination vertices.

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]

		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		movq	mm1,[eax]CRasterBase.fWidth			// fWidth,fHeight
		movq	[cvMaxCoords],mm1					// fTexWidth,Height  = pras_texture->fWidth,Height

		mov		ebx,[bClampUV]						// Clamp flag.

		test	ebx,ebx
		jz		VTXLOOP2

		movq	mm2,[pfTexEdgeTolerance]			// fTexEdgeTolerance,fTexEdgeTolerance

		//
		// Copy vertices & scale texture values with clamping.
		//
VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX *
		//									fTexWidth + fTexEdgeTolerance
		//
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//									fTexHeight + fTexEdgeTolerance
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		pfmul		(m0,m1)								// Scaled texture co-ordinates.

		pf2id		(m4,m4)								// Truncate to integer.
		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X

		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.
		pfadd		(m0,m2)								// Add edge tolerance to tX,tY.

		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y
		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y

		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z
		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP1

		jmp			VTXDONE

		//
		// Copy vertices & scale texture values, no clamping.
		//
VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		pf2id		(m4,m4)								// Truncate to integer.

		pfmul		(m0,m1)								// Scaled texture co-ordinates.
		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X
		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y

		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y
		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP2

VTXDONE:
		//femms
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataLinearGour(CRenderPolygon *rpoly)
//
// Scanline data initialization for gouraud shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	qUVMasks = ((uint64)pras_texture->u4HeightTileMask << 48) |
			   ((uint64)pras_texture->u4WidthTileMask << 32)  |
			   ((uint64)pras_texture->u4HeightTileMask << 16) |
			   ((uint64)pras_texture->u4WidthTileMask);

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = 0;
	u4ConstColour = 0;

#if VER_DEBUG
	iNumIntensities = rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues;
#endif

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapTexture<uint8>::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		femms

		mov		eax,[pras_texture]					// Pointer to texture.

		mov		ecx,[rpoly]							// Pointer to polygon.
		lea		edi,[arvRasterVertices]				// Destination vertices.

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]

		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		movq	mm1,[eax]CRasterBase.fWidth			// fWidth,fHeight
		movq	[cvMaxCoords],mm1					// fTexWidth,Height  = pras_texture->fWidth,Height

		mov		ebx,[bClampUV]						// Clamp flag.

		test	ebx,ebx
		jz		VTXLOOP2

		movq	mm2,[pfTexEdgeTolerance]			// fTexEdgeTolerance,fTexEdgeTolerance

		//
		// Copy vertices & scale texture values with clamping.
		//
VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX *
		//									fTexWidth + fTexEdgeTolerance
		//
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//									fTexHeight + fTexEdgeTolerance
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		pfmul		(m0,m1)								// Scaled texture co-ordinates.

		pf2id		(m4,m4)								// Truncate to integer.
		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X

		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.
		pfadd		(m0,m2)								// Add edge tolerance to tX,tY.

		mov			ebx,[eax]SRenderVertex.cvIntensity	// Load intenstity.
		mov			[edi]SRenderVertex.cvIntensity,ebx	// Store intensity.

		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y
		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y

		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z
		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP1

		jmp			VTXDONE

		//
		// Copy vertices & scale texture values, no clamping.
		//
VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		pf2id		(m4,m4)								// Truncate to integer.

		pfmul		(m0,m1)								// Scaled texture co-ordinates.
		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X
		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y

		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y
		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z

		mov			ebx,[eax]SRenderVertex.cvIntensity	// Load intenstity.
		mov			[edi]SRenderVertex.cvIntensity,ebx	// Store intensity.

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP2

VTXDONE:
		//femms
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataLinearBump(CRenderPolygon *rpoly)
//
// Scanline data initialization for bump shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	qUVMasks = ((uint64)pras_texture->u4HeightTileMask << 48) |
			   ((uint64)pras_texture->u4WidthTileMask << 32)  |
			   ((uint64)pras_texture->u4HeightTileMask << 16) |
			   ((uint64)pras_texture->u4WidthTileMask);

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = 0;
	u4ConstColour = 0;

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapBump::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		femms

		mov		eax,[pras_texture]					// Pointer to texture.

		mov		ecx,[rpoly]							// Pointer to polygon.
		lea		edi,[arvRasterVertices]				// Destination vertices.

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]

		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		movq	mm1,[eax]CRasterBase.fWidth			// fWidth,fHeight
		movq	[cvMaxCoords],mm1					// fTexWidth,Height  = pras_texture->fWidth,Height

		mov		ebx,[bClampUV]						// Clamp flag.

		test	ebx,ebx
		jz		VTXLOOP2

		movq	mm2,[pfTexEdgeTolerance]			// fTexEdgeTolerance,fTexEdgeTolerance

		//
		// Copy vertices & scale texture values with clamping.
		//
VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX *
		//									fTexWidth + fTexEdgeTolerance
		//
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//									fTexHeight + fTexEdgeTolerance
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		pfmul		(m0,m1)								// Scaled texture co-ordinates.

		pf2id		(m4,m4)								// Truncate to integer.
		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X

		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.
		pfadd		(m0,m2)								// Add edge tolerance to tX,tY.

		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y
		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y

		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z
		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP1

		jmp			VTXDONE

		//
		// Copy vertices & scale texture values, no clamping.
		//
VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		movd		mm4,[eax]SRenderCoord.v3Screen.tY	// Screen Y.

		movq		mm0,[eax]SRenderVertex.tcTex.tX		// tX,tY

		mov			ecx,[eax]SRenderCoord.v3Screen.tX	// Load X
		pf2id		(m4,m4)								// Truncate to integer.

		pfmul		(m0,m1)								// Scaled texture co-ordinates.
		movd		[edi]SRenderCoord.iYScr,mm4			// Store truncated value.

		mov			[edi]SRenderCoord.v3Screen.tX,ecx	// Store X
		mov			ebx,[eax]SRenderCoord.v3Screen.tY	// Load Y

		mov			[edi]SRenderCoord.v3Screen.tY,ebx	// Store Y
		mov			ecx,[eax]SRenderCoord.v3Screen.tZ	// Load Z

		mov			[edi]SRenderCoord.v3Screen.tZ,ecx	// Store Z

		movq		[edi]SRenderVertex.tcTex,mm0		// Store texture co-ordinates.
		add			edi,SIZE SRenderVertex				// Step destination.

		dec			edx
		jnz			VTXLOOP2

VTXDONE:
		//femms
	}
}


//*****************************************************************************************
//
template <class aCEdge> void GenericInitializeEdge
(
	aCEdge &edge,				// The edge to initialize.
	SRenderVertex* prv_from,	// Starting coordinate of edge.
	SRenderVertex* prv_to		// End coordinate of edge.
)
//
// Sets up a polygon edge.
//
//**************************************
{
	typedef aCEdge::TScanline aScanline;
	int i4temp[2];
	float f_dy;

	__asm
	{
		//femms

		mov		esi,[prv_from]						// From vertex.
		mov		ebx,1
		 
		mov		edi,[prv_to]						// To vertex.
		mov		[i4temp+4],ebx						// Put one in temp+4

		mov		eax,[esi]SRenderCoord.iYScr			// Integer y.

		inc		eax									// iy + 1
		mov		[i4temp],eax						// Save in temp+0.

		movd	mm1,[edi]SRenderCoord.v3Screen.tY	// prv_to->v3Screen.tY

		movd	mm2,[esi]SRenderCoord.v3Screen.tY	// prv_from->v3Screen.tY

		movq	mm0,[i4temp]						// 1 | prv_from->iYScr+1

		mov		edx,[edge]							// Load edge poniter.
		pfsub	(m1,m2)								// mm1 = prv_to->v3Screen.tY - prv_from->v3Screen.tY  

		pi2fd	(m0,m0)								// (float)(prv_from->iYScr + 1) 

		pfsub	(m0,m2)								// mm0 = (float)(prv_from->iYScr + 1) - prv_from->v3Screen.tY  
		mov		[edx]aCEdge.prvFrom,esi				// prvFrom = prv_from  

		mov		[edx]aCEdge.prvTo,edi				// prvTo   = prv_to  

		movd	[f_dy],mm1							// Save f_dy (mm1)
		mov		ebx,[fPOINT_ZERO_ONE]				// Load minimum constant  

		movd	[edx]aCEdge.fStartYDiff,mm0			// Save fStartYDiff (mm0)
		mov		eax,[f_dy]							// Load f_dy  

		cmp		eax,ebx								// f_dy >= 0.01  
		jge		short DO_DIVIDE								

		movd	mm2,[fRECIP_POINT_ZERO_ONE]			// Clamp fInvDY to 1 / 0.01

		movd	mm4,[esi]SRenderCoord.v3Screen.tX	// prv_from->v3Screen.tX

		movd	mm3,[edi]SRenderCoord.v3Screen.tX	// prv_to->v3Screen.tX

		jmp		short DONE_WITH_DIVIDE						

DO_DIVIDE: 
		pfrcp		(m3,m1)								// mm2 = 1.0 / f_dy
		movq		mm2,mm1								// copy f_dy

		pfrcpit1	(m2,m3)								// Newton-Raphson interation 1.
		movd		mm4,[esi]SRenderCoord.v3Screen.tX	// prv_from->v3Screen.tX

		pfrcpit2	(m2,m3)								// Newton-Raphson interation 2.
		movd		mm3,[edi]SRenderCoord.v3Screen.tX	// prv_to->v3Screen.tX

DONE_WITH_DIVIDE: 
		mov		eax,[esi]SRenderCoord.iYScr			// prv_from->iYScr
		movd	[edx]aCEdge.fInvDY,mm2				// Save fInvDY (mm2)

		pfsub	(m3,m4)								// prv_to->v3Screen.tX - prv_from->v3Screen.tX
		mov		ebx,[edi]SRenderCoord.iYScr			// prv_to->iYScr

		cmp		eax,ebx								// if (prv_from->iYScr == prv_to->iYScr)
		je		RETURN_FROM_FUNC					// return;

		pfmul	(m3,m2)								// f_increment_x = fInvDY * (prv_to->v3Screen.tX - prv_from->v3Screen.tX)
		movq	mm5,[pfFixed16Scale]				// 2^16 scaling factor

		punpckldq	mm3,mm3							// Duplicate f_i_x into both dwords.

		pfmul	(m3,m0)								// f_i_x | f_i_x*fStartYDiff
		
		pfadd	(m3,m4)								// f_i_x | f_i_x*fStartYDiff+tX
	
		pfmul	(m3,m5)								// Scale by 2^16.

		pf2id	(m3,m3)								// Convert to integer.

		movd	[edx]aCEdge.lineStart.fxX,mm3		// Store converted (f_i_x*fStartYDiff+tX)
		psrlq	mm3,32								// Shift f_i_x down

		movd	[edx]aCEdge.lineIncrement.fxX,mm3	// Store converted f_i_x

RETURN_FROM_FUNC:

		//femms
	}
}


//*****************************************************************************************
//
inline int iGetSubdivisionLen_3DX
(
	float f_dinvz,
	int b_usealt
)
//
// Returns the number of pixels to subdivide over given an inverse Z delta.
// Avoid using FPU code since this is called from a 3dx only routine.
//
//**************************************
{
	int i_subdivisionlen;
//	typedef CPerspectiveSettings tdPerspectiveSettings;

	// Use adaptive value?
	if (persetSettings.bAdaptive) __asm
	{
		mov		eax,[f_dinvz]

		mov		ebx,persetSettings.iAdaptiveMaxSubdivision
		and		eax,0x7fffffff

		// If the value is too small, use max.
		cmp		eax,persetSettings.fInvZForMaxSubdivision
		jl		RETURN_EBX

		/* Decrease the subdivision length based on the 1/z delta.
		i_subdivisionlen = iAdaptiveMaxSubdivision - 
						   Fist((f_dinvz - fInvZForMaxSubdivision) * fInvZScale);
		*/

		movd	mm0,eax
		movd	mm1,persetSettings.fInvZForMaxSubdivision
		movd	mm2,persetSettings.fInvZScale
		pfsub	(m0,m1)
		pfmul	(m0,m2)
		pf2id	(m0,m0)
		movd	eax,mm0
		sub		ebx,eax

		// Make subdivision length a multiple of 2.
		and		ebx,0xfffffffe
		mov		eax,persetSettings.iAdaptiveMinSubdivision

		cmp		ebx,eax
		jge		RETURN_EBX

		mov		ebx,eax

RETURN_EBX:
		mov		[i_subdivisionlen],ebx
	}
	else
	{
		if (b_usealt)
			i_subdivisionlen = persetSettings.iAltMinSubdivision;
		else
			i_subdivisionlen = persetSettings.iMinSubdivision;
	}

	// Return the value.
	return i_subdivisionlen;
}


//*****************************************************************************************
//
inline bool GenericInitTriangleData
(
	CDrawPolygonBase*					poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	int									b_update,
	bool								b_altpersp
)
//
// Sets up triangle-wide rasterising info for 
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	Assert(prv_a);
	Assert(prv_b);
	Assert(prv_c);

	//
	// Calculate fInvDx, the partial derivative of x over the triangle, using the following
	// formula:
	//
	//		           1                       1
	//		fInvDx = ---- = ---------------------------------------
	//	              dx    (x2 - x1)(y3 - y1) - (x3 - x1)(y2 - y1)
	//
	// dx also happens to be -2 times the area of the triangle, so we can check for correct
	// orientation here as well.
	//
	float f_dx;

	__asm
	{
		//femms

		mov		edx,[prv_a]						// prv_a = edx

		mov		esi,[prv_b]						// prv_b = esi

		mov		edi,[prv_c]						// prv_c = edi

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		movq		mm0,[edx]SRenderCoord.v3Screen.tX	// prv_a->tY | tX

		movq		mm1,[esi]SRenderCoord.v3Screen.tX	// prv_b->tY | tX

		movq		mm2,[edi]SRenderCoord.v3Screen.tX	// prv_c->tY | tX

		pfsub		(m1,m0)								// b-a

		pfsub		(m2,m0)								// c-a
		movq		mm5,mm1								// save mm1

		// stall

		punpckhdq	mm1,mm2								// f_yac = cY-aY | f_yab = bY-aY
		punpckldq	mm2,mm5								// bX-aX | cX-aX

		pfmul		(m2,m1)								// bX-aX * cY-aY | cX-aX * bY-aY

		// stall

		punpckldq	mm5,mm2								// cX-aX * bY-aY | junk

		pfsub		(m2,m5)								// (bX-aX * cY-aY) - (cX-aX * bY-aY) | junk
		movd		mm5,[fMAX_NEG_AREA]					// Load value for comparison

		punpckhdq	mm2,mm2								// f_dx | f_dx

		movq		mm0,mm2								// copy f_dx

		pfcmpge		(m0,m5)								// Set to mm0 ones if (mm0 >= mm5)

		// stall

		movd		eax,mm0

		test		eax,eax								// if (f_dx >= fMAX_NEG_AREA)
		jnz			RETURN_FALSE

		//
		// Calculate coefficients to pass to template InitializeTriangleData functions.
		//
		// float f_invdx = 1.0f / f_dx;
		// float f_yab_invdx = f_yab * f_invdx;
		// float f_yac_invdx = f_yac * f_invdx;
		//
		pfrcp		(m0,m2)								// f_invdx = 1.0 / f_dx
		movq		mm6,[edx]SRenderVertex.tcTex.tX		// prv_a->tY | tX

		movd		[f_dx],mm2							// save f_dx
		movq		mm3,[esi]SRenderVertex.tcTex.tX		// prv_b->tY | tX

		pfrcpit1	(m2,m0)								// Newton-Raphson interation 1.
		movq		mm4,[edi]SRenderVertex.tcTex.tX		// prv_c->tY | tX

		pfsub		(m3,m6)								// f_vab | f_uab
		movd		mm5,[esi]SRenderCoord.v3Screen.tZ	// prv_b->tZ

		pfrcpit2	(m2,m0)								// Newton-Raphson interation 2.
		pfsub		(m4,m6)								// f_vac | f_uac

		movd		mm6,[edi]SRenderCoord.v3Screen.tZ	// prv_c->tZ

		pfmul		(m1,m2)								// f_yac_invdx | f_yab_invdx
		movd		mm2,[edx]SRenderCoord.v3Screen.tZ	// prv_a->tZ

		//
		// Get the step values for u and v with respect to x.
		//
		// float f_duinvz = (prv_b->tcTex.tX - prv_a->tcTex.tX) * f_yac_invdx -
		//				    (prv_c->tcTex.tX - prv_a->tcTex.tX) * f_yab_invdx;
		//
		// float f_dvinvz = (prv_b->tcTex.tY - prv_a->tcTex.tY) * f_yac_invdx -
		//				    (prv_c->tcTex.tY - prv_a->tcTex.tY) * f_yab_invdx;
		//
		// float f_dinvz =  (prv_b->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yac_invdx -
		//				    (prv_c->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yab_invdx;
		//
		punpckldq	mm6,mm5								// prv_b->tZ | prv_c->tZ

		movq		mm0,mm1								// copy f_yac_invdx | f_yab_invdx
		punpckhdq	mm1,mm1								// f_yac_invdx | f_yac_invdx
	
		pfmul		(m3,m1)								// f_vab*f_yab_invdx | f_uab*f_yab_invdx
		movq		mm1,mm0								// copy f_yac_invdx | f_yab_invdx

		punpckldq	mm0,mm0								// f_yab_invdx | f_yab_invdx
		punpckldq	mm2,mm2								// prv_a->tZ | prv_a->tZ

		pfmul		(m4,m0)								// f_vac*f_yab_invdx | f_uac*f_yab_invdx
		pfsub		(m6,m2)								// f_zab | f_zac

		pfmul		(m6,m1)								// f_zab * f_yac_invdx | f_zac * f_yab_invdx

		pfsub		(m3,m4)								// f_dvinvz | f_duinvz
	
		punpckldq	mm0,mm6								// f_zac * f_yab_invdx | junk

		pfsub		(m6,m0)								// f_dinvz | junk
		mov			eax,[b_update]

		test		eax,eax

		psrlq		mm6,32								// 0 | f_dinvz
		jz			short COPY_UVZ

		//
		// SetMinAbs(fDUInvZ, f_duinvz);
		// SetMinAbs(fDVInvZ, f_dvinvz);
		// SetMax(fDInvZ, f_dinvz);
		//
		movq	mm0,[fDUInvZ]						// fDVInvZ | fDUInvz
		movq	mm1,mm3								// Copy (f_dvinvz | f_duinvz)

		movq	mm4,[pf_abs_mask]

		movq	mm2,mm0								// Copy (fDVInvZ | fDUInvz)

		pand	mm0,mm4								// Mask off sign bits
		pand	mm1,mm4								// Mask off sign bits

		movd	mm5,[fDInvZ]						// 0 | fDInvZ
		pfcmpge	(m0,m1)								// Set ones where we should copy mm3

		pand	mm3,mm0								// Zero parts of new values we don't want.
		pandn	mm0,mm2								// Copy parts of old values we want.

		por		mm3,mm0								// Or them together.
		pfmax	(m6,m5)								// Take maximum of fDInvZ, f_dinvz

COPY_UVZ:
		//
		// fDUInvZ = f_duinvz;
		// fDVInvZ = f_dvinvz;
		// fDInvZ  = f_dinvz;
		//
		movq	[fDUInvZ],mm3						// Store updated values.
		movd	[fDInvZ],mm6
	}

	// Calculate the subdivision length with respect to X.
	iSubdivideLen    = iGetSubdivisionLen_3DX(fDInvZ, b_altpersp);
	fInvSubdivideLen = fInverseIntTable[iSubdivideLen];

	//__asm femms
	return true;

RETURN_FALSE:
	//__asm femms
	return false;
}


//*****************************************************************************************
//
inline bool GenericInitTriangleDataGour
(
	CDrawPolygonBase*					poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	int									b_update,
	bool								b_altpersp
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	Assert(prv_a);
	Assert(prv_b);
	Assert(prv_c);

	//
	// Calculate fInvDx, the partial derivative of x over the triangle, using the following
	// formula:
	//
	//		           1                       1
	//		fInvDx = ---- = ---------------------------------------
	//	              dx    (x2 - x1)(y3 - y1) - (x3 - x1)(y2 - y1)
	//
	// dx also happens to be -2 times the area of the triangle, so we can check for correct
	// orientation here as well.
	//
	float f_dx;

	__asm
	{
		//femms

		mov		edx,[prv_a]						// prv_a = edx

		mov		esi,[prv_b]						// prv_b = esi

		mov		edi,[prv_c]						// prv_c = edi

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		movq		mm0,[edx]SRenderCoord.v3Screen.tX	// prv_a->tY | tX

		movq		mm1,[esi]SRenderCoord.v3Screen.tX	// prv_b->tY | tX

		movq		mm2,[edi]SRenderCoord.v3Screen.tX	// prv_c->tY | tX

		pfsub		(m1,m0)								// b-a

		pfsub		(m2,m0)								// c-a
		movq		mm5,mm1								// save mm1

		// stall

		punpckhdq	mm1,mm2								// f_yac = cY-aY | f_yab = bY-aY
		punpckldq	mm2,mm5								// bX-aX | cX-aX

		pfmul		(m2,m1)								// bX-aX * cY-aY | cX-aX * bY-aY

		// stall

		punpckldq	mm5,mm2								// cX-aX * bY-aY | junk

		pfsub		(m2,m5)								// (bX-aX * cY-aY) - (cX-aX * bY-aY) | junk
		movd		mm5,[fMAX_NEG_AREA]					// Load value for comparison

		punpckhdq	mm2,mm2								// f_dx | f_dx

		movq		mm0,mm2								// copy f_dx

		pfcmpge		(m0,m5)								// Set to mm0 ones if (mm0 >= mm3)

		// stall

		movd		eax,mm0

		test		eax,eax								// if (f_dx >= fMAX_NEG_AREA)
		jnz			RETURN_FALSE

		//
		// Calculate coefficients to pass to template InitializeTriangleData functions.
		//
		// float f_invdx = 1.0f / f_dx;
		// float f_yab_invdx = f_yab * f_invdx;
		// float f_yac_invdx = f_yac * f_invdx;
		//
		pfrcp		(m0,m2)								// f_invdx = 1.0 / f_dx
		movq		mm6,[edx]SRenderVertex.tcTex.tX		// prv_a->tY | tX

		movd		[f_dx],mm2							// save f_dx
		movq		mm3,[esi]SRenderVertex.tcTex.tX		// prv_b->tY | tX

		pfrcpit1	(m2,m0)								// Newton-Raphson interation 1.
		movq		mm4,[edi]SRenderVertex.tcTex.tX		// prv_c->tY | tX

		pfsub		(m3,m6)								// f_vab | f_uab
		movd		mm5,[esi]SRenderCoord.v3Screen.tZ	// prv_b->tZ

		pfrcpit2	(m2,m0)								// Newton-Raphson interation 2.
		pfsub		(m4,m6)								// f_vac | f_uac

		movd		mm6,[edx]SRenderCoord.v3Screen.tZ	// prv_a->tZ
		punpckldq	mm5,[esi]SRenderVertex.cvIntensity	// prv_b->cvIntensity | prv_b->tZ

		pfmul		(m1,m2)								// f_yac_invdx | f_yab_invdx
		movd		mm2,[edi]SRenderCoord.v3Screen.tZ	// prv_c->tZ

		//
		// Get the step values for u and v with respect to x.
		//
		// float f_duinvz = (prv_b->tcTex.tX - prv_a->tcTex.tX) * f_yac_invdx -
		//				    (prv_c->tcTex.tX - prv_a->tcTex.tX) * f_yab_invdx;
		//
		// float f_dvinvz = (prv_b->tcTex.tY - prv_a->tcTex.tY) * f_yac_invdx -
		//				    (prv_c->tcTex.tY - prv_a->tcTex.tY) * f_yab_invdx;
		//
		// float f_dinvz =  (prv_b->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yac_invdx -
		//				    (prv_c->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yab_invdx;
		//
		// Find the step value for intensity with respect to the horizontal axis.
		//
		// float f_di_invdx =	(prv_b->cvIntensity - prv_a->cvIntensity) * f_yac_invdx -
		//						(prv_c->cvIntensity - prv_a->cvIntensity) * f_yab_invdx;
		//
		punpckldq	mm6,[edx]SRenderVertex.cvIntensity	// prv_a->cvIntensity | prv_a->tZ

		movq		mm0,mm1								// copy (f_yac_invdx | f_yab_invdx)
		punpckldq	mm1,mm1								// f_yab_invdx | f_yab_invdx

		punpckldq	mm2,[edi]SRenderVertex.cvIntensity	// prv_c->cvIntensity | prv_c->tZ
		punpckhdq	mm0,mm0								// f_yac_invdx | f_yac_invdx

		pfmul		(m4,m1)								// f_vac*f_yab_invdx | f_uac*f_yab_invdx
		pfsub		(m5,m6)								// f_iab | f_zab

		pfmul		(m3,m0)								// f_vab*f_yac_invdx | f_uab*f_yac_invdx
		pfsubr		(m6,m2)								// f_iac | f_zac

		pfmul		(m5,m0)								// f_iab*f_yac_invdx | f_zab*f_yac_invdx
		pfsub		(m3,m4)								// f_dvinvz | f_duinvz

		pfmul		(m6,m1)								// f_iac*f_yab_invdx | f_zac*f_yab_invdx
		mov			eax,[b_update]

		test		eax,eax

		pfsubr		(m6,m5)								// f_di_invdx | f_dinvz
		jz			short COPY_UVZ

		//
		// SetMinAbs(fDUInvZ, f_duinvz);
		// SetMinAbs(fDVInvZ, f_dvinvz);
		// SetMax(fDInvZ, f_dinvz);
		//
		movq		mm0,[fDUInvZ]						// fDVInvZ | fDUInvz
		movq		mm1,mm3								// Copy (f_dvinvz | f_duinvz)

		movq		mm4,[pf_abs_mask]

		movq		mm2,mm0								// Copy (fDVInvZ | fDUInvz)

		pand		mm0,mm4								// Mask off sign bits
		pand		mm1,mm4								// Mask off sign bits

		movd		mm5,[fDInvZ]						// 0 | fDInvZ
		pfcmpge		(m0,m1)								// Set ones where we should copy mm3

		pand		mm3,mm0								// Zero parts of new values we don't want.
		pandn		mm0,mm2								// Copy parts of old values we want.

		por			mm3,mm0								// Or them together.
		movq		mm2,mm6								// Need a copy of f_di_invdx

		pfmax		(m6,m5)								// Take maximum of fDInvZ, f_dinvz

		//
		//	if (CIntFloat(f_di_invdx).bSign() != CIntFloat(fDeltaXIntensity).bSign())
		//	{
		//		fDeltaXIntensity = 0.0f;
		//		fxDeltaXIntensity = 0;
		//		goto SKIP_INTENSITY_UPDATE;
		//	}
		//
		// if (fabs(f_di_invdx) >= fabs(fDeltaXIntensity))
		//		goto SKIP_INTENSITY_UPDATE;
		//
		psrlq		mm2,32								// 0 | f_di_invdx

		mov			ebx,[fDeltaXIntensity]				// Get fDeltaXIntensity
		movd		eax,mm2

		punpckldq	mm6,mm2								// Repack as (f_di_invdx | f_invz)

		mov		ecx,eax									// copy f_di_invdx
		xor		eax,ebx									// xor sign bits

		and		eax,0x80000000							// test for sign bit.
		jz		COMPARE_INTENSITY						// same sign, compare abs. values

		mov		eax,0
		mov		[fDeltaXIntensity],eax

		mov		[fxDeltaXIntensity],eax
		jmp		SKIP_INTENSITY_UPDATE

COMPARE_INTENSITY:
		and		ecx,0x7fffffff
		and		ebx,0x7fffffff

		cmp		ecx,ebx
		jge		SKIP_INTENSITY_UPDATE
	
COPY_UVZ:
		//
		// fDeltaXIntensity = f_di_invdx;
		//
		// Clamp(fDeltaXIntensity, 16.0f);
		//
		// Convert to fixed point representation.
		//
		// fxDeltaXIntensity.fxFromFloat(fDeltaXIntensity);
		//
		movd	mm1,[fSIXTEEN]					// 0 | 16.0
		movq	mm2,mm6							// f_di_invdx | f_invz

		psrlq	mm2,32							// 0 | f_di_invdx
		movq	mm0,[pfFixed16Scale]			// 2^16 | 2^16

		pfmin	(m2,m1)							// Clamp to 16.0

		movd	[fDeltaXIntensity],mm2			// Save updated, possibly clamped value.
		pfmul	(m2,m0)							// Scale by 2^16

		pf2id	(m2,m2)

		movd	[fxDeltaXIntensity],mm2			// Save fixed point value.

SKIP_INTENSITY_UPDATE:
		//
		// fDUInvZ = f_duinvz;
		// fDVInvZ = f_dvinvz;
		// fDInvZ  = f_dinvz;
		//
		movq	[fDUInvZ],mm3						// Store updated values.
		movd	[fDInvZ],mm6
	}

	// Calculate the subdivision length with respect to X.
	iSubdivideLen    = iGetSubdivisionLen_3DX(fDInvZ, b_altpersp);
	fInvSubdivideLen = fInverseIntTable[iSubdivideLen];

	//__asm femms
	return true;

RETURN_FALSE:
	//__asm femms
	return false;
}


//*****************************************************************************************
//
inline bool GenericInitTriangleDataLinear
(
	CDrawPolygonBase*					poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	int									b_update
)
//
// Sets up triangle-wide rasterising info for 
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	Assert(prv_a);
	Assert(prv_b);
	Assert(prv_c);

	//
	// Calculate fInvDx, the partial derivative of x over the triangle, using the following
	// formula:
	//
	//		           1                       1
	//		fInvDx = ---- = ---------------------------------------
	//	              dx    (x2 - x1)(y3 - y1) - (x3 - x1)(y2 - y1)
	//
	// dx also happens to be -2 times the area of the triangle, so we can check for correct
	// orientation here as well.
	//
	float f_dx;

	__asm
	{
		//femms

		mov		edx,[prv_a]						// prv_a = edx

		mov		esi,[prv_b]						// prv_b = esi

		mov		edi,[prv_c]						// prv_c = edi

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		movq		mm0,[edx]SRenderCoord.v3Screen.tX	// prv_a->tY | tX

		movq		mm1,[esi]SRenderCoord.v3Screen.tX	// prv_b->tY | tX

		movq		mm2,[edi]SRenderCoord.v3Screen.tX	// prv_c->tY | tX

		pfsub		(m1,m0)								// b-a

		pfsub		(m2,m0)								// c-a
		movq		mm5,mm1								// save mm1

		// stall

		punpckhdq	mm1,mm2								// f_yac = cY-aY | f_yab = bY-aY
		punpckldq	mm2,mm5								// bX-aX | cX-aX

		pfmul		(m2,m1)								// bX-aX * cY-aY | cX-aX * bY-aY

		// stall

		punpckldq	mm5,mm2								// cX-aX * bY-aY | junk

		pfsub		(m2,m5)								// (bX-aX * cY-aY) - (cX-aX * bY-aY) | junk
		movd		mm5,[fMAX_NEG_AREA]					// Load value for comparison

		punpckhdq	mm2,mm2								// f_dx | f_dx

		movq		mm0,mm2								// copy f_dx

		pfcmpge		(m0,m5)								// Set to mm0 ones if (mm0 >= mm3)

		// stall

		movd		eax,mm0

		test		eax,eax								// if (f_dx >= fMAX_NEG_AREA)
		jnz			RETURN_FALSE

		//
		// Calculate coefficients to pass to template InitializeTriangleData functions.
		//
		// float f_invdx = 1.0f / f_dx;
		// float f_yab_invdx = f_yab * f_invdx;
		// float f_yac_invdx = f_yac * f_invdx;
		//
		pfrcp		(m0,m2)								// f_invdx = 1.0 / f_dx
		movq		mm6,[edx]SRenderVertex.tcTex.tX		// prv_a->tY | tX

		movd		[f_dx],mm2							// save f_dx
		movq		mm3,[esi]SRenderVertex.tcTex.tX		// prv_b->tY | tX

		pfrcpit1	(m2,m0)								// Newton-Raphson interation 1.
		movq		mm4,[edi]SRenderVertex.tcTex.tX		// prv_c->tY | tX

		pfsub		(m3,m6)								// f_vab | f_uab

		pfrcpit2	(m2,m0)								// Newton-Raphson interation 2.
		pfsub		(m4,m6)								// f_vac | f_uac

		// stall

		//
		// Get the step values for u and v with respect to x.
		//
		// float f_du = (prv_b->tcTex.tX - prv_a->tcTex.tX) * f_yac_invdx -
		//				(prv_c->tcTex.tX - prv_a->tcTex.tX) * f_yab_invdx;
		//
		// float f_dv = (prv_b->tcTex.tY - prv_a->tcTex.tY) * f_yac_invdx -
		//				(prv_c->tcTex.tY - prv_a->tcTex.tY) * f_yab_invdx;
		//
		pfmul		(m1,m2)								// f_yac_invdx | f_yab_invdx

		// stall

		movq		mm0,mm1								// copy f_yac_invdx | f_yab_invdx
		punpckhdq	mm1,mm1								// f_yac_invdx | f_yac_invdx
	
		pfmul		(m3,m1)								// f_vab*f_yab_invdx | f_uab*f_yab_invdx
		punpckldq	mm0,mm0								// f_yab_invdx | f_yab_invdx

		pfmul		(m4,m0)								// f_vac*f_yab_invdx | f_uac*f_yab_invdx

		mov			eax,[b_update]

		pfsub		(m3,m4)								// f_dv | f_du
		movq		mm0,[fDU]							// fDV | fDU
	
		test		eax,eax
		jz			short COPY_UV

		//
		// SetMinAbs(fDU, f_du);
		// SetMinAbs(fDV, f_dv);
		//
		movq	mm4,[pf_abs_mask]					// Mask to get rid of sign bit
		movq	mm1,mm3								// Copy (f_dv | f_du)

		movq	mm2,mm0								// Copy (fDV | fDU)

		pand	mm0,mm4								// Mask off sign bits
		pand	mm1,mm4								// Mask off sign bits

		pfcmpge	(m0,m1)								// Set ones where we should copy mm3
		movq	mm5,mm4								// Copy abs mask

		pandn	mm4,mm2								// Sign bits of (fDV | fDU)
		pandn	mm5,mm3								// Sign bits of (f_dv | f_du)

		pand	mm3,mm0								// Zero parts of new values we don't want.
		pxor	mm4,mm5								// Xor of sign bits (one when sign differs)

		pandn	mm0,mm2								// Copy parts of old values we want.
		por		mm3,mm0								// Or them together

		psrad	mm4,32								// Extend sign bits into mask

		pandn	mm4,mm3								// Zero anything with a different sign.

		movq	mm3,mm4								// Put it back in mm3.

COPY_UV:
		//
		// fDU = f_du;
		// fDV = f_dv;
		//
		movq	[fDU],mm3							// Store updated values.
	}

	//__asm femms
	return true;

RETURN_FALSE:
	//__asm femms
	return false;
}


//*****************************************************************************************
//
inline bool GenericInitTriangleDataLinearGour
(
	CDrawPolygonBase*					poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	int									b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	Assert(prv_a);
	Assert(prv_b);
	Assert(prv_c);

	//
	// Calculate fInvDx, the partial derivative of x over the triangle, using the following
	// formula:
	//
	//		           1                       1
	//		fInvDx = ---- = ---------------------------------------
	//	              dx    (x2 - x1)(y3 - y1) - (x3 - x1)(y2 - y1)
	//
	// dx also happens to be -2 times the area of the triangle, so we can check for correct
	// orientation here as well.
	//
	float f_dx;

	__asm
	{
		//femms

		mov		edx,[prv_a]						// prv_a = edx

		mov		esi,[prv_b]						// prv_b = esi

		mov		edi,[prv_c]						// prv_c = edi

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		movq		mm0,[edx]SRenderCoord.v3Screen.tX	// prv_a->tY | tX

		movq		mm1,[esi]SRenderCoord.v3Screen.tX	// prv_b->tY | tX

		movq		mm2,[edi]SRenderCoord.v3Screen.tX	// prv_c->tY | tX

		pfsub		(m1,m0)								// b-a

		pfsub		(m2,m0)								// c-a
		movq		mm5,mm1								// save mm1

		// stall

		punpckhdq	mm1,mm2								// f_yac = cY-aY | f_yab = bY-aY
		punpckldq	mm2,mm5								// bX-aX | cX-aX

		pfmul		(m2,m1)								// bX-aX * cY-aY | cX-aX * bY-aY

		// stall

		punpckldq	mm5,mm2								// cX-aX * bY-aY | junk

		pfsub		(m2,m5)								// (bX-aX * cY-aY) - (cX-aX * bY-aY) | junk
		movd		mm5,[fMAX_NEG_AREA]					// Load value for comparison

		punpckhdq	mm2,mm2								// f_dx | f_dx

		movq		mm0,mm2								// copy f_dx

		pfcmpge		(m0,m5)								// Set to mm0 ones if (mm0 >= mm3)

		// stall

		movd		eax,mm0

		test		eax,eax								// if (f_dx >= fMAX_NEG_AREA)
		jnz			RETURN_FALSE

		//
		// Calculate coefficients to pass to template InitializeTriangleData functions.
		//
		// float f_invdx = 1.0f / f_dx;
		// float f_yab_invdx = f_yab * f_invdx;
		// float f_yac_invdx = f_yac * f_invdx;
		//
		pfrcp		(m0,m2)								// f_invdx = 1.0 / f_dx
		movq		mm6,[edx]SRenderVertex.tcTex.tX		// prv_a->tY | tX

		movd		[f_dx],mm2							// save f_dx
		movq		mm3,[esi]SRenderVertex.tcTex.tX		// prv_b->tY | tX

		pfrcpit1	(m2,m0)								// Newton-Raphson interation 1.
		movq		mm4,[edi]SRenderVertex.tcTex.tX		// prv_c->tY | tX

		pfsub		(m3,m6)								// f_vab | f_uab
		movd		mm5,[esi]SRenderVertex.cvIntensity	// prv_b->cvIntensity

		pfrcpit2	(m2,m0)								// Newton-Raphson interation 2.
		pfsub		(m4,m6)								// f_vac | f_uac

		movd		mm6,[edi]SRenderVertex.cvIntensity	// prv_c->cvIntensity

		pfmul		(m1,m2)								// f_yac_invdx | f_yab_invdx
		movd		mm2,[edx]SRenderVertex.cvIntensity	// prv_a->cvIntensity

		//
		// Get the step values for u and v with respect to x.
		//
		// float f_du = (prv_b->tcTex.tX - prv_a->tcTex.tX) * f_yac_invdx -
		//				(prv_c->tcTex.tX - prv_a->tcTex.tX) * f_yab_invdx;
		//
		// float f_dv = (prv_b->tcTex.tY - prv_a->tcTex.tY) * f_yac_invdx -
		//				(prv_c->tcTex.tY - prv_a->tcTex.tY) * f_yab_invdx;
		//
		//
		// Find the step value for intensity with respect to the horizontal axis.
		//
		// float f_di_invdx =	(prv_b->cvIntensity - prv_a->cvIntensity) * f_yac_invdx -
		//						(prv_c->cvIntensity - prv_a->cvIntensity) * f_yab_invdx;
		//
		punpckldq	mm6,mm5								// prv_b->cvIntensity | prv_c->cvIntensity

		movq		mm0,mm1								// copy f_yac_invdx | f_yab_invdx
		punpckhdq	mm1,mm1								// f_yac_invdx | f_yac_invdx
	
		pfmul		(m3,m1)								// f_vab*f_yab_invdx | f_uab*f_yab_invdx
		movq		mm1,mm0								// copy f_yac_invdx | f_yab_invdx

		punpckldq	mm0,mm0								// f_yab_invdx | f_yab_invdx
		punpckldq	mm2,mm2								// prv_a->cvIntensity | prv_a->cvIntensity

		pfmul		(m4,m0)								// f_vac*f_yab_invdx | f_uac*f_yab_invdx
		pfsub		(m6,m2)								// f_iab | f_iac

		pfmul		(m6,m1)								// f_iab * f_yac_invdx | f_iac * f_yab_invdx

		pfsub		(m3,m4)								// f_dv | f_du
	
		punpckldq	mm0,mm6								// f_iac * f_yab_invdx | junk

		pfsub		(m6,m0)								// f_di_invdx | junk
		mov			eax,[b_update]

		test		eax,eax
		movq		mm0,[fDU]							// fDV | fDU

		psrlq		mm6,32								// 0 | f_di_invdx
		jz			short COPY_UV
	
		//
		// SetMinAbs(fDU, f_du);
		// SetMinAbs(fDV, f_dv);
		//
		movq	mm4,[pf_abs_mask]					// Mask to get rid of sign bits
		movq	mm1,mm3								// Copy (f_dv | f_du)

		movq	mm2,mm0								// Copy (fDV | fDU)

		pand	mm0,mm4								// Mask off sign bits
		pand	mm1,mm4								// Mask off sign bits

		pfcmpge	(m0,m1)								// Set ones where we should copy mm3
		movq	mm5,mm4								// Copy abs mask

		pandn	mm4,mm2								// Sign bits of (fDV | fDU)
		pandn	mm5,mm3								// Sign bits of (f_dv | f_du)

		pand	mm3,mm0								// Zero parts of new values we don't want.
		pxor	mm4,mm5								// Xor of sign bits (one when sign differs)

		pandn	mm0,mm2								// Copy parts of old values we want.
		por		mm3,mm0								// Or them together

		psrad	mm4,32								// Extend sign bits into mask
		movq	mm7,[pf_abs_mask]					// Mask to get rid of sign bits

		pandn	mm4,mm3								// Zero anything with a different sign.
		movd	eax,mm6								// Need a copy of f_di_invdx

		movq	mm3,mm4								// Put it back in mm3.
		mov		ebx,[fDeltaXIntensity]				// Load fDeltaXIntensity

		//
		//	if (CIntFloat(f_di_invdx).bSign() != CIntFloat(fDeltaXIntensity).bSign())
		//	{
		//		fDeltaXIntensity = 0.0f;
		//		fxDeltaXIntensity = 0;
		//		goto SKIP_INTENSITY_UPDATE;
		//	}
		//
		// if (fabs(f_di_invdx) >= fabs(fDeltaXIntensity))
		//		goto SKIP_INTENSITY_UPDATE;
		//
		mov		ecx,eax									// copy f_di_invdx
		xor		eax,ebx									// xor sign bits

		and		eax,0x80000000							// test for sign bit.
		jz		COMPARE_INTENSITY						// same sign, compare abs. values

		mov		eax,0
		mov		[fDeltaXIntensity],eax

		mov		[fxDeltaXIntensity],eax
		jmp		SKIP_INTENSITY_UPDATE

COMPARE_INTENSITY:
		and		ecx,0x7fffffff
		and		ebx,0x7fffffff

		cmp		ecx,ebx
		jge		SKIP_INTENSITY_UPDATE

COPY_UV:
		//
		// fDeltaXIntensity = f_di_invdx;
		//
		// Clamp(fDeltaXIntensity, 16.0f);
		//
		// Convert to fixed point representation.
		//
		// fxDeltaXIntensity.fxFromFloat(fDeltaXIntensity);
		//
		movd	mm1,[fSIXTEEN]					// 0 | 16.0

		movq	mm0,[pfFixed16Scale]			// 2^16 | 2^16

		pfmin	(m6,m1)							// Clamp to 16.0

		movd	[fDeltaXIntensity],mm6			// Save updated, possibly clamped value.
		pfmul	(m6,m0)							// Scale by 2^16

		pf2id	(m6,m6)

		movd	[fxDeltaXIntensity],mm6			// Save fixed point value.

SKIP_INTENSITY_UPDATE:
		//
		// fDU = f_du;
		// fDV = f_dv;
		//
		movq	[fDU],mm3							// Store updated values.
	}

	//__asm femms
	return true;

RETURN_FALSE:
	//__asm femms
	return false;
}


//*****************************************************************************************
//
template <class TScanlineType> void GenericInitializeForWalk
(
	CDrawPolygon<TScanlineType>* pdtri, 
	CEdge<TScanlineType>* pedge
)
//
// Specialized version of Intialization of an edge for walking.
//
//**************************************
{
	typedef CDrawPolygon<TScanlineType> aCDrawPolygon;
	typedef CEdge<TScanlineType> aCEdge;
	typedef aCEdge::TScanline aScanline;

	Assert(pedge);
	Assert(pedge->prvFrom);
	Assert(pedge->prvTo);
	Assert(pdtri);

#if bPRIM_STATS
	psBaseEdges.Add(0, 1);
	psLines.Add(0, pedge->prvTo->iYScr - pedge->prvFrom->iYScr);
#endif

	// Set base edge.
	pdtri->pedgeBase = pedge;

	__asm
	{
		//femms

		mov		edi,[pdtri]

		mov		esi,[pedge]

		// Initialize edge chosen to be the base edge.
		// pedge->lineStart.InitializeAsBase(pedge, poly);

		//
		// Get the fractional portions of the starting x difference and the increment x
		// difference. See the 'Notes' section of this module for an explanation of this
		// implementation.
		//
		// pedge->lineStart.fxXDifference = fixed(1) - pedge->lineStart.fxX.fxFractional();
		// pedge->lineIncrement.fxXDifference = -pedge->lineIncrement.fxX.fxFractional();
		//
		mov		ecx,[esi]aCEdge.lineStart.fxX.i4Fx

		mov		edx,[esi]aCEdge.lineIncrement.fxX.i4Fx

		mov		eax,ecx										// Copy fixed value.
		sar		eax,16										// Extend sign to 16 bits.

		mov		ebx,edx										// Copy fixed value.
		and		eax,0xffff0000								// Mask integral part (sign).

		sar		ebx,16										// Extend sign to 16 bits.
		and		ecx,0xffff									// Mask fractional part.

		and		ebx,0xffff0000								// Mask integral part (sign).
		or		ecx,eax										// Combine sign bits with fraction.

		and		edx,0xffff									// Mask fractional part.
		mov		eax,0x10000									// Fixed point one.

		sub		eax,ecx										// fixed(1) - fraction part of fxX
		mov		[esi]aCEdge.lineStart.fxXDifference,eax

		or		edx,ebx										// Combine sign bits with fraction.

		neg		edx											// Negate edx.
		mov		[esi]aCEdge.lineIncrement.fxXDifference,edx

		//
		// Convert x difference and increment to floating point.
		// pedge->lineStart.fXDifference = (float)pedge->lineStart.fxXDifference;
		// pedge->lineIncrement.fXDifference = (float)pedge->lineIncrement.fxXDifference;
		//
		movd		mm0,[esi]aCEdge.lineStart.fxXDifference

		movd		mm1,[esi]aCEdge.lineIncrement.fxXDifference

		movd		mm2,[fInverseTwoToTheSixteenth]				// 1.0 / 65536.0

		pi2fd		(m0,m0)										// Convert to float
		mov			eax,[esi]aCEdge.prvFrom

		pi2fd		(m1,m1)										// Convert to float
		mov			ebx,[esi]aCEdge.prvTo

		pfmul		(m0,m2)										// Scale by 1 / 2^16
		movd		mm4,[esi]aCEdge.fInvDY						// f_inv_dy

		pfmul		(m1,m2)										// Scale by 1 / 2^16
		movq		mm5,[eax]SRenderVertex.tcTex.tX				// prv_from->tcTex.tY | tX

		movd		[esi]aCEdge.lineStart.fXDifference,mm0		// Save f_x_diff_edge
		movq		mm2,[ebx]SRenderVertex.tcTex.tX				// prv_to->tcTex.tY | tX

		movd		[esi]aCEdge.lineIncrement.fXDifference,mm1	// Save f_x_diff_increment
		punpckldq	mm4,mm4										// f_inv_dy | f_inv_dy

		//
		// Use the setup routine for indexed coordinates.
		//
		// SRenderVertex* prv_from = pedge->prvFrom;
		// SRenderVertex* prv_to = pedge->prvTo;
		// float f_inv_dy = pedge->fInvDY;
		// float f_x_diff_increment = pedge->lineIncrement.fXDifference;
		// float f_x_diff_edge = pedge->lineStart.fXDifference;
		// float f_y_diff_edge = pedge->fStartYDiff;
		//

		//
		// Find the u and v increment values across x. The value is calculated from:
		//
		//		              du    (u1 - u0)
		//	   u increment = ---- = ---------
		//		              dy    (y1 - y0)
		//
		// with a similar formula for v.
		//
		// float f_increment_u = f_inv_dy * (prv_to->tcTex.tX - prv_from->tcTex.tX);
		// float f_increment_v = f_inv_dy * (prv_to->tcTex.tY - prv_from->tcTex.tY);
		// float f_increment_z = f_inv_dy * (prv_to->v3Screen.tZ - prv_from->v3Screen.tZ);
		//
		pfsub		(m2,m5)								// prv_to - prv_from
		movd		mm6,[eax]SRenderCoord.v3Screen.tZ	// 0 | prv_to->v3Screen.tZ

		punpckldq	mm0,mm0								// f_x_diff_edge | f_x_diff_edge
		punpckldq	mm1,mm1								// f_x_diff_increment | f_x_diff_increment

		pfmul		(m2,m4)								// f_increment_y | f_increment_x
		movd		mm3,[ebx]SRenderCoord.v3Screen.tZ	// 0 | prv_to->v3Screen.tZ 

		movd		mm5,[fDInvZ]						// fDInvZ

		pfsub		(m3,m6)								// prv_to - prv_from

		// stall

		pfmul		(m3,m4)								// 0 | f_increment_z
		movq		mm4,[fDUInvZ]						// fDVInvZ | fDUInvZ

		//
		// Set up the increments for edge walking. Note that the increment of the x difference
		// must be incorporated into this value.
		//
		// pedge->lineIncrement.indCoord.fUInvZ = f_increment_u + f_x_diff_increment * fDUInvZ;
		// pedge->lineIncrement.indCoord.fVInvZ = f_increment_v + f_x_diff_increment * fDVInvZ;
		// pedge->lineIncrement.indCoord.fInvZ  = f_increment_z + f_x_diff_increment * fDInvZ;
		//
		movq		mm7,mm5							// fDInvZ
		movq		mm6,mm4							// fDVInvZ | fDUInvZ

		pfmul		(m7,m1)							// f_x_diff_increment * fDZInvZ;

		pfmul		(m6,m1)							// f_x_diff_increment * fD[U,V]InvZ;

		pfadd		(m7,m3)							// pedge->lineIncrement.indCoord.fInvZ

		pfadd		(m6,m2)							// pedge->lineIncrement.indCoord.f[U,V]InvZ

		movd		[esi]aCEdge.lineIncrement.indCoord.fInvZ,mm7

		movq		[esi]aCEdge.lineIncrement.indCoord.fUInvZ,mm6

		//
		// Adjust u and v to the value they would be when the edge first intersects the first
		// horizontal line, and add the starting adjust values with respect to x. 
		//
		// pedge->lineStart.indCoord.fUInvZ = prv_from->tcTex.tX + f_increment_u * f_y_diff_edge + f_x_diff_edge * fDUInvZEdge;
		// pedge->lineStart.indCoord.fVInvZ = prv_from->tcTex.tY + f_increment_v * f_y_diff_edge + f_x_diff_edge * fDVInvZEdge;
		// pedge->lineStart.indCoord.fInvZ  = prv_from->v3Screen.tZ + f_increment_z * f_y_diff_edge + f_x_diff_edge * fDInvZEdge;
		//
		movd		mm6,[esi]aCEdge.fStartYDiff			// f_y_diff_edge

		// stall

		punpckldq	mm6,mm6								// f_y_diff_edge | f_y_diff_edge

		pfmul		(m2,m6)								// f_increment_[u,v] * f_y_diff_edge
		movq		mm7,mm5								// fDInvZ

		pfmul		(m3,m6)								// f_increment_z * f_y_diff_edge
		movq		mm6,mm4								// fDVInvZ | fDUInvZ

		pfmul		(m7,m0)								// f_x_diff_edge * fDZInvZEdge

		pfmul		(m6,m0)								// f_x_diff_edge * fD[U,V]InvZEdge

		pfadd		(m3,m7)								// + f_increment_z * f_y_diff_edge
		movd		mm7,[eax]SRenderCoord.v3Screen.tZ	// prv_from->v3Screen.tZ

		pfadd		(m2,m6)								// + f_increment_[u,v] * f_y_diff_edge
		movq		mm6,[eax]SRenderVertex.tcTex.tX		// prv_from->tcTex.tY | prv_from->tcTex.tX

		pfadd		(m3,m7)								// pedge->lineStart.indCoord.fInvZ

		pfadd		(m2,m6)										// pedge->lineStart.indCoord.f[U,V]InvZ
		movd		[esi]aCEdge.lineStart.indCoord.fInvZ,mm3	// Save

		movq		[esi]aCEdge.lineStart.indCoord.fUInvZ,mm2	// Save

		//
		// If the negate modulus subraction sign is set, negate the modulus subraction sign
		// for all related variables.
		//
		// if (f_x_diff_increment >= 0.0f)
		// {
			// -pedge->lineStart.indCoord;
			// fNegUInvZ = -fDUInvZ;
			// fNegVInvZ = -fDVInvZ;
			// fNegInvZ  = -fDInvZ;
		// }
		// else
		// {
			// fNegUInvZ = fDUInvZ;
			// fNegVInvZ = fDVInvZ;
			// fNegInvZ  = fDInvZ;
		// }
		//
		// float f_subdivide_len = float(iSubdivideLen);
		//
		// Slopes times subdivision length.
		// fDUInvZEdge = fDUInvZ * f_subdivide_len;
		// fDVInvZEdge = fDVInvZ * f_subdivide_len;
		// fDInvZEdge  = fDInvZ * f_subdivide_len;
		//
		movq		mm0,[qFpSignBits]				// 0x80000000 | 0x80000000
		movq		mm6,mm4							// fDVInvZEdge | fDUInvZEdge

		movd		mm3,iSubdivideLen				// iSubdivideLen
		movq		mm7,mm5							// fDInvZEdge

		punpckldq	mm3,mm3							// iSubdivideLen | iSubdivideLen
		pand		mm1,mm0							// Only leave sign bit of f_x_diff_incrment

		pi2fd		(m3,m3)							// f_subdivide_len | f_subdivide_len
		pxor		mm1,mm0							// Negate sign bit of f_x_diff_incrment

		pxor		mm6,mm1							// May negate sign bit of fDVInvZ,fDUInvZ
		movq		[fNegUInvZ],mm6					// Save

		pxor		mm7,mm1							// May negate sign bit of fDInvZ
		movd		[fNegInvZ],mm7					// Save

		pfmul		(m4,m3)							// fDVInvZ * f_subdivide_len | fDUInvZ * f_subdivide_len

		pfmul		(m5,m3)							// fDInvZ * f_subdivide_len
		movq		[fDUInvZEdge],mm4				// Save

		movd		[fDInvZEdge],mm5				// Save

		//femms
	}
}


//*****************************************************************************************
//
template <class TScanlineType> void GenericInitializeForWalkGour
(
	CDrawPolygon<TScanlineType>* pdtri, 
	CEdge<TScanlineType>* pedge
)
//
// Specialized version of Intialization of an edge for walking.
//
//**************************************
{
	typedef CDrawPolygon<TScanlineType> aCDrawPolygon;
	typedef CEdge<TScanlineType> aCEdge;
	typedef aCEdge::TScanline aScanline;

	Assert(pedge);
	Assert(pedge->prvFrom);
	Assert(pedge->prvTo);
	Assert(pdtri);

	Assert((int)&((TTexturePerspGour *)0)->gourIntensity.fxIntensity - (int)&((TTexturePerspGour *)0)->gourIntensity == 0);

#if bPRIM_STATS
	psBaseEdges.Add(0, 1);
	psLines.Add(0, pedge->prvTo->iYScr - pedge->prvFrom->iYScr);
#endif

	// Set base edge.
	pdtri->pedgeBase = pedge;

	__asm
	{
		//femms

		mov		edi,[pdtri]

		mov		esi,[pedge]

		// Initialize edge chosen to be the base edge.
		// pedge->lineStart.InitializeAsBase(pedge, poly);

		//
		// Get the fractional portions of the starting x difference and the increment x
		// difference. See the 'Notes' section of this module for an explanation of this
		// implementation.
		//
		// pedge->lineStart.fxXDifference = fixed(1) - pedge->lineStart.fxX.fxFractional();
		// pedge->lineIncrement.fxXDifference = -pedge->lineIncrement.fxX.fxFractional();
		//
		mov		ecx,[esi]aCEdge.lineStart.fxX.i4Fx

		mov		edx,[esi]aCEdge.lineIncrement.fxX.i4Fx

		mov		eax,ecx										// Copy fixed value.
		sar		eax,16										// Extend sign to 16 bits.

		mov		ebx,edx										// Copy fixed value.
		and		eax,0xffff0000								// Mask integral part (sign).

		sar		ebx,16										// Extend sign to 16 bits.
		and		ecx,0xffff									// Mask fractional part.

		and		ebx,0xffff0000								// Mask integral part (sign).
		or		ecx,eax										// Combine sign bits with fraction.

		and		edx,0xffff									// Mask fractional part.
		mov		eax,0x10000									// Fixed point one.

		sub		eax,ecx										// fixed(1) - fraction part of fxX
		mov		[esi]aCEdge.lineStart.fxXDifference,eax

		or		edx,ebx										// Combine sign bits with fraction.

		neg		edx											// Negate edx.
		mov		[esi]aCEdge.lineIncrement.fxXDifference,edx

		//
		// Convert x difference and increment to floating point.
		// pedge->lineStart.fXDifference = (float)pedge->lineStart.fxXDifference;
		// pedge->lineIncrement.fXDifference = (float)pedge->lineIncrement.fxXDifference;
		//
		movd		mm0,[esi]aCEdge.lineStart.fxXDifference

		movd		mm1,[esi]aCEdge.lineIncrement.fxXDifference

		movd		mm2,[fInverseTwoToTheSixteenth]				// 1.0 / 65536.0

		pi2fd		(m0,m0)										// Convert to float
		mov			eax,[esi]aCEdge.prvFrom

		pi2fd		(m1,m1)										// Convert to float
		mov			ebx,[esi]aCEdge.prvTo

		pfmul		(m0,m2)										// Scale by 1 / 2^16
		movd		mm4,[esi]aCEdge.fInvDY						// f_inv_dy

		pfmul		(m1,m2)										// Scale by 1 / 2^16
		movq		mm5,[eax]SRenderVertex.tcTex.tX				// prv_from->tcTex.tY | tX

		movd		[esi]aCEdge.lineStart.fXDifference,mm0		// Save f_x_diff_edge
		movq		mm2,[ebx]SRenderVertex.tcTex.tX				// prv_to->tcTex.tY | tX

		movd		[esi]aCEdge.lineIncrement.fXDifference,mm1	// Save f_x_diff_increment
		movd		mm6,[eax]SRenderCoord.v3Screen.tZ			// 0 | prv_from->v3Screen.tZ

		//
		// Use the setup routine for indexed coordinates.
		// Use the setup routine for Gouraud-shaded base edges.
		//
		// SRenderVertex* prv_from = pedge->prvFrom;
		// SRenderVertex* prv_to = pedge->prvTo;
		// float f_inv_dy = pedge->fInvDY;
		// float f_x_diff_increment = pedge->lineIncrement.fXDifference;
		// float f_x_diff_edge = pedge->lineStart.fXDifference;
		// float f_y_diff_edge = pedge->fStartYDiff;
		//

		//
		// Find the u and v increment values across x. The value is calculated from:
		//
		//		              du    (u1 - u0)
		//	   u increment = ---- = ---------
		//		              dy    (y1 - y0)
		//
		// with a similar formula for v.
		//
		// float f_increment_u = f_inv_dy * (prv_to->tcTex.tX - prv_from->tcTex.tX);
		// float f_increment_v = f_inv_dy * (prv_to->tcTex.tY - prv_from->tcTex.tY);
		// float f_increment_z = f_inv_dy * (prv_to->v3Screen.tZ - prv_from->v3Screen.tZ);
		// float f_increment_i = f_inv_dy * (prv_to->cvIntensity - prv_from->cvIntensity);
		//
		pfsub		(m2,m5)									// prv_to - prv_from
		punpckldq	mm4,mm4									// f_inv_dy | f_inv_dy

		punpckldq	mm6,[eax]SRenderVertex.cvIntensity		// prv_from->cvIntensity | prv_from->v3Screen.tZ
		punpckldq	mm0,mm0									// f_x_diff_edge | f_x_diff_edge

		movd		mm3,[ebx]SRenderCoord.v3Screen.tZ		// 0 | prv_to->v3Screen.tZ 
		punpckldq	mm1,mm1									// f_x_diff_increment | f_x_diff_increment

		punpckldq	mm3,[ebx]SRenderVertex.cvIntensity		// prv_to->cvIntensity | prv_to->v3Screen.tZ
		pfmul		(m2,m4)									// f_increment_y | f_increment_x

		movd		mm5,[fDInvZ]							// fDInvZ

		// stall

		pfsub		(m3,m6)									// prv_to - prv_from

		punpckldq	mm5,[fDeltaXIntensity]					// fDeltaXIntensity | fDInvZ

		pfmul		(m3,m4)									// f_increment_i | f_increment_z
		movq		mm4,[fDUInvZ]							// fDVInvZ | fDUInvZ

		//
		// Set up the increments for edge walking. Note that the increment of the x difference
		// must be incorporated into this value.
		//
		// pedge->lineIncrement.indCoord.fUInvZ = f_increment_u + f_x_diff_increment * fDUInvZ;
		// pedge->lineIncrement.indCoord.fVInvZ = f_increment_v + f_x_diff_increment * fDVInvZ;
		// pedge->lineIncrement.indCoord.fInvZ  = f_increment_z + f_x_diff_increment * fDInvZ;
		// pedge->lineIncrement.gourIntensity.fxIntensity = fixed(f_increment_i + f_x_diff_increment * fDeltaXIntensity);
		//
		movq		mm7,mm5									// fDeltaXIntensity | fDInvZ
		movq		mm6,mm4									// fDVInvZ | fDUInvZ

		pfmul		(m7,m1)									// f_x_diff_increment * [fDZInvZ,fDeltaXIntensity];

		pfmul		(m6,m1)									// f_x_diff_increment * fD[U,V]InvZ;

		pfadd		(m7,m3)									// fIntensity | pedge->lineIncrement.indCoord.fInvZ

		pfadd		(m6,m2)									// pedge->lineIncrement.indCoord.f[U,V]InvZ

		movd		[esi]aCEdge.lineIncrement.indCoord.fInvZ,mm7
		psrlq		mm7,32

		movq		[esi]aCEdge.lineIncrement.indCoord.fUInvZ,mm6
		movq		mm6,[pfFixed16Scale]

		// stall

		pfmul		(m7,m6)									// Scale by 2^16
		movd		mm6,[esi]aCEdge.fStartYDiff				// f_y_diff_edge

		// stall

		pf2id		(m7,m7)									// Convert to integer
		punpckldq	mm6,mm6									// f_y_diff_edge | f_y_diff_edge

		movd		[esi]aCEdge.lineIncrement.gourIntensity,mm7

		//
		// Adjust u and v to the value they would be when the edge first intersects the first
		// horizontal line, and add the starting adjust values with respect to x. 
		//
		// pedge->lineStart.indCoord.fUInvZ = prv_from->tcTex.tX + f_increment_u * f_y_diff_edge + f_x_diff_edge * fDUInvZEdge;
		// pedge->lineStart.indCoord.fVInvZ = prv_from->tcTex.tY + f_increment_v * f_y_diff_edge + f_x_diff_edge * fDVInvZEdge;
		// pedge->lineStart.indCoord.fInvZ  = prv_from->v3Screen.tZ + f_increment_z * f_y_diff_edge + f_x_diff_edge * fDInvZEdge;
		// pedge->lineStart.gourIntensity.fxIntensity = fixed(prv_from->cvIntensity + f_increment_i * f_y_diff_edge + f_x_diff_edge * fDeltaXIntensity);
		//
		pfmul		(m2,m6)									// f_increment_[u,v] * f_y_diff_edge
		movq		mm7,mm5									// fDeltaXIntensity | fDInvZ

		pfmul		(m3,m6)									// f_increment_[z,i] * f_y_diff_edge
		movq		mm6,mm4									// fDVInvZ | fDUInvZ

		pfmul		(m7,m0)									// f_x_diff_edge * [fDZInvZ,fDeltaXIntensity]

		pfmul		(m6,m0)									// f_x_diff_edge * fD[U,V]InvZ

		pfadd		(m3,m7)									// + f_increment_[z,i] * f_y_diff_edge
		movd		mm7,[eax]SRenderCoord.v3Screen.tZ		// prv_from->v3Screen.tZ

		pfadd		(m2,m6)									// + f_increment_[u,v] * f_y_diff_edge
		punpckldq	mm7,[eax]SRenderVertex.cvIntensity		// prv_from->cvIntensity | prv_from->v3Screen.tZ

		movq		mm6,[eax]SRenderVertex.tcTex.tX			// prv_from->tcTex.tY | prv_from->tcTex.tX

		movq		mm0,[pfFixed16Scale]					// Load fixed point scaling factor

		pfadd		(m3,m7)									// pedge->lineStart.indCoord.fInvZ

		pfadd		(m2,m6)										// pedge->lineStart.indCoord.f[U,V]InvZ
		movd		[esi]aCEdge.lineStart.indCoord.fInvZ,mm3	// Save

		pfmul		(m3,m0)										// Scale by 2^16
		movq		[esi]aCEdge.lineStart.indCoord.fUInvZ,mm2	// Save

		//
		// If the negate modulus subraction sign is set, negate the modulus subraction sign
		// for all related variables.
		//
		// if (f_x_diff_increment >= 0.0f)
		// {
			// -pedge->lineStart.indCoord;
			// fNegUInvZ = -fDUInvZ;
			// fNegVInvZ = -fDVInvZ;
			// fNegInvZ  = -fDInvZ;
			//
			// -gourIntensity;
			// fxIModDiv = -fxDeltaXIntensity;
		// }
		// else
		// {
			// fNegUInvZ = fDUInvZ;
			// fNegVInvZ = fDVInvZ;
			// fNegInvZ  = fDInvZ;
			// fxIModDiv = fxDeltaXIntensity;
		// }
		//
		// float f_subdivide_len = float(iSubdivideLen);
		//
		// Slopes times subdivision length.
		// fDUInvZEdge = fDUInvZ * f_subdivide_len;
		// fDVInvZEdge = fDVInvZ * f_subdivide_len;
		// fDInvZEdge  = fDInvZ * f_subdivide_len;
		//
		movq		mm0,[qFpSignBits]				// 0x80000000 | 0x80000000
		movq		mm6,mm4							// fDVInvZEdge | fDUInvZEdge

		pf2id		(m3,m3)							// Convert to integer

		movd		mm2,iSubdivideLen				// iSubdivideLen
		movq		mm7,mm5							// fDInvZEdge

		psrlq		mm3,32
		movd		[esi]aCEdge.lineStart.gourIntensity,mm3

		punpckldq	mm2,mm2							// iSubdivideLen | iSubdivideLen
		pand		mm1,mm0							// Only leave sign bit of f_x_diff_incrment

		pi2fd		(m2,m2)							// f_subdivide_len | f_subdivide_len
		pxor		mm1,mm0							// Negate sign bit of f_x_diff_incrment

		pxor		mm6,mm1							// May negate sign bit of fDVInvZ,fDUInvZ
		movq		[fNegUInvZ],mm6					// Save

		pxor		mm7,mm1							// May negate sign bit of fDInvZ
		movd		[fNegInvZ],mm7					// Save

		pfmul		(m4,m2)							// fDVInvZ * f_subdivide_len | fDUInvZ * f_subdivide_len
		mov			ecx,[esi]aCEdge.lineIncrement.fXDifference

		pfmul		(m5,m2)							// fDInvZ * f_subdivide_len
		movq		[fDUInvZEdge],mm4				// Save

		movd		[fDInvZEdge],mm5				// Save
		mov			edx,[fxDeltaXIntensity]

		test		ecx,0x80000000					// Test sign bit.
		jnz			STORE_IMODDIV

		neg			edx								// Negate fxIModDiv

STORE_IMODDIV:
		mov			[fxIModDiv],edx

		//femms
	}
}


//*****************************************************************************************
//
template <class TScanlineType> void GenericInitializeForWalkLinear
(
	CDrawPolygon<TScanlineType>* pdtri, 
	CEdge<TScanlineType>* pedge
)
//
// Specialized version of Intialization of an edge for walking.
//
//**************************************
{
	typedef CDrawPolygon<TScanlineType> aCDrawPolygon;
	typedef CEdge<TScanlineType> aCEdge;
	typedef aCEdge::TScanline aScanline;

	Assert(pedge);
	Assert(pedge->prvFrom);
	Assert(pedge->prvTo);
	Assert(pdtri);

#if bPRIM_STATS
	psBaseEdges.Add(0, 1);
	psLines.Add(0, pedge->prvTo->iYScr - pedge->prvFrom->iYScr);
#endif

	// Set base edge.
	pdtri->pedgeBase = pedge;

	__asm
	{
		//femms

		mov		edi,[pdtri]

		mov		esi,[pedge]

		// Initialize edge chosen to be the base edge.
		// pedge->lineStart.InitializeAsBase(pedge, poly);

		//
		// Get the fractional portions of the starting x difference and the increment x
		// difference. See the 'Notes' section of this module for an explanation of this
		// implementation.
		//
		// pedge->lineStart.fxXDifference = fixed(1) - pedge->lineStart.fxX.fxFractional();
		// pedge->lineIncrement.fxXDifference = -pedge->lineIncrement.fxX.fxFractional();
		//
		mov		ecx,[esi]aCEdge.lineStart.fxX.i4Fx

		mov		edx,[esi]aCEdge.lineIncrement.fxX.i4Fx

		mov		eax,ecx										// Copy fixed value.
		sar		eax,16										// Extend sign to 16 bits.

		mov		ebx,edx										// Copy fixed value.
		and		eax,0xffff0000								// Mask integral part (sign).

		sar		ebx,16										// Extend sign to 16 bits.
		and		ecx,0xffff									// Mask fractional part.

		and		ebx,0xffff0000								// Mask integral part (sign).
		or		ecx,eax										// Combine sign bits with fraction.

		and		edx,0xffff									// Mask fractional part.
		mov		eax,0x10000									// Fixed point one.

		sub		eax,ecx										// fixed(1) - fraction part of fxX
		mov		[esi]aCEdge.lineStart.fxXDifference,eax

		or		edx,ebx										// Combine sign bits with fraction.

		neg		edx											// Negate edx.
		mov		[esi]aCEdge.lineIncrement.fxXDifference,edx

		//
		// Convert x difference and increment to floating point.
		// pedge->lineStart.fXDifference = (float)pedge->lineStart.fxXDifference;
		// pedge->lineIncrement.fXDifference = (float)pedge->lineIncrement.fxXDifference;
		//
		movd		mm0,[esi]aCEdge.lineStart.fxXDifference

		movd		mm1,[esi]aCEdge.lineIncrement.fxXDifference

		movd		mm2,[fInverseTwoToTheSixteenth]				// 1.0 / 65536.0

		pi2fd		(m0,m0)										// Convert to float
		mov			eax,[esi]aCEdge.prvFrom

		pi2fd		(m1,m1)										// Convert to float
		mov			ebx,[esi]aCEdge.prvTo

		pfmul		(m0,m2)										// Scale by 1 / 2^16
		movd		mm4,[esi]aCEdge.fInvDY						// f_inv_dy

		pfmul		(m1,m2)										// Scale by 1 / 2^16
		movq		mm5,[eax]SRenderVertex.tcTex.tX				// prv_from->tcTex.tY | tX

		movd		[esi]aCEdge.lineStart.fXDifference,mm0		// Save f_x_diff_edge
		movq		mm2,[ebx]SRenderVertex.tcTex.tX				// prv_to->tcTex.tY | tX

		movd		[esi]aCEdge.lineIncrement.fXDifference,mm1	// Save f_x_diff_increment
		punpckldq	mm4,mm4										// f_inv_dy | f_inv_dy

		//
		// Use the setup routine for indexed coordinates.
		// Use the setup routine for Gouraud-shaded base edges.
		//
		// SRenderVertex* prv_from = pedge->prvFrom;
		// SRenderVertex* prv_to = pedge->prvTo;
		// float f_inv_dy = pedge->fInvDY;
		// float f_x_diff_increment = pedge->lineIncrement.fXDifference;
		// float f_x_diff_edge = pedge->lineStart.fXDifference;
		// float f_y_diff_edge = pedge->fStartYDiff;
		//

		//
		// Find the u and v increment values across x. The value is calculated from:
		//
		//		              du    (u1 - u0)
		//	   u increment = ---- = ---------
		//		              dy    (y1 - y0)
		//
		// with a similar formula for v.
		//
		// float f_increment_u = f_inv_dy * (prv_to->tcTex.tX - prv_from->tcTex.tX);
		// float f_increment_v = f_inv_dy * (prv_to->tcTex.tY - prv_from->tcTex.tY);
		//
		pfsub		(m2,m5)								// prv_to - prv_from
		movq		mm3,[pfFixed16Scale]				// Fixed point scaling factor

		punpckldq	mm0,mm0								// f_x_diff_edge | f_x_diff_edge
		movq		mm6,[fDU]							// fDV | fDU

		pfmul		(m2,m4)								// f_increment_v | f_increment_u
		punpckldq	mm1,mm1								// f_x_diff_increment | f_x_diff_increment

		//
		// Set up the increments for edge walking. Note that the increment of the x difference
		// must be incorporated into this value.
		//
		// pedge->lineIncrement.indCoord.pfIndex.U = (f_increment_u + f_x_diff_increment * fDU) * 65536.0f;
		// pedge->lineIncrement.indCoord.pfIndex.V = (f_increment_v + f_x_diff_increment * fDV) * 65536.0f;
		//
		//
		// Adjust u and v to the value they would be when the edge first intersects the first
		// horizontal line, and add the starting adjust values with respect to x. 
		//
		// pedge->lineStart.indCoord.pfIndex.U = (prv_from->tcTex.tX + f_increment_u * f_y_diff_edge + f_x_diff_edge * fDU) * 65536.0f;
		// pedge->lineStart.indCoord.pfIndex.V = (prv_from->tcTex.tY + f_increment_v * f_y_diff_edge + f_x_diff_edge * fDV) * 65536.0f;
		//
		movd		mm5,[esi]aCEdge.fStartYDiff		// f_y_diff_edge
		movq		mm4,mm6							// fDV | fDU

		pfmul		(m6,m1)							// f_x_diff_increment * fD[U,V];
		punpckldq	mm5,mm5							// f_y_diff_edge | f_y_diff_edge

		pfadd		(m6,m2)							// pedge->lineIncrement.indCoord.pfIndex.[U,V]
		pfmul		(m2,m5)							// f_increment_[u,v] * f_y_diff_edge

		movq		mm5,mm4							// fDV | fDU
		pfmul		(m4,m0)							// f_x_diff_edge * fD[U,V]

		pfmul		(m6,m3)							// multiply by 2^16
		movq		mm0,[eax]SRenderVertex.tcTex.tX

		pfadd		(m2,m4)							// + f_increment_[u,v] * f_y_diff_edge
	
		movq		[esi]aCEdge.lineIncrement.indCoord.pfIndex,mm6

		pfadd		(m2,m0)							// pedge->lineStart.indCoord.pfIndex.[U,V]
		movq		mm0,[qFpSignBits]

		pfmul		(m2,m3)							// multiply by 2^16

		//
		// If the negate modulus subraction sign is set, negate the modulus subraction sign
		// for all related variables.
		//
		// if (pedge->lineIncrement.fXDifference >= 0.0f)
		// {
		//     -pedge->lineStart.indCoord;
		//     pfNegD.U = -fDU * 65536.0f;
		//     pfNegD.V = -fDV * 65536.0f;
		// }
		// else
		// {
		//     pfNegD.U = fDU * 65536.0f;
		//     pfNegD.V = fDV * 65536.0f;
		// }
		//
		pfmul		(m3,m5)							// fDV * 65536.0f | fDU * 65536.0f
		pand		mm1,mm0							// Only leave sign bit of f_x_diff_incrment

		movq		[esi]aCEdge.lineStart.indCoord.pfIndex,mm2
		pxor		mm1,mm0							// Negate sign bit of f_x_diff_incrment

		pf2id		(m4,m3)
		pxor		mm3,mm1							// May negate sign bit of fDV,fDU

		movq		[pfNegD],mm3					// Save sub-pixel correction offset
		movq		[pfxDeltaTex],mm4				// Set pfxDeltaTex.[U,V]

		//femms
	}
}


//*****************************************************************************************
//
template <class TScanlineType> void GenericInitializeForWalkLinearGour
(
	CDrawPolygon<TScanlineType>* pdtri, 
	CEdge<TScanlineType>* pedge
)
//
// Specialized version of Intialization of an edge for walking.
//
//**************************************
{
	typedef CDrawPolygon<TScanlineType> aCDrawPolygon;
	typedef CEdge<TScanlineType> aCEdge;
	typedef aCEdge::TScanline aScanline;

	Assert(pedge);
	Assert(pedge->prvFrom);
	Assert(pedge->prvTo);
	Assert(pdtri);

#if bPRIM_STATS
	psBaseEdges.Add(0, 1);
	psLines.Add(0, pedge->prvTo->iYScr - pedge->prvFrom->iYScr);
#endif

	// Set base edge.
	pdtri->pedgeBase = pedge;

	__asm
	{
		//femms

		mov		edi,[pdtri]

		mov		esi,[pedge]

		// Initialize edge chosen to be the base edge.
		// pedge->lineStart.InitializeAsBase(pedge, poly);

		//
		// Get the fractional portions of the starting x difference and the increment x
		// difference. See the 'Notes' section of this module for an explanation of this
		// implementation.
		//
		// pedge->lineStart.fxXDifference = fixed(1) - pedge->lineStart.fxX.fxFractional();
		// pedge->lineIncrement.fxXDifference = -pedge->lineIncrement.fxX.fxFractional();
		//
		mov		ecx,[esi]aCEdge.lineStart.fxX.i4Fx

		mov		edx,[esi]aCEdge.lineIncrement.fxX.i4Fx

		mov		eax,ecx										// Copy fixed value.
		sar		eax,16										// Extend sign to 16 bits.

		mov		ebx,edx										// Copy fixed value.
		and		eax,0xffff0000								// Mask integral part (sign).

		sar		ebx,16										// Extend sign to 16 bits.
		and		ecx,0xffff									// Mask fractional part.

		and		ebx,0xffff0000								// Mask integral part (sign).
		or		ecx,eax										// Combine sign bits with fraction.

		and		edx,0xffff									// Mask fractional part.
		mov		eax,0x10000									// Fixed point one.

		sub		eax,ecx										// fixed(1) - fraction part of fxX
		mov		[esi]aCEdge.lineStart.fxXDifference,eax

		or		edx,ebx										// Combine sign bits with fraction.
		movd	mm2,[fInverseTwoToTheSixteenth]				// 1.0 / 65536.0

		neg		edx											// Negate edx.
		mov		[esi]aCEdge.lineIncrement.fxXDifference,edx

		//
		// Convert x difference and increment to floating point.
		// pedge->lineStart.fXDifference = (float)pedge->lineStart.fxXDifference;
		// pedge->lineIncrement.fXDifference = (float)pedge->lineIncrement.fxXDifference;
		//
		movd		mm0,[esi]aCEdge.lineStart.fxXDifference

		movd		mm1,[esi]aCEdge.lineIncrement.fxXDifference

		pi2fd		(m0,m0)										// Convert to float
		mov			eax,[esi]aCEdge.prvFrom

		pi2fd		(m1,m1)										// Convert to float
		mov			ebx,[esi]aCEdge.prvTo

		pfmul		(m0,m2)										// Scale by 1 / 2^16
		movd		mm4,[esi]aCEdge.fInvDY						// f_inv_dy

		pfmul		(m1,m2)										// Scale by 1 / 2^16
		movq		mm5,[eax]SRenderVertex.tcTex.tX				// prv_from->tcTex.tY | tX

		movd		[esi]aCEdge.lineStart.fXDifference,mm0		// Save f_x_diff_edge
		movq		mm2,[ebx]SRenderVertex.tcTex.tX				// prv_to->tcTex.tY | tX

		movd		[esi]aCEdge.lineIncrement.fXDifference,mm1	// Save f_x_diff_increment
		punpckldq	mm4,mm4										// f_inv_dy | f_inv_dy

		//
		// Use the setup routine for indexed coordinates.
		//
		// SRenderVertex* prv_from = pedge->prvFrom;
		// SRenderVertex* prv_to = pedge->prvTo;
		// float f_inv_dy = pedge->fInvDY;
		// float f_x_diff_increment = pedge->lineIncrement.fXDifference;
		// float f_x_diff_edge = pedge->lineStart.fXDifference;
		// float f_y_diff_edge = pedge->fStartYDiff;
		//

		//
		// Find the u and v increment values across x. The value is calculated from:
		//
		//		              du    (u1 - u0)
		//	   u increment = ---- = ---------
		//		              dy    (y1 - y0)
		//
		// with a similar formula for v.
		//
		// float f_increment_u = f_inv_dy * (prv_to->tcTex.tX - prv_from->tcTex.tX);
		// float f_increment_v = f_inv_dy * (prv_to->tcTex.tY - prv_from->tcTex.tY);
		// float f_increment_i = f_inv_dy * (prv_to->cvIntensity - prv_from->cvIntensity);
		//
		pfsub		(m2,m5)								// prv_to - prv_from
		movd		mm6,[eax]SRenderVertex.cvIntensity	// 0 | prv_from->cvIntenstiy

		punpckldq	mm0,mm0								// f_x_diff_edge | f_x_diff_edge
		movd		mm3,[ebx]SRenderVertex.cvIntensity	// 0 | prv_to->cvIntensity

		pfmul		(m2,m4)								// f_increment_y | f_increment_x
		punpckldq	mm1,mm1								// f_x_diff_increment | f_x_diff_increment

		movd		mm5,[fDeltaXIntensity]				// fDeltaXIntensity
		pfsub		(m3,m6)								// prv_to - prv_from

		movq		mm6,[fDU]							// fDV | fDU

		pfmul		(m3,m4)								// 0 | f_increment_i
		movq		mm7,[pfFixed16Scale]				// Fixed point scaling factor

		//
		// Set up the increments for edge walking. Note that the increment of the x difference
		// must be incorporated into this value.
		//
		// pedge->lineIncrement.indCoord.pfIndex.U = (f_increment_u + f_x_diff_increment * fDU) * 65536.0f;
		// pedge->lineIncrement.indCoord.pfIndex.V = (f_increment_v + f_x_diff_increment * fDV) * 65536.0f;
		// pedge->lineIncrement.gourIntensity.fxIntensity = fixed(f_increment_i + f_x_diff_increment * fDeltaXIntensity);
		//
		movq		mm4,mm6							// fDV | fDU
		pfmul		(m5,m1)							// f_x_diff_increment * fDeltaXIntensity

		pfmul		(m6,m1)							// f_x_diff_increment * fD[U,V]

		pfadd		(m5,m3)							// pedge->lineIncrement.gourIntensity.fxIntensity

		pfadd		(m6,m2)							// pedge->lineIncrement.indCoord.f[U,V]

		pfmul		(m5,m7)							// scale by 2^16

		pfmul		(m6,m7)							// scale by 2^16

		pf2id		(m5,m5)							// convert to integer.
		movq		[esi]aCEdge.lineIncrement.indCoord.pfIndex,mm6

		movd		[esi]aCEdge.lineIncrement.gourIntensity,mm5

		//
		// Adjust u and v to the value they would be when the edge first intersects the first
		// horizontal line, and add the starting adjust values with respect to x. 
		//
		// pedge->lineStart.indCoord.pfIndex.U = (prv_from->tcTex.tX + f_increment_u * f_y_diff_edge + f_x_diff_edge * fDU) * 65536.0f;
		// pedge->lineStart.indCoord.pfIndex.V = (prv_from->tcTex.tY + f_increment_v * f_y_diff_edge + f_x_diff_edge * fDV) * 65536.0f;
		// pedge->lineStart.gourIntensity.fxIntensity  = fixed(prv_from->cvIntensity + f_increment_i * f_y_diff_edge + f_x_diff_edge * fDeltaXIntensity);
		//
		movd		mm6,[esi]aCEdge.fStartYDiff			// f_y_diff_edge

		// stall

		punpckldq	mm6,mm6								// f_y_diff_edge | f_y_diff_edge

		pfmul		(m2,m6)								// f_increment_[u,v] * f_y_diff_edge
		movd		mm5,[fDeltaXIntensity]				// fDeltaXIntensity

		pfmul		(m3,m6)								// f_increment_i * f_y_diff_edge
		movq		mm6,mm4								// fDV | fDU

		pfmul		(m5,m0)								// f_x_diff_edge * fDeltaXIntensity

		pfmul		(m6,m0)								// f_x_diff_edge * fD[U,V]

		pfadd		(m3,m5)								// + f_increment_i * f_y_diff_edge
		movd		mm5,[eax]SRenderVertex.cvIntensity	// prv_from->cvIntensity

		pfadd		(m2,m6)								// + f_increment_[u,v] * f_y_diff_edge
		movq		mm6,[eax]SRenderVertex.tcTex.tX		// prv_from->tcTex.tY | prv_from->tcTex.tX

		pfadd		(m3,m5)								// pedge->lineStart.gourIntensity.fxIntensity
		movq		mm0,[qFpSignBits]

		pfadd		(m2,m6)								// pedge->lineStart.indCoord.f[U,V]
		pfmul		(m4,m7)								// fDV * 65536.0f | fDU * 65536.0f

		pfmul		(m3,m7)								// scale by 2^16
		pand		mm1,mm0								// Only leave sign bit of f_x_diff_incrment

		pfmul		(m2,m7)								// scale by 2^16
		pxor		mm1,mm0								// Negate sign bit of f_x_diff_incrment

		pf2id		(m3,m3)											// Convert to integer.
		movq		[esi]aCEdge.lineStart.indCoord.pfIndex,mm2		// Save

		movd		[esi]aCEdge.lineStart.gourIntensity,mm3			// Save
		pf2id		(m3,m4)											// Convert fDV,fDU to integer.

		//
		// If the negate modulus subraction sign is set, negate the modulus subraction sign
		// for all related variables.
		//
		// if (pedge->lineIncrement.fXDifference >= 0.0f)
		// {
		//     -pedge->lineStart.indCoord;
		//     pfNegD.U = -fDU * 65536.0f;
		//     pfNegD.V = -fDV * 65536.0f;
		//
		//     -gourIntensity;
		//     fxIModDiv = -fxDeltaXIntensity;
		// }
		// else
		// {
		//     pfNegD.U = fDU * 65536.0f;
		//     pfNegD.V = fDV * 65536.0f;
		//
		//     fxIModDiv = fxDeltaXIntensity;
		// }
		//
		pxor		mm4,mm1							// May negate sign bit of fDV,fDU
		movq		[pfNegD],mm4					// Save sub-pixel correction offset

		mov			ecx,[esi]aCEdge.lineIncrement.fXDifference
		movq		[pfxDeltaTex],mm3				// Set pfxDeltaTex.[U,V]

		mov			edx,[fxDeltaXIntensity]

		test		ecx,0x80000000					// Test sign bit.
		jnz			STORE_IMODDIV

		neg			edx								// Negate fxIModDiv

STORE_IMODDIV:
		mov			[fxIModDiv],edx

		//femms
	}
}


//*****************************************************************************************
//
inline bool GenericInitGradientDataPlanar(CDrawPolygonBase* poly, bool b_altpersp)
//
// Sets up polygon-wide rasterising info.
//
// Returns:
//		Whether the polygon is large enough to render.
//
//**************************************
{
	// These are saved for the best edge pair.
	float f_dx;
	SRenderVertex *prv_a;
	SRenderVertex *prv_b;
	SRenderVertex *prv_c;

	__asm
	{
		// femms

		//
		// Find the first edge pair with an area greater than 25 pixels, or if no 
		// edge pair has an area greater than 25 pixels find the maximum.
		//

		// Set starting vertices [N-2, N-1, 0].
		lea		edi,[arvRasterVertices]			// prv_c = edi = a[n]
		mov		ecx,[iNumRasterVertices]

		mov		eax,SIZE SRenderVertex
		dec		ecx

		imul	eax,ecx

		lea		edx,[edi + eax]					// prv_b = esi = a[n-1]

		mov		esi,edx
		sub		edx,SIZE SRenderVertex			// prv_a = edx = a[n-2]

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		movq		mm2,[edi]SRenderCoord.v3Screen.tX	// prv_c->tY | tX
		mov			[prv_c],edi

		movq		mm0,[edx]SRenderCoord.v3Screen.tX	// prv_a->tY | tX
		mov			[prv_b],esi

		movq		mm1,[esi]SRenderCoord.v3Screen.tX	// prv_b->tY | tX
		mov			[prv_a],edx

		pfsub		(m2,m0)								// yac | xac

		pfsub		(m1,m0)								// yab | xab
		movq		mm3,mm2								// yac | xac

		punpckldq	mm2,mm1								// xab | xac
		punpckhdq	mm1,mm3								// yac | yab

		pfmul		(m2,m1)								// xab*yac | xac*yab
		movq		mm7,mm1								// Save yac | yab

		movq		mm4,mm2								// xab*yac | xac*yab

		punpckhdq	mm4,mm4								// xab*yac | xab*yac

		pfsub		(m4,m2)								// f_dx = xab*yac - xac*yab
		mov			ebx,[fNegativeFifty]

		movd		eax,mm4								// f_dx

		cmp			eax,ebx								// If our best < -50, then stop
		ja			EXIT_LOOP

EDGE_LOOP:
		mov			edx,esi								// prv_a = prv_b
		mov			esi,edi								// prv_b = prv_c

		add			edi,SIZE SRenderVertex				// prv_c = next vertex

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		movq		mm2,[edi]SRenderCoord.v3Screen.tX	// prv_c->tY | tX

		movq		mm0,[edx]SRenderCoord.v3Screen.tX	// prv_a->tY | tX

		movq		mm1,[esi]SRenderCoord.v3Screen.tX	// prv_b->tY | tX

		pfsub		(m2,m0)								// yac | xac

		pfsub		(m1,m0)								// yab | xab
		movq		mm3,mm2								// yac | xac

		punpckldq	mm2,mm1								// xab | xac
		punpckhdq	mm1,mm3								// yac | yab

		pfmul		(m2,m1)								// xab*yac | xac*yab

		movq		mm0,mm2								// xab*yac | xac*yab

		punpckhdq	mm0,mm0								// xab*yac | xab*yac

		pfsub		(m0,m2)								// f_dx = xab*yac - xac*yab

		movq		mm2,mm0								// Make a copy of f_dx
		pfcmpge		(m0,m4)								// Set mm0 to ones if (mm0 >= mm4)

		// stall

		movd		eax,mm0

		test		eax,eax								// if mm1 < f_dx then new minimum
		jz			NEW_MINIMUM

		dec			ecx
		jnz			EDGE_LOOP

		jmp			short EXIT_LOOP

NEW_MINIMUM:
		// Yes, save f_dx in mm4, yac|yab in mm7 and vertex pointers.
		movd		eax,mm2								// f_dx

		movq		mm4,mm2								// New f_dx
		movq		mm7,mm1								// Save yac | yab

		mov			[prv_a],edx

		mov			[prv_b],esi
		cmp			eax,ebx								// If our best < -50, then stop

		mov			[prv_c],edi
		ja			EXIT_LOOP

		dec			ecx
		jnz			EDGE_LOOP

EXIT_LOOP:
		//
		// Is the polygon too small to draw?
		// if (f_dx >= fMAX_NEG_AREA) goto RETURN_FALSE;
		//
		// mm4 = f_dx
		// mm7 = yac | yab
		//
		mov			ebx,[fMAX_NEG_AREA]

		movd		eax,mm4

		movd		[f_dx],mm4
		punpckldq	mm4,mm4

		cmp			eax,ebx
		jbe			RETURN_FALSE

		//
		// Initialize the texture gradients based on the best edge.
		//
		// float f_invdx = 1.0f / f_dx;
		// float f_yab_invdx = f_yab * f_invdx;
		// float f_yac_invdx = f_yac * f_invdx;
		//
		mov			edx,[prv_a]
		pfrcp		(m0,m4)								// f_invdx = 1.0 / f_dx

		mov			esi,[prv_b]

		mov			edi,[prv_c]
		pfrcpit1	(m4,m0)								// Newton-Raphson interation 1.

		movq		mm6,[edx]SRenderVertex.tcTex.tX		// prv_a->tY | tX

		movq		mm3,[esi]SRenderVertex.tcTex.tX		// prv_b->tY | tX
		pfrcpit2	(m4,m0)								// Newton-Raphson interation 2.

		movq		mm2,[edi]SRenderVertex.tcTex.tX		// prv_c->tY | tX

		pfsub		(m3,m6)								// f_vab | f_uab
		movd		mm5,[esi]SRenderCoord.v3Screen.tZ	// prv_b->tZ

		pfsub		(m2,m6)								// f_vac | f_uac

		movd		mm6,[edi]SRenderCoord.v3Screen.tZ	// prv_c->tZ

		pfmul		(m7,m4)								// f_yac_invdx | f_yab_invdx
		movd		mm4,[edx]SRenderCoord.v3Screen.tZ	// prv_a->tZ

		//
		// Get the step values for u and v with respect to x.
		//
		// float f_duinvz = (prv_b->tcTex.tX - prv_a->tcTex.tX) * f_yac_invdx -
		//				    (prv_c->tcTex.tX - prv_a->tcTex.tX) * f_yab_invdx;
		//
		// float f_dvinvz = (prv_b->tcTex.tY - prv_a->tcTex.tY) * f_yac_invdx -
		//				    (prv_c->tcTex.tY - prv_a->tcTex.tY) * f_yab_invdx;
		//
		// float f_dinvz =  (prv_b->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yac_invdx -
		//				    (prv_c->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yab_invdx;
		//
		punpckldq	mm6,mm5								// prv_b->tZ | prv_c->tZ

		movq		mm0,mm7								// copy f_yac_invdx | f_yab_invdx
		punpckhdq	mm7,mm7								// f_yac_invdx | f_yac_invdx
	
		pfmul		(m3,m7)								// f_vab*f_yab_invdx | f_uab*f_yab_invdx
		movq		mm7,mm0								// copy f_yac_invdx | f_yab_invdx

		punpckldq	mm0,mm0								// f_yab_invdx | f_yab_invdx
		punpckldq	mm4,mm4								// prv_a->tZ | prv_a->tZ

		pfmul		(m2,m0)								// f_vac*f_yab_invdx | f_uac*f_yab_invdx
		pfsub		(m6,m4)								// f_zab | f_zac

		pfmul		(m6,m7)								// f_zab * f_yac_invdx | f_zac * f_yab_invdx

		pfsub		(m3,m2)								// f_dvinvz | f_duinvz
	
		punpckldq	mm0,mm6								// f_zac * f_yab_invdx | junk

		pfsub		(m6,m0)								// f_dinvz | junk

		psrlq		mm6,32								// 0 | f_dinvz

		movq		[fDUInvZ],mm3						// Store updated values.

		movd		[fDInvZ],mm6
	}

	// Calculate the subdivision length with respect to X.
	iSubdivideLen    = iGetSubdivisionLen_3DX(fDInvZ, b_altpersp);
	fInvSubdivideLen = fInverseIntTable[iSubdivideLen];

	//__asm femms
	return true;

RETURN_FALSE:
	//__asm femms
	return false;
}


#else // #if (TARGET_PROCESSOR == PROCESSOR_K6_3D)


//*****************************************************************************************
//
inline void GenericInitializePolygonData(CRenderPolygon *rpoly)
//
// Scanline data initialization for un-shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	u4TextureTileMask = (pras_texture->u4HeightTileMask << 9) | pras_texture->u4WidthTileMask;
	u4TextureTileMaskStepU = pras_texture->u4WidthTileMask;

	__asm
	{
		mov		ebx,bClampUV
		mov		eax,pras_texture					// Pointer to texture.

		test	ebx,ebx
		jz		NO_CLAMP

		//
		// Copy vertices & scale texture values with clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX *
		//								 fTexWidth  + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//								 fTexHeight + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fxch	st(1)								// (tX,tY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tX.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tX,tY)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(1)								// (tX,iY,tY)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tX*Z,iY,tY)
		fxch	st(2)								// (tY,iY,tX*Z)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tY.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tY*Z,iY,tX*Z)
		fxch	st(2)								// (tX*Z,iY,tY*Z)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.
		fstp	d_temp_a							// Store 19.32 integer y value.
		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		ebx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.

		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z
		and		ebx,0x0007ffff							// Mask off FP junk.

		mov		[edi]SRenderCoord.iYScr,ebx				// Store integer Y.
		add		edi,SIZE SRenderVertex					// Step destination.
		
		dec		edx
		jnz		VTXLOOP1

		jmp		VTXDONE

NO_CLAMP:
		//
		// Copy vertices & scale texture values, no clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tY,tX)
		fxch	st(2)								// (tX,tY,iY)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tX*Z,tY,iY)
		fxch	st(2)								// (iY,tY,tX*Z)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(1)								// (tY,iY,tX*Z)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tY*Z,iY,tX*Z)

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z

		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.

		mov		ecx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.
		add		edi,SIZE SRenderVertex					// Step destination.

		and		ecx,0x0007ffff							// Mask off FP junk.
		dec		edx

		mov		[edi - SIZE SRenderVertex]SRenderCoord.iYScr,ecx	// Store integer Y.
		jnz		VTXLOOP2

VTXDONE:
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataFlat(CRenderPolygon *rpoly)
//
// Scanline data initialization for flat shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	u4TextureTileMask = (pras_texture->u4HeightTileMask << 9) | pras_texture->u4WidthTileMask;
	u4TextureTileMaskStepU = pras_texture->u4WidthTileMask;

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = iPosFloatCast(rpoly->cvFace);
	u4ConstColour = 0;

#if VER_DEBUG
	iNumIntensities = 0;
	if (rpoly->ptexTexture && rpoly->ptexTexture->ppcePalClut && rpoly->ptexTexture->ppcePalClut->pclutClut)
	{
		iNumIntensities = rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues;
		Assert(iBaseIntensity < rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues);
	}
#endif

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapTexture<uint8>::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		mov		ebx,bClampUV
		mov		eax,pras_texture					// Pointer to texture.

		test	ebx,ebx
		jz		NO_CLAMP

		//
		// Copy vertices & scale texture values with clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX *
		//								 fTexWidth  + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//								 fTexHeight + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fxch	st(1)								// (tX,tY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tX.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tX,tY)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(1)								// (tX,iY,tY)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tX*Z,iY,tY)
		fxch	st(2)								// (tY,iY,tX*Z)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tY.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tY*Z,iY,tX*Z)
		fxch	st(2)								// (tX*Z,iY,tY*Z)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.
		fstp	d_temp_a							// Store 19.32 integer y value.
		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		ebx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.

		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z
		and		ebx,0x0007ffff							// Mask off FP junk.

		mov		[edi]SRenderCoord.iYScr,ebx				// Store integer Y.
		add		edi,SIZE SRenderVertex					// Step destination.
		
		dec		edx
		jnz		VTXLOOP1

		jmp		VTXDONE

NO_CLAMP:
		//
		// Copy vertices & scale texture values, no clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tY,tX)
		fxch	st(2)								// (tX,tY,iY)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tX*Z,tY,iY)
		fxch	st(2)								// (iY,tY,tX*Z)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(1)								// (tY,iY,tX*Z)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tY*Z,iY,tX*Z)

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z

		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.

		mov		ecx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.
		add		edi,SIZE SRenderVertex					// Step destination.

		and		ecx,0x0007ffff							// Mask off FP junk.
		dec		edx

		mov		[edi - SIZE SRenderVertex]SRenderCoord.iYScr,ecx	// Store integer Y.
		jnz		VTXLOOP2

VTXDONE:
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataGour(CRenderPolygon *rpoly)
//
// Scanline data initialization for gouraud shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	u4TextureTileMask = (pras_texture->u4HeightTileMask << 9) | pras_texture->u4WidthTileMask;
	u4TextureTileMaskStepU = pras_texture->u4WidthTileMask;

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = 0;
	u4ConstColour = 0;

#if VER_DEBUG
	iNumIntensities = rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues;
#endif

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapTexture<uint8>::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		mov		ebx,bClampUV
		mov		eax,pras_texture					// Pointer to texture.

		test	ebx,ebx
		jz		NO_CLAMP

		//
		// Copy vertices & scale texture values with clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX *
		//								 fTexWidth  + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//								 fTexHeight + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fxch	st(1)								// (tX,tY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tX.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tX,tY)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(1)								// (tX,iY,tY)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tX*Z,iY,tY)
		fxch	st(2)								// (tY,iY,tX*Z)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tY.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tY*Z,iY,tX*Z)
		fxch	st(2)								// (tX*Z,iY,tY*Z)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.
		fstp	d_temp_a							// Store 19.32 integer y value.
		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z

		mov		ecx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.
		mov		ebx,[eax]SRenderVertex.cvIntensity		// Load intenstity.

		and		ecx,0x0007ffff							// Mask off FP junk.
		mov		[edi]SRenderVertex.cvIntensity,ebx		// Store intensity.

		mov		[edi]SRenderCoord.iYScr,ecx				// Store integer Y.
		add		edi,SIZE SRenderVertex					// Step destination.
		
		dec		edx
		jnz		VTXLOOP1

		jmp		VTXDONE

NO_CLAMP:
		//
		// Copy vertices & scale texture values, no clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tY,tX)
		fxch	st(2)								// (tX,tY,iY)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tX*Z,tY,iY)
		fxch	st(2)								// (iY,tY,tX*Z)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(1)								// (tY,iY,tX*Z)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tY*Z,iY,tX*Z)

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z

		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.

		mov		ecx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.
		mov		ebx,[eax]SRenderVertex.cvIntensity		// Load intenstity.

		and		ecx,0x0007ffff							// Mask off FP junk.
		mov		[edi]SRenderVertex.cvIntensity,ebx		// Store intensity.

		mov		[edi]SRenderCoord.iYScr,ecx				// Store integer Y.
		add		edi,SIZE SRenderVertex					// Step destination.
		
		dec		edx
		jnz		VTXLOOP2

VTXDONE:
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataBump(CRenderPolygon *rpoly)
//
// Scanline data initialization for bump shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	u4TextureTileMask = (pras_texture->u4HeightTileMask << 9) | pras_texture->u4WidthTileMask;
	u4TextureTileMaskStepU = pras_texture->u4WidthTileMask;

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = 0;
	u4ConstColour = 0;

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapBump::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		mov		ebx,bClampUV
		mov		eax,pras_texture					// Pointer to texture.

		test	ebx,ebx
		jz		NO_CLAMP

		//
		// Copy vertices & scale texture values with clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX *
		//								 fTexWidth  + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//								 fTexHeight + fTexEdgeTolerance) * 
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fxch	st(1)								// (tX,tY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tX.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tX,tY)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(1)								// (tX,iY,tY)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tX*Z,iY,tY)
		fxch	st(2)								// (tY,iY,tX*Z)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tY.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tY*Z,iY,tX*Z)
		fxch	st(2)								// (tX*Z,iY,tY*Z)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.
		fstp	d_temp_a							// Store 19.32 integer y value.
		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		ebx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.

		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z
		and		ebx,0x0007ffff							// Mask off FP junk.

		mov		[edi]SRenderCoord.iYScr,ebx				// Store integer Y.
		add		edi,SIZE SRenderVertex					// Step destination.
		
		dec		edx
		jnz		VTXLOOP1

		jmp		VTXDONE

NO_CLAMP:
		//
		// Copy vertices & scale texture values, no clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight) *
		//								 rpoly.paprvPolyVertices[i]->v3Screen.tZ;
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tY,tX)
		fxch	st(2)								// (tX,tY,iY)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tX*Z,tY,iY)
		fxch	st(2)								// (iY,tY,tX*Z)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(1)								// (tY,iY,tX*Z)
		fmul	[eax]SRenderCoord.v3Screen.tZ		// (tY*Z,iY,tX*Z)

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z

		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.

		mov		ecx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.
		add		edi,SIZE SRenderVertex					// Step destination.

		and		ecx,0x0007ffff							// Mask off FP junk.
		dec		edx

		mov		[edi - SIZE SRenderVertex]SRenderCoord.iYScr,ecx	// Store integer Y.
		jnz		VTXLOOP2

VTXDONE:
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataLinear(CRenderPolygon *rpoly)
//
// Scanline data initialization for un-shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	u4TextureTileMask = (pras_texture->u4HeightTileMask << 9) | pras_texture->u4WidthTileMask;
	u4TextureTileMaskStepU = pras_texture->u4WidthTileMask;

	__asm
	{
		mov		ebx,bClampUV
		mov		eax,pras_texture					// Pointer to texture.

		test	ebx,ebx
		jz		NO_CLAMP

		//
		// Copy vertices & scale texture values with clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX *
		//									fTexWidth + fTexEdgeTolerance
		//
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//									fTexHeight + fTexEdgeTolerance
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fxch	st(1)								// (tX,tY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tX.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tX,tY)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(2)								// (tY,tX,iY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tY.
		fxch	st(1)								// (tX,tY,iY)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		ebx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.

		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z
		and		ebx,0x0007ffff							// Mask off FP junk.

		mov		[edi]SRenderCoord.iYScr,ebx				// Store integer Y.
		add		edi,SIZE SRenderVertex					// Step destination.
		
		dec		edx
		jnz		VTXLOOP1

		jmp		VTXDONE

NO_CLAMP:
		//
		// Copy vertices & scale texture values, no clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tY,tX)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(2)								// (tX,tY,iY)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.
		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z

		mov		ecx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.
		add		edi,SIZE SRenderVertex					// Step destination.

		and		ecx,0x0007ffff							// Mask off FP junk.
		dec		edx

		mov		[edi - SIZE SRenderVertex]SRenderCoord.iYScr,ecx	// Store integer Y.
		jnz		VTXLOOP2

VTXDONE:
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataLinearFlat(CRenderPolygon *rpoly)
//
// Scanline data initialization for flat shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	u4TextureTileMask = (pras_texture->u4HeightTileMask << 9) | pras_texture->u4WidthTileMask;
	u4TextureTileMaskStepU = pras_texture->u4WidthTileMask;

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = iPosFloatCast(rpoly->cvFace);
	u4ConstColour = 0;

#if VER_DEBUG
	iNumIntensities = 0;
	if (rpoly->ptexTexture && rpoly->ptexTexture->ppcePalClut && rpoly->ptexTexture->ppcePalClut->pclutClut)
	{
		iNumIntensities = rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues;
		Assert(iBaseIntensity < rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues);
	}
#endif

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapTexture<uint8>::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		mov		ebx,bClampUV
		mov		eax,pras_texture					// Pointer to texture.

		test	ebx,ebx
		jz		NO_CLAMP

		//
		// Copy vertices & scale texture values with clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX *
		//									fTexWidth + fTexEdgeTolerance
		//
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//									fTexHeight + fTexEdgeTolerance
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fxch	st(1)								// (tX,tY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tX.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tX,tY)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(2)								// (tY,tX,iY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tY.
		fxch	st(1)								// (tX,tY,iY)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		ebx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.

		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z
		and		ebx,0x0007ffff							// Mask off FP junk.

		mov		[edi]SRenderCoord.iYScr,ebx				// Store integer Y.
		add		edi,SIZE SRenderVertex					// Step destination.
		
		dec		edx
		jnz		VTXLOOP1

		jmp		VTXDONE

NO_CLAMP:
		//
		// Copy vertices & scale texture values, no clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tY,tX)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(2)								// (tX,tY,iY)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.
		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z

		mov		ecx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.
		add		edi,SIZE SRenderVertex					// Step destination.

		and		ecx,0x0007ffff							// Mask off FP junk.
		dec		edx

		mov		[edi - SIZE SRenderVertex]SRenderCoord.iYScr,ecx	// Store integer Y.
		jnz		VTXLOOP2

VTXDONE:
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataLinearGour(CRenderPolygon *rpoly)
//
// Scanline data initialization for gouraud shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	u4TextureTileMask = (pras_texture->u4HeightTileMask << 9) | pras_texture->u4WidthTileMask;
	u4TextureTileMaskStepU = pras_texture->u4WidthTileMask;

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = 0;
	u4ConstColour = 0;

#if VER_DEBUG
	iNumIntensities = rpoly->ptexTexture->ppcePalClut->pclutClut->iNumRampValues;
#endif

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapTexture<uint8>::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		mov		ebx,bClampUV
		mov		eax,pras_texture					// Pointer to texture.

		test	ebx,ebx
		jz		NO_CLAMP

		//
		// Copy vertices & scale texture values with clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX *
		//									 fTexWidth + fTexEdgeTolerance)
		//
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//									 fTexHeight + fTexEdgeTolerance) 
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fxch	st(1)								// (tX,tY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tX.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tX,tY)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(2)								// (tY,tX,iY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tY.
		fxch	st(1)								// (tX,tY,iY)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z

		mov		ecx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.
		mov		ebx,[eax]SRenderVertex.cvIntensity		// Load intenstity.

		and		ecx,0x0007ffff							// Mask off FP junk.
		mov		[edi]SRenderVertex.cvIntensity,ebx		// Store intensity.

		mov		[edi]SRenderCoord.iYScr,ecx				// Store integer Y.
		add		edi,SIZE SRenderVertex					// Step destination.
		
		dec		edx
		jnz		VTXLOOP1

		jmp		VTXDONE

NO_CLAMP:
		//
		// Copy vertices & scale texture values, no clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = (rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth)
		// arvRasterVertices[i].tcTex.tY = (rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight)
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tY,tX)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(2)								// (tX,tY,iY)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.
		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z

		mov		ecx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.
		mov		ebx,[eax]SRenderVertex.cvIntensity		// Load intenstity.

		and		ecx,0x0007ffff							// Mask off FP junk.
		mov		[edi]SRenderVertex.cvIntensity,ebx		// Store intensity.

		mov		[edi]SRenderCoord.iYScr,ecx				// Store integer Y.
		add		edi,SIZE SRenderVertex					// Step destination.
		
		dec		edx
		jnz		VTXLOOP2

VTXDONE:
	}
}


//*****************************************************************************************
//
inline void GenericInitializePolygonDataLinearBump(CRenderPolygon *rpoly)
//
// Scanline data initialization for bump shaded primitives.
//
//**************************************
{
	// Make sure these assumptions are correct.
	Assert(CRenderPolygon_paprvPolyVertices_uLen  == (int)&((CRenderPolygon *)0)->paprvPolyVertices.uLen);
	Assert(CRenderPolygon_paprvPolyVertices_atArray == (int)&((CRenderPolygon *)0)->paprvPolyVertices.atArray);

	//
	// Set up polygon-wide variables for rasterising; i.e. those not associated with 
	// triangle gradients.  Also, set needed per-vertex data.
	// Initialize polygon-wide variables for index coordinates.
	//
	rptr<CRaster> pras_texture = rpoly->ptexTexture->prasGetTexture(rpoly->iMipLevel);

	// Set a pointer to the texture.
	pvTextureBitmap = pras_texture->pSurface;

	// Set the global texture width variable.
	iTexWidth = pras_texture->iLinePixels;

	// Set the tiling & mask globals.
	//lineData.indCoord.BuildTileMask(pras_texture);
	bClampUV = pras_texture->bNotTileable;

	u4TextureTileMask = (pras_texture->u4HeightTileMask << 9) | pras_texture->u4WidthTileMask;
	u4TextureTileMaskStepU = pras_texture->u4WidthTileMask;

	// Set the base address for the clut.
	//lineData.SetClutAddress(ptexTexture);
	iBaseIntensity = 0;
	u4ConstColour = 0;

	// Call the get address function for the clut.
	pvClutConversion = rpoly->ptexTexture->ppcePalClut->pclutClut->pvGetConversionAddress
	(
		CMapBump::u4GetClutBaseColour(rpoly->ptexTexture),
		iBaseIntensity,
		iDefaultFog
	);

	__asm
	{
		mov		ebx,bClampUV
		mov		eax,pras_texture					// Pointer to texture.

		test	ebx,ebx
		jz		NO_CLAMP

		//
		// Copy vertices & scale texture values with clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP1:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX *
		//									fTexWidth + fTexEdgeTolerance
		//
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * 
		//									fTexHeight + fTexEdgeTolerance
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fxch	st(1)								// (tX,tY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tX.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tX,tY)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(2)								// (tY,tX,iY)
		fadd	fTexEdgeTolerance					// Add edge tolerance to tY.
		fxch	st(1)								// (tX,tY,iY)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		ebx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.

		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z
		and		ebx,0x0007ffff							// Mask off FP junk.

		mov		[edi]SRenderCoord.iYScr,ebx				// Store integer Y.
		add		edi,SIZE SRenderVertex					// Step destination.
		
		dec		edx
		jnz		VTXLOOP1

		jmp		VTXDONE

NO_CLAMP:
		//
		// Copy vertices & scale texture values, no clamping.
		//
		mov		ecx,rpoly							// Pointer to polygon.
		lea		edi,arvRasterVertices				// Destination vertices.

		mov		ebx,[eax]CRasterBase.fWidth
		mov		eax,[eax]CRasterBase.fHeight

		mov		esi,[ecx + CRenderPolygon_paprvPolyVertices_atArray]
		mov		edx,[ecx + CRenderPolygon_paprvPolyVertices_uLen]

		mov		fTexWidth,ebx						// fTexWidth  = pras_texture->fWidth;
		mov		fTexHeight,eax						// fTexHeight = pras_texture->fHeight;

VTXLOOP2:
		// Pointer to source vertex in eax.
		mov		eax,[esi]							// Pointer to vertex.
		add		esi,4								// Step vertex pointer array index.

		//
		// arvRasterVertices[i].iYScr = TruncateToInt(arvRasterVertices[i].v3Screen.tY);
		//
		// arvRasterVertices[i].tcTex.tX = rpoly.paprvPolyVertices[i]->tcTex.tX * fTexWidth
		// arvRasterVertices[i].tcTex.tY = rpoly.paprvPolyVertices[i]->tcTex.tY * fTexHeight
		//
		// arvRasterVertices[i].v3Screen = rpoly.paprvPolyVertices[i]->v3Screen;
		// arvRasterVertices[i].cvIntensity = rpoly.paprvPolyVertices[i]->cvIntensity;
		//
		fld		[fTexWidth]							// Texture X value. (tX)
		fmul	[eax]SRenderVertex.tcTex.tX			// Scaled by width.
		fld		[eax]SRenderVertex.tcTex.tY			// Texture Y value. (tY,tX)
		fmul	[fTexHeight]						// Scaled by height.
		fld		[eax]SRenderCoord.v3Screen.tY		// Screen Y. (iY,tY,tX)
		fadd	dFloatToFixed32						// Add integer conversion constant.
		fxch	st(2)								// (tX,tY,iY)
		fstp	[edi]SRenderVertex.tcTex.tX			// Store texture X.
		fstp	[edi]SRenderVertex.tcTex.tY			// Store texture Y.
		fstp	d_temp_a							// Store 19.32 integer y value.

		mov		ecx,[eax]SRenderCoord.v3Screen.tX		// Load X
		mov		ebx,[eax]SRenderCoord.v3Screen.tY		// Load Y

		mov		[edi]SRenderCoord.v3Screen.tX,ecx		// Store X
		mov		ecx,[eax]SRenderCoord.v3Screen.tZ		// Load Z

		mov		[edi]SRenderCoord.v3Screen.tY,ebx		// Store Y
		mov		[edi]SRenderCoord.v3Screen.tZ,ecx		// Store Z

		mov		ecx,dword ptr[d_temp_a + 4]				// Get integer part of 19.32 y value.
		add		edi,SIZE SRenderVertex					// Step destination.

		and		ecx,0x0007ffff							// Mask off FP junk.
		dec		edx

		mov		[edi - SIZE SRenderVertex]SRenderCoord.iYScr,ecx	// Store integer Y.
		jnz		VTXLOOP2

VTXDONE:
	}
}


//*****************************************************************************************
//
template <class aCEdge> void GenericInitializeEdge
(
	aCEdge &edge,				// The edge to initialize.
	SRenderVertex* prv_from,	// Starting coordinate of edge.
	SRenderVertex* prv_to		// End coordinate of edge.
)
//
// Sets up a polygon edge.
//
//**************************************
{
	typedef aCEdge::TScanline aScanline;
	int i4temp;
	float f_dy;

	__asm
	{
		mov		esi,prv_from								// From vertex.  
		mov		edi,prv_to									// To vertex.  

		mov		edx,edge									// Load edge poniter.  

		mov		eax,[esi]SRenderCoord.iYScr					

		inc		eax											

		mov		i4temp,eax									

		fild	i4temp										// (float)(prv_from->iYScr + 1)  
		fld		[edi]SRenderCoord.v3Screen.tY				// prv_to->v3Screen.tY  
		fsub	[esi]SRenderCoord.v3Screen.tY				// prv_to->v3Screen.tY - prv_from->v3Screen.tY  
		fxch	st(1)										// i4temp,f_dy  
		fsub	[esi]SRenderCoord.v3Screen.tY				// (float)(prv_from->iYScr + 1) - prv_from->v3Screen.tY  
		fxch	st(1)										// f_dy,fStartYDiff  

		mov		[edx]aCEdge.prvFrom,esi						// prvFrom = prv_from  
		mov		[edx]aCEdge.prvTo,edi						// prvTo   = prv_to  

		fstp	f_dy										// Save f_dy  
		fstp	[edx]aCEdge.fStartYDiff						// Save fStartYDiff  

		mov		eax,f_dy									// Load f_dy  
		mov		ebx,fPOINT_ZERO_ONE							// Load minimum constant  

		cmp		eax,ebx										// fdy >= 0.01  
		jge		short DO_DIVIDE								

		mov		eax,fRECIP_POINT_ZERO_ONE					
		mov		[edx]aCEdge.fInvDY,eax						
		jmp		short DONE_WITH_DIVIDE						

DO_DIVIDE: 
		// fld		fONE									 
		// fdiv	f_dy										// 1.0 / f_dy  

		mov		ebx,[f_dy]									
 		mov		eax,iFI_SIGN_EXPONENT_SUB					
 		sub		eax,ebx										
 		and		ebx,iFI_MASK_MANTISSA						
 		sar		ebx,iSHIFT_MANTISSA							
 		and		eax,iFI_MASK_SIGN_EXPONENT					
 		// One iteration of Newton-Raphson doubles the number of bits of accuary.  
 		fld		[f_dy]										
 		add		eax,dword ptr[i4InverseMantissa + ebx*4]	
 		mov		[edx]aCEdge.fInvDY,eax						
 		fmul	[edx]aCEdge.fInvDY							// (v*r)  
 		fsubr	[fTWO]										// (2.0 - v*r)  
 		fmul	[edx]aCEdge.fInvDY							// (2.0 - v*r)*r  

		fstp	[edx]aCEdge.fInvDY							// fInvDY = ^  

DONE_WITH_DIVIDE: 
		mov		eax,[esi]SRenderCoord.iYScr					
		mov		ebx,[edi]SRenderCoord.iYScr					

		cmp		eax,ebx										// if (prv_from->iYScr == prv_to->iYScr)  
		je		RETURN_FROM_FUNC							// return;  

		fld		[edi]SRenderCoord.v3Screen.tX				// prv_to->v3Screen.tX  
		fsub	[esi]SRenderCoord.v3Screen.tX				// prv_to->v3Screen.tX - prv_from->v3Screen.tX  

		fmul	[edx]aCEdge.fInvDY							// f_increment_x = fInvDY * (prv_to->v3Screen.tX - prv_from->v3Screen.tX)  

		fld		st(0)										// Dup f_increment_x  
		fmul	[edx]aCEdge.fStartYDiff						// fix*fStartYDiff, fix  
		fxch	st(1)										// fix, fix*fStartYDiff  
		fadd	dFloatToFixed16								// Convert to 16.16  
		fxch	st(1)										// fix*fStartYDiff, fix  
		fadd	[esi]SRenderCoord.v3Screen.tX				// fix*fStartYDiff+tX, fix  
		fxch	st(1)										// fix,fix*fStartYDiff+tX  
		fstp	d_temp_a									// Store converted value  
		fadd	dFloatToFixed16								// Convert to 16.16  

		fstp	d_temp_b									// Store converted value  

		mov		ebx,dword ptr[d_temp_a]						
		mov		ecx,dword ptr[d_temp_b]						

		mov		[edx]aCEdge.lineIncrement.fxX,ebx			
		mov		[edx]aCEdge.lineStart.fxX,ecx				
RETURN_FROM_FUNC:
	}
}


//*****************************************************************************************
//
inline bool GenericInitTriangleData
(
	CDrawPolygonBase*					poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	int									b_update,
	bool								b_altpersp
)
//
// Sets up triangle-wide rasterising info for 
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	Assert(prv_a);
	Assert(prv_b);
	Assert(prv_c);

	//
	// Calculate fInvDx, the partial derivative of x over the triangle, using the following
	// formula:
	//
	//		           1                       1
	//		fInvDx = ---- = ---------------------------------------
	//	              dx    (x2 - x1)(y3 - y1) - (x3 - x1)(y2 - y1)
	//
	// dx also happens to be -2 times the area of the triangle, so we can check for correct
	// orientation here as well.
	//
	float f_dx;
	float f_invdx;
	float f_yab_invdx;
	float f_yac_invdx;
	float f_duinvz;
	float f_dvinvz;
	float f_dinvz;
	float f_temp;

	__asm
	{
		mov		edi,prv_c						// prv_c = edi
		mov		edx,prv_a						// prv_a = edx
		mov		esi,prv_b						// prv_b = esi

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		fld		[edi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yac
		fld		[esi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xab,f_yac
		fld		[esi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yab,f_xab,f_yac
		fxch	st(1)							// f_xab,f_yab,f_yac
		fmul	st(0),st(2)						// f_xab*f_yac,f_yab,f_yac
		fld		[edi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xac,f_xab*f_yac,f_yab,f_yac
		fld		[esi]SRenderVertex.tcTex.tX
		fsub	[edx]SRenderVertex.tcTex.tX		// f_uab,f_xac,f_xab*f_yac,f_yab,f_yac
		fxch	st(1)							// f_xac,f_uab,f_xab*f_yac,f_yab,f_yac
		fmul	st(0),st(3)						// f_xac*f_yab,f_uab,f_xab*f_yac,f_yab,f_yac
		fld		[edi]SRenderVertex.tcTex.tX
		fsub	[edx]SRenderVertex.tcTex.tX		// f_uac,f_xac*f_yab,f_uab,f_xab*f_yac,f_yab,f_yac
		fxch	st(3)							// f_xab*f_yac,f_xac*f_yab,f_uab,f_uac,f_yab,f_yac
		fsubrp	st(1),st(0)						// f_dx,f_uab,f_uac,f_yab,f_yac
		fld		[esi]SRenderVertex.tcTex.tY
		fsub	[edx]SRenderVertex.tcTex.tY		// f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fld		st(1)							// f_dx,f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fsub	fMAX_NEG_AREA					// f_dx',f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fxch	st(2)							// f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fld		[edi]SRenderVertex.tcTex.tY		// f_vac,f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fsub	[edx]SRenderVertex.tcTex.tY		// f_vac,f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fxch	st(3)							// f_dx',f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac
		// stall(1)
		fstp	f_temp							// f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac

		mov		eax,f_temp

		test	eax,0x80000000					// if (f_dx >= fMAX_NEG_AREA)
		jnz		short GOOD_TO_GO

		fcompp									// Dump stuff off the stack
		fcompp									// Dump stuff off the stack
		fcompp									// Dump stuff off the stack
		fcomp	st(0)							// Dump stuff off the stack
		jmp		RETURN_FALSE

GOOD_TO_GO:
		//
		// Calculate coefficients to pass to template InitializeTriangleData functions.
		//
		// float f_invdx = 1.0f / f_dx;
		// float f_yab_invdx = f_yab * f_invdx;
		// float f_yac_invdx = f_yac * f_invdx;
		//
		fstp	f_dx							// f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac
//		fdivr	fONE							// invdx,vab,vac,uab,uac,yab,yac

		mov		ebx,dword ptr[f_dx]
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		// One iteration of Newton-Raphson doubles the number of bits of accuary.
		fld		[f_dx]

		add		eax,dword ptr[i4InverseMantissa + ebx*4]

		mov		dword ptr[f_invdx],eax

		fmul	[f_invdx]						// (v*r)
		
		fsubr	[fTWO]							// (2.0 - v*r)

		fmul	[f_invdx]						// (2.0 - v*r)*r

		fmul	st(5),st(0)						// invdx,vab,vac,uab,uac,yab_invdx,yac
		fst		f_invdx
		fmulp	st(6),st(0)						// vab,_vac,uab,uac,yab_invdx,yac_invdx
		fxch	st(4)							// yab_invdx,vac,uab,uac,vab,yac_invdx
		fst		f_yab_invdx
		fmul	st(3),st(0)						// yab_invdx,vac,uab,uac*yab,vab,yac_invdx
		fxch	st(5)							// yac_invdx,vac,uab,uac*yab,vab,yab_invdx
		fst		f_yac_invdx
		fmul	st(2),st(0)						// yac_invdx,vac,uab*yac,uac*yab,vab,yab_invdx

		//
		// Get the step values for u and v with respect to x.
		//
		// float f_duinvz = (prv_b->tcTex.tX - prv_a->tcTex.tX) * f_yac_invdx -
		//				    (prv_c->tcTex.tX - prv_a->tcTex.tX) * f_yab_invdx;
		//
		// float f_dvinvz = (prv_b->tcTex.tY - prv_a->tcTex.tY) * f_yac_invdx -
		//				    (prv_c->tcTex.tY - prv_a->tcTex.tY) * f_yab_invdx;
		//
		// float f_dinvz =  (prv_b->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yac_invdx -
		//				    (prv_c->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yab_invdx;
		//
		fld		[esi]SRenderCoord.v3Screen.tZ
		fsub	[edx]SRenderCoord.v3Screen.tZ	// zab,yac_invdx,vac,uab*yac,uac*yab,vab,yab_invdx
		fxch	st(6)							// yab_invdx,yac_invdx,vac,uab*yac,uac*yab,vab,zab
		fmul	st(2),st(0)						// yab_invdx,yac_invdx,vac*yab,uab*yac,uac*yab,vab,zab
		fld		[edi]SRenderCoord.v3Screen.tZ
		fsub	[edx]SRenderCoord.v3Screen.tZ	// zac,yab_invdx,yac_invdx,vac*yab,uab*yac,uac*yab,vab,zab
		fxch	st(2)							// yac_invdx,yab_invdx,zac,vac*yab,uab*yac,uac*yab,vab,zab
		fmul	st(6),st(0)						// yac_invdx,yab_invdx,zac,vac*yab,uab*yac,uac*yab,vab*yac,zab
		fxch	st(4)							// uab*yac,yab_invdx,zac,vac*yab,yac_invdx,uac*yab,vab*yac,zab
		fsubrp	st(5),st(0)						// yab_invdx,zac,vac*yab,yac_invdx,duinvz,vab*yac,zab
		fxch	st(6)							// zab,zac,vac*yab,yac_invdx,duinvz,vab*yac,yab_invdx
		fmulp	st(3),st(0)						// zac,vac*yab,zab*yac,duinvz,vab*yac,yab_invdx
		fxch	st(1)							// vac*yab,zac,zab*yac,duinvz,vab*yac,yab_invdx
		fsubp	st(4),st(0)						// zac,zab*yac,duinvz,dvinvz,yab_invdx
		fmulp	st(4),st(0)						// zab*yac,duinvz,dvinvz,zac*yab
		fxch	st(1)							// duinvz,zab*yac,dvinvz,zac*yab
		fstp	f_duinvz						// zab*yac,dvinvz,zac*yab
		fsubrp	st(2),st(0)						// dvinvz,dinvz
		fstp	f_dvinvz						// dinvz
		mov		eax,b_update
		// stall(1)
		fstp	f_dinvz

		test	eax,eax
		jz		short COPY_UVZ

		//
		// SetMinAbs(fDUInvZ, f_duinvz);
		// SetMinAbs(fDVInvZ, f_dvinvz);
		// SetMax(fDInvZ, f_dinvz);
		//
		mov		eax,f_duinvz
		mov		ebx,fDUInvZ

		and		eax,0x7fffffff
		and		ebx,0x7fffffff

		cmp		eax,ebx
		jge		short SKIP_U_COPY

		mov		eax,f_duinvz
		mov		fDUInvZ,eax

SKIP_U_COPY:
		mov		eax,f_dvinvz
		mov		ebx,fDVInvZ

		and		eax,0x7fffffff
		and		ebx,0x7fffffff

		cmp		eax,ebx
		jge		short SKIP_V_COPY

		mov		eax,f_dvinvz
		mov		fDVInvZ,eax

SKIP_V_COPY:
		mov		eax,f_dinvz
		mov		ecx,0x80000000

		mov		ebx,fDInvZ
		mov		edx,0x80000000

		and		ecx,eax
		and		eax,0x7fffffff

		sar		ecx,31
		and		edx,ebx

		sar		edx,31
		and		ebx,0x7fffffff

		xor		eax,ecx
		xor		ebx,edx

		cmp		eax,ebx
		jle		short SKIP_Z_COPY

		mov		eax,f_dinvz
		mov		fDInvZ,eax

SKIP_Z_COPY:
		jmp		short DONE_WITH_COPY

COPY_UVZ:
		//
		// fDUInvZ = f_duinvz;
		// fDVInvZ = f_dvinvz;
		// fDInvZ  = f_dinvz;
		//
		mov		eax,f_duinvz
		mov		ebx,f_dvinvz
		mov		ecx,f_dinvz
		mov		fDUInvZ,eax
		mov		fDVInvZ,ebx
		mov		fDInvZ,ecx

DONE_WITH_COPY:
	}

	// Calculate the subdivision length with respect to X.
	iSubdivideLen    = persetSettings.iGetSubdivisionLen(fDInvZ, b_altpersp);
	fInvSubdivideLen = fUnsignedInverseInt(iSubdivideLen);

	return true;

RETURN_FALSE:
	return false;
}


//*****************************************************************************************
//
inline bool GenericInitTriangleDataGour
(
	CDrawPolygonBase*					poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	int									b_update,
	bool								b_altpersp
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	Assert(prv_a);
	Assert(prv_b);
	Assert(prv_c);

	//
	// Calculate fInvDx, the partial derivative of x over the triangle, using the following
	// formula:
	//
	//		           1                       1
	//		fInvDx = ---- = ---------------------------------------
	//	              dx    (x2 - x1)(y3 - y1) - (x3 - x1)(y2 - y1)
	//
	// dx also happens to be -2 times the area of the triangle, so we can check for correct
	// orientation here as well.
	//
	float f_dx;
	float f_invdx;
	float f_yab_invdx;
	float f_yac_invdx;
	float f_duinvz;
	float f_dvinvz;
	float f_dinvz;
	float f_di_invdx;
	float f_temp;

	__asm
	{
		mov		edi,prv_c						// prv_c = edi
		mov		edx,prv_a						// prv_a = edx
		mov		esi,prv_b						// prv_b = esi

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		fld		[edi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yac
		fld		[esi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xab,f_yac
		fld		[esi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yab,f_xab,f_yac
		fxch	st(1)							// f_xab,f_yab,f_yac
		fmul	st(0),st(2)						// f_xab*f_yac,f_yab,f_yac
		fld		[edi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xac,f_xab*f_yac,f_yab,f_yac
		fld		[esi]SRenderVertex.tcTex.tX
		fsub	[edx]SRenderVertex.tcTex.tX		// f_uab,f_xac,f_xab*f_yac,f_yab,f_yac
		fxch	st(1)							// f_xac,f_uab,f_xab*f_yac,f_yab,f_yac
		fmul	st(0),st(3)						// f_xac*f_yab,f_uab,f_xab*f_yac,f_yab,f_yac
		fld		[edi]SRenderVertex.tcTex.tX
		fsub	[edx]SRenderVertex.tcTex.tX		// f_uac,f_xac*f_yab,f_uab,f_xab*f_yac,f_yab,f_yac
		fxch	st(3)							// f_xab*f_yac,f_xac*f_yab,f_uab,f_uac,f_yab,f_yac
		fsubrp	st(1),st(0)						// f_dx,f_uab,f_uac,f_yab,f_yac
		fld		[esi]SRenderVertex.tcTex.tY
		fsub	[edx]SRenderVertex.tcTex.tY		// f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fld		st(1)							// f_dx,f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fsub	fMAX_NEG_AREA					// f_dx',f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fxch	st(2)							// f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fld		[edi]SRenderVertex.tcTex.tY		// f_vac,f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fsub	[edx]SRenderVertex.tcTex.tY		// f_vac,f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fxch	st(3)							// f_dx',f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac
		// stall(1)
		fstp	f_temp							// f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac

		mov		eax,f_temp

		test	eax,0x80000000					// if (f_dx >= fMAX_NEG_AREA)
		jnz		short GOOD_TO_GO

		fcompp									// Dump stuff off the stack
		fcompp									// Dump stuff off the stack
		fcompp									// Dump stuff off the stack
		fcomp	st(0)							// Dump stuff off the stack
		jmp		RETURN_FALSE

GOOD_TO_GO:
		//
		// Calculate coefficients to pass to template InitializeTriangleData functions.
		//
		// float f_invdx = 1.0f / f_dx;
		// float f_yab_invdx = f_yab * f_invdx;
		// float f_yac_invdx = f_yac * f_invdx;
		//
		fstp	f_dx							// f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac
//		fdivr	fONE							// invdx,vab,vac,uab,uac,yab,yac

		mov		ebx,dword ptr[f_dx]
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		// One iteration of Newton-Raphson doubles the number of bits of accuary.
		fld		[f_dx]

		add		eax,dword ptr[i4InverseMantissa + ebx*4]

		mov		dword ptr[f_invdx],eax

		fmul	[f_invdx]						// (v*r)
		
		fsubr	[fTWO]							// (2.0 - v*r)

		fmul	[f_invdx]						// (2.0 - v*r)*r

		fmul	st(5),st(0)						// invdx,vab,vac,uab,uac,yab_invdx,yac
		fst		f_invdx
		fmulp	st(6),st(0)						// vab,_vac,uab,uac,yab_invdx,yac_invdx
		fxch	st(4)							// yab_invdx,vac,uab,uac,vab,yac_invdx
		fst		f_yab_invdx
		fmul	st(3),st(0)						// yab_invdx,vac,uab,uac*yab,vab,yac_invdx
		fxch	st(5)							// yac_invdx,vac,uab,uac*yab,vab,yab_invdx
		fst		f_yac_invdx
		fmul	st(2),st(0)						// yac_invdx,vac,uab*yac,uac*yab,vab,yab_invdx

		//
		// Get the step values for u and v with respect to x.
		//
		// float f_duinvz = (prv_b->tcTex.tX - prv_a->tcTex.tX) * f_yac_invdx -
		//				    (prv_c->tcTex.tX - prv_a->tcTex.tX) * f_yab_invdx;
		//
		// float f_dvinvz = (prv_b->tcTex.tY - prv_a->tcTex.tY) * f_yac_invdx -
		//				    (prv_c->tcTex.tY - prv_a->tcTex.tY) * f_yab_invdx;
		//
		// float f_dinvz =  (prv_b->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yac_invdx -
		//				    (prv_c->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yab_invdx;
		//
		//
		// Find the step value for intensity with respect to the horizontal axis.
		//
		// float f_di_invdx =	(prv_b->cvIntensity - prv_a->cvIntensity) * f_yac_invdx -
		//						(prv_c->cvIntensity - prv_a->cvIntensity) * f_yab_invdx;
		//
		fld		[esi]SRenderCoord.v3Screen.tZ
		fsub	[edx]SRenderCoord.v3Screen.tZ	// zab,yac_invdx,vac,uab*yac,uac*yab,vab,yab_invdx
		fxch	st(6)							// yab_invdx,yac_invdx,vac,uab*yac,uac*yab,vab,zab
		fmul	st(2),st(0)						// yab_invdx,yac_invdx,vac*yab,uab*yac,uac*yab,vab,zab
		fld		[edi]SRenderCoord.v3Screen.tZ
		fsub	[edx]SRenderCoord.v3Screen.tZ	// zac,yab_invdx,yac_invdx,vac*yab,uab*yac,uac*yab,vab,zab
		fxch	st(2)							// yac_invdx,yab_invdx,zac,vac*yab,uab*yac,uac*yab,vab,zab
		fmul	st(6),st(0)						// yac_invdx,yab_invdx,zac,vac*yab,uab*yac,uac*yab,vab*yac,zab
		fxch	st(4)							// uab*yac,yab_invdx,zac,vac*yab,yac_invdx,uac*yab,vab*yac,zab
		fsubrp	st(5),st(0)						// yab,zac,vac*yab,yac,duinvz,vab*yac,zab
		fmul	st(1),st(0)						// yab,zac*yab,vac*yab,yac,duinvz,vab*yac,zab
		fld		[esi]SRenderVertex.cvIntensity
		fsub	[edx]SRenderVertex.cvIntensity	// iab,yab,zac*yab,vac*yab,yac,duinvz,vab*yac,zab
		fxch	st(6)							// vab*yac,yab,zac*yab,vac*yab,yac,duinvz,iab,zab
		fsubrp	st(3),st(0)						// yab,zac*yab,dvinvz,yac,duinvz,iab,zab
		fxch	st(3)							// yac,zac*yab,dvinvz,yab,duinvz,iab,zab
		fmul	st(6),st(0)						// yac,zac*yab,dvinvz,yab,duinvz,iab,zab*yac
		fld		[edi]SRenderVertex.cvIntensity
		fsub	[edx]SRenderVertex.cvIntensity	// iac,yac,zac*yab,dvinvz,yab,duinvz,iab,zab*yac
		fxch	st(2)							// zac*yab,yac,iac,dvinvz,yab,duinvz,iab,zab*yac
		fsubp	st(7),st(0)						// yac,iac,dvinvz,yab,duinvz,iab,dinvz
		fmulp	st(5),st(0)						// iac,dvinvz,yab,duinvz,iab*yac,dinvz
		fxch	st(1)							// dvinvz,iac,yab,duinvz,iab*yac,dinvz
		fstp	f_dvinvz						// iac,yab,duinvz,iab*yac,dinvz
		fmulp	st(1),st(0)						// iac*yab,duinvz,iab*yac,dinvz
		fxch	st(1)							// duinvz,iac*yab,iab*yac,dinvz
		fstp	f_duinvz						// iac*yab,iab*yac,dinvz
		fsubp	st(1),st(0)						// di_invdx,dinvz
		fxch	st(1)							// dinvz,di_invdx
		fstp	f_dinvz							// di_invdx
		mov		eax,b_update					// Load b_update flag.
		fstp	f_di_invdx						// fp stack empty

		test	eax,eax
		jz		COPY_UVZ

		//
		// SetMinAbs(fDUInvZ, f_duinvz);
		// SetMinAbs(fDVInvZ, f_dvinvz);
		// SetMax(fDInvZ, f_dinvz);
		//
		mov		eax,f_duinvz
		mov		ebx,fDUInvZ

		and		eax,0x7fffffff
		and		ebx,0x7fffffff

		cmp		eax,ebx
		jge		short SKIP_U_COPY

		mov		eax,f_duinvz
		mov		fDUInvZ,eax

SKIP_U_COPY:
		mov		eax,f_dvinvz
		mov		ebx,fDVInvZ

		and		eax,0x7fffffff
		and		ebx,0x7fffffff

		cmp		eax,ebx
		jge		short SKIP_V_COPY

		mov		eax,f_dvinvz
		mov		fDVInvZ,eax

SKIP_V_COPY:
		mov		eax,f_dinvz
		mov		ecx,0x80000000

		mov		ebx,fDInvZ
		mov		edx,0x80000000

		and		ecx,eax
		and		eax,0x7fffffff

		sar		ecx,31
		and		edx,ebx

		sar		edx,31
		and		ebx,0x7fffffff

		xor		eax,ecx
		xor		ebx,edx

		cmp		eax,ebx
		jle		short SKIP_Z_COPY

		mov		eax,f_dinvz
		mov		fDInvZ,eax

SKIP_Z_COPY:
		//
		//	if (CIntFloat(f_di_invdx).bSign() != CIntFloat(fDeltaXIntensity).bSign())
		//	{
		//		fDeltaXIntensity = 0.0f;
		//		fxDeltaXIntensity = 0;
		//		goto SKIP_INTENSITY_UPDATE;
		//	}
		//
		//  if (fabs(f_di_invdx) >= fabs(fDeltaXIntensity))
		//		goto SKIP_INTENSITY_UPDATE;
		//
		mov		eax,f_di_invdx
		mov		ebx,fDeltaXIntensity

		mov		ecx,eax							// copy f_di_invdx
		xor		eax,ebx							// xor sign bits

		test	eax,0x80000000					// test for sign bit.
		jz		COMPARE_INTENSITY				// same sign, compare abs. values

		xor		eax,eax
		mov		fDeltaXIntensity,eax

		mov		fxDeltaXIntensity,eax
		jmp		SKIP_INTENSITY_UPDATE

COMPARE_INTENSITY:
		and		ecx,0x7fffffff
		and		ebx,0x7fffffff

		cmp		ecx,ebx
		jge		SKIP_INTENSITY_UPDATE
		
		jmp		DONE_WITH_COPY

COPY_UVZ:
		//
		// fDUInvZ = f_duinvz;
		// fDVInvZ = f_dvinvz;
		// fDInvZ  = f_dinvz;
		//
		mov		eax,f_duinvz
		mov		ebx,f_dvinvz
		mov		ecx,f_dinvz
		mov		fDUInvZ,eax
		mov		fDVInvZ,ebx
		mov		fDInvZ,ecx

DONE_WITH_COPY:
		//
		// fDeltaXIntensity = f_di_invdx;
		//
		// Clamp(fDeltaXIntensity, 16.0f);
		//
		// Convert to fixed point representation.
		//
		// fxDeltaXIntensity.fxFromFloat(fDeltaXIntensity);
		//
		fld		f_di_invdx						// Load intensity (assuming no clamp).
		fadd	dFloatToFixed16					// Add integer conversion constant.

		mov		eax,f_di_invdx					// Load value.
		mov		ecx,fSIXTEEN					// Load clamp value.

		mov		ebx,eax							// Copy value for sign.
		mov		edx,eax							// Keep orignal value.

		and		eax,0x7fffffff					// Mask off sign bit.
		and		ebx,0x80000000					// Keep just the sign bit.

		cmp		eax,ecx							// Compare against 16.0
		jle		short NO_INTENSITY_CLAMP
		
		//
		// Clamp value. This is a relatively rare case.
		//
		fcomp	st(0)							// Dump unclamped value.

		mov		edx,ecx							// Clamp value.

		or		edx,ebx							// Keep sign bit.

		mov		f_di_invdx,edx					// Save clamped value.

		fld		f_di_invdx						// Load clamped intensity.
		fadd	dFloatToFixed16					// Add integer conversion constant.
		// stall(3)
		
NO_INTENSITY_CLAMP:
		fstp	d_temp_a						// Store fixed point value.

		mov		fDeltaXIntensity,edx			// Save updated, possibly clamped value.
		mov		eax,dword ptr[d_temp_a]			// Load fixed point value.

		mov		fxDeltaXIntensity,eax			// Save fixed point value.
	
SKIP_INTENSITY_UPDATE:
	}

	// Calculate the subdivision length with respect to X.
	iSubdivideLen    = persetSettings.iGetSubdivisionLen(fDInvZ, b_altpersp);
	fInvSubdivideLen = fUnsignedInverseInt(iSubdivideLen);

	return true;

RETURN_FALSE:
	return false;
}


//*****************************************************************************************
//
inline bool GenericInitTriangleDataLinear
(
	CDrawPolygonBase*					poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	int									b_update
)
//
// Sets up triangle-wide rasterising info for 
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	Assert(prv_a);
	Assert(prv_b);
	Assert(prv_c);

	//
	// Calculate fInvDx, the partial derivative of x over the triangle, using the following
	// formula:
	//
	//		           1                       1
	//		fInvDx = ---- = ---------------------------------------
	//	              dx    (x2 - x1)(y3 - y1) - (x3 - x1)(y2 - y1)
	//
	// dx also happens to be -2 times the area of the triangle, so we can check for correct
	// orientation here as well.
	//
	float f_dx;
	float f_invdx;
	float f_yab_invdx;
	float f_yac_invdx;
	float f_du;
	float f_dv;
	float f_temp;

	__asm
	{
		mov		edi,prv_c						// prv_c = edi
		mov		edx,prv_a						// prv_a = edx
		mov		esi,prv_b						// prv_b = esi

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		fld		[edi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yac
		fld		[esi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xab,f_yac
		fld		[esi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yab,f_xab,f_yac
		fxch	st(1)							// f_xab,f_yab,f_yac
		fmul	st(0),st(2)						// f_xab*f_yac,f_yab,f_yac
		fld		[edi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xac,f_xab*f_yac,f_yab,f_yac
		fld		[esi]SRenderVertex.tcTex.tX
		fsub	[edx]SRenderVertex.tcTex.tX		// f_uab,f_xac,f_xab*f_yac,f_yab,f_yac
		fxch	st(1)							// f_xac,f_uab,f_xab*f_yac,f_yab,f_yac
		fmul	st(0),st(3)						// f_xac*f_yab,f_uab,f_xab*f_yac,f_yab,f_yac
		fld		[edi]SRenderVertex.tcTex.tX
		fsub	[edx]SRenderVertex.tcTex.tX		// f_uac,f_xac*f_yab,f_uab,f_xab*f_yac,f_yab,f_yac
		fxch	st(3)							// f_xab*f_yac,f_xac*f_yab,f_uab,f_uac,f_yab,f_yac
		fsubrp	st(1),st(0)						// f_dx,f_uab,f_uac,f_yab,f_yac
		fld		[esi]SRenderVertex.tcTex.tY
		fsub	[edx]SRenderVertex.tcTex.tY		// f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fld		st(1)							// f_dx,f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fsub	fMAX_NEG_AREA					// f_dx',f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fxch	st(2)							// f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fld		[edi]SRenderVertex.tcTex.tY		// f_vac,f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fsub	[edx]SRenderVertex.tcTex.tY		// f_vac,f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fxch	st(3)							// f_dx',f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac
		// stall(1)
		fstp	f_temp							// f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac

		mov		eax,f_temp

		test	eax,0x80000000					// if (f_dx >= fMAX_NEG_AREA)
		jnz		short GOOD_TO_GO

		fcompp									// Dump stuff off the stack
		fcompp									// Dump stuff off the stack
		fcompp									// Dump stuff off the stack
		fcomp	st(0)							// Dump stuff off the stack
		jmp		RETURN_FALSE

GOOD_TO_GO:
		//
		// Calculate coefficients to pass to template InitializeTriangleData functions.
		//
		// float f_invdx = 1.0f / f_dx;
		// float f_yab_invdx = f_yab * f_invdx;
		// float f_yac_invdx = f_yac * f_invdx;
		//
		fstp	f_dx							// f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac
//		fdivr	fONE							// invdx,vab,vac,uab,uac,yab,yac

		mov		ebx,dword ptr[f_dx]
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		// One iteration of Newton-Raphson doubles the number of bits of accuary.
		fld		[f_dx]

		add		eax,dword ptr[i4InverseMantissa + ebx*4]

		mov		dword ptr[f_invdx],eax

		fmul	[f_invdx]						// (v*r)
		
		fsubr	[fTWO]							// (2.0 - v*r)

		fmul	[f_invdx]						// (2.0 - v*r)*r

		fmul	st(5),st(0)						// invdx,vab,vac,uab,uac,yab_invdx,yac
		fst		f_invdx
		fmulp	st(6),st(0)						// vab,_vac,uab,uac,yab_invdx,yac_invdx
		fxch	st(4)							// yab_invdx,vac,uab,uac,vab,yac_invdx
		fst		f_yab_invdx
		fmul	st(3),st(0)						// yab_invdx,vac,uab,uac*yab,vab,yac_invdx
		fxch	st(5)							// yac_invdx,vac,uab,uac*yab,vab,yab_invdx
		fst		f_yac_invdx
		fmul	st(2),st(0)						// yac_invdx,vac,uab*yac,uac*yab,vab,yab_invdx

		//
		// Get the step values for u and v with respect to x.
		//
		// float f_du = (prv_b->tcTex.tX - prv_a->tcTex.tX) * f_yac_invdx -
		//				(prv_c->tcTex.tX - prv_a->tcTex.tX) * f_yab_invdx;
		//
		// float f_dv = (prv_b->tcTex.tY - prv_a->tcTex.tY) * f_yac_invdx -
		//				(prv_c->tcTex.tY - prv_a->tcTex.tY) * f_yab_invdx;
		//
		mov		eax,b_update
		fmulp	st(4),st(0)						// vac,uab*yac,uac*yab,vab*yac,yab_invdx
		fxch	st(2)							// uac*yab,uab*yac,vac,vab*yac,yab_invdx
		fsubp	st(1),st(0)						// f_du,vac,vab*yac,yab_invdx
		fxch	st(1)							// vac,f_du,vab*yac,yab_invdx
		fmulp	st(3),st(0)						// f_du,vab*yac,vac*yab
		// stall(2)
		fstp	f_du							// vab*yac,vac*yab
		fsubrp	st(1),st(0)						// f_dv
		// stall(3)	
		fstp	f_dv

		test	eax,eax
		jz		short COPY_UV

		//
		// SetMinAbs(fDUInvZ, f_duinvz);
		// SetMinAbs(fDVInvZ, f_dvinvz);
		//
		mov		eax,[f_du]
		mov		ebx,[fDU]

		mov		ecx,eax							// copy f_du
		xor		eax,ebx							// xor sign bits

		test	eax,0x80000000					// test for sign bit.
		jz		COMPARE_U						// same sign, compare abs. values

		xor		ecx,ecx							// clear ecx
		jmp		SAVE_NEW_U						// go store it
		
COMPARE_U:
		and		ecx,0x7fffffff
		and		ebx,0x7fffffff

		cmp		ecx,ebx
		jge		short SKIP_U_COPY

		mov		ecx,[f_du]

SAVE_NEW_U:
		mov		[fDU],ecx

SKIP_U_COPY:
		mov		eax,[f_dv]
		mov		ebx,[fDV]

		mov		ecx,eax							// copy f_dv
		xor		eax,ebx							// xor sign bits

		test	eax,0x80000000					// test for sign bit.
		jz		COMPARE_V						// same sign, compare abs. values

		xor		ecx,ecx							// clear ecx
		jmp		SAVE_NEW_V						// go store it
		
COMPARE_V:
		and		ecx,0x7fffffff
		and		ebx,0x7fffffff

		cmp		ecx,ebx
		jge		short DONE_WITH_COPY

		mov		ecx,[f_dv]

SAVE_NEW_V:
		mov		[fDV],ecx
		jmp		short DONE_WITH_COPY

COPY_UV:
		//
		// fDU = f_du
		// fDV = f_dv
		//
		mov		eax,[f_du]
		mov		ebx,[f_dv]
		mov		[fDU],eax
		mov		[fDV],ebx

DONE_WITH_COPY:
	}

	return true;

RETURN_FALSE:
	return false;
}


//*****************************************************************************************
//
inline bool GenericInitTriangleDataLinearGour
(
	CDrawPolygonBase*					poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	int									b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	Assert(prv_a);
	Assert(prv_b);
	Assert(prv_c);

	//
	// Calculate fInvDx, the partial derivative of x over the triangle, using the following
	// formula:
	//
	//		           1                       1
	//		fInvDx = ---- = ---------------------------------------
	//	              dx    (x2 - x1)(y3 - y1) - (x3 - x1)(y2 - y1)
	//
	// dx also happens to be -2 times the area of the triangle, so we can check for correct
	// orientation here as well.
	//
	float f_dx;
	float f_invdx;
	float f_yab_invdx;
	float f_yac_invdx;
	float f_du;
	float f_dv;
	float f_di_invdx;
	float f_temp;

	__asm
	{
		mov		edi,prv_c						// prv_c = edi
		mov		edx,prv_a						// prv_a = edx
		mov		esi,prv_b						// prv_b = esi

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		fld		[edi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yac
		fld		[esi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xab,f_yac
		fld		[esi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yab,f_xab,f_yac
		fxch	st(1)							// f_xab,f_yab,f_yac
		fmul	st(0),st(2)						// f_xab*f_yac,f_yab,f_yac
		fld		[edi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xac,f_xab*f_yac,f_yab,f_yac
		fld		[esi]SRenderVertex.tcTex.tX
		fsub	[edx]SRenderVertex.tcTex.tX		// f_uab,f_xac,f_xab*f_yac,f_yab,f_yac
		fxch	st(1)							// f_xac,f_uab,f_xab*f_yac,f_yab,f_yac
		fmul	st(0),st(3)						// f_xac*f_yab,f_uab,f_xab*f_yac,f_yab,f_yac
		fld		[edi]SRenderVertex.tcTex.tX
		fsub	[edx]SRenderVertex.tcTex.tX		// f_uac,f_xac*f_yab,f_uab,f_xab*f_yac,f_yab,f_yac
		fxch	st(3)							// f_xab*f_yac,f_xac*f_yab,f_uab,f_uac,f_yab,f_yac
		fsubrp	st(1),st(0)						// f_dx,f_uab,f_uac,f_yab,f_yac
		fld		[esi]SRenderVertex.tcTex.tY
		fsub	[edx]SRenderVertex.tcTex.tY		// f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fld		st(1)							// f_dx,f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fsub	fMAX_NEG_AREA					// f_dx',f_vab,f_dx,f_uab,f_uac,f_yab,f_yac
		fxch	st(2)							// f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fld		[edi]SRenderVertex.tcTex.tY		// f_vac,f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fsub	[edx]SRenderVertex.tcTex.tY		// f_vac,f_dx,f_vab,f_dx',f_uab,f_uac,f_yab,f_yac
		fxch	st(3)							// f_dx',f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac
		// stall(1)
		fstp	f_temp							// f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac

		mov		eax,f_temp

		test	eax,0x80000000					// if (f_dx >= fMAX_NEG_AREA)
		jnz		short GOOD_TO_GO

		fcompp									// Dump stuff off the stack
		fcompp									// Dump stuff off the stack
		fcompp									// Dump stuff off the stack
		fcomp	st(0)							// Dump stuff off the stack
		jmp		RETURN_FALSE

GOOD_TO_GO:
		//
		// Calculate coefficients to pass to template InitializeTriangleData functions.
		//
		// float f_invdx = 1.0f / f_dx;
		// float f_yab_invdx = f_yab * f_invdx;
		// float f_yac_invdx = f_yac * f_invdx;
		//
		fstp	f_dx							// f_dx,f_vab,f_vac,f_uab,f_uac,f_yab,f_yac
//		fdivr	fONE							// invdx,vab,vac,uab,uac,yab,yac

		mov		ebx,dword ptr[f_dx]
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		// One iteration of Newton-Raphson doubles the number of bits of accuary.
		fld		[f_dx]

		add		eax,dword ptr[i4InverseMantissa + ebx*4]

		mov		dword ptr[f_invdx],eax

		fmul	[f_invdx]						// (v*r)
		
		fsubr	[fTWO]							// (2.0 - v*r)

		fmul	[f_invdx]						// (2.0 - v*r)*r

		fmul	st(5),st(0)						// invdx,vab,vac,uab,uac,yab_invdx,yac
		fst		f_invdx
		fmulp	st(6),st(0)						// vab,_vac,uab,uac,yab_invdx,yac_invdx
		fxch	st(4)							// yab_invdx,vac,uab,uac,vab,yac_invdx
		fst		f_yab_invdx
		fmul	st(3),st(0)						// yab_invdx,vac,uab,uac*yab,vab,yac_invdx
		fxch	st(5)							// yac_invdx,vac,uab,uac*yab,vab,yab_invdx
		fst		f_yac_invdx
		fmul	st(2),st(0)						// yac_invdx,vac,uab*yac,uac*yab,vab,yab_invdx

		//
		// Get the step values for u and v with respect to x.
		//
		// float f_du = (prv_b->tcTex.tX - prv_a->tcTex.tX) * f_yac_invdx -
		//				(prv_c->tcTex.tX - prv_a->tcTex.tX) * f_yab_invdx;
		//
		// float f_dv = (prv_b->tcTex.tY - prv_a->tcTex.tY) * f_yac_invdx -
		//				(prv_c->tcTex.tY - prv_a->tcTex.tY) * f_yab_invdx;
		//
		//
		// Find the step value for intensity with respect to the horizontal axis.
		//
		// float f_di_invdx =	(prv_b->cvIntensity - prv_a->cvIntensity) * f_yac_invdx -
		//						(prv_c->cvIntensity - prv_a->cvIntensity) * f_yab_invdx;
		//
		mov		eax,b_update
		fmul	st(4),st(0)						// yac_invdx,vac,uab*yac,uac*yab,vab*yac,yab_invdx
		fld		[esi]SRenderVertex.cvIntensity	// iab,yac_invdx,vac,uab*yac,uac*yab,vab*yac,yab_invdx
		fsub	[edx]SRenderVertex.cvIntensity	// iab,yac_invdx,vac,uab*yac,uac*yab,vab*yac,yab_invdx
		fxch	st(6)							// yab_invdx,yac_invdx,vac,uab*yac,uac*yab,vab*yac,iab
		fmul	st(2),st(0)						// yab_invdx,yac_invdx,vac*yab,uab*yac,uac*yab,vab*yac,iab
		fld		[edi]SRenderVertex.cvIntensity	// iac,yab_invdx,yac_invdx,vac*yab,uab*yac,uac*yab,vab*yac,iab
		fsub	[edx]SRenderVertex.cvIntensity	// iac,yab_invdx,yac_invdx,vac*yab,uab*yac,uac*yab,vab*yac,iab
		fxch	st(2)							// yac_invdx,yab_invdx,iac,vac*yab,uab*yac,uac*yab,vab*yac,iab
		fmulp	st(7),st(0)						// yab_invdx,iac,vac*yab,uab*yac,uac*yab,vab*yac,iab*yac
		fxch	st(3)							// uab*yac,iac,vac*yab,yab_invdx,uac*yab,vab*yac,iab*yac
		fsubrp	st(4),st(0)						// iac,vac*yab,yab_invdx,f_du,vab*yac,iab*yac
		fmulp	st(2),st(0)						// vac*yab,iac*yab,f_du,vab*yac,iab*yac
		fsubp	st(3),st(0)						// iac*yab,f_du,f_dv,iab*yac
		fxch	st(1)							// f_du,iac*yab,f_dv,iab*yac
		// stall(1)
		fstp	f_du							// iac*yab,f_dv,iab*yac
		fsubp	st(2),st(0)						// f_dv,f_di_invdx
		fstp	f_dv							// f_di_invdx
		// stall(1)
		fstp	f_di_invdx						// fp stack empty


		test	eax,eax
		jz		COPY_UV

		//
		// SetMinAbs(fDUInvZ, f_duinvz);
		// SetMinAbs(fDVInvZ, f_dvinvz);
		//
		mov		eax,[f_du]
		mov		ebx,[fDU]

		mov		ecx,eax							// copy f_du
		xor		eax,ebx							// xor sign bits

		test	eax,0x80000000					// test for sign bit.
		jz		COMPARE_U						// same sign, compare abs. values

		xor		ecx,ecx							// clear ecx
		jmp		SAVE_NEW_U						// go store it
		
COMPARE_U:
		and		ecx,0x7fffffff
		and		ebx,0x7fffffff

		cmp		ecx,ebx
		jge		short SKIP_U_COPY

		mov		ecx,[f_du]

SAVE_NEW_U:
		mov		[fDU],ecx

SKIP_U_COPY:
		mov		eax,[f_dv]
		mov		ebx,[fDV]

		mov		ecx,eax							// copy f_dv
		xor		eax,ebx							// xor sign bits

		test	eax,0x80000000					// test for sign bit.
		jz		COMPARE_V						// same sign, compare abs. values

		xor		ecx,ecx							// clear ecx
		jmp		SAVE_NEW_V						// go store it
		
COMPARE_V:
		and		ecx,0x7fffffff
		and		ebx,0x7fffffff

		cmp		ecx,ebx
		jge		short SKIP_V_COPY

		mov		ecx,[f_dv]

SAVE_NEW_V:
		mov		[fDV],ecx

SKIP_V_COPY:
		//
		//	if (CIntFloat(f_di_invdx).bSign() != CIntFloat(fDeltaXIntensity).bSign())
		//	{
		//		fDeltaXIntensity = 0.0f;
		//		fxDeltaXIntensity = 0;
		//		goto SKIP_INTENSITY_UPDATE;
		//	}
		//
		//  if (fabs(f_di_invdx) >= fabs(fDeltaXIntensity))
		//		goto SKIP_INTENSITY_UPDATE;
		//
		mov		eax,f_di_invdx
		mov		ebx,fDeltaXIntensity

		mov		ecx,eax							// copy f_di_invdx
		xor		eax,ebx							// xor sign bits

		test	eax,0x80000000					// test for sign bit.
		jz		COMPARE_INTENSITY				// same sign, compare abs. values

		xor		eax,eax
		mov		fDeltaXIntensity,eax

		mov		fxDeltaXIntensity,eax
		jmp		SKIP_INTENSITY_UPDATE

COMPARE_INTENSITY:
		and		ecx,0x7fffffff
		and		ebx,0x7fffffff

		cmp		ecx,ebx
		jge		short SKIP_INTENSITY_UPDATE
		
		jmp		short DONE_WITH_COPY

COPY_UV:
		//
		// fDU = f_du
		// fDV = f_dv
		//
		mov		eax,[f_du]
		mov		ebx,[f_dv]
		mov		[fDU],eax
		mov		[fDV],ebx

DONE_WITH_COPY:
		//
		// fDeltaXIntensity = f_di_invdx;
		//
		// Clamp(fDeltaXIntensity, 16.0f);
		//
		// Convert to fixed point representation.
		//
		// fxDeltaXIntensity.fxFromFloat(fDeltaXIntensity);
		//
		fld		f_di_invdx						// Load intensity (assuming no clamp).
		fadd	dFloatToFixed16					// Add integer conversion constant.

		mov		eax,f_di_invdx					// Load value.
		mov		ecx,fSIXTEEN					// Load clamp value.

		mov		ebx,eax							// Copy value for sign.
		mov		edx,eax							// Keep orignal value.

		and		eax,0x7fffffff					// Mask off sign bit.
		and		ebx,0x80000000					// Keep just the sign bit.

		cmp		eax,ecx							// Compare against 16.0
		jle		short NO_INTENSITY_CLAMP
		
		//
		// Clamp value. This is a relatively rare case.
		//
		fcomp	st(0)							// Dump unclamped value.

		mov		edx,ecx							// Clamp value.

		or		edx,ebx							// Keep sign bit.

		mov		f_di_invdx,edx					// Save clamped value.

		fld		f_di_invdx						// Load clamped intensity.
		fadd	dFloatToFixed16					// Add integer conversion constant.
		// stall(3)
		
NO_INTENSITY_CLAMP:
		fstp	d_temp_a						// Store fixed point value.

		mov		fDeltaXIntensity,edx			// Save updated, possibly clamped value.
		mov		eax,dword ptr[d_temp_a]			// Load fixed point value.

		mov		fxDeltaXIntensity,eax			// Save fixed point value.
	
SKIP_INTENSITY_UPDATE:
	}

	return true;

RETURN_FALSE:
	return false;
}


//*****************************************************************************************
//
inline bool GenericInitGradientDataPlanar(CDrawPolygonBase* poly, bool b_altpersp)
//
// Sets up polygon-wide rasterising info.
//
// Returns:
//		Whether the polygon is large enough to render.
//
//**************************************
{
	// These are saved for the best edge pair.
	SRenderVertex *prv_a;
	SRenderVertex *prv_b;
	SRenderVertex *prv_c;
	float f_dx;
	float f_invdx;

	__asm
	{
		//
		// Find the first edge pair with an area greater than 25 pixels, or if no 
		// edge pair has an area greater than 25 pixels find the maximum.
		//

		// Set starting vertices [N-2, N-1, 0].
		lea		edi,[arvRasterVertices]			// prv_c = edi = a[n]
		mov		ecx,[iNumRasterVertices]

		mov		eax,SIZE SRenderVertex
		dec		ecx

		imul	eax,ecx

		lea		edx,[edi + eax]					// prv_b = esi = a[n-1]

		mov		esi,edx
		sub		edx,SIZE SRenderVertex			// prv_a = edx = a[n-2]

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		fld		[edi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yac
		fld		[esi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xab,f_yac
		fld		[esi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yab,f_xab,f_yac
		fld		[edi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xac,f_yab,f_xab,f_yac
		fxch	st(3)							// f_yac,f_yab,f_xab,f_xac
		fmulp	st(2),st(0)						// f_yab,f_xab*f_yac,f_xac
		// 1 cycle stall
		fmulp	st(2),st(0)						// f_xab*f_yac,f_xac*f_yab
		// 2 cycle stall
		fsubrp	st(1),st(0)						// f_xab*f_yac - f_xac*f_yab 
		// 3 cycle stall

		mov		[prv_c],edi
		mov		[prv_b],esi
		mov		[prv_a],edx

		fstp	[f_dx]							// Assume this is best.

		mov		eax,[f_dx]						
		mov		ebx,[fNegativeFifty]

		cmp		eax,ebx							// If our best < -50, then stop
		ja		EXIT_LOOP

EDGE_LOOP:
		mov		edx,esi							// prv_a = prv_b
		mov		esi,edi							// prv_b = prv_c

		add		edi,SIZE SRenderVertex			// prv_c = next vertex

		//
		// float f_yab = (prv_b->v3Screen.tY - prv_a->v3Screen.tY);
		// float f_yac = (prv_c->v3Screen.tY - prv_a->v3Screen.tY);
		//
		// float f_dx  = (prv_b->v3Screen.tX - prv_a->v3Screen.tX) * f_yac -
		//				 (prv_c->v3Screen.tX - prv_a->v3Screen.tX) * f_yab;
		//
		fld		[esi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xab
		fld		[edi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yac,f_xab
		fld		[esi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// f_yab,f_yac,f_xab
		fld		[edi]SRenderCoord.v3Screen.tX
		fsub	[edx]SRenderCoord.v3Screen.tX	// f_xac,f_yab,f_yac,f_xab
		fxch	st(2)							// f_yac,f_yab,f_xac,f_xab
		fmulp	st(3),st(0)						// f_yab,f_xac,f_xab*f_yac
		// 1 cycle stall
		fmulp	st(1),st(0)						// f_xac*f_yab,f_xab*f_yac
		// 2 cycle stall
		fsubp	st(1),st(0)						// f_xab*f_yac - f_xac*f_yab
		// 3 cycle stall
		fcom	[f_dx]							// Is this smaller than f_dx
		fnstsw	ax
		
		test	eax,0x0100						// ST < Operand
		jnz		NEW_MINIMUM

		// No, discard f_dx
		fcomp	st(0)

		dec		ecx
		jnz		EDGE_LOOP

		jmp		short EXIT_LOOP

NEW_MINIMUM:
		// Yes, save f_dx and vertex pointers.
		fstp	[f_dx]

		mov		[prv_a],edx
		mov		[prv_b],esi

		mov		[prv_c],edi
		mov		eax,[f_dx]

		cmp		eax,ebx							// If our best < -50, then stop
		ja		EXIT_LOOP

		dec		ecx
		jnz		EDGE_LOOP

EXIT_LOOP:
		//
		// Is the polygon too small to draw?
		// if (f_dx >= fMAX_NEG_AREA) goto RETURN_FALSE;
		//
		mov		eax,[f_dx]
		mov		ebx,[fMAX_NEG_AREA]

		cmp		eax,ebx
		jbe		RETURN_FALSE

		//
		// Initialize the texture gradients based on the best edge.
		//
		// float f_invdx = 1.0f / f_dx;
		// float f_yab_invdx = f_yab * f_invdx;
		// float f_yac_invdx = f_yac * f_invdx;
		//
		// float f_duinvz = (prv_b->tcTex.tX - prv_a->tcTex.tX) * f_yac_invdx -
		//				    (prv_c->tcTex.tX - prv_a->tcTex.tX) * f_yab_invdx;
		//
		// float f_dvinvz = (prv_b->tcTex.tY - prv_a->tcTex.tY) * f_yac_invdx -
		//				    (prv_c->tcTex.tY - prv_a->tcTex.tY) * f_yab_invdx;
		//
		// float f_dinvz =  (prv_b->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yac_invdx -
		//				    (prv_c->v3Screen.tZ - prv_a->v3Screen.tZ) * f_yab_invdx;
		//
		mov		edx,[prv_a]
		mov		esi,[prv_b]

		mov		ebx,dword ptr[f_dx]
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		// One iteration of Newton-Raphson doubles the number of bits of accuary.
		fld		[f_dx]

		add		eax,dword ptr[i4InverseMantissa + ebx*4]
		mov		edi,[prv_c]

		mov		dword ptr[f_invdx],eax

		fmul	[f_invdx]						// (v*r)
		fld		[esi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// yab (v*r)
		fxch	st(1)							// (v*r) yab
		fsubr	[fTWO]							// (2.0 - v*r) yab
		fld		[edi]SRenderCoord.v3Screen.tY
		fsub	[edx]SRenderCoord.v3Screen.tY	// yac (2.0 - v*r) yab
		fxch	st(1)							// (2.0 - v*r) yac yab
		fmul	[f_invdx]						// (2.0 - v*r)*r yac yab
		fld		[esi]SRenderVertex.tcTex.tX
		fsub	[edx]SRenderVertex.tcTex.tX		// uab invdx yac yab
		fxch	st(2)							// yac invdx uab yab
		fmul	st(0),st(1)						// yac*invdx invdx uab yab
		fld		[edi]SRenderVertex.tcTex.tX
		fsub	[edx]SRenderVertex.tcTex.tX		// uac yac*invdx invdx uab yab
		fxch	st(2)							// invdx yac*invdx uac uab yab
		fmulp	st(4),st(0)						// yac*invdx uac uab yab*invdx
		fld		[esi]SRenderVertex.tcTex.tY
		fsub	[edx]SRenderVertex.tcTex.tY		// vab yac_invdx uac uab yab_invdx
		fxch	st(1)							// yac_invdx vab uac uab yab_invdx
		fmul	st(3),st(0)						// yac_invdx vab uac uab*yac_invdx yab_invdx
		fld		[edi]SRenderVertex.tcTex.tY
		fsub	[edx]SRenderVertex.tcTex.tY		// vac yac_invdx vab uac uab*yac_invdx yab_invdx
		fxch	st(3)							// uac yac_invdx vab vac uab*yac_invdx yab_invdx
		fmul	st(0),st(5)						// uac*yab_invdx yac_invdx vab vac uab*yac_invdx yab_invdx
		fld		[esi]SRenderCoord.v3Screen.tZ
		fsub	[edx]SRenderCoord.v3Screen.tZ	// zab uac*yab_invdx yac_invdx vab vac uab*yac_invdx yab_invdx
		fxch	st(3)							// vab uac*yab_invdx yac_invdx zab vac uab*yac_invdx yab_invdx
		fmul	st(0),st(2)						// vab*yac_invdx uac*yab_invdx yac_invdx zab vac uab*yac_invdx yab_invdx
		fxch	st(5)							// uab*yac_invdx uac*yab_invdx yac_invdx zab vac vab*yac_invdx yab_invdx
		fsubrp	st(1),st(0)						// duinvz yac_invdx zab vac vab*yac_invdx yab_invdx
		fxch	st(3)							// vac yac_invdx zab duinvz vab*yac_invdx yab_invdx
		fmul	st(0),st(5)						// vac*yab_invdx yac_invdx zab duinvz vab*yac_invdx yab_invdx
		fld		[edi]SRenderCoord.v3Screen.tZ
		fsub	[edx]SRenderCoord.v3Screen.tZ	// zac vac*yab_invdx yac_invdx zab duinvz vab*yac_invdx yab_invdx
		fxch	st(3)							// zab vac*yab_invdx yac_invdx zac duinvz vab*yac_invdx yab_invdx
		fmulp	st(2),st(0)						// vac*yab_invdx zab*yac_invdx zac duinvz vab*yac_invdx yab_invdx
		fsubp	st(4),st(0)						// zab*yac_invdx zac duinvz dvinvz yab_invdx
		fxch	st(1)							// zac zab*yac_invdx duinvz dvinvz yab_invdx
		fmulp	st(4),st(0)						// zab*yac_invdx duinvz dvinvz zac*yab_invdx
		fxch	st(1)							// duinvz zab*yac_invdx dvinvz zac*yab_invdx
		fstp	[fDUInvZ]						// zab*yac_invdx dvinvz zac*yab_invdx
		fsubrp	st(2),st(0)						// dvinvz dzinvz
		fstp	[fDVInvZ]
		fstp	[fDInvZ]
	}

	// Calculate the subdivision length with respect to X.
	iSubdivideLen    = persetSettings.iGetSubdivisionLen(fDInvZ, b_altpersp);
	fInvSubdivideLen = fUnsignedInverseInt(iSubdivideLen);

	return true;

RETURN_FALSE:
	return false;
}


#endif  // #else // #if (TARGET_PROCESSOR == PROCESSOR_K6_3D)


//*****************************************************************************************
//
// InitlizePolygonData routines for each scanline type.
//
//*****************************************************************************************

//*****************************************************************************************
//
void InitializePolygonData(TCopyPersp& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TCopyPersp.
//
//**************************************
{
	GenericInitializePolygonData(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TCopyPerspTrans& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TCopyPerspTrans.
//
//**************************************
{
	GenericInitializePolygonData(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TCopyTerrainPersp& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TCopyTerrainPersp.
//
//**************************************
{
	GenericInitializePolygonData(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TTexturePersp& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TTexturePersp.
//
//**************************************
{
	GenericInitializePolygonDataFlat(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TTexturePerspTrans& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TTexturePerspTrans.
//
//**************************************
{
	GenericInitializePolygonDataFlat(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TTexturePerspGour& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TTexturePerspGour.
//
//**************************************
{
	GenericInitializePolygonDataGour(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TTexturePerspTransGour& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TTexturePerspTransGour.
//
//**************************************
{
	GenericInitializePolygonDataGour(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TBumpPersp& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TBumpPersp.
//
//**************************************
{
	GenericInitializePolygonDataBump(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TBumpPerspTrans& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TBumpPerspTrans.
//
//**************************************
{
	GenericInitializePolygonDataBump(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TGFogPersp& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TGFogPersp.
//
//**************************************
{
	GenericInitializePolygonDataGour(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TCopyLinear& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TCopyLinear.
//
//**************************************
{
	GenericInitializePolygonDataLinear(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TCopyLinearTrans& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TCopyLinearTrans.
//
//**************************************
{
	GenericInitializePolygonDataLinear(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TCopyTerrainLinear& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TCopyTerrainLinear.
//
//**************************************
{
	GenericInitializePolygonDataLinear(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TTextureLinear& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TTextureLinear.
//
//**************************************
{
	GenericInitializePolygonDataLinearFlat(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TTextureLinearTrans& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TTextureLinearTrans.
//
//**************************************
{
	GenericInitializePolygonDataLinearFlat(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TTextureLinearGour& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TTextureLinearGour.
//
//**************************************
{
	GenericInitializePolygonDataLinearGour(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TTextureLinearTransGour& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TTextureLinearTransGour.
//
//**************************************
{
	GenericInitializePolygonDataLinearGour(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TBumpLinear& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TBumpLinear.
//
//**************************************
{
	GenericInitializePolygonDataLinearBump(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TBumpLinearTrans& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TBumpLinearTrans.
//
//**************************************
{
	GenericInitializePolygonDataLinearBump(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TGFogLinear& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TGFogLinear.
//
//**************************************
{
	GenericInitializePolygonDataLinearGour(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TTexNoClutLinearTrans& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TTexNoClutLinearTrans.
//
//**************************************
{
#if (NORMALIZE_TERRAIN_OBJECT_TEXTURE_COORDS)

	CPArray<SRenderVertex*> parv_all	= rpoly->paprvPolyVertices;
	ptr_const<CTexture>     ptex		= rpoly->ptexTexture;
	TClutVal				cv_face		= rpoly->cvFace;
	int                     i_mip_level	= rpoly->iMipLevel;

	// Copy screen coords to our local vertices.
	Assert(parv_all.uLen <= iMAX_RASTER_VERTICES);

	// Initialize polygon-wide variables for index coordinates.
	lineData.indCoord.InitializePolygonData(ptex, i_mip_level);

	// Set the base address for the clut.
	lineData.SetClutAddress(ptex);

	if (bClampUV)
	{
		for (int i = 0; i < parv_all.uLen; i++)
		{
			arvRasterVertices[i].v3Screen = parv_all[i]->v3Screen;
			arvRasterVertices[i].iYScr = iPosFloatCast(arvRasterVertices[i].v3Screen.tY);
			arvRasterVertices[i].tcTex.tX = parv_all[i]->tcTex.tX * fTexWidth  + fTexEdgeTolerance;
			arvRasterVertices[i].tcTex.tY = parv_all[i]->tcTex.tY * fTexHeight + fTexEdgeTolerance;
		}
	}
	else
	{
		// Find offsets to normalize range of texture co-ordinates.
		float fMinU = parv_all[0]->tcTex.tX;
		float fMinV = parv_all[0]->tcTex.tY;

		int i;
		for (i = 1; i < parv_all.uLen; i++)
		{
			if (fMinU > parv_all[i]->tcTex.tX)
				fMinU = parv_all[i]->tcTex.tX;
			if (fMinV > parv_all[i]->tcTex.tY)
				fMinV = parv_all[i]->tcTex.tY;
		}

		float fAdjU = floor(fMinU);
		float fAdjV = floor(fMinV);

		for (i = 0; i < parv_all.uLen; i++)
		{
			arvRasterVertices[i].v3Screen = parv_all[i]->v3Screen;
			arvRasterVertices[i].iYScr = iPosFloatCast(arvRasterVertices[i].v3Screen.tY);
			arvRasterVertices[i].tcTex.tX = (parv_all[i]->tcTex.tX - fAdjU) * fTexWidth;
			arvRasterVertices[i].tcTex.tY = (parv_all[i]->tcTex.tY - fAdjV) * fTexHeight;
		}
	}

#else

	GenericInitializePolygonDataLinear(rpoly);

#endif
}

//*****************************************************************************************
//
void InitializePolygonData(TTexNoClutLinear& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TTexNoClutLinear.
//
//**************************************
{
#if (NORMALIZE_TERRAIN_OBJECT_TEXTURE_COORDS)

	CPArray<SRenderVertex*> parv_all	= rpoly->paprvPolyVertices;
	ptr_const<CTexture>     ptex		= rpoly->ptexTexture;
	TClutVal				cv_face		= rpoly->cvFace;
	int                     i_mip_level	= rpoly->iMipLevel;

	// Copy screen coords to our local vertices.
	Assert(parv_all.uLen <= iMAX_RASTER_VERTICES);

	// Initialize polygon-wide variables for index coordinates.
	lineData.indCoord.InitializePolygonData(ptex, i_mip_level);

	// Set the base address for the clut.
	lineData.SetClutAddress(ptex);

	if (bClampUV)
	{
		for (int i = 0; i < parv_all.uLen; i++)
		{
			arvRasterVertices[i].v3Screen = parv_all[i]->v3Screen;
			arvRasterVertices[i].iYScr = iPosFloatCast(arvRasterVertices[i].v3Screen.tY);
			arvRasterVertices[i].tcTex.tX = parv_all[i]->tcTex.tX * fTexWidth  + fTexEdgeTolerance;
			arvRasterVertices[i].tcTex.tY = parv_all[i]->tcTex.tY * fTexHeight + fTexEdgeTolerance;
		}
	}
	else
	{
		// Find offsets to normalize range of texture co-ordinates.
		float fMinU = parv_all[0]->tcTex.tX;
		float fMinV = parv_all[0]->tcTex.tY;

		int i;
		for (i = 1; i < parv_all.uLen; i++)
		{
			if (fMinU > parv_all[i]->tcTex.tX)
				fMinU = parv_all[i]->tcTex.tX;
			if (fMinV > parv_all[i]->tcTex.tY)
				fMinV = parv_all[i]->tcTex.tY;
		}

		float fAdjU = floor(fMinU);
		float fAdjV = floor(fMinV);

		for (i = 0; i < parv_all.uLen; i++)
		{
			arvRasterVertices[i].v3Screen = parv_all[i]->v3Screen;
			arvRasterVertices[i].iYScr = iPosFloatCast(arvRasterVertices[i].v3Screen.tY);
			arvRasterVertices[i].tcTex.tX = (parv_all[i]->tcTex.tX - fAdjU) * fTexWidth;
			arvRasterVertices[i].tcTex.tY = (parv_all[i]->tcTex.tY - fAdjV) * fTexHeight;
		}
	}

#else

	GenericInitializePolygonDataLinear(rpoly);

#endif
}

//*****************************************************************************************
//
void InitializePolygonData(TShadowTrans8& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TShadowTrans8.
//
//**************************************
{
	GenericInitializePolygonDataLinear(rpoly);
}

//*****************************************************************************************
//
void InitializePolygonData(TShadowTrans32& lineData, CRenderPolygon* rpoly)
//
// Scanline data initialization specialized for TShadowTrans32.
//
//**************************************
{
	GenericInitializePolygonDataLinear(rpoly);
}


#if (BILINEAR_FILTER)

void InitializePolygonData(TCopyPerspFilter& lineData, CRenderPolygon* rpoly)
{
	GenericInitializePolygonData(rpoly);
}

void InitializePolygonData(TCopyLinearFilter& lineData, CRenderPolygon* rpoly)
{
	GenericInitializePolygonDataLinear(rpoly);
}

#endif

void InitializePolygonData(TStippleTexPersp& lineData, CRenderPolygon* rpoly)
{
	GenericInitializePolygonData(rpoly);
}

void InitializePolygonData(TStippleTexLinear& lineData, CRenderPolygon* rpoly)
{
	GenericInitializePolygonDataLinear(rpoly);
}

void InitializePolygonData(TWaterPersp& lineData, CRenderPolygon* rpoly)
{
	GenericInitializePolygonData(rpoly);
}

void InitializePolygonData(TWaterLinear& lineData, CRenderPolygon* rpoly)
{
	GenericInitializePolygonDataLinear(rpoly);
}

void InitializePolygonData(TAlphaTexPersp& lineData, CRenderPolygon* rpoly)
{
	GenericInitializePolygonData(rpoly);
}

void InitializePolygonData(TAlphaTexLinear& lineData, CRenderPolygon* rpoly)
{
	GenericInitializePolygonDataLinear(rpoly);
}


//*****************************************************************************************
//
// InitializeEdge routines for each scanline type.
//
//*****************************************************************************************

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TCopyPersp>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,	// Starting coordinate of edge.
	SRenderVertex* prv_to		// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TCopyPersp>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TCopyPerspTrans>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,		// Starting coordinate of edge.
	SRenderVertex* prv_to			// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TCopyPerspTrans>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TCopyTerrainPersp>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,		// Starting coordinate of edge.
	SRenderVertex* prv_to			// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TCopyTerrainPersp>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TTexturePersp>& edge,		// The edge to initialize.
	SRenderVertex* prv_from,		// Starting coordinate of edge.
	SRenderVertex* prv_to			// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTexturePersp>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TTexturePerspTrans>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,			// Starting coordinate of edge.
	SRenderVertex* prv_to				// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTexturePerspTrans>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TTexturePerspGour>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,		// Starting coordinate of edge.
	SRenderVertex* prv_to			// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTexturePerspGour>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TTexturePerspTransGour>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTexturePerspTransGour>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TBumpPersp>& edge,				// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TBumpPersp>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TBumpPerspTrans>& edge,			// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TBumpPerspTrans>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TGFogPersp>& edge,				// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TGFogPersp>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TCopyLinear>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,	// Starting coordinate of edge.
	SRenderVertex* prv_to		// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TCopyLinear>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TCopyLinearTrans>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,		// Starting coordinate of edge.
	SRenderVertex* prv_to			// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TCopyLinearTrans>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}


//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TCopyTerrainLinear>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,		// Starting coordinate of edge.
	SRenderVertex* prv_to			// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TCopyTerrainLinear>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TTextureLinear>& edge,		// The edge to initialize.
	SRenderVertex* prv_from,		// Starting coordinate of edge.
	SRenderVertex* prv_to			// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTextureLinear>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TTextureLinearTrans>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,			// Starting coordinate of edge.
	SRenderVertex* prv_to				// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTextureLinearTrans>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TTextureLinearGour>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,		// Starting coordinate of edge.
	SRenderVertex* prv_to			// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTextureLinearGour>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TTextureLinearTransGour>& edge,	// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTextureLinearTransGour>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TBumpLinear>& edge,				// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTextureLinearTransGour>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TBumpLinearTrans>& edge,			// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTextureLinearTransGour>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TGFogLinear>& edge,				// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TGFogLinear>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TTexNoClutLinear>& edge,			// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTexNoClutLinear>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TTexNoClutLinearTrans>& edge,		// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TTexNoClutLinearTrans>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TShadowTrans8>& edge,				// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TShadowTrans8>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

//*****************************************************************************************
//
void InitializeEdge
(
	CEdge<TShadowTrans32>& edge,			// The edge to initialize.
	SRenderVertex* prv_from,				// Starting coordinate of edge.
	SRenderVertex* prv_to					// End coordinate of edge.
)
//
// Specialzed replacement for CEdge<TShadowTrans32>::IntializeEdge
//
//**************************************
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

#if (BILINEAR_FILTER)

void InitializeEdge(CEdge<TCopyPerspFilter>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to)
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

void InitializeEdge(CEdge<TCopyLinearFilter>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to)
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

#endif

void InitializeEdge(CEdge<TStippleTexPersp>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to)
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

void InitializeEdge(CEdge<TStippleTexLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to)
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

void InitializeEdge(CEdge<TWaterPersp>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to)
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

void InitializeEdge(CEdge<TWaterLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to)
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

void InitializeEdge(CEdge<TAlphaTexPersp>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to)
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}

void InitializeEdge(CEdge<TAlphaTexLinear>& edge, SRenderVertex* prv_from, SRenderVertex* prv_to)
{
	GenericInitializeEdge(edge, prv_from, prv_to);
}


//*****************************************************************************************
//
// bInitTriangleDataEx routines for each scanline type.
//
//*****************************************************************************************

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TCopyPersp>*			poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleData(poly, prv_a, prv_b, prv_c, b_update, true);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TCopyPerspTrans>*		poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleData(poly, prv_a, prv_b, prv_c, b_update, false);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TCopyTerrainPersp>*	poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleData(poly, prv_a, prv_b, prv_c, b_update, true);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TTexturePersp>*		poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleData(poly, prv_a, prv_b, prv_c, b_update, false);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TTexturePerspTrans>*	poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleData(poly, prv_a, prv_b, prv_c, b_update, false);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TTexturePerspGour>*	poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataGour(poly, prv_a, prv_b, prv_c, b_update, false);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TTexturePerspTransGour>*	poly,
	SRenderVertex*							prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*							prv_b,
	SRenderVertex*							prv_c,
	bool									b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataGour(poly, prv_a, prv_b, prv_c, b_update, false);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TBumpPersp>*			poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleData(poly, prv_a, prv_b, prv_c, b_update, false);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TBumpPerspTrans>*		poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleData(poly, prv_a, prv_b, prv_c, b_update, false);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TGFogPersp>*			poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataGour(poly, prv_a, prv_b, prv_c, b_update, true);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TCopyLinear>*			poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TCopyLinearTrans>*		poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TCopyTerrainLinear>*	poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TTextureLinear>*		poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TTextureLinearTrans>*	poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TTextureLinearGour>*	poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinearGour(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TTextureLinearTransGour>*	poly,
	SRenderVertex*							prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*							prv_b,
	SRenderVertex*							prv_c,
	bool									b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinearGour(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TBumpLinear>*			poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TBumpLinearTrans>*		poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TGFogLinear>*			poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinearGour(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TTexNoClutLinear>*		poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TTexNoClutLinearTrans>*	poly,
	SRenderVertex*							prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*							prv_b,
	SRenderVertex*							prv_c,
	bool									b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TShadowTrans8>*		poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

//*****************************************************************************************
//
bool bInitTriangleDataEx
(
	CDrawPolygon<TShadowTrans32>*		poly,
	SRenderVertex*						prv_a,		// 3 vertices making up this triangle.	
	SRenderVertex*						prv_b,
	SRenderVertex*						prv_c,
	bool								b_update
)
//
// Sets up triangle-wide rasterising info.
//
// Returns:
//		Whether the triangle is large enough to render.
//
//**************************************
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}


#if (BILINEAR_FILTER)

bool bInitTriangleDataEx
(
	CDrawPolygon<TCopyPerspFilter>* poly, 
	SRenderVertex* prv_a, 
	SRenderVertex* prv_b, 
	SRenderVertex* prv_c, 
	bool b_update
)
{
	return GenericInitTriangleData(poly, prv_a, prv_b, prv_c, b_update, false);
}

bool bInitTriangleDataEx
(
	CDrawPolygon<TCopyLinearFilter>* poly, 
	SRenderVertex* prv_a, 
	SRenderVertex* prv_b, 
	SRenderVertex* prv_c, 
	bool b_update
)
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

#endif

bool bInitTriangleDataEx
(
	CDrawPolygon<TStippleTexPersp>* poly, 
	SRenderVertex* prv_a, 
	SRenderVertex* prv_b, 
	SRenderVertex* prv_c, 
	bool b_update
)
{
	return GenericInitTriangleData(poly, prv_a, prv_b, prv_c, b_update, false);
}

bool bInitTriangleDataEx
(
	CDrawPolygon<TStippleTexLinear>* poly, 
	SRenderVertex* prv_a, 
	SRenderVertex* prv_b, 
	SRenderVertex* prv_c, 
	bool b_update
)
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

bool bInitTriangleDataEx
(
	CDrawPolygon<TWaterPersp>* poly, 
	SRenderVertex* prv_a, 
	SRenderVertex* prv_b, 
	SRenderVertex* prv_c, 
	bool b_update
)
{
	return GenericInitTriangleData(poly, prv_a, prv_b, prv_c, b_update, false);
}

bool bInitTriangleDataEx
(
	CDrawPolygon<TWaterLinear>* poly, 
	SRenderVertex* prv_a, 
	SRenderVertex* prv_b, 
	SRenderVertex* prv_c, 
	bool b_update
)
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}

bool bInitTriangleDataEx
(
	CDrawPolygon<TAlphaTexPersp>* poly, 
	SRenderVertex* prv_a, 
	SRenderVertex* prv_b, 
	SRenderVertex* prv_c, 
	bool b_update
)
{
	return GenericInitTriangleData(poly, prv_a, prv_b, prv_c, b_update, false);
}

bool bInitTriangleDataEx
(
	CDrawPolygon<TAlphaTexLinear>* poly, 
	SRenderVertex* prv_a, 
	SRenderVertex* prv_b, 
	SRenderVertex* prv_c, 
	bool b_update
)
{
	return GenericInitTriangleDataLinear(poly, prv_a, prv_b, prv_c, b_update);
}


//*****************************************************************************************
//
// bInitGradientDataPlanarEx routines for each scanline type.
//
//*****************************************************************************************

//*****************************************************************************************
//
bool bInitGradientDataPlanarEx(CDrawPolygon<TCopyPersp>* poly)
//
// Sets up polygon-wide rasterising info.
//
// Returns:
//		Whether the polygon is large enough to render.
//
//**************************************
{
	return GenericInitGradientDataPlanar(poly, true);
}

//*****************************************************************************************
//
bool bInitGradientDataPlanarEx(CDrawPolygon<TCopyPerspTrans>* poly)
//
// Sets up polygon-wide rasterising info.
//
// Returns:
//		Whether the polygon is large enough to render.
//
//**************************************
{
	return GenericInitGradientDataPlanar(poly, false);
}

//*****************************************************************************************
//
bool bInitGradientDataPlanarEx(CDrawPolygon<TCopyTerrainPersp>* poly)
//
// Sets up polygon-wide rasterising info.
//
// Returns:
//		Whether the polygon is large enough to render.
//
//**************************************
{
	return GenericInitGradientDataPlanar(poly, true);
}

//*****************************************************************************************
//
bool bInitGradientDataPlanarEx(CDrawPolygon<TTexturePersp>* poly)
//
// Sets up polygon-wide rasterising info.
//
// Returns:
//		Whether the polygon is large enough to render.
//
//**************************************
{
	return GenericInitGradientDataPlanar(poly, false);
}

//*****************************************************************************************
//
bool bInitGradientDataPlanarEx(CDrawPolygon<TTexturePerspTrans>* poly)
//
// Sets up polygon-wide rasterising info.
//
// Returns:
//		Whether the polygon is large enough to render.
//
//**************************************
{
	return GenericInitGradientDataPlanar(poly, false);
}

//*****************************************************************************************
//
bool bInitGradientDataPlanarEx(CDrawPolygon<TBumpPersp>* poly)
//
// Sets up polygon-wide rasterising info.
//
// Returns:
//		Whether the polygon is large enough to render.
//
//**************************************
{
	return GenericInitGradientDataPlanar(poly, false);
}

//*****************************************************************************************
//
bool bInitGradientDataPlanarEx(CDrawPolygon<TBumpPerspTrans>* poly)
//
// Sets up polygon-wide rasterising info.
//
// Returns:
//		Whether the polygon is large enough to render.
//
//**************************************
{
	return GenericInitGradientDataPlanar(poly, false);
}

#if (BILINEAR_FILTER)

bool bInitGradientDataPlanarEx(CDrawPolygon<TCopyPerspFilter>* poly)
{
	return GenericInitGradientDataPlanar(poly, false);
}

#endif

bool bInitGradientDataPlanarEx(CDrawPolygon<TStippleTexPersp>* poly)
{
	return GenericInitGradientDataPlanar(poly, false);
}

bool bInitGradientDataPlanarEx(CDrawPolygon<TWaterPersp>* poly)
{
	return GenericInitGradientDataPlanar(poly, false);
}

bool bInitGradientDataPlanarEx(CDrawPolygon<TAlphaTexPersp>* poly)
{
	return GenericInitGradientDataPlanar(poly, false);
}


#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)


//*****************************************************************************************
//
// InitializeForWalkEx routines for each scanline type.
//
//*****************************************************************************************

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TCopyPersp>* poly, CEdge<TCopyPersp>* pedge)
{
	GenericInitializeForWalk(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TCopyPerspTrans>* poly, CEdge<TCopyPerspTrans>* pedge)
{
	GenericInitializeForWalk(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TCopyTerrainPersp>* poly, CEdge<TCopyTerrainPersp>* pedge)
{
	GenericInitializeForWalk(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TTexturePersp>* poly, CEdge<TTexturePersp>* pedge)
{
	GenericInitializeForWalk(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TTexturePerspTrans>* poly, CEdge<TTexturePerspTrans>* pedge)
{
	GenericInitializeForWalk(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TTexturePerspGour>* poly, CEdge<TTexturePerspGour>* pedge)
{
	GenericInitializeForWalkGour(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TTexturePerspTransGour>* poly, CEdge<TTexturePerspTransGour>* pedge)
{
	GenericInitializeForWalkGour(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TBumpPersp>* poly, CEdge<TBumpPersp>* pedge)
{
	GenericInitializeForWalk(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TBumpPerspTrans>* poly, CEdge<TBumpPerspTrans>* pedge)
{
	GenericInitializeForWalk(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TGFogPersp>* poly, CEdge<TGFogPersp>* pedge)
{
	GenericInitializeForWalkGour(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TCopyLinear>* poly, CEdge<TCopyLinear>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TCopyLinearTrans>* poly, CEdge<TCopyLinearTrans>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TCopyTerrainLinear>* poly, CEdge<TCopyTerrainLinear>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TTextureLinear>* poly, CEdge<TTextureLinear>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TTextureLinearTrans>* poly, CEdge<TTextureLinearTrans>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TTextureLinearGour>* poly, CEdge<TTextureLinearGour>* pedge)
{
	GenericInitializeForWalkLinearGour(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TTextureLinearTransGour>* poly, CEdge<TTextureLinearTransGour>* pedge)
{
	GenericInitializeForWalkLinearGour(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TBumpLinear>* poly, CEdge<TBumpLinear>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TBumpLinearTrans>* poly, CEdge<TBumpLinearTrans>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TGFogLinear>* poly, CEdge<TGFogLinear>* pedge)
{
	GenericInitializeForWalkLinearGour(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TTexNoClutLinear>* poly, CEdge<TTexNoClutLinear>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TTexNoClutLinearTrans>* poly, CEdge<TTexNoClutLinearTrans>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TShadowTrans8>* poly, CEdge<TShadowTrans8>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

//*****************************************************************************************
//
void InitializeForWalkEx(CDrawPolygon<TShadowTrans32>* poly, CEdge<TShadowTrans32>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

#if (BILINEAR_FILTER)

void InitializeForWalkEx(CDrawPolygon<TCopyPerspFilter>* poly, CEdge<TCopyPerspFilter>* pedge)
{
	// Use the same routine as TCopyPersp.
	GenericInitializeForWalk(reinterpret_cast<CDrawPolygon<TCopyPersp>*>(poly), reinterpret_cast<CEdge<TCopyPersp>*>(pedge));
}

void InitializeForWalkEx(CDrawPolygon<TCopyLinearFilter>* poly, CEdge<TCopyLinearFilter>* pedge)
{
	// Use the same routine as TCopyLinear.
	GenericInitializeForWalkLinear(reinterpret_cast<CDrawPolygon<TCopyLinear>*>(poly), reinterpret_cast<CEdge<TCopyLinear>*>(pedge));
}

#endif

void InitializeForWalkEx(CDrawPolygon<TStippleTexPersp>* poly, CEdge<TStippleTexPersp>* pedge)
{
	GenericInitializeForWalk(poly, pedge);
}

void InitializeForWalkEx(CDrawPolygon<TStippleTexLinear>* poly, CEdge<TStippleTexLinear>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

void InitializeForWalkEx(CDrawPolygon<TWaterPersp>* poly, CEdge<TWaterPersp>* pedge)
{
	GenericInitializeForWalk(poly, pedge);
}

void InitializeForWalkEx(CDrawPolygon<TWaterLinear>* poly, CEdge<TWaterLinear>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}

void InitializeForWalkEx(CDrawPolygon<TAlphaTexPersp>* poly, CEdge<TAlphaTexPersp>* pedge)
{
	GenericInitializeForWalk(poly, pedge);
}

void InitializeForWalkEx(CDrawPolygon<TAlphaTexLinear>* poly, CEdge<TAlphaTexLinear>* pedge)
{
	GenericInitializeForWalkLinear(poly, pedge);
}


//*****************************************************************************************
//
// DoneDrawingPolygon routines for each scanline type.
//
//*****************************************************************************************

void DoneDrawingPolygon(CDrawPolygon<TCopyPersp>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TCopyPerspTrans>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TCopyTerrainPersp>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TTexturePersp>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TTexturePerspTrans>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TTexturePerspGour>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TTexturePerspTransGour>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TBumpPersp>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TBumpPerspTrans>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TGFogPersp>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TCopyLinear>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TCopyLinearTrans>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TCopyTerrainLinear>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TTextureLinear>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TTextureLinearTrans>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TTextureLinearGour>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TTextureLinearTransGour>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TBumpLinear>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TBumpLinearTrans>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TGFogLinear>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TTexNoClutLinear>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TTexNoClutLinearTrans>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TShadowTrans8>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TShadowTrans32>* poly)
{
	// Empty MMX state.
	__asm femms
}

#if (BILINEAR_FILTER)

void DoneDrawingPolygon(CDrawPolygon<TCopyPerspFilter>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TCopyLinearFilter>* poly)
{
	// Empty MMX state.
	__asm femms
}

#endif

void DoneDrawingPolygon(CDrawPolygon<TStippleTexPersp>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TStippleTexLinear>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TWaterPersp>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TWaterLinear>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TAlphaTexPersp>* poly)
{
	// Empty MMX state.
	__asm femms
}

void DoneDrawingPolygon(CDrawPolygon<TAlphaTexLinear>* poly)
{
	// Empty MMX state.
	__asm femms
}

#endif	// #if (TARGET_PROCESSOR == PROCESSOR_K6_3D)
