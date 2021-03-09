/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:	Sky Dialog
 *
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogSky.hpp                                                 $
 * 
 * 3     10/22/97 5:04p Rwyatt
 * New slider for perspective sub division
 * Slider ranges are now sensible
 * 
 * 2     10/14/97 7:01p Rwyatt
 * First Check in
 * 
 * 1     10/14/97 7:01p Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGSKY_HPP
#define HEADER_GUIAPP_DIALOGSKY_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
class CDialogSky : public CDialog
//
// Fog settings dialog box.
//
//**************************************
{
public:
	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CDialogSky(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CDialogSky)
	enum { IDD = IDD_SKY };
	CSliderCtrl		sliderSkyHeight;
	CSliderCtrl		sliderSkyScale;
	CSliderCtrl		sliderSkyNearFog;
	CSliderCtrl		sliderSkyFarFog;

	CSliderCtrl		sliderWindX;
	CSliderCtrl		sliderWindY;

	CSliderCtrl		sliderDivide;

	CString			strSkyHeight;
	CString			strSkyScale;
	CString			strSkyNearFog;
	CString			strSkyFarFog;

	CString			strWindX;
	CString			strWindY;

	CString			strDivide;


	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogSky)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_MSG(CDialogSky)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
