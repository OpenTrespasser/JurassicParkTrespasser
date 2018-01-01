/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Dialog box for setting fog values.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogFog.hpp                                                 $
 * 
 * 3     9/26/96 1:55p Pkeet
 * Added controls to set fog table builds based on linear, inverse or exponential functions.
 * 
 * 2     8/20/96 4:48p Pkeet
 * Added fogging switch and dialog.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGFOG_HPP
#define HEADER_GUIAPP_DIALOGFOG_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
class CDialogFog : public CDialog
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
	CDialogFog(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CDialogFog)
	enum { IDD = IDD_FOG };
	CScrollBar	m_ScrollFogPower;
	CScrollBar	m_ScrollFogConstant;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogFog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	//*****************************************************************************************
	void DisplayText();

	//*****************************************************************************************
	void SetButtons();

	//*****************************************************************************************
	void Redraw();

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_MSG(CDialogFog)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRadioLinear();
	afx_msg void OnRadioInverse();
	afx_msg void OnRadioExponential();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
