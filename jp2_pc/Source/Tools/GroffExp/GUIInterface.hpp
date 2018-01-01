/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1996
 *
 * Contents: General GUI interface routines for use in the various exporter modules.
 * 
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/GUIInterface.hpp                                      $
 * 
 * 5     7/15/97 6:58p Gstull
 * Made organizational changes to make management of the exporter more reasonable.
 * 
 * 4     7/14/97 3:36p Gstull
 * Made substantial changes to support tasks for the August 18th milestone for attributes,
 * object representation, geometry checking, dialog interfacing, logfile support, string tables
 * and string support.
 * 
 * 3     4/14/97 7:27p Gstull
 * Added changes to support log file management.
 * 
 * 2     2/25/97 7:51p Gstull
 * Resolved issues with proper object placement in the scene and object validation.
 * 
 * 1     2/21/97 7:24p Gstull
 * 
 *********************************************************************************************/

#ifndef HEADER_TOOLS_GROFFEXP_GUIINTERFACE_HPP
#define HEADER_TOOLS_GROFFEXP_GUIINTERFACE_HPP

#include <stdio.h>
#include <stdarg.h>

#include "Max.h"

// There is some weird definition which breaks the GetMessage call in the class CFileIO.
#ifdef GetMessage
#undef GetMessage
#endif

#include "StandardTypes.hpp"
#include "Lib/Groff/EasyString.hpp"


class CGUIInterface
//
// Prefix: gui
//
// This class is intended to provide a host of basic GUI support functions for all the classes
// which interact with the Groff file exporter.  The intention here is that all "Windows" style
// code can be locally contained in a single place.
//
//*********************************************************************************************
{
private:

	bool	    bUseLogfiles;
	int			iLastProgress;
	HINSTANCE	hiGUIInstance;
	Interface*	pipInterface;
	char		strExportPath[256];
	char		strExportFile[64];
	char		strLogfileDirName[64];
	char		strLogfileDirPath[256];
	char		strBitmapDirName[64];
	char		strBitmapDirPath[256];
	char		strBuffer[4096];


	//******************************************************************************************
	// Define a dummy function for the progress meter.
	//
	DWORD WINAPI fn
	(
		LPVOID arg
	);


	//******************************************************************************************
	// Entry point to create a new directory.  Returns TRUE if the directory was successfully
	// created in the event it did not exist or if it already exists.
	//
	bool bSetupDirectory
	(
		const char* str_directory_name
	);


public:

	//******************************************************************************************
	// Initialize the GUI Interface class private variables.
	//
	CGUIInterface
	(
	);


	//******************************************************************************************
	// Initialize the GUI Interface class private variables.
	//
	~CGUIInterface
	(
	);


	//******************************************************************************************
	//
	// Initialize the GUI Interface class private variables.
	//
	void Initialize
	(
	);


	//******************************************************************************************
	// Configure the log file use.
	//
	void GenerateLogfiles
	(
		bool b_enable
	) 
	{ 
		bUseLogfiles = b_enable; 
	};
	

	//******************************************************************************************
	// Configure the log file use.
	//
	bool bGenerateLogfiles
	(
	)
	{ 
		return bUseLogfiles; 
	};
	

	//******************************************************************************************
	// Set the instance of this DLL.
	//
	void SetInstance
	(
		HINSTANCE hi_instance
	)
	{
		hiGUIInstance = hi_instance; 
	};


	//******************************************************************************************
	// Get the instance of this DLL.
	//
	HINSTANCE GetInstance
	(
	)
	{
		return hiGUIInstance; 
	};


	//******************************************************************************************
	// Function for looking up strings in the resource string table.
	//
	char* strGetString
	(
		int i_string_id
	);


	//******************************************************************************************
	// Display a dialog box which contains the string of the appropriate ID from the resource
	// string table.
	//
	void InfoMsg
	(
		int	i_message_id
	);


	//******************************************************************************************
	// Display a dialog box which contains the string of the appropriate ID from the resource
	// string table.
	//
	void InfoMsg
	(
		const char* str_message
	);


	//******************************************************************************************
	// Display a dialog box which uses a format stringcontains the string of the appropriate ID from the resource
	// string table, possibly followed by a format string and an arbitrary number of arguments.
	//
	void InfoPrintf
	(
		int i_message_id,
		...
	);


	//******************************************************************************************
	// Display a dialog box which contains a format string string of the appropriate ID from the resource
	// string table, possibly followed by a format string and an arbitrary number of arguments.
	//
	void InfoPrintf
	(
		const char* str_format,
		...
	);


	//******************************************************************************************
	// Display a dialog box which contains the string of the appropriate ID from the resource
	// string table.
	//
	bool bWarningMsg
	(
		int	i_message_id
	);


	//******************************************************************************************
	// Display a dialog box which contains the string of the appropriate ID from the resource
	// string table.
	//
	bool bWarningMsg
	(
		const char* str_message
	);


	//******************************************************************************************
	// Display a dialog box which uses a format stringcontains the string of the appropriate ID from the resource
	// string table, possibly followed by a format string and an arbitrary number of arguments.
	//
	bool bWarningPrintf
	(
		int i_message_id,
		...
	);


	//******************************************************************************************
	// Display a dialog box which contains a format string string of the appropriate ID from the resource
	// string table, possibly followed by a format string and an arbitrary number of arguments.
	//
	bool bWarningPrintf
	(
		const char* str_format,
		...
	);


	//******************************************************************************************
	// Display a dialog box which contains the string of the appropriate ID from the resource
	// string table.
	//
	bool bErrorMsg
	(
		int	i_message_id
	);


	//******************************************************************************************
	// Display a dialog box which contains the string of the appropriate ID from the resource
	// string table.
	//
	bool bErrorMsg
	(
		const char* str_message
	);


	//******************************************************************************************
	// Display a dialog box which uses a format stringcontains the string of the appropriate ID from the resource
	// string table, possibly followed by a format string and an arbitrary number of arguments.
	//
	bool bErrorPrintf
	(
		int i_message_id,
		...
	);


	//******************************************************************************************
	// Display a dialog box which contains a format string string of the appropriate ID from the resource
	// string table, possibly followed by a format string and an arbitrary number of arguments.
	//
	bool bErrorPrintf
	(
		const char* str_format,
		...
	);


	//******************************************************************************************
	//
	// Set the interface pointer for this instance of the export DLL.
	//
	void SetInterface
	(
		Interface* pip_interface
	) 
	{ 
		pipInterface = pip_interface; 
	};


	//******************************************************************************************
	// 
	// Get the interface pointer for this instance of the export DLL.
	//
	Interface* GetInterface
	(
	)
	{
		return pipInterface; 
	};
	

	//******************************************************************************************
	//
	// Provide the string message which is displayed with the progress meter and set the percent
	// complete to 0%.  Successful completion of this entry point requires a valid Interface 
	// pointer be setup prior to invocation of this function.  This function automatically places
	// the MAX GUI into a busy state, which changes the mouse pointer to an hourglass.  The user
	// MUST call the destroy progress meter entry point prior to existing the export DLL or the
	// interface will remain in a busy state, thus precluding any additional user input.
	//
	void InitProgressMeter
	(
		char* str_title
	);


	//******************************************************************************************
	//
	// This method is called by the user to update the percent complete status bar.  The range
	// of values varies from 0 to 100.  Any value out of this range is automatically clipped 
	// into the appropriate range.  The return value is intended to indicate whether the user
	// has selected the cancel button (TRUE) indicating that the current activity should be
	// terminated.  When the user has requested to terminate through the CANCEL button, this 
	// function automatically calls DestroyProgressMeter since this progress meter will no 
	// longer be needed.
	//
	bool bUpdateProgressMeter
	(
		int i_percent_complete
	);


	//******************************************************************************************
	//
	// When use of the progress meter is complete, this entry point destroys the progress meter,
	// removes the message from the MAX message bar and return the GUI state to the non-busy
	// mode.  This must be called prior to exiting the export DLL or the GUI will not allow any
	// user input.  Hopefully this functionality can be added to the destructor just in case the
	// caller forgets to invoke it manually.
	//
	void DestroyProgressMeter
	(
	);


	//******************************************************************************************
	//
	// Take the export file name and construct the directories and groff file hierarchy. 
	//
	bool bBuildExportHierarchy
	(
		const char* str_export_filename
	);
	

	//******************************************************************************************
	//
	// Return the path of the Groff file and related directories.
	//
	const char* strGetExportPath
	(
	);


	//******************************************************************************************
	//
	// Return the Groff filename.
	//
	const char* strGetExportFilename
	(
	);


	//******************************************************************************************
	//
	// Return the absolute path used by any bitmaps associated with this export operation.
	//
	const char* strGetBitmapDirName
	(
	);


	//******************************************************************************************
	//
	// Return the absolute path used by any bitmaps associated with this export operation.
	//
	const char* strGetBitmapDirPath
	(
	);


	//******************************************************************************************
	//
	// Return the logfile directory name.
	//
	const char* strGetLogfileDirName
	(
	);


	//******************************************************************************************
	//
	// Return the logfile directory name.
	//
	const char* strGetLogfileDirPath
	(
	);


	//******************************************************************************************
	//
	// Construct a complete path and file from a path and a file.
	//
	void BuildPath
	(
		char*		str_path_and_file, 
		const char* str_path, 
		const char* str_file
	);
};

#endif