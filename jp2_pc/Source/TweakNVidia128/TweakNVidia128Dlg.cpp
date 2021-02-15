// TweakNVidia128Dlg.cpp : implementation file
//

#include "StdAfx.h"
#include "TweakNVidia128.h"
#include "TweakNVidia128Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGKEYPARENT HKEY_LOCAL_MACHINE
#define REGLOCATION "SOFTWARE\\NV3D3D"
#define REG_MIPMAPS "MIPMAPLEVELS"
#define REG_SQUAREONLY "SQUAREONLYENABLE"

const int iNumMipmaplevels  = 0;
const int iSquareOnlyEnable = 0;

HKEY g_hKey = NULL;

void CloseKey()
{
	if (g_hKey)
		RegCloseKey (g_hKey);
}

void OpenKey()
{
	if (RegOpenKeyEx(REGKEYPARENT, REGLOCATION, 0, KEY_ALL_ACCESS, &g_hKey) != ERROR_SUCCESS)
		RegCreateKey(REGKEYPARENT, REGLOCATION, &g_hKey);
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTweakNVidia128Dlg dialog

CTweakNVidia128Dlg::CTweakNVidia128Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTweakNVidia128Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTweakNVidia128Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTweakNVidia128Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTweakNVidia128Dlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTweakNVidia128Dlg, CDialog)
	//{{AFX_MSG_MAP(CTweakNVidia128Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, OnButtonRestore)
	ON_BN_CLICKED(IDC_BUTTON_TWEAK, OnButtonTweak)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTweakNVidia128Dlg message handlers

BOOL CTweakNVidia128Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTweakNVidia128Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTweakNVidia128Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CTweakNVidia128Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTweakNVidia128Dlg::OnButtonRestore() 
{
	OpenKey();
	RegDeleteValue(g_hKey, REG_MIPMAPS);
	RegDeleteValue(g_hKey, REG_SQUAREONLY);
	CloseKey();
	MessageBox("Original settings restored.", "Tweak NVidia Riva 128", MB_OK | MB_ICONWARNING);
}

void CTweakNVidia128Dlg::OnButtonTweak() 
{
	OpenKey();
	RegSetValueEx(g_hKey, REG_MIPMAPS, NULL, REG_DWORD, (LPBYTE)&iNumMipmaplevels, sizeof(iNumMipmaplevels));
	RegSetValueEx(g_hKey, REG_SQUAREONLY, NULL, REG_DWORD, (LPBYTE)&iSquareOnlyEnable, sizeof(iSquareOnlyEnable));
	CloseKey();
	MessageBox("Settings tweaked for Trespasser. Please remember and restore the original\n"
		       "settings when you are finished playing Trespasser.", "Tweak NVidia Riva 128", MB_OK | MB_ICONWARNING);
}
