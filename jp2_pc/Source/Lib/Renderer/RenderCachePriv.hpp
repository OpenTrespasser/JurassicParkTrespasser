/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Private member functions for CRenderCache.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/RenderCachePriv.hpp                                      $
 * 
 * 20    10/03/98 5:43a Pkeet
 * Fixed hardware error function.
 * 
 * 19    8/27/98 9:15p Asouth
 * added constructor so a class can be constructed
 * 
 * 18    8/19/98 1:32p Rwyatt
 * VC6.0 Warning fixes
 * 
 * 17    8/18/98 4:12p Pkeet
 * Caches now use the maximum recommended dimension for hardware.
 * 
 * 16    8/09/98 7:36p Pkeet
 * Restored the page managed only functionality to render caches.
 * 
 * 15    8/09/98 5:11p Pkeet
 * Added discreet allocations for render caches.
 * 
 * 14    7/20/98 10:27p Pkeet
 * Upgraded to Direct3D 4's interface.
 * 
 * 13    7/06/98 6:53p Pkeet
 * Fixed problem assert.
 * 
 * 12    7/06/98 6:52p Pkeet
 * Added code to maintain a 1:1 aspect ratio in hardware for render caches.
 * 
 * 11    7/02/98 10:37p Pkeet
 * Changed assert to include zero.
 * 
 * 10    7/01/98 6:45p Pkeet
 * Fixed bug that caused software render caches to be too low resolution.
 * 
 * 9     6/18/98 4:39p Pkeet
 * Disabled making small caches software.
 * 
 * 8     6/09/98 5:32p Pkeet
 * Added support for non-region uploads.
 * 
 * 7     6/09/98 3:53p Pkeet
 * Fixed bug that prevented image caches from being allocated in hardware.
 * 
 * 6     6/08/98 11:40a Pkeet
 * Changed hardwired values to use static cache parameters.
 * 
 * 5     5/26/98 12:56p Pkeet
 * Fixed assert.
 * 
 * 4     5/21/98 3:55p Pkeet
 * Small textures are now always allocated out of system memory.
 * 
 * 3     5/20/98 3:03p Pkeet
 * Implemented the page manager for Direct3D image caches.
 * 
 * 2     5/10/98 1:56p Pkeet
 * Removed the 'fGetTexelPixelError' member function.
 * 
 * 1     5/05/98 1:33p Pkeet
 * Initial implemenation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_RENDERCACHEPRIV_HPP
#define HEADER_LIB_RENDERER_RENDERCACHEPRIV_HPP


//
// Required includes.
//
#include <Lib/W95/WinInclude.hpp>
#include "Lib/GeomDBase/TexturePageManager.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Renderer/RenderCache.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/View/AGPTextureMemManager.hpp"


//
// Macros.
//

// Debug print values.
#define bDEBUG_PRINT_CACHE_VALUES (0)

#if bDEBUG_PRINT_CACHE_VALUES
	#include "Lib/Sys/DebugConsole.hpp"
#endif // bDEBUG_PRINT_CACHE_VALUES


//
// Constants.
//
const float fErrorIncrement = 0.48f;


//
// External variables.
//
extern rptr<CTexturePageManager> ptexmCacheTextures;


//
// Class definitions.
//

//*********************************************************************************************
//
class CRenderCache::CCacheResolution
//
// Encapsulates resolution tests for CRenderCache.
//
//**************************************
{
public:

	float         fDistanceWhenCacheBuiltSqr;	// Square of the distance to the global camera.
	CVector2<>    v2RequestDimWhenBuilt;		// Request size.
	CVector2<int> v2ActualDim;					// Actual size.
	CVector2<int> v2ActualDimLog2;				// Log 2 of the actual size.
	bool          bXMajor;
	int           iRequestedDim;
	int           iActualDim;
	int           iActualDimLog2;
	bool          bInHardware;					// Indicates that hardware is being used.
	static int    iCacheMaxDim;					// Maximum dimension for a cache in hardware.

public:

	//*****************************************************************************************
	//
	void RequestSize
	(
		float f_width,			// Requested width.
		float f_height,			// Requested height.
		float f_distance_sqr	// Current distance from the main camera to the partition.
	)
	//
	// Sets the requested dimensions and calculates the actual dimensions.
	//
	//**************************************
	{
		// Set the requested dimensions.
		v2RequestDimWhenBuilt.tX   = f_width;
		v2RequestDimWhenBuilt.tY   = f_height;
		fDistanceWhenCacheBuiltSqr = f_distance_sqr;

		//
		// Calculate the actual dimensions.
		//
		v2ActualDim.tX = iCalculateDim(f_width);
		v2ActualDim.tY = iCalculateDim(f_height);

		bInHardware = d3dDriver.bUseD3D();
		
		if (bInHardware)
			bXMajor = bFixAspectRatio(v2ActualDim.tX, v2ActualDim.tY);
		else
			bXMajor = (v2ActualDim.tX > v2ActualDim.tY);

		if (bXMajor)
		{
			iActualDim     = v2ActualDim.tX;
			iActualDimLog2 = uLog2(v2ActualDim.tX);
			iRequestedDim  = v2RequestDimWhenBuilt.tX;
		}
		else
		{
			iActualDim     = v2ActualDim.tY;
			iActualDimLog2 = uLog2(v2ActualDim.tY);
			iRequestedDim  = v2RequestDimWhenBuilt.tY;
		}
	}

	//*****************************************************************************************
	//
	float fDimError
	(
		float f_distance_sqr	// Current distance from the main camera to the partition.
	)
	//
	// Sets the requested dimensions and calculates the actual dimensions.
	//
	//**************************************
	{
		if (bInHardware)
		{
			float f_resize = fSqrtEst(fDistanceWhenCacheBuiltSqr / f_distance_sqr);

			// Get the difference in the log 2 of the new dimensions.
			int i_dim_log2  = uLog2(iCalculateDim(float(iRequestedDim) * f_resize));
			int i_diff_log2 = Abs(iActualDimLog2 - i_dim_log2);

			//
			// If the currently requested size dimension is four or more times the cache size at
			// its creation, force rebuilding always.
			//
			if (i_diff_log2 >= 2)
				return fAlwaysExecute * 0.2f;
			if (i_diff_log2 > 0)
				return fErrorIncrement;
			return 0.0f;
		}
		else
		{
			float f_resize;

			if (fDistanceWhenCacheBuiltSqr > f_distance_sqr)
				f_resize = fDistanceWhenCacheBuiltSqr / f_distance_sqr;
			else
				f_resize = f_distance_sqr / fDistanceWhenCacheBuiltSqr;
			if (f_resize > 3.0f)
				return fAlwaysExecute;
			if (f_resize < 1.4f)
				return 0.0f;
			return fErrorIncrement * f_resize;
		}
	}

	//*****************************************************************************************
	//
	CVector2<int> v2GetActualDim
	(
	) const
	//
	// Returns the actual dimensions.
	//
	//**************************************
	{
		return v2ActualDim;
	}

	//*****************************************************************************************
	//
	bool bFixAspectRatio
	(
		int ri_w_ras,	// Width of actual raster size.
		int ri_h_ras	// Height of actual raster size.
	)
	//
	// Alters the width or the height of the raster to maintain a 1:1 aspect ratio for render
	// caches.
	//
	//**************************************
	{
		int i_w_req = v2RequestDimWhenBuilt.tX;
		int i_h_req = v2RequestDimWhenBuilt.tY;

		// If the request is always less than the actual, simply use the requested.
		if (i_w_req <= ri_w_ras && i_h_req <= ri_h_ras)
		{
		#if bDEBUG_PRINT_CACHE_VALUES
			dprintf("Smaller req than actual cache found\n");
		#endif // bDEBUG_PRINT_CACHE_V

			ri_w_ras = i_w_req;
			ri_h_ras = i_h_req;

		#if bDEBUG_PRINT_CACHE_VALUES
			dprintf("Cache: %ld, %ld :: %ld, %ld\n", ri_w_ras, ri_h_ras, i_w_req, i_h_req);
		#endif // bDEBUG_PRINT_CACHE_V

			return ri_w_ras > ri_h_ras;
		}
		
	#if bDEBUG_PRINT_CACHE_VALUES
		dprintf("\nCache Start: %ld, %ld :: %ld, %ld\n", ri_w_ras, ri_h_ras, i_w_req, i_h_req);
	#endif // bDEBUG_PRINT_CACHE_V

		//
		// If the ratio of the raster width to the requested width is higher than the ratio
		// of the raster height to the requested height, the raster width must be reduced.
		//
		//		   raswidth    rasheight                            rasheight
		//		if -------- >= --------- than raswidth = reqwidth * ---------
		//		   reqwidth    reqheight                            reqheight
		//
		// or perform a similar conversion for height.
		//
		// To avoid an unnecessary divide, perform the comparison parametrically, e.g.,
		//
		//		raswidth    rasheight
		//		-------- >= --------- = raswidth * reqheight >= rasheight * reqwidth
		//		reqwidth    reqheight
		//
		int i_wras_hreq = ri_w_ras * i_h_req;
		int i_hras_wreq = ri_h_ras * i_w_req;

		if (i_wras_hreq >= ri_h_ras * i_w_req)
		{
			ri_w_ras = i_hras_wreq / i_h_req;
			return false;
		}
		else
		{
			ri_h_ras = i_wras_hreq / i_w_req;
			return true;
		}

	#if bDEBUG_PRINT_CACHE_VALUES
		dprintf("Cache Conv : %ld, %ld\n\n", ri_w_ras, ri_h_ras);
	#endif // bDEBUG_PRINT_CACHE_V
	}

private:

	//*****************************************************************************************
	//
	int iCalculateDim
	(
		float f_dim	// Requested dimension.
	) const
	//
	// Returns the actual dimension.
	//
	// Notes:
	//		The value is rounded to the nearest power-of-two, and must be in the range 8 to
	//		256.
	//
	//**************************************
	{
		int i_ret_val;
		int i_min = 8;
		int i_max = 256;

		Assert(f_dim >= 0.0f);

		// Bias to a smaller texture size.
		f_dim *= CRenderCache::fGeneralScale;

		if (d3dDriver.bUseD3D())
		{
			i_max = iCacheMaxDim;

			// The mantissa is stored as: (m - 1) * 2^iFLOAT_MANT_BIT_WIDTH.
			static const int iMANT_ROUND = int((sqrt(2.0) - 1) * (1 << iFLOAT_MANT_BIT_WIDTH));

			//
			// Convert to integer and round to the nearest power-of-two.
			//
			// Notes:
			//		Code stolen from 'WaveletQuadTree.cpp,' someone should really make this a
			//		standard function.
			//
			int i_u_log2 = CIntFloat(f_dim).i4GetExponent() -
						   ((iMANT_ROUND - CIntFloat(f_dim).i4GetMantissa()) >> (sizeof(int) * 8 - 1));

			// Clamp the value between 8 and 256 and return.
			i_ret_val = 1 << i_u_log2;
		}
		else
		{
			i_ret_val = int(f_dim);
		}
		return MinMax(i_ret_val, i_min, i_max);
	}

};


//*********************************************************************************************
//
class CRenderCache::CPriv : private CRenderCache
//
// Private implementation of CRenderCache.
//
//**************************************
{
#ifdef __MWERKS__
	// the base class has no default constructor; hence, this
	// class can have no default constructor either.  A constructor
	// is needed to make the class valid.
	CPriv();
#endif
public:

	//*****************************************************************************************
	//
	float fGetAngularDistortion
	(
		const CCamera& cam
	) const;
	//
	// Returns a value representing the distortion error between the image cache in its current
	// state and the same image cache if it were refreshed.
	//
	// Notes:
	//		The distortion threshold is an angle between a point positioned on the image cache
	//		and the actual calculated position of that point. For convenience, the angle is
	//		not used but the cosine of that angle; the cosine of the angle can therefore be
	//		found by calculating the dot product of the normals formed between a point and
	//		the camera and the point prime and the camera (given as actual and projected
	//		positions.
	//
	//		A further simplification can be made to avoid normalizing the vectors between
	//		the two points:
	//
	//			arccos =        A        *        B       
	//	                  --------------    -------------- ; returns true if arccos < threshold
	//                    Sqrt(A.LenSqr)    Sqrt(B.LenSqr)
	//
	//			       =            A * B
	//                    -------------------------
	//                    Sqrt(A.LenSqr x B.LenSqr)
	//
	//	              2                2
	//			arccos  =       (A * B)                              2            2
	//                    -------------------; returns true if arccos  < threshold
	//                    A.LenSqr x B.LenSqr
	//
	//		And further simplified to remove the divide:
	//
	//                                 2            2
	//			return true if: (A * B)  < threshold  x A.LenSqr x B.LenSqr
	//
	//**************************************

	//*****************************************************************************************
	//
	void GetBufferSize
	(
		int&              ri_width,			// Width of new buffers.
		int&              ri_height,		// Height of new buffers.
		const CVector2<>& v2_coord_min,		// Minimum camera space coordinates.
		const CVector2<>& v2_coord_max,		// Maximum camera space coordinates.
		const CCamera&    cam,				// Camera.
		float             f_distance_sqr	// Distance from the camera to the partition.
	);
	//
	// Sets the ri_width and ri_height parameters.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AllocateRaster
	(
		int i_width,
		int i_height
	);
	//
	// Allocates memory for hardware or software.
	//
	//**************************************

	//*****************************************************************************************
	//
	void DestroyRaster
	(
	);
	//
	// Deallocates memory for hardware or software.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bIsHardware
	(
	) const
	//
	// Returns 'true' if the allocation is in hardware.
	//
	//**************************************
	{
		return rhD3DHandle || prasScreenHardware;
	}

	//*****************************************************************************************
	//
	rptr<CRaster> prasScreenRegion
	(
	);
	//
	// Returns 'true' if the allocation is in hardware.
	//
	//**************************************

	//*****************************************************************************************
	//
	rptr<CRaster> prasScreenPage
	(
	);
	//
	// Returns 'true' if the allocation is in hardware.
	//
	//**************************************

	//*****************************************************************************************
	//
	void TransformTextureCoords
	(
	);
	//
	// Allocates memory for hardware or software.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void Upload
	(
	);
	//
	// Attempts a hardware upload. If it fails, it sets the cache to be rendered in software.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void SetFeatureFlags
	(
	);
	//
	// Sets the feature flag for the cache polygon based on whether the polygon will be rendered
	// in hardware or software.
	//
	//**************************************

};


#endif // HEADER_LIB_RENDERER_RENDERCACHEPRIV_HPP