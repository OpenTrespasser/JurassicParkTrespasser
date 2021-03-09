/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Dialog for editing information about physics.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogPhysics.hpp                                             $
 * 
 * 7     1/22/98 2:47p Pkeet
 * Provided a forward declaration of 'CInstance' to accomodate the reduction in the number of
 * includes in 'StdAfx.h.'
 * 
 * 6     12/05/97 3:56p Pkeet
 * Moved the floating scrollbar definition to a separate file.
 * 
 * 5     97/10/07 18:22 Speter
 * Set fValuePerStep based on correct range.  Fixed bug in SetPos; incorrectly used
 * fStartValue.  Removed Initialize().
 * 
 * 4     12/18/96 3:59p Pkeet
 * Added a reset button.
 * 
 * 3     12/18/96 3:15p Pkeet
 * Added functions to place the CInstance name string in the titlebar. Fixed bugs.
 * 
 * 2     12/18/96 1:54p Pkeet
 * Added standard headers and a member function to get a CInstance object pointer and set a
 * local member variable.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGPHYSICS_HPP
#define HEADER_GUIAPP_DIALOGPHYSICS_HPP


//
// Required includes.
//
#include "DialogScrollbars.hpp"


//
// Forward declarations.
//
class CInstance;


//
// Macros.
//

// Number of scrollbars used by the physics dialog.
#define iNUM_SCROLLS (20)


//
// Class definitions.
//

//*********************************************************************************************
//
class CDialogPhysics : public CDialog
//
// Physics editing dialog box.
//
//**************************************
{
protected:

	CInstance* pinsObject;

public:

	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	// Standard constructor.
	CDialogPhysics(CWnd* pParent = NULL);

	//{{AFX_DATA(CDialogPhysics)
	enum { IDD = IDD_DIALOG_PHYSICS };
	CScrollFloat m_Scroll[iNUM_SCROLLS];
	//}}AFX_DATA


	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void SetInstance
	(
		CInstance* pins	// Pointer to the instance of the object to look at.
	);
	//
	// Sets the pointer to an instance of an object for display purposes.
	//
	//**********************************

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPhysics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	//*****************************************************************************************
	void GetValuesFromScrollbars();

	//*****************************************************************************************
	void SetupScrollbars();

	//*****************************************************************************************
	void Update();

	//*****************************************************************************************
	void DisplayText();

	//*****************************************************************************************
	void DisplayValues();

	// Generated message map functions
	//{{AFX_MSG(CDialogPhysics)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK() override;
	afx_msg void OnReset();
	virtual void OnCancel() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
