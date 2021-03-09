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
 * $Log:: /JP2_PC/Source/Lib/Sys/ConIO.hpp                                                    $
 * 
 * 12    98/07/25 23:45 Speter
 * Added physics stats window.
 * 
 * 11    4/01/98 5:47p Pkeet
 * Added code to activate stats without making a new dialog.
 * 
 * 10    98.01.28 12:09p Mmouni
 * Increased size of character buffer for console name.  It was being overrun by long
 * performance counter names.
 * 
 * 9     1/13/98 9:33p Agrant
 * Made SetActive virtual
 * 
 * 8     97/10/07 5:34p Pkeet
 * Added 'conOcclusion.'
 * 
 * 7     97/10/06 10:59 Speter
 * Added destructor which deletes owned dialog.  Added SetWindowText and SetActive functions.
 * Made conStd a CConsoleEx.
 * 
 * 6     97/07/30 4:06p Pkeet
 * Added the movement prediction output console.
 * 
 * 5     97/07/23 3:17p Pkeet
 * Added an ArtStats dialog.
 * 
 * 4     97/05/13 16:28 Speter
 * Added dimensions to CConsoleEx constructor.
 * 
 * 3     97/04/06 1:53p Pkeet
 * Added console to write depthsort stats.
 * 
 * 2     97/04/03 3:29p Pkeet
 * Move 'bActive' from 'CConsoleEx' to 'CConsoleBuffer.'
 * 
 * 1     97/04/03 1:40p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_SYS_CONIO_HPP
#define HEADER_LIB_SYS_CONIO_HPP


//
// Required includes.
//
#include "Textout.hpp"


//
// Forward class declarations.
//
class CStdDialog;


//
// Class definitions.
//

//*********************************************************************************************
//
class CConsoleEx : public CConsoleBuffer
//
// Extension to the console buffer to use specific target dialogs.
//
// Prefix: con
//
//**************************************
{
private:

	CStdDialog* pdlgConOut;			// Pointer to a dialog box for output.
	char        strConName[256];	// Console name.

public:

	
	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CConsoleEx
	(
		const char* str_conname = "", const char* str_log = 0,
		uint u_chars_per_line   = 80,
		uint u_lines_per_screen = 25
	);

	~CConsoleEx();
	
	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	void SetDialog
	(
		CStdDialog* pdlg_out = 0	// Dialog window.
	);
	//
	// Sets the dialog and the main window for the console.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual void Show
	(
	) const override;
	//
	// Displays characters in buffer onto the text output dialog box. Sizes the font to fit
	// the entire buffer in the standard dialog box. Does nothing if the dialog box is closed.
	//
	//**************************************

	//*****************************************************************************************
	//
	void SetWindowText
	(
		const char* str						// String to set window title to.
	);
	//
	//
	// Overrides.
	//

	//*****************************************************************************************
	//
	virtual void SetActive
	(
		bool b_active,			// New state for the console buffer.
		bool b_make_new = true	// Makes a new dialog window as required.
	);
	//
	// Sets console buffer active, as well as showing or hiding the dialog window.
	//
	//**********************************

private:

	//*****************************************************************************************
	void Initialize();
};


//
// Global console variables.
//

extern CConsoleEx conStd;
extern CConsoleEx conArtStats;
extern CConsoleEx conAI;
extern CConsoleEx conPhysics;
extern CConsoleEx conPhysicsStats;
extern CConsoleEx conShadows;
extern CConsoleEx conTerrain;
extern CConsoleEx conDepthSort;
extern CConsoleEx conPredictMovement;
extern CConsoleEx conOcclusion;
extern CConsoleEx conHardware;


#endif // HEADER_LIB_SYS_CONIO_HPP
