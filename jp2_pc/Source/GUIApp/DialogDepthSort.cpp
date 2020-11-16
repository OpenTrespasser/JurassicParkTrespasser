/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Implementation of DialogDepthSort.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogDepthSort.cpp                                           $
 * 
 * 7     98.02.04 1:57p Mmouni
 * Added new settings for depth sort partitoning.
 * 
 * 6     97/07/30 2:29p Pkeet
 * Added support for separate terrain tolerances.
 * 
 * 5     97/05/26 5:34p Pkeet
 * Added the maximum number of polygons that can be depth sorted scroll bar.
 * 
 * 4     97/05/21 6:29p Pkeet
 * Added pixel tolerance scrollbar.
 * 
 * 3     97/05/20 4:22p Pkeet
 * Set up linear and inverse selection buttons.
 * 
 * 2     97/05/20 3:14p Pkeet
 * Implemented all features for the dialog as it stands.
 * 
 * 1     97/05/20 12:11p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#include <math.h>
#include "StdAfx.h"
#include "GUIApp.h"
#include "DialogDepthSort.hpp"
#include "Lib/Renderer/DepthSort.hpp"
#include "GUITools.hpp"
#include "GUIAppDlg.h"

//
// Constants.
//

// Constant for tolerance to scrollbar position conversion.
const double dLogRange = 5.0;
const int    iLogRange = 100;


//*********************************************************************************************
int iGetPos(float f)
{
	double d = log(f) / log(0.5) * dLogRange;

	int i = int(d + 0.5);
	i = iLogRange - i;
	return i;
}

//*********************************************************************************************
float fGetVal(int i)
{
	double d = double(iLogRange - i) / dLogRange;
	return float(pow(0.5, d));
}


//*********************************************************************************************
//
// CDialogDepthSort implementation.
//

//*********************************************************************************************
//
// Message map for CDialogDepthSort.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogDepthSort, CDialog)
	//{{AFX_MSG_MAP(CDialogDepthSort)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(ID_RADIO_INVERSE, OnRadioInverse)
	ON_BN_CLICKED(ID_RADIO_LINEAR, OnRadioLinear)
	ON_BN_CLICKED(IDC_CHECK_TERRAIN, OnCheckTerrain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*********************************************************************************************
CDialogDepthSort::CDialogDepthSort(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDepthSort::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDepthSort)
	//}}AFX_DATA_INIT
}

//*********************************************************************************************
void CDialogDepthSort::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDepthSort)
	DDX_Control(pDX, IDC_SCROLL_NEARZ_TERRAIN, m_NearZTerrain);
	DDX_Control(pDX, IDC_SCROLL_NEARTOL_TERRAIN, m_NearTolTerrain);
	DDX_Control(pDX, IDC_SCROLL_FARZ_TERRAIN, m_FarZTerrain);
	DDX_Control(pDX, IDC_SCROLL_FARTOL_TERRAIN, m_FarTolTerrain);
	DDX_Control(pDX, IDC_CHECK_TERRAIN, m_CheckTerrain);
	DDX_Control(pDX, IDC_SCROLL_MAXNUM, m_MaxNum);
	DDX_Control(pDX, IDC_SCROLL_PIXELTOL, m_ScrollPixelTolerance);
	DDX_Control(pDX, IDC_SCROLL_NEARZ, m_ScrollNearZ);
	DDX_Control(pDX, IDC_SCROLL_NEARTOL, m_ScrollNearTolerance);
	DDX_Control(pDX, IDC_SCROLL_FARZ_NO, m_ScrollFarZNoDepthSort);
	DDX_Control(pDX, IDC_SCROLL_FARZ, m_ScrollFarZ);
	DDX_Control(pDX, IDC_SCROLL_FARTOL, m_ScrollFarTolerance);
	DDX_Control(pDX, IDC_SCROLL_BINARYAT, m_BinaryAt);
	DDX_Control(pDX, IDC_SCROLL_QUADAT, m_QuadAt);
	//}}AFX_DATA_MAP
}

//*********************************************************************************************

//*********************************************************************************************
//
void CDialogDepthSort::Redraw
(
)
//
// Redraws the main window with updated fog parameters.
//
//**************************************
{
	// Update main screen.
	GetParent()->Invalidate();
}

//*********************************************************************************************
//
void CDialogDepthSort::DisplayText
(
)
//
// Set the text in all related static controls.
//
//**************************************
{
	SetDlgItemFloat(this, IDC_STATIC_NEARTOL,  ptsTolerances.rNearZScale);
	SetDlgItemFloat(this, IDC_STATIC_FARTOL,   ptsTolerances.rFarZScale);
	SetDlgItemFloat(this, IDC_STATIC_NEARZ,    ptsTolerances.rNearZ);
	SetDlgItemFloat(this, IDC_STATIC_FARZ,     ptsTolerances.rFarZ);
	SetDlgItemFloat(this, IDC_STATIC_FARZ_NO,  ptsTolerances.rFarZNoDepthSort);
	SetDlgItemFloat(this, IDC_STATIC_PIXELTOL, ptsTolerances.fPixelBuffer);
	SetDlgItemFloat(this, IDC_STATIC_NEARTOL_TERRAIN, ptsTolerances.rNearZScaleTerrain);
	SetDlgItemFloat(this, IDC_STATIC_FARTOL_TERRAIN,  ptsTolerances.rFarZScaleTerrain);
	SetDlgItemFloat(this, IDC_STATIC_NEARZ_TERRAIN,   ptsTolerances.rNearZTerrain);
	SetDlgItemFloat(this, IDC_STATIC_FARZ_TERRAIN,    ptsTolerances.rFarZTerrain);
	SetDlgItemInt(IDC_STATIC_MAXNUM, ptsTolerances.iMaxToDepthsort);
	SetDlgItemInt(IDC_STATIC_BINARYAT, ptsTolerances.iBinaryPartitionAt);
	SetDlgItemInt(IDC_STATIC_QUADAT, ptsTolerances.iQuadPartitionAt);
}

//*********************************************************************************************
//
void CDialogDepthSort::OnShowWindow(BOOL bShow, UINT nStatus) 
//
// Initialize all controls.
//
//**************************************
{
	// Call the base class member function.
	CDialog::OnShowWindow(bShow, nStatus);

	//
	// Setup scrollbars.
	//

	// Set scrollbar ranges.
	m_ScrollNearTolerance.SetScrollRange(1, iLogRange, FALSE);
	m_ScrollFarTolerance.SetScrollRange(2, iLogRange, FALSE);
	m_ScrollNearZ.SetScrollRange(1, iLogRange, FALSE);
	m_ScrollFarZ.SetScrollRange(2, iLogRange, FALSE);
	m_ScrollFarZNoDepthSort.SetScrollRange(1, iLogRange, FALSE);
	m_ScrollPixelTolerance.SetScrollRange(0, 100, FALSE);
	m_MaxNum.SetScrollRange(10, 200, FALSE);
	m_BinaryAt.SetScrollRange(100, 2000, FALSE);
	m_QuadAt.SetScrollRange(100, 2000, FALSE);

	// Set scrollbar ranges for terrain.
	m_NearTolTerrain.SetScrollRange(1, iLogRange, FALSE);
	m_FarTolTerrain.SetScrollRange(2, iLogRange, FALSE);
	m_NearZTerrain.SetScrollRange(1, iLogRange, FALSE);
	m_FarZTerrain.SetScrollRange(2, iLogRange, FALSE);

	// Set scrollbar positions.
	m_ScrollNearTolerance.SetScrollPos(iGetPos(ptsTolerances.rNearZScale));
	m_ScrollFarTolerance.SetScrollPos(iGetPos(ptsTolerances.rFarZScale));
	m_ScrollNearZ.SetScrollPos(iGetPos(ptsTolerances.rNearZ ));
	m_ScrollFarZ.SetScrollPos(iGetPos(ptsTolerances.rFarZ ));
	m_ScrollFarZNoDepthSort.SetScrollPos(iGetPos(ptsTolerances.rFarZNoDepthSort));
	m_ScrollPixelTolerance.SetScrollPos(int(ptsTolerances.fPixelBuffer * 50.0f + 0.5f));
	m_MaxNum.SetScrollPos(ptsTolerances.iMaxToDepthsort / 10);
	m_BinaryAt.SetScrollPos(ptsTolerances.iBinaryPartitionAt);
	m_QuadAt.SetScrollPos(ptsTolerances.iQuadPartitionAt);

	// Set scrollbar positions for terrain.
	m_NearTolTerrain.SetScrollPos(iGetPos(ptsTolerances.rNearZScaleTerrain));
	m_FarTolTerrain.SetScrollPos(iGetPos(ptsTolerances.rFarZScaleTerrain));
	m_NearZTerrain.SetScrollPos(iGetPos(ptsTolerances.rNearZTerrain));
	m_FarZTerrain.SetScrollPos(iGetPos(ptsTolerances.rFarZTerrain));

	//
	// Setup static text.
	//
	DisplayText();
	SetButtons();
	
}

//*********************************************************************************************
//
void CDialogDepthSort::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
//
// Move the dialog's horizontal scroll bars.
//
//**************************************
{
	MoveScrollbar(nSBCode, nPos, pScrollBar);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	ptsTolerances.rNearZScale        = fGetVal(m_ScrollNearTolerance.GetScrollPos());
	ptsTolerances.rFarZScale         = fGetVal(m_ScrollFarTolerance.GetScrollPos());
	ptsTolerances.rNearZ             = fGetVal(m_ScrollNearZ.GetScrollPos());
	ptsTolerances.rFarZ              = fGetVal(m_ScrollFarZ.GetScrollPos());
	ptsTolerances.rFarZNoDepthSort   = fGetVal(m_ScrollFarZNoDepthSort.GetScrollPos());
	ptsTolerances.iMaxToDepthsort    = m_MaxNum.GetScrollPos() * 10;
	ptsTolerances.rNearZScaleTerrain = fGetVal(m_NearTolTerrain.GetScrollPos());
	ptsTolerances.rFarZScaleTerrain  = fGetVal(m_FarTolTerrain.GetScrollPos());
	ptsTolerances.rNearZTerrain      = fGetVal(m_NearZTerrain.GetScrollPos());
	ptsTolerances.rFarZTerrain       = fGetVal(m_FarZTerrain.GetScrollPos());

	ptsTolerances.iBinaryPartitionAt = m_BinaryAt.GetScrollPos();
	ptsTolerances.iQuadPartitionAt   = m_QuadAt.GetScrollPos();

	if (ptsTolerances.iBinaryPartitionAt > ptsTolerances.iQuadPartitionAt)
	{
		ptsTolerances.iBinaryPartitionAt = ptsTolerances.iQuadPartitionAt;
		m_BinaryAt.SetScrollPos(ptsTolerances.iBinaryPartitionAt);
	}

	if (ptsTolerances.rNearZScale >= ptsTolerances.rFarZScale)
	{
		m_ScrollNearTolerance.SetScrollPos(m_ScrollFarTolerance.GetScrollPos() - 1);
		ptsTolerances.rNearZScale = fGetVal(m_ScrollNearTolerance.GetScrollPos());
	}

	if (ptsTolerances.rNearZ >= ptsTolerances.rFarZ)
	{
		m_ScrollNearZ.SetScrollPos(m_ScrollFarZ.GetScrollPos() - 1);
		ptsTolerances.rNearZ = fGetVal(m_ScrollNearZ.GetScrollPos());
	}

	ptsTolerances.fPixelBuffer = float(m_ScrollPixelTolerance.GetScrollPos()) / 50.0f;

	switch (ptsTolerances.etsGet())
	{
		case etsLINEAR:
			ptsTolerances.SetLinearScale();
			break;
		case etsINVERSE:
			ptsTolerances.SetInverseScale();
			break;
		default:
			Assert(0);
	}
	
	DisplayText();
	Redraw();
}

//*********************************************************************************************
void CDialogDepthSort::OnRadioLinear() 
{
	ptsTolerances.SetLinearScale();
	Redraw();
}

//*********************************************************************************************
void CDialogDepthSort::OnRadioInverse() 
{
	ptsTolerances.SetInverseScale();
	Redraw();
}

//*********************************************************************************************
//
void CDialogDepthSort::SetButtons
(
)
//
// Sets all the button states in the dialog.
//
//**************************************
{
	// Check box buttons.
	m_CheckTerrain.SetCheck((ptsTolerances.bUseSeparateTolerances) ? (1) : (0));

	// Radio Buttons.
	switch (ptsTolerances.etsGet())
	{
		case etsLINEAR:
			CheckRadioButton(ID_RADIO_LINEAR, ID_RADIO_INVERSE, ID_RADIO_LINEAR);
			break;
		case etsINVERSE:
			CheckRadioButton(ID_RADIO_LINEAR, ID_RADIO_INVERSE, ID_RADIO_INVERSE);
			break;
		default:
			Assert(0);
	}
}

//*********************************************************************************************
void CDialogDepthSort::OnCheckTerrain() 
{
	ptsTolerances.bUseSeparateTolerances = !ptsTolerances.bUseSeparateTolerances;
}
