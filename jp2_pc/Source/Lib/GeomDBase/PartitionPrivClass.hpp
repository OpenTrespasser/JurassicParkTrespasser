/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Private classes for use with CPartition.
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/PartitionPrivClass.hpp                                 $
 * 
 * 31    10/01/98 12:35p Pkeet
 * New defaults.
 * 
 * 30    9/28/98 8:09p Pkeet
 * Fixed priority setting bug.
 * 
 * 29    8/29/98 7:25p Pkeet
 * Increased cache quality parameters for hardware.
 * 
 * 28    8/26/98 9:41p Pkeet
 * Made several data members of CPartition short floats.
 * 
 * 27    8/26/98 6:45p Rvande
 * Fixed the "for (uint u_corner..." issue again.  It looks like SourceSafe didn't get the
 * merge right the last time.
 * 
 * 26    8/25/98 10:49p Pkeet
 * Background objects no longer estimate culling distance based on size of mesh, they set their
 * culling distance to the maximum possible distance.
 * 
 * 24    8/18/98 6:08p Pkeet
 * Added hardware flags to the construction member function.
 * 
 * 23    8/17/98 9:41p Agrant
 * Do not always force player shadows off.
 * 
 * 22    7/30/98 4:05p Pkeet
 * Added the 'bHardwareLargeSize' flag.
 * 
 * 21    7/30/98 1:27p Pkeet
 * Enabled animating textures in hardware.
 * 
 * 20    7/21/98 1:00p Pkeet
 * Added the 'bHardwareAble' member function.
 * 
 * 19    98/07/14 20:10 Speter
 * Frozen is gone. Moved to magnet system.
 * 
 * 18    98.07.01 4:32p Mmouni
 * Changed default priority to zero.  Changed clamp to allow zero a value.
 * 
 * 17    6/04/98 1:49p Pkeet
 * Added the cache multiplier value.
 * 
 * 16    6/03/98 8:16p Pkeet
 * Added the 'GetMinMaxDistanceSqr' member function.
 * 
 * 15    6/03/98 2:58p Agrant
 * Frozen flag
 * Save/load selected partition flags
 * 
 * 14    5/21/98 10:39p Pkeet
 * Fixed problem that prevented partitions containing invisble meshes from rendering.
 * 
 * 13    5/14/98 7:33p Pkeet
 * Added code to return values for priorities and cache flags.
 * 
 * 12    5/13/98 8:52p Pkeet
 * Initialization code now marks the water mesh as uncacheable.
 * 
 * 11    5/12/98 5:29p Pkeet
 * Fixed backdrop in cache bug.
 * 
 * 10    5/11/98 9:22p Pkeet
 * Fixed occlusion bug.
 * 
 * 9     5/10/98 8:15p Pkeet
 * Changed the world volume measure for image caching into maximum world area.
 * 
 * 8     5/10/98 1:55p Pkeet
 * Added code for determining volume efficiency for render caching.
 * 
 * 7     5/05/98 5:06p Pkeet
 * Added the 'bAlwaysFace' flag.
 * 
 * 6     5/01/98 7:47p Pkeet
 * Fixed bug in distance culling. Made distance culling work with the radius of the cylinder
 * instead of the radius of the sphere of a partition.
 * 
 * 5     4/27/98 11:03a Pkeet
 * Added shadow culling.
 * 
 * 4     4/24/98 3:38p Pkeet
 * Added additional initial values.
 * 
 * 3     4/23/98 3:19p Pkeet
 * Made the distance culling work correctly.
 * 
 * 2     4/22/98 5:04p Pkeet
 * Added the 'SetSphereRadius,' 'SetCullDistance,' 'fGetCullDistance' and 'fValueSphereRadius'
 * member functions. Initial values are set for the 'bFixedCullDistance' and 'rSphereRadiusSqr'
 * data members. Added code so that if the shadow flag is turned off for a partition, it will
 * remain off despite reinitialization of the partition's data.
 * 
 * 1     4/21/98 8:05p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_PARTITIONPRIVCLASS_HPP
#define HEADER_LIB_GEOMDBASE_PARTITIONPRIVCLASS_HPP

//
// Required includes.
//
#include <math.h>
#include "Lib/EntityDBase/Water.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/Renderer/ShapePresence.hpp"
#include "Lib/W95/Direct3D.hpp"


//
// Macros.
//

#define bCACHE_INVSIBLE_ALPHA (1)

// Compiler switch for verifying the operation of the 'GetMinMaxDistance' member function.
#define bTEST_MINMAX_DISTANCE (0)


//
// Internal class definitions.
//

//******************************************************************************************
//
class CPartition::CPriv : CPartition
//
// Private class of CPartition.
//
//**************************************
{

	//******************************************************************************************
	//
	// Convenience member functions.
	//

	//******************************************************************************************
	//
	CInstance* pinsGet
	(
	)
	//
	// Returns the an instance pointer for the 'this' pointer. Returns a null pointer if 'this'
	// is not an instance.
	//
	//**************************************
	{
		CInstance* pins;
		Cast(&pins);
		return pins;
	}

	//*****************************************************************************************
	//
	void SetCylinderRadius
	(
		TReal r_new_radius	// Value to set the cylinder radius to.
	)
	//
	// Sets the bounding cylinder radius.
	//
	//**************************
	{
		sfCylinderRadiusSqr = r_new_radius * r_new_radius;
	}

	//*****************************************************************************************
	//
	void SetSphereRadius
	(
		TReal r_new_radius	// Value to set the cylinder radius to.
	)
	//
	// Sets the bounding sphere radius.
	//
	//**************************
	{
		sfSphereRadiusSqr = r_new_radius * r_new_radius;
	}

	//*****************************************************************************************
	//
	void SetCullDistance
	(
		float f_cull_distance
	)
	//
	// Sets the cull distance if it is changeable.
	//
	//**************************
	{
		if (!pdData.bFixedCullDistance)
		{
			sfCullDistanceSqr = f_cull_distance * f_cull_distance;
		}
	}

	//*****************************************************************************************
	//
	void SetCullDistanceShadow
	(
		float f_cull_distance
	)
	//
	// Sets the cull distance if it is changeable.
	//
	//**************************
	{
		if (!pdData.bFixedCullShadow)
		{
			sfCullDistanceShadowSqr = f_cull_distance * f_cull_distance;
		}
	}

	//*****************************************************************************************
	//
	float fGetCullDistance
	(
	) const
	//
	// Returns the cull distance.
	//
	//**************************
	{
		return float(sqrt(float(sfCullDistanceSqr)));
	}

	//*****************************************************************************************
	//
	float fGetCullDistance
	(
		const CPartition* ppart
	) const
	//
	// Returns the cull distance from the partition supplied as a parameter.
	//
	//**************************
	{
		return (static_cast<const CPartition::CPriv*>(ppart))->fGetCullDistance();
	}

	//*****************************************************************************************
	//
	float fGetCullDistanceShadow
	(
	) const
	//
	// Returns the cull distance.
	//
	//**************************
	{
		return float(sqrt(float(sfCullDistanceShadowSqr)));
	}

	//*****************************************************************************************
	//
	float fGetCullDistanceShadow
	(
		const CPartition* ppart
	) const
	//
	// Returns the cull distance from the partition supplied as a parameter.
	//
	//**************************
	{
		return (static_cast<const CPartition::CPriv*>(ppart))->fGetCullDistance();
	}

	//*****************************************************************************************
	//
	float fGetVolume
	(
	) const
	//
	// Returns volume of this partition in world space.
	//
	// Notes:
	//		For now, assume volume means maximum world area.
	//
	//**************************
	{
		/*
		Assert(pbvBoundingVol());

		// Calculate the volume of the partition and return it.
		return pbvBoundingVol()->rGetVolume(fGetScale());
		*/
		CVector3<> v3_min; 
		CVector3<> v3_max;
		
		bGetWorldExtents(v3_min, v3_max);

		CVector3<> v3_delta = v3_max - v3_min;
		float f_len = float(sqrt(v3_delta.tX * v3_delta.tX + v3_delta.tY * v3_delta.tY));
		float f_area = f_len * v3_delta.tZ;
		return f_area;
	}

	//*****************************************************************************************
	//
	float fGetVolumeMesh
	(
	) const
	//
	// Returns volume of this partition if it has a mesh, otherwise returns 0.
	//
	//**************************
	{
		// If there is no shape associated with this partition, return 0.
		if (!pdData.bUsesMesh)
			return 0.0f;

		// Calculate the volume of the partition and return it.
		return fGetVolume();
	}

	//*****************************************************************************************
	//
	float fGetVolumeMesh
	(
		const CPartition* ppart
	) const
	//
	// Returns the volume of all the meshes used by this partition and is children. This
	// function is recursive.
	//
	//**************************
	{
		float f_volume = fGetVolumeMesh();

		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();
		if (!ppartc)
			return f_volume;

		for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			f_volume += (static_cast<CPartition::CPriv*>(*it))->fGetVolumeMesh(*it);

		return f_volume;
	}

	//*****************************************************************************************
	//
	float fGetAreaEfficiencySqr
	(
	) const
	//
	// Returns the square of the volume efficiency.
	//
	// Notes:
	//		The power 2 / 3 is applied to convert from a 
	//
	//**************************
	{
		float f_volume_used       = fGetVolumeMesh(this);
		float f_volume_total      = fGetVolume();
		//Assert(f_volume_total > 0.0f);
		if (f_volume_total <= 0.0f)
			f_volume_total = 1.0f;

		// Implied conversion from volume to area.
		//float f_volume_efficiency = float(pow(double(f_volume_used / f_volume_total), 0.6667));
		float f_volume_efficiency = f_volume_used / f_volume_total;

		// Return the result squared.
		return f_volume_efficiency * f_volume_efficiency;
	}

	//*****************************************************************************************
	//
	void SetConstructionValues
	(
	)
	//
	// Sets the initial state for partition values.
	//
	//**************************
	{
		// Links.
		ppartParent = 0;
		ppartChild  = 0;
		ppartNext   = 0;

		// Flags.
		pdData.bVisible           = 1;
		pdData.bUsesMesh          = 0;
		pdData.bFixedCullDistance = 0;
		pdData.bFixedCullShadow   = 0;
		pdData.bCacheable         = 0;
		pdData.bCacheIntersecting = 0;
		pdData.bCacheableVolume   = 0;
		pdData.bCacheNever        = 0;
		pdData.bOcclude           = 0;
		pdData.bCastShadow        = 1;
		pdData.bSimpleObject      = 0;
		pdData.bAlwaysFace        = 0;
		pdData.bHardwareAble      = 1;	// Must assume hardware until otherwise established.
		pdData.bHardwareReq       = 0;	// Turned on only using a text property.
		pdData.bHardwareLargeSize = 0;	// Turned on only using a text property.
		pdData.bHardwareOnly      = 0;	// Turned on only using a text property.
		pdData.bSoftwareOnly      = 0;	// Turned on only using a text property.
		pdData.bNoLowResolution   = 0;	// Turned on only using a text property.

		// Values.
		pdData.iPriority        = 0;
		sfCullDistanceSqr       = fFarAway;
		sfCullDistanceShadowSqr = fFarAway;
		sfSphereRadiusSqr        = 0.0f;
		sfCylinderRadiusSqr     = 0.0f;
		SetCacheMultiplier(d3dDriver.bUseD3D() ? (1.1f) : (1.0f));
		u4NoCacheUntilFrame      = 0;

		// Links.
		prencCache = 0;
	}

	//******************************************************************************************
	//
	// Member functions to initialize flag states.
	//

	//*****************************************************************************************
	//
	int iGetPriority
	(
	)
	//
	// Returns the priority for this partition.
	//
	//**************************
	{
		int i_priority = 4;

		// Use preset priorities for instances.
		if (bIsInstance())
			i_priority = pdData.iPriority;

		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		if (ppartc)
			for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				i_priority = Min(i_priority, (*it)->pdData.iPriority);
		
		// Clamp the priority.
		SetMinMax(i_priority, 0, 4);
		return i_priority;
	}

	//*****************************************************************************************
	//
	float fGetVolumeInstances
	(
	)
	//
	// Returns the volume of the instances in this partition.
	//
	//**************************
	{
		float f_volume = 0.0f;

		// If the partition is an instance, add its volume.
		if (bIsInstance())
			f_volume += fGetVolume();

		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		if (ppartc)
			for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				f_volume += (static_cast<CPartition::CPriv*>(*it))->fGetVolumeInstances();

		return f_volume;
	}

	//******************************************************************************************
	//
	bool bFlagOcclude
	(
	)
	//
	// Returns 'true' if partition contains any meshes containing occlusion.
	//
	//**************************************
	{
		// Test the bOcclude flag of the children.
		{
			// Get a pointer to the child list and iterate through it.
			CPartition* ppartc = (CPartition*)ppartChildren();
			if (ppartc)
			{
				for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
					if ((*it)->pdData.bOcclude)
						return true;
			}
		}

		// Test this partition for a mesh that contains occlusion polygons.
		{
			rptr<CMesh> pmsh = pmshGetMesh();
			if (pmsh)
			{
				if (pmsh->papmpOcclude.uLen)
					return true;
			}
		}
		return false;
	}

	//******************************************************************************************
	//
	bool bFlagCacheable
	(
	)
	//
	// Returns 'false' if there is a mesh that can not be cached, otherwise returns 'true.'
	//
	//**************************************
	{
		// Test if the never cache flag is set.
		if (pdData.bCacheNever)
			return false;

		// Test the bCacheable flag of the children.
		{
			// Get a pointer to the child list and iterate through it.
			CPartition* ppartc = (CPartition*)ppartChildren();
			if (ppartc)
			{
				for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
					if (!(*it)->pdData.bCacheable)
						return false;
			}
		}

		// If this is not an instance, it does not use a cacheable mesh.
		CInstance* pins = pinsGet();
		if (!pins)
			return true;

		// If the object has an animating mesh or contains alpha polygons, it is uncacheable.
		{
			rptr<CMesh> pmsh = pmshGetMesh();
			if (pmsh)
			{
				if (pmsh->bIsAnimated())
				{
					pmsh->bCacheableMesh = false;
					return false;
				}

			#if bCACHE_INVSIBLE_ALPHA
				// Search the surface materials for alpha textures.
				for (uint u = 0; u < pmsh->pasfSurfaces.uLen; ++u)
					if (pmsh->pasfSurfaces[u].ptexTexture && pmsh->pasfSurfaces[u].ptexTexture->seterfFeatures[erfALPHA_COLOUR])
					{
						if (pmsh->pampPolygons.uLen > 0)
						{
							pmsh->bCacheableMesh = false;
							return false;
						}
					}
			#elif
				// Search the surface materials for alpha textures.
				for (uint u = 0; u < pmsh->pasfSurfaces.uLen; ++u)
					if (pmsh->pasfSurfaces[u].ptexTexture && pmsh->pasfSurfaces[u].ptexTexture->seterfFeatures[erfALPHA_COLOUR])
					{
						pmsh->bCacheableMesh = false;
						return false;
					}
			#endif // bCACHE_INVSIBLE_ALPHA

			}
		}

		// If the object is a CAnimate, it cannot be cached.

		// If the object is a backdrop object, it cannot be cached.
		if (pins->bIsBackdrop())
			return false;

		// If this is an entity, check to see if it is water. Water is uncacheable.
		{
			CEntity* pet;
			Cast(&pet);
			if (pet)
			{
				if (dynamic_cast<CEntityWater*>(pet))
				{
					rptr<CMesh> pmsh = pmshGetMesh();
					Assert(pmsh);
					pmsh->bCacheableMesh = false;
					return false;
				}
			}
		}
		return true;
	}

	//******************************************************************************************
	//
	bool bFlagCacheableVolume
	(
	)
	//
	// Returns 'true' if this volume is efficient enough to cache.
	//
	// Notes:
	//		This function should only be called after 'bFlagCacheable' is called.
	//
	//**************************************
	{
		// If this partition is not cacheable, it does not have a cacheable volume.
		if (!pdData.bCacheable)
			return false;

		// Special case: if the volume is infinite, the partition is uncacheable.
		if (pbvBoundingVol()->ebvGetType() == ebvINFINITE)
			return false;

		// If this partition has a mesh, it is inheritently a cacheable volume.
		if (pshGetShape())
			return true;

		// If this partition has too many child instances, it is an uncacheable volume.
		if (iCountInstances() > rcsRenderCacheSettings.iMaxObjectsPerCache)
			return false;

		return true;
		/*

		// Test the volume.
		{
			// Get the volume of this partition.
			float f_vol = fGetVolume();

			// Get the volume of all child partitions that are instances.
			float f_instance_vol = fGetVolumeInstances();

			// Set the volume efficiency.
			if (f_instance_vol < 0.01f)
				f_instance_vol = 0.01f;
			f_instance_vol *= rcsRenderCacheSettings.fVolumeThreshold;

			// Set flag if the volume meets the ratio threshold.
			if (f_vol < f_instance_vol)
				return false;
		}
		return true;
		*/
	}

	//******************************************************************************************
	//
	bool bFlagCacheIntersecting
	(
	)
	//
	// Returns 'true' if this partition requires caches to be created using intersection.
	//
	//**************************************
	{
		// Test the bCacheIntersecting flag of the children.
		{
			// Get a pointer to the child list and iterate through it.
			CPartition* ppartc = (CPartition*)ppartChildren();
			if (ppartc)
			{
				for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
					if ((*it)->pdData.bCacheIntersecting)
						return true;
			}
		}

		// If this partition is an instance, return the value as set in the .grf file.
		CInstance* pins = pinsGet();
		if (pins)
			return pdData.bCacheIntersecting;

		// Return non-intersection by default.
		return false;
	}

	//******************************************************************************************
	//
	bool bFlagCastShadow
	(
	)
	//
	// Returns 'true' if this partition contains a mesh that casts a shadow.
	//
	// Notes:
	//		Unlike most of the rest of the flags, the setting of this flag does not depend
	//		on the settings of its children. The following objects do not cast shadows:
	//
	//			Water	CEntityWater
	//			Player	CPlayer
	//			Camera	CCamera;
	//
	//**************************************
	{
		CInstance* pins = pinsGet();

		// If shadows are turned off by default, always leave them off.
		if (!pdData.bCastShadow)
			return false;

		// If this partition is a pure spatial partition, return the default.
		if (!pins)
			return false;

		// If there is no mesh associated with this partition, it does not cast a shadow.
		if (!pmshGetMesh())
			return false;

		// As an optimization test entities only if an entity can be found.
		CEntity* pet;
		Cast(&pet);
		if (pet)
		{
			// Test Water.
			if (dynamic_cast<CEntityWater*>(pet))
				return false;

			// Test Player.
//			if (dynamic_cast<CPlayer*>(pet))
//				return false;

			// Test Camera.
			if (dynamic_cast<CCamera*>(pet))
				return false;
		}

		// For now, simple return the value set in the object's constructor.
		return true;
	}

	//******************************************************************************************
	//
	bool bFlagSimpleObject
	(
	)
	//
	// Returns 'true' if partition contains a mesh that has only one or two polygons and if
	// there are no children of this partition.
	//
	//**************************************
	{
		// If this partition is marked as a simple object, keep it marked as a simple object.
		if (pdData.bSimpleObject)
			return true;

		// It is not a simple mesh if this partition has children.
		if (ppartChildren())
			return false;

		// If the partition has no children, attempt to set the simple object flag.
		rptr<CShape> psh = pshGetShape();
		if (!psh)
			return false;
		return psh->bSimpleShape();
	}

	//******************************************************************************************
	//
	// Member functions to initialize flag states.
	//

	//******************************************************************************************
	//
	float fValueSphereRadius
	(
	) const
	//
	// Returns the radius of the sphere bounding this partition.
	//
	//**************************************
	{
		Assert(pbvBoundingVol());

		// If the bounding volume is infinite, return a really large number.
		if (pbvBoundingVol()->ebvGetType() == ebvINFINITE)
			return fFarAway;
		
		// Get the bounding sphere.
		float f_extent = pbvBoundingVol()->fMaxExtent();

		// Scale by the partition's presence.
		f_extent *= fGetScale();

		// Set a minimum radius for the world.
		if (f_extent < 5.0f)
			f_extent = 5.0f;

		// Return the modified bounding sphere.
		AlwaysAssert(f_extent > 0.0f && f_extent < FLT_MAX);
		return f_extent;
	}

	//******************************************************************************************
	//
	float fValueCylinderRadius
	(
	) const
	//
	// Returns the radius of the cylinder bounding this partition.
	//
	// Notes:
	//		The cylinder is axis-aligned on the Z axis.
	//
	//**************************************
	{
		CBoundVolBox bvb;
		CVector3<> av3[8];	// Array of world coordinates for the bounding box.
		const CBoundVol* pbv = pbvBoundingVol();
		Assert(pbv);

		// If the bounding volume is infinite, return a really large number.
		if (pbv->ebvGetType() == ebvINFINITE)
			return fFarAway;

		// Get a bounding box if there is one.
		const CBoundVolBox* pbvb = pbv->pbvbCast();

		// Store a flag indicating a true bounding box or not.
		bool b_retval = pbvb != 0;

		// Make a bounding box with the bounding sphere if there is no current bounding box.
		if (!pbvb)
		{
			// Create the temporary bounding box from the partition's bounding sphere.
			TReal r_dim = (TReal)pbv->fMaxExtent() * 2.0f;
			bvb = CBoundVolBox(r_dim, r_dim, r_dim);

			// Assign the pointer to the temporary bounding box.
			pbvb = &bvb;
		}

		// Transform the bounding box to the world.
		uint u_corner;
		for ( u_corner = 0; u_corner < 8; u_corner++)
			av3[u_corner] = (*pbvb)[u_corner] * pr3Presence();

		// Find the maximum radius.
		CVector2<> v2_centre = CVector2<>(v3Pos());

		TReal r_max_rad = (CVector2<>(av3[0]) - v2_centre).tLen();
		for (u_corner = 1; u_corner < 8; u_corner++)
		{
			TReal r_new_rad = (CVector2<>(av3[u_corner]) - v2_centre).tLen();
			r_max_rad = Max(r_max_rad, r_new_rad);
		}

		// Set the calculated radius.
		AlwaysAssert(r_max_rad > 0.0f && r_max_rad < FLT_MAX);
		return r_max_rad;
	}

	//*****************************************************************************************
	//
	float fValueCullDistanceFromShape
	(
	) const
	//
	// Returns the culling distance.
	//
	//**************************************
	{
		// For now, base the culling on the radius of the partition's bounding cylinder.
		float f_radius = rGetCylinderRadius();

		// Given a radius, determine the cull distance.
		return SPartitionSettings::fGetCullDistance(f_radius) + f_radius;
	}

	//*****************************************************************************************
	//
	void FindMaxCullDistanceShape
	(
		const CPartition* ppart,
		bool&             rb_found_cull_distance,
		float&            rf_max_cull_distance
	) const
	//
	// Returns the maximum culling distance for a mesh contained by this partition or any of
	// its descendants.
	//
	//**************************************
	{
		// Iterate through the children of this partition.
		{
			// Get a pointer to the child list and iterate through it.
			CPartition* ppartc = (CPartition*)ppart->ppartChildren();
			if (ppartc)
				for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
					FindMaxCullDistanceShape(*it, rb_found_cull_distance, rf_max_cull_distance);
		}

		//
		// Test this partition to determine if it contains a mesh-based culling distance, and
		// if that distance is the maximum one so far.
		//
		if (pshGetShape())
		{
			float f_cull_distance = fGetCullDistance();
			if (rb_found_cull_distance)
			{
				rf_max_cull_distance = Max(rf_max_cull_distance, f_cull_distance);
			}
			else
			{
				rb_found_cull_distance = true;
				rf_max_cull_distance   = f_cull_distance;
			}
		}
	}

	//*****************************************************************************************
	//
	float fValueCullDistance
	(
	)
	//
	// Returns the culling distance.
	//
	//**************************************
	{
		//
		// If the volume is an infinite volume, return the maximum.
		//
		Assert(pbvBoundingVol());
		if (pbvBoundingVol()->ebvGetType() == ebvINFINITE)
			return fFarAway;

		// Special settings for backdrop objects.
		CInstance* pins = pinsGet();
		if (pins)
		{
			if (pins->bIsBackdrop())
			{
				priv_self.SetCullDistance(fFarAway);
				return fFarAway;
			}

			//
			// If this object contains a mesh ignore all other mechanisms for determining the
			// cull distances.
			//
			if (pshGetShape())
				return fValueCullDistanceFromShape();
		}

		//
		// Base the cull distance on the largest child cull distance plus the radius of the
		// bounding sphere of the partition.
		//
		bool  b_found_cull_distance = false;
		float f_max_cull_distance;

		// Iterate through the children of this partition.
		{
			// Get a pointer to the child list and iterate through it.
			CPartition* ppartc = (CPartition*)ppartChildren();
			if (ppartc)
				for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
					FindMaxCullDistanceShape(*it, b_found_cull_distance, f_max_cull_distance);
		}

		// If a culling distance is not found, generate one from the bounding cylinder.
		if (!b_found_cull_distance)
		{
			float f_radius = rGetCylinderRadius();
			return SPartitionSettings::fGetCullDistance(f_radius) + f_radius;
		}

		// Return the calculated culling distance.
		AlwaysAssert(f_max_cull_distance > 0.1f && f_max_cull_distance < 10000000.0f);
		return f_max_cull_distance + rGetCylinderRadius();
	}

	//*****************************************************************************************
	//
	float fValueCullDistanceFromShapeShadow
	(
	) const
	//
	// Returns the culling distance.
	//
	//**************************************
	{
		// For now, base the culling on the radius of the partition's bounding cylinder.
		float f_radius = rGetCylinderRadius();

		// Given a radius, determine the cull distance.
		return SPartitionSettings::fGetCullDistanceShadow(f_radius) + f_radius;
	}

	//*****************************************************************************************
	//
	void FindMaxCullDistanceShapeShadow
	(
		const CPartition* ppart,
		bool&             rb_found_cull_distance,
		float&            rf_max_cull_distance
	) const
	//
	// Returns the maximum culling distance for a mesh contained by this partition or any of
	// its descendants.
	//
	//**************************************
	{
		// Iterate through the children of this partition.
		{
			// Get a pointer to the child list and iterate through it.
			CPartition* ppartc = (CPartition*)ppart->ppartChildren();
			if (ppartc)
				for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
					FindMaxCullDistanceShapeShadow(*it, rb_found_cull_distance, rf_max_cull_distance);
		}

		//
		// Test this partition to determine if it contains a mesh-based culling distance, and
		// if that distance is the maximum one so far.
		//
		if (pshGetShape())
		{
			float f_cull_distance = fGetCullDistanceShadow();
			if (rb_found_cull_distance)
			{
				rf_max_cull_distance = Max(rf_max_cull_distance, f_cull_distance);
			}
			else
			{
				rb_found_cull_distance = true;
				rf_max_cull_distance   = f_cull_distance;
			}
		}
	}

	//*****************************************************************************************
	//
	float fValueCullDistanceShadow
	(
	) const
	//
	// Returns the culling distance.
	//
	//**************************************
	{
		//
		// If the volume is an infinite volume, return the maximum.
		//
		Assert(pbvBoundingVol());
		if (pbvBoundingVol()->ebvGetType() == ebvINFINITE)
			return fFarAway;

		//
		// If this object contains a mesh ignore all other mechanisms for determining the
		// cull distances.
		//
		if (pshGetShape())
			return fValueCullDistanceFromShapeShadow();

		//
		// Base the cull distance on the largest child cull distance plus the radius of the
		// bounding sphere of the partition.
		//
		bool  b_found_cull_distance = false;
		float f_max_cull_distance;

		// Iterate through the children of this partition.
		{
			// Get a pointer to the child list and iterate through it.
			CPartition* ppartc = (CPartition*)ppartChildren();
			if (ppartc)
				for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
					FindMaxCullDistanceShapeShadow(*it, b_found_cull_distance, f_max_cull_distance);
		}

		// If a culling distance is not found, generate one from the bounding cylinder.
		if (!b_found_cull_distance)
		{
			float f_radius = rGetCylinderRadius();
			return SPartitionSettings::fGetCullDistanceShadow(f_radius) + f_radius;
		}

		// Return the calculated culling distance.
		return f_max_cull_distance + rGetCylinderRadius();
	}

	//*****************************************************************************************
	//
	float fGetMaxCacheMultiplier
	(
	) const
	//
	// Returns the maximum cache multiplier.
	//
	//**************************************
	{
		float f_cache_multiplier = fGetCacheMultiplier();

		// Iterate through the children of this partition.
		CPartition* ppartc = (CPartition*)ppartChildren();
		if (!ppartc)
			return f_cache_multiplier;

		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			f_cache_multiplier = Max(f_cache_multiplier, (*it)->fGetCacheMultiplier());

		return f_cache_multiplier;
	}

	//*****************************************************************************************
	//
	void GetMinMaxDistanceSqr
	(
		float& rf_min_sqr,			// Minimum distance from the camera.
		float& rf_max_sqr,			// Maximum distance from the camera.
		CShapePresence& rsp		// Shape and camera presence information.
	)
	//
	// Gets the minimum and maximum distances for a partition from the camera.
	//
	//**************************
	{
		CVector3<> av3[8];

		bGetBoundingBoxVertices(av3);

		// Get the camera's position in object space.
		CVector3<> v3_cam = rsp.pr3GetCamShape().v3Pos;

		// Initialize with boundary conditions.
		rf_min_sqr = FLT_MAX;
		rf_max_sqr = FLT_MIN;

		// Find the nearest and farthest corners of the box.
		for (uint u_corner = 0; u_corner < 8; ++u_corner)
		{
			float f_distance_sqr = (av3[u_corner] - v3_cam).tLenSqr();

			// Set the near and far values as required.
			if (u4FromFloat(f_distance_sqr) > u4FromFloat(rf_max_sqr))
				rf_max_sqr = f_distance_sqr;
			if (u4FromFloat(f_distance_sqr) < u4FromFloat(rf_min_sqr))
				rf_min_sqr = f_distance_sqr;
		}
		Assert(rf_min_sqr != FLT_MAX);
		Assert(rf_max_sqr != FLT_MIN);
	}

#if bTEST_MINMAX_DISTANCE

	//*****************************************************************************************
	//
	void VerifyGetMinMaxDistance
	(
		float f_min_calc,		// Minimum distance from the camera.
		float f_max_calc,		// Maximum distance from the camera.
		CShapePresence& rsp		// Shape and camera presence information.
	)
	//
	// Gets the minimum and maximum distances for a partition from the camera.
	//
	//**************************
	{
		float f_min_calc_sqr = f_min_calc * f_min_calc;
		float f_max_calc_sqr = f_max_calc * f_max_calc;
		float f_min_test_sqr;
		float f_max_test_sqr;
		GetMinMaxDistanceSqr(f_min_test_sqr, f_max_test_sqr, rsp);
		AlwaysAssert(f_min_test_sqr != FLT_MAX);
		AlwaysAssert(f_max_test_sqr != FLT_MIN);
		AlwaysAssert(bFurryEquals(f_min_calc_sqr, f_min_test_sqr, 0.2f));
		AlwaysAssert(bFurryEquals(f_max_calc_sqr, f_max_test_sqr, 0.2f));
	}

#else

	//*****************************************************************************************
	//
	void VerifyGetMinMaxDistance
	(
		float f_min_calc,		// Minimum distance from the camera.
		float f_max_calc,		// Maximum distance from the camera.
		CShapePresence& rsp		// Shape and camera presence information.
	)
	//
	// Member function does nothing.
	//
	//**************************
	{
	}

#endif // bTEST_MINMAX_DISTANCE

	//******************************************************************************************
	//
	bool bHardwareAble
	(
	)
	//
	// Returns 'true' if there is a mesh that can be put into hardware.
	//
	// Notes:
	//		Currently any mesh containing a bumpmap can be put into hardware.
	//
	//**************************************
	{
		// The hardware able flag should be on by default unless changed by a text property.
		if (!pdData.bHardwareAble)
			return false;

		// If the partition is not an instance, it doesn't contain a mesh and cannot be in hardware.
		CInstance* pins = pinsGet();
		if (!pins)
			return false;

		// If the partition does not contain a mesh, it cannot be in hardware.
		rptr<CMesh> pmsh = pmshGetMesh();
		if (!pmsh)
			return false;

		//
		// If the mesh contains any bumpmapped polgyons, or is animating, it cannot be in hardware.
		//
		//if (pmsh->bIsAnimated())
			//return false;

		for (uint u = 0; u < pmsh->pasfSurfaces.uLen; ++u)
		{
			if (pmsh->pasfSurfaces[u].ptexTexture && pmsh->pasfSurfaces[u].ptexTexture->seterfFeatures[erfBUMP])
			{
				return false;
			}
		}

		// No reason not to put the partition into hardware.
		return true;
	}

	friend class CPartition;

};


#endif // HEADER_LIB_GEOMDBASE_PARTITIONPRIVCLASS_HPP