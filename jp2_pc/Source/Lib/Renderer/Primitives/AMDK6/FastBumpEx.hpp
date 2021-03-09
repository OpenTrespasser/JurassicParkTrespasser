/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Assembly functions for fast bumpmapping.
 *		Optimized for the AMD K6-3D Processor
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/Renderer/Primitives/AMDK6/FastBumpEx.hpp                          $
 * 
 * 3     1/19/98 7:30p Pkeet
 * Added support for 16 bit bumpmaps by adding lower colour resolution and a smaller size for
 * 'CBumpAnglePair.'
 * 
 * 2     97.10.27 1:25p Mmouni
 * Initial K6-3D version, copied from the Pentium versions.
 * 
 * 4     9/01/97 8:03p Rwyatt
 * 
 * 3     9/24/96 2:56p Pkeet
 * Removed the overloaded '+' operator assembly function.
 * 
 * 2     9/11/96 11:35a Pkeet
 * Added explicit references (dword ptr's) to assembly operations involving the angle pair
 * format.
 * 
 * 1     9/06/96 2:50p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_AMDK6_FASTBUMPEX_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_AMDK6_FASTBUMPEX_HPP

// Make sure this file is being included from 'FastBump.hpp'.
#ifndef HEADER_LIB_RENDERER_PRIMITIVES_FASTBUMP_HPP
	#error The header file 'FastBump.hpp' has not been included!
#endif


//*********************************************************************************************
//
inline uint8 u1GetIntensityFromBumpmap
(
	CBumpAnglePair bangp,			// The angle pair of a bumpmap's pixel.
	uint32         u4_light_theta,	// The theta component only of the light source relative
									// to the polygon.
	uint8*         u1a_subtable		// Pointer into the angle to intensity lookup table (this
									// pointer should be set to represent the light source's
									// phi value relative to the polygon.
)
//
// Returns the intensity value given a bumpmap pixel and lighting source values.
//
//**************************************
{
	uint8 u1_intensity;	// Storage for the intensity return value.

	Assert(u1a_subtable);

	__asm
	{
#if iBUMPMAP_RESOLUTION == 16
		movzx eax, word ptr [bangp]			// Load the bumpmap pixel.
#else
		mov eax, dword ptr [bangp]			// Load the bumpmap pixel.
#endif // iBUMPMAP_RESOLUTION == 16
		mov ebx, u1a_subtable				// Load the pointer to the angle to intensity
											// lookup table.
		sub eax, u4_light_theta				// Find differences in theta.
		and eax, iMASK_ANGLETABLE_LOOKUP	// Mask out everything but the theta-phi index.
		mov al, byte ptr[eax + ebx]			// Copy intensity value from the lookup table.
		mov u1_intensity, al				// Move the intensity value to the return value.

	};

	// Return the intensity value.
	return u1_intensity;
}


#endif
