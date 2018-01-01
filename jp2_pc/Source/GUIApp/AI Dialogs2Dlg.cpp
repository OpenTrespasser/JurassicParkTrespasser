// AI Dialogs2Dlg.cpp : implementation file
//

//
// Warning::: Complete fucking bodge starts here...
//

//**********************************************************************************************
// Look at all the shit I had to go through to be able to include WinInclude after stdafx.h
// Why does stdafx.h include GUIAPP.h and GUIAPPDLG.h and GUITOOLS.h ???
// This section should be put into its own header file called stdafxwin.h or something like that.
//

#if (_MSC_VER >= 1100)
// MFC defines some comparison operators that differ from STL templated operators only
// by calling convertion resulting in this warning.
#pragma warning(disable: 4666)
#endif

// Make sure this constant hasn't already been defined.
#ifndef VC_EXTRALEAN
// Exclude rarely-used stuff from Windows headers.
#define VC_EXTRALEAN		
#endif
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#undef min
#undef max

#undef STRICT

// Project includes.
#include "resource.h"

#include "common.hpp"
//#include "AI Dialogs2.h"
#include "AI Dialogs2Dlg.h"
#include "ParameterDlg.h"

#include "Lib/EntityDBase/Animal.hpp"
#include "Game/AI/AIMain.hpp"
#include "Game/AI/Brain.hpp"
#include "Game/AI/Activity.hpp"
#include "Game/AI/ActivityEnum.hpp"

#include "GUIApp\GUIPipeLine.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAIDialogs2Dlg dialog

CAIDialogs2Dlg::CAIDialogs2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAIDialogs2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAIDialogs2Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	pbrBrain = 0;

}

void CAIDialogs2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAIDialogs2Dlg)
	DDX_Control(pDX, ID_AI_TEXT, m_AIText);
	DDX_Control(pDX, IDC_ACTIVITIES, m_ActivityList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAIDialogs2Dlg, CDialog)
	//{{AFX_MSG_MAP(CAIDialogs2Dlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_ALL, OnAll)
	ON_BN_CLICKED(ID_NONE, OnNone)
	ON_LBN_DBLCLK(IDC_ACTIVITIES, OnDblclkActivities)
	ON_LBN_SELCHANGE(IDC_ACTIVITIES, OnSelchangeActivities)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAIDialogs2Dlg message handlers

BOOL CAIDialogs2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAIDialogs2Dlg::OnPaint() 
{

	// Do we have a brain?
	if (pbrBrain == 0)
	{
		// No!  Set up the dialog!
		CAnimal* pani = ptCast<CAnimal>(gaiSystem.pinsSelected);

		if (pani)
			pbrBrain = pani->pbrBrain;

		// Add the activity names here!
		if (pbrBrain)
		{
			SetWindowText(pani->strGetInstanceName());

			// Loop through all activities and add them.
			for (int i = 0; i < eatEND; ++i)
			{
				m_ActivityList.InsertString(i, pbrBrain->sapactActivities[i]->acName);
				if (pbrBrain->sapactActivities[i]->bIsActive())
					m_ActivityList.SetSel(i, true);
			}
		}
		else
		{
			return;
		}
	}

	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		
		// Now actually paint it.
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAIDialogs2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAIDialogs2Dlg::OnOK()  
{
	// TODO: Add extra validation here
	
	// Snag all activity flag values.
	int i;
	for (i = 0; i < eatEND; ++i)
	{
		pbrBrain->sapactActivities[i]->Activate(m_ActivityList.GetSel(i));
	}

	// Find all parameter dialogs and delete them.
	for (i = eatEND - 1; i >= 0; --i)
	{
		CParameterDlg* ppdlg = (CParameterDlg*) m_ActivityList.GetItemData(i);
		if (ppdlg)
		{
			ppdlg->SendMessage(WM_COMMAND,IDOK, 0);
			delete ppdlg;
		}
	}


	pbrBrain = 0;
	m_ActivityList.ResetContent();
	CDialog::OnOK();
}

void CAIDialogs2Dlg::OnCancel() 
{
	int i = 0;

	// Find all parameter dialogs and delete them.
	for (i = eatEND - 1; i >= 0; --i)
	{
		CParameterDlg* ppdlg = (CParameterDlg*) m_ActivityList.GetItemData(i);
		if (ppdlg)
		{
			ppdlg->SendMessage(WM_COMMAND, IDCANCEL, 0);
			delete ppdlg;
		}
	}

	pbrBrain = 0;
	m_ActivityList.ResetContent();

	CDialog::OnCancel();
}

void CAIDialogs2Dlg::OnAll() 
{
	// ALL button pressed.
	m_ActivityList.SelItemRange(true, 0, eatEND - 1);	
}

void CAIDialogs2Dlg::OnNone() 
{
	// NONE button pressed.
	m_ActivityList.SelItemRange(false, 0, eatEND - 1);	
}

void CAIDialogs2Dlg::OnDblclkActivities() 
{
	if (!pbrBrain)
		return;

	// Bring up a parameters dialog for the double clicked activity.
	int i_index = 0;
	i_index = m_ActivityList.GetCaretIndex();
	CParameterDlg* ppdlg = (CParameterDlg*) m_ActivityList.GetItemData(i_index);


	// Do we already have such a dialog?
	if (!ppdlg)
	{
		// No!  Make one.
		ppdlg = new CParameterDlg();
		ppdlg->Create(IDD_PARAMETER_DIALOG, this);

		// Save the pointer.
		m_ActivityList.SetItemData(i_index, (DWORD)ppdlg);

	}

	// Is it visible?
	if (ppdlg->ShowWindow(SW_SHOWNORMAL))
	{
		// Yes!  Do nothing.
	}
	else
	{
		// No!  Update the data.

		// Want some descriptive text for the dialog.
		ppdlg->SetWindowText(pbrBrain->sapactActivities[i_index]->acName);

		// Setup the feeling for the parameter dlg.
		ppdlg->SetFeeling(&pbrBrain->sapactActivities[i_index]->feelRatingFeeling, -8.0f, 8.0f);
	}
}

void CAIDialogs2Dlg::OnSelchangeActivities() 
{
	// TODO: Add your control notification handler code here
	// Bring up a parameters dialog for the double clicked activity.
	static int i_last_caret_index = -1;

	// Do we need to update the text?
	if (i_last_caret_index != m_ActivityList.GetCaretIndex())
	{
		i_last_caret_index = m_ActivityList.GetCaretIndex();

#if VER_TEST
		char buffer[1024];
		// Write up some juicy text.
		pbrBrain->sapactActivities[i_last_caret_index]->iGetDescription(buffer, 1024);
		// Now set some text.
		m_AIText.SetWindowText(buffer);		
#else
		m_AIText.SetWindowText("No information");
#endif
	}
}
