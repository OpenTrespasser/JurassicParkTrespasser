// InitGUIApp2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "InitGUIApp2.h"
#include "InitGUIApp2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInitGUIApp2App

BEGIN_MESSAGE_MAP(CInitGUIApp2App, CWinApp)
	//{{AFX_MSG_MAP(CInitGUIApp2App)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitGUIApp2App construction

CInitGUIApp2App::CInitGUIApp2App()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CInitGUIApp2App object

CInitGUIApp2App theApp;

/////////////////////////////////////////////////////////////////////////////
// CInitGUIApp2App initialization

BOOL CInitGUIApp2App::InitInstance()
{
	// Standard initialization

	CInitGUIApp2Dlg dlg;
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
