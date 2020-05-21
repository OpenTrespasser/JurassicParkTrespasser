/*********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents: Implementation of 'GUIAppDlg.h.'
 *
 **********************************************************************************************
 *
 * $Log: /JP2_PC/Source/GUIApp/GUIAppDlg.cpp $
 * 
 * 598   98.10.03 11:07p Mmouni
 * Disabled setup for self modifying code.
 * 
 * 597   9/26/98 8:09p Pkeet
 * Added stats for Kyle.
 * 
 * 596   98/09/25 1:39 Speter
 * In silly T debug code, use rTINHeight function rather than more
 * dangerous one.
 * 
 * 595   98.09.20 7:21p Mmouni
 * Removed reduntant main clut add code.
 * 
 * 594   98/09/19 14:35 Speter
 * Damn third-person view now uses correct damn distance.
 * 
 * 593   98.09.19 1:58a Mmouni
 * Made console buffer bigger so I can see all the fucking stats.
 * 
 * 592   9/18/98 11:04p Agrant
 * don't delete the main palette-  let the palette database take care of
 * it
 * 
 * 591   9/18/98 2:02a Rwyatt
 * New function to return a resource as a string
 * 
 * 590   9/17/98 11:12p Shernd
 * First pass on updating during d3d surface setup
 * 
 * 589   9/17/98 10:23p Pkeet
 * Made the stats dump work again.
 * 
 * 588   9/17/98 5:37p Pkeet
 * Changed way the main clut is built.
 * 
 * 587   9/16/98 12:32a Agrant
 * removed obsolete implementation of smack selected
 * 
 * 586   98.09.15 8:52p Mmouni
 * Fixed problem with switching on stats after gong full screen.
 * 
 * 585   9/14/98 4:43p Mlange
 * Drag select is now on CTRL instead of SHIFT.
 * 
 * 584   9/14/98 12:09a Rwyatt
 * Added CTRL-F7 key to flush audio caches
 * 
 * 583   98.09.12 12:18a Mmouni
 * Changed shape query to render type query.
 * 
 * 582   98/09/09 12:52 Speter
 * Give us us stow.
 * 
 * 581   9/09/98 12:02p Pkeet
 * Activated texture tracking using 'Shift F3.'
 * 
 *********************************************************************************************/

//
// Includes.
//
#include "stdafx.h"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "mmsystem.h"
#include "Lib/Audio/SoundTypes.hpp"
#include "Lib/Audio/Audio.hpp"

#include "GUIAppDlg.h"
#include "GUIApp.h"
#include "PreDefinedShapes.hpp"
#include "GUIPipeLine.hpp"
#include "GUITools.hpp"
#include "LightProperties.hpp"
#include "CameraProperties.hpp"
#include "Background.hpp"
#include "DialogFog.hpp"
#include "DialogString.hpp"
#include "SoundPropertiesDlg.hpp"
#include "DialogMemLog.hpp"
#include "DialogRenderCache.hpp"
#include "Toolbar.hpp"

#include "Lib/Sys/ProcessorDetect.hpp"
#include "Lib/Sys/PerformanceCount.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include "Shell/WinRenderTools.hpp"
#include "Lib/Control/Control.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"
#include "Lib/EntityDBase/EntityLight.hpp"
#include "Lib/EntityDBase/Replay.hpp"
#include "Lib/EntityDBase/Animal.hpp"
#include "Lib/EntityDBase/AnimationScript.hpp"

#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/Query/QMessage.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"

#include "Lib/EntityDBase/MessageTypes/MsgControl.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgCollision.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Game/DesignDaemon/Gun.hpp"
#include "Lib/GeomDBase/Skeleton.hpp"
#include "Lib/GeomDBase/WaveletDataFormat.hpp"
#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/Groff/GroffIO.hpp"
#include "Lib/Loader/Loader.hpp"
#include "Lib/Loader/DataDaemon.hpp"
#include "Lib/Loader/LoadTexture.hpp"
#include "Lib/Physics/PhysicsSystem.hpp"
#include "Lib/Physics/Xob_bc.hpp"
#include "Lib/Physics/PhysicsImport.hpp"
#include "Lib/Renderer/PipeLine.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/W95/Dd.hpp"
#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "DialogObject.h"
#include "DialogMagnet.h"
#include "DialogGamma.hpp"
#include "DialogGore.hpp"
#include "DialogPhysics.hpp"
#include "DialogSky.hpp"
#include "DialogTexturePack.hpp"
#include "PerspectiveSubdivideDialog.hpp"
#include "Lib/Renderer/RenderCacheInterface.hpp"
#include "DialogDepthSort.hpp"
#include "DialogOcclusion.hpp"
#include "DialogGun.hpp"
#include "DialogScheduler.hpp"
#include "DialogVM.hpp"
#include "DialogBumpPack.hpp"
#include "DialogTexturePackOptions.hpp"
#include "QualityDialog.hpp"
#include "AI Dialogs2Dlg.h"
#include "ParameterDlg.h"
#include "DialogCulling.hpp"
#include "DialogSoundMaterial.hpp"
#include "DialogTeleport.h"

#include "TerrainTest.hpp"

#include "Lib/EntityDBase/PhysicsInfo.hpp"
#include "Game/AI/AIMain.hpp"
#include "Game/AI/AIInfo.hpp"
#include "Game/AI/Brain.hpp"
#include "Game/AI/MentalState.hpp"
#include "Game/DesignDaemon/Daemon.hpp"

#include "Lib/Renderer/DepthSort.hpp"

#include "Lib/Sys/StdDialog.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "DialogTerrain.hpp"
#include "DialogWater.hpp"
#include "Lib/Sys/FileEx.hpp"
#include "DialogMipmap.hpp"
#include "Lib/GeomDBase/TerrainLoad.hpp"
#include "Lib/GeomDBase/TerrainTexture.hpp"

#include "Lib/Physics/InfoSkeleton.hpp"
#include "Lib/Physics/Magnet.hpp"

// the joystick support stuff
#include "mmsystem.h"
#include "Lib/Audio/AudioDaemon.hpp"
#include "Lib/Audio/Audio.hpp"
#include "Lib/Audio/Material.hpp"
#include "Lib/EntityDBase/RenderDB.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/MovementPrediction.hpp"
#include "Lib/View/RasterFile.hpp"

#include "Lib/Renderer/LightBlend.hpp"
#include "DialogAlphaColour.hpp"
#include "Lib/Renderer/Primitives/FastBumpTable.hpp"
#include "Lib/Sys/ExePageModify.hpp"
#include "Lib/Renderer/Occlude.hpp"
#include "Lib/Renderer/Sky.hpp"
#include "Lib/Sys/Scheduler.hpp"
#include "Lib/Sys/FixedHeap.hpp"
#include "Lib/Sys/RegInit.hpp"
#include "Lib/EntityDBase/RenderDB.hpp"
#include "Lib/EntityDBase/Water.hpp"
#include "Lib/Renderer/RenderCache.hpp"
#include "Lib/Std/Hash.hpp"

#include "Lib/Trigger/Trigger.hpp"
#include "Lib/sys/Reg.h"
#include "lib/sys/reginit.hpp"
#include "lib/loader/LoadTexture.hpp"


#include <stdio.h>
#include <stdlib.h>


#ifndef PFNWORLDLOADNOTIFY
typedef uint32 (__stdcall * PFNWORLDLOADNOTIFY)(uint32 dwContext, uint32 dwParam1, uint32 dwParam2, uint32 dwParam3);
#endif

PFNWORLDLOADNOTIFY          g_pfnWorldLoadNotify;
uint32                      g_u4NotifyParam;
bool                        bIsTrespasser = false;
bool                        bInvertMouse = false;

// Check to determine which version of the compiler we are running.  If anything less than 4.2,
// Don't include this file.  Consequences are unknown???
#if _MSC_VER > 1019
#include <exception>
#endif


//
// Statistics dump for Kyle.
//
#define bDUMP_STATS (0)

extern CProfileStat psRenderShapeReg;
extern CProfileStat psRenderShapeBio;
extern CProfileStat psRenderShapeTrr;
extern CProfileStat psRenderShapeImc;
//proProfile.psPresort
//proProfile.psDrawPolygon
extern CProfileStatParent psPixels;

// Replace this macro with the stat you want printed out as "other."
#define psOTHER_STAT psPixels


//
// Constants.
//

// Default timer.
const int iDEFAULT_TIMER = 1;

// Default timer step in milliseconds.
const int iDEFAULT_TIMER_STEP = 33;

//
// Default strength for non-ambient lights added.  
// You can have multiple lights of maximum strength, as they use fCombine to combine.
//
const TLightVal	lvDEFAULT = 1.0;

const char* strLAST_SCENE_FILE = "LastScene.scn";

// Amount to change viewport by.
const float fViewDeltaIncrease = 1.1f;
const float fViewDeltaDecrease = 1.0f / fViewDeltaIncrease;

//
// Module variables.
//

extern CProfileStat psCacheSched;

// How far do we move with debug movement (time corrected)???
TReal	rDebugStepDistance	= 6.7f;
TReal	rDebugTurnAngle		= 120.0f;

// Pointer to the toolbar.
CTool* pdlgToolbar = 0;

CDialogGamma* pdlgobjDialogGamma = 0;

CDialogGore* pdlgobjDialogGore = 0;

// Pointer to the perspective correction properties dialog.
CPerspectiveSubdivideDialog* pdlgPerspectiveSubdivide = 0;

// Pointer to the object properties dialog.
CDialogRenderCache* pdlgrcDialogRenderCache = 0;

// Pointer to the object properties dialog.
CDialogObject* pdlgobjDialogObject = 0;

// Pointer to the material properties dialog.
CDialog* pdlgDialogMaterial = 0;
CDialog* newCDialogMaterial(CInstance* pins);

// Pointer to the magnet properties dialog.
CDialogMagnet* pdlgmagDialogMagnet = 0;

// Pointer to the player properties dialog.
CDialog* pdlgDialogPlayer = 0;
CDialog* newCDialogPlayer();

// Pointer to the physics dialog.
CDialogPhysics* pdlgphyDialogPhysics = 0;

// Pointer to the camera properties dialog.
CCameraProperties* pcamdlgCameraProperties = 0;

// Pointer to the light properties dialog.
CLightProperties*  pltdlgLightProperties = 0;

// Pointer to the background dialog box.
CBackground* pbackdlgBackground = 0;

// Pointer to the occlusion dialog box.
CDialogOcclusion* pdlgOcclusion = 0;

// Pointer to the scheduler dialog box.
CDialogScheduler* pdlgScheduler = 0;

// Pointer to the Gun dialog box.
CDialogGun* pdlgGun = 0;

// Pointer to the sky settings dialog box
CDialogSky* pdlgSky = 0;

// Pointer to the VM settings dialog box
CDialogVM* pdlgVM = 0;

CDialogQuality* pdlgQuality = 0;

// Pointer to the alpha colour settings dialog box.
CDialogAlphaColour* pdlgAlphaColour = 0;

// Pointer to the terrain dialog.
CDialogTerrain* pdlgtDialogTerrain = 0;

// Pointer to the water dialog.
CDialogWater* pdlgtDialogWater = 0;

// Pointer to the fog properties dialog box.
CDialogFog* pfogdlgFogProperties = 0;
CDialogDepthSort* pdepthdlgProperties = 0;
CDialogSoundProp* psounddlgProperties = 0;
CDialogMipmap* pdlgMipmap = 0;
CDialogTexturePack* pdlgTexturePack = 0;
CDialogSoundMaterial* pdlgSoundMaterial = 0;

CString		csProfileName = "Profile - Time";

// Initial camera position is 2m above sea level.
// (This variable is altered by the command line).
CVector3<> v3InitCamPos(0.0, 0.0, 2.0);

TSec sWanderStop = -1.0f;
bool bWanderDurationInSecs = true;

// Do we actually want to create the output files
bool    bUseOutputFiles = TRUE;
bool    bUseReplayFile = TRUE;

// Static wander variables (file scope to allow reset).
static CRandom randWander;
static float fWanderAngleRate = 0;			// Rate of angular of rotation about Z.

// Art stats maximums.
int i_max_total_polys	= 0;
int i_max_cache_polys	= 0;
int i_max_terrain_polys	= 0;

CGUIAppDlg* pgui = 0;

//
// Externally defined variables.
//
extern int iNumPixelsIterated;
extern int iNumPixelsSolid;

bool bExiting = false;
bool bScreensaver = false;

FILE* fileStats = 0;


//
// Forward declarations of module functions.
//

//*********************************************************************************************
//
void SendMessageToWindow
(
	CWnd* pwnd,		// Window to receive message.
	uint  u_message	// Windows message id.
);
//
// Sends a WM_CLOSE message to the specified window.
//
//**************************************

//*********************************************************************************************
//
bool bVKey
(
	uint u_virtualkey	// Virtual key to sample.
)
//
// Returns 'true' if the virtual key is being pressed.
//
//**************************************
{
	return (GetAsyncKeyState(u_virtualkey) & (SHORT)0xFFFE) != 0;
}


//+--------------------------------------------------------------------------
//
//  Function:   CreateDirAlongPath
//
//  Synopsis:   Create the directories necessary along the path.  We pick
//              off the file name then iteratively create the directories
//
//  Arguments:  [pszPath]     -- file name to create the directories based 
//                               off of
//              [bFileIsPath] -- if the pszPath is just a path with no
//                               file name at the end
//
//  Returns:    BOOL --  TRUE  -- if successful
//                       FALSE -- if NOT successful
//
//  History:    10-Oct-95   SHernd   Created
//
//---------------------------------------------------------------------------
BOOL CreateDirAlongPath(LPSTR pszPath, BOOL bFileIsPath = TRUE)
{
    char    szDir[_MAX_PATH];
    char    szPath[_MAX_PATH];
    char    szDrive[_MAX_DRIVE];
    LPSTR   pszEnd;
    LPSTR   psz;
    LPSTR   pszTemp;
    BOOL    bRet;
    DWORD   dw;

    if (!bFileIsPath)
    {
        _splitpath(pszPath, szDrive, szPath, NULL , NULL);
        strcpy(szDir, szDrive);
        strcat(szDir, szPath);
    }
    else
    {
        strcpy(szDir, pszPath);
        if (szDir[strlen(szDir) - 1] != '\\')
        {
            strcat(szDir, "\\");
        }
    }

    pszEnd = strchr(szDir, '\0');
    psz = szDir;

    pszTemp = strchr(psz, '\\');
    for (pszTemp = strchr(pszTemp + 1, '\\');
         (pszTemp != pszEnd) && (pszTemp != NULL);
         psz = pszTemp + 1, pszTemp = strchr(psz, '\\'))
    {
        *pszTemp = '\0';

        dw = GetFileAttributes(szDir);

        if (dw == (DWORD)-1)
        {
            if (!CreateDirectory(szDir, NULL))
            {
                goto Error;
            }
        }

        *pszTemp = '\\';
    }

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    bRet = FALSE;
    goto Cleanup;
}


//
// Class implementation.
//

//*********************************************************************************************
//
// Dialog message map.
//
#define WM_KICKIDLE		0x036A	// private to MFC.


BEGIN_MESSAGE_MAP(CGUIAppDlg, CDialog)
	//{{AFX_MSG_MAP(CGUIAppDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CLOSE()
	ON_COMMAND(MENU_EDIT_GAMMA, OnEditGamma)
	ON_COMMAND(MENU_GORE, OnEditGore)
	ON_COMMAND(MENU_ZBUFFER, OnZbuffer)
	ON_COMMAND(MENU_SCREENCLIP, OnScreenclip)
	ON_COMMAND(MENU_SCREENCULL, OnScreencull)
	ON_COMMAND(MENU_LIGHT, OnLight)
	ON_COMMAND(MENU_LIGHT_SHADE, OnLightShade)
	ON_COMMAND(MENU_FOG, OnFog)
	ON_COMMAND(MENU_FOG_SHADE, OnFogShade)
	ON_COMMAND(MENU_COLOURED, OnColoured)
	ON_COMMAND(MENU_TEXTURE, OnTexture)
	ON_COMMAND(MENU_TRANSPARENT, OnTransparent)
	ON_COMMAND(MENU_BUMP, OnBump)
	ON_COMMAND(MENU_WIREFRAME, OnWireFrame)
	ON_COMMAND(MENU_ALPHA_COLOUR, OnAlphaColour)
	ON_COMMAND(MENU_ALPHA_SHADE, OnAlphaShade)
	ON_COMMAND(MENU_ALPHA_TEXTURE, OnAlphaTexture)
	ON_COMMAND(MENU_TRAPEZOIDS, OnTrapezoids)
	ON_COMMAND(MENU_SUBPIXEL, OnSubpixel)
	ON_COMMAND(MENU_PERSPECTIVE, OnPerspective)
	ON_COMMAND(MENU_MIPMAP, OnMipMap)
	ON_COMMAND(MENU_DITHER, OnDither)
	ON_COMMAND(MENU_FILTER, OnFilter)
	ON_COMMAND(MENU_FILTER_EDGES, OnFilterEdges)
	ON_COMMAND(MENU_SPECULAR, OnSpecular)
	ON_COMMAND(MENU_SHADOWS, OnShadows)
	ON_COMMAND(MENU_SHADOW_TERRAIN, OnShadowTerrain)
	ON_COMMAND(MENU_SHADOW_TERRAIN_MOVING, OnShadowTerrainMove)
	ON_COMMAND(MENU_RENDER_CACHE, OnRenderCache)
	ON_COMMAND(MENU_RENDER_CACHE_TEST, OnRenderCacheTest)
	ON_COMMAND(MENU_SET_MIPMAP, OnMipMapSettings)
	ON_COMMAND(MENU_DETAIL, OnDetail)
	ON_COMMAND(MENU_CONSOLE, OnShowConsole)
	ON_COMMAND(MENU_STATS, OnShowStats)
	ON_COMMAND(MENU_AVG_STATS, OnAvgStats)
	ON_COMMAND(MENU_FPS, OnShowFPS)
	ON_COMMAND(MENU_CACHE_STATS, OnShowCacheStats)
	ON_COMMAND(MENU_STATPHYSICS, OnStatPhysics)
	ON_COMMAND(MENU_VIEW_HAIRS, OnViewHairs)
	ON_COMMAND(MENU_VIEW_CROSSHAIR_RADIUS, OnCrosshairRadius)
	ON_COMMAND(MENU_VIEW_CROSSHAIR_VEGETATION, OnCrosshairVegetation)
	ON_COMMAND(MENU_VIEW_CROSSHAIR_TERRAINTEXTURES, OnCrosshairTerrainTexture)
	ON_COMMAND(MENU_VIEW_SPHERES, OnViewSpheres)
	ON_COMMAND(MENU_VIEW_WIRE, OnViewWire)
	ON_COMMAND(MENU_VIEW_PINHEAD, OnViewPinhead)
	ON_COMMAND(MENU_VIEW_BONES, OnViewBones)
	ON_COMMAND(MENU_VIEW_BONES_BOXES,		OnViewBonesBoxes)
	ON_COMMAND(MENU_VIEW_BONES_PHYSICS,		OnViewBonesCollide)
	ON_COMMAND(MENU_VIEW_BONES_WAKE,		OnViewBonesWake)
	ON_COMMAND(MENU_VIEW_BONES_QUERY,		OnViewBonesQuery)
	ON_COMMAND(MENU_VIEW_BONES_MAGNETS,		OnViewBonesMagnets)
	ON_COMMAND(MENU_VIEW_BONES_SKELETONS,	OnViewBonesSkeletons)
	ON_COMMAND(MENU_VIEW_BONES_ATTACH,		OnViewBonesAttach)
	ON_COMMAND(MENU_VIEW_BONES_WATER,		OnViewBonesWater)
	ON_COMMAND(MENU_VIEW_BONES_RAYCAST,		OnViewBonesRaycast)
	ON_COMMAND(MENU_VIEW_QUADS, OnViewQuads)
	ON_COMMAND(MENU_ALPHA_SETTINGS, OnAlphaColourSettings)
	ON_COMMAND(MENU_PLAYERPHYSICS, OnPlayerPhysics)
	ON_COMMAND(MENU_PLAYERINVULNERABLE, OnPlayerInvulnerable)
	ON_COMMAND(MENU_PHYSICS_SLEEP, OnPhysicsSleep)
	ON_COMMAND(MENU_PHYSICS_STEP, OnPhysicsStep)
	ON_COMMAND(MENU_PHYSICS_PUTTOSLEEP, OnPhysicsPutToSleep)
	ON_COMMAND(MENU_SMACKSELECTED, OnSmackSelected)
	ON_COMMAND(MENU_MAGNET_BREAK, OnMagnetBreak)
	ON_COMMAND(MENU_MAGNET_NOBREAK, OnMagnetNoBreak)
	ON_COMMAND(MENU_DEMAGNET, OnDeMagnet)
	ON_COMMAND(MENU_REDRAW_TERRAIN, OnRedrawTerrain)
	ON_COMMAND(MENU_CAMERAPLAYER, OnCameraPlayer)
	ON_COMMAND(MENU_CAMERASELECTED, OnCameraSelected)
	ON_COMMAND(MENU_CAMERASELECTEDHEAD, OnCameraSelectedHead)
	ON_COMMAND(MENU_CAMERA2M, OnCamera2m)
	ON_COMMAND(MENU_CAMERAFREE, OnCameraFree)
	ON_COMMAND(MENU_OPEN_SCENE, OnLoadScene)
	ON_COMMAND_RANGE(MENU_OPEN_LAST, MENU_OPEN_LAST+4, OnLoadRecentFile)
	ON_COMMAND(MENU_SAVE_SCENE, OnSaveScene)
	ON_COMMAND(MENU_SAVE_AS_SCENE, OnSaveAsScene)
	ON_COMMAND(MENU_TEXTSAVE, OnTextSave)
	ON_COMMAND(MENU_REPLAYSAVE, OnReplaySave)
	ON_COMMAND(MENU_REPLAYLOAD, OnReplayLoad)
	ON_COMMAND(MENU_LOADANIM, OnLoadAnim)
	ON_COMMAND(MENU_CONTROLS_DEFAULTCONTROLS,OnDefaultControls)
	ON_COMMAND(MENU_CONTROLS_STANDARDJOYSTICK,OnStandardJoystick)
	ON_COMMAND(MENU_PLAYER_PROP, OnPlayerProperties)
	ON_COMMAND(MENU_MATERIAL_PROP, OnMaterialProperties)
	ON_COMMAND(MENU_PHYSICS_PROP, OnPhysicsProperties)
	ON_COMMAND(MENU_SOUND_MATERIAL, OnSoundMaterialProp)
	ON_COMMAND(MENU_CHANGE_SMAT, OnChangeSMat)
    ON_COMMAND(IDM_TELEPORT, OnTeleport)
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_COMMAND(MENU_SYSTEM_MEM, OnSystemMem)
	ON_COMMAND(MENU_TERRAIN, OnMenuTerrain)
	ON_COMMAND(MENU_ADD_SKELETON, OnMenuAddSkeleton)
	ON_COMMAND(MENU_AI, OnAI)
	ON_COMMAND(MENU_PHYSICS, OnPhysics)
	ON_COMMAND(MENU_RESET, OnReset)
	ON_COMMAND(MENU_RESET_SELECTED, OnResetSelected)
	ON_COMMAND(MENU_ARTSTATS, OnArtStats)
	ON_COMMAND(MENU_PREDICTMOVEMENT, OnPredictMovement)
	ON_COMMAND(MENU_STAT_CLOCKS, OnStatClocks)
	ON_COMMAND(MENU_STAT_MSR0, OnStatMSR0)
	ON_COMMAND(MENU_STAT_MSR1, OnStatMSR1)
	ON_COMMAND(MENU_STAT_RING3, OnStatRing3)
	ON_COMMAND(MENU_STAT_RING0, OnStatRing0)
	ON_COMMAND_RANGE(MENU_PROFILE_STAT,     MENU_PROFILE_STAT+249, OnChangeStatCounter0)
	ON_COMMAND_RANGE(MENU_PROFILE_STAT+250, MENU_PROFILE_STAT+500, OnChangeStatCounter1)
	ON_COMMAND(MENU_TEXTUREPACK, OnTexturePack)
	ON_COMMAND(MENU_MEMLOG_STATS, OnMemStats)
	ON_COMMAND(MENU_GIVEMEALIGHTL, OnGiveMeALight)
	ON_COMMAND(MENU_ZONLYEDIT, OnZOnlyEdit)
	ON_COMMAND(MENU_NAMESELECT, OnNameSelect)
	ON_COMMAND(MENU_SMALLMOVESTEPS, OnSmallMoveSteps)
	ON_COMMAND(ID_EDIT_TERRAIN_TERRAINSOUNDS, OnTerrainSound)
	ON_COMMAND(MENU_SKY_DISABLE, OnSkyDisable)
	ON_COMMAND(MENU_SKY_REMOVE, OnSkyRemove)
	ON_COMMAND(MENU_SKY_TEXTURE, OnSkyTexture)
	ON_COMMAND(MENU_SKY_FILL, OnSkyFill)
	ON_COMMAND(MENU_SKY_SETTINGS, OnSkySettings)
	ON_COMMAND(MENU_VM_SETTINGS, OnVMSettings)
	ON_COMMAND(MENU_BUMP_PACK, OnBumpPacking)
	ON_COMMAND(MENU_PACK_OPTIONS, OnPackOptions)
	ON_COMMAND(MENU_EDIT_AI, OnEditAI)
	ON_COMMAND(MENU_EDIT_AI_EMOTIONS, OnEditAIEmotions)
	ON_COMMAND(MENU_DRAWPHYSICS, OnDrawPhysics)
	ON_COMMAND(MENU_DRAWAI, OnDrawAI)
	ON_COMMAND(MENU_ROTATE_WORLDZ, OnRotateWorldZ)
	ON_COMMAND(MENU_RESET_START_TRIGGERS, OnResetStartTriggers)
	ON_COMMAND(MENU_RESTORE_DEFAULTS, OnRestoreSubsystemDefaults)
	ON_COMMAND(MENU_SETTINGS_RENDERQUALITY, OnRenderQualitySettings)
	ON_MESSAGE(WM_KICKIDLE, OnIdle)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CGUIAppDlg implementation.
//

//  The onexit guy.
void GUIAppOnExit()
{
	// Close the buffer for printing framerate stats out.
	if (fileStats)
	{
		fclose(fileStats);
		fileStats = 0;
	}

	// Purge the world database to ensure that all the instances are deleted before the
	// static variables are deleted.
	if (pwWorld)
		pwWorld->Purge();

	dprintf("Leftover CMeshes:       %d\n", ulGetMemoryLogCounter(emlMeshes) );
	dprintf("Leftover CPhysicsInfos: %d\n", ulGetMemoryLogCounter(emlTotalPhysicsInfo) );
	dprintf("Leftover CAIInfos:      %d\n", ulGetMemoryLogCounter(emlTotalAIInfo) );
	dprintf("Leftover CInfos:        %d\n", ulGetMemoryLogCounter(emlTotalCInfo) -1);

	// Delete memory associated with any instantiated dialog boxes.
	delete pdlgToolbar;
	delete pcamdlgCameraProperties;
	delete pdlgPerspectiveSubdivide;
	delete pdlgobjDialogObject;
	delete pdlgmagDialogMagnet;
	delete pdlgDialogPlayer;
	delete pdlgDialogMaterial;
	delete pdlgrcDialogRenderCache;
	delete pdlgphyDialogPhysics;
	delete pltdlgLightProperties;
	delete pbackdlgBackground;
	delete pdlgOcclusion;
	delete pdlgScheduler;
	delete pdlgGun;
	delete pdlgAlphaColour;
	delete pdlgtDialogTerrain;
	delete pdlgtDialogWater;
	delete pfogdlgFogProperties;
	delete pdlgMipmap;
	delete pdepthdlgProperties;

	// Delete the audio system
	delete CAudio::pcaAudio;

	// Deallocate memory for the mipmap directory file string.
	DeleteMipDirectoryString();

	delete pwWorld;
	bExiting = true;

	// Reactivate the screen saver.
	if (bScreensaver)
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, 0, SPIF_SENDWININICHANGE);

	// if there is a main palette, delete its CPal
	//  NO don't.  It's handled in the pcdbMain destructor
//	if (pcdbMain.pceMainPalClut)
//		delete pcdbMain.pceMainPalClut->ppalPalette;

}

//*********************************************************************************************
static CGUIAppDlg* pguiRepaint = 0;

void PaintShellWindow()
{
	Assert(pguiRepaint)
	pguiRepaint->PaintWindow();
}

//*********************************************************************************************
//
// CGUIAppDlg Constructor.
//

CGUIAppDlg::CGUIAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGUIAppDlg::IDD, pParent), conProfile("Profile", 0, 68, 64)
{
	//{{AFX_DATA_INIT(CGUIAppDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Set the default background fill colour.
//	clrBackground = CColour(0, 0, 0);
	iScreenMode		 = -1;					// Default is window mode.
	bSystemMem		 = VER_DEBUG;			// Default system memory flag depends on debug mode.
	if (bGetD3D())
		bSystemMem = false;
	bShowConsole	 = false;
	bShowStats		 = false;

	bShowHardwareStats = false;
	bShowFPS		 = false;

	bShowHairs		 = true;
	bCrosshairRadius = false;
	rCrosshairRadius = 50.0f;
	bCrosshairVegetation = true;
	bShowSpheres	 = false;
	bShowWire		 = false;
	bShowPinhead	 = false;
	bAvgStats		 = false;
	bShowToolbar	 = false;
	bJustMoved		 = false;

	bFastMode        = false;
	bFastModeRequest = false;

	bGameModeRequest = false;
	bReplayRequest	 = false;

	bLoadRequest	 = false;

	bDelaunayTest    = false;
	bTerrainTest     = false;

	bCameraFloating  = false;
	bWander			 = false;
	bPhysicsStep	 = false;

	bAllowDebugMovement = true;
	bZOnlyEdit		 = false;
	bSmallMoveSteps	 = false;

	bTerrainSound	 = true;
	bConvertTerrain	 = false;

	bRotateAboutWorldZ = true;

	*strLoadFile = 0;

	// Cleanup code!
	Assert(pgui == 0);
	pgui = this;
	pguiRepaint = this;

	AlwaysAssert(!atexit(GUIAppOnExit));

	// Initing the static objects here.
	CInitStaticObjects InitStaticObjects;

	// Verify color resolution.
	HDC hdc = ::GetDC(0);
	int i_bits = ::GetDeviceCaps(hdc, BITSPIXEL) * ::GetDeviceCaps(hdc, PLANES);
	if (!(i_bits == 16 || i_bits == 15))
	{
		::MessageBox(0,"Must be in 15 or 16 bit color mode!", "GUIApp Error", MB_OK | MB_ICONERROR);
		::ReleaseDC(0,hdc);
		exit(0);
	}
	::ReleaseDC(0,hdc);
}

//*********************************************************************************************
void CGUIAppDlg::operator delete(void *pv_mem)
{
}


//*********************************************************************************************
//
// ClassWizard generated functions..
//

//*********************************************************************************************
//
void CGUIAppDlg::Step(TSec s_step)
//
// Perform a step in the system.
//
//**************************************
{
	CCycleTimer ctmr;

	if (bGDIMode)
		return;

	if (crpReplay.bLoadActive())
	{
		//
		// if we are playing a replay the call then Replay member of
		// the GameLoop class.
		// we do not need to worry abou the timing info
		//
		gmlGameLoop.Replay(s_step);
	}
	else
	{
		//
		// Call the normal main game loop if we are not playing a replay
		//
		gmlGameLoop.Step(s_step);
	}

	// Add the time to both the step stat and the frame stat.
	TCycles cy = ctmr();
	proProfile.psStep.Add(cy, 1);
	proProfile.psFrame.Add(cy);

	// Redraw the screen.
	PaintWindow();
}


//*********************************************************************************************
//
void CGUIAppDlg::OnTimer
(
	UINT nIDEvent
) 
//
// Timer for the main game loop.
//
//**************************************
{
	// Do nothing if the world database is locked.
	if (wWorld.bIsLocked() || wWorld.bHasBeenPurged)
		return;

	// For now, allow debug movement at all times.
	CInstance* pins_move = pcamGetCamera()->pinsAttached() ? pcamGetCamera()->pinsAttached() : pcamGetCamera();
	DebugMovement(pins_move, pcamGetCamera());

	OnIdle(0, 0);
}

//*********************************************************************************************
LRESULT CGUIAppDlg::OnIdle(WPARAM wParam, LPARAM lParam)
//
// Executes the main game loop.
//
//**************************************
{	
	// Do nothing if the world database is locked.
	if (wWorld.bIsLocked())
		return 1;

	// Do nothing if the game loop cannot step.
	if (!gmlGameLoop.bCanStep())
		return 0;

	// Check for pending save.
	if (wWorld.bIsSavePending())
	{
		const std::string &strName = wWorld.strGetPendingSave();

		// Stop simulation.
		CMessageSystem(escSTOP_SIM).Dispatch();

		// Save the level.
		if (!wWorld.bSaveWorld(strName.c_str()))
			MessageBox("Cannot save scene!", "Save Error", MB_OK);

		// Start simulation.
		CMessageSystem(escSTART_SIM).Dispatch();
	}

	// Check for pending level load.
	if (wWorld.bIsLoadPending())
	{
		const std::string &strName = wWorld.strGetPendingLoad();

		char szFile[_MAX_PATH];

		// HACK: to get data drive location.
		GetRegString(REG_KEY_DATA_DRIVE, szFile, sizeof(szFile), "");

		// Add on data sub-directory to path.
		if (*szFile)
			strcat(szFile, "\\data\\");

		// Append file name.
		strcat(szFile, strName.c_str());

		// Stop simulation.
		CMessageSystem(escSTOP_SIM).Dispatch();

		// Load a new level.
		if (!wWorld.bLoadScene(szFile))
			MessageBox("Cannot load file!", "Load Error", MB_OK);
		else
			// Remember the last loaded scene.
			strcpy(strLoadFile, szFile);

		// Start simulation.
		CMessageSystem(escSTART_SIM).Dispatch();

		// Don't step just now.
		return 1;
	}

	Step(bPhysicsStep ? MAX_TIMESTEP : -1.0);

	return 1;
}


//*********************************************************************************************
void CGUIAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGUIAppDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

//*********************************************************************************************
//
BOOL CGUIAppDlg::OnInitDialog
(
)
//
// Initialize the dialog.
//
//**************************************
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	DragAcceptFiles();

	return TRUE;  // return TRUE  unless you set the focus to a control
}


//*********************************************************************************************
//
void CGUIAppDlg::OnPaint
(
)
//
// Handle the WM_PAINT message.
//
//**************************************
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();

		// Set fast render mode if requested.
		if (bFastModeRequest)
		{
			bFastModeRequest = false;
			ToggleFastMode();
			return;
		}

		if (bGameModeRequest)
		{
			bGameModeRequest = false;
			EnterGameMode();
		}

		if (bReplayRequest)
		{
			bReplayRequest = false;

//			RestoreGamePath();

			// open the replay file now so it cannot be used for savinging as well
			if (crpReplay.bOpenReadReplay(acReplayFileName)==FALSE)
			{
				//
				// we cannot open the selected file, either it does not exist
				// or it is not a replay file.
				//
				SetMenuItemState(MENU_REPLAYLOAD, FALSE);
				crpReplay.SetLoad(FALSE);
				return;
			}

			// we now have a valid relay filename so enable replays
			// tick the menu
			SetMenuItemState(MENU_REPLAYLOAD, TRUE);
			crpReplay.SetLoad(TRUE);
		}

		if (bLoadRequest)
		{
			CLoadWorld lw(strLoadFile);
			bLoadRequest = false;
		}

		if (!gmlGameLoop.bCanStep())
		{
			// Paint the window only if we are not in run mode.
			PaintWindow();
		}
	}
}


//*********************************************************************************************
//
HCURSOR CGUIAppDlg::OnQueryDragIcon()
//
// Returns the icon handle.
//
//**************************************
{
	return (HCURSOR)m_hIcon;
}


//*********************************************************************************************
//
LRESULT CGUIAppDlg::DefWindowProc
(
	UINT message,
	WPARAM wParam,
	LPARAM lParam
)
//
// Main window message handling loop. Generated by ClassWizard, and overridden to catch the
// WM_ERASEBKGND message to stop flickering.
//
//**************************************
{
	// Prevent the background of the dialog from being redrawn and causing flicker.
	if (message == WM_ERASEBKGND && !bGDIMode)
		return (LRESULT)0;

	// Enter a menu drawing mode if required.
	if (message == WM_ENTERMENULOOP || message == WM_SYSKEYDOWN)
		SetGDIMode(true);

	return CDialog::DefWindowProc(message, wParam, lParam);
}


//*********************************************************************************************
//
int CGUIAppDlg::OnCreate
(
	LPCREATESTRUCT lpCreateStruct
)
//
// Handles the WM_CREATE message. Created by AppWizard. Code is added here for first-time
// initialization of the renderer and pipeline.
//
//**************************************
{
	CCPUDetect		detProcessor;

	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//
	// Disable the screensaver.
	//
	SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, FALSE, &bScreensaver, 0);
	if (bScreensaver)
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, 0, SPIF_SENDWININICHANGE);


	//
	// Add a palette from a resource bitmap.
	//

	// Create the main palette if there isn't one.
    if (!pcdbMain.ppalMain)
	{
		pcdbMain.ppalMain = ppalGetPaletteFromResource(AfxGetInstanceHandle(), IDB_PALETTE);
	}

	// Create the main clut if there isn't one.
    pcdbMain.CreateMainClut();

    char    szInstalledDir[_MAX_PATH];

    GetRegString(REG_KEY_INSTALLED_DIR, szInstalledDir, sizeof(szInstalledDir), "");

    if (strlen(szInstalledDir) == 0)
    {
        dprintf("Trespasser is not installed\r\n");
        strcpy(szInstalledDir, "C:\\program files\\DreamWorks Interactive\\Trespasser\\");
        SetRegString(REG_KEY_INSTALLED_DIR, szInstalledDir);
        CreateDirAlongPath(szInstalledDir);
	}
    else
        strcat(szInstalledDir, "\\");
    

	//
	// THE WORLD IS INITIALIZED HERE AND NOT IN THE CONSTRUCTOR. THIS IS BECAUSE THE WINDOW HANDLE
	// OF THIS CLASS IS NOT VALID UNTIL THE ABOVE CALL.
	//
	// Create the audio entities.
	extern void*  hwndGetMainHwnd();

	CAudio* pca_audio = ::new CAudio(hwndGetMainHwnd(),
										(bool)GetRegValue(REG_KEY_AUDIO_ENABLE, TRUE),
										(bool)GetRegValue(REG_KEY_AUDIO_ENABLE3D, TRUE)
									);

	pwWorld = new CWorld();

	#if defined(__MWERKS__)
		SetupForSelfModifyingCode(AfxGetInstanceHandle());
	#endif

	// Open the performance monitoring system
	// set all performance timers to read clock ticks
	iPSInit();

	bGDIMode = false;

	// Create the renderer.
	prnshMain = new CRenderShell(m_hWnd, AfxGetApp()->m_hInstance, false);

	// Initialize the pipeline.
	pipeMain.Init();

	// Add direct draw screen modes to the menu.
	SetDriverMenu(GetSubMenu(GetSubMenu(GetMenu()->m_hMenu, 5), 0), 0, MENU_DRIVER_FIRST);
	SetScreenModeMenu(GetSubMenu(GetMenu()->m_hMenu, 6), 5, MENU_WINDOW + 1);
	
	// Set the full screen flag.
	bIsFullScreen = FALSE;

	//
	// If the application is using the registry, set the full screen flag based on the
	// registry entry.
	//
	if (bGetInitFlag(FALSE))
		bIsFullScreen = bGetFullScreen();

	// Create an instance of the toolbar.
	pdlgToolbar = 0;

	// Set the message capture flags.
	bLeftCapture = false;
	bRightCapture = false;

	bDragRect = false;

	// Set the menu.
	UpdateRecentFiles();
	SetMenuState();
	
	// Start the timer.
	SetTimer(iDEFAULT_TIMER, iDEFAULT_TIMER_STEP, 0);

	MoveCameraToInitPosition();

	// Add the shell to the world database.
	wWorld.AddShell(this);

	//
	// Set the default position of the window.
	//
	SetWindowPos(&wndTop, iGUIAPP_DEFAULT_OFFSET, iGUIAPP_DEFAULT_OFFSET, 0, 0,
		         SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOREDRAW);

	// Set the default window size.
	SetWindowSize(iGUIAPP_DEFAULT_SIZEMODE);

	// Flatten the world.
	wWorld.FlattenPartitions();
	
	// Parse those wacky options.
	void ParseOptionFile(char *);
	ParseOptionFile("options.txt");

	// Make the toolbar visible, but wait until after parsing the file.
	//ShowToolbar(bShowToolbar);

	// Initialize the world database for render caching.
	wWorld.InitializePartitions();

	// Initialize the menu state.
	SetMenuState();


	// have we managed to detect the type of the local processor??
	if ( detProcessor.bProcessorDetected() )
	{
		dprintf("LOCAL PROCESSOR: %s\n",detProcessor.strLocalProcessorName() );
		//******************************************************************************************
		#if TARGET_PROCESSOR == PROCESSOR_PENTIUM

			dprintf("TARGET PROCESSOR: Intel Pentium\n");

		#elif TARGET_PROCESSOR == PROCESSOR_PENTIUMPRO

			dprintf("TARGET PROCESSOR: Intel Pentium Pro/Pentium II\n");		

		#elif TARGET_PROCESSOR == PROCESSOR_K6_3D

			dprintf("TARGET PROCESSOR: AMD-K6 3D\n");

		#else
			#error Invalid [No] target processor specified
		#endif
		//******************************************************************************************
	}
	else
	{
		dprintf("Failed to detect type of local processor - check DLL\n");
	}


	StatsMenuState(detProcessor);	

	// Another crude hack - convert terrain file if user requested this from command line.
	if (bConvertTerrain)
	{
		if (!conTerrain.bIsActive())
			ToggleConTerrain();

		conTerrain.CloseFileSession();

		// Redraw the screen.
		PaintWindow();

		std::string str_full = strConvertTRRFileName;

		int i_first_colon_pos  = str_full.find(':');
		int i_second_colon_pos = str_full.find(':', i_first_colon_pos + 1);

		// Remove extension from filename.
		std::string str_filename     = str_full.substr(0, str_full.find('.'));
		std::string str_filename_ext = str_full.substr(str_full.find('.') + 1, 3);

		if (str_filename_ext == "trr")
		{
			AlwaysAssert(i_second_colon_pos == -1);

			std::string str_number = str_full.substr(i_first_colon_pos + 1);

			char* str_dummy;
			double d_number = strtod(str_number.c_str(), &str_dummy);

			ConvertTerrainData(str_filename.c_str(), d_number, conTerrain);
		}
		else if (str_filename_ext == "wtd")
		{
			AlwaysAssert(i_second_colon_pos != -1);
			
			std::string str_number = str_full.substr(i_first_colon_pos + 1, i_second_colon_pos - i_first_colon_pos - 1);

			char* str_dummy;
			double d_number = strtod(str_number.c_str(), &str_dummy);

			bool b_ratio = str_full.substr(i_second_colon_pos + 1) == "ratio";

			bool b_conform = false;

			int i_third_colon_pos = str_full.find(':', i_second_colon_pos + 1);

			if (i_third_colon_pos != -1)
				b_conform = str_full.substr(i_third_colon_pos + 1) == "conform";

			SaveTerrainTriangulation(str_filename.c_str(), d_number, b_ratio, b_conform, conTerrain);
		}
		else
			// Unrecognised extension.
			AlwaysAssert(false);

		conTerrain.CloseFileSession();

		PostQuitMessage(0);
	}

    int i_qual = GetRegValue("RenderQuality", iDEFAULT_QUALITY_SETTING);
    SetQualitySetting(i_qual);

	return 0;
}

#if VER_TIMING_STATS

//*********************************************************************************************
void CGUIAppDlg::StatsMenuState(CCPUDetect&	detProcessor)
{
	//default for all processors is to use RDTSC to obtain stats
	SetMenuItemState(MENU_STAT_CLOCKS,true, true);
	gbMSRProfile		= true;					// we can use MSRs
	gbUseRDTSC			= true;					// we are using RDTSC
	gbNormalizeStats	= true;					// stats is MS

	// if processor was not detected or performance driver was not found then disable
	// any access to the advanced profile options
	if ((detProcessor.bProcessorDetected() == false) || (bPSGoing() == false))
	{
		SetMenuItemState(MENU_STAT_MSR0,false, false);
		SetMenuItemState(MENU_STAT_MSR1,false, false);
		SetMenuItemState(MENU_STAT_RING0,false, false);
		SetMenuItemState(MENU_STAT_RING3,false, false);
		SetMenuItemState(MENU_STAT_SETMSR0,false, false);
		SetMenuItemState(MENU_STAT_SETMSR1,false, false);
		gbMSRProfile = false;

		return;
	}

	switch (detProcessor.cpumanProcessorMake())
	{
	// With intel processors we must have a pentium pro or pentium with MMX
	case cpumanINTEL:
		switch ( detProcessor.cpufamProcessorModel() )
		{
		// only pentiums with MMX support RDPMC
		case cpufamPENTIUM:
			if ((detProcessor.u4ProcessorFlags() & CPU_MMX) == 0)
			{
				// no MMX, no RDPMC..
				SetMenuItemState(MENU_STAT_MSR0,false, false);
				SetMenuItemState(MENU_STAT_MSR1,false, false);
				SetMenuItemState(MENU_STAT_RING0,false, false);
				SetMenuItemState(MENU_STAT_RING3,false, false);
				SetMenuItemState(MENU_STAT_SETMSR0,false, false);
				SetMenuItemState(MENU_STAT_SETMSR1,false, false);
				gbMSRProfile = false;
				return;
			}
			SetMenuItemState(MENU_STAT_RING3,bPSRing3(), gbMSRProfile);
			SetMenuItemState(MENU_STAT_RING0,bPSRing0(), gbMSRProfile);
			break;

		// all pentium pros support RDPMC instruction
		case cpufamPENTIUMPRO:
			SetMenuItemState(MENU_STAT_RING3,bPSRing3(), gbMSRProfile);
			SetMenuItemState(MENU_STAT_RING0,bPSRing0(), gbMSRProfile);
			break;

		// if it is a processor that we do not know about then disable the MSRs
		default:
			SetMenuItemState(MENU_STAT_MSR0,false, false);
			SetMenuItemState(MENU_STAT_MSR1,false, false);
			SetMenuItemState(MENU_STAT_RING0,false, false);
			SetMenuItemState(MENU_STAT_RING3,false, false);
			SetMenuItemState(MENU_STAT_SETMSR0,false, false);
			SetMenuItemState(MENU_STAT_SETMSR1,false, false);
			gbMSRProfile = false;
			return;
		}
		break;

	// AMD have no performance MSRs so disable the whole thing
	case cpumanAMD:
	default:
		SetMenuItemState(MENU_STAT_MSR0,false, false);
		SetMenuItemState(MENU_STAT_MSR1,false, false);
		SetMenuItemState(MENU_STAT_RING0,false, false);
		SetMenuItemState(MENU_STAT_RING3,false, false);
		SetMenuItemState(MENU_STAT_SETMSR0,false, false);
		SetMenuItemState(MENU_STAT_SETMSR1,false, false);
		gbMSRProfile = false;
		return;
	}


	MENUITEMINFO	mii;

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU;
	mii.hSubMenu = PSMakeProcessorMenuCounter0(MENU_PROFILE_STAT);
	SetMenuItemInfo(GetSubMenu(GetMenu()->m_hMenu, 2),
					MENU_STAT_SETMSR0,
					false, 
					&mii);

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU;
	mii.hSubMenu = PSMakeProcessorMenuCounter1(MENU_PROFILE_STAT+250);
	SetMenuItemInfo(GetSubMenu(GetMenu()->m_hMenu, 2),
					MENU_STAT_SETMSR1,
					false, 
					&mii);
}

#else

//*********************************************************************************************
void CGUIAppDlg::StatsMenuState(CCPUDetect&	detProcessor)
{
	//default for all processors is to use RDTSC to obtain stats
	SetMenuItemState(MENU_STAT_CLOCKS,true, true);
	SetMenuItemState(MENU_STAT_MSR0,false, false);
	SetMenuItemState(MENU_STAT_MSR1,false, false);
	SetMenuItemState(MENU_STAT_RING0,false, false);
	SetMenuItemState(MENU_STAT_RING3,false, false);
	SetMenuItemState(MENU_STAT_SETMSR0,false, false);
	SetMenuItemState(MENU_STAT_SETMSR1,false, false);
}

#endif

//*********************************************************************************************
void CGUIAppDlg::OnStatClocks()
{
#if VER_TIMING_STATS
	gbUseRDTSC = true;	
	SetMenuItemState(MENU_STAT_CLOCKS,true, true);

	if (gbMSRProfile)
	{
		SetMenuItemState(MENU_STAT_MSR0,false, gbMSRProfile);
		SetMenuItemState(MENU_STAT_MSR1,false, gbMSRProfile);
		SetMenuItemState(MENU_STAT_RING0,bPSRing0(), gbMSRProfile);
		SetMenuItemState(MENU_STAT_RING3,bPSRing3(), gbMSRProfile);
		SetMenuItemState(MENU_STAT_SETMSR0,false, gbMSRProfile);
		SetMenuItemState(MENU_STAT_SETMSR1,false, gbMSRProfile);
	}

	if (gbUseRDTSC)
	{
		csProfileName = "Profile - Time";
		gbNormalizeStats = true;
		conProfile.SetWindowText(csProfileName);
	}

#endif
}


//*********************************************************************************************
void CGUIAppDlg::OnStatMSR0()
{
#if VER_TIMING_STATS
	if (gbMSRProfile)
	{
		gbUseRDTSC = false;	
		gu4MSRProfileTimerSelect = 0;
		SetMenuItemState(MENU_STAT_CLOCKS,false, true);
		SetMenuItemState(MENU_STAT_MSR0,true, gbMSRProfile);
		SetMenuItemState(MENU_STAT_MSR1,false, gbMSRProfile);
		SetMenuItemState(MENU_STAT_RING0,bPSRing0(), gbMSRProfile);
		SetMenuItemState(MENU_STAT_RING3,bPSRing3(), gbMSRProfile);
		SetMenuItemState(MENU_STAT_SETMSR0,false, gbMSRProfile);
		SetMenuItemState(MENU_STAT_SETMSR1,false, gbMSRProfile);
	}

	if ((!gbUseRDTSC) && (gu4MSRProfileTimerSelect == 0))
	{
		csProfileName = "Profile - Event Counter 0 - ";
		gbNormalizeStats = false;
		csProfileName += strPSGetMenuText(0);
		conProfile.SetWindowText(csProfileName);
	}

#endif
}


//*********************************************************************************************
void CGUIAppDlg::OnStatMSR1()
{
#if VER_TIMING_STATS
	if (gbMSRProfile)
	{
		gbUseRDTSC = false;	
		gu4MSRProfileTimerSelect = 1;
		SetMenuItemState(MENU_STAT_CLOCKS,false, true);
		SetMenuItemState(MENU_STAT_MSR0,false, gbMSRProfile);
		SetMenuItemState(MENU_STAT_MSR1,true, gbMSRProfile);
		SetMenuItemState(MENU_STAT_RING0,bPSRing0(), gbMSRProfile);
		SetMenuItemState(MENU_STAT_RING3,bPSRing3(), gbMSRProfile);
		SetMenuItemState(MENU_STAT_SETMSR0,false, gbMSRProfile);
		SetMenuItemState(MENU_STAT_SETMSR1,false, gbMSRProfile);
	}

	if ((!gbUseRDTSC) && (gu4MSRProfileTimerSelect == 1))
	{
		csProfileName = "Profile - Event Counter 1 - ";
		gbNormalizeStats = false;
		csProfileName += strPSGetMenuText(1);
		conProfile.SetWindowText(csProfileName);
	}
#endif
}

//*********************************************************************************************
void CGUIAppDlg::OnStatRing3()
{
#if VER_TIMING_STATS
	if (gbMSRProfile)
	{
		PSRing3(!bPSRing3());
		SetMenuItemState(MENU_STAT_RING3,bPSRing3(), gbMSRProfile);
	}
#endif
}


//*********************************************************************************************
void CGUIAppDlg::OnStatRing0()
{
#if VER_TIMING_STATS
	if (gbMSRProfile)
	{
		PSRing0(!bPSRing0());
		SetMenuItemState(MENU_STAT_RING0,bPSRing0(), gbMSRProfile);
	}
#endif
}


//*********************************************************************************************
void CGUIAppDlg::OnChangeStatCounter0(UINT uID)
{
#if VER_TIMING_STATS
	uID -= MENU_PROFILE_STAT;
	PSMenu0Click(uID);

	if ((!gbUseRDTSC) && (gu4MSRProfileTimerSelect == 0))
	{
		csProfileName = "Profile - Event Counter 0 - ";
		csProfileName += strPSGetMenuText(0);
		conProfile.SetWindowText(csProfileName);
	}

#endif
}


//*********************************************************************************************
void CGUIAppDlg::OnChangeStatCounter1(UINT uID)
{
#if VER_TIMING_STATS
	uID -= MENU_PROFILE_STAT;
	uID -= 250;
	PSMenu1Click(uID);

	if ((!gbUseRDTSC) && (gu4MSRProfileTimerSelect == 1))
	{
		csProfileName = "Profile - Event Counter 1 - ";
		csProfileName += strPSGetMenuText(1);
		conProfile.SetWindowText(csProfileName);
	}

#endif
}




//*********************************************************************************************
void CGUIAppDlg::MoveCameraToInitPosition()
{
	//
	// Move the camera to the location specified by the command line.
	//
	pcamGetCamera()->Move(CPlacement3<>(CRotate3<>(), v3InitCamPos));
}


//*********************************************************************************************
//
void CGUIAppDlg::OnSize
(
	UINT nType,
	int  cx,
	int cy
)
//
// Handles an WM_SIZE message. Resizes the main screen and raster.
//
//**************************************
{
	d3dDriver.Purge();

	if (prnshMain == 0 || prnshMain->iIgnoreWinCommands || bExiting)
		return;

	if (padAudioDaemon)
		padAudioDaemon->RemoveSubtitle();

	CDialog::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED)
	{
	}
	else
	{
		// Resize the direct draw surfaces.
		if (bGetInitFlag(FALSE))
		{
			int i_width;
			int i_height;

			// Find the video mode matching the requested mode.
			bGetDimensions(i_width, i_height);
			Video::SetToValidMode(i_width, i_height);
			
			int i_id;
			for (i_id = 0; i_id <= Video::iModes; ++i_id)
			{
				if (Video::ascrmdList[i_id].iW    == i_width &&
					Video::ascrmdList[i_id].iH    == i_height &&
					Video::ascrmdList[i_id].iBits == 16)
					break;
			}
			Assert(i_id <= Video::iModes);

			CMessageNewRaster msgnewr
			(
				i_id,
				bGetSystemMem(),
				true
			);
			msgnewr.Dispatch();
		}
		else
		{
			CMessageNewRaster msgnewr(-1, bSystemMem, true);
			msgnewr.Dispatch();
		}
		SetMenuState();
	}
}


//*********************************************************************************************
//
void CGUIAppDlg::OnLButtonDown
(
	UINT   nFlags,
	CPoint point
)
//
// Handles the WM_LBUTTONDOWN message by setting the capture mode for left mouse button
// capture.
//
//************************************** 
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
	{
		//Fire();
	}
	else
	{
		// Use the left mouse button to turn the GDI mode off.
		if (bGDIMode)
			SetGDIMode(false);

		// Transform coordinates to window-relative positions.
		AdjustMousePos(point);

		// Cache coordinates.
		pntCaptureMouse   = point;
		pntCaptureCurrent = point;

	/*
		// THIS MESSAGE WAS JUST USED FOR WATER DEBUGGING.
		// Don't do anything unless in debug mode.
		if (!gmlGameLoop.bDebug())
		{
			ptr<CCamera> pcam = CWDbQueryActiveCamera().tGet();

			// Send this message to any interested entities.
			CMessageInput msginput
			(
				VK_LBUTTON,
				pcam->campropGetProperties().vpViewport.vcVirtualX(point.x),
				pcam->campropGetProperties().vpViewport.vcVirtualY(point.y)
			);
			msginput.Dispatch();
			return;
		}
	*/

		// Don't do anything if a capture is already being performed.
		if (bLeftCapture || bRightCapture)
			return;

		bDragRect = !!(GetAsyncKeyState(VK_CONTROL) & 0xFFFE) && (pipeMain.iSelectedCount() == 0);

		if (!bDragRect)
		{
			bool b_augment = GetAsyncKeyState(VK_CONTROL) & 0xFFFE;
			pipeMain.bSelect(point.x, point.y, b_augment);

			// Keep the AI up to date.
			if (pipeMain.ppartLastSelected())
			{
				CInstance* pins = ptCast<CInstance>(pipeMain.ppartLastSelected());
				if (pins && pins->paniGetOwner())
					gaiSystem.pinsSelected = (CInstance*)pins->paniGetOwner();
			}
		}

		// Start capture.
		bLeftCapture = true;

		SetCapture();
		if (bIsFullScreen)
			ShowCursor(FALSE);

		// Redraw the window.
		Invalidate();
	}

	// Call base class function.
	CDialog::OnLButtonDown(nFlags, point);
}


//*********************************************************************************************
//
void CGUIAppDlg::OnRButtonDown
(
	UINT   nFlags,
	CPoint point
)
//
// Handles the WM_RBUTTONDOWN message by setting the right_capture flag.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!gmlGameLoop.bDebug())
		return;

	// Don't do anything if a capture is already being performed.
	if (bLeftCapture || bRightCapture)
		return;

	// Transform coordinates to window-relative positions.
	AdjustMousePos(point);

	// Cache coordinates.
	pntCaptureMouse   = point;
	pntCaptureCurrent = point;

	bool b_augment = GetAsyncKeyState(VK_CONTROL) & 0xFFFE;
	pipeMain.bSelect(point.x, point.y, b_augment);

	// Start capture.
	bRightCapture = true;

	SetCapture();
	if (bIsFullScreen)
		ShowCursor(FALSE);

	// Redraw the window.
	Invalidate();
	
	// Call base class function.
	CDialog::OnRButtonDown(nFlags, point);
}


//*********************************************************************************************
//
void CGUIAppDlg::OnLButtonUp
(
	UINT nFlags,
	CPoint point
)
//
// Handles the WM_LBUTTONUP message by releasing the capture.
//
//************************************** 
{
	bDragRect = false;

	// End capture.
	if (bLeftCapture)
	{
		bLeftCapture = false;
		ReleaseMouseCapture();

		if (pphSystem->bShowBones)
			// Redraw with bones, which were temporarily disabled.
			Invalidate();
	}
	
	// Call base class function.
	CDialog::OnLButtonUp(nFlags, point);
}


//*********************************************************************************************
//
void CGUIAppDlg::OnRButtonUp
(
	UINT nFlags,
	CPoint point
)
//
// Handles the WM_RBUTTONUP message by releasing the capture.
//
//************************************** 
{
	// End capture.
	if (bRightCapture)
	{
		bRightCapture = false;
		ReleaseMouseCapture();
		if (pphSystem->bShowBones)
			// Redraw with bones, which were temporarily disabled.
			Invalidate();
	}
	
	// Call base class function.
	CDialog::OnRButtonUp(nFlags, point);
}


//*********************************************************************************************
//
void CGUIAppDlg::ReleaseMouseCapture()
//
// Releases the mouse from capture mode.
//
//**************************************
{
	// Make sure capture flags have been turned off.
	if (bLeftCapture || bRightCapture)
	{
		return;
	}

	// Release message capture.
	ReleaseCapture();
/*

	//
	// Move the mouse to a position reflecting its use.
	//

	int   i_x = 0;
	int   i_y = 0;
	RECT  rect;	// Window position.

	// Get the window position.
	GetWindowRect(&rect);
	if (!prnshMain->bIsFullScreen)
	{
		rect.top  += GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION) +
					 GetSystemMetrics(SM_CYSIZEFRAME);
		rect.left += GetSystemMetrics(SM_CXSIZEFRAME);
	}

	// Move the cursor position depending on movement mode.
	switch (egiuMode)
	{
		case egiuCAMERA:
			// Position in the centre of the camera.
			i_x = prasMainScreen->iWidth / 2;
			i_y = prasMainScreen->iHeight / 2;
			break;

		case egiuOBJECT:
			// Position over the object crosshair.
			pipeMain.bGetCentreofSelectedObject(i_x, i_y);
			break;
	}

	// Adjust position to be relative to the edge of the main window.
	i_x += rect.left;
	i_y += rect.top;

	if (pipeMain.pinsSelectedShape)
		// Move the cursor.
		SetCursorPos(i_x, i_y);

	//ClipCursor(NULL);
*/
	
	if (bIsFullScreen)
		ShowCursor(TRUE);
}


//*********************************************************************************************
//
void CGUIAppDlg::OnMouseMove
(
	UINT   nFlags,
	CPoint point
)
//
// Handles the WM_MOUSEMOVE message by manipulating the object or camera.
//
//************************************** 
{
	int i_screen_width;
	int i_screen_height;

	// Don't do anything if no capture is being performed.
	if (!bLeftCapture && !bRightCapture)
		return;

	// Transform coordinates to window-relative positions.
	AdjustMousePos(point);

	//
	// Get the mouse positions based on unit screen measurements.
	//
	if (bDragRect)
	{
		pipeMain.Select(pntCaptureMouse, pntCaptureCurrent);
	}
	else
	{
		GetWindowSize(m_hWnd, i_screen_width, i_screen_height);
		float f_mouse_x = (float)(point.x - pntCaptureCurrent.x) / (float)i_screen_width;
		float f_mouse_y = (float)(point.y - pntCaptureCurrent.y) / (float)i_screen_height;

		// Move the camera or an object.
		if (pipeMain.iSelectedCount())
			MoveObjects(f_mouse_x, f_mouse_y);
		else
			MoveCamera(f_mouse_x, f_mouse_y);
	}

	pntCaptureCurrent = point;

	// Flag to disable bones, etc while moving.
	bJustMoved = true;

	// Redraw the window.
	Invalidate();

	// Call base class function.
	CDialog::OnMouseMove(nFlags, point);
}


//*********************************************************************************************
//
void CGUIAppDlg::OnLButtonDblClk
(
	UINT   nFlags,
	CPoint point
) 
//
// Handles the WM_LMOUSEDBLCLK message by editing the object or camera.
//
//************************************** 
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	//
	// End capture.
	//
	if (bLeftCapture || bRightCapture)
	{
		ReleaseCapture();
		//ClipCursor(NULL);
		if (bIsFullScreen)
			ShowCursor(TRUE);
	}
	bLeftCapture = false;
	bRightCapture = false;

	if (nFlags & MK_SHIFT)
	{
		OnSoundMaterialProp();
	}
	else
	{
		// Edit object.
		EditObject();
	}

	// Call base class member function.
	CDialog::OnLButtonDblClk(nFlags, point);
}


//*********************************************************************************************
bool CGUIAppDlg::bSetScreenMode(int i_mode, bool b_system_mem, bool b_force) 
{
	if (padAudioDaemon)
		padAudioDaemon->RemoveSubtitle();

	if (bWithin(i_mode, -1, Video::iModes-1))
	{
		if (!b_force && iScreenMode == i_mode && bSystemMem == b_system_mem)
			return true;

		iScreenMode = i_mode;

		int i_width;
		int i_height;
		int i_bitdepth;

		if (i_mode == -1)
		{
			if (bIsFullScreen)
			{
				// Set window flags to windowed mode.
				SetWindowMode(m_hWnd, false);
				// Move the window to stored positions.
				MoveWindow(&rectWindowedPos, TRUE);
				bIsFullScreen = false;
			}

			//
			// Get the client area of the main window if in windowed mode.
			//
			GetWindowSize(m_hWnd, i_width, i_height);
			i_bitdepth = 0;
		}
		else
		{
			i_width    = Video::ascrmdList[i_mode].iW;
			i_height   = Video::ascrmdList[i_mode].iH;
			i_bitdepth = Video::ascrmdList[i_mode].iBits;

			//
			// Set window mode and store window position only if the application was
			// previously in windowed mode.
			//
			if (!bIsFullScreen)
			{
				GetWindowRect(&rectWindowedPos);
				SetWindowMode(m_hWnd, true);
				bIsFullScreen = true;
				ShowToolbar(FALSE);
			}
		}

		// Create the raster.
		Assert(prnshMain);
		prnshMain->bCreateScreen(i_width, i_height, i_bitdepth, b_system_mem);

		SetGDIMode(false);

		// Get the background colour.
		clrBackground = prenMain->pSettings->clrBackground;
		bSystemMem = !prasMainScreen->bVideoMem;

		// Return flag indicating command was successful.
		lbAlphaConstant.Setup(prasMainScreen.ptPtrRaw());
		lbAlphaTerrain.CreateBlend(prasMainScreen.ptPtrRaw(), clrDefEndDepth);
		lbAlphaWater.CreateBlendForWater(prasMainScreen.ptPtrRaw());
		abAlphaTexture.Setup(prasMainScreen.ptPtrRaw());
		return true;
	}
	return false;
}


//*********************************************************************************************
bool CGUIAppDlg::bSetRenderer(uint u_id) 
{
	int i_mode_id = (int)u_id - MENU_DRIVER_FIRST;	// Screen mode ID.

	return prnshMain->bChangeRenderer(i_mode_id);
}


//*********************************************************************************************
void CGUIAppDlg::SetMenuItemState(int i_id, bool b_checked, bool b_enabled)
{
	CMenu* pmenu = GetMenu();
	Assert(pmenu);

	pmenu->EnableMenuItem(i_id, MF_BYCOMMAND | (b_enabled ? MF_ENABLED : MF_GRAYED));
	pmenu->CheckMenuItem(i_id, MF_BYCOMMAND | (b_checked ? MF_CHECKED : MF_UNCHECKED));
}


//*********************************************************************************************
void CGUIAppDlg::SetRenderFeatureState(int i_id, ERenderFeature erf)
{
	if (prenMain)
		SetMenuItemState(i_id, prenMain->pSettings->seterfState[erf], prenMain->pScreenRender->seterfModify()[erf]);
}

//*********************************************************************************************
static UINT AFXAPI AfxGetFileName(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax)
{
	ASSERT(lpszTitle == NULL ||
		AfxIsValidAddress(lpszTitle, _MAX_FNAME));
	ASSERT(AfxIsValidString(lpszPathName, FALSE));

	// always capture the complete file name including extension (if present)
	LPTSTR lpszTemp = (LPTSTR)lpszPathName;
	for (LPCTSTR lpsz = lpszPathName; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
	{
		// remember last directory/drive separator
		if (*lpsz == '\\' || *lpsz == '/' || *lpsz == ':')
			lpszTemp = (LPTSTR)_tcsinc(lpsz);
	}

	// lpszTitle can be NULL which just returns the number of bytes
	if (lpszTitle == NULL)
		return lstrlen(lpszTemp)+1;

	// otherwise copy it into the buffer provided
	lstrcpyn(lpszTitle, lpszTemp, nMax);
	return 0;
}

//*********************************************************************************************
static void AbbreviateName(LPTSTR lpszCanon, int cchMax, BOOL bAtLeastName)
{
	int cchFullPath, cchFileName, cchVolName;
	const TCHAR* lpszCur;
	const TCHAR* lpszBase;
	const TCHAR* lpszFileName;

	lpszBase = lpszCanon;
	cchFullPath = lstrlen(lpszCanon);

	cchFileName = AfxGetFileName(lpszCanon, NULL, 0) - 1;
	lpszFileName = lpszBase + (cchFullPath-cchFileName);

	// If cchMax is more than enough to hold the full path name, we're done.
	// This is probably a pretty common case, so we'll put it first.
	if (cchMax >= cchFullPath)
		return;

	// If cchMax isn't enough to hold at least the basename, we're done
	if (cchMax < cchFileName)
	{
		lstrcpy(lpszCanon, (bAtLeastName) ? lpszFileName : "");
		return;
	}

	// Calculate the length of the volume name.  Normally, this is two characters
	// (e.g., "C:", "D:", etc.), but for a UNC name, it could be more (e.g.,
	// "\\server\share").
	//
	// If cchMax isn't enough to hold at least <volume_name>\...\<base_name>, the
	// result is the base filename.

	lpszCur = lpszBase + 2;                 // Skip "C:" or leading "\\"

	if (lpszBase[0] == '\\' && lpszBase[1] == '\\') // UNC pathname
	{
		// First skip to the '\' between the server name and the share name,
		while (*lpszCur != '\\')
		{
			lpszCur = _tcsinc(lpszCur);
			ASSERT(*lpszCur != '\0');
		}
	}
	// if a UNC get the share name, if a drive get at least one directory
	ASSERT(*lpszCur == '\\');
	// make sure there is another directory, not just c:\filename.ext
	if (cchFullPath - cchFileName > 3)
	{
		lpszCur = _tcsinc(lpszCur);
		while (*lpszCur != '\\')
		{
			lpszCur = _tcsinc(lpszCur);
			ASSERT(*lpszCur != '\0');
		}
	}
	ASSERT(*lpszCur == '\\');

	cchVolName = lpszCur - lpszBase;
	if (cchMax < cchVolName + 5 + cchFileName)
	{
		lstrcpy(lpszCanon, lpszFileName);
		return;
	}

	// Now loop through the remaining directory components until something
	// of the form <volume_name>\...\<one_or_more_dirs>\<base_name> fits.
	//
	// Assert that the whole filename doesn't fit -- this should have been
	// handled earlier.

	ASSERT(cchVolName + (int)lstrlen(lpszCur) > cchMax);
	while (cchVolName + 4 + (int)lstrlen(lpszCur) > cchMax)
	{
		do
		{
			lpszCur = _tcsinc(lpszCur);
			ASSERT(*lpszCur != '\0');
		}
		while (*lpszCur != '\\');
	}

	// Form the resultant string and we're done.
	lpszCanon[cchVolName] = '\0';
	lstrcat(lpszCanon, _T("\\..."));
	lstrcat(lpszCanon, lpszCur);
}


//*********************************************************************************************
//
void CGUIAppDlg::UpdateRecentFiles()
//
// Update the recent file list.
//
//**************************************
{
	CMenu* pmenu = GetMenu();

	if (pmenu == 0)
		return;

	//
	// Setup the recent file list.
	//
	int i;
	for (i = 0; i <= 5; i++)
		pmenu->RemoveMenu(MENU_OPEN_LAST+i, MF_BYCOMMAND);

	for (i = 0; i < 5; i++)
	{
		char str_key[32];
		char str_name[512];
		sprintf(str_key, "LastFileOpened%d", i);
		GetRegString(str_key, str_name, sizeof(str_name), "");

		if (strlen(str_name))
		{
			AbbreviateName(str_name, 32, TRUE);
			pmenu->InsertMenu(MENU_RESET, MF_BYCOMMAND | MF_STRING, MENU_OPEN_LAST+i, str_name);
		}
	}

	// Insert seperator.
	pmenu->InsertMenu(MENU_RESET, MF_BYCOMMAND | MF_SEPARATOR, MENU_OPEN_LAST+i);

	// Re-draw the menu.
	DrawMenuBar();
}


//*********************************************************************************************
//
void CGUIAppDlg::AddRecentFile(char *str_name)
//
// Add a file to the recent file list.
//
//**************************************
{
	char str_last_key[32];
	char str_key[32];
	char str_filename[512];

	sprintf(str_last_key, "LastFileOpened%d", 0);

	// Push items down.
	for (int i = 1; i < 5; i++)
	{
		sprintf(str_key, "LastFileOpened%d", i);

		GetRegString(str_key, str_filename, sizeof(str_filename), "");
		SetRegString(str_last_key, str_filename);

		strcpy(str_last_key, str_key);
	}

	// Add this item.
	SetRegString(str_key, str_name);

	UpdateRecentFiles();
}


//*********************************************************************************************
void CGUIAppDlg::ShowLoadedFile(char *str_name)
{
	CString cstr_title;

	// Get current title.
	GetWindowText(cstr_title);

	// Append file name (file part only).
	int i_index = 0;
	for (int i = 0; str_name[i] != '\0'; i++)
		if (str_name[i] == '\\' || str_name[i] == '/' || str_name[i] == ':')
			i_index = i+1;

	cstr_title += " - ";
	cstr_title += (str_name + i_index);

	// Set title.
	SetWindowText(cstr_title);
}


//*********************************************************************************************
void CGUIAppDlg::SetMenuState()
{
	//
	// Set the presorting menu items.
	//
	if (prenMain)
	{
		SetMenuItemState(MENU_DEPTHSORT,    prenMain->pSettings->esSortMethod == esDepthSort,          true);
		SetMenuItemState(MENU_PRESORT_NONE, prenMain->pSettings->esSortMethod == esNone,               true);
		SetMenuItemState(MENU_PRESORT_FTOB, prenMain->pSettings->esSortMethod == esPresortFrontToBack, true);
		SetMenuItemState(MENU_PRESORT_BTF,  prenMain->pSettings->esSortMethod == esPresortBackToFront, true);
		SetMenuItemState(MENU_SHADOWS,		prenMain->pSettings->bShadow, true);
		SetMenuItemState(MENU_DETAIL,		prenMain->pSettings->bDetailReduce);
		SetMenuItemState(MENU_DOUBLEV,		prenMain->pSettings->bDoubleVertical);
		SetMenuItemState(MENU_HALFSCAN,		prenMain->pSettings->bHalfScanlines);
		SetMenuItemState(MENU_SKY_DISABLE,	prenMain->pSettings->bDrawSky);
	}
	SetMenuItemState(MENU_CACHE_INTERSECT,       rcsRenderCacheSettings.bAddIntersectingObjects);
	SetMenuItemState(MENU_SHADOW_TERRAIN,        NMultiResolution::CTextureNode::bEnableShadows);
	SetMenuItemState(MENU_SHADOW_TERRAIN_MOVING, NMultiResolution::CTextureNode::bEnableMovingShadows);
	SetMenuItemState(MENU_PRELOAD,               gmlGameLoop.bPreload);

	SetMenuItemState(MENU_FREEZECACHES, rcsRenderCacheSettings.bFreezeCaches);

	//
	// Set the game control menu items.
	//
	SetMenuItemState(MENU_PLAY,  gmlGameLoop.egmGameMode == egmPLAY);
	SetMenuItemState(MENU_DEBUG, gmlGameLoop.egmGameMode == egmDEBUG);
	SetMenuItemState(MENU_PAUSE, gmlGameLoop.bPauseGame, gmlGameLoop.egmGameMode == egmPLAY);

	SetMenuItemState(MENU_AI, gaiSystem.bActive);
	SetMenuItemState(MENU_PHYSICS, pphSystem->bActive);

	SetMenuItemState(MENU_ZONLYEDIT, bZOnlyEdit);

	// Direct3D menu.
	SetMenuItemState(MENU_ACCELERATION, d3dDriver.bUseD3D(), d3dDriver.bInitEnabled());

	// Set the view states.
	SetMenuItemState(MENU_CONSOLE,		 bShowConsole);
	SetMenuItemState(MENU_STATS,		 bShowStats);
	SetMenuItemState(MENU_HARDWARE_STATS, bShowHardwareStats);
	SetMenuItemState(MENU_AVG_STATS,	 bAvgStats);
	SetMenuItemState(MENU_FPS,			 bShowFPS);
	SetMenuItemState(MENU_CACHE_STATS,	 rcstCacheStats.bKeepStats);
	SetMenuItemState(MENU_VIEW_SPHERES,	 bShowSpheres);
	SetMenuItemState(MENU_VIEW_WIRE,	 bShowWire);
	SetMenuItemState(MENU_VIEW_PINHEAD,	 bShowPinhead);
	SetMenuItemState(MENU_VIEW_BONES,	 pphSystem->bShowBones);
	SetMenuItemState(MENU_VIEW_QUADS,	 NMultiResolution::CTextureNode::bOutlineNodes);
	SetMenuItemState(MENU_VIEW_TRIGGERS, CRenderContext::bRenderTriggers);

	// Set the crosshair states.
	SetMenuItemState(MENU_VIEW_HAIRS,	bShowHairs);
	SetMenuItemState(MENU_STATIC_HANDLES, bStaticHandles);
	SetMenuItemState(MENU_VIEW_CROSSHAIR_RADIUS, bCrosshairRadius);
	SetMenuItemState(MENU_VIEW_CROSSHAIR_VEGETATION, bCrosshairVegetation);
	extern bool bEditTrnObjs;
	SetMenuItemState(MENU_VIEW_CROSSHAIR_TERRAINTEXTURES, bEditTrnObjs);

	// Set the bones sub-states.
	SetMenuItemState(MENU_VIEW_BONES_BOXES,		CPhysicsInfo::setedfMain[edfBOXES]);
	SetMenuItemState(MENU_VIEW_BONES_PHYSICS,	CPhysicsInfo::setedfMain[edfBOXES_PHYSICS]);
	SetMenuItemState(MENU_VIEW_BONES_WAKE,		CPhysicsInfo::setedfMain[edfBOXES_WAKE]);
	SetMenuItemState(MENU_VIEW_BONES_QUERY,		CPhysicsInfo::setedfMain[edfBOXES_QUERY]);
	SetMenuItemState(MENU_VIEW_BONES_MAGNETS,	CPhysicsInfo::setedfMain[edfMAGNETS]);
	SetMenuItemState(MENU_VIEW_BONES_SKELETONS,	CPhysicsInfo::setedfMain[edfSKELETONS]);
	SetMenuItemState(MENU_VIEW_BONES_ATTACH,	CPhysicsInfo::setedfMain[edfATTACHMENTS]);
	SetMenuItemState(MENU_VIEW_BONES_WATER,		CPhysicsInfo::setedfMain[edfWATER]);
	SetMenuItemState(MENU_VIEW_BONES_RAYCAST,	CPhysicsInfo::setedfMain[edfRAYCASTS]);

	// Set the camera state.
	CInstance* pins_attach = pcamGetCamera()->pinsAttached();

	SetMenuItemState(MENU_CAMERAPLAYER,			ptCast<CPlayer>(pins_attach) != 0);
	SetMenuItemState(MENU_CAMERASELECTED,		pins_attach && !ptCast<CPlayer>(pins_attach) && !pcamGetCamera()->bHead());
	SetMenuItemState(MENU_CAMERASELECTEDHEAD,	pins_attach && !ptCast<CPlayer>(pins_attach) && pcamGetCamera()->bHead());
	SetMenuItemState(MENU_CAMERAFREE,			!pins_attach);
	SetMenuItemState(MENU_CAMERA2M,				bCameraFloating);
	SetMenuItemState(MENU_ROTATE_WORLDZ,		bRotateAboutWorldZ);

	// Set the physics player state.
	SetMenuItemState(MENU_PLAYERPHYSICS, gpPlayer && gpPlayer->bPhysics);
	SetMenuItemState(MENU_PLAYERINVULNERABLE, gpPlayer->bInvulnerable);

	SetMenuItemState(MENU_PHYSICS_SLEEP,	pphSystem->bAllowSleep);
	SetMenuItemState(MENU_PHYSICS_STEP,		bPhysicsStep);

	//
	// Set the screenmode menu.
	//

	// Set the Windowed menu.
	SetMenuItemState(MENU_WINDOW, !bIsFullScreen);
	SetMenuItemState(MENU_SYSTEM_MEM, bSystemMem);

	// Set Rendering options menu.
	SetRenderFeatureState(MENU_SCREENCLIP,	erfRASTER_CLIP);
	SetRenderFeatureState(MENU_SCREENCULL,	erfRASTER_CULL);
	SetRenderFeatureState(MENU_ZBUFFER,		erfZ_BUFFER);

	SetRenderFeatureState(MENU_LIGHT,		erfLIGHT);
	SetRenderFeatureState(MENU_LIGHT_SHADE,	erfLIGHT_SHADE);
	SetRenderFeatureState(MENU_FOG,			erfFOG);
	SetRenderFeatureState(MENU_FOG_SHADE,	erfFOG_SHADE);
	SetRenderFeatureState(MENU_SPECULAR,	erfSPECULAR);
	SetRenderFeatureState(MENU_COLOURED,	erfCOLOURED_LIGHTS);

	SetRenderFeatureState(MENU_TEXTURE,		erfTEXTURE);
	SetRenderFeatureState(MENU_TRANSPARENT,	erfTRANSPARENT);
	SetRenderFeatureState(MENU_BUMP,		erfBUMP);
	SetRenderFeatureState(MENU_WIREFRAME,	erfWIRE);
	
	SetRenderFeatureState(MENU_ALPHA_COLOUR,  erfALPHA_COLOUR);

	SetRenderFeatureState(MENU_TRAPEZOIDS,	erfTRAPEZOIDS);
	SetRenderFeatureState(MENU_SUBPIXEL,	erfSUBPIXEL);
	SetRenderFeatureState(MENU_PERSPECTIVE,	erfPERSPECTIVE);
	SetRenderFeatureState(MENU_MIPMAP,		erfMIPMAP);
	SetRenderFeatureState(MENU_DITHER,		erfDITHER);
	SetRenderFeatureState(MENU_FILTER,		erfFILTER);
	SetRenderFeatureState(MENU_FILTER_EDGES,erfFILTER_EDGES);
	SetMenuItemState(MENU_FILTER_CACHES, d3dDriver.bFilterImageCaches());
	SetRenderFeatureState(MENU_RENDER_CACHE, erfCOPY);
	
	SetMenuItemState(MENU_WATER_ALPHA,		CEntityWater::bAlpha);
	SetMenuItemState(MENU_WATER_INTERPOLATE,	CEntityWater::bInterp);

	// Set the render cache menus.
	switch (rcsRenderCacheSettings.erctMode)
	{
		case ercmCACHE_OFF:
			SetMenuItemState(MENU_RENDER_CACHE, false);
			SetMenuItemState(MENU_RENDER_CACHE_TEST, false);
			break;
		case ercmCACHE_ON:
			SetMenuItemState(MENU_RENDER_CACHE, true);
			SetMenuItemState(MENU_RENDER_CACHE_TEST, false);
			break;
		default:
			Assert(0);
	}

	// Image cache state.
	SetMenuItemState(MENU_CONVEXCACHES,      rcsRenderCacheSettings.bUseConvexPolygons);
	SetMenuItemState(MENU_FORCEINTERSECTING, rcsRenderCacheSettings.bForceIntersecting);

	// Image cache border.
	SetMenuItemState(MENU_FASTMODE, bFastMode);

	// Occlusion state.
	SetMenuItemState(MENU_OCCLUDE_OBJECTS,        COcclude::bUseObjectOcclusion);
	SetMenuItemState(MENU_OCCLUDE_POLYGONS,       COcclude::bUsePolygonOcclusion);
	SetMenuItemState(MENU_OCCLUDE_OCCLUDE,        COcclude::bRemoveOccluded);
	SetMenuItemState(MENU_OCCLUDE_CAMERAVIEWTEST, COcclude::bTestCameraView);

	// Scheduler state.
	SetMenuItemState(MENU_SCHEDULER_USE,          CScheduler::bUseScheduler);

	// Delaunay test code.
	SetMenuItemState(MENU_DELAUNAYTEST, bDelaunayTest);

	// Terrain test code.
	SetMenuItemState(MENU_TERRAINTEST, bTerrainTest);

	// Console windows.
	SetMenuItemState(MENU_CONPHYSICS, conPhysics.bIsActive());
	SetMenuItemState(MENU_DRAWPHYSICS, pdldrPhysics && pdldrPhysics->bIsVisible());
	SetMenuItemState(MENU_AI, gaiSystem.bShow3DInfluences);
	SetMenuItemState(MENU_ARTSTATS, conArtStats.bIsActive());
	SetMenuItemState(MENU_PREDICTMOVEMENT, conPredictMovement.bIsActive());

	// Mipmap submenu state.
	SetMenuItemState(MENU_MIPMAP_NORMAL,     false);
	SetMenuItemState(MENU_MIPMAP_SMALLEST,   false);
	SetMenuItemState(MENU_MIPMAP_NO_LARGEST, false);
	switch (CTexture::emuMipUse)
	{
		case emuNORMAL:
			SetMenuItemState(MENU_MIPMAP_NORMAL,     true);
			break;
		case emuSMALLEST:
			SetMenuItemState(MENU_MIPMAP_SMALLEST,   true);
			break;
		case emuNO_LARGEST:
			SetMenuItemState(MENU_MIPMAP_NO_LARGEST, true);
			break;
		default:
			Assert(0);
	}

	// Terrain flags.
	CWDbQueryTerrain wqtrr;

	SetMenuItemState(ID_EDIT_TERRAIN_TERRAINSOUNDS, bTerrainSound);

	switch (gpInputDeemone->ecmGetControlMethod())
	{
	case	ecm_DefaultControls:
		SetMenuItemState(MENU_CONTROLS_DEFAULTCONTROLS,TRUE);
		SetMenuItemState(MENU_CONTROLS_STANDARDJOYSTICK,FALSE);
		break;

	case ecm_Joystick:
		SetMenuItemState(MENU_CONTROLS_DEFAULTCONTROLS,FALSE);
		SetMenuItemState(MENU_CONTROLS_STANDARDJOYSTICK,TRUE);
		break;
	}

	if (gpskyRender==NULL)
	{
		SetMenuItemState(MENU_SKY_TEXTURE,	true);
		SetMenuItemState(MENU_SKY_FILL,		false);
	}
	else
	{
		SetMenuItemState(MENU_SKY_TEXTURE,	gpskyRender->bSkyTextured());
		SetMenuItemState(MENU_SKY_FILL,		gpskyRender->bSkyFill());
	}

	// Set the automatic scene save state.
	SetMenuItemState(MENU_ASSERTSAVE, bGetAutoSave());

	SetMenuItemState(MENU_IGNORE_MSG_RECIPIENTS, CMessage::bIgnoreRegisteredRecipients);

	// Redraw the screen.
	Invalidate();
}


#if VER_TIMING_STATS

// Timer for FPS count.
static CCycleTimer ctmrFPS;

#endif

//__int64 i8FrameStart = 0;
//__int64 i8FrameStop = 0;

//*********************************************************************************************
//
void CGUIAppDlg::PaintWindow
(
)
//
// Code to draw a window or surface.
//
//**************************************
{
	CCycleTimer ctmr;

	// Don't paint if the renderer has not yet been set up.
	if (prnshMain == 0)
		return;

	// Enforce GDI shit.
	if (bGDIMode)
	{
		Assert(bIsFullScreen);

		prasMainScreen->FlipToGDISurface();
		DrawMenuBar();
		RedrawWindow(0, 0, RDW_FRAME);
		return;
	}

	if (!prnshMain->bBeginPaint())
		return;

	//prasMainScreen->ClearBorder();
	pipeMain.Paint();

	// Draw dragging rectangle if necessary.
	if (bDragRect)
	{
		CDraw draw(prasMainScreen);

		draw.Style(PS_DOT);
		draw.Colour(CColour(1.0f, 0.0f, 0.0f));

		draw.Line(pntCaptureMouse.x,   pntCaptureMouse.y,   pntCaptureCurrent.x, pntCaptureMouse.y);
		draw.Line(pntCaptureMouse.x,   pntCaptureCurrent.y, pntCaptureCurrent.x, pntCaptureCurrent.y);

		draw.Line(pntCaptureMouse.x,   pntCaptureMouse.y,   pntCaptureMouse.x,   pntCaptureCurrent.y);
		draw.Line(pntCaptureCurrent.x, pntCaptureMouse.y,   pntCaptureCurrent.x, pntCaptureCurrent.y);
	}

	// Draw various object adornments, such as crosshairs and bounding spheres.
	pipeMain.MarkObjects
	(
		prasMainScreen, 
		gmlGameLoop.bDebug() && (bShowHairs || bLeftCapture || bRightCapture),		// Cross-hairs.
		bShowSpheres, 
		bShowWire,
		bShowPinhead
	);

	bJustMoved = false;

	// Add the total time for painting to the frame stat.
	proProfile.psFrame.Add(ctmr(), 1);

#if bDUMP_STATS
	// Finish rendering and flip surfaces.
	prnshMain->EndPaint();

	StatsDump();
#else
	StatsDisplay();

	// Finish rendering and flip surfaces.
	prnshMain->EndPaint();

	// Add additional time to the frame stat.
	proProfile.psFrame.Add(ctmr());
#endif
}

//*********************************************************************************************
//
void CGUIAppDlg::DisplayCacheStats()
//
// Displays render cache stats.
//
//************************************** 
{
	// Do nothing if render cache stats are not being maintained.
	if (!rcstCacheStats.bKeepStats)
		return;

	static CConsoleBuffer con_cache(48, 24);
	con_cache.SetActive(true);

	// Display the number of caches.
	con_cache.Print("Num Caches:         %ld\n", iNumCaches());

	// Display the average number of objects per cache.
	if (iNumCaches() > 0)
	{
		con_cache.Print
		(
			"Avg Objs per Cache: %1.1f\n",
			float(rcstCacheStats.iNumCachedObjects) / float(iNumCaches())
		);

		// Display the number of updated caches.
		con_cache.Print
		(
			"Caches Updated:     %1.1f%%\n",
			float(rcstCacheStats.iNumCachesUpdated) /
			float(iNumCaches()) * 100.0f
		);
		con_cache.Print
		(
			"Avg Age of Caches:  %1.1f\n", 
			float(rcstCacheStats.iTotalCacheAge) / float(iNumCaches())
		);

		// Display the number of euthanized caches.
		con_cache.Print
		(
			"Euthanized Caches:  %ld\n", 
			rcstCacheStats.iNumCachesEuthanized
		);

		// Display the amount of memory associated with caches.
		int i_cachemem = iCacheMemoryUsed();
		if (i_cachemem >= 1024)
			con_cache.Print
			(
				"Cache Memory Used:  %ld kb\n", 
				i_cachemem >> 10
			);
		else
			con_cache.Print
			(
				"Cache Memory Used:  %ld bytes\n", 
				i_cachemem
			);
	}

	// Display stats associated with the cache heap.
	{
		int i_num_nodes;	// Number of memory allocations.
		int i_mem_used;		// Amount of memory allocated.

		// Get the info about the heap used for the cache.
		fxhHeap.GetInfo(i_num_nodes, i_mem_used);

		// Display the number of memory nodes allocated.
		con_cache.Print("Heap Nodes:         %ld\n", i_num_nodes);

		// Display the amount of memory allocated.
		con_cache.Print("Heap Mem (kb):      %ld\n", i_mem_used >> 10);
	}
	
	// Reset render cache statistics.
	rcstCacheStats.Reset();
	prnshMain->ShowConsoleOnScreen(con_cache);
	con_cache.ClearScreen();
}


//*********************************************************************************************
//
void CGUIAppDlg::ShowToolbar
(
	bool b_show
)
//
// Displays the toolbar in windowed mode if the toolbar is not already displayed.
//
//************************************** 
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Show the toolbar only if not in full screen mode.
	if (bIsFullScreen)
		return;

	if (!pdlgToolbar)
	{
		// Create an instance of the toolbar.
		pdlgToolbar = new CTool();
	}

	// Make sure the toolbar isn't already showing.
	pdlgToolbar->DestroyWindow();

	// Show the toolbar if the b_show flag is 'true.'
	if (b_show)
	{
		pdlgToolbar->Create((UINT)IDD_FLOATTOOLS, this);
		pdlgToolbar->ShowWindow(SW_SHOWNORMAL);
	}
}


//*********************************************************************************************
//
void CGUIAppDlg::MoveCamera
(
	float f_mouse_x,
	float f_mouse_y
)
//
// Moves the camera using mouse input.
//
//**************************************
{
	//
	// Get key states.
	//
	bool b_shift_key = (GetAsyncKeyState(VK_SHIFT) & (SHORT)0xFFFE) != 0;

	CPlacement3<> p3_cam = pcamGetCamera()->p3GetPlacement();

	if (bLeftCapture)
	{
		// Create the direction and rotation vectors.
		CVector3<> v3_camdir;

		if (b_shift_key)
			v3_camdir = CVector3<> (2.0f * f_mouse_x, 0.0f, 2.0f * f_mouse_y);
		else
			v3_camdir = CVector3<> (2.0f * f_mouse_x, 2.0f * f_mouse_y, 0.0f);

		// Move the camera in the camera's direction.
		p3_cam.v3Pos += v3_camdir * p3_cam.r3Rot;
	}
	else if (bRightCapture)
	{
		//
		// If the right mouse button is down, perform xz rotations.
		// First rotate around local Z, then local X.
		// Local rotations are performed by pre-concatentating them to the current rotation.
		//

		CRotate3<> r3_cam = 
			CRotate3<>(-d3XAxis, (CAngle)(dDegreesToRadians(90.0f) * f_mouse_y)) *
			CRotate3<>((b_shift_key ? d3YAxis : -d3ZAxis), (CAngle)(dDegreesToRadians(90.0f) * f_mouse_x));

		p3_cam.r3Rot = r3_cam * p3_cam.r3Rot;
	}

	pcamGetCamera()->Move(p3_cam);
}


//*********************************************************************************************
//
void CGUIAppDlg::MoveObjects
(
	float f_mouse_x,
	float f_mouse_y
)
//
// Moves all selected objects' positions or orientations using mouse input.
//
//**************************************
{
	//
	// Get key states.
	//
	bool b_shift_key = (GetAsyncKeyState(VK_SHIFT) & (SHORT)0xFFFE) != 0;
	bool b_alt_key = (GetAsyncKeyState(VK_MENU) & (SHORT)0xFFFE) != 0;

	//
	//	Scale object.
	//
	if (bLeftCapture && b_alt_key)
	{
		// Scale me!  Scales each object separately.
		float f_scale = 1.0f + (f_mouse_x - f_mouse_y) * 0.5f;

		if (f_scale < 0.5)
			f_scale = 0.5f;

		// Apply the rotation to the objects.
		forall (pipeMain.lsppartSelected, TSelectedList, itppart)
		{
			// This is dangerous, as it violates some abstraction barrier.
			CPartition* ppart = (*itppart);
			
			ppart->SetScale(ppart->fGetScale() * f_scale);

			// Move it to ensure that the partitions are current.
			ppart->Move(ppart->pr3Presence());
		}
		return;
	}

	//
	// Translate position or rotate in position.
	//
	if (bLeftCapture)
	{
		CVector3<> v3_objdir;

		if (!b_shift_key)
		{
			//
			// If the shift key is down, perform xy translations.
			//

			// Get the translation vector.
			v3_objdir = CVector3<>(f_mouse_x, -f_mouse_y, 0.0);
		}
		else
		{
			//
			// If the shift key is not down, perform xz translations.
			//

			// Get the translation vector.
			v3_objdir = CVector3<>(f_mouse_x, 0.0, -f_mouse_y);
		}

		// Translate it from camera to world space.
		v3_objdir *= pcamGetCamera()->r3Rot();

		// Scale it by distance from camera.
		v3_objdir *= (pcamGetCamera()->v3Pos() - pipeMain.ppartLastSelected()->v3Pos()).tLen();

		// If in Z Edit only mode, keep only the Z part.
		if (bZOnlyEdit)
		{
			v3_objdir.tX = 0.0f;
			v3_objdir.tY = 0.0f;
		}

		// Apply the vector to the objects.
		forall (pipeMain.lsppartSelected, TSelectedList, itppart)
		{
			// Move the object in the world partitioning structure, and physics system.
			CPlacement3<> p3 = (*itppart)->pr3Presence();
			p3.v3Pos += v3_objdir;
			(*itppart)->Move(p3);
		}
	}
	else
	{
		//
		// If the shift key is down, perform yx rotations, else zx rotations.
		//
		CRotate3<> r3_objrot = CRotate3<>((b_shift_key ? d3YAxis : d3ZAxis), CAngle(dPI * f_mouse_x)) *
							   CRotate3<>(d3XAxis,							 CAngle(dPI * f_mouse_y));

		// Make the rotation camera-relative by transforming it from world to camera space, and back.
		r3_objrot = ~pcamGetCamera()->r3Rot() * r3_objrot * pcamGetCamera()->r3Rot();

		// In Z edit only mode, use only a Z edit.
		if (bZOnlyEdit)
			r3_objrot = CRotate3<>(d3ZAxis, CAngle(dPI * f_mouse_x));

		// Centre the rotation on the last-selected object.
		CPlacement3<> p3_objrot = TransformAt(r3_objrot, pipeMain.ppartLastSelected()->v3Pos());

		// Apply the rotation to the objects.
		forall (pipeMain.lsppartSelected, TSelectedList, itppart)
		{
			// Move the object in the world partitioning structure, and physics system.
			CPlacement3<> p3 = (*itppart)->pr3Presence().p3Placement() * p3_objrot;
			(*itppart)->Move(p3);
		}
	}
}

//*********************************************************************************************
//
void CGUIAppDlg::AdjustMousePos
(
	CPoint& point	// The mouse position relative to the window.
)
//
// Adjusts the mouse coordinates to account for full screen mode. This function only does
// something in full screen mode.
//
//**************************************
{
	// Do something only if in full screen mode.
	if (!bIsFullScreen)
		return;

	// Add the menu bar size if in full screen mode.
	point.y += GetSystemMetrics(SM_CYMENUSIZE);
}

//*********************************************************************************************
void CGUIAppDlg::OpenLightProperties(rptr<CLight> plt)
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Create the light properties dialog box.
	if (pltdlgLightProperties == 0)
	{
		pltdlgLightProperties = new CLightProperties();
	}

	// Destroy any open light properties dialog to ensure the dialog is sized properly.
	pltdlgLightProperties->DestroyWindow();

	// Create and display the light properties dialog.
	pltdlgLightProperties->Create(IDD_LIGHT_PROPERTIES, this);
	pltdlgLightProperties->Show(plt);
}

//*********************************************************************************************
void CGUIAppDlg::OpenCameraProperties()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pcamdlgCameraProperties == 0)
	{
		pcamdlgCameraProperties = new CCameraProperties();
		pcamdlgCameraProperties->Create(IDD_CAMERA_PROPERTIES, this);
	}
	pcamdlgCameraProperties->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OpenObjectDialogProperties(CInstance* pins)
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	Assert(pins);

	if (pdlgobjDialogObject == 0)
	{
		pdlgobjDialogObject = new CDialogObject();
		pdlgobjDialogObject->Create(IDD_OBJECT_PROPERTIES, this);
	}
	pdlgobjDialogObject->SetInstance(pins);
	pdlgobjDialogObject->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OnMaterialProperties()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	CInstance* pins = ptCast<CInstance>(pipeMain.ppartLastSelected());
	if (!pins)
		return;

	if (pdlgDialogMaterial == 0)
	{
		pdlgDialogMaterial = newCDialogMaterial(pins);
		pdlgDialogMaterial->Create(IDD_MATERIAL, this);
	}
	else
		pdlgDialogMaterial = newCDialogMaterial(pins);

	pdlgDialogMaterial->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OpenMagnetProperties(CMagnetPair* pmp)
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	Assert(pmp);

	if (pdlgmagDialogMagnet == 0)
	{
		pdlgmagDialogMagnet = new CDialogMagnet();
		pdlgmagDialogMagnet->Create(IDD_MAGNET, this);
	}
	pdlgmagDialogMagnet->SetMagnet(pmp);
	pdlgmagDialogMagnet->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OnPlayerProperties()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pdlgDialogPlayer == 0)
	{
		pdlgDialogPlayer = newCDialogPlayer();
		pdlgDialogPlayer->Create(IDD_PLAYER, this);
	}
	pdlgDialogPlayer->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OpenPerspectiveDialog()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pdlgPerspectiveSubdivide == 0)
	{
		pdlgPerspectiveSubdivide = new CPerspectiveSubdivideDialog();
		pdlgPerspectiveSubdivide->Create(IDD_PERSPECTIVE, this);
	}
	pdlgPerspectiveSubdivide->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OnPhysicsProperties()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	CInstance* pins = ptCast<CInstance>(pipeMain.ppartLastSelected());

	// Don't do anything if no object is selected.
	if (pins == 0)
		return;

	//
	// End capture.
	//
	if (bLeftCapture || bRightCapture)
	{
		ReleaseCapture();
		//ClipCursor(NULL);
		if (bIsFullScreen)
			ShowCursor(TRUE);
	}
	bLeftCapture = false;
	bRightCapture = false;

	if (pdlgphyDialogPhysics == 0)
	{
		pdlgphyDialogPhysics = new CDialogPhysics();
		pdlgphyDialogPhysics->Create(IDD_DIALOG_PHYSICS, this);
	}
	pdlgphyDialogPhysics->SetInstance(pins);
	pdlgphyDialogPhysics->ShowWindow(SW_SHOWNORMAL);
}


//*********************************************************************************************
void CGUIAppDlg::EditObject()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Do nothing if no object is currently selected.
	CPartition* ppart = pipeMain.ppartLastSelected();
	if (ppart == 0)
		return;

	CInstance* pins = ptCast<CInstance>(ppart);
	if (pins)
	{
		//
		// If the currently selected object has a light attached, edit the light
		// properties.
		//
		CInstance* pins_light = pipeMain.pinsGetLight(ptCast<CInstance>(ppart));
		if (pins_light)
		{
			OpenLightProperties(
				rptr_dynamic_cast(
					CLight, 
					rptr_nonconst(
						pins_light->prdtGetRenderInfo()
					)
				)
			);
		}
		else
		{
			// Otherwise edit the material of an object.
			OpenObjectDialogProperties(pins);
		}
	}
	else
	{
		CMagnetPair* pmp = dynamic_cast<CMagnetPair*>(ppart);
		if (pmp)
		{
			OpenMagnetProperties(pmp);
		}
	}

	Invalidate();
}


//*********************************************************************************************
//
void CGUIAppDlg::OnEditGamma
(
) 
//
// Responds to a MENU_EDIT_GAMMA message by invoking the gamma dialog box.
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pdlgobjDialogGamma == 0)
	{
		pdlgobjDialogGamma = new CDialogGamma();
		pdlgobjDialogGamma->Create(IDD_GAMMA, this);
	}
	pdlgobjDialogGamma->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
//
void CGUIAppDlg::OnEditGore
(
) 
//
//**************************************
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pdlgobjDialogGore == 0)
	{
		pdlgobjDialogGore = new CDialogGore();
		pdlgobjDialogGore->Create(IDD_GORE, this);
	}
	pdlgobjDialogGore->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OpenFogProperties()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pfogdlgFogProperties == 0)
	{
		pfogdlgFogProperties = new CDialogFog();
		pfogdlgFogProperties->Create(IDD_FOG, this);
	}
	pfogdlgFogProperties->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::AddObject()
{
	char	str_file_name[512];		// Storage for the file name.

	// Get a filename from the file dialog box.
	if (!bGetFilename(m_hWnd, str_file_name, sizeof(str_file_name)))
	{
		// If no file is selected, then return.
		return;
	}

	// Show what is loaded.
	ShowLoadedFile(str_file_name);

	// Create the load world object.
	CLoadWorld lw(str_file_name);

	CCycleTimer ctr;
	dout << "Saving scene file:  \n";
	// Save the current positions to a standard file name.
	wWorld.bSaveWorld(strLAST_SCENE_FILE);
	dout << "Saving scene file:  " << ctr() * ctr.fSecondsPerCycle() << " seconds.\n";

}

//*********************************************************************************************
void CGUIAppDlg::AddDefaultLight()
{
	// This should be taken care of by the WDBase.

	wWorld.AddDefaultLight();
#if 0
	LoadColour(pmshLightDir, CColour(192, 192, 192));

	// Add the light with a rendering shape for interface control.
	CInstance* pins_shape = new CInstance
	(
		CPresence3<>
		(
//			CRotate3<>('x', CAngle(dDegreesToRadians(-150.0))),		// Rotation.
			// Aim light's Z axis at the target.
			CRotate3<>(d3ZAxis, CDir3<>(v3DEFAULT_LIGHT_TARGET - v3DEFAULT_LIGHT_POS)),
			1.0,													// Scale.
			// Don't use v3DEFAULT_LIGHT_POS, because we want the light in front of the viewer.
			CVector3<>(-25.0f, 50.0f, 17.0f)						// Position.
		),
		rptr_cast(CRenderType, pmshLightDir)
	);

	pins_shape->SetInstanceName("Default Light Shape");

	pinsLightDirectionalDefaultShape = pins_shape;
	wWorld.Add(pins_shape);

	// Create a light with shadowing enabled.
	rptr<CLightDirectional> pltd_light = rptr_new CLightDirectional(lvDEFAULT, true);

	// Remember this light.  It's important.
	petltLightDirectionalDefault = new CEntityLight(rptr_cast(CLight, pltd_light), pins_shape);

	// Add the global light to the wdbase.  It's important.
	wWorld.Add(petltLightDirectionalDefault);
#endif
}

//*********************************************************************************************
void CGUIAppDlg::AddDirectionalLight()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	LoadColour(pmshLightPt, CColour(192, 128, 128));

	// Add a shape representing the light.  
	CInstance* pins_shape = new CInstance
	(
		CPresence3<>
		(
			CRotate3<>(),
			1.0,													// Scale.
			CVector3<>(0, 10.0, 0) * pcamGetCamera()->pr3Presence()	// Position: in front of camera.
		),
		rptr_cast(CRenderType, pmshLightDir)
	);
	wWorld.Add(pins_shape);

	rptr<CLightDirectional> pltd_light = rptr_new CLightDirectional(lvDEFAULT);

	// Create an entity to contain it, with the shape as its controlling parent.
	wWorld.Add(new CEntityLight(rptr_cast(CLight, pltd_light), pins_shape));
}


//*********************************************************************************************
void CGUIAppDlg::AddPointLight()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	LoadColour(pmshLightPt, CColour(192, 128, 128));

	// Add the light with a shape for user reference.
	CInstance* pins_shape = new CInstance
	(
		CPresence3<>
		(
			CRotate3<>(),
			0.2,													// Scale.
			CVector3<>(0, 2.0, 0) * pcamGetCamera()->pr3Presence()	// Position: in front of camera.
		),
		rptr_cast(CRenderType, pmshLightPt)
	);
	wWorld.Add(pins_shape);

	rptr<CLightPoint> pltp_light = rptr_new CLightPoint(lvDEFAULT);

	// Create an entity to contain it, with the shape as its controlling parent.
	wWorld.Add(new CEntityLight(rptr_cast(CLight, pltp_light), pins_shape));
}


//*********************************************************************************************
void CGUIAppDlg::AddPointDirectionalLight()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	LoadColour(pmshLightPtDir, CColour(192, 192, 128));

	// Add a shape representing the light.
	CInstance* pins_shape = new CInstance
	(
		CPresence3<>
		(
			CRotate3<>(),
			0.5,													// Scale.
			CVector3<>(0, 5.0, 0) * pcamGetCamera()->pr3Presence()	// Position: in front of camera.
		),
		rptr_cast(CRenderType, pmshLightPtDir)
	);
	wWorld.Add(pins_shape);

	rptr<CLightPointDirectional> pltpd_light = rptr_new CLightPointDirectional(lvDEFAULT, CAngle(dDegreesToRadians(22.5f)));

	// Create an entity to contain it, with the shape as its controlling parent.
	wWorld.Add(new CEntityLight(rptr_cast(CLight, pltpd_light), pins_shape));
}

//*********************************************************************************************
void CGUIAppDlg::EditBackground()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Create the background settings dialog box.
	if (pbackdlgBackground == 0)
	{
		pbackdlgBackground = new CBackground();
	}

	// Destroy any open background settings dialog to ensure the dialog is sized properly.
	pbackdlgBackground->DestroyWindow();

	// Create and display the background settings dialog.
	pbackdlgBackground->Create(IDD_BACKGROUND, this);
	pbackdlgBackground->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::UpdateBackground()
{
	prenMain->pSettings->bClearBackground = true;
	prenMain->pSettings->clrBackground = clrBackground;
	prenMain->UpdateSettings();
}

//*********************************************************************************************
void CGUIAppDlg::OnClose
(
) 
//
// Responds to a WM_CLOSE message by freeing all memory associated with the main dialog.
//
//************************************** 
{
	if (fileStats)
	{
		fclose(fileStats);
		fileStats = 0;
	}
	ExitApp();
	CDialog::OnClose();
}




//*********************************************************************************************
void CGUIAppDlg::ToggleRenderFeature(ERenderFeature erf) 
{
	prenMain->pSettings->seterfState ^= erf;
	SetMenuState();
	proProfile.psFrame.Reset();
}

//*********************************************************************************************
void CGUIAppDlg::OnZbuffer() 
{
	ToggleRenderFeature(erfZ_BUFFER);
}


//*********************************************************************************************
void CGUIAppDlg::OnScreenclip() 
{
	ToggleRenderFeature(erfRASTER_CLIP);
}


//*********************************************************************************************
void CGUIAppDlg::OnScreencull() 
{
	ToggleRenderFeature(erfRASTER_CULL);
}


//*********************************************************************************************
void CGUIAppDlg::OnLight() 
{
	ToggleRenderFeature(erfLIGHT);
}


//*********************************************************************************************
void CGUIAppDlg::OnLightShade() 
{
	ToggleRenderFeature(erfLIGHT_SHADE);
}


//*********************************************************************************************
void CGUIAppDlg::OnFog() 
{
	ToggleRenderFeature(erfFOG);
}


//*********************************************************************************************
void CGUIAppDlg::OnFogShade() 
{
	ToggleRenderFeature(erfFOG_SHADE);
}


//*********************************************************************************************
void CGUIAppDlg::OnSpecular() 
{
	ToggleRenderFeature(erfSPECULAR);
}


//*********************************************************************************************
void CGUIAppDlg::OnColoured() 
{
	ToggleRenderFeature(erfCOLOURED_LIGHTS);
}


//*********************************************************************************************
void CGUIAppDlg::OnTexture() 
{
	ToggleRenderFeature(erfTEXTURE);
}


//*********************************************************************************************
void CGUIAppDlg::OnTransparent() 
{
	ToggleRenderFeature(erfTRANSPARENT);
}


//*********************************************************************************************
void CGUIAppDlg::OnBump() 
{
	ToggleRenderFeature(erfBUMP);
}


//*********************************************************************************************
void CGUIAppDlg::OnWireFrame() 
{
	ToggleRenderFeature(erfWIRE);
}


//*********************************************************************************************
void CGUIAppDlg::OnAlphaColour() 
{
	ToggleRenderFeature(erfALPHA_COLOUR);
}


//*********************************************************************************************
void CGUIAppDlg::OnAlphaShade() 
{
//	ToggleRenderFeature(erfALPHA_SHADE);
}


//*********************************************************************************************
void CGUIAppDlg::OnAlphaTexture() 
{
//	ToggleRenderFeature(erfALPHA_TEXTURE);
}

//*********************************************************************************************
void CGUIAppDlg::OnTrapezoids() 
{
	ToggleRenderFeature(erfTRAPEZOIDS);
}

//*********************************************************************************************
void CGUIAppDlg::OnSubpixel() 
{
	ToggleRenderFeature(erfSUBPIXEL);
}

//*********************************************************************************************
void CGUIAppDlg::OnPerspective() 
{
	ToggleRenderFeature(erfPERSPECTIVE);
}

//*********************************************************************************************
void CGUIAppDlg::OnMipMap() 
{
	ToggleRenderFeature(erfMIPMAP);
}

//*********************************************************************************************
void CGUIAppDlg::OnDither() 
{
	ToggleRenderFeature(erfDITHER);
}

//*********************************************************************************************
void CGUIAppDlg::OnFilter() 
{
	ToggleRenderFeature(erfFILTER);

	// Kill all the terrain textures.
	if (CWDbQueryTerrain().tGet() != 0)
		CWDbQueryTerrain().tGet()->Rebuild(false);
}

//*********************************************************************************************
void CGUIAppDlg::OnFilterEdges() 
{
	ToggleRenderFeature(erfFILTER_EDGES);
}

//*********************************************************************************************
void CGUIAppDlg::OnShadows() 
{
	prenMain->pSettings->bShadow = !prenMain->pSettings->bShadow;
	SetMenuState();
	proProfile.psFrame.Reset();
}

//*********************************************************************************************
void CGUIAppDlg::OnShadowTerrain() 
{
	NMultiResolution::CTextureNode::bEnableShadows ^= 1;
	SetMenuState();
	proProfile.psFrame.Reset();
}

//*********************************************************************************************
void CGUIAppDlg::OnShadowTerrainMove() 
{
	NMultiResolution::CTextureNode::bEnableMovingShadows ^= 1;
	SetMenuState();
	proProfile.psFrame.Reset();
}

//*********************************************************************************************
void CGUIAppDlg::OnRenderCache() 
{
	if (rcsRenderCacheSettings.erctMode == ercmCACHE_ON)
		rcsRenderCacheSettings.erctMode = ercmCACHE_OFF;
	else
		rcsRenderCacheSettings.erctMode = ercmCACHE_ON;
	proProfile.psFrame.Reset();
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::AlphaColourTestBar(int i_num_colour)
{
	prasMainScreen->Lock();
	lbAlphaConstant.Test(prasMainScreen.ptPtrRaw(), i_num_colour);
	prasMainScreen->Unlock();
	prasMainScreen->Flip();
}

//*********************************************************************************************
void CGUIAppDlg::OnRenderCacheTest() 
{
	// Remove all image caches.
	PurgeRenderCaches();

	if (rcsRenderCacheSettings.erctMode == ercmCACHE_ON)
	{
		rcsRenderCacheSettings.erctMode = ercmCACHE_OFF;
	}
	else
	{
		rcsRenderCacheSettings.erctMode = ercmCACHE_ON;
	}
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnDetail() 
{
	prenMain->pSettings->bDetailReduce = !prenMain->pSettings->bDetailReduce;
	proProfile.psFrame.Reset();
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnShowConsole
(
)
//
// Responds to a MENU_CONSOLE message by showing the generic console.
//
//**************************************
{
	bShowConsole = !bShowConsole;
	proProfile.psFrame.Reset();
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnShowStats
(
)
//
// Responds to a MENU_STATS message by showing the timing statistics.
//
//**************************************
{
	bShowStats = !bShowStats;
	proProfile.psFrame.Reset();
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnAvgStats() 
//
// Responds to a MENU_AVG_STATS message by toggling the state.
//
//**************************************
{
	bAvgStats = !bAvgStats;
	proProfile.psFrame.Reset();
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnShowFPS
(
)
//
// Responds to a MENU_FPS message by showing the timing statistics.
//
//**************************************
{
	bShowFPS = !bShowFPS;
	proProfile.psFrame.Reset();
	SetMenuState();
}


//*********************************************************************************************
void CGUIAppDlg::OnShowCacheStats()
//
// Responds to a MENU_CACHE_STATS message by showing the timing statistics.
//
//**************************************
{
	// Reset render cache statistics.
	rcstCacheStats.Reset();

	// Toggle render cache stats flag.
	rcstCacheStats.bKeepStats = !rcstCacheStats.bKeepStats;

	// Update the menu and screen.
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnViewHairs
(
)
//
//**************************************
{
	bShowHairs = !bShowHairs;
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnRotateWorldZ
(
)
//
//**************************************
{
	bRotateAboutWorldZ = !bRotateAboutWorldZ;
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnResetStartTriggers
(
)
//
//**************************************
{
	CStartTrigger::ResetStartTriggers();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnRestoreSubsystemDefaults
(
)
//
//**************************************
{
	pwWorld->RestoreSubsystemDefaults();
}


//*********************************************************************************************
//
void CGUIAppDlg::OnCrosshairVegetation
(
)
//
//**************************************
{
	bCrosshairVegetation = !bCrosshairVegetation;
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnCrosshairTerrainTexture()
{
	extern bool bEditTrnObjs;
	bEditTrnObjs = !bEditTrnObjs;
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnCrosshairRadius
(
)
//
//**************************************
{
	bCrosshairRadius = !bCrosshairRadius;
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnViewSpheres
(
)
//
//**************************************
{
	bShowSpheres = !bShowSpheres;
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnViewWire
(
)
//
//**************************************
{
	bShowWire = !bShowWire;
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnViewPinhead
(
)
//
//**************************************
{
	bShowPinhead = !bShowPinhead;
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnViewBones
(
)
//
//**************************************
{
	pphSystem->bShowBones = !pphSystem->bShowBones;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnViewBonesBoxes()
{
	CPhysicsInfo::setedfMain ^= edfBOXES;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnViewBonesCollide()
{
	CPhysicsInfo::setedfMain ^= edfBOXES_PHYSICS;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnViewBonesWake()
{
	CPhysicsInfo::setedfMain ^= edfBOXES_WAKE;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnViewBonesQuery()
{
	CPhysicsInfo::setedfMain ^= edfBOXES_QUERY;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnViewBonesMagnets()
{
	CPhysicsInfo::setedfMain ^= edfMAGNETS;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnViewBonesSkeletons()
{
	CPhysicsInfo::setedfMain ^= edfSKELETONS;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnViewBonesAttach()
{
	CPhysicsInfo::setedfMain ^= edfATTACHMENTS;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnViewBonesWater()
{
	CPhysicsInfo::setedfMain ^= edfWATER;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnViewBonesRaycast()
{
	CPhysicsInfo::setedfMain ^= edfRAYCASTS;
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnViewQuads
(
)
//
//**************************************
{
	NMultiResolution::CTextureNode::bOutlineNodes ^= 1;

	// Invalidate terrain textures to cause re-render.
	if (CWDbQueryTerrain().tGet() != 0)
		CWDbQueryTerrain().tGet()->Rebuild(false);

	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnRedrawTerrain
(
)
//
//**************************************
{
	// Invalidate terrain textures to cause re-render.
	// To do: communicate this via a message instead.
	if (CWDbQueryTerrain().tGet() != 0)
		CWDbQueryTerrain().tGet()->Rebuild(true);
}

//*********************************************************************************************
//
void CGUIAppDlg::OnPlayerPhysics
(
)
//
// Responds to a MENU_PLAYERPHYSICS message by toggling the player physics on/off.
//
//**************************************
{
	if (gpPlayer->pphiGetPhysicsInfo())
	{
		// If we have a valid mesh, or we have a box model, go ahead.
		if (gpPlayer->prdtGetRenderInfo() ||
			gpPlayer->pphiGetPhysicsInfo()->ppibCast())
		{
			gpPlayer->bPhysics = !gpPlayer->bPhysics;

			if (gpPlayer->bPhysics)
				gpPlayer->PhysicsActivate();
			else
				gpPlayer->PhysicsDeactivate();
		}
	}
	SetMenuState();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnPlayerInvulnerable
(
)
//
// Responds to a MENU_PLAYERINVULNERABLE message by toggling the player's invulnerability on/off.
//
//**************************************
{
	gpPlayer->bInvulnerable = !gpPlayer->bInvulnerable;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnPhysicsSleep()
{
	pphSystem->bAllowSleep = !pphSystem->bAllowSleep;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnPhysicsStep()
{
	bPhysicsStep = !bPhysicsStep;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnPhysicsPutToSleep()
{
	pphSystem->DeactivateAll();
}

//*********************************************************************************************
//
void CGUIAppDlg::OnSmackSelected
(
)
//
// Responds to a MENU_PLAYERINVULNERABLE message by toggling the player's invulnerability on/off.
//
//**************************************
{
	// No longer used
}

//*********************************************************************************************
void CGUIAppDlg::MagnetSelected(const CMagnet* pmag_type)
{
	// Statically magnet together all the selected objects.
	// Deactivate all, to avoid hassle.
	{
		forall (pipeMain.lsppartSelected, TSelectedList, itppart)
		{
			CInstance* pins = ptCast<CInstance>(*itppart);
			if (pins)
				pins->PhysicsDeactivate();
		}
	}

	// Remove any magnets, to avoid duplication.
	// Inefficient, but OK for GUIApp.
	{
		forall (pipeMain.lsppartSelected, TSelectedList, itppart)
		{
			CInstance* pins = ptCast<CInstance>(*itppart);
			if (!pins)
				continue;

			if (!pmag_type)
				// If demagneting, remove any world magnets.
				NMagnetSystem::RemoveMagnetPair(pins, 0);

			forall (pipeMain.lsppartSelected, TSelectedList, itppart2)
			{
				CInstance* pins2 = ptCast<CInstance>(*itppart2);
				if (pins2 && pins != pins2)
					NMagnetSystem::RemoveMagnetPair(pins, pins2);
			}
		}
	}

	if (!pmag_type)
		// We just wanted demagneting.
		return;

	CInstance* pins_master = 0;
	int i_magneted = 0;

	forall (pipeMain.lsppartSelected, TSelectedList, itppart)
	{
		CInstance* pins = ptCast<CInstance>(*itppart);
		if (!pins)
			continue;

		if (!pins_master)
			pins_master = pins;
		else
		{
			// If there is a magnet, magnet this to the master.
			// Place the magnet between the two magneted objects.
			CVector3<> v3_mag = (pins_master->v3Pos() + pins->v3Pos()) * 0.5;

			NMagnetSystem::AddMagnetPair
			(
				pins_master,
				pins,
				pmag_type,
				CPresence3<>(v3_mag) / pins_master->pr3Presence()
			);
			i_magneted++;
		}
	}

	if (pins_master && !i_magneted)
	{
		//
		// Only a single object was selected.
		// Magnet the master to the world.  Can then be edited to turn it into pickup, etc.
		// We must give the magnet a non-default relative placement, or it won't be selectable
		// apart from the object.
		//
		NMagnetSystem::AddMagnetPair
		(
			pins_master,
			0,
			pmag_type,
			d3ZAxis * 0.5
		);
	}
}

//*********************************************************************************************
void CGUIAppDlg::OnMagnetBreak()
{
	// Magnet with a static breakable magnet of unit strength.
	MagnetSelected(CMagnet::pmagFindShared(CMagnet(Set(emfBREAKABLE), 1.0)));
}

//*********************************************************************************************
void CGUIAppDlg::OnMagnetNoBreak()
{
	// Magnet with a static unbreakable magnet.
	MagnetSelected(CMagnet::pmagFindShared(CMagnet()));
}

//*********************************************************************************************
void CGUIAppDlg::OnDeMagnet()
{
	MagnetSelected(0);
}

//*********************************************************************************************
void CGUIAppDlg::OnCameraPlayer()
{
	// Attach the camera to the player.
	Assert(gpPlayer);
	pcamGetCamera()->SetAttached(gpPlayer, true);
	bCameraFloating = false;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnCameraSelected()
{
	// Attach the camera to the selected object.
	if (ptCast<CInstance>(pipeMain.ppartLastSelected()))
	{
		pcamGetCamera()->SetAttached(ptCast<CInstance>(pipeMain.ppartLastSelected()));
		bCameraFloating = false;
	}
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnCameraSelectedHead()
{
	// Attach the camera to the selected object.
	if (ptCast<CInstance>(pipeMain.ppartLastSelected()))
	{
		pcamGetCamera()->SetAttached(ptCast<CInstance>(pipeMain.ppartLastSelected()), true);
		bCameraFloating = false;
	}
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnCameraFree()
{
	// Free the camera (it's attached to nothing).
	pcamGetCamera()->SetAttached(0);
	bCameraFloating = false;
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnCamera2m()
{
	bCameraFloating = !bCameraFloating;
	if (bCameraFloating)
		pcamGetCamera()->SetAttached(0);
	SetMenuState();
}

//*********************************************************************************************
const char* strTEXTUREMAP = "BinData/StdTexture.bmp";
const char* strBUMPMAP    = "BinData/StdBump.bmp";

void CGUIAppDlg::AddTestBumpmap()
{
/*
	// CODE DISABLED DUE TO DELETION OF bLoadTexture.

	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Create a new shape.
	CInstance* pins = new CInstance(rptr_cast(CRenderType, pmshCube));

	// Add scale information.
	pins->SetScale(10.0f);

	// Add the shape to the root presence.
	pipeMain.AddObject(pins);
	
	// Move the object so that it is large in the viewport.
	pins->SetPos(CVector3<>(0.0f, pins->fGetScale() * 4.0f, 0.0f) * pcamGetCamera()->pr3Presence());

	// Load the texture associated with the shape.
	if (!bLoadTexture(pmshCube, strTEXTUREMAP, strBUMPMAP))
	{
		// Could not load the texture, so delete the shape.
		pipeMain.iDeleteSelected();

		// Tell the user!
		MessageBox("Can't find texture file", strBUMPMAP, MB_OK | MB_ICONHAND);
	}
*/
}


//*********************************************************************************************
BOOL CGUIAppDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Do nothing if the return or enter key is pressed.
	if (pMsg)
		if (pMsg->message == WM_KEYDOWN)
			if (pMsg->wParam == VK_RETURN)
				return true;

	if (prnshMain)
		prnshMain->ProcessMessage(*pMsg);
	
	return CDialog::PreTranslateMessage(pMsg);
}


//*********************************************************************************************
void CGUIAppDlg::SendChildrenMessage(uint u_message)
{
	SendMessageToWindow(pdlgToolbar, u_message);
	SendMessageToWindow(pcamdlgCameraProperties, u_message);
	SendMessageToWindow(pdlgobjDialogObject, u_message);
	SendMessageToWindow(pdlgmagDialogMagnet, u_message);
	//SendMessageToWindow(pdlgDialogPlayer, u_message);
	SendMessageToWindow(pdlgDialogMaterial, u_message);
	SendMessageToWindow(pdlgrcDialogRenderCache, u_message);
	SendMessageToWindow(pdlgPerspectiveSubdivide, u_message);
	//SendMessageToWindow(pdlgphyDialogPhysics, u_message);
	SendMessageToWindow(pltdlgLightProperties, u_message);
	SendMessageToWindow(pbackdlgBackground, u_message);
	SendMessageToWindow(pdlgOcclusion, u_message);
	SendMessageToWindow(pdlgScheduler, u_message);
	SendMessageToWindow(pdlgGun, u_message);
	SendMessageToWindow(pdlgAlphaColour, u_message);
	SendMessageToWindow(pdlgtDialogTerrain, u_message);
	SendMessageToWindow(pdlgtDialogWater, u_message);
	SendMessageToWindow(pfogdlgFogProperties, u_message);
	SendMessageToWindow(pdlgMipmap, u_message);
	SendMessageToWindow(pdepthdlgProperties, u_message);

	if (bFullScreen())
	{
		CStdDialog::BroadcastToDialogs(u_message);
	}
}


//*****************************************************************************************
bool CGUIAppDlg::bCanOpenChild()
{
	if (!bIsFullScreen && gmlGameLoop.bDebug())
		return true;

	return bGDIMode;
}


//*****************************************************************************************
void CGUIAppDlg::SetGDIMode(bool b_gdimode)
{
	// GDI mode is not necessary in windowed mode.
	if (!bIsFullScreen)
	{
		bGDIMode = false;
		
		gpInputDeemone->Capture(!gmlGameLoop.bDebug());
		
		return;
	}

	// Set the GDI mode flag.
	bGDIMode = b_gdimode;

	if (bGDIMode)
	{
		// Going into debug mode.

		gpInputDeemone->Capture(false);	// Never capture input in GDI mode.
	}
	else
	{
		// Going into play mode.

		SendChildrenMessage(WM_CLOSE);

		gpInputDeemone->Capture(!gmlGameLoop.bDebug());
	}

	// Redraw the screen.
	Invalidate();
}





//*********************************************************************************************
//
void CGUIAppDlg::JumpToLookAt(CInstance* pins_mover, CPartition* ppart_target)
{
	if (ppart_target == 0)
		return;

	Assert(pins_mover);

	// Move the mover to the object.
	pins_mover->Move(CPlacement3<>
	(
		ppart_target->v3Pos() 
		+ CVector3<>(0, -3.5f * ppart_target->fGetScale(), 0)
	));

	// Redraw the scene.
	Invalidate();
}

//*********************************************************************************************
//
void CGUIAppDlg::MoveIntoView(CInstance* pins_viewer, CPartition* ppart_mover)
{
	if (ppart_mover == 0)
		return;

	Assert(pins_viewer);

	// Move the mover to the viewer.
	ppart_mover->Move(CPlacement3<>
	(
		pins_viewer->v3Pos() 
		- (CVector3<>(0, -3.5f * ppart_mover->fGetScale(), 0) * pins_viewer->r3Rot())
	));

	// Redraw the scene.
	Invalidate();
}

//*****************************************************************************************
void CGUIAppDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	switch (nChar)
	{
		case VK_F1:
			if (bVKey(VK_CONTROL))
			{
				OnStatClocks();
			}
			break;


		case VK_F2:
			if (bVKey(VK_CONTROL))
			{
				if (gbMSRProfile)
					OnStatMSR0();
				break;
			}
			// Write the spatial partition hierarchy.
			wWorld.DumpSpatial();
			break;

		case VK_F3:

			if (bVKey(VK_CONTROL))
			{
				if (gbMSRProfile)
					OnStatMSR1();
				break;
			}

			if (bVKey(VK_SHIFT))
			{
				DumpTextureUse();
				break;
			}

			// Flatten the spatial partition hierarchy.
			//wWorld.FlattenPartitions();
			bRenderTerrain = !bRenderTerrain;
			Invalidate();
			break;

		case VK_F4:
			{
			#if bPARTITION_BUILD
				if (bVKey(VK_CONTROL))
				{
					// Save the current spatial partition.
//					wWorld.SaveSpatialPartitions("SpatialPartitions.prt");
					break;
				}

				if (bVKey(VK_SHIFT))
				{
					// Test load a spatial partition.
//					wWorld.LoadSpatialPartitions("SpatialPartitions.prt");
					wWorld.BuildTerrainPartitions();
					wWorld.BuildTriggerPartitions();
					break;
				}

				// Build an optimized spatial partition hierarchy.
				wWorld.BuildOptimalPartitions(0);

				// Redraw the screen.
				Invalidate();
			#else
				MessageBox("Function not available in Final Mode!", "GUIApp Error",
						   MB_OK | MB_ICONHAND);
			#endif // bPARTITION_BUILD
				break;
			}

		case VK_F5:
			{
				extern bool bDumpPolylist;
				bDumpPolylist = true;
				/*
				// Build an optimized spatial partition hierarchy.
				CPartition* ppart = wWorld.ppartGetWorldSpace();
				Assert(ppart);
				delete ppart;
				*/
			}
			break;

		case VK_F6:
			Invalidate();
			break;

		case VK_F7:
			if (bVKey(VK_CONTROL))
			{
				padAudioDaemon->KillCollisions(true);
			}
			else
			{
				OnRenderCacheTest();
			}
			break;

		// Toggle debug/play if F8 pressed.
		case VK_F8:
			if (gmlGameLoop.egmGameMode == egmDEBUG)
			{
				SendMessage(WM_COMMAND, MENU_PLAY, 0);
			}
			else
				SendMessage(WM_COMMAND, MENU_DEBUG, 0);
			break;

		case VK_F9:
//			// Edit physics object.
//			OnPhysicsProperties();
//			break;
			if (bVKey(VK_SHIFT))
			{
				// Alter the default material, and redo the cluts.
				CMaterial& rmat_default = const_cast<CMaterial&>(matDEFAULT);
				static int i = 1;
				switch (i++)
				{
					default:
						rmat_default = matMATTE;
						i = 1;
						break;
					case 1:
						rmat_default = matMETAL;
						break;
					case 2:
						rmat_default = matSHINY;
						break;
					case 3:
						rmat_default = matWATER;
						break;
				}

				// Remake all the cluts.
				pcdbMain.UpdateCluts();
				Invalidate();
			}
			else if (bVKey(VK_CONTROL))
			{
				static int i_clut_index = 0;

				// Paint a palette's clut on the screen.
				CClut* pclut = pcdbMain.pclutGet(i_clut_index);
				if (!pclut)
				{
					i_clut_index = 0;
				}
				else
				{
					++i_clut_index;
					prasMainScreen->Lock();
					pclut->Draw(rptr_cast(CRaster, prasMainScreen));
					prasMainScreen->Unlock();
					prasMainScreen->Flip();
				}
				break;
			}

		// Toggle between depth sort and Z buffer modes.
		case VK_F12:
			/*
			if (prenMain->pSettings->esSortMethod == esDepthSort)
			{
				prenMain->pSettings->esSortMethod = esPresortFrontToBack;
				SetMenuState();
				break;
			}
			if (prenMain->pSettings->esSortMethod == esPresortFrontToBack)
			{
				prenMain->pSettings->esSortMethod = esDepthSort;
				SetMenuState();
				break;
			}
			*/
			renclRenderCacheList.Dump();
			break;

		// Toggle pause state if the pause/break key is pressed.
		case VK_PAUSE:
		{
			gmlGameLoop.bPauseGame = !gmlGameLoop.bPauseGame;
			SetMenuState();
			break;
		}

		case VK_SCROLL:
		{
			// Single-step through the system.

			// Start the sim.
			gmlGameLoop.egmGameMode = egmPLAY;
			CMessageSystem(escSTART_SIM).Dispatch();

			if (bVKey(VK_SHIFT))
				// Step with default (or replay) time.
				Step(-1.0);
			else
				// Step with the time quantum physics uses.
				Step(MAX_TIMESTEP);

			// Stop the sim.
			CMessageSystem(escSTOP_SIM).Dispatch();
			gmlGameLoop.egmGameMode = egmDEBUG;

			// Do not pass to parent class handler.
			return;			
		}

		// If in debug mode, jump camera to an object.
		case VK_HOME:
		{
			if (gmlGameLoop.egmGameMode == egmDEBUG)
			{
				if (bVKey(VK_SHIFT))
				{
					// Move last selected into camera view.
					MoveIntoView(pcamGetCamera(), pipeMain.ppartLastSelected());
				}
				else
				{
					// For now, jump camera to look at selected object (takes player or attached with it).
					JumpToLookAt(pcamGetCamera(), pipeMain.ppartLastSelected());
				}
			}
			break;
		}

		// If in debug mode, iterate through the renderable objects.
		case VK_SPACE:
		{
			if (gmlGameLoop.egmGameMode == egmDEBUG)
			{
				// Get all objects in the world with rendering info.
				CWDbQueryRenderTypes wrt;

				// Find the selected object.
				foreach(wrt)
				{
					if (wrt.tGet().ppart == pipeMain.ppartLastSelected())
					{
						// Select the next object in the list.
						wrt++;
						break;
					}
				}
			
				if (wrt.bIsNotEnd())
				{
					// We have a valid object!
					CInstance* pins;
					wrt.tGet().ppart->Cast(&pins);
					pipeMain.Select(pins);
				}
				else
				{
					// We have a bogus object!  Select the first one in the WDBase.
					wrt.Begin();
					if (wrt.bIsNotEnd())
					{
						CInstance* pins;
						wrt.tGet().ppart->Cast(&pins);
						pipeMain.Select(pins);
					}
					else
						pipeMain.Select(0);
				}

				Invalidate();
			}
			break;
		}

		// Drop a marker.
		case 'M':
		{
			OnDropMarker();
			break;
		}
		
		// Debug info about the selected mesh!
		case 'O':
		{
			forall (pipeMain.lsppartSelected, TSelectedList, itppart)
			{
				CInstance* pins = ptCast<CInstance>(*itppart);
				if (!pins)
					return;

				// If an object is selected, print out its info to the debug window.

				dprintf("\n///////////////////////////////////////////////////\n");

				const char *str = pins->strGetInstanceName();

				dprintf("Object \"%s\":\n", str);

				CPresence3<> pr3 = pins->pr3Presence();

				dprintf("Origin: %f,\t%f,\t%f\n", pr3.v3Pos.tX, pr3.v3Pos.tY,pr3.v3Pos.tZ);
				dprintf("Rotate: Vector:%f,\t%f,\t%f\tAngle: %f\n", pr3.r3Rot.v3S.tX, 
					pr3.r3Rot.v3S.tY,pr3.r3Rot.v3S.tZ, pr3.r3Rot.tC);

				dprintf("Scale:  %f\n", pr3.rScale);
				
				rptr_const<CRenderType> prdt = pins->prdtGetRenderInfo();

				rptr_const<CMesh> pmsh = rptr_const_dynamic_cast(CMesh,prdt);

				if (pmsh)
				{
					dprintf("World Space Verts:\n");

					int i_last = pmsh->pav3Points.uLen;
					for (int i = 0; i < i_last; i++)
					{
						CVector3<> v3 = pmsh->pav3Points[i];

						v3 = v3 * pins->pr3Presence();

						dprintf("%d:\t%f,\t%f,\t%f\n", i, v3.tX, v3.tY, v3.tZ);
					}
				}
				else
				{
					dprintf("Object does not have a CMesh rendertype.\n");
				}


			}
			break;
		}

		case 'P':
		{
			// P for profile
//			OnShowStats();
//			OnShowFPS();
			OnPlayerPhysics();
			break;
		}
		case 'R':
		{
			if (bVKey(VK_CONTROL))
			{
				//  Purge on ctrl-alt-r
				wWorld.Reset();

				// Add the shell to the world database.
				wWorld.AddShell(this);
				Invalidate();
			}
			else if (bVKey(VK_SHIFT))
			{
				OnReset();
				Invalidate();
			}
			break;
		}

		case 'T':
		{
			if (bVKey(VK_CONTROL))
			{
				extern void PlayerTeleportToNextLocation();
				PlayerTeleportToNextLocation();
			}
			else
		    {
				CTerrain* ptrr = CWDbQueryTerrain().tGet();
				if (ptrr)
				{
					forall (pipeMain.lsppartSelected, TSelectedList, itppart)
					{
						CPresence3<> pr3 = (*itppart)->pr3Presence();
						
						TReal r_height = ptrr->rHeight(pr3.v3Pos.tX, pr3.v3Pos.tY);

						// Test the terrain height at the selected object's location.
						dprintf("\nTerrain height: %f, %f, %f\n", pr3.v3Pos.tX, pr3.v3Pos.tY, r_height);
					}
				}
			}

			Invalidate();
			break;
		}

		case 'L':
		{
			OnGiveMeALight();
			break;
		}

		//
		// No, Windows doesn't define a VK_ symbol for punctuation characters, and there
		// seems to be no relation between the code it returns and the ASCII value.
		//

		// case '/'
		case 0xBF:
			// '/' + shift = '?'
			if (bVKey(VK_SHIFT))
				bWander = !bWander;
			break;

		// case '`':
		case 0xC0:
			// Toggle the video overlay.
			pVideoOverlay->Enable(!pVideoOverlay->bEnabled());
			Invalidate();
			break;


		case 0xbb:
			if (GetAsyncKeyState(VK_CONTROL) < 0)
			{
				prnshMain->AdjustViewportRelativeHorizontal(fViewDeltaIncrease, GetSafeHwnd());
				break;
			}
			if (GetAsyncKeyState(VK_SHIFT) < 0)
			{
				prnshMain->AdjustViewportRelativeVertical(fViewDeltaIncrease, GetSafeHwnd());
				break;
			}
			prnshMain->AdjustViewportRelative(fViewDeltaIncrease, GetSafeHwnd());
			break;

		case 0xbd:		// - and _
			if (GetAsyncKeyState(VK_CONTROL) < 0)
			{
				prnshMain->AdjustViewportRelativeHorizontal(fViewDeltaDecrease, GetSafeHwnd());
				break;
			}
			if (GetAsyncKeyState(VK_SHIFT) < 0)
			{
				prnshMain->AdjustViewportRelativeVertical(fViewDeltaDecrease, GetSafeHwnd());
				break;
			}
			prnshMain->AdjustViewportRelative(fViewDeltaDecrease, GetSafeHwnd());
			break;

		default:
		{
		}

	}
	
	//
	//		Keypad keys position camera relative to attached object, facing the object.
	//
	//		KP centre	centre
	//		KP up		forward
	//		KP down		back
	//		KP left		left
	//		KP right	right
	//		KP pgup		top
	//		KP pgdown	bottom
	//		KP home		move closer
	//		KP end		move farther
	//

	const TReal rDIST_INCREMENT_RATIO	= 1.1;
	const TReal rDEFAULT_DIST_SCALE		= 2.0;

	CCamera* pcam = pcamGetCamera();
	CPlacement3<> p3_rel = pcam->p3Relative();

	// Preserve distance.
	TReal r_dist = p3_rel.v3Pos.tLen();

	// If no current distance, set to default.
	if (Fuzzy(r_dist) == 0)
	{
		if (pcam->pinsAttached())
			r_dist = pcam->pinsAttached()->pbvBoundingVol()->fMaxExtent()
				* pcam->pinsAttached()->fGetScale();

		else
			r_dist = 5.0f;
		r_dist = Max(r_dist, 0.5) * rDEFAULT_DIST_SCALE;
	}

	bool b_update = true;

	switch (nChar)
	{
		case VK_NUMPAD5:
			// Null placement.
			p3_rel = CPlacement3<>();
			break;

		case VK_NUMPAD8:
			// Forward.
			p3_rel.v3Pos = d3YAxis * r_dist;
			p3_rel.r3Rot = CRotate3<>(d3ZAxis, CAngle(dPI));
			break;
		case VK_NUMPAD2:
			// Back.
			p3_rel.v3Pos = -d3YAxis * r_dist;
			p3_rel.r3Rot = CRotate3<>();
			break;
		case VK_NUMPAD4:
			// Left.
			p3_rel.v3Pos = -d3XAxis * r_dist;
			p3_rel.r3Rot = CRotate3<>(d3ZAxis, -CAngle(dPI_2));
			break;
		case VK_NUMPAD6:
			// Right.
			p3_rel.v3Pos = d3XAxis * r_dist;
			p3_rel.r3Rot = CRotate3<>(d3ZAxis, +CAngle(dPI_2));
			break;
		case VK_NUMPAD9:
			// Top.
			p3_rel.v3Pos = d3ZAxis * r_dist;
			p3_rel.r3Rot = CRotate3<>(d3XAxis, -CAngle(dPI_2));
			break;
		case VK_NUMPAD3:
			// Bottom.
			p3_rel.v3Pos = -d3ZAxis * r_dist;
			p3_rel.r3Rot = CRotate3<>(d3XAxis, +CAngle(dPI_2));
			break;

		case VK_NUMPAD7:
			// Decrease distance.
			p3_rel.v3Pos /= rDIST_INCREMENT_RATIO;
			break;

		case VK_NUMPAD1:
			// Increase distance.
			p3_rel.v3Pos *= rDIST_INCREMENT_RATIO;
			break;

		default:
			b_update = false;
	}

	if (b_update)
	{
		if (!pcam->pinsAttached())
			OnCameraPlayer();
		pcam->SetRelative(p3_rel);
		Invalidate();
	}

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

//*********************************************************************************************
//
// Module function implementation.
//

//*********************************************************************************************
void SendMessageToWindow(CWnd* pwnd, uint u_message)
{
	// Do nothing if the window hasn't been instantiated.
	if (pwnd == 0 || pwnd->m_hWnd == 0)
		return;

	// Send the close message.
	pwnd->SendMessage(u_message);
}

//*********************************************************************************************
//
void CGUIAppDlg::DebugMovement
(
	CInstance* pins,			// The instance that is controlled by the move keys.
	const CInstance* pins_pov	// The instance whose point of view is used to interpret the controls.
								//  Usually a camera or the pins itself.
)
//
// Handles movement while in debug mode. This function maps the following keys:
//
//		Key			Function
//		-----		--------
//		shift		fast
//		control		strafe
//		end			return to 'home' position (to the origin)
//		up arrow	move forward
//		down arrow	move backward
//		left arrow	turn left (strafe left with control key)
//		right arrow	turn right (strafe right with control key)
//		plus key	move up
//		minus key	move down
//		page up		turn up
//		page down	turn down
//		insert		twist left
//		delete		twist right
//
//**************************************
{
	static CTimer tmrStep;				// The movement timer.

	if (::GetFocus() != m_hWnd)
		return;

	if (!bAllowDebugMovement)
		return;

	CVector3<> v3_camdir;				// Camera movement vector.
	CRotate3<> r3_camrot;				// Camera rotation.
	float      f_displacement;			// Movement amount.
	float      f_delta_angle;			// Rotation amount.
	bool       b_update_window = false;	// Repaint window flag.
	CPlacement3<>	p3	= pins->pr3Presence();


	// Find the real elapsed time, and clamp it to 5fps.
	TSec s_elapsed = tmrStep.sElapsed();
	s_elapsed = Min(s_elapsed, 0.2f);

	f_displacement = rDebugStepDistance * s_elapsed;
	f_delta_angle  = rDebugTurnAngle * s_elapsed;

	// Fast or normal movement and rotational values.
	if (bVKey(VK_SHIFT))
	{
		f_displacement *=  5.0f;
		f_delta_angle  *=  5.0f;
	}

	if (bSmallMoveSteps)
	{
		f_displacement *=  .1f;
		f_delta_angle  *=  .1f;
	}

	if (gmlGameLoop.bCanStep() && bWander)
	{
		// Wander around when in play mode.
		const float fDECAY_RATE	= 0.95;			// Ratio that rotation decays each frame.
		const float fANGLE_RAND_RANGE = 0.02;	// Range of random addition to fAngle each frame.
		const float fANGLE_HOMING_RATE = 0.05;	// Range of correction toward home at boundary.

		if (sWanderStop > 0)
		{
			// Check to see if we are done.
			bool b_done;

			if (bWanderDurationInSecs)
				b_done = sWanderStop < CMessageStep::sStaticTotal;
			else
				b_done = sWanderStop < CMessageStep::u4Frame;

			if (b_done)
			{
				// Done wandering!
				bWander = false;
				sWanderStop = -1.0f;
				SendMessage(WM_COMMAND, MENU_DEBUG, 0);
				return;
			}
		}

		// Get the world extents, and compare to the current position, in XY.
		CVector3<> v3_world_min, v3_world_max;
		CTerrain* ptrr = CWDbQueryTerrain().tGet();
		AlwaysVerify(ptrr->bGetWorldExtents(v3_world_min, v3_world_max));

		// Find centre, and adjust extents inward a bit.
		CVector3<> v3_adjust = (v3_world_max - v3_world_min) * 0.2;
		v3_world_min += v3_adjust;
		v3_world_max -= v3_adjust;

		// Are we close to the edge?
		if (!bWithin(p3.v3Pos.tX, v3_world_min.tX, v3_world_max.tX) ||
			!bWithin(p3.v3Pos.tY, v3_world_min.tY, v3_world_max.tY))
		{
			// Yes!  Turn back to the center.

			//
			// Swing the camera around to face toward the centre.
			// We want an amount proportional to the difference between the current facing vector
			// and the vector facing the centre of the world.  This can be obtained from the 
			// 2D cross-product (sin) of the facing vector and one halfway between it and the 
			// centre vector.
			//
			CVector3<> v3_centre = (v3_world_max + v3_world_min) * 0.5;
			CDir2<> d2_current = d3YAxis * p3.r3Rot;
			CDir2<> d2_target = v3_centre - p3.v3Pos;
			TReal r_cross = d2_current ^ (d2_current + d2_target);
	
			// Use the cross product to determine direction of turn, but use the difference between the two 
			//  vectors to determine turn rate.
			float f_angle = (r_cross > 0) ? fANGLE_HOMING_RATE : - fANGLE_HOMING_RATE;
			f_angle *= (d2_current - d2_target).tLenSqr();
			
			p3.r3Rot *= CRotate3<>(d3ZAxis, f_angle);
		}
		else
		{
			// No!  Wander randomly.

			// Randomly adjust the angular rotation rate.
			fWanderAngleRate += randWander(-fANGLE_RAND_RANGE, +fANGLE_RAND_RANGE);

			// Apply exponential decay to rate.
			fWanderAngleRate *= fDECAY_RATE;

			// Apply the rotation about the Z axis.
			p3.r3Rot *= CRotate3<>(d3ZAxis, fWanderAngleRate);
		}

		// Move forward based on this rotation.
		p3.v3Pos += (d3YAxis * p3.r3Rot) * f_displacement;
		b_update_window = true;
	}

	// Move to the 'home' position.
	if (bVKey(VK_END))
	{
		MoveCameraToSceneCentre();
		return;
	}

	// Move forward.
	if (bVKey(VK_UP))
	{
		v3_camdir = CVector3<> (0.0f, f_displacement, 0.0f);
		p3.v3Pos += v3_camdir * pins_pov->r3Rot();
		b_update_window = true;
	}

	// Move backward.
	if (bVKey(VK_DOWN))
	{
		v3_camdir = CVector3<> (0.0f, -f_displacement, 0.0f);
		p3.v3Pos += v3_camdir * pins_pov->r3Rot();
		b_update_window = true;
	}

	// Move/strafe left.
	if (bVKey(VK_LEFT))
	{
		if (bVKey(VK_CONTROL))
		{
			v3_camdir = CVector3<> (-f_displacement, 0.0f, 0.0f);
			p3.v3Pos += v3_camdir * pins_pov->r3Rot();
		}
		else
		{
			// Hack to allow slower turning when moving, even if shifting.
			if (b_update_window)
				f_delta_angle  = 5.0f;

			if (bRotateAboutWorldZ)
				r3_camrot = CRotate3<>(d3ZAxis, CAngle(dDegreesToRadians(f_delta_angle)));
			else
				r3_camrot = CRotate3<>(d3ZAxis * pins_pov->r3Rot(), CAngle(dDegreesToRadians(f_delta_angle)));

			p3.r3Rot *= r3_camrot;
		}
		b_update_window = true;
	}

	// Move/strafe right.
	if (bVKey(VK_RIGHT))
	{
		if (bVKey(VK_CONTROL))
		{
			v3_camdir = CVector3<> (f_displacement, 0.0f, 0.0f);
			p3.v3Pos += v3_camdir * pins_pov->r3Rot();
		}
		else
		{
			// Hack to allow slower turning when moving, even if shifting.
			if (b_update_window)
				f_delta_angle  = 5.0f;

			if (bRotateAboutWorldZ)
				r3_camrot = CRotate3<>(d3ZAxis, CAngle(dDegreesToRadians(-f_delta_angle)));
			else
				r3_camrot = CRotate3<>(d3ZAxis * pins_pov->r3Rot(), CAngle(dDegreesToRadians(-f_delta_angle)));

			p3.r3Rot *= r3_camrot;
		}
		b_update_window = true;
	}

	// Move up.
	if (bVKey(VK_ADD))
	{
		v3_camdir = CVector3<> (0.0f, 0.0f, f_displacement);
		p3.v3Pos += v3_camdir * pins_pov->r3Rot();
		b_update_window = true;
	}

	// Move down.
	if (bVKey(VK_SUBTRACT))
	{
		v3_camdir = CVector3<> (0.0f, 0.0f, -f_displacement);
		p3.v3Pos += v3_camdir * pins_pov->r3Rot();
		b_update_window = true;
	}

	// Hack to allow slower turning when moving, even if shifting.
	if (b_update_window)
		f_delta_angle  = 5.0f;

	// Turn up.
	if (bVKey(VK_PRIOR))
	{
		r3_camrot = CRotate3<>(d3XAxis * pins_pov->r3Rot(), CAngle(dDegreesToRadians(f_delta_angle)));
		p3.r3Rot *= r3_camrot;
		b_update_window = true;
	}

	// Turn down.
	if (bVKey(VK_NEXT))
	{
		r3_camrot = CRotate3<>(d3XAxis * pins_pov->r3Rot(), CAngle(dDegreesToRadians(-f_delta_angle)));
		p3.r3Rot *= r3_camrot;
		b_update_window = true;
	}

	// Twist left.
	if (bVKey(VK_INSERT))
	{
		r3_camrot = CRotate3<>(d3YAxis * pins_pov->r3Rot(), CAngle(dDegreesToRadians(f_delta_angle)));
		p3.r3Rot *= r3_camrot;
		b_update_window = true;
	}

	// Twist right.
	if (bVKey(VK_DELETE))
	{
		r3_camrot = CRotate3<>(d3YAxis * pins_pov->r3Rot(), CAngle(dDegreesToRadians(-f_delta_angle)));
		p3.r3Rot *= r3_camrot;
		b_update_window = true;
	}

	// Repaint the scene if required.
	if (b_update_window)
	{
		if (bCameraFloating)// && pins == pcamGetCamera())
		{
			// The floating camera hack.
			p3.v3Pos.tZ = 1.65;

			CTerrain* ptrr = CWDbQueryTerrain().tGet();
			if (ptrr)
				p3.v3Pos.tZ += ptrr->rHeight(p3.v3Pos.tX, p3.v3Pos.tY);

			if (pins == gpPlayer)
				p3.v3Pos.tZ -= gpPlayer->p3HeadPlacement().v3Pos.tZ;
		}

		pins->Move(p3);
		Invalidate();
	}
}

//*********************************************************************************************
void CGUIAppDlg::MoveCameraToSceneCentre()
{
	// Restore null placement.
	pcamGetCamera()->Move(CPlacement3<>());
	Invalidate();
}

//*********************************************************************************************
void CGUIAppDlg::FPSEstimate()
{
	const int iNumFrames = 120;
	CAngle angDeltaRot(dDegreesToRadians(360.0f / float(iNumFrames)));
								// Rotational delta angle 

	// Move the camera to the centre of the currently loaded scene.
	//MoveCameraToSceneCentre();

	// Make sure that we are not running in GDI mode when in full screen.
	SetGDIMode(false);

	// Set game mode state.
	EGameMode egm = gmlGameLoop.egmGameMode;
	gmlGameLoop.egmGameMode = egmPLAY;

	// Start test.
	bool b_avg_save = bAvgStats;
	bool b_fps_save = bShowFPS;
	bAvgStats = true;
	bShowFPS = false;

//	MoveCameraToInitPosition();

	proProfile.psFrame.Reset();

	//
	// Get start data for conducting test spin.
	//
	float f_radius = CVector3<>
	(
		pcamGetCamera()->v3Pos().tX,
		pcamGetCamera()->v3Pos().tY,
		0.0f
	).tLen();

	//
	// Execute the main test loop.
	//
	for (int i_frame = 0; i_frame < iNumFrames; i_frame++)
	{
		// Rotate the camera.
		CRotate3<> r3_camrot = pcamGetCamera()->r3Rot() *
			CRotate3<>(d3ZAxis * pcamGetCamera()->r3Rot(), angDeltaRot);

		// Reposition the camera based on its rotation.
		CVector3<> v3_campos(0.0f, -f_radius, pcamGetCamera()->v3Pos().tZ);
		v3_campos *= r3_camrot;
		pcamGetCamera()->Move(CPlacement3<>(r3_camrot, v3_campos));

		// Update the frame.
		PaintWindow();
	}

	// Switch to GDI output for outputting the dialog box when in full screen mode.
	SetGDIMode(true);
	if (bIsFullScreen)
		ShowCursor(true);
	//ClipCursor(0);

	// Restore game mode state.
	gmlGameLoop.egmGameMode = egm;

	// Output the stats.
	CStrBuffer strbuf(2000);

	proProfile.psMain.WriteToBuffer(strbuf);
	bAvgStats = b_avg_save;
	bShowFPS = b_fps_save;
	MessageBox(strbuf, "Results of Standard Test");
}


//*********************************************************************************************
//
void CGUIAppDlg::OnRButtonDblClk(UINT nFlags, CPoint point) 
//
// Handles the WM_RMOUSEDBLCLK message by editing the object or camera.
//
//************************************** 
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Edit physics object.
	OnPhysicsProperties();

	// Call base class member function.
	CDialog::OnRButtonDblClk(nFlags, point);
}

//*********************************************************************************************
void CGUIAppDlg::OpenRenderCacheDialog()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pdlgrcDialogRenderCache == 0)
	{
		pdlgrcDialogRenderCache = new CDialogRenderCache();
		pdlgrcDialogRenderCache->Create(IDD_RENDERCACHE, this);
	}
	pdlgrcDialogRenderCache->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
//
BOOL CGUIAppDlg::OnCmdMsg
(
	UINT nID,
	int nCode,
	void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo
)
//
// Handles menu commands.
//
//************************************** 
{
	// On IDOK or IDCANCEL we're ending the app, and so should not muck with any windows that
	// may or may not still be valid.
	if (nCode != 0 && !(nID == IDCANCEL || nID == IDOK))
	{
		CMessageNewRaster msgnewr(int(nID) - MENU_WINDOW - 1, bSystemMem, false);
		msgnewr.Dispatch();

		// Clear the screen modes menu.
		for (int i_id = MENU_WINDOW; i_id <= MENU_WINDOW + Video::iModes; i_id++)
			GetMenu()->CheckMenuItem(i_id, i_id == nID ? MF_CHECKED : MF_UNCHECKED);

		SetMenuState();

		return TRUE;
	}

	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//*********************************************************************************************
void CGUIAppDlg::UpdateScreen() 
{
	// Force an update.
	CMessageNewRaster msgnewr(iScreenMode, bSystemMem, true);
	msgnewr.Dispatch();
}

//*********************************************************************************************
void CGUIAppDlg::OnSystemMem() 
{
	bool b_system = bSystemMem;
	if (!bSystemMem && d3dDriver.bUseD3D())
	{
		d3dDriver.Purge();
		d3dDriver.Uninitialize();
		d3dDriver.SetInitEnable(false);
	}

	CMessageNewRaster msgnewr(iScreenMode, !bSystemMem, false);
	msgnewr.Dispatch();

	if (b_system)
	{
		d3dDriver.SetInitEnable(true);
	}

	//
	// Use interpolated water for the software rasterizer and non-interpolated water for
	// hardware rasterizers.
	//
	CEntityWater::bInterp = !d3dDriver.bUseD3D();

	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::ToggleFastMode() 
{
	if (bFastMode)
	{
		RestoreFromFastMode();
		return;
	}

	// Store render states prior to going to fast mode.
	erfPrevious = prenMain->pSettings->seterfState;

	// Set new a new flag combination.
	prenMain->pSettings->seterfState -= erfCOPY;
	prenMain->pSettings->seterfState -= erfLIGHT_SHADE;
	prenMain->pSettings->seterfState -= erfSPECULAR;
	prenMain->pSettings->seterfState -= erfTEXTURE;
	prenMain->pSettings->seterfState -= erfBUMP;
	prenMain->pSettings->seterfState -= erfSUBPIXEL;
	prenMain->pSettings->seterfState -= erfPERSPECTIVE;
	prenMain->pSettings->seterfState -= erfDITHER;
	prenMain->pSettings->seterfState -= erfFILTER;
	prenMain->pSettings->seterfState -= erfFILTER_EDGES;
	prenMain->pSettings->seterfState -= erfFOG;
	prenMain->pSettings->seterfState -= erfFOG_SHADE;
	prenMain->pSettings->seterfState -= erfTRANSPARENT;

	//
	// Set the camera clipping planes to a closer position.
	//
	ptr<CCamera> pcam = CWDbQueryActiveCamera().tGet();
	CCamera::SProperties camprop = pcam->campropGetProperties();

	// Store the old camera property settings.
	campropProp = camprop;

	// Try some new settings.
	camprop.rFarClipPlaneDist  = 100.0f;
	camprop.rNearClipPlaneDist = 0.1f;
	pcam->SetProperties(camprop);

	// Set the fast mode flag and redraw screen and menu.
	bFastMode = true;
	SetMenuState();
}


//*****************************************************************************************
//
void CGUIAppDlg::RestoreFromFastMode
(
)
//
// Restores settings prior to going into fast render mode.
//
//**************************************
{
	bFastMode = false;
	
	// Restore render states settings prior to going to fast mode.
	prenMain->pSettings->seterfState = erfPrevious;

	// Restore the camera settings prior to going into fast mode.
	ptr<CCamera> pcam = CWDbQueryActiveCamera().tGet();
	pcam->SetProperties(campropProp);

	// Change the menu and redraw.
	SetMenuState();
}


//*****************************************************************************************
void CGUIAppDlg::ToggleDelaunayTest()
{
/*
	static CDelaunayTest dlt;
	CDelaunayTest* pdlt = &dlt;

	bDelaunayTest = !bDelaunayTest;

	if (bDelaunayTest)
		wWorld.Add(pdlt);
	else
		wWorld.Remove(pdlt);

	// Change the menu and redraw.
	SetMenuState();
*/
}


//*********************************************************************************************
void CGUIAppDlg::OnMenuTerrain() 
{
	// Add the test terrain object.
//	wWorld.Add(new CEntityTerrain());
}

//*********************************************************************************************
void CGUIAppDlg::ToggleConPhysics()
{
	conPhysics.SetActive(!conPhysics.bIsActive() && !bFullScreen());

	// Set a new menu state.
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnStatPhysics()
{
	conPhysicsStats.SetActive(!conPhysicsStats.bIsActive() && !bFullScreen());

	// Set a new menu state.
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnDrawPhysics()
{
	if (!pdldrPhysics)
	{
		// Open dialog first time.
		pdldrPhysics = new CDialogDraw("Physics Graph");
		pdldrPhysics->ShowWindow(true);
	}
	else
		// Toggle the shown state.
		pdldrPhysics->ShowWindow(!pdldrPhysics->bIsVisible());

	// Set a new menu state.
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnDrawAI()
{
	gaiSystem.bShow3DInfluences = !gaiSystem.bShow3DInfluences;

	// Set a new menu state.
	SetMenuState();
}


//*********************************************************************************************
void CGUIAppDlg::ToggleConShadows()
{
	conShadows.SetActive(!conShadows.bIsActive() && !bFullScreen());

	// Set a new menu state.
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::ToggleConTerrain()
{
	conTerrain.SetActive(!conTerrain.bIsActive() && !bFullScreen());

	// Set a new menu state.
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::ToggleConAI()
{
	conAI.SetActive(!conAI.bIsActive() && !bFullScreen());

	// Set a new menu state.
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::ToggleConDepthSort()
{
	conDepthSort.SetActive(!conDepthSort.bIsActive() && !bFullScreen());

	// Set a new menu state.
	SetMenuState();
}


//*********************************************************************************************
void CGUIAppDlg::OnLoadScene()
{
	char str_filename[512];

	if (bGetFilename(m_hWnd, str_filename, sizeof(str_filename), 
		"Load Scene", 
		"Game (*.scn;*.grf)\0" "*.scn;*.grf\0" 
		"Scene (*.scn)\0" "*.scn\0" 
		"Groff (*.grf)\0" "*.grf\0"
		"All (*.*)\0" "*.*\0"))
	{
		// Add to recent file list.
		AddRecentFile(str_filename);

		// Show what is loaded.
		ShowLoadedFile(str_filename);

		if (bMatchExtension(str_filename, "grf"))
		{
			// Load a groff file.
			CLoadWorld lw(str_filename);

			// Save the current positions to a standard file name.
			dout << "Saving scene file:  \n";
			wWorld.bSaveWorld(strLAST_SCENE_FILE);
		}
		else
		{
			// Assume it's a scene file.
			if (!wWorld.bLoadScene(str_filename))
				MessageBox("Cannot load file!", "Load Error", MB_OK);
			else
				// Remember the last loaded scene.
				strcpy(strLoadFile, str_filename);
		}

		// Update menu state.
		SetMenuState();
	}

// HACK HACK HACK--   Leave these here for area creation purposes, so folks can use it by simple uncommenting.
//	extern void FindDuplicates();
//	FindDuplicates();
}


//*********************************************************************************************
void CGUIAppDlg::OnLoadRecentFile(UINT uID)
{
	char str_key[32];
	char str_filename[512];
	sprintf(str_key, "LastFileOpened%d", uID - MENU_OPEN_LAST);
    GetRegString(str_key, str_filename, sizeof(str_filename), "");

	if (strlen(str_filename))
	{
		// Show what is loaded.
		ShowLoadedFile(str_filename);

		if (bMatchExtension(str_filename, "grf"))
		{
			// Load a groff file.
			CLoadWorld lw(str_filename);

			// Save the current positions to a standard file name.
			dout << "Saving scene file:  \n";
			wWorld.bSaveWorld(strLAST_SCENE_FILE);
		}
		else
		{
			// Assume it's a scene file.
			if (!wWorld.bLoadScene(str_filename))
				MessageBox("Cannot load file!", "Load Error", MB_OK);
			else
				// Remember the last loaded scene.
				strcpy(strLoadFile, str_filename);
		}
	}
}


//*********************************************************************************************
void CGUIAppDlg::OnSaveScene()
{
	if (*strLoadFile)
	{
		if (!wWorld.bSaveWorld(strLoadFile))
			MessageBox("Cannot save file!", "Save Error", MB_OK);
	}
	else
		OnSaveAsScene();
}

//*********************************************************************************************
void CGUIAppDlg::OnSaveAsScene()
{
	char str_filename[512];

	if (bGetSaveFilename(m_hWnd, str_filename, sizeof(str_filename), 
		"Save Scene", "Scene (*.scn)\0*.scn\0All (*.*)\0*.*\0"))
	{
		// If no '.' is found, append .scn.
		if (!strchr(str_filename, '.'))
			strcat(str_filename, ".scn");
		if (!wWorld.bSaveWorld(str_filename))
			MessageBox("Cannot save file!", "Save Error", MB_OK);
		else
			// Remember the last loaded scene.
			strcpy(strLoadFile, str_filename);
	}
}

//*********************************************************************************************
void CGUIAppDlg::OnTextSave()
{
	char str_filename[512];

	if (bGetFilename(m_hWnd, str_filename, sizeof(str_filename), 
		"Save Scene", "Text (*.txt)\0*.txt\0All (*.*)\0*.*\0"))
	{
		if (!wWorld.bSaveAsText(str_filename))
			MessageBox("Cannot save file!", "Save Error", MB_OK);
	}
}

//*********************************************************************************************
void CGUIAppDlg::OnReset()
{
	const char* str_reset_file = *strLoadFile ? strLoadFile : strLAST_SCENE_FILE;

	if (*str_reset_file)
	{
		if (!wWorld.bLoadWorld(str_reset_file, true))
			MessageBox("Cannot load file!", "Reset Error", MB_OK);
	}

	pipeMain.UnselectAll();

	// Reset wander params as well.
	randWander.Seed();
	fWanderAngleRate = 0;
}

//*********************************************************************************************
void CGUIAppDlg::OnResetSelected()
{
	// Not implemented!
	Assert(false);
}

//*********************************************************************************************
void CGUIAppDlg::OnReplaySave()
{
	char	str_fname[MAX_PATH];

	if (crpReplay.bSaveActive())
	{
		// save is active so disable it....
		SetMenuItemState(MENU_REPLAYSAVE, FALSE);
		crpReplay.SetSave(FALSE);
	}
	else
	{
		// save is inactive so ask for a filename and enable it
		// get a filename for the replay and enable saving
		if (::bGetReplayFileName( AfxGetMainWnd()->m_hWnd, str_fname, MAX_PATH, FALSE )==FALSE)
		{
			//
			// if we cannot get a file name ensure that the
			// option is not selected.
			//
			SetMenuItemState(MENU_REPLAYSAVE, FALSE);
			crpReplay.SetSave(FALSE);
			return;
		}

		// open the replay file now so it cannot be used for loading as well
		if (crpReplay.bOpenReplay(str_fname)==FALSE)
		{
			//
			// we cannot open the selected file, either it does not exist
			// or it is not a replay file.
			//
			SetMenuItemState(MENU_REPLAYSAVE, FALSE);
			crpReplay.SetSave(FALSE);
			return;
		}

		// we now have a valid relay filename so enable replay saving
		// and tick the menu
		SetMenuItemState(MENU_REPLAYSAVE, TRUE);
		crpReplay.SetSave(TRUE);
	}
}



//*********************************************************************************************
void CGUIAppDlg::OnReplayLoad()
{
	char	str_fname[MAX_PATH];

	if (crpReplay.bLoadActive())
	{
		// load is active so disable it....
		SetMenuItemState(MENU_REPLAYLOAD, FALSE);
		crpReplay.SetLoad(FALSE);
	}
	else
	{
		// get a filename for the replay and enable loading
		if (::bGetReplayFileName( AfxGetMainWnd()->m_hWnd, str_fname, MAX_PATH, TRUE )==FALSE)
		{
			//
			// if we cannot get a file name ensure that the
			// option is not selected.
			//
			SetMenuItemState(MENU_REPLAYLOAD, FALSE);
			crpReplay.SetLoad(FALSE);
			return;
		}

		// open the replay file now so it cannot be used for savinging as well
		if (crpReplay.bOpenReadReplay(str_fname)==FALSE)
		{
			//
			// we cannot open the selected file, either it does not exist
			// or it is not a replay file.
			//
			SetMenuItemState(MENU_REPLAYLOAD, FALSE);
			crpReplay.SetLoad(FALSE);
			return;
		}

		// we now have a valid relay filename so enable replays
		// tick the menu
		SetMenuItemState(MENU_REPLAYLOAD, TRUE);
		crpReplay.SetLoad(TRUE);
	}
}


//*********************************************************************************************
void CGUIAppDlg::OnLoadAnim()
{
	// The maximum number of characters in the anim file name.
	#define iMAX_ANIMNAME_CHARS (512)

	char str_filename[iMAX_ANIMNAME_CHARS];

	// Open the file dialog.
	bool b_ok = bGetFilename
	(
		m_hWnd,						// Window handle of parent to the dialog.
		str_filename,				// Pointer to string to put filename.
		iMAX_ANIMNAME_CHARS,		// Maximum number of characters for the filename.
		"Load animation",
		"Animation (*.asa;*.asb)\0*.asa;*.asb\0All (*.*)\0*.*\0"
									// Optional extension filter.
	);

	// Abort if the user hits cancel.
	if (!b_ok)
		return;

	// Report if the file is not found.
	if (!bFileExists(str_filename))
	{
		MessageBox("Animation file not found!", "Animation Error", MB_OK);
		return;
	}

	// Redraw the screen.
	PaintWindow();

	CAnimationScript* pans = new CAnimationScript(str_filename, true);
	pAnimations->Add(pans);
	pans->Start();
}

//*********************************************************************************************
void CGUIAppDlg::OpenTerrainDialog()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pdlgtDialogTerrain == 0)
	{
		pdlgtDialogTerrain = new CDialogTerrain();
		pdlgtDialogTerrain->Create(IDD_TERRAIN, this);
	}
	pdlgtDialogTerrain->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OpenWaterDialog()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pdlgtDialogWater == 0)
	{
		pdlgtDialogWater = new CDialogWater();
		pdlgtDialogWater->Create(IDD_WATER, this);
	}
	pdlgtDialogWater->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OnMenuAddSkeleton()
{
#if 0
	// Hack to test raptor physics/skin


		// Get an instance of the mesh being loaded.
		rptr<CMesh> pmesh = rptr_cast(CMesh, rptr_new CSkeletonTestHuman());
		rptr<CRenderType> prdt = rptr_cast(CRenderType, pmesh);

		// We have a biomesh, and ought to create a good physics model for it.
		CPhysicsInfo* pphi = new CPhysicsInfoHuman();

		CAIInfo* paii = new CAIInfo(eaiRAPTOR);

		// Create a CInstance constructor structure.
		CInstance::SInit initins
		(
			CPresence3<>(),
			prdt,
			pphi,
			paii,			// Null AI.
			"Skeletal Raptor"
		);

	CAnimal* pani = new CAnimal(initins);

	wWorld.Add(pani);
#endif
}

//*********************************************************************************************
void CGUIAppDlg::Process(const CMessageNewRaster& msgnewr)
{
	// Set the new screen mode.
	bSetScreenMode(msgnewr.iMode, msgnewr.bSystemMem, msgnewr.bForce);
}

//*********************************************************************************************
void CGUIAppDlg::LoadTerrain()
{
	// This is really the handler for the convert terrain menu item!!!

	// The maximum number of characters in the terrain file name.
	#define iMAX_TERRAINNAME_CHARS (512)

	char str_filename[iMAX_TERRAINNAME_CHARS];	// The terrain file name.

	// Open the file dialog and get the terrain file name.
	bool b_ok = bGetFilename
	(
		m_hWnd,						// Window handle of parent to the dialog.
		str_filename,				// Pointer to string to put filename.
		iMAX_TERRAINNAME_CHARS,		// Maximum number of characters for the filename.
		"Convert Terrain",
		"Terrain (*.trr)\0*.trr\0All (*.*)\0*.*\0"
									// Optional extension filter.
	);

	// Abort if the user hits cancel.
	if (!b_ok)
		return;

	// Report if the file is not found.
	if (!bFileExists(str_filename))
	{
		MessageBox("Terrain file not found!", "Terrain Error", MB_OK);
		return;
	}


	bool b_old_active_state = conTerrain.bIsActive();

	if (!b_old_active_state)
		ToggleConTerrain();

	// Redraw the screen.
	PaintWindow();

	// Remove extension from filename.
	str_filename[strlen(str_filename) - 4] = 0;

	conTerrain.CloseFileSession();

	ConvertTerrainData(str_filename, CDialogTerrain::iNumQuantisationBits, conTerrain);

	conTerrain.CloseFileSession();

	MessageBox("Finished terrain conversion!", "Terrain", MB_OK);

	if (!b_old_active_state)
		ToggleConTerrain();

	// Redraw the screen.
	PaintWindow();
}


//*********************************************************************************************
void CGUIAppDlg::ExportTerrainTri(bool b_conform)
{
	// The maximum number of characters in the terrain file name.
	#define iMAX_TERRAINNAME_CHARS (512)

	char str_filename[iMAX_TERRAINNAME_CHARS];	// The terrain file name.

	// Open the file dialog and get the terrain file name.
	bool b_ok = bGetFilename
	(
		m_hWnd,						// Window handle of parent to the dialog.
		str_filename,				// Pointer to string to put filename.
		iMAX_TERRAINNAME_CHARS,		// Maximum number of characters for the filename.
		"Convert Terrain",
		"Terrain (*.trr)\0*.wtd\0All (*.*)\0*.*\0"
									// Optional extension filter.
	);

	// Abort if the user hits cancel.
	if (!b_ok)
		return;

	// Report if the file is not found.
	if (!bFileExists(str_filename))
	{
		MessageBox("Terrain file not found!", "Terrain Error", MB_OK);
		return;
	}


	bool b_old_active_state = conTerrain.bIsActive();

	if (!b_old_active_state)
		ToggleConTerrain();

	// Redraw the screen.
	PaintWindow();

	// Remove extension from filename.
	str_filename[strlen(str_filename) - 4] = 0;

	conTerrain.CloseFileSession();

	SaveTerrainTriangulation(str_filename, CDialogTerrain::fFreqCutoff, CDialogTerrain::bFreqAsRatio, b_conform, conTerrain);

	conTerrain.CloseFileSession();

	MessageBox("Finished terrain optimisation!", "Terrain", MB_OK);

	if (!b_old_active_state)
		ToggleConTerrain();

	// Redraw the screen.
	PaintWindow();
}

//*****************************************************************************************
void CGUIAppDlg::ToggleTerrainTest()
{
	static CTerrainTest* ptrrt = 0;

	bTerrainTest = !bTerrainTest;

	if (bTerrainTest)
	{
		if (!ptrrt)
			ptrrt = new CTerrainTest;
		wWorld.Add(ptrrt);
	}
	else
	{
		if (ptrrt)
			wWorld.Remove(ptrrt);
	}

	// Change the menu and redraw.
	SetMenuState();
}

//*****************************************************************************************
void CGUIAppDlg::ToggleHeightmap()
{
	// Change the menu and redraw.
	SetMenuState();
}

//*****************************************************************************************
void CGUIAppDlg::ToggleTerrainWire()
{
	// Change the menu and redraw.
	SetMenuState();
}

//*****************************************************************************************
void CGUIAppDlg::ToggleTextureWire()
{
	// Change the menu and redraw.
	SetMenuState();
}


//*****************************************************************************************
// control selection functions....
//*****************************************************************************************
// checks if the first joystick connected is the type passed in.
//
bool CGUIAppDlg::bDetectJoystick(EControlMethod ecm_stick)
{
	// ecm_joystick is ignored at the moment
	JOYINFOEX	ji_stick;

	ji_stick.dwSize=sizeof(JOYINFOEX);
	ji_stick.dwFlags=0;
	if (joyGetPosEx(JOYSTICKID1,&ji_stick)==JOYERR_NOERROR)
		return(TRUE);

	return(FALSE);
}


//****************************************************************************************
void CGUIAppDlg::OnDefaultControls()
{
	gpInputDeemone->SetControlMethod(ecm_DefaultControls);
	SetMenuState();
}


//*****************************************************************************************
void CGUIAppDlg::OnStandardJoystick()
{
	if (!bDetectJoystick(ecm_Joystick))
	{
		MessageBox("No joystick found!", "Joystick Error", MB_OK);
		return;
	}

	gpInputDeemone->SetControlMethod(ecm_Joystick);
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnAI()
{
	gaiSystem.bActive = !gaiSystem.bActive;
	
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnPhysics()
{
	pphSystem->bActive = !pphSystem->bActive;

	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::EnterGameMode()
{
	// Make the GUIApp emulate an actual game shell as much as possible!

	// Load a game GROFF file.
	// Until we have an actual game GROFF file, the user will just have to provide
	// their own game file command line argument if they want one.
	//	CLoadWorld lw(str_argument);

	// Fast render mode.
	if (!bFastMode)
	{
		ToggleFastMode();
	}

	// Start a replay recording.
	char	str_fname[MAX_PATH];

	sprintf(str_fname, "replay.rpl");

	// open the replay file now so it cannot be used for loading as well
	if (crpReplay.bOpenReplay(str_fname)==FALSE)
	{
		//
		// we cannot open the selected file, either it does not exist
		// or it is not a replay file.
		//
		SetMenuItemState(MENU_REPLAYSAVE, FALSE);
		crpReplay.SetSave(FALSE);
		//return;
	}
	else
	{
		// we now have a valid relay filename so enable replay saving
		// and tick the menu
		SetMenuItemState(MENU_REPLAYSAVE, TRUE);
		crpReplay.SetSave(TRUE);
	}

	// Player Physics on.
	if (!gpPlayer->bPhysics)
	{
		OnPlayerPhysics();
	}
				
	// Now actually start the game loop!
	SendMessage(WM_COMMAND, MENU_PLAY, 0);
}

//*********************************************************************************************
void CGUIAppDlg::OpenDepthSortProperties()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pdepthdlgProperties == 0)
	{
		pdepthdlgProperties = new CDialogDepthSort();
		pdepthdlgProperties->Create(IDD_DEPTHSORT_SETTINGS, this);
	}
	pdepthdlgProperties->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::SetWindowSize(int i_mode)
{
	//
	// Select the width and the height for the window.
	//
	int i_width  = 2 * GetSystemMetrics(SM_CXFRAME);
	int i_height = 2 * GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
			   GetSystemMetrics(SM_CYMENU);

	switch (i_mode)
	{
		case 1:
			i_width  += 320;
			i_height += 240;
			break;
		case 2:
			i_width  += 400;
			i_height += 300;
			break;
		case 3:
			i_width  += 512;
			i_height += 384;
			break;
		case 4:
			i_width  += 640;
			i_height += 480;
			break;
		default:
			Assert(0);
	}

	// Set the window size.
	SetWindowPos(&wndTop, 0, 0, i_width, i_height, SWP_SHOWWINDOW | SWP_NOMOVE);
}


//*********************************************************************************************
void CGUIAppDlg::OpenSoundProperties()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (psounddlgProperties == 0)
	{
		psounddlgProperties = new CDialogSoundProp();
		psounddlgProperties->Create(IDD_SOUND_PROPERTIES, this);
	}
	psounddlgProperties->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OnArtStats()
{
	conArtStats.SetActive(!conArtStats.bIsActive() && !bFullScreen());

	if (conArtStats.bIsActive())
	{
		// Reset the max stats.
		i_max_total_polys	= 0;
		i_max_cache_polys	= 0;
		i_max_terrain_polys	= 0;
	}

	// Set a new menu state.
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnPredictMovement()
{
	// Open dialog.
	conPredictMovement.SetActive(!conPredictMovement.bIsActive() && !bFullScreen());

	// Set a new menu state.
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OnMipMapSettings()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pdlgMipmap == 0)
	{
		pdlgMipmap = new CDialogMipmap();
		pdlgMipmap->Create(IDD_MIPMAP, this);
	}
	pdlgMipmap->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OnAlphaColourSettings()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Create the background settings dialog box.
	if (pdlgAlphaColour == 0)
	{
		pdlgAlphaColour = new CDialogAlphaColour();
	}

	// Destroy any open background settings dialog to ensure the dialog is sized properly.
	pdlgAlphaColour->DestroyWindow();

	// Create and display the background settings dialog.
	pdlgAlphaColour->Create(IDD_ALPHA_COLOUR, this);
	pdlgAlphaColour->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
// Open the texture packing window
void CGUIAppDlg::OnTexturePack()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pdlgTexturePack == 0)
	{
		pdlgTexturePack = new CDialogTexturePack();
		pdlgTexturePack->Create(IDD_TEXTUREPACK, this);
	}
	pdlgTexturePack->ShowWindow(SW_SHOWNORMAL);
}


//*********************************************************************************************
// Open the texture packing window
void CGUIAppDlg::OnMemStats()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	CDialogMemLog		pdlgMemLog;

	pdlgMemLog.DoModal();
}

//*********************************************************************************************
// Grab the first directional light.
void CGUIAppDlg::OnGiveMeALight()
{
	// Summon the light to my face, dammit!
	CWDbQueryLights wqlt;
	foreach (wqlt)
	{
		// Do the first directional light.
		if (rptr_const_dynamic_cast(CLightDirectional, (*wqlt)->prdtGetRenderInfo()))
		{
			CPlacement3<> p3 = (*wqlt)->pr3Presence();
			p3.v3Pos = CVector3<>(0, 10.0, 0) * pcamGetCamera()->pr3Presence();	// Position: in front of camera.
			(*wqlt)->Move(p3);
			Invalidate();
			break;
		}
	}
}

//*********************************************************************************************
// Toggle Z Only editing (world space up/down)
void CGUIAppDlg::OnZOnlyEdit()
{
	bZOnlyEdit = !bZOnlyEdit;
	SetMenuState();
}


//*********************************************************************************************
// Selects based on a user-entered name
void CGUIAppDlg::OnNameSelect()
{
	CDialogString ds("Object to select:");
	if (ds.DoModal() == IDOK)
	{
		CInstance* pins = 0;

		if (!strcmpi(ds.strText, "Terrain"))
		{
			CTerrain* ptrr = CWDbQueryTerrain().tGet();
			pins = ptrr;
		}
		else
		{
			if (ds.strText[0] == '#')
			{
				uint32 u4_hash;
				const char* buf = ds.strText;
				sscanf(&buf[1], "%x", &u4_hash);

				pins = wWorld.ppartPartitions->pinsFindInstance(u4_hash);
			}
			else
				pins = wWorld.ppartPartitions->pinsFindNamedInstance(std::basic_string<char>(ds.strText));
		}

		wWorld.Select(pins, true);

		PaintWindow();
	}
}

//*********************************************************************************************
// Turns smaller debug movement on and off
void CGUIAppDlg::OnSmallMoveSteps()
{
	bSmallMoveSteps = !bSmallMoveSteps;
	SetMenuState();
}


//*********************************************************************************************
// Turns terrain sounds on and off
void CGUIAppDlg::OnTerrainSound()
{
	bTerrainSound = !bTerrainSound;
	Assert(padAudioDaemon);
	padAudioDaemon->bTerrainSound = bTerrainSound;
	SetMenuState();
}


//**********************************************************************************************
//
static bool bFileExtention(char* str_fname, char* str_ext)
{
	while ((*str_fname != 0) && (*str_fname != '.'))
	{
		str_fname++;
	}

	if (*str_fname == 0)
	{
		return false;
	}

	if (stricmp(str_fname+1,str_ext) == 0)
		return true;

	return false;
}



//*********************************************************************************************
// Do that drag and drop thingy
void CGUIAppDlg::OnDropFiles(HDROP hDrop)
{
	uint32		u4_files;
	uint32		u4_count = 0;
	CLoadWorld*	plw;
	char		str_fname[MAX_PATH];

    // Determine how many objects have been dropped
    u4_files = DragQueryFile(hDrop, 0xFFFFFFFF, (LPSTR) NULL, 0);

	while (u4_files>0)
	{
		// Get the name of the file dropped.
		DragQueryFile(hDrop, u4_count, str_fname, MAX_PATH);

		if (bFileExtention(str_fname,"grf"))
		{
			//CMemCheck	mem_all("CLoadWorld", MEM_DIFF_STATS|MEM_DIFF_DUMP);

			plw = new CLoadWorld(str_fname);

			// Save the current positions to a standard file name.
			wWorld.bSaveWorld(strLAST_SCENE_FILE);

			// Show what is loaded.
			ShowLoadedFile(str_fname);

			delete plw;

			//dprintf("Allocations: %d\n", ulGetMemoryLogCounter(emlAllocCount));
		}
		else if (bFileExtention(str_fname, "scn"))
		{
			if (!wWorld.bLoadScene(str_fname))
				MessageBox("Cannot load file!", "Load Error", MB_OK);
			else
				// Remember the last loaded scene.
				strcpy(strLoadFile, str_fname);

			// Show what is loaded.
			ShowLoadedFile(str_fname);
		}
		else if (bFileExtention(str_fname, "rpl"))
		{
			bool b = crpReplay.bOpenReadReplay(str_fname);
			SetMenuItemState(MENU_REPLAYLOAD, b);
			crpReplay.SetLoad(b);
		}
		else
		{
			dprintf("Drop file not a .grf, .scn, .rpl: (%s)\n", str_fname);
		}

		u4_files--;
		u4_count++;
	}

    DragFinish(hDrop);
	
	// set the focus back to the GUIApp
	SetFocus();
	SetActiveWindow();
	return;
}

//*********************************************************************************************
void CGUIAppDlg::ToggleOcclusionConsole()
{
	conOcclusion.SetActive(!conOcclusion.bIsActive() && !bFullScreen());

	// Set a new menu state.
	SetMenuState();
}

//*********************************************************************************************
void CGUIAppDlg::OcclusionSettings()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Create the occlusion dialog box.
	if (pdlgOcclusion == 0)
	{
		pdlgOcclusion = new CDialogOcclusion();
	}

	// Destroy any open occlusion dialog to ensure the dialog is sized properly.
	pdlgOcclusion->DestroyWindow();

	// Create and display the occlusion dialog.
	pdlgOcclusion->Create(IDD_OCCLUDE, this);
	pdlgOcclusion->ShowWindow(SW_SHOWNORMAL);
}


//*********************************************************************************************
// Toggles the sky rather than disable it!
void CGUIAppDlg::OnSkyDisable()
{
	prenMain->pSettings->bDrawSky = !prenMain->pSettings->bDrawSky;
	SetMenuItemState(MENU_SKY_DISABLE,	prenMain->pSettings->bDrawSky);
}


//*********************************************************************************************
// Remove the whole sky and the dialog
void CGUIAppDlg::OnSkyRemove()
{
	delete pdlgSky;
	pdlgSky = NULL;

	CSkyRender::RemoveSky();
	SetMenuItemState(MENU_SKY_TEXTURE,	true);
}


//*********************************************************************************************
// Toggles the sky between texture and flat modes, cannot be changed unless there is a sky
// loaded.
void CGUIAppDlg::OnSkyTexture()
{
	if (gpskyRender == NULL)
		return;

	bool b_texture = !gpskyRender->bSkyTextured();

	if (b_texture)
		gpskyRender->SetDrawMode(CSkyRender::sdmTextured);
	else
		gpskyRender->SetDrawMode(CSkyRender::sdmGradient);

	SetMenuItemState(MENU_SKY_TEXTURE,	b_texture);
}


//*********************************************************************************************
// Toggles between filling empty space or not, default is not. This cannot be changed unless 
// there is a sky loaded.
void CGUIAppDlg::OnSkyFill()
{
	if (gpskyRender==NULL)
		return;

	bool b_fill = !gpskyRender->bSkyFill();

	gpskyRender->SetFilled(b_fill);
	SetMenuItemState(MENU_SKY_FILL,	b_fill);
}


//*********************************************************************************************
void CGUIAppDlg::OnSkySettings()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (gpskyRender == NULL)
		return;

	// Create the sky dialog box.
	if (pdlgSky == 0)
	{
		pdlgSky = new CDialogSky();
		// Create and display the occlusion dialog.
		pdlgSky->Create(IDD_SKY, this);
	}
	pdlgSky->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OpenGunSettings()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Create the occlusion dialog box.
	if (pdlgGun == 0)
	{
		pdlgGun = new CDialogGun();
	}

	// Destroy any open occlusion dialog to ensure the dialog is sized properly.
	pdlgGun->DestroyWindow();

	// Create and display the occlusion dialog.
	pdlgGun->Create(IDD_GUN, this);
	pdlgGun->ShowWindow(SW_SHOWNORMAL);
}


//*****************************************************************************************
void CGUIAppDlg::OpenSchedulerSettings()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Create the occlusion dialog box.
	if (pdlgScheduler == 0)
	{
		pdlgScheduler = new CDialogScheduler();
	}

	// Destroy any open occlusion dialog to ensure the dialog is sized properly.
	pdlgScheduler->DestroyWindow();

	// Create and display the occlusion dialog.
	pdlgScheduler->Create(IDD_SCHEDULER, this);
	pdlgScheduler->ShowWindow(SW_SHOWNORMAL);
}

//*****************************************************************************************
char *CGUIAppDlg::pcSave(char *pc_buffer) const
{
	return pc_buffer;
}

//*****************************************************************************************
const char*CGUIAppDlg::pcLoad(const char* pc_buffer)
{
	return pc_buffer;
}

//*********************************************************************************************
void CGUIAppDlg::OnEditAI()
{
	static CAIDialogs2Dlg* paidlg = 0;

	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	CAnimal* pani = ptCast<CAnimal>(gaiSystem.pinsSelected);

	if (!pani)
		return;

	// Create the AI DIALOG box.
	if (paidlg == 0)
	{
		paidlg = new CAIDialogs2Dlg();
	
		// Create and display the ai dialog
		paidlg->Create(IDD_AIDIALOGS2_DIALOG, this);
	}
	else
	{
		paidlg->ShowWindow(SW_SHOWNORMAL);
	}
}

//*********************************************************************************************
void CGUIAppDlg::OnEditAIEmotions()
{
	static CParameterDlg* pParameterDialog = 0;

	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	CAnimal* pani = ptCast<CAnimal>(pipeMain.ppartLastSelected());
	if (!pani)
		return;

	if (pParameterDialog == 0)
	{
		// Lastly, make a parameters dialog.
		pParameterDialog = new CParameterDlg();
		pParameterDialog->Create(IDD_PARAMETER_DIALOG, this);
	}

	CBrain* pbr = 0;
	pbr = pani->pbrBrain;

	AlwaysAssert(pbr);

	pParameterDialog->SetFeeling(&pbr->pmsState->feelEmotions, 0.0f, 1.0f);

	pParameterDialog->ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
void CGUIAppDlg::OnVMSettings()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Create the sky dialog box.
	if (pdlgVM == 0)
	{
		pdlgVM = new CDialogVM();
		// Create and display the vm dialog
		pdlgVM->Create(IDD_VIRTUALMEM, this);
	}
	pdlgVM->ShowWindow(SW_SHOWNORMAL);
}


//*********************************************************************************************
void CGUIAppDlg::OnSoundMaterialProp()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	if (pdlgSoundMaterial == 0)
	{
		pdlgSoundMaterial = new CDialogSoundMaterial();
		// Create and display the vm dialog
		pdlgSoundMaterial->Create(CDialogSoundMaterial::IDD, this);
	}

	// Do nothing if no object is currently selected.
	CPartition* ppart = pipeMain.ppartLastSelected();
	if (ppart == 0)
		return;

	CInstance* pins = ptCast<CInstance>(ppart);
	if (pins == NULL)
		return;
	//
	// If the currently selected object has a light attached, edit the light
	// properties.
	//
	CInstance* pins_light = pipeMain.pinsGetLight(ptCast<CInstance>(ppart));
	if (pins_light)
		return;

	pdlgSoundMaterial->SetInstance(pins);
	pdlgSoundMaterial->ShowWindow(SW_SHOWNORMAL);
}


//*********************************************************************************************
void CGUIAppDlg::OnChangeSMat()
{
	// Do nothing if no object is currently selected.
	CPartition* ppart = pipeMain.ppartLastSelected();
	if (ppart == 0)
		return;

	CInstance* pins = ptCast<CInstance>(ppart);
	if (pins == NULL)
		return;
	//
	// If the currently selected object has a light attached, edit the light
	// properties.
	//
	CInstance* pins_light = pipeMain.pinsGetLight(ptCast<CInstance>(ppart));
	if (pins_light)
		return;

	CDialogString ds("Change Object Sound Material");
	if (ds.DoModal() == IDOK)
	{
		CString	str;
		char buf[256];

		str = "Are you sure you want to change sound material of obejct '";
		str += pins->strGetInstanceName();
		str += "' to '";
		str += ds.strText;
		str += "' (ID = ";

		TSoundHandle sndhnd = sndhndHashIdentifier(ds.strText);
		wsprintf(buf,"%x",(uint32)sndhnd);

		str += buf;
		str += ")?";

		if (MessageBox(str, "Are you sure", MB_YESNO|MB_ICONQUESTION) == IDYES)
		{
			// change the material ID..
			CPhysicsInfo* pphi = (CPhysicsInfo*)pins->pphiGetPhysicsInfo();
			if (pphi)
			{
				pphi->SetMaterialType(sndhnd);
			}
		}
	}
}


void CGUIAppDlg::OnTeleport()
{
    CDialogTeleport     dlg;

    dlg.DoModal();
}

//*********************************************************************************************
// Bump packing has to be modal because the renderer must not be going while we change the
// bump maps!
void CGUIAppDlg::OnBumpPacking()
{
	CDialogBumpPack	dlgPacker;

	dlgPacker.DoModal();
}


//*********************************************************************************************
// Open the dialog that controls texture packing
void CGUIAppDlg::OnDropMarker()
{
#if VER_TEST
	extern CInstance* pinsMasterMarker;
	extern int iMaxMarker;
	extern char strMarkerName[256];

	if (!pinsMasterMarker)
		return;

	static int i_last_marker = 0;

	char buffer[256];
	
	int i_safe;
	for (i_safe = iMaxMarker; i_safe >= 0; --i_safe)
	{
		++i_last_marker;
		if (i_last_marker > iMaxMarker)
			i_last_marker = 1;

		sprintf(buffer, "%s%02d", strMarkerName, i_last_marker);

		CInstance* pins = wWorld.pinsFindInstance(u4Hash(buffer));

		if (pins)
		{
			CInstance* pins_target = pcamGetCamera()->pinsAttached() ? pcamGetCamera()->pinsAttached() : pcamGetCamera();

			pins->Move(pins_target->pr3Presence());
			return;
		}
	}
	
	
	AlwaysAssert(i_safe >= 0);


#endif
}

//*********************************************************************************************
// Open the dialog that controls texture packing
void CGUIAppDlg::OnPackOptions()
{
	CDialogTexturePackOptions	dlg;
	dlg.DoModal();
}


// Porting code.
void* hwndGetMainHwnd()
{
	AlwaysAssert(pgui);
	return pgui->m_hWnd;
}

// Porting code.
HINSTANCE hinstGetMainHInstance()
{
	return AfxGetApp()->m_hInstance;
}

// Porting code.
void ResetAppData()
{
	// Clears all data that needs clearing on a world dbase reset.
	
	// Clear the pipeline selected stuff.
	pipeMain.lsppartSelected.erase(pipeMain.lsppartSelected.begin(), pipeMain.lsppartSelected.end());
}


//*********************************************************************************************
void CGUIAppDlg::InvokeCullingDialog()
{
	CDialogCulling dlgcull;
	dlgcull.DoModal();
}


//*********************************************************************************************
void CGUIAppDlg::OnRenderQualitySettings()
{
	// Don't do anything unless in debug mode.
	if (!bCanOpenChild())
		return;

	// Create the sky dialog box.
	if (pdlgQuality == 0)
	{
		pdlgQuality = new CDialogQuality();
		// Create and display the vm dialog
		pdlgQuality->Create(IDD_QUALITY, this);
	}
	pdlgQuality->ShowWindow(SW_SHOWNORMAL);
}

void CGUIAppDlg::ExitApp()
{
	if (bExiting)
		return;

	bExiting = true;

	// Go back to startup path.
	CPushDir pshd(strStartupPath());

	void SaveMenuSettings(char *);
	//SaveMenuSettings("options.txt");
	SaveMenuSettings("lastopt.txt");

	// Stop the timer.
	KillTimer(1);

	// delete the sky class and the associated texture etc
	CSkyRender::RemoveSky();

	// shut the performance system
	PSClose();

	std::destroy_at(&prasMainScreen);
}

BOOL CGUIAppDlg::DestroyWindow() 
{
	ExitApp();
	return CDialog::DestroyWindow();
}


//*********************************************************************************************
void CGUIAppDlg::StatsDisplay()
{
	//
	// Handle stat display, in a prioritised manner.  If we are windowed, we show each console
	// in its own window, and other stats on screen.  If we are fullscreen, we show only one
	// console or stat on screen.
	//

	//
	// Create any stats needed.
	//

	CCycleTimer ctmr_text;

	DisplayCacheStats();

	// Set terrain stat averaging to current menu state.
	CTerrain* ptrr = CWDbQueryTerrain().tGet();

	if (ptrr != 0)
		ptrr->SetStatAveraging(bAvgStats);


	bool b_showed_on_screen = false;

	if (bIsFullScreen)
	{
		// Show any output needed directly on screen.
		if (bShowConsole)
		{
			prnshMain->ShowConsoleOnScreen(conStd);
			b_showed_on_screen = true;
		}
		else if (bShowHardwareStats)
		{
			int i_hw_total_kb   = d3dDriver.iGetTotalTextureMem() >> 10;
			int i_hw_sky_kb     = 128;
			int i_hw_water_kb   = CEntityWater::iGetManagedMemSize() >> 10;
			int i_hw_terrain_kb = NMultiResolution::CTextureNode::ptexmTexturePages->iGetManagedMemSize() >> 10;
			int i_hw_caches_kb  = CRenderCache::iGetHardwareLimit() >> 10;
			int i_hw_buffer_kb  = i_hw_total_kb - (i_hw_sky_kb + i_hw_water_kb + i_hw_terrain_kb + i_hw_caches_kb);

			conHardware.SetActive(true, false);
			conHardware.ClearScreen();
			conHardware.Print("\n\n\n");
			conHardware.Print("Vid Mem Total   : %ld\n", i_hw_total_kb);
			conHardware.Print("Vid Lim Sky     : %ld\n", i_hw_sky_kb);
			conHardware.Print("Vid Lim Water   : %ld\n", i_hw_water_kb);
			conHardware.Print("Vid Lim Terrain : %ld\n", i_hw_terrain_kb);
			conHardware.Print("Vid Lim Cache   : %ld\n", i_hw_caches_kb);
			conHardware.Print("Vid Buffer      : %ld\n", i_hw_buffer_kb);
			conHardware.Print("\n");
			conHardware.Print("Vid Mem Sky     : %ld\n", i_hw_sky_kb);
			conHardware.Print("Vid Mem Water   : %ld\n", CEntityWater::iGetManagedMemUsed() >> 10);
			conHardware.Print("Vid Mem Terrain : %ld\n", NMultiResolution::CTextureNode::ptexmTexturePages->iGetManagedMemUsed() >> 10);
			conHardware.Print("\n");
			conHardware.Print("Vid Mem Cache   : %ld\n", CRenderCache::iGetHardwareMem()   >> 10);
			conHardware.Print("All Mem Cache   : %ld\n", CRenderCache::iGetTotalMem()      >> 10);
			conHardware.Print("All Lim Cache   : %ld\n", CRenderCache::iGetLimitMem()      >> 10);
			conHardware.Print("\n");
			proHardware.psHardware.WriteToConsole(conHardware);

			prnshMain->ShowConsoleOnScreen(conHardware);
			b_showed_on_screen = true;
		}
		else if (bShowStats)
		{
			conProfile.SetActive(true, false);
			conProfile.ClearScreen();
			proProfile.psMain.WriteToConsole(conProfile);

			prnshMain->ShowConsoleOnScreen(conProfile);
			b_showed_on_screen = true;
		}

		if (!bAvgStats)
		{
			// If averaging is on, let stats accumulate.  Otherwise, reset them.
			proProfile.psFrame.Reset();
			proHardware.psHardware.Reset();
			proHardware.psHardware.Add(0, 1);
		}
	}

	// Clear screen if required.
	if (!b_showed_on_screen && bShowFPS || rcstCacheStats.bKeepStats)
	{
		conHardware.ClearScreen();
		conProfile.ClearScreen();
		b_showed_on_screen = true;
	}

#if VER_TIMING_STATS
	// Show FPS.
	if (bShowFPS)
	{
		char buf[20];

		// Show FPS stats based on the amout of time elapsed since we were last here.
		sprintf(buf, "FPS: %3.1f", 1.0 / ((double)ctmrFPS() * (double)ctmrFPS.fSecondsPerCycle()));
		prnshMain->PrintString(buf);
	}
#endif

//	_asm
//	{
//		_emit 0x0f
//		_emit 0x31
//		mov DWORD PTR [i8FrameStop+0],eax
//		mov DWORD PTR [i8FrameStop+4],edx
//	}
//	__int64 i8_elapsed = i8FrameStop-i8FrameStart;
//	i8FrameStart = i8FrameStop;
//	char buf[20];
//	// Show FPS stats based on the amout of time elapsed since we were last here.
//	sprintf(buf, "FPS: %3.1f", 1.0 / ((double)i8_elapsed * 0.000000005));
//	prnshMain->PrintString(buf);



	int i_remember_num_caches = iNumCaches();

	// Display text.
	if (b_showed_on_screen)
		prnshMain->ShowConsoleOnScreen(conProfile);

	proProfile.psText.Add(ctmr_text());

	//
	// Show specified consoles in their own window.
	//

	ctmr_text.Reset();

	if (!bIsFullScreen)
	{
		// Test camera movement prediction.
		if (conPredictMovement.bIsActive())
		{
			// Test prediction.
			TestPrediction(pcamGetCamera()->ppmGetPrediction());

			// Display results.
			conPredictMovement.Show();
			conPredictMovement.ClearScreen();
		}

		// Occlusion console.
		conocOcclusion.Show();

		if ((int)proProfile.psFrame.fCountPer() % 5 == 0)
		{
			if (bShowConsole)
			{
				// Create dialog if needed.
				conStd.SetActive(true);
				conStd.Show();
			}

			else if (bShowStats)
			{
				// Activate profile dialog.
				conProfile.SetActive(true);
				conProfile.ClearScreen();
				proProfile.psMain.WriteToConsole(conProfile);

				conProfile.Show();
			}

			// Update all system consoles, if active.
			conAI.Show();
			conPhysics.Show();
			conShadows.Show();
			
			if (conArtStats.bIsActive())
			{
				float f_frames = 5.0f;		// Frames over which stats are accumulated.
				int i_temp = 0;

				conArtStats.ClearScreen();
				
				// Start with constant data.
				conArtStats.Print("Screen Mode: \t\t%dx%dx%d\n",	
					prasMainScreen->iWidth,
					prasMainScreen->iHeight,
					prasMainScreen->iPixelBits);

				CCamera* pcam = pcamGetCamera();
				CVector3<> v3_cam = pcam->v3Pos();
				conArtStats.Print("Camera position: %f\t%f\t%f\n", v3_cam.tX, v3_cam.tY, v3_cam.tZ);
				

#define iMAX_OBJECT_POLYGONS 500
#define iMAX_TOTAL_POLYGONS 1200

				extern CProfileStat		psTerrain;

				int i_total_polys = (float(proProfile.psDrawPolygon.iGetCount()) / f_frames) + 0.5f; 
//				int i_cache_polys = i_remember_num_caches;
				int i_cache_polys = (psCacheSched.iGetCount() / f_frames) + 0.5f;
				int i_terrain_polys = float(psTerrain.iGetCount()) / f_frames  + 0.5f;

				int i_object_polys = i_total_polys - i_cache_polys - i_terrain_polys;

				// Make sure our various cache stats match within one polygon.
				// psCacheUsed doesn't seem to match i_cache_polys.
//				Assert(psCacheUse.iGetCount() >= (i_cache_polys-1) * 5);
//				Assert(psCacheUse.iGetCount() <= (i_cache_polys+1) * 5);

//				psCacheUse.Reset();

				
				if (i_max_total_polys < i_total_polys)
					i_max_total_polys = i_total_polys;
				
				conArtStats.Print("Total polys: \t\t%d of %d allowed, %d%% (M %d)\n", 
					i_total_polys, 
					iMAX_TOTAL_POLYGONS, 
					int((100 * i_total_polys) / (iMAX_TOTAL_POLYGONS)),
					i_max_total_polys);

				if (i_max_cache_polys < i_cache_polys)
					i_max_cache_polys = i_cache_polys;
				if (i_max_terrain_polys < i_terrain_polys)
					i_max_terrain_polys = i_terrain_polys;


				conArtStats.Print("Cache polys: \t\t%d (M %d)\nTerrain polys: \t\t%d (M %d)\n", 
					i_cache_polys, 
					i_max_cache_polys,
					i_terrain_polys,
					i_max_terrain_polys);

				conArtStats.Print("Object Polys: \t\t%d of %d allowed, %d%%\n", 
					i_object_polys, 
					iMAX_OBJECT_POLYGONS, 
					(i_object_polys * 100) / iMAX_OBJECT_POLYGONS);

#define iALLOWED_TEXTURE_MEM_KB 4096
#ifdef LOG_MEM
				conArtStats.Print("Num textures: \t\t%d\n", ulGetMemoryLogCounter(emlTextureCount) +
														ulGetMemoryLogCounter(emlBumpMapCount) );
				int i_num_unpacked = ulGetMemoryLogCounter(emlBumpNoPack) + ulGetMemoryLogCounter(emlTextureNoPack);
				int i_bumpmap_mem_bytes = ulGetMemoryLogCounter(emlTexturePages32) * (512*1024);
				int i_texture_mem_bytes = ulGetMemoryLogCounter(emlTexturePages8) * (128*1024);
				int i_unpacked_mem_bytes = ulGetMemoryLogCounter(emlBumpNoPackMem) + ulGetMemoryLogCounter(emlTextureNoPackMem);
				int i_total_mem_bytes = i_bumpmap_mem_bytes + i_texture_mem_bytes + i_unpacked_mem_bytes;
					

				conArtStats.Print("Total texture mem: \t%1.3f of %1.3f MB, %d%%\n", 
					float(i_total_mem_bytes) / (1024.0f * 1024.0f), 
					float(iALLOWED_TEXTURE_MEM_KB) / 1024.0f, 
					float(i_total_mem_bytes) / (1024.0f * float(iALLOWED_TEXTURE_MEM_KB)));

				conArtStats.Print("Mem for bumpmaps: \t%1.3f MB\n", 
					(i_bumpmap_mem_bytes + ulGetMemoryLogCounter(emlBumpNoPackMem)) / (1024.0f * 1024.0f));

				conArtStats.Print("Unpacked textures: \t%d bytes in %d surfaces\n", i_unpacked_mem_bytes, i_num_unpacked);


#else  // LOG_MEM
				conArtStats.Print("No texture data available.");
#endif // LOG_MEM


				extern int iNumClipped;
				conArtStats.Print("Split polys: \t\t%ld\n", iNumClipped);

				static int i_worst_cache = 0;
				conArtStats.Print("Cache update time: \t%dms\n", int(psCacheSched.fSeconds() * 1000));
				if (psCacheSched.fSeconds() * 1000 > i_worst_cache)
					i_worst_cache = psCacheSched.fSeconds() * 1000;

				conArtStats.Print("Worst Cache update time:%dms\n", i_worst_cache);

				conArtStats.Print("FPS: \t\t\t%2.1f\n", 1.0 / proProfile.psFrame.fSecondsPerCount());

				// Print transparency stats.
				if (iNumPixelsIterated > 0)
				{
					float f_percentage_transparent = float(iNumPixelsIterated - iNumPixelsSolid) /
						                             float(iNumPixelsIterated) * 100.0f;
					conArtStats.Print("Percentage Transparent: %1.1f\n", f_percentage_transparent);
				}
				// Clear stats.
				iNumPixelsIterated = 0;
				iNumPixelsSolid    = 0;

				// Now show the stats.
				conArtStats.Show();

				psTerrain.Reset();


			}

			if (!bAvgStats)
				// If averaging is on, let stats accumulate.  Otherwise, reset them.
				proProfile.psFrame.Reset();
		}
		conTerrain.Show();
	}

	// Show stats for depth sorting.
	DepthSortStatsWriteAndClear();

	proProfile.psText.Add(ctmr_text());
}

int iSkipFrames = 100;

extern CProfileStat psExecuteCaches;
extern CProfileStat psExecuteTerrain;

class CFrameTime
{
	float fFrameTime;
	float fOther;
	int32 i4TickCount;
public:

	CFrameTime()
	{
	}

	~CFrameTime()
	{
		if (fileStats)
		{
			fclose(fileStats);
			fileStats = 0;
		}
	}

	void SetCount()
	{
		i4TickCount = GetTickCount();
	}

	void Begin()
	{
		fFrameTime = float(int32(GetTickCount()) - i4TickCount) / 1000.0f;
		if (fFrameTime < 0.0f)
			fFrameTime = 0.1f;

		fOther = fFrameTime;
		
		fprintf(fileStats, "%1.5f", fFrameTime);
	}

	void Print(float f)
	{
		fOther -= f;
		fprintf(fileStats, ",%1.5f", f);
	}

	void Print(const CProfileStat& stat)
	{
		float f = stat.fSeconds();
		fOther -= f;
		Print(f);
	}

	void End()
	{
		//fprintf(fileStats, ",%1.5f", fOther);
		fprintf(fileStats, "\n");
		proProfile.psFrame.Reset();
		SetCount();
	}

};

CFrameTime Frame;

//*********************************************************************************************
void CGUIAppDlg::StatsDump()
{
	if (iSkipFrames > 1)
		iSkipFrames = 1;
	if (iSkipFrames > 0)
	{
		--iSkipFrames;
		proProfile.psFrame.Reset();
		if (iSkipFrames == 0)
		{

			if (!fileStats)
				fileStats = fopen("Stats.csv", "wt");
			fprintf(fileStats, "Total,Physics,AI,Step,Cache Bld,Terr Tex,Special\n");
		}
		Frame.SetCount();
		return;
	}

	Frame.Begin();

	float f_step = proProfile.psStep.fSeconds() - proProfile.psPhysics.fSeconds() - proProfile.psAI.fSeconds();

	Frame.Print(proProfile.psPhysics);
	Frame.Print(proProfile.psAI);
	Frame.Print(f_step);
	Frame.Print(psExecuteCaches);
	Frame.Print(psExecuteTerrain);
	Frame.Print(psOTHER_STAT);

	Frame.End();

	proProfile.psPhysics.Reset();
	proProfile.psAI.Reset();
//	proProfile.psStepOther.Reset();
	psExecuteCaches.Reset();
	psExecuteTerrain.Reset();
	psOTHER_STAT.Reset();
}

//*********************************************************************************************
uint32 u4LookupResourceString(int32 i4_id,char* str_buf,uint32 u4_buf_len)
{
	char  buf[1024];

    int i_res = LoadString(AfxGetInstanceHandle(), IDS_STR_HINTS + i4_id, buf, 1024);

	if ((i_res>0) && (i_res<(int)u4_buf_len))
	{
		// copy the string to the destination buffer and process and escape sequences
		wsprintf(str_buf,buf);

		Assert(strlen(str_buf)<u4_buf_len);

		return strlen(str_buf);
	}

	return 0;
}