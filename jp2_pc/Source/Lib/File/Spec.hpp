/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	Groff file specification.
 *
 *	A groff file looks like this:
 *
 *               ___________________
 *              |                   |	Groff File Header
 *               ___________________
 *           ---|                   |	Section Table (array of
 *          |    -------------------	 Section Headers)
 *         -+---|        ...        |
 *        | |    ___________________
 *        |  -->|                   |
 *        |      -------------------	Raw Section Data
 *         ---->|        ...        |
 *               ___________________
 *              |                   |	Symbol Table
 *               ___________________
 *
 *	Individual Sections are explained below in their respective declarations.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Archive: /JP2_PC/Source/Lib/File/Spec.hpp $
 * $Date: 11/20/96 5:38p $
 * $Revision: 2 $
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_FILE_FILE_SPEC_HPP
#define HEADER_LIB_FILE_FILE_SPEC_HPP

typedef unsigned int TSectionHandle;
typedef unsigned int TSymbolHandle;

#define INVALID (~(unsigned int)0)

typedef unsigned int TRelAddr;

struct SMemRef
/*
 * A memory reference is a section handle and it's offset.  It uniquely specifies
 *  every location that can be written or read.
 */
{
	SMemRef() : handle(0), offset(0) {}
	SMemRef(const TSectionHandle sh, const TRelAddr ra) : handle(sh), offset(ra) {}
	SMemRef(const int i) : handle(i), offset(i) {}
	bool operator==(SMemRef memref) const { return handle == memref.handle && offset == memref.offset; }
	bool operator!=(SMemRef memref) const { return handle != memref.handle || offset != memref.offset; }

	TSectionHandle handle;
	TRelAddr offset;
};

/*
 * Groff File Header
 *	The main header is at the beginning of every groff file and defines what the rest
 *	of the file looks like.
 *
 * Possible extensions:
 * uint ImageBase
 *	When the linker creates a final database, it assumes that the file will be memory-
 *	mapped to a specific location in memory.  That location is stored in this field.
 *	What happens if we cannot load the database into that memory location?  Two options,
 *	one, fail, or two, have the loader patch the addresses.
 *
 */

// Groff file signature.
const unsigned int MAGIC_NUMBER = 'GROF';

// Version number to keep track of architectural changes for upward compatibility.
const unsigned int VERSION_ID = 1000;	// Version 1.000

struct SFileHeader
// File header structure.
{
	unsigned int MagicNumber;	// File signature.
	unsigned int VersionNumber;	// Version of file system this file has been written with.
	unsigned int SizeOfFile;	// Size in bytes of the file.
	
	TRelAddr SectTabOffset;		// File offset to section headers.
	unsigned int SectionCount;	// Number of sections in the file.
	
	TRelAddr SymTabOffset;		// File offset to symbol table.
	unsigned int SymTabEntryCount;	// Number of (variable sized) entries in the symbol table.
	unsigned int SymTabSize;	// Size in bytes of symbol table.
	
	unsigned int TimeStamp;		// The time this file was created.  See <time.h> for format information.
	unsigned int Flags;		// Flags that describe this file.

	unsigned int Reserved[2];

	unsigned int CRC;		// CRC of the header.
};

// Flags for the file header.
#define FILE_HASNAMES		0x00000001	// The symbol table contains names.
#define FILE_CRCENCODING	0x00000002	// This file has a CRC checksum.

/*
 * Section Header
 *	Between the groff file header and the section data is the Section Table, an array
 *	of Section Header's which define the sections.  A section is a defined area that
 *	is used to store object data.
 */

struct SSectionHeader
// Section header structure.
{
	TSectionHandle SectionHandle;	// Internal identifier for this section.
	unsigned int Flags;		// Flags that describe this section.
	unsigned int SectionOffset;	// File offset to beginning of raw section data.
	unsigned int SectionSize;	// Size of the section including padding and relocation table.
	unsigned int RelocationOffset;	// Offset within the section of relocation table.
	unsigned int RelocationCount;	// Number of relocations for the section.
};

// Flags for the section header.
#define SECT_HASREFERENCES	0x00000001	// This file contains references.

// Section areas and major parts of the file are aligned.
#define SECTION_DATA_ALIGNMENT 4
#define SECTION_AREA_ALIGN(x) (((x)+3)&0xFFFFFFFC)

struct SSymbolEntry
/*
 * Define the structure for symbol table entries.  An entry in it's simplest form is a
 *  memory reference and a symbol handle identifier.  But, strings are useful for human
 *  users and an entry on disk can also contain a string identifier.  If the string
 *  symbol bit (FILE_HASNAMES) is set for the file, than every symbol entry contains a
 *  string following the actual entry.  This string can be the null string.  The word is
 *  an associated word with this symbol that is under user control.  If the memref is 0
 *  than this symbol does not exist within this file; this is for a linker to resolve
 *  and thus the name identifier must be valid to find the proper address.
 */
{
	// The symbol is identified with a symbol handle and/or a character name below.
	TSymbolHandle handle;
	// It references a memory location or if memref == 0, it's an outside reference.
	SMemRef memref;
	// Associated word with this symbol.
	unsigned int word;
};
	// char name[1];	Optional name identifier.


struct SRelocationEntry
/*
 * A list of relocations follow each section.  Each relocation is a fixup address and
 *  the address or symbol that location must be fixed up with.  The reladdr denotes an
 *  offset within the same section as the relocation.  Since a reladdr field of 0 is
 *  valid, this field is valid if symbol_handle is 0.  Otherwise, this fixup refers to
 *  an address elsewhere, either in another section within this file, or a symbol outside
 *  the file.
 */
{
	TRelAddr fixup_offset;		// Offset w/in this section that must be fixed up.
	// We must fixup with ...
	TRelAddr reladdr;		// Offset w/in this section or ...
	TSymbolHandle symbol_handle;	// Symbol located elsewhere, w/in file (another section) or out of file.
};

#endif
