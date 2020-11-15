// TrespassAdvDlg.cpp : implementation file
//
#include <math.h>
#include "StdAfx.h"
#include "TrespassAdv.h"
#include "TrespassAdvDlg.h"
#include "DialogName.hpp"
#undef min
#undef max
#include "Lib/W95/Direct3DQuery.hpp"
#include "Lib/Sys/reg.h"
#include "Lib/Sys/RegInit.hpp"
#include "Trespass/DDDevice.hpp"

SDDDevice  ddDevice;
SD3DDevice d3dDevice;

CD3DSettings* pd3dsetGetCard(CListBox& list)
{
	int i_sel = list.GetCurSel();
	if (i_sel == LB_ERR)
		i_sel = 0;
	int i_len = list.GetTextLen(i_sel);
	char* str_cardname = new char[i_len + 1];
	list.GetText(i_sel, str_cardname);
	CD3DSettings* pd3dset = d3darD3DSettings.pd3dsetGetD3DSetting(str_cardname);
	delete[] str_cardname;
	return pd3dset;
}


//**********************************************************************************************
void SetDlgItemFloat(CWnd* pwnd, int i_item_id, float f_value, int i_significant_digits)
{
	char str_out[32];

	// Scale the number of fractional digits according to the size of the number.
	if (fabs(f_value) > 0.1f)
		sprintf(str_out, "%1.2f", f_value);
	else
		if (fabs(f_value) > 0.01f)
			sprintf(str_out, "%1.3f", f_value);
		else
			if (fabs(f_value) > 0.001f)
				sprintf(str_out, "%1.4f", f_value);
			else
				if (f_value == 0.0f)
					sprintf(str_out, "0.0");
				else
					// Too far gone, use scientific notation.
					sprintf(str_out, "%1.2e", f_value);

	// Write the string to the dialog control.
	pwnd->SetDlgItemText(i_item_id, str_out);
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual void OnOK();
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrespassAdvDlg dialog

CTrespassAdvDlg::CTrespassAdvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrespassAdvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrespassAdvDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTrespassAdvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrespassAdvDlg)
	DDX_Control(pDX, IDC_CHECK_DIRECTTEXTUREACCESS, m_DirectTextureAccess);
	DDX_Control(pDX, IDC_CHECK_SUPPORTED, m_Supported);
	DDX_Control(pDX, IDC_SCROLL_SCREEN, m_ScrollScreen);
	DDX_Control(pDX, IDC_SCROLL_CLIP, m_ScrollClip);
	DDX_Control(pDX, IDC_CHECK_SKY, m_CheckSky);
	DDX_Control(pDX, IDC_CHECK_SHAREDBUFFERS, m_SharedBuffers);
	DDX_Control(pDX, IDC_CHECK_SECONDARYCARD, m_SecondaryCard);
	DDX_Control(pDX, IDC_CHECK_REGIONUPLOADS, m_RegionUploads);
	DDX_Control(pDX, IDC_CHECK_HARDWARECACHES, m_HardwareCaches);
	DDX_Control(pDX, IDC_CHECK_FILTERCACHES, m_FilterCaches);
	DDX_Control(pDX, IDC_CHECK_DITHER, m_Dither);
	DDX_Control(pDX, IDC_CHECK_CLIPFOG, m_ClipFog);
	DDX_Control(pDX, IDC_CHECK_CACHEFOG, m_CacheFog);
	DDX_Control(pDX, IDC_CHECK_ALPHATEXTURES, m_AlphaTexture);
	DDX_Control(pDX, IDC_CHECK_ALPHACOLOUR, m_AlphaColour);
	DDX_Control(pDX, IDC_LIST_CARDS, m_ListCards);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTrespassAdvDlg, CDialog)
	//{{AFX_MSG_MAP(CTrespassAdvDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADDCARD, OnButtonAddCard)
	ON_LBN_DBLCLK(IDC_LIST_CARDS, OnDblclkListCards)
	ON_BN_CLICKED(IDC_CHECK_ALPHACOLOUR, OnCheckAlphaColour)
	ON_BN_CLICKED(IDC_CHECK_ALPHATEXTURES, OnCheckAlphaTextures)
	ON_BN_CLICKED(IDC_CHECK_CACHEFOG, OnCheckCacheFog)
	ON_BN_CLICKED(IDC_CHECK_CLIPFOG, OnCheckClipFog)
	ON_BN_CLICKED(IDC_CHECK_DITHER, OnCheckDither)
	ON_BN_CLICKED(IDC_CHECK_FILTERCACHES, OnCheckFilterCaches)
	ON_BN_CLICKED(IDC_CHECK_HARDWARECACHES, OnCheckHardwareCaches)
	ON_BN_CLICKED(IDC_CHECK_REGIONUPLOADS, OnCheckRegionUploads)
	ON_BN_CLICKED(IDC_CHECK_SECONDARYCARD, OnCheckSecondaryCard)
	ON_BN_CLICKED(IDC_CHECK_SHAREDBUFFERS, OnCheckSharedBuffers)
	ON_BN_CLICKED(IDC_CHECK_SKY, OnCheckSky)
	ON_WM_HSCROLL()
	ON_LBN_SELCHANGE(IDC_LIST_CARDS, OnSelchangeListCards)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_CARD, OnButtonRemoveCard)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_SUPPORTED, OnCheckSupported)
	ON_BN_CLICKED(IDC_BUTTON_CARDNOTES, OnButtonCardNotes)
	ON_BN_CLICKED(IDC_BUTTON_CHIPSET, OnButtonChipset)
	ON_BN_CLICKED(IDC_BUTTON_DRIVER, OnButtonDriver)
	ON_BN_CLICKED(IDC_CHECK_DIRECTTEXTUREACCESS, OnCheckDirectTextureAccess)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrespassAdvDlg message handlers

BOOL CTrespassAdvDlg::OnInitDialog()
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	InitializeCards();

	iChangedFile = FALSE;
	bChangedCard = FALSE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTrespassAdvDlg::InitializeCards(BOOL b_load_from_file)
{
	CD3DSettings d3dset;

	if (b_load_from_file)
	{
		if (!d3darD3DSettings.bLoadD3DSettings())
		{
			MessageBox
			(
				"Warning: 'Settings.d3d' file not found. One will be created.",
				"File Warning",
				MB_OK | MB_ICONWARNING
			);
		}
	}
	m_ListCards.ResetContent();
	for (int i = 0; i < d3darD3DSettings.iNumSettings; ++i)
	{
		m_ListCards.AddString(d3darD3DSettings.ad3dsetSettings[i].strCardName);
	}

	// Attempt to select a string.
	char str_cardname[iCARDNAME_LEN + 10];
	OpenKey();
	int i_chars = GetRegString(strVIDEOCARD_NAME, str_cardname, iCARDNAME_LEN, "");
	CloseKey();
	if (i_chars < 1)
		m_ListCards.GetText(0, str_cardname);
	m_ListCards.SelectString(0, str_cardname);

	// Select a card.
	CD3DSettings* pd3dset = d3darD3DSettings.pd3dsetGetD3DSetting(str_cardname);
	if (!pd3dset)
		pd3dset = &d3dset;

	// Set check boxes and scroll bars.
	InitializeControls();
}

void CTrespassAdvDlg::InitializeControls()
{
	CD3DSettings d3dset;

	// Select a card.
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (!pd3dset)
		pd3dset = &d3dset;

	// Set check boxes.
	m_CheckSky.SetCheck(pd3dset->bD3DSky);
	m_SharedBuffers.SetCheck(pd3dset->bSharedSysBuffers);
	m_SecondaryCard.SetCheck(pd3dset->bSecondaryCard);
	m_DirectTextureAccess.SetCheck(pd3dset->bDirectTexture);
	m_RegionUploads.SetCheck(pd3dset->bRegionUploads);
	m_HardwareCaches.SetCheck(pd3dset->bD3DImageCaches);
	m_FilterCaches.SetCheck(pd3dset->bFilterCaches);
	m_Dither.SetCheck(pd3dset->bDither);
	m_ClipFog.SetCheck(pd3dset->bClipFog);
	m_CacheFog.SetCheck(pd3dset->bCacheFog);
	m_AlphaTexture.SetCheck(pd3dset->bAlphaTexture);
	m_AlphaColour.SetCheck(pd3dset->bAlphaColour);
	m_Supported.SetCheck(pd3dset->bSupported);

	m_ScrollScreen.SetScrollRange(0, 200, FALSE);
	m_ScrollScreen.SetScrollPos(int(pd3dset->fAdjustScreen * 100.0f));
	m_ScrollClip.SetScrollRange(0, 200, FALSE);
	m_ScrollClip.SetScrollPos(int(pd3dset->fAdjustClip * 100.0f));

	SetText();
}

void CTrespassAdvDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTrespassAdvDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTrespassAdvDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTrespassAdvDlg::OnButtonAddCard() 
{
	iChangedFile = TRUE;
	bChangedCard = TRUE;

	// Create a new setting.
	CD3DSettings* pd3dset = d3darD3DSettings.pd3dsetAddD3DSetting();

	// Edit the name if required.
	BOOL b_name_found = FALSE;
	do
	{
		CDialogName dlg_name(pd3dset->strCardName);
		dlg_name.DoModal();

		// Make sure the name does not already exist.
		b_name_found = m_ListCards.SelectString(0, pd3dset->strCardName) != LB_ERR;
		if (b_name_found)
		{
			int i_button = MessageBox
			(
				"A card with that name already exists!\nPlease select a new name.",
				"Name Error",
				MB_OKCANCEL | MB_DEFBUTTON1 | MB_ICONHAND
			);
			if (i_button == IDCANCEL)
				return;
		}
	}
	while (b_name_found);

	// Add the string and set the card to the string.
	m_ListCards.AddString(pd3dset->strCardName);
	m_ListCards.SelectString(0, pd3dset->strCardName);

	// Set check boxes and scroll bars.
	InitializeControls();
}

void CAboutDlg::OnOK() 
{	
	CDialog::OnOK();
}

void CTrespassAdvDlg::OnDblclkListCards() 
{
	int i_sel = m_ListCards.GetCurSel();
	if (i_sel == LB_ERR)
		i_sel = 0;
	int i_len = m_ListCards.GetTextLen(i_sel);
	char* str_cardname = new char[i_len + 1];
	m_ListCards.GetText(i_sel, str_cardname);

	// Create a new setting.
	CD3DSettings* pd3dset = d3darD3DSettings.pd3dsetGetD3DSetting(str_cardname);

	if (!pd3dset)
		return;

	// Edit the name if required.
	{
		CDialogName dlg_name(pd3dset->strCardName);
		dlg_name.DoModal();
	}

	if (strcmpi(str_cardname, pd3dset->strCardName))
	{
		// Replace the card name.
		m_ListCards.DeleteString(i_sel);
		m_ListCards.AddString(pd3dset->strCardName);
	}

	m_ListCards.SelectString(0, pd3dset->strCardName);

	delete[] str_cardname;

	// Set check boxes and scroll bars.
	InitializeControls();

	iChangedFile = TRUE;
	bChangedCard = TRUE;
}

void CTrespassAdvDlg::OnCheckAlphaColour() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (!pd3dset)
		return;
	pd3dset->bAlphaColour = m_AlphaColour.GetCheck() != 0;
	iChangedFile = TRUE;
}

void CTrespassAdvDlg::OnCheckAlphaTextures() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bAlphaTexture = m_AlphaTexture.GetCheck() != 0;
	iChangedFile = TRUE;	
}

void CTrespassAdvDlg::OnCheckCacheFog() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bCacheFog = m_CacheFog.GetCheck() != 0;
	iChangedFile = TRUE;
}

void CTrespassAdvDlg::OnCheckClipFog() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bClipFog = m_ClipFog.GetCheck() != 0;
	iChangedFile = TRUE;
}

void CTrespassAdvDlg::OnCheckDither() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bDither = m_Dither.GetCheck() != 0;
	iChangedFile = TRUE;
}

void CTrespassAdvDlg::OnCheckFilterCaches() 
{
	
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bFilterCaches = m_FilterCaches.GetCheck() != 0;
	iChangedFile = TRUE;
}

void CTrespassAdvDlg::OnCheckHardwareCaches() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bD3DImageCaches = m_HardwareCaches.GetCheck() != 0;
	iChangedFile = TRUE;
}

void CTrespassAdvDlg::OnCheckRegionUploads() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bRegionUploads = m_RegionUploads.GetCheck() != 0;
	iChangedFile = TRUE;
}

void CTrespassAdvDlg::OnCheckSecondaryCard() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bSecondaryCard = m_SecondaryCard.GetCheck() != 0;
	iChangedFile = TRUE;
}

void CTrespassAdvDlg::OnCheckSharedBuffers() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bSharedSysBuffers = m_SharedBuffers.GetCheck() != 0;
	iChangedFile = TRUE;
}

void CTrespassAdvDlg::OnCheckSky() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bD3DSky = m_CheckSky.GetCheck() != 0;
	iChangedFile = TRUE;
}

void CTrespassAdvDlg::OnCheckDirectTextureAccess() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bDirectTexture = m_DirectTextureAccess.GetCheck() != 0;
	iChangedFile = TRUE;	
}

void CTrespassAdvDlg::OnCheckSupported() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (pd3dset)
		pd3dset->bSupported = m_Supported.GetCheck() != 0;
	iChangedFile = TRUE;	
}

void CTrespassAdvDlg::OnSelchangeListCards() 
{
	// Set check boxes and scroll bars.
	InitializeControls();
	bChangedCard = TRUE;
}

void CTrespassAdvDlg::OnButtonSave() 
{
	// Write out the file.
	do
	{
		if (d3darD3DSettings.bSaveD3DSettings())
			return;
		int i_button = MessageBox
		(
			"Error: 'Settings.d3d' file could not be written!",
			"File Error",
			MB_RETRYCANCEL | MB_ICONHAND
		);
		if (i_button == IDCANCEL)
			return;
	}
	while (1);
}

int bOk(int i_reval)
{
	if (i_reval == IDOK)
		return 1;
	if (i_reval == IDYES)
		return 1;
	return 0;
}

void CTrespassAdvDlg::OnOK() 
{
	CloseSave();
	d3darD3DSettings.DumpList();
	CDialog::OnOK();
}


//**********************************************************************************************
void MoveScrollbar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int MinPos, MaxPos;

	pScrollBar->GetScrollRange(&MinPos, &MaxPos);

	int ScrollRange = MaxPos - MinPos;
	int FineTune    = 1;//ScrollRange / 100;
	int CoarseTune  = 10;//ScrollRange / 10;
	int Pos         = pScrollBar->GetScrollPos();

	FineTune = (FineTune < 1) ? (1) : (FineTune);

	CoarseTune = (CoarseTune < 1) ? (1) : (CoarseTune);

	switch (nSBCode)
	{
		case SB_LEFT:
			Pos = MinPos;
			break;
		case SB_RIGHT:
			Pos = MaxPos;
			break;
		case SB_PAGELEFT:
			Pos -= CoarseTune;
			if (Pos < MinPos) 
				Pos = MinPos;
			break;
		case SB_LINELEFT:
			Pos -= FineTune;
			if (Pos < MinPos) 
				Pos = MinPos;
			break;
		case SB_PAGERIGHT:
			Pos += CoarseTune;
			if (Pos > MaxPos) 
				Pos = MaxPos;
			break;
		case SB_LINERIGHT:
			Pos += FineTune;
			if (Pos > MaxPos) 
				Pos = MaxPos;
			break;
		case SB_THUMBPOSITION:
			Pos = nPos;
			break;
		case SB_THUMBTRACK:
			//Pos = nPos;
			break;
		default:;
	}
	pScrollBar->SetScrollPos(Pos, TRUE);
}

void CTrespassAdvDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	iChangedFile = TRUE;
	MoveScrollbar(nSBCode, nPos, pScrollBar);

	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (!pd3dset)
		return;

	pd3dset->fAdjustScreen = float(m_ScrollScreen.GetScrollPos()) / 100.0f;
	pd3dset->fAdjustClip   = float(m_ScrollClip.GetScrollPos()) / 100.0f;

	SetText();
}

void CTrespassAdvDlg::SetText()
{
	CD3DSettings d3dset;

	// Select a card.
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (!pd3dset)
		pd3dset = &d3dset;

	SetDlgItemFloat(this, IDC_STATIC_SCREEN, pd3dset->fAdjustScreen, 3);
	SetDlgItemFloat(this, IDC_STATIC_CLIP,   pd3dset->fAdjustClip,   3);
	SetDlgItemText(IDC_STATIC_NOTES, pd3dset->strCardNotes);
	SetDlgItemText(IDC_STATIC_CHIPSET, pd3dset->strChipset);
	SetDlgItemText(IDC_STATIC_DRIVER, pd3dset->strDriver);
}

void CTrespassAdvDlg::OnButtonRemoveCard() 
{
	// Select a card.
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (!pd3dset)
		return;

	iChangedFile = TRUE;
	bChangedCard = TRUE;

	// Remove the setting.
	d3darD3DSettings.RemoveD3DSetting(pd3dset);

	// Reset card list and controls.
	InitializeCards(FALSE);
}

void CTrespassAdvDlg::OnClose() 
{
	CloseSave();
	d3darD3DSettings.DumpList();
	CDialog::OnClose();
}

void CTrespassAdvDlg::CloseSave()
{
	if (iChangedFile || bChangedCard)
	{
		CD3DSettings d3dset;

		// Select a card.
		CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
		if (!pd3dset)
			pd3dset = &d3dset;

		char str[512];
		strcpy(str, "Do you wish to change Trespasser settings to the currently\nselected card: '");
		strcat(str, pd3dset->strCardName);
		strcat(str, "'?");
		if (bOk(MessageBox(str, "Set Trespasser Card Settings", MB_YESNO | MB_ICONQUESTION)))
			OnButtonRegistry();
	}
	if (iChangedFile)
	{
		if (bOk(MessageBox("Do you wish to save the card config file?",
			"Save Config File", MB_YESNO | MB_ICONQUESTION)))
			OnButtonSave();
	}
}


void CTrespassAdvDlg::OnButtonRegistry() 
{
	CD3DSettings d3dset;

	// Select a card.
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (!pd3dset)
		pd3dset = &d3dset;

	bFindDevices(&ddDevice, pd3dset->bSecondaryCard);
	bFindD3D(ddDevice.guidID, &d3dDevice);

	// Store the registry settings.
	OpenKey();
	pd3dset->SaveToRegistry();
	bSetInitFlag(TRUE);
	bSetSystemMem(FALSE);
	SetD3D(TRUE);

	// Write out the direct draw driver selection.
	WriteDDDescription(ddDevice.strName, ddDevice.strDescription);
	WriteDDGUID(ddDevice.guidID);

	// Write out the resolution.
	SetDimensions(640, 480);

	// Write out the D3D driver selection.
	WriteD3DDescription(d3dDevice.strName, d3dDevice.strDescription);
	WriteD3DGUID(d3dDevice.guidID);

	CloseKey();
}

void CTrespassAdvDlg::OnButtonCardNotes() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (!pd3dset)
		return;

	CDialogName dlg_name(pd3dset->strCardNotes);
	dlg_name.DoModal();

	SetText();
}

void CTrespassAdvDlg::OnButtonChipset() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (!pd3dset)
		return;

	char str_buffer[256];
	memset(str_buffer, 0, 256);
	strcpy(str_buffer, pd3dset->strChipset);

	CDialogName dlg_name(str_buffer);
	dlg_name.DoModal();

	for (int i = 0; i < iCHIPSET_LEN; ++i)
		pd3dset->strChipset[i] = str_buffer[i];

	SetText();
}

void CTrespassAdvDlg::OnButtonDriver() 
{
	CD3DSettings* pd3dset = pd3dsetGetCard(m_ListCards);
	if (!pd3dset)
		return;

	char str_buffer[256];
	memset(str_buffer, 0, 256);
	strcpy(str_buffer, pd3dset->strDriver);

	CDialogName dlg_name(str_buffer);
	dlg_name.DoModal();

	for (int i = 0; i < iCHIPSET_LEN; ++i)
		pd3dset->strDriver[i] = str_buffer[i];

	SetText();
}
