#pragma once

// DialogScheduler.hpp : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogScheduler dialog

class CDialogScheduler : public CDialog
{
// Construction
public:
	CDialogScheduler(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogScheduler)
	enum { IDD = IDD_SCHEDULER };
	CScrollBar	m_Threshold;
	CScrollBar	m_ScrollScheduler;
	CScrollBar	m_ScrollSchedulerTerrain;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogScheduler)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void DisplayText();

	// Generated message map functions
	//{{AFX_MSG(CDialogScheduler)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
