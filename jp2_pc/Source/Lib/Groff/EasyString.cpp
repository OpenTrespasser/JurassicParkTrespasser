/***********************************************************************************************
 *
 * Copyright (c) 1997 DreamWorks Interactive, 1997
 *
 * Contents: Implementation of class CEasyString.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/EasyString.cpp                                              $
 * 
 * 7     3/20/98 11:10a Mlange
 * Commented-out those annoying and superfluous messages that were output at compile time.
 * 
 * 6     2/09/98 4:20p Agrant
 * assert --> Assert in GUIApp version
 * 
 * 5     2/09/98 3:57p Gstull
 * Added fixes to prevent memory leaks.
 * 
 * 4     10/22/97 1:34a Gstull
 * Changes to support version 12 of the GROFF file structure definitions.  This mainly includes
 * support for text attributes for use with the attribute compiler.
 * 
 * 3     9/15/97 1:58p Gstull
 * Added support for a resize operator which allows the user to predefine the size of an easy
 * string's data buffer.
 * 
 * 2     5/15/97 7:07p Agrant
 * Made strData() a const function
 * 
 * 1     5/12/97 9:57p Gstull
 * Classes for convenient string management,
 *
 **********************************************************************************************/

// Define a constant which represents this sourcefile.
#ifndef __IMPLEMENTATION_LIB_GROFF_EASYSTRING_CPP__
#define __IMPLEMENTATION_LIB_GROFF_EASYSTRING_CPP__
#endif

// Standard includes.
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

//**********************************************************************************************
// Determine which set of standard types to use based upon the environment.  This is done to
// simplify the use of these classes within both the game and 3D Studio Max.
//

#ifdef USE_MAX_TYPES

// Standard includes.
#include "StandardTypes.hpp"

#else

// Standard includes.
#include "Common.hpp"
#define assert Assert

#endif

#include "Lib/Groff/EasyString.hpp"


//**********************************************************************************************
// Compile time selectable options.
//
//#pragma message("\nCOMPILE-TIME Options...")

//**********************************************************************************************
// Allow the user to specify the default size of the string buffer at runtime.
//
#ifndef __ESTR_INPUT_BUFFER_SIZE__
#define __ESTR_INPUT_BUFFER_SIZE__	  32

//#pragma message(" 1. String storage buffers will be allocated with a buffer size of (32) bytes.")
//#pragma message("    Define the symbol __ESTR_INPUT_BUFFER_SIZE__ to override default size.\n")

//#else

//#pragma message(" 1. Dynamic storage buffers will be allocated with a user specified buffer size.")

#endif

#define __ESTR_GUARD_BUFFER_SIZE__		6		// "Guard"  Length = 5 + Null

#ifndef __ESTR_PRINTF_BUFFER_SIZE__
#define __ESTR_PRINTF_BUFFER_SIZE__		256		// Printf buffer size.

//#pragma message(" 2. Printf local temporary buffer allocated with 256 characters. Define the symbol")
//#pragma message("    __ESTR_PRINTF_BUFFER_SIZE__ with the new size to override default size.\n")

//#else

//#pragma message(" 2. Dynamic storage buffers will be allocated with a user specified buffer size.")

#endif

//**********************************************************************************************
// End of compile time selectable options.
//
//#pragma message("COMPILE-TIME Options - END\n")

//**********************************************************************************************
//
inline void CEasyString::SetupStorage
(
	const char* str_string1,
	const char* str_string2
)
{
	void* heap_storage;

	//
	// Start out by determining how much memory the user requires.
	//

	// Get the length of the first string.  Is this string valid?
	if (str_string1 != 0)
	{
		// Yes!  Get it's length.
		uStringLength = ::strlen(str_string1);
	}
	else
	{
		// No!  Set the length to 0.
		uStringLength = 0;
	}

	// Were we passed a second string?
	if (str_string2 != 0)
	{
		// We were passed 2 strings.
		uStringLength += ::strlen(str_string2);
	}

	//
	// Determine what kind of memory was previously used.
	//

	// Was the previous storage allocated off the heap?
	if (uBufferSize > __ESTR_STORAGE_BUFFER_SIZE__)
	{
		// Yes! Then save a pointer to it for later deletion.
		heap_storage = strString;
	}
	else
	{
		// No!  Then set the heap pointer to NULL.
		heap_storage = 0;
	}

	// Will the user's string fit in the local storage?
	if (uStringLength < __ESTR_STORAGE_BUFFER_SIZE__)
	{
		// Yes! Then use the local memory for string storage.
		strString = strStorage;

		// Keep track of the size of the new string buffer.
		uBufferSize = __ESTR_STORAGE_BUFFER_SIZE__;
	}
	// Will the user's string fit into the existing storage?
	else if (uStringLength < uBufferSize)
	{
		// Yes!  Then eliminate do not reallocate a buffer.  Do not free the existing one.
		heap_storage = 0;
	}
	else
	// No!  We need to allocate a new buffer.  So get to it.
	{
		// Keep track of the size of the new string buffer.
		uBufferSize = uStringLength + 1;
		
		// No! Allocate a chunk of memory for the new string.
		strString = new char[uBufferSize];
	}

	// Was the string length 0?
	if (uStringLength != 0)
	{
		// Were we passed a valid first string?
		if (str_string1 != 0)
		{
			// Yes!  Copy it to the destination.
			::strcpy(strString, str_string1);
		}

		// Were we passed a second string?
		if (str_string2 != 0)
		{
			// Yes!  Add the second string onto the end of the buffer.
			::strcat(strString, str_string2);
		}
	}

	// Free any heap storage that might have been used.
	delete [] heap_storage;
}


//**********************************************************************************************
//
CEasyString::CEasyString
(
)
{
	// Initialize everything to safe default values.
	uBufferSize	  = __ESTR_STORAGE_BUFFER_SIZE__;
	uStringLength = 0;
	strString     = strStorage;
	strString[0]  = '\0';
}


//**********************************************************************************************
//
CEasyString::CEasyString
(
	uint u_length
)
{
	// Setup the string length and buffer size.
	uStringLength = 0;

	// Will the user's string fit in local storage?
	if (u_length < __ESTR_STORAGE_BUFFER_SIZE__)
	{
		// Yes! Then use the local memory for string storage.
		strString = strStorage;

		// Setup the buffersize to the default buffer size.
		uBufferSize = __ESTR_STORAGE_BUFFER_SIZE__;
	}
	else
	{
		// No! Allocate a chunk of memory for the new string.
		strString = new char[u_length];

		// Setup the buffersize to the default buffer size.
		uBufferSize = u_length;
	}

	// Terminate the string.
	strString[0] = '\0';
}


//**********************************************************************************************
//
CEasyString::CEasyString
(
	const char* str_string
)
{
	// Initialize the size variables.
	uBufferSize   = __ESTR_STORAGE_BUFFER_SIZE__;
	uStringLength = 0;

	// Initialize the storage stuff to the correct default values.
	strString	 = strStorage;
	strString[0] = '\0';

	// Setup the storage for this string.
	SetupStorage(str_string, 0);
}


//**********************************************************************************************
//
CEasyString::CEasyString
(
	const CEasyString& estr_string
)
{
	// Initialize the size variables.
	uBufferSize   = __ESTR_STORAGE_BUFFER_SIZE__;
	uStringLength = 0;

	// Initialize the storage stuff to the correct default values.
	strString	 = strStorage;
	strString[0] = '\0';

	// Setup the storage for this string.
	SetupStorage(estr_string.strString, 0);
}


//**********************************************************************************************
//
CEasyString::~CEasyString
(
)
{
	// Was heap storage used for this string?
	if (uBufferSize > __ESTR_STORAGE_BUFFER_SIZE__)
	{
		// Yes! Then delete it.
		delete [] strString;
	}
}	


//**********************************************************************************************
//
CEasyString& CEasyString::operator=
(
	const char* str_string
)
{
	// Setup the storage for this object.
	SetupStorage(str_string, 0);

	// Return this object.
	return *this;
}


//**********************************************************************************************
//
CEasyString& CEasyString::operator=
(
	const CEasyString& estr_string
)
{
	// Setup the storage for this string.
	SetupStorage(estr_string.strString, 0);

	// Return this object.
	return *this;
}


//**********************************************************************************************
//
CEasyString operator+
(
	const char*		   str_string, 
	const CEasyString& estr_string
)
{
	CEasyString estr_result;

	estr_result.SetupStorage(str_string, estr_string.strString);

	// Return the copy of the new string. 
	return estr_result;
}


//**********************************************************************************************
//
CEasyString operator+
(
	const CEasyString& estr_string, 
	const char* str_string
)
{
	CEasyString estr_result;

	estr_result.SetupStorage(estr_string.strString, str_string);

	// Return the copy of the new string. 
	return estr_result;
}


//**********************************************************************************************
//
CEasyString operator+
(
	const CEasyString& estr_string1,
	const CEasyString& estr_string2
)
{
	CEasyString estr_result;

	// Attempt to setup the storage for the strings.
	estr_result.SetupStorage(estr_string1.strString, estr_string2.strString);

	// Return the copy of the new string. 
	return estr_result;
}


//**********************************************************************************************
//
CEasyString& CEasyString::operator+=
(
	const char* str_string
)
{
	// Make sure there is plenty of memory for this string.
	SetupStorage(strString, str_string);

	// Return the new object.
	return *this;
}


//**********************************************************************************************
//
CEasyString& CEasyString::operator+=
(
	const CEasyString& estr_string
)
{
	// Make sure there is plenty of memory for this string.
	SetupStorage(strString, estr_string.strString);

	// Return the new object.
	return *this;
}


//**********************************************************************************************
//
char CEasyString::operator[]
(
	uint u_index
) const
{
	// Is this character within the length of the string buffer?
	if (u_index > uBufferSize)
	{
		// No!  Return a null character.
		return 0;
	}
	else
	{
		// Yes!  Return the character.
		return strString[u_index];
	}
}


//**********************************************************************************************
//
char CEasyString::operator[]
(
	int i_index
) const
{
	// Is this character within the length of the string buffer?
	if (i_index < 0 || i_index > (int) uStringLength)
	{
		// No!  Return a null character.
		return 0;
	}
	else
	{
		// Yes!  Return the character.
		return strString[i_index];
	}
}


//**********************************************************************************************
//
bool CEasyString::operator==
(
	const CEasyString& estr_string
) const
{
	return (uStringLength == estr_string.uStringLength) && ::strcmp(strString, estr_string.strString) == 0;
}


//**********************************************************************************************
//
bool CEasyString::operator!=
(
	const CEasyString& estr_string
) const
{
	return uStringLength != estr_string.uStringLength && ::strcmp(strString, estr_string.strString) != 0;
}


//**********************************************************************************************
//
bool CEasyString::operator<
(
	const CEasyString& estr_string
) const
{
	return ::strcmp(strString, estr_string.strString) < 0;
}


//**********************************************************************************************
//
bool CEasyString::operator>
(
	const CEasyString& estr_string
) const
{
	return ::strcmp(strString, estr_string.strString) > 0;
}


//**********************************************************************************************
//
bool CEasyString::operator<=
(
	const CEasyString& estr_string
) const
{
	return ::strcmp(strString, estr_string.strString) <= 0;
}


//**********************************************************************************************
//
bool CEasyString::operator>=
(
	const CEasyString& estr_string
) const
{
	return ::strcmp(strString, estr_string.strString) >= 0;
}


//**********************************************************************************************
//
bool CEasyString::operator==
(
	const char* str_string
) const
{
	return ::strcmp(strString, str_string) == 0;
}


//**********************************************************************************************
//
bool CEasyString::operator!=
(
	const char* str_string
) const
{
	return ::strcmp(strString, str_string) != 0;
}


//**********************************************************************************************
//
bool CEasyString::operator<
(
	const char* str_string
) const
{
	return ::strcmp(strString, str_string) < 0;
}


//**********************************************************************************************
//
bool CEasyString::operator>
(
	const char* str_string
) const
{
	return ::strcmp(strString, str_string) > 0;
}


//**********************************************************************************************
//
bool CEasyString::operator<=
(
	const char* str_string
) const
{
	return ::strcmp(strString, str_string) <= 0;
}


//**********************************************************************************************
//
bool CEasyString::operator>=
(
	const char* str_string
) const
{
	return ::strcmp(strString, str_string) >= 0;
}


//**********************************************************************************************
//
// General purpose routines for reading and writing to and from the strings.
//

//**********************************************************************************************
//
void CEasyString::Printf
(
	const char* str_format, ...
)
{
	char    str_temp[__ESTR_PRINTF_BUFFER_SIZE__ + __ESTR_GUARD_BUFFER_SIZE__];
	va_list	va_parameters;

	// Setup the printf guard buffer.
	::strcpy(&str_temp[__ESTR_PRINTF_BUFFER_SIZE__], "Guard");

	// Setup the arguement list pointer to the first free argument.
	va_start(va_parameters, str_format);

	// Format the message to the string buffer and return the string length.
	uint u_str_length = vsprintf(str_temp, str_format, va_parameters);

	// Configure the storage for the string.
	SetupStorage(str_temp, 0);

	// Verify that we did not overrun the end of the buffer.
	if (strcmp(&str_temp[__ESTR_PRINTF_BUFFER_SIZE__], "Guard"))
	{
		// The user has overwritten the end of the printf buffer so assert here.
		assert(0);

		// Die here!
		_asm
		{
			int	3
		}
	}

	// Reset the argument list pointer to null.
	va_end(va_parameters);
}


//**********************************************************************************************
//
void CEasyString::Puts
(
	const char* str_string
)
{
	// Configure the storage for the string.
	SetupStorage(str_string, 0);
}


//**********************************************************************************************
//
std::ostream& operator<<
(
	std::ostream&			os_stream,
	const CEasyString&	estr_string
)
{
	// Display the string.
	return os_stream << estr_string.strString;
}


//**********************************************************************************************
//
std::istream& operator>>
(
	std::istream&	 is_stream,
	CEasyString& estr_string
)
{
	char strBuffer[__ESTR_INPUT_BUFFER_SIZE__];

	// Read in the string.
	std::cin.getline(strBuffer, __ESTR_INPUT_BUFFER_SIZE__, '\n');

	// Assign the string to the string.
	estr_string = strBuffer;

	// Pass the input stream along.
	return is_stream;
}


//**********************************************************************************************
// General purpose support functions for various useful string operations.
//

//**********************************************************************************************
//
uint CEasyString::uLength
(
) const 
{
	// Return the number of characters in the string minus the null character.
	return uStringLength; 
}


//**********************************************************************************************
//
void CEasyString::Resize
(
	uint u_length
)
{
	// Setup the string length and buffer size.
	uStringLength = 0;

	// Will the user's string fit in local storage?
	if (uStringLength < __ESTR_STORAGE_BUFFER_SIZE__)
	{
		// Yes! Then use the local memory for string storage.
		strString = strStorage;

		// Setup the buffersize to the default buffer size.
		uBufferSize = __ESTR_STORAGE_BUFFER_SIZE__;
	}
	else
	{
		// Did the user previously have heap memory allocated?
		if (uBufferSize > __ESTR_STORAGE_BUFFER_SIZE__)
		{
			// Yes!  Then delete it.
			delete [] strString;
		}

		// No! Allocate a chunk of memory for the new string.
		strString = new char[u_length];

		// Setup the buffersize to the default buffer size.
		uBufferSize = u_length;
	}

	// Terminate the string.
	strString[0] = '\0';
}


//**********************************************************************************************
//
void CEasyString::ToLower
(
)
{
	// Loop through the string and convert each character to lower case.
	for (uint u_i = 0; u_i < uStringLength; u_i++)
	{
		// Is this character upper case?
		if (strString[u_i] >= 'A' && strString[u_i] <= 'Z')
		{
			// Yes!  Then convert it to lower case.
			strString[u_i] += 0x20;
		}
	}
}


//**********************************************************************************************
//
void CEasyString::ToUpper
(
)
{
	// Loop through the string and convert each character to upper case.
	for (uint u_i = 0; u_i < uStringLength; u_i++)
	{
		// is this character lower case?
		if (strString[u_i] >= 'a' && strString[u_i] <= 'z')
		{
			// Yes!  Then convert it to upper case.
			strString[u_i] -= 0x20;
		}
	}
}


//**********************************************************************************************
//
uint CEasyString::uMatchChar
(
	char c_char
)
{
	uint u_count = 0;

	// Loop through the string.
	for (uint u_i = 0; u_i < uStringLength; u_i++)
	{
		// Is this is the character we are looking for?
		if (strString[u_i] == c_char)
		{
			// Yes!  Then increment the counter.
			u_count++;
		}
	}

	// Return the count.
	return u_count;
}


//**********************************************************************************************
//
bool CEasyString::bFindFirst
(
	char  c_char,			// The character that we are looking for.
	uint& u_index,			// The location in the array of the nth occurance of the character.
	uint  u_occurance		// The number of occurances of the character to match.
)
{
	// Loop through all the characters, starting at index 0.
	for (u_index = 0; u_index < uStringLength; u_index++)
	{
		// Did we find the character we were looking for?
		if (strString[u_index] == c_char)
		{
			// Decrement the occurance counter.
			u_occurance--;

			// Are we at the proper number of occurances?
			if (!u_occurance)
			{
				// Yes!  We have found N occurances of the character so return true.
				return true;
			}

			// No!  Keep looking for additional occurances of the character.
		}
	}

	// The specificed character does not appear the appropriate number of times in this string.
	return false;
}


//**********************************************************************************************
//
bool CEasyString::bFindLast
(
	char c_char,			// The character that we are looking for. 
	uint& u_index,			// The location in the array of the nth occurance of the character.
	uint  u_occurance		// The number of occurances of the character to match.
)
{
	// Begin looking at the very last character
	u_index = uStringLength;
	do
	{
		// Did we find the character we were looking for?
		if (strString[u_index] == c_char)
		{
			// Decrement the occurance counter.  Are we at the proper number of occurances?
			if (--u_occurance == 0)
			{
				// Yes!  We have found N occurances of the character so return true.
				return true;
			}

			// No!  Keep looking for additional occurances of the character.
		}
	}
	while (u_index--);

	// The specificed character does not appear in the string.
	return false;
}


//**********************************************************************************************
//
const char* CEasyString::strData
(
	const uint u_index
) const
{
	// Is the requested address valid?
	if (u_index < uBufferSize)
	{
		// Yes!  Then return a pointer to that location in the string.
		return &strString[u_index];
	}
	else
	{
		// No!  Then return an invalid pointer.
		return (const char *) 0;
	}
}


//******************************************************************************************
//
CEasyString CEasyString::estrExtractString
(
	char c_start_char,			// The starting character of the substring we wish to match.
	uint u_start_occurance,		// The number of occurances of the start character we are looking for.
	char c_end_char,			// The terminating character of the substring we wish to match.
	uint u_end_occurance		// The number of occurances of the termination charcter we are looking for.
)
{
	uint u_start_index;

	// Attempt to find the first occurance of the character.  Were we successful?
	if (!bFindFirst(c_start_char, u_start_index, u_start_occurance))
	{
		// No!  Return an error.
		return estrNULL_STRING;
	}
	
	// Yes!  Now determine the line length.
	const char *str_buffer = strData(u_start_index+1);

	// Loop through the string and find the Nth occurance of the ending character.  Keep track
	// of the character count.
	for (uint u_index = 0; str_buffer[u_index]; u_index++)
	{
		// Is this the character we are looking for?
		if (str_buffer[u_index])
		{
			// Yes!  Then decrement the end occurance counter.
			--u_end_occurance;

			// Is it 0?
			if (!u_end_occurance)
			{
				//
				// Yes!  Then we have found the end of the string.  Construct a CEasyString and
				// return it to the caller.
				//
				CEasyString estr_tmp(u_index+1);

				char* str_tmp = (char *) estr_tmp.strData();

				// Copy the contents into the new string.
				::strncpy(str_tmp, strString, u_index);

				// Terminate the string.
				str_tmp[u_index] = '\0';

				// Return the string to the caller.
				return estr_tmp;
			}
		}
	}


	// We hit the end of the string so return the null string.
	return estrNULL_STRING;
}


//******************************************************************************************
//
uint CEasyString::uWriteCount
(
)
{
	// Return the number of bytes required to represent the string in memory.
	return uStringLength + 1;
}


//******************************************************************************************
//
uint CEasyString::uWrite
(
	char** ppc_buffer
)
{
	// Write the string into a memory buffer and increment the pointer.
	memcpy(*ppc_buffer, strString, uStringLength + 1);
	*ppc_buffer += uStringLength + 1;

	// Return the number of bytes written.
	return uStringLength + 1;
}


//******************************************************************************************
//
uint CEasyString::uRead
(
	char** ppc_buffer
)
{
	// Set the string length to 0 since we don't know what it is.
	uStringLength = 0;

	// Setup the storage for this string and increment the pointer.
	SetupStorage(*ppc_buffer);
	*ppc_buffer += uStringLength + 1;

	// Return the number of characters read.
	return uStringLength + 1;
}


//******************************************************************************************
//
void CEasyString::Dump
(
)
{
	// Display the string information in a dump format.
	std::cout << "CEasyString :" << strString << ":, Length = " << uStringLength << std::endl;
}
