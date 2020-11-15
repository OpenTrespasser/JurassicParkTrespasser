// DialogScheduler.cpp : implementation file
//

#include "StdAfx.h"
#include "GUIApp.h"
#include "Lib/Sys/Scheduler.hpp"
#include "DialogScheduler.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"

const float fThresholdMultiplier = 1000.0f;

/////////////////////////////////////////////////////////////////////////////
// CDialogScheduler dialog

CDialogScheduler::CDialogScheduler(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogScheduler::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogScheduler)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogScheduler::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogScheduler)
	DDX_Control(pDX, IDC_SCROLL_SCHEDULER_THRESHOLD, m_Threshold);
	DDX_Control(pDX, IDC_SCROLL_SCHEDULER,         m_ScrollScheduler);
	DDX_Control(pDX, IDC_SCROLL_SCHEDULER_TERRAIN, m_ScrollSchedulerTerrain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogScheduler, CDialog)
	//{{AFX_MSG_MAP(CDialogScheduler)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogScheduler message handlers

void CDialogScheduler::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	// Setup the scrollbars.
	m_ScrollScheduler.SetScrollRange(0, 100, FALSE);
	m_ScrollScheduler.SetScrollPos(shcScheduler.uMSSlice);

	m_ScrollSchedulerTerrain.SetScrollRange(0, 100, FALSE);
	m_ScrollSchedulerTerrain.SetScrollPos(shcSchedulerTerrainTextures.uMSSlice);

	m_Threshold.SetScrollRange(0, 500, FALSE);
	m_Threshold.SetScrollPos(int(SPartitionSettings::fGetCameraMovementThreshold() * fThresholdMultiplier));

	// Display current values.
	DisplayText();
}

void CDialogScheduler::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Move scrollbar.
	MoveScrollbar(nSBCode, nPos, pScrollBar);

	// Call the base class function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// Set variables.
	shcScheduler.uMSSlice                = m_ScrollScheduler.GetScrollPos();
	shcSchedulerTerrainTextures.uMSSlice = m_ScrollSchedulerTerrain.GetScrollPos();
	SPartitionSettings::SetCameraMovementThreshold(float(m_Threshold.GetScrollPos()) / fThresholdMultiplier);

	// Display current values.
	DisplayText();
}

void CDialogScheduler::DisplayText()
{
	SetDlgItemInt(IDC_STATIC_TIME,         shcScheduler.uMSSlice);
	SetDlgItemInt(IDC_STATIC_TIME_TERRAIN, shcSchedulerTerrainTextures.uMSSlice);
	SetDlgItemFloat(this, IDC_STATIC_SCHEDULER_THRESHOLD, SPartitionSettings::fGetCameraMovementThreshold(), 3);
}
