#pragma once

/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized version of the flat shaded alpha primitive
 *		Optimized for the AMD K6-3D Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/DrawSubTriangleAlpha.cpp                $
 * 
 * 6     98.07.24 9:14p Mmouni
 * Removed bad assert.
 * 
 * 5     98.07.17 6:36p Mmouni
 * Added  new alpha texture primitive.
 * 
 * 4     97.10.30 1:36p Mmouni
 * Optimized inner loop for K6.
 * 
 * 3     97.10.27 1:25p Mmouni
 * Initial K6-3D version, copied from the Pentium versions.
 * 
 * 6     10/10/97 1:47p Mmouni
 * All inner loops are now left to right only, and optimized some.
 * 
 * 5     9/01/97 9:19p Mmouni
 * Applied Pentium optimizations and fixed dword alignment problem.
 * 
 * 4     9/01/97 8:02p Rwyatt
 * 
 * 3     8/17/97 9:43p Rwyatt
 * Copy of Pro version just to get into assembler, this must be optimized later.
 * 
 * 2     8/17/97 12:19a Rwyatt
 * Terrain texturing, added to source safe yet again..
 * 
 * 1     8/17/97 12:18a Rwyatt
 * Alpha blend initial implementation
 * 
 ***********************************************************************************************/

#include "lib/renderer/LightBlend.hpp"


static uint32 u4AlphaColourMask;
static uint32 u4AlphaColourReference;


//*****************************************************************************************
//
// Solid alhpa colour.
//
void DrawSubtriangle(TAlphaColour* pscan, CDrawPolygon<TAlphaColour>* pdtri)
{
	fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
	fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;

	u4AlphaColourMask		= u2AlphaColourMask | (u2AlphaColourMask<<16);
	u4AlphaColourReference	= u2AlphaColourReference | (u2AlphaColourReference<<16);

	_asm
	{
		mov		edx,[pdtri]
		mov		ecx,[pscan]

		mov		eax,[edx]CDrawPolygonBase.iY
		mov		edi,[edx]CDrawPolygonBase.iLineStartIndex
		// ecx, edi, eax and edx are also set from the loop end.

		//--------------------------------------------------------------------------------------
		// scan line start
NEXT_SCAN:
		and		eax,[bEvenScanlinesOnly]
		jnz		short SKIP_LOOP

		mov		esi,[ecx]TAlphaColour.fxX.i4Fx
		mov		eax,[edx]CDrawPolygonBase.fxLineLength.i4Fx

		add		eax,esi

		sar		eax,16

		sar		esi,16
		add		edi,eax

		sub		esi,eax
		jge		short SKIP_LOOP

		//--------------------------------------------------------------------------------------
		// all scan lines are drawn the same way
		add		edi,edi
		mov		eax,gsGlobals.pvScreen

		add		edi,eax
		mov		ecx,[u4AlphaColourMask]

		mov		edx,[u4AlphaColourReference]
		lea		eax,[edi + esi*2]

		test	eax,3
		jz		short N_LOOP

		//--------------------------------------------------------------------------------------
		// do a single WORD to dword align
		movzx	ebx,word ptr[edi + esi*2]
		and		ebx,ecx
		or		ebx,edx
		and		ebx,0x0000ffff
		mov		ax,[lbAlphaConstant.au2Colour + ebx*2]
		mov		[edi + esi*2],ax

		inc		esi						// we are now aligned to a DWORD
		jz		short SKIP_LOOP			// are we done?

		//--------------------------------------------------------------------------------------
		// Main loop does 2 words at a time
N_LOOP:
		add		esi,2
		jge		short DONE_SCAN

		mov		ebx,[edi + esi*2 - 4]		// Read dword
		and		ebx,ecx						// Apply AlphaColourMask
		or		ebx,edx						// Apply AlphaColourRefernce
		mov		eax,ebx						// Copy dword
		and		ebx,0x0000FFFF				// Mask lower 16 bits
		shr		eax,16						// Shift pixel down
		movzx	ecx,word ptr[lbAlphaConstant.au2Colour + ebx*2]
		movzx	ebx,word ptr[lbAlphaConstant.au2Colour + eax*2]
		shl		ebx,16						// Shift pixel up
		or		ebx,ecx						// Merge pixels
		mov		ecx,[u4AlphaColourMask]		// Restore ecx
		mov		[edi + esi*2 - 4],ebx		// Write dword
		jmp		short N_LOOP

DONE_SCAN:
		jz		short DWORD_REMAIN

		//--------------------------------------------------------------------------------------
		// do a single word to finish
		movzx	ebx,word ptr[edi + esi*2 - 4]
		and		ebx,ecx
		or		ebx,edx
		and		ebx,0x0000ffff
		mov		ax,[lbAlphaConstant.au2Colour + ebx*2]
		mov		[edi + esi*2 - 4],ax

		jmp		short SKIP_LOOP


		//--------------------------------------------------------------------------------------
		// do a DWORD to finish
DWORD_REMAIN:
		mov		ebx,[edi + esi*2 - 4]		// Read dword
		and		ebx,ecx						// Apply AlphaColourMask
		or		ebx,edx						// Apply AlphaColourRefernce
		mov		eax,ebx						// Copy dword
		and		ebx,0x0000FFFF				// Mask lower 16 bits
		shr		eax,16						// Shift pixel down
		movzx	ecx,word ptr[lbAlphaConstant.au2Colour + ebx*2]
		movzx	ebx,word ptr[lbAlphaConstant.au2Colour + eax*2]
		shl		ebx,16						// Shift pixel up
		or		ebx,ecx						// Merge pixels
		mov		[edi + esi*2 - 4],ebx		// Write dword


		//--------------------------------------------------------------------------------------
		// Loop control an sub pixel correction for edges
SKIP_LOOP:
		mov		edx,[pdtri]
		mov		ecx,[pscan]
		mov		eax,[fx_inc]
		mov		esi,[ecx]TAlphaColour.fxXDifference
		add		[ecx]TAlphaColour.fxX,eax
		add		esi,[fx_diff]

		cmp		esi,(1<<16)
		jz		short UFLOW

		// greater than or equal to 1 is an overflow
		sub		esi,(1<<16)
		jmp		short DONE_Y
UFLOW:
		cmp		esi,0
		jge		short DONE_Y
		add		esi,(1<<16)
DONE_Y:

		// calc the new end point and line length
		mov		[ecx]TAlphaColour.fxXDifference,esi
		mov		eax,[edx]CDrawPolygonBase.fxLineLength.i4Fx
		mov		edi,[edx]CDrawPolygonBase.iLineStartIndex
		add		eax,[edx]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		edi,gsGlobals.u4LinePixels
		mov     [edx]CDrawPolygonBase.fxLineLength.i4Fx, eax


		mov		eax,[edx]CDrawPolygonBase.iY
		mov		esi,[edx]CDrawPolygonBase.iYTo
		inc		eax
		mov     [edx]CDrawPolygonBase.iLineStartIndex, edi
		mov		[edx]CDrawPolygonBase.iY,eax
		cmp		eax,esi
		jl		short NEXT_SCAN
	}
}


//*****************************************************************************************
//
// Perspective, stipple alpha textured, 16 bit mode.
//
void DrawSubtriangle(TAlphaTexPersp* pscan, CDrawPolygon<TAlphaTexPersp>* pdtri)
{
	TAlphaTexPersp* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

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
		mov		ebx,[eax]TAlphaTexPersp.fxX

		mov		edx,[eax]TAlphaTexPersp.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TAlphaTexPersp.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TAlphaTexPersp.indCoord.fInvZ
		movq	[tvEdgeStep.UInvZ],mm0

		mov		[tvEdgeStep.InvZ],ebx

		//
		// Setup:
		// QWORD qMFactor = width,1,width,1
		// Load global texture values. 
		//
		movd		mm3,[iTexWidth]								// 0,0,0,Width

		movd		mm4,[u4OneOne]								// 0,0,1,1

		punpcklwd	mm3,mm3										// 0,0,Width,Width

		punpcklwd	mm4,mm3										// Width,1,Width,1
		movq		mm0,[ecx]TAlphaTexPersp.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4								// Save mfactor
		movd		mm1,[ecx]TAlphaTexPersp.indCoord.fInvZ	// mm1 = (?,1/Z)

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// esi,ecx,mm0,mm1 setup. 
		//
Y_LOOP:
		mov		eax,[esi]CDrawPolygonBase.iY

		mov		ebx,[bEvenScanlinesOnly]

		// Check to see if we sould skip this scanline.
		and		eax,ebx
		jnz		END_OF_SCANLINE

		//
		// Setup for prespective correction and looping.
		//
		//PERSPECTIVE_SCANLINE_PROLOGUE_CLAMP_3DX(TAlphaTexPersp)
		//
		// Determine the start and end of the scanline. 
		//
		mov		ebx,[ecx]TAlphaTexPersp.fxX.i4Fx

		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,eax

		mov		eax,[ecx]TAlphaTexPersp.fxX.i4Fx
		sar		ebx,16

		sar		eax,16
		add		edx,ebx

		sub		eax,ebx									// eax == i_pixel
		jge		END_OF_SCANLINE

		mov		[i_screen_index],edx					// Save scanline index.
		mov		ebx,[ecx]TAlphaTexPersp.fxX.i4Fx		// Fractional X.

		sar		ebx,16									// Integer X.
		mov		edi,[esi]CDrawPolygonBase.iY			// Integer Y.

		// Load span increments.
		movq	mm2,[tvDEdge.UInvZ]						// Load V,U
		xor		ebx,edi									// X xor Y
	
		movd	mm3,[tvDEdge.InvZ]						// Load Z
		mov		[i_pixel],eax							// Save negative length of scanline.

		mov		edi,[iSubdivideLen]

		// Skip over a pixel if ((y xor x) & 1).
		test	ebx,1
		jz		NO_SKIP

		inc		eax										// Increment i_pixel
		jz		END_OF_SCANLINE

		// Increment starting texture values.
		movd		mm5,[fInvSubdivideLen]				// 1/SubDivideLen

		movq		mm4,mm5								// Save a copy.
		pfmul		(m5,m3)								// Per pixel z step.

		punpckldq	mm4,mm4								// 1/SubDivideLen | 1/SubDivideLen
		pfadd		(m1,m5)								// Add to starting z.

		pfmul		(m4,m2)								// Per pixel uv step.

		pfadd		(m0,m4)								// Add to staring uv.
		mov			[i_pixel],eax						// Save update i_pixel

NO_SKIP:
		// scan line is +ve 
		add		eax,edi									// eax = i_pixel + SubdivideLen
		jle		short DONE_DIVIDE_PIXEL

		//
		// Subdivision is smaller than iSubdivideLen 
		//
		// Adjust span increments by -i_pixel * fInvSubdivideLen 
		//
		mov		edi,[i_pixel]					// Integer width of span.
		xor		eax,eax							// Remaining width.

		neg		edi
		movd	mm4,edi							// -i_pixel

		pi2fd	(m4,m4)							// Convert to fp.
		movd	mm5,[fInvSubdivideLen]			// 1/SubDivideLen

		pfmul	(m4,m5)							// -i_pixel * fInvSubdivideLen 

		punpckldq	mm4,mm4

		pfmul	(m2,m4)							// Adjust V,U increments.

		pfmul	(m3,m4)							// Adjust Z increment.

DONE_DIVIDE_PIXEL:
		//
		// Compute current U,V
		// Step fGUInvZ,fGVInvZ,fGInvZ
		//
		pfrcp	(m4,m1)							// f_z = mm4 = 1/fGInvZ
		mov		[iNextSubdivide],eax			// Save length remaining.

		pfadd	(m3,m1)							// Step fGInvZ
		mov		edx,[bClampUV]					/* Load clamp flag. */

		pfadd	(m2,m0)							// Step fGUInvZ,fGVInvZ
		movd	[tvCurUVZ.InvZ],mm3				// Save updated 1/Z

		pfmul	(m4,m0)							// mm4 = V1,U1
		pfrcp	(m0,m3)							// f_next_z = mm0 = 1/fGInvZ

		movq	[tvCurUVZ.UInvZ],mm2					// Save updated U/Z,V/Z
		movd	mm1,float ptr[fInverseIntTable+edi*4]	// Reciprocal of span width.

		pfmul	(m0,m2)							// mm0 = V2,U2
		movq	mm2,[pfFixed16Scale]			// Load fixed point scale factors.

		cmp		edx,0
		je		CLAMP_DONE

		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
		pfmax	(m4,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m4,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight

CLAMP_DONE:
		punpckldq	mm1,mm1						// Duplicate reciprocal of span width.
		movq		[pfCurUV],mm0				// Save ending U,V

		pfsub	(m0,m4)							// mm0 = V2-V1,U2-U1
		pfmul	(m4,m2)							// Scale to starting U,V 16.16

		mov		edi,[i_pixel]					// edi = current i_pixel
		mov		[i_pixel],eax					// Save updated i_pixel.

		pfmul	(m0,m1)							// DV*1/Width,DU*1/Width
		pf2id	(m7,m4)							// Starting V,U in mm7

		sub		edi,eax							// edi = inner loop count
		mov		ebx,[i_screen_index]			// Load scanline offset.

		pfmul	(m0,m2)							// Scale U,V slope to 16.16
		mov		edx,gsGlobals.pvScreen			// Pointer the screen.

		mov		esi,[pvTextureBitmap]			// Load texture base pointer.
		add		eax,ebx							// Add scanline offset to i_pixel

		pf2id	(m6,m0)							// VStep,UStep in mm6
		lea		edx,[edx+eax*2]					// Base of span in edx.

SUBDIVISION_LOOP:
		//--------------------------------------------------------------------------- 
		// Draw the subdivision.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index2
		// ecx = clut_base_ptr		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		movq		mm0,mm7							// Copy U1,V1
		paddd		mm6,mm6							// Double Step.

		paddd		mm7,mm6							// Step U,V by 2
		psrad		mm0,16							// Shift for integer U1,V1

		movq		mm1,mm7							// Copy U2,V2
		movq		mm2,[qMFactor]					// Load texture multipliers

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values

		dec			esi								// Hack to force SIB + offset in loop
		pmaddwd		mm0,mm2							// Compute texture indicies.

		mov			ecx,[au2AlphaTable]				// Conversion table.

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,4
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to ? past the beginning of a cache line.
		ALIGN	16

		//
		// Do two pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V by 2

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1.
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1.
		paddd		mm7,mm6							// Step U,V by 2

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2.
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		pmaddwd		mm0,mm2							// Compute texture indicies.

		cmp			eax,0x00004000
		jle			PIXEL1_TRANS

		and			eax,0x00000fff					// Mask colors.

		mov			ax,[ecx + eax*2]				// Color conversion.

		mov			[edx + edi*2 - 8],ax			// Store pixel 1

PIXEL1_TRANS:
		cmp			ebx,0x00004000
		jle			PIXEL2_TRANS

		and			ebx,0x00000fff					// Mask colors.

		mov			bx,[ecx + ebx*2]				// Color conversion.

		mov			[edx + edi*2 - 4],bx			// Store pixel 2

PIXEL2_TRANS:
		add			edi,4							// Increment index by 4
		jl			short INNER_LOOP_2P				// Loop (while >4 pixels)

FINISH_REMAINDER:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		cmp			edi,2
		jge			ONE_OR_TWO_PIXELS_LEFT

		//
		// Draw two pixels, step uv by 2.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V by 2

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1.

		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2.

		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1.

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2.

		cmp			eax,0x00004000
		jle			PIXEL1_TRANS_END

		and			eax,0x00000fff					// Mask colors.

		mov			ax,[ecx + eax*2]				// Color conversion.

		mov			[edx + edi*2 - 8],ax			// Store pixel 1

PIXEL1_TRANS_END:
		cmp			ebx,0x00004000
		jle			PIXEL2_TRANS_END

		and			ebx,0x00000fff					// Mask colors.

		mov			bx,[ecx + ebx*2]				// Color conversion.

		mov			[edx + edi*2 - 4],bx			// Store pixel 2

PIXEL2_TRANS_END:
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_OR_TWO_PIXELS_LEFT:
		//
		// Draw one pixel, don't step uv.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,[i_pixel]					// Load remaining length.

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1.

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 1.

		cmp			ebx,0x00004000
		jle			END_OF_SPAN

		and			ebx,0x00000fff					// Mask colors.

		mov			bx,[ecx + ebx*2]				// Color conversion.

		mov			[edx + edi*2 - 8],bx			// Store pixel 1

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		jz		short END_OF_SCANLINE

		//
		// Do perspective correction and looping.
		//
		//PERSPECTIVE_SCANLINE_EPILOGUE_CLAMP_3DX
		mov		edi,[iSubdivideLen]

		// Load last texture values.
		movq	mm0,[tvCurUVZ.UInvZ]						// mm0 = (V/Z,U/Z)

		movd	mm1,[tvCurUVZ.InvZ]							// mm1 = (?,1/Z)

		// Load span increments.
		movq	mm2,[tvDEdge.UInvZ]							// V,U
	
		movd	mm3,[tvDEdge.InvZ]							// Z

		// scan line is +ve 
		add		eax,edi										// eax = i_pixel + SubdivideLen
		jle		short DONE_DIVIDE_PIXEL_END

		//
		// Subdivision is smaller than iSubdivideLen 
		//
		// Adjust span increments by -i_pixel * fInvSubdivideLen 
		//
		mov		edi,[i_pixel]					// Integer width of span.
		xor		eax,eax							// Remaining width.

		neg		edi
		movd	mm4,edi							// -i_pixel

		pi2fd	(m4,m4)							// Convert to fp.
		movd	mm5,[fInvSubdivideLen]			// 1/SubDivideLen

		pfmul	(m4,m5)							// -i_pixel * fInvSubdivideLen 

		punpckldq	mm4,mm4

		pfmul	(m2,m4)							// Adjust V,U increments.

		pfmul	(m3,m4)							// Adjust Z increment.

DONE_DIVIDE_PIXEL_END:
		//
		// Compute current U,V
		// Step fGUInvZ,fGVInvZ,fGInvZ
		//
		pfadd	(m3,m1)							// Step fGInvZ
		mov		[iNextSubdivide],eax			// Save length remaining.

		pfadd	(m2,m0)							// Step fGUInvZ,fGVInvZ
		movd	[tvCurUVZ.InvZ],mm3				// Save updated fGInvZ

		pfrcp	(m0,m3)							// f_z = mm0 = 1/fGInvZ
		movq	mm4,[pfCurUV]					// Load last U,V

		movq	[tvCurUVZ.UInvZ],mm2					// Save updated fGUInvZ,fGVInvZ
		movd	mm1,float ptr[fInverseIntTable+edi*4]	// Reciprocal of span width.

		pfmul	(m0,m2)							// mm0 = V2,U2
		movq	mm2,[pfFixed16Scale]			// Load fixed point scale factors.

		cmp		[bClampUV],0
		je		CLAMP_DONE_END

		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight

CLAMP_DONE_END:
		movq		[pfCurUV],mm0				// Save current U,V
		punpckldq	mm1,mm1						// Duplicate across entire register.

		pfsub	(m0,m4)							// V2-V1,U2-U1
		mov		edi,[i_pixel]					// edi = current i_pixel

		mov		[i_pixel],eax					// Save updated i_pixel.
		mov		edx,gsGlobals.pvScreen			// Pointer the screen.

		pfmul	(m0,m1)							// DV*1/Width,DU*1/Width
		mov		ebx,[i_screen_index]			// Load scanline offset.

		sub		edi,eax							// edi = inner loop count
		mov		esi,[pvTextureBitmap]			// Load texture base pointer.

		pfmul	(m0,m2)							// Scale to 16.16
		add		eax,ebx							// Add scanline offset to i_pixel

		lea		edx,[edx+eax*2]					// Base of span in edx.
		pf2id	(m6,m0)							// VStep,UStep in mm6

		jmp		SUBDIVISION_LOOP

END_OF_SCANLINE:
		//		
		// Increment the base edge.
		//
		mov		esi,[pdtri]						// Pointer to polygon object.

		mov		ecx,[pscan]						// Pointer to scanline object.

		// Step length, line starting address.
		mov		edx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,[esi]CDrawPolygonBase.iLineStartIndex

		add		edx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx

		add		edi,gsGlobals.u4LinePixels

		mov		eax,[ecx]TAlphaTexPersp.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TAlphaTexPersp.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TAlphaTexPersp.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TAlphaTexPersp.indCoord.fInvZ

		movd	mm3,[tvEdgeStep.InvZ]
		pfadd	(m0,m2)

		mov		edi,[fx_diff]
		pfadd	(m1,m3)

		add		eax,[fx_inc]

		add		ebx,edi
		jge		short NO_UNDERFLOW

		// Underflow.
		// Add borrow/carry adjustments to U,V,Z.
		movq	mm2,[tvNegAdj.UInvZ]
		add		ebx,0x00010000

		movd	mm3,[tvNegAdj.InvZ]

		pfadd	(m0,m2)

		pfadd	(m1,m3)
		jmp		short FINISH_LOOPING

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short FINISH_LOOPING

		// Overflow.
		// Add borrow/carry adjustments to U,V,Z.
		movq	mm2,[tvNegAdj.UInvZ]
		sub		ebx,0x00010000

		movd	mm3,[tvNegAdj.InvZ]

		pfadd	(m0,m2)

		pfadd	(m1,m3)

FINISH_LOOPING:
		// Save values and Y Loop control.
		movq	[ecx]TAlphaTexPersp.indCoord.fUInvZ,mm0

		movd	[ecx]TAlphaTexPersp.indCoord.fInvZ,mm1

		mov		[ecx]TAlphaTexPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TAlphaTexPersp.fxXDifference.i4Fx,ebx
		mov		ebx,[esi]CDrawPolygonBase.iYTo

		inc		edx
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}


//*****************************************************************************************
//
// Linear, stipple alpha textured, 16 bit mode.
//
void DrawSubtriangle(TAlphaTexLinear* pscan, CDrawPolygon<TAlphaTexLinear>* pdtri)
{
	typedef TAlphaTexLinear::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TAlphaTexLinear>* pdtriGlbl;
	static TAlphaTexLinear* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TAlphaTexLinear* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[edi]TAlphaTexLinear.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TAlphaTexLinear.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TAlphaTexLinear.indCoord.pfIndex
		mov		fx_diff,ecx

		movq	[pf_texture_inc],mm0

		//
		// Setup:
		//
		// mm2 = qMFactor = width,1,width,1
		// mm3 = qUVMasks
		// mm6 = uvslope
		//
		movd		mm1,[iTexWidth]					// 0,0,0,Width

		movd		mm2,[u4OneOne]					// 0,0,1,1
		punpcklwd	mm1,mm1							// 0,0,Width,Width

		movq		mm6,[pfxDeltaTex]				// Load texture slopes.
		punpcklwd	mm2,mm1							// Width,1,Width,1

		movq		mm7,[eax]TAlphaTexLinear.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TAlphaTexLinear.fxX.i4Fx

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
		// eax = texel_index1		ebx = texel_index2
		// ecx = clut_base_ptr		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm5 = uvslope*2
		// mm6 = uvslope			mm7 = uv
		//
		pf2id		(m7,m7)								// Convert texture values to 16.16 fixed.
		mov			ebx,[eax]TAlphaTexLinear.fxX.i4Fx	// Fractional X.

		movq		mm5,mm6								// Copy step.
		mov			ecx,[esi]CDrawPolygonBase.iY		// Integer Y.

		paddd		mm5,mm5								// Double step.
		sar			ebx,16								// Integer X.

		// if ((y xor x) & 1) skip a pixel.
		xor			ebx,ecx

		and			ebx,1
		jz			NO_SKIP

		paddd		mm7,mm6								// Step U,V
		inc			edi

		jz			END_OF_SCANLINE

NO_SKIP:
		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm5							// Step U,V by 2

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		mov			esi,[pvTextureBitmap]			// Load texture base pointer.

		packssdw	mm0,mm1							// Pack integer texture values
		dec			esi								// Hack to force SIB + offset in loop

		pmaddwd		mm0,mm2							// Compute texture indicies.
		mov			ecx,[au2AlphaTable]				// Conversion table.

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,4
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to ? past the beginning of a cache line.
		ALIGN	16

		//
		// Do two pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm5							// Step U,V by 2

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1.
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1.
		paddd		mm7,mm5							// Step U,V by 2

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2.
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		pmaddwd		mm0,mm2							// Compute texture indicies.

		cmp			eax,0x00004000
		jle			PIXEL1_TRANS

		and			eax,0x00000fff					// Mask colors.

		mov			ax,[ecx + eax*2]				// Color conversion.

		mov			[edx + edi*2 - 8],ax			// Store pixel 1

PIXEL1_TRANS:
		cmp			ebx,0x00004000
		jle			PIXEL2_TRANS

		and			ebx,0x00000fff					// Mask colors.

		mov			bx,[ecx + ebx*2]				// Color conversion.

		mov			[edx + edi*2 - 4],bx			// Store pixel 2

PIXEL2_TRANS:
		add			edi,4							// Increment index by 4
		jl			short INNER_LOOP_2P				// Loop (while >4 pixels)

FINISH_REMAINDER:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		cmp			edi,2
		jge			ONE_OR_TWO_PIXELS_LEFT

		//
		// Draw two pixels.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1.

		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2.

		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1.

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2.

		cmp			eax,0x00004000
		jle			PIXEL1_TRANS_END

		and			eax,0x00000fff					// Mask colors.

		mov			ax,[ecx + eax*2]				// Color conversion.

		mov			[edx + edi*2 - 8],ax			// Store pixel 1

PIXEL1_TRANS_END:
		cmp			ebx,0x00004000
		jle			END_OF_SCANLINE

		and			ebx,0x00000fff					// Mask colors.

		mov			bx,[ecx + ebx*2]				// Color conversion.

		mov			[edx + edi*2 - 4],bx			// Store pixel 2

		jmp			short END_OF_SCANLINE

ONE_OR_TWO_PIXELS_LEFT:
		//
		// Draw one pixel.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1.

		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1.

		cmp			eax,0x00004000
		jle			END_OF_SCANLINE

		and			eax,0x00000fff					// Mask colors.

		mov			ax,[ecx + eax*2]				// Color conversion.

		mov			[edx + edi*2 - 8],ax			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TAlphaTexLinear.indCoord.pfIndex

		mov		ebx,[eax]TAlphaTexLinear.fxXDifference

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
		mov		[eax]TAlphaTexLinear.fxXDifference,ebx

		movq	[eax]TAlphaTexLinear.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TAlphaTexLinear.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TAlphaTexLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}
