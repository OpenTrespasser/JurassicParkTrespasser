/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents: Implementation of 'Background.hpp'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogFog.cpp                                                 $
 * 
 * 14    97.11.13 6:12p Mmouni
 * Increased range of z-half setting in fog dialog.
 * 
 * 13    97/11/13 12:30p Pkeet
 * Increased ranges.
 * 
 * 12    97/11/03 5:53p Pkeet
 * Added the 'fogTerrainFog' global variable.
 * 
 * 11    97/06/10 21:44 Speter
 * Fixed dialog to correct for new fog enums.
 * 
 * 10    12/20/96 12:10p Mlange
 * Moved fog source files to 'Renderer' directory.
 * 
 * 9     12/19/96 1:24p Mlange
 * Updated for changes to fog.
 * 
 * 8     96/12/06 15:21 Speter
 * Removed CRaster* argument from CFog::ConstructTable().
 * 
 * 7     11/23/96 5:51p Mlange
 * Updated for the changes to the camera accessor functions.
 * 
 * 6     10/28/96 7:43p Pkeet
 * Removed references to the global camera variable and replaced them with a call to the world
 * object database to get the primary camera.
 * 
 * 5     9/26/96 1:55p Pkeet
 * Added controls to set fog table builds based on linear, inverse or exponential functions.
 * 
 * 4     96/09/25 15:51 Speter
 * Updated for changed fog parameters.
 * 
 * 3     9/13/96 2:06p Pkeet
 * Added includes taken from 'StdAfx.hpp.'
 * 
 * 2     8/20/96 4:48p Pkeet
 * Added fogging switch and dialog.
 * 
 *********************************************************************************************/


//
// Includes.
//
#include "StdAfx.h"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/Renderer/Fog.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/EntityDBase/FilterIterator.hpp"
#include "GUIPipeLine.hpp"
#include "GUITools.hpp"
#include "GUIApp.h"
#include "GUIAppDlg.h"
#include "DialogFog.hpp"


//*********************************************************************************************
//
// CDialogFog implementation.
//


//*********************************************************************************************
//
// Message map for CDialogFog.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogFog, CDialog)
	//{{AFX_MSG_MAP(CDialogFog)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_RADIO_LINEAR, OnRadioLinear)
	ON_BN_CLICKED(IDC_RADIO_INVERSE, OnRadioInverse)
	ON_BN_CLICKED(IDC_RADIO_EXPONENTIAL, OnRadioExponential)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CDialogFog constructor.
//

//*********************************************************************************************
CDialogFog::CDialogFog(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogFog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogFog)
	//}}AFX_DATA_INIT
}


//*********************************************************************************************
//
// CDialogFog member functions.
//

//*********************************************************************************************
//
void CDialogFog::DoDataExchange(CDataExchange* pDX)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogFog)
	DDX_Control(pDX, IDC_SCROLL_FOGPOWER, m_ScrollFogPower);
	DDX_Control(pDX, IDC_SCROLL_FOGCONSTANT, m_ScrollFogConstant);
	//}}AFX_DATA_MAP
}

//*********************************************************************************************
//
void CDialogFog::OnHScroll
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

	// Set fogging variables.
	CFog::SProperties fogprop = fogFog.fogpropGetProperties();

	fogprop.rHalfFogY = m_ScrollFogConstant.GetScrollPos() / 100.0;
	fogprop.rPower	  = m_ScrollFogPower.GetScrollPos()    / 10.0;

	fogFog.SetProperties(fogprop);
	fogTerrainFog.SetProperties(fogprop);

	// Display new fogging values.
	DisplayText();

	// Show new fog.
	Redraw();
}

//*********************************************************************************************
//
void CDialogFog::OnShowWindow
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

	//
	// Setup scrollbars.
	//
	CFog::SProperties fogprop = fogFog.fogpropGetProperties();

	// Setup the constant scrollbar.
	m_ScrollFogConstant.SetScrollRange(1, 500, FALSE);
	m_ScrollFogConstant.SetScrollPos(fogprop.rHalfFogY * 100.0);

	// Setup the power scrollbar.
	m_ScrollFogPower.SetScrollRange(1, 50, FALSE);
	m_ScrollFogPower.SetScrollPos(fogprop.rPower * 10.0);

	//
	// Setup static text.
	//
	DisplayText();

	//
	// Set buttons.
	//
	SetButtons();
}

//*********************************************************************************************
//
void CDialogFog::SetButtons
(
)
//
// Sets all the button states in the dialog.
//
//**************************************
{
	CFog::SProperties fogprop = fogFog.fogpropGetProperties();

	CheckRadioButton
	(
		IDC_RADIO_LINEAR,
		IDC_RADIO_EXPONENTIAL,
		fogprop.efogFunction == CFog::efogLINEAR ? IDC_RADIO_LINEAR : IDC_RADIO_EXPONENTIAL
	);
}

//*********************************************************************************************
//
void CDialogFog::Redraw
(
)
//
// Redraws the main window with updated fog parameters.
//
//**************************************
{
	// Update the fogging table.
	Assert(prasMainScreen);

//	fogFog.ConstructTable(ptCastRenderType<CCamera>(pinsGetCamera()->prdtGetRenderInfo()));

	// Update screen.
	GetParent()->Invalidate();
}

//*********************************************************************************************
//
void CDialogFog::DisplayText
(
)
//
// Set the text in all related static controls.
//
//**************************************
{	
	CFog::SProperties fogprop = fogFog.fogpropGetProperties();

	SetDlgItemFloat(this, IDC_STATIC_FOGCONSTANT, fogprop.rHalfFogY);
	SetDlgItemFloat(this, IDC_STATIC_FOGPOWER,    fogprop.rPower);
}

void CDialogFog::OnRadioLinear() 
{
	CFog::SProperties fogprop = fogFog.fogpropGetProperties();

	// Set fog type flag.
	fogprop.efogFunction = CFog::efogLINEAR;
	fogFog.SetProperties(fogprop);
	fogTerrainFog.SetProperties(fogprop);

	// Show new fog.
	Redraw();
}

void CDialogFog::OnRadioInverse() 
{
	CFog::SProperties fogprop = fogFog.fogpropGetProperties();

	// Set fog type flag.
	fogprop.efogFunction = CFog::efogLINEAR;
	fogFog.SetProperties(fogprop);
	fogTerrainFog.SetProperties(fogprop);

	// Show new fog.
	Redraw();
}

void CDialogFog::OnRadioExponential() 
{
	CFog::SProperties fogprop = fogFog.fogpropGetProperties();

	// Set fog type flag.
	fogprop.efogFunction = CFog::efogEXPONENTIAL;
	fogFog.SetProperties(fogprop);
	fogTerrainFog.SetProperties(fogprop);

	// Show new fog.
	Redraw();
}
