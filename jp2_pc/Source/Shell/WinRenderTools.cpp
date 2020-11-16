/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Implementation of 'WinRenderTools.hpp.'
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Shell/WinRenderTools.cpp                                             $
 * 
 * 24    97/03/17 14:47 Speter
 * Removed strCommandArg global var; now plain arguments are handled by passing character 0 to
 * ExecuteCommand.
 * 
 * 23    97/02/05 19:44 Speter
 * Removed WinMix and MIX types.  Changed interface to screen menu and driver menu calls.
 * 
 * 22    96/12/09 16:13 Speter
 * Removed ConShowStats() (replaced by CProfile functionality).
 * 
 * 21    12/05/96 3:52p Mlange
 * Added console print statements for the new timing stats.
 * 
 * 20    96/11/25 13:45 Speter
 * Fixed minor error in stat reporting.
 * 
 * 19    96/11/11 15:30 Speter
 * Updated for new CTimingStat auto-denominator, and for possible absence of Step timing stats.
 * 
 * 
 * 18    96/11/08 18:47 Speter
 * Stats such as "Tris per second" now show how many tris can be totally drawn per second, not
 * just the per-triangle portion.
 * 
 * 17    11/07/96 5:58p Agrant
 * moved ppalGetPaletteFromResource() to WinRenderTools.cpp(.hpp)
 * so that it was accessible outside of GUIApp.
 * 
 * 16    96/10/31 18:25 Speter
 * Updated for new stats.
 * Moved includes.
 * 
 * 15    96/10/21 14:33 Speter
 * Fixed bug in display of pixel timing.
 * 
 * 14    96/10/18 18:29 Speter
 * Changed SetDefaultWindowPos to SetWindowPos, taking parameters.
 * Made ConShowStats() operate only when console is shown.
 * 
 * 13    96/10/14 15:39 Speter
 * Added clut timing output  (fat lotta good, it's so inconsistent).
 * 
 * 12    96/10/09 16:22 Speter
 * Changed timing stats output, added screen info.
 * 
 * 11    96/10/08 19:30 Speter
 * Changed display of timing stats.  Now display number of triangles, vertices, and pixels
 * processed.
 * 
 * 
 * 10    96/10/04 17:23 Speter
 * What the hell was that?
 * 
 * 9     96/10/04 17:02 Speter
 * Added ConShowStats function, moved code from TestPipeline.
 * 
 * 8     9/13/96 2:19p Pkeet
 * Removed unnecessary includes.
 * 
 * 7     96/09/05 11:54 Speter
 * Moved Lib/W95/Resource.h to Shell/.
 * 
 * 6     96/08/19 13:11 Speter
 * Added SetDriverMenu.
 * Changed SetDirectDrawMenu to SetScreenModeMenu.
 * 
 * 5     8/07/96 4:42p Pkeet
 * Fixed bug in 'SetDirectDrawMenu' function.
 * 
 * 4     7/31/96 10:36a Pkeet
 * Added 'WinMix' includes.
 * 
 * 3     7/19/96 10:58a Pkeet
 * Changed include files for shell files moved to 'Shell.'
 * 
 * 2     7/18/96 2:37p Pkeet
 * Finished initial representation.
 * 
 * 1     7/17/96 7:32p Pkeet
 * 
 *********************************************************************************************/

//
// Macros, includes and definitions.
//

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"

#include <stdio.h>
#include <stdlib.h>
#include "Lib/Std/StringEx.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Sys/Textout.hpp"

#include "WinRenderTools.hpp"

//*****************************************************************************************
void SetWindowPos(HWND hwnd, int i_width, int i_height)
{
	// Set default window size rect.
	i_width += 2 * GetSystemMetrics(SM_CXFRAME);
	i_height += 2 * GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +
			        GetSystemMetrics(SM_CYMENU);

	// Set the initial window position.
	//SetWindowPos(hwnd, NULL, 0, 0, i_width, i_height, SWP_NOZORDER | SWP_NOACTIVATE);

	MoveWindow
	(
		hwnd,		// Handle of window.
		0,			// Horizontal position.
		0,			// Vertical position.
		i_width,	// Width.
		i_height,	// Height.
		true 		// Repaint flag.
	);
}


//*****************************************************************************************
bool bSetFullScreenMode(int i_dd_mode, int& i_width, int& i_height, int& i_bitdepth)
{
	if (i_dd_mode >= 0 && i_dd_mode < Video::iModes)
	{
		i_width    = Video::ascrmdList[i_dd_mode].iW;
		i_height   = Video::ascrmdList[i_dd_mode].iH;
		i_bitdepth = Video::ascrmdList[i_dd_mode].iBits;
		return true;
	}
	else
	{
		i_width    = 0;
		i_height   = 0;
		i_bitdepth = 0;
		return false;
	}
}


//*****************************************************************************************
void SetWindowMode(HWND hwnd, bool b_is_fullscreen)
{
	if (b_is_fullscreen)
	{
		// Set window style flags while in full screen mode.
		SetWindowLong
		(
			hwnd,
			GWL_STYLE,
			WS_VISIBLE
		);
	}
	else
	{
		// Set window style flags while in windowed mode.
		SetWindowLong
		(
			hwnd,
			GWL_STYLE,
			WS_CAPTION | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU |
			WS_POPUP | WS_VISIBLE
		);
	}
}


//*****************************************************************************************
void GetWindowSize(HWND hwnd, int& i_width, int& i_height)
{
	Assert(hwnd);

	RECT rect_client;

	GetClientRect(hwnd, &rect_client);
	i_width = rect_client.right - rect_client.left;
	i_height = rect_client.bottom - rect_client.top;
}

//*****************************************************************************************
void SetScreenModeMenu(HMENU hmenu, int i_itempos, int i_idstart)
{
	int          i_dd_modes;		// Index of DirectDraw modes.
	char         str_menu_text[64];	// Menu item string.
	int          i_stored;			// Number of characters written to str_menu_text.

	//
	// Enumerate the DirectDraw modes.
	//
	for (i_dd_modes = 0; i_dd_modes < Video::iModes; i_dd_modes++)
	{
		i_stored = sprintf(str_menu_text, "%ld x %ld x %ld", Video::ascrmdList[i_dd_modes].iW,
				           Video::ascrmdList[i_dd_modes].iH,
				           Video::ascrmdList[i_dd_modes].iBits);
		Assert(i_stored < sizeof(str_menu_text));
		InsertMenu
		(
			hmenu,
			i_itempos + i_dd_modes,
			MF_BYPOSITION | MF_STRING,
			i_idstart + i_dd_modes,
			str_menu_text
		);
	}
}

//*****************************************************************************************
void SetDriverMenu(HMENU hmenu, int i_itempos, int i_idstart)
{
	// Delete the dummy first item.
	DeleteMenu(hmenu, 0, MF_BYPOSITION);

	//
	// Enumerate the 3D drivers.
	//
	for (uint u = 0; u < sapRenderDesc.uLen; u++)
	{
		InsertMenu
		(
			hmenu,
			(uint)i_itempos + u,
			MF_BYPOSITION | MF_STRING,
			(uint)i_idstart + u,
			sapRenderDesc[u]->strDesc
		);
	}
}


//*********************************************************************************************
void ParseCommandLine(char* strCommandLine,
					  void (__cdecl *execute_command)(char  c_command, char* str_argument))
{
	Assert(strCommandLine);

	char* strParse = strCommandLine;

	//
	// Parse to the end of the command string.
	//
	for (;;)
	{
		// Skip over blanks.
		while (*strParse == ' ')
			strParse++;

		// Bail if the end of the string has been reached.
		if (*strParse == 0)
			return;

		char  c_command = 0;					// Option character.
		char  str_argument[255];				// Argument text.
		char* str_end_argument = str_argument;

		// Is the next character a letter or a command indicated by a '/'?
		if (*strParse == '/')
		{
			strParse++;
			if (*strParse == 0)
				return;
			c_command = *strParse++;
		}

		// Get the command argument and truncate it.
		*str_argument = 0;
		AppendString(str_argument, strParse, sizeof(str_argument));
		while (*str_end_argument != 0 && *str_end_argument != ' ')
			str_end_argument++;
		*str_end_argument = 0;

		// Execute command with arguement.
		execute_command(c_command, str_argument);
		
		// Skip to next blank.
		while (*strParse != ' ')
		{
			if (*strParse == 0)
				return;
			strParse++;
		}
	}
}

//**********************************************************************************************
CPal* ppalGetPaletteFromResource(HINSTANCE hinstance, int i_id)
//CPal*ppalGetPaletteFromResource(struct HINSTANCE__ *hinstance,int i_id)
{
	Assert(hinstance);
	Assert(i_id);

	struct SBitmapHeader
	{
		 BITMAPINFOHEADER    bmiHeader;
		 RGBQUAD             bmiColors[256];
	};

	//
	// Locate the bitmap resource.
	//
	HRSRC hrsrc = FindResource(hinstance, MAKEINTRESOURCE(i_id), RT_BITMAP);

	Assert(hrsrc);

	HGLOBAL hglobal = LoadResource(hinstance, hrsrc);

	Assert(hglobal);

	SBitmapHeader* pbmphd = (SBitmapHeader*)LockResource(hglobal);

	Assert(pbmphd);
	Assert(pbmphd->bmiColors);
	Assert(pbmphd->bmiHeader.biBitCount == 8);
	//Assert(pbmphd->bmiHeader.biClrUsed  == 256);

	//
	// Copy the palette data.
	//
	CPal* ppal = new CPal(256, false);

	Assert(ppal);
	Assert(ppal->aclrPalette);

	memcpy(ppal->aclrPalette, pbmphd->bmiColors, sizeof(PALETTEENTRY) * 256);

	// Free the bitmap resource.
	UnlockResource(hglobal);
	FreeResource(hglobal);

	// Return the pointer to the palette.
	return ppal;
}
