/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Implementation of RenderCache.hpp.
 *
 * To do:
 *		Enable terrain for intersecting objects.
 *		Incorporate the 'Is Moving' test.
 *		Find rect extents by using shape's bounding polyhedron rather than all its points.
 *		Write an assert test to make sure the new camera is pointed at the shape's centre.
 *		Remove the line 'rensetSettings.bObjectAccept = false;'
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/RenderCache.cpp                                          $
 * 
 * 198   10/09/98 2:18a Pkeet
 * Set the polgyon to null.
 * 
 * 197   10/09/98 2:12a Pkeet
 * Caches with less than three vertices cannot finish being built.
 * 
 * 196   10/07/98 7:56a Pkeet
 * Slight buffering of numbers.
 * 
 * 195   10/07/98 6:14a Pkeet
 * Made the caches have a minimum dimension.
 * 
 * 194   10/06/98 5:53a Pkeet
 * Added support for disabling the page manager.
 * 
 * 193   10/05/98 7:06a Pkeet
 * Fixed depth sort for caches.
 * 
 * 192   10/02/98 2:41a Pkeet
 * Tweaked resolutions for caches.
 * 
 * 191   10/01/98 9:47p Pkeet
 * Changed the render cache list to an STL set.
 * 
 * 190   10/01/98 9:09p Pkeet
 * Fixed bug.
 * 
 * 189   10/01/98 8:38p Pkeet
 * Improved caching for Direct3D.
 * 
 * 188   10/01/98 12:35p Pkeet
 * New defaults.
 * 
 * 187   10/01/98 1:28a Pkeet
 * Upped cache ideal size.
 * 
 * 186   10/01/98 12:24a Pkeet
 * Fixed bug in the LRU.
 * 
 * 185   9/28/98 6:24p Pkeet
 * Fixed bug that left render cache behind when object moved.
 * 
 * 184   98/09/22 16:04 Speter
 * Fixed assert that occurred because null caches would never find a first object.
 * 
 * 183   9/20/98 5:50p Pkeet
 * If a hardware allocation fails, no raster is created and the cache is a null cache.
 * 
 * 182   9/20/98 4:46p Pkeet
 * Maded 'prasScreenHardware' work.
 * 
 * 181   9/19/98 3:18p Pkeet
 * Changed build order so that null caches do not think they are backfaced culled every frame.
 * 
 * 180   9/14/98 10:08p Pkeet
 * Fixed assert.
 * 
 * 179   9/01/98 1:45a Pkeet
 * Fixed assert.
 * 
 * 178   8/27/98 9:14p Asouth
 * loop variable scoping; separate temp fix awaiting new compiler
 * 
 * 177   98.08.19 6:01p Mmouni
 * Moved all quality setting values and multipliers into a single structure.
 * 
 * 176   8/18/98 4:12p Pkeet
 * Caches now use the maximum recommended dimension for hardware.
 * 
 * 175   8/09/98 7:36p Pkeet
 * Restored the page managed only functionality to render caches.
 * 
 * 174   8/09/98 5:11p Pkeet
 * Added discreet allocations for render caches.
 * 
 * 173   8/03/98 5:13p Kmckis
 * Fixed assert.
 * 
 * 172   8/01/98 4:43p Pkeet
 * Added calls to the 'pixSetBackground' member function to optionally set different background
 * colours for clearing render caches.
 * 
 * 171   7/23/98 9:08p Pkeet
 * Added code to track the creation and deletion of Direct3D textures using a macro switch in
 * 'RasterD3D.hpp.'
 * 
 * 170   7/23/98 6:29p Mlange
 * Must now call AllocPagesD3D().
 * 
 * 169   7/10/98 6:53p Mlange
 * Re-enabled use terrain intersection flag.
 * 
 * 168   7/07/98 7:40p Pkeet
 * Added occlusion for caches.
 * 
 * 167   7/07/98 6:40p Pkeet
 * Terrain intersection is turned off for now until Mark fixes the terrain query.
 * 
 * 166   7/06/98 6:52p Pkeet
 * Added code to maintain a 1:1 aspect ratio in hardware for render caches.
 * 
 * 165   7/03/98 4:45p Pkeet
 * Added automatic terrain intersection when building large caches.
 * 
 **********************************************************************************************/


//
// Includes.
//
#include <math.h>
#include <Lib/W95/WinInclude.hpp>
#include "Common.hpp"
#include "RenderCache.hpp"

#include "RenderCachePriv.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "Lib/Transform/VectorRange.hpp"
#include "RenderCacheHelp.hpp"
#include "Pipeline.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/GeomDBase/Partition.hpp"
#include "Lib/Renderer/LineSide2D.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/Renderer/Occlude.hpp"
#include "Lib/Sys/Scheduler.hpp"
#include "Lib/Math/FastSqrt.hpp"
#include "Lib/Math/FastInverse.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/View/RasterD3D.hpp"
#include "Lib/View/RasterPool.hpp"
#include "RenderCacheLRUItem.hpp"
#include "Lib/Renderer/ShapePresence.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/QualitySettings.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/Std/MemLimits.hpp"
#include "Lib/Sys/FixedHeap.hpp"
#include "Lib/Renderer/DepthSort.hpp"


//
// Macros.
//

// Switch to turn terrain intersections on for large caches.
#define bUSE_TERRAIN_INTERSECTION (1)


//
// Module constants.
//
const float fTurnSpeedValue = 0.0f;

// Radius squared at which partitions will caching intersecting with terrain.
#if bUSE_TERRAIN_INTERSECTION
	const float fCacheTerrainIntersectSqr = 625.0f;
#endif

//
// Module specific variables.
//
bool bListActive = false;

// Vector heap.
CDArray< CVector2<> > pav2Convex(u4MAX_CONVEX_VERTICES);
// Note: this heap should be replaced by a CLArray in the function that uses it when the
// debugging of this function is complete.


//
// Profile Stats.
//
static CProfileStat psUploadCache("Cache Upload", &proHardware.psHardware);
//static CProfileStat psUpdateCacheTotal("Cache Update", &proHardware.psHardware);
static CProfileStat psCreateCacheBuffers("Cache Create", &proHardware.psHardware);
static CProfileStat psFailed("Failed", &proHardware.psHardware);

//static CProfileStat psUpdateCache("Cache Update", &proProfile.psRender);


//
// Class implementations.
//

//*********************************************************************************************
//
// CRenderCache implementation.
//

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	//*****************************************************************************************
	CRenderCache::CRenderCache
	(
		CPartition* ppart_shape,
		const CCamera& cam
	) :
		ppartShape(ppart_shape),	// Copy master shape pointer.
		pcrResolution(new CCacheResolution()),
		iCount(0),
		bForceInvalid(true),
		iAge(0),
		iCacheID(iNextCacheID++),
		bInitialized(false),
		iFrameKey(0),
		bIsVisible(false),
		bNewlyVisible(false),
		fDistanceBuild(0.0f),
		bBuiltVisible(false),
		pmshPolygon(rptr0),
		iD3DBytes(0),
		iSysBytes(0),
		fDistanceLastTest(1.0f)
	{
		Assert(ppart_shape);
		Assert(ppart_shape->pbvBoundingVol());

		// Add the partition to the render cache list.
		renclRenderCacheList += ppartShape;
		iEuthansiaAge = 0;

		// Try Direct3D for this cache.
		pcrResolution->fDistanceWhenCacheBuiltSqr = ppart_shape->fDistanceFromGlobalCameraSqr();
	}

	//*****************************************************************************************
	CRenderCache::~CRenderCache()
	{
		priv_self.DestroyRaster();

		// Delete existing arrays.
		if (pav3Projected.atArray)
			delete[] pav3Projected.atArray;
		if (pav3Actual.atArray)
			delete[] pav3Actual.atArray;

		// Indicate that memory has already been deleted.
		pav3Projected.uLen    = 0;
		pav3Actual.uLen       = 0;
		pav3Projected.atArray = 0;
		pav3Actual.atArray    = 0;

		if (bListActive)
		{
			// Remove the partition from the render cache list.
			renclRenderCacheList -= ppartShape;
		}

		// Delete memory allocated for this object.
		delete pcrResolution;
		pcrResolution = 0;
	}

	//*****************************************************************************************
	void CRenderCache::GetMemory()
	{
		int i_limit = (Max(iLimitBytes, iLimitBytesD3D) * 15) >> 4;

		//
		// Delete memory until the allocation falls below the limit, or no more memory can be
		// deleted.
		//
		bool b_delete = true;
		while (b_delete && iGetTotalMem() > i_limit)
			b_delete = lruTerrainCache.bDelete();
	}

	//*****************************************************************************************
	//
	// Member functions.
	//
	
	//*****************************************************************************************
	float CRenderCache::fPriority(const CCamera& cam, CLightList& rltl_lights)
	{
		iEuthansiaAge = 0;

		// Rebuild the render cache always if forced.
		if (bForceInvalid)
		{
			bForceInvalid = false;
			return fAlwaysExecute;
		}

		Assert(ppartShape);

		// If the caches are frozen, never return an error.
		if (rcsRenderCacheSettings.bFreezeCaches)
			return 0.0f;

		// For debugging purposes.
		fAngularError = 0.0f;
		fTexelToPixel = 0.0f;

		// Get the approximate distance from the origin of the cache's partition to the camera.
		CVector3<> v3_cam_normal = ppartShape->v3Pos() - cam.v3Pos();
		fDistanceLastTest = fSqrtEst(v3_cam_normal.tLenSqr());

		// If this cache is not initialized, it has to be built regardless of available time.
		if (!bInitialized)
		{
			return (bIsVisible) ? (fAlwaysExecute) : (fTurnSpeedValue);
		}

		//
		// Force an update if the polygon representing the cache's mesh is rendered at too
		// oblique an angle; this will prevent gross errors like caches being backfaced culled.
		//
		{
			//
			// Get the normal of the plane in camera space and compare with the dot product of
			// the desired normal.
			//
			v3_cam_normal *= fInverse(fDistanceLastTest);

			// Make sure the cache does not get backface culled.
			if (d3CacheNormal * v3_cam_normal < 0.3f)
				return (bIsVisible) ? (fAlwaysExecute) : (fTurnSpeedValue);
		}

		// Get the relative distance in camera space.
		//float f_relative_dis = fDistanceLastTest / cam.campropGetProperties().rFarClipPlaneDist;
		//SetMinMax(f_relative_dis, 0.0f, 1.0f);
		//f_relative_dis = 1.1f - f_relative_dis;

		float f_distance_sqr = ppartShape->fDistanceFromGlobalCameraSqr();

		// Reduce the error drastically if the cache is not in view.
		if (!bIsVisible)
			return 0.0f;

		// If the camera is not moving, freeze optionally built image caches.
		if (!SPartitionSettings::bCameraHasMoved() && !bIsOldFrameKey())
		{
			return 0.0f;
		}

		//
		// Get the texel to screen pixel error in the range 0..1 with 0 representing a 1 to 1
		// (perfect) ratio.
		//
		//fTexelToPixel = priv_self.fGetTexelPixelError(cam);
		fTexelToPixel = pcrResolution->fDimError(f_distance_sqr);
		if (fTexelToPixel >= fAlwaysExecute)
			return fAlwaysExecute;

		//
		// Get the angular distortion as an arccos value in the range 0.0f to 1, with 0
		// representing no angular error.
		//
		fAngularError = priv_self.fGetAngularDistortion(cam);
		if (fAngularError >= fAlwaysExecute)
			return fAlwaysExecute;

		// Concatenate the errors.
		float f_error = fTexelToPixel + fAngularError;

		// Return the calculated error.
		//return f_error * f_relative_dis;
		return f_error * fInvSqrtEst(f_distance_sqr) * ppartShape->fGetCacheMultiplier();
	}
	
	//*****************************************************************************************
	void CRenderCache::UpdateRenderContext(const CCamera& cam, CLightList& rltl_lights)
	{
		CCycleTimer ctmr;

		Assert(fDistanceLastTest != 0.0f);

		// Initialize variables.
		iEuthansiaAge = 0;
		fCreatedScreenSize = ppartShape->fEstimateScreenSize(cam);
		bInitialized       = true;
		bNewlyVisible      = false;
		fDistanceBuild     = fDistanceLastTest;
		bBuiltVisible      = bIsVisible;

		// Get the intersecting objects flag.
		bool b_intersecting = rcsRenderCacheSettings.bAddIntersectingObjects && ppartShape->pdGetData().bCacheIntersecting;
		if (rcsRenderCacheSettings.bForceIntersecting)
			b_intersecting = true;

		// Maintain statistics if required.
		if (rcstCacheStats.bKeepStats)
		{
			rcstCacheStats.iNumCachesUpdated++;
		}

		// Create control points.
		CreateDistortionCoords(cam);

		CVector3<> v3_coord_min(0.0f, 0.0f, 0.0f);
		CVector3<> v3_coord_max(0.0f, 0.0f, 0.0f);

		// Create a new camera instance.  Copy main camera properties.
		CCamera cam_new(cam.pr3VPresence(rcsRenderCacheSettings.bUseCameraPrediction), cam.campropGetProperties());

		//
		// Set the object's points to the correct rectangle in 3space.
		// And set the camera's view properties, to render exactly to the cache raster.
		//
		CPresence3<> pr3_cam_new = cam_new.pr3Presence();

		// Set up the normal for the cache in world space.
		CVector3<> v3_diff = ppartShape->v3Pos() - cam.v3Pos();
		float f_len = v3_diff.tLen();
		v3_diff /= f_len;
		d3CacheNormal.tX = v3_diff.tX;
		d3CacheNormal.tY = v3_diff.tY;
		d3CacheNormal.tZ = v3_diff.tZ;

		//d3CacheNormal = CDir3<>(ppartShape->v3Pos() - cam.v3Pos());

		UpdateMeshAndCamera(pr3_cam_new, cam_new, *((CCamera*)&cam), v3_coord_min, v3_coord_max);
		if (!pmshPolygon)
		{
		#if (0)
			TCycles cy_total = ctmr();

			psUpdateCache.Add(cy_total, 1);
			psUpdateCacheTotal.Add(cy_total, 1);
			CScheduler::cyAccountedScheduleCycles += cy_total;
		#endif

			pcrResolution->RequestSize(1.0f, 1.0f, ppartShape->fDistanceFromGlobalCameraSqr());
			return;
		}
		if (pmshPolygon->pamvVertices.uLen < 3)
		{
			pmshPolygon = rptr0;
			pcrResolution->RequestSize(1.0f, 1.0f, ppartShape->fDistanceFromGlobalCameraSqr());
			return;
		}

		cam_new.SetPresence(pr3_cam_new);

		// Create new buffers based on the local camera and updated mesh.
		CreateBuffers(v3_coord_min, v3_coord_max, cam);

		// Fail to create the cache if no buffer is available.
		if (!priv_self.prasScreenPage())
		{
			pmshPolygon = rptr0;
			pcrResolution->RequestSize(1.0f, 1.0f, ppartShape->fDistanceFromGlobalCameraSqr());
			return;
		}

		// Update texture coordinates for the page manager.
		priv_self.TransformTextureCoords();

		// Set the local camera's properties.
		SetCameraProperties
		(
			cam_new,
			priv_self.prasScreenRegion(),
			rDistance,
			v3_coord_min,
			v3_coord_max
		);
		
		UpdateNewCameraSettings(cam, cam_new);

		// Construct the inverse presence of the camera.
		CPresence3<> pr3_inv_cam = ~cam_new.pr3Presence();

		// Set the control points for the cache's camera.
		SetControlPoints(cam_new);

		// Switch in correct raster.
		rptr<CRaster> pras = priv_self.prasScreenRegion();
		prenContext->pScreenRender->prasScreen = pras.ptGet();

		prenContext->pScreenRender->BeginFrame();

		// Restore the fpu state.
		CFPUState fpu;
		fpu.SetNormalRes();

		//
		// Build an occlusion list for objects within the render cache.
		//

		TOccludeList oclist;		// List of occluding polygons.

		// Get a list from the world database.
		GetOccludePolygons
		(
			(rcsRenderCacheSettings.bAddIntersectingObjects) ?	// Root partition node.
				(wWorld.ppartPartitionList()) : (ppartShape),
			pr3_inv_cam,										// Inverse camera presence.
			cam_new.pbvBoundingVol(),							// Camera bounding volume.
			oclist												// Linked list of visible
																// occluding objects.
		);
		
		// Create an array of occluding polygon objects.
		CLArray(COcclude*, papoc, oclist.size());

		// Copy the list to the array.
		CopyOccludePolygons(papoc, cam_new, oclist, false);

		//
		// Render to the cache.
		//

		// Use D3D cluts if required.
		if (priv_self.bIsHardware())
			CClut::SetClutState(ecsD3D_TRANSPARENT);

		//
		// Use less aggressive culling.
		//
		float f_cull, f_cull_shadow;
		CPartition::GetCombinedCulling(f_cull, f_cull_shadow);
		CPartition::SetCombinedCulling(2.0f * f_cull, f_cull_shadow);

		TCycles cy_before_rshape = proProfile.psRenderShape.cyGetCycles();

		//
		// Set the parent camera for the cache's camera to be the main camera. This will prevent
		// differences in rendering behaviour between cached and non-cached objects.
		//
		cam_new.SetParentCamera(&cam);

		// Fog the polygon.
		if (priv_self.bIsHardware() && d3dDriver.bUseCacheFog())
		{
			prenContext->pSettings->bHardwareCacheFog = true;
		}
		else
		{
			prenContext->pSettings->bHardwareCacheFog = false;
		}

		// Select background colour.
		TPixel pix = 0;
		if (priv_self.bIsHardware())
		{
			switch (d3dDriver.d3dcomGetCommonFormat(ed3dtexSCREEN_TRANSPARENT))
			{
				case ed3dcom5551_BRGA:
					pix = 0x0421;
					break;
				case ed3dcom1555_ARGB:
					pix = 0x2210;
					break;
				default:;
			}
		}
		pix = prenContext->pScreenRender->pixSetBackground(pix);

		// Set new depth sort settings.
		float f_depth_near  = ptsTolerances.rNearZScale;
		float f_depth_far   = ptsTolerances.rFarZScale;
		int   i_depth_polys = ptsTolerances.iMaxToDepthsort;

		ptsTolerances.iMaxToDepthsort = 300;

		float f_position = f_len / cam.campropGetProperties().rFarClipPlaneDist;
		SetMinMax(f_position, 0.0f, 1.0f);

		float f_depth_both;

		if (f_position >= ptsTolerances.rFarZ)
			f_depth_both = f_depth_far;
		else		
			f_depth_both = f_position * (f_depth_far - f_depth_near) + f_depth_near;

		ptsTolerances.rNearZScale = f_depth_both;
		ptsTolerances.rFarZScale  = f_depth_both;

		// Render intersecting or non-intersecting caches.
		if (b_intersecting)
		{
			// Get the terrain's mesh.
			CWDbQueryTerrainMesh wqtmsh;

			// Render all objects that intersect the new camera (including the target partition).
			prenContext->RenderScene
			(
				cam_new,						// Our own camera.
				rltl_lights,					// List of lights for the scene.
				wWorld.ppartPartitionList(),	// Render objects intersecting the new camera.
				papoc,							// Occlusion list.
				esfINTERSECT,					// Using intersecting objects.
				wqtmsh.tGet()					// Don't forget terrain.
			);
		}
		else
		{
		#if bUSE_TERRAIN_INTERSECTION
			bool b_intersect_terrain = ppartShape->rGetCylinderRadiusSqr() >= fCacheTerrainIntersectSqr;
		#else
			bool b_intersect_terrain = false;
		#endif

			// Render just the objects in the target partition.
			if (b_intersect_terrain)
			{
				// Get the terrain's mesh.
				CWDbQueryTerrainMesh wqtmsh;

				// Render all objects that intersect the new camera (including the target partition).
				prenContext->RenderScene
				(
					cam_new,		// Our own camera.
					rltl_lights,	// List of lights for the scene.
					ppartShape,		// Render objects intersecting the new camera.
					papoc,			// Occlusion list.
					esfINSIDE	,	// Not using intersecting objects.
					wqtmsh.tGet()	// Don't forget terrain.
				);
			}
			else
			{
				prenContext->RenderScene
				(
					cam_new,					// Our own camera.
					rltl_lights,				// List of lights for the scene.
					ppartShape,					// The target partition.
					papoc,						// Occlusion list.
					esfINSIDE					// Assert that the entire partition is contained.
				);
			}
		}

		TCycles cy_after_rshape = proProfile.psRenderShape.cyGetCycles();

		// Restore depth sort settings.
		ptsTolerances.rNearZScale = f_depth_near;
		ptsTolerances.rFarZScale  = f_depth_far;
		ptsTolerances.iMaxToDepthsort = i_depth_polys;

		// Restore regular culling.
		CPartition::SetCombinedCulling(f_cull, f_cull_shadow);

		prenContext->pScreenRender->EndFrame();
		prenContext->pScreenRender->prasScreen = 0;

		// Restore normal clut state.
		CClut::SetClutState(ecsSCREEN);

		TCycles cy_total = ctmr();
#if (0)

		// Remove these cycles from the onces counted by the scheduler.
		CScheduler::cyAccountedScheduleCycles += cy_total;

		psUpdateCache.Add(cy_total - (cy_after_rshape - cy_before_rshape), 1);

		psUpdateCacheTotal.Add(cy_total, 1);
#endif

		// Upload the image cache to hardware.
		priv_self.Upload();

		// Set the feature flags for the cache based on its type.
		priv_self.SetFeatureFlags();

		// Restore the previous renderer states.
		prenContext->pScreenRender->pixSetBackground(pix);

		// Set cache age.
		iCount = 0;
		psUploadCache.Add(ctmr(), 1);

		AlwaysAssert(pmshPolygon->pamvVertices.uLen >= 3);
	}
	
	//*****************************************************************************************
	//
	TReal CRenderCache::rGetScaleToScreen
	(
		const CCamera& cam	// Camera.
	)
	//
	// Returns the scale value from converting from projected to screen coordinates.
	//
	//**************************************
	{
		float f_ratio = 1.0f;

		return f_ratio / rDistance * cam.campropGetProperties().vpViewport.scWidth;
	}
	
	//*****************************************************************************************
	//
	TReal CRenderCache::rGetBufferWidth
	(
		const CVector2<>& v2_coord_min,	// Minimum camera space coordinates.
		const CVector2<>& v2_coord_max,	// Maximum camera space coordinates.
		const CCamera&    cam			// Camera.
	)
	//
	// Returns the new buffer's width.
	//
	//**************************************
	{
		return Abs(v2_coord_max.tX - v2_coord_min.tX) * rGetScaleToScreen(cam);
	}
	
	//*****************************************************************************************
	//
	TReal CRenderCache::rGetBufferHeight
	(
		const CVector2<>& v2_coord_min,		// Minimum camera space coordinates.
		const CVector2<>& v2_coord_max,		// Maximum camera space coordinates.
		TReal r_width						// Scale from camera to screen space.
	)
	//
	// Returns the new buffer's width.
	//
	//**************************************
	{
		//
		// Find the inverse aspect ratio of the object (the ratio of the object's width to
		// the object's height).
		//
		float f_aspect_ratio = (v2_coord_max.tY - v2_coord_min.tY) /
			                   (v2_coord_max.tX - v2_coord_min.tX);

		return r_width * f_aspect_ratio;
	}
	
	//*****************************************************************************************
	int CRenderCache::iGetMemUsed()
	{
		rptr<CRaster> pras;

		if (priv_self.bIsHardware())
		{
			if (prasScreenHardware)
				pras = prasScreenHardware;
			else
				pras = priv_self.prasScreenRegion();
		}
		else
		{
			pras = prasScreenSoftware;
		}

		if (!pras)
			return 0;

		return pras->iWidth * pras->iHeight * pras->iPixelBits / 8;
	}

	//*****************************************************************************************
	//
	void CRenderCache::CreateBuffers
	(
		const CVector2<>& v2_coord_min,	// Minimum camera space coordinates.
		const CVector2<>& v2_coord_max,	// Maximum camera space coordinates.
		const CCamera&    cam			// Original camera.
	)
	//
	// Creates a new screen buffer, also creates a new z buffer or assigns a global z buffer.
	//
	//**************************************
	{
		Assert(ppartShape);

		CCycleTimer ctmr;
		int i_width;	// Width of screen buffer.
		int i_height;	// Height of screen buffer.

		// Make sure the LRU is not too full.
		GetMemory();

		// Find the width and the height of the cache's buffers.
		priv_self.GetBufferSize
		(
			i_width,
			i_height,
			v2_coord_min,
			v2_coord_max,
			cam,
			ppartShape->fDistanceFromGlobalCameraSqr()
		);

		priv_self.AllocateRaster(i_width, i_height);

		rptr<CRaster> pras = priv_self.prasScreenPage();
		if (!pras)
			return;

		// Create the new render context.
		psrScreenR = prenMain->pScreenRender->psrCreateCompatible(pras);

		//
		// Update the textures in the cache's mesh.
		//
		SetupMesh();
		psCreateCacheBuffers.Add(ctmr(), 1);
	}

	//******************************************************************************************
	//
	void CRenderCache::UpdateNewCameraSettings
	(
		const CCamera& cam,		// Original camera pointing to the cache.
		      CCamera& cam_new	// Camera to update settings for.
	)
	//
	// Performs the new camera settings.
	//
	//**************************************
	{
		//
		// Associate a new, modified render settings structure.
		//

		// Copy the settings from the main renderer.
		rensetSettings = *prenMain->pSettings;

		// Fill the SSettings structure with new settings.
		rensetSettings.bClearBackground  = true;
		rensetSettings.bDrawSky			 = false;
		rensetSettings.clrBackground     = CColour(0, 0, 0);
		rensetSettings.bHalfScanlines    = false;

		//*************************************************************************************
		//
		rensetSettings.bObjectAccept = false;
		//
		// Note:
		//		In the best of worlds, this setting should not be necessary; however an assert
		//		occurs in the pipeline without it.
		//
		//*************************************************************************************

		// Don't recursively try to render cache.
		rensetSettings.bRenderCache = false;

		// Indicate that the target is a render cache.
		rensetSettings.bTargetCache = true;

		// Indicate that scheduled operations should not be executed.
		rensetSettings.bExecuteScheduler = false;

		// Create a function for converting fog values.
		rensetSettings.remRemapPosition = CRenderer::SSettings::CRemapPosition
		(
			cam.campropGetProperties().rFarClipPlaneDist,
			cam_new.campropGetProperties().rFarClipPlaneDist
		);

		// Create our own CRenderer object.
		prenContext = new CRenderer(psrScreenR, &rensetSettings);
		psrScreenR = 0;

		// Make sure the new settings work.
		prenContext->UpdateSettings();
	}

	//*****************************************************************************************
	//
	void CRenderCache::SetupMesh
	(
	)
	//
	// Sets up the 'pmshPolygon' object with the appropriate initial positions and textures.
	//
	//**************************************
	{
		Assert(pmshPolygon);
		Assert(pmshPolygon->ptexTexture);

		pmshPolygon->ptexTexture->AddMipLevel(rptr_cast(CRaster, priv_self.prasScreenPage()));

		// Force transparent (wasn't set that way before).
		pmshPolygon->ptexTexture->seterfFeatures += erfTRANSPARENT;
	}

	//*********************************************************************************************
	void CRenderCache::UpdateMeshAndCamera
	(
		CPresence3<>& pr3_cam_new,
		CCamera&      cam_new,
		CCamera&      cam,
		CVector3<>&   rv3_coord_min,
		CVector3<>&   rv3_coord_max
	)
	//
	// Calculate the 4 points for the cached image rectangle, in the shape's coordinate space.  
	// This rectangle is at the front of the shape, facing the camera, with its z-axis 
	// aligned with the world's z-axis.  It is large enough to enclose the shape's projection.
	//
	{
		CClosestRenderPoint rcrp;	// Storage for the closest point to the main camera.

		//
		// Point the camera's y-axis at the shape.
		//

		//
		// Construct a rotation which transforms the camera's default frame to a frame where its
		// y-axis points at the shape, and its z-axis remains vertical.
		//
		CVector3<> v3_shape_cam = ppartShape->v3Pos() - pr3_cam_new.v3Pos;
		CVector3<> v3_shape_cam_xy = CVector2<>(v3_shape_cam);

		pr3_cam_new.r3Rot = CRotate3<>(d3YAxis, v3_shape_cam_xy) *
							CRotate3<>(v3_shape_cam_xy, v3_shape_cam);


		CTransform3<> tf3_cam       = ~pr3_cam_new;
		CTransform3<> tf3_shape_cam = ppartShape->pr3Presence() * tf3_cam;

		//
		// Find the distance to the plane which replaces the object.
		// We use the front of the object's bounding sphere, so as to minimise sorting errors.
		// 

		rDistance = v3_shape_cam.tLen();

		Assert(rDistance > 0);

		//
		// Create the points in camera space.  The first step is to transform each of the shape's
		// points to camera space, and project it onto the plane intersecting the shape's centre.
		// Find the maximum extents of these projections.
		//

		// Clear the point heap.
		pav2Convex.Reset(0, false, u4CONVEX_VERTICES_COMMIT);
		pav2Convex.uLen = 0;

		//
		// Get the bounding coordinates of the shape(s) in camera space.
		//

		// Set outer bounds for minimum and maximum coordinates.
		rv3_coord_min = CVector3<>(FLT_MAX, FLT_MAX, FLT_MAX);
		rv3_coord_max = CVector3<>(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		// Make a list of renderable partitions.
		TPPartitionList partlist;
		uint u_num_points = uBuildPartitionList
		(
			partlist,	// Resulting list of partitions.
			ppartShape,	// Root partition.
			cam.v3Pos()	// Viewer (camera) position.
		);

		//
		// Do not build a cache if there are no cacheable objects.
		//
		if (partlist.size() < 1 || u_num_points < 3)
		{
			pmshPolygon = rptr0;
			return;
		}

		bool b_use_box_wrap = u_num_points >= u4CONVEX_VERTICES_ALLOW;
		bool b_add_convex   = !b_use_box_wrap || partlist.size() * 8 < u4CONVEX_VERTICES_ALLOW;

		// Project bounding polyhedra coordinates and get the minimum and maximum values.
		{
			TPPartitionList::iterator it = partlist.begin();
			for (; it != partlist.end(); ++it)
			{
				GetBoundingCoords
				(
					*it,
					tf3_cam,
					rDistance,
					pav2Convex,
					rv3_coord_min,
					rv3_coord_max,
					rcrp,
					b_use_box_wrap,
					b_add_convex				
				);
			}
		}

		//
		// Do not build a cache if there are no cacheable objects.
		//
		if (!rcrp.bFoundFirst || pav2Convex.uLen <= 2)
		{
			pmshPolygon = rptr0;
			return;
		}

		// Note: change AlwaysAssert's to Assert's.
		Assert(bInsideFloatRange(rv3_coord_min));
		Assert(bInsideFloatRange(rv3_coord_max));

		// Use box if possible.
		if (!b_add_convex)
			goto USE_BOX;
		if (!rcsRenderCacheSettings.bUseConvexPolygons)
			goto USE_BOX;

		{
			//
			// Get the area of the rectangular cache area, and determine whether a polygon
			// algorithm should be applied, or a simple rectangle used.
			//
			TReal r_width  = rGetBufferWidth(rv3_coord_min, rv3_coord_max, cam);
			TReal r_height = rGetBufferHeight(rv3_coord_min, rv3_coord_max, r_width);

			// Test to see if a plain rectangle will do.
			if (r_width * r_height < rcsRenderCacheSettings.rMinPolygonMesh)
				goto USE_BOX;

			//
			// Create a simplified convex polygon.
			//
			TReal r_min_z = rv3_coord_min.tZ;
			TReal r_max_z = rv3_coord_max.tZ;
			TReal r_scale = r_width / (rv3_coord_max.tX - rv3_coord_min.tX);

			GrahamScan(pav2Convex, r_scale, rv3_coord_min, rv3_coord_max);
			if (pav2Convex.uLen >= iMAX_RASTER_VERTICES)
				goto USE_BOX;
			
			// Set outer bounds for minimum and maximum coordinates.
			rv3_coord_min = CVector3<>(FLT_MAX, FLT_MAX, FLT_MAX);
			rv3_coord_max = CVector3<>(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			GetMinMax2D(pav2Convex, rv3_coord_min, rv3_coord_max);
			rv3_coord_min.tZ = r_min_z;
			rv3_coord_max.tZ = r_max_z;

			// Set the points defining the mesh.
			FudgeRange(rv3_coord_min, rv3_coord_max);
			DefineMesh(pr3_cam_new, rv3_coord_min, rv3_coord_max, pav2Convex);
		}
		return;

	USE_BOX:

		// Create the polygon mesh as a rectangle.
		pav2Convex.uLen = 4;
		pav2Convex[0] = CVector2<>(rv3_coord_min.tX, rv3_coord_min.tY);
		pav2Convex[1] = CVector2<>(rv3_coord_max.tX, rv3_coord_min.tY);
		pav2Convex[2] = CVector2<>(rv3_coord_max.tX, rv3_coord_max.tY);
		pav2Convex[3] = CVector2<>(rv3_coord_min.tX, rv3_coord_max.tY);
		FudgeRange(rv3_coord_min, rv3_coord_max);
		DefineMesh(pr3_cam_new, rv3_coord_min, rv3_coord_max, pav2Convex);
	}

	//*****************************************************************************************
	//
	void CRenderCache::DefineMesh
	(
		CPresence3<>&                pr3_cam_new,	// The new camera's presence.
		const CVector3<>&            v3_min,		// Minimum camera space coordinates.
		const CVector3<>&            v3_max,		// Maximum camera space coordinates.
		const CPArray< CVector2<> >& pav2			// Array of mesh points.
	)
	//
	// Creates and defines the shape of the polygon mesh.
	//
	// Notes:
	//		The memory allocated by the point arrays for the mesh and texture coordinates should
	//		be deleted by the mesh.
	//
	//**************************************
	{
		// Create the array of points for the mesh.
		CLArray(CVector3<>, pav3_points, pav2.uLen);

		// Create the texture coords.
		CLArray(CTexCoord, patc_tex, pav2.uLen);

		// We now have the rectangle points in camera space.  Transform these to the shape's space.
		CTransform3<> tf3_cam_shape = pr3_cam_new * ~ppartShape->pr3Presence();
		float f_invdx = 0.5f / (Max(Abs(v3_max.tX), Abs(v3_min.tX)));
		float f_invdy = 0.5f / (Max(Abs(v3_max.tY), Abs(v3_min.tY)));

		// Transform the points to the partition's space.
		for (uint u = 0; u < pav2.uLen; ++u)
		{
			pav3_points[u] = CVector3<>(pav2[u].tX, rDistance, pav2[u].tY) * tf3_cam_shape;
			patc_tex[u]    = CTexCoord
			(
				pav2[u].tX * f_invdx + 0.5f,
				1.0f - (pav2[u].tY * f_invdy + 0.5f)
			);
			Assert(bWithin(patc_tex[u].tX, 0.0, 1.0));
			Assert(bWithin(patc_tex[u].tY, 0.0, 1.0));
		}

		// Create a texture, and force the feature set.
		rptr<CTexture> ptex = rptr_new CTexture(CColour(255, 0, 0), &matDEFAULT);

		// Create the mesh.
		pmshPolygon = rptr_new CShapeCache
		(
			pav3_points,	// Vertex positions.
			patc_tex,		// Vertex texture coordinates.
			ptex			// Surface type for polygon.
		);
	}

	//*****************************************************************************************
	//
	void CRenderCache::DefineRectMesh
	(
		CPresence3<>&     pr3_cam_new,	// The new camera's presence.
		const CVector3<>& v3_min,		// Minimum camera space coordinates.
		const CVector3<>& v3_max		// Maximum camera space coordinates.
	)
	//
	// Creates and defines a rectangular polygon mesh.
	//
	// Notes:
	//		The memory allocated by the point arrays for the mesh and texture coordinates should
	//		be deleted by the mesh.
	//
	//**************************************
	{
		AlwaysAssert(0);
		// Reset array.
		pav2Convex.Reset(0,false,u4CONVEX_VERTICES_COMMIT);
		pav2Convex.uLen = 0;

		// Create the array of points for the mesh.
		CPArray< CVector3<> > pav3_points(4);

		// Create the texture coords.
		CLArray(CTexCoord, patc_tex, 4);

		float f_invdx = 0.5f / (Max(Abs(v3_max.tX), Abs(v3_min.tX)));
		float f_invdy = 0.5f / (Max(Abs(v3_max.tY), Abs(v3_min.tY)));

		// We now have the rectangle points in camera space.  Transform these to the shape's space.
		CTransform3<> tf3_cam_shape = pr3_cam_new * ~ppartShape->pr3Presence();

		// Transform the points to the partition's space.
		pav3_points[0] = CVector3<>(v3_min.tX, rDistance, v3_min.tY) * tf3_cam_shape;
		pav3_points[1] = CVector3<>(v3_max.tX, rDistance, v3_min.tY) * tf3_cam_shape;
		pav3_points[2] = CVector3<>(v3_max.tX, rDistance, v3_max.tY) * tf3_cam_shape;
		pav3_points[3] = CVector3<>(v3_min.tX, rDistance, v3_max.tY) * tf3_cam_shape;

		/*
		// Set the texture coordinates.
		patc_tex[0] = CVector2<>(0.0f, 1.0f);
		patc_tex[1] = CVector2<>(1.0f, 1.0f);
		patc_tex[2] = CVector2<>(1.0f, 0.0f);
		patc_tex[3] = CVector2<>(0.0f, 0.0f);
		*/
		// Set the texture coordinates.
		patc_tex[0] = CVector2<>(v3_min.tX * f_invdx + 0.5f, 1.0f - (v3_min.tY * f_invdy + 0.5f));
		patc_tex[1] = CVector2<>(v3_max.tX * f_invdx + 0.5f, 1.0f - (v3_min.tY * f_invdy + 0.5f));
		patc_tex[2] = CVector2<>(v3_max.tX * f_invdx + 0.5f, 1.0f - (v3_max.tY * f_invdy + 0.5f));
		patc_tex[3] = CVector2<>(v3_min.tX * f_invdx + 0.5f, 1.0f - (v3_max.tY * f_invdy + 0.5f));

		// Create a texture, and force the feature set.
		rptr<CTexture> ptex = rptr_new CTexture(CColour(255, 0, 0), &matDEFAULT);

		// Create the mesh.
		pmshPolygon = rptr_new CShapeCache
		(
			pav3_points,	// Vertex positions.
			patc_tex,		// Vertex texture coordinates.
			ptex			// Surface type for polygon.
		);
	}

	//*****************************************************************************************
	//
	void CRenderCache::CreateDistortionCoords
	(
		const CCamera&      cam
	)
	//
	// Fills the CPArray with coordinates representing the current positions of the distortion
	// shape.
	//
	//**************************************
	{
		// Centre point of the camera.
		CVector3<> v3_cam = v3GetCameraPos(cam);

		// Delete existing arrays.
		if (pav3Projected.atArray)
			delete[] pav3Projected.atArray;
		if (pav3Actual.atArray)
			delete[] pav3Actual.atArray;

		// For now, just use the bounding box.
		pav3Projected.uLen    = 8;
		pav3Actual.uLen       = 8;
		pav3Projected.atArray = new CVector3<>[8];
		pav3Actual.atArray    = new CVector3<>[8];

		Assert(pav3Projected.atArray);
		Assert(pav3Actual.atArray);

		// Get the bounding box vertices.
		ppartShape->bGetBoundingBoxVertices(pav3Actual.atArray);

		// Transform to world space coordinates.
		uint u;
		for (u = 0; u < pav3Actual.uLen; ++u)
			pav3Actual[u] *= ppartShape->pr3Presence();

		// Create a clipping plane.
		CVector3<> v3_plane = ppartShape->v3Pos() - v3_cam;
	#ifdef __MWERKS__
		CDir3<> tempDir3(v3_plane);
		CPlaneT<CPlaneDef> pl(CPlaneDef(tempDir3, 0.0f));
	#else
		CPlaneT<CPlaneDef> pl(CPlaneDef(CDir3<>(v3_plane), 0.0f));
	#endif
		pl.rD = -pl.rDistance(ppartShape->v3Pos());

		// Generate all the projected points.
		for (u = 0; u < pav3Actual.uLen; ++u)
			pav3Projected[u] = pl.v3Intersection(pav3Actual[u], v3_cam);
	}

	//*****************************************************************************************
	void CRenderCache::SetControlPoints(const CCamera& cam_new)
	{
		// In the current scheme, consider the origin of the object as the control point.
		pmshPolygon->v3ControlActual = CVector3<>(0.0f, 0.0f, 0.0f);

		// Get the near clipping distance of the new camera.
		float f_near_clip = cam_new.campropGetProperties().rNearClipPlaneDist;

		//
		// Get the position of the actual control point in camera space.
		//

		// Get the shape to camera transform.
		CPresence3<> pr3_shape_cam = ppartShape->pr3Presence() * ~cam_new.pr3Presence();

		// Transform the control point.
		CVector3<> v3_control_near_cam = pmshPolygon->v3ControlActual * pr3_shape_cam;

		// Get the scale to move the control point's y value to the near clipping plane.
		float f_scale = f_near_clip / v3_control_near_cam.tY;

		// Move the control point in camera space.
		v3_control_near_cam *= f_scale;

		//
		// Get the position of the control point in world space.
		//

		// Get the camera to shape transform.
		CPresence3<> pr3_cam_shape = cam_new.pr3Presence() * ~ppartShape->pr3Presence();

		// Transform the control point.
		pmshPolygon->v3ControlNear = v3_control_near_cam * pr3_cam_shape;
	}

	//*****************************************************************************************
	void CRenderCache::SetTextureMemSize()
	{
		SetTextureMemSize(fxhHeap.iGetSizeKB(), 0);
	}

	//*****************************************************************************************
	void CRenderCache::SetTextureMemSize(int i_sys_mem_kb, int i_vid_mem_kb)
	{
	#if bTRACK_D3D_RASTERS
		TrackSystem(etrCaches);
	#endif
		int i_vid_mem_kb_alloc = 0;
		if (i_sys_mem_kb < 256)
			i_sys_mem_kb = 256;

		int i_max_hw_dim = 256;

		if (d3dDriver.bUseD3D())
			i_max_hw_dim = Max(64, d3dDriver.iGetRecommendedMaxDim());

		//
		// Allocate memory from the page manager.
		//
		ptexmCacheTextures->FreePages();
		if (i_vid_mem_kb > 0)
		{
			if (d3dDriver.bIsPageManaged())
			{
				ptexmCacheTextures->AllocPagesD3D(i_vid_mem_kb * 1024, 0, 1, i_max_hw_dim, ed3dtexSCREEN_TRANSPARENT);
				i_vid_mem_kb_alloc = ptexmCacheTextures->iGetManagedMemSize() >> 10;
			}
			else
			{
				i_vid_mem_kb_alloc = i_vid_mem_kb;
			}
		}

		// Reconcile allocations.
		i_sys_mem_kb += i_vid_mem_kb - i_vid_mem_kb_alloc;
		i_vid_mem_kb  = i_vid_mem_kb_alloc;

		// Establish memory limits.
		iLimitBytes    = i_sys_mem_kb * 1024;
		iLimitBytesD3D = i_vid_mem_kb * 1024;

		// Reserve five percent.
		iLimitBytesD3D = (iLimitBytesD3D * 95) / 100;

		if (i_vid_mem_kb)
		{
			CRenderCache::CCacheResolution::iCacheMaxDim = i_max_hw_dim;
		}
		else
		{
			CRenderCache::CCacheResolution::iCacheMaxDim = 256;
		}

		SetParameters();
	}

	//*****************************************************************************************
	int CRenderCache::iGetHardwareLimit()
	{
		return ptexmCacheTextures->iGetManagedMemSize();
	}

	//*****************************************************************************************
	void CRenderCache::SetParameters()
	{
		static const int   iIdealCacheBytes = 8800 * 1024;
		static const int   iIdealScreenArea = 512 * 384;
		static const float fMinGeneralScale = 0.45f;
		float fMaxGeneralScale = 0.85f;

		// Make the resolution for the lowest screen resolution higher.
		if (prasMainScreen->iWidth < 512)
			fMaxGeneralScale = 0.90f;
		if (prasMainScreen->iWidth < 400)
			fMaxGeneralScale = 0.95f;

		// Simulate a get of the global speed and quality setting.
		int i_speed_look = iGetQualitySetting();
		AlwaysAssert(i_speed_look >= 0 && i_speed_look <= 4);

		// Get the total amount of memory available for hardware.
		int i_mem_bytes = Max(iLimitBytes, iLimitBytesD3D);

		// Scale to available memory.
		float f_memory_ratio = float(i_mem_bytes) / float(iIdealCacheBytes);
		float f_area_ratio   = float(iIdealScreenArea) / float(prasMainScreen->iWidth * prasMainScreen->iHeight);
		fGeneralScale = f_memory_ratio * f_area_ratio;

		// Hack for Direct3D.
		if (d3dDriver.bUseD3D() && d3dDriver.iGetRecommendedMaxDim() < 256 && iLimitBytesD3D > 4096 * 1024)
			fGeneralScale = fMaxGeneralScale;

		// Some minimum defaults.
		iSmallestD3DAllocation = 64;

		float f_quality_scale = qdQualitySettings[i_speed_look].fResolutionScale;

		fGeneralScale = Min(fGeneralScale, f_quality_scale);

		// Set minimum D3D allocation.
		{
			float f_scale_area = fGeneralScale / fMaxGeneralScale;
			f_scale_area *= f_scale_area;
			iSmallestD3DAllocation = int(f_scale_area * 256.0f + 0.5f);
		}

		// Set hard boundaries for cache parameters.
		if (i_speed_look < 4)
			SetMinMax(fGeneralScale, fMinGeneralScale, fMaxGeneralScale);
		SetMinMax(iSmallestD3DAllocation, 64, 512);

		// Some more cache parameters.
		switch (i_speed_look)
		{
			case 0:
				rcsRenderCacheSettings.iMaxObjectsPerCache = 10;
				rcsRenderCacheSettings.SetMaxAngleCylinder(15);
				break;
			case 1:
				rcsRenderCacheSettings.iMaxObjectsPerCache = 15;
				rcsRenderCacheSettings.SetMaxAngleCylinder(20);
				break;
			case 2:
				rcsRenderCacheSettings.iMaxObjectsPerCache = 20;
				rcsRenderCacheSettings.SetMaxAngleCylinder(20);
				break;
			default:
			case 3:
				rcsRenderCacheSettings.iMaxObjectsPerCache = 20;
				rcsRenderCacheSettings.SetMaxAngleCylinder(20);
				break;
			case 4:
				rcsRenderCacheSettings.iMaxObjectsPerCache = 20;
				rcsRenderCacheSettings.SetMaxAngleCylinder(20);
				break;
		}
	}


//*********************************************************************************************
//
// CScheduleCache implementation.
//

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CScheduleCache::CScheduleCache(CScheduler& sch, CPartition* ppart, CRenderContext& renc,
		                           const CPArray<COcclude*>& papoc_intersect)
		: CSchedulerItem(), ppartPart(ppart), rencRenc(renc)
	{
		Assert(ppart);
		Assert(ppart->prencGet());
		CRenderCache* prenc = ppartPart->prencGet();

		// Prevent euthanasia.
		prenc->iEuthansiaAge = 0;

		// Create a new array.
		//papocIntersect.uLen    = papoc_intersect.uLen;
		//papocIntersect.atArray = new (sch) COcclude*[papoc_intersect.uLen];
		papocIntersect.uLen    = 0;
		papocIntersect.atArray = 0;

		// Copy the array.
		for (uint u = 0; u < papocIntersect.uLen; ++u)
			papocIntersect[u] = papoc_intersect[u];

		//
		float f_priority = ppart->prencGet()->fPriority(rencRenc.Camera, rencRenc.rLightList);
		SetPriority(f_priority);

		// Add this element to the scheduler.
		sch.AddExecution(this);
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	void CScheduleCache::Execute()
	{
		ppartPart->prencGet()->UpdateRenderContext(rencRenc.Camera, rencRenc.rLightList);
	}

	//*****************************************************************************************
	void CScheduleCache::PostScheduleExecute()
	{
		Assert(ppartPart);
		Assert(ppartPart->prencGet());

		// Get the render cache.
		CRenderCache* prenc = ppartPart->prencGet();
		AlwaysAssert(prenc->iEuthansiaAge == 0);

		// Render the cache to the screen if it is visible.
		if (prenc->bRender())
		{
			CCycleTimer ctmr;
			const CPresence3<> pr3 = ppartPart->pr3Presence();

			Assert(prenc->pshGetShape());

			CShapePresence rsp(pr3, rencRenc.Camera.pr3GetPresence());

			prenc->pshGetShape()->Render
			(
				ptCast<CInstance>(ppartPart),
				rencRenc,
				rsp,
				pr3 * rencRenc.tf3ToNormalisedCamera,
				papocIntersect,
				esfINTERSECT
			);

#if (VER_TIMING_STATS)
			TCycles cy_rshape = ctmr();
			CScheduler::cyAccountedScheduleCycles += cy_rshape;
			proProfile.psRenderShape.Add(cy_rshape);
#endif
		}
	}

	//*****************************************************************************************
	void CScheduleCache::LogItem(CConsoleBuffer& con) const
	{
		CSchedulerItem::LogItem(con);
		con.Print("Cache\t%1.3f\t%1.3f", ppartPart->prencGet()->fAngularError,
			      ppartPart->prencGet()->fTexelToPixel);
	}


//*********************************************************************************************
//
// CRenderCacheList implementation.
//

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	//*****************************************************************************************
	CRenderCacheList::CRenderCacheList()
	{
		bListActive = true;
	}

	//*****************************************************************************************
	CRenderCacheList::~CRenderCacheList()
	{
		bListActive = false;
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	void CRenderCacheList::IncrementCount()
	{
		TCacheSet::iterator it_partlist = partlistCached.begin();	// Iterator.

		for (; it_partlist != partlistCached.end(); it_partlist++)
		{
			CPartition* ppart = *it_partlist;	// To do: use iterator directly, use this only
												// for debugging.
			CRenderCache* prenc = ppart->prencGet();
			Assert(ppart);


			if (prenc)
			{
				// Increment cache age.
				prenc->iEuthansiaAge++;
			}
		}
	}

	//*****************************************************************************************
	void CRenderCacheList::AddToLRU()
	{
		// Set up the LRU container for terrain textures and image caches.
		lruTerrainCache.Reset();

		TCacheSet::iterator it_partlist = partlistCached.begin();	// Iterator.

		for (; it_partlist != partlistCached.end(); it_partlist++)
		{
			CPartition* ppart = *it_partlist;	// To do: use iterator directly, use this only
												// for debugging.
			CRenderCache* prenc = ppart->prencGet();
			Assert(ppart);

			if (prenc)
				if (prenc->iEuthansiaAge > 4)
				{
					lruTerrainCache.AddItem(new (lruTerrainCache) CRenderCacheLRU(prenc));
				}
		}
	}

	//*****************************************************************************************
	void CRenderCacheList::operator +=(CPartition* ppart)
	{
		partlistCached.insert(ppart);
	}

	//*****************************************************************************************
	void CRenderCacheList::operator -=(CPartition* ppart)
	{
		partlistCached.erase(ppart);
	}

	//*****************************************************************************************
	int CRenderCacheList::iGetCacheMemoryUsed()
	{
		//
		// Iterate through the list of partitions with caches.
		//
		int i_mem = 0;
		TCacheSet::iterator it_partlist = partlistCached.begin();	// Iterator.

		for (; it_partlist != partlistCached.end(); it_partlist++)
		{
			CPartition* ppart = *it_partlist;	// To do: use iterator directly, use this only
												// for debugging.
			CRenderCache* prenc = ppart->prencGet();

			if (prenc)
				i_mem += prenc->iGetMemUsed();
		}
		return i_mem;
	}

	//*****************************************************************************************
	int CRenderCacheList::iGetNumCaches()
	{
		return partlistCached.size();
	}

	//*****************************************************************************************
	void CRenderCacheList::RemoveAll()
	{
		if (rcsRenderCacheSettings.bFreezeCaches)
			return;
		list<CPartition*> list_partremove;	// List of partitions to remove.

		//
		// Iterate through the list of partitions with caches.
		//
		{
			TCacheSet::iterator it_partset = partlistCached.begin();	// Iterator.

			for (; it_partset != partlistCached.end(); it_partset++)
			{
				CPartition* ppart = *it_partset;	// To do: use iterator directly, use this only
													// for debugging.
				CRenderCache* prenc = ppart->prencGet();
				Assert(ppart);

				list_partremove.push_back(ppart);
			}
		}

		// Euthanize old caches.
		{
			list<CPartition*>::iterator it_partlist = list_partremove.begin();
			for (; it_partlist != list_partremove.end(); it_partlist++)
				(*it_partlist)->DeleteRenderCache();
		}
	}

	//*****************************************************************************************
	void CRenderCacheList::Dump()
	{
		// Open the text file.
		CConsoleBuffer con_out(120, 25);
		con_out.OpenFileSession("CacheList.txt");

		//
		// Iterate through the list of partitions with caches.
		//
		TCacheSet::iterator it_partlist = partlistCached.begin();	// Iterator.
		for (; it_partlist != partlistCached.end(); it_partlist++)
		{
			CPartition* ppart = *it_partlist;
			CRenderCache* prenc = ppart->prencGet();
			AlwaysAssert(prenc);

			if (prenc->iD3DBytes == 0 && prenc->iSysBytes == 0)
				continue;

			// Print out data.
			{
				rptr<CRaster> pras = static_cast<CRenderCache::CPriv&>(*prenc).prasScreenRegion();
				AlwaysAssert(pras);

				int i_width  = pras->iWidth;
				int i_height = pras->iHeight;
				int i_mem    = pras->iWidth * pras->iHeight * 2;

				con_out.Print("%ld\t %ld: \t%ld ", i_width, i_height, i_mem);
				if (prenc->iD3DBytes)
					con_out.Print("D3D\n");
				else
					con_out.Print("Sys\n");
			}
				
		}

		// Write the text file to disk.
		con_out.CloseFileSession();
	}


//
// Global function implementations.
//

//*********************************************************************************************
int iCacheMemoryUsed()
{
	return renclRenderCacheList.iGetCacheMemoryUsed();
}

//*********************************************************************************************
int iNumCaches()
{
	return renclRenderCacheList.iGetNumCaches();
}


//
// Global functions.
//

//*********************************************************************************************
void PurgeRenderCaches()
{
	// Do nothing if the world database has not been instantiated yet.
	if (!pwWorld)
		return;

	// Get the root partition of the world. The 'const' is cast away as a quick hack.
	CPartition* ppart = (CPartition*)pwWorld->ppartPartitionList();
	if (!ppart)
		return;

	// Delete all caches at or below the root partition.
	ppart->DeleteAllCaches();
}

//*********************************************************************************************
void UploadCaches()
{
	// Upload dirty pages.
	ptexmCacheTextures->bUploadPages();
}

//*********************************************************************************************
void PurgeParentCaches(CPartition* ppart)
{
	// Non-recursively walk up the parent list and purge caches.
	while (ppart)
	{
		// Get rid of the cache associated with this partition if there is one.
		if (ppart->prencGet())
		{
			ppart->DeleteRenderCache();
			return;
		}

		// Get the next parent.
		ppart = ppart->ppartGetParent();
	}
}


//
// Global variables.
//

// Render cache id generator.
int  CRenderCache::iNextCacheID;
int  CRenderCache::iFrameKeyGlobal = 2;

int CRenderCache::iTotalD3DBytes = 0;		// Initialize to zero.
int CRenderCache::iTotalSysBytes = 0;		// Initialize to zero.

int CRenderCache::iLimitBytes    = 6144 * 1024;	// Six megs for now.
int CRenderCache::iLimitBytesD3D = 0;

int   CRenderCache::iSmallestD3DAllocation = 512;
float CRenderCache::fGeneralScale          = 0.925f;


//
// Global variables.
//
rptr<CTexturePageManager> ptexmCacheTextures = rptr_new CTexturePageManager();
CRenderCacheList    renclRenderCacheList;
int CRenderCache::CCacheResolution::iCacheMaxDim = 256;
