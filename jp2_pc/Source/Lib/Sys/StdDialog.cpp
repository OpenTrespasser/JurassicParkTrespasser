/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of 'StdDialog.hpp.'
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/StdDialog.cpp                                                 $
 * 
 * 7     98/09/10 1:12 Speter
 * No clamp on dialog width, please.
 * 
 * 6     8/28/98 11:59a Asouth
 * explicit static declaration of callback function
 * 
 * 5     3/05/98 3:36p Pkeet
 * Uses the external default font size. Fixed bug under NT that causes the stats dialog window
 * to appear too large and where the top of the dialog window is off the top of the screen.
 * 
 * 4     98/02/10 12:55 Speter
 * CDialogDraw no longer creates a CRasterWin for CDraw.
 * 
 * 3     97/10/07 18:11 Speter
 * Rearranged includes so that StdDialog.hpp is valid.  Made ProcessMessage member not PASCAL
 * etc.  Made BroadcastToDialogs a class static, not global.  Made LogFontDefault a global (used
 * only in .cpp).
 * 
 * 2     97/10/06 10:58 Speter
 * Added str_title parameter to CStdDialog; added constructor which extracts HWND params from
 * CRenderShell.  Added embryo of CDialogDraw class.
 * 
 * 1     97/04/03 1:39p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "StdDialog.hpp"

#include "ConIO.hpp"
#include "Shell/ShellResource.h"

#include <map>

//
// Module definitions.
//

// Definition of a string to raster map.
typedef std::map<HWND, CStdDialog*, std::less<HWND> > TMapDialogPump;


//
// Module variables.
//

// Map of window handles to dialog objects.
TMapDialogPump mdpDialogPump;

// Record of font size.
int iFontHeight = 0;

//
// External functions.
//
extern void FillLogFontDefault(LOGFONT& lf, char* str_font_name);


//
// Module function prototypes.
//

	//*********************************************************************************************
	//
	static
	long FAR PASCAL DialogOut
	(
		HWND   hwnd,		// Window recieving the message.
		UINT   u_msg,		// ID of message.
		WPARAM wp_param,	// First parameter describing the message.
		LPARAM lp_param		// Second parameter describing the message.
	);
	//
	// Distributes windows messages to the appropriate dialogs.
	//
	// Returns.
	//		Returns the result of the callback, '0' indicates that nothing was done with the
	//		message.
	//
	//**********************************


//
// Class implementations.
//

//*********************************************************************************************
//
// CStdDialog Implementation.
//

	//*****************************************************************************************
	//
	// CStdDialog constructor.
	//

	//*****************************************************************************************
	CStdDialog::CStdDialog(const char* str_title, HINSTANCE hinst_app, HWND hwnd_parent, CConsoleEx* pcon)
		: hwndParent(hwnd_parent), pconConsole(pcon), bVisible(false)
	{
		// Instantiate the dialog template.
		MakeDialog(hinst_app, hwnd_parent);

		::SetWindowText(hwndGet(), str_title);

		// The console already knows about us, presumably.
	}


	//*****************************************************************************************
	CStdDialog::CStdDialog(const char* str_title, const CRenderShell* prnsh, CConsoleEx* pcon)
	{
		new(this) CStdDialog(str_title, prnsh->hinsAppInstance, prnsh->hwndMain, pcon);
	}


	//*****************************************************************************************
	//
	// CStdDialog member functions.
	//

	//*****************************************************************************************
	uint CStdDialog::ProcessMessage(UINT u_msg, WPARAM wp_param, LPARAM lp_param)
	{
		Assert(hwndWinHandle);

		switch (u_msg)
		{
			case WM_DESTROY:
				{
					EndDialog(hwndWinHandle, 0);

					// Set the visibility flag.
					bVisible = false;
				}
				return 1;

			case WM_CLOSE:
				{
					EndDialog(hwndWinHandle, 0);

					// Set the visibility flag.
					bVisible = false;
				}
				return 1;

			case WM_ERASEBKGND:
				EraseBkgnd();
				return 1;
		}
		return 0;
	}

	//*****************************************************************************************
	void CStdDialog::MakeDialog(HINSTANCE hinst_app, HWND hwnd_main)
	{
		Assert(hinst_app);
		Assert(hwnd_main);

		// Create the dialog object.
		hwndWinHandle = CreateDialog
		(
			hinst_app,							// hinst_app handle to application instance
			MAKEINTRESOURCE(DIALOG_TEXTOUT),	// LPCTSTR identifies dialog box template name  
			hwnd_main,							// hwnd handle to owner window
			(DLGPROC)DialogOut		 			// DLGPROC pointer to dialog box procedure
		);
		Assert(hwndWinHandle);

		// Add this class to the map of dialogs.
		mdpDialogPump[hwndWinHandle] = this;

		// Move the dialog to the bottom right corner of the screen.
		AlignDialogText();
	}

	//*****************************************************************************************
	void CStdDialog::ShowWindow(bool b_visible)
	{
		// Set the visibility flag.
		bVisible = b_visible;

		if (b_visible)
		{
			::ShowWindow(hwndWinHandle, SW_SHOWNORMAL);

			// Make sure the parent window has the focus.
			SetWindowPos
			(
				hwndWinHandle,							// Handle of window.
				HWND_TOP,								// Placement-order handle.
				0,										// Horizontal position.
				0,										// Vertical position.
				0,										// Width.
				0,										// Height.
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW	// Window-positioning flags.
			);

			// Make sure the parent window has the focus.
			SetActiveWindow(hwndParent);

			// Repaint the window.
			EraseBkgnd();
			//InvalidateRect(hwndParent,0, true);
		}
		else
			::ShowWindow(hwndWinHandle, SW_HIDE);
	}

	//*****************************************************************************************
	bool CStdDialog::bIsVisible() const
	{
		return bVisible;
	}

	//*****************************************************************************************
	HWND CStdDialog::hwndGet() const
	{
		Assert(hwndWinHandle);
		return hwndWinHandle;
	}

	//*****************************************************************************************
	void CStdDialog::SetWindowText(const char* str) const
	{
		::SetWindowText(hwndGet(), str);
	}

	//*****************************************************************************************
	void CStdDialog::EraseBkgnd()
	{
		// Blit black onto the screen.
		RECT rect;				// Dialog dimensions.
		HDC  hdc = GetDC(hwndWinHandle);	// Dialog device context.

		// Get the dialog dimensions.
		GetClientRect(hwndWinHandle, &rect);
		int i_width  = rect.right  - rect.left;
		int i_height = rect.bottom - rect.top;

		// Blit black.
		BitBlt(hdc, 0, 0, i_width, i_height, 0, 0, 0, BLACKNESS);

		// Release the device context.
		ReleaseDC(hwndWinHandle, hdc);

		// Display the console.
		if (pconConsole)
			pconConsole->Show();
	}

	//*****************************************************************************************
	//
	void CStdDialog::GetFontMetrics
	(
		HWND hwnd,		// Window handle to test font on.
		int& i_width,	// Width of font character in pixels.
		int& i_height	// Height of font character in pixels.
	) const
	//
	// Determines the default font character width and height.
	//
	//**************************************
	{
		LOGFONT    lf;
		TEXTMETRIC tm;
		HDC        hdc;
		HFONT      hFont;
		HFONT      hFontOld;

		//
		// Create font and device context.
		//
		hdc = GetDC(hwnd);
		FillLogFontDefault(lf, 0);
		hFont = CreateFontIndirect(&lf);
		hFontOld = (HFONT)SelectObject(hdc, hFont);

		//
		// Get the width and the height.
		//
		GetTextMetrics(hdc, &tm);
		i_height = tm.tmHeight;
		i_width  = tm.tmMaxCharWidth;

		//
		// Delete and release created handles.
		//
		SelectObject(hdc, hFontOld);
		DeleteObject(hFont);
		ReleaseDC(hwnd, hdc);
	}

	//*****************************************************************************************
	//
	void CStdDialog::AlignDialogText
	(
	)
	//
	// Aligns the text output dialog box to the bottom right-hand corner of the screen.
	//
	//**************************************
	{
		// Do nothing if there is no console associated with this dialog.
		if (!pconConsole)
			return;

		int  w_dialog, h_dialog;
		int  x_dialog, y_dialog;
		int  w_screen, h_screen;
		RECT r_dialog;

		// Text buffer dimensions;
		int i_cols = (int)pconConsole->uCharsPerLine;
		int i_rows = (int)pconConsole->uLinesPerScreen;

		//
		// Get text metrics to ensure the default font fits within the dialog box.
		//

		// Get font dimensions and dialog size.
		GetFontMetrics(hwndGet(), w_dialog, iFontHeight);
		w_dialog = (w_dialog - 1) * i_cols + GetSystemMetrics(SM_CXDLGFRAME) * 2;
		h_dialog = iFontHeight * i_rows + GetSystemMetrics(SM_CYCAPTION) +
				   GetSystemMetrics(SM_CYDLGFRAME) * 2;

		// Get dialog and screen dimensions.  If it's big enough, return.
		GetWindowRect(hwndGet(), &r_dialog);
		if (r_dialog.right+1  - r_dialog.left == w_dialog &&
			r_dialog.bottom+1 - r_dialog.top  == h_dialog)
			return;

		w_screen = GetSystemMetrics(SM_CXFULLSCREEN);
		h_screen = GetSystemMetrics(SM_CYFULLSCREEN);

		// Calculate the appropriate desired edge coordinates.
		x_dialog = w_screen - w_dialog;
		y_dialog = h_screen - h_dialog + GetSystemMetrics(SM_CYCAPTION);
		if (y_dialog < 0)
			y_dialog = 0;
		
		// Move Dialog box to the corner of the window.
		MoveWindow
		(
			hwndGet(),		// hwnd handle of window
			x_dialog,				// int horizontal position
			y_dialog,				// int vertical position
			w_dialog,				// int width
			h_dialog,				// int height
			true 					// bool repaint flag
		);
	}

	//*********************************************************************************************
	void CStdDialog::BroadcastToDialogs(UINT u_msg, WPARAM wp_param, LPARAM lp_param)
	{
		// Set the iterator to the beginning of the map.
		TMapDialogPump::iterator it = mdpDialogPump.begin();

		// Loop through the list of dialogs and send the message.
		for (; it != mdpDialogPump.end(); it++)
		{
			CStdDialog* pstddlg = mdpDialogPump[(*it).first];

			// Send the message.
			if (pstddlg)
			{
				pstddlg->ProcessMessage(u_msg, wp_param, lp_param);
			}
		}
	}

//*********************************************************************************************
//
// CDialogDraw implementation.
//

	//*****************************************************************************************
	CDialogDraw::CDialogDraw(const char* str_title, const CRenderShell* prnsh, CConsoleEx* pcon) :
		CStdDialog(str_title, prnsh, pcon),
		
		// Create a CDraw object for the window.
		CDraw(hwndGet())
	{
	}


//**********************************************************************************************
//
// Function implementations.
//

	//*********************************************************************************************
	static long FAR PASCAL DialogOut(HWND hwnd, UINT u_msg, WPARAM wp_param, LPARAM lp_param)
	{
		// Verify that there is a dialog corresponding to the window handle.
		TMapDialogPump::iterator it = mdpDialogPump.find(hwnd);
		
		if (it == mdpDialogPump.end())
			return 0;
		
		if (!mdpDialogPump[hwnd])
			return 0;

		// Execute the dialog's message pump.
		return mdpDialogPump[hwnd]->ProcessMessage(u_msg, wp_param, lp_param);
	}

