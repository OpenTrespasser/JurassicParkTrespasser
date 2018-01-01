/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Overloaded functions for "Walk.hpp" optimized for the Pentium.
 *
 * Bugs:
 *
 * To do:
 *		Changed the name of the module to 'TexWalkEx.hpp.' Make functions member functions of
 *		CWalk1D and CWalk2D.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P5/WalkEx.hpp                                 $
 * 
 * 8     9/01/97 8:04p Rwyatt
 * 
 * 7     8/15/97 2:33a Rwyatt
 * Fixed local variable problem
 * 
 * 6     8/15/97 12:58a Rwyatt
 * Removed local floating point constants and used the global ones.
 * 
 * 5     1/15/97 1:02p Pkeet
 * Moved the assembly versions of the 2d initializing functions to WalkEx.hpp. Removed fog
 * parameters.
 * 
 * 4     8/12/96 7:15p Pkeet
 * Added comments.
 * 
 * 3     7/10/96 1:48p Pkeet
 * Added additional comments.
 * 
 * 2     7/09/96 6:38p Pkeet
 * Added a '+=' operator for 'CWalk1D.'
 * 
 * 1     7/09/96 6:00p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_P5_WALKEX_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_P5_WALKEX_HPP

// Make sure this file is being included from 'FixedP.hpp'.
#ifndef HEADER_LIB_RENDERER_PRIMITIVES_WALK_HPP
	#error The header file Walk.hpp has not been included!
#endif


//
// Global functions.
//

//*********************************************************************************************
//
inline void operator+=(CWalk1D& w1d_a, const CWalk1D& w1d_b)
//
// Adds two CWalk1D parameters together and stores the result in the first parameter.
//
// Notes:
//		The assembly version is equivalent to the following 'C' code:
//
//		   w1d_a.bfx.i4Int += w1d_b.bfx.i4Int;
//		   if (iAddCarry(w1d_a.bfx.u4Frac, w1d_a.bfx.u4Frac, w1d_b.bfx.u4Frac))
//				w1d_a.bfx.i4Int += w1d_a.iOffsetPerLine;
//
//**************************************
{
	//
	// Register usage:
	//
	//	 Register    Variable
	//
	//		ebx		w1d_a.bfx.u4Frac
	//		ecx		w1d_b.bfx.i4Int
	//		edi		w1d_a
	//		esi		w1d_b
	//
	__asm
	{
		// Load variables into the registers.
		mov edi, w1d_a
		mov esi, w1d_b
		mov ebx, [edi]CWalk1D.bfxValue.u4Frac
		mov ecx, [edi]CWalk1D.bfxValue.i4Int

		// Add the integer portions together.
		add ecx, [esi]CWalk1D.bfxValue.i4Int

		//
		// Add the fractional portions together, and add an extra step value if a carry
		// results.
		//
		add ebx, [esi]CWalk1D.bfxValue.u4Frac
		jnc short SKIP_STEP_ADD
		add ecx, [edi]CWalk1D.iOffsetPerLine

	SKIP_STEP_ADD:

		// Store the modified variables.
		mov [edi]CWalk1D.bfxValue.u4Frac, ebx
		mov [edi]CWalk1D.bfxValue.i4Int, ecx
	}
}


//*********************************************************************************************
//
inline void operator+=
(
	      CWalk2D& w2d_a,	// First 2D walk parameter, cumulative results stored here.
	const CWalk2D& w2d_b	// Second 2D walk parameter.
)
//
// Adds two CWalk2D parameters together and stores the result in the first parameter.
//
// Notes:
//		This code demonstrates an implementation Michael Abrash's trick of walking through
//		a 2D texture map with arbitrary widths and heights. The assembly version is equivalent
//		to the following 'C' code:
//
//		   int i_carry      = iAddCarry(w2d_a.uVFrac, w2d_a.uVFrac, w2d_b.uVFrac);
//		   w2d_a.iUVInt[1] += w2d_b.iUVInt[i_carry + 1] - iAddCarry(w2d_a.uUFrac, w2d_a.uUFrac,
//				                                                     w2d_b.uUFrac);
//
//		The algorithm can be described as follows:
//
//			1.	Add the fractional components of V together, and store the carry in a register.
//
//			2.	Add the fractional components of U together, and add the carry of the U
//				additions to the integer UV portion.
//
//			3.	If there was no carry from the V fraction addition, add the combined integer
//				components of U and V. If there was a carry from the V fraction addition, add
//				the combined integer components of U and V plus a value representing the V line
//				offset.
//
//**************************************
{
	//
	// Register usage:
	//
	//	 Register    Variable
	//
	//		eax		Temporary
	//		ebx		w2d_a.uUFrac
	//		ecx		w2d_a.uVFrac
	//		edx		w2d_a.iUVInt[1]
	//		edi		w2d_a
	//		esi		w2d_b
	//
	__asm
	{
		// Load variables into the registers.
		mov edi, w2d_a
		mov esi, w2d_b
		mov ebx, [edi]CWalk2D.uUFrac
		mov ecx, [edi]CWalk2D.uVFrac
		mov edx, [edi + 4]CWalk2D.iUVInt

		// Add the V fractional values together and store the carry.
		add ecx, [esi]CWalk2D.uVFrac
		sbb eax, eax

		// Add the U fractional values, setting the carry flag.
		add ebx, [esi]CWalk2D.uUFrac

		// Add the integer values from the integer table, and add a possible carry.
		adc edx, [esi + eax*4 + 4]CWalk2D.iUVInt

		// Store the modified variables.
		mov [edi]CWalk2D.uUFrac, ebx
		mov [edi]CWalk2D.uVFrac, ecx
		mov [edi + 4]CWalk2D.iUVInt, edx
	}
}


#if VER_ASM

	//*****************************************************************************************
	//
	inline void Initialize
	(
		CWalk2D& rw2d,
		float    f_u,		// The 'U' component.
		float    f_v,		// The 'V' component.
		int      i_width	// The width of the texture map.
	)
	//
	//	Sets the fractional and integer parts of the CWalk2D object.
	//
	//**************************************
	{
		const float fFixed16Scale = 65536.0;
		int32 i4_temp;

		__asm
		{
			mov esi, [rw2d]

			fld   [fFixed16Scale]
			fmul  [f_u]
			fistp [i4_temp]

			fld   [fFixed16Scale]
			fmul  [f_v]
			mov   ecx, [i4_temp]

			sar   ecx, 16
			mov   edx, [i4_temp]

			fistp [i4_temp]

			shl   edx, 16
			mov   eax, [i4_temp]

			sar   eax, 16
			mov   [esi]CWalk2D.uUFrac, edx

			imul  [i_width]
			mov   ebx, [i4_temp]

			shl   ebx, 16
			add eax, ecx

			mov [esi]CWalk2D.uVFrac, ebx
			mov [esi + 4]CWalk2D.iUVInt, eax

			add eax, i_width
			mov [esi]CWalk2D.iUVInt, eax
		}
	}


	//*****************************************************************************************
	//
	inline void InitializePos
	(
		CWalk2D& rw2d,
		float    f_u,		// The 'U' component.
		float    f_v,		// The 'V' component.
		int      i_width	// The width of the texture map.
	)
	//
	//	Sets the fractional and integer parts of the CWalk2D object.
	//
	//**************************************
	{
		#define iBITS_ACCURACY 11
		const float fFastFixed16Conversion = float(1 << (23 - iBITS_ACCURACY));
		int32 i4_temp_a;
		int32 i4_temp_b;

		__asm
		{
			mov esi, [rw2d]

			fld  [fFastFixed16Conversion]
			fld  st(0)
			fadd [f_u]
			fstp float ptr[i4_temp_a]
			mov   ecx, [i4_temp_a]
			fadd [f_v]

			sar   ecx, iBITS_ACCURACY

			fstp [i4_temp_b]

			and   ecx, 0x007FFFFF >> iBITS_ACCURACY
			mov   edx, [i4_temp_a]

			shl   edx, 32 - iBITS_ACCURACY
			mov   eax, [i4_temp_b]

			sar   eax, iBITS_ACCURACY
			and   eax, 0x007FFFFF >> iBITS_ACCURACY
			mov   [esi]CWalk2D.uUFrac, edx

			imul  [i_width]
			mov   ebx, [i4_temp_b]

			shl   ebx, 32 - iBITS_ACCURACY
			add   eax, ecx

			mov [esi]CWalk2D.uVFrac, ebx
			mov [esi + 4]CWalk2D.iUVInt, eax

			//add eax, i_width
			//mov [esi]CWalk2D.iUVInt, eax
		}
	}

#endif // VER_ASM


//
// Additional notes.
//

/**********************************************************************************************
*
* This function is included only to illustrate how the 'C' versions of the overloaded '+='
* operators for CWalk1D and CWalk2D classes work. Reference is made in the notes of these
* respective functions to this 'iAddCarry' function.
*
*	inline int iAddCarry(uint& u_result, uint i_a, uint i_b)
*	{
*		// Add the two values together.
*		u_result = i_a + i_b;
*
*		// Return -1 if a carry is present.
*		if ((u_result < i_a) || (u_result < i_b))
*			return -1;
*
*		// Return 0 if there is no carry.
*		return 0;
*	}
*
**********************************************************************************************/


#endif
