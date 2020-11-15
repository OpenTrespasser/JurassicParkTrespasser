/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function.
 *		The flat shaded polygons are already optimal for all processors so are not required
 *		to be replicated in the processor specific directories.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/DrawSubTriangleFlat.cpp                       $
 * 
 * 10    98.09.24 1:40a Mmouni
 * Shrunk CRenderPolygon by a large amount.
 * Multiple list depth sort stuff is now on a compile switch.
 * 
 * 9     9/03/98 1:52p Asouth
 * explicit scoping of the fixed data type
 * 
 * 8     98.06.18 3:57p Mmouni
 * Added stippled flat shaded primitive.
 * 
 * 7     8/18/97 1:55p Rwyatt
 * Removed partial stall on eflags from within the DWORD alignment code.
 * 
 * 6     8/17/97 12:24a Rwyatt
 * Moved alpha and terrain shadow primitives to their correct processor specific files.
 * 
 * 5     97/08/14 9:49p Pkeet
 * Added C++ version of the 'TAlphaColour,' 'TShadow' and 'TShadowTrans' primitives.
 * 
 * 4     97/07/18 3:29p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 3     7/07/97 11:43p Rwyatt
 * Updated for different processors
 * 
 * 2     7/07/97 3:54p Rwyatt
 * Moved up one level
 * 
 * 1     7/07/97 2:50p Rwyatt
 * 
 * 13    97/06/25 2:34p Pkeet
 * Fixed bug in wireframe primitive.
 * 
 * 12    97/06/25 2:08p Pkeet
 * Added correct wireframe.
 * 
 * 11    97/06/25 1:14p Pkeet
 * Includes code for displaying depth sort clipping.
 * 
 * 10    97/06/25 12:45p Pkeet
 * Support for a wireframe mode.
 * 
 * 9     6/15/97 8:05p Rwyatt
 * flat shaded 16 bit is now fully in asm.
 * 
 * 8     97/06/15 1:20a Pkeet
 * Rewrote the '++' function to use explicit code.
 * 
 * 7     6/14/97 10:42p Rwyatt
 * Semi-optimized Y loop
 * 
 * 5     6/15/97 1:31a Bbell
 * Removed unnecessary include.
 * 
 * 4     97/06/12 5:56p Pkeet
 * Added capability to render only every other scanline and to double the number of scanlines.
 * 
 * 3     6/11/97 5:23p Rwyatt
 * Removed some AGIs and improved the DWORD alignmnet code
 * 
 * 2     6/11/97 2:06a Rwyatt
 * Both 16 and 8 bit versions do multiple pixel writes within the main loop. This code must be
 * checked with V-Tune because there is some really strange branches and there is a few AGIs
 * that need to be removed. This code is not as fast as it can be.
 * 
 * 1     97/06/10 6:08p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "common.hpp"

//#if VER_ASM

#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"



//*****************************************************************************************
//
// Flat shaded 16 bit.
//
void DrawSubtriangle(TFlat* pscan, CDrawPolygon<TFlat>* pdtri)
{
	::fixed fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
	::fixed fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;

	_asm
	{
		mov		eax, u4ConstColour
		mov		ebx, eax
		shl		ebx, 16
		or		ebx, eax


		mov		edx,pdtri
		mov		eax, [edx]CDrawPolygonBase.iY
		mov		edi,[edx]CDrawPolygonBase.iLineStartIndex
		mov		ecx,pscan
		//		edi, eax and edx are also set from the loop end

NEXT_SCAN:
		cmp		bEvenScanlinesOnly, 0
		jz		short SCAN_LINE

		// do we have to skip this scan?? (Is it an odd scan)
		//mov		eax, [edx]CDrawPolygonBase.iY
		test	eax, 1
		jz		short SKIP_LOOP

SCAN_LINE:
		cmp		[i4ScanlineDirection],0
		jge		short LEFT

		// scan line going to the right
		mov		eax,[ecx]TFlat.fxX.i4Fx
		mov		esi,eax
		sub		eax,[edx]CDrawPolygonBase.fxLineLength.i4Fx
		sar		eax,16
		sar		esi,16
		add		edi,eax
		sub		esi,eax
		jle		short SKIP_LOOP

		//		flip the scan line around, renderer draws the other way
		add		edi,esi
		neg		esi

		jmp		short GO_RIGHT
LEFT:
		// scan line going to the left
		mov		esi,[ecx]TFlat.fxX.i4Fx
		mov		eax,esi
		//mov		edi,[edx]CDrawPolygonBase.iLineStartIndex
		add		eax,[edx]CDrawPolygonBase.fxLineLength.i4Fx
		sar		esi,16
		sar		eax,16
		add		edi,eax
		sub		esi,eax
		jge		short SKIP_LOOP

GO_RIGHT:
		// start of the main scan line
		add		edi, edi
		mov		ecx, esi
		add		edi, gsGlobals.pvScreen

		add		ecx,edi
		test	ecx,3
		jz		short ENTER_LOOP		// effective destination address is DWORD aligned

		mov		[edi + esi*2 ], bx
		inc		esi						// we are now aligned to a DWORD
		jz		short SKIP_LOOP			// are we done?
ENTER_LOOP:		
		mov		ecx,esi					// keep a second counter to reduce AGIs
LEFT_LOOP8:
		add		esi,8					// can we do 8 pixels/ 16 bytes??
		jg		short END_LEFT_LOOP		// cannot do 8 pixels

		mov		[edi + ecx*2], ebx
		mov		[edi + ecx*2+4], ebx	// do the 8 pixels
		mov		[edi + ecx*2+8], ebx
		mov		[edi + ecx*2+12], ebx

		add		ecx,8					// this will not change the flags, result same as add esi+8
		jnz		short LEFT_LOOP8		// there are more pixels
		jmp		short SKIP_LOOP			// we are done

END_LEFT_LOOP:
		add		ecx,2					// can we do 2 pixels/1 DWORD
		jg		short LEFT_4			// no
		mov		[edi + ecx*2-4], ebx	// store the dword
		jnz		short END_LEFT_LOOP		// any more??
		jmp		short SKIP_LOOP			// we are done
LEFT_4:
		// if we get to here there can be only 1 word left
		mov word ptr[edi + ecx*2-4], bx
SKIP_LOOP:
		mov		ecx,pscan
		mov		eax, fx_inc
		mov		esi,[ecx]TFlat.fxXDifference
		add		[ecx]TFlat.fxX,eax
		add		esi, fx_diff

		cmp		esi, 1<<16
		jz		short UFLOW

		// greater than or equal to 1 is an overflow
		sub		esi, 1<<16
		jmp		short DONE_Y
UFLOW:
		cmp		esi, 0
		jge		short DONE_Y
		add		esi, 1<<16
DONE_Y:

		// calc the new end point and line length
		mov		[ecx]TFlat.fxXDifference, esi
		mov		eax, [edx]CDrawPolygonBase.fxLineLength.i4Fx
		mov		edi, [edx]CDrawPolygonBase.iLineStartIndex
		add		eax, [edx]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		edi, gsGlobals.u4LinePixels
		mov     [edx]CDrawPolygonBase.fxLineLength.i4Fx, eax


		mov		eax, [edx]CDrawPolygonBase.iY
		mov		esi, [edx]CDrawPolygonBase.iYTo
		inc		eax
		mov     [edx]CDrawPolygonBase.iLineStartIndex, edi
		mov		[edx]CDrawPolygonBase.iY,eax
		cmp		eax, esi
		jl		short NEXT_SCAN
	}
}


//*****************************************************************************************
//
// Flat shaded stippled 16 bit.
//
void DrawSubtriangle(TFlatStipple* pscan, CDrawPolygon<TFlatStipple>* pdtri)
{
	::fixed fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
	::fixed fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;

	_asm
	{
		mov		edx,pdtri
		mov		ecx,pscan

		mov		eax,[edx]CDrawPolygonBase.iY
		mov		edi,[edx]CDrawPolygonBase.iLineStartIndex
		//		edi, eax and edx are also set from the loop end

NEXT_SCAN:
		// do we have to skip this scan?? (Is it an odd scan)
		mov		ebx,bEvenScanlinesOnly

		and		ebx,eax
		jnz		SKIP_LOOP

		//
		// Scan line always goes to the left.
		//
		mov		ebx,[ecx]TFlatStipple.fxX.i4Fx					// Fractional X.

		mov		esi,ebx											// Copy of fractional X.
		add		ebx,[edx]CDrawPolygonBase.fxLineLength.i4Fx

		sar		esi,16											// Integer X.

		sar		ebx,16											// Integer x+width
		xor		eax,esi											// Integer Y XOR integer X.

		add		edi,ebx											// Add iLineStartIndex.
		and		eax,1											// (y xor x) & 1

		sub		esi,ebx											// Width in esi
		jge		SKIP_LOOP

		mov		bx,u2AlphaColourReference						// Load colour value.
		add		edi,edi											// edi * 2.

		add		edi,gsGlobals.pvScreen							// Add base of screen.

		// if ((y xor x) & 1) skip a pixel.
		add		esi,eax
		jz		SKIP_LOOP

INNER_LOOP:
		mov		[edi + esi*2],bx			// Store pixel

		add		esi,2						// Step by two
		jl		INNER_LOOP					// Loop

SKIP_LOOP:
		mov		ecx,pscan

		mov		eax,fx_inc
		mov		esi,[ecx]TFlatStipple.fxXDifference
		add		[ecx]TFlatStipple.fxX,eax
		add		esi,fx_diff

		cmp		esi,(1<<16)
		jz		UFLOW

		// greater than or equal to 1 is an overflow
		sub		esi,(1<<16)
		jmp		DONE_Y

UFLOW:
		cmp		esi, 0
		jge		DONE_Y

		add		esi,(1<<16)

DONE_Y:
		// calc the new end point and line length
		mov		[ecx]TFlatStipple.fxXDifference,esi
		mov		eax,[edx]CDrawPolygonBase.fxLineLength.i4Fx
		mov		edi,[edx]CDrawPolygonBase.iLineStartIndex
		add		eax,[edx]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		edi,gsGlobals.u4LinePixels
		mov     [edx]CDrawPolygonBase.fxLineLength.i4Fx, eax

		mov		eax,[edx]CDrawPolygonBase.iY
		mov		esi,[edx]CDrawPolygonBase.iYTo
		inc		eax
		mov     [edx]CDrawPolygonBase.iLineStartIndex,edi
		mov		[edx]CDrawPolygonBase.iY,eax

		cmp		eax,esi
		jl		NEXT_SCAN
	}
}


//*****************************************************************************************
//
// Flat shaded 8 bit.
//
void DrawSubtriangle(TFlat8* pscan, CDrawPolygon<TFlat8>* pdtri)
{
	Assert(pscan);
	Assert(pdtri);

	int i_x_from;
	int i_x_to;
	int i_screen_index;
	//int i_pixel;

	// Get the screen colour and make a dword from it
	uint8  pix_screen  = (TFlat8::TPixel)TFlat8::TMap::u4GetConstColour();
	uint32 pix32 = pix_screen | (pix_screen<<8);
	pix32 = pix32 | (pix32<<16);
	uint8* ppix_screen = (uint8*)pdtri->prasScreen->pSurface;


	
	do
	{
		if (bEvenScanlinesOnly && ((pdtri->iY & 1) == 0))
			goto SKIP;

		if (bRIGHT_TO_LEFT)
		{
			i_x_from = (pscan->fxX.i4Fx - pdtri->fxLineLength.i4Fx)>>16;
			i_x_to   = (pscan->fxX.i4Fx)>>16;
			i_screen_index = pdtri->iLineStartIndex + i_x_from;// - 1;

			_asm
			{
				mov		esi, [i_x_to]
				mov		edi, i_screen_index

				sub		esi, [i_x_from]
				jle		short SKIP
			}
		}
		else
		{

			i_x_from = (pscan->fxX.i4Fx)>>16;
			i_x_to   = (pscan->fxX.i4Fx + pdtri->fxLineLength.i4Fx)>>16;
			i_screen_index = pdtri->iLineStartIndex + i_x_to;
			__asm
			{
				mov		esi, [i_x_from]
				mov		edi, i_screen_index

				sub		esi, [i_x_to]
				jge		short SKIP
			}
		}

		//
		// PLACE NO CODE HERE....
		// I ASSUME THE FLAGS ARE SET FROM THE ABOVE ASSEMBLY
		//

		__asm
		{
			jl		short GO_LEFT
			add		edi,esi
			neg		esi
GO_LEFT:
			mov		ecx, esi
			add		edi, [ppix_screen]

			add		ecx,edi
			mov		eax, pix32
			and		ecx,3
			jz		short ENTER_LOOP

DW_ALIGN:
			mov		[edi+esi], al
			inc		esi
			jz		short SKIP
			dec		ecx
			jnz		short DW_ALIGN

ENTER_LOOP:		
			mov		ecx,esi
LOOP_8BIT:
			add		esi,16			
			jg		short END_LOOP

			mov		[edi + ecx], eax
			mov		[edi + ecx+4], eax	
			mov		[edi + ecx+8], eax
			mov		[edi + ecx+12], eax

			add		ecx,16					// this will not change the flags, result same as add esi+8
			jnz		short LOOP_8BIT			// there are more pixels
			jmp		short SKIP				// we are done
END_LOOP:
			mov		[edi + ecx], al
			inc		ecx
			jnz		short END_LOOP
		}
SKIP:
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
//
template<class T> void DrawLineT(T* pt, int i_a, int i_b, T t_fill)
//
// Function used to draw a horizontal line.
//
//**************************************
{
	// Always draw left to right.
	if (i_a > i_b)
		Swap(i_a, i_b);

	// Fill the line.
	for (int i = i_a; i <= i_b; i++)
		pt[i] = t_fill;
}


//*****************************************************************************************
//
// 16 bit wire frame.
//
void DrawSubtriangle(TWire* pscan, CDrawPolygon<TWire>* pdtri)
{
	Assert(pscan);
	Assert(pdtri);

	uint16 u2_fill;				// Fill colour.
	bool   b_first_line = true;	// Flag indicating that this is the first scanline
								// rendered.
	int    i_min_y = pdtri->prpolyPoly->v2ScreenMinInt.tY;
	int    i_max_y = pdtri->prpolyPoly->v2ScreenMaxInt.tY;
	int    i_x_from;
	int    i_x_to;
	int    i_x_from_last;
	int    i_x_to_last;
	int    i_width = pdtri->prasScreen->iWidth;

	//
	// Get the min and max y values.
	//
	i_min_y = i_max_y = pdtri->prpolyPoly->paprvPolyVertices[0]->iYScr;
	for (int i_vert = 1; i_vert < int(pdtri->prpolyPoly->paprvPolyVertices.uLen); i_vert++)
	{
		int i_y = pdtri->prpolyPoly->paprvPolyVertices[i_vert]->iYScr;
		i_min_y = Min(i_min_y, i_y);
		i_max_y = Max(i_max_y, i_y);
	}
	--i_max_y;

#if (VER_DEBUG)
	// Set the fill colour according to whether the polygon was clipped by depth sorting.
	if (pdtri->prpolyPoly->bClippedByDepthSort)
	{
		// Fill with red.
		u2_fill = 0xF800;
	}
	else
#endif
	{
		// Fill with green.
		u2_fill = 0x07E0;
	}


	//
	// Iterate through the scanlines that intersect the subtriangle.
	//
	do
	{
		if (bRIGHT_TO_LEFT)
		{
			i_x_from = (pscan->fxX.i4Fx - pdtri->fxLineLength.i4Fx) >> 16;
			i_x_to   = pscan->fxX.i4Fx >> 16;
		}
		else
		{
			i_x_from = pscan->fxX.i4Fx >> 16;
			i_x_to   = (pscan->fxX.i4Fx + pdtri->fxLineLength.i4Fx) >> 16;
		}
		SetMinMax(i_x_from, 0, i_width - 1);
		SetMinMax(i_x_to, 0, i_width - 1);

		// Draw if there are pixels to draw.
		if (i_x_to > i_x_from)
		{
			//
			// Start the scanline.
			//

			// Get the start address for the scanline on the destination raster.
			uint16* pu2 = ((uint16*)pdtri->prasScreen->pSurface) + pdtri->iLineStartIndex;

			if (pdtri->iY <= i_min_y || pdtri->iY >= i_max_y)
			{
				DrawLineT(pu2, i_x_from, i_x_to, u2_fill);
				b_first_line = false;
			}
			else
			{
				if (b_first_line)
				{
					pu2[i_x_from] = u2_fill;
					pu2[i_x_to]   = u2_fill;
					b_first_line = false;
				}
				else
				{
					DrawLineT(pu2, i_x_from, i_x_from_last, u2_fill);
					DrawLineT(pu2, i_x_to, i_x_to_last, u2_fill);
				}
			}

			// Store last values.
			i_x_from_last = i_x_from;
			i_x_to_last   = i_x_to;
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


//#endif // VER_ASM
