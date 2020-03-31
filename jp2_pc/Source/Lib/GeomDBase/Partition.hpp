/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		The base class of any spatial object in the world database:
 *
 *			CPartition
 *				CSpace
 *
 * To do:
 *		Add loader capabilities.
 *		Optimize for certain bounding volumes.
 *		Implement a member function template iterator when member function templates become
 *		available.
 *
 * Notes:
 *		The object hierarchy can be described independant of the spatial techniques that
 *		actually describe the relationships between objects in the hierarchy. Accordingly,
 *		CPartition is an abstract base class that describes a node in the object hierarchy.
 *		CSpace is a descendant of CPartition describing a node with a bounding volume but
 *		no objects, while CInstance describes a descendant that is an object in the world.
 *
 *		Remove the count instances code after E3, unless it is still viewed as desirable - in
 *		which case it must be reworked to never incur extra memory or time penalties.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/Partition.hpp                                          $
 * 
 * 126   9/28/98 6:25p Pkeet
 * Forced inlined several member functions.
 * 
 * 125   9/27/98 7:59p Pkeet
 * Added a parameter to 'bAddChild' and 'bFastAddChild' that allows reparenting only if the
 * priorities allow it.
 * 
 * 124   9/20/98 7:31p Agrant
 * cast waterdisturbance
 * 
 * 123   9/18/98 10:48a Mlange
 * Added cast function for location trigger.
 * 
 * 122   9/17/98 3:53p Rwyatt
 * New flag for attached audio
 * 
 * 121   98.09.12 12:17a Mmouni
 * Added virtual function to get RenderType.
 * 
 * 120   98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 119   8/26/98 10:11p Pkeet
 * Reenabled partition building.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_PARTITION_HPP
#define HEADER_LIB_GEOMDBASE_PARTITION_HPP


//
// Required includes for this object.
//
#include "Lib/Transform/Presence.hpp"
#include "Lib/GeomDBase/Plane.hpp"
#include "Lib/Std/Set.hpp"
#include <string>
#include "Lib/Math/FloatShort.hpp"


//
// Forward declarations for CPartition.
//
class CBoundVol;
class CBoundVolSphere;
class CPartition;
class CInstance;
class CPartitionSpace;
class CConsoleBuffer;
class CRenderCache;
class CRenderContext;
class CTerrain;
class CTerrainObj;
class CEntity;
class CEntityWater;
class CAnimal;
class CPlayer;
class CCamera;
class CSubsystem;
class CEntityTrigger;
class CGun;
class CAnimate;
class CBoundaryBox;
class CShape;
class CMesh;
class CSocket;
class CTrigger;
class CLocationTrigger;
class TPartitionList;
class TPartitionListChild;
class CShapePresence;
class CRenderType;
class CWaterDisturbance;

//
// Constant values.
//

// Flag indicates partitions should allocate storage for optimized partition building.
#define bPARTITION_BUILD (1 && BUILDVER_MODE != MODE_FINAL)

// Number of bits used for flags in the partition data structure.
#define iNUM_DATA_BITS   (23)

//
// Class definitions.
//

//*********************************************************************************************
//
class CPartition
//
// Describes an abstraction of a spatial object hierarchy.
//
// Prefix: part
//
// Notes:
//
//**************************
{
public:

	struct SPartitionData
	{
		int iPriority          :4;	// Priority for including this partition in the world.
		int bVisible           :1;	// Flag is set to 'true' if the object may be rendered.
		int bUsesMesh          :1;	// Flag indicates partition uses a mesh directly.
		int bFixedCullDistance :1;	// Flag indicates cull distance is unchangeable.
		int bFixedCullShadow   :1;	// Flag indicates shadow cull distance fis unchangeable.
		int bCacheable         :1;	// Flag indicates this partition can be cached.
		int bCacheIntersecting :1;	// Flag for cache including intersecting objects.
		int bCacheableVolume   :1;	// Flag indicates desirable volume to cache.
		int bCacheNever        :1;	// Flag indicates object should never be cached.
		int bOcclude           :1;	// Flag indicates containment occluding polygons.
		int bCastShadow        :1;	// Flag indicates the partition can cast a shadow.
		int bSimpleObject      :1;	// Flag indicates that the object is not worth caching alone.
		int bAlwaysFace        :1;	// Flag indicates that the object always faces the camera.
		int bHardwareAble      :1;	// Flag indicates that contained mesh may be put into hardware.
		int bHardwareReq       :1;	// Flag indicates that hardware must be used if enabled.
		int bHardwareLargeSize :1;	// Flag indicates large size textures are permitted for hardware.
		int bHardwareOnly      :1;	// Flag indicates object may only render with hardware.
		int bSoftwareOnly      :1;	// Flag indicates object may only render with software.
		int bNoLowResolution   :1;	// Flag indicates that object's textures may not be rendered at low resolution.
		int bSampleAttached	   :1;	// Flag indicates that object has a sample attached to it
		int Reserved    :(32 - iNUM_DATA_BITS);	// Reserved bits for a floating point union.
	};

private:
	
	// Floating point parameters.
	CShortFloat    sfCacheMultiplier;		// Value to multiply cache quality measures by.
	CShortFloat    sfCullDistanceSqr;		// The visibility distance squared.
	CShortFloat    sfCullDistanceShadowSqr;	// The visibility distance squared for shadows.
	CShortFloat    sfCylinderRadiusSqr;		// Square of the radius of the bounding cylinder.
	CShortFloat    sfSphereRadiusSqr;		// Square of the radius of the bounding sphere.
	CShortFloat    sfAreaEfficiencySqr;		// The ratio of the amount of volume occupied by
											// meshes to the total volume.

	// Values.
	SPartitionData pdData;					// Flags and data.

	// Cache variables.
	uint32         u4NoCacheUntilFrame;		// Frame number to consider caching again.
	CRenderCache*  prencCache;				// Pointer to an associated render cache.
	
	// Pointers.
	CPartition*    ppartNext;				// Pointer to the next sibling partition.
	CPartition*    ppartChild;				// Pointer to the first child partition.
	CPartition*    ppartParent;				// Pointer to the parent partition.

	// Settings.
	static int     iPriority;					// Visibility priority.
	static float   fAreaEfficiencyThresholdSqr;	// Square of the threshold area efficiency
												// for image caching.
	static float   fCullDistanceCombined;		// Distance multiplier for culling objects.
	static float   fCullDistanceCombinedShadow;	// Distance multiplier for culling objects
												// while rendering shadows.

	// Miscellaneous
	static CPartition* ppartUnwanted;		// Culled partitions.

	// Important: Do not use any data here for anything but optimized partition building.
#if bPARTITION_BUILD
public:
	float      fVolumeScore;
	CVector3<> v3WorldMin;
	CVector3<> v3WorldMax;
#endif // bPARTITION_BUILD

public:

	static int iDuplicateCount;				// The number of duplicate partitions removed.

public:

	//*****************************************************************************************
	//
	// Associated classes.
	//

	//*****************************************************************************************
	//
	class iterator
	//
	// Provides a specialized iterator for CPartition consistent with STL.
	//
	// Prefix: it (same as any STL iterator).
	//
	//**************************
	{
	private:

		CPartition* ppartCurrent;	// Pointer to the current partition.

	public:
		
		//*************************************************************************************
		//
		// Constructors.
		//

		// Default constructor.
		forceinline iterator()
			: ppartCurrent(0)
		{
		}

		// Constructor using a partition pointer.
		forceinline iterator(CPartition* ppart)
			: ppartCurrent(ppart)
		{
		}
		
		//*************************************************************************************
		//
		// Member functions.
		//
		
		//*************************************************************************************
		//
		forceinline void operator ++()
		//
		// Sets the iterator to the next sibling of the current partition.
		//
		//**************************
		{
			Assert(ppartCurrent);

			ppartCurrent = ppartCurrent->ppartNext;
		}
		
		//*************************************************************************************
		//
		forceinline bool operator !=(iterator it)
		//
		// Returns 'true' if this iterator and the specified iterator do not point to the
		// same partition.
		//
		//**************************
		{
			return ppartCurrent != it.ppartCurrent;
		}
		
		//*************************************************************************************
		//
		forceinline bool operator ==(iterator it)
		//
		// Returns 'true' if this iterator and the specified iterator point to the same
		// partition.
		//
		//**************************
		{
			return ppartCurrent == it.ppartCurrent;
		}
		
		//*************************************************************************************
		//
		forceinline CPartition* operator*()
		//
		// Returns the current partition's pointer.
		//
		//**************************
		{
			return ppartCurrent;
		}
	};

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	// Default constructor.
	CPartition
	(
	);

	// Construct from a file.
	CPartition
	(
	 	int i_handle	// File handle.
	);
	
	// Destructor.
	virtual ~CPartition();


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	virtual void InitializeDataStatic
	(
	);
	//
	// Initializes all data that will not change through the course of the game for the
	// partition except for links.
	//
	// Notes:
	//		This function is recursive.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void InitializeDataStaticTerrain
	(
	);
	//
	// Same as 'InitializeDataStatic' with reduced functionality for terrain.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void InitializeDataVolatile
	(
	);
	//
	// Initializes all dynamic data for the partition except for links.
	//
	// Notes:
	//		This function is recursive.
	//
	//**************************

	//*****************************************************************************************
	//
	forceinline float fGetCacheMultiplier
	(
	) const
	//
	// Returns the value of the 'fCacheMultiplier.'
	//
	//**************************
	{
		return float(sfCacheMultiplier);
	}

	//*****************************************************************************************
	//
	void SetCacheMultiplier
	(
		float f_cache_multiplier
	);
	//
	// Sets the value of the 'fCacheMultiplier.'
	//
	//**************************

	//*****************************************************************************************
	//
	forceinline bool bIsVisible
	(
	) const
	//
	// Returns the state of the 'bVisible' flag.
	//
	//**************************
	{
		return pdData.bVisible;
	}

	//*****************************************************************************************
	//
	forceinline bool bIsWithinPriority
	(
	) const
	//
	// Returns 'true' if the priority of the object is less than or equal to the priority
	// setting.
	//
	//**************************
	{
		return pdData.iPriority <= iPriority;
	}

	//*****************************************************************************************
	//
	void SetRenderPriority
	(
		int i_priority
	)
	//
	// Sets the priority for this partition..
	//
	//**************************
	{
		Assert(i_priority > 0);
		Assert(i_priority <= 4);

		pdData.iPriority = i_priority;
	}

	//*****************************************************************************************
	//
	void SetVisible
	(
		bool b_visible	// The new state for the 'bVisible' flag.
	);
	//
	// Returns the state of the 'bVisible' flag.
	//
	//**************************

	//*****************************************************************************************
	//
	forceinline bool bGetAlwaysFace
	(
	) const
	//
	// Returns the state of the 'bAlwaysFace' flag.
	//
	//**************************
	{
		return pdData.bAlwaysFace != 0;
	}

	//*****************************************************************************************
	//
	forceinline bool bTestBoxIntersection
	(
	) const
	//
	// Returns 'true' if the bounding box intersection test should be performed in the
	// pipeline.
	//
	//**************************
	{
		return !pdData.bAlwaysFace && !pdData.bSimpleObject;
	}

	//*****************************************************************************************
	//
	void SetFlagAlwaysFace
	(
		bool b_always_face = true	// New value for the 'bAlwaysFace' flag.
	);
	//
	// Sets the 'bAlwaysFace' flag to the provided value.
	//
	//**************************

	//*****************************************************************************************
	//
	forceinline bool bIsSampleAttached
	(
	)
	//
	// returns 'true' if object has a sample attached to it
	//
	//**************************
	{
		return pdData.bSampleAttached != 0;
	}


	//*****************************************************************************************
	//
	forceinline void SetSampleAttached
	(
		bool b
	)
	//
	// Set the sample attached bit
	//
	//**************************
	{
		pdData.bSampleAttached = b;
	}


	//*****************************************************************************************
	//
	void SetFlagHardwareAble
	(
		bool b	// New value for the flag.
	)
	//
	// Sets the 'bHardwareAble' flag to the provided value.
	//
	//**************************
	{
		pdData.bHardwareAble = b;
	}

	//*****************************************************************************************
	//
	void SetFlagHardwareOnly
	(
		bool b
	)
	//
	// Sets hardware flags.
	//
	//**************************
	{
		pdData.bHardwareOnly = b;
	}

	//*****************************************************************************************
	//
	void SetFlagSoftwareOnly
	(
		bool b
	)
	//
	// Sets hardware flags.
	//
	//**************************
	{
		pdData.bSoftwareOnly = b;
	}

	//*****************************************************************************************
	//
	void SetFlagNoLowResolution
	(
		bool b
	)
	//
	// Sets hardware flags.
	//
	//**************************
	{
		pdData.bNoLowResolution = b;
	}
	
	//*****************************************************************************************
	//
	forceinline const SPartitionData& pdGetData
	(
	) const
	//
	// Provides trivial access to data flags.
	//
	//**************************
	{
		return pdData;
	}

	//*****************************************************************************************
	//
	virtual CPresence3<> pr3Presence
	(
	) const = 0;
	//
	// Returns a copy of the partition's presence.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void SetPresence
	(
		const CPresence3<> &pr3	
	) = 0;
	//
	// Sets the partition's presence.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual CPlacement3<> p3Placement
	(
	) const = 0;
	//
	// Returns a copy of the partition's placement.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void SetPlacement
	(
		const CPlacement3<>& p3	
	) = 0;
	//
	// Sets the partition's placement.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual CVector3<> v3Pos
	(
	) const = 0;
	//
	// Returns the world position of the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void SetPos
	(
		const CVector3<>& v3_pos
	) = 0;
	//
	// Sets the world position of the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual CRotate3<> r3Rot
	(
	) const = 0;
	//
	// Returns the world position of the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void SetRot
	(
		const CRotate3<>& r3_rot
	) = 0;
	//
	// Sets the world position of the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual float fDistanceFromGlobalCameraSqr
	(
	) const;
	//
	// Returns the distance from the global camera to this partition.
	//
	//**************************

	//*****************************************************************************************
	//
	TReal rGetCylinderRadius
	(
	) const;
	//
	// Returns the radius of the Z axis aligned cylinder bounding the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	TReal rGetSphereRadius
	(
	) const;
	//
	// Returns the radius of the sphere bounding the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	TReal rGetCylinderRadiusSqr
	(
	) const;
	//
	// Returns the square of the radius of the Z axis aligned cylinder bounding the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	TReal rGetSphereRadiusSqr
	(
	) const;
	//
	// Returns the square of the radius of the sphere bounding the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	void FixCullDistance
	(
		float f_cull_distance
	);
	//
	// Sets the cull distance and a flag that locks the cull distance.
	//
	//**************************

	//*****************************************************************************************
	//
	void FixCullDistanceShadow
	(
		float f_cull_distance
	);
	//
	// Sets the cull distance and a flag that locks the cull distance for the shadow.
	//
	//**************************

	//*****************************************************************************************
	//
	void SetFlagShadow
	(
		bool b_flag	// New flag state.
	);
	//
	// Turns shadows on or off for this partition.
	//
	// Notes:
	//		Once the shadow is turned off, it will remain off.
	//
	//**************************

	//*****************************************************************************************
	//
	void SetFlagCacheIntersecting
	(
		bool b_flag	// New flag state.
	);
	//
	// Turns shadows on or off for this partition.
	//
	//**************************

	//*****************************************************************************************
	//
	void SetFlagNoCache
	(
		bool b_flag	// New flag state.
	)
	//
	// Turns caching on or off for this partition.
	//
	//**************************
	{
		pdData.bCacheNever = b_flag;
	}

	//*****************************************************************************************
	//
	void SetFlagNoCacheAlone
	(
		bool b_flag	// New flag state.
	)
	//
	// Turns single object caching on or off for this partition.
	//
	//**************************
	{
		pdData.bSimpleObject = b_flag;
	}

	//*****************************************************************************************
	//
	forceinline iterator begin
	(
	)
	//
	// Returns an iterator corresponding to this element of a linked list, presumed to be the
	// first element in the list.
	//
	// Notes:
	//		This function is designed to provide an STL-style interface.
	//
	//**************************
	{
		return iterator(this);
	}

	//*****************************************************************************************
	//
	forceinline iterator end
	(
	)
	//
	// Returns an iterator corresponding to one past the last element of a linked list, which
	// is always a null element.
	//
	// Notes:
	//		This function is designed to provide an STL-style interface.
	//
	//**************************
	{
		return iterator(0);
	}

	//*****************************************************************************************
	//
	virtual bool bIncludeInBuildPart
	(
	) const
	//
	// Returns 'true' if this partition should be included in building partitions.
	//
	//**************************
	{
		return true;
	}

	//*****************************************************************************************
	//
	virtual void Move
	(
		const CPlacement3<>& p3_new,	// New placement for partition.
		CEntity* pet_sender = 0			// Entity responsible for the move (e.g. PhysicsSystem).
	);
	//
	// Change the placement portion of the partition's presence, and do anything else that it
	// alone knows it must do.
	// 
	//**********************************

	//*****************************************************************************************
	//
	forceinline CPartition*& rppartChildren
	(
	)
	//
	// Returns a pointer to the pointer of the partition's children list.
	//
	//**************************
	{
		return ppartChild;
	}

	//*****************************************************************************************
	//
	forceinline CPartition* ppartChildren
	(
	)
	//
	// Returns a pointer to the partition's children list.
	//
	//**************************
	{
		return ppartChild;
	}

	//*****************************************************************************************
	//
	forceinline const CPartition* ppartChildren
	(
	) const
	//
	// Returns a const pointer to the partition's children list.
	//
	//**************************
	{
		return ppartChild;
	}

	//*****************************************************************************************
	//
	virtual void Cast(CInstance** ppins)
	//
	// CInstance cast. In this class, this call does nothing. If an object inherited from this
	// class is a CInstance or derived object, a valid pointer will be assigned.
	//
	//**************************
	{
		*ppins = 0;
	}

	//*****************************************************************************************
	//
	virtual void Cast(CPartitionSpace** ppparts)
	//
	// CPartitionSpace cast. In this class, this call does nothing. If an object inherited from
	// this class is a CPartitionSpace or derived object, a valid pointer will be assigned.
	//
	//**************************
	{
		*ppparts = 0;
	}


	//*****************************************************************************************
	//
	virtual void Cast(CTerrainObj** ptrrobj)
	//
	// CTerrainObj cast. In this class, this call does nothing. If an object inherited from
	// this class is a CTerrainObj or derived object, a valid pointer will be assigned.
	//
	//**************************
	{
		*ptrrobj = 0;
	}


	//*****************************************************************************************
	virtual void Cast(CEntity** ppet)
	{
		*ppet = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CEntityWater** ppetw)
	{
		*ppetw = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CWaterDisturbance** pwd)
	{
		*pwd = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CGun** ppgun)
	{
		*ppgun = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CAnimal** ppani)
	{
		*ppani = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CPlayer** ppplay)
	{
		*ppplay = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CCamera** ppins)
	{
		*ppins = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CSubsystem** ppsub)
	{
		*ppsub = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CTerrain** ptrr)
	{
		*ptrr = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CEntityTrigger** ppettrig)
	{
		*ppettrig = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CAnimate** ppant)
	{
		*ppant = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CBoundaryBox** ppbb)
	{
		*ppbb = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CTrigger** ppant)
	{
		*ppant = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CLocationTrigger** ppant)
	{
		*ppant = 0;
	}

	//*****************************************************************************************
	virtual void Cast(CSocket** psock)
	{
		*psock = 0;
	}

	//*****************************************************************************************
	//
	virtual bool bIsInstance() const
	//
	// Returns true if this is an instance object.
	//
	//**************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	virtual const CBoundVol* pbvBoundingVol
	(
	) const = 0;
	//
	// Returns the bounding volume for the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual float fGetScale
	(
	) const = 0;
	//
	// Returns the scale of the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void SetScale
	(
		float f_new_scale	// Value to set scale to.
	) = 0;
	//
	// Returns the scale of the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual ESideOf esfSideOf
	(
		const CPartition* ppart	// Partition to check against for intersection.
	) const;
	//
	// Returns the relationship of ppart to this partition, in world space:
	// esfINSIDE if ppart is inside this, esfOUTSIDE if outside, esfINTERSECT if they intersect.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bEmptyPart
	(
	) const;
	//
	// Returns 'true' if the partition is empty, otherwise returns 'false.'
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void PruneEmpties
	(
	);
	//
	// Recursively removes empty partitions.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void SimplifyChild
	(
	);
	//
	// Recursively tests child partitions, and if a partition only has one child that is a
	// pure spatial partition, it removes it.
	//
	//**************************

	//*****************************************************************************************
	//
	static void RemoveUnwanted
	(
	);
	//
	// Removes partitions placed in the unwanted list.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void BuildPartList
	(
	 	const CPartition* ppart,	// Partition to test for intersection.
		TPartitionList&   rpartlist	// List to add partition pointers to.
	);
	//
	// Builds a list of CPartition objects given a bounding volume and a world presence.
	//
	// Notes:
	//		This function is recursive.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void BuildPartListAll
	(
	 	const CPartition* ppart,	// Partition to test for intersection.
		TPartitionList&   rpartlist	// List to add partition pointers to.
	);
	//
	// Builds a list of CPartition objects given a bounding volume and a world presence.
	//
	// Notes:
	//		This function is recursive.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bCanHaveChildren
	(
	);
	//
	// Returns 'true' if the partition can add children.
	//
	// Notes:
	//		This function prevents partitions from implementing the 'bAddChild' function; this
	//		is important for moving partitions like the camera.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bAddChild
	(
	 	CPartition* ppart,				// Partition to add to the child partition list.
		bool b_check_priority = false	// Flag to not allow higher priority partitions into
										// lower priority partitions.
	);
	//
	// Adds a CPartition object to the child list.
	//
	// Returns 'true' if the child was successfully added to the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bFastAddChild
	(
	 	CPartition* ppart,	// Partition to add to the child partition list.
		bool b_check_priority = false	// Flag to not allow higher priority partitions into
										// lower priority partitions.
	);
	//
	// Adds a CPartition object to the child list.
	//
	// Returns 'true' if the child was successfully added to the partition.
	//
	//	Notes:
	//		Will not send "ppart" down the tree of children- just quickly adds it to itself if possible.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void AddChildren
	(
	 	CPartition& rpart	// Partition to add to children from.
	);
	//
	// Adds as many children from the specified partition into the current partition as can
	// be contained. This function uses "bAddChild" to attempt the fit.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void SetParent
	(
	 	CPartition* ppart = 0	// Parent pointer for the current partition.
	);
	//
	// Sets the parent pointer for the partition. This function also removes the node from
	// its previous parent's list.
	//
	//**************************

	//*****************************************************************************************
	//
	forceinline CPartition* ppartGetParent
	(
	)
	//
	// Adds a CPartition object to the child list.
	//
	// Returns 'true' if the child was successfully added to the partition.
	//
	//**************************
	{
		return ppartParent;
	}

	//*****************************************************************************************
	//
	virtual bool bCanContain
	(
	 	CPartition* ppart	// Partition to test for containment.
	);
	//
	// Returns 'true' if the bounding volume for the current partition entirely encloses the
	// bounding volume for the test partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void Write
	(
	 	CConsoleBuffer& rcon,			// Console buffer to write to.
		int             i_depth = 0		// Recursive depth.
	) const;
	//
	// Writes the partition and its children to a console buffer.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual const char* strPartType
	(
	) const = 0;
	//
	// Returns a partition type string.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual int iCountInstances
	(
	);
	//
	// Counts the number of instances found in a partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bMakeChildList
	(
		TPartitionListChild& rpartlist_children
	);
	//
	// Makes a list of direct children.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void MoveSingleInstancesUp
	(
	);
	//
	// Moves instances with no siblings up in the parition list.
	//
	//**************************
	
	//*****************************************************************************************
	//
	virtual void Flatten
	(
		CPartition* ppart_top	// Top node to flatten to.
	);
	//
	// Flattens the spatial hierarchy so there are only two layers: the top node, and
	// everything else.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void ClearHierarchyPointers
	(
	);
	//
	// Removes parent, child, and next pointer info.
	// Dangerous-  use only when the partition is not part of an existing hierarchy.
	//
	//**************************
	
	//*****************************************************************************************
	//
	virtual void TakeChildren
	(
		CPartition& rpart	// Partition to take the children from.
	);
	//
	// Takes the children from the partition specified by the parameter and moves them into
	// this partition's child list.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void TakeNonSpatialChildren
	(
		CPartition& rpart	// Partition to take the children from.
	);
	//
	// Takes the non-spatial (i.e., a time trigger) children from the partition specified by
	// the parameter and moves them into this partition's child list.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual int iCountChildren
	(
	) const;
	//
	// Counts the number of objects in this partition's child list. This function counts only
	// direct children, it does not recurse through sublists.
	//
	//**************************

	//*****************************************************************************************
	//
	bool bValid
	(
	) const;
	//
	// Returns 'true' if the data is valid for the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bValidV
	(
	) const;
	//
	// Returns 'true' if the data is valid for the partition.
	//
	// Notes:
	//		Works exactly like 'bValid' but is virtual.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual int iCountAllChildren
	(
	) const;
	//
	// Counts the number of objects in this partition's child list. This function is recursive.
	//
	//**************************

	//*********************************************************************************************
	//
	virtual bool bNoSpatialInfo
	(
	) const;
	//
	// Returns 'true' if this partition has no useable spacial information.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bIsMoving
	(
	) const
	//
	// Returns 'true' if the instance is moving or is likely to move.
	//
	//**************************
	{
		return false;
	}

	//*****************************************************************************************
	//
	virtual rptr_const<CRenderType> prdtGetRenderType
	(
	) const
	//
	// Returns:
	//		The CShape object for an instance containing a renderable shape.
	//		If not an instance, or the shape is not renderable, returns rptr0.
	//
	//**************************
	{
		return rptr0;
	}

	//*****************************************************************************************
	//
	virtual rptr<CShape> pshGetShape
	(
	) const
	//
	// Returns:
	//		The CShape object for an instance containing a renderable shape.
	//		If not an instance, or the shape is not renderable, returns rptr0.
	//
	//**************************
	{
		return rptr0;
	}

	//*****************************************************************************************
	//
	virtual rptr<CMesh> pmshGetMesh
	(
	) const
	//
	// Returns:
	//		The CMesh object for an instance containing a renderable shape.
	//		If not an instance, or the shape is not renderable, returns rptr0.
	//
	//**************************
	{
		return rptr0;
	}

	//*****************************************************************************************
	//
	virtual bool bContainsShape
	(
	) const;
	//
	// Returns 'true' if this partition, or one of its children, grandchildren etc., contains
	// a shape.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bContainsMovingObject
	(
	) const;
	//
	// Returns 'true' if this partition, or one of its children, grandchildren
	// etc., contain a partition that is a moving instance.
	//
	//**************************

	//******************************************************************************************
	//
	virtual void PreRender
	(
		CRenderContext& renc			// Target, camera, settings, etc.
	);
	//
	// Called by pipeline if in view, just before rendering.
	// Should perform any pre-rendering tasks necessary.
	//
	//**********************************

	//*****************************************************************************************
	//
	virtual float fEstimateScreenSize
	(
		const CCamera& cam	// Camera that defines the view on the object.
	);
	//
	// Returns an approximation of the largest dimension of the object in screen space.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual float fEstimateScreenSizeChildren
	(
		const CCamera& cam	// Camera that defines the view on the object.
	);
	//
	// Returns an approximation of the largest dimension of the object's children in screen
	// space.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual float fEstimateScreenSize
	(
		const CCamera& cam,		// Camera that defines the view on the object.
		float f_distance_sqr	// The square of the distance from the object to the camera.
	);
	//
	// Returns an approximation of the largest dimension of the object in screen space.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bGetBoundingBoxVertices
	(
		CVector3<> av3[]	// Array of vertices to write the results to.
	) const;
	//
	// Provides the bounding box corners from the object.
	//
	// Returns:
	//		Returns 'true' if the bounding volume is a true bounding box.
	//
	// Notes:
	//		The order in which the corners are supplied in the array remains unchanged no
	//		matter how the partition is transformed.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bGetBoundingBoxVerticesCam
	(
		CVector3<>     av3[],	// Array of vertices to write the results to.
		const CCamera& cam		// Camera that defines the view on the object.
	) const;
	//
	// Transforms the bounding box corners from the object's local space into world and then
	// into camera space. If the partition doesn't have a bounding box, one is built from
	// a bounding sphere and used temporarily in deriving the corners.
	//
	// Returns:
	//		Returns 'true' if the bounding volume is a true bounding box.
	//
	// Notes:
	//
	//		The order in which the corners are supplied in the array remains unchanged no
	//		matter how the partition is transformed.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual bool bGetWorldExtents
	(
		CVector3<>& v3_min, 
		CVector3<>& v3_max
	) const;
	//
	// Gets the combined extents of actual physical objects within this partition, ignoring
	// objects of infinite volume, or which are not physical (such as cameras).
	// This may be different than the partition's bounding volume extents, which may be infinite.
	// 
	// Returns:
	//		Whether the partition has finite spatial extents.
	//		(The vectors are not set if this result is false).
	//
	//**********************************

	//*****************************************************************************************
	//
	void GetBoundingScreenBox
	(
		const CCamera& cam,		// Camera that defines the view on the object.
		CVector3<>&    rv3_min,	// Starting coordinates of the rectangle.
		CVector3<>&    rv3_max	// End coordinates of the rectangle.
	) const;
	//
	// Sets the min and max 3D coordinates to the screen rectangle bounding the projection of
	// the partition's bounding volume, on the screen.
	//
	//**************************************

	//*****************************************************************************************
	//
	TReal rGetBoundingScreenArea
	(
		const CCamera& cam		// Camera that defines the view on the object.
	) const;
	//
	// Returns an approximation of the screen area of the bounding rectangle of the projection
	// of this partition.
	//
	//**************************************

	//*****************************************************************************************
	//
	TReal rGetBoundingScreenAreaChildInstances
	(
		const CCamera& cam		// Camera that defines the view on the object.
	) const;
	//
	// Returns an approximation of the sum of screen areas of the bounding rectangle of the
	// projections of children  of this partition that are instances.
	//
	//**************************************

	//*****************************************************************************************
	//
	CInstance* pinsFindNamedInstance
	(
		std::string str_name,				// Name of instance.
		std::string str_class = ""			// Optional name of C++ class, for further disambiguation.
	) const;
	//
	// Returns the first instance found in the partition with matching name and class.
	//
	// Notes:
	//		Slow, not to used in shipping version.  For loading, debugging, and magnet/trigger 
	//		construction hacks.
	//
	//**************************************

	//*****************************************************************************************
	//
	CInstance* pinsFindInstance
	(
		uint32 u4_handle	// The handle of the instance we seek
	) const;
	//
	// Returns the first instance found in the partition with matching handle
	//
	//**************************************

	//*****************************************************************************************
	//
	CInstance* pinsFindInstance
	(
		std::string str_name					// Name of instance
	) const;
	//
	// Returns the first instance found in the partition with matching name
	//
	//	Notes:
	//		Faster than pinsFindNamedInstance, but not to be used very often.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	virtual TReal rGetWorldVolume
	(
	);
	//
	// Returns the world volume occupied by the partition.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void SortChildrenByVolume
	(
	);
	//
	// Orders the child list from the child with the smallest volume to the child with the
	// largest volume.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void StuffChildren
	(
	);
	//
	// Stuffs child partitions with other child partitions that can fit inside.
	//
	// Notes:
	//		This function, used after a call to 'SortChildrenByVolume' is use to build more
	//		efficient paritions.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual int iSizeOf
	(
	) const;
	//
	// Returns the size of the partition object in bytes.
	//
	// Notes:
	//		The reason for using this interface instead of sizeof(CPartition) or a template
	//		size is so that a) objects can be treated abstractly and b) to allow for the
	//		association of data with the object (ie., data for an object directly comes
	//		after the object in memory.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual bool bIsPureSpatial
	(
	) const;
	//
	// Returns 'true' if the object is a pure spatial partition (ie., is of type
	// CPartitionSpace).
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual bool bPreload
	(
		const CBoundVol* pbv,			// Query range.
		const CPresence3<>*	pr3,		// Presence of the bounding volume.
		bool b_is_contained = false		// Flag indicates if containment is already known.
	);
	//
	// Preloads object into memory.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual char* pcSave
	(
		char * pc_buffer // Buffer to save to.
	) const;
	//
	// Saves this object to a buffer.
	//
	//	Notes:
	//		Returns the address of the first unused byte in the buffer.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual char* pcSaveHierarchy
	(
		char * pc_buffer // Buffer to save to.
	) const;
	//
	// Saves this object to a buffer.
	//
	//	Notes:
	//		Returns the address of the first unused byte in the buffer.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual const char* pcLoad
	(
		const char * pc_buffer // Buffer to save to.
	) ;
	//
	// Saves this object to a buffer.
	//
	//	Notes:
	//		Returns the address of the first unused byte in the buffer.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual char* pcSaveSpatial
	(
		char* pc_buffer  // Save buffer
	) const;
	//
	// Saves the entire spatial hierarchy to a buffer.  Similar to pcSave in instance.hpp
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual uint32 u4GetUniqueHandle
	(
	) const;
	//
	// Returns a value uniquely representing the object.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bInRange
	(
		float f_distance_sqr
	) const;
	//
	// Returns 'true' if the value is within the 'fCullDistanceSqr' data member's range.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bInRangeShadow
	(
		float f_distance_sqr
	) const;
	//
	// Returns 'true' if the value is within the 'fCullDistanceShadowSqr' data member's range.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bCacheGroup
	(
		float f_distance_sqr
	) const;
	//
	// Returns 'true' if the value is within the 'fCullDistanceSqr' data member's range.
	//
	//**************************************

	//*****************************************************************************************
	//
	static void IncrementFrameCount
	(
	);
	//
	// Increments the frame count variable.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ResetFrameCounts
	(
	);
	//
	// Resets the frame counts for this partition and all its children.
	//
	//**************************************

#if bPARTITION_BUILD

	//*****************************************************************************************
	//
	virtual void SetWorldExtents
	(
	);
	//
	// Sets the world extents for this object and its children.
	//
	//**************************************

#endif // bPARTITION_BUILD


	//*****************************************************************************************
	//
	// Member functions associated with image caching.
	//

	//*****************************************************************************************
	//
	forceinline CRenderCache* prencGet
	(
	) const
	//
	// Returns a pointer to the render cache if there is one.
	//
	//**************************************
	{
		return prencCache;
	}

	//*****************************************************************************************
	//
	void CreateRenderCache
	(
		const CCamera& cam	// Camera to create the cache for.
	);
	//
	// Creates a render cache object for this partition if there isn't already one.
	//
	//**************************

	//*****************************************************************************************
	//
	void DeleteRenderCache
	(
	);
	//
	// Deletes the render cache object associated with the partition.
	//
	//**************************

	//*****************************************************************************************
	//
	void DeleteAllChildCaches
	(
	);
	//
	// Deletes the render cache objects associated with children of this partition.
	//
	//**************************

	//*****************************************************************************************
	//
	void DeleteAllCaches
	(
	);
	//
	// Deletes the render cache object associated with this partition and the children of
	// this partition.
	//
	//**************************

	//*****************************************************************************************
	//
	void SetCacheNoBuildPart
	(
		const CPartition* ppart,	// Partition to check against for intersection.
		int i_frame_delay = 2		// Number of frames to delay before reconsidereing caching.
	);
	//
	// Disables caching of this and all child partitions that intersect the provided partition.
	//
	//**************************

	//*****************************************************************************************
	//
	bool bCanCache
	(
	);
	//
	// Returns 'true' if the partition can be cached.
	//
	// The partition may be uncacheable because:
	//
	//		- It is inherently uncacheable.
	//		- It is or contains a moving physics object.
	//		- It has a mesh of one or two polgyons and has no children.
	//		- If caching is turned off (does not apply in final mode).
	//
	// If the partition cannot be cached, this function will delete an existing cache if there
	// is one.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetCacheNoBuild
	(
		int i_frame_delay = 2	// Number of frames to delay before caching is reconsidered.
	);
	//
	// Disables caching of this partition and its parents for the given number of frames.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void InvalidateRenderCache
	(
		const CPartition* ppart // Partition to invalidate for
	);
	//
	// Invalidates the render cache for this partition, or one of its children, grandchildren
	// etc. if the target partition intersects.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void InvalidateAllChildCaches
	(
	);
	//
	// Invalidates all render caches for this partition's children, grandchildren and so on.
	// It does not invalidate the cache for this partition.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void GetMinMaxDistance
	(
		float& rf_min,			// Minimum distance from the camera.
		float& rf_max,			// Maximum distance from the camera.
		CShapePresence& rsp		// Shape and camera presence information.
	);
	//
	// Gets the minimum and maximum distances for a partition from the camera.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual bool bIsEqualToSpatial
	(
		const CPartition* ppart
	) const;
	//
	// Returns 'true' if this partition is a spatial partition and is a duplicate of the
	// partition supplied as a parameter.
	//
	//**************************

	//*****************************************************************************************
	//
	virtual void RemoveDuplicates
	(
	);
	//
	// Recursively descends the partition tree and removes duplicate partitions.
	//
	//**************************

	//*****************************************************************************************
	//
	static void SetCombinedCulling
	(
		float f_cull,
		float f_cull_shadow
	);
	//
	// Sets the combined culling values for the regular renderer and the shadow renderer.
	//
	//**************************

	//*****************************************************************************************
	//
	static void GetCombinedCulling
	(
		float& rf_cull,
		float& rf_cull_shadow
	);
	//
	// Gets the combined culling values for the regular renderer and the shadow renderer.
	//
	//**************************

	//*****************************************************************************************
	//
	static float fGetAreaEfficiencyThreshold
	(
	);
	//
	// Returns the area efficiency threshold value.
	//
	//**************************

	//*****************************************************************************************
	//
	static void SetAreaEfficiencyThreshold
	(
		float f_area_efficiency	// The new value for the area efficiency threshold.
	);
	//
	// Sets the area efficiency threshold value.
	//
	//**************************

	//*****************************************************************************************
	//
	static void BeginDuplicateList
	(
	);
	//
	// Initializes the duplicate list.
	//
	//**************************

	//*****************************************************************************************
	//
	static void EndDuplicateList
	(
	);
	//
	// Clears the duplicate list.
	//
	//**************************

protected:

	//*****************************************************************************************
	void PushChildToList(CPartition* ppart);

	//*****************************************************************************************
	void RemoveChildFromList(CPartition* ppart);

	//*****************************************************************************************
	friend class iterator;

private:

	class CPriv;
	friend class CPriv;
	friend int  iGetPrioritySetting();
	friend void SetPrioritySetting(int i_priority);

};



//*********************************************************************************************
//
template<class T_TYPE> class ptCast
//
// Class that behaves like the dynamic_cast<> operator for classes derived from CInstance.
//
// Prefix: N/A
//
// Example:
//		CEntity et;
//
//		CPartition* ppart = &et;
//
//		CEntity* pet = ptCast<CEntity>(ppart);
//
//**************************
{
	T_TYPE* ptVal;

public:
	//*****************************************************************************************
	//
	// Constructors and destructors.
	//
	ptCast(CPartition* ppart)
	{
		ptVal = (T_TYPE*)ppart;
		if (ppart)
			ppart->Cast(&ptVal);
	}

	//*****************************************************************************************
	//
	// Overloaded operator.
	//
	operator T_TYPE*() const
	{
		return ptVal;
	}

};






//
// Global functions.
//

//*********************************************************************************************
//
void BuildOptimizedTree
(
	CPartition& rpart,						// Root partition.
	void (__cdecl *partcallback)(int i),	// Callback for the partition building function.
	bool b_terrain = false					// Flag indicates partitioning is for terrain.
);
//
// Builds an optimized partition tree for the given partition.
//
//**************************


#endif // HEADER_LIB_GEOMDBASE_PARTITION_HPP
