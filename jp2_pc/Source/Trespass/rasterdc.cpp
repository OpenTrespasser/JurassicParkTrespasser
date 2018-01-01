//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1997.
//
//  File:       rasterdc.cpp
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    13-Nov-97   SHernd   Created
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "rasterdc.hpp"


extern rptr<CRasterWin> prasMainScreen;


CRasterDC::CRasterDC(HWND hwnd, int iWidthIn, int iHeightIn, int iBits)
{
    m_hwnd = hwnd;
    m_hDCScreen = ::GetDC(hwnd);

    Init(m_hDCScreen, iWidthIn, iHeightIn, iBits);
}

CRasterDC::CRasterDC(HDC hdcBase, int iWidthIn, int iHeightIn, int iBits)
{
    m_hwnd = NULL;
    m_hDCScreen = NULL;

    Init(hdcBase, iWidthIn, iHeightIn, iBits);
}

void CRasterDC::Init(HDC hdcBase, int iWidthIn, int iHeightIn, int iBits)
{
    LPBITMAPINFO lpInfo;
    DWORD *      pdw;
    DWORD        dwSize;

    m_hdc = NULL;
    iWidth  = iWidthIn;
    iHeight  = iHeightIn;
    iPixelBits = iBits;
    iLinePixels = iWidth;

    // create a bitmap header, with an identity palette, since
    // we will force the palette in exactly.
    dwSize = sizeof(BITMAPINFOHEADER);
    if (iBits == 16)
    {
        dwSize += sizeof(DWORD) * 3;
    }

    lpInfo = (LPBITMAPINFO)new BYTE[dwSize];

    lpInfo->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
    lpInfo->bmiHeader.biWidth    = iLinePixels;
    // set to negative for top down.
    lpInfo->bmiHeader.biHeight   = -iHeight;
    lpInfo->bmiHeader.biPlanes   = 1;
    lpInfo->bmiHeader.biBitCount = iBits;
    // leave other values as 0
    lpInfo->bmiHeader.biClrUsed  = 0;

    switch (iBits)
    {
        case 16:
            {
                lpInfo->bmiHeader.biCompression = BI_BITFIELDS;

                pdw = (DWORD*)&lpInfo->bmiColors;
                // Set the appropriate color mask for the BMP
                if (prasMainScreen->pxf.cposG.u1WidthDiff != 3)
                {
                    *pdw++ = 0xF800;
                    *pdw++ = 0x07E0;
                    *pdw++ = 0x001F;
                }
                else
                {
                    *pdw++ = 0x7C00;
                    *pdw++ = 0x03E0;
                    *pdw++ = 0x001F;
                }
            }
            break;

        case 24:
            lpInfo->bmiHeader.biCompression = BI_RGB;
            break;
    }


    // get a compatible DC if we do not have one. This
    // will store the DIBSection for bltting to the screen.
    if (m_hdc==NULL)
    {
        m_hdc = CreateCompatibleDC(hdcBase);
    }

    // assume screen width is divisble by 4 (32bits)
    // get HBITMAP for screen size
    m_hBmp = CreateDIBSection(m_hdc,
                              lpInfo,
                              DIB_RGB_COLORS,
                              (void **)&pSurface,
                              NULL,
                              0);

    delete [] (BYTE*)lpInfo;

    m_hBmpOld   = (HBITMAP)SelectObject(m_hdc, m_hBmp);
}



CRasterDC::~CRasterDC()
{
    if (m_hdc && m_hBmpOld) SelectObject (m_hdc, m_hBmpOld);
    DeleteObject(m_hBmp);
    DeleteDC (m_hdc);
    ::ReleaseDC(m_hwnd, m_hDCScreen);
}


void CRasterDC::Lock()
{
}


void CRasterDC::Unlock()
{
}


void CRasterDC::Clear(TPixel pix)
{
    WORD *  pw;
    int     x;
    int     y;
    int     iPitch;
    int     iBytes;
    int     iPitchDiff;

    iPitch = iLineBytes();
    iBytes = iPixelBytes();
    iPitchDiff = iLineBytes() - (iWidth * iBytes);

    pw = (WORD*)pSurface;
    for (y = 0; y < iHeight; y++)
    {
        for (x = 0; x < iWidth; x++)
        {
            *pw++ = pix;
        }

        pw += iPitchDiff;
    }
}


void CRasterDC::Blit(int idx, 
          int idy, 
          CRaster & rasSrc, 
          SRect* prect_src,
          bool b_clip, 
          bool b_colour_key,
          TPixel pix_colour_key)
{
}


HDC CRasterDC::hdcGet()
{
    return m_hdc;
}


void CRasterDC::ReleaseDC(HDC hdc)
{
}

