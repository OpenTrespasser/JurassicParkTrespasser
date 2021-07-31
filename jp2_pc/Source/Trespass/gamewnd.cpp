//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1996.
//
//  File:       mainwnd.cpp
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    26-Oct-96   SHernd   Created
//
//---------------------------------------------------------------------------


#include "precomp.h"
#pragma hdrstop

#include "tpassglobals.h"
#include "resource.h"
#include "supportfn.hpp"
#include "uidlgs.h"
#include "ctrls.h"
#include "main.h"
#include "keyremap.h"
#include "../Lib/Sys/reg.h"
#include "../Lib/Sys/RegInit.hpp"
#include "../Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "../Game/AI/AIMain.hpp"

BOOL bQuitGame = FALSE;


extern CPlayer *    gpPlayer;
extern HWND         g_hwnd;
extern HINSTANCE    g_hInst;
extern bool         bInvertMouse;

bool g_bDisplayLocation = false;
bool g_bDisplayFPS = false;

float fFrameRate = 0;           // Framerate in frames per second, calculated once per second
int iNumFramesThisSample = 0;   // Number of frames rendered in the current sample period
DWORD dwTimeLastSample = 0;     // Time when the framerate was last calculated
DWORD dwTimeLastFrame = 0;      // The time when the previous frame was rendered
float fTimeElapsed = 0;         // Milliseconds elapsed since the last frame was rendered

#define COUNT_NOTICED_DEAD      20


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

enum
{
    CHEAT_BONES,
    CHEAT_INVULNERABLE,
    CHEAT_LOCATION,
    CHEAT_TELEPORT,
    CHEAT_TNEXT,
    CHEAT_GORE,
    CHEAT_INVERTMOUSE,
    CHEAT_WIN,
    CHEAT_SORT,
    CHEAT_ALLAMMO,
    CHEAT_SLOMO,
	CHEAT_DINOS,
    CHEAT_FPS,
};

struct
{
    LPSTR   psz;
    DWORD   dw;
} CHEATS[] =
{
    "BONES", CHEAT_BONES,
    "INVUL", CHEAT_INVULNERABLE,
    "LOC", CHEAT_LOCATION,
    "TELE", CHEAT_TELEPORT,
    "TNEXT", CHEAT_TNEXT,
    "GORE", CHEAT_GORE,
    "IMOUSE", CHEAT_INVERTMOUSE,
    "WIN", CHEAT_WIN,
    "SORT", CHEAT_SORT,
    "WOO", CHEAT_ALLAMMO,
    "BIONICWOMAN", CHEAT_SLOMO,
	"DINOS", CHEAT_DINOS,
    "FPS", CHEAT_FPS,
};

int g_icCheats = sizeof(CHEATS) / sizeof(CHEATS[0]);

const char* g_cheatPrompt = " > ";

bool ExecuteCheat(LPSTR pszCheat)
{
    LPSTR   psz;
    bool    bRet;
    int     i;
    bool    bFound;
    char terminator = '\0';

    auto promptlen = std::strlen(g_cheatPrompt);
    if (std::strlen(pszCheat) < promptlen)
    {
        return false;
    }
    pszCheat += promptlen;

    psz = strchr(pszCheat, ' ');

    if (psz)
    {
        *psz = '\0';
        psz++;
    }
    else
    {
        psz = &terminator; //Avoid string literals for non-const char*
    }

    for (i = 0, bFound = false; i < g_icCheats && !bFound; i++)
    {
        if (strcmpi(CHEATS[i].psz, pszCheat) == 0)
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        return false;
    }

    switch (CHEATS[i].dw)
    {
        case CHEAT_BONES:
            pphSystem->bShowBones = !pphSystem->bShowBones;
            break;

        case CHEAT_INVULNERABLE:
#if 0
            gpPlayer->bInvulnerable = !gpPlayer->bInvulnerable;
#endif
            break;

        case CHEAT_LOCATION:
            g_bDisplayLocation = !g_bDisplayLocation;
            break;

        case CHEAT_TELEPORT:
#if 0
            {
                float       fX;
                float       fY;
                float       fZ;

                sscanf(psz, "%f, %f, %f", &fX, &fY, &fZ);

			    extern void PlayerTeleportToXYZ(float fX, float fY, float fZ);

                PlayerTeleportToXYZ(fX, fY, fZ);
            }
#endif
            break;

        case CHEAT_TNEXT:
            {
				extern void PlayerTeleportToNextLocation();
				PlayerTeleportToNextLocation();
            }
            break;

        case CHEAT_GORE:
            {
                int         i;

                sscanf(psz, "%i", &i);

                if (i > 2)
                {
                    i = 2;
                }
                else if (i < 0)
                {
                    i = 0;
                }

                SetRegValue(REG_KEY_GORE, i);
                CAnimate::iGoreLevel = i;
            }
            break;

        case CHEAT_INVERTMOUSE:
            {
                int         i;

                sscanf(psz, "%i", &i);

                if (i > 2)
                {
                    i = 2;
                }
                else if (i < 0)
                {
                    i = 0;
                }

                SetRegValue(REG_KEY_INVERTMOUSE, i);
                bInvertMouse = i ? true : false;
            }
            break;

        case CHEAT_WIN:
            wWorld.GameIsOver();
            break;

        case CHEAT_SORT:
            {
                char        sz[50];

                sscanf(psz, "%s", &sz);

                if (strcmpi(sz, "BTF") == 0)
                {
	                prenMain->pSettings->esSortMethod = esPresortBackToFront;
                }
                else if (strcmpi(sz, "FTB") == 0)
                {
                    prenMain->pSettings->esSortMethod = esPresortFrontToBack;
                }
            }
            break;

        case CHEAT_ALLAMMO:
            {
                extern bool g_bUnlimitedAmmo;
                g_bUnlimitedAmmo = !g_bUnlimitedAmmo;
            }
            break;

        case CHEAT_SLOMO:
            {
                if (CMessageStep::sMultiplier == 1.0f)
                {
                    CMessageStep::sMultiplier = 0.30f;
                }
                else
                {
                    CMessageStep::sMultiplier = 1.0f;
                }
            }
            break;
		case CHEAT_DINOS:
			{
				// Toggle the boring dino bit.
				gaiSystem.bBoring = !gaiSystem.bBoring;
			}
            break;

        case CHEAT_FPS:
            g_bDisplayFPS = !g_bDisplayFPS;
            break;
    }

    bRet = TRUE;

    return bRet;
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



CGameWnd::CGameWnd(CUIManager * puimgr) : CUIWnd(puimgr)
{
    g_CTPassGlobals.m_prasBkgnd = NULL;
    g_CTPassGlobals.m_prasMiniBkgnd = NULL;
    m_bPaused = FALSE;
    m_iClear = 0;
    m_pUIMgr->m_bDrawMouse = FALSE;
    m_puictlCheat = NULL;
	g_CTPassGlobals.bInGame = true;
	g_CTPassGlobals.bHardReset = true;
}

CGameWnd::~CGameWnd()
{
    delete g_CTPassGlobals.m_prasBkgnd;
    delete g_CTPassGlobals.m_prasMiniBkgnd;
    g_CTPassGlobals.m_prasBkgnd = NULL;
    g_CTPassGlobals.m_prasMiniBkgnd = NULL;
	g_CTPassGlobals.bInGame = false;
}


BOOL CGameWnd::OnCreate()
{
    int         i;

    CUIWnd::OnCreate();

	// Free cursor movement.
    ClipCursor(0);

	// Create a screen of the right size if required.
	{
		int i_width;
		int i_height;

		bGetDimensions(i_width, i_height);
		if (prasMainScreen->iWidthFront != i_width || prasMainScreen->iHeightFront != i_height)
			SetupGameScreen();
        else
        {
            int iGore = GetRegValue(REG_KEY_GORE, DEFAULT_GORE);
            if (iGore > 2)
            {
                SetRegValue(REG_KEY_GORE, 2);
                iGore = 2;
            }
            else if (iGore < 0)
            {
                SetRegValue(REG_KEY_GORE, 0);
                iGore = 0;
            }

            CAnimate::iGoreLevel = iGore;
            bInvertMouse = GetRegValue(REG_KEY_INVERTMOUSE, DEFAULT_INVERTMOUSE) ? true : false;
        }
	}

    g_CTPassGlobals.SetupBackground();
    RefreshAudioSettings();

    i = GetRegValue(REG_KEY_RENDERING_QUALITY, DEFAULT_RENDERING_QUALITY);
    SetQualitySetting(i);

    CMessageSystem(escSTART_SIM).Dispatch();

    m_puictlCheat = (CUIEditbox *)GetUICtrl(102);
    m_puictlCheat->SetPrompt(g_cheatPrompt);

    ResizeScreen(prasMainScreen->iWidthFront, prasMainScreen->iHeightFront);

    g_CTPassGlobals.InitGamma();

    return TRUE;
}

extern BOOL bQuitGame;

void CGameWnd::OnDestroy()
{
    RECT        rc;

    POINT screenSize = GetCurrentClientSize();
	
    SetRect(&rc, 0, 0, screenSize.x, screenSize.y);
    ClipCursor(&rc);

	if (!bQuitGame)
		prnshMain->bCreateScreen(screenSize.x, screenSize.y, 16, bGetSystemMem());

    CUIWnd::OnDestroy();
}


void CGameWnd::UIEditboxKey(CUICtrl * pctrl, UINT vk, int cRepeat, UINT flags)
{
    switch (vk)
    {
        case VK_ESCAPE:
        case VK_F11:
            ClearInputState(true);

            gpInputDeemone->Capture(true);
            m_puictlCheat->SetVisible(FALSE);
            m_iClear = 3;
            break;

        case VK_RETURN:
            {
                char    sz[255];
                LPCSTR   psz;

                psz = m_puictlCheat->GetText();
                strcpy(sz, psz);

                if (ExecuteCheat(sz))
                {
                    m_puictlCheat->SetText(g_cheatPrompt);
                }
            }
            break;

    }
}

void CGameWnd::OnChar(TCHAR ch, int cRepeat)
{
    if (m_puictlCheat->GetVisible())
    {
        m_puictlCheat->OnChar(ch, cRepeat);
        return;
    }

    switch (ch)
    {
        case ']':
            g_CTPassGlobals.RenderQualityUp();
            break;

        case '[':
            g_CTPassGlobals.RenderQualityDn();
            break;
    }
}


void CGameWnd::SetupGameStoppage()
{
    CMessageSystem      msg(escSTOP_SIM);

    // Stop the simulation
    msg.Dispatch();

    //Repaint in case we have a cleared framebuffer right now
    this->OnPaint(g_hwnd);

    m_bPaused = TRUE;
    m_pUIMgr->m_bDrawMouse = TRUE;

    int width = 0;
    int height = 0;
    GetWindowSize(g_hwnd, width, height);
	
	// Limit cursor movement.
	RECT rc;
	SetRect(&rc, 0, 0, width, height);
	ClipCursor(&rc);

    SetCursorPos(prasMainScreen->iWidth / 2, 
                 prasMainScreen->iHeight / 2);

    g_CTPassGlobals.CaptureBackground(true);
	g_CTPassGlobals.bHardReset = false;
}


void CGameWnd::ClearGameStoppage(BOOL bStartSim)
{
    RECT    rc;
    int     i;

    for (i = prasMainScreen->iBuffers; i > 0; i--)
    {
        SetRect(&rc, 0, 0, prasMainScreen->iWidthFront, prasMainScreen->iHeightFront);
        MyFillRect(&rc, 0, prasMainScreen.ptPtrRaw());
        prasMainScreen->Flip();
    }

    if (bStartSim)
    {
        CMessageSystem(escSTART_SIM).Dispatch();
        m_pUIMgr->m_bDrawMouse = FALSE;
    }

    m_bPaused = FALSE;

	// Free cursor movement.
	ClipCursor(0);

	// Center Mouse so we don't throw the user's view out of wack.
	// Copied code out of control.cpp so it will work on Voodoo cards.
	RECT rect;
	POINT point;
	GetClientRect(g_hwnd, &rect);
	point.x = rect.right/2;
	point.y = rect.bottom/2;
	ClientToScreen(g_hwnd, &point);
	SetCursorPos(point.x, point.y);
}


void CGameWnd::OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (m_puictlCheat->GetVisible())
    {
        if (fDown && (vk == VK_ESCAPE))
            return;
        
        m_puictlCheat->OnKey(vk, fDown, cRepeat, flags);
        return;
    }

    if (!fDown)
    {
    	if (vk == VK_ESCAPE) {
            CInGameOptionsWnd   dlg(m_pUIMgr);
            BOOL                bContinue = TRUE;

            SetupGameStoppage();

            // Popup the dialog
            dlg.StartUIWnd();

            if (dlg.m_dwExitValue == (DWORD)-1)
            {
                CQuitWnd        dlgQuit(m_pUIMgr);

                dlgQuit.StartUIWnd();

                switch (dlgQuit.m_dwExitValue)
                {
                    // Quit Game
                case 1:
                    m_pUIMgr->m_bDrawMouse = TRUE;
                    bContinue = FALSE;
                    bQuitGame = TRUE;
                    EndUIWnd(-1);
                    break;

                    // Quit Menu
                case 2:
                    EndUIWnd(1);
                    m_pUIMgr->m_bDrawMouse = TRUE;
                    bContinue = FALSE;
                    break;
                }
            }
            else if (dlg.m_dwExitValue == 3)
            {
                // restart the game with the Render dialog
                EndUIWnd(3);
            }

            if (dlg.m_dwExitValue == 3)
            {
                // If we are going to the render dialog then do not send a start
                // message otherwise audio will be started for a fraction of a
                // second and it sounds bad.
                ClearGameStoppage(false);
            }
            else
            {
                ClearGameStoppage(bContinue);
            }
        }
        return;
    }

    switch (vk)
    {
        case VK_SPACE:
            if (gpPlayer->bDead() && m_iNoticedDead > COUNT_NOTICED_DEAD)
            {
                SetupGameStoppage();

                CYesNoMsgDlg   dlg(IDS_RESTARTLEVEL, m_pUIMgr);

                dlg.StartUIWnd();
                if (dlg.m_dwExitValue != 0)
                {
                    g_CTPassGlobals.LoadLastScene();
                }

                ClearGameStoppage(TRUE);
            }
            break;

		case 0xbb:
			if (GetAsyncKeyState(VK_CONTROL) < 0)
			{
                ChangeViewportSize(-1, 0);
				break;
			}
			if (GetAsyncKeyState(VK_SHIFT) < 0)
			{
                ChangeViewportSize(0, -1);
				break;
			}
            ChangeViewportSize(-1, -1);
			break;

		case 0xbd:		// - and _
			if (GetAsyncKeyState(VK_CONTROL) < 0)
			{
                ChangeViewportSize(1, 0);
				break;
			}
			if (GetAsyncKeyState(VK_SHIFT) < 0)
			{
                ChangeViewportSize(0, 1);
				break;
			}
            ChangeViewportSize(1, 1);
			break;

        case VK_F1:
            {
                CHintWnd        dlg(m_pUIMgr);

                SetupGameStoppage();
                g_CTPassGlobals.CreateMenuAudioDatabase();
                g_CTPassGlobals.SetupButtonAudio();

                dlg.StartUIWnd();

                g_CTPassGlobals.FreeMenuAudio();
                ClearGameStoppage(TRUE);
            }
            break;

        case VK_F11:
			if (GetAsyncKeyState(VK_CONTROL) < 0)
			{
                if (m_puictlCheat->GetVisible())
                {
                    ClearInputState(true);

                    gpInputDeemone->Capture(true);

                    m_puictlCheat->SetVisible(FALSE);
                }
                else
                {
                    ClearInputState(false);

                    gpInputDeemone->Capture(false);
                    m_puictlCheat->SetVisible(TRUE);
                    m_puictlCheat->SetText(g_cheatPrompt);
                }

                m_iClear = 3;
            }
            break;

        case VK_F12:
            {
                CControlsWnd    dlg(m_pUIMgr);

                SetupGameStoppage();
                g_CTPassGlobals.CreateMenuAudioDatabase();
                g_CTPassGlobals.SetupButtonAudio();

                dlg.StartUIWnd();

                g_CTPassGlobals.FreeMenuAudio();
                ClearGameStoppage(TRUE);
            }
            break;


#if BUILDVER_MODE != MODE_FINAL
		case 'T':
		{
			if ((GetAsyncKeyState(VK_CONTROL) & (SHORT)0xFFFE) != 0)
			{
				extern void PlayerTeleportToNextLocation();
				PlayerTeleportToNextLocation();
			}
		}
#endif
    }
}


void CGameWnd::InnerLoopCall()
{
    if (m_bPaused)
    {
        return;
    }

    if (gpPlayer->bDead())
    {
        m_iNoticedDead++;
    }
    else
    {
        m_iNoticedDead = 0;
    }


    if (wWorld.bIsGameOver())
    {
		// Stop simulation.
		CMessageSystem(escSTOP_SIM).Dispatch();

        EndUIWnd((DWORD)-2);
    }

	// Check for pending save.
	if (wWorld.bIsSavePending())
	{
		const std::string &strName = wWorld.strGetPendingSave();

		// Stop simulation.
		CMessageSystem(escSTOP_SIM).Dispatch();

		// Save the level.
		g_CTPassGlobals.SaveGame(strName.c_str());

		// Start simulation.
		CMessageSystem(escSTART_SIM).Dispatch();
	}

	// Check for pending level load.
	if (wWorld.bIsLoadPending())
	{
		const std::string &strName = wWorld.strGetPendingLoad();

		// Stop simulation.
		CMessageSystem(escSTOP_SIM).Dispatch();

        MiddleMessage(IDS_LOADING_LEVEL);

		// Load a new level.
		g_CTPassGlobals.LoadLevel(strName.c_str());

		// Start simulation.
		CMessageSystem(escSTART_SIM).Dispatch();

		// Don't step just now.
		return;
	}

	gmlGameLoop.Step();
	InvalidateRect(NULL);
}


void CGameWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "gamewnd.ddf");
}


BOOL CGameWnd::OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    HDC     hdcSrc;

    if (!g_CTPassGlobals.m_prasBkgnd)
    {
        return FALSE;
    }

    hdcSrc = g_CTPassGlobals.m_prasBkgnd->hdcGet();

    BitBlt(hdc, 
           0, 
           0, 
           g_CTPassGlobals.m_prasBkgnd->iWidth, 
           g_CTPassGlobals.m_prasBkgnd->iHeight, 
           hdcSrc,
           0, 
           0, 
           SRCCOPY);

    g_CTPassGlobals.m_prasBkgnd->ReleaseDC(hdcSrc);

    return TRUE;
}


void CGameWnd::ResizeScreen(int iWidth, int iHeight)
{
    CUICtrl *   pctrl;
    RECT        rc;

    pctrl = GetUICtrl(100);
    if (pctrl)
    {
        pctrl->GetRect(&rc);
        rc.bottom = rc.bottom - rc.top;
        rc.top = prasMainScreen->iHeightFront - rc.bottom;
        rc.bottom += rc.top;
        pctrl->SetRect(&rc);
    }

    pctrl = GetUICtrl(101);
    if (pctrl)
    {
        pctrl->GetRect(&rc);
        rc.bottom = rc.bottom - rc.top;
        rc.right = rc.right - rc.left;
        rc.left = prasMainScreen->iWidthFront - rc.right;
        rc.right += rc.left;
        rc.bottom += rc.top;
        pctrl->SetRect(&rc);
    }

    pctrl = GetUICtrl(102);
    if (pctrl)
    {
        pctrl->GetRect(&rc);
        rc.bottom = rc.bottom - rc.top;
        rc.top = prasMainScreen->iHeightFront - rc.bottom;
        rc.bottom += rc.top;
        pctrl->SetRect(&rc);
    }
}


void CGameWnd::DrawWndInfo(CRaster * pRaster, RECT * prc)
{
    if (m_bPaused)
    {
        RasterBlt(g_CTPassGlobals.m_prasBkgnd, prc, pRaster, 0, 0, FALSE, 0);
    }
    else
    {
	    gmlGameLoop.Paint();

        if (m_puictlCheat->GetVisible() || m_iClear)
        {
            prasMainScreen->ClearBorder(true);
        }

        if (m_iClear > 0)
        {
            m_iClear--;
        }

	    prenMain->pScreenRender->EndFrame();

        if (g_bDisplayLocation)
        {
            char            sz[50];
            HDC             hdc;
			CCamera* pcam = CWDbQueryActiveCamera().tGet();
            CPlacement3<>   place = pcam->pr3Presence();
            CVector3<>      unit(0.0, 1.0, 0.0);

            sprintf(sz, 
                    "Location:  %.1f, %.1f, %.1f",
                    place.v3Pos.tX,
                    place.v3Pos.tY,
                    place.v3Pos.tZ);

            hdc = prasMainScreen->hdcGet();
            TextOut(hdc, 0, 15, sz, strlen(sz));
            prasMainScreen->ReleaseDC(hdc);

            unit = unit * place.r3Rot;
            sprintf(sz, 
                    "Facing:    %.1f, %.1f, %.1f",
                    unit.tX,
                    unit.tY,
                    unit.tZ);

            hdc = prasMainScreen->hdcGet();
            TextOut(hdc, 0, 30, sz, strlen(sz));
            prasMainScreen->ReleaseDC(hdc);

        }

        if (g_bDisplayFPS)
        {
            char sz[50];
            HDC hdc;

            if (dwTimeLastSample != 0)
            {
                DWORD dwTimeNow = timeGetTime();
                DWORD dwTimeElapsed = dwTimeNow - dwTimeLastSample;
                if ((dwTimeElapsed > 1000) && (iNumFramesThisSample > 0))
                {
                    float fTimeElapsed = (float)dwTimeElapsed / 1000.0f;
                    fFrameRate = iNumFramesThisSample / fTimeElapsed;
                    iNumFramesThisSample = 0;
                    dwTimeLastSample = dwTimeNow;
                }
            }
            else
            {
                dwTimeLastSample = timeGetTime();
            }
            iNumFramesThisSample++;

            sprintf(sz, 
                    "FPS: %i (%.2f ms)", 
                    (int)fFrameRate, 
                    1000.0f / fFrameRate);

            hdc = prasMainScreen->hdcGet();
            TextOut(hdc, 0, 0, sz, strlen(sz));
			prasMainScreen->ReleaseDC(hdc);
        }
    }
}



