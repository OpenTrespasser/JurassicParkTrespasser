/***********************************************************************************************
 *
 * Copyright (c) 1997 DreamWorks Interactive
 *
 * Contents: Class for managing a symbol table used during parsing and code generation.
 *
 * Bugs:
 *
 * To do:
 *
 * 1. Integrate error tracking to the uRead and uWrite classes.
 *
 * 2. Integrate the CFileIO functionality for message and stream IO logging.
 *
 * 3. Add support for CGuiInterface.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/SymbolTable.hpp                                             $
 * 
 * 4     8/16/98 8:37p Mmouni
 * Fixed problem with returning a reference to a temporary.
 * 
 * 3     11/24/97 7:11p Agrant
 * Now reporting any valuetable symbols that are not expected by the loader.
 * This allows easier debugging of text props.  Unexpected symbols reported to debug window.
 * 
 * 2     5/13/97 1:17p Rwyatt
 * Renamed CSymbolTable to CNewsymbolTable to prevent clashes with the class of the same name
 * defined in SymTab.cpp/hpp
 * 
 * 1     5/12/97 9:58p Gstull
 * Classes for symbol table management,
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_GROFF_SYMBOLTABLE_HPP
#define HEADER_LIB_GROFF_SYMBOLTABLE_HPP

#include <iostream>
#include <iomanip>

//
// Disable a number of annoying warning messages about symbol truncation, and unsigned
// comparisons.
//

#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4786)

#include <map>
#include <deque>

//
// Determine which set of standard types to use based upon the environment.  This is done to
// simplify the use of these classes within both the game and 3D Studio Max.
//

#ifdef USE_MAX_TYPES

#include "StandardTypes.hpp"
#include "EasyString.hpp"
#include "ObjectHandle.hpp"

#else

#include "Common.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "Lib/Groff/ObjectHandle.hpp"

#endif


//**********************************************************************************************
//
class CSymbolEntry
//
// Prefix: se
//
{
	CEasyString	estrSymbolName;
	CHandle		hSymbolHandle;

public:

	//******************************************************************************************
	//
	CSymbolEntry
	(
	);
	

	//******************************************************************************************
	//
	CSymbolEntry
	(
		const CEasyString&	estr_symbol_name,
		const CHandle&		h_handle
	);


	CSymbolEntry& operator=
	(
		const CSymbolEntry& se_symbol
	);
	

	//**********************************************************************************************
	//
	friend std::ostream& operator<<
	(
		std::ostream&		os_stream,
		const CSymbolEntry&	se_symbol
	);

	
	//**********************************************************************************************
	//
	friend std::istream& operator>>
	(
		std::istream& is_stream,
		CSymbolEntry& se_symbol
	);


	//******************************************************************************************
	//
	CEasyString& estrSymbol
	(
	);

	
	//******************************************************************************************
	//
	CHandle& hHandle
	(
	);

	
	//******************************************************************************************
	//
	uint uCount
	(
	);

	
	//******************************************************************************************
	//
	CSymbolEntry& operator+=
	(
	   uint u_reference_count
	);


	//******************************************************************************************
	//
	CSymbolEntry& operator-=
	(
		uint u_reference_count
	);


	//******************************************************************************************
	//
	CSymbolEntry operator++
	(
	   int
	);


	//******************************************************************************************
	//
	CSymbolEntry operator--
	(
		int
	);


	//******************************************************************************************
	//
	bool operator==
	(
		const CEasyString& estr_symbol		// Character string containing the symbol.
	);


	//******************************************************************************************
	//
	bool bDelete
	(
		const CEasyString& estr_symbol		// Easystring containing the symbol name.
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


//**********************************************************************************************
//
// template <class Insert_Class, class Lookup_Class, class Element_Class> class CNewSymbolTable
class CNewSymbolTable
//
// Prefix: st
//
{
	friend CSymbolEntry;

	CHandleManager											hmgrManager;
	CObjectHandle											objhHandle;
	CEasyString												estrTableName;

	// Fast associative access to the symbol entry information.
	std::map< CEasyString, CSymbolEntry*, std::less<CEasyString> >	asiSymbolIndex;

	// Fast associative access to the element through this type. 
	std::map< CHandle, CSymbolEntry*, std::less<CHandle> >			asiHandleIndex;

public:   // Need access to verify validity of loaded value table.
	// Storage container for the element type.
	std::deque<CSymbolEntry>										aseSymbolTable;

public:

	//******************************************************************************************
	//
	CNewSymbolTable
	(
	);


	//******************************************************************************************
	//
	CNewSymbolTable
	(
		const CEasyString& estr_tablename
	);


	//******************************************************************************************
	//
	~CNewSymbolTable
	(
	);


	//******************************************************************************************
	//
	const CHandle& operator[]
	(
		const CEasyString& estr_symbol
	);
	

	//******************************************************************************************
	//

	const CEasyString& operator[]
	(
		CHandle h_handle
	);


	//******************************************************************************************
	//
	CHandle hSymbol
	(
		const CEasyString& estr_symbol
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

#endif