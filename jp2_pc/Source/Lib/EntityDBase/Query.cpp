/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Query.hpp.
 *
 * To do:
 *		Get the physics query using a partition to work, and test it with the physics subsystem.
 *		Fix 'constness' mess.  All queries use const CWorld&, but call routines with non-const 
 *		 CPartition*, and place in non-const CPartition* return pointers or containers.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Query.cpp                                             $
 * 
 * 81    98/10/02 15:40 Speter
 * Added sort option to terrain object query.
 * 
 * 80    98/10/01 16:23 Speter
 * New presence used for tighter fitting collide volumes.
 * 
 * 79    9/24/98 5:05p Mlange
 * Improved stat reporting.
 * 
 * 78    9/18/98 10:51a Mlange
 * Location trigger query now returns CLocationTrigger types only, instead of CTrigger base
 * class types. Added more performance stats.
 * 
 * 77    9/16/98 6:58p Mlange
 * Added profile stat for world db query time.
 * 
 * 76    98.09.12 5:40p Mmouni
 * Render type query now check the bEditTrnObjs flag to hide/show terrain objects.
 * 
 * 75    98/09/12 1:10 Speter
 * Added Partition-based query for QueryPhysicsBoxFast.
 * 
 **********************************************************************************************/

#include "GblInc/Common.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Query.hpp"
#include "Query/QRenderer.hpp"
#include "Query/QPhysics.hpp"
#include "Query/QAI.hpp"
#include "Query/QMessage.hpp"
#include "Query/QTerrain.hpp"
#include "Query/QTerrainObj.hpp"
#include "Query/QWater.hpp"
#include "Query/QTriggers.hpp"

#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Lib/Trigger/Trigger.hpp"
#include "Lib/GeomDBase/Shape.hpp"
#include "Lib/GeomDBase/PartitionSpace.hpp"
#include "Lib/GeomDBase/Terrain.hpp"
#include "Lib/GeomDBase/TerrainObj.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/Renderer/Pipeline.hpp"
#include "Lib/Physics/Magnet.hpp"
#include "Game/DesignDaemon/Daemon.hpp"
#include "WorldPriv.hpp"
#include "Game/AI/AIInfo.hpp"


static CProfileStat psWorldDBQuery("Wrld DB query", &proProfile.psFrame);
	static CProfileStat psQueryTerrainObj(     "Terrain obj",  &psWorldDBQuery, Set(epfHIDDEN));
	static CProfileStat psQueryRenderTypes(    "Render type",  &psWorldDBQuery, Set(epfHIDDEN));
	static CProfileStat psQueryAllParts(       "All parts",    &psWorldDBQuery, Set(epfHIDDEN));
	static CProfileStat psQueryAllChildren(    "All children", &psWorldDBQuery, Set(epfHIDDEN));
	static CProfileStat psQueryPhysics(        "Physics",      &psWorldDBQuery, Set(epfHIDDEN));
	static CProfileStat psQueryPhysicsMovable( "Physics move", &psWorldDBQuery, Set(epfHIDDEN));
	static CProfileStat psQueryPhysicsBoxFast( "Physics box",  &psWorldDBQuery, Set(epfHIDDEN));
	static CProfileStat psQueryWater(          "Water",        &psWorldDBQuery, Set(epfHIDDEN));
	static CProfileStat psQueryAnimal(         "Animal",       &psWorldDBQuery, Set(epfHIDDEN));
	static CProfileStat psQueryAI(             "AI",           &psWorldDBQuery, Set(epfHIDDEN));
	static CProfileStat psQueryLocationTrigger("Loc trigger",  &psWorldDBQuery, Set(epfHIDDEN));

//
// Class implementations.
//


//**********************************************************************************************
template<class ACT> class CTraversePartitions
//
// Prefix: trvpart
//
//**************************************
{
	const CBoundVol* pbvVolume;			// Volume to test in, if any.
	const CPresence3<>* ppr3InvVolume;	// Inverse presence thereof, if any.
	ACT Act;							// Action object to perform on partitions.
	
public:

	//******************************************************************************************
	CTraversePartitions
	(
		CPartition* ppart_data,			// The top-level partition to test.
		const CBoundVol* pbv,			// Volume to test within, if any.
		const CPresence3<>* ppr3_inv,	// Inverse presence applied to volume, if any.
		const ACT& act					// Action object, to perform on all partitions
	);

	//******************************************************************************************
	CTraversePartitions
	(
		CPartition* ppart_data,					// The top-level partition to test.
		const CPartition* ppart_volume,			// Volume to test within.
		const ACT& act							// Action object, to perform on all partitions
												// within the volume.
	);

private:

	//******************************************************************************************
	void Traverse
	(
		CPartition* ppart_data,					// The top-level partition to check.
		ESideOf esf
	);

	//******************************************************************************************
	void TraverseAll
	(
		CPartition* ppart_data					// The top-level partition to check.
	);
};

	//******************************************************************************************
	template<class ACT> CTraversePartitions<ACT>::CTraversePartitions
	(
		CPartition* ppart_data,
		const CBoundVol* pbv,			// Volume to test within, if any.
		const CPresence3<>* ppr3_inv,	// Inverse presence applied to volume, if any.
		const ACT& act
	)
		// Copy volume and act reference.
		: pbvVolume(pbv), ppr3InvVolume(ppr3_inv), Act(act)
	{
		// Start test at top of tree.  
		if (pbvVolume)
			Traverse(ppart_data, esfINTERSECT);
		else
			// If volume was 0, we don't need to check any intersections.
			TraverseAll(ppart_data);
	}

	//******************************************************************************************
	template<class ACT> CTraversePartitions<ACT>::CTraversePartitions
	(
		CPartition* ppart_data,
		const CPartition* ppart_volume,
		const ACT& act
	)
		// Copy volume and act reference.
		: Act(act)
	{
		CPresence3<> pr3_inv;

		if (ppart_volume)
		{
			// Extract volume and presence from partition.
			pbvVolume = ppart_volume->pbvBoundingVol();
			pr3_inv = ~ppart_volume->pr3Presence();
			ppr3InvVolume = &pr3_inv;

			// Start test at top of tree.  
			Traverse(ppart_data, esfINTERSECT);
		}
		else
		{
			// Start test at top of tree.  If volume was 0, we don't need to check any intersections.
			pbvVolume = 0;
			ppr3InvVolume = 0;
			TraverseAll(ppart_data);
		}
	}

	//******************************************************************************************
	template<class ACT> void CTraversePartitions<ACT>::Traverse(CPartition* ppart_data, ESideOf esf)
	{
		Assert(esf != esfOUTSIDE);

		bool b_test = Act.bTest(ppart_data);
		if (!b_test && !ppart_data->ppartChildren())
			// Nothing to do with this partition.
			return;

		if (esf == esfINTERSECT)
		{
			// Perform custom intersection function.
			esf = Act.esfSideOf(pbvVolume, ppr3InvVolume, ppart_data);

			if (esf == esfOUTSIDE)
				// No intersection, done with this branch.
				return;

			// If esf == esfINSIDE, children will not have to perform volume intersection.
		}

		// Perform action on this partition.
		if (b_test)
			Act(ppart_data, esf);

		//
		// Check if there are children, and if they intersect the target partition.
		//

		// Get a pointer to the child list.
		CPartition* ppartc = ppart_data->ppartChildren();

		if (ppartc)
		{
			// Iterate through the container.
			for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				Traverse(*it, esf);
		}
	}

	//******************************************************************************************
	template<class ACT> void CTraversePartitions<ACT>::TraverseAll(CPartition* ppart_data)
	{
		if (Act.bTest(ppart_data))
			Act(ppart_data, esfINSIDE);

		// Get a pointer to the child list.
		CPartition* ppartc = ppart_data->ppartChildren();
		if (ppartc)
		{
			// Iterate through the container.
			for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				TraverseAll(*it);
		}
	}

	//******************************************************************************************
	//
	// Helper global functions.
	//

	//******************************************************************************************
	template<class ACT> inline void TraversePartitions
	(
		CPartition* ppart_data,					// The top-level partition to test.
		const CPartition* ppart_volume,			// Volume to check for.
		const ACT& act							// Action object, to perform on all partitions
												// within the volume.
	)
	//**********************************
	{
		CTraversePartitions<ACT>(ppart_data, ppart_volume, act);
	}

	//******************************************************************************************
	template<class ACT> inline void TraversePartitions
	(
		CPartition* ppart_data,			// The top-level partition to test.
		const CBoundVol* pbv,			// Volume to test within, if any.
		const CPresence3<>* ppr3,		// Presence applied to volume, if any.
		const ACT& act					// Action object, to perform on all partitions
										// within the volume.
	)
	//**********************************
	{
		if (ppr3)
		{
			// Create inverse presence.
			CPresence3<> pr3_inv = ~*ppr3;
			CTraversePartitions<ACT>(ppart_data, pbv, &pr3_inv, act);
		}
		else
		{
			CTraversePartitions<ACT>(ppart_data, pbv, 0, act);
		}
	}

	//******************************************************************************************
	template<class ACT> inline void TraverseWorld
	(
		const CWorld& w,						// The world to query.
		const CPartition* ppart_volume,			// Volume to check for.
		const ACT& act							// Action object, to perform on all partitions
												// within the volume.
	)
	//**********************************
	{
		w.Lock();
		CTraversePartitions<ACT>(w.ppartPartitionList(), ppart_volume, act);
		w.Unlock();
	}

	//******************************************************************************************
	template<class ACT> inline void TraverseWorld
	(
		const CWorld& w,				// The world to query.
		const CBoundVol* pbv,			// Volume to test within, if any.
		const CPresence3<>* ppr3,		// Presence applied to volume, if any.
		const ACT& act					// Action object, to perform on all partitions
										// within the volume.
	)
	//**********************************
	{
		w.Lock();
		if (ppr3)
		{
			// Create inverse presence.
			CPresence3<> pr3_inv = ~*ppr3;
			CTraversePartitions<ACT>(w.ppartPartitionList(), pbv, &pr3_inv, act);
		}
		else
		{
			CTraversePartitions<ACT>(w.ppartPartitionList(), pbv, 0, act);
		}
		w.Unlock();
	}

//
// Some ACT classes which build partition lists based on various criteria.
//


//**********************************************************************************************
template<class T> class CBuildList
//
// Class useful as a base for ACT classes which build a list<T*>.
//
//**************************************
{
	std::list<T*>& rlistBuild;

public:

	//******************************************************************************************
	CBuildList(std::list<T*>& rlist)
		: rlistBuild(rlist)
	{
	}

	//******************************************************************************************
	static bool bTest(CPartition* ppart)
	{
		// Ensure that the partition is of type T.
		return ptCast<T>(ppart) != 0;
	}

	//******************************************************************************************
	static ESideOf esfSideOf(const CBoundVol* pbv_vol, const CPresence3<>* ppr3_invvol, CPartition* ppart)
	{
		// Perform volume check.
		Assert(pbv_vol);
		Assert(ppr3_invvol);

		CPresence3<> pr3_total = ppart->pr3Presence() * *ppr3_invvol;
		return pbv_vol->esfSideOf(*ppart->pbvBoundingVol(), 0, &pr3_total);
	}

	//******************************************************************************************
	void operator()	(CPartition* ppart, ESideOf)
	{
		// Add ppart to list.
		Assert(ptCast<T>(ppart));
		rlistBuild.push_back(static_cast<T*>(ppart));
	}
};

	
//**********************************************************************************************
template<class TEST> class CBuildPartitionList: public TEST
//
// Class useful as a base for ACT classes which build a TPartitionList.
//
//**************************************
{
	TPartitionList& rpartlistBuild;

public:

	//******************************************************************************************
	CBuildPartitionList(TPartitionList& rpartlist)
		: rpartlistBuild(rpartlist)
	{
	}

	//******************************************************************************************
	static ESideOf esfSideOf(const CBoundVol* pbv_vol, const CPresence3<>* ppr3_invvol, CPartition* ppart)
	{
		// Perform volume check.
		Assert(pbv_vol);
		Assert(ppr3_invvol);

		CPresence3<> pr3_total = ppart->pr3Presence() * *ppr3_invvol;
		return pbv_vol->esfSideOf(*ppart->pbvBoundingVol(), 0, &pr3_total);
	}

	//******************************************************************************************
	void operator()	(CPartition* ppart, ESideOf esf)
	{
		// Add ppart to list.
		SPartitionListElement ple = {ppart, esf};
		rpartlistBuild.push_back(ple);
	}
};
	
//******************************************************************************************
template<class T> class CTestType
{
public:
	static bool bTest(CPartition* ppart)
	{
		return ptCast<T>(ppart) != 0;
	}
};
		
//**********************************************************************************************
//
// CWDbQueryTerrainObj implementation.
//

	//******************************************************************************************
	//
	inline bool operator <
	(
		SPartitionListElement& t1,
		SPartitionListElement& t2
	)
	//
	// Comparison function for TPartitionListElements containing CTerrainObjs.  Compares the
	// instances by height.
	//
	//**********************************
	{
		// Assert that these are the correct types, then static cast them to avoid function calls.
		Assert(ptCast<CTerrainObj>(t1.ppart));
		Assert(ptCast<CTerrainObj>(t2.ppart));

		CTerrainObj* ptrrobj2 = static_cast<CTerrainObj*>(t2.ppart);
		CTerrainObj* ptrrobj1 = static_cast<CTerrainObj*>(t1.ppart);

		return ptrrobj1->iHeight < ptrrobj2->iHeight;
	}

	//******************************************************************************************
	CWDbQueryTerrainObj::CWDbQueryTerrainObj(const CPartition* ppart, bool b_sort, const CWorld& w)
		: CWDbQuery< TPartitionList >(w)
	{
		CCycleTimer ctmr;

		w.Lock();
		TraversePartitions(w.ppartTerrainPartitionList(), ppart, CBuildPartitionList< CTestType<CTerrainObj> >(*this));
		w.Unlock();

		if (b_sort)
		{
			// Sort them by height param...that's the order they must be rendered.
			sort();
#if 0
			//
			// THIS OVERLY CLEVER FUNCTIONALITY NOT NEEDED.
			// IT WON'T WORK FOR RENDERING, AS IT DOESN'T ACCOUNT FOR TRANSPARENT TEXTURES.
			// AND IT WILL RARELY HELP COLLISION QUERIES.
			//

			//
			// Also cull out hidden textures.
			// Iterate in reverse order, finding highest texture that completely contains
			// the volume.
			//
			list<SPartitionListElement>::iterator it = end();
			if (it != begin())
			{
				while (--it, it != begin())
				{
					// See whether it entirely contains the partition.
					if ((*it).esfView == esfINTERSECT)
					{
						// It is not contained in the partition.
						if ((*it).ppart->esfSideOf(ppart) == esfINSIDE)
						{
							// This is a base texture, and all lower-height textures may be culled.
							erase(begin(), it);
							break;
						}
					}
				}
			}
#endif
		}

		Begin();

		TCycles cy = ctmr();
		psQueryTerrainObj.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}



//**********************************************************************************************
//
// CWDbQueryRenderTypes implementation.
//

	bool bEditTrnObjs = false;

	//******************************************************************************************
	class CTestRenderType
	{
	public:
		static bool bTest(CPartition* ppart)
		{
			// Terrain isn't supposed to be part of partitioning.
			Assert(!ptCast<CTerrain>(ppart));

			// Return everything with a shape.
			return ppart->prdtGetRenderType() != 0;
		}
	};
			
	//******************************************************************************************
	CWDbQueryRenderTypes::CWDbQueryRenderTypes(const CPartition* ppart, bool b_include_terrain, const CWorld& w)
		: CWDbQuery< TPartitionList >(w)
	{
		CCycleTimer ctmr;

		if (b_include_terrain)
		{
			CTerrain* ptrr = CWDbQueryTerrain(w).tGet();
			if (ptrr)
			{
				SPartitionListElement ple = {ptrr, esfINTERSECT};
				push_back(ple);
			}
		}

		// Add remaining world shapes.
		TraverseWorld(w, ppart, CBuildPartitionList<CTestRenderType>(*this));

		// Add the triggers in, if they are visible.
		if (CRenderContext::bRenderTriggers)
			TraversePartitions(w.ppartTriggerPartitionList(), ppart, CBuildPartitionList<CTestRenderType>(*this));
		
		// Add the terrain objects if we want to render them.
		if (bEditTrnObjs)
			TraversePartitions(w.ppartTerrainPartitionList(), ppart, CBuildPartitionList<CTestRenderType>(*this));

		Begin();

		TCycles cy = ctmr();
		psQueryRenderTypes.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}


//**********************************************************************************************
//
// CWDbQueryRenderTypesAndMagnets implementation.
//

	//******************************************************************************************
	CWDbQueryRenderTypesAndMagnets::CWDbQueryRenderTypesAndMagnets(const CPartition* ppart, const CWorld& w)
		: CWDbQueryRenderTypes(ppart, false, w)
	{
		// Add magnets to the list.
		NMagnetSystem::QueryMagnets(*this, ppart);
	}


//**********************************************************************************************
//
// CWDbQueryActiveCameraPlacement implementation.
//

	CWDbQueryActiveCameraPlacement::CWDbQueryActiveCameraPlacement(const CWorld& w)
	{
		// Query the camera as usual, get its placement.
		CWDbQueryActiveCamera wqcam;

		p3Camera = wqcam.tGet()->pr3Presence();
		pp3Camera = &p3Camera;
	}


//**********************************************************************************************
//
// CWDbQueryAllParts implementation.
//

	//******************************************************************************************
	class CTestAll
	{
	public:
		static bool bTest(CPartition* ppart)
		{
			// Add the partition if it has a bounding volume that is finite.
			return ppart->pbvBoundingVol()->fMaxExtent() < 1000.0;
		}
	};
		
	//******************************************************************************************
	CWDbQueryAllParts::CWDbQueryAllParts(const CPartition* ppart, const CWorld& w)
		: CWDbQuery< TPartitionList >(w)
	{
		CCycleTimer ctmr;

		// Return normal and terrain objects.
		TraverseWorld(w, ppart, CBuildPartitionList<CTestAll>(*this));
		w.Lock();
		TraversePartitions(w.ppartTerrainPartitionList(), ppart, CBuildPartitionList< CTestType<CTerrainObj> >(*this));
		w.Unlock();

		Begin();

		TCycles cy = ctmr();
		psQueryAllParts.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}

	//******************************************************************************************
	CWDbQueryAllChildren::CWDbQueryAllChildren(CPartition* ppart_root)
	{
		CCycleTimer ctmr;

		// Fill with self and all children.
		TraversePartitions(ppart_root, 0, CBuildPartitionList<CTestAll>(*this));

		Begin();

		TCycles cy = ctmr();
		psQueryAllChildren.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}



//**********************************************************************************************
//
// CWDbQueryLights implementation.
//

/*
	//******************************************************************************************
	class CTestLight
	{
	public:
		static bool bTest(CInstance* pinst)
		{
			// Test whether this instance's render type is a CLight.
			CInstance* pinst = ptCast<CInstance>(ppart);
			return pinst && ptCastRenderType<CLight>(pinst->prdtGetRenderInfo()) != 0;
		}
	};
*/

	std::list<CInstance*> CWDbQueryLights::lpinsActiveLights;	// The active light list, maintained by the world dbase.

	//******************************************************************************************
	CWDbQueryLights::CWDbQueryLights(const CPartition* ppart, const CWorld& w)
		: CWDbQuery< std::list<CInstance*> >(w)
	{
		std::list<CInstance*>::const_iterator i_light;
		std::list<CInstance*>::const_iterator i_end = lpinsActiveLights.end();
		for (i_light = lpinsActiveLights.begin(); i_light != i_end; ++i_light)
		{
			push_back(*i_light);
		}

		Begin();
	}


//**********************************************************************************************
//
// CWDbQueryPhysics implementation.
//

	//******************************************************************************************
	class CBuildPhysics: public CBuildList<CInstance>
	{
	public:

		//******************************************************************************************
		CBuildPhysics(std::list<CInstance*>& rlist)
			: CBuildList<CInstance>(rlist)
		{
		}

		//
		// Overrides.
		//

		//******************************************************************************************
		static bool bTest(CPartition* ppart)
		{
			// Test whether this object has physics info and is tangible.
			CInstance* pins = ptCast<CInstance>(ppart);
			return pins && pins->pphiGetPhysicsInfo()->bIsTangible();
		}

		//******************************************************************************************
		static ESideOf esfSideOf(const CBoundVol* pbv_vol, const CPresence3<>* ppr3_invvol, CPartition* ppart)
		{
			const CBoundVol* pbv;
			CPresence3<> pr3;

			CInstance* pins = ptCast<CInstance>(ppart);
			if (pins && !pins->ppartChildren())
			{
				// For leaf instances, get the physics info bounding volume.
				pbv = pins->pphiGetPhysicsInfo()->pbvGetCollideVol();
				pr3 = pins->pphiGetPhysicsInfo()->pr3Collide(pins);
				Assert(pbv);
				if (pbv->pbviCast())
				{
					// Infinite physics volume, let's use rendering volume.
					rptr_const<CRenderType> prdt = pins->prdtGetRenderInfo();
					if (prdt)
						pbv = &prdt->bvGet();
				}
			}
			else
			{
				// Default version simply returns partition's bounding volume and presence.
				pbv = ppart->pbvBoundingVol();
				pr3 = ppart->pr3Presence();
			}

			// Perform volume check.
			Assert(pbv_vol);
			Assert(ppr3_invvol);

			CPresence3<> pr3_total = pr3 * *ppr3_invvol;
			return pbv_vol->esfSideOf(*pbv, 0, &pr3_total);
		}
	};

	//******************************************************************************************
	CWDbQueryPhysics::CWDbQueryPhysics(const CPartition* ppart, const CWorld& w)
		: CWDbQuery< list<CInstance*> >(w)
	{
		CCycleTimer ctmr;

		TraverseWorld(w, ppart, CBuildPhysics(*this));

		Begin();

		TCycles cy = ctmr();
		psQueryPhysics.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}

	//******************************************************************************************
	CWDbQueryPhysics::CWDbQueryPhysics(const CBoundVol& bv, const CPresence3<>& pr3_boundvol, const CWorld& w)
		: CWDbQuery< list<CInstance*> >(w)
	{
		CCycleTimer ctmr;

		// Build the query list.
		TraverseWorld(w, &bv, &pr3_boundvol, CBuildPhysics(*this));

		Begin();

		TCycles cy = ctmr();
		psQueryPhysics.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}


//**********************************************************************************************
//
// CWDbQueryPhysicsMovable implementation.
//

	//******************************************************************************************
	class CBuildPhysicsMovable: public CBuildPhysics
	{
	public:

		//******************************************************************************************
		CBuildPhysicsMovable(std::list<CInstance*>& rlist)
			: CBuildPhysics(rlist)
		{
		}

		//
		// Overrides.
		//

		//******************************************************************************************
		static bool bTest(CPartition* ppart)
		{
			// Test whether this object has physics info and is tangible.
			CInstance* pins = ptCast<CInstance>(ppart);
			if (pins)
			{
				const CPhysicsInfo* pphi = pins->pphiGetPhysicsInfo();
				return pphi->bIsTangible() && pphi->bIsMovable();
			}
			return false;
		}
	};

	//******************************************************************************************
	CWDbQueryPhysicsMovable::CWDbQueryPhysicsMovable(const CPartition* ppart, const CWorld& w)
		: CWDbQuery< list<CInstance*> >(w)
	{
		CCycleTimer ctmr;

		TraverseWorld(w, ppart, CBuildPhysicsMovable(*this));

		Begin();

		TCycles cy = ctmr();
		psQueryPhysicsMovable.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}

//**********************************************************************************************
//
// CWDbQueryPhysicsBoxFast implementation.
//

	//******************************************************************************************
	class CBuildPhysicsBoxFast: public CBuildPhysics
	{
	public:

		//******************************************************************************************
		CBuildPhysicsBoxFast(std::list<CInstance*>& rlist)
			: CBuildPhysics(rlist)
		{
		}

		//
		// Overrides.
		//

		//******************************************************************************************
		static bool bTest(CPartition* ppart)
		{
			// Test whether this object has physics info, is tangible, and is a box instance.
			CInstance* pins = ptCast<CInstance>(ppart);
			if (pins)
			{
				const CPhysicsInfo* pphi = pins->pphiGetPhysicsInfo();
				return (pphi->bIsTangible() && (pphi->ppibCast() || pphi->pphicCast()));
			}
			return false;
		}
/*
		//******************************************************************************************
		static ESideOf esfSideOf(const CBoundVol* pbv_vol, const CPresence3<>* ppr3_invvol, CPartition* ppart)
		{
			if (!ppart->ppartChildren())
				// Return success for all leaf partitions (will almost surely be instances).
				return esfINTERSECT;
			else
				// Perform standard intersection for partitions.
				return CBuildList<CInstance>::esfSideOf(pbv_vol, ppr3_invvol, ppart);
		}
*/
	};

	//******************************************************************************************
	CWDbQueryPhysicsBoxFast::CWDbQueryPhysicsBoxFast(const CBoundVol& bv, const CPresence3<>& pr3_boundvol, const CWorld& w)
		: CWDbQuery< list<CInstance*> >(w)
	{
		CCycleTimer ctmr;

		// Build the query list.
		TraverseWorld(w, &bv, &pr3_boundvol, CBuildPhysicsBoxFast(*this));

		Begin();

		TCycles cy = ctmr();
		psQueryPhysicsBoxFast.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}

	//******************************************************************************************
	CWDbQueryPhysicsBoxFast::CWDbQueryPhysicsBoxFast(const CPartition* ppart, const CWorld& w)
		: CWDbQuery< list<CInstance*> >(w)
	{
		CCycleTimer ctmr;

		// Build the query list.
		TraverseWorld(w, ppart, CBuildPhysicsBoxFast(*this));

		Begin();

		TCycles cy = ctmr();
		psQueryPhysicsBoxFast.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}


//**********************************************************************************************
//
// CWDbQueryWater implementation.
//

	// The water object list, maintained by the world dbase.
	std::list<CEntityWater*> CWDbQueryWater::lspetWater;

	//******************************************************************************************
	CWDbQueryWater::CWDbQueryWater(const CBoundVol& bv, const CPresence3<>& pr3_boundvol, const CWorld& w)
		: CWDbQuery< list<CEntityWater*> >(w)
	{
		CCycleTimer ctmr;

		// Build the query list.
		TraverseWorld(w, &bv, &pr3_boundvol, CBuildList<CEntityWater>(*this));

		Begin();

		TCycles cy = ctmr();
		psQueryWater.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}

//*********************************************************************************************
//
// class CWDbQueryWaterHeight implementation.
//

	//*****************************************************************************************
	CWDbQueryWaterHeight::CWDbQueryWaterHeight(const CVector2<>& v2_world, const std::list<CEntityWater*>& lspetw)
	{
		// Copy the querying vector.
		v3Water = v2_world;
		v3Water.tZ = rWATER_NONE;
		petWater = 0;

		forall_const (lspetw, std::list<CEntityWater*>, itpew)
		{
			// Ask the water its height.
			v3Water.tZ = (*itpew)->rWaterHeight(v2_world);
			if (v3Water.tZ > rWATER_NONE)
			{
				// There is water here, and it is above the terrain.
				petWater = *itpew;
				break;
			}
		}
	}

//**********************************************************************************************
//
// CWDbQueryAnimal implementation.
//

	//******************************************************************************************
	CWDbQueryAnimal::CWDbQueryAnimal(const CPartition* ppart, const CWorld& w)
		: CWDbQuery< list<CAnimal*> >(w)
	{
		CCycleTimer ctmr;

		TraverseWorld(w, ppart, CBuildList<CAnimal>(*this));

		Begin();

		TCycles cy = ctmr();
		psQueryAnimal.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}

//**********************************************************************************************
//
// CWDbQueryAI implementation.
//

	//******************************************************************************************
	class CBuildAI: public CBuildList<CInstance>
	{
	public:

		//******************************************************************************************
		CBuildAI(std::list<CInstance*>& rlist)
			: CBuildList<CInstance>(rlist)
		{
		}

		//
		// Overrides.
		//

		//******************************************************************************************
		static bool bTest(CPartition* ppart)
		{
			// Test whether this object has ai info.
			CInstance* pins = ptCast<CInstance>(ppart);
			return pins && pins->paiiGetAIInfo() && !pins->paiiGetAIInfo()->bIgnore();
		}

		//******************************************************************************************
		static ESideOf esfSideOf(const CBoundVol* pbv_vol, const CPresence3<>* ppr3_invvol, CPartition* ppart)
		{
			const CBoundVol* pbv;
			CPresence3<> pr3 = ppart->pr3Presence();

			CInstance* pins = ptCast<CInstance>(ppart);
			if (pins && !pins->ppartChildren())
			{
				Assert(pins->paiiGetAIInfo());
				Assert(!pins->paiiGetAIInfo()->bIgnore());

				// Snag the bound vol the way AI's think about it.
				pbv = pins->paiiGetAIInfo()->pbvGetBoundVol(pins);
			}
			else
				// Default version simply returns partition's bounding volume and presence.
				pbv = ppart->pbvBoundingVol();

			// Perform volume check.
			Assert(pbv_vol);
			Assert(ppr3_invvol);

			CPresence3<> pr3_total = pr3 * *ppr3_invvol;
			return pbv_vol->esfSideOf(*pbv, 0, &pr3_total);
		}
	};

	//******************************************************************************************
	CWDbQueryAI::CWDbQueryAI(const CBoundVol& bv, const CPresence3<>& pr3_boundvol, const CWorld& w)
		: CWDbQuery< list<CInstance*> >(w)
	{
		CCycleTimer ctmr;

		// Build the query list.
		TraverseWorld(w, &bv, &pr3_boundvol, CBuildAI(*this));

		Begin();

		TCycles cy = ctmr();
		psQueryAI.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}



//**********************************************************************************************
//
// CWDbQueryActiveEntities implementation.
//

	bool CWDbQueryActiveEntities::bAlreadyExists = false;			// There can be only one of these instantiated at any one time.
	std::list<CEntity*>	CWDbQueryActiveEntities::lpetActiveEntities;	// A list of active entities, maintained by the world database.



	//******************************************************************************************
	CWDbQueryActiveEntities::CWDbQueryActiveEntities(const CWorld& w)
		: CWDbQuery< list<CEntity*> >(w)
	{

		// There are no entities if the world dbase has been purged.
		if (w.bHasBeenPurged)
			return;

		// For now, we'll just be sloppy and copy the global list.  There shouldn't be too many....
//		Assert(!bAlreadyExists);
//		bAlreadyExists = true;

		// For now we'll simply return *ALL* the entities in the world because we have no way
		// to distinguish an active entity.

		list<CEntity*>::const_iterator i_entity;
		list<CEntity*>::const_iterator i_end = lpetActiveEntities.end();
		for (i_entity = lpetActiveEntities.begin(); i_entity != i_end; ++i_entity)
		{
			push_back(*i_entity);
		}


		//
		// Add subsystems.
		//

		TListInstance::iterator it_w;	// CInstance iterator for subsystem.

		for (it_w = w.psubList->begin(); it_w != w.psubList->end(); it_w++)
		{
			CEntity* pet = ptCast<CEntity>(*it_w);

			// Every subsystem must also be an entity.
			Assert(pet != 0);

			push_back(pet);
		}

		Begin();
	}



//**********************************************************************************************
//
// CWDbQueryActiveDaemon implementation.
//

	//******************************************************************************************
	CWDbQueryActiveDaemon::CWDbQueryActiveDaemon(const CWorld& w)
	{
		// Null the pointer to the daemon.
		pdemDaemon = 0;

		TListInstance::iterator it_w;

		for (it_w = w.psubList->begin(); it_w != w.psubList->end(); it_w++)
		{
			//
			// To do: Use the ptCast function.
			//		pdemDaemon = ptCast<CDaemon>(*it_w);
			//
			pdemDaemon = dynamic_cast<CDaemon*>(*it_w);

			// If the daemon is found
			if (pdemDaemon)
				return;
		}

		// Can't find the design daemon!
		Assert(0);
	}



//**********************************************************************************************
//
// CWDbQueryActiveDaemon implementation.
//

	//**********************************************************************************************
	class CFindPlayer
	//
	// Finds a single player in the partitions.
	// 
	// This could obviously be improved by using a different TraverseWorld which bailed out when
	// the player was found.  It could be improved futher by storing the player separately.
	//
	//**************************************
	{
	public:
		CPlayer*& rpPlayer;

	public:

		//******************************************************************************************
		CFindPlayer(CPlayer*& rpplay)
			: rpPlayer(rpplay)
		{
		}

		//******************************************************************************************
		void operator()	(CPartition* ppart, ESideOf)
		{
			CPlayer* pplay = ptCast<CPlayer>(ppart);
			if (pplay)
			{
				Assert(!rpPlayer);
				rpPlayer = pplay;
			}
		}
	};

	//******************************************************************************************
	CWDbQueryActivePlayer::CWDbQueryActivePlayer(const CWorld& w)
	{
//		TraverseWorld(w, 0, CFindPlayer(pplayPlayer));
//		Assert(pplayPlayer);
		extern CPlayer* gpPlayer;
		Assert(gpPlayer);
		pplayPlayer = gpPlayer;
	}



//*****************************************************************************************
//
// CWDbQueryLocationTrigger implementation.
//

	//******************************************************************************************
	CWDbQueryLocationTrigger::CWDbQueryLocationTrigger(const CBoundVol& bv, const CPresence3<>& pr3_boundvol, const CWorld& w)
		: CWDbQuery< list<CLocationTrigger*> >(w)
	{
		CCycleTimer ctmr;

		w.Lock();
		TraversePartitions(w.ppartTriggerPartitionList(), &bv, &pr3_boundvol, CBuildList<CLocationTrigger>(*this));
		w.Unlock();

		Begin();

		TCycles cy = ctmr();
		psQueryLocationTrigger.Add(cy, 1);
		psWorldDBQuery.Add(cy, 1);
	}
