//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1997.
//
//  File:       uidlgs.h
//
//  Contents:   Header file for User Interface Dialogs
//
//  Classes:
//
//  Functions:  
//
//  History:    30-Jan-97   SHernd   Created
//
//---------------------------------------------------------------------------


#ifndef __UIDLGS_H__
#define __UIDLGS_H__


#include "uiwnd.h"

#define KEYSTYLE_NONE           0
#define KEYSTYLE_ESCAPE         1
#define KEYSTYLE_ENTER          2

class CDib;

class CUIDlg : public CUIWnd
{
public:
    CUIDlg(CUIManager * puimgr);
    virtual ~CUIDlg();

    virtual void InnerLoopCall() 
    { 
        CUIWnd::InnerLoopCall();
    }

    void OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags)
    {
        if (!fDown && vk == VK_ESCAPE)
        {
            EscapeKey();
        }
        else if (!fDown && vk == VK_RETURN)
        {
            EnterKey();
        }
    }

    BOOL UIButtonAudioCmd(CUIButton * pbutton, BUTTONCMD bc);

    BOOL OnCreate();
    void OnDestroy();

    virtual BOOL AllowEscape() { return m_bKeyStyle & KEYSTYLE_ESCAPE; }
    virtual BOOL AllowEnter() { return m_bKeyStyle & KEYSTYLE_ENTER; }
    virtual void EscapeKey() { if (AllowEscape()) EndUIWnd(0); }
    virtual void EnterKey() { if (AllowEnter()) EndUIWnd(1); }

    BYTE            m_bKeyStyle;
};

class CMainScreenWnd : public CUIDlg
{
public:
    CMainScreenWnd(CUIManager * puimgr);
    virtual ~CMainScreenWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);
    void OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags);
    void OnTimer(UINT uiID);

    void UIButtonUp(CUIButton * pbutton);

private:
    UINT                m_uiInitial;
    CUICtrl *           m_pctrlAMDLogo;
    static bool         m_fAMDShowLogoOnce;

    void DirectLoad();
};


class CLoaderWnd : public CUIDlg
{
public:
    CLoaderWnd(CUIManager * puimgr);
    virtual ~CLoaderWnd();

    BOOL SetupUIWnd();
    void DestroyUIWnd();
    void InnerWindowLoop(bool bPaint);

    BOOL OnCreate();
    void OnTimer(UINT uiID);

    void GetWndFile(LPSTR psz, int ic);

    DWORD   HandleNotify(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);

private:
    CRITICAL_SECTION        m_cs;
    UINT                    m_uiTimer;
    int                     m_iCycle;
    CUITextbox *            m_puitext;

	// Total size for copy.
	DWORD dwCopyTotal;

	// Current percentages.
	int iLoadPercent;
	int iCopyPercent;

	// Percentages at last update.
	int iLastCopyPercent;
	int iLastLoadPercent;

    void SetupBackgroundImage();
};


class CDirectLoadWnd : public CUIDlg
{
public:
    CDirectLoadWnd(CUIManager * puimgr);
    virtual ~CDirectLoadWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);

    void UIButtonUp(CUIButton * pbutton);
    void UIListboxClick(CUICtrl * pctrl, int iIndex);
    void UIListboxDblClk(CUICtrl * pctrl, int iIndex);
};


class CNewGameWnd : public CUIDlg
{
public:
    CNewGameWnd(CUIManager * puimgr);
    virtual ~CNewGameWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);

    void UIButtonUp(CUIButton * pbutton);
};


class CGameWnd : public CUIWnd
{
public:
    CGameWnd(CUIManager * puimgr);
    virtual ~CGameWnd();

    BOOL OnCreate();
    void OnDestroy();

    void GetWndFile(LPSTR psz, int ic);
    void OnKey(UINT vk, BOOL fDown, int cRepet, UINT flags);
    void OnChar(TCHAR ch, int cRepeat);
    BOOL OnEraseBkgnd(HWND hwnd, HDC hdc);
    void DrawWndInfo(CRaster * pRaster, RECT * prc);
    void ResizeScreen(int iWidth, int iHeight);
    void InnerLoopCall();

    void UIEditboxKey(CUICtrl * pctrl, UINT vk, int cRepeat, UINT flags);

    void SetupGameStoppage();
    void ClearGameStoppage(BOOL bStartSim);

    BOOL            m_bPaused;
    int             m_iClear;
    CUIEditbox *    m_puictlCheat;
    int             m_iNoticedDead;
};


class CQuitWnd : public CUIDlg
{
public:
    CQuitWnd(CUIManager * puimgr);
    virtual ~CQuitWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);

    void UIButtonUp(CUIButton * pbutton);
};

class COptionsWnd : public CUIDlg
{
public:
    COptionsWnd(CUIManager * puimgr);
    virtual ~COptionsWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);

    void UIButtonUp(CUIButton * pbutton);
};

class CInGameOptionsWnd : public CUIDlg
{
public:
    CInGameOptionsWnd(CUIManager * puimgr);
    virtual ~CInGameOptionsWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);

    void UIButtonUp(CUIButton * pbutton);
};


class CAudioWnd : public CUIDlg
{
public:
    CAudioWnd(CUIManager * puimgr);
    virtual ~CAudioWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);

    void UIButtonUp(CUIButton * pbutton);
    void UISliderChange(CUISlider * pctrl, int iNewValue);
    void UIHotspotClick(CUIHotspot * pctrl, BOOL bDown);

    void OnOk();
    void OnCancel();

private:
    UINT        m_uiVolume;
    BOOL        m_bSFX : 1;
    BOOL        m_bAmbient : 1;
    BOOL        m_bVO : 1;
    BOOL        m_bMusic : 1;
    BOOL        m_bSubtitles : 1;
    BOOL        m_bEnable3D : 1;
};


typedef struct tagSAVEGAMEINFO
{
    char            szName[40];
    char            szFName[15];
    char            szSCN[15];
    CDib *          pdib;
    FILETIME        ft;
} SAVEGAMEINFO;


class CLoadGameWnd : public CUIDlg
{
public:
    CLoadGameWnd(CUIManager * puimgr);
    virtual ~CLoadGameWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);

    void UIButtonUp(CUIButton * pbutton);
    void UIListboxClick(CUICtrl * pctrl, int iIndex);
    void UIListboxDblClk(CUICtrl * pctrl, int iIndex);

    void UpdateButtons();
    void ActualLoad();

    CUIListbox *            m_pSaveGameList;
    std::vector<SAVEGAMEINFO>    m_vInfo;
};


class CSaveGameWnd : public CUIDlg
{
public:
    CSaveGameWnd(CUIManager * puimgr);
    virtual ~CSaveGameWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);

    void UIButtonUp(CUIButton * pbutton);
    void UIEditboxKey(CUICtrl * pctrl, UINT vk, int cRepeat, UINT flags);
    void UIEditboxMaxText(CUICtrl * pctrl);
    void UIListboxClick(CUICtrl * pctrl, int iIndex);

    void OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags);
    void OnChar(TCHAR ch, int cRepeat);

    void UpdateButtons(BOOL bIgnoreText = FALSE);
    void ActualSave();

    CUIEditbox *            m_pGameName;
    CUIListbox *            m_pSaveGameList;
    std::vector<SAVEGAMEINFO>    m_vInfo;
};


class CRenderWnd : public CUIDlg
{
public:
    CRenderWnd(CUIManager * puimgr);
    virtual ~CRenderWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);

    void UIButtonUp(CUIButton * pbutton);

    void OnOk();
    void Persist();

private:
    void EnterVideoRes();


    GUID            m_guidDefAdapter;
    GUID            m_guidDefD3D;
    int             m_iDefWidth;
    int             m_iDefHeight;
    int             m_iDefQuality;
    int             m_iDefGamma;
    int             m_iDefScreenSize;

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

	void    OnSelectNewDriver();
};


class CHintWnd : public CUIDlg
{
public:
    CHintWnd(CUIManager * puimgr);
    virtual ~CHintWnd();

    BOOL OnCreate();

    void GetWndFile(LPSTR psz, int ic);

    void UIButtonUp(CUIButton * pbutton);
};


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


class CMsgDlg : public CUIDlg
{
public:
    CMsgDlg(CUIManager * puimgr);
    virtual ~CMsgDlg();

    BOOL OnCreate();
    void UIButtonUp(CUIButton * pbutton);

    virtual int JustStatement() { return -1;}
    virtual BOOL GetText(LPSTR pszText, int icLen) { return FALSE; }

    void OnEnter() { EndUIWnd(1); }
    void OnEscape() { EndUIWnd(0); }
};


class CYesNo : public CMsgDlg
{
public:
    CYesNo(CUIManager * puimgr) : CMsgDlg(puimgr) { m_bKeyStyle = KEYSTYLE_ESCAPE;}
    virtual ~CYesNo() {;}

    void GetWndFile(LPSTR psz, int ic)
    {
        strcpy(psz, "yesno.ddf");
    }

    virtual BOOL GetText(LPSTR pszText, int icLen) 
    { 
        return FALSE; 
    }
};


class CYesNoMsgDlg : public CYesNo
{
public:
    CYesNoMsgDlg(int iMsgId, CUIManager * puimgr) : CYesNo(puimgr)
    {
        m_iMsgId = iMsgId;
    }
    
    int JustStatement() { return m_iMsgId;}

private:
    int     m_iMsgId;
};


class CYesNoTextDlg : public CYesNo
{
public:
    CYesNoTextDlg(LPSTR pszMsg, CUIManager * puimgr) : CYesNo(puimgr)
    {
        m_pszMsg = pszMsg;
    }

    virtual BOOL GetText(LPSTR pszText, int icLen)
    {
        if (!m_pszMsg || (int)strlen(m_pszMsg) + 1 > icLen)
        {
            return FALSE;
        }

        strcpy(pszText, m_pszMsg);

        return TRUE;
    }
    
private:
    LPSTR     m_pszMsg;
};


class COKDlg : public CMsgDlg
{
public:
    COKDlg(CUIManager * puimgr) : CMsgDlg(puimgr) { m_bKeyStyle = KEYSTYLE_ESCAPE;}
    virtual ~COKDlg() {;}

    void GetWndFile(LPSTR psz, int ic)
    {
        strcpy(psz, "okdlg.ddf");
    }

    virtual BOOL GetText(LPSTR pszText, int icLen) { return FALSE; }
};



class CMsgTextOkDlg : public COKDlg
{
public:
    CMsgTextOkDlg(LPSTR pszMsg, CUIManager * puimgr) : COKDlg(puimgr)
    {
        m_pszMsg = pszMsg;
    }

    virtual BOOL GetText(LPSTR pszText, int icLen)
    {
        if (!m_pszMsg || (int)strlen(m_pszMsg) + 1 > icLen)
        {
            return FALSE;
        }

        strcpy(pszText, m_pszMsg);

        return TRUE;
    }
    
private:
    LPSTR     m_pszMsg;
};



class CMsgOkDlg : public COKDlg
{
public:
    CMsgOkDlg(int iMsgId, CUIManager * puimgr) : COKDlg(puimgr)
    {
        m_iMsgId = iMsgId;
    }
    
    int JustStatement() { return m_iMsgId;}

private:
    int     m_iMsgId;
};



#endif
