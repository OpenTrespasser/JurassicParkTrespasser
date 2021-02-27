#pragma once

// InitGUIApp2Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInitGUIApp2Dlg dialog

class CInitGUIApp2Dlg : public CDialog
{
// Construction
public:
	CInitGUIApp2Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInitGUIApp2Dlg)
	enum { IDD = IDD_INITGUIAPP2_DIALOG };
	CButton	m_ButtonStuff;
	CScrollBar	m_ScrollSubdivision;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInitGUIApp2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CInitGUIApp2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCheckStuffChildren();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
