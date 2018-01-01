// TweakNVidia128.h : main header file for the TWEAKNVIDIA128 application
//

#if !defined(AFX_TWEAKNVIDIA128_H__20EAD00A_3303_11D2_A32C_00A0C92A3A07__INCLUDED_)
#define AFX_TWEAKNVIDIA128_H__20EAD00A_3303_11D2_A32C_00A0C92A3A07__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTweakNVidia128App:
// See TweakNVidia128.cpp for the implementation of this class
//

class CTweakNVidia128App : public CWinApp
{
public:
	CTweakNVidia128App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTweakNVidia128App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTweakNVidia128App)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//
// External functions and keys for manipulating the registry.
//
enum ETweakError { eNoDX6, eNoRiva128, eSuccess };
extern HKEY g_hKey;
void CloseKey();
void OpenKey();
ETweakError eDetectNVidiaRiva128();


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TWEAKNVIDIA128_H__20EAD00A_3303_11D2_A32C_00A0C92A3A07__INCLUDED_)
