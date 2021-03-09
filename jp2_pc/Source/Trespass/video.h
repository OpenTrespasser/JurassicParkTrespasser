//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1997.
//
//  File:       VIDEO.h
//
//  Contents:   Header file for Videos
//
//  Classes:
//
//  Functions:  
//
//  History:    08-Jul-98   SHernd   Created
//
//---------------------------------------------------------------------------


#ifndef __VIDEO_H__
#define __VIDEO_H__


#include "uiwnd.h"
#include "smacker/SMACK.H"


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


typedef struct {
  s32 x;
  s32 y;
  s32 w;
  s32 h;
} whRECT;

class CVideoWnd : public CUIWnd
{
public:
    CVideoWnd(CUIManager * puimgr);
    virtual ~CVideoWnd();

    BOOL Play(LPCSTR pszFile);
    void OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags) override;

    BOOL AllowEscape() { return FALSE; }
    void UpdatePalette();

    void NextSmackerFrame();
    void NextDirect();
    void NextNonDirect();


    void Pause();
    void Resume();

    char                m_szFile[_MAX_PATH];
    Smack *             m_pSmack;
    SmackBuf *          m_pBuf;
    rptr<CRasterMem>    m_pBuff;
    int                 m_iLastKey;
    int                 m_iTop;
    int                 m_iLeft;
    bool                m_fVideoOver;
    int                 m_iSurfaceType;
    bool                m_fDirect;
    bool                m_bFullRedraw;
};


#endif

