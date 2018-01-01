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
 * $Log:: /JP2_PC/source/Lib/Sys/Errors.hpp                                              $
 * 
 * 13    3/19/98 5:17p Pkeet
 * Added variables to control scene saving.
 * 
 * 12    97/08/01 9:46 Speter
 * TerminalError() split into bTerminalError(), which returns whether to break, and
 * DebuggerBreak() macro.  These are now called inline by Assert().
 * 
 * 11    97-05-01 19:04 Speter
 * Added Warning() function, which invokes MessageBox.
 * 
 * 10    12/10/96 5:00p Pkeet
 * Added text caching to provide additional information in the error log when an assert
 * occurs.
 * 
 * 9     96/09/05 11:52 Speter
 * Changed #include resource.h to #include errors.h.
 * 
 * 8     7/16/96 11:29a Mlange
 * Moved several files to different directories, updated includes.
 * 
 * 7     96/06/06 18:42 Speter
 * Changed global bTerminalErrorFlag to local static b_called flag.
 * Now it clears b_called if the app does not in fact quit.
 * 
 * 6     96/05/14 10:49 Speter
 * Made TerminalError bRetry parameter default to false.
 * 
 * 5     5/14/96 10:16a Pkeet
 * Added 'Retry' or 'Cancel' options to TerminalError.
 * 
 * 4     5/08/96 12:04p Pkeet
 * Updated to allow error reporting before the main window is created.
 * 
 * 3     4/18/96 6:00p Mlange
 * Added support for Assert() failures.
 * 
 * 2     4/18/96 11:52a Pkeet
 * Putting into common format.
 * 
 * 1     4/17/96 10:18a Pkeet
 *
 *****************************************************************************************/

#ifndef HEADER_LIB_SYS_ERRORS_HPP
#define HEADER_LIB_SYS_ERRORS_HPP

//
// Macros and #includes.
//

#include "Lib/W95/Errors.h"


//
// Function prototypes.
//

//*********************************************************************************************
//
// void DebuggerBreak();
//
// Causes the debugger to break at this point.
//
// Notes:
//		This functionality is duplicated by a couple of Windows functions, such as DebugBreak().
//		But this allows easy use by Assert(), without having to include Windows files in all
//		modules.
//
//**************************************

#define DebuggerBreak()	_asm { int 3 }

//*********************************************************************************************
//
void Warning
(
	const char* str_message,			// Specific warning message.
	const char* str_class = "Warning"	// General class of warning.
);
//
// Pops up a warning dialog.
//
//**************************************

//*********************************************************************************************
//
bool bTerminalError
(
	uint        u_error_code,			// The terminating error from "resource.h"
	bool        bRetry = false,			// If 'true,' offer the user the option of continuing.
	const char* str_add_message = 0,	// Optional additional message information.
	uint        u_assert_lineno = 0		// Line # if caused by "ERROR_ASSERTFAIL."
);
// Produces a "errorlog.txt" file containing a timestamp and the error message, produces
// a dialog box with the error message, and attempts to clean up files, handles and so
// on before exiting the application.
//
// Returns:
//  Whether the user has chosen 'Ignore' and wishes to break.
//
// Critical cross references:
//
//  Class CWindowsShell defined in "winshell.hpp."
//  Function "AppendString" defined in "stringex.hpp."
//
// To Do:
//
//  Additional terminating functions have to be added to virtually every
//  module that creates handles or allocates memory.
//
// Notes:
//	Handle cases where the application and main window are not yet created.
//
//**************************************

//*********************************************************************************************
//
void TerminalError
(
	uint        u_error_code,			// The terminating error from "resource.h"
	bool        bRetry = false,			// If 'true,' offer the user the option of continuing.
	const char* str_add_message = 0,	// Optional additional message information.
	uint        u_assert_lineno = 0		// Line # if caused by "ERROR_ASSERTFAIL."
);
//
// Calls bTerminalError, and then DebugBreak() if it returns true.
//
//**************************************

//*********************************************************************************************
//
void SetCacheText
(
	const char* str = 0	// Text to cache.
);
//
// Caches text in string for output on an assertion fail.
//
//**************************************


//
// Global variables.
//

// Flag to temporarily enable and disable the automatic saving of a scene.
extern bool bAutoSaveScene;

// Flag to permanently disable the automatic saving of a scene.
extern bool bDisbableAutoSaveScene;


#endif
