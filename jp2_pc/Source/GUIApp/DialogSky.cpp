/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents: Implementation of Sky Dialog
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogSky.cpp                                                 $
 * 
 * 3     10/22/97 5:04p Rwyatt
 * New slider for perspective sub division
 * Slider ranges are now sensible
 * 
 * 2     10/14/97 7:01p Rwyatt
 * First Check in
 * 
 * 1     10/14/97 7:00p Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/


//
// Includes.
//
#include "StdAfx.h"
#include "DialogSky.hpp"
#include "Lib/Renderer/Sky.hpp"


//*********************************************************************************************
//
// CDialogSky implementation.
//


//*********************************************************************************************
//
// Message map for CDialogSky.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogSky, CDialog)
	//{{AFX_MSG_MAP(CDialogSky)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CDialogSky constructor.
//

//*********************************************************************************************
CDialogSky::	CDialogSky(CWnd* pParent /*= NULL*/)
	: CDialog(CDialogSky::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSky)
//	strText		= _T(ac_default_text);
	//}}AFX_DATA_INIT

}


//*********************************************************************************************
//
// CDialogSky member functions.
//

//*********************************************************************************************
//
void CDialogSky::DoDataExchange(CDataExchange* pDX)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSky)
	DDX_Control(pDX,		IDC_SLIDER1,			sliderSkyHeight);
	DDX_Control(pDX,		IDC_SLIDER2,			sliderSkyNearFog);
	DDX_Control(pDX,		IDC_SLIDER3,			sliderSkyFarFog);
	DDX_Control(pDX,		IDC_SLIDER4,			sliderSkyScale);
	DDX_Control(pDX,		IDC_SLIDER5,			sliderWindX);
	DDX_Control(pDX,		IDC_SLIDER6,			sliderWindY);
	DDX_Control(pDX,		IDC_SLIDER7,			sliderDivide);

	DDX_Text(pDX,			IDC_TEXT1,				strSkyHeight);
	DDX_Text(pDX,			IDC_TEXT2,				strSkyNearFog);
	DDX_Text(pDX,			IDC_TEXT3,				strSkyFarFog);
	DDX_Text(pDX,			IDC_TEXT4,				strSkyScale);
	DDX_Text(pDX,			IDC_TEXT5,				strWindX);
	DDX_Text(pDX,			IDC_TEXT6,				strWindY);
	DDX_Text(pDX,			IDC_TEXT7,				strDivide);
	//}}AFX_DATA_MAP
}



//*********************************************************************************************
//
void CDialogSky::OnShowWindow
(
	BOOL bShow,
	UINT nStatus
)
//
// Responds to the WM_SHOWWINDOW message by initializing dialog controls.
//
//**************************************
{
	char	buf[128];
	float	f_x, f_y;

	// Call the base class member function.
	CDialog::OnShowWindow(bShow, nStatus);

	sliderSkyHeight.SetRange(100,2500,FALSE);
	sliderSkyHeight.SetPos((uint32)gpskyRender->fGetHeight());
	sprintf(buf,"%.2f m",gpskyRender->fGetHeight());
	strSkyHeight = buf;

	sliderSkyScale.SetRange(1,100,FALSE);
	sliderSkyScale.SetPos((uint32)(gpskyRender->fGetPixelsPerMeter()*1000.0f));
	sprintf(buf,"%.3f pix/m",gpskyRender->fGetPixelsPerMeter() );
	strSkyScale = buf;

	sliderSkyNearFog.SetRange(0,100,FALSE);
	sliderSkyNearFog.SetPos((uint32)(gpskyRender->fGetFogNear()*100.0f));
	sprintf(buf,"%.2f", gpskyRender->fGetFogNear() );
	strSkyNearFog = buf;

	sliderSkyFarFog.SetRange(0,100,FALSE);
	sliderSkyFarFog.SetPos((uint32)(gpskyRender->fGetFogFar()*100.0f));
	sprintf(buf,"%.2f", gpskyRender->fGetFogFar() );
	strSkyFarFog = buf;

	gpskyRender->GetSkyWind(f_x,f_y);

	sliderWindX.SetRange(1,100,FALSE);
	sliderWindX.SetPos((uint32)(f_x*10.0f));
	sprintf(buf,"%.2f m/s", f_x );
	strWindX = buf;

	sliderWindY.SetRange(1,100,FALSE);
	sliderWindY.SetPos((uint32)(f_y*10.0f));
	sprintf(buf,"%.2f m/s", f_y );
	strWindY = buf;

	sliderDivide.SetRange(1,32,FALSE);
	sliderDivide.SetPos(gpskyRender->u4GetDivisionLength()/2);
	sprintf(buf,"%d pixels", gpskyRender->u4GetDivisionLength());
	strDivide = buf;

	UpdateData(false);
}


//*********************************************************************************************
//
void CDialogSky::OnHScroll
(
	UINT nSBCode, 
	UINT nPos, 
	CScrollBar* pScrollBar
)
//
// respond to WM_HSCROLL to move the slider bars.
//
//**************************************
{
	// get the ID of the control so we can switch on it
	char				buf[128];
	int					iscr_id=pScrollBar->GetDlgCtrlID();
	CClientDC			dc(this);
	bool				b_draw = false;
	float				f_sky_height = gpskyRender->fGetHeight();
	float				f_sky_scale  = gpskyRender->fGetPixelsPerMeter();
	float				f_sky_near	 = gpskyRender->fGetFogNear();
	float				f_sky_far	 = gpskyRender->fGetFogFar();
	float				f_x, f_y;
	uint32				u4_division	 = gpskyRender->u4GetDivisionLength();

	// Call base class member function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	gpskyRender->GetSkyWind(f_x,f_y);

	// after calling the default handler set the slider position
	nPos=((CSliderCtrl*)pScrollBar)->GetPos();

	// which slider have we clicked on, we have the position so do not care
	// about the scroll code
	switch (iscr_id)
	{
	case IDC_SLIDER1:
		f_sky_height = (float)nPos;
		b_draw = true;
		sprintf(buf,"%.2f m",f_sky_height);
		strSkyHeight = buf;
		break;

	case IDC_SLIDER4:
		f_sky_scale = (float)nPos / 1000.0f;
		b_draw = true;
		sprintf(buf,"%.3f pix/m",f_sky_scale);
		strSkyScale = buf;
		break;

	case IDC_SLIDER2:
		f_sky_near = (float)nPos /100.0f;
		if (f_sky_near >= f_sky_far)
		{
			f_sky_near = f_sky_far;
			((CSliderCtrl*)pScrollBar)->SetPos( (uint32) (f_sky_near*100.0f) );
		}
		b_draw = true;
		sprintf(buf,"%.2f",f_sky_near);
		strSkyNearFog = buf;
		break;

	case IDC_SLIDER3:
		f_sky_far = (float)nPos/100.0f;
		if (f_sky_far <= f_sky_near)
		{
			f_sky_far = f_sky_near;
			((CSliderCtrl*)pScrollBar)->SetPos( (uint32) (f_sky_far*100.0f) );
		}
		b_draw = true;
		sprintf(buf,"%.2f",f_sky_far);
		strSkyFarFog = buf;
		break;

	case IDC_SLIDER5:
		f_x = (float)nPos / 10.0f;
		b_draw = true;
		sprintf(buf,"%.2f m",f_x);
		strWindX = buf;
		break;

	case IDC_SLIDER6:
		f_y = (float)nPos / 10.0f;
		b_draw = true;
		sprintf(buf,"%.2f m",f_y);
		strWindY = buf;
		break;

	case IDC_SLIDER7:
		u4_division = nPos * 2;
		b_draw = true;
		sprintf(buf,"%d pixels",u4_division);
		strDivide = buf;
		break;

	}

	//
	// Update main window display.
	//
	if (b_draw)
	{
		UpdateData(false);
		gpskyRender->SetWorkingConstants
		(
			f_sky_scale,
			f_sky_height,
			f_sky_near,
			f_sky_far,
			u4_division
		);
		
		gpskyRender->SetSkyWind(f_x,f_y);

		GetParent()->Invalidate();
	}
}