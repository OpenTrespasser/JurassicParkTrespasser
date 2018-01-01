/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Implementation of RenderCacheHelp.hpp.
 *
 * To do: 
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/RenderCacheHelp.cpp                                      $
 * 
 * 95    10/01/98 9:47p Pkeet
 * Changed the render cache list to an STL set.
 * 
 * 94    9/25/98 1:50a Pkeet
 * Set up caching for biomeshes.
 * 
 * 93    8/31/98 10:04p Pkeet
 * Changed the hardware defaults.
 * 
 * 92    8/29/98 7:25p Pkeet
 * Increased cache quality parameters for hardware.
 * 
 * 91    8/19/98 1:34p Rwyatt
 * Opps.
 * 
 * 90    8/19/98 1:33p Rwyatt
 * VC6.0 Warning fixes
 * 
 * 89    8/19/98 11:42a Pkeet
 * Fixed bug.
 * 
 * 88    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 87    7/06/98 6:52p Pkeet
 * Added code to maintain a 1:1 aspect ratio in hardware for render caches.
 * 
 * 86    6/21/98 2:43p Pkeet
 * Added the 'SetVisible' member function call to render caches and moved the point where the
 * key frame for render caches was updated.
 * 
 * 85    6/04/98 7:43p Pkeet
 * Added a parallax shear distance threshold.
 * 
 * 84    6/04/98 6:50p Pkeet
 * Added in the cache multiplier value.
 * 
 * 83    6/03/98 8:18p Pkeet
 * Added a parallax shear test for determing if a partition should be cached.
 * 
 * 82    5/26/98 11:28p Pkeet
 * Increased the minimum distance constant.
 * 
 * 81    5/26/98 2:39p Pkeet
 * Set minimum cache distance outward.
 * 
 * 80    5/20/98 7:39p Pkeet
 * Fixed crash bug.
 * 
 * 79    5/14/98 7:32p Pkeet
 * Caches get points from a partition only if they are visible and of an appropriate priority.
 * 
 * 78    5/12/98 6:10p Pkeet
 * Added code to freeze caches.
 * 
 * 77    5/10/98 8:16p Pkeet
 * Changed the acceptance sphere to be more permissive.
 * 
 * 76    5/10/98 1:58p Pkeet
 * Added area efficiency code. Added numerous asserts to debug broken caches.
 * 
 * 75    5/05/98 2:40p Pkeet
 * Added a macro switch to force the minimum distance for caching in test scene to always be 10
 * metres.
 * 
 * 74    5/05/98 1:34p Pkeet
 * Altered includes to include the render cache private header file.
 * 
 * 73    4/24/98 3:42p Pkeet
 * Changed calls to the 'fGetDistanceSqr' member function of 'CPartition' to
 * 'fDistanceFromGlobalCameraSqr.'
 * 
 * 72    4/22/98 4:53p Pkeet
 * Replaced slower functions to get the world radius of a partition's bounding sphere and its
 * bounding radius with more efficient functions.
 * 
 * 71    4/21/98 8:17p Pkeet
 * Removed a call to 'InitializeForCaching.'
 * 
 * 70    4/16/98 3:32p Pkeet
 * Put some of the functionality of 'bShouldCache' into 'bCanCache.'
 * 
 **********************************************************************************************/


//
// Includes.
//
#include <math.h>
#include "Common.hpp"
#include "RenderCacheHelp.hpp"

#include "RenderCachePriv.hpp"
#include "RenderCacheInterface.hpp"

#include "Lib/Math/FloatDef.hpp"
#include "Lib/Transform/VectorRange.hpp"
#include "Lib/GeomDBase/Partition.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"
#include "Lib/Math/FastInverse.hpp"
#include "Lib/Renderer/LineSide2D.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/Scheduler.hpp"
#include "Lib/GeomDBase/PartitionSpace.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/W95/Direct3D.hpp"


//
// Macros and constants.
//

const float fMinDistanceToCache = 15.0f;

// One.
const float fOne = 1.0f;

// Distance after which the shear test will not be conducted.
const float fNoShearTestDistance = 100.0f;

// Scene file version number.
const int iCacheSceneVersion = 2;

// Distance Anne will walk in two frames.
const float fWalkDistance2 = 2.5f;

// Storage for points.
CVector3<> av3AnimalPoints[8];


//
// Module specific variables.
//
SRenderCacheSettings rcsRenderCacheSettings;
CRenderCacheStats    rcstCacheStats;


//*********************************************************************************************
void AddStats(CPartition* ppart);

//
// Global functions.
//

//*********************************************************************************************
bool bShouldCache(CPartition* ppart, const CCamera& cam, float f_distance_sqr,
				  CShapePresence& rsp, bool b_visible)
{
	Assert(ppart);
	Assert(f_distance_sqr >= 0.0f);

	float f_radius_cyl_sqr;
	float f_cache_multiplier;

	// Always cache if the caching system is frozen and there is a cache already.
	if (rcsRenderCacheSettings.bFreezeCaches)
	{
		if (ppart->prencGet())
		{
			return true;
		}
	}

	//
	// Do not cache the partition if it contains a moving physics object or if it is
	// uncacheable for some other reason.
	//
	if (!ppart->bCanCache())
		goto FAIL_CACHE;

	// Fail if the partition is too close.
	{
		float f_cache_mul = (d3dDriver.bUseD3D()) ? (4.0f) : (2.5f);
		float f_radius_sph_sqr3 = f_cache_mul * ppart->rGetSphereRadiusSqr();
		if (u4FromFloat(f_radius_sph_sqr3) > u4FromFloat(f_distance_sqr))
			goto FAIL_CACHE;
	}

	// Get the cache multiplier.
	f_cache_multiplier = ppart->fGetCacheMultiplier();

	// Fail if the partition is too big on the screen using the radius of the cylinder.
	f_radius_cyl_sqr = ppart->rGetCylinderRadiusSqr();
	{
		float f_radius_cyl_rel = f_radius_cyl_sqr * rcsRenderCacheSettings.fMaxAcceptCylinder * f_cache_multiplier;
		if (u4FromFloat(f_radius_cyl_rel) > u4FromFloat(f_distance_sqr))
			goto FAIL_CACHE;
	}

	// Always accept if the partition is small enough on screen.
	if (!ppart->bCacheGroup(f_distance_sqr))
			goto FAIL_CACHE;

	// More expensive test.
	{
		Assert(f_distance_sqr > 0.0f);
		float f_dist     = fSqrtEst(f_distance_sqr) + ppart->rGetCylinderRadius();
		float f_min_dist = fMinDistanceToCache * f_cache_multiplier;

		// Is the cache at or beyond the minimum distance.
		if (u4FromFloat(f_dist) < u4FromFloat(f_min_dist))
		{
			goto FAIL_CACHE;
		}
	}

	// Final hardcore test.
	{
		float f_near;				// Distance to the nearest point of the partition.
		float f_far;				// Distance to the farthest point of the partition.
		float f_shear_threshold;	// Threshold above which the partition will cache.

		ppart->GetMinMaxDistance(f_near, f_far, rsp);

		Assert(f_near <= f_far);
		Assert(f_near > 0.0f);

		if (u4FromFloat(f_near) > u4FromFloat(fNoShearTestDistance))
			goto ADD_CACHE;

		f_near -= fWalkDistance2;
		if (u4FromFloat(f_near) & 0x80000000)
			goto FAIL_CACHE;
		f_far  -= fWalkDistance2;
		
		f_shear_threshold = rcsRenderCacheSettings.fParallaxShear;
		if (u4FromFloat(f_cache_multiplier) == u4FromFloat(fOne))
		{	
		}
		else
		{
			if (u4FromFloat(f_cache_multiplier) > u4FromFloat(fOne))
			{
				f_shear_threshold = 1.0f - (1.0f - rcsRenderCacheSettings.fParallaxShear) / f_cache_multiplier;
			}
			else
			{
				f_shear_threshold *= f_cache_multiplier;
			}
		}
		Assert(f_shear_threshold >= 0.0f);
		Assert(f_shear_threshold <= 1.0f);

		f_shear_threshold *= f_far;
		if (u4FromFloat(f_near) > u4FromFloat(f_shear_threshold))
			goto ADD_CACHE;
	}

FAIL_CACHE:

	//
	// Test failed. Make sure the partition doesn't have a cache.
	//
	ppart->DeleteRenderCache();
	return false;

ADD_CACHE:

	// Make sure that this partition does not contain a moving object.
	//if (!ppart->bContainsMovingObject())

	//
	// If there is a cache associated with this partition already, then the
	// partition is cacheable.
	//
	if (ppart->prencGet())
	{
		// Set the frame key.
		AddStats(ppart);
		return true;
	}

	// Make sure no children are using a cache.
	ppart->DeleteAllChildCaches();

	//
	// If a cache can be created for this partition, then it is cacheable.
	//

	// Make sure a cache exist for this partition.
	ppart->CreateRenderCache(cam);

	// Set the frame key.
	AddStats(ppart);
	return true;
}


//
// Class implementation.
//

//*********************************************************************************************
//
// CClosestRenderPoint implementation.
//

	//*****************************************************************************************
	//
	// Constructor.
	//

	// Default constructor.
	CClosestRenderPoint::CClosestRenderPoint()
		: bFoundFirst(false)
	{
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	void CClosestRenderPoint::Try(const CVector3<>& v3_mesh, TReal r_min_projected_z)
	{
		// Clamp the y value.
		if (r_min_projected_z < 0.0f)
			r_min_projected_z = 0.0f;

		// If this is the first point found, just store it.
		if (!bFoundFirst)
		{
			v3ClosestMeshPt = v3_mesh;
			rMinProjectedZ  = r_min_projected_z;
			bFoundFirst     = true;
			return;
		}

		// Compare the Y values and store the point if it is the closest.
		if (r_min_projected_z < rMinProjectedZ)
		{
			v3ClosestMeshPt = v3_mesh;
			rMinProjectedZ  = r_min_projected_z;
		}
	}


//
// Global function bodies.
//

//*********************************************************************************************
void AddStats(CPartition* ppart)
{
	// Maintain statistics if required.
	if (!rcstCacheStats.bKeepStats)
		return;

	// Count number of objects cached.
	rcstCacheStats.iNumCachedObjects += ppart->iCountInstances();

	// Add the age of the cache to the sum of the ages.
	rcstCacheStats.iTotalCacheAge += ppart->prencGet()->iGetCacheAge();
}

//*********************************************************************************************
uint uBuildPartitionList(TPPartitionList& rpartlist, CPartition* ppart, const CVector3<>& v3_pos)
{
	Assert(ppart);

	uint u_num_points = 0;

	//
	// Continue only if the partition is viewable.
	//
	float f_distance_sqr = ppart->fDistanceFromGlobalCameraSqr();
	if (!ppart->bInRange(f_distance_sqr))
		return u_num_points;

	//
	// If the partition is an instance with a mesh, add the partition to the list.
	//
	{
		CInstance*  pins;	// The pointer cast as an instance.
		ppart->Cast(&pins);
		if (pins)
		{
			// Get the shape from the instance.
			rptr<CMesh> pmsh = ptCastRenderType<CMesh>(pins->prdtGetRenderInfo());
			if (pmsh)
			{
				rpartlist.push_back(ppart);
				u_num_points = pmsh->pav3GetWrap().uLen;
				if (!u_num_points)
				{
					CAnimal* pani = 0;
					ppart->Cast(&pani);
					if (pani)
						u_num_points = 8;
				}
			}
		}
	}

	//
	// Recurse through children.
	//

	// Get a pointer to the partition's child list.
	CPartition* ppartc = ppart->ppartChildren();

	// Return if there are no children.
	if (ppartc)
	{
		// Iterate through the child list to perform this function.
		for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			u_num_points += uBuildPartitionList(rpartlist, *it, v3_pos);
	}
	return u_num_points;
}

//*********************************************************************************************
void GetBoundingCoords(CPartition* ppart, const CTransform3<>& t3f_cam, TReal r_distance,
					   CDArray< CVector2<> >& rav2_convex, CVector3<>& rv3_min,
					   CVector3<>& rv3_max, CClosestRenderPoint& rcrp, bool b_use_box,
					   bool b_add_points)
{
	Assert(ppart);

	// Check the visible flag.
	if (!ppart->bIsVisible())
		return;

	// Check if the partition priority suggests continuing.
	if (!ppart->bIsWithinPriority())
		return;

	CInstance* pins;	// The pointer cast as an instance.

	// Cast the pointer to an instance.
	ppart->Cast(&pins);

	//
	// If the partition is an instance with a mesh, iterate through the mesh vertices and
	// extract the minimum and maximum points in camera space.
	//
	if (pins)
	{
		// Get the shape from the instance.
		rptr<CMesh> pmsh = ptCastRenderType<CMesh>(pins->prdtGetRenderInfo());

		// Get the minimun and maximum points.
		if (pmsh)
		{
			// Get the shape to world transform.
			CTransform3<> t3f_shape_cam = ppart->pr3Presence() * t3f_cam;

			// Get the shape's wrap in camera space coordinates.
			CPArray< CVector3<> > pav3_mesh = pmsh->pav3GetWrap();

			CAnimal* pani = 0;	// Animal cast.

			// If no array is found, assume that the mesh is for an animal.
			if (!pav3_mesh.atArray)
				ppart->Cast(&pani);

			// If there is no point cloud, assume the instance to be an animal.
			if (pani)
			{				
				CVector3<> v3_min_animal;
				CVector3<> v3_max_animal;
				CTransform3<> t3f_shape;

				// Use global storage temporarily.
				pav3_mesh.atArray = av3AnimalPoints;
				pav3_mesh.uLen    = 8;

				// Synthesize a point cloud.
				pmsh->GetExtents(pins, t3f_shape, v3_min_animal, v3_max_animal);
				pav3_mesh[0] = v3_min_animal;
				pav3_mesh[1] = CVector3<>(v3_min_animal.tX, v3_min_animal.tY, v3_max_animal.tZ);
				pav3_mesh[2] = CVector3<>(v3_min_animal.tX, v3_max_animal.tY, v3_min_animal.tZ);
				pav3_mesh[3] = CVector3<>(v3_min_animal.tX, v3_max_animal.tY, v3_max_animal.tZ);
				pav3_mesh[4] = CVector3<>(v3_max_animal.tX, v3_min_animal.tY, v3_min_animal.tZ);
				pav3_mesh[5] = CVector3<>(v3_max_animal.tX, v3_min_animal.tY, v3_max_animal.tZ);
				pav3_mesh[6] = CVector3<>(v3_max_animal.tX, v3_max_animal.tY, v3_min_animal.tZ);
				pav3_mesh[7] = v3_max_animal;
			}

			uint u_num_new_points = pav3_mesh.uLen;	// Number of points to add.

			// Limit the number of new points that can be added.
			if (b_use_box)
			{
				u_num_new_points = Min(8, pav3_mesh.uLen);
			}
			
			// Create intermediate storage for the new points.
			CLArray(CVector3<>, pav3, u_num_new_points);

			if (b_use_box && pav3.uLen >= 8)
			{
				ppart->bGetBoundingBoxVertices(pav3);
				for (uint u = 0; u < pav3.uLen; ++u)
				{
					pav3[u] = pav3[u] * t3f_shape_cam;
					Assert(_finite(pav3[u].tX));
					Assert(_finite(pav3[u].tY));
					Assert(_finite(pav3[u].tZ));
				}
			}
			else
			{
				// Iterate through the shape's points to transform and copy the point.
				for (uint u = 0; u < pav3.uLen; ++u)
				{
					pav3[u] = pav3_mesh[u] * t3f_shape_cam;
					Assert(_finite(pav3[u].tX));
					Assert(_finite(pav3[u].tY));
					Assert(_finite(pav3[u].tZ));
				}
			}

			// Iterate through the shape's points to produce a list of convex points.
			for (uint u = 0; u < pav3.uLen; ++u)
			{
				// Copy the point.
				CVector3<> v3 = pav3[u];

				Assert(_finite(v3.tX));
				Assert(_finite(v3.tY));
				Assert(_finite(v3.tZ));

				// Get the projection value.
				float f_inverse_z = fInverse(v3.tY);
				float f_project   = r_distance * f_inverse_z;

				// Project the point into camera space.
				CVector2<> v2(v3.tX * f_project, v3.tZ * f_project);

				Assert(_finite(v2.tX));
				Assert(_finite(v2.tY));

				// Add the point to the list.
				if (b_add_points)
					rav2_convex.daAdd(v2);

				//
				// Store the point if it is the closest one yet to the camera, or the first
				// point found.
				//
				if (u < pav3_mesh.uLen)
					rcrp.Try(pav3_mesh[u], v2.tY);

				// Get the minimum and maximum x, y and z positions.
				rv3_min.tX = Min(rv3_min.tX, v2.tX);
				rv3_max.tX = Max(rv3_max.tX, v2.tX);
				rv3_min.tY = Min(rv3_min.tY, v2.tY);
				rv3_max.tY = Max(rv3_max.tY, v2.tY);
				rv3_min.tZ = Min(rv3_min.tZ, v3.tY);
				rv3_max.tZ = Max(rv3_max.tZ, v3.tY);
			}
		}
	}
}

//*********************************************************************************************
void FudgeRange(CVector3<>& rv3_min, CVector3<>& rv3_max, TReal r_expand)
{
	rv3_min.tX -= r_expand;
	rv3_min.tY -= r_expand;
	rv3_max.tX += r_expand;
	rv3_max.tY += r_expand;
}

//*********************************************************************************************
void SetCameraProperties(CCamera& cam_new, rptr<CRaster> pras, TReal r_distance,
						 CVector3<>&   rv3_coord_min, CVector3<>& rv3_coord_max)
{
	CVector2<> v2_coord_max;
	float f_dx = rv3_coord_max.tX - rv3_coord_min.tX;
	float f_dy = rv3_coord_max.tY - rv3_coord_min.tY;

	// Get absolute max coordinates.
	v2_coord_max.tX = Max(Abs(rv3_coord_max.tX), Abs(rv3_coord_min.tX));
	v2_coord_max.tY = Max(Abs(rv3_coord_max.tY), Abs(rv3_coord_min.tY));

	// Get the camera's existing properties.
	CCamera::SProperties camprop = cam_new.campropGetProperties();

	//
	// Setup the main viewport object.
	//

	// Set the screen size (for scaling during projection) to the raster size.
	camprop.vpViewport.SetSize(pras->iWidth, pras->iHeight);

#if bDEBUG_PRINT_CACHE_VALUES
	dprintf("Cache camera: %ld, %ld\n", pras->iWidth, pras->iHeight);
#endif // bDEBUG_PRINT_CACHE_VALUES

	camprop.rViewWidth = v2_coord_max.tX / r_distance;

	// Set the physical aspect ratio.
	camprop.fAspectRatio = v2_coord_max.tX / v2_coord_max.tY;

	//
	// Set the near and far clipping planes for the object.
	//
	
	// Distance of the near clipping plane from the camera.
	camprop.rNearClipPlaneDist = rv3_coord_min.tZ;
	if (camprop.rNearClipPlaneDist < 0.001f)
		camprop.rNearClipPlaneDist = 0.001f;
	
	// Distance of the far clipping plane from the camera. 
	camprop.rFarClipPlaneDist  = rv3_coord_max.tZ;

	// Set the properties of the new camera.
	cam_new.SetProperties(camprop);
}

//*********************************************************************************************
CVector3<> v3GetCameraPos(const CCamera& cam)
{
	// Use a predicted position if the settings allow for it.
	return cam.pr3VPresence(rcsRenderCacheSettings.bUseCameraPrediction).v3Pos;
}


//*********************************************************************************************
//
// SRenderCacheSettings implementation.
//

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	void SRenderCacheSettings::SetMaxAngleCylinder(int i_degrees)
	{
		Assert(i_degrees > 0);
		Assert(i_degrees < 90);

		double d_radians   = double(i_degrees) * 3.14159 / 180.0;
		float  f_tan_alpha = float(1.0 / tan(d_radians));
		fMaxAcceptCylinder = f_tan_alpha * f_tan_alpha + 1.0f;
		iMaxAcceptAngleCylinder = i_degrees;
	}

	//*****************************************************************************************
	//
	char* SRenderCacheSettings::pcSave
	(
		char* pc
	) const
	//
	// Saves the render cache settings to a buffer.
	//
	//**************************************
	{
		// Save version number.
		pc = pcSaveT(pc, iCacheSceneVersion);

		pc = pcSaveT(pc, iMaxAcceptAngleCylinder);
		if (prenMain && prenMain->pSettings)
			pc = pcSaveT(pc, prenMain->pSettings->fDetailReduceFactor);
		else
			pc = pcSaveT(pc, 1.0f);
		pc = pcSaveT(pc, rPixelsPerArea);
		pc = pcSaveT(pc, rPixelsPerLine);
		pc = pcSaveT(pc, rMinPolygonMesh);
		pc = pcSaveT(pc, bUseCameraPrediction);
		pc = pcSaveT(pc, bFasterPhysics);
		pc = pcSaveT(pc, iMaxObjectsPerCache);
		pc = pcSaveT(pc, fEfficiencyArea);
		pc = pcSaveT(pc, fDistortionWeight);

		return pc;
	}

	//*****************************************************************************************
	//
	const char* SRenderCacheSettings::pcLoad
	(
		const char* pc
	)
	//
	// Loads the render cache settings.
	//
	//**************************************
	{
		int iVersion;
		float f_dummy;
		int i_dummy;
		bool b_dummy;

		pc = pcLoadT(pc, &iVersion);

		switch (iVersion)
		{
			case 1:
				pc = pcLoadT(pc, &f_dummy);
				pc = pcLoadT(pc, &iMaxAcceptAngleCylinder);
				pc = pcLoadT(pc, &f_dummy);
				pc = pcLoadT(pc, &i_dummy);
				pc = pcLoadT(pc, &prenMain->pSettings->fDetailReduceFactor);
				pc = pcLoadT(pc, &rPixelsPerArea);
				pc = pcLoadT(pc, &rPixelsPerLine);
				pc = pcLoadT(pc, &f_dummy);
				pc = pcLoadT(pc, &rMinPolygonMesh);
				pc = pcLoadT(pc, &f_dummy);
				pc = pcLoadT(pc, &bUseCameraPrediction);
				pc = pcLoadT(pc, &b_dummy);
				pc = pcLoadT(pc, &b_dummy);
				pc = pcLoadT(pc, &bFasterPhysics);
				pc = pcLoadT(pc, &i_dummy);
				pc = pcLoadT(pc, &f_dummy);
				pc = pcLoadT(pc, &iMaxObjectsPerCache);
				pc = pcLoadT(pc, &fEfficiencyArea);
				pc = pcLoadT(pc, &fDistortionWeight);
				pc = pcLoadT(pc, &f_dummy);
				pc = pcLoadT(pc, &f_dummy);
				pc = pcLoadT(pc, &f_dummy);
			break;

			case 2:
				pc = pcLoadT(pc, &iMaxAcceptAngleCylinder);
				pc = pcLoadT(pc, &prenMain->pSettings->fDetailReduceFactor);
				pc = pcLoadT(pc, &rPixelsPerArea);
				pc = pcLoadT(pc, &rPixelsPerLine);
				pc = pcLoadT(pc, &rMinPolygonMesh);
				pc = pcLoadT(pc, &bUseCameraPrediction);
				pc = pcLoadT(pc, &bFasterPhysics);
				pc = pcLoadT(pc, &iMaxObjectsPerCache);
				pc = pcLoadT(pc, &fEfficiencyArea);
				pc = pcLoadT(pc, &fDistortionWeight);
			break;

			default:
				AlwaysAssert("Unknown version of render cache settings");
		}

		// Reset partitions for render cache use.
		wWorld.InitializePartitions();

		return pc;
	}


//
// Maintain the lists of moving and stopped partitions.
//

//*********************************************************************************************
void AddToMovingList(CPartition* ppart)
{
	ppartlistMoved.push_back(ppart);
}

//*********************************************************************************************
void AddToStoppedList(CPartition* ppart)
{
	ppartlistStopped.push_back(ppart);
}

//*********************************************************************************************
void BeginCacheUpdate()
{
	CPartition::IncrementFrameCount();
}

//*********************************************************************************************
void EndCacheUpdate()
{
	Assert(pwWorld);

	//
	// For now, the simplest thing may be the best. Simply mark the partition and its parents
	// as uncacheable for a frame.
	//
	// To do:
	//		Changed partition parameters to reflect objects added to partition nodes.
	//
	TPPartitionList::iterator it = ppartlistMoved.begin();
	for (; it != ppartlistMoved.end(); ++it)
	{
		Assert(*it);

		if (rcsRenderCacheSettings.bFasterPhysics)
		{
			(*it)->SetCacheNoBuild();
		}
		else
		{
			Assert(pwWorld->ppartPartitionList());
			{
				//
				// Create a query with a slightly expanded bounding volume.
				//
				CBoundVol* pbv = (CBoundVol*)(*it)->pbvBoundingVol();
				Assert(pbv);

				if (pbv->ebvGetType() == ebvBOX)
				{
					CBoundVolBox bvb((static_cast<CBoundVolBox*>(pbv))->v3GetMax() * 1.05f);
					CPartitionSpaceQuery psq((*it)->pr3Presence(), bvb);

					pwWorld->ppartPartitionList()->SetCacheNoBuildPart(*it);
				}
				else
				{
					CPartitionSpaceQuery psq((*it)->pr3Presence(), pbv->fMaxExtent() * 1.05f);

					pwWorld->ppartPartitionList()->SetCacheNoBuildPart(*it);
				}
			}
		}
	}

	// Get rid of lists.
	DumpMoveStopLists();
}

//*********************************************************************************************
void DumpMoveStopLists()
{
	ppartlistMoved.erase(ppartlistMoved.begin(), ppartlistMoved.end());
	ppartlistStopped.erase(ppartlistStopped.begin(), ppartlistStopped.end());
}

//*********************************************************************************************
void CollectCacheGarbage()
{
	if (rcsRenderCacheSettings.bFreezeCaches)
		return;

	// Allocate a local array of partitions with caches to be destroyed.
	CMLArray(CPartition*, appart, renclRenderCacheList.partlistCached.size());

	// Find the unused partitions.
	CRenderCacheList::TCacheSet::iterator it = renclRenderCacheList.partlistCached.begin();
	for (; it != renclRenderCacheList.partlistCached.end(); ++it)
	{
		Assert(*it);
		Assert((*it)->prencGet());

		// If the partition has an unused cache, push it to the list.
		if (!(*it)->prencGet()->bIsCurrentFrameKey())
			appart << (*it);
	}

	// Remove unused partitions.
	for (uint u = 0; u < appart.uLen; ++u)
	{
		Assert(appart[u]);
		appart[u]->DeleteRenderCache();
	}
}

//*********************************************************************************************
void AddUnseenCaches(CScheduler& scheduler, const CCamera& cam, CRenderContext& renc,
					 CPartition* ppart)
{
	Assert(ppart);
	return;
	//if (!rcsRenderCacheSettings.bBuildNonVisible)
		//return;

	// Get the pointer to an existing cache if there is one.
	CRenderCache* prenc = ppart->prencGet();

	// Ignore if the partition is already included for the current frame.
	if (prenc)
	{
		if (prenc->bIsCurrentFrameKey())
			return;
		if (!prenc->bEvaluate())
		{
			prenc->UpdateFrameKey();
			return;
		}
	}

	//
	// Use distance culling to reject the partition if possible.
	//

	// Get the distance square.
	float f_distance_sqr = ppart->fDistanceFromGlobalCameraSqr();

	// Cull if out of range.
	if (!ppart->bInRange(f_distance_sqr))
		return;

	/*
	// Determine if the partition should be cached.
	if (bShouldCache(ppart, cam, f_distance_sqr, false))
	{
		// Create a dummy intersect list.
		CLArray(COcclude*, apoc, 0);

		// Invoke the scheduler, passing along all the pipeline stuff.
		ppart->prencGet()->SetVisible();
		new (scheduler) CScheduleCache
		(
			scheduler,	// Scheduler used for scheduling caches.
			ppart,		// Cached partition.
			renc,		// Render context.
			apoc		// List of intersecting occlusion objects.
		);
		ppart->prencGet()->UpdateFrameKey(false);
		return;
	}
	*/

	// Call this function recursively on the child partitions.
	if (!ppart->prencGet())
	{
		CPartition* ppartc = ppart->ppartChildren();
		if (ppartc)
		{
			// Iterate through the children and call this function recursively.
			for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				AddUnseenCaches(scheduler, cam, renc, *it);
		}
	}
}


//
// Global variables.
//
TPPartitionList ppartlistMoved;
TPPartitionList ppartlistStopped;
