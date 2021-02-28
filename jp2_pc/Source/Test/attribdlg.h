#pragma once

// AttribDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSoundAttribDlg dialog

class CSoundAttribDlg : public CDialog
{
// Construction
public:
	CSoundAttribDlg(CWnd* pParent = NULL);   // standard constructor

	BOOL	m_bControl;

// Dialog Data
	//{{AFX_DATA(CSoundAttribDlg)
	enum { IDD = IDD_SOUNDATTRIBUTES };
	CString	m_filename;
	float	m_intensity;
	float	m_maxback;
	float	m_maxfront;
	float	m_minback;
	float	m_minfront;
	float	m_orientx;
	float	m_orienty;
	float	m_orientz;
	float	m_posx;
	float	m_posy;
	float	m_posz;
	BOOL	m_fattenuate;
	BOOL	m_fdoppler;
	BOOL	m_freverb;
	BOOL	m_fspatialize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundAttribDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSoundAttribDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CListenerAttribDlg dialog

class CListenerAttribDlg : public CDialog
{
// Construction
public:
	CListenerAttribDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CListenerAttribDlg)
	enum { IDD = IDD_LISTENERATTRIBUTES };
	float	m_dirx;
	float	m_diry;
	float	m_dirz;
	float	m_posx;
	float	m_posy;
	float	m_posz;
	float	m_upx;
	float	m_upy;
	float	m_upz;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListenerAttribDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CListenerAttribDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CEnvAttribDlg dialog

class CEnvAttribDlg : public CDialog
{
// Construction
public:
	CEnvAttribDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnvAttribDlg)
	enum { IDD = IDD_ENVATTRIBUTES };
	float	m_revdecay;
	float	m_revintensity;
	BOOL	m_usereverb;
	BOOL	m_userighthand;
	float	m_speedofsound;
	int		m_cpulocbudget;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnvAttribDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnvAttribDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSceneAttribDlg dialog

class CSceneAttribDlg : public CDialog
{
// Construction
public:
	CSceneAttribDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSceneAttribDlg)
	enum { IDD = IDD_SCENEATTRIBUTES };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSceneAttribDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSceneAttribDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CMusicSoundAttribDlg dialog

class CMusicSoundAttribDlg : public CDialog
{
// Construction
public:
	CMusicSoundAttribDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMusicSoundAttribDlg)
	enum { IDD = IDD_MUSICSOUNDATTRIBUTES };
	BOOL	m_fdoppler;
	BOOL	m_freverb;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMusicSoundAttribDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMusicSoundAttribDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
