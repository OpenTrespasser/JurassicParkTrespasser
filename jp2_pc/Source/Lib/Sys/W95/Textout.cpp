/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Implementation of 'TextOut.hpp.'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/W95/Textout.cpp                                              $
 * 
 * 40    9/24/98 12:30a Pkeet
 * Memory is conserved in final mode.
 * 
 * 39    98/09/17 16:41 Speter
 * No more console output in final mode.
 * 
 * 38    98.09.15 8:51p Mmouni
 * Chaned text printing so that it works better on small screens.
 * 
 * 37    3/05/98 3:35p Pkeet
 * Changed the defaults so the colour is red, the font is "Courier" and the font size is 12.
 * 
 * 36    97/12/08 22:30 Speter
 * Fixed bad, bad, bug.  Added Asserts.
 * 
 * 35    11/12/97 7:17a Shernd
 * Added the ability to use flags to not have output files used
 * 
 * 34    10/14/97 8:10p Mlange
 * Added CConsoleBuffer::MoveCursor().
 * 
 * 33    97/10/06 10:56 Speter
 * Removed MakeDialogText() and DestroyDialogText(), superseded by CConsoleEx.  Changed conStd
 * to CConsoleEx.
 * 
 * 32    97/04/03 3:29p Pkeet
 * Move 'bActive' from 'CConsoleEx' to 'CConsoleBuffer.'
 * 
 * 31    97/03/27 3:54p Pkeet
 * Misspelled 'Trespasser.'
 * 
 * 30    97/03/27 3:43p Pkeet
 * Added a goodbye message when a file session closes.
 * 
 * 29    97/03/27 1:57p Pkeet
 * Fixed redirection bug.
 * 
 * 28    97/03/05 12:52p Pkeet
 * Added a Tab and PrintFloat8 function.
 * 
 * 27    97/02/06 13:31 Speter
 * Made console dialog work with multiple consoles.  AlignTextDialog() now takes cols/rows
 * params, and resizes window accordingly.
 * 
 * 26    97/02/05 19:42 Speter
 * Removed WinMix and MIX types.
 * 
 * 25    12/23/96 4:16p Cwalla
 * Fixed array delete being called without [].
 * 
 * 24    96/12/10 20:26 Speter
 * Made Show() only show as many lines as were written, speeding up display of FPS.
 * 
 * 23    96/12/09 16:06 Speter
 * Added PutString function for unformatted writing.
 * Removed timing calls (now done in GUIAppDlg main loop).  Removed done to do's.
 * 
 * 22    11/27/96 4:58p Pkeet
 * Made the deletion of the character buffer occur after the file has been closed in the
 * destructor.
 * 
 * 21    11/20/96 4:33p Pkeet
 * Disabled the 'verify' macro for the CloseFileSession member function because of a bug where
 * files would report an error after loading an object.
 * 
 * 20    96/11/11 15:27 Speter
 * Updated for new CTimingStat auto-denominator
 * Paused the timer during text operations, to provide more useful timings of actual routines.
 * 
 * 19    11/07/96 6:00p Agrant
 * now using ShellResources.h instead of resources.h
 * 
 * 18    10/24/96 6:31p Pkeet
 * Added commands to clear and flush the buffer before opening and closing a file session.
 * 
 * 17    96/10/08 19:29 Speter
 * Added more timings to msText.
 * 
 * 16    96/09/05 11:53 Speter
 * Moved Lib/W95/Resource.h to Shell/.
 * 
 * 15    8/09/96 3:50p Pkeet
 * Changed windows handle to a void*.
 * 
 * 14    96/07/31 15:43 Speter
 * Added timing stats for text showing.
 * Changed <window.h> to WinInclude.h.
 * 
 * 13    7/16/96 1:51p Pkeet
 * Added handling for WM_DESTROY message.
 * 
 * 12    7/16/96 11:29a Mlange
 * Moved several files to different directories, updated includes.
 * 
 * 11    5/23/96 11:38a Pkeet
 * Made 'hwndDialogTextout' a global variable. Changed so dialog text gives the main window the
 * focus upon creation.
 * 
 * 10    5/16/96 11:11a Pkeet
 * Added the PrintBinary function.
 * 
 * 9     5/14/96 5:16p Pkeet
 * Added a flush flag for the session file.
 * 
 * 8     5/14/96 4:34p Pkeet
 * Added 'OpenFileSession' and 'CloseFileSession' functions which allow console buffer data to
 * be duplicated when written to a text file.
 * 
 * 7     5/09/96 4:36p Pkeet
 * Replaced global variable 'conStandardConsole' with 'conStd.'
 * 
 * 6     5/09/96 4:02p Pkeet
 * Commented recent changes. Fixed tabs. Added form feed and asserts for '\b' and '\v'.
 * 
 * 5     5/09/96 2:34p Pkeet
 * Removed bHasMoved variable. Made dialog always align to the bottom right corner when
 * created. Made dialog fit font instead of visa-versa. Right mouse button click on the dialog
 * will always move the dialog to the default position.
 * 
 * 4     5/03/96 5:10p Pkeet
 * 
 * 3     5/01/96 5:18p Pkeet
 * 
 * 2     4/30/96 6:05p Pkeet
 * First trial.
 * 
 * 1     4/30/96 10:01a Pkeet
 * 
 *********************************************************************************************/

//
// Macros, includes and definitions.
//

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "../Textout.hpp"
#include "Shell/ShellResource.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern bool bUseOutputFiles;

//
// File scope constants.
//

//
// File scope variables.
//

// Handle of the text output dialog box.
HWND hwndDialogTextout = 0;

// Height of the default font in pixels.
static int iDefaultFontHeight = 12;

//
// Forward declared function prototypes.
//
void FillLogFontDefault(LOGFONT& lf, char* str_font_name = 0);
void AlignDialogText(int i_cols, int i_rows);

//
// Class member functions.
//

//*********************************************************************************************
//
// CConsoleBuffer Member Functions.
//
//

	//*****************************************************************************************
	//
	// CConsoleBuffer constructors and destructors.
	//

	//*****************************************************************************************
	CConsoleBuffer::CConsoleBuffer(uint u_chars_per_line, uint u_lines_per_screen)
		: bActive(true)
	{
		// Create the character buffer.
		uCharsPerLine = u_chars_per_line;
		uLinesPerScreen = u_lines_per_screen;

	#if (BUILDVER_MODE != MODE_FINAL)
		strCharBuffer = new char[u_chars_per_line * u_lines_per_screen];
	#else
		strCharBuffer = 0;
	#endif

		Assert(strCharBuffer != 0);
		pfileSession = 0;

		// Set default states.
		SetTabSize();
		SetTransparency();
		SetColour(RGB(255, 0, 0));

	#if (BUILDVER_MODE != MODE_FINAL)
		// Clear the buffer and set the cursor to the top left position.
		ClearScreen();
	#endif
	}

	//*****************************************************************************************
	CConsoleBuffer::~CConsoleBuffer()
	{
		// Close the session file if opened.
		CloseFileSession();

	#if (BUILDVER_MODE != MODE_FINAL)
		Assert(strCharBuffer);
		delete[] strCharBuffer;
	#endif
	}

	//*****************************************************************************************
	//
	// CConsoleBuffer Show member functions.
	//

	//*****************************************************************************************
	void CConsoleBuffer::Show(HDC hdc_screen, uint u_screen_height, bool b_match_height) const
	{
	#if (BUILDVER_MODE != MODE_FINAL)

		LOGFONT    lf;
		HFONT      hFont;
		HFONT      hFontOld;
		COLORREF   OldColor;
		uint       u_char_height;
		uint       u_line;
		int        i_old_bkmode;

		Assert(hdc_screen);

		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		//
		// Create a font for the screen.
		//
		if (b_match_height)
		{
			FillLogFontDefault(lf, 0);

			// Calculate the font character dimensions based on the window size.
			u_char_height = u_screen_height / uLinesFilled;

			// Maximum size.
			if (u_char_height > 12)
				u_char_height = 12;

			// Minimum size.
			if (u_char_height < 8)
				lf.lfHeight = -8;
			else
				lf.lfHeight = -u_char_height;
		}
		else
		{
			FillLogFontDefault(lf, 0);

			// Use fixed dimensions for the standard dialog box.
			u_char_height = iDefaultFontHeight;
		}

		hFont = CreateFontIndirect(&lf);
		Assert(hFont);

		// Apply font and characteristics to the screen.
		hFontOld = (HFONT)SelectObject(hdc_screen, hFont);
		Assert(hFontOld);

		OldColor = SetTextColor(hdc_screen, colChar);
		SetBkColor(hdc_screen, colBackground);
		i_old_bkmode = SetBkMode(hdc_screen, (bTransparent) ? (TRANSPARENT) : (OPAQUE));
		SetTextAlign(hdc_screen, TA_TOP | TA_LEFT);

		//
		// Draw text on screen.
		//
		for (u_line = 0; u_line < uLinesFilled; u_line++)
		{
			TextOut
			(
				hdc_screen,
				0,
				u_line * u_char_height,
				strCharBuffer + u_line * uCharsPerLine,
				uCharsPerLine
			);
		}
		
		// Reset screen drawing characteristics.
		SetBkMode(hdc_screen, i_old_bkmode);
		SetTextColor(hdc_screen, OldColor);
		SelectObject(hdc_screen, hFontOld);
		DeleteObject(hFont);
	#endif
 	}
	
	//*****************************************************************************************
	void CConsoleBuffer::Show(HWND hwnd_window, bool b_match_height) const
	{
	#if (BUILDVER_MODE != MODE_FINAL)
		HDC    hdc_window;
		RECT   r_window;

		Assert(hwnd_window);

		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		// Get the window size and device context, and call the appropriate show.
		hdc_window = GetDC(hwnd_window);
		GetClientRect(hwnd_window, &r_window);
		Show(hdc_window, r_window.bottom - r_window.top, b_match_height);
		ReleaseDC(hwnd_window, hdc_window);
	#endif
	}

	//*****************************************************************************************
	void CConsoleBuffer::Show() const
	{
	#if (BUILDVER_MODE != MODE_FINAL)
		if (hwndDialogTextout == 0)
			return;

		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		AlignDialogText(uCharsPerLine, uLinesPerScreen);
		Show(hwndDialogTextout, false);
	#endif
	}

	//*****************************************************************************************
	//
	// CConsoleBuffer set state member functions.
	//

	//*****************************************************************************************
	void CConsoleBuffer::SetCursorPosition(uint u_x, uint u_y)
	{
	#if (BUILDVER_MODE != MODE_FINAL)
		Assert(u_x < uCharsPerLine);
		Assert(u_y < uLinesPerScreen);

		uCursorX = u_x;
		uCursorY = u_y;
		SetMax(uLinesFilled, uCursorY+1);
	#endif
	}

	//*****************************************************************************************
	void CConsoleBuffer::MoveCursor(int i_delta_x, int i_delta_y)
	{
	#if (BUILDVER_MODE != MODE_FINAL)
		SetCursorPosition(int(uCursorX) + i_delta_x, int(uCursorY) + i_delta_y);
	#endif
	}

	//*****************************************************************************************
	void CConsoleBuffer::SetColour(COLORREF col_char, COLORREF col_background)
	{
	#if (BUILDVER_MODE != MODE_FINAL)
		colChar       = col_char;
		colBackground = col_background;
	#endif
	}

	//*****************************************************************************************
	void CConsoleBuffer::SetTabSize(uint u_tab_size)
	{
		Assert(u_tab_size);

		uTabSize = u_tab_size;
	}

	//*****************************************************************************************
	void CConsoleBuffer::SetTransparency(bool b_transparent)
	{
		bTransparent = b_transparent;
	}

#if VER_TEST

	//*****************************************************************************************
	//
	// CConsoleBuffer output member functions.
	//

	//*****************************************************************************************
	void CConsoleBuffer::PutChar(char c_put)
	{
		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		switch (c_put)
		{
			case '\a':
				// Bell.
				MessageBeep(0);
				break;

			case '\b':
				// Backspace.
				Assert(false);
				break;

			case '\f':
				// Form feed.
				ClearScreen();
				break;

			case '\n':
				// New line.
				NewLine();
				break;

			case '\r':
				// Carriage return.
				uCursorX = 0;
				break;

			case '\t':
				// Horizontal tab.
				PutChar(' ');
				while (uCursorX % uTabSize)
					PutChar(' ');
				break;

			case '\v':
				// Vertical tab.
				Assert(false);
				break;

			default:
				// Printing characters.
				Assert(uCursorX < uCharsPerLine);
				Assert(uCursorY < uLinesPerScreen);
				strCharBuffer[uCursorX + uCursorY * uCharsPerLine] = c_put;
				uCursorX++;
				if (uCursorX >= uCharsPerLine)
					NewLine();
		}
	}

	//*****************************************************************************************
	void CConsoleBuffer::PutString(const char *str)
	{
		Assert(str);

		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		while (*str)
			PutChar(*str++);
	}

	//*****************************************************************************************
	void CConsoleBuffer::Print(const char *str_format, ...)
	{
		va_list vlist;					// Variable arguments list.
		char    str_linebuffer[1024];	// Intermediate buffer.
		uint    u_char_written;			// Number of characters written by vsprintf.

		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		//
		// Write output string using variable number of arguments.
		//
		va_start(vlist, str_format);
		Assert(str_format);
		u_char_written = vsprintf(str_linebuffer, str_format, vlist);
		Assert(u_char_written <= sizeof(str_linebuffer));
		va_end(vlist);

		//
		// Copy string to console buffer.
		//
		PutString(str_linebuffer);
	}

	//*****************************************************************************************
	void CConsoleBuffer::PrintBinary(uint32 u4_binary_num, int i_binary_size, uint b_new_line)
	{
		int i_bit;

		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		Assert(i_binary_size <= 32);
		Assert(i_binary_size >  0);

		//
		// Print one bit at a time.
		//
		for (i_bit = i_binary_size - 1; i_bit >= 0; i_bit--)
		{
			if (((uint32)1 << i_bit) & u4_binary_num)
			{
				PutChar('1');
			}
			else
			{
				PutChar('0');
			}

			// Put a space every fourth bit.
			if ((i_bit & 0x03) == 0)
			{
				PutChar(' ');
			}
		}

		// Put in the optional new line.
		if (b_new_line)
		{
			PutChar('\n');
		}
	}

	//*****************************************************************************************
	void CConsoleBuffer::NewLine()
	{
		char str_top_line[1024];
		int  i_find_end_of_line;

		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		uCursorX = 0;
		uCursorY++;

		Assert(strCharBuffer);

		// Scroll the vertical cursor.
		if (uCursorY >= uLinesPerScreen)
		{
			//
			// Move characters "blown-off" the top to the session file.
			//
			if (pfileSession)
			{
				//
				// Write top line to the buffer string.
				//
				Assert(sizeof(str_top_line) > uCharsPerLine);
				memmove(str_top_line, strCharBuffer, uCharsPerLine);

				//
				// Put a null character at the end of the buffer string.
				//
				for (i_find_end_of_line = (int)uCharsPerLine - 1; i_find_end_of_line >= 0; i_find_end_of_line--)
				{
					if (str_top_line[i_find_end_of_line] != ' ')
						break;
				};
				str_top_line[i_find_end_of_line + 1] = 0;

				//
				// Output the buffer string to the session file.
				//
				fprintf(pfileSession, "%s\n", str_top_line);

				// Flush if appropriate.
				if (bFlushAlways)
				{
					fflush(pfileSession);
				}
			}

			// Move characters up one line.
			memmove(strCharBuffer, strCharBuffer + uCharsPerLine, (uLinesPerScreen - 1) * uCharsPerLine);
			uCursorY = uLinesPerScreen - 1;
		}

		SetMax(uLinesFilled, uCursorY+1);

		// Clear characters in new line.
		ClearLine();
	}
	
	//*****************************************************************************************
	void CConsoleBuffer::PrintFloat8(float f)
	{
		char str[256];	// Temporary string for the floating point value.

		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		// Write a leading character.
		PutChar(' ');
		
		// If the value is positive, write another leading character.
		if (f >= 0.0f)
			PutChar(' ');

		// Write the floating point string.
		sprintf(str, "%1.9f         ", f);

		// Zero terminate on the seventh character.
		str[7] = 0;

		// Write the string to the buffer.
		PutString(str);
	}


	//*****************************************************************************************
	void CConsoleBuffer::ClearScreen()
	{
		Assert(strCharBuffer);

		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		uLinesFilled = 0;
		SetCursorPosition(0, 0);
		memset(strCharBuffer, ' ', uCharsPerLine * uLinesPerScreen);
	}

	//*****************************************************************************************
	void CConsoleBuffer::ClearLine()
	{
		Assert(strCharBuffer);

		// Do nothing if the console is not active.
		if (!bIsActive())
			return;

		uCursorX = 0;
		memset(strCharBuffer + uCursorY * uCharsPerLine, ' ', uCharsPerLine);
	}

// #if VER_TEST
#endif

	//*****************************************************************************************
	//
	// File session member functions.
	//

	//*****************************************************************************************
	void CConsoleBuffer::OpenFileSession(const char *str_file_name, bool b_flush_always)
	{
	#if (BUILDVER_MODE != MODE_FINAL)

		time_t tm_timestamp;

		// Close the session file if opened, and clear the buffer.
		CloseFileSession();
		Flush();
		ClearScreen();

		// Set flush flag.
		bFlushAlways = b_flush_always;

        if (bUseOutputFiles)
        {
		    //
		    // Open a new file session.
		    //
		    if (str_file_name)
		    {
			    pfileSession = fopen(str_file_name, "w");
		    }
		    else
		    {
			    pfileSession = fopen("session.txt", "w");
		    }
		    Assert(pfileSession);

		    //
		    // Print a banner to the file.
		    //
		    time(&tm_timestamp);
		    fprintf(pfileSession, "*******************************************************************************\n\n");
		    fprintf(pfileSession, "    Jurassic Part 'Trespasser' File  Session:\n");
		    fprintf(pfileSession, "    %s\n", ctime(&tm_timestamp));
		    fprintf(pfileSession, "*******************************************************************************\n\n");
        }
        else
        {
            pfileSession = NULL;
        }
	#endif
	}

	//*****************************************************************************************
	void CConsoleBuffer::CloseFileSession()
	{
	#if (BUILDVER_MODE != MODE_FINAL)

		// Close the session file if opened.
		if (pfileSession)
		{
			// Write a goodbye message.
			Print("\n\nSession finished.");
			//
			// Move characters "blown-off" the top to the session file.
			//
			for (uint u_push_line = 0; u_push_line < uLinesPerScreen; u_push_line++)
				 NewLine();

			Flush();

			//
			// Close the file handle.
			//
			//Verify(fclose(pfileSession) == 0);
			fclose(pfileSession);
			pfileSession = 0;
		}

	#endif
	}


//
// File scope functions.
//

//*********************************************************************************************
//
void GetFontWidthHeight
(
	HWND hwnd,		// Window handle to test font on.
	int& i_width,	// Width of font character in pixels.
	int& i_height	// Height of font character in pixels.
)
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
	FillLogFontDefault(lf);
	hFont = CreateFontIndirect(&lf);
	hFontOld = (HFONT)SelectObject(hdc, hFont);

	//
	// Get the width and the height.
	//
	GetTextMetrics(hdc, &tm);
	i_height = tm.tmHeight;
	i_width  = tm.tmAveCharWidth;

	//
	// Delete and release created handles.
	//
	SelectObject(hdc, hFontOld);
	DeleteObject(hFont);
	ReleaseDC(hwnd, hdc);
}

//*********************************************************************************************
//
void AlignDialogText
(
	int i_cols, int i_rows				// Text dimensions of box.
)
//
// Aligns the text output dialog box to the bottom right-hand corner of the screen.
//
//**************************************
{
	int  w_dialog, h_dialog;
	int  x_dialog, y_dialog;
	int  w_screen, h_screen;
	RECT r_dialog;

	//
	// Get text metrics to ensure the default font fits within the dialog box.
	//

	// Get font dimensions and dialog size.
	GetFontWidthHeight(hwndDialogTextout, w_dialog, h_dialog);
	w_dialog = (w_dialog - 1) * i_cols + GetSystemMetrics(SM_CXDLGFRAME) * 2;
	h_dialog = (h_dialog - 1) * i_rows + GetSystemMetrics(SM_CYCAPTION) +
			   GetSystemMetrics(SM_CYDLGFRAME) * 2;

	if (w_dialog > 512)
		w_dialog = 512;

	// Get dialog and screen dimensions.  If it's big enough, return.
	GetWindowRect(hwndDialogTextout, &r_dialog);
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
		hwndDialogTextout,		// hwnd handle of window
		x_dialog,				// int horizontal position
		y_dialog,				// int vertical position
		w_dialog,				// int width
		h_dialog,				// int height
		true 					// bool repaint flag
	);
}

//*********************************************************************************************
//
long FAR PASCAL DialogTextOut
(
	HWND   hwnd,		// Window recieving the message.
	UINT   u_msg,		// ID of message.
	WPARAM wp_param,	// First parameter describing the message.
	LPARAM lp_param		// Second parameter describing the message.
)
//
// Text output dialog box message callback function.
//
// Returns.
//		Returns the result of the callback, '0' indicates that nothing was done with the
//		message.
//
//**************************************
{
    switch (u_msg)
    {
		case WM_DESTROY:
			EndDialog(hwnd, 0);
			hwndDialogTextout = 0;
			return 1;

		case WM_CLOSE:
			EndDialog(hwnd, 0);
			hwndDialogTextout = 0;
			return 1;

		case WM_ERASEBKGND:
//			conStd.Show();
			return 1;

		case WM_RBUTTONDOWN:
//			AlignDialogText();
			return 1;
    }
    return 0;
}


//*********************************************************************************************
//
void FillLogFontDefault
(
	LOGFONT& lf,			// Font struct to be filled out.
	char*    str_font_name	// Name of font.
)
//
// Fills the 'LOGFONT' struct with default information.
//
//**************************************
{
	// Create default font conditions.
	lf.lfHeight         = -iDefaultFontHeight;
	lf.lfWidth          = 0;
	lf.lfEscapement     = 0;
	lf.lfOrientation    = 0;
	lf.lfWeight         = FW_DONTCARE;
	lf.lfItalic         = 0;
	lf.lfUnderline      = 0;
	lf.lfStrikeOut      = 0;
	lf.lfCharSet        = ANSI_CHARSET;
	lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	lf.lfQuality        = DRAFT_QUALITY;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	if (str_font_name)
		strcpy(lf.lfFaceName, str_font_name);
	else
		strcpy(lf.lfFaceName, "Terminal");
}

//
// Global functions.
//

