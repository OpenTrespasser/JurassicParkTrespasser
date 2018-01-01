//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       winctrls.cpp
//
//  Contents:   Code for overriding the windows control code.
//
//  Classes:    
//
//  Functions:
//
//  History:    05-Jan-98   SHernd   Created
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "winctrls.h"
#include "resource.h"
#include "rasterdc.hpp"
#include "supportfn.hpp"

extern HINSTANCE        g_hInst;

//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#define MAX_EDIT_LEN 256

#define COLOR3D_WHITE   RGB(255, 255, 255)
#define COLOR3D_LTGRAY  RGB(192, 192, 192)
#define COLOR3D_DKGRAY  RGB(127, 127, 127)
#define COLOR3D_BLACK   RGB(  0,   0,   0)


COLORREF    g_crEdgeColors[4][4] =
{
    { COLOR3D_WHITE,  COLOR3D_BLACK,  COLOR3D_LTGRAY, COLOR3D_DKGRAY }, // EDGE_RAISED
    { COLOR3D_DKGRAY, COLOR3D_WHITE,  COLOR3D_BLACK,  COLOR3D_LTGRAY }, // EDGE_SUNKEN
    { COLOR3D_LTGRAY, COLOR3D_DKGRAY, COLOR3D_DKGRAY, COLOR3D_LTGRAY }, // EDGE_BUMP
    { COLOR3D_DKGRAY, COLOR3D_LTGRAY, COLOR3D_LTGRAY, COLOR3D_DKGRAY }, // EDGE_ETCHED
};


BOOL MyInnerDrawEdge(HDC hdc, 
                     RECT * prc, 
                     UINT grfFlags, 
                     COLORREF crLT, 
                     COLORREF crRB)
{
    POINT       ptOrig;
    HPEN        hpenOld;
    HPEN        hpenLT;
    HPEN        hpenRB;

    hpenLT = CreatePen(PS_SOLID, 0, crLT);
    hpenRB = CreatePen(PS_SOLID, 0, crRB);
    hpenOld = SelectPen(hdc, hpenLT);

    // Draw Left Side
    MoveToEx(hdc, prc->left, prc->bottom, &ptOrig);
    if (grfFlags & BF_LEFT)
    {
        LineTo(hdc, prc->left, prc->top);
    }
    else
    {
        MoveToEx(hdc, prc->left, prc->top, NULL);
    }

    // Draw top
    if (grfFlags & BF_TOP)
    {
        LineTo(hdc, prc->right, prc->top);
    }
    else
    {
        MoveToEx(hdc, prc->right, prc->top, NULL);
    }

    SelectPen(hdc, hpenRB);

    // Draw right
    if (grfFlags & BF_RIGHT)
    {
        LineTo(hdc, prc->right, prc->bottom);
    }
    else
    {
        MoveToEx(hdc, prc->right, prc->bottom, NULL);
    }

    // Draw bottom
    if (grfFlags & BF_BOTTOM)
    {
        LineTo(hdc, prc->left, prc->bottom);
    }

    // Restore the DC info
    SelectPen(hdc, hpenOld);
    DeletePen(hpenLT);
    DeletePen(hpenRB);
    MoveToEx(hdc, ptOrig.x, ptOrig.y, NULL);

    return TRUE;
}


BOOL MyDrawEdge(HDC hdc, RECT * prc, UINT uiEdge, UINT grfFlags)
{
    RECT        rc;
    int         iStyle;

    SetRect(&rc, prc->left, prc->top, prc->right - 1, prc->bottom - 1);

    switch (uiEdge)
    {
        case EDGE_RAISED:
            iStyle = 0;
            break;

        case EDGE_SUNKEN:
            iStyle = 1;
            break;

        case EDGE_BUMP:
            iStyle = 2;
            break;

        case EDGE_ETCHED:
            iStyle = 3;
            break;
    }

    MyInnerDrawEdge(hdc, 
                    &rc, 
                    grfFlags,
                    g_crEdgeColors[iStyle][0],
                    g_crEdgeColors[iStyle][1]);

    InflateRect(&rc, -1, -1);
    MyInnerDrawEdge(hdc, 
                    &rc, 
                    grfFlags,
                    g_crEdgeColors[iStyle][2],
                    g_crEdgeColors[iStyle][3]);

    if (grfFlags & BF_ADJUST)
    {
        SetRect(prc, rc.left + 1, rc.top + 1, rc.right, rc.bottom);
    }

    return TRUE;
}



BOOL MyDrawDlgFrame(HDC hdc, RECT * prc)
{
    RECT        rc;
    int         iDlgFrame;

    // Get How big the dlg frame needs to be
    iDlgFrame = GetSystemMetrics(SM_CXFIXEDFRAME) - 1;

    SetRect(&rc, prc->left, prc->top, prc->right - 1, prc->bottom - 1);
    MyInnerDrawEdge(hdc, &rc, BF_RECT, g_crEdgeColors[0][0], g_crEdgeColors[0][1]);

    InflateRect(&rc, -1, -1);
    MyInnerDrawEdge(hdc, &rc, BF_RECT, g_crEdgeColors[0][2], g_crEdgeColors[0][3]);

    while (iDlgFrame-- > 0)
    {
        InflateRect(&rc, -1, -1);
        MyInnerDrawEdge(hdc, &rc, BF_RECT, COLOR3D_LTGRAY, COLOR3D_LTGRAY);
    }

    return TRUE;
}

void EraseParentBkgnd(HWND hwndChild, HDC hdc)
{
    RECT        rc;
    POINT       pt;
    POINT       ptOrig;

    GetClientRect(hwndChild, &rc);
    MapWindowRect(hwndChild, GetParent(hwndChild), &rc);

    GetViewportOrgEx(hdc, &ptOrig);

    rc.left -= ptOrig.x;
    rc.top -= ptOrig.y;
    SetViewportOrgEx(hdc, -rc.left, -rc.top, &pt);

    FORWARD_WM_ERASEBKGND(GetParent(hwndChild), hdc, SendMessage);

    SetViewportOrgEx(hdc, pt.x, pt.y, &pt);
}

void EraseParentBkgndSegment(HWND hwndChild, HDC hdc)
{
    RECT            rc;

    GetClipBox(hdc, &rc);

    MapWindowRect(hwndChild, GetParent(hwndChild), &rc);
    RedrawWindow(GetParent(hwndChild), 
                 &rc, 
                 NULL, 
                 RDW_ERASE | 
                 RDW_INVALIDATE | 
                 RDW_NOCHILDREN | 
                 RDW_NOINTERNALPAINT | 
                 RDW_ERASENOW | 
                 RDW_UPDATENOW);
}

HFONT CreateGameFont(int iHeight, int iWeight)
{
    HFONT   hfont;

    if (iHeight == -1)
    {
        iHeight = DEFAULT_GAMEFONTHEIGHT;
    }

    if (iWeight == -1)
    {
        iWeight = DEFUALT_GAMEFONTWEIGHT;
    }

    hfont = CreateFont(-iHeight,
                       0,
                       0,
                       0,
                       iWeight,
                       FALSE,
                       FALSE,
                       FALSE,
                       DEFAULT_CHARSET,
                       OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY,
                       FF_MODERN,
                       GAMEFONTNAME);

    return hfont;
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

COwnerSubclassBase::COwnerSubclassBase()
{
    m_wndproc = NULL;
}


COwnerSubclassBase::~COwnerSubclassBase()
{
}


void COwnerSubclassBase::Subclass(HWND hwnd)
{
    m_wndproc = SubclassWindow(hwnd, CMultiDlgProc);
    SetProp(hwnd, "CMultiBase Class", this);
    m_hwnd = hwnd;
}

LRESULT COwnerSubclassBase::BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (m_wndproc)
    {
        return CallWindowProc(m_wndproc, hwnd, message, wParam, lParam);
    }

    return CMultiWnd::BaseHandler(hwnd, message, wParam, lParam);
}

void COwnerSubclassBase::OnDestroy(HWND hwnd)
{
    CMultiWnd::OnDestroy(hwnd);
    delete this;
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


COwnerCheckbox::COwnerCheckbox()
{
    m_hbm = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CHECKBOX)); 
    GetObject(m_hbm, sizeof(m_bm), &m_bm);
    m_iCheckWidth = m_bm.bmWidth;
    m_iCheckHeight = m_bm.bmHeight / 6;
    m_bTextRect = false;
    m_bKeyDown = false;
}

COwnerCheckbox::~COwnerCheckbox()
{
    DeleteObject(m_hbm);
}


LRESULT COwnerCheckbox::BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case BM_GETCHECK:
            return m_dwState & 0x3;
            break;

        case BM_SETCHECK:
            m_dwState &= ~0x3;
            m_dwState |= (wParam & 0x3);
            DrawCheckPart(NULL, m_dwState & BST_CHECKED, FALSE);
            return 0;
            break;

        case BM_GETSTATE:
            return m_dwState;
            break;

        case BM_SETSTATE:
            DrawCheckPart(NULL, m_dwState & BST_CHECKED, wParam);
            return 0;
            break;
    }

    return COwnerSubclassBase::BaseHandler(hwnd, message, wParam, lParam);
}

void COwnerCheckbox::Subclass(HWND hwnd)
{
    m_dwState = SendMessage(hwnd, BM_GETSTATE, 0, 0);
    m_dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    Button_SetStyle(hwnd, BS_OWNERDRAW, false);

    COwnerSubclassBase::Subclass(hwnd);

    GetClientRect(hwnd, &m_rc);
}

void COwnerCheckbox::OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (vk != VK_SPACE)
    {
        CMultiWnd::OnKey(hwnd, vk, fDown, cRepeat, flags);
    }

    if (fDown)
    {
        m_bKeyDown = true;
    }
    else
    {
        m_bKeyDown = false;

        if (m_dwState & BST_CHECKED)
        {
            m_dwState &= ~0x3;
            m_dwState |= BST_UNCHECKED;
        }
        else
        {
            m_dwState &= ~0x3;
            m_dwState |= BST_CHECKED;
        }
    }

    DrawCheckPart(NULL, m_dwState & BST_CHECKED, m_bKeyDown);
}


UINT COwnerCheckbox::OnNCHitTest(HWND hwnd, int x, int y)
{
    RECT        rc;
    POINT       pt;

    GetWindowRect(hwnd, &rc);
    pt.x = x;
    pt.y = y;

    if (PtInRect(&rc, pt))
    {
        return HTCLIENT;
    }
    else
    {
        return HTNOWHERE;
    }
}

void COwnerCheckbox::OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    BOOL    bDown;

    if (GetCapture() != hwnd)
    {
        return;
    }

    if (x < 0 || y < 0 || x > m_rc.right || y > m_rc.bottom)
    {
        bDown = FALSE;
    }
    else
    {
        bDown = TRUE;
    }
    
    DrawCheckPart(NULL, m_dwState & BST_CHECKED, bDown);
}

void COwnerCheckbox::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    SetCapture(hwnd);
    SetFocus(hwnd);
    DrawCheckPart(NULL, m_dwState & BST_CHECKED, TRUE);
}

void COwnerCheckbox::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    if (GetCapture() != hwnd)
    {
        return;
    }

    ReleaseCapture();

    if (x < 0 || y < 0 || x > m_rc.right || y > m_rc.bottom)
    {
        return;
    }

    if (m_dwState & BST_CHECKED)
    {
        m_dwState &= ~0x3;
        m_dwState |= BST_UNCHECKED;
    }
    else
    {
        m_dwState &= ~0x3;
        m_dwState |= BST_CHECKED;
    }

    DrawCheckPart(NULL, m_dwState & BST_CHECKED, FALSE);
}


void COwnerCheckbox::SetupTextRect(HDC hdc)
{
    int                 i;
    BOOL                bLeftText;
    char                sz[512];
    HFONT               hfont;
    HFONT               hfontOld;
    int                 iHeight;
    BOOL                bGotDC = FALSE;

    if (hdc == NULL)
    {
        bGotDC = TRUE;
        hdc = GetDC(m_hwnd);
    }

    // Chose 1 horizontal alignment
    if (m_dwStyle & BS_CENTER)
    {
        m_dwTextFlags = DT_CENTER;
    }
    else if (m_dwStyle & BS_RIGHT)
    {
        m_dwTextFlags = DT_RIGHT;
    }
    else
    {
        m_dwTextFlags = DT_LEFT;
    }

    // Chose 1 vertical alignment
    switch ((m_dwStyle & (BS_TOP | BS_BOTTOM | BS_VCENTER)) >> 10)
    {
        default:
        case 3:
            m_dwTextFlags |= DT_VCENTER;
            break;

        case 2:
            m_dwTextFlags |= DT_BOTTOM;
            break;

        case 1:
            m_dwTextFlags |= DT_TOP;
            break;
    }

    // Chose multi-line or single-line
    if (!(m_dwStyle & BS_MULTILINE))
    {
        m_dwTextFlags |= DT_SINGLELINE;
    }
    else
    {
        m_dwTextFlags |= DT_WORDBREAK;
    }

    m_rcText = m_rc;

    // if this is non-zero then the check box is on the right hand side.
    bLeftText = FALSE;
    if (m_dwStyle & BS_LEFT)
    {
        bLeftText = TRUE;
        m_rcText.right -= m_iCheckWidth + 4;
    }
    else
    {
        m_rcText.left += m_iCheckWidth + 4;
    }

    GetWindowText(m_hwnd, sz, sizeof(sz));
    hfont = GetWindowFont(m_hwnd);
    
    hfontOld = SelectFont(hdc, hfont);

    iHeight = DrawTextEx(hdc, sz, -1, &m_rcText, m_dwTextFlags | DT_CALCRECT, NULL);

    i = (((m_rc.bottom - m_rc.top) / 2) - (iHeight / 2));
    OffsetRect(&m_rcText, 0, i);

    SelectFont(hdc, hfontOld);

    if (bGotDC == TRUE)
    {
        ReleaseDC(m_hwnd, hdc);
    }

    m_bTextRect = TRUE;
}

void COwnerCheckbox::DrawFocusRect(HDC hdc)
{
    BOOL    bGotDC = FALSE;
    RECT    rc;

    if (hdc == NULL)
    {
        bGotDC = TRUE;
        hdc = GetDC(m_hwnd);
    }

    if (!m_bTextRect)
    {
        SetupTextRect(hdc);
    }

    rc = m_rcText;
    rc.left -= 2;
    rc.right += 2;
    rc.bottom += 2;

    ::DrawFocusRect(hdc, &rc);

    if (bGotDC)
    {
        ReleaseDC(m_hwnd, hdc);
    }
}

void COwnerCheckbox::OnPaint(HWND hwnd)
{
    PAINTSTRUCT         ps;
    HDC                 hdc;
    char                sz[512];
    HFONT               hfont;
    HFONT               hfontOld;
    int                 iBkMode;
    BOOL                bEnabled;
    COLORREF            crTextOld;
    COLORREF            crFg;
    COLORREF            crBk;

    hdc = BeginPaint(hwnd, &ps);

    if (!m_bTextRect)
    {
        SetupTextRect(hdc);
    }

    // Get the check state
    DrawCheckPart(hdc, m_dwState & BST_CHECKED, FALSE);

    bEnabled = IsWindowEnabled(m_hwnd);

    GetWindowText(hwnd, sz, sizeof(sz));
    hfont = GetWindowFont(hwnd);
    
    hfontOld = SelectFont(hdc, hfont);
    iBkMode = SetBkMode(hdc, TRANSPARENT);

    if (!bEnabled)
    {
        crBk = GetSysColor(COLOR_3DHILIGHT);
        crFg =GetSysColor(COLOR_GRAYTEXT);
    }
    else
    {
        crBk = COLOR_NORMAL_BK;
        crFg = COLOR_NORMAL_FG;
    }

    OffsetRect(&m_rcText, 1, 1);
    crTextOld = SetTextColor(hdc, crBk);
    DrawTextEx(hdc, sz, -1, &m_rcText, m_dwTextFlags, NULL);

    OffsetRect(&m_rcText, -1, -1);
    SetTextColor(hdc, crFg);
    DrawTextEx(hdc, sz, -1, &m_rcText, m_dwTextFlags, NULL);

    SetTextColor(hdc, crTextOld);

    if (m_dwState & BST_FOCUS)
    {
        DrawFocusRect(hdc);
    }

    SetBkMode(hdc, iBkMode);
    SelectFont(hdc, hfontOld);

    EndPaint(hwnd, &ps);
}

void COwnerCheckbox::OnSetFocus(HWND hwnd, HWND hwndOldFocus)
{
    m_dwState |= BST_FOCUS;
    DrawFocusRect(NULL);
}

void COwnerCheckbox::OnKillFocus(HWND hwnd, HWND hwndNewFocus)
{
    m_dwState &= ~BST_FOCUS;
    DrawFocusRect(NULL);
}

void COwnerCheckbox::DrawCheckPart(HDC hdc, BOOL bChecked, BOOL bDown)
{
    BOOL    bGotDC = FALSE;
    HDC     hdcMem;
    HBITMAP hbmOld;
    int     xIndex;
    int     yIndex;
    int     xDst;
    int     yDst;
    BOOL    bEnabled;

    // if the hdc == NULL then do a GetDC
    if (hdc == NULL)
    {
        bGotDC = TRUE;
        hdc = GetDC(m_hwnd);
    }

    bEnabled = IsWindowEnabled(m_hwnd);

    if (!bChecked)
    {
        xIndex = 0;
        yIndex = 0;
    }
    else
    {
        xIndex = 0;
        yIndex = 1;
    }

    if (bDown || !bEnabled)
    {
        yIndex += 2;
    }

    hdcMem = CreateCompatibleDC(hdc);
    hbmOld = SelectBitmap(hdcMem, m_hbm);

    if (m_dwStyle & BS_LEFT)
    {
        xDst = m_rc.right - m_iCheckWidth;
    }
    else
    {
        xDst = m_rc.left;
    }

    // Always center half way down the section
    yDst = m_rc.top + (((m_rc.bottom - m_rc.top) / 2) - (m_iCheckHeight / 2));

    // Draw the Check Mark
    BitBlt(hdc, 
           xDst, 
           yDst, 
           m_iCheckWidth, 
           m_iCheckHeight, 
           hdcMem, 
           xIndex * m_iCheckWidth, 
           yIndex * m_iCheckHeight, 
           SRCCOPY);

    // Restore Original State and Free Resources
    SelectBitmap(hdcMem, hbmOld);

    if (bGotDC)
    {
        ReleaseDC(m_hwnd, hdc);
    }
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


COwnerButton::COwnerButton()
{
    m_prasImg = NULL;
    m_prasBkgnd = NULL;
}


COwnerButton::~COwnerButton()
{
    delete m_prasImg;
    delete m_prasBkgnd;
}


BOOL COwnerButton::Init(CRasterMem * praster)
{
    delete m_prasImg;
    delete m_prasBkgnd;

    m_prasImg = praster;

    m_iWidth = m_prasImg->iWidth;
    m_iHeight = m_prasImg->iHeight / 4;

    m_prasBkgnd = new CRasterDC(m_hwnd, m_iWidth, m_iHeight, 16);

    m_prasImg->Lock();
    Assert(m_prasImg->pSurface);
    m_wTrans = *((WORD*)m_prasImg->pSurface);
    m_prasImg->Unlock();

    SetWindowPos(m_hwnd, 
                 NULL, 
                 0, 
                 0, 
                 m_iWidth, 
                 m_iHeight, 
                 SWP_NOMOVE | 
                 SWP_NOACTIVATE | 
                 SWP_NOCOPYBITS |
                 SWP_NOOWNERZORDER |
                 SWP_NOREDRAW | SWP_NOSENDCHANGING);

    return TRUE;
}


void COwnerButton::Draw(const DRAWITEMSTRUCT * lpDrawItem)
{
    int     ySrc;
    HDC     hdcBkgnd;
    RECT    rcSrc;

    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        ySrc = 3;
    }
    else if (lpDrawItem->itemState & ODS_FOCUS)
    {
        ySrc = 1;
    }
    else if (lpDrawItem->itemState & ODS_DISABLED)
    {
        ySrc = 2;
    }
    else
    {
        ySrc = 0;
    }

    ySrc *= m_iHeight;

    // Update the background
    hdcBkgnd = m_prasBkgnd->hdcGet();
    EraseParentBkgnd(lpDrawItem->hwndItem, hdcBkgnd);
    m_prasBkgnd->ReleaseDC(hdcBkgnd);

    // Now blt the transparent image onto the background
    SetRect(&rcSrc, 0, ySrc, m_iWidth, ySrc + m_iHeight);
    RasterBlt(m_prasImg, 
              &rcSrc, 
              m_prasBkgnd, 
              0, 
              0,
              true,
              m_wTrans,
              0);

    hdcBkgnd = m_prasBkgnd->hdcGet();

    BitBlt(lpDrawItem->hDC,
           0,
           0,
           m_iWidth,
           m_iHeight,
           hdcBkgnd,
           0,
           0,
           SRCCOPY);

    m_prasBkgnd->ReleaseDC(hdcBkgnd);
}


UINT COwnerButton::OnNCHitTest(HWND hwnd, int x, int y)
{
    SetFocus(m_hwnd);

    return HTCLIENT;
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


COwnerTab::COwnerTab()
{
}

COwnerTab::~COwnerTab()
{
}


void COwnerTab::Subclass(HWND hwnd)
{
    COwnerSubclassBase::Subclass(hwnd);
}


LRESULT COwnerTab::BaseHandler(HWND hwnd, 
                               UINT message, 
                               WPARAM wParam, 
                               LPARAM lParam)
{
    return COwnerSubclassBase::BaseHandler(hwnd, message, wParam, lParam);
}

BOOL COwnerTab::OnEraseBkgnd(HWND hwnd, HDC hdcDst)
{
    EraseParentBkgnd(hwnd, hdcDst);

    return TRUE;
}


void COwnerTab::DrawTabEdge(RECT * prc, HDC hdc)
{
    POINT       ptOrig;
    HPEN        hpenOld;
    HPEN        hpen;

    hpen = CreatePen(PS_SOLID, 0, COLOR3D_WHITE);
    hpenOld = SelectPen(hdc, hpen);

    //
    // Draw Left-top edge of the tab
    //

    // Draw the outer edge of the tab
    MoveToEx(hdc, prc->left, prc->bottom - 1, &ptOrig);
    LineTo(hdc, prc->left, prc->top + 2);

    // Draw Diagonal little piece
    LineTo(hdc, prc->left + 2, prc->top);

    // Draw top edge 
    LineTo(hdc, prc->right - 2, prc->top);

    // Draw the inner part of the left side

    SelectPen(hdc, hpenOld);
    DeleteObject(hpen);
    hpen = CreatePen(PS_SOLID, 0, COLOR3D_LTGRAY);
    hpenOld = SelectPen(hdc, hpen);

    MoveToEx(hdc, prc->left + 1, prc->bottom, NULL);
    LineTo(hdc, prc->left + 1, prc->top + 1);

    // Draw the inner part of the top
    MoveToEx(hdc, prc->left + 2, prc->top + 1, NULL);
    LineTo(hdc, prc->right - 1, prc->top + 1);

    //
    // Draw the right side of the tab
    //

    SelectPen(hdc, hpenOld);
    DeleteObject(hpen);
    hpen = CreatePen(PS_SOLID, 0, COLOR3D_DKGRAY);
    hpenOld = SelectPen(hdc, hpen);

    // Draw the inner part of the right edge

    MoveToEx(hdc, prc->right - 1, prc->top + 2, NULL);
    LineTo(hdc, prc->right - 1, prc->bottom + 1);

    SelectPen(hdc, hpenOld);
    DeleteObject(hpen);

    hpen = CreatePen(PS_SOLID, 0, COLOR3D_BLACK);
    MoveToEx(hdc, prc->right - 1, prc->top + 1, NULL);
    LineTo(hdc, prc->right, prc->top + 2);
    LineTo(hdc, prc->right, prc->bottom + 1);

    // Restore the original HDC info
    SelectPen(hdc, hpenOld);
    DeleteObject(hpen);
    MoveToEx(hdc, ptOrig.x, ptOrig.y, NULL);
}


void COwnerTab::DrawTab(int iTab, 
                        BOOL bSel, 
                        BOOL bFocus, 
                        HDC hdc, 
                        RECT * prcBounding)
{
    TC_ITEM         tci;
    RECT            rc;
    char            sz[512];

    tci.mask = TCIF_TEXT;
    tci.pszText = sz;
    tci.cchTextMax = sizeof(sz);

    TabCtrl_GetItem(m_hwnd, iTab, &tci);
    TabCtrl_GetItemRect(m_hwnd, iTab, &rc);

    if (bSel)
    {
        InflateRect(&rc, 2, 2);
    }
    else
    {
        rc.bottom += 1;
    }

    if (prcBounding)
    {
        *prcBounding = rc;
    }

    // Make Drawing Callback if necessary
    if (GetWindowLong(m_hwnd, GWL_STYLE) & TCS_OWNERDRAWFIXED)
    {
        DRAWITEMSTRUCT      dis;

        dis.CtlType = ODT_TAB;
        dis.CtlID = GetDlgCtrlID(m_hwnd);
        dis.itemID = iTab;
        dis.itemAction = ODA_DRAWENTIRE;
        dis.itemState = 0;
        if (bSel)
        {
            dis.itemState |= ODS_SELECTED;
        }
        if (bFocus)
        {
            dis.itemState |= ODS_FOCUS;
        }
        if (!IsWindowEnabled(m_hwnd))
        {
            dis.itemState |= ODS_DISABLED;
        }
        dis.hwndItem = m_hwnd;
        dis.hDC = hdc;
        dis.rcItem = rc;
        dis.itemData = 0;

        FORWARD_WM_DRAWITEM(GetParent(m_hwnd), &dis, SendMessage);
    }
    else
    {
        DrawTextEx(hdc, sz, -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE, NULL);
    }

    rc.right -= 1;
    DrawTabEdge(&rc, hdc);
}

void COwnerTab::OnPaint(HWND hwnd)
{
    HDC             hdc;
    PAINTSTRUCT     ps;
    int             iCount;
    int             iCurFocus;
    int             iCurSel;
    RECT            rc;
    RECT            rcFinal;
    int             icRows;
    int             i;
    RECT            rcDisplay;
    int             iBkMode;
    HFONT           hfontOld;
    
    hdc = BeginPaint(hwnd, &ps);

    iCount = TabCtrl_GetItemCount(m_hwnd);
    iCurFocus = TabCtrl_GetCurFocus(m_hwnd);
    iCurSel = TabCtrl_GetCurSel(m_hwnd);
    icRows = TabCtrl_GetRowCount(m_hwnd);

    // Get the client region from the Tab Control given it's new size
    GetClientRect(m_hwnd, &rcDisplay);
    TabCtrl_AdjustRect(m_hwnd, FALSE, &rcDisplay);
    m_iAdjustSize = rcDisplay.left;

    InflateRect(&rcDisplay, m_iAdjustSize, m_iAdjustSize);

    // Draw the main tab border
    rcDisplay.top += 2;
    MyDrawEdge(hdc, &rcDisplay, EDGE_RAISED, BF_LEFT | BF_BOTTOM | BF_RIGHT);

    iBkMode = SetBkMode(hdc, TRANSPARENT);
    hfontOld = SelectFont(hdc, GetWindowFont(m_hwnd));

    for (i = 0; i < iCount; i++)
    {
        if (iCurSel != i)
        {
            DrawTab(i, FALSE, FALSE, hdc, NULL);
        }
    }

    if (iCurSel != -1)
    {
        DrawTab(iCurSel, TRUE, FALSE, hdc, &rc);
    }

    // Draw the Continuance edge from the last item to the right hand
    // side of the tab window
    SetRect(&rcFinal, 
            rcDisplay.left,
            rcDisplay.top, //rc.bottom,
            rc.left + 2,
            rcDisplay.bottom);

    if (iCurSel != 0)
    {
        MyDrawEdge(hdc, &rcFinal, EDGE_RAISED, BF_TOP);
    }

    rcFinal.left = rc.right;
    rcFinal.right = rcDisplay.right;
    MyDrawEdge(hdc, &rcFinal, EDGE_RAISED, BF_TOP);

    SelectFont(hdc, hfontOld);
    SetBkMode(hdc, iBkMode);

    EndPaint(hwnd, &ps);
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


COwnerSlider::COwnerSlider()
{
}


COwnerSlider::~COwnerSlider()
{
}


BOOL COwnerSlider::ValidDrawStyle()
{
    LONG            lStyle;

    lStyle = GetWindowLong(m_hwnd, GWL_STYLE);

    if (lStyle & (TBS_VERT | TBS_ENABLESELRANGE | TBS_NOTHUMB | TBS_TOOLTIPS))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL COwnerSlider::OnEraseBkgnd(HWND hwnd, HDC hdcDst)
{
    if (!ValidDrawStyle())
    {
        return COwnerSubclassBase::OnEraseBkgnd(hwnd, hdcDst);
    }

    EraseParentBkgnd(hwnd, hdcDst);

    return TRUE;
}


void COwnerSlider::OnPaint(HWND hwnd)
{
    PAINTSTRUCT     ps;
    HDC             hdc;
    RECT            rcWindow;
    RECT            rc;
    LONG            lStyle;
    HBRUSH          hbr;
    DWORD           dwBatchLimit;

    if (!ValidDrawStyle())
    {
        COwnerSubclassBase::OnPaint(hwnd);
        return;
    }

    lStyle = GetWindowLong(m_hwnd, GWL_STYLE);

    hdc = BeginPaint(hwnd, &ps);

    dwBatchLimit = GdiSetBatchLimit(50);

    if (!ps.fErase)
    {
        POINT       pt;
        RECT        rc;

        GetClientRect(m_hwnd, &rc);
        MapWindowRect(m_hwnd, GetParent(m_hwnd), &rc);

        SetViewportOrgEx(hdc, -rc.left, -rc.top, &pt);
        FORWARD_WM_ERASEBKGND(GetParent(m_hwnd), hdc, SendMessage);
        SetViewportOrgEx(hdc, pt.x, pt.y, &pt);
    }

    GetClientRect(m_hwnd, &rcWindow);

    //
    // Draw Gutter
    //
    SendMessage(m_hwnd, TBM_GETCHANNELRECT, 0, (LPARAM)&rc);
    MyDrawEdge(hdc, &rc, EDGE_SUNKEN, BF_RECT);

    //
    // Draw Thumb
    //
    SendMessage(m_hwnd, TBM_GETTHUMBRECT, 0, (LPARAM)&rc);
    MyDrawEdge(hdc, &rc, EDGE_RAISED, BF_RECT | BF_ADJUST);

    hbr = CreateSolidBrush(RGB(160, 160, 160));
    FillRect(hdc, &rc, hbr);
    DeleteBrush(hbr);

    GdiSetBatchLimit(dwBatchLimit);

    //
    // Draw Tick Marks if necessary
    //

    // BUGBUG - Tick mark drawing is not implemented

    EndPaint(hwnd, &ps);
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


COwnerListBox::COwnerListBox(HWND hwndList)
{
	nLastIndex = -1;
	nSelectedItem = -1;
}

COwnerListBox::~COwnerListBox()
{
}

void COwnerListBox::OnSetFocus(HWND hwnd, HWND hwndOldFocus)
{
    m_dwState |= BST_FOCUS;
}

void COwnerListBox::OnKillFocus(HWND hwnd, HWND hwndNewFocus)
{
    m_dwState &= ~BST_FOCUS;
}

/*
void COwnerListBox::OnMouseMove(HWND hwnd, int MouseX, int MouseY, UINT keyFlags)
{
	CallWindowProc(m_wndproc, hwnd, WM_MOUSEMOVE, keyFlags, MAKELONG(MouseX, MouseY));
}
*/
void COwnerListBox::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int MouseX, int MouseY, UINT keyFlags)
{	
	RECT	rc;
	int		nOldItem, nNewItem, nTextHeight;
	// Get the first visible string in the list
	int		nTopIndex = ListBox_GetTopIndex(hwnd); 
	HDC		hdc = GetDC(hwnd);

//	SetCapture(hwnd);
	// turn off drawing to the screen
	SetWindowRedraw(hwnd, FALSE);
	nOldItem = ListBox_GetCurSel(hwnd);

	LockWindowUpdate(hwnd);
	CallWindowProc(m_wndproc, hwnd, WM_LBUTTONDOWN, keyFlags, MAKELONG(MouseX, MouseY));
	LockWindowUpdate(NULL);
	// Calculate the selected item from the position and height
	nTextHeight = ListBox_GetItemHeight(hwnd, 0); // The height is fixed therefore same for each entry
	nSelectedItem  = (MouseY / nTextHeight) + nTopIndex;

	// Is the mouse within the selected rectangle
	ListBox_GetItemRect(hwnd, nSelectedItem, &rc);
	if (!(MouseX >= rc.left && MouseX <= rc.right))
		return;

	nNewItem = nSelectedItem;
	ListBox_SetCurSel(hwnd, nSelectedItem);

	// redraw background if we have scrolled
	if (nTopIndex != ListBox_GetTopIndex(hwnd))
	{
		OnEraseBkgndSegment(hwnd, hdc, NULL);
		GetClientRect(hwnd, &rc);
		InvalidateRect(hwnd, &rc, TRUE);
	}
	else
	if (nOldItem != nNewItem)
	{
		if (nOldItem >=0 )
		{
			ListBox_GetItemRect(hwnd, nOldItem, &rc); 
			InvalidateRect(hwnd, &rc, TRUE);
		}
		// Invalidate the new item
		if (nNewItem >= 0)
		{
			ListBox_GetItemRect(hwnd, nNewItem, &rc); 
			InvalidateRect(hwnd, &rc, TRUE);
		}
	}
	// turn on drawing again
	SetWindowRedraw(hwnd, TRUE);
//	SetReturnResult(hwnd, TRUE);
	ReleaseDC(hwnd, hdc);

	if (nSelectedItem >= 0)
		EnableWindow(GetDlgItem(GetParent(hwnd), IDOK), TRUE);

	// This applies to the load game menu.
	// It is here because with the Windows setfocus not being called
	// the LBN_SELCHANGE call is not generated
	// The Setfocus is not called to eliminate the rectangle flash
	// If the reader knows how else to deal with this please inform me - Floria
	// Thanks
	SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDC_FILES, LBN_SELCHANGE), (long)hwnd);

//	ReleaseCapture();
}

void COwnerListBox::OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	RECT	rc;

	// disallow debounce
	if (bKeyDownFlag)
	{
		bKeyDownFlag = fDown;
		return;
	}

	bKeyDownFlag = fDown;

	// Ignore the keyup
	if (!fDown)
		return;

	switch (vk)
	{
		case VK_END:   
		case VK_HOME:
		case VK_UP:   
		case VK_DOWN:
		case VK_PRIOR:   
		case VK_NEXT:
		{
			// Get the first visible string in the list
			int nTopIndex = ListBox_GetTopIndex(hwnd); 
			int nOldItem;
			HDC	hdc = GetDC(hwnd);

			SetWindowRedraw(hwnd, FALSE);

			nOldItem = nSelectedItem;
			CallWindowProc(m_wndproc, hwnd, WM_KEYDOWN, vk, flags);
			nSelectedItem = ListBox_GetCurSel(hwnd);

			// redraw background if we have scrolled
			if (nTopIndex != ListBox_GetTopIndex(hwnd))
			{
				OnEraseBkgndSegment(hwnd, hdc, NULL);
				GetClientRect(hwnd, &rc);
				InvalidateRect(hwnd, &rc, TRUE);
			}
			else
			if (nOldItem != nSelectedItem)
			{
				if (nOldItem >=0 )
				{
					ListBox_GetItemRect(hwnd, nOldItem, &rc); 
					InvalidateRect(hwnd, &rc, TRUE);
				}

				// Invalidate the new item
				if (nSelectedItem >= 0)
				{
					ListBox_GetItemRect(hwnd, nSelectedItem, &rc); 
					InvalidateRect(hwnd, &rc, TRUE);
				}
			}

			GetClientRect(hwnd, &rc);
			InvalidateRect(hwnd, &rc, TRUE);
			SetWindowRedraw(hwnd, TRUE);
			UpdateWindow(hwnd);
			ReleaseDC(hwnd, hdc);

			// This applies to the load game menu.
			// It is here because with the Windows setfocus not being called
			// the LBN_SELCHANGE call is not generated
			// The Setfocus is not called to eliminate the rectangle flash
			// If the reader knows how else to deal with this please inform me - Floria
			// Thanks
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDC_FILES, LBN_SELCHANGE), (long)hwnd);
		}
			break;
		default:
			break;
	}
}
	
void COwnerListBox::OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	RECT    rc;

	if (code >= 0 &&  code < SB_ENDSCROLL)
	{
		HDC	hdc = GetDC(hwnd);

		SetWindowRedraw(hwnd, FALSE);
		// pos is hi word and code is low word
		CallWindowProc(m_wndproc, hwnd, WM_HSCROLL, MAKELONG(code, pos), (long)hwndCtl);
		GetClientRect(hwnd, &rc);
		InvalidateRect(hwnd, &rc, TRUE);
		SetWindowRedraw(hwnd, TRUE);
		OnEraseBkgndSegment(hwnd, hdc, NULL);
		UpdateWindow(hwnd);
		ReleaseDC(hwnd, hdc);
	}
}
	
void COwnerListBox::OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{	
	RECT    rc;
	if (code >= 0 && code < SB_ENDSCROLL)
	{
		HDC	hdc = GetDC(hwnd);

		SetWindowRedraw(hwnd, FALSE);
		LockWindowUpdate(hwnd);
		// pos is hi word and code is low word
		CallWindowProc(m_wndproc, hwnd, WM_VSCROLL, MAKELONG(code, pos), (long)hwndCtl);
		LockWindowUpdate(NULL);
		SetWindowRedraw(hwnd, TRUE);
		GetClientRect(hwnd, &rc);
		InvalidateRect(hwnd, &rc, TRUE);
		OnEraseBkgndSegment(hwnd, hdc, NULL);
		UpdateWindow(hwnd);
		ReleaseDC(hwnd, hdc);
	}
}	

LRESULT COwnerListBox::BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
/*	switch (message)
	{
		case LB_GETTEXT:
			{
				int nresult;

				// wParam is the index
				strcpy((char *)lParam, ptrOwnerListNames+(wParam * _MAX_FNAME));
				nresult = strlen((char *)ptrOwnerListNames+(wParam * _MAX_FNAME));
//				nresult = COwnerSubclassBase::BaseHandler(hwnd, message, wParam, lParam);
				return nresult;
				break;
			}
		case LB_ADDSTRING:
			return COwnerSubclassBase::BaseHandler(hwnd, message, wParam, lParam);
			break;

		case LB_GETCURSEL:
//			nSelectedItem =  COwnerSubclassBase::BaseHandler(hwnd, message, wParam, lParam);
			return nSelectedItem;
			break;

		case LB_SETCURSEL:
			g_SelectListbox = nSelectedItem = wParam;
//			return COwnerSubclassBase::BaseHandler(hwnd, message, wParam, lParam);
			return nSelectedItem;
			break;
			
		default:
			break;
	}
*/    return COwnerSubclassBase::BaseHandler(hwnd, message, wParam, lParam);
}

COwnerComboBox::COwnerComboBox()
{
}


COwnerComboBox::~COwnerComboBox()
{
}

LRESULT COwnerComboBox::BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_LBUTTONDOWN)
	{
		SetFocus(hwnd);
		return COwnerSubclassBase::BaseHandler(hwnd, message, wParam, lParam);
		
	}
    return COwnerSubclassBase::BaseHandler(hwnd, message, wParam, lParam);
}


HBRUSH COwnerComboBox::OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
	return (HBRUSH)GetStockObject(DKGRAY_BRUSH);
}

BOOL OnEraseBkgndSegment(HWND hwnd, HDC hdcDst, RECT *lprcValues)
{
    HRGN        hrgnWindow;
    HRGN        hrgnClip;
    RECT        rc;

    if (lprcValues)
	{
		rc = *lprcValues;
	}
	else
		GetClientRect(hwnd, &rc);

    hrgnWindow = CreateRectRgnIndirect(&rc);
    hrgnClip = CreateRectRgnIndirect(&rc);

    GetClipRgn(hdcDst, hrgnClip);

    SelectClipRgn(hdcDst, hrgnWindow);

    if (lprcValues)
	    EraseParentBkgndSegment(hwnd, hdcDst);
	else
	    EraseParentBkgnd(hwnd, hdcDst);

    // Restore the DC to the original state
    SelectClipRgn(hdcDst, hrgnClip);

    // Free up allocated resources
    DeleteObject(hrgnClip);
    DeleteObject(hrgnWindow);

    return TRUE;
}

//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

COwnerEdit::COwnerEdit()
{
	cxChar = cxClient = cxBuffer = xCaret = 0;
	pBuffer[0] = '\0';
	iEndHilite = iStartHilite = 0;
	iLastMouseX = 0;
	bHiliteFlag = FALSE;
	m_dwState = 0;
}


COwnerEdit::~COwnerEdit()
{
}

BOOL COwnerEdit::OnEraseBkgnd(HWND hwnd, HDC hdcDst)
{
    HRGN        hrgnWindow;
    HRGN        hrgnTmp;
    HRGN        hrgnClip;
    RECT        rc;

    GetWindowRect(hwnd, &rc);
    MapWindowRect(hwnd, NULL, &rc);
    hrgnWindow = CreateRectRgnIndirect(&rc);
    hrgnClip = CreateRectRgnIndirect(&rc);
    hrgnTmp = CreateRectRgnIndirect(&rc);

    GetClipRgn(hdcDst, hrgnClip);
    CombineRgn(hrgnTmp, hrgnWindow, hrgnClip, RGN_AND);
    SelectClipRgn(hdcDst, hrgnTmp);

    EraseParentBkgnd(hwnd, hdcDst);

    // Restore the DC to the original state
    SelectClipRgn(hdcDst, hrgnClip);

    // Free up allocated resources
    DeleteObject(hrgnTmp);
    DeleteObject(hrgnClip);
    DeleteObject(hrgnWindow);

    return TRUE;
}

void COwnerEdit::OnMouseMove(HWND hwnd, int MouseX, int MouseY, UINT keyFlags)
{
    HDC         hdc = GetDC(hwnd);
    RECT		rc;
	
	BOOL bDone = FALSE;
	int i = 0;
	char cBuildStr[MAX_EDIT_LEN] = {"\0"};
	int  iLastX = 0;

	if (keyFlags & MK_LBUTTON)
	{
		// find the character index that is covered 
		while (i <= strlen(pBuffer) && !bDone)
		{
			strncat(cBuildStr, &pBuffer[i], 1);
			GetTextExtentPoint32(hdc, (LPCTSTR)cBuildStr, 
				(int)strlen(cBuildStr), (LPSIZE)&HilitePos);

			// if the width is greater then the mouse position place
			// caret in at this postion
			if (HilitePos.cx >= MouseX)
			{
				// Moving left to right
				if (MouseX >= iLastMouseX)
				{
					if ((int)(MouseX - iLastX) > (int)(HilitePos.cx - MouseX))
					{
						iEndHilite = min((int)strlen(pBuffer),(int)i+1);
						// move caret to the end of the selection
						GetTextExtentPoint32(hdc, (LPCTSTR)pBuffer, 
							iEndHilite, (LPSIZE)&CaretPos);
						xCaret = iEndHilite;
						bDone = TRUE;
						GetClientRect(hwnd, &rc);
						InvalidateRect(hwnd, &rc, TRUE);
						UpdateWindow(hwnd);
						bHiliteFlag = TRUE;
					}
				}
				else
				// moving right to left
				if ((iLastX - MouseX ) > (MouseX - HilitePos.cx))
				{
					iEndHilite = max(0,i);
					// move caret to the front of the selection
					GetTextExtentPoint32(hdc, (LPCTSTR)pBuffer, 
						iEndHilite, (LPSIZE)&CaretPos);
					xCaret = iEndHilite;
					bDone = TRUE;
					GetClientRect(hwnd, &rc);
					InvalidateRect(hwnd, &rc, TRUE);
					UpdateWindow(hwnd);
					bHiliteFlag = TRUE;
				}

			}
			i++;
			iLastX = HilitePos.cx;
		}
		// if we are hiliting and have moved out to white space we
		// need to make sure that we have include all the characters
		GetTextExtentPoint32(hdc, (LPCTSTR)pBuffer, 
			(int)strlen(pBuffer), (LPSIZE)&HilitePos);
		if (MouseX >= HilitePos.cx)
		{
			iEndHilite = strlen(pBuffer);
			GetClientRect(hwnd, &rc);
			InvalidateRect(hwnd, &rc, TRUE);
			UpdateWindow(hwnd);
			bHiliteFlag = TRUE;
		}

	}
	iLastMouseX = MouseX;
	ReleaseDC(hwnd, hdc);
}

void COwnerEdit::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	if (!bHiliteFlag)
	{
		iEndHilite = iStartHilite = 0;
	}
    SetCaretPos (CaretPos.cx, iCaretHeight);
    ShowCaret (hwnd);
}

void COwnerEdit::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int MouseX, int MouseY, UINT keyFlags)
{
    HDC     hdc = GetDC(hwnd);
    RECT	rc;
	BOOL	bDone = FALSE;
	int		i = 1;  // the first character in string
	int		iLastX = 0;
	char	cBuildStr[MAX_EDIT_LEN] = {"\0"};

	
	if (!(m_dwState & ODS_FOCUS))
		SetFocus(hwnd);

	HideCaret (hwnd);
	if (bHiliteFlag)
	{
		iStartHilite =iEndHilite = 0;
		GetClientRect(hwnd, &rc);
		InvalidateRect(hwnd, &rc, TRUE);
		UpdateWindow(hwnd);
		bHiliteFlag = FALSE;
	}

	// Find the caret position given the mouse postion
	// start with the first character and get the width
	while (i <= strlen(pBuffer) && !bDone)
	{
		strncpy(cBuildStr, &pBuffer[0], i);
 		GetTextExtentPoint32(hdc, (LPCTSTR)cBuildStr, (int)strlen(cBuildStr), (LPSIZE)&CaretPos);
		// if the width is greater then the mouse position place
		// caret in at this postion
		if (CaretPos.cx >= MouseX)
		{
			if ((int)(MouseX - iLastX) < (int)(CaretPos.cx - MouseX))
			{
				CaretPos.cx = iLastX;
				xCaret = i-1;
			}
			else
				xCaret = i;

			iStartHilite = xCaret;
			bDone = TRUE;
		}
		iLastX = CaretPos.cx;
		i++;
	}

	// end of string somewhere in white space
	if (i > strlen(pBuffer))
	{
		iStartHilite = strlen(pBuffer);
		CaretPos.cx = iLastX;
		xCaret = strlen(pBuffer);
	}

	SetCaretPos (CaretPos.cx, iCaretHeight);
	iLastMouseX = MouseX;
	ReleaseDC(hwnd, hdc);
}

void COwnerEdit::OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    HDC         hdc;
    RECT		rc;
    int         x, i;
	char		pCaretBuffer[MAX_EDIT_LEN];
	
	if (!fDown)
		return;

	hdc = GetDC(hwnd);
	
	switch (vk)
    {
         case VK_HOME :
				xCaret = 0;
				CaretPos.cx = 0;
			    SetCaretPos (CaretPos.cx, iCaretHeight);
				break;

         case VK_END :
				xCaret = strlen(pBuffer);
				GetTextExtentPoint32(hdc, (LPCTSTR)pBuffer, (int)strlen(pBuffer), (LPSIZE)&CaretPos);
		        SetCaretPos (CaretPos.cx, iCaretHeight);
				break;

         case VK_LEFT :
				xCaret = max (xCaret - 1, 0);
				strncpy(pCaretBuffer, pBuffer, xCaret);
				pCaretBuffer[xCaret] = 0;
				GetTextExtentPoint32(hdc, (LPCTSTR)pCaretBuffer, (int)strlen(pCaretBuffer), (LPSIZE)&CaretPos);
		        SetCaretPos (CaretPos.cx, iCaretHeight);
				break;

         case VK_RIGHT :
				xCaret = min (xCaret + 1, (int)strlen(pBuffer));
 				strncpy(pCaretBuffer, pBuffer, xCaret);
				pCaretBuffer[xCaret] = 0;
				GetTextExtentPoint32(hdc, (LPCTSTR)pCaretBuffer, (int)strlen(pCaretBuffer), (LPSIZE)&CaretPos);
		        SetCaretPos (CaretPos.cx, iCaretHeight);
				break;

         case VK_DELETE :
				if (iStartHilite != iEndHilite)
				{
					int iStartHiliteTmp, iEndHiliteTmp;

					SwapEnds(&iStartHiliteTmp, &iEndHiliteTmp);

					for (x = iStartHiliteTmp; x <= strlen(pBuffer); x++)
						pBuffer[x] = pBuffer[iEndHiliteTmp+x-iStartHiliteTmp];

					xCaret = iStartHiliteTmp;
					GetTextExtentPoint32(hdc, (LPCTSTR)pBuffer, xCaret, (LPSIZE)&CaretPos);
					iStartHilite = iEndHilite = 0;
					bHiliteFlag = FALSE;
					HideCaret (hwnd);
 					Edit_SetText(hwnd, pBuffer);
					GetClientRect(hwnd, &rc);
					InvalidateRect(hwnd, &rc, TRUE);
					UpdateWindow(hwnd);
					ShowCaret (hwnd);
					break;
				}
		 case VK_BACK :                    // backspace
                if (xCaret > 0)
                {
					int x1, x2;

					if (iStartHilite != iEndHilite)
					{
						int iStartHiliteTmp, iEndHiliteTmp;

						SwapEnds(&iStartHiliteTmp, &iEndHiliteTmp);
						x1 = iStartHiliteTmp;
						x2 = iEndHiliteTmp-x1;
						xCaret = iStartHiliteTmp+1; // later one is removed
						iStartHilite = iEndHilite = 0;
						bHiliteFlag = FALSE;
					}
					else
					{
						x1 = xCaret-1;
						x2 = 1;	
					}

					GetTextExtentPoint32(hdc, (LPCTSTR)pBuffer, (int)strlen(pBuffer), (LPSIZE)&CaretPos);

					for (i=x1; i<strlen(pBuffer); i++)
						pBuffer[i] = pBuffer[x2+i];

					if (xCaret >= strlen(pBuffer))
					{
						pBuffer[xCaret] = 0;
					}

 					HideCaret (hwnd);
					Edit_SetText(hwnd, pBuffer);

					GetClientRect(hwnd, &rc);
					rc.bottom = rc.top+CaretPos.cy;
					rc.right = rc.left+CaretPos.cx;
					// clear the text
					OnEraseBkgndSegment(hwnd, hdc, &rc);

					GetClientRect(hwnd, &rc);
					InvalidateRect(hwnd, &rc, TRUE);
					UpdateWindow(hwnd);

					xCaret--;
					GetTextExtentPoint32(hdc, (LPCTSTR)pBuffer, xCaret, (LPSIZE)&CaretPos);
					SetCaretPos (CaretPos.cx, iCaretHeight);
					ShowCaret (hwnd);
				}
                break;
	 }
 	 SetCaretPos (CaretPos.cx, iCaretHeight);
	 ReleaseDC(hwnd, hdc);

}

void COwnerEdit::OnChar(HWND hwnd, TCHAR ch, int cRepeat)
{
    RECT		rc;
    int         i, j;
	
	for (j = 0; j < cRepeat; j++)
    {
		if (isalnum(ch))
		{
			HDC hdc = GetDC(hwnd);

			HideCaret(hwnd);
			if (xCaret < strlen(pBuffer))
			{
				int iOrigLen;
				int count;
			
				//Remove any hilited characters first
				if (iStartHilite != iEndHilite)
				{
					int iStartHiliteTmp, iEndHiliteTmp;
					int x;
			
					SwapEnds(&iStartHiliteTmp, &iEndHiliteTmp);
			
					for (x = iStartHiliteTmp; x <= strlen(pBuffer); x++)
						pBuffer[x] = pBuffer[iEndHiliteTmp+x-iStartHiliteTmp];
					
					xCaret = iStartHiliteTmp;
					iStartHilite = iEndHilite = 0;
					bHiliteFlag = FALSE;
			
				}
			
				iOrigLen = strlen(pBuffer);
				count = strlen(pBuffer)+1;
			
				// insert in middle of the string
				for (i=iOrigLen; i>=xCaret; i--)
				{
					pBuffer[count--] = pBuffer[i];
				}
			
				pBuffer[xCaret++] = (char) ch;
				pBuffer[++iOrigLen] = (char) '\0';
			
				// place carent after the new character entered
				GetTextExtentPoint32(hdc, (LPCTSTR)pBuffer, strlen(pBuffer), (LPSIZE)&CaretPos);
				GetClientRect(hwnd, &rc);
				rc.right = rc.left+CaretPos.cx;
				// clear the text
				OnEraseBkgndSegment(hwnd, hdc, &rc);
			}
			else
			{
				// add to the end
				pBuffer[xCaret++] = (char) ch;
				pBuffer[xCaret] = (char) '\0';
			}
			Edit_SetText(hwnd, pBuffer);
			GetClientRect(hwnd, &rc);
			InvalidateRect(hwnd, &rc, TRUE);
			UpdateWindow(hwnd);
			
			GetTextExtentPoint32(hdc, (LPCTSTR)pBuffer, xCaret, (LPSIZE)&CaretPos);
			SetCaretPos (CaretPos.cx, iCaretHeight);
			ShowCaret (hwnd);

			// turn on a OK button when type is entered
			if (strlen(pBuffer) > 0)
			    EnableWindow(GetDlgItem(GetParent(hwnd), IDOK), TRUE);

			ReleaseDC(hwnd, hdc);
		}
    }

}

void COwnerEdit::SwapEnds(int *iStartHiliteTmp, int *iEndHiliteTmp)
{
		if (iStartHilite > iEndHilite)
		{
			*iEndHiliteTmp = iStartHilite;
			*iStartHiliteTmp = iEndHilite;
		}
		else
		{
			*iEndHiliteTmp = iEndHilite;
			*iStartHiliteTmp = iStartHilite;
		}
}			

void COwnerEdit::OnPaint(HWND hwnd)
{
    PAINTSTRUCT     ps;
    HDC             hdc;
    RECT            rcWindow;
    LONG            lStyle;
	SIZE			Pos;
    int             iFlags;
    int             iFirstVisible;
    int             iLength;
    int             iMode;
    char            sz[MAX_EDIT_LEN], szPart[MAX_EDIT_LEN];
	int				iEndHiliteTmp, iStartHiliteTmp;

	HideCaret(hwnd);
    iFirstVisible = Edit_GetFirstVisibleLine(m_hwnd);
    iLength = Edit_LineLength(m_hwnd, iFirstVisible);
    if (iLength + 1 > sizeof(sz))
    {
        COwnerSubclassBase::OnPaint(hwnd);
        return;
    }

    iLength = Edit_GetLine(m_hwnd, iFirstVisible, sz, sizeof(sz));

    // since the previous call doesn't set the null character then
    // we do it.
    sz[iLength] = '\0';

    hdc = BeginPaint(m_hwnd, &ps);
	hdc = GetDC(hwnd);

    lStyle = GetWindowLong(m_hwnd, GWL_STYLE);

    GetClientRect(m_hwnd, &rcWindow);

    iFlags = DT_EDITCONTROL;

    // Pickup horizontal settings
    if (lStyle & ES_CENTER)
    {
        iFlags |= DT_CENTER;
    }
    else if (lStyle & ES_RIGHT)
    {
        iFlags |= DT_RIGHT;
    }
    else
    {
        iFlags |= DT_LEFT;
    }

    if (!(lStyle & ES_MULTILINE))
    {
        iFlags |= DT_SINGLELINE;
    }

    iMode = SetBkMode(hdc, TRANSPARENT);

	// Check we can change this so we only need to delete when required &%@!%@#%!@&
	GetClientRect(hwnd, &rcWindow);
	OnEraseBkgndSegment(hwnd, hdc, &rcWindow);

	if (iStartHilite == iEndHilite)
	{
		ProcessText(hdc, COLOR_NORMAL_FG, COLOR_NORMAL_BK, &rcWindow, sz);
	}
	else
	{
									GetClientRect(hwnd, &rcWindow);
									OnEraseBkgndSegment(hwnd, hdc, &rcWindow);

		SwapEnds(&iStartHiliteTmp, &iEndHiliteTmp);
		
		if (iStartHiliteTmp == 0)
		{
			// HiliteTmp first section
			strncpy(szPart, sz, iEndHiliteTmp);
			szPart[iEndHiliteTmp] = 0;
			ProcessText(hdc, COLOR_SELECTED_FG, COLOR_SELECTED_BK, &rcWindow, szPart);
			// Adjust the rectangle
			rcWindow.left = rcWindow.right;
 			GetTextExtentPoint32(hdc, (LPCTSTR)szPart, 
						(int)strlen(szPart), (LPSIZE)&Pos);
			rcWindow.right = Pos.cx;

			// Normal last section
			if (iEndHiliteTmp < strlen(sz))
			{
				rcWindow.left = rcWindow.right;
 				GetTextExtentPoint32(hdc, (LPCTSTR)sz, 
							(int)strlen(sz), (LPSIZE)&Pos);
				rcWindow.right = Pos.cx;
				strncpy(szPart, &sz[iEndHiliteTmp], strlen(sz) - iEndHiliteTmp);
				szPart[strlen(sz) - iEndHiliteTmp] = 0;
				ProcessText(hdc, COLOR_NORMAL_FG, COLOR_NORMAL_BK, &rcWindow, szPart);
			}
		}
		else
		{
			// Normal first section
			// get from start to this point
			strncpy(szPart, sz, iStartHiliteTmp);
			szPart[iStartHiliteTmp] = 0;
 			GetTextExtentPoint32(hdc, (LPCTSTR)szPart, 
						(int)strlen(szPart), (LPSIZE)&Pos);
			rcWindow.right = Pos.cx;
			ProcessText(hdc, COLOR_NORMAL_FG, COLOR_NORMAL_BK, &rcWindow, szPart);

			// HiliteTmp second section
			if (iStartHiliteTmp < strlen(sz))
			{
				// Adjust the rectangle
				// get from start to this point
				strncpy(szPart, sz, iEndHiliteTmp);
				rcWindow.left = rcWindow.right;
 				GetTextExtentPoint32(hdc, (LPCTSTR)szPart, 
							(int)strlen(szPart), (LPSIZE)&Pos);
				rcWindow.right = Pos.cx;
				strncpy(szPart, &sz[iStartHiliteTmp], iEndHiliteTmp - iStartHiliteTmp + 1);
				szPart[iEndHiliteTmp - iStartHiliteTmp] = 0;

				ProcessText(hdc, COLOR_SELECTED_FG, COLOR_SELECTED_BK, &rcWindow, szPart);
			}
			
			// Normal Last section
			if (iEndHiliteTmp < strlen(sz))
			{
				// Adjust the rectangle
				// get from start to this point
				GetClientRect(hwnd, &rcWindow);
				strncpy(szPart, sz, iEndHiliteTmp);
 				GetTextExtentPoint32(hdc, (LPCTSTR)szPart, 
							iEndHiliteTmp, (LPSIZE)&Pos);
				rcWindow.left = Pos.cx;
				strncpy(szPart, &sz[iEndHiliteTmp], strlen(sz) - iEndHiliteTmp);
				szPart[strlen(sz) - iEndHiliteTmp] = 0;
				ProcessText(hdc, COLOR_NORMAL_FG, COLOR_NORMAL_BK, &rcWindow, szPart);
			}

		}
	}

    SetBkMode(hdc, iMode);
	ReleaseDC(hwnd, hdc);

    EndPaint(m_hwnd, &ps);
	ShowCaret(hwnd);
}

void COwnerEdit::OnSetFocus(HWND hwnd, HWND hwndOldFocus)
{
    RECT		rc;
    TEXTMETRIC  tm;
	HDC         hdc = GetDC(hwnd);

    // create and show the caret
	m_dwState |= ODS_FOCUS;
    GetTextMetrics (hdc, &tm);
	GetClientRect(hwnd, &rc);
	iCaretHeight = rc.top;
    CreateCaret (hwnd, NULL, cxChar, tm.tmHeight);
   	GetTextExtentPoint32(hdc, (LPCTSTR)pBuffer, (int)strlen(pBuffer), (LPSIZE)&CaretPos);
    SetCaretPos (CaretPos.cx, iCaretHeight);
    ShowCaret (hwnd);
	ReleaseDC(hwnd, hdc);
}

void COwnerEdit::OnKillFocus(HWND hwnd, HWND hwndNewFocus)
{
    RECT		rc;
	HDC         hdc = GetDC(hwnd);

    // hide and destroy the caret
	m_dwState &= ~ODS_FOCUS;
    HideCaret (hwnd);
	if (bHiliteFlag)
	{
		iStartHilite =iEndHilite = 0;
		GetClientRect(hwnd, &rc);
		InvalidateRect(hwnd, &rc, TRUE);
		UpdateWindow(hwnd);
		bHiliteFlag = FALSE;
	}

    DestroyCaret ();
 	ReleaseDC(hwnd, hdc);
}

LRESULT COwnerEdit::BaseHandler(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return COwnerSubclassBase::BaseHandler(hwnd, Message, wParam, lParam);
}

void COwnerEdit::ProcessText(HDC hdc, COLORREF crFg, COLORREF crBk, RECT *rcWindow, char *sz)
{
	COLORREF crText;

    crText = SetTextColor(hdc, crBk);
    ExtTextOut(hdc,
               rcWindow->left + 1,
               rcWindow->top + 1, 
               ETO_CLIPPED,
               rcWindow, 
               sz,
               strlen(sz), 
               NULL);

    SetTextColor(hdc, crFg);
    ExtTextOut(hdc,
               rcWindow->left,
               rcWindow->top, 
               ETO_CLIPPED,
               rcWindow, 
               sz,
               strlen(sz), 
               NULL);

	SetTextColor(hdc, crText);
}

COwnerScrollBar::COwnerScrollBar()
{
}


COwnerScrollBar::~COwnerScrollBar()
{
}

void COwnerScrollBar::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{

}

void COwnerScrollBar::OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	RECT    rc;

	if (code >= 0 &&  code < SB_ENDSCROLL)
	{
		HDC	hdc = GetDC(hwnd);

		SetWindowRedraw(hwnd, FALSE);
		// pos is hi word and code is low word
		CallWindowProc(m_wndproc, hwnd, WM_HSCROLL, MAKELONG(code, pos), (long)hwndCtl);
		GetClientRect(hwnd, &rc);
		InvalidateRect(hwnd, &rc, TRUE);
		SetWindowRedraw(hwnd, TRUE);
		OnEraseBkgndSegment(hwnd, hdc, NULL);
		UpdateWindow(hwnd);
		ReleaseDC(hwnd, hdc);
	}
}
	
void COwnerScrollBar::OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{	
	RECT    rc;
	if (code >= 0 && code < SB_ENDSCROLL)
	{
		HDC	hdc = GetDC(hwnd);

		SetWindowRedraw(hwnd, FALSE);
		LockWindowUpdate(hwnd);
		// pos is hi word and code is low word
		CallWindowProc(m_wndproc, hwnd, WM_VSCROLL, MAKELONG(code, pos), (long)hwndCtl);
		LockWindowUpdate(NULL);
		SetWindowRedraw(hwnd, TRUE);
		GetClientRect(hwnd, &rc);
		InvalidateRect(hwnd, &rc, TRUE);
		OnEraseBkgndSegment(hwnd, hdc, NULL);
		UpdateWindow(hwnd);
		ReleaseDC(hwnd, hdc);
	}
}	

void COwnerScrollBar::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int MouseX, int MouseY, UINT keyFlags)
{
    SetCapture(hwnd);
    SetFocus(hwnd);
//    DrawCheckPart(NULL, m_dwState & BST_CHECKED, TRUE);
	CallWindowProc(m_wndproc, hwnd, WM_LBUTTONDOWN, keyFlags, MAKELONG(MouseX, MouseY));
}

LRESULT COwnerScrollBar::BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
/*	RECT rc;

	switch (message)
	{
		case WS_HSCROLL:
			if (LOWORD(wParam) >= 0 &&  LOWORD(wParam) < SB_ENDSCROLL)
			{
				HDC	hdc = GetDC(hwnd);

				SetWindowRedraw(hwnd, FALSE);
				// pos is hi word and code is low word
				CallWindowProc(m_wndproc, hwnd, message, wParam, lParam);
				GetClientRect(hwnd, &rc);
				InvalidateRect(hwnd, &rc, TRUE);
				SetWindowRedraw(hwnd, TRUE);
				OnEraseBkgndSegment(hwnd, hdc, NULL);
				UpdateWindow(hwnd);
				ReleaseDC(hwnd, hdc);
				return 0;
			}
			break;

		case WS_VSCROLL:
			if (LOWORD(wParam) >= 0 &&  LOWORD(wParam) < SB_ENDSCROLL)
			{
				HDC	hdc = GetDC(hwnd);

				SetWindowRedraw(hwnd, FALSE);
				LockWindowUpdate(hwnd);
				// pos is hi word and code is low word
				CallWindowProc(m_wndproc, hwnd, message, wParam, lParam);
				LockWindowUpdate(NULL);
				SetWindowRedraw(hwnd, TRUE);
				GetClientRect(hwnd, &rc);
				InvalidateRect(hwnd, &rc, TRUE);
				OnEraseBkgndSegment(hwnd, hdc, NULL);
				UpdateWindow(hwnd);
				ReleaseDC(hwnd, hdc);
				return 0;
			}
			break;
	}
*/
    return COwnerSubclassBase::BaseHandler(hwnd, message, wParam, lParam);
}


