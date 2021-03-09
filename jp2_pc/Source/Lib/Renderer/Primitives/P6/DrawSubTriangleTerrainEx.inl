#pragma once

/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle (terrain texture) function.
 *		Optimized for the Pentium PRO Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P6/DrawSubTriangleTerrainEx.cpp               $
 * 
 * 12    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 11    98/01/26 11:49a Mmouni
 * 
 * 10    97.12.03 7:46p Mmouni
 * Optimized P6 versions based on P5 version.
 * 
 * 9     97/11/25 3:28p Pkeet
 * Fixed shadow bump map primitive.
 * 
 * 8     9/15/97 2:07p Mmouni
 * Terrain drawing primitives now use texture tile mask.
 * 
 * 7     9/01/97 8:05p Rwyatt
 * 
 * 6     8/19/97 12:09a Rwyatt
 * Added 8 and 32 bit shadow prims in asm
 * 
 * 5     8/18/97 1:56p Rwyatt
 * Solid shadow primitive is optimized to do 8 bytes at a time, it has been difficult to remove
 * the partial stalls.
 * 
 * 4     8/17/97 12:21a Rwyatt
 * Moved terrain texture sadow primitives from DrawSubTriangleFlat
 * 
 * 3     8/15/97 1:58a Rwyatt
 * Fixed 2 bugs, the first was the source texture offset being multipled by 2 and the second was
 * left to right scan lines used word ptr to load instead of byte ptr...
 * 
 * 2     8/15/97 12:50a Rwyatt
 * Initial implementation of terrain textures, this code does not do multiple writes so may not
 * be efficient on a Pentium or K6
 * 
 * 1     8/14/97 6:16p Rwyatt
 * Terrain texture primiteves initial implementation, Pentium Pro
 * 
 **********************************************************************************************/

#include "common.hpp"

#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"


//*****************************************************************************************
//
// Transparent, Linear, Terrain texture.
//
void DrawSubtriangle(TTexNoClutLinearTrans* pscan, CDrawPolygon<TTexNoClutLinearTrans>* pdtri)
{
	typedef CDrawPolygon<TTexNoClutLinearTrans> TAccess;
	TTexNoClutLinearTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	_asm
	{
		push	ebp
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
		mov		ebx,[edi]TTexNoClutLinearTrans.fxX
		mov		ecx,[edi]TTexNoClutLinearTrans.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TTexNoClutLinearTrans.indCoord.bfU.i4Int
		mov		ecx,[edi]TTexNoClutLinearTrans.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TTexNoClutLinearTrans.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TTexNoClutLinearTrans.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriTerTrans,esi
		mov		pscanTerTrans,eax

		mov		ebx,[pvTextureBitmap]
		mov		ebp,[esi]CDrawPolygonBase.iY

		lea		ecx,LIN_TEXTURE_MODIFYB1
		mov		[ecx-4],ebx

		// -------------------------------------------------------------------------
		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// -------------------------------------------------------------------------

LIN_NEXT_SCAN_LINE:
		// check for odd scan lines here and skip if even
		and		ebp,bEvenScanlinesOnly
		jnz		INC_BASE_EDGE

		mov		ecx,[esi]TAccess.prasScreen

		mov		edi,[eax]TTexNoClutLinearTrans.fxX.i4Fx
		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex

		mov		ebx,edi

		add		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		sar		ebx,16

		mov		esi, [ecx]CRasterBase.pSurface

		sar		edi,16
		lea		ecx,[edx+ebx]

		sub		edi,ebx
		jge		INC_BASE_EDGE				// no pixels

		mov		ebx,[eax]TTexNoClutLinearTrans.indCoord.bfU.u4Frac				// UFrac
		mov		edx,[eax]TTexNoClutLinearTrans.indCoord.w1dV.bfxValue.i4Int

		lea		esi, [esi + ecx*2]

		add		edx, [eax]TTexNoClutLinearTrans.indCoord.bfU.i4Int
		mov		ecx,[eax]TTexNoClutLinearTrans.indCoord.w1dV.bfxValue.u4Frac		// VFrac

		//------------------------------------------------------------------------------
LIN_LOOP_B:
		and		edx,[u4TextureTileMask]
		mov		eax,[w2dDeltaTex]CWalk2D.uVFrac
		add		ecx,eax

		sbb		ebp,ebp
		movzx	eax,byte ptr [0xDEADC0DE + edx]
LIN_TEXTURE_MODIFYB1:

		test	eax,eax
		jz		short LIN_TRANSPARENT_B

		mov		word ptr[esi + edi*2],ax
LIN_TRANSPARENT_B:
		mov		eax,[w2dDeltaTex]CWalk2D.uUFrac
		add		ebx,eax

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt
		inc		edi
		jnz		short LIN_LOOP_B

		// -------------------------------------------------------------------------
INC_BASE_EDGE:
		mov		eax, pscanTerTrans
		mov		edi, fx_inc

		mov		ebx,[eax]TTexNoClutLinearTrans.fxXDifference
		mov		ecx,[eax]TTexNoClutLinearTrans.indCoord.bfU			// bottom 32 bits
		add		ebx,fx_diff
		mov		edx,[eax+4]TTexNoClutLinearTrans.indCoord.bfU		// top 32 bits
		add		[eax]TTexNoClutLinearTrans.fxX, edi
		add		ecx,DWORD PTR [bf_u_inc]
		mov		edi, [eax]TTexNoClutLinearTrans.indCoord.w1dV.bfxValue.i4Int
		adc		edx,DWORD PTR [bf_u_inc+4]
		mov		esi, [eax]TTexNoClutLinearTrans.indCoord.w1dV.bfxValue.u4Frac
		// Add the integer portions together.
		add		edi, w1d_v_inc.bfxValue.i4Int
		// Add the fractional portions together, and add an extra step value if a carry
		// results.
		add		esi, w1d_v_inc.bfxValue.u4Frac
		jnc		short SKIP_STEP_ADD1
		add		edi, [eax]TTexNoClutLinearTrans.indCoord.w1dV.iOffsetPerLine
SKIP_STEP_ADD1:

		cmp		ebx, 0x00010000
		jl		short XDIFF_LESS
		// do this if greater than or equal

		sub		ebx, 0x00010000

		add		edi, w1dNegDV.bfxValue.i4Int

		add		ecx, DWORD PTR [bfNegDU]
		adc		edx, DWORD PTR [bfNegDU+4]

		add		esi, w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE
		add		edi, [eax]TTexNoClutLinearTrans.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

XDIFF_LESS:
		cmp		ebx,0
		jge		short EDGE_DONE
		// do this if less than 0

		add		ebx, 0x00010000
		add		edi, w1dNegDV.bfxValue.i4Int
		add		ecx, DWORD PTR [bfNegDU]
		adc		edx, DWORD PTR [bfNegDU+4]
		add		esi, w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE
		add		edi, [eax]TTexNoClutLinearTrans.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// -------------------------------------------------------------------------
		// store all the working variables back to the original raster
		// and also interleave the co-ordinate update variables..
		mov		[eax]TTexNoClutLinearTrans.indCoord.w1dV.bfxValue.u4Frac, esi
		mov		esi,pdtriTerTrans
		mov		[eax]TTexNoClutLinearTrans.indCoord.bfU, ecx
		mov		[eax]TTexNoClutLinearTrans.indCoord.w1dV.bfxValue.i4Int, edi
		mov		[eax]TTexNoClutLinearTrans.fxXDifference, ebx
		mov		ecx,[esi]TAccess.prasScreen
		mov		[eax]TTexNoClutLinearTrans.indCoord.bfU+4, edx
		mov		edi,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		mov		ebp,[esi]CDrawPolygonBase.iY
		mov		edx,[ecx]CRasterBase.iLinePixels
		add		[esi]CDrawPolygonBase.fxLineLength.i4Fx, edi
		inc		ebp
		add		[esi]CDrawPolygonBase.iLineStartIndex, edx
		cmp		ebp,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY, ebp
		jl		LIN_NEXT_SCAN_LINE
		pop ebp
	}
}


//*****************************************************************************************
//
// Non-transparent, Linear, Terrain texture.
//
void DrawSubtriangle(TTexNoClutLinear* pscan, CDrawPolygon<TTexNoClutLinear>* pdtri)
{
	typedef CDrawPolygon<TTexNoClutLinear> TAccess;
	TTexNoClutLinear* plinc = &pdtri->pedgeBase->lineIncrement;
	
	_asm
	{
		push	ebp
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
		mov		ebx,[edi]TTexNoClutLinear.fxX
		mov		ecx,[edi]TTexNoClutLinear.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TTexNoClutLinear.indCoord.bfU.i4Int
		mov		ecx,[edi]TTexNoClutLinear.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TTexNoClutLinear.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TTexNoClutLinear.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriTer,esi
		mov		pscanTer,eax

		mov		ebx,[pvTextureBitmap]
		mov		ebp,[esi]CDrawPolygonBase.iY

		lea		ecx,LIN_TEXTURE_MODIFYB1
		mov		[ecx-4],ebx

		// -------------------------------------------------------------------------
		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// -------------------------------------------------------------------------

LIN_NEXT_SCAN_LINE:
		// check for odd scan lines here and skip if even
		and		ebp,bEvenScanlinesOnly
		jnz		INC_BASE_EDGE

		mov		ecx,[esi]TAccess.prasScreen

		mov		edi,[eax]TTexNoClutLinear.fxX.i4Fx
		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex

		mov		ebx,edi

		add		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		sar		ebx,16

		mov		esi, [ecx]CRasterBase.pSurface

		sar		edi,16
		lea		ecx,[edx+ebx]

		sub		edi,ebx
		jge		INC_BASE_EDGE				// no pixels

		mov		ebx,[eax]TTexNoClutLinear.indCoord.bfU.u4Frac				// UFrac
		mov		edx,[eax]TTexNoClutLinear.indCoord.w1dV.bfxValue.i4Int

		lea		esi, [esi + ecx*2]

		add		edx, [eax]TTexNoClutLinear.indCoord.bfU.i4Int
		mov		ecx,[eax]TTexNoClutLinear.indCoord.w1dV.bfxValue.u4Frac		// VFrac
		xor		eax,eax

		//------------------------------------------------------------------------------
LIN_LOOP_B:
		and		edx,[u4TextureTileMask]
		mov		eax,[w2dDeltaTex]CWalk2D.uVFrac
		add		ecx,eax

		sbb		ebp,ebp
		movzx	eax,byte ptr [0xDEADC0DE + edx]
LIN_TEXTURE_MODIFYB1:

		mov		word ptr[esi + edi*2],ax
		mov		eax,[w2dDeltaTex]CWalk2D.uUFrac
		add		ebx,eax

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt
		inc		edi
		jnz		short LIN_LOOP_B

		// -------------------------------------------------------------------------
INC_BASE_EDGE:
		mov		eax,pscanTer
		mov		edi,fx_inc

		mov		ebx,[eax]TTexNoClutLinear.fxXDifference
		mov		ecx,[eax]TTexNoClutLinear.indCoord.bfU			// bottom 32 bits
		add		ebx,fx_diff
		mov		edx,[eax+4]TTexNoClutLinear.indCoord.bfU		// top 32 bits
		add		[eax]TTexNoClutLinear.fxX, edi
		add		ecx,DWORD PTR [bf_u_inc]
		mov		edi, [eax]TTexNoClutLinear.indCoord.w1dV.bfxValue.i4Int
		adc		edx,DWORD PTR [bf_u_inc+4]
		mov		esi, [eax]TTexNoClutLinear.indCoord.w1dV.bfxValue.u4Frac
		// Add the integer portions together.
		add		edi, w1d_v_inc.bfxValue.i4Int
		// Add the fractional portions together, and add an extra step value if a carry
		// results.
		add		esi, w1d_v_inc.bfxValue.u4Frac
		jnc		short SKIP_STEP_ADD1
		add		edi, [eax]TTexNoClutLinear.indCoord.w1dV.iOffsetPerLine
SKIP_STEP_ADD1:

		cmp		ebx, 0x00010000
		jl		short XDIFF_LESS
		// do this if greater than or equal

		sub		ebx, 0x00010000

		add		edi, w1dNegDV.bfxValue.i4Int

		add		ecx, DWORD PTR [bfNegDU]
		adc		edx, DWORD PTR [bfNegDU+4]

		add		esi, w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE
		add		edi, [eax]TTexNoClutLinear.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

XDIFF_LESS:
		cmp		ebx,0
		jge		short EDGE_DONE
		// do this if less than 0

		add		ebx, 0x00010000
		add		edi, w1dNegDV.bfxValue.i4Int
		add		ecx, DWORD PTR [bfNegDU]
		adc		edx, DWORD PTR [bfNegDU+4]
		add		esi, w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE
		add		edi, [eax]TTexNoClutLinear.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// -------------------------------------------------------------------------
		// store all the working variables back to the original raster
		// and also interleave the co-ordinate update variables..
		mov		[eax]TTexNoClutLinear.indCoord.w1dV.bfxValue.u4Frac, esi
		mov		esi,pdtriTer
		mov		[eax]TTexNoClutLinear.indCoord.bfU, ecx
		mov		[eax]TTexNoClutLinear.indCoord.w1dV.bfxValue.i4Int, edi
		mov		[eax]TTexNoClutLinear.fxXDifference, ebx
		mov		ecx,[esi]TAccess.prasScreen
		mov		[eax]TTexNoClutLinear.indCoord.bfU+4, edx
		mov		edi,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		mov		ebp,[esi]CDrawPolygonBase.iY
		mov		edx,[ecx]CRasterBase.iLinePixels
		add		[esi]CDrawPolygonBase.fxLineLength.i4Fx, edi
		inc		ebp
		add		[esi]CDrawPolygonBase.iLineStartIndex, edx
		cmp		ebp,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY, ebp
		jl		LIN_NEXT_SCAN_LINE
		pop ebp
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
	TShadowTrans8* plinc = &pdtri->pedgeBase->lineIncrement;

	_asm
	{
		push	ebp
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
		mov		ebx,[edi]TShadowTrans8.fxX
		mov		ecx,[edi]TShadowTrans8.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TShadowTrans8.indCoord.bfU.i4Int
		mov		ecx,[edi]TShadowTrans8.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TShadowTrans8.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TShadowTrans8.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriShadow8,esi
		mov		pscanShadow8,eax

		mov		ebx,[pvTextureBitmap]
		mov		ebp,[esi]CDrawPolygonBase.iY

		lea		ecx,LIN_TEXTURE_MODIFYB1
		mov		[ecx-4],ebx						// Self modification.

		// -------------------------------------------------------------------------
		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// -------------------------------------------------------------------------

LIN_NEXT_SCAN_LINE:
		// check for odd scan lines here and skip if even
		and		ebp,bEvenScanlinesOnly
		jnz		INC_BASE_EDGE

		mov		ecx,[esi]TAccess.prasScreen

		mov		edi,[eax]TShadowTrans8.fxX.i4Fx
		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex

		mov		ebx,edi

		add		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		sar		ebx,16

		mov		esi,[ecx]CRasterBase.pSurface

		sar		edi,16
		lea		ecx,[edx+ebx]

		sub		edi,ebx
		jge		INC_BASE_EDGE											// no pixels

		mov		ebx,[eax]TShadowTrans8.indCoord.bfU.u4Frac				// UFrac
		mov		edx,[eax]TShadowTrans8.indCoord.w1dV.bfxValue.i4Int		// UInt

		lea		esi,[esi + ecx*2]
		mov		ecx,[eax]TShadowTrans8.indCoord.bfU.i4Int

		add		edx,ecx													// VInt
		mov		ecx,[eax]TShadowTrans8.indCoord.w1dV.bfxValue.u4Frac	// VFrac

		mov		ah,[u1ShadowIntensity]									// Load shadow value

		//------------------------------------------------------------------------------
LIN_LOOP:
		and		edx,[u4TextureTileMask]							// Tiling mask

		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac					// V step

		sbb		ebp,ebp											// Carry from V step
		mov		al,byte ptr[0xDEADC0DE + edx]					// Byte of source texture
LIN_TEXTURE_MODIFYB1:

		add		ebx,[w2dDeltaTex]CWalk2D.uUFrac					// U step

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step
		test	al,al											// Test for transparency.
		jz		short TRANS

		// insert the shadow colour into the destination raster		
		mov		byte ptr[esi + edi*2 + 1],ah

TRANS:
		inc		edi
		jnz		short LIN_LOOP

		// -------------------------------------------------------------------------
INC_BASE_EDGE:
		mov		eax, pscanShadow8
		mov		edi, fx_inc

		mov		ebx,[eax]TShadowTrans8.fxXDifference
		mov		ecx,[eax]TShadowTrans8.indCoord.bfU			// bottom 32 bits
		add		ebx,fx_diff
		mov		edx,[eax+4]TShadowTrans8.indCoord.bfU		// top 32 bits
		add		[eax]TShadowTrans8.fxX, edi
		add		ecx,DWORD PTR [bf_u_inc]
		mov		edi, [eax]TShadowTrans8.indCoord.w1dV.bfxValue.i4Int
		adc		edx,DWORD PTR [bf_u_inc+4]
		mov		esi, [eax]TShadowTrans8.indCoord.w1dV.bfxValue.u4Frac
		// Add the integer portions together.
		add		edi, w1d_v_inc.bfxValue.i4Int
		// Add the fractional portions together, and add an extra step value if a carry
		// results.
		add		esi, w1d_v_inc.bfxValue.u4Frac
		jnc		short SKIP_STEP_ADD1
		add		edi, [eax]TShadowTrans8.indCoord.w1dV.iOffsetPerLine
SKIP_STEP_ADD1:

		cmp		ebx, 0x00010000
		jl		short XDIFF_LESS
		// do this if greater than or equal

		sub		ebx, 0x00010000

		add		edi, w1dNegDV.bfxValue.i4Int

		add		ecx, DWORD PTR [bfNegDU]
		adc		edx, DWORD PTR [bfNegDU+4]

		add		esi, w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE
		add		edi, [eax]TShadowTrans8.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

XDIFF_LESS:
		cmp		ebx,0
		jge		short EDGE_DONE
		// do this if less than 0

		add		ebx, 0x00010000
		add		edi, w1dNegDV.bfxValue.i4Int
		add		ecx, DWORD PTR [bfNegDU]
		adc		edx, DWORD PTR [bfNegDU+4]
		add		esi, w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE
		add		edi, [eax]TShadowTrans8.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// -------------------------------------------------------------------------
		// store all the working variables back to the original raster
		// and also interleave the co-ordinate update variables..
		mov		[eax]TShadowTrans8.indCoord.w1dV.bfxValue.u4Frac, esi
		mov		esi,pdtriShadow8
		mov		[eax]TShadowTrans8.indCoord.bfU, ecx
		mov		[eax]TShadowTrans8.indCoord.w1dV.bfxValue.i4Int, edi
		mov		[eax]TShadowTrans8.fxXDifference, ebx
		mov		ecx,[esi]TAccess.prasScreen
		mov		[eax]TShadowTrans8.indCoord.bfU+4, edx
		mov		edi,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		mov		ebp,[esi]CDrawPolygonBase.iY
		mov		edx,[ecx]CRasterBase.iLinePixels
		add		[esi]CDrawPolygonBase.fxLineLength.i4Fx, edi
		inc		ebp
		add		[esi]CDrawPolygonBase.iLineStartIndex, edx
		cmp		ebp,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY, ebp
		jl		LIN_NEXT_SCAN_LINE
		pop ebp
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
	TShadowTrans32* plinc = &pdtri->pedgeBase->lineIncrement;

	_asm
	{
		push	ebp
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
		mov		ebx,[edi]TShadowTrans32.fxX
		mov		ecx,[edi]TShadowTrans32.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TShadowTrans32.indCoord.bfU.i4Int
		mov		ecx,[edi]TShadowTrans32.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TShadowTrans32.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TShadowTrans32.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriShadow32,esi
		mov		ebx,[pvTextureBitmap]

		mov		pscanShadow32,eax
		add		ebx,1								// Byte of bump-map that contains color value.

		mov		ebp,[esi]CDrawPolygonBase.iY

		lea		ecx,LIN_TEXTURE_MODIFYB1
		mov		[ecx-4],ebx							// Self modification.

		// -------------------------------------------------------------------------
		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// -------------------------------------------------------------------------

LIN_NEXT_SCAN_LINE:
		// check for odd scan lines here and skip if even
		and		ebp,bEvenScanlinesOnly
		jnz		INC_BASE_EDGE

		mov		ecx,[esi]TAccess.prasScreen

		mov		edi,[eax]TShadowTrans32.fxX.i4Fx
		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex

		mov		ebx,edi

		add		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		sar		ebx,16

		mov		esi, [ecx]CRasterBase.pSurface

		sar		edi,16
		lea		ecx,[edx+ebx]

		sub		edi,ebx
		jge		INC_BASE_EDGE				// no pixels

		mov		ebx,[eax]TShadowTrans32.indCoord.bfU.u4Frac				// UFrac
		mov		edx,[eax]TShadowTrans32.indCoord.w1dV.bfxValue.i4Int	// UInt

		lea		esi,[esi + ecx*2]
		mov		ecx,[eax]TShadowTrans32.indCoord.bfU.i4Int

		add		edx,ecx													// VInt
		mov		ecx,[eax]TShadowTrans32.indCoord.w1dV.bfxValue.u4Frac	// VFrac

		mov		ah,[u1ShadowIntensity]									// Load shadow value

		//------------------------------------------------------------------------------
LIN_LOOP:
		and		edx,[u4TextureTileMask]							// Tiling mask

		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac					// V step

		sbb		ebp,ebp											// Carry from V step
		mov		al,[0xDEADC0DE + edx*2]							// Byte of source texture
LIN_TEXTURE_MODIFYB1:

		add		ebx,[w2dDeltaTex]CWalk2D.uUFrac					// U step

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step
		test	al,0xfc											// Test for transparency.
		jz		short TRANS

		// insert the shadow colour into the destination raster		
		mov		byte ptr[esi + edi*2 + 1],ah

TRANS:
		inc		edi
		jnz		short LIN_LOOP

		// -------------------------------------------------------------------------
INC_BASE_EDGE:
		mov		eax, pscanShadow32
		mov		edi, fx_inc

		mov		ebx,[eax]TShadowTrans32.fxXDifference
		mov		ecx,[eax]TShadowTrans32.indCoord.bfU			// bottom 32 bits
		add		ebx,fx_diff
		mov		edx,[eax+4]TShadowTrans32.indCoord.bfU		// top 32 bits
		add		[eax]TShadowTrans32.fxX, edi
		add		ecx,DWORD PTR [bf_u_inc]
		mov		edi, [eax]TShadowTrans32.indCoord.w1dV.bfxValue.i4Int
		adc		edx,DWORD PTR [bf_u_inc+4]
		mov		esi, [eax]TShadowTrans32.indCoord.w1dV.bfxValue.u4Frac
		// Add the integer portions together.
		add		edi, w1d_v_inc.bfxValue.i4Int
		// Add the fractional portions together, and add an extra step value if a carry
		// results.
		add		esi, w1d_v_inc.bfxValue.u4Frac
		jnc		short SKIP_STEP_ADD1
		add		edi, [eax]TShadowTrans32.indCoord.w1dV.iOffsetPerLine
SKIP_STEP_ADD1:

		cmp		ebx, 0x00010000
		jl		short XDIFF_LESS
		// do this if greater than or equal

		sub		ebx, 0x00010000

		add		edi, w1dNegDV.bfxValue.i4Int

		add		ecx, DWORD PTR [bfNegDU]
		adc		edx, DWORD PTR [bfNegDU+4]

		add		esi, w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE
		add		edi, [eax]TShadowTrans32.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

XDIFF_LESS:
		cmp		ebx,0
		jge		short EDGE_DONE
		// do this if less than 0

		add		ebx, 0x00010000
		add		edi, w1dNegDV.bfxValue.i4Int
		add		ecx, DWORD PTR [bfNegDU]
		adc		edx, DWORD PTR [bfNegDU+4]
		add		esi, w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE
		add		edi, [eax]TShadowTrans32.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// -------------------------------------------------------------------------
		// store all the working variables back to the original raster
		// and also interleave the co-ordinate update variables..
		mov		[eax]TShadowTrans32.indCoord.w1dV.bfxValue.u4Frac, esi
		mov		esi,pdtriShadow32
		mov		[eax]TShadowTrans32.indCoord.bfU, ecx
		mov		[eax]TShadowTrans32.indCoord.w1dV.bfxValue.i4Int, edi
		mov		[eax]TShadowTrans32.fxXDifference, ebx
		mov		ecx,[esi]TAccess.prasScreen
		mov		[eax]TShadowTrans32.indCoord.bfU+4, edx
		mov		edi,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		mov		ebp,[esi]CDrawPolygonBase.iY
		mov		edx,[ecx]CRasterBase.iLinePixels
		add		[esi]CDrawPolygonBase.fxLineLength.i4Fx, edi
		inc		ebp
		add		[esi]CDrawPolygonBase.iLineStartIndex, edx
		cmp		ebp,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY, ebp
		jl		LIN_NEXT_SCAN_LINE
		pop ebp
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
	TShadowTrans32* plinc = &pdtri->pedgeBase->lineIncrement;

	_asm
	{
		push	ebp
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
		mov		ebx,[edi]TShadowTrans32.fxX
		mov		ecx,[edi]TShadowTrans32.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TShadowTrans32.indCoord.bfU.i4Int
		mov		ecx,[edi]TShadowTrans32.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TShadowTrans32.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TShadowTrans32.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriShadow32,esi
		mov		ebx,[pvTextureBitmap]

		mov		pscanShadow32,eax
		add		ebx,3								// Byte of bump-map that is color value.

		mov		ebp,[esi]CDrawPolygonBase.iY

		lea		ecx,LIN_TEXTURE_MODIFYB1
		mov		[ecx-4],ebx							// Self modification.

		// -------------------------------------------------------------------------
		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// -------------------------------------------------------------------------

LIN_NEXT_SCAN_LINE:
		// check for odd scan lines here and skip if even
		and		ebp,bEvenScanlinesOnly
		jnz		INC_BASE_EDGE

		mov		ecx,[esi]TAccess.prasScreen

		mov		edi,[eax]TShadowTrans32.fxX.i4Fx
		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex

		mov		ebx,edi

		add		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		sar		ebx,16

		mov		esi, [ecx]CRasterBase.pSurface

		sar		edi,16
		lea		ecx,[edx+ebx]

		sub		edi,ebx
		jge		INC_BASE_EDGE				// no pixels

		mov		ebx,[eax]TShadowTrans32.indCoord.bfU.u4Frac				// UFrac
		mov		edx,[eax]TShadowTrans32.indCoord.w1dV.bfxValue.i4Int	// UInt

		lea		esi,[esi + ecx*2]
		mov		ecx,[eax]TShadowTrans32.indCoord.bfU.i4Int

		add		edx,ecx													// VInt
		mov		ecx,[eax]TShadowTrans32.indCoord.w1dV.bfxValue.u4Frac	// VFrac

		mov		ah,[u1ShadowIntensity]									// Load shadow value

		//------------------------------------------------------------------------------
LIN_LOOP:
		and		edx,[u4TextureTileMask]							// Tiling mask

		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac					// V step

		sbb		ebp,ebp											// Carry from V step
		mov		al,byte ptr [0xDEADC0DE + edx*4]				// Byte of source texture
LIN_TEXTURE_MODIFYB1:

		add		ebx,[w2dDeltaTex]CWalk2D.uUFrac					// U step

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step
		test	al,al											// Test for transparency.
		jz		short TRANS

		// insert the shadow colour into the destination raster		
		mov		byte ptr[esi + edi*2 + 1],ah

TRANS:
		inc		edi
		jnz		short LIN_LOOP

		// -------------------------------------------------------------------------
INC_BASE_EDGE:
		mov		eax, pscanShadow32
		mov		edi, fx_inc

		mov		ebx,[eax]TShadowTrans32.fxXDifference
		mov		ecx,[eax]TShadowTrans32.indCoord.bfU			// bottom 32 bits
		add		ebx,fx_diff
		mov		edx,[eax+4]TShadowTrans32.indCoord.bfU		// top 32 bits
		add		[eax]TShadowTrans32.fxX, edi
		add		ecx,DWORD PTR [bf_u_inc]
		mov		edi, [eax]TShadowTrans32.indCoord.w1dV.bfxValue.i4Int
		adc		edx,DWORD PTR [bf_u_inc+4]
		mov		esi, [eax]TShadowTrans32.indCoord.w1dV.bfxValue.u4Frac
		// Add the integer portions together.
		add		edi, w1d_v_inc.bfxValue.i4Int
		// Add the fractional portions together, and add an extra step value if a carry
		// results.
		add		esi, w1d_v_inc.bfxValue.u4Frac
		jnc		short SKIP_STEP_ADD1
		add		edi, [eax]TShadowTrans32.indCoord.w1dV.iOffsetPerLine
SKIP_STEP_ADD1:

		cmp		ebx, 0x00010000
		jl		short XDIFF_LESS
		// do this if greater than or equal

		sub		ebx, 0x00010000

		add		edi, w1dNegDV.bfxValue.i4Int

		add		ecx, DWORD PTR [bfNegDU]
		adc		edx, DWORD PTR [bfNegDU+4]

		add		esi, w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE
		add		edi, [eax]TShadowTrans32.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

XDIFF_LESS:
		cmp		ebx,0
		jge		short EDGE_DONE
		// do this if less than 0

		add		ebx, 0x00010000
		add		edi, w1dNegDV.bfxValue.i4Int
		add		ecx, DWORD PTR [bfNegDU]
		adc		edx, DWORD PTR [bfNegDU+4]
		add		esi, w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE
		add		edi, [eax]TShadowTrans32.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// -------------------------------------------------------------------------
		// store all the working variables back to the original raster
		// and also interleave the co-ordinate update variables..
		mov		[eax]TShadowTrans32.indCoord.w1dV.bfxValue.u4Frac, esi
		mov		esi,pdtriShadow32
		mov		[eax]TShadowTrans32.indCoord.bfU, ecx
		mov		[eax]TShadowTrans32.indCoord.w1dV.bfxValue.i4Int, edi
		mov		[eax]TShadowTrans32.fxXDifference, ebx
		mov		ecx,[esi]TAccess.prasScreen
		mov		[eax]TShadowTrans32.indCoord.bfU+4, edx
		mov		edi,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		mov		ebp,[esi]CDrawPolygonBase.iY
		mov		edx,[ecx]CRasterBase.iLinePixels
		add		[esi]CDrawPolygonBase.fxLineLength.i4Fx, edi
		inc		ebp
		add		[esi]CDrawPolygonBase.iLineStartIndex, edx
		cmp		ebp,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY, ebp
		jl		LIN_NEXT_SCAN_LINE
		pop ebp
	}
}

#endif  // elif (iBUMPMAP_RESOLUTION == 32)
