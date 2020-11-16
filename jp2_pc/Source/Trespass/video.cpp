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
#include "video.h"
#include "../Lib/Sys/reg.h"
#include "../lib/sys/reginit.hpp"
#include "Lib/Sys/DWSizeStruct.hpp"


extern HINSTANCE    g_hInst;
extern HWND		    g_hwnd;


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



CVideoWnd::CVideoWnd(CUIManager * puimgr) : CUIWnd(puimgr)
{
    m_szFile[0] = '\0';
    m_pSmack = NULL;
    m_fVideoOver = false;
    m_iLastKey = 0;
    m_fDirect = FALSE;
}


CVideoWnd::~CVideoWnd()
{
}


void CVideoWnd::NextDirect()
{
    IDirectDrawSurface4 *   pSurface;
    HDC                     hdc;
    HRESULT                 hr;

    if (m_pSmack->NewPalette)
    {
        SmackBufferNewPalette(m_pBuf, m_pSmack->Palette,(u16)m_pSmack->PalType);
        SmackColorRemap(m_pSmack,m_pBuf->Palette,m_pBuf->MaxPalColors,m_pBuf->PalType);
    }

    pSurface = prasMainScreen->GetPrimarySurface();
    do
    {
        hr = pSurface->GetDC(&hdc);
        prasMainScreen->bRestore(hr);
    }
    while (hr == DDERR_SURFACELOST);

    SmackDoFrame(m_pSmack);

    while (SmackToBufferRect(m_pSmack, m_pBuf->SurfaceType))
    {
        if (SmackBufferBlit(m_pBuf,
                            hdc,
                            m_iLeft,
                            m_iTop,
                            m_pSmack->LastRectx,
                            m_pSmack->LastRecty,
                            m_pSmack->LastRectw,
                            m_pSmack->LastRecth))
        {
            break;
        }
    }

    pSurface->ReleaseDC(hdc);
}


void CVideoWnd::NextNonDirect()
{
    LPBYTE      pbBits;
    LPBYTE      pb;
    int         y;
    int         iSrcPitch;
    int         iDstPitch;
    LPBYTE      pbSrc;
    LPBYTE      pbDst;
    int         iSurface;
    IDirectDrawSurface4 *   pSurface;
    CDDSize<DDSURFACEDESC2> dds;
    HRESULT                 hr;

    pSurface = prasMainScreen->GetPrimarySurface();
    do
    {
        hr = pSurface->Lock(NULL, 
                            &dds, 
                            DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, 
                            NULL);
        prasMainScreen->bRestore(hr);
    }
    while (hr == DDERR_SURFACELOST);

    pbBits = (LPBYTE)dds.lpSurface;
    m_pBuff->Lock();
    pb = (LPBYTE)m_pBuff->pSurface;

    SmackToBuffer(m_pSmack, 
                  m_iLeft, 
                  m_iTop, 
                  m_pBuff->iLineBytes(), 
                  m_pBuff->iHeight, 
                  pb, 
                  m_iSurfaceType);
    SmackDoFrame(m_pSmack);

    iSrcPitch = m_pBuff->iLineBytes();
    iDstPitch = dds.lPitch;

    if (m_bFullRedraw)
    {
        iSurface = SMACKSURFACESLOW;
        m_bFullRedraw = false;
    }
    else
    {
        iSurface = SMACKSURFACEFAST;
    }

    iSurface |= m_iSurfaceType;

    while (SmackToBufferRect(m_pSmack, SMACKSURFACESLOW/*iSurface*/))
    {
        if (m_pSmack->LastRectw)
        {
            int h;
            int w;
            int x;

            x = m_pSmack->LastRectx;
            y = m_pSmack->LastRecty;
            w = m_pSmack->LastRectw;
            h = m_pSmack->LastRecth;

            pbSrc = pb + (x * 2) + (iSrcPitch * y);
            pbDst = pbBits + (x * 2) + (iDstPitch * y);
            w *= 2;

            for (y = 0; y < h; y++)
            {
                memcpy(pbDst, pbSrc, w);
                pbSrc += iSrcPitch;
                pbDst += iDstPitch;
            }
        }
    }

    m_pBuff->Unlock();
    pSurface->Unlock(nullptr);
}


void CVideoWnd::NextSmackerFrame()
{
    if (m_fDirect)
    {
        NextDirect();
    }
    else
    {
        NextNonDirect();
    }

    if (m_pSmack->FrameNum == m_pSmack->Frames - 1)
    {
        m_fVideoOver = true;
    }
    else
    {
        SmackNextFrame(m_pSmack);
    }
}


void CVideoWnd::Pause()
{
}


void CVideoWnd::Resume()
{
    if (m_iLastKey)
    {
        Trace(("Restoring to Key Frame %i", m_iLastKey));
        SmackSoundOnOff(m_pSmack, 0);
        SmackGoto(m_pSmack, m_iLastKey);
        SmackSoundOnOff(m_pSmack, 1);
    }
}


BOOL CVideoWnd::Play(LPCSTR pszFile)
{
    HANDLE          hfile = INVALID_HANDLE_VALUE;
    char            sz[50];
    bool            bAudio;

    if (GetRegValue(REG_KEY_NOVIDEO, DEFAULT_REG_NOVIDEO))
    {
        MSG   msg;

        if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return TRUE;
    }

    strcpy(sz, pszFile);

    m_pUIMgr->Attach(this);

    SmackSoundUseDirectSound(CAudio::pcaAudio->pdsGetDSInterface());

    if (prasMainScreen->pxf.cposG.u1WidthDiff != 3)
    {
        m_iSurfaceType = SMACKBUFFER565;
    }
    else
    {
        m_iSurfaceType = SMACKBUFFER555;
    }

    strcpy(sz, pszFile); 
    strcat(sz, ".smk");

    hfile = FileAbstraction(sz);
    if (hfile == INVALID_HANDLE_VALUE)
    {
#ifdef _DEBUG
        dprintf("CVideoWnd::Play() -- Unable to load/find %s", sz);
#endif
        m_pUIMgr->Detach(this);
    	return FALSE;
    }

    m_pSmack = SmackOpen((char *)hfile, 
                         SMACKTRACKS | SMACKFILEHANDLE | SMACKNEEDVOLUME,
                         SMACKAUTOEXTRA);
    if (!m_pSmack)
    {
        m_pUIMgr->Detach(this);
        CloseHandle(hfile);
        return FALSE;
    }

    Trace(("Smack open flags   0x%08x", m_pSmack->OpenFlags));
    Trace(("      Track 0 info 0x%08x", m_pSmack->TrackType[0]));

    bAudio = CAudio::pcaAudio->bAudioActive();
    SmackSoundOnOff(m_pSmack, bAudio);

    //
    // Determine now if we need to go direct to the primary surface via the 
    // HDC or if the normal back buffer is sufficient.
    //

    {
        IDirectDrawSurface4 *   pSurface;
        CDDSize<DDSURFACEDESC2> dds;
        HRESULT                 hr;

        pSurface = prasMainScreen->GetPrimarySurface();
		
		do
		{
			hr = pSurface->Lock(NULL, 
								&dds, 
								DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, 
								NULL);
			prasMainScreen->bRestore(hr);
		}
		while (hr == DDERR_SURFACELOST);

        if ((LPBYTE)dds.lpSurface == NULL)
        {
            Trace(("Video going Direct to primary surface"));
            m_fDirect = TRUE;
        }
        else
        {
            m_fDirect = FALSE;
        }

        pSurface->Unlock(nullptr);
    }

    POINT screenSize = GetCurrentClientSize();
	
    if (m_fDirect)
    {
        m_pBuf = SmackBufferOpen(g_hwnd, 
                               SMACKAUTOBLIT,
                               m_pSmack->Width,
                               m_pSmack->Height,
                               m_pSmack->Width,
                               m_pSmack->Height);
        if (!m_pBuf)
        {
            TraceError(("CVideoWnd::Play() -- "
                        "Unable to open smacker buffer"));
            goto Cleanup;
        }
    }
    else
    {
	    m_pBuff = rptr_new CRasterMem(screenSize.x, screenSize.y, 16, 0);

        m_pBuff->Clear(0);

        m_bFullRedraw = TRUE;
    }

    // Determine Left Top for Centering the video
    m_iLeft = (screenSize.x / 2) - (m_pSmack->Width / 2);
    m_iTop = (screenSize.y / 2) - (m_pSmack->Height / 2);

    if (m_fDirect)
    {
        m_pBuf->SurfaceType = SMACKSURFACESLOW;
        SmackToBuffer(m_pSmack,
                      (m_pBuf->Width - m_pSmack->Width) / 2,
                      (m_pBuf->Height - m_pSmack->Height) /2,
                      m_pBuf->Pitch,
                      m_pBuf->Height,
                      m_pBuf->Buffer,
                      (u8)m_pBuf->Reversed);
    }

	d3dDriver.SetFlipClear(false);
	prasMainScreen->Clear(0);
	prasMainScreen->Flip();
	prasMainScreen->Clear(0);
	prasMainScreen->Flip();
	prasMainScreen->Clear(0);
	prasMainScreen->Flip();
	d3dDriver.SetFlipClear(true);

    while (!m_fVideoOver)
    {
        MSG   msg;

        if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            switch (msg.message)
            {
                case WM_KEYUP:
                    if (msg.wParam == VK_SNAPSHOT)
                    {
                        ScreenCapture();
                        break;
                    }
                    
                    if (m_pSmack->FrameNum > 8)
                    {
                        m_fVideoOver = true;
                    }
                    break;

                case WM_LBUTTONUP:
                case WM_RBUTTONUP:
                    if (m_pSmack->FrameNum > 8)
                    {
                        m_fVideoOver = true;
                    }
                    break;

                default:
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                    break;
            }
        }
        else if (!SmackWait(m_pSmack) && !IsMinimized(g_hwnd))
        {
            NextSmackerFrame();
        }
    }

#if _DEBUG
    {
        SmackSum ss;
        SmackSummary(m_pSmack,&ss);

        Trace(("Video Summary"));
        Trace(("fps:     %i", ss.TotalFrames*1000/ss.TotalTime));
        Trace(("decomp %% %i", ss.TotalDecompTime*100/ss.TotalTime));
        Trace(("blit   %% %i", ss.TotalBlitTime*100/ss.TotalTime));
        Trace(("read   %% %i", ss.TotalReadTime*100/ss.TotalTime));
    }
#endif

Cleanup:
    if (m_fDirect)
    {
        SmackBufferClose(m_pBuf);
    }

    SmackClose(m_pSmack);
    CloseHandle(hfile);

    m_pUIMgr->Detach(this);

    return TRUE;
}


void CVideoWnd::OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (fDown)
    {
        return;
    }

    switch (vk)
    {
        case VK_ESCAPE:
        case VK_SPACE:
            if (m_pSmack->FrameNum > 8)
            {
                m_fVideoOver = true;
            }
            break;
    }
}


