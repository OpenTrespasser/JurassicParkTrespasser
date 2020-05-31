/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of WorldDBase.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/WorldDBase.cpp                                        $
 * 
 * 265   10/01/98 1:56a Pkeet
 * Moved call to disable parent caches from the world move function to the render database move
 * message.
 * 
 * 264   9/30/98 9:35p Rwyatt
 * Reset the message queue on world databse reset.
 * 
 * 263   9/28/98 6:24p Pkeet
 * Fixed bug that left render cache behind when object moved.
 * 
 * 262   9/27/98 7:59p Pkeet
 * The 'Move' member function now checks child priorities.
 * 
 * 261   9/27/98 2:13a Mmouni
 * Fixed load progress to be more consistent.
 * 
 * 260   9/25/98 11:54a Mlange
 * Now sends out scene file loaded message.
 * 
 * 259   9/23/98 5:52p Rwyatt
 * Audio daemon cleanup is now done first
 * 
 * 258   98.09.20 7:19p Mmouni
 * Added CLUT clear back in.
 * 
 * 257   98/09/19 14:45 Speter
 * Made 2 bSaveWorlds different names, so one can be called from the debugger.
 * 
 * 256   9/18/98 11:52p Pkeet
 * Disabled clearing of the cluts between levels.
 * 
 **********************************************************************************************/

//  Here the ugly bug rears its head again.
template<class X> class CMicrosoftsCompilerIsBuggyAndWeHateIt
{
};

//
// Includes.
//
#ifdef __MWERKS__
 #include "Lib/W95/WinInclude.hpp"
 #include <fcntl.h>
 #include <io.h>
 #include <stdio.h>
 #include "GblInc/Common.hpp"
 #include "WorldDBase.hpp"
#else
 #include <fcntl.h>
 #include <io.h>
 #include <stdio.h>
 #include "GblInc/Common.hpp"
 #include "WorldDBase.hpp"
 #include "Lib/W95/WinInclude.hpp"
#endif

#include "Lib/sys/VirtualMem.hpp"
#include "Lib/Loader/TextureManager.hpp"
#include "Lib/Loader/DataDaemon.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Loader/ImageLoader.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/PipeLine.hpp"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Animal.hpp"
#include "QueueMessage.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Instancer.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/GeomDBase/Terrain.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Game/DesignDaemon/Daemon.hpp"
#include "Game/DesignDaemon/HitSpang.hpp"
#include "Game/DesignDaemon/BloodSplat.hpp"
#include "Lib/Trigger/Trigger.hpp"
#include "Lib/EntityDBase/ParticleGen.hpp"

#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/Query/QWater.hpp"
#include "Lib/EntityDBase/Query/QTerrainObj.hpp"

#include "Lib/EntityDBase/MessageTypes/MsgPaint.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgCreate.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgDelete.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgCollision.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgTrigger.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPhysicsReq.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgControl.hpp"
#include "Lib\EntityDBase\TextOverlay.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/reg.h"
#include "Lib/Sys/RegInit.hpp"
#include "Lib/GeomDBase/PartitionSpace.hpp"
#include "Lib/GeomDBase/PartitionBuild.hpp"
#include "Lib/GeomDBase/TerrainObj.hpp"

#include "Lib/Physics/PhysicsSystem.hpp"
#include "Lib/Physics/Infobox.hpp"
#include "Lib/Physics/InfoSkeleton.hpp"
#include "Lib/Physics/Magnet.hpp"


#include "Lib/EntityDBase/RenderDB.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"

#include "Lib/EntityDBase/Query/QMessage.hpp"
#include "Lib/Transform/TransformIO.hpp"

#include "Lib/Loader/SaveFile.hpp"
#include "Lib/Audio/AudioDaemon.hpp"

#include "Game/AI/AIMain.hpp"
#include "Game/AI/AIInfo.hpp"
#include "Game/DesignDaemon/Gun.hpp"

#include "Lib\Sys\ConIO.hpp"

#include "Lib\Control\Control.hpp"

#include <fstream>


#include "Lib\Sys\W95\Render.hpp"
#include "Lib\Loader\PlatonicInstance.hpp"
#include "Lib\Renderer\Sky.hpp"

#include "Lib/View/Grab.hpp"

#include "Lib/Sys/FileEx.hpp"
#include "WorldPriv.hpp"
#include "Lib\Loader\LoadTexture.hpp"
#include "Lib\GeomDBase\LightShape.hpp"
#include "Lib\EntityDBase\EntityLight.hpp"
#include "Lib/Renderer/RenderCacheInterface.hpp"
#include "Lib/EntityDBase/AnimationScript.hpp"
#include "Lib/EntityDBase/CameraPrime.hpp"
#include "Lib/Renderer/Particles.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Renderer/RenderCache.hpp"

#include <map>

#include "Shell\WinRenderTools.hpp"


#define bPRINT_PARTITION_SEARCH_TIMES	0

//
// Externally defined function.
//

extern bool bIsTrespasser;

// Console for predictive loading.
extern CConsoleBuffer conPreLoader;

// Extern to allow proper clearing of world dbase.
typedef std::multimap<CInstance*, CMagnetPair*, std::less<CInstance*> >   TMagnetTable;
namespace NMagnetSystem
{
//	extern TMagnetTable	mtSlaveLookup;		// A table to find magnets pairs from the slave side.
//	extern TMagnetTable	mtMasterLookup;		// A table to find magnets pairs from the master side.
	void NMagnetSystem::RemoveAllMagnets();
};

#ifdef __MWERKS__
	// MSL's multimap implementation requires the index
	// to be const, as specified by the STL standard.
	typedef pair<const uint32, CPartition*> THandlePair;
#else
	typedef std::pair<uint32, CPartition*> THandlePair;
#endif


// Perturb values for building partitions.
float fPerturbBox     = 0.0f;
int   iPerturbBoxAxis = 0;
bool bCheckForDuplicates;


//
// Module-specific functions.
//

//*********************************************************************************************
//
template<class C> bool bIsType
(
	CInstance* pins, C* //pc
)
//
// Returns:
//		'true' if the object 'ppress' is of class 'C',
//		'false' otherwise.
//
//**************************************
{
	return ((dynamic_cast<C*>(pins)) != 0);
}


namespace
{
#if VER_TEST && bPRINT_PARTITION_SEARCH_TIMES
	TCycles cySearchPartitionMap = 0;
	TCycles cySearchInstance     = 0;

	int iFoundPartitionInMapCount = 0;
	int iSearchPartitionCount     = 0;
#endif
};

	
//*********************************************************************************************
//
// CWorld implementation.
//

CSubsystem* ps_daemon = 0;
CRenderDB*  ps_renderDB = 0;

	//*****************************************************************************************
	CWorld::CWorld()
		: pcamActive(0), petShell(0), ptrrTerrain(0),
			iNumInstances(0), iNumSubsystems(0), iLockLevel(0)
	{
		AlwaysAssert(sizeof(CPartition::SPartitionData) == 4);

		pmapHandlePointer  = new TMapIntPPart();
		psubList           = new TListInstance();
		plsstrGroffsLoaded = new TListConstChar();
		pwWorld = this;
		bCheckForDuplicates = false;
		bLoadPending = false;
		bSavePending = false;
		bGameOver = false;

		Init();
	}

	//*****************************************************************************************
	CWorld::~CWorld()
	{
		Assert(bHasBeenPurged);
		delete pmapHandlePointer;
		delete psubList;
		delete plsstrGroffsLoaded;
	}
	

	//*****************************************************************************************
	void CWorld::Init()
	{
		Lock();

		// Assure good construction order.
		extern void * hwndGetMainHwnd();
		extern HINSTANCE hinstGetMainHInstance();

		// Reset the message queue system
		qmQueueMessage.Reset();

		// Reset any muzzle flash data
		CMuzzleFlash::ResetMuzzleFlash();

		// Reset the hit spang system
		CHitSpang::Reset();

		// Init the texture manager
		gtxmTexMan.Reset();

		// Reset the particle map.
		NParticleMap::Reset();

		// Reset the particle system.
		Particles.Reset();

		// Create the partitioning structure.
		ppartPartitions  = ::new CPartitionSpace();
		ppartBackdrops   = ::new CPartitionSpace();
		ppartTriggers    = ::new CPartitionSpace();
		ppartTerrainObjs = ::new CPartitionSpace();

		// Reset the pure spatial partition heap.
		CPartitionSpace::ResetHeap();

		// Also reset the instance partition heap.
		CInstance::ResetHeap();

		// Make an ambient light.
		AddWithNoLock(new CInstance(rptr_cast(CRenderType, rptr_new CLightAmbient(0.2f))));

		// And a directional light.
		Unlock();
		AddDefaultLight();
		Lock();

		//
		// Subsystems must be added in the order we wish them to process messages.
		// Since Step and Paint run off of messages, this is critical.
		//

		//
		//  Input
		//

		if (gpInputDeemone == 0)
		{
			gpInputDeemone = new CInputDeemone(hwndGetMainHwnd(),hinstGetMainHInstance(),ecm_DefaultControls);
			AddWithNoLock(gpInputDeemone);
		}

		//
		//  Design Daemon system.
		//

		AddWithNoLock(ps_daemon = new CDaemon());

		//
		//  Render System.
		//

		AddWithNoLock(ps_renderDB = new CRenderDB());

		// CVideoOverlay, and other systems that respond to CMessagePaint, must be added after CRenderDB.

		//
		//  Video Overlay system
		//

		AddWithNoLock(pVideoOverlay = new CVideoOverlay);

		//
		//  Frame grabber system
		//

		AddWithNoLock(pagAutoGrabber = new CAutoGrabber);


		//
		//  AI Subsystem
		//


		gpaiSystem = new CAISystem(&conAI);
		
		// Add the AI system to the world database.
		AddWithNoLock(gpaiSystem);

		//
		//	Physics System
		//

		// Create physics system.
		pphSystem = new CPhysicsSystem();
		AddWithNoLock( pphSystem );
	
		//
		//  Data Daemon (Prefetcher)
		//

//		getmainwindow
#ifdef USING_FETCH
		// Make the data daemon.
		pddDataDaemon = new CDataDaemon();
		AddWithNoLock(pddDataDaemon);
#endif

		//
		//	Animation System
		//

		AddWithNoLock(pAnimations = new CAnimations);

		//
		//	Camera primer System
		//

		AddWithNoLock(pCameraPrime = new CCameraPrime);

		//
		//	Terrain bloodsplat system
		//

		AddWithNoLock(pBloodSplats = new CBloodSplats);



		//
		//  Additional standard objects.
		//

		//
		// Camera
		//

		CCamera* pcam = new CCamera;

		// If we have a main screen, match the camera to that.
		if (prasMainScreen)
		{
			// Get the current properties of the camera.
			CCamera::SProperties camprop = pcam->campropGetProperties();

			// Setup the main viewport object.
			camprop.vpViewport.SetSize(prasMainScreen->iWidth, prasMainScreen->iHeight);

			// Set the physical aspect ratio to the product of the raster and pixel aspect ratios.
			camprop.fAspectRatio = prasMainScreen->GetAspectRatio();

			pcam->SetProperties(camprop);
		}

		AddWithNoLock(pcam);

		// Initialize Player with default values.
		gpPlayer = CPlayer::pplayNew();
		AddWithNoLock(gpPlayer);

		//
		//  Audio - INIT THE AUDIO LAST OTHERWISE THERE WILL BE SYNCRONIZATION PROBLEMS WITH LOOPED SAMPLES
		//

		// HACK HACK HACK
		// To get around MFC vs Windows, we'll cheat.
		extern void InitAudioDaemon(CWorld*);

		//
		// The audio system should already be going, this function will assert if this is not the case.
		//
		InitAudioDaemon(this);

		// new the text system, this class contains a static pointer to itself which at this point
		// should be NULL
		Assert(CTextOverlay::ptovTextSystem == NULL);
		AddWithNoLock( new CTextOverlay() );

		// Attach camera to player.
		pcamActive->SetAttached(gpPlayer, true);

		// Mark the database as used again.
		bHasBeenPurged = false;

		Unlock();

		// Set up the GROFF list.


		//
		//  Verify initialization.
		//

		Assert(ppartPartitions);
		Assert(ppartBackdrops);
		Assert(ppartTerrainObjs);
		Assert(ppartTriggers);
		Assert(pcamActive);

		// Initialize.
		InitializePartitions();

		// Enable scene saving for asserts.  The database is now ready.
		bAutoSaveScene = true;

		// Save default settings for subsystems.
		TListInstance::iterator i;
		for (i = psubList->begin(); i != psubList->end(); ++i)
		{
			CSubsystem* psub = dynamic_cast<CSubsystem*>(*i);
			
			if (psub)
				psub->SaveDefaults();
		}

	}

	//*****************************************************************************************
	void CWorld::Reset()
	{
		Purge();
		Init();
	}

	//*****************************************************************************************
	void CWorld::AddDefaultLight()
	{
		extern rptr<CMesh>	pmshLightDir;
//		extern CInstance*		pinsLightDirectionalDefaultShape;
		const TLightVal	lvDEFAULT = 1.0;

		// Make the default light mesh if it doesn't already exist.
		if (!pmshLightDir)
		{
			pmshLightDir	= rptr_cast(CMesh, rptr_new(CMesh::fhGlobalMesh) CMeshLightDir());
			MEMLOG_ADD_COUNTER(emlMeshHeap, sizeof(CMesh));
		}

		//
		// Now add the Default light into the world
		//
		LoadColour(pmshLightDir, CColour(192, 192, 192));

		// Add the light with a rendering shape for interface control.
		CInstance* pins_shape = new CInstance
		(
			CPresence3<>
			(
				// Aim light's Z axis at the target.
				CRotate3<>(d3ZAxis, CDir3<>(v3DEFAULT_LIGHT_TARGET - v3DEFAULT_LIGHT_POS)),
				// Scale.
				5.0,
				// Don't use v3DEFAULT_LIGHT_POS, because we want the light in 
				// front of the viewer.
				// Position.
				CVector3<>(-25.0f, 50.0f, 17.0f)
			),
			rptr_cast(CRenderType, pmshLightDir)
		);

		pinsLightDirectionalDefaultShape = pins_shape;
		wWorld.Add(pins_shape);

		// Create a light with shadowing enabled.
		rptr<CLightDirectional> pltd_light = rptr_new CLightDirectional(lvDEFAULT, true);

		// Remember this light.  It's important.
		petltLightDirectionalDefault = new CEntityLight(rptr_cast(CLight, pltd_light), 
														pins_shape);

		// Add the global light to the wdbase.  It's important.
		wWorld.Add(petltLightDirectionalDefault);
	}

	//*****************************************************************************************
	void CWorld::Add(CInstance* pins, bool b_fast)
	{
		// Make sure that the world database is unlocked.
		Verify(iLockLevel == 0);

		// Use the add with no lock function.
		AddWithNoLock(pins, b_fast);
	}

	//*****************************************************************************************
	void CWorld::AddWithNoLock(CInstance* pins, bool b_fast)
	{
		// To do: give each instance its own Add handler.
		Assert(pins);

		// For now, set the handle again to be safe.
		pins->SetUniqueHandle(0);

		// Adding a camera?
		CCamera* pcam = ptCast<CCamera>(pins);

		if (pcam)
		{
			// We cannot add multiple cameras at this point.
			Assert(pcamActive == 0);

			pcamActive = pcam;
		}
		// Adding a subsystem?
		else if (ptCast<CSubsystem>(pins) != 0)
		{
			psubList->push_back(pins);
			++iNumSubsystems;

			CTerrain* ptrr = ptCast<CTerrain>(pins);
			if (ptrr)
			{
				// Remove the old terrain subsystem if it exists.
				if (ptrrTerrain != 0)
					Remove(ptrrTerrain);

				ptrrTerrain = ptrr;
			}
			// Subsystems don't get partitioned.
			return;
		}
		else if (ptCast<CTerrainObj>(pins))
		{
			if (b_fast)
				Verify(ppartTerrainObjs->bFastAddChild(pins));
			else
				Verify(ppartTerrainObjs->bAddChild(pins));
			
			return;
		}
		else if (ptCast<CTrigger>(pins))
		{
			// Add triggers to their own partitions.
			if (b_fast)
				Verify(ppartTriggers->bFastAddChild(pins));
			else
				Verify(ppartTriggers->bAddChild(pins));
		}
		else
		{
			// Add to the world list by default.
			if (b_fast)
				Verify(ppartPartitions->bFastAddChild(pins));
			else
				Verify(ppartPartitions->bAddChild(pins));
			++iNumInstances;
		}

		// Keep track of all non-subsystem entities added to world database.
		CEntity* pet = ptCast<CEntity>(pins);
		if (pet)
		{
//			Assert(!CWDbQueryActiveEntities::lpetActiveEntities.find(pet));
			CWDbQueryActiveEntities::lpetActiveEntities.push_back(pet);	

			// Put water in separate list.
			CEntityWater* pew;
			if (pew = dynamic_cast<CEntityWater*>(pins))
			{
				CWDbQueryWater::lspetWater.push_back(pew);	
			}
		}

		// Keep track of all lights.
		if (pins->prdtGetRenderInfo() && ptCastRenderType<CLight>(pins->prdtGetRenderInfo()) != 0)
		{
			CWDbQueryLights::lpinsActiveLights.push_back(pins);	
		}

		// Notify the world of our brand new baby instance.
		CMessageCreate(pins, 0).Dispatch();
	}

	//*****************************************************************************************
	void CWorld::AddShell(CEntity* pet)
	{
		Assert(pet);
		petShell = pet;
	}

	//*****************************************************************************************
	void CWorld::AddGroff(const char* str_groff)
	{
		plsstrGroffsLoaded->push_back(strdup(str_groff));
	}

	//*****************************************************************************************
	void CWorld::Remove(CInstance* pins)
	{
		Assert(pins != 0);

		// Make sure that the world database is unlocked.
		Verify(iLockLevel == 0);

		// Send a delete message to warn everyone.
		CMessageDelete(pins).Dispatch();

		// Removing a camera?
		CCamera* pcam = ptCast<CCamera>(pins);
		if (pcam)
		{
			// We cannot handle multiple cameras at this point.
			Assert(pcamActive == pcam);

			pcamActive = 0;
		}
		// Removing a subsystem?
		else if (ptCast<CSubsystem>(pins) != 0)
		{
			--iNumSubsystems;
			psubList->remove(pins);

			CTerrain* ptrr = ptCast<CTerrain>(pins);
			if (ptrr)
			{
				Assert(ptrrTerrain == ptrr);
				ptrrTerrain = 0;
			}
			return;
		}
		else if (ptCast<CTerrainObj>(pins))
		{
			pins->SetParent();
			return;
		}

		// Removing an entity?
		// Keep track of all non-subsystem entities added to world database.
		CEntity* pet = ptCast<CEntity>(pins);
		if (pet)
		{
			CWDbQueryActiveEntities::lpetActiveEntities.remove(pet);
			
			// Track water too.
			CEntityWater* pew;
			if (pew = dynamic_cast<CEntityWater*>(pins))
			{
				CWDbQueryWater::lspetWater.remove(pew);	
			}
		}

		// Keep track of all lights.
		if (pins->prdtGetRenderInfo() && ptCastRenderType<CLight>(pins->prdtGetRenderInfo()) != 0)
		{
			CWDbQueryLights::lpinsActiveLights.remove(pins);	
		}


		// Remove from the world list.
		pins->SetParent();
		--iNumInstances;
	}
	
	//*****************************************************************************************
	void CWorld::Move(CPartition* ppart, const CPlacement3<>& p3)
	{
		// Assume that we are using the normal partition.
		Move(ppart, p3, ppartPartitions);
	}

	//*****************************************************************************************
	void CWorld::Move(CPartition* ppart, const CPlacement3<>& p3, CPartition* ppart_top)
	{
		//
		// Note: a possible optimization might include checking if the object is still
		// contained by its current partition before removing and reinserting it in the world
		// partition hierarchy.
		//
		Assert(ppart);
		Assert(p3.r3Rot.bIsNormalised());

#if VER_DEBUG
		extern bool bEditTrnObjs;
		// Terrain objs can't move, and don't live in the main partitioning tree.
//		Assert(bEditTrnObjs || !ptCast<CTerrainObj>(ppart));
#endif

		// Change the placement.
		ppart->SetPlacement(p3);

		// If this partition is not currently participating in the world dbase, don't add it.
		if (!ppart->ppartGetParent())
			return;

		// Add the object back into the world database.
		Verify(ppart_top->bAddChild(ppart, true));

		// If this is a shape, invalidate all render caches it intersects.
		if (ppart->pshGetShape())
		{
			//ppartPartitions->InvalidateRenderCache(ppart);
			AddToMovingList(ppart);
		}

		// Reinsert children into the world database.
		{
			TPartitionListChild partlist_children;	// Copy of the child list.

			// Make a copy of the child list to prevent recursion from changing the list.
			if (ppart->bMakeChildList(partlist_children))
			{
				// Recurse through tree, moving instances to the top level node.
				for (partlist_children.Begin(); partlist_children.bIsNotEnd(); partlist_children++)
				{
					CPartition* ppart_child = partlist_children.rtGet();
					if (!ppart->bAddChild(ppart_child, true))
						Verify(ppart_top->bAddChild(ppart_child, true));
				}
			}
		}
	}

	//*****************************************************************************************
	void CWorld::PrunePartitions()
	{
		ppartPartitions->PruneEmpties();
		ppartPartitions->MoveSingleInstancesUp();
		ppartPartitions->PruneEmpties();
	}

	//*****************************************************************************************
	void CWorld::DumpWorld(const char* str_fileout)
	{
		CConsoleBuffer con_out(120, 25);	// Text buffer to write to.

		// Open the text file.
		con_out.OpenFileSession(str_fileout);

		con_out.Print("\nSizes of data members...\n\n");
		con_out.Print("Size of CPartition: %ld\n", sizeof(CPartition));
		con_out.Print("Size of CPartitionSpace (with bounding box): %ld\n",
			          sizeof(CPartitionSpace));
		con_out.Print("Size of CInstance: %ld\n", sizeof(CInstance));
		con_out.Print("Size of CPresence3<>: %ld\n", sizeof(CPresence3<>));
		con_out.Print("Size of SPartitionData: %ld\n", sizeof(CPartition::SPartitionData));
		con_out.Print("Size of CBoundVolBox: %ld\n", sizeof(CBoundVolBox));

		int i_ccd = 0;
		con_out.Print("\n\nTotal spatial volume: %1.1f\n", fGetTotalSpatialVol(*ppartPartitions, i_ccd));
		con_out.Print("Cumulative Component Dependency: %ld\n\n", i_ccd);

		{
			const CPartitionSpace ps;
			TPartitionList partlist;
			ppartPartitions->BuildPartList(&ps, partlist);

			// Iterate through the world object list.
			con_out.Print("\nWorld Objects...\n\n");
			for (partlist.Begin(); partlist; partlist++)
			{
				// Print out type of object.
				con_out.Print((*partlist).ppart->strPartType());

				// Print out the position and size of the object.
				CPresence3<> pr3 = (*partlist).ppart->pr3Presence();

				con_out.Print("\nx: %1.1f, y: %1.1f, z: %1.1f, s: %1.1f\n\n", pr3.v3Pos.tX,
							  pr3.v3Pos.tY, pr3.v3Pos.tZ, pr3.rScale);
			}
		}

		{
			std::list<CInstance*>::iterator it_psub;

			// Iterate through the subsystem object list.
			con_out.Print("\n\nSubsystems...\n\n");\
			for (it_psub = psubList->begin(); it_psub != psubList->end(); it_psub++)
			{
				// Print out type of object.
				con_out.Print((*it_psub)->strPartType());
				con_out.Print("\n");
			}
		}

		{
			const CPartitionSpace ps;
			TPartitionList partlist;

			con_out.Print("\n\nTriggers...\n\n");
			ppartTriggers->BuildPartList(&ps, partlist);
			for (partlist.Begin(); partlist; partlist++)
			{
				// Print out type of object.
				con_out.Print((*partlist).ppart->strPartType());

				// Print out the position and size of the object.
				CPresence3<> pr3 = (*partlist).ppart->pr3Presence();

				con_out.Print("\nx: %1.1f, y: %1.1f, z: %1.1f, s: %1.1f\n\n", pr3.v3Pos.tX,
							  pr3.v3Pos.tY, pr3.v3Pos.tZ, pr3.rScale);
			}
		}
		
		// Write the text file to disk.
		con_out.CloseFileSession();
	}
	
	//*****************************************************************************************
	void CWorld::DumpSpatial()
	{
		CConsoleBuffer con_out(256, 10);	// Text buffer to write to.

		// Open the text file.
		con_out.SetTabSize(4);
		con_out.OpenFileSession("Spatial.txt");
		con_out.Print("\nFlag Key (format 12345):\n\n");
		con_out.Print("\t1: bOcclude\n");
		con_out.Print("\t2: bCacheableVolume\n");
		con_out.Print("\t3: bCacheable\n");
		con_out.Print("\t4: bCacheIntersecting\n");
		con_out.Print("\t5: bCastShadow\n");
		
		con_out.Print("\nSpatial Hierarchy...\n\n");

		// Traverse the spatial hierarchy, and write to disk.
		ppartPartitions->Write(con_out);
		ppartTerrainObjs->Write(con_out);
		ppartTriggers->Write(con_out);
		
		// Write the text file to disk.
		con_out.Print("\nFinished!");
		con_out.CloseFileSession();
	}
	
	//*****************************************************************************************
	bool CWorld::bSaveWorld(const char* str_filename)
	{
	#if (BUILDVER_MODE != MODE_FINAL)

		if (bCheckForDuplicates)
		{
			bCheckForDuplicates = false;

			// Remove duplicates from the regular partitioning system.
			CPartition::BeginDuplicateList();
			ppartPartitions->RemoveDuplicates();
			CPartition::EndDuplicateList();

			// Remove duplicates from the trigger partitioning system.
			CPartition::BeginDuplicateList();
			ppartTriggers->RemoveDuplicates();
			CPartition::EndDuplicateList();

			// Remove duplicates from the terrain object partitioning system.
			CPartition::BeginDuplicateList();
			ppartTerrainObjs->RemoveDuplicates();
			CPartition::EndDuplicateList();

			dprintf("\n\n%ld duplicate partitions removed when saving!\n\n", CPartition::iDuplicateCount);

			AssertOnDuplicates();
		}

	#endif

		// Open the save file.
		CSaveFile sf(str_filename, false, true);	
		if (!sf.bValidFile)
			return false;

        return bSaveTheWorld(&sf);
	}


	//*****************************************************************************************
	bool CWorld::bSaveTheWorld(CSaveFile * pSF)
    {
        // Note:  This call might have to go before the creation of the save file
		CHitSpang::RemoveVisible();

		// First write the Groffs.
		TListConstChar::iterator itstr = plsstrGroffsLoaded->begin();
		for (; itstr != plsstrGroffsLoaded->end(); ++itstr)
		{
			pSF->AddGROFF(*itstr);
		}

		pSF->SaveHeader();

		// For debug verification!  Hold al the handles as you save them to make sure we have no collisions.
		// Clear the handle map.
		ClearHandleMap();

		// Get a buffer with plenty of space for partitions.
		char *pc_buffer = pSF->pcBuffer;

		// Save the partition data into the buffer.
		char *pc = pcSaveSpatialPartitions(pc_buffer);
		
		// Put the data in the save file.
		Assert(pc - pc_buffer < iPLENTY_BYTES);
		Assert(pc - pc_buffer >= 0);
		pSF->bSave("Partitions", pc_buffer, pc - pc_buffer);

		//
		// Add all instances to the save file.
		//

		// Now write all instance positions.
		TPartitionList partlist;	
		const CPartitionSpace ps;

		ppartPartitions->BuildPartList(&ps, partlist);
		ppartTriggers->BuildPartList(&ps, partlist);

		// Handle the camera as a special case.
		if (pcamActive)
		{
			// Update the handle map for save verification.
			AddToHandleMap(pcamActive->u4GetUniqueHandle(), pcamActive);

			pSF->Save(pcamActive);
		}

		foreach (partlist)
		{
			CInstance* pins = ptCast<CInstance>(partlist->ppart);
			if (pins)
			{
				// Update the handle map for save verification.
				AddToHandleMap(pins->u4GetUniqueHandle(), pins);
				
				pSF->Save(pins);
			}
		}

		// Save the subsystems.
		TListInstance::iterator i;
		for (i = psubList->begin(); i != psubList->end(); ++i)
		{
			CInstance *pins_sub = *i;

			// Update the handle map for save verification.
			AddToHandleMap(pins_sub->u4GetUniqueHandle(), pins_sub);

			pSF->Save(pins_sub);
		}

		// Save the partition hierarchy into the buffer.
		SaveHierarchyInfo(pSF, "Hierarchy", ppartPartitions);

		// Save the partition hierarchy into the buffer.
		SaveHierarchyInfo(pSF, "TerrainHierarchy", ppartTerrainObjs);

		// Save the partition hierarchy into the buffer.
		SaveHierarchyInfo(pSF, "TriggerHierarchy", ppartTriggers);
		
		// Now write the file.
		AlwaysVerify(pSF->bWrite());

		// Clear the handle map just in case.
		ClearHandleMap();

		return true;
    }

    
	//*****************************************************************************************
    const char *strAfterSlash(const char *str)
	{
		// Scans back in str until it finds a slash.  
		// Returns a pointer to the character immediately after the string.

		const char *str_return = str + strlen(str);

		// Scan back from the end, and find the beginning of the string, or a slash.
		for ( ; str_return > str; str_return--)
		{	
			if (*str_return == '\\' || *str_return == '/')
			{
				// We've found a short name!
				str_return++;

				return str_return;
			}
		}

		// Found no slash.  Return a null string.
		return 0;

	}


	//*****************************************************************************************
	int32 CWorld::bLoadScene(const char* str_filename, PFNWORLDLOADNOTIFY pfnLoadNotify, uint32 ui32_NotifyParam)
	{
		// Loads a scene, whether or not the database already has data in it.

		//  If we have the GROFFs we need, just reset.
		//  Otherwise, reset the world database and then load the groffs.


		// True if we already have the right GROFFs loaded.
		bool b_reset = true;
		bCheckForDuplicates = false;

		CHitSpang::RemoveVisible();

		// Scope control to avoid having the same savefile open twice.
		{
			// Open a save file for reading.
			CSaveFile sf(str_filename, true, false);
			if (!sf.bValidFile)
				return -1;

			for (int i_groff = 0; i_groff < iNUM_GROFFS; ++i_groff)
			{
				if (sf.shHeader.strGROFF[i_groff][0] != '\0')
				{	
					bool b_already_have_it = false;

					// Need this GROFF file.  Do we already have it?
					std::list<const char*>::iterator i;
					for (i = plsstrGroffsLoaded->begin(); i != plsstrGroffsLoaded->end(); ++i)
					{
						// Is this the GROFF we are looking for?
						
						// Find the slash.
						const char * str1 = strAfterSlash(*i);
						const char * str2 = strAfterSlash(sf.shHeader.strGROFF[i_groff]);
						

						if (str1 && str2 && !strcmp(str1, str2))
						{	
							// Yes!
							b_already_have_it = true;
							break;
						}
					}

					if (!b_already_have_it)
					{
						// No good.  We need to reload the GROFFs.
						b_reset = false;
						break;
					}
				}
			}
		}

		if (!b_reset)
		{
			// Confusing notation here-  in save files, "reset" means that the objects are all
			// present and we just need to "reset" thier positions.

			// In the WDBase, "reset" means destroy all objects and clean out the database.
			// Therefore, if we are just "reseting" the .scn file, we DON'T reset the database.
			// If we are loading the .scn file from scratch, we MUST reset the database.
			Reset();
		}

		return bLoadWorld(str_filename, b_reset, pfnLoadNotify, ui32_NotifyParam);
	}

	//*****************************************************************************************
	int32 CWorld::bLoadWorld(const char* str_filename, bool b_reset, PFNWORLDLOADNOTIFY pfnLoadNotify, uint32 ui32_NotifyParam)
	{
        int32 i4_ret = 0;

		CHitSpang::RemoveVisible();

		// Open a save file for reading.
		CSaveFile sf(str_filename, true, false);
		if (!sf.bValidFile)
			return -1;

		//
		// Some cleanup before we begin!
		//

		// Kill all sounds.
		if (padAudioDaemon)
			padAudioDaemon->KillAllSounds();

		// Kill all image caches.

		// Clear the handle map.
		ClearHandleMap();

		// Clear out physics.
		pphSystem->Clear();

		//
		//	Now to load!
		//
		
		// Is it just a reset request?
		if (!b_reset)
		{
			// No!  Reload the GROFF files.

			char buffer[MAX_PATH];
            char sz[MAX_PATH];
			strcpy(buffer, str_filename);

			char *pc_end = buffer + strlen(buffer);

			// Scan back in the filename to find the begining or a slash.
			for ( ; pc_end > buffer; pc_end--)
			{	
				if (*pc_end == '\\' || *pc_end == '/')
				{
					pc_end++;

					// We've found a short name!
					break;
				}
			}

			// Mark the end of the string.
			*pc_end = '\0';

			int i_groff = 0;
			for (; i_groff < iNUM_GROFFS; ++i_groff)
			{
				if (sf.shHeader.strGROFF[i_groff][0] != '\0')
				{
					char *pc = sf.shHeader.strGROFF[i_groff];
					const char *pc_short_name = strAfterSlash(pc);
					bool b_found = false;
                    int icTimes = 0;
                    char * psz = NULL;

                    int aiOrder[] = { 0, 1, 2};

                    if (!bIsTrespasser)
                    {
                        aiOrder[1] = 2;
                        aiOrder[2] = 1;
                    }

					int i = 0;
                    for (; i < 3 && !b_found; i++)
                    {
                        if (pfnLoadNotify)
                        {
                            (pfnLoadNotify)(ui32_NotifyParam, 0, 0, 0);
                        }

                        switch (aiOrder[i])
                        {
                            case 0:
					            if (pc_short_name)
					            {
						            strcat(buffer, pc_short_name);
                                    psz = buffer;
					            }
                                break;

                            case 1:
                                if (pc_short_name &&
                                    GetRegString(REG_KEY_DATA_DRIVE, sz, sizeof(sz), "") > 0)
                                {
                                    strcat(sz, "\\data\\");
                                    strcat(sz, pc_short_name);

                                    psz = sz;
                                }
                                break;

                            case 2:
                                psz = sf.shHeader.strGROFF[i_groff];
                                break;

                            default:
                                psz = NULL;
                                break;
                        }

                        if (psz)
                        {
                            if (bFileExists(psz))
                            {
							    CLoadWorld lw(psz, pfnLoadNotify, ui32_NotifyParam);

                                if (lw.i4Error < 0)
                                {
                                    i4_ret = lw.i4Error;
                                }

                                b_found = true;
                            }
                        }
                    }
#if VER_TEST
					if (!bIsTrespasser && !b_found)
					{
						char str_buffer[512];
						sprintf(str_buffer, "%s:\nScene file cannot find %s\n", __FILE__, sf.shHeader.strGROFF[i_groff]);

						if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
							DebuggerBreak();
					}
#endif
				}
			}
		}

        if (i4_ret < 0)
        {
            return i4_ret;
        }

        if (pfnLoadNotify)
        {
			// 90% comletion at this point.
            (pfnLoadNotify)(ui32_NotifyParam, 1, 90, 100);
        }

		// Clear the handle map just in case.
		ClearHandleMap();

		// Slam the step time.
		CMessageStep::sStaticTotal = sf.shHeader.sCurrentTime;
		CMessageStep::sElapsedRealTime = sf.shHeader.sCurrentRealTime;

		// Get the partition raw data.
		int i_size;
		const char *pc = sf.pcLoad("Partitions", &i_size);
		Assert(i_size < iPLENTY_BYTES);
		Assert(i_size >= 0);
		Assert(pc);

		// Load the partitions.
		pc = pcLoadSpatialPartitions(pc, i_size);

		// Now load the instance stuff.
		TPartitionList partlist;	
		const CPartitionSpace ps;

		ppartPartitions->BuildPartList(&ps, partlist);
		ppartTerrainObjs->BuildPartList(&ps, partlist);
		ppartTriggers->BuildPartList(&ps, partlist);

		foreach (partlist)
		{
			// Update the handle map for quick access.
			AddToHandleMap(partlist->ppart->u4GetUniqueHandle(), partlist->ppart);
		}

		// Handle the camera as a special case.
		if (pcamActive)
		{
			sf.Load(pcamActive);
			AddToHandleMap(pcamActive->u4GetUniqueHandle(), pcamActive);
		}

		foreach (partlist)
		{
			CInstance* pins = ptCast<CInstance>(partlist->ppart);
			if (pins)
			{
				sf.Load(pins);
			}
		}

		if (pfnLoadNotify)
		{
			// 91% completion at this point.
			(pfnLoadNotify)(ui32_NotifyParam, 1, 91, 100);
		}

		// Load the subsystems.
		TListInstance::iterator i;
		for (i = psubList->begin(); i != psubList->end(); ++i)
		{
			sf.Load(*i);
		}

		#if VER_TEST && bPRINT_PARTITION_SEARCH_TIMES
			cySearchPartitionMap = 0;
			cySearchInstance     = 0;

			iFoundPartitionInMapCount = 0;
			iSearchPartitionCount     = 0;
		#endif

		// Load Hierarchy Info.
		LoadHierarchyInfo(&sf, "Hierarchy", ppartPartitions);
		LoadHierarchyInfo(&sf, "TerrainHierarchy", ppartTerrainObjs);
		LoadHierarchyInfo(&sf, "TriggerHierarchy", ppartTriggers);

		#if VER_TEST && bPRINT_PARTITION_SEARCH_TIMES
			dout << "Num partition searches: " << iSearchPartitionCount << '\n';
			dout << "Num partitions found in STL map: " << iFoundPartitionInMapCount << '\n';

			dout << "Time spent searching STL map: " << cySearchPartitionMap * CCycleTimer::fSecondsPerCycle() << " secs.\n";
			dout << "Time spent searching partitions: " << cySearchInstance * CCycleTimer::fSecondsPerCycle() << " secs.\n";
		#endif

		// Send the quality change message to make sure all our settings are in effect.
		CMessageSystem msgsys(escQUALITY_CHANGE);
		msgsys.Dispatch();

		if (pfnLoadNotify)
		{
			// 92% completion at this point.
			(pfnLoadNotify)(ui32_NotifyParam, 1, 92, 100);
		}

		// Initialize partition flags.
		InitializePartitions();

		// Debugging info.
		#if VER_TEST
			DumpSpatial();
		#endif

		if (pfnLoadNotify)
		{

			// Leave 5% for loading D3D surfaces.
			if (d3dDriver.bUseD3D())
			{
				// 95% completion at this point.
				(pfnLoadNotify)(ui32_NotifyParam, 1, 95, 100);
			}
			else
			{
				// 100% completion at this point.  Note 100 does not seem to work.
				(pfnLoadNotify)(ui32_NotifyParam, 1, 99, 100);

				// Short pause so we have the satisfaction of seeing 100%.
				Sleep(500);
			}
		}

		// Send out message informing the system a scene file has finished loading.
		CMessageSystem msgsys_scn(escSCENE_FILE_LOADED);
		msgsys_scn.Send();

		return true;
	}

	//*****************************************************************************************
	bool CWorld::bLoadPartitionFromScene(const char* str_filename)
	{
		CHitSpang::RemoveVisible();

		// Open a save file for reading.
		CSaveFile sf(str_filename, true, false);
		if (!sf.bValidFile)
			return false;

		//
		// Some cleanup before we begin!
		//

		// Clear the handle map.
		ClearHandleMap();

		//
		//	Now to load!
		//
		
		// Slam the step time.
		CMessageStep::sStaticTotal = sf.shHeader.sCurrentTime;
		CMessageStep::sElapsedRealTime = sf.shHeader.sCurrentRealTime;

		// Get the partition raw data.
		int i_size;
		const char *pc = sf.pcLoad("Partitions", &i_size);
		Assert(i_size < iPLENTY_BYTES);
		Assert(i_size >= 0);
		Assert(pc);

		// Load the partitions.
		pc = pcLoadSpatialPartitions(pc, i_size);

		// Now load the instance stuff.
		TPartitionList partlist;	
		const CPartitionSpace ps;

		ppartPartitions->BuildPartList(&ps, partlist);
		ppartTerrainObjs->BuildPartList(&ps, partlist);
		ppartTriggers->BuildPartList(&ps, partlist);

		foreach (partlist)
		{
			// Update the handle map for quick access.
			AddToHandleMap(partlist->ppart->u4GetUniqueHandle(), partlist->ppart);
		}

		#if VER_TEST && bPRINT_PARTITION_SEARCH_TIMES
			cySearchPartitionMap = 0;
			cySearchInstance     = 0;

			iFoundPartitionInMapCount = 0;
			iSearchPartitionCount     = 0;
		#endif

		// Load Hierarchy Info.
		LoadHierarchyInfo(&sf, "Hierarchy", ppartPartitions, true);
		LoadHierarchyInfo(&sf, "TerrainHierarchy", ppartTerrainObjs, true);
		LoadHierarchyInfo(&sf, "TriggerHierarchy", ppartTriggers, true);

		#if VER_TEST && bPRINT_PARTITION_SEARCH_TIMES
			dout << "Num partition searches: " << iSearchPartitionCount << '\n';
			dout << "Num partitions found in STL map: " << iFoundPartitionInMapCount << '\n';

			dout << "Time spent searching STL map: " << cySearchPartitionMap * CCycleTimer::fSecondsPerCycle() << " secs.\n";
			dout << "Time spent searching partitions: " << cySearchInstance * CCycleTimer::fSecondsPerCycle() << " secs.\n";
		#endif

		// Initialize partition flags.
		InitializePartitions();

		// Debugging info.
		#if VER_TEST
			DumpSpatial();
		#endif

		ResetFrameCounts();

		return true;
	}


	void CWorld::LoadHierarchyInfo(CSaveFile* sf, const char *str_section, CPartition* ppart_base, bool b_error_correct)
	{
		// And finally, load the hierarchy information for the partitions.
		// Get the hierarchy raw data.
		int i_size;		// How big is our buffer?
		const char* pc;	// The buffer itself.

		pc = sf->pcLoad(str_section, &i_size);
		Assert(i_size < iPLENTY_BYTES);
		Assert(i_size >= 0);
		Assert(pc);

		const char *pc_end = pc + i_size;

		// Assert that i_size is divisible by two uint32's
		Assert((i_size / (2 * sizeof(uint32))) * (2 * sizeof(uint32)) == i_size);

		uint32 u4_child;
		uint32 u4_parent;
		TPartitionList partlist_unparented;	


		while(pc < pc_end)
		{
			pc = pcLoadT(pc, &u4_child);
			pc = pcLoadT(pc, &u4_parent);

			//Assert(u4_child != u4_parent);
			if (u4_child == u4_parent)
				continue;

			// Lookup parent and child, and set the parentage.
			CPartition* ppart_child = ppartFindPartition(u4_child, ppart_base);

			if (ppart_child)
			{
				CPartition* ppart_parent = ppartFindPartition(u4_parent, ppart_base);

				// Did we find the parent?
				if (ppart_parent == 0)
				{
					// Try error correction?
					if (b_error_correct)
					{
						// Try to insert the partition in some reasonable fashion.  Save it in a list.
						SPartitionListElement ple = {ppart_child, esfINSIDE};  // the INSIDE bit is ignored, and thus doesn't matter
						partlist_unparented.push_back(ple);
					}
					else
					{
						// No!  Put it in the top level!
						ppart_parent = ppart_base;
#if 0
						CInstance* pins_child = ptCast<CInstance>(ppart_child);
						// Is the child a trigger? (We can ignore triggers, since their spatial info is pointless)
						if (!ptCast<CTrigger>(pins_child))
						{
							// Yes!  It is a trigger.  Warn about it.
							dout << "Load Hierarchy cannot find parent " << u4_parent << " for " << u4_child;
							if (pins_child)
							{
								dout << '(' << pins_child->strGetInstanceName() << ')';
							}
							dout << '\n';
						}
#endif
					}
				}

				ppart_child->SetParent(ppart_parent);
			}
			else
			{
#if VER_TEST
				dout << "Load Hierarchy cannot find child " << u4_child << "\n";
#endif
			}
		}
		Assert(pc == pc_end);

		// If error correcting, try to insert the parentless partitions.
		if (b_error_correct)
		{
			foreach(partlist_unparented)
			{
				ppart_base->bAddChild(partlist_unparented.tGet().ppart);
			}
		}

		ResetFrameCounts();
	}

	//*****************************************************************************************
	void CWorld::SaveHierarchyInfo(CSaveFile* sf, const char *str_section, CPartition* ppart_base)
	{
		char *pc_buffer = sf->pcBuffer;
		// Save the partition hierarchy into the buffer.
		char *pc = ppart_base->pcSaveHierarchy(pc_buffer);
		
		// Put the data in the save file.
		Assert(pc - pc_buffer < iPLENTY_BYTES);
		Assert(pc - pc_buffer >= 0);

		AlwaysAssert(sf->bSave(str_section, pc_buffer, pc - pc_buffer));
	}

	//*****************************************************************************************
	void CWorld::FlattenPartitions()
	{
		Assert(ppartPartitions);

		// Flatten the existing partitioning structure by removing pure spatial partitions.
		ppartPartitions->Flatten(ppartPartitions);
	}
	
	//*****************************************************************************************
	void CWorld::BuildOptimalPartitions(void (__cdecl *partcallback)(int i))
	{
		Assert(ppartPartitions);

		// Count the number of duplicates removed.
		CPartition::iDuplicateCount = 0;
		
		bCheckForDuplicates = true;

		if (partcallback)
			partcallback(-1);

		// Flatten the existing tree.
		ppartPartitions->Flatten(ppartPartitions);

		// Set perturbations.
		iPerturbBoxAxis = 3;
		fPerturbBox     = 0.0f;

		// Build a new tree.
		BuildOptimizedTree(*ppartPartitions, partcallback);

		if (partcallback)
			partcallback(-4);

		// Remove duplicate partitions.
		CPartition::BeginDuplicateList();
		ppartPartitions->RemoveDuplicates();
		CPartition::EndDuplicateList();

		BuildTerrainPartitions();
		BuildTriggerPartitions();

		// Initialize partitions for image caching.
		InitializePartitions();

	#if (BUILDVER_MODE != MODE_FINAL)
		dprintf("\n\n%ld duplicate partitions removed!\n\n", CPartition::iDuplicateCount);
	#endif
		
		AssertOnDuplicates();
	}
	
	//*****************************************************************************************
	void CWorld::BuildTerrainPartitions()
	{
		Assert(ppartTerrainObjs);

		// Flatten the existing tree.
		ppartTerrainObjs->Flatten(ppartTerrainObjs);

		// Set perturbations.
		iPerturbBoxAxis = 0;
		fPerturbBox     = 0.00237f;

		// Build a new tree.
		BuildOptimizedTree(*ppartTerrainObjs, 0, true);

		// Remove duplicate partitions.
		CPartition::BeginDuplicateList();
		ppartTerrainObjs->RemoveDuplicates();
		CPartition::EndDuplicateList();
	}
	
	//*****************************************************************************************
	void CWorld::BuildTriggerPartitions()
	{
		Assert(ppartTriggers);

		// Flatten the existing tree.
		ppartTriggers->Flatten(ppartTriggers);

		// Set perturbations.
		iPerturbBoxAxis = 2;
		fPerturbBox     = 0.00251f;

		// Build a new tree.
		BuildOptimizedTree(*ppartTriggers, 0);

		// Remove duplicate partitions.
		CPartition::BeginDuplicateList();
		ppartTriggers->RemoveDuplicates();
		CPartition::EndDuplicateList();
	}

	//*****************************************************************************************
	CPartition* CWorld::ppartGetWorldSpace() const
	{
		Assert(ppartPartitions);
		Assert(ppartPartitions->ppartChildren());

		std::list<CPartition*> list_part;	// List of spatial partitions.
		int i_count = 0;

		// Construct a list of spatial partitions.
		CPartition* ppartc = (CPartition*)ppartPartitions->ppartChildren();

		for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
		{
			if (!(*it)->bNoSpatialInfo())
			{
				list_part.push_back(*it);
				i_count++;
			}
		}

		Assert(i_count > 1);

		//
		// In the special case that the count is one, construct a dummy spatial partition.
		//
		if (i_count == 1)
		{
			CPartitionVol<2> partvol;	// Partition set.

			// Add partitions.
			partvol.Set(*list_part.begin());
			partvol.Set(*list_part.begin());
			return partvol.ppartNewPart();
		}

		//
		// Do it for two partitions for now.
		//
		if (i_count == 2)
		{
			CPartitionVol<2> partvol;				// Partition set.
			std::list<CPartition*>::iterator it_partlist;	// Partition list iterator.

			// Add partitions.
			it_partlist = list_part.begin();
			partvol.Set(*it_partlist);
			it_partlist++;
			partvol.Set(*it_partlist);
			return partvol.ppartNewPart();
		}

		//
		// Not done for more than two partitions.
		//
		Assert(false);
		return 0;
	}
	
	//*****************************************************************************************
	void CWorld::GetWorldExtents(CVector3<>& v3_min, CVector3<>& v3_max) const
	{
		// Combine the extents of the top-level partition, and any terrain.
		Assert(ppartPartitionList());
		Verify(ppartPartitionList()->bGetWorldExtents(v3_min, v3_min));
		if (ptrrTerrain)
		{
			CVector3<> v3_min_trr, v3_max_trr;
			if (ptrrTerrain->bGetWorldExtents(v3_min_trr, v3_max_trr))
			{
				v3_min.SetMin(v3_min_trr);
				v3_max.SetMax(v3_max_trr);
			}
		}
	}

	//*****************************************************************************************
	void CWorld::Purge()
	{
        if (bHasBeenPurged)
        {
            return;
        }

		// Make sure that the audio daemon has no thread data hanging out in the audio system!
		padAudioDaemon->CleanUp();

		// Make sure Direct3D doesn't have any surprises.
		d3dDriver.Purge();
		
		bCheckForDuplicates = false;

		// Disable scene saving for asserts.  We cannot autosave when the partitions are gone!
		bAutoSaveScene = false;

		// Empty the trash in the gameloop.
		gmlGameLoop.EmptyTrash();

		// Get rid of the render cache moved and stop lists.
		DumpMoveStopLists();

		// Remove hit spangs and muzzle flashes before we lock the worldDBase
		CHitSpang::Remove();
		CMuzzleFlash::ClearMuzzleFlashList();

		Lock();

		DumpWorld();

		// Clear the active entities list.
		CWDbQueryActiveEntities::lpetActiveEntities.erase(CWDbQueryActiveEntities::lpetActiveEntities.begin(), 
															CWDbQueryActiveEntities::lpetActiveEntities.end());
		CWDbQueryLights::lpinsActiveLights.erase(CWDbQueryLights::lpinsActiveLights.begin(),
													CWDbQueryLights::lpinsActiveLights.end());

		tmPlatonicIdeal.erase(tmPlatonicIdeal.begin(), tmPlatonicIdeal.end());

		
		// Remove the partitioning structure.
		::delete ppartPartitions;
		::delete ppartBackdrops;
		::delete ppartTriggers;
		::delete ppartTerrainObjs;
		ppartPartitions  = 0;
		ppartBackdrops   = 0;
		ppartTriggers    = 0;
		ppartTerrainObjs = 0;

		// Delete the subsystems.
		for (TListInstance::iterator it_psub = psubList->begin(); it_psub != psubList->end(); it_psub++)
			delete (*it_psub);
		// Erase the entries.
		psubList->erase(psubList->begin(), psubList->end());

		// Zero the global subsystem pointers.
#ifdef USING_FETCH
		pddDataDaemon = 0;
#endif
		gpaiSystem = 0;
		gpInputDeemone = 0;
		ptrrTerrain = 0;
		ps_daemon = 0;
		ps_renderDB = 0;
		pVideoOverlay = 0;
		pagAutoGrabber = 0;
		pAnimations = 0;
		pCameraPrime = 0;
		pBloodSplats = 0;
		gpPlayer = 0;

		// Delete the sky, if there is one.
		CSkyRender::RemoveSky();

		// Delete the camera.
		delete pcamActive;
		pcamActive = 0;


		// Reset the pure spatial partition heap.
		CPartitionSpace::ResetHeap();

		// Also reset the instance partition heap.
		CInstance::ResetHeap();

		// Free up the GROFFs loaded record.
		TListConstChar::iterator itstr = plsstrGroffsLoaded->begin();
		for (; itstr != plsstrGroffsLoaded->end(); ++itstr)
		{
			free ((void*)(*itstr));
		}
		plsstrGroffsLoaded->erase(plsstrGroffsLoaded->begin(), plsstrGroffsLoaded->end());

		// Get rid of references to meshes in the instancer.
		CLoadWorld::PurgeMeshes();

		// Remove shared CInfo structures.
		CInfo::ReleaseShared();
//		AlwaysAssert(CInfo::iTotal == 1);
	
		// Set the flag to indicate that we've purged.
		bHasBeenPurged = true;

		Unlock();

		//
		//	Now purge all subsidiary arrays.
		//

#define ERASE(foo) foo.erase(foo.begin(), foo.end())

		// tsaiAIInfo
 		typedef std::set<CAIInfo, std::less<CAIInfo> > TSAI;
		extern TSAI tsaiAIInfo;	// A set containing all shared AI infos, for instancing.
		ERASE(tsaiAIInfo);

		// tsiInfo released above-  		CInfo::ReleaseShared();
		//		typedef set<CInfo, less<CInfo> > TSI;
		//extern TSI tsiInfo;	// A set containing all shared infos, for instancing.
		//ERASE(tsiInfo);

		// setHandles
		// Not necessary- setHandles is part of CSaveFile

		// tsmMagnet
		typedef std::set<CMagnet, std::less<CMagnet> > TSMagnet;
		extern TSMagnet tsmMagnet;	// A set containing all shared magnets, for instancing.
		ERASE(tsmMagnet);

		// tsmMaterialInstances
		typedef std::set<CMaterial, std::less<CMaterial> > TSM;
		extern TSM tsmMaterialInstances;
		ERASE(tsmMaterialInstances);

		// tspbPhysicsInfoBox
		typedef std::set<CPhysicsInfoBox, std::less<CPhysicsInfoBox> > TSPB;
		extern TSPB tspbPhysicsInfoBox;	// A set containing all shared box infos, for instancing.
		ERASE(tspbPhysicsInfoBox);

		// Track the skeletons for deletion.
		extern std::list<CPhysicsInfoSkeleton*> lppisPhysicsSkeletons;
		std::list<CPhysicsInfoSkeleton*>::iterator itskel;
		for (itskel = lppisPhysicsSkeletons.begin(); itskel != lppisPhysicsSkeletons.end(); ++itskel)
		{
			delete *itskel;
		}
		ERASE(lppisPhysicsSkeletons);


		// setinsprIgnore
		typedef std::pair<CInstance*, CInstance*> TInstancePair;
		typedef std::set<TInstancePair, CLessInstancePair> TSetInstancePair;
		extern TSetInstancePair setinsprIgnore;
		ERASE(setinsprIgnore);

		// mapTextures
		extern std::map< uint32, rptr<CTexture>, std::less<uint32> > mapTextures;
		ERASE(mapTextures);

		// mapTextureNames
		//extern map< uint32, string, less<uint32> > mapTextureNames;
		//ERASE(mapTextureNames);

		// mapMeshInstances
		extern std::map<uint32, SMeshInstance, std::less<uint32> > mapMeshInstances;
		ERASE(mapMeshInstances);

		// tmspPlatonicIdeal
		typedef std::map<std::string, const CInstance*, std::less<std::string> >	TMapStrPins;
		extern TMapHashPins  tmPlatonicIdeal;
		ERASE(tmPlatonicIdeal);

		// NMagnetSystem::mtSlaveLookup
		// NMagnetSystem::mtMasterLookup
		NMagnetSystem::RemoveAllMagnets();

		// Any app specific details to reset.
		extern void ResetAppData();
		ResetAppData();

		// The global list of water.
		CWDbQueryWater::lspetWater.erase(CWDbQueryWater::lspetWater.begin(), CWDbQueryWater::lspetWater.end());
		AlwaysAssert(CWDbQueryWater::lspetWater.size() == 0);

		//typedef set<SCurvedParentElement, test_curved_parent >	TCurvedParentList;
		//extern TCurvedParentList*			pcplParentBumpMaps;

		// Zero the terrain obj clut pointer.
//		extern ptr<CClut> CTerrainObj::pclutTerrain;
		CTerrainObj::pclutTerrain = 0;

		// Shared by all spherical triggers!
		extern const CInfo* pinfoSphericalTrigger;
		pinfoSphericalTrigger = 0;

#if VER_TEST
		extern std::set<uint32, std::less<uint32> > setSubmodels;
		ERASE(setSubmodels);
#endif

		// A list of stuff to also purge.
		/*
		TCurvedParentList
		default light and default light mesh variables
		list<CPhysicsInfoCompound> lphicCompoundInfos
		 */

		// Make sure all message recipients have been properly unregistered.
		Assert(         CMessageMove::iGetNumRegisteredRecipients() == 0);
		Assert(    CMessageCollision::iGetNumRegisteredRecipients() == 0);
		Assert(      CMessageTrigger::iGetNumRegisteredRecipients() == 0);
		Assert(CMessageMoveTriggerTo::iGetNumRegisteredRecipients() == 0);
		Assert(       CMessagePickUp::iGetNumRegisteredRecipients() == 0);
		Assert(          CMessageUse::iGetNumRegisteredRecipients() == 0);
		Assert(        CMessageDeath::iGetNumRegisteredRecipients() == 0);
		Assert(       CMessageDamage::iGetNumRegisteredRecipients() == 0);
		Assert(       CMessageSystem::iGetNumRegisteredRecipients() == 0);
		Assert(         CMessageStep::iGetNumRegisteredRecipients() == 0);
		Assert(        CMessagePaint::iGetNumRegisteredRecipients() == 0);
		Assert(   CMessagePhysicsReq::iGetNumRegisteredRecipients() == 0);
		Assert(      CMessageControl::iGetNumRegisteredRecipients() == 0);
		Assert(        CMessageAudio::iGetNumRegisteredRecipients() == 0);

		// Force remove all recipients just to be safe.
		         CMessageMove::UnregisterAll();
		    CMessageCollision::UnregisterAll();
		      CMessageTrigger::UnregisterAll();
		CMessageMoveTriggerTo::UnregisterAll();
		       CMessagePickUp::UnregisterAll();
		          CMessageUse::UnregisterAll();
		        CMessageDeath::UnregisterAll();
		       CMessageDamage::UnregisterAll();
		       CMessageSystem::UnregisterAll();
		         CMessageStep::UnregisterAll();
		        CMessagePaint::UnregisterAll();
		   CMessagePhysicsReq::UnregisterAll();
		      CMessageControl::UnregisterAll();
		        CMessageAudio::UnregisterAll();


		//
		// remove the only reference to the light mesh, it will get recreated in
		// the init function.
		//
		extern rptr<CMesh>	pmshLightDir;
		pmshLightDir = rptr0;

		extern rptr<CMesh>	pmshCube;
		pmshCube = rptr0;

		extern rptr<CMesh>	pmshLightPtDir;
		pmshLightPtDir = rptr0;

		extern rptr<CMesh>	pmshLightPt;
		pmshLightPt = rptr0;

		// Release texture pointers.
		EraseTextureMap();

		// Reset the texture manager
		gtxmTexMan.Reset();

		//
		// Reset the load heap
		//
		CLoadImageDirectory::ResetLoadHeap();

		//
		// Reset the instance names
		//
		CInstance::ResetNameHeap();

		//
		// Reset the mesh heap
		//
		CMesh::ResetMeshHeap();

		//
		// Reset the clut database. This is done last because textures may have references to
		// clut palettes.
		//
		pcdbMain.Clear();

		// These counters never get decremented so reset them here
		MEMLOG_SET_COUNTER(emlBumpCurveCount,0);
		MEMLOG_SET_COUNTER(emlBumpMatrix,0);
		MEMLOG_SET_COUNTER(emlTotalPolygon,0);
		MEMLOG_SET_COUNTER(emltotalVertPoint,0);
		MEMLOG_SET_COUNTER(emlTotalVertex,0);
		MEMLOG_SET_COUNTER(emlWrapPoints,0);
		MEMLOG_SET_COUNTER(emlMeshes,0);
		MEMLOG_SET_COUNTER(emlDeletedMeshes,0);
		MEMLOG_SET_COUNTER(emlDeletedMeshMemory,0);
		MEMLOG_SET_COUNTER(emlOcclude,0);
	}



	//*****************************************************************************************
	void CWorld::Preload()
	{
#ifdef USING_FETCH
		// Get the current active camera.
		CWDbQueryActiveCamera wqcam(*this);

		//
		// Build a bounding sphere positioned around the player with a radii larger than the
		// what actually can be seen.
		//

		// Get the radius and extend it.
		TReal r_radius = pddDataDaemon->rFetchDistance(wqcam.tGet()->campropGetProperties().rFarClipPlaneDist);
		CPresence3<> pr3_cam = wqcam.tGet()->pr3Presence();

		// Create the bounding sphere.
		CBoundVolSphere bvs(r_radius);

		// Preload.  Needs presence!
		ppartPartitions->bPreload(&bvs, &pr3_cam, false);
		ppartTerrainObjs->bPreload(&bvs, &pr3_cam, false);
		if (conPreLoader.bFileSessionIsOpen())
		{
			conPreLoader.CloseFileSession();
		}
#endif
	}

	//*****************************************************************************************
	char *CWorld::pcSaveSpatialPartitions(char* pc_buffer)
	{
		Assert(pc_buffer);
		Assert(ppartPartitions);

		// Write the version info out.
		pc_buffer[0] = 1;
		++pc_buffer;

		// Save the partition system starting at the root node.
		pc_buffer = ppartPartitions->pcSaveSpatial(pc_buffer);

		pc_buffer = ppartTriggers->pcSaveSpatial(pc_buffer);

		pc_buffer = ppartTerrainObjs->pcSaveSpatial(pc_buffer);

		// How much of the buffer did we use?
		return pc_buffer;
	}

	//*****************************************************************************************
	const char* CWorld::pcLoadSpatialPartitions(const char* pc, int i_size)
	{
		Assert(pc);
		Assert(ppartPartitions);

		// Calculate the end condition.
		const char* pc_end = pc + i_size;

		// Version control for the file.
		char c_version = pc[0];
		++pc;
		AlwaysAssert(c_version == 1);

		// Flatten the existing partitioning structure by removing pure spatial partitions.
		ppartPartitions->Flatten(ppartPartitions);
		ppartTriggers->Flatten(ppartTriggers);
		ppartTerrainObjs->Flatten(ppartTerrainObjs);

		// Also reset the pure spatial partition heap.
		CPartitionSpace::ResetHeap();

		// Create the partitions.
		while (pc < pc_end)
		{
			CPartitionSpace* pps = new CPartitionSpace();
			pc = pps->pcLoad(pc);

			// Update the handle map for quick lookups.
			AddToHandleMap(pps->u4GetUniqueHandle(), pps);
		}
		Assert(pc == pc_end);
		ResetFrameCounts();

		return pc;
	}
	
	//*****************************************************************************************
	void CWorld::InitializePartitions()
	{
		Assert(ppartPartitions);
		Assert(ppartBackdrops);
		Assert(ppartTriggers);

		ppartPartitions->DeleteAllCaches();
		ppartPartitions->InitializeDataStatic();
		ppartBackdrops->InitializeDataStatic();
		ppartTriggers->InitializeDataStatic();
		ppartTerrainObjs->InitializeDataStaticTerrain();
	}

	//*****************************************************************************************
	const CTerrainObj* CWorld::ptobjGetTopTerrainObjAt
	(
		TReal r_x,		// World X coord
		TReal r_y		// World Y coord
	)
	{
		// Query the terrain partition for objects at X,Y
		
		// Make a tiny spatial partition.
		CPartitionSpace ps = CBoundVolPoint();
		ps.SetPos(CVector3<>(r_x,r_y,0.0f));

		CWDbQueryTerrainObj qto(&ps);


		int i_best_height = -1;
		const CTerrainObj* pto_best = 0;
		foreach(qto)
		{
			const CTerrainObj* pto = (CTerrainObj*) qto.tGet().ppart;

			if (pto->iHeight > i_best_height)
			{
				pto_best = pto;
				i_best_height = pto->iHeight;
			}
		}

		return pto_best;
	}

	//*****************************************************************************************
	CPartition* CWorld::ppartFindPartition(uint32 u4_handle)
	{
		return ppartFindPartition(u4_handle, ppartPartitions);
	}

	//*****************************************************************************************
	CPartition* CWorld::ppartFindPartition(uint32 u4_handle, CPartition* ppart_parent)
	{
		#if VER_TEST && bPRINT_PARTITION_SEARCH_TIMES
			iSearchPartitionCount++;
		#endif

		// Null handle gives a null instance.
		if (u4_handle == 0)
			return 0;

		// Does the map have any entries?
		if (!pmapHandlePointer->empty())
		{
			#if VER_TEST && bPRINT_PARTITION_SEARCH_TIMES
				CCycleTimer ctmr;
			#endif

			// Yes!  Try to find what we're looking for.
			TMapIntPPart::iterator i;
			i = pmapHandlePointer->find(u4_handle);

			#if VER_TEST && bPRINT_PARTITION_SEARCH_TIMES
				cySearchPartitionMap += ctmr();
			#endif


			if (i != pmapHandlePointer->end())
			{
				#if VER_TEST && bPRINT_PARTITION_SEARCH_TIMES
					iFoundPartitionInMapCount++;
				#endif

				return (*i).second;
			}
		}

		#if VER_TEST && bPRINT_PARTITION_SEARCH_TIMES
			CCycleTimer ctmr;
		#endif

		// Failed to find it in the map.  Try the slow way.
		CPartition* ppart_ret = ppart_parent->pinsFindInstance(u4_handle);

		#if VER_TEST && bPRINT_PARTITION_SEARCH_TIMES
			cySearchInstance += ctmr();
		#endif

		return ppart_ret;
	}



	//*****************************************************************************************
	CInstance* CWorld::pinsFindInstance(uint32 u4_handle)
	{
		if (u4_handle == pcamActive->u4GetUniqueHandle())
			return pcamActive;

		CPartition* ppart = ppartFindPartition(u4_handle);
	
		if (ppart)
		{
			CInstance* pins = ptCast<CInstance>(ppart);
			Assert(pins);
			return pins;
		}

		return 0;
	}
 
	//*****************************************************************************************
	bool CWorld::bSaveAsText(const char *str_filename)
	{
		std::ofstream ofs(str_filename);

		if (!ofs.is_open())
			// Some sort of failure.
			return false;

		TPartitionList partlist;	
		const CPartitionSpace ps;

		ppartPartitions->BuildPartList(&ps, partlist);
		ppartTriggers->BuildPartList(&ps, partlist);
		ppartTerrainObjs->BuildPartList(&ps, partlist);
		
		foreach (partlist)
		{
			CInstance* pins = ptCast<CInstance>(partlist->ppart);
			if (pins)
			{
				// Write its class and name.
				ofs << "Instance " << strTypeName(*pins) << ' ' << pins->strGetInstanceName();

				// Write its placement.  
				ofs << ' ' << pins->pr3Presence();

				ofs << ' ' << pins->fGetScale();

				ofs << std::endl;
			}
		}

		return true;
	}

	//*********************************************************************************************
	void CWorld::ClearHandleMap()
	{
		pmapHandlePointer->erase(pmapHandlePointer->begin(), pmapHandlePointer->end());
	}

	//*****************************************************************************************
	const char* CWorld::strGetGroff(uint32 u4_index)
	{
		Assert( (uint32)(plsstrGroffsLoaded->size())>u4_index );

		std::list<const char*>::iterator i;
		uint32						u4_count = 0;

		for (i = plsstrGroffsLoaded->begin(); i != plsstrGroffsLoaded->end(); ++i)
		{
			if (u4_count == u4_index)
				return (*i);
			u4_count++;
		}

		// should never get to here...
		return NULL;
	}

	//*****************************************************************************************
	void CWorld::AssertOnDuplicates()
	{
	#if (BUILDVER_MODE != MODE_FINAL)

		// Test just for partitions.
		{
			TPartitionList partlist;	
			const CPartitionSpace ps;

			ClearHandleMap();
			ppartPartitions->BuildPartList(&ps, partlist);
		}

		// Test just for terrain objects.
		{
			TPartitionList partlist;	
			const CPartitionSpace ps;

			ClearHandleMap();
			ppartTerrainObjs->BuildPartList(&ps, partlist);
		}

		// Test just for triggers.
		{
			TPartitionList partlist;	
			const CPartitionSpace ps;

			ClearHandleMap();
			ppartTriggers->BuildPartList(&ps, partlist);
		}

		// Test just for background objects.
		{
			TPartitionList partlist;	
			const CPartitionSpace ps;

			ClearHandleMap();
			ppartBackdrops->BuildPartList(&ps, partlist);
		}

		TPartitionList partlist;	
		const CPartitionSpace ps;

		// Clear the handle map.
		ClearHandleMap();

		// Add handles from each of the systems.
		ppartPartitions->BuildPartList(&ps, partlist);
		ppartTerrainObjs->BuildPartList(&ps, partlist);
		ppartTriggers->BuildPartList(&ps, partlist);
		ppartBackdrops->BuildPartList(&ps, partlist);

		foreach (partlist)
		{
			// Update the handle map for quick access.
			AddToHandleMap(partlist->ppart->u4GetUniqueHandle(), partlist->ppart);
		}

		// Clear the handle again.
		ClearHandleMap();

	#endif // (BUILDVER_MODE != MODE_FINAL)
	}

	//*****************************************************************************************
	void CWorld::AddToHandleMap(uint32 u4_handle, CPartition* ppart)
	{
		// Don't add the zero handles.
		if (u4_handle == 0)
			return;
		
	#if (BUILDVER_MODE != MODE_FINAL)

		// Determine if a duplicate handle exists.
		TMapIntPPart::iterator it_find = pmapHandlePointer->find(u4_handle);
		if (it_find != pmapHandlePointer->end())
		{	
			char str_buffer[1024];	// A buffer for the impending error message.
			CPartition* ppart_dup = (*it_find).second;
			
			// Test for collisions between two pure spatial partitions.
			if (ppart->bIsPureSpatial() && ppart_dup->bIsPureSpatial())
			{
				// We have two partitions colliding.  Yikes!
				sprintf(str_buffer, "Partition vs Partition hash collision!  Get Paul immediately!\n");
				
				if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
					DebuggerBreak();
				return;
			}

			// Check for collisions involving instances.
			CInstance* pins1 = ptCast<CInstance>(ppart);
			CInstance* pins2 = ptCast<CInstance>(ppart_dup);

			// We have at least one instance.  Do we have two?
			if (ppart->bIsPureSpatial() || ppart_dup->bIsPureSpatial())
			{
				// A single instance has collided with a partition.
				CInstance* pins = pins1 ? pins1 : pins2;
				sprintf(str_buffer, "%s:\nHash collision between %s and a partition.\nRename the instance.\n", __FILE__, pins->strGetInstanceName());

				if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
					DebuggerBreak();
			}

			// Yes!  Two instances have collided.
			sprintf(str_buffer, "%s:\nHash collision between %s and %s\n\n", __FILE__, pins1->strGetInstanceName(), pins2->strGetInstanceName());

			if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
				DebuggerBreak();

			// Do the only thing that can be done.
			return;
		}

	#endif // (BUILDVER_MODE != MODE_FINAL)

		// The app is in final mode, or this handle is not duplicated.
		pmapHandlePointer->insert(THandlePair(u4_handle, ppart));
	}

	//*****************************************************************************************
	void CWorld::AddBackdrop(CInstance* pins)
	{
		Assert(pins);
		Assert(ppartBackdrops);

		//AddWithNoLock(pins);
		Verify(ppartBackdrops->bAddChild(pins));
	}

	//*****************************************************************************************
	void CWorld::ResetFrameCounts()
	{
		if (ppartPartitions)
			ppartPartitions->ResetFrameCounts();
	}

	//*****************************************************************************************
	void CWorld::RestoreSubsystemDefaults()
	{
		// Iterate through the subsystems.
		TListInstance::iterator i;
		for (i = psubList->begin(); i != psubList->end(); ++i)
		{
			CSubsystem* psub = dynamic_cast<CSubsystem*>(*i);
			
			if (psub)
				psub->RestoreDefaults();
		}
	}

	//*****************************************************************************************
	//
	void CWorld::DeferredLoad
	(
		const char *str_name
	)
	//
	// Set the level we want to load (before next frame).
	//
	//**********************************
	{
		bLoadPending = true;
		strLoadName = str_name;
	}

	//*****************************************************************************************
	//
	bool CWorld::bIsLoadPending
	(
	)
	//
	// Returns true if a deferred load is pending.
	//
	//**********************************
	{
		return bLoadPending;
	}

	//*****************************************************************************************
	//
	const std::string& CWorld::strGetPendingLoad
	(
	)
	//
	// Get the pending load level name and clear it.
	//
	//**********************************
	{
		bLoadPending = false;
		return strLoadName;
	}

	//*****************************************************************************************
	//
	void CWorld::DeferredSave
	(
		const char *str_name
	)
	//
	// Set the level we want to save to (before next frame).
	//
	//**********************************
	{
		bSavePending = true;
		strSaveName = str_name;
	}

	//*****************************************************************************************
	//
	bool CWorld::bIsSavePending
	(
	)
	//
	// Returns true if a deferred save is pending.
	//
	//**********************************
	{
		return bSavePending;
	}

	//*****************************************************************************************
	//
	const std::string& CWorld::strGetPendingSave
	(
	)
	//
	// Get the pending save level name and clear it.
	//
	//**********************************
	{
		bSavePending = false;
		return strSaveName;
	}

//
// Global variables.
//
CWorld* pwWorld = 0;

	//
	//	Global functions.
	//

	//*****************************************************************************************
	char * pcSaveInstancePointer(char * pc,	const CInstance* pins)
	{

		uint32 u4 = 0;

		// If pins exists, use its handle.  Otherwise, save a zero.
		if (pins)
			u4 = pins->u4GetUniqueHandle();

		// Save the data.
		*((uint32*)pc) = u4;

		// Return the next free byte.
		return pc + sizeof(uint32);
	}
	

	//*****************************************************************************************
	const char * pcLoadInstancePointer(const char* pc,	CInstance** ppins)
	{
		uint32 u4 = *((uint32*)pc);

		// Did we find a valid handle (non-zero)?
		if (u4)
		{
			// Yes!  Look up the instance.
			*ppins = wWorld.pinsFindInstance(u4);
			AlwaysAssert(*ppins);
		}
		else
			// A null instance was indicated.
			*ppins = 0;

		return pc + sizeof(uint32);
	}

	

	int iQualitySetting = iDEFAULT_QUALITY_SETTING;
	//*****************************************************************************************
	int iGetQualitySetting()
	{
		return iQualitySetting;
	}

	//*****************************************************************************************
	float fGetQualitySetting()
	{
		return iQualitySetting * (1.0f / iMAX_QUALITY_SETTING);
	}

	//*****************************************************************************************
	void SetQualitySetting(int i_quality)
	{
		AlwaysAssert(i_quality <= iMAX_QUALITY_SETTING);
		AlwaysAssert(i_quality >= 0);

		iQualitySetting = i_quality;

		// Send the quality change message to all systems.
		CMessageSystem msgsys(escQUALITY_CHANGE);
		msgsys.Dispatch();

		// Rebuild the world database (needed for changed culling parameters).
		if (pwWorld)
			pwWorld->InitializePartitions();
	}
