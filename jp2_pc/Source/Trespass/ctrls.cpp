//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1997.
//
//  File:       ctrls.cpp
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    20-Nov-96   SHernd   Created
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "token.h"
#include "main.h"
#include "uiwnd.h"
#include "ctrls.h"
#include "supportfn.hpp"
#include "resource.h"

extern CMainWnd *   g_pMainWnd;
extern HINSTANCE    g_hInst;

#define FONT_TYPEFACE   "Arial"


CUICtrl::CUICtrl(CUICtrlCallback * pParent)
{
    m_pParent = pParent;
    m_bActive = TRUE;
    m_bVisible = TRUE;
    m_dwID = (DWORD) -1;
    ::SetRect(&m_rc, -1, -1, -1, -1);
}

CUICtrl::~CUICtrl()
{
}

void CUICtrl::DoFrame(POINT ptMouse)
{
}

void CUICtrl::Draw(CRaster * pRaster, RECT * prcClip)
{
}


BOOL CUICtrl::HitTest(int x, int y)
{
    POINT   pt;

    pt.x = x;
    pt.y = y;

    return HitTest(pt);
}


BOOL CUICtrl::HitTest(POINT pt)
{
    if (PtInRect(&m_rc, pt) && m_bVisible)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CUICtrl::GetActive()
{
    return m_bActive;
}

BOOL CUICtrl::SetActive(BOOL bActive)
{
    BOOL    bOld = m_bActive;

    if (m_bActive != bActive)
    {
        m_pParent->CtrlInvalidateRect(&m_rc);
    }

    m_bActive = bActive;

    return bOld;
}

DWORD CUICtrl::GetID()
{
    return m_dwID;
}

void CUICtrl::SetID(DWORD dwID)
{
    m_dwID = dwID;
}

BOOL CUICtrl::GetVisible()
{
    return m_bVisible;
}

BOOL CUICtrl::SetVisible(BOOL bVisible)
{
    BOOL    bOld = m_bVisible;

    if (m_bVisible != bVisible)
    {
        m_pParent->CtrlInvalidateRect(&m_rc);
    }

    m_bVisible = bVisible;

    return bOld;
}

void CUICtrl::GetRect(RECT * prc)
{
    Assert(prc);

    *prc = m_rc;
}

void CUICtrl::SetRect(RECT * prc)
{
    Assert(prc);

    m_rc = *prc;
}

void CUICtrl::SetFocus(BOOL bFocus)
{
    m_bFocus = bFocus;
}

BOOL CUICtrl::GetFocus()
{
    return m_bFocus;
}

void CUICtrl::MouseMove(int x, int y, UINT keyFlags)
{
}

BOOL CUICtrl::LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags)
{
    return FALSE;
}

BOOL CUICtrl::LButtonUp(int x, int y, UINT keyFlags)
{
    return FALSE;
}


BOOL CUICtrl::TokenLoad(HANDLE hFile)
{
    BOOL            bRet;
    WNDFILETOKEN    wft;

    // Active 
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bActive = wft.val.iVal;

    // Visible
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bVisible = wft.val.iVal;

    // ID 
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_dwID = (DWORD)wft.val.iVal;

    // left
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_rc.left = wft.val.iVal;

    // top
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_rc.top = wft.val.iVal;

    // right
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_rc.right = wft.val.iVal;

    // bottom
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_rc.bottom = wft.val.iVal;

    bRet = TRUE;

    return bRet;
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



CUIButton::CUIButton(CUICtrlCallback * pParent) : 
    CUICtrl(pParent)
{
    m_uiCmd = 0;
    RtlZeroMemory(m_apRaster, sizeof(m_apRaster));
    m_bUseTrans = FALSE;
    m_bOver = FALSE;
    m_bDown = FALSE;
    m_bCaptured = FALSE;
}


CUIButton::~CUIButton()
{
    int     i;

    for (i = 0; i < 4; i++)
    {
        if (m_apRaster[i])
        {
            delete m_apRaster[i];
        }
    }
}


BOOL CUIButton::HitTest(POINT pt)
{
    LPBYTE          pb;
    int             iPitch;
    BOOL            bRet = TRUE;
    CRaster *       pRaster;

    if (!m_bUseTrans)
    {
        return CUICtrl::HitTest(pt);
    }

    if (!CUICtrl::HitTest(pt))
    {
        return FALSE;
    }

    pRaster = m_apRaster[CUIBUTTON_UP];

    pRaster->Lock();

    pb = (BYTE*)pRaster->pSurface;
    iPitch = pRaster->iLineBytes();

    pb = pb + 
         (iPitch * (pt.y - m_rc.top)) + 
         (pRaster->iPixelBytes() * (pt.x - m_rc.left));

    if (*(LPWORD)pb == m_wTrans)
    {
        bRet = FALSE;
    }

    pRaster->Unlock();

    return bRet;
}


CRaster * CUIButton::GetRaster()
{
    CRaster *   pRaster = NULL;

    if (m_bVisible)
    {
        if (!m_bActive)
        {
            pRaster = m_apRaster[CUIBUTTON_INACTIVE];
        }
        else if (m_bDown)
        {
            pRaster = m_apRaster[CUIBUTTON_DOWN];
        }
        else if (m_bOver)
        {
            pRaster = m_apRaster[CUIBUTTON_OVER];
        }
        else
        {
            pRaster = m_apRaster[CUIBUTTON_UP];
        }
    }

    return pRaster;
}


void CUIButton::DoFrame(POINT ptMouse)
{
    int             iAudioType = -1;
    bool            fInvalid = false;
    CRaster *       praster;
    int             x;
    int             y;

    praster = GetRaster();

    x = ptMouse.x - m_rc.left;
    y = ptMouse.y - m_rc.top;

    if (HitTest(ptMouse) && m_bVisible)
    {
        if (!m_bOver)
        {
            m_pParent->RegisterUIOver(TRUE, this);
            m_pParent->CtrlInvalidateRect(&m_rc);
            fInvalid = true;
            iAudioType = 2;
        }

        m_bOver = TRUE;
    }
    else
    {
        if (m_bOver)
        {
            m_pParent->RegisterUIOver(FALSE, this);
            m_pParent->CtrlInvalidateRect(&m_rc);
            fInvalid = true;
        }

        m_bOver = FALSE;
    }

    if (m_bCaptured && m_bOver)
    {
        iAudioType = 1;
        m_bDown = TRUE;
    }
    else if (m_bCaptured && !m_bOver)
    {
        iAudioType = 0;
        m_bDown = FALSE;
    }

    if (iAudioType >= 0 && 
        fInvalid == true && 
        m_bActive && 
        m_bVisible)
    {
        m_pParent->UIButtonAudioCmd(this, (BUTTONCMD)iAudioType);
    }
}

void CUIButton::Draw(CRaster * pRDst, RECT * prcClip)
{
    CRaster *   pRaster;

    pRaster = GetRaster();

    if (pRaster)
    {
        RECT        rc;
        RECT        rc2;

        if (!IntersectRect(&rc, &m_rc, prcClip))
        {
            return;
        }

        ::SetRect(&rc2, 
                  rc.left - m_rc.left,
                  rc.top - m_rc.top,
                  rc.right - m_rc.left,
                  rc.bottom - m_rc.top);

        if (m_bUseTrans)
        {
            pRaster->Lock();
            m_wTrans = *((WORD*)pRaster->pSurface);
            pRaster->Unlock();
        }

        RasterBlt(pRaster, &rc2, pRDst, rc.left, rc.top, m_bUseTrans, m_wTrans);
    }
}

void CUIButton::MouseMove(int x, int y, UINT keyFlags)
{
}

BOOL CUIButton::LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags)
{
    m_bDown = FALSE;

    // If you selected within my button rectangle but I'm not
    // ACTIVE don't continue processing.  Covers situation
    // where we have stacked buttons on the screen. 
    if (CUICtrl::HitTest(x,y) && !m_bActive)
    {
        return TRUE;
    }

    if (!CUICtrl::HitTest(x,y) || !m_bActive || !m_bVisible)
    {
        return FALSE;
    }

    m_pParent->CtrlInvalidateRect(&m_rc);

    if (m_pParent)
    {
        m_pParent->CaptureMouse(this);
        m_bCaptured = TRUE;
    }

    m_bDown = TRUE;
    m_pParent->UIButtonAudioCmd(this, BC_DOWN);

    return TRUE;
}

BOOL CUIButton::LButtonUp(int x, int y, UINT keyFlags)
{
    if (!m_bCaptured || !GetVisible() || !GetActive())
    {
        m_bDown = FALSE;
        return FALSE;
    }

    m_pParent->CtrlInvalidateRect(&m_rc);

    if (m_pParent && m_bCaptured)
    {
        m_pParent->ReleaseMouse(this);
    }

    m_bCaptured = FALSE;

    if (!m_bDown)
    {
        return TRUE;
    }

    m_bDown = FALSE;
    m_pParent->UIButtonAudioCmd(this, BC_UP);

    // if we were down then call the parent 
    if (m_pParent)
    {
        m_pParent->UIButtonUp(this);
    }

    return TRUE;
}


BOOL CUIButton::TokenLoad(HANDLE hFile)
{
    BOOL            bRet;
    int             i;
    WNDFILETOKEN    wft;

    bRet = CUICtrl::TokenLoad(hFile);
    if (!bRet)
    {
        TraceError(("CUIButton::TokenLoad() -- Base class load failed"));
        goto Error;
    }

    for (i = 0; i < 4; i++)
    {
        ReadWndFileToken(hFile, &wft);

        if (wft.TokenType == LFTT_SPRITE)
        {
            ReadWndFileToken(hFile, &wft);

            if (wft.TokenType == LFTT_VALUE)
            {
                m_bUseTrans = wft.val.iVal;
                ReadWndFileToken(hFile, &wft);
            }
        }
        else if (wft.TokenType == LFTT_VALUE)
        {
            m_bUseTrans = wft.val.iVal;
            ReadWndFileToken(hFile, &wft);
        }

        Assert(wft.TokenType == LFTT_STRING);

        // If there is a null string value then we just want to ignore
        // this sprite value
        if (strlen(wft.val.sz) == 0)
        {
            m_apRaster[i] = NULL;
            continue;
        }

        m_apRaster[i] = ReadAndConvertBMP(wft.val.sz, false);
    }

    Init();

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CUIButton::TokenLoad"));
    bRet = FALSE;
    goto Cleanup;
}



BOOL CUIButton::SetRaster(CRaster * pRaster, int iState)
{
    if (m_apRaster[iState])
    {
        delete m_apRaster[iState];
    }

    m_apRaster[iState] = pRaster;

    return TRUE;
}


void CUIButton::Init()
{
    int             i;
    int             iWidth;
    int             iHeight;
    bool            bNoSet;

    iWidth = 0;
    iHeight = 0;
    bNoSet = true;
    for (i = 0; i < 4; i++)
    {
        if (m_apRaster[i] == NULL)
        {
            continue;
        }

        bNoSet = false;

        if (m_apRaster[i]->iWidth > iWidth)
        {
            iWidth = m_apRaster[i]->iWidth;
        }

        if (m_apRaster[i]->iHeight > iHeight)
        {
            iHeight = m_apRaster[i]->iHeight;
        }
    }

    if (!bNoSet)
    {
        m_rc.right = m_rc.left + iWidth;
        m_rc.bottom = m_rc.top + iHeight;
    }
}



void CUIButton::ReleaseCapture()
{
    if (m_pParent && m_bCaptured)
    {
        m_pParent->ReleaseMouse(this);
    }

    m_bCaptured = FALSE;
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

CUIStatic::CUIStatic(CUICtrlCallback * pParent) : 
    CUICtrl(pParent)
{
    m_pRaster = NULL;
    m_bTrans = TRUE;
}


CUIStatic::~CUIStatic()
{
    delete m_pRaster;
}


void CUIStatic::Draw(CRaster * pRDst, RECT * prcClip)
{
    if (m_pRaster && m_bVisible)
    {
        RECT        rc;
        RECT        rc2;

        if (!IntersectRect(&rc, &m_rc, prcClip))
        {
            return;
        }

        ::SetRect(&rc2, 
                  rc.left - m_rc.left,
                  rc.top - m_rc.top,
                  rc.right - m_rc.left,
                  rc.bottom - m_rc.top);

        RasterBlt(m_pRaster, 
                  &rc2, 
                  pRDst, 
                  rc.left, 
                  rc.top, 
                  m_bTrans,
                  m_wTrans);
    }
}

BOOL CUIStatic::TokenLoad(HANDLE hFile)
{
    BOOL            bRet;
    WNDFILETOKEN    wft;

    bRet = CUICtrl::TokenLoad(hFile);
    if (!bRet)
    {
        TraceError(("CUIStatic::TokenLoad() -- Base class load failed"));
        goto Error;
    }

    ReadWndFileToken(hFile, &wft);
    m_bTrans = wft.val.iVal;

    ReadWndFileToken(hFile, &wft);
    if (strlen(wft.val.sz) != 0)
    {
        m_pRaster = ReadAndConvertBMP(wft.val.sz, false);
        Init();
    }

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CUIStatic::TokenLoad"));
    bRet = FALSE;
    goto Cleanup;
}


WORD CUIStatic::GetTransColor()
{
    WORD    wRet;

    if (!m_pRaster)
    {
        return GetColor(0, 255, 0);
    }

    m_pRaster->Lock();
    wRet = *((WORD*)m_pRaster->pSurface);
    m_pRaster->Unlock();

    return wRet;
}



BOOL CUIStatic::SetRaster(CRaster * pRaster, bool bTrans)
{
    if (m_pRaster)
    {
        delete m_pRaster;
    }

    m_pRaster = pRaster;
    m_bTrans = bTrans;
    m_wTrans = GetTransColor();

    return TRUE;
}


void CUIStatic::Init()
{
    if (!m_pRaster)
    {
        return;
    }

    m_rc.right = m_rc.left + m_pRaster->iWidth;
    m_rc.bottom = m_rc.top + m_pRaster->iHeight;

    if (m_bTrans)
    {
        m_wTrans = GetTransColor();
    }
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CUIListbox::CUIListbox(CUICtrlCallback * pParent) : 
    CUICtrl(pParent)
{
    m_bCaptured = FALSE;
    m_bAllowSelection = FALSE;
    m_bShowScrollAlways = FALSE;
    m_bScrollShown = FALSE;
    m_iCurrSel = -1;
    m_iTop = 0;
    m_iItemsMaxVis = 0;
    m_iRowHeight = 0;

    m_pRasterDC = NULL;
    m_bUpdate = TRUE;
    m_crBkColor = RGB(0, 0, 0);
    m_crFGColor = RGB(255, 255, 255);
    m_bBackLit = FALSE;
    m_bBackLitOffset = 1;
    m_bBorder = TRUE;
    m_bTransBk = FALSE;
    m_hfont = NULL;
    m_bFontSize = 10;
    m_iFontWeight = 400;
}


CUIListbox::~CUIListbox()
{
    DeleteObject(m_hfont);
    delete m_pRasterDC;
    RemoveAllItems();
}


BOOL CUIListbox::Initialize()
{
    CUIButton *     pbutton;
    CRaster *       pRaster;
    RECT            rc;

    pbutton = new CUIButton(this);
    pbutton->SetID(IDSCROLLUP);
    ::SetRect(&rc, 0, 0, 0, 0);
    pbutton->SetRect(&rc);
    pRaster = ReadAndConvertBMP("menu\\sarrow_up_0.tga", true);
    pbutton->SetRaster(pRaster, CUIBUTTON_UP);
    pRaster = ReadAndConvertBMP("menu\\sarrow_up_1.tga", true);
    pbutton->SetRaster(pRaster, CUIBUTTON_DOWN);
    pRaster = ReadAndConvertBMP("menu\\sarrow_up_2.tga", true);
    pbutton->SetRaster(pRaster, CUIBUTTON_OVER);
    pRaster = ReadAndConvertBMP("menu\\sarrow_up_3.tga", true);
    pbutton->SetRaster(pRaster, CUIBUTTON_INACTIVE);
    pbutton->Init();
    pbutton->GetRect(&rc);
    m_iScrollWidth = rc.right;
    ::SetRect(&rc, 
              m_rc.right - rc.right, 
              m_rc.top, 
              m_rc.right, 
              m_rc.top + rc.bottom);
    if (m_bBorder)
    {
        OffsetRect(&rc, -1, 1);
    }
    pbutton->SetRect(&rc);
    g_pMainWnd->m_pUIMgr->GetActiveUIWnd()->AddToUICtrlSet(pbutton);
    m_pScroll[0] = pbutton;

    pbutton = new CUIButton(this);
    pbutton->SetID(IDSCROLLDN);
    ::SetRect(&rc, 0, 0, 0, 0);
    pbutton->SetRect(&rc);
    pRaster = ReadAndConvertBMP("menu\\sarrow_dn_0.tga", true);
    pbutton->SetRaster(pRaster, CUIBUTTON_UP);
    pRaster = ReadAndConvertBMP("menu\\sarrow_dn_1.tga", true);
    pbutton->SetRaster(pRaster, CUIBUTTON_DOWN);
    pRaster = ReadAndConvertBMP("menu\\sarrow_dn_2.tga", true);
    pbutton->SetRaster(pRaster, CUIBUTTON_OVER);
    pRaster = ReadAndConvertBMP("menu\\sarrow_dn_3.tga", true);
    pbutton->SetRaster(pRaster, CUIBUTTON_INACTIVE);
    pbutton->Init();
    pbutton->GetRect(&rc);
    ::SetRect(&rc, 
              m_rc.right - rc.right, 
              m_rc.bottom - rc.bottom, 
              m_rc.right,
              m_rc.bottom);
    if (m_bBorder)
    {
        OffsetRect(&rc, -1, -1);
    }
    pbutton->SetRect(&rc);
    g_pMainWnd->m_pUIMgr->GetActiveUIWnd()->AddToUICtrlSet(pbutton);
    m_pScroll[1] = pbutton;

    m_wUp = GetColor(255, 255, 255);
    m_wDown = GetColor(107, 107, 107);
    m_wFlat = GetColor(148, 148, 148);

    Update();

    return TRUE;
}


void CUIListbox::DoFrame(POINT ptMouse)
{
    int         iThumbHeight;
    int         iNewTop;
    int         y;

    if (!m_bCaptured)
    {
        return;
    }

    iThumbHeight = (m_rc.bottom - m_rc.top) - (2 + (2 * m_iScrollWidth));
    y = (ptMouse.y - m_rc.top) - (m_iScrollWidth + 1);

    iNewTop = (int)((double)((y - m_iMouseFromTop) * m_vInfo.size()) / (double) iThumbHeight);
    if (iNewTop < 0)
    {
        iNewTop = 0;
    }
    else if (iNewTop > m_vInfo.size() - m_iItemsMaxVis)
    {
        iNewTop = m_vInfo.size() - m_iItemsMaxVis;
    }

    if (m_iTop != iNewTop)
    {
        m_bUpdate = TRUE;
    }

    m_iTop = iNewTop;

    if (m_bUpdate)
    {
        m_pParent->CtrlInvalidateRect(&m_rc);
    }
}


void CUIListbox::Draw(CRaster * pRaster, RECT * prcClip)
{
    RECT        rc;
    RECT        rc2;
    RECT        rcThumb;
    int         iThumbHeight;

    if (m_bUpdate)
    {
        Update();
    }

    rc = m_rc;

    if (m_bBorder)
    {
        rc.top++;
        rc.left++;
        rc.bottom--;
    }

    rc.right = rc.left + m_pRasterDC->iWidth;
    if (IntersectRect(&rc2, &rc, prcClip))
    {
        ::SetRect(&rc2, 
                  rc.left - rc.left,
                  rc.top - rc.top,
                  rc.right - rc.left,
                  rc.bottom - rc.top);

        RasterBlt(m_pRasterDC, 
                  &rc2, 
                  pRaster, 
                  rc.left, 
                  rc.top, 
                  m_bTransBk, 
                  m_wTransColor);
    }

    if (m_bBorder)
    {
        MySunkenRect(&m_rc, m_wUp, m_wDown, pRaster, prcClip);

        iThumbHeight = (m_rc.bottom - m_rc.top) - (2 + (2 * m_iScrollWidth));
        rc.left = m_rc.right - (1 + m_iScrollWidth);
        rc.top = m_rc.top + (1 + m_iScrollWidth);
        rc.right = m_rc.right - 1;
        rc.bottom = rc.top + iThumbHeight;
    }
    else
    {
        iThumbHeight = (m_rc.bottom - m_rc.top) - (2 * m_iScrollWidth);
        rc.left = m_rc.right - m_iScrollWidth;
        rc.top = m_rc.top + m_iScrollWidth;
        rc.right = m_rc.right;
        rc.bottom = rc.top + iThumbHeight;
    }

    if (m_bScrollShown)
    {
        // If we aren't drawing transparent background then fill in the 
        // scroll slot on the right side.
        if (!m_bTransBk || m_bShowScrollAlways)
        {
            IntersectRect(&rc2, &rc, prcClip);
            MyFillRect(&rc2, m_wDown, pRaster);
        }

        if (m_vInfo.size() > m_iItemsMaxVis)
        {
            // Draw Thumb Position
            rcThumb.left = rc.left;
            rcThumb.top = rc.top + (int)(((double)m_iTop / (double)m_vInfo.size()) * (double)iThumbHeight);
            rcThumb.right = rc.right;
            rcThumb.bottom = rc.top + (int)(((double)(m_iTop + m_iItemsMaxVis) / (double)m_vInfo.size()) * (double)iThumbHeight);
            IntersectRect(&rc2, &rcThumb, prcClip);
            MyFillRect(&rc2, m_wFlat, pRaster);
            MySunkenRect(&rcThumb, m_wUp, GetColor(0, 0, 0), pRaster, prcClip);
        }
    }
}

BOOL CUIListbox::LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags)
{
    int         iHit;
    RECT        rc2;
    RECT        rcThumb;
    int         iThumbHeight;

    if (!HitTest(x,y))
    {
        return FALSE;
    }

    // convert x, y to local coordinate system
    x -= m_rc.left;
    y -= m_rc.top;

    if (x >= m_pRasterDC->iWidth)
    {
		if (m_vInfo.size() >= m_iItemsMaxVis)
        {
            m_pScroll[0]->GetRect(&rc2);
            m_pScroll[1]->GetRect(&rcThumb);

            if ((y > rc2.bottom - rc2.top) &&
                (y < (m_rc.bottom - m_rc.top) - (rcThumb.bottom - rcThumb.top)))
            {
                iThumbHeight = (m_rc.bottom - m_rc.top) - (2 + (2 * m_iScrollWidth));

                rcThumb.top = m_iScrollWidth + (int)(((double)m_iTop / (double)m_vInfo.size()) * (double)iThumbHeight);
                rcThumb.bottom = m_iScrollWidth + (int)(((double)(m_iTop + m_iItemsMaxVis) / (double)m_vInfo.size()) * (double)iThumbHeight);

                if (y < rcThumb.top)
                {
                    // page up
                    m_iTop -= m_iItemsMaxVis;
                    if (m_iTop < 0)
                    {
                        m_iTop = 0;
                    }

                    m_bUpdate = TRUE;
                }
                else if (y > rcThumb.bottom)
                {
                    // Page down
                    m_iTop += m_iItemsMaxVis;
                    if (m_iTop > m_vInfo.size() - m_iItemsMaxVis)
                    {
                        m_iTop = m_vInfo.size() - m_iItemsMaxVis;
                    }

                    m_bUpdate = TRUE;
                }
                else
                {
                    if (m_pParent)
                    {
                        m_pParent->CaptureMouse(this);
                        m_bCaptured = TRUE;
                    }

                    m_iMouseFromTop = y - rcThumb.top;
                }
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else if (m_bAllowSelection)
    {
        iHit = (y / m_iRowHeight) + m_iTop;
        if (iHit < m_vInfo.size())
        {
            m_iCurrSel = iHit;
            m_bUpdate = TRUE;

            if (bDoubleClick)
            {
                m_pParent->UIListboxDblClk(this, iHit);
            }
            else
            {
                m_pParent->UIListboxClick(this, iHit);
            }
        }
    }

    if (m_bUpdate)
    {
        m_pParent->CtrlInvalidateRect(&m_rc);
    }

    return TRUE;
}



BOOL CUIListbox::LButtonUp(int x, int y, UINT keyFlags)
{
    if (!m_bCaptured || !GetVisible() || !GetActive())
    {
        return FALSE;
    }

    if (m_pParent && m_bCaptured)
    {
        m_pParent->ReleaseMouse(this);
    }

    m_bCaptured = FALSE;

    return TRUE;
}


void CUIListbox::UIButtonUp(CUIButton * pbutton)
{
    int iNewTop;

    if ((pbutton->GetID() == IDSCROLLUP) && (pbutton->GetActive()))
    {
        iNewTop = m_iTop - 1;
        if (iNewTop >= 0)
        {
            m_iTop = iNewTop;
            m_bUpdate = TRUE;
        }
    }
    else if ((pbutton->GetID() == IDSCROLLDN) && (pbutton->GetActive()))
    {
        iNewTop = m_iTop + 1;
        if (iNewTop <= m_vInfo.size()- m_iItemsMaxVis)
        {
            m_iTop = iNewTop;
            m_bUpdate = TRUE;
        }
    }

    if (m_bUpdate)
    {
        m_pParent->CtrlInvalidateRect(&m_rc);
    }
}

BOOL CUIListbox::TokenLoad(HANDLE hFile)
{
    BOOL            bRet;
    WNDFILETOKEN    wft;
    BYTE            bRed;
    BYTE            bGreen;
    BYTE            bBlue;

    bRet = CUICtrl::TokenLoad(hFile);
    if (!bRet)
    {
        TraceError(("CUIListbox::TokenLoad() -- Base class load failed"));
        goto Error;
    }

    // Read in point size
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bFontSize = wft.val.iVal;

    // Read in font weight
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_iFontWeight = wft.val.iVal;

    // Read In Allow Selection
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bAllowSelection = wft.val.iVal;

    // Read In Transparent BK
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bTransBk = wft.val.iVal;

    ReadWndFileToken(hFile, &wft);

    if (m_bTransBk)
    {
        // Read In Transparent ColorRef
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bRed = wft.val.iVal;
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bGreen = wft.val.iVal;
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bBlue = wft.val.iVal;

        m_crTrans = RGB(bRed, bGreen, bBlue);
    }

    // Read Backlit flag
    m_bBackLit = wft.val.iVal;
    if (m_bBackLit)
    {
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);

        m_bBackLitOffset = wft.val.iVal;

        // Read In Back Lit ColorRef
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bRed = wft.val.iVal;
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bGreen = wft.val.iVal;
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bBlue = wft.val.iVal;

        m_crBackLit = RGB(bRed, bGreen, bBlue);
    }

    // Read In BK ColorRef
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bRed = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bGreen = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bBlue = wft.val.iVal;

    m_crBkColor = RGB(bRed, bGreen, bBlue);

    // Read In FG ColorRef
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bRed = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bGreen = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bBlue = wft.val.iVal;

    m_crFGColor = RGB(bRed, bGreen, bBlue);

    // Read In Show VScroll Always
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bShowScrollAlways = wft.val.iVal;

    // Read In Border
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bBorder = wft.val.iVal;
    
    Initialize();

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CUIListbox::TokenLoad"));
    bRet = FALSE;
    goto Cleanup;
}


int CUIListbox::AddItem(LPCSTR pszText, DWORD dwParam, int iIndex, WORD wFlags)
{
    CUILISTBOXINFO  info;

    m_bUpdate = TRUE;

    if (pszText == NULL)
    {
        info.psz = NULL;
    }
    else
    {
        info.psz = (LPSTR)new BYTE[strlen(pszText) + 1];
        strcpy(info.psz, pszText);
    }

    info.dwParam = dwParam;
    info.wFlags = wFlags;

    if (iIndex == -1)
    {
        m_vInfo.push_back(info);
    }
    else
    {
        int i;
        vector<CUILISTBOXINFO>::iterator    ppinfo;

        for (i = 0, ppinfo = m_vInfo.begin(); i < iIndex && ppinfo; i++, ppinfo++)
        {
            ;
        }

        m_vInfo.insert(ppinfo, info);
    }

    return iIndex;
}


BOOL CUIListbox::RemoveItem(int iIndex)
{
    vector<CUILISTBOXINFO>::iterator    ppinfo;

    int     i;

    for (i = 0, ppinfo = m_vInfo.begin(); i < iIndex; ppinfo++,i++)
    {
        ;
    }

    if (ppinfo)
    {
        delete [] (*ppinfo).psz;
        m_vInfo.erase(ppinfo);
        m_pParent->CtrlInvalidateRect(&m_rc);
        m_bUpdate = TRUE;
    }

    if (m_iCurrSel >= iIndex)
    {
        m_iCurrSel--;

        if (m_iCurrSel < 0 && m_vInfo.size() > 0)
        {
            m_iCurrSel = 0;
        }
    }

    if (m_iTop > m_vInfo.size() - m_iItemsMaxVis)
    {
        m_iTop = m_vInfo.size() - m_iItemsMaxVis;
    }

    if (m_iTop < 0)
    {
        m_iTop = 0;
    }


    return TRUE;
}


BOOL CUIListbox::RemoveAllItems()
{
    vector<CUILISTBOXINFO>::iterator    pinfo;

    for (pinfo = m_vInfo.begin(); pinfo < m_vInfo.end(); pinfo++)
    {
        if (pinfo->psz)
        {
            delete [] pinfo->psz;
        }
    }

    m_vInfo.erase(m_vInfo.begin(), m_vInfo.end());

    return TRUE;
}


int CUIListbox::FindItem(LPCSTR pszText)
{
    vector<CUILISTBOXINFO>::iterator    pinfo;
    int  i;
    int  iIndex = -1;

    for (i = 0, pinfo = m_vInfo.begin(); 
         pinfo < m_vInfo.end(); 
         i++, pinfo++)
    {
        if (pinfo->psz && (strcmpi(pszText, pinfo->psz) == 0))
        {
            iIndex = i;
            break;
        }
    }

    return iIndex;
}


int CUIListbox::GetItem(LPSTR psz, int icText, DWORD & dwParam, WORD & wFlags, int iIndex)
{
    CUILISTBOXINFO *    pinfo;
    int                 icCopy;

    pinfo = &m_vInfo[iIndex];
    if (!pinfo)
    {
        return -1;
    }

    dwParam = pinfo->dwParam;
    wFlags = pinfo->wFlags;

    if (psz != NULL && icText > 0)
    {
        icCopy = 0;
        if (pinfo->psz)
        {
            icCopy = strlen(pinfo->psz) + 1;
        }

        if (icCopy >= icText)
        {
            icCopy = icText - 1;
        }

        strncpy(psz, pinfo->psz, icCopy);
        psz[icCopy] = '\0';
    }

    return iIndex;
}


int CUIListbox::SetItem(LPSTR psz, DWORD dwParam, WORD wFlags, int iIndex)
{
    vector<CUILISTBOXINFO>::iterator    pinfo;

    pinfo = m_vInfo.begin() + iIndex;
    if (!pinfo)
    {
        return -1;
    }

    pinfo->dwParam = dwParam;
    pinfo->wFlags = wFlags;

    delete [] pinfo->psz;
    pinfo->psz = NULL;

    if (psz)
    {
        pinfo->psz = (LPSTR)new BYTE[strlen(psz) + 1];
        strcpy(pinfo->psz, psz);
    }

    return iIndex;
}

int CUIListbox::SetCurrSel(int iIndex)
{
    if (iIndex < m_vInfo.size() &&
        iIndex != -1)
    {
        m_iCurrSel = iIndex;
        m_bUpdate = TRUE;
        return iIndex;
    }

    return -1;
}

BOOL CUIListbox::SetFont(HFONT hfont)
{
    return TRUE;
}

BOOL CUIListbox::InitSurface()
{
    int     iWidth;
    int     iHeight;
    HDC     hdc;

    m_bUpdate = TRUE;

    iWidth = (m_rc.right - m_rc.left) - m_iScrollWidth;
    iHeight = (m_rc.bottom - m_rc.top);

    if (m_bBorder)
    {
        iWidth -= 2;
        iHeight -= 2;
    }

    delete m_pRasterDC;

    m_pRasterDC = new CRasterDC(g_pMainWnd->m_pUIMgr->m_hwnd, iWidth, iHeight, 16);
    if (m_pRasterDC == NULL)
    {
        TraceError(("CUIListbox::InitSurface() -- OOM -- "
                    "Error allocating Surface"));
        return FALSE;
    }

    hdc = m_pRasterDC->hdcGet();

    m_hfont = CreateFont(-m_bFontSize,
                         0,
                         0,
                         0,
                         m_iFontWeight,
                         FALSE,
                         FALSE,
                         FALSE,
                         DEFAULT_CHARSET,
                         OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS,
                         DEFAULT_QUALITY,
                         FF_MODERN,
                         FONT_TYPEFACE);

    m_pRasterDC->ReleaseDC(hdc);

    return TRUE;
}



void CUIListbox::Update()
{
    HDC                 hdc;
    RECT                rcWindow;
    RECT                rc;
    HBRUSH              hbr;
    int                 i;
    int                 iStop;
    vector<CUILISTBOXINFO>::iterator    pinfo;
    LPSTR               pszText;
    HFONT               hfontOld;
    WORD                wFill;
    COLORREF            cr;

    if (!m_pRasterDC)
    {
        InitSurface();
    }

    ::SetRect(&rcWindow, 0, 0, m_pRasterDC->iWidth, m_pRasterDC->iHeight);

    // Flood Fill the Background with the BK Color
    if (m_bTransBk)
    {
        cr = m_crTrans;
    }
    else
    {
        cr = m_crBkColor;
    }

    wFill = GetColor(GetRValue(cr), GetGValue(cr), GetBValue(cr)); 

    if (m_bTransBk)
    {
        m_wTransColor = wFill;
    }

    m_pRasterDC->Lock();

    MyFillRect(&rcWindow, 
               wFill, 
               (LPBYTE)m_pRasterDC->pSurface, 
               m_pRasterDC->iWidth, 
               m_pRasterDC->iHeight,
               m_pRasterDC->iLineBytes());

    m_pRasterDC->Unlock();

    hdc = m_pRasterDC->hdcGet();

    SetMapMode(hdc, MM_TEXT);
    hfontOld = SelectFont(hdc, m_hfont);

    SetTextColor(hdc, m_crFGColor);

    SetBkMode(hdc, TRANSPARENT);

    m_iRowHeight = DrawTextEx(hdc, 
                              "A", 
                              -1, 
                              &rc, 
                              DT_LEFT | DT_TOP | DT_CALCRECT, 
                              NULL);

    m_iItemsMaxVis = (rcWindow.bottom - rcWindow.top) / m_iRowHeight;

    if (m_vInfo.size() < m_iItemsMaxVis && !m_bShowScrollAlways)
    {
        m_bScrollShown = FALSE;
        m_pScroll[0]->SetVisible(FALSE);
        m_pScroll[1]->SetVisible(FALSE);
    }
    else
    {
        m_bScrollShown = TRUE;
        m_pScroll[0]->SetVisible(TRUE);
        m_pScroll[0]->SetActive(m_iTop == 0 ? FALSE : TRUE);
        m_pScroll[1]->SetVisible(TRUE);

        i = m_vInfo.size();

        if (((i - m_iItemsMaxVis) > 0) &&
            (m_iTop < (i - m_iItemsMaxVis)))
        {
            m_pScroll[1]->SetActive(TRUE);
        }
        else
        {
            m_pScroll[1]->SetActive(FALSE);
        }
    }

    iStop = m_iTop + m_iItemsMaxVis;
    if (iStop > m_vInfo.size())
    {
        iStop = m_vInfo.size();
    }

    rc.left = 5;
    rc.top = 0;
    rc.right = rcWindow.right - 5;
    rc.bottom = m_iRowHeight;

    for (i = m_iTop; rc.top < rcWindow.bottom && i < iStop; i++)
    {
        pinfo = &m_vInfo[i];
        if (!pinfo)
        {
            break;
        }

        if (pinfo->wFlags & UILB_TEXTCALLBACK)
        {
            // BUGBUG:  get callback text
            pszText = NULL;
        }
        else
        {
            pszText = pinfo->psz;
        }

        if (i == m_iCurrSel)
        {
            hbr = CreateSolidBrush(m_crFGColor);
            rc.left -= 5;
            rc.right += 5;
            FillRect(hdc, &rc, hbr);
            rc.left += 5;
            rc.right -= 5;
            DeleteBrush(hbr);

            SetTextColor(hdc, m_crBkColor);
        }
        else
        {
            RECT        rc2;
            COLORREF    cr;

            cr = SetTextColor(hdc, m_crBackLit);
            rc2 = rc;
            OffsetRect(&rc2, m_bBackLitOffset, m_bBackLitOffset);
            DrawTextEx(hdc, pszText, -1, &rc2, DT_LEFT | DT_TOP, NULL);
            SetTextColor(hdc, cr);
        }
        
        DrawTextEx(hdc, pszText, -1, &rc, DT_LEFT | DT_TOP, NULL);

        if (i == m_iCurrSel)
        {
            SetTextColor(hdc, m_crFGColor);
        }

        rc.top = rc.bottom;
        rc.bottom = rc.top + m_iRowHeight;
    }

    SelectFont(hdc, hfontOld);

    m_pRasterDC->ReleaseDC(hdc);

    m_bUpdate = FALSE;
}


void CUIListbox::ReleaseCapture()
{
    if (m_pParent && m_bCaptured)
    {
        m_pParent->ReleaseMouse(this);
    }

    m_bCaptured = FALSE;
}



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CUICheckbox::CUICheckbox(CUICtrlCallback * pParent) : 
    CUICtrl(pParent)
{
    int     i;

    m_uiCmd = 0;
    m_bOver = FALSE;
    m_bDown = FALSE;
    m_bLastState = 0xFF;
    m_bCaptured = FALSE;

    for (i = 0; i < 6; i++)
    {
        m_aprasCheck[i] = NULL;
    }
}


CUICheckbox::~CUICheckbox()
{
    for (int i = 0; i < 6; i++)
    {
        delete m_aprasCheck[i];
    }
}


void CUICheckbox::DoFrame(POINT ptMouse)
{
    if (PtInRect(&m_rc, ptMouse) && m_bVisible)
    {
        if (!m_bOver)
        {
            m_pParent->RegisterUIOver(TRUE, this);
            m_pParent->CtrlInvalidateRect(&m_rc);
        }

        m_bOver = TRUE;
    }
    else
    {
        if (m_bOver)
        {
            m_pParent->RegisterUIOver(FALSE, this);
            m_pParent->CtrlInvalidateRect(&m_rc);
        }

        m_bOver = FALSE;
    }
}

void CUICheckbox::Draw(CRaster * pRaster, RECT * prcClip)
{
    CRasterDC  *    pras;
    BYTE            b;

    if (m_bVisible)
    {
        if (!m_bActive)
        {
            if (!m_bDown)
            {
                b = CUICHECKBOX_INACTIVE;
            }
            else
            {
                b = CUICHECKBOX_INACTIVE_CHK;
            }
        }
        else if (m_bDown)
        {
            if (m_bOver)
            {
                b = CUICHECKBOX_CHK_OVER;
            }
            else
            {
                b = CUICHECKBOX_CHK;
            }
        }
        else
        {
            if (m_bOver)
            {
                b = CUICHECKBOX_UN_OVER;
            }
            else
            {
                b = CUICHECKBOX_UN;
            }
        }

        pras = m_aprasCheck[b];

        if (b != m_bLastState)
        {
            InvalidateSelf();
            m_bLastState = b;
        }

        if (pras)
        {
            RECT        rc;

            IntersectRect(&rc, &m_rc, prcClip);
            OffsetRect(&rc, -m_rc.left, -m_rc.top);

            RasterBlt(pras, 
                      &rc, 
                      pRaster, 
                      m_rc.left, 
                      m_rc.top, 
                      FALSE, 
                      0);
        }
    }
}


BOOL CUICheckbox::LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags)
{
    if (!HitTest(x,y))
    {
        return FALSE;
    }

    InvalidateSelf();

    if (bDoubleClick)
    {
        m_pParent->UIDoubleClick(this);
    }
    else
    {
        m_bCaptured = TRUE;
        m_pParent->CaptureMouse(this);
    }

    return TRUE;
}


BOOL CUICheckbox::LButtonUp(int x, int y, UINT keyFlags)
{
    if (!m_bCaptured || !GetVisible() || !GetActive())
    {
        return FALSE;
    }

    InvalidateSelf();

    if (m_pParent && m_bCaptured)
    {
        m_pParent->ReleaseMouse(this);
    }

    m_bCaptured = FALSE;

    // if we were down then call the parent 
    if (m_pParent && HitTest(x, y))
    {
        m_bDown = !m_bDown;
        m_pParent->UICheck(this);
    }

    return TRUE;
}


BOOL CUICheckbox::TokenLoad(HANDLE hFile)
{
    BOOL            bRet;
    int             i;
    WNDFILETOKEN    wft;
    int             iWidth;
    int             iHeight;

    bRet = CUICtrl::TokenLoad(hFile);
    if (!bRet)
    {
        TraceError(("CUICheckbox::TokenLoad() -- Base class load failed"));
        goto Error;
    }

    iWidth = 0;
    iHeight = 0;
    for (i = 0; i < 4; i++)
    {
        ReadWndFileToken(hFile, &wft);

        if (wft.TokenType == LFTT_SPRITE)
        {
            ReadWndFileToken(hFile, &wft);
        }

        Assert(wft.TokenType == LFTT_STRING);

        // If there is a null string value then we just want to ignore
        // this sprite value
        if (strlen(wft.val.sz) == 0)
        {
            continue;
        }

        m_aprasCheck[i] = (CRasterDC*)ReadAndConvertBMP(wft.val.sz, false);

        if (m_aprasCheck[i]->iWidth > iWidth)
        {
            iWidth = m_aprasCheck[i]->iWidth;
        }
        if (m_aprasCheck[i]->iHeight > iHeight)
        {
            iHeight = m_aprasCheck[i]->iHeight;
        }
    }

    // Adjust Rect to what our actual image size is
    m_rc.right = m_rc.left + iWidth;
    m_rc.bottom = m_rc.top + iHeight;

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CUICheckbox::TokenLoad"));
    bRet = FALSE;
    goto Cleanup;
}


void CUICheckbox::ReleaseCapture()
{
    if (m_pParent && m_bCaptured)
    {
        m_pParent->ReleaseMouse(this);
    }

    m_bCaptured = FALSE;
}



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CUITextbox::CUITextbox(CUICtrlCallback * pParent) : 
    CUICtrl(pParent)
{
    m_bUpdate = TRUE;
    m_pszText = NULL;
    m_pras = NULL;
    m_crBkColor = RGB(0, 0, 0);
    m_crFGColor = RGB(255, 255, 255);
    m_dwDTFormat = 0;
    m_bFontSize = 12;
    m_iFontWeight = 400;
    m_bBackLit = FALSE;
    m_bBackLitOffset = 0;
    m_bBorder = FALSE;
    m_bTransBk = FALSE;
    m_hfont = NULL;
}


CUITextbox::~CUITextbox()
{
    delete m_pszText;
    delete m_pras;
    DeleteFont(m_hfont);
}



BOOL CUITextbox::InitSurface()
{
    HDC     hdc;

    m_bUpdate = TRUE;

    delete m_pras;
    DeleteFont(m_hfont);

    m_pras = new CRasterDC(g_pMainWnd->m_pUIMgr->m_hwnd, 
                           m_rc.right - m_rc.left, 
                           m_rc.bottom - m_rc.top, 
                           16);
    if (m_pras == NULL)
    {
        TraceError(("CUITextbox::InitSurface() -- OOM -- "
                    "Error allocating Surface"));
        return FALSE;
    }

    hdc = m_pras->hdcGet();

    m_hfont = CreateFont(-m_bFontSize,
                         0,
                         0,
                         0,
                         m_iFontWeight,
                         FALSE,
                         FALSE,
                         FALSE,
                         DEFAULT_CHARSET,
                         OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS,
                         DEFAULT_QUALITY,
                         FF_MODERN,
                         FONT_TYPEFACE);

    m_pras->ReleaseDC(hdc);

    return TRUE;
}



void CUITextbox::Update()
{
    HDC         hdc;
    RECT        rc;
    HFONT       hfontOld;
    WORD        wFill;

    ::SetRect(&rc, 0, 0, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top);

    if (!m_pras)
    {
        InitSurface();
    }

    // Flood Fill the Background with the BK Color
    wFill = GetColor(GetRValue(m_crBkColor), 
                     GetGValue(m_crBkColor), 
                     GetBValue(m_crBkColor)); 
    if (m_bTransBk)
    {
        m_wTransColor = wFill;
    }

    m_pras->Lock();

    MyFillRect(&rc, 
               wFill, 
               (LPBYTE)m_pras->pSurface, 
               m_pras->iWidth, 
               m_pras->iHeight,
               m_pras->iLineBytes());

    m_pras->Unlock();

    hdc = m_pras->hdcGet();

    SetMapMode(hdc, MM_TEXT);
    hfontOld = SelectFont(hdc, m_hfont);

    SetBkMode(hdc, TRANSPARENT);
    if (m_bBorder)
    {
        DrawEdge(hdc, &rc, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
    }

    // If we want VCenter and it is not single line. 
    // (DrawTextEx does not support this)  We need to calculate
    // the actual rect ourselves
    if ((m_dwDTFormat & DT_VCENTER) && !(m_dwDTFormat & DT_SINGLELINE))
    {
        RECT        rcInner;
        int         iHeight;

        rcInner = rc;
        iHeight = DrawTextEx(hdc, 
                             m_pszText, 
                             -1, 
                             &rcInner, 
                             m_dwDTFormat | DT_CALCRECT, 
                             NULL);
        rc.top = rc.top + (((rc.bottom - rc.top) / 2) - (iHeight / 2));
    }

    if (m_bBackLit)
    {
        RECT        rcBackLit;

        SetTextColor(hdc, m_crBackLit);
        rcBackLit = rc;
        OffsetRect(&rcBackLit, m_bBackLitOffset, m_bBackLitOffset);
        DrawTextEx(hdc, m_pszText, -1, &rcBackLit, m_dwDTFormat, NULL);
    }

    // Draw FG Text
    SetTextColor(hdc, m_crFGColor);
    DrawTextEx(hdc, m_pszText, -1, &rc, m_dwDTFormat, NULL);

    SelectFont(hdc, hfontOld);

    m_pras->ReleaseDC(hdc);

    m_bUpdate = FALSE;
}


void CUITextbox::Draw(LPBYTE pbDst,
                          int iDstWidth,
                          int iDstHeight,
                          int iDstPitch,
                          int iDstBytes,
                          RECT * prcClip)
{
    if (m_bUpdate)
    {
        Update();
    }

    if (!m_bVisible)
    {
        return;
    }

    RECT        rc;
    int         xDst;
    int         yDst;

    if (IntersectRect(&rc, &m_rc, prcClip))
    {
        xDst = rc.left;
        yDst = rc.top;
        OffsetRect(&rc, -m_rc.left, -m_rc.top);

        m_pras->Lock();

        MyBlt((LPBYTE)m_pras->pSurface,
              m_pras->iLineBytes(),
              m_pras->iPixelBytes(),
              rc.left,
              rc.top,
              rc.right - rc.left,
              rc.bottom - rc.top,
              pbDst,
              iDstPitch,
              iDstBytes,
              xDst,
              yDst,
              m_bTransBk,
              m_wTransColor);
    }

    m_pras->Unlock();
}



void CUITextbox::Draw(CRaster * pRaster, RECT * prcClip)
{
    if (m_bUpdate)
    {
        Update();
    }

    if (!m_bVisible)
    {
        return;
    }

    RECT        rc;
    RECT        rc2;

    if (!IntersectRect(&rc, &m_rc, prcClip))
    {
        return;
    }

    ::SetRect(&rc2,
              rc.left - m_rc.left,
              rc.top - m_rc.top,
              rc.right - rc.left,
              rc.bottom - rc.top);

    RasterBlt(m_pras,
              &rc2,
              pRaster,
              rc.left,
              rc.top,
              m_bTransBk,
              m_wTransColor);
}



BOOL CUITextbox::SetText(LPSTR pszNew)
{
    BOOL        bRet;
    LPSTR       psz;

    psz = (LPSTR)new BYTE[strlen(pszNew) + 1];
    if (psz == NULL)
    {
        TraceError(("CUITextbox::SetText() -- OOM -- "
                    "unable to set new text"));
        goto Error;
    }

    strcpy(psz, pszNew);

    delete m_pszText;

    m_pszText = psz;

    bRet = TRUE;
    m_bUpdate = TRUE;

    m_pParent->CtrlInvalidateRect(&m_rc);

Cleanup:
    return bRet;

Error:
    TraceError(("CUITextbox::SetText"));
    bRet = FALSE;
    goto Cleanup;
}



BOOL CUITextbox::TokenLoad(HANDLE hFile)
{
    BOOL            bRet;
    WNDFILETOKEN    wft;
    BYTE            bRed;
    BYTE            bGreen;
    BYTE            bBlue;
    char            sz[200];

    bRet = CUICtrl::TokenLoad(hFile);
    if (!bRet)
    {
        TraceError(("CUITextbox::TokenLoad() -- Base class load failed"));
        goto Error;
    }

    // Read In Transparent BK
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bTransBk = wft.val.iVal;

    ReadWndFileToken(hFile, &wft);

    if (wft.TokenType == LFTT_VALUE)
    {
        m_bBackLit = wft.val.iVal;

        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);

        m_bBackLitOffset = wft.val.iVal;

        // Read In Back Lit ColorRef
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bRed = wft.val.iVal;
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bGreen = wft.val.iVal;
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bBlue = wft.val.iVal;

        m_crBackLit = RGB(bRed, bGreen, bBlue);

        ReadWndFileToken(hFile, &wft);
    }

    // Read In Initial Text
    if (wft.TokenType == LFTT_STRING)
    {
        SetText(wft.val.sz);
    }
    else if (wft.TokenType == LFTT_VALUE)
    {
        LoadString(g_hInst, 
                   IDS_STR_CONTROLS_TEXT + wft.val.iVal, 
                   sz, 
                   sizeof(sz));
        SetText(sz);
    }

    // Read in point size
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bFontSize = wft.val.iVal;

    // Read in font weight
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_iFontWeight = wft.val.iVal;

    // Read In BK ColorRef
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bRed = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bGreen = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bBlue = wft.val.iVal;

    m_crBkColor = RGB(bRed, bGreen, bBlue);

    // Read In FG ColorRef
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bRed = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bGreen = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bBlue = wft.val.iVal;

    m_crFGColor = RGB(bRed, bGreen, bBlue);

    // Read In Dword Drawing Flags
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_dwDTFormat = wft.val.iVal;

    // Read In Border
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bBorder = wft.val.iVal;
    
    // Use the current global palette as the palette for this
    InitSurface();

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CUITextbox::TokenLoad"));
    bRet = FALSE;
    goto Cleanup;
}



void CUITextbox::SetPalette(HPALETTE hpal)
{
    m_bUpdate = TRUE;
}



void CUITextbox::SetPalette(LOGPALETTE * plogpal)
{
    m_bUpdate = TRUE;
}



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



CUISlider::CUISlider(CUICtrlCallback * pParent) : 
    CUICtrl(pParent)
{
    m_uiCmd = 0;
    m_bOver = FALSE;
    m_bDown = FALSE;
    m_bCaptured = FALSE;
    m_iUnits = 0;
    m_iCurrUnit = 0;
    m_bBorder = FALSE;
    m_pRaster = NULL;
}


CUISlider::~CUISlider()
{
    delete m_pRaster;
}


void CUISlider::DoFrame(POINT ptMouse)
{
    if (PtInRect(&m_rc, ptMouse) && m_bVisible)
    {
        if (!m_bOver)
        {
            m_pParent->CtrlInvalidateRect(&m_rc);
        }

        m_bOver = TRUE;
    }
    else
    {
        if (m_bOver)
        {
            m_pParent->CtrlInvalidateRect(&m_rc);
        }

        m_bOver = FALSE;
    }

    if (m_bCaptured)
    {
        m_pParent->CtrlInvalidateRect(&m_rc);

        m_x = ptMouse.x;
    }
}

void CUISlider::Draw(CRaster * pRaster, RECT * prcClip)
{
    RECT        rc;
    int         iPos;
    int         iWidth;
    int         i;

    if (!m_bVisible)
    {
        return;
    }

    iWidth = (m_rc.right - m_rc.left) - 10;

    // Draw Tick marks
    if ((m_iUnits <= 10) && 
        (m_iUnits != 0) && 
        (m_iUnits != 1) && 
        m_bBorder)
    {
        for (i = 0; i < m_iUnits; i++)
        {
            iPos = (int)(((double)i / (double)(m_iUnits - 1)) * 
                         (double)iWidth);
            rc.left = m_rc.left + 5 + iPos - 1;
            rc.top = m_rc.top + 7;
            rc.right = rc.left + 2;
            rc.bottom = rc.top + 10;
            MyFillRect(&rc, GetColor(255, 255, 255), pRaster);
        }
    }

    if (m_pRaster)
    {
        rc.left = m_rc.left + (m_pRaster->iWidth / 2);
        rc.right = m_rc.right - (m_pRaster->iWidth / 2);
        rc.top = m_rc.top + (m_pRaster->iHeight / 2) - 1;
        rc.bottom = rc.top + 3;
    }
    else
    {
        rc.left = m_rc.left + 5;
        rc.right = m_rc.right - 5;
        rc.top = m_rc.top + 10;
        rc.bottom = rc.top + 3;
    }

    if (m_bBorder)
    {
        // Draw Slot
        MyFillRect(&rc, GetColor(0, 0, 0), pRaster);
        MyFrameRect(&rc, GetColor(255, 255, 255), pRaster);
    }

    if ((m_iUnits == 0) || m_iUnits == 1)
    {
        return;
    }

    // Draw Slider
    if (!m_bCaptured)
    {
        iPos = (int)(((double)m_iCurrUnit / (double)(m_iUnits - 1)) * 
                     (double)(rc.right - rc.left));
    }
    else
    {
        if (m_x > m_rc.right - 5)
        {
            iPos = (m_rc.right - 5) - (m_rc.left + 5);
        }
        else if (m_x < m_rc.left + 5)
        {
            iPos = 0;
        }
        else
        {
            iPos = m_x - (m_rc.left + 5);
        }
    }

    // Draw Thumb Position
    if (m_pRaster)
    {
        RECT        rc2;
        ::SetRect(&rc2, 0, 0, m_pRaster->iWidth, m_pRaster->iHeight);
        RasterBlt(m_pRaster,
                  &rc2,
                  pRaster,
                  rc.left + iPos - (m_pRaster->iWidth / 2),
                  m_rc.top,
                  false,
                  0);
    }
    else
    {
        rc.left = rc.left + iPos - 5;
        rc.right = rc.left + 10;
        rc.top = m_rc.top + 1;
        rc.bottom = rc.top + 20;
        MyFillRect(&rc, m_wFlatClr, pRaster);
        MySunkenRect(&rc, m_wUpClr, m_wDownClr, pRaster, &rc);
    }
}

BOOL CUISlider::LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags)
{
    int     iPos;
    RECT    rc;
    POINT   pt;

    m_bDown = FALSE;
    if (!HitTest(x,y))
    {
        return FALSE;
    }

    iPos = (int)(((double)m_iCurrUnit / (double)(m_iUnits - 1)) * 
                 (double)((m_rc.right - 5) - (m_rc.left + 5)));

    if (m_pRaster)
    {
        rc.left = m_rc.left + iPos; // - (m_pRaster->iWidth / 2);
        rc.right = rc.left + m_pRaster->iWidth;
        rc.top = m_rc.top + 1;
        rc.bottom = rc.top + m_pRaster->iHeight;
    }
    else
    {
        rc.left = m_rc.left + iPos;
        rc.right = rc.left + 10;
        rc.top = m_rc.top + 1;
        rc.bottom = rc.top + 20;
    }

    // Check if hit slider control
    pt.x = x;
    pt.y = y;
    if (!PtInRect(&rc, pt))
    {
        // page in the appropriate direction and continue
        if (x < rc.left)
        {
            m_iCurrUnit--;
            if (m_iCurrUnit == -1)
            {
                m_iCurrUnit = 0;
            }
        }
        else
        {
            m_iCurrUnit++;
            if (m_iCurrUnit == m_iUnits)
            {
                m_iCurrUnit = m_iUnits - 1;
            }
        }

        m_pParent->UISliderChange(this, m_iCurrUnit);
        m_pParent->CtrlInvalidateRect(&m_rc);

        return TRUE;
    }

    m_x = x;    

    if (m_pParent)
    {
        m_pParent->CaptureMouse(this);
        m_bCaptured = TRUE;
    }

    m_bDown = TRUE;

    return TRUE;
}

void CUISlider::MouseMove(int x, int y, UINT keyFlags)
{
}

BOOL CUISlider::LButtonUp(int x, int y, UINT keyFlags)
{
    double  dtemp;
    int     iPos;
    int     iWidth;

    if (!m_bCaptured || !GetVisible() || !GetActive())
    {
        m_bDown = FALSE;
        return FALSE;
    }

    iWidth = (m_rc.right - 5) - (m_rc.left + 5);
    if (iWidth == 0)
    {
        iWidth = 1;
    }

    // convert x relative to slider board
    if (x < m_rc.left + 5)
    {
        x = 0;
    }
    else if (x > m_rc.right - 5)
    {
        x = iWidth;
    }
    else
    {
        x = x - (m_rc.left + 5);
    }

    dtemp = (double)x / (double)iWidth;
    dtemp = dtemp * (double)(m_iUnits - 1);
    iPos = (int)dtemp;
    if ((int)(dtemp * 100) % 100 > 50)
    {
        iPos++;
    }

    if (iPos >= m_iUnits)
    {
        iPos = m_iUnits - 1;
    }
    else if (iPos < 0)
    {
        iPos = 0;
    }

    m_iCurrUnit = iPos;
    m_pParent->UISliderChange(this, m_iCurrUnit);

    m_pParent->CtrlInvalidateRect(&m_rc);

    if (m_pParent && m_bCaptured)
    {
        m_pParent->ReleaseMouse(this);
    }

    m_bCaptured = FALSE;

    if (!m_bDown)
    {
        return TRUE;
    }

    m_bDown = FALSE;

    return TRUE;
}

BOOL CUISlider::TokenLoad(HANDLE hFile)
{
    BOOL            bRet;
    WNDFILETOKEN    wft;

    bRet = CUICtrl::TokenLoad(hFile);
    if (!bRet)
    {
        TraceError(("CUISlider::TokenLoad() -- Base class load failed"));
        goto Error;
    }

    ReadWndFileToken(hFile, &wft);
    if (wft.TokenType == LFTT_STRING)
    {
        m_pRaster = ReadAndConvertBMP(wft.val.sz, false);
        ReadWndFileToken(hFile, &wft);
    }

    m_iUnits = wft.val.iVal;

    ReadWndFileToken(hFile, &wft);
    m_iCurrUnit = wft.val.iVal;

    ReadWndFileToken(hFile, &wft);
    m_bBorder = wft.val.iVal;

    m_wUpClr = GetColor(255, 255, 255);
    m_wDownClr = GetColor(107, 107, 107);
    m_wFlatClr = GetColor(148, 148, 148);

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CUISlider::TokenLoad"));
    bRet = FALSE;
    goto Cleanup;
}


void CUISlider::ReleaseCapture()
{
    if (m_pParent && m_bCaptured)
    {
        m_pParent->ReleaseMouse(this);
    }

    m_bCaptured = FALSE;
}


//+--------------------------------------------------------------------------
//
// Function:   CUISlider::SetCurrUnit
//
// Synopsis:   Sets the slider unit position
//
// Arguments:  [iUnit] -- new slider position
//
// Returns:    void 
//
// History:    05-Mar-97 Shernd    Created
//
//---------------------------------------------------------------------------
void CUISlider::SetCurrUnit(int iUnit)
{
    m_iCurrUnit = iUnit;
    m_pParent->CtrlInvalidateRect(&m_rc);
}



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CUIEditbox::CUIEditbox(CUICtrlCallback * pParent) : 
    CUICtrl(pParent)
{
    m_iMaxSize = 1024;
    m_bUpdate = TRUE;
    m_pras = NULL;
    m_crBkColor = RGB(0, 0, 0);
    m_crFGColor = RGB(255, 255, 255);
    m_dwDTFormat = 0;
    m_bBorder = FALSE;
    m_bFontSize = 10;
    m_iFontWeight = 400;
    m_hfont = NULL;
    m_vInfo.push_back('\0');
    m_iLeft = 0;
    m_bBackLit = FALSE;
    m_bBackLitOffset = 0;
    m_bTransBk = FALSE;
}


CUIEditbox::~CUIEditbox()
{
    delete m_pras;
    DeleteFont(m_hfont);
}



BOOL CUIEditbox::InitSurface()
{
    HDC     hdc;

    m_bUpdate = TRUE;

    delete m_pras;

    m_pras = new CRasterDC(g_pMainWnd->m_pUIMgr->m_hwnd, 
                           m_rc.right - m_rc.left, 
                           m_rc.bottom - m_rc.top, 
                           16);
    if (m_pras == NULL)
    {
        TraceError(("CUITextbox::InitSurface() -- OOM -- "
                    "Error allocating Surface"));
        return FALSE;
    }

    hdc = m_pras->hdcGet();

    m_hfont = CreateFont(-m_bFontSize,
                         0,
                         0,
                         0,
                         m_iFontWeight,
                         FALSE,
                         FALSE,
                         FALSE,
                         DEFAULT_CHARSET,
                         OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS,
                         DEFAULT_QUALITY,
                         FF_MODERN,
                         FONT_TYPEFACE);

    m_pras->ReleaseDC(hdc);

    return TRUE;
}



void CUIEditbox::Update()
{
    HDC     hdc;
    RECT    rc;
    HBRUSH  hbr;
    LPSTR   psz;
    int     iHeight;
    int     iLen;
    RECT    rcWindow;
    HFONT   hfontOld;
    WORD    wFill;

    ::SetRect(&rcWindow, 0, 0, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top);

    if (!m_pras)
    {
        InitSurface();
    }

    // Flood Fill the Background with the BK Color
    wFill = GetColor(GetRValue(m_crBkColor), 
                     GetGValue(m_crBkColor), 
                     GetBValue(m_crBkColor)); 
    if (m_bTransBk)
    {
        m_wTransColor = wFill;
    }

    m_pras->Lock();

    MyFillRect(&rcWindow, 
               wFill, 
               (LPBYTE)m_pras->pSurface, 
               m_pras->iWidth, 
               m_pras->iHeight,
               m_pras->iLineBytes());

    m_pras->Unlock();

    hdc = m_pras->hdcGet();

    SetMapMode(hdc, MM_TEXT);
    hfontOld = SelectFont(hdc, m_hfont);

    SetTextColor(hdc, m_crFGColor);

    psz = m_vInfo.begin();
    if (psz == NULL)
    {
        iLen = 0;
    }
    else
    {
        iLen = strlen(psz);
    }

    SetBkMode(hdc, TRANSPARENT);
    if (m_bBorder)
    {
        DrawEdge(hdc, &rcWindow, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
    }

    if (iLen > 0)
    {
        while (TRUE)
        {
            rc = rcWindow;

            if (m_iLeft >= iLen)
            {
                m_iLeft -= 5;
                if (m_iLeft < 0)
                {
                    m_iLeft = 0;
                }
            }

            iHeight = DrawTextEx(hdc, psz + m_iLeft, -1, &rc, m_dwDTFormat | DT_CALCRECT, NULL);
            if (rc.right < rcWindow.right)
            {
                break;
            }

            m_iLeft += 5;
        }

        if (m_bBackLit)
        {
            COLORREF    cr;
            RECT        rc2;

            cr = SetTextColor(hdc, m_crBackLit);
            rc2 = rc;
            OffsetRect(&rc2, m_bBackLitOffset, m_bBackLitOffset);
            DrawTextEx(hdc, psz + m_iLeft, -1, &rc2, m_dwDTFormat, NULL);
            SetTextColor(hdc, cr);
        }

        DrawTextEx(hdc, psz + m_iLeft, -1, &rc, m_dwDTFormat, NULL);
    }
    else
    {
        rc = rcWindow;
        rc.right = rc.left + 1;
    }

    hbr = CreateSolidBrush(m_crFGColor);
    rc.left = rc.right;
    rc.right = rc.left + 1;
    rc.bottom = iHeight;
    FillRect(hdc, &rc, hbr);
    DeleteObject(hbr);

    SelectFont(hdc, hfontOld);

    m_pras->ReleaseDC(hdc);

    m_bUpdate = FALSE;
}


void CUIEditbox::DoFrame(POINT ptMouse)
{
}

BOOL CUIEditbox::LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags)
{
    return FALSE;
}

BOOL CUIEditbox::LButtonUp(int x, int y, UINT keyFlags)
{
    return FALSE;
}


void CUIEditbox::Draw(CRaster * pRaster, RECT * prcClip)
{
    RECT    rc;
    RECT    rc2;

    if (m_bUpdate)
    {
        Update();
    }

    if (!m_bVisible)
    {
        return;
    }

    if (!IntersectRect(&rc, &m_rc, prcClip))
    {
        return;
    }

    ::SetRect(&rc2, 
              rc.left - m_rc.left,
              rc.top - m_rc.top,
              rc.right - m_rc.left,
              rc.bottom - m_rc.top);

    RasterBlt(m_pras,
              &rc2,
              pRaster,
              rc.left,
              rc.top,
              m_bTransBk,
              m_wTransColor,
              0);
}


int CUIEditbox::SetMaxCharNum(int icMax)
{
    if (icMax < 1 || icMax > 1023)
    {
        icMax = 1023;
    }

    // increment the size for the Null character
    icMax++;

    while (icMax < m_vInfo.size() &&
           m_vInfo.size() > 1)
    {
        m_vInfo.erase(m_vInfo.end() - 2);
    }

    m_iMaxSize = icMax;

    return icMax - 1;
}


int CUIEditbox::GetMaxCharNum()
{
    return m_iMaxSize;
}



LPSTR CUIEditbox::GetText()
{
    if (!m_vInfo.empty())
    {
        return (LPSTR)m_vInfo.begin();
    }

    return NULL;
}



BOOL CUIEditbox::SetText(LPSTR pszNew)
{
    int     iLen;

    if (pszNew == NULL)
    {
        m_vInfo.erase(m_vInfo.begin(), m_vInfo.end());
        m_vInfo.push_back('\0');
    }
    else
    {
        iLen = strlen(pszNew);

        if (iLen == 0)
        {
            m_vInfo.erase(m_vInfo.begin(), m_vInfo.end());
            m_vInfo.push_back('\0');
        }
        else if (iLen < m_iMaxSize)
        {
            m_vInfo.erase(m_vInfo.begin(), m_vInfo.end());
            m_vInfo.insert(m_vInfo.begin(), strlen(pszNew) + 1, '\0');
            strcpy(m_vInfo.begin(), pszNew);
        }
        else
        {
            return FALSE;
        }
    }

    if (m_iLeft >= m_vInfo.size())
    {
        m_iLeft = m_vInfo.size() - 1;
    }

    m_pParent->CtrlInvalidateRect(&m_rc);
    m_bUpdate = TRUE;
    return TRUE;
}



BOOL CUIEditbox::TokenLoad(HANDLE hFile)
{
    BOOL            bRet;
    WNDFILETOKEN    wft;
    BYTE            bRed;
    BYTE            bGreen;
    BYTE            bBlue;
    char            sz[200];

    bRet = CUICtrl::TokenLoad(hFile);
    if (!bRet)
    {
        TraceError(("CUIEditbox::TokenLoad() -- Base class load failed"));
        goto Error;
    }

    // Read in point size
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bFontSize = wft.val.iVal;

    // Read in font weight
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_iFontWeight = wft.val.iVal;

    // Read In Transparent Box
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bTransBk = wft.val.iVal;
    
    ReadWndFileToken(hFile, &wft);

    if (wft.TokenType == LFTT_VALUE)
    {
        m_bBackLit = wft.val.iVal;

        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);

        m_bBackLitOffset = wft.val.iVal;

        // Read In Back Lit ColorRef
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bRed = wft.val.iVal;
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bGreen = wft.val.iVal;
        ReadWndFileToken(hFile, &wft);
        Assert(wft.TokenType == LFTT_VALUE);
        bBlue = wft.val.iVal;

        m_crBackLit = RGB(bRed, bGreen, bBlue);

        ReadWndFileToken(hFile, &wft);
    }

    // Read In Initial Text
    if (wft.TokenType == LFTT_STRING)
    {
        SetText(wft.val.sz);
    }
    else if (wft.TokenType == LFTT_VALUE)
    {
        LoadString(g_hInst, 
                   IDS_STR_CONTROLS_TEXT + wft.val.iVal, 
                   sz, 
                   sizeof(sz));
        SetText(sz);
    }

    // Read In BK ColorRef
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bRed = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bGreen = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bBlue = wft.val.iVal;

    m_crBkColor = RGB(bRed, bGreen, bBlue);

    // Read In FG ColorRef
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bRed = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bGreen = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bBlue = wft.val.iVal;

    m_crFGColor = RGB(bRed, bGreen, bBlue);

    // Read In Dword Drawing Flags
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_dwDTFormat = wft.val.iVal;

    // Read In Border
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bBorder = wft.val.iVal;
    
    // Use the current global palette as the palette for this
    InitSurface();

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CUIEditbox::TokenLoad"));
    bRet = FALSE;
    goto Cleanup;
}



void CUIEditbox::SetPalette(HPALETTE hpal)
{
    m_bUpdate = TRUE;
}



void CUIEditbox::SetPalette(LOGPALETTE * plogpal)
{
    m_bUpdate = TRUE;
}


void CUIEditbox::OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (!fDown)
    {
        return;
    }

    switch (vk)
    {
        case VK_BACK:
            {
                int iSize;

                iSize = m_vInfo.size();
                if (iSize > 1)
                {
                    vector<char>::iterator  pch;

                    pch = m_vInfo.end() - 2;
                    m_vInfo.erase(pch);
                    m_pParent->CtrlInvalidateRect(&m_rc);
                    m_bUpdate = TRUE;
                }
            }
            break;
    }

    if (m_pParent)
    {
        m_pParent->UIEditboxKey(this, vk, cRepeat, flags);
    }
}


void CUIEditbox::OnChar(TCHAR tch, int cRepeat)
{
    if (tch < 32 || tch > 126)
    {
        return;
    }

    if (m_vInfo.size() + 1 >= m_iMaxSize)
    {
        if (m_pParent)
        {
            m_pParent->UIEditboxMaxText(this);
        }

        return;
    }

    m_vInfo.insert(m_vInfo.end() - 1, tch);
    m_pParent->CtrlInvalidateRect(&m_rc);
    m_bUpdate = TRUE;
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CUIProgressBar::CUIProgressBar(CUICtrlCallback * pParent) : 
    CUICtrl(pParent)
{
    m_iMin = 0;
    m_iMax = 99;
    m_iCurr = 0;
    m_iStep = 10;
    m_wClrFill = 0;
}


CUIProgressBar::~CUIProgressBar()
{
}


BOOL CUIProgressBar::TokenLoad(HANDLE hFile)
{
    BOOL            bRet;
    WNDFILETOKEN    wft;
    BYTE            bRed;
    BYTE            bGreen;
    BYTE            bBlue;

    bRet = CUICtrl::TokenLoad(hFile);
    if (!bRet)
    {
        TraceError(("CUIProgressBar::TokenLoad() -- Base class load failed"));
        return FALSE;
    }

    // Read In Show Frame
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_bFrame = wft.val.iVal;

    // Read In Min Range
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_iMin = wft.val.iVal;

    // Read In Max Range
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_iMax = wft.val.iVal;

    // Read In Curr Value
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_iCurr = wft.val.iVal;

    // Read In Step Value
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    m_iStep = wft.val.iVal;

    // Read In Fill ColorRef
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bRed = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bGreen = wft.val.iVal;
    ReadWndFileToken(hFile, &wft);
    Assert(wft.TokenType == LFTT_VALUE);
    bBlue = wft.val.iVal;

    m_wClrFill = GetColor(bRed, bGreen, bBlue);

    return TRUE;
}


void CUIProgressBar::Draw(LPBYTE pbDst,
                          int iDstWidth,
                          int iDstHeight,
                          int iDstPitch,
                          int iDstBytes,
                          RECT * prcClip)
{
    RECT        rc;
    RECT        rcInter;
    float       fPercent;

    // Calculate the full rect for the Health Bar
    fPercent = (float)(m_iCurr - m_iMin) / (float)(m_iMax - m_iMin);

    rc = m_rc;

    if (m_bFrame)
    {
        MyFrameRect(&rc, GetColor(128, 128, 128), pbDst, iDstWidth, iDstHeight, iDstPitch);
        InflateRect(&rc, -1, -1);
    }

    rc.right = rc.left + (int)((float)(m_rc.right - m_rc.left) * fPercent);
    if (rc.right == rc.left)
    {
        rc.right++;
    }

    if (IntersectRect(&rcInter, &rc, prcClip))
    {
        MyFillRect(&rcInter, 
                   m_wClrFill, 
                   pbDst, 
                   iDstWidth, 
                   iDstHeight, 
                   iDstPitch);
    }
}


void CUIProgressBar::Draw(CRaster * pRaster, RECT * prcClip)
{
    RECT        rc;
    RECT        rcInter;
    float       fPercent;

    // Calculate the full rect for the Health Bar
    fPercent = (float)(m_iCurr - m_iMin) / (float)(m_iMax - m_iMin);

    rc = m_rc;

    if (m_bFrame)
    {
        MyFrameRect(&rc, GetColor(128, 128, 128), pRaster);
        InflateRect(&rc, -1, -1);
    }

    rc.right = rc.left + (int)((float)(m_rc.right - m_rc.left) * fPercent);
    if (rc.right == rc.left)
    {
        rc.right++;
    }

    if (IntersectRect(&rcInter, &rc, prcClip))
    {
        MyFillRect(&rcInter, m_wClrFill, pRaster);
    }
}


int CUIProgressBar::DeltaPos(int iIncrement)
{
    int     iLast;

    iLast = m_iCurr;

    m_iCurr += iIncrement;

    if (m_iCurr > m_iMax)
    {
        m_iCurr = (m_iCurr - m_iMax) + m_iMin;
    }

    m_pParent->CtrlInvalidateRect(&m_rc);

    return iLast;
}


int CUIProgressBar::SetPos(int iNewPos)
{
    int     iLast;

    iLast = m_iCurr;

    if (iNewPos > m_iMax || iNewPos < m_iMin)
    {
        return m_iCurr;
    }

    m_iCurr = iNewPos;

    m_pParent->CtrlInvalidateRect(&m_rc);

    return iLast;
}


BOOL  CUIProgressBar::SetRange(int iMinRange, 
                               int iMaxRange,
                               int * piOldMin, 
                               int * piOldMax)
{
    if (iMaxRange - iMinRange <= 0)
    {
        return FALSE;
    }

    if (piOldMin)
    {
        *piOldMin = m_iMin;
    }

    if (piOldMax)
    {
        *piOldMax = m_iMax;
    }

    m_iMin = iMinRange;
    m_iMax = iMaxRange;

    return TRUE;
}


int CUIProgressBar::SetStep(int iStep)
{
    int iStepOld;

    iStepOld = m_iStep;
    m_iStep = iStep;

    return iStepOld;
}


int CUIProgressBar::StepIt()
{
    int iLast;

    iLast = m_iCurr;

    m_iCurr += m_iStep;

    if (m_iCurr > m_iMax)
    {
        m_iCurr = (m_iCurr - m_iMax) + m_iMin;
    }

    m_pParent->CtrlInvalidateRect(&m_rc);

    return iLast;
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

CUIHotspot::CUIHotspot(CUICtrlCallback * pParent) : 
    CUICtrl(pParent)
{
    m_bFrame = false;
}


CUIHotspot::~CUIHotspot()
{
}


void CUIHotspot::Draw(CRaster * pRDst, RECT * prcClip)
{
    if (m_bVisible && m_bFrame)
    {
        MyFrameRect(&m_rc, GetColor(255, 255, 255), pRDst);
    }
}

BOOL CUIHotspot::TokenLoad(HANDLE hFile)
{
    BOOL            bRet;
    WNDFILETOKEN    wft;

    bRet = CUICtrl::TokenLoad(hFile);
    if (!bRet)
    {
        TraceError(("CUIHotspot::TokenLoad() -- Base class load failed"));
        goto Error;
    }

    ReadWndFileToken(hFile, &wft);
    m_bFrame = wft.val.iVal;

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CUIHotspot::TokenLoad"));
    bRet = FALSE;
    goto Cleanup;
}


void CUIHotspot::MouseMove(int x, int y, UINT keyFlags)
{
}


BOOL CUIHotspot::LButtonDown(int x, int y, BOOL bDoubleClick, UINT keyFlags)
{
    m_pParent->UIHotspotClick(this, TRUE);

    return TRUE;
}


BOOL CUIHotspot::LButtonUp(int x, int y, UINT keyFlags)
{
    m_pParent->UIHotspotClick(this, FALSE);

    return TRUE;
}



#if 0
//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CUIXButton::CUIXButton(CUICtrlCallback * pParent) :
    CUIButton(pParent)
{
}



CUIXButton::~CUIXButton()
{
}



CRaster * CUIXButton::GetRaster()
{
    return m_apRaster[0];
}



void CUIXButton::Draw(CRaster * pRDst, RECT * prcClip)
{
    CRaster *       pRaster;
    int             iLeft;
    int             iTop;
    int             iIllum = 0;
    RECT            rc;
    RECT            rc2;

    if (!IntersectRect(&rc, &m_rc, prcClip))
    {
        return;
    }

    ::SetRect(&rc2, 
            rc.left - m_rc.left,
            rc.top - m_rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top);

    pRaster = GetRaster();

    if (!pRaster)
    {
        return;
    }

    if (m_bDown)
    {
        iLeft = m_rc.left + 2;
        iTop = m_rc.top + 2;
        iIllum = 50;
    }
    else
    {
        iLeft = m_rc.left;
        iTop = m_rc.top;
    }

    if (m_bOver)
    {
        iIllum = 50;
    }
    else if (!m_bActive)
    {
        iIllum = -50;
    }

    RasterBlt(pRaster,
              &rc2,
              pRDst,
              iLeft,
              iTop,
              TRUE,
              g_pMainWnd->m_pUIMgr->m_wTransColor,
              iIllum);
}


#if 0
BOOL CUIXButton::TokenLoad(CDWIStream * pstm)
{
    BOOL            bRet;
    LEVELFILETOKEN  lft;

    bRet = CUICtrl::TokenLoad(pstm);
    if (!bRet)
    {
        TraceError(("CUIXButton::TokenLoad() -- Base class load failed"));
        goto Error;
    }

    ReadLevelFileToken(pstm, &lft);
    Assert(lft.TokenType = LFTT_VALUE);
    m_bUseTrans = lft.val.iVal;

    ReadLevelFileToken(pstm, &lft);
    Assert(lft.TokenType = LFTT_STRING);

    m_apSprite[0] = LoadSprite(lft.val.sz, HD_CFS);

    Init();

    bRet = TRUE;

Cleanup:
    return bRet;

Error:
    TraceError(("CUIXButton::TokenLoad"));
    bRet = FALSE;
    goto Cleanup;
}
#endif



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


#endif

//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
