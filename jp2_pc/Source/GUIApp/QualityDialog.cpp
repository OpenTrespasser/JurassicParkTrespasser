/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998
 *
 * Contents: Implementation of QualityDialog.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/QualityDialog.cpp                                             $
 * 
 * 3     6/29/98 8:16p Rwyatt
 * Writes quality setting back to registry
 * 
 * 2     6/10/98 6:37p Rwyatt
 * First working version
 * 
 * 1     6/10/98 6:36p Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/


//// Includes.
//
#include "stdafx.h"
#include "QualityDialog.hpp"
#include "Lib\EntityDBase\WorldDBase.hpp"
#include "Lib/Sys/Reg.h"


//*********************************************************************************************
//
// CDialogQuality implementation.
//


//*********************************************************************************************
//
// Message map for CDialogQuality.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogQuality, CDialog)
	//{{AFX_MSG_MAP(CDialogQuality)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CDialogQuality constructor.
//

//*********************************************************************************************
CDialogQuality::CDialogQuality(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogQuality::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogQuality)
	//}}AFX_DATA_INIT
}



//*********************************************************************************************
//
// CDialogQuality member functions.
//



//*********************************************************************************************
//
void CDialogQuality::DoDataExchange(CDataExchange* pDX)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogQuality)
	DDX_Control(pDX, IDC_SLIDER1, sliderQuality);
	//}}AFX_DATA_MAP
}







//*********************************************************************************************
//
void CDialogQuality::OnHScroll
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
	// get the ID of the control so we can switch on it
	int					iscr_id=pScrollBar->GetDlgCtrlID();

	// Call base class member function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	nPos=((CSliderCtrl*)pScrollBar)->GetPos();

	// which slider have we clicked on, we have the position so do not care
	// about the scroll code
	switch (iscr_id)
	{
	case IDC_SLIDER1:
		SetRegValue("RenderQuality",nPos);
		SetQualitySetting(nPos);
		SetDlgItemInt(IDC_QUALITY_TEXT,iGetQualitySetting());
		break;
	}
}








//*********************************************************************************************
//
void CDialogQuality::OnShowWindow
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

	sliderQuality.SetRange(0,iMAX_QUALITY_SETTING,TRUE);
	sliderQuality.SetPos(iGetQualitySetting());
	SetDlgItemInt(IDC_QUALITY_TEXT,iGetQualitySetting());

	UpdateData(false);
}


