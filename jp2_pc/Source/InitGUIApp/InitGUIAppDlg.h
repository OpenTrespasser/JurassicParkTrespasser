#pragma once

// InitGUIAppDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInitGUIAppDlg dialog

class CInitGUIAppDlg : public CDialog
{
// Construction
public:
	CInitGUIAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInitGUIAppDlg)
	enum { IDD = IDD_INITGUIAPP_DIALOG };
	CListBox	m_ListCards;
	CComboBox	m_D3DDriver;
	CButton	m_SystemMem;
	CComboBox	m_Resolutions;
	CComboBox	m_VideoGUID;
	CButton	m_UseD3D;
	CButton	m_FullScreen;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInitGUIAppDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	virtual void InitializeCardSelection();
	virtual void InitializeOptions();
	virtual void InitializeResolutions();
	virtual void InitializeD3D();

	// Generated message map functions
	//{{AFX_MSG(CInitGUIAppDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnSelchangeComboVideoguid();
	afx_msg void OnSelchangeComboD3d();
	afx_msg void OnSelchangeListCard();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
