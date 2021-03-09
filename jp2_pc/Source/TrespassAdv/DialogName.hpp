#pragma once

// DialogName.hpp : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogName dialog

class CDialogName : public CDialog
{
// Construction
public:
	CDialogName(char* str_name, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogName)
	enum { IDD = IDD_DIALOG_NAME };
	CEdit	m_EditName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	char* strName;

	// Generated message map functions
	//{{AFX_MSG(CDialogName)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
