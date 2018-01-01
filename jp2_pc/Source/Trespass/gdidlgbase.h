//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       gdidlgbase.h
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

#ifndef __GDIDLGBASE_H__
#define __GDIDLGBASE_H__

#include "dialogs.h"

class CRasterDC;

void SetStaticOwnerDraw(HWND hwnd, int idCtl);
DWORD GDIDialogHandler(CUIWnd * pbase, 
                       CGDIDlgBase * pmultidlg, 
                       LPCTSTR pszTemplate, 
                       HWND hwndParent);
LRESULT CALLBACK CtrlProcHack(HWND, UINT, WPARAM, LPARAM);

class CGDIDlgBase : public CMultiDlg
{
public:
    CGDIDlgBase();
    virtual ~CGDIDlgBase();

    VOID OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
    HBRUSH OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type);
    BOOL OnEraseBkgnd(HWND hwnd, HDC hdc);
    void OnActivateApp(HWND hwnd, BOOL fActivate, DWORD dwThreadId);
    void OnNCPaint(HWND hwnd, HRGN hrgn);
    void OnPaint(HWND hwnd);
    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnDestroy(HWND hwnd);

protected:
    CRasterDC *     m_pRaster;
    bool            m_bBecomingActive;
    HFONT           m_hFont;

};


#endif // __GDIDLGBASE_H__

