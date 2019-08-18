/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Archive: /JP2_PC/Source/Lib/File/Image.hpp $
 * $Date: 3/13/97 3:13p $
 * $Revision: 3 $
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_FILE_FILE_IMAGE_HPP
#define HEADER_LIB_FILE_FILE_IMAGE_HPP

// Opaque types from Windows.
typedef void* HANDLE;

#include "Spec.hpp"

#include <string>
#include <vector>
#include <assert.h>

struct SSectionInfo;
class CFileImage;
class CSymbolTable;
class CSectionDB;
class CSection;

//*********************************************************************************************
class CSymbolTable
// Prefix: st
// Describes a symbol table.  A possible optimization is that we can overload operator new() since we 
//  will be constructing many small objects (symbols).  The symbol table itself is a private class for
//  CFileImage to manipulate.  Users only see SSymbol.
//**************************************
{
public:
	struct SSymbol
		// The symbol iteself is a handle (symbol) and it's memory reference.  There is also an
		//  associated word for storing data with this symbol.
	{
		SSymbol();
		~SSymbol();
		SSymbol(const SSymbol& symbol);
		SSymbol& operator=(const SSymbol& symbol);
		TSymbolHandle get_handle();
		void set_memref(const SMemRef& memref);
		SMemRef get_memref();
		void set_word(unsigned int dword);
		unsigned int get_word();
#ifdef _DEBUG
		void dump();
#endif
	private:
		friend CSymbolTable;
		SSymbol(const SSymbolEntry& sym_entry, const char* sym_name);
		SSymbol(const char* sym_name);

		// The symbol is a ...
		TSymbolHandle handle;		// Symbol handle and ...
		char* name;					//  optional string rep of the symbol.
		// It references a ...
		SMemRef memref;				// Memory reference or ...
									//  nothing if 0 (unresolved or outside reference).
		unsigned int word;			// Associated word with this symbol.

		static TSymbolHandle unused_handle;	// Next handle available, this gets incremented every time a symbol is created.
	};

private:
	friend CFileImage;
	CSymbolTable();
	CSymbolTable(SSymbolEntry symbols[], int number_of_symbols, bool contains_names);
	~CSymbolTable();

	SSymbol* create_symbol(const char* name = 0);
	SSymbol* get_symbol(const TSymbolHandle symbol_handle);	// We can retrieve a symbol by it's handle ...
	SSymbol* get_symbol(const char* symbol_name);			//  or it's name.
	unsigned int number_of_symbols();

	unsigned int write_out(HANDLE hfile);					// Write out symbol table to file.

	std::vector<SSymbol*, std::allocator<SSymbol*> > table;	// The table itself is just a list of SSymbols.
#ifdef _DEBUG
	void dump();
#endif
};

typedef CSymbolTable::SSymbol TSymbol;

//*********************************************************************************************
class CFileImage
// Prefix: se
// Describes a groff section.  For reading, all we need is the file image and then we retrieve by
//  the symbol.  For writing, we create sections and symbols through the file image and create
//  areas through the section (see CSection and CArea).
//**************************************
{
public:
	// Open existing or new file image, if path_name exists than we assume writing.
	CFileImage(const char* path_name);
	~CFileImage();

	// Reading.  We can read in two modes, resolve all symbols recursively reading all data,
	//  or resolve only current symbols leaving all relocations as symbols for later
	//  retrieval.
	TSymbol* get_symbol(const TSymbolHandle symbol_handle);
	TSymbol* get_symbol(const char* symbol_name);
	void* get_data(TSymbol* symbol, bool resolve_to_symbols = true);

	// Writing.  Take a file image, create sections and symbols.
	CSection* create_section();
	TSymbol* create_symbol(const char* name = 0);
	void write_out();						// Write this file out to disk.

#ifdef _DEBUG
	void dump();
#endif
private:
	friend CSection;
	HANDLE hfile;
	SFileHeader header;
	CSymbolTable* symbol_table;				// Symbol table for this file.
	CSectionDB* section_db;					// Section database for this file; section table is contained here.

	CFileImage(const CFileImage&);
	CFileImage& operator=(const CFileImage&);

	bool writing;							// Are we writing?
};

//*********************************************************************************************
// Inlines

// CFileImage

inline TSymbol* CFileImage::get_symbol(const TSymbolHandle symbol_handle)
{ return symbol_table->get_symbol(symbol_handle); }

inline TSymbol* CFileImage::get_symbol(const char* symbol_name)
{ return symbol_table->get_symbol(symbol_name); }

// CSymbolTable

inline CSymbolTable::CSymbolTable()
{}

inline CSymbolTable::~CSymbolTable()
{}

inline unsigned int CSymbolTable::number_of_symbols()
{ return table.size(); }

// CSymbolTable::SSymbol

inline CSymbolTable::SSymbol::SSymbol()
	: memref(0), handle(0), name(0), word(0)
{}

inline TSymbolHandle CSymbolTable::SSymbol::get_handle()
{ return handle; }

inline void CSymbolTable::SSymbol::set_memref(const SMemRef& ref)
{ memref = ref; }

inline SMemRef CSymbolTable::SSymbol::get_memref()
{ return memref; }

inline void CSymbolTable::SSymbol::set_word(unsigned int w)
{ word = w; }

inline unsigned int CSymbolTable::SSymbol::get_word()
{ return word; }



#endif
