/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998
 *
 * Contents: Implementation of 'TexturePackOptions.hpp'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogTexturePackOptions.cpp                                  $
 * 
 * 3     1/14/98 6:24p Rwyatt
 * New check box to force the texture packer to pack curved bump maps as power of 2 maps. This
 * is for debugging and testing only
 * 
 * 2     1/09/98 6:47p Rwyatt
 * 
 * 1     1/09/98 6:47p Rwyatt
 * Initial implementation
 * 
 *********************************************************************************************/


//
// Includes.
//
#include "StdAfx.h"
#include "GUIApp.h"
#include "GUITools.hpp"
#include "DialogTexturePackOptions.hpp"
#include "Lib/Loader/TextureManager.hpp"


//*********************************************************************************************
//
// CDialogTexturePackOptions implementation.
//


//*********************************************************************************************
//
// Message map for CDialogTexturePackOptions.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogTexturePackOptions, CDialog)
	//{{AFX_MSG_MAP(CDialogTexturePackOptions)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK1, OnPackBumps)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CDialogTexturePackOptions constructor.
//

//*********************************************************************************************
CDialogTexturePackOptions::CDialogTexturePackOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogTexturePackOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogTexturePackOptions)
	//}}AFX_DATA_INIT
}


//*********************************************************************************************
//
// CDialogTexturePackOptions member functions.
//

//*********************************************************************************************
//
void CDialogTexturePackOptions::DoDataExchange(CDataExchange* pDX)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogTexturePackOptions)
	DDX_Control(pDX, IDC_SCROLLBAR1, sbPack);
	//}}AFX_DATA_MAP
}

//*********************************************************************************************
//
void CDialogTexturePackOptions::OnHScroll
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
	// Move scrollbar.
	MoveScrollbar(nSBCode, nPos, pScrollBar);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	gtxmTexMan.SetMipLevelPackingDifference(sbPack.GetScrollPos());
	char buf[32];
	wsprintf(buf,"%d",sbPack.GetScrollPos());
	GetDlgItem(IDC_TEXT_MIP_DIFF)->SetWindowText(buf);
}


//*********************************************************************************************
//
void CDialogTexturePackOptions::OnShowWindow
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

	// Setup the constant scrollbar.
	sbPack.SetScrollRange(0,6,FALSE);
	sbPack.SetScrollPos(gtxmTexMan.i4GetMipLevelPackingDifference());

	char buf[32];
	wsprintf(buf,"%d",sbPack.GetScrollPos());
	GetDlgItem(IDC_TEXT_MIP_DIFF)->SetWindowText(buf);

	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck( gtxmTexMan.bPackCurvedBumpMaps());
}


//*********************************************************************************************
//
void CDialogTexturePackOptions::OnPackBumps
(
)
//
//**************************************
{
	gtxmTexMan.PackCurvedBumpMaps( ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() );
}