//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       gdidlgbase.cpp
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    14-Jan-98   SHernd   Created
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "supportfn.hpp"
#include "gdidlgbase.h"
#include "uiwnd.h"
#include "winctrls.h"
#include "main.h"

extern HINSTANCE    g_hInst;
extern HWND         g_hwnd;
extern CMainWnd *   g_pMainWnd;


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


DWORD GDIDialogHandler(CUIWnd * pbase, 
                       CGDIDlgBase * pgdidlg, 
                       LPCTSTR pszTemplate,
                       HWND hwndParent)
{
    DWORD       dwRet;
    CUIWnd *    puiwnd = NULL;

    if (pbase)
    {
        puiwnd = pbase->m_pUIMgr->GetActiveUIWnd();
    }

#if 0
    if (puiwnd != NULL)
    {
        puiwnd->Draw();
        puiwnd->Draw();
    }

    if (hwndParent == g_hwnd)
    {
        prasMainScreen->FlipToGDISurface();
    }
#endif

    SetCursor(LoadCursor(NULL, IDC_ARROW));
    ShowCursor(TRUE);

    dwRet = pgdidlg->MultiDialogBox(g_hInst, pszTemplate, hwndParent);

    ShowCursor(FALSE);

    return dwRet;
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void SetStaticOwnerDraw(HWND hwnd, int idCtl)
{
    HWND    hwndCtl;
    LONG    l;

    hwndCtl = GetDlgItem(hwnd, idCtl);
    l = GetWindowLong(hwndCtl, GWL_STYLE);
    l |= SS_OWNERDRAW;
    SetWindowLong(hwndCtl, GWL_STYLE, l);
}

void DrawButton(const DRAWITEMSTRUCT * lpDrawItem)
{
    COLORREF        crText;
    int             iMode;
    char            sz[_MAX_PATH];
    COLORREF        crFg;
    COLORREF        crBk;
    RECT            rc;
    int             iFlags;
    COwnerButton *  pbutton;

    // Check if this is a subclassed Button
    pbutton = (COwnerButton*)GetProp(lpDrawItem->hwndItem, "CMultiBase Class");
    if (pbutton)
    {
        pbutton->Draw(lpDrawItem);
        return;
    }

    EraseParentBkgnd(lpDrawItem->hwndItem, lpDrawItem->hDC);

    rc = lpDrawItem->rcItem;

    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        crFg = COLOR_SELECTED_FG;
        crBk = COLOR_SELECTED_BK;
        iFlags = EDGE_SUNKEN;
    }
    else
    {
        iFlags = EDGE_RAISED;
        if (lpDrawItem->itemState & ODS_DISABLED)
        {
            crFg = COLOR_DISABLED_FG;
            crBk = COLOR_DISABLED_BK;
        }
        else
        {
            crFg = COLOR_NORMAL_FG;
            crBk = COLOR_NORMAL_BK;
        }
    }

    MyDrawEdge(lpDrawItem->hDC,
               &rc,
               iFlags,
               BF_RECT | BF_ADJUST | BF_SOFT);

    if (lpDrawItem->itemState & ODS_FOCUS)
    {
        DrawFocusRect(lpDrawItem->hDC, &rc);
    }
             
    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        OffsetRect(&rc, 1, 1);
    }

    iMode = SetBkMode(lpDrawItem->hDC, TRANSPARENT);
    GetWindowText(lpDrawItem->hwndItem, sz, sizeof(sz));

    crText = SetTextColor(lpDrawItem->hDC, crBk);

    OffsetRect(&rc, 1, 1);
    DrawTextEx(lpDrawItem->hDC, sz, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE, NULL);

    OffsetRect(&rc, -1, -1);
    SetTextColor(lpDrawItem->hDC, crFg);
    DrawTextEx(lpDrawItem->hDC, sz, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE, NULL);

    SetTextColor(lpDrawItem->hDC, crText);
    SetBkMode(lpDrawItem->hDC, iMode);
}

VOID HandleSelectionState(const DRAWITEMSTRUCT *lpdis, int inflate)
{
	RECT    rc;
    HBRUSH  hbr;

    /* Resize rectangle to place selection frame outside of the focus
     * frame and the item.
     */
    CopyRect ((LPRECT)&rc, (LPRECT)&lpdis->rcItem);
    InflateRect ((LPRECT)&rc, inflate, inflate);

    if (lpdis->itemState & ODS_SELECTED)
    {
            /* selecting item -- paint a black frame */
            hbr = (HBRUSH)GetStockObject(BLACK_BRUSH);
    }
    else
    {
            /* de-selecting item -- remove frame */
            hbr = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    }
    FrameRect(lpdis->hDC, (LPRECT)&rc, hbr);
    DeleteObject (hbr);
}

VOID HandleFocusState(const DRAWITEMSTRUCT *lpdis, int inflate)
{
	RECT    rc;
    HBRUSH  hbr;

    /* Resize rectangle to place focus frame between the selection
     * frame and the item.
     */
    CopyRect ((LPRECT)&rc, (LPRECT)&lpdis->rcItem);
    InflateRect ((LPRECT)&rc, inflate, inflate);

    if (lpdis->itemState & ODS_FOCUS)
    {
            /* gaining input focus -- paint a gray frame */
            hbr = (HBRUSH)GetStockObject(GRAY_BRUSH);
    }
    else
    {
            /* losing input focus -- remove (paint over) frame */
            hbr = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    }
    FrameRect(lpdis->hDC, (LPRECT)&rc, hbr);
    DeleteObject (hbr);
}

void DrawListbox(const DRAWITEMSTRUCT * lpDrawItem)
{
    COLORREF        crText;
    int             iMode;
    char            sz[_MAX_PATH];
    COLORREF        crFg;
    COLORREF        crBk;
    RECT            rc;

    if ((lpDrawItem->itemState == (ODS_SELECTED | ODS_FOCUS)) || lpDrawItem->itemID == -1)
    {
		return;
    }
/*
	if (lpDrawItem->itemAction == ODA_SELECT)
    {
		HandleSelectionState(lpDrawItem, SELETED_OUTLINE);
		return TRUE;
    }

	if (lpDrawItem->itemAction == ODA_FOCUS)
    {
		HandleFocusState(lpDrawItem, SELETED_OUTLINE);
		return TRUE;
    }
*/
    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        crFg = COLOR_SELECTED_FG;
        crBk = COLOR_SELECTED_BK;
    }
    else
    {
        crFg = COLOR_NORMAL_FG;
        crBk = COLOR_NORMAL_BK;
    }

    iMode = SetBkMode(lpDrawItem->hDC, TRANSPARENT);

    ListBox_GetText(lpDrawItem->hwndItem, lpDrawItem->itemID, sz);
	
    rc = lpDrawItem->rcItem;

    rc.left += 5;

    crText = SetTextColor(lpDrawItem->hDC, crBk);
    ExtTextOut(lpDrawItem->hDC,
               rc.left + 1,
               rc.top + 1, 
               ETO_CLIPPED,
               &rc, 
               sz,
               strlen(sz), 
               NULL);

    SetTextColor(lpDrawItem->hDC, crFg);
    ExtTextOut(lpDrawItem->hDC,
               rc.left,
               rc.top, 
               ETO_CLIPPED,
               &rc, 
               sz,
               strlen(sz), 
               NULL);

    SetTextColor(lpDrawItem->hDC, crText);
    SetBkMode(lpDrawItem->hDC, iMode);

	return;

}

void DrawComboBox(const DRAWITEMSTRUCT * lpDrawItem)
{
    COLORREF        crText;
    int             iMode;
    char            sz[_MAX_PATH];
    COLORREF        crFg;
    COLORREF        crBk;
    RECT            rc;
	int				nIndex;

    if (lpDrawItem->itemID == -1)
    {
        return;
    }

    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        crFg = COLOR_SELECTED_FG;
        crBk = COLOR_SELECTED_BK;
    }
    else
    {
        crFg = COLOR_NORMAL_FG;
        crBk = COLOR_NORMAL_BK;
    }

    iMode = SetBkMode(lpDrawItem->hDC, TRANSPARENT);

	nIndex = SendMessage(lpDrawItem->hwndItem, LB_ITEMFROMPOINT, 0, 
				(LPARAM) MAKELPARAM(lpDrawItem->rcItem.top, lpDrawItem->rcItem.left));  
    ComboBox_GetLBText(lpDrawItem->hwndItem, lpDrawItem->itemID, sz);

    rc = lpDrawItem->rcItem;
    rc.left += 5;

    crText = SetTextColor(lpDrawItem->hDC, crBk);
    ExtTextOut(lpDrawItem->hDC,
               rc.left + 1,
               rc.top + 1, 
               ETO_CLIPPED,
               &rc, 
               sz,
               strlen(sz), 
               NULL);

    SetTextColor(lpDrawItem->hDC, crFg);
    ExtTextOut(lpDrawItem->hDC,
               rc.left,
               rc.top, 
               ETO_CLIPPED,
               &rc, 
               sz,
               strlen(sz), 
               NULL);

    SetTextColor(lpDrawItem->hDC, crText);
    SetBkMode(lpDrawItem->hDC, iMode);
}

void DrawStatic(const DRAWITEMSTRUCT * lpDrawItem)
{
    COLORREF        crText;
    int             iMode;
    char            sz[_MAX_PATH];
    COLORREF        crFg;
    COLORREF        crBk;
    RECT            rc;
    LONG            l;
    int             iFlags;

    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        crFg = COLOR_SELECTED_FG;
        crBk = COLOR_SELECTED_BK;
    }
    else
    {
        crFg = COLOR_NORMAL_FG;
        crBk = COLOR_NORMAL_BK;
    }

    l = GetWindowLong(lpDrawItem->hwndItem, GWL_STYLE);
    switch (l & 0x1F)
    {
        default:
        case SS_LEFT:
            iFlags = DT_LEFT;
            break;

        case SS_CENTER:
            iFlags = DT_CENTER;
            break;

        case SS_RIGHT:
            iFlags = DT_RIGHT;
            break;

        case SS_LEFTNOWORDWRAP:
            iFlags = DT_LEFT | DT_SINGLELINE;
            break;
    }

    iFlags |= DT_VCENTER;

    iMode = SetBkMode(lpDrawItem->hDC, TRANSPARENT);

    GetWindowText(lpDrawItem->hwndItem, sz, sizeof(sz));

    rc = lpDrawItem->rcItem;
    crText = SetTextColor(lpDrawItem->hDC, crBk);

    OffsetRect(&rc, 1, 1);
    DrawTextEx(lpDrawItem->hDC, sz, -1, &rc, iFlags, NULL);

    OffsetRect(&rc, -1, -1);
    SetTextColor(lpDrawItem->hDC, crFg);
    DrawTextEx(lpDrawItem->hDC, sz, -1, &rc, iFlags, NULL);

    SetTextColor(lpDrawItem->hDC, crText);
    SetBkMode(lpDrawItem->hDC, iMode);
}

void DrawTab(const DRAWITEMSTRUCT * lpDrawItem)
{
    COLORREF        crText;
    int             iMode;
    char            sz[_MAX_PATH];
    COLORREF        crFg;
    COLORREF        crBk;
    RECT            rc;
    int             iFlags;
    TC_ITEM         tci;

    tci.mask = TCIF_TEXT;
    tci.pszText = sz;
    tci.cchTextMax = sizeof(sz);

    TabCtrl_GetItem(lpDrawItem->hwndItem, lpDrawItem->itemID, &tci);

    rc = lpDrawItem->rcItem;
    rc.top += 2;

    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        crFg = COLOR_SELECTED_FG;
        crBk = COLOR_SELECTED_BK;
    }
    else
    {
        crFg = COLOR_NORMAL_FG;
        crBk = COLOR_NORMAL_BK;
    }

    iFlags = DT_CENTER | DT_VCENTER;

    iMode = SetBkMode(lpDrawItem->hDC, TRANSPARENT);

    crText = SetTextColor(lpDrawItem->hDC, crBk);

    OffsetRect(&rc, 1, 1);
    DrawTextEx(lpDrawItem->hDC, sz, -1, &rc, iFlags, NULL);

    OffsetRect(&rc, -1, -1);
    SetTextColor(lpDrawItem->hDC, crFg);
    DrawTextEx(lpDrawItem->hDC, sz, -1, &rc, iFlags, NULL);

    SetTextColor(lpDrawItem->hDC, crText);
    SetBkMode(lpDrawItem->hDC, iMode);
}


BOOL CALLBACK GDIDlgEnumProc(HWND hwnd, LPARAM lParam)
{
    char        sz[50];
    LONG        l;
    WNDPROC     wndproc;
    BYTE        b;

    wndproc = SubclassWindow(hwnd, CtrlProcHack);

    SetProp(hwnd, "CtrlProcHack Base", (HANDLE)wndproc);

    GetClassName(hwnd, sz, sizeof(sz));
    l = GetWindowLong(hwnd, GWL_STYLE);

    b = l & 0xF;
    if (strcmpi(sz, "button") == 0 && 
        ((b == BS_AUTOCHECKBOX) ||
         (b == BS_CHECKBOX)))
    {
        COwnerCheckbox *    pcheck;
        pcheck = new COwnerCheckbox();
        pcheck->Subclass(hwnd);
    }
    else if (strcmpi(sz, "static") == 0 && 
             ((b == SS_LEFT) ||
              (b == SS_LEFTNOWORDWRAP) ||
              (b == SS_RIGHT) ||
              (b == SS_CENTER) ||
              (b == SS_SIMPLE)))
    {
        l |= SS_OWNERDRAW;
        SetWindowLong(hwnd, GWL_STYLE, l);
    }
    else if (strcmpi(sz, WC_TABCONTROL) == 0)
    {
        COwnerTab * ptab;

        ptab = new COwnerTab();
        ptab->Subclass(hwnd);
    }
    else if (strcmpi(sz, TRACKBAR_CLASS) == 0)
    {
         COwnerSlider * pslider;

        pslider = new COwnerSlider();
        pslider->Subclass(hwnd);
    }
    else if (strcmpi(sz, "ListBox") == 0)
    {
        COwnerListBox *    plistbox;
        
        plistbox = new COwnerListBox(hwnd);
        plistbox->Subclass(hwnd);
    }
    else if (strcmpi(sz, "ScrollBar") == 0)
    {
        COwnerScrollBar *    pscrollbar;
        
        pscrollbar = new COwnerScrollBar();
        pscrollbar->Subclass(hwnd);
    }
    else if (strcmpi(sz, "ComboBox") == 0)
    {
        COwnerComboBox *    pcombobox;
        
        pcombobox = new COwnerComboBox();
        pcombobox->Subclass(hwnd);
    }
    else if (strcmpi(sz, "edit") == 0)
    {
        COwnerEdit *    pedit;
        
        pedit = new COwnerEdit();
        pedit->Subclass(hwnd);
    }
#if 0
#endif

    return TRUE;
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


LRESULT CALLBACK CtrlProcHack(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC wndproc = (WNDPROC)GetProp(hwnd, "CtrlProcHack Base");

    switch (uiMsg)
    {
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP: 
			SendMessage(g_hwnd, uiMsg, wParam, lParam);
			break;
    }

    if (wndproc)
    {
        return CallWindowProc(wndproc, hwnd, uiMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uiMsg, wParam, lParam);
}



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

CGDIDlgBase::CGDIDlgBase()
{
    m_pRaster = NULL;
    m_bBecomingActive = false;
}

CGDIDlgBase::~CGDIDlgBase()
{
    delete m_pRaster;
}

void CGDIDlgBase::OnDestroy(HWND hwnd)
{
    WNDPROC wndproc = (WNDPROC)GetProp(hwnd, "CtrlProcHack Base");

    SubclassWindow(hwnd, wndproc);

    CMultiDlg::OnDestroy(hwnd);
}


BOOL CGDIDlgBase::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    EnumChildWindows(hwnd, GDIDlgEnumProc, NULL);

    return CMultiDlg::OnInitDialog(hwnd, hwndFocus, lParam);
}

void CGDIDlgBase::OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if (m_bBecomingActive)
    {
        return;
    }

    switch (lpDrawItem->CtlType)
    {
        case ODT_LISTBOX:
            DrawListbox(lpDrawItem);
            break;

        case ODT_BUTTON:
            DrawButton(lpDrawItem);
            break;

        case ODT_STATIC:
            DrawStatic(lpDrawItem);
            break;

        case ODT_TAB:
            DrawTab(lpDrawItem);
            break;

        case ODT_COMBOBOX:
            DrawComboBox(lpDrawItem);
            break;
	}
}

HBRUSH CGDIDlgBase::OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
    char     sz[50];
    LONG     l;

    GetClassName(hwndChild, sz, sizeof(sz));
    l = GetWindowLong(hwndChild, GWL_STYLE);

	if ((strcmpi(sz, "button") == 0 && (l & BS_OWNERDRAW)) ||
        (strcmpi(sz, "edit") == 0) ||
        (strcmpi(sz, WC_TABCONTROL) == 0 && (l & TCS_OWNERDRAWFIXED)))
        //(strcmpi(sz, TRACKBAR_CLASS) == 0))
    {
        return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    }
    else
	if (strcmpi(sz, "static") == 0 && (l & SS_OWNERDRAW))
	{
		l &= 0x0000000FL;  // strip the LSNibble
		// Is a border being drawn
		if (l == SS_BLACKFRAME || l == SS_GRAYFRAME || l == SS_WHITEFRAME)
		{
			// Draw our border
			HDC     hdc;
			RECT    rc;

			GetWindowRect(hwndChild, &rc);
			MapWindowRect(NULL, hwndChild, &rc);

			OffsetRect(&rc, -rc.left, -rc.top);

			hdc = GetWindowDC(hwndChild);

			MyDrawDlgFrame(hdc, &rc);

			ReleaseDC(hwndChild, hdc);
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
		}
		else
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	}
	else
	if (strcmpi(sz, "ScrollBar") == 0)
	{
        HDC     hdc;
        RECT    rc;

        GetWindowRect(hwndChild, &rc);
        MapWindowRect(NULL, hwndChild, &rc);

        OffsetRect(&rc, -rc.left, -rc.top);

        hdc = GetWindowDC(hwndChild);

        ReleaseDC(hwndChild, hdc);
        return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	}
	else
	if (strcmpi(sz, "ComboBox") == 0 && (l & CBS_OWNERDRAWFIXED))
    {
        HDC     hdc;
        RECT    rc;

        GetWindowRect(hwndChild, &rc);
        MapWindowRect(NULL, hwndChild, &rc);

        OffsetRect(&rc, -rc.left, -rc.top);

        hdc = GetWindowDC(hwndChild);

//		OnEraseBkgnd(hwndChild, hdc);

		MyDrawDlgFrame(hdc, &rc);

        ReleaseDC(hwndChild, hdc);
        return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    }
	else
	if (strcmpi(sz, "listbox") == 0 && (l & LBS_OWNERDRAWFIXED))
    {
        HDC     hdc;
        RECT    rc;
	    SCROLLINFO scrollH;

        GetWindowRect(hwndChild, &rc);
        MapWindowRect(NULL, hwndChild, &rc);

        OffsetRect(&rc, -rc.left, -rc.top);

        hdc = GetWindowDC(hwndChild);

//		OnEraseBkgnd(hwndChild, hdc);

		MyDrawDlgFrame(hdc, &rc);
		
		scrollH.cbSize = sizeof(scrollH);
		scrollH.fMask = SIF_ALL;

		BOOL stat = GetScrollInfo(hwndChild, SB_VERT, &scrollH);

        ReleaseDC(hwndChild, hdc);
        return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    }

    return CMultiDlg::OnCtlColor(hwnd, hdc, hwndChild, type);
}

BOOL CGDIDlgBase::OnEraseBkgnd(HWND hwnd, HDC hdcDst)
{
    HDC     hdcSrc;
    RECT    rc;

    if (!m_pRaster)
    {
        EraseParentBkgnd(hwnd, hdcDst);

        return 1;
#if 0
        return CMultiDlg::OnEraseBkgnd(hwnd, hdcDst);
#endif
    }

#if 0
    {
        // Analyze the Clipping Regions
        RGNDATA *       prgndata;
        LPBYTE          pb;
        DWORD           dwSize;
        RECT *          prc;
        int             i;
        HRGN            hrgn;

        SetRect(&rc, 0, 0, 0, 0);
        hrgn = CreateRectRgnIndirect(&rc);
        GetClipRgn(hdcDst, hrgn);

        dwSize = GetRegionData(hrgn, 0, NULL);
        pb = new BYTE[dwSize];
        prgndata = (RGNDATA*)pb;
        prc = (RECT*)prgndata->Buffer;
        GetRegionData(hrgn, dwSize, prgndata);

        for (i = 0; i < prgndata->rdh.nCount; i++)
        {
            rc = prc[i];
        }

        delete [] pb;
    }
#endif

    GetClientRect(hwnd, &rc);

    hdcSrc = m_pRaster->hdcGet();

    BitBlt(hdcDst, 
           0, 
           0,
           rc.right - rc.left,
           rc.bottom - rc.top,
           hdcSrc,
           0,
           0,
           SRCCOPY);

    m_pRaster->ReleaseDC(hdcSrc);

    SetDlgMsgResult(hwnd, WM_ERASEBKGND, TRUE);
    return TRUE;
}



void CGDIDlgBase::OnActivateApp(HWND hwnd, BOOL fActivate, DWORD dwThreadId)
{
    CMultiDlg::OnActivateApp(hwnd, fActivate, dwThreadId);

    m_bBecomingActive = fActivate;
    InvalidateRect(hwnd, NULL, TRUE);
    g_pMainWnd->OnActivateApp(g_hwnd, fActivate, dwThreadId);
}


void CGDIDlgBase::OnNCPaint(HWND hwnd, HRGN hrgn)
{
    LONG        l;

    l = GetWindowLong(hwnd, GWL_STYLE);
    if (l & DS_MODALFRAME)
    {
        HDC     hdc;
        RECT    rc;

        GetWindowRect(hwnd, &rc);
        MapWindowRect(NULL, hwnd, &rc);

        OffsetRect(&rc, -rc.left, -rc.top);

        hdc = GetWindowDC(hwnd);

        MyDrawDlgFrame(hdc, &rc);

        ReleaseDC(hwnd, hdc);
        return;
    }

    CMultiDlg::OnNCPaint(hwnd, hrgn);
}


void CGDIDlgBase::OnPaint(HWND hwnd)
{
    if (IsIconic(hwnd) || !m_bBecomingActive)
    {
        CMultiDlg::OnPaint(hwnd);
        return;
    }

    m_bBecomingActive = false;

    prasMainScreen->Lock();
    prasMainScreen->Unlock();
    prasMainScreen->FlipToGDISurface();
}


void CGDIDlgBase::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
#if 0
    switch (id)
    {
        // IDOK means that the user just hit the ENTER key.  So we need
        // to simply extract the ID from the key that was actually pressed
        // then post a message for that command
        case IDOK:
            id = GetWindowID(GetFocus());
            FORWARD_WM_COMMAND(hwnd, id, GetFocus(), 0, PostMessage);
            break;
        default:
            CMultiDlg::OnCommand(hwnd, id, hwndCtl, codeNotify);
            break;
    }
#else
    CMultiDlg::OnCommand(hwnd, id, hwndCtl, codeNotify);
#endif
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

