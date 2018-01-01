/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Classes for using different pixel indexing techniques for raster primitives.
 *
 * Notes:
 *		Class hierarchy:
 *			CIndex
 *				CIndexNone
 *				CIndexLinear
 *
 * To Do:
 *		Rework 'AssertRange.'
 *		The 'fTexEdgeTolerance' constant should be made to work in conjunction with the actual
 *		width and height of the texturemap being rendered. On small bitmaps renderered to large
 *		sizes, the fTexEdgeTolerance constant is too high -- pixels rendered from the border of
 *		the texture map may appear noticeably thin (in most cases, however, the effect is
 *		probably quite unnoticeable).
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/IndexT.hpp                                    $
 * 
 * 38    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 37    8/26/98 11:08a Rvande
 * Added explicit scoping to the fixed data type
 * 
 * 36    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 35    98.04.30 5:22p Mmouni
 * Added support for stippled texture.
 * 
 * 34    98.03.24 8:17p Mmouni
 * Made changes to support alternate perspective correction settings.
 * 
 * 33    97.11.14 11:54p Mmouni
 * Added CIndexLinearFilter index type.
 * 
 * 32    97.11.04 10:57p Mmouni
 * Made changes for K6-3D specific CIndexLinear.
 * 
 * 31    97.10.30 11:20a Mmouni
 * BuildTileMask() was not being called in InitializePolygonData causing the fTexWidth,
 * fTexHeight to be set incorrectly for non-tiled polygons.
 * 
 * 30    97/10/28 13:23 Speter
 * Restored previous SetMinAbs() code, which is needed for proper linear mapping with traps.
 * 
 * 29    97.10.27 1:27p Mmouni
 * Made changes to support the K6-3D.
 * 
 * 28    97.10.15 7:41p Mmouni
 * Removed support for right to left scanlines.
 * 
 * 27    10/10/97 1:44p Mmouni
 * Now gets tiling data directly from texture raster.
 * 
 * 26    9/15/97 2:03p Mmouni
 * Now sets u4TextureTileMaskStepU so that tiling works without any shifting.
 * 
 * 25    9/01/97 8:02p Rwyatt
 * bClampUV is defined in this file
 * GetIndex member of CIndexLinear ands with the texture tile mask to enable tiling
 * The TextureTileMask function has been modified to account for the run time clamping.
 * 
 * 24    97/08/17 4:14p Pkeet
 * Added the 'BuildTileMask' member function.
 * 
 * 23    8/17/97 12:18a Agrant
 * Put texture coord clamping on a VER switch.
 * 
 * 22    8/15/97 12:51a Rwyatt
 * Adjusted the texture tolerance to 0.2
 * 
 * 21    97/08/07 11:35a Pkeet
 * Added interface support for mipmapping.
 * 
 * 20    97/08/06 3:20p Pkeet
 * Added mipmapping interface.
 * 
 * 19    97/08/05 14:19 Speter
 * Slightly optimised polygon vertex setup by adding InitializeVertexData() functions.
 * 
 * 18    97/07/16 15:54 Speter
 * InitializeTriangleData now takes two params to use as coefficients in derivative calculation
 * rather than f_invdx; also takes b_update parameter to use for multiple-triangle gradient
 * calculations.   Added AssertXRange function to test the validity of a single scanline.
 * Removed Clamp of derivatives, as the texturing size threshold should now prevent errors here.
 * 
 * 17    97/07/07 14:05 Speter
 * Now copy vertices for current polygon locally to arvRasterVertices, and set tcTex to scaled
 * coordinates.
 * 
 * 16    97/06/27 15:32 Speter
 * Moved code into new InitializePolygonData(), added bIsPlanar().  Now use global fTexWidth and
 * fTexHeight rather than local versions (now need values across functions).
 * 
 * 15    97/06/03 18:48 Speter
 * Added bIsPerspective() member.
 * 
 * 14    97-03-31 22:21 Speter
 * Removed pvGetTexture() function, placed code into InitializeTriangleData().
 * 
 * 
 * 13    97/01/31 12:04p Pkeet
 * Fixed bug in 'AssertRange.'
 * 
 * 12    1/15/97 1:05p Pkeet
 * Added the 'i4StartSubdivision' member function.
 * 
 * 11    97/01/07 12:05 Speter
 * Updated for ptr_const.
 * 
 * 10    96/12/31 17:05 Speter
 * Changed some pointers to ptr<>.
 * 
 * 9     12/12/96 12:05p Pkeet
 * Added an extra parameter for calling the CIndex constructor on a per scanline basis.
 * 
 * 8     12/06/96 3:51p Pkeet
 * Removed the private keyword from the linear texturemapper.
 * 
 * 7     11/13/96 6:07p Pkeet
 * Changed the 'fTexEdgeTolerance' constant and added comments about it to the 'To Do' list.
 * 
 * 6     10/10/96 7:57p Pkeet
 * Made statically declared global variables externally declared variables.
 * 
 * 5     10/09/96 7:38p Pkeet
 * Added constructors. Removed the 'pvInitializeForScanline' member function and replaced it
 * with the 'pvGetTexture' member function.
 * 
 * 4     10/04/96 5:35p Pkeet
 * Added the '-' operator member function.
 * 
 * 3     10/03/96 6:18p Pkeet
 * Removed the 'u4GetClutBaseColour' member functions and place them in the MapT module.
 * 
 * 2     10/03/96 3:17p Pkeet
 * Added member functions and code associated with the former CLinearTexZ class here.
 * 
 * 1     10/02/96 4:39p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_INDEXT_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_INDEXT_HPP


//
// Includes.
//
#include "Walk.hpp"


//
// A vector of two float values used for clamping texture co-ordinates.
// Prefix: cv
//
struct ClampVals
{
	float U;
	float V;
};

//
// Packed floating point U,V.
// Prefix: pf
//
struct PackedFloatUV
{
	float U;
	float V;
};

//
// Packed fixed point U,V.
// Prefix: pfx
//
struct PackedFixedUV
{
	::fixed U;
	::fixed V;
};


//
// Global variables.
//

// Floating point version of the u and v step values over the horizontal axis.
extern float fDU, fDV;
extern const void* pvTextureBitmap;

#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)

extern ClampVals cvMaxCoords;

#define fTexWidth	cvMaxCoords.U
#define fTexHeight	cvMaxCoords.V

#else

extern float fTexWidth;
extern float fTexHeight;

#endif


// Mask used for tiling textures.
extern uint32	u4TextureTileMask;
extern uint32	u4TextureTileMaskStepU;
extern uint64	qUVMasks;
extern int		bClampUV;


#if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)

// Texture width.
extern int iTexWidth;

// Negative versions of the u and v step values over the horizontal axis.
extern PackedFloatUV pfNegD;

// 2D walk version of the u and v step values over the horizontal axis.
extern PackedFixedUV pfxDeltaTex;

#else

// Negative versions of the u and v step values over the horizontal axis.
extern UBigFixed bfNegDU;
extern CWalk1D   w1dNegDV;

// The seperate version of the u and v step values over the horizontal axis.
extern UBigFixed bfDeltaU;
extern CWalk1D   w1dDeltaV;

// 2D walk version of the u and v step values over the horizontal axis.
extern CWalk2D w2dDeltaTex;

#endif


#if VER_DEBUG
	extern int iTexSize;	// Variable used in debugging only.
#endif


//
// Class definitions.
//

//*********************************************************************************************
//
class CIndex
//
// Abstract base class for indexing bitmaps for raster primitives.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CIndex constructors.
	//

	forceinline CIndex()
	{
	}

	forceinline CIndex(const CIndex& index, int i_pixel)
	{
	}

	//*****************************************************************************************
	//
	// CIndex member functions.
	//

	forceinline void Mask()
	{
	}

	//*****************************************************************************************
	//
	static forceinline bool bIsIndexed
	(
	)
	//
	// Returns false.
	//
	//**************************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	static forceinline bool bIsPerspective
	(
	)
	//
	// Returns false.
	//
	//**************************************
	{
		return false;
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
		int32 i4_retval = ri4_pixel;
		ri4_pixel = 0;
		return i4_retval;
	}

	//*****************************************************************************************
	//
	forceinline int iGetIndex
	(
	)
	//
	// Returns zero.
	//
	//**************************************
	{
		return 0;
	}
	
	//*****************************************************************************************
	//
	forceinline void UpdatePerspective
	(
		int i_pixel
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}
	
	//*****************************************************************************************
	//
	forceinline void operator++
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

	//*****************************************************************************************
	//
	forceinline void operator +=
	(
		const CIndex& rind
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	void AssertRange
	(
		ptr_const<CTexture> ptex,
		int                 i_mip_level
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
	// Does nothing.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline void InitializeAsBase
	(
		ptr_const<CTexture> ptex,
		SRenderVertex* prv_from,	// First of three non-collinear coordinates describing the
		SRenderVertex* prv_to,		// Second coordinate describing the plane.
		CIndex&        rind_linear_increment,
		float          f_inv_dy,
		float          f_x_diff_increment,
		float          f_x_diff_edge,
		float          f_y_diff_edge,
		int            i_mip_level
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline void BuildTileMask
	(
		rptr<CRaster> pras	// Raster to build the tile for.
	)
	//
	// Sets the 'u4TextureTileMask' global to mask bits to provide an index into a power of
	// two texture.
	//
	// Notes:
	//	(MSM) Added check to not tile 16 bit rasters since the 16-bit primitives do not have
	//  the code to tile correctly in them.
	//
	//**************************************
	{
		// Just copy into globals from stuff in the texture.
		bClampUV = pras->bNotTileable;
		u4TextureTileMask = (pras->u4HeightTileMask << 9) | pras->u4WidthTileMask;
		u4TextureTileMaskStepU = pras->u4WidthTileMask;

#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)
		qUVMasks = ((uint64)pras->u4HeightTileMask << 48) |
				   ((uint64)pras->u4WidthTileMask << 32)  |
				   ((uint64)pras->u4HeightTileMask << 16) |
				   ((uint64)pras->u4WidthTileMask);
#endif
	}

};


//*********************************************************************************************
//
class CIndexNone : public CIndex
//
// Indexing class for no indexing.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CIndexNone constructors.
	//

	//*****************************************************************************************
	//
	forceinline CIndexNone()
	{
	}

	forceinline CIndexNone(const CIndexNone& index, int i_pixel)
	{
	}

	//*****************************************************************************************
	//
	// CIndexNone member functions.
	//

	forceinline void Mask()
	{
	}

	//*****************************************************************************************
	//
	forceinline void operator +=
	(
		const CIndexNone& rind_none
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline void InitializeAsBase
	(
		ptr_const<CTexture> ptex,
		SRenderVertex* prv_from,	// First of three non-collinear coordinates describing the
		SRenderVertex* prv_to,		// Second coordinate describing the plane.
		CIndexNone&    rind_linear_increment,
		float          f_inv_dy,
		float          f_x_diff_increment,
		float          f_x_diff_edge,
		float          f_y_diff_edge,
		int            i_mip_level
	)
	//
	// Does nothing.
	//
	//**************************************
	{
	}

};


#if (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)

//*********************************************************************************************
//
class CIndexLinear : public CIndex
//
// Linear texturemapping for 3DX using float U,V values for the edges.
//
// Notes:
//		All the FP values are packed in pairs and scaled by 65536.0 for easy
//		conversion to fixed point.
//
//**************************************
{
public:
	PackedFloatUV pfIndex;
	PackedFixedUV pfxTexture;

public:

	//*****************************************************************************************
	//
	// CIndexLinear constructors.
	//

	forceinline CIndexLinear()
	{
	}

	forceinline CIndexLinear(const CIndexLinear& index, int i_pixel)
	{
		// Bypass built in assignment overload, pfIndex is already scaled.
		pfxTexture.U.i4Fx = index.pfIndex.U;
		pfxTexture.V.i4Fx = index.pfIndex.V;
	}

	//*****************************************************************************************
	//
	// CIndexLinear member functions.
	//

	//*****************************************************************************************
	//
	static forceinline bool bIsIndexed
	(
	)
	//
	// Returns false.
	//
	//**************************************
	{
		return true;
	}

	//*****************************************************************************************
	//
	forceinline void Mask
	(
	)
	//
	// Masks U and V values.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline int iGetIndex
	(
	)
	//
	// Returns the index value into the bitmap representing the position reference by the u and
	// v values..
	//
	//**************************************
	{
		int i_index = (((pfxTexture.V.i4Fx & (uint32)qUVMasks) >> 16) * iTexWidth + ((pfxTexture.U.i4Fx >> 16) & (uint32)qUVMasks));

#if VER_CLAMP_UV_TILE
		Assert(i_index >= 0);
		Assert(i_index  < iTexSize);
#endif

		return i_index;
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
		pfxTexture.U.i4Fx += pfxDeltaTex.U.i4Fx;
		pfxTexture.V.i4Fx += pfxDeltaTex.V.i4Fx;
	}

	//*****************************************************************************************
	//
	forceinline void operator +=
	(
		const CIndexLinear& rind_linear
	)
	//
	// Adds two CLineLinearTexZ objects together and accumulates it in the calling object.
	//
	//**************************************
	{
		pfIndex.U += rind_linear.pfIndex.U;
		pfIndex.V += rind_linear.pfIndex.V;
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
		pfNegD.U = -pfNegD.U;
		pfNegD.V = -pfNegD.V;
	}

	//*****************************************************************************************
	//
	forceinline void ModulusSubtract
	(
	)
	//
	// Perform the modulus subtraction. Note that no '-=' operator is available, but that
	// 'bfNegDU' and 'w1dNegDV' contain values that are already made negative.
	//
	//**************************************
	{
		pfIndex.U += pfNegD.U;
		pfIndex.V += pfNegD.V;
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
		if (bClampUV)
		{
			prv_dst->tcTex.tX = prv_src->tcTex.tX * fTexWidth  + fTexEdgeTolerance;
			prv_dst->tcTex.tY = prv_src->tcTex.tY * fTexHeight + fTexEdgeTolerance;
		}
		else
		{
			prv_dst->tcTex.tX = prv_src->tcTex.tX * fTexWidth;
			prv_dst->tcTex.tY = prv_src->tcTex.tY * fTexHeight;
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
	// from unit values in the range [0..1] to pixel values in the range [0..Width - 1] and
	// [0..Height - 1], initializing the fDU and fDV values and finding the base address for the
	// clut associated with the texture.
	//
	//**************************************
	{
		Assert(prv_1);
		Assert(prv_2);
		Assert(prv_3);
		Assert(ptex);

		//
		// Get the step values for u and v with respect to x.
		//
		float f_du = (prv_2->tcTex.tX - prv_1->tcTex.tX) * f_yac_invdx -
					 (prv_3->tcTex.tX - prv_1->tcTex.tX) * f_yab_invdx;

		float f_dv = (prv_2->tcTex.tY - prv_1->tcTex.tY) * f_yac_invdx -
					 (prv_3->tcTex.tY - prv_1->tcTex.tY) * f_yab_invdx;

		if (b_update)
		{
			SetMinAbs(fDU, f_du);
			SetMinAbs(fDV, f_dv);
		}
		else
		{
			fDU = f_du;
			fDV = f_dv;
		}
	}

	//*****************************************************************************************
	//
	forceinline void InitializeAsBase
	(
		ptr_const<CTexture> ptex,
		SRenderVertex* prv_from,	// First of three non-collinear coordinates describing the
		SRenderVertex* prv_to,		// Second coordinate describing the plane.
		CIndexLinear&  rind_linear_increment,
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
		rptr<CRaster> pras_texture = ptex->prasGetTexture(i_mip_level);

		Assert(pras_texture);
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

		//
		// Calculate the increment for the U and V values for modulus subtraction.
		//
		pfNegD.U = fDU * 65536.0f;
		pfNegD.V = fDV * 65536.0f;

		//
		// Set up the 2D stepping values for rasterizing the scanline.
		//
		pfxDeltaTex.U = fDU;
		pfxDeltaTex.V = fDV;

		//
		// Set up the increments for edge walking. Note that the increment of the x difference
		// must be incorporated into this value.
		//
		rind_linear_increment.pfIndex.U = (f_increment_u + f_x_diff_increment * fDU) * 65536.0f;
		rind_linear_increment.pfIndex.V = (f_increment_v + f_x_diff_increment * fDV) * 65536.0f;

		//
		// Adjust u and v to the value they would be when the edge first intersects the first
		// horizontal line, and add the starting adjust values with respect to x. 
		//
		pfIndex.U = (prv_from->tcTex.tX + f_increment_u * f_y_diff_edge + f_x_diff_edge * fDU) * 65536.0f;
		pfIndex.V = (prv_from->tcTex.tY + f_increment_v * f_y_diff_edge + f_x_diff_edge * fDV) * 65536.0f;
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
		rptr<CRaster> pras_texture = ptex->prasGetTexture(i_mip_level);
		Assert(pras_texture);

		//
		// Check to see that texture coordinates are within the texture map.
		//
#if VER_CLAMP_UV_TILE
		Assert(pfIndex.U >= 0.0f);
		Assert(pfIndex.V >= 0.0f);
		
		Assert(pfIndex.U < pras_texture->iWidth * 65536.0f);
		Assert(pfIndex.V < pras_texture->iHeight * 65536.0f);
#endif
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
		float f_u = pfIndex.U * (1.0f/65536.0f);
		float f_v = pfIndex.V * (1.0f/65536.0f);

		// fTexWidth is a float slightly less than the width of the texture.
		// So int(fTexWidth) is the maximum integer value allowed.
		Assert(bWithin(int(f_u), 0, int(fTexWidth)));
		Assert(bWithin(int(f_v), 0, int(fTexHeight)));

		f_u += fDU * i_xrange;
		f_v += fDV * i_xrange;

		Assert(bWithin(int(f_u), 0, int(fTexWidth)));
		Assert(bWithin(int(f_v), 0, int(fTexHeight)));
#endif
#endif
	}
};


//*********************************************************************************************
//
class CIndexLinearFilter : public CIndexLinear
//
// Linear index that will give us the fractional part of the index.
//
//**************************************
{
public:
	//*****************************************************************************************
	//
	// CIndexLinearFilter constructors.
	//
	forceinline CIndexLinearFilter() : CIndexLinear()
	{
	}

	forceinline CIndexLinearFilter(const CIndexLinear& index, int i_pixel) : CIndexLinear(index, i_pixel)
	{
	}
};


#else // (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)


//*********************************************************************************************
//
class CIndexLinear : public CIndex
//
// Linear texturemapping using M. Abrash's trick for arbitrary sized bitmaps.
//
// Notes:
//		The trick used in walking through the scanline and finding the index into the texture
//		map comes from Michael Abrash, and is superficially explained in the April/May issue of
//		the Game Developer's Magazine ("Behind the Screen," C. Hecker, pages 25 - 33).
//		
//		Basically, instead of incrementing 'u' and 'v' texture coordinates separately, the
//		two values are combined; refer to 'CWalk2D' in 'Walk.hpp' for implementation details.
//		
//		To get an subpixel correct positions for the 'u' and 'v' texture coordinates it is
//		necessary to adjust these values by an amount representing the distance u and v would
//		walk between the x subpixel position on a scanline and the x position of the first
//		pixel lit on the scanline.
//		
//		Because the increment of the x starting position for the scanline is constant, the
//		u and v starting positions can be likewise incremented by a constant amount. The
//		original implementation of this module kept the adjustment value for u and v
//		separately, then incremented these values based on the fractional portion of the
//		x increment multiplied by du and dv step values with respect to x. Whenever the
//		fractional portion of the x increment would cause an overflow, the modulus of the
//		u and v adjustment values was obtained by subtracting du and dv step values.
//		
//		However, tracking u, v and their respective adjust values separately proved
//		unnecessary because the modulus for the adjust values required only a simple
//		subtraction. U and v values therefore are initialized with their x difference adjust
//		values added to their initial values, and their increments have the x difference
//		adjust increment built in. The modulus subtraction is performed as it was in earlier
//		implementations.
//		
//		Adjusting the 'u' and 'v' texture coordinates for the 'x' subpixel position cannot
//		be reliably done with 'CWalk2D' variables only. 'CWalk1D' variables are needed
//		because u and v coordinates must be separately incrememented and their subpixel
//		positions calculated right up until the scanline operation.
//		
//		Refer to the 'Notes' section of 'LineXDifference.hpp' for background information on
//		handling subpixel x differences.
//
//**************************************
{

public:

	UBigFixed  bfU;			// The 1D walk value for 'u' texture coordinates.
	CWalk1D    w1dV;		// The 1D walk value for 'v' texture coordinates.
	CWalk2D w2dTexture;		// The 2D walk value for scanline.

public:

	//*****************************************************************************************
	//
	// CIndexLinear constructors.
	//

	forceinline CIndexLinear()
	{
	}

	forceinline CIndexLinear(const CIndexLinear& index, int i_pixel)
	{
		w2dTexture.Initialize(index.bfU, index.w1dV);
	}

	//*****************************************************************************************
	//
	// CIndexLinear member functions.
	//

	//*****************************************************************************************
	//
	static forceinline bool bIsIndexed
	(
	)
	//
	// Returns false.
	//
	//**************************************
	{
		return true;
	}

	//*****************************************************************************************
	//
	forceinline void Mask
	(
	)
	//
	// Masks U and V values.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	forceinline int iGetIndex
	(
	)
	//
	// Returns the index value into the bitmap representing the position reference by the u and
	// v values..
	//
	//**************************************
	{
		w2dTexture.iUVInt[1] &= u4TextureTileMask;
		int i_index = w2dTexture.iUVInt[1];

#if VER_CLAMP_UV_TILE
		Assert(i_index >= 0);
		Assert(i_index  < iTexSize);
#endif

		return i_index;
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
		w2dTexture += w2dDeltaTex;
	}

	//*****************************************************************************************
	//
	forceinline void operator +=
	(
		const CIndexLinear& rind_linear
	)
	//
	// Adds two CLineLinearTexZ objects together and accumulates it in the calling object.
	//
	//**************************************
	{
		bfU  += rind_linear.bfU;
		w1dV += rind_linear.w1dV;
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
		bfNegDU  = -bfNegDU;
		w1dNegDV = -w1dNegDV;
	}

	//*****************************************************************************************
	//
	forceinline void ModulusSubtract
	(
	)
	//
	// Perform the modulus subtraction. Note that no '-=' operator is available, but that
	// 'bfNegDU' and 'w1dNegDV' contain values that are already made negative.
	//
	//**************************************
	{
		bfU  += bfNegDU;
		w1dV += w1dNegDV;
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
		if (bClampUV)
		{
			prv_dst->tcTex.tX = prv_src->tcTex.tX * fTexWidth  + fTexEdgeTolerance;
			prv_dst->tcTex.tY = prv_src->tcTex.tY * fTexHeight + fTexEdgeTolerance;
		}
		else
		{
			prv_dst->tcTex.tX = prv_src->tcTex.tX * fTexWidth;
			prv_dst->tcTex.tY = prv_src->tcTex.tY * fTexHeight;
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
	// from unit values in the range [0..1] to pixel values in the range [0..Width - 1] and
	// [0..Height - 1], initializing the fDU and fDV values and finding the base address for the
	// clut associated with the texture.
	//
	//**************************************
	{
		Assert(prv_1);
		Assert(prv_2);
		Assert(prv_3);
		Assert(ptex);

		//
		// Get the step values for u and v with respect to x.
		//
		float f_du = (prv_2->tcTex.tX - prv_1->tcTex.tX) * f_yac_invdx -
					 (prv_3->tcTex.tX - prv_1->tcTex.tX) * f_yab_invdx;

		float f_dv = (prv_2->tcTex.tY - prv_1->tcTex.tY) * f_yac_invdx -
					 (prv_3->tcTex.tY - prv_1->tcTex.tY) * f_yab_invdx;

		if (b_update)
		{
			SetMinAbs(fDU, f_du);
			SetMinAbs(fDV, f_dv);
		}
		else
		{
			fDU = f_du;
			fDV = f_dv;
		}
	}

	//*****************************************************************************************
	//
	forceinline void InitializeAsBase
	(
		ptr_const<CTexture> ptex,
		SRenderVertex* prv_from,	// First of three non-collinear coordinates describing the
		SRenderVertex* prv_to,		// Second coordinate describing the plane.
		CIndexLinear&  rind_linear_increment,
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
		rptr<CRaster> pras_texture = ptex->prasGetTexture(i_mip_level);

		Assert(pras_texture);
		Assert(prv_from);
		Assert(prv_to);

		int i_horizontal_line = pras_texture->iLinePixels;

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

		//
		// Calculate the increment for the U and V values for modulus subtraction.
		//
		bfDeltaU = fDU;
		w1dDeltaV.Initialize(fDV, i_horizontal_line);

		bfNegDU = bfDeltaU;
		w1dNegDV = w1dDeltaV;

		//
		// Set up the 2D stepping values for rasterizing the scanline according to
		// Abrash's trick.
		//
		w2dDeltaTex.Initialize(bfNegDU, w1dNegDV);

		//
		// Set up the increments for edge walking. Note that the increment of the x difference
		// must be incorporated into this value.
		//
		rind_linear_increment.bfU = f_increment_u + f_x_diff_increment * fDU;
		rind_linear_increment.w1dV.Initialize
		(
			f_increment_v + f_x_diff_increment * fDV,
			i_horizontal_line
		);

		//
		// Adjust u and v to the value they would be when the edge first intersects the first
		// horizontal line, and add the starting adjust values with respect to x. 
		//
		bfU = prv_from->tcTex.tX + f_increment_u * f_y_diff_edge + f_x_diff_edge * fDU;
		w1dV.Initialize
		(
			prv_from->tcTex.tY + f_increment_v * f_y_diff_edge + f_x_diff_edge * fDV,
			i_horizontal_line
		);
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
		rptr<CRaster> pras_texture = ptex->prasGetTexture(i_mip_level);
		Assert(pras_texture);

		//
		// Check to see that texture coordinates are within the texture map.
		//
#if VER_CLAMP_UV_TILE
		Assert(bfU.i8 >= (int64)0);
		Assert(w1dV.bfxValue.i8 >= (int64)0);
		
		Assert(bfU.i4Int < pras_texture->iWidth);
		Assert(w1dV.bfxValue.i4Int < iTexSize);
#endif
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
		float f_u = bfU.dGet();
		float f_v = w1dV.dGet();

		// fTexWidth is a float slightly less than the width of the texture.
		// So int(fTexWidth) is the maximum integer value allowed.
		Assert(bWithin(int(f_u), 0, int(fTexWidth)));
		Assert(bWithin(int(f_v), 0, int(fTexHeight)));

		f_u += fDU * i_xrange;
		f_v += fDV * i_xrange;

		Assert(bWithin(int(f_u), 0, int(fTexWidth)));
		Assert(bWithin(int(f_v), 0, int(fTexHeight)));
#endif
#endif
	}
};

#endif // (VER_ASM && TARGET_PROCESSOR == PROCESSOR_K6_3D)


#include "IndexPerspectiveT.hpp"


#endif
