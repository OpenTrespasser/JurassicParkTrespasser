/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		A helper library for render caching.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/RenderCacheHelp.hpp                                      $
 * 
 * 16    9/01/98 6:29p Mmouni
 * Moved DumpMoveStopLists() function declaration.
 * 
 * 15    5/05/98 1:34p Pkeet
 * Altered includes to include the render cache private header file.
 * 
 * 14    4/22/98 4:51p Pkeet
 * Removed the 'bPLANE_FRONT' macro switch.
 * 
 * 13    2/13/98 6:58p Pkeet
 * Added the 'uBuildPartitionList' function.
 * 
 * 12    2/13/98 5:57p Pkeet
 * Altered the 'GetBoundingCoords' to accept a bool argument that is 'true' if the number of
 * points in the point cloud is below the maximum allowed.
 * 
 * 11    12/21/97 11:02p Rwyatt
 * Changed references to CMArrays to CDArrays
 * 
 * 10    97/10/28 3:35p Pkeet
 * Removed unnecessary and unused render cache code.
 * 
 * 9     97/10/24 6:59p Pkeet
 * Added STL lists to represent moving cache objects.
 * 
 * 8     97/10/17 2:18p Pkeet
 * Changed function to work on minimum projected Z (in normalized camera space).
 * 
 * 7     97/10/17 11:42a Pkeet
 * Added code to find the closest point to the camera.
 * 
 * 6     97/08/04 6:19p Pkeet
 * Removed the camera parameter from the 'GetBoundingCoords' function. Added the
 * 'v3GetCameraPos' function.
 * 
 * 5     97/07/15 1:03p Pkeet
 * Changed 'GetMinMaxCameraCoords' to 'GetBoundingCoords.'
 * 
 * 4     97/06/27 1:41p Pkeet
 * Added support for setting near and far clipping planes for a cached partition.
 * 
 * 3     97/03/21 1:59p Pkeet
 * Changed a 'GetMinMaxCameraCoords' parameter.
 * 
 * 2     97/03/19 12:24p Pkeet
 * Moved the 'SetCameraProperties' and 'DrawBorder' functions to the helper module.
 * 
 * 1     97/03/18 5:41p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_RENDERCACHEHELP_HPP
#define HEADER_LIB_RENDERER_RENDERCACHEHELP_HPP


//
// Includes.
//
#include "list.h"
#include "Lib/Transform/Vector.hpp"
#include "Lib/Transform/Rotate.hpp"
#include "Lib/Sys/FastHeap.hpp"


//
// Defines and constants.
//
const float fFUDGE = 1.01;
const int	iMIN_RASTER_SIZE = 4;


//
// Forward declarations.
//
class CPartition;
class CCamera;
class CRaster;


//
// Typedefs.
//

// Type used to maintain a list of partition pointers.
typedef list<CPartition*> TPPartitionList;


//
// Class definitions.
//

//*********************************************************************************************
//
class CClosestRenderPoint
//
// Object contains information about the closest point found to the render cache.
//
// Prefix: crp
//
//**************************************
{
public:

	CVector3<> v3ClosestMeshPt;
	TReal      rMinProjectedZ;
	bool       bFoundFirst;

public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	// Default constructor.
	CClosestRenderPoint();

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void Try
	(
		const CVector3<>& v3_mesh,
		TReal             r_min_projected_z
	);
	//
	// Tests if the point is the closest point to the camera and store the new mesh point and
	// normalized camera space y value if it is.
	//
	//**************************************

};

//
// Global functions.
//

//*********************************************************************************************
//
void SetCameraProperties
(
	CCamera&      cam_new,			// Local camera.
	rptr<CRaster> pras,				// Screen for camera.
	TReal         r_distance,		// Projection distance for points.
	CVector3<>&   rv3_coord_min,	// Maximum local coordinates.
	CVector3<>&   rv3_coord_max		// Maximum local coordinates.
);
//
// Sets up a new camera given a screen and local screen coordinates.
//
//**************************************

//*********************************************************************************************
//
uint uBuildPartitionList
(
	TPPartitionList&  rpartlist,	// Resulting list of partitions.
	CPartition*       ppart,		// Root partition.
	const CVector3<>& v3_pos		// Viewer (camera) position.
);
//
// Builds a list of visible partitions.
//
// Returns the number of points in the point cloud.
//
//**************************************

//*********************************************************************************************
//
void GetBoundingCoords
(
	CPartition*            ppart,			// Partition to find extents of.
	const CTransform3<>&   tf3_cam,			// Partial camera transform.
	TReal                  r_distance,		// Projection distance for points.
	CDArray< CVector2<> >& av2_convex,		// Points that make up the convex shell.
	CVector3<>&            rv3_min,			// Minimum z extent.
	CVector3<>&            rv3_max,			// Maximum z extent.
	CClosestRenderPoint&   rcrp,			// Closest y point.
	bool                   b_use_box,		// Use the bounding box only.
	bool                   b_add_points		// Add points to 'av2_convex.'
);
//
// Adds points defining the bounding coordinates of the partition to the convex point array.
//
// Stores the minimum and maximum x, y and z values.
//
// Returns 'true' if the number of bounding coordinates is in range.
//
//**************************************

//*********************************************************************************************
//
void FudgeRange
(
	CVector3<>& rv3_min,		// Minimum value.
	CVector3<>& rv3_max,		// Maximum value.
	TReal r_expand = TReal(0.5)	// Value to expand range by.
);
//
// Increase these coords by a fudge factor to ensure that the entire image will render inside
// the raster without clipping or overrun.
//
//**************************************

//*********************************************************************************************
//
CVector3<> v3GetCameraPos
(
	const CCamera& cam	// Camera for projection.
);
//
// Returns the current or predicted position of the camera.
//
//**************************************


//
// Global variables.
//
extern TPPartitionList ppartlistMoved;		// List of moving partitions.
extern TPPartitionList ppartlistStopped;	// List of stopped partitions.


#endif // HEADER_LIB_RENDERER_RENDERCACHEHELP_HPP
