

#include "precomp.h"
#pragma hdrstop


#include "dialogs.h"


extern HINSTANCE    g_hInst;



void CMultiBase::OnClose(HWND hwnd)
{
    FORWARD_WM_CLOSE(hwnd, BaseHandler);
}
void CMultiBase::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, BaseHandler);
}
BOOL CMultiBase::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    return FORWARD_WM_CREATE(hwnd, lpCreateStruct, BaseHandler);
}
void CMultiBase::OnDestroy(HWND hwnd)
{
    RemoveProp(hwnd, "CMultiBase Class"); 
}
BOOL CMultiBase::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    return FORWARD_WM_INITDIALOG(hwnd, hwndFocus, lParam, BaseHandler);
}
void CMultiBase::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    FORWARD_WM_LBUTTONUP(hwnd, x, y, keyFlags, BaseHandler);
}
void CMultiBase::OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    FORWARD_WM_MOUSEMOVE(hwnd, x, y, keyFlags, BaseHandler);
}
LRESULT CMultiBase::OnNotify(HWND hwnd, int idCtrl, LPNMHDR pnmhdr)
{
    return FORWARD_WM_NOTIFY(hwnd, idCtrl, pnmhdr, BaseHandler);
}
void CMultiBase::OnParentNotify(HWND hwnd, UINT msg, HWND hwndChild, int idChild)
{
    FORWARD_WM_PARENTNOTIFY(hwnd, msg, hwndChild, idChild, BaseHandler);
}
void CMultiBase::OnTimer(HWND hwnd, UINT id)
{
    FORWARD_WM_TIMER(hwnd, id, BaseHandler);
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
    CenterWindow(hwnd, GetParent(hwnd));

    return TRUE;
}


LRESULT CALLBACK CMultiBaseProc(HWND hwnd, 
                                UINT message, 
                                WPARAM wParam, 
                                LPARAM lParam)
{
    CMultiBase * pwnd = (CMultiBase *)GetProp(hwnd, "CMultiBase Class");

    if (pwnd == NULL)
    {
        if (message == WM_CREATE)
        {
            pwnd = (CMultiBase *)((LPCREATESTRUCT)lParam)->lpCreateParams;
            SetProp(hwnd, "CMultiBase Class", pwnd);
        
            if (pwnd == NULL)
            {
                return TRUE;
            }

            pwnd->m_hwnd = hwnd;
        }
        else
        {
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }
    
    switch (message)
    {
        HANDLE_MSG(hwnd, WM_COMMAND,        pwnd->OnCommand);
        HANDLE_MSG(hwnd, WM_DESTROY,        pwnd->OnDestroy);
        HANDLE_MSG(hwnd, WM_CREATE,         pwnd->OnCreate);
        HANDLE_MSG(hwnd, WM_INITDIALOG,     pwnd->OnInitDialog);
        HANDLE_MSG(hwnd, WM_NOTIFY,         pwnd->OnNotify);
        HANDLE_MSG(hwnd, WM_PARENTNOTIFY,   pwnd->OnParentNotify);
    }
    
    return pwnd->BaseHandler(hwnd, message, wParam, lParam);
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
    
    switch (message)
    {
        HANDLE_MSG(hwnd, WM_CLOSE,          pwnd->OnClose);
        HANDLE_MSG(hwnd, WM_COMMAND,        pwnd->OnCommand);
        HANDLE_MSG(hwnd, WM_DESTROY,        pwnd->OnDestroy);
        HANDLE_MSG(hwnd, WM_DROPFILES,      pwnd->OnDropFiles);
        HANDLE_MSG(hwnd, WM_CREATE,         pwnd->OnCreate);
        HANDLE_MSG(hwnd, WM_INITDIALOG,     pwnd->OnInitDialog);
        HANDLE_MSG(hwnd, WM_MOUSEMOVE,      pwnd->OnMouseMove);
        HANDLE_MSG(hwnd, WM_LBUTTONUP,      pwnd->OnLButtonUp);
        HANDLE_MSG(hwnd, WM_NOTIFY,         pwnd->OnNotify);
        HANDLE_MSG(hwnd, WM_PARENTNOTIFY,   pwnd->OnParentNotify);
        HANDLE_MSG(hwnd, WM_TIMER,          pwnd->OnTimer);
    }
    
    return pwnd->BaseHandler(hwnd, message, wParam, lParam);
}



