// InitGUIApp2Dlg.cpp : implementation file
//

#include "StdAfx.h"
#include "InitGUIApp2.h"
#include "InitGUIApp2Dlg.h"
#include "Lib/Sys/reg.h"
#include "Lib/Sys/RegInit.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int  iNumSubdivisions = -1;
BOOL bStuffChildren   = FALSE;

//**********************************************************************************************
void MoveScrollbar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

/////////////////////////////////////////////////////////////////////////////
// CInitGUIApp2Dlg dialog

CInitGUIApp2Dlg::CInitGUIApp2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInitGUIApp2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInitGUIApp2Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInitGUIApp2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitGUIApp2Dlg)
	DDX_Control(pDX, IDC_CHECK_STUFFCHILDREN, m_ButtonStuff);
	DDX_Control(pDX, IDC_SCROLL_SUBDIVISION, m_ScrollSubdivision);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInitGUIApp2Dlg, CDialog)
	//{{AFX_MSG_MAP(CInitGUIApp2Dlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_STUFFCHILDREN, OnCheckStuffChildren)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitGUIApp2Dlg message handlers

BOOL CInitGUIApp2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CInitGUIApp2Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CInitGUIApp2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CInitGUIApp2Dlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	OpenKey();
	GetPartitionBuildInfo(iNumSubdivisions, bStuffChildren);
	CloseKey();

	::SetDlgItemInt(m_hWnd, IDC_STATIC_SUBDIVISION, iNumSubdivisions, TRUE);
	m_ScrollSubdivision.SetScrollRange(100, 5000, FALSE);
	m_ScrollSubdivision.SetScrollPos(iNumSubdivisions);
	m_ButtonStuff.SetCheck(bStuffChildren);

}

void CInitGUIApp2Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Move scrollbar.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	MoveScrollbar(nSBCode, nPos, pScrollBar);

	iNumSubdivisions = m_ScrollSubdivision.GetScrollPos();
	::SetDlgItemInt(m_hWnd, IDC_STATIC_SUBDIVISION, iNumSubdivisions, TRUE);
}

void CInitGUIApp2Dlg::OnOK() 
{
	OpenKey();
	SetPartitionBuildInfo(iNumSubdivisions, bStuffChildren);
	CloseKey();	
	CDialog::OnOK();
}

void CInitGUIApp2Dlg::OnCheckStuffChildren() 
{
	bStuffChildren = m_ButtonStuff.GetCheck() != 0;
}

//**********************************************************************************************
void MoveScrollbar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int MinPos, MaxPos;

	pScrollBar->GetScrollRange(&MinPos, &MaxPos);

	int ScrollRange = MaxPos - MinPos;
	int FineTune    = 10;
	int CoarseTune  = 100;
	int Pos         = pScrollBar->GetScrollPos();

	switch (nSBCode)
	{
		case SB_LEFT:
			Pos = MinPos;
			break;
		case SB_RIGHT:
			Pos = MaxPos;
			break;
		case SB_PAGELEFT:
			Pos -= CoarseTune;
			if (Pos < MinPos) 
				Pos = MinPos;
			break;
		case SB_LINELEFT:
			Pos -= FineTune;
			if (Pos < MinPos) 
				Pos = MinPos;
			break;
		case SB_PAGERIGHT:
			Pos += CoarseTune;
			if (Pos > MaxPos) 
				Pos = MaxPos;
			break;
		case SB_LINERIGHT:
			Pos += FineTune;
			if (Pos > MaxPos) 
				Pos = MaxPos;
			break;
		case SB_THUMBPOSITION:
			Pos = nPos;
			break;
		case SB_THUMBTRACK:
			//Pos = nPos;
			break;
		default:;
	}
	pScrollBar->SetScrollPos(Pos, TRUE);
}