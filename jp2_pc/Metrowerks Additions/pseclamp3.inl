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
// #define PERSPECTIVE_SCANLINE_EPILOGUE_CLAMP_3DX
	mov		edi,[iSubdivideLen]

	/* Load last texture values. */
	movq	mm0,[tvCurUVZ.UInvZ]						/* mm0 = (V/Z,U/Z) */

	movd	mm1,[tvCurUVZ.InvZ]							/* mm1 = (?,1/Z) */

	/* Load span increments. */
	movq	mm2,[tvDEdge.UInvZ]							/* V,U */

	movd	mm3,[tvDEdge.InvZ]							/* Z */

	/* scan line is +ve  */
	add		eax,edi										/* eax = i_pixel + SubdivideLen */
	jle		short DONE_DIVIDE_PIXEL_END

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

DONE_DIVIDE_PIXEL_END:
	/* */
	/* Compute current U,V */
	/* Step fGUInvZ,fGVInvZ,fGInvZ */
	/* */
	pfadd	(m3,m1)							/* Step fGInvZ */
	mov		[iNextSubdivide],eax			/* Save length remaining. */

	pfadd	(m2,m0)							/* Step fGUInvZ,fGVInvZ */
	movd	[tvCurUVZ.InvZ],mm3				/* Save updated fGInvZ */

	pfrcp	(m0,m3)							/* f_z = mm0 = 1/fGInvZ */
	movq	mm4,[pfCurUV]					/* Load last U,V */

	movq	[tvCurUVZ.UInvZ],mm2					/* Save updated fGUInvZ,fGVInvZ */
	movd	mm1,float ptr[fInverseIntTable+edi*4]	/* Reciprocal of span width. */

	pfmul	(m0,m2)							/* mm0 = V2,U2 */
	movq	mm2,[pfFixed16Scale]			/* Load fixed point scale factors. */

	movq	mm3,[pfTexEdgeTolerance]
	movq	mm5,[cvMaxCoords]
	pfmax	(m0,m3)							/* Clamp U1,V1 to >= fTexEdgeTolerance */
	pfmin	(m0,m5)							/* Clamp U1 < fTexWidth, V1 < fTexHeight */

	movq		[pfCurUV],mm0				/* Save current U,V */
	punpckldq	mm1,mm1						/* Duplicate across entire register. */

	pfsub	(m0,m4)							/* V2-V1,U2-U1 */
	mov		edi,[i_pixel]					/* edi = current i_pixel */

	mov		[i_pixel],eax					/* Save updated i_pixel. */
	mov		edx,gsGlobals.pvScreen			/* Pointer the screen. */

	pfmul	(m0,m1)							/* DV*1/Width,DU*1/Width */
	mov		ebx,[i_screen_index]			/* Load scanline offset. */

	sub		edi,eax							/* edi = inner loop count */
	mov		esi,[pvTextureBitmap]			/* Load texture base pointer. */

	pfmul	(m0,m2)							/* Scale to 16.16 */
	add		eax,ebx							/* Add scanline offset to i_pixel */

	lea		edx,[edx+eax*2]					/* Base of span in edx. */
	pf2id	(m6,m0)							/* VStep,UStep in mm6 */
