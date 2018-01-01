//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1998.
//
//  File:       main.cpp
//
//  Contents:   Main file for the Groff Builder Application.
//
//  Classes:    CMainWnd
//
//  Functions:
//              About
//              CheckCommandLine
//              InitApplication
//              InitInstance
//              ReadApplicationInfo
//              WinMain
//
//  History:    02-Feb-98   SHernd   Created
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include "resource.h"
#include "dialogs.h"
#include "main.h"

BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);


//+--------------------------------------------------------------------------
//-------------------------       Globals      ------------------------------
//---------------------------------------------------------------------------

HINSTANCE   g_hInst;
HWND        g_hwnd;
char        g_szAppName[] = "DWI - Groff Build";
char        g_szTitle[255];
CMainDlg *  g_pMainDlg;


bool        bUseOutputFiles = false;
bool        bUseReplayFile = false;


//+--------------------------------------------------------------------------
//
//  Function:   WinMain
//
//  Synopsis:   Entry point function for this application
//
//  Arguments:  [hInstance]     -- Instance handle for this application
//              [hPrevInstance] -- previous instance handle
//              [lpCmdLine]     -- command line parameters
//              [nCmdShow]      -- how to display the window when created
//
//  Returns:    int -- Return from Quit Message
//
//  History:    25-Sep-95   SHernd   Created
//
//---------------------------------------------------------------------------
int WINAPI
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
    MSG         msg;
    HANDLE      hAccelTable;
    int         iRet;
    CMainDlg    cdlgMain;


    g_hInst = hInstance;
    g_pMainDlg = &cdlgMain;

    InitCommonControls();

    if (!hPrevInstance)
    {
        if (!InitApplication(hInstance))
        {
            goto Error;
        }
    }

    if (!InitInstance(hInstance, nCmdShow))
    {
        goto Error;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_GENERIC));

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!IsDialogMessage(g_hwnd, &msg) && 
            !TranslateAccelerator (msg.hwnd, (HACCEL)hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    iRet = msg.wParam;

Cleanup:

    return iRet;

Error:
    iRet = 0;
    goto Cleanup;
}



//+--------------------------------------------------------------------------
//
//  Function:   InitApplication
//
//  Synopsis:   Registers the window classes necessary for the application
//              to run
//
//  Arguments:  [hInstance] -- instance that we are registering the classes
//
//  Returns:    BOOL -- TRUE  -- if successful
//                      FALSE -- if NOT successful
//
//  History:    10-Oct-95   SHernd   Created
//
//---------------------------------------------------------------------------
BOOL InitApplication(HINSTANCE hInstance)
{
    return TRUE;
}



//+--------------------------------------------------------------------------
//
//  Function:   InitInstance
//
//  Synopsis:   Reads the application information and then creates the main
//              window that is to be associated with this instance of the
//              application
//
//  Arguments:  [hInstance] -- instance of the application
//              [nCmdShow]  -- how to show the window
//
//  Returns:    BOOL -- TRUE  -- if successful
//                      FALSE -- if NOT successful
//
//  History:    10-Oct-95   SHernd   Created
//
//---------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND   hwnd;

    g_hInst = hInstance;

    g_pMainDlg->CreateMultiDialog(hInstance, 
                                  MAKEINTRESOURCE(IDD_MAIN),
                                  NULL);
    hwnd = g_hwnd = g_pMainDlg->GetHwnd();
    if (!hwnd)
    {
        return FALSE;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    return TRUE;
}



LRESULT CALLBACK 
About(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static  HFONT   hfontDlg = NULL;

    LPSTR   lpVersion;
    DWORD   dwVerInfoSize;
    DWORD   dwVerHnd;
    UINT    uVersionLen;
    WORD    wRootLen;
    BOOL    bRetCode;
    int     i;
    char    szFullPath[256];
    char    szResult[256];
    char    szGetName[256];

    switch (msg)
    {
        case WM_INITDIALOG:  // message: initialize dialog box
            // Center the dialog over the application window
            CenterWindow (hwnd, GetWindow (hwnd, GW_OWNER));

            // Get version information from the application
            GetModuleFileName (g_hInst, szFullPath, sizeof(szFullPath));
            dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
            if (dwVerInfoSize)
            {
                // If we were able to get the information, process it:
                LPSTR   lpstrVffInfo;
                HANDLE  hMem;
                hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
                lpstrVffInfo  = (LPSTR)GlobalLock(hMem);
                GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpstrVffInfo);
                lstrcpy(szGetName, "\\StringFileInfo\\040904e4\\");
                wRootLen = lstrlen(szGetName);

                // Walk through the dialog items that we want to replace:
                for (i = IDC_FILEDESCRIPTION; i <= IDC_FILEVERSION; i++)
                {
                    GetDlgItemText(hwnd, i, szResult, sizeof(szResult));
                    szGetName[wRootLen] = (char)0;
                    lstrcat (szGetName, szResult);
                    uVersionLen   = 0;
                    lpVersion     = NULL;
                    bRetCode      =  VerQueryValue((LPVOID)lpstrVffInfo,
                            (LPSTR)szGetName,
                            (LPVOID *)&lpVersion,
                            (PUINT)&uVersionLen); // For MIPS strictness

                    if ( bRetCode && uVersionLen && lpVersion) {
                            // Replace dialog item text with version info
                            lstrcpy(szResult, lpVersion);
                            SetDlgItemText(hwnd, i, szResult);
                    }
                }

                GlobalUnlock(hMem);
                GlobalFree(hMem);
            }

            // Create a font to use
            hfontDlg = CreateFont(12, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0,
                                  VARIABLE_PITCH | FF_SWISS, "");

            // Walk through the dialog items and change font
            for (i = IDC_FILEDESCRIPTION; i <= IDC_FILEVERSION; i++)
            {
				SendMessage(GetDlgItem(hwnd, i), 
				            WM_SETFONT, 
				            (UINT)hfontDlg, 
				            TRUE);
            }

            return TRUE;

        case WM_COMMAND:                          
                if (LOWORD(wParam) == IDOK ||   // "OK" box selected?
                    LOWORD(wParam) == IDCANCEL) // System menu close command?
                {  
                    EndDialog(hwnd, TRUE);
                    DeleteObject(hfontDlg);
                    return TRUE;
                }
                break;
    }

    return FALSE;
}



// Porting code.
void *hwndGetMainHwnd()
{
	return g_hwnd;
}

// Porting code.
HINSTANCE hinstGetMainHInstance()
{
	return g_hInst;
}

// Porting code.
void ResetAppData()
{
	// Clears all data that needs clearing on a world dbase reset.

	// None for Trespass at this time.
}
 