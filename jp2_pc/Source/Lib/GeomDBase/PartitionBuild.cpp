/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of PartitionBuild.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/PartitionBuild.cpp                                      $
 * 
 * 31    9/03/98 7:27p Pkeet
 * Added perturbations for new spatial partitions based on which system they are in.
 * 
 * 30    8/25/98 6:56p Rvande
 * Loop variable re-scoped
 * 
 * 29    8/21/98 5:13p Mlange
 * Build optimised tree function now handles partition trees that contain no spatial nodes.
 * 
 * 28    98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 27    7/06/98 10:47p Pkeet
 * Disabled using extra partition memory for building optimized partitions in final mode.
 * 
 * 26    2/18/98 6:43p Pkeet
 * Changed test size from two to three possible children.
 * 
 * 25    2/18/98 3:57p Pkeet
 * Added improved measure of cache quality.
 * 
 **********************************************************************************************/

#include <algorithm>
#include "Lib/Sys/RegInit.hpp"
#include "common.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Partition.hpp"
#include "PartitionSpace.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "PartitionBuild.hpp"


//
// Constants.
//

// Maximum number of objects in a subdivision.
int  iSubDivision   = 200;

// Flag indicating children should be stuffed before building and optimized partition.
bool bStuffChildren = true;

#define bONE_LIST (0)

extern float fPerturbBox;
extern int   iPerturbBoxAxis;


//
// Global functions.
//

//*********************************************************************************************
//
void SubdividePartition
(
	CPartition& rpart
);
//
// Divide the partition into smaller groups.
//
//**************************

//*********************************************************************************************
//
void GetBestSet
(
	CPartition& rpart,		// Partition to find match within.
	TPartSet&   rpartset	// The best match.
);
//
// Obtains the best set of child partitions based on the templated match criteria.
//
//**************************

//*********************************************************************************************
//
bool bBuildTree
(
	CPartition& rpart,		// Partition to build tree for.
	TPartSet*   ppartset,	// Dummy parameter for templating.
	void (__cdecl *partcallback)(int i)
);
//
// Builds an optimal spatial partitioning tree for the given template parameters.
//
//**************************

//*********************************************************************************************
//
bool bBuildTreeTerrain
(
	CPartition& rpart,		// Partition to build tree for.
	TPartSet*   ppartset,	// Dummy parameter for templating.
	void (__cdecl *partcallback)(int i)
);
//
// Builds an optimal spatial partitioning tree for the given template parameters.
//
//**************************

//*********************************************************************************************
void BuildOptimizedTree(CPartition& rpart, void (__cdecl *partcallback)(int i), bool b_terrain)
{
#if bPARTITION_BUILD
	if (!rpart.ppartChildren())
		return;


	// Get the registry settings.
	BOOL B_temp = bStuffChildren;
	GetPartitionBuildInfo(iSubDivision, B_temp);
	bStuffChildren = B_temp != 0;

	CPartitionSpace parts_nonspace;	// Temporary partition buffer;

	// Filter out everything that would interfere with the subdivision.
	parts_nonspace.TakeNonSpatialChildren(rpart);

	if (rpart.ppartChildren())
	{
		if (partcallback)
			partcallback(-2);

		// Calculate the world extents of every object.
		rpart.SetWorldExtents();

		// Put instances into other instances if possible.
		if (bStuffChildren)
		{
			rpart.StuffChildren();
		}

		if (partcallback)
		{
			partcallback(-3);
			partcallback(rpart.iCountChildren());
		}

		// While there are more than the maximum allowed children, keep building the tree.
		if (b_terrain)
		{
			while (bBuildTreeTerrain(rpart, (TPartSet*)0, partcallback));
		}
		else
		{
		#if bONE_LIST
			while (bBuildTree(rpart, (TPartSet*)0, partcallback));
		#else
			while (bBuildTreeTerrain(rpart, (TPartSet*)0, partcallback));
		#endif // bONE_LIST
		}
	}

	rpart.iCountInstances();

	rpart.TakeChildren(parts_nonspace);
#endif // bPARTITION_BUILD
}

//*********************************************************************************************
float fGetTotalSpatialVol(CPartition& part, int& ri_ccd)
{
	ri_ccd += part.iCountAllChildren();
	float f_vol = 0.0f;
	if (part.bIsPureSpatial() && part.pbvBoundingVol()->ebvGetType() != ebvINFINITE)
		f_vol = part.rGetWorldVolume();

	// Get a pointer to the child list.
	CPartition* ppartc = part.ppartChildren();

	// Get the child list iterators.
	CPartition::iterator it     = ppartc->begin();
	CPartition::iterator it_end = ppartc->end();

	for (; it != it_end; ++it)
		f_vol += fGetTotalSpatialVol(*(*it), ri_ccd);
	return f_vol;
}

//*********************************************************************************************
bool bBuildTree(CPartition& rpart, TPartSet* ppartset, void (__cdecl *partcallback)(int i))
{
	// Break the world up into smaller bits if required.
	SubdividePartition(rpart);

	//
	// While closest neighbours can be found, find them, partition them and move them to the
	// new list.
	//
	if (rpart.iCountChildren() > iNUM_CHILDREN)
	{
		TPartSet partset;

		if (partcallback)
			partcallback(1);

		GetBestSet(rpart, partset);

		// Create the new partition with a bounding sphere.
		CPartition* ppart_new = partset.ppartNewPart();

		// Add children to new partition.
		for (int i_part = 0; i_part < partset.iNumNodes(); i_part++)
		{
			//AlwaysAssert(ppart_new->bAddChild(partset.ppartGet(i_part)));
			partset.ppartGet(i_part)->SetParent(ppart_new);
		}
		ppart_new->SetParent(&rpart);
		return true;
	}

	//
	// Recursively call this function on children.
	//
	CPartition* ppartc = (CPartition*)rpart.ppartChildren();
	if (ppartc)
		for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			while (bBuildTree(*(*it), ppartset, partcallback));

	// Indicate that this function no longer has to be called.
	return false;
}

//*********************************************************************************************
bool bBuildTreeTerrain(CPartition& rpart, TPartSet* ppartset, void (__cdecl *partcallback)(int i))
{
	CPartitionSpace parts;		// Temporary partition buffer.

	// Break the world up into smaller bits if required.
	SubdividePartition(rpart);

	//
	// While closest neighbours can be found, find them, partition them and move them to the
	// new list.
	//
	while (rpart.iCountChildren() > iNUM_CHILDREN)
	{
		TPartSet partset;

		if (partcallback)
			partcallback(1);

		GetBestSet(rpart, partset);

		// Create the new partition with a bounding sphere.
		CPartition* ppart_new = partset.ppartNewPart();

		// Add children to new partition.
		for (int i_part = 0; i_part < partset.iNumNodes(); i_part++)
		{
			//AlwaysAssert(ppart_new->bAddChild(partset.ppartGet(i_part)));
			partset.ppartGet(i_part)->SetParent(ppart_new);
		}
		ppart_new->SetParent(&parts);
	}

	// Copy the newly created children back to the original list.
	rpart.TakeChildren(parts);

	// Do this again if necessary.
	if (rpart.iCountChildren() > iNUM_CHILDREN)
		return true;

	//
	// Recursively call this function on children.
	//
	CPartition* ppartc = (CPartition*)rpart.ppartChildren();
	if (ppartc)
		for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			while (bBuildTreeTerrain(*(*it), ppartset, partcallback));

	// Indicate that this function no longer has to be called.
	return false;
}

//*********************************************************************************************
void GetBestSet(CPartition& rpart, TPartSet& rpartset)
{
	Assert(rpart.iCountChildren() > 2);

	// Get a pointer to the child list.
	CPartition* ppartc = rpart.ppartChildren();

	// Get the child list iterators.
	CPartition::iterator it     = ppartc->begin();
	CPartition::iterator it_end = ppartc->end();
	if (it == it_end)
		return;

	switch (rpart.iCountChildren())
	{
		case 2:
			rpartset.Reset();
			rpartset.Set(*it);
			++it;
			rpartset.Set(*it);
			return;
		case 3:
			rpartset.Reset();
			rpartset.Set(*it);
			++it;
			rpartset.Set(*it);
			++it;
			rpartset.Set(*it);
			return;
	}

	// Iterate through each child, looking for the closest neighbours.
	for (;;)
	{
		TPartSet partset_new;

		partset_new.Set(*it);
		CPartition::iterator it_next = it;
		++it_next;
		if (it_next == it_end)
			return;

		// Iterate through possibilities.
		for (; it_next != it_end; ++it_next)
		{
			partset_new.Set(*it_next);

			// Test score.
			{
				partset_new.SetScore();
				if (partset_new < rpartset)
					rpartset = partset_new;
			}

			CPartition::iterator it_next_next = it_next;
			++it_next_next;
			for (; it_next_next != it_end; ++it_next_next)
			{
				partset_new.Set(*it_next_next);

				// Test score.
				{
					partset_new.SetScore();
					if (partset_new < rpartset)
						rpartset = partset_new;
				}

				partset_new.Back();
				if (it_next_next == it_end)
					return;
			}
			partset_new.Back();
		}

		// Increment outer loop.
		++it;
	}
}

//******************************************************************************************
//
class CPartitionSortX
//
// Less than comparison class for CPartition.
//
//**************************************
{
public:

	//**************************************************************************************
	//
	bool operator()(const CPartition* ppart_0, const CPartition* ppart_1) const
	//
	// Returns 'true' if the farthest (smallest) inverse Z value of the first polygon is
	// closer (larger) than the farthest inverse z value of the second polygon.
	//
	//**************************************
	{
		Assert(ppart_0);
		Assert(ppart_1);

		// Return the results of the comparision.
	#if bPARTITION_BUILD
		return ppart_0->v3WorldMin.tX < ppart_1->v3WorldMin.tX;
	#else
		return 0;
	#endif // bPARTITION_BUILD
	}
};

//******************************************************************************************
//
class CPartitionSortY
//
// Less than comparison class for CPartition.
//
//**************************************
{
public:

	//**************************************************************************************
	//
	bool operator()(const CPartition* ppart_0, const CPartition* ppart_1) const
	//
	// Returns 'true' if the farthest (smallest) inverse Z value of the first polygon is
	// closer (larger) than the farthest inverse z value of the second polygon.
	//
	//**************************************
	{
		Assert(ppart_0);
		Assert(ppart_1);

		// Return the results of the comparision.
	#if bPARTITION_BUILD
		return ppart_0->v3WorldMin.tY < ppart_1->v3WorldMin.tY;
	#else
		return 0;
	#endif // bPARTITION_BUILD
	}
};

//*****************************************************************************************
CPartition* ppartMake(CPartition** appart, int i_num_parts)
{
#if bPARTITION_BUILD
	Assert(appart);
	Assert(i_num_parts > 0);

	CVector3<> v3_min = appart[0]->v3WorldMin;		// Minimum world position.
	CVector3<> v3_max = appart[0]->v3WorldMax;		// Minimum world position.
	CVector3<> v3_centre;	// Centre world position.

	// Allocate memory if it has not already been allocated.
	CPartition* ppart = new CPartitionSpace();

	// Use box partitions.
	CPresence3<> pr3;							// New partition's presence.
	CBoundVolBox bvb;	// New bounding box.

	// Get the world extents.
	int i;
	for (i = 1; i < i_num_parts; ++i)
	{
		v3_min.SetMin(appart[i]->v3WorldMin);
		v3_max.SetMax(appart[i]->v3WorldMax);
	}

	// Set the centre point of the new bounding box.
	v3_centre = (v3_min + v3_max) * 0.5f;
	pr3 = CPresence3<>(v3_centre);

	// Set the bounding box using corner information.
	bvb = CBoundVolBox(v3_max - v3_centre);

	// Create the new spatial partition.
	ppart = ::new (ppart) CPartitionSpace(bvb);
	ppart->SetPos(pr3.v3Pos);
	ppart->SetWorldExtents();

	// Add children.
	for (i = 0; i < i_num_parts; ++i)
		appart[i]->SetParent(ppart);

	// Return a pointer to the new partition.
	return ppart;
#else
	return 0;
#endif // bPARTITION_BUILD
}

//*********************************************************************************************
void SubdividePartition(CPartition& rpart)
{
#if bPARTITION_BUILD
	// Do nothing if the subdivision is small enough.
	if (rpart.iCountChildren() <= iSubDivision)
		return;

	int i_num_children = rpart.iCountChildren();
	CPartition* ppartc = (CPartition*)rpart.ppartChildren();
	AlwaysAssert(ppartc);

	// Get the sizes.
	CVector3<> v3_dim = rpart.v3WorldMax - rpart.v3WorldMin;

	// Build a list of partitions.
	CPartition** appart_main = (CPartition**)_alloca(sizeof(CPartition*) * i_num_children);
	{
		int i = 0;
		
		for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it, ++i)
			appart_main[i] = *it;
	}

	// Divide on the X or Y axis.
	if (Abs(v3_dim.tX) > Abs(v3_dim.tY))
	{
		std::sort(appart_main, appart_main + i_num_children, CPartitionSortX());
	}
	else
	{
		std::sort(appart_main, appart_main + i_num_children, CPartitionSortY());
	}
	int i_num_a = i_num_children / 2;
	int i_num_b = i_num_children - i_num_a;
	CPartition** appart_a = appart_main;
	CPartition** appart_b = appart_main + i_num_a;

	// Create the new child partitions.
	CPartition* ppart_a = ppartMake(appart_a, i_num_a);
	CPartition* ppart_b = ppartMake(appart_b, i_num_b);

	// Subdivide the child partitions further if necessary.
	SubdividePartition(*ppart_a);
	SubdividePartition(*ppart_b);

	// Add the new children to the main partition.
	ppart_a->SetParent(&rpart);
	ppart_b->SetParent(&rpart);
#endif // bPARTITION_BUILD
}
