/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		The main dialog box for the graphic user interface.
 *
 * Bugs:
 *		In GUIApp, in fullscreen flipped mode, no menus or mouse are drawn.  In any fullscreen
 *		mode, no dialogs appear.
 *
 * To do:
 *
 * Notes:
 *		Most menu handling is done here, rather that in the application module.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/GUIAppDlg.h                                                   $
 * 
 * 167   9/08/98 3:16p Mlange
 * Implemented dragging rectangle select.
 * 
 * 166   8/30/98 4:50p Asouth
 * removed file scope
 * 
 * 165   98/08/28 19:49 Speter
 * Added wonderful, beautiful Gore dialog.
 * 
 * 164   8/21/98 9:08a Shernd
 * Added The Teleport Dialog
 * 
 * 163   8/13/98 10:18p Pkeet
 * Added a function for dumping stats to a csv file.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_GUIAPPDLG_HPP
#define HEADER_GUIAPP_GUIAPPDLG_HPP

//
// Includes.
//

#include "Lib/View/Colour.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Sys/ProcessorDetect.hpp"
#include "Lib/Sys/Timer.hpp"
#include "Lib/Renderer/RenderDefs.hpp"
#include "Lib/Control/Control.hpp"

//
// Opaque declarations.
//

class CMagnet;
class CMagnetPair;

//
// Defines.
//

// Default width and height of the view surface of GUIApp.
#define iGUIAPP_DEFAULT_OFFSET   (0)
#define iGUIAPP_DEFAULT_SIZEMODE (3)


//*********************************************************************************************
//
class CGUIAppDlg : public CDialog, public CEntity
//
// The main application dialog box.
//
// Prefix: guidlg
//
//**************************************
{
public:

	CColour clrBackground;		// The current background colour.
	int		iScreenMode;		// Current screen mode index, or -1 for windowed.
	bool	bSystemMem;		// True if back buffer should be in system mem, else video mem.
	bool    bFastModeRequest;	// 'True' if a request is pending for the fast render mode.
	bool    bGameModeRequest;	// 'True' if a request is pending for the game sim mode.
	bool	bReplayRequest;		// true if a replay is to be played
	bool	bShowToolbar;		// Display toolbar at startup.
	char    acReplayFileName[MAX_PATH];   // The file to replay.
	bool	bLoadRequest;		// true if we want to load a groff file at start.
	char	strLoadFile[MAX_PATH];		// the groff file to load at start.
	char    strConvertTRRFileName[MAX_PATH];   // The terrain .trr file to convert.

//
	// Don't protect all this, as we may need to look at it or set flags
	// instead of merely toggling them.
//protected:
	
	RECT     rectWindowedPos;	// The last position and size of the window before going into
								// full screen mode.
	HICON    m_hIcon;			// The main application icon.
	bool     bLeftCapture;		// Set to 'true' if message capture initiated with the left
								// mouse button.
	bool     bRightCapture;		// Set to 'true' if message capture initiated with the right
								// mouse button.
	bool     bDragRect;			// Whether dragging rectangle selection is currently active.
	CPoint   pntCaptureMouse;	// The position of the mouse when capturing started.
	CPoint   pntCaptureCurrent;	// The current position of the mouse when capturing.
	CConsoleEx	conProfile;		// The console to use for profile stats.
	bool     bIsFullScreen;		// Set to 'true' if the application is in full screen mode.
	bool     bGDIMode;			// Set to 'true' if the application is using GDI.
	bool	 bShowConsole;		// Show generic console window.
	bool	 bShowStats;		// Show profiling stats.
	bool     bShowHardwareStats;	// Show hardware profile stats.
	bool	 bAvgStats;			// Whether to avg stats over time, rather than frame-by-frame.
	bool	 bShowFPS;			// Whether to show the frames-per-second on screen.
	bool     bShowCacheStats;	// Flag determines if caching statistics are displayed on screen.
	bool	 bShowHairs;		// Whether to always show cross-hairs; else only when mouse clicked.
	bool	 bCrosshairRadius;	// Whether to limit crosshairs to within a distance of the camera; else all crosshairs
	TReal	 rCrosshairRadius;	// The distance at which crosshairs turn off if bCrosshairRadius is set.
	bool	 bCrosshairVegetation;// Whether to show crosshairs on "V" objects; else don't display crosshairs on "V" objects
	bool	 bShowSpheres;		// Whether to show objects's bounding spheres.
	bool	 bShowWire;			// Whether to show objects's wireframe.
	bool	 bShowPinhead;		// Whether to show objects's vertex normals.
	bool	 bJustMoved;		// Flag enabled just after mouse moves objects; disables bones, etc.
	CSet<ERenderFeature> erfPrevious;	// Render flags prior to switching to fast mode.
	CCamera::SProperties campropProp;	// Camera properties prior to going into fast render mode.
	bool     bFastMode;			// 'True' if the renderer is in fast mode.
	bool	 bDelaunayTest;		// 'True' Delaunay test code is active.
	bool     bTerrainTest;		// If 'true' the terrain test code is active.
	bool	 bCameraFloating;	// true if camera is be restricted to exactly 2m above terrain height field.
	bool	 bWander;			// Camera wanders around aimlessly.
	bool	 bAllowDebugMovement;	// true if debug movement keys are allowed.
	bool	 bSmallMoveSteps;	// Smaller debug movement.
	bool	 bZOnlyEdit;		// Constrain mouse object move to world Z axis.
	bool	 bTerrainSound;		// True if terrain makes a sound.
	bool	 bConvertTerrain;	// True if terrain .trr file must be converted at startup.
	bool	 bRotateAboutWorldZ;	// Debug movement keys rotate about world z instead of local z.
	bool	 bPhysicsStep;		// Run in physics-step increments.

public:

	//*****************************************************************************************
	//
	// Constructor
	//

	// AppWizard generated constructor.
	CGUIAppDlg(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void SetMenuItemState
	(
		int i_id,						// ID of menu item to modify.
		bool b_checked,					// Whether item should be checked.
		bool b_enabled = true			// Whether item should be enabled.
	);
	//
	// Sets checkmarks and enabling flags for menu items associated with rendering states.
	//
	//**********************************

	//*****************************************************************************************
	//
	void SetRenderFeatureState
	(
		int i_id,						// ID of menu item to modify.
		ERenderFeature erf				// Which render feature it corresponds to.
	);
	//
	// Sets checkmarks and enabling flags for menu items associated with rendering states.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetMenuState
	(
	);
	//
	// Sets checkmarks for movement mode and movement type settings in the main menu, and
	// changes the toolbar button states to match.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SendChildrenMessage
	(
		uint u_message	// Message to send to children.
	);
	//
	// Sends a message to all open child dialog boxes.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetGDIMode
	(
		bool b_gdimode // State to set the GDI flag to.
	);
	//
	// Switches to and from GDI mode when the application is in full screen.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bCanOpenChild
	(
	);
	//
	// Returns 'true' if a child dialog box can be opened, otherwise returns false.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ShowToolbar
	(
		bool b_show = true	// Shows the toolbar if 'true,' otherwise hides the toolbar.
	);
	//
	// Display the main toolbar.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bSetScreenMode
	(
		int i_screen_mode,				// Screen mode index.
		bool b_system_mem,				// Whether to create in system memory.
		bool b_force = false			// Always create, even if same mode.
	);
	//
	// Sets the screen mode according to the menu command.
	//
	// Returns 'true' if the menu action was successful.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bSetRenderer
	(
		uint u_id	// Menu command.
	);
	//
	// Sets the renderer according to the menu command.
	//
	// Returns 'true' if the menu action was successful.
	//
	//**************************************

	//*****************************************************************************************
	//
	void EditObject
	(
	);
	//
	// Edits an object, a light or the camera.
	//
	//**************************************

	//*****************************************************************************************
	//
	void OpenLightProperties
	(
		rptr<CLight> plt					// Pointer to the light.
	);
	//
	// Edits the properties of a light.
	//
	//**************************************

	//*****************************************************************************************
	//
	void OpenCameraProperties
	(
	);
	//
	// Edits the properties of the camera.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void OpenPerspectiveDialog
	(
	);
	//
	// Edits perspective correction properties.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void OpenTerrainDialog
	(
	);
	//
	// Edits terrain properties.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void OpenWaterDialog
	(
	);
	//
	// Edits water properties.
	//
	//**************************************
	
	//*****************************************************************************************
	//
	void OpenRenderCacheDialog
	(
	);
	//
	// Edits render cache properties.
	//
	//**************************************

	//*****************************************************************************************
	//
	void OpenObjectDialogProperties
	(
		CInstance* pins	// Pointer to the instance of the object to look at.
	);
	//
	// Edits the properties of an object.
	//
	//**************************************

	//*****************************************************************************************
	//
	void OpenMagnetProperties
	(
		CMagnetPair* pmp
	);
	//
	// Edits the properties of a magnet.
	//
	//**************************************

	//*****************************************************************************************
	//
	void OpenPhysicsDialogProperties
	(
	);
	//
	// Edits the properties of an object's physics.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddObject
	(
	);
	//
	// Adds a default object into the scene.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddDirectionalLight
	(
	);
	//
	// Adds a directional light into the scene.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddPointLight
	(
	);
	//
	// Adds a point light into the scene.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AddPointDirectionalLight
	(
	);
	//
	// Adds a point directional light into the scene.
	//
	//**************************************

	//*****************************************************************************************
	//
	void EditBackground
	(
	);
	//
	// Invokes the background editing dialog box.
	//
	//**************************************

	//*****************************************************************************************
	//
	void OpenFogProperties
	(
	);
	//
	// Invokes the fog dialog box.
	//
	//**************************************

	//*****************************************************************************************
	//
	void OpenDepthSortProperties
	(
	);
	//
	// Invokes the depth sort dialog box.
	//
	//**************************************

	//*****************************************************************************************
	//
	void UpdateBackground
	(
	);
	//
	// Sets the screen to the background colour.
	//
	//**************************************

	//*****************************************************************************************
	//
	void FPSEstimate
	(
	);
	//
	// Runs a frames per second test.
	//
	//**************************************

	//*****************************************************************************************
	//
	void MoveCameraToSceneCentre
	(
	);
	//
	// Moves the main camera to the physical centre of the currently loaded scene.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ToggleFastMode
	(
	);
	//
	// Sets flags, cameras and so on to a fast render mode.
	//
	//**************************************

	//*****************************************************************************************
	//
	void MoveCameraToInitPosition
	(
	);
	//
	// Sets the camera position to the default, or command-line driven, initial position.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ToggleDelaunayTest();
	//
	// Toggle Delaunay test code on/off.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ToggleTerrainTest();
	//
	// Toggle the terrain test code on/off.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ToggleConPhysics();
	//
	// Toggles the  console.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ToggleConShadows();
	//
	// Toggles the  console.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ToggleConTerrain();
	//
	// Toggles the  console.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ToggleConAI();
	//
	// Toggles the  console.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ToggleConDepthSort();
	//
	// Toggles the  console.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ToggleOcclusionConsole();
	//
	// Toggles the occlusion console.
	//
	//**************************************

	//*****************************************************************************************
	//
	void LoadTerrain();
	//
	// Loads terrain data from a file.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ExportTerrainTri(bool b_conform);
	//
	// Export terrain triangulation data to a file.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bFullScreen
	(
	)
	//
	// Returns 'true' if GUIApp is in full screen mode.
	//
	//**************************************
	{
		return bIsFullScreen;
	}

	//*****************************************************************************************
	//
	void ToggleTerrainWire();
	//
	// Toggles the wireframe mode for the terrain.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ToggleHeightmap();
	//
	// Toggles the heightmap mode for the terrain.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ToggleTextureWire();
	//
	// Toggles the heightmap mode for the terrain.
	//
	//**************************************

	//*****************************************************************************************
	//
	void EnterGameMode();
	//
	// Brings GUIApp into the state most resembling the actual final game mode.
	//
	//**************************************

	//*****************************************************************************************
	void AddTestBumpmap();

	//*****************************************************************************************
	void PaintWindow();

	//*****************************************************************************************
	//
	void Process
	(
		const CMessageNewRaster& msgnewr
	) override;
	//
	// Processes CMessageNewRaster messages.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetWindowSize
	(
		int i_mode
	);
	//
	// Sets the windowed size to:
	//
	//		Mode 1: 400 x 300
	//		Mode 2: 512 x 384
	//		Mode 3: 640 x 480
	//
	// This function does nothing in full screen mode.
	//
	//**************************************


	//*****************************************************************************************
	//
	void OpenSoundProperties();
	//
	//**************************************

	//*****************************************************************************************
	//
	void AlphaColourTestBar(int i_num_colour);
	//
	// Draws an alpha colour test bar on the screen.
	//
	//**************************************

	//*****************************************************************************************
	//
	void OcclusionSettings();
	//
	// Opens the occlusion settings dialog box.
	//
	//**************************************

	//*****************************************************************************************
	//
	void OpenGunSettings();
	//
	// Opens the gun settings dialog box.
	//
	//**************************************

	//*****************************************************************************************
	//
	void InvokeCullingDialog();
	//
	// Opens the culling settings dialog box.
	//
	//**************************************

	//*****************************************************************************************
	//
	void OpenSchedulerSettings();
	//
	// Opens the scheduler settings dialog box.
	//
	//**************************************
	
	//*********************************************************************************************
	//
	void UpdateScreen();
	//
	// Reinitializes the main screen raster with the current settings.
	//
	//**************************************

	//*********************************************************************************************
	//
	void UpdateRecentFiles();
	//
	// Update the recent file list.
	//
	//**************************************

	//*********************************************************************************************
	//
	void AddRecentFile(char *str_name);
	//
	// Add a file to the recent file list.
	//
	//**************************************

	//*********************************************************************************************
	//
	void ShowLoadedFile(char *str_name);
	//
	// Show the files that are loaded in the tile bar.
	//
	//**************************************

	//*****************************************************************************************
	void operator delete(void *pv_mem);

	//*****************************************************************************************
	//
	// ClassWizard stuff.
	//

	//{{AFX_DATA(CGUIAppDlg)
	enum { IDD = IDD_GUIAPP_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGUIAppDlg)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual BOOL DestroyWindow() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	//}}AFX_VIRTUAL


protected:

	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	void RestoreFromFastMode();

	//*****************************************************************************************
	void AddDefaultLight();

	//*****************************************************************************************
	void ReleaseMouseCapture();

	//*****************************************************************************************
	void AdjustMousePos(CPoint& point);

	//*****************************************************************************************
	void CreateMainScreen(int i_screen_width = 0, int i_screen_height = 0,
		                  int i_screen_bits   = 0);

	//*****************************************************************************************
	void MoveCamera(float f_mouse_x, float f_mouse_y);

	//*****************************************************************************************
	void MoveObjects(float f_mouse_x, float f_mouse_y);

	//*****************************************************************************************
	void Step
	(
		TSec s_step = -1.0					// Amount to step; default is real-time.
	);

	//*****************************************************************************************
	void DebugMovement(CInstance* pins_mover, const CInstance* pins_pov);

	//*****************************************************************************************
	void JumpToLookAt(CInstance* pins_mover, CPartition* ppart_target);
	//
	// Moves the mover so that the mover can see "ppart_target".
	//
	//**************************************

	//*****************************************************************************************
	void MoveIntoView(CInstance* pins_viewer, CPartition* ppart_mover);
	//
	// Moves the mover so that the viewer can see the mover.
	//
	//**************************************

	//*****************************************************************************************
	void StatsMenuState(CCPUDetect&	detProcessor);
	//
	// Sets the profile stats that are possible on the current processor
	//
	//**************************************


	//*****************************************************************************************
	bool bDetectJoystick(EControlMethod ecm_stick);
	//
	// detects if a joytstick of the type passed in is present.
	//
	//**************************************

	// Overridden from CInstance...
	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const override;

	//*****************************************************************************************
	virtual const char* pcLoad(const char* pc_buffer) override;



	//*****************************************************************************************
	void ToggleRenderFeature(ERenderFeature erf);

	//*****************************************************************************************
	void DisplayCacheStats();

	//*****************************************************************************************
	void ExitApp();
				
	//*****************************************************************************************
	void MagnetSelected
	(
		const CMagnet* pmag_type
	);

	//*****************************************************************************************
	void StatsDisplay();

	//*****************************************************************************************
	void StatsDump();

	//*****************************************************************************************
	//
	// ClassWizard stuff.
	//

public:

	//{{AFX_MSG(CGUIAppDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnEditGamma();
	afx_msg void OnEditGore();
	afx_msg void OnZbuffer();
	afx_msg void OnScreenclip();
	afx_msg void OnScreencull();
	afx_msg void OnLight();
	afx_msg void OnLightShade();
	afx_msg void OnFog();
	afx_msg void OnFogShade();
	afx_msg void OnColoured();
	afx_msg void OnTexture();
	afx_msg void OnTransparent();
	afx_msg void OnBump();
	afx_msg void OnWireFrame();
	afx_msg void OnAlphaColour();
	afx_msg void OnAlphaShade();
	afx_msg void OnAlphaTexture();
	afx_msg void OnTrapezoids();
	afx_msg void OnSubpixel();
	afx_msg void OnPerspective();
	afx_msg void OnMipMap();
	afx_msg void OnDither();
	afx_msg void OnFilter();
	afx_msg void OnFilterEdges();
	afx_msg void OnSpecular();
	afx_msg void OnShadows();
	afx_msg void OnShadowTerrain();
	afx_msg void OnShadowTerrainMove();
	afx_msg void OnRenderCache();
	afx_msg void OnRenderCacheTest();
	afx_msg void OnMipMapSettings();
	afx_msg void OnDetail();
	afx_msg void OnShowConsole();
	afx_msg void OnShowStats();
	afx_msg void OnAvgStats();
	afx_msg void OnShowFPS();
	afx_msg void OnShowCacheStats();
	afx_msg void OnStatPhysics();
	afx_msg void OnViewHairs();
	afx_msg void OnCrosshairRadius();
	afx_msg void OnCrosshairVegetation();
	afx_msg void OnCrosshairTerrainTexture();
	afx_msg void OnViewSpheres();
	afx_msg void OnViewWire();
	afx_msg void OnViewPinhead();
	afx_msg void OnViewBones();
	afx_msg void OnViewBonesBoxes();
	afx_msg void OnViewBonesCollide();
	afx_msg void OnViewBonesWake();
	afx_msg void OnViewBonesQuery();
	afx_msg void OnViewBonesMagnets();
	afx_msg void OnViewBonesSkeletons();
	afx_msg void OnViewBonesAttach();
	afx_msg void OnViewBonesWater();
	afx_msg void OnViewBonesRaycast();
	afx_msg void OnViewQuads();
	afx_msg void OnPlayerPhysics();
	afx_msg void OnPlayerInvulnerable();
	afx_msg void OnPhysicsSleep();
	afx_msg void OnPhysicsStep();
	afx_msg void OnPhysicsPutToSleep();
	afx_msg void OnSmackSelected();
	afx_msg void OnSettlePhysics();
	afx_msg void OnMagnetBreak();
	afx_msg void OnMagnetNoBreak();
	afx_msg void OnDeMagnet();
	afx_msg void OnRedrawTerrain();
	afx_msg void OnCameraPlayer();
	afx_msg void OnAlphaColourSettings();
	afx_msg void OnCameraSelected();
	afx_msg void OnCameraSelectedHead();
	afx_msg void OnCameraFree();
	afx_msg void OnCamera2m();
	afx_msg void OnLoadScene();
	afx_msg void OnLoadRecentFile(UINT uID);
	afx_msg void OnSaveScene();
	afx_msg void OnSaveAsScene();
	afx_msg void OnTextSave();
	afx_msg void OnReplaySave();
	afx_msg void OnReplayLoad();
	afx_msg void OnLoadAnim();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSystemMem();
	afx_msg void OnMenuTerrain();
	afx_msg void OnMenuAddSkeleton();
	afx_msg void OnDefaultControls();
	afx_msg void OnStandardJoystick();
    afx_msg void OnTeleport();
	afx_msg void OnAI();
	afx_msg void OnPlayerProperties();
	afx_msg void OnMaterialProperties();
	afx_msg void OnPhysicsProperties();
	afx_msg void OnSoundMaterialProp();
	afx_msg void OnChangeSMat();
	afx_msg void OnPhysics();
	afx_msg void OnReset();
	afx_msg void OnResetSelected();
	afx_msg void OnArtStats();
	afx_msg void OnPredictMovement();
	afx_msg void OnStatClocks();
	afx_msg void OnStatMSR0();
	afx_msg void OnStatMSR1();
	afx_msg void OnStatRing0();
	afx_msg void OnStatRing3();
	afx_msg void OnTexturePack();
	afx_msg void OnMemStats();
	afx_msg void OnChangeStatCounter0(UINT nID);
	afx_msg void OnChangeStatCounter1(UINT nID);
	afx_msg void OnGiveMeALight();
	afx_msg void OnZOnlyEdit();
	afx_msg void OnNameSelect();
	afx_msg void OnSmallMoveSteps();
	afx_msg void OnTerrainSound();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg	void OnSkyDisable();
	afx_msg	void OnSkyRemove();
	afx_msg	void OnSkyTexture();
	afx_msg	void OnSkyFill();
	afx_msg void OnSkySettings();
	afx_msg void OnVMSettings();
	afx_msg void OnPackOptions();
	afx_msg void OnBumpPacking();
	afx_msg void OnEditAI();
	afx_msg void OnEditAIEmotions();
	afx_msg void OnDrawPhysics();
	afx_msg void OnDrawAI();
	afx_msg void OnDropMarker();
	afx_msg void OnRotateWorldZ();
	afx_msg void OnResetStartTriggers();
	afx_msg void OnRestoreSubsystemDefaults();
	afx_msg void OnRenderQualitySettings();
	afx_msg LRESULT OnIdle(WPARAM wParam, LPARAM lParam);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//
// Global variables.
//
extern CVector3<> v3InitCamPos;


#endif
