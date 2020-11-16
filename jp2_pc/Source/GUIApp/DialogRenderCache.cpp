/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of 'DialogPhysics.hpp.'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogRenderCache.cpp                                         $
 * 
 * 32    6/02/98 11:53a Pkeet
 * Added the 'ParallaxShear' slider.
 * 
 * 31    5/13/98 6:46p Pkeet
 * Changed range for distortion.
 * 
 * 30    5/10/98 8:18p Pkeet
 * Changed ranges for sliders.
 * 
 * 29    5/10/98 2:07p Pkeet
 * Removed unused render cache parameters, added new ones.
 * 
 * 28    4/21/98 8:14p Pkeet
 * Removed the call to 'InitializeCaching.'
 * 
 * 27    3/18/98 5:03p Pkeet
 * Added the 'bFasterPhysics' flag.
 * 
 * 26    1/15/98 10:14p Agrant
 * A smaller range for detail reduction settings.
 * Lower number now means lower detail used.
 * 
 * 25    1/02/98 11:27a Pkeet
 * Added minimum distance parameter.
 * 
 * 24    12/06/97 2:37p Pkeet
 * Updates static cache information.
 * 
 * 23    12/05/97 5:04p Pkeet
 * Added the 'fEfficiencyArea' parameter.
 * 
 * 22    12/05/97 3:50p Pkeet
 * Added new parameters for setting schedule weights and multiple object caching efficiency.
 * 
 * 21    97/11/14 9:18p Pkeet
 * Added slider for volume angle threshold.
 * 
 * 20    97/10/29 5:26p Pkeet
 * Added and removed scrollbars.
 * 
 * 19    97/10/28 3:35p Pkeet
 * Removed unnecessary and unused render cache code.
 * 
 * 18    97/09/26 5:38p Pkeet
 * Added the 'InitializePartitions' function.
 * 
 * 17    97/08/04 6:17p Pkeet
 * Added the 'bUseCameraPrediction' parameter.
 * 
 * 16    97/07/22 3:33p Pkeet
 * Replaced 'rcsRenderCacheSettings.fPerspectiveMaxError' with
 * 'rcsRenderCacheSettings.fDistortionAngleThreshold.'
 * 
 * 15    97/07/16 7:03p Pkeet
 * Changed scroll bar range.
 * 
 * 14    97/07/15 3:39p Pkeet
 * Changed parameter ranges.
 * 
 * 13    97/07/15 2:39p Pkeet
 * Added a scrollbar to set the threshold area for using polygons instead of rectangles for the
 * image cache.
 * 
 * 12    97/07/15 1:00p Pkeet
 * Added new parameters for convex polygon simplification.
 * 
 * 11    97/06/16 12:34p Pkeet
 * Added a max instances scrollbar.
 * 
 * 10    97/06/15 7:14p Pkeet
 * Added cache scheduling.
 * 
 * 9     97/06/15 17:35 Speter
 * Changed range of detail reduction scrollbar.
 * 
 * 8     97/06/14 0:26 Speter
 * Added detail-reduction menu item, and slider (to Render Cache dialog).
 * 
 * 7     97/06/10 1:27p Pkeet
 * Uses the 'SetRadiusToDistanceThreshold' member function.
 * 
 * 6     97/06/07 2:12p Pkeet
 * Altered range of max size threshold.
 * 
 * 5     97-04-15 16:25 Speter
 * Removed unneeded include.
 * 
 * 4     97/03/26 5:15p Pkeet
 * Added euthanasia settings and display.
 * 
 * 3     3/11/97 3:21p Blee
 * Revamped trigger system.
 * 
 * 2     1/25/97 1:48p Pkeet
 * Added controls for render cache settings.
 * 
 *********************************************************************************************/


//
// Includes.
//

#include "StdAfx.h"
#include "DialogRenderCache.hpp"
#include "Lib/Renderer/RenderCacheInterface.hpp"
#include "Lib/Renderer/PipeLine.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/Sys/Scheduler.hpp"


//
// Constants.
//

// Scrollbar to angular error conversion factor.
const float fScrollbarToAngular = 10000.0f;


//
// Class implementations.
//

//*********************************************************************************************
//
// CDialogRenderCache implementation.
//


CDialogRenderCache::CDialogRenderCache(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRenderCache::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRenderCache)
	//}}AFX_DATA_INIT
}


void CDialogRenderCache::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRenderCache)
	DDX_Control(pDX, IDC_SCROLL_PARALLAXSHEAR, m_ParallaxShear);
	DDX_Control(pDX, IDC_CHECK_FORCECACHING, m_ForceCaching);
	DDX_Control(pDX, IDC_SCROLL_AREAEFFICIENCY, m_AreaEfficiency);
	DDX_Control(pDX, IDC_SCROLL_CYLINDERDEG, m_CylinderDeg);
	DDX_Control(pDX, IDC_SCROLL_MAXOBJECTS, m_MaxObjects);
	DDX_Control(pDX, IDC_SCROLL_DISTORTION, m_Distortion);
	DDX_Control(pDX, IDC_SCROLL_DETAILREDUCE, m_DetailReduce);
	DDX_Control(pDX, IDC_CHECK_FASTERPHYSICS, m_FasterPhysics);
	DDX_Control(pDX, IDC_CHECK_PREDICTION, m_CheckPrediction);
	DDX_Control(pDX, IDC_SCROLL_POLYAREA, m_PolyArea);
	DDX_Control(pDX, IDC_SCROLL_LINE, m_Line);
	DDX_Control(pDX, IDC_SCROLL_AREA, m_Area);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRenderCache, CDialog)
	//{{AFX_MSG_MAP(CDialogRenderCache)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_PREDICTION, OnCheckPrediction)
	ON_BN_CLICKED(IDC_CHECK_FASTERPHYSICS, OnCheckFasterPhysics)
	ON_BN_CLICKED(IDC_CHECK_FORCECACHING, OnCheckForceCaching)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogRenderCache message handlers


//*********************************************************************************************
//
void CDialogRenderCache::Redraw
(
)
//
// Redraws the main window with updated fog parameters.
//
//**************************************
{
	// Update screen.
	GetParent()->Invalidate();
}

//*********************************************************************************************
//
void CDialogRenderCache::DisplayText
(
)
//
// Set the text in all related static controls.
//
//**************************************
{
	SetDlgItemInt(IDC_STATIC_CYLINDERDEG,          rcsRenderCacheSettings.iMaxAcceptAngleCylinder);
	SetDlgItemFloat(this, IDC_STATIC_DETAILREDUCE, prenMain->pSettings->fDetailReduceFactor);
	SetDlgItemFloat(this, IDC_STATIC_AREA, rcsRenderCacheSettings.rPixelsPerArea);
	SetDlgItemFloat(this, IDC_STATIC_LINE, rcsRenderCacheSettings.rPixelsPerLine);
	SetDlgItemFloat(this, IDC_STATIC_PARALLAXSHEAR, rcsRenderCacheSettings.fParallaxShear);
	SetDlgItemInt(IDC_STATIC_POLYAREA, rcsRenderCacheSettings.rMinPolygonMesh);

	// Multiple object image caching.
	SetDlgItemInt(IDC_STATIC_MAXOBJECTS,           rcsRenderCacheSettings.iMaxObjectsPerCache);

	// Schedule items.
	SetDlgItemFloat(this, IDC_STATIC_DISTORTION,    rcsRenderCacheSettings.fDistortionWeight);

	//
	// New render cache settings.
	//
	SetDlgItemFloat(this, IDC_STATIC_AREAEFFICIENCY, CPartition::fGetAreaEfficiencyThreshold());
}

//*********************************************************************************************
void CDialogRenderCache::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Move scrollbar.
	MoveScrollbar(nSBCode, nPos, pScrollBar);

	// Call the base class function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// Set render cache variables.
	rcsRenderCacheSettings.SetMaxAngleCylinder(m_CylinderDeg.GetScrollPos());

	rcsRenderCacheSettings.rPixelsPerArea        = float(m_Area.GetScrollPos()) / 1000.0f;
	rcsRenderCacheSettings.rPixelsPerLine        = float(m_Line.GetScrollPos()) / 10.0f;
	rcsRenderCacheSettings.rMinPolygonMesh       = m_PolyArea.GetScrollPos() * 10;

	rcsRenderCacheSettings.iMaxObjectsPerCache  = m_MaxObjects.GetScrollPos();

	// Schedule items.
	rcsRenderCacheSettings.fDistortionWeight    = m_Distortion.fGet();
	rcsRenderCacheSettings.fParallaxShear       = m_ParallaxShear.fGet();

	//
	// New render cache settings.
	//
	CPartition::SetAreaEfficiencyThreshold(m_AreaEfficiency.fGet());

	// Multi-level mesh stuff goes here too.
	prenMain->pSettings->fDetailReduceFactor    = float(m_DetailReduce.GetScrollPos()) / 100.0f;

	// Display new render cache values.
	DisplayText();

	// Reset partitions for render cache use.
	wWorld.InitializePartitions();

	// Show new render cache display.
	Redraw();
}

//*********************************************************************************************
void CDialogRenderCache::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	// Call the base class function.
	CDialog::OnShowWindow(bShow, nStatus);

	//
	// Setup scrollbars.
	//

	// Set the sphere and cylinder angle values.
	m_CylinderDeg.SetScrollRange(1, 89, FALSE);
	m_CylinderDeg.SetScrollPos(rcsRenderCacheSettings.iMaxAcceptAngleCylinder);
	
	// Setup the multi-level threshold scrollbar.
	m_DetailReduce.SetScrollRange(1, 1000, FALSE);
	m_DetailReduce.SetScrollPos(prenMain->pSettings->fDetailReduceFactor * 100.0f);
	
	// Setup the area threshold scrollbar.
	m_Area.SetScrollRange(1, 2000, FALSE);
	m_Area.SetScrollPos(int(rcsRenderCacheSettings.rPixelsPerArea * 1000.0f));
	
	// Setup the line threshold scrollbar.
	m_Line.SetScrollRange(1, 2000, FALSE);
	m_Line.SetScrollPos(int(rcsRenderCacheSettings.rPixelsPerLine * 10.0f));
	
	// Setup the minimum polygon area scrollbar.
	m_PolyArea.SetScrollRange(1, 20000, FALSE);
	m_PolyArea.SetScrollPos(rcsRenderCacheSettings.rMinPolygonMesh / 10);

	// Set buttons.
	m_CheckPrediction.SetCheck(rcsRenderCacheSettings.bUseCameraPrediction);
	m_FasterPhysics.SetCheck(rcsRenderCacheSettings.bFasterPhysics);

	// Multiple object image caching.
	m_MaxObjects.SetScrollRange(1, 250, FALSE);
	m_MaxObjects.SetScrollPos(rcsRenderCacheSettings.iMaxObjectsPerCache);

	// Distortion and parallax shear scroll bars.
	m_Distortion.Set(rcsRenderCacheSettings.fDistortionWeight, 0.001f, fAlwaysExecute / 100.0f);
	m_ParallaxShear.Set(rcsRenderCacheSettings.fParallaxShear, 0.05f, 1.1f);

	//
	// New image cache settings.
	//
	m_AreaEfficiency.Set(CPartition::fGetAreaEfficiencyThreshold(), 0.000001f, 100.0f);

	// Display the current settings.
	DisplayText();
}

//*********************************************************************************************
void CDialogRenderCache::OnCheckPrediction() 
{
	rcsRenderCacheSettings.bUseCameraPrediction = !rcsRenderCacheSettings.bUseCameraPrediction;
}

//*********************************************************************************************
void CDialogRenderCache::OnCheckFasterPhysics() 
{
	rcsRenderCacheSettings.bFasterPhysics = !rcsRenderCacheSettings.bFasterPhysics;
}

//*********************************************************************************************
void CDialogRenderCache::OnCheckForceCaching() 
{
}
