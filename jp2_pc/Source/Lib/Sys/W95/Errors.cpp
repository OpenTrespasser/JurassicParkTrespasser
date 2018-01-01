/****************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:   Error handling mechanism.
 *
 * Notes:      File must be constantly updated as new modules are created.
 *
 * To do:      Add "Abort\Retry" error handling.
 *
 * Critical cross references:
 *
 *  Uses strings from "winshell.rc" and macros from "resources.h."
 *  Calls terminating functions from the following modules:
 *     - winshell.cpp
 *
 *****************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/W95/Errors.cpp                             $
 * 
 * 43    98/09/17 16:41 Speter
 * Update due to DebugConsole.hpp.
 * 
 * 42    9/11/98 5:16p Mmouni
 * Removed assert if errolog.txt file fails to open.
 * 
 * 41    7/09/98 5:15p Rwyatt
 * _CRT_LEACK_CHECK_DF is not defined in release mode. Full marks to
 * Microsoft
 * 
 * 40    7/09/98 4:35p Rwyatt
 * Removed the memory dump when abort is used from the assert dialog.
 * 
 * 39    6/30/98 9:26p Pkeet
 * Autosave is disabled in final mode.
 * 
 * 38    4/16/98 1:57p Pkeet
 * Fixed bug that caused a crash if an assert was hit prior to created
 * 'pwWorld.'
 * 
 * 37    3/19/98 5:56p Pkeet
 * Fixed minor bugs in the scene autosave code.
 * 
 * 36    3/19/98 5:18p Pkeet
 * Scenes are automatically saved with an assert unless explicitly disabled.
 * 
 * 35    12/09/97 4:47p Shernd
 * Added line number and error number to non GDI version of TerminalError
 * 
 * 34    12/08/97 12:00p Shernd
 * Added the ability to not have MessageBoxes popup.
 * 
 * 33    11/26/97 11:30a Agrant
 * If the game is not paused while in the assert, it keeps sending step
 * messages during the assert.  This is mighty strange, as the dialog is
 * supposed to be modal.
 * We now pause the game for the duration of the TerminalError call.
 * 
 *****************************************************************************************/

//
// Macros and #includes.
//

#include "Common.hpp"
#include "../Errors.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/EntityDBase/GameLoop.hpp"
#include "Lib/Sys/DebugConsole.hpp"

#include "Lib/Std/Stringex.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Sys/RegInit.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"

#include <stdio.h>
#include <time.h>
#include <crtdbg.h>

//
// Module specific variables.
//

// Storage for cached string.
char strCacheText[1024] = "";
bool bUseGDIForMessages = true;


//*********************************************************************************************
// Empty CRT dump/report functions used only when an assert dialog is aborted.
//
void __cdecl TrespasserCrtDumpClient(void * pvData, unsigned int nBytes)
{
}

int __cdecl TrespasserCrtReportHook(int nRptType, char *szMsg, int* pResult)
{
	return true;
}

//*********************************************************************************************
void Warning(const char* str_message, const char* str_class)
{
    if (bUseGDIForMessages)
    {
	    MessageBox(0, str_message, str_class, MB_OK | MB_SETFOREGROUND |
		           MB_SYSTEMMODAL | MB_ICONHAND);
    }
    else
    {
        dprintf("%s: %s\r\n", str_class, str_message);
    }
}

//*********************************************************************************************
void SetCacheText(const char* str)
{
	// Null the cache string.
	*strCacheText = 0;

	// If the string is null, do nothing.
	if (str == 0)
	{
		return;
	}

	// Copy the string.
	AppendString(strCacheText, str, sizeof(strCacheText));
}


//*********************************************************************************************
bool bTerminalError(uint u_error_code, bool b_retry, const char * str_add_message,
				    uint u_assert_lineno)
{
	// Store the save scene flag.
	bool b_auto_save_scene = bAutoSaveScene;

	//
	// Attempt to save the scene.
	//

	// See if the scene is permanently disabled.
	bDisbableAutoSaveScene = !bGetAutoSave();

	// Attempt to save the scene.
	if (bAutoSaveScene && !bDisbableAutoSaveScene && BUILDVER_MODE != MODE_FINAL)
	{
		// Toggle the save scene flag off to prevent a recursive assert.
		bAutoSaveScene = false;

		// Save the scene.
		if (pwWorld)
			pwWorld->bSaveWorld("Error.scn");
	}

    if (!bUseGDIForMessages)
    {
        dprintf("Terminal Error:  code %i at %i in %s\r\n", u_error_code, u_assert_lineno, str_add_message);
		bAutoSaveScene = b_auto_save_scene;
        return false;
    }

	//
	// Undo funny cursor problems.
	//
	ReleaseCapture();
	ShowCursor(true);

	// NO!   Pausing is the overlooked feature that costs us valuable hours.
	gmlGameLoop.bPauseGame = true;
	//ClipCursor(0);

	static bool b_called = false;		// Prevent recursive calls.
	char   str_message[1024];
	char   str_lineno[16];
	int    i_terminate;


	//
	// Make sure that TerminalError function is called once only.
	//
	if (b_called)
	{
		// Cause a fault, so the debugger can stop here.
		*(int*)0 = 7;
		TerminateShell(TRUE);
	}
	b_called = true;

	// Make the string null-terminated.
	*str_message = 0;

	//
	// Load the initial error message string from the Windows resource, or, if the application is
	// not yet fully initialized, display a default string.
	//
	if (prnshMain == 0)
	{
		// Display a default string.
		AppendString(str_message, "Error during application initialization.", sizeof(str_message));
	}
	else
	{
		// Load the error message string from the Windows resource.
		if (LoadString(prnshMain->hinsAppInstance, u_error_code, str_message, sizeof(str_message)) == 0)
		{
			AppendString(str_message, "Unable to find the error message string!", sizeof(str_message));
		}
	}

	// Print out any additional cached text.
	if (*strCacheText)
	{
		AppendString(str_message, "\n(", sizeof(str_message));
		AppendString(str_message, strCacheText, sizeof(str_message));
		AppendString(str_message, ")", sizeof(str_message));
	}

	//
	// Append an additional message string if there is one.
	//
	if (str_add_message)
	{
		AppendString(str_message, "\n\n", sizeof(str_message));
		AppendString(str_message, str_add_message, sizeof(str_message));

		// If the message is caused by a failed assertion, print out the line number.
		if (u_error_code == ERROR_ASSERTFAIL)
		{
			AppendString(str_message, "\n\nLine #: ", sizeof(str_message));
			itoa(u_assert_lineno, str_lineno, 10);
			AppendString(str_message, str_lineno, sizeof(str_message));
		}
	}

	//
	// Output to the error file (in debug mode only).
	//
#if VER_LOG_MESSAGES 
	FILE* fl_error_file_stream = freopen("errorlog.txt", "w", stderr);
	if (fl_error_file_stream)
	{
		// Print out file header.
		fprintf(fl_error_file_stream, "Lost World Error:\n");
		fprintf(fl_error_file_stream, "-----------------\n\n");

		// Print out time and date stamp.
		time_t tm_timestamp;

		time(&tm_timestamp);
		fprintf(fl_error_file_stream, "Date and Time:\n");
		fprintf(fl_error_file_stream, "--------------\n\n");
		fprintf(fl_error_file_stream, "%s\n\n", ctime(&tm_timestamp));

		// Print out error message.
		fprintf(fl_error_file_stream, "Error Message:\n");
		fprintf(fl_error_file_stream, "--------------\n\n");
		fprintf(fl_error_file_stream, str_message);
		fprintf(fl_error_file_stream, "\n");

		// Close the file.
		fprintf(fl_error_file_stream, "\n\n");
		fclose(fl_error_file_stream);
	}
#endif

	// Get the active window and minimize it.
	HWND hwnd = GetActiveWindow();

	if (hwnd)
	{
		ShowWindow(hwnd, SW_MINIMIZE);

		// Get the parent window and minimize it.
		hwnd = GetParent(hwnd);
		if (hwnd)
		{
			ShowWindow(hwnd, SW_MINIMIZE);
		}
	}

	//
	// Output a Windows message box.
	//
	if (b_retry)
	{
		AppendString(str_message, "\n\nPress IGNORE for debugger", sizeof(str_message));
		i_terminate = MessageBox(0, str_message, "Lost World Error", MB_OK | MB_ICONSTOP |
			                     MB_SYSTEMMODAL | MB_ABORTRETRYIGNORE | MB_DEFBUTTON3 |
								 MB_SETFOREGROUND);
	}
	else
	{
		i_terminate = IDRETRY;
		MessageBox(0, str_message, "Lost World Error", MB_OK | MB_SETFOREGROUND |
		           MB_SYSTEMMODAL | MB_ICONHAND);
	}

	//
	// Terminate the pWindowsShell application.
	//
	if (i_terminate == IDABORT)
	{
		//
		// If abort is clicked switch of the memory leaks so it does not dump
		// the 50000 or so allocated memory blocks....
		//

#ifdef _DEBUG
		// Get the current state of the debug flags
		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
		// Turn off the memory dump
		tmpFlag &= ~(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
		// Set the new state for the flag
		_CrtSetDbgFlag( tmpFlag );
#endif

		
		//
		// The above code does not work in an MFC app because within MFC there
		// is a static _AFX_DEBUG_STATE class which when destroyed explicitly
		// calls _CrtDumpMemoryLeaks(). To prevent all the memory leaks being
		// dumped we replace the dump function with an empty function does
		// nothing. 
		// This means that each block gets passed to the dump function but no
		// OutputDebugString calls are made so no ReadProcessMemory calls are
		// made and therefore the app should exit immediately.
		//
		_CrtSetReportHook(TrespasserCrtReportHook);
		_CrtSetDumpClient(TrespasserCrtDumpClient);

		TerminateShell(TRUE);
	}

	// In case we get out of here, reset recursion check.
	b_called = false;

	// Restore the active window to its original state, if there is one.
	if (hwnd) 
		ShowWindow(hwnd, SW_NORMAL);

	// Restore the game to run mode!
	gmlGameLoop.bPauseGame = false;

	// Restore the save scene flag.
	bAutoSaveScene = b_auto_save_scene;
	return i_terminate == IDIGNORE;
}

//*********************************************************************************************
void TerminalError(uint u_error_code, bool b_retry, const char* str_add_message,
				   uint u_assert_lineno)
{
	if (bTerminalError(u_error_code, b_retry, str_add_message, u_assert_lineno))
		DebuggerBreak();
}


//
// Global variables.
//
bool bAutoSaveScene         = true;
bool bDisbableAutoSaveScene = false;
