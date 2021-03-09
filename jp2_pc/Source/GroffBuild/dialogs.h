

#ifndef __DIALOGS_H__
#define __DIALOGS_H__


class CMultiDlg;



#ifndef HANDLE_WM_NOTIFY
/* LRESULT Cls_OnNotify(HWND hwnd, int idCtrl, LPNMHDR pnmhdr) */
#define HANDLE_WM_NOTIFY(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((hwnd), (int)(wParam), (LPNMHDR)(lParam))
#define FORWARD_WM_NOTIFY(hwnd, idCtrl, pnmhdr, fn) \
    (LRESULT)(fn)((hwnd), WM_NOTIFY, (WPARAM)(idCtrl), (LPARAM)(pnmhdr))
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

    virtual void OnClose(HWND hwnd);
    virtual void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    virtual BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
    virtual void OnDestroy(HWND hwnd);
    virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
    virtual void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
    virtual LRESULT OnNotify(HWND hwnd, int idCtrl, LPNMHDR pnmhdr);
    virtual void OnParentNotify(HWND hwnd, UINT msg, HWND hwndChild, int idChild);
    virtual void OnTimer(HWND hwnd, UINT id);
    virtual void OnDropFiles(HWND hwnd, HDROP hdrop)
    {
        FORWARD_WM_DROPFILES(hwnd, hdrop, BaseHandler);
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

    virtual void    OnCancel();
    virtual void    OnOK();

protected:
    virtual LRESULT BaseHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override;

    virtual void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) override;
    virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) override;
};


#endif // ifndef __DIALOGS_H__

