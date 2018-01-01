// DialogCulling.cpp : implementation file
//

#include <math.h>
#include "stdafx.h"
#include "guiapp.h"
#include "DialogCulling.hpp"
#include "Lib/GeomDBase/Partition.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"


/////////////////////////////////////////////////////////////////////////////
// CDialogCulling dialog


CDialogCulling::CDialogCulling(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogCulling::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogCulling)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogCulling::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogCulling)
	DDX_Control(pDX, IDC_SCROLL_PRIORITY, m_Priority);
	DDX_Control(pDX, IDC_SCROLL_CULLINGSHADOW, m_ScrollCullingShadows);
	DDX_Control(pDX, IDC_SCROLL_CULLING, m_ScrollCulling);
	DDX_Control(pDX, IDC_SCROLL_RADIUS, m_ScrollRadius);
	DDX_Control(pDX, IDC_SCROLL_RADIUSSHADOW, m_ScrollRadiusShadows);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogCulling, CDialog)
	//{{AFX_MSG_MAP(CDialogCulling)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
void CDialogCulling::DisplayText
(
)
//
// Set the text in all related static controls.
//
//**************************************
{
	SetDlgItemFloat(this, IDC_STATIC_CULLING, SPartitionSettings::fGetCullMaxAtDistance());
	SetDlgItemFloat(this, IDC_STATIC_CULLINGSHADOW, SPartitionSettings::fGetCullMaxAtDistanceShadow());
	SetDlgItemFloat(this, IDC_STATIC_RADIUS, SPartitionSettings::fGetMaxRadius());
	SetDlgItemFloat(this, IDC_STATIC_RADIUSSHADOW, SPartitionSettings::fGetMaxRadiusShadow());
	SetDlgItemInt(IDC_STATIC_PRIORITY, iGetPrioritySetting());
}

//*********************************************************************************************
void CDialogCulling::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	m_ScrollCulling.Set(SPartitionSettings::fGetCullMaxAtDistance(), 10.0f, 1000.0f);
	m_ScrollCullingShadows.Set(SPartitionSettings::fGetCullMaxAtDistanceShadow(), 10.0f, 100.0f);
	m_ScrollRadius.Set(SPartitionSettings::fGetMaxRadius(), 0.1f, 10.0f);
	m_ScrollRadiusShadows.Set(SPartitionSettings::fGetMaxRadiusShadow(), 0.1f, 10.0f);

	m_Priority.SetScrollRange(1, 4, FALSE);
	m_Priority.SetScrollPos(iGetPrioritySetting());

	// Display the current settings.
	DisplayText();
}

//*********************************************************************************************
void CDialogCulling::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Move scrollbar.
	MoveScrollbar(nSBCode, nPos, pScrollBar);

	// Call the base class function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// Get the distance multiplier.
	SPartitionSettings::SetCullMaxAtDistance(m_ScrollCulling.fGet());
	SPartitionSettings::SetCullMaxAtDistanceShadow(m_ScrollCullingShadows.fGet());
	SPartitionSettings::SetMaxRadius(m_ScrollRadius.fGet());
	SPartitionSettings::SetMaxRadiusShadow(m_ScrollRadiusShadows.fGet());

	// Get the priority.
	SetPrioritySetting(m_Priority.GetScrollPos());

	// Rebuild the world database.
	if (pwWorld)
		pwWorld->InitializePartitions();

	// Redraw image caches and the terrain.
	if (CWDbQueryTerrain().tGet())
		CWDbQueryTerrain().tGet()->Rebuild(true);

	// Display the current settings.
	DisplayText();

	// Update screen.
	GetParent()->Invalidate();
}
