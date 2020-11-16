// InitGUIApp.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include "InitGUIApp.h"
#include "InitGUIAppDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInitGUIAppApp

BEGIN_MESSAGE_MAP(CInitGUIAppApp, CWinApp)
	//{{AFX_MSG_MAP(CInitGUIAppApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitGUIAppApp construction

CInitGUIAppApp::CInitGUIAppApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CInitGUIAppApp object

CInitGUIAppApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CInitGUIAppApp initialization

BOOL CInitGUIAppApp::InitInstance()
{
	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CInitGUIAppDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
