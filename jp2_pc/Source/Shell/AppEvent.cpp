/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of AppEvent.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Shell/AppEvent.cpp                                                    $
 * 
 * 4     96/11/08 18:46 Speter
 * Added handler for WM_COMMAND message, so that apps can receive menu commands.
 * 
 * 3     96/11/06 11:48 Speter
 * Reordered data to make WindowsEvent overridable again.  Now AppShell handles the key states,
 * and WindowsEvent calls CAppShell::RawKeyPress.
 * 
 * 2     96/11/05 17:44 Speter
 * Added bKeyState function, and code to keep track of which keys/buttons are pressed, because
 * Windows seems to have a hard time doing so.
 * 
 * 1     96/10/30 15:19 Speter
 * Resurrected AppEvent.cpp (but not AppEvent.hpp), moved WindowsEvent() from AppShell.cpp to
 * AppEvent.cpp.  This allows WindowsEvent to be redefined in another module
 * 
 * 6     7/19/96 10:58a Pkeet
 * Changed include files for shell files moved to 'Shell.'
 * 
 * 5     7/16/96 11:29a Mlange
 * Moved several files to different directories, updated includes.
 * 
 * 4     96/05/22 19:36 Speter
 * Created AM_INIT event, made WinShell::Create() generate it.
 * 
 * 3     96/05/17 12:04 Speter
 * Added Windows VK_ definitions.
 * Added bKeyState() function.
 * Replaced MousePress() with KeyPress(VK_LBUTTON), etc.
 * 
 * 2     96/05/16 18:19 Speter
 * Added a lot of comments.
 * 
 * 1     96/05/16 16:47 Speter
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"

//
// Shell includes.
//
#include "WinEvent.hpp"
#include "AppShell.hpp"
#include "winshell.hpp"

//******************************************************************************************
//
void WindowsEvent(uint u_message, WPARAM wp_param, LPARAM lp_param)
//
// Default implemention of the WindowsEvent handler function.
// This implementation decodes the Windows events, and calls pappMain's handler functions.
//
// Notes:
//		This function is designed to be overridable by other modules which link to the
//		WinShell library.  For some reason, it is necessary to place this function in a
//		file by itself.  If it is placed in AppShell.cpp, with other code, then apparently,
//		if another module uses any code in AppShell, WindowsEvent is linked in as well,
//		and cannot be redefined, or it causes a link error.  However, when in a file by
//		itself, WindowsEvent can be overridden.
//
//**************************************
{
	switch (u_message)
	{
		// Control flow events.
		case AM_INIT:
			pappMain->Init();
			break;
		case WM_DESTROY:
			pappMain->Exit();
			break;
		case AM_PAUSED:
			pappMain->Paused(wp_param);
			break;
		case AM_STEP:
			pappMain->Step();
			break;

		// Raster events.
		case AM_PAINT:
			pappMain->Paint();
			break;
		case AM_NEWRASTER:
			pappMain->NewRaster();
			break;
		case WM_SIZE:
			pappMain->Resized();
			break;

		//
		// User input events.
		//

		// Keyboard.
		case WM_KEYDOWN:
			pappMain->RawKeyPress(wp_param);
			break;
		case WM_KEYUP:
			pappMain->RawKeyPress(-(int)wp_param);
			break;

		// Mouse buttons.
		case WM_LBUTTONDOWN:
			pappMain->RawKeyPress(VK_LBUTTON);
			break;
		case WM_LBUTTONUP:
			pappMain->RawKeyPress(-(int)VK_LBUTTON);
			break;

		case WM_MBUTTONDOWN:
			pappMain->RawKeyPress(VK_MBUTTON);
			break;
		case WM_MBUTTONUP:
			pappMain->RawKeyPress(-(int)VK_MBUTTON);
			break;

		case WM_RBUTTONDOWN:
			pappMain->RawKeyPress(VK_RBUTTON);
			break;
		case WM_RBUTTONUP:
			pappMain->RawKeyPress(-(int)VK_RBUTTON);
			break;

		// Mouse movement.
		case WM_MOUSEMOVE:
			pappMain->MouseMove(LOWORD(lp_param), HIWORD(lp_param));
			break;

		// Menu commands.
		case WM_COMMAND:
		{
			HMENU hmenu = GetMenu(pwnsWindowsShell->hwndMain);
			int i_id = wp_param;

			// See if this is a checked item.
			MENUITEMINFO minf;
			minf.cbSize = sizeof(MENUITEMINFO);
			minf.fMask = MIIM_DATA | MIIM_STATE;
			Verify(GetMenuItemInfo(hmenu, i_id, false, &minf));

			// Toggle the state variable.
			bool *pb_variable = (bool*)minf.dwItemData;
			if (pb_variable)
				*pb_variable = !*pb_variable;

			// Let the app handle the command.
			pappMain->MenuCommand(i_id, pb_variable);

			if (pb_variable)
				// Set the menu check mark based on new state.
				CheckMenuItem(hmenu, i_id, *pb_variable ? MF_CHECKED : MF_UNCHECKED);
			break;
		}
	}
};

