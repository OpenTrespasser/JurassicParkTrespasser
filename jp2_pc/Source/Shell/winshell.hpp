/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		The main Windows shell and Windows message loop. The global variable "pwnsWindowsShell"
 *		is automatically created when the application is run. The object pwnsWindowsShell
 *		contains variables associated with the main window, and procedures for handling Windows
 *		messages sent to the main Window.
 *
 * Bugs:
 *		The check marks in toggle menu items are not updated.
 *
 * To do:
 *      Add member functions to CWindowsShell as additional Windows message handling is
 *		required. Add capability for applications to decide whether raster needs clearing
 *      before painting.
 *
 * Notes:
 *		WinShell accepts command-line arguments at runtime. The current arguments are:
 *
 *				/w#		Sets default width for full-screen mode (e.g. /w640).
 *				/h#		Sets default height for full-screen mode (e.g. /h480).
 *				/b#		Sets default bit-depth for full-screen mode (e.g. /b8).
 *						Note: if the bit-depth is 0, the application will be initialized in
 *						windowed mode.
 *
 *				/g#		Sets the gamma-correction value. The value must be greater than zero.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Shell/winshell.hpp                                                   $
 * 
 * 23    96/08/05 12:34 Speter
 * Updated for new b_system_memory menu command and flag.
 * Added experimental code to test DirectDraw/GDI interaction.
 * 
 * 22    96/07/29 14:48 Speter
 * Changed management of text dialog.  Now just receives commands from Console menu, and lets
 * the Render module handle it.
 * 
 * 21    7/18/96 3:18p Pkeet
 * Removed appropriate components to the 'Render' module and the 'WinRenderTools' module.
 * 
 * 20    96/07/03 13:02 Speter
 * Moved several files to new directories, and changed corresponding include statements.
 * 
 * 19    6/24/96 6:25p Pkeet
 * Added 'pcdbMain' global variable.
 * 
 * 18    6/21/96 2:01p Pkeet
 * Added the '/G' for gamma-correction command-line argument.
 * 
 * 17    6/20/96 2:12p Pkeet
 * Added 'strCommandArg' global variable.
 * 
 * 16    6/19/96 3:24p Pkeet
 * Added command line argument parser (see 'Notes').
 * 
 * 15    96/06/07 9:58 Speter
 * Changed includes, namespaces to match new Video module.
 * 
 * 14    96/05/30 17:48 Speter
 * Added HideCursor function.
 * 
 * 13    5/29/96 4:32p Pkeet
 * Added CScreenRender global variable.
 * 
 * 12    5/24/96 4:03p Pkeet
 * Added 'prasZBuffer' global variable.
 * 
 * 11    5/23/96 11:59a Pkeet
 * Fixed bug when returning from minimized/full screen state. Automatically closed dialog box
 * when in full screen mode. Restores window position and size when returning from full screen
 * mode. Creates a client area of 640 x 480 by default. Fixed terminate shell. Added console
 * functions to the main menu.
 * 
 * 10    20-05-96 7:15p Mlange
 * Now declares the external definition of the main viewport.
 * 
 * 9     96/05/17 10:49 Speter
 * Added some Bugs comments.
 * 
 * 8     96/05/16 16:45 Speter
 * Added bContinuous member variable.
 * 
 * 7     5/09/96 4:39p Pkeet
 * Added 'CreateTextDialog' function.
 * 
 * 6     96/05/08 14:09 Speter
 * Updated for move of View/ files to Lib/View/
 * 
 * 5     5/08/96 1:16p Pkeet
 * Fixed DestroyWindow bug causing an Assert fail during normal program termination. Eliminated
 * the 'scrmodeMain' global and the 'SetWindowed' function. Fixed window style bug that allowed
 * resizing and menu drawing in full screen mode.
 * 
 * 4     5/07/96 2:29p Pkeet
 * Altered according to code review. Added palette functionality.
 * 
 * 3     5/03/96 5:10p Pkeet
 * Remove FAR's.
 * 
 * 2     4/30/96 6:03p Pkeet
 * To common standards.
 * 
 * 1     4/17/96 10:18a Pkeet
 *
 *********************************************************************************************/

#ifndef HEADER_LIB_WINSHELL_HPP
#define HEADER_LIB_WINSHELL_HPP

//
// Macros, includes and definitions.
//

#include "Lib/W95/WinAlias.hpp"
#include "Lib/View/Colour.hpp"
#include "Lib/View/Palette.hpp"
#include "Lib/View/Viewport.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/View/ColourBase.hpp"
#include "Lib/Renderer/ScreenRender.hpp"

// Application name string.
#define strAPPLICATION_NAME "LostWorld"

// String to appear in the Window title bar.
#define strTITLE            "The Lost World"


//
// Class definitions.
//

//*********************************************************************************************
//
class CWindowsShell
//
//  CWindowsShell encapsulates the functionality required of the mainwindow for a Windows
//  application. Certain member functions are included only as dummies, to be "filled-in"
//  as required, other functions are provided to simplify use of handles, timers and so on,
//  while the remaining functions are used internally within the module to manage the main
//  window.
//
// Prefix: wns
//
// Notes:
//		Code may be added to the following modules to extend functionality:
//
//		- PaintWindow()
//		- Create()
//
//		CWindowsShell requires "winshell.rc," "resource.h," and "errors.hpp."
//
//**************************************
{
public:
	// Public non-Windows variables.
	bool      bPause;				// Set to 'true' if the game is paused.
	bool	  bContinuous;			// Whether the app wants to run in continuous mode.

public:
	// Public Windows variables that may be needed outside of "winshell.cpp."
	HWND      hwndMain;				// Window handle.
	HINSTANCE hinsAppInstance;		// Handle of the application instance.

protected:

	// Member functions and variables that should not be accessed directly.
	bool      bActivePause;			// Pause state storage used by the WM_ACTIVE handler.
	bool	  bSystemMem;			// State of System Memory menu toggle.

public:

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	//*****************************************************************************************
	//
	CWindowsShell
	(
		HINSTANCE hins_app_instance	// The handle of the application instance.
	);
	//
	// Stops timer u4WindowsShellTimerID.
	//
	//**************************************


	//*****************************************************************************************
	//
	// Public member functions.
	//

	//*****************************************************************************************
	//
	void PaintWindow
	(
	);
	//
	// Insert code to redraw a window or surface here. Responds to a WM_ERASEBKND message in
	// preference to a WM_PAINT message.
	//
	//**************************************

	//*****************************************************************************************
	//
	void CreateMainScreen
	(
		int i_screen_width  = 0,	// Main screen width.
		int i_screen_height = 0,	// Main screen height.
		int i_screen_bits   = 0		// Main screen bit depth.
	);
	//
	// Creates a new instance of the main raster screen 'prasMainScreen.'
	//
	//**************************************

	//*****************************************************************************************
	//
	bool InitializeApplication
	(
		int i_show_flag
	);
	//
	// Creates the first instance of the application. Should not be called outside of module.
	//
	// Returns:
	//		Returns 'true' if the application was successfully initialized, otherwise returns
	//		'false.'
	//
	//**************************************

	//*****************************************************************************************
	//
	LRESULT WindowsMessageLoop
	(
		HWND   hwnd_handler,	// Handle of the window sending the message.
		uint   u_message,		// Windows message ID value.
		WPARAM wp_wparam,		// The first parameter passed with the message.
		LPARAM lp_lparam		// The second parameter passed with the message.
	);
	//
	// Main message loop for the window. Should not be called outside of module.
	//
	// Returns:
	//		The member function returns '0' for the LRESULT for any messages handled within
	//		the function. Messages not handled by the function default to the default Windows
	//		procedure, and the value returned will depend on the message.
	//
	//**************************************


	//*****************************************************************************************
	//
	// Protected member functions.
	//

protected:
	bool bKeydown(uint u_key_code, uint u_key_data);
	bool bMenuCommand(uint u_menu_item, HMENU hmenu);
	void SetSize(uint32 u4_size_flag, uint32 u4_client_width, uint32 u4_client_height);
	void UpdateTitle();
	void ExecuteCommand(char c_command, char* str_argument);
};

//
// Global Variables.
//
extern CWindowsShell* pwnsWindowsShell;	// Windows application shell.


#endif
