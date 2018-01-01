/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of GUITools.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/GUITools.cpp                                                   $
 * 
 * 29    8/16/98 8:34p Mmouni
 * Fixed minor error in fGetDlgItemFloat() -> "&" used instead of "&&".
 * 
 * 28    98/04/05 20:41 Speter
 * Moved Raster.hpp from .hpp file, corrected include order.
 * 
 * 27    3/31/98 4:55p Mlange
 * CTerrain interface change.
 * 
 * 26    3/13/98 4:34p Mlange
 * CTerrain::RebuildTextures() interface change.
 * 
 * 25    3/13/98 10:42a Mlange
 * Now uses CTerrain::RebuildTextures().
 * 
 * 24    2/27/98 5:28p Rwyatt
 * LoadFilename name function now starts in the direcotry that you left it in.
 * 
 * 23    1/20/98 4:51p Pkeet
 * Added the 'bGetSaveFilename' function.
 * 
 * 22    1/09/98 7:05p Rwyatt
 * Fixed a bug with scroll bar positions, you can now scroll to the end with the arrows
 * 
 * 21    98/01/06 15:51 Speter
 * Removed UpdateTIN() function; update now performed by CPolyIterator.
 * 
 * 20    98/01/02 17:26 Speter
 * Removed old TIN.hpp include.
 * 
 * 19    12/23/97 2:18p Pkeet
 * Added the 'ResetTerrainSkyTextures' function.
 * 
 * 18    97/09/16 15:23 Speter
 * Added str_title param to bGetFilename().
 * 
 * 17    97/08/04 15:08 Speter
 * Added necessary locking to cross-hair draw.
 * 
 * 16    97/07/17 17:24 Speter
 * Fixed bug in DrawCrosshairs that made everything black.
 * 
 * 15    97/07/15 1:01p Pkeet
 * Added a parameter to set colour for crosshairs.
 * 
 * 14    97/06/16 1:11p Pkeet
 * Changed the scroll range.
 * 
 * 13    97/05/20 3:13p Pkeet
 * Fixed bug where the floating point to text conversion did not use exponent format.
 * 
 * 12    97/04/15 3:02p Pkeet
 * Added terrain load dialog.
 * 
 * 11    4/09/97 11:02p Rwyatt
 * Added filename dialogs for loading and saving replay files
 * 
 * 10    1/27/97 4:55p Pkeet
 * Fixed bug with scrollbars.
 * 
 * 9     1/25/97 1:56p Pkeet
 * Added capability to display significant digits in the 'SetDlgItemFloat' function.
 * 
 * 8     1/22/97 5:53p Pkeet
 * Removed the 'moron' message.
 * 
 * 7     96/12/31 16:43 Speter
 * Updated for rptr and new CRaster funcs.
 * 
 * 6     11/07/96 5:58p Agrant
 * moved ppalGetPaletteFromResource() to WinRenderTools.cpp(.hpp)
 * so that it was accessible outside of GUIApp.
 * 
 * 5     96/09/24 13:27 Speter
 * Changed default file extension from .3db to .grf.
 * 
 * 4     8/15/96 4:35p Pkeet
 * Added the 'ppalGetPaletteFromResource' function.
 * 
 * 3     8/13/96 3:02p Pkeet
 * Added the 'bGetFilename' function.
 * 
 * 2     8/09/96 3:54p Pkeet
 * Initial implemenation.
 * 
 **********************************************************************************************/

#include "stdafx.h"
#include "GUITools.hpp"
#include "Lib/GeomDBase/Terrain.hpp"
#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/Sys/FileEx.hpp"
#include "Lib/View/Raster.hpp"

#include <stdio.h>
#include <memory.h>
#include <direct.h>

//**********************************************************************************************
void Crosshatch(rptr<CRaster> pras)
{
	uint32 pix_pixel = pras->pixFromColour(CColour(0, 255, 0));

	for (int i_y = 0; i_y < pras->iHeight; i_y++)
	{
		int i_index = pras->iIndex(0, i_y);
		for (int i_x = 0; i_x < pras->iWidth; i_x++)
			if ((i_x + i_y) & 1)
				pras->PutPixel(i_index + i_x, pix_pixel);
			else
				pras->PutPixel(i_index + i_x, 0);
	}
}


//**********************************************************************************************
void PutSafePixel(rptr<CRaster> pras, int i_x, int i_y, uint32 pix_pixel)
{
	if (i_x < 0)
		return;
	if (i_x >= pras->iWidth)
		return;
	if (i_y < 0)
		return;
	if (i_y >= pras->iHeight)
		return;
	pras->PutPixel(i_x, i_y, pix_pixel);
}


//**********************************************************************************************
void DrawCrosshair(rptr<CRaster> pras, int i_x, int i_y, int i_col)
{
	static TPixel	pix_pixel;
	static int		i_last_col = -1;
	
	// Get the modulus of the color.
	i_col = i_col % 6;

	// Select the pixel colour.
	if (i_col != i_last_col)
	{
		switch (i_col)
		{
			case 0:
				pix_pixel = pras->pixFromColour(CColour(0.0f, 1.0f, 0.0f));
				break;
			case 1:
 				pix_pixel = pras->pixFromColour(CColour(1.0f, 0.0f, 0.0f));
				break;
			case 2:
				pix_pixel = pras->pixFromColour(CColour(0.0f, 0.0f, 1.0f));
				break;
			case 3:
				pix_pixel = pras->pixFromColour(CColour(1.0f, 1.0f, 0.0f));
				break;
			case 4:
				pix_pixel = pras->pixFromColour(CColour(1.0f, 0.0f, 1.0f));
				break;
			case 5:
				pix_pixel = pras->pixFromColour(CColour(1.0f, 1.0f, 1.0f));
				break;
			default:
				Assert(0);
		}
		i_last_col = i_col;
	}

	pras->Lock();

	//
	// Draw the crosshair.
	//
	PutSafePixel(pras, i_x-3, i_y, pix_pixel);
	PutSafePixel(pras, i_x-2, i_y, pix_pixel);
	PutSafePixel(pras, i_x-1, i_y, pix_pixel);
	PutSafePixel(pras, i_x, i_y, pix_pixel);
	PutSafePixel(pras, i_x+1, i_y, pix_pixel);
	PutSafePixel(pras, i_x+2, i_y, pix_pixel);
	PutSafePixel(pras, i_x+3, i_y, pix_pixel);
	PutSafePixel(pras, i_x, i_y-3, pix_pixel);
	PutSafePixel(pras, i_x, i_y-2, pix_pixel);
	PutSafePixel(pras, i_x, i_y-1, pix_pixel);
	PutSafePixel(pras, i_x, i_y+1, pix_pixel);
	PutSafePixel(pras, i_x, i_y+2, pix_pixel);
	PutSafePixel(pras, i_x, i_y+3, pix_pixel);

	//
	// Draw the black frame around the crosshair.
	//

	// Get the colour black for the raster.
	TPixel pix_black = pras->pixFromColour(CColour(0, 0, 0));

	// Horizontal.
	PutSafePixel(pras, i_x-3, i_y-1, pix_black);
	PutSafePixel(pras, i_x-2, i_y-1, pix_black);
	PutSafePixel(pras, i_x-1, i_y-1, pix_black);
	PutSafePixel(pras, i_x+1, i_y-1, pix_black);
	PutSafePixel(pras, i_x+2, i_y-1, pix_black);
	PutSafePixel(pras, i_x+3, i_y-1, pix_black);
	PutSafePixel(pras, i_x-3, i_y+1, pix_black);
	PutSafePixel(pras, i_x-2, i_y+1, pix_black);
	PutSafePixel(pras, i_x-1, i_y+1, pix_black);
	PutSafePixel(pras, i_x+1, i_y+1, pix_black);
	PutSafePixel(pras, i_x+2, i_y+1, pix_black);
	PutSafePixel(pras, i_x+3, i_y+1, pix_black);

	// Vertical.
	PutSafePixel(pras, i_x-1, i_y-3, pix_black);
	PutSafePixel(pras, i_x-1, i_y-2, pix_black);
	PutSafePixel(pras, i_x-1, i_y+2, pix_black);
	PutSafePixel(pras, i_x-1, i_y+3, pix_black);
	PutSafePixel(pras, i_x+1, i_y-3, pix_black);
	PutSafePixel(pras, i_x+1, i_y-2, pix_black);
	PutSafePixel(pras, i_x+1, i_y+2, pix_black);
	PutSafePixel(pras, i_x+1, i_y+3, pix_black);

	pras->Unlock();
}


//**********************************************************************************************
void SetDlgItemFloat(CWnd* pwnd, int i_item_id, float f_value, int i_significant_digits)
{
	char str_out[32];

	// Scale the number of fractional digits according to the size of the number.
	if (Abs(f_value) > 0.1f)
		sprintf(str_out, "%1.2f", f_value);
	else
		if (Abs(f_value) > 0.01f)
			sprintf(str_out, "%1.3f", f_value);
		else
			if (Abs(f_value) > 0.001f)
				sprintf(str_out, "%1.4f", f_value);
			else
				if (f_value == 0.0f)
					sprintf(str_out, "0.0");
				else
					// Too far gone, use scientific notation.
					sprintf(str_out, "%1.2e", f_value);

	// Write the string to the dialog control.
	pwnd->SetDlgItemText(i_item_id, str_out);
}


//**********************************************************************************************
float fGetDlgItemFloat(CWnd* pwnd, int i_item_id)
{
	char  str_in[32];	// Buffer to copy the control text to.
	float f_out;		// Floating point output value.

	// Get the control's text.
	pwnd->GetDlgItemText(i_item_id, str_in, sizeof(str_in));

	// If no text is returned, assume the value to be returned is zero.
	if (strlen(str_in) < 1)
	{
		return 0.0f;
	}

	// If the decimal point is pressed, replace it with a zero and a decimal point.
	if (strlen(str_in) == 1 && *str_in == '.')
	{
		pwnd->SetDlgItemText(i_item_id, "0.");
		pwnd->SendDlgItemMessage(i_item_id, EM_SETSEL, 2, 2);
		return 0.0f;
	}

	// Convert the text to a floating point value.
	int i_result = sscanf(str_in, "%f", &f_out);

	// Verify input.
	if (i_result == 1)
	{
		// Input successful. Return value.
		return f_out;
	}

	// Display error message.
	pwnd->MessageBox("Invalid type! Enter a floating point value",
		             "Input Error.", MB_OK | MB_ICONHAND);

	// Clear control text.
	pwnd->SetDlgItemText(i_item_id, "");

	return 0.0f;
}


//**********************************************************************************************
void MoveScrollbar(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int MinPos, MaxPos;

	pScrollBar->GetScrollRange(&MinPos, &MaxPos);

	int ScrollRange = MaxPos - MinPos;
	int FineTune    = 1;//ScrollRange / 100;
	int CoarseTune  = 10;//ScrollRange / 10;
	int Pos         = pScrollBar->GetScrollPos();

	FineTune = (FineTune < 1) ? (1) : (FineTune);

	CoarseTune = (CoarseTune < 1) ? (1) : (CoarseTune);

	switch (nSBCode)
	{
		case SB_LEFT:
			Pos = MinPos;
			break;
		case SB_RIGHT:
			Pos = MaxPos;
			break;
		case SB_PAGELEFT:
			Pos -= CoarseTune;
			if (Pos < MinPos) 
				Pos = MinPos;
			break;
		case SB_LINELEFT:
			Pos -= FineTune;
			if (Pos < MinPos) 
				Pos = MinPos;
			break;
		case SB_PAGERIGHT:
			Pos += CoarseTune;
			if (Pos > MaxPos) 
				Pos = MaxPos;
			break;
		case SB_LINERIGHT:
			Pos += FineTune;
			if (Pos > MaxPos) 
				Pos = MaxPos;
			break;
		case SB_THUMBPOSITION:
			Pos = nPos;
			break;
		case SB_THUMBTRACK:
			//Pos = nPos;
			break;
		default:;
	}
	pScrollBar->SetScrollPos(Pos, TRUE);
}

//**********************************************************************************************
// This function uses a convoluted method of maintaing the current load directory and the
// current working directory. It is trivial to maintain the current load directory if you are
// willing to allow the current worling directory to chnage. Doing this will cause GUIApp to
// crash.
BOOL bGetFilename(HWND hwnd, char* str_filename, int i_max_chars, const char* str_title, const char* str_filter)
{
	Assert(str_filter);

	static bool b_local = false;
	static char str_dir[MAX_PATH];

	CPushDir	pd(".");

	OPENFILENAME ofn;

	// Null the string.
	*str_filename = 0;
			
	// Fill structure.
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hwnd;
	ofn.lpstrFilter       = str_filter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = str_filename;
	ofn.nMaxFile          = i_max_chars;
	if (b_local)
	{
		ofn.lpstrInitialDir   = str_dir;
	}
	else
	{
		ofn.lpstrInitialDir   = NULL;
	}
	ofn.lpstrTitle        = str_title;
	ofn.lpstrFileTitle    = NULL;
	ofn.lpstrDefExt       = NULL;
	ofn.Flags             = OFN_HIDEREADONLY;

	// Get the file name.
	if (GetOpenFileName((LPOPENFILENAME)&ofn))
	{
		_getcwd(str_dir, MAX_PATH);
		b_local=true;
		return TRUE;
	}
	return FALSE;
}

//**********************************************************************************************
BOOL bGetSaveFilename(HWND hwnd, char* str_filename, int i_max_chars, const char* str_title, const char* str_filter)
{
	Assert(str_filter);

	OPENFILENAME ofn;

	// Null the string.
	*str_filename = 0;
			
	// Fill structure.
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hwnd;
	ofn.lpstrFilter       = str_filter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = str_filename;
	ofn.nMaxFile          = i_max_chars;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = str_title;
	ofn.lpstrFileTitle    = NULL;
	ofn.lpstrDefExt       = NULL;
	ofn.Flags             = OFN_HIDEREADONLY | OFN_NOCHANGEDIR;

	// Get the file name.
	if (GetSaveFileName((LPOPENFILENAME)&ofn))
	{
		return TRUE;
	}
	return FALSE;
}


//*********************************************************************************************
bool bGetReplayFileName(HWND hwnd, char* str_filename, int i_max_chars, bool b_load)
{
	OPENFILENAME ofn;

	// Null the string.
	*str_filename = 0;
			
	// Fill structure.
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hwnd;
	ofn.lpstrFilter       = "Replay Files (*.rpl)\0*.rpl\0All Files(*.*)\0*.*\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = str_filename;
	ofn.nMaxFile          = i_max_chars;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrFileTitle    = NULL;
	ofn.lpstrDefExt       = NULL;

	if (b_load)
	{
		ofn.Flags		= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR; 
		ofn.lpstrTitle	= "Load Replay File";

		// Get the file load name.
		if (GetOpenFileName((LPOPENFILENAME)&ofn))
		{
			return TRUE;
		}
	}
	else
	{
		ofn.Flags		= OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		ofn.lpstrTitle	= "Save Replay File";

		// Get the file load name.
		if (GetSaveFileName((LPOPENFILENAME)&ofn))
		{
			return TRUE;
		}
	}
	return FALSE;
}

//**********************************************************************************************
void ResetTerrainSkyTextures()
{
	// Invalidate terrain textures to cause re-render.
	// To do: communicate this via a message instead.
	if (CWDbQueryTerrain().tGet() != 0)
		CWDbQueryTerrain().tGet()->Rebuild(false);
}
