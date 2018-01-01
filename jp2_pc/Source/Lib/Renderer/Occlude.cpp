/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Implementation of Occlude.hpp.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Occlude.cpp                                              $
 * 
 * 33    9/23/98 3:12p Pkeet
 * Added a stat for occluding partitions.
 * 
 * 32    9/23/98 2:29p Pkeet
 * Fixed member functions declared inline.
 * 
 * 31    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 30    9/09/98 3:08p Pkeet
 * Made occlusion more efficient for shadows.
 * 
 * 29    9/09/98 1:54p Pkeet
 * 
 * 28    8/28/98 3:59p Mmouni
 * Added support for occlusion bias to K6-3D assembly.
 * 
 * 27    8/26/98 12:25a Pkeet
 * Added a slight bias value to occlusion planes.
 * 
 * 26    8/14/98 8:47p Mmouni
 * Fixed occlusion error.
 * 
 * 25    5/14/98 7:29p Pkeet
 * Added code so that occlusion objects are only added to the occlusion list if their 'bVisible'
 * flag is set.
 * 
 * 24    4/22/98 4:52p Pkeet
 * Changed partition flag access to use the new access function.
 * 
 * 23    98.02.05 4:50p Mmouni
 * Added #pragma to disable return value warning.
 * 
 * 22    98.01.22 3:09p Mmouni
 * Moved body of bInsideNormPlanes() function into out of the header.
 * 
 * 21    1/20/98 5:57p Pkeet
 * Fixed bug where stats were not being reported correctly.
 * 
 * 20    12/29/97 9:16p Gfavor
 * Commented out unused invocation of GetMinMax in bOccludePolygon.
 * 
 * 19    97/11/19 18:13 Speter
 * Use CMesh::SPolygon::v3GetCentre() rather than defunct v3Centre var.
 * 
 * 18    97/10/10 4:05p Pkeet
 * Added a switch to occlude in normalized camera space only.
 * 
 * 17    97/10/10 3:25p Pkeet
 * Added code to do partition occlusion tests in normalized camera space.
 * 
 * 16    97/10/10 2:24p Pkeet
 * Added a partition occlusion test the uses a box transform in place of a bounding box and a
 * presence.
 * 
 * 15    97/10/10 10:52a Pkeet
 * Disabled the normalized camera space extents test because it should use projected points.
 * 
 * 14    97/10/09 10:46a Pkeet
 * Added a data members and a function to test the normalized camera space extents for a polygon
 * first.
 * 
 * 13    97/10/08 7:23p Pkeet
 * Added the 'bInCameraView' member function. Added the ' bTestCameraView' flag.
 * 
 * 12    97/10/08 3:13p Pkeet
 * Added the 'bRemoveOccluded' and 'iMaxNumOccludeObjects' static members. Added code to prevent
 * too many occlusion objects from being added to the list. Added code to remove occlusion
 * objects occluded by other occlusion objects.
 * 
 * 11    97/10/08 2:14p Pkeet
 * Added a world area threshold.
 * 
 * 10    97/10/08 11:31a Pkeet
 * Added the camera's inverse presence and a pointer to its bounding volume as parameters to the
 * 'GetOccludePolygons' function. Made the 'GetOccludePolygons' function accept occlusion
 * objects from partitions that are in camera space only.
 * 
 * 9     97/10/08 10:49a Pkeet
 * Added counts for occluding objects. Added comments to the stats class.
 * 
 * 8     97/10/07 6:28p Pkeet
 * Fixed stats to work with the correct console window.
 * 
 * 7     97/10/07 5:36p Pkeet
 * Added the 'CConOcclude' object.
 * 
 * 6     97/10/07 2:50p Pkeet
 * Added code to move the occluding polyhedron in regular camera space in slightly based on a
 * switch.
 * 
 * 5     97/10/07 1:41p Pkeet
 * Added an array of planes representing the occluding polyhedron in normalized camera space.
 * Added a function to test for inclusion of these points. Added a polygon occlusion function.
 * 
 * 4     97/10/06 4:55p Pkeet
 * Added the 'bOccludePartition' function. Changed function calls for occlusion to use an array
 * of pointers to occlusion objects instead of an array of occlusion objects. Set the default
 * occlusion flags to 'true.'
 * 
 * 3     97/10/03 7:12p Pkeet
 * Added a fast heap static member, and enabled the construction of point and plane lists and a
 * polyhedral bounding volume in camera space.
 * 
 * 2     97/10/02 5:10p Pkeet
 * Added flags and a constructor for 'COcclude' to toggle occlusion on and off. Added the
 * 'CopyOccludePolygons' global function.
 * 
 * 1     97/10/02 1:47p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Algo.h"
#include "Lib/Transform/VectorRange.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Occlude.hpp"


//
// Defines.
//

// Switch to move the camera (in regular camera space) occluding polygon in slightly.
#define bMOVE_REGULAR_CAM_OCCLUSION_OUT (0)


//
// Constants.
//

#if bBIAS_OCCLUSION

float fLocalBias;
bool  bUseBias = true;

#endif // bBIAS_OCCLUSION



//
// Module specific variables.
//

//
// Flag for using world area calculated through normalized camera space for culling occlusion
// polygons with a small screen area.
//
bool bCullSmallPolygons = true;

// Stats for renderering.
static CProfileStat psOcclusionPolygon("Polygon", &proProfile.psOcclusion);
static CProfileStat psOcclusionParition("Parition", &proProfile.psOcclusion);


//
// Function prototypes.
//

//**********************************************************************************************
void Print(CConsoleBuffer& rcon, const CVector3<>& v3);


//
// Internal class definitions.
//

//******************************************************************************************
//
class CSortOcclude
//
// Less than comparison class for COcclude pointers.
//
//**************************************
{
public:

	//**************************************************************************************
	//
	bool operator()(const COcclude* poc_0, const COcclude* poc_1) const
	//
	// Returns 'true' if the farthest (smallest) inverse Z value of the first polygon is
	// closer (larger) than the farthest inverse z value of the second polygon.
	//
	//**************************************
	{
		Assert(poc_0);
		Assert(poc_1);
		Assert(poc_0->rWorldArea > 0.0f);
		Assert(poc_1->rWorldArea > 0.0f);

		// Return the results of the comparision.
		return u4FromFloat(poc_1->rWorldArea) < u4FromFloat(poc_0->rWorldArea);
	}
};


//
// Class implementations.
//

//**********************************************************************************************
//
// COcclude implementation.
//

	//******************************************************************************************
	//
	// Constructors and destructors.
	//

	//*****************************************************************************************
	COcclude::COcclude()
		: bFacing(false), ppolyPolgyon(0), ppartPartition(0)
	{
	}

	//*****************************************************************************************
	COcclude::COcclude(CMesh::SPolygon* ppoly, CPartition* ppart)
		: bFacing(false), ppolyPolgyon(ppoly), ppartPartition(ppart)
	{
		Assert(ppoly);
		Assert(ppart);
	}

	//*****************************************************************************************
	COcclude::COcclude(const COcclude& oc)
		: ppolyPolgyon(oc.ppolyPolgyon), ppartPartition(oc.ppartPartition)
	{
		Assert(ppolyPolgyon);
		Assert(ppartPartition);
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	bool COcclude::bInitialize(const CPresence3<>& pr3_cam_inv, const CTransform3<>& tf3)
	{
		Assert(ppolyPolgyon);
		Assert(ppartPartition);

		// Get the world transform.
		CPresence3<> pr3_world = ppartPartition->pr3Presence();
		CPresence3<> pr3 = pr3_world * pr3_cam_inv;

		// Set the centre point of the occluding object.
		v3Centre = ppolyPolgyon->v3GetCentre() * pr3;

		// Create a plane for the transformed polygon.
		CPlane pl = ppolyPolgyon->plPlane;
		pl *= pr3;

		TReal r_distance = Abs(pl.rDistance(CVector3<>(0.0f, 0.0f, 0.0f)));

		// Reject if the polygon is too close to the camera.
		if (r_distance < 0.005f)
			return false;

		// Create a normal pointing from the camera's origin to the centre of the polygon.
		CDir3<> d3_normal(v3Centre);

		// Get the dot product of the two normals.
		TReal r_dot = d3_normal * pl.d3Normal;

		// Set the direction of the plane.
		bFacing = r_dot < 0.0f;

		// Get the world area of the polygon as it faces the camera.
		TReal r_scale = pr3_world.rScale;
		rWorldArea = ppolyPolgyon->rWorldArea * Abs(r_dot) * r_scale * r_scale / r_distance;

		if (bCullSmallPolygons)
		{
			// If the world area falls below the threshold, reject the polygon.
			if (rWorldArea < rWorldAreaThreshold)
				return false;
		}

		//
		// To do:
		//		Test against the camera's volume.
		//

		// Construct the partition to normalized camera space transform.
		CTransform3<> tf3_part_camera = pr3_world * tf3;

		#if _DEBUG
			// Set the centre point of the occluding object in normalized camera space.
			v3CentreNorm = ppolyPolgyon->v3GetCentre() * tf3_part_camera;
		#endif // _DEBUG

		// Construct a list of transformed points.
		if (bFacing)
		{
		#if !bUSE_NORMSPACE_ONLY
			TransformPoints(0, ppolyPolgyon->iNumVertices(), 1, pr3);
		#endif // bUSE_NORMSPACE_ONLY
			TransformPointsNorm(0, ppolyPolgyon->iNumVertices(), 1, tf3_part_camera);
		}
		else
		{
		#if !bUSE_NORMSPACE_ONLY
			TransformPoints(int(ppolyPolgyon->papmvVertices.uLen) - 1, -1, -1, pr3);
		#endif // bUSE_NORMSPACE_ONLY
			TransformPointsNorm(int(ppolyPolgyon->papmvVertices.uLen) - 1, -1, -1, tf3_part_camera);
		}

		// Test if the occluding object is in the camera's view volume.
		if (bTestCameraView)
		{
			if (!bInCameraView())
				return false;
		}

		// Set the minimum and maximum values for points in normalized camera space.
		//GetMinMax(pav3PointsNorm, v3MinNorm, v3MaxNorm);

	#if !bUSE_NORMSPACE_ONLY
		// Build a list of planes for the polyhedron.
		BuildPlaneList(pl);
	#endif // bUSE_NORMSPACE_ONLY

		// Build a list of planes for the polyhedron in normalized camera space.
		BuildPlaneListNorm();

		//
		// Create the polyhedron.
		//
		// Notes:
		//		Using this constructor with these points assumes that the bounding sphere
		//		contained in the bounding polyhedron is never used. Tests could break horribly
		//		if the bounding sphere is implemented as a quick test for the polyhedron.
		//
	#if !bUSE_NORMSPACE_ONLY
		pbvpPolyhedron     = new(fhMemory) CBoundVolPolyhedron(paplPlanes, pav3Points);
	#endif // bUSE_NORMSPACE_ONLY
		pbvpPolyhedronNorm = new(fhMemory) CBoundVolPolyhedron(paplPlanesNorm, pav3PointsNorm);

		// Test everything.
		AssertValid();

		// Indicate success.
		return true;
	}

#if !bUSE_NORMSPACE_ONLY

	//******************************************************************************************
	//
	void COcclude::TransformPoints
	(
		int i_start,
		int i_end,
		int i_increment,
		const CPresence3<>& pr3
	)
	//
	// Creates a list of transformed points for the occluding polygon in 'pav3Points.'
	//
	//**********************************
	{
		pav3Points.uLen = ppolyPolgyon->papmvVertices.uLen;
		pav3Points.atArray = new(fhMemory) CVector3<>[pav3Points.uLen];

		// Add and transform the first point.
		int i_dest = 0;
		for (int i_source = i_start; i_source != i_end; i_source += i_increment, ++i_dest)
		{
			pav3Points[i_dest] = ppolyPolgyon->v3Point(i_source) * pr3;

			#if bMOVE_REGULAR_CAM_OCCLUSION_OUT
				// Provide some extra tolerance.
				TReal r_adjust_y = pav3Points[i_dest].tY * 0.001;
				r_adjust_y = Min(r_adjust_y, 0.0001f);
				r_adjust_y = Max(r_adjust_y, 1.0000f);
				pav3Points[i_dest].tY += r_adjust_y;
			#endif // bMOVE_REGULAR_CAM_OCCLUSION_OUT
		}
		Assert(uint(i_dest) == pav3Points.uLen);
	}

	//******************************************************************************************
	//
	void COcclude::BuildPlaneList
	(
		const CPlane& pl_first
	)
	//
	// Creates the list of bounding planes for the occluding polyhedron in 'paplPlanes.'
	//
	//**********************************
	{
		// Set the size of the array.
		paplPlanes.uLen = pav3Points.uLen + 1;
		paplPlanes.atArray = new(fhMemory) CPlane[paplPlanes.uLen];

		// Add the first plane.
		paplPlanes[0] = CPlane(pav3Points[0], pav3Points[1], pav3Points[2]);

		// Create and add subsequent planes.
		for (uint u = 0; u < pav3Points.uLen - 1; ++u)
		{
			paplPlanes[u + 1] = CPlane
			(
				CVector3<>(0.0f, 0.0f, 0.0f),
				pav3Points[u],
				pav3Points[u + 1]
			);
		}

		// Add the last plane.
		paplPlanes[paplPlanes.uLen - 1] = CPlane
		(
			CVector3<>(0.0f, 0.0f, 0.0f),
			pav3Points[pav3Points.uLen - 1],
			pav3Points[0]
		);
	}

#endif // bUSE_NORMSPACE_ONLY

	//******************************************************************************************
	//
	void COcclude::BuildPlaneListNorm
	(
	)
	//
	// Creates the list of bounding planes for the occluding polyhedron in 'paplPlanes.'
	//
	//**********************************
	{
		// Set the size of the array.
		paplPlanesNorm.uLen = pav3PointsNorm.uLen + 1;
		paplPlanesNorm.atArray = new(fhMemory) CPlane[paplPlanesNorm.uLen];

		// Add the first plane.
		paplPlanesNorm[0] = CPlane(pav3PointsNorm[0], pav3PointsNorm[1], pav3PointsNorm[2]);

		// Create and add subsequent planes.
		for (uint u = 0; u < pav3PointsNorm.uLen - 1; ++u)
		{
			paplPlanesNorm[u + 1] = CPlane
			(
				CVector3<>(0.0f, 0.0f, 0.0f),
				pav3PointsNorm[u],
				pav3PointsNorm[u + 1]
			);
		}

		// Add the last plane.
		paplPlanesNorm[paplPlanesNorm.uLen - 1] = CPlane
		(
			CVector3<>(0.0f, 0.0f, 0.0f),
			pav3PointsNorm[pav3PointsNorm.uLen - 1],
			pav3PointsNorm[0]
		);
	}

	//******************************************************************************************
	//
	void COcclude::TransformPointsNorm
	(
		int i_start,
		int i_end,
		int i_increment,
		const CTransform3<>& tf3
	)
	//
	// Creates a list of transformed points for the occluding polygon in 'pav3Points.'
	//
	//**********************************
	{
		pav3PointsNorm.uLen = ppolyPolgyon->papmvVertices.uLen;
		pav3PointsNorm.atArray = new(fhMemory) CVector3<>[pav3PointsNorm.uLen];

		// Add and transform the first point.
		int i_dest = 0;
		for (int i_source = i_start; i_source != i_end; i_source += i_increment, ++i_dest)
		{
			pav3PointsNorm[i_dest] = ppolyPolgyon->v3Point(i_source) * tf3;
			pav3PointsNorm[i_dest].tY += 0.0001f;
		}
		Assert(uint(i_dest) == pav3PointsNorm.uLen);
	}

	//******************************************************************************************
	//
	bool COcclude::bInCameraView
	(
	) const
	//
	// Returns 'true' if the occlusion object is in the camera's view.
	//
	// Notes:
	//		Requires that the 'pav3PointsNorm' array has been filled with values.
	//
	//**********************************
	{
		Assert(pav3PointsNorm.uLen >= 3);
		Assert(pav3PointsNorm.atArray);

		uint u;	// Point index.

		//
		// Test to see if all the points are on one side of any plane.
		//

		// Top camera plane.
		for (u = 0; u < pav3PointsNorm.uLen; ++u)
			if (pav3PointsNorm[u].tZ < 1.0f)
				break;
		if (u == pav3PointsNorm.uLen)
			return false;

		// Bottom camera plane.
		for (u = 0; u < pav3PointsNorm.uLen; ++u)
			if (pav3PointsNorm[u].tZ > -1.0f)
				break;
		if (u == pav3PointsNorm.uLen)
			return false;

		// Left camera plane.
		for (u = 0; u < pav3PointsNorm.uLen; ++u)
			if (pav3PointsNorm[u].tX > -1.0f)
				break;
		if (u == pav3PointsNorm.uLen)
			return false;

		// Right camera plane.
		for (u = 0; u < pav3PointsNorm.uLen; ++u)
			if (pav3PointsNorm[u].tX < 1.0f)
				break;
		if (u == pav3PointsNorm.uLen)
			return false;

		// Far clipping plane.
		for (u = 0; u < pav3PointsNorm.uLen; ++u)
			if (pav3PointsNorm[u].tY < 1.0f)
				break;
		if (u == pav3PointsNorm.uLen)
			return false;

		// Near clipping plane.
		for (u = 0; u < pav3PointsNorm.uLen; ++u)
			if (pav3PointsNorm[u].tY > 0.0f)
				break;
		if (u == pav3PointsNorm.uLen)
			return false;

		// Test succeeded.
		return true;
	}
	
	//******************************************************************************************
	void COcclude::AssertValid() const
	{
		#if (!_DEBUG)
			return;
		#endif // _DEBUG

		Assert(ppartPartition);
		Assert(ppolyPolgyon);

		Assert(pbvpPolyhedronNorm);
		Assert(pav3PointsNorm.uLen >= 3);
		Assert(pav3PointsNorm.atArray);
		Assert(paplPlanesNorm.uLen >= 3);
		Assert(paplPlanesNorm.atArray);

	#if !bUSE_NORMSPACE_ONLY

		uint u;	// Iterator value.

		Assert(pbvpPolyhedron);
		Assert(pav3Points.uLen >= 3);
		Assert(pav3Points.atArray);
		Assert(paplPlanes.uLen >= 3);
		Assert(paplPlanes.atArray);

		// Camera space.
		Assert(paplPlanes[0].esfSideOf(v3Centre * 0.5f) == esfOUTSIDE);

		#if (!bMOVE_REGULAR_CAM_OCCLUSION_OUT)
			for (u = 0; u < paplPlanes.uLen; ++u)
			{
				Assert(paplPlanes[u].esfSideOf(v3Centre * 2.0f) == esfINSIDE);
			}
		#endif // !bMOVE_REGULAR_CAM_OCCLUSION_OUT

		//
		// Test that a point half-way between the origin and the centre of the occluding
		// object is not contained within the polyhedron.
		//
		Assert(pbvpPolyhedron->esfSideOf(v3Centre * 0.5f) == esfOUTSIDE);

		#if (!bMOVE_REGULAR_CAM_OCCLUSION_OUT)
			//
			// Make sure that a point projected from the origin through the centre of the
			// occluding object is.
			//
			Assert(pbvpPolyhedron->esfSideOf(v3Centre * 2.0f) == esfINSIDE);
		#endif // !bMOVE_REGULAR_CAM_OCCLUSION_OUT

	#endif // bUSE_NORMSPACE_ONLY
	}

	//******************************************************************************************
	//
	void COcclude::PrintPoints
	(
	) const
	//
	// Prints occluding polygon points to a text file 'OccludePoints.txt.'
	//
	//**********************************
	{
		// Print stuff out.
		CConsoleBuffer con;

		con.OpenFileSession("OccludePoints.txt");
		con.Print("Centre: ");
		Print(con, v3Centre);
		con.Print("        ");
		Print(con, ppolyPolgyon->v3GetCentre());
		con.Print("\n");

	#if !bUSE_NORMSPACE_ONLY
		for (uint u = 0; u < pav3Points.uLen; ++u)
		{
			CVector3<> v3 = ppolyPolgyon->v3Point(u);
			con.Print("Point: %ld: ", u);
			Print(con, pav3Points[u]);
			con.Print("          ");
			Print(con, v3);
			con.Print("\n");
		}
	#endif // bUSE_NORMSPACE_ONLY

		con.CloseFileSession();
	}


//**********************************************************************************************
//
// CConOcclude implementation.
//

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	// Default constructor.
	CConOcclude::CConOcclude()
	{
		Reset();
	}

	// Destructor.
	CConOcclude::~CConOcclude()
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	bool CConOcclude::bIsActive() const
	{
		return conOcclusion.bIsActive();
	}

	//******************************************************************************************
	void CConOcclude::SetNumOcclusionObjectsInCamera(int i_num)
	{
		iNumOcclusionObjectsInCamera = i_num;
	}

	//******************************************************************************************
	void CConOcclude::SetNumOcclusionObjectsUsed(int i_num)
	{
		iNumOcclusionObjectsUsed = i_num;
	}

	//******************************************************************************************
	void CConOcclude::Reset()
	{
		iPolysOccluded = 0;
		iPolysVisible  = 0;
		iNumOcclusionObjectsInCamera = 0;
		iNumOcclusionObjectsUsed = 0;
		iPartitionsOccluded = 0;
	}

	//******************************************************************************************
	void CConOcclude::Show()
	{
		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		conOcclusion.Print("Num occlusion objects:      %ld\n", iNumOcclusionObjectsInCamera);
		conOcclusion.Print("Num occlusion objects used: %ld\n", iNumOcclusionObjectsUsed);
		conOcclusion.Print("Num partitions occluded:    %ld\n", iPartitionsOccluded);
		conOcclusion.Print("Num polys occluded:         %ld\n", iPolysOccluded);
		conOcclusion.Print("Num polys visible:          %ld\n", iPolysVisible);

		// Display and clear.
		conOcclusion.Show();
		conOcclusion.ClearScreen();
		Reset();
	}



//*********************************************************************************************
//
// Global functions.
//

//*********************************************************************************************
void GetOccludePolygons(CPartition* ppart, const CPresence3<>& pr3_inv_cam,
						const CBoundVol* pbv_cam, TOccludeList& roclist, ESideOf esf_view)
{
	Assert(ppart);

	// Do nothing if this partition does not contain occluding polygons.
	if (!ppart->pdGetData().bOcclude)
		return;

	//
	// Test if the partition is in the camera's bounding volume and only continue if it is.
	//
	if (esf_view != esfINSIDE)
	{
		// Get a pointer to the bounding volume of the partition.
		const CBoundVol* pbv_it = ppart->pbvBoundingVol();

		// If there is no bounding volume, assume intersection.
		if (pbv_it)
		{
			// Construct the partition-to-camera transform.
			CPresence3<> pr3_it_cam = ppart->pr3Presence() * pr3_inv_cam;

			// Intersect bounding volumes.
			esf_view = pbv_cam->esfSideOf
			(
				*pbv_it,	// Target partition bounding volume
				0,			// Camera effectively has null presence.
				&pr3_it_cam	// Partition's presence transforms to camera space.
			);

			// Return if no part of this partition is within the camera view volume.
			if (esf_view == esfOUTSIDE)
				return;
		}
		else
			esf_view = esfINTERSECT;
	}

	// Add occluding polygons from this mesh.
	{
		rptr<CMesh> pmsh = ppart->pmshGetMesh();	// Possible mesh.

		// If the partition contains a mesh and is visible, look for occluding polygons.
		if (pmsh)
		{
			for (uint u = 0; u < pmsh->papmpOcclude.uLen; ++u)
			{
				if (ppart->bIsVisible())
					roclist.push_back(COcclude(pmsh->papmpOcclude[u], ppart));
			}
		}
	}

	// Add occluding polygons from child meshes.
	{
		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppart->ppartChildren();

		// Iterate through children.
		if (ppartc)
		{
			for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			{
				GetOccludePolygons(*it, pr3_inv_cam, pbv_cam, roclist, esf_view);
			}
		}
	}
}

//*********************************************************************************************
void CopyOccludePolygons(CPArray<COcclude*>& rpapoc, const CCamera& cam, TOccludeList& roclist,
						 bool b_stats, bool b_cull)
{
	bool b_cull_small = bCullSmallPolygons;
	bCullSmallPolygons = b_cull;
	bUseBias = false;

	CPresence3<>  pr3_cam_inv = ~cam.pr3Presence();
	CTransform3<> tf3 = cam.tf3ToNormalisedCamera();

	int i = 0;
	TOccludeList::iterator it = roclist.begin();
	for (; it != roclist.end(); ++it)
	{
		if ((*it).bInitialize(pr3_cam_inv, tf3))
		{
			rpapoc[i] = &(*it);
			++i;
		}
	}
	rpapoc.uLen = uint(i);

	// Sort occlusion object by size from the largest to the smallest.
	sort(rpapoc.atArray, rpapoc.atArray + rpapoc.uLen, CSortOcclude());

	// Cap the number of occluding objects that can be considered.
	rpapoc.uLen = Min(rpapoc.uLen, uint(COcclude::iMaxNumOccludeObjects));

	// Remove occluded occluding objects.
	if (COcclude::bRemoveOccluded)
	{
		if (rpapoc.uLen > 1)
		{
			for (uint u_small = rpapoc.uLen - 1; u_small > 0; --u_small)
			{
				for (uint u_large = 0; u_large < u_small; ++u_large)
				{
					// If the smaller object is in the larger one, remove it.
					if (rpapoc[u_large]->bInsideNormPlanes(rpapoc[u_small]->pav3PointsNorm))
					{
						// Copy the last element to the current small element.
						rpapoc[u_small] = rpapoc[rpapoc.uLen - 1];

						// Remove the last element.
						--rpapoc.uLen;

						// Break from inner loop or from both loops.
						if (rpapoc.uLen <= 1)
							break;
						goto BREAK_FROM_INNER_LOOP;
					}
				}
			BREAK_FROM_INNER_LOOP:;
			}
		}
	}

	// Reset the cull flag.
	bCullSmallPolygons = b_cull_small;
	bUseBias = true;

	// Record stats.
	if (b_stats)
	{
		conocOcclusion.SetNumOcclusionObjectsInCamera(roclist.size());
		conocOcclusion.SetNumOcclusionObjectsUsed(rpapoc.uLen);
	}
}

//*********************************************************************************************
bool bOccludePartition(const CPArray<COcclude*>& rpapoc,
					   CPArray<COcclude*>& rpapoc_intersect_list,
                       const CPresence3<>& pr3_it_cam, const CBoundVol* pbv_it)
{
	// Start the timer.
	CCycleTimer	ctmr;

#if bUSE_NORMSPACE_ONLY

	// Just copy the occlusion pointer array.
	rpapoc_intersect_list.uLen = rpapoc.uLen;
	for (uint u = 0; u < rpapoc.uLen; ++u)
		rpapoc_intersect_list[u] = rpapoc[u];

#else // bUSE_NORMSPACE_ONLY

	// Clear the array of intersecting occluding objects.
	rpapoc_intersect_list.uLen = 0;

	// Iterate through the occlusion objects looking for occlusion or intersection.
	for (uint u = 0; u < rpapoc.uLen; ++u)
	{
		// Determine geometric relationship between the partition and the occluding object.
		ESideOf esf = rpapoc[u]->pbvpGet()->esfSideOf
		(
			*pbv_it,	// Partition's bounding volume
			0,			// Occlusion object effectively has null presence.
			&pr3_it_cam	// Partition transforms to camera space transform.
		);

		// Act on occlusion or intersection.
		switch (esf)
		{
			// Return flag indicating that the partition is occluding.
			case esfINSIDE:
				{
					// For polygon only occlusion.
					rpapoc_intersect_list.uLen = 1;
					rpapoc_intersect_list[0] = rpapoc[u];
				}

				// Set stats.
				psOcclusionParition.Add(ctmr(), 1);
				return true;

			// Record the intersecting occlusion object.
			case esfINTERSECT:
				{
					// Use the next available element in the array.
					uint u_next = rpapoc_intersect_list.uLen;
					++rpapoc_intersect_list.uLen;
					rpapoc_intersect_list[u_next] = rpapoc[u];
				}
				break;
		}
	}

#endif // bUSE_NORMSPACE_ONLY

	// Set stats.
	psOcclusionParition.Add(ctmr(), 1);

	// Return flag indicating that occlusion is not found.
	return false;
}

//*********************************************************************************************
bool bOccludePartition(const CPArray<COcclude*>& rpapoc,
					   CPArray<COcclude*>& rpapoc_intersect_list,
                       const CTransform3<>& tf3_box)
{
	// Start the timer.
	CCycleTimer	ctmr;

#if bUSE_NORMSPACE_ONLY

	// Just copy the occlusion pointer array.
	rpapoc_intersect_list.uLen = rpapoc.uLen;
	for (uint u = 0; u < rpapoc.uLen; ++u)
		rpapoc_intersect_list[u] = rpapoc[u];

#else // bUSE_NORMSPACE_ONLY

	// Clear the array of intersecting occluding objects.
	rpapoc_intersect_list.uLen = 0;

	// Iterate through the occlusion objects looking for occlusion or intersection.
	for (uint u = 0; u < rpapoc.uLen; ++u)
	{
		// Determine geometric relationship between the partition and the occluding object.
		ESideOf esf = rpapoc[u]->pbvpGet()->esfSideOf(tf3_box);

		// Act on occlusion or intersection.
		switch (esf)
		{
			// Return flag indicating that the partition is occluding.
			case esfINSIDE:
				{
					// For polygon only occlusion.
					rpapoc_intersect_list.uLen = 1;
					rpapoc_intersect_list[0] = rpapoc[u];
				}

				// Set stats.
				psOcclusionParition.Add(ctmr(), 1);
				return true;

			// Record the intersecting occlusion object.
			case esfINTERSECT:
				{
					// Use the next available element in the array.
					uint u_next = rpapoc_intersect_list.uLen;
					++rpapoc_intersect_list.uLen;
					rpapoc_intersect_list[u_next] = rpapoc[u];
				}
				break;
		}
	}

#endif // bUSE_NORMSPACE_ONLY

	// Set stats.
	psOcclusionParition.Add(ctmr(), 1);

	// Return flag indicating that occlusion is not found.
	return false;
}

//*********************************************************************************************
bool bOccludePartitionNorm(const CPArray<COcclude*>& rpapoc,
					       CPArray<COcclude*>& rpapoc_intersect_list,
                           const CTransform3<>& tf3_box)
{
	// Start the timer.
	CCycleTimer	ctmr;

	// Clear the array of intersecting occluding objects.
	rpapoc_intersect_list.uLen = 0;

	// Iterate through the occlusion objects looking for occlusion or intersection.
	for (uint u = 0; u < rpapoc.uLen; ++u)
	{
		// Determine geometric relationship between the partition and the occluding object.
		ESideOf esf = rpapoc[u]->pbvpGetNorm()->esfSideOf(tf3_box);

		// Act on occlusion or intersection.
		switch (esf)
		{
			// Return flag indicating that the partition is occluding.
			case esfINSIDE:
				{
					// For polygon only occlusion.
					rpapoc_intersect_list.uLen = 1;
					rpapoc_intersect_list[0] = rpapoc[u];
				}

				// Set stats.
				psOcclusionParition.Add(ctmr(), 1);
				return true;

			// Record the intersecting occlusion object.
			case esfINTERSECT:
				{
					// Use the next available element in the array.
					uint u_next = rpapoc_intersect_list.uLen;
					++rpapoc_intersect_list.uLen;
					rpapoc_intersect_list[u_next] = rpapoc[u];
				}
				break;
		}
	}

	// Set stats.
	psOcclusionParition.Add(ctmr(), 1);

	// Return flag indicating that occlusion is not found.
	return false;
}

//*********************************************************************************************
bool bOccludePolygon(const CPArray<COcclude*>& rpapoc,
					 const CPArray< CVector3<> >& pav3_cam_vertices)
{
	// Start the timer.
	CCycleTimer	ctmr;

	//CVector3<> v3_poly_min;	// Minimum polygon extents in normalized camera space.
	//CVector3<> v3_poly_max;	// Maximum polygon extents in normalized camera space.

	// Set the minimum and maximum values for points in normalized camera space.
	//GetMinMax(pav3_cam_vertices, v3_poly_min, v3_poly_max);
	// This is commented out until bInsideNormExtents is used below

	// Iterate through the occlusion objects looking for occlusion or intersection.
	for (uint u = 0; u < rpapoc.uLen; ++u)
	{
		//
		// If the points are entirely inside the occluding object's bounding volume,
		// the polygon is occluded.
		//

		// Not ready for this!!!
		//if (rpapoc[u]->bInsideNormExtents(v3_poly_min, v3_poly_max))
		{
			if (rpapoc[u]->bInsideNormPlanes(pav3_cam_vertices))
			{
				// Set stats.
				psOcclusionPolygon.Add(ctmr(), 1);
				conocOcclusion.AddOccludedPoly();
				return true;
			}
		}
	}

	// Set stats.
	psOcclusionPolygon.Add(ctmr(), 1);
	conocOcclusion.AddVisiblePoly();

	// Return flag indicating that occlusion is not found.
	return false;
}

//**********************************************************************************************
void Print(CConsoleBuffer& rcon, const CVector3<>& v3)
{
	rcon.Print("x: %1.2f, y: %1.2f, z: %1.2f\n", v3.tX, v3.tY, v3.tZ);
}


//
// Static variables.
//
bool      COcclude::bUseObjectOcclusion   = true;
bool      COcclude::bUsePolygonOcclusion  = true;
bool      COcclude::bRemoveOccluded       = true;
bool      COcclude::bTestCameraView       = true;
TReal     COcclude::rWorldAreaThreshold   = TReal(1.0);
int       COcclude::iMaxNumOccludeObjects = 25;
CFastHeap COcclude::fhMemory(1 << 20);


//
// Global variables.
//
CConOcclude conocOcclusion;
