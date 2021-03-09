#pragma once

/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function.
 *		Optimized for the Pentium Pro Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P6/DrawSubTriangleTexEx.cpp                   $
 * 
 * 6     98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 5     97/12/03 1:27p Pkeet
 * Optimized more.
 * 
 * 4     97.12.01 4:26p Mmouni
 * P6 optimized version created from P5 optimized version.
 * 
 * 3     9/01/97 8:05p Rwyatt
 * ANDs added to enable tiling
 * 
 * 2     97/07/18 3:30p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 1     7/08/97 1:15p Rwyatt
 * Initial Implementation of Pentium Pro rendering specific optimizations
 * 
 ***********************************************************************************************
 ***********************************************************************************************
 *
 * 10    6/15/97 1:31a Bbell
 * Removed unnecessary include.
 * 
 * 9     97/06/12 5:56p Pkeet
 * Added capability to render only every other scanline and to double the number of scanlines.
 * 
 * 8     97/06/11 4:38p Pkeet
 * Fixed warnings.
 * 
 * 7     97/06/11 4:07p Pkeet
 * Added linear optimizations.
 * 
 * 6     97/06/11 3:44p Pkeet
 * Optimized transparency perspective correct.
 * 
 * 5     97/06/11 2:34p Pkeet
 * Assembly optimizations on the perspective non-transparent version.
 * 
 * 4     97/06/11 12:41p Pkeet
 * Put inner loop into assembly.
 * 
 * 3     97/06/11 11:55a Pkeet
 * Reorganized C++ code.
 * 
 * 2     97/06/10 11:02p Pkeet
 * Cleaned up the C++ versions a bit.
 * 
 * 1     97/06/10 9:11p Pkeet
 * Initial C++ version.
 * 
 **********************************************************************************************/

#include "common.hpp"

#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"


//*****************************************************************************************
//
// Perspective texture mapped with transparency for 16 bit mode.
//
void DrawSubtriangle(TTexturePerspTrans* pscan, CDrawPolygon<TTexturePerspTrans>* pdtri)
{
	typedef TTexturePerspTrans::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;

	TTexturePerspTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self modification.
		//
		mov		edx,[pvClutConversion]				// Clut conversion pointer.
		mov		ecx,[pvTextureBitmap]				// Texture pointer.

		mov		ebx,[pvLastClut]
		mov		eax,[pvLastTexture]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,edx
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx					// Update last texture pointer.
		mov		[pvLastClut],edx					// Update last clut pointer.
		
		lea		eax,MODIFY_FOR_TEXTUREPOINTER_A
		mov		[eax-4],ecx
		lea		eax,MODIFY_FOR_CLUT_A
		mov		[eax-4],edx

DONE_WITH_MODIFY:
		//
		// Local copies of edge stepping values.
		//
		// fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;
		// f_inc_uinvz = pdtri->pedgeBase->lineIncrement.indCoord.fUInvZ;
		// f_inc_vinvz = pdtri->pedgeBase->lineIncrement.indCoord.fVInvZ;
		// f_inc_invz  = pdtri->pedgeBase->lineIncrement.indCoord.fInvZ;
		//
		mov		eax,[plinc]

		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		mov		ebx,[eax]TTexturePerspTrans.fxX
		mov		edx,[eax]TTexturePerspTrans.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TTexturePerspTrans.indCoord.fUInvZ
		mov		edx,[eax]TTexturePerspTrans.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		ebx,[eax]TTexturePerspTrans.indCoord.fInvZ

		mov		f_inc_vinvz,edx
		mov		f_inc_invz,ebx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		push	ebp									// Save ebp.

		// Check to see if we sould skip this scanline.
		mov		eax,[esi]CDrawPolygonBase.iY
		mov		ebx,[bEvenScanlinesOnly]

		and		eax,ebx
		jnz		END_OF_SCANLINE

		// Setup for prespective correction and looping.
		PERSPECTIVE_SCANLINE_PROLOGUE(TTexturePerspTrans)

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

		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.

		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		and		edx,[u4TextureTileMask]				// Mask for tiling.
		mov		ebx,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction step.
		add		ecx,ebx								// Step V fraction.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		movzx	eax,[edx + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_A:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction step.

		add		esi,ebx								// Step U fraction.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt	// Integer step + U carry.
		add		eax,eax										// Double eax
		jz		short SKIP_TRANSPARENT						// Skip zero pixels.

		mov		ebp,[pvBaseOfLine]					// Load pointer to destination.
		mov		ax,[eax + 0xDEADBEEF]				// Do CLUT lookup.
MODIFY_FOR_CLUT_A:

		mov		[ebp + edi*2],ax					// Store pixel.
SKIP_TRANSPARENT:
		inc		edi									// Step destination/count.
		jnz		short INNER_LOOP

		// See if there are any pixels left.
		cmp		[i_pixel],0
		jz		END_OF_SCANLINE

		// Do perspective correction and looping.
		PERSPECTIVE_SCANLINE_EPILOGUE

END_OF_SCANLINE:
		pop		ebp						// Restore ebp.

		//
		// Increment the base edge.
		//
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		// Step length, line starting address.
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ebx,[esi]CDrawPolygonBase.iLineStartIndex

		add		eax,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ebx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TTexturePerspTrans.fxX.i4Fx
		mov		ebx,[ecx]TTexturePerspTrans.fxXDifference.i4Fx

		fld		[ecx]TTexturePerspTrans.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TTexturePerspTrans.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TTexturePerspTrans.indCoord.fInvZ
		fadd	[f_inc_invz]

		add		eax,[fx_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Underflow.
		add		ebx,0x00010000
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jle		short NO_OVERFLOW

		// Overflow.
		sub		ebx,0x00010000
		jmp		short BORROW_OR_CARRY

NO_OVERFLOW:
		fstp	[ecx]TTexturePerspTrans.indCoord.fInvZ
		fstp	[ecx]TTexturePerspTrans.indCoord.fVInvZ
		fstp	[ecx]TTexturePerspTrans.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		fstp	[ecx]TTexturePerspTrans.indCoord.fInvZ
		fstp	[ecx]TTexturePerspTrans.indCoord.fUInvZ
		fstp	[ecx]TTexturePerspTrans.indCoord.fVInvZ

FINISH_LOOPING:
		// Y Loop control.
		mov		[ecx]TTexturePerspTrans.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TTexturePerspTrans.fxXDifference.i4Fx,ebx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Perspective texture mapped with no transparency for 16 bit mode.
//
void DrawSubtriangle(TTexturePersp* pscan, CDrawPolygon<TTexturePersp>* pdtri)
{
	typedef TTexturePersp::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;

	TTexturePersp* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self modification.
		//
		mov		edx,[pvClutConversion]				// Clut conversion pointer.
		mov		ecx,[pvTextureBitmap]				// Texture pointer.
		
		mov		ebx,[pvLastClut]
		mov		eax,[pvLastTexture]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,edx
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx					// Update last texture pointer.
		mov		[pvLastClut],edx					// Update last clut pointer.

		lea		eax,MODIFY_FOR_TEXTUREPOINTER_A
		mov		[eax-4],ecx
		lea		eax,MODIFY_FOR_CLUT_A
		mov		[eax-4],edx

DONE_WITH_MODIFY:
		//
		// Local copies of edge stepping values.
		//
		// fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;
		// f_inc_uinvz = pdtri->pedgeBase->lineIncrement.indCoord.fUInvZ;
		// f_inc_vinvz = pdtri->pedgeBase->lineIncrement.indCoord.fVInvZ;
		// f_inc_invz  = pdtri->pedgeBase->lineIncrement.indCoord.fInvZ;
		//
		mov		eax,[plinc]

		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		mov		ebx,[eax]TTexturePersp.fxX
		mov		edx,[eax]TTexturePersp.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TTexturePersp.indCoord.fUInvZ
		mov		edx,[eax]TTexturePersp.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		ebx,[eax]TTexturePersp.indCoord.fInvZ

		mov		f_inc_vinvz,edx
		mov		f_inc_invz,ebx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		push	ebp									// Save ebp.

		// Check to see if we sould skip this scanline.
		mov		eax,[esi]CDrawPolygonBase.iY
		mov		ebx,[bEvenScanlinesOnly]

		and		eax,ebx
		jnz		END_OF_SCANLINE

		// Setup for prespective correction and looping.
		PERSPECTIVE_SCANLINE_PROLOGUE(TTexturePersp)

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

		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.

		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		and		edx,[u4TextureTileMask]				// Mask for tiling.
		mov		ebx,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction step.
		add		ecx,ebx								// Step V fraction.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction step.
		movzx	eax,byte ptr[edx + 0xDEADBEEF]		// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_A:

		add		eax,eax								// Double eax.
		add		esi,ebx								// Step U fraction.
		mov		ax,word ptr[eax + 0xDEADBEEF]		// Do CLUT lookup.
MODIFY_FOR_CLUT_A:

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt	// Integer step + U carry.
		mov		ebp,[pvBaseOfLine]							// Load pointer to destination.

		mov		[ebp + edi*2],ax					// Store pixel.
		inc		edi									// Step destination/count.
		jnz		short INNER_LOOP

		// See if there are any pixels left.
		cmp		[i_pixel],0
		je		END_OF_SCANLINE

		// Do perspective correction and looping.
		PERSPECTIVE_SCANLINE_EPILOGUE

END_OF_SCANLINE:
		pop		ebp						// Restore ebp.

		//
		// Increment the base edge.
		//
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		// Step length, line starting address.
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ebx,[esi]CDrawPolygonBase.iLineStartIndex

		add		eax,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ebx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TTexturePersp.fxX.i4Fx
		mov		ebx,[ecx]TTexturePersp.fxXDifference.i4Fx

		fld		[ecx]TTexturePersp.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TTexturePersp.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TTexturePersp.indCoord.fInvZ
		fadd	[f_inc_invz]

		add		eax,[fx_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Underflow.
		add		ebx,0x00010000
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jle		short NO_OVERFLOW

		// Overflow.
		sub		ebx,0x00010000
		jmp		short BORROW_OR_CARRY

NO_OVERFLOW:
		fstp	[ecx]TTexturePersp.indCoord.fInvZ
		fstp	[ecx]TTexturePersp.indCoord.fVInvZ
		fstp	[ecx]TTexturePersp.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		fstp	[ecx]TTexturePersp.indCoord.fInvZ
		fstp	[ecx]TTexturePersp.indCoord.fUInvZ
		fstp	[ecx]TTexturePersp.indCoord.fVInvZ

FINISH_LOOPING:
		// Y Loop control.
		mov		[ecx]TTexturePersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TTexturePersp.fxXDifference.i4Fx,ebx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Linear texture mapped with transparency in 16 bit mode.
//
void DrawSubtriangle(TTextureLinearTrans* pscan, CDrawPolygon<TTextureLinearTrans>* pdtri)
{
	typedef TTextureLinearTrans::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static CDrawPolygon<TTextureLinearTrans>* pdtriGlbl;
	static TTextureLinearTrans* pscanGlbl;

	TTextureLinearTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self modification.
		//
		mov		edx,[pvClutConversion]				// Clut conversion pointer.
		mov		ecx,[pvTextureBitmap]				// Texture pointer.
		
		mov		ebx,[pvLastClut]
		mov		eax,[pvLastTexture]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,edx
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx					// Update last texture pointer.
		mov		[pvLastClut],edx					// Update last clut pointer.

		lea		eax,MODIFY_FOR_TEXTUREPOINTER_A
		mov		[eax-4],ecx
		lea		eax,MODIFY_FOR_CLUT_A
		mov		[eax-4],edx

DONE_WITH_MODIFY:
		push	ebp									// Save ebp.
		mov		edi,[plinc]							// Pointer to scanline increment.

		mov		eax,[pscan]							// Pointer to scanline.
		mov		esi,[pdtri]							// Pointer to polygon.

		//
		// Copies of edge stepping values.
		//
		// fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
		// bf_u_inc			= pdtri->pedgeBase->lineIncrement.indCoord.bfU;
		// w1d_v_inc		= pdtri->pedgeBase->lineIncrement.indCoord.w1dV;
		// pdtriGlbl		= pscan;
		// pscanGlbl		= pdtri;
		//
		mov		ebx,[edi]TTextureLinearTrans.fxX
		mov		ecx,[edi]TTextureLinearTrans.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TTextureLinearTrans.indCoord.bfU.i4Int
		mov		ecx,[edi]TTextureLinearTrans.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TTextureLinearTrans.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TTextureLinearTrans.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriGlbl,esi
		mov		pscanGlbl,eax

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		// Check to see if we should skip this scanline.
		mov		ecx,[esi]CDrawPolygonBase.iY
		mov		ebx,[bEvenScanlinesOnly]

		and		ecx,ebx
		jnz		END_OF_SCANLINE

		// Determine the start and end of the scanline.
		mov		ebx,[eax]TTextureLinearTrans.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,ecx

		sar		ebx,16
		mov		edi,[eax]TTextureLinearTrans.fxX.i4Fx

		sar		edi,16
		add		edx,ebx

		sub		edi,ebx									// i_pixel
		jge		END_OF_SCANLINE

		//
		// edi = i_pixel
		// edx = i_screen_index
		// eax = pointer to scanline
		//
		mov		ebp,gsGlobals.pvScreen										// Pointer the screen.
		mov		ebx,[eax]TTextureLinearTrans.indCoord.bfU.i4Int				// Uint
		mov		esi,[eax]TTextureLinearTrans.indCoord.bfU.u4Frac			// Ufrac

		mov		ecx,[eax]TTextureLinearTrans.indCoord.w1dV.bfxValue.u4Frac	// Vfrac
		lea		ebp,[ebp+edx*2]												// Base of scanline.
		mov		edx,[eax]TTextureLinearTrans.indCoord.w1dV.bfxValue.i4Int	// Vint

		mov		[pvBaseOfLine],ebp					// Save pointer to base of line.
		add		edx,ebx								// Add Uint to Vint

		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		and		edx,[u4TextureTileMask]				// Mask for tiling.
		mov		ebx,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction step.
		add		ecx,ebx								// Step V fraction.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		movzx	eax,[edx + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_A:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction step.

		add		esi,ebx								// Step U fraction.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt	// Integer step + U carry.
		add		eax,eax										// Double eax
		jz		short SKIP_TRANSPARENT						// Skip zero pixels.

		mov		ebp,[pvBaseOfLine]					// Load pointer to destination.
		mov		ax,[eax + 0xDEADBEEF]				// Do CLUT lookup.
MODIFY_FOR_CLUT_A:

		mov		[ebp + edi*2],ax					// Store pixel.
SKIP_TRANSPARENT:
		inc		edi									// Step destination/count.
		jnz		short INNER_LOOP

END_OF_SCANLINE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TTextureLinearTrans.indCoord.bfU.u4Frac
		mov		edx,[eax]TTextureLinearTrans.indCoord.bfU.i4Int

		mov		edi,[eax]TTextureLinearTrans.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TTextureLinearTrans.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TTextureLinearTrans.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebx,[eax]TTextureLinearTrans.fxXDifference
		mov		ebp,[fx_diff]

		add		ebx,ebp
		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000
		mov		ebp,bfNegDU.i4Int

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,ebp
		mov		ebp,w1dNegDV.bfxValue.u4Frac

		add		esi,ebp
		jnc		short EDGE_DONE

		add		edi,[eax]TTextureLinearTrans.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000
		mov		ebp,bfNegDU.i4Int

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,ebp
		mov		ebp,w1dNegDV.bfxValue.u4Frac

		add		esi,ebp
		jnc		short EDGE_DONE

		add		edi,[eax]TTextureLinearTrans.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TTextureLinearTrans.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TTextureLinearTrans.indCoord.w1dV.bfxValue.i4Int,edi

		mov		[eax]TTextureLinearTrans.fxXDifference,ebx
		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TTextureLinearTrans.indCoord.bfU.u4Frac,ecx
		mov		[eax]TTextureLinearTrans.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TTextureLinearTrans.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TTextureLinearTrans.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop ebp
	}
}


//*****************************************************************************************
//
// Linear texture mapped with no transparency in 16 bit mode.
//
void DrawSubtriangle(TTextureLinear* pscan, CDrawPolygon<TTextureLinear>* pdtri)
{
	typedef TTextureLinear::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static CDrawPolygon<TTextureLinear>* pdtriGlbl;
	static TTextureLinear* pscanGlbl;

	TTextureLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self modification.
		//
		mov		edx,[pvClutConversion]				// Clut conversion pointer.
		mov		ecx,[pvTextureBitmap]				// Texture pointer.
		
		mov		ebx,[pvLastClut]
		mov		eax,[pvLastTexture]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,edx
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx					// Update last texture pointer.
		mov		[pvLastClut],edx					// Update last clut pointer.

		lea		eax,MODIFY_FOR_TEXTUREPOINTER_A
		mov		[eax-4],ecx
		lea		eax,MODIFY_FOR_CLUT_A
		mov		[eax-4],edx

DONE_WITH_MODIFY:
		push	ebp									// Save ebp.
		mov		edi,[plinc]							// Pointer to scanline increment.

		mov		eax,[pscan]							// Pointer to scanline.
		mov		esi,[pdtri]							// Pointer to polygon.

		//
		// Copies of edge stepping values.
		//
		// fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
		// bf_u_inc			= pdtri->pedgeBase->lineIncrement.indCoord.bfU;
		// w1d_v_inc		= pdtri->pedgeBase->lineIncrement.indCoord.w1dV;
		// pscanGlbl		= pscan;
		// pdtriGlbl		= pdtri;
		//
		mov		ebx,[edi]TTextureLinear.fxX
		mov		ecx,[edi]TTextureLinear.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TTextureLinear.indCoord.bfU.i4Int
		mov		ecx,[edi]TTextureLinear.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TTextureLinear.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TTextureLinear.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriGlbl,esi
		mov		pscanGlbl,eax

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		// Check to see if we should skip this scanline.
		mov		ecx,[esi]CDrawPolygonBase.iY
		mov		ebx,[bEvenScanlinesOnly]

		and		ecx,ebx
		jnz		END_OF_SCANLINE

		// Determine the start and end of the scanline.
		mov		ebx,[eax]TTextureLinear.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,ecx

		sar		ebx,16
		mov		edi,[eax]TTextureLinear.fxX.i4Fx

		sar		edi,16
		add		edx,ebx

		sub		edi,ebx									// i_pixel
		jge		END_OF_SCANLINE

		//
		// edi = i_pixel
		// edx = i_screen_index
		// eax = pointer to scanline
		//
		mov		ebp,gsGlobals.pvScreen									// Pointer the screen.
		mov		ebx,[eax]TTextureLinear.indCoord.bfU.i4Int				// Uint
		mov		esi,[eax]TTextureLinear.indCoord.bfU.u4Frac				// Ufrac

		mov		ecx,[eax]TTextureLinear.indCoord.w1dV.bfxValue.u4Frac	// Vfrac
		lea		ebp,[ebp+edx*2]											// Base of scanline.
		mov		edx,[eax]TTextureLinear.indCoord.w1dV.bfxValue.i4Int	// Vint

		mov		[pvBaseOfLine],ebp					// Save pointer to base of line.
		add		edx,ebx								// Add Uint to Vint

		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		and		edx,[u4TextureTileMask]				// Mask for tiling.
		mov		ebx,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction step.
		add		ecx,ebx								// Step V fraction.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction step.
		movzx	eax,[edx + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_A:

		add		eax,eax								// Double eax.
		add		esi,ebx								// Step U fraction.
		mov		ax,word ptr[eax + 0xDEADBEEF]		// Do CLUT lookup.
MODIFY_FOR_CLUT_A:

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt	// Integer step + U carry.
		mov		ebp,[pvBaseOfLine]							// Load pointer to destination.

		mov		[ebp + edi*2],ax					// Store pixel.
		inc		edi									// Step destination/count.
		jnz		short INNER_LOOP

END_OF_SCANLINE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TTextureLinear.indCoord.bfU.u4Frac
		mov		edx,[eax]TTextureLinear.indCoord.bfU.i4Int

		mov		edi,[eax]TTextureLinear.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TTextureLinear.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TTextureLinear.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebx,[eax]TTextureLinear.fxXDifference
		mov		ebp,[fx_diff]

		add		ebx,ebp
		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000
		mov		ebp,bfNegDU.i4Int

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,ebp
		mov		ebp,w1dNegDV.bfxValue.u4Frac

		add		esi,ebp
		jnc		short EDGE_DONE

		add		edi,[eax]TTextureLinear.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000
		mov		ebp,bfNegDU.i4Int

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,ebp
		mov		ebp,w1dNegDV.bfxValue.u4Frac

		add		esi,ebp
		jnc		short EDGE_DONE

		add		edi,[eax]TTextureLinear.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TTextureLinear.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TTextureLinear.indCoord.w1dV.bfxValue.i4Int,edi

		mov		[eax]TTextureLinear.fxXDifference,ebx
		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TTextureLinear.indCoord.bfU.u4Frac,ecx
		mov		[eax]TTextureLinear.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TTextureLinear.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TTextureLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop ebp
	}
}
