/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Implementation of PartitionPriv.cpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/PartitionPriv.cpp                                       $
 * 
 * 11    98.08.19 6:01p Mmouni
 * Moved all quality setting values and multipliers into a single structure.
 * 
 * 10    98.06.30 9:40p Mmouni
 * Quality setting adjustment is now applied in SPartitionSettings::fGetCullDistance().
 * 
 * 9     6/29/98 10:20p Pkeet
 * Removed assert.
 * 
 * 8     5/14/98 7:28p Pkeet
 * Added functions for getting and setting the priority levels for the partitioning system.
 * 
 * 7     5/10/98 8:59p Pkeet
 * Reduced culling stats for shadows by default.
 * 
 * 6     5/03/98 4:00p Pkeet
 * Increased the minimum culling distance.
 * 
 * 5     5/01/98 7:50p Pkeet
 * Changed distance culling parameters to use objects with a maximum radius and cull according
 * to the distance of an object with the maximum radius.
 * 
 * 4     4/29/98 2:41p Pkeet
 * Added code for determining if the main camera has moved on a per frame basis using a
 * threshold.
 * 
 * 3     4/27/98 11:03a Pkeet
 * Changed cull shadow parameter.
 * 
 * 2     4/24/98 3:39p Pkeet
 * Added data members and code for shadow culling. Moved constants and code from the header
 * file.
 * 
 * 1     4/24/98 11:15a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include <math.h>
#include "Common.hpp"
#include "PartitionPriv.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/EntityDBase/QualitySettings.hpp"


//
// Constants.
//
const float fMinCullingDistance       = 20.0f;
const float fMaxCullingDistance       = 900.0f;
const float fMinCullingDistanceShadow = 10.0f;
const float fMaxCullingDistanceShadow = 200.0f;


//
// Class implementations.
//

//*********************************************************************************************
//
// SPartitionSettings implementation.
//

	//*****************************************************************************************
	//
	// SPartitionSettings member functions.
	//

	//*****************************************************************************************
	float SPartitionSettings::fGetCullDistance(float f_radius)
	{
		f_radius = Min(f_radius, fMaxRadius);

		// Calculate culling distance with quality multiplier.
		float f_cull_distance = f_radius * 
								(fCullMaxAtDistance * qdQualitySettings[iGetQualitySetting()].fCullDistScale)
								/ fMaxRadius;

		SetMinMax(f_cull_distance, fMinCullingDistance, fMaxCullingDistance);

		return f_cull_distance;
	}

	//*****************************************************************************************
	float SPartitionSettings::fGetCullMaxAtDistance()
	{
		return fCullMaxAtDistance;
	}

	//*****************************************************************************************
	void SPartitionSettings::SetCullMaxAtDistance(float f)
	{
		if (f < 1.0f)
			f = 1.0f;
		fCullMaxAtDistance = f;
	}

	//*****************************************************************************************
	float SPartitionSettings::fGetMaxRadius()
	{
		return fMaxRadius;
	}

	//*****************************************************************************************
	void SPartitionSettings::SetMaxRadius(float f)
	{
		fMaxRadius = f;
	}

	//*****************************************************************************************
	float SPartitionSettings::fGetMaxRadiusShadow()
	{
		return fMaxRadiusShadow;
	}

	//*****************************************************************************************
	void SPartitionSettings::SetMaxRadiusShadow(float f)
	{
		if (f > 4.0f)
			f = 4.0f;
		fMaxRadiusShadow = f;
	}

	//*****************************************************************************************
	float SPartitionSettings::fGetCullDistanceShadow(float f_radius)
	{
		f_radius = Min(f_radius, fMaxRadiusShadow);

		// Calculate shadow culling distance with quality multiplier.
		float f_cull_distance = f_radius * 
								(
									fCullMaxAtDistanceShadow * 
									qdQualitySettings[iGetQualitySetting()].fCullShadowDistScale
								)
								/ fMaxRadiusShadow;

		SetMinMax(f_cull_distance, fMinCullingDistanceShadow, fMaxCullingDistanceShadow);

		return f_cull_distance;
	}

	//*****************************************************************************************
	float SPartitionSettings::fGetCullMaxAtDistanceShadow()
	{
		return fCullMaxAtDistanceShadow;
	}

	//*****************************************************************************************
	void SPartitionSettings::SetCullMaxAtDistanceShadow(float f)
	{
		if (f < 1.0f)
			f = 1.0f;
		if (f > 200.0f)
			f = 200.0f;
		fCullMaxAtDistanceShadow = f;
	}

	//*****************************************************************************************
	void SPartitionSettings::SetGlobalPartitionData()
	{
		//
		// Get the ratio of the perfect width (512 pixels) and the current screen width, and
		// combine it with the distance multiplier.
		//
		float f_current_width = 512.0f;
		float f_ratio = f_current_width / 512.0f;

		if (prasMainScreen)
		{
			f_current_width = float(prasMainScreen->iWidth);
		}
		SetMinMax(f_current_width, 320.0f, 640.0f);

		CPartition::SetCombinedCulling
		(
			1.0f,
			1.0f
		);
		/*
		(
			f_ratio * //ullDistanceMultiplierSqr,
			f_ratio * //ullDistanceMultiplierShadowSqr
		);
		*/

		//
		// Set the main camera's position.
		//
		CWDbQueryActiveCamera wqcam(*pwWorld);
		if (wqcam.tGet())
		{
			CVector3<> v3_old      = v3GlobalCameraPosition;
			float      f_threshold = fCameraMoveThreshold * fCameraMoveThreshold;
			v3GlobalCameraPosition = wqcam.tGet()->v3Pos();

			bCameraMoved = (v3GlobalCameraPosition - v3_old).tLenSqr() > f_threshold;
		}
	}
	
	//*****************************************************************************************
	void SPartitionSettings::SetCameraMovementThreshold(float f)
	{
		fCameraMoveThreshold = f;
	}

	//*****************************************************************************************
	float SPartitionSettings::fGetCameraMovementThreshold()
	{
		return fCameraMoveThreshold;
	}
	

//
// Global function implementations.
//
	
//*********************************************************************************************
int iGetPrioritySetting()
{
	return CPartition::iPriority;
}

//*********************************************************************************************
void SetPrioritySetting(int i_priority)
{
	Assert(i_priority <= 4);

	CPartition::iPriority = i_priority;
}


//
// Static global variables.
//
float      SPartitionSettings::fMaxRadius                       = 8.0f;
float      SPartitionSettings::fCullMaxAtDistance               = 800.0f;
float      SPartitionSettings::fMaxRadiusShadow                 = 1.0f;
float      SPartitionSettings::fCullMaxAtDistanceShadow         = 120.0f;
uint32     SPartitionSettings::u4FrameNumber                    = 10;
CVector3<> SPartitionSettings::v3GlobalCameraPosition           = CVector3<>(0.0f, 0.0f, 0.0f);
float      SPartitionSettings::fCameraMoveThreshold             = 0.125f;
bool       SPartitionSettings::bCameraMoved                     = true;