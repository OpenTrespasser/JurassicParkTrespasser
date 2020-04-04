#include "stdafx.h"
#include <string>
#include <set>
#include "Database.h"
#include "CollisionEditor.h"
#include "SampleListDlg.h"


#include <fstream>
#include <ctype.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//**********************************************************************************************
//
CSampleListDlg::CSampleListDlg(SEffect* peff) : CDialog(CSampleListDlg::IDD)
{
	//{{AFX_DATA_INIT(CSampleListDlg)
	//}}AFX_DATA_INIT
	peffSample = peff;
}


//**********************************************************************************************
//
void CSampleListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSampleListDlg)

	DDX_Control(pDX, IDC_SLIDER1,	sliderAttenuation);
	DDX_Control(pDX, IDC_SLIDER2,	sliderMasterVolume);

	//}}AFX_DATA_MAP
}

//**********************************************************************************************
//
BEGIN_MESSAGE_MAP(CSampleListDlg, CDialog)
	//{{AFX_MSG_MAP(CSampleListDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//**********************************************************************************************
//
BOOL CSampleListDlg::OnInitDialog()
{
	char	buf[64];
	CDialog::OnInitDialog();

	CString	str = "Sample Properties : ";
	str+=peffSample->cstrID;
	SetWindowText(str);

	sliderAttenuation.SetRange(0,200,TRUE );
	sliderAttenuation.SetPos(peffSample->fAttenuation*10);
	sprintf(buf,"%.1f dB / m", -peffSample->fAttenuation);
	GetDlgItem(IDC_ATTEN_TEXT)->SetWindowText(buf);

	sliderMasterVolume.SetRange(0,300,TRUE);
	sliderMasterVolume.SetPos(peffSample->fMasterVolume*-10);
	sprintf(buf,"%.1f dB", peffSample->fMasterVolume);
	GetDlgItem(IDC_VOL_TEXT)->SetWindowText(buf);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


//*********************************************************************************************
//
void CSampleListDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	char buf[128];

	// get the ID of the control so we can switch on it
	int					iscr_id=pScrollBar->GetDlgCtrlID();

	// Call base class member function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// after calling the default handler set the slider position
	nPos=((CSliderCtrl*)pScrollBar)->GetPos();

	// which slider have we clicked on, we have the position so do not care
	// about the scroll code
	switch (iscr_id)
	{
	case IDC_SLIDER1:			// Attenuation
		peffSample->fAttenuation = (float)nPos/10.0f;
		sprintf(buf,"%.1f dB / m", -peffSample->fAttenuation);
		GetDlgItem(IDC_ATTEN_TEXT)->SetWindowText(buf);
		break;

	case IDC_SLIDER2:			// volume.
		peffSample->fMasterVolume = (float)(-(int)nPos)/10.0f;
		sprintf(buf,"%.1f dB", peffSample->fMasterVolume);
		GetDlgItem(IDC_VOL_TEXT)->SetWindowText(buf);
		break;

	default:
		break;
	}
}
