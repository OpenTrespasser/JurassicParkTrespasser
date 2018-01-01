/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogString.hpp                                              $
 * 
 * 3     10/02/97 9:49p Agrant
 * Specify string caption and default at creation time.
 * 
 * 2     10/02/97 8:11p Rwyatt
 * 
 * 1     10/02/97 8:11p Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGSTRING_HPP
#define HEADER_GUIAPP_DIALOGSTRING_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
class CDialogString : public CDialog
//
// Fog settings dialog box.
//
//**************************************
{
public:
	CString strCaption;		// The dialog title.


	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CDialogString(char *ac_caption, char *ac_default_text = "", CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CDialogString)
	enum { IDD = IDD_STRING };
	CString		strText;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogString)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_MSG(CDialogString)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
