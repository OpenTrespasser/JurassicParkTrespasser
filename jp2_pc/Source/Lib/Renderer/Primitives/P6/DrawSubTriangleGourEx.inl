/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function.
 *		Optimized for the Pentium PRO Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P6/DrawSubTriangleGourEx.cpp                  $
 * 
 * 7     97/12/23 5:51p Mmouni
 * 
 * 6     97.12.22 9:08p Mmouni
 * Fixed error with destination of terrain gouraud shading primitive.
 * 
 * 5     97.12.22 6:07p Mmouni
 * Changed terrain gouraud shading routines so that they don't always draw to the screen.
 * 
 * 4     97.12.22 2:53p Mmouni
 * Added new terrain gouraud shading primitive.
 * 
 * 3     9/01/97 8:05p Rwyatt
 * 
 * 2     97/07/18 3:29p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 1     7/08/97 1:15p Rwyatt
 * Initial Implementation of Pentium Pro rendering specific optimizations
 * 
 ***********************************************************************************************
 ***********************************************************************************************
 *
 * 5     97/06/12 5:56p Pkeet
 * Added capability to render only every other scanline and to double the number of scanlines.
 * 
 * 4     97/06/11 11:41p Pkeet
 * Added more optimal inner loop code.
 * 
 * 3     97/06/11 11:27p Pkeet
 * Initial assembly version.
 * 
 * 2     97/06/11 11:20p Pkeet
 * Simplified the C++ version.
 * 
 * 1     97/06/11 10:15p Pkeet
 * Initial C++ versions of primitives.
 * 
 **********************************************************************************************/

#include "common.hpp"

#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"

//
// Temporary Globals.
//
// Notes:
//		These variables should be arraigned along with the external globals for maximum
//		cache efficiency.
//
static fixed fx_gour_inc;
static int i_line_pixels;
static uint16* ppix_clut;


//*****************************************************************************************
void DrawSubtriangle(TGour* pscan, CDrawPolygon<TGour>* pdtri)
{
	int     i_x_from;
	int     i_x_to;
	int     i_screen_index;
	int     i_pixel;
	uint16* apix_screen;
	fixed   fx_intensity;

	uint16* ppix_clut = ((uint16*)pvClutConversion) + u4ConstColour;

	//
	// Iterate through the scanlines that intersect the subtriangle.
	//
	do
	{
		if (bRIGHT_TO_LEFT)
		{
			i_x_from = (pscan->fxX.i4Fx - pdtri->fxLineLength.i4Fx)>>16;
			i_x_to   = (pscan->fxX.i4Fx)>>16;
			i_screen_index = pdtri->iLineStartIndex + i_x_from - 1;
			i_pixel  = i_x_to - i_x_from;
		}
		else
		{
			i_x_from = (pscan->fxX.i4Fx)>>16;
			i_x_to   = (pscan->fxX.i4Fx + pdtri->fxLineLength.i4Fx)>>16;
			i_screen_index = pdtri->iLineStartIndex + i_x_to;
			i_pixel  = i_x_from - i_x_to;
		}

		// Draw if there are pixels to draw.
		if (i_x_to > i_x_from && !(bEvenScanlinesOnly && ((pdtri->iY & 1) == 0)))
		{

			// Get the start address for the scanline on the destination raster.
			apix_screen = (uint16*)pdtri->prasScreen->pSurface;
			apix_screen += i_screen_index;

			// Get the starting gouraud shading value.
			fx_intensity.i4Fx = pscan->gourIntensity.fxIntensity.i4Fx;

			__asm
			{
				mov edx, [fx_intensity]
				xor ebx, ebx
				mov esi, [apix_screen]
				mov edi, [i_pixel]
				mov ecx, [ppix_clut]
				push ebp
				mov ebp, [i4ScanlineDirection]
				mov eax, 0xFFFF0000

		BEGIN_INNER_LOOP:

				and eax, edx
				add edx, [fxDeltaXIntensity]

				shr eax, 8

				mov bx, [ecx + eax*2]
				mov eax, 0xFFFF0000

				mov [esi + edi*2], bx

				add edi, ebp

				jnz short BEGIN_INNER_LOOP
				pop ebp
			}
		}
		
		// Increment the base edge.
		++*pdtri->pedgeBase;

		// Set the new length of the line to be rasterized.
		pdtri->fxLineLength.i4Fx += pdtri->fxDeltaLineLength.i4Fx;

		// Get new starting pixel TIndex for the scanline.
		pdtri->iLineStartIndex += pdtri->prasScreen->iLinePixels;
	}
	while (++pdtri->iY < pdtri->iYTo);
}


//*****************************************************************************************
// Gouraud shaded triangle function that draws to a composite texture map.
//
void DrawSubtriangle(TShadeTerrain* pscan, CDrawPolygon<TShadeTerrain>* pdtri)
{
	typedef CDrawPolygon<TShadeTerrain> TAccess;
	static TShadeTerrain* pscanGlbl;
	static CDrawPolygon<TShadeTerrain>* pdtriGlbl;

	fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
	fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
	fx_gour_inc		= pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
	i_line_pixels	= pdtri->prasScreen->iLinePixels;
	pscanGlbl		= pscan;
	pdtriGlbl		= pdtri;

	_asm
	{
		mov		edx,[pdtri]
		mov		ecx,[pscan]

		mov		eax,[edx]CDrawPolygonBase.iY
		mov		edi,[edx]CDrawPolygonBase.iLineStartIndex
		// ecx, edi, eax and edx are also set from the loop end.

		//--------------------------------------------------------------------------------------
		// scan line start
NEXT_SCAN:
		and		eax,[bEvenScanlinesOnly]
		jnz		short SKIP_LOOP

		//--------------------------------------------------------------------------------------
		// Calculate start, length of edge.
		mov		esi,[ecx]TGour.fxX.i4Fx
		mov		eax,[edx]CDrawPolygonBase.fxLineLength.i4Fx

		add		eax,esi

		sar		eax,16
		mov		ebx,[edx]TAccess.prasScreen

		sar		esi,16
		add		edi,eax

		mov		ebx,[ebx]CRasterBase.pSurface

		sub		esi,eax
		jge		short SKIP_LOOP

		// Get the starting gouraud shading value.
		mov		edx,[ecx]TShadeTerrain.gourIntensity.fxIntensity.i4Fx		
		lea		edi,[edi*2+ebx]

		mov		[pvBaseOfLine],edi
		mov		eax,[fxDeltaXIntensity]

		//
		// Write intensity value to every other pixel.
		//
INNER_LOOP:
		mov		ebx,edx					// Copy intensity value
		add		edx,eax					// Step intensity value

		shr		ebx,16					// Shift intensity value

		mov		[edi + esi*2 + 1],bl	// Store integer intensity byte
		inc		esi						// Step count

		jnz		short INNER_LOOP

		//--------------------------------------------------------------------------------------
		// Loop control an sub pixel correction for edges
SKIP_LOOP:
		mov		edx,[pdtriGlbl]
		mov		ecx,[pscanGlbl]

		mov		esi,[ecx]TShadeTerrain.gourIntensity.fxIntensity.i4Fx
		mov		ebx,[ecx]TShadeTerrain.fxXDifference

		add		esi,[fx_gour_inc]
		add		ebx,[fx_diff]

		mov		eax,[fxIModDiv]
		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000
		add		esi,eax
		
		jmp		short DONE_Y

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short DONE_Y

		// Overflow of fxXDifference.
		sub		ebx,0x00010000
		add		esi,eax

DONE_Y:
		// Store modified variables.
		mov		[ecx]TShadeTerrain.gourIntensity.fxIntensity.i4Fx,esi
		mov		[ecx]TShadeTerrain.fxXDifference,ebx

		// Step length, line starting address.
		mov		ebx,[edx]CDrawPolygonBase.fxLineLength.i4Fx
		mov		edi,[edx]CDrawPolygonBase.iLineStartIndex

		add		ebx,[edx]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		edi,[i_line_pixels]

		mov		[edx]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[edx]CDrawPolygonBase.iLineStartIndex,edi

		mov		ebx,[ecx]TShadeTerrain.fxX
		mov		esi,[fx_inc]

		add		ebx,esi
		mov		eax,[edx]CDrawPolygonBase.iY

		mov		[ecx]TShadeTerrain.fxX,ebx
		inc		eax

		mov		ebx,[edx]CDrawPolygonBase.iYTo
		mov		[edx]CDrawPolygonBase.iY,eax

		// ecx, edi, eax and edx are set for the next interation of the loop.
		cmp		eax,ebx
		jl		NEXT_SCAN
	}
}
