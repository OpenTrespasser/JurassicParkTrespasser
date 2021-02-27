#pragma once

// DialogGore.hpp : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogGore dialog

class CDialogGore : public CDialog
{
// Construction
public:
	CDialogGore(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogGore)
	enum { IDD = IDD_GORE };
	CScrollBar	m_ScrollGore;
	CButton	m_ButtonGore;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogGore)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void Update();

	// Generated message map functions
	//{{AFX_MSG(CDialogGore)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
