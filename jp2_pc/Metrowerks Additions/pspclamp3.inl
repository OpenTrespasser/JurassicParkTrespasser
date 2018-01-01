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
// #define PERSPECTIVE_SCANLINE_PROLOGUE_CLAMP_3DX(SLTYPE)
	/* */
	/* Determine the start and end of the scanline.  */
	/* */
	mov		ebx,[ecx]SLTYPE.fxX.i4Fx

	mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx

	mov		edx,[esi]CDrawPolygonBase.iLineStartIndex
	add		ebx,eax

	mov		eax,[ecx]SLTYPE.fxX.i4Fx
	sar		ebx,16

	sar		eax,16
	add		edx,ebx

	sub		eax,ebx									/* eax == i_pixel */
	jge		END_OF_SCANLINE

	/* Load span increments. */
	movq	mm2,[tvDEdge.UInvZ]						/* Load V,U */
	mov		[i_screen_index],edx					/* Save scanline index. */

	movd	mm3,[tvDEdge.InvZ]						/* Load Z */
	mov		[i_pixel],eax							/* Save negative length of scanline. */

	mov		edi,[iSubdivideLen]

	/* scan line is +ve  */
	add		eax,edi									/* eax = i_pixel + SubdivideLen */
	jle		short DONE_DIVIDE_PIXEL

	/* */
	/* Subdivision is smaller than iSubdivideLen  */
	/* */
	/* Adjust span increments by -i_pixel * fInvSubdivideLen  */
	/* */
	mov		edi,[i_pixel]					/* Integer width of span. */
	xor		eax,eax							/* Remaining width. */

	neg		edi
	movd	mm4,edi							/* -i_pixel */

	pi2fd	(m4,m4)							/* Convert to fp. */
	movd	mm5,[fInvSubdivideLen]			/* 1/SubDivideLen */

	pfmul	(m4,m5)							/* -i_pixel * fInvSubdivideLen  */

	punpckldq	mm4,mm4

	pfmul	(m2,m4)							/* Adjust V,U increments. */

	pfmul	(m3,m4)							/* Adjust Z increment. */

DONE_DIVIDE_PIXEL:
	/* */
	/* Compute current U,V */
	/* Step fGUInvZ,fGVInvZ,fGInvZ */
	/* */
	pfrcp	(m4,m1)							/* f_z = mm4 = 1/fGInvZ */
	mov		[iNextSubdivide],eax			/* Save length remaining. */

	pfadd	(m3,m1)							/* Step fGInvZ */

	pfadd	(m2,m0)							/* Step fGUInvZ,fGVInvZ */
	movd	[tvCurUVZ.InvZ],mm3				/* Save updated 1/Z */

	pfmul	(m4,m0)							/* mm4 = V1,U1 */
	pfrcp	(m0,m3)							/* f_next_z = mm0 = 1/fGInvZ */

	movq	[tvCurUVZ.UInvZ],mm2					/* Save updated U/Z,V/Z */
	movd	mm1,float ptr[fInverseIntTable+edi*4]	/* Reciprocal of span width. */

	pfmul	(m0,m2)							/* mm0 = V2,U2 */
	movq	mm2,[pfFixed16Scale]			/* Load fixed point scale factors. */

	movq	mm3,[pfTexEdgeTolerance]
	movq	mm5,[cvMaxCoords]
	pfmax	(m0,m3)							/* Clamp U1,V1 to >= fTexEdgeTolerance */
	pfmin	(m0,m5)							/* Clamp U1 < fTexWidth, V1 < fTexHeight */
	pfmax	(m4,m3)							/* Clamp U1,V1 to >= fTexEdgeTolerance */
	pfmin	(m4,m5)							/* Clamp U1 < fTexWidth, V1 < fTexHeight */

	punpckldq	mm1,mm1						/* Duplicate reciprocal of span width. */
	movq		[pfCurUV],mm0				/* Save ending U,V */

	pfsub	(m0,m4)							/* mm0 = V2-V1,U2-U1 */
	pfmul	(m4,m2)							/* Scale to starting U,V 16.16 */

	mov		edi,[i_pixel]					/* edi = current i_pixel */
	mov		[i_pixel],eax					/* Save updated i_pixel. */

	pfmul	(m0,m1)							/* DV*1/Width,DU*1/Width */
	pf2id	(m7,m4)							/* Starting V,U in mm7 */

	sub		edi,eax							/* edi = inner loop count */
	mov		ebx,[i_screen_index]			/* Load scanline offset. */

	pfmul	(m0,m2)							/* Scale U,V slope to 16.16 */
	mov		edx,gsGlobals.pvScreen			/* Pointer the screen. */

	mov		esi,[pvTextureBitmap]			/* Load texture base pointer. */
	add		eax,ebx							/* Add scanline offset to i_pixel */

	pf2id	(m6,m0)							/* VStep,UStep in mm6 */
	lea		edx,[edx+eax*2]					/* Base of span in edx. */


