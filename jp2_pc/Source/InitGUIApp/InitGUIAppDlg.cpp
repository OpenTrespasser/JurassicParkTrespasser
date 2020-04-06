// InitGUIAppDlg.cpp : implementation file
//

#include <stdio.h>
#include "stdafx.h"
#include "InitGUIApp.h"
#include "InitGUIAppDlg.h"
#include "../Lib/Sys/reg.h"
#include "../Lib/Sys/RegInit.hpp"
#include "DDDevice.hpp"
#include "lib/w95/Direct3DQuery.hpp"

int iNumDDDevices = 0;			// Number of DD devices.
SDDDevice addDevices[100];		// More DD devices than can exist.
int iNumResolutions = 0;		// Number of DD devices.
SResolution arResolutions[100];	// More DD devices than can exist.
int iNumD3D = 0;
SD3DDevice ad3dDrivers[100];
ECardType ecardSelectedCard;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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
// CInitGUIAppDlg dialog

CInitGUIAppDlg::CInitGUIAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInitGUIAppDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInitGUIAppDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInitGUIAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitGUIAppDlg)
	DDX_Control(pDX, IDC_LIST_CARD, m_ListCards);
	DDX_Control(pDX, IDC_COMBO_D3D, m_D3DDriver);
	DDX_Control(pDX, IDC_CHECK_SYSTEMMEM, m_SystemMem);
	DDX_Control(pDX, IDC_COMBO_RESOLUTION, m_Resolutions);
	DDX_Control(pDX, IDC_COMBO_VIDEOGUID, m_VideoGUID);
	DDX_Control(pDX, IDC_CHECK_USED3D, m_UseD3D);
	DDX_Control(pDX, IDC_CHECK_FULLSCREEN, m_FullScreen);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInitGUIAppDlg, CDialog)
	//{{AFX_MSG_MAP(CInitGUIAppDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_VIDEOGUID, OnSelchangeComboVideoguid)
	ON_CBN_SELCHANGE(IDC_COMBO_D3D, OnSelchangeComboD3d)
	ON_LBN_SELCHANGE(IDC_LIST_CARD, OnSelchangeListCard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitGUIAppDlg message handlers

BOOL CInitGUIAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	InitializeCardSelection();
	InitializeOptions();
	InitializeResolutions();
	InitializeD3D();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInitGUIAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CInitGUIAppDlg::OnPaint() 
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

HCURSOR CInitGUIAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CInitGUIAppDlg::InitializeOptions()
{
	// Clear current selections.
	m_VideoGUID.ResetContent();

	// Get a list of direct draw devices.
	if (!bFindDevices(addDevices, iNumDDDevices) || iNumDDDevices < 1)
	{
		MessageBox("Could not find any DirectDraw devices.", "Error", MB_OK | MB_ICONHAND);
		exit(-1);
	}

	// Add these devices to the combo box used for video modes.
	for (int i = 0; i < iNumDDDevices; ++i)
	{
		m_VideoGUID.AddString(addDevices[i].strName);
	}

	// Set the first device as the default.
	m_VideoGUID.SetCurSel(0);
	SetDlgItemText(IDC_STATIC_VIDEOGUID, addDevices[0].strDescription);
}

void CInitGUIAppDlg::InitializeResolutions()
{
	// Clear current selections.
	m_Resolutions.ResetContent();

	// Get the device currently selected.
	int  i_device = m_VideoGUID.GetCurSel();
	GUID guid = addDevices[i_device].guidID;

	// Get a list of supported resolutions for the guid.
	bFindResolutions(guid, arResolutions, iNumResolutions);

	// Add these devices to the combo box used for video modes.
	for (int i = 0; i < iNumResolutions; ++i)
	{
		char str_desc[32];

		sprintf(str_desc, "%ld x %ld (%ld Hz)", arResolutions[i].iWidth,
			    arResolutions[i].iHeight, arResolutions[i].iRefreshHz);
		m_Resolutions.AddString(str_desc);
	}

	// Set the first device as the default.
	m_Resolutions.SetCurSel(0);
}

void CInitGUIAppDlg::InitializeD3D()
{
	// Clear current selections.
	m_D3DDriver.ResetContent();

	// Get the device currently selected.
	int  i_device = m_VideoGUID.GetCurSel();
	GUID guid = addDevices[i_device].guidID;

	// Get a list of supported resolutions for the guid.
	bFindD3D(guid, ad3dDrivers, iNumD3D);

	// Add these devices to the combo box used for video modes.
	for (int i = 0; i < iNumD3D; ++i)
	{
		m_D3DDriver.AddString(ad3dDrivers[i].strName);
	}

	// Set the first device as the default.
	m_D3DDriver.SetCurSel(0);

	// Write out the description for the first device.
	SetDlgItemText(IDC_STATIC_D3D, ad3dDrivers[0].strDescription);
}

void CInitGUIAppDlg::OnOK() 
{
	//
	// Write stuff to the registry before existing.
	//

	// Get the device and resolution currently selected.
	int i_device     = m_VideoGUID.GetCurSel();
	int i_driver     = m_D3DDriver.GetCurSel();
	int i_resolution = m_Resolutions.GetCurSel();

	// Open the registry.
	OpenKey();

	// Write out the direct draw driver selection.
	WriteDDDescription(addDevices[i_device].strName, addDevices[i_device].strDescription);
	WriteDDGUID(addDevices[i_device].guidID);

	// Write out flags.
	SetFullScreen(m_FullScreen.GetState() != 0);
	SetD3D(m_UseD3D.GetState() != 0);
	bSetSystemMem(m_SystemMem.GetState() != 0);
	bSetInitFlag(TRUE);

	// Write out the resolution.
	SetDimensions(arResolutions[i_resolution].iWidth, arResolutions[i_resolution].iHeight);

	// Write out the D3D driver selection.
	WriteD3DDescription(ad3dDrivers[i_driver].strName, ad3dDrivers[i_driver].strDescription);
	WriteD3DGUID(ad3dDrivers[i_driver].guidID);

	// Write out the video card selection.
	ecardSelectedCard = ECardType(m_ListCards.GetCurSel());
	SetVideoCard(ecardSelectedCard);

	// Close the registry and exit.
	CloseKey();
	MessageBox("Registry initialization successful!", "Registry Initialization", MB_OK);
	CDialog::OnOK();
}

void CInitGUIAppDlg::OnSelchangeComboVideoguid() 
{
	int i_device = m_VideoGUID.GetCurSel();
	SetDlgItemText(IDC_STATIC_VIDEOGUID, addDevices[i_device].strDescription);
	InitializeResolutions();
	InitializeD3D();
}

void CInitGUIAppDlg::OnSelchangeComboD3d() 
{
	int i_driver = m_D3DDriver.GetCurSel();
	SetDlgItemText(IDC_STATIC_D3D, ad3dDrivers[i_driver].strDescription);
}

void CInitGUIAppDlg::InitializeCardSelection()
{
	OpenKey();
	ecardSelectedCard = ecardGetVideoCard();
	CloseKey();

	for (ECardType ecard = ecardBegin; ecard < ecardEnd; ecard = ECardType(int(ecard) + 1))
	{
		m_ListCards.AddString(strCardNames[ecard]);
	}
	m_ListCards.SetCurSel(int(ecardSelectedCard));
}

void CInitGUIAppDlg::OnSelchangeListCard() 
{	
}
