//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1996.
//
//  File:       gdidlgs.cpp
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    04-Dec-97   SHernd   Created
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "tpassglobals.h"
#include "supportfn.hpp"
#include "resource.h"
#include "main.h"
#include "uiwnd.h"
#include "rasterdc.hpp"
#include "..\Lib\Sys\reg.h"
#include "..\lib\sys\reginit.hpp"
#include "dddevice.hpp"
#include "winctrls.h"
#include "gdidlgs.h"
#include "dlgrender.h"
#include "grab.hpp"
#include "..\lib\loader\savefile.hpp"
#include "..\Lib\GeomDBase\LightShape.hpp"

extern HINSTANCE    g_hInst;
extern HWND		    g_hwnd;
extern CMainWnd *   g_pMainWnd;
extern CAudio *     pca_audio;


void LoadThumbNail(HWND hwndParent, HDC hdcSrc, char *szName, int nID);

//
// Default strength for non-ambient lights added.  
// You can have multiple lights of maximum strength, as they use fCombine to combine.
//
const TLightVal	lvDEFAULT = 1.0;

rptr<CMesh>     pmshLightDir;



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



void SetupButtonImage(HWND hwnd, HWND hwndButton, LPCSTR pszImage)
{
    CRasterMem *     praster;
    COwnerButton *  pbutton;

    praster = (CRasterMem*)ReadAndConvertBMP((LPSTR)pszImage);
    pbutton = new COwnerButton();
    pbutton->Subclass(hwndButton);
    pbutton->Init(praster);
}



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CGDIMain::CGDIMain()
{
    int     i;

    m_padbMenu = NULL;
    m_psamBkgnd = NULL;

    for (i = 0; i < 14; i++)
    {
        m_apsamRandoms[i] = NULL;
    }

    m_uiRandom = (UINT)-1;
}


CGDIMain::~CGDIMain()
{
    int     i;

    delete m_psamBkgnd;

    for (i = 0; i < 14; i++)
    {
        delete m_apsamRandoms[i];
    }

    delete m_padbMenu;

    if (m_uiRandom != (UINT)-1)
    {
        KillTimer(m_hwnd, m_uiRandom);
    }
}


BOOL CGDIMain::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    char        sz[50];
    RECT        rc;
    HWND        hwndCtl;
    int         i;
    char        szFile[_MAX_PATH];
    char        szSamples[14][25] =
    {
        "DINO - TREX DIST MISC",
        "DINO - TREX DIST A",
        "DINO - TREX DIST B",
        "DINO - RAPT DIST A",
        "DINO - RAPT DIST B",
        "DINO - RAPT DIST C",
        "DINO - TREX FOOT",
        "BIRD 01",
        "BIRD 02",
        "BIRD 03",
        "BIRD 04",
        "BIRD 05",
        "BIRD 06",
        "BIRD 07",
    };

    m_pRaster = (CRasterDC*)ReadAndConvertBMP("menu\\ms_bkgnd.bmp", true, g_hwnd);

    // Setup the dialog to be "full screen"
    MoveWindow(m_hwnd, 0, 0, 640, 480, FALSE);

    CenterWindow(hwnd, GetParent(hwnd));

    CGDIDlgBase::OnInitDialog(hwnd, hwndFocus, lParam);

    SetupButtonImage(m_hwnd, GetDlgItem(m_hwnd, IDC_QUIT), "menu\\ms_quit.bmp");
    SetupButtonImage(m_hwnd, GetDlgItem(m_hwnd, IDC_LOADGAME), "menu\\ms_load.bmp");
    SetupButtonImage(m_hwnd, GetDlgItem(m_hwnd, IDC_OPTIONS), "menu\\ms_options.bmp");
    SetupButtonImage(m_hwnd, GetDlgItem(m_hwnd, IDC_CREDITS), "menu\\ms_credits.bmp");
    SetupButtonImage(m_hwnd, GetDlgItem(m_hwnd, IDC_NEWGAME), "menu\\ms_new.bmp");

    GetRegString(REG_KEY_PID, sz, sizeof(sz), "Unlicenced Copy");
    hwndCtl = GetDlgItem(m_hwnd, IDC_STATIC_PID);
    SetWindowText(hwndCtl, sz);
    GetWindowRect(hwndCtl, &rc);
    rc.bottom = rc.bottom - rc.top;
    rc.right = rc.right - rc.left;
    rc.left = 320 - (rc.right / 2);
    MoveWindow(hwndCtl, rc.left, 480 - rc.bottom, rc.right, rc.bottom, FALSE);

    // BUGBUG:  Hack to get data drive location
    GetRegString(REG_KEY_INSTALLED_DIR, szFile, sizeof(szFile), "");
    if (szFile[strlen(szFile) - 1] != '\\')
    {
        strcat(szFile, "\\");
    }

    strcat(szFile, "menu.tpa");

    m_padbMenu = new CAudioDatabase(szFile);

    m_psamBkgnd = CAudio::psamCreateSample(sndhndHashIdentifier("OPTIONS - MAIN LOOP"),
                                   m_padbMenu,
                                   AU_CREATE_STREAM | AU_CREATE_STEREO);

    for (i = 0; i < 14; i++)
    {
        m_apsamRandoms[i] = CAudio::psamCreateSample(sndhndHashIdentifier(szSamples[i]),
                                             m_padbMenu,
                                             AU_CREATE_STATIC | AU_CREATE_STEREO);
    }

    m_psamBkgnd->bPlay(AU_PLAY_LOOPED);

    m_uiRandom = SetTimer(m_hwnd, IDTIMER_MAINRANDOM, 5000, NULL);

    return TRUE;
}


void CGDIMain::OnTimer(HWND hwnd, UINT id)
{
    int     i;

    if (id != IDTIMER_MAINRANDOM)
    {
        return;
    }

    i = rand() % 14;

    if (!m_apsamRandoms[i])
    {
        return;
    }

    m_apsamRandoms[i]->bPlay(AU_PLAY_ONCE);
}


void CGDIMain::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    int         i;

    switch (id)
    {
        // Stub this out so the ESC key won't be processed as a close
        case IDCANCEL:
        case IDOK:
            break;

        default:
            CGDIDlgBase::OnCommand(hwnd, id, hwndCtl, codeNotify);
            break;

        case IDC_NEWGAME:
            {
                CGDINewGameDlg  dlg;

                i = GDIDialogHandler(NULL, 
                                     &dlg, 
                                     MAKEINTRESOURCE(IDD_NEWGAME),
                                     m_hwnd);
                if (i == -1)
                {
                    EndDialog(m_hwnd, (DWORD)2);
                }
            }
            break;

        case IDC_CREDITS:
            break;

        case IDC_OPTIONS:
            {
                COptionsDlg     dlg;

                GDIDialogHandler(NULL, 
                                 &dlg, 
                                 MAKEINTRESOURCE(IDD_OPTIONS),
                                 m_hwnd);
            }
            break;

        case IDC_LOADGAME:
            {
                CLoadGameDlg    dlg;

                i = GDIDialogHandler(NULL, 
                                 &dlg, 
                                 MAKEINTRESOURCE(IDD_LOADGAME),
                                 m_hwnd);
                if (i == -1)
                {
                    EndDialog(m_hwnd, (DWORD)2);
                }

            }
            break;

        case IDC_QUIT:
            EndDialog(m_hwnd, -1);
            break;
    }
}



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CControlsDlg::CControlsDlg()
{
}


CControlsDlg::~CControlsDlg()
{
}

BOOL CControlsDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    MoveWindow(m_hwnd, 0, 0, 320, 200, FALSE);

    CenterWindow(hwnd, GetParent(hwnd));

    return CGDIDlgBase::OnInitDialog(hwnd, hwndFocus, lParam);
}


void CControlsDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    CGDIDlgBase::OnCommand(hwnd, id, hwndCtl, codeNotify);
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CSoundDlg::CSoundDlg()
{
}


CSoundDlg::~CSoundDlg()
{
}


BOOL CSoundDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    UINT    ui;
    HWND    hwndTrack;

    MoveWindow(m_hwnd, 0, 0, 320, 200, FALSE);

    CenterWindow(hwnd, GetParent(hwnd));

    CGDIDlgBase::OnInitDialog(hwnd, hwndFocus, lParam);

    ui = GetRegValue(REG_KEY_AUDIO_LEVEL, DEFAULT_AUDIO_LEVEL);
    hwndTrack = GetDlgItem(hwnd, IDC_AUDIOLEVEL);
    SendMessage(hwndTrack, TBM_SETRANGE, FALSE, MAKELONG(1, 10));
    SendMessage(hwndTrack, TBM_SETPOS, TRUE, (LPARAM)ui);

    ui = GetRegValue(REG_KEY_AUDIO_EFFECT, DEFAULT_AUDIO_EFFECT) ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(hwnd, IDC_EFFECTS, ui);
    ui = GetRegValue(REG_KEY_AUDIO_AMBIENT, DEFAULT_AUDIO_AMBIENT) ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(hwnd, IDC_AMBIENT, ui);
    ui = GetRegValue(REG_KEY_AUDIO_VOICEOVER, DEFAULT_AUDIO_VOICEOVER) ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(hwnd, IDC_VOICEOVER, ui);
    ui = GetRegValue(REG_KEY_AUDIO_MUSIC, DEFAULT_AUDIO_MUSIC) ? BST_CHECKED : BST_UNCHECKED;
    CheckDlgButton(hwnd, IDC_MUSIC, ui);

    return TRUE;
}


void CSoundDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    CGDIDlgBase::OnCommand(hwnd, id, hwndCtl, codeNotify);
}


void CSoundDlg::OnOK()
{
    UINT        ui;

    ui = SendMessage(GetDlgItem(m_hwnd, IDC_AUDIOLEVEL), TBM_GETPOS, 0, 0);
    SetRegValue(REG_KEY_AUDIO_LEVEL, ui);

    ui = IsDlgButtonChecked(m_hwnd, IDC_EFFECTS) == BST_CHECKED;
    SetRegValue(REG_KEY_AUDIO_EFFECT, ui);

    ui = IsDlgButtonChecked(m_hwnd, IDC_AMBIENT) & BST_CHECKED;
    SetRegValue(REG_KEY_AUDIO_AMBIENT, ui);

    ui = IsDlgButtonChecked(m_hwnd, IDC_VOICEOVER) & BST_CHECKED;
    SetRegValue(REG_KEY_AUDIO_VOICEOVER, ui);

    ui = IsDlgButtonChecked(m_hwnd, IDC_MUSIC) & BST_CHECKED;
    SetRegValue(REG_KEY_AUDIO_MUSIC, ui);

    RefreshAudioSettings();

    CGDIDlgBase::OnOK();
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CHelpDlg::CHelpDlg()
{
}


CHelpDlg::~CHelpDlg()
{
}


BOOL CHelpDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    MoveWindow(m_hwnd, 0, 0, 320, 200, FALSE);

    CenterWindow(hwnd, GetParent(hwnd));

    return CGDIDlgBase::OnInitDialog(hwnd, hwndFocus, lParam);
}


void CHelpDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    CGDIDlgBase::OnCommand(hwnd, id, hwndCtl, codeNotify);
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CSaveGameDlg::CSaveGameDlg()
{
}


CSaveGameDlg::~CSaveGameDlg()
{
}


BOOL CSaveGameDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HWND            hwndList;
    HANDLE          hfind;
    WIN32_FIND_DATA finddata;
    char            szFile[_MAX_PATH];

    GetRegString(REG_KEY_INSTALLED_DIR, szFile, sizeof(szFile), "");
    if (szFile[strlen(szFile)] != '\\')
    {
        strcat(szFile, "\\");
    }
    strcat(szFile, "*.scn");

    hwndList = GetDlgItem(hwnd, IDC_LISTSAVE);

    hfind = FindFirstFile(szFile, &finddata);
    if (hfind != INVALID_HANDLE_VALUE)
    {
        do
        {
            ListBox_AddString(hwndList, finddata.cFileName);
        }
        while (FindNextFile(hfind, &finddata));

        FindClose(hfind);
	}

    ListBox_SetItemHeight(hwndList, 0, 16);
	ListBox_SetHorizontalExtent(hwndList, 300);

    EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);

    CenterWindow(hwnd, GetParent(hwnd));

    return CGDIDlgBase::OnInitDialog(hwnd, hwndFocus, lParam);
}

void CSaveGameDlg::SaveLevel(LPCSTR pszName)
{
    char    szFile[_MAX_PATH], szFileSCN[_MAX_PATH];
	SnapDIB	ScreenGrab;
	char	* ptrExt;

	// Copy screen to Dib
	ScreenGrab.GrabGameScreen();
	// Save to file

    // BUGBUG:  Hack to get data drive location
    GetRegString(REG_KEY_INSTALLED_DIR, szFileSCN, sizeof(szFileSCN), "");
    if (szFileSCN[strlen(szFileSCN)] != '\\')
    {
        strcat(szFileSCN, "\\");
    }
    strcat(szFileSCN, pszName);
	strcpy(szFile, szFileSCN);

	// replace the .scn extension bmp
	ptrExt = strstr(szFile, ".");
	if (ptrExt == NULL)
		strcat((char *)szFile, ".BMP");
	else
		strcpy(ptrExt, ".BMP");

	if (strstr(szFileSCN, ".SCN") == NULL)
		strcat((char *)szFileSCN, ".SCN");

	ScreenGrab.Write((LPSTR)szFile);

	if (!wWorld.bSaveWorld(szFileSCN))
		MessageBox(NULL, "Cannot save file!", "Save Error", MB_OK);

}

void CSaveGameDlg::OnOK()
{
    CMultiDlg::OnOK();
}

void CSaveGameDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
        case IDOK:
		{
			int nIndex;
            char    szName[_MAX_FNAME];

			if (GetDlgItemText(hwnd, IDC_EDIT_SAVE, szName, _MAX_FNAME) == 0)
			{
				// Take the selected item
				HWND SelHwnd = GetDlgItem(hwnd, IDC_LISTSAVE);
				if ((nIndex = ListBox_GetCurSel(SelHwnd)) != -1)
				{
					char    szName[_MAX_FNAME];

					ListBox_GetText(SelHwnd, nIndex, szName);
					SaveLevel(szName);
					EndDialog(hwnd, -1);
				}
			}
			else
			{
				// use the edit box string
					SaveLevel(szName);
					EndDialog(hwnd, -1);
			}
			break;
		}
        case IDC_LISTSAVE:
            switch (codeNotify)
            {
                case LBN_DBLCLK:
                    {
						char    szName[_MAX_FNAME];

                        ListBox_GetText(hwndCtl, ListBox_GetCurSel(hwndCtl), szName);
 						SetDlgItemText(hwnd, IDC_EDIT_SAVE, szName);
                        SaveLevel(szName);
                        EndDialog(m_hwnd, -1);
                    }
                    break;

                case LBN_SELCHANGE:
					{
						char    szName[_MAX_FNAME];

                        ListBox_GetText(hwndCtl, ListBox_GetCurSel(hwndCtl), szName);
						SetDlgItemText(hwnd, IDC_EDIT_SAVE, szName);
						EnableWindow(GetDlgItem(m_hwnd, IDOK), TRUE);
					}
                    break;
 				default:
				   CMultiDlg::OnCommand(hwnd, id, hwndCtl, codeNotify);
				   break;
           }
            break;

		default:
           CMultiDlg::OnCommand(hwnd, id, hwndCtl, codeNotify);
           break;

    }
}



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CLoadGameDlg::CLoadGameDlg()
{
}


CLoadGameDlg::~CLoadGameDlg()
{
}

BOOL CLoadGameDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HWND            hwndList;
    HANDLE          hfind;
    WIN32_FIND_DATA finddata;
    char            szFile[_MAX_PATH];

    GetRegString(REG_KEY_INSTALLED_DIR, szFile, sizeof(szFile), "");
    if (szFile[strlen(szFile)] != '\\')
    {
        strcat(szFile, "\\");
    }

    strcat(szFile, "*.scn");

    hwndList = GetDlgItem(hwnd, IDC_FILES);

    hfind = FindFirstFile(szFile, &finddata);
    if (hfind != INVALID_HANDLE_VALUE)
    {
        do
        {
            ListBox_AddString(hwndList, finddata.cFileName);
        }
        while (FindNextFile(hfind, &finddata));

        FindClose(hfind);
    }

    ListBox_SetItemHeight(hwndList, 0, 16);
	ListBox_SetHorizontalExtent(hwndList, 300);

    EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);

    CenterWindow(hwnd, GetParent(hwnd));

    return CGDIDlgBase::OnInitDialog(hwnd, hwndFocus, lParam);
}

void CLoadGameDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
        case IDOK:
		{
			int nIndex;
			HWND SelHwnd = GetDlgItem(hwnd, IDC_FILES);
            if ((nIndex = ListBox_GetCurSel(SelHwnd)) != -1)
            {
                char    szName[_MAX_FNAME];

                ListBox_GetText(SelHwnd, nIndex, szName);
                LoadLevel(szName);
                EndDialog(hwnd, -1);
            }
            break;
		}
        case IDC_FILES:
            switch (codeNotify)
            {
                case LBN_DBLCLK:
                    {
                        char    szName[_MAX_FNAME];

                        ListBox_GetText(hwndCtl, ListBox_GetCurSel(hwndCtl), szName);
                        LoadLevel(szName);
                        EndDialog(m_hwnd, -1);
                    }
                    break;

                case LBN_SELCHANGE:
					{
                        char    szName[_MAX_FNAME];
						HDC		hdcSrc;

						ListBox_GetText(hwndCtl, ListBox_GetCurSel(hwndCtl), szName);
						LoadThumbNail(GetParent(hwndCtl), hdcSrc, szName, IDC_LEVEL_PICTURE);
						EnableWindow(GetDlgItem(m_hwnd, IDOK), TRUE);
					}
                    break;

                case LBN_SETFOCUS:
				   CMultiDlg::OnCommand(hwnd, id, hwndCtl, codeNotify);
				   break;
					
 				default:
				   CMultiDlg::OnCommand(hwnd, id, hwndCtl, codeNotify);
				   break;
           }
           break;

		default:
           CMultiDlg::OnCommand(hwnd, id, hwndCtl, codeNotify);
           break;


    }
}


void CLoadGameDlg::OnOK()
{
    CMultiDlg::OnOK();
}


BOOL CLoadGameDlg::LoadLevel(LPCSTR pszName)
{
    char            szFile[_MAX_PATH];

    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

    // BUGBUG:  Hack to get data drive location
    GetRegString(REG_KEY_INSTALLED_DIR, szFile, sizeof(szFile), "");
    if (szFile[strlen(szFile)] != '\\')
    {
        strcat(szFile, "\\");
    }
    strcat(szFile, pszName);

    g_CTPassGlobals.LoadScene(szFile);

    return TRUE;
}

void LoadThumbNail(HWND hwndParent, HDC hdcSrc, char *szName, int nID)
{
    RECT		rc, rcDst;
	HWND		hwndChild;
	HDC			hdc;
	char		*ptrExt;
    CRasterDC	*TempBitmap;
    char        szFile[_MAX_PATH];

    // BUGBUG:  Hack to get data drive location
    GetRegString("Data Drive", szFile, sizeof(szFile), "");
    strcat(szFile, "data\\");

	// replace the .grf extension bmp
	ptrExt = strstr(szName, ".");
	if (ptrExt == NULL)
		strcat(szName, ".BMP");
	else
		strcpy(ptrExt, ".BMP");

	strcat(szFile, szName);
	

    TempBitmap = (CRasterDC*)ReadAndConvertBMP(szName, true, g_hwnd);
	if (TempBitmap == NULL)
		return;  // Should never happen

	// Get the information of the area that will contain the bitmap
	hwndChild = GetDlgItem(hwndParent, nID);
	hdc = GetDC(hwndChild);

    GetWindowRect(hwndChild, &rcDst);
	// map the co-ordinates to the screen values
    MapWindowRect(hwndChild, NULL, &rcDst);

 //   SetRect(&rc, 0, 0, TempBitmap->iWidth, TempBitmap->iHeight);
    SetRect(&rc, 0, 0, TempBitmap->iWidth, TempBitmap->iHeight);
//    SetRect(&rc, 0, 0, rcDst.right-rcDst.left- BORDER_OFFSET, rcDst.bottom-rcDst.top - BORDER_OFFSET);

/*    BitBlt(GetDC(hwndChild), 0, 0,
           FILEICON_WIDTH, FILEICON_HEIGHT,
           TempBitmap->hdcGet(),
           0, 0,
           SRCCOPY);
*/

	StretchBlt(hdc, 0, 0,
           rcDst.right - rcDst.left - BORDER_OFFSET,
           rcDst.bottom - rcDst.top - BORDER_OFFSET,
           TempBitmap->hdcGet(),
           0, 0,
		   TempBitmap->iWidth, TempBitmap->iHeight,
           SRCCOPY);
	ReleaseDC (hwndChild, hdc);
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CDifficultyDlg::CDifficultyDlg()
{
}


CDifficultyDlg::~CDifficultyDlg()
{
}

BOOL CDifficultyDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    MoveWindow(m_hwnd, 0, 0, 320, 200, FALSE);

    CenterWindow(hwnd, GetParent(hwnd));

    return CGDIDlgBase::OnInitDialog(hwnd, hwndFocus, lParam);
}


void CDifficultyDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    CGDIDlgBase::OnCommand(hwnd, id, hwndCtl, codeNotify);
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

COptionsBase::COptionsBase()
{
}

COptionsBase::~COptionsBase()
{
}


BOOL COptionsBase::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    MoveWindow(m_hwnd, 0, 0, 320, 200, FALSE);

    CenterWindow(hwnd, GetParent(hwnd));

    return CGDIDlgBase::OnInitDialog(hwnd, hwndFocus, lParam);
}


void COptionsBase::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    int     i;

    switch (id)
    {
        default:
            CGDIDlgBase::OnCommand(hwnd, id, hwndCtl, codeNotify);
            break;

        case IDC_CONTROLS:
            {
                CControlsDlg        dlg;

                GDIDialogHandler(NULL, 
                                 &dlg, 
                                 MAKEINTRESOURCE(IDD_CONTROLS),
                                 m_hwnd);
            }
            break;

        case IDC_RENDERING:
#if 0
            {
                CRenderingDlgBase     dlg;

                i = GDIDialogHandler(NULL, 
                                     &dlg, 
                                     MAKEINTRESOURCE(IDD_RENDERING_BASE),
                                     m_hwnd);
                if (i == IDOK)
                {
                    dlg.Persist();

                    if (m_bInGame)
                    {
                        CUIWnd *        puiwnd;

                        SetupGameScreen();

                        CenterWindow(m_hwnd, GetParent(m_hwnd));
                        puiwnd = g_pMainWnd->m_pUIMgr->GetActiveUIWnd();
                        puiwnd->Draw();
                        puiwnd->Draw();
                        prasMainScreen->FlipToGDISurface();
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                }
            }
#endif
            break;

        case IDC_SOUND:
            {
                CSoundDlg       dlg;

                GDIDialogHandler(NULL, &dlg, MAKEINTRESOURCE(IDD_SOUND), m_hwnd);
            }
            break;

        case IDC_HELPBOX:
            {
                CHelpDlg        dlg;

                GDIDialogHandler(NULL, &dlg, MAKEINTRESOURCE(IDD_HELP), m_hwnd);
            }
            break;

        case IDOK:
            {
                EndDialog(m_hwnd, 0);
            }
            break;

        case IDC_QUITGAME:
            EndDialog(m_hwnd, -1);
            break;

        case IDC_SAVEGAME:
            {
                CSaveGameDlg        dlg;
			
                GDIDialogHandler(NULL, &dlg, MAKEINTRESOURCE(IDD_SAVEGAME), m_hwnd);
            }
            break;

        case IDC_LOADGAME:
            {
                CLoadGameDlg        dlg;

                GDIDialogHandler(NULL, &dlg, MAKEINTRESOURCE(IDD_LOADGAME), m_hwnd);
            }
            break;

        case IDC_DIFFICULTY:
            {
                CDifficultyDlg        dlg;

                GDIDialogHandler(NULL, &dlg, MAKEINTRESOURCE(IDD_DIFFICULTY), m_hwnd);
            }
            break;
    }
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CGDINewGameDlg::CGDINewGameDlg()
{
}

CGDINewGameDlg::~CGDINewGameDlg()
{
}

BOOL CGDINewGameDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HWND            hwndList;
    HANDLE          hfind;
    WIN32_FIND_DATA finddata;
    char            szFile[_MAX_PATH];

    GetRegString("Data Drive", szFile, sizeof(szFile), "");
    strcat(szFile, "data\\");
    strcat(szFile, "*.scn");

    hwndList = GetDlgItem(hwnd, IDC_LIST);

    hfind = FindFirstFile(szFile, &finddata);
    if (hfind != INVALID_HANDLE_VALUE)
    {
        do
        {
            ListBox_AddString(hwndList, finddata.cFileName);
        }
        while (FindNextFile(hfind, &finddata));

        FindClose(hfind);
    }

    ListBox_SetItemHeight(hwndList, 0, 16);

    EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);

    CenterWindow(hwnd, GetParent(hwnd));

    return CGDIDlgBase::OnInitDialog(hwnd, hwndFocus, lParam);
}

void CGDINewGameDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    HWND        hwndList;

    switch (id)
    {
        default:
            CMultiDlg::OnCommand(hwnd, id, hwndCtl, codeNotify);
            break;

        case IDOK:
            hwndList = GetDlgItem(m_hwnd, IDC_LIST);
            if (ListBox_GetCurSel(hwndList) != -1)
            {
                char    szName[50];

                ListBox_GetText(hwndList, ListBox_GetCurSel(hwndList), szName);
                LoadLevel(szName);
                EndDialog(m_hwnd, -1);
            }
            break;

        case IDC_LIST:
            switch (codeNotify)
            {
                case LBN_DBLCLK:
                    {
                        char    szName[50];

                        ListBox_GetText(hwndCtl, ListBox_GetCurSel(hwndCtl), szName);
                        LoadLevel(szName);
                        EndDialog(m_hwnd, -1);
                    }
                    break;

                case LBN_SELCHANGE:
                    EnableWindow(GetDlgItem(m_hwnd, IDOK), TRUE);
                    break;
            }
            break;
    }
}


void CGDINewGameDlg::OnOK()
{
    CMultiDlg::OnOK();
}


BOOL CGDINewGameDlg::LoadLevel(LPCSTR pszName)
{
    char            szFile[_MAX_PATH];

    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

    // BUGBUG:  Hack to get data drive location
    GetRegString("Data Drive", szFile, sizeof(szFile), "");
    strcat(szFile, "data\\");
    strcat(szFile, pszName);

    g_CTPassGlobals.LoadScene(szFile);

    return TRUE;
}

