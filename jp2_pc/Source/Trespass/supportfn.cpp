//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1997.
//
//  File:       supportfn.cpp
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

#include "supportfn.hpp"
#include "..\Lib\Sys\reg.h"
#include "..\lib\sys\reginit.hpp"
#include "rasterdc.hpp"
#include "main.h"
#include "uiwnd.h"
#include "uidlgs.h"
#include "cdib.h"
#include "resource.h"
#include "Lib/View/DisplayMode.hpp"

#include <filesystem>

extern HINSTANCE    g_hInst;
extern HWND         g_hwnd;
extern CMainWnd *	g_pMainWnd;
extern bool         bInvertMouse;


void ForceShowCursor(BOOL bShow)
{
    static int     ic = 0;

    if (!bShow && ic >= 0)
    {
        do
        {
            ic = ShowCursor(FALSE);
        }
        while (ic >= 0);
    }
    else if (bShow && ic < 0)
    {
        do
        {
            ic = ShowCursor(TRUE);
        }
        while (ic < 0);
    }
}



BOOL PASCAL MemDibRead(VOID * pv, VOID * pbDst, DWORD dwSize, DWORD * pdwRead)
{
    MEMDIBINFO *    pmemdib = (MEMDIBINFO*)pv;

    if (pmemdib->pbCurr - pmemdib->pbBase > pmemdib->dwSize)
    {
        dwSize = 0;
        return TRUE;
    }

    if ((pmemdib->pbCurr - pmemdib->pbBase) + dwSize > pmemdib->dwSize)
    {
        dwSize = pmemdib->dwSize - (pmemdib->pbCurr - pmemdib->pbBase);
    }

    memcpy(pbDst, pmemdib->pbCurr, dwSize);

    pmemdib->pbCurr += dwSize;
    
    if (pdwRead)
    {
        *pdwRead = dwSize;
    }

    return TRUE;
}

BOOL PASCAL MemDibWrite(VOID * pv, VOID * pbSrc, DWORD dwSize, DWORD * pdwWritten)
{
    MEMDIBINFO *    pmemdib = (MEMDIBINFO*)pv;

    if (pmemdib->pbCurr - pmemdib->pbBase > pmemdib->dwSize)
    {
        return FALSE;
    }

    if ((pmemdib->pbCurr - pmemdib->pbBase) + dwSize > pmemdib->dwSize)
    {
        dwSize = pmemdib->dwSize - (pmemdib->pbCurr - pmemdib->pbBase);
    }

    memcpy(pmemdib->pbCurr, pbSrc, dwSize);

    pmemdib->pbCurr += dwSize;
    
    if (pdwWritten)
    {
        *pdwWritten = dwSize;
    }

    return TRUE;
}

BOOL PASCAL MemDibSeek(VOID * pv, DWORD dwMove, DWORD seek, DWORD * pNewOffset)
{
    MEMDIBINFO *    pmemdib = (MEMDIBINFO*)pv;
    DWORD           dwOffset;

    switch (seek)
    {
        case FILE_BEGIN:
            dwOffset = (DWORD)pmemdib->pbBase + dwMove;
            break;

        case FILE_CURRENT:
            dwOffset = (DWORD)pmemdib->pbCurr + (LONG)dwMove;
            break;

        case FILE_END:
            if (dwMove > (DWORD)pmemdib->dwSize)
            {
                dwOffset = 0;
            }
            else
            {
                dwOffset = (DWORD)(pmemdib->pbBase + pmemdib->dwSize) - dwMove;
            }
            break;
    }

    if ((dwOffset >= 0) && (dwOffset < pmemdib->dwSize))
    {
        pmemdib->pbCurr = pmemdib->pbBase + dwOffset;
    }
    else
    {
        return FALSE;
    }

    if (pNewOffset)
    {
        *pNewOffset = dwOffset;
    }

    return TRUE;
}



int GetFileLoc(FA_TYPE faFileLoc, char * psz, int icSize)
{
    Assert(psz);

    switch (faFileLoc)
    {
        case FA_DATADRIVE:
            GetRegString(REG_KEY_DATA_DRIVE, psz, icSize, "");
    		strcat(psz, "\\data\\");
            break;

        case FA_INSTALLDIR:
            GetRegString(REG_KEY_INSTALLED_DIR, psz, icSize, "");
            strcat(psz, "\\");
            break;
    }

    return strlen(psz);
}


CRaster * ConvertBMP(CDib * pdib, bool bRasterDC)
{
    int                 iBits;
    bool                b565 = false;
    BYTE                bRed;
    BYTE                bGreen;
    BYTE                bBlue;
    bool                bTopDown;
    WORD *              pw;
    int                 x;
    int                 y;
    int                 iDstPitch;
    BYTE *              pb;
    CRaster *           pras;

    if (prasMainScreen->pxf.cposG.u1WidthDiff != 3)
    {
        b565 = true;
        iBits = 16;
    }
    else
    {
        b565 = false;
        iBits = 15;
    }

#if 0
	rptr<CRaster>		rptrras;
    rptrras = rptr_cast(CRaster, rptr_new CRasterMem(pdib->GetWidth(), pdib->GetHeight(), 16, 0));
#endif

    if (bRasterDC)
    {
        pras = new CRasterDC(g_hwnd, pdib->GetWidth(), pdib->GetHeight(), 16);    
    }
    else
    {
	    pras = new CRasterMem(pdib->GetWidth(), pdib->GetHeight(), 16, 0);
    }

    if (!pras)
    {
        TraceError(("Error:  ConvertBMP() -- Error creating Raster"));
        Assert(FALSE);
        return NULL;
    }

    if (pdib->GetOrientation() == -1)
    {
        bTopDown = true;
    }
    else
    {
        bTopDown = false;
    }

    pras->Lock();

    iDstPitch = pras->iLineBytes(); //((pras->iWidth * (pras->iPixelBits / 8)) + 3) & ~0x03;

    for (y = 0; y < pdib->GetHeight(); y++)
    {
        if (bTopDown)
        {
            pb = pdib->GetPixelPointer(0, y);
        }
        else
        {
            pb = pdib->GetPixelPointer(0, pdib->GetHeight() - (y + 1));
        }

        pw = (WORD*)((BYTE*)pras->pSurface + (y * iDstPitch));

        for (x = 0; x < pdib->GetWidth(); x++)
        {
            RGBFROM24(pb, bRed, bGreen, bBlue);

            if (b565)
            {
                *pw = MAKE565(bRed, bGreen, bBlue);
            }
            else
            {
                *pw = MAKE555(bRed, bGreen, bBlue);
            }

            pw++;
        }
    }

    pras->Unlock();

    return pras;
}


CRaster * ReadAndConvertBMP(LPSTR pszFile, bool bRasterDC, FA_TYPE faFileLoc)
{
    char        szSource[_MAX_PATH];
    CDib *      pdib = NULL;
    CRaster *   pras;

    GetFileLoc(faFileLoc, szSource, sizeof(szSource));
    strcat(szSource, pszFile);

    if (CDib::DibFromFile(szSource, &pdib) < 0)
    {
        Trace(("Error:  Unable to open/read file %s. Looking elsewhere.", szSource));

        //just being extra careful since we're now going to look for resources on the
        //hard drive first.
        if (faFileLoc == FA_INSTALLDIR)
        {
            faFileLoc = FA_DATADRIVE;
        }
        else
        {
            faFileLoc = FA_INSTALLDIR;
        }

        GetFileLoc(faFileLoc, szSource, sizeof(szSource));
        strcat(szSource, pszFile);

        if (CDib::DibFromFile(szSource, &pdib) < 0)
        {
            Trace(("Error:  Unable to open/read file %s", szSource));
            return NULL;
        }
    }

    pras = ConvertBMP(pdib, bRasterDC);

    delete pdib;

    return pras;
}


void CenterUIWindow(CUIWnd * puiwnd)
{
    RECT            rcOld;
    RECT            rcClient;
    int             x;
    int             y;

    if (!puiwnd)
    {
        return;
    }

    rcOld = puiwnd->m_rc;

    // Center the dialog over the base window
    SetRect(&rcClient, 
            0, 
            0, 
            prasMainScreen->iWidthFront,
            prasMainScreen->iHeightFront);

    x = (rcClient.left + ((rcClient.right - rcClient.left) / 2)) - 
        ((rcOld.right - rcOld.left) / 2);
    y = (rcClient.top + ((rcClient.bottom - rcClient.top) / 2)) - 
        ((rcOld.bottom - rcOld.top) / 2);

    // Turn X,Y into offsets
    x -= rcOld.left;
    y -= rcOld.top;

    // Move the base dialog
    OffsetRect(&puiwnd->m_rc, x, y);

    // Move the UI controls
    if (puiwnd->m_vUICtrls.size() == 0)
    {
        return;
    }

    std::vector<CUICtrl *>::iterator      i;

    for (i = puiwnd->m_vUICtrls.begin(); i < puiwnd->m_vUICtrls.end() && *i; i++)
    {
        (*i)->GetRect(&rcClient);
        OffsetRect(&rcClient, x, y);
        (*i)->SetRect(&rcClient);
    }
}


// BUGBUG: This shit needs to be replaced once we have a file
// BUGBUG: abstraction layer
HANDLE FileAbstraction(LPCSTR psz, FA_TYPE faFileLoc)
{
    HANDLE  hfile;
    char    szSource[_MAX_PATH];

    GetFileLoc(faFileLoc, szSource, sizeof(szSource));

    strcat(szSource, psz);

    hfile = CreateFile(szSource,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);
#if _DEBUG
    if (hfile == INVALID_HANDLE_VALUE)
    {
        Trace(("FileAbstraction() -- unable to find/load %s", szSource));
    }
#endif

    return hfile;
}



WORD GetColor(BYTE bRed, BYTE bGreen, BYTE bBlue)
{
    if (prasMainScreen->pxf.cposG.u1WidthDiff != 3)
    {
        return MAKE565(bRed, bGreen, bBlue);
    }

    return MAKE555(bRed, bGreen, bBlue);
}


void SeperateColor(WORD wColor, BYTE & bRed, BYTE & bGreen, BYTE & bBlue)
{
    if (prasMainScreen->pxf.cposG.u1WidthDiff != 3)
    {
        RGBFROM565(wColor, bRed, bGreen, bBlue);
        return;
    }

    RGBFROM555(wColor, bRed, bGreen, bBlue);
}


void MyFillRect(LPRECT prc,
                WORD wColor,
                LPBYTE pbDst, 
                int iWidth,
                int iHeight,
                int iPitch)
{
    WORD *  pw;
    int     x;

    if (prc->left < 0)
    {
        prc->left = 0;
    }
    if (prc->right > iWidth)
    {
        prc->right = iWidth;
    }

    if (prc->top < 0)
    {
        prc->top = 0;
    }
    if (prc->bottom > iHeight)
    {
        prc->bottom = iHeight;
    }

    iWidth = prc->right - prc->left;
    if ((iWidth <= 0) || ((prc->bottom - prc->top) <= 0))
    {
        return;
    }

    for (iHeight = prc->top; iHeight < prc->bottom; iHeight++)
    {
        pw = (WORD*)(pbDst + (prc->left * 2) + (iHeight * iPitch));
        for (x = 0; x < iWidth; x++)
        {
            *pw++ = wColor;
        }
    }
}


void MyFillRect(RECT * prc, WORD wColor, CRaster * pRaster)
{
    int     iWidth;
    int     iHeight;

	CRasterWin * prasw = dynamic_cast<CRasterWin*>(pRaster);
	if (prasw)
    {
        iWidth = ((CRasterWin*)pRaster)->iWidthFront;
        iHeight = ((CRasterWin*)pRaster)->iHeightFront;
        ((CRasterWin*)pRaster)->LockAll();
    }
    else
    {
        pRaster->Lock();
        iWidth = pRaster->iWidth;
        iHeight = pRaster->iHeight;
    }

    MyFillRect(prc, 
               wColor, 
               (LPBYTE)pRaster->pSurface,
               iWidth,
               iHeight,
               pRaster->iLineBytes());

    pRaster->Unlock();
}


void MySunkenRect(RECT * prc, 
                  WORD wUp, 
                  WORD wDown, 
                  CRaster * pRaster, 
                  RECT * prcClip)
{
    RECT        rc2;
    RECT        rcPaint;

    // Draw top line
    rc2.left = prc->left;
    rc2.top = prc->top;
    rc2.right = prc->right;
    rc2.bottom = rc2.top + 1;
    if (prcClip && IntersectRect(&rcPaint, &rc2, prcClip))
    {
        MyFillRect(&rcPaint, wUp, pRaster);
    }
    else if (!prcClip)
    {
        MyFillRect(&rc2, wUp, pRaster);
    }

    // Draw right line
    rc2.left = prc->right - 1;
    rc2.top = prc->top;
    rc2.right = rc2.left + 1;
    rc2.bottom = prc->bottom;
    if (prcClip && IntersectRect(&rcPaint, &rc2, prcClip))
    {
        MyFillRect(&rcPaint, wDown, pRaster);
    }
    else if (!prcClip)
    {
        MyFillRect(&rc2, wDown, pRaster);
    }

    // Draw bottom line
    rc2.left = prc->left;
    rc2.top = prc->bottom - 1;
    rc2.right = prc->right;
    rc2.bottom = rc2.top + 1;
    if (prcClip && IntersectRect(&rcPaint, &rc2, prcClip))
    {
        MyFillRect(&rcPaint, wDown, pRaster);
    }
    else if (!prcClip)
    {
        MyFillRect(&rc2, wDown, pRaster);
    }

    // Draw left line
    rc2.left = prc->left;
    rc2.top = prc->top;
    rc2.right = rc2.left + 1;
    rc2.bottom = prc->bottom;
    if (prcClip && IntersectRect(&rcPaint, &rc2, prcClip))
    {
        MyFillRect(&rcPaint, wUp, pRaster);
    }
    else if (!prcClip)
    {
        MyFillRect(&rc2, wUp, pRaster);
    }
}


void MyFrameRect(RECT * prc, WORD wFill, LPBYTE pbDst, int iWidth, int iHeight, int iPitch)
{
    RECT        rc2;

    // Draw top line
    rc2.left = prc->left;
    rc2.top = prc->top;
    rc2.right = prc->right;
    rc2.bottom = rc2.top + 1;
    MyFillRect(&rc2, wFill, pbDst, iWidth, iHeight, iPitch);

    // Draw right line
    rc2.left = prc->right - 1;
    rc2.top = prc->top;
    rc2.right = rc2.left + 1;
    rc2.bottom = prc->bottom;
    MyFillRect(&rc2, wFill,  pbDst, iWidth, iHeight, iPitch);

    // Draw bottom line
    rc2.left = prc->left;
    rc2.top = prc->bottom - 1;
    rc2.right = prc->right;
    rc2.bottom = rc2.top + 1;
    MyFillRect(&rc2, wFill,  pbDst, iWidth, iHeight, iPitch);

    // Draw left line
    rc2.left = prc->left;
    rc2.top = prc->top;
    rc2.right = rc2.left + 1;
    rc2.bottom = prc->bottom;
    MyFillRect(&rc2, wFill,  pbDst, iWidth, iHeight, iPitch);
}


void MyFrameRect(RECT * prc, WORD wFill, CRaster * pRaster)
{
    RECT        rc2;

    // Draw top line
    rc2.left = prc->left;
    rc2.top = prc->top;
    rc2.right = prc->right;
    rc2.bottom = rc2.top + 1;
    MyFillRect(&rc2, wFill, pRaster);

    // Draw right line
    rc2.left = prc->right - 1;
    rc2.top = prc->top;
    rc2.right = rc2.left + 1;
    rc2.bottom = prc->bottom;
    MyFillRect(&rc2, wFill, pRaster);

    // Draw bottom line
    rc2.left = prc->left;
    rc2.top = prc->bottom - 1;
    rc2.right = prc->right;
    rc2.bottom = rc2.top + 1;
    MyFillRect(&rc2, wFill, pRaster);

    // Draw left line
    rc2.left = prc->left;
    rc2.top = prc->top;
    rc2.right = rc2.left + 1;
    rc2.bottom = prc->bottom;
    MyFillRect(&rc2, wFill, pRaster);
}



WORD ChangeIllum(WORD w, double dIllum, bool b555)
{
    BYTE        bRed;
    BYTE        bGreen;
    BYTE        bBlue;
    WORD        wRet;
    int         i;

    if (b555)
    {
        RGBFROM565(w, bRed, bGreen, bBlue);
    }
    else
    {
        RGBFROM555(w, bRed, bGreen, bBlue);
    }

    // Do the illuminescence change
    i = (int)((double)bRed * dIllum);
    if (i < 0)
    {
        i = 0;
    }
    else if (i > 255)
    {
        i = 255;
    }
    bRed = (BYTE)i;

    i = (int)((double)bGreen * dIllum);
    if (i < 0)
    {
        i = 0;
    }
    else if (i > 255)
    {
        i = 255;
    }
    bGreen = (BYTE)i;

    i = (int)((double)bBlue * dIllum);
    if (i < 0)
    {
        i = 0;
    }
    else if (i > 255)
    {
        i = 255;
    }
    bBlue = (BYTE)i;

    if (b555)
    {
        wRet = MAKE565(bRed, bGreen, bBlue);
    }
    else
    {
        wRet = MAKE555(bRed, bGreen, bBlue);
    }

    return wRet;
}



void MyBlt(LPBYTE pbSrc,
           int iSrcPitch,
           int iSrcBytes,
           int xSrc,
           int ySrc,
           int iWidth,
           int iHeight,
           LPBYTE pbDst,
           int iDstPitch,
           int iDstBytes,
           int xDst,
           int yDst,
           bool bColorKey,
           int iColorKey,
           int iIllum)
{
    int     iSrcDiff;
    int     iDstDiff;
    int     x;
    int     y;
    int     iWidthBytes;

    pbSrc = pbSrc + 
            (xSrc * iSrcBytes) +
            (ySrc * iSrcPitch);
    iSrcDiff = iSrcPitch - (iWidth * iSrcBytes);

    pbDst = pbDst +
            (xDst * iDstBytes) +
            (yDst * iDstPitch);
    iDstDiff = iDstPitch - (iWidth * iDstBytes);

    // We are going to assume that the surfaces are the same bit depth
    Assert(iSrcBytes == iDstBytes);
    iWidthBytes = iWidth * iDstBytes;

    // only do the illumination stuff if we are in 16bpp mode
    if (iIllum && iDstBytes == 2)
    {
        bool        b565;
        double      dIllum;
            
        if (iIllum > 0)
        {
            dIllum = 1 + ((double)iIllum / 100.0);
        }
        else
        {
            dIllum = (double)abs(iIllum) / 100.0;
        }

        if (prasMainScreen->pxf.cposG.u1WidthDiff != 3)
        {
            b565 = true;
        }
        else
        {
            b565 = false;
        }

        if (bColorKey)
        {
            for (y = 0; y < iHeight; y++)
            {
                for (x = 0; x < iWidth; x++)
                {
                    if (*(WORD*)pbSrc != (WORD)iColorKey)
                    {
                        *(WORD*)pbDst = ChangeIllum(*(WORD*)pbSrc, dIllum, b565);
                    }

                    pbSrc += iSrcBytes;
                    pbDst += iSrcBytes;
                }

                pbDst += iDstDiff;
                pbSrc += iSrcDiff;
            }
        }
        else
        {
            for (y = 0; y < iHeight; y++)
            {
                for (x = 0; x < iWidth; x++)
                {
                    *(WORD*)pbDst = ChangeIllum(*(WORD*)pbSrc, dIllum, b565);
                    pbSrc += iSrcBytes;
                    pbDst += iSrcBytes;
                }

                pbDst += iDstDiff;
                pbSrc += iSrcDiff;
            }
        }
    }
    else if (bColorKey)
    {
        BYTE *  pbDstBase = pbDst;
        BYTE *  pbSrcBase = pbSrc;
        
        for (y = 0; y < iHeight; y++)
        {
            pbDst = pbDstBase;
            pbSrc = pbSrcBase;

            for (x = 0; x < iWidth; x++)
            {

                if (*(WORD*)pbSrc != (WORD)iColorKey)
                {
                    *(WORD*)pbDst = *(WORD*)pbSrc;
                }

                pbSrc += iSrcBytes;
                pbDst += iSrcBytes;
            }

            pbDstBase += iDstPitch;
            pbSrcBase += iSrcPitch;
        }
    }
    else
    {
        //Use smaller size when buffer sizes do not match
        size_t copysize = iSrcBytes == iDstBytes ? iWidthBytes : iWidth * std::min(iDstBytes, iSrcBytes);
        
        for (y = 0; y < iHeight; y++)
        {
            memcpy(pbDst, pbSrc, copysize);
            pbDst += iDstPitch;
            pbSrc += iSrcPitch;
        }
    }
}


void RasterBlt(CRaster * pRSrc,
               RECT * prcSrc,
               CRaster * pRDst,
               int xDst,
               int yDst,
               bool bColorKey,
               int iColorKey,
               int iIllum)
{
    int     iWidth;
    int     iHeight;
    int     iDstWidth;
    int     iDstHeight;
    int     xSrc;
    int     ySrc;

    Assert(prcSrc);
    Assert(pRSrc);
    Assert(pRDst);

	CRasterWin* prasw = dynamic_cast<CRasterWin*>(pRSrc);
	if (prasw)
    {
        ((CRasterWin*)pRSrc)->LockAll();
    }
    else
    {
        pRSrc->Lock();
    }

	prasw = dynamic_cast<CRasterWin*>(pRDst);
	if (prasw)
    {
        ((CRasterWin*)pRDst)->LockAll();
        iDstWidth = ((CRasterWin*)pRDst)->iWidthFront;
        iDstHeight = ((CRasterWin*)pRDst)->iHeightFront;
    }
    else
    {
        pRDst->Lock();
        iDstWidth = pRDst->iWidth;
        iDstHeight = pRDst->iHeight;
    }

    xSrc = prcSrc->left;
    ySrc = prcSrc->top;
    iWidth = prcSrc->right - prcSrc->left;
    iHeight = prcSrc->bottom - prcSrc->top;

    if (xDst < 0)
    {
        iWidth += xDst;
        xSrc -= xDst;
        xDst = 0;
    }

    if (yDst < 0)
    {
        iHeight += yDst;
        ySrc -= yDst;
        yDst = 0;
    }

    // Check for left side and bottom clipping
    if (xDst + iWidth >= iDstWidth)
    {
        iWidth = iDstWidth - xDst;
    }

    if (yDst + iHeight >= iDstHeight)
    {
        iHeight = iDstHeight - yDst;
    }


    MyBlt((LPBYTE)pRSrc->pSurface, 
          pRSrc->iLineBytes(),
          pRSrc->iPixelBytes(),
          xSrc,
          ySrc,
          iWidth,
          iHeight,
          (LPBYTE)pRDst->pSurface,
          pRDst->iLineBytes(),
          pRDst->iPixelBytes(),
          xDst,
          yDst,
          bColorKey,
          iColorKey,
          iIllum);

    pRSrc->Unlock();
    pRDst->Unlock();
}


CCamera* pcamGetCamera()
{
	CCamera* pcam = CWDbQueryActiveCamera().tGet();
	Assert(pcam);
	return pcam;
}

void SetupGameScreen()
{
    int             iWindowWidth;
    int             iWindowHeight;
    int             iClientWidth;
    int             iClientHeight;
    BOOL            bSystemMem;
    RECT            rc;
    int             iGore;

    bGetDimensions(iWindowWidth, iWindowHeight);
	
	if (GetWindowModeConfigured() == WindowMode::FRAMED)
	{
        POINT clientsize = GetCurrentClientSize();
        iClientWidth = clientsize.x;
        iClientHeight = clientsize.y;
	}
    else 
    {
        iClientWidth = iWindowWidth;
        iClientHeight = iWindowHeight;
    }
	
    bSystemMem = bGetSystemMem();

    SetRect(&rc, 0, 0, iClientWidth, iClientHeight);
    ClipCursor(&rc);

    prnshMain->bCreateScreen(iClientWidth, 
                             iClientHeight, 
                             16, 
                             bSystemMem);

	
    SetWindowPos(g_hwnd, NULL, -1, -1, iWindowWidth, iWindowHeight,  
                 SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);
                 

	d3dDriver.Purge();
	d3dDriver.Restore();

	// Setup the default alpha light values.
	lbAlphaConstant.Setup(prasMainScreen.ptPtrRaw());
	lbAlphaTerrain.CreateBlend(prasMainScreen.ptPtrRaw(), clrDefEndDepth);
	lbAlphaWater.CreateBlendForWater(prasMainScreen.ptPtrRaw());
	abAlphaTexture.Setup(prasMainScreen.ptPtrRaw());

    iGore = GetRegValue(REG_KEY_GORE, DEFAULT_GORE);
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

    ChangeViewportSize(0, 0, false);
}

void ChangeViewportSize(int iX, int iY, bool bRedraw /* = true */)
{
    int     iRegX;
    int     iRegY;
    int     iXInc;
    int     iYInc;
    int     iWidth;
    int     iHeight;

    if (bRedraw)
    {
	    prasMainScreen->Clear(0);
	    prasMainScreen->Flip();

	    prasMainScreen->Clear(0);
	    prasMainScreen->Flip();

	    prasMainScreen->Clear(0);
	    prasMainScreen->Flip();
    }

    iXInc = (((float)prasMainScreen->iWidthFront * 0.66f) / VIEWPORT_INCREMENTS_FLOAT);
    iYInc = (((float)prasMainScreen->iHeightFront * 0.66f) / VIEWPORT_INCREMENTS_FLOAT);

    iRegX = GetRegValue(REG_KEY_VIEWPORT_X, DEFAULT_VIEWPORT_X) + iX;
    if (iRegX >= VIEWPORT_INCREMENTS)
    {
        iRegX = VIEWPORT_INCREMENTS - 1;
    }
    else if (iRegX < 0)
    {
        iRegX = 0;
    }
    SetRegValue(REG_KEY_VIEWPORT_X, iRegX);

    iRegY = GetRegValue(REG_KEY_VIEWPORT_Y, DEFAULT_VIEWPORT_Y) + iY;
    if (iRegY >= VIEWPORT_INCREMENTS)
    {
        iRegY = VIEWPORT_INCREMENTS - 1;
    }
    else if (iRegY < 0)
    {
        iRegY = 0;
    }
    SetRegValue(REG_KEY_VIEWPORT_Y, iRegY);

    iWidth = prasMainScreen->iWidthFront - (iRegX * iXInc);
    iHeight = prasMainScreen->iHeightFront - (iRegY * iYInc);

    prnshMain->AdjustViewport(iWidth, iHeight, g_hwnd, bRedraw);
}



void RefreshAudioSettings()
{
    BOOL        b;
    bool        b_hw;

	if (CAudio::pcaAudio)
	{
		b_hw = (bool)GetRegValue(REG_KEY_AUDIO_ENABLE3D, DEFAULT_AUDIO_ENABLE3D);
		CAudio::pcaAudio->UseHardware(b_hw);
	}

	if (padAudioDaemon)
	{
		b = GetRegValue(REG_KEY_AUDIO_EFFECT, DEFAULT_AUDIO_EFFECT);
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_EFFECT, b ? AUDIO_FEATURE_EFFECT : 0);

		b = GetRegValue(REG_KEY_AUDIO_AMBIENT, DEFAULT_AUDIO_AMBIENT);
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_AMBIENT, b ? AUDIO_FEATURE_AMBIENT : 0);

		b = GetRegValue(REG_KEY_AUDIO_VOICEOVER, DEFAULT_AUDIO_VOICEOVER);
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_VOICEOVER, b ? AUDIO_FEATURE_VOICEOVER : 0);

		b = GetRegValue(REG_KEY_AUDIO_MUSIC, DEFAULT_AUDIO_MUSIC);
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_MUSIC, b ? AUDIO_FEATURE_MUSIC : 0);

		b = GetRegValue(REG_KEY_AUDIO_SUBTITLES, DEFAULT_AUDIO_SUBTITLES);
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_SUBTITLES, b ? AUDIO_FEATURE_SUBTITLES : 0);
	}
}

//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

BOOL GetNextAvailName(LPSTR pszFileName, 
                      int icFileLen, 
                      LPSTR pszPrefix,
                      LPSTR pszExt)
{
    char        szPath[_MAX_PATH];
    char        szBase[_MAX_PATH];
    int         iLen;
    BOOL        bFound;
    int         iBaseExt;

    GetFileLoc(FA_INSTALLDIR, szBase, sizeof(szBase));
    strcat(szBase, pszPrefix);

    iBaseExt = 0;
    bFound = FALSE;
    strcpy(szPath, szBase);
    iLen = strlen(szPath);

    while (!bFound)
    {
        szPath[iLen] = '\0';
        wsprintf(&szPath[iLen], "%03i.%s", iBaseExt++, pszExt);
        if (GetFileAttributes(szPath) == 0xFFFFFFFF)
        {
            bFound = TRUE;
            strcpy(pszFileName, szPath);
        }
    }

    return bFound;
}



void ScreenCapture()
{
    CDib                    dib;
    CRasterDC *             pras24;
    HDC                     hdcSrc;
    HDC                     hdcDst;
    LPBYTE                  pbSrc;
    LPBYTE                  pbDst;
    int                     i;
    int                     j;
    char                    szFile[_MAX_PATH];
    IDirectDrawSurface4 *   pSurface;
    HRESULT                 hr;

    pras24 = new CRasterDC(g_hwnd, 
                           prasMainScreen->iWidthFront, 
                           prasMainScreen->iHeightFront, 
                           24);
	hdcDst = pras24->hdcGet();

	// Save out a blank bitmap if a real one cannot be found.

    pSurface = prasMainScreen->GetPrimarySurface();
    hr = pSurface->GetDC(&hdcSrc);

	SetStretchBltMode(hdcDst, COLORONCOLOR);
	StretchBlt(hdcDst, 0, 0, pras24->iWidth, pras24->iHeight,
			   hdcSrc, 0, 0, prasMainScreen->iWidthFront, prasMainScreen->iHeightFront,
			   SRCCOPY);

	pSurface->ReleaseDC(hdcSrc);
	pras24->ReleaseDC(hdcDst);

    dib.Create(pras24->iWidth, pras24->iHeight, 24, 0);

	pbDst = dib.GetBits();
    pras24->Lock();
    pbSrc = (LPBYTE)pras24->pSurface;

	for (i = 0; i < dib.GetHeight(); i++)
    {
        pbDst = dib.GetPixelPointer(0, (dib.GetHeight() - 1) - i);

		for (j = 0; j < dib.GetWidth(); j++)
		{
			*pbDst++ = *pbSrc;
			*pbDst++ = *(pbSrc+1);
			*pbDst++ = *(pbSrc+2);
			pbSrc += 3;
		}
	}

    pras24->Unlock();

    delete pras24;

    GetNextAvailName(szFile, sizeof(szFile), "TPass", "BMP");
    dib.Write(szFile);
}



//+--------------------------------------------------------------------------
//
//  Function:   MiddleMessage
//
//  Synopsis:   Draws a message in the center of the user's screen
//
//  Arguments:  [uiIDS] -- String Identifier to draw
//
//  Returns:    void
//
//  History:    26-Aug-98    SHernd  Created
//
//---------------------------------------------------------------------------
void MiddleMessage(UINT uiIDS)
{
    char                    sz[50];
    HDC                     hdcSrc;
    IDirectDrawSurface4 *   pSurface;
    HRESULT                 hr;
    RECT                    rc;
    COLORREF                cr;
    int                     iBkMode;

    pSurface = prasMainScreen->GetPrimarySurface();
    if (pSurface == NULL)
    {
        return;
    }

    LoadString(g_hInst, uiIDS, sz, sizeof(sz));

    SetRect(&rc, 
            1, 
            1, 
            prasMainScreen->iWidthFront + 1, 
            prasMainScreen->iHeightFront + 1);

    hr = pSurface->GetDC(&hdcSrc);
    if (FAILED(hr))
    {
        return;
    }

    cr = GetTextColor(hdcSrc);

    SetTextColor(hdcSrc, RGB(0, 0, 0));
    iBkMode = SetBkMode(hdcSrc, TRANSPARENT);

    DrawTextEx(hdcSrc, 
               sz, 
               -1, 
               &rc, 
               DT_CENTER | DT_VCENTER | DT_SINGLELINE,
               NULL);

    SetTextColor(hdcSrc, RGB(255, 255, 255));

    SetRect(&rc, 
            0, 
            0, 
            prasMainScreen->iWidthFront, 
            prasMainScreen->iHeightFront);

    DrawTextEx(hdcSrc, 
               sz, 
               -1, 
               &rc, 
               DT_CENTER | DT_VCENTER | DT_SINGLELINE,
               NULL);

    SetTextColor(hdcSrc, cr);
    SetBkMode(hdcSrc, iBkMode);

    hr = pSurface->ReleaseDC(hdcSrc);
}


void DisplayLoadingFileError(int iErr)
{
    switch (iErr)
    {
        default:
            iErr = IDS_ERROR_LOAD_UNKNOWN;
            break;

        case -1:
            // Invalid SceneFile
            iErr = IDS_ERROR_LOAD_INALID_SCENE;
            break;

        case -2:
            // Not enough HD space for SWP
            iErr = IDS_ERROR_NOT_ENOUGH_HD;
            break;
    }

    CMsgOkDlg       dlg(iErr, g_pMainWnd->m_pUIMgr);

    dlg.StartUIWnd();
}



//+--------------------------------------------------------------------------
//
//  Function:   ClearInputState
//
//  Synopsis:   This function will remove all input messages from the 
//              windows message queue and center the mouse
//
//  Arguments:  [bCenterMouse] -- if we should center the mouse
//
//  Returns:    void
//
//  History:    05-Sep-98    SHernd  Created
//
//---------------------------------------------------------------------------
void ClearInputState(bool bCenterMouse /* = false */)
{
    MSG         msg;

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

	for (uint u_count = 0; u_count < KEYMAP_COUNT; u_count++)
	{
		GetAsyncKeyState(km_DefaultKeyMapping[u_count].u4_KeyCode);
    }

    if (bCenterMouse)
    {
	    RECT    rect;
        POINT   point;

	    GetClientRect(g_hwnd, &rect);
	    point.x = rect.right / 2;
	    point.y = rect.bottom / 2;
	    ClientToScreen(g_hwnd, &point);

	    SetCursorPos(point.x, point.y);
    }
}

POINT GetCurrentClientSize()
{
    RECT rect = { 0 };
    GetClientRect(g_hwnd, &rect);
    POINT result = { rect.right - rect.left, rect.bottom - rect.top };
    return result;
}

std::string GetFirstLevelName()
{
    {
        char firstlevelname[MAX_PATH] = { '\0' };
        GetRegString(REG_KEY_FIRST_LEVEL, firstlevelname, sizeof(firstlevelname), "");
        if (std::strlen(firstlevelname) > 0)
            return firstlevelname;
    }

    //TODO add support for the 1.1 patch

    const std::string candidates[] = { "be.scn", "demo.scn" };

    char datadirstring[MAX_PATH] = { '\0' };
    GetFileLoc(FA_TYPE::FA_DATADRIVE, datadirstring, sizeof(datadirstring));
    const std::filesystem::path datapath = datadirstring;

    for (const auto& entry : candidates)
    {
        const auto fullpath = datapath / entry;
        if (std::filesystem::exists(fullpath))
            return entry;
    }

    return "";
}
