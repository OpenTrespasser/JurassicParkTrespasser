/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Dialog box for setting depth sort values.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogDepthSort.hpp                                           $
 * 
 * 7     98.02.04 1:57p Mmouni
 * Added new settings for depth sort partitoning.
 * 
 * 6     97/07/30 2:29p Pkeet
 * Added support for separate terrain tolerances.
 * 
 * 5     97/05/26 5:34p Pkeet
 * Added the maximum number of polygons that can be depth sorted scroll bar.
 * 
 * 4     97/05/21 6:29p Pkeet
 * Added pixel tolerance scrollbar.
 * 
 * 3     97/05/20 4:23p Pkeet
 * Set up linear and inverse selection buttons.
 * 
 * 2     97/05/20 3:14p Pkeet
 * Implemented all features for the dialog as it stands.
 * 
 * 1     97/05/20 12:10p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGDEPTHSORT_HPP
#define HEADER_GUIAPP_DIALOGDEPTHSORT_HPP




//
// Class definitions.
//

//*********************************************************************************************
//
class CDialogDepthSort : public CDialog
//
// Depth sort settings dialog box.
//
//**************************************
{
// Construction
public:
	CDialogDepthSort(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDepthSort)
	enum { IDD = IDD_DEPTHSORT_SETTINGS };
	CScrollBar	m_NearZTerrain;
	CScrollBar	m_NearTolTerrain;
	CScrollBar	m_FarZTerrain;
	CScrollBar	m_FarTolTerrain;
	CButton	m_CheckTerrain;
	CScrollBar	m_MaxNum;
	CScrollBar	m_ScrollPixelTolerance;
	CScrollBar	m_ScrollNearZ;
	CScrollBar	m_ScrollNearTolerance;
	CScrollBar	m_ScrollFarZNoDepthSort;
	CScrollBar	m_ScrollFarZ;
	CScrollBar	m_ScrollFarTolerance;
	CScrollBar	m_BinaryAt;
	CScrollBar	m_QuadAt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDepthSort)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	void DisplayText();
	void Redraw();
	void SetButtons();

	// Generated message map functions
	//{{AFX_MSG(CDialogDepthSort)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRadioInverse();
	afx_msg void OnRadioLinear();
	afx_msg void OnCheckTerrain();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // HEADER_GUIAPP_DIALOGDEPTHSORT_HPP
