// DialogName.cpp : implementation file
//

#include "StdAfx.h"
#include "TrespassAdv.h"
#include "DialogName.hpp"
#undef min
#undef max
#include "Lib/W95/Direct3DQuery.hpp"

/////////////////////////////////////////////////////////////////////////////
// CDialogName dialog


CDialogName::CDialogName(char* str_name, CWnd* pParent /*=NULL*/)
	: strName(str_name), CDialog(CDialogName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogName)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogName)
	DDX_Control(pDX, IDC_EDIT_NAME, m_EditName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogName, CDialog)
	//{{AFX_MSG_MAP(CDialogName)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogName message handlers

void CDialogName::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	m_EditName.Clear();
	m_EditName.SetWindowText(strName);
	m_EditName.SetFocus();
	int i_line_len = m_EditName.LineLength();
	m_EditName.SetSel(0, i_line_len, 0);	
}

void CDialogName::OnOK() 
{
	// TODO: Add extra validation here
	m_EditName.GetWindowText(strName, iCARDNAME_LEN - 1);
	
	CDialog::OnOK();
}
