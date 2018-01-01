/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Defines objects for walking through 1D and 2D surfaces.
 *
 * Bugs:
 *
 * To do:
 *		Change the name of the module to 'TexWalk.hpp.'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/Walk.hpp                                     $
 * 
 * 13    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 12    97/10/23 10:57a Pkeet
 * Added a K6 3D switch.
 * 
 * 11    9/15/97 2:04p Mmouni
 * Now uses u4TextureTileMaskStepU so that tiling works without shifting.
 * 
 * 10    7/07/97 11:53p Rwyatt
 * No includes headers that contain assembly code from the processor specific directories
 * 
 * 9     1/15/97 1:02p Pkeet
 * Moved the assembly versions of the 2d initializing functions to WalkEx.hpp. Removed fog
 * parameters.
 * 
 * 8     12/15/96 3:18p Pkeet
 * Added the 'InitializePos' function.
 * 
 * 7     12/11/96 9:04p Pkeet
 * Added the inline function for assembly initialization of the CWalk2D class.
 * 
 * 6     12/11/96 7:15p Pkeet
 * Initial assembly implementation of the initialize member function of CWalk2D.
 * 
 * 5     12/11/96 5:26p Pkeet
 * Changing initializing parameters for the 2D walking from doubles to floats.
 * 
 * 4     12/09/96 1:27p Pkeet
 * Added the dGet member function.
 * 
 * 3     8/12/96 7:15p Pkeet
 * Added comments. Changed the 'SetData' member functions to 'Initialize.'
 * 
 * 2     7/10/96 1:48p Pkeet
 * Added additional comments.
 * 
 * 1     7/10/96 12:17p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_WALK_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_WALK_HPP


//
// Includes.
//
#include "Config.hpp"
#include "Lib/Types/BigFixed.hpp"


// Global mask for tiling U integer step.
extern uint32 u4TextureTileMaskStepU;


//
// Class definitions.
//

//*********************************************************************************************
//
class CWalk1D
//
// A 64 bit fixed point type with an offset value and offset functions for the integer
// portion. Simplifies walking through 2D texture maps when used with CWalk2D.
//
// Notes:
//		This class differs from the UBigFixed type in that instead of having an integer portion
//		representing units of one, its integer portion represents units of some arbitrary size.
//
//		So, for example, in the UBigFixed type when a carry is generated in the fractional
//		portion as a result of addition, one is added to the integer portion. In the CWalk1D
//		type, when the fractional values generate a carry, an arbitrary amount is added to the
//		integer portion instead of a one.
//
//		This type is a precursor to CWalk2D, an object which essentially encapsulates Michael
//		Abrash's trick to walking arbitrary-sized texture maps. For more information, consult
//		documentation for CWalk2D and for the '+=' overloaded operator associated with it.
//
//		This class contains a 64 bit fixed point variable instead of being inherited from it
//		to prevent errors in its use.
//
//**************************************
{
public:

	UBigFixed bfxValue;			// 64 bit fixed point value as a base value.
	int       iOffsetPerLine;	// Units for the integer portion of the value.

public:

	//*****************************************************************************************
	//
	forceinline void Initialize
	(
		float f,
		int   i_offset_per_line
	)
	//
	// Sets the member variables of the class based on a floating-point value and a arbitrary
	// walk offset.
	//
	//**************************************
	{
		// Set the offset value.
		iOffsetPerLine = i_offset_per_line;

		// Convert the floating point value to a 64 bit fixed point value.
		bfxValue = f;

		// Set the integer portion of the value into offset units.
		bfxValue.i4Int *= i_offset_per_line;
	}

	//*****************************************************************************************
	//
	forceinline CWalk1D operator-
	(
	) const
	//
	// Returns the negative this value.
	//
	// Notes:
	//		The negative of a 64 bit value (ei., a 32.32 fixed point value) is found by
	//		subtracting the value from 0. If the fractional component of the original value
	//		is not zero, a borrow will be performed from the integer portion.
	//
	//		This function approximates this behaviour by performing a one's complement on
	//		the fractional portion of the value instead of a two's complement. A borrow will
	//		always be performed on the integer portion. This function is therefore
	//		not totally accurate, but it is fast. The loss of accuracy is negligible, however.
	//
	//**************************************
	{
		CWalk1D fw_return_val;	// Return value.

		// Copy the offset the return value.
		fw_return_val.iOffsetPerLine  = iOffsetPerLine;

		// Negate the integer portion and "borrow" the offset value.
		fw_return_val.bfxValue.i4Int  = -bfxValue.i4Int - iOffsetPerLine;

		// Perform a one's complement on the fractional value.
		fw_return_val.bfxValue.u4Frac = ~bfxValue.u4Frac;
		return fw_return_val;
	}

	// Conversion to double.
	double dGet() const
	{
		double d_int  = double(bfxValue.i4Int / iOffsetPerLine);
		double d_frac = double(bfxValue.u4Frac) * dFracMultiplier;
		return d_int + d_frac;
	}

};


//*********************************************************************************************
//
class CWalk2D
//
// An object for walking through arbitrary-sized texture maps.
//
// Notes:
//		If the object represents a delta, iUVInt[1] represents the value to add to the integer
//		portion if there is no carry from the addition of the fractional portions, iUVInt[0]
//		represents the value to add if there is a carry.
//
//		If the object represents an accumulative value, the integer portion is contained in
//		iUVInt[1], which represents the index into the bitmap being walked.
//
//		For further information on how this class works, consult the notes associated with
//		the overloaded '+=' operator for this class.
//
//**************************************
{

public:

	int  iUVInt[2];	// The integer components of the Walk object. 
	uint uUFrac;	// The U fractional component.
	uint uVFrac;	// The V fractional component.

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	forceinline CWalk2D
	(
	)
	{
	}

	//*****************************************************************************************
	//
	forceinline CWalk2D
	(
		const UBigFixed& bfx_u,	// The 'U' component.
		const CWalk1D&   w1d_v	// The 'V' component.
	)
	//
	//	Constructs the CWalk2D object given two values.
	//
	//**************************************
	{
		Initialize(bfx_u, w1d_v);
	}

	//*****************************************************************************************
	//
	forceinline CWalk2D
	(
		float f_u,		// The 'U' component.
		float f_v,		// The 'V' component.
		int   i_width	// The width of the texture map.
	)
	//
	//	Constructs the CWalk2D object given two values.
	//
	//**************************************
	{
		// Don't use this bit of code just yet.
		//Assert(0);

		Initialize(f_u, f_v, i_width);
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	forceinline void Initialize
	(
		float f_u,		// The 'U' component.
		float f_v,		// The 'V' component.
		int   i_width	// The width of the texture map.
	)
	//
	//	Sets the fractional and integer parts of the CWalk2D object.
	//
	//**************************************
	{
		// Don't use this bit of code just yet.
		//Assert(0);

		UBigFixed bfx_u;	// Fixed point version of 'u.'
		CWalk1D   w1d_v;	// Fixed point version of 'v.'

		bfx_u = double(f_u);
		w1d_v.Initialize(double(f_v), i_width);

		Initialize(bfx_u, w1d_v);
	}

	//*****************************************************************************************
	//
	forceinline void Initialize
	(
		const UBigFixed& bfx_u,	// The 'U' component.
		const CWalk1D&   w1d_v	// The 'V' component.
	)
	//
	//	Sets the fractional and integer parts of the CWalk2D object.
	//
	//**************************************
	{
		// Set the U and V fractional values.
		// MSM: Masked to keep only 16 fractional bits becuase
		// some of the Pentium primitives utilize the low part of
		// the registers the fractional bits are kept in.
		uUFrac = bfx_u.u4Frac & 0xFFFF0000;
		uVFrac = w1d_v.bfxValue.u4Frac & 0xFFFF0000;

		// Set the combined integer value, and generate a carry version.
		// U must be masked for tiled texture before being combined with V.
		iUVInt[1] = (bfx_u.i4Int & u4TextureTileMaskStepU) + w1d_v.bfxValue.i4Int;
		iUVInt[0] = iUVInt[1] + w1d_v.iOffsetPerLine;
	}
	
};


//
// Global functions.
//

#if !VER_ASM

	//*****************************************************************************************
	//
	forceinline void Initialize
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
		CWalk2D w2d(f_u, f_v, i_width);

		rw2d = w2d;
	}

	//*****************************************************************************************
	//
	forceinline void InitializePos
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
		Initialize(rw2d, f_u, f_v, i_width);
	}

#endif // !VER_ASM


//
// Optimized versions of overloaded functions.
//
//**********************************************************************************************
#if TARGET_PROCESSOR == PROCESSOR_PENTIUM
	#include "Lib/Renderer/Primitives/P5/WalkEx.hpp"
#elif TARGET_PROCESSOR == PROCESSOR_PENTIUMPRO
	#include "Lib/Renderer/Primitives/P6/WalkEx.hpp"
#elif TARGET_PROCESSOR == PROCESSOR_K6_3D
	#include "Lib/Renderer/Primitives/AMDK6/WalkEx.hpp"
#else
	#error Invalid [No] target processor specified
#endif
//**********************************************************************************************



#endif
