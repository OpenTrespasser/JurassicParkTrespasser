/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	 	CStdDialog
 *			CDialogDraw
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/StdDialog.hpp                                                 $
 * 
 * 3     97/10/07 18:10 Speter
 * Made file includable by other than StdDialog.cpp.  Made ProcessMessage member not PASCAL etc.
 * Made BroadcastToDialogs a class static, not global.  Made LogFontDefault a global (used only
 * in .cpp).
 * 
 * 2     97/10/06 10:57 Speter
 * Added str_title parameter to CStdDialog; added constructor which extracts HWND params from
 * CRenderShell.  Added embryo of CDialogDraw class.
 * 
 * 1     97/04/03 1:40p Pkeet
 * Initial implementation.
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_SYS_STDDIALOG_HPP
#define HEADER_LIB_SYS_STDDIALOG_HPP

#include "Lib/W95/WinAlias.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/View/LineDraw.hpp"

//*********************************************************************************************
//
// Forward class declarations.
//
class CConsoleEx;

//
// Class definitions.
//

//*********************************************************************************************
//
class CStdDialog
//
// Prefix: dlg
//
// Standard dialog for use with console buffers. Replaces the MFC version.
//
//**************************************
{
private:

	CConsoleEx* pconConsole;	// Pointer to the console that uses this dialog.
	HWND        hwndWinHandle;	// Handle for this window.
	HWND        hwndParent;		// Handle for the parent window.
	bool        bVisible;		// Visibility flag, is set to 'true' if the window is visible.

public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	CStdDialog
	(
		const char* str_title,		// String to set window title to.
		HINSTANCE   hinst_app,		// Instance handle.
		HWND        hwnd_parent,	// Parent window handle.
		CConsoleEx* pcon = 0		// Console that owns the dialog.
	);
	
	// Constructor.
	CStdDialog
	(
		const char* str_title,		// String to set window title to.
		const CRenderShell* prnsh = prnshMain,
									// Render shell (provides instance and main window).
		CConsoleEx* pcon = 0		// Console that owns the dialog.
	);
	
	
	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void ShowWindow
	(
		bool b_visible	// Flag to set visibility to.
	);
	//
	// If the supplied parameter is 'true' the window will be made visible, otherwise the
	// window will be hidden.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bIsVisible
	(
	) const;
	//
	// Returns 'true' if the dialog is visible.
	//
	//**************************************

	//*****************************************************************************************
	//
	uint ProcessMessage
	(
		UINT   u_msg,		// ID of message.
		WPARAM wp_param,	// First parameter describing the message.
		LPARAM lp_param		// Second parameter describing the message.
	);
	//
	// Handles the message sent to the window.
	//
	// Returns success state of the operation.
	//
	//**************************************

	//*****************************************************************************************
	//
	HWND hwndGet
	(
	) const;
	//
	// Returns the window's handle.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetWindowText
	(
		const char* str	// String to set window text to.
	) const;
	//
	// Calls the Window's function to set the titlebar text for the window.
	//
	//**************************************


	//*********************************************************************************************
	//
	static void BroadcastToDialogs
	(
		UINT   u_msg,			// ID of message.
		WPARAM wp_param = 0,	// First parameter describing the message.
		LPARAM lp_param	= 0		// Second parameter describing the message.
	);
	//
	// Sends a message to all of the standard dialogs.
	//
	//**************************************

private:

	//*****************************************************************************************
	void AlignDialogText();

	//*****************************************************************************************
	void MakeDialog(HINSTANCE hinst_app, HWND hwnd_main);

	//*****************************************************************************************
	void EraseBkgnd();

	//*****************************************************************************************
	void GetFontMetrics(HWND hwnd, int& i_width, int& i_height) const;
};

//*********************************************************************************************
//
class CDialogDraw: public CStdDialog, public CDraw
//
// Prefix: dldr
//
// Dialog for use with line drawing and console buffers.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors.
	//

	// Constructor.
	CDialogDraw
	(
		const char* str_title,		// String to set window title to.
		const CRenderShell* prnsh = prnshMain,
									// Render shell (provides instance and main window).
		CConsoleEx* pcon = 0		// Optional console for text drawing.
	);
};


#endif // HEADER_LIB_SYS_STDDIALOG_HPP