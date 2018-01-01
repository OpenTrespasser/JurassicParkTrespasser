/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CRenderCache
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/RenderCache.hpp                                          $
 * 
 * 71    10/01/98 9:47p Pkeet
 * Changed the render cache list to an STL set.
 * 
 * 70    10/01/98 12:24a Pkeet
 * Fixed bug in the LRU.
 * 
 * 69    9/28/98 6:24p Pkeet
 * Fixed bug that left render cache behind when object moved.
 * 
 * 68    8/09/98 5:11p Pkeet
 * Added discreet allocations for render caches.
 * 
 * 67    98/06/29 16:25 Speter
 * Rendering functions now take CInstance*.
 * 
 * 66    6/21/98 2:44p Pkeet
 * Added the 'SetVisible' member function and moved code there from the 'UpdateFrameKey' member
 * function.
 * 
 * 65    6/09/98 5:32p Pkeet
 * Added support for non-region uploads.
 * 
 * 64    6/08/98 11:42a Pkeet
 * Added static cache variables and a member function for setting static cache variables per
 * frame.
 * 
 * 63    6/03/98 8:16p Pkeet
 * Changed 'Render' to use the 'CShapePresence' variable.
 * 
 * 62    5/21/98 3:56p Pkeet
 * Added code to dump the contents of the render cache system.
 * 
 * 61    5/20/98 3:03p Pkeet
 * Implemented the page manager for Direct3D image caches.
 * 
 * 60    5/05/98 1:34p Pkeet
 * Moved member function prototypes to the private class.
 * 
 * 59    4/20/98 3:21p Pkeet
 * Added the 'Purge' global function.
 * 
 * 58    4/15/98 5:48p Pkeet
 * Made the constructor and destructor for 'CRenderCache' private and made the functions to
 * create and destroy this object friends.
 * 
 * 57    4/13/98 5:08p Pkeet
 * Added the 'SetTextureMemSize' member function.
 * 
 * 56    4/13/98 3:36p Pkeet
 * Added member functions to get the amount of memory used and available in the system.
 * 
 * 55    4/13/98 11:38a Pkeet
 * Added LRU code for the image caches.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_RENDERCACHE_HPP
#define HEADER_LIB_RENDERER_RENDERCACHE_HPP

//
// Includes.
//
#include <set.h>
#include "RenderCacheInterface.hpp"
#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/Renderer/Pipeline.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Sys/Scheduler.hpp"
#include "Lib/GeomDBase/TexturePageManager.hpp"


//
// Forward declarations.
//
class CConsoleBuffer;
class CRenderCacheList;


//*********************************************************************************************
// Maximum number of vertices in the render cache convex buffer, (pav2Convex).
// The buffer is a CDArray so real memory is not commited until it is used, at maximum memory
// consumption is sizeof(CVector2<>)*u4MAX_CONVEX_VERTICES bytes.
// The u4CONVEX_VERTICES_COMMIT value is the maximum amount of elements that will remain 
// committed in this buffer across frames. If this limit is not reached then the current
// maximum used will remain.
// The minimum for this value is 0 which would force all memory to be decommitted after every
// cache build, this would be inefficient because memory would be recommitted on the very next
// build.
//
#define u4MAX_CONVEX_VERTICES		(8192)	// 65,536 BYTES
#define u4CONVEX_VERTICES_COMMIT	(4096)	// 32,768 BYTES
#define u4CONVEX_VERTICES_ALLOW  	(2048)


//
// Class definitions.
//

//*********************************************************************************************
//
class CShapeCache: public CMeshCache
//
// A shape for render caches.  A single polygon, with a streamlined rendering function.
//
//**************************************
{
public:
	CVector3<>      v3ControlActual;	// Object's actual control point.
	CVector3<>      v3ControlNear;		// Control point's projection on the near clipping
										// plane.
public:

	//******************************************************************************************
	CShapeCache
	(
		CPArray< CVector3<> > pav3_points,
		CPArray<CTexCoord> patc_tex,
		rptr<CTexture> ptex = rptr0
	)
		: CMeshCache(pav3_points, patc_tex, ptex)
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual void Render
	(
		const CInstance* pins,
		CRenderContext& renc, CShapePresence& rsp,
		const CTransform3<>& tf3_shape_camera, const CPArray<COcclude*>& papoc,
		ESideOf esf_view
	) const;
	//
	// Defined in Pipeline.cpp.
	//
};


//*********************************************************************************************
//
class CRenderCache
//
// Object encapsulates a pre-rendered shape or hierarchy of shapes.
//
// Prefix: renc
//
// Notes:
//		Initialization of this structure, other than inclusion of a mesh pointer, should occur
//		outside of the constructor to allow for persistence of the object, even when it is not
//		being used.
//
//**************************************
{
private:

	class CCacheResolution;
	CCacheResolution* pcrResolution;	// Factors for resolution tests.

	rptr<CShapeCache> pmshPolygon;		// Pointer to the newly constructed polygon.
	CRenderer::SSettings rensetSettings;// Render settings.
	aptr<CRenderer> prenContext;		// Pointer to the object used for rendering to the cache.
	rptr<CRaster>   prasScreenHardware;	// Video buffer.
	rptr<CRaster>   prasScreenSoftware;	// System memory buffer.
	int				iCount;				// How many times current cache used.
	TReal           rDistance;			// Distance from the object to the local camera.
	bool            bForceInvalid;		// If 'true,' the cache will always return 'false' to
										// the bIsValid cache test.
	int             iAge;				// Age of current cache in number of frames.
	ptr<CScreenRender> psrScreenR;		// Screen render object.
	static int      iNextCacheID;		// Next available cache ID.

	static int      iTotalSysBytes;		// Total amount of system memory used for textures.
	static int      iTotalD3DBytes;		// Total amount of D3D textures.
	int             iD3DBytes;			// Amount of D3D textures in hardware for this cache.
	int             iSysBytes;			// Amount of system memory used for textures for this cache.

	CTexturePageManager::CRegionHandle rhD3DHandle;	// Region handle for page-managed textures.

	CPArray< CVector3<> > pav3Projected;		// Point array for distortion box measure.
	CPArray< CVector3<> > pav3Actual;			// Point array for distortion box measure.
	CDir3<>               d3CacheNormal;		// Normal of the cache in world space.
	bool                  bInitialized;			// 'True' if the cache has already been
												// initialized.
	float                 fCreatedScreenSize;	// Estimated screen size of cache at its time
												// of creation.
	int					  iFrameKey;			// Frame key for this object.

	bool            bBuiltVisible;		// Flag indicating that the cache was built when
										// visible.
	float           fDistanceLastTest;	// Approximate distance at which the cache was last
										// tested.
	float           fDistanceBuild;		// Approximate distance at which the cache
										// was last built.

	//
	// Static parameters.
	//
	static int   iSmallestD3DAllocation;	// Smallest area in pixels that can be allocated.
	static float fGeneralScale;				// Amount to scale rasters by.
	static int   iLimitBytesD3D;			// Limit of texture memory in bytes.

public:

	static int      iLimitBytes;		// Limit of texture memory in bytes.
	bool            bNewlyVisible;		// Flag indicates if the cache is new in view.
	bool            bIsVisible;			// Flag indicates the cache is in the view of the
										// current camera.
	CPartition*		ppartShape;			// Pointer to the instance containing the original
										// shape.
	int             iEuthansiaAge;		// Number of frames since last used.
	int             iCacheID;			// Unique id for the cache.
	float           fAngularError;		// Last calculated angular error.
	float           fTexelToPixel;		// Last calculated texel to pixel ratio.
	static int      iFrameKeyGlobal;	// Global frame key value.

public:

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	rptr_const<CShape> pshGetShape
	(
	)
	//
	// Returns a pointer to the cached shape.
	//
	//**************************************
	{
		return rptr_cast(CShape, pmshPolygon);
	}

	//*****************************************************************************************
	//
	float fPriority
	(
		const CCamera& cam,						// Camera that defines the view on the object.
		CLightList& rltl_lights					// Light list for the scene.
	);
	//
	// Returns a value between 0 and 1 representing the priority for rebuilding this cache
	// relative to other caches and terrain textures. A priority of 0 means nothing should be
	// done while a priority of 1 means something has to be done. Values in between represent
	// the relative importance of updating this cache.
	//
	// Notes:
	//		The priority is established by compounding the angular distortion error with the
	//		texel to screen pixel error.
	//
	//**************************************

	//*****************************************************************************************
	//
	void UpdateRenderContext
	(
		const CCamera& cam,						// Camera that defines the view on the object.
		CLightList& rltl_lights					// Light list for the scene.
	);
	//
	// Creates a new CScreenRender object and buffers for that object. Also adjusts the size,
	// position and orientation of the pmshPolygon object.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Invalidate
	(
	)
	//
	// Invalidates the cache. The 'bIsValid' function will always return false.
	//
	//**************************************
	{
		bForceInvalid = true;
	}

	//*****************************************************************************************
	//
	CPartition* ppartGet
	(
	) const
	//
	// Returns the partition that owns the image cache.
	//
	//**************************************
	{
		return ppartShape;
	}

	//*****************************************************************************************
	//
	int iGetCacheAge
	(
	)
	//
	// Returns the age of the cache.
	//
	//**************************************
	{
		return iAge;
	}

	//*****************************************************************************************
	//
	int iGetMemUsed
	(
	);
	//
	// Returns the amount of memory used for the cache.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetVisible
	(
		bool b_is_visible	// Flag indicates the cache is in the view of the current camera.
	)
	//
	// Updates the visibility flag and the age for the cache.
	//
	//**************************************
	{
		if (!bIsVisible && b_is_visible)
			bNewlyVisible = true;
		bIsVisible = b_is_visible;
		++iAge;
	}

	//*****************************************************************************************
	//
	void UpdateFrameKey
	(
	)
	//
	// Updates the frame key for the cache.
	//
	//**************************************
	{
		iFrameKey  = iFrameKeyGlobal;
	}

	//*****************************************************************************************
	//
	bool bIsCurrentFrameKey
	(
	) const
	//
	// Returns 'true' if the frame key for the cache is for the current frame.
	//
	//**************************************
	{
		return iFrameKey == iFrameKeyGlobal;
	}

	//*****************************************************************************************
	//
	bool bIsOldFrameKey
	(
	) const
	//
	// Returns 'true' if the cache is old.
	//
	// Notes:
	//		The cache is old if it was not visible the last frame, indicated by whether or
	//		not is has a recent frame key.
	//
	//**************************************
	{
		return iFrameKey < iFrameKeyGlobal - 1;
	}

	//*****************************************************************************************
	//
	bool bEvaluate
	(
	) const
	//
	// Returns 'true' if the frame key for the cache is for the current frame.
	//
	//**************************************
	{
		return (iCacheID & 0x07) == (iFrameKeyGlobal & 0x07);
	}

	//*****************************************************************************************
	//
	bool bRender
	(
	) const
	//
	// Returns 'true' if the cache is renderable.
	//
	//**************************************
	{
		return bIsVisible && pmshPolygon;
	}

	//*****************************************************************************************
	//
	// Static member functions.
	//

	//*****************************************************************************************
	//
	static void SetTextureMemSize
	(
	);
	//
	// Sets the amount of memory that can be used for image caches automatically.
	//
	//**************************************

	//*****************************************************************************************
	//
	static void SetTextureMemSize
	(
		int i_sys_mem_kb,
		int i_vid_mem_kb
	);
	//
	// Sets the amount of memory that can be used for image caches.
	//
	//**************************************

	//*****************************************************************************************
	//
	static int iGetTotalMem
	(
	)
	//
	// Returns the amount of bytes of memory used for image caches.
	//
	//**************************************
	{
		return iTotalSysBytes + iTotalD3DBytes;
	}

	//*****************************************************************************************
	//
	static int iGetHardwareMem
	(
	)
	//
	// Returns the amount of bytes of memory in hardware used for image caches.
	//
	//**************************************
	{
		return iTotalD3DBytes;
	}

	//*****************************************************************************************
	//
	static int iGetLimitMem
	(
	)
	//
	// Returns the amount of bytes of memory that can be used for image caches.
	//
	//**************************************
	{
		return iLimitBytes;
	}

	//*****************************************************************************************
	//
	static int iGetHardwareLimit
	(
	);
	//
	// Returns the amount of bytes of memory in hardware that can be used for image caches.
	//
	//**************************************

	//*****************************************************************************************
	//
	static void SetParameters
	(
	);
	//
	// Sets the operating parameters for the render cache system based on the amount of memory
	// available to it and the general speed and quality settings.
	//
	//**************************************

protected:

	//*****************************************************************************************
	void CreateBuffers(const CVector2<>& v2_coord_min, const CVector2<>& v2_coord_max,
		               const CCamera& cam);

	//*****************************************************************************************
	void UpdateNewCameraSettings(const CCamera& cam, CCamera& cam_new);
	
	//*****************************************************************************************
	TReal rGetScaleToScreen(const CCamera& cam);

	//*****************************************************************************************
	TReal rGetBufferWidth(const CVector2<>& v2_coord_min, const CVector2<>& v2_coord_max,
		                  const CCamera& cam);

	//*****************************************************************************************
	TReal rGetBufferHeight(const CVector2<>& v2_coord_min, const CVector2<>& v2_coord_max,
		                   TReal r_width);

	//*****************************************************************************************
	void SetupMesh();

	//*********************************************************************************************
	void UpdateMeshAndCamera(CPresence3<>& pr3_cam_new, CCamera& cam_new, CCamera& cam,
		                     CVector3<>& rv3_coord_min, CVector3<>& rv3_coord_max);

	//*****************************************************************************************
	void DefineMesh(CPresence3<>& pr3_cam_new, const CVector3<>& v3_min,
		            const CVector3<>& v3_max, const CPArray< CVector2<> >& pav2);

	//*****************************************************************************************
	void DefineRectMesh(CPresence3<>& pr3_cam_new, const CVector3<>& v3_min,
		                const CVector3<>& v3_max);

	//*****************************************************************************************
	void CreateDistortionCoords(const CCamera& cam);

	//*****************************************************************************************
	void SetControlPoints(const CCamera& cam_new);

	//*****************************************************************************************
	//
	void GetMemory
	(
	);
	//
	// Ensures the memory allocation for textures.
	//
	//**************************************

private:

	CRenderCache(CPartition* pins_shape, const CCamera& cam);
	~CRenderCache();

	class CPriv;
	friend class CPriv;
	friend class CRenderCacheList;
	friend class CCacheResolution;

	friend void CPartition::CreateRenderCache(const CCamera& cam);
	friend void CPartition::DeleteRenderCache();

};


//*********************************************************************************************
//
class CRenderCacheList
//
// Object maintains the list of render cached objects, and kills old ones.
//
// Prefix: rencl
//
// Notes:
//
//**************************************
{
public:

	typedef set<CPartition*, less<CPartition*> > TCacheSet;
	TCacheSet partlistCached;

public:

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	// Default constructor.
	CRenderCacheList
	(
	);

	// Destructor.
	~CRenderCacheList
	(
	);

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void IncrementCount
	(
	);
	//
	// Increments the age count of all caches.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddToLRU
	(
	);
	//
	// Adds currently unused caches to the LRU.
	//
	//**************************************

	//*****************************************************************************************
	//
	void operator +=
	(
		CPartition* ppart
	);
	//
	// Add a partition to this list.
	//
	//**************************************

	//*****************************************************************************************
	//
	void operator -=
	(
		CPartition* ppart
	);
	//
	// Remove a partition from this list.
	//
	//**************************************

	//*****************************************************************************************
	//
	int iGetCacheMemoryUsed
	(
	);
	//
	// Returns the number of bytes associated with the render caches in the list.
	//
	//**************************************

	//*****************************************************************************************
	//
	int iGetNumCaches
	(
	);
	//
	// Returns the number of caches in the list.
	//
	//**************************************

	//*****************************************************************************************
	//
	void RemoveAll
	(
	);
	//
	// Removes all caches in the list.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Dump
	(
	);
	//
	// Prints out the dimensions of all the caches currently in use.
	//
	//**************************************

};


//*********************************************************************************************
//
class CScheduleCache : public CSchedulerItem
//
// Element of the cache schedule list.
//
// Prefix: schc
//
//**************************************
{
private:

	CPartition*        ppartPart;
	CRenderContext&    rencRenc;
	CPArray<COcclude*> papocIntersect;

public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	// Initializing constructor.
	CScheduleCache
	(
		CScheduler&               sch,
		CPartition*               ppart,
		CRenderContext&           renc,
		const CPArray<COcclude*>& papoc_intersect
	);

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	virtual void Execute();

	//*****************************************************************************************
	virtual void PostScheduleExecute();

	//*****************************************************************************************
	virtual void LogItem(CConsoleBuffer& con) const;

};


//
// Global functions.
//

//*********************************************************************************************
//
void PurgeParentCaches
(
	CPartition* ppart	// Partition to purge caches from.
);
//
// Removes all render caches associated with the supplied partition or any of its parents.
//
//**************************************

//*********************************************************************************************
//
void PurgeRenderCaches
(
);
//
// Removes all caches in the database.
//
//**************************************

//*********************************************************************************************
//
void UploadCaches
(
);
//
// Uploads all 'dirty' render cache textures.
//
//**************************************


//
// Global variables.
//
extern CRenderCacheList renclRenderCacheList;

#endif