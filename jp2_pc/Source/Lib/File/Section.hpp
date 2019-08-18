/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	A section is a logical structure that holds related data.  Areas within sections are
 *	 named (with symbols) and can be referenced by symbols.  Practically, related data that
 *	 reference each other (with pointers) should be put in the same section.  This is
 *	 because a section is loaded into memory all at once.  If pointers cross section
 *	 boundries, than each section must be loaded separately.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Archive: /JP2_PC/Source/Lib/File/Section.hpp $
 * $Date: 8/25/98 4:41p $
 * $Revision: 4 $
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_FILE_FILE_SECTION_HPP
#define HEADER_LIB_FILE_FILE_SECTION_HPP

#include "Spec.hpp"
#include "Image.hpp"
#include "Section.hpp"

#ifndef __MWERKS__
 #include <minmax.h>
#endif

#include <assert.h>
#include <vector>

class CSection;
class CArea;
class CSectionDB;

const int DEF_AREA_SIZE = (1024*16);

//*********************************************************************************************
class CSectionDB
// Prefix: sd
// Describes all sections.  This is a private class used by CFileImage that reads and writes
//  CSection types to file images.
//**************************************
{
public:
	friend CFileImage;

	// Create a new section database.
	CSectionDB();
	// Create a section database with sections[].
	CSectionDB(const SSectionHeader sections[], int number_of_sections);
	~CSectionDB();

	// Get data at memref.  See CFileImage get_data() for full explanation.
	void* get_data(CFileImage* file_image, const SMemRef& memref, bool resolve_to_symbols);

	// Create a new section.
	CSection* create_section();
	
	// Write out section headers and sections.
	unsigned int write_out(HANDLE hfile);

	unsigned int number_of_sections();

	// The database consists of section info structures.
	std::vector<CSection*, std::allocator<CSection*> > db;

#ifdef _DEBUG
	void dump();
#endif
};

//*********************************************************************************************
class CSection
// Prefix: se
// Sections are created and managed by CSectionDB (which is a private class for CFileImage).
//  Writing data and pointers to sections is done through CArea, hence there is only one
//  function in the public interface, to create an area.
{
public:
	// Reserve an area for writing, we can give a size but memory allocation is automatic.
	CArea* reserve_area(int size = DEF_AREA_SIZE);

private:
	friend CSectionDB;
	struct SSectionInfo
	// Describes the internal data footprint in memory of a section.
	{
		SSectionInfo();
		SSectionInfo(const SSectionHeader& sh);
		// Is this memory reference in this section?
		bool operator==(const SMemRef& memref);

		SSectionHeader header;			// Copy of disk section_header (on disk).
		void* image_addr;				// Address where section image starts in memory.
		SRelocationEntry* relocations;	// Pointer to relocation table for this section.
		bool resolve_to_symbols;		// See CFileImage.
#ifdef _DEBUG
		void dump();
#endif
	};
	
	CSection(const SSectionHeader& sect_hdr);
	// Create a new (empty) section in the file image.
	CSection();

	CSection(const CSection&);
	CSection& operator=(const CSection&);
	~CSection();

	// Is this memref in this section?
	bool operator==(const SMemRef& memref);

	SSectionInfo section_info;							// Data in memory.
	std::vector<CArea*, std::allocator<CArea*> > areas;	// All areas in this section.

	// Write out section data.
	unsigned int write_out(HANDLE hfile, TSectionHandle handle);
	
	// See CFileImage.
	void update_section(CFileImage* file_image, bool resolve_to_symbols);
	friend void* CSectionDB::get_data(CFileImage*, const SMemRef&, bool);
	void* get_data(CFileImage* file_image, const TRelAddr offset, bool resolve_to_symbols);
#ifdef _DEBUG
	void dump();
#endif
};

//*********************************************************************************************
class CArea
// Prefix: ar
// Describes areas within sections.  An area is one data type (e.g. texture, PRS, or mesh)
//  and can be complex (contain pointers to other areas).  Each area can have a symbol
//  associated with it for later retrieval.  You can either write data or references to
//  areas.  Areas are purely for writing data.  The abstraction for an area is not valid
//  for reading - what once were areas is just a void pointer to some data whose relocations
//  have been resolved.
{
public:
	void assoc_symbol(TSymbol* symbol);			// Associate a symbol for this section area.
	int write_data(const void* data, int size);	// Write data to this area.
	int write_reference(CArea* area);			// Write reference to another area within this section.
	int write_reference(TSymbol* symbol);		// Write reference to a symbol.
private:
	friend class CSection;
	CArea(const CSection& section, int size);
	~CArea();

	CArea(const CArea&);
	CArea& operator=(const CArea&);

	struct SAreaRelocation
	/*
	 * Area relocations are area based relocations for this area.  We must use AB
	 *  relocations because the area has not been fixed in the section nor has the
	 *  section been fixed in the file, thus FB or SB offsets are not yet determined.
	 *  The fixups can either be another area (within the same section of this area)
	 *  or a symbol that must be fixed by the section database later.
	 */
	{
		SAreaRelocation();
		SAreaRelocation(CArea* pa);
		SAreaRelocation(TSymbol* sym);
		~SAreaRelocation();

		TRelAddr AB_fixup;					// AB offset of this fixup.
		// Just as in SRelocationEntry, fixups are either a section offset or a symbol handle.
		CArea* area;						// Area that the fixup should point to or ...
		TSymbol* symbol;					//  symbol of the fixup.
#ifdef _DEBUG
		void dump();
#endif
	};

	struct SAreaMemory
	/*
	 * Area memory is the internal memory structure of the area.  It will grow
	 *  automatically and the initial_size on construction is just a hint.
	 */
	{
		char* pstart;
		int off_free;
		int off_end;

		SAreaMemory(int initial_size);
		~SAreaMemory();
		char* reserve(int size);
		void write(const void* pbuf, int size);
#ifdef _DEBUG
		void dump();
#endif
	};

	SAreaMemory memory;		// Memory for storing data.
	TRelAddr SR_offset;		// Offset in section this area is located at.
	TSymbol* symbol;		// Symbol name for this area, this is optional since all areas aren't named.

	// List of all the relocations for this area.
	std::vector<SAreaRelocation, std::allocator<SAreaRelocation> > AB_relocations;
#ifdef _DEBUG
	void dump();
#endif
};

//*********************************************************************************************
// Inlines

// CSectionDB

inline CSectionDB::CSectionDB()
{}

inline unsigned int CSectionDB::number_of_sections()
{ return db.size(); }

// CSection

inline CSection::CSection()
{}

inline CSection::CSection(const SSectionHeader& sh)
	: section_info(sh)
{}

inline bool CSection::operator==(const SMemRef& memref)
{ return section_info == memref; }

// CArea

inline CArea::CArea(const CSection& section, int size)
	: memory(size), SR_offset(INVALID), symbol(0)
{}

inline CArea::~CArea()
{}

inline void CArea::assoc_symbol(TSymbol* sym)
{ symbol = sym; }

// CArea::SAreaRelocation

inline CArea::SAreaRelocation::SAreaRelocation()
	: AB_fixup(INVALID), area(0), symbol(0)
{}

inline CArea::SAreaRelocation::SAreaRelocation(CArea* pa)
	: AB_fixup(INVALID), area(pa), symbol(0)
{}

inline CArea::SAreaRelocation::SAreaRelocation(TSymbol* sym)
	: AB_fixup(INVALID), area(0), symbol(sym)
{}

inline CArea::SAreaRelocation::~SAreaRelocation()
{}

#endif
