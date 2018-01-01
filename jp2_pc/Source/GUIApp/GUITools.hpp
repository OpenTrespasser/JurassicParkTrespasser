/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Miscellaneous tools to assist the GUI.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/GUITools.hpp                                                  $
 * 
 * 13    98/04/05 20:40 Speter
 * Removed Raster.hpp from file.
 * 
 * 12    1/20/98 4:51p Pkeet
 * Added the 'bGetSaveFilename' function.
 * 
 * 11    12/23/97 2:18p Pkeet
 * Added the 'ResetTerrainSkyTextures' function.
 * 
 * 10    97/09/16 15:23 Speter
 * Added str_title param to bGetFilename().
 * 
 * 9     97/07/15 1:01p Pkeet
 * Added a parameter to set colour for crosshairs.
 * 
 * 8     97/04/15 3:03p Pkeet
 * Added terrain load dialog.
 * 
 * 7     4/09/97 11:17p Rwyatt
 * Added definition for replay load and save name function (bGetReplayFileName)
 * 
 * 6     96/12/31 16:43 Speter
 * Updated for rptr.
 * 
 * 5     11/07/96 5:58p Agrant
 * moved ppalGetPaletteFromResource() to WinRenderTools.cpp(.hpp)
 * so that it was accessible outside of GUIApp.
 * 
 * 4     8/15/96 4:35p Pkeet
 * Added the 'ppalGetPaletteFromResource' function.
 * 
 * 3     8/13/96 3:02p Pkeet
 * Added the 'bGetFilename' function.
 * 
 * 2     8/09/96 3:34p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_GUITOOLS_HPP
#define HEADER_GUIAPP_GUITOOLS_HPP


//
// Opaque declarations.
//

class CRaster;

//
// Global functions.
//

//**********************************************************************************************
//
void Crosshatch
(
	rptr<CRaster> pras	// The raster to draw on.
);
//
// Draws a pattern on the raster to test its integrity.
//
//**************************************

//**********************************************************************************************
//
void DrawCrosshair
(
	rptr<CRaster> pras,	// Pointer to the raster.
	int      i_x,		// X position of centre of crosshair.
	int      i_y,		// Y position of centre of crosshair.
	int      i_col		// If true draw a red crosshair, otherwise draw a green crosshair.
);
//
// Draws a crosshair at the specified location.
//
//**************************************

//**********************************************************************************************
//
void SetDlgItemFloat
(
	CWnd* pwnd, 					// Pointer to the calling window.
	int   i_item_id,				// Identifier of control.
	float f_value, 					// Value to set.
	int   i_significant_digits = 2	// Number of significant digits to display.
);
//
// Sets the text of a static control to a float.
//
// Notes:
//		The interface design for this function is intended to match SetDlgItemInt.
//
//**************************************

//**********************************************************************************************
//
float fGetDlgItemFloat
(
	CWnd* pwnd, 		// Pointer to the calling window.
	int   i_item_id 	// Identifier of control.
);
//
// Gets the text of a static control and converts it to a float.
//
//**************************************

//**********************************************************************************************
//
void MoveScrollbar
(
	UINT        nSBCode,	// Part of the scrollbar used.
	UINT        nPos,		// Integer position of the scrollbar.
	CScrollBar* pScrollBar	// Pointer to the MFC scrollbar.
);
//
// Moves the scrollbar.
//
// Notes:
//		The interface design for this function is intended to be used with MFC-handled
//		scrollbars.
//
//**************************************

//**********************************************************************************************
//
BOOL bGetFilename
(
	HWND        hwnd,			// Window handle of parent.
	char*       str_filename,	// Pointer to string to put filename.
	int         i_max_chars,	// Maximum number of characters for the filename.
	const char* str_title = "Load 3D File",
	const char* str_filter = "GROFF (*.grf)\0*.grf\0All (*.*)\0*.*\0"
								// Optional extension filter.
);
//
// Opens a dialog box and gets the name of a file.
//
// Returns 'true' if a file was selected.
//
//**************************************

//**********************************************************************************************
//
BOOL bGetSaveFilename
(
	HWND        hwnd,			// Window handle of parent.
	char*       str_filename,	// Pointer to string to put filename.
	int         i_max_chars,	// Maximum number of characters for the filename.
	const char* str_title = "Load 3D File",
	const char* str_filter = "GROFF (*.grf)\0*.grf\0All (*.*)\0*.*\0"
								// Optional extension filter.
);
//
// Opens a dialog box and gets the name of a file.
//
// Returns 'true' if a file was selected.
//
//**************************************


//**********************************************************************************************
//
bool bGetReplayFileName
(
	HWND	hwnd,			// Window handle of parent.
	char*	str_filename,	// Pointer to string to put filename.
	int		i_max_chars,	// Maximum number of characters for the filename.
	bool	b_load			// TRUE for load replay, FALSE for save replay
);
//
// Opens a dialog box and gets the name of a replay file.
//
// Returns 'true' if a file was selected.
//
//**************************************


//**********************************************************************************************
//
void ResetTerrainSkyTextures
(
);
//
// Destroys terrain and sky textures.
//
// Notes:
//		This function may be called when screen modes change or when the application exits.
//
//**************************************



#endif
