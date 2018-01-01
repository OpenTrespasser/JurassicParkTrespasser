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

#include <vector.h>

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
    vector<GBUILDINFO>  m_vGBuildInfo;

    char        m_szSaveName[_MAX_PATH];
    HWND        m_hwndList;

protected:
    void OnClose(HWND hwnd);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnDestroy(HWND hwnd);
    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    LRESULT OnNotify(HWND hwnd, int idCtrl, LPNMHDR pnmhdr);
    void OnParentNotify(HWND hwnd, UINT msg, HWND hwndChild, int idChild);
    void OnDropFiles(HWND hwnd, HDROP hdrop);
};


class CGroffBuildDlg : public CMultiDlg
{
public:
    CGroffBuildDlg();
    ~CGroffBuildDlg();

    BOOL    HandleInfoMsg(LPSTR pszInfo, int iInfoStyle);

    char            m_szGroffFile[_MAX_PATH];
    vector<GBUILD>  m_vGBuild;

protected:
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
};


#endif // __MAIN_H__


