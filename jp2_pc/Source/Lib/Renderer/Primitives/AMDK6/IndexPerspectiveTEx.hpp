/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Assembly member functions of 'CIndexPerspective.'
 *		Optimized for the AMD K6-3D Processor
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/IndexPerspectiveTEx.hpp                 $
 * 
 * 3     9/05/98 12:18p Asouth
 * removed shorts from long jumps
 * 
 * 2     97.10.27 1:25p Mmouni
 * Initial K6-3D version, copied from the Pentium versions.
 * 
 * 9     9/15/97 2:09p Mmouni
 * Optimized and fixed for tiling.
 * 
 * 8     9/01/97 8:04p Rwyatt
 * Clamps are based on the P6 clamps, with the P6 specific instructions removed
 * 
 * 7     8/17/97 9:45p Rwyatt
 * Temporary clamps for UV co-ords....To be removed later
 * 
 * 6     8/15/97 12:57a Rwyatt
 * Removed local constants and used the global ones.
 * 
 * 5     97/06/17 1:06a Pkeet
 * Made the begin subdivision asm based on previous version.
 * 
 * 4     6/17/97 12:37a Rwyatt
 * removed some unnecessary register clears.
 * 
 * 3     6/17/97 12:36a Rwyatt
 * Optimized InitializeSubdivision by re-ordering instructions and placing all C in-line so this
 * is a single function.
 * 
 * 2     1/15/97 1:07p Pkeet
 * Initializing the walk 2d structure for a new subdivision now uses an assembly function.
 * 
 * 1     1/09/97 4:41p Pkeet
 * Initial implementation. Assembly code moved here from 'IndexPerspectiveTEx.hpp.'
 * 
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_P5_INDEXPERSPECTIVETEX_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_P5_INDEXPERSPECTIVETEX_HPP

// Make sure this file is being included from 'IndexPerspectiveT.hpp'.
#ifndef HEADER_LIB_RENDERER_PRIMITIVES_INDEXPERSPECTIVET_HPP
	#error The header file 'IndexPerspectiveT.hpp' has not been included!
#endif


// These need to be 64 bit aligned for speed.
extern double d_temp_a;
extern double d_temp_b;


//
// Class implementations.
//
	//*****************************************************************************************
	inline void CIndexPerspective::InitializeSubdivision(int i_pixel)
	{
		float f_u;
		float f_v;
		float f_z;
		float f_next_z;

		// Get the next pixel index value to subdivide to.
		_asm
		{
			mov		eax,[i_pixel]
			mov		ebx,[iSubdivideLen]

			cmp		[i4ScanlineDirection],0
			jle		short NEG_SCAN

			// scan line is +ve
			add		eax,ebx
			jle		short DONE_DIVIDE_PIXEL

			// calc the new +ve ratio
			fild	[i_pixel]
			fld		fInvSubdivideLen
			fchs
			jmp		short NEW_RATIO
NEG_SCAN:
			// scan line is -ve
			sub		eax,ebx
			jge		short DONE_DIVIDE_PIXEL

			// calc the new -ve ratio
			fild	[i_pixel]
			fld		fInvSubdivideLen
			// stall(1)

NEW_RATIO:
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

			// Are we clamping for this polygon
			test	ebx,ebx
			jz		NO_CLAMP

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

			// Setup uFrac, UVInt for Abrash texture loop.
			mov		ecx,dword ptr[d_temp_a]			// U (16.16)
			mov		eax,dword ptr[d_temp_b]			// V (16.16)
	
			sar		eax,16							// Integral V
			mov		edx,ecx							// Copy of U

			shl		edx,16							// UFrac
			mov		ebx,dword ptr[d_temp_b]			// Copy of V

			sar		ecx,16							// Integral U
			mov		[w2dTex]CWalk2D.uUFrac,edx

			fstp	[fGInvZ]

			imul	eax,[iTexWidth]					// iv*twidth

			shl		ebx,16							// VFrac
			add		eax,ecx							// iv*twidth + iu

			mov		[w2dTex]CWalk2D.uVFrac,ebx
			mov		[w2dTex + 4]CWalk2D.iUVInt,eax

			// Get next u, v and z values.
			mov		ebx,dword ptr[fGInvZ]			// f_next_z = fInverse(fGInvZ);
			mov		eax,iFI_SIGN_EXPONENT_SUB

			sub		eax,ebx
			and		ebx,iFI_MASK_MANTISSA

			sar		ebx,iSHIFT_MANTISSA
			and		eax,iFI_MASK_SIGN_EXPONENT

			fst		[fGUInvZ]

			mov		ebx,dword ptr[i4InverseMantissa + ebx*4]
			mov		ecx,[i_pixel]

			add		eax,ebx
			mov		ebx,[iNextSubdivide]

			mov		dword ptr[f_next_z],eax
			sub		ecx,ebx

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
			mov		edx,fTexWidth
			cmp		eax,ebx
			jge		short U_NOT_LESS2
			mov		eax,ebx
U_NOT_LESS2:
			cmp		eax,edx
			jle		short U_NOT_GREATER2
			mov		eax,edx
U_NOT_GREATER2:
			mov		edx,fTexHeight
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
			cmp		eax,edx
			jle		short V_NOT_GREATER2
			mov		eax,edx
V_NOT_GREATER2:
			mov		[fV],eax

			fld		[fV]							// V U-fu

			jmp		CLAMP_DONE

NO_CLAMP:
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

			// Setup uFrac, UVInt for Abrash texture loop.
			mov		ecx,dword ptr[d_temp_a]		// U (16.16)
			mov		eax,dword ptr[d_temp_b]		// V (16.16)
	
			sar		eax,16							// Integral V
			mov		edx,ecx							// Copy of U

			shl		edx,16							// UFrac
			mov		ebx,dword ptr[d_temp_b]			// Copy of V

			sar		ecx,16							// Integral U
			mov		[w2dTex]CWalk2D.uUFrac,edx

			fstp	[fGInvZ]

			imul	eax,[iTexWidth]					// iv*twidth

			shl		ebx,16							// VFrac
			add		eax,ecx							// iv*twidth + iu

			mov		[w2dTex]CWalk2D.uVFrac,ebx
			mov		[w2dTex + 4]CWalk2D.iUVInt,eax

			// Get next u, v and z values.
			mov		ebx,dword ptr[fGInvZ]			// f_next_z = fInverse(fGInvZ);
			mov		eax,iFI_SIGN_EXPONENT_SUB

			sub		eax,ebx
			and		ebx,iFI_MASK_MANTISSA

			sar		ebx,iSHIFT_MANTISSA
			and		eax,iFI_MASK_SIGN_EXPONENT

			fst		[fGUInvZ]

			mov		ebx,dword ptr[i4InverseMantissa + ebx*4]
			mov		ecx,[i_pixel]

			add		eax,ebx
			mov		ebx,[iNextSubdivide]

			mov		dword ptr[f_next_z],eax
			sub		ecx,ebx

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

			// ---------------------------------------------------------------------------------
			// Both clmaped and non-clamped primitives end up here..
CLAMP_DONE:
			fsub	[f_v]								// V-fv,U-fu
			fxch	st(1)								// U,V
			fmul	float ptr[fInverseIntTable+ecx*4]	// (C*U),V
			fxch	st(1)								// V,(C*U)
			// stall(1)
			fmul	float ptr[fInverseIntTable+ecx*4]	// (C*V),(C*U)
			fxch	st(1)								// (C*U),(C*V)
			fadd	[dFastFixed16Conversion]
			fxch	st(1)
			// stall(1)
			fadd	[dFastFixed16Conversion]
			fxch	st(1)
			// stall(1)
			fstp	[d_temp_a]
			fstp	[d_temp_b]

			mov		ecx,dword ptr[d_temp_a]			// uslope
			mov		eax,dword ptr[d_temp_b]			// vslope

			sar		ecx,16							// integer part of uslope
			mov		edx,dword ptr[d_temp_a]			// uslope again

			shl		edx,16							// fractional part of uslope
			mov		ebx,eax							// vslope again

			sar		eax,16							// integer part of vslope
			mov		[w2dDeltaTex.uUFrac],edx		// store UFrac

			imul	eax,[iTexWidth]					// ivslope*twidth

			shl		ebx,16							// fractional part of vslope
			mov		edx,[u4TextureTileMaskStepU]	// Load mask for integral U step

			and		ecx,edx							// Mask integral U before adding.
			mov		[w2dDeltaTex.uVFrac],ebx		// store VFrac

			add		ecx,eax							// ivslope*twidth + iuslope
			mov		edx,[iTexWidth]					// Load texture width.

			mov		[w2dDeltaTex.iUVInt+4],ecx		// store integer stride
			add		ecx,edx							// add twidth to integer stride

			mov		[w2dDeltaTex.iUVInt],ecx		// store integer stride + twidth
			mov		eax,[iNextSubdivide]

			//iCacheNextSubdivide = iSetNextDividePixel(iNextSubdivide);
			mov		ecx,[iSubdivideLen]
			mov		edx,[i4ScanlineDirection]
			
			cmp		edx,0
			jle		short NEG_SCAN_CACHE

			// scan line is +ve
			add		eax,ecx
			jle		short DONE_DIVIDE_PIXEL_CACHE

			// calc the new +ve ratio
			fild	iNextSubdivide
			fld		fInvSubdivideLen
			fchs
			jmp		short NEW_RATIO_CACHE

NEG_SCAN_CACHE:
			// scan line is -ve
			sub		eax,ecx
			jge		short DONE_DIVIDE_PIXEL_CACHE

			// calc the new -ve ratio
			fild	iNextSubdivide
			fld		fInvSubdivideLen
			// stall(1)

NEW_RATIO_CACHE:
			fmulp	st(1),st(0)			// st(0) = (-)fInvSubdivideLen * i_pixel;
			fld		fDUInvZScanline		// U C
			xor		eax,eax
			fmul	st(0),st(1)			// U*C C
			fxch	st(1)				// C U*C
			fld		fDVInvZScanline		// V C U*C
			fxch	st(1)				// C V U*C
			fmul	st(1),st(0)			// C V*C U*C

			mov		[iCacheNextSubdivide],eax
			mov		ebx,[iNextSubdivide]

			fmul	fDInvZScanline		// Z*C V*C U*C
			fxch	st(2)				// U*C V*C Z*C

			cmp		eax,ebx
			jne		short INCREMENT_STORE

			// just store the new scan line values and exit.
			fstp	fDUInvZScanline
			fstp	fDVInvZScanline
			fstp	fDInvZScanline
			jmp		EXIT_BEGIN_NEXT
			
DONE_DIVIDE_PIXEL_CACHE:
			mov		[iCacheNextSubdivide],eax
			mov		ebx,[iNextSubdivide]

			cmp		eax,ebx
			je		EXIT_BEGIN_NEXT

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

			fdivr	fOne						// Start the next division.

			jmp		short EXIT_BEGIN_NEXT

INCREMENT_STORE:
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

			fdivr	fOne						// Start the next division.

EXIT_BEGIN_NEXT:
		}

	}


	//*****************************************************************************************
	inline void CIndexPerspective::EndNextSubdivision()
	{
		float f_u, f_v;

		_asm
		{
			// stack top contains the result of the divide
			mov		edi,[iNextSubdivide]
			mov		eax,[iCacheNextSubdivide]

			sub		edi,eax
			jz		EXIT_BEGIN_NEXT_QUICK

			fld		[fGUInvZ]					// U/Z,Z
			fxch	st(1)						// Z,U/Z
			fmul	st(1),st(0)					// Z,U
			mov		ebx,[bClampUV]
			fmul	[fGVInvZ]					// V,U

			// Are we clamping for this polygon?
			test	ebx,ebx
			jz		short CLAMP_DONE

			//----------------------------------------------------------------------------------
			// Clamp U and V
			fxch	st(1)
			fstp	[f_u]
			fstp	[f_v]

			mov		esi,[f_u]
			mov		ebx,[fTexEdgeTolerance]
			mov		eax,[f_v]
			cmp		eax,ebx
			jge		short V_NOT_LESS
			mov		eax,ebx
V_NOT_LESS:
			cmp		esi,ebx
			jge		short U_NOT_LESS
			mov		esi,ebx
U_NOT_LESS:
			mov		ebx,[fTexWidth]
			cmp		esi,ebx
			jle		short U_NOT_GREATER
			mov		esi,ebx
U_NOT_GREATER:
			mov		ebx,[fTexHeight]
			cmp		eax,ebx
			jle		short V_NOT_GREATER
			mov		eax,ebx
V_NOT_GREATER:
			mov		[f_u],esi
			mov		[f_v],eax

			fld		[f_u]
			fld		[f_v]

CLAMP_DONE:
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

			mov		ecx,dword ptr[d_temp_a]			// uslope
			mov		eax,dword ptr[d_temp_b]			// vslope

			sar		ecx,16							// integer part of uslope
			mov		edx,dword ptr[d_temp_a]			// uslope again

			shl		edx,16							// fractional part of uslope
			mov		ebx,eax							// vslope again

			sar		eax,16							// integer part of vslope
			mov		[w2dDeltaTex.uUFrac],edx		// store UFrac

			imul	eax,[iTexWidth]					// ivslope*twidth

			shl		ebx,16							// fractional part of vslope
			mov		edx,[u4TextureTileMaskStepU]	// Load mask for integral U step

			and		ecx,edx							// Mask integral U before adding.
			mov		[w2dDeltaTex.uVFrac],ebx		// store VFrac

			add		ecx,eax							// ivslope*twidth + iuslope
			mov		edx,[iTexWidth]					// Load texture width.

			mov		[w2dDeltaTex.iUVInt+4],ecx		// store integer stride
			add		ecx,edx							// add twidth to integer stride

			mov		[w2dDeltaTex.iUVInt],ecx		// store integer stride + twidth
			mov		eax,[iCacheNextSubdivide]

			//----------------------------------------------------------------------------------
			// Begin Next Subdivision
			mov		[iNextSubdivide],eax			// eax == iNextSubdivide
			mov		edx,[i4ScanlineDirection]

			cmp		edx,0
			jle		short NEG_SCAN_CACHE

			// scan line is +ve
			add		eax,[iSubdivideLen]
			jle		short DONE_DIVIDE_PIXEL_CACHE

			// calc the new +ve ratio
			fild	iNextSubdivide
			fld		fInvSubdivideLen
			fchs
			jmp		short NEW_RATIO_CACHE

NEG_SCAN_CACHE:
			// scan line is -ve
			sub		eax,[iSubdivideLen]
			jge		short DONE_DIVIDE_PIXEL_CACHE

			// calc the new -ve ratio
			fild	iNextSubdivide
			fld		fInvSubdivideLen
			// stall(1)

NEW_RATIO_CACHE:
			fmulp	st(1),st(0)			// st(0) = (-)fInvSubdivideLen * i_pixel;
			fld		fDUInvZScanline		// U C
			xor		eax,eax
			fmul	st(0),st(1)			// U*C C
			fxch	st(1)				// C U*C
			fld		fDVInvZScanline		// V C U*C
			fxch	st(1)				// C V U*C
			fmul	st(1),st(0)			// C V*C U*C

			mov		[iCacheNextSubdivide],eax
			mov		ebx,[iNextSubdivide]

			fmul	fDInvZScanline		// Z*C V*C U*C
			fxch	st(2)				// U*C V*C Z*C

			cmp		eax,ebx
			jne		short INCREMENT_STORE

			// just store the new scan line values and exit.
			fstp	fDUInvZScanline
			fstp	fDVInvZScanline
			fstp	fDInvZScanline
			jmp		EXIT_BEGIN_NEXT

DONE_DIVIDE_PIXEL_CACHE:
			mov		[iCacheNextSubdivide],eax
			mov		ebx,[iNextSubdivide]

			cmp		eax,ebx
			je		EXIT_BEGIN_NEXT

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

			jmp		short EXIT_BEGIN_NEXT

			// When the sub divide equals the cached sub-divide we end up here but
			// there is an element left on the fp stack.
EXIT_BEGIN_NEXT_QUICK:
			
			// Dump value on stack
			ffree	st(0)
			fincstp

			jmp		short EXIT_BEGIN_NEXT

INCREMENT_STORE:
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

			fdivr	fOne						// Start the next division.

EXIT_BEGIN_NEXT:
		}
	}

#endif
