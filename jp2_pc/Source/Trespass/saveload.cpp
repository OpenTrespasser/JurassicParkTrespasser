//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1996.
//
//  File:       uidlgs.cpp
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    23-Nov-96   SHernd   Created
//
//---------------------------------------------------------------------------


#include "precomp.h"
#pragma hdrstop

#include "tpassglobals.h"
#include "supportfn.hpp"
#include "resource.h"
#include "dialogs.h"
#include "main.h"
#include "uiwnd.h"
#include "uidlgs.h"
#include "Cdib.h"
#include "../Lib/Sys/reg.h"
#include "../Lib/Sys/RegInit.hpp"


extern HINSTANCE    g_hInst;
extern HWND		    g_hwnd;


//+--------------------------------------------------------------------------
//
//  Function:   AddToLoadGameVector
//
//  Synopsis:   Basic Insertion sort.  Most recent files first in the list.
//
//  Arguments:  [psgi]  -- item to insert
//              [pInfo] -- List to insert into
//
//  Returns:    void
//
//  History:    10-Jul-98    SHernd  Created
//
//---------------------------------------------------------------------------
void AddToLoadGameVector(SAVEGAMEINFO * psgi, std::vector<SAVEGAMEINFO> * pInfo)
{
    bool                            fEntered;
    std::vector<SAVEGAMEINFO>::iterator  ppinfo;

    for (ppinfo = pInfo->begin(), fEntered = false; 
         ppinfo != pInfo->end(); 
         ppinfo++)
    {
        if (CompareFileTime(&ppinfo->ft, &psgi->ft) == -1)
        {
            fEntered = true;
            pInfo->insert(ppinfo, *psgi);
            break;
        }
    }

    if (!fEntered)
    {
        pInfo->push_back(*psgi);
    }
}



BOOL ExtractDataFromSaveFile(LPSTR pszFile, SAVEGAMEINFO * psgi)
{
    CSaveFile   sf(pszFile, true, true);	
    int         iSize;
    const char *pc;
    MEMDIBINFO  memdib;

	if (!sf.bValidFile)
    {
        TraceError(("ExtractDataFromSaveFile() -- Invalid File %s",
                    pszFile));
		return FALSE;
    }

    // Read in the save name
    pc = sf.pcLoad("SaveGameName", &iSize);
    strcpy(psgi->szName, pc);

    // Read in the original SCN name
    pc = sf.pcLoad("SaveSCNName", &iSize);
    strcpy(psgi->szSCN, pc);

    // Read in the bmp
    pc = sf.pcLoad("SaveBMP", &iSize);
    memdib.pbBase = (LPBYTE)pc;
    memdib.pbCurr = (LPBYTE)pc;
    memdib.dwSize = (DWORD)iSize;

    psgi->pdib = new CDib();
    Assert(psgi->pdib);
    psgi->pdib->LoadFromStream(&memdib, 
                               (PFNDIBREAD)MemDibRead, 
                               (PFNDIBSEEK)MemDibSeek);

    return TRUE;
}

//+--------------------------------------------------------------------------
//
//  Function:   EnterSavedGamesIntoLoadGameVector
//
//  Synopsis:   Looks in the installed directory for the SAVEGAME.* file type
//              and then tries to add them all to the Load Game Vector
//
//  Arguments:  [pInfo] -- Vector to enter the files into
//
//  Returns:    BOOL
//
//  History:    01-Jun-97    SHernd  Created
//
//---------------------------------------------------------------------------
BOOL EnterSavedGamesIntoLoadGameVector(std::vector<SAVEGAMEINFO> * pInfo)
{
    char                szPath[_MAX_PATH];
    char                szBase[_MAX_PATH];
    char                szSavePrefix[_MAX_PATH];
    HANDLE              hfind;       
    WIN32_FIND_DATA     finddata;
    SAVEGAMEINFO        sgi;

    LoadString(g_hInst, 
               IDS_SAVEGAMEPREFIX, 
               szSavePrefix, 
               sizeof(szSavePrefix));

    GetFileLoc(FA_INSTALLDIR, szBase, sizeof(szBase));

    // Save Off Base path
    strcpy(szPath, szBase);

    strcat(szBase, szSavePrefix);
    strcat(szBase, ".*");

    hfind = FindFirstFile(szBase, &finddata);
    if (hfind == INVALID_HANDLE_VALUE)
    {
        // No files found so back out.
        return TRUE;
    }

    do
    {
        // Create the full path for this.
        if (finddata.ftLastWriteTime.dwLowDateTime != 0 &&
            finddata.ftLastWriteTime.dwHighDateTime != 0)
        {
            sgi.ft = finddata.ftLastWriteTime;
        }
        else
        {
            sgi.ft = finddata.ftCreationTime;
        }

        strcpy(sgi.szFName, finddata.cFileName);

        wsprintf(szBase, "%s%s", szPath, finddata.cFileName);

        strcpy(sgi.szFName, finddata.cFileName);

        ExtractDataFromSaveFile(szBase, &sgi);
        AddToLoadGameVector(&sgi, pInfo);
    }
    while (FindNextFile(hfind, &finddata));

    FindClose(hfind);
    return TRUE;
}


void FindSavedGames(CUIListbox * plist, std::vector<SAVEGAMEINFO> * psgi)
{
    std::vector<SAVEGAMEINFO>::iterator  ppinfo;

    EnterSavedGamesIntoLoadGameVector(psgi);

    for (ppinfo = psgi->begin(); ppinfo != psgi->end(); ppinfo++)
    {
        plist->AddItem(ppinfo->szName, (DWORD)&(*ppinfo), -1, 0);
    }
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CLoadGameWnd::CLoadGameWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
}

CLoadGameWnd::~CLoadGameWnd()
{
    std::vector<SAVEGAMEINFO>::iterator  ppinfo;

    for (ppinfo = m_vInfo.begin(); ppinfo != m_vInfo.end(); ppinfo++)
    {
        delete ppinfo->pdib;
    }
}


BOOL CLoadGameWnd::OnCreate()
{
    if (!CUIDlg::OnCreate())
    {
        TraceError(("CLoadGameWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    m_pSaveGameList = (CUIListbox *)GetUICtrl(1002);
    FindSavedGames(m_pSaveGameList, &m_vInfo);

    CenterUIWindow(this);

    return TRUE;
}


void CLoadGameWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "loadgame.ddf");
}


void CLoadGameWnd::UIButtonUp(CUIButton * pbutton)
{
    switch (pbutton->GetID())
    {
        // Controls

        // Cancel
        case 1000:
            EndUIWnd(0);
            break;

        // OK
        case 1001:
            ActualLoad();
            break;

        // Delete
        case 1004:
            {
                char            szExt[100];
                char            szMsg[250];
                char            szFName[50];
                DWORD           dwParam;
                WORD            wFlags;
                int             i;
                SAVEGAMEINFO    sgi;

                i = m_pSaveGameList->GetCurrSel();
                Assert(i != -1);

                m_pSaveGameList->GetItem(szFName, sizeof(szFName), dwParam, wFlags, i);

                // Ask the user if they want to delete this savegame.
                LoadString(g_hInst, IDS_DELETE_SAVEGAME, szExt, sizeof(szExt));
                wsprintf(szMsg, szExt, szFName);

                CYesNoTextDlg   dlg(szMsg, m_pUIMgr);

                dlg.StartUIWnd();
                if (dlg.m_dwExitValue == 0)
                {
                    return;
                }

                // Delete Save Game
                m_pSaveGameList->RemoveItem(i);
                sgi = m_vInfo[i];
                m_vInfo.erase(m_vInfo.begin() + i);
                g_CTPassGlobals.DeleteGame(sgi.szFName);

                i = m_pSaveGameList->GetCurrSel();
                if (i == -1)
                {
                    ((CUIStatic*)GetUICtrl(1003))->SetRaster(NULL);
                    GetUICtrl(1004)->SetActive(FALSE);
                    GetUICtrl(1001)->SetActive(FALSE);
                }
                else
                {
                    UIListboxClick(m_pSaveGameList, i);
                }
            }
            break;
    }
}


void CLoadGameWnd::UIListboxClick(CUICtrl * pctrl, int iIndex)
{
    CRasterDC *     pras;
    char            sz[255];
    DWORD           dwParam;
    WORD            wFlags;
    CUIStatic *     pstatic;
    SAVEGAMEINFO*   ppinfo;

    if (iIndex == -1)
    {
        return;
    }

    // Activate the OK button
    GetUICtrl(1001)->SetActive(TRUE);

    // Activate the Delete Button
    GetUICtrl(1004)->SetActive(TRUE);

    // Load in the correct BMP to put in the static control
    m_pSaveGameList->GetItem(sz, sizeof(sz), dwParam, wFlags, iIndex);
    ppinfo = (SAVEGAMEINFO*)dwParam;

    pras = (CRasterDC *)ConvertBMP(ppinfo->pdib, true);

    pstatic = (CUIStatic *)GetUICtrl(1003);
    pstatic->SetRaster(pras, FALSE);
}


void CLoadGameWnd::UIListboxDblClk(CUICtrl * pctrl, int iIndex)
{
    if (iIndex == -1)
    {
        return;
    }

    ActualLoad();
}


void CLoadGameWnd::UpdateButtons()
{
}


void CLoadGameWnd::ActualLoad()
{
    char            szDir[_MAX_PATH];
    int             i;

    i = m_pSaveGameList->GetCurrSel();
    GetFileLoc(FA_INSTALLDIR, szDir, sizeof(szDir));
    strcat(szDir, m_vInfo[i].szFName);

    g_CTPassGlobals.FreeMenuAudio();
    i = g_CTPassGlobals.LoadScene(szDir, m_vInfo[i].szSCN);

    if (i < 0)
    {
        DisplayLoadingFileError(i);
    }
    else
    {
        EndUIWnd(1);
    }
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


CSaveGameWnd::CSaveGameWnd(CUIManager * puimgr) : CUIDlg(puimgr)
{
}

CSaveGameWnd::~CSaveGameWnd()
{
    std::vector<SAVEGAMEINFO>::iterator  ppinfo;

    for (ppinfo = m_vInfo.begin(); ppinfo != m_vInfo.end(); ppinfo++)
    {
        delete ppinfo->pdib;
    }
}


BOOL CSaveGameWnd::OnCreate()
{
    char            sz[50];

    if (!CUIDlg::OnCreate())
    {
        TraceError(("CSaveGameWnd::OnCreate() -- base create failed"));
        return FALSE;
    }

    m_pSaveGameList = (CUIListbox *)GetUICtrl(1002);
    Assert(m_pSaveGameList);
    LoadString(g_hInst, IDS_NEWSAVE, sz, sizeof(sz));
    m_pSaveGameList->AddItem(sz, 0, -1, 0);
    m_pSaveGameList->SetCurrSel(0);

    FindSavedGames(m_pSaveGameList, &m_vInfo);

    m_pGameName = (CUIEditbox *)GetUICtrl(1004);
    m_pGameName->SetMaxCharNum(21);

    UpdateButtons();

    CenterUIWindow(this);

    return TRUE;
}


void CSaveGameWnd::GetWndFile(LPSTR psz, int ic)
{
    strcpy(psz, "savegame.ddf");
}


void CSaveGameWnd::UIButtonUp(CUIButton * pbutton)
{
    switch (pbutton->GetID())
    {
        // Controls
        case 1000:
            //cancel
            EndUIWnd(0);
            break;

        case 1001:
            ActualSave();
            break;

        // Delete
        case 1005:
            {
                char            szExt[100];
                char            szMsg[250];
                char            szFName[50];
                DWORD           dwParam;
                WORD            wFlags;
                int             i;
                SAVEGAMEINFO    sgi;

                i = m_pSaveGameList->GetCurrSel();
                Assert(i != -1);

                m_pSaveGameList->GetItem(szFName, sizeof(szFName), dwParam, wFlags, i);

                // Ask the user if they want to delete this savegame.
                LoadString(g_hInst, IDS_DELETE_SAVEGAME, szExt, sizeof(szExt));
                wsprintf(szMsg, szExt, szFName);

                CYesNoTextDlg   dlg(szMsg, m_pUIMgr);

                dlg.StartUIWnd();
                if (dlg.m_dwExitValue == 0)
                {
                    return;
                }

                // Delete Save Game
                m_pSaveGameList->RemoveItem(i);
                sgi = m_vInfo[i - 1];
                m_vInfo.erase(m_vInfo.begin() + (i - 1));
                g_CTPassGlobals.DeleteGame(sgi.szFName);

                i = m_pSaveGameList->GetCurrSel();
                if (i == -1)
                {
                    ((CUIStatic*)GetUICtrl(1003))->SetRaster(NULL);
                    GetUICtrl(1004)->SetActive(FALSE);
                    GetUICtrl(1001)->SetActive(FALSE);
                }
                else
                {
                    UIListboxClick(m_pSaveGameList, i);
                }

                UpdateButtons();
            }
            break;
    }
}


void CSaveGameWnd::UIEditboxKey(CUICtrl * pctrl, UINT vk, int cRepeat, UINT flags)
{
    if (vk == VK_ESCAPE)
    {
        EndUIWnd(0);
    }
    else if (vk == VK_RETURN)
    {
        ActualSave();
    }
}


void CSaveGameWnd::UIEditboxMaxText(CUICtrl * pctrl)
{
    g_CTPassGlobals.PlayButtonAudio();
}


void CSaveGameWnd::OnKey(UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    m_pGameName->OnKey(vk, fDown, cRepeat, flags);
    UpdateButtons(TRUE);
}

void CSaveGameWnd::OnChar(TCHAR ch, int cRepeat)
{
    if (ch < 32 || ch > 126)
    {
        return;
    }

    m_pGameName->OnChar(ch, cRepeat);
    UpdateButtons(TRUE);
}


void CSaveGameWnd:: UIListboxClick(CUICtrl * pctrl, int iIndex)
{
    CRasterDC *     pras;
    char            sz[255];
    DWORD           dwParam;
    WORD            wFlags;
    CUIStatic *     pstatic;
    SAVEGAMEINFO*   ppinfo;

    pstatic = (CUIStatic *)GetUICtrl(1003);

    if (iIndex == 0)
    {
        pstatic->SetRaster(NULL);
        UpdateButtons();
        return;
    }

    // Load in the correct BMP to put in the static control
    m_pSaveGameList->GetItem(sz, sizeof(sz), dwParam, wFlags, iIndex);
    ppinfo = (SAVEGAMEINFO*)dwParam;

    pras = (CRasterDC *)ConvertBMP(ppinfo->pdib, true);

    pstatic = (CUIStatic *)GetUICtrl(1003);
    pstatic->SetRaster(pras, FALSE);

    UpdateButtons();
}


void CSaveGameWnd::ActualSave()
{
    int             i;
    char            szExt[255];
    char            szMsg[255];
    char            szFName[_MAX_PATH];
    LPCSTR          psz;
    DWORD           dwParam;
    WORD            wFlags;
    BOOL            bValidName = FALSE;
    int             iLen;
    SAVEGAMEINFO *  psgi;
    LPSTR           pszExt;
    DWORD           dwSectorsPerCluster;
    DWORD           dwBytesPerSector;
    DWORD           dwFreeClusters;
    DWORD           dwTotalClusters;

    psz = m_pGameName->GetText();
    if (psz)
    {
        iLen = strlen(psz);
        if (!iLen)
            return;

        //lets not let the user create names that are all white space
        for (i = 0; i < iLen; i++)
        {
            if (!isspace(psz[i]))
                bValidName = TRUE;
        }

        if (!bValidName)
            return;

        // Check for available disk space
        GetFileLoc(FA_INSTALLDIR, szFName, sizeof(szFName));
        _splitpath(szFName, szExt, NULL,NULL, NULL);
        strcat(szExt, "\\");
        bValidName = GetDiskFreeSpace(szExt,
                                      &dwSectorsPerCluster,
                                      &dwBytesPerSector,
                                      &dwFreeClusters,
                                      &dwTotalClusters);
        if (!bValidName)
        {
            Trace(("CSaveGameWnd::ActualSave() -- GetFreeDiskSpace error %i", 
                   GetLastError()));
        }

        // Check for enough free space for a save
        if (dwSectorsPerCluster * dwBytesPerSector * dwFreeClusters < 1024 * 1024)
        {
            CMsgOkDlg   dlg(IDS_ERROR_DISKSPACE_SAVE, m_pUIMgr);

            dlg.StartUIWnd();
            return;
        }


        i = m_pSaveGameList->GetCurrSel();
        if (i != 0)
        {
            m_pSaveGameList->GetItem(szFName, sizeof(szFName), dwParam, wFlags, i);

            // Ask the user if they want to overwrite this savegame.
            LoadString(g_hInst, IDS_OVERWRITE_SAVEGAME, szExt, sizeof(szExt));
            wsprintf(szMsg, szExt, szFName);

            CYesNoTextDlg   dlg(szMsg, m_pUIMgr);

            dlg.StartUIWnd();
            if (dlg.m_dwExitValue == 0)
            {
                return;
            }

            psgi = (SAVEGAMEINFO*)dwParam;

            pszExt = strchr(psgi->szFName, '.');
            Assert(pszExt);
            pszExt++;
            i = atoi(pszExt);
        }
        else
        {
            i = -1;
        }

        g_CTPassGlobals.SaveGame(psz, i);
        EndUIWnd(1);
    }
}


void CSaveGameWnd::UpdateButtons(BOOL bIgnoreText)
{
    LPCSTR          psz;
    int             i;
    char            sz[40];
    DWORD           dwParam;
    WORD            wFlags;

    if (!bIgnoreText)
    {
        i = m_pSaveGameList->GetCurrSel();

        if (i == 0)
        {
            GetUICtrl(1005)->SetActive(FALSE);
            m_pGameName->SetText("");
        }
        else
        {
            GetUICtrl(1005)->SetActive(TRUE);
            m_pSaveGameList->GetItem(sz, sizeof(sz), dwParam, wFlags, i);
            m_pGameName->SetText(sz);
        }
    }

    psz = m_pGameName->GetText();
    if (psz && (int)strlen(psz) > 0 )
    {
        GetUICtrl(1001)->SetActive(TRUE);
    }
    else
    {
        GetUICtrl(1001)->SetActive(FALSE);
    }
}


