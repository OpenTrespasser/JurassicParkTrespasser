/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Main windows application class for the GUIApp.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/GUIApp.h                                                      $
 * 
 * 85    8/11/98 8:32p Mlange
 * Added menu item to revert back to old broadcast messaging system.
 * 
 * 84    8/05/98 3:17p Mlange
 * Working on grab continous command.
 * 
 * 83    98/07/25 23:45 Speter
 * Added command for physics stats window.
 * 
 * 82    7/13/98 12:03a Pkeet
 * Removed the 'OnStretch' member function.
 * 
 * 81    98.05.14 7:02p Mmouni
 * Added menu item to restore default settings.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_GUIAPP_HPP
#define HEADER_GUIAPP_GUIAPP_HPP

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


//*********************************************************************************************
//
class CGUIAppApp : public CWinApp
//
// The main application loop. 
//
// Prefix: guiapp
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	// AppWizard generated constructor.
	CGUIAppApp();


	//*****************************************************************************************
	//
	// ClassWizard stuff.
	//

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGUIAppApp)
	public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGUIAppApp)
	afx_msg void OnAbout();
	afx_msg void OnShowtoolbar();
	afx_msg void OnAddobject();
	afx_msg void OnDelete();
	afx_msg void OnProperties();
	afx_msg void OnCameraproperties();
	afx_msg void OnAmbient();
	afx_msg void OnDirectional();
	afx_msg void OnPoint();
	afx_msg void OnPointdirectional();
	afx_msg void OnBackground();
	afx_msg void OnEditFog();
	afx_msg void OnTransparencies();
	afx_msg void OnEditPlayaudio();
	afx_msg void OnEditStopaudio();
	afx_msg void OnEditAudioproperties();
	afx_msg void OnBumpmap();
	afx_msg void OnDebug();
	afx_msg void OnPlay();
	afx_msg void OnPause();
	afx_msg void OnMenuWater();
	afx_msg void OnFpsEstimate();
	afx_msg void OnPresortNone();
	afx_msg void OnPresortFtob();
	afx_msg void OnPresortBtf();
	afx_msg void OnEditPerspective();
	afx_msg void OnRendercacheSettings();
	afx_msg void OnFastmode();
	afx_msg void OnDelaunayTest();
	afx_msg void OnCacheIntersect();
	afx_msg void OnConphysics();
	afx_msg void OnConshadows();
	afx_msg void OnConterrain();
	afx_msg void OnConAI();
	afx_msg void OnDepthSort();
	afx_msg void OnStatsDepthSort();
	afx_msg void OnTerrainSettings();
	afx_msg void OnWaterSettings();
	afx_msg void OnLoadTerrain();
	afx_msg void OnExportTerrainTri();
	afx_msg void OnTerrainTest();
	afx_msg void OnHeightmap();
	afx_msg void OnTerrainWire();
	afx_msg void OnTextureWire();
	afx_msg void OnDepthSortSettings();
	afx_msg void OnMode1();
	afx_msg void OnMode2();
	afx_msg void OnMode3();
	afx_msg void OnMode4();
	afx_msg void OnSoundProperties();
	afx_msg void OnViewPartitions();
	afx_msg void OnDoubleV();
	afx_msg void OnHalfscan();
	afx_msg void OnProcessor();
	afx_msg void OnConvexcaches();
	afx_msg void OnStaticHandles();
	afx_msg void OnPreload();
	afx_msg void OnPartitions();
	afx_msg void OnMipmapNormal();
	afx_msg void OnMipmapNoLargest();
	afx_msg void OnMipmapSmallest();
	afx_msg void OnOccludeObjects();
	afx_msg void OnOccludePolygons();
	afx_msg void OnOcclusionConsole();
	afx_msg void OnOccludeOcclude();
	afx_msg void OnOccludeSettings();
	afx_msg void OnOccludeCameraviewtest();
	afx_msg void OnGunSettings();
	afx_msg void OnSchedulerSettings();
	afx_msg void OnSchedulerUse();
	afx_msg void OnViewTriggers();
	afx_msg void OnDumpFixedheap();
	afx_msg void OnForceIntersecting();
	afx_msg void OnGrab();
	afx_msg void OnGrabContinous();
	afx_msg void OnCulling();
	afx_msg void OnAcceleration();
	afx_msg void OnWaterAlpha();
	afx_msg void OnWaterInterpolate();
	afx_msg void OnAssertSave();
	afx_msg void OnIgnoreMsgRecipients();
	afx_msg void OnHardwareStats();
	afx_msg void OnFilterCaches();
	afx_msg void OnFreezeCaches();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
