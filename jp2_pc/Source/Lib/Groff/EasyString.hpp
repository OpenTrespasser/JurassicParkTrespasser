/***********************************************************************************************
 *
 * Copyright (c) 1997 DreamWorks Interactive, 1997
 *
 * Contents: String class which provides generic support for a variety of string operations 
 * whick make dealing with filenames, directory paths, etc. a lot less arduous, and much faster.
 *
 * Bugs:
 *
 * To do:
 *
 * 1. Tune the static buffer size and profile the code.
 *
 * 2. Modify the stream IO stuff to use CFileIO for logging messages to the files.
 *
 * 3. Integrate the CGuiInterface class into this code.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/EasyString.hpp                                              $
 * 
 * 6     2/09/98 3:57p Gstull
 * Added fixes to prevent memory leaks.
 * 
 * 5     10/22/97 1:34a Gstull
 * Changes to support version 12 of the GROFF file structure definitions.  This mainly includes
 * support for text attributes for use with the attribute compiler.
 * 
 * 4     9/15/97 1:58p Gstull
 * Added support for a resize operator which allows the user to predefine the size of an easy
 * string's data buffer.
 * 
 * 3     5/26/97 1:38p Agrant
 * LINT minor syntax error  fixed.
 * 
 * 2     5/15/97 7:07p Agrant
 * Made strData() a const function
 * 
 * 1     5/12/97 9:57p Gstull
 * Classes for convenient string management,
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_GROFF_EASYSTRING_HPP
#define HEADER_LIB_GROFF_EASYSTRING_HPP

#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


//**********************************************************************************************
// Determine which default types to use.
//
#ifdef USE_MAX_TYPES

#include "StandardTypes.hpp"

#else

#include "Common.hpp"

#endif


//**********************************************************************************************
// Setup all easy strings to have a default size of 16 bytes.  The user can trivially override
// this at compile time when the optimal storage buffer sizes have been determined.
// 
#ifndef __ESTR_STORAGE_BUFFER_SIZE__
#define __ESTR_STORAGE_BUFFER_SIZE__	16		// This means 15 characters plus the NULL.
#endif


//**********************************************************************************************
//
class CEasyString
//
// Prefix: estr
//
{
	uint  uStringLength;								// The actual string length without null.
	uint  uBufferSize;									// The actual size of the current buffer.
	char  strStorage[__ESTR_STORAGE_BUFFER_SIZE__];		// Local buffer for small strings.
	char* strString;									// Pointer to the buffer starting address.

	
	//******************************************************************************************
	//
	inline void SetupStorage
	(
		const char* str_string1,
		const char* str_string2 = 0
	);


public:
	
	//******************************************************************************************
	//
	CEasyString
	(
	);


	//******************************************************************************************
	//
	CEasyString
	(
		uint u_string_length
	);


	//******************************************************************************************
	//
	CEasyString
	(
		const char* str_string
	);


	//******************************************************************************************
	//
	CEasyString
	(
		const CEasyString& estr_string
	);


	//******************************************************************************************
	//
	~CEasyString
	(
	);


	//**********************************************************************************************
	//
	CEasyString& operator=
	(
		const CEasyString& estr_string
	);


	//**********************************************************************************************
	//
	CEasyString& operator=
	(
		const char* str_string
	);


	//**********************************************************************************************
	//
	friend CEasyString operator+
	(
		const char* str_string, 
		const CEasyString& estr_string
	);


	//**********************************************************************************************
	//
	friend CEasyString operator+
	(
		const CEasyString& estr_string, 
		const char* str_string
	);


	//**********************************************************************************************
	//
	friend CEasyString operator+
	(
		const CEasyString& estr_string1,
		const CEasyString& estr_string2
	);


	//**********************************************************************************************
	//
	CEasyString& operator+=
	(
		const char* str_string
	);


	//**********************************************************************************************
	//
	CEasyString& operator+=
	(
		const CEasyString& estr_string
	);


	//**********************************************************************************************
	//
	char operator[]
	(
		uint u_index
	) const;


	//**********************************************************************************************
	//
	char operator[]
	(
		int i_index
	) const;


	//**********************************************************************************************
	//
	bool operator==
	(
		const CEasyString& estr_string
	) const;


	//**********************************************************************************************
	//
	bool operator!=
	(
		const CEasyString& estr_string
	) const;


	//**********************************************************************************************
	//
	bool operator<
	(
		const CEasyString& estr_string
	) const;


	//**********************************************************************************************
	//
	bool operator>
	(
		const CEasyString& estr_string
	) const;


	//**********************************************************************************************
	//
	bool operator<=
	(
		const CEasyString& estr_string
	) const;


	//**********************************************************************************************
	//
	bool operator>=
	(
		const CEasyString& estr_string
	) const;


	//**********************************************************************************************
	//
	bool operator==
	(
		const char* str_string
	) const;


	//**********************************************************************************************
	//
	bool operator!=
	(
		const char* str_string
	) const;


	//**********************************************************************************************
	//
	bool operator<
	(
		const char* str_string
	) const;


	//**********************************************************************************************
	//
	bool operator>
	(
		const char* str_string
	) const;


	//**********************************************************************************************
	//
	bool operator<=
	(
		const char* str_string
	) const;


	//**********************************************************************************************
	//
	bool operator>=
	(
		const char* str_string
	) const;


	//**********************************************************************************************
	//
	void Printf
	(
		const char* str_format, ...
	);


	//**********************************************************************************************
	//
	void Puts
	(
		const char* str_string
	);


	//**********************************************************************************************
	//
	friend std::ostream& operator<<
	(
		std::ostream&		os_stream,
		const CEasyString&	estr_string
	);

	
	//**********************************************************************************************
	//
	friend std::istream& operator>>
	(
		std::istream&	 is_stream,
		CEasyString& estr_string
	);


	//**********************************************************************************************
	//
	// General purpose support functions for various useful string operations.
	//
	//**********************************************************************************************


	//**********************************************************************************************
	//
	uint uLength
	(
	) const;
	

	//**********************************************************************************************
	//
	void Resize
	(
		uint u_length
	);


	//******************************************************************************************
	//
	void ToLower
	(
	);


	//******************************************************************************************
	//
	void ToUpper
	(
	);


	//******************************************************************************************
	//
	uint uMatchChar
	(
		char c_char
	);


	//******************************************************************************************
	//
	bool bFindFirst
	(
		char  c_char,				// The character we wish to match.
		uint& u_index,				// The location of the Nth occurance of the character.
		uint  u_occurances = 1		// The number of occurances of the character we are looking for.
	);


	//******************************************************************************************
	//
	bool bFindLast
	(
		char  c_char,				// The character we wish to match.
		uint& u_index,				// The location of the Nth occurance of the character.
		uint  u_occurances = 1		// The number of occurances of the character we are looking for.
	);


	//******************************************************************************************
	//
	const char* strData
	(
		const uint u_index = 0		// The offset into the string.
	) const;


	//******************************************************************************************
	//
	CEasyString estrExtractString
	(
		char c_start_char,			// The starting character of the substring we wish to match.
		uint u_start_occurance,		// The number of occurances of the start character we are looking for.
		char c_end_char,			// The terminating character of the substring we wish to match.
		uint u_end_occurance = 1	// The number of occurances of the termination charcter we are looking for.
	);


	//******************************************************************************************
	//
	uint uWriteCount
	(
	);


	//******************************************************************************************
	//
	uint uWrite
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	uint uRead
	(
		char** ppc_buffer
	);


	//******************************************************************************************
	//
	void Dump
	(
	);
};

// Define a generic NULL string.
static const CEasyString estrNULL_STRING("");

#endif