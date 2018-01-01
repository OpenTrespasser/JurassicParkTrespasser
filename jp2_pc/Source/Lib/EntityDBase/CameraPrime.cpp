/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of CameraPrime.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/CameraPrime.cpp                                       $
 * 
 * 9     10/05/98 11:42p Pkeet
 * Turned scheduler for Direct3D off during the initial camera prime stuff.
 * 
 * 8     10/03/98 4:41a Rwyatt
 * Bump angles tables are now generated before the first frame to prevent spikes during the
 * game.
 * 
 * 7     9/25/98 11:54a Mlange
 * Now listens for scene file loaded message instead of GROFF file loaded.
 * 
 * 6     9/24/98 7:50p Mlange
 * Now updates camera for attached entity before doing a spin.
 * 
 * 5     9/21/98 8:59p Pkeet
 * Backbuffer is cleared to black even in Direct3D mode.
 * 
 * 4     9/21/98 11:41a Mlange
 * Now renders a final view after the camera has been re-attached.
 * 
 * 3     9/16/98 3:49p Mlange
 * Now delays camera priming until next paint message to ensure screen surfaces have been
 * created.
 * 
 **********************************************************************************************/

#include "GblInc/Common.hpp"
#include "CameraPrime.hpp"
#include "set.h"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPaint.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/Renderer/Primitives/FastbumpTable.hpp"
#include "Lib/View/ColourBase.hpp"
#include "Query/QRenderer.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"

namespace
{
	const int iNumCamViews = 6;
}

//**********************************************************************************************
//
// CCameraPrime implementation.
//

	//******************************************************************************************
	CCameraPrime::CCameraPrime()
		: bPrimeOnPaint(false)
	{
		SetInstanceName("CCameraPrime");

		CMessageSystem::RegisterRecipient(this);
		 CMessagePaint::RegisterRecipient(this);
	}

	//******************************************************************************************
	CCameraPrime::~CCameraPrime()
	{
		 CMessagePaint::UnregisterRecipient(this);
		CMessageSystem::UnregisterRecipient(this);
	}

	//******************************************************************************************
	void CCameraPrime::Prime() const
	{
		// Turn D3D texture scheduler off for a while.
		srd3dRenderer.SetScheduler(iNumCamViews + 1);

		CCamera* pcam = pwWorld->pcamGetActiveCamera();

		pcam->UpdateFromAttached();

		// We will rotate the camera which requires it to be free. Record the instance
		// the camera was on so we can restore this later.
		CInstance* pins_camera_on        = pcam->pinsAttached();
		CPlacement3<> p3_camera_relative = pcam->p3Relative();
		bool b_camera_on_head            = pcam->bHead();

		// Free the camera.
		pcam->SetAttached(0);


		// Rotate the camera and render each view.
		CAngle ang_curr = 0;
		CAngle ang_step = d2_PI / iNumCamViews;

		for (int i = 0; i < iNumCamViews; i++)
		{
			CPlacement3<> p3_new(CRotate3<>(CDir3<>(0, 0, 1), ang_curr), pcam->v3Pos());
			pcam->Move(p3_new);

			gmlGameLoop.Paint();

			ang_curr += ang_step;
		}


		// Restore camera attachment.
		pcam->SetAttached(pins_camera_on, b_camera_on_head);
		pcam->SetRelative(p3_camera_relative);

		// Render first frame.
		gmlGameLoop.Paint();

		if (prasMainScreen)
		{
			d3dDriver.SetFlipClear(false);
			prasMainScreen->Clear(0);
			prasMainScreen->Flip();
			prasMainScreen->Clear(0);
			prasMainScreen->Flip();
			prasMainScreen->Clear(0);
			prasMainScreen->Flip();
			d3dDriver.SetFlipClear(true);
		}

		// Turn D3D texture scheduler back on.
		srd3dRenderer.SetScheduler(0);
	}

	//*****************************************************************************************
	void CCameraPrime::BuildInitialBumpTables()
	{
		SBumpLighting	blt_primary;

		//
		// blt_primary is the bump light we should use for all bump tables.
		//

		aptr<CLightList> pltl_lights = new CLightList( CWDbQueryLights::lpinsActiveLights );
		blt_primary = pltl_lights->bltGetPrimaryBumpLighting();

		//
		// Iterate directly through the clut database and set the bump tables of any entries that
		// have not already been set
		//
		for (set<CPalClut, CPalClutLess>::iterator it_palclut_db = pcdbMain.psetPalClut->begin(); it_palclut_db != pcdbMain.psetPalClut->end(); ++it_palclut_db)
		{
			// Does this clut entry have a bump table?
			if ( (*it_palclut_db).pBumpTable )
			{
				// If our light vector is different from the one in thw bump table then regenerate the bump table
				if (Fuzzy((float)(*it_palclut_db).pBumpTable->bltMain.lvAmbient, (*it_palclut_db).pBumpTable->fLightTolerance) != blt_primary.lvAmbient ||
					Fuzzy((float)(*it_palclut_db).pBumpTable->bltMain.lvStrength, (*it_palclut_db).pBumpTable->fLightTolerance) != blt_primary.lvStrength)
				{
					(*it_palclut_db).pBumpTable->bltMain = blt_primary;
					FillBumpTable((*it_palclut_db).pBumpTable->au1BumpToIntensity, blt_primary, *((*it_palclut_db).pBumpTable->pcluCLU));
				}
			}
		}
	}


	//*****************************************************************************************
	void CCameraPrime::Process(const CMessagePaint& ms)
	{
		if (bPrimeOnPaint)
		{
			bPrimeOnPaint = false;

			BuildInitialBumpTables();
			Prime();
		}
	}

	//*****************************************************************************************
	void CCameraPrime::Process(const CMessageSystem& ms)
	{
		if (ms.escCode == escSCENE_FILE_LOADED)
		{
			bPrimeOnPaint = true;
		}
	}

//*****************************************************************************************
CCameraPrime* pCameraPrime;

