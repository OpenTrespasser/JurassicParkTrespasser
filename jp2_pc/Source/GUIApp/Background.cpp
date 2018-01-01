/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents: Implementation of 'Background.hpp'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/Background.cpp                                                $
 * 
 * 6     96/12/06 15:21 Speter
 * Made an assignment more efficient.
 * 
 * 5     96/10/04 18:11 Speter
 * Removed include of defunct ObjDef3D.hpp.
 * 
 * 4     9/13/96 2:00p Pkeet
 * Added includes taken from 'StdAfx.cpp.'
 * 
 * 3     8/20/96 4:48p Pkeet
 * Added fogging switch and dialog.
 * 
 * 2     8/15/96 10:49a Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/


//
// Includes.
//
#include "stdafx.h"
#include "Lib/Renderer/Light.hpp"
#include "GUITools.hpp"
#include "Background.hpp"
#include "GUIApp.h"
#include "GUIAppDlg.h"


//*********************************************************************************************
//
// CBackground implementation.


//*********************************************************************************************
//
// CBackground message map.

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CBackground, CDialog)
	//{{AFX_MSG_MAP(CBackground)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_GRAYSCALE, OnCheckGrayscale)
	ON_BN_CLICKED(BUTTON_BACK_TOFOGBACK, OnBackTofogback)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CBackground constructor.
//

//*********************************************************************************************
CBackground::CBackground(CWnd* pParent /*=NULL*/)
	: CDialog(CBackground::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBackground)
	//}}AFX_DATA_INIT
	bGrayscale = true;
}


//*********************************************************************************************
//
// CBackground constructor.
//

//*********************************************************************************************
//
void CBackground::DoDataExchange
(
	CDataExchange* pDX
)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackground)
	DDX_Control(pDX, IDC_CHECK_GRAYSCALE, m_ButtonGrayscale);
	DDX_Control(pDX, IDC_SCROLL_BLUE, m_ScrollBlue);
	DDX_Control(pDX, IDC_SCROLL_GREEN, m_ScrollGreen);
	DDX_Control(pDX, IDC_SCROLL_RED, m_ScrollRed);
	//}}AFX_DATA_MAP
}

//*********************************************************************************************
//
void CBackground::OnHScroll
(
	UINT        nSBCode,
	UINT        nPos,
	CScrollBar* pScrollBar
) 
//
// Responds to the WM_HSCROLL message by moving one or more scrollbars and updating colour
// information accordingly.
//
//**************************************
{
	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pappdlg);

	// Move the scrollbar.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	MoveScrollbar(nSBCode, nPos, pScrollBar);
	
	// Set all colours to the same value for grayscale.
	if (bGrayscale)
	{
		int i_value = pScrollBar->GetScrollPos();
		m_ScrollRed.SetScrollPos(i_value);
		m_ScrollGreen.SetScrollPos(i_value);
		m_ScrollBlue.SetScrollPos(i_value);
	}

	pappdlg->clrBackground.u1Red   = m_ScrollRed.GetScrollPos();
	pappdlg->clrBackground.u1Green = m_ScrollGreen.GetScrollPos();
	pappdlg->clrBackground.u1Blue  = m_ScrollBlue.GetScrollPos();
	pappdlg->UpdateBackground();
	pappdlg->Invalidate();

	// Set text.
	DisplayText();
}

//*********************************************************************************************
//
void CBackground::OnShowWindow
(
	BOOL bShow,
	UINT nStatus
)
//
// Responds to the WM_SHOWWINDOW message by initializing dialog controls.
//
//**************************************
{
	CDialog::OnShowWindow(bShow, nStatus);

	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pappdlg);

	//
	// Setup the grayscale button.
	//
	m_ButtonGrayscale.SetCheck((bGrayscale) ? (1) : (0));

	//
	// Setup scrollbars.
	//

	// Setup the red scrollbar.
	m_ScrollRed.SetScrollRange(0, 255, FALSE);
	m_ScrollRed.SetScrollPos(pappdlg->clrBackground.u1Red);

	// Setup the green scrollbar.
	m_ScrollGreen.SetScrollRange(0, 255, FALSE);
	m_ScrollGreen.SetScrollPos(pappdlg->clrBackground.u1Green);

	// Setup the blue scrollbar.
	m_ScrollBlue.SetScrollRange(0, 255, FALSE);
	m_ScrollBlue.SetScrollPos(pappdlg->clrBackground.u1Blue);

	//
	// Setup static text.
	//
	DisplayText();
}

//*********************************************************************************************
//
void CBackground::DisplayText
(
)
//
// Set the text in all related static controls.
//
//**************************************
{	
	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pappdlg);

	SetDlgItemInt(IDC_STATIC_RED,   pappdlg->clrBackground.u1Red);
	SetDlgItemInt(IDC_STATIC_GREEN, pappdlg->clrBackground.u1Green);
	SetDlgItemInt(IDC_STATIC_BLUE,  pappdlg->clrBackground.u1Blue);
}

//*********************************************************************************************
//
void CBackground::OnCheckGrayscale
(
) 
//
// Responds to a IDC_CHECK_GRAYSCALE message by toggling the scroll controls between grayscale
// mode and non-grayscale mode.
//
//**************************************
{
	bGrayscale = m_ButtonGrayscale.GetCheck() != 0;

	// If the grayscale option is selected, make sure the background is gray.
	if (bGrayscale)
	{
		// Find the average of all the colours.
		int i_gray = (m_ScrollRed.GetScrollPos() + m_ScrollGreen.GetScrollPos() * 2 +
		             m_ScrollBlue.GetScrollPos()) / 4;

		// Set all the colours to the average.
		m_ScrollRed.SetScrollPos(i_gray);
		m_ScrollGreen.SetScrollPos(i_gray);
		m_ScrollBlue.SetScrollPos(i_gray);

		// Output the text values.
		DisplayText();

		// Redraw the screen.
		CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

		Assert(pappdlg);

		pappdlg->clrBackground.u1Red   = i_gray;
		pappdlg->clrBackground.u1Green = i_gray;
		pappdlg->clrBackground.u1Blue  = i_gray;
		pappdlg->UpdateBackground();
		pappdlg->Invalidate();
	}
}

//*********************************************************************************************
//
void CBackground::OnBackTofogback
(
) 
//
// Set to fog background colour.
//
//**************************************
{
	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pappdlg);

	// Set background colour.
	pappdlg->clrBackground = clrDefEndDepth;

	// Set scrollbars.
	m_ScrollRed.SetScrollPos(pappdlg->clrBackground.u1Red);
	m_ScrollGreen.SetScrollPos(pappdlg->clrBackground.u1Green);
	m_ScrollBlue.SetScrollPos(pappdlg->clrBackground.u1Blue);

	// Reset the background.
	pappdlg->UpdateBackground();
	pappdlg->Invalidate();
}
