/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Private classes used to allow opaque declarations for use with CPartition.
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/PartitionPriv.hpp                                      $
 * 
 * 9     6/03/98 8:15p Pkeet
 * Added the 'v3GetGlobalCameraPos' member function.
 * 
 * 8     5/14/98 7:26p Pkeet
 * Added functions for getting and setting priority levels in the partitioning system.
 * 
 * 7     5/01/98 7:49p Pkeet
 * Changed distance culling parameters to use objects with a maximum radius and cull according
 * to the distance of an object with the maximum radius.
 * 
 * 6     4/29/98 2:41p Pkeet
 * Added code for determining if the main camera has moved on a per frame basis using a
 * threshold.
 * 
 * 5     4/24/98 3:39p Pkeet
 * Added data members and code for shadow culling. Moved constants and code to the
 * implementation module.
 * 
 * 4     4/23/98 3:21p Pkeet
 * Removed unnecessary settings and associated member functions.
 * 
 * 3     4/22/98 5:05p Pkeet
 * Moved 'CPartition' settings header from 'Partition.hpp.'
 * 
 * 2     4/21/98 8:10p Pkeet
 * Added functions to set and get the distance multiplier.
 * 
 * 1     3/18/98 4:03p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_PARTITIONPRIV_HPP
#define HEADER_LIB_GEOMDBASE_PARTITIONPRIV_HPP

//
// Required includes for this object.
//
#include <list>
#include "Lib/EntityDBase/Container.hpp"
#include "Partition.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/GeomDBase/PartitionSpace.hpp"


//
// Constants.
//
const float fFarAway = 1000000.0f;


//
// Structure definitions.
//

//*********************************************************************************************
//
struct SPartitionListElement
//
// Structure for maintaining render lists with spatial relation information.
//
// Prefix: ple
//
//**************************
{
	CPartition* ppart;
	ESideOf     esfView;

	//*****************************************************************************************
	bool operator==(const SPartitionListElement& ple)
	{
		return ppart == ple.ppart;
	}
};

//*********************************************************************************************
//
struct SPartitionSettings
//
// Settings for use by CPartition.
//
// Prefix: pset
//
//**************************
{
private:


	// Static camera optimization parameters.
	static uint32     u4FrameNumber;				// Count of the number of frames rendered.
	static float      fCameraMoveThreshold;			// Frame-to-frame threshold for movement.
	static CVector3<> v3GlobalCameraPosition;		// Position of the current main camera.
	static bool       bCameraMoved;					// Set to 'true' if the camera moved
													// signficantly since the last frame.

	// Culling parameters for rendering.
	static float      fMaxRadius;					// Maximum radius for an object.
	static float      fCullMaxAtDistance;			// Distance at which an object of maximum
													// Radius would be culled.

	// Culling parameters for shadowing.
	static float      fMaxRadiusShadow;
	static float      fCullMaxAtDistanceShadow;

public:

	//*****************************************************************************************
	//
	static float fGetCullDistance
	(
		float f_radius	// Radius to determine the cull distance from.
	);
	//
	// Returns a culling distance for the given radius based on the culling distance settings.
	//
	//**************************************

	//*****************************************************************************************
	//
	static float fGetCullMaxAtDistance
	(
	);
	//
	// Returns the distance multiplier value.
	//
	//**************************

	//*****************************************************************************************
	//
	static void SetCullMaxAtDistance
	(
		float f
	);
	//
	// Sets the distance multiplier value to the new value.
	//
	//**************************

	//*****************************************************************************************
	//
	static float fGetMaxRadius
	(
	);
	//
	// Returns the maximum radius value.
	//
	//**************************

	//*****************************************************************************************
	//
	static void SetMaxRadius
	(
		float f
	);
	//
	// Sets the maximum radius value to the new value.
	//
	//**************************

	//*****************************************************************************************
	//
	static float fGetCullDistanceShadow
	(
		float f_radius	// Radius to determine the cull distance from.
	);
	//
	// Returns a culling distance for the given radius based on the culling distance settings.
	//
	//**************************************

	//*****************************************************************************************
	//
	static float fGetCullMaxAtDistanceShadow
	(
	);
	//
	// Returns the distance multiplier value.
	//
	//**************************

	//*****************************************************************************************
	//
	static void SetCullMaxAtDistanceShadow
	(
		float f
	);
	//
	// Sets the distance multiplier value to the new value.
	//
	//**************************

	//*****************************************************************************************
	//
	static float fGetMaxRadiusShadow
	(
	);
	//
	// Returns the maximum radius value.
	//
	//**************************

	//*****************************************************************************************
	//
	static void SetMaxRadiusShadow
	(
		float f
	);
	//
	// Sets the maximum radius value to the new value.
	//
	//**************************

	//*****************************************************************************************
	//
	static void SetGlobalPartitionData
	(
	);
	//
	// Sets values used by the partition to render the current frame.
	//
	// Notes:
	//		This function uses the difference between the preset screen resolution of 512x384
	//		and the current resolution to set 'fCullDistanceCombined.' This function also sets
	//		the global camera position.
	//
	//**************************

	//*****************************************************************************************
	//
	static void SetCameraMovementThreshold
	(
		float f	// New setting.
	);
	//
	// Sets 'fCameraMoveThreshold.'
	//
	//**************************

	//*****************************************************************************************
	//
	static float fGetCameraMovementThreshold
	(
	);
	//
	// Returns 'fCameraMoveThreshold.'
	//
	//**************************

	//*****************************************************************************************
	//
	static bool bCameraHasMoved
	(
	)
	//
	// Returns 'bCameraMoved.'
	//
	//**************************
	{
		return bCameraMoved;
	}

	//*****************************************************************************************
	//
	static CVector3<>& v3GetGlobalCameraPos
	(
	)
	//
	// Returns the global camera position.
	//
	//**************************
	{
		return v3GlobalCameraPosition;
	}

private:

	//class CPartition::CPriv;

	friend class CPartition;
	//friend class CPartition::CPriv;
	friend float CPartitionSpace::fDistanceFromGlobalCameraSqr() const;
	friend float CInstance::fDistanceFromGlobalCameraSqr() const;

};


//
// Global functions.
//

//*********************************************************************************************
//
int iGetPrioritySetting
(
);
//
// Returns the priority setting for the partitioning system.
//
//**************************

//*********************************************************************************************
//
void SetPrioritySetting
(
	int i_priority
);
//
// Sets the priority setting for the partitioning system.
//
//**************************



//
// Trivial classes.
//

// Type describing a container of CPartition pointers.
class TPartitionList : public CContainer< std::list<SPartitionListElement> >
// Prefix: partlist
{
};

// Type describing a container of CPartition pointers.
class TPartitionListChild : public CContainer< std::list<CPartition*> >
// Prefix: partlist
{
};


#endif // HEADER_LIB_GEOMDBASE_PARTITIONPRIV_HPP