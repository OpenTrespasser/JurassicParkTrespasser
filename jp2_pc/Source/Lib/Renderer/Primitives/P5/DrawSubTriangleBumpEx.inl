#pragma once

/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function.
 *		Optimized for the Pentium Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P5/DrawSubTriangleBumpEx.cpp                  $
 * 
 * 14    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 13    98.01.22 3:15p Mmouni
 * Added support for 16-bit deep bump-maps.
 * 
 * 12    97.10.27 1:22p Mmouni
 * Changed unreference locals (preventing internal compiler errors) to typedefs.
 * 
 * 11    97.10.22 7:11p Mmouni
 * Moved code that makes copies of edge values into assembly.
 * 
 * 10    10/10/97 1:47p Mmouni
 * All inner loops are now left to right only, and optimized some.
 * 
 * 9     9/30/97 9:31p Mmouni
 * All primitives now have optimized y loops.
 * 
 * 8     9/29/97 11:55a Mmouni
 * Made self modification conditional on direction.
 * 
 * 7     9/15/97 2:10p Mmouni
 * Optimized inner loops (not final yet).
 * 
 * 6     9/01/97 8:02p Rwyatt
 * ANDs added to enable tiling
 * 
 * 5     97/07/18 3:28p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 4     97/07/16 15:49 Speter
 * Commented out redundant static delcarations now that all DrawSubTriangles are included into
 * one file (now just declared in DrawSubTriangleEx.cpp.)
 * 
 * 3     7/14/97 12:36p Rwyatt
 * Checkled in so Scott can move them....
 * 
 * 2     7/11/97 2:52p Rwyatt
 * Highly optimized Linear Non-transparent for P5
 * 
 * 1     7/07/97 7:23p Rwyatt
 * Initial Implementation of Pentium Specific bump mapping
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
		mov		[pvLastClut],ebx
		mov		[pvLastBumpToIntensity],edx

		lea		ecx,[eax+1]								// Texture pointer plus one.
		lea		esi,[ebx+1]								// Clut pointer plus one.

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_A
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_A
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_A
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_A
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_A
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_B
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_B
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_B
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_B
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_B
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_C
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_C
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_C
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_C
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_C
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_D
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_D
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_D
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_D
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_D
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_E
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_E
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_E
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_E
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_E
		mov		[edi-4],esi

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
		jnz		short END_OF_SCANLINE

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
		lea		eax,[ebp+edi*2]

		// Check alignement.
		test	eax,3
		jz		short ALIGNED

		//
		// Do one pixel for alignment. 
		//
		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_A:
		mov		ebp,[u4LightTheta]						// Load light theta.

		test	al,0xfc									// Test for transparent.
		jz		short SKIP_ALIGN_PIXEL

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_A:
		mov		bh,al									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_A:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		add		esi,ebx									// Step the U fraction.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_A:
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_A:
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],cx				// Write to frame buffer.

		inc		edi
		jz		short DONE_WITH_SPAN

		jmp		short ALIGNED

SKIP_ALIGN_PIXEL:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.

		// AGI

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.

		and		edx,ebx									// Mask for tiling.

		inc		edi
		jz		short DONE_WITH_SPAN

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

INNER_LOOP_2P:
		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_B:
		mov		ebp,[u4LightTheta]						// Load light theta.

		test	al,0xfc									// Test for transparent.
		jz		short SKIP_FIRST_PIXEL

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_B:
		mov		bh,al									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_B:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_B:

		and		edx,ebx									// Mask for tiling.
		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_B:

		// AGI on ecx

		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_C:
		mov		ebp,[u4LightTheta]						// Load light theta.

		test	al,0xfc									// Test for transparent.
		jz		short SKIP_SECOND_PIXEL

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_C:
		mov		bh,al									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		nop

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_C:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,ecx									// Copy pixel values.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[u4TextureTileMask]					// Load tiling mask.

		shl		ebx,16									// Shift first pixel up.
		and		edx,ebp									// Mask for tiling.

		mov		bh,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_C:
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		bl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_C:
		add		edi,2									// Step count.

		bswap	ebx										// Swap pixels into place.

		mov		[ebp + edi*2 - 8],ebx					// Write to frame buffer.
		jle		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		//
		// Skippped over the first pixel.
		//	
SKIP_FIRST_PIXEL:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.

		// AGI on ebp

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.

		and		edx,ebx									// Mask for tiling.

		// AGI on edx

		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_D:
		mov		ebp,[u4LightTheta]						// Load light theta.

		test	al,0xfc									// Test for transparent.
		jz		short SKIP_BOTH_PIXELS

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_D:
		mov		bh,al									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_D:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,ecx									// Copy pixel values.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[u4TextureTileMask]					// Load tiling mask.

		shl		ebx,16									// Shift first pixel up.
		and		edx,ebp									// Mask for tiling.

		mov		bl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_D:
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		bh,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_D:

		mov		[ebp + edi*2 - 2],bx					// Store second pixel.

		add		edi,2									// Step count.
		jle		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		//
		// Skippped over the second pixel, fist pixel is non-transparent.
		//	
SKIP_SECOND_PIXEL:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		ecx,ebp									// Step the V fraction.
		mov		al,ch									// Copy first pixel.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.

		mov		ah,cl									// Copy first pixel.
		nop

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		and		edx,ebx									// Mask for tiling.

		mov		[ebp + edi*2 - 4],ax					// Store first pixel.

		add		edi,2									// Step count.
		jle		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		//
		// Skippped over the both pixels.
		//	
SKIP_BOTH_PIXELS:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.

		// AGI

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.

		and		edx,ebx									// Mask for tiling.

		add		edi,2									// Step count.
		jle		short INNER_LOOP_2P

		//
		// Finish left over pixel (if any)
		//
FINISH_REMAINDER:
		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short DONE_WITH_SPAN

		//
		// One pixel, update texture values.
		//
		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_E:
		mov		ebp,[u4LightTheta]						// Load light theta.

		test	al,0xfc									// Test for transparent.
		jz		short SKIP_LEFTOVER_PIXEL

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_E:
		mov		bh,al									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_E:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		add		esi,ebx									// Step the U fraction.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_E:
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_E:
		and		edx,ebx									// Mask for tiling.
	
		mov		[ebp - 2],cx							// Write to frame buffer.
		jmp		short DONE_WITH_SPAN

SKIP_LEFTOVER_PIXEL:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.

		// AGI

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.

		and		edx,ebx									// Mask for tiling.

DONE_WITH_SPAN:
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
		mov		[pvLastClut],ebx
		mov		[pvLastBumpToIntensity],edx

		lea		ecx,[eax+1]								// Texture pointer plus one.
		lea		esi,[ebx+1]								// Clut pointer plus one.

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_A
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_A
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_A
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_A
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_A
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_B
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_B
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_B
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_B
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_B
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_C
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_C
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_C
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_C
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_C
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_D
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_D
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_D
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_D
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_D
		mov		[edi-4],esi

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
		lea		eax,[ebp+edi*2]

		// Check alignement.
		test	eax,3
		jz		short ALIGNED

		//
		// Do one pixel for alignment. 
		//
		mov		bh,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_A:
		mov		ebp,[u4LightTheta]						// Load light theta.

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_A:
		mov		al,bh									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_A:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		add		esi,ebx									// Step the U fraction.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_A:
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_A:
		and		edx,ebx									// Mask for tiling.
	
		mov		[ebp + edi*2],cx						// Write to frame buffer.

		inc		edi
		jz		short DONE_WITH_SPAN

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

		//
		// Do two pixels at a time.
		//
INNER_LOOP_2P:
		mov		bh,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_B:
		mov		ebp,[u4LightTheta]						// Load light theta.

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_B:
		mov		al,bh									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_B:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_B:

		and		edx,ebx									// Mask for tiling.
		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_B:
	
		// AGI on edx

		mov		bh,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_C:
		mov		ebp,[u4LightTheta]						// Load light theta.

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_C:
		mov		al,bh									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		nop

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_C:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,ecx									// Copy pixel values.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[u4TextureTileMask]					// Load tiling mask.

		shl		ebx,16									// Shift first pixel up.
		and		edx,ebp									// Mask for tiling.

		mov		bh,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_C:
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		bl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_C:
		add		edi,2									// Step count.

		bswap	ebx										// Swap pixels into place.

		mov		[ebp + edi*2 - 8],ebx					// Write to frame buffer.
		jle		short INNER_LOOP_2P

		//
		// Finish left over pixel (if any)
		//
FINISH_REMAINDER:
		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short DONE_WITH_SPAN

		//
		// One pixel, update texture values.
		//
		mov		bh,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_D:
		mov		ebp,[u4LightTheta]						// Load light theta.

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_D:
		mov		al,bh									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_D:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		add		esi,ebx									// Step the U fraction.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_D:
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_D:
		and		edx,ebx									// Mask for tiling.
	
		mov		[ebp - 2],cx							// Write to frame buffer.

DONE_WITH_SPAN:
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
		mov		[pvLastClut],ebx
		mov		[pvLastBumpToIntensity],edx

		lea		ecx,[eax+1]								// Texture pointer plus one.
		lea		esi,[ebx+1]								// Clut pointer plus one.

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_A
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_A
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_A
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_A
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_A
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_B
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_B
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_B
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_B
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_B
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_C
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_C
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_C
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_C
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_C
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_D
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_D
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_D
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_D
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_D
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_E
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_E
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_E
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_E
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_E
		mov		[edi-4],esi

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
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.

		mov		ebx,[eax]TBumpLinearTrans.indCoord.bfU.i4Int				// Uint
		mov		esi,[eax]TBumpLinearTrans.indCoord.bfU.u4Frac			// Ufrac

		lea		ebp,[ebp+edx*2]										// Base of scanline.
		mov		edx,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.i4Int	// Vint

		add		edx,ebx												// Add Uint to Vint
		mov		ecx,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.u4Frac	// Vfrac

		mov		ebx,[u4TextureTileMask]				// Load tiling mask.
		mov		[pvBaseOfLine],ebp					// Save pointer to base of line.

		and		edx,ebx								// Mask for tiling.
		lea		eax,[ebp+edi*2]

		// Check alignement.
		test	eax,3
		jz		short ALIGNED

		//
		// Do one pixel for alignment. 
		//
		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_A:
		mov		ebp,[u4LightTheta]						// Load light theta.

		test	al,0xfc									// Test for transparent.
		jz		short SKIP_ALIGN_PIXEL

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_A:
		mov		bh,al									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_A:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		add		esi,ebx									// Step the U fraction.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_A:
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_A:
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],cx				// Write to frame buffer.

		inc		edi
		jz		short END_OF_SCANLINE

		jmp		short ALIGNED

SKIP_ALIGN_PIXEL:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.

		// AGI

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.

		and		edx,ebx									// Mask for tiling.

		inc		edi
		jz		short END_OF_SCANLINE

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

INNER_LOOP_2P:
		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_B:
		mov		ebp,[u4LightTheta]						// Load light theta.

		test	al,0xfc									// Test for transparent.
		jz		short SKIP_FIRST_PIXEL

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_B:
		mov		bh,al									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_B:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_B:

		and		edx,ebx									// Mask for tiling.
		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_B:

		// AGI on ecx

		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_C:
		mov		ebp,[u4LightTheta]						// Load light theta.

		test	al,0xfc									// Test for transparent.
		jz		short SKIP_SECOND_PIXEL

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_C:
		mov		bh,al									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		nop

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_C:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,ecx									// Copy pixel values.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[u4TextureTileMask]					// Load tiling mask.

		shl		ebx,16									// Shift first pixel up.
		and		edx,ebp									// Mask for tiling.

		mov		bh,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_C:
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		bl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_C:
		add		edi,2									// Step count.

		bswap	ebx										// Swap pixels into place.

		mov		[ebp + edi*2 - 8],ebx					// Write to frame buffer.
		jle		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		//
		// Skippped over the first pixel.
		//	
SKIP_FIRST_PIXEL:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.

		// AGI on ebp

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.

		and		edx,ebx									// Mask for tiling.

		// AGI on edx

		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_D:
		mov		ebp,[u4LightTheta]						// Load light theta.

		test	al,0xfc									// Test for transparent.
		jz		short SKIP_BOTH_PIXELS

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_D:
		mov		bh,al									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_D:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,ecx									// Copy pixel values.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[u4TextureTileMask]					// Load tiling mask.

		shl		ebx,16									// Shift first pixel up.
		and		edx,ebp									// Mask for tiling.

		mov		bl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_D:
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		bh,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_D:

		mov		[ebp + edi*2 - 2],bx					// Store second pixel.

		add		edi,2									// Step count.
		jle		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		//
		// Skippped over the second pixel, fist pixel is non-transparent.
		//	
SKIP_SECOND_PIXEL:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		ecx,ebp									// Step the V fraction.
		mov		al,ch									// Copy first pixel.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.

		mov		ah,cl									// Copy first pixel.
		nop

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		and		edx,ebx									// Mask for tiling.

		mov		[ebp + edi*2 - 4],ax					// Store first pixel.

		add		edi,2									// Step count.
		jle		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		//
		// Skippped over the both pixels.
		//	
SKIP_BOTH_PIXELS:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		add		esi,ebx									// Step the U fraction.

		// AGI

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.

		and		edx,ebx									// Mask for tiling.

		add		edi,2									// Step count.
		jle		short INNER_LOOP_2P

		//
		// Finish left over pixel (if any)
		//
FINISH_REMAINDER:
		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short END_OF_SCANLINE

		//
		// One pixel, update texture values.
		//
		mov		al,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_E:
		mov		ebp,[u4LightTheta]						// Load light theta.

		test	al,0xfc									// Test for transparent.
		jz		short END_OF_SCANLINE

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_E:
		mov		bh,al									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_E:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		add		esi,ebx									// Step the U fraction.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_E:
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_E:
		and		edx,ebx									// Mask for tiling.
	
		mov		[ebp - 2],cx							// Write to frame buffer.

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

		pop		ebp				// Restore the base pointer.
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
		mov		[pvLastClut],ebx
		mov		[pvLastBumpToIntensity],edx

		lea		ecx,[eax+1]								// Texture pointer plus one.
		lea		esi,[ebx+1]								// Clut pointer plus one.

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_A
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_A
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_A
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_A
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_A
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_B
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_B
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_B
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_B
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_B
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_C
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_C
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_C
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_C
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_C
		mov		[edi-4],esi

		lea		edi,MODIFY_FOR_TEXTUREPOINTER_D
		mov		[edi-4],eax
		lea		edi,MODIFY_FOR_TEXTUREPOINTER_PL1_D
		mov		[edi-4],ecx
		lea		edi,MODIFY_FOR_ANGLETOINTENSITY_D
		mov		[edi-4],edx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_D
		mov		[edi-4],ebx
		lea		edi,MODIFY_FOR_CLUT_CONVERSION_PL1_D
		mov		[edi-4],esi

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
		mov		[pvBaseOfLine],ebp					// Save pointer to base of line.

		and		edx,ebx								// Mask for tiling.
		lea		eax,[ebp+edi*2]

		// Check alignement.
		test	eax,3
		jz		short ALIGNED

		// Step to see if this is the only pixel.
		inc		edi
		jz		short FINISH_REMAINDER

		//
		// Do one pixel for alignment. 
		//
		mov		bh,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_A:
		mov		ebp,[u4LightTheta]						// Load light theta.

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_A:
		mov		al,bh									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_A:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		add		esi,ebx									// Step the U fraction.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_A:
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_A:
		and		edx,ebx									// Mask for tiling.
	
		mov		[ebp + edi*2 - 2],cx					// Write to frame buffer.

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

		//
		// Do two pixels at a time.
		//
INNER_LOOP_2P:
		mov		bh,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_B:
		mov		ebp,[u4LightTheta]						// Load light theta.

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_B:
		mov		al,bh									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		and		eax,0x0000ffff							// Clear upper part of eax.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_B:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_B:

		and		edx,ebx									// Mask for tiling.
		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_B:
	
		// AGI on edx

		mov		bh,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_C:
		mov		ebp,[u4LightTheta]						// Load light theta.

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_C:
		mov		al,bh									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		nop

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_C:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		shr		eax,2									// Shift off extra bits.
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Lookup integer step based on v carry.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,ecx									// Copy pixel values.

		adc		edx,ebp									// Add integer step plus u carry.
		mov		ebp,[u4TextureTileMask]					// Load tiling mask.

		shl		ebx,16									// Shift first pixel up.
		and		edx,ebp									// Mask for tiling.

		mov		bh,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_C:
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		bl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_C:
		add		edi,2									// Step count.

		bswap	ebx										// Swap pixels into place.

		mov		[ebp + edi*2 - 8],ebx					// Write to frame buffer.
		jle		short INNER_LOOP_2P

		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short END_OF_SCANLINE

FINISH_REMAINDER:
		//
		// One pixel, don't update texture values.
		//
		mov		bh,[edx*2 + 0xDEADBEEF]					// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_PL1_D:
		mov		ebp,[u4LightTheta]						// Load light theta.

		mov		bl,[edx*2 + 0xDEADBEEF]					// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_D:
		mov		al,bh									// Copy texture value (high).

		sub		ebx,ebp									// Subtract light theta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		and		eax,0x0000ffff							// Clear upper part of eax.

		// AGI on ebx

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map intensity lookup.
MODIFY_FOR_ANGLETOINTENSITY_D:

		shr		eax,2									// Shift off extra bits.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		// AGI on eax

		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_D:

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_D:
	
		mov		[ebp - 2],cx							// Write to frame buffer.

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

	TBumpPerspTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self-modifications here.
		//
		mov		eax,[au1SubBumpToIntensity]			// Load the pointer to the angle to intensity lookup table.
		mov		ebx,[pvClutConversion]				// Clut conversion pointer.

		mov		ecx,[pvTextureBitmap]
		mov		[MODIFY_FOR_ANGLETOINTENSITY-4],eax

		mov		[MODIFY_FOR_CLUT_CONVERSION-4],ebx
		inc		ebx

		mov		[MODIFY_FOR_CLUT_CONVERSION_P1-4],ebx

		mov		[MODIFY_FOR_TEXTUREPOINTER_A-4],ecx
		add		ecx,3

		mov		[MODIFY_FOR_TEXTUREPOINTER_B-4],ecx

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

INNER_LOOP:
		mov		eax,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_A:
		mov		ebx,[u4LightTheta]						// Get light theta.

		test	eax,0xff000000
		jz		short SKIP_TRANSPARENCY

		sub		eax,ebx									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		eax,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		mov		bl,byte ptr[edx*4 + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_B:

		mov		bh,byte ptr[eax + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY:
		mov		eax,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		and		ebx,0x0000FFFF							// Mask off upper part of ebx
		add		esi,eax									// Step the U fraction.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[ebx*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION:
		mov		eax,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[ebx*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_P1:
		and		edx,eax									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],cx				// Write to frame buffer.

		inc		edi										// Increment index.
		jnz		short INNER_LOOP

		jmp		short EXIT_INNER_LOOP

SKIP_TRANSPARENCY:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load V fraction delta.

		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from V fractional add.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		and		edx,ebx									// Mask for tiling.

		inc		edi										// Increment index.
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
		mov		[pvLastClut],ebx
		mov		[pvLastBumpToIntensity],edx

		lea		ecx,[eax+3]								// Texture pointer plus 3.
		lea		esi,[ebx+1]								// Clut pointer plus one.

		mov		[MODIFY_FOR_TEXTUREPOINTER_E-4],eax
		mov		[MODIFY_FOR_ANGLETOINTENSITY_E-4],edx
		mov		[MODIFY_FOR_TEXTUREPOINTER_PL3_E-4],ecx
		mov		[MODIFY_FOR_CLUT_CONVERSION_E-4],ebx
		mov		[MODIFY_FOR_CLUT_CONVERSION_PL1_E-4],esi

		mov		[MODIFY_FOR_TEXTUREPOINTER_F-4],eax
		mov		[MODIFY_FOR_ANGLETOINTENSITY_F-4],edx
		mov		[MODIFY_FOR_TEXTUREPOINTER_PL3_F-4],ecx
		mov		[MODIFY_FOR_CLUT_CONVERSION_F-4],ebx
		mov		[MODIFY_FOR_CLUT_CONVERSION_PL1_F-4],esi

		mov		[MODIFY_FOR_TEXTUREPOINTER_G-4],eax
		mov		[MODIFY_FOR_ANGLETOINTENSITY_G-4],edx
		mov		[MODIFY_FOR_TEXTUREPOINTER_PL3_G-4],ecx
		mov		[MODIFY_FOR_CLUT_CONVERSION_G-4],ebx
		mov		[MODIFY_FOR_CLUT_CONVERSION_PL1_G-4],esi

		mov		[MODIFY_FOR_TEXTUREPOINTER_H-4],eax
		mov		[MODIFY_FOR_ANGLETOINTENSITY_H-4],edx
		mov		[MODIFY_FOR_TEXTUREPOINTER_PL3_H-4],ecx
		mov		[MODIFY_FOR_CLUT_CONVERSION_H-4],ebx
		mov		[MODIFY_FOR_CLUT_CONVERSION_PL1_H-4],esi

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
		lea		eax,[ebp+edi*2]

		// Check alignement.
		test	eax,3
		jz		short ALIGNED

		//
		// Do one pixel for alignment. 
		//
		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_E:
		mov		eax,[u4LightTheta]						// Get light theta.

		sub		ebx,eax									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		mov		al,byte ptr[edx*4 + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_PL3_E:

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY_E:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		and		eax,0x0000FFFF							// Mask off upper part to eax.
		add		esi,ebx									// Step the U fraction.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_E:
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_E:
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],cx				// Write to frame buffer.

		inc		edi
		jz		short DONE_WITH_SPAN

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

		//
		// Do two pixels at a time.
		//
INNER_LOOP_2P:
		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_F:
		mov		eax,[u4LightTheta]						// Get light theta.

		sub		ebx,eax									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		mov		al,byte ptr[edx*4 + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_PL3_F:

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY_F:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Load the integer step based on v carry.
		and		eax,0x0000FFFF							// Mask off upper part to eax.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add the integer values from the integer table, and add a possible carry.
		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_F:

		and		edx,ebx									// Mask for tiling.
		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_F:

		mov		eax,[u4LightTheta]						// Get light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_G:
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		sub		ebx,eax									// Subtract light theta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		xor		eax,eax									// Clear eax.

		mov		al,byte ptr[edx*4 + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_PL3_G:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Load the integer step based on v carry.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY_G:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,ecx									// Get copy of low texture word.

		adc		edx,ebp									// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[u4TextureTileMask]					// Load tiling mask.

		shl		ebx,16									// Shift low pixel up.
		and		edx,ebp									// Mask for tiling.

		mov		bh,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_G:
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		bl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_G:
		add		edi,2									// Step count.

		bswap	ebx										// Swap pixels into place.

		mov		dword ptr[ebp + edi*2 - 8],ebx			// Write to frame buffer.
		jle		short INNER_LOOP_2P

		//
		// Finish left over pixel (if any)
		//
FINISH_REMAINDER:
		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short DONE_WITH_SPAN

		//
		// One pixel, update texture values.
		//
		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_H:
		mov		eax,[u4LightTheta]						// Get light theta.

		sub		ebx,eax									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		mov		al,byte ptr[edx*4 + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_PL3_H:

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY_H:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		and		eax,0x0000FFFF							// Mask off upper part to eax.
		add		esi,ebx									// Step the U fraction.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_H:
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_H:
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp - 2],cx					// Write to frame buffer.

DONE_WITH_SPAN:
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
	static CDrawPolygon<TBumpLinearTrans>* pdtriGlbl;
	static TBumpLinearTrans* pscanGlbl;

	TBumpLinearTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self-modifications here.
		//
		mov		eax,[au1SubBumpToIntensity]			// Load the pointer to the angle to intensity lookup table.
		mov		ebx,[pvClutConversion]				// Clut conversion pointer.

		mov		ecx,[pvTextureBitmap]
		mov		[MODIFY_FOR_ANGLETOINTENSITY-4],eax

		mov		[MODIFY_FOR_CLUT_CONVERSION-4],ebx
		inc		ebx

		mov		[MODIFY_FOR_CLUT_CONVERSION_P1-4],ebx

		mov		[MODIFY_FOR_TEXTUREPOINTER_A-4],ecx
		add		ecx,3

		mov		[MODIFY_FOR_TEXTUREPOINTER_B-4],ecx

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

		mov		esi,[eax]TBumpLinearTrans.indCoord.bfU.u4Frac				// Ufrac
		mov		ecx,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.u4Frac		// Vfrac

		lea		ebp,[ebp+edx*2]											// Base of scanline.
		mov		edx,[eax]TBumpLinearTrans.indCoord.w1dV.bfxValue.i4Int	// Vint

		add		edx,ebx													// Add Uint to Vint
		mov		ebx,[u4TextureTileMask]									// Load tiling mask.

		and		edx,ebx									// Mask for tiling.
		mov		[pvBaseOfLine],ebp						// Save pointer to base of line.

INNER_LOOP:
		mov		eax,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_A:
		mov		ebx,[u4LightTheta]						// Get light theta.

		test	eax,0xff000000
		jz		short SKIP_TRANSPARENCY

		sub		eax,ebx									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		eax,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		mov		bl,byte ptr[edx*4 + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_B:

		mov		bh,byte ptr[eax + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY:
		mov		eax,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		and		ebx,0x0000FFFF							// Mask off upper part of ebx
		add		esi,eax									// Step the U fraction.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[ebx*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION:
		mov		eax,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[ebx*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_P1:
		and		edx,eax									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],cx				// Write to frame buffer.

		inc		edi										// Increment index.
		jnz		short INNER_LOOP

		jmp		short END_OF_SCANLINE

SKIP_TRANSPARENCY:
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load V fraction delta.

		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from V fractional add.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		and		edx,ebx									// Mask for tiling.

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
		mov		[pvLastClut],ebx
		mov		[pvLastBumpToIntensity],edx

		lea		ecx,[eax+3]								// Texture pointer plus 3.
		lea		esi,[ebx+1]								// Clut pointer plus one.

		mov		[MODIFY_FOR_TEXTUREPOINTER_E-4],eax
		mov		[MODIFY_FOR_ANGLETOINTENSITY_E-4],edx
		mov		[MODIFY_FOR_TEXTUREPOINTER_PL3_E-4],ecx
		mov		[MODIFY_FOR_CLUT_CONVERSION_E-4],ebx
		mov		[MODIFY_FOR_CLUT_CONVERSION_PL1_E-4],esi

		mov		[MODIFY_FOR_TEXTUREPOINTER_F-4],eax
		mov		[MODIFY_FOR_ANGLETOINTENSITY_F-4],edx
		mov		[MODIFY_FOR_TEXTUREPOINTER_PL3_F-4],ecx
		mov		[MODIFY_FOR_CLUT_CONVERSION_F-4],ebx
		mov		[MODIFY_FOR_CLUT_CONVERSION_PL1_F-4],esi

		mov		[MODIFY_FOR_TEXTUREPOINTER_G-4],eax
		mov		[MODIFY_FOR_ANGLETOINTENSITY_G-4],edx
		mov		[MODIFY_FOR_TEXTUREPOINTER_PL3_G-4],ecx
		mov		[MODIFY_FOR_CLUT_CONVERSION_G-4],ebx
		mov		[MODIFY_FOR_CLUT_CONVERSION_PL1_G-4],esi

		mov		[MODIFY_FOR_TEXTUREPOINTER_H-4],eax
		mov		[MODIFY_FOR_ANGLETOINTENSITY_H-4],edx
		mov		[MODIFY_FOR_TEXTUREPOINTER_PL3_H-4],ecx
		mov		[MODIFY_FOR_CLUT_CONVERSION_H-4],ebx
		mov		[MODIFY_FOR_CLUT_CONVERSION_PL1_H-4],esi

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
		mov		[pvBaseOfLine],ebp					// Save pointer to base of line.

		and		edx,ebx								// Mask for tiling.
		lea		eax,[ebp+edi*2]

		// Check alignement.
		test	eax,3
		jz		short ALIGNED

		//
		// Do one pixel for alignment. 
		//
		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_E:
		mov		eax,[u4LightTheta]						// Get light theta.

		sub		ebx,eax									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		mov		al,byte ptr[edx*4 + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_PL3_E:

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY_E:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		and		eax,0x0000FFFF							// Mask off upper part to eax.
		add		esi,ebx									// Step the U fraction.

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_E:
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_E:
		and		edx,ebx									// Mask for tiling.
	
		mov		word ptr[ebp + edi*2],cx				// Write to frame buffer.

		inc		edi
		jz		short END_OF_SCANLINE

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

		//
		// Do two pixels at a time.
		//
INNER_LOOP_2P:
		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_F:
		mov		eax,[u4LightTheta]						// Get light theta.

		sub		ebx,eax									// Subtract light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		mov		al,byte ptr[edx*4 + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_PL3_F:

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY_F:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Load the integer step based on v carry.
		and		eax,0x0000FFFF							// Mask off upper part to eax.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,[u4TextureTileMask]					// Load tiling mask.

		adc		edx,ebp									// Add the integer values from the integer table, and add a possible carry.
		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_F:

		and		edx,ebx									// Mask for tiling.
		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_F:

		mov		eax,[u4LightTheta]						// Get light theta.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac			// Load v fraction delta.

		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_G:
		add		ecx,ebp									// Step the V fraction.

		sbb		ebp,ebp									// Get carry from v fractional add.
		sub		ebx,eax									// Subtract light theta.

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		xor		eax,eax									// Clear eax.

		mov		al,byte ptr[edx*4 + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_PL3_G:
		mov		ebp,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Load the integer step based on v carry.

		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY_G:
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac			// Load u fraction delta.

		add		esi,ebx									// Step the U fraction.
		mov		ebx,ecx									// Get copy of low texture word.

		adc		edx,ebp									// Add the integer values from the integer table, and add a possible carry.
		mov		ebp,[u4TextureTileMask]					// Load tiling mask.

		shl		ebx,16									// Shift low pixel up.
		and		edx,ebp									// Mask for tiling.

		mov		bh,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_G:
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		mov		bl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_G:
		add		edi,2									// Step count.

		bswap	ebx										// Swap pixels into place.

		mov		dword ptr[ebp + edi*2 - 8],ebx			// Write to frame buffer.
		jle		short INNER_LOOP_2P

		//
		// Finish left over pixel (if any)
		//
FINISH_REMAINDER:
		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short END_OF_SCANLINE

		//
		// One pixel without updating texture values.
		//
		mov		ebx,[edx*4 + 0xDEADBEEF]				// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_H:
		mov		eax,[u4LightTheta]						// Get light theta.

		sub		ebx,eax									// Subtract light theta.
		xor		eax,eax									// Clear eax

		and		ebx,iMASK_ANGLETABLE_LOOKUP				// Mask for bump map table index.
		mov		al,byte ptr[edx*4 + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTUREPOINTER_PL3_H:

		// AGI on ebx
		mov		ah,byte ptr[ebx + 0xDEADBEEF]			// Bump map lookup.
MODIFY_FOR_ANGLETOINTENSITY_H:
		mov		ebp,[pvBaseOfLine]						// Load desination base pointer.

		// AGI on eax
		mov		cl,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (low).
MODIFY_FOR_CLUT_CONVERSION_H:

		mov		ch,byte ptr[eax*2 + 0xDEADBEEF]			// Intesity lookup (high).
MODIFY_FOR_CLUT_CONVERSION_PL1_H:
	
		mov		word ptr[ebp -2],cx						// Write to frame buffer.

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

#endif // elif (iBUMPMAP_RESOLUTION == 32)
