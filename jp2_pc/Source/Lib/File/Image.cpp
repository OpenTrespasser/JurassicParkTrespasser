/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Todo:
 *
 ***********************************************************************************************
 *
 * $Archive: /JP2_PC/Source/Lib/File/Image.cpp $
 * $Date: 2/03/98 2:25p $
 * $Revision: 4 $
 * 
 **********************************************************************************************/

#include "Spec.hpp"
#include "Image.hpp"
#include "Section.hpp"

#include <windows.h>

#pragma warning(disable: 4237)	// Windows.h turns this back on.

#include <algorithm>

#ifdef _DEBUG
#include <iostream>
#endif

//*********************************************************************************************
//
// CFileImage

CFileImage::CFileImage(const char* path_name)
{
	unsigned long lbytes;

	// Open for reading or writing.
	writing = false;
	hfile = CreateFile(path_name, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE)
	{
		// Failed, open for writing.
		hfile = CreateFile(path_name, GENERIC_WRITE, 0, NULL,
			CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		assert(hfile != INVALID_HANDLE_VALUE);
		writing = true;
	}

	// Initialize file header.
	if (writing)
	{
		// Create new file header.
		memset(&header, 0, sizeof(SFileHeader));
		header.MagicNumber = MAGIC_NUMBER;
		header.VersionNumber = VERSION_ID;
		header.Flags = 0;
	}
	else
	{
		// Read the file header.
		ReadFile(hfile, &header, sizeof(SFileHeader), &lbytes, NULL);
		assert(lbytes == sizeof(SFileHeader));
		assert(header.VersionNumber == VERSION_ID);	// No backward compatability!
	}

	// Initialize the section database.
	if (writing)
	{
		section_db = new CSectionDB;
	}
	else
	{
		// Read the section table.
		SetFilePointer(hfile, header.SectTabOffset, NULL, FILE_BEGIN);
		lbytes = header.SectionCount * sizeof(SSectionHeader);
		char* sections = new char[lbytes];
		ReadFile(hfile, sections, lbytes, &lbytes, NULL);
		assert(lbytes == header.SectionCount * sizeof(SSectionHeader));
		
		section_db = new CSectionDB((SSectionHeader*)sections, header.SectionCount);

		delete[] sections;
	}

	// Initialize the symbol table.
	if (writing)
	{
		symbol_table = new CSymbolTable();
	}
	else
	{
		// Read the symbol table.
		SetFilePointer(hfile, header.SymTabOffset, NULL, FILE_BEGIN);

		char* symbols = new char[header.SymTabSize];
		ReadFile(hfile, symbols, header.SymTabSize, &lbytes, NULL);
		assert(lbytes == header.SymTabSize);
		
		symbol_table = new CSymbolTable((SSymbolEntry*)symbols, header.SymTabEntryCount, header.Flags | FILE_HASNAMES);

		delete[] symbols;
	}

}

CFileImage::~CFileImage()
{
	if (writing && hfile)
	{
		write_out();
	}
	else
		CloseHandle(hfile);
	
	delete symbol_table;
	delete section_db;
}

inline void* CFileImage::get_data(TSymbol* symbol, bool resolve_to_symbols /* = true */)
{
	assert(!writing);
	assert(symbol);
	return section_db->get_data(this, symbol->get_memref(), resolve_to_symbols);
}

inline CSection* CFileImage::create_section()
{
	assert(writing);
	return section_db->create_section();
}

inline TSymbol* CFileImage::create_symbol(const char* name /* = 0 */)
{
	assert(writing);
	return symbol_table->create_symbol(name);
}

void CFileImage::write_out()
{
	assert(writing && hfile);

	unsigned int size = sizeof(SFileHeader);
	unsigned long lbytes;

	// Go to beginning of file and reserve space for file header.
	SetFilePointer(hfile, sizeof(SFileHeader), NULL, FILE_BEGIN);

	// Section headers follow file header.
	header.SectTabOffset = sizeof(SFileHeader);
	// Write out section headers and raw section data.
	size += section_db->write_out(hfile);
	header.SectionCount = section_db->number_of_sections();
	
	header.SymTabOffset = size;
	// Write out symbol table.
	SetFilePointer(hfile, header.SymTabOffset, NULL, FILE_BEGIN);
	header.SymTabSize = symbol_table->write_out(hfile);
	header.SymTabEntryCount = symbol_table->number_of_symbols();
	size += header.SymTabSize;
	// TODO: assume has names for now
	if (1)
		header.Flags |= FILE_HASNAMES;

	// Write out file header.
	header.SizeOfFile = size;
	header.TimeStamp = 0;
	header.Flags = 0;
	header.Reserved[0] = 0;
	header.Reserved[1] = 0;
	header.CRC = 0;

	SetFilePointer(hfile, 0, NULL, FILE_BEGIN);
	WriteFile(hfile, &header, sizeof(SFileHeader), &lbytes, NULL);
	assert(lbytes == sizeof(SFileHeader));

	CloseHandle(hfile);
	hfile = 0;
}

#ifdef _DEBUG
void CFileImage::dump()
{
	cout << "CFileImage: \n";
	cout << " <writing>" << writing << '\n';
	cout << " <hfile>" << hfile << '\n';
	cout << " <Magic Number>" << header.MagicNumber << '\n';
	cout << " <Version Number>" << header.VersionNumber << '\n';
	cout << " <Size of File>" << header.SizeOfFile << '\n';
	cout << " <Section Table Offset>" << header.SectTabOffset << '\n';
	cout << " <Section Count>" << header.SectionCount << '\n';
	cout << " <Symbol Table Offset>" << header.SymTabOffset << '\n';
	cout << " <Symbol Table Entry Count>" << header.SymTabEntryCount << '\n';
	cout << " <Symbol Table Size>" << header.SymTabSize << '\n';
	cout << " <Time Stamp>" << header.TimeStamp << '\n';
	cout << " <Flags>0x";
	cout.setf(ios::hex, ios::basefield);
	cout << header.Flags << '\n';
	cout.setf(ios::dec, ios::basefield);
	
	symbol_table->dump();
	section_db->dump();
}
#endif

//*********************************************************************************************
//
// CSymbolTable

CSymbolTable::CSymbolTable(SSymbolEntry symbols[], int number_of_symbols, bool contains_names)
{
	char* p = (char*)symbols;
	SSymbolEntry* entry;
	
	if (contains_names)
	{
		for (; number_of_symbols > 0; --number_of_symbols)
		{
			entry = (SSymbolEntry*)p;
			p += sizeof(SSymbolEntry);
			table.push_back(new SSymbol(*entry, p));
			p += strlen(p) + 1;
		}
	}
	else
	{
		for (; number_of_symbols > 0; --number_of_symbols)
		{
			entry = (SSymbolEntry*)p;
			p += sizeof(SSymbolEntry);
			table.push_back(new SSymbol(*entry, 0));
		}
	}
}

TSymbol* CSymbolTable::create_symbol(const char* name)
{
	TSymbol* symbol = new SSymbol(name);
	table.push_back(symbol);

	assert(symbol->handle);
	return symbol;
}

TSymbol* CSymbolTable::get_symbol(const TSymbolHandle symbol_handle)
{
	vector<SSymbol*, allocator<SSymbol*> >::iterator i;
	for (i = table.begin(); i != table.end(); ++i)
		if ((*i)->handle == symbol_handle)
			break;

	if (i == table.end())
		return 0;
	else
		return *i;
}

TSymbol* CSymbolTable::get_symbol(const char* symbol_name)
{
	vector<SSymbol*, allocator<SSymbol*> >::iterator i;
	for (i = table.begin(); i != table.end(); ++i)
		if (!strcmp((*i)->name, symbol_name))
			break;

	if (i == table.end())
		return 0;
	else
		return *i;
}

unsigned int CSymbolTable::write_out(HANDLE hfile)
{
	unsigned long lbytes;
	unsigned int size = 0;
	SSymbolEntry entry;

	vector<SSymbol*, allocator<SSymbol*> >::iterator i;

	for (i = table.begin(); i != table.end(); ++i)
	{
		entry.handle = (*i)->handle;
		entry.memref = (*i)->memref;
		entry.word = (*i)->word;

		WriteFile(hfile, &entry, sizeof(SSymbolEntry), &lbytes, NULL);
		assert(lbytes == sizeof(SSymbolEntry));
		size += (unsigned int)lbytes;

		if ((*i)->name)
		{
			WriteFile(hfile, (*i)->name, strlen((*i)->name)+1, &lbytes, NULL);	// Write out '\0' also.
			assert(lbytes == strlen((*i)->name)+1);
			size += (unsigned int)lbytes;
		}
	}
	return size;
}

#ifdef _DEBUG
void CSymbolTable::dump()
{
	cout << "Symbol Table:\n";

	vector<SSymbol*, allocator<SSymbol*> >::iterator i;
	for (i = table.begin(); i != table.end(); ++i)
	{
		(*i)->dump();
	}
}
#endif

//*********************************************************************************************
//
// CSymbolTable::SSymbol

TSymbolHandle CSymbolTable::SSymbol::unused_handle = 1;

CSymbolTable::SSymbol::SSymbol(const SSymbol& symbol)
{
	handle = symbol.handle;
	if (symbol.name)
	{
		name = new char[strlen(symbol.name)+1];
		strcpy(name, symbol.name);
	}
	else
		name = 0;
	memref = symbol.memref;
	word = symbol.word;
}

CSymbolTable::SSymbol::SSymbol(const SSymbolEntry& sym_entry, const char* sym_name)
{
	handle = sym_entry.handle;
	unused_handle = max(unused_handle, handle)+1;
	
	if (sym_name && sym_name[0])
	{
		name = new char[strlen(sym_name)+1];
		strcpy(name, sym_name);
	}
	else
		name = 0;

	memref = sym_entry.memref;

	word = sym_entry.word;
}

CSymbolTable::SSymbol::SSymbol(const char* sym_name)
{
	handle = unused_handle++;
	
	if (sym_name)
	{
		name = new char[strlen(sym_name)+1];
		strcpy(name, sym_name);
	}
	else
		name = 0;

	memref = 0;	// Reference unresolved.
	word = 0;
}

CSymbolTable::SSymbol::~SSymbol()
{
	if (name)
		delete[] name;
}

TSymbol& CSymbolTable::SSymbol::operator=(const TSymbol& symbol)
{
	if (this != &symbol)
	{
		handle = symbol.handle;
		if (symbol.name)
		{
			name = new char[strlen(symbol.name)+1];
			strcpy(name, symbol.name);
		}
		else
			name = 0;
		memref = symbol.memref;
		word = symbol.word;
	}
	return *this;
}

#ifdef _DEBUG
void CSymbolTable::SSymbol::dump()
{
	cout << "Symbol: " << "<handle>" << handle;
	cout << " <name>" << name;
	cout << " <memref>(" << memref.handle << ", " << memref.offset << ")";
	cout << " <word>:" << word << '\n';
}
#endif
