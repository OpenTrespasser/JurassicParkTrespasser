/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function.
 *		Optimized for the AMD K6-3D Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/DrawSubTriangleBumpTblEx.cpp            $
 * 
 * 6     97.11.04 10:59p Mmouni
 * Linear version of primitives now use 3dx/mmx.
 * 
 * 5     97.10.27 1:25p Mmouni
 * Initial K6-3D version, copied from the Pentium versions.
 * 
 * 6     97/10/12 21:40 Speter
 * Changed BumpLookup.au1CosToIntensity to au1IntensityTrans.
 * 
 * 5     9/01/97 8:02p Rwyatt
 * ANDs added to enable tiling
 * 
 * 4     97/07/18 3:29p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 3     97/07/16 15:49 Speter
 * Commented out redundant static delcarations now that all DrawSubTriangles are included into
 * one file (now just declared in DrawSubTriangleEx.cpp.)
 * 
 * 2     7/14/97 12:36p Rwyatt
 * Checkled in so Scott can move them....
 * 
 * 2     7/14/97 12:36p Rwyatt
 * Checkled in so Scott can move them....
 * 
 * 1     7/07/97 7:26p Rwyatt
 * Initial Implementation of Pentium Specific bump mapping
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

// Try to make variabels that would be local static so I do not need to use EBP to reference them.
// Variables that must be static can be force referenced by ESP if required.
// Note: the following static variables are declared in DrawSubTriangleBumpEx.cpp, which is
// included before me.  These should be made global instead.

extern uint32 u4ClutIndex;

// Declared in DrawSubTriangleEx.cpp.  Should be moved and renamed.
/*
static int    i_x_from;
static int    i_x_to;
static int    i_screen_index;
static int    i_pixel;
*/


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
					and	edx, [u4TextureTileMask]
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
	typedef TBumpLinearTbl::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TBumpLinear>* pdtriGlbl;
	static TBumpLinearTbl* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TBumpLinearTbl* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.
		push	ebp

		mov		esi,[pdtri]							// Pointer to polygon object.

		mov		eax,[pscan]							// Pointer to scanline object.
		mov		pdtriGlbl,esi

		//
		// Local copies of edge stepping values.
		//
		// fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
		// pf_texture_inc	= pdtri->pedgeBase->lineIncrement.indCoord.pfIndex;
		// pdtriGlbl		= pdtri;
		// pscanGlbl		= pscan;
		// fx_gour_inc		= pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		//
		mov		ebx,[edi]TBumpLinearTbl.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TBumpLinearTbl.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TBumpLinearTbl.indCoord.pfIndex
		mov		fx_diff,ecx

		movq	[pf_texture_inc],mm0

		//
		// Setup:
		//
		// mm2 = qMFactor = width,1,width,1
		// mm3 = qUVMasks
		// mm6 = uvslope
		//
		movd		mm0,[iTexWidth]					// 0,0,0,Width

		movd		mm2,[u4OneOne]					// 0,0,1,1

		movq		mm3,[qUVMasks]					// Load texture masks
		punpcklwd	mm0,mm0							// 0,0,Width,Width

		movq		mm6,[pfxDeltaTex]				// Load texture slopes.
		punpcklwd	mm2,mm0							// Width,1,Width,1

		movq		mm7,[eax]TBumpLinearTbl.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TBumpLinearTbl.fxX.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		mov		edx,gsGlobals.pvScreen

		add		ebx,ecx
		sar		edi,16

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex
		sar		ebx,16

		add		ecx,ebx

		lea		edx,[edx + ecx*2]

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// eax = texel1 temp		ebx = texel2 temp
		// ecx = texel1	temp		edx = dest_base_ptr
		// esi = texure base/temp	edi = pixel_offset & count
		// ebp = clut/bump temp
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		pf2id		(m7,m7)							// Convert texture values to 16.16 fixed.
		mov			esi,[pvTextureBitmap]			// Load texture ponter.

		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm6							// Step U,V

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values

		pand		mm0,mm3							// Mask for tiling
		pmaddwd		mm0,mm2							// Compute texture indicies

		movq		[qIndexTemp],mm0				// Save texture indicies.

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 9? past the beginning of a cache line.
		ALIGN	16
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop

		//
		// Do two pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ecx,dword ptr[esi + eax*4]		// Lookup texel 1.
		psrad		mm0,16							// Shift for integer U1,V1

		mov			esi,dword ptr[esi + ebx*4]		// Lookup texel 2.
		paddd		mm7,mm6							// Step U,V

		mov			eax,ecx							// Copy texel 1
		sub			ecx,[u4LightTheta]				// Subtract light theta 1.

		mov			ebx,esi							// Copy texel 2
		sub			esi,[u4LightTheta]				// Subtract light theta 2.

		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		shr			eax,24							// Shift color value down.

		shr			ebx,24							// Shift color value down.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		movzx		ecx,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		and			esi,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.

		movzx		esi,byte ptr[ebp + esi]			// Bump map intensity lookup 2.
		mov			ebp,[au1IntensityTrans]			// Additional bump map shading.

		mov			ah,byte ptr[ebp + ecx]			// Do additional lookup for intensity.
		movq		mm1,mm7							// Copy U2,V2

		mov			bh,byte ptr[ebp + esi]			// Do additional lookup for intensity.
		mov			ebp,[pvClutConversion]			// Base of CLUT.

		psrad		mm1,16							// Shift for integer U2,V2
		mov			esi,[pvTextureBitmap]			// Base of texture.

		packssdw	mm0,mm1							// Pack integer texture values.
		pand		mm0,mm3							// Mask for tiling

		movzx		eax,word ptr[ebp + eax*2]		// Clut lookup 1.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

		movzx		ebx,word ptr[ebp + ebx*2]		// Clut lookup 2.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			[edx + edi*2 - 2],bx			// Store pixel 2

		add			edi,2							// Inrcement index by 2
		jl			short INNER_LOOP_2P				// Loop (while >2 pixels)

FINISH_REMAINDER:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			short ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture co-ordinates once.
		//
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			ecx,dword ptr[esi + eax*4]		// Lookup texel 1.
		mov			esi,dword ptr[esi + ebx*4]		// Lookup texel 2.
		mov			eax,ecx							// Copy texel 1
		sub			ecx,[u4LightTheta]				// Subtract light theta 1.
		mov			ebx,esi							// Copy texel 2
		sub			esi,[u4LightTheta]				// Subtract light theta 2.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		shr			eax,24							// Shift color value down.
		shr			ebx,24							// Shift color value down.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		movzx		ecx,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		and			esi,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		movzx		esi,byte ptr[ebp + esi]			// Bump map intensity lookup 2.
		mov			ebp,[au1IntensityTrans]			// Additional bump map shading.
		mov			ah,byte ptr[ebp + ecx]			// Do additional lookup.
		mov			bh,byte ptr[ebp + esi]			// Do additional lookup.
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		movzx		eax,word ptr[ebp + eax*2]		// Clut lookup 1.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		movzx		ebx,word ptr[ebp + ebx*2]		// Clut lookup 2.
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ecx,dword ptr[esi + eax*4]		// Lookup texel 1.
		mov			eax,ecx							// Copy texel 1
		sub			ecx,[u4LightTheta]				// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		shr			eax,24							// Shift color value down.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		movzx		ecx,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		mov			ebp,[au1IntensityTrans]			// Additional bump map shading.
		mov			ah,byte ptr[ebp + ecx]			// Do additional lookup.
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		movzx		eax,word ptr[ebp + eax*2]		// Clut lookup 1.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TBumpLinearTbl.indCoord.pfIndex

		mov		ebx,[eax]TBumpLinearTbl.fxXDifference

		pfadd	(m7,m1)
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000
		movq	mm1,[pfNegD]

		pfadd	(m7,m1)
		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000
		movq	mm1,[pfNegD]

		pfadd	(m7,m1)

EDGE_DONE:
		// Store modified variables and do looping.
		// Step length, line starting address.
		mov		esi,[pdtriGlbl]
		mov		[eax]TBumpLinearTbl.fxXDifference,ebx

		movq	[eax]TBumpLinearTbl.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TBumpLinearTbl.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TBumpLinearTbl.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp

		// Done with MMX/3DX.
		femms
	}
}
