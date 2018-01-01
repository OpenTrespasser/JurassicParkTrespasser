/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Implementation of 'WinShell.hpp.'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Shell/winshell.cpp                                                   $
 * 
 * 88    9/11/98 12:38p Agrant
 * code rot
 * 
 * 87    8/23/98 3:03a Agrant
 * AI Test compile fixes
 * 
 * 86    8/22/98 6:53p Pkeet
 * Removed instance of the 'CInitDD' object.
 * 
 * 85    6/03/98 3:00p Agrant
 * init audio properly in winshell
 * 
 * 84    2/09/98 3:48p Agrant
 * Create the world after direct draw.
 * Delete the world when done with it.
 * 
 * 83    2/05/98 7:31p Agrant
 * Winshell now supports the self-initing world database.
 * 
 *********************************************************************************************/

//
// Macros, includes and definitions.
//
#include "Lib/W95/WinInclude.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "GblInc/Common.hpp"
#include "Shell/ShellResource.h"
#include "Lib/Sys/Errors.hpp"
#include "Lib/Sys/Textout.hpp"
#include "Lib/W95/dd.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Std/StringEx.hpp"
#include "Lib/View/Viewport.hpp"
#include "Lib/View/Clut.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Renderer/Camera.hpp"

#include "Lib/Audio/Audio.hpp"

//
// Shell includes.
//
#include "WinEvent.hpp"
#include "WinRenderTools.hpp"
#include "WinShell.hpp"

//
// File scope constants.
//

// Default initialization variables.
int  iDefScreenWidth  = 400;
int  iDefScreenHeight = 300;
int  iDefBitDepth     = 0;
bool bDefSystemMem	  = VER_DEBUG;

// Provide storage for the application name string.
char str_ApplicationName[] = strAPPLICATION_NAME;

//
// Foward declarations of functions included in this code module:
//

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//*********************************************************************************************
//
// CWindowsShell Implementation.
//

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	//*****************************************************************************************
	CWindowsShell::CWindowsShell(HINSTANCE hins_app_instance)
	{
		Assert(hins_app_instance != 0);

		// Initialize class variables.
		hinsAppInstance = hins_app_instance;
		hwndMain = 0;

		// Set the default pause state.
		bPause = bContinuous = false;
		bSystemMem = bDefSystemMem;
	}

	//*****************************************************************************************
	//
	// Public member functions.
	//

	//*****************************************************************************************
	void CWindowsShell::PaintWindow()
	{
		// Don't paint if the renderer has not yet been set up.
		if (prnshMain == 0)
			return;

		if (prnshMain->bBeginPaint())
		{
			// Render here.
			WindowsEvent(AM_PAINT);
			prnshMain->EndPaint();
		}
	}

	//*****************************************************************************************
	void CWindowsShell::CreateMainScreen(int i_screen_width, int i_screen_height, int i_screen_bits)
	{
		Assert(prnshMain);

		//
		// Get the client area of the main window if in windowed mode.
		//
		if (i_screen_bits == 0)
		{
			GetWindowSize(hwndMain, i_screen_width, i_screen_height);
		}

		// Set window style based on whether the application is in full screen mode or not.
		SetWindowMode(hwndMain, i_screen_bits != 0);

		if (!prnshMain->bCreateScreen(i_screen_width, i_screen_height, i_screen_bits, bSystemMem))
			return;

		WindowsEvent(AM_NEWRASTER);
	}

	//*****************************************************************************************
	bool CWindowsShell::InitializeApplication(int i_show_flag)
	{
		WNDCLASS wc_class_description;


		// Initialize DirectDraw.
		int i = g_initDD.Initialize();
		if (i != 0)
		{
			switch (i)
			{
				case -1:
					AlwaysAssert(false);
					//TerminalError(IDS_ERROR_DDRAW_NOTFOUND, FALSE, "DirectDraw Error");
					break;

				case -2:
				case -3:
				case -4:
					AlwaysAssert(false);
					//TerminalError(IDS_ERROR_DDRAW_FATAL, FALSE, "DirectDraw Error");
					break;
			}
		}


		//
		// Initialize application by filling in the WNDCLASS structure
		// and registering the main window.
		//
		wc_class_description.style         = CS_HREDRAW | CS_VREDRAW;
		wc_class_description.lpfnWndProc   = (WNDPROC)WndProc;
		wc_class_description.cbClsExtra    = 0;
		wc_class_description.cbWndExtra    = 0;
		wc_class_description.hInstance     = hinsAppInstance;
		wc_class_description.hIcon         = LoadIcon(hinsAppInstance, MAKEINTRESOURCE(ICON_LOSTWORLD));

		// With hCursor == 0, the application can change the cursor on the fly.
		wc_class_description.hCursor       = 0;
//		wc_class_description.hCursor       = LoadCursor(NULL, IDC_ARROW);

		wc_class_description.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
//		wc_class_description.hbrBackground = 0;
		wc_class_description.lpszMenuName  = MAKEINTRESOURCE(MAIN_MENU);
		wc_class_description.lpszClassName = str_ApplicationName;

		// Register the window class.
		if (!RegisterClass(&wc_class_description))
		{
			// Exit application if RegisterClass fails.
			TerminalError(ERROR_CREATEWINDOW);
			return false;
		}

#if 1
		// Create an instance of the main window.
		hwndMain = CreateWindow
		(
			str_ApplicationName,
			"The Lost World",
			0,
			CW_USEDEFAULT,
			0,
			CW_USEDEFAULT,
			0,
			0,
			0,
			hinsAppInstance,
			0
		);
		SetWindowMode(hwndMain, false);
#else
		hwndMain = CreateWindowEx
		(
			WS_EX_TOPMOST,
			str_ApplicationName,
			"The Lost World",
			WS_POPUP,
			0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			0, 0, hinsAppInstance,
			0
		);

		ShowWindow(hwndMain, i_show_flag);
		UpdateWindow(hwndMain);
#endif

		if (hwndMain == 0)
		{
			TerminalError(ERROR_CREATEWINDOW);
			return false;
		}

		// Add all DirectDraw screen modes to menu.
		SetDriverMenu(GetSubMenu(GetMenu(hwndMain), 1), 0, MENU_DRIVER_FIRST);
		SetScreenModeMenu(GetSubMenu(GetMenu(hwndMain), 2), 3, MENU_WINDOWED + 1);

		SetWindowPos(hwndMain, iDefScreenWidth, iDefScreenHeight);

		//
		// Create the CRenderEntry instance.
		//

		pcdbMain.ppceAddEntry
		(
			ppalGetPaletteFromResource
			(
				hinsAppInstance,
				IDB_DEFAULT_PALETTE
			),
			&matDEFAULT,
			true
		);


		//pceMainPalClut
//		pcdbMain.ppalAddPalette("BinData\\Palettes\\Standard.bmp", true);
		/*
		pcdbMain.ppalAddPalette
		(
			ppalGetPaletteFromResource
			(
				hinsAppInstance,
				IDB_DEFAULT_PALETTE
			),
			true
		);
		*/
		
		prnshMain = new CRenderShell(hwndMain, hinsAppInstance, false);

		// Make audio before the world dbase.
		extern void*  hwndGetMainHwnd();
		CAudio* pca_audio = ::new CAudio(hwndGetMainHwnd(), true, false);

		// Create a world database.
		pwWorld = new CWorld();

		// Set the default game pause state to 'not paused.'
		bPause = false;

		// Create the raster surface.
		CreateMainScreen(iDefScreenWidth, iDefScreenHeight, iDefBitDepth);

		WindowsEvent(AM_INIT);

		// Show and paint the main window.
		UpdateTitle();

		// Redraw the menu in case anyone added menus during AM_INIT.
		DrawMenuBar(hwndMain);
//		ShowWindow(hwndMain, SW_SHOWNORMAL);
		PaintWindow();

		return true;
	}

	//*****************************************************************************************
	//
	template<class T> class CIncrement
	//
	// Prefix: inc
	//
	// A class to automatically handle semaphore setting.  
	// Increments a flag when created, decrements it when destroyed.
	//
	//**********************************
	{
	protected:
		T& riFlag;						// The flag to increment/decrement.

	public:
		CIncrement(T& ri)
			: riFlag(ri)
		{
			riFlag++;
		}

		~CIncrement()
		{
			riFlag--;
		}

		operator int()
		{
			return riFlag;
		}
	};

	//*****************************************************************************************
	LRESULT CWindowsShell::WindowsMessageLoop(HWND hwnd_handler, uint u_message,
											  WPARAM wp_wparam, LPARAM lp_lparam)
	{
		static bool b_ignore_win_commands = 0;
		static uint u_message_last;

		//
		// Execute the main Windows message loop.
		//

		if (!b_ignore_win_commands)
		{
			int i_stupid = b_ignore_win_commands;
			CIncrement<int> inc_ignore_win_commands(i_stupid);

			u_message_last = u_message;

			switch (u_message)
			{
				case WM_COMMAND:
					if (bMenuCommand(wp_wparam, (HMENU)lp_lparam))
						return 1;
					break;

				case WM_CLOSE:
				case WM_DESTROY:
					WindowsEvent(AM_DESTROY);
					delete prnshMain;
					prnshMain = 0;

					// Add the WM_QUIT message to the Windows message loop.
					PostQuitMessage(0);
					return 0;

				// Make sure that any ERASEBKGND msg that comes is ignored.
				// We do this by returning 1.
				case WM_ERASEBKGND:
					return 1;

				// Handle the paint message.
				case WM_PAINT:
					PaintWindow();
					break;

				// Handle a change in the size of the Window.
				case WM_SIZE:
					SetSize(wp_wparam, LOWORD(lp_lparam), HIWORD(lp_lparam));
					break;

				// Handle a keypress.
				case WM_KEYDOWN:
					if (bKeydown(wp_wparam, lp_lparam))
						return 1;
					break;
			}
		}

		// Let unused messages slip back into the Windows default loop.
		return DefWindowProc(hwnd_handler, u_message, wp_wparam, lp_lparam);
	}


	//*****************************************************************************************
	//
	// CWindowsShell protected member functions.
	//

	//*****************************************************************************************
	//
	bool CWindowsShell::bKeydown
	(
		uint u_key_code,	// Virtual key code.
		uint u_key_data		// Key data.
	)
	//
	// Handles the "WM_KEYDOWN" message, a message is sent whenever a key is pressed on the
	// keyboard. If the member function does not handle the keyboard message, the keyboard
	// message will be handled by the default Windows procedure.
	//
	// Returns:
	//		Returns 'true' if the message was handled by the member function, otherwise returns
	//		'false.'
	//
	//**************************************
	{
		switch (u_key_code)
		{

			case VK_PAUSE:
				// Set the 'Pause' state based on the key pressed.
				bPause = !bPause;
				WindowsEvent(AM_PAUSED, bPause);
				UpdateTitle();
				return true;

			case VK_F11:
				WindowsEvent(AM_STEP);
				return true;
		}
		return false;
	}

	//*****************************************************************************************
	//
	bool CWindowsShell::bMenuCommand
	(
		uint u_menu_item,	// ID of menu item.
		HMENU hmenu			// Menu it came from.
	)
	//
	// Handles menu commands.
	//
	// Returns:
	//		Returns 'true' if the message was handled by the member function, otherwise returns
	//		'false.'
	//
	//**************************************
	{
		//
		// Select action based on command.
		//
		switch (u_menu_item)
		{
			/*
			case MENU_SYSTEM_MEM:
				CheckMenuItem(hmenu, u_menu_item, bSystemMem? MF_UNCHECKED : MF_CHECKED);
   				bSystemMem = !bSystemMem;
				CreateMainScreen();
				return true;
			*/

			case MENU_WINDOWED:
				CheckMenuItem(hmenu, u_menu_item, MF_CHECKED);
				CreateMainScreen();
				SetWindowPos(hwndMain, iDefScreenWidth, iDefScreenHeight);
				return true;

			// Post exit message.
			case MENU_EXIT:
				PostMessage(hwndMain, WM_CLOSE, 0, 0);
				return true;

/*
			case MENU_CONSOLE_OFF:
				prnshMain->SetConsoleState(CRenderShell::econsNONE);
				return true;

			case MENU_CONSOLE_WINDOW:
				prnshMain->SetConsoleState(CRenderShell::econsWINDOW);
				return true;

			case MENU_CONSOLE_SCREEN:
				prnshMain->SetConsoleState(CRenderShell::econsSCREEN);
				return true;

			case MENU_CONSOLE_MODE:
				prnshMain->SetConsoleState(CRenderShell::econsBY_MODE);
				return true;
*/
				
			default:
				{
					//
					// See if it's a screen-mode command.
					//
					int i_width, i_height, i_bitdepth;

					if (bSetFullScreenMode(u_menu_item - MENU_WINDOWED - 1, i_width, i_height, i_bitdepth))
					{
						// Create the new full screen raster.
						CreateMainScreen(i_width, i_height, i_bitdepth);
						PaintWindow();
						return true;
					}
				}

				// Check to see if this is a driver selection.
				if (prnshMain->bChangeRenderer(u_menu_item - MENU_DRIVER_FIRST))
				{
					return true;
				}
				return false;
		}
	}

	//*****************************************************************************************
	//
	void CWindowsShell::SetSize
	(
		uint32 u4_size_flag,		// State of the Window (ie., SW_MINIMIZE).
		uint32 u4_client_width,		// Width of the client area of the window.
		uint32 u4_client_height		// Height of the client area of the window.
	)
	//
	// Handles the "WM_SIZE" message, a message is sent whenever the Window's size is change. Note
	// that this includes when the window is initially created.
	//
	//**************************************
	{
		if (prnshMain == 0)
			return;

		//
		// Size the DirectDraw surface.
		//
		if (u4_size_flag != SIZE_MINIMIZED)
			CreateMainScreen();
	}

	//*****************************************************************************************
	//
	void CWindowsShell::UpdateTitle
	(
	)
	//
	// Puts the default text into the window caption bar.
	//
	//**************************************
	{
		//
		// Return if the Window handle has not yet been created.
		//
		if (hwndMain == 0)
		{
			return;
		}

		//
		// Set the appropriate window text based on the pause state.
		//
		if (bPause)
		{
			SetWindowText(hwndMain, strTITLE" (Game Paused)");
		}
		else
		{
			SetWindowText(hwndMain, strTITLE);
		}
	}

//*****************************************************************************************
//
void ExecuteCommand
(
	char  c_command,	// Character indicating command.
	char* str_argument	// Argument string for command.
)
//
// Executes a '/' command from the command line string.
//
//**************************************
{
	Assert(str_argument);

	switch (c_command)
	{
		case 's':
		case 'S':
			// Set system-memory flag.
			bDefSystemMem = true;
			break;

		case 'b':
		case 'B':
			// Set screen bit-depth default.
			Assert(*str_argument);
			iDefBitDepth = atoi(str_argument);
			break;

		case 'g':
		case 'G':
/*
			// Set gamma-correction table:
			Assert(*str_argument);
			{
				float f_gamma;

				int i_result = sscanf(str_argument, "%f", &f_gamma);
				if (i_result == 1)
				{
					if (f_gamma <= 0.0f)
					{
						TerminalError(ERROR_INVALID_GAMMA);
						return;
					}
					gcScreenGamma.SetGammaCorrection(f_gamma);
				}
			}
*/
			break;

		case 'h':
		case 'H':
			// Set screen height default:
			Assert(*str_argument);
			iDefScreenHeight = atoi(str_argument);
			break;

		case 'w':
		case 'W':
			// Set screen Width default:
			Assert(*str_argument);
			iDefScreenWidth  = atoi(str_argument);
			break;

		case 'z':
		case 'Z':
			// Set palette to grayscale:
			break;

		default:
			Assert(false);
	}
}

//*********************************************************************************************
//
int APIENTRY WinMain
(
	HINSTANCE hinst_instance,		// Handle to the current instance of the application.
	HINSTANCE hinst_prev_instance,	// Handle to any previous instance of the application.
	LPSTR     str_command_line,		// String containing the command line causing
									// application execution.
	int       i4_command_flag		// Flag suggesting how window be displayed. Ignored.
)
//
// The main entry point for the application. Windows automatically calls this function; this
// function in turn sets up the application instance and the main shell application.
//
// Returns.
//		The function returns the result of executing the application. Generally '0' means
//		the application terminated without an error, any other value indicates an execution
//		error.
//
//**************************************

{
	MSG msg_windows_message;

	Unreferenced(i4_command_flag);

	// Return if a previous instance of this application has been launched.
	if (hinst_prev_instance)
	{
		return false;
	}
	
	//
	// Set default screen variables.
	//

	ParseCommandLine(str_command_line, &ExecuteCommand);

	pwnsWindowsShell = new CWindowsShell(hinst_instance);

	// Perform instance initialization:
	if (!pwnsWindowsShell->InitializeApplication(i4_command_flag))
	{
		return false;
	}

	//
	// Execute main message loop.
	//
	for (;;)
	{
		// Check if there are any messages in the queue, and process them.
		while (PeekMessage(&msg_windows_message, 0, 0, 0, PM_REMOVE))
		{
			if (msg_windows_message.message == WM_QUIT)
			{
				goto quit;
			}
			
			// Send message through the main message loop.
			TranslateMessage(&msg_windows_message); 
			DispatchMessage( &msg_windows_message);

			if (prnshMain)
			{
				prnshMain->ProcessMessage(msg_windows_message);
			}
		}

		// Check if game is paused.
		if (pwnsWindowsShell->bContinuous && !pwnsWindowsShell->bPause)
		{
			// Execute one pass of the game loop.
			CCycleTimer ctmr;
			WindowsEvent(AM_STEP);
			proProfile.psFrame.Add(ctmr(), 1);
		}
		else
			WaitMessage();
	}

quit:

	delete prnshMain;
	prnshMain = 0;
	//
	// Delete instance of application and return exit code.
	//
	delete pwnsWindowsShell;
	delete pwWorld;

	return (msg_windows_message.wParam);
}


//
// File scope functions.
//

//*********************************************************************************************
//
LRESULT CALLBACK WndProc
(
	HWND h_wnd,			// The handle of the window handling the message.
	uint u_message,		// The message ID, usually based on a #define in <Windows.h>.
	WPARAM wp_wparam,	// The first descriptive parameter passed with the windows message.
	LPARAM lp_lparam	// The second descriptive parameter passed with the windows message.
)
//
//  This procedure applies the main Windows message loop to the class
//  CWindowsShell.
//
// Returns:
//		Returns the result of the action of the windows message. This is highly
//		dependant on the windows message, but is usually a boolean "true"
//		reflecting a successful operation.
//
//**************************************
{
	// Transfer the WndProc callback to pwnsWindowsShell.
	LRESULT lres_retval = pwnsWindowsShell->WindowsMessageLoop(h_wnd, u_message, wp_wparam, lp_lparam);
	WindowsEvent(u_message, wp_wparam, lp_lparam);
	return lres_retval;
}


//
// Global Variables.
//
CWindowsShell*  pwnsWindowsShell = 0;
