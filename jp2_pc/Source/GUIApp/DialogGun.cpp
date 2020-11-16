/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogGun.cpp                                                 $
 * 
 * 7     12/12/97 9:56p Agrant
 * Changed global gun to a global gun data.
 * 
 * 6     12/12/97 5:22p Agrant
 * Move guns to the design daemon
 * 
 * 5     97/10/17 5:32p Pkeet
 * Implemented pull parameters.
 * 
 * 4     97/10/15 6:46p Pkeet
 * Changed slider ranges.
 * 
 * 3     97/10/15 4:51p Pkeet
 * Changed 'fMoment' to 'fImpulse.' Changed 'm_GunMoment' to 'm_GunImpulse.'
 * 
 * 2     97/10/15 3:04p Pkeet
 * Hooked up the scrollbars.
 * 
 * 1     97/10/15 2:27p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#include "StdAfx.h"
#include "GUIApp.h"
#include "GUIPipeLine.hpp"
#include "GUITools.hpp"
#include "GUIAppDlg.h"
#include "Game/DesignDaemon/Gun.hpp"
#include "DialogGun.hpp"


/////////////////////////////////////////////////////////////////////////////
// CDialogGun dialog

CDialogGun::CDialogGun(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogGun::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogGun)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogGun::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogGun)
	DDX_Control(pDX, IDC_SCROLL_GUN_IMPULSE_PULL, m_GunImpulsePull);
	DDX_Control(pDX, IDC_SCROLL_GUN_EXTENSION_PULL, m_GunExtensionPull);
	DDX_Control(pDX, IDC_SCROLL_GUN_EXTENSION, m_GunExtension);
	DDX_Control(pDX, IDC_SCROLL_GUN_MOMENT, m_GunImpulse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogGun, CDialog)
	//{{AFX_MSG_MAP(CDialogGun)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogGun message handlers

//*********************************************************************************************
void CDialogGun::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	// Set the scrollbars for range and values.
	m_GunExtension.Set(gdGlobalGunData.fExtension, 0.1f, 100.0f, 1000);
	m_GunImpulse.Set(gdGlobalGunData.fImpulse, 0.0f, 1000.0f, 1000);
	m_GunExtensionPull.Set(gdGlobalGunData.fExtensionPull, 0.1f, 100.0f, 1000);
	m_GunImpulsePull.Set(gdGlobalGunData.fImpulsePull, 0.0f, 1000.0f, 1000);

	// Display the current values.
	DisplayText();
}

//*********************************************************************************************
void CDialogGun::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// Move the scrollbar in question.
	MoveScrollbar(nSBCode, nPos, pScrollBar);

	// Get the values from the scrollbars.
	gdGlobalGunData.fExtension     = m_GunExtension.fGet();
	gdGlobalGunData.fImpulse       = m_GunImpulse.fGet();
	gdGlobalGunData.fExtensionPull = m_GunExtensionPull.fGet();
	gdGlobalGunData.fImpulsePull   = m_GunImpulsePull.fGet();

	// Display the new values.
	DisplayText();
}

//*****************************************************************************************
//
void CDialogGun::DisplayText
(
)
//
// Sets the text for static text items.
//
//**************************************
{
	SetDlgItemFloat(this, IDC_STATIC_GUN_EXTENSION, gdGlobalGunData.fExtension);
	SetDlgItemFloat(this, IDC_STATIC_GUN_MOMENT, gdGlobalGunData.fImpulse);
	SetDlgItemFloat(this, IDC_STATIC_GUN_EXTENSION_PULL, gdGlobalGunData.fExtension);
	SetDlgItemFloat(this, IDC_STATIC_GUN_IMPULSE_PULL, gdGlobalGunData.fImpulsePull);
}
