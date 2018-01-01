/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle (terrain texture) function.
 *		Optimized for the AMD K6-3D Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/DrawSubTriangleTerrainEx.cpp            $
 * 
 * 9     98.01.22 3:15p Mmouni
 * Added support for 16-bit deep bump-maps.
 * 
 * 8     97.11.15 12:00a Mmouni
 * Corrected some type names.
 * 
 * 7     97.11.04 10:59p Mmouni
 * Linear version of primitives now use 3dx/mmx.
 * 
 * 6     97.10.27 1:25p Mmouni
 * Initial K6-3D version, copied from the Pentium versions.
 * 
 * 11    97.10.22 7:11p Mmouni
 * Moved code that makes copies of edge values into assembly.
 * 
 * 10    10/10/97 1:47p Mmouni
 * All inner loops are now left to right only, and optimized some.
 * 
 * 9     10/01/97 7:17p Mmouni
 * Removed some more MOVZX instructions from the inner loops.
 * 
 * 8     9/30/97 9:32p Mmouni
 * Got rid of MOVZX in inner loop.
 * 
 * 7     9/15/97 2:10p Mmouni
 * Optimized inner loops (not final yet).
 * 
 * 6     9/01/97 8:03p Rwyatt
 * 
 * 5     97/08/19 16:15 Speter
 * (PKeet) Updated P5 and K6 shadow primitives.
 * 
 * 4     8/17/97 12:21a Rwyatt
 * Moved terrain texture sadow primitives from DrawSubTriangleFlat
 * 
 * 3     8/15/97 2:01a Rwyatt
 * Fixed 2 bugs related to 8 bit source textures
 * 
 * 2     8/15/97 12:50a Rwyatt
 * Initial implementation of terrain textures, this code does not do multiple writes so may not
 * be efficient on a Pentium or K6
 * 
 * 1     8/14/97 6:16p Rwyatt
 * Terrain texture primiteves initial implementation, Pentium
 * 
 **********************************************************************************************/

#include "Common.hpp"

#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"

//*********************************************************************************************


// Put this stuff in a seperate section for alignment.
#pragma code_seg("StriTerr")


//*****************************************************************************************
//
// Transparent, Linear, Terrain texture.
//
void DrawSubtriangle(TTexNoClutLinearTrans* pscan, CDrawPolygon<TTexNoClutLinearTrans>* pdtri)
{
	typedef CDrawPolygon<TTexNoClutLinearTrans> TAccess;
	typedef TTexNoClutLinearTrans::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TTexNoClutLinearTrans>* pdtriGlbl;
	static TTexNoClutLinearTrans* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TTexNoClutLinearTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.

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
		mov		ebx,[edi]TTexNoClutLinearTrans.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TTexNoClutLinearTrans.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TTexNoClutLinearTrans.indCoord.pfIndex
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

		movq		mm7,[eax]TTexNoClutLinearTrans.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TTexNoClutLinearTrans.fxX.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		mov		edx,[esi]TAccess.prasScreen

		add		ebx,ecx
		sar		edi,16

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex
		sar		ebx,16

		mov		edx,[edx]CRasterBase.pSurface
		add		ecx,ebx

		lea		edx,[edx + ecx*2]

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index2
		// ecx = ?					edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
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
		sub			esi,4							// Hack to force SIB + offset in loop

		packssdw	mm0,mm1							// Pack integer texture values
		pand		mm0,mm3							// Mask for tiling

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 1 past the beginning of a cache line.
		ALIGN	16
		nop

		//
		// Do two pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 2
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		pand		mm0,mm3							// Mask for tiling

		test		eax,eax
		jz			short SKIP_P1

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

SKIP_P1:
		test		ebx,ebx
		jz			short SKIP_P2

		mov			[edx + edi*2 - 2],bx			// Store pixel 2

SKIP_P2:
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
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 1
		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 2

		test		eax,eax
		jz			short SKIP_P1_END

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

SKIP_P1_END:
		test		ebx,ebx
		jz			short END_OF_SCANLINE

		mov			[edx + edi*2 - 2],bx			// Store pixel 2

		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		test		ebx,ebx
		jz			short END_OF_SCANLINE

		mov			[edx + edi*2 - 4],bx			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TTexNoClutLinearTrans.indCoord.pfIndex

		mov		ebx,[eax]TTexNoClutLinearTrans.fxXDifference

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
		mov		[eax]TTexNoClutLinearTrans.fxXDifference,ebx

		movq	[eax]TTexNoClutLinearTrans.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		mov		edx,[esi]TAccess.prasScreen

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[edx]CRasterBase.iLinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TTexNoClutLinearTrans.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TTexNoClutLinearTrans.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}


//*****************************************************************************************
//
// Non-transparent, Linear, Terrain texture.
//
void DrawSubtriangle(TTexNoClutLinear* pscan, CDrawPolygon<TTexNoClutLinear>* pdtri)
{
	typedef CDrawPolygon<TTexNoClutLinear> TAccess;
	typedef TTexNoClutLinear::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TTexNoClutLinear>* pdtriGlbl;
	static TTexNoClutLinear* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TTexNoClutLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.

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
		mov		ebx,[edi]TTexNoClutLinear.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TTexNoClutLinear.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TTexNoClutLinear.indCoord.pfIndex
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

		movq		mm7,[eax]TTexNoClutLinear.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TTexNoClutLinear.fxX.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		mov		edx,[esi]TAccess.prasScreen

		add		ebx,ecx
		sar		edi,16

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex
		sar		ebx,16

		mov		edx,[edx]CRasterBase.pSurface
		add		ecx,ebx

		lea		edx,[edx + ecx*2]

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index2
		// ecx = ?					edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
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
		sub			esi,4							// Hack to force SIB + offset in loop

		packssdw	mm0,mm1							// Pack integer texture values
		pand		mm0,mm3							// Mask for tiling

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 2 past the beginning of a cache line.
		ALIGN	16
		nop
		nop

		//
		// Do two pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 2
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		pand		mm0,mm3							// Mask for tiling

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

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
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 1
		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 2
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		mov			[edx + edi*2 - 4],bx			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TTexNoClutLinear.indCoord.pfIndex

		mov		ebx,[eax]TTexNoClutLinear.fxXDifference

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
		mov		[eax]TTexNoClutLinear.fxXDifference,ebx

		movq	[eax]TTexNoClutLinear.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		mov		edx,[esi]TAccess.prasScreen

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[edx]CRasterBase.iLinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TTexNoClutLinear.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TTexNoClutLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}


//*********************************************************************************************
//
// Draw shadow primitive.
//
void DrawSubtriangle(TShadow* pscan, CDrawPolygon<TShadow>* pdtri)
{
	typedef CDrawPolygon<TShadow> TAccess;

	// Local copies of edges stepping values.
	fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
	fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;

	// put the shadow colour in the top byte of both words within a dword
	u4ShadowColour = (u1ShadowIntensity<<8) | (u1ShadowIntensity<<24);

	_asm
	{
		mov		edx,[pdtri]
		mov		eax, [edx]CDrawPolygonBase.iY

		mov		edi,[edx]CDrawPolygonBase.iLineStartIndex
		mov		ecx,[pscan]
		//		edi, eax and edx are also set from the loop end

		//--------------------------------------------------------------------------------------
		// scan line start
NEXT_SCAN:
		and		eax,[bEvenScanlinesOnly]
		jnz		short SKIP_LOOP

		mov		ebx,[edx]TAccess.prasScreen
		mov		esi,[ecx]TShadow.fxX.i4Fx

		mov		eax,esi

		add		eax,[edx]CDrawPolygonBase.fxLineLength.i4Fx

		sar		esi,16

		sar		eax,16

		add		edi,eax

		sub		esi,eax
		jge		short SKIP_LOOP

		//--------------------------------------------------------------------------------------
		// all scan lines are drawn the same way
		add		edi,edi
		mov		ecx,esi

		add		edi,[ebx]CRasterBase.pSurface

		mov		ebx,[u4ShadowColour]

		add		ecx,edi

		and		ecx,3
		jz		short N_LOOP

		//--------------------------------------------------------------------------------------
		// do a single WORD to dword align
		mov		[edi + esi*2 + 1],bh

		inc		esi						// we are now aligned to a DWORD
		jz		short SKIP_LOOP			// are we done?

		//--------------------------------------------------------------------------------------
		// Main loop does 4 words at a time to avoid AGI
N_LOOP:
		add		esi,4
		jge		short DONE_SCAN
		mov		ecx,[edi + esi*2 - 8]
		mov		edx,[edi + esi*2 - 4]
		and		ecx,0x00ff00ff
		and		edx,0x00ff00ff
		or		ecx,ebx
		or		edx,ebx
		mov		[edi + esi*2 - 8],ecx
		mov		[edi + esi*2 - 4],edx
		jmp		short N_LOOP

DONE_SCAN:
		jz		short TWO_DWORD_REMAIN
		sub		esi,2
		jg		short SINGLE_WORD_REMAIN


		//--------------------------------------------------------------------------------------
		// There is either 2 or 3 pixles left when we get to here
		mov		edx,[edi + esi*2 - 4]
		and		edx,0x00ff00ff
		or		edx,ebx
		mov		[edi + esi*2 - 4],edx
		inc		esi
		jnz		short SKIP_LOOP

		//--------------------------------------------------------------------------------------
		// Do the single word that remains
		mov		[edi + esi*2 - 2 + 1],bh
		jmp		short SKIP_LOOP


SINGLE_WORD_REMAIN:
		//--------------------------------------------------------------------------------------
		// do a single word to finish
		movzx	ecx,word ptr [edi + esi*2 - 4]
		and		ecx,0x00ff
		or		ecx,ebx
		mov		[edi + esi*2 - 4], cx
		jmp		short SKIP_LOOP

		//--------------------------------------------------------------------------------------
		// do 2 DWORDs to finish
TWO_DWORD_REMAIN:
		mov		ecx,[edi + esi*2 - 8]
		mov		edx,[edi + esi*2 - 4]
		and		ecx,0x00ff00ff
		and		edx,0x00ff00ff
		or		ecx,ebx
		or		edx,ebx
		mov		[edi + esi*2 - 8], ecx
		mov		[edi + esi*2 - 4], edx

		//--------------------------------------------------------------------------------------
		// Loop control an sub pixel correction for edges
SKIP_LOOP:
		mov		edx,pdtri
		mov		ecx,pscan
		mov		eax, fx_inc
		mov		esi,[ecx]TShadow.fxXDifference
		add		[ecx]TShadow.fxX,eax
		add		esi, fx_diff
		mov		ebx,[edx]TAccess.prasScreen

		cmp		esi, 1<<16
		jz		short UFLOW

		// greater than or equal to 1 is an overflow
		sub		esi, 1<<16
		jmp		short DONE_Y
UFLOW:
		cmp		esi, 0
		jge		short DONE_Y
		add		esi, 1<<16
DONE_Y:
		// calc the new end point and line length
		mov		[ecx]TShadow.fxXDifference, esi
		mov		eax, [edx]CDrawPolygonBase.fxLineLength.i4Fx
		mov		edi, [edx]CDrawPolygonBase.iLineStartIndex
		add		eax, [edx]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		edi, [ebx]CRasterBase.iLinePixels
		mov     [edx]CDrawPolygonBase.fxLineLength.i4Fx, eax


		mov		eax, [edx]CDrawPolygonBase.iY
		mov		esi, [edx]CDrawPolygonBase.iYTo
		inc		eax
		mov     [edx]CDrawPolygonBase.iLineStartIndex, edi
		mov		[edx]CDrawPolygonBase.iY,eax
		cmp		eax, esi
		jl		short NEXT_SCAN
	}
}


//*********************************************************************************************
//
// Draw shadow transparent primitive, 8 bit source textures.
//
void DrawSubtriangle(TShadowTrans8* pscan, CDrawPolygon<TShadowTrans8>* pdtri)
{
	typedef CDrawPolygon<TShadowTrans8> TAccess;
	typedef TShadowTrans8::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TShadowTrans8>* pdtriGlbl;
	static TShadowTrans8* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TShadowTrans8* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.

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
		mov		ebx,[edi]TShadowTrans8.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TShadowTrans8.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TShadowTrans8.indCoord.pfIndex
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

		movq		mm7,[eax]TShadowTrans8.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TShadowTrans8.fxX.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		mov		edx,[esi]TAccess.prasScreen

		add		ebx,ecx
		sar		edi,16

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex
		sar		ebx,16

		mov		edx,[edx]CRasterBase.pSurface
		add		ecx,ebx

		lea		edx,[edx + ecx*2]

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index2
		// ecx = shadow intensity	edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
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
		mov			cl,[u1ShadowIntensity]			// Load shadow intensity value.

		packssdw	mm0,mm1							// Pack integer texture values
		pand		mm0,mm3							// Mask for tiling

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 1 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16
		nop

		//
		// Do two pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			al,byte ptr[esi + eax]			// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		mov			bl,byte ptr[esi + ebx]			// Lookup texel 2
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		pand		mm0,mm3							// Mask for tiling

		test		al,al
		jz			short SKIP_P1

		mov			[edx + edi*2 - 3],cl			// Store pixel 1

SKIP_P1:
		test		bl,bl
		jz			short SKIP_P2

		mov			[edx + edi*2 - 1],cl			// Store pixel 2

SKIP_P2:
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
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			al,byte ptr[esi + eax]			// Lookup texel 1
		mov			bl,byte ptr[esi + ebx]			// Lookup texel 2

		test		al,al
		jz			short SKIP_P1_END

		mov			[edx + edi*2 - 3],cl			// Store pixel 1

SKIP_P1_END:
		test		bl,bl
		jz			short END_OF_SCANLINE

		mov			[edx + edi*2 - 1],cl			// Store pixel 2

		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		mov			bl,byte ptr[esi + ebx]			// Lookup texel 1

		test		bl,bl
		jz			short END_OF_SCANLINE

		mov			[edx + edi*2 - 3],cl			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TShadowTrans8.indCoord.pfIndex

		mov		ebx,[eax]TShadowTrans8.fxXDifference

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
		mov		[eax]TShadowTrans8.fxXDifference,ebx

		movq	[eax]TShadowTrans8.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		mov		edx,[esi]TAccess.prasScreen

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[edx]CRasterBase.iLinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TShadowTrans8.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TShadowTrans8.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}

#if (iBUMPMAP_RESOLUTION == 16)

//*********************************************************************************************
//
// Draw shadow transparent primitive, 16 bit source textures (bumps)
//
void DrawSubtriangle(TShadowTrans32* pscan, CDrawPolygon<TShadowTrans32>* pdtri)
{
	typedef CDrawPolygon<TShadowTrans32> TAccess;
	typedef TShadowTrans32::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TShadowTrans32>* pdtriGlbl;
	static TShadowTrans32* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TShadowTrans32* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.

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
		mov		ebx,[edi]TShadowTrans32.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TShadowTrans32.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TShadowTrans32.indCoord.pfIndex
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

		movq		mm7,[eax]TShadowTrans32.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TShadowTrans32.fxX.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		mov		edx,[esi]TAccess.prasScreen

		add		ebx,ecx
		sar		edi,16

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex
		sar		ebx,16

		mov		edx,[edx]CRasterBase.pSurface
		add		ecx,ebx

		lea		edx,[edx + ecx*2]

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index2
		// ecx = shadow intensity	edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
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
		mov			cl,[u1ShadowIntensity]			// Load shadow intensity value.

		packssdw	mm0,mm1							// Pack integer texture values
		pand		mm0,mm3							// Mask for tiling

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 1 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16
		nop

		//
		// Do two pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			al,byte ptr[esi + eax*2 + 1]	// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		mov			bl,byte ptr[esi + ebx*2 + 1]	// Lookup texel 2
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		pand		mm0,mm3							// Mask for tiling

		and			al,0xfc
		jz			short SKIP_P1

		mov			[edx + edi*2 - 3],cl			// Store pixel 1

SKIP_P1:
		and			bl,0xfc
		jz			short SKIP_P2

		mov			[edx + edi*2 - 1],cl			// Store pixel 2

SKIP_P2:
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
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			al,byte ptr[esi + eax*2 + 1]	// Lookup texel 1
		mov			bl,byte ptr[esi + ebx*2 + 1]	// Lookup texel 2

		and			al,0xfc
		jz			short SKIP_P1_END

		mov			[edx + edi*2 - 3],cl			// Store pixel 1

SKIP_P1_END:
		and			bl,0xfc
		jz			short END_OF_SCANLINE

		mov			[edx + edi*2 - 1],cl			// Store pixel 2

		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		mov			bl,byte ptr[esi + ebx*2 + 1]	// Lookup texel 1

		and			bl,0xfc
		jz			short END_OF_SCANLINE

		mov			[edx + edi*2 - 3],cl			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TShadowTrans32.indCoord.pfIndex

		mov		ebx,[eax]TShadowTrans32.fxXDifference

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
		mov		[eax]TShadowTrans32.fxXDifference,ebx

		movq	[eax]TShadowTrans32.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		mov		edx,[esi]TAccess.prasScreen

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[edx]CRasterBase.iLinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TShadowTrans32.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TShadowTrans32.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}

#elif (iBUMPMAP_RESOLUTION == 32)	// if (iBUMPMAP_RESOLUTION == 16)

//*********************************************************************************************
//
// Draw shadow transparent primitive, 32 bit source textures (bumps)
//
void DrawSubtriangle(TShadowTrans32* pscan, CDrawPolygon<TShadowTrans32>* pdtri)
{
	typedef CDrawPolygon<TShadowTrans32> TAccess;
	typedef TShadowTrans32::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TShadowTrans32>* pdtriGlbl;
	static TShadowTrans32* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TShadowTrans32* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.

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
		mov		ebx,[edi]TShadowTrans32.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TShadowTrans32.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TShadowTrans32.indCoord.pfIndex
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

		movq		mm7,[eax]TShadowTrans32.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TShadowTrans32.fxX.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		mov		edx,[esi]TAccess.prasScreen

		add		ebx,ecx
		sar		edi,16

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex
		sar		ebx,16

		mov		edx,[edx]CRasterBase.pSurface
		add		ecx,ebx

		lea		edx,[edx + ecx*2]

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index2
		// ecx = shadow intensity	edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
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
		mov			cl,[u1ShadowIntensity]			// Load shadow intensity value.

		packssdw	mm0,mm1							// Pack integer texture values
		pand		mm0,mm3							// Mask for tiling

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 1 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16
		nop

		//
		// Do two pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			al,byte ptr[esi + eax*4 + 3]	// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		mov			bl,byte ptr[esi + ebx*4 + 3]	// Lookup texel 2
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		pand		mm0,mm3							// Mask for tiling

		test		al,al
		jz			short SKIP_P1

		mov			[edx + edi*2 - 3],cl			// Store pixel 1

SKIP_P1:
		test		bl,bl
		jz			short SKIP_P2

		mov			[edx + edi*2 - 1],cl			// Store pixel 2

SKIP_P2:
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
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			al,byte ptr[esi + eax*4 + 3]	// Lookup texel 1
		mov			bl,byte ptr[esi + ebx*4 + 3]	// Lookup texel 2

		test		al,al
		jz			short SKIP_P1_END

		mov			[edx + edi*2 - 3],cl			// Store pixel 1

SKIP_P1_END:
		test		bl,bl
		jz			short END_OF_SCANLINE

		mov			[edx + edi*2 - 1],cl			// Store pixel 2

		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		mov			bl,byte ptr[esi + ebx*4 + 3]	// Lookup texel 1

		test		bl,bl
		jz			short END_OF_SCANLINE

		mov			[edx + edi*2 - 3],cl			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TShadowTrans32.indCoord.pfIndex

		mov		ebx,[eax]TShadowTrans32.fxXDifference

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
		mov		[eax]TShadowTrans32.fxXDifference,ebx

		movq	[eax]TShadowTrans32.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		mov		edx,[esi]TAccess.prasScreen

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[edx]CRasterBase.iLinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TShadowTrans32.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TShadowTrans32.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}

#endif // elif (iBUMPMAP_RESOLUTION == 32)

// Restore default section.
#pragma code_seg()
