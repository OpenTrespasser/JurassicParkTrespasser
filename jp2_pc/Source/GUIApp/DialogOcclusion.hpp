#pragma once

/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogOcclusion.hpp                                           $
 * 
 * 2     97/10/08 6:21p Pkeet
 * Added code to display values and operate scrollbars.
 * 
 * 1     97/10/08 6:02p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/


//
// Class definitions.
//

class CDialogOcclusion : public CDialog
{
// Construction
public:
	CDialogOcclusion(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogOcclusion)
	enum { IDD = IDD_OCCLUDE };
	CScrollBar	m_AreaThreshold;
	CScrollBar	m_MaxInView;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogOcclusion)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void DisplayText();
	void RedrawScreen();

	// Generated message map functions
	//{{AFX_MSG(CDialogOcclusion)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
