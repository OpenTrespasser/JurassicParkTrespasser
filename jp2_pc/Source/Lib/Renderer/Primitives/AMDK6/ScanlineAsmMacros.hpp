/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Macros for scanline prologue and epilogue.
 *		Optimized for the AMD K6-3D Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/ScanlineAsmMacros.hpp                   $
 * 
 * 3     97.11.20 4:22p Mmouni
 * Got rid of Pentium version, added clamp always version of perspective prologue and epilogue.
 * 
 * 
 * 2     97.10.30 1:38p Mmouni
 * Added 3DX versions of perspecive scanline prologue and epilogue.
 * 
 * 1     97.10.27 1:24p Mmouni
 * Initial K6-3D version (same as the pentium for now)
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
// 3DX version.
//
// On Entry:
//	 esi = pointer to polygon.
//	 ecx = pointer to scanline.
//	 mm0 = (V/Z,U/Z)
//	 mm1 = (?,1/Z)
//
// On Exit:
//	 edx = Base of current scanline
//	 esi = Base of texture map
//	 edi = Scanline offset and count
//	 mm7 = Packed fixed (16.16) U,V
//   mm6 = Packed fixed (16.16) U,V slopes
//
#define PERSPECTIVE_SCANLINE_PROLOGUE_3DX(SLTYPE) \
__asm		/* */ \
__asm		/* Determine the start and end of the scanline.  */ \
__asm		/* */ \
__asm		mov		ebx,[ecx]SLTYPE.fxX.i4Fx \
__asm \
__asm		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx \
__asm \
__asm		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex \
__asm		add		ebx,eax \
__asm \
__asm		mov		eax,[ecx]SLTYPE.fxX.i4Fx \
__asm		sar		ebx,16 \
__asm \
__asm		sar		eax,16 \
__asm		add		edx,ebx \
__asm \
__asm		sub		eax,ebx									/* eax == i_pixel */ \
__asm		jge		END_OF_SCANLINE \
__asm \
__asm		/* Load span increments. */ \
__asm		movq	mm2,[tvDEdge.UInvZ]						/* Load V,U */ \
__asm		mov		[i_screen_index],edx					/* Save scanline index. */ \
__asm	 \
__asm		movd	mm3,[tvDEdge.InvZ]						/* Load Z */ \
__asm		mov		[i_pixel],eax							/* Save negative length of scanline. */ \
__asm \
__asm		mov		edi,[iSubdivideLen] \
__asm \
__asm		/* scan line is +ve  */ \
__asm		add		eax,edi									/* eax = i_pixel + SubdivideLen */ \
__asm		jle		short DONE_DIVIDE_PIXEL \
__asm \
__asm		/* */ \
__asm		/* Subdivision is smaller than iSubdivideLen  */ \
__asm		/* */ \
__asm		/* Adjust span increments by -i_pixel * fInvSubdivideLen  */ \
__asm		/* */ \
__asm		mov		edi,[i_pixel]					/* Integer width of span. */ \
__asm		xor		eax,eax							/* Remaining width. */ \
__asm \
__asm		neg		edi \
__asm		movd	mm4,edi							/* -i_pixel */ \
__asm \
__asm		pi2fd	(m4,m4)							/* Convert to fp. */ \
__asm		movd	mm5,[fInvSubdivideLen]			/* 1/SubDivideLen */ \
__asm \
__asm		pfmul	(m4,m5)							/* -i_pixel * fInvSubdivideLen  */ \
__asm \
__asm		punpckldq	mm4,mm4 \
__asm \
__asm		pfmul	(m2,m4)							/* Adjust V,U increments. */ \
__asm \
__asm		pfmul	(m3,m4)							/* Adjust Z increment. */ \
__asm \
__asm DONE_DIVIDE_PIXEL: \
__asm		/* */ \
__asm		/* Compute current U,V */ \
__asm		/* Step fGUInvZ,fGVInvZ,fGInvZ */ \
__asm		/* */ \
__asm		pfrcp	(m4,m1)							/* f_z = mm4 = 1/fGInvZ */ \
__asm		mov		[iNextSubdivide],eax			/* Save length remaining. */ \
__asm \
__asm		pfadd	(m3,m1)							/* Step fGInvZ */ \
__asm		mov		edx,[bClampUV]					/* Load clamp flag. */ \
__asm \
__asm		pfadd	(m2,m0)							/* Step fGUInvZ,fGVInvZ */ \
__asm		movd	[tvCurUVZ.InvZ],mm3				/* Save updated 1/Z */ \
__asm \
__asm		pfmul	(m4,m0)							/* mm4 = V1,U1 */ \
__asm		pfrcp	(m0,m3)							/* f_next_z = mm0 = 1/fGInvZ */ \
__asm \
__asm		movq	[tvCurUVZ.UInvZ],mm2					/* Save updated U/Z,V/Z */ \
__asm		movd	mm1,float ptr[fInverseIntTable+edi*4]	/* Reciprocal of span width. */ \
__asm \
__asm		pfmul	(m0,m2)							/* mm0 = V2,U2 */ \
__asm		movq	mm2,[pfFixed16Scale]			/* Load fixed point scale factors. */ \
__asm \
__asm		cmp		edx,0 \
__asm		je		short CLAMP_DONE \
__asm \
__asm		movq	mm3,[pfTexEdgeTolerance] \
__asm		movq	mm5,[cvMaxCoords] \
__asm		pfmax	(m0,m3)							/* Clamp U1,V1 to >= fTexEdgeTolerance */ \
__asm		pfmin	(m0,m5)							/* Clamp U1 < fTexWidth, V1 < fTexHeight */ \
__asm		pfmax	(m4,m3)							/* Clamp U1,V1 to >= fTexEdgeTolerance */ \
__asm		pfmin	(m4,m5)							/* Clamp U1 < fTexWidth, V1 < fTexHeight */ \
__asm \
__asm CLAMP_DONE: \
__asm		punpckldq	mm1,mm1						/* Duplicate reciprocal of span width. */ \
__asm		movq		[pfCurUV],mm0				/* Save ending U,V */ \
__asm \
__asm		pfsub	(m0,m4)							/* mm0 = V2-V1,U2-U1 */ \
__asm		pfmul	(m4,m2)							/* Scale to starting U,V 16.16 */ \
__asm \
__asm		mov		edi,[i_pixel]					/* edi = current i_pixel */ \
__asm		mov		[i_pixel],eax					/* Save updated i_pixel. */ \
__asm \
__asm		pfmul	(m0,m1)							/* DV*1/Width,DU*1/Width */ \
__asm		pf2id	(m7,m4)							/* Starting V,U in mm7 */ \
__asm \
__asm		sub		edi,eax							/* edi = inner loop count */ \
__asm		mov		ebx,[i_screen_index]			/* Load scanline offset. */ \
__asm \
__asm		pfmul	(m0,m2)							/* Scale U,V slope to 16.16 */ \
__asm		mov		edx,gsGlobals.pvScreen			/* Pointer the screen. */ \
__asm \
__asm		mov		esi,[pvTextureBitmap]			/* Load texture base pointer. */ \
__asm		add		eax,ebx							/* Add scanline offset to i_pixel */ \
__asm \
__asm		pf2id	(m6,m0)							/* VStep,UStep in mm6 */ \
__asm		lea		edx,[edx+eax*2]					/* Base of span in edx. */


//
// Do perspective correction and looping for next span.
// 3DX version.
//
// On Entry:
//	 eax = i_pixel
//	 mm7 = Packed fixed (16.16) U,V
//
// On Exit:
//	 edx = Base of current scanline
//	 esi = Base of texture map
//	 edi = Scanline offset and count
//	 mm7 = Packed fixed (16.16) U,V
//   mm6 = Packed fixed (16.16) U,V slopes
//
#define PERSPECTIVE_SCANLINE_EPILOGUE_3DX \
__asm		mov		edi,[iSubdivideLen] \
__asm \
__asm		/* Load last texture values. */ \
__asm		movq	mm0,[tvCurUVZ.UInvZ]						/* mm0 = (V/Z,U/Z) */ \
__asm \
__asm		movd	mm1,[tvCurUVZ.InvZ]							/* mm1 = (?,1/Z) */ \
__asm \
__asm		/* Load span increments. */ \
__asm		movq	mm2,[tvDEdge.UInvZ]							/* V,U */ \
__asm	 \
__asm		movd	mm3,[tvDEdge.InvZ]							/* Z */ \
__asm \
__asm		/* scan line is +ve  */ \
__asm		add		eax,edi										/* eax = i_pixel + SubdivideLen */ \
__asm		jle		short DONE_DIVIDE_PIXEL_END \
__asm \
__asm		/* */ \
__asm		/* Subdivision is smaller than iSubdivideLen  */ \
__asm		/* */ \
__asm		/* Adjust span increments by -i_pixel * fInvSubdivideLen  */ \
__asm		/* */ \
__asm		mov		edi,[i_pixel]					/* Integer width of span. */ \
__asm		xor		eax,eax							/* Remaining width. */ \
__asm \
__asm		neg		edi \
__asm		movd	mm4,edi							/* -i_pixel */ \
__asm \
__asm		pi2fd	(m4,m4)							/* Convert to fp. */ \
__asm		movd	mm5,[fInvSubdivideLen]			/* 1/SubDivideLen */ \
__asm \
__asm		pfmul	(m4,m5)							/* -i_pixel * fInvSubdivideLen  */ \
__asm \
__asm		punpckldq	mm4,mm4 \
__asm \
__asm		pfmul	(m2,m4)							/* Adjust V,U increments. */ \
__asm \
__asm		pfmul	(m3,m4)							/* Adjust Z increment. */ \
__asm \
__asm DONE_DIVIDE_PIXEL_END: \
__asm		/* */ \
__asm		/* Compute current U,V */ \
__asm		/* Step fGUInvZ,fGVInvZ,fGInvZ */ \
__asm		/* */ \
__asm		pfadd	(m3,m1)							/* Step fGInvZ */ \
__asm		mov		[iNextSubdivide],eax			/* Save length remaining. */ \
__asm \
__asm		pfadd	(m2,m0)							/* Step fGUInvZ,fGVInvZ */ \
__asm		movd	[tvCurUVZ.InvZ],mm3				/* Save updated fGInvZ */ \
__asm \
__asm		pfrcp	(m0,m3)							/* f_z = mm0 = 1/fGInvZ */ \
__asm		movq	mm4,[pfCurUV]					/* Load last U,V */ \
__asm \
__asm		movq	[tvCurUVZ.UInvZ],mm2					/* Save updated fGUInvZ,fGVInvZ */ \
__asm		movd	mm1,float ptr[fInverseIntTable+edi*4]	/* Reciprocal of span width. */ \
__asm \
__asm		pfmul	(m0,m2)							/* mm0 = V2,U2 */ \
__asm		movq	mm2,[pfFixed16Scale]			/* Load fixed point scale factors. */ \
__asm \
__asm		cmp		[bClampUV],0 \
__asm		je		short CLAMP_DONE_END \
__asm \
__asm		movq	mm3,[pfTexEdgeTolerance] \
__asm		movq	mm5,[cvMaxCoords] \
__asm		pfmax	(m0,m3)							/* Clamp U1,V1 to >= fTexEdgeTolerance */ \
__asm		pfmin	(m0,m5)							/* Clamp U1 < fTexWidth, V1 < fTexHeight */ \
__asm \
__asm CLAMP_DONE_END: \
__asm		movq		[pfCurUV],mm0				/* Save current U,V */ \
__asm		punpckldq	mm1,mm1						/* Duplicate across entire register. */ \
__asm \
__asm		pfsub	(m0,m4)							/* V2-V1,U2-U1 */ \
__asm		mov		edi,[i_pixel]					/* edi = current i_pixel */ \
__asm \
__asm		mov		[i_pixel],eax					/* Save updated i_pixel. */ \
__asm		mov		edx,gsGlobals.pvScreen			/* Pointer the screen. */ \
__asm \
__asm		pfmul	(m0,m1)							/* DV*1/Width,DU*1/Width */ \
__asm		mov		ebx,[i_screen_index]			/* Load scanline offset. */ \
__asm \
__asm		sub		edi,eax							/* edi = inner loop count */ \
__asm		mov		esi,[pvTextureBitmap]			/* Load texture base pointer. */ \
__asm \
__asm		pfmul	(m0,m2)							/* Scale to 16.16 */ \
__asm		add		eax,ebx							/* Add scanline offset to i_pixel */ \
__asm \
__asm		lea		edx,[edx+eax*2]					/* Base of span in edx. */ \
__asm		pf2id	(m6,m0)							/* VStep,UStep in mm6 */


//
// Setup for prespective correction and looping.
// 3DX version.
//
// On Entry:
//	 esi = pointer to polygon.
//	 ecx = pointer to scanline.
//	 mm0 = (V/Z,U/Z)
//	 mm1 = (?,1/Z)
//
// On Exit:
//	 edx = Base of current scanline
//	 esi = Base of texture map
//	 edi = Scanline offset and count
//	 mm7 = Packed fixed (16.16) U,V
//   mm6 = Packed fixed (16.16) U,V slopes
//
#define PERSPECTIVE_SCANLINE_PROLOGUE_CLAMP_3DX(SLTYPE) \
__asm		/* */ \
__asm		/* Determine the start and end of the scanline.  */ \
__asm		/* */ \
__asm		mov		ebx,[ecx]SLTYPE.fxX.i4Fx \
__asm \
__asm		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx \
__asm \
__asm		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex \
__asm		add		ebx,eax \
__asm \
__asm		mov		eax,[ecx]SLTYPE.fxX.i4Fx \
__asm		sar		ebx,16 \
__asm \
__asm		sar		eax,16 \
__asm		add		edx,ebx \
__asm \
__asm		sub		eax,ebx									/* eax == i_pixel */ \
__asm		jge		END_OF_SCANLINE \
__asm \
__asm		/* Load span increments. */ \
__asm		movq	mm2,[tvDEdge.UInvZ]						/* Load V,U */ \
__asm		mov		[i_screen_index],edx					/* Save scanline index. */ \
__asm	 \
__asm		movd	mm3,[tvDEdge.InvZ]						/* Load Z */ \
__asm		mov		[i_pixel],eax							/* Save negative length of scanline. */ \
__asm \
__asm		mov		edi,[iSubdivideLen] \
__asm \
__asm		/* scan line is +ve  */ \
__asm		add		eax,edi									/* eax = i_pixel + SubdivideLen */ \
__asm		jle		short DONE_DIVIDE_PIXEL \
__asm \
__asm		/* */ \
__asm		/* Subdivision is smaller than iSubdivideLen  */ \
__asm		/* */ \
__asm		/* Adjust span increments by -i_pixel * fInvSubdivideLen  */ \
__asm		/* */ \
__asm		mov		edi,[i_pixel]					/* Integer width of span. */ \
__asm		xor		eax,eax							/* Remaining width. */ \
__asm \
__asm		neg		edi \
__asm		movd	mm4,edi							/* -i_pixel */ \
__asm \
__asm		pi2fd	(m4,m4)							/* Convert to fp. */ \
__asm		movd	mm5,[fInvSubdivideLen]			/* 1/SubDivideLen */ \
__asm \
__asm		pfmul	(m4,m5)							/* -i_pixel * fInvSubdivideLen  */ \
__asm \
__asm		punpckldq	mm4,mm4 \
__asm \
__asm		pfmul	(m2,m4)							/* Adjust V,U increments. */ \
__asm \
__asm		pfmul	(m3,m4)							/* Adjust Z increment. */ \
__asm \
__asm DONE_DIVIDE_PIXEL: \
__asm		/* */ \
__asm		/* Compute current U,V */ \
__asm		/* Step fGUInvZ,fGVInvZ,fGInvZ */ \
__asm		/* */ \
__asm		pfrcp	(m4,m1)							/* f_z = mm4 = 1/fGInvZ */ \
__asm		mov		[iNextSubdivide],eax			/* Save length remaining. */ \
__asm \
__asm		pfadd	(m3,m1)							/* Step fGInvZ */ \
__asm \
__asm		pfadd	(m2,m0)							/* Step fGUInvZ,fGVInvZ */ \
__asm		movd	[tvCurUVZ.InvZ],mm3				/* Save updated 1/Z */ \
__asm \
__asm		pfmul	(m4,m0)							/* mm4 = V1,U1 */ \
__asm		pfrcp	(m0,m3)							/* f_next_z = mm0 = 1/fGInvZ */ \
__asm \
__asm		movq	[tvCurUVZ.UInvZ],mm2					/* Save updated U/Z,V/Z */ \
__asm		movd	mm1,float ptr[fInverseIntTable+edi*4]	/* Reciprocal of span width. */ \
__asm \
__asm		pfmul	(m0,m2)							/* mm0 = V2,U2 */ \
__asm		movq	mm2,[pfFixed16Scale]			/* Load fixed point scale factors. */ \
__asm \
__asm		movq	mm3,[pfTexEdgeTolerance] \
__asm		movq	mm5,[cvMaxCoords] \
__asm		pfmax	(m0,m3)							/* Clamp U1,V1 to >= fTexEdgeTolerance */ \
__asm		pfmin	(m0,m5)							/* Clamp U1 < fTexWidth, V1 < fTexHeight */ \
__asm		pfmax	(m4,m3)							/* Clamp U1,V1 to >= fTexEdgeTolerance */ \
__asm		pfmin	(m4,m5)							/* Clamp U1 < fTexWidth, V1 < fTexHeight */ \
__asm \
__asm		punpckldq	mm1,mm1						/* Duplicate reciprocal of span width. */ \
__asm		movq		[pfCurUV],mm0				/* Save ending U,V */ \
__asm \
__asm		pfsub	(m0,m4)							/* mm0 = V2-V1,U2-U1 */ \
__asm		pfmul	(m4,m2)							/* Scale to starting U,V 16.16 */ \
__asm \
__asm		mov		edi,[i_pixel]					/* edi = current i_pixel */ \
__asm		mov		[i_pixel],eax					/* Save updated i_pixel. */ \
__asm \
__asm		pfmul	(m0,m1)							/* DV*1/Width,DU*1/Width */ \
__asm		pf2id	(m7,m4)							/* Starting V,U in mm7 */ \
__asm \
__asm		sub		edi,eax							/* edi = inner loop count */ \
__asm		mov		ebx,[i_screen_index]			/* Load scanline offset. */ \
__asm \
__asm		pfmul	(m0,m2)							/* Scale U,V slope to 16.16 */ \
__asm		mov		edx,gsGlobals.pvScreen			/* Pointer the screen. */ \
__asm \
__asm		mov		esi,[pvTextureBitmap]			/* Load texture base pointer. */ \
__asm		add		eax,ebx							/* Add scanline offset to i_pixel */ \
__asm \
__asm		pf2id	(m6,m0)							/* VStep,UStep in mm6 */ \
__asm		lea		edx,[edx+eax*2]					/* Base of span in edx. */


//
// Do perspective correction and looping for next span.
// 3DX version.
//
// On Entry:
//	 eax = i_pixel
//	 mm7 = Packed fixed (16.16) U,V
//
// On Exit:
//	 edx = Base of current scanline
//	 esi = Base of texture map
//	 edi = Scanline offset and count
//	 mm7 = Packed fixed (16.16) U,V
//   mm6 = Packed fixed (16.16) U,V slopes
//
#define PERSPECTIVE_SCANLINE_EPILOGUE_CLAMP_3DX \
__asm		mov		edi,[iSubdivideLen] \
__asm \
__asm		/* Load last texture values. */ \
__asm		movq	mm0,[tvCurUVZ.UInvZ]						/* mm0 = (V/Z,U/Z) */ \
__asm \
__asm		movd	mm1,[tvCurUVZ.InvZ]							/* mm1 = (?,1/Z) */ \
__asm \
__asm		/* Load span increments. */ \
__asm		movq	mm2,[tvDEdge.UInvZ]							/* V,U */ \
__asm	 \
__asm		movd	mm3,[tvDEdge.InvZ]							/* Z */ \
__asm \
__asm		/* scan line is +ve  */ \
__asm		add		eax,edi										/* eax = i_pixel + SubdivideLen */ \
__asm		jle		short DONE_DIVIDE_PIXEL_END \
__asm \
__asm		/* */ \
__asm		/* Subdivision is smaller than iSubdivideLen  */ \
__asm		/* */ \
__asm		/* Adjust span increments by -i_pixel * fInvSubdivideLen  */ \
__asm		/* */ \
__asm		mov		edi,[i_pixel]					/* Integer width of span. */ \
__asm		xor		eax,eax							/* Remaining width. */ \
__asm \
__asm		neg		edi \
__asm		movd	mm4,edi							/* -i_pixel */ \
__asm \
__asm		pi2fd	(m4,m4)							/* Convert to fp. */ \
__asm		movd	mm5,[fInvSubdivideLen]			/* 1/SubDivideLen */ \
__asm \
__asm		pfmul	(m4,m5)							/* -i_pixel * fInvSubdivideLen  */ \
__asm \
__asm		punpckldq	mm4,mm4 \
__asm \
__asm		pfmul	(m2,m4)							/* Adjust V,U increments. */ \
__asm \
__asm		pfmul	(m3,m4)							/* Adjust Z increment. */ \
__asm \
__asm DONE_DIVIDE_PIXEL_END: \
__asm		/* */ \
__asm		/* Compute current U,V */ \
__asm		/* Step fGUInvZ,fGVInvZ,fGInvZ */ \
__asm		/* */ \
__asm		pfadd	(m3,m1)							/* Step fGInvZ */ \
__asm		mov		[iNextSubdivide],eax			/* Save length remaining. */ \
__asm \
__asm		pfadd	(m2,m0)							/* Step fGUInvZ,fGVInvZ */ \
__asm		movd	[tvCurUVZ.InvZ],mm3				/* Save updated fGInvZ */ \
__asm \
__asm		pfrcp	(m0,m3)							/* f_z = mm0 = 1/fGInvZ */ \
__asm		movq	mm4,[pfCurUV]					/* Load last U,V */ \
__asm \
__asm		movq	[tvCurUVZ.UInvZ],mm2					/* Save updated fGUInvZ,fGVInvZ */ \
__asm		movd	mm1,float ptr[fInverseIntTable+edi*4]	/* Reciprocal of span width. */ \
__asm \
__asm		pfmul	(m0,m2)							/* mm0 = V2,U2 */ \
__asm		movq	mm2,[pfFixed16Scale]			/* Load fixed point scale factors. */ \
__asm \
__asm		movq	mm3,[pfTexEdgeTolerance] \
__asm		movq	mm5,[cvMaxCoords] \
__asm		pfmax	(m0,m3)							/* Clamp U1,V1 to >= fTexEdgeTolerance */ \
__asm		pfmin	(m0,m5)							/* Clamp U1 < fTexWidth, V1 < fTexHeight */ \
__asm \
__asm		movq		[pfCurUV],mm0				/* Save current U,V */ \
__asm		punpckldq	mm1,mm1						/* Duplicate across entire register. */ \
__asm \
__asm		pfsub	(m0,m4)							/* V2-V1,U2-U1 */ \
__asm		mov		edi,[i_pixel]					/* edi = current i_pixel */ \
__asm \
__asm		mov		[i_pixel],eax					/* Save updated i_pixel. */ \
__asm		mov		edx,gsGlobals.pvScreen			/* Pointer the screen. */ \
__asm \
__asm		pfmul	(m0,m1)							/* DV*1/Width,DU*1/Width */ \
__asm		mov		ebx,[i_screen_index]			/* Load scanline offset. */ \
__asm \
__asm		sub		edi,eax							/* edi = inner loop count */ \
__asm		mov		esi,[pvTextureBitmap]			/* Load texture base pointer. */ \
__asm \
__asm		pfmul	(m0,m2)							/* Scale to 16.16 */ \
__asm		add		eax,ebx							/* Add scanline offset to i_pixel */ \
__asm \
__asm		lea		edx,[edx+eax*2]					/* Base of span in edx. */ \
__asm		pf2id	(m6,m0)							/* VStep,UStep in mm6 */
