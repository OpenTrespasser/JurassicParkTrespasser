/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents: The implementation of Instance.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Instance.cpp                                         $
 * 
 * 247   98/10/04 2:34 Speter
 * Added bFloats to iGetDescription.
 * 
 * 246   10/01/98 6:49p Mlange
 * Improved collision message stats.
 * 
 * 245   10/01/98 4:05p Mlange
 * Improved move message stat reporting.
 * 
 * 244   98/09/27 18:06 Speter
 * Re-enable text prop particles, but disable it for terrain materials; also, they will not
 * override code defaults.
 * 
 * 243   9/24/98 5:40p Asouth
 * operator new[] #ifdefs
 * 
 * 242   98.09.22 12:25a Mmouni
 * Added warning for priority culling tangible objects.
 * 
 * 241   9/20/98 7:32p Agrant
 * Remove Hit Spangs
 * 
 *********************************************************************************************/

#include "Common.hpp"
#include "Instance.hpp"
#include "Gameloop.hpp"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/Loader/DataDaemon.hpp"
#include "MessageTypes/MsgMove.hpp"
#include "Lib/GeomDBase/Shape.hpp"
#include "PhysicsInfo.hpp"
#include "Game/AI/AIInfo.hpp"
#include "Lib/Trigger/Trigger.hpp"
#include "Lib/EntityDBase/Teleport.hpp"
#include "Lib/EntityDBase/Query/QMessage.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/FastHeap.hpp"

#include "Lib/Groff/ValueTable.hpp"
#include "Lib/Groff/VTParse.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Loader/PlatonicInstance.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "Lib/Physics/InfoBox.hpp"
#include "Lib/Physics/InfoSkeleton.hpp"
#include "Lib/Physics/InfoPlayer.hpp"
#include "Lib/Physics/Magnet.hpp"
#include "Lib/Physics/WaterDisturbance.hpp"

#include "Lib/Renderer/Light.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/EntityDBase/EntityLight.hpp"
#include "Lib/Renderer/Sky.hpp"
#include "Lib/Renderer/LightBlend.hpp"
#include "Lib/EntityDBase/ParticleGen.hpp"

#include "Game/DesignDaemon/Gun.hpp"
#include "Game/DesignDaemon/HitSpang.hpp"
#include "Game/DesignDaemon/Socket.hpp"

#include "Lib\Std\Hash.hpp"

#include "Lib/Sys/Profile.hpp"

CProfileStat	psMove("(Move)", &proProfile.psFrame, Set(epfOVERLAP) + epfHIDDEN),

				psMoveMsg("(MoveMsg)", &proProfile.psFrame, Set(epfOVERLAP) + epfOTHER + epfHIDDEN),
					psMoveMsgBrain(         "Brain",     &psMoveMsg, Set(epfHIDDEN)),
					psMoveMsgTerrain(       "Terrain",   &psMoveMsg, Set(epfHIDDEN)),
					psMoveMsgTrigger(       "Triggers",  &psMoveMsg, Set(epfHIDDEN)),
					psMoveMsgGun(           "Gun",       &psMoveMsg, Set(epfHIDDEN)),
					psMoveMsgPlayer(        "Player",    &psMoveMsg, Set(epfHIDDEN)),
					psMoveMsgAudio(         "Audio",     &psMoveMsg, Set(epfHIDDEN)),
					psMoveMsgEntityAttached("Ent. Att.", &psMoveMsg, Set(epfHIDDEN)),
					psMoveMsgRenderDB(      "Render DB", &psMoveMsg, Set(epfHIDDEN)),
					psMoveMsgPhysics(       "Physics",   &psMoveMsg, Set(epfHIDDEN)),
					psMoveMsgQuery(         "Query",     &psMoveMsg, Set(epfHIDDEN)),
						
				psCollisionMsg("Collide Msg", &proProfile.psFrame, Set(epfOVERLAP) + epfOTHER + epfHIDDEN),
					psCollisionMsgConstruct("Construct", &psCollisionMsg, Set(epfHIDDEN)),
					psCollisionMsgPlayer(   "Player",    &psCollisionMsg, Set(epfHIDDEN)),
					psCollisionMsgAudio(    "Audio",     &psCollisionMsg, Set(epfHIDDEN)),
					psCollisionMsgBrain(    "Brain",     &psCollisionMsg, Set(epfHIDDEN)),
					psCollisionMsgAnimate(  "Animate",   &psCollisionMsg, Set(epfHIDDEN)),
					psCollisionMsgPhysics(  "Physics",   &psCollisionMsg, Set(epfHIDDEN)),
					psCollisionMsgTrigger(  "Triggers",  &psCollisionMsg, Set(epfHIDDEN));

//
// Module specific variables.
//
extern CFastHeap fhInstance;

#if VER_TEST
	// Make the master marker.
	CInstance *pinsMasterMarker = 0;
	int iMaxMarker = 0;
	char strMarkerName[256];
#endif


// Shared by all spherical triggers!
const CInfo* pinfoSphericalTrigger = 0;

//  Local function helper for pinsCreate
static void ProcessSettings(CGroffObjectName* pgon,
							 CLoadWorld*	pload,
							 const CHandle& h,
							 CValueTable* pvt,
							 const CInfo* pinfo)
{
	// Sets up global variables for an area.

	SETUP_TEXT_PROCESSING(pvt, pload)
	SETUP_OBJECT_HANDLE(h)
	{

		// 
		int i_alpha_channel = -1;
		if (bFILL_INT(i_alpha_channel, esAlphaChannel))
		{
			int i_red = 0;
			int i_green = 0;
			int i_blue = 0;
			float f_alpha = 0.0f;

			bFILL_INT(i_red, esR);
			bFILL_INT(i_green, esG);
			bFILL_INT(i_blue, esB);
			bFILL_FLOAT(f_alpha, esAlpha);

			CLightBlend::SLightBlendSettings* plbs = lbAlphaConstant.lpsSettings + i_alpha_channel;
			plbs->clrBlendColour = CColour(i_red, i_green, i_blue);
			plbs->fAlpha = f_alpha;
		}
	}
	END_OBJECT_HANDLE;
	END_TEXT_PROCESSING;
}


namespace
{
	// Local helper function that determines if a given mesh has any surfaces with mipmaps.
	bool bMeshHasMips(rptr<CMesh> pmsh)
	{
		// Iterate surfaces.
		for (int i_surf = 0; i_surf < pmsh->pasfSurfaces.uLen; i_surf++)
		{
			rptr<CTexture> ptex = pmsh->pasfSurfaces[i_surf].ptexTexture;

			if (ptex && ptex->iGetNumMipLevels() > 1)
			{
				// This texture has multiple rasters. If their sizes are different, this mesh
				// has actual mipmaps.
				rptr<CRaster> pras_0 = ptex->prasGetTexture(0);
				rptr<CRaster> pras_1 = ptex->prasGetTexture(1);

				if (pras_0->iWidth > pras_1->iWidth || pras_0->iHeight > pras_1->iHeight)
					return true;
			}
		}

		return false;
	}
};


//
// Class implementations.
//

//*********************************************************************************************
//
// CInfo implementation.
//

	//*****************************************************************************************
	CInfo::CInfo()
	{
		MEMLOG_ADD_COUNTER(emlTotalCInfo,1);
	}

	//*****************************************************************************************
	CInfo::CInfo
	(
		bool b_is_unique,
		rptr<CRenderType>	prdt,
		CPhysicsInfo*		pphi,
		CAIInfo*			paii
	)
		: prdtRenderInfo(prdt), pphiPhysicsInfo(pphi ? pphi : &phiDEFAULT), paiiAIInfo(paii)
	{
		if (prdt && prdt->rpbmCast())
		{
			setUnique[eitRENDERER] = true;
			b_is_unique = true;
		}

		setUnique[eitTHIS] = b_is_unique;

		
		// HACK HACK HACK
		// Temporary fix for memory leaking AI and Physics Infos.
		if (pphi)
			setUnique[eitPHYSICS]	= pphi->setFlags[epfHACK_UNIQUE];

		if (paii)
			setUnique[eitAI]		= paii->setFlags[eaifUNIQUE];

		MEMLOG_ADD_COUNTER(emlTotalCInfo,1);
	}

	//*****************************************************************************************
	CInfo::CInfo(const CInfo& info)
	{
		setUnique       = info.setUnique;
		prdtRenderInfo  = info.prdtRenderInfo;
		pphiPhysicsInfo = info.pphiPhysicsInfo;
		paiiAIInfo      = info.paiiAIInfo;

		MEMLOG_ADD_COUNTER(emlTotalCInfo,1);
	}
	
	//*****************************************************************************************
	CInfo::~CInfo()
	{
		// Ensure we're not destructing a shared CInfo.
		// It's okay to destruct shared infos when you exit the game.
//		Assert(setUnique[eitTHIS]);
		
//		if (setUnique[eitRENDERER])
//			delete prdtRenderInfo;

		if (setUnique[eitPHYSICS])
			delete pphiPhysicsInfo;

		if (setUnique[eitAI])
			delete paiiAIInfo;

		MEMLOG_SUB_COUNTER(emlTotalCInfo,1);
	}


	typedef std::set<CInfo, std::less<CInfo> > TSI;
	// This guy is here instead of in the class header to cut down on compile times.
	TSI tsiInfo;	// A set containing all shared infos, for instancing.
	
	//*****************************************************************************************
	void CInfo::ReleaseShared()
	{
		tsiInfo.erase(tsiInfo.begin(), tsiInfo.end());
	}

	//*****************************************************************************************
	CInfo* CInfo::pinfoFindShared
	(
		rptr<CRenderType>  prdt,
		CPhysicsInfo* pphi,
		CAIInfo*      paii
	)
	{
//		return new CInfo(true, prdt, pphi, paii);

		
		CInfo info(false, prdt, pphi, paii);

		CInfo* pinfo_ret = pinfoFindShared(&info);
		
		// In order to properly delete our temp (info) we need to cheat and set all unique flags to zero
		// except the self unique flag.
		info.setUnique = CSet<EInfoType>();
		info.setUnique[eitTHIS] = true;

		// If we found a duplicate, it will do.
		// If we inserted a new one, the new one will do.
		return (CInfo*)pinfo_ret;
	}


	//*****************************************************************************************
	CInfo* CInfo::pinfoFindShared
	(
		const CInfo* pinfo
	)
	{
		// Cannot find a shared info when the unique flag is set!
		if (pinfo->setUnique[eitTHIS])
		{
			// Can't instance a unique info, so return an allocated copy.
			return pinfo->pinfoCopy();
		}

		// Insert or find, please.
		std::pair<TSI::iterator, bool> p = tsiInfo.insert(*pinfo);

		// If we found a duplicate, it will do.
		// If we inserted a new one, the new one will do.
		const CInfo* pinfo_ret = &(*p.first);
		return (CInfo*)pinfo_ret;
	}

	//*****************************************************************************************
	bool CInfo::operator<
	(
		const CInfo& info
	) const
	{
		return 0 > memcmp(this, &info, sizeof(CInfo));
	}

	//*****************************************************************************************
	CInfo* CInfo::pinfoCopy() const
	{
		CInfo* pinfo = new CInfo(true, prdtRenderInfo, pphiPhysicsInfo, paiiAIInfo);

		return pinfo;
	}

	/*
	//*****************************************************************************************
	CInfo* CInfo::pinfoFindShared
	(
		rptr<CRenderType>  prdt,
		CPhysicsInfo* pphi,
		CAIInfo*      paii
	)
	{


		// Does not yet actually instance anything. Add code to interface with the instancer here.
		CInfo* pinfo = new CInfo(true, prdt, pphi, paii);

		return pinfo;
	}
	*/

	//*****************************************************************************************
	rptr<CRenderType> CInfo::prdtGetRenderInfoWritable()
	{
		Assert(setUnique[eitTHIS]);

		if (!setUnique[eitRENDERER])
		{
			// The info is shared, so we must copy it.
			prdtRenderInfo         = prdtRenderInfo->prdtCopy();
			setUnique[eitRENDERER] = true;
		}

		return prdtRenderInfo;
	}


	//*****************************************************************************************
	void CInfo::SetRenderInfo(rptr<CRenderType> prdt)
	{
		Assert(setUnique[eitTHIS]);

		prdtRenderInfo         = prdt;
		setUnique[eitRENDERER] = true;
	}


	//*****************************************************************************************
	void CInfo::SetPhysicsInfo(CPhysicsInfo* pphi)
	{
		Assert(setUnique[eitTHIS]);

		pphiPhysicsInfo       = pphi ? pphi : &phiDEFAULT;
//		setUnique[eitPHYSICS] = true;
		setUnique[eitPHYSICS] = pphi->setFlags[epfHACK_UNIQUE];
	}

	//*****************************************************************************************
	void CInfo::SetAIInfo(CAIInfo* paii)
	{
		Assert(setUnique[eitTHIS]);

		paiiAIInfo       = paii;
		setUnique[eitAI] = true;
	}

	//*****************************************************************************************
	CPhysicsInfo* CInfo::pphiGetPhysicsInfoWritable()
	{
//		Assert(setUnique[eitTHIS]);

		if (!setUnique[eitPHYSICS])
		{
			// The info is shared, so we must copy it.
			pphiPhysicsInfo       = pphiPhysicsInfo->pphiCopy();
			setUnique[eitPHYSICS] = true;
		}

		return pphiPhysicsInfo;
	}


	//*****************************************************************************************
	CAIInfo* CInfo::paiiGetAIInfoWritable()
	{
		Assert(setUnique[eitTHIS]);

		if (!setUnique[eitAI])
		{
			// The info is shared, so we must copy it.
			paiiAIInfo       = paiiAIInfo->paiiCopy();
			setUnique[eitAI] = true;
		}

		return paiiAIInfo;
	}


//**********************************************************************************************
//
// CInstance implementation.
//

	//
	// To implement pinsCreate(), we must include the header files of all descendents here.
	// This is a major breach of modulatiry, but seems better than the alternatives.  
	//
	// One alternative would be that each descendent module has a different global function
	// such as pinsCreateEntity(), etc.  Then Instance.cpp could declare extern declarations
	// for all those functions, and call those, and not have to include all the header files.
	// But this would involve making every descendent module aware of this scheme,
	// writing new functions, and the dubious practice of explicitly writing extern declarations 
	// rather than including the appropriate header file.
	//

#include "Entity.hpp"
#include "Subsystem.hpp"
#include "Animate.hpp"
#include "Animal.hpp"
#include "Water.hpp"
#include "Lib/GeomDBase/Terrain.hpp"
#include "Lib/GeomDBase/TerrainObj.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Lib/Groff/GroffIO.hpp"

	//******************************************************************************************
	CFastHeap*			CInstance::pfhInstanceNames = 0;
	TInstanceNameMap*	CInstance::pnmNameMap = 0;
#if (VER_TEST)
	bool				CInstance::bDeleted = false;
#else
	bool				CInstance::bDeleted = true;
#endif

	//******************************************************************************************
	CInstance::CInstance()
		: CPartition()
	{
		static CInfo default_info(false,rptr0,0,0);

		// Try to instance the info if possible.
		pinfoProperties = &default_info;

		SetInstanceName("DefaultCon");

		MEMLOG_ADD_COUNTER(emlTotalInstances,1);
	}

	//******************************************************************************************
	CInstance::CInstance(const SInit& initins)
		: CPartition(),  pr3Pres(initins.pr3Presence)
	{
		SetInstanceName( initins.strName.c_str() );
		// Try to instance the info if possible.
		pinfoProperties = CInfo::pinfoFindShared
		(
			initins.prdtRenderInfo,		// Rendering info.
			initins.pphiPhysicsInfo,	// Physics info.
			initins.paiiAIInfo			// AI info.
		);

		#ifdef VER_RESET_POSITIONS
		// Reset to the initial position/orientation of the instance.
		p3Reset = pr3Presence();
		#endif

		MEMLOG_ADD_COUNTER(emlTotalInstances,1);
	}

	//******************************************************************************************
	CInstance::CInstance(CInfo* pinfo)
		: CPartition(), pinfoProperties(pinfo)
	{
		SetInstanceName("InfoCon");
		#ifdef VER_RESET_POSITIONS
		// Reset to the initial position/orientation of the instance.
		p3Reset = pr3Presence();
		#endif

		MEMLOG_ADD_COUNTER(emlTotalInstances,1);
	}


	//******************************************************************************************
	CInstance::CInstance(CPresence3<>  pr3, CInfo* pinfo)
		: CPartition(), pinfoProperties(pinfo), pr3Pres(pr3)
	{
		SetInstanceName("PRSInfoCon");
		#ifdef VER_RESET_POSITIONS
		// Reset to the initial position/orientation of the instance.
		p3Reset = pr3Presence();
		#endif

		MEMLOG_ADD_COUNTER(emlTotalInstances,1);
	}


	//******************************************************************************************
	CInstance::CInstance
	(
	 	CPresence3<>  pr3,
		rptr<CRenderType>	prdt,
		CPhysicsInfo* pphi,
		CAIInfo*      paii
	)
		: CPartition(), pr3Pres(pr3)
	{
		// Try to instance the info if possible.
		pinfoProperties = CInfo::pinfoFindShared(prdt, pphi, paii);

		SetInstanceName("PRS-RPA-Con");
		#ifdef VER_RESET_POSITIONS
		// Reset to the initial position/orientation of the instance.
		p3Reset = pr3Presence();
		#endif

		MEMLOG_ADD_COUNTER(emlTotalInstances,1);
	}

	//******************************************************************************************
	CInstance::CInstance
	(
		rptr<CRenderType> prdt,
		CPhysicsInfo* pphi,
		CAIInfo*      paii
	)
		: CPartition()
	{
		// Try to instance the info if possible.
		pinfoProperties = CInfo::pinfoFindShared(prdt, pphi, paii);

		SetInstanceName("RPA-Con");
		#ifdef VER_RESET_POSITIONS
		// Reset to the initial position/orientation of the instance.
		p3Reset = pr3Presence();
		#endif

		MEMLOG_ADD_COUNTER(emlTotalInstances,1);
	}

	//******************************************************************************************
	CInstance::CInstance
	(
		const CGroffObjectName*	pgon,		// Object to load.
		CLoadWorld*				pload,		// The loader.
		const CHandle&			h_object,	// handle of object in value table.
		CValueTable*			pvtable,	// Value Table.
		const CInfo*			pinfo
		) : CPartition(), pr3Pres(::pr3Presence(*pgon))
	{
		// Constructs a CInstance directly from the GROFF info.
 
		if (pinfo)
		{
			// If we are provided with a CInfo, use it (or an instance of it)
			pinfoProperties = CInfo::pinfoFindShared(pinfo);
		}
		else
		{
			const CEasyString* pestr_mesh	= 0;
			const CEasyString* pestr_physics = 0;	

			bool b_frozen = false;
			int i_priority = 5;

			SETUP_TEXT_PROCESSING(pvtable, pload)
			SETUP_OBJECT_HANDLE(h_object)
			{	
				bFILL_pEASYSTRING(pestr_mesh, esMesh);
				bFILL_pEASYSTRING(pestr_physics, esPhysics);

				bool b_cast_shadow = true;
				bFILL_BOOL(b_cast_shadow, esShadow);
				SetFlagShadow(b_cast_shadow);

				//
				// Culling behaviour for the object.
				//
				{
					float f_cull;
					if (bFILL_FLOAT(f_cull, esCulling))
					{
						FixCullDistance(f_cull);
					}
				}
				{
					float f_shadow_cull;
					if (bFILL_FLOAT(f_shadow_cull, esShadowCulling))
					{
						FixCullDistanceShadow(f_shadow_cull);
					}
				}
				{
					if (bFILL_INT(i_priority, esPriority))
					{
						SetRenderPriority(i_priority);
					}
				}

				//
				// Rendering behaviour for the object.
				//
				{
					bool b_always_face;
					if (bFILL_BOOL(b_always_face, esAlwaysFace))
					{
						SetFlagAlwaysFace(b_always_face);
					}
				}

				//
				// Caching behaviour for the object.
				//
				{
					bool b_never_cache;
					if (bFILL_BOOL(b_never_cache, esNoCache))
					{
						SetFlagNoCache(b_never_cache);
					}
				}
				{
					bool b_never_cache_alone;
					if (bFILL_BOOL(b_never_cache_alone, esNoCacheAlone))
					{
						SetFlagNoCacheAlone(b_never_cache_alone);
					}
				}
				{
					float f_cache_multiplier;
					if (bFILL_FLOAT(f_cache_multiplier, esCacheMultiplier))
					{
						SetCacheMultiplier(f_cache_multiplier);
					}
				}

				// Get frozen flag for use after CInfo has been built.
				bFILL_BOOL(b_frozen, esFrozen);

				// Hardware behaviour for the object.
				{
					bool b;
					if (bFILL_BOOL(b, esD3DHardwareOnly))
					{
						SetFlagHardwareOnly(b);
					}
				}
				{
					bool b;
					if (bFILL_BOOL(b, esD3DSoftwareOnly))
					{
						SetFlagSoftwareOnly(b);
					}
				}
				{
					bool b;
					if (bFILL_BOOL(b, esD3DNoLowRes))
					{
						SetFlagNoLowResolution(b);
					}
				}

				//// Perhaps this ought to be in the mesh loader, 
				//// but since it's stored on a per-instance basis and 
				//// I don't want to modify CMesh, here it is.
				// Get the cache intersecting flag.
				bool b = false;
				bFILL_BOOL(b, esCacheIntersecting);
				SetFlagCacheIntersecting(b);

			}
			END_OBJECT;
			END_TEXT_PROCESSING;


			//
			// Now begin actual initialization.
			//


			// Renderinfo.

			rptr<CRenderType>	prdt = rptr0;
			extern CProfileStat psMesh;
			CCycleTimer ctmr;
			// Do we have a hack mesh instance?
			if (pestr_mesh)
			{
				// Yes!  Load the hack.
				CGroffObjectName* pgon_mesh = pload->goiInfo.pgonFindObject(pestr_mesh->strData());
#if VER_TEST
				if (!pgon_mesh)
				{
					char str_buffer[256];
					sprintf(str_buffer, 
							"%s\n\nMissing hack Mesh instance:\n%s needs %s", 
							__FILE__, 
							pgon->strObjectName,
							pestr_mesh->strData());

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
				}
#endif
				prdt = CRenderType::prdtFindShared(pgon_mesh, pload, pgon_mesh->hAttributeHandle, pvtable);
			}
			else
			{
				// No! Load normally.
				prdt = CRenderType::prdtFindShared(pgon, pload, h_object, pvtable);
			}
			psMesh.Add(ctmr());

			const CPhysicsInfo* pphi = 0;
			// Do we have a hack physics instance?
			if (pestr_physics)
			{
				// Yes!  Load the hack.
				CGroffObjectName* pgon_physics = pload->goiInfo.pgonFindObject(pestr_physics->strData());
#if VER_TEST
				if (!pgon_physics)
				{
					char str_buffer[256];
					sprintf(str_buffer, 
							"%s\n\nMissing hack Physics instance:\n%s needs %s", 
							__FILE__, 
							pgon->strObjectName,
							pestr_mesh->strData());

					if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
						DebuggerBreak();
					
				}
#endif
				pphi = CPhysicsInfo::pphiFindShared(prdt, pgon, pgon->hAttributeHandle, pvtable, pload);
			}
			else
			{
				// No!  Load normally.
				pphi = CPhysicsInfo::pphiFindShared(prdt, pgon, h_object, pvtable, pload);
			}

			const CAIInfo* paii = CAIInfo::paiiFindShared(pgon, h_object, pvtable, pload);
			
			pinfoProperties = CInfo::pinfoFindShared(prdt, (CPhysicsInfo*)pphi, (CAIInfo*)paii);

			if (b_frozen)
				// Can freeze object, now that CInfo is valid.
				NMagnetSystem::SetFrozen(this, true);

			#if (VER_TEST)
			{
				// Warning for bump-mapped immovable objects.

				// Get the physics info.
				const CPhysicsInfo* pphi = pphiGetPhysicsInfo();

				// Check for immoveable objects.
				if (pphi && !pphi->epfPhysicsFlags()[epfMOVEABLE])
				{
					// If the partition does not contain a mesh, it cannot be in hardware.
					rptr<CMesh> pmsh = pmshGetMesh();
					if (pmsh)
					{
						// Check for bump-mapping.
						bool b_bump_mapped = false;
						for (uint u = 0; u < pmsh->pasfSurfaces.uLen; ++u)
						{
							if (pmsh->pasfSurfaces[u].ptexTexture && pmsh->pasfSurfaces[u].ptexTexture->seterfFeatures[erfBUMP])
							{
								b_bump_mapped = true;
								break;
							}
						}

						if (b_bump_mapped)
						{
							dout << "Warning: bump-mapped immovable object (" << pgon->strObjectName << ")\n";
						}
					}
				}

				// Warning for tangible objects that will be priority culled.
				if (pphi && pphi->epfPhysicsFlags()[epfTANGIBLE] && i_priority < 4)
				{
					dout << "Warning: tangible object will be priority culled (" << pgon->strObjectName << ")\n";
				}
			}	
			#endif
		}
		
		// Name me.
		SetInstanceName(pgon->strObjectName);

		#ifdef VER_RESET_POSITIONS
		// Reset to the initial position/orientation of the instance.
		p3Reset = pr3Presence();
		#endif

		MEMLOG_ADD_COUNTER(emlTotalInstances,1);
	}

	//******************************************************************************************
	CInstance* CInstance::pinsCopy() const
	{
		// Must return an identical copy!

		// Make sure that the info is not unique.  Not sure what to do when it is unique.
		AlwaysAssert(!pinfoProperties->setUnique[CInfo::eitTHIS]);

		CInstance* pins = new CInstance();

		*pins = *this;

		// And instance pickup magnets or similar add-ons.
		pins->CopyExternalData(this);

		return pins;
	}

	//*****************************************************************************************
	void CInstance::CopyExternalData
	(
		const CInstance* pins
	)
	{
		// Copy any pickup magnets.
		const CMagnetPair *pmp;
		if (pmp = NMagnetSystem::pmpFindMagnet(pins, Set(emfHAND_HOLD)))
		{
			// Copy it.
			NMagnetSystem::AddMagnetPair(this, 0, pmp->pmagData, 
				pmp->pr3RelMaster(), pmp->r3HingeRel());
		}

		// Copy any hold magnets.
		if (pmp = NMagnetSystem::pmpFindMagnet(pins, Set(emfSHOULDER_HOLD)))
		{
			// Copy it.
			NMagnetSystem::AddMagnetPair(this, 0, pmp->pmagData, 
				pmp->pr3RelMaster(), pmp->r3HingeRel());
		}

		// Copy any "magnet to world" magnets.
		if (pmp = NMagnetSystem::pmpFindMagnet(pins, Set(emfHINGE)))
		{
			// Copy it.
			NMagnetSystem::AddMagnetPair(this, 0, pmp->pmagData, 
				pmp->pr3RelMaster(), pmp->r3HingeRel());
		}

		if (pmp = NMagnetSystem::pmpFindMagnet(pins, Set(emfSLIDE)))
		{
			// Copy it.
			NMagnetSystem::AddMagnetPair(this, 0, pmp->pmagData, 
				pmp->pr3RelMaster(), pmp->r3HingeRel());
		}

		if (pmp = NMagnetSystem::pmpFindMagnet(pins, Set(emfFREEZE)))
		{
			// Copy it.
			NMagnetSystem::AddMagnetPair(this, 0, pmp->pmagData, 
				pmp->pr3RelMaster(), pmp->r3HingeRel());
		}
	}



	//******************************************************************************************
	CInstance::~CInstance()
	{
		// If the CInfo data member is unique (i.e. not shared by anyone else) we must delete it at this point.
		if (pinfoProperties->setUnique[CInfo::eitTHIS])
			delete pinfoProperties;

		MEMLOG_SUB_COUNTER(emlTotalInstances,1);
	}
	
	//*****************************************************************************************
	void* CInstance::operator new(uint  u_size_type)
	{
#ifdef LOG_MEM

		// if we are logging memory allocate 8 bytes more that required, the first DWORD  is
		// a check value so we can verify that a freed block was allocated by this operator.
		// The second DWORD is the size of the block so we can log memory usage.
	#ifdef __MWERKS__
		// "operator new []" and "operator new" are different functions
		// this skips right to the underlying function
		void* pv = fhInstance.pvAllocate(u_size_type+8);
	#else
		void* pv = new (fhInstance) uint8[u_size_type+8];
	#endif
		*((uint32*)pv)	= MEM_LOG_INSTANCE_CHECKWORD;
		*(((uint32*)pv)+1) = u_size_type;

		MEMLOG_ADD_COUNTER(emlInstance, u_size_type);

		// return 8 bytes after the allocated address to the caller.
		return (void*) (((uint8*)pv)+8);
#else
	#ifdef __MWERKS__
		// "operator new []" and "operator new" are different functions
		// this skips right to the underlying function
		return fhInstance.pvAllocate(u_size_type);
	#else
		return new (fhInstance) uint8[u_size_type];
	#endif
#endif
	}

	//*****************************************************************************************
	void CInstance::operator delete(void *pv_mem)
	{
#ifdef LOG_MEM
		uint32*	pu4 = (uint32*) (((uint8*)pv_mem)-8);

		if (pu4[0] == MEM_LOG_INSTANCE_CHECKWORD)
		{
			MEMLOG_SUB_COUNTER(emlInstance, pu4[1]);
		}
		else
		{
			dprintf("CInstance delete: Adress %x not allocated by CInstance new\n",pv_mem);
		}
#endif
	}

	//*****************************************************************************************
	void CInstance::ResetHeap()
	{
		dprintf("Memory used for instances: %ldKb\n",fhInstance.uNumBytesUsed() / 1024);
		fhInstance.Reset(0,0);		// reset the instance fast heap

		MEMLOG_SET_COUNTER(emlInstance, 0);
	}

	//******************************************************************************************
	CInstance* CInstance::pinsCreate(CGroffObjectName* pgon,
									 CLoadWorld*	pload,
									 const CHandle& h,
									 CValueTable* pvt,
									 const CInfo* pinfo)
	{

		if (h == hNULL_HANDLE)
		{
			extern uint g_u_Version_Number;

			if (g_u_Version_Number > 11)
				return new CInstance(pgon, pload, h, pvt, pinfo);
			else
				return pinsINVALID;
		}

		// Snag the class string.  Default to CInstance.
		char const*	str_class = "CInstance";

		SETUP_TEXT_PROCESSING(pvt, pload)
		SETUP_OBJECT_HANDLE(h)
		{
			const CEasyString* pestr = 0;
			
			if (bFILL_pEASYSTRING(pestr, esClass))
			{
				str_class = pestr->strData();
			}

			// Select the correct constructor.

			//
			// Just doing a bunch of if tests seems simpler than writing separate functions
			// and sticking them in a table.
			//

			// Make a macro so changing this is easier....
	#define CASE(foo) if (!strcmpi(str_class, foo))


			CASE("CInstance")
				return new CInstance(pgon, pload, h, pvt, pinfo);

			//
			// Check for backdrops.
			CASE("CBackdrop")
			{
				return new CBackdrop(pgon, pload, h, pvt, pinfo);
			}

			//
			// Triggers all have to check for a valid expression before they ar created...
			//

			CASE("CLocationTrigger")
			{
				if (CLocationTrigger::bValidateTriggerProperties(pgon, h, pvt, pload))
				{
					int i_bv_type = 0;
					bFILL_INT(i_bv_type, esTBoundVol);

					if (i_bv_type == 0)
						pinfo = pinfoSphericalTrigger;

					return new CLocationTrigger(pgon, pload, h, pvt, pinfo);
				}
				else
				{
					pgon->bDoneLoading = false;
					AlwaysAssert(pload->iLoadPass < 5);
					return 0;
				}
			}

			CASE("CCollisionTrigger")
			{
				if (CCollisionTrigger::bValidateTriggerProperties(pgon, h, pvt, pload))
				{
					int i_bv_type = 0;
					bFILL_INT(i_bv_type, esTBoundVol);

					if (i_bv_type == 0)
						pinfo = pinfoSphericalTrigger;

					return new CCollisionTrigger(pgon, pload, h, pvt, pinfo);
				}
				else
				{
					pgon->bDoneLoading = false;
					AlwaysAssert(pload->iLoadPass < 5);
					return 0;
				}
			}

			CASE("CStartTrigger")
			{
				if (CStartTrigger::bValidateTriggerProperties(pgon, h, pvt, pload))
				{
					int i_bv_type = 0;
					bFILL_INT(i_bv_type, esTBoundVol);

					if (i_bv_type == 0)
						pinfo = pinfoSphericalTrigger;

					return new CStartTrigger(pgon, pload, h, pvt, pinfo);
				}
				else
				{
					pgon->bDoneLoading = false;
					AlwaysAssert(pload->iLoadPass < 5);
					return 0;
				}
			}

			CASE("CCreatureTrigger")
			{
				if (CCreatureTrigger::bValidateTriggerProperties(pgon, h, pvt, pload))
				{
					int i_bv_type = 0;
					bFILL_INT(i_bv_type, esTBoundVol);

					if (i_bv_type == 0)
						pinfo = pinfoSphericalTrigger;

					return new CCreatureTrigger(pgon, pload, h, pvt, pinfo);
				}
				else
				{
					pgon->bDoneLoading = false;
					AlwaysAssert(pload->iLoadPass < 5);
					return 0;
				}
			}


			CASE("CSequenceTrigger")
			{
				if (CSequenceTrigger::bValidateTriggerProperties(pgon, h, pvt, pload))
				{
					int i_bv_type = 0;
					bFILL_INT(i_bv_type, esTBoundVol);

					if (i_bv_type == 0)
						pinfo = pinfoSphericalTrigger;

					return new CSequenceTrigger(pgon, pload, h, pvt, pinfo);
				}
				else
				{
					pgon->bDoneLoading = false;
					AlwaysAssert(pload->iLoadPass < 5);
					return 0;
				}
			}


			CASE("CBooleanTrigger")
			{
				if (CBooleanTrigger::bValidateTriggerProperties(pgon, h, pvt, pload))
				{
					int i_bv_type = 0;
					bFILL_INT(i_bv_type, esTBoundVol);

					if (i_bv_type == 0)
						pinfo = pinfoSphericalTrigger;

					return new CBooleanTrigger(pgon, pload, h, pvt, pinfo);
				}
				else
				{
					pgon->bDoneLoading = false;
					AlwaysAssert(pload->iLoadPass < 5);
					return 0;
				}
			}

			CASE("CTimerTrigger")
			{
				if (CTimerTrigger::bValidateTriggerProperties(pgon, h, pvt, pload))
				{
					int i_bv_type = 0;
					bFILL_INT(i_bv_type, esTBoundVol);

					if (i_bv_type == 0)
						pinfo = pinfoSphericalTrigger;

					return new CTimerTrigger(pgon, pload, h, pvt, pinfo);
				}
				else
				{
					pgon->bDoneLoading = false;
					AlwaysAssert(pload->iLoadPass < 5);
					return 0;
				}
			}

			CASE("CVariableTrigger")
			{
				if (CVariableTrigger::bValidateTriggerProperties(pgon, h, pvt, pload))
				{
					int i_bv_type = 0;
					bFILL_INT(i_bv_type, esTBoundVol);

					if (i_bv_type == 0)
						pinfo = pinfoSphericalTrigger;

					return new CVariableTrigger(pgon, pload, h, pvt, pinfo);
				}
				else
				{
					pgon->bDoneLoading = false;
					AlwaysAssert(pload->iLoadPass < 5);
					return 0;
				}
			}

			CASE("CObjectTrigger")
			{
				if (CObjectTrigger::bValidateTriggerProperties(pgon, h, pvt, pload))
				{
					int i_bv_type = 0;
					bFILL_INT(i_bv_type, esTBoundVol);

					if (i_bv_type == 0)
						pinfo = pinfoSphericalTrigger;

					return new CObjectTrigger(pgon, pload, h, pvt, pinfo);
				}
				else
				{
					pgon->bDoneLoading = false;
					AlwaysAssert(pload->iLoadPass < 5);
					return 0;
				}
			}

			CASE("CMoreMassTrigger")
			{
				if (CMoreMassTrigger::bValidateTriggerProperties(pgon, h, pvt, pload))
				{
					int i_bv_type = 0;
					bFILL_INT(i_bv_type, esTBoundVol);

					if (i_bv_type == 0)
						pinfo = pinfoSphericalTrigger;

					return new CMoreMassTrigger(pgon, pload, h, pvt, pinfo);
				}
				else
				{
					pgon->bDoneLoading = false;
					AlwaysAssert(pload->iLoadPass < 5);
					return 0;
				}
			}
/*
			CASE("CMagnetTrigger")
			{
				return new CMagnetTrigger(pgon, pload, h, pvt, pinfo);
			}
*/

			CASE("Disturbance")
				return new CWaterDisturbance(pgon, pload, h, pvt, pinfo);
			CASE("CWaterDisturbance")
				return new CWaterDisturbance(pgon, pload, h, pvt, pinfo);

			CASE("CMagnet")
			{
				// This is going to take some hacking.
				return CMagnet::pinsLoadMasterMagnet(pgon,
													 pload,
													 h,
													 pvt);
			}

			CASE("MasterMagnet")
			{
				// This is going to take some hacking.
				return CMagnet::pinsLoadMasterMagnet(pgon,
													 pload,
													 h,
													 pvt);
			}
			
			CASE("SlaveMagnet")
			{
				// This is going to take some hacking.
				return CMagnet::pinsLoadSlaveMagnet(pgon,
													 pload,
													 h,
													 pvt);
			}

			CASE("CAnimal")
				return new CAnimal(pgon, pload, h, pvt, pinfo);
			
			CASE("CGun")
				if (CGun::bValidateGunProperties(h, pvt, pload))
				{
					return new CGun(pgon, pload, h, pvt, pinfo);
				}
				else
				{
					pgon->bDoneLoading = false;
					return 0;
				}

			CASE("CMuzzleFlash")
			{
				//
				// This returns zero because muzzle flash meshes are not added
				// to the world until they are used. They are kept in an array
				// in the gun code.
				//
				CInstance* pins = new CMuzzleFlash(pgon, pload, h, pvt, pinfo);
				return 0;
			}

			CASE("CHitSpang")
			{
				//  DO NOT LOAD HIT SPANGS-  THEY HAVE BEEN REPLACED WITH PARTICLES

				//
				// Like the muzzle flash above this this returns zero because
				// hit spang meshes are not addedto the world until they are used.
				//
				//CInstance* pins = new CHitSpang(pgon, pload, h, pvt, pinfo);
				return 0;
			}

			CASE("CEntityWater")
				return new CEntityWater(pgon, pload, h, pvt, pinfo);

			CASE("CTerrainObj")
				return new CTerrainObj(pgon, pload, h, pvt, pinfo);

			CASE("CSocket")
				return new CSocket(pgon, pload, h, pvt, pinfo);

			CASE("IGNORE")
				return 0;
			
			CASE("CSubsystem")
			{
				// Why would this ever happen???
				Assert(false);
				return new CSubsystem();
			}

			CASE("CEntityAttached")
			{
				// Wait for all instances to load, then go for the entity attached.
				if (pload->iLoadPass < 2)
				{
					pgon->bDoneLoading = false;
					return 0;
				}
				else
					return new CEntityAttached(pgon, pload, h, pvt, pinfo);
			}

			CASE("CSky")
			{
				//
				// The sky is loaded a little different from othe objects.
				// When an object call the TheSky is loaded, the mesh is loaded and the first
				// texture is extracted and the mesh/instance deleted.
				// The texture map is then used to create the sky.
				//
				CSkyRender::CreateSky(pgon, pload, h, pvt, pinfo);
				return 0;
			}

			CASE("CLightDirectional")
			{
				// Get rid of the global light, if it still exists.
				if (petltLightDirectionalDefault)
				{
					wWorld.Remove(petltLightDirectionalDefault);
					petltLightDirectionalDefault->Kill();
					petltLightDirectionalDefault = 0;
					pinsLightDirectionalDefaultShape->Kill();
					pinsLightDirectionalDefaultShape = 0;
				}

				CInstance *pins_shape = new CInstance(pgon, pload, h, pvt, pinfo);

				wWorld.Add(pins_shape);

				TReal r_intensity = pgon->fScale;
				bFILL_FLOAT(r_intensity, esIntensity);

				if (r_intensity > .99)
					r_intensity = .99;

				rptr<CLightDirectional> pltd_light = rptr_new CLightDirectional(r_intensity);

				// Create an entity to contain it, with the shape as its controlling parent.
				return new CEntityLight(rptr_cast(CLight, pltd_light), pins_shape);
			}

			CASE("CTest")
			{
				int i = 0;
				char c_test = 0;
				int i_test = -1;
				float f_test = -1;
				bool b_test = false;
				const CEasyString* pestr_test = 0;

				dout << "Test object attribs:";

				CObjectValue* pov = povalCast(&(*pvt)[h]);

				
				if (bFillBool(&b_test, pvt->hSymbol("b"), pov, pvt, &i))
					dout << "\nb = " << (b_test ? "true" : "false");

				if (bFillChar(&c_test, pvt->hSymbol("c"), pov, pvt, &i))
					dout << "\nc = \'" << c_test << "\'";
				
				if (bFillInt(&i_test, pvt->hSymbol("i"), pov, pvt, &i))
					dout << "\ni = " << i_test;

				if (bFillFloat(&f_test, pvt->hSymbol("f"), pov, pvt, &i))
					dout << "\nf = " << f_test;

				if (bFillString(&pestr_test, pvt->hSymbol("s"), pov, pvt, &i))
					dout << "\ns = " << pestr_test->strData();
			
				dout << "\n";
				return 0;
			}

			CASE("Settings")
			{
				// Call the settings processing function.
				ProcessSettings(pgon, pload, h, pvt, pinfo);

				// Do not create anything.
				return 0;
			}


			CASE("Player")
			{
				if (pload->iLoadPass < 1)
				{
					pgon->bDoneLoading = false;
					return 0;
				}

				// Stick this shape onto the pre-made player, discarding any previous shape (cause of rptr).
				ptr<CPlayer> pplay = CWDbQueryActivePlayer().pplayGet();
				Assert((const CPlayer*)pplay != NULL);

				pplay->Update(pgon, pload, h, pvt, pinfo);

				// Must get camera to update in case it's attached to the player.
				Assert(wWorld.pcamActive);
				wWorld.pcamActive->UpdateFromAttached();

				return 0;
			}

			CASE("Player Settings")
			{
				// Stick this shape onto the pre-made player, discarding any previous shape (cause of rptr).
				ptr<CPlayer> pplay = CWDbQueryActivePlayer().pplayGet();
				Assert((const CPlayer*)pplay);

				pplay->UpdatePlayerSettings(pgon, pload, h, pvt, pinfo);
				return 0;
			}


			CASE("AI Command")
			{
				gaiSystem.ProcessAICommand(pgon, pload, h, pvt, pinfo);
				return 0;
			}

			CASE("TerrainPlacement")
			{
#if VER_TEST
				if (pload->iLoadPass == 0)
				{
										// Sample value to fill, with defaults.
					const CEasyString* pestr_terrain_file = 0;
				
					SETUP_TEXT_PROCESSING(pvt, pload)
					SETUP_OBJECT_HANDLE(h)
					{
						bFILL_pEASYSTRING(pestr_terrain_file, esFile);
					}
					END_OBJECT;
					END_TEXT_PROCESSING;

					AlwaysAssert(pestr);

					std::string str_terrain_name = pestr_terrain_file->strData();

					// Attempt to open a matching file with a .wtd extension.
					std::string str_transformed_filename = str_terrain_name + ".wtd";

					std::ifstream stream_transformed(str_transformed_filename.c_str(), std::ios::in | std::ios::_Nocreate | std::ios::binary);

					if (stream_transformed.fail())
					{
						// Failed to find wtd file!
				#if VER_TEST
						char str_buffer[1024];
						sprintf(str_buffer, "%s\n\nMissing .wtd file: %s", __FILE__, str_transformed_filename.c_str());

						if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
							DebuggerBreak();
				#endif
					}
				}
#endif


				// Don't load the terrain placement until after all CTerrainObjs.
				if (pload->iLoadPass < 3)
				{
					pgon->bDoneLoading = false;
					return 0;
				}

				// Resets the old CTerrain data to match the current GROFF file.
				rptr<CRenderType>	prdt = rptr0;

				// Get the render info, needed by the terrain constructor.
				{
					SInit initins(prdt, 0, 0, pgon->strObjectName);

					// Sample value to fill, with defaults.
					const CEasyString* pestr_terrain_file = 0;
				
					SETUP_TEXT_PROCESSING(pvt, pload)
					SETUP_OBJECT_HANDLE(h)
					{
						if (bFILL_pEASYSTRING(pestr_terrain_file, esFile))
						{
							// Fake a TrnObj name.
							initins.strName = "_";
							initins.strName += pestr_terrain_file->strData();
						}

						// We should have a mesh maker function call here, which can use the value table.
						prdt = CRenderType::prdtFindShared(pgon, pload, h, pvt);

						bool b_has_mips = true;
						if (!bFILL_BOOL(b_has_mips, esMipMap))
							TerminalError(ERROR_ASSERTFAIL, true, "Base terrain texture's mipmap flag is not specified");

						rptr<CMesh> pmsh = ptCastRenderType<CMesh>(prdt);
						AlwaysAssert(pmsh);
						b_has_mips = b_has_mips || bMeshHasMips(pmsh);

						if (b_has_mips)
							TerminalError(ERROR_ASSERTFAIL, true, "Base terrain texture has mipmaps");

						// If we have mipmaps here (because of bad data) make sure we disable the base texture. It will
						// be managed by the virtual loader otherwise and then we cannot guarantee that the texture will
						// actually be in memory when we access it in the terrain texture system.
						if (!b_has_mips)
							initins.prdtRenderInfo = prdt;
					}
					END_OBJECT;
					END_TEXT_PROCESSING;

					return new CTerrain(initins);
				}
			}


			CASE("Marker")
			{
#if VER_TEST
				// We should only have one master marker.
				AlwaysAssert(!pinsMasterMarker);

				// Make the master marker.
				pinsMasterMarker = new CInstance(pgon, pload, h, pvt, pinfo);

				strcpy(strMarkerName, pgon->strObjectName);

				// Scan back and end the name right after the dash.
				for (int i = strlen(strMarkerName); i >= 0; --i)
				{
					if (strMarkerName[i] == '-')
					{
						strMarkerName[i+1] = '\0';
						break;
					}
				}

				return pinsMasterMarker;
#else
				// Should never load a marker in shipping build.
				//AlwaysAssert(false);
#endif

			}

			CASE("Teleport")
			{
				// Add this guy to the teleport object list.
				return new CTeleport(pgon, pload, h, pvt, pinfo);
			}

			CASE("CParticles")
			{
				// Create a new particle effect.
				CCreateParticles crt(pload, h, pvt);
				CCreateParticles* pcrt = NParticleMap::pcrtAddParticle(crt);

				// Process the list of materials that this object should be used for....
				const CEasyString* pestr_material = NULL;
				for ( uint u_count = 0; bFILL_pEASYSTRING(pestr_material, (ESymbol)((uint32)esA00+u_count)); u_count++ )
				{
					// HACK to avoid overriding our carefully constructed terrain dust effects.
					if (CBeginsWith(pestr_material->strData()) == "Terrain")
						continue;
					TSoundMaterial smat = matHashIdentifier(pestr_material->strData());
					if (smat == matHashIdentifier("Default"))
						smat = 0;
					NParticleMap::AddMapping(pcrt, smat);
				}
				return 0;
			}
		}
		END_OBJECT;
		END_TEXT_PROCESSING;


#if VER_TEST
		{
			char str_buffer[512];
			sprintf(str_buffer, 
					"%s\n\nClass (%s) not found.  Cannot create %s\n", 
					__FILE__, 
					str_class,
					pgon->strObjectName);
			if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
				DebuggerBreak();
		}
#endif

		// Could not match the class.
		return pinsINVALID;
	}

	//*****************************************************************************************
	void CInstance::Kill
	(
	)
	{
		char acBuffer[256];

		// If it is ideal, remove it from the ideal list.
		if (bGetPlatonicIdealName(strGetInstanceName(), acBuffer))
		{
			uint32 u4_hash = u4Hash(acBuffer,0,true);
			RemoveFromIdealList(u4_hash);
		}

		// Add this to the trash list.
		gmlGameLoop.AddToTrash(this);
	}

	//*****************************************************************************************
	void CInstance::SetRenderInfo(rptr<CRenderType> prdt)
	{
		Assert(pinfoProperties);
		ForceUniqueInfo();
		pinfoProperties->SetRenderInfo(prdt);
	}

	//*****************************************************************************************
	void CInstance::SetPhysicsInfo(CPhysicsInfo* pphi)
	{
		Assert(pinfoProperties);
		ForceUniqueInfo();
		pinfoProperties->SetPhysicsInfo(pphi);
	}

	//*****************************************************************************************
	void CInstance::SetAIInfo(CAIInfo* paii)
	{
		Assert(pinfoProperties);
		ForceUniqueInfo();
		pinfoProperties->SetAIInfo(paii);
	}

	//*****************************************************************************************
	void CInstance::PhysicsActivate()
	{
		pphiGetPhysicsInfo()->Activate(this);
	}

	//*****************************************************************************************
	void CInstance::PhysicsDeactivate()
	{
		pphiGetPhysicsInfo()->Deactivate(this);
	}

	//*****************************************************************************************
	bool CInstance::bIsMoving() const
	{
		// If magneted, check master object.
		CInstance* pins = NMagnetSystem::pinsFindMaster(const_cast<CInstance*>(this));

		// We ask physics whether the object is currently active and moving.
		const CPhysicsInfo* pphi = pins->pphiGetPhysicsInfo();
		return pphi->bIsMoving(pins);
	}

	//*****************************************************************************************
	bool CInstance::bCanHaveChildren()
	{
		// Only immovable instances can have partition children.
		// Complicates move logic otherwise.
		return !pphiGetPhysicsInfo()->bIsMovable();
	}

	//*****************************************************************************************
	const CBoundVol* CInstance::pbvBoundingVol() const
	{
		// Return a pointer to either the rendering or physics bounding volume.
		rptr_const<CRenderType> prdt = prdtGetRenderInfo();
		if (prdt)
			return &prdt->bvGet();

		Assert(pphiGetPhysicsInfo()->pbvGetBoundVol());
		return pphiGetPhysicsInfo()->pbvGetBoundVol();
	}

	//*****************************************************************************************
	const char* CInstance::strPartType() const
	{
		return "Instance";
	}

	//*****************************************************************************************
	void CInstance::Move(const CPlacement3<>& p3_new, CEntity* pet_sender)
	{
		CCycleTimer	ctmr;

		// Remember the current placement.
		CPlacement3<> p3_old = pr3Pres;

		// Perform the partition's movement.
		CPartition::Move(p3_new, pet_sender);
		psMove.Add(ctmr(), 1);

		// Dispatch a move message.
		CMessageMove(CMessageMove::etMOVED, this, p3_old, pet_sender).Dispatch();
		psMoveMsg.Add(ctmr(), 1);
	}

	//*****************************************************************************************
	// Override from CPartition
	rptr_const<CRenderType> CInstance::prdtGetRenderType() const
	{
		// Get the RenderType, if any, from the instance.
		return prdtGetRenderInfo();
	}

	//*****************************************************************************************
	// Override from CPartition
	rptr<CShape> CInstance::pshGetShape() const
	{
		// Get the shape, if any, from the instance.
		return ptCastRenderType<CShape>(prdtGetRenderInfo());
	}

	//*****************************************************************************************
	// Override from CPartition
	// If instance in the partition is moving then return true, otherwise call the base class
	// to check the partition children (if any)
	bool CInstance::bContainsMovingObject() const
	{
		if (bIsMoving())
			return true;

		//
		// This partition is not moving, check its children.
		//
		return CPartition::bContainsMovingObject();
	}

	//*****************************************************************************************
	bool CInstance::bPreload(const CBoundVol* pbv, const CPresence3<>* ppr3, bool b_is_contained)
	{
		return CPartition::bPreload(pbv, ppr3, b_is_contained);
	}

	//*****************************************************************************************
	uint32 CInstance::u4GetUniqueHandle() const
	{
		return (uint32)uHandle;
	}
	
	//*****************************************************************************************
	void CInstance::SetUniqueHandle(uint u_handle)
	{
		if (u_handle)
			uHandle = u_handle;
	}

	//*****************************************************************************************
	CVector3<> CInstance::v3Pos() const
	{
		return pr3Pres.v3Pos;
	}
	
	//*****************************************************************************************
	void CInstance::SetPos(const CVector3<>& v3_pos)
	{
		CPlacement3<> p3_new = pr3Pres;

		p3_new.v3Pos = v3_pos;

		Move(p3_new);
	}

	//*****************************************************************************************
	CRotate3<> CInstance::r3Rot() const
	{
		return pr3Pres.r3Rot;
	}
	
	//*****************************************************************************************
	void CInstance::SetRot(const CRotate3<>& r3_rot)
	{
		pr3Pres.r3Rot = r3_rot;
	}

	//*****************************************************************************************
	float CInstance::fGetScale() const
	{
		return pr3Pres.rScale;
	}

	//*****************************************************************************************
	void CInstance::SetScale(float f_new_scale)
	{
		pr3Pres.rScale = f_new_scale;
	}

	//*****************************************************************************************
	CPresence3<> CInstance::pr3Presence() const
	{
		return pr3Pres;
	}

	//*****************************************************************************************
	void CInstance::SetPresence(const CPresence3<> &pr3)
	{
		pr3Pres = pr3;
	}

	//*****************************************************************************************
	CPlacement3<> CInstance::p3Placement() const
	{
		return pr3Pres;
	}

	//*****************************************************************************************
	void CInstance::SetPlacement(const CPlacement3<>& p3)
	{
		static_cast<CPlacement3<>&>(pr3Pres) = p3;
	}

	//*****************************************************************************************
	rptr<CMesh> CInstance::pmshGetMesh() const
	{
		rptr<CMesh> pmsh = ptCastRenderType<CMesh>(prdtGetRenderInfo());

		// Discount "invisible" meshes.
		if (pmsh && pmsh->iNumPolygons() > 0)
			return pmsh;
		return rptr0;
	}

	//*****************************************************************************************
	void CInstance::OnPrefetch(bool b_in_thread) const
	{
#ifdef USING_FETCH
		if (prdtGetRenderInfo())
			prdtGetRenderInfo()->OnPrefetch(b_in_thread);
#endif
	}
	
	//*****************************************************************************************
	void CInstance::OnFetch()
	{
#ifdef USING_FETCH
		// Should also fetch the mesh, physics, and AI infos.
		pddDataDaemon->Fetch(&*prdtGetRenderInfo());
		pddDataDaemon->Fetch(pphiGetPhysicsInfo());
		pddDataDaemon->Fetch(paiiGetAIInfo());

		// Strictly speaking, we ought to set up the CInfo with the return values here,
		// but that is not yet required.
#endif
	}

	//*****************************************************************************************
	void CInstance::OnUnfetch()
	{
		// Do nothing for now.
	}

	//*****************************************************************************************
	char *CInstance::pcSave
	(
		char * pc_buffer
	) const
	{
		// Are we performing a brief save?
		if (CSaveFile::bBrief)
		{
			const CPhysicsInfo* pphi = pphiGetPhysicsInfo();
			// Do we have physics data?
			if (!pphi)
			{
				// No! Don't bother saving.
				return pc_buffer;
			}
			else
			{
				// Yes!

				// Are we immovable?
				if (!pphi->bIsMovable())
				{
					// Yes.  No saving.
					return pc_buffer;
				}

			}
		}

		// Save pr3Pres.
		pc_buffer = pr3Pres.pcSave(pc_buffer);

		// Save the partition data!
		pc_buffer = CPartition::pcSave(pc_buffer);

		return pc_buffer;
	}

	//*****************************************************************************************
	const char *CInstance::pcLoad
	(
		const char * pc_buffer
	)
	{
		// Are we performing a brief load?
		if (CSaveFile::bBrief)
		{
			const CPhysicsInfo* pphi = pphiGetPhysicsInfo();
			// Do we have physics data?
			if (!pphi)
			{
				// No! Don't bother loading.
				return pc_buffer;
			}
			else
			{
				// Yes!

				// Are we immovable?
				if (!pphi->bIsMovable())
				{
					// Yes!  No loading.
					return pc_buffer;
				}

			}
		}

		// Load pr3Pres.
		pc_buffer = pr3Pres.pcLoad(pc_buffer);
		if (!_finite(pr3Pres.v3Pos.tX) ||
			!_finite(pr3Pres.v3Pos.tY) ||
			!_finite(pr3Pres.v3Pos.tZ))
		{
			// The object has been loaded in with very bad data.  It probably was saved after a physics explosion.
			static CRandom rnd(1111);
#define OUT_THERE 100000.0f
			// Stick it out in the boonies.
			pr3Pres.v3Pos = CVector3<>(OUT_THERE, OUT_THERE, OUT_THERE);

			// Perturb it so it isn't intersecting other guys out in the boonies.
			pr3Pres.v3Pos.tX += rnd(-1000.0f,1000.0f);

#if VER_TEST
			// Verify.
			char str_buffer[256];
			sprintf(str_buffer, 
					"%s\n\nFound object at infinity in save file: %s\n", 
					__FILE__, 
					strGetInstanceName());

			if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
				DebuggerBreak();
#endif

		}

		// Load the partition data!
		pc_buffer = CPartition::pcLoad(pc_buffer);

		return pc_buffer;
	}

#if VER_TEST
	//*****************************************************************************************
	int CInstance::iGetDescription
	(
		char *	pc_buffer,
		int		i_buffer_len
	)
	{
		*pc_buffer = 0;

		if (pphiGetPhysicsInfo() && pphiGetPhysicsInfo()->bIsTangible())
		{
			strcpy(pc_buffer, "Tangible Instance!\n");

			sprintf(pc_buffer + strlen(pc_buffer), "  Armour: %f\tDamage: %f\n",
				pphiGetPhysicsInfo()->fArmour, pphiGetPhysicsInfo()->fDamage);
			sprintf(pc_buffer + strlen(pc_buffer), "  Floats: %d\n",
				pphiGetPhysicsInfo()->bFloats());

			// Get attachment list.
			std::list<CMagnetPair*> lspmp;
			NMagnetSystem::GetAttachedMagnets(this, &lspmp);

			// List any magnets.
			for (std::list<CMagnetPair*>::iterator itpmp = lspmp.begin(); itpmp != lspmp.end(); ++itpmp)
			{
				CMagnetPair& pmp = *(*itpmp);

				// Is this the master?
				if (this == pmp.pinsMaster)
				{
					if (pmp.pinsSlave)
						// Yes!  List slave.
						sprintf(pc_buffer + strlen(pc_buffer), "  Magnet: Master of %s\n", 
							pmp.pinsSlave ? pmp.pinsSlave->strGetInstanceName() : "???");
					else
					{
						// No, but there is a magnet to the world.
						Assert(pmp.pmagData);
						sprintf(pc_buffer + strlen(pc_buffer), "  Magnet: %s %s %s\n",
							pmp.pmagData->setFlags[emfHAND_HOLD] ? "Hand" : 
							pmp.pmagData->setFlags[emfSHOULDER_HOLD] ? "Shoulder" : "World",
							pmp.pmagData->setFlags[emfSLIDE] ? "Slide" : "",
							pmp.pmagData->setFlags[emfHINGE] ? "Hinge" : "");
					}
				}
				else if (this == pmp.pinsSlave)
				{
					// No!  this is the slave.
					// Is there a master?
					if (pmp.pinsMaster)
						// Yes!  List the master.
						sprintf(pc_buffer + strlen(pc_buffer), "  Magnet: Slave to %s\n", pmp.pinsMaster->strGetInstanceName());
				}
			}
		}
		else
		{
			strcpy(pc_buffer, "Intangible Instance!\n");
		}

		//
		//  AI info
		//

		const CAIInfo* paii = paiiGetAIInfo();
		if (!paii)
		{
			strcat(pc_buffer, "No AI Info.\n");
		}
		else
		{
			int i = strlen(pc_buffer);

			paii->iGetDescription(pc_buffer + i, i_buffer_len - i);

		}
			

		int i_len = strlen(pc_buffer);
		Assert(i_buffer_len > i_len);
		return i_len;
	}
#endif

	//*****************************************************************************************
	bool CInstance::bUse
	(
		bool b_repeat
	)
	{
		// Most Instances do nothing.
		return false;
	}

	//*****************************************************************************************
	void CInstance::PickedUp
	(
	)
	{
		// Most Instances do nothing.
	}

	//*****************************************************************************************
	// Returns a hex string that identifies the instance
	const char* CInstance::strGetUniqueName(char* str) const
	//**********************************
	{
		sprintf(str,"%X",uHandle);
		return str;
	}


	//*****************************************************************************************
	const char* CInstance::strGetInstanceName
	(
	) const
	//**********************************
	{
		if (this == 0)
		{
			return "NULL";
		}
		else
			return strGetInstanceName(uHandle);
	}

	
	//*****************************************************************************************
	const char* CInstance::strGetInstanceName
	(
		uint32 u4_handle
	)
	//**********************************
	{
		static char str_buf[12];
 		if ((bDeleted) || (pfhInstanceNames == 0))
		{
			Assert(pnmNameMap == 0);
			sprintf(str_buf,"%X",u4_handle);
			return str_buf;
		}

		Assert(u4_handle != 0);

		TInstanceNameMap::iterator i;

		i = pnmNameMap->find(u4_handle);
		if (i == pnmNameMap->end())
		{
			return NULL;
		}

		return (*i).second;
	}


	//*****************************************************************************************
	void CInstance::SetInstanceName
	(
		const char* str
	)
	//**********************************
	{
		// set the unique ID
		uHandle = u4Hash(str);

		// If the bdeleted flag is set we have either deleted the instance name or
		// started without them
		if (bDeleted)
			return;

		if (pfhInstanceNames == 0)
		{
			// We have not deleted the names so we must be starting, so allocate.
			Assert(pnmNameMap==0);
			// no names have been allocated yet. So create the fast heap and name map.
			pfhInstanceNames = new CFastHeap(u4INSTANCE_NAMES_VIRTUALSIZE);
			pnmNameMap = new TInstanceNameMap;
		}

		//
		// Look for this ID in the map
		//
		TInstanceNameMap::iterator i;

		i = pnmNameMap->find((uint32)uHandle);
		if (i != pnmNameMap->end())
		{
			// We have found the string so do not add it again
			return;
		}

		MEMLOG_ADD_COUNTER(emlInstanceNames, strlen(str)+1);
		char* str_block = (char*)pfhInstanceNames->pvAllocate(strlen(str)+1);

		strcpy(str_block, str);
		(*pnmNameMap)[uHandle] = str_block;
	}

	//*****************************************************************************************
	void CInstance::DeleteInstanceNames
	(
	)
	//**********************************
	{
		ResetNameHeap();
		bDeleted = true;
	}

	//*****************************************************************************************
	void CInstance::ResetNameHeap()
	{
		if (pfhInstanceNames)
		{
			delete pfhInstanceNames;
		}

		if (pnmNameMap)
		{
			delete pnmNameMap;
		}

		pfhInstanceNames = 0;
		pnmNameMap = 0;
		MEMLOG_SET_COUNTER(emlInstanceNames, 0);
	}


	//*****************************************************************************************
	char* CInstance::strInstanceNameFromHandle
	(
		uint32 u4_handle
	)
	//**********************************
	{
#if VER_TEST
		if (pnmNameMap && pfhInstanceNames)
		{
			// Both the map and string heap are valid

			//
			// Look for this ID in the map
			//
			TInstanceNameMap::iterator i;

			i = pnmNameMap->find((uint32)u4_handle);
			if (i != pnmNameMap->end())
			{
				return (*i).second;
			}
		}
#endif

		return NULL;
	}


	//*****************************************************************************************
	float CInstance::fDistanceFromGlobalCameraSqr() const
	{
		float f_x = SPartitionSettings::v3GlobalCameraPosition.tX - pr3Pres.v3Pos.tX;
		float f_y = SPartitionSettings::v3GlobalCameraPosition.tY - pr3Pres.v3Pos.tY;
		return f_x * f_x + f_y * f_y;
	}
	
	//*****************************************************************************************
	void CInstance::SetAlwaysFaceBV()
	{
		// Do nothing if the object does not always face the camera.
		if (pdGetData().bAlwaysFace == 0)
			return;

		// Get a pointer to a valid mesh.
		rptr<CMesh> pmsh = ptCastRenderType<CMesh>(prdtGetRenderInfo());
		if (!pmsh)
			return;
		if (pmsh->iNumPolygons() <= 0)
			return;

		//
		// If the always face flag is already set, the work has been done to the mesh. If it
		// has not be set, the mesh's bounding volume must expanded to include the volume
		// covered by a full rotation of the mesh.
		//
		if (!pmsh->bAlwaysFaceMesh)
		{
			CVector3<> v3_corner = pmsh->bvbVolume.v3GetMax();
			float f_diagonal_2d = float(sqrt(v3_corner.tX * v3_corner.tX + v3_corner.tY * v3_corner.tY));
			f_diagonal_2d *= 1.001f;
			v3_corner.tX = f_diagonal_2d;
			v3_corner.tY = f_diagonal_2d;
			pmsh->bvbVolume = CBoundVolBox(v3_corner);
			pmsh->bAlwaysFaceMesh = true;
		}

		//
		// The instance cannot have rotational information.
		//
		CPresence3<> pr3;
		pr3.v3Pos  = pr3Presence().v3Pos;
		pr3.rScale = pr3Presence().rScale;
		SetPresence(pr3);
	}

	//*****************************************************************************************
	int CInstance::iGetTeam() const
	{
		return 0;
	}

	//*****************************************************************************************
	const CAnimate* CInstance::paniGetOwner() const
	{
		return 0;
	}


//*********************************************************************************************
//
// CBackdrop implementation.
//

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CBackdrop::CBackdrop(const CGroffObjectName* pgon, CLoadWorld* pload,
		                 const CHandle& h_object, CValueTable* pvtable, const CInfo* pinfo)
		: CInstance(pgon, pload, h_object, pvtable, pinfo)
	{
	}



// Add this pragma to make sure fhInstance is initialised before use.
#pragma warning(disable:4073)
#pragma init_seg(lib)

// Virtual memory for allocating CInstance objects.
CFastHeap fhInstance(1 << 24);
