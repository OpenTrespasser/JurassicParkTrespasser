/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents: Implementation of 'Background.hpp'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogString.cpp                                              $
 * 
 * 3     10/02/97 9:49p Agrant
 * Specify string caption and default at creation time.
 * 
 * 2     10/02/97 8:11p Rwyatt
 * 
 * 1     10/02/97 8:07p Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/


//
// Includes.
//
#include "stdafx.h"
#include "DialogString.hpp"


//*********************************************************************************************
//
// CDialogString implementation.
//


//*********************************************************************************************
//
// Message map for CDialogString.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogString, CDialog)
	//{{AFX_MSG_MAP(CDialogString)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CDialogString constructor.
//

//*********************************************************************************************
CDialogString::	CDialogString(char *ac_caption, char *ac_default_text /*= ""*/, CWnd* pParent /*= NULL*/)
	: CDialog(CDialogString::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogString)
	strText		= _T(ac_default_text);
	//}}AFX_DATA_INIT

	strCaption = ac_caption;
}


//*********************************************************************************************
//
// CDialogString member functions.
//

//*********************************************************************************************
//
void CDialogString::DoDataExchange(CDataExchange* pDX)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogString)
	DDX_Text(pDX, IDC_TEXT1, strText);
	//}}AFX_DATA_MAP
}



//*********************************************************************************************
//
void CDialogString::OnShowWindow
(
	BOOL bShow,
	UINT nStatus
)
//
// Responds to the WM_SHOWWINDOW message by initializing dialog controls.
//
//**************************************
{
	// Call the base class member function.
	CDialog::OnShowWindow(bShow, nStatus);

	SetWindowText(strCaption);
	GetDlgItem(IDC_TEXT1)->SetFocus();
}

