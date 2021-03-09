/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998
 *
 * Contents:
 *		Dialog box for controlling texture packing
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogTexturePackOptions.hpp                                  $
 * 
 * 2     1/14/98 6:24p Rwyatt
 * New check box to force the texture packer to pack curved bump maps as power of 2 maps. This
 * is for debugging and testing only
 * 
 * 1     1/09/98 6:47p Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGTEXTUREPACK_HPP
#define HEADER_GUIAPP_DIALOGTEXTUREPACK_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
class CDialogTexturePackOptions : public CDialog
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CDialogTexturePackOptions(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CDialogTexturePackOptions)
	enum { IDD = IDD_DIALOG_PACKOPTIONS };
	CScrollBar	sbPack;

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogTexturePackOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_MSG(CDialogTexturePackOptions)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPackBumps();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
