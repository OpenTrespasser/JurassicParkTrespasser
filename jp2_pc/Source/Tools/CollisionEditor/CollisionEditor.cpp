// CollisionEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CollisionEditor.h"
#include "CollisionEditorDlg.h"
#include "Lib/Sys/MemoryLog.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCollisionEditorApp

BEGIN_MESSAGE_MAP(CCollisionEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CCollisionEditorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Globals that hold the state

/////////////////////////////////////////////////////////////////////////////
// CCollisionEditorApp construction

CCollisionEditorApp::CCollisionEditorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

bool bIsTrespasser = false;

/////////////////////////////////////////////////////////////////////////////
// The one and only CCollisionEditorApp object

CCollisionEditorApp theApp;


void __cdecl dprintf(char* str,...)
{
	char	buf[256];

	va_list argptr;
	va_start (argptr,str);
	vsprintf (buf,str,argptr);
	va_end (argptr);

	OutputDebugString(buf);
}


/////////////////////////////////////////////////////////////////////////////
// CCollisionEditorApp initialization

BOOL CCollisionEditorApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	
	CCollisionEditorDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CCollisionEditorApp::ExitInstance() 
{
	return CWinApp::ExitInstance();
}


void  AddToCounter(EMemoryLog eml,long l)
{
}
