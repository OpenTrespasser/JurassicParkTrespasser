/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Assembly member functions of 'CIndexPerspective.'
 *		PENTIUM PRO Version
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P6/IndexPerspectiveTEx.hpp                    $
 * 
 * 4     9/15/97 2:06p Mmouni
 * Now ands integer U step with u4TextureTileMaskStepU before combining so that tiling works
 * without shifting.
 * 
 * 3     9/01/97 8:06p Rwyatt
 * Clamping is now controlled at run time not compile time
 * 
 * 2     8/15/97 12:53a Rwyatt
 * Optimized perspective code, BeginNextSubdivision no longer exists. Compiler switches are
 * provided to control texture U,V clamping.
 * 
 * 1     7/07/97 5:05p Rwyatt
 * Pentum Pro specific renderer optimizations
 * 
 * 1     7/07/97 5:05p Rwyatt
 * Pentium Pro specific optimizations
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
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_P6_INDEXPERSPECTIVETEX_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_P6_INDEXPERSPECTIVETEX_HPP

// Make sure this file is being included from 'IndexPerspectiveT.hpp'.
#ifndef HEADER_LIB_RENDERER_PRIMITIVES_INDEXPERSPECTIVET_HPP
	#error The header file 'IndexPerspectiveT.hpp' has not been included!
#endif


#include "AsmSupport.hpp"


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
		//const float fOne = 1.0f;
		int32 i4_temp_a;
		int32 i4_temp_b;

		// Get the next pixel index value to subdivide to.

		_asm
		{
			mov		eax, i_pixel
			cmp		[i4ScanlineDirection],0
			jle		short NEG_SCAN

			// scan line is +ve
			add		eax, iSubdivideLen				
			jle		short DONE_DIVIDE_PIXEL
			// calc the new +ve ratio

			fild	i_pixel
			xor		eax,eax
			fld		fInvSubdivideLen
			fchs
			jmp		short NEW_RATIO
NEG_SCAN:
			//scan line is -ve
			sub		eax, iSubdivideLen
			jge		short DONE_DIVIDE_PIXEL

			// calc the new -ve ratio

			fild	i_pixel
			xor		eax,eax
			fld		fInvSubdivideLen

NEW_RATIO:
			fmulp	st(1),st(0)// st(0) = fInvSubdivideLen * (-)i_pixel;
			fld		fDUInvZScanline
			fmul	st(0),st(1)
			fld		fDVInvZScanline
			fmul	st(0),st(2)
			fld		fDInvZScanline
			fmulp	st(3),st(0)
			fxch	st(1)
			fstp	fDUInvZScanline
			fstp	fDVInvZScanline
			fstp	fDInvZScanline

DONE_DIVIDE_PIXEL:
			mov		iNextSubdivide, eax

			// Get current u, v and z values.
			mov		eax, iFI_SIGN_EXPONENT_SUB
			mov		ebx, dword ptr[fGInvZ]	// f_z = fInverse(fGInvZ);

			fld		[fGVInvZ]

			sub		eax, ebx
			and		ebx, iFI_MASK_MANTISSA

			and		eax, iFI_MASK_SIGN_EXPONENT
			sar		ebx, iSHIFT_MANTISSA

			add		eax, dword ptr[i4InverseMantissa + ebx*4]
			fld		[fGUInvZ]
			mov		dword ptr[f_z], eax

			// Set current texture coordinates.
			fmul	[f_z]
			fxch	st(1)
			fmul	[f_z]
			fxch	st(1)

			// are we clamping for this polygon
			cmp		[bClampUV],0
			jnz		short CLAMP_PERSP

			// ---------------------------------------------------------------------------------
			// no clamps....
			fst		[f_u]
			fadd	[fFastFixed16Conversion]
			fxch	st(1)
			fst		[f_v]
			fadd	[fFastFixed16Conversion]
			fxch	st(1)
			fstp	float ptr[i4_temp_a]
			fstp	float ptr[i4_temp_b]

			mov		ecx, [i4_temp_a]

			mov		edx, ecx
			sar		ecx, iBITS_ACCURACY

			fld		[fGUInvZ]
			fadd	[fDUInvZScanline]

			and		ecx, 0x007FFFFF >> iBITS_ACCURACY

			mov		eax, [i4_temp_b]
			shl		edx, 32 - iBITS_ACCURACY
			sar		eax, iBITS_ACCURACY
			mov		[w2dTex]CWalk2D.uUFrac, edx
			and		eax, 0x007FFFFF >> iBITS_ACCURACY

			imul	eax,[iTexWidth]

			mov		ebx, [i4_temp_b]

			fld		[fGVInvZ]
			fadd	[fDVInvZScanline]
			fld		[fGInvZ]
			fadd	[fDInvZScanline]

			shl		ebx, 32 - iBITS_ACCURACY
			add		eax, ecx

			mov		[w2dTex]CWalk2D.uVFrac, ebx
			mov		[w2dTex + 4]CWalk2D.iUVInt, eax

			// Increment u, v and z values.

			fstp	[fGInvZ]

			// Get next u, v and z values.

			mov		ebx, dword ptr[fGInvZ]	// f_next_z = fInverse(fGInvZ);
			mov		eax, iFI_SIGN_EXPONENT_SUB

			fst		[fGVInvZ]
			fxch

			and		ebx, iFI_MASK_MANTISSA
			sub		eax, dword ptr[fGInvZ]

			fst		[fGUInvZ]

			sar		ebx, iSHIFT_MANTISSA
			and		eax, iFI_MASK_SIGN_EXPONENT

			mov		ecx, i_pixel
			add		eax, dword ptr[i4InverseMantissa + ebx*4]

			mov		dword ptr[f_next_z], eax
			sub		ecx, iNextSubdivide

			// Set new texture coordinate increments.
			fmul	[f_next_z]
			fxch	st(1)
			fmul	[f_next_z]
			fxch	st(1)

			fst		[fU]
			fsub	[f_u]
			fld		[fInverseIntTable+ecx*4]		// C,U,V
			fmul	[fFixed16Scale]
			fxch	st(2)							// V,U,C
			fst		[fV]

			fsub	[f_v]
			fxch	st(1)							// U,V,C
			fmul	st(0),st(2)						// C*U
			fxch	st(2)							// C,V, (C*U)
			fmulp	st(1),st(0)						// C*V, C*U
			fxch	st(1)							// U,V
			fistp	[i4_temp_a]
			jmp		CLAMP_DONE

			// ---------------------------------------------------------------------------------			
CLAMP_PERSP:
			// this is the perspective code when there are clamps...

			// Clamp f_u			
			fstp	[f_u]
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

			fadd	[fFastFixed16Conversion]
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

			fadd	[fFastFixed16Conversion]
			fxch	st(1)
			fstp	float ptr[i4_temp_a]
			fstp	float ptr[i4_temp_b]

			mov		ecx, [i4_temp_a]

			mov		edx, ecx
			sar		ecx, iBITS_ACCURACY

			fld		[fGUInvZ]
			fadd	[fDUInvZScanline]

			and		ecx, 0x007FFFFF >> iBITS_ACCURACY

			mov		eax, [i4_temp_b]
			shl		edx, 32 - iBITS_ACCURACY
			sar		eax, iBITS_ACCURACY
			mov		[w2dTex]CWalk2D.uUFrac, edx
			and		eax, 0x007FFFFF >> iBITS_ACCURACY

			imul	eax,[iTexWidth]

			mov		ebx, [i4_temp_b]

			fld		[fGVInvZ]
			fadd	[fDVInvZScanline]
			fld		[fGInvZ]
			fadd	[fDInvZScanline]

			shl		ebx, 32 - iBITS_ACCURACY
			add		eax, ecx

			mov		[w2dTex]CWalk2D.uVFrac, ebx
			mov		[w2dTex + 4]CWalk2D.iUVInt, eax

			// Increment u, v and z values.

			fstp	[fGInvZ]

			// Get next u, v and z values.
			mov		ebx, dword ptr[fGInvZ]	// f_next_z = fInverse(fGInvZ);
			mov		eax, iFI_SIGN_EXPONENT_SUB

			fst		[fGVInvZ]
			fxch

			and		ebx, iFI_MASK_MANTISSA
			sub		eax, dword ptr[fGInvZ]

			fst		[fGUInvZ]

			sar		ebx, iSHIFT_MANTISSA
			and		eax, iFI_MASK_SIGN_EXPONENT

			mov		ecx, i_pixel
			add		eax, dword ptr[i4InverseMantissa + ebx*4]

			mov		dword ptr[f_next_z], eax
			sub		ecx, iNextSubdivide

			// Set new texture coordinate increments.
			fmul	[f_next_z]
			fxch	st(1)
			fmul	[f_next_z]
			fxch	st(1)

			// Clamp fU			
			fstp	[fU]
			mov		eax,[fU]
			mov		ebx,fTexEdgeTolerance
			mov		edx,fTexWidth
			cmp		eax,ebx
			cmovl	(_eax,_ebx)
			cmp		eax,edx
			cmovg	(_eax,_edx)
			mov		edx,fTexHeight
			mov		[fU],eax
			fld		[fU]

			fsub	[f_u]

			fld		[fInverseIntTable+ecx*4]		// C,U,V
			fmul	[fFixed16Scale]

			fxch	st(2)							// V,U,C

			// Clamp fV
			fstp	[fV]

			mov		eax,[fV]
			cmp		eax,ebx
			cmovl	(_eax,_ebx)
			cmp		eax,edx
			cmovg	(_eax,_edx)
			mov		[fV],eax
			fld		[fV]

			fsub	[f_v]
			fxch	st(1)							// U,V,C
			fmul	st(0),st(2)						// C*U
			fxch	st(2)							// C,V, (C*U)
			fmulp	st(1),st(0)						// C*V, C*U
			fxch	st(1)							// U,V
			fistp	[i4_temp_a]


			// ---------------------------------------------------------------------------------
			// both clmaped and non-clamped primitives end up here..
CLAMP_DONE:
			mov		ecx, [i4_temp_a]

			sar		ecx, 16
			mov		edx, [i4_temp_a]

			fistp	[i4_temp_a]

			shl		edx, 16
			mov		eax, [i4_temp_a]

			sar		eax, 16
			mov		[w2dDeltaTex]CWalk2D.uUFrac, edx

			and		ecx,[u4TextureTileMaskStepU]	// Mask integral U before adding.

			imul	[iTexWidth]
			mov		ebx, [i4_temp_a]

			shl		ebx, 16
			add		eax, ecx

			mov		[w2dDeltaTex]CWalk2D.uVFrac, ebx
			mov		[w2dDeltaTex + 4]CWalk2D.iUVInt, eax

			add		eax, iTexWidth
			mov		[w2dDeltaTex]CWalk2D.iUVInt, eax

			mov		eax, iNextSubdivide
			cmp		[i4ScanlineDirection],0
			jle		short NEG_SCAN_CACHE

			// scan line is +ve
			add		eax, iSubdivideLen				
			jle		short DONE_DIVIDE_PIXEL_CACHE
			// calc the new +ve ratio

			fild	iNextSubdivide
			fld		fInvSubdivideLen
			fchs
			jmp		short NEW_RATIO_CACHE
NEG_SCAN_CACHE:
			//scan line is -ve
			sub		eax, iSubdivideLen
			jge		short DONE_DIVIDE_PIXEL_CACHE

			// calc the new -ve ratio

			fild	iNextSubdivide
			fld		fInvSubdivideLen

NEW_RATIO_CACHE:
			xor		eax,eax
			fmulp	st(1),st(0)// st(0) = fInvSubdivideLen * (-)i_pixel;
			fld		fDUInvZScanline
			fmul	st(0),st(1)
			fld		fDVInvZScanline
			fmul	st(0),st(2)
			fld		fDInvZScanline
			fmulp	st(3),st(0)
			fxch	st(1)

			mov		iCacheNextSubdivide, eax

			cmp		eax, iNextSubdivide
			jne		short INCREMENT_STORE

			// just store the new scan line values and exit.
			fstp	fDUInvZScanline
			fstp	fDVInvZScanline
			fstp	fDInvZScanline
			jmp		EXIT_BEGIN_NEXT

			
DONE_DIVIDE_PIXEL_CACHE:
			mov		iCacheNextSubdivide, eax

			cmp		eax, iNextSubdivide
			je		short EXIT_BEGIN_NEXT

			fld		fDInvZScanline
			fadd	fGInvZ

			fld		fDVInvZScanline
			fadd	fGVInvZ

			fld		fDUInvZScanline
			fadd	fGUInvZ
			fxch	st(1)
			fstp	fGVInvZ
			fstp	fGUInvZ
			fst		fGInvZ

			fld		fOne

			fdivrp	st(1),st(0)					// Start the next division.
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

			fld		fOne

			fdivrp	st(1),st(0)					// Start the next division.
EXIT_BEGIN_NEXT:
		}
	}



	//*****************************************************************************************
	inline void CIndexPerspective::EndNextSubdivision()
	{
		int32	i4_temp;
		float	f_u,f_v;

		_asm
		{
			// stack top contains the result of the divide
			mov		edi,[iNextSubdivide]
			fld		st(0)
			sub		edi,[iCacheNextSubdivide]
			jz		short EXIT_BEGIN_NEXT_QUICK
			fmul	[fGUInvZ]
			fxch	st(1)						// v,u,H,W,T
			fmul	[fGVInvZ]

			cmp		[bClampUV],0
			jz		short NO_CLAMP

			//----------------------------------------------------------------------------------
			// Clamp
			fxch	st(1)
			fstp	[f_u]
			mov		esi,[f_u]
			mov		ebx,fTexEdgeTolerance
			fstp	[f_v]
			mov		eax,[f_v]
			cmp		eax,ebx
			cmovl	(_eax,_ebx)
			cmp		esi,ebx
			cmovl	(_esi,_ebx)
			mov		ebx,fTexWidth
			cmp		esi,ebx
			cmovg	(_esi,_ebx)
			mov		ebx,fTexHeight
			cmp		eax,ebx
			cmovg	(_eax,_ebx)
			mov		[f_u],esi
			mov		[f_v],eax

			fld		[f_u]
			fld		[f_v]

NO_CLAMP:
			//----------------------------------------------------------------------------------
			// Initialize walking values
			fld		[fInverseIntTable+edi*4]
			fmul	DWORD PTR[fFixed16Scale]			// C,V,U
			fxch	st(1)								// V,C,U
			fld		st(0)								// V,V,C,U
			fsub	[fV]								// V-fV,V,C,U
			fxch	st(3)								// U,V,C,V-fV
			fld		st(0)								// U,U,V,C,V-fV
			fsub	[fU]								// U-fU,U,V,C,V-fV
			fxch	st(3)								// C,U,V,U-fU,V-fV
			fmul	st(4),st(0)							// C,U,V,U-fU,(V-fV)*C
			fxch	st(1)								// U,C,V,U-fU,(V-fV)*C						
			fstp	[fU]								// C,V,U-fU,(V-fV)*C						
			fmulp	st(2),st(0)							// V,(U-fU)*C,(V-fV)*C

			fstp	[fV]								// (U-fU)*C,(V-fV)*C
			fistp	[i4_temp]							// (V-fV)*C

			mov		ecx, [i4_temp]
			mov		edx, ecx
			sar		ecx, 16

			fistp	[i4_temp]

			mov		eax, [i4_temp]
			shl		edx, 16
			mov		ebx, eax
			sar		eax, 16
			mov		w2dDeltaTex.uUFrac, edx

			and		ecx,[u4TextureTileMaskStepU]	// Mask integral U before adding.

			imul	eax,iTexWidth

			shl		ebx, 16
			add		eax, ecx

			mov		w2dDeltaTex.uVFrac, ebx
			mov		w2dDeltaTex.iUVInt+4, eax


			add		eax, iTexWidth
			mov		edx,iCacheNextSubdivide
			mov		w2dDeltaTex.iUVInt, eax
			mov		iNextSubdivide, edx


			//----------------------------------------------------------------------------------
			// Begin Next Subdivision
			mov		eax, iNextSubdivide
			cmp		[i4ScanlineDirection],0
			jle		short NEG_SCAN_CACHE

			// scan line is +ve
			add		eax, iSubdivideLen				
			jle		short DONE_DIVIDE_PIXEL_CACHE
			// calc the new +ve ratio

			fild	iNextSubdivide
			fld		fInvSubdivideLen
			fchs
			jmp		short NEW_RATIO_CACHE
NEG_SCAN_CACHE:
			//scan line is -ve
			sub		eax, iSubdivideLen
			jge		short DONE_DIVIDE_PIXEL_CACHE

			// calc the new -ve ratio

			fild	iNextSubdivide
			fld		fInvSubdivideLen

NEW_RATIO_CACHE:
			xor		eax,eax
			fmulp	st(1),st(0)// st(0) = fInvSubdivideLen * (-)i_pixel;
			fld		fDUInvZScanline
			fmul	st(0),st(1)
			fld		fDVInvZScanline
			fmul	st(0),st(2)
			fld		fDInvZScanline
			fmulp	st(3),st(0)
			fxch	st(1)

			mov		iCacheNextSubdivide, eax

			cmp		eax, iNextSubdivide
			jne		short INCREMENT_STORE

			// just store the new scan line values and exit.
			fstp	fDUInvZScanline
			fstp	fDVInvZScanline
			fstp	fDInvZScanline
			jmp		EXIT_BEGIN_NEXT

			
DONE_DIVIDE_PIXEL_CACHE:
			mov		iCacheNextSubdivide, eax

			cmp		eax, iNextSubdivide
			je		short EXIT_BEGIN_NEXT

			fld		fDInvZScanline
			fadd	fGInvZ

			fld		fDVInvZScanline
			fadd	fGVInvZ

			fld		fDUInvZScanline
			fadd	fGUInvZ
			fxch	st(1)
			fstp	fGVInvZ
			fstp	fGUInvZ
			fst		fGInvZ

			fld		fOne

			fdivrp	st(1),st(0)					// Start the next division.
			jmp		short EXIT_BEGIN_NEXT

			// when the sub divide equals the cahced sub-divide we end up here but there
			// if 2 elements on the FP stack.
EXIT_BEGIN_NEXT_QUICK:
			fstp	[i4_temp]
			fstp	[i4_temp]
			jmp		short	EXIT_BEGIN_NEXT


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

			fld		fOne

			fdivrp	st(1),st(0)					// Start the next division.
EXIT_BEGIN_NEXT:

		}
	}


#endif
