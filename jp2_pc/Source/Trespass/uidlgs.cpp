//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1996.
//
//  File:       uidlgs.cpp
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    23-Nov-96   SHernd   Created
//
//---------------------------------------------------------------------------


#include "precomp.h"
#pragma hdrstop

#include "tpassglobals.h"
#include "supportfn.hpp"
#include "resource.h"
#include "dialogs.h"
#include "main.h"
#include "uiwnd.h"
#include "uidlgs.h"
#include "video.h"
#include "token.h"
#include "..\Lib\Sys\reg.h"
#include "..\lib\sys\reginit.hpp"
#include "keyremap.h"
#include "Lib/Sys/DWSizeStruct.hpp"



extern HINSTANCE    g_hInst;
extern HWND		    g_hwnd;
extern CMainWnd *   g_pMainWnd;

UINT                g_uiRandom = (UINT)-1;



#define COUNT_LEVEL_IMAGES      1
struct
{
    char    szSCN[20];
    char    szImage[20];
} g_aszSCNImage[] =
{
    {  "as.scn", "menu\\li_a1" },
    { "as2.scn", "menu\\li_a2" },
    {  "be.scn", "menu\\li_be" },
    {  "ij.scn", "menu\\li_ij" },
    {  "it.scn", "menu\\li_it" },
    {  "jr.scn", "menu\\li_jr" },
    { "lab.scn", "menu\\li_lab" },
    {  "pv.scn", "menu\\li_pv" },
    { "sum.scn", "menu\\li_sum" },
    {        "", "menu\\li_other" },
};


void LoadDDFIntoListbox(HANDLE hFile, CUIListbox * plistbox)
{

    WNDFILETOKEN    wft;
    BOOL            bFinished;
    BOOL            bRet;

    bFinished = FALSE;
    while (!bFinished)
    {
        bRet = ReadWndFileToken(hFile, &wft);
        if (!bRet || wft.TokenType == LFTT_EOF)
        {
            bFinished = TRUE;
        }
        else
        {
            plistbox->AddItem(wft.val.sz, 0, -1, 0);
        }
    }
}


void InitRandom()
{
    if (g_uiRandom == (UINT)-1)
    {
        g_uiRandom = SetTimer(g_pMainWnd->m_pUIMgr->m_hwnd, IDTIMER_MAINRANDOM, 10000, NULL);
    }
}


void KillRandom()
{
    KillTimer(g_pMainWnd->m_pUIMgr->m_hwnd, g_uiRandom);
    g_uiRandom = (UINT)-1;
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



CUIDlg::CUIDlg(CUIManager * puimgr) : CUIWnd(puimgr)
{
    m_bKeyStyle = KEYSTYLE_ESCAPE;
}


CUIDlg::~CUIDlg()
{
}


BOOL CUIDlg::OnCreate()
{
    if (!CUIWnd::OnCreate())
    {
        TraceError(("CUIDlg::OnCreate() -- base create failed"));
        return FALSE;
    }

    return TRUE;
}


void CUIDlg::OnDestroy()
{
    CUIWnd::OnDestroy();
}


BOOL CUIDlg::UIButtonAudioCmd(CUIButton * pbutton, BUTTONCMD bc)
{
    if (bc == BC_DOWN)
    {
        g_CTPassGlobals.PlayButtonAudio();
    }

    return TRUE;
}

//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool CMainScreenWnd::m_fAMDShowLogoOnce = FALSE;

CMainScreenWnd::CMainScreenWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
    m_uiInitial = (UINT)-1;
    m_pUIMgr->m_bDrawMouse = TRUE;
    m_pctrlAMDLogo = NULL;
}

CMainScreenWnd::~CMainScreenWnd()
{
    g_CTPassGlobals.StopBackgroundAudio();
    g_CTPassGlobals.FreeMenuAudio();

    if (m_uiInitial != (UINT)-1)
    {
        KillTimer(m_pUIMgr->m_hwnd, m_uiInitial);
    }

    KillRandom();
}


BOOL CMainScreenWnd::OnCreate()
{
    CUICtrl * pctrl;

    if (!CUIDlg::OnCreate())
    {
        TraceError(("CMainScreenWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    CenterUIWindow(this);

    g_CTPassGlobals.CreateMenuAudioDatabase();
    g_CTPassGlobals.SetupMenuAudio();

    m_uiInitial = SetTimer(m_pUIMgr->m_hwnd, IDTIMER_MAININIT, 2000, NULL);

    m_pctrlAMDLogo = GetUICtrl(1005);
    if (m_pctrlAMDLogo && m_fAMDShowLogoOnce)
    {
        m_pctrlAMDLogo->SetVisible(FALSE);
    }
    else
    {
        m_fAMDShowLogoOnce = TRUE;
    }

#if BUILDVER_MODE == MODE_FINAL
    pctrl = GetUICtrl(1004);
    if (pctrl)
    {
        pctrl->SetActive(FALSE);
    }
#endif

    return TRUE;
}


void CMainScreenWnd::OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (fDown)
    {
        return;
    }

    switch (vk)
    {
        case 'W':
            if (IsKeyDown('Q') && IsKeyDown(VK_SHIFT) && IsKeyDown(VK_CONTROL))
            {
                if (m_pctrlAMDLogo)
                {
                    m_fAMDShowLogoOnce = TRUE;
                    m_pctrlAMDLogo->SetVisible(FALSE);
                }

                DirectLoad();
            }
            break;
    }
}


void CMainScreenWnd::OnTimer(UINT uiID)
{
    switch (uiID)
    {
        case IDTIMER_MAININIT:
            KillTimer(m_pUIMgr->m_hwnd, m_uiInitial);
            g_CTPassGlobals.StartBackgroundAudio();
            InitRandom();
            break;

        case IDTIMER_MAINRANDOM:
            g_CTPassGlobals.PlayRandomAudio();
            break;
    }
}


void CMainScreenWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "mainscreen.ddf");
}


void CMainScreenWnd::UIButtonUp(CUIButton * pbutton)
{
    if (m_pctrlAMDLogo)
    {
        m_fAMDShowLogoOnce = TRUE;
        m_pctrlAMDLogo->SetVisible(FALSE);
    }

    switch (pbutton->GetID())
    {
        case 1000:
            {
                int     iRet;

                g_CTPassGlobals.FreeMenuAudio();

                CVideoWnd   video(m_pUIMgr);

                video.Play("menu\\newgame");
                m_pUIMgr->Detach(&video);

                iRet = g_CTPassGlobals.LoadLevel(GetFirstLevelName().c_str());

                if (iRet >= 0)
                {
                    EndUIWnd(2);
                }
                else
                {
                    DisplayLoadingFileError(iRet);
                }
            }
            break;

        // Load Game
        case 1001:
            {
                CLoadGameWnd    dlg(m_pUIMgr);

                dlg.StartUIWnd();
                
                // A Level was loaded so now we start the game
                if (dlg.m_dwExitValue == 1)
                {
                    EndUIWnd(2);
                }
            }
            break;

        // configure
        case 1002:
            {
                COptionsWnd     dlg(m_pUIMgr);

                dlg.StartUIWnd();
				if (dlg.m_dwExitValue == 3)
				{
					// Restart The game with the render 
					// dialog
					EndUIWnd(3);
				}
            }
            break;

        case 1003:
            {
                CYesNoMsgDlg    dlgExit(IDS_EXIT_TRESPASSER, m_pUIMgr);

                dlgExit.StartUIWnd();

                if (dlgExit.m_dwExitValue == 1)
                {
                    EndUIWnd((DWORD)-1);
                }
            }
            break;

        case 1004:
            DirectLoad();
            break;
    }
}


void CMainScreenWnd::DirectLoad()
{
    CDirectLoadWnd     dlg(m_pUIMgr);

    dlg.StartUIWnd();

    // A Level was loaded so now we start the game
    if (dlg.m_dwExitValue == 1)
    {
        EndUIWnd(2);
    }
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CLoaderWnd::CLoaderWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
    InitializeCriticalSection(&m_cs);

    m_puitext = NULL;
    m_iCycle = 0;

	iLoadPercent = 0;
	iLastLoadPercent = -1;
	iCopyPercent = 0;
	iLastCopyPercent = -1;

	dwCopyTotal = 1;
}


CLoaderWnd::~CLoaderWnd()
{
    DeleteCriticalSection(&m_cs);
}


BOOL CLoaderWnd::SetupUIWnd()
{
    BOOL        bRet;
    MSG         msg;

    if (!m_pUIMgr->Attach(this))
    {
        TraceError(("CLoaderWnd::SetupUIWnd() -- "
                    "Unable to attach window"));
        goto Error;
    }

    if (!OnCreate())
    {
        TraceError(("CLoaderWnd::SetupUIWnd() -- "
                    "UIWnd creation failed"));
        goto Error;
    }

    // Suck off all the mouse and keyboard Messages that are 
    // in the message queue
    while (PeekMessage(&msg,NULL,WM_MOUSEFIRST, WM_MOUSELAST,PM_REMOVE))
    {
        ;
    }
    while (PeekMessage(&msg,NULL,WM_KEYFIRST, WM_KEYLAST,PM_REMOVE))
    {
        ;
    }

    m_uiTimer = SetTimer(g_hwnd, 54, 100, NULL);

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CLoaderWnd::SetupUIWnd"));
    bRet = FALSE;
    goto Cleanup;
}


void CLoaderWnd::DestroyUIWnd()
{
    MSG         msg;

    m_pUIMgr->m_iMouse = 0;

    KillTimer(g_hwnd, 54);

    OnDestroy();

    m_pUIMgr->Detach(this);

    // Suck off all Messages that are in the message queue
    while (PeekMessage(&msg,NULL,WM_MOUSEFIRST, WM_MOUSELAST,PM_REMOVE))
    {
        ;
    }
    while (PeekMessage(&msg,NULL,WM_KEYFIRST, WM_KEYLAST,PM_REMOVE))
    {
        ;
    }
}


void CLoaderWnd::OnTimer(UINT uiID)
{
    int     iIndex;

    if (uiID != 54 || !m_puitext)
    {
        return;
    }

    COLORREF acr[10] = 
    {
        RGB(255, 255, 255),
        RGB(255, 255, 227),
        RGB(255, 255, 199),
        RGB(255, 255, 171),
        RGB(255, 255, 143),
        RGB(255, 255, 114),
        RGB(255, 255,  85),
        RGB(255, 255,  56),
        RGB(255, 255,  27),
        RGB(255, 255,   0),
    };

    if (++m_iCycle >= 20)
    {
        m_iCycle = 0;
    }

    if (m_iCycle >= 10)
    {
        iIndex = 9 - (m_iCycle - 10);
    }
    else
    {
        iIndex = m_iCycle;
    }

    m_puitext->SetFGColor(acr[iIndex]);
}


void CLoaderWnd::InnerWindowLoop(bool bPaint)
{
    IDirectDrawSurface4 *   pSurface;
    HRESULT                 hr;
    CUIWnd *                puiwnd;
    CDDSize<DDBLTFX>        ddfx;

    PreLoopCall();

    CheckMessages();

    if (m_pUIMgr->m_bActive && !m_bExitWnd && bPaint)
    {
        DoUIHandling();
        InnerLoopCall();

        // Draw everything to the back buffer
        puiwnd = m_pUIMgr->GetActiveUIWnd();
        puiwnd->DrawIntoSurface(prasMainScreen.ptPtrRaw(), &m_pUIMgr->m_rcInvalid);

        ddfx.dwROP = SRCCOPY;
        ddfx.dwDDFX = DDBLTFX_NOTEARING;

        // Now draw everthing from the back buffer to the front buffer
        pSurface = prasMainScreen->GetPrimarySurface();
        hr = pSurface->Blt(&m_pUIMgr->m_rcInvalid,
                           prasMainScreen->pddsDraw4,
                           &m_pUIMgr->m_rcInvalid,
                           DDBLT_WAIT | DDBLT_ROP,
                           &ddfx);

        // Reset the invalid rect
        SetRect(&m_pUIMgr->m_rcInvalid, 0, 0, 0, 0);
    }
}


BOOL CLoaderWnd::OnCreate()
{
    if (!CUIDlg::OnCreate())
    {
        TraceError(("CMainScreenWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    CenterUIWindow(this);

    SetRect(&m_pUIMgr->m_rcInvalid,
            0,
            0,
            prasMainScreen->iWidthFront,
            prasMainScreen->iHeightFront);

    m_rc.left = 0;
    m_rc.top = 0;
    m_rc.right = prasMainScreen->iWidthFront;
    m_rc.bottom = prasMainScreen->iHeightFront;

    m_pUIMgr->m_iMouse = 1;

    m_puitext = (CUITextbox *)GetUICtrl(102);

    SetupBackgroundImage();

    return TRUE;
}



// look for the proper SCN matches from the currently loading
// level and the SCN Image match array
void CLoaderWnd::SetupBackgroundImage()
{
    int         icSCNs = sizeof(g_aszSCNImage)/sizeof(g_aszSCNImage[0]);
    int         i;
    bool        bFound;
    int         iFoundAt;
    CRaster *   pras;
    RECT        rcMain;
    RECT        rcImage;
    int         xDst;
    int         yDst;
    char        szName[_MAX_PATH];

    // Identify the image we want to load as the background.  This is
    // determined by the SCN name.  If the scn is not known we will use
    // an "other" generic image.

    for (i = 0, bFound = false; i < icSCNs && !bFound; i++)
    {
        if (strcmpi(g_CTPassGlobals.m_szSCN, g_aszSCNImage[i].szSCN) == 0)
        {
            bFound = true;
            iFoundAt = i;
        }
    }

    if (!bFound)
    {
        iFoundAt = icSCNs - 1;
    }

    // now that we have identified the image that we want to load,
    // We must create the background to draw from.

    // Create the primary background image.
    SetRect(&rcMain, 0, 0, prasMainScreen->iWidthFront, prasMainScreen->iHeightFront);
    m_pRaster = new CRasterMem(rcMain.right, rcMain.bottom, 16, 0);

    // Fill the background with black
    MyFillRect(&rcMain, 0, m_pRaster);

    // Create The image name
    wsprintf(szName, 
             "%s_%i.tga", 
             g_aszSCNImage[iFoundAt].szImage,
             rand() % COUNT_LEVEL_IMAGES);

    // Load the image.
    pras = ReadAndConvertBMP(szName, false);
    if (!pras)
        return;

    SetRect(&rcImage, 0, 0, pras->iWidth, pras->iHeight);

    // Determine the blitting area
    xDst = (rcMain.right / 2) - (rcImage.right / 2);
    yDst = (rcMain.bottom / 2) - (rcImage.bottom / 2);

    // now blit this image into the center of the background.
    RasterBlt(pras,
              &rcImage,
              m_pRaster,
              xDst,
              yDst,
              false,
              0);

    delete pras;
}


void CLoaderWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "loader.ddf");
}


DWORD CLoaderWnd::HandleNotify(DWORD dwParam1, 
                               DWORD dwParam2, 
                               DWORD dwParam3)
{
    bool    bPaint = false;

    //EnterCriticalSection(&m_cs);

	switch (dwParam1)
	{
		case 0:
			// Just update (if there are any changes) no actual messages
			bPaint = true;
			break;

		case 1:
			// Percent complete dialog update.
			{
				CUIProgressBar *    pprog;

				// Compute load percent complete.
				iLoadPercent = (int)((dwParam2 * 100) / dwParam3);

				// Update progress bar.
				pprog = (CUIProgressBar *)GetUICtrl(100);
				Assert(pprog);
				pprog->SetPos(iLoadPercent);

				bPaint = true;
			}
			break;

		case 2:
			// Copy progress complete dialog update.
			{
				CUIProgressBar *    pprog;

				// Compute copy percent complete.
				iCopyPercent = (int)((dwParam2 * 100) / dwCopyTotal);

				// Update progress bar.
				pprog = (CUIProgressBar *)GetUICtrl(101);
				Assert(pprog);
				pprog->SetPos(dwParam2);

				return 0;
			}
			break;

		case 3:
			// Copy progress complete dialog range.
			{
				CUIProgressBar *    pprog;

				pprog = (CUIProgressBar *)GetUICtrl(101);
				Assert(pprog);

				pprog->SetRange(dwParam2, dwParam3, NULL, NULL);

				// Save total.
				dwCopyTotal = dwParam3;

				return 0;
			}
			break;

		case 4:
			// Begin loading D3D surfaces.
			{
			}
			break;

		case 5:
			// Loading D3D surfaces (update iLoadPercent).
			{
				if (iLoadPercent < 100)
				{
					// Increment percentage.
					iLoadPercent++;

					// Update progress bar.
					CUIProgressBar* pprog = (CUIProgressBar *)GetUICtrl(100);
					Assert(pprog);
					pprog->SetPos(iLoadPercent);

					bPaint = true;
				}
			}
			break;

		case 6:
			{
				CenterUIWindow(this);
				InvalidateRect(NULL);

				// Force paint.
				iLastCopyPercent = -1;
				iLastLoadPercent = -1;
				bPaint = true;
			}
			break;
	}

	if (bPaint && (iCopyPercent > iLastCopyPercent || iLoadPercent > iLastLoadPercent))
	{
		iLastCopyPercent = iCopyPercent;
		iLastLoadPercent = iLoadPercent;

		InnerWindowLoop(bPaint);
	}

    //LeaveCriticalSection(&m_cs);

    return 0;
}



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------




CDirectLoadWnd::CDirectLoadWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
}

CDirectLoadWnd::~CDirectLoadWnd()
{
}


BOOL CDirectLoadWnd::OnCreate()
{
    CUIListbox *    puilist;
    HANDLE          hfind;
    WIN32_FIND_DATA finddata;
    char            szFile[_MAX_PATH];

    if (!CUIDlg::OnCreate())
    {
        TraceError(("CDirectLoadWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    GetFileLoc(FA_DATADRIVE, szFile, sizeof(szFile));
    strcat(szFile, "*.scn");

    puilist = (CUIListbox*)GetUICtrl(1002);

    hfind = FindFirstFile(szFile, &finddata);
    if (hfind != INVALID_HANDLE_VALUE)
    {
        do
        {
            puilist->AddItem(finddata.cFileName, 0, -1, 0);
        }
        while (FindNextFile(hfind, &finddata));

        FindClose(hfind);
    }

    CUIButton * pbutton;

    pbutton = (CUIButton*)GetUICtrl(1001);
    Assert(pbutton);
    pbutton->SetActive(FALSE);

    CenterUIWindow(this);

    return TRUE;
}


void CDirectLoadWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "directload.ddf");
}


void CDirectLoadWnd::UIButtonUp(CUIButton * pbutton)
{
    switch (pbutton->GetID())
    {
        case 1000:
            EndUIWnd(0);
            break;

        case 1001:
            {
                char            szName[50];
                DWORD           dwParam;
                WORD            wFlags;
                CUIListbox *    pctrl;
                int             iIndex;

                pctrl = (CUIListbox *)GetUICtrl(1002);
                iIndex = pctrl->GetCurrSel();

                pctrl->GetItem(szName, 
                               sizeof(szName), 
                               dwParam, 
                               wFlags,
                               iIndex);

                g_CTPassGlobals.FreeMenuAudio();
                iIndex = g_CTPassGlobals.LoadLevel(szName);

                if (iIndex < 0)
                {
                    DisplayLoadingFileError(iIndex);
                }
                else
                {
                    EndUIWnd(1);
                }
            }
            break;
    }
}


void CDirectLoadWnd::UIListboxClick(CUICtrl * pctrl, int iIndex)
{
    (GetUICtrl(1001))->SetActive(TRUE);
}


void CDirectLoadWnd::UIListboxDblClk(CUICtrl * pctrl, int iIndex)
{
    char    szName[50];
    DWORD   dwParam;
    WORD    wFlags;

    ((CUIListbox *)pctrl)->GetItem(szName, 
                                   sizeof(szName), 
                                   dwParam, 
                                   wFlags,
                                   iIndex);
	
    g_CTPassGlobals.FreeMenuAudio();
    g_CTPassGlobals.LoadLevel(szName);
    EndUIWnd(1);
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------




CNewGameWnd::CNewGameWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
}

CNewGameWnd::~CNewGameWnd()
{
}


BOOL CNewGameWnd::OnCreate()
{
    CUIListbox *    puilist;
    HANDLE          hFile;
    char            szFile[_MAX_PATH];

    if (!CUIDlg::OnCreate())
    {
        TraceError(("CDirectLoadWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    GetFileLoc(FA_DATADRIVE, szFile, sizeof(szFile));
    strcat(szFile, "menu\\ngi.ddf");

    puilist = (CUIListbox*)GetUICtrl(1002);

    hFile = CreateFile(szFile, 
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        LoadDDFIntoListbox(hFile, puilist);
    }

    CloseHandle(hFile);

    CenterUIWindow(this);

    return TRUE;
}


void CNewGameWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "newgame.ddf");
}


void CNewGameWnd::UIButtonUp(CUIButton * pbutton)
{
    switch (pbutton->GetID())
    {
        case 1000:
            EndUIWnd(0);
            break;

        case 1001:
            EndUIWnd(1);
            break;
    }
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CQuitWnd::CQuitWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
}

CQuitWnd::~CQuitWnd()
{
}


BOOL CQuitWnd::OnCreate()
{
    if (!CUIDlg::OnCreate())
    {
        TraceError(("CQuitWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    CenterUIWindow(this);

    return TRUE;
}


void CQuitWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "quit.ddf");
}


void CQuitWnd::UIButtonUp(CUIButton * pbutton)
{
    switch (pbutton->GetID())
    {
        // Cancel
        case 1000:
            EndUIWnd(0);
            break;

        // Quit Game
        case 1001:
            EndUIWnd(1);
            break;

        // Quit Menu
        case 1002:
            EndUIWnd(2);
            break;
    }
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


COptionsWnd::COptionsWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
}

COptionsWnd::~COptionsWnd()
{
}


BOOL COptionsWnd::OnCreate()
{
    if (!CUIDlg::OnCreate())
    {
        TraceError(("COptionsWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    CenterUIWindow(this);

    return TRUE;
}


void COptionsWnd::GetWndFile(LPSTR psz, int ic)
{
    if (g_CTPassGlobals.bInGame)
    {
        strcpy(psz, "options2.ddf");
    }
    else
    {
        strcpy(psz, "options.ddf");
    }
}


void COptionsWnd::UIButtonUp(CUIButton * pbutton)
{
    switch (pbutton->GetID())
    {
        // Controls
        case 1000:
            {
                CControlsWnd    dlg(m_pUIMgr);

                dlg.StartUIWnd();
            }
            break;

        // Rendering
        case 1001:
            {
                CRenderWnd       dlg(m_pUIMgr);

                dlg.StartUIWnd();
                if (dlg.m_dwExitValue != 0)
                {
                    EndUIWnd(dlg.m_dwExitValue);
                }
            }
            break;

        // Audio
        case 1002:
            {
                CAudioWnd       dlg(m_pUIMgr);

                dlg.StartUIWnd();
            }
            break;

        // Credits
        case 1003:
            {
                // Pause the background audio
                g_CTPassGlobals.StopBackgroundAudio();
                KillRandom();

                // Play the credits video
                CVideoWnd   video(m_pUIMgr);

                video.Play("menu\\credits");

                // Start the background audio
                g_CTPassGlobals.StartBackgroundAudio();
                InitRandom();
            }
            break;

        // Return
        case 1004:
            EndUIWnd(0);
            break;
    }
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CInGameOptionsWnd::CInGameOptionsWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
}

CInGameOptionsWnd::~CInGameOptionsWnd()
{
    g_CTPassGlobals.FreeMenuAudio();
}


BOOL CInGameOptionsWnd::OnCreate()
{
    if (!CUIDlg::OnCreate())
    {
        TraceError(("CInGameOptionsWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    g_CTPassGlobals.CreateMenuAudioDatabase();
    g_CTPassGlobals.SetupButtonAudio();

    CenterUIWindow(this);

    return TRUE;
}


void CInGameOptionsWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "ingameopt.ddf");
}


void CInGameOptionsWnd::UIButtonUp(CUIButton * pbutton)
{
    switch (pbutton->GetID())
    {
        // Quit
        case 1000:
            EndUIWnd((DWORD)-1);
            break;

        // Resume
        case 1001:
            EndUIWnd(0);
            break;

        // Restart
        case 1002:
            {
                CYesNoMsgDlg   dlg(IDS_RESTARTLEVEL, m_pUIMgr);

                dlg.StartUIWnd();
                if (dlg.m_dwExitValue == 0)
                {
                    return;
                }

                g_CTPassGlobals.LoadLastScene();
                EndUIWnd(0);
            }
            break;

        case 1003:
            {
                CSaveGameWnd    dlg(m_pUIMgr);

                dlg.StartUIWnd();
            }
            break;

        case 1004:
            {
                CLoadGameWnd    dlg(m_pUIMgr);

                dlg.StartUIWnd();
                if (dlg.m_dwExitValue == 1)
                {
                    EndUIWnd(0);
                }
            }
            break;

        case 1005:
            {
                COptionsWnd     dlg(m_pUIMgr);

                dlg.StartUIWnd();

                switch (dlg.m_dwExitValue)
                {
                    case 0:
                        break;

                    case 1:
                    case 2:
					    g_CTPassGlobals.ResetScreen(dlg.m_dwExitValue == 2);
					    CenterUIWindow(this);
                        break;

                    case 3:
                        EndUIWnd(3);
                        break;

                }

            }
            break;
    }
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CAudioWnd::CAudioWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
}

CAudioWnd::~CAudioWnd()
{
}


BOOL CAudioWnd::OnCreate()
{
    CUICheckbox *   pcheck;
    CUISlider *     pslider;

    if (!CUIDlg::OnCreate())
    {
        TraceError(("CAudioWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    CenterUIWindow(this);

    // Fill In the defaults for changed
    m_uiVolume = (UINT)CAudio::pcaAudio->GetVolume();
    pslider = (CUISlider *)GetUICtrl(1004);
    pslider->SetCurrUnit(m_uiVolume);

    m_bSFX = GetRegValue(REG_KEY_AUDIO_EFFECT, DEFAULT_AUDIO_EFFECT);
    pcheck = (CUICheckbox *)GetUICtrl(1020);
    pcheck->SetDown(m_bSFX);

    m_bAmbient = GetRegValue(REG_KEY_AUDIO_AMBIENT, DEFAULT_AUDIO_AMBIENT);
    pcheck = (CUICheckbox *)GetUICtrl(1021);
    pcheck->SetDown(m_bAmbient);

    m_bVO = GetRegValue(REG_KEY_AUDIO_VOICEOVER, DEFAULT_AUDIO_VOICEOVER);
    pcheck = (CUICheckbox *)GetUICtrl(1022);
    pcheck->SetDown(m_bVO);

    m_bMusic = GetRegValue(REG_KEY_AUDIO_MUSIC, DEFAULT_AUDIO_MUSIC);
    pcheck = (CUICheckbox *)GetUICtrl(1023);
    pcheck->SetDown(m_bMusic);

    m_bSubtitles = GetRegValue(REG_KEY_AUDIO_SUBTITLES, DEFAULT_AUDIO_SUBTITLES);
    pcheck = (CUICheckbox *)GetUICtrl(1024);
    pcheck->SetDown(m_bSubtitles);

    m_bEnable3D = GetRegValue(REG_KEY_AUDIO_ENABLE3D, DEFAULT_AUDIO_ENABLE3D);
    pcheck = (CUICheckbox *)GetUICtrl(1025);
    pcheck->SetDown(m_bEnable3D);

    return TRUE;
}


void CAudioWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "audio.ddf");
}


void CAudioWnd::UIButtonUp(CUIButton * pbutton)
{
    switch (pbutton->GetID())
    {
        // cancel
        case 1000:
            OnCancel();
            EndUIWnd((DWORD)-1);
            break;

        // ok
        case 1001:
            OnOk();
            EndUIWnd(0);
            break;
    }
}


void CAudioWnd::UIHotspotClick(CUIHotspot * pctrl, BOOL bDown)
{
    int             iCmdSrc;
    CUICheckbox *   pcheck;

    if (!bDown)
    {
        return;
    }

    // Convert the control ID to a command #
    iCmdSrc = pctrl->GetID() - 1100;
    
    pcheck = (CUICheckbox*)GetUICtrl(iCmdSrc + 1020);
    if (!pcheck)
    {
        return;
    }

    pcheck->SetDown(!pcheck->GetDown());
}


void CAudioWnd::UISliderChange(CUISlider * pslider, int iNewValue)
{
    CUICheckbox *   pcheck;

    if (iNewValue == 0)
    {
        pcheck = (CUICheckbox *)GetUICtrl(1024);
        pcheck->SetDown(TRUE);

        pcheck = (CUICheckbox *)GetUICtrl(1022);
        pcheck->SetDown(TRUE);
    }
}


void CAudioWnd::OnOk()
{
    CUISlider *     pslider;
    CUICheckbox *   pcheck;

    // Update the items
    pslider = (CUISlider *)GetUICtrl(1004);
    m_uiVolume = pslider->GetCurrUnit();
    //pass value from 0 to 100
    CAudio::pcaAudio->SetVolume((float)m_uiVolume);

    pcheck = (CUICheckbox *)GetUICtrl(1020);
    m_bSFX = pcheck->GetDown();
    SetRegValue(REG_KEY_AUDIO_EFFECT, m_bSFX);

    pcheck = (CUICheckbox *)GetUICtrl(1021);
    m_bAmbient = pcheck->GetDown();
    SetRegValue(REG_KEY_AUDIO_AMBIENT, m_bAmbient);

    pcheck = (CUICheckbox *)GetUICtrl(1022);
    m_bVO = pcheck->GetDown();
    SetRegValue(REG_KEY_AUDIO_VOICEOVER, m_bVO);

    pcheck = (CUICheckbox *)GetUICtrl(1023);
    m_bMusic = pcheck->GetDown();
    SetRegValue(REG_KEY_AUDIO_MUSIC, m_bMusic);

    pcheck = (CUICheckbox *)GetUICtrl(1024);
    m_bSubtitles = pcheck->GetDown();
    SetRegValue(REG_KEY_AUDIO_SUBTITLES, m_bSubtitles);

    pcheck = (CUICheckbox *)GetUICtrl(1025);
    m_bEnable3D = pcheck->GetDown();
    SetRegValue(REG_KEY_AUDIO_ENABLE3D, m_bEnable3D);

    RefreshAudioSettings();
}


void CAudioWnd::OnCancel()
{
    // Set the items to their orignal values
    SetRegValue(REG_KEY_AUDIO_EFFECT, m_bSFX);
    SetRegValue(REG_KEY_AUDIO_AMBIENT, m_bAmbient);
    SetRegValue(REG_KEY_AUDIO_VOICEOVER, m_bVO);
    SetRegValue(REG_KEY_AUDIO_MUSIC, m_bMusic);
    SetRegValue(REG_KEY_AUDIO_SUBTITLES, m_bSubtitles);
    SetRegValue(REG_KEY_AUDIO_ENABLE3D, m_bEnable3D);

    RefreshAudioSettings();
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



CHintWnd::CHintWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
}

CHintWnd::~CHintWnd()
{
}

BOOL CHintWnd::OnCreate()
{
    BOOL            bRet;
    char            sz[1024];
    CUITextbox *    ptextbox;

    if (!CUIDlg::OnCreate())
    {
        TraceError(("CHintWnd::OnCreate() -- base class create failed"));
        goto Error;
    }

    // We are going to literally translate the Hint ID into a string 
    // resource ID.

    ptextbox = (CUITextbox *)GetUICtrl(1002);
    Assert(ptextbox != NULL);

    LoadString(g_hInst, IDS_STR_HINTS + CSetHintAction::iCurrentHint, sz, sizeof(sz));

    ptextbox->SetText(sz);

    bRet = TRUE;

Cleanup:
    CenterUIWindow(this);
    return bRet;

Error:
    TraceError(("CHintWnd::OnCreate"));
    bRet = FALSE;
    goto Cleanup;
}


void CHintWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "hint.ddf");
}

void CHintWnd::UIButtonUp(CUIButton * pbutton)
{
    DWORD   dwID;

    dwID = pbutton->GetID();

    switch (dwID)
    {
        case 1000:
            EndUIWnd(0);
            break;
    }
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



CMsgDlg::CMsgDlg(CUIManager * puimgr) : CUIDlg(puimgr)
{
    m_bKeyStyle = 0;
}

CMsgDlg::~CMsgDlg()
{
}

BOOL CMsgDlg::OnCreate()
{
    BOOL            bRet;
    char            sz[1024];
    int             iStmt;
    CUITextbox *    ptextbox;

    if (!CUIDlg::OnCreate())
    {
        TraceError(("CMsgDlg::OnCreate() -- base class create failed"));
        goto Error;
    }

    ptextbox = (CUITextbox *)GetUICtrl(1002);
    Assert(ptextbox != NULL);

    iStmt = JustStatement();
    if (iStmt != -1)
    {
        LoadString(g_hInst, iStmt, sz, sizeof(sz));
    }
    else if (!GetText(sz, sizeof(sz)))
    {
        TraceError(("CMsgDlg::OnCreate() -- getting text to place in dialog failed"));
        goto Error;
    }

    ptextbox->SetText(sz);

    bRet = TRUE;

Cleanup:
    CenterUIWindow(this);
    return bRet;

Error:
    TraceError(("CMsgDlg::OnCreate"));
    bRet = FALSE;
    goto Cleanup;
}


void CMsgDlg::UIButtonUp(CUIButton * pbutton)
{
    DWORD   dwID;

    dwID = pbutton->GetID();

    switch (dwID)
    {
        case 1000:
            OnEnter();
            break;

        case 1001:
            OnEscape();
    }
}


