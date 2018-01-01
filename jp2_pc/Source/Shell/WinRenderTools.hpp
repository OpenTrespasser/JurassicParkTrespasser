/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents: Tools for any Windows shell to use with the renderer.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Shell/WinRenderTools.hpp                                             $
 * 
 * 10    97/02/05 19:44 Speter
 * Removed WinMix and MIX types.  Changed interface to screen menu and driver menu calls.
 * 
 * 9     96/12/09 16:13 Speter
 * Removed ConShowStats() (replaced by CProfile functionality).
 * 
 * 8     11/07/96 5:58p Agrant
 * moved ppalGetPaletteFromResource() to WinRenderTools.cpp(.hpp)
 * so that it was accessible outside of GUIApp.
 * 
 * 7     96/10/18 18:29 Speter
 * Changed SetDefaultWindowPos to SetWindowPos, taking parameters.
 * Made ConShowStats() operate only when console is shown.
 * 
 * 6     96/10/04 17:02 Speter
 * Added ConShowStats function, moved code from TestPipeline.
 * 
 * 5     96/08/19 13:11 Speter
 * Added SetDriverMenu.
 * Changed SetDirectDrawMenu to SetScreenModeMenu.
 * 
 * 4     8/12/96 6:21p Pkeet
 * Added comments.
 * 
 * 3     7/31/96 10:36a Pkeet
 * Added 'WinMix' includes.
 * 
 * 2     7/18/96 2:37p Pkeet
 * Finished initial representation.
 * 
 * 1     7/17/96 7:32p Pkeet
 * 
 *********************************************************************************************/

#ifndef HEADER_SHELL_WINRENDERTOOLS_HPP
#define HEADER_SHELL_WINRENDERTOOLS_HPP

//
// Includes.
//

#include "Lib/W95/WinAlias.hpp"

#include "Lib/View/Palette.hpp"





//
// Global functions.
//

//*****************************************************************************************
//
bool bSetFullScreenMode
(
	int  i_dd_mode,
	int& i_width,
	int& i_height,
	int& i_bitdepth
);
//
//**************************************

//*****************************************************************************************
//
void SetWindowMode
(
	HWND	hwnd,		// Handle to the window.
	bool    b_is_fullscreen	// Set to 'true' if the application is in full screen mode.
);
//
// Sets the appropriate style for a window based on its direct draw mode.
//
//**************************************

//*****************************************************************************************
//
void SetScreenModeMenu
(
	HMENU	hmenu,				// Menu on which to put screen modes.
	int		i_itempos,			// Position to insert items.
	int		i_idstart			// ID number of first item to insert.
);
//
// Sets up the DirectDraw screen mode menu.
//
//**************************************

//*****************************************************************************************
//
void SetDriverMenu
(
	HMENU	hmenu,				// Menu on which to put drivers.
	int		i_itempos,			// Position to insert items.
	int		i_idstart			// ID number of first item to insert.
);
//
// Sets up the Driver menu.
//
//**************************************

//*********************************************************************************************
//
void ParseCommandLine
(
 	char* strCommandLine,	// Command line to parse.
	void (__cdecl *execute_command)(char  c_command, char* str_argument)
							// Execute command callback.
);
//
// Splits the command line into separate command strings, and processes each of the strings
// in WinShell.
//
//**************************************

//*********************************************************************************************
//
void GetWindowSize
(
	HWND	hwnd,		// Handle of the window.
 	int&    i_width,	// Width of the client area of the screen.
	int&    i_height	// Height of the client area of the screen.
);
//
// Finds the width and height of the client area of a window.
//
//**************************************

//*********************************************************************************************
//
void SetWindowPos
(
	HWND hwnd,		// Handle of the window.
	int i_width = 640,
	int i_height = 480
);
//
//**************************************


//**********************************************************************************************
//
CPal* ppalGetPaletteFromResource
(
	HINSTANCE hinstance,	// Instance of module containing resource.
	int       i_id			// ID of bitmap resource.
);
//
// Returns a palette generated from a resource bitmap.
//
//**************************************


#endif
