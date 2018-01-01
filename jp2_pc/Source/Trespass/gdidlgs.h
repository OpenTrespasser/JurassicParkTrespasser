//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       gdidlgs.h
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

#ifndef __GDIDLGS_H__
#define __GDIDLGS_H__

#include "gdidlgbase.h"

#define FILEICON_WIDTH	80
#define FILEICON_HEIGHT 60

class CAudioDatabase;
class CSample;

void SetupButtonImage(HWND hwnd, HWND hwndButton, LPCSTR pszImage);

class CGDIMain : public CGDIDlgBase
{
public:
    CGDIMain();
    virtual ~CGDIMain();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnTimer(HWND hwnd, UINT id);

private:
    CAudioDatabase *    m_padbMenu;
    CSample *           m_psamBkgnd;
    CSample *           m_apsamRandoms[14];
    UINT                m_uiRandom;
};


class CControlsDlg : public CGDIDlgBase
{
public:
    CControlsDlg();
    virtual ~CControlsDlg();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
};


class CSoundDlg : public CGDIDlgBase
{
public:
    CSoundDlg();
    virtual ~CSoundDlg();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

    void OnOK();
};


class CHelpDlg : public CGDIDlgBase
{
public:
    CHelpDlg();
    virtual ~CHelpDlg();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
};


class CSaveGameDlg : public CGDIDlgBase
{
private:
	void* pIconData;
public:
    CSaveGameDlg();
    virtual ~CSaveGameDlg();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	void OnOK();
	void SaveLevel(LPCSTR pszName);
};


class CLoadGameDlg : public CGDIDlgBase
{
public:
    CLoadGameDlg();
    virtual ~CLoadGameDlg();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	void OnOK();
	BOOL LoadLevel(LPCSTR pszName);
};

class CDifficultyDlg : public CGDIDlgBase
{
public:
    CDifficultyDlg();
    virtual ~CDifficultyDlg();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
};

class COptionsBase : public CGDIDlgBase
{
public:
    COptionsBase();
    virtual ~COptionsBase();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	void OnGrab();

protected:
    bool        m_bInGame;
};


class COptionsDlg : public COptionsBase
{
public:
    COptionsDlg() { m_bInGame = false;}
    virtual ~COptionsDlg() {;}
};


class CGameOptionsDlg : public COptionsBase
{
public:
    CGameOptionsDlg() { m_bInGame = true;}
    virtual ~CGameOptionsDlg() {;}
};


class CGDINewGameDlg : public CGDIDlgBase
{
public:
    CGDINewGameDlg();
    ~CGDINewGameDlg();

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

    void OnOK();

private:
    BOOL    LoadLevel(LPCSTR pszLevelName);

    HFONT           m_hFont;
};


#endif // __GDIDLGS_H__

