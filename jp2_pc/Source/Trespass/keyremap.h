//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       keyremap.h
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    05-Aug-98   SHernd   Created
//
//---------------------------------------------------------------------------


#ifndef __KEYREMAP_H__
#define __KEYREMAP_H__

#include "uidlgs.h"

class CControlsWnd : public CUIDlg
{
public:
    CControlsWnd(CUIManager * puimgr);
    virtual ~CControlsWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);
    void UIButtonUp(CUIButton * pbutton);
    void UIHotspotClick(CUIHotspot * pctrl, BOOL bDown);
    void OnActivateApp(BOOL fActivate, DWORD dwThreadId);

    void OnMButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags);
    void OnLButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags);
    void OnRButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags);
    void OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags);

protected:
    bool        m_bKeyWait;
    int         m_iCmdNum;
    COLORREF    m_crFG;
    COLORREF    m_crBK;
    BOOL        m_bTrans;
    char        m_sz[50];
    bool        m_bIgnoreKey;
    UINT        m_uiIgnoreKey;
    SKeyMapping m_CurrKeys[KEYMAP_COUNT];
    int         m_iDefGore;
    int         m_iGore;
    int         m_iInvertMouse;
    int         m_iDefInvertMouse;

    void InitialUpdate();
    void RegisterVK(UINT vk);
    BOOL IsAlreadyUsed(UINT vk, int & iIndex);
    void UpdateCmdText(int iCmd, UINT uiKey);
    int CmdToIndex(int iCmd);
    int IndexToCmd(int iIndex);
    void UpdateGore();
    void UpdateInvertMouse();
};

#endif // __KEYREMAP_H__

