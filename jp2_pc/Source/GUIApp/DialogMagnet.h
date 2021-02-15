/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		CDialogMagnet
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/DialogMagnet.h                                                $
 * 
 * 1     98/02/06 11:03 Speter
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGMAGNET_HPP
#define HEADER_GUIAPP_DIALOGMAGNET_HPP

#include "Lib/Physics/Magnet.hpp"

/////////////////////////////////////////////////////////////////////////////
// CDialogMagnet dialog

class CDialogMagnet : public CDialog
{
protected:
	ptr<CMagnetPair>	pmpMagnet;			// Magnet object we're editing.
	CMagnet				magData;			// Local copy of magnet properties.

// Construction
public:
	CDialogMagnet(CWnd* pParent = NULL);   // standard constructor

	void SetMagnet(CMagnetPair* pmp);

// Dialog Data
	//{{AFX_DATA(CDialogMagnet)
	enum { IDD = IDD_MAGNET };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogMagnet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogMagnet)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
