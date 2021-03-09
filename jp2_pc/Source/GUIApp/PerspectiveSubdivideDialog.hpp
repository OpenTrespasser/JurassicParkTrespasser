/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Dialog box for examining and changing parameters for perspective correction using
 *		scanline subdivision.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/PerspectiveSubdivideDialog.hpp                                $
 * 
 * 5     98.07.17 6:29p Mmouni
 * Added new adaptive perspective settings.
 * 
 * 4     98.03.24 8:16p Mmouni
 * Made changes to support alternate perspective correction settings.
 * 
 * 3     97/06/24 1:59p Pkeet
 * Added adaptive subdivision.
 * 
 * 2     1/03/97 4:41p Pkeet
 * Added support for changing the subdivision length.
 * 
 * 1     1/03/97 4:04p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_PERSPECTIVESUBDIVISIONDIALOG_HPP
#define HEADER_GUIAPP_PERSPECTIVESUBDIVISIONDIALOG_HPP


//*********************************************************************************************
//
class CPerspectiveSubdivideDialog : public CDialog
//
// A dialog box for manipulating perspective correction properties.
//
//**************************************
{
// Construction
public:
	CPerspectiveSubdivideDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPerspectiveSubdivideDialog)
	enum { IDD = IDD_PERSPECTIVE };
	CScrollBar	m_ScrollMinInvZDX;
	CButton	m_CheckAdaptive;
	CScrollBar	m_SubdivideLen;
	CScrollBar	m_Error;
	CScrollBar	m_SubdivideLen2;
	CScrollBar	m_Error2;
	CScrollBar	m_AdaptiveMinSubdivision;
	CScrollBar	m_AdaptiveMaxSubdivision;
	CScrollBar	m_InvZForMaxSubdivision;
	CScrollBar	m_fInvZScale;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPerspectiveSubdivideDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//*****************************************************************************************
	void DisplayText();

	// Generated message map functions
	//{{AFX_MSG(CPerspectiveSubdivideDialog)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCheckAdaptive();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
