// TweakNVidia128Dlg.h : header file
//

#if !defined(AFX_TWEAKNVIDIA128DLG_H__20EAD00C_3303_11D2_A32C_00A0C92A3A07__INCLUDED_)
#define AFX_TWEAKNVIDIA128DLG_H__20EAD00C_3303_11D2_A32C_00A0C92A3A07__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CTweakNVidia128Dlg dialog

class CTweakNVidia128Dlg : public CDialog
{
// Construction
public:
	CTweakNVidia128Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTweakNVidia128Dlg)
	enum { IDD = IDD_TWEAKNVIDIA128_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTweakNVidia128Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTweakNVidia128Dlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonRestore();
	afx_msg void OnButtonTweak();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TWEAKNVIDIA128DLG_H__20EAD00C_3303_11D2_A32C_00A0C92A3A07__INCLUDED_)
