/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of Partition.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/Partition.cpp                                           $
 * 
 * 152   10/01/98 12:46p Pkeet
 * Added the bCacheableVolume flag to bCanCache.
 * 
 * 151   9/27/98 7:59p Pkeet
 * Added a parameter to 'bAddChild' and 'bFastAddChild' that allows reparenting only if the
 * priorities allow it.
 * 
 * 150   9/25/98 1:51a Pkeet
 * Added a hack to make biomeshes cacheable.
 * 
 * 149   9/24/98 8:39p Mlange
 * Now loads/saves partition flags again. It now sets the visiblity state of the partition from
 * the saved partition flags.
 * 
 * 148   9/10/98 4:12p Agrant
 * Don't save partition flags.  We don't ever use them.
 * 
 * 147   98/09/04 22:01 Speter
 * Added sender to Move().
 * 
 * 146   9/04/98 2:22p Pkeet
 * Fixed discrepancy between partition radius and culling distance when initializing
 * partitioning systems.
 * 
 * 145   9/04/98 1:54p Pkeet
 * Fixed crash on exit after rebuilding partitions.
 * 
 **********************************************************************************************/

// Note: reording done to insure Windows.h is included before
// io.h "disables" a subsequent include of same.
//#include "multiset.h"
#include <map>
#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "PartitionPriv.hpp"
#include "Lib/Loader/DataDaemon.hpp"
#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/Std/StringEx.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/Renderer/RenderCacheInterface.hpp"
#include "Lib/Renderer/RenderCacheHelp.hpp"
#include "Lib/Renderer/RenderCache.hpp"
#include "Lib/Std/BlockArray.hpp"
#include "PartitionSpace.hpp"
#include "PartitionBuild.hpp"
#include "Lib/Std/Hash.hpp"
#include "Lib/Math/FastInverse.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "PartitionPrivClass.hpp"
#include "Lib/Loader/SaveFile.hpp"


//
// Defines and constants.
//

// Reverse the order of lists.
#define bREVERSE_LIST_ORDER   (0)

// Flag for logging preloader activity.
#define bLOG_PRELOADER        (0)

// Console for predictive loading.
CConsoleBuffer conPreLoader;


//
// Map for finding duplicate spatial partitions.
//
typedef std::map< uint32, CPartition*, std::less<uint32> > TPartDuplicate;
TPartDuplicate partdupMap;

//
// Internal class definitions.
//

//******************************************************************************************
//
class CPartVol
//
// Object used for sorting partitions by volume.
//
//**************************************
{
public:

	CPartition* ppartPartition;
	TReal       rVolume;

public:

	//**********************************************************************************
	//
	// Constructors.
	//

	CPartVol()
	{
	}

	CPartVol(CPartition* ppart)
	{
		Assert(ppart);

		ppartPartition = ppart;
		rVolume        = ppart->rGetWorldVolume();
	}

	
	//**********************************************************************************
	//
	// Member functions.
	//

	//**********************************************************************************
	//
	bool operator()(const CPartVol& pvol_a, const CPartVol& pvol_b) const
	//
	// Returns 'true' if the volume of partition 'a' is less than the volume of
	// partition 'b.'
	//
	//**************************************
	{
		// Return the results of the comparision.
		return pvol_a.rVolume < pvol_b.rVolume;
	};

};


//******************************************************************************************
//
static void PrintPos(CConsoleBuffer& con, const CPartition* ppart)
//
// Outputs the partition's position to a log file.
//
//**************************************
{
	CVector3<> v3 = ppart->v3Pos();
	con.Print("\t%1.1f, %1.1f, %1.1f\n", v3.tX, v3.tY, v3.tZ);
}


//
// Class implementations.
//

//*********************************************************************************************
//
// CPartition implementation.
//

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	//*****************************************************************************************
	CPartition::CPartition()
	{
		priv_self.SetConstructionValues();
	}

	//*****************************************************************************************
	CPartition::CPartition(int i_handle)
	{
		int i_num_read;

		priv_self.SetConstructionValues();

		// Store the partition data.
		int32 i4_dummy;
		i_num_read = _read(i_handle, (char*)&i4_dummy, (int)sizeof(pdData));
		//i_num_read = _read(i_handle, &pdData, sizeof(pdData));
		AlwaysAssert(i_num_read == sizeof(pdData));
	}

	//*****************************************************************************************
	CPartition::~CPartition()
	{
		while (ppartChild)
		{
			CPartition* ppart_next = ppartChild->ppartNext;
			delete ppartChild;
			ppartChild = ppart_next;
		}
		DeleteRenderCache();
	}


	//*****************************************************************************************
	//
	// CPartition member functions.
	//

	//*****************************************************************************************
	void CPartition::SetFlagShadow(bool b_flag)
	{
		pdData.bCastShadow = b_flag;
	}

	//*****************************************************************************************
	void CPartition::SetFlagCacheIntersecting(bool b_flag)
	{
		pdData.bCacheIntersecting = b_flag;
	}

	//*****************************************************************************************
	void CPartition::Move(const CPlacement3<>& p3_new, CEntity*)
	{
		// Sender not referenced in base class move.
		// The world database really knows how to deal with moved partitions.
		wWorld.Move(this, p3_new);
	}

	//*****************************************************************************************
	ESideOf CPartition::esfSideOf(const CPartition* ppart) const
	{
		//
		// If there is no partition object specified, it is assumed that a partition with an
		// infinite bounding volume is intended. Infinite bounding volumes always intersect
		// with everything; therefore if no partition object is specified, this function should
		// always esfINTERSECT.
		//
		if (!ppart)
			return esfINTERSECT;

		const CBoundVol* pbv_it = ppart->pbvBoundingVol();
		Assert(pbv_it);

		const CBoundVol* pbv_this = pbvBoundingVol();
		Assert(pbv_this);

		//
		// Using the bounding volume intersection code to see if the partitions intersect.
		//
		return pbv_this->esfSideOf
		(
			*pbv_it,					// Target partition bounding volume.
			&pr3Presence(),				// Current presence.
			&ppart->pr3Presence()		// Target presence.
		);
	}

	//*****************************************************************************************
	void CPartition::BuildPartList(const CPartition* ppart, TPartitionList& rpartlist)
	{
		ESideOf esf;

		if (ppart)
		{
			esf = ppart->esfSideOf(this);

			// Intersect this with ppart.
			if (esf == esfOUTSIDE)
				// No intersection, do not add this partition.
				return;
			if (esf == esfINSIDE)
				// Set the bounding partition to 0, to disable checking on children.
				ppart = 0;
		}
		else
			esf = esfINSIDE;

		// Check if partition is an instance, otherwise don't add it to the list.
		if (bIsInstance())
		{
			SPartitionListElement ple = {this, esf};
			rpartlist.push_back(ple);
		}

		//
		// Check if there are children, and if they intersect the target partition.
		//

		// Return if there are no children.
		if (!ppartChildren())
			return;

		// Get a pointer to the child list.
		CPartition* ppartc = ppartChildren();

		if (ppartc)
		{
			// Iterate throught the container and write elements to disk.
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				(*it)->BuildPartList(ppart, rpartlist);
		}
	}

	//*****************************************************************************************
	void CPartition::BuildPartListAll(const CPartition* ppart, TPartitionList& rpartlist)
	{
		ESideOf esf;

		if (ppart)
		{
			esf = ppart->esfSideOf(this);

			// Intersect this with ppart.
			if (esf == esfOUTSIDE)
				// No intersection, do not add this partition.
				return;
			if (esf == esfINSIDE)
				// Set the bounding partition to 0, to disable checking on children.
				ppart = 0;
		}
		else
			esf = esfINSIDE;

		// Check if partition is an instance, otherwise don't add it to the list.
		SPartitionListElement ple = {this, esf};
		rpartlist.push_back(ple);

		//
		// Check if there are children, and if they intersect the target partition.
		//

		// Return if there are no children.
		if (!ppartChildren())
			return;

		// Get a pointer to the child list.
		CPartition* ppartc = ppartChildren();

		if (ppartc)
		{
			// Iterate throught the container and write elements to disk.
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				(*it)->BuildPartListAll(ppart, rpartlist);
		}
	}
	
	//*****************************************************************************************
	bool CPartition::bEmptyPart() const
	{
		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		// If there are no children, the partition is empty.
		if (!ppartc)
			return true;

		// If there are children to iterate through, the partition is not empty.
		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			if (!(*it)->bEmptyPart())
				return false;
		return true;
	}
	
	//*****************************************************************************************
	void CPartition::PruneEmpties()
	{
		// Get a pointer to the child list.
		CPartition* ppartc = ppartChildren();

		// If there are no children, the partition is empty.
		if (!ppartc)
			return;
		if (bEmptyPart())
			return;

		// Empty children partitions.
		iterator it = ppartc->begin();
		for (; it != ppartc->end(); ++it)
			(*it)->PruneEmpties();

		// Build a list of children to prune.
		TPartitionListChild contpart;

		// Iterate through the children looking for empty partitions.
		for (it = ppartc->begin(); it != ppartc->end(); ++it)
			if ((*it)->bEmptyPart())
				contpart.push_back(*it);

		// Delete child partitions.
		for (contpart.Begin(); contpart.bIsNotEnd(); contpart++)
		{
			(*contpart)->SetParent();
			delete (*contpart);
		}
	}
	
	//*****************************************************************************************
	bool CPartition::bCanHaveChildren()
	{
		return true;
	}

	//*****************************************************************************************
	bool CPartition::bFastAddChild(CPartition* ppart, bool b_check_priority)
	{
		// Do nothing if the partition cannot have children.
		if (!bCanHaveChildren())
			return false;

		// Do nothing if the partitions are the same.
		if (ppart == this)
			return false;

		// Test priorities.
		if (b_check_priority)
		{
			//
			// Allow child only if this partition has a lower or equal priority to the potential
			// new child partition.
			//
			if (pdData.iPriority > ppart->pdData.iPriority)
				return false;
		}

		// Can we have this child?
		if (bCanContain(ppart))
		{
			// Yes!  Adopt it.
			// Don't forget to set the parent pointer.
			ppart->SetParent(this);

			// Containment done.
			return true;
		}

		// Containment is not possible.
		return false;
	}
	
	//*****************************************************************************************
	bool CPartition::bAddChild(CPartition* ppart, bool b_check_priority)
	{
		// Do nothing if the partition cannot have children.
		if (!bCanHaveChildren())
			return false;

		// Do nothing if the partitions are the same.
		if (ppart == this)
			return false;

		// Test priorities.
		if (b_check_priority)
		{
			//
			// Allow child only if this partition has a lower or equal priority to the potential
			// new child partition.
			//
			if (pdData.iPriority > ppart->pdData.iPriority)
				return false;
		}

		// Get a pointer to the child list.
		CPartition* ppartc = ppartChildren();

		//
		// None of the children can accomodate the new partition, see if the current partition
		// can therefore contain the new partition.
		//

		if (bCanContain(ppart))
		{
			if (ppartc)
			{
				// Recurse the container looking for a child to add the new partition to.
				for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
					if ((*it)->bAddChild(ppart))
						return true;
			}

			// Don't forget to set the parent pointer.
			ppart->SetParent(this);

			// Containment done.
			return true;
		}

		// Containment is not possible.
		return false;
	}

	//*****************************************************************************************
	bool CPartition::bCanContain(CPartition* ppart)
	{
		Assert(ppart);
		Assert(pbvBoundingVol());

		return pbvBoundingVol()->bContains(*ppart->pbvBoundingVol(), &pr3Presence(),
			                               &ppart->pr3Presence());
	}

	//*****************************************************************************************
	void CPartition::SetParent(CPartition* ppart)
	{
		// Do nothing if the current parent is the same as the target parent.
		if (ppart == ppartParent)
			return;

	#if (0 && BUILDVER_MODE != MODE_FINAL)
		if (ppart)
		{
			AlwaysAssert(ppart->u4GetUniqueHandle() != u4GetUniqueHandle());
		}
	#endif

		// Remove from previous parent.
		if (ppartParent)
		{
			// Get a pointer to the child list of the parent.
			ppartParent->RemoveChildFromList(this);
		}

		// Set the new parent pointer.
		ppartParent = ppart;

		// If the new parent is a null pointer, this function if finished.
		if (!ppart)
			return;

		// Add the new partition to the parent's child list.
		ppart->PushChildToList(this);
	}
	
	//*****************************************************************************************
	void CPartition::Write(CConsoleBuffer& rcon, int i_depth) const
	{
		// Write the information for the current node.
		for (int i_tab = 0; i_tab < i_depth; i_tab++)
			rcon.Tab();

		// Write the type information.
		rcon.PutString(strTypeName(*this));
		rcon.Tab();

		rcon.Print((pdData.bOcclude) ? ("Y") : ("N"));
		rcon.Print((pdData.bCacheableVolume) ? ("Y") : ("N"));
		rcon.Print((pdData.bCacheable) ? ("Y") : ("N"));
		rcon.Print((pdData.bCacheIntersecting) ? ("Y") : ("N"));
		rcon.Print((pdData.bCastShadow) ? ("Y") : ("N"));
		rcon.Tab();
		//rcon.Print("%d", u4GetUniqueHandle());
		//rcon.Tab();

		// Write out the name of the instance.
		CInstance* pins = dynamic_cast<CInstance*>(((CPartition*)this));
		if (pins)
			rcon.Print(pins->strGetInstanceName());

		// Write the location information.
		if (pbvBoundingVol())
		{
			if (!bNoSpatialInfo())
			{
				CVector3<> v3_min, v3_max;
				pbvBoundingVol()->GetWorldExtents(pr3Presence(), v3_min, v3_max);
				rcon.Print("");
				//rcon.Print("(%.3f, %.3f, %.3f) (%.3f, %.3f, %.3f)", v3_min.tX, v3_min.tY, v3_min.tZ, v3_max.tX, v3_max.tY, v3_max.tZ);
			}

			if (ppartParent)
			{
				// Check whether we are really contained in parent.
				const CBoundVol* pbv_parent = ppartParent->pbvBoundingVol();
				Assert(pbv_parent);

				// Expand the parent presence scale to produce a more lenient test.
				CPresence3<> pr3_parent = ppartParent->pr3Presence();
				pr3_parent.rScale *= 1.01;

				switch (pbv_parent->esfSideOf(*pbvBoundingVol(), &pr3_parent, &pr3Presence()))
				{
					case esfINSIDE:
						// No prob.
						break;
					case esfINTERSECT:
						rcon.Print(" INTERSECT!");
						break;
					case esfOUTSIDE:
						rcon.Print(" OUTSIDE!");
						break;
					default:
						Assert(0);
				}
			}
		}

		rcon.NewLine();

		//
		// Recurse down the hierarchy and write the information.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		// Return if there are no children.
		if (!ppartc)
			return;

		// Iterate throught the container and write elements to disk.
		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			(*it)->Write(rcon, i_depth + 1);
	}
	
	//*****************************************************************************************
	void CPartition::MoveSingleInstancesUp()
	{
		TPartitionListChild partlist;

		// Get a pointer to the child list.
		CPartition* ppartc = ppartChildren();

		// Duplicate child list.
		if (ppartc)
		{
			iterator it = ppartc->begin();
			for (; it != ppartc->end(); ++it)
				partlist.push_back(*it);

			// Iterate throught the container recursively move instances up.
			for (partlist.Begin(); partlist.bIsNotEnd(); partlist++)
				(*it)->MoveSingleInstancesUp();
		}

		//
		// Get the parent and grandparent partitions.
		//
		CPartition* ppart_parent = ppartGetParent();

		// If this partition has no parent, do nothing.
		if (!ppart_parent)
			return;

		CPartition* ppart_grandparent = ppart_parent->ppartGetParent();

		// If this partition has no grandparent, do nothing.
		if (!ppart_grandparent)
			return;

		// If the current partition is an instance, look for siblings.
		if (bIsInstance())
		{
			// If this node has no siblings, move it to its grandparent.
			if (ppart_parent->iCountInstances() == 1)
				SetParent(ppart_grandparent);
		}
	}

	//*****************************************************************************************
	bool CPartition::bMakeChildList(TPartitionListChild& rpartlist_children)
	{
		bool b_has_children = false;	// Child flag.

		// Get a pointer to the child list.
		CPartition* ppartc = ppartChildren();

		// Duplicate child list.
		if (ppartc)
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			{
				rpartlist_children.push_back(*it);
				b_has_children = true;
			}
		return b_has_children;
	}

	//*****************************************************************************************
	void CPartition::Flatten(CPartition* ppart_top)
	{
		Assert(ppart_top);

		TPartitionListChild partlist_children;	// Copy of the child list.

		// Make a copy of the child list to prevent recursion from changing the list.
		if (bMakeChildList(partlist_children))
			// Recurse through tree, moving instances to the top level node.
			for (partlist_children.Begin(); partlist_children.bIsNotEnd(); partlist_children++)
				partlist_children.rtGet()->Flatten(ppart_top);

		if (ppart_top == this)
			// Remove empty spatial nodes.
			PruneEmpties();
		else
			// Move instances to the top node.
			if (bIsInstance())
				SetParent(ppart_top);
	}

	//*****************************************************************************************
	void CPartition::ClearHierarchyPointers()
	{
		ppartChild = 0;
		ppartParent = 0;
		ppartNext = 0;

	}


	//*****************************************************************************************
	void CPartition::TakeChildren(CPartition& rpart)
	{
		// If there is no child list from the starting partition, do nothing.
		if (rpart.iCountChildren() == 0)
			return;

		TPartitionListChild partlist_children;	// Copy of the child list.

		// Get the list of children from the starting partition.
		Assert(bValid());
		rpart.bMakeChildList(partlist_children);
		Assert(rpart.bValid());
		Assert(bValid());

		// Recurse through tree, moving objects to this partition's child list..
		for (partlist_children.Begin(); partlist_children.bIsNotEnd(); partlist_children++)
		{
			Assert(partlist_children.rtGet()->bValid());
			partlist_children.rtGet()->SetParent(this);
			Assert(bValid());
		}
	}

	//*****************************************************************************************
	void CPartition::TakeNonSpatialChildren(CPartition& rpart)
	{
		TPartitionListChild partlist;	// Copy of the child list.

		// Make a copy of the child list to prevent later operations from changing the list.
		Assert(bValid());
		rpart.bMakeChildList(partlist);
		Assert(rpart.bValid());
		Assert(bValid());

		// Iterate through children, moving offending partitions out.
		for (partlist.Begin(); partlist.bIsNotEnd(); partlist++)
		{
			CPartition* ppart = partlist.rtGet();
			Assert(ppart->bValid());
			if (ppart->bNoSpatialInfo() || !ppart->bIncludeInBuildPart() ||
				!ppart->pbvBoundingVol())
			{
				// Move the partition to the new list.
				ppart->SetParent(this);
				Assert(bValid());
			}
		}
	}

	//*****************************************************************************************
	int CPartition::iCountChildren() const
	{
		int i_count = 0;	// Count of the number of children.

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		// Iterate through the container and count child partitions.
		if (ppartc)
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it, i_count++);

		return i_count;
	}

	//*****************************************************************************************
	int CPartition::iCountAllChildren() const
	{
		int i_count = 1;	// Count of the number of children.

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		// Iterate through the container and count child partitions.
		if (ppartc)
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			{
				i_count += (*it)->iCountAllChildren();
			}

		return i_count;
	}

	//*****************************************************************************************
	bool CPartition::bNoSpatialInfo() const
	{
		// If the partition has no bounding volume, it has no spatial information.
		if (!pbvBoundingVol())
			return true;

		// If the partition has an infinite bounding volume, it has no spatial information.
		if (pbvBoundingVol()->fMaxExtent() > 100000.0f)
			return true;

		// Otherwise the partition has spatial information.
		return false;
	}

	//*****************************************************************************************
	void CPartition::AddChildren(CPartition& rpart)
	{
		TPartitionListChild partlist;	// Copy of the child list.

		// Make a copy of the child list to prevent later operations from changing the list.
		rpart.bMakeChildList(partlist);

		for (partlist.Begin(); partlist.bIsNotEnd(); partlist++)
		{
			// Move the partition to the new list.
			bAddChild(partlist.rtGet());
		}
	}

	//*****************************************************************************************
	bool CPartition::bContainsShape() const
	{
		//
		// This function calls its (virtual) pshGetShape(), and returns if not null.
		// Otherwise, it calls itself recursively for its children.  No derived class should have
		// need to override this function.
		//
		if (pshGetShape())
			return true;

		// Iterate through the container and count child partitions.
		CPartition* ppartc = (CPartition*)ppartChildren();
		if (ppartc)
		{
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			{
				if ((*it)->bContainsShape())
					return true;
			}
		}

		// All tests failed.
		return false;
	}
	
	//*****************************************************************************************
	float CPartition::fEstimateScreenSize(const CCamera& cam)
	{
		Assert(pbvBoundingVol());
		
		//
		// Rather than obtaining the world bounding sphere, assume that the sphere origin is
		// almost the same as the object origin.  This avoids transforming the point.
		//

		// Get the distance from the shape to the camera.
		float f_inv_distance_to_cam_sqr = fInverseLow((cam.v3Pos() - v3Pos()).tLenSqr());

		// Get the world radius.  
		float f_radius_of_shape = pbvBoundingVol()->fMaxExtent() * fGetScale();

		// Get the radius in screen pixels.
		f_radius_of_shape *= cam.campropGetProperties().fViewWidthRatio;
		
		// Return an estimate of the screen size of a potential new render cache object.
		return f_radius_of_shape * f_radius_of_shape * f_inv_distance_to_cam_sqr;
	}
	
	//*****************************************************************************************
	float CPartition::fEstimateScreenSizeChildren(const CCamera& cam)
	{
		float f_sum_screensize = 0.0f;

		// Get a pointer to the child list.
		CPartition* ppartc = ppartChildren();

		// Iterate through the container and count child partitions.
		if (ppartc)
		{
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			{
				f_sum_screensize += (*it)->fEstimateScreenSize(cam);
				f_sum_screensize += (*it)->fEstimateScreenSizeChildren(cam);
			}
		}

		// Return an estimate of the screen size of a potential new render cache object.
		return f_sum_screensize;
	}
	
	//*****************************************************************************************
	float CPartition::fEstimateScreenSize(const CCamera& cam, float f_distance_sqr)
	{
		Assert(pbvBoundingVol());
		
		//
		// Rather than obtaining the world bounding sphere, assume that the sphere origin is
		// almost the same as the object origin.  This avoids transforming the point.
		//

		// Get the distance from the shape to the camera.
		float f_inv_distance_to_cam_sqr = fInverseLow(f_distance_sqr);

		// Get the world radius.  
		float f_radius_of_shape = pbvBoundingVol()->fMaxExtent() * fGetScale();

		// Get the radius in screen pixels.
		f_radius_of_shape *= cam.campropGetProperties().fViewWidthRatio;
		
		// Return an estimate of the screen size of a potential new render cache object.
		return f_radius_of_shape * f_radius_of_shape * f_inv_distance_to_cam_sqr;
	}

	//*****************************************************************************************
	// this function is similar to the above in that it cannot be moving itself
	bool CPartition::bContainsMovingObject() const
	{
		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		if (ppartc)
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				if ((*it)->bContainsMovingObject())
					return true;

		// All tests failed.
		return false;
	}

	//******************************************************************************************
	void CPartition::PreRender(CRenderContext& renc)
	{
		// Base CPartition does nothing.
	}

	//*****************************************************************************************
	bool CPartition::bGetBoundingBoxVerticesCam(CVector3<> av3[], const CCamera& cam) const
	{
		Assert(av3);

		// Store a flag indicating a true bounding box or not.
		bool b_retval = bGetBoundingBoxVertices(av3);
		
		//
		// Build an object space to camera space transform.
		//
		CTransform3<> tf3_to_cam = pr3Presence() * cam.tf3ToNormalisedCamera();

		//
		// Transform and store all the corners of the box.
		//
		for (uint u_corner = 0; u_corner < 8; u_corner++)
		{
			av3[u_corner] = av3[u_corner] * tf3_to_cam;
		}

		// Return the flag indicating if a true bounding box was used or not.
		return b_retval;
	}

	//*****************************************************************************************
	bool CPartition::bGetBoundingBoxVertices(CVector3<> av3[]) const
	{
		Assert(av3);
		Assert(pbvBoundingVol());

		const CBoundVol* pbv = pbvBoundingVol();	// Pointer to the partition's bounding
													// volume.
		CBoundVolBox     bvb;						// Temporary bounding box to use if the
													// partition doesn't already have one.

		//
		// Obtain a valid pointer to a bounding box associated with this partition.
		//

		// Get a bounding box if there is one.
		const CBoundVolBox* pbvb = pbv->pbvbCast();

		// Store a flag indicating a true bounding box or not.
		bool b_retval = pbvb != 0;

		// Make a bounding box with the bounding sphere if there is no current bounding box.
		if (!pbvb)
		{
			// Create the temporary bounding box from the partition's bounding sphere.
			TReal r_dim = (TReal)pbv->fMaxExtent() * 2.0f;
			bvb = CBoundVolBox(r_dim, r_dim, r_dim);

			// Assign the pointer to the temporary bounding box.
			pbvb = &bvb;
		}

		//
		// Store all the corners of the box.
		//
		for (uint u_corner = 0; u_corner < 8; u_corner++)
		{
			av3[u_corner] = (*pbvb)[u_corner];
		}

		// Return the flag indicating if a true bounding box was used or not.
		return b_retval;
	}

	//*****************************************************************************************
	bool CPartition::bGetWorldExtents(CVector3<>& v3_min, CVector3<>& v3_max) const
	{
		if (!bNoSpatialInfo() && bIncludeInBuildPart())
		{
			// Return this partition's particular extents.
			pbvBoundingVol()->GetWorldExtents(pr3Presence(), v3_min, v3_max);
			return true;
		}

		// This partition does not have finite spatial extent.
		// Therefore, we must combine the extents of its children.

		bool b_any = false;

		CPartition* ppartc = (CPartition*)ppartChildren();
		if (ppartc)
		{
			for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			{
				if (!b_any)
					// Get the extents of the first child.
					b_any = (*it)->bGetWorldExtents(v3_min, v3_max);
				else
				{
					CVector3<> v3_min_loc, v3_max_loc;
					if ((*it)->bGetWorldExtents(v3_min_loc, v3_max_loc))
					{
						// Combine these extents with previous.
						SetMin(v3_min.tX, v3_min_loc.tX);
						SetMin(v3_min.tY, v3_min_loc.tY);
						SetMin(v3_min.tZ, v3_min_loc.tZ);

						SetMax(v3_max.tX, v3_max_loc.tX);
						SetMax(v3_max.tY, v3_max_loc.tY);
						SetMax(v3_max.tZ, v3_max_loc.tZ);

						b_any = true;
					}
				}
			}
		}

		return b_any;
	}

	//*****************************************************************************************
	void CPartition::GetBoundingScreenBox(const CCamera& cam, CVector3<>& rv3_min,
		                                     CVector3<>&rv3_max) const
	{
		CVector3<> av3_cam[8];		// Array of coordinates for the camera space bounding box.
		CVector3<> av3_screen[8];	// Array of coordinates for the screen space bounding box.

		// Get the camera space bounding box.
		bGetBoundingBoxVerticesCam(av3_cam, cam);

		// Project the points for the bounding box.
		uint u_corner;
		for (u_corner = 0; u_corner < 8; u_corner++)
		{
			av3_screen[u_corner] = cam.ProjectPoint(av3_cam[u_corner]);
			av3_screen[u_corner].tZ = av3_cam[u_corner].tY;
		}

		//
		// Find the minimum and maximum screen projections.
		//

		// Start with the first corner.
		rv3_min = av3_screen[0];
		rv3_max = rv3_min;

		// Do the remaining corners.
		for (u_corner = 1; u_corner < 8; u_corner++)
		{
			TReal r_x = av3_screen[u_corner].tX;
			TReal r_y = av3_screen[u_corner].tY;
			TReal r_z = av3_screen[u_corner].tZ;

			// Set the minimum values.
			if (r_x < rv3_min.tX)
				rv3_min.tX = r_x;
			if (r_y < rv3_min.tY)
				rv3_min.tY = r_y;
			if (r_z < rv3_min.tZ)
				rv3_min.tZ = r_z;

			// Set the maximum values.
			if (r_x > rv3_max.tX)
				rv3_max.tX = r_x;
			if (r_y > rv3_max.tY)
				rv3_max.tY = r_y;
			if (r_z > rv3_max.tZ)
				rv3_max.tZ = r_z;
		}
	}

	//*****************************************************************************************
	TReal CPartition::rGetBoundingScreenArea(const CCamera& cam) const
	{
		return ((CPartition*)this)->fEstimateScreenSize(cam);
		/*
		CVector3<> v3_min;	// Minimum coordinate for the screen rectangle.
		CVector3<> v3_max;	// Maximum coordinate for the screen rectangle.

		// Obtain the screen rectangle.
		GetBoundingScreenBox(cam, v3_min, v3_max);

		// Return the screen area.
		CVector3<> v3_dim = v3_max - v3_min;
		return v3_dim.tX * v3_dim.tY;
		*/
	}

	//*****************************************************************************************
	TReal CPartition::rGetBoundingScreenAreaChildInstances(const CCamera& cam) const
	{
		TReal r_area = TReal(0);

		// If the object is an instance, add its area to the total.
		if (bIsInstance())
			r_area += rGetBoundingScreenArea(cam);

		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		if (ppartc)
		{
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				r_area += (*it)->rGetBoundingScreenAreaChildInstances(cam);
		}

		// Return the total area.
		return r_area;
	}

	//*****************************************************************************************
	CInstance* CPartition::pinsFindNamedInstance
	(
		std::string str_name,
		std::string str_class
	) const
	{
		uint32 u4_handle = u4Hash(str_name.c_str());

		return pinsFindInstance(u4_handle);

		/*

		// If we have an instance, check the name.
		CInstance* pins = ptCast<CInstance>(const_cast<CPartition*>(this));
		if (pins)
		{
			if (!str_class.length() || strTypeName(*pins) == str_class)
				if (strcmp(pins->strGetInstanceName(), str_name.c_str()) == 0)
					return pins;
		}

		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		if (ppartc)
		{
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			{
				pins = (*it)->pinsFindNamedInstance(str_name, str_class);
				if (pins)
					return pins;
			}
		}

		// Return a failure.
		return 0;

  */
	}


	//*****************************************************************************************
	CInstance* CPartition::pinsFindInstance
	(
		uint32 u4_handle	// The handle of the instance we seek
	) const
	{
		// If we have an instance, check the handle.
		CInstance* pins = ptCast<CInstance>(const_cast<CPartition*>(this));
		if (pins)
		{
			if (u4_handle == pins->u4GetUniqueHandle())
				return pins;
		}

		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		if (ppartc)
		{
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			{
				pins = (*it)->pinsFindInstance(u4_handle);
				if (pins)
					return pins;
			}
		}

		// Return a failure.
		return 0;
	}

	//*****************************************************************************************
	//
	CInstance* CPartition::pinsFindInstance
	(
		std::string str_name					// Name of instance
	) const	
	{
		uint32 u4_handle = u4Hash(str_name.c_str());

		return pinsFindInstance(u4_handle);
	}

	//*****************************************************************************************
	int CPartition::iCountInstances()
	{
		int i_count = 0;
		bool b_can_cache = false;

		// If this object is an instance, add it to the count.
		if (bIsInstance())
		{
			++i_count;
			b_can_cache = true;
		}

		//
		// Recurse through children summing their instance count.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		if (ppartc)
		{
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			{
				i_count += (*it)->iCountInstances();
			}
		}

		// Return count.
		return i_count;
	}

	//*****************************************************************************************
	TReal CPartition::rGetWorldVolume()
	{
		Assert(pbvBoundingVol());

		return pbvBoundingVol()->rGetVolume(fGetScale());
	}
	
	//*****************************************************************************************
	void CPartition::SortChildrenByVolume()
	{
		std::multiset<CPartVol, CPartVol>           mset_pvol;	// Multiset.
		std::multiset<CPartVol, CPartVol>::iterator itmset;		// Multiset iterator.

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		// Do nothing if there are no children.
		if (!ppartc)
			return;

		// Insert children into the new sorted list in order.
		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
		{
			// Get the partition from the old list.
			CPartition* ppart = *it;

			// Sort the partition into the new list.
			mset_pvol.insert(CPartVol(ppart));
		}

		// Clear the child list.
		//ppartc->erase(ppartc->begin(), ppartc->end());
		ppartChild = 0;

		// Copy the sorted children back into the partition's child list.
		for (itmset = mset_pvol.begin(); itmset != mset_pvol.end(); ++itmset)
		{
			// Get the partition from the sorted list.
			CPartition* ppart = (*itmset).ppartPartition;
			Assert(ppart);

			// Put the partition into the child list.
			PushChildToList(ppart);
		}
	}

	//*****************************************************************************************
	void CPartition::StuffChildren()
	{
		CPartitionSpace part;	// Dummy partition.

		// Organize children from the smallest to the largest.
		SortChildrenByVolume();

		// Copy the children to a separate list.
		part.TakeChildren(*this);

		CPartition* ppartc_source = (CPartition*)part.ppartChildren();

		// Do nothing if there are no children.
		if (!ppartc_source)
			return;

		// Get a pointer to the child list.
		CPartition* ppartc_dest = (CPartition*)ppartChildren();

		while (ppartc_source && ppartc_source->ppartNext)
		{
			// Get the first partition and remove it from the list.
			CPartition* ppart = ppartc_source;
			CVector3<>  v3    = ppart->v3Pos();
			part.RemoveChildFromList(ppart);
			ppartc_source = (CPartition*)part.ppartChildren();

			//
			// Attempt to stuff it into a child if the child has a volume and the
			// extents of that volume are less than a given amount.
			//
			for (iterator it = ppartc_source->begin(); it != ppartc_source->end(); ++it)
			{
				// Get the target child partition.
				CPartition* ppart_targ = *it;

				//
				// Skip if the centre of the selected partition is not inside the target
				// partition.
				//
			#if bPARTITION_BUILD
				if (v3.tX < ppart_targ->v3WorldMin.tX || v3.tY < ppart_targ->v3WorldMin.tY ||
					v3.tZ < ppart_targ->v3WorldMin.tZ)
					continue;
				if (v3.tX > ppart_targ->v3WorldMax.tX || v3.tY > ppart_targ->v3WorldMax.tY ||
					v3.tZ > ppart_targ->v3WorldMax.tZ)
					continue;
			#endif // bPARTITION_BUILD

				// If the target partition can potentially have children, try and stuff it.
				if (ppart_targ->bCanHaveChildren())
				{
					// If it can be stuffed, stop here.
					if (ppart_targ->bAddChild(ppart))
					{
						ppart = 0;
						break;
					}
				}
			}

			//
			// If the attempt to stuff it failed, just transfer it to the original partition's
			// child list.
			//
			if (ppart)
				ppart->SetParent(this);
		}

		// Push back the sole remaining child partition.
		CPartition* ppart = ppartc_source;
		part.RemoveChildFromList(ppart);
		bAddChild(ppart);
	}

	//*****************************************************************************************
	//
	void CPartition::PushChildToList
	(
		CPartition* ppart	// Child to be added.
	)
	//
	// Pushes the child onto the child list.
	//
	//**************************
	{
	#if bREVERSE_LIST_ORDER
		ppart->ppartNext = 0;
		if (!ppartChild)
		{
			ppartChild = ppart;
			return;
		}
		CPartition* ppart_current = ppartChild;
		while (ppart_current->ppartNext)
			ppart_current = ppart_current->ppartNext;
		ppart_current->ppartNext = ppart;
	#else
		ppart->ppartNext = ppartChild;
		ppartChild = ppart;
	#endif // bREVERSE_LIST_ORDER
	}

	//*****************************************************************************************
	//
	void CPartition::RemoveChildFromList
	(
		CPartition* ppart	// Child to be removed.
	)
	//
	// Removes the child from the child list of this partition.
	//
	//**************************
	{
		// Do nothing if nothing is to be removed.
		if (ppart == 0)
			return;
		if (ppartChild == 0)
			return;

		//
		// If the first child is the same as the child to be removed, remove it. Otherwise
		// search for a sibling of the first child that matches the child to be removed.
		//
		if (ppart == ppartChild)
		{
			ppartChild = ppartChild->ppartNext;
			ppart->ppartNext   = 0;
			ppart->ppartParent = 0;
			return;
		}
		CPartition* ppart_current = ppartChild;
		while (ppart_current)
		{
			// If the next pointer matches, remove it.
			if (ppart == ppart_current->ppartNext)
			{
				ppart_current->ppartNext = ppart_current->ppartNext->ppartNext;
				ppart->ppartNext   = 0;
				ppart->ppartParent = 0;
				return;
			}
			ppart_current = ppart_current->ppartNext;
		}
	}
	
	//*****************************************************************************************
	int CPartition::iSizeOf() const
	{
		return sizeof(CPartition);
	}

	//*****************************************************************************************
	bool CPartition::bIsPureSpatial() const
	{
		return false;
	}
	
	//
	// Preloads object in
	//*****************************************************************************************
	bool CPartition::bPreload(const CBoundVol* pbv, const CPresence3<> *ppr3, bool b_is_contained)
	{
		ESideOf esf;

	#if bLOG_PRELOADER
		// Log preload requests.
		if (!conPreLoader.bFileSessionIsOpen())
			conPreLoader.OpenFileSession("Preload.txt");
	#endif // bLOG_PRELOADER

		//
		// Find:
		//		Does the specified bounding volume contain this partition?
		//		Does the specified bounding volume and this partition intersect?
		//
		if (b_is_contained)
		{
			esf = esfINSIDE;
		}
		else
		{
			if (!pbv)
			{
				// If no bounding volume exists, containment is assumed.
				esf = esfINSIDE;
			}
			else
			{
				// If no bounding volume exists for this partition, containment is assumed.
				const CBoundVol* pbv_this = pbvBoundingVol();
				if (!pbv_this)
				{
					esf = esfINTERSECT;
				}
				else
				{
					// Make a unit presence for the specified bounding volume.
					//CPresence3<> pr3;
					// Always use spheres, as accuracy isn't very important, but speed is.

					esf = pbv->bvsGetBoundingSphere().esfSideOf
					(
						pbv_this->bvsGetBoundingSphere(),
						ppr3,			// Target presence.
						&pr3Presence()	// Current presence.
					);
				}			
			}
		}

		//
		// If the partition is outside the specified bounding volume there is no requirement
		// for preloading.
		//
		if (esf == esfOUTSIDE)
			return false;

	#if bLOG_PRELOADER
		PrintPos(conPreLoader, this);
	#endif

		// Preload the partition if it is an instance.
		CInstance* pins;
		Cast(&pins);
		if (pins)
			// Go ahead and touch it- it should already be in memory.
			pins->OnPrefetch(false);
//			pddDataDaemon->Prefetch(pins);

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		// Do nothing if there are no children.
		if (!ppartc)
			return true;

		// Recursively preload children.
		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
		{
			(*it)->bPreload(pbv, ppr3, esf == esfINSIDE);
		}
		return true;
	}

	//*****************************************************************************************
	char * CPartition::pcSaveHierarchy(char* pc) const
	{
		// Save this handle.
		uint32 u4_this = u4GetUniqueHandle();

		// Do we have a valid handle?
		if (u4_this)
		{
			// Yes!  Save this and parent.
			pc = pcSaveT(pc, u4_this);

			// Save parent handle.
			uint32 u4_parent = 0;
			if (ppartParent)
				u4_parent = ppartParent->u4GetUniqueHandle();
			pc = pcSaveT(pc, u4_parent);

//			AlwaysAssert(u4_this != u4_parent);
		}

		// Always save child data.
		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		// Return if there is nothing left to do.
		if (!ppartc)
			return pc;

		// Save the children.
		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
		{
			pc = (*it)->pcSaveHierarchy(pc);
		}

		return pc;
	}

	//*****************************************************************************************
	char * CPartition::pcSave(char* pc) const
	{
		// Save flags.
		// As of version 13, do not save flags.
		pc = pcSaveT(pc, pdData);
		return pc;
	}

	//*****************************************************************************************
	const char * CPartition::pcLoad(const char* pc)
	{
		// As of version 13, do not load flags.
		if (CSaveFile::iCurrentVersion < 13 || CSaveFile::iCurrentVersion > 16)
		{
			SPartitionData pd;

			// Snag the flags.
			pc = pcLoadT(pc, &pd);

			// Only set the visibilty state for now.
			if (CSaveFile::iCurrentVersion > 16)
				SetVisible(pd.bVisible);
		}
		return pc;
	}

	//*****************************************************************************************
	char* CPartition::pcSaveSpatial(char * pc) const
	{
		// We are not spatial, so save NO DATA for "this."

		// Save the children!

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		// Return if there is nothing left to do.
		if (!ppartc)
			return pc;

		// Save the children.

		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
		{
			pc = (*it)->pcSaveSpatial(pc);
		}

		return pc;
	}

	//*****************************************************************************************
	uint32 CPartition::u4GetUniqueHandle() const
	{
		// This is a bad handle!  Don't use it!
		Assert(false);
		return (uint32)this;
	}

	//*****************************************************************************************
	//
	bool CPartition::bInRange
	(
		float f_distance_sqr
	) const
	//
	// Returns 'true' if the value is within the 'fCullDistanceSqr' data member's range.
	//
	//**************************************
	{
		Assert(f_distance_sqr >= 0.0f);
		Assert(float(sfCullDistanceSqr) >= 0.0f);

		float f_dist_threshold = float(sfCullDistanceSqr) * fCullDistanceCombined;

		return u4FromFloat(f_distance_sqr) < u4FromFloat(f_dist_threshold);
	}

	//*****************************************************************************************
	//
	bool CPartition::bInRangeShadow
	(
		float f_distance_sqr
	) const
	//
	// Returns 'true' if the value is within the 'fCullDistanceShadowSqr' data member's range.
	//
	//**************************************
	{
		Assert(f_distance_sqr >= 0.0f);
		Assert(float(sfCullDistanceShadowSqr) >= 0.0f);

		float f_dist_threshold = float(sfCullDistanceShadowSqr) * fCullDistanceCombinedShadow;

		return u4FromFloat(f_distance_sqr) < u4FromFloat(f_dist_threshold);
	}

#if bPARTITION_BUILD
	
	//*****************************************************************************************
	void CPartition::SetWorldExtents()
	{
		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		if (ppartc)
		{
			for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				(*it)->SetWorldExtents();
		}

		if (pbvBoundingVol()->ebvGetType() == ebvINFINITE)
		{
			//AlwaysAssert(ppartc);
			iterator it = ppartc->begin();
			v3WorldMin = (*it)->v3WorldMin;
			v3WorldMax = (*it)->v3WorldMax;
			++it;

			for (; it != ppartc->end(); ++it)
			{
				v3WorldMin.SetMin((*it)->v3WorldMin);
				v3WorldMax.SetMin((*it)->v3WorldMax);
			}
		}
		else
		{
			// Calculate the world extents.
			pbvBoundingVol()->GetWorldExtents(pr3Presence(), v3WorldMin, v3WorldMax);

			// Fudge the coordinates slightly.
			v3WorldMin.tX -= 0.01f;
			v3WorldMin.tY -= 0.01f;
			v3WorldMin.tZ -= 0.01f;
			v3WorldMax.tX += 0.01f;
			v3WorldMax.tY += 0.01f;
			v3WorldMax.tZ += 0.01f;
		}
		// Calculate the volume.
		CVector3<> v3 = v3WorldMax - v3WorldMin;
		fVolumeScore = Abs(v3.tX * v3.tY * v3.tZ);
	}

#endif // bPARTITION_BUILD

	//*****************************************************************************************
	bool CPartition::bValid() const
	{
		// Obvious test: is the 'this' pointer valid.
		if (!this)
		{
			AlwaysAssert(0);
			return false;
		}

		//
		// Test to see if the virtual table contains valid addresses. For current applications
		// it is sufficient to test that table entries are non-zero.
		//
		{
			uint32* pu4 = (uint32*)pvGetVTable((void*)this);
			if (!pu4)
			{
				AlwaysAssert(0);
				return false;
			}
			if (!*pu4)
			{
				AlwaysAssert(0);
				return false;
			}
		}

		//
		// Test children.
		//
		{
			CPartition* ppartc = (CPartition*)ppartChildren();

			if (ppartc)
			{
				for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				{
					if (!(*it))
					{
						AlwaysAssert(0);
						return false;
					}
					if (!(*it)->bValid())
					{
						AlwaysAssert(0);
						return false;
					}
				}
			}
		}
		return bValidV();
	}

	//*****************************************************************************************
	bool CPartition::bValidV() const
	{
		return true;
	}

	//*****************************************************************************************
	void CPartition::IncrementFrameCount()
	{
		++SPartitionSettings::u4FrameNumber;
	}
	
	//*****************************************************************************************
	void CPartition::ResetFrameCounts()
	{
		u4NoCacheUntilFrame = 0;

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		if (ppartc)
		{
			iterator it = ppartc->begin();
			for (; it != ppartc->end(); ++it)
			{
				(*it)->ResetFrameCounts();
			}
		}
	}


	//*****************************************************************************************
	//
	// Member function implementations used for image caching.
	//

	//*****************************************************************************************
	void CPartition::CreateRenderCache(const CCamera& cam)
	{
		// Delete the previous cache associated with this partition if there is one.
		DeleteRenderCache();

		// Create the new cache.
		prencCache = new CRenderCache(this, cam);
	}

	//*****************************************************************************************
	void CPartition::DeleteRenderCache()
	{
		delete prencCache;
		prencCache = 0;
	}

	//*****************************************************************************************
	bool CPartition::bCanCache()
	{
		// Determine if the partition can be cached.
		bool b_can_cache =	pdData.bCacheable && !pdData.bSimpleObject && !pdData.bAlwaysFace &&
			                u4NoCacheUntilFrame < SPartitionSettings::u4FrameNumber &&
							pdData.bCacheableVolume
	#if (BUILDVER_MODE != MODE_FINAL)
							&& rcsRenderCacheSettings.erctMode != ercmCACHE_OFF
	#endif
							;

		// If the partition can be cached, return true;
		if (b_can_cache)
			return true;

		if (prencCache)
		{
			// Ignore if the partition is already included for the current frame.
			if (!prencCache->bIsCurrentFrameKey())
			{
				// If the partition cannot be cached, make sure no cache exists and return false;
				DeleteAllChildCaches();
			}
		}
		return false;
	}

	//*****************************************************************************************
	void CPartition::DeleteAllChildCaches()
	{
		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		if (!ppartc)
			return;

		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			(*it)->DeleteAllCaches();
	}

	//*****************************************************************************************
	void CPartition::DeleteAllCaches()
	{
		// Delete a possible cache belonging to the current node.
		DeleteRenderCache();

		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = (CPartition*)ppartChildren();

		if (!ppartc)
			return;

		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			(*it)->DeleteAllCaches();
	}

	//*****************************************************************************************
	void CPartition::SetCacheNoBuild(int i_frame_delay)
	{
		u4NoCacheUntilFrame = SPartitionSettings::u4FrameNumber + uint32(i_frame_delay);

		//
		// Recursively call the parent.
		//
		if (ppartParent)
		{
			ppartParent->SetCacheNoBuild(i_frame_delay);
		}
	}

	//*****************************************************************************************
	void CPartition::SetCacheNoBuildPart(const CPartition* ppart, int i_frame_delay)
	{
		Assert(ppart);

		//
		// If the moving partition does not intersect with the existing partition, do nothing.
		//
		if (esfSideOf(ppart) == esfOUTSIDE)
			return;

		//
		// Mark the current partition so that it will not build an image cache for the next
		// couple of frames.
		//
		u4NoCacheUntilFrame = SPartitionSettings::u4FrameNumber + uint32(i_frame_delay);

		//
		// Recursively call this function on the children of this partition.
		//
		CPartition* ppartc = (CPartition*)ppartChildren();
		if (ppartc)
		{
			iterator it = ppartc->begin();
			for (; it != ppartc->end(); ++it)
			{
				(*it)->SetCacheNoBuildPart(ppart, i_frame_delay);
			}
		}
	}

	//*****************************************************************************************
	void CPartition::InvalidateRenderCache(const CPartition* ppart)
	{
		//
		// Test for intersection.
		//
		if (esfSideOf(ppart) == esfOUTSIDE)
			return;

		// Invalidate this render cache if there is one.
		if (prencGet())
		{
			InvalidateAllChildCaches();
			DeleteRenderCache();
			return;
		}

		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = ppartChildren();

		if (!ppartc)
			return;

		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			(*it)->InvalidateRenderCache(ppart);
	}

	//*****************************************************************************************
	void CPartition::InvalidateAllChildCaches()
	{
		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = ppartChildren();

		if (!ppartc)
			return;

		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
		{
			(*it)->InvalidateAllChildCaches();
			if ((*it)->prencGet())
				(*it)->prencGet()->Invalidate();
		}
	}

	//*****************************************************************************************
	TReal CPartition::rGetCylinderRadius() const
	{
		return fSqrtEst(float(sfCylinderRadiusSqr));
	}

	//*****************************************************************************************
	TReal CPartition::rGetSphereRadius() const
	{
		return fSqrtEst(float(sfSphereRadiusSqr));
	}

	//*****************************************************************************************
	TReal CPartition::rGetCylinderRadiusSqr() const
	{
		return float(sfCylinderRadiusSqr);
	}

	//*****************************************************************************************
	TReal CPartition::rGetSphereRadiusSqr() const
	{
		return float(sfSphereRadiusSqr);
	}

	//*****************************************************************************************
	void CPartition::FixCullDistance(float f_cull_distance)
	{
		pdData.bFixedCullDistance = 1;
		sfCullDistanceSqr = f_cull_distance * f_cull_distance;
	}

	//*****************************************************************************************
	void CPartition::FixCullDistanceShadow(float f_cull_distance)
	{
		pdData.bFixedCullShadow = 1;
		sfCullDistanceShadowSqr = f_cull_distance * f_cull_distance;
	}
	
	//*****************************************************************************************
	void CPartition::SetCombinedCulling(float f_cull, float f_cull_shadow)
	{
		fCullDistanceCombined       = f_cull;
		fCullDistanceCombinedShadow = f_cull_shadow;
	}
	
	//*****************************************************************************************
	void CPartition::GetCombinedCulling(float& rf_cull, float& rf_cull_shadow)
	{
		rf_cull        = fCullDistanceCombined;
		rf_cull_shadow = fCullDistanceCombinedShadow;
	}
	
	//*****************************************************************************************
	float CPartition::fGetAreaEfficiencyThreshold()
	{
		return float(sqrt(fAreaEfficiencyThresholdSqr));
	}

	//*****************************************************************************************
	void CPartition::SetAreaEfficiencyThreshold(float f_area_efficiency)
	{
		Assert(f_area_efficiency > 0.0f);
		fAreaEfficiencyThresholdSqr = f_area_efficiency * f_area_efficiency;
	}
	
	//*****************************************************************************************
	float CPartition::fDistanceFromGlobalCameraSqr() const
	{
		float f_x = SPartitionSettings::v3GlobalCameraPosition.tX - v3Pos().tX;
		float f_y = SPartitionSettings::v3GlobalCameraPosition.tY - v3Pos().tY;
		return f_x * f_x + f_y * f_y;
	}

	//*****************************************************************************************
	void CPartition::SimplifyChild()
	{
	}

	//*****************************************************************************************
	void CPartition::RemoveUnwanted()
	{
	}
	
	//*****************************************************************************************
	bool CPartition::bCacheGroup(float f_distance_sqr) const
	{
		// Distance below which group caches will not form.
		const float fCacheSingleDistance    = 25.0f;
		const float fCacheSingleDistanceSqr = fCacheSingleDistance * fCacheSingleDistance;

		// If this partition has a mesh, then it has the perfect efficiency for caching.
		if (pdData.bUsesMesh)
			return true;

		// Prevent groups caches below threshold.
		if (u4FromFloat(f_distance_sqr) < u4FromFloat(fCacheSingleDistanceSqr))
			return false;

		//
		// To do:
		//		Lump as many of the cache tests in here as possible.
		//

		//
		// 
		//
		//return true;
		float f = fAreaEfficiencyThresholdSqr * f_distance_sqr;
		return sfAreaEfficiencySqr.u4Get() < u4FromFloat(f);
	}
	
	//*****************************************************************************************
	void CPartition::SetFlagAlwaysFace(bool b_always_face)
	{
		pdData.bAlwaysFace = b_always_face;
	}

	//*****************************************************************************************
	void CPartition::SetCacheMultiplier(float f_cache_multiplier)
	{
		sfCacheMultiplier = f_cache_multiplier;
	}
	
	//*****************************************************************************************
	void CPartition::InitializeDataVolatile()
	{
	}

	//*****************************************************************************************
	void CPartition::SetVisible(bool b_visible)
	{
		if (d3dDriver.bUseD3D())
		{
			if (pdData.bSoftwareOnly)
			{
				pdData.bVisible = false;
				return;
			}
		}
		else
		{
			if (pdData.bHardwareOnly)
			{
				pdData.bVisible = false;
				return;
			}
		}
		pdData.bVisible = b_visible;
	}
	
	//*****************************************************************************************
	void CPartition::InitializeDataStatic()
	{
		CAnimal* pani = 0;

		//
		// First recurse through children to set all flags and data members. Some flag states
		// for the current partition may rely on flag states set for its children.
		//
		{
			// Get a pointer to the child list and iterate through it.
			CPartition* ppartc = (CPartition*)ppartChildren();
			if (ppartc)
				for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
					(*it)->InitializeDataStatic();
		}

		// Set constantly changing data members.
		u4NoCacheUntilFrame = 0;

		//
		// Set static values.
		//
		// Notes:
		//		The culling distance must be set after setting the cylinder radius.
		//
		pdData.bUsesMesh = pshGetShape() != 0;
		priv_self.SetSphereRadius(priv_self.fValueSphereRadius());
		priv_self.SetCylinderRadius(priv_self.fValueCylinderRadius());
		priv_self.SetCullDistance(priv_self.fValueCullDistance());
		priv_self.SetCullDistanceShadow(priv_self.fValueCullDistanceShadow());
		SetCacheMultiplier(priv_self.fGetMaxCacheMultiplier());

		// Measure area stuff.
		sfAreaEfficiencySqr = priv_self.fGetAreaEfficiencySqr();

		// Make sure distances are correct.
		if (sfCullDistanceSqr.u2Get() < sfCylinderRadiusSqr.u2Get())
			sfCullDistanceSqr = float(sfCylinderRadiusSqr);
		
		if (sfCullDistanceShadowSqr.u2Get() < sfCylinderRadiusSqr.u2Get())
			sfCullDistanceShadowSqr = float(sfCylinderRadiusSqr);

		Assert(float(sfCullDistanceSqr) > 0.0f);
		Assert(float(sfCullDistanceShadowSqr) > 0.0f);

		//
		// Set the partition flags.
		//
		// Notes:
		//		The bCacheable flag must be set before using the bFlagCacheableVolume call to
		//		set the bCacheableVolume flag.
		//
		pdData.bCacheable         = priv_self.bFlagCacheable();

		// Hack to allow animals to be cacheable.
		if (pdData.bCacheable == 0)
		{
			Cast(&pani);
			if (pani)
			{
				rptr<CMesh> pmsh = pmshGetMesh();
				AlwaysAssert(pmsh);
				if (pmsh)
					pmsh->bCacheableMesh = true;
				pdData.bCacheable = true;
			}
		}

		pdData.bCacheIntersecting = priv_self.bFlagCacheIntersecting();
		pdData.bCacheableVolume   = priv_self.bFlagCacheableVolume();
		pdData.bOcclude           = priv_self.bFlagOcclude();
		pdData.bCastShadow        = priv_self.bFlagCastShadow();
		pdData.bSimpleObject      = priv_self.bFlagSimpleObject();
		pdData.iPriority          = priv_self.iGetPriority();
		pdData.bHardwareAble      = priv_self.bHardwareAble();

		// Billboard objects should not be render cached if hardware is available for rendering.
		if (d3dDriver.bUseD3D() && pdData.bHardwareAble && pdData.bSimpleObject && pdData.bAlwaysFace)
		{
			pdData.bHardwareReq       = true;
			pdData.bCacheable         = false;
			pdData.bCacheIntersecting = false;
			pdData.bCastShadow        = false;
			pdData.bCacheableVolume   = false;
		}

		//
		// Determine if the objects that are specific to hardware or software renderering are
		// visible.
		//
		if (d3dDriver.bUseD3D())
		{
			if (pdData.bSoftwareOnly)
			{
				pdData.bVisible = false;
			}
		}
		else
		{
			if (pdData.bHardwareOnly)
			{
				pdData.bVisible = false;
			}
		}
	}
	
	//*****************************************************************************************
	void CPartition::InitializeDataStaticTerrain()
	{
		//
		// First recurse through children to set all flags and data members. Some flag states
		// for the current partition may rely on flag states set for its children.
		//
		{
			// Get a pointer to the child list and iterate through it.
			CPartition* ppartc = (CPartition*)ppartChildren();
			if (ppartc)
				for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
					(*it)->InitializeDataStaticTerrain();
		}

		// Set constantly changing data members.
		u4NoCacheUntilFrame = 0;

		//
		// Set static values.
		//
		// Notes:
		//		The culling distance must be set after setting the cylinder radius.
		//
		pdData.bUsesMesh = pshGetShape() != 0;
		priv_self.SetSphereRadius(priv_self.fValueSphereRadius());
		priv_self.SetCylinderRadius(priv_self.fValueCylinderRadius());
		priv_self.SetCullDistance(priv_self.fValueCullDistance());

		// Relevant flags.
		pdData.bHardwareAble = false;
	}

	//*****************************************************************************************
	void CPartition::GetMinMaxDistance(float& rf_min, float& rf_max, CShapePresence& rsp)
	{
		CVector3<> av3[8];

		bGetBoundingBoxVertices(av3);


		// Get the camera's position in object space.
		CVector3<> v3_cam = rsp.pr3GetCamShape().v3Pos;

		// Get the near and far corners.
		uint u_near_corner = uGetBoxNearestCorner(v3_cam);
		uint u_far_corner  = 0x7 - u_near_corner;

		rf_min = (av3[u_near_corner] - v3_cam).tManhattanDistance();
		rf_max = (av3[u_far_corner]  - v3_cam).tManhattanDistance();

	#if 0
		priv_self.VerifyGetMinMaxDistance(rf_min, rf_max, rsp);
	#endif

		// Scale to world coordinates.
		float f_scale = rsp.pr3GetShapeWorld().rScale;
		rf_min *= f_scale;
		rf_max *= f_scale;
	}

	//*****************************************************************************************
	bool CPartition::bIsEqualToSpatial(const CPartition* ppart) const
	{
		// This is not a spatial partition.
		return false;
	}

	//*****************************************************************************************
	void CPartition::RemoveDuplicates()
	{
		// Look for a duplicate of self.
		if (bIsPureSpatial())
		{
			uint32 u4_handle = u4GetUniqueHandle();

			{
				// Test against siblings.
				CPartition* ppart_next = ppartNext;
				while (ppart_next)
				{
					CPartition* ppart = ppart_next;
					ppart_next = ppart_next->ppartNext;

					if (ppart->bIsPureSpatial())
					{
						if (u4_handle == ppart->u4GetUniqueHandle() || bIsEqualToSpatial(ppart))
						{
							// Remove the unwanted partition.
							TakeChildren(*ppart);
							ppart->SetParent(0);

							// Count the duplicate.
							++iDuplicateCount;
						}
					}
				}
			}

			if (partdupMap.find(u4_handle) == partdupMap.end())
			{
				// Add this partition to the list of unique handles.
				partdupMap[u4_handle] = this;
			}
			else
			{
				Assert(partdupMap[u4_handle]);
				AlwaysAssert(partdupMap[u4_handle]->bIsPureSpatial());
				AlwaysAssert(bIsEqualToSpatial(partdupMap[u4_handle]));

				// Duplicate found. Give the children to the first version.
				partdupMap[u4_handle]->TakeChildren(*this);

				// Move the duplicate child to the unwanted list.
				SetParent(0);

				// Count the duplicate.
				++iDuplicateCount;
			}
		}

		//
		// Recurse through children.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = ppartChildren();

		if (!ppartc)
			return;

		for (iterator it = ppartc->begin(); it != ppartc->end(); ++it)
			(*it)->RemoveDuplicates();
	}

	//*****************************************************************************************
	void CPartition::BeginDuplicateList()
	{
		partdupMap.erase(partdupMap.begin(), partdupMap.end());
	}

	//*****************************************************************************************
	void CPartition::EndDuplicateList()
	{
		partdupMap.erase(partdupMap.begin(), partdupMap.end());
	}


//
// Global static variables.
//
int   CPartition::iPriority                   = 4;
float CPartition::fAreaEfficiencyThresholdSqr = Sqr(0.085f);
float CPartition::fCullDistanceCombined       = 1.0f;
float CPartition::fCullDistanceCombinedShadow = 1.0f;
int   CPartition::iDuplicateCount             = 0;
CPartitionSpace partspaceUnwanted;
CPartition* CPartition::ppartUnwanted = &partspaceUnwanted;