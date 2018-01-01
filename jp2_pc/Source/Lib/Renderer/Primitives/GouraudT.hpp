/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Classes for using, or not using, Gouraud shading in raster primitives.
 *
 * Notes:
 *
 * To Do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/GouraudT.hpp                                 $
 * 
 * 28    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 27    8/25/98 3:06p Rvande
 * Qualified fixed as being in the global scope
 * 
 * 26    98.04.08 8:42p Mmouni
 * Added support for fogged dithered flat shaded primitive.
 * 
 * 25    98.02.17 10:20p Mmouni
 * Gouraud shaded gradient is now zeroed if gradients from any two subtriangles have different
 * signs.
 * 
 * 24    98/01/02 17:22 Speter
 * Added safety check before setting iNumIntensities.
 * 
 * 23    97/11/06 4:49p Pkeet
 * Added class 'CGouraudFog.'
 * 
 * 22    97.10.15 7:41p Mmouni
 * Removed support for right to left scanlines.
 * 
 * 21    97/10/13 2:16p Pkeet
 * Fixed bug with alpha.
 * 
 * 20    97/10/12 20:41 Speter
 * Replaced rvIntensity with cvIntensity, pre-scaled.  Removed SetNumIntensities().
 * iNumIntensities is a debug-only variable.
 * 
 * 19    97/08/05 14:19 Speter
 * Slightly optimised polygon vertex setup by adding InitializeVertexData() functions.
 * 
 * 18    97/07/16 15:54 Speter
 * InitializeTriangleData now takes two params to use as coefficients in derivative calculation
 * rather than f_invdx; also takes b_update parameter to use for multiple-triangle gradient
 * calculations.   Added AssertXRange function to test the validity of a single scanline. 
 * 
 * 17    97/07/07 14:05 Speter
 * Now copy vertices for current polygon locally to arvRasterVertices, and set rvIntensity to
 * scaled intensity.
 * 
 * 16    97/06/27 15:31 Speter
 * Added new CGouraudNone, which does no base intensity calculation.  CGouraudOff now uses
 * polygon's rvFace rather than averaging vertices.  Moved code into new
 * InitializePolygonData(), added bIsPlanar().
 * 
 * 15    6/26/97 9:59a Mlange
 * Now uses the (fast) float to int conversion functions.
 * 
 * 14    97/06/23 20:31 Speter
 * Made gcfScreen a static member of CScreenRender.
 * 
 * 13    6/16/97 8:35p Mlange
 * Updated for fixed::fxFromFloat() name change.
 * 
 * 12    97/01/20 11:51 Speter
 * Moved gcfScreen from CScreenRender to a global var for this rasteriser.
 * 
 * 11    97/01/16 11:55 Speter
 * Updated for CScreenRender changes.
 * 
 * 10    97/01/07 12:05 Speter
 * Updated for ptr_const.
 * 
 * 9     96/12/31 17:04 Speter
 * Changed some pointers to ptr<>.
 * 
 * 8     96/12/17 13:17 Speter
 * Added gamma correction code.
 * Moved extern fClutRampScale declaration to DrawTriangle.hpp.
 * 
 * 7     10/14/96 11:47a Pkeet
 * Maded statically declared global variables externally declared global variables.
 * 
 * 6     10/09/96 7:38p Pkeet
 * Added constructors.
 * 
 * 5     10/04/96 5:36p Pkeet
 * Added an abstract base class. Added the '-' operator member function.
 * 
 * 4     10/03/96 6:17p Pkeet
 * Added the 'iBaseIntensity' variable to the module.
 * 
 * 3     10/03/96 3:16p Pkeet
 * Moved constants and static global variables associated with Gouraud shading here.
 * 
 * 2     10/01/96 3:18p Pkeet
 * Enabled the 'iInitializeTriangleData' function to work without a texture pointer. This would
 * occur when using the LineBumpMake raster drawing routine.
 * 
 * 1     9/27/96 4:32p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_GOURAUDT_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_GOURAUDT_HPP


//
// Constants.
//
const float fGouraudTolerance = 0.05f;


//
// Static variables used for assembly coding.
//

// Floating and fixed point representation of intensity step values with respect to x.
extern float fDeltaXIntensity;
extern ::fixed fxDeltaXIntensity;

// Value for modulus subtract.
extern ::fixed fxIModDiv;

#if VER_DEBUG
	// Maximum intensity value (range-checking only).
	#include "Lib/Renderer/Fog.hpp"
	extern int iNumIntensities;
#endif

// Base intensity value for the triangle.
extern int iBaseIntensity;


//
// Class definitions.
//

//*********************************************************************************************
//
class CGouraudNone
//
// Defines a base class for Gouraud shading.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CGouraudNone member functions.
	//

	//*****************************************************************************************
	//
	forceinline CGouraudNone
	(
	)
	//
	// 
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline CGouraudNone
	(
		const CGouraudNone& gour
	)
	//
	// 
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline uint32 u4Offset
	(
	)
	//
	// Returns the offset into the clut for a given intensity.
	//
	//**************************************
	{
		return 0;
	}

	//*****************************************************************************************
	//
	void AssertRange
	(
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	void AssertXRange
	(
		int i_xrange
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline void operator +=
	(
		const CGouraudNone& gour	// Amount to increment intensity by.
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline void operator -
	(
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline void ModulusSubtract
	(
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}
	
	forceinline void operator ++
	(
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}

	//*********************************************************************************************
	//
	static forceinline bool bIsPlanar()
	//
	// Returns:
	//		Whether this feature has planar gradients across any polygon.
	//
	//**********************************
	{
		return true;
	}

	//*********************************************************************************************
	//
	static forceinline bool bGouraudFog()
	//
	// Returns 'true' if this class is used for fogging.
	//
	//**********************************
	{
		return false;
	}

	//*********************************************************************************************
	//
	forceinline void InitializePolygonData
	(
		ptr_const<CTexture> ptex,			// Texture for polygon.
		TClutVal cv_face					// Face-wide lighting.
	)
	//
	//**************************************
	{
		iBaseIntensity = 0;
	}

	//*********************************************************************************************
	//
	forceinline void InitializeVertexData
	(
		SRenderVertex* prv_src,
		SRenderVertex* prv_dst
	)
	//
	//**************************************
	{
	}

	//*********************************************************************************************
	//
	forceinline void InitializeTriangleData
	(
		SRenderVertex*  prv_1,	// First of three non-colinear coordinates describing the
								// plane of the triangle in screen coordinates.
		SRenderVertex*  prv_2,	// Second coordinate describing the plane.
		SRenderVertex*  prv_3,	// Third coordinate describing the plane.
		float           f_yab_invdx,		// Multipliers for gradients.
		float           f_yac_invdx,
		bool			b_update = false	// Modify the gradients rather than setting them.
	)
	//
	// Sets the average intensity across the triangle.
	//
	//**************************************
	{
	}

	//*********************************************************************************************
	//
	forceinline void InitializeAsBase
	(
 		const SRenderVertex* prv_from,
		const SRenderVertex* prv_to,
		float     f_inverse_dy,
		float     f_start_y,
		float     f_x_difference,
		CGouraudNone* pgour_increment,
		float     f_xdiff_increment
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}

};

//*********************************************************************************************
//
class CGouraudOff: public CGouraudNone
//
// Defines a class which applies constant lighting, without Gouraud shading.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CGouraudOff member functions.
	//

	//*********************************************************************************************
	//
	forceinline void InitializePolygonData
	(
		ptr_const<CTexture> ptex,			// Texture for polygon.
		TClutVal cv_face					// Face-wide lighting.
	)
	//
	//**************************************
	{
		iBaseIntensity = iPosFloatCast(cv_face);
#if VER_DEBUG
		iNumIntensities = 0;
		if (ptex && ptex->ppcePalClut && ptex->ppcePalClut->pclutClut)
		{
			iNumIntensities = ptex->ppcePalClut->pclutClut->iNumRampValues;
			Assert(iBaseIntensity < ptex->ppcePalClut->pclutClut->iNumRampValues);
		}
#endif
	}

};

//*********************************************************************************************
//
class CGouraudOn : public CGouraudNone
//
// Defines a class for rasterizing primitives with Gouraud shading.
//
//**************************************
{
public:

	::fixed fxIntensity;

public:

	//*****************************************************************************************
	//
	// CGouraudOn member functions.
	//

	//*****************************************************************************************
	//
	forceinline CGouraudOn
	(
	)
	//
	// 
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline CGouraudOn
	(
		const CGouraudOn& gour
	)
	//
	// 
	//
	//**************************************
	{
		fxIntensity = gour.fxIntensity;
		AssertRange();
	}

	//*****************************************************************************************
	//
	forceinline void operator +=
	(
		const CGouraudOn& gour	// Amount to increment intensity by.
	)
	//
	// Increments intensity by an amount.
	//
	//**************************************
	{
		fxIntensity += gour.fxIntensity;
		AssertRange();
	}

	//*****************************************************************************************
	//
	forceinline void operator -
	(
	)
	//
	// Negates the sign of the modulus subtraction factor.
	//
	//**************************************
	{
		fxIModDiv = -fxIModDiv;
	}

	//*****************************************************************************************
	//
	forceinline void ModulusSubtract
	(
	)
	//
	// Perform modulus subtraction.
	//
	//**************************************
	{
		fxIntensity += fxIModDiv;
		AssertRange();
	}
	
	forceinline void operator ++
	(
	)
	//
	// Increments the intensity value for the next position on the horizontal scanline.
	//
	//**************************************
	{
		fxIntensity += fxDeltaXIntensity;
		AssertRange();
	}

	//*****************************************************************************************
	//
	forceinline uint32 u4Offset
	(
	)
	//
	// Returns the offset into the clut for a given intensity.
	//
	//**************************************
	{
		// Check that the intensity is in a valid range.
		AssertRange();

		return uint32(int(fxIntensity)) << 8;
	}

	//*****************************************************************************************
	//
	void AssertRange
	(
	)
	//
	// Uses asserts to check the if the value of an intensity is valid.
	//
	//**************************************
	{
		/*
		Assert(fxIntensity >= fixed(0));
		Assert(fxIntensity  < fixed(iNumIntensities));
		*/
	}

	//*****************************************************************************************
	//
	void AssertXRange
	(
		int i_xrange
	)
	//
	// Uses asserts to check if the range of intensities for this scanline is valid.
	//
	//**************************************
	{
#if VER_DEBUG
		/*
		// Check starting value.
		AssertRange();

		// Check ending value.
		fixed fx_end = fxIntensity + fxDeltaXIntensity * fixed(i_xrange);
		Assert(fx_end >= fixed(0));
		Assert(fx_end < fixed(iNumIntensities));
		*/
#endif
	}

	//*********************************************************************************************
	//
	static forceinline bool bIsPlanar()
	//
	// Returns:
	//		Whether this feature has planar gradients across any polygon.
	//
	//**********************************
	{
		return false;
	}

	//*********************************************************************************************
	//
	forceinline void InitializePolygonData
	(
		ptr_const<CTexture> ptex,			// Texture for polygon.
		TClutVal cv_face					// Face-wide lighting.
	)
	//
	//**************************************
	{
#if VER_DEBUG
		iNumIntensities = 0;
		if (ptex && ptex->ppcePalClut && ptex->ppcePalClut->pclutClut)
			iNumIntensities = ptex->ppcePalClut->pclutClut->iNumRampValues;
#endif
		iBaseIntensity = 0;
	}

	//*********************************************************************************************
	//
	forceinline void InitializeVertexData
	(
		SRenderVertex* prv_src,
		SRenderVertex* prv_dst
	)
	//
	//**************************************
	{
		prv_dst->cvIntensity = prv_src->cvIntensity;
	}

	//*********************************************************************************************
	//
	forceinline void InitializeTriangleData
	(
		SRenderVertex* prv_1,			// First of three non-colinear coordinates describing
										// the plane of the triangle in screen coordinates.
		SRenderVertex* prv_2,			// Second coordinate describing the plane.
		SRenderVertex* prv_3,			// Third coordinate describing the plane.
		float          f_yab_invdx,		// Multipliers for gradients.
		float          f_yac_invdx,
		bool		   b_update = false	// Modify the gradients rather than setting them.
	)
	//
	// Setup up triangle-wide data for a Gouraud-shaded triangle; specifically setting the step
	// value with respect to the 'x' axis for intensity and finding the base address in the clut
	// for the colour.
	//
	// Returns 0.
	//
	//**************************************
	{
		Assert(prv_1);
		Assert(prv_2);
		Assert(prv_3);

		//
		// Find the step value for intensity with respect to the horizontal axis.
		//
		float f_di_invdx =	(prv_2->cvIntensity - prv_1->cvIntensity) * f_yac_invdx -
							(prv_3->cvIntensity - prv_1->cvIntensity) * f_yab_invdx;

		if (b_update)
		{
			// If the sign of two gradients is different, then use zero.
			if (CIntFloat(f_di_invdx).bSign() != CIntFloat(fDeltaXIntensity).bSign())
			{
				fDeltaXIntensity = 0.0f;
				fxDeltaXIntensity = 0;
				return;
			}

			// Skip update if we already have a smaller value.
			if (fabs(f_di_invdx) >= fabs(fDeltaXIntensity))
				return;
		}

		fDeltaXIntensity = f_di_invdx;
		
		//
		// Make sure that intensity does not exceed the maximum ramp value of the clut.
		// This clamping is necessary because triangles that are viewed almost edge on can
		// generate very large values for fDeltaXIntensity -- resulting in small inaccuracies
		// that cause texture coordinates to go out-of-range.
		//
		// It can be assumed that if intensity is actually larger than the ramp value of the
		// clut, there will never have more than one pixel per scanline. Therefore clamping
		// this value will not cause incorrect mapping to be used, but will keep inaccuracies
		// from causing the primitive to fail.
		//
		Clamp(fDeltaXIntensity, 16.0f);

		// Convert to fixed point representation.
		fxDeltaXIntensity.fxFromFloat(fDeltaXIntensity);
	}

	//*********************************************************************************************
	//
	forceinline void InitializeAsBase
	(
 		const SRenderVertex* prv_from,
		const SRenderVertex* prv_to,
		float       f_inverse_dy,
		float       f_start_y,
		float       f_x_difference,
		CGouraudOn* pgour_increment,
		float       f_xdiff_increment
	)
	//
	// Initializes the intensity components of a base edge, including starting position and
	// edge walking increment.
	//
	// Notes:
	//		The subpixel 'y' calculation made in CEdge is applied here.
	//
	//**************************************
	{
		Assert(prv_from);
		Assert(prv_to);
		Assert(pgour_increment);

		//
		// Find the i increment value across x. The value is calculated from:
		//
		//		              di    (i1 - i0)
		//	   i increment = ---- = ---------
		//		              dy    (y1 - y0)
		//
		// Note that f_increment_i may be positive or negative, therefore no fast float to
		// int conversion can be used.
		//
		float f_increment_i = f_inverse_dy * (prv_to->cvIntensity - prv_from->cvIntensity);

		//
		// The modulus subtraction value might be a different sign than fxDeltaXIntensity,
		// therefore it has to be stored separately.
		//
		fxIModDiv = fxDeltaXIntensity;

		//
		// Set up the increments for edge walking. Note that the increment of the x difference
		// must be incorporated into this value.
		//
		pgour_increment->fxIntensity.fxFromFloat(f_increment_i + f_xdiff_increment * fDeltaXIntensity);

		//
		// Adjust intensity to the value it would be when the edge first intersects the first
		// horizontal line, and add the starting adjust value with respect to x. 
		//
		fxIntensity.fxFromFloat(prv_from->cvIntensity + f_increment_i * f_start_y +
							    f_x_difference * fDeltaXIntensity);
		AssertRange();
	}

};


//*********************************************************************************************
//
class CGouraudFog : public CGouraudOn
//
// Uses the Gouraud shading algorithm to supply values for fogging.
//
//**************************************
{
public:

	float fFog;

	//*****************************************************************************************
	//
	forceinline CGouraudFog
	(
	)
	//
	// 
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline CGouraudFog
	(
		const CGouraudFog& gour
	)
		: CGouraudOn(gour)
	//
	// 
	//
	//**************************************
	{
	}
	
	//*********************************************************************************************
	//
	static forceinline bool bGouraudFog()
	//
	// Returns 'true' if this class is used for fogging.
	//
	//**********************************
	{
		return false;
	}

	//*********************************************************************************************
	//
	forceinline void InitializePolygonData
	(
		ptr_const<CTexture> ptex,			// Texture for polygon.
		TClutVal cv_face					// Face-wide lighting.
	)
	//
	//**************************************
	{
#if VER_DEBUG
		iNumIntensities = iNUM_TERRAIN_FOG_BANDS;
#endif
		// Get the base color value, the fog is applied via a seperate table.
		iBaseIntensity = iPosFloatCast(cv_face);
	}

	//*****************************************************************************************
	//
	void AssertXRange
	(
		int i_xrange
	)
	//
	// Uses asserts to check if the range of intensities for this scanline is valid.
	//
	//**************************************
	{
#if VER_DEBUG
		/*
		// Check starting value.
		AssertRange();

		// Check ending value.
		fixed fx_end = fxIntensity + fxDeltaXIntensity * fixed(i_xrange);
		Assert(fx_end >= fixed(0));
		Assert(fx_end < fixed(iNumIntensities));
		*/
#endif
	}

};


#endif
