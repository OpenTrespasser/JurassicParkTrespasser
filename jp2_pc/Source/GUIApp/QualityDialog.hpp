/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998
 *
 * Contents:
 *		Dialog for render quality
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/QualityDialog.hpp                                             $
 * 
 * 2     6/10/98 6:37p Rwyatt
 * First working version
 * 
 * 1     6/10/98 6:36p Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGQUALITY_HPP
#define HEADER_GUIAPP_DIALOGQUALITY_HPP


//*********************************************************************************************
//
class CDialogQuality : public CDialog
//
// Sound settings dialog box.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CDialogQuality(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CDialogQuality)
	enum { IDD = IDD_QUALITY };
	CSliderCtrl			sliderQuality;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogQuality)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_MSG(CDialogQuality)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
