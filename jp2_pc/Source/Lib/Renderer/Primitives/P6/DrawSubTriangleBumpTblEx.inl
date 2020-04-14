/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function.
 *		Optimized for the Pentium PRO Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P6/DrawSubTriangleBumpTblEx.cpp               $
 * 
 * 5     97/10/12 21:40 Speter
 * Changed BumpLookup.au1CosToIntensity to au1IntensityTrans.
 * 
 * 4     9/01/97 8:05p Rwyatt
 * ANDs added to enable tiling
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
 * 2     6/19/97 2:53p Mlange
 * Moved Profile.hpp to Lib/Sys.
 * 
 * 1     97/06/14 6:46p Pkeet
 * Initial version.
 * 
 * 17    97/06/12 5:56p Pkeet
 * Added capability to render only every other scanline and to double the number of scanlines.
 * 
 * 16    97/06/11 4:58p Pkeet
 * Added opaque versions of the optimized copy primitives.
 * 
 * 15    6/11/97 4:11p Rwyatt
 * Removed 3 AGIs and 2 PRO stalls from the perspective primitive. These stalls were costing
 * about 40 clocks and the net result is 50% speed increase!!!!!!!!
 * 
 * 14    6/11/97 2:45p Rwyatt
 * Removed 2 AGI stalls from the linear copy
 * 
 * 13    6/11/97 2:04a Rwyatt
 * Slight;y optimized, need to run through V-Tune so we can spot any stalls
 * 
 * 12    6/10/97 10:02p Rwyatt
 * Blazing linear transparent copy
 * 
 * 11    6/10/97 4:34p Rwyatt
 * Slightly optimized perspective copy primitive
 * 
 * 10    97/06/10 2:27p Pkeet
 * Moved the bumpmap routines into a seperate module.
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

#include "Lib/Sys/Profile.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"


//
// Temporary Globals.
//
// Notes:
//		These variables should be arraigned along with the external globals for maximum
//		cache efficiency.
//

// try to make variabels that would be local static so I do not need to use EBP to reference them.
// Variables that must be static can be force referenced by ESP if required.

// Declared in DrawSubTriangleEx.cpp.  Should be moved and renamed.
/*
static int    i_x_from;
static int    i_x_to;
static int    i_screen_index;
static int    i_pixel;
*/
extern uint32 u4ClutIndex;


//*****************************************************************************************
//
// Bumpmapped and Perspective correct for 16 bit screen.
//
void DrawSubtriangle(TBumpPerspTbl* pscan, CDrawPolygon<TBumpPerspTbl>* pdtri)
{
	//
	// Do self-modifications here.
	//
	uint32 u4_table_address = (uint32)au1IntensityTrans;
	__asm
	{
		mov eax, [au1SubBumpToIntensity]			// Load the pointer to the angle to intensity lookup table.
		mov ebx, [pvClutConversion]					// Clut conversion pointer.
		mov ecx, [pvTextureBitmap]
		mov edx, dword ptr[u4_table_address]

		mov [MODIFY_FOR_ANGLETOINTENSITY-4], eax
		mov [MODIFY_FOR_CLUT_CONVERSION-4], ebx
		mov [MODIFY_FOR_TEXTUREPOINTER_A-4], ecx
		add ecx, 3
		mov [MODIFY_FOR_TEXTUREPOINTER_B-4], ecx
		mov [MODIFY_FOR_BUMPMAP_TABLE - 4], edx

		// scan line direction
		cmp		i4ScanlineDirection,0
		jle		short MDEC
		// +ve
		mov		byte ptr [MODIFY_DIRECTION], 047h	// inc edi
		jmp		short MDONE
	MDEC:
		// -ve
		mov		byte ptr [MODIFY_DIRECTION], 04Fh	// dec edi
	MDONE:
	}

	//
	// Iterate through the scanlines that intersect the subtriangle.
	//
	do
	{
		if (bRIGHT_TO_LEFT)
		{
			i_x_from = (pscan->fxX.i4Fx - pdtri->fxLineLength.i4Fx)>>16;
			i_x_to   = (pscan->fxX.i4Fx)>>16;
			i_screen_index = pdtri->iLineStartIndex + i_x_from - 1;
			i_pixel  = i_x_to - i_x_from;
		}
		else
		{
			i_x_from = (pscan->fxX.i4Fx)>>16;
			i_x_to   = (pscan->fxX.i4Fx + pdtri->fxLineLength.i4Fx)>>16;
			i_screen_index = pdtri->iLineStartIndex + i_x_to;
			i_pixel  = i_x_from - i_x_to;
		}

		// Draw if there are pixels to draw.
		if (i_x_to > i_x_from && !(bEvenScanlinesOnly && ((pdtri->iY & 1) == 0)))
		{
			// Get the start address for the scanline on the destination raster.
			TBumpPerspTbl::TPixel* apix_screen = (TBumpPerspTbl::TPixel*)pdtri->prasScreen->pSurface;
			apix_screen += i_screen_index;

			TBumpPerspTbl::TIndex index(pscan->indCoord, i_pixel);

			//
			// Iterate left or right along the destination scanline depending on the sign of
			// 'i4ScanlineDirection.'
			//
			for (;;)
			{
				// Do next subdivision.
				int i_pixel_inner = index.i4StartSubdivision(i_pixel);

				TBumpPerspTbl::TPixel* apix_screen_inner = apix_screen + i_pixel;

				__asm
				{
					mov edi, i_pixel_inner
					mov esi, [w2dTex]CWalk2D.uUFrac
					mov ecx, [w2dTex]CWalk2D.uVFrac
					mov edx, [w2dTex + 4]CWalk2D.iUVInt
					push ebp
					mov ebp, apix_screen_inner
					jmp short BEGIN_INNER_LOOP

				BEGIN_INNER_LOOP:
					and edx, [u4TextureTileMask]
					xor ebx, ebx
					mov bl, byte ptr[edx*4 + 0x12345678]
				MODIFY_FOR_TEXTUREPOINTER_B:

					mov byte ptr[u4ClutIndex], bl
					mov ebx, [edx*4 + 0x12345678]
				MODIFY_FOR_TEXTUREPOINTER_A:

					sub ebx, [u4LightTheta]
					xor eax, eax

					and ebx, iMASK_ANGLETABLE_LOOKUP

					movzx ebx, byte ptr[ebx + 0x12345678]
				MODIFY_FOR_ANGLETOINTENSITY:
					
					movzx eax, byte ptr[ebx + 0x12345678]
				MODIFY_FOR_BUMPMAP_TABLE:

					shl eax, 8
					xor ebx, ebx

					add eax, [u4ClutIndex]

					mov bx, word ptr[eax*2 + 0x12345678]
				MODIFY_FOR_CLUT_CONVERSION:

					// Add the V fractional values together and store the carry.
					add ecx, [w2dDeltaTex]CWalk2D.uVFrac
					sbb eax, eax

					// Add the U fractional values, setting the carry flag.
					add esi, [w2dDeltaTex]CWalk2D.uUFrac

					// Add the integer values from the integer table, and add a possible carry.
					adc edx, [w2dDeltaTex + eax*4 + 4]CWalk2D.iUVInt
					mov word ptr[ebp + edi*2], bx

					//
					// Increment the inner loop screen index. This nop will get modified into
					// an inc or dec edi
					//
MODIFY_DIRECTION:	nop
					jnz short BEGIN_INNER_LOOP

					// Store the modified variables.
					pop ebp
					mov [w2dTex]CWalk2D.uUFrac, esi
					mov [w2dTex]CWalk2D.uVFrac, ecx
					mov [w2dTex + 4]CWalk2D.iUVInt, edx
				}

				// Exit scanline routine if there are no more pixels to render.
				if (i_pixel == 0)
					break;

				// Implement perspective correction.
				index.UpdatePerspective(0);
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


//*****************************************************************************************
//
// Bumpmapped and Linear for 16 bit screen.
//
void DrawSubtriangle(TBumpLinearTbl* pscan, CDrawPolygon<TBumpLinearTbl>* pdtri)
{
	//
	// Do self-modifications here.
	//
	uint32 u4_table_address = (uint32)au1IntensityTrans;
	__asm
	{
		mov		ecx, [pvTextureBitmap]
		mov		eax, [au1SubBumpToIntensity]			// Load the pointer to the angle to intensity lookup table.
		mov		ebx, [pvClutConversion]					// Clut conversion pointer.
		mov     edx, dword ptr[u4_table_address]

		mov		[MODIFY_FOR_TEXTUREPOINTER_A-4], ecx
		mov		[MODIFY_FOR_ANGLETOINTENSITY-4], eax
		add		ecx, 3
		mov		[MODIFY_FOR_CLUT_CONVERSION-4], ebx
		mov		[MODIFY_FOR_TEXTUREPOINTER_B-4], ecx
		mov     [MODIFY_FOR_BUMPMAP_TABLE - 4], edx

		// scan line direction
		cmp		i4ScanlineDirection,0
		jle		short MDEC
		// +ve
		mov		byte ptr [MODIFY_DIRECTION], 047h	// inc edi
		jmp		short MDONE
MDEC:
		// -ve
		mov		byte ptr [MODIFY_DIRECTION], 04Fh	// dec edi
MDONE:
	}

	//
	// Iterate through the scanlines that intersect the subtriangle.
	//
	do
	{
		if (bRIGHT_TO_LEFT)
		{
			i_x_from = (pscan->fxX.i4Fx - pdtri->fxLineLength.i4Fx)>>16;
			i_x_to   = (pscan->fxX.i4Fx)>>16;
			i_screen_index = pdtri->iLineStartIndex + i_x_from - 1;
			i_pixel  = i_x_to - i_x_from;
		}
		else
		{
			i_x_from = (pscan->fxX.i4Fx)>>16;
			i_x_to   = (pscan->fxX.i4Fx + pdtri->fxLineLength.i4Fx)>>16;
			i_screen_index = pdtri->iLineStartIndex + i_x_to;
			i_pixel  = i_x_from - i_x_to;
		}

		// Draw if there are pixels to draw.
		if (i_x_to > i_x_from && !(bEvenScanlinesOnly && ((pdtri->iY & 1) == 0)))
		{
			// Get the start address for the scanline on the destination raster.
			TBumpLinearTbl::TPixel* apix_screen = (TBumpLinearTbl::TPixel*)pdtri->prasScreen->pSurface;

			// Iterate left or right along the destination scanline.
			__asm
			{
				mov		eax, pscan
				mov		ecx, i_screen_index	
				mov		edi, apix_screen
				push	ebp
			
				mov		edx, [eax]TBumpLinearTbl.indCoord.w1dV.bfxValue.i4Int
				mov		esi, [eax]TBumpLinearTbl.indCoord.bfU.u4Frac
				lea		ebp, [edi+ecx*2]
				mov		ecx, [eax]TBumpLinearTbl.indCoord.w1dV.bfxValue.u4Frac

				add		edx, [eax]TBumpLinearTbl.indCoord.bfU.i4Int
				mov		edi, i_pixel
				jmp		short BEGIN_INNER_LOOP

INNER_LOOP:
				// Add the U fractional values, setting the carry flag.
				add		esi, [w2dDeltaTex]CWalk2D.uUFrac
				// Add the integer values from the integer table, and add a possible carry.
				adc		edx, [w2dDeltaTex + eax*4 + 4]CWalk2D.iUVInt

BEGIN_INNER_LOOP:
					and edx, [u4TextureTileMask]
				movzx	eax, byte ptr[edx*4 + 0x12345678]
MODIFY_FOR_TEXTUREPOINTER_B:

				mov		ebx, [edx*4 + 0x12345678]
MODIFY_FOR_TEXTUREPOINTER_A:

				sub		ebx, [u4LightTheta]
				and		ebx, iMASK_ANGLETABLE_LOOKUP
				movzx	ebx, byte ptr[ebx + 0x12345678]
MODIFY_FOR_ANGLETOINTENSITY:
					
				movzx	ebx, byte ptr[ebx + 0x12345678]
			MODIFY_FOR_BUMPMAP_TABLE:

				shl		ebx, 8
				add		eax, ebx
				xor		ebx,ebx
				mov		bx, word ptr[eax*2 + 0x12345678]
MODIFY_FOR_CLUT_CONVERSION:

				// Add the V fractional values together and store the carry.
				add		ecx, [w2dDeltaTex]CWalk2D.uVFrac
				sbb		eax, eax
				mov		word ptr[ebp + edi*2], bx

MODIFY_DIRECTION:	nop
				jnz		short INNER_LOOP

				// Restore the base pointer.
				pop		ebp
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


