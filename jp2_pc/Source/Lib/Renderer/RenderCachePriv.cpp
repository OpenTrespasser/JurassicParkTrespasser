/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Implementation of 'RenderCachePriv.hpp.'
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/RenderCachePriv.cpp                                      $
 * 
 * 6     10/06/98 5:53a Pkeet
 * Added support for disabling the page manager.
 * 
 * 5     9/20/98 5:50p Pkeet
 * If a hardware allocation fails, no raster is created and the cache is a null cache.
 * 
 * 4     9/20/98 4:46p Pkeet
 * Maded 'prasScreenHardware' work.
 * 
 * 3     8/27/98 9:14p Asouth
 * full path scoping reduced to relative
 * 
 * 2     8/09/98 7:36p Pkeet
 * Restored the page managed only functionality to render caches.
 * 
 * 1     8/09/98 5:11p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/


//
// Required includes.
//
#include "Common.hpp"
#include "RenderCachePriv.hpp"

#include <Lib/W95/WinInclude.hpp>
#include "Lib/GeomDBase/TexturePageManager.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Renderer/RenderCache.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/View/AGPTextureMemManager.hpp"


//
// Macros.
//


//
// Constants.
//


//
// External variables.
//
extern rptr<CTexturePageManager> ptexmCacheTextures;


//
// Class definitions.
//

//*********************************************************************************************
//
// class CRenderCache::CCacheResolution Implementation.
//


//*********************************************************************************************
//
// class CRenderCache::CPriv Implementation.
//

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	float CRenderCache::CPriv::fGetAngularDistortion(const CCamera& cam) const
	{
		float f_arccos_min_sqr;	// Smallest arccos (largest angular error) found.

		// Centre point of the camera.
		CVector3<> v3_cam = cam.v3Pos();

		// Get the smallest cosine of the angle.
		for (uint u = 0; u < pav3Actual.uLen; ++u)
		{
			//
			// Get the vectors defining the rays between the actual and projected control
			// points and the camera's position.
			//
			CVector3<> v3_actual(pav3Actual[u] - v3_cam);
			CVector3<> v3_projected(pav3Projected[u] - v3_cam);

			//
			// Find the square of the dot product between the two normalized vectors. As an
			// optimization, the normalization has been ommitted.
			//
			float f_mag_dot    = v3_actual * v3_projected;
			float f_mag_sqr    = v3_actual.tLenSqr() * v3_projected.tLenSqr();
			float f_arccos_sqr = f_mag_dot * f_mag_dot / f_mag_sqr;

			// Set the minimum arccos value.
			if (u == 0)
				f_arccos_min_sqr = f_arccos_sqr;
			else
				f_arccos_min_sqr = Min(f_arccos_min_sqr, f_arccos_sqr);
		}

		// Limit the range of angular cache error.
		SetMinMax(f_arccos_min_sqr, 0.0f, 1.0f);

		// Derive the error.
		float f_angular_error = 1.0f - f_arccos_min_sqr;

		// Return the angular error.
		return f_angular_error * rcsRenderCacheSettings.fDistortionWeight;
	}

	//*****************************************************************************************
	void CRenderCache::CPriv::GetBufferSize(int& ri_width, int& ri_height, const CVector2<>& v2_coord_min,
		                                    const CVector2<>& v2_coord_max, const CCamera& cam, float f_distance_sqr)
	{
		//
		// Find the inverse aspect ratio of the object (the ratio of the object's width to
		// the object's height).
		//
		float f_aspect_ratio = (v2_coord_max.tY - v2_coord_min.tY) /
			                   (v2_coord_max.tX - v2_coord_min.tX);
		TReal r_width  = rGetBufferWidth(v2_coord_min, v2_coord_max, cam);
		TReal r_height = rGetBufferHeight(v2_coord_min, v2_coord_max, r_width);

		// Request the given size.
		pcrResolution->RequestSize(r_width, r_height, f_distance_sqr);

		// Get the actual size.
		CVector2<int> v2 = pcrResolution->v2GetActualDim();
		ri_width  = v2.tX;
		ri_height = v2.tY;
	}

	//*****************************************************************************************
	void CRenderCache::CPriv::AllocateRaster(int i_width, int i_height)
	{
		bool b_d3d = d3dDriver.bUseD3D() && iLimitBytesD3D > 0;	// Flag indicating if the raster should be D3D.

		// Get rid of an existing raster allocations.
		DestroyRaster();

		//
		// Try hardware first.
		//
		if (b_d3d)
		{
			CVector2<int> v2_dim(i_width, i_height);
			
			if (d3dDriver.bIsPageManaged())
			{
				ptexmCacheTextures->AllocateRegion(rhD3DHandle, v2_dim, !d3dDriver.bUseRegionUploads());
			}

			// If the allocation has failed, allocate a discreet raster in hardware.
			if (!bIsHardware())
			{
				prasScreenHardware = rptr_cast(CRaster, rptr_new CRasterD3D(CRasterD3D::CInitDiscreet
				(
					i_width,
					i_height,
					ed3dtexSCREEN_TRANSPARENT
				)));
			}

			if (prasScreenHardware)
			{
				if (!prasScreenHardware->bVerifyConstruction())
					prasScreenHardware = rptr0;
			}
			
			// If a hardware allocation succeeded, no more work is required.
			if (bIsHardware())
			{
				iD3DBytes = v2_dim.tX * v2_dim.tY * 2;
				iTotalD3DBytes += iD3DBytes;
				return;
			}

			// Return even if no buffer is created.
			return;
		}

		//
		// Create a new screen raster in same pixel format as screen for use by the software
		// renderer.
		//
		//FixAspectRatio(i_width, i_height);
		prasScreenSoftware = rptr_cast(CRaster, rptr_new CRasterMem
		(
			i_width,				// Width of cache raster.
			i_height,				// Height of cache raster.
			16,						// Number of pixels for the screen format.
			0,						// Calculate stride.
			&prasMainScreen->pxf,	// Use the screen pixel formal.
			emtFixed				// Indicate that the fixed heap should be used
									// for memory allocations.
		));

		// Account for the texture memory used.
		iSysBytes = i_width * i_height * 2;
		iTotalSysBytes += iSysBytes;
	}

	//*****************************************************************************************
	void CRenderCache::CPriv::DestroyRaster()
	{
		// Get rid of an existing raster allocations.
		prasScreenSoftware = rptr0;
		prasScreenHardware = rptr0;
		if (rhD3DHandle)
		{
			ptexmCacheTextures->DeleteRegion(rhD3DHandle);
			rhD3DHandle.SetToNull();
		}

		// Account for memory.
		iTotalSysBytes -= iSysBytes;
		iTotalD3DBytes -= iD3DBytes;
		iSysBytes = 0;
		iD3DBytes = 0;
	}

	//*****************************************************************************************
	rptr<CRaster> CRenderCache::CPriv::prasScreenRegion()
	{
		// Return the software raster if there is one.
		if (prasScreenSoftware)
		{
			Assert(!bIsHardware());
			return prasScreenSoftware;
		}

		// If there is no software raster, there must be a hardware one.
		Assert(bIsHardware());

		if (prasScreenHardware)
			return prasScreenHardware;

		if (!rhD3DHandle)
			return rptr0;

		// Construct a temporary raster for the region in the page.
		CVector2<int> v2_origin = rhD3DHandle.v2GetOrigin();
		CVector2<int> v2_size   = rhD3DHandle.v2GetSize();

		// Make sure the aspect ratio for the cache is 1:1.
		//FixAspectRatio(v2_size.tX, v2_size.tY);

		rptr<CRaster> pras_region = rptr_cast(CRaster, rptr_new CRasterMem
		(
			ptexmCacheTextures->prasGetPage(rhD3DHandle.iGetPage()),
			SRect(v2_origin.tX, v2_origin.tY, v2_size.tX, v2_size.tY)
		));

		return pras_region;
	}

	//*****************************************************************************************
	rptr<CRaster> CRenderCache::CPriv::prasScreenPage()
	{
		// Return the software raster if there is one.
		if (prasScreenSoftware)
		{
			Assert(!bIsHardware());
			return prasScreenSoftware;
		}

		// If there is no software raster, there must be a hardware one.
		Assert(bIsHardware());

		if (prasScreenHardware)
			return prasScreenHardware;

		if (!rhD3DHandle)
			return rptr0;

		return ptexmCacheTextures->prasGetPage(rhD3DHandle.iGetPage());
	}

	//*****************************************************************************************
	void CRenderCache::CPriv::TransformTextureCoords()
	{
		// If the textures are not in hardware or are not paged managed, do nothing.
		if (!bIsHardware() || prasScreenHardware)
			return;
		Assert(pmshPolygon);

		CTransLinear2<> tlr2 = rhD3DHandle.tlr2ToPage();

		for (uint u = 0; u < pmshPolygon->pamvVertices.uLen; ++u)
		{
			pmshPolygon->pamvVertices[u].tcTex *= tlr2;
		}
	}
	
	//*****************************************************************************************
	void CRenderCache::CPriv::Upload()
	{
		// Do nothing if hardware is not active.
		if (!d3dDriver.bUseD3D() || !bIsHardware())
			return;

		// If the texture is a discreet allocation, upload it immediately.
		if (prasScreenHardware)
		{
			prasScreenHardware->bUpload();
			return;
		}

		// Defer full uploads.
		if (!d3dDriver.bUseRegionUploads())
			return;

		Assert(ptexmCacheTextures);
		Assert(prasScreenPage()->pd3dtexGet());

		// Make sure the pointer is unlocked.
		prasScreenPage()->Unlock();
		Verify(ptexmCacheTextures->bUploadRegion(rhD3DHandle));
	}
	
	//*****************************************************************************************
	void CRenderCache::CPriv::SetFeatureFlags()
	{
		// Define the feature set for the surface of a render cache mesh.	
		static const CSet<ERenderFeature> erfCacheFeatures    = Set(erfCOPY) + erfTEXTURE + erfTRANSPARENT +
														        erfPERSPECTIVE + erfTRAPEZOIDS;
		static const CSet<ERenderFeature> erfCacheFeaturesD3D = Set(erfD3D_CACHE) + erfTEXTURE + erfTRANSPARENT +
														        erfPERSPECTIVE + erfTRAPEZOIDS;

		Assert(pmshPolygon);
		Assert(pmshPolygon->ptexTexture);
		
		pmshPolygon->ptexTexture->seterfFeatures = (bIsHardware()) ? (erfCacheFeaturesD3D) :
		                                           (erfCacheFeatures);
	}