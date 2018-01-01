/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Definition of CDialogDummy.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogDummy.hpp                                               $
 * 
 * 1     97/03/27 7:14p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUI_DIALOGDUMMY_HPP
#define HEADER_GUI_DIALOGDUMMY_HPP


//
// Forward class declarations.
//
class CConsoleEx;
class CGUIAppDlg;


//
// Class definitions.
//

//*********************************************************************************************
class CDialogDummy : public CDialog
{
private:

	CGUIAppDlg* pdlgGuiApp;
	CConsoleEx* pconConsole;

// Construction
public:

	// Constructor.
	CDialogDummy(CGUIAppDlg* pdlg, CConsoleEx* pcon);

	//*****************************************************************************************
	//
	bool bIsVisible
	(
	) const;
	//
	// Returns 'true' if the dialog is visible.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual BOOL ShowWindow
	(
		bool b_show
	);
	//
	// Calls 'ShowWindow.'
	//
	//**************************************

// Dialog Data
	//{{AFX_DATA(CDialogDummy)
	enum { IDD = IDD_DUMMY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDummy)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDummy)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // HEADER_GUI_DIALOGDUMMY_HPP
