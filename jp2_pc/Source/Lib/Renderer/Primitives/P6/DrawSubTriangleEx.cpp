/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function.
 *		Optimized for the Pentium Pro Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P6/DrawSubTriangleEx.cpp                      $
 * 
 * 14    97/12/03 2:54p Pkeet
 * Fixed crash in copy primitive.
 * 
 * 13    97/12/03 1:25p Pkeet
 * New P6 optimized version created from P5 version.
 * 
 * 12    97.12.03 1:15p Mmouni
 * 
 * 11    97.11.05 7:52p Mmouni
 * Added terrain fog mask.
 * 
 * 10    10/01/97 2:44p Mmouni
 * Added seperate terrain specific copy primitives.
 * 
 * 9     9/01/97 8:05p Rwyatt
 * 
 * 8     8/19/97 9:08p Bbell
 * Added asserts.
 * 
 * 7     8/17/97 10:39p Rwyatt
 * Removed old unused labels
 * 
 * 6     8/15/97 8:43p Bbell
 * Added fogging for terrain texturing.
 * 
 * 5     8/15/97 12:44a Rwyatt
 * Big optimizations, all 16 bit primitives are now optimized and perspective is in line.
 * Clamping in the perspective code is controlled by VER_CLAMP_UV_16BIT this is independant of
 * clamping in the general perspective code.
 * 
 * 4     8/01/97 6:55p Rwyatt
 * Linear, No Clut, Non-transparent has had its scan lines optimized for Pentium Pro. Y loop
 * still needs doing.
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


//*****************************************************************************************
//
// Perspective, No Clut, Transparent.
//
void DrawSubtriangle(TCopyPerspTrans* pscan, CDrawPolygon<TCopyPerspTrans>* pdtri)
{
	fixed fx_inc;
	fixed fx_diff;
	float f_inc_uinvz;
	float f_inc_vinvz;
	float f_inc_invz;

	TCopyPerspTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
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
		mov		fx_diff,edx

		mov		ebx,[eax]TCopyPerspTrans.indCoord.fUInvZ
		mov		edx,[eax]TCopyPerspTrans.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		ebx,[eax]TCopyPerspTrans.indCoord.fInvZ

		mov		f_inc_vinvz,edx
		mov		f_inc_invz,ebx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		push	ebp									// Save ebp.

		// Check to see if we sould skip this scanline.
		mov		eax,[esi]CDrawPolygonBase.iY
		mov		ebx,[bEvenScanlinesOnly]

		and		eax,ebx
		jnz		END_OF_SCANLINE

		mov		ebx,[ecx]TCopyPerspTrans.fxX.i4Fx
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		add		ebx,eax
		mov		eax,[ecx]TCopyPerspTrans.fxX.i4Fx

		sar		ebx,16
		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		eax,16
		add		edx,ebx

		sub		eax,ebx									// i_pixel
		jge		END_OF_SCANLINE

		mov		[i_screen_index],edx
		mov		[i_pixel],eax

		//---------------------------------------------------------------------------
		// Caclulate values for the first correction span, and start the divide for
		// the next span.
		//

		// Copy global texture values.
		mov		ebx,[ecx]TCopyPerspTrans.indCoord.fUInvZ
		mov		esi,[ecx]TCopyPerspTrans.indCoord.fVInvZ

		mov		edi,[ecx]TCopyPerspTrans.indCoord.fInvZ
		mov		fGUInvZ,ebx

		mov		fGVInvZ,esi
		mov		fGInvZ,edi

		mov		ebx,fDUInvZEdge
		mov		esi,fDVInvZEdge

		mov		edi,fDInvZEdge
		mov		fDUInvZScanline,ebx

		mov		fDVInvZScanline,esi
		mov		fDInvZScanline,edi

		mov		ebx,[iSubdivideLen]

		// scan line is +ve
		add		eax,ebx
		jle		short DONE_DIVIDE_PIXEL

		// calc the new +ve ratio
		fild	[i_pixel]
		fld		fInvSubdivideLen
		fchs
		fmulp	st(1),st(0)			// st(0) = (-)fInvSubdivideLen * i_pixel;
		fld		fDUInvZScanline		// U C
		xor		eax,eax
		fmul	st(0),st(1)			// U*C C
		fxch	st(1)				// C U*C
		fld		fDVInvZScanline		// V C U*C
		fxch	st(1)				// C V U*C
		fmul	st(1),st(0)			// C V*C U*C
		// stall(1)
		fmul	fDInvZScanline		// Z*C V*C U*C
		fxch	st(2)				// U*C V*C Z*C
		fstp	fDUInvZScanline		// V*C Z*C
		fstp	fDVInvZScanline		// Z*C
		fstp	fDInvZScanline

DONE_DIVIDE_PIXEL:
		// Get current u, v and z values.
		mov		[iNextSubdivide],eax
		mov		ebx,dword ptr[fGInvZ]		// f_z = fInverse(fGInvZ);

		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		fld		[fGUInvZ]					// U/Z

		add		eax,dword ptr[i4InverseMantissa + ebx*4]
		mov		ebx,[bClampUV]

		mov		dword ptr[f_z],eax

#if (VER_CLAMP_UV_16BIT==TRUE)

		//---------------------------------------------------------------------------
		// Set current texture coordinates (clamped).
		fmul	[f_z]
		fld		[fGVInvZ]
		fmul	[f_z]
		fxch	st(1)
		// stall(1)
		fstp	[f_u]

		// Clamp f_u			
		mov		eax,[f_u]
		mov		ebx,fTexEdgeTolerance
		mov		ecx,fTexWidth
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ecx
		cmovg	(_eax,_ecx)
		mov		ecx,fTexHeight
		mov		[f_u],eax
		fld		[f_u]

		fadd	[dFastFixed16Conversion]
		fxch	st(1)

		// Clamp f_v
		fstp	[f_v]
		mov		eax,[f_v]
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ecx
		cmovg	(_eax,_ecx)
		mov		[f_v],eax
		fld		[f_v]

		fadd	[dFastFixed16Conversion]
		fxch	st(1)
		fstp	[d_temp_a]
		fld		[fGUInvZ]
		fxch	st(1)
		fstp	[d_temp_b]

		// Increment u, v and z values.
		fadd	[fDUInvZScanline]
		fld		[fGVInvZ]
		fadd	[fDVInvZScanline]
		fxch	st(1)
		fld		[fGInvZ]
		fadd	[fDInvZScanline]

		// Setup esi=uFrac, ecx=vFrac, edx=UVInt for Abrash texture loop.
		mov		edx,dword ptr[d_temp_a]			// U (16.16)
		mov		eax,dword ptr[d_temp_b]			// V (16.16)

		sar		eax,16							// Integral V
		mov		ecx,[iTexWidth]					// Texture width.

		imul	eax,ecx							// iv*twidth

		sar		edx,16							// Integral U
		mov		esi,dword ptr[d_temp_a]			// Copy of U

		shl		esi,16							// UFrac
		mov		ecx,dword ptr[d_temp_b]			// Copy of V

		shl		ecx,16							// VFrac
		mov		ebx,[pvTextureBitmap]			// Texture base pointer.

		shr		ebx,1							// Into pixel offset.
		add		edx,eax							// edx = iu + iv*twidth

		add		edx,ebx							// Add texture base to edx.

		fstp	[fGInvZ]

		// Get next u, v and z values.
		mov		ebx,dword ptr[fGInvZ]			// f_next_z = fInverse(fGInvZ);
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		fst		[fGUInvZ]

		mov		ebx,dword ptr[i4InverseMantissa + ebx*4]
		mov		edi,[i_pixel]

		add		eax,ebx
		mov		ebx,[iNextSubdivide]

		mov		dword ptr[f_next_z],eax
		sub		edi,ebx

		// Set new texture coordinate increments.
		fmul	[f_next_z]
		fxch	st(1)
		fst		[fGVInvZ]
		fmul	[f_next_z]
		fxch	st(1)
		fstp	[fU]			// V

		// Clamp fU			
		mov		eax,[fU]
		mov		ebx,fTexEdgeTolerance
		mov		ebp,fTexWidth
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ebp
		cmovg	(_eax,_ebp)
		mov		ebp,fTexHeight
		mov		[fU],eax

		fld		[fU]							// U,V
		fsub	[f_u]							// U-fu,V
		fxch	st(1)							// V,U-fu
		fstp	[fV]							// U-fu

		// Clamp fV
		mov		eax,[fV]
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ebp
		cmovg	(_eax,_ebp)
		mov		[fV],eax

		fld		[fV]							// V U-fu

#else // (VER_CLAMP_UV_16BIT==TRUE)

		//---------------------------------------------------------------------------
		// Set current texture coordinates (not clamped).
		fmul	[f_z]						// U
		fld		[fGVInvZ]					// V/Z U
		fmul	[f_z]						// V U
		fxch	st(1)						// U V
		// stall(1)
		fst		[f_u]
		fadd	[dFastFixed16Conversion]	// iU V
		fxch	st(1)						// V iU
		fst		[f_v]						
		fadd	[dFastFixed16Conversion]	// iV iU
		fxch	st(1)						// iU iV
		fstp	[d_temp_a]					// iV
		fld		[fGUInvZ]					// U/Z iV
		fxch	st(1)						// iV U/Z
		fstp	[d_temp_b]					// U/Z

		// Increment u, v and z values.
		fadd	[fDUInvZScanline]			// U2/Z
		fld		[fGVInvZ]
		fadd	[fDVInvZScanline]			// V2/Z U2/Z
		fxch	st(1)						// U2/Z V2/Z
		fld		[fGInvZ]					// 1/Z U2/Z V2/Z
		fadd	[fDInvZScanline]			// 1/Z2 U2/Z V2/Z

		// Setup esi=uFrac, ecx=vFrac, edx=UVInt for Abrash texture loop.
		mov		edx,dword ptr[d_temp_a]			// U (16.16)
		mov		eax,dword ptr[d_temp_b]			// V (16.16)

		sar		eax,16							// Integral V
		mov		ecx,[iTexWidth]					// Texture width.

		imul	eax,ecx							// iv*twidth

		sar		edx,16							// Integral U
		mov		esi,dword ptr[d_temp_a]			// Copy of U

		shl		esi,16							// UFrac
		mov		ecx,dword ptr[d_temp_b]			// Copy of V

		shl		ecx,16							// VFrac
		mov		ebx,[pvTextureBitmap]			// Texture base pointer.

		shr		ebx,1							// Into pixel offset.
		add		edx,eax							// iu + iv*twidth

		add		edx,ebx							// Add to edx.

		fstp	[fGInvZ]

		// Get next u, v and z values.
		mov		ebx,dword ptr[fGInvZ]			// f_next_z = fInverse(fGInvZ);
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		fst		[fGUInvZ]

		mov		ebx,dword ptr[i4InverseMantissa + ebx*4]
		mov		edi,[i_pixel]

		add		eax,ebx
		mov		ebx,[iNextSubdivide]

		mov		dword ptr[f_next_z],eax
		sub		edi,ebx

		// Set new texture coordinate increments.
		fmul	[f_next_z]						// U,V/Z
		fxch	st(1)							// V/Z,U
		fst		[fGVInvZ]
		fmul	[f_next_z]						// V,U
		fxch	st(1)							// U,V
		fst		[fU]							// U,V
		fsub	[f_u]							// U-fu,V
		fxch	st(1)							// V,U-fu
		fst		[fV]

#endif // (VER_CLAMP_UV_16BIT==TRUE)

		// ---------------------------------------------------------------------------------
		// Both clmaped and non-clamped primitives end up here..
		fsub	[f_v]								// V-fv,U-fu
		fxch	st(1)								// U,V
		fmul	float ptr[fInverseIntTable+edi*4]	// (C*U),V
		fxch	st(1)								// V,(C*U)
		// stall(1)
		fmul	float ptr[fInverseIntTable+edi*4]	// (C*V),(C*U)
		fxch	st(1)								// (C*U),(C*V)
		fadd	[dFastFixed16Conversion]
		fxch	st(1)
		// stall(1)
		fadd	[dFastFixed16Conversion]
		fxch	st(1)
		// stall(1)
		fstp	[d_temp_a]
		fstp	[d_temp_b]

		mov		edi,dword ptr[d_temp_a]			// uslope
		mov		eax,dword ptr[d_temp_b]			// vslope

		sar		edi,16							// integer part of uslope
		mov		ebp,dword ptr[d_temp_a]			// uslope again

		shl		ebp,16							// fractional part of uslope
		mov		ebx,eax							// vslope again

		sar		eax,16							// integer part of vslope
		mov		[w2dDeltaTex.uUFrac],ebp		// store UFrac

		shl		ebx,16							// fractional part of vslope
		mov		ebp,[iTexWidth]					// Load texture width.

		imul	eax,ebp							// ivslope*twidth

		add		edi,eax							// ivslope*twidth + iuslope
		mov		[w2dDeltaTex.uVFrac],ebx		// store VFrac

		mov		[w2dDeltaTex.iUVInt+4],edi		// store integer stride
		add		edi,ebp							// add twidth to integer stride

		mov		[w2dDeltaTex.iUVInt],edi		// store integer stride + twidth
		mov		eax,[iNextSubdivide]

		test	eax,eax							// Next subdivision is zero length?
		jz		SUBDIVISION_LOOP

		//iCacheNextSubdivide = iSetNextDividePixel(iNextSubdivide);
		mov		edi,[iSubdivideLen]

		// scan line is +ve
		add		eax,edi
		jle		short DONE_DIVIDE_PIXEL_CACHE

		// calc the new +ve ratio
		fild	iNextSubdivide
		fld		fInvSubdivideLen
		fchs
		fmulp	st(1),st(0)			// st(0) = (-)fInvSubdivideLen * i_pixel;
		fld		fDUInvZScanline		// U C

		xor		eax,eax

		fmul	st(0),st(1)			// U*C C
		fxch	st(1)				// C U*C
		fld		fDVInvZScanline		// V C U*C
		fxch	st(1)				// C V U*C
		fmul	st(1),st(0)			// C V*C U*C

		mov		[iCacheNextSubdivide],eax

		fmul	fDInvZScanline		// Z*C V*C U*C
		fxch	st(2)				// U*C V*C Z*C
		fst		fDUInvZScanline
		fadd	fGUInvZ
		fxch	st(1)
		fst		fDVInvZScanline
		fadd	fGVInvZ
		fxch	st(2)
		fst		fDInvZScanline
		fadd	fGInvZ
		fxch	st(2)
		fstp	fGVInvZ			
		fstp	fGUInvZ
		fst		fGInvZ
		fdivr	fOne							// Start the next division.

		jmp		short SUBDIVISION_LOOP

DONE_DIVIDE_PIXEL_CACHE:
		fld		fDUInvZScanline			// U
		fadd	fGUInvZ
		fld		fDVInvZScanline			// V U
		fadd	fGVInvZ
		fld		fDInvZScanline			// Z V U
		fadd	fGInvZ
		fxch	st(2)					// U V Z
		fstp	fGUInvZ
		fstp	fGVInvZ
		fst		fGInvZ
		fdivr	fOne							// Start the next division.

		mov		[iCacheNextSubdivide],eax

SUBDIVISION_LOOP:
		//---------------------------------------------------------------------------
		// Start the next subdivision.
		// ecx,edx,esi = texture values
		//
		mov		edi,[i_pixel]
		mov		eax,[iNextSubdivide]				// eax = i_pixel_inner

		sub		edi,eax								// edi = inner loop count
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.

		mov		ebx,[i_screen_index]				// Load scanline offset.

		add		ebx,eax								// Add scanline offset to i_pixel
		mov		[i_pixel],eax						// Save i_pixel.

		lea		ebp,[ebp + ebx*2]					// Base of span in ebp.
		mov		[pvBaseOfLine],ebp					// Save pointer to base of line.
		jmp		short INNER_LOOP

		ALIGN	16

		//
		// Draw Pixels
		//
INNER_LOOP:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac		// Step V fraction.
		movzx	eax,word ptr[edx*2]					// Read texture value (low).
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction step.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		add		esi,ebx								// Step U fraction.
		mov		ebx,[pvBaseOfLine]					// Load pointer to base of line.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step + u carry.
		test	eax,eax
		jz		short SKIP_TRANSPARENT

		mov		[ebx + edi*2],ax					// Store pixel.

SKIP_TRANSPARENT:
		inc		edi
		jnz		short INNER_LOOP

		cmp		[i_pixel],0
		je		END_OF_SCANLINE

		//----------------------------------------------------------------------------------
		// Implement perspective correction.
		// stack top contains the result of the divide
		// Preserve: ecx,edx,esi
		//
		mov		edi,[iNextSubdivide]
		mov		eax,[iCacheNextSubdivide]

		sub		edi,eax
		jz		short EXIT_BEGIN_NEXT_QUICK_END

		fld		[fGUInvZ]					// U/Z,Z
		fxch	st(1)						// Z,U/Z
		fmul	st(1),st(0)					// Z,U
		mov		ebx,[bClampUV]
		fmul	[fGVInvZ]					// V,U

#if (VER_CLAMP_UV_16BIT==TRUE)

		//----------------------------------------------------------------------------------
		// Clamp U and V
		fxch	st(1)
		fstp	[f_u]
		fstp	[f_v]

		mov		ebp,[f_u]
		mov		ebx,[fTexEdgeTolerance]
		mov		eax,[f_v]
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		ebp,ebx
		cmovl	(_ebp,_ebx)
		mov		ebx,[fTexWidth]
		cmp		ebp,ebx
		cmovg	(_ebp,_ebx)
		mov		ebx,[fTexHeight]
		cmp		eax,ebx
		cmovg	(_eax,_ebx)
		mov		[f_u],ebp
		mov		[f_v],eax

		fld		[f_u]
		fld		[f_v]

#endif // (VER_CLAMP_UV_16BIT==TRUE)

		//----------------------------------------------------------------------------------
		// Initialize walking values
		fld		st(1)								// U,V,U
		fsub	[fU]								// U-fU,V,U
		fld		st(1)								// V,U-fU,V,U
		fsub	[fV]								// V-fV,U-fU,V,U
		fxch	st(1)								// U-fU,V-fV,V,U
		fmul	float ptr[fInverseIntTable+edi*4]	// (U-fU)*C,V-fV,V,U
		fxch	st(2)								// V,V-fV,(U-fU)*C,U
		fstp	[fV]								// V-fV,(U-fU)*C,U
		fmul	float ptr[fInverseIntTable+edi*4]	// (V-fV)*C,(U-fU)*C,U
		fxch	st(1)								// (U-fU)*C,(V-fV)*C,U
		fadd	[dFastFixed16Conversion]			// f(U-fU)*C,(V-fV)*C,U
		fxch	st(1)								// (V-fV)*C,f(U-fU)*C,U
		// stall(1)
		fadd	[dFastFixed16Conversion]			// f(V-fV)*C,f(U-fU)*C,U
		fxch	st(2)								// U,f(U-fU)*C,f(V-fV)*C
		fstp	[fU]								// f(U-fU)*C,f(V-fV)*C
		fstp	[d_temp_a]							// f(V-fV)*C
		fstp	[d_temp_b]							

		mov		edi,dword ptr[d_temp_a]			// uslope
		mov		eax,dword ptr[d_temp_b]			// vslope

		sar		edi,16							// integer part of uslope
		mov		ebp,dword ptr[d_temp_a]			// uslope again

		shl		ebp,16							// fractional part of uslope
		mov		ebx,eax							// vslope again

		sar		eax,16							// integer part of vslope
		mov		[w2dDeltaTex.uUFrac],ebp		// store UFrac

		shl		ebx,16							// fractional part of vslope
		mov		ebp,[iTexWidth]					// Load texture width.

		imul	eax,ebp							// ivslope*twidth

		add		edi,eax							// ivslope*twidth + iuslope
		mov		[w2dDeltaTex.uVFrac],ebx		// store VFrac

		mov		[w2dDeltaTex.iUVInt+4],edi		// store integer stride
		add		edi,ebp							// add twidth to integer stride

		mov		[w2dDeltaTex.iUVInt],edi		// store integer stride + twidth
		mov		eax,[iCacheNextSubdivide]

		//----------------------------------------------------------------------------------
		// Begin Next Subdivision
		mov		[iNextSubdivide],eax			// eax == iNextSubdivide

		test	eax,eax							// Next subdivision is zero length?
		jz		SUBDIVISION_LOOP

		// scan line is +ve
		add		eax,[iSubdivideLen]
		jle		short DONE_DIVIDE_PIXEL_CACHE_END

		// calc the new +ve ratio
		fild	iNextSubdivide
		fld		fInvSubdivideLen
		fchs
		fmulp	st(1),st(0)			// st(0) = (-)fInvSubdivideLen * i_pixel;
		fld		fDUInvZScanline		// U C

		xor		eax,eax

		fmul	st(0),st(1)			// U*C C
		fxch	st(1)				// C U*C
		fld		fDVInvZScanline		// V C U*C
		fxch	st(1)				// C V U*C
		fmul	st(1),st(0)			// C V*C U*C

		mov		[iCacheNextSubdivide],eax

		fmul	fDInvZScanline		// Z*C V*C U*C
		fxch	st(2)				// U*C V*C Z*C
		fst		fDUInvZScanline		// U V Z
		fadd	fGUInvZ
		fxch	st(1)				// V U Z
		fst		fDVInvZScanline
		fadd	fGVInvZ
		fxch	st(2)				// Z U V
		fst		fDInvZScanline
		fadd	fGInvZ
		fxch	st(2)				// V U Z
		fstp	fGVInvZ			
		fstp	fGUInvZ
		fst		fGInvZ
		fdivr	fOne				// Start the next division.

		jmp		SUBDIVISION_LOOP

DONE_DIVIDE_PIXEL_CACHE_END:
		fld		fDUInvZScanline			// U
		fadd	fGUInvZ
		fld		fDVInvZScanline			// V U
		fadd	fGVInvZ
		fld		fDInvZScanline			// Z V U
		fadd	fGInvZ
		fxch	st(2)					// U V Z
		fstp	fGUInvZ
		fstp	fGVInvZ
		fst		fGInvZ
		fdivr	fOne					// Start the next division.

		mov		[iCacheNextSubdivide],eax
		jmp		SUBDIVISION_LOOP

		// When the sub divide equals the cached sub-divide we end up here but
		// there is an element left on the fp stack.
EXIT_BEGIN_NEXT_QUICK_END:
		
		// Dump value on stack
		ffree	st(0)
		fincstp
		jmp		SUBDIVISION_LOOP

END_OF_SCANLINE:
		pop		ebp						// Restore ebp.

		//
		// Increment the base edge.
		//
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		// Step length, line starting address.
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ebx,[esi]CDrawPolygonBase.iLineStartIndex

		add		eax,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ebx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TCopyPerspTrans.fxX.i4Fx
		mov		ebx,[ecx]TCopyPerspTrans.fxXDifference.i4Fx

		fld		[ecx]TCopyPerspTrans.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TCopyPerspTrans.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TCopyPerspTrans.indCoord.fInvZ
		fadd	[f_inc_invz]

		add		eax,[fx_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Underflow.
		add		ebx,0x00010000
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jle		short NO_OVERFLOW

		// Overflow.
		sub		ebx,0x00010000
		jmp		short BORROW_OR_CARRY

NO_OVERFLOW:
		fstp	[ecx]TCopyPerspTrans.indCoord.fInvZ
		fstp	[ecx]TCopyPerspTrans.indCoord.fVInvZ
		fstp	[ecx]TCopyPerspTrans.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		fstp	[ecx]TCopyPerspTrans.indCoord.fInvZ
		fstp	[ecx]TCopyPerspTrans.indCoord.fUInvZ
		fstp	[ecx]TCopyPerspTrans.indCoord.fVInvZ

FINISH_LOOPING:
		// Y Loop control.
		mov		[ecx]TCopyPerspTrans.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TCopyPerspTrans.fxXDifference.i4Fx,ebx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Perspective, no Clut, non-transparent.
//
void DrawSubtriangle(TCopyPersp* pscan, CDrawPolygon<TCopyPersp>* pdtri)
{
	fixed fx_inc;
	fixed fx_diff;
	float f_inc_uinvz;
	float f_inc_vinvz;
	float f_inc_invz;

	TCopyPersp* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Local copies of edge stepping values.
		//
		// fixed fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
		// fixed fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;
		// float f_inc_uinvz = pdtri->pedgeBase->lineIncrement.indCoord.fUInvZ;
		// float f_inc_vinvz = pdtri->pedgeBase->lineIncrement.indCoord.fVInvZ;
		// float f_inc_invz  = pdtri->pedgeBase->lineIncrement.indCoord.fInvZ;
		//
		mov		eax,[plinc]

		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		mov		ebx,[eax]TCopyPersp.fxX
		mov		edx,[eax]TCopyPersp.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TCopyPersp.indCoord.fUInvZ
		mov		edx,[eax]TCopyPersp.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		ebx,[eax]TCopyPersp.indCoord.fInvZ

		mov		f_inc_vinvz,edx
		mov		f_inc_invz,ebx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		push	ebp									// Save ebp.

		// Check to see if we sould skip this scanline.
		mov		eax,[esi]CDrawPolygonBase.iY
		mov		ebx,[bEvenScanlinesOnly]

		and		eax,ebx
		jnz		END_OF_SCANLINE

		mov		ebx,[ecx]TCopyPersp.fxX.i4Fx
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		add		ebx,eax
		mov		eax,[ecx]TCopyPersp.fxX.i4Fx

		sar		ebx,16
		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		eax,16
		add		edx,ebx

		sub		eax,ebx									// i_pixel
		jge		END_OF_SCANLINE

		mov		[i_screen_index],edx
		mov		[i_pixel],eax

		//---------------------------------------------------------------------------
		// Caclulate values for the first correction span, and start the divide for
		// the next span.
		//

		// Copy global texture values.
		mov		ebx,[ecx]TCopyPersp.indCoord.fUInvZ
		mov		esi,[ecx]TCopyPersp.indCoord.fVInvZ

		mov		edi,[ecx]TCopyPersp.indCoord.fInvZ
		mov		[fGUInvZ],ebx

		mov		[fGVInvZ],esi
		mov		[fGInvZ],edi

		mov		ebx,[fDUInvZEdge]
		mov		esi,[fDVInvZEdge]

		mov		edi,[fDInvZEdge]
		mov		[fDUInvZScanline],ebx

		mov		[fDVInvZScanline],esi
		mov		[fDInvZScanline],edi

		mov		ebx,[iSubdivideLen]

		// scan line is +ve
		add		eax,ebx
		jg		short PARTIAL_SUBDIVIDE_POS

		// Check alignment.
		add		edx,eax						// i_screen_index + i_pixel
		add		edx,edx						// (i_screen_index + i_pixel) * 2

		and		edx,3
		jz		short DONE_DIVIDE_PIXEL

		// We are using subdivide length - 1 for alignment.
		mov		ebx,[fDUInvZEdgeMinusOne]
		mov		esi,[fDVInvZEdgeMinusOne]

		mov		edi,[fDInvZEdgeMinusOne]
		mov		[fDUInvZScanline],ebx

		mov		[fDVInvZScanline],esi
		mov		[fDInvZScanline],edi

		dec		eax
		jmp		DONE_DIVIDE_PIXEL

PARTIAL_SUBDIVIDE_POS:
		// calc the new +ve ratio
		fild	[i_pixel]
		fld		fInvSubdivideLen
		fchs
		xor		eax,eax
		fmulp	st(1),st(0)			// st(0) = (-)fInvSubdivideLen * i_pixel;
		fld		fDUInvZScanline		// U C
		// stall(1)
		fmul	st(0),st(1)			// U*C C
		fxch	st(1)				// C U*C
		fld		fDVInvZScanline		// V C U*C
		fxch	st(1)				// C V U*C
		fmul	st(1),st(0)			// C V*C U*C
		// stall(1)
		fmul	fDInvZScanline		// Z*C V*C U*C
		fxch	st(2)				// U*C V*C Z*C
		fstp	fDUInvZScanline		// V*C Z*C
		fstp	fDVInvZScanline		// Z*C
		fstp	fDInvZScanline

DONE_DIVIDE_PIXEL:
		// Get current u, v and z values.
		mov		[iNextSubdivide],eax
		mov		ebx,dword ptr[fGInvZ]		// f_z = fInverse(fGInvZ);

		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		fld		[fGUInvZ]					// U/Z

		add		eax,dword ptr[i4InverseMantissa + ebx*4]
		mov		ebx,[bClampUV]

		mov		dword ptr[f_z],eax

#if (VER_CLAMP_UV_16BIT==TRUE)

		//---------------------------------------------------------------------------
		// Set current texture coordinates (clamped).
		fmul	[f_z]
		fld		[fGVInvZ]
		fmul	[f_z]
		fxch	st(1)
		// stall(1)
		fstp	[f_u]

		// Clamp f_u			
		mov		eax,[f_u]
		mov		ebx,fTexEdgeTolerance
		mov		ecx,fTexWidth
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ecx
		cmovg	(_eax,_ecx)
		mov		ecx,fTexHeight
		mov		[f_u],eax
		fld		[f_u]

		fadd	[dFastFixed16Conversion]
		fxch	st(1)

		// Clamp f_v
		fstp	[f_v]
		mov		eax,[f_v]
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ecx
		cmovg	(_eax,_ecx)
		mov		[f_v],eax
		fld		[f_v]

		fadd	[dFastFixed16Conversion]
		fxch	st(1)
		fstp	[d_temp_a]
		fld		[fGUInvZ]
		fxch	st(1)
		fstp	[d_temp_b]

		// Increment u, v and z values.
		fadd	[fDUInvZScanline]
		fld		[fGVInvZ]
		fadd	[fDVInvZScanline]
		fxch	st(1)
		fld		[fGInvZ]
		fadd	[fDInvZScanline]

		// Setup esi=uFrac, ecx=vFrac, edx=UVInt for Abrash texture loop.
		mov		edx,dword ptr[d_temp_a]			// U (16.16)
		mov		eax,dword ptr[d_temp_b]			// V (16.16)

		sar		eax,16							// Integral V
		mov		ecx,[iTexWidth]					// Texture width.

		imul	eax,ecx							// iv*twidth

		sar		edx,16							// Integral U
		mov		esi,dword ptr[d_temp_a]			// Copy of U

		shl		esi,16							// UFrac
		mov		ecx,dword ptr[d_temp_b]			// Copy of V

		shl		ecx,16							// VFrac
		add		edx,eax							// iu + iv*twidth

		mov		ebx,[pvTextureBitmap]			// Texture base pointer.
		shr		ebx,1							// Into pixel offset.
		add		edx,ebx							// Add texture pointer in.

		fstp	[fGInvZ]

		// Get next u, v and z values.
		mov		ebx,dword ptr[fGInvZ]			// f_next_z = fInverse(fGInvZ);
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		fst		[fGUInvZ]

		mov		ebx,dword ptr[i4InverseMantissa + ebx*4]
		mov		edi,[i_pixel]

		add		eax,ebx
		mov		ebx,[iNextSubdivide]

		mov		dword ptr[f_next_z],eax
		sub		edi,ebx

		// Set new texture coordinate increments.
		fmul	[f_next_z]
		fxch	st(1)
		fst		[fGVInvZ]
		fmul	[f_next_z]
		fxch	st(1)
		fstp	[fU]			// V

		// Clamp fU			
		mov		eax,[fU]
		mov		ebx,fTexEdgeTolerance
		mov		ebp,fTexWidth
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ebp
		cmovg	(_eax,_ebp)
		mov		ebp,fTexHeight
		mov		[fU],eax

		fld		[fU]							// U,V
		fsub	[f_u]							// U-fu,V
		fxch	st(1)							// V,U-fu
		fstp	[fV]							// U-fu

		// Clamp fV
		mov		eax,[fV]
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ebp
		cmovg	(_eax,_ebp)
		mov		[fV],eax

		fld		[fV]							// V U-fu

#else // (VER_CLAMP_UV_16BIT==TRUE)

		//---------------------------------------------------------------------------
		// Set current texture coordinates (not clamped).
		fmul	[f_z]						// U
		fld		[fGVInvZ]					// V/Z U
		fmul	[f_z]						// V U
		fxch	st(1)						// U V
		// stall(1)
		fst		[f_u]
		fadd	[dFastFixed16Conversion]	// iU V
		fxch	st(1)						// V iU
		fst		[f_v]						
		fadd	[dFastFixed16Conversion]	// iV iU
		fxch	st(1)						// iU iV
		fstp	[d_temp_a]					// iV
		fld		[fGUInvZ]					// U/Z iV
		fxch	st(1)						// iV U/Z
		fstp	[d_temp_b]					// U/Z

		// Increment u, v and z values.
		fadd	[fDUInvZScanline]			// U2/Z
		fld		[fGVInvZ]
		fadd	[fDVInvZScanline]			// V2/Z U2/Z
		fxch	st(1)						// U2/Z V2/Z
		fld		[fGInvZ]					// 1/Z U2/Z V2/Z
		fadd	[fDInvZScanline]			// 1/Z2 U2/Z V2/Z

		// Setup esi=uFrac, ecx=vFrac, edx=UVInt for Abrash texture loop.
		mov		edx,dword ptr[d_temp_a]			// U (16.16)
		mov		eax,dword ptr[d_temp_b]			// V (16.16)

		sar		eax,16							// Integral V
		mov		ecx,[iTexWidth]					// Texture width.

		imul	eax,ecx							// iv*twidth

		sar		edx,16							// Integral U
		mov		esi,dword ptr[d_temp_a]			// Copy of U

		shl		esi,16							// UFrac
		mov		ecx,dword ptr[d_temp_b]			// Copy of V

		shl		ecx,16							// VFrac
		add		edx,eax							// iu + iv*twidth

		mov		ebx,[pvTextureBitmap]			// Texture base pointer.
		shr		ebx,1							// Into pixel offset.
		add		edx,ebx							// Add texture pointer in.

		fstp	[fGInvZ]

		// Get next u, v and z values.
		mov		ebx,dword ptr[fGInvZ]			// f_next_z = fInverse(fGInvZ);
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		fst		[fGUInvZ]

		mov		ebx,dword ptr[i4InverseMantissa + ebx*4]
		mov		edi,[i_pixel]

		add		eax,ebx
		mov		ebx,[iNextSubdivide]

		mov		dword ptr[f_next_z],eax
		sub		edi,ebx

		// Set new texture coordinate increments.
		fmul	[f_next_z]						// U,V/Z
		fxch	st(1)							// V/Z,U
		fst		[fGVInvZ]
		fmul	[f_next_z]						// V,U
		fxch	st(1)							// U,V
		fst		[fU]							// U,V
		fsub	[f_u]							// U-fu,V
		fxch	st(1)							// V,U-fu
		fst		[fV]

#endif // (VER_CLAMP_UV_16BIT==TRUE)

		// ---------------------------------------------------------------------------------
		// Both clmaped and non-clamped primitives end up here..
		fsub	[f_v]								// V-fv,U-fu
		fxch	st(1)								// U,V
		fmul	float ptr[fInverseIntTable+edi*4]	// (C*U),V
		fxch	st(1)								// V,(C*U)
		// stall(1)
		fmul	float ptr[fInverseIntTable+edi*4]	// (C*V),(C*U)
		fxch	st(1)								// (C*U),(C*V)
		fadd	[dFastFixed16Conversion]
		fxch	st(1)
		// stall(1)
		fadd	[dFastFixed16Conversion]
		fxch	st(1)
		// stall(1)
		fstp	[d_temp_a]
		fstp	[d_temp_b]

		mov		edi,dword ptr[d_temp_a]			// uslope
		mov		eax,dword ptr[d_temp_b]			// vslope

		sar		edi,16							// integer part of uslope
		mov		ebp,dword ptr[d_temp_a]			// uslope again

		shl		ebp,16							// fractional part of uslope
		mov		ebx,eax							// vslope again

		sar		eax,16							// integer part of vslope
		mov		[w2dDeltaTex.uUFrac],ebp		// store UFrac

		shl		ebx,16							// fractional part of vslope
		mov		ebp,[iTexWidth]					// Load texture width.

		imul	eax,ebp							// ivslope*twidth

		add		edi,eax							// ivslope*twidth + iuslope
		mov		[w2dDeltaTex.uVFrac],ebx		// store VFrac

		mov		[w2dDeltaTex.iUVInt+4],edi		// store integer stride
		add		edi,ebp							// add twidth to integer stride

		mov		[w2dDeltaTex.iUVInt],edi		// store integer stride + twidth
		mov		eax,[iNextSubdivide]

		test	eax,eax							// Next subdivision is zero length?
		jz		SUBDIVISION_LOOP

		//iCacheNextSubdivide = iSetNextDividePixel(iNextSubdivide);
		mov		edi,[iSubdivideLen]
		
		// scan line is +ve
		add		eax,edi
		jle		short DONE_DIVIDE_PIXEL_CACHE

		// calc the new +ve ratio
		fild	iNextSubdivide
		fld		fInvSubdivideLen
		fchs
		fmulp	st(1),st(0)			// st(0) = (-)fInvSubdivideLen * i_pixel;
		fld		fDUInvZEdge			// U C

		xor		eax,eax

		fmul	st(0),st(1)			// U*C C
		fxch	st(1)				// C U*C
		fld		fDVInvZEdge			// V C U*C
		fxch	st(1)				// C V U*C
		fmul	st(1),st(0)			// C V*C U*C

		mov		[iCacheNextSubdivide],eax

		fmul	fDInvZEdge			// Z*C V*C U*C
		fxch	st(2)				// U*C V*C Z*C
		fst		fDUInvZScanline		// U*C V*C Z*C
		fadd	fGUInvZ				// U*C+L V*C Z*C
		fxch	st(1)				// V*C U*C+L Z*C
		fst		fDVInvZScanline		// V*C U*C+L Z*C
		fadd	fGVInvZ				// V*C+L U*C+L Z*C
		fxch	st(2)				// Z*C U*C+L V*C+L
		fst		fDInvZScanline		// Z*C U*C+L V*C+L
		fadd	fGInvZ				// Z*C+L U*C+L V*C+L
		fxch	st(2)
		fstp	fGVInvZ			
		fstp	fGUInvZ
		fst		fGInvZ
		fdivr	fOne							// Start the next division.

		jmp		short SUBDIVISION_LOOP

DONE_DIVIDE_PIXEL_CACHE:
		// Copy texture values.
		mov		ebp,[fDUInvZEdge]
		mov		ebx,[fDVInvZEdge]

		mov		edi,[fDInvZEdge]
		mov		[fDUInvZScanline],ebp

		mov		[fDVInvZScanline],ebx
		mov		[fDInvZScanline],edi

		// Step texture values.
		fld		fDUInvZScanline			// U
		fadd	fGUInvZ
		fld		fDVInvZScanline			// V U
		fadd	fGVInvZ
		fld		fDInvZScanline			// Z V U
		fadd	fGInvZ
		fxch	st(2)					// U V Z
		fstp	fGUInvZ
		fstp	fGVInvZ
		fst		fGInvZ
		fdivr	fOne							// Start the next division.

		mov		[iCacheNextSubdivide],eax

SUBDIVISION_LOOP:
		//---------------------------------------------------------------------------
		// Start the next subdivision.
		// ecx,edx,esi = texture values
		//
		mov		edi,[i_pixel]
		mov		eax,[iNextSubdivide]
		sub		edi,eax

		//
		// eax = i_pixel
		// edi = inner loop count
		// ecx,edx,esi = texture values
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.
		mov		ebx,[i_screen_index]				// Load scanline offset.

		add		ebx,eax								// Add scanline offset to i_pixel
		mov		[i_pixel],eax						// Save i_pixel.

		lea		ebp,[ebp+ebx*2]						// Base of span in ebp.
		xor		eax,eax								// Clear eax.

		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.
		jmp		short INNER_LOOP

		ALIGN	16

		//
		// Draw Pixels
		//
INNER_LOOP:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac		// Step V fraction.
		movzx	eax,word ptr[edx*2]					// Read texture value (low).
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction step.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		add		esi,ebx								// Step U fraction.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step + u carry.
		mov		ebx,[pvBaseOfLine]								// Load pointer to base of line.

		mov		[ebx + edi*2],ax					// Store pixel.
		inc		edi
		jnz		short INNER_LOOP

		cmp		[i_pixel],0
		je		END_OF_SCANLINE

		//----------------------------------------------------------------------------------
		// Implement perspective correction.
		// stack top contains the result of the divide
		// Preserve: ecx,edx,esi
		//
		mov		edi,[iNextSubdivide]
		mov		eax,[iCacheNextSubdivide]

		sub		edi,eax
		jz		short EXIT_BEGIN_NEXT_QUICK_END				// This should never happen.

		fld		[fGUInvZ]					// U/Z,Z
		fxch	st(1)						// Z,U/Z
		fmul	st(1),st(0)					// Z,U
		mov		ebx,[bClampUV]
		fmul	[fGVInvZ]					// V,U

#if (VER_CLAMP_UV_16BIT==TRUE)

		//----------------------------------------------------------------------------------
		// Clamp U and V
		fxch	st(1)
		fstp	[f_u]
		fstp	[f_v]

		mov		ebp,[f_u]
		mov		ebx,[fTexEdgeTolerance]
		mov		eax,[f_v]
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		ebp,ebx
		cmovl	(_ebp,_ebx)
		mov		ebx,[fTexWidth]
		cmp		ebp,ebx
		cmovg	(_ebp,_ebx)
		mov		ebx,[fTexHeight]
		cmp		eax,ebx
		cmovg	(_eax,_ebx)
		mov		[f_u],ebp
		mov		[f_v],eax

		fld		[f_u]
		fld		[f_v]

#endif // (VER_CLAMP_UV_16BIT==TRUE)

		//----------------------------------------------------------------------------------
		// Initialize walking values
		fld		st(1)								// U,V,U
		fsub	[fU]								// U-fU,V,U
		fld		st(1)								// V,U-fU,V,U
		fsub	[fV]								// V-fV,U-fU,V,U
		fxch	st(1)								// U-fU,V-fV,V,U
		fmul	float ptr[fInverseIntTable+edi*4]	// (U-fU)*C,V-fV,V,U
		fxch	st(2)								// V,V-fV,(U-fU)*C,U
		fstp	[fV]								// V-fV,(U-fU)*C,U
		fmul	float ptr[fInverseIntTable+edi*4]	// (V-fV)*C,(U-fU)*C,U
		fxch	st(1)								// (U-fU)*C,(V-fV)*C,U
		fadd	[dFastFixed16Conversion]			// f(U-fU)*C,(V-fV)*C,U
		fxch	st(1)								// (V-fV)*C,f(U-fU)*C,U
		// stall(1)
		fadd	[dFastFixed16Conversion]			// f(V-fV)*C,f(U-fU)*C,U
		fxch	st(2)								// U,f(U-fU)*C,f(V-fV)*C
		fstp	[fU]								// f(U-fU)*C,f(V-fV)*C
		fstp	[d_temp_a]							// f(V-fV)*C
		fstp	[d_temp_b]							

		mov		edi,dword ptr[d_temp_a]			// uslope
		mov		eax,dword ptr[d_temp_b]			// vslope

		sar		edi,16							// integer part of uslope
		mov		ebp,dword ptr[d_temp_a]			// uslope again

		shl		ebp,16							// fractional part of uslope
		mov		ebx,eax							// vslope again

		sar		eax,16							// integer part of vslope
		mov		[w2dDeltaTex.uUFrac],ebp		// store UFrac

		shl		ebx,16							// fractional part of vslope
		mov		ebp,[iTexWidth]					// Load texture width.

		imul	eax,ebp							// ivslope*twidth

		add		edi,eax							// ivslope*twidth + iuslope
		mov		[w2dDeltaTex.uVFrac],ebx		// store VFrac

		mov		[w2dDeltaTex.iUVInt+4],edi		// store integer stride
		add		edi,ebp							// add twidth to integer stride

		mov		[w2dDeltaTex.iUVInt],edi		// store integer stride + twidth
		mov		eax,[iCacheNextSubdivide]		// Load cached next subdivision.

		mov		[iNextSubdivide],eax			// eax == iNextSubdivide

		test	eax,eax							// Next subdivision is zero length?
		jz		SUBDIVISION_LOOP

		//----------------------------------------------------------------------------------
		// Begin Next Subdivision

		// scan line is +ve
		add		eax,[iSubdivideLen]
		jle		short DONE_DIVIDE_PIXEL_CACHE_END

		// calc the new +ve ratio
		fild	iNextSubdivide
		fld		fInvSubdivideLen
		fchs
		fmulp	st(1),st(0)			// st(0) = (-)fInvSubdivideLen * i_pixel;
		fld		fDUInvZScanline		// U C

		xor		eax,eax

		fmul	st(0),st(1)			// U*C C
		fxch	st(1)				// C U*C
		fld		fDVInvZScanline		// V C U*C
		fxch	st(1)				// C V U*C
		fmul	st(1),st(0)			// C V*C U*C

		mov		[iCacheNextSubdivide],eax

		fmul	fDInvZScanline			// Z*C V*C U*C
		fxch	st(2)					// U*C V*C Z*C
		fst		fDUInvZScanline			// U V Z
		fadd	fGUInvZ
		fxch	st(1)					// V U Z
		fst		fDVInvZScanline
		fadd	fGVInvZ
		fxch	st(2)					// Z U V
		fst		fDInvZScanline
		fadd	fGInvZ
		fxch	st(2)					// V U Z
		fstp	fGVInvZ			
		fstp	fGUInvZ
		fst		fGInvZ
		fdivr	fOne					// Start the next division.

		jmp		SUBDIVISION_LOOP

DONE_DIVIDE_PIXEL_CACHE_END:
		fld		fDUInvZScanline			// U
		fadd	fGUInvZ
		fld		fDVInvZScanline			// V U
		fadd	fGVInvZ
		fld		fDInvZScanline			// Z V U
		fadd	fGInvZ
		fxch	st(2)					// U V Z
		fstp	fGUInvZ
		fstp	fGVInvZ
		fst		fGInvZ
		fdivr	fOne					// Start the next division.

		mov		[iCacheNextSubdivide],eax
		jmp		SUBDIVISION_LOOP

EXIT_BEGIN_NEXT_QUICK_END:
		// When the sub divide equals the cached sub-divide we end up here but
		// there is an element left on the fp stack.
		// This is never called unless there is something wrong with the loop
		// counter.
		
		// Dump value on stack
		fcomp	st(0)
		jmp		SUBDIVISION_LOOP

END_OF_SCANLINE:
		pop		ebp						// Restore ebp.

		//
		// Increment the base edge.
		//
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		// Step length, line starting address.
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ebx,[esi]CDrawPolygonBase.iLineStartIndex

		add		eax,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ebx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TCopyPersp.fxX.i4Fx
		mov		ebx,[ecx]TCopyPersp.fxXDifference.i4Fx

		fld		[ecx]TCopyPersp.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TCopyPersp.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TCopyPersp.indCoord.fInvZ
		fadd	[f_inc_invz]

		add		eax,[fx_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Underflow.
		add		ebx,0x00010000
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jle		short NO_OVERFLOW

		// Overflow.
		sub		ebx,0x00010000
		jmp		short BORROW_OR_CARRY

NO_OVERFLOW:
		fstp	[ecx]TCopyPersp.indCoord.fInvZ
		fstp	[ecx]TCopyPersp.indCoord.fVInvZ
		fstp	[ecx]TCopyPersp.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		fstp	[ecx]TCopyPersp.indCoord.fInvZ
		fstp	[ecx]TCopyPersp.indCoord.fUInvZ
		fstp	[ecx]TCopyPersp.indCoord.fVInvZ

FINISH_LOOPING:
		// Y Loop control.
		mov		[ecx]TCopyPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TCopyPersp.fxXDifference.i4Fx,ebx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Linear, no clut, non-transparent.
//
//**********************************
void DrawSubtriangle(TCopyLinear* pscan, CDrawPolygon<TCopyLinear>* pdtri)
{
	TCopyLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	_asm
	{
		push	ebp									// Save ebp.
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
		// pscanCopy		= pscan;
		// pdtriCopy		= pdtri;
		//
		mov		ebx,[edi]TCopyLinear.fxX
		mov		ecx,[edi]TCopyLinear.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TCopyLinear.indCoord.bfU.i4Int
		mov		ecx,[edi]TCopyLinear.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TCopyLinear.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TCopyLinear.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriCopy,esi
		mov		pscanCopy,eax

		// -------------------------------------------------------------------------
		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// -------------------------------------------------------------------------

LIN_NEXT_SCAN_LINE:
		mov		ebx,[bEvenScanlinesOnly]
		mov		ebp,[esi]CDrawPolygonBase.iY

		// check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short INC_BASE_EDGE

		mov		ebx,[eax]TCopyLinear.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		add		ebx,ecx

		sar		ebx,16
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		edi,16
		mov		ebp,gsGlobals.pvScreen

		add		ecx,ebx

		sub		edi,ebx
		jge		short INC_BASE_EDGE									// no pixels to draw

		mov		esi,[eax]TCopyLinear.indCoord.bfU.u4Frac			// UFrac
		mov		edx,[eax]TCopyLinear.indCoord.w1dV.bfxValue.i4Int		

		lea		ebp,[ebp + ecx*2]
		mov		ebx,[eax]TCopyLinear.indCoord.bfU.i4Int

		add		edx,ebx												// Add integer steps.

		mov		ebx,[pvTextureBitmap]			// Texture base pointer.
		shr		ebx,1							// Into pixel offset.
		add		edx,ebx							// Add texture pointer in.

		mov		ecx,[eax]TCopyLinear.indCoord.w1dV.bfxValue.u4Frac	// VFrac

		mov		[pvBaseOfLine],ebp
		jmp		short INNER_LOOP

		ALIGN	16

		//
		// Draw Pixels
		//
INNER_LOOP:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac		// Step V fraction.
		movzx	eax,word ptr[edx*2]					// Read texture value (low).
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction step.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		add		esi,ebx								// Step U fraction.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step + u carry.
		mov		ebx,[pvBaseOfLine]								// Load pointer to base of line.

		mov		[ebx + edi*2],ax					// Store pixel.
		inc		edi
		jnz		short INNER_LOOP

		// -------------------------------------------------------------------------
		// Increment the base edge.
INC_BASE_EDGE:
		mov		eax,[pscanCopy]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TCopyLinear.indCoord.bfU.u4Frac
		mov		edx,[eax]TCopyLinear.indCoord.bfU.i4Int

		mov		edi,[eax]TCopyLinear.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TCopyLinear.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TCopyLinear.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebx,[eax]TCopyLinear.fxXDifference
		mov		ebp,[fx_diff]

		add		ebx,ebp
		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000
		mov		ebp,bfNegDU.i4Int

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,ebp
		mov		ebp,w1dNegDV.bfxValue.u4Frac

		add		esi,ebp
		jnc		short EDGE_DONE

		add		edi,[eax]TCopyLinear.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000
		mov		ebp,bfNegDU.i4Int

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,ebp
		mov		ebp,w1dNegDV.bfxValue.u4Frac

		add		esi,ebp
		jnc		short EDGE_DONE

		add		edi,[eax]TCopyLinear.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TCopyLinear.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TCopyLinear.indCoord.w1dV.bfxValue.i4Int,edi

		mov		[eax]TCopyLinear.fxXDifference,ebx
		mov		esi,[pdtriCopy]								// Pointer to polygon object.

		mov		[eax]TCopyLinear.indCoord.bfU.u4Frac,ecx
		mov		[eax]TCopyLinear.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TCopyLinear.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TCopyLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		LIN_NEXT_SCAN_LINE

		pop ebp
	}
}


//*****************************************************************************************
//
// Linear, No clut, Transparent.
//
//******************************
void DrawSubtriangle(TCopyLinearTrans* pscan, CDrawPolygon<TCopyLinearTrans>* pdtri)
{
	TCopyLinearTrans* plinc = &pdtri->pedgeBase->lineIncrement;

	_asm
	{
		push	ebp									// Save ebp.
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
		// pscanCopy		= pscan;
		// pdtriCopy		= pdtri;
		//
		mov		ebx,[edi]TCopyLinearTrans.fxX
		mov		ecx,[edi]TCopyLinearTrans.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TCopyLinearTrans.indCoord.bfU.i4Int
		mov		ecx,[edi]TCopyLinearTrans.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TCopyLinearTrans.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TCopyLinearTrans.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriLin,esi
		mov		pscanLin,eax

LIN_NEXT_SCAN_LINE:
		mov		ebx,[bEvenScanlinesOnly]
		mov		edx,[esi]CDrawPolygonBase.iY

		// check for odd scan lines here and skip if even
		and		edx,ebx
		jnz		short INC_BASE_EDGE

		mov		ebx,[eax]TCopyLinear.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		add		ebx,ecx

		sar		ebx,16
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		edi,16
		mov		ebp,gsGlobals.pvScreen

		add		ecx,ebx

		sub		edi,ebx
		jge		short INC_BASE_EDGE										// no pixels to draw

		mov		esi,[eax]TCopyLinear.indCoord.bfU.u4Frac				// UFrac
		mov		edx,[eax]TCopyLinear.indCoord.w1dV.bfxValue.i4Int		

		lea		ebp,[ebp + ecx*2]
		mov		ebx,[eax]TCopyLinear.indCoord.bfU.i4Int

		add		edx,ebx													// Add integer steps.
		mov		ebx,[pvTextureBitmap]									// Texture base pointer.

		shr		ebx,1													// Into pixel offset.
		mov		ecx,[eax]TCopyLinear.indCoord.w1dV.bfxValue.u4Frac		// VFrac

		mov		[pvBaseOfLine],ebp
		add		edx,ebx													// Add to edx.
		jmp		short INNER_LOOP

		ALIGN	16

		//
		// Draw Pixels
		//
INNER_LOOP:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac		// Step V fraction.
		movzx	eax,word ptr[edx*2]					// Read texture value (low).
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction step.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		add		esi,ebx								// Step U fraction.
		mov		ebx,[pvBaseOfLine]					// Load pointer to base of line.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step + u carry.
		test	eax,eax
		jz		short SKIP_TRANSPARENT

		mov		[ebx + edi*2],ax					// Store pixel.

SKIP_TRANSPARENT:
		inc		edi
		jnz		short INNER_LOOP

		// -------------------------------------------------------------------------
		// Increment the base edge.
INC_BASE_EDGE:
		mov		eax,[pscanLin]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TCopyLinearTrans.indCoord.bfU.u4Frac
		mov		edx,[eax]TCopyLinearTrans.indCoord.bfU.i4Int

		mov		edi,[eax]TCopyLinearTrans.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TCopyLinearTrans.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TCopyLinearTrans.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebx,[eax]TCopyLinearTrans.fxXDifference
		mov		ebp,[fx_diff]

		add		ebx,ebp
		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000
		mov		ebp,bfNegDU.i4Int

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,ebp
		mov		ebp,w1dNegDV.bfxValue.u4Frac

		add		esi,ebp
		jnc		short EDGE_DONE

		add		edi,[eax]TCopyLinearTrans.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000
		mov		ebp,bfNegDU.i4Int

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,ebp
		mov		ebp,w1dNegDV.bfxValue.u4Frac

		add		esi,ebp
		jnc		short EDGE_DONE

		add		edi,[eax]TCopyLinearTrans.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TCopyLinearTrans.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TCopyLinearTrans.indCoord.w1dV.bfxValue.i4Int,edi

		mov		[eax]TCopyLinearTrans.fxXDifference,ebx
		mov		esi,[pdtriLin]								// Pointer to polygon object.

		mov		[eax]TCopyLinearTrans.indCoord.bfU.u4Frac,ecx
		mov		[eax]TCopyLinearTrans.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TCopyLinearTrans.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TCopyLinearTrans.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		LIN_NEXT_SCAN_LINE

		pop ebp
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
		//
		// Do self-modifications here.
		//
		mov		eax,[pvTextureBitmap]						// Texture pointer
		mov     ebx,[pu2TerrainTextureFogClut]				// Terrain clut pointer. Hack.

		mov		ecx,[pvLastTexture]
		mov		edx,[pvLastFogClut]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,edx
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],eax
		mov		[pvLastFogClut],ebx

		lea		ecx,[eax+1]
		lea		edx,[ebx+1]

		mov		[MODIFY_FOR_TEXTUREPOINTER_E-4],eax
		mov		[MODIFY_FOR_TEXTUREPOINTER_P1_E-4],ecx
		mov		[MODIFY_FOR_CLUT_E-4],ebx
		mov		[MODIFY_FOR_CLUT_P1_E-4],edx

		mov		[MODIFY_FOR_TEXTUREPOINTER_F-4],eax
		mov		[MODIFY_FOR_TEXTUREPOINTER_P1_F-4],ecx
		mov		[MODIFY_FOR_CLUT_F-4],ebx
		mov		[MODIFY_FOR_CLUT_P1_F-4],edx

		mov		[MODIFY_FOR_TEXTUREPOINTER_G-4],eax
		mov		[MODIFY_FOR_TEXTUREPOINTER_P1_G-4],ecx
		mov		[MODIFY_FOR_CLUT_G-4],ebx
		mov		[MODIFY_FOR_CLUT_P1_G-4],edx

		mov		[MODIFY_FOR_TEXTUREPOINTER_H-4],eax
		mov		[MODIFY_FOR_TEXTUREPOINTER_P1_H-4],ecx
		mov		[MODIFY_FOR_CLUT_H-4],ebx
		mov		[MODIFY_FOR_CLUT_P1_H-4],edx

DONE_WITH_MODIFY:
		//
		// Local copies of edge stepping values.
		//
		// fixed fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
		// fixed fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;
		// float f_inc_uinvz = pdtri->pedgeBase->lineIncrement.indCoord.fUInvZ;
		// float f_inc_vinvz = pdtri->pedgeBase->lineIncrement.indCoord.fVInvZ;
		// float f_inc_invz  = pdtri->pedgeBase->lineIncrement.indCoord.fInvZ;
		//
		mov		eax,[plinc]

		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		mov		ebx,[eax]TCopyTerrainPersp.fxX
		mov		edx,[eax]TCopyTerrainPersp.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TCopyTerrainPersp.indCoord.fUInvZ
		mov		edx,[eax]TCopyTerrainPersp.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		ebx,[eax]TCopyTerrainPersp.indCoord.fInvZ

		mov		f_inc_vinvz,edx
		mov		f_inc_invz,ebx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		push	ebp									// Save ebp.

		// Check to see if we sould skip this scanline.
		mov		eax,[esi]CDrawPolygonBase.iY
		mov		ebx,[bEvenScanlinesOnly]

		and		eax,ebx
		jnz		END_OF_SCANLINE

		mov		ebx,[ecx]TCopyTerrainPersp.fxX.i4Fx
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		add		ebx,eax
		mov		eax,[ecx]TCopyTerrainPersp.fxX.i4Fx

		sar		ebx,16
		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		eax,16
		add		edx,ebx

		sub		eax,ebx									// i_pixel
		jge		END_OF_SCANLINE

		mov		[i_screen_index],edx
		mov		[i_pixel],eax

		//---------------------------------------------------------------------------
		// Caclulate values for the first correction span, and start the divide for
		// the next span.
		//

		// Copy global texture values.
		mov		ebx,[ecx]TCopyTerrainPersp.indCoord.fUInvZ
		mov		esi,[ecx]TCopyTerrainPersp.indCoord.fVInvZ

		mov		edi,[ecx]TCopyTerrainPersp.indCoord.fInvZ
		mov		[fGUInvZ],ebx

		mov		[fGVInvZ],esi
		mov		[fGInvZ],edi

		mov		ebx,[fDUInvZEdge]
		mov		esi,[fDVInvZEdge]

		mov		edi,[fDInvZEdge]
		mov		[fDUInvZScanline],ebx

		mov		[fDVInvZScanline],esi
		mov		[fDInvZScanline],edi

		mov		ebx,[iSubdivideLen]

		// scan line is +ve
		add		eax,ebx
		jg		short PARTIAL_SUBDIVIDE_POS

		// Check alignment.
		add		edx,eax						// i_screen_index + i_pixel
		add		edx,edx						// (i_screen_index + i_pixel) * 2

		and		edx,3
		jz		short DONE_DIVIDE_PIXEL

		// We are using subdivide length - 1 for alignment.
		mov		ebx,[fDUInvZEdgeMinusOne]
		mov		esi,[fDVInvZEdgeMinusOne]

		mov		edi,[fDInvZEdgeMinusOne]
		mov		[fDUInvZScanline],ebx

		mov		[fDVInvZScanline],esi
		mov		[fDInvZScanline],edi

		dec		eax
		jmp		DONE_DIVIDE_PIXEL

PARTIAL_SUBDIVIDE_POS:
		// calc the new +ve ratio
		fild	[i_pixel]
		fld		fInvSubdivideLen
		fchs
		fmulp	st(1),st(0)			// st(0) = (-)fInvSubdivideLen * i_pixel;
		fld		fDUInvZScanline		// U C

		xor		eax,eax

		fmul	st(0),st(1)			// U*C C
		fxch	st(1)				// C U*C
		fld		fDVInvZScanline		// V C U*C
		fxch	st(1)				// C V U*C
		fmul	st(1),st(0)			// C V*C U*C
		// stall(1)
		fmul	fDInvZScanline		// Z*C V*C U*C
		fxch	st(2)				// U*C V*C Z*C
		fstp	fDUInvZScanline		// V*C Z*C
		fstp	fDVInvZScanline		// Z*C
		fstp	fDInvZScanline

DONE_DIVIDE_PIXEL:
		// Get current u, v and z values.
		mov		[iNextSubdivide],eax
		mov		ebx,dword ptr[fGInvZ]		// f_z = fInverse(fGInvZ);

		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		fld		[fGUInvZ]					// U/Z

		add		eax,dword ptr[i4InverseMantissa + ebx*4]
		mov		ebx,[bClampUV]

		mov		dword ptr[f_z],eax

#if (VER_CLAMP_UV_16BIT==TRUE)

		//---------------------------------------------------------------------------
		// Set current texture coordinates (clamped).
		fmul	[f_z]
		fld		[fGVInvZ]
		fmul	[f_z]
		fxch	st(1)
		// stall(1)
		fstp	[f_u]

		// Clamp f_u			
		mov		eax,[f_u]
		mov		ebx,fTexEdgeTolerance
		mov		ecx,fTexWidth
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ecx
		cmovg	(_eax,_ecx)
		mov		ecx,fTexHeight
		mov		[f_u],eax
		fld		[f_u]

		fadd	[dFastFixed16Conversion]
		fxch	st(1)

		// Clamp f_v
		fstp	[f_v]
		mov		eax,[f_v]
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ecx
		cmovg	(_eax,_ecx)
		mov		[f_v],eax
		fld		[f_v]

		fadd	[dFastFixed16Conversion]
		fxch	st(1)
		fstp	[d_temp_a]
		fld		[fGUInvZ]
		fxch	st(1)
		fstp	[d_temp_b]

		// Increment u, v and z values.
		fadd	[fDUInvZScanline]
		fld		[fGVInvZ]
		fadd	[fDVInvZScanline]
		fxch	st(1)
		fld		[fGInvZ]
		fadd	[fDInvZScanline]

		// Setup esi=uFrac, ecx=vFrac, edx=UVInt for Abrash texture loop.
		mov		edx,dword ptr[d_temp_a]			// U (16.16)
		mov		eax,dword ptr[d_temp_b]			// V (16.16)

		sar		eax,16							// Integral V
		mov		ecx,[iTexWidth]					// Texture width.

		imul	eax,ecx							// iv*twidth

		sar		edx,16							// Integral U
		mov		esi,dword ptr[d_temp_a]			// Copy of U

		shl		esi,16							// UFrac
		mov		ecx,dword ptr[d_temp_b]			// Copy of V

		shl		ecx,16							// VFrac
		add		edx,eax							// iu + iv*twidth

		fstp	[fGInvZ]

		// Get next u, v and z values.
		mov		ebx,dword ptr[fGInvZ]			// f_next_z = fInverse(fGInvZ);
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		fst		[fGUInvZ]

		mov		ebx,dword ptr[i4InverseMantissa + ebx*4]
		mov		edi,[i_pixel]

		add		eax,ebx
		mov		ebx,[iNextSubdivide]

		mov		dword ptr[f_next_z],eax
		sub		edi,ebx

		// Set new texture coordinate increments.
		fmul	[f_next_z]
		fxch	st(1)
		fst		[fGVInvZ]
		fmul	[f_next_z]
		fxch	st(1)
		fstp	[fU]			// V

		// Clamp fU			
		mov		eax,[fU]
		mov		ebx,fTexEdgeTolerance
		mov		ebp,fTexWidth
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ebp
		cmovg	(_eax,_ebp)
		mov		ebp,fTexHeight
		mov		[fU],eax

		fld		[fU]							// U,V
		fsub	[f_u]							// U-fu,V
		fxch	st(1)							// V,U-fu
		fstp	[fV]							// U-fu

		// Clamp fV
		mov		eax,[fV]
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		eax,ebp
		cmovg	(_eax,_ebp)
		mov		[fV],eax

		fld		[fV]							// V U-fu

#else // (VER_CLAMP_UV_16BIT==TRUE)

		//---------------------------------------------------------------------------
		// Set current texture coordinates (not clamped).
		fmul	[f_z]						// U
		fld		[fGVInvZ]					// V/Z U
		fmul	[f_z]						// V U
		fxch	st(1)						// U V
		// stall(1)
		fst		[f_u]
		fadd	[dFastFixed16Conversion]	// iU V
		fxch	st(1)						// V iU
		fst		[f_v]						
		fadd	[dFastFixed16Conversion]	// iV iU
		fxch	st(1)						// iU iV
		fstp	[d_temp_a]					// iV
		fld		[fGUInvZ]					// U/Z iV
		fxch	st(1)						// iV U/Z
		fstp	[d_temp_b]					// U/Z

		// Increment u, v and z values.
		fadd	[fDUInvZScanline]			// U2/Z
		fld		[fGVInvZ]
		fadd	[fDVInvZScanline]			// V2/Z U2/Z
		fxch	st(1)						// U2/Z V2/Z
		fld		[fGInvZ]					// 1/Z U2/Z V2/Z
		fadd	[fDInvZScanline]			// 1/Z2 U2/Z V2/Z

		// Setup esi=uFrac, ecx=vFrac, edx=UVInt for Abrash texture loop.
		mov		edx,dword ptr[d_temp_a]			// U (16.16)
		mov		eax,dword ptr[d_temp_b]			// V (16.16)

		sar		eax,16							// Integral V
		mov		ecx,[iTexWidth]					// Texture width.

		imul	eax,ecx							// iv*twidth

		sar		edx,16							// Integral U
		mov		esi,dword ptr[d_temp_a]			// Copy of U

		shl		esi,16							// UFrac
		mov		ecx,dword ptr[d_temp_b]			// Copy of V

		shl		ecx,16							// VFrac
		add		edx,eax							// iu + iv*twidth

		fstp	[fGInvZ]

		// Get next u, v and z values.
		mov		ebx,dword ptr[fGInvZ]			// f_next_z = fInverse(fGInvZ);
		mov		eax,iFI_SIGN_EXPONENT_SUB

		sub		eax,ebx
		and		ebx,iFI_MASK_MANTISSA

		sar		ebx,iSHIFT_MANTISSA
		and		eax,iFI_MASK_SIGN_EXPONENT

		fst		[fGUInvZ]

		mov		ebx,dword ptr[i4InverseMantissa + ebx*4]
		mov		edi,[i_pixel]

		add		eax,ebx
		mov		ebx,[iNextSubdivide]

		mov		dword ptr[f_next_z],eax
		sub		edi,ebx

		// Set new texture coordinate increments.
		fmul	[f_next_z]						// U,V/Z
		fxch	st(1)							// V/Z,U
		fst		[fGVInvZ]
		fmul	[f_next_z]						// V,U
		fxch	st(1)							// U,V
		fst		[fU]							// U,V
		fsub	[f_u]							// U-fu,V
		fxch	st(1)							// V,U-fu
		fst		[fV]

#endif // (VER_CLAMP_UV_16BIT==TRUE)

		// ---------------------------------------------------------------------------------
		// Both clmaped and non-clamped primitives end up here..
		fsub	[f_v]								// V-fv,U-fu
		fxch	st(1)								// U,V
		fmul	float ptr[fInverseIntTable+edi*4]	// (C*U),V
		fxch	st(1)								// V,(C*U)
		// stall(1)
		fmul	float ptr[fInverseIntTable+edi*4]	// (C*V),(C*U)
		fxch	st(1)								// (C*U),(C*V)
		fadd	[dFastFixed16Conversion]
		fxch	st(1)
		// stall(1)
		fadd	[dFastFixed16Conversion]
		fxch	st(1)
		// stall(1)
		fstp	[d_temp_a]
		fstp	[d_temp_b]

		mov		edi,dword ptr[d_temp_a]			// uslope
		mov		eax,dword ptr[d_temp_b]			// vslope

		sar		edi,16							// integer part of uslope
		mov		ebp,dword ptr[d_temp_a]			// uslope again

		shl		ebp,16							// fractional part of uslope
		mov		ebx,eax							// vslope again

		sar		eax,16							// integer part of vslope
		mov		[w2dDeltaTex.uUFrac],ebp		// store UFrac

		shl		ebx,16							// fractional part of vslope
		mov		ebp,[iTexWidth]					// Load texture width.

		imul	eax,ebp							// ivslope*twidth

		add		edi,eax							// ivslope*twidth + iuslope
		mov		[w2dDeltaTex.uVFrac],ebx		// store VFrac

		mov		[w2dDeltaTex.iUVInt+4],edi		// store integer stride
		add		edi,ebp							// add twidth to integer stride

		mov		[w2dDeltaTex.iUVInt],edi		// store integer stride + twidth
		mov		eax,[iNextSubdivide]

		test	eax,eax							// Next subdivision is zero length?
		jz		SUBDIVISION_LOOP

		//iCacheNextSubdivide = iSetNextDividePixel(iNextSubdivide);
		mov		edi,[iSubdivideLen]
		
		// scan line is +ve
		add		eax,edi
		jle		short DONE_DIVIDE_PIXEL_CACHE

		// calc the new +ve ratio
		fild	iNextSubdivide
		fld		fInvSubdivideLen
		fchs
		fmulp	st(1),st(0)			// st(0) = (-)fInvSubdivideLen * i_pixel;
		fld		fDUInvZEdge			// U C

		xor		eax,eax

		fmul	st(0),st(1)			// U*C C
		fxch	st(1)				// C U*C
		fld		fDVInvZEdge			// V C U*C
		fxch	st(1)				// C V U*C
		fmul	st(1),st(0)			// C V*C U*C

		mov		[iCacheNextSubdivide],eax

		fmul	fDInvZEdge			// Z*C V*C U*C
		fxch	st(2)				// U*C V*C Z*C
		fst		fDUInvZScanline		// U*C V*C Z*C
		fadd	fGUInvZ				// U*C+L V*C Z*C
		fxch	st(1)				// V*C U*C+L Z*C
		fst		fDVInvZScanline		// V*C U*C+L Z*C
		fadd	fGVInvZ				// V*C+L U*C+L Z*C
		fxch	st(2)				// Z*C U*C+L V*C+L
		fst		fDInvZScanline		// Z*C U*C+L V*C+L
		fadd	fGInvZ				// Z*C+L U*C+L V*C+L
		fxch	st(2)
		fstp	fGVInvZ			
		fstp	fGUInvZ
		fst		fGInvZ
		fdivr	fOne							// Start the next division.

		jmp		short SUBDIVISION_LOOP

DONE_DIVIDE_PIXEL_CACHE:
		// Copy texture values.
		mov		ebp,[fDUInvZEdge]
		mov		ebx,[fDVInvZEdge]

		mov		edi,[fDInvZEdge]
		mov		[fDUInvZScanline],ebp

		mov		[fDVInvZScanline],ebx
		mov		[fDInvZScanline],edi

		// Step texture values.
		fld		fDUInvZScanline			// U
		fadd	fGUInvZ
		fld		fDVInvZScanline			// V U
		fadd	fGVInvZ
		fld		fDInvZScanline			// Z V U
		fadd	fGInvZ
		fxch	st(2)					// U V Z
		fstp	fGUInvZ
		fstp	fGVInvZ
		fst		fGInvZ
		fdivr	fOne							// Start the next division.

		mov		[iCacheNextSubdivide],eax

SUBDIVISION_LOOP:
		//---------------------------------------------------------------------------
		// Start the next subdivision.
		// ecx,edx,esi = texture values
		//
		mov		edi,[i_pixel]
		mov		eax,[iNextSubdivide]
		sub		edi,eax

		//
		// eax = i_pixel
		// edi = inner loop count
		// ecx,edx,esi = texture values
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.
		mov		ebx,[i_screen_index]				// Load scanline offset.

		add		ebx,eax								// Add scanline offset to i_pixel
		mov		[i_pixel],eax						// Save i_pixel.

		lea		ebp,[ebp+ebx*2]						// Base of span in ebp.
		mov		eax,[u4TerrainFogMask]

		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.
		lea		ebp,[ebp + edi*2]					// Calculate destination address.

		// Check alignement.
		and		ebp,3
		jz		short ALIGNED

		//
		// Do one pixel for alignment.
		//
		and		al,[edx*2 + 0xDEADBEEF]				// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_E:
		mov		ebx,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction.

		and		ah,[edx*2 + 0xDEADBEEF]				// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_P1_E:
		add		ecx,ebx								// Step V fraction.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction.

		mov		cl,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (low).
MODIFY_FOR_CLUT_E:
		add		esi,ebx								// Step U fraction.

		mov		ch,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (high).
MODIFY_FOR_CLUT_P1_E:
		mov		ebx,[pvBaseOfLine]					// Load pointer to destination.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt	// Integer step + u carry.
		mov		eax,[u4TerrainFogMask]

		mov		[ebx + edi*2],cx					// Store pixel.

		inc		edi
		jz		short DONE_WITH_SPAN

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

		//
		// Do two pixels at a time.
		//
		mov		ebx,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction.
		mov		eax,[u4TerrainFogMask]

INNER_LOOP_2P:
		add		ecx,ebx								// Step V fraction.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction step.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		and		al,[edx*2 + 0xDEADBEEF]				// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_F:

		and		ah,[edx*2 + 0xDEADBEEF]				// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_P1_F:
		add		esi,ebx								// Step U fraction.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step + U carry.
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac					// Step V fraction.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		mov		bl,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (low).
MODIFY_FOR_CLUT_F:

		mov		bh,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (high).
MODIFY_FOR_CLUT_P1_F:
		mov		eax,[u4TerrainFogMask]

		shl		ebx,16								// Shift pixel up.
		and		al,[edx*2 + 0xDEADBEEF]				// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_G:

		and		ah,[edx*2 + 0xDEADBEEF]				// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_P1_G:
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac		// Step U fraction.

		mov		bl,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (low).
MODIFY_FOR_CLUT_G:
		mov		ebp,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step + V carry.

		adc		edx,ebp								// Integer step + U carry.
		mov		bh,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (high).
MODIFY_FOR_CLUT_P1_G:

		mov		ebp,[pvBaseOfLine]
		mov		eax,[u4TerrainFogMask]

		rol		ebx,16								// Reverse pixels.

		mov		[ebp + edi*2 - 4],ebx				// Store pixel.
		mov		ebx,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction.

		add		edi,2
		jle		short INNER_LOOP_2P

		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short DONE_WITH_SPAN

		//
		// Finish left over pixel.
		//
		// eax is zero.
		//
FINISH_REMAINDER:
		and		al,[edx*2 + 0xDEADBEEF]				// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_H:
		mov		ebx,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction.

		and		ah,[edx*2 + 0xDEADBEEF]				// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_P1_H:
		add		ecx,ebx								// Step V fraction.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction.

		mov		cl,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (low).
MODIFY_FOR_CLUT_H:
		add		esi,ebx								// Step U fraction.

		mov		ch,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (high).
MODIFY_FOR_CLUT_P1_H:
		mov		ebx,[pvBaseOfLine]					// Load pointer to destination.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt	// Integer step + u carry.

		mov		[ebx - 2],cx						// Store pixel.

DONE_WITH_SPAN:
		cmp		[i_pixel],0
		je		END_OF_SCANLINE

		//----------------------------------------------------------------------------------
		// Implement perspective correction.
		// stack top contains the result of the divide
		// Preserve: ecx,edx,esi
		//
		mov		edi,[iNextSubdivide]
		mov		eax,[iCacheNextSubdivide]

		sub		edi,eax
		jz		short EXIT_BEGIN_NEXT_QUICK_END				// This should never happen.

		fld		[fGUInvZ]					// U/Z,Z
		fxch	st(1)						// Z,U/Z
		fmul	st(1),st(0)					// Z,U
		mov		ebx,[bClampUV]
		fmul	[fGVInvZ]					// V,U

#if (VER_CLAMP_UV_16BIT==TRUE)

		//----------------------------------------------------------------------------------
		// Clamp U and V
		fxch	st(1)
		fstp	[f_u]
		fstp	[f_v]

		mov		ebp,[f_u]
		mov		ebx,[fTexEdgeTolerance]
		mov		eax,[f_v]
		cmp		eax,ebx
		cmovl	(_eax,_ebx)
		cmp		ebp,ebx
		cmovl	(_ebp,_ebx)
		mov		ebx,[fTexWidth]
		cmp		ebp,ebx
		cmovg	(_ebp,_ebx)
		mov		ebx,[fTexHeight]
		cmp		eax,ebx
		cmovg	(_eax,_ebx)
		mov		[f_u],ebp
		mov		[f_v],eax

		fld		[f_u]
		fld		[f_v]

#endif // (VER_CLAMP_UV_16BIT==TRUE)

		//----------------------------------------------------------------------------------
		// Initialize walking values
		fld		st(1)								// U,V,U
		fsub	[fU]								// U-fU,V,U
		fld		st(1)								// V,U-fU,V,U
		fsub	[fV]								// V-fV,U-fU,V,U
		fxch	st(1)								// U-fU,V-fV,V,U
		fmul	float ptr[fInverseIntTable+edi*4]	// (U-fU)*C,V-fV,V,U
		fxch	st(2)								// V,V-fV,(U-fU)*C,U
		fstp	[fV]								// V-fV,(U-fU)*C,U
		fmul	float ptr[fInverseIntTable+edi*4]	// (V-fV)*C,(U-fU)*C,U
		fxch	st(1)								// (U-fU)*C,(V-fV)*C,U
		fadd	[dFastFixed16Conversion]			// f(U-fU)*C,(V-fV)*C,U
		fxch	st(1)								// (V-fV)*C,f(U-fU)*C,U
		// stall(1)
		fadd	[dFastFixed16Conversion]			// f(V-fV)*C,f(U-fU)*C,U
		fxch	st(2)								// U,f(U-fU)*C,f(V-fV)*C
		fstp	[fU]								// f(U-fU)*C,f(V-fV)*C
		fstp	[d_temp_a]							// f(V-fV)*C
		fstp	[d_temp_b]							

		mov		edi,dword ptr[d_temp_a]			// uslope
		mov		eax,dword ptr[d_temp_b]			// vslope

		sar		edi,16							// integer part of uslope
		mov		ebp,dword ptr[d_temp_a]			// uslope again

		shl		ebp,16							// fractional part of uslope
		mov		ebx,eax							// vslope again

		sar		eax,16							// integer part of vslope
		mov		[w2dDeltaTex.uUFrac],ebp		// store UFrac

		shl		ebx,16							// fractional part of vslope
		mov		ebp,[iTexWidth]					// Load texture width.

		imul	eax,ebp							// ivslope*twidth

		add		edi,eax							// ivslope*twidth + iuslope
		mov		[w2dDeltaTex.uVFrac],ebx		// store VFrac

		mov		[w2dDeltaTex.iUVInt+4],edi		// store integer stride
		add		edi,ebp							// add twidth to integer stride

		mov		[w2dDeltaTex.iUVInt],edi		// store integer stride + twidth
		mov		eax,[iCacheNextSubdivide]		// Load cached next subdivision.

		mov		[iNextSubdivide],eax			// eax == iNextSubdivide

		test	eax,eax							// Next subdivision is zero length?
		jz		SUBDIVISION_LOOP

		//----------------------------------------------------------------------------------
		// Begin Next Subdivision

		// scan line is +ve
		add		eax,[iSubdivideLen]
		jle		short DONE_DIVIDE_PIXEL_CACHE_END

		// calc the new +ve ratio
		fild	iNextSubdivide
		fld		fInvSubdivideLen
		fchs
		fmulp	st(1),st(0)			// st(0) = (-)fInvSubdivideLen * i_pixel;
		fld		fDUInvZScanline		// U C

		xor		eax,eax

		fmul	st(0),st(1)			// U*C C
		fxch	st(1)				// C U*C
		fld		fDVInvZScanline		// V C U*C
		fxch	st(1)				// C V U*C
		fmul	st(1),st(0)			// C V*C U*C

		mov		[iCacheNextSubdivide],eax

		fmul	fDInvZScanline			// Z*C V*C U*C
		fxch	st(2)					// U*C V*C Z*C
		fst		fDUInvZScanline			// U V Z
		fadd	fGUInvZ
		fxch	st(1)					// V U Z
		fst		fDVInvZScanline
		fadd	fGVInvZ
		fxch	st(2)					// Z U V
		fst		fDInvZScanline
		fadd	fGInvZ
		fxch	st(2)					// V U Z
		fstp	fGVInvZ			
		fstp	fGUInvZ
		fst		fGInvZ
		fdivr	fOne					// Start the next division.

		jmp		SUBDIVISION_LOOP

DONE_DIVIDE_PIXEL_CACHE_END:
		fld		fDUInvZScanline			// U
		fadd	fGUInvZ
		fld		fDVInvZScanline			// V U
		fadd	fGVInvZ
		fld		fDInvZScanline			// Z V U
		fadd	fGInvZ
		fxch	st(2)					// U V Z
		fstp	fGUInvZ
		fstp	fGVInvZ
		fst		fGInvZ
		fdivr	fOne					// Start the next division.

		mov		[iCacheNextSubdivide],eax
		jmp		SUBDIVISION_LOOP

EXIT_BEGIN_NEXT_QUICK_END:
		// When the sub divide equals the cached sub-divide we end up here but
		// there is an element left on the fp stack.
		// This is never called unless there is something wrong with the loop
		// counter.
		
		// Dump value on stack
		ffree	st(0)
		fincstp
		jmp		SUBDIVISION_LOOP

END_OF_SCANLINE:
		pop		ebp						// Restore ebp.

		//
		// Increment the base edge.
		//
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		// Step length, line starting address.
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ebx,[esi]CDrawPolygonBase.iLineStartIndex

		add		eax,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ebx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TCopyTerrainPersp.fxX.i4Fx
		mov		ebx,[ecx]TCopyTerrainPersp.fxXDifference.i4Fx

		fld		[ecx]TCopyTerrainPersp.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TCopyTerrainPersp.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TCopyTerrainPersp.indCoord.fInvZ
		fadd	[f_inc_invz]

		add		eax,[fx_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Underflow.
		add		ebx,0x00010000
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jle		short NO_OVERFLOW

		// Overflow.
		sub		ebx,0x00010000
		jmp		short BORROW_OR_CARRY

NO_OVERFLOW:
		fstp	[ecx]TCopyTerrainPersp.indCoord.fInvZ
		fstp	[ecx]TCopyTerrainPersp.indCoord.fVInvZ
		fstp	[ecx]TCopyTerrainPersp.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		fstp	[ecx]TCopyTerrainPersp.indCoord.fInvZ
		fstp	[ecx]TCopyTerrainPersp.indCoord.fUInvZ
		fstp	[ecx]TCopyTerrainPersp.indCoord.fVInvZ

FINISH_LOOPING:
		// Y Loop control.
		mov		[ecx]TCopyTerrainPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TCopyTerrainPersp.fxXDifference.i4Fx,ebx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Linear, Terrain clut, non-transparent.
//
//**********************************
void DrawSubtriangle(TCopyTerrainLinear* pscan, CDrawPolygon<TCopyTerrainLinear>* pdtri)
{
	static void* pvLastTexture = 0;
	static void* pvLastFogClut = 0;
	static TCopyTerrainLinear* pscanGlbl;
	static CDrawPolygon<TCopyTerrainLinear>* pdtriGlbl;
	TCopyTerrainLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	Assert(pu2TerrainTextureFogClut);

	_asm
	{
		//
		// Do self modifications.
		//
		mov		ebx,[pvTextureBitmap]					// Texture pointer.
		mov     ecx,[pu2TerrainTextureFogClut]			// Terrain clut pointer. Hack.

		mov		edi,[pvLastTexture]
		mov		edx,[pvLastFogClut]

		cmp		ebx,edi
		jne		short DO_MODIFY

		cmp		ecx,edx
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ebx
		mov		[pvLastFogClut],ecx

		lea		edi,[ebx+1]
		lea		edx,[ecx+1]

		mov		[MODIFY_FOR_TEXTUREPOINTER_E-4],ebx
		mov		[MODIFY_FOR_TEXTUREPOINTER_P1_E-4],edi
		mov		[MODIFY_FOR_CLUT_E-4],ecx
		mov		[MODIFY_FOR_CLUT_P1_E-4],edx

		mov		[MODIFY_FOR_TEXTUREPOINTER_F-4],ebx
		mov		[MODIFY_FOR_TEXTUREPOINTER_P1_F-4],edi
		mov		[MODIFY_FOR_CLUT_F-4],ecx
		mov		[MODIFY_FOR_CLUT_P1_F-4],edx

		mov		[MODIFY_FOR_TEXTUREPOINTER_G-4],ebx
		mov		[MODIFY_FOR_TEXTUREPOINTER_P1_G-4],edi
		mov		[MODIFY_FOR_CLUT_G-4],ecx
		mov		[MODIFY_FOR_CLUT_P1_G-4],edx

		mov		[MODIFY_FOR_TEXTUREPOINTER_H-4],ebx
		mov		[MODIFY_FOR_TEXTUREPOINTER_P1_H-4],edi
		mov		[MODIFY_FOR_CLUT_H-4],ecx
		mov		[MODIFY_FOR_CLUT_P1_H-4],edx

DONE_WITH_MODIFY:
		push	ebp									// Save ebp.
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
		// pscanCopy		= pscan;
		// pdtriCopy		= pdtri;
		//
		mov		ebx,[edi]TCopyTerrainLinear.fxX
		mov		ecx,[edi]TCopyTerrainLinear.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TCopyTerrainLinear.indCoord.bfU.i4Int
		mov		ecx,[edi]TCopyTerrainLinear.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TCopyTerrainLinear.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TCopyTerrainLinear.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriGlbl,esi
		mov		pscanGlbl,eax

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
LIN_NEXT_SCAN_LINE:
		mov		ebx,[bEvenScanlinesOnly]
		mov		ebp,[esi]CDrawPolygonBase.iY

		// check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short INC_BASE_EDGE

		mov		ebx,[eax]TCopyTerrainLinear.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		add		ebx,ecx

		sar		ebx,16
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		edi,16
		mov		ebp,gsGlobals.pvScreen

		add		ecx,ebx

		sub		edi,ebx
		jge		short INC_BASE_EDGE										// no pixels to draw

		mov		esi,[eax]TCopyTerrainLinear.indCoord.bfU.u4Frac				// UFrac
		mov		edx,[eax]TCopyTerrainLinear.indCoord.w1dV.bfxValue.i4Int		

		lea		ebp,[ebp + ecx*2]
		mov		ebx,[eax]TCopyTerrainLinear.indCoord.bfU.i4Int

		add		edx,ebx													// Add integer steps.
		mov		ecx,[eax]TCopyTerrainLinear.indCoord.w1dV.bfxValue.u4Frac		// VFrac

		mov		[pvBaseOfLine],ebp
		lea		ebp,[ebp + edi*2]					// Calculate destination address.

		mov		eax,[u4TerrainFogMask]

		// Check alignement.
		and		ebp,3
		jz		short ALIGNED

		// Detect one pixel case before starting.
		inc		edi
		jz		short FINISH_REMAINDER

		//
		// Do one pixel for alignment.
		//
		mov		eax,[u4TerrainFogMask]

		and		al,[edx*2 + 0xDEADBEEF]				// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_E:
		mov		ebx,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction.

		and		ah,[edx*2 + 0xDEADBEEF]				// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_P1_E:
		add		ecx,ebx								// Step V fraction.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction.

		mov		cl,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (low).
MODIFY_FOR_CLUT_E:
		add		esi,ebx								// Step U fraction.

		mov		ch,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (high).
MODIFY_FOR_CLUT_P1_E:
		mov		ebx,[pvBaseOfLine]					// Load pointer to destination.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step + u carry.
		mov		eax,[u4TerrainFogMask]

		mov		[ebx + edi*2 - 2],cx				// Store pixel.

ALIGNED:
		// Make sure we have two pixels left.
		add		edi,2
		jg		short FINISH_REMAINDER

		//
		// Do two pixels at a time.
		//
		mov		ebx,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction.
		mov		eax,[u4TerrainFogMask]

INNER_LOOP_2P:
		add		ecx,ebx								// Step V fraction.
		mov		ebx,[w2dDeltaTex]CWalk2D.uUFrac		// Load U fraction step.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		and		al,[edx*2 + 0xDEADBEEF]				// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_F:

		and		ah,[edx*2 + 0xDEADBEEF]				// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_P1_F:
		add		esi,ebx								// Step U fraction.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step + U carry.
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac					// Step V fraction.

		sbb		ebp,ebp								// Get borrow from V fraction step.
		mov		bl,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (low).
MODIFY_FOR_CLUT_F:

		mov		bh,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (high).
MODIFY_FOR_CLUT_P1_F:
		mov		eax,[u4TerrainFogMask]

		shl		ebx,16								// Shift pixel up.
		and		al,[edx*2 + 0xDEADBEEF]				// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_G:

		and		ah,[edx*2 + 0xDEADBEEF]				// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_P1_G:
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac		// Step U fraction.

		mov		bl,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (low).
MODIFY_FOR_CLUT_G:
		mov		ebp,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step + V carry.

		adc		edx,ebp								// Integer step + U carry.
		mov		bh,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (high).
MODIFY_FOR_CLUT_P1_G:

		mov		ebp,[pvBaseOfLine]
		mov		eax,[u4TerrainFogMask]

		rol		ebx,16								// Reverse pixels.

		mov		[ebp + edi*2 - 4],ebx				// Store pixel.
		mov		ebx,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction.

		add		edi,2
		jle		short INNER_LOOP_2P

		// edi is 1 if there is a pixel left.
		cmp		edi,1
		jne		short INC_BASE_EDGE

		//
		// Finish left over pixel.
		//
FINISH_REMAINDER:
		mov		eax,[u4TerrainFogMask]

		and		al,[edx*2 + 0xDEADBEEF]				// Read texture value (low).
MODIFY_FOR_TEXTUREPOINTER_H:

		and		ah,[edx*2 + 0xDEADBEEF]				// Read texture value (high).
MODIFY_FOR_TEXTUREPOINTER_P1_H:

		mov		ebx,[pvBaseOfLine]					// Load pointer to destination.

		mov		cl,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (low).
MODIFY_FOR_CLUT_H:

		mov		ch,[eax*2 + 0xDEADBEEF]				// Do CLUT lookup (high).
MODIFY_FOR_CLUT_P1_H:

		mov		[ebx - 2],cx						// Store pixel.

		// -------------------------------------------------------------------------
		// Increment the base edge.
INC_BASE_EDGE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TCopyTerrainLinear.indCoord.bfU.u4Frac
		mov		edx,[eax]TCopyTerrainLinear.indCoord.bfU.i4Int

		mov		edi,[eax]TCopyTerrainLinear.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TCopyTerrainLinear.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TCopyTerrainLinear.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebx,[eax]TCopyTerrainLinear.fxXDifference
		mov		ebp,[fx_diff]

		add		ebx,ebp
		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000
		mov		ebp,bfNegDU.i4Int

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,ebp
		mov		ebp,w1dNegDV.bfxValue.u4Frac

		add		esi,ebp
		jnc		short EDGE_DONE

		add		edi,[eax]TCopyTerrainLinear.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000
		mov		ebp,bfNegDU.i4Int

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,ebp
		mov		ebp,w1dNegDV.bfxValue.u4Frac

		add		esi,ebp
		jnc		short EDGE_DONE

		add		edi,[eax]TCopyTerrainLinear.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TCopyTerrainLinear.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TCopyTerrainLinear.indCoord.w1dV.bfxValue.i4Int,edi

		mov		[eax]TCopyTerrainLinear.fxXDifference,ebx
		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TCopyTerrainLinear.indCoord.bfU.u4Frac,ecx
		mov		[eax]TCopyTerrainLinear.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TCopyTerrainLinear.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TCopyTerrainLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		LIN_NEXT_SCAN_LINE

		pop ebp
	}
}
