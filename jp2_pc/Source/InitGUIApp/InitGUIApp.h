// InitGUIApp.h : main header file for the INITGUIAPP application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CInitGUIAppApp:
// See InitGUIApp.cpp for the implementation of this class
//

class CInitGUIAppApp : public CWinApp
{
public:
	CInitGUIAppApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInitGUIAppApp)
	public:
	virtual BOOL InitInstance() override;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CInitGUIAppApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
