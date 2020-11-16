/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized version of the flat shaded alpha primitive
 *		Optimized for the Pentium Pro Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P6/DrawSubTriangleAlpha.cpp                   $
 * 
 * 5     98.07.17 6:37p Mmouni
 * Added  new alpha texture primitive.
 * 
 * 4     97/12/03 5:50p Mmouni
 * Fixed problem with DWORD alignment.
 * 
 * 3     9/01/97 8:04p Rwyatt
 * 
 * 2     8/17/97 12:19a Rwyatt
 * Terrain texturing, added to source safe yet again..
 * 
 * 1     8/17/97 12:18a Rwyatt
 * Alpha blend initial implementation
 * 
 ***********************************************************************************************/

#include "Lib/Renderer/LightBlend.hpp"


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
		mov		edx,pdtri
		mov		eax, [edx]CDrawPolygonBase.iY
		mov		edi,[edx]CDrawPolygonBase.iLineStartIndex
		mov		ecx,pscan
		//		edi, eax and edx are also set from the loop end


		//--------------------------------------------------------------------------------------
		// scan line start
NEXT_SCAN:
		and		eax,[bEvenScanlinesOnly]
		jnz		short SCAN_LINE

SCAN_LINE:
		cmp		[i4ScanlineDirection],0
		jge		short LEFT

		//--------------------------------------------------------------------------------------
		// scan line going to the right
		mov		eax,[ecx]TAlphaColour.fxX.i4Fx
		mov		esi,eax
		sub		eax,[edx]CDrawPolygonBase.fxLineLength.i4Fx
		sar		eax,16
		sar		esi,16
		add		edi,eax
		sub		esi,eax
		jle		short SKIP_LOOP

		//		flip the scan line around, renderer draws the other way
		add		edi,esi
		neg		esi

		jmp		short GO_RIGHT
LEFT:
		//--------------------------------------------------------------------------------------
		// scan line going to the left
		mov		esi,[ecx]TAlphaColour.fxX.i4Fx
		mov		eax,esi
		//mov		edi,[edx]CDrawPolygonBase.iLineStartIndex
		add		eax,[edx]CDrawPolygonBase.fxLineLength.i4Fx
		sar		esi,16
		sar		eax,16
		add		edi,eax
		sub		esi,eax
		jge		short SKIP_LOOP

GO_RIGHT:
		//--------------------------------------------------------------------------------------
		// all scan lines are drawn the same way
		mov		eax,gsGlobals.pvScreen				// Base of screen.
		lea		edi,[eax + edi*2]					// Base of span.

		lea		ecx,[edi + esi*2]					// Starting address.
		test	ecx,3
		jz		short N_LOOP

		//--------------------------------------------------------------------------------------
		// do a single WORD to dword align
		movzx	ebx,word ptr [edi + esi*2]
		and		ebx,[u4AlphaColourMask]
		or		ebx,[u4AlphaColourReference]
		and		ebx,0x0000ffff										// avoid partial-reg usage
		movzx	ebx,word ptr [lbAlphaConstant.au2Colour + ebx*2]
		mov		[edi + esi*2],bx

		inc		esi						// we are now aligned to a DWORD
		jz		short SKIP_LOOP			// are we done?

		//--------------------------------------------------------------------------------------
		// Main loop does 2 words at a time
N_LOOP:
		add		esi,2
		jge		short DONE_SCAN
		mov		ebx,[edi + esi*2 - 4]
		and		ebx,[u4AlphaColourMask]
		or		ebx,[u4AlphaColourReference]
		movzx	ecx, bx
		shr		ebx,16
		movzx	edx,word ptr [lbAlphaConstant.au2Colour + ebx*2]
		movzx	ebx,word ptr [lbAlphaConstant.au2Colour + ecx*2]
		shl		edx,16
		or		edx,ebx
		mov		[edi + esi*2 - 4], edx
		jmp		short N_LOOP

DONE_SCAN:
		jz		short DWORD_REMAIN

		//--------------------------------------------------------------------------------------
		// do a single word to finish
		movzx	ebx,word ptr [edi + esi*2 - 4]
		and		ebx, [u4AlphaColourMask]
		or		ebx, [u4AlphaColourReference]
		and		ebx, 0x0000ffff										// avoid partial-reg usage
		movzx	ebx,word ptr [lbAlphaConstant.au2Colour + ebx*2]
		mov		[edi + esi*2 - 4], bx
		jmp		short SKIP_LOOP


		//--------------------------------------------------------------------------------------
		// do a DWORD to finish
DWORD_REMAIN:
		mov		ebx,[edi + esi*2 - 4]
		and		ebx, [u4AlphaColourMask]
		or		ebx, [u4AlphaColourReference]
		movzx	ecx, bx
		shr		ebx,16
		movzx	edx,word ptr [lbAlphaConstant.au2Colour + ebx*2]
		movzx	ebx,word ptr [lbAlphaConstant.au2Colour + ecx*2]
		shl		edx,16
		or		edx,ebx
		mov		[edi + esi*2 - 4], edx

		//--------------------------------------------------------------------------------------
		// Loop control an sub pixel correction for edges
SKIP_LOOP:
		mov		edx,pdtri
		mov		ecx,pscan
		mov		eax, fx_inc
		mov		esi,[ecx]TAlphaColour.fxXDifference
		add		[ecx]TAlphaColour.fxX,eax
		add		esi, fx_diff

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
		mov		[ecx]TAlphaColour.fxXDifference, esi
		mov		eax, [edx]CDrawPolygonBase.fxLineLength.i4Fx
		mov		edi, [edx]CDrawPolygonBase.iLineStartIndex
		add		eax, [edx]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		edi, gsGlobals.u4LinePixels
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


//*****************************************************************************************
//
// Perspective, stipple alpha textured, 16 bit mode.
//
void DrawSubtriangle(TAlphaTexPersp* pscan, CDrawPolygon<TAlphaTexPersp>* pdtri)
{
	typedef TAlphaTexPersp::TIndex prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;

	TAlphaTexPersp* plinc = &pdtri->pedgeBase->lineIncrement;

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

		mov		ebx,[eax]TAlphaTexPersp.fxX
		mov		edx,[eax]TAlphaTexPersp.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TAlphaTexPersp.indCoord.fUInvZ
		mov		edx,[eax]TAlphaTexPersp.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		ebx,[eax]TAlphaTexPersp.indCoord.fInvZ

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

		// Setup for prespective correction and looping.
		//PERSPECTIVE_SCANLINE_PROLOGUE(TAlphaTexPersp)

		// Determine the start and end of the scanline. 
		mov		ebx,[ecx]TAlphaTexPersp.fxX.i4Fx
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		add		ebx,eax
		mov		eax,[ecx]TAlphaTexPersp.fxX.i4Fx

		sar		ebx,16
		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		eax,16
		add		edx,ebx

		sub		eax,ebx									// i_pixel 
		jge		END_OF_SCANLINE

		mov		[i_screen_index],edx
		mov		ebx,[ecx]TAlphaTexPersp.fxX.i4Fx		// Fractional X.

		sar		ebx,16									// Integer X.
		mov		edi,[esi]CDrawPolygonBase.iY			// Integer Y.

		xor		ebx,edi									// X xor Y
		mov		[i_pixel],eax

		// Skip over a pixel if ((y xor x) & 1).
		test	ebx,1
		jz		NO_SKIP

		inc		eax										// Increment i_pixel
		jz		END_OF_SCANLINE

		// Copy and increment global texture values (16 cycles).
		fld		fDUInvZEdge								// ustep
		fmul	fInvSubdivideLen
		fld		fDVInvZEdge								// vstep
		fmul	fInvSubdivideLen
		fld		fDInvZEdge								// zstep
		fmul	fInvSubdivideLen
		fxch	st(2)									// us vs zs
		fadd	[ecx]TAlphaTexPersp.indCoord.fUInvZ	// u vs zs
		fxch	st(1)
		fadd	[ecx]TAlphaTexPersp.indCoord.fVInvZ	// v u zs
		fxch	st(2)
		fadd	[ecx]TAlphaTexPersp.indCoord.fInvZ	// z u v
		fxch	st(1)									// u z v
		fstp	fGUInvZ
		fxch	st(1)
		fstp	fGVInvZ
		fstp	fGInvZ

		mov		[i_pixel],eax
		jmp		short DONE_WITH_SKIP

		//--------------------------------------------------------------------------- 
		// Caclulate values for the first correction span, and start the divide for 
		// the next span. 
		// 
NO_SKIP:
		// Copy global texture values. 
		mov		ebx,[ecx]TAlphaTexPersp.indCoord.fUInvZ
		mov		esi,[ecx]TAlphaTexPersp.indCoord.fVInvZ

		mov		edi,[ecx]TAlphaTexPersp.indCoord.fInvZ
		mov		fGUInvZ,ebx

		mov		fGVInvZ,esi
		mov		fGInvZ,edi

DONE_WITH_SKIP:
		// Copy edge values.
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

		// Subdivision is smaller than iSubdivideLen 
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

		/* Are we clamping for this polygon */
		test	ebx,ebx
		jz		short NO_CLAMP

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
		jge		short U_NOT_LESS1
		mov		eax,ebx
U_NOT_LESS1:
		cmp		eax,ecx
		jle		short U_NOT_GREATER1
		mov		eax,ecx
U_NOT_GREATER1:
		mov		ecx,fTexHeight
		mov		[f_u],eax
		fld		[f_u]

		fadd	[dFastFixed16Conversion]
		fxch	st(1)

		// Clamp f_v 
		fstp	[f_v]
		mov		eax,[f_v]
		cmp		eax,ebx
		jge		short V_NOT_LESS1
		mov		eax,ebx
V_NOT_LESS1:
		cmp		eax,ecx
		jle		short V_NOT_GREATER1
		mov		eax,ecx
V_NOT_GREATER1:
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

		shr		ebx,1							// Turn into word offset.
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
		jge		short U_NOT_LESS2
		mov		eax,ebx
U_NOT_LESS2:
		cmp		eax,ebp
		jle		short U_NOT_GREATER2
		mov		eax,ebp
U_NOT_GREATER2:
		mov		ebp,fTexHeight
		mov		[fU],eax

		fld		[fU]							// U,V 
		fsub	[f_u]							// U-fu,V 
		fxch	st(1)							// V,U-fu 
		fstp	[fV]							// U-fu 

		// Clamp fV 
		mov		eax,[fV]
		cmp		eax,ebx
		jge		short V_NOT_LESS2
		mov		eax,ebx
V_NOT_LESS2:
		cmp		eax,ebp
		jle		short V_NOT_GREATER2
		mov		eax,ebp
V_NOT_GREATER2:
		mov		[fV],eax

		fld		[fV]								// V U-fu 
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

		sar		edi,15							// integer part of uslope 
		mov		ebp,dword ptr[d_temp_a]			// uslope again 

		shl		ebp,17							// fractional part of uslope 
		mov		ebx,eax							// vslope again 

		sar		eax,15							// integer part of vslope 
		mov		[w2dDeltaTex.uUFrac],ebp		// store UFrac 

		imul	eax,[iTexWidth]					// ivslope*twidth 

		shl		ebx,17							// fractional part of vslope 

		mov		[w2dDeltaTex.uVFrac],ebx		// store VFrac 

		add		edi,eax							// ivslope*twidth + iuslope 
		mov		ebp,[iTexWidth]					// Load texture width. 

		mov		[w2dDeltaTex.iUVInt+4],edi		// store integer stride 
		add		edi,ebp							// add twidth to integer stride 

		mov		[w2dDeltaTex.iUVInt],edi		// store integer stride + twidth 
		mov		eax,[iNextSubdivide]

		jmp		CLAMP_DONE

NO_CLAMP:
		/*--------------------------------------------------------------------------- */
		/* Set current texture coordinates (not clamped). */
		fmul	[f_z]						/* U */
		fld		[fGVInvZ]					/* V/Z U */
		fmul	[f_z]						/* V U */
		fxch	st(1)						/* U V */
		/* stall(1) */
		fst		[f_u]
		fadd	[dFastFixed16Conversion]	/* iU V */
		fxch	st(1)						/* V iU */
		fst		[f_v]						
		fadd	[dFastFixed16Conversion]	/* iV iU */
		fxch	st(1)						/* iU iV */
		fstp	[d_temp_a]					/* iV */
		fld		[fGUInvZ]					/* U/Z iV */
		fxch	st(1)						/* iV U/Z */
		fstp	[d_temp_b]					/* U/Z */

		/* Increment u, v and z values. */
		fadd	[fDUInvZScanline]			/* U2/Z */
		fld		[fGVInvZ]
		fadd	[fDVInvZScanline]			/* V2/Z U2/Z */
		fxch	st(1)						/* U2/Z V2/Z */
		fld		[fGInvZ]					/* 1/Z U2/Z V2/Z */
		fadd	[fDInvZScanline]			/* 1/Z2 U2/Z V2/Z */

		/* Setup esi=uFrac, ecx=vFrac, edx=UVInt for Abrash texture loop. */
		mov		edx,dword ptr[d_temp_a]			/* U (16.16) */
		mov		eax,dword ptr[d_temp_b]			/* V (16.16) */

		sar		eax,16							/* Integral V */
		mov		ecx,[iTexWidth]					/* Texture width. */

		shl		eax,9							/* iv*twidth */

		sar		edx,16							/* Integral U */
		mov		ebp,[u4TextureTileMaskStepU]	/* Load mask for integral U */

		and		edx,ebp							/* Mask integral U before adding. */
		mov		esi,dword ptr[d_temp_a]			/* Copy of U */

		shl		esi,16							/* UFrac */
		mov		ecx,dword ptr[d_temp_b]			/* Copy of V */

		shl		ecx,16							/* VFrac */
		add		edx,eax							/* iu + iv*twidth */

		fstp	[fGInvZ]

		/* Get next u, v and z values. */
		mov		ebx,dword ptr[fGInvZ]			/* f_next_z = fInverse(fGInvZ); */
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

		/* Set new texture coordinate increments. */
		fmul	[f_next_z]						/* U,V/Z */
		fxch	st(1)							/* V/Z,U */
		fst		[fGVInvZ]
		fmul	[f_next_z]						/* V,U */
		fxch	st(1)							/* U,V */
		fst		[fU]							/* U,V */
		fsub	[f_u]							/* U-fu,V */
		fxch	st(1)							/* V,U-fu */
		fst		[fV]
		fsub	[f_v]								/* V-fv,U-fu */
		fxch	st(1)								/* U,V */
		fmul	float ptr[fInverseIntTable+edi*4]	/* (C*U),V */
		fxch	st(1)								/* V,(C*U) */
		/* stall(1) */
		fmul	float ptr[fInverseIntTable+edi*4]	/* (C*V),(C*U) */
		fxch	st(1)								/* (C*U),(C*V) */
		fadd	[dFastFixed16Conversion]
		fxch	st(1)
		/* stall(1) */
		fadd	[dFastFixed16Conversion]
		fxch	st(1)
		/* stall(1) */
		fstp	[d_temp_a]
		fstp	[d_temp_b]

		mov		edi,dword ptr[d_temp_a]			/* uslope */
		mov		eax,dword ptr[d_temp_b]			/* vslope */

		sar		edi,15							/* integer part of uslope */
		mov		ebp,dword ptr[d_temp_a]			/* uslope again */

		shl		ebp,17							/* fractional part of uslope */
		mov		ebx,eax							/* vslope again */

		sar		eax,15							/* integer part of vslope */
		mov		[w2dDeltaTex.uUFrac],ebp		/* store UFrac */

		shl		eax,9							/* ivslope*twidth */

		shl		ebx,17							/* fractional part of vslope */
		mov		ebp,[u4TextureTileMaskStepU]	/* Load mask for integral U step */

		and		edi,ebp							/* Mask integral U before adding. */
		mov		[w2dDeltaTex.uVFrac],ebx		/* store VFrac */

		add		edi,eax							/* ivslope*twidth + iuslope */
		mov		ebp,[iTexWidth]					/* Load texture width. */

		mov		[w2dDeltaTex.iUVInt+4],edi		/* store integer stride */
		add		edi,ebp							/* add twidth to integer stride */

		mov		[w2dDeltaTex.iUVInt],edi		/* store integer stride + twidth */
		mov		eax,[iNextSubdivide]

		/* --------------------------------------------------------------------------------- */
		/* Both clmaped and non-clamped primitives end up here..                             */
CLAMP_DONE:

		test	eax,eax							// Next subdivision is zero length? 
		jz		SUBDIVISION_LOOP

		// iCacheNextSubdivide = iSetNextDividePixel(iNextSubdivide); 
		mov		edi,[iSubdivideLen]

		// scan line is +ve 
		add		eax,edi
		jle		short DONE_DIVIDE_PIXEL_CACHE

		// calc the new +ve ratio 
		fild	iNextSubdivide
		fld		fInvSubdivideLen
		fchs

		// Subdivision is smaller than iSubdivideLen 
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

		jmp		SUBDIVISION_LOOP

DONE_DIVIDE_PIXEL_CACHE:
		// Copy texture values. 
		mov		ebp,[fDUInvZEdge]
		mov		ebx,[fDVInvZEdge]

		mov		edi,[fDInvZEdge]
		mov		[fDUInvZScanline],ebp

		mov		[fDVInvZScanline],ebx
		mov		[fDInvZScanline],edi

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

		and		edx,[u4TextureTileMask]				// Mask for tiling.
		xor		eax,eax								// Clear eax

		lea		ebx,[ebp+ebx*2]						// Base of span in ebx.
		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac		// Load V fraction step.

		mov		[pvBaseOfLine],ebx					// Save pointer to destination.
		mov		ebx,[pvTextureBitmap]				// Load pointer to texture.

		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw the scanline.
		//
		// eax = temp for pixel
		// ebx = base of texture map / base of scanline
		// ecx = Ufrac
		// edx = UVint
		// esi = Vfrac
		// edi = offset & count
		// ebp = vfrac step / v carry
		//
INNER_LOOP:
		add		ecx,ebp											// Step V fraction.
		movzx	eax,word ptr[ebx + edx*2]						// Read texture value.

		sbb		ebp,ebp											// Get borrow from V fraction step.

		add		esi,[w2dDeltaTex]CWalk2D.uUFrac					// Step U fraction.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step + u carry.
		mov		ebp,[au2AlphaTable]								// Conversion table.

		cmp		eax,0x00004000
		jle		PIXEL_TRANS

		and		eax,0x00000fff									// Mask colors.
		mov		ebx,[pvBaseOfLine]								// Load pointer to destination.

		movzx	eax,word ptr[ebp + eax*2]						// Color conversion.

		mov		[ebx + edi*2],ax								// Store pixel.
		mov		ebx,[pvTextureBitmap]							// Load pointer to texture.

PIXEL_TRANS:
		and		edx,[u4TextureTileMask]							// Mask for tiling.
		add		edi,2											// Step 2 pixels

		mov		ebp,[w2dDeltaTex]CWalk2D.uVFrac					// Load V fraction step.
		jl		INNER_LOOP

		// See if there are any pixels left.
		cmp		[i_pixel],0
		je		END_OF_SCANLINE

		// Do perspective correction and looping.
		//PERSPECTIVE_SCANLINE_EPILOGUE

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

		/* Are we clamping for this polygon? */
		test	ebx,ebx
		jz		short NO_CLAMP_END

		//---------------------------------------------------------------------------------- 
		// Clamp U and V 
		fxch	st(1)
		fstp	[f_u]
		fstp	[f_v]

		mov		ebp,[f_u]
		mov		ebx,[fTexEdgeTolerance]
		mov		eax,[f_v]
		cmp		eax,ebx
		jge		short V_NOT_LESS_END
		mov		eax,ebx
V_NOT_LESS_END:
		cmp		ebp,ebx
		jge		short U_NOT_LESS_END
		mov		ebp,ebx
U_NOT_LESS_END:
		mov		ebx,[fTexWidth]
		cmp		ebp,ebx
		jle		short U_NOT_GREATER_END
		mov		ebp,ebx
U_NOT_GREATER_END:
		mov		ebx,[fTexHeight]
		cmp		eax,ebx
		jle		short V_NOT_GREATER_END
		mov		eax,ebx
V_NOT_GREATER_END:
		mov		[f_u],ebp
		mov		[f_v],eax

		fld		[f_u]
		fld		[f_v]

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

		sar		edi,15							// integer part of uslope 
		mov		ebp,dword ptr[d_temp_a]			// uslope again 

		shl		ebp,17							// fractional part of uslope 
		mov		ebx,eax							// vslope again 

		sar		eax,15							// integer part of vslope 
		mov		[w2dDeltaTex.uUFrac],ebp		// store UFrac 

		imul	eax,[iTexWidth]					// ivslope*twidth 

		shl		ebx,17							// fractional part of vslope 

		mov		[w2dDeltaTex.uVFrac],ebx		// store VFrac 

		add		edi,eax							// ivslope*twidth + iuslope 
		mov		ebp,[iTexWidth]					// Load texture width. 

		mov		[w2dDeltaTex.iUVInt+4],edi		// store integer stride 
		add		edi,ebp							// add twidth to integer stride 

		mov		[w2dDeltaTex.iUVInt],edi		// store integer stride + twidth 
		mov		eax,[iCacheNextSubdivide]

		jmp		CLAMP_DONE_END

NO_CLAMP_END:
		/*---------------------------------------------------------------------------------- */
		/* Initialize walking values */
		fld		st(1)								/* U,V,U */
		fsub	[fU]								/* U-fU,V,U */
		fld		st(1)								/* V,U-fU,V,U */
		fsub	[fV]								/* V-fV,U-fU,V,U */
		fxch	st(1)								/* U-fU,V-fV,V,U */
		fmul	float ptr[fInverseIntTable+edi*4]	/* (U-fU)*C,V-fV,V,U */
		fxch	st(2)								/* V,V-fV,(U-fU)*C,U */
		fstp	[fV]								/* V-fV,(U-fU)*C,U */
		fmul	float ptr[fInverseIntTable+edi*4]	/* (V-fV)*C,(U-fU)*C,U */
		fxch	st(1)								/* (U-fU)*C,(V-fV)*C,U */
		fadd	[dFastFixed16Conversion]			/* f(U-fU)*C,(V-fV)*C,U */
		fxch	st(1)								/* (V-fV)*C,f(U-fU)*C,U */
		/* stall(1) */
		fadd	[dFastFixed16Conversion]			/* f(V-fV)*C,f(U-fU)*C,U */
		fxch	st(2)								/* U,f(U-fU)*C,f(V-fV)*C */
		fstp	[fU]								/* f(U-fU)*C,f(V-fV)*C */
		fstp	[d_temp_a]							/* f(V-fV)*C */
		fstp	[d_temp_b]							

		mov		edi,dword ptr[d_temp_a]			/* uslope */
		mov		eax,dword ptr[d_temp_b]			/* vslope */

		sar		edi,15							/* integer part of uslope */
		mov		ebp,dword ptr[d_temp_a]			/* uslope again */

		shl		ebp,17							/* fractional part of uslope */
		mov		ebx,eax							/* vslope again */

		sar		eax,15							/* integer part of vslope */
		mov		[w2dDeltaTex.uUFrac],ebp		/* store UFrac */

		shl		eax,9							/* ivslope*twidth */

		shl		ebx,17							/* fractional part of vslope */
		mov		ebp,[u4TextureTileMaskStepU]	/* Load mask for integral U step */

		and		edi,ebp							/* Mask integral U before adding. */
		mov		[w2dDeltaTex.uVFrac],ebx		/* store VFrac */

		add		edi,eax							/* ivslope*twidth + iuslope */
		mov		ebp,[iTexWidth]					/* Load texture width. */

		mov		[w2dDeltaTex.iUVInt+4],edi		/* store integer stride */
		add		edi,ebp							/* add twidth to integer stride */

		mov		[w2dDeltaTex.iUVInt],edi		/* store integer stride + twidth */
		mov		eax,[iCacheNextSubdivide]

CLAMP_DONE_END:
		//---------------------------------------------------------------------------------- 
		// Begin Next Subdivision 
		mov		[iNextSubdivide],eax			// eax == iNextSubdivide 
		mov		ebx,[iSubdivideLen]

		test	eax,eax							// Next subdivision is zero length? 
		jz		SUBDIVISION_LOOP

		// scan line is +ve 
		add		eax,ebx
		jle		short DONE_DIVIDE_PIXEL_CACHE_END

		// calc the new +ve ratio 
		fild	iNextSubdivide
		fld		fInvSubdivideLen
		fchs

		// Subdivision is smaller than iSubdivideLen 
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
		mov		[iCacheNextSubdivide],eax
		mov		ebx,[iNextSubdivide]

		cmp		eax,ebx
		je		SUBDIVISION_LOOP

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

		jmp		SUBDIVISION_LOOP

		// When the sub divide equals the cached sub-divide we end up here but 
		// there is an element left on the fp stack. This sould never happen. 
EXIT_BEGIN_NEXT_QUICK_END:

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
		mov		eax,[ecx]TAlphaTexPersp.fxX.i4Fx
		mov		ebx,[ecx]TAlphaTexPersp.fxXDifference.i4Fx

		fld		[ecx]TAlphaTexPersp.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TAlphaTexPersp.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TAlphaTexPersp.indCoord.fInvZ
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
		fstp	[ecx]TAlphaTexPersp.indCoord.fInvZ
		fstp	[ecx]TAlphaTexPersp.indCoord.fVInvZ
		fstp	[ecx]TAlphaTexPersp.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		fstp	[ecx]TAlphaTexPersp.indCoord.fInvZ
		fstp	[ecx]TAlphaTexPersp.indCoord.fUInvZ
		fstp	[ecx]TAlphaTexPersp.indCoord.fVInvZ

FINISH_LOOPING:
		// Y Loop control.
		mov		[ecx]TAlphaTexPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TAlphaTexPersp.fxXDifference.i4Fx,ebx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP
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
	static CWalk2D w2dDeltaTex2;

	TAlphaTexLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
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
		// pscanGlbl		= pscan;
		// pdtriGlbl		= pdtri;
		//
		mov		ebx,[edi]TAlphaTexLinear.fxX
		mov		ecx,[edi]TAlphaTexLinear.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TAlphaTexLinear.indCoord.bfU.i4Int
		mov		ecx,[edi]TAlphaTexLinear.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TAlphaTexLinear.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TAlphaTexLinear.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriGlbl,esi
		mov		pscanGlbl,eax

		// Compute double step.
		mov		ebx,[bfDeltaU]UBigFixed.u4Frac

		and		ebx,0xffff0000

		add		ebx,ebx

		mov		[w2dDeltaTex2]CWalk2D.uUFrac,ebx

		mov		ecx,[bfDeltaU]UBigFixed.i4Int

		adc		ecx,ecx

		and		ecx,[u4TextureTileMaskStepU]

		mov		ebx,[w1dDeltaV]CWalk1D.bfxValue.u4Frac

		and		ebx,0xffff0000

		mov		edx,[w1dDeltaV]CWalk1D.bfxValue.i4Int

		add		edx,edx

		add		ebx,ebx

		mov		[w2dDeltaTex2]CWalk2D.uVFrac,ebx
		mov		ebx,[w1dDeltaV]CWalk1D.iOffsetPerLine

		jnc		NO_V_CARRY

		add		edx,ebx

NO_V_CARRY:

		add		edx,ecx
		
		mov		[w2dDeltaTex2 + 4]CWalk2D.iUVInt,edx

		add		edx,ebx

		mov		[w2dDeltaTex2]CWalk2D.iUVInt,edx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		// Check to see if we should skip this scanline.
		mov		ecx,[esi]CDrawPolygonBase.iY
		mov		ebx,[bEvenScanlinesOnly]

		and		ecx,ebx
		jnz		END_OF_SCANLINE

		// Determine the start and end of the scanline.
		mov		ebx,[eax]TAlphaTexLinear.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
		add		ebx,ecx

		sar		ebx,16
		mov		edi,[eax]TAlphaTexLinear.fxX.i4Fx

		sar		edi,16
		add		edx,ebx

		sub		edi,ebx													// i_pixel
		jge		END_OF_SCANLINE

		//
		// edi = i_pixel
		// edx = i_screen_index
		// eax = pointer to scanline
		//
		mov		ebx,[eax]TAlphaTexPersp.fxX.i4Fx						// Fractional X.
		mov		ebp,[esi]CDrawPolygonBase.iY							// Integer Y.

		sar		ebx,16													// Integer X.
		mov		esi,[eax]TAlphaTexLinear.indCoord.bfU.u4Frac			// Ufrac

		xor		ebp,ebx													// XOR with integer X.
		mov		ebx,gsGlobals.pvScreen									// Pointer the screen.

		mov		ecx,[eax]TAlphaTexLinear.indCoord.w1dV.bfxValue.u4Frac	// Vfrac

		lea		ebx,[ebx+edx*2]											// Base of scanline.
		mov		edx,[eax]TAlphaTexLinear.indCoord.w1dV.bfxValue.i4Int	// Vint

		mov		[pvBaseOfLine],ebx								// Save pointer to destination.
		mov		eax,[eax]TAlphaTexLinear.indCoord.bfU.i4Int		// Uint

		add		edx,eax											// Add Uint to Vint
		mov		ebx,[pvTextureBitmap]							// Load pointer to texture.

		and		edx,[u4TextureTileMask]							// Mask for tiling.

		// if ((y xor x) & 1) skip a pixel.
		test	ebp,1
		jz		NO_SKIP

		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac					// Step V fraction.

		sbb		ebp,ebp											// Get borrow from V fraction step.
		add		esi,[w2dDeltaTex]CWalk2D.uUFrac					// Step U fraction.

		adc		edx,[w2dDeltaTex + ebp*4 + 4]CWalk2D.iUVInt		// Integer step + u carry.

		and		edx,[u4TextureTileMask]							// Mask for tiling.

		inc		edi
		jz		END_OF_SCANLINE

NO_SKIP:
		xor		eax,eax											// Clear eax.
		mov		ebp,[w2dDeltaTex2]CWalk2D.uVFrac				// Load V fraction step.
	
		ALIGN	16

		//-----------------------------------------------------------------------
		// Draw the scanline.
		//
		// eax = temp for pixel
		// ebx = base of texture map / base of scanline
		// ecx = Ufrac
		// edx = UVint
		// esi = Vfrac
		// edi = offset & count
		// ebp = vfrac step / v carry
		//
INNER_LOOP:
		add		ecx,ebp											// Step V fraction.
		movzx	eax,word ptr[ebx + edx*2]						// Read texture value.

		sbb		ebp,ebp											// Get borrow from V fraction step.

		add		esi,[w2dDeltaTex2]CWalk2D.uUFrac				// Step U fraction.

		adc		edx,[w2dDeltaTex2 + ebp*4 + 4]CWalk2D.iUVInt	// Integer step + u carry.
		mov		ebp,[au2AlphaTable]								// Conversion table.

		cmp		eax,0x00004000
		jle		PIXEL_TRANS

		and		eax,0x00000fff									// Mask colors.
		mov		ebx,[pvBaseOfLine]								// Load pointer to destination.

		movzx	eax,word ptr[ebp + eax*2]						// Color conversion.

		mov		[ebx + edi*2],ax								// Store pixel.
		mov		ebx,[pvTextureBitmap]							// Load pointer to texture.

PIXEL_TRANS:
		and		edx,[u4TextureTileMask]							// Mask for tiling.
		add		edi,2											// Step 2 pixels

		mov		ebp,[w2dDeltaTex2]CWalk2D.uVFrac				// Load V fraction step.
		jl		INNER_LOOP

END_OF_SCANLINE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TAlphaTexLinear.indCoord.bfU.u4Frac
		mov		edx,[eax]TAlphaTexLinear.indCoord.bfU.i4Int

		mov		edi,[eax]TAlphaTexLinear.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TAlphaTexLinear.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TAlphaTexLinear.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebx,[eax]TAlphaTexLinear.fxXDifference
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

		add		edi,[eax]TAlphaTexLinear.indCoord.w1dV.iOffsetPerLine
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

		add		edi,[eax]TAlphaTexLinear.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TAlphaTexLinear.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TAlphaTexLinear.indCoord.w1dV.bfxValue.i4Int,edi

		mov		[eax]TAlphaTexLinear.fxXDifference,ebx
		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TAlphaTexLinear.indCoord.bfU.u4Frac,ecx
		mov		[eax]TAlphaTexLinear.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,[gsGlobals.u4LinePixels]

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TAlphaTexLinear.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TAlphaTexLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop ebp
	}
}
