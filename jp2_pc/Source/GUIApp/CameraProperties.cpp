/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents: Implementation of 'CameraProperties.hpp'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/CameraProperties.cpp                                          $
 * 
 * 20    98.06.26 11:36a Mmouni
 * Now adjusts desired far-clip distance.
 * 
 * 19    3/18/98 4:07p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 * 18    97/08/14 12:11 Speter
 * Changed near/far ranges to reasonable values.
 * 
 * 17    97/07/17 17:24 Speter
 * Upped range of far clipping plane to 10K.
 * 
 * 16    97/03/24 15:20 Speter
 * Changed camera CInstance to CCamera.
 * 
 * 15    2/03/97 10:34p Agrant
 * Query.hpp and MessageTypes.hpp have been split into
 * myriad parts so that they may have friends.
 * Or rather, so compile times go down.
 * Look for your favorite query in Lib/EntityDBase/Query/
 * Look for messages in                Lib/EntityDBase/MessageTypes/
 * 
 * 14    1/27/97 4:55p Pkeet
 * Fixed bug with scrollbars.
 * 
 * 13    97/01/16 18:27 Speter
 * Updated for change in CCamera::SProperties.
 * 
 * 12    97/01/07 12:10 Speter
 * Made all CRenderTypes use rptr<>.
 * 
 * 11    12/09/96 1:42p Mlange
 * Updated for changes to the CCamera interface.
 * 
 * 10    96/12/04 20:34 Speter
 * A big change: Updated for new CPresence3<>, and CInstance functions.
 * 
 * 9     11/23/96 5:51p Mlange
 * Updated for the changes to the camera accessor functions.
 * 
 * 8     11/20/96 1:12p Gstull
 * Integration of multiple objects into the GUIApp.
 * 
 * 7     10/28/96 7:43p Pkeet
 * Removed references to the global camera variable and replaced them with a call to the world
 * object database to get the primary camera.
 * 
 * 6     96/10/14 15:42 Speter
 * Fixed bug (probably?) in Asserts.
 * 
 * 5     96/09/25 19:49 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 4     9/13/96 2:03p Pkeet
 * Added includes taken from 'StdAfx.hpp.'
 * 
 * 3     8/12/96 5:35p Pkeet
 * Added comments.
 * 
 * 2     8/09/96 2:45p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/


//
// Includes.
//
#include "stdafx.h"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "GUIPipeLine.hpp"
#include "GUITools.hpp"
#include "CameraProperties.hpp"


//
// Constants.
//

const int iSCROLL_RANGE	= 1000;
const float fNEAR_MAX	= 5;
const float fFAR_MAX	= 5000;

//
// Class implementation.
//


//*********************************************************************************************
//
// CCameraProperties implementation.
//

//*********************************************************************************************
//
// CCameraProperties Constructor.
//

//*********************************************************************************************
CCameraProperties::CCameraProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CCameraProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCameraProperties)
	//}}AFX_DATA_INIT
}

//*********************************************************************************************
//
// Message map for dialog.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CCameraProperties, CDialog)
	//{{AFX_MSG_MAP(CCameraProperties)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*********************************************************************************************
//
// Member functions..
//

//*********************************************************************************************
void CCameraProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCameraProperties)
	DDX_Control(pDX, IDC_SCROLL_CAM_ZOOM, m_ScrollZoom);
	DDX_Control(pDX, IDC_SCROLL_CAM_NEAR, m_ScrollNear);
	DDX_Control(pDX, IDC_SCROLL_CAM_FAR, m_ScrollFar);
	DDX_Control(pDX, IDC_SCROLL_CAM_ANGLE, m_ScrollAngle);
	//}}AFX_DATA_MAP
}

//*********************************************************************************************
//
void CCameraProperties::OnHScroll
(
	UINT        nSBCode,
	UINT        nPos,
	CScrollBar* pScrollBar
) 
//
// Responds to a WM_HSCROLL message by moving the appropriate scrollbar.
//
//**************************************
{	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	MoveScrollbar(nSBCode, nPos, pScrollBar);

	//
	// If the user attempts to set the far clipping plane in front of the near clipping plane,
	// block the action.
	//
	if (pScrollBar == &m_ScrollFar)
	{
		if (m_ScrollFar.GetScrollPos() <= m_ScrollNear.GetScrollPos() * fNEAR_MAX / fFAR_MAX)
		{
			m_ScrollFar.SetScrollPos(m_ScrollNear.GetScrollPos() * fNEAR_MAX / fFAR_MAX + 1);
		}
	}

	//
	// If the user attempts to set the near clipping plane behind the far clipping plane,
	// block the action.
	//
	if (pScrollBar == &m_ScrollNear)
	{
		if (m_ScrollNear.GetScrollPos() >= m_ScrollFar.GetScrollPos() * fFAR_MAX / fNEAR_MAX)
		{
			m_ScrollNear.SetScrollPos(m_ScrollFar.GetScrollPos() * fFAR_MAX / fNEAR_MAX - 1);
		}
	}

	ActOnChange();
}

//*********************************************************************************************
//
void CCameraProperties::OnShowWindow(BOOL bShow, UINT nStatus)
//
// Responds to a WM_SHOWWINDOW message. Initializes scrollbars and text for static control.
//
//**************************************
{
	CDialog::OnShowWindow(bShow, nStatus);

	ptr<CCamera> pcam = CWDbQueryActiveCamera(wWorld).tGet();

	//
	// Setup scrollbars.
	//

	// Setup the zoom factor.
	m_ScrollZoom.SetScrollRange(1, 100, FALSE);
	m_ScrollZoom.SetScrollPos((int)(pcam->campropGetProperties().fZoomFactor * 10.0f + 0.5f));

	// Setup the camera view angle scrollbar.
	m_ScrollAngle.SetScrollRange(10, 170, FALSE);
	m_ScrollAngle.SetScrollPos((int)(dRadiansToDegrees(pcam->campropGetProperties().angGetAngleOfView()) + 0.5f));

	// Setup the near and far clipping plane scrollbars.
	m_ScrollNear.SetScrollRange(1, iSCROLL_RANGE, FALSE);
	m_ScrollFar.SetScrollRange(1, iSCROLL_RANGE, FALSE);

	m_ScrollNear.SetScrollPos((int)(pcam->campropGetProperties().rNearClipPlaneDist * iSCROLL_RANGE / fNEAR_MAX + 0.5f));
	m_ScrollFar.SetScrollPos((int)(pcam->campropGetProperties().rDesiredFarClipPlaneDist * iSCROLL_RANGE / fFAR_MAX + 0.5f));

	//
	// Setup static text controls.
	//
	DisplayText();
}

//*********************************************************************************************
//
void CCameraProperties::ActOnChange
(
)
//
// Responds to scrollbar changes by adjusting camera parameters.
//
//**************************************
{
	Assert(GetParent());

	//
	// Set camera properties.
	//
	ptr<CCamera> pcam = CWDbQueryActiveCamera(wWorld).tGet();

	CCamera::SProperties camprop = pcam->campropGetProperties();

	// Set camera zoom.
	camprop.fZoomFactor = (float)m_ScrollZoom.GetScrollPos() / 10.0f;

	// Set camera angle.
	camprop.SetAngleOfView(dDegreesToRadians(m_ScrollAngle.GetScrollPos()));

	// Set camera near and far clipping planes.
	camprop.rNearClipPlaneDist = (float)m_ScrollNear.GetScrollPos() * fNEAR_MAX / iSCROLL_RANGE;
	camprop.rDesiredFarClipPlaneDist  = (float)m_ScrollFar.GetScrollPos() * fFAR_MAX / iSCROLL_RANGE;
	camprop.SetFarClipFromDesired();

	pcam->SetProperties(camprop);

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
void CCameraProperties::DisplayText
(
)
//
// Sets the text in static controls to reflect camera parameters.
//
//**************************************
{
	ptr<CCamera> pcam = CWDbQueryActiveCamera(wWorld).tGet();

	SetDlgItemInt(IDC_STATIC_CAM_ANGLE, (int)(dRadiansToDegrees(pcam->campropGetProperties().angGetAngleOfView()) + 0.5));
	SetDlgItemFloat(this, IDC_STATIC_CAM_ZOOM, pcam->campropGetProperties().fZoomFactor);
	SetDlgItemFloat(this, IDC_STATIC_CAM_NEAR, pcam->campropGetProperties().rNearClipPlaneDist);
	SetDlgItemFloat(this, IDC_STATIC_CAM_FAR,  pcam->campropGetProperties().rDesiredFarClipPlaneDist);
}
