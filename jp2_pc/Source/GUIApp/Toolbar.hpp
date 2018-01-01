/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		A toolbar for operating the GUI application.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/Toolbar.hpp                                                   $
 * 
 * 7     97/09/08 17:28 Speter
 * OpenLightProperties takes CLight*, not CLightAmbient*.
 * 
 * 6     97-05-12 11:38 Speter
 * Removed egiuMode, and "Camera" and "Object" commands.
 * 
 * 5     97/01/07 12:10 Speter
 * Made all CRenderTypes use rptr<>.
 * 
 * 4     8/15/96 1:30p Pkeet
 * Replaced 'ROTATION' and 'TRANSLATION' movement types with the 'shift' key.
 * 
 * 3     8/13/96 3:04p Pkeet
 * Replicated toolbar functions in the menu.
 * 
 * 2     8/09/96 3:21p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUINTERFACE_TOOLBAR_HPP
#define HEADER_GUINTERFACE_TOOLBAR_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
class CTool : public CDialog
//
// Toolbar for operating the GUI.
//
// Prefix: guitool
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor
	//

	// AppWizard generated constructor.
	CTool(CWnd* pParent = NULL);


	//*****************************************************************************************
	//
	// ClassWizard stuff.
	//

	//{{AFX_DATA(CTool)
	enum { IDD = IDD_FLOATTOOLS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTool)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	//*****************************************************************************************
	void SetButtonStates();

	//*****************************************************************************************
	void OpenLightProperties(rptr<CLight> plt);


	//*****************************************************************************************
	//
	// ClassWizard stuff.
	//

	//{{AFX_MSG(CTool)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRadioMove();
	afx_msg void OnRadioObject();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonAddobject();
	afx_msg void OnButtonAdddirlight();
	afx_msg void OnButtonAddptdirlight();
	afx_msg void OnButtonAddptlight();
	afx_msg void OnButtonEditambient();
	afx_msg void OnButtonEditcamera();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//
// External variable declarations.
//

#endif
