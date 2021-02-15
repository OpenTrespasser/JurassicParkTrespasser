//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1995.
//
//  File:       main.h
//
//  Contents:   Header file for the application.
//
//  Classes:
//
//  Functions:  InitApplication
//              InitInstance
//              About
//              CheckCommandLine
//
//  History:    25-Sep-95   SHernd   Created
//
//---------------------------------------------------------------------------


#ifndef __MAIN_H__
#define __MAIN_H__

#include <vector>

#include "gbuilder.h"

LRESULT CALLBACK About  (HWND, UINT, WPARAM, LPARAM);


typedef struct tagGBUILDINFO
{
    char        szName[50];
    char        szFName[_MAX_PATH];
} GBUILDINFO;


class CMainDlg : public CMultiDlg
{
public:
    CMainDlg();
    ~CMainDlg();

    void        OnSave();
    void        OnSaveAs();
    void        OnOpen();
    BOOL        LoadMRU(int n);

    BOOL        OpenFile(LPCSTR pszFile);
    BOOL        SaveFile(LPCSTR pszFile);

    void        FreeData();

    void        ColumnClick(NM_LISTVIEW * pnmv);
    void        ClickOnItemColumn(int iItem, 
                                  int iColumn, 
                                  BOOL bDoubleClick);

    char                m_szGroffFile[_MAX_PATH];
    std::vector<GBUILDINFO>  m_vGBuildInfo;

    char        m_szSaveName[_MAX_PATH];
    HWND        m_hwndList;

protected:
    void OnClose(HWND hwnd) override;
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) override;
    void OnDestroy(HWND hwnd) override;
    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) override;
    LRESULT OnNotify(HWND hwnd, int idCtrl, LPNMHDR pnmhdr) override;
    void OnParentNotify(HWND hwnd, UINT msg, HWND hwndChild, int idChild) override;
    void OnDropFiles(HWND hwnd, HDROP hdrop) override;
};


class CGroffBuildDlg : public CMultiDlg
{
public:
    CGroffBuildDlg();
    ~CGroffBuildDlg();

    BOOL    HandleInfoMsg(LPSTR pszInfo, int iInfoStyle);

    char            m_szGroffFile[_MAX_PATH];
    std::vector<GBUILD>  m_vGBuild;

protected:
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) override;
    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) override;
};


#endif // __MAIN_H__


