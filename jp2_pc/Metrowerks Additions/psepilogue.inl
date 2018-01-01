// PERSPECTIVE_SCANLINE_EPILOGUE
		/*---------------------------------------------------------------------------------- */ 
		/* Implement perspective correction. */ 
		/* stack top contains the result of the divide */ 
		/* Preserve: ecx,edx,esi */ 
		/* */ 
		mov		edi,[iNextSubdivide] 
		mov		eax,[iCacheNextSubdivide] 
 
		sub		edi,eax 
		jz		EXIT_BEGIN_NEXT_QUICK_END 
 
		fld		[fGUInvZ]					/* U/Z,Z */ 
		fxch	st(1)						/* Z,U/Z */ 
		fmul	st(1),st(0)					/* Z,U */ 
		mov		ebx,[bClampUV] 
		fmul	[fGVInvZ]					/* V,U */ 
 
		/* Are we clamping for this polygon? */ 
		test	ebx,ebx 
		jz		NO_CLAMP_END 
 
		/*---------------------------------------------------------------------------------- */ 
		/* Clamp U and V */ 
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
 
		sar		edi,16							/* integer part of uslope */ 
		mov		ebp,dword ptr[d_temp_a]			/* uslope again */ 
 
		shl		ebp,16							/* fractional part of uslope */ 
		mov		ebx,eax							/* vslope again */ 
 
		sar		eax,16							/* integer part of vslope */ 
		mov		[w2dDeltaTex.uUFrac],ebp		/* store UFrac */ 
 
		shl		eax,9							/* ivslope*twidth */ 
 
		shl		ebx,16							/* fractional part of vslope */ 
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
 
		/*---------------------------------------------------------------------------------- */ 
		/* Begin Next Subdivision */ 
		mov		[iNextSubdivide],eax			/* eax == iNextSubdivide */ 
		mov		ebx,[iSubdivideLen] 
 
		test	eax,eax							/* Next subdivision is zero length? */ 
		jz		SUBDIVISION_LOOP 
 
		/* scan line is +ve */ 
		add		eax,ebx 
		jle		short DONE_DIVIDE_PIXEL_CACHE_END 
 
		/* calc the new +ve ratio */ 
		fild	iNextSubdivide 
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
 
		mov		[iCacheNextSubdivide],eax 
 
		fmul	fDInvZScanline		/* Z*C V*C U*C */ 
		fxch	st(2)				/* U*C V*C Z*C */ 
		fst		fDUInvZScanline			/* U V Z */ 
		fadd	fGUInvZ 
		fxch	st(1)					/* V U Z */ 
		fst		fDVInvZScanline 
		fadd	fGVInvZ 
		fxch	st(2)					/* Z U V */ 
		fst		fDInvZScanline 
		fadd	fGInvZ 
		fxch	st(2)					/* V U Z */ 
		fstp	fGVInvZ			 
		fstp	fGUInvZ 
		fst		fGInvZ 
		fdivr	fOne					/* Start the next division. */ 
 
		jmp		SUBDIVISION_LOOP 
 
 DONE_DIVIDE_PIXEL_CACHE_END: 
		mov		[iCacheNextSubdivide],eax 
		mov		ebx,[iNextSubdivide] 
 
		cmp		eax,ebx 
		je		SUBDIVISION_LOOP 
 
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
		fdivr	fOne					/* Start the next division. */ 
 
		jmp		SUBDIVISION_LOOP 
 
		/* When the sub divide equals the cached sub-divide we end up here but */ 
		/* there is an element left on the fp stack. This sould never happen. */ 
 EXIT_BEGIN_NEXT_QUICK_END: 
 
		/* Dump value on stack */ 
		fcomp	st(0) 
		jmp		SUBDIVISION_LOOP
