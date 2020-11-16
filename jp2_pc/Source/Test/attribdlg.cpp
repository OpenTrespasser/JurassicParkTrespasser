// AttribDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "AudioTest.h"
#include "attribdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoundAttribDlg dialog


CSoundAttribDlg::CSoundAttribDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundAttribDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSoundAttribDlg)
	m_filename = _T("");
	m_intensity = 0.0f;
	m_maxback = 0.0f;
	m_maxfront = 0.0f;
	m_minback = 0.0f;
	m_minfront = 0.0f;
	m_orientx = 0.0f;
	m_orienty = 0.0f;
	m_orientz = 0.0f;
	m_posx = 0.0f;
	m_posy = 0.0f;
	m_posz = 0.0f;
	m_fattenuate = FALSE;
	m_fdoppler = FALSE;
	m_freverb = FALSE;
	m_fspatialize = FALSE;
	//}}AFX_DATA_INIT
}


void CSoundAttribDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoundAttribDlg)
	DDX_Text(pDX, IDC_FILENAME, m_filename);
	DDX_Text(pDX, IDC_INTENSITY, m_intensity);
	DDX_Text(pDX, IDC_MAXBACK, m_maxback);
	DDX_Text(pDX, IDC_MAXFRONT, m_maxfront);
	DDX_Text(pDX, IDC_MINBACK, m_minback);
	DDX_Text(pDX, IDC_MINFRONT, m_minfront);
	DDX_Text(pDX, IDC_ORIENTX, m_orientx);
	DDX_Text(pDX, IDC_ORIENTY, m_orienty);
	DDX_Text(pDX, IDC_ORIENTZ, m_orientz);
	DDX_Text(pDX, IDC_POSX, m_posx);
	DDX_Text(pDX, IDC_POSY, m_posy);
	DDX_Text(pDX, IDC_POSZ, m_posz);
	DDX_Check(pDX, IDC_FATTENUATE, m_fattenuate);
	DDX_Check(pDX, IDC_FDOPPLER, m_fdoppler);
	DDX_Check(pDX, IDC_FREVERB, m_freverb);
	DDX_Check(pDX, IDC_FSPATIALIZE, m_fspatialize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSoundAttribDlg, CDialog)
	//{{AFX_MSG_MAP(CSoundAttribDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundAttribDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CListenerAttribDlg dialog


CListenerAttribDlg::CListenerAttribDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CListenerAttribDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CListenerAttribDlg)
	m_dirx = 0.0f;
	m_diry = 0.0f;
	m_dirz = 0.0f;
	m_posx = 0.0f;
	m_posy = 0.0f;
	m_posz = 0.0f;
	m_upx = 0.0f;
	m_upy = 0.0f;
	m_upz = 0.0f;
	//}}AFX_DATA_INIT
}


void CListenerAttribDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListenerAttribDlg)
	DDX_Text(pDX, IDC_DIRX, m_dirx);
	DDX_Text(pDX, IDC_DIRY, m_diry);
	DDX_Text(pDX, IDC_DIRZ, m_dirz);
	DDX_Text(pDX, IDC_POSX, m_posx);
	DDX_Text(pDX, IDC_POSY, m_posy);
	DDX_Text(pDX, IDC_POSZ, m_posz);
	DDX_Text(pDX, IDC_UPX, m_upx);
	DDX_Text(pDX, IDC_UPY, m_upy);
	DDX_Text(pDX, IDC_UPZ, m_upz);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListenerAttribDlg, CDialog)
	//{{AFX_MSG_MAP(CListenerAttribDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListenerAttribDlg message handlers
/////////////////////////////////////////////////////////////////////////////
// CEnvAttribDlg dialog


CEnvAttribDlg::CEnvAttribDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnvAttribDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnvAttribDlg)
	m_revdecay = 0.0f;
	m_revintensity = 0.0f;
	m_usereverb = FALSE;
	m_userighthand = FALSE;
	m_speedofsound = 0.0f;
	m_cpulocbudget = -1;
	//}}AFX_DATA_INIT
}


void CEnvAttribDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnvAttribDlg)
	DDX_Text(pDX, IDC_REVDECAY, m_revdecay);
	DDX_Text(pDX, IDC_REVINTENSITY, m_revintensity);
	DDX_Check(pDX, IDC_USEREVERB, m_usereverb);
	DDX_Check(pDX, IDC_USERIGHTHAND, m_userighthand);
	DDX_Text(pDX, IDC_SPEEDOFSOUND, m_speedofsound);
	DDX_Radio(pDX, IDC_CPULOC_NONE, m_cpulocbudget);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnvAttribDlg, CDialog)
	//{{AFX_MSG_MAP(CEnvAttribDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnvAttribDlg message handlers


BOOL CSoundAttribDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( m_bControl )
	{
		GetDlgItem ( IDC_FATTENUATE )->EnableWindow( FALSE );
		GetDlgItem ( IDC_FSPATIALIZE )->EnableWindow( FALSE );
		GetDlgItem ( IDC_FDOPPLER )->EnableWindow( FALSE );
		GetDlgItem ( IDC_FREVERB )->EnableWindow( FALSE );
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CMusicSoundAttribDlg dialog


CMusicSoundAttribDlg::CMusicSoundAttribDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMusicSoundAttribDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMusicSoundAttribDlg)
	m_fdoppler = FALSE;
	m_freverb = FALSE;
	//}}AFX_DATA_INIT
}


void CMusicSoundAttribDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMusicSoundAttribDlg)
	DDX_Check(pDX, IDC_FDOPPLER, m_fdoppler);
	DDX_Check(pDX, IDC_FREVERB, m_freverb);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMusicSoundAttribDlg, CDialog)
	//{{AFX_MSG_MAP(CMusicSoundAttribDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMusicSoundAttribDlg message handlers
