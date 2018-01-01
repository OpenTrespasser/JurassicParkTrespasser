/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Implementation of 'PerspectiveSubdivideDialog.hpp.'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/PerspectiveSubdivideDialog.cpp                                $
 * 
 * 15    98.07.21 8:20p Mmouni
 * Changed range of adaptive settings scroll bars.
 * 
 * 14    98.07.17 6:29p Mmouni
 * Added new adaptive perspective settings.
 * 
 * 13    98.03.24 8:16p Mmouni
 * Made changes to support alternate perspective correction settings.
 * 
 * 12    1/25/98 4:16p Pkeet
 * Added externally defined global variables.
 * 
 * 11    1/12/98 11:45a Pkeet
 * Added in the software lock request for the auxilary renderer.
 * 
 * 10    97/11/14 11:46p Pkeet
 * Changed to a more suitable range.
 * 
 * 9     97/11/07 5:34p Pkeet
 * Changed slider to use screen area.
 * 
 * 8     97/07/07 14:24 Speter
 * Re-ordered and fixed includes.
 * 
 * 7     97/06/27 2:04p Pkeet
 * Adjusted slider ranges.
 * 
 * 6     97/06/24 1:59p Pkeet
 * Added adaptive subdivision.
 * 
 * 5     97/06/16 10:55p Pkeet
 * Upped the upper limit.
 * 
 * 4     97/06/15 17:35 Speter
 * Updated perspective dialog for new fPerspectivePixelError variable.
 * 
 * 3     1/03/97 5:51p Pkeet
 * Added support for a dynamically changeable inverse z error ratio.
 * 
 * 2     1/03/97 4:41p Pkeet
 * Added support for changing the subdivision length.
 * 
 * 1     1/03/97 4:04p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#include "StdAfx.h"
#include "PerspectiveSubdivideDialog.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "Lib/Renderer/PipeLine.hpp"

extern float fPerspectivePixelError;
extern float fMinZPerspective;
extern float fAltPerspectivePixelError;


//*********************************************************************************************
//
template<class T> inline void SetMinAbs
(
	T& t_a,								// Variable to set.
	T t_b								// Value to compare with.
)
//
// Set t_a to t_b if the absolute value of t_b is less than t_a.
//
//**************************************
{
//	if (Abs(t_b) < Abs(t_a))
//		t_a = t_b;
	if (t_a >= 0)
	{
		if (t_b < 0)
			t_a = 0;
		else
			SetMin(t_a, t_b);
	}
	else
	{
		if (t_b > 0)
			t_a = 0;
		else
			SetMax(t_a, t_b);
	}
}

#include "Lib/Types/FixedP.hpp"
#include "Lib/Renderer/Primitives/Walk.hpp"
#include "Lib/Renderer/Primitives/IndexT.hpp"
#include "Lib/Renderer/Primitives/IndexPerspectiveT.hpp"

extern CPerspectiveSettings persetSettings;

//
// Constants.
//


CPerspectiveSubdivideDialog::CPerspectiveSubdivideDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPerspectiveSubdivideDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPerspectiveSubdivideDialog)
	//}}AFX_DATA_INIT
}


void CPerspectiveSubdivideDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPerspectiveSubdivideDialog)
	DDX_Control(pDX, IDC_SCROLL_INVZDX, m_ScrollMinInvZDX);
	DDX_Control(pDX, IDC_CHECK_ADAPTIVE, m_CheckAdaptive);
	DDX_Control(pDX, IDC_SCROLL_PERSPECTIVE_SUBDIVIDE, m_SubdivideLen);
	DDX_Control(pDX, IDC_SCROLL_PERSPECTIVE_ERROR, m_Error);
	DDX_Control(pDX, IDC_SCROLL_PERSPECTIVE_SUBDIVIDE2, m_SubdivideLen2);
	DDX_Control(pDX, IDC_SCROLL_PERSPECTIVE_ERROR2, m_Error2);
	DDX_Control(pDX, IDC_SCROLL_PERSPECTIVE_MINSUBDIVIDE, m_AdaptiveMinSubdivision);
	DDX_Control(pDX, IDC_SCROLL_PERSPECTIVE_MAXSUBDIVIDE, m_AdaptiveMaxSubdivision);
	DDX_Control(pDX, IDC_SCROLL_PERSPECTIVE_INVZFORMAX, m_InvZForMaxSubdivision);
	DDX_Control(pDX, IDC_SCROLL_PERSPECTIVE_INVZSCALE, m_fInvZScale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPerspectiveSubdivideDialog, CDialog)
	//{{AFX_MSG_MAP(CPerspectiveSubdivideDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_ADAPTIVE, OnCheckAdaptive)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*********************************************************************************************
void CPerspectiveSubdivideDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	// Set up the error ratio scrollbar.
	m_Error.SetScrollRange(0, 200, FALSE);
	m_Error.SetScrollPos(fPerspectivePixelError * 10.0);

	// Set up the subdivision length scrollbar.
	m_SubdivideLen.SetScrollRange(1, 200, FALSE);
	m_SubdivideLen.SetScrollPos(persetSettings.iMinSubdivision);

	// Set up the alternate error ratio scrollbar.
	m_Error2.SetScrollRange(0, 200, FALSE);
	m_Error2.SetScrollPos(fAltPerspectivePixelError * 10.0);

	// Set up the alternate subdivision length scrollbar.
	m_SubdivideLen2.SetScrollRange(1, 200, FALSE);
	m_SubdivideLen2.SetScrollPos(persetSettings.iAltMinSubdivision);

	// Set up the inverse z division scrollbar.
	m_ScrollMinInvZDX.SetScrollRange(1, 100, FALSE);
	m_ScrollMinInvZDX.SetScrollPos(int(fMinZPerspective * 100.0f));

	// Set the adaptive subdivision check box.
	m_CheckAdaptive.SetCheck(persetSettings.bAdaptive);
	
	m_AdaptiveMinSubdivision.SetScrollRange(1, 63, FALSE);
	m_AdaptiveMinSubdivision.SetScrollPos(persetSettings.iAdaptiveMinSubdivision/2);

	m_AdaptiveMaxSubdivision.SetScrollRange(1, 63, FALSE);
	m_AdaptiveMaxSubdivision.SetScrollPos(persetSettings.iAdaptiveMaxSubdivision/2);

	m_InvZForMaxSubdivision.SetScrollRange(0, 100, FALSE);
	m_InvZForMaxSubdivision.SetScrollPos(int((persetSettings.fInvZForMaxSubdivision - 1e-7) / 1e-7));

	m_fInvZScale.SetScrollRange(0, 100, FALSE);
	m_fInvZScale.SetScrollPos(int((persetSettings.fInvZScale-524288.0)/((16777216.0-524288.0)/100.0)));
	
	// Show text associated with controls.
	DisplayText();
}

//*********************************************************************************************
void CPerspectiveSubdivideDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Call base class member function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// Default scrollbar movement.
	MoveScrollbar(nSBCode, nPos, pScrollBar);

	// Change the error ratio value.
	fPerspectivePixelError = float(m_Error.GetScrollPos()) * 0.1;

	// Change the value for subdivision length.
	persetSettings.iMinSubdivision = m_SubdivideLen.GetScrollPos();

	// Change the error ratio value.
	fAltPerspectivePixelError = float(m_Error2.GetScrollPos()) * 0.1;

	// Change the value for subdivision length.
	persetSettings.iAltMinSubdivision = m_SubdivideLen2.GetScrollPos();

	fMinZPerspective = float(m_ScrollMinInvZDX.GetScrollPos()) / 100.0f;

	// Adptive settings.
	persetSettings.iAdaptiveMinSubdivision = m_AdaptiveMinSubdivision.GetScrollPos()*2;
	persetSettings.iAdaptiveMaxSubdivision = m_AdaptiveMaxSubdivision.GetScrollPos()*2;
	persetSettings.fInvZForMaxSubdivision = 1e-7 + m_InvZForMaxSubdivision.GetScrollPos()*1e-7;
	persetSettings.fInvZScale = 524288.0 + m_fInvZScale.GetScrollPos()*((16777216.0-524288.0)/100.0);
	
	// Show text associated with controls.
	DisplayText();

	//
	// Update main window display.
	//
	GetParent()->Invalidate();
}

//*********************************************************************************************
//
void CPerspectiveSubdivideDialog::DisplayText
(
)
//
// Sets the text in static controls to reflect camera parameters.
//
//**************************************
{
	SetDlgItemFloat(this, IDC_STATIC_PERSPECTIVE_ERROR, fPerspectivePixelError);
	SetDlgItemInt(IDC_STATIC_PERSPECTIVE_SUBDIVIDE, persetSettings.iMinSubdivision);

	SetDlgItemFloat(this, IDC_STATIC_PERSPECTIVE_ERROR2, fAltPerspectivePixelError);
	SetDlgItemInt(IDC_STATIC_PERSPECTIVE_SUBDIVIDE2, persetSettings.iAltMinSubdivision);

	SetDlgItemFloat(this, IDC_STATIC_INVZDX, fMinZPerspective);

	SetDlgItemInt(IDC_STATIC_PERSPECTIVE_MINSUBDIVIDE, persetSettings.iAdaptiveMinSubdivision);
	SetDlgItemInt(IDC_STATIC_PERSPECTIVE_MAXSUBDIVIDE, persetSettings.iAdaptiveMaxSubdivision);
	SetDlgItemFloat(this, IDC_STATIC_PERSPECTIVE_INVZFORMAX, persetSettings.fInvZForMaxSubdivision);
	SetDlgItemFloat(this, IDC_STATIC_PERSPECTIVE_INVZSCALE, persetSettings.fInvZScale);
}

//*********************************************************************************************
//
void CPerspectiveSubdivideDialog::OnCheckAdaptive() 
//
// Toggles the adaptive subdivision feature.
//
//**************************************
{
	// Set the adaptive switch.
	persetSettings.bAdaptive = !persetSettings.bAdaptive;

	// Set the adaptive subdivision check box.
	m_CheckAdaptive.SetCheck(persetSettings.bAdaptive);

	// Update main window display.
	GetParent()->Invalidate();
}
