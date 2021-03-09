/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Dialog box for setting background colours.
 *
 * To Do:
 *		Enable setting of a background bitmap.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/Background.hpp                                                $
 * 
 * 3     8/20/96 4:48p Pkeet
 * Added fogging switch and dialog.
 * 
 * 2     8/15/96 10:49a Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_BACKGROUND_HPP
#define HEADER_GUIAPP_BACKGROUND_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
class CBackground : public CDialog
//
// Dialog to set the background colour.
//
//**************************************
{
protected:

	bool bGrayscale;	// Flag for 'Grayscale' check setting.

public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CBackground(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CBackground)
	enum { IDD = IDD_BACKGROUND };
	CButton	m_ButtonGrayscale;
	CScrollBar	m_ScrollBlue;
	CScrollBar	m_ScrollGreen;
	CScrollBar	m_ScrollRed;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackground)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	//}}AFX_VIRTUAL

protected:

	//*****************************************************************************************
	void DisplayText();

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	// Generated message map functions
	//{{AFX_MSG(CBackground)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCheckGrayscale();
	afx_msg void OnBackTofogback();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif