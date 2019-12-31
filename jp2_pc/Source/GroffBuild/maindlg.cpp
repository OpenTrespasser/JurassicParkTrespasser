//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       maindlg.cpp
//
//  Contents:   Main Dialog for the Groff Builder Application.
//
//  Classes:    CMainDlg
//
//  Functions:
//
//  History:    02-Feb-98   SHernd   Created
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include <limits.h>
#include "resource.h"
#include "dialogs.h"
#include "main.h"
#include "utils.h"
#include "reg.h"

extern HINSTANCE    g_hInst;

void DeleteMRU (HWND hWnd, int nDel);
void SetMRU (HWND hWnd);
int AddMRU (HWND hWnd, LPCSTR lpszFile);
BOOL LoadMRU (HWND hWnd, int n);


BOOL FAR PASCAL GeneratorPump()
{
	MSG msg;

	while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

    return TRUE;
}


BOOL FAR PASCAL GeneratorInfo(LPSTR pszInfo, int iInfoStyle, LPARAM lParam)
{
    CGroffBuildDlg *    pgbd;

    pgbd = (CGroffBuildDlg *)lParam;

    if (!pgbd)
    {
        return TRUE;
    }

    return pgbd->HandleInfoMsg(pszInfo, iInfoStyle);
}


void Size2String(DWORD dwSize, LPSTR pszDst)
{
    char        szType[3][10] = { "Bytes",
                                  "KB",
                                  "MB" };
    int         iType;

    if ((dwSize / 1024) < 50) // less than 50K
    {
        iType = 0;
    }
    else if ((dwSize / (1024 * 1024)) < 2) // Less than 2MB
    {
        iType = 1;
        dwSize /= 1024;
    }
    else
    {
        iType = 2;
        dwSize /= (1024 * 1024);
    }

    wsprintf(pszDst, "%i %s", dwSize, szType[iType]);
}


#if 0
void AppendFile(HWND hwndCtl, FINFO * pfi, int i)
{
    char        sz[255];
    int         iItem;
    LV_ITEM     lvi;

    sz[0] = 0;
    if (pfi)
    {
        wsprintf(sz, "%s (%i KB)", pfi->szFile, pfi->dwSize / 1024);
    }

    memset(&lvi, 0, sizeof(lvi));
    lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
    lvi.pszText = sz;
    lvi.lParam = (LPARAM)pfi;
    lvi.iImage = 0;

    if (i == -1)
    {
        i = ListView_GetItemCount(hwndCtl);
    }

    lvi.iItem = i;

    iItem = ListView_InsertItem(hwndCtl, &lvi);
}
#endif


void DeleteMRU (HWND hWnd, int nDel)
{
	int i;
	int nStart;
	char sz[15];
	char szFile[_MAX_PATH];

	nStart = GetRegValue ("MRU Num", 0);
	i	   = nDel;
	while ((i+1)%4 != nStart)
	{
		wsprintf (sz, "MRUPRJ%d", (i+1)%4);
		GetRegString (sz, szFile, sizeof (szFile), "");
		wsprintf (sz, "MRUPRJ%d", i);
		SetRegString (sz, szFile);
		i = (i+1)%4;
	} 

	wsprintf (sz, "MRUPRJ%d", i);
	SetRegString (sz, "");
}


void SetMRU (HWND hWnd)
{
	HMENU hMenu;
	int   nStart;
	int   i;
	char  sz[15];
	char  szFile[_MAX_PATH+4];

	hMenu = GetMenu (hWnd);
	nStart = GetRegValue ("MRU Num", 0);

	for (i=0; i < 4; i++)
	{
		wsprintf (sz, "MRUPRJ%d", nStart);
		nStart++;
		if (nStart == 4) nStart = 0;

		wsprintf (szFile, "&%1d ",i+1);
		GetRegString (sz, szFile+3, sizeof (szFile)-3, "");
		ModifyMenu (hMenu, ID_FILE_MRU1+i, MF_BYCOMMAND|MF_STRING,
			ID_FILE_MRU1+i, szFile);
	}

	DrawMenuBar(hWnd);
}



int AddMRU (HWND hWnd, LPCSTR lpszFile)
{
	int nStart;
	int nOriginal;
	char sz[15];
	char szFile[_MAX_PATH];

	nOriginal = GetRegValue ("MRU Num", 0);

	// nothing to add
	if (*lpszFile=='\0') return nOriginal;


	// first, check if we are already there.
	for (nStart=0; nStart<4; nStart++)
	{
		wsprintf (sz, "MRUPRJ%d", nStart);
		GetRegString (sz, szFile, sizeof (szFile), "");
		if (lstrcmpi(szFile, lpszFile)==0)
			DeleteMRU (hWnd, nStart);
	}

	
	// now, add it.
	nStart = nOriginal-1;
	if (nStart<0) nStart = 3;
	wsprintf (sz, "MRUPRJ%d", nStart);
	SetRegString (sz, lpszFile);
	SetRegValue ("MRU Num", nStart);

	SetMRU (hWnd);

	return nOriginal;
}


CMainDlg::CMainDlg()
{
    m_szGroffFile[0] = '\0';
    OpenKey();
}



CMainDlg::~CMainDlg()
{
    CloseKey();
}



void CMainDlg::OnDestroy(HWND hwnd)
{
    FreeData();

    CMultiDlg::OnDestroy(hwnd);

    PostQuitMessage(0);
}



void CMainDlg::FreeData()
{
}



BOOL CMainDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    LV_COLUMN       lvc;
    RECT            rc;
    LONG            lStyle;

    DragAcceptFiles(m_hwnd, TRUE);

    m_hwndList = GetDlgItem(hwnd, IDC_FILELIST);
    lStyle = GetWindowLong(m_hwndList, GWL_EXSTYLE);
    lStyle &= ~WS_EX_NOPARENTNOTIFY;
    SetWindowLong(m_hwndList, GWL_EXSTYLE, lStyle);
    GetClientRect(m_hwndList, &rc);

    memset(&lvc, 0, sizeof(lvc));
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = 100;
    lvc.pszText = "Groff Name";
    ListView_InsertColumn(m_hwndList, 0, &lvc);

    memset(&lvc, 0, sizeof(lvc));
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = 200;
    lvc.pszText = "File Name";
    ListView_InsertColumn(m_hwndList, 1, &lvc);

    SetMRU(m_hwnd);

    return TRUE;
}



void CMainDlg::OnParentNotify(HWND hwnd, 
                              UINT msg, 
                              HWND hwndChild, 
                              int idChild)
{
    switch (msg)
    {
        case WM_LBUTTONDOWN:
            POINT           pt;
            int             iItem;
            int             i;
            int             iTop;
            int             icPage;
            int             icMax;
            int             iColumn;
            int             iWidth;
            BOOL            bFound;
            BOOL            bErr;
            RECT            rc;
            int             x;
            int             y;

            pt.x = LOWORD(hwndChild);
            pt.y = HIWORD(hwndChild);

            hwndChild = ChildWindowFromPoint(m_hwnd, pt);
            if (hwndChild != GetDlgItem(m_hwnd, IDC_FILELIST))
            {
                break;
            }

            icMax = ListView_GetItemCount(hwndChild);
            if (icMax == 0)
            {
                break;
            }

            iTop = ListView_GetTopIndex(hwndChild);
            icPage = ListView_GetCountPerPage(hwndChild);

            MapWindowPoints(m_hwnd, hwndChild, &pt, 1);
            x = pt.x;
            y = pt.y;

            for (i = iTop, bFound = FALSE; 
                 (i < (iTop + icPage + 1)) && (i < icMax) && !bFound; 
                 i++)
            {
                ListView_GetItemRect(hwndChild, i, &rc, LVIR_BOUNDS);

                if ((rc.top <= y) && (rc.bottom >= y))
                {
                    iItem = i;
                    bFound = TRUE;
                }
            }

            if (!bFound)
            {
                break;
            }

            ListView_GetOrigin(hwndChild, &pt);
            // Find Column Clicked On
            for (i = 0, iTop = 0, bFound = FALSE, bErr = FALSE, iColumn = -1; 
                 !bFound && !bErr; 
                 i++)
            {
                iWidth = ListView_GetColumnWidth(hwndChild, i);
                if (iWidth == 0)
                {
                    bErr = TRUE;
                }

                iTop += iWidth;

                if (x <= iTop)
                {
                    bFound = TRUE;
                    iColumn = i;
                }
            }

            if (bFound)
            {
                ClickOnItemColumn(iItem, iColumn, FALSE);
            }
            break;
    }
}



void CMainDlg::ClickOnItemColumn(int iItem, 
                                 int iColumn, 
                                 BOOL bDoubleClick)
{
}



#if 0
FINFO * CMainDlg::FindFINFO(LPSTR pszName)
{
    int         i;
    FINFO *  pfi = NULL;
    BOOL        bFound = FALSE;

    if (m_pVFiles == NULL)
    {
        return NULL;
    }

    for (i = 0; i < m_pVFiles->GetUsed() && !bFound; i++)
    {
        pfi = m_pVFiles->GetItem(i);
        Assert(pfi != NULL);

        if (strcmpi(pszName, pfi->szFile) == 0)
        {
            bFound = TRUE;
            break;
        }
    }

    if (!bFound)
    {
        pfi = NULL;
    }

    return pfi;
}
#endif


void CMainDlg::OnSave()
{
    if (strlen(m_szSaveName) == 0)
    {
        OnSaveAs();
        return;
    }

    SaveFile(m_szSaveName);
    AddMRU(m_hwnd, m_szSaveName);
}

void CMainDlg::OnSaveAs()
{
    OPENFILENAME        ofn;
    char                szFilter[] = {"MultiIndex (*.MDF)\0*.MDF\0"
                                      "All Files (*.*)\0*.*\0\0"};
    char                szFile[_MAX_PATH];

    strcpy(szFile, m_szSaveName);
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hwnd;
    ofn.hInstance = g_hInst;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrDefExt = "MDF";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

    if (!GetSaveFileName(&ofn))
    {
#ifdef _DEBUG
        DWORD dw = CommDlgExtendedError();
#endif
        return;
    }
    
    strcpy(m_szSaveName, szFile);

    OnSave();
}


BOOL CMainDlg::LoadMRU(int n)
{
	int  nStart;
	char sz[15];

	nStart = GetRegValue ("MRU Num", 0);
	nStart = (nStart+n) % 4;
	wsprintf (sz, "MRUPRJ%d", nStart);
	GetRegString (sz, m_szSaveName, sizeof (m_szSaveName), "");
	DeleteMRU(m_hwnd, nStart);

	AddMRU(m_hwnd, m_szSaveName);

	OpenFile(m_szSaveName);

	return TRUE;
}


void CMainDlg::OnOpen()
{
    OPENFILENAME        ofn;
    char                szFilter[] = {"MultiIndex (*.MDF)\0*.MDF\0"
                                      "All Files (*.*)\0*.*\0\0"};
    char                szFile[_MAX_PATH];

    szFile[0] = '\0';
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hwnd;
    ofn.hInstance = g_hInst;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrDefExt = "MDF";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

    if (!GetOpenFileName(&ofn))
    {
#ifdef _DEBUG
        DWORD dw = CommDlgExtendedError();
#endif
        return;
    }
    
    strcpy(m_szSaveName, szFile);
    OpenFile(m_szSaveName);
    AddMRU(m_hwnd, m_szSaveName);
}



void CMainDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
        case ID_FILE_MRU1:
        case ID_FILE_MRU2:
        case ID_FILE_MRU3:
        case ID_FILE_MRU4:
            LoadMRU(id - ID_FILE_MRU1);
            break;

        case IDM_EXIT:
            DestroyWindow(m_hwnd);
            break;

        case IDM_SAVE:
            OnSave();
            break;

        case IDM_SAVEAS:
            OnSaveAs();
            break;

        case IDM_OPEN:
            OnOpen();
            break;


        case IDC_GROFFNAME:
            {
                OPENFILENAME        ofn;
                char                szFilter[] = {"Groff (*.GRF)\0*.GRF\0"
                                                  "All Files (*.*)\0*.*\0\0"};
                char                szFile[_MAX_PATH];

                strcpy(szFile, m_szGroffFile);
                memset(&ofn, 0, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = m_hwnd;
                ofn.hInstance = g_hInst;
                ofn.lpstrFilter = szFilter;
                ofn.lpstrDefExt = "GRF";
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(szFile);
                ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;

                if (!GetSaveFileName(&ofn))
                {
#ifdef _DEBUG
                    DWORD dw = CommDlgExtendedError();
#endif
                    return;
                }
    
                strcpy(m_szGroffFile, szFile);
                SetDlgItemText(m_hwnd, IDC_NAME, szFile);
            }
            break;

        case IDM_GENERATE:
            {
                CGroffBuildDlg  dlg;
                GBUILD          gb;

                if (strlen(m_szGroffFile) == 0)
                {
                    MessageBox(m_hwnd,
                               "Destination Groff Not chosen",
                               "Error",
                               MB_OK);
                    return;
                }


                //
                // Setup the appropriate pointer information
                // 

                std::vector<GBUILDINFO>::iterator    i;

                for (i = m_vGBuildInfo.begin(); 
                     i != m_vGBuildInfo.end(); 
                     i++)
                {
                    gb.pszSection = &(*i->szName);
                    gb.pszSrcName = &(*i->szFName);

                    dlg.m_vGBuild.push_back(gb);
                }

                strcpy(dlg.m_szGroffFile, m_szGroffFile);     

                dlg.MultiDialogBox(g_hInst,
                                   MAKEINTRESOURCE(IDD_GROFFBUILD),
                                   m_hwnd);
            }
            break;
    }
}


void CMainDlg::ColumnClick(NM_LISTVIEW * pnmv)
{
}



LRESULT CMainDlg::OnNotify(HWND hwnd, int idCtrl, LPNMHDR pnmhdr)
{
    switch (idCtrl)
    {
        case IDC_FILELIST:
            switch (pnmhdr->code)
            {
                case LVN_COLUMNCLICK:
                    ColumnClick((NM_LISTVIEW *)pnmhdr);
                    break;
            }
            break;
    }

    return CMultiDlg::OnNotify(hwnd, idCtrl, pnmhdr);
}


void CMainDlg::OnClose(HWND hwnd)
{
    AddMRU(m_hwnd, m_szSaveName);
    DestroyWindow(m_hwnd);
}



BOOL CMainDlg::OpenFile(LPCSTR pszFile)
{
    return FALSE;
}


BOOL CMainDlg::SaveFile(LPCSTR pszFile)
{
    return FALSE;
}



void CMainDlg::OnDropFiles(HWND hwnd, HDROP hdrop)
{
    UINT            ui;
    char            szMaxPath[_MAX_PATH];
    char            szExt[_MAX_PATH];
    char            szName[_MAX_PATH];
    UINT            uiMax;
    int             i;
    int             iItem;
    LVITEM          lvi;
    GBUILDINFO      ginfo;

    uiMax = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

    ZeroMemory(&lvi, sizeof(lvi));

    for (ui = 1; ui <= uiMax; ui++)
    {
        DragQueryFile(hdrop, uiMax - ui, szMaxPath, sizeof(szMaxPath));

        _splitpath(szMaxPath, NULL, NULL, szName, szExt);

        strcpy(ginfo.szName, szName);
        strcpy(ginfo.szFName, szMaxPath);

        m_vGBuildInfo.push_back(ginfo);
        iItem = m_vGBuildInfo.size();

        i = ListView_GetItemCount(m_hwndList);

        lvi.mask = LVIF_TEXT | LVIF_PARAM;
        lvi.pszText = szName;
        lvi.lParam = (LPARAM)iItem - 1;
        lvi.iItem = i;
        lvi.iSubItem = 0;

        ListView_InsertItem(m_hwndList, &lvi);

        lvi.mask = LVIF_TEXT;
        lvi.pszText = szMaxPath;
        lvi.iSubItem = 1;
        ListView_SetItem(m_hwndList, &lvi);
    }

    if (uiMax > 0)
    {
        EnableMenuItem(GetMenu(m_hwnd), 1, MF_BYPOSITION | MF_ENABLED);
        DrawMenuBar(m_hwnd);
    }

    DragFinish(hdrop);

    ShowWindow(m_hwnd, SW_SHOWNORMAL);
}


//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



CGroffBuildDlg::CGroffBuildDlg()
{
}


CGroffBuildDlg::~CGroffBuildDlg()
{
}


BOOL CGroffBuildDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    FORWARD_WM_COMMAND(m_hwnd, IDM_START, NULL, 0, PostMessage);

    return TRUE;
}


void CGroffBuildDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
        default:
            CMultiDlg::OnCommand(hwnd, id, hwndCtl, codeNotify);
            break;

        case IDM_START:
            UpdateWindow(hwnd);
            GBuild_SetupCallbacks(GeneratorPump, GeneratorInfo, (LPARAM)this);

            GBuild_Build(m_szGroffFile, m_vGBuild.size(), m_vGBuild.data());

            EnableWindow(GetDlgItem(m_hwnd, IDOK), TRUE);
            break;
    }
}


BOOL CGroffBuildDlg::HandleInfoMsg(LPSTR pszInfo, int iInfoStyle)
{
    HWND        hwndList;
    int         i;

    hwndList = GetDlgItem(m_hwnd, IDC_INFOLIST);

    i = ListBox_AddString(hwndList, pszInfo);
    ListBox_SetTopIndex(hwndList, i);


    return TRUE;
}


