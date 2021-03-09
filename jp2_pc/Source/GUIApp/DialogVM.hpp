/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Dialog box for setting virtual memory
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogVM.hpp                                                  $
 * 
 * 3     4/21/98 2:57p Rwyatt
 * Modified for the new VM system
 * 
 * 2     1/29/98 7:26p Rwyatt
 * New buttons to commit all, use conventional loader and commit on load.
 * 
 * 1     12/04/97 3:21p Rwyatt
 * Initial implementation
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGVM_HPP
#define HEADER_GUIAPP_DIALOGVM_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
class CDialogVM : public CDialog
//
// VM settings dialog box.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CDialogVM(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CDialogVM)
	enum { IDD = IDD_VIRTUALMEM };

	CString			strPhysicalMem;

	CSliderCtrl		sliderPhysicalMem;
	int				radPageSize;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogVM)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_MSG(CDialogVM)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnGoVirtual();
	afx_msg void OnDecommit();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnRadio6();
	afx_msg void OnRadio7();
	afx_msg void OnRadio8();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck6();
	afx_msg void OnCommitAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
