// DialogGore.cpp : implementation file
//

#include "StdAfx.h"
#include "DialogGore.hpp"
#include "GUITools.hpp"
#include "GUIAppDlg.h"
#include "Lib/EntityDBase/Animate.hpp"

/////////////////////////////////////////////////////////////////////////////
// CDialogGore dialog


CDialogGore::CDialogGore(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogGore::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogGore)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogGore::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogGore)
	DDX_Control(pDX, IDC_SCROLL_GORE, m_ScrollGore);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogGore, CDialog)
	//{{AFX_MSG_MAP(CDialogGore)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogGore message handlers

void CDialogGore::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	// Setup the gore scrollbar.
	m_ScrollGore.SetScrollRange(0, 2, FALSE);
	m_ScrollGore.SetScrollPos(CAnimate::iGoreLevel);

	Update();
}

void CDialogGore::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// Move the scrollbar based on the user's actions.
	MoveScrollbar(nSBCode, nPos, pScrollBar);
	CAnimate::iGoreLevel = m_ScrollGore.GetScrollPos();

	Update();
}

//*********************************************************************************************
//
void CDialogGore::Update
(
)
//
// Update all dialog fields.
//
//**************************************
{
	// Display the current value in the text.	
	SetDlgItemInt(IDC_STATIC_GORE, CAnimate::iGoreLevel);
}

