/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Specialized (non-template) versions of the DrawSubtriangle function.
 *		Optimized for the Pentium Pro Processor
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/P6/DrawSubTriangleTexGourEx.cpp               $
 * 
 * 8     98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 7     97/12/10 8:10p Mmouni
 * Fixed problem with primitive incrementing shade before using it.
 * 
 * 6     97.12.01 4:26p Mmouni
 * P6 optimized version created from P5 optimized version.
 * 
 * 5     9/01/97 8:06p Rwyatt
 * ANDs added to enable tiling
 * 
 * 4     8/15/97 12:48a Rwyatt
 * Removed 8 bit terrain primitive and put it into its own file
 * 
 * 3     97/08/12 6:57p Pkeet
 * Added special primitives for terrain texturing.
 * 
 * 2     97/07/18 3:30p Pkeet
 * Removed use of the 'b_right' parameter from DrawSubTriangle.
 * 
 * 1     7/08/97 1:15p Rwyatt
 * Initial Implementation of Pentium Pro rendering specific optimizations
 * 
 ***********************************************************************************************
 ***********************************************************************************************
 * 
 * 6     97/06/14 12:19a Pkeet
 * Some changes.
 * 
 * 5     97/06/13 6:42p Pkeet
 * Initial pass at the linear versions.
 * 
 * 4     97/06/13 6:29p Pkeet
 * Initial pass at perspective corrected, opaque assembly primitive.
 * 
 * 3     97/06/13 5:53p Pkeet
 * Initial clumsy attempt to get the perspective, transparent primitive into assembly.
 * 
 * 2     97/06/12 5:56p Pkeet
 * Added capability to render only every other scanline and to double the number of scanlines.
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
static fixed fxIntensityTemp;


//*****************************************************************************************
//
// Gouroud shaded transparent perspective correct texture (16 bit dest).
//
void DrawSubtriangle(TTexturePerspTransGour* pscan, CDrawPolygon<TTexturePerspTransGour>* pdtri)
{
	typedef TTexturePerspTransGour::TPixel prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	fixed fx_inc;
	fixed fx_diff;
	fixed fx_gour_inc;
	float f_inc_uinvz;
	float f_inc_vinvz;
	float f_inc_invz;

	TTexturePerspTransGour* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self modifications.
		//
		mov		esi,[pvClutConversion]				// Clut conversion pointer.
		mov		ecx,[pvTextureBitmap]				// Texture pointer.

		mov		ebx,[pvLastClut]
		mov		eax,[pvLastTexture]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,esi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx					// Update last texture pointer.
		mov		[pvLastClut],esi					// Update last clut pointer.

		lea		eax,MODIFY_FOR_TEXTURE_POINTER_A
		mov		[eax-4],ecx
		lea		eax,MODIFY_FOR_CLUT_POINTER_A
		mov		[eax-4],esi

DONE_WITH_MODIFY:
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

		mov		ebx,[eax]TTexturePerspTransGour.fxX
		mov		edx,[eax]TTexturePerspTransGour.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TTexturePerspTransGour.indCoord.fUInvZ
		mov		edx,[eax]TTexturePerspTransGour.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		f_inc_vinvz,edx

		mov		ebx,[eax]TTexturePerspTransGour.indCoord.fInvZ
		mov		edx,[eax]TTexturePerspTransGour.gourIntensity.fxIntensity

		mov		f_inc_invz,ebx
		mov		fx_gour_inc,edx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		push	ebp									// Save ebp.

		mov		eax,[esi]CDrawPolygonBase.iY
		mov		edi,[ecx]TTexturePerspTransGour.gourIntensity.fxIntensity.i4Fx

		mov		ebx,[bEvenScanlinesOnly]
		mov		[fxIntensityTemp],edi				// Set the starting gouraud shading value.

		// Check to see if we sould skip this scanline.
		and		eax,ebx
		jnz		END_OF_SCANLINE

		// Setup for prespective correction and looping.
		PERSPECTIVE_SCANLINE_PROLOGUE(TTexturePerspTransGour)

		//
		// eax = i_pixel
		// edi = inner loop count
		// ecx,edx,esi = texture values
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.
		mov		ebx,[i_screen_index]				// Load scanline offset.
		add		ebx,eax								// Add scanline offset to i_pixel

		mov		[i_pixel],eax						// Save i_pixel.
		mov		eax,[u4TextureTileMask]				// Load tiling mask.
		and		edx,eax								// Mask for tiling.

		lea		ebp,[ebp+ebx*2]						// Base of span in ebp.
		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.
		mov		ebx,[fxIntensityTemp]				// Load intensity value.

		jmp		short INNER_LOOP
		ALIGN	16

		// -------------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.
		mov		eax,[w2dDeltaTex]CWalk2D.uUFrac			// Load U fraction step.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		add		esi,eax									// Step U fraction.
		movzx	eax,byte ptr[edx + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTURE_POINTER_A:

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step plus U frac. carry.

		and		edx,[u4TextureTileMask]					// Mask for tiling
		test	eax,eax									// Test for transparency.
		jz		short SKIP_TRANSPARENT

		mov		ebp,ebx									// Copy intensity value.
		shr		ebp,7									// Shift intensity value.
		and		ebp,0x0001fe00							// Mask fractional bits.

		mov		ax,[ebp + eax*2 + 0xDEADBEEF]			// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_A:

		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		[ebp + edi*2],ax						// Store pixel.
SKIP_TRANSPARENT:

		add		ebx,[fxDeltaXIntensity]					// Step intensity.
		inc		edi										// Increment dest & count.
		jnz		short INNER_LOOP

		// See if there are any pixels left.
		mov		[fxIntensityTemp],ebx				// Save intensity value.
		mov		eax,[i_pixel]

		cmp		eax,0
		jz		END_OF_SCANLINE

		// Do perspective correction and looping.
		PERSPECTIVE_SCANLINE_EPILOGUE

END_OF_SCANLINE:
		pop		ebp

		//		
		// Increment the base edge.
		//
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		// Step length, line starting address.
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ebx,[esi]CDrawPolygonBase.iLineStartIndex

		add		eax,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ebx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TTexturePerspTransGour.fxX.i4Fx
		mov		ebx,[ecx]TTexturePerspTransGour.fxXDifference.i4Fx

		fld		[ecx]TTexturePerspTransGour.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TTexturePerspTransGour.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TTexturePerspTransGour.indCoord.fInvZ
		fadd	[f_inc_invz]

		mov		edx,[ecx]TTexturePerspTransGour.gourIntensity.fxIntensity.i4Fx
		mov		edi,[fx_diff]

		add		edx,[fx_gour_inc]
		add		eax,[fx_inc]

		add		ebx,edi
		jge		short NO_UNDERFLOW

		// Underflow.
		add		ebx,0x00010000
		mov		edi,[fxIModDiv]

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short NO_OVERFLOW

		// Overflow.
		sub		ebx,0x00010000
		mov		edi,[fxIModDiv]

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_OVERFLOW:
		fstp	[ecx]TTexturePerspTransGour.indCoord.fInvZ
		fstp	[ecx]TTexturePerspTransGour.indCoord.fVInvZ
		fstp	[ecx]TTexturePerspTransGour.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		// Stall (1)
		fstp	[ecx]TTexturePerspTransGour.indCoord.fInvZ
		fstp	[ecx]TTexturePerspTransGour.indCoord.fUInvZ
		fstp	[ecx]TTexturePerspTransGour.indCoord.fVInvZ

FINISH_LOOPING:
		// Save values and Y Loop control.
		mov		[ecx]TTexturePerspTransGour.gourIntensity.fxIntensity.i4Fx,edx
		mov		[ecx]TTexturePerspTransGour.fxX.i4Fx,eax

		mov		edx,[esi]CDrawPolygonBase.iY
		mov		[ecx]TTexturePerspTransGour.fxXDifference.i4Fx,ebx

		inc		edx
		mov		ebx,[esi]CDrawPolygonBase.iYTo

		mov		[esi]CDrawPolygonBase.iY,edx
		cmp		edx,ebx

		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Gouroud shaded perspective correct texture (16 bit dest).
//
void DrawSubtriangle(TTexturePerspGour* pscan, CDrawPolygon<TTexturePerspGour>* pdtri)
{
	typedef TTexturePerspGour::TPixel prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	fixed fx_inc;
	fixed fx_diff;
	fixed fx_gour_inc;
	float f_inc_uinvz;
	float f_inc_vinvz;
	float f_inc_invz;

	TTexturePerspGour* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self modifications.
		//
		mov		edx,[pvClutConversion]				// Clut conversion pointer.
		mov		ecx,[pvTextureBitmap]				// Texture pointer.
		
		mov		ebx,[pvLastClut]
		mov		eax,[pvLastTexture]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,edx
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx					// Update last texture pointer.
		mov		[pvLastClut],edx					// Update last clut pointer.

		lea		eax,MODIFY_FOR_TEXTURE_POINTER_A
		mov		[eax-4],ecx
		lea		eax,MODIFY_FOR_CLUT_POINTER_A
		mov		[eax-4],edx

DONE_WITH_MODIFY:
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

		mov		ebx,[eax]TTexturePerspGour.fxX
		mov		edx,[eax]TTexturePerspGour.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,edx

		mov		ebx,[eax]TTexturePerspGour.indCoord.fUInvZ
		mov		edx,[eax]TTexturePerspGour.indCoord.fVInvZ

		mov		f_inc_uinvz,ebx
		mov		f_inc_vinvz,edx

		mov		ebx,[eax]TTexturePerspGour.indCoord.fInvZ
		mov		edx,[eax]TTexturePerspGour.gourIntensity.fxIntensity

		mov		f_inc_invz,ebx
		mov		fx_gour_inc,edx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		push	ebp									// Save ebp.

		mov		eax,[esi]CDrawPolygonBase.iY
		mov		edi,[ecx]TTexturePerspGour.gourIntensity.fxIntensity.i4Fx

		mov		ebx,[bEvenScanlinesOnly]
		mov		[fxIntensityTemp],edi				// Set the starting gouraud shading value.

		// Check to see if we sould skip this scanline.
		and		eax,ebx
		jnz		END_OF_SCANLINE

		// Setup for prespective correction and looping.
		PERSPECTIVE_SCANLINE_PROLOGUE(TTexturePerspGour)

		//
		// eax = i_pixel
		// edi = inner loop count
		// ecx,edx,esi = texture values
		//
		mov		ebp,gsGlobals.pvScreen				// Pointer the screen.
		mov		ebx,[i_screen_index]				// Load scanline offset.
		add		ebx,eax								// Add scanline offset to i_pixel

		mov		[i_pixel],eax						// Save i_pixel.
		mov		eax,[u4TextureTileMask]				// Load tiling mask.
		and		edx,eax								// Mask for tiling.

		lea		ebp,[ebp+ebx*2]						// Base of span in ebp.
		mov		[pvBaseOfLine],ebp					// Keep pointer to base of span.
		mov		ebx,[fxIntensityTemp]				// Load intensity value.

		jmp		short INNER_LOOP
		ALIGN	16

		// -------------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.
		mov		eax,[w2dDeltaTex]CWalk2D.uUFrac			// Load U fraction step.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		add		esi,eax									// Step U fraction.
		movzx	eax,byte ptr[edx + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTURE_POINTER_A:

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step plus U frac. carry.
		mov		ebp,ebx									// Copy intensity value.

		add		ebx,[fxDeltaXIntensity]					// Step intensity.
		shr		ebp,7									// Shift intensity value.
		and		ebp,0x0001fe00							// Mask fractional bits.

		mov		ax,[ebp + eax*2 + 0xDEADBEEF]			// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_A:

		and		edx,[u4TextureTileMask]					// Mask for tiling.
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		[ebp + edi*2],ax						// Store pixel.
		inc		edi										// Increment dest & count.
		jnz		short INNER_LOOP

		// See if there are any pixels left.
		mov		[fxIntensityTemp],ebx					// Save intensity value.
		mov		eax,[i_pixel]

		cmp		eax,0
		jz		END_OF_SCANLINE

		// Do perspective correction and looping.
		PERSPECTIVE_SCANLINE_EPILOGUE

END_OF_SCANLINE:
		pop		ebp

		//		
		// Increment the base edge.
		//
		mov		esi,[pdtri]							// Pointer to polygon object.
		mov		ecx,[pscan]							// Pointer to scanline object.

		// Step length, line starting address.
		mov		eax,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ebx,[esi]CDrawPolygonBase.iLineStartIndex

		add		eax,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ebx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,eax
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ebx

		// Step edge values.
		mov		eax,[ecx]TTexturePerspGour.fxX.i4Fx
		mov		ebx,[ecx]TTexturePerspGour.fxXDifference.i4Fx

		fld		[ecx]TTexturePerspGour.indCoord.fUInvZ
		fadd	[f_inc_uinvz]
		fld		[ecx]TTexturePerspGour.indCoord.fVInvZ
		fadd	[f_inc_vinvz]
		fld		[ecx]TTexturePerspGour.indCoord.fInvZ
		fadd	[f_inc_invz]

		mov		edx,[ecx]TTexturePerspGour.gourIntensity.fxIntensity.i4Fx
		mov		edi,[fx_diff]

		add		edx,[fx_gour_inc]
		add		eax,[fx_inc]

		add		ebx,edi
		jge		short NO_UNDERFLOW

		// Underflow.
		add		ebx,0x00010000
		mov		edi,[fxIModDiv]

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short NO_OVERFLOW

		// Overflow.
		sub		ebx,0x00010000
		mov		edi,[fxIModDiv]

		add		edx,edi
		jmp		short BORROW_OR_CARRY

NO_OVERFLOW:
		fstp	[ecx]TTexturePerspGour.indCoord.fInvZ
		fstp	[ecx]TTexturePerspGour.indCoord.fVInvZ
		fstp	[ecx]TTexturePerspGour.indCoord.fUInvZ

		jmp		short FINISH_LOOPING

BORROW_OR_CARRY:
		fadd	[fNegInvZ]								// Z V U
		fxch	st(1)									// V Z U
		fadd	[fNegVInvZ]								// V Z U
		fxch	st(2)									// U Z V
		fadd	[fNegUInvZ]								// U Z V
		fxch	st(1)									// Z U V
		// Stall (1)
		fstp	[ecx]TTexturePerspGour.indCoord.fInvZ
		fstp	[ecx]TTexturePerspGour.indCoord.fUInvZ
		fstp	[ecx]TTexturePerspGour.indCoord.fVInvZ

FINISH_LOOPING:
		// Save values and Y Loop control.
		mov		[ecx]TTexturePerspGour.gourIntensity.fxIntensity.i4Fx,edx
		mov		[ecx]TTexturePerspGour.fxX.i4Fx,eax

		mov		edx,[esi]CDrawPolygonBase.iY
		mov		[ecx]TTexturePerspGour.fxXDifference.i4Fx,ebx

		inc		edx
		mov		ebx,[esi]CDrawPolygonBase.iYTo

		mov		[esi]CDrawPolygonBase.iY,edx
		cmp		edx,ebx

		jl		Y_LOOP
	}
}


//*****************************************************************************************
//
// Gouroud shaded transparent linear texture (16 bit dest).
//
void DrawSubtriangle(TTextureLinearTransGour* pscan, CDrawPolygon<TTextureLinearTransGour>* pdtri)
{
	typedef TTextureLinearTransGour::TPixel prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static CDrawPolygon<TTextureLinearTransGour>* pdtriGlbl;
	static TTextureLinearTransGour* pscanGlbl;
	static fixed fx_gour_inc;

	TTextureLinearTransGour* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self modifications.
		//
		mov		esi,[pvClutConversion]				// Clut conversion pointer.
		mov		ecx,[pvTextureBitmap]				// Texture pointer.

		mov		ebx,[pvLastClut]
		mov		eax,[pvLastTexture]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,esi
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx					// Update last texture pointer.
		mov		[pvLastClut],esi					// Update last clut pointer.

		lea		eax,MODIFY_FOR_TEXTURE_POINTER_A
		mov		[eax-4],ecx
		lea		eax,MODIFY_FOR_CLUT_POINTER_A
		mov		[eax-4],esi

DONE_WITH_MODIFY:
		mov		edi,[plinc]							// Pointer to scanline increment.
		mov		esi,[pdtri]							// Pointer to polygon object.

		mov		eax,[pscan]							// Pointer to scanline object.
		push	ebp									// Save ebp.

		//
		// Local copies of edge stepping values.
		//
		// fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
		// bf_u_inc			= pdtri->pedgeBase->lineIncrement.indCoord.bfU;
		// w1d_v_inc		= pdtri->pedgeBase->lineIncrement.indCoord.w1dV;
		// pdtriGlbl		= pdtri;
		// pscanGlbl		= pscan;
		// fx_gour_inc		= pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		//
		mov		ebx,[edi]TTextureLinearTransGour.fxX
		mov		ecx,[edi]TTextureLinearTransGour.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TTextureLinearTransGour.indCoord.bfU.i4Int
		mov		ecx,[edi]TTextureLinearTransGour.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TTextureLinearTransGour.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TTextureLinearTransGour.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriGlbl,esi
		mov		ebx,[edi]TTextureLinearTransGour.gourIntensity.fxIntensity

		mov		pscanGlbl,eax
		mov		fx_gour_inc,ebx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		ebp,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TCopyLinear.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		add		ebx,ecx

		sar		ebx,16
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		edi,16
		mov		ebp,gsGlobals.pvScreen

		add		ecx,ebx

		sub		edi,ebx
		jge		short END_OF_SCANLINE											// no pixels to draw

		mov		esi,[eax]TTextureLinearTransGour.indCoord.bfU.u4Frac			// UFrac
		mov		edx,[eax]TTextureLinearTransGour.indCoord.w1dV.bfxValue.i4Int	// Vint	

		lea		ebp,[ebp + ecx*2]												// Destination
		mov		ebx,[eax]TTextureLinearTransGour.indCoord.bfU.i4Int				// Uint

		add		edx,ebx															// Add integer steps.
		mov		ecx,[eax]TTextureLinearTransGour.indCoord.w1dV.bfxValue.u4Frac	// VFrac

		mov		ebx,[eax]TTextureLinearTransGour.gourIntensity.fxIntensity.i4Fx	// Intensity
		mov		eax,[u4TextureTileMask]											// Load tiling mask.

		mov		[pvBaseOfLine],ebp									// Save pointer to scanline.
		and		edx,eax												// Mask for tiling.

		jmp		short INNER_LOOP
		ALIGN	16

		// -------------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.
		mov		eax,[w2dDeltaTex]CWalk2D.uUFrac			// Load U fraction step.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		add		esi,eax									// Step U fraction.
		movzx	eax,byte ptr[edx + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTURE_POINTER_A:

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step plus U frac. carry.

		and		edx,[u4TextureTileMask]					// Mask for tiling
		test	eax,eax									// Test for transparency.
		jz		short SKIP_TRANSPARENT

		mov		ebp,ebx									// Copy intensity value.
		shr		ebp,7									// Shift intensity value.
		and		ebp,0x0001fe00							// Mask fractional bits.

		mov		ax,[ebp + eax*2 + 0xDEADBEEF]			// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_A:

		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		[ebp + edi*2],ax						// Store pixel.
SKIP_TRANSPARENT:

		add		ebx,[fxDeltaXIntensity]					// Step intensity.
		inc		edi										// Increment dest & count.
		jnz		short INNER_LOOP

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TTextureLinearTransGour.indCoord.bfU.u4Frac
		mov		edx,[eax]TTextureLinearTransGour.indCoord.bfU.i4Int

		mov		edi,[eax]TTextureLinearTransGour.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TTextureLinearTransGour.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TTextureLinearTransGour.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebp,[eax]TTextureLinearTransGour.gourIntensity.fxIntensity.i4Fx
		mov		ebx,[eax]TTextureLinearTransGour.fxXDifference

		add		ebp,[fx_gour_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,bfNegDU.i4Int
		add		ebp,[fxIModDiv]

		add		esi,w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE

		add		edi,[eax]TTextureLinearTransGour.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,bfNegDU.i4Int
		add		ebp,[fxIModDiv]

		add		esi,w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE

		add		edi,[eax]TTextureLinearTransGour.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TTextureLinearTransGour.gourIntensity.fxIntensity.i4Fx,ebp
		mov		[eax]TTextureLinearTransGour.fxXDifference,ebx

		mov		[eax]TTextureLinearTransGour.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TTextureLinearTransGour.indCoord.w1dV.bfxValue.i4Int,edi

		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TTextureLinearTransGour.indCoord.bfU.u4Frac,ecx
		mov		[eax]TTextureLinearTransGour.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TTextureLinearTransGour.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TTextureLinearTransGour.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp
	}
}


//*****************************************************************************************
//
// Gouroud shaded linear texture (16 bit dest).
//
void DrawSubtriangle(TTextureLinearGour* pscan, CDrawPolygon<TTextureLinearGour>* pdtri)
{
	typedef TTextureLinearGour::TPixel prevent_internal_compiler_errors;
	static void* pvLastClut = 0;
	static void* pvLastTexture = 0;
	static CDrawPolygon<TTextureLinearGour>* pdtriGlbl;
	static TTextureLinearGour* pscanGlbl;
	static fixed fx_gour_inc;

	TTextureLinearGour* plinc = &pdtri->pedgeBase->lineIncrement;

	__asm
	{
		//
		// Do self modifications.
		//
		mov		edx,[pvClutConversion]				// Clut conversion pointer.
		mov		ecx,[pvTextureBitmap]				// Texture pointer.

		mov		ebx,[pvLastClut]
		mov		eax,[pvLastTexture]

		cmp		eax,ecx
		jne		short DO_MODIFY

		cmp		ebx,edx
		je		short DONE_WITH_MODIFY

DO_MODIFY:
		mov		[pvLastTexture],ecx					// Update last texture pointer.
		mov		[pvLastClut],edx					// Update last clut pointer.

		lea		eax,MODIFY_FOR_TEXTURE_POINTER_A
		mov		[eax-4],ecx
		lea		eax,MODIFY_FOR_CLUT_POINTER_A
		mov		[eax-4],edx

DONE_WITH_MODIFY:
		mov		edi,[plinc]							// Pointer to scanline increment.
		mov		esi,[pdtri]							// Pointer to polygon object.

		mov		eax,[pscan]							// Pointer to scanline object.
		push	ebp									// Save ebp.

		//
		// Local copies of edge stepping values.
		//
		// fx_inc			= pdtri->pedgeBase->lineIncrement.fxX;
		// fx_diff			= pdtri->pedgeBase->lineIncrement.fxXDifference;
		// bf_u_inc			= pdtri->pedgeBase->lineIncrement.indCoord.bfU;
		// w1d_v_inc		= pdtri->pedgeBase->lineIncrement.indCoord.w1dV;
		// pdtriGlbl		= pdtri;
		// pscanGlbl		= pscan;
		// fx_gour_inc		= pdtri->pedgeBase->lineIncrement.gourIntensity.fxIntensity;
		//
		mov		ebx,[edi]TTextureLinearGour.fxX
		mov		ecx,[edi]TTextureLinearGour.fxXDifference

		mov		fx_inc,ebx
		mov		fx_diff,ecx

		mov		ebx,[edi]TTextureLinearGour.indCoord.bfU.i4Int
		mov		ecx,[edi]TTextureLinearGour.indCoord.bfU.u4Frac

		mov		bf_u_inc.i4Int,ebx
		mov		bf_u_inc.u4Frac,ecx

		mov		ebx,[edi]TTextureLinearGour.indCoord.w1dV.bfxValue.i4Int
		mov		ecx,[edi]TTextureLinearGour.indCoord.w1dV.bfxValue.u4Frac

		mov		w1d_v_inc.bfxValue.i4Int,ebx
		mov		w1d_v_inc.bfxValue.u4Frac,ecx

		mov		pdtriGlbl,esi
		mov		ebx,[edi]TTextureLinearGour.gourIntensity.fxIntensity

		mov		pscanGlbl,eax
		mov		fx_gour_inc,ebx

		//
		// Iterate through the scanlines that intersect the subtriangle.
		//
Y_LOOP:
		mov		ebx,[bEvenScanlinesOnly]
		mov		ebp,[esi]CDrawPolygonBase.iY

		// Check for odd scan lines here and skip if even
		and		ebp,ebx
		jnz		short END_OF_SCANLINE

		mov		ebx,[eax]TTextureLinearGour.fxX.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.fxLineLength.i4Fx

		mov		edi,ebx
		add		ebx,ecx

		sar		ebx,16
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		sar		edi,16
		mov		ebp,gsGlobals.pvScreen

		add		ecx,ebx

		sub		edi,ebx
		jge		short END_OF_SCANLINE										// no pixels to draw

		mov		esi,[eax]TTextureLinearGour.indCoord.bfU.u4Frac				// UFrac
		mov		edx,[eax]TTextureLinearGour.indCoord.w1dV.bfxValue.i4Int	// Vint	

		lea		ebp,[ebp + ecx*2]											// Destination
		mov		ebx,[eax]TTextureLinearGour.indCoord.bfU.i4Int				// Uint

		add		edx,ebx														// Add integer steps.
		mov		ecx,[eax]TTextureLinearGour.indCoord.w1dV.bfxValue.u4Frac	// VFrac

		and		edx,[u4TextureTileMask]										// Mask for tiling.
		mov		[pvBaseOfLine],ebp
		mov		ebx,[eax]TTextureLinearGour.gourIntensity.fxIntensity.i4Fx

		jmp		short INNER_LOOP
		ALIGN	16

		// -------------------------------------------------------------------------
		// Draw pixels.
		//
INNER_LOOP:
		add		ecx,[w2dDeltaTex]CWalk2D.uVFrac			// Step V fraction.
		mov		eax,[w2dDeltaTex]CWalk2D.uUFrac			// Load U fraction step.

		sbb		ebp,ebp									// Get borrow from V fraction step.
		add		esi,eax									// Step U fraction.
		movzx	eax,byte ptr[edx + 0xDEADBEEF]			// Read texture value.
MODIFY_FOR_TEXTURE_POINTER_A:

		adc		edx,[w2dDeltaTex+ebp*4+4]CWalk2D.iUVInt	// Integer step plus U frac. carry.
		mov		ebp,ebx									// Copy intensity value.

		add		ebx,[fxDeltaXIntensity]					// Step intensity.
		shr		ebp,7									// Shift intensity value.
		and		ebp,0x0001fe00							// Mask fractional bits.

		mov		ax,[ebp + eax*2 + 0xDEADBEEF]			// Do CLUT lookup (low).
MODIFY_FOR_CLUT_POINTER_A:

		and		edx,[u4TextureTileMask]					// Mask for tiling.
		mov		ebp,[pvBaseOfLine]						// Load pointer to destination.

		mov		[ebp + edi*2],ax						// Store pixel.
		inc		edi										// Increment dest & count.
		jnz		short INNER_LOOP

		// -------------------------------------------------------------------------
		// Increment the base edge.
		//
END_OF_SCANLINE:
		mov		eax,[pscanGlbl]
		mov		ebp,bf_u_inc.i4Int

		// AGI on eax.

		mov		ecx,[eax]TTextureLinearGour.indCoord.bfU.u4Frac
		mov		edx,[eax]TTextureLinearGour.indCoord.bfU.i4Int

		mov		edi,[eax]TTextureLinearGour.indCoord.w1dV.bfxValue.i4Int
		mov		esi,[eax]TTextureLinearGour.indCoord.w1dV.bfxValue.u4Frac

		add		edi,w1d_v_inc.bfxValue.i4Int
		add		ecx,bf_u_inc.u4Frac

		adc		edx,ebp
		mov		ebp,w1d_v_inc.bfxValue.u4Frac

		add		esi,ebp
		jnc		short NO_VFRAC_CARRY

		add		edi,[eax]TTextureLinearGour.indCoord.w1dV.iOffsetPerLine

NO_VFRAC_CARRY:
		mov		ebp,[eax]TTextureLinearGour.gourIntensity.fxIntensity.i4Fx
		mov		ebx,[eax]TTextureLinearGour.fxXDifference

		add		ebp,[fx_gour_inc]
		add		ebx,[fx_diff]

		jge		short NO_UNDERFLOW

		// Undeflow of fxXDifference.
		add		ebx,0x00010000

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,bfNegDU.i4Int
		add		ebp,[fxIModDiv]

		add		esi,w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE

		add		edi,[eax]TTextureLinearGour.indCoord.w1dV.iOffsetPerLine
		jmp		short EDGE_DONE

NO_UNDERFLOW:
		cmp		ebx,0x00010000
		jl		short EDGE_DONE

		// Overflow of fxXDifference.
		sub		ebx,0x00010000

		add		edi,w1dNegDV.bfxValue.i4Int
		add		ecx,bfNegDU.u4Frac

		adc		edx,bfNegDU.i4Int
		add		ebp,[fxIModDiv]

		add		esi,w1dNegDV.bfxValue.u4Frac
		jnc		short EDGE_DONE

		add		edi,[eax]TTextureLinearGour.indCoord.w1dV.iOffsetPerLine

EDGE_DONE:
		// Store modified variables and do looping.
		mov		[eax]TTextureLinearGour.gourIntensity.fxIntensity.i4Fx,ebp
		mov		[eax]TTextureLinearGour.fxXDifference,ebx

		mov		[eax]TTextureLinearGour.indCoord.w1dV.bfxValue.u4Frac,esi
		mov		[eax]TTextureLinearGour.indCoord.w1dV.bfxValue.i4Int,edi

		mov		esi,[pdtriGlbl]								// Pointer to polygon object.

		mov		[eax]TTextureLinearGour.indCoord.bfU.u4Frac,ecx
		mov		[eax]TTextureLinearGour.indCoord.bfU.i4Int,edx

		// Step length, line starting address.
		mov		ebx,[esi]CDrawPolygonBase.fxLineLength.i4Fx
		mov		ecx,[esi]CDrawPolygonBase.iLineStartIndex

		add		ebx,[esi]CDrawPolygonBase.fxDeltaLineLength.i4Fx
		add		ecx,gsGlobals.u4LinePixels

		mov		[esi]CDrawPolygonBase.fxLineLength.i4Fx,ebx
		mov		[esi]CDrawPolygonBase.iLineStartIndex,ecx

		mov		ecx,[eax]TTextureLinearGour.fxX
		mov		ebp,[fx_inc]

		add		ecx,ebp
		mov		edx,[esi]CDrawPolygonBase.iY

		mov		[eax]TTextureLinearGour.fxX,ecx
		inc		edx

		mov		ebx,[esi]CDrawPolygonBase.iYTo
		mov		[esi]CDrawPolygonBase.iY,edx

		cmp		edx,ebx
		jl		Y_LOOP

		pop		ebp
	}
}
