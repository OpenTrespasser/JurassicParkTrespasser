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


#include "supportfn.hpp"
#include "video.h"
#include "../Lib/Sys/reg.h"
#include "../Lib/Sys/RegInit.hpp"
#include "Lib/Sys/DWSizeStruct.hpp"

#include "../../lib/libsmacker-1.1.1/smacker.h"

#include <thread>
#include <chrono>

#ifndef DSBCAPS_CTRLDEFAULT
#define DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2)
#endif


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



CVideoWnd::CVideoWnd(CUIManager* puimgr) : CUIWnd(puimgr)
{
    m_szFile[0] = '\0';
    m_pSmack = NULL;
    m_fVideoOver = false;
    m_fVideoLoaded = false;
    m_audioState = { 0 };
    m_audioState.m_playingFirstHalf = true;
    m_iLastKey = 0;
}


CVideoWnd::~CVideoWnd()
{
}

void CVideoWnd::NextImageFrame()
{
    if (m_frameQueue.empty() || !m_pVideoFrameBuf)
        return;

    auto& current = m_frameQueue.front();

    const auto& palette = current.videoPalette;
    const auto& videoframe = current.video;


    m_pVideoFrameBuf->Lock();
    auto numPixels = m_pVideoFrameBuf->iWidth * m_pVideoFrameBuf->iHeight;

    //Three variants to avoid branching within the loop
    if (m_pVideoFrameBuf->iPixelBytes() == 4) {
        for (int i = 0; i < numPixels; i++) {
            BYTE b = palette[videoframe[i]].values[0];
            BYTE g = palette[videoframe[i]].values[1];
            BYTE r = palette[videoframe[i]].values[2];

            uint32 combined = (b << 16) | (g << 8) | (r << 0);
            m_pVideoFrameBuf->PutPixel(i, combined);
        }
    }
    else { //16bit color depth
        if (prasMainScreen->pxf.cposG.u1WidthDiff != 3) {
            for (int i = 0; i < numPixels; i++) {
                BYTE b = palette[videoframe[i]].values[0];
                BYTE g = palette[videoframe[i]].values[1];
                BYTE r = palette[videoframe[i]].values[2];

                uint32 combined = MAKE565(b, g, r);
                m_pVideoFrameBuf->PutPixel(i, combined);
            }
        }
        else {
            for (int i = 0; i < numPixels; i++) {
                BYTE b = palette[videoframe[i]].values[0];
                BYTE g = palette[videoframe[i]].values[1];
                BYTE r = palette[videoframe[i]].values[2];

                uint32 combined = MAKE555(b, g, r);
                m_pVideoFrameBuf->PutPixel(i, combined);
            }
        }
    }

    m_pVideoFrameBuf->Unlock();

    prasMainScreen->Clear(0);
    prasMainScreen->Blit(m_iLeft, m_iTop, *m_pVideoFrameBuf);


    m_frameQueue.pop_front();

#if 0
//Copypasted code for the FPS overlay
//Helpful for measuring the correct timing
//TODO: deduplicate and make enabling FPS encounter available via config
    {
        char sz[50];
        HDC hdc;

        static float fFrameRate = 0;           // Framerate in frames per second, calculated once per second
        static int iNumFramesThisSample = 0;   // Number of frames rendered in the current sample period
        static DWORD dwTimeLastSample = 0;     // Time when the framerate was last calculated
        static DWORD dwTimeLastFrame = 0;      // The time when the previous frame was rendered
        static float fTimeElapsed = 0;         // Milliseconds elapsed since the last frame was rendered

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
        dout << sz << std::endl;
    }
#endif
}

bool CVideoWnd::CreateSoundBuffer()
{
    unsigned char trackmask = 0;
    unsigned char channels[7] = { 0 };
    unsigned char bitdepth[7] = { 0 };
    unsigned long bitrate[7] = { 0 };

    smk_info_audio(m_pSmack, &trackmask, channels, bitdepth, bitrate);


    WAVEFORMATEX pcmwf = { 0 };

    pcmwf.wFormatTag = WAVE_FORMAT_PCM;
    pcmwf.nChannels = channels[0];
    pcmwf.nSamplesPerSec = bitrate[0];
    pcmwf.wBitsPerSample = bitdepth[0];
    //pcmwf.nBlockAlign = 2;
    pcmwf.nBlockAlign = (pcmwf.nChannels * pcmwf.wBitsPerSample) / 8;
    pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
    pcmwf.cbSize = 0;

    m_audioState.m_silenceByte = pcmwf.wBitsPerSample == 8 ? 128 : 0;
    Assert(m_framerate != 0);
    m_audioState.m_avgBytesPerSec = pcmwf.nAvgBytesPerSec / m_framerate;

    // prepare to create sounds buffer
    CDDSize<DSBUFFERDESC> dsbd;
    dsbd.dwFlags = DSBCAPS_CTRLDEFAULT;
    dsbd.dwBufferBytes = pcmwf.nAvgBytesPerSec * 4; //4 seconds worth of data
    dsbd.dwReserved = 0;
    dsbd.guid3DAlgorithm = GUID_NULL;
    dsbd.lpwfxFormat = &pcmwf;

    m_audioState.m_soundBufferSize = dsbd.dwBufferBytes;


    // create the sound buffer
    HRESULT result = CAudio::pcaAudio->pdsGetDSInterface()->CreateSoundBuffer(&dsbd, &m_soundbuffer, nullptr);
    if (FAILED(result))
    {
        return false;
    }


    return true;
}

bool CVideoWnd::WriteChunkIntoAudioBuffer(DWORD& lastWriteOffset)
{
    if (m_audioQueue.empty() || !m_soundbuffer)
        return false;

    const auto current = m_audioQueue.front(); //Intentional copy, do not use reference type
    m_audioQueue.pop_front(); //Because reference would become dangling here
    const auto size = current.size();


    void* audio_ptr_1 = nullptr;
    void* audio_ptr_2 = nullptr;

    DWORD audio_length_1 = 0;
    DWORD audio_length_2 = 0;

    if (FAILED(m_soundbuffer->Lock(lastWriteOffset, size,
        &audio_ptr_1, &audio_length_1,
        &audio_ptr_2, &audio_length_2,
        DSBLOCK_ENTIREBUFFER)))
    {
        return false;
    }

    size_t firstsize = 0;
    size_t secondsize = 0;
    if (size <= audio_length_1) {
        firstsize = size;
        lastWriteOffset += size;
    }
    else
    {
        firstsize = audio_length_1;
        secondsize = size - audio_length_1;
        lastWriteOffset = secondsize;
    }

    std::memcpy(audio_ptr_1, current.data(), firstsize);

    // copy to beginning of circular buffer if there is leftover data
    if (secondsize) {
        Assert(secondsize <= audio_length_2);
        if (secondsize > audio_length_2)
            puts("BREAK HERE");
        const auto copysize = std::min(secondsize, static_cast<size_t>(audio_length_2));
        std::memcpy(audio_ptr_2, (current.data() + firstsize), copysize);
    }

    if (FAILED(m_soundbuffer->Unlock(audio_ptr_1, audio_length_1,
        audio_ptr_2, audio_length_2)))
    {
        return false;
    }

    return true;
}

bool CVideoWnd::ServiceAudioBuffer(bool force)
{
    //The audio buffer is used as a ring buffer
    //We virtually divide it into the first and second half
    //One half is currently being played
    //The other half can be written to


    DWORD playOffset = 0;
    DWORD writeOffset = 0;
    DWORD& lastWriteOffset = m_audioState.m_lastWriteOffset;

    if (!m_soundbuffer)
        return false;

    if (m_audioState.m_audioFinished) {
        return true;
    }

    const DWORD margin = m_audioState.m_soundBufferSize / 2;
    const DWORD eighth = margin / 4;
    bool& playingFirstHalf = m_audioState.m_playingFirstHalf;

    if (FAILED(m_soundbuffer->GetCurrentPosition(&playOffset, &writeOffset)))
    {
        return false;
    }

    if (m_audioState.m_audioWriteFinished && playOffset >= m_audioState.m_finalWriteOffset) {
        m_soundbuffer->Stop();
        m_audioState.m_audioFinished = true;
    }


    if (force) {
        while (lastWriteOffset < margin)
        {
            if (!WriteChunkIntoAudioBuffer(lastWriteOffset))
                return false;
        }
        return true;
    }

    //If playing position has switched half
    if ((writeOffset <= margin) != playingFirstHalf)
    {
        while (playingFirstHalf ? lastWriteOffset > margin : lastWriteOffset <= margin) {
            if (!WriteChunkIntoAudioBuffer(lastWriteOffset)) {
                m_audioState.m_audioWriteFinished = true;
                m_audioState.m_finalWriteOffset = lastWriteOffset;

                //Write a few frames worth of silence into the buffer
                //So that playback does not stumble upon old data before it stops
                for (int i = 0; i < 5; i++) {
                    m_audioQueue.emplace_back(m_audioState.m_avgBytesPerSec, m_audioState.m_silenceByte);
                    WriteChunkIntoAudioBuffer(lastWriteOffset);
                }

                return false;
            }
        }

        playingFirstHalf = writeOffset <= margin;

        return true;
    }
    else
        return true;
}

void CVideoWnd::LoadFrameIntoQueues()
{
    if (m_soundbuffer) {
        auto& audio = m_audioQueue.emplace_back();
        audio.resize(smk_get_audio_size(m_pSmack, 0));
        std::memcpy(audio.data(), smk_get_audio(m_pSmack, 0), audio.size());
    }
    {
        auto& frame = m_frameQueue.emplace_back();
        std::memcpy(frame.videoPalette.data(), smk_get_palette(m_pSmack), 256 * 3);
        std::memcpy(frame.video.data(), smk_get_video(m_pSmack), m_pVideoFrameBuf->iWidth * m_pVideoFrameBuf->iHeight);
    }
}

void CVideoWnd::NextSmackerFrame()
{
    NextImageFrame();
    ServiceAudioBuffer();

    if (smk_next(m_pSmack) != SMK_MORE)
    {
        m_fVideoLoaded = true;
    }
    if (!m_fVideoLoaded)
    {
        LoadFrameIntoQueues();
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
    }
}

BOOL CVideoWnd::Play(LPCSTR pszFile)
{
    HANDLE          hfile = INVALID_HANDLE_VALUE;
    char            sz[MAX_PATH + 1];
    bool            bAudio;

    if (GetRegValue(REG_KEY_NOVIDEO, DEFAULT_REG_NOVIDEO))
    {
        MSG   msg;

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return TRUE;
    }

    strcpy(sz, pszFile);

    m_pUIMgr->Attach(this);



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
    CloseHandle(hfile);


    char fullpath[MAX_PATH] = { '\0' };
    GetFileLoc(FA_DATADRIVE, fullpath, sizeof(fullpath));
    strcat(fullpath, sz);

    m_pSmack = smk_open_file(fullpath, SMK_MODE_MEMORY);
    if (!m_pSmack)
    {
        m_pUIMgr->Detach(this);
        return FALSE;
    }

    unsigned long width = 0;
    unsigned long height = 0;
    double usf = 0.0; //USF = microseconds per frame
    unsigned long framecount = 0;
    smk_info_all(m_pSmack, nullptr, &framecount, &usf);
    smk_info_video(m_pSmack, &width, &height, nullptr);
    m_framerate = static_cast<int>(1000000.0 / usf);

    bAudio = CAudio::pcaAudio->bAudioActive();
    if (bAudio)
    {
        if (!CreateSoundBuffer())
        {
            m_pUIMgr->Detach(this);
            return FALSE;
        }
        smk_enable_audio(m_pSmack, 0, true);
    }
    else
        m_soundbuffer = nullptr;
    smk_enable_video(m_pSmack, true);




    m_pVideoFrameBuf = std::make_unique<CRasterVid>(width, height, prasMainScreen->iPixelBits);

    POINT screenSize = GetCurrentClientSize();
    // Determine Left Top for Centering the video
    m_iLeft = (screenSize.x / 2) - (width / 2);
    m_iTop = (screenSize.y / 2) - (height / 2);


    smk_first(m_pSmack);


    //Prefetch three seconds worth of video
    for (size_t i = 0; i < std::min(m_framerate * 3ul, framecount); i++) {
        LoadFrameIntoQueues();
        if (smk_next(m_pSmack) != SMK_MORE)
            break;
    }
    for (size_t i = 0; i < std::min(m_framerate * 3ul, framecount); i++)
        ServiceAudioBuffer(true);


    d3dDriver.SetFlipClear(false);
    prasMainScreen->Clear(0);
    prasMainScreen->Flip();
    prasMainScreen->Clear(0);
    prasMainScreen->Flip();
    prasMainScreen->Clear(0);
    prasMainScreen->Flip();
    d3dDriver.SetFlipClear(true);

    m_soundbuffer->Play(0, 0, DSBPLAY_LOOPING);

    while (!m_fVideoOver)
    {
        auto start = std::chrono::high_resolution_clock::now();

        MSG   msg;

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            switch (msg.message)
            {
            case WM_KEYUP:
                if (msg.wParam == VK_SNAPSHOT)
                {
                    ScreenCapture();
                    break;
                }

                m_fVideoOver = true;
                break;

            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
                m_fVideoOver = true;
                break;


            default:
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                break;
            }
        }

        NextSmackerFrame();
        prasMainScreen->Flip();

        //Sleeping for a few milliseconds has unprecise timing
        //So a lesser form of busy waiting is used
        while (std::chrono::high_resolution_clock::now() - start < std::chrono::microseconds(static_cast<long>(usf)))
            std::this_thread::sleep_for(std::chrono::milliseconds(0));
        if (m_fVideoLoaded && m_audioState.m_audioFinished)
            m_fVideoOver = true;
    }


Cleanup:
    smk_close(m_pSmack);
    if (m_soundbuffer)
        m_soundbuffer->Stop();

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
        /*if (m_pSmack->FrameNum > 8)
        {
                m_fVideoOver = true;
        }*/
        m_fVideoOver = true;
        break;
    }
}


