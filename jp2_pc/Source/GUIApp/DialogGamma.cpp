// DialogGamma.cpp : implementation file
//

#include "StdAfx.h"
#include "DialogGamma.hpp"
#include "GUITools.hpp"
#include "GUIAppDlg.h"
#include "Lib/Renderer/PipeLine.hpp"

const float fMaxPower = 3.0f;
const float fMinPower = 0.1f;

const float fMaxStart = 1.0f;
const float fMinStart = 0.0f;

/////////////////////////////////////////////////////////////////////////////
// CDialogGamma dialog


CDialogGamma::CDialogGamma(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogGamma::IDD, pParent)
{
	bEnable = true;
	fPower = 1.0;
	//{{AFX_DATA_INIT(CDialogGamma)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogGamma::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogGamma)
	DDX_Control(pDX, IDC_ENABLE_GAMMA, m_ButtonGamma);
	DDX_Control(pDX, IDC_SCROLL_GAMMA, m_ScrollGamma);
	DDX_Control(pDX, IDC_SCROLL_MIN_AMBIENT, m_ScrollClutStart);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogGamma, CDialog)
	//{{AFX_MSG_MAP(CDialogGamma)
	ON_BN_CLICKED(IDC_ENABLE_GAMMA, OnEnableGamma)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogGamma message handlers

void CDialogGamma::OnEnableGamma() 
{
	bEnable = m_ButtonGamma.GetCheck() != 0;
	Update();
}

void CDialogGamma::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	// Set the checkmark in the enable gamma function.
	m_ButtonGamma.SetCheck(bEnable);

	// Setup the gamma scrollbar.
	if (bEnable)
		fPower = CClu::gcfMain.fGamma;
	m_ScrollGamma.SetScrollRange(0, 255, FALSE);
	m_ScrollGamma.SetScrollPos(int((fPower / (fMaxPower - fMinPower)) * 255.0));

	// Setup the min ambient scrollbar.
	m_ScrollClutStart.SetScrollRange(0, 100, FALSE);
	m_ScrollClutStart.SetScrollPos(int((lvDefStart / (fMaxStart - fMinStart)) * 100.0));

	prenMain->pSettings->bTestGamma = true;

	Update();
}

void CDialogGamma::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// Move the scrollbar based on the user's actions.
	MoveScrollbar(nSBCode, nPos, pScrollBar);
	fPower = float(m_ScrollGamma.GetScrollPos()) * (fMaxPower - fMinPower) / 255.0;
	lvDefStart = float(m_ScrollClutStart.GetScrollPos()) * (fMaxStart - fMinStart) / 100.0;

	Update();
}


void CDialogGamma::OnClose() 
{
	prenMain->pSettings->bTestGamma = false;
	Update();
	CDialog::OnClose();
}

void CDialogGamma::OnOK() 
{
	prenMain->pSettings->bTestGamma = false;
	Update();
	CDialog::OnOK();
}

//*********************************************************************************************
//
void CDialogGamma::Update
(
)
//
// Update the world gamma setting, repaint the main window, and update all dialog fields.
//
//**************************************
{
	// Display the current value in the text.	
	SetDlgItemFloat(this, IDC_STATIC_GAMMA, fPower, 2);
	SetDlgItemFloat(this, IDC_STATIC_MIN_AMBIENT, lvDefStart, 2);

	// Update the renderer.
	CClu::gcfMain.Set
	(
		CClu::gcfMain.tOutputRange, 
		bEnable ? fPower : 1.0
	);

	pcdbMain.UpdateCluts();

	// Repaint the main window.
	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());
	Assert(pappdlg);
	pappdlg->Invalidate();
}

