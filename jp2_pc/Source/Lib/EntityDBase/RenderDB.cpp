/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of RenderDB.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/RenderDB.cpp                                          $
 * 
 * 86    10/02/98 2:59a Pkeet
 * Added different schedule times for Direct3D.
 * 
 * 85    10/01/98 4:05p Mlange
 * Improved move message stat reporting.
 * 
 * 84    10/01/98 1:56a Pkeet
 * Moved call to disable parent caches from the world move function to the render database move
 * message.
 * 
 * 83    10/01/98 12:24a Pkeet
 * Moved the cache lru to the cache module.
 * 
 * 82    9/28/98 10:07p Pkeet
 * Added automatic allocations for the VM loader.
 * 
 * 81    9/25/98 7:23p Rwyatt
 * RenderDBase sets no VM paging on scene and groff file loads
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "RenderDB.hpp"
#include "Query/QRenderer.hpp"
#include "Query/QTerrain.hpp"
#include "MessageTypes/MsgPaint.hpp"
#include "MessageTypes/MsgMove.hpp"
#include "MessageTypes/MsgSystem.hpp"
#include "Lib/Renderer/PipeLine.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/RenderCacheInterface.hpp"
#include "Lib/Renderer/RenderCache.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/Sys/FixedHeap.hpp"
#include "Lib/Renderer/Occlude.hpp"
#include "Lib/Sys/VirtualMem.hpp"
#include "Lib/Loader/TextureManager.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "Lib/Renderer/DepthSort.hpp"
#include "Lib/Renderer/Fog.hpp"
#include "Lib/Renderer/Sky.hpp"
#include "Lib/Sys/LRU.hpp"
#include "Lib/View/AGPTextureMemManager.hpp"
#include "Lib/EntityDBase/QualitySettings.hpp"
#include "Lib/View/Direct3DRenderState.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"
#include "Lib/Sys/W95/Render.hpp"

// The following mess is for the perspective settings.
#include "Lib/Types/FixedP.hpp"
#include "Lib/Renderer/Primitives/DrawTriangle.hpp"
#include "Lib/Renderer/Primitives/Walk.hpp"
#include "Lib/Renderer/Primitives/IndexT.hpp"
#include "Lib/Renderer/Primitives/IndexPerspectiveT.hpp"

#include <memory.h>

//
// Module specific variables.
//

// Switch for turning terrain off.
bool bRenderTerrain = true;

// Stats for renderering.
static CProfileStat psOcclusionGetList("Get list", &proProfile.psOcclusion);

// Render save/load version number.
const int iRenderVersion = 3;

static rptr<CLightAmbient>	pamb;
//
// Class implementations.
//

//**********************************************************************************************
//
// CRenderDB implementation.
//
	//*****************************************************************************************
	CRenderDB::CRenderDB(const CWorld& w)
		: wDBase(w)
	{
		SetInstanceName("Render DBase");

		// No defaults yet.
		pc_defaults = 0;
		u4NoPageFrames = 0;

		// Register this entity with the message types it needs to receive.
		  CMessageMove::RegisterRecipient(this);
		 CMessagePaint::RegisterRecipient(this);
		CMessageSystem::RegisterRecipient(this);
	}

	//*****************************************************************************************
	CRenderDB::~CRenderDB()
	{
		CMessageSystem::UnregisterRecipient(this);
		 CMessagePaint::UnregisterRecipient(this);
		  CMessageMove::UnregisterRecipient(this);

		delete pc_defaults;
	}

	//******************************************************************************************
	void CRenderDB::Process(const CMessagePaint& msgpaint)
	{
		// Make sure Direct3D is enabled.
		d3dDriver.Restore();

		// Report stats if enabled.
		//d3dDriver.ReportPerFrameStats();

		// Prepare AGP texturing if active.
		agpAGPTextureMemManager.BeginFrame();

		// Process moved objects.
		EndCacheUpdate();

		// Increment the LRU count of image caches, allowing them to use the LRU container.
		renclRenderCacheList.IncrementCount();

		// Set up values for use by the partitioning system.
		SPartitionSettings::SetGlobalPartitionData();

		// Set up values for use by the render cache system.
		CRenderCache::SetParameters();
				
		TOccludeList oclist;		// List of occluding polygons.
		CPresence3<> pr3_inv_cam;	// Inverse presence of the camera.

		// Update the frame key for render caching.
		++CRenderCache::iFrameKeyGlobal;

		// Get the current active camera.
		CWDbQueryActiveCamera wqcam(wDBase);

		// Perform camera prediction.
		wqcam.tGet()->UpdatePrediction();

		//
		// Render the backdrop.
		//
		{
			// Create a camera with a much farther clipping plane.
			CCamera::SProperties camprop = wqcam.tGet()->campropGetProperties();
			camprop.rFarClipPlaneDist = 20000.0f;
			CCamera cam_backdrop(wqcam.tGet()->pr3VPresence(), camprop);

			// Get pointer to the settings.
			ptr<CRenderer::SSettings>     prenset   = msgpaint.renContext.pSettings;

			// Store the render settings.
			CRenderer::SSettings     renset   = *prenset;

			//
			// Set the rendering properties to turn off shading, texturing, and fogging.
			// We still perform lighting, using a full ambient light, so that textures are
			// maximally lit.
			//
			prenset->seterfState   = Set(erfLIGHT);

			// Do not execute the scheduler for the backdrop.
			prenset->bExecuteScheduler = false;

			if (!pamb)
				pamb =  rptr_new CLightAmbient(1.0f);

			// Construct a special ambient light for the backdrop, for full lighting.
			//CInstance insFullAmbient(rptr_cast(CRenderType, rptr_new CLightAmbient(1.0f)));
			CInstance insFullAmbient(rptr_cast(CRenderType, pamb));

			// Start the 3D renderer.
			if (d3dDriver.bUseD3D())
			{
				srd3dRenderer.SetOutputFlag(true);
				srd3dRenderer.bBeginScene();
				d3dstState.SetAllowZBuffer(false);
			}
			else
			{
				srd3dRenderer.SetOutputFlag(false);
			}

			// Lock and render.
			wDBase.Lock();
			CLArray(COcclude*, papoc, 0);	// Use a dummy occlusion list.
			msgpaint.renContext.RenderScene
			(
				cam_backdrop,					// Special backdrop camera.
				std::list<CInstance*>(1, &insFullAmbient),	// Special backdrop light list.
				wDBase.ppartBackdropsList(),	// Special backdrop partition.
				papoc,							// Use no occlusion objects.
				esfINTERSECT,					// Assume intersection.
				0								// Us no terrain.
			);
			wDBase.Unlock();

			// Reenable Z buffering.
			if (d3dDriver.bUseD3D())
			{
				d3dstState.SetAllowZBuffer(true);
			}

			// Reset the render settings.
			*prenset = renset;

			// Unlock the main buffer.
			prasMainScreen->Unlock();
		}


		// If the terrain exists, update it.
		if (CWDbQueryTerrain().tGet() != 0)
			CWDbQueryTerrain().tGet()->FrameBegin(wqcam.tGet());

		// Get the terrain's mesh.
		CWDbQueryTerrainMesh wqtmsh(wDBase);

		// Start the timer.
		CCycleTimer	ctmr;

		// Build an occlusion list.
		if (COcclude::bBuildOcclusionList())
		{
			// This is a good time to reset the heap used for occlusion data.
			COcclude::Reset();

			// Get the inverse presence of the camera.
			pr3_inv_cam = ~wqcam.tGet()->pr3Presence();

			// Get a list from the world database.
			GetOccludePolygons
			(
				wDBase.ppartPartitionList(),	// Root partition node.
				pr3_inv_cam,					// Inverse camera presence.
				wqcam.tGet()->pbvBoundingVol(),	// Camera bounding volume.
				oclist							// Linked list of visible occluding objects.
			);
		}
		
		// Create an array of occluding polygon objects.
		CLArray(COcclude*, papoc, oclist.size());

		// Copy the list to the array.
		CopyOccludePolygons(papoc, *wqcam.tGet(), oclist);

		// Set stats for occlusion.
		TCycles cy_get_list = ctmr();
		psOcclusionGetList.Add(cy_get_list, oclist.size());
		proProfile.psOcclusion.Add(cy_get_list, 1);

		// Render the scene.
		wDBase.Lock();

		{
			CCamera::SProperties camprop = wqcam.tGet()->campropGetProperties();
			CCamera cam(wqcam.tGet()->pr3VPresence(), camprop);

			// Toggle the clear off.
			CScreenRender::SSettings screnset = *msgpaint.renContext.pScreenRender->pSettings;
			msgpaint.renContext.pScreenRender->pSettings->bClearBackground = false;
			msgpaint.renContext.pScreenRender->pSettings->bDrawSky         = false;

			// Get the lights whose influence intersects the camera's bounding volume.
			CWDbQueryLights wqlt(&cam, wDBase);

			// Render the main scene.
			msgpaint.renContext.RenderScene
			(
				cam,
				wqlt,
				wDBase.ppartPartitionList(),
				papoc,
				esfINTERSECT,
				(bRenderTerrain) ? (wqtmsh.tGet()) : (0)
			);

			// Reset the clear.
			*msgpaint.renContext.pScreenRender->pSettings = screnset;
		}
	
		wDBase.Unlock();

		// Finish hardware renderering.
		srd3dRenderer.EndScene();

		// Execute terrain updates scheduled for this frame.
		msgpaint.renContext.ExecuteScheduleForTerrain();

		if (CWDbQueryTerrain().tGet() != 0)
			CWDbQueryTerrain().tGet()->FrameEnd();

		// Clean up the fixed heap used for render caches and terrain textures.
		fxhHeap.bConglomerate();

		// Update the count for the next frame.
		BeginCacheUpdate();

		if (u4NoPageFrames>0)
		{
			u4NoPageFrames--;
			if (u4NoPageFrames == 0)
			{
				// Let the texture manager VM system dynamically page textures
				gtxmTexMan.pvmeTextures->SetAlwaysLoad(false);
			}
		}

		// Clean up AGP texturing if active.
		agpAGPTextureMemManager.EndFrame();

		// Decommit the pipeline heap if we are not running.
		if (!CMessageSystem::bSimulationGoing())
			CPipelineHeap::Decommit();
	}

	extern CProfileStat psMoveMsgRenderDB;
	
	//******************************************************************************************
	void CRenderDB::Process(const CMessageMove& msgmv)
	{
		CTimeBlock tmb(&psMoveMsgRenderDB);

		// Help render caching by tracking all currently woken objects.
		if (msgmv.pinsMover->pshGetShape())
		{
			// A visible object.
			switch (msgmv.etType)
			{
				case CMessageMove::etAWOKE:
					// Invalidate parent caches.
					PurgeParentCaches(msgmv.pinsMover);
					break;

				case CMessageMove::etMOVED :
				case CMessageMove::etACTIVE :
					AddToMovingList(msgmv.pinsMover);
					break;

				case CMessageMove::etSLEPT :
					AddToStoppedList(msgmv.pinsMover);
					break;
			}
		}
		else if (msgmv.etType == CMessageMove::etMOVED && ptCast<CCamera>(msgmv.pinsMover))
		{
			// Detect significant camera movement, and inform render cache.
			//if (msgmv.bSignificant())
				//CRenderCache::bCameraMoved = true;
		}
	}

	//******************************************************************************************
	void CRenderDB::Process(const CMessageSystem& msg_system)
	{
		if (msg_system.escCode == escQUALITY_CHANGE)
		{
			// Perspective settings (all hacked up).
			extern float fPerspectivePixelError;
			extern float fAltPerspectivePixelError;
			extern CPerspectiveSettings persetSettings;

			// Update the far clipping distance.
			ptr<CCamera> pcam = CWDbQueryActiveCamera(wWorld).tGet();
			if (pcam)
			{
				CCamera::SProperties camprop = pcam->campropGetProperties();

				// Update actual far clip distance based on desired & quality.
				camprop.SetFarClipFromDesired();

				pcam->SetProperties(camprop);

				// Adjust the fog for the changed far clipping distance.
				fogFog.SetQualityAdjustment(camprop.rFarClipPlaneDist, camprop.rDesiredFarClipPlaneDist);
				fogTerrainFog.SetQualityAdjustment(camprop.rFarClipPlaneDist, camprop.rDesiredFarClipPlaneDist);
			}

			// Set the object culling priority.
			SetPrioritySetting(iGetQualitySetting());

			//
			// Set discrete quality values.
			//
			int i_quality = iGetQualitySetting();

			// Sky (none).
			if (gpskyRender)
			{
				gpskyRender->SetDrawMode
				(
					(CSkyRender::SkyDrawMode)(qdQualitySettings[i_quality].iSkyDrawMode)
				);
			}

			// Perspective.
			fPerspectivePixelError = qdQualitySettings[i_quality].fPersectiveError;
			persetSettings.iMinSubdivision = qdQualitySettings[i_quality].iMinSubdivison;
			fAltPerspectivePixelError = qdQualitySettings[i_quality].fAltPerspectiveError;
			persetSettings.iAltMinSubdivision = qdQualitySettings[i_quality].iAltMinSubdivision;
			persetSettings.iAdaptiveMinSubdivision = qdQualitySettings[i_quality].iAdaptiveMinSubdivision;

			// Bi-linear filter (off).
			if (prenMain && prenMain->pSettings &&  prenMain->pScreenRender->seterfModify()[erfFILTER])
			{
				prenMain->pSettings->seterfState[erfFILTER] = qdQualitySettings[i_quality].bBilinearFilter;
			}

			// Disable largest mip-maps.
			if (qdQualitySettings[i_quality].bDisableLargestMip)
				CTexture::emuMipUse = emuNO_LARGEST;
			else
				CTexture::emuMipUse = emuNORMAL;

			// Set the maximum amount of physical memory used by the texture VM.
			gtxmTexMan.pvmeTextures->AutoSetMemory();

			// Don't delay paging for the next N frames.
			SetNoPageFrames(2);

			// Default maximum number of polys to sort.
			ptsTolerances.iMaxToDepthsort = qdQualitySettings[i_quality].iMaxToDepthsort;

			// Redraw image caches and the terrain.
			if (CWDbQueryTerrain().tGet())
				CWDbQueryTerrain().tGet()->Rebuild(true);

			// Set the scheduler time slices.
			//shcScheduler.uMSSlice = qdQualitySettings[i_quality].uCacheMs;
			//shcSchedulerTerrainTextures.uMSSlice = qdQualitySettings[i_quality].uTerrainMs;
		}

		//
		// If we load a scene or a groff then set no VM paging for a couple of frames
		//
		if ((msg_system.escCode == escGROFF_LOADED) || (msg_system.escCode == escSCENE_FILE_LOADED))
		{
			SetNoPageFrames(2);
		}
	}

	//*****************************************************************************************
	char *CRenderDB::pcSave
	(
		char* pc
	) const
	{
		// Version number.
		pc = pcSaveT(pc, iRenderVersion);

		// Depthsort Settings.
		pc = ptsTolerances.pcSave(pc);

		// Perspective settings (all hacked up).
		extern float fPerspectivePixelError;
		extern float fMinZPerspective;
		extern float fAltPerspectivePixelError;
		extern CPerspectiveSettings persetSettings;

		pc = pcSaveT(pc, 1);
		pc = pcSaveT(pc, fPerspectivePixelError);
		pc = pcSaveT(pc, fAltPerspectivePixelError);
		pc = pcSaveT(pc, persetSettings.iMinSubdivision);
		pc = pcSaveT(pc, persetSettings.iAltMinSubdivision);
		pc = pcSaveT(pc, fMinZPerspective);

		// Image Cache settings.
		pc = rcsRenderCacheSettings.pcSave(pc);

		// Fog settings.
		CFog::SProperties fogprop = fogFog.fogpropGetProperties();
		pc = pcSaveT(pc, fogprop.rHalfFogY);
		pc = pcSaveT(pc, fogprop.rPower);

		// Culling settings.
		float f_cull_dist = SPartitionSettings::fGetCullMaxAtDistance();
		pc = pcSaveT(pc, f_cull_dist);

		float f_cull_radius = SPartitionSettings::fGetMaxRadius();
		pc = pcSaveT(pc, f_cull_radius);

		float f_cull_dist_shadow = SPartitionSettings::fGetCullMaxAtDistanceShadow();
		pc = pcSaveT(pc, f_cull_dist_shadow);

		float f_cull_radius_shadow = SPartitionSettings::fGetMaxRadiusShadow();
		pc = pcSaveT(pc, f_cull_radius_shadow);

		// Mip-Map settings.
		pc = pcSaveT(pc, CTexture::fMipmapThreshold);

		// Sky settings.
		pc = gpskyRender->pcSave(pc);

		// Camera.
		ptr<CCamera> pcam = CWDbQueryActiveCamera(wWorld).tGet();

		pc = pcSaveT(pc, pcam->campropGetProperties().fZoomFactor);
		pc = pcSaveT(pc, pcam->campropGetProperties().angGetAngleOfView());
		pc = pcSaveT(pc, pcam->campropGetProperties().rNearClipPlaneDist);
		pc = pcSaveT(pc, pcam->campropGetProperties().rDesiredFarClipPlaneDist);

		// Filtering render flag.
		int i_filter = 0;				// Value indicating this is not supported.

		if (prenMain && prenMain->pSettings)
		{
			if (prenMain->pScreenRender->seterfModify()[erfFILTER])
			{
				if (prenMain->pSettings->seterfState[erfFILTER])
					i_filter = 5;	// On.
				else
					i_filter = 4;	// Off.
			}
		}

		pc = pcSaveT(pc, i_filter);

		// Save animating mesh data.
		LPMA::const_iterator i = lpmaAnimatedMeshes.begin();
		for ( ; i != lpmaAnimatedMeshes.end(); ++i)
		{
			pc = (*i)->pcSave(pc);
		}

		return pc;
	}

	//*****************************************************************************************
	const char *CRenderDB::pcLoad
	(
		const char* pc
	)
	{
		int iVersion;

		pc = pcLoadT(pc, &iVersion);

		if (iVersion >= 1)
		{
			// Depthsort Settings.
			pc = ptsTolerances.pcLoad(pc);

			// Perspective settings (all hacked up).
			extern float fPerspectivePixelError;
			extern float fMinZPerspective;
			extern float fAltPerspectivePixelError;
			extern CPerspectiveSettings persetSettings;

			int iPerspSetVersion;
			pc = pcLoadT(pc, &iPerspSetVersion);

			float f_pixel_error, f_alt_pixel_error, f_min_z_perspective;
			int i_min_subdivision, i_alt_min_subdivision;

			pc = pcLoadT(pc, &f_pixel_error);
			pc = pcLoadT(pc, &f_alt_pixel_error);
			pc = pcLoadT(pc, &i_min_subdivision);
			pc = pcLoadT(pc, &i_alt_min_subdivision);
			pc = pcLoadT(pc, &f_min_z_perspective);

			/* 
			 * Ignore these values, they are set via the global quality value.
			 *
			fPerspectivePixelError = f_pixel_error;
			fAltPerspectivePixelError = f_alt_pixel_error;
			persetSettings.iMinSubdivision = i_min_subdivision;
			persetSettings.iAltMinSubdivision = i_alt_min_subdivision;
			fMinZPerspective = f_min_z_perspective;
			*/

			// Image Cache settings.
			pc = rcsRenderCacheSettings.pcLoad(pc);

			// Fog settings.
			CFog::SProperties fogprop = fogFog.fogpropGetProperties();

			pc = pcLoadT(pc, &fogprop.rHalfFogY);
			pc = pcLoadT(pc, &fogprop.rPower);

			fogFog.SetProperties(fogprop);
			fogTerrainFog.SetProperties(fogprop);

			// Culling settings.
			if (iVersion >= 2)
			{
				float f_cull_dist, f_cull_dist_shadow;
				float f_cull_radius, f_cull_radius_shadow;

				pc = pcLoadT(pc, &f_cull_dist);
				SPartitionSettings::SetCullMaxAtDistance(f_cull_dist);

				pc = pcLoadT(pc, &f_cull_radius);
				SPartitionSettings::SetMaxRadius(f_cull_radius);

				pc = pcLoadT(pc, &f_cull_dist_shadow);
				SPartitionSettings::SetCullMaxAtDistanceShadow(f_cull_dist_shadow);

				pc = pcLoadT(pc, &f_cull_radius_shadow);
				SPartitionSettings::SetMaxRadiusShadow(f_cull_radius_shadow);
			}
			else
			{
				float f_cull_dist;
				pc = pcLoadT(pc, &f_cull_dist);
				// Hack for now to ensure compatibility with future scene files.
				if (f_cull_dist < 250.0f)
					f_cull_dist = 250.0f;
				SPartitionSettings::SetCullMaxAtDistance(f_cull_dist);
			}

			// Mip-Map settings.
			pc = pcLoadT(pc, &CTexture::fMipmapThreshold);

			// Sky settings.
			pc = gpskyRender->pcLoad(pc);

			// Camera.
			ptr<CCamera> pcam = CWDbQueryActiveCamera(wWorld).tGet();
			if (pcam)
			{
				CCamera::SProperties camprop = pcam->campropGetProperties();

				pc = pcLoadT(pc, &camprop.fZoomFactor);

				CAngle angTemp;

				pc = pcLoadT(pc, &angTemp);
				camprop.SetAngleOfView(angTemp);

				pc = pcLoadT(pc, &camprop.rNearClipPlaneDist);
				pc = pcLoadT(pc, &camprop.rDesiredFarClipPlaneDist);
				camprop.SetFarClipFromDesired();

				pcam->SetProperties(camprop);

				// Adjust the fog.
				fogFog.SetQualityAdjustment(camprop.rFarClipPlaneDist, camprop.rDesiredFarClipPlaneDist);
				fogTerrainFog.SetQualityAdjustment(camprop.rFarClipPlaneDist, camprop.rDesiredFarClipPlaneDist);
			}

			// Filtering render flag.
			int i_filter;
			pc = pcLoadT(pc, &i_filter);

			/* 
			 * Ignore this value, it is set via the global quality value.
			 *
			// Only set the flag if it is allowed for this processor.
			if (prenMain->pScreenRender->seterfModify()[erfFILTER] && (i_filter >= 4))
				prenMain->pSettings->seterfState[erfFILTER] = (i_filter - 4);
			*/


			if (iVersion >= 3)
			{
				// Load animating mesh data.
				std::list<CMeshAnimating*>::iterator i = lpmaAnimatedMeshes.begin();
				for ( ; i != lpmaAnimatedMeshes.end(); ++i)
				{
					pc = (*i)->pcLoad(pc);
				}
			}
		}
		else
		{
			AlwaysAssert("Unknown version of render settings");
		}

		return pc;
	}

	//*****************************************************************************************
	//
	void CRenderDB::SaveDefaults()
	{
		char ac_buffer[4096];
		char *pc_end;

		pc_end = pcSave(ac_buffer);

		if (pc_defaults)
			delete pc_defaults; 

		int i_len = pc_end - ac_buffer;
		pc_defaults = new char[i_len];

		if (pc_defaults)
			memcpy(pc_defaults, ac_buffer, i_len);
	}

	//*****************************************************************************************
	//
	void CRenderDB::RestoreDefaults()
	{
		if (pc_defaults)
			pcLoad(pc_defaults);
	}

	//*****************************************************************************************
	void CRenderDB::SetNoPageFrames(uint32 u4_frames)
	{
		// Tell the VM to stop paging and load textures as requested.
		gtxmTexMan.pvmeTextures->SetAlwaysLoad(true);
		u4NoPageFrames = u4_frames;
	}


//
// Global variables.
//
uint32 CRenderDB::u4NoPageFrames = 0;
