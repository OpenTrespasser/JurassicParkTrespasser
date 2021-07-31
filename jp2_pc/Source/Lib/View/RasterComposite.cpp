/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Implementation of RasterComposite.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/RasterComposite.cpp                                          $
 * 
 * 8     7/09/98 7:59p Mlange
 * Apply clut function now has optional sub-rect parameter.
 * 
 * 7     98.05.20 7:32p Mmouni
 * Optimized Pentium version of ApplyClut16() to process a DWORD at a time.
 * 
 * 6     3/16/98 6:34p Pkeet
 * Made slight assembly optimizations.
 * 
 * 5     3/16/98 6:19p Pkeet
 * Added an and to clamp the clut index value to prevent a crash bug. Reorganized code to make
 * optimizing particular targets easier.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Clut.hpp"
#include "Raster.hpp"
#include "RasterComposite.hpp"

#undef VER_ASM

//
// Function prototypes.
//

//**********************************************************************************************
//
void ApplyClut16
(
	uint16* pu2,		// Surface to apply clut to.
	const uint16* au2_clut,	// Clut table.
	int     i_width,	// Width of the surface in pixels.
	int     i_height,	// Height of the surface in pixels.
	int     i_stride,	// Stride of the surface in pixels.
	uint16  u2_mask		// Bit mask for alpha.
);
//
// Applies a clut to a 16 bit raster.
//
//**********************************


//
// Function implementations.
//

//**********************************************************************************************
void ApplyClut(rptr<CRaster> pras, const CClut& clut, int i_fog, uint16 u2_mask, const SRect* prect)
{
	Assert(pras);
	Assert(bWithin(i_fog, 0, iDefNumDepthValues - 1));

	Assert(!prect || (prect->iX >= 0 && prect->iY >= 0));
	Assert(!prect || ((prect->iX + prect->iWidth) <= pras->iWidth && (prect->iY + prect->iHeight) <= pras->iHeight));
	Assert(!prect || ((prect->iX & 1) == 0 && (prect->iWidth & 1) == 0));

	// Get a pointer to the set clut position.
	void* pv_clut = clut.pvGetConversionAddress(0, 0, i_fog);
	Assert(pv_clut);

	// Select a conversion based on the raster's depth.
	switch (pras->iPixelBits)
	{
		case 16:
			// Apply a clut to a composite 16 bit surface.
			ApplyClut16
			(
				(uint16*)(prect ? pras->pAddress(prect->iX, prect->iY) : pras->pAddress(0, 0)),
				(uint16*)pv_clut,
				prect ? prect->iWidth  : pras->iWidth,
				prect ? prect->iHeight : pras->iHeight,
				pras->iLinePixels,
				u2_mask
			);
			break;

		default:
			Assert(0);
	}
}

//**********************************************************************************************
void ApplyClut16(uint16* pu2, const uint16* au2_clut, int i_width, int i_height, int i_stride,
				 uint16 u2_mask)
{

#if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_PENTIUM)

	int i_stride2;

	__asm
	{
		mov		edx,[i_height]		// Height.
		mov		ecx,[i_width]		// Width.

		mov		edi,[pu2]			// Pixmap pointer.
		xor		eax,eax				// Clear eax.

		mov		esi,[au2_clut]		// Clut pointer.
		mov		ebx,[i_stride]		// Stride

		lea		edi,[edi+ecx*2]		// Adjust to one past the last pixel.
		add		ebx,ebx				// Double stride for offset.

		mov		[i_stride2],ebx		// Stride times 2.
		xor		ebx,ebx				// Clear ebx.

		neg		ecx					// index goes is in the interval [-i_width, 0)

PIXEL_LOOP:
		mov		ebx,[edi + ecx*2]		// Get pixels.

		mov		eax,ebx					// Copy pixels
		and		ebx,0x1fff0000			// Mask second pixel.

		shr		ebx,16					// Shift second pixel down.
		and		eax,0x00001fff			// Mask first pixel.

		// AGI on ebx.

		mov		bx,[esi + ebx*2]		// Clut lookup.

		shl		ebx,16					// Shift second pixel up.
		add		ecx,2

		mov		bx,[esi + eax*2]		// Clut lookup.

		mov		[edi + ecx*2 - 4],ebx	// Store pixels.
		jl		PIXEL_LOOP

		// Leftover pixel?
		nop
		jz		DONE_WITH_LINE

		mov		al,[edi - 2]			// Get pixel (low).
		xor		ecx,ecx					// Clear ecx.
		mov		ah,[edi - 1]			// Get pixel (high).
		and     ah,0x1F					// Mask out of range pixels.
		mov		ax,[esi + eax*2]		// Clut lookup.
		mov		[edi - 2],ax			// Store pixel.

DONE_WITH_LINE:
		add		edi,[i_stride2]		// Next line.
		sub		ecx,[i_width]		// Negative width in ecx.

		dec		edx
		jnz		PIXEL_LOOP
	}

#elif (VER_ASM && TARGET_PROCESSOR != PROCESSOR_PENTIUM)

	__asm
	{
		mov		edx,[i_height]		// Height.
		mov		ecx,[i_width]		// Width.

		mov		edi,[pu2]			// Pixmap pointer.
		xor		eax,eax				// Clear eax.

		mov		esi,[au2_clut]		// Clut pointer.
		mov		ebx,[i_stride]		// Stride

		lea		edi,[edi+ecx*2]		// Adjust to one past the last pixel.
		add		ebx,ebx				// Double stride for offset.

		neg		ecx					// index goes is in the interval [-i_width, 0)

PIXEL_LOOP:

		movzx	eax,word ptr[edi + ecx*2]	// Get pixel.
		and     eax,0x00001FFF

		mov		ax,[esi + eax*2]			// Clut lookup.

		mov		[edi + ecx*2],ax			// Store pixel.

		inc		ecx
		jnz		PIXEL_LOOP

		add		edi,ebx				// Next line.
		sub		ecx,[i_width]		// Negative width in ecx.

		dec		edx
		jnz		PIXEL_LOOP
	}

#else

	// Adjust the pointer to accomodate the increment that terminates when equal to zero.
	pu2 += i_width;

	// Calculate a pointer to one past the last pixel to be converted.
	uint16* pu2_last = pu2 + i_height * i_stride;

	// Iterate through lines of pixels.
	for (; pu2 < pu2_last; pu2 += i_stride)
	{
		// Iterate through pixels on a line.
		for (int i_x = -i_width; i_x != 0; ++i_x)
		{
			// Transform the pixel.
			pu2[i_x] = au2_clut[pu2[i_x] & 0x1FFF] /*& u2_mask*/;
		}
	}

#endif // else
}
