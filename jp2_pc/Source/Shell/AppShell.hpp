/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		namespace AppShell
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Shell/AppShell.hpp                                                   $
 * 
 * 9     96/12/31 17:08 Speter
 * Updated for rptr.
 * 
 * 
 * 8     96/11/08 18:45 Speter
 * Added AddMenuItem(), hmenuAddSubMenu(), and bContinuous() functions.
 * 
 * 7     96/11/06 11:48 Speter
 * Reordered data to make WindowsEvent overridable again.  Now AppShell handles the key states,
 * and WindowsEvent calls CAppShell::RawKeyPress.
 * 
 * 6     96/10/30 15:18 Speter
 * Resurrected AppEvent.cpp (but not AppEvent.hpp), moved WindowsEvent() from AppShell.cpp to
 * AppEvent.cpp.  This allows WindowsEvent to be redefined in another module
 * 
 * 5     96/10/28 14:51 Speter
 * Merged AppEvent module into AppShell.  Now, both regular and virtual shell-handling
 * functions are handled through single CAppShell class.
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
 * 2     96/05/13 18:45 Speter
 * Added comments to function prototypes.
 * 
 * 1     96/05/08 20:10 Speter
 * Interface to module which executes specific game code.
 * 
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_SYS_APPSHELL_HPP
#define HEADER_LIB_SYS_APPSHELL_HPP

#include "Lib/View/RasterVid.hpp"
#include "Lib/W95/WinAlias.hpp"

//
// Duplicate most Windows VK_ symbols from <winuser.h>.  Sorry.
//

#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */

#define VK_BACK           0x08
#define VK_TAB            0x09

#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D

#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14

#define VK_ESCAPE         0x1B

#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F

/* VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
/* VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */

#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B

#define VK_NUMLOCK        0x90
#define VK_SCROLL         0x91

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5

//
// A few aliases which should be used in preference to the Windows symbols.
//

#define VK_PGUP		VK_PRIOR
#define VK_PGDN		VK_NEXT

#define VK_ALT		VK_MENU
#define VK_LALT		VK_LMENU
#define VK_RALT		VK_RMENU


#define iMENU_ID_START	16000

//**********************************************************************************************
//
class CAppShell
//
// Prefix: app
//
// An abstract class which defines an interface for handling events, and also provides a few
// key functions to control the flow of the application.  These functions interface with 
// WinShell.cpp.  However, WinShell.hpp is not needed to use this module.
//
// This class is used by defining a descendent of it which implements its virtual functions,
// and then defining the global variable CAppShell* TVP, and initialising it to an
// instance of the descendent class.
//
// Only the virtual functions that the application wants to handle need be overridden.
// All the virtual functions do nothing by default.
//
// Example:
//		class CAppShellTest: public CAppShell
//		// Implement the CAppShell interface.
//		{
//			// Override all virtual functions I want to handle.
//			void Paint()
//			{ 
//				// Paint prasMainScreen.
//			}
//			void Step()
//			{
//				// Execute one iteration of the game.
//			}
//		};
//
//		// Now define the global var that AppEvent's WindowsEvent handler will pass 
//		// messages through.
//		CAppShell* pappMain = new CAppShellTest;
//
// Cross-references.
//		The global variable pappMain is accessed by the WindowsEvent function defined in
//		AppEvent.cpp.  If WindowsEvent is then not redefined outside the library, that
//		version will be used.
//
// Prefix: eh
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Member functions.
	//

	//**********************************************************************************************
	//
	static void SetContinuous
	(
		bool b_continuous					// Whether app is in a continuous or event-driven state.
	);
	//
	// Set whether the app is to run continuously.
	//
	// Notes:
	//		When the app is in continuous mode, and the user has not paused the app, WinShell calls 
	//		WindowsEvent(AM_STEP) event repeatedly.  Otherwise, the app is event-driven, and gets 
	//		control only via WinShell calling WindowsEvent() with other events.
	//
	//
	//**************************************

	//**********************************************************************************************
	//
	static bool bContinuous();
	//
	// Returns:
	//		Whether the app is to run continuously.
	//
	// Notes:
	//		This returns true only if both the app has been SetContinuous(true), and the user
	//		has not paused the app.
	//
	//**************************************

	//**********************************************************************************************
	//
	static void HideCursor
	(
		bool b_hide						// Whether to hide the cursor.
	);
	//
	//**************************************

	//**********************************************************************************************
	//
	static void Repaint();
	//
	// Causes prasMainScreen to be repainted, using the WindowsEvent(AM_PAINT) callback.
	//
	// Cross-references:
	//		Causes WinShell to do the paint pre- and post-setup, then call WindowsEvent(AM_PAINT);
	//
	//**************************************

	//**********************************************************************************************
	//
	static void TerminateShell();
	//
	// Exits the program.
	//
	//**************************************

	//******************************************************************************************
	//
	void RawKeyPress
	(
		int i_key						// Keycode to process.
	);
	//
	// Called by WindowsEvent to process key inputs.  Should not be called by anyone else.
	//
	//**********************************

	//******************************************************************************************
	//
	bool bKeyState
	(
		int i_key						// Keycode to check.
	);
	//
	// Returns:
	//		Whether the i_key was pressed at the time the last KeyPress() event was generated.
	//
	// Notes:
	//		To check for modifier keys, you can pass either VK_LSHIFT, VK_RSHIFT, etc. to check
	//		specifically for left and right keys, or VK_SHIFT, etc. to check for either left or
	//		right.
	//
	//**********************************

	//******************************************************************************************
	//
	void AddMenuItem
	(
		HMENU	hmenu,					// Menu to add to, or 0 for the main menu.
		char*	str_text,				// Text of menu item, or 0 for separator.
		int		i_id,					// Command ID of item.
		bool*	pb_variable = 0			// Address of boolean variable to hold item state.
	);
	//
	// Adds an item of the given text and id to the end of hmenu.
	//
	//**********************************

	//******************************************************************************************
	//
	HMENU hmenuAddSubMenu
	(
		HMENU	hmenu,					// Menu to add to.
		char*	str_text				// Text of menu item.
	);
	//
	// Adds a submenu of the given text to the end of hmenu.
	//
	// Returns:
	//		The HMENU handle of the new submenu.  Can be passed to AddMenuItem to add items
	//		to the submenu.
	//
	//**********************************

	//******************************************************************************************
	//
	// Overridable virtual functions.
	//

	//******************************************************************************************
	//
	// Control flow events.
	//

	//******************************************************************************************
	//
	virtual void Init()
	//
	// Called upon application initialisation.
	//
	//**********************************
	{
	}

	//******************************************************************************************
	//
	virtual void Exit()
	//
	// Called just before app's window is to be destroyed.
	//
	//**********************************
	{
	}

	//******************************************************************************************
	//
	virtual void Step()
	//
	// Called repeatedly whenever app is in continuous mode (see AppShell).
	//
	//**********************************
	{
	}

	//******************************************************************************************
	//
	virtual void Paused
	(
		bool b_paused					// True if application is paused, else false.
	)
	//
	// Called whenever user has paused or unpaused the app.
	//
	// Notes:
	//		When the app is paused, it will not receive Step() events even if in continuous mode.
	//		The app cannot pause or unpause itself; this function merely informs it that the user
	//		has done so.
	//
	//**********************************
	{
	}

	//**********************************************************************************************
	//
	// Raster events.
	//

	//******************************************************************************************
	//
	virtual void Paint()
	//
	// Called when the app should redraw prasMainRaster.
	//
	// Notes:
	//		WinShell takes care of raster pre- and post-setup.  The app should just draw.
	//
	//**********************************
	{
	}

	//******************************************************************************************
	//
	virtual void NewRaster()
	//
	// Called when the raster is created, and whenever its screenmode changes.
	// Look at prasMainScreen to find the new mode.
	//
	//**********************************
	{
	}

	//******************************************************************************************
	//
	virtual void Resized()
	//
	// Called when the raster has been resized, but is in the same screen mode.
	// Look at prasMainScreen to find the new size.
	//
	//**********************************
	{
	}

	//**********************************************************************************************
	//
	// Input events.
	//

	//******************************************************************************************
	//
	virtual void KeyPress
	(
		int i_key						// Keycode, ASCII or VK_ symbol.
										// Positive for keypress, negative for release.
	)
	//
	// Called upon a key or mouse button press or release.
	//
	// Notes:
	//		The Windows key values are used as is.  Alphabetic keys are always passed as
	//		uppercase ASCII, regardless of the Shift key state.
	//
	//		Modifiers are not included as part of the i_key code.  To check the state of
	//		any key, call bKeyState().
	//
	//**********************************
	{
	}

	//******************************************************************************************
	//
	virtual void MouseMove
	(
		int i_x, int i_y				// Mouse screen position.
	)
	//
	// Called when the mouse moves.
	//
	//**********************************
	{
	}

	//******************************************************************************************
	//
	virtual void MenuCommand
	(
		int i_command,					// ID of menu command.
		bool* pb_variable				// Boolean variable indicating state, if any.
	)
	//
	// Called when the user chooses a menu command.
	//
	//**********************************
	{
	}

};

//
// The global CAppShell pointer that AppEvent's WindowsEvent handler communicates with.
// NOT defined in this module.  Must be defined in another module.
//
// Example:
//		CAppShell* pappMain = new CAppShellTest;
//

extern CAppShell* pappMain;

//
// Duplicate declarations of a few key items in WinShell.cpp that are generally useful to the world,
// and non-Windows-specific.
//

extern rptr<CRasterWin> prasMainScreen;


#endif
