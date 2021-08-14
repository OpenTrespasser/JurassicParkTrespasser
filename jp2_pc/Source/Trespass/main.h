//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1997.
//
//  File:       main.h
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    21-Oct-97   SHernd   Created
//
//---------------------------------------------------------------------------

#ifndef __MAIN_H__
#define __MAIN_H__

#include "dialogs.h"

class CUIManager;
class CRaster;
class CRasterDC;
class CUIWnd;
class CMultiDlg;
class CGDIDlgBase;

void __cdecl ErrorDlg(HWND hwndParent, UINT uiErrMsg, ...);
int __cdecl MsgDlg(HWND hwndParent,
                   UINT uiType,
                   UINT uiCaption,
                   UINT uiMsg,
                   ...);
int __cdecl ActualMsgDlg(HWND hwndParent,
                         UINT uiType,
                         UINT uiCaption,
                         UINT uiMsg,
                         va_list ap);

BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ErrorDlgProc(HWND, UINT, WPARAM, LPARAM);


class CMainWnd
{
public:
    CMainWnd();
    virtual ~CMainWnd();

    HWND GetHwnd() { return m_hwnd;}

    HWND            m_hwnd;
    CUIManager *    m_pUIMgr;
    bool            m_bRelaunch;
    bool            m_bInit;

    BOOL InitSurface();
    void StartGame();

    void OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized);
    void OnActivateApp(HWND hwnd, BOOL fActivate, DWORD dwThreadId);
    void OnClose(HWND hwnd);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
    BOOL OnEraseBkgnd(HWND hwnd, HDC hdc);
    void OnDestroy(HWND hwnd);
    void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
    void OnChar(HWND hwnd, TCHAR ch, int cRepeat);
    void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
    void OnMouseWheel(HWND hwnd, int x, int y, int zDelta, UINT fwKeys);
    void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
    void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
    void OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
    void OnMButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
    void OnPaint(HWND hwnd);
    void OnTimer(HWND hwnd, UINT id);
    void OnSysCommand(HWND hwnd, UINT cmd, int x, int y);

    void GameLoop();
    void CopyrightScreen();
    void DWIPresents();
    void DWIIntroAVI();
    void IntroAVI();
    void LoadSplash();
    void SendActivate(BOOL fActivate, DWORD dwThreadId);
};

class CConfigureWnd : public CMultiDlg
{
public:
    CConfigureWnd();
    virtual ~CConfigureWnd();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) override;
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) override;
    void OnCancel() override;
    void OnOK() override;

private:

    HWND m_hwndResolutions;
    HWND m_hwndD3DDriver;
    HWND m_hwndList;
    HWND m_hwndTextureSizes;
    HWND m_hwndWindowModes;

	void InitializeCardSelection();
	void InitializeResolutions();
	void InitializeTextureSizes();
    void InitializeWindowModes();
    void OnSelchangeListCard();
};

enum
{
    ERRORDLG_DSOUND,
    ERRORDLG_DDRAW_CERTIFIED,
    ERRORDLG_DDRAW_HARDWARE
};

class CErrorDlg
{
public:
    CErrorDlg(int iErrType);
    virtual ~CErrorDlg();

    HWND        m_hwnd;
    int         m_iType;

    int DoDialog();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnDestroy(HWND hwnd);
};

#endif // __MAIN_H__
