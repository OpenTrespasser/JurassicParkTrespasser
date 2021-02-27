#pragma once

// DialogGamma.hpp : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogGamma dialog

class CDialogGamma : public CDialog
{
// Construction
public:
	CDialogGamma(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogGamma)
	enum { IDD = IDD_GAMMA };
	CScrollBar	m_ScrollGamma;
	CScrollBar	m_ScrollClutStart;
	CButton	m_ButtonGamma;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogGamma)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	bool bEnable;
	float fPower;

	void Update();

	// Generated message map functions
	//{{AFX_MSG(CDialogGamma)
	afx_msg void OnEnableGamma();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClose();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
