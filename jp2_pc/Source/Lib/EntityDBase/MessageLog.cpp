/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of "MessageLog.hpp."
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageLog.cpp                                        $
 * 
 * 1     10/21/96 6:20p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "gblinc/common.hpp"
#include "MessageLog.hpp"


//
// Defines.
//

// Define the number of lines in the message log.
#define iNUM_LINES_IN_MESSAGELOG (25)

// Define the number of characters across the screen in the message log.
#define iNUM_CHARS_PER_LINE_IN_MESSAGELOG (80)


//*********************************************************************************************
//
class CMessageLog
//
// Class to initialize and close the message text file.
//
// Prefix: msglog
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// CMessageLog constructor and destructor.
	//

	//*****************************************************************************************
	CMessageLog()
	{
		// Open the message log file.
		conMessageLog.OpenFileSession("MessageLog.txt");

		// Add log-specific header.
		conMessageLog.Print("\nStarting message log...\n");
	}

	//*****************************************************************************************
	~CMessageLog()
	{
		//
		// Temporary fix for a bug where the last n-lines of text are not written to file.
		//
		for (int i_line = 0; i_line < iNUM_LINES_IN_MESSAGELOG; i_line++)
			conMessageLog.Print("\n");
		conMessageLog.Flush();

		// Close the file and write to disk.
		conMessageLog.CloseFileSession();
	}

};


//
// Global variables.
//
CConsoleBuffer conMessageLog(iNUM_CHARS_PER_LINE_IN_MESSAGELOG, iNUM_LINES_IN_MESSAGELOG);


//
// Module-specific variables.
//

//
// Variable to instantiate the message log and close it down again automatically at runtime.
// Note that this declaration should happen after "conMessageLog."
//
CMessageLog msglogLog;
