/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		A dialog box for manipulating camera properties.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/CameraProperties.hpp                                          $
 * 
 * 2     8/09/96 2:37p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_CAMERAPROPERTIES_HPP
#define HEADER_GUIAPP_CAMERAPROPERTIES_HPP


//*********************************************************************************************
//
class CCameraProperties : public CDialog
//
// A dialog box for manipulating camera properties.
//
// Prefix: camdlg
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor
	//

	// AppWizard generated constructor.
	CCameraProperties(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// ClassWizard stuff.
	//

	//{{AFX_DATA(CCameraProperties)
	enum { IDD = IDD_CAMERA_PROPERTIES };
	CScrollBar	m_ScrollZoom;
	CScrollBar	m_ScrollNear;
	CScrollBar	m_ScrollFar;
	CScrollBar	m_ScrollAngle;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCameraProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	//*****************************************************************************************
	void ActOnChange();

	//*****************************************************************************************
	void DisplayText();

	//*****************************************************************************************
	//
	// ClassWizard stuff.
	//

	//{{AFX_MSG(CCameraProperties)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif