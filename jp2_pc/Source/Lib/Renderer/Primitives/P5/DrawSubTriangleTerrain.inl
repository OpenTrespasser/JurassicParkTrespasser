#pragma once

/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function for terrain.
 *		Optimized for the Pentium Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P5/DrawSubTriangleTerrain.cpp                 $
 * 
 * 8     98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 7     98.07.24 2:43p Mmouni
 * Sped up 2x2 dither primitives.
 * 
 * 6     98.04.08 8:42p Mmouni
 * Added fogged dithered flat shaded primitive.
 * 
 * 5     98.01.26 3:42p Mmouni
 * Sped up inner loops for 4x4 dithered fogged terrain.
 * 
 * 4     97.11.25 7:04p Mmouni
 * Removed tiling code from fogged terrain drawing primitives.
 * 
 * 3     97.11.11 9:48p Mmouni
 * Optimized for Pentium.
 * 
 * 2     97.11.07 5:44p Mmouni
 * Added dithering support.
 * 
 * 1     97/11/06 4:48p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Fog.hpp"
#include "Lib/Renderer/LightBlend.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"


//
// The number of integral bits of the fog shading values.
//
#define LOG2_NUM_SHADES		3

//
// The mask for the combined screen y, screen x, shade value.
//
#define SY_SX_SHADE_MASK ((0x360000 << LOG2_NUM_SHADES) | (0x7FFFFFFF >> (15-LOG2_NUM_SHADES)))


#if (DITHER_SIZE == 2)


//*****************************************************************************************
//
// Dithered, alpha-fogged, perspective texture.
//
// Notes:
//	The inner loop keeps both the current intensity and the low bit of screen y and 
//	screen x in one register.  The contents of the register are layed out as follows:
//	00000000000YXSSSFFFFFFFFFFFFFFFF.
//
void DrawSubtriangle(TGFogPersp* pscan, CDrawPolygon<TGFogPersp>* pdtri)
{
	typedef TGFogPersp::TPixel prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastDither = 0;
	static uint32 u4LastFogMask = 0xffffffff;
	fixed fx_inc;
	fixed fx_diff;
	fixed fx_gour_inc;
	float f_inc_uinvz;
	float f_inc_vinvz;
	float f_inc_invz;

	TGFogPersp* plinc = &pdtri->pedgeBase->lineIncrement;

	//
	// TGFogPersp with CGouraudOn substitued for CGouraudFog.
	// This is necessary so that we can access members of CGouraudOn.
	//
	typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMapTexture<uint16>, CIndexPerspective, CColLookupOff> FakeTGFogPersp;
	typedef FakeTGFogPersp::TPixel prevent_internal_compiler_errors2;

	__asm
	{
		//
		// Do self modification for fog mask.
		// Fog mask should be relatively constant.
		//
		mov		ebx,[u4TerrainFogMask]
		mov		eax,[u4LastFogMask]

		cmp		ebx,eax
		je		ALREADY_MODIFIED

		mov		[u4LastFogMask],ebx
		mov		[MODIFY_FOR_FOG_MASK_A-4],ebx
		mov		[MODIFY_FOR_FOG_MASK_B-4],ebx
		mov		[MODIFY_FOR_FOG_MASK_C-4],ebx
		mov		[MODIFY_FOR_FOG_MASK_D-4],ebx

ALREADY_MODIFIED:
		//
		// Do self modifications.
		//
		mov		ecx,[pvTextureBitmap]					// Texture pointer.
		lea		edx,lbAlphaTerrain.au2Colour			// Alpha color to color table.
		mov		esi,lbAlphaTerrain.au4AlphaRefDither	// Intensity to Alpha+Dither table.
		
		mov		eax,[pvLastTexture]
		mov		ebx,[pvLastClut]
		mov		edi,[pvLastDither]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,edx
		jne		short DO_MODIFY

		cmp		edi,esi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx						// Update last texture pointer.
		mov		[pvLastClut],edx						// Update last clut pointer.
		mov		[pvLastDither],esi						// Update last alhpa/dither table pointer.

		mov	eax,ecx
		inc	ecx
		mov	ebx,edx										// Clut pointer plus one.
		inc	edx

		mov		[MODIFY_FOR_TEXTURE_POINTER_A-4],eax
		mov		[MODIFY_FOR_TEXTURE_POINTER_P1_A-4],ecx
		mov		[MODIFY_FOR_ALPHA_DITHER_A-4],esi
		mov		[MODIFY_FOR_CLUT_POINTER_A-4],ebx
		mov		[MODIFY_FOR_CLUT_POINTER_P1_A-4],edx

		mov		[MODIFY_FOR_TEXTURE_POINTER_B-4],eax
		mov		[MODIFY_FOR_TEXTURE_POINTER_P1_B-4],ecx
		mov		[MODIFY_FOR_ALPHA_DITHER_B-4],esi
		mov		[MODIFY_FOR_CLUT_POINTER_B-4],ebx
		mov		[MODIFY_FOR_CLUT_POINTER_P1_B-4],edx

		mov		[MODIFY_FOR_TEXTURE_POINTER_C-4],eax
		mov		[MODIFY_FOR_TEXTURE_POINTER_P1_C-4],ecx
		mov		[MODIFY_FOR_ALPHA_DITHER_C-4],esi
		mov		[MODIFY_FOR_CLUT_POINTER_C-4],ebx
		mov		[MODIFY_FOR_CLUT_POINTER_P1_C-4],edx

		mov		[MODIFY_FOR_TEXTURE_POINTER_D-4],eax
		mov		[MODIFY_FOR_TEXTURE_POINTER_P1_D-4],ecx
		mov		[MODIFY_FOR_ALPHA_DITHER_D-4],esi
		mov		[MODIFY_FOR_CLUT_POINTER_D-4],ebx
		mov		[MODIFY_FOR_CLUT_POINTER_P1_D-4],edx

DONE_WITH_MODIFY:
		//
		// Local copies of edge stepping values.
		//
		// fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;
		// fx_gour_inc = pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		// f_inc_uinvz = pdtri->pedgeBase->lineIncrement.indCoord.fUInvZ;
		// f_inc_vinvz = pdtri->pedgeBase->lineIncrement.indCoord.fVInvZ;
		// f_inc_invz  = pdtri->pedgeBase->lineIncrement.indCoord.fInvZ;
		//
		mov		eax,[plinc]

		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		mov		ebx,[eax]TGFogPersp.fxX
		mov		edx,[eax]TGFogPersp.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TGFogPersp.indCoord.fUInvZ
		mov		edx,[eax]TGFogPersp.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		f_inc_vinvz,edx

		mov		ebx,[eax]TGFogPersp.indCoord.fInvZ
		mov		edx,[eax]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx

		mov		f_inc_invz,ebx
		mov		fx_gour_inc,edx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		eax,[esi]CDrawPolygonBase.iY		// Get current integer y.
		mov		ebx,[ecx]TGFogPersp.fxX.i4Fx		// Get current fixed point x.

		shl		ebx,LOG2_NUM_SHADES					// Shift low bit of x into position.
		and		eax,1								// Mask low bit of y.
	
		shl		eax,(17 + LOG2_NUM_SHADES)			// Shift low bit of y into position.
		and		ebx,(1 << (16+LOG2_NUM_SHADES))		// Mask shifted low bit of x.	

		mov		edi,[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx
		or		eax,ebx								// Combine low bits of x and y.

		or		edi,eax								// Combine low bits of x and y with intensity.
		push	ebp									// Save base pointer.

		mov		[fxIntensityTemp],edi				// Set the starting fog shading value.
		mov		eax,[esi]CDrawPolygonBase.iY		// Load current integer y.

		// Check to see if we should skip this scanline.
		and		eax,[bEvenScanlinesOnly]
		jnz		END_OF_SCANLINE

		// Setup for prespective correction and looping.
		PERSPECTIVE_SCANLINE_PROLOGUE_CLAMP(TGFogPersp)

		//
		// eax = i_pixel
		// edi = inner loop count
		// ecx,edx,esi = texture values
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.
		mov		ebx,[i_screen_index]				// Load scanline offset.

		add		ebx,eax								// Add scanline offset to i_pixel
		mov		[i_pixel],eax						// Save i_pixel.

		lea		ebp,[ebp+ebx*2]						// Base of span in ebp.
		mov		ebx,[fxIntensityTemp]				// Load intensity value.

		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.

		//-----------------------------------------------------------------------
		// Left to right scanline.
		lea		eax,[ebp + edi*2]					// Calculate destination address.

		// Check alignement.
		test	eax,3
		jz		short ALIGNED

		// Check for one pixel before starting.
		inc		edi
		jz		short FINISH_REMAINDER

		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_A:

		shr		ebp,14									// Only leave 2 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_A:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_A:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_A:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		add		ebx,[fxDeltaXIntensity]					// Step intensity.

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_A:
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_A:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		[ebp + edi*2 - 2],cx					// Store pixel.
		xor		ebx,(1 << (16+LOG2_NUM_SHADES))			// Toggle screen x bit.

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

		//
		// Do two pixels at a time.
		//
INNER_LOOP_2P:
		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTURE_POINTER_B:

		shr		ebp,14									// Only leave 2 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_B:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_B:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_B:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		xor		ebx,(1 << (16+LOG2_NUM_SHADES))			// Toggle screen x bit.

		add		ebx,[fxDeltaXIntensity]					// Step intensity.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.
		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_B:

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_B:

		// stall

		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_C:

		shr		ebp,14									// Only leave 2 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_C:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_C:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_C:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		mov		[i4_temp_a],ecx							// Save V fraction.

		shl		ecx,16									// Shift pixel into positon.
		xor		ebx,(1 << (16+LOG2_NUM_SHADES))			// Toggle screen x bit.

		add		ebx,[fxDeltaXIntensity]					// Step intensity.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.
		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_C:

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_C:

		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		rol		ecx,16

		mov		[ebp + edi*2 - 4],ecx					// Store 2 pixels.
		mov		ecx,[i4_temp_a]							// Restore V fraction.

		add		edi,2
		jle		short INNER_LOOP_2P

		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short END_OF_SPAN

		//
		// Finish left over pixel.
		//
FINISH_REMAINDER:
		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_D:

		shr		ebp,14									// Only leave 2 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_D:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_D:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_D:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		add		ebx,[fxDeltaXIntensity]					// Step intensity.

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_D:
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_D:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		[ebp - 2],cx							// Store pixel.
		xor		ebx,(1 << (16+LOG2_NUM_SHADES))			// Toggle screen x bit.

END_OF_SPAN:
		// See if there are any pixels left.
		mov		[fxIntensityTemp],ebx					// Save intensity value.
		mov		eax,[i_pixel]

		cmp		eax,0
		jz		END_OF_SCANLINE

		// Do perspective correction and looping.
		PERSPECTIVE_SCANLINE_EPILOGUE_CLAMP

END_OF_SCANLINE:
		pop		ebp

		//		
		// Increment the base edge.
		//
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		// Step length, line starting address.
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ebx,[esi]CDrawPolygonBase.iLineStartIndex

		add		eax,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ebx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TGFogPersp.fxX.i4Fx
		mov		ebx,[ecx]TGFogPersp.fxXDifference.i4Fx

		fld		[ecx]TGFogPersp.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TGFogPersp.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TGFogPersp.indCoord.fInvZ
		fadd	[f_inc_invz]

		mov		edx,[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx
		mov		edi,[fx_diff]

		add		edx,[fx_gour_inc]
		add		eax,[fx_inc]

		add		ebx,edi
		jge		short NO_UNDERFLOW

		// Underflow.
		add		ebx,0x00010000
		mov		edi,[fxIModDiv]

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short NO_OVERFLOW

		// Overflow.
		sub		ebx,0x00010000
		mov		edi,[fxIModDiv]

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_OVERFLOW:
		fstp	[ecx]TGFogPersp.indCoord.fInvZ
		fstp	[ecx]TGFogPersp.indCoord.fVInvZ
		fstp	[ecx]TGFogPersp.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		// Stall (1)
		fstp	[ecx]TGFogPersp.indCoord.fInvZ
		fstp	[ecx]TGFogPersp.indCoord.fUInvZ
		fstp	[ecx]TGFogPersp.indCoord.fVInvZ

FINISH_LOOPING:
		// Save values and Y Loop control.
		mov		[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx,edx
		mov		[ecx]TGFogPersp.fxX.i4Fx,eax

		mov		edx,[esi]CDrawPolygonBase.iY
		mov		[ecx]TGFogPersp.fxXDifference.i4Fx,ebx

		inc		edx
		mov		ebx,[esi]CDrawPolygonBase.iYTo

		mov		[esi]CDrawPolygonBase.iY,edx
		cmp		edx,ebx

		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Dithered, alpha-fogged, linear texture.
//
void DrawSubtriangle(TGFogLinear* pscan, CDrawPolygon<TGFogLinear>* pdtri)
{
	typedef TGFogLinear::TPixel prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastDither = 0;
	static uint32 u4LastFogMask = 0xffffffff;
	static CDrawPolygon<TGFogLinear>* pdtriGlbl;
	static TGFogLinear* pscanGlbl;
	static fixed fx_gour_inc;

	TGFogLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	//
	// TGFogLinear with CGouraudOn substitued for CGouraudFog.
	// This is necessary so that we can access members of CGouraudOn.
	//
	typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMapTexture<uint16>, CIndexLinear, CColLookupOff> FakeTGFogLinear;
	typedef FakeTGFogLinear::TPixel prevent_internal_compiler_errors2;

	__asm
	{
		//
		// Do self modification for fog mask.
		// Fog mask should be relatively constant.
		//
		mov		ebx,[u4TerrainFogMask]
		mov		eax,[u4LastFogMask]

		cmp		ebx,eax
		je		ALREADY_MODIFIED

		mov		[u4LastFogMask],ebx
		mov		[MODIFY_FOR_FOG_MASK_A-4],ebx
		mov		[MODIFY_FOR_FOG_MASK_B-4],ebx
		mov		[MODIFY_FOR_FOG_MASK_C-4],ebx
		mov		[MODIFY_FOR_FOG_MASK_D-4],ebx

ALREADY_MODIFIED:
		//
		// Do self modifications.
		//
		mov		ecx,[pvTextureBitmap]					// Texture pointer.
		lea		edx,lbAlphaTerrain.au2Colour			// Alpha color to color table.
		mov		esi,lbAlphaTerrain.au4AlphaRefDither	// Intensity to Alpha+Dither table.
		
		mov		eax,[pvLastTexture]
		mov		ebx,[pvLastClut]
		mov		edi,[pvLastDither]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,edx
		jne		short DO_MODIFY

		cmp		edi,esi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx						// Update last texture pointer.
		mov		[pvLastClut],edx						// Update last clut pointer.
		mov		[pvLastDither],esi						// Update last alhpa/dither table pointer.

		mov	eax,ecx
		inc	ecx
		mov	ebx,edx										// Clut pointer plus one.
		inc	edx

		mov		[MODIFY_FOR_TEXTURE_POINTER_A-4],eax
		mov		[MODIFY_FOR_TEXTURE_POINTER_P1_A-4],ecx
		mov		[MODIFY_FOR_ALPHA_DITHER_A-4],esi
		mov		[MODIFY_FOR_CLUT_POINTER_A-4],ebx
		mov		[MODIFY_FOR_CLUT_POINTER_P1_A-4],edx

		mov		[MODIFY_FOR_TEXTURE_POINTER_B-4],eax
		mov		[MODIFY_FOR_TEXTURE_POINTER_P1_B-4],ecx
		mov		[MODIFY_FOR_ALPHA_DITHER_B-4],esi
		mov		[MODIFY_FOR_CLUT_POINTER_B-4],ebx
		mov		[MODIFY_FOR_CLUT_POINTER_P1_B-4],edx

		mov		[MODIFY_FOR_TEXTURE_POINTER_C-4],eax
		mov		[MODIFY_FOR_TEXTURE_POINTER_P1_C-4],ecx
		mov		[MODIFY_FOR_ALPHA_DITHER_C-4],esi
		mov		[MODIFY_FOR_CLUT_POINTER_C-4],ebx
		mov		[MODIFY_FOR_CLUT_POINTER_P1_C-4],edx

		mov		[MODIFY_FOR_TEXTURE_POINTER_D-4],eax
		mov		[MODIFY_FOR_TEXTURE_POINTER_P1_D-4],ecx
		mov		[MODIFY_FOR_ALPHA_DITHER_D-4],esi
		mov		[MODIFY_FOR_CLUT_POINTER_D-4],ebx
		mov		[MODIFY_FOR_CLUT_POINTER_P1_D-4],edx

DONE_WITH_MODIFY:
		mov		edi,[plinc]							// Pointer to scanline increment.
		mov		esi,[pdtri]							// Pointer to polygon object.

		mov		eax,[pscan]							// Pointer to scanline object.
		push	ebp									// Save ebp.

		//
		// Local copies of edge stepping values.
		//
		// fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
		// bf_u_inc			= pdtri->pedgeBase->lineIncrement.indCoord.bfU;
		// w1d_v_inc		= pdtri->pedgeBase->lineIncrement.indCoord.w1dV;
		// pdtriGlbl		= pdtri;
		// pscanGlbl		= pscan;
		// fx_gour_inc		= pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		//
		mov		ebx,[edi]TGFogLinear.fxX
		mov		ecx,[edi]TGFogLinear.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TGFogLinear.indCoord.bfU.i4Int
		mov		ecx,[edi]TGFogLinear.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TGFogLinear.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TGFogLinear.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriGlbl,esi
		mov		ebx,[edi]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx

		mov		pscanGlbl,eax
		mov		fx_gour_inc,ebx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebp,[esi]CDrawPolygonBase.iY		// Get current integer y.
		mov		ebx,[eax]FakeTGFogLinear.fxX.i4Fx	// Get current fixed point x.

		shl		ebx,LOG2_NUM_SHADES					// Shift low bit of x into position.
		and		ebp,1								// Mask low bit of y.
	
		shl		ebp,(17 + LOG2_NUM_SHADES)			// Shift low bit of y into position.
		and		ebx,(1 << (16+LOG2_NUM_SHADES))		// Mask shifted low bit of x.

		mov		edi,[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx
		or		ebp,ebx								// Combine low bits of x and y.

		or		edi,ebp								// Combine low bits of x and y with intensity.
		mov		ebx,[bEvenScanlinesOnly]

		mov		[fxIntensityTemp],edi				// Set the starting fog shading value.
		mov		ebp,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TGFogLinear.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		add		ebx,ecx

		sar		ebx,16
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		edi,16
		mov		ebp,gsGlobals.pvScreen

		add		ecx,ebx

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		mov		esi,[eax]TGFogLinear.indCoord.bfU.u4Frac					// UFrac
		mov		edx,[eax]TGFogLinear.indCoord.w1dV.bfxValue.i4Int		

		lea		ebp,[ebp + ecx*2]
		mov		ebx,[eax]TGFogLinear.indCoord.bfU.i4Int

		add		edx,ebx														// Add integer steps.
		mov		ecx,[eax]TGFogLinear.indCoord.w1dV.bfxValue.u4Frac			// VFrac

		mov		[pvBaseOfLine],ebp
		mov		ebx,[fxIntensityTemp]					// Load intensity value.

		//-----------------------------------------------------------------------
		// Left to right scanline.
		lea		eax,[ebp + edi*2]					// Calculate destination address.

		// Check alignement.
		test	eax,3
		jz		short ALIGNED

		// Check for one pixel before starting.
		inc		edi
		jz		short FINISH_REMAINDER

		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_A:

		shr		ebp,14									// Only leave 2 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_A:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_A:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_A:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		add		ebx,[fxDeltaXIntensity]					// Step intensity.

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_A:
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_A:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		[ebp + edi*2 - 2],cx					// Store pixel.
		xor		ebx,(1 << (16+LOG2_NUM_SHADES))			// Toggle screen x bit.

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

		//
		// Do two pixels at a time.
		//
INNER_LOOP_2P:
		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTURE_POINTER_B:

		shr		ebp,14									// Only leave 2 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_B:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_B:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_B:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		xor		ebx,(1 << (16+LOG2_NUM_SHADES))			// Toggle screen x bit.

		add		ebx,[fxDeltaXIntensity]					// Step intensity.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.
		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_B:

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_B:

		// stall

		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_C:

		shr		ebp,14									// Only leave 2 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_C:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_C:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_C:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		mov		[i4_temp_a],ecx							// Save V fraction.

		shl		ecx,16									// Shift pixel into positon.
		xor		ebx,(1 << (16+LOG2_NUM_SHADES))			// Toggle screen x bit.

		add		ebx,[fxDeltaXIntensity]					// Step intensity.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.
		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_C:

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_C:

		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		rol		ecx,16

		mov		[ebp + edi*2 - 4],ecx					// Store 2 pixels.
		mov		ecx,[i4_temp_a]							// Restore V fraction.

		add		edi,2
		jle		short INNER_LOOP_2P

		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short END_OF_SCANLINE

		//
		// Finish left over pixel.
		//
FINISH_REMAINDER:

		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_D:

		shr		ebp,14									// Only leave 2 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_D:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_D:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_D:

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_D:
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_D:

		mov		[ebp - 2],cx							// Store pixel.

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TGFogLinear.indCoord.bfU.u4Frac
		mov		edx,[eax]TGFogLinear.indCoord.bfU.i4Int

		mov		edi,[eax]TGFogLinear.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TGFogLinear.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TGFogLinear.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebp,[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx
		mov		ebx,[eax]TGFogLinear.fxXDifference

		add		ebp,[fx_gour_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,bfNegDU.i4Int
		add		ebp,[fxIModDiv]

		add		esi,w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE

		add		edi,[eax]TGFogLinear.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,bfNegDU.i4Int
		add		ebp,[fxIModDiv]

		add		esi,w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE

		add		edi,[eax]TGFogLinear.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx,ebp
		mov		[eax]TGFogLinear.fxXDifference,ebx

		mov		[eax]TGFogLinear.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TGFogLinear.indCoord.w1dV.bfxValue.i4Int,edi

		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TGFogLinear.indCoord.bfU.u4Frac,ecx
		mov		[eax]TGFogLinear.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TGFogLinear.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TGFogLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp
	}
}


//*********************************************************************************************
void DrawSubtriangle(TFlatGourFog* pscan, CDrawPolygon<TFlatGourFog>* pdtri)
{
	typedef TFlatGourFog::TPixel TDest;

	Assert(pscan);
	Assert(pdtri);

	int i_x_from;
	int i_x_to;
	int i_screen_index;
	int i_pixel;

	// Iterate through the scanlines that intersect the subtriangle.
	do
	{
		i_x_from = pscan->fxX.i4Fx >> 16;
		i_x_to   = (pscan->fxX.i4Fx + pdtri->fxLineLength.i4Fx) >> 16;
		i_screen_index = pdtri->iLineStartIndex + i_x_to;
		i_pixel = i_x_from - i_x_to;

		// Draw if there are pixels to draw.
		if (i_x_to > i_x_from && !(bEvenScanlinesOnly && ((pdtri->iY & 1) == 0)))
		{
			TFlatGourFog::TGouraud gour(pscan->gourIntensity);
			TDest* apix_screen = ((TDest*)pdtri->prasScreen->pSurface) + i_screen_index;

			int i_x = i_x_from;

			// Iterate left along the destination scanline.
			do
			{
//				int32  i4_fog_index     = int(gour.fxIntensity);
//				Assert(bWithin(i4_fog_index, 0, iNUM_TERRAIN_FOG_BANDS - 1));

				uint16 u2_source_pixel  = u4ConstColour;

				u2_source_pixel &= lbAlphaTerrain.u2GetAlphaMask();
				u2_source_pixel |= lbAlphaTerrain.u4GetAlphaRefDither(i_x, pdtri->iY, gour.fxIntensity.i4Fx);

				apix_screen[i_pixel] = lbAlphaTerrain.au2Colour[u2_source_pixel];
				
				i_x++;

				// Increment values.
				++gour;
			}
			while (++i_pixel);
		}
		
		// Increment the base edge.
		++*pdtri->pedgeBase;

		// Set the new length of the line to be rasterized.
		pdtri->fxLineLength.i4Fx += pdtri->fxDeltaLineLength.i4Fx;

		// Get new starting pixel TIndex for the scanline.
		pdtri->iLineStartIndex += pdtri->prasScreen->iLinePixels;
	}
	while (++pdtri->iY < pdtri->iYTo);
}


#elif (DITHER_SIZE == 4)  // if (DITHER_SIZE == 2)


//*****************************************************************************************
//
// Dithered, alpha-fogged, perspective texture.
//
// Notes:
//	The inner loop keeps both the current intensity and 2 bits of the current screen x
//	and curren screen y value in one register.  The contents of the register are layed
//	out as follows: 0000000YY0XX0SSSFFFFFFFFFFFFFFFF.  Where YY is the lower two bits
//  of the current screen y, XX is the lower 2 bits of the current x, SSS is the 
//  integral part of the shading value (more bits would be used for more than 8 shades),
//	and the F's are the fractional part oft the shading values.
//
void DrawSubtriangle(TGFogPersp* pscan, CDrawPolygon<TGFogPersp>* pdtri)
{
	typedef TGFogPersp::TPixel prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastDither = 0;
	static uint32 u4LastFogMask = 0xffffffff;
	static fixed DeltaXIntensityAndX;
	fixed fx_inc;
	fixed fx_diff;
	fixed fx_gour_inc;
	float f_inc_uinvz;
	float f_inc_vinvz;
	float f_inc_invz;

	TGFogPersp* plinc = &pdtri->pedgeBase->lineIncrement;

	//
	// TGFogPersp with CGouraudOn substitued for CGouraudFog.
	// This is necessary so that we can access members of CGouraudOn.
	//
	typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMapTexture<uint16>, CIndexPerspective, CColLookupOff> FakeTGFogPersp;
	typedef FakeTGFogPersp::TPixel prevent_internal_compiler_errors2;

	__asm
	{
		//
		// Do self modification for fog mask.
		// Fog mask should be relatively constant.
		//
		mov		ebx,[u4TerrainFogMask]
		mov		eax,[u4LastFogMask]

		cmp		ebx,eax
		je		ALREADY_MODIFIED

		mov		[u4LastFogMask],ebx

		lea		eax,MODIFY_FOR_FOG_MASK_A
		mov		[eax-4],ebx
		lea		eax,MODIFY_FOR_FOG_MASK_B
		mov		[eax-4],ebx
		lea		eax,MODIFY_FOR_FOG_MASK_C
		mov		[eax-4],ebx
		lea		eax,MODIFY_FOR_FOG_MASK_D
		mov		[eax-4],ebx

ALREADY_MODIFIED:
		//
		// Do self modifications.
		//
		mov		ecx,[pvTextureBitmap]					// Texture pointer.
		lea		edx,lbAlphaTerrain.au2Colour			// Alpha color to color table.
		mov		esi,lbAlphaTerrain.au4AlphaRefDither	// Intensity to Alpha+Dither table.
		
		mov		eax,[pvLastTexture]
		mov		ebx,[pvLastClut]
		mov		edi,[pvLastDither]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,edx
		jne		short DO_MODIFY

		cmp		edi,esi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx						// Update last texture pointer.
		mov		[pvLastClut],edx						// Update last clut pointer.
		mov		[pvLastDither],esi						// Update last alhpa/dither table pointer.

		mov	eax,ecx
		inc	ecx
		mov	ebx,edx										// Clut pointer plus one.
		inc	edx

		lea		edi,MODIFY_FOR_TEXTURE_POINTER_A
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTURE_POINTER_P1_A
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ALPHA_DITHER_A
		mov		[edi-4],esi
		lea		edi,MODIFY_FOR_CLUT_POINTER_A
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_POINTER_P1_A
		mov		[edi-4],edx

		lea		edi,MODIFY_FOR_TEXTURE_POINTER_B
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTURE_POINTER_P1_B
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ALPHA_DITHER_B
		mov		[edi-4],esi
		lea		edi,MODIFY_FOR_CLUT_POINTER_B
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_POINTER_P1_B
		mov		[edi-4],edx

		lea		edi,MODIFY_FOR_TEXTURE_POINTER_C
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTURE_POINTER_P1_C
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ALPHA_DITHER_C
		mov		[edi-4],esi
		lea		edi,MODIFY_FOR_CLUT_POINTER_C
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_POINTER_P1_C
		mov		[edi-4],edx

		lea		edi,MODIFY_FOR_TEXTURE_POINTER_D
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTURE_POINTER_P1_D
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ALPHA_DITHER_D
		mov		[edi-4],esi
		lea		edi,MODIFY_FOR_CLUT_POINTER_D
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_POINTER_P1_D
		mov		[edi-4],edx

DONE_WITH_MODIFY:
		//
		// Local copies of edge stepping values.
		//
		// fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;
		// fx_gour_inc = pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		// f_inc_uinvz = pdtri->pedgeBase->lineIncrement.indCoord.fUInvZ;
		// f_inc_vinvz = pdtri->pedgeBase->lineIncrement.indCoord.fVInvZ;
		// f_inc_invz  = pdtri->pedgeBase->lineIncrement.indCoord.fInvZ;
		//
		mov		eax,[plinc]

		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		mov		ebx,[eax]TGFogPersp.fxX
		mov		edx,[eax]TGFogPersp.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TGFogPersp.indCoord.fUInvZ
		mov		edx,[eax]TGFogPersp.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		f_inc_vinvz,edx

		mov		ebx,[eax]TGFogPersp.indCoord.fInvZ
		mov		edi,[fxDeltaXIntensity]

		mov		edx,[eax]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx
		and		edi,(0x7fffffff >> (15 - LOG2_NUM_SHADES))			// Mask negative values.

		mov		f_inc_invz,ebx
		add		edi,(1 << (LOG2_NUM_SHADES+17))						// The low bit of screen x.

		mov		fx_gour_inc,edx
		mov		[DeltaXIntensityAndX],edi

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		eax,[esi]CDrawPolygonBase.iY		// Get current integer y.
		mov		ebx,[ecx]TGFogPersp.fxX.i4Fx		// Get current fixed point x.

		shl		ebx,(LOG2_NUM_SHADES+1)				// Shift low 2 bits of x into position.
		and		eax,3								// Mask low 2 bits of y.
	
		shl		eax,(20 + LOG2_NUM_SHADES)			// Shift low 2 bits of y into position.
		and		ebx,(3 << (17+LOG2_NUM_SHADES))		// Mask shifted low 2 bits of x.	

		mov		edi,[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx
		or		eax,ebx								// Combine low bits of x and y.

		or		edi,eax								// Combine low bits of x and y with intensity.
		push	ebp									// Save base pointer.

		mov		[fxIntensityTemp],edi				// Set the starting fog shading value.
		mov		eax,[esi]CDrawPolygonBase.iY		// Load current integer y.

		// Check to see if we should skip this scanline.
		and		eax,[bEvenScanlinesOnly]
		jnz		END_OF_SCANLINE

		// Setup for prespective correction and looping.
		PERSPECTIVE_SCANLINE_PROLOGUE_CLAMP(TGFogPersp)

		//
		// eax = i_pixel
		// edi = inner loop count
		// ecx,edx,esi = texture values
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.
		mov		ebx,[i_screen_index]				// Load scanline offset.

		add		ebx,eax								// Add scanline offset to i_pixel
		mov		[i_pixel],eax						// Save i_pixel.

		lea		ebp,[ebp+ebx*2]						// Base of span in ebp.
		mov		ebx,[fxIntensityTemp]				// Load intensity value.

		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.

		//-----------------------------------------------------------------------
		// Left to right scanline.
		lea		eax,[ebp + edi*2]					// Calculate destination address.

		// Check alignement.
		test	eax,3
		jz		short ALIGNED

		// Check for one pixel before starting.
		inc		edi
		jz		short FINISH_REMAINDER

		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_A:

		shr		ebp,12									// Only leave 4 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_A:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_A:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_A:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		nop

		add		ebx,[DeltaXIntensityAndX]				// Step intensity & screen x.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_A:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_A:
		and		ebx,SY_SX_SHADE_MASK					// Mask off screen x and screen y carry.

		mov		[ebp + edi*2 - 2],cx					// Store pixel.

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

		//
		// Do two pixels at a time.
		//
INNER_LOOP_2P:
		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_B:

		shr		ebp,12									// Only leave 4 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_B:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_B:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_B:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		nop

		add		ebx,[DeltaXIntensityAndX]				// Step intensity & screen x.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_B:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_B:

		and		ebx,SY_SX_SHADE_MASK					// Mask off screen x and screen y carry.

		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_C:

		shr		ebp,12									// Only leave 4 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_C:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_C:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_C:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		mov		[i4_temp_a],ecx							// Save V fraction.

		shl		ecx,16									// Shift pixel into positon.
		nop

		add		ebx,[DeltaXIntensityAndX]				// Step intensity & screen x.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_C:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_C:

		and		ebx,SY_SX_SHADE_MASK					// Mask off screen x and screen y carry.
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		rol		ecx,16

		mov		[ebp + edi*2 - 4],ecx					// Store 2 pixels.
		mov		ecx,[i4_temp_a]							// Restore V fraction.

		add		edi,2
		jle		short INNER_LOOP_2P

		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short END_OF_SPAN

		//
		// Finish left over pixel.
		//
FINISH_REMAINDER:
		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_D:

		shr		ebp,12									// Only leave 4 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_D:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_D:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_D:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		nop

		add		ebx,[DeltaXIntensityAndX]				// Step intensity & screen x.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_D:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_D:
		and		ebx,SY_SX_SHADE_MASK					// Mask off screen x and screen y carry.

		mov		[ebp - 2],cx							// Store pixel.

END_OF_SPAN:
		// See if there are any pixels left.
		mov		[fxIntensityTemp],ebx					// Save intensity value.
		mov		eax,[i_pixel]

		cmp		eax,0
		jz		END_OF_SCANLINE

		// Do perspective correction and looping.
		PERSPECTIVE_SCANLINE_EPILOGUE_CLAMP

END_OF_SCANLINE:
		pop		ebp

		//		
		// Increment the base edge.
		//
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		// Step length, line starting address.
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ebx,[esi]CDrawPolygonBase.iLineStartIndex

		add		eax,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ebx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TGFogPersp.fxX.i4Fx
		mov		ebx,[ecx]TGFogPersp.fxXDifference.i4Fx

		fld		[ecx]TGFogPersp.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TGFogPersp.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TGFogPersp.indCoord.fInvZ
		fadd	[f_inc_invz]

		mov		edx,[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx
		mov		edi,[fx_diff]

		add		edx,[fx_gour_inc]
		add		eax,[fx_inc]

		add		ebx,edi
		jge		short NO_UNDERFLOW

		// Underflow.
		add		ebx,0x00010000
		mov		edi,[fxIModDiv]

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short NO_OVERFLOW

		// Overflow.
		sub		ebx,0x00010000
		mov		edi,[fxIModDiv]

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_OVERFLOW:
		fstp	[ecx]TGFogPersp.indCoord.fInvZ
		fstp	[ecx]TGFogPersp.indCoord.fVInvZ
		fstp	[ecx]TGFogPersp.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		// Stall (1)
		fstp	[ecx]TGFogPersp.indCoord.fInvZ
		fstp	[ecx]TGFogPersp.indCoord.fUInvZ
		fstp	[ecx]TGFogPersp.indCoord.fVInvZ

FINISH_LOOPING:
		// Save values and Y Loop control.
		mov		[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx,edx
		mov		[ecx]TGFogPersp.fxX.i4Fx,eax

		mov		edx,[esi]CDrawPolygonBase.iY
		mov		[ecx]TGFogPersp.fxXDifference.i4Fx,ebx

		inc		edx
		mov		ebx,[esi]CDrawPolygonBase.iYTo

		mov		[esi]CDrawPolygonBase.iY,edx
		cmp		edx,ebx

		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Dithered, alpha-fogged, linear texture.
//
void DrawSubtriangle(TGFogLinear* pscan, CDrawPolygon<TGFogLinear>* pdtri)
{
	typedef TGFogLinear::TPixel prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastDither = 0;
	static uint32 u4LastFogMask = 0xffffffff;
	static fixed DeltaXIntensityAndX;
	static CDrawPolygon<TGFogLinear>* pdtriGlbl;
	static TGFogLinear* pscanGlbl;
	static fixed fx_gour_inc;

	TGFogLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	//
	// TGFogLinear with CGouraudOn substitued for CGouraudFog.
	// This is necessary so that we can access members of CGouraudOn.
	//
	typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMapTexture<uint16>, CIndexLinear, CColLookupOff> FakeTGFogLinear;
	typedef FakeTGFogLinear::TPixel prevent_internal_compiler_errors2;

	__asm
	{
		//
		// Do self modification for fog mask.
		// Fog mask should be relatively constant.
		//
		mov		ebx,[u4TerrainFogMask]
		mov		eax,[u4LastFogMask]

		cmp		ebx,eax
		je		ALREADY_MODIFIED

		mov		[u4LastFogMask],ebx

		lea		eax,MODIFY_FOR_FOG_MASK_A
		mov		[eax-4],ebx
		lea		eax,MODIFY_FOR_FOG_MASK_B
		mov		[eax-4],ebx
		lea		eax,MODIFY_FOR_FOG_MASK_C
		mov		[eax-4],ebx
		lea		eax,MODIFY_FOR_FOG_MASK_D
		mov		[eax-4],ebx

ALREADY_MODIFIED:
		//
		// Do self modifications.
		//
		mov		ecx,[pvTextureBitmap]					// Texture pointer.
		lea		edx,lbAlphaTerrain.au2Colour			// Alpha color to color table.
		mov		esi,lbAlphaTerrain.au4AlphaRefDither	// Intensity to Alpha+Dither table.
		
		mov		eax,[pvLastTexture]
		mov		ebx,[pvLastClut]
		mov		edi,[pvLastDither]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,edx
		jne		short DO_MODIFY

		cmp		edi,esi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx						// Update last texture pointer.
		mov		[pvLastClut],edx						// Update last clut pointer.
		mov		[pvLastDither],esi						// Update last alhpa/dither table pointer.

		mov	eax,ecx
		inc	ecx
		mov	ebx,edx										// Clut pointer plus one.
		inc	edx

		lea		edi,MODIFY_FOR_TEXTURE_POINTER_A
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTURE_POINTER_P1_A
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ALPHA_DITHER_A
		mov		[edi-4],esi
		lea		edi,MODIFY_FOR_CLUT_POINTER_A
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_POINTER_P1_A
		mov		[edi-4],edx

		lea		edi,MODIFY_FOR_TEXTURE_POINTER_B
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTURE_POINTER_P1_B
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ALPHA_DITHER_B
		mov		[edi-4],esi
		lea		edi,MODIFY_FOR_CLUT_POINTER_B
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_POINTER_P1_B
		mov		[edi-4],edx

		lea		edi,MODIFY_FOR_TEXTURE_POINTER_C
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTURE_POINTER_P1_C
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ALPHA_DITHER_C
		mov		[edi-4],esi
		lea		edi,MODIFY_FOR_CLUT_POINTER_C
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_POINTER_P1_C
		mov		[edi-4],edx

		lea		edi,MODIFY_FOR_TEXTURE_POINTER_D
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTURE_POINTER_P1_D
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ALPHA_DITHER_D
		mov		[edi-4],esi
		lea		edi,MODIFY_FOR_CLUT_POINTER_D
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_POINTER_P1_D
		mov		[edi-4],edx

DONE_WITH_MODIFY:
		mov		edi,[plinc]							// Pointer to scanline increment.
		mov		esi,[pdtri]							// Pointer to polygon object.

		mov		eax,[pscan]							// Pointer to scanline object.
		push	ebp									// Save ebp.

		//
		// Local copies of edge stepping values.
		//
		// fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
		// bf_u_inc			= pdtri->pedgeBase->lineIncrement.indCoord.bfU;
		// w1d_v_inc		= pdtri->pedgeBase->lineIncrement.indCoord.w1dV;
		// pdtriGlbl		= pdtri;
		// pscanGlbl		= pscan;
		// fx_gour_inc		= pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		//
		mov		ebx,[edi]TGFogLinear.fxX
		mov		ecx,[edi]TGFogLinear.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TGFogLinear.indCoord.bfU.i4Int
		mov		ecx,[edi]TGFogLinear.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TGFogLinear.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TGFogLinear.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriGlbl,esi
		mov		ecx,[fxDeltaXIntensity]

		mov		ebx,[edi]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx
		and		ecx,(0x7fffffff >> (15 - LOG2_NUM_SHADES))			// Mask negative values.

		mov		pscanGlbl,eax
		add		ecx,(1 << (LOG2_NUM_SHADES+17))						// The low bit of screen x.

		mov		fx_gour_inc,ebx
		mov		[DeltaXIntensityAndX],ecx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebp,[esi]CDrawPolygonBase.iY		// Get current integer y.
		mov		ebx,[eax]FakeTGFogLinear.fxX.i4Fx	// Get current fixed point x.

		shl		ebx,(LOG2_NUM_SHADES+1)				// Shift low 2 bits of x into position.
		and		ebp,3								// Mask low 2 bits of y.
	
		shl		ebp,(20 + LOG2_NUM_SHADES)			// Shift low 2 bits of y into position.
		and		ebx,(3 << (17+LOG2_NUM_SHADES))		// Mask shifted low 2 bits of x.	

		mov		edi,[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx
		or		ebp,ebx								// Combine low bits of x and y.

		or		edi,ebp								// Combine low bits of x and y with intensity.
		mov		ebx,[bEvenScanlinesOnly]

		mov		[fxIntensityTemp],edi				// Set the starting fog shading value.
		mov		ebp,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TGFogLinear.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		add		ebx,ecx

		sar		ebx,16
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		edi,16
		mov		ebp,gsGlobals.pvScreen

		add		ecx,ebx

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		mov		esi,[eax]TGFogLinear.indCoord.bfU.u4Frac					// UFrac
		mov		edx,[eax]TGFogLinear.indCoord.w1dV.bfxValue.i4Int		

		lea		ebp,[ebp + ecx*2]
		mov		ebx,[eax]TGFogLinear.indCoord.bfU.i4Int

		add		edx,ebx														// Add integer steps.
		mov		ecx,[eax]TGFogLinear.indCoord.w1dV.bfxValue.u4Frac			// VFrac

		mov		[pvBaseOfLine],ebp
		mov		ebx,[fxIntensityTemp]					// Load intensity value.

		//-----------------------------------------------------------------------
		// Left to right scanline.
		lea		eax,[ebp + edi*2]					// Calculate destination address.

		// Check alignement.
		test	eax,3
		jz		short ALIGNED

		// Check for one pixel before starting.
		inc		edi
		jz		short FINISH_REMAINDER

		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_A:

		shr		ebp,12									// Only leave 4 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_A:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_A:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_A:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		nop

		add		ebx,[DeltaXIntensityAndX]				// Step intensity & screen x.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_A:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_A:
		and		ebx,SY_SX_SHADE_MASK					// Mask off screen x and screen y carry.

		mov		[ebp + edi*2 - 2],cx					// Store pixel.

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

		//
		// Do two pixels at a time.
		//
INNER_LOOP_2P:
		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_B:

		shr		ebp,12									// Only leave 4 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_B:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_B:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_B:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		nop

		add		ebx,[DeltaXIntensityAndX]				// Step intensity & screen x.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_B:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_B:

		and		ebx,SY_SX_SHADE_MASK					// Mask off screen x and screen y carry.

		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_C:

		shr		ebp,12									// Only leave 4 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_C:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_C:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_C:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		mov		[i4_temp_a],ecx							// Save V fraction.

		shl		ecx,16									// Shift pixel into positon.
		nop

		add		ebx,[DeltaXIntensityAndX]				// Step intensity & screen x.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac			// Step U fraction.

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_C:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step based on V frac. carry.

		adc		edx,ebp									// Integer step plus U frac. carry.
		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_C:

		and		ebx,SY_SX_SHADE_MASK					// Mask off screen x and screen y carry.
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		rol		ecx,16

		mov		[ebp + edi*2 - 4],ecx					// Store 2 pixels.
		mov		ecx,[i4_temp_a]							// Restore V fraction.

		add		edi,2
		jle		short INNER_LOOP_2P

		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short END_OF_SCANLINE

		//
		// Finish left over pixel.
		//
FINISH_REMAINDER:
		mov		ebp,ebx									// Copy shade/screen x,y value.
		mov		al,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (low).
MODIFY_FOR_TEXTURE_POINTER_D:

		shr		ebp,12									// Only leave 4 fractional bits.
		mov		ah,[edx*2 + 0xDEADBEEF]					// Read (and mask) texture value (high).
MODIFY_FOR_TEXTURE_POINTER_P1_D:

		and		eax,0xDEADBEEF							// Mask off bits used for fogging.
MODIFY_FOR_FOG_MASK_D:

		or		eax,[ebp*4 + 0xDEADBEEF]				// Combine with dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_D:
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_D:

		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_D:

		mov		[ebp - 2],cx							// Store pixel.

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TGFogLinear.indCoord.bfU.u4Frac
		mov		edx,[eax]TGFogLinear.indCoord.bfU.i4Int

		mov		edi,[eax]TGFogLinear.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TGFogLinear.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TGFogLinear.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebp,[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx
		mov		ebx,[eax]TGFogLinear.fxXDifference

		add		ebp,[fx_gour_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,bfNegDU.i4Int
		add		ebp,[fxIModDiv]

		add		esi,w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE

		add		edi,[eax]TGFogLinear.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,bfNegDU.i4Int
		add		ebp,[fxIModDiv]

		add		esi,w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE

		add		edi,[eax]TGFogLinear.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx,ebp
		mov		[eax]TGFogLinear.fxXDifference,ebx

		mov		[eax]TGFogLinear.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TGFogLinear.indCoord.w1dV.bfxValue.i4Int,edi

		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TGFogLinear.indCoord.bfU.u4Frac,ecx
		mov		[eax]TGFogLinear.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TGFogLinear.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TGFogLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp
	}
}


//*****************************************************************************************
//
// Dithered, alpha-fogged, linear texture.
//
void DrawSubtriangle(TFlatGourFog* pscan, CDrawPolygon<TFlatGourFog>* pdtri)
{
	typedef TFlatGourFog::TPixel prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastDither = 0;
	static fixed DeltaXIntensityAndX;
	static CDrawPolygon<TFlatGourFog>* pdtriGlbl;
	static TFlatGourFog* pscanGlbl;
	static fixed fx_gour_inc;
	static uint32 u4MaskedColor;

	TFlatGourFog* plinc = &pdtri->pedgeBase->lineIncrement;
	u4MaskedColor = u4ConstColour & u4TerrainFogMask;

	//
	// TFlatGourFog with CGouraudOn substitued for CGouraudFog.
	// This is necessary so that we can access members of CGouraudOn.
	//
	typedef CScanline<uint16, CGouraudOn, CTransparencyOff, CMapFlat, CIndexNone, 
					  CColLookupOff>							FakeTFlatGourFog;
	typedef FakeTFlatGourFog::TPixel prevent_internal_compiler_errors2;

	__asm
	{
		//
		// Do self modifications.
		//
		lea		edx,lbAlphaTerrain.au2Colour			// Alpha color to color table.
		mov		esi,lbAlphaTerrain.au4AlphaRefDither	// Intensity to Alpha+Dither table.
		
		mov		ebx,[pvLastClut]
		mov		edi,[pvLastDither]

		cmp		ebx,edx
		jne		short DO_MODIFY

		cmp		edi,esi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastClut],edx						// Update last clut pointer.
		mov		[pvLastDither],esi						// Update last alhpa/dither table pointer.

		mov	ebx,edx										// Clut pointer plus one.
		inc	edx

		lea		eax,MODIFY_FOR_ALPHA_DITHER_A
		mov		[eax-4],esi
		lea		eax,MODIFY_FOR_CLUT_POINTER_A
		mov		[eax-4],ebx
		lea		eax,MODIFY_FOR_CLUT_POINTER_P1_A
		mov		[eax-4],edx

		lea		eax,MODIFY_FOR_ALPHA_DITHER_B
		mov		[eax-4],esi
		lea		eax,MODIFY_FOR_CLUT_POINTER_B
		mov		[eax-4],ebx
		lea		eax,MODIFY_FOR_CLUT_POINTER_P1_B
		mov		[eax-4],edx

		lea		eax,MODIFY_FOR_ALPHA_DITHER_C
		mov		[eax-4],esi
		lea		eax,MODIFY_FOR_CLUT_POINTER_C
		mov		[eax-4],ebx
		lea		eax,MODIFY_FOR_CLUT_POINTER_P1_C
		mov		[eax-4],edx

		lea		eax,MODIFY_FOR_ALPHA_DITHER_D
		mov		[eax-4],esi
		lea		eax,MODIFY_FOR_CLUT_POINTER_D
		mov		[eax-4],ebx
		lea		eax,MODIFY_FOR_CLUT_POINTER_P1_D
		mov		[eax-4],edx

DONE_WITH_MODIFY:
		mov		edi,[plinc]							// Pointer to scanline increment.
		mov		esi,[pdtri]							// Pointer to polygon object.

		mov		eax,[pscan]							// Pointer to scanline object.
		push	ebp									// Save ebp.

		//
		// Local copies of edge stepping values.
		//
		// fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
		// pdtriGlbl		= pdtri;
		// pscanGlbl		= pscan;
		// fx_gour_inc		= pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		//
		mov		ebx,[edi]TGFogLinear.fxX
		mov		ecx,[edi]TGFogLinear.fxXDifference

		mov		[fx_inc],ebx
		mov		[fx_diff],ecx

		mov		[pdtriGlbl],esi
		mov		ecx,[fxDeltaXIntensity]

		mov		ebx,[edi]FakeTFlatGourFog.gourIntensity.fxIntensity.i4Fx
		and		ecx,(0x7fffffff >> (15 - LOG2_NUM_SHADES))			// Mask negative values.

		mov		[pscanGlbl],eax
		add		ecx,(1 << (LOG2_NUM_SHADES+17))						// The low bit of screen x.

		mov		[fx_gour_inc],ebx
		mov		[DeltaXIntensityAndX],ecx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebp,[esi]CDrawPolygonBase.iY			// Get current integer y.
		mov		ebx,[eax]FakeTFlatGourFog.fxX.i4Fx		// Get current fixed point x.

		shl		ebx,(LOG2_NUM_SHADES+1)					// Shift low 2 bits of x into position.
		and		ebp,3									// Mask low 2 bits of y.
	
		shl		ebp,(20 + LOG2_NUM_SHADES)				// Shift low 2 bits of y into position.
		and		ebx,(3 << (17+LOG2_NUM_SHADES))			// Mask shifted low 2 bits of x.	

		mov		edi,[eax]FakeTFlatGourFog.gourIntensity.fxIntensity.i4Fx
		or		ebp,ebx									// Combine low bits of x and y.

		or		edi,ebp									// Combine low bits of x and y with intensity.
		mov		ebx,[bEvenScanlinesOnly]

		mov		[fxIntensityTemp],edi					// Set the starting fog shading value.
		mov		ebp,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TFlatGourFog.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		add		ebx,ecx

		sar		ebx,16
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		edi,16
		mov		ebp,gsGlobals.pvScreen

		add		ecx,ebx

		sub		edi,ebx
		jge		short END_OF_SCANLINE					// no pixels to draw

		lea		ebp,[ebp + ecx*2]						// Calculate destination address.
		mov		ebx,[fxIntensityTemp]					// Load intensity value.

		mov		esi,ebx									// Copy shade/screen x,y value.
		xor		ecx,ecx									// Clear ecx

		shr		esi,12									// Only leave 4 fractional bits.

		//-----------------------------------------------------------------------
		// Left to right scanline.
		lea		eax,[ebp + edi*2]						// Calculate destination address.

		// Check alignement.
		test	eax,3
		jz		short ALIGNED

		// Check for one pixel before starting.
		inc		edi
		jz		short FINISH_REMAINDER

		mov		edx,[esi*4 + 0xDEADBEEF]				// Load dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_A:
		mov		eax,[u4MaskedColor]						// Load masked base color.

		or		eax,edx
		mov		edx,[DeltaXIntensityAndX]				// Step intensity & screen x.

		add		ebx,edx

		and		ebx,SY_SX_SHADE_MASK					// Mask off screen x and screen y carry.

		mov		esi,ebx									// Copy shade/screen x,y value.
		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_A:

		shr		esi,12									// Only leave 4 fractional bits.
		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_A:

		mov		[ebp + edi*2 - 2],cx					// Store pixel.

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

		//
		// Do two pixels at a time.
		//
INNER_LOOP_2P:
		mov		edx,[esi*4 + 0xDEADBEEF]				// Load dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_B:
		mov		eax,[u4MaskedColor]						// Load masked base color.

		or		eax,edx
		mov		edx,[DeltaXIntensityAndX]				// Step intensity & screen x.

		add		ebx,edx

		and		ebx,SY_SX_SHADE_MASK					// Mask off screen x and screen y carry.

		mov		esi,ebx									// Copy shade/screen x,y value.
		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_B:

		shr		esi,12									// Only leave 4 fractional bits.
		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_B:

		mov		edx,[esi*4 + 0xDEADBEEF]				// Load dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_C:
		mov		eax,[u4MaskedColor]						// Load masked base color.

		or		eax,edx
		mov		edx,[DeltaXIntensityAndX]				// Step intensity & screen x.

		add		ebx,edx

		and		ebx,SY_SX_SHADE_MASK					// Mask off screen x and screen y carry.

		mov		esi,ebx									// Copy shade/screen x,y value.
		mov		dl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_C:

		shr		esi,12									// Only leave 4 fractional bits.
		mov		dh,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_C:

		shl		edx,16									// Shift second pixel up.
		mov		eax,edi									// Copy edi (avoid AGI).

		or		edx,ecx									// Combine pixels
		add		edi,2

		mov		[ebp + eax*2 - 4],edx					// Store pixels.
		jle		short INNER_LOOP_2P

		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short END_OF_SCANLINE

		//
		// Finish left over pixel.
		//
FINISH_REMAINDER:
		mov		edx,[esi*4 + 0xDEADBEEF]				// Load dithered alpha value.
MODIFY_FOR_ALPHA_DITHER_D:
		mov		eax,[u4MaskedColor]						// Load masked base color.

		or		eax,edx
		mov		edx,[DeltaXIntensityAndX]				// Step intensity & screen x.

		add		ebx,edx

		and		ebx,SY_SX_SHADE_MASK					// Mask off screen x and screen y carry.

		mov		esi,ebx									// Copy shade/screen x,y value.
		mov		cl,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_D:

		shr		esi,12									// Only leave 4 fractional bits.
		mov		ch,[eax*2 + 0xDEADBEEF]					// Do CLUT lookup (high).
MODIFY_FOR_CLUT_POINTER_P1_D:

		mov		[ebp - 2],cx							// Store pixel.

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		// AGI on eax.

		mov		ebp,[eax]FakeTFlatGourFog.gourIntensity.fxIntensity.i4Fx
		mov		ebx,[eax]TFlatGourFog.fxXDifference

		add		ebp,[fx_gour_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebp,[fxIModDiv]
		add		ebx,0x00010000

		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		add		ebp,[fxIModDiv]
		sub		ebx,0x00010000

EDGE_DONE:
		mov		esi,[pdtriGlbl]

		// Store modified variables and do looping.
		mov		[eax]FakeTFlatGourFog.gourIntensity.fxIntensity.i4Fx,ebp
		mov		[eax]TFlatGourFog.fxXDifference,ebx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TFlatGourFog.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TGFogLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp
	}
}


#else // elif (DITHER_SIZE == 4)


//*********************************************************************************************
//
// Reference C++ implementation.
//
void DrawSubtriangle(TGFogPersp* pscan,	CDrawPolygon<TGFogPersp>* pdtri)
{
	typedef TGFogPersp::TPixel TDest;

	Assert(pscan);
	Assert(pdtri);

	int i_x_from;
	int i_x_to;
	int i_screen_index;
	int i_pixel;

	// Iterate through the scanlines that intersect the subtriangle.
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
			{
				TGFogPersp::TGouraud gour(pscan->gourIntensity);
				TGFogPersp::TIndex   index(pscan->indCoord, i_pixel);
				TDest* apix_screen = ((TDest*)pdtri->prasScreen->pSurface) + i_screen_index;

				int i_x = i_x_from;

				// Iterate left or right along the destination scanline.
				for (;;)
				{
					// Do next subdivision.
					int i_pixel_inner = index.i4StartSubdivision(i_pixel);

					TDest* apix_screen_inner = apix_screen + i_pixel;

					for (;;)
					{
//						int32  i4_fog_index     = int(gour.fxIntensity);
//						Assert(bWithin(i4_fog_index, 0, iNUM_TERRAIN_FOG_BANDS - 1));

						uint16 u2_source_pixel  = ((uint16*)pvTextureBitmap)[index.iGetIndex()];
						++index;
						u2_source_pixel &= lbAlphaTerrain.u2GetAlphaMask();
						u2_source_pixel |= lbAlphaTerrain.u4GetAlphaRefDither(i_x, pdtri->iY, gour.fxIntensity.i4Fx);

						apix_screen_inner[i_pixel_inner] = lbAlphaTerrain.au2Colour[u2_source_pixel];

						i_x++;

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
}


//*********************************************************************************************
void DrawSubtriangle(TGFogLinear* pscan, CDrawPolygon<TGFogLinear>* pdtri)
{
	typedef TGFogLinear::TPixel TDest;

	Assert(pscan);
	Assert(pdtri);

	int i_x_from;
	int i_x_to;
	int i_screen_index;
	int i_pixel;

	// Iterate through the scanlines that intersect the subtriangle.
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
			{
				TGFogLinear::TGouraud gour(pscan->gourIntensity);
				TGFogLinear::TIndex   index(pscan->indCoord, i_pixel);
				TDest* apix_screen = ((TDest*)pdtri->prasScreen->pSurface) + i_screen_index;

				int i_x = i_x_from;

				// Iterate left or right along the destination scanline.
				for (;;)
				{
					// Do next subdivision.
					int i_pixel_inner = index.i4StartSubdivision(i_pixel);

					TDest* apix_screen_inner = apix_screen + i_pixel;

					for (;;)
					{
//						int32  i4_fog_index     = int(gour.fxIntensity);
//						Assert(bWithin(i4_fog_index, 0, iNUM_TERRAIN_FOG_BANDS - 1));

						uint16 u2_source_pixel  = ((uint16*)pvTextureBitmap)[index.iGetIndex()];
						++index;
						u2_source_pixel &= lbAlphaTerrain.u2GetAlphaMask();
						u2_source_pixel |= lbAlphaTerrain.u4GetAlphaRefDither(i_x, pdtri->iY, gour.fxIntensity.i4Fx);

						apix_screen_inner[i_pixel_inner] = lbAlphaTerrain.au2Colour[u2_source_pixel];
						
						i_x++;

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
}


//*********************************************************************************************
void DrawSubtriangle(TFlatGourFog* pscan, CDrawPolygon<TFlatGourFog>* pdtri)
{
	typedef TFlatGourFog::TPixel TDest;

	Assert(pscan);
	Assert(pdtri);

	int i_x_from;
	int i_x_to;
	int i_screen_index;
	int i_pixel;

	// Iterate through the scanlines that intersect the subtriangle.
	do
	{
		i_x_from = pscan->fxX.i4Fx >> 16;
		i_x_to   = (pscan->fxX.i4Fx + pdtri->fxLineLength.i4Fx) >> 16;
		i_screen_index = pdtri->iLineStartIndex + i_x_to;
		i_pixel = i_x_from - i_x_to;

		// Draw if there are pixels to draw.
		if (i_x_to > i_x_from && !(bEvenScanlinesOnly && ((pdtri->iY & 1) == 0)))
		{
			TFlatGourFog::TGouraud gour(pscan->gourIntensity);
			TDest* apix_screen = ((TDest*)pdtri->prasScreen->pSurface) + i_screen_index;

			int i_x = i_x_from;

			// Iterate left along the destination scanline.
			do
			{
//				int32  i4_fog_index     = int(gour.fxIntensity);
//				Assert(bWithin(i4_fog_index, 0, iNUM_TERRAIN_FOG_BANDS - 1));

				uint16 u2_source_pixel  = u4ConstColour;

				u2_source_pixel &= lbAlphaTerrain.u2GetAlphaMask();
				u2_source_pixel |= lbAlphaTerrain.u4GetAlphaRefDither(i_x, pdtri->iY, gour.fxIntensity.i4Fx);

				apix_screen[i_pixel] = lbAlphaTerrain.au2Colour[u2_source_pixel];
				
				i_x++;

				// Increment values.
				++gour;
			}
			while (++i_pixel);
		}
		
		// Increment the base edge.
		++*pdtri->pedgeBase;

		// Set the new length of the line to be rasterized.
		pdtri->fxLineLength.i4Fx += pdtri->fxDeltaLineLength.i4Fx;

		// Get new starting pixel TIndex for the scanline.
		pdtri->iLineStartIndex += pdtri->prasScreen->iLinePixels;
	}
	while (++pdtri->iY < pdtri->iYTo);
}

#endif // else
