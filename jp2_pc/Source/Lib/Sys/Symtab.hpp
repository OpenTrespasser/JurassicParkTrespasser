/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1996
 *
 * Contents:
 *		This file contains methods for management of a symbol table.
 * 
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/Sys/Symtab.hpp                                                   $
 * 
 * 3     6/18/97 7:35p Gstull
 * Added changes to support fast exporting.
 *
 *********************************************************************************************/

#ifndef HEADER_LIB_COMMON_SYMTAB_HPP
#define HEADER_COMMON_SYMTAB_HPP

#ifdef USE_MAX_TYPES
#include "StandardTypes.hpp"
#else
#include "common.hpp"
#endif

#include "Lib/Sys/SysLog.hpp"

// Prefix: sh
typedef uint TSymbolHandle;

// Prefix: se
struct SSymbolRecord
//
 {
	uint			uNameLength;						// Includes terminating null char.
	char*			strName;							// Pointer to the string.
	TSymbolHandle	shHandle;							// Handle to the symbol entry.
};

struct SSymtab
{
	SSymbolRecord*	pseSymbol;
	SSymtab*		pstNext;
};


//*********************************************************************************************
//
class CSymTab
//
// Prefix: st
//
// This class is intended to be used to manage the symbol table associated which could exist
// with each GROFF file.
//
// Example:
//
//*********************************************************************************************
{
private:
	uint			uSymtabImageSize;						// Size of the internal table image.
	void*			SymtabImage;							// Internal representation of the symbol table.

	SSymtab*		pstHead;								// Pointer to the front of the list.

	CSysLog			slLogfile;								// Keep a logfile for debugging purposes.

	bool			bBuildSymtab(
						void*	symtab, 
						uint	u_size
					);										// Member function to construct a symbol table.

	SSymtab*		NewNode(
						char*		str_name,				// The actual symbol string.
						SSymtab*	pst_next				// The pointer to the next node in the list.
					);

public:
					CSymTab();
					CSymTab(
						void*	symtab,						// Void pointer to symbol table memory image.
						uint	u_size						// Size in bytes of symbol table memory image. 
					);

					~CSymTab();						// Symbol table destructor.

	void			Initialize();							// Place the symbol table into it's initialize the state.
	uint			uCount();								// Return the symbol count.

	TSymbolHandle	shInsert(char* str_name);				// Insert a symbol in the list
	
	bool			bDelete(char* str_name);				// Remove a symbol from the table
	bool			bDelete(TSymbolHandle sh_handle);		// Remove a symbol from the table

	const char*		strLookup(TSymbolHandle sh_handle);		// Lookup a symbol in the table
	TSymbolHandle	shLookup(char* str_name);				// Lookup a symbol in the table

	bool			bData(void** symtab, uint* u_size);		// Return a pointer to the internal rep and it's size.
		
	const char*		operator[](uint u_index);				// Return a constant string based upon an array index.

	void			Dump();									// Dump the symbol table to a logfile.
};

#endif