#pragma once

/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Water Dialog.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogWater.hpp                                               $
 * 
 * 2     5/11/98 2:24p Mlange
 * Operational.
 * 
 * 1     5/11/98 1:44p Mlange
 * Initial revision.
 * 
 *********************************************************************************************/


#include "DialogScrollbars.hpp"


//*********************************************************************************************
//
class CDialogWater : public CDialog
//
// Water dialog.
//
//**************************************
{
public:

	//*****************************************************************************************
	CDialogWater(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogWater)
	enum { IDD = IDD_WATER };
	CScrollBar	m_ScrollWaterMaxScreenRes;
	CScrollBar	m_ScrollWaterMaxWorldRes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogWater)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	//*****************************************************************************************
	void DisplayText();

	//*********************************************************************************************
	void Update();

	// Generated message map functions
	//{{AFX_MSG(CDialogWater)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
