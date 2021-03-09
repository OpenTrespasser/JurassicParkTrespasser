// AI Dialogs2Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAIDialogs2Dlg dialog

class CBrain;
class CParameterDlg;

class CAIDialogs2Dlg : public CDialog
{
// Construction
public:
	CAIDialogs2Dlg(CWnd* pParent = NULL);	// standard constructor

	CBrain* pbrBrain;				// The brain of the dino
	CParameterDlg*  ppdState;		// Dialog for the feelings of the dino

// Dialog Data
	//{{AFX_DATA(CAIDialogs2Dlg)
	enum { IDD = IDD_AIDIALOGS2_DIALOG };
	CStatic	m_AIText;
	CListBox	m_ActivityList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAIDialogs2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAIDialogs2Dlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK() override;
	virtual void OnCancel() override;
	afx_msg void OnAll();
	afx_msg void OnNone();
	afx_msg void OnDblclkActivities();
	afx_msg void OnSelchangeActivities();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
