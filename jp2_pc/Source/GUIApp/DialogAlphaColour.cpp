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
 * $Log:: /JP2_PC/Source/GUIApp/DialogAlphaColour.cpp                                         $
 * 
 * 5     97/08/13 7:05p Pkeet
 * Cleaned up code. Added support for interleaved alpha colours. Added code for th 565 format,
 * but have not been able to get it to work correctly yet.
 * 
 * 4     97/08/13 3:29p Pkeet
 * Added the 'RedrawScreen' member function.
 * 
 * 3     97/08/11 12:04p Pkeet
 * Put creation code into the setup function. Made test work with a alpha colour number.
 * 
 * 2     97/08/09 3:12p Pkeet
 * Enabled the dialog to work with the light blend structure.
 * 
 * 1     97/08/09 2:16p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

//
// Includes.
//
#include "StdAfx.h"
#include "GUIApp.h"
#include "common.hpp"
#include "GUIPipeLine.hpp"
#include "GUITools.hpp"
#include "GUIApp.h"
#include "GUIAppDlg.h"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Renderer/LightBlend.hpp"
#include "DialogAlphaColour.hpp"


int iAlphaColour = 0;


/////////////////////////////////////////////////////////////////////////////
// CDialogAlphaColour dialog


CDialogAlphaColour::CDialogAlphaColour(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAlphaColour::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAlphaColour)
	//}}AFX_DATA_INIT
}


void CDialogAlphaColour::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAlphaColour)
	DDX_Control(pDX, IDC_SCROLL_RED, m_ScrollRed);
	DDX_Control(pDX, IDC_SCROLL_GREEN, m_ScrollGreen);
	DDX_Control(pDX, IDC_SCROLL_COLOUR, m_ScrollColour);
	DDX_Control(pDX, IDC_SCROLL_BLUE, m_ScrollBlue);
	DDX_Control(pDX, IDC_SCROLL_BLEND, m_ScrollBlend);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAlphaColour, CDialog)
	//{{AFX_MSG_MAP(CDialogAlphaColour)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnButtonView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAlphaColour message handlers

void CDialogAlphaColour::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	//
	// Setup scrollbars.
	//

	// Setup the colour scrollbar.
	m_ScrollColour.SetScrollRange(0, 15, FALSE);
	m_ScrollColour.SetScrollPos(iAlphaColour);

	// Setup the red scrollbar.
	m_ScrollRed.SetScrollRange(0, 255, FALSE);
	m_ScrollRed.SetScrollPos(lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Red);

	// Setup the blue scrollbar.
	m_ScrollGreen.SetScrollRange(0, 255, FALSE);
	m_ScrollGreen.SetScrollPos(lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Green);

	// Setup the green scrollbar.
	m_ScrollBlue.SetScrollRange(0, 255, FALSE);
	m_ScrollBlue.SetScrollPos(lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Blue);

	// Setup the blend scrollbar.
	m_ScrollBlend.SetScrollRange(0, 100, FALSE);
	m_ScrollBlend.SetScrollPos(int(lbAlphaConstant.lpsSettings[iAlphaColour].fAlpha * 100.0f));

	//
	// Setup static text.
	//
	DisplayText();
}

void CDialogAlphaColour::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Move scrollbar.
	MoveScrollbar(nSBCode, nPos, pScrollBar);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// Set variables.
	if (iAlphaColour != m_ScrollColour.GetScrollPos())
	{
		iAlphaColour = m_ScrollColour.GetScrollPos();
		m_ScrollRed.SetScrollPos(lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Red);
		m_ScrollGreen.SetScrollPos(lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Green);
		m_ScrollBlue.SetScrollPos(lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Blue);
		m_ScrollBlend.SetScrollPos(int(lbAlphaConstant.lpsSettings[iAlphaColour].fAlpha * 100.0f));
	}
	else
	{
		lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Red = m_ScrollRed.GetScrollPos();
		lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Green = m_ScrollGreen.GetScrollPos();
		lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Blue = m_ScrollBlue.GetScrollPos();
		lbAlphaConstant.lpsSettings[iAlphaColour].fAlpha = float(m_ScrollBlend.GetScrollPos()) / 100.0f;
		RebuildTable();
	}

	// Display new values.
	DisplayText();
	RedrawScreen();
}

//*********************************************************************************************
//
void CDialogAlphaColour::DisplayText
(
)
//
// Set the text in all related static controls.
//
//**************************************
{	
	SetDlgItemInt(IDC_STATIC_COLOUR, iAlphaColour);
	SetDlgItemFloat(this, IDC_STATIC_RED, float(lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Red) / 255.0f);
	SetDlgItemFloat(this, IDC_STATIC_GREEN, float(lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Green) / 255.0f);
	SetDlgItemFloat(this, IDC_STATIC_BLUE, float(lbAlphaConstant.lpsSettings[iAlphaColour].clrBlendColour.u1Blue) / 255.0f);
	SetDlgItemFloat(this, IDC_STATIC_BLEND, lbAlphaConstant.lpsSettings[iAlphaColour].fAlpha);
}

//*********************************************************************************************
//
void CDialogAlphaColour::RedrawScreen
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
	pgui->AlphaColourTestBar(iAlphaColour);
}

//*********************************************************************************************
//
void CDialogAlphaColour::RebuildTable
(
)
//
// Rebuilds the alpha blend table.
//
//**************************************
{
	lbAlphaConstant.Setup(prasMainScreen.ptPtrRaw(), iAlphaColour);
}

//*********************************************************************************************
//
void CDialogAlphaColour::OnButtonView() 
//
// Responds to clicking on the 'View' button by redrawing the screen.
//
//**************************************
{
	RedrawScreen();
}
