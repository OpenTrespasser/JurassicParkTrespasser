/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Implementation of "DialogWater.hpp."
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogWater.cpp                                               $
 * 
 * 2     5/11/98 2:24p Mlange
 * Operational.
 * 
 * 1     5/11/98 1:44p Mlange
 * Initial revision.
 * 
 *********************************************************************************************/

#include "StdAfx.h"
#include "DialogWater.hpp"
#include "GUIApp/GUIAppDlg.h"
#include "Lib/EntityDBase/Water.hpp"

//*********************************************************************************************
//
// CDialogWater Implementation.
//

//*********************************************************************************************
CDialogWater::CDialogWater(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogWater::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogWater)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*********************************************************************************************
void CDialogWater::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogWater)
	DDX_Control(pDX, IDC_SCROLL_WATER_MAX_SCREEN_RES, m_ScrollWaterMaxScreenRes);
	DDX_Control(pDX, IDC_SCROLL_WATER_MAX_WORLD_RES, m_ScrollWaterMaxWorldRes);
	//}}AFX_DATA_MAP
}

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogWater, CDialog)
	//{{AFX_MSG_MAP(CDialogWater)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*********************************************************************************************
void CDialogWater::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Move scrollbar.
	MoveScrollbar(nSBCode, nPos, pScrollBar);

	// Call the base class member function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	CEntityWater::rvarMaxScreenRes.SetFraction(m_ScrollWaterMaxScreenRes.GetScrollPos() / 100.0);
	CEntityWater::rvarMaxWorldRes.SetFraction(m_ScrollWaterMaxWorldRes.GetScrollPos() / 100.0);

	// Display new values.
	DisplayText();

	// Re-evaluate the water.
	Update();
}

//*********************************************************************************************
void CDialogWater::Update()
{
	// Update main screen.
	GetParent()->Invalidate();
}

//*********************************************************************************************
void CDialogWater::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);	

	// Set to default position.
	int i_x  = 512 + 2 * GetSystemMetrics(SM_CXFRAME) + iGUIAPP_DEFAULT_OFFSET;
	SetWindowPos(&wndTop, i_x, iGUIAPP_DEFAULT_OFFSET, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);

	//
	// Setup scrollbars.
	//

	// Setup the constant scrollbar.
	m_ScrollWaterMaxScreenRes.SetScrollRange(0, 100, FALSE);
	m_ScrollWaterMaxWorldRes.SetScrollRange(0, 100, FALSE);

	int i_max_screen_res = int(CEntityWater::rvarMaxScreenRes.fGetFraction() * 100 + .499);
	int i_max_world_res  = int(CEntityWater::rvarMaxWorldRes.fGetFraction() * 100 + .499);

	m_ScrollWaterMaxScreenRes.SetScrollPos(i_max_screen_res);
	m_ScrollWaterMaxWorldRes.SetScrollPos(i_max_world_res);

	//
	// Setup static text.
	//
	DisplayText();
}

//*********************************************************************************************
//
void CDialogWater::DisplayText
(
)
//
// Set the text in all related static controls.
//
//**************************************
{
	float f_max_screen_res = CEntityWater::rvarMaxScreenRes;
	float f_max_world_res  = CEntityWater::rvarMaxWorldRes;

	SetDlgItemFloat(this, IDC_STATIC_WATER_MAX_SCREEN_RES, f_max_screen_res, 2);
	SetDlgItemFloat(this, IDC_STATIC_WATER_MAX_WORLD_RES, f_max_world_res, 2);
}

