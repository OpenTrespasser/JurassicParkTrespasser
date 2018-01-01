/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of UIModes.hpp
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/UIModes.cpp                                                  $
 * 
 * 11    9/09/97 8:51p Agrant
 * Move, goto modes
 * 
 * 10    5/09/97 12:13p Agrant
 * Mouse player debug option
 * 
 * 9     4/08/97 6:42p Agrant
 * Debugging improvements for AI Test app.
 * Allows test app treatment of GUIApp AI's.
 * Better tools for seeing what is going on in an AI's brain.
 * 
 * 8     2/05/97 1:05p Agrant
 * Added a graph test mode for testing A* graph searches.
 * 
 * 7     11/14/96 11:17p Agrant
 * Added Club mode to fake collisions with dinos.
 * 
 * 6     11/02/96 7:12p Agrant
 * added meat and grass
 * 
 * 5     10/14/96 12:21p Agrant
 * Added walls to test app.
 * 
 * 4     10/10/96 7:18p Agrant
 * Modified for code spec.
 * 
 * 3     10/02/96 8:11p Agrant
 * Added Sheep and Wolves.
 * Added Influence Debugging.
 * Added AI frame rate counter.
 * Unified all meter/pixel conversions.
 * 
 * 2     9/27/96 5:44p Agrant
 * Added Trees
 * Added some notion of acceleration to dino model.
 * 
 * 1     9/03/96 8:08p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "UIModes.hpp"

#include "Lib/W95/WinInclude.hpp"
#include "Shell\WinShell.hpp"

#include "airesource.h"

EUIMode		euiMode;							// A global mode value for the user interface.
static HCURSOR hcursorModeCursor[euiEND];		// An array of cursors, one for each mode.

//*********************************************************************************************
//
void InitMode()
//
//	initializes the mode system
//
//**************************************
{
	HCURSOR hcursor_arrow =  LoadCursor(NULL,IDC_ARROW);  // Default cursor.
	HCURSOR hcursor_cross =  LoadCursor(NULL,IDC_CROSS);  // Crosshairs.

	// Default cursor for all modes is the arrow.
	for (int i = euiEND - 1 ; i >= 0 ; i--)
	{
		hcursorModeCursor[i] = hcursor_arrow;
	}

	// Cursor for adding dinos.
	hcursorModeCursor[euiADDDINOSAUR] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_DINO1)	);

	hcursorModeCursor[euiADDTREE] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_TREE_ADD)	);

	hcursorModeCursor[euiADDWALL] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_TREE_ADD)	);

	hcursorModeCursor[euiCURRENTLYADDINGWALL] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_TREE_ADD)	);

	hcursorModeCursor[euiADDWOLF] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_ADD_WOLF)	);

	hcursorModeCursor[euiADDSHEEP] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_ADD_SHEEP)	);

	hcursorModeCursor[euiREMOVEOBJECT] =	
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_DINO_REMOVE)	);

	hcursorModeCursor[euiCURRENTLYFACINGOBJECT] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_DINO_FACING)	);

	hcursorModeCursor[euiBEGINFACINGOBJECT] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_DINO_FACING)	);

	hcursorModeCursor[euiADDMEAT] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_MEAT)	);

	hcursorModeCursor[euiADDGRASS] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_GRASS));

	hcursorModeCursor[euiCLUB] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_CLUB));

	hcursorModeCursor[euiMOUSEPLAYER] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_PLAYER));

	hcursorModeCursor[euiSTARTMOVE] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_HAND));

	hcursorModeCursor[euiMOVING] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_HAND));

	hcursorModeCursor[euiGOTO] =		
		LoadCursor(	pwnsWindowsShell->hinsAppInstance,
		MAKEINTRESOURCE(ID_CURSOR_X));

	hcursorModeCursor[euiADDHINT] =	hcursor_cross;
}


//*********************************************************************************************
//
void SetMode(EUIMode eui)
{
	// Set the mode
	euiMode = eui;

	// Set the cursor if necessary.
	if (GetCursor() != hcursorModeCursor[eui])
	{
		if (hcursorModeCursor[eui])
 		{
			SetCursor(hcursorModeCursor[eui]);
		}
		else
			// Bad cursor.
			Assert(0);
	}
}

