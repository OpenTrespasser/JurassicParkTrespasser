// CollisionEditorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCollisionEditorDlg dialog

#include "resource.h"

class CCollisionEditorDlg : public CDialog
{
// Construction
public:
	CCollisionEditorDlg(CWnd* pParent = NULL);	// standard constructor
	~CCollisionEditorDlg();

// Dialog Data
	//{{AFX_DATA(CCollisionEditorDlg)
	enum { IDD = IDD_COLLISIONEDITOR_DIALOG };
	CListBox	lbMaterialList;
	CEdit		eAddMaterial;
	CString		cstrAddMaterial;

	CListCtrl	lcEffects;
	CString		cstrEffectID;
	CString		cstrEffectFile;
	int			iLang;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCollisionEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	virtual void OnCancel() override;
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON	m_hIcon;
	bool	bModified;
	// Generated message map functions
	//{{AFX_MSG(CCollisionEditorDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnCopy();
	afx_msg void OnRename();
	afx_msg void OnTest();
	afx_msg void OnLoadfile();
	afx_msg void OnAddEffect();
	afx_msg void OnRenameEffect();
	afx_msg void OnWritefile();
	afx_msg void OnListSamples();
	afx_msg void OnDblclkMaterialList();
	afx_msg void OnBrowseSample();
	afx_msg void OnDeleteSample();
	afx_msg void OnWriteBinary();
	afx_msg void OnTestEffect();
	afx_msg void OnStopEffect();
	afx_msg void OnEnableCollisions();
	afx_msg void OnSampleProp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetEffectsModified(bool b_mod);
	void EnableCollisionWindow(bool b_enable);

};
