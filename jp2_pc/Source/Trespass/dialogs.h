

#ifndef __DIALOGS_H__
#define __DIALOGS_H__


class CMultiDlg;



#if 0
#ifndef HANDLE_WM_NOTIFY
/* LRESULT Cls_OnNotify(HWND hwnd, int idCtrl, LPNMHDR pnmhdr) */
#define HANDLE_WM_NOTIFY(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((hwnd), (int)(wParam), (LPNMHDR)(lParam))
#define FORWARD_WM_NOTIFY(hwnd, idCtrl, pnmhdr, fn) \
    (LRESULT)(fn)((hwnd), WM_NOTIFY, (WPARAM)(idCtrl), (LPARAM)(pnmhdr))
#endif
#endif

BOOL CenterWindow(HWND hwndChild, HWND hwndParent);
LRESULT CALLBACK CMultiBaseProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK CMultiDlgProc(HWND, UINT, WPARAM, LPARAM);


class CMultiBase
{
friend LRESULT CALLBACK CMultiBaseProc(HWND, UINT, WPARAM, LPARAM);
friend BOOL CALLBACK CMultiDlgProc(HWND, UINT, WPARAM, LPARAM);

public:
    CMultiBase() { m_hwnd = NULL; }
    virtual ~CMultiBase() { m_hwnd = NULL;}

    HWND GetHwnd() { return m_hwnd;}

protected:
    HWND        m_hwnd;

    virtual LRESULT BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

    virtual void OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
    {
        FORWARD_WM_ACTIVATE(hwnd, state, hwndActDeact, fMinimized, BaseHandler);
    }

    virtual void OnActivateApp(HWND hwnd, BOOL fActivate, DWORD dwThreadId)
    {
        FORWARD_WM_ACTIVATEAPP(hwnd, fActivate, dwThreadId, BaseHandler);
    }

    virtual void OnClose(HWND hwnd) 
    {   
        FORWARD_WM_CLOSE(hwnd, BaseHandler);
    }

    virtual void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, BaseHandler);
    }

    virtual BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
    {
        return FORWARD_WM_CREATE(hwnd, lpCreateStruct, BaseHandler);
    }

    virtual HBRUSH OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type)
    {
        switch (type)
        {
            case WM_CTLCOLORMSGBOX:
                return FORWARD_WM_CTLCOLORMSGBOX(hwnd, hdc, hwndChild, BaseHandler);
                break;
            case WM_CTLCOLOREDIT:
                return FORWARD_WM_CTLCOLOREDIT(hwnd, hdc, hwndChild, BaseHandler);
                break;
            case WM_CTLCOLORLISTBOX:
                return FORWARD_WM_CTLCOLORLISTBOX(hwnd, hdc, hwndChild, BaseHandler);
                break;
            case WM_CTLCOLORBTN:
                return FORWARD_WM_CTLCOLORBTN(hwnd, hdc, hwndChild, BaseHandler);
                break;
            case WM_CTLCOLORDLG:
                return FORWARD_WM_CTLCOLORDLG(hwnd, hdc, hwndChild, BaseHandler);
                break;
            case WM_CTLCOLORSCROLLBAR:
                return FORWARD_WM_CTLCOLORSCROLLBAR(hwnd, hdc, hwndChild, BaseHandler);
                break;
            case WM_CTLCOLORSTATIC:
                return FORWARD_WM_CTLCOLORSTATIC(hwnd, hdc, hwndChild, BaseHandler);
                break;
        }

        return NULL;
    }

    virtual void OnDestroy(HWND hwnd)
    {
        RemoveProp(hwnd, "CMultiBase Class"); 
        m_hwnd = NULL;
    }

    virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        return FORWARD_WM_INITDIALOG(hwnd, hwndFocus, lParam, BaseHandler);
    }

    virtual BOOL OnEraseBkgnd(HWND hwnd, HDC hdc)
    {
        return FORWARD_WM_ERASEBKGND(hwnd, hdc, BaseHandler);
    }

    virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
    {
        FORWARD_WM_LBUTTONUP(hwnd, x, y, keyFlags, BaseHandler);
    }

    virtual void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
    {
        FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, keyFlags, BaseHandler);
    }

    virtual void OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
    {
        FORWARD_WM_MEASUREITEM(hwnd, lpMeasureItem, BaseHandler);
    }

    virtual void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
    {
        FORWARD_WM_MOUSEMOVE(hwnd, x, y, keyFlags, BaseHandler);
    }

    virtual UINT OnNCHitTest(HWND hwnd, int x, int y)
    {
        return FORWARD_WM_NCHITTEST(hwnd, x, y, BaseHandler);
    }

    virtual void OnNCPaint(HWND hwnd, HRGN hrgn)
    {
        FORWARD_WM_NCPAINT(hwnd, hrgn, BaseHandler);
    }

    virtual LRESULT OnNotify(HWND hwnd, int idCtrl, LPNMHDR pnmhdr)
    {
        return FORWARD_WM_NOTIFY(hwnd, idCtrl, pnmhdr, BaseHandler);
    }

    virtual void OnParentNotify(HWND hwnd, UINT msg, HWND hwndChild, int idChild)
    {
        FORWARD_WM_PARENTNOTIFY(hwnd, msg, hwndChild, idChild, BaseHandler);
    }

    virtual void OnSetFocus(HWND hwnd, HWND hwndOldFocus)
    {
        FORWARD_WM_SETFOCUS(hwnd, hwndOldFocus, BaseHandler);
    }

    virtual void OnKillFocus(HWND hwnd, HWND hwndNewFocus)
    {
        FORWARD_WM_KILLFOCUS(hwnd, hwndNewFocus, BaseHandler);
    }

    virtual void OnTimer(HWND hwnd, UINT id)
    {
        FORWARD_WM_TIMER(hwnd, id, BaseHandler);
    }

    virtual void OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
    {
		FORWARD_WM_DRAWITEM(hwnd, lpDrawItem, BaseHandler);
    }

    virtual void OnDropFiles(HWND hwnd, HDROP hdrop)
    {
        FORWARD_WM_DROPFILES(hwnd, hdrop, BaseHandler);
    }

    virtual void OnPaint(HWND hwnd)
    {
        FORWARD_WM_PAINT(hwnd, BaseHandler);
    }

    virtual void OnChar(HWND hwnd, TCHAR ch, int cRepeat)
    {
        FORWARD_WM_CHAR(hwnd, ch, cRepeat, BaseHandler);
    }

    virtual void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
    {
        if (fDown)
        {
            FORWARD_WM_KEYDOWN(hwnd, vk, cRepeat, flags, BaseHandler);
        }
        else
        {
            FORWARD_WM_KEYUP(hwnd, vk, cRepeat, flags, BaseHandler);
        }
    }

    virtual void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
    {
        FORWARD_WM_HSCROLL(hwnd, hwndCtl, code, pos, BaseHandler);
    }

    virtual void OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
    {
        FORWARD_WM_VSCROLL(hwnd, hwndCtl, code, pos, BaseHandler);
    }

    virtual void OnSysCommand(HWND hwnd, UINT cmd, int x, int y)
    {
        FORWARD_WM_SYSCOMMAND(hwnd, cmd, x, y, BaseHandler);
    }

    virtual BOOL OnWindowPosChanging(HWND hwnd, LPWINDOWPOS lpwpos)
    {
        return FORWARD_WM_WINDOWPOSCHANGING(hwnd, lpwpos, BaseHandler);
    }

    virtual void OnWindowPosChanged(HWND hwnd, const LPWINDOWPOS lpwpos)
    {
        FORWARD_WM_WINDOWPOSCHANGED(hwnd, lpwpos, BaseHandler);
    }

};



class CMultiWnd : public CMultiBase
{
public:
    CMultiWnd() {;}
    virtual ~CMultiWnd() {;}

protected:
    virtual LRESULT BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};


class CMultiDlg : public CMultiBase
{
public:
    CMultiDlg() {;}
    virtual ~CMultiDlg() {;}

    virtual int     MultiDialogBox(HINSTANCE hInst, 
                                   LPCTSTR lpTemplate, 
                                   HWND hwndParent);
    virtual BOOL    CreateMultiDialog(HINSTANCE hInst,
                                      LPCTSTR lpTemplate,
                                      HWND hwndParent);
    virtual BOOL    CreateIndirect(HINSTANCE hInst,
                                   LPCDLGTEMPLATE lpTemplate,
                                   HWND hwndParent);
                                   

    virtual void    OnCancel();
    virtual void    OnOK();

protected:
    virtual LRESULT BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override;

    virtual void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) override;
    virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) override;
};


#endif // ifndef __DIALOGS_H__

