//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1996.
//
//  File:       uiwnd.h
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    21-Nov-96   SHernd   Created
//
//---------------------------------------------------------------------------

#ifndef __UIWND_H__
#define __UIWND_H__

#include <vector>

#include "ctrls.h"

class CUIManager;
class CUIWnd;
class CRasterMem;
typedef struct tagWNDFILETOKEN WNDFILETOKEN;


class CUIManager
{
public:
    CUIManager();
    ~CUIManager();

    BOOL Attach(CUIWnd * puiwnd);
    BOOL Detach(CUIWnd * puiwnd);

    CUIWnd * GetActiveUIWnd();

    void CopySurfaceToClipboard();

    void    Draw();
    void    DrawWindowChain(LPBYTE pbDst,
                            int iDstWidth,
                            int iDstHeight,
                            int iDstPitch,
                            int iDstBytes,
                            RECT * prcClip);
    void    DrawWindowChain(CRaster * pRaster, RECT * prcClip);
    void    DrawMouse(LPBYTE pbDst,
                      int iDstWidth,
                      int iDstHeight,
                      int iDstPitch,
                      int iDstBytes,
                      RECT * prcClip);
    void    DrawMouse(CRaster * pRaster, RECT * prcClip);
    void    ResizeScreen();

    HWND            m_hwnd;
    bool            m_bActive;
    bool            m_bPause;
    RECT            m_rcInvalid;
    POINT           m_ptMouse;
    BOOL            m_bDrawMouse;
    int             m_iMouse;
    CRaster *       m_prasMouse[2];
    WORD            m_wTransColor;
    CUICtrl *       m_pUICtrlMouseCaptured;

    std::vector<CUIWnd *> m_vUIWnd;

};

class CUIWnd : public CUICtrlCallback
{
public:
    CUIWnd(CUIManager * puimgr);
    virtual ~CUIWnd();

    CUIManager * m_pUIMgr;          // Interface manager
    bool        m_bExitWnd;         // exit the UIWnd handler
    DWORD       m_dwExitValue;      // exit value of UIWnd handler

    RECT        m_rc;               // window rect
    std::vector<CUICtrl *> m_vUICtrls;

    CRaster *   m_pRaster;          // Background To Draw
	static BOOL bIgnoreSysKey;

    virtual void GetWndFile(LPSTR psz, int ic);
    virtual BOOL StartUIWnd();
    virtual void EndUIWnd(DWORD dwExitValue);
    virtual void WindowLoop();   // the loop handler for this window (ie.message pump)
    virtual void InnerLoopCall();
    virtual void CheckMessages();
    virtual void PreLoopCall();

    // Window information loading functions
    virtual BOOL LoadWindowInfo();
    virtual BOOL HandleTokenRead(HANDLE hFile, WNDFILETOKEN * pwft);
    virtual BOOL ReadUICtrl(HANDLE hFile);
    virtual BOOL ReadBackground(HANDLE hFile);


    virtual BOOL AddToUICtrlSet(CUICtrl * pctrl);
    virtual CUICtrl * GetUICtrl(DWORD dwID);
    virtual CUICtrl *   CaptureMouse(CUICtrl * pctrl) override;
    virtual void        ReleaseMouse(CUICtrl * pctrl) override;
    virtual void InvalidateRect(RECT * prc);
    virtual void CtrlInvalidateRect(RECT * prc) override { InvalidateRect(prc); }
    virtual void DoUIHandling();

    virtual void DrawWndInfo(LPBYTE pbDst,
                             int iDstWidth,
                             int iDstHeight,
                             int iDstPitch,
                             int iDstBytes,
                             RECT * prc);
    virtual void DrawUICtrls(LPBYTE pbDst,
                             int iDstWidth,
                             int iDstHeight,
                             int iDstPitch,
                             int iDstBytes,
                             RECT * prc);
    virtual void DrawIntoSurface(LPBYTE pbDst,
                                 int iDstWidth,
                                 int iDstHeight,
                                 int iDstPitch,
                                 int iDstBytes,
                                 RECT * prcClip);

    virtual void DrawWndInfo(CRaster * pRaster, RECT * prc);
    virtual void DrawUICtrls(CRaster * pRaster, RECT * prc);
    virtual void DrawIntoSurface(CRaster * pRaster, RECT * prcClip);
    virtual void ResizeScreen(int iWidth, int iHeight);

    virtual BOOL OnCreate();
    virtual void OnDestroy();

    virtual void OnActivateApp(BOOL fActivate, DWORD dwThreadId);
    virtual void OnCommand(int id, HWND hwndCtl, UINT codeNotify);
    virtual void OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags);
    virtual void OnChar(TCHAR ch, int cRepeat);
    virtual void OnMouseMove(int x, int y, UINT keyFlags);
    virtual void OnMouseWheel(int x, int y, int zDelta, UINT fwKeys);
    virtual void OnLButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags);
    virtual void OnLButtonUp(int x, int y, UINT keyFlags);
    virtual void OnRButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags);
    virtual void OnMButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags);
    virtual void OnTimer(UINT uiID);
    virtual void OnPaint(HWND hwnd);
    virtual BOOL OnEraseBkgnd(HWND hwnd, HDC hdc);
    virtual void OnWindowPosChanged();
};



#endif // #ifndef __UIWND_H__


