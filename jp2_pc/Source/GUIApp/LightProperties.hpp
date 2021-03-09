/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Dialog box for manipulating the properties of lights.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/LightProperties.hpp                                           $
 * 
 * 4     97/09/08 17:28 Speter
 * Fixes for old lighting changes; now works again.
 * 
 * 3     97/01/07 12:10 Speter
 * Made all CRenderTypes use rptr<>.
 * 
 * 2     8/09/96 3:32p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUINTERFACE_LIGHTPROPERTIES_HPP
#define HEADER_GUINTERFACE_LIGHTPROPERTIES_HPP


//*********************************************************************************************
//
class CLightProperties : public CDialog
//
// Dialog for light properties.
//
// Prefix: guilt
//
//**************************************
{
protected:

	rptr<CLightAmbient>          pltaAmbient;
	rptr<CLightDirectional>      pltdDirectional;
	rptr<CLightPoint>            pltPoint;
	rptr<CLightPointDirectional> pltpdPointDirectional;

public:

	//*****************************************************************************************
	//
	// Constructor
	//

	// AppWizard generated constructor.
	CLightProperties(CWnd* pParent = NULL);   // standard constructor

	//*****************************************************************************************
	//
	void Show
	(
		rptr<CLight> plt = rptr0	// Pointer to the light to act upon.
	);
	//
	// Activate the light properties dialog box, and size it according to the type of light.
	//
	//**************************************


	//*****************************************************************************************
	//
	// ClassWizard stuff.
	//

	//{{AFX_DATA(CLightProperties)
	enum { IDD = IDD_LIGHT_PROPERTIES };
	CScrollBar	m_ScrollOriginDistance;
	CScrollBar	m_ScrollIntensity;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightProperties)
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

	//{{AFX_MSG(CLightProperties)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnChangeEditCosFull();
	afx_msg void OnChangeEditCosZero();
	afx_msg void OnChangeEditFalloff();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
