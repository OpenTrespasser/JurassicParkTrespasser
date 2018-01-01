/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function.
 *		Optimized for the AMD K6-3D Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/DrawSubTriangleGourEx.cpp               $
 * 
 * 8     10/01/98 12:40a Asouth
 * explicitly scoped fixed class
 * 
 * 7     97.12.22 9:08p Mmouni
 * Fixed error with destination of terrain gouraud shading primitive.
 * 
 * 6     97.12.22 6:07p Mmouni
 * Changed terrain gouraud shading routines so that they don't always draw to the screen.
 * 
 * 5     97.12.22 2:53p Mmouni
 * Added new terrain gouraud shading primitive.
 * 
 * 4     97.10.30 1:37p Mmouni
 * Optimized inner loop for K6.
 * 
 * 3     97.10.27 1:25p Mmouni
 * Initial K6-3D version, copied from the Pentium versions.
 * 
 * 6     10/10/97 1:47p Mmouni
 * All inner loops are now left to right only, and optimized some.
 * 
 * 5     9/15/97 2:10p Mmouni
 * Optimized inner loops (not final yet).
 * 
 * 4     9/01/97 8:02p Rwyatt
 * 
 * 3     97/07/18 3:29p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 2     7/14/97 12:36p Rwyatt
 * Checkled in so Scott can move them....
 * 
 * 1     7/07/97 11:15p Rwyatt
 * Initial Implementation of Pentium Specifics
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

#include "Common.hpp"

#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"

//
// Temporary Globals.
//
// Notes:
//		These variables should be arraigned along with the external globals for maximum
//		cache efficiency.
//
static ::fixed fx_gour_inc;
static int i_line_pixels;
static uint16* ppix_clut;


//*****************************************************************************************
// Gouraud shaded triangle function.
//
void DrawSubtriangle(TGour* pscan, CDrawPolygon<TGour>* pdtri)
{
	static TGour* pscanGlbl;
	static CDrawPolygon<TGour>* pdtriGlbl;

	fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
	fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
	fx_gour_inc		= pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
	i_line_pixels	= pdtri->prasScreen->iLinePixels;
	ppix_clut		= ((uint16*)pvClutConversion) + u4ConstColour;
	pscanGlbl		= pscan;
	pdtriGlbl		= pdtri;

	_asm
	{
		mov		edx,[pdtri]
		mov		ecx,[pscan]

		push	ebp

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
		mov		ebx,gsGlobals.pvScreen

		sar		esi,16
		add		edi,eax

		sub		esi,eax
		jge		short SKIP_LOOP

		// Get the starting gouraud shading value.
		mov		edx,[ecx]TGour.gourIntensity.fxIntensity.i4Fx		
		lea		edi,[edi*2+ebx]

		mov		ecx,[ppix_clut]
		mov		ebx,0xFFFF0000

		mov		[pvBaseOfLine],edi
		mov		ebp,[fxDeltaXIntensity]

		lea		eax,[edi + esi*2]

		test	eax,3
		jz		short ALIGNED

		// Check for one pixel before starting.
		inc		esi
		jz		short FINISH_REMAINDER

		//
		// Do one pixel for alignment.
		//
		and		ebx,edx					// Copy and mask intensity value
		add		edx,ebp					// Step intensity value
		shr		ebx,8					// Shift masked intensity value
		mov		bx,[ecx + ebx*2]		// Lookup pixel
		mov		[edi + esi*2 - 2],bx	// Store pixel
		mov		ebx,0xFFFF0000			// Load mask value into ebx

ALIGNED:
		// Make sure we have two pixels left.
		add		esi,2
		jg		short FINISH_REMAINDER

		mov		edi,0xFFFF0000			// Load mask value into edi.

		//
		// Do two pixels at a time.
		//
INNER_LOOP_2P:
		and		ebx,edx						// Copy and mask intensity (1).
		add		edx,ebp						// Step intensity.

		shr		ebx,8						// Shift masked intensity value (1).
		and		edi,edx						// Copy and mask intensity (2).

		movzx	eax,word ptr[ecx + ebx*2]	// Lookup pixel (1)
		shr		edi,8						// Shift masked intensity value (2).

		movzx	ebx,word ptr[ecx + edi*2]	// Lookup pixel (2)
		add		edx,ebp						// Step intensity.

		mov		edi,[pvBaseOfLine]			// Load pointer to base of scanline.
		shl		ebx,16						// Shift second pixel up (stall on ebx?)

		or		eax,ebx						// Merge pixels
		mov		[edi + esi*2 - 4],eax		// Store pixels.

		mov		ebx,0xFFFF0000				// Load mask value into ebx.
		mov		edi,0xFFFF0000				// Load mask value into edi.

		add		esi,2
		jle		short INNER_LOOP_2P

		// esi is 1 if there is a pixel left.
		cmp		esi,1
		jne		short SKIP_LOOP

		//
		// Finish left over pixel.
		//
FINISH_REMAINDER:
		and		ebx,edx					// Copy and mask intensity value
		mov		edi,[pvBaseOfLine]		// Load pointer to base of scanline.
		shr		ebx,8					// Shift masked intensity value
		mov		bx,[ecx + ebx*2]		// Lookup pixel
		mov		[edi - 2],bx			// Store pixel

		//--------------------------------------------------------------------------------------
		// Loop control an sub pixel correction for edges
SKIP_LOOP:
		mov		edx,[pdtriGlbl]
		mov		ecx,[pscanGlbl]

		mov		esi,[ecx]TGour.gourIntensity.fxIntensity.i4Fx
		mov		ebx,[ecx]TGour.fxXDifference

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
		mov		[ecx]TGour.gourIntensity.fxIntensity.i4Fx,esi
		mov		[ecx]TGour.fxXDifference,ebx

		// Step length, line starting address.
		mov		ebx,[edx]CDrawPolygonBase.fxLineLength.i4Fx
		mov		edi,[edx]CDrawPolygonBase.iLineStartIndex

		add		ebx,[edx]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		edi,[i_line_pixels]

		mov		[edx]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[edx]CDrawPolygonBase.iLineStartIndex,edi

		mov		ebx,[ecx]TGour.fxX
		mov		esi,[fx_inc]

		add		ebx,esi
		mov		eax,[edx]CDrawPolygonBase.iY

		mov		[ecx]TGour.fxX,ebx
		inc		eax

		mov		ebx,[edx]CDrawPolygonBase.iYTo
		mov		[edx]CDrawPolygonBase.iY,eax

		// ecx, edi, eax and edx are set for the next interation of the loop.
		cmp		eax,ebx
		jl		NEXT_SCAN

		pop		ebp
	}
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
