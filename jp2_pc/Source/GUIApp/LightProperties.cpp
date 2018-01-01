/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of LightProperties.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/LightProperties.cpp                                            $
 * 
 * 14    97/09/08 17:28 Speter
 * Fixes for old lighting changes; now works again.
 * 
 * 13    97/02/13 18:12 Speter
 * Changed LightProperties dialog to match new light settings.  Angles are now specified in
 * degrees.
 * 
 * 12    97/02/13 13:32 Speter
 * Updated properties for new light fields.
 * 
 * 11    97/01/07 12:10 Speter
 * Made all CRenderTypes use rptr<>.
 * 
 * 10    12/09/96 1:42p Mlange
 * Updated for changes to the CCamera interface.
 * 
 * 9     96/12/04 20:34 Speter
 * A big change: Updated for new CPresence3<>, and CInstance functions.
 * 
 * 8     11/23/96 5:49p Mlange
 * Updated for the changes to the camera accessor function.
 * 
 * 7     10/28/96 7:43p Pkeet
 * Removed references to the global camera variable and replaced them with a call to the world
 * object database to get the primary camera.
 * 
 * 6     96/10/14 15:42 Speter
 * Fixed bug (probably?) in Asserts.
 * 
 * 5     96/09/23 17:02 Speter
 * Changed for replacement in CPointLight of rOriginDistance with rRadius.
 * 
 * 4     9/13/96 2:12p Pkeet
 * Added includes taken from "stdafx.h."
 * 
 * 3     8/12/96 5:53p Pkeet
 * Added comments.
 * 
 * 2     8/09/96 4:06p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/


//
// Includes.
//
#include "stdafx.h"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/EntityDBase/FilterIterator.hpp"
#include "GUIPipeLine.hpp"
#include "LightProperties.hpp"
#include "GUITools.hpp"


//
// Class implementation.
//


//*********************************************************************************************
//
// CLightProperties implementation.
//

//*********************************************************************************************
//
// CLightProperties message map.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CLightProperties, CDialog)
	//{{AFX_MSG_MAP(CLightProperties)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_EDIT_COS_FULL, OnChangeEditCosFull)
	ON_EN_CHANGE(IDC_EDIT_COS_ZERO, OnChangeEditCosZero)
	ON_EN_CHANGE(IDC_EDIT_FALLOFF, OnChangeEditFalloff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CLightProperties constructor.
//

//*********************************************************************************************
CLightProperties::CLightProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CLightProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLightProperties)
	//}}AFX_DATA_INIT
}


//*********************************************************************************************
//
// CLightProperties member functions.
//

//*********************************************************************************************
void CLightProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLightProperties)
	DDX_Control(pDX, IDC_SCROLL_ORIGINDISTANCE, m_ScrollOriginDistance);
	DDX_Control(pDX, IDC_SCROLL_AMBIENT, m_ScrollIntensity);
	//}}AFX_DATA_MAP
}

//*********************************************************************************************
//
void CLightProperties::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//
// Respond to a WM_HSCROLL message.
//
//**************************************
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	MoveScrollbar(nSBCode, nPos, pScrollBar);
	ActOnChange();
}

//*********************************************************************************************
void CLightProperties::OnShowWindow(BOOL bShow, UINT nStatus)
//
// Responds to a WM_SHOWWINDOW message. Initializes scrollbars and text for static control.
//
//**************************************
{
	CDialog::OnShowWindow(bShow, nStatus);

	//
	// Display light type.
	//
	if (pltpdPointDirectional)
		SetDlgItemText(IDC_STATIC_LIGHTTYPE, "Point Directional");
	else
		if (pltPoint)
			SetDlgItemText(IDC_STATIC_LIGHTTYPE, "Point");
		else
			if (pltdDirectional)
				SetDlgItemText(IDC_STATIC_LIGHTTYPE, "Directional");
			else
				SetDlgItemText(IDC_STATIC_LIGHTTYPE, "Ambient");

	//
	// Resize the dialog box according to the light type.
	//
	RECT rect_dialog;
	RECT rect_control;

	GetWindowRect(&rect_dialog);

	if (!pltPoint)
	{
		GetDlgItem(IDC_STATIC_ORIGINDISTANCE_START)->GetWindowRect(&rect_control);
	}
	else
	{
		if (pltpdPointDirectional)
		{
			GetDlgItem(IDC_STATIC_LAST_START)->GetWindowRect(&rect_control);
		}
		else
		{
			GetDlgItem(IDC_STATIC_POINTDIR_START)->GetWindowRect(&rect_control);
		}
	}
	rect_dialog.bottom = rect_control.top + 5;
	MoveWindow(&rect_dialog);

	//
	// Setup scrollbars.
	//

	// Setup the CLightProperties light scrollbar.
	m_ScrollIntensity.SetScrollRange(0, 100, FALSE);
	m_ScrollIntensity.SetScrollPos((int)(pltaAmbient->lvIntensity * 100.0f + 0.5f));

	// Setup the OriginDistance scrollbar.
	if (pltPoint)
	{
		m_ScrollOriginDistance.SetScrollRange(0, 1000, FALSE);
		m_ScrollOriginDistance.SetScrollPos((int)(pltPoint->rRadius * 100.0f + 0.5f));
	}

	//
	// Setup the edit controls for CLightPointDirectional.
	//
	if (pltpdPointDirectional)
	{
		SetDlgItemFloat(this, IDC_EDIT_COS_FULL, 
			fAngleFromAngleWidth(pltpdPointDirectional->angwInner) / dDEGREES);
		SetDlgItemFloat(this, IDC_EDIT_COS_ZERO, 
			fAngleFromAngleWidth(pltpdPointDirectional->angwOuter) / dDEGREES);
	}

	//
	// Setup static text.
	//
	DisplayText();
}

//*********************************************************************************************
void CLightProperties::Show(rptr<CLight> plt)
{
	if (!plt)
		return;
	pltaAmbient           = rptr_dynamic_cast(CLightAmbient, plt);
	pltdDirectional       = rptr_dynamic_cast(CLightDirectional, plt);
	pltPoint              = rptr_dynamic_cast(CLightPoint, plt);
	pltpdPointDirectional = rptr_dynamic_cast(CLightPointDirectional, plt);

	ShowWindow(SW_SHOWNORMAL);
}

//*********************************************************************************************
//
void CLightProperties::ActOnChange()
//
// Change the parameters of the selected light based on changes in scrollbar positions or
// CEdit text boxes.
//
//**************************************
{
	Assert(GetParent());

	//
	// Set CLightProperties lighting level.
	//
	pltaAmbient->lvIntensity = (float)m_ScrollIntensity.GetScrollPos() / 100.0f;
	if (pltPoint)
	{
		pltPoint->rRadius = (float)m_ScrollOriginDistance.GetScrollPos() / 100.0f;
	}

	//
	// Update static text.
	//
	DisplayText();

	//
	// Update main window display.
	//
	GetParent()->Invalidate();
}

//*********************************************************************************************
//
void CLightProperties::DisplayText()
//
// Set the text in all related static controls.
//
//**************************************
{
	SetDlgItemFloat(this, IDC_STATIC_AMBIENT, pltaAmbient->lvIntensity);
	if (pltPoint)
	{
		SetDlgItemFloat(this, IDC_STATIC_ORIGINDISTANCE, pltPoint->rRadius);
	}
}

//*********************************************************************************************
//
void CLightProperties::OnChangeEditCosFull() 
//
// Changed the text in the IDC_EDIT_COS_FULL control.
//
//**************************************
{
	pltpdPointDirectional->angwInner = angwFromAngle
	(
		fGetDlgItemFloat(this, IDC_EDIT_COS_FULL) * dDEGREES
	);
}

//*********************************************************************************************
//
void CLightProperties::OnChangeEditCosZero() 
//
// Changed the text in the IDC_EDIT_COS_ZERO control.
//
//**************************************
{
	pltpdPointDirectional->angwOuter = angwFromAngle
	(
		fGetDlgItemFloat(this, IDC_EDIT_COS_ZERO) * dDEGREES
	);
}

//*********************************************************************************************
//
void CLightProperties::OnChangeEditFalloff
(
) 
//
// Changed the text in the IDC_EDIT_FALLOFF control.
//
//**************************************
{
}
