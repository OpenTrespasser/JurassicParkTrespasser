/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Implementation of "DialogMagnet.h."
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogMagnet.cpp                                              $
 * 
 * 5     98/08/22 21:42 Speter
 * Added translational magnet params.
 * 
 * 4     98/06/30 22:19 Speter
 * Updated for new magnet stuff.
 * 
 * 3     3/22/98 5:02p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 2     98/02/24 21:11 Speter
 * Updated for shoulder magnet.
 * 
 * 1     98/02/05 18:30 Speter
 * 
 * 
 *********************************************************************************************/

#include "StdAfx.h"
#include "DialogMagnet.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogMagnet dialog


CDialogMagnet::CDialogMagnet(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMagnet::IDD, pParent), pmpMagnet(0)
{
	//{{AFX_DATA_INIT(CDialogMagnet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogMagnet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogMagnet)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogMagnet, CDialog)
	//{{AFX_MSG_MAP(CDialogMagnet)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogMagnet message handlers

void CDialogMagnet::SetMagnet(CMagnetPair* pmp) 
{
	Assert(pmp);
	Assert(pmp->pmagData);
	pmpMagnet = pmp;
	magData = *pmpMagnet->pmagData;
}

void CDialogMagnet::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	Assert(pmpMagnet->pinsMaster);
	SetDlgItemText(IDC_EDIT_MASTER,		pmpMagnet->pinsMaster->strGetInstanceName() );
	SetDlgItemText(IDC_EDIT_SLAVE,		pmpMagnet->pinsSlave ? pmpMagnet->pinsSlave->strGetInstanceName() : "");

	CheckDlgButton(IDC_CHECK_BREAKABLE,	magData.setFlags[emfBREAKABLE]);
	CheckDlgButton(IDC_CHECK_LIMITS,	magData.setFlags[emfLIMITS]);
	CheckDlgButton(IDC_CHECK_XTFREE,	magData.setFlags[emfXTFREE]);
	CheckDlgButton(IDC_CHECK_YTFREE,	magData.setFlags[emfYTFREE]);
	CheckDlgButton(IDC_CHECK_ZTFREE,	magData.setFlags[emfZTFREE]);
	CheckDlgButton(IDC_CHECK_XFREE,		magData.setFlags[emfXFREE]);
	CheckDlgButton(IDC_CHECK_YFREE,		magData.setFlags[emfYFREE]);
	CheckDlgButton(IDC_CHECK_ZFREE,		magData.setFlags[emfZFREE]);
	CheckDlgButton(IDC_CHECK_PICKUP,	magData.setFlags[emfHAND_HOLD]);
	CheckDlgButton(IDC_CHECK_ORIENT,	magData.setFlags[emfSHOULDER_HOLD]);

	char str_dud[30];

	sprintf(str_dud, "%f", magData.fBreakStrength);
	SetDlgItemText(IDC_EDIT_MAG_STRENGTH, magData.setFlags[emfBREAKABLE] ? str_dud : "");

	sprintf(str_dud, "%.0f", magData.fAngleMin / dDEGREES);
	SetDlgItemText(IDC_EDIT_MAG_MIN, magData.setFlags[emfLIMITS] ? str_dud : "");

	sprintf(str_dud, "%.0f", magData.fAngleMax / dDEGREES);
	SetDlgItemText(IDC_EDIT_MAG_MAX, magData.setFlags[emfLIMITS] ? str_dud : "");

	sprintf(str_dud, "%f", magData.fRestore);
	SetDlgItemText(IDC_EDIT_MAG_RESTORE, str_dud);

	sprintf(str_dud, "%f", magData.fFriction);
	SetDlgItemText(IDC_EDIT_MAG_FRICTION, str_dud);

	sprintf(str_dud, "%f", magData.fDrive);
	SetDlgItemText(IDC_EDIT_MAG_DRIVE, str_dud);
}

BOOL CDialogMagnet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam == IDOK)
	{
		// Replace magnet properties with new ones.
		magData.setFlags[emfBREAKABLE]		= IsDlgButtonChecked(IDC_CHECK_BREAKABLE);
		magData.setFlags[emfLIMITS]			= IsDlgButtonChecked(IDC_CHECK_LIMITS);
		magData.setFlags[emfXTFREE]			= IsDlgButtonChecked(IDC_CHECK_XTFREE);
		magData.setFlags[emfYTFREE]			= IsDlgButtonChecked(IDC_CHECK_YTFREE);
		magData.setFlags[emfZTFREE]			= IsDlgButtonChecked(IDC_CHECK_ZTFREE);
		magData.setFlags[emfXFREE]			= IsDlgButtonChecked(IDC_CHECK_XFREE);
		magData.setFlags[emfYFREE]			= IsDlgButtonChecked(IDC_CHECK_YFREE);
		magData.setFlags[emfZFREE]			= IsDlgButtonChecked(IDC_CHECK_ZFREE);
		magData.setFlags[emfHAND_HOLD]		= IsDlgButtonChecked(IDC_CHECK_PICKUP);
		magData.setFlags[emfSHOULDER_HOLD]	= IsDlgButtonChecked(IDC_CHECK_ORIENT);

		char str_dud[30];
		if (magData.setFlags[emfBREAKABLE])
		{
			GetDlgItemText(IDC_EDIT_MAG_STRENGTH, str_dud, 29);
			magData.fBreakStrength = atof(str_dud);
		}
		if (magData.setFlags[emfLIMITS])
		{
			GetDlgItemText(IDC_EDIT_MAG_MIN, str_dud, 29);
			magData.fAngleMin = atof(str_dud) * dDEGREES;

			GetDlgItemText(IDC_EDIT_MAG_MAX, str_dud, 29);
			magData.fAngleMax = atof(str_dud) * dDEGREES;
		}

		GetDlgItemText(IDC_EDIT_MAG_RESTORE, str_dud, 29);
		magData.fRestore = atof(str_dud);

		GetDlgItemText(IDC_EDIT_MAG_FRICTION, str_dud, 29);
		magData.fFriction = atof(str_dud);

		GetDlgItemText(IDC_EDIT_MAG_DRIVE, str_dud, 29);
		magData.fDrive = atof(str_dud);

		pmpMagnet->pmagData = CMagnet::pmagFindShared(magData);
	}
	
	return CDialog::OnCommand(wParam, lParam);
}

