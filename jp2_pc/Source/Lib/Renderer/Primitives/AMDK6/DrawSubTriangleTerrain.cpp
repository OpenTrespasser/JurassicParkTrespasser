/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function for terrain.
 *		Optimized for the AMD K6-3D processor.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/AMDK6/DrawSubTriangleTerrain.cpp              $
 * 
 * 6     10/01/98 12:41a Asouth
 * Explicit fixed; #ifdefs for MW build
 * 
 * 5     98.04.08 8:42p Mmouni
 * Added fogged dithered flat shaded primitive.
 * 
 * 4     3/16/98 7:54p Mmouni
 * Fixed a problem with incorrect shading values.
 * 
 * 3     3/11/98 9:05p Mmouni
 * Perspective primitive now does QWORD writes.
 * 
 * 2     97.11.12 1:28p Mmouni
 * Aligned inner loops for optimal pre-decode.
 * 
 * 1     97.11.11 9:49p Mmouni
 * Optimized for K6-3D.
 * 
 * 2     97.11.07 5:44p Mmouni
 * Added dithering support.
 * 
 * 1     97/11/06 4:48p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/Fog.hpp"
#include "Lib/Renderer/LightBlend.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"


// Put this stuff in a seperate section for alignment.
#pragma code_seg("StriDTerr")


//
// The number of integral bits of the fog shading values.
//
#define LOG2_NUM_SHADES		3

//
// The mask for the combined screen y, screen x, shade value.
//
#define SY_SX_MASK (0xFFFFFF60 << (LOG2_NUM_SHADES+2))


#if (DITHER_SIZE == 2)


//*****************************************************************************************
//
// Dithered, alpha-fogged, perspective texture.
//
// Notes:
//	The inner loop keeps both the current intensity and the low bit of screen y and 
//	screen x in one register.  The contents of the register are layed out as follows:
//	00000000000YXSSSFFFFFFFFFFFFFFFF.
//
void DrawSubtriangle(TGFogPersp* pscan, CDrawPolygon<TGFogPersp>* pdtri)
{
	typedef TGFogPersp::TPixel prevent_internal_compiler_errors;
	static void* pvAlphaDitherTable;
	::fixed fx_inc;
	::fixed fx_diff;
	::fixed fx_gour_inc;

	TGFogPersp* plinc = &pdtri->pedgeBase->lineIncrement;

	//
	// TGFogPersp with CGouraudOn substitued for CGouraudFog.
	// This is necessary so that we can access members of CGouraudOn.
	//
	typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMapTexture<uint16>, CIndexPerspective, CColLookupOff> FakeTGFogPersp;
	typedef FakeTGFogPersp::TPixel prevent_internal_compiler_errors2;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		//
		// Local copies of edge stepping values.
		//
		// fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;
		// fx_gour_inc = pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		// f_inc_uinvz = pdtri->pedgeBase->lineIncrement.indCoord.fUInvZ;
		// f_inc_vinvz = pdtri->pedgeBase->lineIncrement.indCoord.fVInvZ;
		// f_inc_invz  = pdtri->pedgeBase->lineIncrement.indCoord.fInvZ;
		//
		mov		eax,[plinc]
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.
		mov		ebx,[eax]TGFogPersp.fxX

		mov		edx,[eax]TGFogPersp.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TGFogPersp.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TGFogPersp.indCoord.fInvZ
		movq	[tvEdgeStep.UInvZ],mm0

		mov		edx,[eax]FakeTGFogPersp.gourIntensity.fxIntensity
		mov		[tvEdgeStep.InvZ],ebx

		mov		fx_gour_inc,edx
	
		//
		// Setup:
		// QWORD qShadeSlopes	= fxDeltaXIntensity*2,fxDeltaXIntensity*2
		// QWORD qMFactor		= width,1,width,1
		// Load global texture values. 
		//
		movd		mm2,[fxDeltaXIntensity]

		punpckldq	mm2,mm2									// fxDeltaXIntensity,fxDeltaXIntensity
		movd		mm3,[iTexWidth]							// 0,0,0,Width

		paddd		mm2,mm2									// fxDeltaXIntensity*2,fxDeltaXIntensity*2
		movd		mm4,[u4OneOne]							// 0,0,1,1

		movq		[qShadeSlopes],mm2
		punpcklwd	mm3,mm3									// 0,0,Width,Width

		punpcklwd	mm4,mm3									// Width,1,Width,1
		movq		mm0,[ecx]TGFogPersp.indCoord.fUInvZ		// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4							// Save mfactor
		movd		mm1,[ecx]TGFogPersp.indCoord.fInvZ		// mm1 = (?,1/Z)

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// esi,ecx,mm0,mm1 setup. 
		//
Y_LOOP:
		mov		eax,[esi]CDrawPolygonBase.iY
		mov		ebx,[bEvenScanlinesOnly]
		push	ebp									// Save ebp.

		// Check to see if we sould skip this scanline.
		and		eax,ebx
		jnz		END_OF_SCANLINE

		//
		// Setup:
		// QWORD qShadeValue	= fxIntensity+1,fxIntensity
		// pvDither = lbAlphaTerrain.au4AlphaRefDither + low 2 bits of Y + low 2 bits of X.
		//
		mov		edi,[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx

		mov		dword ptr[qShadeValue],edi
		mov		eax,[esi]CDrawPolygonBase.iY			// Get current integer y.

		add		edi,[fxDeltaXIntensity]

		mov		ebx,[ecx]TGFogPersp.fxX.i4Fx			// Get current fixed point x.
		mov		dword ptr[qShadeValue+4],edi

		shr		ebx,(16-(LOG2_NUM_SHADES+4))			// Shift low bit of x into position.
		and		eax,1									// Mask low bit of y.
	
		shl		eax,(LOG2_NUM_SHADES+5)		 			// Shift low bit of y into position.
		and		ebx,(1 << (LOG2_NUM_SHADES+4))			// Mask shifted low bit of x.	

		mov		edi,lbAlphaTerrain.au4AlphaRefDither	// Intensity to Alpha+Dither table pointer.
		or		eax,ebx									// Combine low bits of x and y.

		add		edi,eax									// Combine low bits of x and y with pointer.
		mov		[pvAlphaDitherTable],edi				// Save pointer + y + x value.

		//
		// Setup for prespective correction and looping.
	#ifdef __MWERKS__
		#define SLTYPE TGFogPersp
		#include "psprologue3.inl"
	#else
		PERSPECTIVE_SCANLINE_PROLOGUE_3DX(TGFogPersp)
	#endif

SUBDIVISION_LOOP:
		//--------------------------------------------------------------------------- 
		// Draw the subdivision.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel/alpha index	ebx = alpha/texel index
		// ecx = alpha_base_ptr		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = clut_base_ptr
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = uvmask 
		// mm4 = shade+1,shade		mm5 = shadeslope*2,shadeslope*2
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		lea			ebp,lbAlphaTerrain.au2Colour	// Alpha color to color table.
		movq		mm0,mm7							// Copy U1,V1

		dec			ebp								// Hack to force SIB+offset in loop.
		paddd		mm7,mm6							// Step U,V

		movq		mm2,[qMFactor]					// Load texture multipliers
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm0,16							// Shift for integer U1,V1

		movq		mm4,[qShadeValue]				// Load shading value
		psrad		mm1,16							// Shift for integer U2,V2

		movq		mm5,[qShadeSlopes]				// Load shading slopes
		packssdw	mm0,mm1							// Pack integer texture values

		movq		mm1,mm4							// Copy shading values

		psrld		mm1,14							// Shift shading values
		paddd		mm4,mm5							// Step shading values

		pmaddwd		mm0,mm2							// Compute texture indices
		movq		[qShadeTemp],mm1				// Save shading indices

		dec			esi								// Hack to force SIB+offset in loop.
		mov			ecx,[pvAlphaDitherTable]		// Intensity to Alpha+Dither table.

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 0 past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16

		//
		// Do two pixels at a time.
		//
		// mm0 = Texture indices.
		// qShadeTemp = CLUT indices.
		// mm4 is stepped once.
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U1,V1
		
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		movq		mm1,mm7							// Copy U2,V2

		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		psrad		mm1,16							// Shift for integer U2,V2

		xor			ecx,(1 << (LOG2_NUM_SHADES+4))	// Step screen x.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2

		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1

		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 2
		movq		mm1,mm4							// Copy shading values.

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2
		paddd		mm4,mm5							// Step shading values.

		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		psrld		mm1,14							// Shift shading values (leave 4 bits of fraction).

		xor			ecx,(1 << (LOG2_NUM_SHADES+4))	// Step screen x.
		movq		[qShadeTemp],mm1				// Save shading indicies.

		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for p1

		mov			[edx + edi*2 - 2],bx			// Store pixel 2

		add			edi,2							// Increment index by 2
		jl			short INNER_LOOP_2P				// Loop (while >2 pixels)

FINISH_REMAINDER:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			short ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture co-ordinates once.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		[qShadeValue],mm4				// Save intensity value.
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		xor			ecx,(1 << (LOG2_NUM_SHADES+4))	// Step screen x.
		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 2
		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2
		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		xor			ecx,(1 << (LOG2_NUM_SHADES+4))	// Step screen x.
		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for p1
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		mov			[pvAlphaDitherTable],ecx		// Save intensity to Alpha+Dither table.
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			short END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		psrad		mm5,1							// Reduce shade step to 1x.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		psubd		mm4,mm5							// Backup shading values by 1.
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		movq		[qShadeValue],mm4				// Save intensity value.
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		xor			ecx,(1 << (LOG2_NUM_SHADES+4))	// Step screen x.
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1
		mov			[pvAlphaDitherTable],ecx		// Save intensity to Alpha+Dither table.
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		mov			eax,[i_pixel]					// Load remaining length.

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		je		short END_OF_SCANLINE

		//
		// Do perspective correction for next span.
	#ifdef __MWERKS__
		#include "psepilogue3.inl"
	#else
		PERSPECTIVE_SCANLINE_EPILOGUE_3DX
	#endif

		jmp		SUBDIVISION_LOOP

END_OF_SCANLINE:
		pop		ebp								// Restore ebp.

		//		
		// Increment the base edge.
		//
		mov		esi,[pdtri]						// Pointer to polygon object.

		mov		ecx,[pscan]						// Pointer to scanline object.

		// Step length, line starting address.
		mov		edx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,[esi]CDrawPolygonBase.iLineStartIndex

		add		edx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx

		add		edi,gsGlobals.u4LinePixels

		mov		eax,[ecx]TGFogPersp.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TGFogPersp.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TGFogPersp.indCoord.fUInvZ

		movd	mm1,[ecx]TGFogPersp.indCoord.fInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm3,[tvEdgeStep.InvZ]

		pfadd	(m0,m2)
		mov		edx,[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx

		pfadd	(m1,m3)
		mov		edi,[fx_diff]

		add		edx,[fx_gour_inc]

		add		eax,[fx_inc]

		add		ebx,edi
		jge		short NO_UNDERFLOW

		// Underflow.
		mov		edi,[fxIModDiv]
		add		ebx,0x00010000

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short FINISH_LOOPING

		// Overflow.
		mov		edi,[fxIModDiv]
		sub		ebx,0x00010000

		add		edx,edi

BORROW_OR_CARRY:
		// Add borrow/carry adjustments to U,V,Z.
		movq	mm2,[tvNegAdj.UInvZ]

		movd	mm3,[tvNegAdj.InvZ]

		pfadd	(m0,m2)

		pfadd	(m1,m3)

FINISH_LOOPING:
		// Save values and Y Loop control.
		movq	[ecx]TGFogPersp.indCoord.fUInvZ,mm0

		movd	[ecx]TGFogPersp.indCoord.fInvZ,mm1

		mov		[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx,edx

		mov		[ecx]TGFogPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TGFogPersp.fxXDifference.i4Fx,ebx
		mov		ebx,[esi]CDrawPolygonBase.iYTo

		inc		edx
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}


//*****************************************************************************************
//
// Dithered, alpha-fogged, linear texture.
//
void DrawSubtriangle(TGFogLinear* pscan, CDrawPolygon<TGFogLinear>* pdtri)
{
	typedef TGFogLinear::TPixel prevent_internal_compiler_errors;
	static CDrawPolygon<TGFogLinear>* pdtriGlbl;
	static TGFogLinear* pscanGlbl;
	static ::fixed fx_gour_inc;
	static PackedFloatUV pf_texture_inc;

	TGFogLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	//
	// TGFogLinear with CGouraudOn substitued for CGouraudFog.
	// This is necessary so that we can access members of CGouraudOn.
	//
	typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMapTexture<uint16>, CIndexLinear, CColLookupOff> FakeTGFogLinear;
	typedef FakeTGFogLinear::TPixel prevent_internal_compiler_errors2;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.

		mov		esi,[pdtri]							// Pointer to polygon object.
		push	ebp									// Save ebp.

		mov		eax,[pscan]							// Pointer to scanline object.
		mov		pdtriGlbl,esi

		//
		// Local copies of edge stepping values.
		//
		// fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
		// pf_texture_inc	= pdtri->pedgeBase->lineIncrement.indCoord.pfIndex;
		// pdtriGlbl		= pdtri;
		// pscanGlbl		= pscan;
		// fx_gour_inc		= pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		//
		mov		ebx,[edi]TGFogLinear.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TGFogLinear.fxXDifference
		mov		[fx_inc],ebx

		movq	mm0,[edi]TGFogLinear.indCoord.pfIndex
		mov		[fx_diff],ecx

		mov		ebx,[edi]FakeTGFogLinear.gourIntensity.fxIntensity
		movq	[pf_texture_inc],mm0

		mov		[fx_gour_inc],ebx

		//
		// Setup:
		//
		// mm2 = qMFactor = width,1,width,1
		// mm5 = qShadeSlopes = fxDeltaXIntensity*2,fxDeltaXIntensity*2
		// mm6 = uvslope
		//
		movd		mm5,[fxDeltaXIntensity]

		movd		mm0,[iTexWidth]					// 0,0,0,Width
		punpckldq	mm5,mm5							// fxDeltaXIntensity,fxDeltaXIntensity

		movd		mm2,[u4OneOne]					// 0,0,1,1
		paddd		mm5,mm5							// fxDeltaXIntensity*2,fxDeltaXIntensity*2

		punpcklwd	mm0,mm0							// 0,0,Width,Width

		movq		mm6,[pfxDeltaTex]				// Load texture slopes.
		punpcklwd	mm2,mm0							// Width,1,Width,1

		movq		mm7,[eax]TGFogLinear.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		ebp,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TGFogLinear.fxX.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		mov		edx,gsGlobals.pvScreen

		add		ebx,ecx
		sar		edi,16

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex
		sar		ebx,16

		add		ecx,ebx

		lea		edx,[edx + ecx*2]

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel/alpha index	ebx = alpha/texel index
		// ecx = alpha_base_ptr		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = clut_base_ptr
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = temp
		// mm4 = shade+1,shade		mm5 = shadeslope*2,shadeslope*2
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		// Setup:
		// QWORD qShadeValue	= fxIntensity+1,fxIntensity
		// pvDither = lbAlphaTerrain.au4AlphaRefDither + low 2 bits of Y + low 2 bits of X.
		//
		movd		mm4,[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx
		pf2id		(m7,m7)							// Convert texture values to 16.16 fixed.

		mov			ecx,[esi]CDrawPolygonBase.iY			// Get current integer y.

		mov			ebx,[eax]TGFogPersp.fxX.i4Fx			// Get current fixed point x.

		shr			ebx,(16-(LOG2_NUM_SHADES+4))			// Shift low bit of x into position.
		and			ecx,1									// Mask low bit of y.
	
		shl			ecx,(LOG2_NUM_SHADES+5)					// Shift low bit of y into position.
		and			ebx,(1 << (LOG2_NUM_SHADES+4))			// Mask shifted low bit of x.	

		or			ecx,ebx									// Combine low bits of x and y.
		mov			ebx,lbAlphaTerrain.au4AlphaRefDither	// Intensity to Alpha+Dither table pointer.

		add			ecx,ebx									// Combine low bits of x and y with pointer.

		movd		mm1,[fxDeltaXIntensity]			// Load intensity delta.
		movq		mm0,mm7							// Copy U1,V1

		punpckldq	mm4,mm4							// Duplicate intensity value.
		paddd		mm7,mm6							// Step U,V

		mov			esi,[pvTextureBitmap]			// Load texture ponter.
		psllq		mm1,32							// Shift to upper dword.

		lea			ebp,lbAlphaTerrain.au2Colour	// Load clut base pointer.
		paddd		mm4,mm1							// Add to starting intensity.

		movq		mm1,mm7							// Copy U2,V2
		psrad		mm0,16							// Shift for integer U1,V1

		dec			ebp								// Hack to force SIB+offset in loop.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values
		movq		mm1,mm4							// Copy shading values

		psrld		mm1,14							// Shift shading values

		paddd		mm4,mm5							// Step shading values
		pmaddwd		mm0,mm2							// Compute texture indices

		dec			esi								// Hack to force SIB+offset in loop.
		movq		[qShadeTemp],mm1				// Save shading indicies

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 0 past the beginning of a cache line.
		ALIGN	16

		//
		// Do two pixels at a time.
		//
		// mm0 = Texture indices.
		// qShadeTemp = CLUT indices.
		// mm4 is stepped once.
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U1,V1
		
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		movq		mm1,mm7							// Copy U2,V2

		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		psrad		mm1,16							// Shift for integer U2,V2

		xor			ecx,(1 << (LOG2_NUM_SHADES+4))	// Step screen x.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2

		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1

		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 2
		movq		mm1,mm4							// Copy shading values.

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2
		paddd		mm4,mm5							// Step shading values.

		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		psrld		mm1,14							// Shift shading values (leave 4 bits of fraction).

		xor			ecx,(1 << (LOG2_NUM_SHADES+4))	// Step screen x.
		movq		[qShadeTemp],mm1				// Save shading indicies.

		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for p2

		mov			[edx + edi*2 - 2],bx			// Store pixel 2

		add			edi,2							// Inrcement index by 2
		jl			short INNER_LOOP_2P				// Loop (while >2 pixels)

FINISH_REMAINDER:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			short ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture/shade co-ordinates once.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		xor			ecx,(1 << (LOG2_NUM_SHADES+4))	// Step screen x.
		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 2
		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2
		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for p2
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TGFogLinear.indCoord.pfIndex

		mov		ebp,[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx

		mov		ebx,[eax]TGFogLinear.fxXDifference
		pfadd	(m7,m1)

		add		ebp,[fx_gour_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000
		movq	mm1,[pfNegD]

		add		ebp,[fxIModDiv]
		pfadd	(m7,m1)

		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000
		movq	mm1,[pfNegD]

		add		ebp,[fxIModDiv]
		pfadd	(m7,m1)

EDGE_DONE:
		// Store modified variables and do looping.
		// Step length, line starting address.
		mov		esi,[pdtriGlbl]
		mov		[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx,ebp

		mov		[eax]TGFogLinear.fxXDifference,ebx
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		movq	[eax]TGFogLinear.indCoord.pfIndex,mm7
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx

		add		ecx,gsGlobals.u4LinePixels
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx

		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx
		mov		ecx,[eax]TGFogLinear.fxX

		mov		ebp,[fx_inc]

		mov		edx,[esi]CDrawPolygonBase.iY
		add		ecx,ebp

		mov		[eax]TGFogLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp

		// Done with MMX/3DX.
		femms
	}
}


#elif (DITHER_SIZE == 4)  // if (DITHER_SIZE == 2)


//*****************************************************************************************
//
// Dithered, alpha-fogged, perspective texture.
//
// Notes:
//	The inner loop keeps both the current intensity and 2 bits of the current screen x
//	and curren screen y value in one register.  The contents of the register are layed
//	out as follows: 0000000YY0XX0SSSFFFFFFFFFFFFFFFF.  Where YY is the lower two bits
//  of the current screen y, XX is the lower 2 bits of the current x, SSS is the 
//  integral part of the shading value (more bits would be used for more than 8 shades),
//	and the F's are the fractional part oft the shading values.
//
void DrawSubtriangle(TGFogPersp* pscan, CDrawPolygon<TGFogPersp>* pdtri)
{
	typedef TGFogPersp::TPixel prevent_internal_compiler_errors;
	static void* pvAlphaDitherTable;
	::fixed fx_inc;
	::fixed fx_diff;
	::fixed fx_gour_inc;

	TGFogPersp* plinc = &pdtri->pedgeBase->lineIncrement;

	//
	// TGFogPersp with CGouraudOn substitued for CGouraudFog.
	// This is necessary so that we can access members of CGouraudOn.
	//
	typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMapTexture<uint16>, CIndexPerspective, CColLookupOff> FakeTGFogPersp;
	typedef FakeTGFogPersp::TPixel prevent_internal_compiler_errors2;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		//
		// Local copies of edge stepping values.
		//
		// fx_inc  = pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff = pdtri->pedgeBase->lineIncrement.fxXDifference;
		// fx_gour_inc = pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		// f_inc_uinvz = pdtri->pedgeBase->lineIncrement.indCoord.fUInvZ;
		// f_inc_vinvz = pdtri->pedgeBase->lineIncrement.indCoord.fVInvZ;
		// f_inc_invz  = pdtri->pedgeBase->lineIncrement.indCoord.fInvZ;
		//
		mov		eax,[plinc]
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.
		mov		ebx,[eax]TGFogPersp.fxX

		mov		edx,[eax]TGFogPersp.fxXDifference
		mov		fx_inc,ebx

		movq	mm0,[eax]TGFogPersp.indCoord.fUInvZ
		mov		fx_diff,edx

		mov		ebx,[eax]TGFogPersp.indCoord.fInvZ
		movq	[tvEdgeStep.UInvZ],mm0

		mov		edx,[eax]FakeTGFogPersp.gourIntensity.fxIntensity
		mov		[tvEdgeStep.InvZ],ebx

		mov		fx_gour_inc,edx
	
		//
		// Setup:
		// QWORD qShadeSlopes	= fxDeltaXIntensity*2,fxDeltaXIntensity*2
		// QWORD qMFactor		= width,1,width,1
		// Load global texture values. 
		//
		movd		mm2,[fxDeltaXIntensity]

		punpckldq	mm2,mm2									// fxDeltaXIntensity,fxDeltaXIntensity
		movd		mm3,[iTexWidth]							// 0,0,0,Width

		paddd		mm2,mm2									// fxDeltaXIntensity*2,fxDeltaXIntensity*2
		movd		mm4,[u4OneOne]							// 0,0,1,1

		movq		[qShadeSlopes],mm2
		punpcklwd	mm3,mm3									// 0,0,Width,Width

		punpcklwd	mm4,mm3									// Width,1,Width,1
		movq		mm0,[ecx]TGFogPersp.indCoord.fUInvZ		// mm0 = (V/Z,U/Z)

		movq		[qMFactor],mm4							// Save mfactor
		movd		mm1,[ecx]TGFogPersp.indCoord.fInvZ		// mm1 = (?,1/Z)

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
		// esi,ecx,mm0,mm1 setup. 
		//
Y_LOOP:
		mov		eax,[esi]CDrawPolygonBase.iY
		mov		ebx,[bEvenScanlinesOnly]
		push	ebp									// Save ebp.

		// Check to see if we sould skip this scanline.
		and		eax,ebx
		jnz		END_OF_SCANLINE

		//
		// Setup:
		// QWORD qShadeValue	= fxIntensity+1,fxIntensity
		// pvDither = lbAlphaTerrain.au4AlphaRefDither + low 2 bits of Y + low 2 bits of X.
		//
		mov		edi,[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx

		mov		dword ptr[qShadeValue],edi
		mov		eax,[esi]CDrawPolygonBase.iY			// Get current integer y.

		add		edi,[fxDeltaXIntensity]

		mov		ebx,[ecx]TGFogPersp.fxX.i4Fx			// Get current fixed point x.
		mov		dword ptr[qShadeValue+4],edi

		shr		ebx,(16-(LOG2_NUM_SHADES+7))			// Shift low 2 bits of x into position.
		and		eax,3									// Mask low 2 bits of y.
	
		shl		eax,(LOG2_NUM_SHADES+10)				// Shift low 2 bits of y into position.
		and		ebx,(3 << (LOG2_NUM_SHADES+7))			// Mask shifted low 2 bits of x.	

		mov		edi,lbAlphaTerrain.au4AlphaRefDither	// Intensity to Alpha+Dither table pointer.
		or		eax,ebx									// Combine low bits of x and y.

		add		edi,eax									// Combine low bits of x and y with pointer.
		mov		[pvAlphaDitherTable],edi				// Save pointer + y + x value.

		//
		// Setup for prespective correction and looping.
		//
	#ifdef __MWERKS__
		#define SLTYPE TGFogPersp
		#include "psprologue3.inl"
	#else
		PERSPECTIVE_SCANLINE_PROLOGUE_3DX(TGFogPersp)
	#endif

SUBDIVISION_LOOP:
		//--------------------------------------------------------------------------- 
		// Draw the subdivision.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel/alpha index	ebx = alpha/texel index
		// ecx = alpha_base_ptr		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = clut_base_ptr
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = temp
		// mm4 = shade+1,shade		mm5 = shadeslope*2,shadeslope*2
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		lea			ebp,lbAlphaTerrain.au2Colour	// Alpha color to color table.
		movq		mm0,mm7							// Copy U1,V1

		dec			ebp								// Hack to force SIB+offset in loop.
		paddd		mm7,mm6							// Step U,V

		movq		mm2,[qMFactor]					// Load texture multipliers
		movq		mm1,mm7							// Copy U2,V2

		psrad		mm0,16							// Shift for integer U1,V1

		movq		mm4,[qShadeValue]				// Load shading value
		psrad		mm1,16							// Shift for integer U2,V2

		movq		mm5,[qShadeSlopes]				// Load shading slopes
		packssdw	mm0,mm1							// Pack integer texture values

		movq		mm3,mm4							// Copy shading values

		psrld		mm3,12							// Shift shading values
		paddd		mm4,mm5							// Step shading values

		pmaddwd		mm0,mm2							// Compute texture indices
		movq		[qShadeTemp],mm3				// Save shading indices

		dec			esi								// Hack to force SIB+offset in loop.
		mov			ecx,[pvAlphaDitherTable]		// Intensity to Alpha+Dither table.

		//
		// Align to a quadword boundry.
		//
		lea			eax,[edx + edi*2]
		mov			ebx,3

		test		eax,ebx
		jz			ALIGNED2

		inc			edi								// One pixel left?
		jz			ONE_PIXEL_LEFT

		//
		// Do one pixel for alignment.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U3,V3

		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U3,V3.
		
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		packssdw	mm1,mm0							// Pack as U3V3 and U2V2.

		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		movq		mm0,mm1							// Put back into mm0.
	
		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		movq		mm3,mm5							// Copy shading slopes (*2).

		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		psrad		mm3,1							// Half them.

		and			ecx,SY_SX_MASK					// Mask carry.
		movq		mm1,mm4							// Copy shading values.

		paddd		mm4,mm3							// Step next shading values by one.
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1

		psubd		mm1,mm3							// Backup previous next shading values.

		mov			[edx + edi*2 - 2],ax			// Store pixel 1

		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).
		pmaddwd		mm0,mm2							// Compute texture indices

		movq		[qShadeTemp],mm1				// Save shading indicies.
		lea			eax,[edx + edi*2]

ALIGNED2:
		and			eax,7
		jz			ALIGNED4

		add			edi,2							// Two pixels left?
		jge			FINISH_REMAINDER2

		//
		// Do two pixels for alignment.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U1,V1
		
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		movq		mm1,mm7							// Copy U2,V2

		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		psrad		mm1,16							// Shift for integer U2,V2

		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2
		and			ecx,SY_SX_MASK					// Mask carry.

		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1
		movq		mm1,mm4							// Copy shading values.

		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 2
		paddd		mm4,mm5							// Step shading values.

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2
		pmaddwd		mm0,mm2							// Compute texture indicies.

		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).

		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		movq		[qShadeTemp],mm1				// Save shading indicies.

		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.

		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for p1
		and			ecx,SY_SX_MASK					// Mask carry.

		mov			[edx + edi*2 - 2],bx			// Store pixel 2

ALIGNED4:
		add		edi,4								// Four pixels left.
		jl		INNER_LOOP_4P

		jmp		FINISH_REMAINDER4

		// Align start of loop to ? past the beginning of a cache line.
		ALIGN	16
		nop
		ALIGN	16
		nop
		nop
		nop
		nop

		//
		// Do four pixels at a time.
		//
		// mm0 = Texture indices.
		// qShadeTemp = CLUT indices.
		// mm4 is stepped once.
		// mm7 is stepped once.
		//
INNER_LOOP_4P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U1,V1
		
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		movq		mm1,mm7							// Copy U2,V2

		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.

		and			ecx,SY_SX_MASK					// Mask carry.
		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2

		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for pixel 1
		movq		mm1,mm4							// Copy shading values.

		movd		mm3,eax							// Save pixel 1
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 2

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2
		paddd		mm4,mm5							// Step shading values.

		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).

		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		movq		[qShadeTemp],mm1				// Save shading indicies.

		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for pixel 2
		and			ecx,SY_SX_MASK					// Mask carry.

		movd		mm1,ebx							// Copy pixel 2
		paddd		mm7,mm6							// Step U,V

		punpcklwd	mm3,mm1							// Save pixel 2
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 3
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 3
		psrad		mm0,16							// Shift for integer U1,V1
		
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 3
		paddd		mm7,mm6							// Step U,V

		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		movq		mm1,mm7							// Copy U2,V2

		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		psrad		mm1,16							// Shift for integer U2,V2

		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 4
		and			ecx,SY_SX_MASK					// Mask carry.

		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p3
		movd		mm1,eax							// Copy pixel 3

		punpckldq	mm3,mm1							// Save pixel 3
		movq		mm1,mm4							// Copy shading values.

		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 4

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 4
		paddd		mm4,mm5							// Step shading values.

		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).

		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		movq		[qShadeTemp],mm1				// Save shading indicies.

		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for p4
		and			ecx,SY_SX_MASK					// Mask carry.

		nop											// Avoid degraded pre-decode.
		movd		mm1,ebx							// Copy pixel 4

		psllq		mm1,48							// Shift pixel 4 up.

		por			mm3,mm1							// Combine with other pixels.
		movq		[edx + edi*2 - 8],mm3			// Store 4 pixels.

		add			edi,4							// Increment index by 4
		jl			INNER_LOOP_4P					// Loop (while >4 pixels)

FINISH_REMAINDER4:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		jnz			THREE_OR_LESS

		test		eax,eax							// Two bytes for alignment.

		//
		// Four pixels left, step texture coords 3 times, shades once.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U1,V1
		
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		movq		mm1,mm7							// Copy U2,V2

		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values.
		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.

		and			ecx,SY_SX_MASK					// Mask carry.
		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2

		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for pixel 1
		movq		mm1,mm4							// Copy shading values.

		movd		mm3,eax							// Save pixel 1
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 2

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2
		paddd		mm4,mm5							// Step shading values.

		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).

		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		movq		[qShadeTemp],mm1				// Save shading indicies.

		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for pixel 2
		and			ecx,SY_SX_MASK					// Mask carry.

		movd		mm1,ebx							// Copy pixel 2
		paddd		mm7,mm6							// Step U,V

		punpcklwd	mm3,mm1							// Save pixel 2
		movq		[qIndexTemp],mm0				// Save texture indicies.

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 3

		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 3
		
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 3

		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.

		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.

		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.

		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 4
		and			ecx,SY_SX_MASK					// Mask carry.

		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p3
		movd		mm1,eax							// Copy pixel 3

		punpckldq	mm3,mm1							// Save pixel 3
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 4

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 4
		movq		[qShadeValue],mm4				// Save intensity value.
	
		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.

		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.

		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for p4
		and			ecx,SY_SX_MASK					// Mask carry.

		movd		mm1,ebx							// Copy pixel 4

		psllq		mm1,48							// Shift pixel 4 up.
		mov			[pvAlphaDitherTable],ecx		// Save intensity to Alpha+Dither table.

		por			mm3,mm1							// Combine with other pixels.
		movq		[edx - 8],mm3					// Store 4 pixels.

		mov			eax,[i_pixel]					// Load remaining length.
		jmp			END_OF_SPAN

THREE_OR_LESS:
		sub			edi,2
		jge			FINISH_REMAINDER2

		//
		// Three pixels left, step texture coords twice, shades by half.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U1,V1
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		paddd		mm7,mm6							// Step U,V
		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		movq		mm1,mm7							// Copy U2,V2
		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		psrad		mm1,16							// Shift for integer U2,V2
		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		packssdw	mm0,mm1							// Pack integer texture values.
		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2
		and			ecx,SY_SX_MASK					// Mask carry.
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1
		movq		mm1,mm4							// Copy shading values.
		mov			[edx - 6],ax					// Store pixel 1
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 2
		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2
		pmaddwd		mm0,mm2							// Compute texture indicies.
		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).
		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		movq		[qShadeTemp],mm1				// Save shading indicies.
		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for p1
		and			ecx,SY_SX_MASK					// Mask carry.
		mov			[edx - 4],bx					// Store pixel 2

		movq		[qIndexTemp],mm0				// Save texture indicies.
		psrad		mm5,1							// Reduce shade step to 1x.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 3
		paddd		mm4,mm5							// Step shading values by half.
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 3
		movq		[qShadeValue],mm4				// Save intensity value.
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 3
		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		and			ecx,SY_SX_MASK					// Mask carry.
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p3
		mov			[pvAlphaDitherTable],ecx		// Save intensity to Alpha+Dither table.
		mov			[edx - 2],ax					// Store pixel 3
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			END_OF_SPAN

FINISH_REMAINDER2:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture coords once, don't step shades.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		[qShadeValue],mm4				// Save intensity value.
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2
		and			ecx,SY_SX_MASK					// Mask carry.
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1
		mov			[edx - 4],ax					// Store pixel 1
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 2
		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2
		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for p1
		and			ecx,SY_SX_MASK					// Mask carry.
		mov			[edx + - 2],bx					// Store pixel 2
		mov			[pvAlphaDitherTable],ecx		// Save intensity to Alpha+Dither table.
		mov			eax,[i_pixel]					// Load remaining length.
		jmp			END_OF_SPAN

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates, backup shades by half.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		psrad		mm5,1							// Reduce shade step to 1x.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		psubd		mm4,mm5							// Backup shading values by 1.
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		movq		[qShadeValue],mm4				// Save intensity value.
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		and			ecx,SY_SX_MASK					// Mask carry.
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1
		mov			[pvAlphaDitherTable],ecx		// Save intensity to Alpha+Dither table.
		mov			[edx - 2],ax					// Store pixel 1
		mov			eax,[i_pixel]					// Load remaining length.

END_OF_SPAN:
		// See if there are any pixels left.
		cmp		eax,0
		je		END_OF_SCANLINE

		//
		// Do perspective correction for next span.
		//
	#ifdef __MWERKS__
		#include "psepilogue3.inl"
	#else
		PERSPECTIVE_SCANLINE_EPILOGUE_3DX
	#endif

		jmp		SUBDIVISION_LOOP

END_OF_SCANLINE:
		pop		ebp								// Restore ebp.

		//		
		// Increment the base edge.
		//
		mov		esi,[pdtri]						// Pointer to polygon object.

		mov		ecx,[pscan]						// Pointer to scanline object.

		// Step length, line starting address.
		mov		edx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,[esi]CDrawPolygonBase.iLineStartIndex

		add		edx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx

		add		edi,gsGlobals.u4LinePixels

		mov		eax,[ecx]TGFogPersp.fxX.i4Fx
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,edx

		mov		ebx,[ecx]TGFogPersp.fxXDifference.i4Fx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,edi

		// Step edge values.
		movq	mm0,[ecx]TGFogPersp.indCoord.fUInvZ

		movd	mm1,[ecx]TGFogPersp.indCoord.fInvZ

		movq	mm2,[tvEdgeStep.UInvZ]

		movd	mm3,[tvEdgeStep.InvZ]

		pfadd	(m0,m2)
		mov		edx,[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx

		pfadd	(m1,m3)
		mov		edi,[fx_diff]

		add		edx,[fx_gour_inc]

		add		eax,[fx_inc]

		add		ebx,edi
		jge		short NO_UNDERFLOW

		// Underflow.
		mov		edi,[fxIModDiv]
		add		ebx,0x00010000

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short FINISH_LOOPING

		// Overflow.
		mov		edi,[fxIModDiv]
		sub		ebx,0x00010000

		add		edx,edi

BORROW_OR_CARRY:
		// Add borrow/carry adjustments to U,V,Z.
		movq	mm2,[tvNegAdj.UInvZ]

		movd	mm3,[tvNegAdj.InvZ]

		pfadd	(m0,m2)

		pfadd	(m1,m3)

FINISH_LOOPING:
		// Save values and Y Loop control.
		movq	[ecx]TGFogPersp.indCoord.fUInvZ,mm0

		movd	[ecx]TGFogPersp.indCoord.fInvZ,mm1

		mov		[ecx]FakeTGFogPersp.gourIntensity.fxIntensity.i4Fx,edx

		mov		[ecx]TGFogPersp.fxX.i4Fx,eax
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[ecx]TGFogPersp.fxXDifference.i4Fx,ebx
		mov		ebx,[esi]CDrawPolygonBase.iYTo

		inc		edx
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		// Done with MMX/3DX.
		femms
	}
}


//*****************************************************************************************
//
// Dithered, alpha-fogged, linear texture.
//
void DrawSubtriangle(TGFogLinear* pscan, CDrawPolygon<TGFogLinear>* pdtri)
{
	typedef TGFogLinear::TPixel prevent_internal_compiler_errors;
	static CDrawPolygon<TGFogLinear>* pdtriGlbl;
	static TGFogLinear* pscanGlbl;
	static ::fixed fx_gour_inc;
	static PackedFloatUV pf_texture_inc;

	TGFogLinear* plinc = &pdtri->pedgeBase->lineIncrement;

	//
	// TGFogLinear with CGouraudOn substitued for CGouraudFog.
	// This is necessary so that we can access members of CGouraudOn.
	//
	typedef CScanline<uint16, CGouraudOn, CTransparencyOff,
			      CMapTexture<uint16>, CIndexLinear, CColLookupOff> FakeTGFogLinear;
	typedef FakeTGFogLinear::TPixel prevent_internal_compiler_errors2;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.

		mov		esi,[pdtri]							// Pointer to polygon object.
		push	ebp									// Save ebp.

		mov		eax,[pscan]							// Pointer to scanline object.
		mov		pdtriGlbl,esi

		//
		// Local copies of edge stepping values.
		//
		// fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
		// pf_texture_inc	= pdtri->pedgeBase->lineIncrement.indCoord.pfIndex;
		// pdtriGlbl		= pdtri;
		// pscanGlbl		= pscan;
		// fx_gour_inc		= pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		//
		mov		ebx,[edi]TGFogLinear.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TGFogLinear.fxXDifference
		mov		[fx_inc],ebx

		movq	mm0,[edi]TGFogLinear.indCoord.pfIndex
		mov		[fx_diff],ecx

		mov		ebx,[edi]FakeTGFogLinear.gourIntensity.fxIntensity
		movq	[pf_texture_inc],mm0

		mov		[fx_gour_inc],ebx

		//
		// Setup:
		//
		// mm2 = qMFactor = width,1,width,1
		// mm5 = qShadeSlopes = fxDeltaXIntensity*2,fxDeltaXIntensity*2
		// mm6 = uvslope
		//
		movd		mm5,[fxDeltaXIntensity]

		movd		mm0,[iTexWidth]					// 0,0,0,Width
		punpckldq	mm5,mm5							// fxDeltaXIntensity,fxDeltaXIntensity

		movd		mm2,[u4OneOne]					// 0,0,1,1
		paddd		mm5,mm5							// fxDeltaXIntensity*2,fxDeltaXIntensity*2

		punpcklwd	mm0,mm0							// 0,0,Width,Width

		movq		mm6,[pfxDeltaTex]				// Load texture slopes.
		punpcklwd	mm2,mm0							// Width,1,Width,1

		movq		mm7,[eax]TGFogLinear.indCoord.pfIndex

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		ebp,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TGFogLinear.fxX.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		mov		edx,gsGlobals.pvScreen

		add		ebx,ecx
		sar		edi,16

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex
		sar		ebx,16

		add		ecx,ebx

		lea		edx,[edx + ecx*2]

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// mm6,mm7 = texture values.
		//
		// Setup registers:
		//
		// eax = texel/alpha index	ebx = alpha/texel index
		// ecx = alpha_base_ptr		edx = dest_base_ptr
		// esi = texture_base_ptr	edi = pixel_offset & count
		// ebp = clut_base_ptr
		//
		// mm0 = temp				mm1 = temp
		// mm2 = mfactor			mm3 = temp
		// mm4 = shade+1,shade		mm5 = shadeslope*2,shadeslope*2
		// mm6 = uvslope			mm7 = uv (stepped once)
		//
		// Setup:
		// QWORD qShadeValue	= fxIntensity+1,fxIntensity
		// pvDither = lbAlphaTerrain.au4AlphaRefDither + low 2 bits of Y + low 2 bits of X.
		//
		movd		mm4,[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx
		pf2id		(m7,m7)							// Convert texture values to 16.16 fixed.

		mov			ecx,[esi]CDrawPolygonBase.iY			// Get current integer y.

		mov			ebx,[eax]TGFogPersp.fxX.i4Fx			// Get current fixed point x.

		shr			ebx,(16-(LOG2_NUM_SHADES+7))			// Shift low 2 bits of x into position.
		and			ecx,3									// Mask low 2 bits of y.
	
		shl			ecx,(LOG2_NUM_SHADES+10)				// Shift low 2 bits of y into position.
		and			ebx,(3 << (LOG2_NUM_SHADES+7))			// Mask shifted low 2 bits of x.	

		or			ecx,ebx									// Combine low bits of x and y.
		mov			ebx,lbAlphaTerrain.au4AlphaRefDither	// Intensity to Alpha+Dither table pointer.

		add			ecx,ebx									// Combine low bits of x and y with pointer.

		movd		mm1,[fxDeltaXIntensity]			// Load intensity delta.
		movq		mm0,mm7							// Copy U1,V1

		punpckldq	mm4,mm4							// Duplicate intensity value.
		paddd		mm7,mm6							// Step U,V

		mov			esi,[pvTextureBitmap]			// Load texture ponter.
		psllq		mm1,32							// Shift to upper dword.

		lea			ebp,lbAlphaTerrain.au2Colour	// Load clut base pointer.
		paddd		mm4,mm1							// Add to starting intensity.

		movq		mm1,mm7							// Copy U2,V2
		psrad		mm0,16							// Shift for integer U1,V1

		dec			ebp								// Hack to force SIB+offset in loop.
		psrad		mm1,16							// Shift for integer U2,V2

		packssdw	mm0,mm1							// Pack integer texture values
		movq		mm1,mm4							// Copy shading values

		psrld		mm1,12							// Shift shading values

		paddd		mm4,mm5							// Step shading values
		pmaddwd		mm0,mm2							// Compute texture indices

		dec			esi								// Hack to force SIB+offset in loop.
		movq		[qShadeTemp],mm1				// Save shading indicies

		//
		// Make sure we have more than 2 pixels left.
		//
		add		edi,2
		jl		short INNER_LOOP_2P

		jmp		FINISH_REMAINDER

		// Align start of loop to 1 past the beginning of a cache line.
		ALIGN	16
		nop

		//
		// Do two pixels at a time.
		//
		// mm0 = Texture indices.
		// qShadeTemp = CLUT indices.
		// mm4 is stepped once.
		// mm7 is stepped once.
		//
INNER_LOOP_2P:
		movq		[qIndexTemp],mm0				// Save texture indicies.
		paddd		mm7,mm6							// Step U,V

		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		movq		mm0,mm7							// Copy U1,V1

		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		psrad		mm0,16							// Shift for integer U1,V1
		
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		paddd		mm7,mm6							// Step U,V

		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		movq		mm1,mm7							// Copy U2,V2

		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		psrad		mm1,16							// Shift for integer U2,V2

		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		packssdw	mm0,mm1							// Pack integer texture values.

		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2
		and			ecx,SY_SX_MASK					// Mask carry.

		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1

		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 2
		movq		mm1,mm4							// Copy shading values.

		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2
		paddd		mm4,mm5							// Step shading values.

		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		pmaddwd		mm0,mm2							// Compute texture indicies.

		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).

		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		movq		[qShadeTemp],mm1				// Save shading indicies.

		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for p2
		and			ecx,SY_SX_MASK					// Mask carry.

		mov			[edx + edi*2 - 2],bx			// Store pixel 2

		add			edi,2							// Inrcement index by 2
		jl			short INNER_LOOP_2P				// Loop (while >2 pixels)

FINISH_REMAINDER:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			short ONE_PIXEL_LEFT

		//
		// Two pixels left, only step texture/shade co-ordinates once.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		add			ecx,(1 << (LOG2_NUM_SHADES+7))	// Step screen x.
		mov			ebx,dword ptr[qIndexTemp+4]		// Get texture index 2
		and			ecx,SY_SX_MASK					// Mask carry.
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1
		mov			eax,dword ptr[qShadeTemp+4]		// Get shade value 2
		movzx		ebx,word ptr[esi + ebx*2 + 1]	// Lookup texel 2
		and			ebx,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			ebx,[ecx + eax*4]				// Apply dithered alhpa bits.
		movzx		ebx,word ptr[ebp + ebx*2 + 1]	// Clut lookup for p2
		mov			[edx + edi*2 - 2],bx			// Store pixel 2
		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left, don't step texture co-ordinates.
		//
		movq		[qIndexTemp],mm0				// Save texture indicies.
		mov			eax,dword ptr[qIndexTemp]		// Get texture index 1
		mov			ebx,dword ptr[qShadeTemp]		// Get shade value 1
		movzx		eax,word ptr[esi + eax*2 + 1]	// Lookup texel 1
		and			eax,[u4TerrainFogMask]			// Mask off bits used for alpha.
		or			eax,[ecx + ebx*4]				// Apply dithered alhpa bits.
		movzx		eax,word ptr[ebp + eax*2 + 1]	// Clut lookup for p1
		mov			[edx + edi*2 - 4],ax			// Store pixel 1

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		movq	mm1,[pf_texture_inc]

		movq	mm7,[eax]TGFogLinear.indCoord.pfIndex

		mov		ebp,[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx

		mov		ebx,[eax]TGFogLinear.fxXDifference
		pfadd	(m7,m1)

		add		ebp,[fx_gour_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000
		movq	mm1,[pfNegD]

		add		ebp,[fxIModDiv]
		pfadd	(m7,m1)

		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000
		movq	mm1,[pfNegD]

		add		ebp,[fxIModDiv]
		pfadd	(m7,m1)

EDGE_DONE:
		// Store modified variables and do looping.
		// Step length, line starting address.
		mov		esi,[pdtriGlbl]
		mov		[eax]FakeTGFogLinear.gourIntensity.fxIntensity.i4Fx,ebp

		mov		[eax]TGFogLinear.fxXDifference,ebx
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		movq	[eax]TGFogLinear.indCoord.pfIndex,mm7
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx

		add		ecx,gsGlobals.u4LinePixels
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx

		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx
		mov		ecx,[eax]TGFogLinear.fxX

		mov		ebp,[fx_inc]

		mov		edx,[esi]CDrawPolygonBase.iY
		add		ecx,ebp

		mov		[eax]TGFogLinear.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp

		// Done with MMX/3DX.
		femms
	}
}


//*********************************************************************************************
void DrawSubtriangle(TFlatGourFog* pscan, CDrawPolygon<TFlatGourFog>* pdtri)
{
	typedef TFlatGourFog::TPixel prevent_internal_compiler_errors;
	static CDrawPolygon<TFlatGourFog>* pdtriGlbl;
	static TFlatGourFog* pscanGlbl;
	static ::fixed fx_gour_inc;

	TFlatGourFog* plinc = &pdtri->pedgeBase->lineIncrement;

	//
	// TFlatGourFog with CGouraudOn substitued for CGouraudFog.
	// This is necessary so that we can access members of CGouraudOn.
	//
	typedef CScanline<uint16, CGouraudOn, CTransparencyOff, CMapFlat, CIndexNone, 
					  CColLookupOff>							FakeTFlatGourFog;
	typedef FakeTFlatGourFog::TPixel prevent_internal_compiler_errors2;

	__asm
	{
		// Get ready to do MMX/3DX.
		femms

		mov		edi,[plinc]							// Pointer to scanline increment.

		mov		esi,[pdtri]							// Pointer to polygon object.
		push	ebp									// Save ebp.

		mov		eax,[pscan]							// Pointer to scanline object.
		mov		pdtriGlbl,esi

		//
		// Local copies of edge stepping values.
		//
		// fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
		// pdtriGlbl		= pdtri;
		// pscanGlbl		= pscan;
		// fx_gour_inc		= pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		//
		mov		ebx,[edi]TFlatGourFog.fxX
		mov		pscanGlbl,eax

		mov		ecx,[edi]TFlatGourFog.fxXDifference
		mov		[fx_inc],ebx

		mov		[fx_diff],ecx
		mov		ebx,[edi]FakeTFlatGourFog.gourIntensity.fxIntensity

		mov		[fx_gour_inc],ebx

		//
		// Setup:
		//
		// mm5 = 2,fxDeltaXIntensity*2 | 2,fxDeltaXIntensity*2
		// mm6 = yxfog_mask | yxfog_mask
		// mm7 = 1,fxDeltaXIntensity | 1,fxDeltaXIntensity
		//
		mov			ecx,[fxDeltaXIntensity]				// Intensity step.
		mov			ebx,0x01b7ffff						// Mask for y,x,shade

		movd		mm6,ebx								
		mov			ebp,ecx								// Copy step.

		add			ecx,ecx								// fxDeltaXIntensity*2
		and			ebp,0x0007ffff						// Mask off 3+16 bits

		and			ecx,0x0007ffff						// Mask off 3+16 bits
		or			ebp,(1 << (LOG2_NUM_SHADES+17))

		movd		mm7,ebp
		or			ecx,(2 << (LOG2_NUM_SHADES+17))

		movd		mm5,ecx
		punpckldq	mm7,mm7								// 1,fxDeltaXIntensity | 1,fxDeltaXIntensity

		punpckldq	mm6,mm6								// yxfog_mask | yxfog_mask
		punpckldq	mm5,mm5								// 2,fxDeltaXIntensity*2 | 2,fxDeltaXIntensity*2


		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		ebp,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TFlatGourFog.fxX.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		mov		edx,gsGlobals.pvScreen

		add		ebx,ecx
		sar		edi,16

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex
		sar		ebx,16

		add		ecx,ebx

		lea		edx,[edx + ecx*2]

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		//--------------------------------------------------------------------------- 
		// Draw the scanline.
		//
		// Setup registers:
		//
		// eax = texel/alpha index				ebx = alpha/texel index
		// ecx = clut_base_ptr + masked color	edx = dest_base_ptr
		// esi = alpha_base_ptr					edi = pixel_offset & count
		// ebp = 
		//
		// mm1 = shifted shading values.
		// mm4 = y,x+1,shade+1 | y,x,shade		mm5 = 2,shadeslope*2 | 2,shadeslope*2
		// mm6 = carry mask | carry mask		mm7 = 1,shadeslope | 1,shadeslope
		//
		mov			ecx,[esi]CDrawPolygonBase.iY			// Get current integer y.

		and			ecx,3									// Mask low 2 bits of y.

		mov			ebx,[eax]TFlatGourFog.fxX.i4Fx			// Get current fixed point x.

		and			ebx,(3 << 16)							// Mask low 2 bits of x.

		shl			ebx,(LOG2_NUM_SHADES+1)					// Shift low 2 bits of x into position.

		shl			ecx,(LOG2_NUM_SHADES+20)				// Shift low 2 bits of y into position.

		or			ebx,ecx									// Combine shifted low bits of x and y.

		mov			ecx,[eax]FakeTFlatGourFog.gourIntensity.fxIntensity.i4Fx

		or			ebx,ecx									// y,x,shade

		movd		mm4,ebx									// n

		movq		mm1,mm4									// Copy value

		paddd		mm1,mm7									// Step by one

		pand		mm1,mm6									// Mask carries

		punpckldq	mm4,mm1									// Combine n and n+1

		lea			ecx,lbAlphaTerrain.au2Colour			// Load clut base pointer.

		mov			eax,[u4ConstColour]						// Load color.

		and			eax,[u4TerrainFogMask]					// Mask out fog bits.

		lea			ecx,[ecx + eax*2 - 1]					// Base + colour + hacked offset.

		mov			esi,lbAlphaTerrain.au4AlphaRefDither	// Intensity to Alpha+Dither table pointer.

		movq		mm1,mm4									// Copy shading values

		psrld		mm1,12									// Shift shading values

		//
		// Align to a quadword boundry.
		//
		lea			eax,[edx + edi*2]

		and			eax,3
		jz			ALIGNED2

		inc			edi								// One pixel left?
		jz			ONE_PIXEL_LEFT

		//
		// Do one pixel for alignment.
		//
		movd		ebx,mm1							// y,x,shade

		paddd		mm4,mm7							// Step shading values by 1.

		pand		mm4,mm6							// Mask carries.
		mov			ebx,[esi + ebx*4]				// Lookup dithered alhpa value 1.

		movq		mm1,mm4							// Copy shading values.
		lea			eax,[edx + edi*2]

		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Do CLUT lookup 1.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).

		mov			[edx + edi*2 - 2],bx			// Store pixel.

ALIGNED2:
		and			eax,7
		jz			ALIGNED4

		add			edi,2							// Two pixels left?
		jge			FINISH_REMAINDER2

		//
		// Do two pixels for alignment.
		//
		movd		eax,mm1							// y,x,shade
		psrlq		mm1,32							// shift second value down.

		movd		ebx,mm1							// y,x,shade
		paddd		mm4,mm5							// Step shading values by 2.

		mov			ebx,[esi + ebx*4]				// Lookup dithered alhpa value 2.
		pand		mm4,mm6							// Toggle bit 1 of x (+2).

		mov			eax,[esi + eax*4]				// Lookup dithered alhpa value 1.
		movq		mm1,mm4							// Copy shading values.

		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Do CLUT lookup 2.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).

		movzx		eax,word ptr[ecx + eax*2 + 1]	// Do CLUT lookup 1.
		shl			ebx,16

		or			eax,ebx
		mov			[edx + edi*2 - 4],eax			// Store pixels.

ALIGNED4:
		add		edi,4		
		jl		short INNER_LOOP_4P					// Four pixles left?

		jmp		FINISH_REMAINDER4

		// Align start of loop to 7 past the beginning of a cache line.
		ALIGN	16
		nop
		nop
		nop
		nop
		nop
		nop
		nop

		//
		// Do four pixels at a time.
		//
INNER_LOOP_4P:
		movd		eax,mm1							// y,x,shade
		psrlq		mm1,32							// shift second value down.

		movd		ebx,mm1							// y,x,shade
		paddd		mm4,mm5							// Step shading values by 2.

		mov			ebx,[esi + ebx*4]				// Lookup dithered alhpa value 2.
		pand		mm4,mm6							// Toggle bit 1 of x (+2).

		mov			eax,[esi + eax*4]				// Lookup dithered alhpa value 1.
		movq		mm1,mm4							// Copy shading values.

		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Do CLUT lookup 2.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).

		movzx		eax,word ptr[ecx + eax*2 + 1]	// Do CLUT lookup 1.
		shl			ebx,16							// Shift pixel 2 up.

		or			eax,ebx							// Combine pixels.
		movd		mm2,eax							// Put in MMX register.

		movd		eax,mm1							// y,x,shade
		psrlq		mm1,32							// shift second value down.

		movd		ebx,mm1							// y,x,shade
		paddd		mm4,mm5							// Step shading values by 2.

		mov			ebx,[esi + ebx*4]				// Lookup dithered alhpa value 2.
		pand		mm4,mm6							// Toggle bit 1 of x (+2).

		mov			eax,[esi + eax*4]				// Lookup dithered alhpa value 1.
		movq		mm1,mm4							// Copy shading values.

		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Do CLUT lookup 2.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).

		movzx		eax,word ptr[ecx + eax*2 + 1]	// Do CLUT lookup 1.
		shl			ebx,16							// Shift pixel 2 up.

		or			eax,ebx							// Combine pixels.
		movd		mm3,eax							// Put in MMX register.

		punpckldq	mm2,mm3							// Combine all four pixels.
		movq		[edx + edi*2 - 8],mm2			// Store 4 pixels

		add			edi,4
		jl			INNER_LOOP_4P

FINISH_REMAINDER4:
		//
		// One (edi == 3), two (edi == 2), three (edi == 1) or four (edi == 0) pixels left.
		//
		jnz			THREE_OR_LESS

		//
		// Four pixels left.
		//
		movd		eax,mm1							// y,x,shade
		psrlq		mm1,32							// shift second value down.

		movd		ebx,mm1							// y,x,shade
		paddd		mm4,mm5							// Step shading values by 2.

		mov			ebx,[esi + ebx*4]				// Lookup dithered alhpa value 2.
		pand		mm4,mm6							// Toggle bit 1 of x (+2).

		mov			eax,[esi + eax*4]				// Lookup dithered alhpa value 1.
		movq		mm1,mm4							// Copy shading values.

		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Do CLUT lookup 2.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).

		movzx		eax,word ptr[ecx + eax*2 + 1]	// Do CLUT lookup 1.
		shl			ebx,16							// Shift pixel 2 up.

		or			eax,ebx							// Combine pixels.
		movd		mm2,eax							// Put in MMX register.

		movd		eax,mm1							// y,x,shade
		psrlq		mm1,32							// shift second value down.

		movd		ebx,mm1							// y,x,shade

		mov			ebx,[esi + ebx*4]				// Lookup dithered alhpa value 2.

		mov			eax,[esi + eax*4]				// Lookup dithered alhpa value 1.

		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Do CLUT lookup 2.

		movzx		eax,word ptr[ecx + eax*2 + 1]	// Do CLUT lookup 1.
		shl			ebx,16							// Shift pixel 2 up.

		or			eax,ebx							// Combine pixels.
		movd		mm3,eax							// Put in MMX register.

		punpckldq	mm2,mm3							// Combine all four pixels.
		movq		[edx + edi*2 - 8],mm2			// Store 4 pixels

		jmp			END_OF_SCANLINE

THREE_OR_LESS:
		sub			edi,2
		jge			FINISH_REMAINDER2

		//
		// Three pixels left.
		//
		movd		eax,mm1							// y,x,shade
		psrlq		mm1,32							// shift second value down.
		movd		ebx,mm1							// y,x,shade
		paddd		mm4,mm5							// Step shading values by 2.
		mov			ebx,[esi + ebx*4]				// Lookup dithered alhpa value 2.
		pand		mm4,mm6							// Toggle bit 1 of x (+2).
		mov			eax,[esi + eax*4]				// Lookup dithered alhpa value 1.
		movq		mm1,mm4							// Copy shading values.
		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Do CLUT lookup 2.
		psrld		mm1,12							// Shift shading values (leave 4 bits of fraction).
		movzx		eax,word ptr[ecx + eax*2 + 1]	// Do CLUT lookup 1.
		shl			ebx,16
		or			eax,ebx
		mov			[edx - 6],eax					// Store pixels.
		movd		eax,mm1							// y,x,shade
		mov			eax,[esi + eax*4]				// Lookup dithered alhpa value 1.
		movzx		eax,word ptr[ecx + eax*2 + 1]	// Do CLUT lookup 1.
		mov			[edx - 2],ax					// Store pixel.
		jmp			short END_OF_SCANLINE

FINISH_REMAINDER2:
		//
		// One pixel (edi == 1) or two pixels (edi == 0) left.
		//
		jnz			ONE_PIXEL_LEFT

		//
		// Two pixels left.
		//
		movd		eax,mm1							// y,x,shade
		psrlq		mm1,32							// shift second value down.
		movd		ebx,mm1							// y,x,shade
		mov			ebx,[esi + ebx*4]				// Lookup dithered alhpa value 2.
		mov			eax,[esi + eax*4]				// Lookup dithered alhpa value 1.
		movzx		ebx,word ptr[ecx + ebx*2 + 1]	// Do CLUT lookup 2.
		movzx		eax,word ptr[ecx + eax*2 + 1]	// Do CLUT lookup 1.
		shl			ebx,16
		or			eax,ebx
		mov			[edx - 4],eax						// Store pixels.

		jmp			short END_OF_SCANLINE

ONE_PIXEL_LEFT:
		//
		// One pixel left.
		//
		movd		eax,mm1							// y,x,shade
		mov			eax,[esi + eax*4]				// Lookup dithered alhpa value 1.
		movzx		eax,word ptr[ecx + eax*2 + 1]	// Do CLUT lookup 1.
		mov			[edx - 2],ax					// Store pixel.

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]

		mov		ebp,[eax]FakeTFlatGourFog.gourIntensity.fxIntensity.i4Fx

		mov		ebx,[eax]TFlatGourFog.fxXDifference

		add		ebp,[fx_gour_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000

		add		ebp,[fxIModDiv]

		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000

		add		ebp,[fxIModDiv]

EDGE_DONE:
		// Store modified variables and do looping.
		// Step length, line starting address.
		mov		esi,[pdtriGlbl]
		mov		[eax]FakeTFlatGourFog.gourIntensity.fxIntensity.i4Fx,ebp

		mov		[eax]TFlatGourFog.fxXDifference,ebx
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx

		add		ecx,gsGlobals.u4LinePixels
		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx

		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx
		mov		ecx,[eax]TFlatGourFog.fxX

		mov		ebp,[fx_inc]

		mov		edx,[esi]CDrawPolygonBase.iY
		add		ecx,ebp

		mov		[eax]TFlatGourFog.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp

		// Done with MMX/3DX.
		femms
	}
}


#else // elif (DITHER_SIZE == 4)


//*********************************************************************************************
//
// Reference C++ implementation.
//
void DrawSubtriangle(TGFogPersp* pscan,	CDrawPolygon<TGFogPersp>* pdtri)
{
	typedef TGFogPersp::TPixel TDest;

	Assert(pscan);
	Assert(pdtri);

	int i_x_from;
	int i_x_to;
	int i_screen_index;
	int i_pixel;

	// Iterate through the scanlines that intersect the subtriangle.
	do
	{
		if (bRIGHT_TO_LEFT)
		{
			i_x_from = (pscan->fxX.i4Fx - pdtri->fxLineLength.i4Fx) >> 16;
			i_x_to   = pscan->fxX.i4Fx >> 16;
			i_screen_index = pdtri->iLineStartIndex + i_x_from - 1;
			i_pixel = i_x_to - i_x_from;
		}
		else
		{
			i_x_from = pscan->fxX.i4Fx >> 16;
			i_x_to   = (pscan->fxX.i4Fx + pdtri->fxLineLength.i4Fx) >> 16;
			i_screen_index = pdtri->iLineStartIndex + i_x_to;
			i_pixel = i_x_from - i_x_to;
		}

		// Draw if there are pixels to draw.
		if (i_x_to > i_x_from && !(bEvenScanlinesOnly && ((pdtri->iY & 1) == 0)))
		{
			{
				TGFogPersp::TGouraud gour(pscan->gourIntensity);
				TGFogPersp::TIndex   index(pscan->indCoord, i_pixel);
				TDest* apix_screen = ((TDest*)pdtri->prasScreen->pSurface) + i_screen_index;

				int i_x = i_x_from;

				// Iterate left or right along the destination scanline.
				for (;;)
				{
					// Do next subdivision.
					int i_pixel_inner = index.i4StartSubdivision(i_pixel);

					TDest* apix_screen_inner = apix_screen + i_pixel;

					for (;;)
					{
//						int32  i4_fog_index     = int(gour.fxIntensity);
//						Assert(bWithin(i4_fog_index, 0, iNUM_TERRAIN_FOG_BANDS - 1));

						uint16 u2_source_pixel  = ((uint16*)pvTextureBitmap)[index.iGetIndex()];
						++index;
						u2_source_pixel &= lbAlphaTerrain.u2GetAlphaMask();
//						u2_source_pixel |= lbAlphaTerrain.u2GetAlphaReference(i4_fog_index);
						u2_source_pixel |= lbAlphaTerrain.u4GetAlphaRefDither(i_x, pdtri->iY, gour.fxIntensity.i4Fx);

						apix_screen_inner[i_pixel_inner] = lbAlphaTerrain.au2Colour[u2_source_pixel];

						i_x++;

						// Increment values.
						i_pixel_inner += i4ScanlineDirection;

						// Break from subdivision loop if there are no more pixels to render.
						if (i_pixel_inner == 0)
							break;

						// Increment values.
						++gour;
					}

					// Exit scanline routine if there are no more pixels to render.
					if (i_pixel == 0)
						break;

					// Increment values.
					++gour;

					// Implement perspective correction.
					index.UpdatePerspective(0);
				}
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


//*********************************************************************************************
void DrawSubtriangle(TGFogLinear* pscan, CDrawPolygon<TGFogLinear>* pdtri)
{
	typedef TGFogLinear::TPixel TDest;

	Assert(pscan);
	Assert(pdtri);

	int i_x_from;
	int i_x_to;
	int i_screen_index;
	int i_pixel;

	// Iterate through the scanlines that intersect the subtriangle.
	do
	{
		if (bRIGHT_TO_LEFT)
		{
			i_x_from = (pscan->fxX.i4Fx - pdtri->fxLineLength.i4Fx) >> 16;
			i_x_to   = pscan->fxX.i4Fx >> 16;
			i_screen_index = pdtri->iLineStartIndex + i_x_from - 1;
			i_pixel = i_x_to - i_x_from;
		}
		else
		{
			i_x_from = pscan->fxX.i4Fx >> 16;
			i_x_to   = (pscan->fxX.i4Fx + pdtri->fxLineLength.i4Fx) >> 16;
			i_screen_index = pdtri->iLineStartIndex + i_x_to;
			i_pixel = i_x_from - i_x_to;
		}

		// Draw if there are pixels to draw.
		if (i_x_to > i_x_from && !(bEvenScanlinesOnly && ((pdtri->iY & 1) == 0)))
		{
			{
				TGFogLinear::TGouraud gour(pscan->gourIntensity);
				TGFogLinear::TIndex   index(pscan->indCoord, i_pixel);
				TDest* apix_screen = ((TDest*)pdtri->prasScreen->pSurface) + i_screen_index;

				int i_x = i_x_from;

				// Iterate left or right along the destination scanline.
				for (;;)
				{
					// Do next subdivision.
					int i_pixel_inner = index.i4StartSubdivision(i_pixel);

					TDest* apix_screen_inner = apix_screen + i_pixel;

					for (;;)
					{
//						int32  i4_fog_index     = int(gour.fxIntensity);
//						Assert(bWithin(i4_fog_index, 0, iNUM_TERRAIN_FOG_BANDS - 1));

						uint16 u2_source_pixel  = ((uint16*)pvTextureBitmap)[index.iGetIndex()];
						++index;
						u2_source_pixel &= lbAlphaTerrain.u2GetAlphaMask();
//						u2_source_pixel |= lbAlphaTerrain.u2GetAlphaReference(i4_fog_index);
						u2_source_pixel |= lbAlphaTerrain.u4GetAlphaRefDither(i_x, pdtri->iY, gour.fxIntensity.i4Fx);

						apix_screen_inner[i_pixel_inner] = lbAlphaTerrain.au2Colour[u2_source_pixel];
						
						i_x++;

						// Increment values.
						i_pixel_inner += i4ScanlineDirection;

						// Break from subdivision loop if there are no more pixels to render.
						if (i_pixel_inner == 0)
							break;

						// Increment values.
						++gour;
					}

					// Exit scanline routine if there are no more pixels to render.
					if (i_pixel == 0)
						break;

					// Increment values.
					++gour;

					// Implement perspective correction.
					index.UpdatePerspective(0);
				}
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


//*********************************************************************************************
void DrawSubtriangle(TFlatGourFog* pscan, CDrawPolygon<TFlatGourFog>* pdtri)
{
	typedef TFlatGourFog::TPixel TDest;

	Assert(pscan);
	Assert(pdtri);

	int i_x_from;
	int i_x_to;
	int i_screen_index;
	int i_pixel;

	// Iterate through the scanlines that intersect the subtriangle.
	do
	{
		i_x_from = pscan->fxX.i4Fx >> 16;
		i_x_to   = (pscan->fxX.i4Fx + pdtri->fxLineLength.i4Fx) >> 16;
		i_screen_index = pdtri->iLineStartIndex + i_x_to;
		i_pixel = i_x_from - i_x_to;

		// Draw if there are pixels to draw.
		if (i_x_to > i_x_from && !(bEvenScanlinesOnly && ((pdtri->iY & 1) == 0)))
		{
			TFlatGourFog::TGouraud gour(pscan->gourIntensity);
			TDest* apix_screen = ((TDest*)pdtri->prasScreen->pSurface) + i_screen_index;

			int i_x = i_x_from;

			// Iterate left along the destination scanline.
			do
			{
//				int32  i4_fog_index     = int(gour.fxIntensity);
//				Assert(bWithin(i4_fog_index, 0, iNUM_TERRAIN_FOG_BANDS - 1));

				uint16 u2_source_pixel  = u4ConstColour;

				u2_source_pixel &= lbAlphaTerrain.u2GetAlphaMask();
				u2_source_pixel |= lbAlphaTerrain.u4GetAlphaRefDither(i_x, pdtri->iY, gour.fxIntensity.i4Fx);

				apix_screen[i_pixel] = lbAlphaTerrain.au2Colour[u2_source_pixel];
				
				i_x++;

				// Increment values.
				++gour;
			}
			while (++i_pixel);
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

#endif // else


// Restore default section.
#pragma code_seg()
