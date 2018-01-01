/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * Notes:
 *
 * To Do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/IndexPerspectiveT.hpp                         $
 * 
 * 56    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 55    98.08.02 7:25p Mmouni
 * Changed default adaptive min to 16.
 * 
 * 54    98.07.21 8:21p Mmouni
 * Adaptive perspective correction is now on by default.
 * Change adaptive default values.
 * 
 * 53    98.07.17 6:30p Mmouni
 * Added new adaptive perspective stuff.
 * 
 * 52    98.04.24 6:59p Mmouni
 * Changed alternate perspective correction setting.
 * 
 * 51    98.03.24 8:17p Mmouni
 * Made changes to support alternate perspective correction settings.
 * 
 * 50    98.01.14 9:08p Mmouni
 * Made changes for K6 3D polygon setup optimizations.
 * 
 * 49    97.11.14 11:54p Mmouni
 * Added CIndexPerspectiveFilter index type.
 * 
 * 48    97.11.04 10:21p Mmouni
 * Spelling corrections.
 * 
 * 47    97.10.30 11:18a Mmouni
 * iGetSubdivisionLen was using the wrong variable for insuring the subdivision length chosen is
 * even.
 * 
 * 46    97.10.27 1:27p Mmouni
 * Made changes to support the K6-3D.
 * 
 * 45    97/10/23 10:56a Pkeet
 * Added a K6 3D switch.
 * 
 * 44    97.10.15 7:41p Mmouni
 * Removed support for right to left scanlines.
 * 
 * 43    10/10/97 1:44p Mmouni
 * Now gets fTexWith, fTexHeight directly from texture.
 * 
 * 42    9/29/97 11:12a Mmouni
 * Now calculates values to support aligment in first texture correction span.
 * Calculated correction spans are now always a power of 2.
 * 
 * 41    9/15/97 2:02p Mmouni
 * Added dFastFixed16Conversion constant.
 * Now takes into account clamp flag when calculating fTexWidth, fTexheight.
 * 
 * 40    9/01/97 8:00p Rwyatt
 * GetIndex member is and-ed with the texture tile mask to enable tiling
 * 
 * 39    97/08/17 4:14p Pkeet
 * Added the 'BuildTileMask' member function.
 * 
 * 38    8/17/97 12:18a Agrant
 * Put texture coord clamping on a VER switch.
 * 
 * 37    8/15/97 12:50a Rwyatt
 * Added externs for new global perspective constants.
 * 
 * 36    97/08/07 11:35a Pkeet
 * Added interface support for mipmapping.
 * 
 * 35    97/08/06 3:20p Pkeet
 * Added mipmapping interface.
 * 
 * 34    97/08/05 14:19 Speter
 * Slightly optimised polygon vertex setup by adding InitializeVertexData() functions.
 * 
 * 33    97/07/17 5:09p Pkeet
 * Turned adaptive subdivision off.
 * 
 * 32    97/07/16 15:54 Speter
 * InitializeTriangleData now takes two params to use as coefficients in derivative calculation
 * rather than f_invdx; also takes b_update parameter to use for multiple-triangle gradient
 * calculations.   Added AssertXRange function to test the validity of a single scanline. 
 * 
 * 31    7/07/97 11:53p Rwyatt
 * No includes headers that contain assembly code from the processor specific directories
 * 
 * 30    97/07/07 14:05 Speter
 * Now copy vertices for current polygon locally to arvRasterVertices, and set tcTex to scaled
 * coordinates.
 * 
 * 29    97/06/27 15:34 Speter
 * Moved code to new InitializePolygonData(), added bIsPlanar().  Added fD[U||V]InvEdge
 * variables, needed for multiple base edges per polygon.
 * 
 * 28    97/06/27 2:03p Pkeet
 * Changed default values for a better-looking fit.
 * 
 * 27    97/06/24 1:59p Pkeet
 * Added adaptive subdivision.
 * 
 * 26    6/19/97 2:53p Mlange
 * Moved Profile.hpp to Lib/Sys.
 * 
 * 25    97/06/16 11:06p Pkeet
 * Changed default subdivision setting.
 * 
 * 24    97/06/03 18:48 Speter
 * Added bIsPerspective() member.
 * 
 * 23    97-03-31 22:21 Speter
 * Removed pvGetTexture() function, placed code into InitializeTriangleData().
 * 
 * 
 * 22    1/27/97 9:04p Pkeet
 * Changed default subdivision value to 20.
 * 
 * 21    1/16/97 1:35p Pkeet
 * Changed the default subdivision length to 16.
 * 
 * 20    1/15/97 1:09p Pkeet
 * Added the 'i4StartSubdivision' member function. Simplified the UpdatePerspective function.
 * 
 * 19    1/09/97 5:12p Pkeet
 * Moved assembly code to a separate file in the 'P5' subdirectory. Fixed bugs associated with
 * compilation under Visual C++ 5.0.
 * 
 * 18    97/01/07 12:05 Speter
 * Updated for ptr_const.
 * 
 * 17    1/03/97 4:41p Pkeet
 * Added the 'iSubdivideLen' and 'fInvSubdivideLen' globals.
 * 
 * 16    96/12/31 17:04 Speter
 * Changed some pointers to ptr<>.
 * 
 * 15    12/16/96 12:36p Pkeet
 * More optimizations.
 * 
 * 14    12/15/96 4:36p Pkeet
 * More optimizations.
 * 
 * 13    12/15/96 4:04p Pkeet
 * Converted the entire initialization function to  assembly.
 * 
 * 12    12/15/96 3:19p Pkeet
 * More optimizations.
 * 
 * 11    12/12/96 7:40p Pkeet
 * More conversions to assembly.
 * 
 * 10    12/12/96 2:53p Pkeet
 * Changed subdivision length from 8 to 12.
 * 
 * 9     12/12/96 11:55a Pkeet
 * Added more assembly code.
 * 
 * 8     12/11/96 10:29p Pkeet
 * Converted more member functions to assembly.
 * 
 * 7     12/11/96 5:15p Pkeet
 * Initial code for the subdivision set up to optionally use the fast inverse approximation or
 * the fdiv instruction.
 * 
 * 6     12/11/96 3:55p Pkeet
 * Overlapping fdiv done.
 * 
 * 5     12/11/96 2:51p Pkeet
 * Removed unnecessary code.
 * 
 * 4     12/11/96 2:22p Pkeet
 * Enabled subdivision using the fast inverse function.
 * 
 * 3     12/10/96 10:40a Pkeet
 * Made perspective correction occur every eight pixels.
 * 
 * 2     12/09/96 4:10p Pkeet
 * Implemented perspective correction using floats.
 * 
 * 1     12/09/96 3:50p Pkeet
 * Initial implementation. Converted all values to use floats instead of big fixed point
 * numbers.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_INDEXPERSPECTIVET_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_INDEXPERSPECTIVET_HPP


//
// Defines.
//

// Define the maximum scanline length to subdivide over.
#define iDEFAULT_SUBDIVIDELEN		(32)

// Define the minimum scanline subdivision length.
#define iDEFAULT_MIN_SUBDIVIDELEN		(32)
#define iDEFAULT_ALT_MIN_SUBDIVIDELEN	(64)

// Maximum scanline subdivision length.
#define iMAX_SUBDIVIDELEN           (iNUM_ENTRIES_INVERSEINT - 2)

// Switch to use the fast inverse subdivision.
#define bUSE_FAST_INVERSE   (false)

// Constants for fast conversions.
#define iBITS_ACCURACY 11

//const float fFixed16Scale = 65536.0;
const float fFastFixed16Conversion = float(1 << (23 - iBITS_ACCURACY));

// Constant to add for low dword of a double to be signed 16.16 fixed.
const double dFastFixed16Conversion = 103079215104.0;


//
// Includes.
//

#include "Config.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "Lib/Math/FastInverse.hpp"


//
// A vector of three float values used for perspective correction.
// These must be allocated together for 3dx/mmx.
//
struct TexVals
{
	float UInvZ;
	float VInvZ;
	float InvZ;
};


//
// Externally defined globals.
//

extern int   iTexWidth;
extern float fDUInvZScanline;
extern float fDVInvZScanline;
extern float fDInvZScanline;
extern const float fFixed16Scale;
extern const float fOne;

#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

extern TexVals tvDeltas;

#define fDUInvZ		tvDeltas.UInvZ
#define fDVInvZ		tvDeltas.VInvZ
#define fDInvZ		tvDeltas.InvZ

extern TexVals tvDEdge;

#define fDUInvZEdge tvDEdge.UInvZ
#define fDVInvZEdge tvDEdge.VInvZ
#define fDInvZEdge	tvDEdge.InvZ

#else

extern float fDUInvZ;
extern float fDVInvZ;
extern float fDInvZ;

extern float fDUInvZEdge;
extern float fDVInvZEdge;
extern float fDInvZEdge;

#endif

extern float fDUInvZEdgeMinusOne;
extern float fDVInvZEdgeMinusOne;
extern float fDInvZEdgeMinusOne;

#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

extern TexVals tvNegAdj;

#define fNegUInvZ	tvNegAdj.UInvZ
#define fNegVInvZ	tvNegAdj.VInvZ
#define fNegInvZ	tvNegAdj.InvZ

#else

extern float fNegUInvZ;
extern float fNegVInvZ;
extern float fNegInvZ;

#endif

extern float fGUInvZ;
extern float fGVInvZ;
extern float fGInvZ;

#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

extern ClampVals cvMaxCoords;

#define fTexWidth	cvMaxCoords.U
#define fTexHeight	cvMaxCoords.V

#else

extern float fTexWidth;
extern float fTexHeight;

#endif

extern float fU, fNextU;
extern float fV, fNextV;

extern int   iNextSubdivide;
extern int   iCacheNextSubdivide;

extern int32 i4ScanlineDirection;

extern CWalk2D w2dTex;
extern CWalk2D w2dDeltaTex;

// Inverse subdivision length.
extern float fInvSubdivideLen;

// Subdivision length.
extern int   iSubdivideLen;


//*********************************************************************************************
//
class CPerspectiveSettings
//
// Object encapsulates settings and functions for perspective correction subdivision.
//
// Prefix: perset
//
//**************************************
{
public:

	bool  bAdaptive;				// Use adaptive subdivision.

	// Adaptive settings.
	int   iAdaptiveMinSubdivision;		// Adaptive mininum subdivision size.
	int   iAdaptiveMaxSubdivision;		// Adaptive maximimum subdivision size.
	float fInvZForMaxSubdivision;		// 1/Z delta where maximum is used.
	float fInvZScale;					// Scale for 1/z delta.
									
	// Non-adaptive settings.
	int   iMinSubdivision;			// Mininum subdivision size.
	int   iAltMinSubdivision;		// Alternative mininum subdivision size.

	//*****************************************************************************************
	//
	// Constructor.
	//

	CPerspectiveSettings()
	{
		// Default settings.
		bAdaptive              = true;

		iAdaptiveMinSubdivision = 16;
		iAdaptiveMaxSubdivision = 64;
		fInvZForMaxSubdivision	= 1e-6f;
		fInvZScale				= 1e7f;

		iMinSubdivision        = iDEFAULT_MIN_SUBDIVIDELEN;
		iAltMinSubdivision     = iDEFAULT_ALT_MIN_SUBDIVIDELEN;
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	forceinline int iGetSubdivisionLen
	(
		float f_dinvz,
		int b_usealt
	)
	//
	// Returns the number of pixels to subdivide over given an inverse Z delta.
	//
	//**************************************
	{
		int i_subdivisionlen;

		// Use adaptive value?
		if (bAdaptive)
		{
			// Use positive values only.
			f_dinvz = fabs(f_dinvz);

			// If the value is too small, use max.
			if (f_dinvz < fInvZForMaxSubdivision)
			{
				return iAdaptiveMaxSubdivision;
			}

			// Decrease the subdivision length based on the 1/z delta.
			i_subdivisionlen = iAdaptiveMaxSubdivision - 
							   Fist((f_dinvz - fInvZForMaxSubdivision) * fInvZScale);

			// Make subdivision length a multiple of 2.
			i_subdivisionlen &= 0xfffffffe;

			// Keep the value from being too small.
			if (i_subdivisionlen < iAdaptiveMinSubdivision)
				return iAdaptiveMinSubdivision;
		}
		else
		{
			if (b_usealt)
				i_subdivisionlen = iAltMinSubdivision;
			else
				i_subdivisionlen = iMinSubdivision;
		}

		// Return the value.
		return i_subdivisionlen;
	}
};

// Extern the settings so that everybody can access them.
extern CPerspectiveSettings persetSettings;


//
// Class definitions.
//

//*********************************************************************************************
//
class CIndexPerspective : public CIndex
//
// Prefix: ipls
//
//**************************************
{
public:

	float fUInvZ;
	float fVInvZ;
	float fInvZ;

public:

	//*****************************************************************************************
	//
	// CIndexPerspective constructors.
	//

	forceinline CIndexPerspective()
	{
	}

	forceinline CIndexPerspective(const CIndexPerspective& index, int i_pixel)
	{
		// Copy the essential values.
		fGUInvZ = index.fUInvZ;
		fGVInvZ = index.fVInvZ;
		fGInvZ  = index.fInvZ;

		fDUInvZScanline = fDUInvZEdge;
		fDVInvZScanline = fDVInvZEdge;
		fDInvZScanline  = fDInvZEdge;

		// Start the divide for the first scanline subdivision.
		InitializeSubdivision(i_pixel);
	}

	//*****************************************************************************************
	//
	// CIndexPerspective member functions.
	//

	forceinline void Mask()
	{
		w2dTex.iUVInt[1] &= u4TextureTileMask;
	}

	//*****************************************************************************************
	//
	static forceinline bool bIsIndexed
	(
	)
	//
	//**************************************
	{
		return true;
	}

	//*****************************************************************************************
	//
	static forceinline bool bIsPerspective
	(
	)
	//
	//**************************************
	{
		return true;
	}

	//*****************************************************************************************
	//
	forceinline int i4StartSubdivision
	(
		int32& ri4_pixel	// Pixel
	)
	//
	// Returns zero.
	//
	//**************************************
	{
		int32 i4_retval;

		if (ri4_pixel < 0)
		{
			if (-ri4_pixel > iSubdivideLen)
			{
				i4_retval  = -iSubdivideLen;
				ri4_pixel +=  iSubdivideLen;
			}
			else
			{
				i4_retval = ri4_pixel;
				ri4_pixel = 0;
			}
		}
		else
		{
			if (ri4_pixel > iSubdivideLen)
			{
				i4_retval  = iSubdivideLen;
				ri4_pixel -= iSubdivideLen;
			}
			else
			{
				i4_retval = ri4_pixel;
				ri4_pixel = 0;
			}
		}

		return i4_retval;
	}

	//*****************************************************************************************
	//
	void InitializeSubdivision
	(
		int i_pixel	// Pixel position in the scanline.
	);
	//
	// Returns the index value into the bitmap representing the position reference by the u and
	// v values.
	//
	//**************************************

	//*****************************************************************************************
	//
	forceinline int iSetNextDividePixel
	(
		int i_pixel	// Pixel position in the scanline.
	)
	//
	// 
	//
	//**************************************
	{
		int i_next_subdivide;

		i_next_subdivide = i_pixel + iSubdivideLen;
		if (i_next_subdivide > 0)
		{
			i_next_subdivide = 0;
			float f_ratio = fInvSubdivideLen * float(-i_pixel);
			fDUInvZScanline *= f_ratio;
			fDVInvZScanline *= f_ratio;
			fDInvZScanline  *= f_ratio;
		}

		return i_next_subdivide;
	}

	//*****************************************************************************************
	//
	void BeginNextSubdivision
	(
	);
	//
	// Returns the index value into the bitmap representing the position reference by the u and
	// v values.
	//
	//**************************************

	//*****************************************************************************************
	//
	void EndNextSubdivision
	(
	);
	//
	// Returns the index value into the bitmap representing the position reference by the u and
	// v values.
	//
	//**************************************

	//*****************************************************************************************
	//
	forceinline int iGetIndex
	(
	)
	//
	// Returns the index value into the bitmap representing the position reference by the u and
	// v values.
	//
	//**************************************
	{
		w2dTex.iUVInt[1] &= u4TextureTileMask;
		int i_index = w2dTex.iUVInt[1];
#if VER_CLAMP_UV_TILE
		Assert(i_index >= 0);
		Assert(i_index  < iTexSize);
#endif

		return i_index;
	}
	
	//*****************************************************************************************
	//
	forceinline void UpdatePerspective
	(
		int i_pixel
	)
	//
	// Increments the u and v values for the next position on the horizontal scanline.
	//
	//**************************************
	{
		EndNextSubdivision();
	}
	
	//*****************************************************************************************
	//
	forceinline void operator++
	(
	)
	//
	// Increments the u and v values for the next position on the horizontal scanline.
	//
	//**************************************
	{
		w2dTex += w2dDeltaTex;
	}

	//*****************************************************************************************
	//
	forceinline void operator +=
	(
		const CIndexPerspective& rind_linear
	)
	//
	// Adds two CLineLinearTexZ objects together and accumulates it in the calling object.
	//
	//**************************************
	{
		fUInvZ += rind_linear.fUInvZ;
		fVInvZ += rind_linear.fVInvZ;
		fInvZ  += rind_linear.fInvZ;
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
		fNegUInvZ = -fNegUInvZ;
		fNegVInvZ = -fNegVInvZ;
		fNegInvZ  = -fNegInvZ;
	}

	//*****************************************************************************************
	//
	forceinline void ModulusSubtract
	(
	)
	//
	// Perform the modulus subtraction. Note that no '-=' operator is available, but that
	// 'bfNegDU' and 'w1fNegDV' contain values that are already made negative.
	//
	//**************************************
	{
		fUInvZ += fNegUInvZ;
		fVInvZ += fNegVInvZ;
		fInvZ  += fNegInvZ;
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
	forceinline void InitializePolygonData
	(
		ptr_const<CTexture> ptex,			// Texture for polygon.
		int                 i_mip_level
	)
	//
	//**************************************
	{
		rptr<CRaster> pras_texture = ptex->prasGetTexture(i_mip_level);
		Assert(pras_texture);

		// Build the texture mask.
		BuildTileMask(pras_texture);

		// Set a pointer to the texture.
		pvTextureBitmap = pras_texture->pSurface;
		Assert(pvTextureBitmap);

		// Set the global texture width variable.
		iTexWidth = pras_texture->iLinePixels;

		// Initialize values used in debugging.
		#if VER_DEBUG
			iTexSize = pras_texture->iLinePixels * pras_texture->iHeight;
		#endif

		//
		// Scale the texture coordinates according to the width and height of the texture.
		//
		fTexWidth  = pras_texture->fWidth;
		fTexHeight = pras_texture->fHeight;
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
		// Copy scaled, unprojected texture coords to our private vertices.
		if (bClampUV)
		{
			prv_dst->tcTex.tX = (prv_src->tcTex.tX * fTexWidth  + fTexEdgeTolerance) * prv_src->v3Screen.tZ;
			prv_dst->tcTex.tY = (prv_src->tcTex.tY * fTexHeight + fTexEdgeTolerance) * prv_src->v3Screen.tZ;
		}
		else
		{
			prv_dst->tcTex.tX = (prv_src->tcTex.tX * fTexWidth) * prv_src->v3Screen.tZ;
			prv_dst->tcTex.tY = (prv_src->tcTex.tY * fTexHeight) * prv_src->v3Screen.tZ;
		}
	}

	//*****************************************************************************************
	//
	forceinline void InitializeTriangleData
	(
		SRenderVertex*  prv_1,	// First of three non-collinear coordinates describing the
								// plane of the triangle in screen coordinates.
		SRenderVertex*  prv_2,	// Second coordinate describing the plane.
		SRenderVertex*  prv_3,	// Third coordinate describing the plane.
		ptr_const<CTexture> ptex,
		float           f_yab_invdx,		// Multipliers for gradients.
		float           f_yac_invdx,
		bool			b_update = false,	// Modify the gradients rather than setting them.
		bool			b_altpersp = false	// Use alternate perspecive settings.
	)
	//
	// Initialize data used for the entire triangle, including converting texture coordinates
	// from unit values in the range [0..1] to pixel values in the range [0..WifTh - 1] and
	// [0..Height - 1], initializing the fDUInvZScanline and fDVInvZScanline values and finding the base address for the
	// clut associated with the texture.
	//
	//**************************************
	{
		Assert(prv_1);
		Assert(prv_2);
		Assert(prv_3);

		//
		// Get the step values for u and v with respect to x.
		//
		float f_duinvz = (prv_2->tcTex.tX - prv_1->tcTex.tX) * f_yac_invdx -
						 (prv_3->tcTex.tX - prv_1->tcTex.tX) * f_yab_invdx;

		float f_dvinvz = (prv_2->tcTex.tY - prv_1->tcTex.tY) * f_yac_invdx -
						 (prv_3->tcTex.tY - prv_1->tcTex.tY) * f_yab_invdx;

		float f_dinvz =  (prv_2->v3Screen.tZ - prv_1->v3Screen.tZ) * f_yac_invdx -
						 (prv_3->v3Screen.tZ - prv_1->v3Screen.tZ) * f_yab_invdx;

		if (b_update)
		{
			//SetMinAbs(fDUInvZ, f_duinvz);
			if (fabs(f_duinvz) < fabs(fDUInvZ))
				fDUInvZ = f_duinvz;

			//SetMinAbs(fDVInvZ, f_dvinvz);
			if (fabs(f_dvinvz) < fabs(fDVInvZ))
				fDVInvZ = f_dvinvz;

			SetMax(fDInvZ,  f_dinvz);
		}
		else
		{
			fDUInvZ = f_duinvz;
			fDVInvZ = f_dvinvz;
			fDInvZ  = f_dinvz;
		}

		// Calculate the subdivision length with respect to X.
		iSubdivideLen    = persetSettings.iGetSubdivisionLen(fDInvZ, b_altpersp);
		fInvSubdivideLen = fUnsignedInverseInt(iSubdivideLen);
	}

	//*****************************************************************************************
	//
	forceinline void InitializeAsBase
	(
		ptr_const<CTexture> ptex,
		SRenderVertex* prv_from,	// First of three non-collinear coordinates describing the
		SRenderVertex* prv_to,		// Second coordinate describing the plane.
		CIndexPerspective&  rind_linear_increment,
		float          f_inv_dy,
		float          f_x_diff_increment,
		float          f_x_diff_edge,
		float          f_y_diff_edge,
		int            i_mip_level 
	)
	//
	// Initializes the Z components of the edge, including starting position and edge w2ding
	// increment.
	//
	// Notes:
	//		The subpixel calculation made in CEdge is applied here.
	//
	//**************************************
	{
		Assert(prv_from);
		Assert(prv_to);

		//
		// Find the u and v increment values across x. The value is calculated from:
		//
		//		              du    (u1 - u0)
		//	   u increment = ---- = ---------
		//		              dy    (y1 - y0)
		//
		// with a similar formula for v.
		//
		float f_increment_u = f_inv_dy * (prv_to->tcTex.tX - prv_from->tcTex.tX);
		float f_increment_v = f_inv_dy * (prv_to->tcTex.tY - prv_from->tcTex.tY);
		float f_increment_z = f_inv_dy * (prv_to->v3Screen.tZ - prv_from->v3Screen.tZ);

		//
		// Calculate the increment for the U and V values for modulus subtraction.
		//
		fNegUInvZ = fDUInvZ;
		fNegVInvZ = fDVInvZ;
		fNegInvZ  = fDInvZ;

		fDUInvZEdge = fDUInvZ;
		fDVInvZEdge = fDVInvZ;
		fDInvZEdge  = fDInvZ;

		//
		// Set up the increments for edge walking. Note that the increment of the x difference
		// must be incorporated into this value.
		//
		rind_linear_increment.fUInvZ = f_increment_u + f_x_diff_increment * fDUInvZEdge;
		rind_linear_increment.fVInvZ = f_increment_v + f_x_diff_increment * fDVInvZEdge;
		rind_linear_increment.fInvZ  = f_increment_z + f_x_diff_increment * fDInvZEdge;

		//
		// Adjust u and v to the value they would be when the edge first intersects the first
		// horizontal line, and add the starting adjust values with respect to x. 
		//
		fUInvZ = prv_from->tcTex.tX + f_increment_u * f_y_diff_edge + f_x_diff_edge * fDUInvZEdge;
		fVInvZ = prv_from->tcTex.tY + f_increment_v * f_y_diff_edge + f_x_diff_edge * fDVInvZEdge;
		fInvZ  = prv_from->v3Screen.tZ   + f_increment_z * f_y_diff_edge + f_x_diff_edge * fDInvZEdge;

		float f_subdivide_len = float(iSubdivideLen);

		// Slopes times subdivision length minus one.
		fDUInvZEdgeMinusOne = fDUInvZEdge * (f_subdivide_len - 1.0f);
		fDVInvZEdgeMinusOne = fDVInvZEdge * (f_subdivide_len - 1.0f);
		fDInvZEdgeMinusOne  = fDInvZEdge * (f_subdivide_len - 1.0f);

		// Slopes times subdivision length.
		fDUInvZEdge *= f_subdivide_len;
		fDVInvZEdge *= f_subdivide_len;
		fDInvZEdge  *= f_subdivide_len;
	}

	//*****************************************************************************************
	//
	void AssertRange
	(
		ptr_const<CTexture> ptex,
		int                 i_mip_level
	)
	//
	// Tests for valid UV entries. Use in debug mode only.
	//
	//**************************************
	{
	#if VER_DEBUG
	#endif
	}

	//*****************************************************************************************
	//
	void AssertXRange
	(
		int i_xrange
	)
	//
	// Uses asserts to check if the range of texture coords for this scanline is valid.
	//
	//**************************************
	{
#if VER_DEBUG
#if VER_CLAMP_UV_TILE
		// Check starting values.
		float f_u = fUInvZ / fInvZ;
		float f_v = fVInvZ / fInvZ;

		// fTexWidth is a float slightly less than the width of the texture.
		// So int(fTexWidth) is the maximum integer value allowed.
		Assert(bWithin(int(f_u), 0, int(fTexWidth)));
		Assert(bWithin(int(f_v), 0, int(fTexHeight)));

		// Check ending values.
		float f_uinvz = fUInvZ + fDUInvZ * i_xrange;
		float f_vinvz = fVInvZ + fDVInvZ * i_xrange;
		float f_invz  = fInvZ  + fDInvZ  * i_xrange;

		f_u = f_uinvz / f_invz;
		f_v = f_vinvz / f_invz;

		Assert(bWithin(int(f_u), 0, int(fTexWidth)));
		Assert(bWithin(int(f_v), 0, int(fTexHeight)));
#endif
#endif
	}
};


//*********************************************************************************************
//
class CIndexPerspectiveFilter : public CIndexPerspective
//
// Perspective index that will give us the fractional part of the index.
//
//**************************************
{
public:
	//*****************************************************************************************
	//
	// CIndexLinearFilter constructors.
	//
	forceinline CIndexPerspectiveFilter() : CIndexPerspective()
	{
	}

	forceinline CIndexPerspectiveFilter(const CIndexPerspective& index, int i_pixel) : CIndexPerspective(index, i_pixel)
	{
	}
};


//
// Class implementations.
//

	#if VER_ASM

	//******************************************************************************************
	#if TARGET_PROCESSOR == PROCESSOR_PENTIUM
		#include "Lib/Renderer/Primitives/P5/IndexPerspectiveTEx.hpp"
	#elif TARGET_PROCESSOR == PROCESSOR_PENTIUMPRO
		#include "Lib/Renderer/Primitives/P6/IndexPerspectiveTEx.hpp"
	#elif TARGET_PROCESSOR == PROCESSOR_K6_3D
		#include "Lib/Renderer/Primitives/AMDK6/IndexPerspectiveTEx.hpp"
	#else
		#error Invalid [No] target processor specified
	#endif
	//******************************************************************************************

	#else

		//*************************************************************************************
		forceinline  void CIndexPerspective::InitializeSubdivision(int i_pixel)
		{
			float f_u;
			float f_v;
			float f_z;
			float f_next_z;

			// Get the next pixel index value to subdivide to.
			iNextSubdivide = iSetNextDividePixel(i_pixel);

			// Get current u, v and z values.
			f_z = 1.0f / fGInvZ;

			// Set current texture coordinates.
			f_u = fGUInvZ * f_z;
			f_v = fGVInvZ * f_z;

			if (bClampUV)
			{
				SetMinMax(f_u, fTexEdgeTolerance, fTexWidth);
				SetMinMax(f_v, fTexEdgeTolerance, fTexHeight);
			}

			w2dTex.Initialize(f_u, f_v, iTexWidth);

			// Increment u, v and z values.
			fGUInvZ += fDUInvZScanline;
			fGVInvZ += fDVInvZScanline;
			fGInvZ  += fDInvZScanline;

			// Get next u, v and z values.
			f_next_z = 1.0f / fGInvZ;

			// Set new texture coordinate increments.
			fU = fGUInvZ * f_next_z;
			fV = fGVInvZ * f_next_z;

			if (bClampUV)
			{
				SetMinMax(fU, fTexEdgeTolerance, fTexWidth);
				SetMinMax(fV, fTexEdgeTolerance, fTexHeight);
			}

			float f_inv_pixel = fUnsignedInverseInt(i_pixel - iNextSubdivide);

			w2dDeltaTex.Initialize
			(
				(fU - f_u) * f_inv_pixel,
				(fV - f_v) * f_inv_pixel,
				iTexWidth
			);

			// Get ready for the next subdivision.
			BeginNextSubdivision();
		}

		//*************************************************************************************
		forceinline  void CIndexPerspective::BeginNextSubdivision()
		{
			iCacheNextSubdivide = iSetNextDividePixel(iNextSubdivide);
		
			// Increment global U, V and inverse Z values.
			fGUInvZ += fDUInvZScanline;
			fGVInvZ += fDVInvZScanline;
			fGInvZ  += fDInvZScanline;

		}

		//*************************************************************************************
		forceinline  void CIndexPerspective::EndNextSubdivision()
		{
			float f_u, f_v, f_z;

			if (iNextSubdivide == iCacheNextSubdivide)
				return;

			float f_inv_pixel = fUnsignedInverseInt(iNextSubdivide - iCacheNextSubdivide);

			f_z = 1.0f / fGInvZ;
			f_u = fGUInvZ * f_z;
			f_v = fGVInvZ * f_z;

			if (bClampUV)
			{
				SetMinMax(f_u, fTexEdgeTolerance, fTexWidth);
				SetMinMax(f_v, fTexEdgeTolerance, fTexHeight);
			}

			w2dDeltaTex.Initialize
			(
				(f_u - fU) * f_inv_pixel,
				(f_v - fV) * f_inv_pixel,
				iTexWidth
			);

			fU = f_u;
			fV = f_v;

			iNextSubdivide = iCacheNextSubdivide;
			BeginNextSubdivision();
		}

	#endif


#endif
