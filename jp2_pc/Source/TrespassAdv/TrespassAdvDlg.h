// TrespassAdvDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrespassAdvDlg dialog

class CTrespassAdvDlg : public CDialog
{
// Construction
public:
	CTrespassAdvDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTrespassAdvDlg)
	enum { IDD = IDD_TRESPASSADV_DIALOG };
	CButton	m_DirectTextureAccess;
	CButton	m_Supported;
	CScrollBar	m_ScrollScreen;
	CScrollBar	m_ScrollClip;
	CButton	m_CheckSky;
	CButton	m_SharedBuffers;
	CButton	m_SecondaryCard;
	CButton	m_RegionUploads;
	CButton	m_HardwareCaches;
	CButton	m_FilterCaches;
	CButton	m_Dither;
	CButton	m_ClipFog;
	CButton	m_CacheFog;
	CButton	m_AlphaTexture;
	CButton	m_AlphaColour;
	CListBox	m_ListCards;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrespassAdvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	BOOL  iChangedFile;
	BOOL  bChangedCard;

	void InitializeCards(BOOL b_load_from_file = TRUE);
	void InitializeControls();
	void OnButtonRegistry();
	void OnButtonSave();
	void SetText();
	void CloseSave();

	// Generated message map functions
	//{{AFX_MSG(CTrespassAdvDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonAddCard();
	virtual void OnOK();
	afx_msg void OnDblclkListCards();
	afx_msg void OnCheckAlphaColour();
	afx_msg void OnCheckAlphaTextures();
	afx_msg void OnCheckCacheFog();
	afx_msg void OnCheckClipFog();
	afx_msg void OnCheckDither();
	afx_msg void OnCheckFilterCaches();
	afx_msg void OnCheckHardwareCaches();
	afx_msg void OnCheckRegionUploads();
	afx_msg void OnCheckSecondaryCard();
	afx_msg void OnCheckSharedBuffers();
	afx_msg void OnCheckSky();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchangeListCards();
	afx_msg void OnButtonRemoveCard();
	afx_msg void OnClose();
	afx_msg void OnCheckSupported();
	afx_msg void OnButtonCardNotes();
	afx_msg void OnButtonChipset();
	afx_msg void OnButtonDriver();
	afx_msg void OnCheckDirectTextureAccess();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
