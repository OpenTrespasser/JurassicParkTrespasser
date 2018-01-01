//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       dlgrender.h
//
//  Contents:   User Dialog code to change the rendering statistics.
//
//  Classes:
//
//  Functions:
//
//  History:    06-Jan-98   SHernd   Created
//
//---------------------------------------------------------------------------

#ifndef __DLGRENDER_H__
#define __DLGRENDER_H__

#include "gdidlgbase.h"

class CRenderingDlgVideo : public CGDIDlgBase
{
public:
    CRenderingDlgVideo();
    virtual ~CRenderingDlgVideo();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnDestroy(HWND hwnd);
    BOOL OnEraseBkgnd(HWND hwnd, HDC hdcDst);

    void Persist();

    RECT    m_rcDisplay;

private:
    GUID            m_guidDefAdapter;
    GUID            m_guidDefD3D;
    int             m_iDefWidth;
    int             m_iDefHeight;

    HWND            m_hwndModes;
    HWND            m_hwndAdapters;
    HWND            m_hwndD3D;
    GUID            m_guidAdapter;
    GUID            m_guidD3D;
    BOOL            m_bD3DAvail;
    BOOL            m_bUseHardware;
    BOOL            m_bUseSystemMem;
    int             m_iWidth;
    int             m_iHeight;

    void    AddAdaptersToList();
    void    SelectCurrentAdapter();
    void    ChangeAdapter();

    void    AddResolutionsToList();
    void    SelectCurrentResolution();

    void    AddD3DToList();
    void    SelectCurrentD3D();

    void    ResetListItems();
};


class CRenderingDlgTerrain : public CGDIDlgBase
{
public:
    CRenderingDlgTerrain();
    virtual ~CRenderingDlgTerrain();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    BOOL OnEraseBkgnd(HWND hwnd, HDC hdcDst);

    RECT    m_rcDisplay;
};


class CRenderingDlgFog : public CGDIDlgBase
{
public:
    CRenderingDlgFog();
    virtual ~CRenderingDlgFog();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    BOOL OnEraseBkgnd(HWND hwnd, HDC hdcDst);

    RECT    m_rcDisplay;
};


class CRenderingDlgBase : public CGDIDlgBase
{
public:
    CRenderingDlgBase();
    virtual ~CRenderingDlgBase();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    LRESULT OnNotify(HWND hwnd, int idCtrl, LPNMHDR pnmhdr);

    void OnOK();

    void Persist();

    CRenderingDlgVideo      m_dlgVideo;
    CRenderingDlgFog        m_dlgFog;
    CRenderingDlgTerrain    m_dlgTerrain;

private:
    HWND            m_hwndTab;       // tab control
    HWND            m_hwndDisplay;   // current child dialog box
    RECT            m_rcDisplay;     // display rectangle for the tab control
    DLGTEMPLATE *   m_apRes[3];

    void    OnSelChanged(HWND hwnd);
};


#endif

