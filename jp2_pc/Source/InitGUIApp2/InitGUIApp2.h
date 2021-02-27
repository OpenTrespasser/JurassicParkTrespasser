#pragma once

// InitGUIApp2.h : main header file for the INITGUIAPP2 application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CInitGUIApp2App:
// See InitGUIApp2.cpp for the implementation of this class
//

class CInitGUIApp2App : public CWinApp
{
public:
	CInitGUIApp2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInitGUIApp2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CInitGUIApp2App)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
