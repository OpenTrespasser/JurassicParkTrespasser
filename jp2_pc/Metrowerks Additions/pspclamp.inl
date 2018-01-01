		/* Determine the start and end of the scanline. */ 
		mov		ebx,[ecx]SLTYPE.fxX.i4Fx 
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx 
 
		add		ebx,eax 
		mov		eax,[ecx]SLTYPE.fxX.i4Fx 
 
		sar		ebx,16 
		mov		edx,[esi]CDrawPolygonBase.iLineStartIndex 
 
		sar		eax,16 
		add		edx,ebx 
 
		sub		eax,ebx									/* i_pixel */ 
		jge		END_OF_SCANLINE 
 
		mov		[i_screen_index],edx 
		mov		[i_pixel],eax 
 
		/*--------------------------------------------------------------------------- */ 
		/* Caclulate values for the first correction span, and start the divide for */ 
		/* the next span. */ 
		/* */ 
 
		/* Copy global texture values. */ 
		mov		ebx,[ecx]SLTYPE.indCoord.fUInvZ 
		mov		esi,[ecx]SLTYPE.indCoord.fVInvZ 
 
		mov		edi,[ecx]SLTYPE.indCoord.fInvZ 
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
 
		/* scan line is +ve */ 
		add		eax,ebx 
		jg		short PARTIAL_SUBDIVIDE 
 
		/* Check alignment. */ 
		add		edx,eax						/* i_screen_index + i_pixel */ 
		add		edx,edx						/* (i_screen_index + i_pixel) * 2 */ 
 
		and		edx,3 
		jz		short DONE_DIVIDE_PIXEL 
 
		/* We are using subdivide length - 1 for alignment. */ 
		mov		ebx,[fDUInvZEdgeMinusOne] 
		mov		esi,[fDVInvZEdgeMinusOne] 
 
		mov		edi,[fDInvZEdgeMinusOne] 
		mov		[fDUInvZScanline],ebx 
 
		mov		[fDVInvZScanline],esi 
		mov		[fDInvZScanline],edi 
 
		dec		eax 
		jmp		short DONE_DIVIDE_PIXEL 
 
 PARTIAL_SUBDIVIDE: 
		/* calc the new +ve ratio */ 
		fild	[i_pixel] 
		fld		fInvSubdivideLen 
		fchs 
 
		/* Subdivision is smaller than iSubdivideLen */ 
		fmulp	st(1),st(0)			/* st(0) = (-)fInvSubdivideLen * i_pixel; */ 
		fld		fDUInvZScanline		/* U C */ 
		xor		eax,eax 
		fmul	st(0),st(1)			/* U*C C */ 
		fxch	st(1)				/* C U*C */ 
		fld		fDVInvZScanline		/* V C U*C */ 
		fxch	st(1)				/* C V U*C */ 
		fmul	st(1),st(0)			/* C V*C U*C */ 
		/* stall(1) */ 
		fmul	fDInvZScanline		/* Z*C V*C U*C */ 
		fxch	st(2)				/* U*C V*C Z*C */ 
		fstp	fDUInvZScanline		/* V*C Z*C */ 
		fstp	fDVInvZScanline		/* Z*C */ 
		fstp	fDInvZScanline 
 
 DONE_DIVIDE_PIXEL: 
		/* Get current u, v and z values. */ 
		mov		[iNextSubdivide],eax 
		mov		ebx,dword ptr[fGInvZ]		/* f_z = fInverse(fGInvZ); */ 
 
		mov		eax,iFI_SIGN_EXPONENT_SUB 
 
		sub		eax,ebx 
		and		ebx,iFI_MASK_MANTISSA 
 
		sar		ebx,iSHIFT_MANTISSA 
		and		eax,iFI_MASK_SIGN_EXPONENT 
 
		fld		[fGUInvZ]					/* U/Z */ 
 
		add		eax,dword ptr[i4InverseMantissa + ebx*4] 
 
		mov		dword ptr[f_z],eax 
 
		/*--------------------------------------------------------------------------- */ 
		/* Set current texture coordinates (clamped). */ 
		fmul	[f_z] 
		fld		[fGVInvZ] 
		fmul	[f_z] 
		fxch	st(1) 
		/* stall(1) */ 
		fstp	[f_u] 
 
		/* Clamp f_u			 */ 
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
 
		/* Clamp f_v */ 
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
 
		/* Increment u, v and z values. */ 
		fadd	[fDUInvZScanline] 
		fld		[fGVInvZ] 
		fadd	[fDVInvZScanline] 
		fxch	st(1) 
		fld		[fGInvZ] 
		fadd	[fDInvZScanline] 
 
		/* Setup esi=uFrac, ecx=vFrac, edx=UVInt for Abrash texture loop. */ 
		mov		edx,dword ptr[d_temp_a]			/* U (16.16) */ 
		mov		eax,dword ptr[d_temp_b]			/* V (16.16) */ 
 
		sar		eax,16							/* Integral V */ 
		mov		ecx,[iTexWidth]					/* Texture width. */ 
 
		imul	eax,ecx							/* iv*twidth */ 
 
		sar		edx,16							/* Integral U */ 
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
		fmul	[f_next_z] 
		fxch	st(1) 
		fst		[fGVInvZ] 
		fmul	[f_next_z] 
		fxch	st(1) 
		fstp	[fU]			/* V */ 
 
		/* Clamp fU			 */ 
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
 
		fld		[fU]							/* U,V */ 
		fsub	[f_u]							/* U-fu,V */ 
		fxch	st(1)							/* V,U-fu */ 
		fstp	[fV]							/* U-fu */ 
 
		/* Clamp fV */ 
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
 
		fld		[fV]								/* V U-fu */ 
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
 
		sar		edi,16							/* integer part of uslope */ 
		mov		ebp,dword ptr[d_temp_a]			/* uslope again */ 
 
		shl		ebp,16							/* fractional part of uslope */ 
		mov		ebx,eax							/* vslope again */ 
 
		sar		eax,16							/* integer part of vslope */ 
		mov		[w2dDeltaTex.uUFrac],ebp		/* store UFrac */ 
 
		imul	eax,[iTexWidth]					/* ivslope*twidth */ 
 
		shl		ebx,16							/* fractional part of vslope */ 
		mov		ebp,[u4TextureTileMaskStepU]	/* Load mask for integral U step */ 
 
		and		edi,ebp							/* Mask integral U before adding. */ 
		mov		[w2dDeltaTex.uVFrac],ebx		/* store VFrac */ 
 
		add		edi,eax							/* ivslope*twidth + iuslope */ 
		mov		ebp,[iTexWidth]					/* Load texture width. */ 
 
		mov		[w2dDeltaTex.iUVInt+4],edi		/* store integer stride */ 
		add		edi,ebp							/* add twidth to integer stride */ 
 
		mov		[w2dDeltaTex.iUVInt],edi		/* store integer stride + twidth */ 
		mov		eax,[iNextSubdivide] 
 
		test	eax,eax							/* Next subdivision is zero length? */ 
		jz		SUBDIVISION_LOOP 
 
		/* iCacheNextSubdivide = iSetNextDividePixel(iNextSubdivide); */ 
		mov		edi,[iSubdivideLen] 
 
		/* scan line is +ve */ 
		add		eax,edi 
		jle		short DONE_DIVIDE_PIXEL_CACHE 
 
		/* calc the new +ve ratio */ 
		fild	iNextSubdivide 
		fld		fInvSubdivideLen 
		fchs 
 
		/* Subdivision is smaller than iSubdivideLen */ 
		fmulp	st(1),st(0)			/* st(0) = (-)fInvSubdivideLen * i_pixel; */ 
		fld		fDUInvZEdge			/* U C */ 
 
		xor		eax,eax 
 
		fmul	st(0),st(1)			/* U*C C */ 
		fxch	st(1)				/* C U*C */ 
		fld		fDVInvZEdge			/* V C U*C */ 
		fxch	st(1)				/* C V U*C */ 
		fmul	st(1),st(0)			/* C V*C U*C */ 
 
		mov		[iCacheNextSubdivide],eax 
 
		fmul	fDInvZEdge			/* Z*C V*C U*C */ 
		fxch	st(2)				/* U*C V*C Z*C */ 
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
		fdivr	fOne							/* Start the next division. */ 
 
		jmp		SUBDIVISION_LOOP 
 
 DONE_DIVIDE_PIXEL_CACHE: 
		/* Copy texture values. */ 
		mov		ebp,[fDUInvZEdge] 
		mov		ebx,[fDVInvZEdge] 
 
		mov		edi,[fDInvZEdge] 
		mov		[fDUInvZScanline],ebp 
 
		mov		[fDVInvZScanline],ebx 
		mov		[fDInvZScanline],edi 
 
		fld		fDUInvZScanline			/* U */ 
		fadd	fGUInvZ 
		fld		fDVInvZScanline			/* V U */ 
		fadd	fGVInvZ 
		fld		fDInvZScanline			/* Z V U */ 
		fadd	fGInvZ 
		fxch	st(2)					/* U V Z */ 
		fstp	fGUInvZ 
		fstp	fGVInvZ 
		fst		fGInvZ 
		fdivr	fOne							/* Start the next division. */ 
 
		mov		[iCacheNextSubdivide],eax 
 
 SUBDIVISION_LOOP: 
		/*--------------------------------------------------------------------------- */ 
		/* Start the next subdivision. */ 
		/* ecx,edx,esi = texture values */ 
		/* */ 
		mov		edi,[i_pixel] 
		mov		eax,[iNextSubdivide] 
		sub		edi,eax
