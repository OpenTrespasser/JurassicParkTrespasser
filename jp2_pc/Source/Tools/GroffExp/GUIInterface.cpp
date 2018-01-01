/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1997
 *
 * Contents: General GUI interface routines for use in the various exporter modules.
 * 
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Tools/GroffExp/GUIInterface.cpp                                      $
 * 
 * 7     7/15/97 6:58p Gstull
 * Made organizational changes to make management of the exporter more reasonable.
 * 
 * 6     7/14/97 3:36p Gstull
 * Made substantial changes to support tasks for the August 18th milestone for attributes,
 * object representation, geometry checking, dialog interfacing, logfile support, string tables
 * and string support.
 * 
 * 5     6/18/97 7:33p Gstull
 * Added changes to support fast exporting.
 * 
 * 4     4/14/97 7:27p Gstull
 * Added changes to support log file control.
 * 
 * 3     4/13/97 11:26p Gstull
 * File hierarchy now places thel map and log files for each object in a Map\<object name> or
 * Log\<object name> to reduce clutter in the export directory.
 * 
 * 2     2/25/97 7:51p Gstull
 * Resolved issues with proper object placement in the scene and object validation.
 * 
 * 1     2/21/97 7:24p Gstull
 * 
 *********************************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <direct.h>
#include <string.h>

#include "Max.h"
#include "GroffExp.h"

#include "StandardTypes.hpp"
#include "GUIInterface.hpp"
#include "Lib/Groff/EasyString.hpp"


//******************************************************************************************
//
// Constructor.
//
CGUIInterface::CGUIInterface()
{
	// Set the last progress interval to an invalid value.
	iLastProgress = -1;

	// Set the instance of this DLL to null.
	hiGUIInstance = 0;

	// Initialize the interface pointer to null.
	pipInterface = 0;
}


//******************************************************************************************
//
// Destructor.  The member variable instance is not set to zero here since it is not clear
// how destructors are invoked with regard to DLL's and their linking/unlinking.
//
CGUIInterface::~CGUIInterface()
{
	// Set the last progress level value to an invalid value.
	iLastProgress = -1;

	// Set the instance of the DLL to null.
	// hiGUIInterface = 0;

	// Set the interface pointer to null.
	pipInterface = 0;
}


//******************************************************************************************
//
// Function for retrieving a string from the string table based upon an ID.
//
char* CGUIInterface::strGetString(int i_string_id)
{
	// Do we have a valid instance of our DLL?
	if (hiGUIInstance != 0)
	{
		// Yes! Attempt to lookup the string in the resource string table.  Were we successful?
		if (LoadString(hiGUIInstance, i_string_id, strBuffer, 4095))
		{
			// Yes! Return a pointer to the start of the string.
			return strBuffer;
		}
	}	

	// No! We could find the string, so return a null string.
	return 0;
}


//******************************************************************************************
//
void CGUIInterface::InfoMsg(int i_message_id)
{
	CEasyString estr_message(strGetString(i_message_id));
	CEasyString estr_title(strGetString(IDS_INFO_MSG));

	// Just throw up an informational message box.
	MessageBox(GetActiveWindow(), estr_message.strData(), estr_title.strData(), MB_ICONINFORMATION | MB_OK);
}


//******************************************************************************************
//
void CGUIInterface::InfoMsg(const char* str_message)
{
	CEasyString estr_message(str_message);
	CEasyString estr_title(strGetString(IDS_INFO_MSG));

	// Just throw up an informational message box.
	MessageBox(GetActiveWindow(), estr_message.strData(), estr_title.strData(), MB_ICONINFORMATION | MB_OK);
}


//******************************************************************************************
//
void CGUIInterface::InfoPrintf(int i_message_id, ...)
{
	CEasyString estr_format(strGetString(i_message_id));
	CEasyString estr_title(strGetString(IDS_INFO_MSG));

	// Setup a variable argument list parameter structure.
	va_list	va_parameters;

	// Setup the arguement list pointer to the first free argument.
	va_start(va_parameters,	i_message_id);

	// Send the message to the log file
	_vsnprintf(strBuffer, 4095, estr_format.strData(), va_parameters);

	// Reset the argument list pointer to null.
	va_end(va_parameters);

	// Just throw up an informational message box.
	MessageBox(GetActiveWindow(), strBuffer, estr_title.strData(), MB_ICONINFORMATION | MB_OK);
}


//******************************************************************************************
//
void CGUIInterface::InfoPrintf(const char* str_format, ...)
{
	CEasyString estr_format(str_format);
	CEasyString estr_title(strGetString(IDS_INFO_MSG));

	// Setup a variable argument list parameter structure.
	va_list	va_parameters;

	// Setup the arguement list pointer to the first free argument.
	va_start(va_parameters,	str_format);

	// Send the message to the log file
	_vsnprintf(strBuffer, 4095, estr_format.strData(), va_parameters);

	// Reset the argument list pointer to null.
	va_end(va_parameters);

	// Just throw up an informational message box.
	MessageBox(GetActiveWindow(), strBuffer, estr_title.strData(), MB_ICONINFORMATION | MB_OK);
}


//******************************************************************************************
//
bool CGUIInterface::bWarningMsg(int i_message_id)
{
	CEasyString estr_message(strGetString(i_message_id));
	CEasyString estr_title(strGetString(IDS_WARNING_MSG));

	// Yes!  Let the user decide what to do.
	return MessageBox(GetActiveWindow(), estr_message.strData(), estr_title.strData(), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL;
}


//******************************************************************************************
//
bool CGUIInterface::bWarningMsg(const char* str_message)
{
	CEasyString estr_message(str_message);
	CEasyString estr_title(strGetString(IDS_WARNING_MSG));

	// Yes!  Let the user decide what to do.
	return MessageBox(GetActiveWindow(), estr_message.strData(), estr_title.strData(), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL;
}


//******************************************************************************************
//
bool CGUIInterface::bWarningPrintf(int i_message_id, ...)
{
	CEasyString estr_format(strGetString(i_message_id));
	CEasyString estr_title(strGetString(IDS_INFO_MSG));

	// Setup a variable argument list parameter structure.
	va_list	va_parameters;

	// Setup the arguement list pointer to the first free argument.
	va_start(va_parameters,	i_message_id);

	// Send the message to the log file
	_vsnprintf(strBuffer, 4095, estr_format.strData(), va_parameters);

	// Reset the argument list pointer to null.
	va_end(va_parameters);

	// Just throw up an informational message box.
	return MessageBox(GetActiveWindow(), strBuffer, estr_title.strData(), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL;
}


//******************************************************************************************
//
bool CGUIInterface::bWarningPrintf(const char* str_format, ...)
{
	CEasyString estr_format(str_format);
	CEasyString estr_title(strGetString(IDS_WARNING_MSG));

	// Setup a variable argument list parameter structure.
	va_list	va_parameters;

	// Setup the arguement list pointer to the first free argument.
	va_start(va_parameters,	str_format);

	// Send the message to the log file
	_vsnprintf(strBuffer, 4095, estr_format.strData(), va_parameters);

	// Reset the argument list pointer to null.
	va_end(va_parameters);

	// Just throw up an informational message box.
	return MessageBox(GetActiveWindow(), strBuffer, estr_title.strData(), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL;
}


//******************************************************************************************
//
bool CGUIInterface::bErrorMsg(int i_message_id)
{
	CEasyString estr_message(strGetString(i_message_id));
	CEasyString estr_title(strGetString(IDS_ERROR_MSG));

	// Yes!  Let the user decide what to do.
	return MessageBox(GetActiveWindow(), estr_message.strData(), estr_title.strData(), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL;
}


//******************************************************************************************
//
bool CGUIInterface::bErrorMsg(const char* str_message)
{
	CEasyString estr_message(str_message);
	CEasyString estr_title(strGetString(IDS_ERROR_MSG));

	// Yes!  Let the user decide what to do.
	return MessageBox(GetActiveWindow(), estr_message.strData(), estr_title.strData(), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL;
}


//******************************************************************************************
//
bool CGUIInterface::bErrorPrintf(int i_message_id, ...)
{
	CEasyString estr_format(strGetString(i_message_id));
	CEasyString estr_title(strGetString(IDS_ERROR_MSG));

	// Setup a variable argument list parameter structure.
	va_list	va_parameters;

	// Setup the arguement list pointer to the first free argument.
	va_start(va_parameters, i_message_id);

	// Send the message to the log file
	_vsnprintf(strBuffer, 4095, estr_format.strData(), va_parameters);

	// Reset the argument list pointer to null.
	va_end(va_parameters);

	// Just throw up an informational message box.
	return MessageBox(GetActiveWindow(), strBuffer, estr_title.strData(), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL;
}


//******************************************************************************************
//
bool CGUIInterface::bErrorPrintf(const char* str_format, ...)
{
	CEasyString estr_format(str_format);
	CEasyString estr_title(strGetString(IDS_ERROR_MSG));

	// Setup a variable argument list parameter structure.
	va_list	va_parameters;

	// Setup the arguement list pointer to the first free argument.
	va_start(va_parameters,	str_format);

	// Send the message to the log file
	_vsnprintf(strBuffer, 4095, estr_format.strData(), va_parameters);

	// Reset the argument list pointer to null.
	va_end(va_parameters);

	// Just throw up an informational message box.
	return MessageBox(GetActiveWindow(), strBuffer, estr_title.strData(), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL;
}


//**********************************************************************************************
//
DWORD WINAPI dummy_fn(LPVOID arg)
{
	// Dummy function for progress meter.
	return(0);
}


//******************************************************************************************
//
void CGUIInterface::InitProgressMeter(char* str_title)
{
	LPVOID	lpv_arg = 0;

	// Setup the progress bar.  Did we succesfully allocate an interface pointer?
	if (pipInterface != 0)
	{
		// Set the progress meter value to an invalid value.
		iLastProgress = -1;

		// Yes! Now setup the progress counter.
		pipInterface->ProgressStart(_T(str_title), TRUE, dummy_fn, lpv_arg);
	}
}


//******************************************************************************************
//
bool CGUIInterface::bUpdateProgressMeter(int i_percent_complete)
{
	bool b_cancel = false;

	// Is the interface pointer valid?
	if (pipInterface != 0)
	{
		// Is this value the same as the last value? If it is, then don't waste CPU cycles.
		if (i_percent_complete != iLastProgress)
		{
			// Update the value.
			iLastProgress = i_percent_complete;

			// Request and update of the progress indicator.
			pipInterface->ProgressUpdate(i_percent_complete);

			// Does the user wish to abort for some reason?
			if (pipInterface->GetCancel())
			{
				// Through up a dialog box to make sure the user really wants to quit.
				int i_result = MessageBox(pipInterface->GetMAXHWnd(), _T(strGetString(IDS_ABORT_OP)), 
					_T(strGetString(IDS_QUESTION)), MB_ICONQUESTION | MB_YESNO);

				// Does the user want to quit?
				if (i_result == IDYES)
				{
					// Yes! Abort the export process.  Delete the quantizer then exit.
					b_cancel = true;

					// Destroy the progress meter as a matter of convenience.
					pipInterface->ProgressEnd();
				}
				else if (i_result == IDNO)
				{
					// No! Reset the cancel flag to false.
					pipInterface->SetCancel(FALSE);
				}
			}
		}
	}

	// Return the cancel status.
	return b_cancel;
}


//******************************************************************************************
//
void CGUIInterface::DestroyProgressMeter()
{
	// Is the interface pointer valid?
	if (pipInterface != 0)
	{
		// Remove the progress indicator.
		pipInterface->ProgressEnd();
	}
}


//******************************************************************************************
//
// Entry point to create a new directory.  Returns TRUE if the directory was successfully
// created in the event it did not exist or if it already exists.
//
bool CGUIInterface::bSetupDirectory(const char* str_directory_name)
{
	char str_current_directory[256];

	// Get the current working directory and save it locally.  Were we successful?
	if (_getcwd(str_current_directory, 255) == 0)
	{
		// No! Unable to get the current working directory so return an error.
		return false;
	}

	// Return the result.
	bool b_result = _chdir(str_directory_name) == 0;

	// Attempt to change to the requested directory.  Were we successful?
	if (!b_result)
	{
		// No! It is possible the directory does not exist, try and create it and return the result.
		b_result = _mkdir(str_directory_name) == 0;
	}
	
	// Restore the old directory path, Assume we can restore the old path.
	_chdir(str_current_directory);

	// Return a successful result.
	return b_result;
}


//******************************************************************************************
//
bool CGUIInterface::bBuildExportHierarchy(const char* str_export_filename)
{
	// The filename without it's extension.
	char str_filename[256];

	//
	// First make sure the filename we were passed is a valid filename.
	//

	// Make sure the string is long enough.  Is this export filename valid?
	if (str_export_filename == 0)
	{
		// Yes!  Return false since the hierarchy was no built.
		return false;
	}

	// Is the string empty?
	if (str_export_filename[0] == 0)
	{
		// Yes!  Return false since the hierarchy was not built.
		return false;
	}

	//
	// Now split the export filename into the path, filename and extension.
	//
	int i_strlen = strlen(str_export_filename);

	bool b_path_found = false;
	bool b_period_found = false;

	// Points to character past last character of file name.
	const char* str_filename_end = &str_export_filename[i_strlen];

	// Points to the first character of the file name.
	const char* str_filename_start = &str_export_filename[0];	

	// Scan backwards through the filename.
	for (int i_i = i_strlen-1; i_i > 0 && !b_path_found; i_i--) 
	{
		// Begin parsing the string.
		switch(str_export_filename[i_i])
		{
			case '.':
				// 
				// We are looking for the first period '.' upto the file path character '\' to designate
				// the file name. In the event that is detected, the strFilenameEnd pointer is set to 
				// point to the period to reflect the point in the export filename where the file 
				// extension begins.  In the event that the path character '\' is detected prior to a
				// period being detected, the filename without extension and filename with extension are
				// determined to be the same, which is not a problem at all.  The logfile and bitmap 
				// directory extensions are happily added to the end of the file name.
				//
				// In the event that the string terminates prior to the detection of either a '\' path 
				// character or a period.  The path is assumed to be the root of the default drive,  This
				// should not happen however since a drive is almost always supplied.
				//
				// Well... there it is.
				//

				// Is this the start of the filename extension?
				if (!b_period_found)
				{
					// Yes!  Setup a pointer here.
					str_filename_end = &str_export_filename[i_i];

					// Mark the period as having been found.
					b_period_found = true;
				}

				break;

			case '\\':
				// Setup a pointer to the start of the filename.
				str_filename_start = &str_export_filename[i_i+1];
				
				// Mark the file path as having been found.
				b_path_found = true;
				
				break;
		}
	}

	//
	// Now that we have exited the loop determine what was found.
	//

	// Separate the filename from the extension.
	int i_length = (int) ((uint) str_filename_end - (uint) str_filename_start);

	// Copy the entire filename.
	strcpy(strExportFile, str_filename_start);
 
	// Copy the subsection of the string and terminate it.
	strncpy(str_filename, str_filename_start, i_length);
	str_filename[i_length] = 0;

	// Build the directory names.
	strcpy(strBitmapDirName, strGetString(IDS_BITMAP_PATH));
	strcat(strBitmapDirName, str_filename);
	strcat(strBitmapDirName, "\\");
    
	strcpy(strLogfileDirName, strGetString(IDS_LOGFILE_PATH));
	strcat(strLogfileDirName, str_filename);
	strcat(strLogfileDirName, "\\");

	// Now construct the file path variable.
	i_length = (int) ((uint) str_filename_start - (uint) &str_export_filename[0]);
	strncpy(strExportPath, str_export_filename, i_length);
	strExportPath[i_length] = 0;

	// Build the directory paths.
	strcpy(strBitmapDirPath, strExportPath);
	strcat(strBitmapDirPath, strGetString(IDS_BITMAP_PATH));

	// Make sure the export directory is present.
	if (!bSetupDirectory(strBitmapDirPath))
	{
		// No! Return an error.
		return false;
	}

	// Add the object name to the path.
	strcat(strBitmapDirPath, str_filename);
	strcat(strBitmapDirPath, "\\");

	strcpy(strLogfileDirPath, strExportPath);
	strcat(strLogfileDirPath, strGetString(IDS_LOGFILE_PATH));

	// Make sure the export directory is present.
	if (!bSetupDirectory(strLogfileDirPath))
	{
		// No! Return an error.
		return false;
	}

	// Add the object name to the path.
	strcat(strLogfileDirPath, str_filename);
	strcat(strLogfileDirPath, "\\");

	//
	// Now that the paths have been constructed, attempt to create the directories.
	//

	// Setup the bitmap file directory. Were we successful?
	if (!bSetupDirectory(strBitmapDirPath))
	{
		// No! Return an error.
		return false;
	}

	// Setup the log file directory. Return the result.
	return bSetupDirectory(strLogfileDirPath);
}

	
//******************************************************************************************
//
const char* CGUIInterface::strGetExportPath()
{
	// Return the export file path.
	return &strExportPath[0];
}


//******************************************************************************************
//
const char* CGUIInterface::strGetExportFilename()
{
	// Return the export file name.
	return &strExportFile[0];
}


//******************************************************************************************
//
const char* CGUIInterface::strGetBitmapDirName()
{
	// Return the bitmap file directory name.
	return &strBitmapDirName[0];
}


//******************************************************************************************
//
const char* CGUIInterface::strGetBitmapDirPath()
{
	// Return the bitmap file directory name.
	return &strBitmapDirPath[0];
}


//******************************************************************************************
//
const char* CGUIInterface::strGetLogfileDirName()
{
	// Return the log file directory name.
	return &strLogfileDirName[0];
}


//******************************************************************************************
//
const char* CGUIInterface::strGetLogfileDirPath()
{
	// Return the log file directory name.
	return &strLogfileDirPath[0];
}


//******************************************************************************************
//
void CGUIInterface::BuildPath(char* str_path_and_file, const char* str_path, const char* str_file)
{
	// Place the path in the string first then add the file name.
	strcpy(str_path_and_file, str_path);
	strcat(str_path_and_file, str_file);
}
