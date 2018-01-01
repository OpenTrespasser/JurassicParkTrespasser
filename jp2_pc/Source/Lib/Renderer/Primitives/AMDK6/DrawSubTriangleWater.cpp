/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function for terrain.
 *		Optimized for the AMD K6-3D processor.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/DrawSubTriangleWater.cpp                $
 * 
 * 5     10/01/98 12:42a Asouth
 * Added #ifdefs for MW build; explicit scope of fixed
 * 
 * 4     98.04.30 5:22p Mmouni
 * Added stippled texture primitives.
 * 
 * 3     97.11.20 4:23p Mmouni
 * Perspective routine now uses prologue/epilogue macros.
 * 
 * 2     97.11.12 1:28p Mmouni
 * Aligned inner loops for optimal pre-decode.
 * 
 * 1     97.11.11 9:50p Mmouni
 * Optimized for K6-3D.
 * 
 * 1     97/11/09 7:07p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"


// Put this stuff in a seperate section for alignment.
#pragma code_seg("StriWater")


//
// Globals.
//

// Values supplied by the light blend module.
extern uint16  u2WaterMask;
extern uint16* pu2WaterAlpha;

// Zero extended version of u2WaterMask.
static uint32 u4WaterMask;


//
// Function implementations.
//

//*****************************************************************************************
//
// Perspective, alpha texture.
//
void DrawSubtriangle(TWaterPersp* pscan, CDrawPolygon<TWaterPersp>* pdtri)
{
	static void* pvLastTexture = 0;
	static void* pvLastFogClut = 0;
	TWaterPersp* plinc = &pdtri->pedgeBase->lineIncrement;

	Assert(pu2WaterAlpha);

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
		mov		ebx,[eax]TWaterPersp.fxX

		mov		edx,[eax]TWaterPersp.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TWaterPersp.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TWaterPersp.indCoord.fInvZ
		movq	[tvEdgeStep.UInvZ],mm0

		mov		[tvEdgeStep.InvZ],ebx

		//
		// Setup:
		// QWORD qMFactor = width,1,width,1
		// Load global texture values. 
		//
		movd		mm3,[iTexWidth]					// 0,0,0,Width

		movd		mm4,[u4OneOne]					// 0,0,1,1

		punpcklwd	mm3,mm3							// 0,0,Width,Width

		punpcklwd	mm4,mm3									// Width,1,Width,1
		movq		mm0,[ecx]TWaterPersp.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4							// Save mfactor
		movd		mm1,[ecx]TWaterPersp.indCoord.fInvZ		// mm1 = (?,1/Z)

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// esi,ecx,mm0,mm1 setup. 
		//
Y_LOOP:
		mov		eax,[esi]CDrawPolygonBase.iY

		mov		ebx,[bEvenScanlinesOnly]
		push	ebp

		// Check to see if we sould skip this scanline.
		and		eax,ebx
		jnz		END_OF_SCANLINE

		//
		// Setup for prespective correction and looping.
		//
	#ifdef __MWERKS__
		#define SLTYPE TWaterPersp
		#include "pspclamp3.inl"
	#else
		PERSPECTIVE_SCANLINE_PROLOGUE_CLAMP_3DX(TWaterPersp)
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
		// ecx = clut pointer		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = alpha index mask
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm6							// Step U,V

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		movq		mm2,[qMFactor]					// Load texture multipliers
		psrad		mm1,16							// Shift for integer U2,V2

		mov			ecx,[pu2WaterAlpha]				// Water alpha table.
		packssdw	mm0,mm1							// Pack integer texture values

		movzx		ebp,[u2WaterMask]				// Load water alpha mask.
		dec			esi								// Hack to force SIB + offset in loop

		dec			ecx								// Hack to force SIB + offset in loop
		mov			[pvBaseOfLine],edx				// Save base of scanline pointer.

		pmaddwd		mm0,mm2							// Compute texture indicies.
		mov			[u4WaterMask],ebp				// Save zero extended version of u2WaterMask.

		movq		[qIndexTemp],mm0				// Save texture indicies.

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
		movzx		eax,word ptr[edx + edi*2 - 4]	// Get screen pixel 1.
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,word ptr[edx + edi*2 - 2]	// Get screen pixel 2.
		movq		mm0,mm7							// Copy U1,V1

		psrad		mm0,16							// Shift for integer U1,V1
		paddd		mm7,mm6							// Step U,V

		and			eax,ebp							// And p1 with u2WaterMask.
		mov			edx,dword ptr[qIndexTemp]		// Get texture index 1.

		and			ebx,ebp							// And p2 with u2WaterMask.
		mov			ebp,dword ptr[qIndexTemp+4]		// Get texture index 2.

		movzx		edx,word ptr[esi + edx*2 + 1]	// Lookup texel 1.
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebp,word ptr[esi + ebp*2 + 1]	// Lookup texel 2.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.

		or			eax,edx							// Combine alhpa and screen pixel.
		mov			edx,[pvBaseOfLine]				// Reload base of line pointer.

		or			ebx,ebp							// Combine alhpa and screen pixel.
		mov			ebp,[u4WaterMask]				// Re-load water alpha mask value.

		movzx		eax,word ptr[ecx + eax*2 + 1]	// Lookup through alpha water table.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Lookup through alpha water table.
		movq		[qIndexTemp],mm0				// Save texture indicies.

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
		movzx		eax,word ptr[edx + edi*2 - 4]	// Get screen pixel 1.
		paddd		mm7,mm6							// Step U,V
		movzx		ebx,word ptr[edx + edi*2 - 2]	// Get screen pixel 2.
		and			eax,ebp							// And p1 with u2WaterMask.
		mov			edx,dword ptr[qIndexTemp]		// Get texture index 1.
		and			ebx,ebp							// And p2 with u2WaterMask.
		mov			ebp,dword ptr[qIndexTemp+4]		// Get texture index 2.
		movzx		edx,word ptr[esi + edx*2 + 1]	// Lookup texel 1.
		movzx		ebp,word ptr[esi + ebp*2 + 1]	// Lookup texel 2.
		or			eax,edx							// Combine alhpa and screen pixel.
		mov			edx,[pvBaseOfLine]				// Reload base of line pointer.
		or			ebx,ebp							// Combine alhpa and screen pixel.
		mov			ebp,[u4WaterMask]				// Re-load water alpha mask value.
		movzx		eax,word ptr[ecx + eax*2 + 1]	// Lookup through alpha water table.
		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Lookup through alpha water table.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movzx		eax,word ptr[edx + edi*2 - 4]	// Get screen pixel 1.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1.
		and			eax,ebp							// And p1 with u2WaterMask.
		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 1.
		or			eax,ebx							// Combine alhpa and screen pixel.
		movzx		eax,word ptr[ecx + eax*2 + 1]	// Lookup through alpha water table.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		mov			eax,[i_pixel]					// Load remaining length.

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		jz		short END_OF_SCANLINE

		//
		// Do perspective correction and looping.
		//
	#ifdef __MWERKS__
		#include "pseclamp3.inl"
	#else
		PERSPECTIVE_SCANLINE_EPILOGUE_CLAMP_3DX
	#endif

		jmp		SUBDIVISION_LOOP

END_OF_SCANLINE:
		//		
		// Increment the base edge.
		//
		pop		ebp
		mov		esi,[pdtri]						// Pointer to polygon object.

		mov		ecx,[pscan]						// Pointer to scanline object.

		// Step length, line starting address.
		mov		edx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,[esi]CDrawPolygonBase.iLineStartIndex

		add		edx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx

		add		edi,gsGlobals.u4LinePixels

		mov		eax,[ecx]TWaterPersp.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TWaterPersp.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TWaterPersp.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TWaterPersp.indCoord.fInvZ

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
		movq	[ecx]TWaterPersp.indCoord.fUInvZ,mm0

		movd	[ecx]TWaterPersp.indCoord.fInvZ,mm1

		mov		[ecx]TWaterPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TWaterPersp.fxXDifference.i4Fx,ebx
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
// Linear, alpha texture.
//
//**********************************
void DrawSubtriangle(TWaterLinear* pscan, CDrawPolygon<TWaterLinear>* pdtri)
{
	typedef TWaterLinear::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TWaterLinear>* pdtriGlbl;
	static TWaterLinear* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TWaterLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.

		mov		esi,[pdtri]							// Pointer to polygon object.
		push	ebp									// Save base pointer.

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
		mov		ebx,[edi]TWaterLinear.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TWaterLinear.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TWaterLinear.indCoord.pfIndex
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

		movq		mm7,[eax]TWaterLinear.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TWaterLinear.fxX.i4Fx

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
		// eax = texel_index2		ebx = texel_index2
		// ecx = clut pointer		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = alpha index mask
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		pf2id		(m7,m7)							// Convert texture values to 16.16 fixed.
		mov			esi,[pvTextureBitmap]			// Load texture base pointer.

		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm6							// Step U,V

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		mov			ecx,[pu2WaterAlpha]				// Water alpha table.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values
		movzx		ebp,[u2WaterMask]				// Load water alpha mask.

		pmaddwd		mm0,mm2							// Compute texture indicies.
		mov			[pvBaseOfLine],edx				// Save base of scanline pointer.

		dec			esi								// Hack to force SIB + offset in loop
		mov			[u4WaterMask],ebp				// Save zero extended version of u2WaterMask.

		dec			ecx								// Hack to force SIB + offset in loop
		movq		[qIndexTemp],mm0				// Save texture indicies.

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
		movzx		eax,word ptr[edx + edi*2 - 4]	// Get screen pixel 1.
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,word ptr[edx + edi*2 - 2]	// Get screen pixel 2.
		movq		mm0,mm7							// Copy U1,V1

		psrad		mm0,16							// Shift for integer U1,V1
		paddd		mm7,mm6							// Step U,V

		and			eax,ebp							// And p1 with u2WaterMask.
		mov			edx,dword ptr[qIndexTemp]		// Get texture index 1.

		and			ebx,ebp							// And p2 with u2WaterMask.
		mov			ebp,dword ptr[qIndexTemp+4]		// Get texture index 2.

		movzx		edx,word ptr[esi + edx*2 + 1]	// Lookup texel 1.
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebp,word ptr[esi + ebp*2 + 1]	// Lookup texel 2.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.

		or			eax,edx							// Combine alhpa and screen pixel.
		mov			edx,[pvBaseOfLine]				// Reload base of line pointer.

		or			ebx,ebp							// Combine alhpa and screen pixel.
		mov			ebp,[u4WaterMask]				// Re-load water alpha mask value.

		movzx		eax,word ptr[ecx + eax*2 + 1]	// Lookup through alpha water table.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Lookup through alpha water table.
		movq		[qIndexTemp],mm0				// Save texture indicies.

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
		movzx		eax,word ptr[edx + edi*2 - 4]	// Get screen pixel 1.
		movzx		ebx,word ptr[edx + edi*2 - 2]	// Get screen pixel 2.
		and			eax,ebp							// And p1 with u2WaterMask.
		mov			edx,dword ptr[qIndexTemp]		// Get texture index 1.
		and			ebx,ebp							// And p2 with u2WaterMask.
		mov			ebp,dword ptr[qIndexTemp+4]		// Get texture index 2.
		movzx		edx,word ptr[esi + edx*2 + 1]	// Lookup texel 1.
		movzx		ebp,word ptr[esi + ebp*2 + 1]	// Lookup texel 2.
		or			eax,edx							// Combine alhpa and screen pixel.
		mov			edx,[pvBaseOfLine]				// Reload base of line pointer.
		or			ebx,ebp							// Combine alhpa and screen pixel.
		mov			ebp,[u4WaterMask]				// Re-load water alpha mask value.
		movzx		eax,word ptr[ecx + eax*2 + 1]	// Lookup through alpha water table.
		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Lookup through alpha water table.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movzx		eax,word ptr[edx + edi*2 - 4]	// Get screen pixel 1.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1.
		and			eax,ebp							// And p1 with u2WaterMask.
		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 1.
		or			eax,ebx							// Combine alhpa and screen pixel.
		movzx		eax,word ptr[ecx + eax*2 + 1]	// Lookup through alpha water table.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TWaterLinear.indCoord.pfIndex

		mov		ebx,[eax]TWaterLinear.fxXDifference

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
		mov		[eax]TWaterLinear.fxXDifference,ebx

		movq	[eax]TWaterLinear.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TWaterLinear.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TWaterLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms

		pop		ebp									// Restore base pointer.
	}
}


//*****************************************************************************************
//
// Perspective, stippled texture.
//
void DrawSubtriangle(TStippleTexPersp* pscan, CDrawPolygon<TStippleTexPersp>* pdtri)
{
	TStippleTexPersp* plinc = &pdtri->pedgeBase->lineIncrement;

	Assert(pu2WaterAlpha);

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
		mov		ebx,[eax]TStippleTexPersp.fxX

		mov		edx,[eax]TStippleTexPersp.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TStippleTexPersp.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TStippleTexPersp.indCoord.fInvZ
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
		movq		mm0,[ecx]TStippleTexPersp.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4								// Save mfactor
		movd		mm1,[ecx]TStippleTexPersp.indCoord.fInvZ	// mm1 = (?,1/Z)

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
		//PERSPECTIVE_SCANLINE_PROLOGUE_CLAMP_3DX(TStippleTexPersp)
		//
		// Determine the start and end of the scanline. 
		//
		mov		ebx,[ecx]TStippleTexPersp.fxX.i4Fx

		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,eax

		mov		eax,[ecx]TStippleTexPersp.fxX.i4Fx
		sar		ebx,16

		sar		eax,16
		add		edx,ebx

		sub		eax,ebx									// eax == i_pixel
		jge		END_OF_SCANLINE

		mov		[i_screen_index],edx					// Save scanline index.
		mov		ebx,[ecx]TStippleTexPersp.fxX.i4Fx		// Fractional X.

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

		pfadd	(m2,m0)							// Step fGUInvZ,fGVInvZ
		movd	[tvCurUVZ.InvZ],mm3				// Save updated 1/Z

		pfmul	(m4,m0)							// mm4 = V1,U1
		pfrcp	(m0,m3)							// f_next_z = mm0 = 1/fGInvZ

		movq	[tvCurUVZ.UInvZ],mm2					// Save updated U/Z,V/Z
		movd	mm1,float ptr[fInverseIntTable+edi*4]	// Reciprocal of span width.

		pfmul	(m0,m2)							// mm0 = V2,U2
		movq	mm2,[pfFixed16Scale]			// Load fixed point scale factors.

		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
		pfmax	(m4,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m4,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight

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
		// ecx = ???				edx = dest_base_ptr
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

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,4
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 0, 1, or 17 past the beginning of a cache line.
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

		mov			[edx + edi*2 - 8],ax			// Store pixel 1

		mov			[edx + edi*2 - 4],bx			// Store pixel 2

		add			edi,4							// Inrcement index by 4
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

		mov			[edx + edi*2 - 8],ax			// Store pixel 1

		mov			[edx + edi*2 - 4],bx			// Store pixel 2

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

		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight

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

		mov		eax,[ecx]TStippleTexPersp.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TStippleTexPersp.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TStippleTexPersp.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TStippleTexPersp.indCoord.fInvZ

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
		movq	[ecx]TStippleTexPersp.indCoord.fUInvZ,mm0

		movd	[ecx]TStippleTexPersp.indCoord.fInvZ,mm1

		mov		[ecx]TStippleTexPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TStippleTexPersp.fxXDifference.i4Fx,ebx
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
// Linear, stippled texture.
//
//**********************************
void DrawSubtriangle(TStippleTexLinear* pscan, CDrawPolygon<TStippleTexLinear>* pdtri)
{
	typedef TStippleTexLinear::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TStippleTexLinear>* pdtriGlbl;
	static TStippleTexLinear* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TStippleTexLinear* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[edi]TStippleTexLinear.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TStippleTexLinear.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TStippleTexLinear.indCoord.pfIndex
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

		movq		mm7,[eax]TStippleTexLinear.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TStippleTexLinear.fxX.i4Fx

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
		// ecx = ???				edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm5 = uvslope*2
		// mm6 = uvslope			mm7 = uv
		//
		pf2id		(m7,m7)								// Convert texture values to 16.16 fixed.
		mov			ebx,[eax]TStippleTexPersp.fxX.i4Fx	// Fractional X.

		movq		mm5,mm6								// Copy step.
		mov			ecx,[esi]CDrawPolygonBase.iY		// Integer Y.

		paddd		mm5,mm5								// Double step.
		sar			ebx,16								// Integer X.

		// if ((y xor x) & 1) skip a pixel.
		xor		ebx,ecx

		and		ebx,1
		jz		NO_SKIP

		paddd	mm7,mm6								// Step U,V
		inc		edi

		jz		END_OF_SCANLINE

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

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,4
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 0, 1 or 17 past the beginning of a cache line.
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

		mov			[edx + edi*2 - 8],ax			// Store pixel 1

		mov			[edx + edi*2 - 4],bx			// Store pixel 2

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

		mov			[edx + edi*2 - 8],ax			// Store pixel 1

		mov			[edx + edi*2 - 4],bx			// Store pixel 2

		jmp			short END_OF_SCANLINE

ONE_OR_TWO_PIXELS_LEFT:
		//
		// Draw one pixel.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1.

		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1.

		mov			[edx + edi*2 - 8],ax			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TStippleTexLinear.indCoord.pfIndex

		mov		ebx,[eax]TStippleTexLinear.fxXDifference

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
		mov		[eax]TStippleTexLinear.fxXDifference,ebx

		movq	[eax]TStippleTexLinear.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TStippleTexLinear.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TStippleTexLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}


#if (0)

//*********************************************************************************************
void DrawSubtriangle(TWaterPersp* pscan, CDrawPolygon<TWaterPersp>* pdtri)
{
	typedef TWaterPersp::TPixel TDest;

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
				TWaterPersp::TIndex index(pscan->indCoord, i_pixel);
				TDest* apix_screen = ((TDest*)pdtri->prasScreen->pSurface) + i_screen_index;

				// Iterate left or right along the destination scanline.
				for (;;)
				{
					// Do next subdivision.
					int i_pixel_inner = index.i4StartSubdivision(i_pixel);

					TDest* apix_screen_inner = apix_screen + i_pixel;

					for (;;)
					{
						
						// Get an indexed alpha pixel.
						uint16 u2_alpha = ((uint16*)pvTextureBitmap)[index.iGetIndex()];

						++index;

						// Get the screen pixel.
						uint16 u2_screen = apix_screen_inner[i_pixel_inner];

						// And off bits for the alpha mask.
						u2_screen &= u2WaterMask;

						// Or in bits for the alpha band.
						u2_screen |= u2_alpha;

						// Look up the alpha colour.
						u2_screen = pu2WaterAlpha[u2_screen];

						// Write the pixel back to the screen.
						apix_screen_inner[i_pixel_inner] = u2_screen;

						// Increment values.
						i_pixel_inner += i4ScanlineDirection;

						// Break from subdivision loop if there are no more pixels to render.
						if (i_pixel_inner == 0)
							break;
					}

					// Exit scanline routine if there are no more pixels to render.
					if (i_pixel == 0)
						break;

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
void DrawSubtriangle(TWaterLinear* pscan, CDrawPolygon<TWaterLinear>* pdtri)
{
	typedef TWaterLinear::TPixel TDest;

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
				TWaterLinear::TIndex index(pscan->indCoord, i_pixel);
				TDest* apix_screen = ((TDest*)pdtri->prasScreen->pSurface) + i_screen_index;

				// Iterate left or right along the destination scanline.
				for (;;)
				{
					// Do next subdivision.
					int i_pixel_inner = index.i4StartSubdivision(i_pixel);

					TDest* apix_screen_inner = apix_screen + i_pixel;

					for (;;)
					{
						
						// Get an indexed alpha pixel.
						uint16 u2_alpha = ((uint16*)pvTextureBitmap)[index.iGetIndex()];

						++index;

						// Get the screen pixel.
						uint16 u2_screen = apix_screen_inner[i_pixel_inner];

						// And off bits for the alpha mask.
						u2_screen &= u2WaterMask;

						// Or in bits for the alpha band.
						u2_screen |= u2_alpha;

						// Look up the alpha colour.
						u2_screen = pu2WaterAlpha[u2_screen];

						// Write the pixel back to the screen.
						apix_screen_inner[i_pixel_inner] = u2_screen;

						// Increment values.
						i_pixel_inner += i4ScanlineDirection;

						// Break from subdivision loop if there are no more pixels to render.
						if (i_pixel_inner == 0)
							break;
					}

					// Exit scanline routine if there are no more pixels to render.
					if (i_pixel == 0)
						break;

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

#endif


// Restore default section.
#pragma code_seg()
