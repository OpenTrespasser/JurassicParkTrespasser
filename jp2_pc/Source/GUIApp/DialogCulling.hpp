/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
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
 * $Log:: /JP2_PC/Source/GUIApp/DialogCulling.hpp                                             $
 * 
 * 5     5/14/98 7:34p Pkeet
 * Added a priority slider.
 * 
 * 4     5/01/98 7:53p Pkeet
 * Changed culling parameters.
 * 
 * 3     4/24/98 4:03p Pkeet
 * Added a scrollbar for shadow culling.
 * 
 * 2     2/26/98 2:47p Pkeet
 * Implemented the basic functionality.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGCULLING_HPP
#define HEADER_GUIAPP_DIALOGCULLING_HPP


//
// Required includes.
//
#include "DialogScrollbars.hpp"


//
// Class definitions.
//

class CDialogCulling : public CDialog
{
// Construction
public:
	CDialogCulling(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogCulling)
	enum { IDD = IDD_DIALOG_OBJECTCULLING };
	CScrollBar	m_Priority;
	CScrollFloatLog	m_ScrollCullingShadows;
	CScrollFloatLog	m_ScrollCulling;
	CScrollFloatLog	m_ScrollRadius;
	CScrollFloatLog	m_ScrollRadiusShadows;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogCulling)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//*****************************************************************************************
	void DisplayText();

	// Generated message map functions
	//{{AFX_MSG(CDialogCulling)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // HEADER_GUIAPP_DIALOGCULLING_HPP
