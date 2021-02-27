#pragma once

// DialogMipmap.hpp : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogMipmap dialog

class CDialogMipmap : public CDialog
{
// Construction
public:
	CDialogMipmap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogMipmap)
	enum { IDD = IDD_MIPMAP };
	CScrollBar	m_ScrollThreshold;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogMipmap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	void DisplayText();

	// Generated message map functions
	//{{AFX_MSG(CDialogMipmap)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
