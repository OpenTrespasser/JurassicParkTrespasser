/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Defines class CRenderShell. CRenderShell encapsulates the basic functionality of the Lost World
 *		renderer.
 *
 * Bugs:
 *
 * To do:
 *		Make 'TerminateShell' fully operational. Move global variables to the class.
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/W95/Render.hpp                                               $
 * 
 * 37    9/04/98 8:59p Shernd
 * Setting Viewport changes
 * 
 * 36    8/25/98 2:33p Rvande
 * removed redundant class scope
 * 
 * 35    7/30/98 5:55p Pkeet
 * Added the 'hwnd' parameter.
 * 
 * 34    7/13/98 12:06a Pkeet
 * Added the new 'AdjustViewport' member functions. The screen viewport is now resized without
 * recreating the screen every time.
 * 
 * 33    3/06/98 2:09p Pkeet
 * Made 'rensetSettings' public so that it can be modified dynamically outside the class.
 * 
 * 32    98.01.22 8:45p Mmouni
 * Added simple routine to print a text string.
 * 
 * 31    12/08/97 6:40p Rwyatt
 * New members to get and set the screen properties
 * 
 * 30    97/10/06 11:00 Speter
 * Cleaned up some dependencies.  Moved fogFog and pcdbMain from here to appropriate modules.
 * 
 * 29    97-03-28 15:55 Speter
 * Renamed CRender to CRenderShell, preMain to prnshMain; changed RenderSettings to
 * rensetSettings.
 * Removed psrMain, replaced with prenMain.
 * 
 * 28    97/03/17 14:30 Speter
 * Removed strCommandArg global var (now done via ExecuteCommand).
 * 
 * 27    97/02/05 20:05 Speter
 * Removed EConsole members and CreateTextDialog (superseded by GUIAppDlg functionality).
 * Removed some includes.
 * 
 * 26    97/02/05 19:42 Speter
 * Removed WinMix and MIX types.
 * 
 * 25    97/01/16 11:56 Speter
 * Updated for CProfile changes.  CRender now contains an SRenderSettings structure, allowing
 * easier persistence when changing screen renderers.
 * 
 * 24    96/12/31 17:07 Speter
 * Updated for rptr.
 * 
 * 23    12/20/96 12:10p Mlange
 * Moved fog source files to 'Renderer' directory.
 * 
 * 22    96/12/17 13:14 Speter
 * Added econsMode variable.  econsState is now the effective state of the console.
 * Moved psrMain global variable to ScreenRender.cpp.
 * 
 * 
 * 21    96/12/09 21:08 Speter
 * Made ShowConsoleOnScreen() function public.  Now always shows console; test for econsState
 * must be done by caller.
 * Moved psFrame increment to GUIAppDlg.
 * 
 * 20    96/12/09 16:07 Speter
 * Removed some timings, converted others to new CProfile.
 * 
 * 19    11/23/96 5:51p Pkeet
 * Made the 'pcdbMain' a real variable again because the InitPtr stuff prevented the clut and
 * palette destructors from being called.
 * 
 * 18    10/28/96 7:12p Pkeet
 * Removed the global viewport variable.
 * 
 * 17    96/10/18 18:21 Speter
 * econsState now a public member.  Updated for CViewport changes.
 * 
 * 
 * 16    96/10/04 16:59 Speter
 * Changed pcdbMain to a pointer, and added an InitPtr var to ensure it's initialised.
 * 
 * 15    96/08/22 17:22 Speter
 * Changed bIgnoreWinCommands to int iIgnoreWinCommands, now allows stacking.
 * 
 * 14    8/20/96 4:46p Pkeet
 * Added 'fogFog' global variable.
 * 
 * 13    96/08/19 12:36 Speter
 * Added bChangeRenderer function, allowing dynamic switching of rasterisers.
 * 
 * 12    8/14/96 3:08p Pkeet
 * Made 'bIsFullScreen' a public data member.
 * 
 * 11    96/08/09 11:01 Speter
 * Changes for new CScreenRender class.  Z buffer now created by CScreenRender.
 * 
 * 10    96/08/07 20:41 Speter
 * Changed bIgnoreDDCommands to bIgnoreWinCommands, made it public because WinShell needs to
 * check it.
 * 
 * 9     96/08/05 12:25 Speter
 * Added b_system_mem flag to CreateScreen().
 * 
 * 8     7/31/96 5:08p Pkeet
 * Added 'b_show_textdialog' flag to CRender constructor.
 * 
 * 7     96/07/31 15:37 Speter
 * Added code to generate timing stats for overall frame-rate.
 * Changed CreateScreen function to return a success value.
 * 
 * 6     7/31/96 10:31a Pkeet
 * Added 'WinMix' includes.
 * 
 * 5     96/07/29 14:49 Speter
 * Added comments.
 * 
 * 4     96/07/29 14:43 Speter
 * Added code to maintain console window in both dialog and overlay states.
 * 
 * 3     7/25/96 10:38a Pkeet
 * Changed 'CPalette' to 'CPal.'
 * 
 * 2     7/18/96 3:14p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_SYS_W95_RENDER_HPP
#define HEADER_LIB_SYS_W95_RENDER_HPP

//
// Macros, includes and definitions.
//

#include "Lib/W95/WinAlias.hpp"
#include "Lib/View/RasterVid.hpp"
#include "Lib/Renderer/PipeLine.hpp"

class CConsoleBuffer;

//
// Class definitions.
//

//*********************************************************************************************
//
class CRenderShell
//
// Class provides glue between the renderer and the controlling application shell.
//
// Prefix: rnsh
//
//**************************************
{

public:
	
	HWND      hwndMain;				// Window handle.
	HINSTANCE hinsAppInstance;		// Handle of the application instance.

	int		iIgnoreWinCommands;		// Non-zero if no window event processing should be done.
									// Shell can set or examine this flag.
	bool	bIsFullScreen;			// Set to 'true' if the application is running in full screen mode.
	uint	uCurrentDriver;			// Driver used for rendering.
	CRenderer::SSettings rensetSettings;// The current settings, used across drivers.

protected:
	
	bool			bActiveApp;		// Set to 'true' if the application is the active application.

	int		iLastScreenBits;		// the last settings passed to bCreateScreen
	bool	bLastSystemMem;


public:

	//*****************************************************************************************
	//
	// CRenderShell constructor and destructor.
	//

	//*****************************************************************************************
	//
	CRenderShell
	(
		HWND      hwnd,
		HINSTANCE hins,
		bool      b_show_textdialog = true
	);
	//
	// Constructs the instance of CRenderShell for the application.
	//
	//**************************************

	//*****************************************************************************************
	//
	~CRenderShell
	(
	);
	//
	// Closes and destroys all allocations associated with CRenderShell.
	//
	//**************************************


	//*****************************************************************************************
	//
	// CRenderShell member functions.
	//

	//*****************************************************************************************
	//
	bool bCreateScreen
	(
		int i_screen_width,			// Width of desired surface.
		int i_screen_height,		// Height of desired surface.
		int i_screen_bits,			// Bit depth of desired surface.
		bool b_system_mem = false	// Force video surface in system memory.
	);
	//
	// Creates a direct draw surface given a width, height and bitdepth. If the bitdepth is
	// set to zero, the application will run in windowed mode and use the window dimensions
	// and bitdepth.
	//
	// Returns:
	//		Success.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AdjustViewport
	(
		int  i_view_width,
		int  i_view_height,
		HWND hwnd = 0,
        bool bRedraw = true
	);
	//
	// Adjust the current viewport.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AdjustViewportRelative
	(
		float f_resize_amount,
		HWND  hwnd = 0
	);
	//
	// Adjust the current viewport's with and height by a relative amount.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AdjustViewportRelativeHorizontal
	(
		float f_resize_amount,
		HWND  hwnd = 0
	);
	//
	// Adjust the current viewport's width only by a relative amount.
	//
	//**************************************

	//*****************************************************************************************
	//
	void AdjustViewportRelativeVertical
	(
		float f_resize_amount,
		HWND  hwnd = 0
	);
	//
	// Adjust the current viewport's height only by a relative amount.
	//
	//**************************************


	//*****************************************************************************************
	//
	bool bChangeRenderer
	(
		uint u_driver				// Which driver to use.
	);
	//
	// Deletes current renderer, and creates new one, indexing sapRenderDesc with u_driver.
	//
	// Returns:
	//		Success (whether u_driver is in valid range).
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bBeginPaint
	(
	);
	//
	// Initializes the render surfaces for painting by locking the appropriate surfaces.
	//
	// Returns:
	//		Returns 'true' if the surfaces are valid for painting, otherwise returns 'false.'
	//
	//**************************************

	//*****************************************************************************************
	//
	void EndPaint
	(
	);
	//
	// Unlocks the surfaces after painting and flips the display page to the screen. This
	// function should only be called after a 'true' is returned from the 'bBeginPaint'
	// function.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ProcessMessage
	(
		MSG& msg
	);
	//
	// Acts on Windows messages relevent to CRenderShell. This member function should be inserted
	// into the calling application's main message loop.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ShowConsoleOnScreen
	(
		CConsoleBuffer& con					// The console to show.
	);
	//
	// Display the console contents overlaid on the main screen.
	//
	//**************************************

	//*********************************************************************************************
	void PrintString(char *cstr);
	//
	// Displays a string in the top left corner of the render area.
	//
	//**************************************

protected:

	//*****************************************************************************************
	//
	void SetPaletteUse
	(
		bool b_use_full_palette
	);
	//
	// If the application is running with a screen mode using a palette, this function will
	// determine if 254 palette colours are used or 236.
	//
	//**************************************
};


//
// Function prototypes.
//

//*********************************************************************************************
//
void TerminateShell
(
	bool b_terminal_error = false	// Set to 'true' if the request is made by 'TerminalError,'
									// otherwise 'false.'
);
//
//  Will destruct the "pwnsWindowsShell" object properly.
//
//**************************************


//
// Global Variables.
//

extern CRenderShell*		prnshMain;

extern rptr<CRasterWin>		prasMainScreen;		// Main view raster.

#endif
