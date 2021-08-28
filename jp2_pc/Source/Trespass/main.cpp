//+--------------------------------------------------------------------------
//
//  Copyright (C) DreamWorks Interactive, 1997.
//
//  File:       main.cpp
//
//  Contents:   WinMain window handler routine
//
//  Classes:
//
//  Functions:
//
//  History:    21-Oct-97   SHernd   Created
//
//---------------------------------------------------------------------------

#include "precomp.h"
#pragma hdrstop

#include <time.h>
#include <ShlObj.h>
#include "resource.h"
#include "main.h"
#include "../Lib/Sys/reg.h"
#include "../Lib/Sys/RegInit.hpp"
#include "Lib/Sys/Permissions.hpp"
#include "Lib/Sys/FileEx.hpp"
#include "supportfn.hpp"
#include "Lib/View/DisplayMode.hpp"
#include "tpassglobals.h"
#include "gblinc/buildver.hpp"
#include "Lib/W95/Direct3DCards.hpp"
#include "version.hpp"
#include <filesystem>

#if TARGET_PROCESSOR == PROCESSOR_PENTIUM
	#pragma message("Target build processor : Intel Pentium")
#elif TARGET_PROCESSOR == PROCESSOR_PENTIUMPRO
	#pragma message("Target build processor : Intel Pentium Pro / Pentium II")
#elif TARGET_PROCESSOR == PROCESSOR_K6_3D
	#pragma message("Target build processor : AMD-K6 3D")
#endif




//+--------------------------------------------------------------------------
//-------------------------       Globals      ------------------------------
//---------------------------------------------------------------------------

HINSTANCE   g_hInst = NULL;
HWND        g_hwnd = NULL;
char        g_szAppName[] = "DWI - Trespasser";
char        g_szTitle[255];
UINT        g_uiRegMsg;
CMainWnd *	g_pMainWnd;
bool        bUseOutputFiles = FALSE;
bool        bUseReplayFile = TRUE;
bool		g_bRestartWithRenderDlg = FALSE;
bool        bIsTrespasser = true;

extern bool bUseGDIForMessages;

BOOL CALLBACK SiblingFindProc(HWND hwnd, LPARAM lParam)
{
    SendMessage(hwnd, lParam, 0, 0);

    return TRUE;
}


void FindRunningApp(LPARAM lMsgName)
{
    EnumWindows((WNDENUMPROC)SiblingFindProc, lMsgName);
}

std::filesystem::path OpenDirectoryChooser(const std::filesystem::path& defaultPath)
{
    CCom<IFileDialog> dlg = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg))) || !dlg)
        return {};
	
    FILEOPENDIALOGOPTIONS options;
    if (FAILED(dlg->GetOptions(&options)))
	    return {};
    
    options |= FOS_PICKFOLDERS;
    options |= FOS_PATHMUSTEXIST;
	if (FAILED(dlg->SetOptions(options)))
        return {};

    CCom<IShellItem> startPathItem = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName(defaultPath.wstring().c_str(), nullptr, IID_PPV_ARGS(&startPathItem))))
        dlg->SetDefaultFolder(startPathItem); //Can ignore error here

	
    if (FAILED(dlg->Show(g_hwnd)))
        return {};

	
    CCom<IShellItem> selected = nullptr;
    if (FAILED(dlg->GetResult(&selected)) || !selected)
	    return {};
    
    PWSTR pathstring = nullptr;
	if (FAILED(selected->GetDisplayName(SIGDN_FILESYSPATH, &pathstring)) || !pathstring)
        return {};
	
    std::filesystem::path result = pathstring;
    CoTaskMemFree(pathstring);
    return result;
}

//+--------------------------------------------------------------------------
//
//  Function:   GetModulePath
//
//  Synopsis:   Returns the path (no file name) of the instance specified.
//
//  Arguments:  [hInst]  -- instance to return the path of
//              [pszDst] -- buffer to place the path in
//              [cch]    -- size of the path
//
//  Returns:    int - length of the string placed in the buffer
//
//  History:    10-Oct-95   SHernd   Created
//
//---------------------------------------------------------------------------
int GetModulePath(HINSTANCE hInst, LPSTR pszDst, int cch)
{
    char    sz[_MAX_PATH];
    char    sz2[_MAX_PATH];
    char    szDrive[_MAX_DRIVE];

    GetModuleFileName(g_hInst, sz, cch);

    _splitpath(sz, szDrive, sz2, NULL, NULL);

    strcpy(sz, szDrive);
    strcat(sz, sz2);

    strcpy(pszDst, sz);
    return strlen(pszDst);
}

void SetProperWorkingDir()
{
    char        szPath[_MAX_PATH];

#if VER_TEST
    GetFileLoc(FA_INSTALLDIR, szPath, sizeof(szPath));
#else
    GetModulePath(g_hInst, szPath, sizeof(szPath));
#endif
	Trace(("%s", szPath));

	//SetCurrentDirectory would default to C:\ with empty input
    //Backslash was appended by GetFileLoc
	if (szPath[0] != '\0' && strcmp(szPath, "\\") != 0) 
		SetCurrentDirectory(szPath);
}


//+--------------------------------------------------------------------------
//
//  Function:   bRunSafeMode
//
//  Synopsis:   Sets settings to run safemode.
//
//  Arguments:  None.
//
//  Returns:    None.
//
//  History:    8-July-98 PKeet
//
//---------------------------------------------------------------------------
bool bRunSafeMode()
{
	// Display dialog.
	int i_retval = MsgDlg(g_hwnd, MB_YESNOCANCEL | MB_ICONQUESTION, IDS_TITLE, IDS_SAFEMODE);

	if (i_retval == IDNO)
	{
		DisableSafemode();
		return false;
	}

	// Exit the application if 'Cancel' is selected.
	if (i_retval == IDCANCEL)
	{
		return true;
	}

	// Conservative, non-hardware accelerated settings.
	SetD3D(FALSE);
	SetDimensions(640, 480);
	bSetInitFlag(TRUE);
	SetAutoLoad(FALSE);

	// Delete Registry Keys.
    DeleteValue(strD3D_TITLE);
	DeleteValue(strFLAG_D3D);
	DeleteValue(strD3D_FILTERCACHES);
	DeleteValue(strVIDEOCARD_TYPE);
	DeleteValue(strVIDEOCARD_NAME);
	DeleteValue(strDDDEVICE_NAME);
	DeleteValue(strDDDEVICE_DESCRIPTION);
	DeleteValue(strDDDEVICE_GUID);
	DeleteValue(strD3DDEVICE_NAME);
	DeleteValue(strD3DDEVICE_DESCRIPTION);
	DeleteValue(strD3DDEVICE_GUID);
	SetAutoLoad(FALSE);

	// Set new conservative audio values.
    SetRegValue(REG_KEY_AUDIO_EFFECT,    0);
    SetRegValue(REG_KEY_AUDIO_AMBIENT,   0);
    SetRegValue(REG_KEY_AUDIO_VOICEOVER, 0);
    SetRegValue(REG_KEY_AUDIO_MUSIC,     0);
    SetRegValue(REG_KEY_AUDIO_SUBTITLES, 1);
    SetRegValue(REG_KEY_AUDIO_ENABLE3D,  0);
	return false;
}



//+--------------------------------------------------------------------------
//
//  Function:   SetSpeedDefaults
//
//  Synopsis:   Sets settings to run based on detected CPU speed. Will not
//              change already stored settings.
//
//  Arguments:  None.
//
//  Returns:    None.
//
//  History:    8-July-98 PKeet
//
//---------------------------------------------------------------------------
void SetSpeedDefaults()
{
	// Ignore these defaults if safe mode is active.
	if (bSafemode())
		return;

	if (bAutoSettings(true))
	{
		Trace(("Settings for machine already performed...\n"));
		return;
	}

	// Set defaults.
	SetRegValue(REG_KEY_RENDERING_QUALITY, DEFAULT_RENDERING_QUALITY);
	SetDimensions(320, 240);

	// Get the cpu speed.
	uint32 u4_mhz = u4GetCPUSpeed();
	Trace(("Machine CPU speed: %ld\n", u4_mhz));

	// There is a mistake if cpu speed is null or very small.
	if (u4_mhz < 10)
	{
		Trace(("Error in autosettings for machine...\n"));
	}

	// Slow machine settings.
	if (u4_mhz < 220)
	{
		SetRegValue(REG_KEY_RENDERING_QUALITY, 2);
		SetDimensions(320, 240);
		Trace(("Set for slow machine...\n"));
		return;
	}

	// Moderate speed machines.
	if (u4_mhz < 280)
	{
		SetRegValue(REG_KEY_RENDERING_QUALITY, 3);
		SetDimensions(400, 300);
		Trace(("Set for moderate machine...\n"));
		return;
	}

	// Fastest machines.
	SetRegValue(REG_KEY_RENDERING_QUALITY, 3);
	SetDimensions(512, 384);
	Trace(("Set for fastest machine...\n"));
}



//+--------------------------------------------------------------------------
//
//  Function:   TrespassExceptionCleanup
//
//  Synopsis:   When an exception is trapped in WinMain this code gets called
//              to clean up as much as possible for the app to terminally 
//              fail gracefully.
//
//  Returns:    void
//
//  History:    19-Aug-98    SHernd  Created
//
//---------------------------------------------------------------------------
void TrespassExceptionCleanup()
{
    ForceShowCursor(TRUE);
    ClipCursor(NULL);

	// Remove the audio, we are about to quit.
	delete CAudio::pcaAudio;

	std::destroy_at(&prasMainScreen);

	g_initDD.ReleaseAll();

    if (g_hwnd)
    {
        DestroyWindow(g_hwnd);
		g_hwnd = 0;
    }

	delete g_pMainWnd;
	g_pMainWnd = NULL;

    CoUninitialize();

    ErrorDlg(g_hwnd, IDS_ERROR_FATAL);

    ExitProcess(0);
}




bool ValidateDiskSpace(int iMB)
{
    char szPath[_MAX_PATH] = { '\0' };
    GetFileLoc(FA_INSTALLDIR, szPath, sizeof(szPath));
    
    const auto diskstat = std::filesystem::space(szPath);
    
    //Existing swp files count as free space
    std::uintmax_t swpFilesSize = 0;
    for (const auto& entry : std::filesystem::directory_iterator("."))
        if (entry.is_regular_file() && entry.path().extension() == ".swp")
            swpFilesSize += entry.file_size();
    
    const std::uintmax_t neededBytes = iMB * static_cast<std::uintmax_t>(1024 * 1024);
    return diskstat.free + swpFilesSize > neededBytes;
}


bool TestDataDriveValidity()
{
	bool validatedDataDrive = false;
	while (!validatedDataDrive)
	{
		char    sz[_MAX_PATH];
		GetFileLoc(FA_DATADRIVE, sz, sizeof(sz));

		std::filesystem::path datapath = sz;
		
        // This should be the unique file on the disk
		if (!std::filesystem::exists(datapath / "menu" / "tpassintro.smk"))
		{
            //Without the data subpath
            auto datadrive = datapath.has_parent_path() ? datapath.parent_path() : datapath;
			
			if (datadrive.has_parent_path() && datadrive.has_stem() && datadrive.stem() == "data"
                && std::filesystem::exists(datadrive / "menu" / "tpassintro.smk"))
			{
				//We are in the data directory itself, correct to parent directory
                auto parent = datadrive.parent_path();
                SetRegString(REG_KEY_DATA_DRIVE, parent.string().c_str());
                return true;
			}
			
			if (MsgDlg(g_hwnd,
			           MB_YESNOCANCEL | MB_SETFOREGROUND,
			           IDS_ERROR_TITLE,
			           IDS_DATA_DRIVE_NOT_FOUND,
			           datadrive.string().c_str()) != IDYES)
			{
				Trace(("We are Cancelling from no data found"));
				return false;
			}

			auto newpath = OpenDirectoryChooser(datadrive);
			if (!newpath.empty())
				SetRegString(REG_KEY_DATA_DRIVE, newpath.string().c_str());
		}
		else
			validatedDataDrive = true;
	}
	
	return true;
}

//+--------------------------------------------------------------------------
//
//  Function:   DoWinMain
//
//  Synopsis:   Extra WinMain handler.  This exists because of the 
//              __try/__finally block in WinMain
//
//  Arguments:  [hInstance]     -- 
//              [hPrevInstance] -- 
//              [lpCmdLine]     -- 
//              [nCmdShow]      -- 
//
//  Returns:    int
//
//  History:    19-Aug-98    SHernd  Created
//
//---------------------------------------------------------------------------
int DoWinMain(HINSTANCE hInstance,
              HINSTANCE hPrevInstance,
              LPSTR lpCmdLine,
              int nCmdShow)
{
    int                 iRet = 0;
    HANDLE              hMutex;
    int                 i;
    bool                bVideoCardChosen;

	BOOL b_change_safemode = TRUE;

    CoInitialize(nullptr);
	
	// Make sure the cursor can access the safe mode dialog.
	ClipCursor(0);

    g_hInst = hInstance;
    bUseGDIForMessages = false;

    OpenKey();

    SetProperWorkingDir();

    if (!bCanCreateFile("permissiontestfile.txt") && !IsProcessElevated()) {
        if (!StartAsElevated(g_hwnd, hInstance))
			dout << "Start with elevated permissions failed or declined by user" << std::endl;
    	//New process started (or not), exit current one
        goto Cleanup;
    }
	
    InitCommonControls();

    g_uiRegMsg = RegisterWindowMessage("DWI Trespasser FINDER");
    hMutex = CreateMutex(NULL, FALSE, "DWI Trespasser MUTEX");
    if (WaitForSingleObjectEx(hMutex, 1000, FALSE) == WAIT_TIMEOUT)
    {
        FindRunningApp(g_uiRegMsg);
        return -1;
    }

    if ( !IsDisplayConfigurationValid( GetSystemBitDepth(g_hwnd), bGetD3D(), GetWindowModeActive()))
    {
        if (MsgDlg(g_hwnd,
            MB_YESNOCANCEL | MB_SETFOREGROUND,
            IDS_ERROR_TITLE,
            IDS_ERR_INVALID_DISPLAY_CONFIGURATION) != IDYES)
            goto Cleanup;
    }

    //
    // Check To see if the installed registry flag is set.  If it has not been
    // set then we haven't been installed.
    //
    if (!GetRegValue(REG_KEY_INSTALLED, FALSE))
    {
        SetAllSettingsToDefault();
    	
        std::filesystem::path path = std::filesystem::current_path();
        SetRegString(REG_KEY_DATA_DRIVE, path.string().c_str());
        SetRegString(REG_KEY_INSTALLED_DIR, path.string().c_str());
    }

	// Check existance of file system
    if (!TestDataDriveValidity()) 
        goto Cleanup;

    if (!ValidateDiskSpace(110))
    {
        ErrorDlg(g_hwnd, IDS_ERROR_DISKSPACE, 100);
        goto Error;
    }

	//
	// Display a dialog indicating that the application is running in safe 
    // mode if it is running in safe mode.
	//
	if (bSafemode() && !IsKeyDown(VK_SHIFT))
	{
		if (bRunSafeMode())
		{
			b_change_safemode = FALSE;
			goto Cleanup;
		}
	}

    if (!hPrevInstance)
    {
        if (!InitApplication(hInstance))
        {
            goto Error;
        }
    }

    g_pMainWnd = new CMainWnd;
    if (g_pMainWnd == NULL)
    {
        goto Error;
    }

    if (!InitInstance(hInstance, nCmdShow))
    {
        goto Error;
    }

    if (!g_hwnd)
    {
        goto Error;
    }

DoRestartWithRenderDlg:
	g_bRestartWithRenderDlg = FALSE;
    bVideoCardChosen = false;
    if (!bGetInitFlag() || IsKeyDown(VK_SHIFT))
    {
		pcdbMain.Clear();
		g_CTPassGlobals.bHardReset = true;

        CConfigureWnd       dlg;

        ForceShowCursor(TRUE);
        dlg.MultiDialogBox(g_hInst, MAKEINTRESOURCE(IDD_INITIALIZATION), g_hwnd);
        g_pMainWnd->m_bRelaunch = true;
        bVideoCardChosen = true;
    }

    EnableMenuItem(GetSystemMenu(g_hwnd, FALSE),
                   SC_CLOSE,
                   MF_GRAYED);
    EnableMenuItem(GetSystemMenu(g_hwnd, FALSE),
                   SC_MOVE,
                   MF_GRAYED);

	// setup a global that contains the system page size.
	SetupSystemPageSize();

    //
	// Create the audio system now we have a window handle
    //
    if (!CAudio::pcaAudio)
    {
	    new CAudio(g_hwnd,
				(bool)GetRegValue(REG_KEY_AUDIO_ENABLE, TRUE),
				(bool)GetRegValue(REG_KEY_AUDIO_ENABLE3D, TRUE) & (!(bool)bSafeModeReg) );
    }

    RefreshAudioSettings();

	// Initialize the global DirectDraw object.
    i = g_initDD.Initialize();

	if (i != 0)
	{
		switch (i)
		{
			case -1:
                ErrorDlg(g_hwnd, IDS_ERROR_DDRAW_NOTFOUND);
                goto Cleanup;
				break;

			case -2:
			case -3:
			case -4:
                ErrorDlg(g_hwnd, IDS_ERROR_DDRAW_NOTFOUND);
                goto Cleanup;
				break;
		}
	}

    //
    // Check for Video Driver Certification
    //
    if (!g_initDD.IsCertified() && 
        !GetRegValue(REG_KEY_DDRAW_CERT_IGNORE, FALSE))
    {
        CErrorDlg   cdlg(ERRORDLG_DDRAW_CERTIFIED);

        TraceError(("WinMain() -- "
                    "Video Card is not certified"));

        if (cdlg.DoDialog() == IDCANCEL)
        {
            if (bVideoCardChosen)
            {
                bSetInitFlag(FALSE);
                g_bRestartWithRenderDlg = true;
            }

            goto Cleanup;
        }
    }

    //
    // Check for Hardware Support
    //
    if (!g_initDD.IsHardwareSupported() && 
        !GetRegValue(REG_KEY_DDRAW_HARD_IGNORE, FALSE))
    {
        CErrorDlg   cdlg(ERRORDLG_DDRAW_HARDWARE);

        TraceError(("WinMain() -- "
                    "Video Card does not have hardware support"));

        if (cdlg.DoDialog() == IDCANCEL)
        {
            goto Cleanup;
        }
    }

    Video::EnumerateDisplayModes();

	//
	// If safe mode is not active, set some defaults that depend on the CPU speed.
	//
	SetSpeedDefaults();
    g_CTPassGlobals.SaveDefaultKeys();
    g_CTPassGlobals.GetSavedKeys();

    g_pMainWnd->m_bInit = true;

    g_pMainWnd->InitSurface();

    g_pMainWnd->StartGame();

    iRet = 1;

Cleanup:

    if (DirectDraw::pdd4)
    {
    	DirectDraw::err = DirectDraw::pdd4->SetCooperativeLevel(g_hwnd, DDSCL_NORMAL);
    }

	// Release everything really well.
	d3dDriver.Purge();
	d3dDriver.Uninitialize();
	if (pwWorld)
		wWorld.Reset();
	if (prasMainScreen)
	{
		prasMainScreen->uRefs = 1;
		std::destroy_at(&prasMainScreen);
	}
	g_initDD.ReleaseAll();

	// Remove the render shell.
	delete prnshMain;
	prnshMain = 0;

    if (g_pMainWnd)
    {
        g_pMainWnd->m_bInit = false;
    }

	if (g_bRestartWithRenderDlg)
	{
		goto DoRestartWithRenderDlg;
	}

    if (g_hwnd)
    {
        DestroyWindow(g_hwnd);
		g_hwnd = 0;
    }

	delete g_pMainWnd;
	g_pMainWnd = NULL;

	// Remove the audio, we are about to quit.
	delete CAudio::pcaAudio;

    CoUninitialize();
	
    PostQuitMessage(0);

	// BUGBUG:  Close out file system

	CloseKey(b_change_safemode);
	
    return iRet;

Error:
    iRet = 0;

    goto Cleanup;
}


void DumpException(LPEXCEPTION_POINTERS pep_info)
{
	if (!pep_info)
		return;

	// Open log with a timestamp.
	time_t tm_timestamp;
	time(&tm_timestamp);
	FILE* pfile = fopen("error.log", "w");
	if (!pfile)
		return;
	fprintf(pfile, "Trespasser Build: %ld, %ld, %ld\n", BUILD_MAJOR, BUILD_MINOR, BUILD_NUM);
	fprintf(pfile, "Trespasser Fault: %s\n", ctime(&tm_timestamp));

	// Print the exception record.
	if (pep_info->ExceptionRecord)
	{
		fprintf(pfile, "Exception Record\n");
		fprintf(pfile, "----------------\n");
		fprintf(pfile, "Code   : 0x%08X\n", pep_info->ExceptionRecord->ExceptionCode);
		fprintf(pfile, "Flags  : 0x%08X\n", pep_info->ExceptionRecord->ExceptionFlags);
		fprintf(pfile, "Address: 0x%08p\n", pep_info->ExceptionRecord->ExceptionAddress);
		for (uint u = 0; u < pep_info->ExceptionRecord->NumberParameters && u < 16; ++u)
			fprintf(pfile, "Info %2ld: 0x%08X\n", u, pep_info->ExceptionRecord->ExceptionInformation[u]);
	}

	// Print the context record.
	if (pep_info->ContextRecord)
	{
		fprintf(pfile, "\n");
		fprintf(pfile, "Context Record\n");
		fprintf(pfile, "--------------\n");
		uint32* pu4 = (uint32*)pep_info->ContextRecord;
		for (uint u = 0; u < sizeof(CONTEXT) / 4; ++u)
		{
			fprintf(pfile, "%2ld: 0x%08X", u, pu4[u]);
			++u;
			if (u >= sizeof(CONTEXT) / 4)
				break;
			fprintf(pfile, ", 0x%08X\n", pu4[u]);
		}
	}

	// Close log.
	fprintf(pfile, "\n");
	fclose(pfile);
}


int FilterException(LPEXCEPTION_POINTERS ep_info)
{
    Trace(("Exception:"));
    Trace(("Exception Code: %x",ep_info->ExceptionRecord->ExceptionCode));
    Trace(("Execution Address: %x",ep_info->ExceptionRecord->ExceptionAddress));

    if (ep_info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
    {
        // Access violations have more info in the parameter section.
        if (ep_info->ExceptionRecord->ExceptionInformation[0] == 0)
        {
            // illegal read
            Trace(("Illegal Memory read from address %x",
                   ep_info->ExceptionRecord->ExceptionInformation[1]));
        }
        else
        {
            // illegal write
            Trace(("Illegal Memory write to address %x",
                  ep_info->ExceptionRecord->ExceptionInformation[1]));
        }
    }

    if (ep_info->ExceptionRecord->ExceptionFlags == 0)
    {
        Trace(("Excecution can be restarted..."));
    }
    else
    {
        // in this case do not return EXCEPTION_CONTINUE_EXECUTION 
        // otherwise you will get a EXCEPTION_NONCONTINUABLE_EXCEPTION.
        Trace(("Excecution CANNOT be restarted"));
    }
	
	// Output the exception
	DumpException(ep_info);

    return EXCEPTION_EXECUTE_HANDLER;
}



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
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    int                 iRet;

#if BUILDVER_MODE == MODE_FINAL
    __try
#endif
    {
        iRet = DoWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }
#if BUILDVER_MODE == MODE_FINAL
    __except (FilterException(GetExceptionInformation()))
    {
        __try
        {
            TrespassExceptionCleanup();
        }
        __except (FilterException(GetExceptionInformation()))
        {
            ExitProcess(0);
        }
    }
#endif

    return iRet;
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
    WNDCLASS  wc;

    wc.style         = CS_DBLCLKS;
    wc.lpfnWndProc   = (WNDPROC)WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szAppName;

    if (!::RegisterClass(&wc))
    {
        return FALSE;
    }

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
    char    sz[255];

    g_hInst = hInstance;

    LoadString(g_hInst, IDS_TITLE, sz, sizeof(sz));

	g_hwnd = NULL;

    int windowWidth = 640;
    int windowHeight = 480;
    bGetDimensions(windowWidth, windowHeight);

    DWORD style = WS_VISIBLE | WS_POPUP | WS_SYSMENU;
    if (GetWindowModeActive() == WindowMode::FRAMED)
        style |= WS_OVERLAPPEDWINDOW;
	
    if (!CreateWindowEx(0,
                    g_szAppName,
                    sz,
                    style,
                    0,
                    0,
                    windowWidth,
                    windowHeight,
                    NULL,
                    NULL,
                    hInstance,
                    NULL))
    {
        TraceError(("InitInstance() -- CreateWindow failed"));
        return FALSE;
    }

    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);

    return TRUE;
}


//+--------------------------------------------------------------------------
//.
//  Function:   MsgDlg
//
//  Synopsis:   Actual warning/error handler.
//
//  Arguments:  [hwndParent] -- parent window handle
//              [uiType]     -- button types
//              [uiCaption]  -- caption string index
//              [uiMsg]      -- messge string index
//              [...]        -- variable argument list
//
//  Returns:    void
//
//  History:    13-May-96   SHernd   Created
//
//---------------------------------------------------------------------------
int __cdecl MsgDlg(HWND hwndParent,
                   UINT uiType,
                   UINT uiCaption,
                   UINT uiMsg,
                   ...)
{
    int         iRet;
    va_list     vlShow;

    va_start(vlShow, uiMsg);
    iRet = ActualMsgDlg(hwndParent, uiType, uiCaption, uiMsg, vlShow);
    va_end(vlShow);

    return iRet;
}



int __cdecl ActualMsgDlg(HWND hwndParent,
                         UINT uiType,
                         UINT uiCaption,
                         UINT uiMsg,
                         va_list ap)
{
    char        szCaption[255];
    char        szMsg[4096];
    char        szResText[4096];
    int         iRet;

    iRet = LoadString(g_hInst, uiCaption, szCaption, sizeof(szCaption));
	if (iRet == 0)
	{
		return -1;
	}

    iRet = LoadString(g_hInst, uiMsg, szResText, sizeof(szResText));
    if (iRet == 0)
    {
        return -1;
    }

    wvsprintf(szMsg, szResText, ap);

    iRet = MessageBox(hwndParent,
                      szMsg,
                      szCaption,
                      MB_TOPMOST | MB_APPLMODAL | uiType);

    return iRet;
}




//+--------------------------------------------------------------------------
//.
//  Function:   ErrorDlg
//
//  Synopsis:   Displays an error message.  The error message is constructed
//              from the String Resource Table number then using the passed
//              in parameters it creates an error string.  This error string
//              is displayed as the text in the message box
//
//  Arguments:  [hwndParent] -- Parent Window
//              [uiErrMsg]   -- String Table Entry for error message base
//
//  Returns:    void
//
//  History:    25-Apr-96   SHernd   Created
//
//---------------------------------------------------------------------------
void __cdecl ErrorDlg(HWND hwndParent, UINT uiErrMsg, ...)
{
    va_list     vlShow;

    va_start(vlShow, uiErrMsg);
    ActualMsgDlg(hwndParent, MB_OK, IDS_ERROR_TITLE, uiErrMsg, vlShow);
    va_end(vlShow);
}


// new allocator for audio.lib
#ifdef _DEBUG
void* __cdecl operator new(size_t nSize, const char* lpszFileName, int nLine)
{
    return _malloc_dbg(nSize, _CLIENT_BLOCK, lpszFileName, nLine);
}
#endif



//+--------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

CErrorDlg::CErrorDlg(int iType)
{
    m_iType = iType;
    m_hwnd = NULL;
}

CErrorDlg::~CErrorDlg()
{
}


int CErrorDlg::DoDialog()
{
    return DialogBoxParam(g_hInst, 
                          MAKEINTRESOURCE(IDD_DXERROR), 
                          NULL, 
                          (DLGPROC)ErrorDlgProc, 
                          (LPARAM)this);
}


BOOL CErrorDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    char        sz[1024];
    int         iIDS;

    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    CenterWindow(hwnd, NULL);

    // Load in Dialog Title
    switch (m_iType)
    {
        case ERRORDLG_DSOUND:
            iIDS = IDS_ERR_AUDIO_NOT_CERTIFIED;
            break;

        case ERRORDLG_DDRAW_CERTIFIED:
            iIDS = IDS_ERR_VIDEO_NOT_CERTIFIED;
            break;

        case ERRORDLG_DDRAW_HARDWARE:
            iIDS = IDS_ERR_VIDEO_NO_HARDWARE;
            break;
    }

    LoadString(g_hInst, iIDS, sz, sizeof(sz));

    SetWindowText(hwnd, sz);

    // Load in Message Text
    switch (m_iType)
    {
        case ERRORDLG_DSOUND:
            iIDS = IDS_AUDIO_ERROR;
            break;

        case ERRORDLG_DDRAW_CERTIFIED:
            iIDS = IDS_DISPLAY_ERROR;
            break;

        case ERRORDLG_DDRAW_HARDWARE:
            iIDS = IDS_NOHARDWARE_ERROR;
            break;
    }

    LoadString(g_hInst, iIDS, sz, sizeof(sz));
    SetDlgItemText(hwnd, IDC_MESSAGE, sz);

    return TRUE;
}


void CErrorDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
        case IDOK:
            if (IsDlgButtonChecked(hwnd, IDC_NODISPLAY) == BST_CHECKED)
            {
                switch (m_iType)
                {
                    case ERRORDLG_DSOUND:
                        SetRegValue(REG_KEY_DSOUND_IGNORE, TRUE);
                        break;

                    case ERRORDLG_DDRAW_CERTIFIED:
                        SetRegValue(REG_KEY_DDRAW_CERT_IGNORE, TRUE);
                        break;

                    case ERRORDLG_DDRAW_HARDWARE:
                        SetRegValue(REG_KEY_DDRAW_HARD_IGNORE, TRUE);
                        break;
                }
            }

        case IDCANCEL:
            EndDialog(hwnd, id);
            break;
    }
}


void CErrorDlg::OnDestroy(HWND hwnd)
{
}


BOOL CALLBACK ErrorDlgProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    CErrorDlg * pwnd = (CErrorDlg*)GetProp(hwnd, "ClassWindowName");

    if (pwnd == NULL)
    {
        if (uiMsg == WM_INITDIALOG)
        {
            pwnd = (CErrorDlg *)lParam;
            SetProp(hwnd, "ClassWindowName", pwnd);
        
            if (pwnd == NULL)
            {
                return TRUE;
            }

            pwnd->m_hwnd = hwnd;
        }
        else
        {
            return FALSE;
        }
    }
    switch (uiMsg)
    {
        HANDLE_MSG(hwnd, WM_COMMAND,        pwnd->OnCommand);
        HANDLE_MSG(hwnd, WM_DESTROY,        pwnd->OnDestroy);
        HANDLE_MSG(hwnd, WM_INITDIALOG,     pwnd->OnInitDialog);
    }

    return FALSE;
}


//*********************************************************************************************
uint32 u4LookupResourceString(int32 i4_id,char* str_buf,uint32 u4_buf_len)
{
	char  buf[1024];

    int i_res = LoadString(g_hInst, IDS_STR_HINTS + i4_id, buf, 1024);

	if ((i_res>0) && (i_res<(int)u4_buf_len))
	{
		// copy the string to the destination buffer and process and escape sequences
		wsprintf(str_buf,buf);

		Assert(strlen(str_buf)<u4_buf_len);

		return strlen(str_buf);
	}

	return 0;
}