/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Dialog for setting gun parameters.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogGun.hpp                                                 $
 * 
 * 4     97/10/17 5:32p Pkeet
 * Implemented pull parameters.
 * 
 * 3     97/10/15 4:52p Pkeet
 * Changed 'm_GunMoment' to 'm_GunImpulse.'
 * 
 * 2     97/10/15 3:04p Pkeet
 * Hooked up the scrollbars.
 * 
 * 1     97/10/15 2:26p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGGUN_HPP
#define HEADER_GUIAPP_DIALOGGUN_HPP


//
// Necessary includes.
//
#include "DialogPhysics.hpp"


/////////////////////////////////////////////////////////////////////////////
// CDialogGun dialog

class CDialogGun : public CDialog
{
// Construction
public:
	CDialogGun(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogGun)
	enum { IDD = IDD_GUN };
	CScrollFloat m_GunImpulsePull;
	CScrollFloat m_GunExtensionPull;
	CScrollFloat m_GunExtension;
	CScrollFloat m_GunImpulse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogGun)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//*****************************************************************************************
	void DisplayText();

	// Generated message map functions
	//{{AFX_MSG(CDialogGun)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // HEADER_GUIAPP_DIALOGGUN_HPP
