/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of GUIApp.h.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/GUIApp.cpp                                                     $
 * 
 * 149   8/22/98 6:47p Pkeet
 * Removed the 'g_initDD' object as there is a global one in 'dd.hpp.'
 * 
 * 148   98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 147   8/11/98 8:32p Mlange
 * Added menu item to revert back to old broadcast messaging system.
 * 
 * 146   8/05/98 5:28p Mlange
 * Added auto frame grab capability.
 * 
 * 145   8/05/98 3:17p Mlange
 * Working on grab continous command.
 * 
 * 144   98/07/25 23:45 Speter
 * Added command for physics stats window.
 * 
 * 143   7/21/98 8:00p Mlange
 * Can now refine the exported terrain triangulation to the limit of the minimum terrain texture
 * node size.
 * 
 * 142   7/13/98 12:03a Pkeet
 * Removed the 'OnStretch' member function.
 * 
 * 141   7/06/98 10:47p Pkeet
 * Disabled using extra partition memory for building optimized partitions in final mode.
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "stdafx.h"
#include "Lib/W95/Direct3D.hpp"
#include <StdIO.h>
#include "Config.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Sys/debugConsole.hpp"
#include "Lib/Renderer/PipeLine.hpp"
#include "Shell/WinRenderTools.hpp"
#include "GUIPipeLine.hpp"
#include "Toolbar.hpp"
#include "GUIApp.h"
#include "GUIAppDlg.h"
#include "GUIPipeLine.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Physics/PhysicsSystem.hpp"
#include "Lib/Renderer/RenderCacheInterface.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "DialogPartition.hpp"
#include "Lib/Renderer/Occlude.hpp"
#include "Lib/Std/Mem.hpp"
#include "Lib/Std/MemLimits.hpp"
#include "Lib/Sys/FixedHeap.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/ProcessorDetect.hpp"
#include "Lib/Audio/Material.hpp"
#include "Lib/Sys/Scheduler.hpp"
#include "lib\w95\dd.hpp"
#include "lib\view\video.hpp"
#include "Lib/Sys/Reg.h"
#include "Lib/Sys/RegInit.hpp"
#include "Lib/View/Grab.hpp"
#include "Lib/EntityDBase/Water.hpp"
#include "Lib/Sys/RegInit.hpp"
#include "Version.hpp"
#include "Lib/Renderer/LightBlend.hpp"


#if TARGET_PROCESSOR == PROCESSOR_PENTIUM
	#pragma message("Target build processor : Intel Pentium")
#elif TARGET_PROCESSOR == PROCESSOR_PENTIUMPRO
	#pragma message("Target build processor : Intel Pentium Pro / Pentium II")
#elif TARGET_PROCESSOR == PROCESSOR_K6_3D
	#pragma message("Target build processor : AMD-K6 3DX/Cedar")
#endif


//
// File-scope variables.
//
CGUIAppDlg* pappdlgMainWindow = 0;


extern CColour clrInitColour;


//
// Local functions.
//

//*********************************************************************************************
//
float fGetFloatFromString
(
	char* str_float	// String representing the floating point value.
);
//
// Returns a float converted from string representation of a floating point value.
//
//**************************************


//
// Class implementation.
//

//*********************************************************************************************
//
// Application message map.
//

BEGIN_MESSAGE_MAP(CGUIAppApp, CWinApp)
	//{{AFX_MSG_MAP(CGUIAppApp)
	ON_COMMAND(MENU_ABOUT, OnAbout)
	ON_COMMAND(MENU_SHOWTOOLBAR, OnShowtoolbar)
	ON_COMMAND(MENU_ADDOBJECT, OnAddobject)
	ON_COMMAND(MENU_DELETE, OnDelete)
	ON_COMMAND(MENU_PROPERTIES, OnProperties)
	ON_COMMAND(MENU_CAMERAPROPERTIES, OnCameraproperties)
	ON_COMMAND(MENU_AMBIENT, OnAmbient)
	ON_COMMAND(MENU_DIRECTIONAL, OnDirectional)
	ON_COMMAND(MENU_POINT, OnPoint)
	ON_COMMAND(MENU_POINTDIRECTIONAL, OnPointdirectional)
	ON_COMMAND(MENU_BACKGROUND, OnBackground)
	ON_COMMAND(MENU_EDIT_FOG, OnEditFog)
	ON_COMMAND(MENU_BUMPMAP, OnBumpmap)
	ON_COMMAND(MENU_DEBUG, OnDebug)
	ON_COMMAND(MENU_PLAY, OnPlay)
	ON_COMMAND(MENU_PAUSE, OnPause)
	ON_COMMAND(MENU_FPS_ESTIMATE, OnFpsEstimate)
	ON_COMMAND(MENU_PRESORT_NONE, OnPresortNone)
	ON_COMMAND(MENU_PRESORT_FTOB, OnPresortFtob)
	ON_COMMAND(MENU_PRESORT_BTF, OnPresortBtf)
	ON_COMMAND(MENU_EDIT_PERSPECTIVE, OnEditPerspective)
	ON_COMMAND(MENU_RENDERCACHE_SETTINGS, OnRendercacheSettings)
	ON_COMMAND(MENU_FASTMODE, OnFastmode)
	ON_COMMAND(MENU_DELAUNAYTEST, OnDelaunayTest)
	ON_COMMAND(MENU_CACHE_INTERSECT, OnCacheIntersect)
	ON_COMMAND(MENU_CONPHYSICS, OnConphysics)
	ON_COMMAND(MENU_CONSHADOWS, OnConshadows)
	ON_COMMAND(MENU_CONTERRAIN, OnConterrain)
	ON_COMMAND(MENU_CONAI, OnConAI)
	ON_COMMAND(MENU_DEPTHSORT, OnDepthSort)
	ON_COMMAND(MENU_STATS_DEPTHSORT, OnStatsDepthSort)
	ON_COMMAND(MENU_TERRAIN_SETTINGS, OnTerrainSettings)
	ON_COMMAND(MENU_WATER_SETTINGS, OnWaterSettings)
	ON_COMMAND(MENU_LOAD_TERRAIN, OnLoadTerrain)
	ON_COMMAND(MENU_EXPORT_TERRAIN_TRI, OnExportTerrainTri)
	ON_COMMAND(MENU_TERRAINTEST, OnTerrainTest)
	ON_COMMAND(MENU_HEIGHTMAP, OnHeightmap)
	ON_COMMAND(MENU_TERRAIN_WIRE, OnTerrainWire)
	ON_COMMAND(MENU_TEXTURE_WIRE, OnTextureWire)
	ON_COMMAND(MENU_DEPTHSORT_SETTINGS, OnDepthSortSettings)
	ON_COMMAND(MENU_MODE1, OnMode1)
	ON_COMMAND(MENU_MODE2, OnMode2)
	ON_COMMAND(MENU_MODE_3, OnMode3)
	ON_COMMAND(MENU_MODE_4, OnMode4)
	ON_COMMAND(MENU_SOUND, OnSoundProperties)
	ON_COMMAND(MENU_VIEW_PARTITIONS, OnViewPartitions)
	ON_COMMAND(MENU_DOUBLEV, OnDoubleV)
	ON_COMMAND(MENU_HALFSCAN, OnHalfscan)
	ON_COMMAND(MENU_PROCESSOR, OnProcessor)
	ON_COMMAND(MENU_CONVEXCACHES, OnConvexcaches)
	ON_COMMAND(MENU_STATIC_HANDLES, OnStaticHandles)
	ON_COMMAND(MENU_PRELOAD, OnPreload)
	ON_COMMAND(MENU_PARTITIONS, OnPartitions)
	ON_COMMAND(MENU_MIPMAP_NORMAL, OnMipmapNormal)
	ON_COMMAND(MENU_MIPMAP_NO_LARGEST, OnMipmapNoLargest)
	ON_COMMAND(MENU_MIPMAP_SMALLEST, OnMipmapSmallest)
	ON_COMMAND(MENU_OCCLUDE_OBJECTS, OnOccludeObjects)
	ON_COMMAND(MENU_OCCLUDE_POLYGONS, OnOccludePolygons)
	ON_COMMAND(MENU_OCCLUSION_CONSOLE, OnOcclusionConsole)
	ON_COMMAND(MENU_OCCLUDE_OCCLUDE, OnOccludeOcclude)
	ON_COMMAND(MENU_OCCLUDE_SETTINGS, OnOccludeSettings)
	ON_COMMAND(MENU_OCCLUDE_CAMERAVIEWTEST, OnOccludeCameraviewtest)
	ON_COMMAND(MENU_GUN_SETTINGS, OnGunSettings)
	ON_COMMAND(MENU_SCHEDULER_SETTINGS, OnSchedulerSettings)
	ON_COMMAND(MENU_SCHEDULER_USE, OnSchedulerUse)
	ON_COMMAND(MENU_VIEW_TRIGGERS, OnViewTriggers)
	ON_COMMAND(MENU_DUMP_FIXEDHEAP, OnDumpFixedheap)
	ON_COMMAND(MENU_FORCEINTERSECTING, OnForceIntersecting)
	ON_COMMAND(MENU_GRAB, OnGrab)
	ON_COMMAND(MENU_GRABCONTINOUS, OnGrabContinous)
	ON_COMMAND(MENU_CULLING, OnCulling)
	ON_COMMAND(MENU_ACCELERATION, OnAcceleration)
	ON_COMMAND(MENU_WATER_ALPHA, OnWaterAlpha)
	ON_COMMAND(MENU_WATER_INTERPOLATE, OnWaterInterpolate)
	ON_COMMAND(MENU_ASSERTSAVE, OnAssertSave)
	ON_COMMAND(MENU_IGNORE_MSG_RECIPIENTS, OnIgnoreMsgRecipients)
	ON_COMMAND(MENU_HARDWARE_STATS, OnHardwareStats)
	ON_COMMAND(MENU_FILTER_CACHES, OnFilterCaches)
	ON_COMMAND(MENU_FREEZECACHES, OnFreezeCaches)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()



//*********************************************************************************************
//
// CGUIAppDlg implementation.
//

//*********************************************************************************************
//
CGUIAppApp::CGUIAppApp()
//
// Default constructor.
//
//**************************************
{
}

//*********************************************************************************************
//
static void __cdecl ExecuteCommand
(
	char  c_command,	// Character indicating command.
	char* str_argument	// Argument string for command.
)
//
// Executes a '/' command from the command line string.
//
//**************************************
{
	Assert(str_argument);

	// For boolean arguments, set to true unless the next character is a minus.
	bool b_flag = *str_argument != '-';

	switch (c_command)
	{
		case 0:
		{
			// Add the given object.
			pappdlgMainWindow->bLoadRequest = true;
			strcpy(pappdlgMainWindow->strLoadFile, str_argument);
//			CLoadWorld lw(str_argument);
			break;
		}



		case 'c':
		case 'C':
			// Convert .trr terrain file request.
			pappdlgMainWindow->bConvertTerrain = true;
			sprintf(pappdlgMainWindow->strConvertTRRFileName, str_argument);
			break;


		case 'f':
		case 'F':
			// Set fast mode request flag.
			pappdlgMainWindow->bFastModeRequest = true;
			break;

		case 'g':
		case 'G':
			// Set game mode request flag.
			pappdlgMainWindow->bGameModeRequest = true;
			break;

		case 'b':
		case 'B':
			// Set background colour to fog.
			clrInitColour = clrDefEndDepth;
			break;

		case 'i':
		case 'I':
			// Enable mesh intersection splitting.
			CMesh::bSplitPolygons = b_flag;
			break;

		case 't':
		case 'T':
			// Set the toolbar flag to false.
			pappdlgMainWindow->bShowToolbar = false;
			break;

		case 'r':
		case 'R':
			// Set game mode request flag.
			pappdlgMainWindow->bReplayRequest = true;
			sprintf(pappdlgMainWindow->acReplayFileName, str_argument);
			break;
		
		case 's':
		case 'S':
			// Set system-memory flag.
			pappdlgMainWindow->bSystemMem = true;
			break;

		case 'v':
		case 'V':
			// Set system-memory flag.
			pappdlgMainWindow->bSystemMem = false;
			break;
		
		case 'w':
		case 'W':
			// Set create optimized mesh wrapper flag.
			CLoadWorld::bCreateWrap = false;
			break;

		case 'x':
		case 'X':
			// Set the x position.
			v3InitCamPos.tX = fGetFloatFromString(str_argument);
			break;

		case 'y':
		case 'Y':
			// Set the y position.
			v3InitCamPos.tY = fGetFloatFromString(str_argument);
			break;

		case 'z':
		case 'Z':
			// Set the z position.
			v3InitCamPos.tZ = fGetFloatFromString(str_argument);
			break;
	}
}

//*********************************************************************************************
//
BOOL CGUIAppApp::InitInstance()
//
// Application-generated InitInstance function.
//
//**************************************
{
	// Standard initialization

    int32      i;

	// Open the registry.
	OpenKey();

	// Initialize DirectDraw.
    i = g_initDD.Initialize();


	CGUIAppDlg dlg;
	pappdlgMainWindow = &dlg;
	m_pMainWnd = &dlg;


    if (i != 0)
    {
        switch (i)
        {
            case -1:
                TerminalError(IDS_ERROR_DDRAW_NOTFOUND, FALSE, "DirectDraw Error");
                break;

            case -2:
            case -3:
            case -4:
                TerminalError(IDS_ERROR_DDRAW_FATAL, FALSE, "DirectDraw Error");
                break;
        }
    }

    Video::EnumerateDisplayModes();

	::ParseCommandLine(m_lpCmdLine, ExecuteCommand);

	// setup a global that contains the system page size.
	SetupSystemPageSize();

	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Delete memory associated with the renderer.
	delete prnshMain;

	// Close the registry.
	CloseKey();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CGUIAppApp::ExitInstance() 
{
	int i_ret =CWinApp::ExitInstance() ;
	return (i_ret == 0 || i_ret == 2) ? (0) : (i_ret);
}


//*********************************************************************************************
//
class CDialogAbout : public CDialog
//
// Class for the "About" dialog.
//
//**************************************
{
public:
	//*****************************************************************************************
	//
	CDialogAbout(UINT nIDTemplate, CWnd* pParentWnd = NULL) : CDialog(nIDTemplate, pParentWnd)
	//
	// Just call our parent constructor.
	//
	//**************************************
	{
	}

	//*****************************************************************************************
	//
	BOOL OnInitDialog()
	//
	// Set the version and date strings.
	//
	//**************************************
	{
		char szBuild[128];

        wsprintf(szBuild, "Version: %s", BUILD_VERSION);
		SetDlgItemText(IDC_STATIC_ABOUT_VERSION, szBuild);

		sprintf(szBuild, "Built: %s %s", __DATE__, __TIME__);
		SetDlgItemText(IDC_STATIC_ABOUT_DATE, szBuild);

		return 1;
	}
};


//*********************************************************************************************
//
void CGUIAppApp::OnAbout() 
//
// Responds to a MENU_ABOUT message by showing the 'About' dialog box.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	CDialogAbout dlgAbout(IDD_ABOUT, NULL);

	dlgAbout.DoModal();
}


//*********************************************************************************************
//
void CGUIAppApp::OnShowtoolbar
(
)
//
// Responds to a MENU_TOOLBAR message by showing the toolbar.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ShowToolbar();
}

//*********************************************************************************************
//
void CGUIAppApp::OnAddobject
(
) 
//
// Responds to a MENU_ADDOJBECT message by adding an object to the scene.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);

	pappdlgMainWindow->AddObject();
}

//*********************************************************************************************
//
void CGUIAppApp::OnDelete
(
) 
//
// Responds to a MENU_DELETE message by deleting a selected object.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);

	if (pipeMain.iDeleteSelected())
	{
		pappdlgMainWindow->Invalidate();
	}
	else
	{
		pappdlgMainWindow->MessageBox("No object selected.", "GUIApp Error",
			                          MB_OK | MB_ICONHAND);
	}
}

//*********************************************************************************************
//
void CGUIAppApp::OnProperties() 
//
// Responds to a MENU_PROPERTIES message by opening a dialox box to edit material or light
// properties.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);

	pappdlgMainWindow->EditObject();
}

//*********************************************************************************************
//
void CGUIAppApp::OnCameraproperties
(
) 
//
// Responds to a MENU_CAMERAPROPERTIES message by opening a dialox box to edit the camera's
// properties.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OpenCameraProperties();
}

//*********************************************************************************************
//
void CGUIAppApp::OnAmbient
(
) 
//
// Responds to a MENU_AMBIENT message by opening a dialox box to edit the ambient light's
// properties.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OpenLightProperties(rptr_cast(CLight, pipeMain.pltaGetAmbientLight()));
}

//*********************************************************************************************
//
void CGUIAppApp::OnDirectional
(
) 
//
// Responds to a MENU_DIRECTIONAL message by adding a directional light.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);

	pappdlgMainWindow->AddDirectionalLight();
}

//*********************************************************************************************
//
void CGUIAppApp::OnPoint
(
) 
//
// Responds to a MENU_POINT message by adding a point light.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);

	pappdlgMainWindow->AddPointLight();
}

//*********************************************************************************************
//
void CGUIAppApp::OnPointdirectional
(
) 
//
// Responds to a MENU_POINTDIRECTIONAL message by adding a point directional light.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);

	pappdlgMainWindow->AddPointDirectionalLight();
}

//*********************************************************************************************
//
void CGUIAppApp::OnBackground
(
) 
//
// Responds to a MENU_BACKGROUND message by invoking the background colour dialog box.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);

	pappdlgMainWindow->EditBackground();
}

//*********************************************************************************************
//
void CGUIAppApp::OnEditFog
(
) 
//
// Responds to a MENU_EDIT_FOG message by invoking the fog dialog box.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OpenFogProperties();
}


//
// Variables.
//

// The application
CGUIAppApp theApp;



//*********************************************************************************************
void CGUIAppApp::OnBumpmap() 
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	Assert(pappdlgMainWindow);
	pappdlgMainWindow->AddTestBumpmap();	
}

//*********************************************************************************************
void CGUIAppApp::OnDebug() 
{
	CMessageSystem msg(escSTOP_SIM);
	msg.Dispatch();
	
	gmlGameLoop.egmGameMode = egmDEBUG;
	Assert(pappdlgMainWindow);
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnPlay() 
{
	gmlGameLoop.egmGameMode = egmPLAY;
	Assert(pappdlgMainWindow);
	pappdlgMainWindow->SetMenuState();

	CMessageSystem(escSTART_SIM).Dispatch();

	// Send close message to children.
	pappdlgMainWindow->SendChildrenMessage(WM_CLOSE);

}

//*********************************************************************************************
void CGUIAppApp::OnPause() 
{
	gmlGameLoop.bPauseGame = !gmlGameLoop.bPauseGame;
	Assert(pappdlgMainWindow);
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnMenuWater() 
{
}

//*********************************************************************************************
void CGUIAppApp::OnFpsEstimate() 
{
	Assert(pappdlgMainWindow);

	// Run test.
	pappdlgMainWindow->FPSEstimate();
}

//*********************************************************************************************
void CGUIAppApp::OnPresortNone() 
{
	prenMain->pSettings->esSortMethod = esNone;
	Assert(pappdlgMainWindow);
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnPresortFtob() 
{
	prenMain->pSettings->esSortMethod = esPresortFrontToBack;
	Assert(pappdlgMainWindow);
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnPresortBtf() 
{
	prenMain->pSettings->esSortMethod = esPresortBackToFront;
	Assert(pappdlgMainWindow);
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnDepthSort() 
{
	prenMain->pSettings->esSortMethod = esDepthSort;
	Assert(pappdlgMainWindow);
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnEditPerspective() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OpenPerspectiveDialog();
}

//*********************************************************************************************
void CGUIAppApp::OnRendercacheSettings() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OpenRenderCacheDialog();	
}


//*********************************************************************************************
void CGUIAppApp::OnFastmode() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleFastMode();	
}


//*********************************************************************************************
void CGUIAppApp::OnDelaunayTest() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleDelaunayTest();	
}

//*********************************************************************************************
void CGUIAppApp::OnCacheIntersect() 
{
	Assert(pappdlgMainWindow);

	rcsRenderCacheSettings.bAddIntersectingObjects = !rcsRenderCacheSettings.bAddIntersectingObjects;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnConphysics() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleConPhysics();
}

//*********************************************************************************************
void CGUIAppApp::OnConshadows() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleConShadows();
}

//*********************************************************************************************
void CGUIAppApp::OnConterrain() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleConTerrain();
}

//*********************************************************************************************
void CGUIAppApp::OnConAI() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleConAI();
}

//*********************************************************************************************
void CGUIAppApp::OnStatsDepthSort() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleConDepthSort();
}

//*********************************************************************************************
void CGUIAppApp::OnTerrainSettings() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OpenTerrainDialog();
}

//*********************************************************************************************
void CGUIAppApp::OnWaterSettings() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OpenWaterDialog();
}

//*********************************************************************************************
void CGUIAppApp::OnLoadTerrain() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->LoadTerrain();
}

//*********************************************************************************************
void CGUIAppApp::OnExportTerrainTri() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ExportTerrainTri(false);
}

//*********************************************************************************************
void CGUIAppApp::OnTerrainTest() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleTerrainTest();
}

//*********************************************************************************************
void CGUIAppApp::OnHeightmap() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleHeightmap();
}

//*********************************************************************************************
void CGUIAppApp::OnTerrainWire() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleTerrainWire();
}

//*********************************************************************************************
void CGUIAppApp::OnTextureWire() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleTextureWire();
}

//*********************************************************************************************
void CGUIAppApp::OnDepthSortSettings() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OpenDepthSortProperties();
}

//*********************************************************************************************
void CGUIAppApp::OnMode1() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->SetWindowSize(1);
}


//*********************************************************************************************
void CGUIAppApp::OnMode2() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->SetWindowSize(2);
}

//*********************************************************************************************
void CGUIAppApp::OnMode3() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->SetWindowSize(3);
}

//*********************************************************************************************
void CGUIAppApp::OnMode4() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->SetWindowSize(4);
}


//*********************************************************************************************
void CGUIAppApp::OnSoundProperties()
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OpenSoundProperties();
}

//*********************************************************************************************
void CGUIAppApp::OnViewPartitions() 
{
	Assert(pappdlgMainWindow);
	
	bDrawPartitions = !bDrawPartitions;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnDoubleV() 
{
	Assert(pappdlgMainWindow);
	
	prenMain->pSettings->bDoubleVertical = !prenMain->pSettings->bDoubleVertical;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnHalfscan() 
{
	Assert(pappdlgMainWindow);
	
	prenMain->pSettings->bHalfScanlines = !prenMain->pSettings->bHalfScanlines;
	pappdlgMainWindow->SetMenuState();	
}

//*********************************************************************************************
void CGUIAppApp::OnConvexcaches() 
{
	Assert(pappdlgMainWindow);
	
	rcsRenderCacheSettings.bUseConvexPolygons = !rcsRenderCacheSettings.bUseConvexPolygons;
	pappdlgMainWindow->SetMenuState();	
}

//*********************************************************************************************
//
void CGUIAppApp::OnProcessor() 
{
	CCPUDetect		detProcessor;

	//
	// set the target processor string from the build variable..
	//

#if TARGET_PROCESSOR == PROCESSOR_PENTIUM
	char buffer[256] = "Target build Processor: Intel Pentium\n";
#elif TARGET_PROCESSOR == PROCESSOR_PENTIUMPRO
	char buffer[256] = "Target build Processor: Intel Pentium Pro / Pentium II\n";
#elif TARGET_PROCESSOR == PROCESSOR_K6_3D
	char buffer[256] = "Target build Processor: AMD-K6 3DX/Cedar\n";
#endif

	// have we managed to detect the type of the local processor??
	if ( detProcessor.bProcessorDetected() )
	{
		char		temp[128];
		wsprintf(temp,"Local Processor: %s\n",detProcessor.strLocalProcessorName());
		strcat(buffer,temp);
		MessageBox(NULL,buffer,"Processor Detect",MB_ICONINFORMATION|MB_OK);
	}
	else
	{
		MessageBox(NULL,"Failed to load Processor.dll","Processor Detect",MB_ICONERROR|MB_OK);
	}
}

//*********************************************************************************************
void CGUIAppApp::OnStaticHandles() 
{
	Assert(pappdlgMainWindow);

	bStaticHandles = !bStaticHandles;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnPreload() 
{
	Assert(pappdlgMainWindow);

	gmlGameLoop.bPreload = !gmlGameLoop.bPreload;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnPartitions() 
{
#if bPARTITION_BUILD
	CDialogPartition dlg;
	dlg.DoModal();
#else
	pappdlgMainWindow->MessageBox("Function not available in Final Mode!", "GUIApp Error",
			                      MB_OK | MB_ICONHAND);
#endif // bPARTITION_BUILD
}

//*********************************************************************************************
void CGUIAppApp::OnMipmapNormal() 
{
	Assert(pappdlgMainWindow);

	CTexture::emuMipUse = emuNORMAL;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnMipmapNoLargest() 
{
	Assert(pappdlgMainWindow);

	CTexture::emuMipUse = emuNO_LARGEST;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnMipmapSmallest() 
{
	Assert(pappdlgMainWindow);

	CTexture::emuMipUse = emuSMALLEST;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnOccludeObjects() 
{
	Assert(pappdlgMainWindow);

	COcclude::bUseObjectOcclusion = !COcclude::bUseObjectOcclusion;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnOccludePolygons() 
{
	Assert(pappdlgMainWindow);

	COcclude::bUsePolygonOcclusion = !COcclude::bUsePolygonOcclusion;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnOcclusionConsole() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->ToggleOcclusionConsole();
}

//*********************************************************************************************
void CGUIAppApp::OnOccludeOcclude() 
{
	Assert(pappdlgMainWindow);

	COcclude::bRemoveOccluded = !COcclude::bRemoveOccluded;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnOccludeSettings() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OcclusionSettings();
}

//*********************************************************************************************
void CGUIAppApp::OnOccludeCameraviewtest() 
{
	Assert(pappdlgMainWindow);

	COcclude::bTestCameraView = !COcclude::bTestCameraView;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnGunSettings() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OpenGunSettings();
}

//*********************************************************************************************
void CGUIAppApp::OnSchedulerSettings() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->OpenSchedulerSettings();
}

//*********************************************************************************************
void CGUIAppApp::OnSchedulerUse() 
{
	Assert(pappdlgMainWindow);

	CScheduler::bUseScheduler = !CScheduler::bUseScheduler;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnViewTriggers() 
{
	Assert(pappdlgMainWindow);

	CRenderContext::bRenderTriggers = !CRenderContext::bRenderTriggers;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnDumpFixedheap() 
{
	fxhHeap.DumpState();
}

//*********************************************************************************************
void CGUIAppApp::OnForceIntersecting() 
{
	Assert(pappdlgMainWindow);
	
	rcsRenderCacheSettings.bForceIntersecting = !rcsRenderCacheSettings.bForceIntersecting;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnGrab() 
{
	grabRaster.SetDimensions(160, 120);
	grabRaster.GrabBackbuffer();
	grabRaster.DumpBitmap();
	grabRaster.ReleaseLastGrab();
}

//*********************************************************************************************
void CGUIAppApp::OnGrabContinous() 
{
	if (!pagAutoGrabber->bIsActive())
	{
		// The maximum number of characters in the file name.
		#define iMAX_GRABNAME_CHARS (512)

		char str_filename[iMAX_GRABNAME_CHARS];

		// Open the file dialog.
		bool b_ok = bGetFilename
		(
			pappdlgMainWindow->m_hWnd,						// Window handle of parent to the dialog.
			str_filename,				// Pointer to string to put filename.
			iMAX_GRABNAME_CHARS,		// Maximum number of characters for the filename.
			"Auto grab",
			"Bitmaps (*.bmp)\0*.bmp\0All (*.*)\0*.*\0"
										// Optional extension filter.
		);

		// Abort if the user hits cancel.
		if (!b_ok)
			return;

		// Redraw the screen.
		pappdlgMainWindow->PaintWindow();

		pagAutoGrabber->Start(str_filename, prasMainScreen->iWidth, prasMainScreen->iHeight);

		pappdlgMainWindow->SetMenuItemState(MENU_GRABCONTINOUS, TRUE);
	}
	else
	{
		pagAutoGrabber->Stop();

		pappdlgMainWindow->SetMenuItemState(MENU_GRABCONTINOUS, FALSE);
	}
}

//*********************************************************************************************
void CGUIAppApp::OnCulling() 
{
	Assert(pappdlgMainWindow);
	
	pappdlgMainWindow->InvokeCullingDialog();
}

//*********************************************************************************************
void CGUIAppApp::OnAcceleration() 
{
	Assert(pappdlgMainWindow);

	d3dDriver.Purge();
	if (d3dDriver.bUseD3D())
	{
		d3dDriver.Uninitialize();
	}
	else
	{
		d3dDriver.SetInitEnable(true);

		pappdlgMainWindow->UpdateScreen();

		if (d3dDriver.bInitialize())
			d3dDriver.Restore();
		else
		{
			bTerminalError
			(
				ERROR_D3D_INITIALIZATION_FAILED,	// The terminating error from "resource.h"
				true,								// If 'true,' offer the option of continuing.
				"Consult your manual"				// Additional message information.
			);	
		}
	}
	pappdlgMainWindow->SetMenuState();
	
}

//*********************************************************************************************
void CGUIAppApp::OnWaterAlpha() 
{
	Assert(pappdlgMainWindow);

	CEntityWater::bAlpha = !CEntityWater::bAlpha;
	prenMain->pSettings->bSoftwareAlpha = !prenMain->pSettings->bSoftwareAlpha;

	// Re-create light blend for water.
	lbAlphaWater.CreateBlendForWater(prasMainScreen.ptPtrRaw());

	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnWaterInterpolate() 
{
	Assert(pappdlgMainWindow);

	CEntityWater::bInterp = !CEntityWater::bInterp;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnAssertSave() 
{
	Assert(pappdlgMainWindow);

	bool b_auto_save = !bGetAutoSave();
	
	bDisbableAutoSaveScene = !b_auto_save;
	SetAutoSave(b_auto_save);
	AlwaysAssert(b_auto_save == (bGetAutoSave() != 0));
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnIgnoreMsgRecipients() 
{
	Assert(pappdlgMainWindow);

	CMessage::bIgnoreRegisteredRecipients = !CMessage::bIgnoreRegisteredRecipients;
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnHardwareStats() 
{
	Assert(pappdlgMainWindow);

	pappdlgMainWindow->bShowHardwareStats = !pappdlgMainWindow->bShowHardwareStats;
	pappdlgMainWindow->SetMenuState();

	// Turn on the frame counter if the hardware stats are up.
	if (pappdlgMainWindow->bShowHardwareStats)
		pappdlgMainWindow->bShowFPS = true;
}

//*********************************************************************************************
void CGUIAppApp::OnFilterCaches() 
{
	Assert(pappdlgMainWindow);

	d3dDriver.SetFilterImageCaches(!d3dDriver.bFilterImageCaches());
	pappdlgMainWindow->SetMenuState();
}

//*********************************************************************************************
void CGUIAppApp::OnFreezeCaches() 
{
	rcsRenderCacheSettings.bFreezeCaches = !rcsRenderCacheSettings.bFreezeCaches;
	pappdlgMainWindow->SetMenuState();
}


//
// Local function implementations.
//

//*********************************************************************************************
float fGetFloatFromString(char* str_float)
{
	float f = 0.0f;

	Verify(sscanf(str_float, "%f", &f) == 1);
	return f;
}

//Global variables and functions declared elsewhere as extern
//needed by the libraries
void LineColour(int, int, int) {}
