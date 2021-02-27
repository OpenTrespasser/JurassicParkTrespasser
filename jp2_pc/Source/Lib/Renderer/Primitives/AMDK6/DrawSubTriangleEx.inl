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
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/DrawSubTriangleEx.cpp                   $
 * 
 * 20    98.10.03 11:10p Mmouni
 * Put self moding code in code segment "SelfMod".
 * 
 * 19    9/05/98 12:18p Asouth
 * explicit scoping of 'fixed'
 * 
 * 18    98.08.13 10:43p Mmouni
 * Changed to work with VC5.0sp3.
 * 
 * 17    3/13/98 6:06p Mmouni
 * 16-bit transparent copy now does dword writes (if it can).
 * 
 * 16    3/12/98 1:14p Mmouni
 * Fixed error in QWORD alignment.
 * 
 * 15    2/17/98 5:42p Mmouni
 * Copy primitive now writes QWORD aligned QWORDS.
 * 
 * 14    2/04/98 4:10p Mmouni
 * 16-bit copy primitive now write aligned  DWORDS.
 * 
 * 13    11/15/97 8:51p Mmouni
 * Added extra ALIGN,nop to linear filtered routine.
 * 
 * 12    97.11.15 7:47p Mmouni
 * Optmized alignment of filtered inner loop.
 * 
 * 11    97.11.15 12:01a Mmouni
 * Made bi-linear filter primitives seperate from non-filtered copy primitive.
 * 
 * 10    97.11.11 9:49p Mmouni
 * Made changes to filtered copy primitives to support 555 or 565 color.
 * 
 * 9     97.11.06 9:05p Mmouni
 * Added bi-linear filtered copy.
 * 
 * 8     97.11.05 7:52p Mmouni
 * Added terrain fog mask.
 * 
 * 7     97.11.04 10:59p Mmouni
 * Linear version of primitives now use 3dx/mmx.
 * 
 * 6     97.10.30 1:39p Mmouni
 * All perspective sub-triangle routines are now 3DX/MMX.
 * 
 * 5     97.10.27 1:25p Mmouni
 * Initial K6-3D version, copied from the Pentium versions.
 * 
 * 16    97.10.22 7:11p Mmouni
 * Moved code that makes copies of edge values into assembly.
 * 
 * 15    10/10/97 1:47p Mmouni
 * All inner loops are now left to right only, and optimized some.
 * 
 * 14    9/30/97 9:32p Mmouni
 * Made seperate routines for 16-bit copy and terrain polygons.
 * 
 * 13    9/29/97 11:49a Mmouni
 * Optimized, now has support for fogging.
 * 
 * 12    9/15/97 2:10p Mmouni
 * Optimized inner loops (not final yet).
 * 
 * 11    9/01/97 8:02p Rwyatt
 * 
 * 10    8/17/97 10:00p Rwyatt
 * Inlined perspective for non-transparent 16bit copy. This is so its clamping can be control
 * outside of the general perspective code.
 * 
 * 9     8/17/97 9:50p Rwyatt
 * changed the clamp condition to use VER_CLAMP_UV_16BIT and not the general clamp control
 * VER_CLAMP_UV_TILE
 * 
 * 8     8/17/97 9:44p Rwyatt
 * Added temporary #defines to P5 clamp code. This clamp code needs to be optimized later.
 * 
 * 7     8/15/97 2:33a Rwyatt
 * Removed local static variables, all renderer statics are now defined in DrawSubrtiangle.cpp
 * 
 * 6     8/15/97 12:45a Rwyatt
 * Y Loop optimizarions and inlined perspective
 * 
 * 5     8/01/97 6:57p Rwyatt
 * Linear, No Clut, Non-Transparent has been optimized for 32 bit writes. There is now only 2 16
 * bit reads in the main loop and 1 32 bit write. This means that L to R and R to L scanlines
 * are completely different.
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
 * 1     7/07/97 5:43p Rwyatt
 * Initial Implementation of Pentium Specifics
 * 
 ***********************************************************************************************
 ***********************************************************************************************
 * 22    6/25/97 3:28p Rwyatt
 * Re-aranged code
 * 
 * 21    6/17/97 4:10p Rwyatt
 * Optimized perspective call
 * 
 * 20    97/06/16 10:16p Pkeet
 * Moved linear primitives into non-template code.
 * 
 * 19    97/06/16 8:47p Pkeet
 * Replaced template calls with explicit C++ code for perspective routines.
 * 
 * 18    6/15/97 1:31a Bbell
 * Removed unnecessary include.
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
#include "AsmSupport.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"


//
// Turn off the global optimized because it was re-arranging the assembly code
// without account for the self-modification offsets.
//
#pragma optimize("g", off)


// Put this stuff in a seperate section for alignment.
#pragma code_seg("StriCopy")


//*****************************************************************************************
//
// Perspective, No Clut, Transparent.
//
void DrawSubtriangle(TCopyPerspTrans* pscan, CDrawPolygon<TCopyPerspTrans>* pdtri)
{
	::fixed fx_inc;
	::fixed fx_diff;

	TCopyPerspTrans* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[eax]TCopyPerspTrans.fxX

		mov		edx,[eax]TCopyPerspTrans.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TCopyPerspTrans.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TCopyPerspTrans.indCoord.fInvZ
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

		punpcklwd	mm4,mm3										// Width,1,Width,1
		movq		mm0,[ecx]TCopyPerspTrans.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4								// Save mfactor
		movd		mm1,[ecx]TCopyPerspTrans.indCoord.fInvZ	// mm1 = (?,1/Z)

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
		// PERSPECTIVE_SCANLINE_PROLOGUE(TCopyPerspTrans)
		//

		//
		// Determine the start and end of the scanline. 
		//
		mov		ebx,[ecx]TCopyPerspTrans.fxX.i4Fx

		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,eax

		mov		eax,[ecx]TCopyPerspTrans.fxX.i4Fx
		sar		ebx,16

		sar		eax,16
		add		edx,ebx

		sub		eax,ebx									// eax == i_pixel
		jge		END_OF_SCANLINE

		// Load span increments.
		movq	mm2,[tvDEdge.UInvZ]						// Load V,U
		mov		[i_screen_index],edx					// Save scanline index.
	
		movd	mm3,[tvDEdge.InvZ]						// Load Z
		mov		[i_pixel],eax							// Save negative length of scanline.

		mov		edi,[iSubdivideLen]

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
		mov		edx,[bClampUV]					// Load clamp flag.

		pfadd	(m2,m0)							// Step fGUInvZ,fGVInvZ
		movd	[tvCurUVZ.InvZ],mm3				// Save updated 1/Z

		pfmul	(m4,m0)							// mm4 = V1,U1
		pfrcp	(m0,m3)							// f_next_z = mm0 = 1/fGInvZ

		movq	[tvCurUVZ.UInvZ],mm2					// Save updated U/Z,V/Z
		movd	mm1,float ptr[fInverseIntTable+edi*4]	// Reciprocal of span width.

		pfmul	(m0,m2)							// mm0 = V2,U2
		movq	mm2,[pfFixed16Scale]			// Load fixed point scale factors.

#if (VER_CLAMP_UV_16BIT == TRUE)
		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
		pfmax	(m4,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m4,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
#endif

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
		// eax = texel_index2		ebx = texel_index1
		// ecx = temp				edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = temp
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm6							// Step U,V

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		movq		mm2,[qMFactor]					// Load texture multipliers
		psrad		mm1,16							// Shift for integer U2,V2

		sub			esi,4							// Hack to force SIB + offset in loop
		packssdw	mm0,mm1							// Pack integer texture values

		//
		// Check for alignment on a dword boundry.
		//
		lea			eax,[edx + edi * 2]				// Destination address.

		and			eax,3							// Check for dword alignment.
		jz			INNER_LOOP_2P

		inc			edi								// One pixel left?
		jz			ONE_PIXEL_LEFT

		//
		// Do one pixel for alignment.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy new U2,V2

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		packssdw	mm1,mm0							// Pack integer texture values

		movq		mm0,mm1							// Pack new U2,V2 with old U2,V2
		test		ebx,ebx

		jz			INNER_LOOP_2P
		mov			[edx + edi*2 - 2],bx			// Store pixel 1

		jmp		short INNER_LOOP_2P

		// Align start of loop to 4 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16
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
		add			edi,2							// Inrcement index by 2
		jge			FINISH_REMAINDER				// Loop (while >2 pixels)

		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.

		shl			eax,16							// Shift pixel 2 up.
		jz			P2_TRANSPARENT

		test		ebx,ebx							// Test P1
		jz			P1_TRANSPARENT

		or			eax,ebx							// Combine pixels.
		mov			[edx + edi*2 - 4],eax			// Store both pixels.

		jmp			short INNER_LOOP_2P

P2_TRANSPARENT:
		// P1 may or may not be transparent.
		test		ebx,ebx							// Test P1
		jz			INNER_LOOP_2P

		mov			[edx + edi*2 - 4],bx			// Store pixel 1

		jmp			short INNER_LOOP_2P

P1_TRANSPARENT:
		// P2 is not transparent.
		shr			eax,16							// Shift pixel 2 down.

		mov			[edx + edi*2 - 2],ax			// Store pixel 2

		jmp			short INNER_LOOP_2P

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
		paddd		mm7,mm6							// Step U,V
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		shl			eax,16							// Shift pixel 2 up.
		jz			P2_TRANSPARENT_END
		test		ebx,ebx							// Test P1
		jz			P1_TRANSPARENT_END
		or			eax,ebx							// Combine pixels.
		mov			[edx - 4],eax					// Store both pixels.
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

P2_TRANSPARENT_END:
		// P1 may or may not be transparent.
		test		ebx,ebx							// Test P1
		mov			eax,[i_pixel]					// Load remaining length.
		jz			END_OF_SPAN
		mov			[edx - 4],bx					// Store pixel 1
		jmp			short END_OF_SPAN

P1_TRANSPARENT_END:
		// P2 is not transparent.
		shr			eax,16							// Shift pixel 2 down.
		mov			[edx - 2],ax					// Store pixel 2
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,[i_pixel]					// Load remaining length.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
	
		test		ebx,ebx
		jz			END_OF_SPAN

		mov			[edx - 2],bx					// Store pixel 1

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		jz		short END_OF_SCANLINE

		//
		// Do perspective correction and looping.
		// PERSPECTIVE_SCANLINE_EPILOGUE
		//

		//---------------------------------------------------------------------------------- 
		// Implement perspective correction. 
		//
		// eax = i_pixel
		// Preserve: mm7
		//
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

#if (VER_CLAMP_UV_16BIT == TRUE)
		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
#endif

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

		mov		eax,[ecx]TCopyPerspTrans.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TCopyPerspTrans.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TCopyPerspTrans.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TCopyPerspTrans.indCoord.fInvZ

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
		movq	[ecx]TCopyPerspTrans.indCoord.fUInvZ,mm0

		movd	[ecx]TCopyPerspTrans.indCoord.fInvZ,mm1

		mov		[ecx]TCopyPerspTrans.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TCopyPerspTrans.fxXDifference.i4Fx,ebx
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
// Linear, No clut, Transparent.
//
//******************************
void DrawSubtriangle(TCopyLinearTrans* pscan, CDrawPolygon<TCopyLinearTrans>* pdtri)
{
	typedef TCopyLinearTrans::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TCopyLinearTrans>* pdtriGlbl;
	static TCopyLinearTrans* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TCopyLinearTrans* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[edi]TCopyLinearTrans.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TCopyLinearTrans.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TCopyLinearTrans.indCoord.pfIndex
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

		movq		mm7,[eax]TCopyLinearTrans.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TCopyLinearTrans.fxX.i4Fx

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
		// eax = texel_index2		ebx = texel_index1
		// ecx = temp				edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = temp
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

		//
		// Check for alignment on a dword boundry.
		//
		lea			eax,[edx + edi * 2]				// Destination address.

		and			eax,3							// Check for dword alignment.
		jz			INNER_LOOP_2P

		inc			edi								// One pixel left?
		jz			ONE_PIXEL_LEFT

		//
		// Do one pixel for alignment.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy new U2,V2

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		packssdw	mm1,mm0							// Pack integer texture values

		movq		mm0,mm1							// Pack new U2,V2 with old U2,V2
		test		ebx,ebx

		jz			INNER_LOOP_2P
		mov			[edx + edi*2 - 2],bx			// Store pixel 1

		jmp		short INNER_LOOP_2P

		// Align start of loop to 4 past the beginning of a cache line.
		ALIGN	16
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
		add			edi,2							// Inrcement index by 2
		jge			FINISH_REMAINDER				// Loop (while >2 pixels)

		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.

		shl			eax,16							// Shift pixel 2 up.
		jz			P2_TRANSPARENT

		test		ebx,ebx							// Test P1
		jz			P1_TRANSPARENT

		or			eax,ebx							// Combine pixels.
		mov			[edx + edi*2 - 4],eax			// Store both pixels.

		jmp			short INNER_LOOP_2P

P2_TRANSPARENT:
		// P1 may or may not be transparent.
		test		ebx,ebx							// Test P1
		jz			INNER_LOOP_2P

		mov			[edx + edi*2 - 4],bx			// Store pixel 1

		jmp			short INNER_LOOP_2P

P1_TRANSPARENT:
		// P2 is not transparent.
		shr			eax,16							// Shift pixel 2 down.

		mov			[edx + edi*2 - 2],ax			// Store pixel 2

		jmp			short INNER_LOOP_2P

FINISH_REMAINDER:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			short ONE_PIXEL_LEFT

		//
		// Two pixels left, don't step texture co-ordinates.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		shl			eax,16							// Shift pixel 2 up.
		jz			P2_TRANSPARENT_END
		test		ebx,ebx							// Test P1
		jz			P1_TRANSPARENT_END
		or			eax,ebx							// Combine pixels.
		mov			[edx - 4],eax					// Store both pixels.
		jmp			short END_OF_SCANLINE

P2_TRANSPARENT_END:
		// P1 may or may not be transparent.
		test		ebx,ebx							// Test P1
		jz			END_OF_SCANLINE
		mov			[edx - 4],bx					// Store pixel 1
		jmp			short END_OF_SCANLINE

P1_TRANSPARENT_END:
		// P2 is not transparent.
		shr			eax,16							// Shift pixel 2 down.
		mov			[edx - 2],ax					// Store pixel 2
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
	
		test		ebx,ebx
		jz			END_OF_SCANLINE

		mov			[edx - 2],bx					// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TCopyLinearTrans.indCoord.pfIndex

		mov		ebx,[eax]TCopyLinearTrans.fxXDifference

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
		mov		[eax]TCopyLinearTrans.fxXDifference,ebx

		movq	[eax]TCopyLinearTrans.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TCopyLinearTrans.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TCopyLinearTrans.fxX,ecx
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
// Perspective, no Clut, non-transparent.
//
void DrawSubtriangle(TCopyPersp* pscan, CDrawPolygon<TCopyPersp>* pdtri)
{
	::fixed fx_inc;
	::fixed fx_diff;

	TCopyPersp* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[eax]TCopyPersp.fxX

		mov		edx,[eax]TCopyPersp.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TCopyPersp.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TCopyPersp.indCoord.fInvZ
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

		punpcklwd	mm4,mm3										// Width,1,Width,1
		movq		mm0,[ecx]TCopyPersp.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4								// Save mfactor
		movd		mm1,[ecx]TCopyPersp.indCoord.fInvZ	// mm1 = (?,1/Z)

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
		// PERSPECTIVE_SCANLINE_PROLOGUE(TCopyPersp)
		//

		//
		// Determine the start and end of the scanline. 
		//
		mov		ebx,[ecx]TCopyPersp.fxX.i4Fx

		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,eax

		mov		eax,[ecx]TCopyPersp.fxX.i4Fx
		sar		ebx,16

		sar		eax,16
		add		edx,ebx

		sub		eax,ebx									// eax == i_pixel
		jge		END_OF_SCANLINE

		// Load span increments.
		movq	mm2,[tvDEdge.UInvZ]						// Load V,U
		mov		[i_screen_index],edx					// Save scanline index.
	
		movd	mm3,[tvDEdge.InvZ]						// Load Z
		mov		[i_pixel],eax							// Save negative length of scanline.

		mov		edi,[iSubdivideLen]

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
		mov		edx,[bClampUV]					// Load clamp flag.

		pfadd	(m2,m0)							// Step fGUInvZ,fGVInvZ
		movd	[tvCurUVZ.InvZ],mm3				// Save updated 1/Z

		pfmul	(m4,m0)							// mm4 = V1,U1
		pfrcp	(m0,m3)							// f_next_z = mm0 = 1/fGInvZ

		movq	[tvCurUVZ.UInvZ],mm2					// Save updated U/Z,V/Z
		movd	mm1,float ptr[fInverseIntTable+edi*4]	// Reciprocal of span width.

		pfmul	(m0,m2)							// mm0 = V2,U2
		movq	mm2,[pfFixed16Scale]			// Load fixed point scale factors.

#if (VER_CLAMP_UV_16BIT == TRUE)
		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
		pfmax	(m4,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m4,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
#endif

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
		// eax = texel_index2		ebx = texel_index1
		// ecx = temp				edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = temp
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		movq		mm0,mm7							// Copy U1,V1
		paddd		mm7,mm6							// Step U,V

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		movq		mm2,[qMFactor]					// Load texture multipliers
		psrad		mm1,16							// Shift for integer U2,V2

		sub			esi,4							// Hack to force SIB + offset in loop
		packssdw	mm0,mm1							// Pack integer texture values

		//
		// Check for alignment on a dword boundry.
		//
		lea			eax,[edx + edi * 2]

		and			eax,3
		jz			ALIGNED2

		inc			edi
		jz			ONE_PIXEL_LEFT

		//
		// Do one pixel for alignment.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy new U2,V2

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		packssdw	mm1,mm0							// Pack integer texture values

		movq		mm0,mm1							// Pack new U2,V2 with old U2,V2
		lea			eax,[edx + edi * 2]				// Update eax.

		mov			[edx + edi*2 - 2],bx			// Store pixel 1

ALIGNED2:
		and		eax,7
		jz		ALIGNED4

		add		edi,2
		jge		FINISH_REMAINDER2

		//
		// Do two pixels for alignment.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		shl			eax,16							// Shift second pixel up

		or			eax,ebx							// Combine pixels.
		mov			[edx + edi*2 - 4],eax			// Store both pixels

ALIGNED4:
		//
		// Make sure we have more than 4 pixels left.
		//
		add		edi,4
		jl		INNER_LOOP_4P

		jmp		FINISH_REMAINDER4

		// Align start of loop to 4 past the beginning of a cache line.
		ALIGN 16
		nop
		nop
		nop
		nop

		//
		// Do four pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_4P:
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		shl			eax,16							// Shift second pixel up

		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		or			eax,ebx							// Combine pixels.
		movd		mm3,eax							// Copy P2,P1

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		shl			eax,16							// Shift second pixel up

		or			eax,ebx							// Combine pixels.
		movd		mm4,eax							// Copy P4,P3

		punpckldq	mm3,mm4							// Combine P4,P3,P2,P1
		movq		[edx + edi*2 - 8],mm3			// Store 4 pixels

		add			edi,4							// Increment index by 4
		jl			INNER_LOOP_4P				// Loop (while >4 pixels)

FINISH_REMAINDER4:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		jnz			THREE_OR_LESS

		//
		// Four pixels left, step texture coords 3 times.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V
		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.
		shl			eax,16							// Shift second pixel up.
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V
		or			eax,ebx							// Combine pixels
		movd		mm3,eax							// Copy P2,P1
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		shl			eax,16							// Shift second pixel up
		or			eax,ebx							// Combine pixels.
		movd		mm4,eax							// Copy P4,P3
		punpckldq	mm3,mm4							// Combine P4,P3,P2,P1
		movq		[edx - 8],mm3					// Store 4 pixels
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			END_OF_SPAN

THREE_OR_LESS:
		sub			edi,2
		jge			FINISH_REMAINDER2

		//
		// Three pixels left, step texture coords twice.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V
		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.
		shl			eax,16							// Shift second pixel up
		or			eax,ebx							// Combine pixels.
		mov			[edx - 6],eax					// Store both pixels
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,[i_pixel]					// Load remaining length.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		mov			[edx - 2],bx					// Store pixel 1
		jmp			END_OF_SPAN

FINISH_REMAINDER2:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture co-ordinates once.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		shl			eax,16							// Shift second pixel up
		or			eax,ebx							// Combine pixels.
		mov			[edx - 4],eax					// Store both pixels
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,[i_pixel]					// Load remaining length.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		mov			[edx - 2],bx					// Store pixel 1

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		jz		short END_OF_SCANLINE

		//
		// Do perspective correction and looping.
		// PERSPECTIVE_SCANLINE_EPILOGUE
		//

		//---------------------------------------------------------------------------------- 
		// Implement perspective correction. 
		//
		// eax = i_pixel
		// Preserve: mm7
		//
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

#if (VER_CLAMP_UV_16BIT == TRUE)
		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
#endif

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

		mov		eax,[ecx]TCopyPersp.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TCopyPersp.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TCopyPersp.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TCopyPersp.indCoord.fInvZ

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
		movq	[ecx]TCopyPersp.indCoord.fUInvZ,mm0

		movd	[ecx]TCopyPersp.indCoord.fInvZ,mm1

		mov		[ecx]TCopyPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TCopyPersp.fxXDifference.i4Fx,ebx
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
// Linear, no clut, non-transparent.
//
//**********************************
void DrawSubtriangle(TCopyLinear* pscan, CDrawPolygon<TCopyLinear>* pdtri)
{
	typedef TCopyLinear::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TCopyLinear>* pdtriGlbl;
	static TCopyLinear* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TCopyLinear* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[edi]TCopyLinear.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TCopyLinear.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TCopyLinear.indCoord.pfIndex
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

		movq		mm7,[eax]TCopyLinear.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TCopyLinear.fxX.i4Fx

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
		// eax = texel_index2		ebx = texel_index1
		// ecx = temp				edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = temp
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

		//
		// Check for alignment on a dword boundry.
		//
		lea			eax,[edx + edi * 2]

		and			eax,3
		jz			ALIGNED2

		inc			edi
		jz			ONE_PIXEL_LEFT

		//
		// Do one pixel for alignment.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy new U2,V2

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		psrad		mm0,16							// Shift for integer U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		packssdw	mm1,mm0							// Pack integer texture values

		movq		mm0,mm1							// Pack new U2,V2 with old U2,V2
		lea			eax,[edx + edi * 2]				// Update eax.

		mov			[edx + edi*2 - 2],bx			// Store pixel 1

ALIGNED2:
		and		eax,7
		jz		ALIGNED4

		add		edi,2
		jge		FINISH_REMAINDER2

		//
		// Do two pixels for alignment.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		shl			eax,16							// Shift second pixel up

		or			eax,ebx							// Combine pixels.
		mov			[edx + edi*2 - 4],eax			// Store both pixels

ALIGNED4:
		//
		// Make sure we have more than 4 pixels left.
		//
		add		edi,4
		jl		INNER_LOOP_4P

		jmp		FINISH_REMAINDER4

		// Align start of loop to 4 past the beginning of a cache line.
		ALIGN 16
		nop
		nop
		nop
		nop

		//
		// Do four pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_4P:
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		shl			eax,16							// Shift second pixel up

		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V

		or			eax,ebx							// Combine pixels.
		movd		mm3,eax							// Copy P2,P1

		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1

		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1

		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V

		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		shl			eax,16							// Shift second pixel up

		or			eax,ebx							// Combine pixels.
		movd		mm4,eax							// Copy P4,P3

		punpckldq	mm3,mm4							// Combine P4,P3,P2,P1
		movq		[edx + edi*2 - 8],mm3			// Store 4 pixels

		add			edi,4							// Increment index by 4
		jl			INNER_LOOP_4P					// Loop (while >4 pixels)

FINISH_REMAINDER4:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		jnz			THREE_OR_LESS

		//
		// Four pixels left, step texture coords 3 times.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V
		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.
		shl			eax,16							// Shift second pixel up.
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V
		or			eax,ebx							// Combine pixels
		movd		mm3,eax							// Copy P2,P1
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		shl			eax,16							// Shift second pixel up
		or			eax,ebx							// Combine pixels.
		movd		mm4,eax							// Copy P4,P3
		punpckldq	mm3,mm4							// Combine P4,P3,P2,P1
		movq		[edx - 8],mm3					// Store 4 pixels
		jmp			END_OF_SCANLINE

THREE_OR_LESS:
		sub			edi,2
		jge			FINISH_REMAINDER2

		//
		// Three pixels left, step texture coords twice.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		paddd		mm7,mm6							// Step U,V
		movq		[qIndexTemp],mm0				// Save texture indicies.
		movq		mm0,mm7							// Copy U1,V1
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		psrad		mm0,16							// Shift for integer U1,V1
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		paddd		mm7,mm6							// Step U,V
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.
		shl			eax,16							// Shift second pixel up
		or			eax,ebx							// Combine pixels.
		mov			[edx - 6],eax					// Store both pixels
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		mov			[edx - 2],bx					// Store pixel 1
		jmp			END_OF_SCANLINE

FINISH_REMAINDER2:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture co-ordinates once.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp + 4]	// Get texture index 2
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 2
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		shl			eax,16							// Shift second pixel up
		or			eax,ebx							// Combine pixels.
		mov			[edx - 4],eax					// Store both pixels
		jmp			END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		pmaddwd		mm0,mm2							// Compute texture indicies.
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		mov			[edx - 2],bx					// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TCopyLinear.indCoord.pfIndex

		mov		ebx,[eax]TCopyLinear.fxXDifference

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
		mov		[eax]TCopyLinear.fxXDifference,ebx

		movq	[eax]TCopyLinear.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TCopyLinear.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TCopyLinear.fxX,ecx
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
// Perspective, Terrain Clut, non-transparent.
//
void DrawSubtriangle(TCopyTerrainPersp* pscan, CDrawPolygon<TCopyTerrainPersp>* pdtri)
{
	static void* pvLastTexture = 0;
	static void* pvLastFogClut = 0;
	TCopyTerrainPersp* plinc = &pdtri->pedgeBase->lineIncrement;

	Assert(pu2TerrainTextureFogClut);

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
		mov		ebx,[eax]TCopyTerrainPersp.fxX

		mov		edx,[eax]TCopyTerrainPersp.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TCopyTerrainPersp.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TCopyTerrainPersp.indCoord.fInvZ
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

		punpcklwd	mm4,mm3										// Width,1,Width,1
		movq		mm0,[ecx]TCopyTerrainPersp.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4								// Save mfactor
		movd		mm1,[ecx]TCopyTerrainPersp.indCoord.fInvZ	// mm1 = (?,1/Z)

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
		// PERSPECTIVE_SCANLINE_PROLOGUE(TCopyTerrainPersp)
		//

		//
		// Determine the start and end of the scanline. 
		//
		mov		ebx,[ecx]TCopyTerrainPersp.fxX.i4Fx

		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,eax

		mov		eax,[ecx]TCopyTerrainPersp.fxX.i4Fx
		sar		ebx,16

		sar		eax,16
		add		edx,ebx

		sub		eax,ebx									// eax == i_pixel
		jge		END_OF_SCANLINE

		// Load span increments.
		movq	mm2,[tvDEdge.UInvZ]						// Load V,U
		mov		[i_screen_index],edx					// Save scanline index.
	
		movd	mm3,[tvDEdge.InvZ]						// Load Z
		mov		[i_pixel],eax							// Save negative length of scanline.

		mov		edi,[iSubdivideLen]

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
		mov		edx,[bClampUV]					// Load clamp flag.

		pfadd	(m2,m0)							// Step fGUInvZ,fGVInvZ
		movd	[tvCurUVZ.InvZ],mm3				// Save updated 1/Z

		pfmul	(m4,m0)							// mm4 = V1,U1
		pfrcp	(m0,m3)							// f_next_z = mm0 = 1/fGInvZ

		movq	[tvCurUVZ.UInvZ],mm2					// Save updated U/Z,V/Z
		movd	mm1,float ptr[fInverseIntTable+edi*4]	// Reciprocal of span width.

		pfmul	(m0,m2)							// mm0 = V2,U2
		movq	mm2,[pfFixed16Scale]			// Load fixed point scale factors.

#if (VER_CLAMP_UV_16BIT == TRUE)
		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
		pfmax	(m4,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m4,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
#endif

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

		mov			ecx,[pu2TerrainTextureFogClut]	// Terrain clut pointer. Hack.
		packssdw	mm0,mm1							// Pack integer texture values

		sub			esi,4							// Hack to force SIB + offset in loop
		sub			ecx,4							// Hack to force SIB + offset in loop

		pmaddwd		mm0,mm2							// Compute texture indicies.
		mov			ebp,[u4TerrainFogMask]

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 0 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16

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

		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		and			eax,ebp							// Apply alpha mask.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for p1
		and			ebx,ebp							// Apply alpha mask.

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p2

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
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 1
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 2
		and			eax,ebp							// Apply alpha mask.
		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for p1
		and			ebx,ebp							// Apply alpha mask.
		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p2
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,[i_pixel]					// Load remaining length.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		and			ebx,ebp							// Apply alpha mask.
		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p1
		mov			[edx + edi*2 - 4],bx			// Store pixel 1

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		jz		short END_OF_SCANLINE

		//
		// Do perspective correction and looping.
		// PERSPECTIVE_SCANLINE_EPILOGUE
		//

		//---------------------------------------------------------------------------------- 
		// Implement perspective correction. 
		//
		// eax = i_pixel
		// Preserve: mm7
		//
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

#if (VER_CLAMP_UV_16BIT == TRUE)
		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
#endif

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
		pop		ebp
		mov		esi,[pdtri]						// Pointer to polygon object.

		mov		ecx,[pscan]						// Pointer to scanline object.

		// Step length, line starting address.
		mov		edx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,[esi]CDrawPolygonBase.iLineStartIndex

		add		edx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx

		add		edi,gsGlobals.u4LinePixels

		mov		eax,[ecx]TCopyTerrainPersp.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TCopyTerrainPersp.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TCopyTerrainPersp.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]TCopyTerrainPersp.indCoord.fInvZ

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
		movq	[ecx]TCopyTerrainPersp.indCoord.fUInvZ,mm0

		movd	[ecx]TCopyTerrainPersp.indCoord.fInvZ,mm1

		mov		[ecx]TCopyTerrainPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TCopyTerrainPersp.fxXDifference.i4Fx,ebx
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
// Linear, Terrain clut, non-transparent.
//
//**********************************
void DrawSubtriangle(TCopyTerrainLinear* pscan, CDrawPolygon<TCopyTerrainLinear>* pdtri)
{
	typedef TCopyTerrainLinear::TIndex prevent_internal_compiler_errors;
	static CDrawPolygon<TCopyTerrainLinear>* pdtriGlbl;
	static TCopyTerrainLinear* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TCopyTerrainLinear* plinc = &pdtri->pedgeBase->lineIncrement;

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
		mov		ebx,[edi]TCopyTerrainLinear.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TCopyTerrainLinear.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]TCopyTerrainLinear.indCoord.pfIndex
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

		movq		mm7,[eax]TCopyTerrainLinear.indCoord.pfIndex

		mov			ebp,[u4TerrainFogMask]

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TCopyTerrainLinear.fxX.i4Fx

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

		mov			ecx,[pu2TerrainTextureFogClut]	// Terrain clut pointer. Hack.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values
		sub			esi,4							// Hack to force SIB + offset in loop

		sub			ecx,4							// Hack to force SIB + offset in loop
		pmaddwd		mm0,mm2							// Compute texture indicies.

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 0 past the beginning of a cache line.
		ALIGN	16

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

		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 2
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2
		packssdw	mm0,mm1							// Pack integer texture values.

		and			eax,ebp							// Apply alpha mask.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for p1
		and			ebx,ebp							// Apply alpha mask.

		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p2

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
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movzx		eax,word ptr[esi + eax*2 + 4]	// Lookup texel 1
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 2
		and			eax,ebp							// Apply alpha mask.
		movzx		eax,word ptr[ecx + eax*2 + 4]	// Clut lookup for p1
		and			ebx,ebp							// Apply alpha mask.
		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p2
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			ebx,dword ptr[qIndexTemp]		// Get texture index 1
		movzx		ebx,word ptr[esi + ebx*2 + 4]	// Lookup texel 1
		and			ebx,ebp							// Apply alpha mask.
		movzx		ebx,word ptr[ecx + ebx*2 + 4]	// Clut lookup for p1
		mov			[edx + edi*2 - 4],bx			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TCopyTerrainLinear.indCoord.pfIndex

		mov		ebx,[eax]TCopyTerrainLinear.fxXDifference

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
		mov		[eax]TCopyTerrainLinear.fxXDifference,ebx

		movq	[eax]TCopyTerrainLinear.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TCopyTerrainLinear.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TCopyTerrainLinear.fxX,ecx
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


#if (BILINEAR_FILTER)

#pragma code_seg("SelfMod")

//
// Globals used by filtering.
//
static uint64 qUVStep;
static uint64 usteps;
static uint64 vsteps;

const uint64 mask_greens_555 = 0x03e003e003e003e0;
const uint64 mask_greens_565 = 0x07e007e007e007e0;
const uint64 mask_blues = 0x001f001f001f001f;
const uint64 red_green_shift_555 = 0x0020002004000400; // 2^5,2^5,2^10,2^10
const uint64 red_green_shift_565 = 0x0020002008000800; // 2^5,2^5,2^11,2^11
const uint64 red_green_mask_555 = 0x0003e00000f80000;
const uint64 red_green_mask_565 = 0x0007e00000f80000;
const uint64 uf_xors = 0x00000000ffffffff;
const uint64 uf_adds = 0x0000000000010001;
const uint64 vf_xors = 0x0000ffff0000ffff;
const uint64 vf_adds = 0x0000000100000001;

// Default setup for 565 color.
static uint64 mask_greens = mask_greens_565;
static uint64 red_green_shift = red_green_shift_565;
static uint64 red_green_mask = red_green_mask_565;


//*****************************************************************************************
//
// Perspective, no Clut, non-transparent, filtered.
//
void DrawSubtriangle(TCopyPerspFilter* pscan, CDrawPolygon<TCopyPerspFilter>* pdtri)
{
	::fixed fx_inc;
	::fixed fx_diff;
	static int iBits = 16;

	TCopyPerspFilter* plinc = &pdtri->pedgeBase->lineIncrement;

	//
	// This type is the same as TCopyPerspFilter, except that it uses the parent class of
	// CIndexPerspectiveFilter to allow us to access members of it.
	//
	typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapTexture<uint16>, CIndexPerspective, CColLookupOff> FakeTCopyPerspFilter;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		// Check/Modify for 555/565 color.
		mov		eax,gsGlobals.iBits

		cmp		eax,[iBits]
		je		short DONE_WITH_MODIFY

		mov		[iBits],eax

		cmp		eax,16
		je		MODIFY_FOR_565

		// Change to 555.
		movq	mm0,mask_greens_555
		movq	mask_greens,mm0

		movq	mm0,red_green_shift_555
		movq	red_green_shift,mm0

		movq	mm0,red_green_mask_555
		movq	red_green_mask,mm0

		lea		eax,MODIFY_FOR_RED_SHIFT_A
		mov		byte ptr[eax-1],10
		lea		eax,MODIFY_FOR_RED_SHIFT_B
		mov		byte ptr[eax-1],10
		lea		eax,MODIFY_FOR_RED_SHIFT_C
		mov		byte ptr[eax-1],10
		lea		eax,MODIFY_FOR_RED_SHIFT_D
		mov		byte ptr[eax-1],10
		lea		eax,MODIFY_FOR_RED_SHIFT_E
		mov		byte ptr[eax-1],10
		jmp		short DONE_WITH_MODIFY

MODIFY_FOR_565:
		// Change to 565.
		movq	mm0,mask_greens_565
		movq	mask_greens,mm0

		movq	mm0,red_green_shift_565
		movq	red_green_shift,mm0

		movq	mm0,red_green_mask_565
		movq	red_green_mask,mm0

		lea		eax,MODIFY_FOR_RED_SHIFT_A
		mov		byte ptr[eax-1],11
		lea		eax,MODIFY_FOR_RED_SHIFT_B
		mov		byte ptr[eax-1],11
		lea		eax,MODIFY_FOR_RED_SHIFT_C
		mov		byte ptr[eax-1],11
		lea		eax,MODIFY_FOR_RED_SHIFT_D
		mov		byte ptr[eax-1],11
		lea		eax,MODIFY_FOR_RED_SHIFT_E
		mov		byte ptr[eax-1],11

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
		mov		ebx,[eax]TCopyPerspFilter.fxX

		mov		edx,[eax]TCopyPerspFilter.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]FakeTCopyPerspFilter.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]FakeTCopyPerspFilter.indCoord.fInvZ
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

		punpcklwd	mm4,mm3											// Width,1,Width,1
		movq		mm0,[ecx]FakeTCopyPerspFilter.indCoord.fUInvZ	// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4									// Save mfactor
		movd		mm1,[ecx]FakeTCopyPerspFilter.indCoord.fInvZ	// mm1 = (?,1/Z)

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
		// PERSPECTIVE_SCANLINE_PROLOGUE(TCopyPerspFilter)
		//

		//
		// Determine the start and end of the scanline. 
		//
		mov		ebx,[ecx]TCopyPerspFilter.fxX.i4Fx

		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,eax

		mov		eax,[ecx]TCopyPerspFilter.fxX.i4Fx
		sar		ebx,16

		sar		eax,16
		add		edx,ebx

		sub		eax,ebx									// eax == i_pixel
		jge		END_OF_SCANLINE

		// Load span increments.
		movq	mm2,[tvDEdge.UInvZ]						// Load V,U
		mov		[i_screen_index],edx					// Save scanline index.
	
		movd	mm3,[tvDEdge.InvZ]						// Load Z
		mov		[i_pixel],eax							// Save negative length of scanline.

		mov		edi,[iSubdivideLen]

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
		mov		edx,[bClampUV]					// Load clamp flag.

		pfadd	(m2,m0)							// Step fGUInvZ,fGVInvZ
		movd	[tvCurUVZ.InvZ],mm3				// Save updated 1/Z

		pfmul	(m4,m0)							// mm4 = V1,U1
		pfrcp	(m0,m3)							// f_next_z = mm0 = 1/fGInvZ

		movq	[tvCurUVZ.UInvZ],mm2					// Save updated U/Z,V/Z
		movd	mm1,float ptr[fInverseIntTable+edi*4]	// Reciprocal of span width.

		pfmul	(m0,m2)							// mm0 = V2,U2
		movq	mm2,[pfFixed16Scale]			// Load fixed point scale factors.

#if (VER_CLAMP_UV_16BIT == TRUE)
		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
		pfmax	(m4,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m4,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
#endif

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
		// eax = texel_index1		ebx = texel_index2
		// ecx = esi+twidth			edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor/temp		mm3 = temp
		// mm4 = temp				mm5 = u fracs
		// mm6 = uvslope/v fracs	mm7 = uv
		//

		movq		[qUVStep],mm6					// Save span slopes.

		// Setup usteps and vsteps
		movq		mm3,mm6
		movq		mm4,mm6

		punpcklwd	mm3,mm3
		psrlq		mm4,32

		punpckldq	mm3,mm3
		punpcklwd	mm4,mm4

		pxor		mm3,dword ptr[uf_xors]
		punpckldq	mm4,mm4

		paddw		mm3,dword ptr[uf_adds]

		pxor		mm4,dword ptr[vf_xors]

		paddw		mm4,dword ptr[vf_adds]

		movq		[usteps],mm3

		movq		[vsteps],mm4

		mov			ecx,[iTexWidth]

		lea			ecx,[esi+ecx*2]

		// Setup mm5 with u fractions, mm6 with v fractions, initial texture walk.
		movq		mm5,mm7
		movq		mm0,mm7							// Copy U1,V1

		paddd		mm7,mm6							// Step U,V

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values

		pmaddwd		mm0,[qMFactor]					// Compute texture indicies.

		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm6,mm5

		punpcklwd	mm5,mm5						// ?,du,?,du
		psrlq		mm6,32

		punpckldq	mm5,mm5						// du,du,du,du
		punpcklwd	mm6,mm6						// ?,dv,?,dv

		pxor		mm5,dword ptr[uf_xors]
		punpckldq	mm6,mm6						// dv,dv,dv,dv

		pxor		mm6,dword ptr[vf_xors]

		dec			esi							// Force sib+offset in loop.
		dec			ecx							// Force sib+offset in loop.

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 7 past the beginning of a cache line.
		ALIGN	16
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
		movd		mm0,[esi+eax*2+1]			// lookup texels (0,0,T1,T0)
		movq		mm3,mm5						// copy u fractions

		punpcklwd	mm0,[ecx+eax*2+1]			// lookup texels (T3, T1, T2, T0)
		movq		mm4,mm6						// copy v fractions

		psrlw		mm3,4						// shift ufracs to be unsigned
		paddw		mm5,[usteps]				// advance u fractions

		psrlw		mm4,4						// shift vfracs to be unsinged
		paddw		mm6,[vsteps]				// advance v fractions

		movq		mm2,mm0						// copy texels
		pand		mm0,[mask_greens]			// mask greens

		pmulhw		mm3,mm4						// combine u and v fractions (0x00FF == 1)
		movq		mm1,mm2						// copy texels

		pand		mm2,[mask_blues]			// mask blues
		psrlw		mm1,11						// shift reds into position (no need to mask)
MODIFY_FOR_RED_SHIFT_A:

		pmaddwd		mm1,mm3						// multiply and add reds (R0+R2, R1+R3)
		psrlw		mm0,5						// shift greens into positon

		pmaddwd		mm0,mm3						// multiply and add greens (G0+G2, G1+G3)
		movq		mm4,[red_green_shift]		// Load multiplication factors for shift.

		pmaddwd		mm2,mm3						// multiply and add blues (B0+B2, B1+B3)
		movq		mm3,[red_green_mask]		// Load masks for red and green.

		packssdw	mm1,mm0						// Pack reds and greens together (G0,G1,R0,R1)
		movd		mm0,[esi+ebx*2+1]			// lookup texels (0,0,T1,T0)

		pmaddwd		mm1,mm4						// Sum and shift reds and greens. (G, R)
		movq		mm4,mm2						// Copy blues.

		psrlq		mm4,32						// Shift high blues down.
		punpcklwd	mm0,[ecx+ebx*2+1]			// lookup texels (T3, T1, T2, T0)

		paddd		mm2,mm4						// Sum blues. (X, B)
		pand		mm1,mm3						// Mask reds and greens.

		por			mm2,mm1						// Combine blue and red.
		psrlq		mm1,32						// Shift green down.

		por			mm2,mm1						// Combine red with blue and green.
		movq		mm3,mm5						// copy u fractions

		psrld		mm2,8						// Shift into final position.
		movq		mm4,mm6						// copy v fractions

		movd		eax,mm2						// Get pixel value.
		psrlw		mm3,4						// shift ufracs to be unsigned

		mov			[edx + edi*2 - 4],ax		// Store pixel 1

		paddw		mm5,[usteps]				// advance u fractions
		nop										// for cache alignment

		psrlw		mm4,4						// shift vfracs to be unsinged
		paddw		mm6,[vsteps]				// advance v fractions

		movq		mm2,mm0						// copy texels
		pand		mm0,[mask_greens]			// mask greens

		pmulhw		mm3,mm4						// combine u and v fractions (0x00FF == 1)
		movq		mm1,mm2						// copy texels

		pand		mm2,[mask_blues]			// mask blues
		psrlw		mm1,11						// shift reds into position (no need to mask)
MODIFY_FOR_RED_SHIFT_B:

		pmaddwd		mm1,mm3						// multiply and add reds (R0+R2, R1+R3)
		psrlw		mm0,5						// shift greens into positon

		pmaddwd		mm0,mm3						// multiply and add greens (G0+G2, G1+G3)
		movq		mm4,[red_green_shift]		// Load multiplication factors for shift.

		pmaddwd		mm2,mm3						// multiply and add blues (B0+B2, B1+B3)
		movq		mm3,[red_green_mask]		// Load masks for red and green.

		packssdw	mm1,mm0						// Pack reds and greens together (G0,G1,R0,R1)
		movq		mm0,[qUVStep]				// Load texture step.

		pmaddwd		mm1,mm4						// Sum and shift reds and greens. (G, R)
		movq		mm4,mm2						// Copy blues.

		paddd		mm7,mm0						// Step U,V
		psrlq		mm4,32						// Shift high blues down.

		paddd		mm2,mm4						// Sum blues. (X, B)
		movq		mm4,mm7						// Copy U1,V1

		paddd		mm7,mm0						// Step U,V
		pand		mm1,mm3						// Mask reds and greens.

		psrad		mm4,16						// Shift for integer U1,V1
		movq		mm0,mm7						// Copy U2,V2

		por			mm2,mm1						// Combine blue and red.
		psrad		mm0,16						// Shift for integer U2,V2

		packssdw	mm4,mm0						// Pack integer texture values.
		movq		mm3,[qMFactor]				// Texture multipliers

		psrlq		mm1,32						// Shift green down.
		por			mm2,mm1						// Combine red with blue and green.

		pmaddwd		mm4,mm3						// Compute texture indicies.
		psrld		mm2,8						// Shift into final position.

		movq		[qIndexTemp],mm4			// Save texture indicies.
		movd		ebx,mm2						// Get pixel value.

		mov			eax,dword ptr[qIndexTemp]	// Load texel index 1.
		add			edi,2						// Inrcement index by 2

		mov			[edx + edi*2 - 6],bx		// Store pixel 2

		mov			ebx,dword ptr[qIndexTemp+4]	// Load texel index 2.
		jl			short INNER_LOOP_2P			// Loop (while >2 pixels)

FINISH_REMAINDER:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			short ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture co-ordinates once.
		//
		movd		mm0,[esi+eax*2+1]			// lookup texels (0,0,T1,T0)
		movq		mm3,mm5						// copy u fractions

		punpcklwd	mm0,[ecx+eax*2+1]			// lookup texels (T3, T1, T2, T0)
		movq		mm4,mm6						// copy v fractions

		psrlw		mm3,4						// shift ufracs to be unsigned
		paddw		mm5,[usteps]				// advance u fractions

		psrlw		mm4,4						// shift vfracs to be unsinged
		paddw		mm6,[vsteps]				// advance v fractions

		movq		mm2,mm0						// copy texels
		pand		mm0,[mask_greens]			// mask greens

		pmulhw		mm3,mm4						// combine u and v fractions (0x00FF == 1)
		movq		mm1,mm2						// copy texels

		pand		mm2,[mask_blues]			// mask blues
		psrlw		mm1,11						// shift reds into position (no need to mask)
MODIFY_FOR_RED_SHIFT_C:

		pmaddwd		mm1,mm3						// multiply and add reds (R0+R2, R1+R3)
		psrlw		mm0,5						// shift greens into positon

		pmaddwd		mm0,mm3						// multiply and add greens (G0+G2, G1+G3)
		movq		mm4,[red_green_shift]		// Load multiplication factors for shift.

		pmaddwd		mm2,mm3						// multiply and add blues (B0+B2, B1+B3)
		movq		mm3,[red_green_mask]		// Load masks for red and green.

		packssdw	mm1,mm0						// Pack reds and greens together (G0,G1,R0,R1)
		movd		mm0,[esi+ebx*2+1]			// lookup texels (0,0,T1,T0)

		pmaddwd		mm1,mm4						// Sum and shift reds and greens. (G, R)
		movq		mm4,mm2						// Copy blues.

		psrlq		mm4,32						// Shift high blues down.
		punpcklwd	mm0,[ecx+ebx*2+1]			// lookup texels (T3, T1, T2, T0)

		paddd		mm2,mm4						// Sum blues. (X, B)
		pand		mm1,mm3						// Mask reds and greens.

		por			mm2,mm1						// Combine blue and red.
		psrlq		mm1,32						// Shift green down.

		por			mm2,mm1						// Combine red with blue and green.
		movq		mm3,mm5						// copy u fractions

		psrld		mm2,8						// Shift into final position.
		movq		mm4,mm6						// copy v fractions

		movd		eax,mm2						// Get pixel value.
		psrlw		mm3,4						// shift ufracs to be unsigned

		mov			[edx + edi*2 - 4],ax		// Store pixel 1

		psrlw		mm4,4						// shift vfracs to be unsinged

		movq		mm2,mm0						// copy texels
		pand		mm0,[mask_greens]			// mask greens

		pmulhw		mm3,mm4						// combine u and v fractions (0x00FF == 1)
		movq		mm1,mm2						// copy texels

		pand		mm2,[mask_blues]			// mask blues
		psrlw		mm1,11						// shift reds into position (no need to mask)
MODIFY_FOR_RED_SHIFT_D:

		pmaddwd		mm1,mm3						// multiply and add reds (R0+R2, R1+R3)
		psrlw		mm0,5						// shift greens into positon

		pmaddwd		mm0,mm3						// multiply and add greens (G0+G2, G1+G3)
		movq		mm4,[red_green_shift]		// Load multiplication factors for shift.

		pmaddwd		mm2,mm3						// multiply and add blues (B0+B2, B1+B3)
		movq		mm3,[red_green_mask]		// Load masks for red and green.

		packssdw	mm1,mm0						// Pack reds and greens together (G0,G1,R0,R1)
		movq		mm0,[qUVStep]				// Load texture step.

		pmaddwd		mm1,mm4						// Sum and shift reds and greens. (G, R)
		movq		mm4,mm2						// Copy blues.

		paddd		mm7,mm0						// Step U,V
		psrlq		mm4,32						// Shift high blues down.

		paddd		mm2,mm4						// Sum blues. (X, B)

		pand		mm1,mm3						// Mask reds and greens.

		por			mm2,mm1						// Combine blue and red.

		psrlq		mm1,32						// Shift green down.
		por			mm2,mm1						// Combine red with blue and green.

		psrld		mm2,8						// Shift into final position.

		movd		ebx,mm2						// Get pixel value.

		mov			[edx + edi*2 - 2],bx		// Store pixel 2

		mov			eax,[i_pixel]
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movd		mm0,[esi+eax*2+1]			// lookup texels (0,0,T1,T0)
		movq		mm3,mm5						// copy u fractions

		punpcklwd	mm0,[ecx+eax*2+1]			// lookup texels (T3, T1, T2, T0)
		movq		mm4,mm6						// copy v fractions

		psrlw		mm3,4						// shift ufracs to be unsigned

		psrlw		mm4,4						// shift vfracs to be unsinged

		movq		mm2,mm0						// copy texels
		pand		mm0,[mask_greens]			// mask greens

		pmulhw		mm3,mm4						// combine u and v fractions (0x00FF == 1)
		movq		mm1,mm2						// copy texels

		pand		mm2,[mask_blues]			// mask blues
		psrlw		mm1,11						// shift reds into position (no need to mask)
MODIFY_FOR_RED_SHIFT_E:

		pmaddwd		mm1,mm3						// multiply and add reds (R0+R2, R1+R3)
		psrlw		mm0,5						// shift greens into positon

		pmaddwd		mm0,mm3						// multiply and add greens (G0+G2, G1+G3)
		movq		mm4,[red_green_shift]		// Load multiplication factors for shift.

		pmaddwd		mm2,mm3						// multiply and add blues (B0+B2, B1+B3)
		movq		mm3,[red_green_mask]		// Load masks for red and green.

		packssdw	mm1,mm0						// Pack reds and greens together (G0,G1,R0,R1)

		pmaddwd		mm1,mm4						// Sum and shift reds and greens. (G, R)
		movq		mm4,mm2						// Copy blues.

		psrlq		mm4,32						// Shift high blues down.

		paddd		mm2,mm4						// Sum blues. (X, B)
		pand		mm1,mm3						// Mask reds and greens.

		por			mm2,mm1						// Combine blue and red.
		psrlq		mm1,32						// Shift green down.

		por			mm2,mm1						// Combine red with blue and green.

		psrld		mm2,8						// Shift into final position.

		movd		eax,mm2						// Get pixel value.

		mov			[edx + edi*2 - 4],ax		// Store pixel 1

		mov			eax,[i_pixel]

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		jz		short END_OF_SCANLINE

		//
		// Do perspective correction and looping.
		// PERSPECTIVE_SCANLINE_EPILOGUE
		//

		//---------------------------------------------------------------------------------- 
		// Implement perspective correction. 
		//
		// eax = i_pixel
		// Preserve: mm7
		//
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

#if (VER_CLAMP_UV_16BIT == TRUE)
		movq	mm3,[pfTexEdgeTolerance]
		movq	mm5,[cvMaxCoords]
		pfmax	(m0,m3)							// Clamp U1,V1 to >= fTexEdgeTolerance
		pfmin	(m0,m5)							// Clamp U1 < fTexWidth, V1 < fTexHeight
#endif

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

		mov		eax,[ecx]TCopyPerspFilter.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TCopyPerspFilter.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]FakeTCopyPerspFilter.indCoord.fUInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm1,[ecx]FakeTCopyPerspFilter.indCoord.fInvZ

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
		movq	[ecx]FakeTCopyPerspFilter.indCoord.fUInvZ,mm0

		movd	[ecx]FakeTCopyPerspFilter.indCoord.fInvZ,mm1

		mov		[ecx]TCopyPerspFilter.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TCopyPerspFilter.fxXDifference.i4Fx,ebx
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
// Linear, no clut, non-transparent, filtered.
//
//**********************************
void DrawSubtriangle(TCopyLinearFilter* pscan, CDrawPolygon<TCopyLinearFilter>* pdtri)
{
	typedef TCopyLinearFilter::TIndex prevent_internal_compiler_errors;
	static int iBits = 16;
	static CDrawPolygon<TCopyLinearFilter>* pdtriGlbl;
	static TCopyLinearFilter* pscanGlbl;
	static PackedFloatUV pf_texture_inc;

	TCopyLinearFilter* plinc = &pdtri->pedgeBase->lineIncrement;

	//
	// This type is the same as TCopyLinearFilter, except that it uses the parent class of
	// CIndexLinearFilter to allow us to access members of it.
	//
	typedef CScanline<uint16, CGouraudNone, CTransparencyOff,
			      CMapTexture<uint16>, CIndexLinear, CColLookupOff>      FakeTCopyLinearFilter;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		// Check/Modify for 555/565 color.
		mov		eax,gsGlobals.iBits

		cmp		eax,[iBits]
		je		short DONE_WITH_MODIFY

		mov		[iBits],eax

		cmp		eax,16
		je		MODIFY_FOR_565

		// Change to 555.
		movq	mm0,mask_greens_555
		movq	mask_greens,mm0

		movq	mm0,red_green_shift_555
		movq	red_green_shift,mm0

		movq	mm0,red_green_mask_555
		movq	red_green_mask,mm0

		lea		eax,MODIFY_FOR_RED_SHIFT_A
		mov		byte ptr[eax-1],10
		lea		eax,MODIFY_FOR_RED_SHIFT_B
		mov		byte ptr[eax-1],10
		lea		eax,MODIFY_FOR_RED_SHIFT_C
		mov		byte ptr[eax-1],10
		lea		eax,MODIFY_FOR_RED_SHIFT_D
		mov		byte ptr[eax-1],10
		lea		eax,MODIFY_FOR_RED_SHIFT_E
		mov		byte ptr[eax-1],10
		jmp		short DONE_WITH_MODIFY

MODIFY_FOR_565:
		// Change to 565.
		movq	mm0,mask_greens_565
		movq	mask_greens,mm0

		movq	mm0,red_green_shift_565
		movq	red_green_shift,mm0

		movq	mm0,red_green_mask_565
		movq	red_green_mask,mm0

		lea		eax,MODIFY_FOR_RED_SHIFT_A
		mov		byte ptr[eax-1],11
		lea		eax,MODIFY_FOR_RED_SHIFT_B
		mov		byte ptr[eax-1],11
		lea		eax,MODIFY_FOR_RED_SHIFT_C
		mov		byte ptr[eax-1],11
		lea		eax,MODIFY_FOR_RED_SHIFT_D
		mov		byte ptr[eax-1],11
		lea		eax,MODIFY_FOR_RED_SHIFT_E
		mov		byte ptr[eax-1],11

DONE_WITH_MODIFY:
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
		mov		ebx,[edi]TCopyLinearFilter.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TCopyLinearFilter.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[edi]FakeTCopyLinearFilter.indCoord.pfIndex
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

		movq		mm7,[eax]FakeTCopyLinearFilter.indCoord.pfIndex
		movq		[qMFactor],mm2					// Save mfactor

		movq		[qUVStep],mm6					// Save slopes

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TCopyLinearFilter.fxX.i4Fx

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
		// eax = texel_index1		ebx = texel_index2
		// ecx = esi+twidth			edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = base pointer
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor/temp		mm3 = temp
		// mm4 = temp				mm5 = u fracs
		// mm6 = uvslope/v fracs	mm7 = uv
		//
		movq		mm6,[qUVStep]					// Load slopes
		pf2id		(m7,m7)							// Convert texture values to 16.16 fixed.

		// Setup usteps and vsteps
		movq		mm3,mm6
		movq		mm4,mm6

		punpcklwd	mm3,mm3
		psrlq		mm4,32

		punpckldq	mm3,mm3
		punpcklwd	mm4,mm4

		pxor		mm3,dword ptr[uf_xors]
		punpckldq	mm4,mm4

		paddw		mm3,dword ptr[uf_adds]

		pxor		mm4,dword ptr[vf_xors]

		paddw		mm4,dword ptr[vf_adds]

		movq		[usteps],mm3

		movq		[vsteps],mm4

		mov			esi,[pvTextureBitmap]			// Load texture ponter.

		mov			ecx,[iTexWidth]

		lea			ecx,[esi+ecx*2]

		// Setup mm5 with u fractions, mm6 with v fractions, initial texture walk.
		movq		mm5,mm7
		movq		mm0,mm7							// Copy U1,V1

		paddd		mm7,mm6							// Step U,V

		psrad		mm0,16							// Shift for integer U1,V1
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values

		pmaddwd		mm0,[qMFactor]					// Compute texture indicies.

		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1

		mov			ebx,dword ptr[qIndexTemp + 4]	// Get texture index 2
		movq		mm6,mm5

		punpcklwd	mm5,mm5						// ?,du,?,du
		psrlq		mm6,32

		punpckldq	mm5,mm5						// du,du,du,du
		punpcklwd	mm6,mm6						// ?,dv,?,dv

		pxor		mm5,dword ptr[uf_xors]
		punpckldq	mm6,mm6						// dv,dv,dv,dv

		pxor		mm6,dword ptr[vf_xors]

		dec			esi							// Force sib+offset in loop.
		dec			ecx							// Force sib+offset in loop.

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 7 past the beginning of a cache line.
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

		//
		// Do two pixels at a time.
		//
		// mm0 = packed & masked U1,V1,U2,V2
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movd		mm0,[esi+eax*2+1]			// lookup texels (0,0,T1,T0)
		movq		mm3,mm5						// copy u fractions

		punpcklwd	mm0,[ecx+eax*2+1]			// lookup texels (T3, T1, T2, T0)
		movq		mm4,mm6						// copy v fractions

		psrlw		mm3,4						// shift ufracs to be unsigned
		paddw		mm5,[usteps]				// advance u fractions

		psrlw		mm4,4						// shift vfracs to be unsinged
		paddw		mm6,[vsteps]				// advance v fractions

		movq		mm2,mm0						// copy texels
		pand		mm0,[mask_greens]			// mask greens

		pmulhw		mm3,mm4						// combine u and v fractions (0x00FF == 1)
		movq		mm1,mm2						// copy texels

		pand		mm2,[mask_blues]			// mask blues
		psrlw		mm1,11						// shift reds into position (no need to mask)
MODIFY_FOR_RED_SHIFT_A:

		pmaddwd		mm1,mm3						// multiply and add reds (R0+R2, R1+R3)
		psrlw		mm0,5						// shift greens into positon

		pmaddwd		mm0,mm3						// multiply and add greens (G0+G2, G1+G3)
		movq		mm4,[red_green_shift]		// Load multiplication factors for shift.

		pmaddwd		mm2,mm3						// multiply and add blues (B0+B2, B1+B3)
		movq		mm3,[red_green_mask]		// Load masks for red and green.

		packssdw	mm1,mm0						// Pack reds and greens together (G0,G1,R0,R1)
		movd		mm0,[esi+ebx*2+1]			// lookup texels (0,0,T1,T0)

		pmaddwd		mm1,mm4						// Sum and shift reds and greens. (G, R)
		movq		mm4,mm2						// Copy blues.

		psrlq		mm4,32						// Shift high blues down.
		punpcklwd	mm0,[ecx+ebx*2+1]			// lookup texels (T3, T1, T2, T0)

		paddd		mm2,mm4						// Sum blues. (X, B)
		pand		mm1,mm3						// Mask reds and greens.

		por			mm2,mm1						// Combine blue and red.
		psrlq		mm1,32						// Shift green down.

		por			mm2,mm1						// Combine red with blue and green.
		movq		mm3,mm5						// copy u fractions

		psrld		mm2,8						// Shift into final position.
		movq		mm4,mm6						// copy v fractions

		movd		eax,mm2						// Get pixel value.
		psrlw		mm3,4						// shift ufracs to be unsigned

		mov			[edx + edi*2 - 4],ax		// Store pixel 1

		paddw		mm5,[usteps]				// advance u fractions
		nop										// for cache alignment

		psrlw		mm4,4						// shift vfracs to be unsinged
		paddw		mm6,[vsteps]				// advance v fractions

		movq		mm2,mm0						// copy texels
		pand		mm0,[mask_greens]			// mask greens

		pmulhw		mm3,mm4						// combine u and v fractions (0x00FF == 1)
		movq		mm1,mm2						// copy texels

		pand		mm2,[mask_blues]			// mask blues
		psrlw		mm1,11						// shift reds into position (no need to mask)
MODIFY_FOR_RED_SHIFT_B:

		pmaddwd		mm1,mm3						// multiply and add reds (R0+R2, R1+R3)
		psrlw		mm0,5						// shift greens into positon

		pmaddwd		mm0,mm3						// multiply and add greens (G0+G2, G1+G3)
		movq		mm4,[red_green_shift]		// Load multiplication factors for shift.

		pmaddwd		mm2,mm3						// multiply and add blues (B0+B2, B1+B3)
		movq		mm3,[red_green_mask]		// Load masks for red and green.

		packssdw	mm1,mm0						// Pack reds and greens together (G0,G1,R0,R1)
		movq		mm0,[qUVStep]				// Load texture step.

		pmaddwd		mm1,mm4						// Sum and shift reds and greens. (G, R)
		movq		mm4,mm2						// Copy blues.

		paddd		mm7,mm0						// Step U,V
		psrlq		mm4,32						// Shift high blues down.

		paddd		mm2,mm4						// Sum blues. (X, B)
		movq		mm4,mm7						// Copy U1,V1

		paddd		mm7,mm0						// Step U,V
		pand		mm1,mm3						// Mask reds and greens.

		psrad		mm4,16						// Shift for integer U1,V1
		movq		mm0,mm7						// Copy U2,V2

		por			mm2,mm1						// Combine blue and red.
		psrad		mm0,16						// Shift for integer U2,V2

		packssdw	mm4,mm0						// Pack integer texture values.
		movq		mm3,[qMFactor]				// Texture multipliers

		psrlq		mm1,32						// Shift green down.
		por			mm2,mm1						// Combine red with blue and green.

		pmaddwd		mm4,mm3						// Compute texture indicies.
		psrld		mm2,8						// Shift into final position.

		movq		[qIndexTemp],mm4			// Save texture indicies.
		movd		ebx,mm2						// Get pixel value.

		mov			eax,dword ptr[qIndexTemp]	// Load texel index 1.
		add			edi,2						// Inrcement index by 2

		mov			[edx + edi*2 - 6],bx		// Store pixel 2

		mov			ebx,dword ptr[qIndexTemp+4]	// Load texel index 2.
		jl			short INNER_LOOP_2P			// Loop (while >2 pixels)

FINISH_REMAINDER:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			short ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture co-ordinates once.
		//
		movd		mm0,[esi+eax*2+1]			// lookup texels (0,0,T1,T0)
		movq		mm3,mm5						// copy u fractions

		punpcklwd	mm0,[ecx+eax*2+1]			// lookup texels (T3, T1, T2, T0)
		movq		mm4,mm6						// copy v fractions

		psrlw		mm3,4						// shift ufracs to be unsigned
		paddw		mm5,[usteps]				// advance u fractions

		psrlw		mm4,4						// shift vfracs to be unsinged
		paddw		mm6,[vsteps]				// advance v fractions

		movq		mm2,mm0						// copy texels
		pand		mm0,[mask_greens]			// mask greens

		pmulhw		mm3,mm4						// combine u and v fractions (0x00FF == 1)
		movq		mm1,mm2						// copy texels

		pand		mm2,[mask_blues]			// mask blues
		psrlw		mm1,11						// shift reds into position (no need to mask)
MODIFY_FOR_RED_SHIFT_C:

		pmaddwd		mm1,mm3						// multiply and add reds (R0+R2, R1+R3)
		psrlw		mm0,5						// shift greens into positon

		pmaddwd		mm0,mm3						// multiply and add greens (G0+G2, G1+G3)
		movq		mm4,[red_green_shift]		// Load multiplication factors for shift.

		pmaddwd		mm2,mm3						// multiply and add blues (B0+B2, B1+B3)
		movq		mm3,[red_green_mask]		// Load masks for red and green.

		packssdw	mm1,mm0						// Pack reds and greens together (G0,G1,R0,R1)
		movd		mm0,[esi+ebx*2+1]			// lookup texels (0,0,T1,T0)

		pmaddwd		mm1,mm4						// Sum and shift reds and greens. (G, R)
		movq		mm4,mm2						// Copy blues.

		psrlq		mm4,32						// Shift high blues down.
		punpcklwd	mm0,[ecx+ebx*2+1]			// lookup texels (T3, T1, T2, T0)

		paddd		mm2,mm4						// Sum blues. (X, B)
		pand		mm1,mm3						// Mask reds and greens.

		por			mm2,mm1						// Combine blue and red.
		psrlq		mm1,32						// Shift green down.

		por			mm2,mm1						// Combine red with blue and green.
		movq		mm3,mm5						// copy u fractions

		psrld		mm2,8						// Shift into final position.
		movq		mm4,mm6						// copy v fractions

		movd		eax,mm2						// Get pixel value.
		psrlw		mm3,4						// shift ufracs to be unsigned

		mov			[edx + edi*2 - 4],ax		// Store pixel 1

		psrlw		mm4,4						// shift vfracs to be unsinged

		movq		mm2,mm0						// copy texels
		pand		mm0,[mask_greens]			// mask greens

		pmulhw		mm3,mm4						// combine u and v fractions (0x00FF == 1)
		movq		mm1,mm2						// copy texels

		pand		mm2,[mask_blues]			// mask blues
		psrlw		mm1,11						// shift reds into position (no need to mask)
MODIFY_FOR_RED_SHIFT_D:

		pmaddwd		mm1,mm3						// multiply and add reds (R0+R2, R1+R3)
		psrlw		mm0,5						// shift greens into positon

		pmaddwd		mm0,mm3						// multiply and add greens (G0+G2, G1+G3)
		movq		mm4,[red_green_shift]		// Load multiplication factors for shift.

		pmaddwd		mm2,mm3						// multiply and add blues (B0+B2, B1+B3)
		movq		mm3,[red_green_mask]		// Load masks for red and green.

		packssdw	mm1,mm0						// Pack reds and greens together (G0,G1,R0,R1)
		movq		mm0,[qUVStep]				// Load texture step.

		pmaddwd		mm1,mm4						// Sum and shift reds and greens. (G, R)
		movq		mm4,mm2						// Copy blues.

		paddd		mm7,mm0						// Step U,V
		psrlq		mm4,32						// Shift high blues down.

		paddd		mm2,mm4						// Sum blues. (X, B)

		pand		mm1,mm3						// Mask reds and greens.

		por			mm2,mm1						// Combine blue and red.

		psrlq		mm1,32						// Shift green down.
		por			mm2,mm1						// Combine red with blue and green.

		psrld		mm2,8						// Shift into final position.

		movd		ebx,mm2						// Get pixel value.

		mov			[edx + edi*2 - 2],bx		// Store pixel 2

		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movd		mm0,[esi+eax*2+1]			// lookup texels (0,0,T1,T0)
		movq		mm3,mm5						// copy u fractions

		punpcklwd	mm0,[ecx+eax*2+1]			// lookup texels (T3, T1, T2, T0)
		movq		mm4,mm6						// copy v fractions

		psrlw		mm3,4						// shift ufracs to be unsigned

		psrlw		mm4,4						// shift vfracs to be unsinged

		movq		mm2,mm0						// copy texels
		pand		mm0,[mask_greens]			// mask greens

		pmulhw		mm3,mm4						// combine u and v fractions (0x00FF == 1)
		movq		mm1,mm2						// copy texels

		pand		mm2,[mask_blues]			// mask blues
		psrlw		mm1,11						// shift reds into position (no need to mask)
MODIFY_FOR_RED_SHIFT_E:

		pmaddwd		mm1,mm3						// multiply and add reds (R0+R2, R1+R3)
		psrlw		mm0,5						// shift greens into positon

		pmaddwd		mm0,mm3						// multiply and add greens (G0+G2, G1+G3)
		movq		mm4,[red_green_shift]		// Load multiplication factors for shift.

		pmaddwd		mm2,mm3						// multiply and add blues (B0+B2, B1+B3)
		movq		mm3,[red_green_mask]		// Load masks for red and green.

		packssdw	mm1,mm0						// Pack reds and greens together (G0,G1,R0,R1)

		pmaddwd		mm1,mm4						// Sum and shift reds and greens. (G, R)
		movq		mm4,mm2						// Copy blues.

		psrlq		mm4,32						// Shift high blues down.

		paddd		mm2,mm4						// Sum blues. (X, B)
		pand		mm1,mm3						// Mask reds and greens.

		por			mm2,mm1						// Combine blue and red.
		psrlq		mm1,32						// Shift green down.

		por			mm2,mm1						// Combine red with blue and green.

		psrld		mm2,8						// Shift into final position.

		movd		eax,mm2						// Get pixel value.

		mov			[edx + edi*2 - 4],ax		// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]FakeTCopyLinearFilter.indCoord.pfIndex

		mov		ebx,[eax]TCopyLinearFilter.fxXDifference

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
		mov		[eax]TCopyLinearFilter.fxXDifference,ebx

		movq	[eax]FakeTCopyLinearFilter.indCoord.pfIndex,mm7
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TCopyLinearFilter.fxX
		mov		edx,[fx_inc]

		add		ecx,edx
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TCopyLinearFilter.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}

#pragma code_seg()

#endif // if (BILINEAR_FILTER)
