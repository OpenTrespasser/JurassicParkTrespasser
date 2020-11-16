//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       dlgrender.cpp
//
//  Contents:   User Dialog code to change the rendering statistics.
//
//  Classes:
//
//  Functions:
//
//  History:    02-Jan-98   SHernd   Created
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "supportfn.hpp"
#include "resource.h"
#include "main.h"
#include "uiwnd.h"
#include "uidlgs.h"
#include "tpassglobals.h"
#include "rasterdc.hpp"
#include "../Lib/Sys/reg.h"
#include "../Lib/Sys/RegInit.hpp"
#include "DDDevice.hpp"
#include "Lib/Std/MemLimits.hpp"


//
// Constants.
//

// Texture resolution constants.
const char* strLow    = "Low";
const char* strMedium = "Medium";
const char* strHigh   = "High";


extern HINSTANCE    g_hInst;
extern HWND		    g_hwnd;

int         iNumResolutions = 0;	// Number of DD devices.
SResolution arResolutions[20];	    // More DD devices than can exist.


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CConfigureWnd::CConfigureWnd()
{
}


CConfigureWnd::~CConfigureWnd()
{
	if (penumdevDevices)
	{
		delete penumdevDevices;
		penumdevDevices = 0;
	}
}


BOOL CConfigureWnd::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	// Initialize the enumeration.
	if (!penumdevDevices)
		penumdevDevices = new CEnumerateDevices();

	//
	// Determine if the dialog is worth running. If there is only one device on the system,
	// it could only be the Trespasser software device.
	//
	if (penumdevDevices->iGetNumDevices() < 2)
	{
        ErrorDlg(NULL, IDS_ERR_NO_ACCELERATORS);

        delete penumdevDevices;
		penumdevDevices = 0;
		bSetInitFlag(TRUE);
		CMultiDlg::OnOK();
		return FALSE;
	}

    CMultiDlg::OnInitDialog(hwnd, hwndFocus, lParam);

    m_hwndResolutions = GetDlgItem(m_hwnd, IDC_COMBO_RESOLUTION);
    m_hwndTextureSizes = GetDlgItem(m_hwnd, IDC_COMBO_TEXTURESIZE);
    m_hwndList = GetDlgItem(m_hwnd, IDC_LIST_CARD);

	InitializeCardSelection();
	
    return TRUE;
}


void CConfigureWnd::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	// Check the water button.
    if (id == IDC_CHECK_HARDWAREWATER)
	{
	}

	// Check the water button.
    if (id == IDHELP)
	{
		char str_cmd[512];
		char str_dir[256];

		// Create the command line.
		GetRegString(REG_KEY_INSTALLED_DIR, str_dir, sizeof(str_dir), "");
		strcpy(str_cmd, "Notepad.exe ");
		strcat(str_cmd, str_dir);
		strcat(str_cmd, "Readme.txt");

		// Execute the command line.
		int i_exec = WinExec(str_cmd, SW_SHOWDEFAULT);

		// Any return value 31 or less is an error.
		if (i_exec <= 31)
		{
            MsgDlg(NULL, MB_OK, IDS_TITLE, IDS_ERR_DISPLAY_README);
		}
	}

	if (codeNotify == LBN_SELCHANGE && hwndCtl == m_hwndList)
    {
		OnSelchangeListCard();
		return;
    }

	CMultiDlg::OnCommand(hwnd, id, hwndCtl, codeNotify);
}

static bool bMatchResolution(int i_width, int i_height)
{
	SResolution res(i_width, i_height);

	for (int i = 0; i < iNumResolutions; ++i)
	{
		if (arResolutions[i] == res)
			return true;
	}
	return false;
}


void CConfigureWnd::InitializeResolutions()
{
    int     i_width;
    int     i_height;
	char    str_desc[32];

	// Clear current selections.
    ComboBox_ResetContent(m_hwndResolutions);

	// Get the device currently selected.
	GUID guid = penumdevDevices->devGetSelectedDevice().ddDevice.guidID;

	// Get a list of supported resolutions for the guid.
	bFindResolutions(guid, arResolutions, iNumResolutions);

	// Add these devices to the combo box used for video modes.
	for (int i = 0; i < iNumResolutions; ++i)
	{
		wsprintf(str_desc, "%ld x %ld", arResolutions[i].iWidth, arResolutions[i].iHeight);
        ComboBox_AddString(m_hwndResolutions, str_desc);
	}

	// Try to select the preset resolution; if it is not available some other resolutions.
	bGetDimensions(i_width, i_height);
	if (!bMatchResolution(i_width, i_height))
	{
		// Try 400x300.
		i_width  = 400;
		i_height = 300;
		if (!bMatchResolution(i_width, i_height))
		{
			// Try 512x384.
			i_width  = 512;
			i_height = 384;
			if (!bMatchResolution(i_width, i_height))
			{
				// Try 640x480.
				i_width  = 640;
				i_height = 480;
				if (!bMatchResolution(i_width, i_height))
				{
					// Use first available resolution.
					i_width  = arResolutions[0].iWidth;
					i_height = arResolutions[0].iHeight;
				}
			}
		}
	}

	wsprintf(str_desc, "%ld x %ld", i_width, i_height);

    // Select this in the Adapter listing
    int i = ComboBox_SelectString(m_hwndResolutions, -1, str_desc);
    if (i == CB_ERR)
    {
	    // Set the first device as the default.
        ComboBox_SetCurSel(m_hwndResolutions, 0);
    }
}

void CConfigureWnd::InitializeTextureSizes()
{
	// Clear current selections.
    ComboBox_ResetContent(m_hwndTextureSizes);

	bool b_disable_large = u4TotalPhysicalMemory() < 40 * 1024 * 1024;

	// Add the available sizes.
    ComboBox_AddString(m_hwndTextureSizes, strLow);
    ComboBox_AddString(m_hwndTextureSizes, strMedium);
	if (!b_disable_large)
		ComboBox_AddString(m_hwndTextureSizes, strHigh);

	// Get the recommended maximum texture dimension.
	int i_dim = iGetRecommendedTextureMaxDim();
	if (b_disable_large && i_dim > 64)
		i_dim = 64;
	if (i_dim > 64 && penumdevDevices->devGetSelectedDevice().d3dDevice.bSmallMemory)
		i_dim = 64;

    // Select the texture size.
	int i_sel = 2;
	switch (i_dim)
	{
		case 32:
			i_sel = 0;
			break;
		case 64:
			i_sel = 1;
			break;
	}
    ComboBox_SetCurSel(m_hwndTextureSizes, i_sel);
}


void CConfigureWnd::InitializeCardSelection()
{
	char str[2048];

	// Clear the box.
    ListBox_ResetContent(m_hwndList);

	//
	// List the enumerated devices.
	//
	for (int i = 0; i < penumdevDevices->iGetNumDevices(); ++i)
	{
		penumdevDevices->GetTitle(i, str);
		ListBox_AddString(m_hwndList, str);
	}

	// Attempt to select a string.
	ListBox_SelectString(m_hwndList, 0, str);

	// Select card information.
	OnSelchangeListCard();
}

void CConfigureWnd::OnSelchangeListCard() 
{
	// Select the device from the list.
	{
		int i_sel = ListBox_GetCurSel(m_hwndList);
		if (i_sel == LB_ERR)
			i_sel = 0;
		int i_len = ListBox_GetTextLen(m_hwndList, i_sel);
		char* str_cardname = new char[i_len + 1];
		ListBox_GetText(m_hwndList, i_sel, str_cardname);
		penumdevDevices->SelectDevice(str_cardname);
		delete[] str_cardname;
	}

	// Output 3D driver information.
	SD3DDevice d3ddev = penumdevDevices->devGetSelectedDevice().d3dDevice;
	SetDlgItemText(m_hwnd, IDC_STATIC_VIDEODRIVER, d3ddev.strName);
	SetDlgItemText(m_hwnd, IDC_STATIC_D3D, d3ddev.strDescription);

	// If the card is a Voodoo 2, turn page management off.
	switch (d3ddev.evcVideoCard)
	{
		case evcPermedia2:
		case evcVoodoo2:
			SetPageManaged(false);
			break;

		default:;
		// Do nothing.
	}

	// Set the preferred state for water.
	HWND hwnd_water_check = GetDlgItem(m_hwnd, IDC_CHECK_HARDWAREWATER);
	if (d3ddev.bModulatedAlpha)
	{
		EnableWindow(hwnd_water_check, true);
		CheckDlgButton(m_hwnd, IDC_CHECK_HARDWAREWATER, BST_CHECKED);
	}
	else
	{
		EnableWindow(hwnd_water_check, false);
		CheckDlgButton(m_hwnd, IDC_CHECK_HARDWAREWATER, BST_UNCHECKED);
	}

	// Set the preferred state for triple buffering.
	HWND hwnd_triple_check = GetDlgItem(m_hwnd, IDC_CHECK_TRIPLEBUFFER);
	if (bGetTripleBuffer())
	{
		CheckDlgButton(m_hwnd, IDC_CHECK_TRIPLEBUFFER, BST_CHECKED);
	}
	else
	{
		CheckDlgButton(m_hwnd, IDC_CHECK_TRIPLEBUFFER, BST_UNCHECKED);
	}

	// Set the preferred state for page managed textures.
	HWND hwnd_page_check = GetDlgItem(m_hwnd, IDC_CHECK_PAGEMANAGED);
	if (bGetPageManaged())
	{
		CheckDlgButton(m_hwnd, IDC_CHECK_PAGEMANAGED, BST_CHECKED);
	}
	else
	{
		CheckDlgButton(m_hwnd, IDC_CHECK_PAGEMANAGED, BST_UNCHECKED);
	}

	// Set up the resolution combo box.
	InitializeResolutions();
	InitializeTextureSizes();

	// Set up dither check box.
	CheckDlgButton(m_hwnd, IDC_CHECK_DITHER, (bGetDither()) ? (BST_CHECKED) : (BST_UNCHECKED));

	// Grey out stuff.
	HWND hwnd_dither_check = GetDlgItem(m_hwnd, IDC_CHECK_DITHER);
	HWND hwnd_texres_check = GetDlgItem(m_hwnd, IDC_COMBO_TEXTURESIZE);
	if (penumdevDevices->bSoftwareSelected())
	{
		EnableWindow(hwnd_triple_check, false);
		EnableWindow(hwnd_dither_check, false);
		EnableWindow(hwnd_texres_check, false);
		EnableWindow(hwnd_page_check,   false);
	}
	else
	{
		EnableWindow(hwnd_triple_check, true);
		EnableWindow(hwnd_dither_check, true);
		EnableWindow(hwnd_texres_check, true);
		EnableWindow(hwnd_page_check,   true);
	}
}


void CConfigureWnd::OnCancel()
{
	// Remove the device enumeration object.
	delete penumdevDevices;
	penumdevDevices = 0;

	bSetInitFlag(TRUE);
	CMultiDlg::OnCancel();
}

void CConfigureWnd::OnOK() 
{
	SD3DDevice d3ddev = penumdevDevices->devGetSelectedDevice().d3dDevice;

	//
	// Write the resolution currently selected to the registry.
	//
	int i_resolution = ComboBox_GetCurSel(m_hwndResolutions);
	SetDimensions(arResolutions[i_resolution].iWidth, arResolutions[i_resolution].iHeight);

	int i_sel = ComboBox_GetCurSel(m_hwndTextureSizes);
	int i_recommended_max = 128;
	switch (i_sel)
	{
		case 0:
			i_recommended_max = 32;
			break;
		case 1:
			i_recommended_max = 64;
			break;
	}
	SetRecommendedTextureMaxDim(i_recommended_max);

	// Write D3D settings to the registry.
	penumdevDevices->WriteSelectedDeviceToRegistry();

	// Save the hardware water flag to the registery.
	SetRegValue(strHARDWARE_WATER, d3ddev.bModulatedAlpha && IsDlgButtonChecked(m_hwnd, IDC_CHECK_HARDWAREWATER));

	// Set the page manager flag to the registry.
	SetPageManaged(IsDlgButtonChecked(m_hwnd, IDC_CHECK_PAGEMANAGED));

	// Set the triple buffer flag.
	SetTripleBuffer(IsDlgButtonChecked(m_hwnd, IDC_CHECK_TRIPLEBUFFER));

	// Set the dither flag.
	SetDither(IsDlgButtonChecked(m_hwnd, IDC_CHECK_DITHER));

	// Close the registry and exit.
    CMultiDlg::OnOK();

	// Remove the device enumeration object.
	delete penumdevDevices;
	penumdevDevices = 0;

	//exit(0);
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CRenderWnd::CRenderWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
    m_guidDefAdapter = ReadDDGUID();
    m_guidAdapter = m_guidDefAdapter;

    m_guidDefD3D = ReadD3DGUID();
    m_guidD3D = m_guidDefD3D;

    bGetDimensions(m_iDefWidth, m_iDefHeight);
    m_iWidth = m_iDefWidth;
    m_iHeight = m_iDefHeight;
	Video::SetToValidMode(m_iWidth, m_iHeight);
	SetDimensions(m_iWidth, m_iHeight);

    bFindResolutions(m_guidAdapter, arResolutions, iNumResolutions);
}

CRenderWnd::~CRenderWnd()
{
}


BOOL CRenderWnd::OnCreate()
{
    CUISlider *     pslider;
    CUITextbox *    ptext;
    char            sz[200];
    //CUICheckbox *   pcheck;

    if (!CUIDlg::OnCreate())
    {
        TraceError(("CRenderWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    AddResolutionsToList();
    SelectCurrentResolution();

    //
    // Quality Settings
    //
    pslider = (CUISlider *)GetUICtrl(1006);
    Assert(pslider != NULL);

    m_iDefQuality = g_CTPassGlobals.GetRenderQuality();
    pslider->SetCurrUnit(m_iDefQuality);

    //
    // Gamma Settings
    //
    pslider = (CUISlider *)GetUICtrl(1008);
    Assert(pslider != NULL);
	
    m_iDefGamma = g_CTPassGlobals.GetGamma();
    pslider->SetCurrUnit(m_iDefGamma);

    //
    // Video Driver Name
    //
    ptext = (CUITextbox *)GetUICtrl(1011);
    GetRegString(strD3D_TITLE, sz, sizeof(sz), "");
    if (strlen(sz) == 0)
    {
        LoadString(g_hInst, IDS_SOFTWARE_DRIVER, sz, sizeof(sz));
    }
    ptext->SetText(sz);

    //
    // Screen Size
    //
    pslider = (CUISlider *)GetUICtrl(1012);
    Assert(pslider != NULL);
	
    m_iDefScreenSize = (VIEWPORT_INCREMENTS - 1) - 
                       GetRegValue(REG_KEY_VIEWPORT_X, DEFAULT_VIEWPORT_X);
    if (m_iDefScreenSize < 0)
    {
        m_iDefScreenSize = 0;
    }
    else if (m_iDefScreenSize >= VIEWPORT_INCREMENTS)
    {
        m_iDefScreenSize = VIEWPORT_INCREMENTS - 1;
    }

    pslider->SetCurrUnit(m_iDefScreenSize);

	/*
    pcheck = (CUICheckbox *)GetUICtrl(1003);
    Assert(pcheck != NULL);
    pcheck->SetDown(m_bUseHardware);
	*/

    CenterUIWindow(this);

    return TRUE;
}


void CRenderWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "render.ddf");
}


void CRenderWnd::EnterVideoRes()
{
}


void CRenderWnd::UIButtonUp(CUIButton * pbutton)
{
    switch (pbutton->GetID())
    {
        // Cancel
        case 1000:
            EndUIWnd(0);
            break;

        // OK
        case 1001:
            {
                OnOk();
            }
            break;

        // Select new driver.
        case 1003:
            {
                OnSelectNewDriver();
            }
            break;
    }
}

void CRenderWnd::OnOk()
{
    CUIListbox *    plist;
    CUISlider *     pslider;
    //CUICheckbox *   pcheck;
    int             i;
    BOOL            iChanges = 0;
    BOOL            bFullReset = FALSE;

    // Get The current Resolution
    plist = (CUIListbox *)GetUICtrl(1002);
    Assert(plist != NULL);
    i = plist->GetCurrSel();
    m_iWidth = arResolutions[i].iWidth;
    m_iHeight = arResolutions[i].iHeight;

    if ((m_iWidth != m_iDefWidth) ||
        (m_iHeight != m_iDefHeight))
    {
        iChanges++;
        bFullReset = TRUE;
    }

    //
    // Quality Settings
    //
    pslider = (CUISlider *)GetUICtrl(1006);
    Assert(pslider != NULL);

    i = pslider->GetCurrUnit();
    if (m_iDefQuality != i)
    {
        iChanges++;
        g_CTPassGlobals.SetRenderQuality(i);
    }

    //
    // Gamma Settings
    //
    pslider = (CUISlider *)GetUICtrl(1008);
    Assert(pslider != NULL);

    i = pslider->GetCurrUnit();
    if (m_iDefGamma != i)
    {
        iChanges++;
        g_CTPassGlobals.SetGamma(i);
        g_CTPassGlobals.InitGamma();
    }

    //
    // Screen Size
    //
    pslider = (CUISlider *)GetUICtrl(1012);
    Assert(pslider != NULL);

    i = pslider->GetCurrUnit();
    if (m_iDefScreenSize != i)
    {
        iChanges++;
        i = (VIEWPORT_INCREMENTS - 1) - i;
        SetRegValue(REG_KEY_VIEWPORT_X, i);
        SetRegValue(REG_KEY_VIEWPORT_Y, i);
    }

    // Save out all the settings
    Persist();

#if 0
	if (!bGetInitFlag())
	{
		//
		// BUGBUG!
		//
		// To do:
		//		Fix this problem.
		//
		//MessageBox(0, "Due to a minor bug, you must unfortunately fully exit the application!",
			       //"Sorry, Minor Bug", MB_OK | MB_ICONHAND);
		//exit(0);
		EndUIWnd(3);
	}
	else
#endif        
        if (bFullReset)
    {
        EndUIWnd(2);
    }
    else if (iChanges > 0)
    {
        EndUIWnd(1);
    }
    else
    {
        EndUIWnd(0);
    }
}


void CRenderWnd::Persist()
{
    if (m_iHeight != m_iDefHeight || m_iWidth != m_iDefWidth)
    {
        SetDimensions(m_iWidth, m_iHeight);
    }
}


void CRenderWnd::AddAdaptersToList()
{
}


void CRenderWnd::SelectCurrentAdapter()
{
}


void CRenderWnd::ResetListItems()
{
}


void CRenderWnd::ChangeAdapter()
{
}


void CRenderWnd::AddResolutionsToList()
{
	char            szDesc[32];
    int             i;
    CUIListbox *    plist;

	// Clear current selections.
    plist = (CUIListbox *)GetUICtrl(1002);

	// Add these devices to the combo box used for video modes.
	for (i = 0; i < iNumResolutions; ++i)
	{
        if (arResolutions[i].iRefreshHz)
        {
            wsprintf(szDesc, 
                     "%ld x %ld at %ld Hz", 
                     arResolutions[i].iWidth,
			         arResolutions[i].iHeight,
                     arResolutions[i].iRefreshHz);
        }
        else
        {
            wsprintf(szDesc, 
                     "%ld x %ld", 
                     arResolutions[i].iWidth,
			         arResolutions[i].iHeight);
        }

        plist->AddItem(szDesc, 0, -1, 0);
	}
}


void CRenderWnd::SelectCurrentResolution()
{
    int             i;
    char            sz[50];
    CUIListbox *    plistbox;

    // Read in the current game dimensions
    wsprintf(sz, "%ld x %ld", m_iWidth, m_iHeight);

    plistbox = (CUIListbox *)GetUICtrl(1002);

    i = plistbox->FindItem(sz);
    if (i == -1)
    {
        // This Resolution mode doesn't exist on the currently
        // selected device.
        
        // try 640 x 480
        wsprintf(sz, "%ld x %ld", DEFAULT_SIZE_WIDTH, DEFAULT_SIZE_HEIGHT);
        i = plistbox->FindItem(sz);
        if (i == -1)
        {
            // Man in the video card doesn't support 640x480 then we
            // are completely fucked.
            
            // BUGBUG: throw up an error dialog here
            return;
        }

        // BUGBUG:  We may want to have this saved out to the user's
        // BUGBUG:  registry potentially.
    }

    plistbox->SetCurrSel(i);
}


void CRenderWnd::AddD3DToList()
{
}


void CRenderWnd::SelectCurrentD3D()
{
}


void CRenderWnd::OnSelectNewDriver()
{
	// Set flag to cause the card selection dialog to pop up on startup.
    bSetInitFlag(false);

    // Save out all the settings
    Persist();

	//
	// Save out the current game in a temporary file if a game is loaded.
	//
	// To do:
	//		Get the correct way of determining if a level is loaded.
	//
	if (g_CTPassGlobals.bInGame)
	{
		//
		// Note that the scene is saved out here instead of a savegame! The earlier system
		// prevented the automatic restoring of a game after selecting a new video driver.
		//
		if (g_CTPassGlobals.bSaveScene(strTEMP_GAME))
			SetAutoLoad(TRUE);
	}
    else
    {
        SetAutoLoad(FALSE);
    }

	// Kill Direct3D.
	//d3dDriver.Uninitialize();

	// Reboot the application.
	EndUIWnd(3);
}