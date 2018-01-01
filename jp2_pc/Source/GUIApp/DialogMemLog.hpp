/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Dialog box for displaying memort usage
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogMemLog.hpp                                              $
 * 
 * 3     12/17/97 12:18a Rwyatt
 * New memory debugging buttons
 * 
 * 2     9/18/97 1:23p Rwyatt
 * Basic memory log view.
 * 
 * 1     9/18/97 1:16p Rwyatt
 * Definition of dialog for memory logging
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_MEMSTATS_HPP
#define HEADER_GUIAPP_MEMSTATS_HPP

//
// Class definitions.
//

//*********************************************************************************************
//
class CDialogMemLog : public CDialog
//
// Texture pack settings dialog box.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CDialogMemLog(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CDialogTexturePack)
	enum { IDD = IDD_DIALOG_MEMSTATS };

	CListCtrl	m_ListLog;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogTexturePack)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	void UpdateList();

	//{{AFX_MSG(CDialogTexturePack)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClearSelected();
	afx_msg void OnCalcFragmentation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
