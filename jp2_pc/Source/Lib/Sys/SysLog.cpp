/***********************************************************************************************
 * Copyright (c) DreamWorks Interactive. 1996
 *
 * Implementation of CSysLog.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/SysLog.cpp                                                    $
 * 
 * 7     8/26/98 5:25p Asouth
 * Fixed loop variable scope
 * 
 * 6     4/15/97 10:36p Gstull
 * Added changes to support export options specified in a dialog box at export time.
 * 
 * 5     4/14/97 7:19p Gstull
 * Added changes to support log file management.
 * 
 * 4     2/21/97 6:54p Gstull
 * Added code to perform a number of additional checks   
 * on data as well as a new GUI interface class, removal 
 * of the bump map hack.
 * 
 * 3     1/22/97 11:14a Gstull
 * Added printf statement to the Syslog class.
 * 
 * 2     9/16/96 8:24p Gstull
 * Added changes to integrate the new loader stuff with the existing GUI App.
 * 
 * 1     9/12/96 5:29p Gstull
 *
 **********************************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef USE_MAX_TYPES
#include "StandardTypes.hpp"
#else
#include "Common.hpp"
#endif

#include "SysLog.hpp"


//**********************************************************************************************
//
void CSysLog::Printf(const char* str_format, ...)
{
	// Make sure the logfile is open
	if (fFile != 0 && bEnableLogging) 
	{
		// Setup a variable argument list parameter structure.
		va_list	va_parameters;

		// Setup the arguement list pointer to the first free argument.
		va_start(va_parameters, str_format);

		// Send the message to the log file
		vfprintf(fFile, str_format, va_parameters);

		// Should this output be flushed to the logfile immediately?
		if (bFlushOutput)
		{
			// Flush the output to the file.
			fflush(fFile);
		}

		// Reset the argument list pointer to null.
		va_end(va_parameters);
	}
}


//**********************************************************************************************
//
void CSysLog::Msg(const char* str_message, uint8 u1_nest_level)
{
	// Make sure the logfile is open
	if (fFile != 0 && bEnableLogging) 
	{
		// Is the nesting level > 0?
		for (uint u_i = 0; u_i < u1_nest_level; u_i++)
		{
			// Nest the comment to the correct level.
			fprintf(fFile, "    ");
		}

		// Send the message to the log file
		fprintf(fFile, "%s\n", str_message);

		// Should this output be flushed to the logfile immediately?
		if (bFlushOutput)
		{
			// File the output to the file.
			fflush(fFile);
		}
	}
}


//**********************************************************************************************
//
void CSysLog::Msg(const char* str_message, int i_value, uint8 u1_nest_level)
{
	// Make sure the logfile is open
	if (fFile != 0 && bEnableLogging)
	{
		// Is the nesting level > 0?
		for (uint u_i = 0; u_i < u1_nest_level; u_i++)
		{
			// Nest the comment to the correct level.
			fprintf(fFile, "    ");
		}
		
		// Send the message to the log file
		fprintf(fFile, "%s: %d\n", str_message, i_value);

		// Should this output be flushed to the logfile immediately?
		if (bFlushOutput)
		{
			fflush(fFile);
		}
	}
}


//**********************************************************************************************
//
void CSysLog::Msg(const char* str_message, uint32 u_value, uint8 u1_nest_level)
{
	// Make sure the logfile is open
	if (fFile != 0 && bEnableLogging)
	{
		// Is the nesting level > 0?
		for (uint u_i = 0; u_i < u1_nest_level; u_i++)
		{
			// Nest the comment to the correct level.
			fprintf(fFile, "    ");
		}
		
		// Send the message to the log file
		fprintf(fFile, "%s: 0x%X\n", str_message, u_value);

		// Should this output be flushed to the logfile immediately?
		if (bFlushOutput)
		{
			fflush(fFile);
		}
	}
}


//**********************************************************************************************
//
void CSysLog::Msg(const char* str_message, float f_value, uint8 u1_nest_level)
{
	// Make sure the logfile is open
	if (fFile != 0 && bEnableLogging)
	{
		// Is the nesting level > 0?
		for (uint u_i = 0; u_i < u1_nest_level; u_i++)
		{
			// Nest the comment to the correct level.
			fprintf(fFile, "    ");
		}
		
		// Send the message to the log file
		fprintf(fFile, "%s: %f\n", str_message, f_value);

		// Should this output be flushed to the logfile immediately?
		if (bFlushOutput)
		{
			// fl
			fflush(fFile);
		}
	}
}


//**********************************************************************************************
//
void CSysLog::Buffer(const char* str_message, void* v_data, int i_count, uint8 u1_nest_level)
{
	uint8*	u1_data = (uint8 *) v_data;


	// Make sure the logfile is open
	if (fFile != 0 && bEnableLogging)
	{
		// Are the parameters reasonable?
		if (i_count <= 0)
		{
			return;
		}

		// Is the nesting level > 0?
		uint u_i;
		for (u_i = 0; u_i < u1_nest_level; u_i++)
		{
			// Nest the comment to the correct level.
			fprintf(fFile, "    ");
		}
		
		// Send the message to the log file
		fprintf(fFile, "%s\n", str_message);

		// Process the buffer. 
		for (u_i = 0; u_i < (uint) i_count; u_i += 16)
		{
			// Is the nesting level > 0?
			for (uint8 u_j = 0; u_j < u1_nest_level; u_j++)
			{
				fprintf(fFile, "    ");
			}
		
			fprintf(fFile, "%4X:", u_i);

			uint u_k;
			for (u_k = 0; u_k < 16; u_k++)
			{
				// Don't print anthing not requested.
				if ((u_i + u_k) < (uint) i_count) 
				{
					fprintf(fFile, " %.2X", u1_data[u_i+u_k]);
				}
				else
				{
					fprintf(fFile, "   ");
				}
			}

			fprintf(fFile, "   ");
			for (u_k = 0; u_k < 16; u_k++)
			{
				// Don't print anthing not requested.
				if ((u_i + u_k) < (uint) i_count) 
				{
					// Display the printable characters
					if (u1_data[u_i + u_k] > 31 && u1_data[u_i + u_k] < 128)
					{
					    fprintf(fFile, "%c", u1_data[u_i+u_k]);
					}
					else
					{
					    fprintf(fFile, "%c", '.');
					}
				}
				else
				{
					fprintf(fFile, " ");
				}
			}

			fprintf(fFile, "\n");
		}

		// Should this output be flushed to the logfile immediately?
		if (bFlushOutput)
		{
			fflush(fFile);
		}
	}
}
