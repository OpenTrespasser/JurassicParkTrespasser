/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Occlude.hpp                                              $
 * 
 * 28    10/05/98 6:00a Pkeet
 * Increased occlusion thickness.
 * 
 * 27    10/02/98 3:03p Mmouni
 * Modified K6 assembly to use a temporary for the return value.
 * 
 * 26    9/24/98 6:07p Asouth
 * changed inline asm ; comments to //
 * 
 * 25    9/23/98 3:12p Pkeet
 * Added a stat for occluding partitions.
 * 
 * 24    9/23/98 2:29p Pkeet
 * Fixed member functions declared inline.
 * 
 * 23    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 22    9/09/98 1:54p Pkeet
 * Added parameter to switch small occlusion object culling off.
 * 
 * 21    4/06/98 3:42p Agrant
 * Added include (necessary because it was taken out of the color base hpp)
 * 
 * 20    98.01.22 3:09p Mmouni
 * Moved body of bInsideNormPlanes() function into out of the header.
 * 
 * 19    1/20/98 5:57p Pkeet
 * Fixed bug where stats were not being reported correctly.
 * 
 * 18    1/12/98 4:03p Mmouni
 * Fixed error in assertions.
 * 
 * 17    1/05/98 11:55p Gfavor
 * Converted bInsideNormPlanes to 3DX.
 * 
 * 16    12/29/97 9:18p Gfavor
 * Optimized sign check in bInsideNormPlanes.
 * 
 * 15    97/10/10 4:05p Pkeet
 * Added a switch to occlude in normalized camera space only.
 * 
 * 14    97/10/10 3:25p Pkeet
 * Added code to do partition occlusion tests in normalized camera space.
 * 
 * 13    97/10/10 2:24p Pkeet
 * Added a partition occlusion test the uses a box transform in place of a bounding box and a
 * presence.
 * 
 * 12    97/10/09 10:46a Pkeet
 * Added a data members and a function to test the normalized camera space extents for a polygon
 * first.
 * 
 * 11    97/10/08 7:22p Pkeet
 * Added the 'bInCameraView' member function. Added the ' bTestCameraView' flag.
 * 
 * 10    97/10/08 3:11p Pkeet
 * Added the 'bRemoveOccluded' and 'iMaxNumOccludeObjects' static members. Added friends of
 * 'COcclude.'
 * 
 * 9     97/10/08 2:14p Pkeet
 * Added a world area threshold.
 * 
 * 8     97/10/08 11:31a Pkeet
 * Added the camera's inverse presence and a pointer to its bounding volume as parameters to the
 * 'GetOccludePolygons' function.
 * 
 * 7     97/10/08 10:49a Pkeet
 * Added counts for occluding objects. Added comments to the stats class.
 * 
 * 6     97/10/07 5:35p Pkeet
 * Added the 'CConOcclude' object.
 * 
 * 5     97/10/07 1:41p Pkeet
 * Added an array of planes representing the occluding polyhedron in normalized camera space.
 * Added a function to test for inclusion of these points. Added a polygon occlusion function.
 * 
 * 4     97/10/06 4:56p Pkeet
 * Added the 'bOccludePartition' function. Added the 'pbvpGet' member function. Changed function
 * calls for occlusion to use an array of pointers to occlusion objects instead of an array of
 * occlusion objects.
 * 
 * 3     97/10/03 7:11p Pkeet
 * Added a fast heap static member, and enabled the construction of point and plane lists and a
 * polyhedral bounding volume in camera space.
 * 
 * 2     97/10/02 5:10p Pkeet
 * Added flags and a constructor for 'COcclude' to toggle occlusion on and off. Added the
 * 'CopyOccludePolygons' global function.
 * 
 * 1     97/10/02 1:48p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_OCCLUDE_HPP
#define HEADER_LIB_RENDERER_OCCLUDE_HPP


//
// Required includes.
//
#include <list>
#include "Lib/GeomDBase/Partition.hpp"
#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/Sys/FastHeap.hpp"
#include "AsmSupport.hpp"
#include "Lib/EntityDBase/Container.hpp"


//
// Defines.
//

// Flag indicates all tests should be done in normalized camera space.
#define bUSE_NORMSPACE_ONLY (1)

//
// Forward declarations.
//
class CCamera;
class CBoundVolPolyhedron;
class COcclude;


//
// Types.
//

// Type describing a container of pointers to occluding polygons.
typedef CContainer< std::list<COcclude> > TOccludeList;
// Prefix: polylist

// Switch to apply a slight bias to occlusion planes to make them more tolerant.
#define bBIAS_OCCLUSION (1)


//
// Constants.
//

#if bBIAS_OCCLUSION

	//
	// Bias value for occlusion planes. This value should be an approximation arrived at
	// by determining a world value for the bias and dividing by the far clipping plane.
	// For example, if the bias desired is 2 cm, and the far clipping plane is at a 1000 m,
	// then the bias value = 0.02 m / 1000.0 m, or 0.00002.
	//
	// The current value is set for 1 cm.
	//
	const float  fBiasOcclusion = 0.00003f;
	extern float fLocalBias;
	extern bool  bUseBias;

#endif // bBIAS_OCCLUSION


//
// Class definitions.
//

//**********************************************************************************************
//
class COcclude
//
// A list of occluding geometries.
//
// Prefix: oc
//
// Notes:
//		Occlusion objects have a heap for storage of object data because of the difficulty
//		of dynamically allocating memory from the stack and the slowness of new and delete.
//
//		The 'bTestCameraView' flag causes an additional test to see if the occlusion object
//		is in the camera's view; otherwise occlusion objects are included if their containing
//		partition's are in the camera's view.
//
//**************************************
{
public:

	static bool  bUseObjectOcclusion;	// Flag indicates object occlusion should be use.
	static bool  bUsePolygonOcclusion;	// Flag indicates object occlusion should be use.
	static bool  bTestCameraView;		// Test that the object is within the camera view.
	static bool  bRemoveOccluded;		// Flag indicates occluded occluding objects should
										// not be used.
	static TReal rWorldAreaThreshold;	// Threshold above which to use an occlusion polygon.
	static int   iMaxNumOccludeObjects;	// Maximum number of occluding objects in one view.

private:

	// Data for the object.
	CPartition*           ppartPartition;	// Owning partition.
	CMesh::SPolygon*      ppolyPolgyon;		// Pointer to the original occluding polygon.
	bool				  bFacing;			// Flag indicates polygon is facing the camera.
	TReal                 rWorldArea;		// World area of the polygon facing the camera.

#if !bUSE_NORMSPACE_ONLY
	// Data for camera space polyhedron.
	CBoundVolPolyhedron*  pbvpPolyhedron;	// Polyhedron bounding volume.
	CPArray< CVector3<> > pav3Points;		// Points defining the vertices of the polyhedron.
	CPArray<CPlane>       paplPlanes;		// Planes defining the walls of the polyhedron.
#endif // bUSE_NORMSPACE_ONLY
	CVector3<>            v3Centre;			// Centre of the occluding object.

	// Data for normalized camera space polyhedron.
	CBoundVolPolyhedron*  pbvpPolyhedronNorm;	// Polyhedron bounding volume.
	CPArray< CVector3<> > pav3PointsNorm;	// Points defining the vertices of the polyhedron.
	CPArray<CPlane>       paplPlanesNorm;	// Planes defining the walls of the polyhedron.
	CVector3<>            v3MinNorm;		// Minimum values for vertices.
	CVector3<>            v3MaxNorm;		// Maximum values for vertices.
	#if _DEBUG
		CVector3<>        v3CentreNorm;		// Centre of the occluding object.
	#endif // _DEBUG

	// Allocation heap.
	static CFastHeap      fhMemory;			// Memory for allocating internal objects.

public:
	
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	COcclude();

	// Constructor.
	COcclude(CMesh::SPolygon* ppoly, CPartition* ppart);

	// Copy constructor.
	COcclude(const COcclude& oc);

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	bool bInitialize
	(
		const CPresence3<>&  pr3_cam_inv,	// The inverse presence of the camera.
		const CTransform3<>& tf3			// Transform to normalized camera space.
	);
	//
	// Initializes the class.
	//
	//**********************************

	//******************************************************************************************
	//
	static bool bBuildOcclusionList
	(
	)
	//
	// Returns true if a list of occluding polygons should be built for rendering.
	//		
	//**********************************
	{
		return bUseObjectOcclusion || bUsePolygonOcclusion;
	}

	//******************************************************************************************
	//
	static void Reset
	(
	)
	//
	// Resets memory allocations for this class.
	//		
	//**********************************
	{
		fhMemory.Reset();
	}

	//******************************************************************************************
	//
	void AssertValid
	(
	) const;
	//
	// Asserts if data within the object contains invalid values.
	// 
	//**********************************

#if !bUSE_NORMSPACE_ONLY
	//******************************************************************************************
	//
	const CBoundVolPolyhedron* pbvpGet
	(
	) const
	//
	// Returns a pointer to the bounding polyhedron created by the occluding object.
	// 
	//**********************************
	{
		Assert(pbvpPolyhedron);

		return pbvpPolyhedron;
	}
#endif // bUSE_NORMSPACE_ONLY

	//******************************************************************************************
	//
	const CBoundVolPolyhedron* pbvpGetNorm
	(
	) const
	//
	// Returns a pointer to the bounding polyhedron created by the occluding object.
	// 
	//**********************************
	{
		Assert(pbvpPolyhedronNorm);

		return pbvpPolyhedronNorm;
	}

	//******************************************************************************************
	//
	forceinline bool bInsideNormPlanes
	(
		const CVector3<>& v3
	) const
	//
	// Returns 'true' if the point is inside the normalized polygon's plane.
	//
	// Notes:
	//		This test differs from regular plane tests in that tolerances are not required.
	// 
	//**********************************
	{
		// Plane test.
	#if bBIAS_OCCLUSION
		fLocalBias = bUseBias ? fBiasOcclusion : 0.0f;
	#endif // bBIAS_OCCLUSION

	#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		bool bReturn;

		uint u_cnt = paplPlanesNorm.uLen;
		const CPlane* papl_0 = &paplPlanesNorm[0];

		typedef CVector3<> tdCVector3;

		Assert((char *)&papl_0->d3Normal.tX - (char *)&papl_0->d3Normal == 0);
		Assert((char *)&papl_0->d3Normal.tY - (char *)&papl_0->d3Normal == 4);
		Assert((char *)&papl_0->d3Normal.tZ - (char *)&papl_0->d3Normal == 8);

		__asm
		{
			femms									// ensure fast switch

			mov		eax,[v3]						// get pointer to v3

			mov		ecx,[u_cnt]

			mov		ebx,[papl_0]					// get ptr to plane of first polygon in array

			movq	mm1,[eax]tdCVector3.tX			// m1=   v3.Y   | v3.X

			movd	mm3,[eax]tdCVector3.tZ			// m3=     0    | v3.Z
			test	ecx,ecx							// check if any planes to check against

			mov		eax,1							// initialize return value to true
			jnz		PlaneLoop						// if there are planes to check, go start

			jmp		ExitPlaneLoop					// else already done and go return true

			// EAX = return value (0 or 1 / true or false)
			// EBX = ptr to first element in array of pointers to planes to be checked against
			// ECX = number of planes to be checked

			// MM1 = v3.Y | v3.X
			// MM3 =   0  | v3.Z

			align 16
			nop										// establish 1 byte starting code offset
		PlaneLoop:
			movq	mm0,[ebx+0]CPlaneDef.d3Normal	// m0=   pl.Y   | pl.X

			movd	mm2,[ebx+8]CPlaneDef.d3Normal	// m2=     0    | pl.Z

			movd	mm4,[ebx]CPlaneDef.rD			// m4= rD
			pfmul	(m0,m1)							// m0= pl.Y*v.Y | pl.X*v.X

			pfmul	(m2,m3)							// m2=     0    | pl.Z*v3.Z

			pfacc	(m0,m0)							// m0= pl.Y*v3.Y + pl.X*v3.X

			pfadd	(m2,m4)							// m2= pl.Z*v3.Z + rD

		#if bBIAS_OCCLUSION
			movd	mm4,fLocalBias					// load bias constant.
		#endif

			pfadd	(m0,m2)							// m0= rDistance

		#if bBIAS_OCCLUSION
			pfadd	(m0,m4)							// add bias.
		#endif

			movd	eax,mm0							// eax= rDistance

			shr		eax,31							// eax= sign(rDistance)
			jz		ExitPlaneLoop					// go return false if sign(rDistance)==0

			add		ebx,SIZE CPlane					// advance to next plane
			loop	PlaneLoop						// if not done, go check next plane
													// else exit and return true
		ExitPlaneLoop:
			// EAX = 0 or 1 = false or true
			femms									// empty MMX state and ensure fast switch
			mov		[bReturn],al
		}
		return bReturn;

	#else // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		for (uint u = 0; u < paplPlanesNorm.uLen; ++u)
		{
			// Get the distance from the point to the plane.
		#if bBIAS_OCCLUSION
			TReal r_dist = paplPlanesNorm[u].rDistance(v3) + fLocalBias;
		#else
			TReal r_dist = paplPlanesNorm[u].rDistance(v3);
		#endif // bBIAS_OCCLUSION

			//
			// If the distance is negative, the point is inside the plane. If the point is not
			// inside the plane, it is always outside. Terminate only when an outside point is
			// found.
			//
			if (!CIntFloat(r_dist).bSign())
				return false;
		}
		return true;

	#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
	}

	//******************************************************************************************
	//
	forceinline bool bInsideNormPlanes
	(
		const CPArray< CVector3<> >& pav3
	) const
	//
	// Returns 'true' if the specified points lay entirely within the normalized camera space
	// planes of the occluding object.
	// 
	//**********************************
	{
		// Test all points against the plane.
		for (uint u = 0; u < pav3.uLen; ++u)
		{
			// The test fails if a point is found that is not inside the planes.
			if (!bInsideNormPlanes(pav3[u]))
				return false;
		}
		return true;
	}

	//******************************************************************************************
	//
	bool bInsideNormExtents
	(
		const CVector3<>& v3_poly_min,
		const CVector3<>& v3_poly_max
	) const
	//
	// Returns 'true' if the specified points lay entirely within the normalized camera space
	// extents of the occluding object.
	// 
	//**********************************
	{
		// Trivial rejection based on screen space extents.
		if (u4FromFloat(v3_poly_min.tY) < u4FromFloat(v3MinNorm.tY))
			return false;
		if (v3_poly_min.tX < v3MinNorm.tX)
			return false;
		if (v3_poly_max.tX > v3MaxNorm.tX)
			return false;
		if (v3_poly_min.tZ < v3MinNorm.tZ)
			return false;
		if (v3_poly_max.tZ > v3MaxNorm.tZ)
			return false;

		// Test succeeded.
		return true;
	}
	
private:
	
#if !bUSE_NORMSPACE_ONLY

	//******************************************************************************************
	void TransformPoints(int i_start, int i_end, int i_increment, const CPresence3<>& pr3);

	//******************************************************************************************
	void BuildPlaneList(const CPlane& pl_first);

#endif // bUSE_NORMSPACE_ONLY

	//******************************************************************************************
	void TransformPointsNorm(int i_start, int i_end, int i_increment, const CTransform3<>& tf3);

	//******************************************************************************************
	void BuildPlaneListNorm();

	//******************************************************************************************
	void PrintPoints() const;

	//******************************************************************************************
	bool bInCameraView() const;

	//******************************************************************************************
	friend class CSortOcclude;

public:

	//*****************************************************************************************
	//
	friend void CopyOccludePolygons
	(
		CPArray<COcclude*>& rpapoc,			// Array to create and copy occluding objects to.
		const CCamera&      cam,			// Camera supplying bounding volume.
		TOccludeList&       roclist,		// Source list of mesh polygons to grow.
		bool                b_stats = true,	// Flag to record stats.
		bool                b_cull = true	// Flag to cull small occlusion polygons.
	);
	//
	// Recursively generates a list of occlusion polygons
	//
	//**************************************

};


//**********************************************************************************************
//
class CConOcclude
//
// Object for reporting occlusion stats.
//
// Prefix: conoc
//
//**************************************
{
	int iPolysOccluded;
	int iPolysVisible;
	int iNumOcclusionObjectsInCamera;
	int iNumOcclusionObjectsUsed;
	int iPartitionsOccluded;

public:

	//******************************************************************************************
	//
	// Constructor and destructor.
	//

	// Default constructor.
	CConOcclude();

	// Destructor.
	~CConOcclude();

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void AddOccludedPoly
	(
	)
	//
	// Increments the count of occluded polygons.
	//
	//**********************************
	{
		++iPolysOccluded;
	}

	//******************************************************************************************
	//
	void AddOccludedPartition
	(
	)
	//
	// Increments the count of occluded partitions.
	//
	//**********************************
	{
		++iPartitionsOccluded;
	}

	//******************************************************************************************
	//
	void AddVisiblePoly
	(
	)
	//**********************************
	{
		++iPolysVisible;
	}
	//
	// Increments the count of visible polygons.
	//

	//******************************************************************************************
	//
	void SetNumOcclusionObjectsInCamera
	(
		int i_num
	);
	//
	// Sets the number of occlusion objects visible.
	//

	//******************************************************************************************
	//
	void SetNumOcclusionObjectsUsed
	(
		int i_num
	);
	//
	// Sets the number of occlusion objects used.
	//
	//**********************************

	//******************************************************************************************
	//
	bool bIsActive
	(
	) const;
	//
	// Returns 'true' if the console buffer for outputting occlusion information is active.
	//
	//**********************************

	//******************************************************************************************
	//
	void Reset
	(
	);
	//
	// Reset stats by initializing counts to zero.
	//
	//**********************************

	//******************************************************************************************
	//
	void Show
	(
	);
	//
	// Display stats.
	//
	//**********************************

};


//
// Global functions.
//

//*********************************************************************************************
//
void GetOccludePolygons
(
	CPartition*         ppart,				// Partition to find occluding polygons in.
	const CPresence3<>& pr3_inv_cam,		// Inverse presence of camera.
	const CBoundVol*    pbv_cam,			// Camera's bounding volume.
	TOccludeList&       roclist,			// List of mesh polygons to grow.
	ESideOf             esf_view = esfON	// View relation to camera.
);
//
// Recursively generates a list of occlusion polygons
//
//**************************************

//*********************************************************************************************
//
bool bOccludePartition
(
	const CPArray<COcclude*>& rpapoc,					// Array of occluding objects.
	CPArray<COcclude*>&       rpapoc_intersect_list,	// Array of occluding objects that
														// intersect.
	const CPresence3<>&       pr3_it_cam,				// Partition to camera space transform.
	const CBoundVol*          pbv_it					// Partition's bounding volume.
);
//
// Returns 'true' if the partition is occluded. Builds a secondary array of occluding objects
// that intersects the partition.
//
//**************************************

//*********************************************************************************************
//
bool bOccludePartition
(
	const CPArray<COcclude*>& rpapoc,					// Array of occluding objects.
	CPArray<COcclude*>&       rpapoc_intersect_list,	// Array of occluding objects that
														// intersect.
	const CTransform3<>&      tf3_box					// Partition's box transform.
);
//
// Returns 'true' if the partition is occluded. Builds a secondary array of occluding objects
// that intersects the partition.
//
// Notes:
//		This is a more efficient function that the general 'bOccludePartition' function.
//
//**************************************

//*********************************************************************************************
//
bool bOccludePartitionNorm
(
	const CPArray<COcclude*>& rpapoc,					// Array of occluding objects.
	CPArray<COcclude*>&       rpapoc_intersect_list,	// Array of occluding objects that
														// intersect.
	const CTransform3<>&      tf3_box					// Partition's box transform.
);
//
// Returns 'true' if the partition is occluded. Builds a secondary array of occluding objects
// that intersects the partition.
//
// Notes:
//		This is a more efficient function that the general 'bOccludePartition' function.
//
//**************************************

//*********************************************************************************************
//
bool bOccludePolygon
(
	const CPArray<COcclude*>&    rpapoc,			// Array of occluding objects.
	const CPArray< CVector3<> >& pav3_cam_vertices	// Transformed vertices of the polygon.
);
//
// Returns 'true' if the polygon is occluded.
//
//**************************************


//
// Global variables.
//
extern CConOcclude conocOcclusion;


#endif // HEADER_LIB_RENDERER_OCCLUDE_HPP
