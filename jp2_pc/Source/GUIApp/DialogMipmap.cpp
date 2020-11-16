// DialogMipmap.cpp : implementation file
//

#include "StdAfx.h"
#include "GUIApp.h"
#include "common.hpp"
#include "Lib/Renderer/Texture.hpp"
#include "GUIPipeLine.hpp"
#include "GUITools.hpp"
#include "GUIApp.h"
#include "GUIAppDlg.h"
#include "DialogMipmap.hpp"

/////////////////////////////////////////////////////////////////////////////
// CDialogMipmap dialog


CDialogMipmap::CDialogMipmap(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMipmap::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogMipmap)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogMipmap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogMipmap)
	DDX_Control(pDX, IDC_SCROLL_MIP_THRESHOLD, m_ScrollThreshold);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogMipmap, CDialog)
	//{{AFX_MSG_MAP(CDialogMipmap)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogMipmap message handlers

//*********************************************************************************************
//
void CDialogMipmap::OnShowWindow(BOOL bShow, UINT nStatus) 
//
// Set the text in all related static controls.
//
//**************************************
{
	CDialog::OnShowWindow(bShow, nStatus);

	//
	// Setup scrollbars.
	//

	// Setup the threshold scrollbar.
	m_ScrollThreshold.SetScrollRange(1, 200, FALSE);
	m_ScrollThreshold.SetScrollPos(int(CTexture::fMipmapThreshold * 100.0f));

	//
	// Setup static text.
	//
	DisplayText();
	
}

//*********************************************************************************************
//
void CDialogMipmap::DisplayText
(
)
//
// Set the text in all related static controls.
//
//**************************************
{	
	SetDlgItemFloat(this, IDC_STATIC_MIP_THRESHOLD, CTexture::fMipmapThreshold);
}

//*********************************************************************************************
//
void CDialogMipmap::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
//
// Responds to the WM_HSCROLL message.
//
//**************************************
{
	// Move scrollbar.
	MoveScrollbar(nSBCode, nPos, pScrollBar);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// Set variables.
	CTexture::fMipmapThreshold = float(m_ScrollThreshold.GetScrollPos()) / 100.0f;

	// Display new values.
	DisplayText();
	GetParent()->Invalidate();
}