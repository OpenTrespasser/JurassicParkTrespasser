#pragma once

/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Macros for scanline prologue and epilogue.
 *		Optimized for the Pentium Pro Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P6/ScanlineAsmMacros.hpp                      $
 * 
 * 2     98.03.09 11:40p Mmouni
 * Fixed problem with staring texture U co-ordinates greater that the map width.
 * 
 * 1     97.12.01 4:23p Mmouni
 * Pro version created from Pentium version with branches replaced by CMOV's.
 * 
 * 5     97.11.20 3:13p Mmouni
 * Added always clamp versions of perspective prologue and epilogue.
 * 
 * 4     10/10/97 1:47p Mmouni
 * All inner loops are now left to right only, and optimized some.
 * 
 * 3     10/02/97 10:40a Mmouni
 * Left to right and right to left loops now use shifts by 9 instead of IMULs for non-clamped
 * (ie packed) textures.
 * 
 * 2     9/29/97 11:53a Mmouni
 * Optimized some, now adjusts the size of  the first span to preserve alignment.
 * 
 * 1     9/15/97 2:08p Mmouni
 * Created from IndexPerspectiveTEx.hpp.
 * 
 **********************************************************************************************/

//
// Setup for prespective correction and looping.
// Left to right version (dir = 1).
//
// On Entry:
//	 esi = pointer to polygon.
//	 ecx = pointer to scanline.
//
// On Exit:
//	 eax = i_pixel_inner
//	 edi = inner loop count
//	 ecx=vFrac, edx=UVInt, esi=uFrac
//	 i_pixel, i_screen_index are set.
//	 perspective correction values are setup.
//
#define PERSPECTIVE_SCANLINE_PROLOGUE(SLTYPE) \
__asm		/* Determine the start and end of the scanline. */ \
__asm		mov		ebx,[ecx]SLTYPE.fxX.i4Fx \
__asm		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx \
__asm \
__asm		add		ebx,eax \
__asm		mov		eax,[ecx]SLTYPE.fxX.i4Fx \
__asm \
__asm		sar		ebx,16 \
__asm		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex \
__asm \
__asm		sar		eax,16 \
__asm		add		edx,ebx \
__asm \
__asm		sub		eax,ebx									/* i_pixel */ \
__asm		jge		END_OF_SCANLINE \
__asm \
__asm		mov		[i_screen_index],edx \
__asm		mov		[i_pixel],eax \
__asm \
__asm		/*--------------------------------------------------------------------------- */ \
__asm		/* Caclulate values for the first correction span, and start the divide for */ \
__asm		/* the next span. */ \
__asm		/* */ \
__asm \
__asm		/* Copy global texture values. */ \
__asm		mov		ebx,[ecx]SLTYPE.indCoord.fUInvZ \
__asm		mov		esi,[ecx]SLTYPE.indCoord.fVInvZ \
__asm \
__asm		mov		edi,[ecx]SLTYPE.indCoord.fInvZ \
__asm		mov		fGUInvZ,ebx \
__asm \
__asm		mov		fGVInvZ,esi \
__asm		mov		fGInvZ,edi \
__asm \
__asm		mov		ebx,fDUInvZEdge \
__asm		mov		esi,fDVInvZEdge \
__asm \
__asm		mov		edi,fDInvZEdge \
__asm		mov		fDUInvZScanline,ebx \
__asm \
__asm		mov		fDVInvZScanline,esi \
__asm		mov		fDInvZScanline,edi \
__asm \
__asm		mov		ebx,[iSubdivideLen] \
__asm \
__asm		/* scan line is +ve */ \
__asm		add		eax,ebx \
__asm		jg		short PARTIAL_SUBDIVIDE \
__asm \
__asm		/* Check alignment. */ \
__asm		add		edx,eax						/* i_screen_index + i_pixel */ \
__asm		add		edx,edx						/* (i_screen_index + i_pixel) * 2 */ \
__asm \
__asm		and		edx,3 \
__asm		jz		short DONE_DIVIDE_PIXEL \
__asm \
__asm		/* We are using subdivide length - 1 for alignment. */ \
__asm		mov		ebx,[fDUInvZEdgeMinusOne] \
__asm		mov		esi,[fDVInvZEdgeMinusOne] \
__asm \
__asm		mov		edi,[fDInvZEdgeMinusOne] \
__asm		mov		[fDUInvZScanline],ebx \
__asm \
__asm		mov		[fDVInvZScanline],esi \
__asm		mov		[fDInvZScanline],edi \
__asm \
__asm		dec		eax \
__asm		jmp		short DONE_DIVIDE_PIXEL \
__asm \
__asm PARTIAL_SUBDIVIDE: \
__asm		/* calc the new +ve ratio */ \
__asm		fild	[i_pixel] \
__asm		fld		fInvSubdivideLen \
__asm		fchs \
__asm \
__asm		/* Subdivision is smaller than iSubdivideLen */ \
__asm		fmulp	st(1),st(0)			/* st(0) = (-)fInvSubdivideLen * i_pixel; */ \
__asm		fld		fDUInvZScanline		/* U C */ \
__asm		xor		eax,eax \
__asm		fmul	st(0),st(1)			/* U*C C */ \
__asm		fxch	st(1)				/* C U*C */ \
__asm		fld		fDVInvZScanline		/* V C U*C */ \
__asm		fxch	st(1)				/* C V U*C */ \
__asm		fmul	st(1),st(0)			/* C V*C U*C */ \
__asm		/* stall(1) */ \
__asm		fmul	fDInvZScanline		/* Z*C V*C U*C */ \
__asm		fxch	st(2)				/* U*C V*C Z*C */ \
__asm		fstp	fDUInvZScanline		/* V*C Z*C */ \
__asm		fstp	fDVInvZScanline		/* Z*C */ \
__asm		fstp	fDInvZScanline \
__asm \
__asm DONE_DIVIDE_PIXEL: \
__asm		/* Get current u, v and z values. */ \
__asm		mov		[iNextSubdivide],eax \
__asm		mov		ebx,dword ptr[fGInvZ]		/* f_z = fInverse(fGInvZ); */ \
__asm \
__asm		mov		eax,iFI_SIGN_EXPONENT_SUB \
__asm \
__asm		sub		eax,ebx \
__asm		and		ebx,iFI_MASK_MANTISSA \
__asm \
__asm		sar		ebx,iSHIFT_MANTISSA \
__asm		and		eax,iFI_MASK_SIGN_EXPONENT \
__asm \
__asm		fld		[fGUInvZ]					/* U/Z */ \
__asm \
__asm		add		eax,dword ptr[i4InverseMantissa + ebx*4] \
__asm		mov		ebx,[bClampUV] \
__asm \
__asm		mov		dword ptr[f_z],eax \
__asm \
__asm		/* Are we clamping for this polygon */ \
__asm		test	ebx,ebx \
__asm		jz		short NO_CLAMP \
__asm \
__asm		/*--------------------------------------------------------------------------- */ \
__asm		/* Set current texture coordinates (clamped). */ \
__asm		fmul	[f_z] \
__asm		fld		[fGVInvZ] \
__asm		fmul	[f_z] \
__asm		fxch	st(1) \
__asm		/* stall(1) */ \
__asm		fstp	[f_u] \
__asm \
__asm		/* Clamp f_u			 */ \
__asm		mov		eax,[f_u] \
__asm		mov		ebx,fTexEdgeTolerance \
__asm		mov		ecx,fTexWidth \
__asm		cmp		eax,ebx \
__asm		cmovl	(_eax,_ebx) \
__asm		cmp		eax,ecx \
__asm		cmovg	(_eax,_ecx) \
__asm		mov		ecx,fTexHeight \
__asm		mov		[f_u],eax \
__asm		fld		[f_u] \
__asm \
__asm		fadd	[dFastFixed16Conversion] \
__asm		fxch	st(1) \
__asm \
__asm		/* Clamp f_v */ \
__asm		fstp	[f_v] \
__asm		mov		eax,[f_v] \
__asm		cmp		eax,ebx \
__asm		cmovl	(_eax,_ebx) \
__asm		cmp		eax,ecx \
__asm		cmovg	(_eax,_ecx) \
__asm		mov		[f_v],eax \
__asm		fld		[f_v] \
__asm \
__asm		fadd	[dFastFixed16Conversion] \
__asm		fxch	st(1) \
__asm		fstp	[d_temp_a] \
__asm		fld		[fGUInvZ] \
__asm		fxch	st(1) \
__asm		fstp	[d_temp_b] \
__asm \
__asm		/* Increment u, v and z values. */ \
__asm		fadd	[fDUInvZScanline] \
__asm		fld		[fGVInvZ] \
__asm		fadd	[fDVInvZScanline] \
__asm		fxch	st(1) \
__asm		fld		[fGInvZ] \
__asm		fadd	[fDInvZScanline] \
__asm \
__asm		/* Setup esi=uFrac, ecx=vFrac, edx=UVInt for Abrash texture loop. */ \
__asm		mov		edx,dword ptr[d_temp_a]			/* U (16.16) */ \
__asm		mov		eax,dword ptr[d_temp_b]			/* V (16.16) */ \
__asm \
__asm		sar		eax,16							/* Integral V */ \
__asm		mov		ecx,[iTexWidth]					/* Texture width. */ \
__asm \
__asm		imul	eax,ecx							/* iv*twidth */ \
__asm \
__asm		sar		edx,16							/* Integral U */ \
__asm		mov		esi,dword ptr[d_temp_a]			/* Copy of U */ \
__asm \
__asm		shl		esi,16							/* UFrac */ \
__asm		mov		ecx,dword ptr[d_temp_b]			/* Copy of V */ \
__asm \
__asm		shl		ecx,16							/* VFrac */ \
__asm		add		edx,eax							/* iu + iv*twidth */ \
__asm \
__asm		fstp	[fGInvZ] \
__asm \
__asm		/* Get next u, v and z values. */ \
__asm		mov		ebx,dword ptr[fGInvZ]			/* f_next_z = fInverse(fGInvZ); */ \
__asm		mov		eax,iFI_SIGN_EXPONENT_SUB \
__asm \
__asm		sub		eax,ebx \
__asm		and		ebx,iFI_MASK_MANTISSA \
__asm \
__asm		sar		ebx,iSHIFT_MANTISSA \
__asm		and		eax,iFI_MASK_SIGN_EXPONENT \
__asm \
__asm		fst		[fGUInvZ] \
__asm \
__asm		mov		ebx,dword ptr[i4InverseMantissa + ebx*4] \
__asm		mov		edi,[i_pixel] \
__asm \
__asm		add		eax,ebx \
__asm		mov		ebx,[iNextSubdivide] \
__asm \
__asm		mov		dword ptr[f_next_z],eax \
__asm		sub		edi,ebx \
__asm \
__asm		/* Set new texture coordinate increments. */ \
__asm		fmul	[f_next_z] \
__asm		fxch	st(1) \
__asm		fst		[fGVInvZ] \
__asm		fmul	[f_next_z] \
__asm		fxch	st(1) \
__asm		fstp	[fU]			/* V */ \
__asm \
__asm		/* Clamp fU			 */ \
__asm		mov		eax,[fU] \
__asm		mov		ebx,fTexEdgeTolerance \
__asm		mov		ebp,fTexWidth \
__asm		cmp		eax,ebx \
__asm		cmovl	(_eax,_ebx) \
__asm		cmp		eax,ebp \
__asm		cmovg	(_eax,_ebp) \
__asm		mov		ebp,fTexHeight \
__asm		mov		[fU],eax \
__asm \
__asm		fld		[fU]							/* U,V */ \
__asm		fsub	[f_u]							/* U-fu,V */ \
__asm		fxch	st(1)							/* V,U-fu */ \
__asm		fstp	[fV]							/* U-fu */ \
__asm \
__asm		/* Clamp fV */ \
__asm		mov		eax,[fV] \
__asm		cmp		eax,ebx \
__asm		cmovl	(_eax,_ebx) \
__asm		cmp		eax,ebp \
__asm		cmovg	(_eax,_ebp) \
__asm		mov		[fV],eax \
__asm \
__asm		fld		[fV]								/* V U-fu */ \
__asm		fsub	[f_v]								/* V-fv,U-fu */ \
__asm		fxch	st(1)								/* U,V */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (C*U),V */ \
__asm		fxch	st(1)								/* V,(C*U) */ \
__asm		/* stall(1) */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (C*V),(C*U) */ \
__asm		fxch	st(1)								/* (C*U),(C*V) */ \
__asm		fadd	[dFastFixed16Conversion] \
__asm		fxch	st(1) \
__asm		/* stall(1) */ \
__asm		fadd	[dFastFixed16Conversion] \
__asm		fxch	st(1) \
__asm		/* stall(1) */ \
__asm		fstp	[d_temp_a] \
__asm		fstp	[d_temp_b] \
__asm \
__asm		mov		edi,dword ptr[d_temp_a]			/* uslope */ \
__asm		mov		eax,dword ptr[d_temp_b]			/* vslope */ \
__asm \
__asm		sar		edi,16							/* integer part of uslope */ \
__asm		mov		ebp,dword ptr[d_temp_a]			/* uslope again */ \
__asm \
__asm		shl		ebp,16							/* fractional part of uslope */ \
__asm		mov		ebx,eax							/* vslope again */ \
__asm \
__asm		sar		eax,16							/* integer part of vslope */ \
__asm		mov		[w2dDeltaTex.uUFrac],ebp		/* store UFrac */ \
__asm \
__asm		imul	eax,[iTexWidth]					/* ivslope*twidth */ \
__asm \
__asm		shl		ebx,16							/* fractional part of vslope */ \
__asm		mov		ebp,[u4TextureTileMaskStepU]	/* Load mask for integral U step */ \
__asm \
__asm		and		edi,ebp							/* Mask integral U before adding. */ \
__asm		mov		[w2dDeltaTex.uVFrac],ebx		/* store VFrac */ \
__asm \
__asm		add		edi,eax							/* ivslope*twidth + iuslope */ \
__asm		mov		ebp,[iTexWidth]					/* Load texture width. */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt+4],edi		/* store integer stride */ \
__asm		add		edi,ebp							/* add twidth to integer stride */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt],edi		/* store integer stride + twidth */ \
__asm		mov		eax,[iNextSubdivide] \
__asm \
__asm		jmp		CLAMP_DONE \
__asm \
__asm NO_CLAMP: \
__asm		/*--------------------------------------------------------------------------- */ \
__asm		/* Set current texture coordinates (not clamped). */ \
__asm		fmul	[f_z]						/* U */ \
__asm		fld		[fGVInvZ]					/* V/Z U */ \
__asm		fmul	[f_z]						/* V U */ \
__asm		fxch	st(1)						/* U V */ \
__asm		/* stall(1) */ \
__asm		fst		[f_u] \
__asm		fadd	[dFastFixed16Conversion]	/* iU V */ \
__asm		fxch	st(1)						/* V iU */ \
__asm		fst		[f_v]						 \
__asm		fadd	[dFastFixed16Conversion]	/* iV iU */ \
__asm		fxch	st(1)						/* iU iV */ \
__asm		fstp	[d_temp_a]					/* iV */ \
__asm		fld		[fGUInvZ]					/* U/Z iV */ \
__asm		fxch	st(1)						/* iV U/Z */ \
__asm		fstp	[d_temp_b]					/* U/Z */ \
__asm \
__asm		/* Increment u, v and z values. */ \
__asm		fadd	[fDUInvZScanline]			/* U2/Z */ \
__asm		fld		[fGVInvZ] \
__asm		fadd	[fDVInvZScanline]			/* V2/Z U2/Z */ \
__asm		fxch	st(1)						/* U2/Z V2/Z */ \
__asm		fld		[fGInvZ]					/* 1/Z U2/Z V2/Z */ \
__asm		fadd	[fDInvZScanline]			/* 1/Z2 U2/Z V2/Z */ \
__asm \
__asm		/* Setup esi=uFrac, ecx=vFrac, edx=UVInt for Abrash texture loop. */ \
__asm		mov		edx,dword ptr[d_temp_a]			/* U (16.16) */ \
__asm		mov		eax,dword ptr[d_temp_b]			/* V (16.16) */ \
__asm \
__asm		sar		eax,16							/* Integral V */ \
__asm		mov		ecx,[iTexWidth]					/* Texture width. */ \
__asm \
__asm		shl		eax,9							/* iv*twidth */ \
__asm \
__asm		sar		edx,16							/* Integral U */ \
__asm		mov		ebp,[u4TextureTileMaskStepU]	/* Load mask for integral U */ \
__asm \
__asm		and		edx,ebp							/* Mask integral U before adding. */ \
__asm		mov		esi,dword ptr[d_temp_a]			/* Copy of U */ \
__asm \
__asm		shl		esi,16							/* UFrac */ \
__asm		mov		ecx,dword ptr[d_temp_b]			/* Copy of V */ \
__asm \
__asm		shl		ecx,16							/* VFrac */ \
__asm		add		edx,eax							/* iu + iv*twidth */ \
__asm \
__asm		fstp	[fGInvZ] \
__asm \
__asm		/* Get next u, v and z values. */ \
__asm		mov		ebx,dword ptr[fGInvZ]			/* f_next_z = fInverse(fGInvZ); */ \
__asm		mov		eax,iFI_SIGN_EXPONENT_SUB \
__asm \
__asm		sub		eax,ebx \
__asm		and		ebx,iFI_MASK_MANTISSA \
__asm \
__asm		sar		ebx,iSHIFT_MANTISSA \
__asm		and		eax,iFI_MASK_SIGN_EXPONENT \
__asm \
__asm		fst		[fGUInvZ] \
__asm \
__asm		mov		ebx,dword ptr[i4InverseMantissa + ebx*4] \
__asm		mov		edi,[i_pixel] \
__asm \
__asm		add		eax,ebx \
__asm		mov		ebx,[iNextSubdivide] \
__asm \
__asm		mov		dword ptr[f_next_z],eax \
__asm		sub		edi,ebx \
__asm \
__asm		/* Set new texture coordinate increments. */ \
__asm		fmul	[f_next_z]						/* U,V/Z */ \
__asm		fxch	st(1)							/* V/Z,U */ \
__asm		fst		[fGVInvZ] \
__asm		fmul	[f_next_z]						/* V,U */ \
__asm		fxch	st(1)							/* U,V */ \
__asm		fst		[fU]							/* U,V */ \
__asm		fsub	[f_u]							/* U-fu,V */ \
__asm		fxch	st(1)							/* V,U-fu */ \
__asm		fst		[fV] \
__asm		fsub	[f_v]								/* V-fv,U-fu */ \
__asm		fxch	st(1)								/* U,V */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (C*U),V */ \
__asm		fxch	st(1)								/* V,(C*U) */ \
__asm		/* stall(1) */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (C*V),(C*U) */ \
__asm		fxch	st(1)								/* (C*U),(C*V) */ \
__asm		fadd	[dFastFixed16Conversion] \
__asm		fxch	st(1) \
__asm		/* stall(1) */ \
__asm		fadd	[dFastFixed16Conversion] \
__asm		fxch	st(1) \
__asm		/* stall(1) */ \
__asm		fstp	[d_temp_a] \
__asm		fstp	[d_temp_b] \
__asm \
__asm		mov		edi,dword ptr[d_temp_a]			/* uslope */ \
__asm		mov		eax,dword ptr[d_temp_b]			/* vslope */ \
__asm \
__asm		sar		edi,16							/* integer part of uslope */ \
__asm		mov		ebp,dword ptr[d_temp_a]			/* uslope again */ \
__asm \
__asm		shl		ebp,16							/* fractional part of uslope */ \
__asm		mov		ebx,eax							/* vslope again */ \
__asm \
__asm		sar		eax,16							/* integer part of vslope */ \
__asm		mov		[w2dDeltaTex.uUFrac],ebp		/* store UFrac */ \
__asm \
__asm		shl		eax,9							/* ivslope*twidth */ \
__asm \
__asm		shl		ebx,16							/* fractional part of vslope */ \
__asm		mov		ebp,[u4TextureTileMaskStepU]	/* Load mask for integral U step */ \
__asm \
__asm		and		edi,ebp							/* Mask integral U before adding. */ \
__asm		mov		[w2dDeltaTex.uVFrac],ebx		/* store VFrac */ \
__asm \
__asm		add		edi,eax							/* ivslope*twidth + iuslope */ \
__asm		mov		ebp,[iTexWidth]					/* Load texture width. */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt+4],edi		/* store integer stride */ \
__asm		add		edi,ebp							/* add twidth to integer stride */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt],edi		/* store integer stride + twidth */ \
__asm		mov		eax,[iNextSubdivide] \
__asm \
__asm		/* --------------------------------------------------------------------------------- */ \
__asm		/* Both clmaped and non-clamped primitives end up here.. */ \
__asm CLAMP_DONE: \
__asm \
__asm		test	eax,eax							/* Next subdivision is zero length? */ \
__asm		jz		SUBDIVISION_LOOP \
__asm \
__asm		/* iCacheNextSubdivide = iSetNextDividePixel(iNextSubdivide); */ \
__asm		mov		edi,[iSubdivideLen] \
__asm \
__asm		/* scan line is +ve */ \
__asm		add		eax,edi \
__asm		jle		short DONE_DIVIDE_PIXEL_CACHE \
__asm \
__asm		/* calc the new +ve ratio */ \
__asm		fild	iNextSubdivide \
__asm		fld		fInvSubdivideLen \
__asm		fchs \
__asm \
__asm		/* Subdivision is smaller than iSubdivideLen */ \
__asm		fmulp	st(1),st(0)			/* st(0) = (-)fInvSubdivideLen * i_pixel; */ \
__asm		fld		fDUInvZEdge			/* U C */ \
__asm \
__asm		xor		eax,eax \
__asm \
__asm		fmul	st(0),st(1)			/* U*C C */ \
__asm		fxch	st(1)				/* C U*C */ \
__asm		fld		fDVInvZEdge			/* V C U*C */ \
__asm		fxch	st(1)				/* C V U*C */ \
__asm		fmul	st(1),st(0)			/* C V*C U*C */ \
__asm \
__asm		mov		[iCacheNextSubdivide],eax \
__asm \
__asm		fmul	fDInvZEdge			/* Z*C V*C U*C */ \
__asm		fxch	st(2)				/* U*C V*C Z*C */ \
__asm		fst		fDUInvZScanline \
__asm		fadd	fGUInvZ \
__asm		fxch	st(1) \
__asm		fst		fDVInvZScanline \
__asm		fadd	fGVInvZ \
__asm		fxch	st(2) \
__asm		fst		fDInvZScanline \
__asm		fadd	fGInvZ \
__asm		fxch	st(2) \
__asm		fstp	fGVInvZ			 \
__asm		fstp	fGUInvZ \
__asm		fst		fGInvZ \
__asm		fdivr	fOne							/* Start the next division. */ \
__asm \
__asm		jmp		SUBDIVISION_LOOP \
__asm \
__asm DONE_DIVIDE_PIXEL_CACHE: \
__asm		/* Copy texture values. */ \
__asm		mov		ebp,[fDUInvZEdge] \
__asm		mov		ebx,[fDVInvZEdge] \
__asm \
__asm		mov		edi,[fDInvZEdge] \
__asm		mov		[fDUInvZScanline],ebp \
__asm \
__asm		mov		[fDVInvZScanline],ebx \
__asm		mov		[fDInvZScanline],edi \
__asm \
__asm		fld		fDUInvZScanline			/* U */ \
__asm		fadd	fGUInvZ \
__asm		fld		fDVInvZScanline			/* V U */ \
__asm		fadd	fGVInvZ \
__asm		fld		fDInvZScanline			/* Z V U */ \
__asm		fadd	fGInvZ \
__asm		fxch	st(2)					/* U V Z */ \
__asm		fstp	fGUInvZ \
__asm		fstp	fGVInvZ \
__asm		fst		fGInvZ \
__asm		fdivr	fOne							/* Start the next division. */ \
__asm \
__asm		mov		[iCacheNextSubdivide],eax \
__asm \
__asm SUBDIVISION_LOOP: \
__asm		/*--------------------------------------------------------------------------- */ \
__asm		/* Start the next subdivision. */ \
__asm		/* ecx,edx,esi = texture values */ \
__asm		/* */ \
__asm		mov		edi,[i_pixel] \
__asm		mov		eax,[iNextSubdivide] \
__asm		sub		edi,eax


//
// Do perspective correction and looping.
// Left to right version.
//
#define PERSPECTIVE_SCANLINE_EPILOGUE \
__asm		/*---------------------------------------------------------------------------------- */ \
__asm		/* Implement perspective correction. */ \
__asm		/* stack top contains the result of the divide */ \
__asm		/* Preserve: ecx,edx,esi */ \
__asm		/* */ \
__asm		mov		edi,[iNextSubdivide] \
__asm		mov		eax,[iCacheNextSubdivide] \
__asm \
__asm		sub		edi,eax \
__asm		jz		short EXIT_BEGIN_NEXT_QUICK_END \
__asm \
__asm		fld		[fGUInvZ]					/* U/Z,Z */ \
__asm		fxch	st(1)						/* Z,U/Z */ \
__asm		fmul	st(1),st(0)					/* Z,U */ \
__asm		mov		ebx,[bClampUV] \
__asm		fmul	[fGVInvZ]					/* V,U */ \
__asm \
__asm		/* Are we clamping for this polygon? */ \
__asm		test	ebx,ebx \
__asm		jz		short NO_CLAMP_END \
__asm \
__asm		/*---------------------------------------------------------------------------------- */ \
__asm		/* Clamp U and V */ \
__asm		fxch	st(1) \
__asm		fstp	[f_u] \
__asm		fstp	[f_v] \
__asm \
__asm		mov		ebp,[f_u] \
__asm		mov		ebx,[fTexEdgeTolerance] \
__asm		mov		eax,[f_v] \
__asm		cmp		eax,ebx \
__asm		cmovl	(_eax,_ebx) \
__asm		cmp		ebp,ebx \
__asm		cmovl	(_ebp,_ebx) \
__asm		mov		ebx,[fTexWidth] \
__asm		cmp		ebp,ebx \
__asm		cmovg	(_ebp,_ebx) \
__asm		mov		ebx,[fTexHeight] \
__asm		cmp		eax,ebx \
__asm		cmovg	(_eax,_ebx) \
__asm		mov		[f_u],ebp \
__asm		mov		[f_v],eax \
__asm \
__asm		fld		[f_u] \
__asm		fld		[f_v] \
__asm \
__asm		/*---------------------------------------------------------------------------------- */ \
__asm		/* Initialize walking values */ \
__asm		fld		st(1)								/* U,V,U */ \
__asm		fsub	[fU]								/* U-fU,V,U */ \
__asm		fld		st(1)								/* V,U-fU,V,U */ \
__asm		fsub	[fV]								/* V-fV,U-fU,V,U */ \
__asm		fxch	st(1)								/* U-fU,V-fV,V,U */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (U-fU)*C,V-fV,V,U */ \
__asm		fxch	st(2)								/* V,V-fV,(U-fU)*C,U */ \
__asm		fstp	[fV]								/* V-fV,(U-fU)*C,U */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (V-fV)*C,(U-fU)*C,U */ \
__asm		fxch	st(1)								/* (U-fU)*C,(V-fV)*C,U */ \
__asm		fadd	[dFastFixed16Conversion]			/* f(U-fU)*C,(V-fV)*C,U */ \
__asm		fxch	st(1)								/* (V-fV)*C,f(U-fU)*C,U */ \
__asm		/* stall(1) */ \
__asm		fadd	[dFastFixed16Conversion]			/* f(V-fV)*C,f(U-fU)*C,U */ \
__asm		fxch	st(2)								/* U,f(U-fU)*C,f(V-fV)*C */ \
__asm		fstp	[fU]								/* f(U-fU)*C,f(V-fV)*C */ \
__asm		fstp	[d_temp_a]							/* f(V-fV)*C */ \
__asm		fstp	[d_temp_b]							 \
__asm \
__asm		mov		edi,dword ptr[d_temp_a]			/* uslope */ \
__asm		mov		eax,dword ptr[d_temp_b]			/* vslope */ \
__asm \
__asm		sar		edi,16							/* integer part of uslope */ \
__asm		mov		ebp,dword ptr[d_temp_a]			/* uslope again */ \
__asm \
__asm		shl		ebp,16							/* fractional part of uslope */ \
__asm		mov		ebx,eax							/* vslope again */ \
__asm \
__asm		sar		eax,16							/* integer part of vslope */ \
__asm		mov		[w2dDeltaTex.uUFrac],ebp		/* store UFrac */ \
__asm \
__asm		imul	eax,[iTexWidth]					/* ivslope*twidth */ \
__asm \
__asm		shl		ebx,16							/* fractional part of vslope */ \
__asm		mov		ebp,[u4TextureTileMaskStepU]	/* Load mask for integral U step */ \
__asm \
__asm		and		edi,ebp							/* Mask integral U before adding. */ \
__asm		mov		[w2dDeltaTex.uVFrac],ebx		/* store VFrac */ \
__asm \
__asm		add		edi,eax							/* ivslope*twidth + iuslope */ \
__asm		mov		ebp,[iTexWidth]					/* Load texture width. */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt+4],edi		/* store integer stride */ \
__asm		add		edi,ebp							/* add twidth to integer stride */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt],edi		/* store integer stride + twidth */ \
__asm		mov		eax,[iCacheNextSubdivide] \
__asm \
__asm		jmp		CLAMP_DONE_END \
__asm \
__asm NO_CLAMP_END: \
__asm		/*---------------------------------------------------------------------------------- */ \
__asm		/* Initialize walking values */ \
__asm		fld		st(1)								/* U,V,U */ \
__asm		fsub	[fU]								/* U-fU,V,U */ \
__asm		fld		st(1)								/* V,U-fU,V,U */ \
__asm		fsub	[fV]								/* V-fV,U-fU,V,U */ \
__asm		fxch	st(1)								/* U-fU,V-fV,V,U */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (U-fU)*C,V-fV,V,U */ \
__asm		fxch	st(2)								/* V,V-fV,(U-fU)*C,U */ \
__asm		fstp	[fV]								/* V-fV,(U-fU)*C,U */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (V-fV)*C,(U-fU)*C,U */ \
__asm		fxch	st(1)								/* (U-fU)*C,(V-fV)*C,U */ \
__asm		fadd	[dFastFixed16Conversion]			/* f(U-fU)*C,(V-fV)*C,U */ \
__asm		fxch	st(1)								/* (V-fV)*C,f(U-fU)*C,U */ \
__asm		/* stall(1) */ \
__asm		fadd	[dFastFixed16Conversion]			/* f(V-fV)*C,f(U-fU)*C,U */ \
__asm		fxch	st(2)								/* U,f(U-fU)*C,f(V-fV)*C */ \
__asm		fstp	[fU]								/* f(U-fU)*C,f(V-fV)*C */ \
__asm		fstp	[d_temp_a]							/* f(V-fV)*C */ \
__asm		fstp	[d_temp_b]							 \
__asm \
__asm		mov		edi,dword ptr[d_temp_a]			/* uslope */ \
__asm		mov		eax,dword ptr[d_temp_b]			/* vslope */ \
__asm \
__asm		sar		edi,16							/* integer part of uslope */ \
__asm		mov		ebp,dword ptr[d_temp_a]			/* uslope again */ \
__asm \
__asm		shl		ebp,16							/* fractional part of uslope */ \
__asm		mov		ebx,eax							/* vslope again */ \
__asm \
__asm		sar		eax,16							/* integer part of vslope */ \
__asm		mov		[w2dDeltaTex.uUFrac],ebp		/* store UFrac */ \
__asm \
__asm		shl		eax,9							/* ivslope*twidth */ \
__asm \
__asm		shl		ebx,16							/* fractional part of vslope */ \
__asm		mov		ebp,[u4TextureTileMaskStepU]	/* Load mask for integral U step */ \
__asm \
__asm		and		edi,ebp							/* Mask integral U before adding. */ \
__asm		mov		[w2dDeltaTex.uVFrac],ebx		/* store VFrac */ \
__asm \
__asm		add		edi,eax							/* ivslope*twidth + iuslope */ \
__asm		mov		ebp,[iTexWidth]					/* Load texture width. */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt+4],edi		/* store integer stride */ \
__asm		add		edi,ebp							/* add twidth to integer stride */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt],edi		/* store integer stride + twidth */ \
__asm		mov		eax,[iCacheNextSubdivide] \
__asm \
__asm CLAMP_DONE_END: \
__asm \
__asm		/*---------------------------------------------------------------------------------- */ \
__asm		/* Begin Next Subdivision */ \
__asm		mov		[iNextSubdivide],eax			/* eax == iNextSubdivide */ \
__asm		mov		ebx,[iSubdivideLen] \
__asm \
__asm		test	eax,eax							/* Next subdivision is zero length? */ \
__asm		jz		SUBDIVISION_LOOP \
__asm \
__asm		/* scan line is +ve */ \
__asm		add		eax,ebx \
__asm		jle		short DONE_DIVIDE_PIXEL_CACHE_END \
__asm \
__asm		/* calc the new +ve ratio */ \
__asm		fild	iNextSubdivide \
__asm		fld		fInvSubdivideLen \
__asm		fchs \
__asm \
__asm		/* Subdivision is smaller than iSubdivideLen */ \
__asm		fmulp	st(1),st(0)			/* st(0) = (-)fInvSubdivideLen * i_pixel; */ \
__asm		fld		fDUInvZScanline		/* U C */ \
__asm \
__asm		xor		eax,eax \
__asm \
__asm		fmul	st(0),st(1)			/* U*C C */ \
__asm		fxch	st(1)				/* C U*C */ \
__asm		fld		fDVInvZScanline		/* V C U*C */ \
__asm		fxch	st(1)				/* C V U*C */ \
__asm		fmul	st(1),st(0)			/* C V*C U*C */ \
__asm \
__asm		mov		[iCacheNextSubdivide],eax \
__asm \
__asm		fmul	fDInvZScanline		/* Z*C V*C U*C */ \
__asm		fxch	st(2)				/* U*C V*C Z*C */ \
__asm		fst		fDUInvZScanline			/* U V Z */ \
__asm		fadd	fGUInvZ \
__asm		fxch	st(1)					/* V U Z */ \
__asm		fst		fDVInvZScanline \
__asm		fadd	fGVInvZ \
__asm		fxch	st(2)					/* Z U V */ \
__asm		fst		fDInvZScanline \
__asm		fadd	fGInvZ \
__asm		fxch	st(2)					/* V U Z */ \
__asm		fstp	fGVInvZ			 \
__asm		fstp	fGUInvZ \
__asm		fst		fGInvZ \
__asm		fdivr	fOne					/* Start the next division. */ \
__asm \
__asm		jmp		SUBDIVISION_LOOP \
__asm \
__asm DONE_DIVIDE_PIXEL_CACHE_END: \
__asm		mov		[iCacheNextSubdivide],eax \
__asm		mov		ebx,[iNextSubdivide] \
__asm \
__asm		cmp		eax,ebx \
__asm		je		SUBDIVISION_LOOP \
__asm \
__asm		fld		fDUInvZScanline			/* U */ \
__asm		fadd	fGUInvZ \
__asm		fld		fDVInvZScanline			/* V U */ \
__asm		fadd	fGVInvZ \
__asm		fld		fDInvZScanline			/* Z V U */ \
__asm		fadd	fGInvZ \
__asm		fxch	st(2)					/* U V Z */ \
__asm		fstp	fGUInvZ \
__asm		fstp	fGVInvZ \
__asm		fst		fGInvZ \
__asm		fdivr	fOne					/* Start the next division. */ \
__asm \
__asm		jmp		SUBDIVISION_LOOP \
__asm \
__asm		/* When the sub divide equals the cached sub-divide we end up here but */ \
__asm		/* there is an element left on the fp stack. This sould never happen. */ \
__asm EXIT_BEGIN_NEXT_QUICK_END: \
__asm \
__asm		/* Dump value on stack */ \
__asm		fcomp	st(0) \
__asm		jmp		SUBDIVISION_LOOP


//
// Setup for prespective correction and looping.
// Left to right version (dir = 1).
//
// On Entry:
//	 esi = pointer to polygon.
//	 ecx = pointer to scanline.
//
// On Exit:
//	 eax = i_pixel_inner
//	 edi = inner loop count
//	 ecx=vFrac, edx=UVInt, esi=uFrac
//	 i_pixel, i_screen_index are set.
//	 perspective correction values are setup.
//
#define PERSPECTIVE_SCANLINE_PROLOGUE_CLAMP(SLTYPE) \
__asm		/* Determine the start and end of the scanline. */ \
__asm		mov		ebx,[ecx]SLTYPE.fxX.i4Fx \
__asm		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx \
__asm \
__asm		add		ebx,eax \
__asm		mov		eax,[ecx]SLTYPE.fxX.i4Fx \
__asm \
__asm		sar		ebx,16 \
__asm		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex \
__asm \
__asm		sar		eax,16 \
__asm		add		edx,ebx \
__asm \
__asm		sub		eax,ebx									/* i_pixel */ \
__asm		jge		END_OF_SCANLINE \
__asm \
__asm		mov		[i_screen_index],edx \
__asm		mov		[i_pixel],eax \
__asm \
__asm		/*--------------------------------------------------------------------------- */ \
__asm		/* Caclulate values for the first correction span, and start the divide for */ \
__asm		/* the next span. */ \
__asm		/* */ \
__asm \
__asm		/* Copy global texture values. */ \
__asm		mov		ebx,[ecx]SLTYPE.indCoord.fUInvZ \
__asm		mov		esi,[ecx]SLTYPE.indCoord.fVInvZ \
__asm \
__asm		mov		edi,[ecx]SLTYPE.indCoord.fInvZ \
__asm		mov		fGUInvZ,ebx \
__asm \
__asm		mov		fGVInvZ,esi \
__asm		mov		fGInvZ,edi \
__asm \
__asm		mov		ebx,fDUInvZEdge \
__asm		mov		esi,fDVInvZEdge \
__asm \
__asm		mov		edi,fDInvZEdge \
__asm		mov		fDUInvZScanline,ebx \
__asm \
__asm		mov		fDVInvZScanline,esi \
__asm		mov		fDInvZScanline,edi \
__asm \
__asm		mov		ebx,[iSubdivideLen] \
__asm \
__asm		/* scan line is +ve */ \
__asm		add		eax,ebx \
__asm		jg		short PARTIAL_SUBDIVIDE \
__asm \
__asm		/* Check alignment. */ \
__asm		add		edx,eax						/* i_screen_index + i_pixel */ \
__asm		add		edx,edx						/* (i_screen_index + i_pixel) * 2 */ \
__asm \
__asm		and		edx,3 \
__asm		jz		short DONE_DIVIDE_PIXEL \
__asm \
__asm		/* We are using subdivide length - 1 for alignment. */ \
__asm		mov		ebx,[fDUInvZEdgeMinusOne] \
__asm		mov		esi,[fDVInvZEdgeMinusOne] \
__asm \
__asm		mov		edi,[fDInvZEdgeMinusOne] \
__asm		mov		[fDUInvZScanline],ebx \
__asm \
__asm		mov		[fDVInvZScanline],esi \
__asm		mov		[fDInvZScanline],edi \
__asm \
__asm		dec		eax \
__asm		jmp		short DONE_DIVIDE_PIXEL \
__asm \
__asm PARTIAL_SUBDIVIDE: \
__asm		/* calc the new +ve ratio */ \
__asm		fild	[i_pixel] \
__asm		fld		fInvSubdivideLen \
__asm		fchs \
__asm \
__asm		/* Subdivision is smaller than iSubdivideLen */ \
__asm		fmulp	st(1),st(0)			/* st(0) = (-)fInvSubdivideLen * i_pixel; */ \
__asm		fld		fDUInvZScanline		/* U C */ \
__asm		xor		eax,eax \
__asm		fmul	st(0),st(1)			/* U*C C */ \
__asm		fxch	st(1)				/* C U*C */ \
__asm		fld		fDVInvZScanline		/* V C U*C */ \
__asm		fxch	st(1)				/* C V U*C */ \
__asm		fmul	st(1),st(0)			/* C V*C U*C */ \
__asm		/* stall(1) */ \
__asm		fmul	fDInvZScanline		/* Z*C V*C U*C */ \
__asm		fxch	st(2)				/* U*C V*C Z*C */ \
__asm		fstp	fDUInvZScanline		/* V*C Z*C */ \
__asm		fstp	fDVInvZScanline		/* Z*C */ \
__asm		fstp	fDInvZScanline \
__asm \
__asm DONE_DIVIDE_PIXEL: \
__asm		/* Get current u, v and z values. */ \
__asm		mov		[iNextSubdivide],eax \
__asm		mov		ebx,dword ptr[fGInvZ]		/* f_z = fInverse(fGInvZ); */ \
__asm \
__asm		mov		eax,iFI_SIGN_EXPONENT_SUB \
__asm \
__asm		sub		eax,ebx \
__asm		and		ebx,iFI_MASK_MANTISSA \
__asm \
__asm		sar		ebx,iSHIFT_MANTISSA \
__asm		and		eax,iFI_MASK_SIGN_EXPONENT \
__asm \
__asm		fld		[fGUInvZ]					/* U/Z */ \
__asm \
__asm		add		eax,dword ptr[i4InverseMantissa + ebx*4] \
__asm \
__asm		mov		dword ptr[f_z],eax \
__asm \
__asm		/*--------------------------------------------------------------------------- */ \
__asm		/* Set current texture coordinates (clamped). */ \
__asm		fmul	[f_z] \
__asm		fld		[fGVInvZ] \
__asm		fmul	[f_z] \
__asm		fxch	st(1) \
__asm		/* stall(1) */ \
__asm		fstp	[f_u] \
__asm \
__asm		/* Clamp f_u			 */ \
__asm		mov		eax,[f_u] \
__asm		mov		ebx,fTexEdgeTolerance \
__asm		mov		ecx,fTexWidth \
__asm		cmp		eax,ebx \
__asm		cmovl	(_eax,_ebx) \
__asm		cmp		eax,ecx \
__asm		cmovg	(_eax,_ecx) \
__asm		mov		ecx,fTexHeight \
__asm		mov		[f_u],eax \
__asm		fld		[f_u] \
__asm \
__asm		fadd	[dFastFixed16Conversion] \
__asm		fxch	st(1) \
__asm \
__asm		/* Clamp f_v */ \
__asm		fstp	[f_v] \
__asm		mov		eax,[f_v] \
__asm		cmp		eax,ebx \
__asm		cmovl	(_eax,_ebx) \
__asm		cmp		eax,ecx \
__asm		cmovg	(_eax,_ecx) \
__asm		mov		[f_v],eax \
__asm		fld		[f_v] \
__asm \
__asm		fadd	[dFastFixed16Conversion] \
__asm		fxch	st(1) \
__asm		fstp	[d_temp_a] \
__asm		fld		[fGUInvZ] \
__asm		fxch	st(1) \
__asm		fstp	[d_temp_b] \
__asm \
__asm		/* Increment u, v and z values. */ \
__asm		fadd	[fDUInvZScanline] \
__asm		fld		[fGVInvZ] \
__asm		fadd	[fDVInvZScanline] \
__asm		fxch	st(1) \
__asm		fld		[fGInvZ] \
__asm		fadd	[fDInvZScanline] \
__asm \
__asm		/* Setup esi=uFrac, ecx=vFrac, edx=UVInt for Abrash texture loop. */ \
__asm		mov		edx,dword ptr[d_temp_a]			/* U (16.16) */ \
__asm		mov		eax,dword ptr[d_temp_b]			/* V (16.16) */ \
__asm \
__asm		sar		eax,16							/* Integral V */ \
__asm		mov		ecx,[iTexWidth]					/* Texture width. */ \
__asm \
__asm		imul	eax,ecx							/* iv*twidth */ \
__asm \
__asm		sar		edx,16							/* Integral U */ \
__asm		mov		esi,dword ptr[d_temp_a]			/* Copy of U */ \
__asm \
__asm		shl		esi,16							/* UFrac */ \
__asm		mov		ecx,dword ptr[d_temp_b]			/* Copy of V */ \
__asm \
__asm		shl		ecx,16							/* VFrac */ \
__asm		add		edx,eax							/* iu + iv*twidth */ \
__asm \
__asm		fstp	[fGInvZ] \
__asm \
__asm		/* Get next u, v and z values. */ \
__asm		mov		ebx,dword ptr[fGInvZ]			/* f_next_z = fInverse(fGInvZ); */ \
__asm		mov		eax,iFI_SIGN_EXPONENT_SUB \
__asm \
__asm		sub		eax,ebx \
__asm		and		ebx,iFI_MASK_MANTISSA \
__asm \
__asm		sar		ebx,iSHIFT_MANTISSA \
__asm		and		eax,iFI_MASK_SIGN_EXPONENT \
__asm \
__asm		fst		[fGUInvZ] \
__asm \
__asm		mov		ebx,dword ptr[i4InverseMantissa + ebx*4] \
__asm		mov		edi,[i_pixel] \
__asm \
__asm		add		eax,ebx \
__asm		mov		ebx,[iNextSubdivide] \
__asm \
__asm		mov		dword ptr[f_next_z],eax \
__asm		sub		edi,ebx \
__asm \
__asm		/* Set new texture coordinate increments. */ \
__asm		fmul	[f_next_z] \
__asm		fxch	st(1) \
__asm		fst		[fGVInvZ] \
__asm		fmul	[f_next_z] \
__asm		fxch	st(1) \
__asm		fstp	[fU]			/* V */ \
__asm \
__asm		/* Clamp fU			 */ \
__asm		mov		eax,[fU] \
__asm		mov		ebx,fTexEdgeTolerance \
__asm		mov		ebp,fTexWidth \
__asm		cmp		eax,ebx \
__asm		cmovl	(_eax,_ebx) \
__asm		cmp		eax,ebp \
__asm		cmovg	(_eax,_ebp) \
__asm		mov		ebp,fTexHeight \
__asm		mov		[fU],eax \
__asm \
__asm		fld		[fU]							/* U,V */ \
__asm		fsub	[f_u]							/* U-fu,V */ \
__asm		fxch	st(1)							/* V,U-fu */ \
__asm		fstp	[fV]							/* U-fu */ \
__asm \
__asm		/* Clamp fV */ \
__asm		mov		eax,[fV] \
__asm		cmp		eax,ebx \
__asm		cmovl	(_eax,_ebx) \
__asm		cmp		eax,ebp \
__asm		cmovg	(_eax,_ebp) \
__asm		mov		[fV],eax \
__asm \
__asm		fld		[fV]								/* V U-fu */ \
__asm		fsub	[f_v]								/* V-fv,U-fu */ \
__asm		fxch	st(1)								/* U,V */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (C*U),V */ \
__asm		fxch	st(1)								/* V,(C*U) */ \
__asm		/* stall(1) */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (C*V),(C*U) */ \
__asm		fxch	st(1)								/* (C*U),(C*V) */ \
__asm		fadd	[dFastFixed16Conversion] \
__asm		fxch	st(1) \
__asm		/* stall(1) */ \
__asm		fadd	[dFastFixed16Conversion] \
__asm		fxch	st(1) \
__asm		/* stall(1) */ \
__asm		fstp	[d_temp_a] \
__asm		fstp	[d_temp_b] \
__asm \
__asm		mov		edi,dword ptr[d_temp_a]			/* uslope */ \
__asm		mov		eax,dword ptr[d_temp_b]			/* vslope */ \
__asm \
__asm		sar		edi,16							/* integer part of uslope */ \
__asm		mov		ebp,dword ptr[d_temp_a]			/* uslope again */ \
__asm \
__asm		shl		ebp,16							/* fractional part of uslope */ \
__asm		mov		ebx,eax							/* vslope again */ \
__asm \
__asm		sar		eax,16							/* integer part of vslope */ \
__asm		mov		[w2dDeltaTex.uUFrac],ebp		/* store UFrac */ \
__asm \
__asm		imul	eax,[iTexWidth]					/* ivslope*twidth */ \
__asm \
__asm		shl		ebx,16							/* fractional part of vslope */ \
__asm		mov		ebp,[u4TextureTileMaskStepU]	/* Load mask for integral U step */ \
__asm \
__asm		and		edi,ebp							/* Mask integral U before adding. */ \
__asm		mov		[w2dDeltaTex.uVFrac],ebx		/* store VFrac */ \
__asm \
__asm		add		edi,eax							/* ivslope*twidth + iuslope */ \
__asm		mov		ebp,[iTexWidth]					/* Load texture width. */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt+4],edi		/* store integer stride */ \
__asm		add		edi,ebp							/* add twidth to integer stride */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt],edi		/* store integer stride + twidth */ \
__asm		mov		eax,[iNextSubdivide] \
__asm \
__asm		test	eax,eax							/* Next subdivision is zero length? */ \
__asm		jz		SUBDIVISION_LOOP \
__asm \
__asm		/* iCacheNextSubdivide = iSetNextDividePixel(iNextSubdivide); */ \
__asm		mov		edi,[iSubdivideLen] \
__asm \
__asm		/* scan line is +ve */ \
__asm		add		eax,edi \
__asm		jle		short DONE_DIVIDE_PIXEL_CACHE \
__asm \
__asm		/* calc the new +ve ratio */ \
__asm		fild	iNextSubdivide \
__asm		fld		fInvSubdivideLen \
__asm		fchs \
__asm \
__asm		/* Subdivision is smaller than iSubdivideLen */ \
__asm		fmulp	st(1),st(0)			/* st(0) = (-)fInvSubdivideLen * i_pixel; */ \
__asm		fld		fDUInvZEdge			/* U C */ \
__asm \
__asm		xor		eax,eax \
__asm \
__asm		fmul	st(0),st(1)			/* U*C C */ \
__asm		fxch	st(1)				/* C U*C */ \
__asm		fld		fDVInvZEdge			/* V C U*C */ \
__asm		fxch	st(1)				/* C V U*C */ \
__asm		fmul	st(1),st(0)			/* C V*C U*C */ \
__asm \
__asm		mov		[iCacheNextSubdivide],eax \
__asm \
__asm		fmul	fDInvZEdge			/* Z*C V*C U*C */ \
__asm		fxch	st(2)				/* U*C V*C Z*C */ \
__asm		fst		fDUInvZScanline \
__asm		fadd	fGUInvZ \
__asm		fxch	st(1) \
__asm		fst		fDVInvZScanline \
__asm		fadd	fGVInvZ \
__asm		fxch	st(2) \
__asm		fst		fDInvZScanline \
__asm		fadd	fGInvZ \
__asm		fxch	st(2) \
__asm		fstp	fGVInvZ			 \
__asm		fstp	fGUInvZ \
__asm		fst		fGInvZ \
__asm		fdivr	fOne							/* Start the next division. */ \
__asm \
__asm		jmp		SUBDIVISION_LOOP \
__asm \
__asm DONE_DIVIDE_PIXEL_CACHE: \
__asm		/* Copy texture values. */ \
__asm		mov		ebp,[fDUInvZEdge] \
__asm		mov		ebx,[fDVInvZEdge] \
__asm \
__asm		mov		edi,[fDInvZEdge] \
__asm		mov		[fDUInvZScanline],ebp \
__asm \
__asm		mov		[fDVInvZScanline],ebx \
__asm		mov		[fDInvZScanline],edi \
__asm \
__asm		fld		fDUInvZScanline			/* U */ \
__asm		fadd	fGUInvZ \
__asm		fld		fDVInvZScanline			/* V U */ \
__asm		fadd	fGVInvZ \
__asm		fld		fDInvZScanline			/* Z V U */ \
__asm		fadd	fGInvZ \
__asm		fxch	st(2)					/* U V Z */ \
__asm		fstp	fGUInvZ \
__asm		fstp	fGVInvZ \
__asm		fst		fGInvZ \
__asm		fdivr	fOne							/* Start the next division. */ \
__asm \
__asm		mov		[iCacheNextSubdivide],eax \
__asm \
__asm SUBDIVISION_LOOP: \
__asm		/*--------------------------------------------------------------------------- */ \
__asm		/* Start the next subdivision. */ \
__asm		/* ecx,edx,esi = texture values */ \
__asm		/* */ \
__asm		mov		edi,[i_pixel] \
__asm		mov		eax,[iNextSubdivide] \
__asm		sub		edi,eax


//
// Do perspective correction and looping.
// Left to right version.
//
#define PERSPECTIVE_SCANLINE_EPILOGUE_CLAMP \
__asm		/*---------------------------------------------------------------------------------- */ \
__asm		/* Implement perspective correction. */ \
__asm		/* stack top contains the result of the divide */ \
__asm		/* Preserve: ecx,edx,esi */ \
__asm		/* */ \
__asm		mov		edi,[iNextSubdivide] \
__asm		mov		eax,[iCacheNextSubdivide] \
__asm \
__asm		sub		edi,eax \
__asm		jz		short EXIT_BEGIN_NEXT_QUICK_END \
__asm \
__asm		fld		[fGUInvZ]					/* U/Z,Z */ \
__asm		fxch	st(1)						/* Z,U/Z */ \
__asm		fmul	st(1),st(0)					/* Z,U */ \
__asm \
__asm		fmul	[fGVInvZ]					/* V,U */ \
__asm \
__asm		/*---------------------------------------------------------------------------------- */ \
__asm		/* Clamp U and V */ \
__asm		fxch	st(1) \
__asm		fstp	[f_u] \
__asm		fstp	[f_v] \
__asm \
__asm		mov		ebp,[f_u] \
__asm		mov		ebx,[fTexEdgeTolerance] \
__asm		mov		eax,[f_v] \
__asm		cmp		eax,ebx \
__asm		cmovl	(_eax,_ebx) \
__asm		cmp		ebp,ebx \
__asm		cmovl	(_ebp,_ebx) \
__asm		mov		ebx,[fTexWidth] \
__asm		cmp		ebp,ebx \
__asm		cmovg	(_ebp,_ebx) \
__asm		mov		ebx,[fTexHeight] \
__asm		cmp		eax,ebx \
__asm		cmovg	(_eax,_ebx) \
__asm		mov		[f_u],ebp \
__asm		mov		[f_v],eax \
__asm \
__asm		fld		[f_u] \
__asm		fld		[f_v] \
__asm \
__asm		/*---------------------------------------------------------------------------------- */ \
__asm		/* Initialize walking values */ \
__asm		fld		st(1)								/* U,V,U */ \
__asm		fsub	[fU]								/* U-fU,V,U */ \
__asm		fld		st(1)								/* V,U-fU,V,U */ \
__asm		fsub	[fV]								/* V-fV,U-fU,V,U */ \
__asm		fxch	st(1)								/* U-fU,V-fV,V,U */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (U-fU)*C,V-fV,V,U */ \
__asm		fxch	st(2)								/* V,V-fV,(U-fU)*C,U */ \
__asm		fstp	[fV]								/* V-fV,(U-fU)*C,U */ \
__asm		fmul	float ptr[fInverseIntTable+edi*4]	/* (V-fV)*C,(U-fU)*C,U */ \
__asm		fxch	st(1)								/* (U-fU)*C,(V-fV)*C,U */ \
__asm		fadd	[dFastFixed16Conversion]			/* f(U-fU)*C,(V-fV)*C,U */ \
__asm		fxch	st(1)								/* (V-fV)*C,f(U-fU)*C,U */ \
__asm		/* stall(1) */ \
__asm		fadd	[dFastFixed16Conversion]			/* f(V-fV)*C,f(U-fU)*C,U */ \
__asm		fxch	st(2)								/* U,f(U-fU)*C,f(V-fV)*C */ \
__asm		fstp	[fU]								/* f(U-fU)*C,f(V-fV)*C */ \
__asm		fstp	[d_temp_a]							/* f(V-fV)*C */ \
__asm		fstp	[d_temp_b]							 \
__asm \
__asm		mov		edi,dword ptr[d_temp_a]			/* uslope */ \
__asm		mov		eax,dword ptr[d_temp_b]			/* vslope */ \
__asm \
__asm		sar		edi,16							/* integer part of uslope */ \
__asm		mov		ebp,dword ptr[d_temp_a]			/* uslope again */ \
__asm \
__asm		shl		ebp,16							/* fractional part of uslope */ \
__asm		mov		ebx,eax							/* vslope again */ \
__asm \
__asm		sar		eax,16							/* integer part of vslope */ \
__asm		mov		[w2dDeltaTex.uUFrac],ebp		/* store UFrac */ \
__asm \
__asm		imul	eax,[iTexWidth]					/* ivslope*twidth */ \
__asm \
__asm		shl		ebx,16							/* fractional part of vslope */ \
__asm		mov		ebp,[u4TextureTileMaskStepU]	/* Load mask for integral U step */ \
__asm \
__asm		and		edi,ebp							/* Mask integral U before adding. */ \
__asm		mov		[w2dDeltaTex.uVFrac],ebx		/* store VFrac */ \
__asm \
__asm		add		edi,eax							/* ivslope*twidth + iuslope */ \
__asm		mov		ebp,[iTexWidth]					/* Load texture width. */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt+4],edi		/* store integer stride */ \
__asm		add		edi,ebp							/* add twidth to integer stride */ \
__asm \
__asm		mov		[w2dDeltaTex.iUVInt],edi		/* store integer stride + twidth */ \
__asm		mov		eax,[iCacheNextSubdivide] \
__asm \
__asm		/*---------------------------------------------------------------------------------- */ \
__asm		/* Begin Next Subdivision */ \
__asm		mov		[iNextSubdivide],eax			/* eax == iNextSubdivide */ \
__asm		mov		ebx,[iSubdivideLen] \
__asm \
__asm		test	eax,eax							/* Next subdivision is zero length? */ \
__asm		jz		SUBDIVISION_LOOP \
__asm \
__asm		/* scan line is +ve */ \
__asm		add		eax,ebx \
__asm		jle		short DONE_DIVIDE_PIXEL_CACHE_END \
__asm \
__asm		/* calc the new +ve ratio */ \
__asm		fild	iNextSubdivide \
__asm		fld		fInvSubdivideLen \
__asm		fchs \
__asm \
__asm		/* Subdivision is smaller than iSubdivideLen */ \
__asm		fmulp	st(1),st(0)			/* st(0) = (-)fInvSubdivideLen * i_pixel; */ \
__asm		fld		fDUInvZScanline		/* U C */ \
__asm \
__asm		xor		eax,eax \
__asm \
__asm		fmul	st(0),st(1)			/* U*C C */ \
__asm		fxch	st(1)				/* C U*C */ \
__asm		fld		fDVInvZScanline		/* V C U*C */ \
__asm		fxch	st(1)				/* C V U*C */ \
__asm		fmul	st(1),st(0)			/* C V*C U*C */ \
__asm \
__asm		mov		[iCacheNextSubdivide],eax \
__asm \
__asm		fmul	fDInvZScanline		/* Z*C V*C U*C */ \
__asm		fxch	st(2)				/* U*C V*C Z*C */ \
__asm		fst		fDUInvZScanline			/* U V Z */ \
__asm		fadd	fGUInvZ \
__asm		fxch	st(1)					/* V U Z */ \
__asm		fst		fDVInvZScanline \
__asm		fadd	fGVInvZ \
__asm		fxch	st(2)					/* Z U V */ \
__asm		fst		fDInvZScanline \
__asm		fadd	fGInvZ \
__asm		fxch	st(2)					/* V U Z */ \
__asm		fstp	fGVInvZ			 \
__asm		fstp	fGUInvZ \
__asm		fst		fGInvZ \
__asm		fdivr	fOne					/* Start the next division. */ \
__asm \
__asm		jmp		SUBDIVISION_LOOP \
__asm \
__asm DONE_DIVIDE_PIXEL_CACHE_END: \
__asm		mov		[iCacheNextSubdivide],eax \
__asm		mov		ebx,[iNextSubdivide] \
__asm \
__asm		cmp		eax,ebx \
__asm		je		SUBDIVISION_LOOP \
__asm \
__asm		fld		fDUInvZScanline			/* U */ \
__asm		fadd	fGUInvZ \
__asm		fld		fDVInvZScanline			/* V U */ \
__asm		fadd	fGVInvZ \
__asm		fld		fDInvZScanline			/* Z V U */ \
__asm		fadd	fGInvZ \
__asm		fxch	st(2)					/* U V Z */ \
__asm		fstp	fGUInvZ \
__asm		fstp	fGVInvZ \
__asm		fst		fGInvZ \
__asm		fdivr	fOne					/* Start the next division. */ \
__asm \
__asm		jmp		SUBDIVISION_LOOP \
__asm \
__asm		/* When the sub divide equals the cached sub-divide we end up here but */ \
__asm		/* there is an element left on the fp stack. This sould never happen. */ \
__asm EXIT_BEGIN_NEXT_QUICK_END: \
__asm \
__asm		/* Dump value on stack */ \
__asm		fcomp	st(0) \
__asm		jmp		SUBDIVISION_LOOP
