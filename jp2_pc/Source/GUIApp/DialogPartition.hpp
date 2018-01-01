/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997
 *
 * Contents:
 *		Dialog for building spatial partitions.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogPartition.hpp                                           $
 * 
 * 4     1/30/98 4:34p Pkeet
 * Removed the fast partition build.
 * 
 * 3     1/20/98 5:18p Pkeet
 * Added a fast create button.
 * 
 * 2     97/09/25 11:42a Pkeet
 * Added a callback to the 'BuildOptimalPartitions' function for reporting progress.
 * 
 * 1     97/09/24 10:55a Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGPARTITIONS_HPP
#define HEADER_GUIAPP_DIALOGPARTITIONS_HPP

class CDialogPartition : public CDialog
{
// Construction
public:
	CDialogPartition(CWnd* pParent = NULL);   // standard constructor
	void Update(int i_count);

// Dialog Data
	//{{AFX_DATA(CDialogPartition)
	enum { IDD = IDD_PARTITION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPartition)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogPartition)
	afx_msg void OnCreate();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif HEADER_GUIAPP_DIALOGPARTITIONS_HPP
