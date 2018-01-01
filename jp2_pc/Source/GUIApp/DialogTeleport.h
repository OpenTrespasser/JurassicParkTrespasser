#if !defined(AFX_DIALOGTELEPORT_H__1B486482_381A_11D2_ADE2_00A0C92A3AE4__INCLUDED_)
#define AFX_DIALOGTELEPORT_H__1B486482_381A_11D2_ADE2_00A0C92A3AE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogTeleport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogTeleport dialog

class CDialogTeleport : public CDialog
{
// Construction
public:
	CDialogTeleport(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogTeleport)
	enum { IDD = IDD_TELEPORT };
	CEdit	m_editTeleZ;
	CEdit	m_editTeleY;
	CEdit	m_editTeleX;
	CString	m_csCurrX;
	CString	m_csCurrY;
	CString	m_csCurrZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogTeleport)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogTeleport)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGTELEPORT_H__1B486482_381A_11D2_ADE2_00A0C92A3AE4__INCLUDED_)
