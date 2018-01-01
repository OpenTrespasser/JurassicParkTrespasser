/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Text output box for debugging purposes.
 *
 * To do:
 *		Add bitmap buffer support for ModeX screen modes.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/Textout.hpp                                                  $
 * 
 * 27    98/09/17 16:40 Speter
 * No more console output in final mode.
 * 
 * 26    2/06/98 5:54p Pkeet
 * Added the 'SetAlwaysFlush' member function.
 * 
 * 25    1/13/98 9:33p Agrant
 * Made SetActive virtual
 * 
 * 24    97/10/30 14:50 Speter
 * Changed <<(char*) to <<(const char*), sped up other funcs.
 * 
 * 23    10/14/97 8:10p Mlange
 * Added CConsoleBuffer::MoveCursor().
 * 
 * 22    97/10/06 10:56 Speter
 * Removed MakeDialogText() and DestroyDialogText(), superseded by CConsoleEx.  Changed conStd
 * to CConsoleEx.
 * 
 * 21    9/04/97 3:55p Pkeet
 * Added the 'bFileSessionIsOpen' member function.
 * 
 * 20    97/04/03 3:29p Pkeet
 * Move 'bActive' from 'CConsoleEx' to 'CConsoleBuffer.'
 * 
 * 19    97/04/03 1:01p Pkeet
 * Made character dimension buffers public.
 * 
 * 18    97/03/27 3:44p Pkeet
 * Made the 'Show()' member function and the destructor virtual.
 * 
 * 17    97/03/05 12:52p Pkeet
 * Added a Tab and PrintFloat8 function.
 * 
 * 16    97/02/05 19:42 Speter
 * Removed WinMix and MIX types.
 * 
 * 15    96/12/09 16:06 Speter
 * Added PutString function for unformatted writing.
 * Removed timing calls (now done in GUIAppDlg main loop).  Removed done to do's.
 * 
 * 14    11/20/96 4:34p Pkeet
 * Gave the 'CConsoleBuffer' constructor default parameters.
 * 
 * 13    8/14/96 5:36p Pkeet
 * Added streaming operators.
 * 
 * 12    8/09/96 3:27p Pkeet
 * Changed windows handle to void*.
 * 
 * 11    96/07/31 15:44 Speter
 * Made cursor position variables public.
 * 
 * 10    7/16/96 11:29a Mlange
 * Moved several files to different directories, updated includes.
 * 
 * 9     96/07/02 17:55 Speter
 * Added to do.
 * 
 * 8     5/23/96 11:38a Pkeet
 * Made 'hwndDialogTextout' a global variable. Changed so dialog text gives the main window the
 * focus upon creation.
 * 
 * 7     5/16/96 11:11a Pkeet
 * Added the PrintBinary function.
 * 
 * 6     5/14/96 5:16p Pkeet
 * Added a flush flag for the session file.
 * 
 * 5     5/14/96 4:34p Pkeet
 * Added 'OpenFileSession' and 'CloseFileSession' functions which allow console buffer data to
 * be duplicated when written to a text file.
 * 
 * 4     5/09/96 4:36p Pkeet
 * Replaced global variable 'conStandardConsole' with 'conStd.'
 * 
 * 3     5/09/96 2:13p Pkeet
 * 
 * Moved 'To Do' list to file header. Changed uint32's to uint's. Added appropriate const's
 * after function headers.
 * 
 * 2     5/01/96 5:18p Pkeet
 * 
 * 1     4/30/96 6:06p Pkeet
 * Header for text output dialog box.
 * 
 * 1     4/17/96 10:18a Pkeet
 *
 *********************************************************************************************/

#ifndef HEADER_LIB_SYS_TEXTOUT_HPP
#define HEADER_LIB_SYS_TEXTOUT_HPP

//
// Macros, includes and definitions.
//

#include "Lib/W95/WinAlias.hpp"


//
// Class definitions.
//

//*********************************************************************************************
//
class CConsoleBuffer
//
// The CConsoleBuffer class emulates standard console operations by maintaining an array of
// characters. The buffer can then automatically display the formatted text through any of
// the 'Show' functions. Generally only one instance of 'CConsoleBuffer' is used in any
// application.
//
// Prefix: con
//
//**************************************
{
private:
	bool     bActive;			// Indicates if the console is currently receiving input.
public:
	COLORREF colChar;			// RGB colour of character.
	COLORREF colBackground;		// RGB colour of character background (if drawn).
	uint	 uCursorX;			// The horizontal cursor position.
	uint	 uCursorY;			// The vertical cursor position.
	uint	 uTabSize;			// The tab size in number of spaces.
	uint	 uCharsPerLine;		// Number of characters per line.
	uint	 uLinesPerScreen;	// Number of lines per screen.

protected:
	char*    strCharBuffer;		// Pointer to the buffer of characters.
	uint	 uLinesFilled;		// Number of lines with text.
	bool     bTransparent;		// Will draw tranparent backgrounds for characters
								// if 'true,' solid backgrounds if 'false.'
	FILE*    pfileSession;		// Session file handle.
	bool     bFlushAlways;		// If 'true,' the file will be flushed after every write.

public:

	//*****************************************************************************************
	//
	// Constructors and destructors.
	//

	//*****************************************************************************************
	//
	CConsoleBuffer
	(
		uint u_chars_per_line   = 80,	// uCharsPerLine set by this value
		uint u_lines_per_screen = 25	// uLinesPerScreen set by this value.
	);
	//
	// Constructs an emulation buffer of u_chars_per_line * u_lines_per_screen in size.
	// The global instance of 'CConsoleBuffer' defaults to 80 x 25.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual ~CConsoleBuffer
	(
	);
	//
	// Destroys the emulation buffer.
	//
	//**************************************

	//*****************************************************************************************
	//
	// Show member functions.
	//

	//*****************************************************************************************
	//
	void Show
	(
		HDC  hdc_screen,			// Device context of screen or window to draw to.
		uint u_screen_height,		// Height of screen or window to draw to in pixels.
		bool b_match_height = true	// If 'true' the font tries to match the window height.
	) const;
	//
	// Displays characters in buffer given a valid device context. Sizes the font to fit the
	// entire buffer in the Window. Use for displaying text over a raster.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Show
	(
		HWND hwnd_window,			// Handle of window to draw to.
		bool b_match_height = true	// If 'true' the font tries to match the window height.
	) const;
	//
	// Displays characters in buffer given a valid window handle. Sizes the font to fit the
	// entire buffer in the Window. Use for displaying to any Window.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void Show
	(
	) const;
	//
	// Displays characters in buffer onto the text output dialog box. Sizes the font to fit
	// the entire buffer in the standard dialog box. Does nothing if the dialog box is closed.
	//
	//**************************************

	//*****************************************************************************************
	//
	// Set state member functions.
	//

	//*****************************************************************************************
	//
	bool bIsActive
	(
	) const
	//
	// Returns 'true' if the console is activated.
	//
	//**************************************
	{
		return bActive;
	}

	//*****************************************************************************************
	//
	virtual void SetActive
	(
		bool b_active = true	// New state.
	)
	//
	// Sets the activation state of the console.
	//
	//**************************************
	{
		bActive = b_active;
	}

	//*****************************************************************************************
	//
	void SetCursorPosition
	(
		uint u_x,	// Horizontal position to set the cursor to.
		uint u_y	// Vertical position to set the cursor to.
	);
	//
	// Sets the cursor horizontal and vertical positions. Causes an Assert error if the cursor
	// is outside the valid range.
	//
	//**************************************

	//*****************************************************************************************
	//
	void MoveCursor
	(
		int i_delta_x,	// Cursor movement deltas.
		int i_delta_y
	);
	//
	// Moves the cursor. Causes an Assert error if the cursor is moved outside the valid range.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetTabSize
	(
		uint u_tab_size = 8		// Number of spaces to set the tab to produce.
	);
	//
	// Sets the number of spaces in a tab.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetTransparency
	(
		bool b_transparent = false	// Value to set the bTransparent member variable to.
	);
	//
	// Sets the background transparency flag. If the flag is 'true,' the background of the
	// character will not be drawn.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetColour
	(
		COLORREF col_char       = RGB(255, 255, 0),	// The rgb colour of the character.
		COLORREF col_background = RGB(0, 0, 0)		// The rgb colour of the background.
	);
	//
	// Sets the colour of the letter and the background.
	//
	//**************************************

	//*****************************************************************************************
	//
	// File session member functions.
	//

	//*****************************************************************************************
	//
	void OpenFileSession
	(
		const char* str_file_name = 0,		// The name of the text file to output the session to.
											// If no name is specified, 'session.txt' will be used.
		bool        b_flush_always = false	// Always flushes the text file after a write.
	);
	//
	// Opens a text file to dump console data to. Any text scrolled off the top of the console
	// buffer will be dumped to the text file. Any text remaining in the console buffer when
	// 'CloseFileSession()' is called will also be dumped.
	//
	//**************************************

	//*****************************************************************************************
	//
	void CloseFileSession
	(
	);
	//
	// Closes the session file and invalidates the file handle.
	//
	//**************************************

	//*****************************************************************************************
	//
	bool bFileSessionIsOpen
	(
	)
	//
	// Returns 'true' if a file session is open.
	//
	//**************************************
	{
		return pfileSession != 0;
	}

#if VER_TEST

	//*****************************************************************************************
	//
	// Output member functions.
	//

	//*****************************************************************************************
	//
	void PutChar
	(
		char c_put	// Character to put to the buffer.
	);
	//
	// Puts a single character to the buffer at the current cursor position and moves the
	// cursor forward.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Tab
	(
	)
	//
	// Uses 'PutChar' to move the cursor to the next tab.
	//
	//**************************************
	{
		PutChar('\t');
	}

	//*****************************************************************************************
	//
	void PutString
	(
		const char *str					// A string to print.
	);
	//
	// Prints the string to the buffer.
	//
	// Notes:
	//		This has lower overhead than Print() below, as it does not have to do printf
	//		formatting into a temporary buffer.
	//
	//**********************************

	//*****************************************************************************************
	//
	void Print
	(
		const char *str_format,		// A format string using the 'printf' format.
		...							// A variable-sized arguement list for the format
									// string.
	);
	//
	// Emulates 'printf' using the console buffer instead of a string or file stream and
	// moves the cursor.
	//
	//**************************************

	//*****************************************************************************************
	//
	void PrintBinary
	(
		uint32 u4_binary_num,		// Number to print out in binary.
		int    i_binary_size,		// Number of bits to print out.
		uint   b_new_line = true	// Prints a new line character at the end of the string.
	);
	//
	// Prints out the number as a string representing a binary number in xxxx xxxx xxxx xxxx
	// format, and then prints an optional new line character at the end of the string.
	//
	//**************************************

	//*****************************************************************************************
	//
	void PrintFloat8
	(
		float f		// Floating point value to write
	);
	//
	// Prints a floating point value that is always 8 characters long.
	//
	//**************************************

	//*****************************************************************************************
	//
	void NewLine
	(
	);
	//
	// Moves the cursor to the next line, clears the next line and moves the cursor to the
	// beginning of the line.
	//
	//**************************************

	//*****************************************************************************************
	//
	void ClearScreen
	(
	);
	//
	// Clears the entire buffer and puts the cursor at the top left of the screen.
	//
	//**************************************

	//*****************************************************************************************
	//
	void Flush
	(
	)
	//
	// Displays then clears the entire buffer.
	//
	//**************************************
	{
		Show();
		ClearScreen();
	}

	//*****************************************************************************************
	//
	void SetAlwaysFlush
	(
		bool b_new_flag
	)
	//
	// Displays then clears the entire buffer.
	//
	//**************************************
	{
		bFlushAlways = b_new_flag;
	}

	//*****************************************************************************************
	//
	void ClearLine
	(
	);
	//
	// Clears the current line pointed to by the cursor in the buffer and moves the cursor to
	// the beginning of the line.
	//
	//**************************************


	//*****************************************************************************************
	//
	// Stream overloads.
	//

	CConsoleBuffer& operator <<(char c)
	{
		PutChar(c);
		return *this;
	}

	CConsoleBuffer& operator <<(const char* str)
	{
		Assert(str);
		PutString(str);
		return *this;
	}

	CConsoleBuffer& operator <<(int i)
	{
		Print("%d", i);
		return *this;
	}

	CConsoleBuffer& operator <<(uint u)
	{
		Print("%u", u);
		return *this;
	}

	CConsoleBuffer& operator <<(float f)
	{
		Print("%f", f);
		return *this;
	}

	CConsoleBuffer& operator <<(double d)
	{
		Print("%f", d);
		return *this;
	}

// #if VER_TEST
#else

	//*****************************************************************************************
	//
	// Disable output member functions.
	//

	//*****************************************************************************************
	void PutChar(char)
	{
	}

	//*****************************************************************************************
	void Tab()
	{
	}

	//*****************************************************************************************
	void PutString(const char*)
	{
	}

	//*****************************************************************************************
	void Print(const char*, ...)
	{
	}

	//*****************************************************************************************
	void PrintBinary(uint32, int, uint)
	{
	}

	//*****************************************************************************************
	void PrintFloat8(float)
	{
	}

	//*****************************************************************************************
	void NewLine()
	{
	}

	//*****************************************************************************************
	void ClearScreen()
	{
	}

	//*****************************************************************************************
	void Flush()
	{
	}

	//*****************************************************************************************
	//
	void SetAlwaysFlush
	(
		bool b_new_flag
	)
	//
	// Displays then clears the entire buffer.
	//
	//**************************************
	{
		bFlushAlways = b_new_flag;
	}

	//*****************************************************************************************
	void ClearLine()
	{
	}

	CConsoleBuffer& operator <<(char)
	{
		return *this;
	}

	CConsoleBuffer& operator <<(const char*)
	{
		return *this;
	}

	CConsoleBuffer& operator <<(int)
	{
		return *this;
	}

	CConsoleBuffer& operator <<(uint)
	{
		return *this;
	}

	CConsoleBuffer& operator <<(float)
	{
		return *this;
	}

	CConsoleBuffer& operator <<(double)
	{
		return *this;
	}

// #if VER_TEST #else
#endif
};

#endif