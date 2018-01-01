/****************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:   Extended string manipulations. Module contains functionality not found
 *             within <stdlib.h>.
 *
 * Notes:      
 *
 * To do:      Add a CopyString function that copies a string while converting from
 *             upper to lower case.
 *
 *****************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/StringEx.cpp                                            $
 * 
 * 10    98/04/10 23:59 Speter
 * Removed requirement that file extensions be 3 characters (this ain't DOS).
 * 
 * 9     97/10/01 4:41p Pkeet
 * Added the 'strFindLastSlash' function.
 * 
 * 8     97-05-09 13:46 Speter
 * Added strTypeName() function.
 * 
 * 7     96/12/09 16:02 Speter
 * Added CStrBuffer class.
 * 
 * 6     11/22/96 7:27p Pkeet
 * Added the implementation for the 'CBeginsWith' class.
 * 
 * 5     11/17/96 2:50p Pkeet
 * Added the 'strDuplicate' function.
 * 
 * 4     11/05/96 8:52p Pkeet
 * Added the 'bMatchExtension' and 'ReplaceExtension' functions.
 * 
 * 3     96/06/04 13:22 Speter
 * Removed unnecessary includes.
 * 
 * 2     4/18/96 11:52a Pkeet
 * Putting into common format.
 * 
 * 1     4/17/96 10:01a Pkeet
 * Extended string handling routines.
 *
 *****************************************************************************************/

//
// Macros and #includes.
//

#include "Common.hpp"
#include "StringEx.hpp"

#include <string.h>
#include <stdio.h>
#include <typeinfo.h>

//
// Constants.
//

// Constant for converting characters from upper to lower case.
const char cUpperToLower = 'A' - 'a';


//
// Functions.
//

//*********************************************************************************************
bool bMatchExtension(const char* str_name, const char* str_ext)
{
	//
	// Check that an extension of the right length exists.
	//
	while (*str_name != 0 && *str_name != '.')
		str_name++;

	if (*str_name != '.')
		return false;

	str_name++;

	return stricmp(str_name, str_ext) == 0;
}

//*********************************************************************************************
void ReplaceExtension(char* str_name, const char* str_ext)
{
	//
	// Check that an extension of the right length exists.
	//
	while (*str_name != 0 && *str_name != '.')
		str_name++;

	if (*str_name != '.')
		return;

	str_name++;

	Assert(strlen(str_name) == 3);

	if (*str_ext == '.')
		str_ext++;

	// Replace the last three characters.
	*str_name = *str_ext;
	str_name++;
	str_ext++;
	*str_name = *str_ext;
	str_name++;
	str_ext++;
	*str_name = *str_ext;
}

//***************************************************************************************
//
void AppendString
(
	char * pc_dest_string,	// Destination character string.
	const char * pc_source_string,
							// Source string to be appended to the destination string.
	uint32 u4_dest_string_size,
							// The maximum number of characters that may be in the
							// destination string.
	bool b_make_lowercase	// Will convert the source characters to lowercase before
							// appending them to the destination string if true. No
							// effect if this is false.
)
//
// Appends a source string to a destination string while checking and preventing
// overflow. If a string overflow is detected, AppendString will only append as many
// characters as can fit in the destination string.
//
//**************************
{
	uint32 u4_dest_string_end;
	uint32 u4_source_string_index;

	// Debug asserts.
	Assert(pc_dest_string != 0);
	Assert(pc_source_string != 0);
	Assert(u4_dest_string_size != 0);

	//
	// Find null-terminating byte of the destination string.
	//
	u4_dest_string_end = 0;
	while ((pc_dest_string[u4_dest_string_end] != 0) && (u4_dest_string_end < u4_dest_string_size))
		u4_dest_string_end++;
	if (u4_dest_string_end >= u4_dest_string_size)
		return;

	//
	// Append the source string to the destination string without writing out of
	// the destination string's bounds.
	//
	u4_source_string_index = 0;
	while((pc_source_string[u4_source_string_index] != 0) && (u4_dest_string_end < u4_dest_string_size - 1))
	{
		// Check if lower case conversion is required.
		if (b_make_lowercase)
		{
			char c_temp = pc_source_string[u4_source_string_index];
			if ((c_temp >= 'A') && (c_temp <= 'Z'))
				c_temp -= cUpperToLower;
			pc_dest_string[u4_dest_string_end] = c_temp;
		}
		else
		{
			// Copy without lower case conversion.
			pc_dest_string[u4_dest_string_end] = pc_source_string[u4_source_string_index];
		}
		u4_dest_string_end++;
		u4_source_string_index++;
	}

	// Append a null-terminating character to the end of the destination string.
	pc_dest_string[u4_dest_string_end] = 0;
}


//*********************************************************************************************
char* strDuplicate(const char* pc_source_string, bool b_make_lowercase)
{
	Assert(pc_source_string);

	uint  u_strlen = uStrLen(pc_source_string);	// Size of the string to duplicate.
	char* str_dest = new char[u_strlen + 1];	// Memory for the new string.

	Assert(str_dest);

	// Copy the string.
	for (uint u_pos = 0; u_pos < u_strlen; u_pos++)
	{
		// Get the character.
		char c = pc_source_string[u_pos];

		// Convert the character to lower case if required.
		if (b_make_lowercase)
		{
			if ((c >= 'A') && (c <= 'Z'))
				c -= cUpperToLower;
		}

		// Copy the character to the destination string.
		str_dest[u_pos] = c;
	}

	// Add the null termination character.
	str_dest[u_strlen] = 0;

	return str_dest;
}

//*****************************************************************************************
const char* strFindLastSlash(const char* str)
{
	Assert(str);

	// If there are no characters, just return a pointer to the string.
	int i = strlen(str);
	if (i <= 0)
		return str;

	// Move backwards through the string until a slash is found.
	while (i > 0)
	{
		if (str[i - 1] == '\\' || str[i - 1] == '/')
			return str + i;
		--i;
	}

	// If the first character is a slash, return a pointer one past the first character.
	if (*str == '\\' || *str == '/')
		return str + 1;

	// If no slashes found, return a pointer to the string.
	return str;
}


//
// Class implementations.
//

//*****************************************************************************************
//
// CBeginsWith implementation.
//

	//*****************************************************************************************
	//
	// CBeginsWith constructor.
	//

	//*****************************************************************************************
	CBeginsWith::CBeginsWith(const char* str_beginning)
		: strBeginning(str_beginning)
	{
		Assert(strBeginning);

		// Remove leading blanks.
		while (*strBeginning == ' ')
			strBeginning++;
	}

	//*****************************************************************************************
	//
	// CBeginsWith operator.
	//

	//*****************************************************************************************
	bool CBeginsWith::operator ==(const char* str) const
	{
		Assert(str);

		uint u_char = 0;

		// Remove leading blanks.
		while (*str == ' ')
			str++;

		// Iterate through the entire begins with string.
		while (str[u_char] != 0)
		{
			// If we have reached the end of the comparison string, the comparison has failed.
			if (strBeginning[u_char] == 0)
				return false;

			// If the characters don't match, the comparison fails.
			if (cLower(strBeginning[u_char]) != cLower(str[u_char]))
				return false;

			// Increment to the next character.
			u_char++;
		}

		// The iteration made it through the string, therefore the comparision succeeded.
		return true;
	}

//**********************************************************************************************
//
// CStrBuffer implementation.
//

	//******************************************************************************************
	CStrBuffer::CStrBuffer(uint u_max)
		: CMAArray<char>(u_max)
	{
		atArray[0] = 0;
	}

	//******************************************************************************************
	void CStrBuffer::Print(const char *str_format, ...)
	{
		Assert(str_format);

		va_list vlist;					// Variable arguments list.

		//
		// Write output string using variable number of arguments.
		//
		va_start(vlist, str_format);
		uLen += vsprintf(atArray + uLen, str_format, vlist);
		Assert(uLen < uMax);
		va_end(vlist);
	}

//******************************************************************************************
const char* strTypeInfoName(const type_info& type)
{
	const char* str_name = type.name();

	// The name returned has a superfluous "struct " or "class " prefix.
	// We'll blow that away right here.
	if (strlen(str_name) > 5 && str_name[5] == ' ')
		str_name += 6;
	return str_name;
}

