/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of AppShell.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Shell/AppShell.cpp                                                    $
 * 
 * 14    4/28/98 11:31p Agrant
 * Porting code for world database reset
 * 
 * 13    2/05/98 7:31p Agrant
 * More porting code for world database
 * 
 * 12    96/11/08 18:45 Speter
 * Added AddMenuItem(), hmenuAddSubMenu(), and bContinuous() functions.
 * 
 * 11    96/11/06 11:48 Speter
 * Reordered data to make WindowsEvent overridable again.  Now AppShell handles the key states,
 * and WindowsEvent calls CAppShell::RawKeyPress.
 * 
 * 10    96/11/05 17:43 Speter
 * Changed Repaint back to old version, where it explicitly calls WinShell's PaintWindow.
 * Hand bKeyState() call off to function in AppEvent.
 * 
 * 9     96/10/30 15:18 Speter
 * Resurrected AppEvent.cpp (but not AppEvent.hpp), moved WindowsEvent() from AppShell.cpp to
 * AppEvent.cpp.  This allows WindowsEvent to be redefined in another module
 * 
 * 8     96/10/28 14:51 Speter
 * Merged AppEvent module into AppShell.  Now, both regular and virtual shell-handling functions
 * are handled through single CAppShell class.
 * 
 * 7     96/10/25 14:33 Speter
 * Changed repainting strategy.  Now repaint on WM_PAINT message, ignore WM_ERASEBKGROUND.
 * AppShell::Repaint() function invalidates the window, generating WM_PAINT.  This allows
 * Repaint() to be called during any handler function.
 * 
 * 6     7/19/96 10:58a Pkeet
 * Changed include files for shell files moved to 'Shell.'
 * 
 * 5     7/18/96 3:16p Pkeet
 * Altered to rely on 'Render.hpp' instead of 'WinShell.hpp.'
 * 
 * 4     7/16/96 11:29a Mlange
 * Moved several files to different directories, updated includes.
 * 
 * 3     96/05/30 17:48 Speter
 * Added HideCursor function.
 * 
 * 2     96/05/16 18:19 Speter
 * Added separate version of TerminateShell to namespace.
 * Corrected some comments.
 * 
 * 1     96/05/16 16:47 Speter
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "AppShell.hpp"

#include "AppEvent.hpp"
#include "winshell.hpp"
#include "Lib/Sys/W95/Render.hpp"

//**********************************************************************************************
//
// CAppShell implementation.
//

	static char acKeyState[256];
	static CPArray<char> pacKeyState(256, acKeyState);

	//**********************************************************************************************
	void CAppShell::SetContinuous(bool b_continuous)
	{
		pwnsWindowsShell->bContinuous = b_continuous;
	}

	//**********************************************************************************************
	bool CAppShell::bContinuous()
	{
		return pwnsWindowsShell->bContinuous && !pwnsWindowsShell->bPause;
	}

	//**********************************************************************************************
	void CAppShell::HideCursor(bool b_hide)
	{
		//pwnsWindowsShell->HideCursor(b_hide);
	}

	//**********************************************************************************************
	void CAppShell::Repaint()
	{
		pwnsWindowsShell->PaintWindow();
	}

	//**********************************************************************************************
	void CAppShell::TerminateShell()
	{
		// Call the WinShell version.
		::TerminateShell();
	}

	//**********************************************************************************************
	void CAppShell::RawKeyPress(int i_key)
	// Store the state of the key, then call the virtual function KeyPress.
	{
		if (i_key > 0)
			pacKeyState[i_key] = 1;
		else
			pacKeyState[-i_key] = 0;
		KeyPress(i_key);
	}

	//**********************************************************************************************
	bool CAppShell::bKeyState(int i_key)
	{
		//
		// It would be nice if the GetKeyState() function worked, but like most things in Windows,
		// it doesn't.
		//
		return pacKeyState[i_key];
	}


	//******************************************************************************************
	void CAppShell::AddMenuItem(HMENU hmenu, char* str_text, int i_id, bool* pb_variable)
	{
		if (!hmenu)
			hmenu = GetMenu(pwnsWindowsShell->hwndMain);

		Verify(AppendMenu
		(
			hmenu,
			str_text ? (MF_STRING | (pb_variable && *pb_variable ? MF_CHECKED : 0)) : MF_SEPARATOR,
			i_id,
			str_text
		));

		// Remember the variable in the menu item info.
		MENUITEMINFO minf;
		minf.cbSize = sizeof(MENUITEMINFO);
		minf.fMask = MIIM_DATA;
		minf.dwItemData = (DWORD) pb_variable;

		Verify(SetMenuItemInfo(hmenu, i_id, false, &minf));
	}

	//******************************************************************************************
	HMENU CAppShell::hmenuAddSubMenu(HMENU hmenu, char* str_text)
	{
		if (!hmenu)
			hmenu = GetMenu(pwnsWindowsShell->hwndMain);

		HMENU hmenu_sub = CreateMenu();
		Assert(hmenu_sub);

		Verify(AppendMenu
		(
			hmenu,
			MF_POPUP | MF_STRING,
			(uint) hmenu_sub,
			str_text
		));

		return hmenu_sub;
	}


	// Porting code.
	void* hwndGetMainHwnd()
	{
		AlwaysAssert(pwnsWindowsShell);
		return pwnsWindowsShell->hwndMain;
	}

	// Porting code.
	HINSTANCE hinstGetMainHInstance()
	{
		AlwaysAssert(pwnsWindowsShell);
		return pwnsWindowsShell->hinsAppInstance;
	}

	// Porting code.
void ResetAppData()
{
	// Clears all data that needs clearing on a world dbase reset.
}



