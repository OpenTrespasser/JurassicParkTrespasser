/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Class types for building optimal spatial partitions. All classes defined inherit from
 *		CPartitionSet, the base class for these types.
 *
 * To do:
 *
 * Notes:
 *		
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/PartitionBuild.hpp                                     $
 * 
 * 20    9/03/98 7:27p Pkeet
 * Added perturbations for new spatial partitions based on which system they are in.
 * 
 * 19    7/06/98 10:47p Pkeet
 * Disabled using extra partition memory for building optimized partitions in final mode.
 * 
 * 18    2/18/98 6:43p Pkeet
 * Changed test size from two to three possible children.
 * 
 * 17    2/18/98 3:57p Pkeet
 * Added improved measure of cache quality.
 * 
 * 16    2/18/98 12:06p Pkeet
 * Partition fitness test does not take into account volumes removed by the addition of a
 * partition.
 * 
 * 15    1/30/98 4:33p Pkeet
 * Removed the fast partition build.
 * 
 * 14    1/27/98 6:23p Pkeet
 * Optimized optimal partition building.
 * 
 * 13    1/27/98 5:04p Pkeet
 * Added data members and functions for partition building only to optimize builds.
 * 
 * 12    97/10/13 5:09p Pkeet
 * Added a class specific new and delete for 'CPartitionSpace.'
 * 
 * 11    97/10/13 4:20p Pkeet
 * Added an optional parameter to the partition new function to provide memory instead of
 * calling the global new.
 * 
 * 10    97/09/30 1:21p Pkeet
 * Used the set position function instead of passing a presence to the constructor when
 * building a CPartitionSpace object.
 * 
 * 9     97/09/29 16:31 Speter
 * All bounding volumes are now positionless.  Removed v3Pos from Box and Sphere.  
 * 
 * 8     97/09/29 2:20p Pkeet
 * Added use of the 'fGetScale' function.
 * 
 * 7     97/09/28 3:07p Pkeet
 * Replace the use of 'CPresence3<>().v3Pos' with 'v3Pos().'
 * 
 * 6     97/09/26 3:34p Pkeet
 * Changed function to use the new form of the CPartitionSpace constructor.
 * 
 * 5     97/07/01 11:24a Pkeet
 * Removed memory leaks.
 * 
 * 4     97/06/30 3:16p Pkeet
 * Changed the builder to use the add volume as a score instead of the total new volume.
 * 
 * 3     97/03/14 7:02p Pkeet
 * Added support for aligned boxes.
 * 
 * 2     97/03/14 5:31p Pkeet
 * Added axis aligned box code.
 * 
 * 1     97/03/14 12:24p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_PARTITIONBUILD_HPP
#define HEADER_LIB_GEOMDBASE_PARTITIONBUILD_HPP


//
// Necessary includes.
//
#include "PartitionBuildBase.hpp"


//
// Defines.
//

// Number of children for a partition.
#define iNUM_CHILDREN (3)

extern float fPerturbBox;
extern int   iPerturbBoxAxis;


//
// Class definitions.
//

//*********************************************************************************************
//
template<int N> class CPartitionVol : public CPartitionSet<N>
//
// A slower optimal tree using minimum volumes.
//
//**************************
{
public:
	
	//*****************************************************************************************
	//
	// Member functions for this class only.
	//

	//*****************************************************************************************
	//
	void GetMinMaxBox
	(
		CVector3<>& rv3_min,	// Minimum world position.
		CVector3<>& rv3_max		// Maximum world position.
	)
	//
	// Gets the world extents represented by the volume.
	//
	//**************************
	{
	#if bPARTITION_BUILD
		CPartition* ppart_anchor = ppartGet(0);
		rv3_min = ppart_anchor->v3WorldMin;
		rv3_max = ppart_anchor->v3WorldMax;

		//
		// Iterate through the partition set to get the minimum and maximum world positions.
		//
		for (int i_part = 1; i_part < iNumNodes(); i_part++)
		{
			CPartition* ppart_test = ppartGet(i_part);
			CVector3<> v3_min_new = ppart_test->v3WorldMin;
			CVector3<> v3_max_new = ppart_test->v3WorldMax;

			// Minimum coordinates.
			rv3_min.tX = Min(rv3_min.tX, v3_min_new.tX);
			rv3_min.tY = Min(rv3_min.tY, v3_min_new.tY);
			rv3_min.tZ = Min(rv3_min.tZ, v3_min_new.tZ);

			// Maximum coordinates.
			rv3_max.tX = Max(rv3_max.tX, v3_max_new.tX);
			rv3_max.tY = Max(rv3_max.tY, v3_max_new.tY);
			rv3_max.tZ = Max(rv3_max.tZ, v3_max_new.tZ);
		}
	#endif // bPARTITION_BUILD
	}

	//*****************************************************************************************
	//
	void SetAxisBox
	(
		CPresence3<>& rpr3,	// Presence for the new box.
		CBoundVolBox& rbvb	// Bounding box to set.
	)
	//
	// Sets a bounding box as an axis-aligned box to fit the current bounding volumes.
	//
	//**************************
	{
		Assert(iNumNodes() >= 1);

		CVector3<> v3_min;		// Minimum world position.
		CVector3<> v3_max;		// Minimum world position.
		CVector3<> v3_centre;	// Centre world position.

		// Get the world extents.
		GetMinMaxBox(v3_min, v3_max);

		// Set the centre point of the new bounding box.
		v3_centre = (v3_min + v3_max) * 0.5f;
		rpr3 = CPresence3<>(v3_centre);

		// Perturb box.
		switch (iPerturbBoxAxis)
		{
			//fPerturbBox
			case 0:
				v3_max.tX += fPerturbBox;
				break;

			case 1:
				v3_max.tY += fPerturbBox;
				break;

			case 2:
				v3_max.tZ += fPerturbBox;
				break;

			case 3:
				// No perturbation.
				break;

			default:
				AlwaysAssert(0);
		}

		// Set the bounding box using corner information.
		rbvb = CBoundVolBox(v3_max - v3_centre);
	}

	//*****************************************************************************************
	//
	void SetBoundingSphere
	(
		CVector3<>& rv3,	// Centre point.
		TReal&      rr_rad	// Radius.
	)
	//
	// Sets the bounding sphere parameters to fit this partition set.
	//
	//**************************
	{
		CBoundVolSphere bvs[N];		// Bounding spheres.
		CVector3<>      v3[N];		// Vector array.
		TReal           r_rad[N];	// Radii.

		rr_rad = 0.0f;
		rv3 = CVector3<>(0.0f, 0.0f, 0.0f);

		// Get the bounding spheres, positions and radii for the objects.
		for (int i_bvs = 0; i_bvs < iNumNodes(); i_bvs++)
		{
			bvs[i_bvs]   = ppartGet(i_bvs)->pbvBoundingVol()->bvsGetBoundingSphere();
			v3[i_bvs]    = ppartGet(i_bvs)->pr3Presence().v3Pos;
			r_rad[i_bvs] = bvs[i_bvs].rRadius * ppartGet(i_bvs)->fGetScale();
			rv3         += v3[i_bvs];
		}

		// Calculate the midpoint for the partition set.
		rv3 /= TReal(iNumNodes());
			
		// Calculate the new radius.
		for (i_bvs = 0; i_bvs < iNumNodes(); i_bvs++)
		{
			rr_rad = Max(rr_rad, (rv3 - v3[i_bvs]).tLen() + r_rad[i_bvs]);
		}
		rr_rad *= 1.001f;
	}
	
	//*****************************************************************************************
	//
	// Overloaded member functions.
	//

	//*****************************************************************************************
	void SetScore()
	{
	#if bPARTITION_BUILD
		if (iNumNodes() < 2)
		{
			fScore = FLT_MAX;
			return;
		}

		//
		// The best score represents the smallest newly added volume -- ie., each existing
		// partition contributes an amount of volume to the world database; the goal is
		// to minimize the total amount of volume represented by the world database.
		// Therefore the amount of new volume a new spatial partition adds is examine.
		//
		CVector3<> v3_min;
		CVector3<> v3_max;

		//
		// Get the volume of the potential spatial partition.
		//
		GetMinMaxBox(v3_min, v3_max);
		CVector3<> v3 = v3_max - v3_min;
		fScore = Abs(v3.tX * v3.tY * v3.tZ);

		//
		// Subtract potential child partition volumes from the score.
		//
		for (int i_part = 0; i_part < iNumNodes(); i_part++)
			fScore -= ppartGet(i_part)->fVolumeScore;
	#endif bPARTITION_BUILD
	}

	//*****************************************************************************************
	CPartition* ppartNewPart(CPartition* ppart = 0)
	{
	#if bPARTITION_BUILD
		// Allocate memory if it has not already been allocated.
		if (!ppart)
		{
			ppart = new CPartitionSpace();
		}
		Assert(ppart);

		// Use box partitions.
		CPresence3<> pr3;							// New partition's presence.
		CBoundVolBox bvb;	// New bounding box.
		SetAxisBox(pr3, bvb);

		// Create the new spatial partition.
		ppart = ::new (ppart) CPartitionSpace(bvb);
		ppart->SetPos(pr3.v3Pos);
		ppart->SetWorldExtents();
		return ppart;
	#else
		return 0;
	#endif // bPARTITION_BUILD
	}

};

//*********************************************************************************************
//
float fGetTotalSpatialVol
(
	CPartition& part,
	int&        ri_ccd
);
//
// Returns the total spatial volume.
//
//**************************


//
// Global type definitions.
//

// Define an optimal tree.
typedef CPartitionVol<iNUM_CHILDREN> TPartSet;


#endif // HEADER_LIB_GEOMDBASE_PARTITIONBUILD_HPP
