// ParameterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParameterDlg dialog

class CFeeling;

class CParameterDlg : public CDialog
{
// Construction
public:
	CParameterDlg(CWnd* pParent = NULL);   // standard constructor
	CFeeling *pFeeling;						// The feeling to set and get from
	bool bDidInit;
	void SetFeeling(CFeeling * pfeel, float f_low, float f_high);		// Set the target feeling with possible min and max values
	void UpdateEditFromScroll();
	void UpdateScrollFromEdit();

// Dialog Data
	//{{AFX_DATA(CParameterDlg)
	enum { IDD = IDD_PARAMETER_DIALOG };
	CSliderCtrl	m_ParamSlider8;
	CSliderCtrl	m_ParamSlider7;
	CSliderCtrl	m_ParamSlider6;
	CSliderCtrl	m_ParamSlider5;
	CSliderCtrl	m_ParamSlider4;
	CSliderCtrl	m_ParamSlider3;
	CSliderCtrl	m_ParamSlider2;
	CSliderCtrl	m_ParamSlider1;
	CSliderCtrl	m_ParamSlider0;
	float	m_Param0;
	float	m_Param1;
	float	m_Param2;
	float	m_Param3;
	float	m_Param4;
	float	m_Param5;
	float	m_Param6;
	float	m_Param7;
	float	m_Param8;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParameterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParameterDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEditParameter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
