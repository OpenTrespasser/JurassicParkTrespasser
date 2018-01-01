

#include "precomp.h"
#pragma hdrstop


#include "dialogs.h"
#include "supportfn.hpp"


extern HINSTANCE    g_hInst;


BOOL 
CenterWindow(HWND hwndChild, HWND hwndParent)
{
    RECT    rChild, rParent;
    int     wChild, hChild, wParent, hParent;
    int     wScreen, hScreen, xNew, yNew;
    HDC     hdc;

    // Get the Height and Width of the child window
    GetWindowRect (hwndChild, &rChild);
    wChild = rChild.right - rChild.left;
    hChild = rChild.bottom - rChild.top;

    // Get the Height and Width of the parent window
    if (hwndParent == NULL)
    {
        SetRect(&rParent, 
                0, 
                0, 
                GetSystemMetrics(SM_CXSCREEN), 
                GetSystemMetrics(SM_CYSCREEN));
    }
    else
    {
        GetWindowRect (hwndParent, &rParent);
    }

    wParent = rParent.right - rParent.left;
    hParent = rParent.bottom - rParent.top;

    // Get the display limits
    hdc = GetDC (hwndChild);
    wScreen = GetDeviceCaps (hdc, HORZRES);
    hScreen = GetDeviceCaps (hdc, VERTRES);
    ReleaseDC (hwndChild, hdc);

    // Calculate new X position, then adjust for screen
    xNew = rParent.left + ((wParent - wChild) /2);
    if (xNew < 0) {
            xNew = 0;
    } else if ((xNew+wChild) > wScreen) {
            xNew = wScreen - wChild;
    }

    // Calculate new Y position, then adjust for screen
    yNew = rParent.top  + ((hParent - hChild) /2);
    if (yNew < 0) {
            yNew = 0;
    } else if ((yNew+hChild) > hScreen) {
            yNew = hScreen - hChild;
    }

    // Set it, and return
    return SetWindowPos (hwndChild, NULL,
            xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


LRESULT CMultiWnd::BaseHandler(HWND hwnd, 
                               UINT message, 
                               WPARAM wParam, 
                               LPARAM lParam)
{
    return DefWindowProc(hwnd, message, wParam, lParam);
}


LRESULT CMultiDlg::BaseHandler(HWND hwnd, 
                               UINT message, 
                               WPARAM wParam, 
                               LPARAM lParam)
{
    SetDlgMsgResult(hwnd, message, FALSE);
    return (LRESULT)FALSE;
}




int CMultiDlg::MultiDialogBox(HINSTANCE hInst, 
                              LPCTSTR lpTemplate, 
                              HWND hwndParent)
{
    int     iRet;

    iRet = DialogBoxParam(g_hInst,
                          lpTemplate, 
                          hwndParent, 
                          (DLGPROC)CMultiDlgProc,
                          (LPARAM)this);

    return iRet;
}



BOOL CMultiDlg::CreateMultiDialog(HINSTANCE hInst,
                                  LPCTSTR lpTemplate,
                                  HWND hwndParent)
{
    HWND    hwnd;

    hwnd = CreateDialogParam(g_hInst,
                             lpTemplate,
                             hwndParent,
                             (DLGPROC)CMultiDlgProc,
                             (LPARAM)this);

    Assert(hwnd == m_hwnd);

    return (hwnd == m_hwnd);
}


BOOL CMultiDlg::CreateIndirect(HINSTANCE hInst,
                               LPCDLGTEMPLATE lpTemplate,
                               HWND hwndParent)
{
    HWND        hwnd;

    hwnd = CreateDialogIndirectParam(hInst,
                                     lpTemplate, 
                                     hwndParent,
                                     (DLGPROC)CMultiDlgProc,
                                     (LPARAM)this);

    Assert(hwnd == m_hwnd);

    return (hwnd == m_hwnd);
}



void CMultiDlg::OnCancel()
{
    EndDialog(m_hwnd, IDCANCEL);
}



void CMultiDlg::OnOK()
{
    EndDialog(m_hwnd, IDOK);
}



void CMultiDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
        case IDCANCEL:
            OnCancel();
            break;

        case IDOK:
            OnOK();
            break;
    }
}



BOOL CMultiDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    return TRUE;
}


BOOL CALLBACK CMultiDlgProc(HWND hwnd, 
                            UINT message, 
                            WPARAM wParam, 
                            LPARAM lParam)
{
    CMultiBase * pwnd = (CMultiBase *)GetProp(hwnd, "CMultiBase Class");

    if (pwnd == NULL)
    {
        if (message == WM_INITDIALOG)
        {
            pwnd = (CMultiBase *)lParam;
            SetProp(hwnd, "CMultiBase Class", pwnd);
        
            if (pwnd == NULL)
            {
                return TRUE;
            }

            pwnd->m_hwnd = hwnd;
        }
        else
        {
            return FALSE;
        }
    }
    
	// Trace(("message   %x\n", message));
   switch (message)
    {
        HANDLE_MSG(hwnd, WM_ACTIVATE,       pwnd->OnActivate);
        HANDLE_MSG(hwnd, WM_ACTIVATEAPP,    pwnd->OnActivateApp);
        HANDLE_MSG(hwnd, WM_CHAR,           pwnd->OnChar);
        HANDLE_MSG(hwnd, WM_CLOSE,          pwnd->OnClose);
        HANDLE_MSG(hwnd, WM_COMMAND,        pwnd->OnCommand);
        HANDLE_MSG(hwnd, WM_CREATE,         pwnd->OnCreate);
		HANDLE_MSG(hwnd, WM_CTLCOLORMSGBOX, pwnd->OnCtlColor);
        HANDLE_MSG(hwnd, WM_CTLCOLOREDIT,   pwnd->OnCtlColor);
        HANDLE_MSG(hwnd, WM_CTLCOLORLISTBOX,pwnd->OnCtlColor);
        HANDLE_MSG(hwnd, WM_CTLCOLORBTN,    pwnd->OnCtlColor);
        HANDLE_MSG(hwnd, WM_CTLCOLORDLG,    pwnd->OnCtlColor);
        HANDLE_MSG(hwnd, WM_CTLCOLORSCROLLBAR, pwnd->OnCtlColor);
        HANDLE_MSG(hwnd, WM_CTLCOLORSTATIC, pwnd->OnCtlColor);
        HANDLE_MSG(hwnd, WM_DESTROY,        pwnd->OnDestroy);
        HANDLE_MSG(hwnd, WM_DRAWITEM,       pwnd->OnDrawItem);
        HANDLE_MSG(hwnd, WM_DROPFILES,      pwnd->OnDropFiles);
        HANDLE_MSG(hwnd, WM_ERASEBKGND,     pwnd->OnEraseBkgnd);
        HANDLE_MSG(hwnd, WM_HSCROLL,        pwnd->OnHScroll);
        HANDLE_MSG(hwnd, WM_VSCROLL,        pwnd->OnVScroll);
        HANDLE_MSG(hwnd, WM_INITDIALOG,     pwnd->OnInitDialog);
        HANDLE_MSG(hwnd, WM_KEYDOWN,        pwnd->OnKey);
        HANDLE_MSG(hwnd, WM_KEYUP,          pwnd->OnKey);
        HANDLE_MSG(hwnd, WM_LBUTTONUP,      pwnd->OnLButtonUp);
        HANDLE_MSG(hwnd, WM_LBUTTONDOWN,    pwnd->OnLButtonDown);
        HANDLE_MSG(hwnd, WM_LBUTTONDBLCLK,  pwnd->OnLButtonDown);
        HANDLE_MSG(hwnd, WM_MEASUREITEM,    pwnd->OnMeasureItem);
        HANDLE_MSG(hwnd, WM_MOUSEMOVE,      pwnd->OnMouseMove);
        HANDLE_MSG(hwnd, WM_NCHITTEST,      pwnd->OnNCHitTest);
        HANDLE_MSG(hwnd, WM_NCPAINT,        pwnd->OnNCPaint);
        HANDLE_MSG(hwnd, WM_NOTIFY,         pwnd->OnNotify);
        HANDLE_MSG(hwnd, WM_PARENTNOTIFY,   pwnd->OnParentNotify);
        HANDLE_MSG(hwnd, WM_TIMER,          pwnd->OnTimer);
        HANDLE_MSG(hwnd, WM_PAINT,          pwnd->OnPaint);
        HANDLE_MSG(hwnd, WM_SETFOCUS,       pwnd->OnSetFocus);
        HANDLE_MSG(hwnd, WM_KILLFOCUS,      pwnd->OnKillFocus);
        HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING, pwnd->OnWindowPosChanging);
        HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGED, pwnd->OnWindowPosChanged);
        HANDLE_MSG(hwnd, WM_SYSCOMMAND,     pwnd->OnSysCommand);
   }
    
    return pwnd->BaseHandler(hwnd, message, wParam, lParam);
}



