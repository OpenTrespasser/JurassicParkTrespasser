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
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/DrawSubTriangleTexGourEx.cpp            $
 * 
 * 8     10/01/98 12:42a Asouth
 * Added #ifdefs for MW build; explicit scope of fixed
 * 
 * 7     97.11.04 10:59p Mmouni
 * Linear version of primitives now use 3dx/mmx.
 * 
 * 6     97.10.30 1:40p Mmouni
 * All perspective sub-triangle routines are now 3DX/MMX.
 * 
 * 5     97.10.27 1:25p Mmouni
 * Initial K6-3D version, copied from the Pentium versions.
 * 
 * 9     10/10/97 1:47p Mmouni
 * All inner loops are now left to right only, and optimized some.
 * 
 * 8     9/29/97 11:54a Mmouni
 * Optimized y-loops, and optimized inner loops some.
 * 
 * 7     9/15/97 2:10p Mmouni
 * Optimized inner loops (not final yet).
 * 
 * 6     9/01/97 8:03p Rwyatt
 * ANDs added to enable tiling
 * 
 * 5     8/15/97 12:48a Rwyatt
 * Removed 8 bit terrain primitive and put it into its own file
 * 
 * 4     97/08/12 6:58p Pkeet
 * Added special primitives for terrain texturing.
 * 
 * 3     97/07/18 3:30p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 2     7/14/97 12:36p Rwyatt
 * Checkled in so Scott can move them....
 * 
 * 1     7/07/97 11:31p Rwyatt
 * Initial Implementation of Pentium Specifics
 * 
 ***********************************************************************************************
 ***********************************************************************************************
 * 
 * 6     97/06/14 12:19a Pkeet
 * Some changes.
 * 
 * 5     97/06/13 6:42p Pkeet
 * Initial pass at the linear versions.
 * 
 * 4     97/06/13 6:29p Pkeet
 * Initial pass at perspective corrected, opaque assembly primitive.
 * 
 * 3     97/06/13 5:53p Pkeet
 * Initial clumsy attempt to get the perspective, transparent primitive into assembly.
 * 
 * 2     97/06/12 5:56p Pkeet
 * Added capability to render only every other scanline and to double the number of scanlines.
 * 
 * 1     97/06/11 10:15p Pkeet
 * Initial C++ versions of primitives.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "AsmSupport.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"


// Put this stuff in a seperate section for alignment.
#pragma code_seg("StriGTex")


//*****************************************************************************************
//
// Gouraud shaded transparent perspective correct texture (16 bit dest).
//
void DrawSubtriangle(TTexturePerspTransGour* pscan, CDrawPolygon<TTexturePerspTransGour>* pdtri)
{
	typedef TTexturePerspTransGour::TPixel prevent_internal_compiler_errors;
	::fixed fx_inc;
	::fixed fx_diff;
	::fixed fx_gour_inc;

	TTexturePerspTransGour* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		//
		// Local copies of edge stepping values.
		//
		// fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;
		// fx_gour_inc = pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		// f_inc_uinvz = pdtri->pedgeBase->lineIncrement.indCoord.fUInvZ;
		// f_inc_vinvz = pdtri->pedgeBase->lineIncrement.indCoord.fVInvZ;
		// f_inc_invz  = pdtri->pedgeBase->lineIncrement.indCoord.fInvZ;
		//
		mov		eax,[plinc]
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.
		mov		ebx,[eax]TTexturePerspTransGour.fxX

		mov		edx,[eax]TTexturePerspTransGour.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TTexturePerspTransGour.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TTexturePerspTransGour.indCoord.fInvZ
		movq	[tvEdgeStep.UInvZ],mm0

		mov		edx,[eax]TTexturePerspTransGour.gourIntensity.fxIntensity
		mov		[tvEdgeStep.InvZ],ebx

		mov		fx_gour_inc,edx
	
		//
		// Setup:
		// QWORD qShadeSlopes	= fxDeltaXIntensity*2,fxDeltaXIntensity*2
		// QWORD qMFactor		= width,1,width,1
		// Load global texture values. 
		//
		movd		mm2,[fxDeltaXIntensity]

		punpckldq	mm2,mm2							// fxDeltaXIntensity,fxDeltaXIntensity
		movd		mm3,[iTexWidth]					// 0,0,0,Width

		paddd		mm2,mm2							// fxDeltaXIntensity*2,fxDeltaXIntensity*2
		movd		mm4,[u4OneOne]					// 0,0,1,1

		movq		[qShadeSlopes],mm2
		punpcklwd	mm3,mm3							// 0,0,Width,Width

		punpcklwd	mm4,mm3										// Width,1,Width,1
		movq		mm0,[ecx]TTexturePerspTransGour.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4								// Save mfactor
		movd		mm1,[ecx]TTexturePerspTransGour.indCoord.fInvZ	// mm1 = (?,1/Z)

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// esi,ecx,mm0,mm1 setup. 
		//
Y_LOOP:
		push	ebp									// Save ebp.
		mov		eax,[esi]CDrawPolygonBase.iY

		mov		ebx,[bEvenScanlinesOnly]

		// Check to see if we sould skip this scanline.
		and		eax,ebx
		jnz		END_OF_SCANLINE

		//
		// Setup:
		// QWORD qShadeValue	= fxIntensity+1,fxIntensity
		//
		mov		edi,[ecx]TTexturePerspTransGour.gourIntensity.fxIntensity.i4Fx

		mov		dword ptr[qShadeValue],edi

		add		edi,[fxDeltaXIntensity]

		mov		dword ptr[qShadeValue+4],edi

		//
		// Setup for prespective correction and looping.
	#ifdef __MWERKS__
		#define SLTYPE TTexturePerspTransGour
		#include "psprologue3.inl"
	#else
		PERSPECTIVE_SCANLINE_PROLOGUE_3DX(TTexturePerspTransGour)
	#endif

SUBDIVISION_LOOP:
		//--------------------------------------------------------------------------- 
		// Draw the subdivision.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = clut_index1		ebx = texel_index
		// ecx = clut_index2		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = clut_base_ptr
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm4 = shade+1,shade		mm5 = shadeslope*2,shadeslope*2
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		mov			ebp,[pvClutConversion]			// Load clut base pointer.
		movq		mm0,mm7							// Copy U1,V1

		paddd		mm7,mm6							// Step U,V

		movq		mm2,[qMFactor]					// Load texture multipliers
		movq		mm1,mm7							// Copy U2,V2

		movq		mm3,[qUVMasks]					// Load texture masks
		psrad		mm0,16							// Shift for integer U1,V1

		movq		mm4,[qShadeValue]				// Load shading value
		psrad		mm1,16							// Shift for integer U2,V2

		movq		mm5,[qShadeSlopes]				// Load shading slopes
		packssdw	mm0,mm1							// Pack integer texture values

		movq		mm1,mm4							// Copy shading values
		pand		mm0,mm3							// Mask for tiling

		psrld		mm1,8							// Shift shading values
		paddd		mm4,mm5							// Step shading values

		pmaddwd		mm0,mm2							// Compute texture indices
		movq		[qShadeTemp],mm1				// Save shading indices

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 8 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16

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
		// mm0 = Texture indices.
		// qShadeTemp = CLUT indices.
		// mm4 is stepped once.
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U1,V1

		mov			al,[esi + ebx]					// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm1,mm7							// Copy U2,V2

		mov			ecx,dword ptr[qShadeTemp + 4]	// Get shade value 2
		psrad		mm1,16							// Shift for integer U2,V2

		mov			cl,[esi + ebx]					// Lookup texel 2
		packssdw	mm0,mm1							// Pack integer texture values.

		pand		mm0,mm3							// Mask for tiling
		movq		mm1,mm4							// Copy shading values.

		psrld		mm1,8							// Shift shading values.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		paddd		mm4,mm5							// Step shading values.
		movq		[qShadeTemp],mm1				// Save shading indicies.

		test		al,al							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT

		mov			ax,word ptr[ebp + eax*2]		// Clut lookup for p1

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT:
		test		cl,cl							// Test for transparency.
		jz			short PIXEL2_TRANSPARENT

		mov			cx,word ptr[ebp + ecx*2]		// Clut lookup for p2

		mov			[edx + edi*2 - 2],cx			// Store pixel 2

PIXEL2_TRANSPARENT:
		add			edi,2							// Inrcement index by 2
		jl			short INNER_LOOP_2P				// Loop (while >2 pixels)

FINISH_REMAINDER:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			short ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture/shade co-ordinates once.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1

		mov			eax,dword ptr[qShadeTemp]		// Get shade value 1

		mov			al,[esi + ebx]					// Lookup texel 1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2

		mov			ecx,dword ptr[qShadeTemp + 4]	// Get shade value 2
		movq		[qShadeValue],mm4				// Save intensity value.

		mov			cl,[esi + ebx]					// Lookup texel 2

		test		al,al							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT_END

		mov			ax,word ptr[ebp + eax*2]		// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT_END:
		test		cl,cl							// Test for transparency.
		jz			short PIXEL2_TRANSPARENT_END

		mov			cx,word ptr[ebp + ecx*2]		// Clut lookup for p2
		mov			[edx + edi*2 - 2],cx			// Store pixel 2

PIXEL2_TRANSPARENT_END:

		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		mov			eax,[i_pixel]					// Load remaining length.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			ecx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm5,1							// Reduce shade step to 1x.
		
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		psubd		mm4,mm5							// Backup shading values by 1.

		mov			bl,[esi + ecx]					// Lookup texel 1
		movq		[qShadeValue],mm4				// Save intensity value.

		test		bl,bl							// Test for transparency.
		jz			short END_OF_SPAN

		mov			bx,word ptr[ebp + ebx*2]		// Clut lookup for p1
		mov			[edx + edi*2 - 4],bx			// Store pixel 1

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		je		short END_OF_SCANLINE

		//
		// Do perspective correction for next span.
	#ifdef __MWERKS__
		#include "psepilogue3.inl"
	#else
		PERSPECTIVE_SCANLINE_EPILOGUE_3DX
	#endif

		jmp		SUBDIVISION_LOOP

END_OF_SCANLINE:
		pop		ebp								// Restore ebp.

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

		mov		eax,[ecx]TTexturePerspTransGour.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TTexturePerspTransGour.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TTexturePerspTransGour.indCoord.fUInvZ

		movd	mm1,[ecx]TTexturePerspTransGour.indCoord.fInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm3,[tvEdgeStep.InvZ]

		pfadd	(m0,m2)
		mov		edx,[ecx]TTexturePerspTransGour.gourIntensity.fxIntensity.i4Fx

		pfadd	(m1,m3)
		mov		edi,[fx_diff]

		add		edx,[fx_gour_inc]

		add		eax,[fx_inc]

		add		ebx,edi
		jge		short NO_UNDERFLOW

		// Underflow.
		mov		edi,[fxIModDiv]
		add		ebx,0x00010000

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short FINISH_LOOPING

		// Overflow.
		mov		edi,[fxIModDiv]
		sub		ebx,0x00010000

		add		edx,edi

BORROW_OR_CARRY:
		// Add borrow/carry adjustments to U,V,Z.
		movq	mm2,[tvNegAdj.UInvZ]

		movd	mm3,[tvNegAdj.InvZ]

		pfadd	(m0,m2)

		pfadd	(m1,m3)

FINISH_LOOPING:
		// Save values and Y Loop control.
		movq	[ecx]TTexturePerspTransGour.indCoord.fUInvZ,mm0

		movd	[ecx]TTexturePerspTransGour.indCoord.fInvZ,mm1

		mov		[ecx]TTexturePerspTransGour.gourIntensity.fxIntensity.i4Fx,edx

		mov		[ecx]TTexturePerspTransGour.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TTexturePerspTransGour.fxXDifference.i4Fx,ebx
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
// Gouraud shaded perspective correct texture (16 bit dest).
//
void DrawSubtriangle(TTexturePerspGour* pscan, CDrawPolygon<TTexturePerspGour>* pdtri)
{
	typedef TTexturePerspGour::TPixel prevent_internal_compiler_errors;
	::fixed fx_inc;
	::fixed fx_diff;
	::fixed fx_gour_inc;

	TTexturePerspGour* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		//
		// Local copies of edge stepping values.
		//
		// fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;
		// fx_gour_inc = pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		// f_inc_uinvz = pdtri->pedgeBase->lineIncrement.indCoord.fUInvZ;
		// f_inc_vinvz = pdtri->pedgeBase->lineIncrement.indCoord.fVInvZ;
		// f_inc_invz  = pdtri->pedgeBase->lineIncrement.indCoord.fInvZ;
		//
		mov		eax,[plinc]
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.
		mov		ebx,[eax]TTexturePerspGour.fxX

		mov		edx,[eax]TTexturePerspGour.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TTexturePerspGour.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TTexturePerspGour.indCoord.fInvZ
		movq	[tvEdgeStep.UInvZ],mm0

		mov		edx,[eax]TTexturePerspGour.gourIntensity.fxIntensity
		mov		[tvEdgeStep.InvZ],ebx

		mov		fx_gour_inc,edx
	
		//
		// Setup:
		// QWORD qShadeSlopes	= fxDeltaXIntensity*2,fxDeltaXIntensity*2
		// QWORD qMFactor		= width,1,width,1
		// Load global texture values. 
		//
		movd		mm2,[fxDeltaXIntensity]

		punpckldq	mm2,mm2							// fxDeltaXIntensity,fxDeltaXIntensity
		movd		mm3,[iTexWidth]					// 0,0,0,Width

		paddd		mm2,mm2							// fxDeltaXIntensity*2,fxDeltaXIntensity*2
		movd		mm4,[u4OneOne]					// 0,0,1,1

		movq		[qShadeSlopes],mm2
		punpcklwd	mm3,mm3							// 0,0,Width,Width

		punpcklwd	mm4,mm3										// Width,1,Width,1
		movq		mm0,[ecx]TTexturePerspGour.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4								// Save mfactor
		movd		mm1,[ecx]TTexturePerspGour.indCoord.fInvZ	// mm1 = (?,1/Z)

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// esi,ecx,mm0,mm1 setup. 
		//
Y_LOOP:
		mov		eax,[esi]CDrawPolygonBase.iY
		mov		ebx,[bEvenScanlinesOnly]
		push	ebp									// Save ebp.

		// Check to see if we sould skip this scanline.
		and		eax,ebx
		jnz		END_OF_SCANLINE

		//
		// Setup:
		// QWORD qShadeValue	= fxIntensity+1,fxIntensity
		//
		mov		edi,[ecx]TTexturePerspGour.gourIntensity.fxIntensity.i4Fx

		mov		dword ptr[qShadeValue],edi

		add		edi,[fxDeltaXIntensity]

		mov		dword ptr[qShadeValue+4],edi

		//
		// Setup for prespective correction and looping.
	#ifdef __MWERKS__
		#define SLTYPE TTexturePerspGour
		#include "psprologue3.inl"
	#else
		PERSPECTIVE_SCANLINE_PROLOGUE_3DX(TTexturePerspGour)
	#endif

SUBDIVISION_LOOP:
		//--------------------------------------------------------------------------- 
		// Draw the subdivision.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = clut_index1		ebx = texel_index
		// ecx = clut_index2		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = clut_base_ptr
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm4 = shade+1,shade		mm5 = shadeslope*2,shadeslope*2
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		mov			ebp,[pvClutConversion]			// Load clut base pointer.
		movq		mm0,mm7							// Copy U1,V1

		sub			ebp,4							// Hack to force SIB+offset in loop.
		paddd		mm7,mm6							// Step U,V

		movq		mm2,[qMFactor]					// Load texture multipliers
		movq		mm1,mm7							// Copy U2,V2

		movq		mm3,[qUVMasks]					// Load texture masks
		psrad		mm0,16							// Shift for integer U1,V1

		movq		mm4,[qShadeValue]				// Load shading value
		psrad		mm1,16							// Shift for integer U2,V2

		movq		mm5,[qShadeSlopes]				// Load shading slopes
		packssdw	mm0,mm1							// Pack integer texture values

		movq		mm1,mm4							// Copy shading values
		pand		mm0,mm3							// Mask for tiling

		psrld		mm1,8							// Shift shading values
		paddd		mm4,mm5							// Step shading values

		pmaddwd		mm0,mm2							// Compute texture indices
		movq		[qShadeTemp],mm1				// Save shading indices

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 8 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16

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
		// mm0 = Texture indices.
		// qShadeTemp = CLUT indices.
		// mm4 is stepped once.
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U1,V1

		mov			al,[esi + ebx]					// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm1,mm7							// Copy U2,V2

		mov			ecx,dword ptr[qShadeTemp + 4]	// Get shade value 2
		psrad		mm1,16							// Shift for integer U2,V2

		mov			cl,[esi + ebx]					// Lookup texel 2
		packssdw	mm0,mm1							// Pack integer texture values.

		pand		mm0,mm3							// Mask for tiling
		movq		mm1,mm4							// Copy shading values.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup for p1
		psrld		mm1,8							// Shift shading values.

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qShadeTemp],mm1				// Save shading indicies.

		movzx		ecx,word ptr[ebp + ecx*2 + 4]	// Clut lookup for p2
		paddd		mm4,mm5							// Step shading values.

		mov			[edx + edi*2 - 2],cx			// Store pixel 2

		add			edi,2							// Inrcement index by 2
		jl			short INNER_LOOP_2P				// Loop (while >2 pixels)

FINISH_REMAINDER:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			short ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture/shade co-ordinates once.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1

		mov			eax,dword ptr[qShadeTemp]		// Get shade value 1

		mov			al,[esi + ebx]					// Lookup texel 1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2

		mov			ecx,dword ptr[qShadeTemp + 4]	// Get shade value 2

		mov			cl,[esi + ebx]					// Lookup texel 2

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup for p1

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

		movzx		ecx,word ptr[ebp + ecx*2 + 4]	// Clut lookup for p2
		movq		[qShadeValue],mm4				// Save intensity value.

		mov			[edx + edi*2 - 2],cx			// Store pixel 2

		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		mov			eax,[i_pixel]					// Load remaining length.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			ecx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm5,1							// Reduce shade step to 1x.
		
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		psubd		mm4,mm5							// Backup shading values by 1.

		mov			bl,[esi + ecx]					// Lookup texel 1
		movq		[qShadeValue],mm4				// Save intensity value.

		mov			bx,word ptr[ebp + ebx*2 + 4]	// Clut lookup for p1

		mov			[edx + edi*2 - 4],bx			// Store pixel 1

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		je		short END_OF_SCANLINE

		//
		// Do perspective correction for next span.
	#ifdef __MWERKS__
		#include "psepilogue3.inl"
	#else
		PERSPECTIVE_SCANLINE_EPILOGUE_3DX
	#endif

		jmp		SUBDIVISION_LOOP

END_OF_SCANLINE:
		pop		ebp								// Restore ebp.

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

		mov		eax,[ecx]TTexturePerspGour.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TTexturePerspGour.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TTexturePerspGour.indCoord.fUInvZ

		movd	mm1,[ecx]TTexturePerspGour.indCoord.fInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm3,[tvEdgeStep.InvZ]

		pfadd	(m0,m2)
		mov		edx,[ecx]TTexturePerspGour.gourIntensity.fxIntensity.i4Fx

		pfadd	(m1,m3)
		mov		edi,[fx_diff]

		add		edx,[fx_gour_inc]

		add		eax,[fx_inc]

		add		ebx,edi
		jge		short NO_UNDERFLOW

		// Underflow.
		mov		edi,[fxIModDiv]
		add		ebx,0x00010000

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short FINISH_LOOPING

		// Overflow.
		mov		edi,[fxIModDiv]
		sub		ebx,0x00010000

		add		edx,edi

BORROW_OR_CARRY:
		// Add borrow/carry adjustments to U,V,Z.
		movq	mm2,[tvNegAdj.UInvZ]

		movd	mm3,[tvNegAdj.InvZ]

		pfadd	(m0,m2)

		pfadd	(m1,m3)

FINISH_LOOPING:
		// Save values and Y Loop control.
		movq	[ecx]TTexturePerspGour.indCoord.fUInvZ,mm0

		movd	[ecx]TTexturePerspGour.indCoord.fInvZ,mm1

		mov		[ecx]TTexturePerspGour.gourIntensity.fxIntensity.i4Fx,edx

		mov		[ecx]TTexturePerspGour.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TTexturePerspGour.fxXDifference.i4Fx,ebx
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
// Gouraud shaded transparent linear texture (16 bit dest).
//
void DrawSubtriangle(TTextureLinearTransGour* pscan, CDrawPolygon<TTextureLinearTransGour>* pdtri)
{
	typedef TTextureLinearTransGour::TPixel prevent_internal_compiler_errors;
	static CDrawPolygon<TTextureLinearTransGour>* pdtriGlbl;
	static TTextureLinearTransGour* pscanGlbl;
	static ::fixed fx_gour_inc;
	static PackedFloatUV pf_texture_inc;

	TTextureLinearTransGour* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.

		mov		esi,[pdtri]							// Pointer to polygon object.
		push	ebp									// Save ebp.

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
		mov		ebx,[edi]TTextureLinearTransGour.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TTextureLinearTransGour.fxXDifference
		mov		[fx_inc],ebx

		movq	mm0,[edi]TTextureLinearTransGour.indCoord.pfIndex
		mov		[fx_diff],ecx

		mov		ebx,[edi]TTextureLinearTransGour.gourIntensity.fxIntensity
		movq	[pf_texture_inc],mm0

		mov		[fx_gour_inc],ebx

		//
		// Setup:
		//
		// mm2 = qMFactor = width,1,width,1
		// mm3 = qUVMasks
		// mm5 = qShadeSlopes = fxDeltaXIntensity*2,fxDeltaXIntensity*2
		// mm6 = uvslope
		//
		movd		mm5,[fxDeltaXIntensity]

		movd		mm0,[iTexWidth]					// 0,0,0,Width
		punpckldq	mm5,mm5							// fxDeltaXIntensity,fxDeltaXIntensity

		movd		mm2,[u4OneOne]					// 0,0,1,1
		paddd		mm5,mm5							// fxDeltaXIntensity*2,fxDeltaXIntensity*2

		movq		mm3,[qUVMasks]					// Load texture masks
		punpcklwd	mm0,mm0							// 0,0,Width,Width

		movq		mm6,[pfxDeltaTex]				// Load texture slopes.
		punpcklwd	mm2,mm0							// Width,1,Width,1

		movq		mm7,[eax]TTextureLinearTransGour.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		ebp,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TTextureLinearTransGour.fxX.i4Fx

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
		// eax = clut_index1		ebx = texel_index
		// ecx = clut_index2		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = clut_base_ptr
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm4 = shade+1,shade		mm5 = shadeslope*2,shadeslope*2
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		//
		// Setup:
		// QWORD qShadeValue	= fxIntensity+1,fxIntensity
		//
		movd		mm4,[eax]TTextureLinearTransGour.gourIntensity.fxIntensity.i4Fx
		pf2id		(m7,m7)							// Convert texture values to 16.16 fixed.

		movd		mm1,[fxDeltaXIntensity]			// Load intensity delta.
		movq		mm0,mm7							// Copy U1,V1

		punpckldq	mm4,mm4							// Duplicate intensity value.
		paddd		mm7,mm6							// Step U,V

		mov			esi,[pvTextureBitmap]			// Load texture ponter.
		psllq		mm1,32							// Shift to upper dword.

		mov			ebp,[pvClutConversion]			// Load clut base pointer.
		paddd		mm4,mm1							// Add to starting intensity.

		movq		mm1,mm7							// Copy U2,V2
		psrad		mm0,16							// Shift for integer U1,V1

		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values
		movq		mm1,mm4							// Copy shading values

		pand		mm0,mm3							// Mask for tiling
		psrld		mm1,8							// Shift shading values

		paddd		mm4,mm5							// Step shading values
		pmaddwd		mm0,mm2							// Compute texture indices

		movq		[qShadeTemp],mm1				// Save shading indicies

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 8 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16

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
		// mm0 = Texture indices.
		// qShadeTemp = CLUT indices.
		// mm4 is stepped once.
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U1,V1

		mov			al,[esi + ebx]					// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm1,mm7							// Copy U2,V2

		mov			ecx,dword ptr[qShadeTemp + 4]	// Get shade value 2
		psrad		mm1,16							// Shift for integer U2,V2

		mov			cl,[esi + ebx]					// Lookup texel 2
		packssdw	mm0,mm1							// Pack integer texture values.

		pand		mm0,mm3							// Mask for tiling
		movq		mm1,mm4							// Copy shading values.

		psrld		mm1,8							// Shift shading values.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		paddd		mm4,mm5							// Step shading values.
		movq		[qShadeTemp],mm1				// Save shading indicies.

		test		al,al							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT

		mov			ax,word ptr[ebp + eax*2]		// Clut lookup for p1

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT:
		test		cl,cl							// Test for transparency.
		jz			short PIXEL2_TRANSPARENT

		mov			cx,word ptr[ebp + ecx*2]		// Clut lookup for p2

		mov			[edx + edi*2 - 2],cx			// Store pixel 2

PIXEL2_TRANSPARENT:
		add			edi,2							// Inrcement index by 2
		jl			short INNER_LOOP_2P				// Loop (while >2 pixels)

FINISH_REMAINDER:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			short ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture/shade co-ordinates once.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		mov			eax,dword ptr[qShadeTemp]		// Get shade value 1
		mov			al,[esi + ebx]					// Lookup texel 1
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			ecx,dword ptr[qShadeTemp + 4]	// Get shade value 2
		mov			cl,[esi + ebx]					// Lookup texel 2

		test		al,al							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT_END

		mov			ax,word ptr[ebp + eax*2]		// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT_END:
		test		cl,cl							// Test for transparency.
		jz			short END_OF_SCANLINE

		mov			cx,word ptr[ebp + ecx*2]		// Clut lookup for p2
		mov			[edx + edi*2 - 2],cx			// Store pixel 2

		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ecx,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		mov			bl,[esi + ecx]					// Lookup texel 1

		test		bl,bl							// Test for transparency.
		jz			short END_OF_SCANLINE

		mov			bx,word ptr[ebp + ebx*2]		// Clut lookup for p1
		mov			[edx + edi*2 - 4],bx			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TTextureLinearTransGour.indCoord.pfIndex

		mov		ebp,[eax]TTextureLinearTransGour.gourIntensity.fxIntensity.i4Fx

		mov		ebx,[eax]TTextureLinearTransGour.fxXDifference
		pfadd	(m7,m1)

		add		ebp,[fx_gour_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000
		movq	mm1,[pfNegD]

		add		ebp,[fxIModDiv]
		pfadd	(m7,m1)

		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000
		movq	mm1,[pfNegD]

		add		ebp,[fxIModDiv]
		pfadd	(m7,m1)

EDGE_DONE:
		// Store modified variables and do looping.
		// Step length, line starting address.
		mov		esi,[pdtriGlbl]
		mov		[eax]TTextureLinearTransGour.gourIntensity.fxIntensity.i4Fx,ebp

		mov		[eax]TTextureLinearTransGour.fxXDifference,ebx
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		movq	[eax]TTextureLinearTransGour.indCoord.pfIndex,mm7
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx

		add		ecx,gsGlobals.u4LinePixels
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx

		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx
		mov		ecx,[eax]TTextureLinearTransGour.fxX

		mov		ebp,[fx_inc]

		mov		edx,[esi]CDrawPolygonBase.iY
		add		ecx,ebp

		mov		[eax]TTextureLinearTransGour.fxX,ecx
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


//*****************************************************************************************
//
// Gouraud shaded linear texture (16 bit dest).
//
void DrawSubtriangle(TTextureLinearGour* pscan, CDrawPolygon<TTextureLinearGour>* pdtri)
{
	typedef TTextureLinearGour::TPixel prevent_internal_compiler_errors;
	static CDrawPolygon<TTextureLinearGour>* pdtriGlbl;
	static TTextureLinearGour* pscanGlbl;
	static ::fixed fx_gour_inc;
	static PackedFloatUV pf_texture_inc;

	TTextureLinearGour* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.

		mov		esi,[pdtri]							// Pointer to polygon object.
		push	ebp									// Save ebp.

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
		mov		ebx,[edi]TTextureLinearGour.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TTextureLinearGour.fxXDifference
		mov		[fx_inc],ebx

		movq	mm0,[edi]TTextureLinearGour.indCoord.pfIndex
		mov		[fx_diff],ecx

		mov		ebx,[edi]TTextureLinearGour.gourIntensity.fxIntensity
		movq	[pf_texture_inc],mm0

		mov		[fx_gour_inc],ebx

		//
		// Setup:
		//
		// mm2 = qMFactor = width,1,width,1
		// mm3 = qUVMasks
		// mm5 = qShadeSlopes = fxDeltaXIntensity*2,fxDeltaXIntensity*2
		// mm6 = uvslope
		//
		movd		mm5,[fxDeltaXIntensity]

		movd		mm0,[iTexWidth]					// 0,0,0,Width
		punpckldq	mm5,mm5							// fxDeltaXIntensity,fxDeltaXIntensity

		movd		mm2,[u4OneOne]					// 0,0,1,1
		paddd		mm5,mm5							// fxDeltaXIntensity*2,fxDeltaXIntensity*2

		movq		mm3,[qUVMasks]					// Load texture masks
		punpcklwd	mm0,mm0							// 0,0,Width,Width

		movq		mm6,[pfxDeltaTex]				// Load texture slopes.
		punpcklwd	mm2,mm0							// Width,1,Width,1

		movq		mm7,[eax]TTextureLinearGour.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		ebp,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TTextureLinearGour.fxX.i4Fx

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
		// eax = clut_index1		ebx = texel_index
		// ecx = clut_index2		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = clut_base_ptr
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm4 = shade+1,shade		mm5 = shadeslope*2,shadeslope*2
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		//
		// Setup:
		// QWORD qShadeValue	= fxIntensity+1,fxIntensity
		//
		movd		mm4,[eax]TTextureLinearGour.gourIntensity.fxIntensity.i4Fx
		pf2id		(m7,m7)							// Convert texture values to 16.16 fixed.

		movd		mm1,[fxDeltaXIntensity]			// Load intensity delta.
		movq		mm0,mm7							// Copy U1,V1

		punpckldq	mm4,mm4							// Duplicate intensity value.
		paddd		mm7,mm6							// Step U,V

		mov			esi,[pvTextureBitmap]			// Load texture ponter.
		psllq		mm1,32							// Shift to upper dword.

		mov			ebp,[pvClutConversion]			// Load clut base pointer.
		paddd		mm4,mm1							// Add to starting intensity.

		movq		mm1,mm7							// Copy U2,V2
		psrad		mm0,16							// Shift for integer U1,V1

		sub			ebp,4							// Hack to force SIB+offset in loop.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values
		movq		mm1,mm4							// Copy shading values

		pand		mm0,mm3							// Mask for tiling
		psrld		mm1,8							// Shift shading values

		paddd		mm4,mm5							// Step shading values
		pmaddwd		mm0,mm2							// Compute texture indices

		movq		[qShadeTemp],mm1				// Save shading indicies

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 8 past the beginning of a cache line.
		ALIGN	16

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
		// mm0 = Texture indices.
		// qShadeTemp = CLUT indices.
		// mm4 is stepped once.
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U1,V1

		mov			al,[esi + ebx]					// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm1,mm7							// Copy U2,V2

		mov			ecx,dword ptr[qShadeTemp + 4]	// Get shade value 2
		psrad		mm1,16							// Shift for integer U2,V2

		mov			cl,[esi + ebx]					// Lookup texel 2
		packssdw	mm0,mm1							// Pack integer texture values.

		pand		mm0,mm3							// Mask for tiling
		movq		mm1,mm4							// Copy shading values.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup for p1
		psrld		mm1,8							// Shift shading values.

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qShadeTemp],mm1				// Save shading indicies.

		movzx		ecx,word ptr[ebp + ecx*2 + 4]	// Clut lookup for p2
		paddd		mm4,mm5							// Step shading values.

		mov			[edx + edi*2 - 2],cx			// Store pixel 2

		add			edi,2							// Inrcement index by 2
		jl			short INNER_LOOP_2P				// Loop (while >2 pixels)

FINISH_REMAINDER:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			short ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture/shade co-ordinates once.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		mov			eax,dword ptr[qShadeTemp]		// Get shade value 1
		mov			al,[esi + ebx]					// Lookup texel 1
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			ecx,dword ptr[qShadeTemp + 4]	// Get shade value 2
		mov			cl,[esi + ebx]					// Lookup texel 2
		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		movzx		ecx,word ptr[ebp + ecx*2 + 4]	// Clut lookup for p2
		mov			[edx + edi*2 - 2],cx			// Store pixel 2
		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ecx,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		mov			bl,[esi + ecx]					// Lookup texel 1
		mov			bx,word ptr[ebp + ebx*2 + 4]	// Clut lookup for p1
		mov			[edx + edi*2 - 4],bx			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TTextureLinearGour.indCoord.pfIndex

		mov		ebp,[eax]TTextureLinearGour.gourIntensity.fxIntensity.i4Fx

		mov		ebx,[eax]TTextureLinearGour.fxXDifference
		pfadd	(m7,m1)

		add		ebp,[fx_gour_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000
		movq	mm1,[pfNegD]

		add		ebp,[fxIModDiv]
		pfadd	(m7,m1)

		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000
		movq	mm1,[pfNegD]

		add		ebp,[fxIModDiv]
		pfadd	(m7,m1)

EDGE_DONE:
		// Store modified variables and do looping.
		// Step length, line starting address.
		mov		esi,[pdtriGlbl]
		mov		[eax]TTextureLinearGour.gourIntensity.fxIntensity.i4Fx,ebp

		mov		[eax]TTextureLinearGour.fxXDifference,ebx
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		movq	[eax]TTextureLinearGour.indCoord.pfIndex,mm7
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx

		add		ecx,gsGlobals.u4LinePixels
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx

		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx
		mov		ecx,[eax]TTextureLinearGour.fxX

		mov		ebp,[fx_inc]

		mov		edx,[esi]CDrawPolygonBase.iY
		add		ecx,ebp

		mov		[eax]TTextureLinearGour.fxX,ecx
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

// Restore default section.
#pragma code_seg()
