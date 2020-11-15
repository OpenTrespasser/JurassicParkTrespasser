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
 * $Log:: /JP2_PC/Source/GUIApp/DialogOcclusion.cpp                                           $
 * 
 * 3     3/05/98 11:16a Pkeet
 * Added range to the area threshold.
 * 
 * 2     97/10/08 6:22p Pkeet
 * Added code to display values and operate scrollbars.
 * 
 * 1     97/10/08 6:02p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#include "StdAfx.h"
#include "GUIApp.h"
#include "GUIPipeLine.hpp"
#include "GUITools.hpp"
#include "GUIAppDlg.h"
#include "Lib/Renderer/Occlude.hpp"
#include "DialogOcclusion.hpp"


//**********************************************************************************************
//
// CDialogOcclusion implementation.
//

//**********************************************************************************************
CDialogOcclusion::CDialogOcclusion(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogOcclusion::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogOcclusion)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//**********************************************************************************************
void CDialogOcclusion::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogOcclusion)
	DDX_Control(pDX, IDC_SCROLL_OCCLUDE_THRESHOLD, m_AreaThreshold);
	DDX_Control(pDX, IDC_SCROLL_OCCLUDE_MAXINVIEW, m_MaxInView);
	//}}AFX_DATA_MAP
}

//**********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogOcclusion, CDialog)
	//{{AFX_MSG_MAP(CDialogOcclusion)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogOcclusion message handlers

//**********************************************************************************************
void CDialogOcclusion::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	//
	// Setup scrollbars.
	//

	// Setup the colour scrollbar.
	m_AreaThreshold.SetScrollRange(1, 2500, FALSE);
	m_AreaThreshold.SetScrollPos(int(COcclude::rWorldAreaThreshold * 10.0f));

	// Setup the red scrollbar.
	m_MaxInView.SetScrollRange(1, 100, FALSE);
	m_MaxInView.SetScrollPos(COcclude::iMaxNumOccludeObjects);

	//
	// Setup static text.
	//
	DisplayText();
}

//**********************************************************************************************
void CDialogOcclusion::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Move scrollbar.
	MoveScrollbar(nSBCode, nPos, pScrollBar);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	
	COcclude::rWorldAreaThreshold   = float(m_AreaThreshold.GetScrollPos()) / 10.0f;
	COcclude::iMaxNumOccludeObjects = m_MaxInView.GetScrollPos();

	// Display new values.
	DisplayText();
	RedrawScreen();
}

//*********************************************************************************************
//
void CDialogOcclusion::DisplayText
(
)
//
// Set the text in all related static controls.
//
//**************************************
{	
	SetDlgItemFloat(this, IDC_STATIC_OCCLUDE_THRESHOLD, COcclude::rWorldAreaThreshold);
	SetDlgItemInt(IDC_STATIC_OCCLUDE_MAXINVIEW, COcclude::iMaxNumOccludeObjects);
}

//*********************************************************************************************
//
void CDialogOcclusion::RedrawScreen
(
)
//
// Set the text in all related static controls.
//
//**************************************
{
	CGUIAppDlg* pgui = dynamic_cast<CGUIAppDlg*>(GetParent());
	AlwaysAssert(pgui);

	pgui->PaintWindow();
}
