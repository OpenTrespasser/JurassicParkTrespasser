#pragma once

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
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/DrawSubTriangleTexEx.cpp                   $
 * 
 * 7     10/01/98 12:41a Asouth
 * Added #ifdefs for MW build; explicit scope of fixed
 * 
 * 6     3/12/98 7:02p Mmouni
 * Solid primitives now write aligned quadwords.
 * 
 * 5     97.11.04 10:59p Mmouni
 * Linear version of primitives now use 3dx/mmx.
 * 
 * 4     97.10.30 1:40p Mmouni
 * All perspective sub-triangle routines are now 3DX/MMX.
 * 
 * 3     97.10.27 1:25p Mmouni
 * Initial K6-3D version, copied from the Pentium versions.
 * 
 * 9     97.10.22 7:11p Mmouni
 * Moved code that makes copies of edge values into assembly.
 * 
 * 8     10/10/97 1:47p Mmouni
 * All inner loops are now left to right only, and optimized some.
 * 
 * 7     9/30/97 9:33p Mmouni
 * All routines now have optimized y-loops.
 * 
 * 6     9/29/97 11:51a Mmouni
 * Now has seperate left to right and right to left perspective loops.
 * 
 * 5     9/15/97 2:10p Mmouni
 * Optimized inner loops (not final yet).
 * 
 * 4     9/01/97 8:03p Rwyatt
 * ANDs added to enable tiling
 * 
 * 3     97/07/18 3:30p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 2     7/14/97 12:36p Rwyatt
 * Checkled in so Scott can move them....
 * 
 * 1     7/07/97 11:26p Rwyatt
 * Initial Implementation of Pentium Specifics
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

#include "Common.hpp"

#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"


// Put this stuff in a seperate section for alignment.
#pragma code_seg("StriTex")


//*****************************************************************************************
//
// Perspective texture mapped with transparency for 16 bit mode.
//
void DrawSubtriangle(TTexturePerspTrans* pscan, CDrawPolygon<TTexturePerspTrans>* pdtri)
{
	typedef TTexturePerspTrans::TIndex prevent_internal_compiler_errors;
	::fixed fx_inc;
	::fixed fx_diff;

	TTexturePerspTrans* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[eax]TTexturePerspTrans.fxX

		mov		edx,[eax]TTexturePerspTrans.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TTexturePerspTrans.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TTexturePerspTrans.indCoord.fInvZ
		movq	[tvEdgeStep.UInvZ],mm0

		mov		[tvEdgeStep.InvZ],ebx

		//
		// Setup:
		// QWORD qMFactor		= width,1,width,1
		// Load global texture values. 
		//
		movd		mm3,[iTexWidth]					// 0,0,0,Width

		movd		mm4,[u4OneOne]					// 0,0,1,1
		punpcklwd	mm3,mm3							// 0,0,Width,Width

		punpcklwd	mm4,mm3										// Width,1,Width,1
		movq		mm0,[ecx]TTexturePerspTrans.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4								// Save mfactor
		movd		mm1,[ecx]TTexturePerspTrans.indCoord.fInvZ	// mm1 = (?,1/Z)

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
	#ifdef __MWERKS__
		#define SLTYPE TTexturePerspTrans
		#include "psprologue3.inl"
	#else
		PERSPECTIVE_SCANLINE_PROLOGUE_3DX(TTexturePerspTrans)
	#endif

SUBDIVISION_LOOP:
		//--------------------------------------------------------------------------- 
		// Draw the subdivision.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index2
		// ecx = clut_base_pointer	edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm4 = ???				mm5 = ???
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm6							// Step U,V

		movq		mm3,[qUVMasks]					// Load texture masks
		psrad		mm0,16							// Shift for integer U1,V1

		movq		mm2,[qMFactor]					// Load texture multipliers
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		mov			ecx,[pvClutConversion]			// Load clut pointer.

		packssdw	mm0,mm1							// Pack integer texture values
		sub			esi,4							// Hack to force SIB + offset in loop

		pand		mm0,mm3							// Mask for tiling
		sub			ecx,4							// Hack to force SIB + offset in loop

		pmaddwd		mm0,mm2							// Compute texture indicies.

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 1? past the beginning of a cache line.
		ALIGN	16
		nop

		//
		// Do two pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		test		eax,eax							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT

		mov			ax,[ecx + eax*2 + 4]			// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT:
		pand		mm0,mm3							// Mask for tiling
		test		ebx,ebx							// Test for transparency.

		pmaddwd		mm0,mm2							// Compute texture indicies.
		jz			short PIXEL2_TRANSPARENT

		mov			bx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p2
		mov			[edx + edi*2 - 2],bx			// Store pixel 2

PIXEL2_TRANSPARENT:
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
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 2

		test		eax,eax							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT_END

		mov			ax,[ecx + eax*2 + 4]			// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT_END:
		test		ebx,ebx							// Test for transparency.
		jz			short PIXEL2_TRANSPARENT_END

		mov			bx,[ecx + ebx*2 + 4]			// Clut lookup for p2
		mov			[edx + edi*2 - 2],bx			// Store pixel 2

PIXEL2_TRANSPARENT_END:
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		mov			eax,[i_pixel]					// Load remaining length.

		test		ebx,ebx							// Test for transparency.
		jz			short END_OF_SPAN

		mov			bx,[ecx + ebx*2 + 4]			// Clut lookup for p1
		mov			[edx + edi*2 - 4],bx			// Store pixel 1

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		jz		short END_OF_SCANLINE

		//
		// Do perspective correction for next span.
	#ifdef __MWERKS__
		#include "psepilogue3.inl"
	#else
		PERSPECTIVE_SCANLINE_EPILOGUE_3DX
	#endif

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

		mov		eax,[ecx]TTexturePerspTrans.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TTexturePerspTrans.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TTexturePerspTrans.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TTexturePerspTrans.indCoord.fInvZ

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
		movq	[ecx]TTexturePerspTrans.indCoord.fUInvZ,mm0

		movd	[ecx]TTexturePerspTrans.indCoord.fInvZ,mm1

		mov		[ecx]TTexturePerspTrans.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TTexturePerspTrans.fxXDifference.i4Fx,ebx
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
// Perspective texture mapped with no transparency for 16 bit mode.
//
void DrawSubtriangle(TTexturePersp* pscan, CDrawPolygon<TTexturePersp>* pdtri)
{
	typedef TTexturePersp::TIndex prevent_internal_compiler_errors;
	::fixed fx_inc;
	::fixed fx_diff;

	TTexturePersp* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[eax]TTexturePersp.fxX

		mov		edx,[eax]TTexturePersp.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TTexturePersp.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TTexturePersp.indCoord.fInvZ
		movq	[tvEdgeStep.UInvZ],mm0

		mov		[tvEdgeStep.InvZ],ebx

		//
		// Setup:
		// QWORD qMFactor		= width,1,width,1
		// Load global texture values. 
		//
		movd		mm3,[iTexWidth]					// 0,0,0,Width

		movd		mm4,[u4OneOne]					// 0,0,1,1
		punpcklwd	mm3,mm3							// 0,0,Width,Width

		punpcklwd	mm4,mm3										// Width,1,Width,1
		movq		mm0,[ecx]TTexturePersp.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4								// Save mfactor
		movd		mm1,[ecx]TTexturePersp.indCoord.fInvZ	// mm1 = (?,1/Z)

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
	#ifdef __MWERKS__
		#define SLTYPE TTexturePersp
		#include "psprologue3.inl"
	#else
		PERSPECTIVE_SCANLINE_PROLOGUE_3DX(TTexturePersp)
	#endif

SUBDIVISION_LOOP:
		//--------------------------------------------------------------------------- 
		// Draw the subdivision.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index2
		// ecx = clut_base_pointer	edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm4 = ???				mm5 = ???
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm6							// Step U,V

		movq		mm3,[qUVMasks]					// Load texture masks
		psrad		mm0,16							// Shift for integer U1,V1

		movq		mm2,[qMFactor]					// Load texture multipliers
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		mov			ecx,[pvClutConversion]			// Load clut pointer.

		packssdw	mm0,mm1							// Pack integer texture values
		sub			esi,4							// Hack to force SIB + offset in loop

		pand		mm0,mm3							// Mask for tiling
		sub			ecx,4							// Hack to force SIB + offset in loop

		pmaddwd		mm0,mm2							// Compute texture indicies.

		//
		// Align to a quadword boundry.
		//
		lea			eax,[edx + edi*2]

		and			eax,3
		jz			ALIGNED2

		inc			edi								// One pixel left?
		jz			ONE_PIXEL_LEFT

		//
		// Do one pixel for alignment.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		psrad		mm0,16							// Shift for integer U1,V1
		packssdw	mm1,mm0							// Pack integer texture values.

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm0,mm1							// Put values back into mm0

		pand		mm0,mm3							// Mask for tiling
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		lea			eax,[edx + edi*2]

		mov			[edx + edi*2 - 2],bx			// Store pixel

ALIGNED2:
		and			eax,7
		jz			ALIGNED4

		add			edi,2							// Two pixels left?
		jge			FINISH_REMAINDER2

		//
		// Do two pixels for alignment.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 2
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx + edi*2 - 4],ebx			// Store pixels

ALIGNED4:
		add		edi,4		
		jl		short INNER_LOOP_4P					// Four pixles left?

		jmp		FINISH_REMAINDER4

		// Align start of loop to 0 past the beginning of a cache line.
		ALIGN	16

		//
		// Do four pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_4P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift second pixel up.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm4,ebx							// Put in MMX register.

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		movq		mm1,mm7							// Copy U2,V2
		psrad		mm1,16							// Shift for integer U2,V2

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		shl			eax,16							// Shift second pixel up.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm1,ebx							// Put in MMX register.

		punpckldq	mm4,mm1							// Combine all four pixels.
		movq		[edx + edi*2 - 8],mm4			// Store 4 pixels

		add			edi,4							// Inrcement index by 4
		jl			INNER_LOOP_4P					// Loop (while >4 pixels)

FINISH_REMAINDER4:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		jnz			THREE_OR_LESS

		//
		// Four pixels left, step co-ordinates 3 times.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift second pixel up.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm4,ebx							// Put in MMX register.

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1

		shl			eax,16							// Shift second pixel up.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm1,ebx							// Put in MMX register.

		punpckldq	mm4,mm1							// Combine all four pixels.
		movq		[edx - 8],mm4					// Store 4 pixels

		mov			eax,[i_pixel]					// Load remaining length.
		jmp			END_OF_SPAN


THREE_OR_LESS:
		sub			edi,2
		jge			FINISH_REMAINDER2

		//
		// Three pixels left, step co-ordinates twice.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 2
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx - 6],ebx					// Store pixels

		movd		ebx,mm0							// Get texture index 1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		mov			eax,[i_pixel]					// Load remaining length.

		mov			bx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p1

		mov			[edx - 2],bx					// Store pixel 1

		jmp			short END_OF_SPAN


FINISH_REMAINDER2:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture co-ordinates once.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx - 4],ebx					// Store pixels
		mov			eax,[i_pixel]					// Load remaining length.

		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movd		ebx,mm0							// Get texture index 1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		mov			eax,[i_pixel]					// Load remaining length.

		mov			bx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p1

		mov			[edx - 2],bx					// Store pixel 1

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		jz		short END_OF_SCANLINE

		//
		// Do perspective correction for next span.
	#ifdef __MWERKS__
		#include "psepilogue3.inl"
	#else
		PERSPECTIVE_SCANLINE_EPILOGUE_3DX
	#endif

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

		mov		eax,[ecx]TTexturePersp.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TTexturePersp.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TTexturePersp.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TTexturePersp.indCoord.fInvZ

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
		movq	[ecx]TTexturePersp.indCoord.fUInvZ,mm0

		movd	[ecx]TTexturePersp.indCoord.fInvZ,mm1

		mov		[ecx]TTexturePersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TTexturePersp.fxXDifference.i4Fx,ebx
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
// Linear texture mapped with transparency in 16 bit mode.
//
void DrawSubtriangle(TTextureLinearTrans* pscan, CDrawPolygon<TTextureLinearTrans>* pdtri)
{
	typedef TTextureLinearTrans::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TTextureLinearTrans>* pdtriGlbl;
	static TTextureLinearTrans* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TTextureLinearTrans* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[edi]TTextureLinearTrans.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TTextureLinearTrans.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TTextureLinearTrans.indCoord.pfIndex
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

		movq		mm7,[eax]TTextureLinearTrans.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TTextureLinearTrans.fxX.i4Fx

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
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index2
		// ecx = clut_base_pointer	edx = dest_base_ptr
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
		mov			ecx,[pvClutConversion]			// Load clut pointer.

		packssdw	mm0,mm1							// Pack integer texture values
		sub			esi,4							// Hack to force SIB + offset in loop

		pand		mm0,mm3							// Mask for tiling
		pmaddwd		mm0,mm2							// Compute texture indicies.

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 1 (or 17) past the beginning of a cache line.
		ALIGN	16
		nop

		//
		// Do two pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		test		eax,eax							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT

		mov			ax,[ecx + eax*2]				// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT:
		pand		mm0,mm3							// Mask for tiling
		test		ebx,ebx							// Test for transparency.

		pmaddwd		mm0,mm2							// Compute texture indicies.
		jz			short PIXEL2_TRANSPARENT

		mov			bx,word ptr[ecx + ebx*2]		// Clut lookup for p2
		mov			[edx + edi*2 - 2],bx			// Store pixel 2

PIXEL2_TRANSPARENT:
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
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 1
		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 2

		test		eax,eax							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT_END

		mov			ax,[ecx + eax*2]				// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT_END:
		test		ebx,ebx							// Test for transparency.
		jz			short END_OF_SCANLINE

		mov			bx,[ecx + ebx*2]				// Clut lookup for p2
		mov			[edx + edi*2 - 2],bx			// Store pixel 2

		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		test		ebx,ebx							// Test for transparency.
		jz			short END_OF_SCANLINE

		mov			bx,[ecx + ebx*2]				// Clut lookup for p1
		mov			[edx + edi*2 - 4],bx			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TTextureLinearTrans.indCoord.pfIndex

		mov		ebx,[eax]TTextureLinearTrans.fxXDifference

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
		mov		[eax]TTextureLinearTrans.fxXDifference,ebx

		movq	[eax]TTextureLinearTrans.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TTextureLinearTrans.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TTextureLinearTrans.fxX,ecx
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
// Linear texture mapped with no transparency in 16 bit mode.
//
void DrawSubtriangle(TTextureLinear* pscan, CDrawPolygon<TTextureLinear>* pdtri)
{
	typedef TTextureLinear::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TTextureLinear>* pdtriGlbl;
	static TTextureLinear* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TTextureLinear* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[edi]TTextureLinear.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TTextureLinear.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TTextureLinear.indCoord.pfIndex
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

		movq		mm7,[eax]TTextureLinear.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TTextureLinear.fxX.i4Fx

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
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel_index2		ebx = texel_index2
		// ecx = clut_base_pointer	edx = dest_base_ptr
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
		mov			ecx,[pvClutConversion]			// Load clut pointer.

		packssdw	mm0,mm1							// Pack integer texture values
		sub			esi,4							// Hack to force SIB + offset in loop

		pand		mm0,mm3							// Mask for tiling
		sub			ecx,4							// Hack to force SIB + offset in loop

		pmaddwd		mm0,mm2							// Compute texture indicies.

		//
		// Align to a quadword boundry.
		//
		lea			eax,[edx + edi*2]

		and			eax,3
		jz			ALIGNED2

		inc			edi								// One pixel left?
		jz			ONE_PIXEL_LEFT

		//
		// Do one pixel for alignment.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		psrad		mm0,16							// Shift for integer U1,V1
		packssdw	mm1,mm0							// Pack integer texture values.

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm0,mm1							// Put values back into mm0

		pand		mm0,mm3							// Mask for tiling
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		lea			eax,[edx + edi*2]

		mov			[edx + edi*2 - 2],bx			// Store pixel

ALIGNED2:
		and			eax,7
		jz			ALIGNED4

		add			edi,2							// Two pixels left?
		jge			FINISH_REMAINDER2

		//
		// Do two pixels for alignment.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 2
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx + edi*2 - 4],ebx			// Store pixels

ALIGNED4:
		add		edi,4		
		jl		short INNER_LOOP_4P					// Four pixles left?

		jmp		FINISH_REMAINDER4

		// Align start of loop to 0 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16

		//
		// Do four pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_4P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift second pixel up.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm4,ebx							// Put in MMX register.

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		movq		mm1,mm7							// Copy U2,V2
		psrad		mm1,16							// Shift for integer U2,V2

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		shl			eax,16							// Shift second pixel up.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm1,ebx							// Put in MMX register.

		punpckldq	mm4,mm1							// Combine all four pixels.
		movq		[edx + edi*2 - 8],mm4			// Store 4 pixels

		add			edi,4							// Inrcement index by 4
		jl			INNER_LOOP_4P					// Loop (while >4 pixels)

FINISH_REMAINDER4:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		jnz			THREE_OR_LESS

		//
		// Four pixels left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift second pixel up.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm4,ebx							// Put in MMX register.

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1

		shl			eax,16							// Shift second pixel up.

		or			ebx,eax							// Combine 2 pixels.
		movd		mm1,ebx							// Put in MMX register.

		punpckldq	mm4,mm1							// Combine all four pixels.
		movq		[edx - 8],mm4					// Store 4 pixels

		jmp			END_OF_SCANLINE

THREE_OR_LESS:
		sub			edi,2
		jge			FINISH_REMAINDER2

		//
		// Three pixels left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		packssdw	mm0,mm0							// Pack integer texture values.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 1
		pand		mm0,mm3							// Mask for tiling

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 2
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx - 6],ebx					// Store pixels

		movd		ebx,mm0							// Get texture index 1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		mov			bx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p1

		mov			[edx - 2],bx					// Store pixel 1

		jmp			short END_OF_SCANLINE

FINISH_REMAINDER2:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			ONE_PIXEL_LEFT

		//
		// Two pixels left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1

		movzx		eax,byte ptr[esi + eax + 4]		// Lookup texel 2

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for pixel 2

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for pixel 1

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[edx - 4],ebx					// Store pixels

		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movd		ebx,mm0							// Get texture index 1

		movzx		ebx,byte ptr[esi + ebx + 4]		// Lookup texel 1
		mov			eax,[i_pixel]					// Load remaining length.

		mov			bx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p1

		mov			[edx - 2],bx					// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TTextureLinear.indCoord.pfIndex

		mov		ebx,[eax]TTextureLinear.fxXDifference

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
		mov		[eax]TTextureLinear.fxXDifference,ebx

		movq	[eax]TTextureLinear.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TTextureLinear.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TTextureLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}

// Restore default section.
#pragma code_seg()
