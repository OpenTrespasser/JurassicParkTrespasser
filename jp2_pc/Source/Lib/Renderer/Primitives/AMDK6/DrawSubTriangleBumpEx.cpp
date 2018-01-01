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
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/DrawSubTriangleBumpEx.cpp               $
 * 
 * 10    10/01/98 12:39a Asouth
 * Added #ifdefs for MW build; explicit scope of fixed
 * 
 * 9     9/11/98 5:13p Mmouni
 * Optimized to avoid degraded pre-decode.
 * 
 * 8     9/10/98 10:34p Mmouni
 * Made solid primitives write qword aligned qwords.
 * 
 * 7     98.01.22 3:15p Mmouni
 * Added support for 16-bit deep bump-maps.
 * 
 * 6     97.11.04 10:59p Mmouni
 * Linear version of primitives now use 3dx/mmx.
 * 
 * 5     97.10.30 1:39p Mmouni
 * All perspective sub-triangle routines are now 3DX/MMX.
 * 
 * 4     97.10.27 1:25p Mmouni
 * Initial K6-3D version, copied from the Pentium versions.
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


// Put this stuff in a seperate section for alignment.
#pragma code_seg("StriBump")


#if (iBUMPMAP_RESOLUTION == 16)

//*****************************************************************************************
//
// Bumpmapped, Perspective correct and Transparent for 16 bit screen.
//
void DrawSubtriangle(TBumpPerspTrans* pscan, CDrawPolygon<TBumpPerspTrans>* pdtri)
{
	typedef TBumpPerspTrans::TIndex prevent_internal_compiler_errors;
	static void *pvTextureBitmapAdj;
	::fixed fx_inc;
	::fixed fx_diff;

	TBumpPerspTrans* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[eax]TBumpPerspTrans.fxX

		mov		edx,[eax]TBumpPerspTrans.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TBumpPerspTrans.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TBumpPerspTrans.indCoord.fInvZ
		movq	[tvEdgeStep.UInvZ],mm0

		mov		[tvEdgeStep.InvZ],ebx

		// Create offset base pointer.
		mov			ebx,[pvTextureBitmap]			// Base of texture.

		sub			ebx,4							// Adjust by -4.
		mov			[pvTextureBitmapAdj],ebx		// Base of texture - 4.

		//
		// Setup:
		// QWORD qMFactor		= width,1,width,1
		// Load global texture values. 
		//
		movd		mm3,[iTexWidth]					// 0,0,0,Width

		movd		mm4,[u4OneOne]					// 0,0,1,1
		punpcklwd	mm3,mm3							// 0,0,Width,Width

		punpcklwd	mm4,mm3										// Width,1,Width,1
		movq		mm0,[ecx]TBumpPerspTrans.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4								// Save mfactor
		movd		mm1,[ecx]TBumpPerspTrans.indCoord.fInvZ		// mm1 = (?,1/Z)

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
	#ifdef __MWERKS__
		#define SLTYPE TBumpPerspTrans
		#include "psprologue3.inl"
	#else
		PERSPECTIVE_SCANLINE_PROLOGUE_3DX(TBumpPerspTrans)
	#endif

SUBDIVISION_LOOP:
		//--------------------------------------------------------------------------- 
		// Draw the subdivision.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel1 temp		ebx = texel2 temp
		// ecx = texel1	temp		edx = dest_base_ptr
		// esi = texure base/temp	edi = pixel_offset & count
		// ebp = clut/bump temp
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm4 = ???				mm5 = ???
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm6							// Step U,V

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		movq		mm3,[qUVMasks]					// Load texture masks

		packssdw	mm0,mm1							// Pack integer texture values
		movq		mm2,[qMFactor]					// Load texture multipliers

		pand		mm0,mm3							// Mask for tiling
		mov			esi,edx							// Move destination base to esi.

		pmaddwd		mm0,mm2							// Compute texture indici
		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.

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
		ALIGN	16
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

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 1.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 2.
		paddd		mm7,mm6							// Step U,V

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 1 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 2 color
		sub			ecx,ebp							// Subtract light theta 1.

		sub			edx,ebp							// Subtract light theta 2.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		movq		mm1,mm7							// Copy U2,V2

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 2.
		psrad		mm1,16							// Shift for integer U2,V2
	
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		pand		mm0,mm3							// Mask for tiling

		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		test		al,0xfc							// test for transparent (long decode)

		jz			short PIXEL1_TRANS
		shr			eax,2							// Shift off lower 2 bits.

		mov			ax,[ebp + eax*2]				// Clut lookup 1.
		mov			[esi + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANS:
		test		bl,0xfc							// test for transparent (long decode)

		jz			short PIXEL2_TRANS
		shr			ebx,2							// Shift off lower 2 bits.

		nop											// For cache alignment.
		nop

		mov			bx,[ebp + ebx*2]				// Clut lookup 2.
		mov			[esi + edi*2 - 2],bx			// Store pixel 2

PIXEL2_TRANS:
		add			edi,2							// Increment index by 2
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
		paddd		mm7,mm6							// Step U,V
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 1.
		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 2.
		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax
		mov			al,ch							// Copy texel 1 color
		mov			ebx,0							// Clear ebx
		mov			bl,dh							// Copy texel 2 color
		sub			ecx,ebp							// Subtract light theta 1.
		sub			edx,ebp							// Subtract light theta 2.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 2.
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.

		test		al,0xfc							// test for transparent (long decode)

		jz			short PIXEL1_TRANS_END
		shr			eax,2							// Shift off lower 2 bits.

		mov			ax,[ebp + eax*2]				// Clut lookup 1.
		mov			[esi + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANS_END:
		test		bl,0xfc							// test for transparent (long decode)

		jz			short PIXEL2_TRANS_END
		shr			ebx,2							// Shift off lower 2 bits.

		mov			bx,[ebp + ebx*2]				// Clut lookup 2.
		mov			[esi + edi*2 - 2],bx			// Store pixel 2

PIXEL2_TRANS_END:
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ecx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			ebx,0							// Clear eax
		mov			eax,[i_pixel]					// Load remaining length.
		mov			bl,ch							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		mov			bh,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		mov			ebp,[pvClutConversion]			// Base of CLUT.

		test		bl,0xfc							// test for transparent (long decode)

		jz			short END_OF_SPAN
		shr			ebx,2							// Shift off lower 2 bits.

		mov			bx,[ebp + ebx*2]				// Clut lookup 1.
		mov			[esi + edi*2 - 4],bx			// Store pixel 1

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
		pop		ebp

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

		mov		eax,[ecx]TBumpPerspTrans.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TBumpPerspTrans.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TBumpPerspTrans.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TBumpPerspTrans.indCoord.fInvZ

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
		movq	[ecx]TBumpPerspTrans.indCoord.fUInvZ,mm0

		movd	[ecx]TBumpPerspTrans.indCoord.fInvZ,mm1

		mov		[ecx]TBumpPerspTrans.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TBumpPerspTrans.fxXDifference.i4Fx,ebx
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
// Bumpmapped and Perspective correct for 16 bit screen.
//
void DrawSubtriangle(TBumpPersp* pscan, CDrawPolygon<TBumpPersp>* pdtri)
{
	typedef TBumpPersp::TIndex prevent_internal_compiler_errors;
	static void *pvClutConversionAdj;
	static void *pvTextureBitmapAdj;
	::fixed fx_inc;
	::fixed fx_diff;

	TBumpPersp* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[eax]TBumpPersp.fxX

		mov		edx,[eax]TBumpPersp.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TBumpPersp.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TBumpPersp.indCoord.fInvZ
		movq	[tvEdgeStep.UInvZ],mm0

		mov		[tvEdgeStep.InvZ],ebx

		// Create offset base pointers.
		mov			eax,[pvClutConversion]			// Base of CLUT.

		mov			ebx,[pvTextureBitmap]			// Base of texture.
		sub			eax,4							// Adjust by -4.

		mov			[pvClutConversionAdj],eax		// Base of CLUT - 4.
		sub			ebx,4							// Adjust by -4.

		mov			[pvTextureBitmapAdj],ebx		// Base of texture - 4.

		//
		// Setup:
		// QWORD qMFactor		= width,1,width,1
		// Load global texture values. 
		//
		movd		mm3,[iTexWidth]					// 0,0,0,Width

		movd		mm4,[u4OneOne]					// 0,0,1,1
		punpcklwd	mm3,mm3							// 0,0,Width,Width

		punpcklwd	mm4,mm3									// Width,1,Width,1
		movq		mm0,[ecx]TBumpPersp.indCoord.fUInvZ		// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4							// Save mfactor
		movd		mm1,[ecx]TBumpPersp.indCoord.fInvZ		// mm1 = (?,1/Z)

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
	#ifdef __MWERKS__
		#define SLTYPE TBumpPersp
		#include "psprologue3.inl"
	#else
		PERSPECTIVE_SCANLINE_PROLOGUE_3DX(TBumpPersp)
	#endif

SUBDIVISION_LOOP:
		//--------------------------------------------------------------------------- 
		// Draw the subdivision.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel1 temp		ebx = texel2 temp
		// ecx = texel1	temp		edx = texture_ptr/texel2 temp
		// esi = base of scanline	edi = pixel_offset & count
		// ebp = clut_ptr/bump_ptr/temp
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm4 = ?					mm5 = ?
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm6							// Step U,V

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		movq		mm3,[qUVMasks]					// Load texture masks

		packssdw	mm0,mm1							// Pack integer texture values
		movq		mm2,[qMFactor]					// Load texture multipliers

		pand		mm0,mm3							// Mask for tiling
		mov			esi,edx							// Move destination base to esi.

		pmaddwd		mm0,mm2							// Compute texture indici
		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.

		movq		[qIndexTemp],mm0				// Save texture indicies.

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
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 1.
		movq		mm0,mm7							// Copy U1,V1

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 1.

		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		psrad		mm0,16							// Shift for integer U1,V1

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		packssdw	mm1,mm0							// Pack integer texture values.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		movq		mm0,mm1							// Move packed values back into mm0.

		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		pand		mm0,mm3							// Mask for tiling

		shr			eax,2							// Shift off lower 2 bits.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 1.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			[esi + edi*2 - 2],ax			// Store pixel 1

ALIGNED2:
		and			eax,7
		jz			ALIGNED4

		add			edi,2							// Two pixels left?
		jge			FINISH_REMAINDER2

		//
		// Do two pixels for alignment.
		//
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		paddd		mm7,mm6							// Step U,V

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
		psrad		mm1,16							// Shift for integer U2,V2
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		pand		mm0,mm3							// Mask for tiling

		shr			eax,2							// Shift off lower 2 bits.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[esi + edi*2 - 4],ebx			// Store pixels

ALIGNED4:
		add		edi,4		
		jl		INNER_LOOP_4P					// Four pixles left?

		jmp		FINISH_REMAINDER4

		// Align start of loop to 4 past the beginning of a cache line.
		ALIGN	16
		nop
		nop
		nop
		nop

		//
		// Do four pixels at a time.
		//
		// qIndexTemp = texture indices.
		// mm7 is stepped once.
		//
INNER_LOOP_4P:
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		paddd		mm7,mm6							// Step U,V

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
		psrad		mm1,16							// Shift for integer U2,V2
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		pand		mm0,mm3							// Mask for tiling

		shr			eax,2							// Shift off lower 2 bits.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movd		mm4,ebx							// Copy into MMX reg.

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.
		movq		mm0,mm7							// Copy U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		mov			ebp,[u4LightTheta]				// Load light theta.

		psrad		mm0,16							// Shift for integer U1,V1
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.
		paddd		mm7,mm6							// Step U,V

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
		movq		mm1,mm7							// Copy U2,V2
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		psrad		mm1,16							// Shift for integer U2,V2

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		packssdw	mm0,mm1							// Pack integer texture values.

		shr			eax,2							// Shift off lower 2 bits.
		pand		mm0,mm3							// Mask for tiling

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		or			ebx,eax							// Combine pixels.
		movd		mm5,ebx							// Copy into MMX reg.

		punpckldq	mm4,mm5							// Combine all four pixels.
		movq		[esi + edi*2 - 8],mm4			// Store pixels

		add			edi,4							// Increment index by 4
		jl			INNER_LOOP_4P					// Loop (while >4 pixels)

FINISH_REMAINDER4:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		jnz			THREE_OR_LESS

		//
		// Four pixels left, step co-ordinates 3 times.
		//
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		paddd		mm7,mm6							// Step U,V

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
		psrad		mm1,16							// Shift for integer U2,V2
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		pand		mm0,mm3							// Mask for tiling

		shr			eax,2							// Shift off lower 2 bits.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movd		mm4,ebx							// Copy into MMX reg.

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.

		shr			eax,2							// Shift off lower 2 bits.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.

		shl			eax,16							// Shift pixel 2 up.

		or			ebx,eax							// Combine pixels.
		movd		mm5,ebx							// Copy into MMX reg.

		punpckldq	mm4,mm5							// Combine all four pixels.
		movq		[esi - 8],mm4					// Store pixels

		mov			eax,[i_pixel]					// Load remaining length.
		jmp			END_OF_SPAN


THREE_OR_LESS:
		sub			edi,2
		jge			FINISH_REMAINDER2

		//
		// Three pixels left, step co-ordinates twice.
		//
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		paddd		mm7,mm6							// Step U,V

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
		psrad		mm1,16							// Shift for integer U2,V2
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		pand		mm0,mm3							// Mask for tiling

		shr			eax,2							// Shift off lower 2 bits.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[esi - 6],ebx					// Store pixels

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 1.

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 1.

		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.

		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.

		shr			eax,2							// Shift off lower 2 bits.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 1.

		mov			[esi - 2],ax					// Store pixel 1

		mov			eax,[i_pixel]					// Load remaining length.
		jmp			END_OF_SPAN

FINISH_REMAINDER2:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture co-ordinates once.
		//
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.

		shr			eax,2							// Shift off lower 2 bits.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[esi  - 4],ebx					// Store pixels

		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 1.
		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax
		mov			al,ch							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		shr			eax,2							// Shift off lower 2 bits.
		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 1.
		mov			[esi - 2],ax					// Store pixel 1
		mov			eax,[i_pixel]					// Load remaining length.

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		jz		END_OF_SCANLINE

		//
		// Do perspective correction for next span.
	#ifdef __MWERKS__
		#include "psepilogue3.inl"
	#else
		PERSPECTIVE_SCANLINE_EPILOGUE_3DX
	#endif

		jmp		SUBDIVISION_LOOP

END_OF_SCANLINE:
		pop		ebp

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

		mov		eax,[ecx]TBumpPersp.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TBumpPersp.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TBumpPersp.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TBumpPersp.indCoord.fInvZ

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
		movq	[ecx]TBumpPersp.indCoord.fUInvZ,mm0

		movd	[ecx]TBumpPersp.indCoord.fInvZ,mm1

		mov		[ecx]TBumpPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TBumpPersp.fxXDifference.i4Fx,ebx
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
// Bumpmapped, Linear and transparent for 16 bit screen.
//
void DrawSubtriangle(TBumpLinearTrans* pscan, CDrawPolygon<TBumpLinearTrans>* pdtri)
{
	typedef TBumpLinearTrans::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TBumpLinearTrans>* pdtriGlbl;
	static TBumpLinearTrans* pscanGlbl;
	static PackedFloatUV pf_texture_inc;
	static void *pvTextureBitmapAdj;

	TBumpLinearTrans* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[edi]TBumpLinearTrans.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TBumpLinearTrans.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TBumpLinearTrans.indCoord.pfIndex
		mov		fx_diff,ecx

		movq	[pf_texture_inc],mm0

		// Create offset base pointers.
		mov			edx,[pvTextureBitmap]			// Base of texture.

		sub			edx,4							// Adjust by -4.
		mov			[pvTextureBitmapAdj],edx		// Base of texture - 4.

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

		movq		mm7,[eax]TBumpLinearTrans.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TBumpLinearTrans.fxX.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		mov		edx,gsGlobals.pvScreen

		add		ebx,ecx
		sar		edi,16

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex
		sar		ebx,16

		add		ecx,ebx

		lea		esi,[edx + ecx*2]

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
		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.

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
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm0,mm7							// Copy U1,V1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 1.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 2.
		paddd		mm7,mm6							// Step U,V

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 1 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 2 color
		sub			ecx,ebp							// Subtract light theta 1.

		sub			edx,ebp							// Subtract light theta 2.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		movq		mm1,mm7							// Copy U2,V2

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 2.
		psrad		mm1,16							// Shift for integer U2,V2
	
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		pand		mm0,mm3							// Mask for tiling

		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		test		al,0xfc							// test for transparent (long decode)

		jz			short PIXEL1_TRANS
		shr			eax,2							// Shift off lower 2 bits.

		mov			ax,[ebp + eax*2]				// Clut lookup 1.
		mov			[esi + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANS:
		test		bl,0xfc							// test for transparent (long decode)

		jz			short PIXEL2_TRANS
		shr			ebx,2							// Shift off lower 2 bits.

		nop											// For cache alignment.
		nop

		mov			bx,[ebp + ebx*2]				// Clut lookup 2.
		mov			[esi + edi*2 - 2],bx			// Store pixel 2

PIXEL2_TRANS:
		add			edi,2							// Increment index by 2
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
		paddd		mm7,mm6							// Step U,V
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 1.
		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 2.
		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax
		mov			al,ch							// Copy texel 1 color
		mov			ebx,0							// Clear ebx
		mov			bl,dh							// Copy texel 2 color
		sub			ecx,ebp							// Subtract light theta 1.
		sub			edx,ebp							// Subtract light theta 2.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 2.
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.

		test		al,0xfc							// test for transparent (long decode)

		jz			short PIXEL1_TRANS_END
		shr			eax,2							// Shift off lower 2 bits.

		mov			ax,[ebp + eax*2]				// Clut lookup 1.
		mov			[esi + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANS_END:
		test		bl,0xfc							// test for transparent (long decode)

		jz			short PIXEL2_TRANS_END
		shr			ebx,2							// Shift off lower 2 bits.

		mov			bx,[ebp + ebx*2]				// Clut lookup 2.
		mov			[esi + edi*2 - 2],bx			// Store pixel 2

PIXEL2_TRANS_END:
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ecx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			ebx,0							// Clear eax
		mov			eax,[i_pixel]					// Load remaining length.
		mov			bl,ch							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		mov			bh,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		mov			ebp,[pvClutConversion]			// Base of CLUT.

		test		bl,0xfc							// test for transparent (long decode)

		jz			short END_OF_SCANLINE
		shr			ebx,2							// Shift off lower 2 bits.

		mov			bx,[ebp + ebx*2]				// Clut lookup 1.
		mov			[esi + edi*2 - 4],bx			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TBumpLinearTrans.indCoord.pfIndex

		mov		ebx,[eax]TBumpLinearTrans.fxXDifference

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
		mov		[eax]TBumpLinearTrans.fxXDifference,ebx

		movq	[eax]TBumpLinearTrans.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TBumpLinearTrans.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TBumpLinearTrans.fxX,ecx
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
// Bumpmapped and Linear for 16 bit screen.
//
void DrawSubtriangle(TBumpLinear* pscan, CDrawPolygon<TBumpLinear>* pdtri)
{
	typedef TBumpLinear::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TBumpLinear>* pdtriGlbl;
	static TBumpLinear* pscanGlbl;
	static PackedFloatUV pf_texture_inc;
	static void *pvClutConversionAdj;
	static void *pvTextureBitmapAdj;

	TBumpLinear* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[edi]TBumpLinear.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TBumpLinear.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TBumpLinear.indCoord.pfIndex
		mov		fx_diff,ecx

		movq	[pf_texture_inc],mm0

		// Create offset base pointers.
		mov			ebx,[pvClutConversion]			// Base of CLUT.

		mov			edx,[pvTextureBitmap]			// Base of texture.
		sub			ebx,4							// Adjust by -4.

		mov			[pvClutConversionAdj],ebx		// Base of CLUT - 4.
		sub			edx,4							// Adjust by -4.

		mov			[pvTextureBitmapAdj],edx		// Base of texture - 4.

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

		movq		mm7,[eax]TBumpLinear.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		END_OF_SCANLINE

		mov		ebx,[eax]TBumpLinear.fxX.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		mov		edx,gsGlobals.pvScreen

		add		ebx,ecx
		sar		edi,16

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex
		sar		ebx,16

		add		ecx,ebx

		lea		esi,[edx + ecx*2]

		sub		edi,ebx
		jge		END_OF_SCANLINE						// no pixels to draw

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// eax = texel1 temp		ebx = texel2 temp
		// ecx = texel1	temp		edx = texture_ptr/texel2 temp
		// esi = base of scanline	edi = pixel_offset & count
		// ebp = clut_ptr/bump_ptr/temp
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm4 = ?					mm5 = ?
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		pf2id		(m7,m7)							// Convert texture values to 16.16 fixed.
		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.

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
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 1.
		movq		mm0,mm7							// Copy U1,V1

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 1.

		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		psrad		mm0,16							// Shift for integer U1,V1

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		packssdw	mm1,mm0							// Pack integer texture values.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		movq		mm0,mm1							// Move packed values back into mm0.

		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		pand		mm0,mm3							// Mask for tiling

		shr			eax,2							// Shift off lower 2 bits.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 1.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			[esi + edi*2 - 2],ax			// Store pixel 1

ALIGNED2:
		and			eax,7
		jz			ALIGNED4

		add			edi,2							// Two pixels left?
		jge			FINISH_REMAINDER2

		//
		// Do two pixels for alignment.
		//
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		paddd		mm7,mm6							// Step U,V

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
		psrad		mm1,16							// Shift for integer U2,V2
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		pand		mm0,mm3							// Mask for tiling

		shr			eax,2							// Shift off lower 2 bits.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[esi + edi*2 - 4],ebx			// Store pixels

ALIGNED4:
		add		edi,4		
		jl		INNER_LOOP_4P						// Four pixles left?

		jmp		FINISH_REMAINDER4

		// Align start of loop to 4 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16
		nop
		nop
		nop
		nop

		//
		// Do four pixels at a time.
		//
		// qIndexTemp = texture indices.
		// mm7 is stepped once.
		//
INNER_LOOP_4P:
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		paddd		mm7,mm6							// Step U,V

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
		psrad		mm1,16							// Shift for integer U2,V2
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		pand		mm0,mm3							// Mask for tiling

		shr			eax,2							// Shift off lower 2 bits.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movd		mm4,ebx							// Copy into MMX reg.

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.
		movq		mm0,mm7							// Copy U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		mov			ebp,[u4LightTheta]				// Load light theta.

		psrad		mm0,16							// Shift for integer U1,V1
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.
		paddd		mm7,mm6							// Step U,V

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
		movq		mm1,mm7							// Copy U2,V2
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		psrad		mm1,16							// Shift for integer U2,V2

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		packssdw	mm0,mm1							// Pack integer texture values.

		shr			eax,2							// Shift off lower 2 bits.
		pand		mm0,mm3							// Mask for tiling

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		or			ebx,eax							// Combine pixels.
		movd		mm5,ebx							// Copy into MMX reg.

		punpckldq	mm4,mm5							// Combine all four pixels.
		movq		[esi + edi*2 - 8],mm4			// Store pixels

		add			edi,4							// Increment index by 4
		jl			INNER_LOOP_4P					// Loop (while >4 pixels)

FINISH_REMAINDER4:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		jnz			THREE_OR_LESS

		//
		// Four pixels left, step co-ordinates twice.
		//
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		paddd		mm7,mm6							// Step U,V

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
		psrad		mm1,16							// Shift for integer U2,V2
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		pand		mm0,mm3							// Mask for tiling

		shr			eax,2							// Shift off lower 2 bits.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movd		mm4,ebx							// Copy into MMX reg.

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.

		shr			eax,2							// Shift off lower 2 bits.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.

		shl			eax,16							// Shift pixel 2 up.

		or			ebx,eax							// Combine pixels.
		movd		mm5,ebx							// Copy into MMX reg.

		punpckldq	mm4,mm5							// Combine all four pixels.
		movq		[esi - 8],mm4					// Store pixels

		jmp			END_OF_SCANLINE


THREE_OR_LESS:
		sub			edi,2
		jge			FINISH_REMAINDER2

		//
		// Three pixels left, step co-ordinates twice.
		//
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.
		psrad		mm0,16							// Shift for integer U1,V1

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.
		paddd		mm7,mm6							// Step U,V

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
		psrad		mm1,16							// Shift for integer U2,V2
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.
		pand		mm0,mm3							// Mask for tiling

		shr			eax,2							// Shift off lower 2 bits.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[esi - 6],ebx					// Store pixels

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 1.

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 1.

		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.

		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.

		shr			eax,2							// Shift off lower 2 bits.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 1.

		mov			[esi - 2],ax					// Store pixel 1

		jmp			END_OF_SCANLINE

FINISH_REMAINDER2:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			ONE_PIXEL_LEFT

		//
		// Two pixels left, don't step texture co-ordinates.
		//
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		paddd		mm7,mm6							// Step U,V

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1

		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 2.

		movzx		edx,word ptr[edx + ebx*2 + 4]	// Lookup texel 1.

		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax

		mov			al,ch							// Copy texel 2 color
		mov			ebx,0							// Clear ebx

		mov			bl,dh							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 2.

		sub			edx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.

		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		and			edx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 2.

		mov			bh,byte ptr[ebp + edx]			// Bump map intensity lookup 1.
	
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.

		mov			edx,[pvTextureBitmapAdj]		// Base of texture - 4.

		shr			eax,2							// Shift off lower 2 bits.

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 2.
		shr			ebx,2							// Shift off lower 2 bits.

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 1.

		shl			eax,16							// Shift pixel 2 up.
		or			ebx,eax							// Combine pixels.

		mov			[esi  - 4],ebx					// Store pixels

		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ecx,word ptr[edx + eax*2 + 4]	// Lookup texel 1.
		mov			ebp,[u4LightTheta]				// Load light theta.
		mov			eax,0							// Clear eax
		mov			al,ch							// Copy texel 1 color
		sub			ecx,ebp							// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		mov			ebp,[pvClutConversionAdj]		// Base of CLUT - 4.
		shr			eax,2							// Shift off lower 2 bits.
		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 1.
		mov			[esi - 2],ax					// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TBumpLinear.indCoord.pfIndex

		mov		ebx,[eax]TBumpLinear.fxXDifference

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
		mov		[eax]TBumpLinear.fxXDifference,ebx

		movq	[eax]TBumpLinear.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TBumpLinear.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TBumpLinear.fxX,ecx
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


#elif (iBUMPMAP_RESOLUTION == 32)	// if (iBUMPMAP_RESOLUTION == 16)


//*****************************************************************************************
//
// Bumpmapped, Perspective correct and Transparent for 16 bit screen.
//
void DrawSubtriangle(TBumpPerspTrans* pscan, CDrawPolygon<TBumpPerspTrans>* pdtri)
{
	typedef TBumpPerspTrans::TIndex prevent_internal_compiler_errors;
	::fixed fx_inc;
	::fixed fx_diff;

	TBumpPerspTrans* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[eax]TBumpPerspTrans.fxX

		mov		edx,[eax]TBumpPerspTrans.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TBumpPerspTrans.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TBumpPerspTrans.indCoord.fInvZ
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
		movq		mm0,[ecx]TBumpPerspTrans.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4								// Save mfactor
		movd		mm1,[ecx]TBumpPerspTrans.indCoord.fInvZ		// mm1 = (?,1/Z)

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
	#ifdef __MWERKS__
		#define SLTYPE TBumpPerspTrans
		#include "psprologue3.inl"
	#else
		PERSPECTIVE_SCANLINE_PROLOGUE_3DX(TBumpPerspTrans)
	#endif

SUBDIVISION_LOOP:
		//--------------------------------------------------------------------------- 
		// Draw the subdivision.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel1 temp		ebx = texel2 temp
		// ecx = texel1	temp		edx = dest_base_ptr
		// esi = texure base/temp	edi = pixel_offset & count
		// ebp = clut/bump temp
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm4 = ???				mm5 = ???
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm6							// Step U,V

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		movq		mm3,[qUVMasks]					// Load texture masks

		packssdw	mm0,mm1							// Pack integer texture values
		movq		mm2,[qMFactor]					// Load texture multipliers

		pand		mm0,mm3							// Mask for tiling

		pmaddwd		mm0,mm2							// Compute texture indici

		movq		[qIndexTemp],mm0				// Save texture indicies.

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 9 past the beginning of a cache line.
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

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		and			esi,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.

		mov			bh,byte ptr[ebp + esi]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2
	
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.

		mov			esi,[pvTextureBitmap]			// Base of texture.
		pand		mm0,mm3							// Mask for tiling

		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		test		al,al							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT

		mov			ax,word ptr[ebp + eax*2]		// Clut lookup 1.

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT:
		test		bl,bl							// Test for transparency.
		jz			short PIXEL2_TRANSPARENT

		mov			bx,word ptr[ebp + ebx*2]		// Clut lookup 2.

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
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V
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
		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		and			esi,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		mov			bh,byte ptr[ebp + esi]			// Bump map intensity lookup 2.
		mov			ebp,[pvClutConversion]			// Base of CLUT.

		test		al,al							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT_END

		mov			ax,word ptr[ebp + eax*2]		// Clut lookup 1.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT_END:
		test		bl,bl							// Test for transparency.
		jz			short PIXEL2_TRANSPARENT_END

		mov			bx,word ptr[ebp + ebx*2]		// Clut lookup 2.
		mov			[edx + edi*2 - 2],bx			// Store pixel 2

PIXEL2_TRANSPARENT_END:
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ecx,dword ptr[esi + ebx*4]		// Lookup texel 1.
		mov			ebx,ecx							// Copy texel 1
		sub			ecx,[u4LightTheta]				// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		shr			ebx,24							// Shift color value down.
		mov			eax,[i_pixel]					// Load remaining length.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		mov			bh,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		mov			ebp,[pvClutConversion]			// Base of CLUT.

		test		bl,bl							// Test for transparency.
		jz			short END_OF_SPAN

		mov			bx,word ptr[ebp + ebx*2]		// Clut lookup 1.
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
		pop		ebp

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

		mov		eax,[ecx]TBumpPerspTrans.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TBumpPerspTrans.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TBumpPerspTrans.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TBumpPerspTrans.indCoord.fInvZ

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
		movq	[ecx]TBumpPerspTrans.indCoord.fUInvZ,mm0

		movd	[ecx]TBumpPerspTrans.indCoord.fInvZ,mm1

		mov		[ecx]TBumpPerspTrans.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TBumpPerspTrans.fxXDifference.i4Fx,ebx
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
// Bumpmapped and Perspective correct for 16 bit screen.
//
void DrawSubtriangle(TBumpPersp* pscan, CDrawPolygon<TBumpPersp>* pdtri)
{
	typedef TBumpPersp::TIndex prevent_internal_compiler_errors;
	::fixed fx_inc;
	::fixed fx_diff;

	TBumpPersp* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[eax]TBumpPersp.fxX

		mov		edx,[eax]TBumpPersp.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TBumpPersp.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TBumpPersp.indCoord.fInvZ
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

		punpcklwd	mm4,mm3									// Width,1,Width,1
		movq		mm0,[ecx]TBumpPersp.indCoord.fUInvZ		// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4							// Save mfactor
		movd		mm1,[ecx]TBumpPersp.indCoord.fInvZ		// mm1 = (?,1/Z)

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
	#ifdef __MWERKS__
		#define SLTYPE TBumpPersp
		#include "psprologue3.inl"
	#else
		PERSPECTIVE_SCANLINE_PROLOGUE_3DX(TBumpPersp)
	#endif

SUBDIVISION_LOOP:
		//--------------------------------------------------------------------------- 
		// Draw the subdivision.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
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
		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm6							// Step U,V

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		movq		mm3,[qUVMasks]					// Load texture masks

		packssdw	mm0,mm1							// Pack integer texture values
		movq		mm2,[qMFactor]					// Load texture multipliers

		pand		mm0,mm3							// Mask for tiling

		pmaddwd		mm0,mm2							// Compute texture indici

		movq		[qIndexTemp],mm0				// Save texture indicies.

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 9 past the beginning of a cache line.
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

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		and			esi,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.

		mov			bh,byte ptr[ebp + esi]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2
	
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		sub			ebp,4							// Force offset for movzx (was a nop)

		mov			esi,[pvTextureBitmap]			// Base of texture.
		pand		mm0,mm3							// Mask for tiling

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 1.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 2.
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
		paddd		mm7,mm6							// Step U,V
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
		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		and			esi,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		mov			bh,byte ptr[ebp + esi]			// Bump map intensity lookup 2.
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		dec			ebp								// Force offset for movzx (was a nop)
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup 1.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup 2.
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ecx,dword ptr[esi + ebx*4]		// Lookup texel 1.
		mov			ebx,ecx							// Copy texel 1
		sub			ecx,[u4LightTheta]				// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		shr			ebx,24							// Shift color value down.
		mov			eax,[i_pixel]					// Load remaining length.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		mov			bh,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		dec			ebp								// Force offset for movzx (was a nop)
		mov			bx,word ptr[ebp + ebx*2 + 1]	// Clut lookup 1.
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
		pop		ebp

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

		mov		eax,[ecx]TBumpPersp.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TBumpPersp.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TBumpPersp.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TBumpPersp.indCoord.fInvZ

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
		movq	[ecx]TBumpPersp.indCoord.fUInvZ,mm0

		movd	[ecx]TBumpPersp.indCoord.fInvZ,mm1

		mov		[ecx]TBumpPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TBumpPersp.fxXDifference.i4Fx,ebx
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
// Bumpmapped, Linear and transparent for 16 bit screen.
//
void DrawSubtriangle(TBumpLinearTrans* pscan, CDrawPolygon<TBumpLinearTrans>* pdtri)
{
	typedef TBumpLinearTrans::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TBumpLinearTrans>* pdtriGlbl;
	static TBumpLinearTrans* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TBumpLinearTrans* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[edi]TBumpLinearTrans.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TBumpLinearTrans.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TBumpLinearTrans.indCoord.pfIndex
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

		movq		mm7,[eax]TBumpLinearTrans.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TBumpLinearTrans.fxX.i4Fx

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

		// Align start of loop to 9 past the beginning of a cache line.
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

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		and			esi,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.

		mov			bh,byte ptr[ebp + esi]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2
	
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.

		mov			esi,[pvTextureBitmap]			// Base of texture.
		pand		mm0,mm3							// Mask for tiling

		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.

		test		al,al							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT

		mov			ax,word ptr[ebp + eax*2]		// Clut lookup 1.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT:
		test		bl,bl							// Test for transparency.
		jz			short PIXEL2_TRANSPARENT

		mov			bx,word ptr[ebp + ebx*2]		// Clut lookup 2.
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
		// Two pixels left.
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
		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		and			esi,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		mov			bh,byte ptr[ebp + esi]			// Bump map intensity lookup 2.
		mov			ebp,[pvClutConversion]			// Base of CLUT.

		test		al,al							// Test for transparency.
		jz			short PIXEL1_TRANSPARENT_END

		mov			ax,word ptr[ebp + eax*2]		// Clut lookup 1.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

PIXEL1_TRANSPARENT_END:
		test		bl,bl							// Test for transparency.
		jz			short END_OF_SCANLINE

		mov			bx,word ptr[ebp + ebx*2]		// Clut lookup 2.
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left.
		//
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ecx,dword ptr[esi + ebx*4]		// Lookup texel 1.
		mov			ebx,ecx							// Copy texel 1
		sub			ecx,[u4LightTheta]				// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		shr			ebx,24							// Shift color value down.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		mov			bh,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		mov			ebp,[pvClutConversion]			// Base of CLUT.

		test		bl,bl							// Test for transparency.
		jz			short END_OF_SCANLINE

		mov			bx,word ptr[ebp + ebx*2]		// Clut lookup 1.
		mov			[edx + edi*2 - 4],bx			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TBumpLinearTrans.indCoord.pfIndex

		mov		ebx,[eax]TBumpLinearTrans.fxXDifference

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
		mov		[eax]TBumpLinearTrans.fxXDifference,ebx

		movq	[eax]TBumpLinearTrans.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TBumpLinearTrans.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TBumpLinearTrans.fxX,ecx
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
// Bumpmapped and Linear for 16 bit screen.
//
void DrawSubtriangle(TBumpLinear* pscan, CDrawPolygon<TBumpLinear>* pdtri)
{
	typedef TBumpLinear::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TBumpLinear>* pdtriGlbl;
	static TBumpLinear* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TBumpLinear* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[edi]TBumpLinear.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TBumpLinear.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TBumpLinear.indCoord.pfIndex
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

		movq		mm7,[eax]TBumpLinear.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TBumpLinear.fxX.i4Fx

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

		// Align start of loop to 9 past the beginning of a cache line.
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

		mov		esi,dword ptr[esi + ebx*4]		// Lookup texel 2.
		paddd		mm7,mm6							// Step U,V

		mov			eax,ecx							// Copy texel 1
		sub			ecx,[u4LightTheta]				// Subtract light theta 1.

		mov			ebx,esi							// Copy texel 2
		sub			esi,[u4LightTheta]				// Subtract light theta 2.

		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		shr			eax,24							// Shift color value down.

		shr			ebx,24							// Shift color value down.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.

		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		and			esi,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.

		mov			bh,byte ptr[ebp + esi]			// Bump map intensity lookup 2.
		movq		mm1,mm7							// Copy U2,V2
	
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		sub			ebp,4							// Force offset for movzx (was a nop)

		mov			esi,[pvTextureBitmap]			// Base of texture.
		pand		mm0,mm3							// Mask for tiling

		movzx		eax,word ptr[ebp + eax*2 + 4]	// Clut lookup 1.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		mov			[edx + edi*2 - 4],ax			// Store pixel 1

		movzx		ebx,word ptr[ebp + ebx*2 + 4]	// Clut lookup 2.
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
		paddd		mm7,mm6							// Step U,V
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
		mov			ah,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		and			esi,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 2.
		mov			bh,byte ptr[ebp + esi]			// Bump map intensity lookup 2.
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		dec			ebp								// Force offset for movzx (was a nop)
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup 1.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup 2.
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ecx,dword ptr[esi + ebx*4]		// Lookup texel 1.
		mov			ebx,ecx							// Copy texel 1
		sub			ecx,[u4LightTheta]				// Subtract light theta 1.
		mov			ebp,[au1SubBumpToIntensity]		// Base of bump to intensity table.
		shr			ebx,24							// Shift color value down.
		and			ecx,iMASK_ANGLETABLE_LOOKUP		// Mask for bump map table index 1.
		mov			bh,byte ptr[ebp + ecx]			// Bump map intensity lookup 1.
		mov			ebp,[pvClutConversion]			// Base of CLUT.
		dec			ebp								// Force offset for movzx (was a nop)
		mov			bx,word ptr[ebp + ebx*2 + 1]	// Clut lookup 1.
		mov			[edx + edi*2 - 4],bx			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TBumpLinear.indCoord.pfIndex

		mov		ebx,[eax]TBumpLinear.fxXDifference

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
		mov		[eax]TBumpLinear.fxXDifference,ebx

		movq	[eax]TBumpLinear.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TBumpLinear.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TBumpLinear.fxX,ecx
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

#endif // elif (iBUMPMAP_RESOLUTION == 32)

// Restore default section.
#pragma code_seg()
