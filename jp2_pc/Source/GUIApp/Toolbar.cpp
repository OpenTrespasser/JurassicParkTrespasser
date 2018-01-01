/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of Toolbar.hpp.
 *
 * To Do:
 *		Add capabilities to edit the properties of an object.
 *		Add a binary object loader.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/Toolbar.cpp                                                    $
 * 
 * 13    97/09/08 17:35 Speter
 * OpenLightProperties takes CLight*, not CLightAmbient*.
 * 
 * 12    97/09/03 18:14 Speter
 * Now allows multiple selected objects (via Ctrl key).  Delete, Move, and other functions now
 * work on all selected objects.
 * 
 * 11    97/05/27 12:42p Pkeet
 * Changed the default window size to 512 by 384 and added menu items to select other sizes.
 * 
 * 10    97/05/27 10:51a Pkeet
 * Changed from topmost to top window.
 * 
 * 9     97/05/26 6:36p Pkeet
 * Set new default window size and position.
 * 
 * 8     97-05-12 11:38 Speter
 * Removed egiuMode, and "Camera" and "Object" commands.
 * 
 * 7     97/01/07 12:10 Speter
 * Made all CRenderTypes use rptr<>.
 * 
 * 6     9/13/96 2:15p Pkeet
 * Added includes take from "stdafx.h."
 * 
 * 5     8/15/96 1:30p Pkeet
 * Replaced 'ROTATION' and 'TRANSLATION' movement types with the 'shift' key.
 * 
 * 4     8/13/96 3:04p Pkeet
 * Replicated toolbar functions in the menu.
 * 
 * 3     8/12/96 6:13p Pkeet
 * Added comments.
 * 
 * 2     8/09/96 4:17p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "stdafx.h"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Light.hpp"
#include "GUIPipeLine.hpp"
#include "Toolbar.hpp"
#include "GUIAppDlg.h"

//
// Class implementation.
//


//*********************************************************************************************
//
// CGUIAppDlg implementation.
//

//*********************************************************************************************
//
// CGUIAppDlg message map.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CTool, CDialog)
	//{{AFX_MSG_MAP(CTool)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_RADIO_MOVE, OnRadioMove)
	ON_BN_CLICKED(IDC_RADIO_OBJECT, OnRadioObject)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_ADDOBJECT, OnButtonAddobject)
	ON_BN_CLICKED(IDC_BUTTON_ADDDIRLIGHT, OnButtonAdddirlight)
	ON_BN_CLICKED(IDC_BUTTON_ADDPTDIRLIGHT, OnButtonAddptdirlight)
	ON_BN_CLICKED(IDC_BUTTON_ADDPTLIGHT, OnButtonAddptlight)
	ON_BN_CLICKED(IDC_BUTTON_EDITAMBIENT, OnButtonEditambient)
	ON_BN_CLICKED(IDC_BUTTON_EDITCAMERA, OnButtonEditcamera)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CGUIAppDlg constructor.
//

//*********************************************************************************************
CTool::CTool(CWnd* pParent /*=NULL*/)
	: CDialog(CTool::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTool)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


//*********************************************************************************************
//
// CGUIAppDlg member functions.
//

//*********************************************************************************************
void CTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTool)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

//*********************************************************************************************
//
void CTool::SetButtonStates
(
)
//
// Sets the states of all the radio buttons in the dialog. Also set menu states for the
// application.
//
//**************************************
{
	CGUIAppDlg* pguidlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pguidlg);

	pguidlg->SetMenuState();
}

//*********************************************************************************************
//
void CTool::OnShowWindow
(
	BOOL bShow,
	UINT nStatus
)
//
// Responds to a WM_SHOWWINDOW message. Sets the initial states for all the radio buttons.
//
//**************************************
{
	CDialog::OnShowWindow(bShow, nStatus);

	// Set to default position.
	int i_x  = 512 + 2 * GetSystemMetrics(SM_CXFRAME) + iGUIAPP_DEFAULT_OFFSET;
	SetWindowPos(&wndTop, i_x, iGUIAPP_DEFAULT_OFFSET, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
}

//*********************************************************************************************
//
void CTool::OnRadioMove
(
) 
//
// Responds to the 'Camera' move mode radio button.
//
//**************************************
{
	SetButtonStates();
}

//*********************************************************************************************
//
void CTool::OnRadioObject
(
) 
//
// Responds to the 'Object' move mode radio button.
//
//**************************************
{
	SetButtonStates();
}

//*********************************************************************************************
//
void CTool::OnButtonDelete
(
)
//
// Responds to the 'Delete' button.
//
//**************************************
{
	if (pipeMain.iDeleteSelected())
	{
		GetParent()->Invalidate();
	}
	else
	{
		GetParent()->MessageBox("No object selected.", "GUIApp Error",
				                MB_OK | MB_ICONHAND);
	}
}

//*********************************************************************************************
//
void CTool::OnButtonEdit
(
)
//
// Responds to the 'Edit' button. Edits an object, a light or the camera.
//
//**************************************
{
	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pappdlg);

	pappdlg->EditObject();
}

//*********************************************************************************************
//
void CTool::OnButtonAddobject()
//
// Adds a default object into the scene.
//
//**************************************
{
	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pappdlg);

	pappdlg->AddObject();
}

//*********************************************************************************************
//
void CTool::OnButtonAdddirlight()
//
// Adds a directional light into the scene.
//
//**************************************
{
	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pappdlg);

	pappdlg->AddDirectionalLight();
}

//*********************************************************************************************
//
void CTool::OnButtonAddptdirlight()
//
// Adds a point directional light into the scene.
//
//**************************************
{
	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pappdlg);

	pappdlg->AddPointDirectionalLight();
}

//*********************************************************************************************
//
void CTool::OnButtonAddptlight()
//
// Adds a point light into the scene.
//
//**************************************
{
	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pappdlg);

	pappdlg->AddPointLight();
}

//*********************************************************************************************
//
void CTool::OnButtonEditambient()
//
// Edits the ambient light's properties.
//
//**************************************
{
	OpenLightProperties(rptr_cast(CLight, pipeMain.pltaGetAmbientLight()));
}

//*********************************************************************************************
//
void CTool::OpenLightProperties(rptr<CLight> plt)
//
// Edits the properties of a light.
//
//**************************************
{
	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pappdlg);

	pappdlg->OpenLightProperties(plt);
}

//*********************************************************************************************
//
void CTool::OnButtonEditcamera
(
) 
//
// Edits the properties of the camera.
//
//**************************************
{
	CGUIAppDlg* pappdlg = dynamic_cast<CGUIAppDlg*>(GetParent());

	Assert(pappdlg);

	pappdlg->OpenCameraProperties();
}

