/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Console interfaces.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/ConIO.cpp                                                    $
 * 
 * 20    98/09/30 18:57 Speter
 * Mo phys lines.
 * 
 * 19    98/09/10 1:11 Speter
 * Shadows has 80 cols.
 * 
 * 18    98/08/21 18:03 Speter
 * Increased shadow console size.
 * 
 * 17    98/07/30 22:23 Speter
 * Increased size of physics stats window.
 * 
 * 16    7/26/98 6:11p Pkeet
 * Added switch to prevent dialogs.
 * 
 * 15    98/07/25 23:45 Speter
 * Added physics stats window.
 * 
 * 14    7/23/98 6:29p Mlange
 * Changed size of terrain console.
 * 
 * 13    4/02/98 5:24p Mlange
 * Increased terrain console size.
 * 
 * 12    4/01/98 5:47p Pkeet
 * Added code to activate stats without making a new dialog.
 * 
 * 11    97/10/17 14:30 Speter
 * Make conStd log to a file.
 * 
 * 10    10/09/97 5:19p Mlange
 * Changed conTerrain size.
 * 
 *********************************************************************************************/

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "ConIO.hpp"
#include "Lib/Sys/StdDialog.hpp"

#include <string.h>

//
// Macros.
//

// Switch to enable or disable using a separate display window.
#define bSEPARATE_WINDOW_DISPLAY (1)


//
// Class implementations.
//

//**********************************************************************************************
//
// CConsoleEx implementation.
//

	//*********************************************************************************************
	CConsoleEx::CConsoleEx(const char* str_conname, const char* str_log,
		uint u_chars_per_line, uint u_lines_per_screen)
		: CConsoleBuffer(u_chars_per_line, u_lines_per_screen)
	{
		Assert(strlen(str_conname) < 64);
		// Initialize object parameters.
		Initialize();

		if (str_log)
			// Open a text file for this console.
			OpenFileSession(str_log, true);

		// Copy the console's name.
		strcpy(strConName, str_conname);
	}

	//*********************************************************************************************
	CConsoleEx::~CConsoleEx()
	{
		delete pdlgConOut;
	}

	//*********************************************************************************************
	void CConsoleEx::SetActive(bool b_active, bool b_make_new)
	{
	#if bSEPARATE_WINDOW_DISPLAY
		if (b_active && pdlgConOut == 0 && b_make_new)
		{
			// Open dialog first time.
			pdlgConOut = new CStdDialog(strConName, prnshMain, this);
		}

		// Show or hide the dialog.
		if (pdlgConOut)
			pdlgConOut->ShowWindow(b_active);
	#endif // bSEPARATE_WINDOW_DISPLAY

		CConsoleBuffer::SetActive(b_active);
	}

	//*********************************************************************************************
	void CConsoleEx::SetWindowText(const char* str)
	{
		// Copy the console's name.
		strcpy(strConName, str);

	#if bSEPARATE_WINDOW_DISPLAY
		if (pdlgConOut)
			pdlgConOut->SetWindowText(str);
	#endif // bSEPARATE_WINDOW_DISPLAY
	}

	//*********************************************************************************************
	void CConsoleEx::Initialize()
	{
		pdlgConOut  = 0;
		*strConName = 0;
		SetActive(false);
	}

	//*********************************************************************************************
	void CConsoleEx::Show() const
	{
		// If the console is not active, do nothing.
		if (!bIsActive())
			return;

		// Try writing to the Dialog.
	#if bSEPARATE_WINDOW_DISPLAY
		if (pdlgConOut)
			if (pdlgConOut->bIsVisible())
			{
				CConsoleBuffer::Show(pdlgConOut->hwndGet());
			}
	#endif // bSEPARATE_WINDOW_DISPLAY

		// Flush if appropriate.
		//if (bFlushAlways)
			//fflush(pfileSession);
	}

//
// Global variables.
//

// The handle of the standard output dialog box when active.
CConsoleEx conStd("Std Console", "standard.txt");
CConsoleEx conArtStats("Art Stats", "ArtStats.txt");
CConsoleEx conAI("AI", "ai.txt");
CConsoleEx conPhysics("Physics", "physics.txt");
CConsoleEx conPhysicsStats("Physics Stats", "physics-stats.txt", 60, 40);
CConsoleEx conShadows("Shadows", "shadows.txt", 80, 60);
CConsoleEx conTerrain("Terrain", "terrain.txt", 60, 55);
CConsoleEx conDepthSort("Depth Sort", "dsort.txt");
CConsoleEx conPredictMovement("Predict Movement", "predict.txt");
CConsoleEx conOcclusion("Occlusion", "occlusion.txt");
CConsoleEx conHardware("Hardware", "Hardware.txt", 60, 40);
