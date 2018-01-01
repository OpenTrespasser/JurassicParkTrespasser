//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1996.
//
//  File:       uiwnd.cpp
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    21-Nov-96   SHernd   Created
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "supportfn.hpp"
#include "uiwnd.h"
#include "main.h"
#include "..\Lib\Sys\reg.h"
#include "..\lib\sys\reginit.hpp"
#include "token.h"
#include "Lib/EntityDBase/Water.hpp"

extern HINSTANCE    g_hInst;
extern HWND         g_hwnd;

extern rptr<CRasterWin> prasMainScreen;


CUIManager::CUIManager()
{
    m_hwnd = NULL;
    m_bActive = FALSE;
    m_bPause = FALSE;
    m_bDrawMouse = TRUE;
    m_ptMouse.x = -1;
    m_ptMouse.y = -1;
    m_iMouse = 0;
    m_prasMouse[0] = NULL;
    m_prasMouse[1] = NULL;
    m_pUICtrlMouseCaptured = NULL;
}

CUIManager::~CUIManager()
{
    delete m_prasMouse[0];
    delete m_prasMouse[1];
}


BOOL CUIManager::Attach(CUIWnd * puiwnd)
{
    BOOL        bRet;

    m_vUIWnd.push_back(puiwnd);

    bRet = TRUE;

    return bRet;
}


BOOL CUIManager::Detach(CUIWnd * puiwnd)
{
    BOOL                            bRet = FALSE;
    vector<CUIWnd *>::iterator      i;

    for (i = m_vUIWnd.end(); i >= m_vUIWnd.begin(); i--)
    {
        if (puiwnd == *i)
        {
            bRet = TRUE;
            m_vUIWnd.erase(i);
        }
    }

    return bRet;
}


CUIWnd * CUIManager::GetActiveUIWnd()
{
    if (m_vUIWnd.size() == 0)
    {
        return NULL;
    }

    return m_vUIWnd.back();
}


void CUIManager::Draw()
{
    RECT    rc;

    SetRect(&m_rcInvalid, 
            0, 
            0, 
            prasMainScreen->iWidthFront, 
            prasMainScreen->iHeightFront);

    rc = m_rcInvalid;

    DrawWindowChain(prasMainScreen.ptPtrRaw(), &rc);

    DrawMouse(prasMainScreen.ptPtrRaw(), &rc);

    prasMainScreen->Flip();
}


void CUIManager::DrawWindowChain(LPBYTE pbDst,
                                 int iDstWidth,
                                 int iDstHeight,
                                 int iDstPitch,
                                 int iDstBytes,
                                 RECT * prcClip)
{
    vector<CUIWnd *>::iterator      i;

    if (m_vUIWnd.size() == 0)
    {
        return;
    }

    for (i = m_vUIWnd.begin(); i < m_vUIWnd.end() && i; i++)
    {
        (*i)->DrawIntoSurface(pbDst, 
                              iDstWidth, 
                              iDstHeight, 
                              iDstPitch, 
                              iDstBytes, 
                              prcClip);
    }
}


void CUIManager::DrawWindowChain(CRaster * pRaster, RECT * prcClip)
{
    vector<CUIWnd *>::iterator      i;

    if (m_vUIWnd.size() == 0)
    {
        return;
    }

    for (i = m_vUIWnd.begin(); i < m_vUIWnd.end() && i; i++)
    {
        (*i)->DrawIntoSurface(pRaster, prcClip);
    }
}


void CUIManager::ResizeScreen()
{
    vector<CUIWnd *>::iterator      i;
    int                             iWidth;
    int                             iHeight;

    iWidth = prasMainScreen->iWidthFront;
    iHeight = prasMainScreen->iHeightFront;

    for (i = m_vUIWnd.begin(); i < m_vUIWnd.end() && i; i++)
    {
        (*i)->ResizeScreen(iWidth, iHeight);
    }
}


void CUIManager::DrawMouse(LPBYTE pbDst,
                      int iDstWidth,
                      int iDstHeight,
                      int iDstPitch,
                      int iDstBytes,
                      RECT * prcClip)
{
    RECT        rcMouse;
    RECT        rc;
    RECT        rc2;
    CRaster *   prasMouse;

    if (!m_prasMouse[m_iMouse] || !m_bDrawMouse) 
    {
        return;
    }

    prasMouse = m_prasMouse[m_iMouse];

    SetRect(&rcMouse, 
            m_ptMouse.x, 
            m_ptMouse.y,
            m_ptMouse.x + prasMouse->iWidth,
            m_ptMouse.y + prasMouse->iHeight);

    if (!IntersectRect(&rc, &rcMouse, prcClip))
    {
        return;
    }

    ::SetRect(&rc2, 
              rc.left - rcMouse.left,
              rc.top - rcMouse.top,
              rc.right - rcMouse.left,
              rc.bottom - rcMouse.top);

    prasMouse->Lock();

    MyBlt((LPBYTE)prasMouse->pSurface,
          prasMouse->iLineBytes(),
          prasMouse->iPixelBytes(),
          rc2.left,
          rc2.top,
          rc2.right - rc2.left,
          rc2.bottom - rc2.top,
          pbDst,
          iDstPitch,
          iDstBytes,
          rc.left,
          rc.top,
          true,
          m_wTransColor);

    prasMouse->Unlock();
}


void CUIManager::DrawMouse(CRaster * pRaster, RECT * prcClip)
{
    RECT        rcMouse;
    RECT        rc;
    RECT        rc2;
    CRaster *   prasMouse;
    WORD        wTrans;

    if (!m_prasMouse[m_iMouse] || !m_bDrawMouse) 
    {
        return;
    }

    prasMouse = m_prasMouse[m_iMouse];

    SetRect(&rcMouse, 
            m_ptMouse.x, 
            m_ptMouse.y,
            m_ptMouse.x + prasMouse->iWidth,
            m_ptMouse.y + prasMouse->iHeight);

    if (!IntersectRect(&rc, &rcMouse, prcClip))
    {
        return;
    }

    ::SetRect(&rc2, 
              rc.left - rcMouse.left,
              rc.top - rcMouse.top,
              rc.right - rcMouse.left,
              rc.bottom - rcMouse.top);

    prasMouse->Lock();
    wTrans = *((WORD*)prasMouse->pSurface);
    prasMouse->Unlock();

    RasterBlt(prasMouse, 
              &rc2, 
              pRaster, 
              rc.left, 
              rc.top, 
              true, 
              wTrans);
}



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



CUIWnd::CUIWnd(CUIManager * pUIMgr)
{
    m_pUIMgr = pUIMgr;
    m_bExitWnd = false;
    m_dwExitValue = (DWORD)-1;
    m_pRaster = NULL;
    SetRect(&m_rc, 0, 0, 0, 0);
}


CUIWnd::~CUIWnd()
{
    delete m_pRaster;
}


void CUIWnd::GetWndFile(LPSTR psz, int ic)
{
    *psz = '\0';
}

BOOL CUIWnd::StartUIWnd()
{
    BOOL        bRet;

    if (!m_pUIMgr->Attach(this))
    {
        TraceError(("CUIWnd::StartUIWnd() -- "
                    "Unable to attach window"));
        goto Error;
    }

    if (!OnCreate())
    {
        TraceError(("CUIWnd::StartUIWnd() -- "
                    "UIWnd creation failed"));
        goto Error;
    }

    ClearInputState();

	CEntityWater::ResetLuts();

    WindowLoop();

    OnDestroy();

    m_pUIMgr->Detach(this);

    ClearInputState();

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CUIWnd::StartUIWnd"));
    bRet = FALSE;
    goto Cleanup;
}


CUICtrl * CUIWnd::CaptureMouse(CUICtrl * pctrl)
{
    CUICtrl *   pold = m_pUIMgr->m_pUICtrlMouseCaptured;

    m_pUIMgr->m_pUICtrlMouseCaptured = pctrl;

    return pold;
}


void CUIWnd::ReleaseMouse(CUICtrl * pctrl)
{
    if (m_pUIMgr->m_pUICtrlMouseCaptured == pctrl)
    {
        m_pUIMgr->m_pUICtrlMouseCaptured = NULL;
    }
}


void CUIWnd::InvalidateRect(RECT * prc)
{
    RECT        rc;
    RECT        rcIn;

    SetRect(&rc, 0, 0, prasMainScreen->iWidthFront, prasMainScreen->iHeightFront);

    if (prc == NULL)
    {
        rcIn = rc;
    }
    else
    {
        IntersectRect(&rcIn, &rc, prc);
    }

    UnionRect(&rc, &m_pUIMgr->m_rcInvalid, &rcIn);
    m_pUIMgr->m_rcInvalid = rc;
}


void CUIWnd::EndUIWnd(DWORD dwExitValue)
{
    m_bExitWnd = true;
    m_dwExitValue = dwExitValue;
}


void CUIWnd::OnActivateApp(BOOL fActivate, DWORD dwThreadId)
{
    bool    bState;
    RECT    rc;

    bState = m_pUIMgr->m_bActive;

    if (!fActivate)
    {
        if (m_pUIMgr->m_bActive)
        {
            m_pUIMgr->m_bPause = TRUE;

            // BUGBUG:  We may have to pause/halt audio and what not
        }

        m_pUIMgr->m_bActive = FALSE;
    }
    else
    {
        if (!m_pUIMgr->m_bActive)
        {
            m_pUIMgr->m_bPause = FALSE;

            // BUGBUG:  We may have to restart audio and what not
        }

        m_pUIMgr->m_bActive = TRUE;
    }

    GetWindowRect(m_pUIMgr->m_hwnd, &rc);
    InvalidateRect(&rc);
}

void CUIWnd::CheckMessages()
{
	AlwaysAssert(m_pUIMgr);
	BOOL bFoundMessage;

    do
    {
        MSG msg;
		bFoundMessage = PeekMessage(&msg,NULL,0,0,PM_REMOVE);

        if (bFoundMessage)
        {
			TranslateAccelerator(msg.hwnd, NULL, &msg);
            switch (msg.message)
            {
                default:
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                    break;

                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
					if (bIgnoreSysKey)
						continue;
                case WM_ACTIVATEAPP:
                case WM_ACTIVATE:
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
					continue;  // automatically check next message
							   // this helps prevent some weird behavior
							   // which happens if the delay between 
							   // processing WM_ACTIVATEAPP and WaitMessage
							   // is too long. We could potentially hit
							   // Alt-Tab again, which confuses the app
							   // making messages go to the Alt-Tab window
							   // instead of us.
                    break;
            }
        }

        if (IsIconic(g_hwnd) || !m_pUIMgr->m_bActive)
        {
            //WaitMessage();
			Sleep(25);
			bFoundMessage = TRUE;
        }
    }
    while (!m_pUIMgr->m_bActive || bFoundMessage);
}


void CUIWnd::PreLoopCall()
{
}


void CUIWnd::WindowLoop()
{
    InvalidateRect(NULL);

	// Clear the chain of buffers.
	d3dDriver.SetFlipClear(false);
	prasMainScreen->Clear(0);
	prasMainScreen->Flip();
	prasMainScreen->Clear(0);
	prasMainScreen->Flip();
	prasMainScreen->Clear(0);
	prasMainScreen->Flip();
	d3dDriver.SetFlipClear(true);

    do
    {
        PreLoopCall();

        GetCursorPos(&m_pUIMgr->m_ptMouse);
        CheckMessages();

        if (m_pUIMgr->m_bActive && !m_bExitWnd)
        {
            DoUIHandling();
            InnerLoopCall();
            m_pUIMgr->Draw();
        }
    }
    while (!m_bExitWnd);
}

void CUIWnd::InnerLoopCall()
{
}


BOOL CUIWnd::AddToUICtrlSet(CUICtrl * pctrl)
{
    m_vUICtrls.push_back(pctrl);
    return TRUE;
}

void CUIWnd::DoUIHandling()
{
    vector<CUICtrl*>::iterator      i;

    if (m_vUICtrls.size() == 0)
    {
        return;
    }

    for (i = m_vUICtrls.end() - 1; i >= m_vUICtrls.begin() && i; i--)
    {
        (*i)->DoFrame(m_pUIMgr->m_ptMouse);
    }
}


BOOL CUIWnd::OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    return FALSE;
}


void CUIWnd::OnPaint(HWND hwnd)
{
    RECT        rc;

    if (!prasMainScreen.ptPtrRaw())
    {
        return;
    }

    SetRect(&rc, 
            0, 
            0, 
            prasMainScreen->iWidthFront, 
            prasMainScreen->iHeightFront);
    DrawWndInfo(prasMainScreen.ptPtrRaw(), &rc);
    DrawUICtrls(prasMainScreen.ptPtrRaw(), &rc);
}


void CUIWnd::ResizeScreen(int iWidth, int iHeight)
{
}


void CUIWnd::DrawIntoSurface(LPBYTE pbDst, 
                             int iDstWidth, 
                             int iDstHeight,
                             int iDstPitch,
                             int iDstBytes,
                             RECT * prcClip)
{
    DrawWndInfo(pbDst, iDstWidth, iDstHeight, iDstPitch, iDstBytes, prcClip);
    DrawUICtrls(pbDst, iDstWidth, iDstHeight, iDstPitch, iDstBytes, prcClip);
}

// Draws into the Surface Buffer without drawing the mouse and without
// performing a flip
void CUIWnd::DrawIntoSurface(CRaster * pRaster, RECT * prcClip)
{
    DrawWndInfo(pRaster, prcClip);
    DrawUICtrls(pRaster, prcClip);
}


void CUIWnd::DrawWndInfo(LPBYTE pbDst,
                         int iDstWidth,
                         int iDstHeight,
                         int iDstPitch,
                         int iDstBytes,
                         RECT * prc)
{
    if (m_pRaster && pbDst)
    {
        RECT    rc;

        if (!IntersectRect(&rc, &m_rc, prc))
        {
            return;
        }

        m_pRaster->Lock();

        MyBlt((LPBYTE)m_pRaster->pSurface,
              m_pRaster->iLineBytes(),
              m_pRaster->iPixelBytes(),
              rc.left,
              rc.top,
              rc.right - rc.left,
              rc.bottom - rc.top,
              pbDst,
              iDstPitch,
              iDstBytes,
              rc.left,
              rc.top,
              FALSE,
              0,
              0);

        m_pRaster->Unlock();
    }
}


void CUIWnd::DrawWndInfo(CRaster * pRaster, RECT * prc)
{
    if (m_pRaster)
    {
        RECT    rc;
        RECT    rc2;

        if (!IntersectRect(&rc, &m_rc, prc))
        {
            return;
        }

        SetRect(&rc2, 
                rc.left - m_rc.left, 
                rc.top - m_rc.top,
                rc.right - rc.left,
                rc.bottom - rc.top);

        RasterBlt(m_pRaster, 
                  &rc2, 
                  pRaster,
                  rc.left,
                  rc.top,
                  FALSE,
                  0);
    }
}


void CUIWnd::DrawUICtrls(LPBYTE pbDst,
                         int iDstWidth,
                         int iDstHeight,
                         int iDstPitch,
                         int iDstBytes,
                         RECT * prc)
{
    vector<CUICtrl *>::iterator     i;

    if (m_pUIMgr->GetActiveUIWnd() != this)
    {
        return;
    }

    for (i = m_vUICtrls.begin(); i < m_vUICtrls.end() && i; i++)
    {
        (*i)->Draw(pbDst, iDstWidth, iDstHeight, iDstPitch, iDstBytes, prc);
    }
}


void CUIWnd::DrawUICtrls(CRaster * pRaster, RECT * prc)
{
    vector<CUICtrl *>::iterator     i;

    if (m_pUIMgr->GetActiveUIWnd() != this)
    {
        return;
    }

    for (i = m_vUICtrls.begin(); i < m_vUICtrls.end() && i; i++)
    {
        (*i)->Draw(pRaster, prc);
    }
}




BOOL CUIWnd::ReadUICtrl(HANDLE hFile)
{
    BOOL            bRet;
    CUICtrl *       puictrl = NULL;
    WNDFILETOKEN    wft;

    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType = LFTT_STRING);

    if (strcmpi(wft.val.sz, "BUTTON") == 0)
    {
        puictrl = new CUIButton(this);
    }
    else if (strcmpi(wft.val.sz, "listbox") == 0)
    {
        puictrl = new CUIListbox(this);
    }
    else if (strcmpi(wft.val.sz, "checkbox") == 0)
    {
        puictrl = new CUICheckbox(this);
    }
    else if (strcmpi(wft.val.sz, "textbox") == 0)
    {
        puictrl = new CUITextbox(this);
    }
    else if (strcmpi(wft.val.sz, "slider") == 0)
    {
        puictrl = new CUISlider(this);
    }
    else if (strcmpi(wft.val.sz, "editbox") == 0)
    {
        puictrl = new CUIEditbox(this);
    }
    else if (strcmpi(wft.val.sz, "static") == 0)
    {
        puictrl = new CUIStatic(this);
    }
    else if (strcmpi(wft.val.sz, "progress") == 0)
    {
        puictrl = new CUIProgressBar(this);
    }
    else if (strcmpi(wft.val.sz, "hotspot") == 0)
    {
        puictrl = new CUIHotspot(this);
    }

    if (puictrl == NULL)
    {
        TraceError(("CUIWnd::ReadUICtrl() -- OOM -- "
                    "unable to allocate UICtrl"));
        goto Error;
    }

    bRet = puictrl->TokenLoad(hFile);
    AddToUICtrlSet(puictrl);

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CUIWnd::ReadUICtrl"));
    bRet = FALSE;
    goto Cleanup;
}


BOOL CUIWnd::ReadBackground(HANDLE hFile)
{
    BOOL            bRet;
    WNDFILETOKEN    wft;

    // left
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType = LFTT_VALUE);
    m_rc.left = wft.val.iVal;

    // top
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType = LFTT_VALUE);
    m_rc.top = wft.val.iVal;

    // right
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType = LFTT_VALUE);
    m_rc.right = wft.val.iVal;

    // bottom
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType = LFTT_VALUE);
    m_rc.bottom = wft.val.iVal;

    // reading in the background image
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType = LFTT_STRING);

    if (strlen(wft.val.sz) > 0)
    {
        m_pRaster = ReadAndConvertBMP(wft.val.sz, false);
        if (m_pRaster)
        {
            m_rc.right = m_rc.left + m_pRaster->iWidth;
            m_rc.bottom = m_rc.top + m_pRaster->iHeight;
        }
    }

    bRet = TRUE;

    return bRet;
}


BOOL CUIWnd::HandleTokenRead(HANDLE hFile, WNDFILETOKEN * pwft)
{
    switch (pwft->TokenType)
    {
        case LFTT_BACKGROUND:
            ReadBackground(hFile);
            break;

        case LFTT_UICTRL:
            ReadUICtrl(hFile);
            break;
    }

    return TRUE;
}


BOOL CUIWnd::LoadWindowInfo()
{
    char            szFile[_MAX_PATH];
    char            szWndName[50];
    BOOL            bFinished;
    HANDLE          hFile = INVALID_HANDLE_VALUE;
    BOOL            bRet;
    WNDFILETOKEN    wft;

    GetWndFile(szWndName, sizeof(szWndName));

    //The Hack's hack to try and find the file first in the installed dir
    GetRegString(REG_KEY_INSTALLED_DIR, szFile, sizeof(szFile), "");
    if (szFile[strlen(szFile) - 1] != '\\')
    {
        strcat(szFile, "\\");
    }

    strcat(szFile, "menu\\");
    strcat(szFile, szWndName);

    hFile = CreateFile(szFile,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        TraceError(("CUIWnd::LoadWindowInfo() -- "
                    "Unable to open file %s from Install Dir. Trying Data Dir.",
                    szFile));

        // BUGBUG:  Hack to get data drive location
        GetRegString(REG_KEY_DATA_DRIVE, szFile, sizeof(szFile), "");
        if (szFile[strlen(szFile) - 1] != '\\')
        {
            strcat(szFile, "\\");
        }

        strcat(szFile, "data\\menu\\");
        strcat(szFile, szWndName);

        hFile = CreateFile(szFile,
                           GENERIC_READ,
                           FILE_SHARE_READ,
                           NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            TraceError(("CUIWnd::LoadWindowInfo() -- "
                        "Unable to open file %s",
                        szFile));
            goto Error;
        }
    }

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
            HandleTokenRead(hFile, &wft);
        }
    }

    bRet = TRUE;

Cleanup:    
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
    }

    return bRet;

Error:
    TraceError(("CUIWnd::LoadWindowInfo()"));
    bRet = FALSE;
    goto Cleanup;
}


BOOL CUIWnd::OnCreate()
{
    vector<CUIWnd *>::iterator  iuiwnd;
    vector<CUICtrl*>::iterator  iuictrl;

    BOOL        bRet;
	POINT		pt;

    iuiwnd = m_pUIMgr->m_vUIWnd.end();
    iuiwnd--;

    if (iuiwnd >= m_pUIMgr->m_vUIWnd.begin())
    {
        for (iuictrl = (*iuiwnd)->m_vUICtrls.begin();
             iuictrl < (*iuiwnd)->m_vUICtrls.end() && iuictrl;
             iuictrl++)
        {
            pt.x = pt.y = -1;
            (*iuictrl)->DoFrame(pt);
            (*iuictrl)->ReleaseCapture();
        }
    }

    if (!LoadWindowInfo())
    {
        TraceError(("CGameWnd::OnCreate() -- Loading Window Info failed"));
        goto Error;
    }

    InvalidateRect(NULL);

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CGameWnd::OnCreate"));
    bRet = FALSE;
    goto Cleanup;
}

void CUIWnd::OnDestroy()
{
    vector<CUICtrl*>::iterator      iuictrl;

    InvalidateRect(NULL);

    CaptureMouse(NULL);

    // Walk through all the UIControls deleting them
    for (iuictrl = m_vUICtrls.begin(); 
         iuictrl < m_vUICtrls.end();
         iuictrl++)
    {
        delete *iuictrl;
    }
}


CUICtrl * CUIWnd::GetUICtrl(DWORD dwID)
{
    vector<CUICtrl*>::iterator      iuictrl;

    for (iuictrl = m_vUICtrls.begin(); 
         iuictrl < m_vUICtrls.end();
         iuictrl++)
    {
        if ((*iuictrl)->GetID() == dwID)
        {
            return *iuictrl;
        }
    }

    return NULL;
}

void CUIWnd::OnCommand(int id, HWND hwndCtl, UINT codeNotify)
{
}

void CUIWnd::OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
}

void CUIWnd::OnChar(TCHAR tch, int cRepeat)
{
}

void CUIWnd::OnMouseMove(int x, int y, UINT keyFlags)
{
}

void CUIWnd::OnLButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    vector<CUICtrl*>::iterator          i;

    if (m_vUICtrls.size() == 0)
    {
        return;
    }

    for (i = m_vUICtrls.end() - 1; i >= m_vUICtrls.begin() && i; i--)
    {
        if (*i && 
            (*i)->HitTest(m_pUIMgr->m_ptMouse.x, m_pUIMgr->m_ptMouse.y) &&
            (*i)->LButtonDown(m_pUIMgr->m_ptMouse.x,
                              m_pUIMgr->m_ptMouse.y,
                              fDoubleClick,
                              keyFlags))
        {
            return;
        }
    }
}

void CUIWnd::OnLButtonUp(int x, int y, UINT keyFlags)
{
    if (m_pUIMgr->m_pUICtrlMouseCaptured)
    {
        m_pUIMgr->m_pUICtrlMouseCaptured->LButtonUp(x, y, keyFlags);
    }
    else
    {
        vector<CUICtrl*>::iterator          i;

        if (m_vUICtrls.size() == 0)
        {
            return;
        }

        for (i = m_vUICtrls.end() - 1; i >= m_vUICtrls.begin() && i; i--)
        {
            if (*i && 
                (*i)->HitTest(m_pUIMgr->m_ptMouse.x, m_pUIMgr->m_ptMouse.y) &&
                (*i)->LButtonUp(m_pUIMgr->m_ptMouse.x,
                                m_pUIMgr->m_ptMouse.y,
                                keyFlags))
            {
                return;
            }
        }
    }
}

void CUIWnd::OnRButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
}

void CUIWnd::OnMButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
}


void CUIWnd::OnTimer(UINT uiID)
{
}

BOOL CUIWnd::bIgnoreSysKey = FALSE;

