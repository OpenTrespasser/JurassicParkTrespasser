/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of InitSys.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/InitSys.cpp                                                   $
 * 
 * 8     8/27/98 9:32p Asouth
 * MW runtime does not currently support new-failure handlers
 * 
 * 7     5/08/97 7:12p Cwalla
 * Added change to allow guiapp to run from any
 * directory.
 * 
 * 6     96/10/18 19:03 Speter
 * Took out include of <new.h>, now that it's in Common.hpp.
 * 
 * 5     96/07/03 12:58 Speter
 * Moved many files to new directories, changed include statements.
 * 
 * 4     96/05/29 17:10 Speter
 * Changed init classes to have a per-file static instance rather than use inheritance to
 * enforce order.
 * 
 * 3     96/05/14 10:50 Speter
 * Changed TerminalError call to use new bRetry parameter.
 * 
 * 2     96/05/13 19:59 Speter
 * Now call TerminalError, added ERROR_NOMEMORY resource string.
 * 
 * 1     96/05/13 18:43 Speter
 * New module, provide system-level init code.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Lib/Sys/Errors.hpp"
#include "InitSys.hpp"

#include <direct.h>
#include <stdlib.h>

//**********************************************************************************************
//
// CInitSys implementation.
//

	//******************************************************************************************
	//
	static int iNewHandler
	(
		size_t sz						// Size of memory requested.
	)
	//
	// Handles failure of operator new by putting up a Retry/Cancel dialog.
	//
	// Returns:
	//		The protocol of this callback is to return 1 if operator new is to try again, 
	//		and 0 if it is to fail and return 0.
	//		We return 1 if the user hits Retry, and exit the program otherwise.
	//
	// Cross references:
	//		Installed by CInitSys class via _set_new_handler().
	//
	//**********************************
	{
		TerminalError(ERROR_NOMEMORY, /*bRetry*/ 1);

		// If TerminalError returns, user wants to retry.  Returning 1 will do that.
		return 1;
	}

	//******************************************************************************************
	CInitSys::CInitSys()
	{
		char*c_basedir;

		// Boilerplate code to ensure we execute only once.
		static bool b_init = 0;
		if (b_init)
			return;
		b_init = 1;

	// MW does not currently support these two extensions
	#ifndef __MWERKS__
		// Set our own new_handler to catch new failures.
		_set_new_handler(iNewHandler);

		// Make malloc() call the new handler when it fails.
		// Although our code does not call malloc(), system functions like fopen() might.
		_set_new_mode(1);
	#endif

		c_basedir = getenv( "BASE_DIR" );

		if( c_basedir != NULL ) 
			chdir( c_basedir );
	}

	//******************************************************************************************
	CInitSys::~CInitSys()
	{
		// No destruction code needed at this time.
	}

