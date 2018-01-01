/***********************************************************************************************
 *
 * Copyright (c) 1996 DreamWorks Interactive, 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/SysLog.hpp                                                    $
 * 
 * 6     6/19/97 2:53p Mlange
 * Moved Profile.hpp to Lib/Sys.
 * 
 * 5     4/14/97 7:27p Gstull
 * Added changes to support log file control.
 * 
 * 4     2/21/97 6:55p Gstull
 * Added code to perform a number of additional checks   
 * on data as well as a new GUI interface class, removal 
 * of the bump map hack.
 * 
 * 3     1/22/97 11:14a Gstull
 * Added printf statement to the Syslog class.
 * 
 * 2     9/16/96 8:18p Gstull
 * Added project relative path to the include statements.
 * 
 * 1     9/12/96 5:46p Gstull
 *
 **********************************************************************************************/

#ifndef HEADER_COMMON_SYSLOG_HPP
#define HEADER_COMMON_SYSLOG_HPP

#include <stdio.h>
#include <string.h>

#ifdef USE_MAX_TYPES
#include "StandardTypes.hpp"
#else
#include "Common.hpp"
#endif


//**********************************************************************************************
//
class CSysLog
//
// Prefix: sl
//
// This class provides a general mechanism for logging comments into a specified logfile.
// 
// Example:
//
//**********************************************************************************************
{
private:
	bool	bEnableLogging;		// Is file logging enabled?
	FILE*   fFile;				// The file descriptor structure.
	bool    bFlushOutput;		// Should all output to the file be flushed after each write?

public:
	//******************************************************************************************
	//
	// Constructors and destructors.
	//
			CSysLog()	{ fFile = 0; bEnableLogging = true; };
			~CSysLog()	{ Close();	 };

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Enable
	( 
	)
	{
		bEnableLogging = true;
	}
	//
	// Function enables the logging of info to the logfile.
	//
	// Returns:
	//
	// Globals:
	//
	//******************************************************************************************

	//******************************************************************************************
	//
	void Disable
	( 
	)
	{
		bEnableLogging = false;
	}
	//
	// Function disables the logging of info to the file.
	//
	// Returns:
	//
	// Globals:
	//
	//******************************************************************************************

	//******************************************************************************************
	//
	bool Open
	( 
		const char* str_filename, 
		bool		b_flush = true 
	)
	//
	// Function responsible for opening the logfile.
	//
	// Returns:
	//		true indicates that the open request was successful.
	//
	// Globals:
	//		fFile		 - File descriptor for the logfile.
	//		bFlushOutput - boolean indicating whether each invocation is flushed to the disk.
	//
	//******************************************************************************************
	{	
		// Attempt to open the file.
		fFile = fopen(str_filename, "w"); 

		// Were we successful?
		if ( fFile != 0 )
		{
			// Does the user wish for all writes to be immediately flushed?
			bFlushOutput = b_flush; 

			// Return a successful result
			return true;
		}

		// Return an error
		return false;
	};


	//******************************************************************************************
	//
	bool Close
	(
	)
	//
	// Function responsible for closing the logfile, if it is open.
	//
	// Returns:
	//		true indicates that the open request was successful.
	//
	// Globals:
	//		fFile		 - File descriptor for the logfile.
	//
	//******************************************************************************************
	{
		// Is the logfile still open?
		if (fFile != 0) 
		{ 
			// Then close it.
			if (fclose(fFile) == 0)
			{
				// Mark it as closed.
				fFile = 0; 
			}
			else
			{
				// Return a error
				return false;
			}
		} 

		// Return a successful result.
		return true;
	};


	//******************************************************************************************
	//
	void Printf
	( 
		const char* str_format,					// Arbitrary string message.
		...										// Variable parameter list.
	);
	//
	// Function responsible for logging a formatted message and and variables to the logfile.
	//
	// Returns:
	//		Nothing.
	//
	// Globals:
	//		fFile		 - File descriptor for the logfile.
	//		bFlushOutput - boolean indicating whether each invocation is flushed to the disk.
	//
	//******************************************************************************************


	//******************************************************************************************
	//
	void Msg
	( 
		const char* str_message,				// Arbitrary string message.
		uint8		u1_nest_level = 0			// Nesting level in 4 space increments.
	);
	//
	// Function responsible for logging a message to the logfile.
	//
	// Returns:
	//		Nothing.
	//
	// Globals:
	//		fFile		 - File descriptor for the logfile.
	//		bFlushOutput - boolean indicating whether each invocation is flushed to the disk.
	//
	//******************************************************************************************

	
	//******************************************************************************************
	//
	void Msg
	( 
		const char* str_message,				// Arbitrary string message.
		int			i_value,					// Integer value.
		uint8		u1_nest_level = 0			// Nesting level in 4 space increments.
	);
	//
	// Function responsible for logging a message and a int value to the logfile.
	//
	// Returns:
	//		Nothing.
	//
	// Globals:
	//		fFile		 - File descriptor for the logfile.
	//		bFlushOutput - boolean indicating whether each invocation is flushed to the disk.
	//
	//******************************************************************************************
	

	//******************************************************************************************
	//
	void Msg
	( 
		const char* str_message,				// Arbitrary string message.
		uint32		u_value,					// uint32 value.
		uint8		u1_nest_level = 0			// Nesting level in 4 space increments.
	);
	//
	// Function responsible for logging a message and a uint32 value to the logfile.
	//
	// Returns:
	//		Nothing.
	//
	// Globals:
	//		fFile		 - File descriptor for the logfile.
	//		bFlushOutput - boolean indicating whether each invocation is flushed to the disk.
	//
	//******************************************************************************************


	//******************************************************************************************
	//
	void Msg
	( 
		const char* str_message,				// Arbitrary string message.
		float		f_value,					// Floating point value.
		uint8		u1_nest_level = 0			// Nesting level in 4 space increments.
	);
	//
	// Function responsible for logging a message and a floting point value to the logfile.
	//
	// Returns:
	//		Nothing.
	//
	// Globals:
	//		fFile		 - File descriptor for the logfile.
	//		bFlushOutput - boolean indicating whether each invocation is flushed to the disk.
	//
	//******************************************************************************************


	//******************************************************************************************
	//
	void Buffer
	( 
		const char* str_message,				// Arbitrary string message.
		void*		p_data,						// Data buffer 
		int			i_count,					// Byte count
		uint8		u1_nest_level = 0			// Nesting level in 4 space increments.
	);
	//
	// Function responsible for logging a message and a floting point value to the logfile.
	//
	// Returns:
	//		Nothing.
	//
	// Globals:
	//		fFile		 - File descriptor for the logfile.
	//		bFlushOutput - boolean indicating whether each invocation is flushed to the disk.
	//
	//******************************************************************************************
};

#endif
