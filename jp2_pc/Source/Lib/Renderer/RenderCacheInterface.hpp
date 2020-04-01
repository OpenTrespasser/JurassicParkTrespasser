/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Interface to the CRenderCache object.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/RenderCacheInterface.hpp                                 $
 * 
 * 90    10/01/98 12:35p Pkeet
 * New defaults.
 * 
 * 89    9/01/98 6:29p Mmouni
 * Moved DumpMoveStopLists() function declaration.
 * 
 * 88    6/04/98 9:09p Pkeet
 * Changed the parallax shear default value.
 * 
 * 87    6/04/98 7:44p Pkeet
 * Increased the default parallax shear threshold value.
 * 
 * 86    6/04/98 6:49p Pkeet
 * Changed the 'fParallaxShear' default value.
 * 
 * 85    6/03/98 8:07p Pkeet
 * Changed the 'fParallaxShear' default value. Changed 'bShouldCache' to accept the
 * 'CShapePresence' variable.
 * 
 * 84    6/02/98 11:53a Pkeet
 * Added the 'fParallaxShear' data member.
 * 
 * 83    5/26/98 11:30p Pkeet
 * Changed constants for max cylinder angle and distortion multiplier.
 * 
 * 82    5/21/98 3:56p Pkeet
 * Increased the weight of the distortion error.
 * 
 * 81    5/12/98 6:10p Pkeet
 * Added code to freeze caches.
 * 
 * 80    5/10/98 1:52p Pkeet
 * Removed unused settings.
 * 
 * 79    5/03/98 5:13p Pkeet
 * Changed the minimum distance back to 10 metres.
 * 
 * 78    4/25/98 9:12p Agrant
 * New image cache settings, per Frame Rate Cabal April 17, 1998
 * 
 * 77    4/09/98 10:56a Pkeet
 * Provided a non-zero setting for the 'iEuthanasiaAge.'
 * 
 * 76    98.04.01 7:29p Mmouni
 * Added save/load methods to SRenderCacheSettings.
 * 
 * 75    3/20/98 12:20p Pkeet
 * Changed default settings.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_RENDERCACHEINTERFACE_HPP
#define HEADER_LIB_RENDERER_RENDERCACHEINTERFACE_HPP


//
// Includes.
//
#include "Lib/GeomDBase/Partition.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/GeomDBase/Shape.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include <list>


//
// Forward class declarations.
//
class CRenderCache;
class CLightList;
class CScheduler;
class CCamera;

//
// Global structures.
//

enum ERenderCacheModes
// Enumeration of render cache modes.
// Prefix: ercm
{
	ercmCACHE_OFF,
	ercmCACHE_ON
};


//
// Default constants.
//

// Default for radius to distance threshold.
const float fDefaultRadiusDistance = 0.4000f;


//
// Structures.
//

//*********************************************************************************************
//
struct SRenderCacheSettings
//
// Structure for render cache control settings.
//
// Prefix: rcs
//
//**************************************
{
	ERenderCacheModes erctMode;			// Render cache mode.
	bool  bAddIntersectingObjects;		// If 'true' adds intersecting objects to the cache.
	TReal rMinPolygonMesh;				// Minimum area to use the polygon mesh cache instead
	TReal rPixelsPerArea;
	TReal rPixelsPerLine;
	bool  bUseConvexPolygons;			// Flag to use convex polygons instead of rectangles.
	bool  bUseCameraPrediction;			// Use the predicted camera position.
	float fMaxAcceptCylinder;			// Set to inverse tan alpha squared plus one.
										// of the rectangular mesh for the cache.
	int   iMaxAcceptAngleCylinder;		// Maximum acceptable angle the view cone of the
										// object's cylinder in the main camera.
	float fDistortionWeight;			// Priority of angular distortion.
	float fEfficiencyArea;				// Efficiency of area to cache.
	int   iMaxObjectsPerCache;			// The maximum number of objects that can be cached.
	bool  bForceIntersecting;			// Forces intersecting objects on if true.
	bool  bFasterPhysics;				// Faster but not better handling of moving physics
										// objects.
	bool  bFreezeCaches;				// Flag to freeze render caches.
	float fParallaxShear;			// Prediction of longevity of a cache, represents the
										// near point of a cache relative to the camera divided
										// by the far point of a cache.

	//
	// To do:
	//		Remove references to these datamembers:
	//
	int   iEuthanasiaAge;				// Number of frames to leave an unrendered cache
										// untouched.

	//*****************************************************************************************
	//
	// Constructor.
	//

	// Default constructor.
	SRenderCacheSettings()
	{
		erctMode                = ercmCACHE_ON;
		bAddIntersectingObjects = true;
		rMinPolygonMesh         = TReal(96 * 64);
		rPixelsPerArea          = TReal(0.225);
		rPixelsPerLine          = TReal(4.0);
		bUseConvexPolygons      = true;
		bUseCameraPrediction    = false;
		bForceIntersecting      = false;
		iMaxObjectsPerCache     = 10;
		fEfficiencyArea         = 0.17f;
		fDistortionWeight       = 8000000.0f;
		bFasterPhysics          = true;
		iEuthanasiaAge          = 200;
		bFreezeCaches           = false;
		fParallaxShear          = 0.5f;

		SetMaxAngleCylinder(20);
	}


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void SetMaxAngleCylinder
	(
		int i_degrees
	);
	//
	// Sets the maximum view angle the cylinder of a cache can occupy.
	//
	//**************************************

	//*****************************************************************************************
	//
	char* pcSave
	(
		char* pc
	) const;
	//
	// Saves the render cache settings to a buffer.
	//
	//**************************************

	//*****************************************************************************************
	//
	const char* pcLoad
	(
		const char* pc
	);
	//
	// Loads the render cache settings.
	//
	//**************************************
};


//
// Classes.
//

//*********************************************************************************************
//
class CRenderCacheStats
//
// Object encapsulates statistics gathering for the render cache.
//
// Prefix: rcs
//
//**************************************
{
public:

	int  iTotalCacheAge;
	int  iNumCachedObjects;
	int  iNumCachesUpdated;
	bool bKeepStats;
	int  iNumCachesEuthanized;

public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	CRenderCacheStats()
	{
		Reset();
		bKeepStats = false;
	}

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Reset
	(
	)
	//
	// Resets the statistics counts.
	//
	//**************************************
	{
		iTotalCacheAge       = 0;
		iNumCachedObjects    = 0;
		iNumCachesUpdated    = 0;
		iNumCachesEuthanized = 0;
	}

};


//
// Global functions.
//

//*********************************************************************************************
//
bool bShouldCache
(
	CPartition*          ppart,				// Partition to test for caching.
	const CCamera&       cam,				// Camera whose scene contains the partition.
	float                f_distance_sqr,	// Square of the distance from the partition to the camera.
	CShapePresence&      rsp,				// Presence information for the camera and partition.
	bool                 b_visible = true	// Flag indicating if the partition is in the current
											// camera view.
);
//
// Returns 'true' if the partition should be cached, otherwise returns 'false.'
//
//**************************************

//*********************************************************************************************
//
void EuthanizeOldCaches
(
);
//
// Destroys old caches. Called once per frame.
//
//**************************************

//*********************************************************************************************
//
int iCacheMemoryUsed
(
);
//
// Returns the amount of memory used for render caches in bytes.
//
//**************************************

//*********************************************************************************************
//
int iNumCaches
(
);
//
// Returns the amount of image caches currently being used.
//
//**************************************

//*********************************************************************************************
//
void AddToMovingList
(
	CPartition* ppart	// Partition pointer to add to list.
);
//
// Adds the partition to the moving list to update cache status.
//
//**************************************

//*********************************************************************************************
//
void AddToStoppedList
(
	CPartition* ppart	// Partition pointer to add to list.
);
//
// Adds the partition to the moving list to update cache status.
//
//**************************************

//*********************************************************************************************
//
void DumpMoveStopLists
(
);
//
// Clears the move and stop lists to a text file.
//
//**************************************

//*********************************************************************************************
//
void BeginCacheUpdate
(
);
//
// Uses the move and stop lists to set partition cache flags.
//
//**************************************

//*********************************************************************************************
//
void EndCacheUpdate
(
);
//
// Uses the move and stop lists to set partition cache flags.
//
//**************************************

//*********************************************************************************************
//
void AddUnseenCaches
(
	CScheduler&     scheduler,	// Scheduler object for current scene.
	const CCamera&  cam,		// Current player camera.
	CRenderContext& renc,		// The render context for the current scene.
	CPartition*     ppart = wWorld.ppartPartitionList()
								// Top level partition.
);
//
// Adds unseen caches at a low priority to the scheduler.
//
//**************************************

//*********************************************************************************************
//
void CollectCacheGarbage
(
);
//
// Deletes caches that are not sent to the scheduler for the current frame.
//
//**************************************

//
// Externally defined global variables.
//

// Settings for the render caching module.
extern SRenderCacheSettings rcsRenderCacheSettings;

// Stats gathered from render caching.
extern CRenderCacheStats    rcstCacheStats;

#endif // HEADER_LIB_RENDERER_RENDERCACHEINTERFACE_HPP
