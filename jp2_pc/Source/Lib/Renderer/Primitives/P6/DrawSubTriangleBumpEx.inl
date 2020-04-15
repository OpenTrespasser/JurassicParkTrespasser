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
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P6/DrawSubTriangleBumpEx.cpp                  $
 * 
 * 8     98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 7     98/01/22 5:39p Mmouni
 * Added support for 16-bit bump-maps.
 * 
 * 6     97.12.01 4:26p Mmouni
 * P6 optimized version created from P5 optimized version.
 * 
 * 5     9/01/97 8:05p Rwyatt
 * ANDs added to enable tiling
 * 
 * 4     8/25/97 5:26p Rwyatt
 * New bump mapping code for linear non-transparent
 * 
 * 3     97/07/18 3:29p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 2     97/07/16 15:49 Speter
 * Commented out redundant static delcarations now that all DrawSubTriangles are included into
 * one file (now just declared in DrawSubTriangleEx.cpp.)
 * 
 * 1     7/08/97 1:15p Rwyatt
 * Initial Implementation of Pentium Pro rendering specific optimizations
 * 
 ***********************************************************************************************
 ***********************************************************************************************
 * 
 * 8     6/15/97 2:40a Rwyatt
 * Optimized perspective bump mapping to be in line with the non perspective.
 * 
 * 7     6/15/97 1:31a Bbell
 * Removed unnecessary include.
 * 
 * 6     97/06/12 5:56p Pkeet
 * Added capability to render only every other scanline and to double the number of scanlines.
 * 
 * 5     6/12/97 3:15p Rwyatt
 * Linear bump mapping optimized for PENTIUM PRO only, on this devices it is about 10 - 20%
 * faster. On a pentium this code will be much (30%) slower due to lots of data dependencies and
 * AGIs which the pro is immune to.
 * 
 * 4     6/11/97 11:29p Rwyatt
 * Optimized scan setup time and for both linear bump mappers.
 * 
 * 3     6/11/97 7:08p Rwyatt
 * Removed AGIs from UV walking in Transparent linear
 * 
 * 2     97/06/10 11:15p Pkeet
 * Added self-modifying code for the increment and the decrement.
 * 
 * 1     97/06/10 2:27p Pkeet
 * Moved the bumpmap routines here from DrawSubTriangle.
 * 
 * 9     97/06/10 1:20p Pkeet
 * Added initial assembly optimizations for bumpmap linear routines.
 * 
 * 8     97/06/10 12:53p Pkeet
 * Fixed bug in bumpmap assembly.
 * 
 * 7     97/06/10 10:51a Pkeet
 * Added initial assembly optimization passes for perspective corrected bumpmapping for both
 * transparent and non-transparent versions.
 * 
 * 6     97/06/09 6:38p Pkeet
 * Algorithmic optimizations.
 * 
 * 5     97/06/08 4:16p Pkeet
 * Added perspective correction bump map primitives.
 * 
 * 4     97/06/06 8:51p Pkeet
 * Added linear.
 * 
 * 3     97/06/06 6:44p Pkeet
 * Moved the inner loop to self-modifying code.
 * 
 * 2     97/06/06 4:36p Pkeet
 * It works again.
 * 
 * 1     97/06/04 6:46p Pkeet
 * Initial specialized version of DrawSubTriangle.
 * 
 **********************************************************************************************/

#include "Common.hpp"

#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"


#if (iBUMPMAP_RESOLUTION == 16)


//*****************************************************************************************
//
// Bumpmapped, Perspective correct and transparent for 16 bit screen.
//
void DrawSubtriangle(TBumpPerspTrans* pscan, CDrawPolygon<TBumpPerspTrans>* pdtri)
{
	typedef TBumpPerspTrans::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastBumpToIntensity = 0;

	TBumpPerspTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self-modifications here.
		//
		mov		eax,[pvTextureBitmap]					// Texuremap pointer.
		mov		ebx,[pvClutConversion]					// Clut conversion pointer.

		mov		edx,[au1SubBumpToIntensity]				// Pointer to the angle to intensity lookup table.
		mov		ecx,[pvLastTexture]

		mov		esi,[pvLastClut]
		mov		edi,[pvLastBumpToIntensity]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,esi
		jne		short DO_MODIFY

		cmp		edx,edi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],eax
		mov		[pvLastBumpToIntensity],edx
		mov		[pvLastClut],ebx

		lea		edi,MODIFY_FOR_TEXTUREPOINTER
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION
		mov		[edi-4],ebx

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

		mov		ebx,[eax]TBumpPerspTrans.fxX
		mov		edx,[eax]TBumpPerspTrans.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TBumpPerspTrans.indCoord.fUInvZ
		mov		edx,[eax]TBumpPerspTrans.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		ebx,[eax]TBumpPerspTrans.indCoord.fInvZ

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
		PERSPECTIVE_SCANLINE_PROLOGUE(TBumpPerspTrans)

		//
		// eax = i_pixel
		// edi = inner loop count
		// ecx,edx,esi = texture values
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.
		mov		ebx,[i_screen_index]				// Load scanline offset.

		add		ebx,eax								// Add scanline offset to i_pixel
		mov		[i_pixel],eax						// Save i_pixel.

		mov		eax,[u4TextureTileMask]				// Load tiling mask.

		and		edx,eax								// Mask for tiling.
		lea		ebp,[ebp+ebx*2]						// Base of span in ebp.

		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.
		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		movzx	ebx,word ptr[edx*2 + 0xDEADBEEF]		// Read texture value.
MODIFY_FOR_TEXTUREPOINTER:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		test	ebx,0xfc00								// Test color value of pixel.
		jz		short SKIP_TRANSPARENT					// Skip over transparent pixels.
		mov		eax,ebx									// Copy texture value.

		sub		ebx,[u4LightTheta]						// Subtract light theta.
		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		shr		eax,10									// Shift color value down.
		movzx	ebx,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY:

		shl		ebx,7									// Shift intensity up.
		mov		ax,word ptr[eax*2 + ebx + 0xDEADBEEF]	// CLUT lookup.
MODIFY_FOR_CLUT_CONVERSION:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],ax				// Write to frame buffer.
		inc		edi
		jnz		short INNER_LOOP

		jmp		short EXIT_INNER_LOOP

SKIP_TRANSPARENT:
		add		ecx,ebp									// Step the V fraction.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.
		mov		eax,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		and		edx,eax									// Mask for tiling.

		inc		edi										// Step index.
		jnz		short INNER_LOOP

EXIT_INNER_LOOP:
		// See if there are any pixels left.
		cmp		[i_pixel],0
		jz		END_OF_SCANLINE

		// Do perspective correction and looping.
		PERSPECTIVE_SCANLINE_EPILOGUE

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
		add		ebx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TBumpPerspTrans.fxX.i4Fx
		mov		ebx,[ecx]TBumpPerspTrans.fxXDifference.i4Fx

		fld		[ecx]TBumpPerspTrans.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TBumpPerspTrans.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TBumpPerspTrans.indCoord.fInvZ
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
		fstp	[ecx]TBumpPerspTrans.indCoord.fInvZ
		fstp	[ecx]TBumpPerspTrans.indCoord.fVInvZ
		fstp	[ecx]TBumpPerspTrans.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		fstp	[ecx]TBumpPerspTrans.indCoord.fInvZ
		fstp	[ecx]TBumpPerspTrans.indCoord.fUInvZ
		fstp	[ecx]TBumpPerspTrans.indCoord.fVInvZ

FINISH_LOOPING:
		// Y Loop control.
		mov		[ecx]TBumpPerspTrans.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TBumpPerspTrans.fxXDifference.i4Fx,ebx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Bumpmapped and Perspective correct for 16 bit screen.
//
void DrawSubtriangle(TBumpPersp* pscan, CDrawPolygon<TBumpPersp>* pdtri)
{
	typedef TBumpPersp::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastBumpToIntensity = 0;

	TBumpPersp* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self-modifications here.
		//
		mov		eax,[pvTextureBitmap]					// Texuremap pointer.
		mov		ebx,[pvClutConversion]					// Clut conversion pointer.

		mov		edx,[au1SubBumpToIntensity]				// Pointer to the angle to intensity lookup table.
		mov		ecx,[pvLastTexture]

		mov		esi,[pvLastClut]
		mov		edi,[pvLastBumpToIntensity]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,esi
		jne		short DO_MODIFY

		cmp		edx,edi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],eax
		mov		[pvLastBumpToIntensity],edx
		mov		[pvLastClut],ebx

		lea		edi,MODIFY_FOR_TEXTUREPOINTER
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION
		mov		[edi-4],ebx

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

		mov		ebx,[eax]TBumpPersp.fxX
		mov		edx,[eax]TBumpPersp.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TBumpPersp.indCoord.fUInvZ
		mov		edx,[eax]TBumpPersp.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		ebx,[eax]TBumpPersp.indCoord.fInvZ

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
		jnz		short END_OF_SCANLINE

		// Setup for prespective correction and looping.
		PERSPECTIVE_SCANLINE_PROLOGUE(TBumpPersp)

		//
		// eax = i_pixel
		// edi = inner loop count
		// ecx,edx,esi = texture values
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.
		mov		ebx,[i_screen_index]				// Load scanline offset.

		add		ebx,eax								// Add scanline offset to i_pixel
		mov		[i_pixel],eax						// Save i_pixel.

		mov		eax,[u4TextureTileMask]				// Load tiling mask.

		and		edx,eax								// Mask for tiling.
		lea		ebp,[ebp+ebx*2]						// Base of span in ebp.

		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.
		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		movzx	ebx,word ptr[edx*2 + 0xDEADBEEF]		// Read texture value.
MODIFY_FOR_TEXTUREPOINTER:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		eax,ebx									// Copy texture value.

		sub		ebx,[u4LightTheta]						// Subtract light theta.
		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		shr		eax,10									// Shift color value down.
		movzx	ebx,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY:

		shl		ebx,7									// Shift intensity up.
		mov		ax,word ptr[eax*2 + ebx + 0xDEADBEEF]	// CLUT lookup.
MODIFY_FOR_CLUT_CONVERSION:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],ax				// Write to frame buffer.
		inc		edi
		jnz		short INNER_LOOP

		// See if there are any pixels left.
		cmp		[i_pixel],0
		jz		END_OF_SCANLINE

		// Do perspective correction and looping.
		PERSPECTIVE_SCANLINE_EPILOGUE

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
		add		ebx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TBumpPersp.fxX.i4Fx
		mov		ebx,[ecx]TBumpPersp.fxXDifference.i4Fx

		fld		[ecx]TBumpPersp.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TBumpPersp.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TBumpPersp.indCoord.fInvZ
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
		fstp	[ecx]TBumpPersp.indCoord.fInvZ
		fstp	[ecx]TBumpPersp.indCoord.fVInvZ
		fstp	[ecx]TBumpPersp.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		fstp	[ecx]TBumpPersp.indCoord.fInvZ
		fstp	[ecx]TBumpPersp.indCoord.fUInvZ
		fstp	[ecx]TBumpPersp.indCoord.fVInvZ

FINISH_LOOPING:
		// Y Loop control.
		mov		[ecx]TBumpPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TBumpPersp.fxXDifference.i4Fx,ebx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Bumpmapped, Linear and transparent for 16 bit screen.
//
void DrawSubtriangle(TBumpLinearTrans* pscan, CDrawPolygon<TBumpLinearTrans>* pdtri)
{
	typedef TBumpLinearTrans::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastBumpToIntensity = 0;
	static CDrawPolygon<TBumpLinearTrans>* pdtriGlbl;
	static TBumpLinearTrans* pscanGlbl;

	TBumpLinearTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self-modifications here.
		//
		mov		eax,[pvTextureBitmap]					// Texuremap pointer.
		mov		ebx,[pvClutConversion]					// Clut conversion pointer.

		mov		edx,[au1SubBumpToIntensity]				// Pointer to the angle to intensity lookup table.
		mov		ecx,[pvLastTexture]

		mov		esi,[pvLastClut]
		mov		edi,[pvLastBumpToIntensity]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,esi
		jne		short DO_MODIFY

		cmp		edx,edi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],eax
		mov		[pvLastBumpToIntensity],edx
		mov		[pvLastClut],ebx

		lea		edi,MODIFY_FOR_TEXTUREPOINTER
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION
		mov		[edi-4],ebx

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
		// pscanCopy		= pscan;
		// pdtriCopy		= pdtri;
		//
		mov		ebx,[edi]TBumpLinearTrans.fxX
		mov		ecx,[edi]TBumpLinearTrans.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TBumpLinearTrans.indCoord.bfU.i4Int
		mov		ecx,[edi]TBumpLinearTrans.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TBumpLinearTrans.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TBumpLinearTrans.indCoord.w1dV.bfxValue.u4Frac

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
		mov		ebx,[eax]TBumpLinearTrans.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,ecx

		sar		ebx,16
		mov		edi,[eax]TBumpLinearTrans.fxX.i4Fx

		sar		edi,16
		add		edx,ebx

		sub		edi,ebx									// i_pixel
		jge		END_OF_SCANLINE

		//
		// edi = i_pixel
		// edx = i_screen_index
		// ecx = pointer to scanline
		//
		mov		ebp,gsGlobals.pvScreen								// Pointer the screen.
		mov		ebx,[eax]TBumpLinearTrans.indCoord.bfU.i4Int		// Uint

		mov		esi,[eax]TBumpLinearTrans.indCoord.bfU.u4Frac			// Ufrac
		mov		ecx,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.u4Frac	// Vfrac

		lea		ebp,[ebp+edx*2]											// Base of scanline.
		mov		edx,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.i4Int	// Vint

		add		edx,ebx													// Add Uint to Vint
		mov		ebx,[u4TextureTileMask]									// Load tiling mask.
		and		edx,ebx									// Mask for tiling.

		mov		[pvBaseOfLine],ebp						// Save pointer to base of line.
		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		movzx	ebx,word ptr[edx*2 + 0xDEADBEEF]		// Read texture value.
MODIFY_FOR_TEXTUREPOINTER:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		test	ebx,0xfc00								// Test color value of pixel.
		jz		short SKIP_TRANSPARENT					// Skip over transparent pixels.
		mov		eax,ebx									// Copy texture value.

		sub		ebx,[u4LightTheta]						// Subtract light theta.
		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		shr		eax,10									// Shift color value down.
		movzx	ebx,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY:

		shl		ebx,7									// Shift intensity up.
		mov		ax,word ptr[eax*2 + ebx + 0xDEADBEEF]	// CLUT lookup.
MODIFY_FOR_CLUT_CONVERSION:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],ax				// Write to frame buffer.
		inc		edi
		jnz		short INNER_LOOP

		jmp		short END_OF_SCANLINE

SKIP_TRANSPARENT:
		add		ecx,ebp									// Step the V fraction.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.
		mov		eax,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		and		edx,eax									// Mask for tiling.

		inc		edi										// Step index.
		jnz		short INNER_LOOP

END_OF_SCANLINE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TBumpLinearTrans.indCoord.bfU.u4Frac
		mov		edx,[eax]TBumpLinearTrans.indCoord.bfU.i4Int

		mov		edi,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TBumpLinearTrans.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebx,[eax]TBumpLinearTrans.fxXDifference
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

		add		edi,[eax]TBumpLinearTrans.indCoord.w1dV.iOffsetPerLine
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

		add		edi,[eax]TBumpLinearTrans.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.i4Int,edi

		mov		[eax]TBumpLinearTrans.fxXDifference,ebx
		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TBumpLinearTrans.indCoord.bfU.u4Frac,ecx
		mov		[eax]TBumpLinearTrans.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TBumpLinearTrans.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TBumpLinearTrans.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp					// Restore the base pointer.
	}
}


//*****************************************************************************************
//
// Bumpmapped and Linear for 16 bit screen.
//
void DrawSubtriangle(TBumpLinear* pscan, CDrawPolygon<TBumpLinear>* pdtri)
{
	typedef TBumpLinear::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastBumpToIntensity = 0;
	static CDrawPolygon<TBumpLinear>* pdtriGlbl;
	static TBumpLinear* pscanGlbl;

	TBumpLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self-modifications here.
		//
		mov		eax,[pvTextureBitmap]					// Texuremap pointer.
		mov		ebx,[pvClutConversion]					// Clut conversion pointer.

		mov		edx,[au1SubBumpToIntensity]				// Pointer to the angle to intensity lookup table.
		mov		ecx,[pvLastTexture]

		mov		esi,[pvLastClut]
		mov		edi,[pvLastBumpToIntensity]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,esi
		jne		short DO_MODIFY

		cmp		edx,edi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],eax
		mov		[pvLastBumpToIntensity],edx
		mov		[pvLastClut],ebx

		lea		edi,MODIFY_FOR_TEXTUREPOINTER
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION
		mov		[edi-4],ebx

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
		// pscanCopy		= pscan;
		// pdtriCopy		= pdtri;
		//
		mov		ebx,[edi]TBumpLinear.fxX
		mov		ecx,[edi]TBumpLinear.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TBumpLinear.indCoord.bfU.i4Int
		mov		ecx,[edi]TBumpLinear.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TBumpLinear.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TBumpLinear.indCoord.w1dV.bfxValue.u4Frac

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
		mov		ebx,[eax]TBumpLinear.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,ecx

		sar		ebx,16
		mov		edi,[eax]TBumpLinear.fxX.i4Fx

		sar		edi,16
		add		edx,ebx

		sub		edi,ebx									// i_pixel
		jge		END_OF_SCANLINE

		//
		// edi = i_pixel
		// edx = i_screen_index
		// ecx = pointer to scanline
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.

		mov		ebx,[eax]TBumpLinear.indCoord.bfU.i4Int				// Uint
		mov		esi,[eax]TBumpLinear.indCoord.bfU.u4Frac			// Ufrac

		lea		ebp,[ebp+edx*2]										// Base of scanline.
		mov		edx,[eax]TBumpLinear.indCoord.w1dV.bfxValue.i4Int	// Vint

		add		edx,ebx												// Add Uint to Vint
		mov		ecx,[eax]TBumpLinear.indCoord.w1dV.bfxValue.u4Frac	// Vfrac

		mov		ebx,[u4TextureTileMask]				// Load tiling mask.
		and		edx,ebx								// Mask for tiling.

		mov		[pvBaseOfLine],ebp					// Save pointer to base of line.
		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		movzx	ebx,word ptr[edx*2 + 0xDEADBEEF]		// Read texture value.
MODIFY_FOR_TEXTUREPOINTER:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		eax,ebx									// Copy texture value.

		sub		ebx,[u4LightTheta]						// Subtract light theta.
		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		shr		eax,10									// Shift color value down.
		movzx	ebx,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY:

		shl		ebx,7									// Shift intensity up.
		mov		ax,word ptr[eax*2 + ebx + 0xDEADBEEF]	// CLUT lookup.
MODIFY_FOR_CLUT_CONVERSION:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],ax				// Write to frame buffer.
		inc		edi
		jnz		short INNER_LOOP

END_OF_SCANLINE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TBumpLinear.indCoord.bfU.u4Frac
		mov		edx,[eax]TBumpLinear.indCoord.bfU.i4Int

		mov		edi,[eax]TBumpLinear.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TBumpLinear.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TBumpLinear.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebx,[eax]TBumpLinear.fxXDifference
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

		add		edi,[eax]TBumpLinear.indCoord.w1dV.iOffsetPerLine
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

		add		edi,[eax]TBumpLinear.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TBumpLinear.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TBumpLinear.indCoord.w1dV.bfxValue.i4Int,edi

		mov		[eax]TBumpLinear.fxXDifference,ebx
		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TBumpLinear.indCoord.bfU.u4Frac,ecx
		mov		[eax]TBumpLinear.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TBumpLinear.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TBumpLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp				// Restore the base pointer.
	}
}


#elif (iBUMPMAP_RESOLUTION == 32)	// if (iBUMPMAP_RESOLUTION == 16)


//*****************************************************************************************
//
// Bumpmapped, Perspective correct and transparent for 16 bit screen.
//
void DrawSubtriangle(TBumpPerspTrans* pscan, CDrawPolygon<TBumpPerspTrans>* pdtri)
{
	typedef TBumpPerspTrans::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastBumpToIntensity = 0;

	TBumpPerspTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self-modifications here.
		//
		mov		eax,[pvTextureBitmap]					// Texuremap pointer.
		mov		ebx,[pvClutConversion]					// Clut conversion pointer.

		mov		edx,[au1SubBumpToIntensity]				// Pointer to the angle to intensity lookup table.
		mov		ecx,[pvLastTexture]

		mov		esi,[pvLastClut]
		mov		edi,[pvLastBumpToIntensity]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,esi
		jne		short DO_MODIFY

		cmp		edx,edi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],eax
		mov		[pvLastBumpToIntensity],edx
		mov		[pvLastClut],ebx

		lea		edi,MODIFY_FOR_TEXTUREPOINTER
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION
		mov		[edi-4],ebx

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

		mov		ebx,[eax]TBumpPerspTrans.fxX
		mov		edx,[eax]TBumpPerspTrans.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TBumpPerspTrans.indCoord.fUInvZ
		mov		edx,[eax]TBumpPerspTrans.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		ebx,[eax]TBumpPerspTrans.indCoord.fInvZ

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
		PERSPECTIVE_SCANLINE_PROLOGUE(TBumpPerspTrans)

		//
		// eax = i_pixel
		// edi = inner loop count
		// ecx,edx,esi = texture values
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.
		mov		ebx,[i_screen_index]				// Load scanline offset.

		add		ebx,eax								// Add scanline offset to i_pixel
		mov		[i_pixel],eax						// Save i_pixel.

		mov		eax,[u4TextureTileMask]				// Load tiling mask.

		and		edx,eax								// Mask for tiling.
		lea		ebp,[ebp+ebx*2]						// Base of span in ebp.

		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.
		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		test	ebx,0xff000000							// Test color value of pixel.
		jz		short SKIP_TRANSPARENT					// Skip over transparent pixels.
		mov		eax,ebx									// Copy texture value.

		sub		ebx,[u4LightTheta]						// Subtract light theta.
		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		shr		eax,24									// Shift color value down.
		movzx	ebx,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY:

		shl		ebx,9									// Shift intensity up.
		mov		ax,word ptr[eax*2 + ebx + 0xDEADBEEF]	// CLUT lookup.
MODIFY_FOR_CLUT_CONVERSION:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],ax				// Write to frame buffer.
		inc		edi
		jnz		short INNER_LOOP

		jmp		short EXIT_INNER_LOOP

SKIP_TRANSPARENT:
		add		ecx,ebp									// Step the V fraction.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.
		mov		eax,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		and		edx,eax									// Mask for tiling.

		inc		edi										// Step index.
		jnz		short INNER_LOOP

EXIT_INNER_LOOP:
		// See if there are any pixels left.
		cmp		[i_pixel],0
		jz		END_OF_SCANLINE

		// Do perspective correction and looping.
		PERSPECTIVE_SCANLINE_EPILOGUE

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
		add		ebx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TBumpPerspTrans.fxX.i4Fx
		mov		ebx,[ecx]TBumpPerspTrans.fxXDifference.i4Fx

		fld		[ecx]TBumpPerspTrans.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TBumpPerspTrans.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TBumpPerspTrans.indCoord.fInvZ
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
		fstp	[ecx]TBumpPerspTrans.indCoord.fInvZ
		fstp	[ecx]TBumpPerspTrans.indCoord.fVInvZ
		fstp	[ecx]TBumpPerspTrans.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		fstp	[ecx]TBumpPerspTrans.indCoord.fInvZ
		fstp	[ecx]TBumpPerspTrans.indCoord.fUInvZ
		fstp	[ecx]TBumpPerspTrans.indCoord.fVInvZ

FINISH_LOOPING:
		// Y Loop control.
		mov		[ecx]TBumpPerspTrans.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TBumpPerspTrans.fxXDifference.i4Fx,ebx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Bumpmapped and Perspective correct for 16 bit screen.
//
void DrawSubtriangle(TBumpPersp* pscan, CDrawPolygon<TBumpPersp>* pdtri)
{
	typedef TBumpPersp::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastBumpToIntensity = 0;

	TBumpPersp* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self-modifications here.
		//
		mov		eax,[pvTextureBitmap]					// Texuremap pointer.
		mov		ebx,[pvClutConversion]					// Clut conversion pointer.

		mov		edx,[au1SubBumpToIntensity]				// Pointer to the angle to intensity lookup table.
		mov		ecx,[pvLastTexture]

		mov		esi,[pvLastClut]
		mov		edi,[pvLastBumpToIntensity]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,esi
		jne		short DO_MODIFY

		cmp		edx,edi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],eax
		mov		[pvLastBumpToIntensity],edx
		mov		[pvLastClut],ebx

		lea		edi,MODIFY_FOR_TEXTUREPOINTER
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION
		mov		[edi-4],ebx

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

		mov		ebx,[eax]TBumpPersp.fxX
		mov		edx,[eax]TBumpPersp.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TBumpPersp.indCoord.fUInvZ
		mov		edx,[eax]TBumpPersp.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		ebx,[eax]TBumpPersp.indCoord.fInvZ

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
		jnz		short END_OF_SCANLINE

		// Setup for prespective correction and looping.
		PERSPECTIVE_SCANLINE_PROLOGUE(TBumpPersp)

		//
		// eax = i_pixel
		// edi = inner loop count
		// ecx,edx,esi = texture values
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.
		mov		ebx,[i_screen_index]				// Load scanline offset.

		add		ebx,eax								// Add scanline offset to i_pixel
		mov		[i_pixel],eax						// Save i_pixel.

		mov		eax,[u4TextureTileMask]				// Load tiling mask.

		and		edx,eax								// Mask for tiling.
		lea		ebp,[ebp+ebx*2]						// Base of span in ebp.

		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.
		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		eax,ebx									// Copy texture value.

		sub		ebx,[u4LightTheta]						// Subtract light theta.
		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		shr		eax,24									// Shift color value down.
		movzx	ebx,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY:

		shl		ebx,9									// Shift intensity up.
		mov		ax,word ptr[eax*2 + ebx + 0xDEADBEEF]	// CLUT lookup.
MODIFY_FOR_CLUT_CONVERSION:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],ax				// Write to frame buffer.
		inc		edi
		jnz		short INNER_LOOP

		// See if there are any pixels left.
		cmp		[i_pixel],0
		jz		END_OF_SCANLINE

		// Do perspective correction and looping.
		PERSPECTIVE_SCANLINE_EPILOGUE

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
		add		ebx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TBumpPersp.fxX.i4Fx
		mov		ebx,[ecx]TBumpPersp.fxXDifference.i4Fx

		fld		[ecx]TBumpPersp.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TBumpPersp.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TBumpPersp.indCoord.fInvZ
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
		fstp	[ecx]TBumpPersp.indCoord.fInvZ
		fstp	[ecx]TBumpPersp.indCoord.fVInvZ
		fstp	[ecx]TBumpPersp.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		fstp	[ecx]TBumpPersp.indCoord.fInvZ
		fstp	[ecx]TBumpPersp.indCoord.fUInvZ
		fstp	[ecx]TBumpPersp.indCoord.fVInvZ

FINISH_LOOPING:
		// Y Loop control.
		mov		[ecx]TBumpPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TBumpPersp.fxXDifference.i4Fx,ebx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Bumpmapped, Linear and transparent for 16 bit screen.
//
void DrawSubtriangle(TBumpLinearTrans* pscan, CDrawPolygon<TBumpLinearTrans>* pdtri)
{
	typedef TBumpLinearTrans::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastBumpToIntensity = 0;
	static CDrawPolygon<TBumpLinearTrans>* pdtriGlbl;
	static TBumpLinearTrans* pscanGlbl;

	TBumpLinearTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self-modifications here.
		//
		mov		eax,[pvTextureBitmap]					// Texuremap pointer.
		mov		ebx,[pvClutConversion]					// Clut conversion pointer.

		mov		edx,[au1SubBumpToIntensity]				// Pointer to the angle to intensity lookup table.
		mov		ecx,[pvLastTexture]

		mov		esi,[pvLastClut]
		mov		edi,[pvLastBumpToIntensity]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,esi
		jne		short DO_MODIFY

		cmp		edx,edi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],eax
		mov		[pvLastBumpToIntensity],edx
		mov		[pvLastClut],ebx

		lea		edi,MODIFY_FOR_TEXTUREPOINTER
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION
		mov		[edi-4],ebx

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
		// pscanCopy		= pscan;
		// pdtriCopy		= pdtri;
		//
		mov		ebx,[edi]TBumpLinearTrans.fxX
		mov		ecx,[edi]TBumpLinearTrans.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TBumpLinearTrans.indCoord.bfU.i4Int
		mov		ecx,[edi]TBumpLinearTrans.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TBumpLinearTrans.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TBumpLinearTrans.indCoord.w1dV.bfxValue.u4Frac

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
		mov		ebx,[eax]TBumpLinearTrans.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,ecx

		sar		ebx,16
		mov		edi,[eax]TBumpLinearTrans.fxX.i4Fx

		sar		edi,16
		add		edx,ebx

		sub		edi,ebx									// i_pixel
		jge		END_OF_SCANLINE

		//
		// edi = i_pixel
		// edx = i_screen_index
		// ecx = pointer to scanline
		//
		mov		ebp,gsGlobals.pvScreen								// Pointer the screen.
		mov		ebx,[eax]TBumpLinearTrans.indCoord.bfU.i4Int		// Uint

		mov		esi,[eax]TBumpLinearTrans.indCoord.bfU.u4Frac			// Ufrac
		mov		ecx,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.u4Frac	// Vfrac

		lea		ebp,[ebp+edx*2]											// Base of scanline.
		mov		edx,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.i4Int	// Vint

		add		edx,ebx													// Add Uint to Vint
		mov		ebx,[u4TextureTileMask]									// Load tiling mask.
		and		edx,ebx									// Mask for tiling.

		mov		[pvBaseOfLine],ebp						// Save pointer to base of line.
		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		test	ebx,0xff000000							// Test color value of pixel.
		jz		short SKIP_TRANSPARENT					// Skip over transparent pixels.
		mov		eax,ebx									// Copy texture value.

		sub		ebx,[u4LightTheta]						// Subtract light theta.
		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		shr		eax,24									// Shift color value down.
		movzx	ebx,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY:

		shl		ebx,9									// Shift intensity up.
		mov		ax,word ptr[eax*2 + ebx + 0xDEADBEEF]	// CLUT lookup.
MODIFY_FOR_CLUT_CONVERSION:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],ax				// Write to frame buffer.
		inc		edi
		jnz		short INNER_LOOP

		jmp		short END_OF_SCANLINE

SKIP_TRANSPARENT:
		add		ecx,ebp									// Step the V fraction.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.
		mov		eax,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		and		edx,eax									// Mask for tiling.

		inc		edi										// Step index.
		jnz		short INNER_LOOP

END_OF_SCANLINE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TBumpLinearTrans.indCoord.bfU.u4Frac
		mov		edx,[eax]TBumpLinearTrans.indCoord.bfU.i4Int

		mov		edi,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TBumpLinearTrans.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebx,[eax]TBumpLinearTrans.fxXDifference
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

		add		edi,[eax]TBumpLinearTrans.indCoord.w1dV.iOffsetPerLine
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

		add		edi,[eax]TBumpLinearTrans.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.i4Int,edi

		mov		[eax]TBumpLinearTrans.fxXDifference,ebx
		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TBumpLinearTrans.indCoord.bfU.u4Frac,ecx
		mov		[eax]TBumpLinearTrans.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TBumpLinearTrans.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TBumpLinearTrans.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp					// Restore the base pointer.
	}
}


//*****************************************************************************************
//
// Bumpmapped and Linear for 16 bit screen.
//
void DrawSubtriangle(TBumpLinear* pscan, CDrawPolygon<TBumpLinear>* pdtri)
{
	typedef TBumpLinear::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static void* pvLastBumpToIntensity = 0;
	static CDrawPolygon<TBumpLinear>* pdtriGlbl;
	static TBumpLinear* pscanGlbl;

	TBumpLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self-modifications here.
		//
		mov		eax,[pvTextureBitmap]					// Texuremap pointer.
		mov		ebx,[pvClutConversion]					// Clut conversion pointer.

		mov		edx,[au1SubBumpToIntensity]				// Pointer to the angle to intensity lookup table.
		mov		ecx,[pvLastTexture]

		mov		esi,[pvLastClut]
		mov		edi,[pvLastBumpToIntensity]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,esi
		jne		short DO_MODIFY

		cmp		edx,edi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],eax
		mov		[pvLastBumpToIntensity],edx
		mov		[pvLastClut],ebx

		lea		edi,MODIFY_FOR_TEXTUREPOINTER
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION
		mov		[edi-4],ebx

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
		// pscanCopy		= pscan;
		// pdtriCopy		= pdtri;
		//
		mov		ebx,[edi]TBumpLinear.fxX
		mov		ecx,[edi]TBumpLinear.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TBumpLinear.indCoord.bfU.i4Int
		mov		ecx,[edi]TBumpLinear.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TBumpLinear.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TBumpLinear.indCoord.w1dV.bfxValue.u4Frac

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
		mov		ebx,[eax]TBumpLinear.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,ecx

		sar		ebx,16
		mov		edi,[eax]TBumpLinear.fxX.i4Fx

		sar		edi,16
		add		edx,ebx

		sub		edi,ebx									// i_pixel
		jge		END_OF_SCANLINE

		//
		// edi = i_pixel
		// edx = i_screen_index
		// ecx = pointer to scanline
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.

		mov		ebx,[eax]TBumpLinear.indCoord.bfU.i4Int				// Uint
		mov		esi,[eax]TBumpLinear.indCoord.bfU.u4Frac			// Ufrac

		lea		ebp,[ebp+edx*2]										// Base of scanline.
		mov		edx,[eax]TBumpLinear.indCoord.w1dV.bfxValue.i4Int	// Vint

		add		edx,ebx												// Add Uint to Vint
		mov		ecx,[eax]TBumpLinear.indCoord.w1dV.bfxValue.u4Frac	// Vfrac

		mov		ebx,[u4TextureTileMask]				// Load tiling mask.
		and		edx,ebx								// Mask for tiling.

		mov		[pvBaseOfLine],ebp					// Save pointer to base of line.
		jmp		short INNER_LOOP
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		eax,ebx									// Copy texture value.

		sub		ebx,[u4LightTheta]						// Subtract light theta.
		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		shr		eax,24									// Shift color value down.
		movzx	ebx,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY:

		shl		ebx,9									// Shift intensity up.
		mov		ax,word ptr[eax*2 + ebx + 0xDEADBEEF]	// CLUT lookup.
MODIFY_FOR_CLUT_CONVERSION:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],ax				// Write to frame buffer.
		inc		edi
		jnz		short INNER_LOOP

END_OF_SCANLINE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TBumpLinear.indCoord.bfU.u4Frac
		mov		edx,[eax]TBumpLinear.indCoord.bfU.i4Int

		mov		edi,[eax]TBumpLinear.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TBumpLinear.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TBumpLinear.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebx,[eax]TBumpLinear.fxXDifference
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

		add		edi,[eax]TBumpLinear.indCoord.w1dV.iOffsetPerLine
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

		add		edi,[eax]TBumpLinear.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TBumpLinear.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TBumpLinear.indCoord.w1dV.bfxValue.i4Int,edi

		mov		[eax]TBumpLinear.fxXDifference,ebx
		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TBumpLinear.indCoord.bfU.u4Frac,ecx
		mov		[eax]TBumpLinear.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TBumpLinear.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TBumpLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp				// Restore the base pointer.
	}
}


#endif  // elif (iBUMPMAP_RESOLUTION == 32)
