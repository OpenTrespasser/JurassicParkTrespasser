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
 * $Log:: /JP2_PC/Source/GUIApp/DialogAlphaColour.hpp                                         $
 * 
 * 4     97/08/13 7:05p Pkeet
 * Cleaned up code. Added support for interleaved alpha colours. Added code for th 565 format,
 * but have not been able to get it to work correctly yet.
 * 
 * 3     97/08/13 3:29p Pkeet
 * Added the 'RedrawScreen' member function.
 * 
 * 2     97/08/11 12:04p Pkeet
 * Put creation code into the setup function. Made test work with a alpha colour number.
 * 
 * 1     97/08/09 2:15p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGALPHACOLOUR_HPP
#define HEADER_GUIAPP_DIALOGALPHACOLOUR_HPP


/////////////////////////////////////////////////////////////////////////////
// CDialogAlphaColour dialog

class CDialogAlphaColour : public CDialog
{
// Construction
public:
	CDialogAlphaColour(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogAlphaColour)
	enum { IDD = IDD_ALPHA_COLOUR };
	CScrollBar	m_ScrollRed;
	CScrollBar	m_ScrollGreen;
	CScrollBar	m_ScrollColour;
	CScrollBar	m_ScrollBlue;
	CScrollBar	m_ScrollBlend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAlphaColour)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void DisplayText();
	void RebuildTable();
	void RedrawScreen();

	// Generated message map functions
	//{{AFX_MSG(CDialogAlphaColour)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnButtonView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//
// Global variables.
//
extern int iAlphaColour;


#endif // HEADER_GUIAPP_DIALOGALPHACOLOUR_HPP
