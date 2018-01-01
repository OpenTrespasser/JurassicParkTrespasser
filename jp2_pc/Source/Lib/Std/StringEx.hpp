/**********************************************************************************************
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
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Std/StringEx.hpp                                                 $
 * 
 * 12    8/26/98 5:33p Asouth
 * "Fixed" a warning
 * 
 * 11    8/25/98 11:43a Rvande
 * Added std:: qualifier for MW build (with MSL)
 * 
 * 10    97/10/01 4:40p Pkeet
 * Added the 'strFindLastSlash' function.
 * 
 * 9     97-05-09 13:45 Speter
 * Added strTypeName() function.
 * 
 * 8     97/03/17 14:29 Speter
 * Changed AppendString to default to no case conversion (as that's the only way it was ever
 * used).
 * 
 * 7     96/12/09 16:02 Speter
 * Added CStrBuffer class.
 * 
 * 6     11/22/96 7:27p Pkeet
 * Added the definition for the 'CBeginsWith' class and the 'cLower' inline function.
 * 
 * 5     11/17/96 3:10p Pkeet
 * Added the 'uStrLen' function.
 * 
 * 4     11/17/96 2:50p Pkeet
 * Added the 'strDuplicate' function.
 * 
 * 3     11/05/96 8:52p Pkeet
 * Added the 'bMatchExtension' and 'ReplaceExtension' functions.
 * 
 * 2     4/18/96 11:52a Pkeet
 * Putting into common format.
 * 
 * 1     4/17/96 10:01a Pkeet
 * Header for extended string handling routines.
 *
 *********************************************************************************************/

#ifndef HEADER_LIB_STRINGEX_HPP
#define HEADER_LIB_STRINGEX_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
class CBeginsWith
//
// Class for determining if a string begins with a certain patten.
//
// Prefix: begin
//
//**************************************
{
protected:

	const char* strBeginning;	// Target string.

public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CBeginsWith
	(
		const char* str_beginning	// Target string.
	);

	//*****************************************************************************************
	//
	// Member operator.
	//

	//*****************************************************************************************
	//
	bool operator ==
	(
		const char* str
	) const;
	//
	// Returns 'true' if the string parameter matches the beginning of the target string.
	//
	//**************************************

};


//
// Global inline functions.
//

//**********************************************************************************************
//
inline uint uStrLen
(
	const char* str	// Null-terminated string.
)
//
// Returns the number of characters in the string.
//
// Notes:
//		This function is designed to replace the "strlen" function.
//
//**************************************
{
	Assert(str);

	uint u_count = 0;	// Count for the number of characters.

	// Count characters.
	#ifdef __MWERKS__
	 // i hate this warning
	 while (str[u_count])
	 	u_count++;
	 u_count++;
	#else
	 while (str[u_count++]);
	#endif

	return u_count;
}

//*********************************************************************************************
//
inline char cLower
(
	char c
)
//
// Returns the character converted to lower case.
//
//**************************************
{
	if (c >= 'A' && c <= 'Z')
		c -= (char)('A' - 'a');
	return c;
}


//
// Template and inline functions, and function prototypes.
//

//*********************************************************************************************
//
void AppendString
(
	char*       pc_dest_string,
	const char* pc_source_string,
	uint32      u4_dest_string_size,
    bool        b_make_lowercase = false
);
//
// Adds a string to the end of the first string.
//
//**************************************

//*********************************************************************************************
//
bool bMatchExtension
(
	const char* str_name,
	const char* str_ext
);
//
// Returns 'true' if the extension string matches the file extension, otherwise returns
// 'false.'
//
//**************************************

//*********************************************************************************************
//
void ReplaceExtension
(
	char* str_name,
	const char* str_ext
);
//
// Replaces the three character extension.
//
//**************************************

//*********************************************************************************************
//
char* strDuplicate
(
	const char* pc_source_string,
    bool        b_make_lowercase = false
);
//
// Returns a duplicate of the source string. Will convert all the characters on the duplicate
// string to lower case if the lower case flag is 'true.'
//
//**************************************

//*********************************************************************************************
//
const char* strFindLastSlash
(
	const char* str
);
//
// Returns a pointer to the position in the string ahead of the last slash found.
//
//**************************************

//*********************************************************************************************
//
class CStrBuffer: public CMAArray<char>
//
//**************************************
{
public:
	CStrBuffer(uint u_max);

	void Print
	(
		const char *str_format, 
		...
	);
};

#ifdef __MWERKS__
namespace std { class type_info ; }
#else
class type_info;
#endif

//******************************************************************************************
//
const char* strTypeInfoName
(
	const type_info& type				// A type_info object, returned by typeid().
);
//
// Returns:
//		The C++ type name of the object, with useless prefixes removed.
//
//**************************************

//******************************************************************************************
//
template<class T> const char* strTypeName
(
	const T& t							// Any object that has runtime type information;
										// that is, any object with virtual functions.
)
//
// Returns:
//		The C++ type name of the object, accessed via runtime type information.
//
//**************************************
{
	return strTypeInfoName(typeid(t));
}

#endif
