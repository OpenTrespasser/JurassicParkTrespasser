/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Todo:
 *
 ***********************************************************************************************
 *
 * $Archive: /JP2_PC/Source/Lib/File/Section.cpp $
 * $Date: 3/13/97 3:13p $
 * $Revision: 3 $
 * 
 **********************************************************************************************/

#include "Section.hpp"

#include <algorithm>

#include <windows.h>

#pragma warning(disable: 4237)	// Windows.h turns this back on.

#ifdef _DEBUG
#include <iostream>
#endif

//*********************************************************************************************
//
// CSectionDB

CSectionDB::~CSectionDB()
{
	std::vector<CSection*, std::allocator<CSection*> >::iterator i;

	for (i = db.begin(); i != db.end(); ++i)
	{
		delete *i;
	}
}

CSectionDB::CSectionDB(const SSectionHeader sections[], int number_of_sections)
{
	const SSectionHeader *header = sections;
	for (; number_of_sections > 0; --number_of_sections)
	{
		db.push_back(new CSection(*header));
		++header;
	}
}

void* CSectionDB::get_data(CFileImage* file_image, const SMemRef& memref, bool resolve_to_symbols)
{
	if (memref == 0)
		return 0;
	
	// Get section that memref refers to.
	std::vector<CSection*, std::allocator<CSection*> >::iterator i;
	for (i = db.begin(); i != db.end(); ++i)
		if (**i == memref)
			break;
	assert(i != db.end());
	
	return (*i)->get_data(file_image, memref.offset, resolve_to_symbols);
}

CSection* CSectionDB::create_section()
{
	CSection* section = new CSection();
	db.push_back(section);
	return section;
}

unsigned int CSectionDB::write_out(HANDLE hfile)
{
	assert(hfile);

	unsigned long lbytes;
	unsigned int bytes, align;
	unsigned int bytes_written = 0;

	TSectionHandle handle = 1;	// Unique section handles for this DB; note handle of 0 is invalid.

	TRelAddr FR_start = SetFilePointer(hfile, 0, NULL, FILE_CURRENT);
	
	std::vector<CSection*, std::allocator<CSection*> >::iterator i;
	
	// Write out sections first, since the sections fill in (partly) their own headers.
	// Seek to raw section start.
	bytes = db.size() * sizeof(SSectionHeader);
	bytes = SECTION_AREA_ALIGN(bytes);
	bytes_written += bytes;		// Will write later.
	SetFilePointer(hfile, bytes, NULL, FILE_CURRENT);
	// Write out sections.
	for (i = db.begin(); i != db.end(); ++i)
	{
		bytes = (*i)->write_out(hfile, handle);
		assert(bytes);
		// Fill in header information not written in CSection::write_out().
		assert((*i)->section_info.header.SectionSize);
		(*i)->section_info.header.SectionOffset = FR_start+bytes_written;
		(*i)->section_info.header.SectionHandle = handle++;
		(*i)->section_info.header.Flags = 0x0;		// TODO: fill this in.

		align = SECTION_AREA_ALIGN(bytes);
		if (align -= bytes)
			SetFilePointer(hfile, align, NULL, FILE_CURRENT);

		bytes_written += bytes+align;
	}
	bytes_written -= align;		// Don't count last alignment.
	
	// Write section headers.
	SetFilePointer(hfile, FR_start, NULL, FILE_BEGIN);
	for (i = db.begin(); i != db.end(); ++i)
	{
		WriteFile(hfile, &((*i)->section_info.header), sizeof(SSectionHeader), &lbytes, NULL);
		assert(lbytes == sizeof(SSectionHeader));
	}

	return bytes_written;
}

#ifdef _DEBUG
void CSectionDB::dump()
{
	std::cout << "Section Database:\n";
	std::cout << "<Sections>(list):\n";
	std::vector<CSection*, std::allocator<CSection*> >::iterator i;
	for (i = db.begin(); i != db.end(); ++i)
	{
		(*i)->dump();
	}
}
#endif


//*********************************************************************************************
//
// CSection

CSection::~CSection()
{
	// Delete section areas.
	std::vector<CArea*, std::allocator<CArea*> >::iterator i;
	for (i = areas.begin(); i != areas.end(); ++i)
	{
		delete (*i);
	}
}

CArea* CSection::reserve_area(int size)
{
	CArea* area = new CArea(*this, size);
	areas.push_back(area);
	return area;
}

void CSection::update_section(CFileImage* file_image, bool resolve_to_symbols)
{
	unsigned long lbytes;

	// Check if section image has been loaded to memory.
	if (section_info.image_addr &&
	    section_info.resolve_to_symbols == resolve_to_symbols)
		return;

	// Read section into memory.
	if (!section_info.image_addr)
	{
		int size = section_info.header.SectionSize;
		section_info.image_addr = new char[size];
		SetFilePointer(file_image->hfile, section_info.header.SectionOffset, NULL, FILE_BEGIN);
		ReadFile(file_image->hfile, section_info.image_addr, size, &lbytes, NULL);
		assert(lbytes == size);
		section_info.relocations = (SRelocationEntry*)((char*)section_info.image_addr + section_info.header.RelocationOffset);
	}

	// Fixup pointers.
	SRelocationEntry* relo = section_info.relocations;
	for (int i = section_info.header.RelocationCount; i > 0; --i)
 	{
		// image_start+offset is the pointer to the relative address.
		void** relo_addr = (void**)((char*)section_info.image_addr + relo->fixup_offset);

		if (resolve_to_symbols)
		{
			// Symbol must be defined for this relo.
			// TODO: is this correct behavior?  Or should we put 0 or the actual address here?
			assert(relo->symbol_handle);
			*relo_addr = (void*)relo->symbol_handle;
		}
		else
		{
			if (relo->reladdr != INVALID)
			{
				// Relocation is within this section.
				// Fix up the relative address with the actual address.
				*relo_addr = (void*)((char*)section_info.image_addr + relo->reladdr);
			}
			else
			{
				assert(relo->symbol_handle);
				// Recursively get data (and thus update_section) for this symbol.
				TSymbol* symbol = file_image->get_symbol(relo->symbol_handle);
				assert(symbol);
				void* data = file_image->get_data(symbol, resolve_to_symbols);
				if (data)
				{
					// Relocation was in this file.

					// Fix up the relative address with the actual address returned above.
					*relo_addr = data;
				}
				else
				{
					// Relocation is out of file.
					assert(0);	// We don't support real time linking across files - this symbol should
							//  have been fixed at link time.
				}
			}
		}

		++relo;
	}
}

void* CSection::get_data(CFileImage* file_image, const TRelAddr offset, bool resolve_to_symbols)
{
	update_section(file_image, resolve_to_symbols);
	return (char*)section_info.image_addr + offset;
}

unsigned int CSection::write_out(HANDLE hfile, TSectionHandle handle)
{
	assert(hfile);	// Must be valid handle.

	unsigned int bytes, align;
	unsigned long lbytes;

	// Clear header.
	section_info.header.SectionOffset = 0;
	section_info.header.SectionSize = 0;
	section_info.header.RelocationOffset = 0;
	section_info.header.RelocationCount = 0;
	
	// Relocation list.
	std::vector<SRelocationEntry, std::allocator<SRelocationEntry> > relos;

	std::vector<CArea*, std::allocator<CArea*> >::iterator i;

	// First we fill in offset in section where areas will begin and
	//  we fix up symbol since we now know the memref.
	TRelAddr section_offset = 0;	// SR offset of section area.
	for (i = areas.begin(); i != areas.end(); ++i)
	{
		(*i)->SR_offset = section_offset;
		if ((*i)->symbol)
			(*i)->symbol->set_memref(SMemRef(handle, (*i)->SR_offset));

		section_offset += (*i)->memory.off_free;
		section_offset = SECTION_AREA_ALIGN(section_offset);
	}
	
	// Now we write the areas and build the relocations.
	for (i = areas.begin(); i != areas.end(); ++i)
	{
		// Write section area data.
		WriteFile(hfile, (*i)->memory.pstart, (*i)->memory.off_free, &lbytes, NULL);
		bytes = (unsigned int)lbytes;
		assert(bytes == (*i)->memory.off_free);
		// Align for next write.
		align = SECTION_AREA_ALIGN(bytes);
		if (align -= bytes)
			SetFilePointer(hfile, align, NULL, FILE_CURRENT);

		section_info.header.SectionSize += bytes+align;
		
		// Build section area relocations list.
		std::vector<CArea::SAreaRelocation, std::allocator<CArea::SAreaRelocation> >::iterator j;
		for (j = (*i)->AB_relocations.begin(); j != (*i)->AB_relocations.end(); ++j)
		{
			SRelocationEntry relo_entry;
			assert((*j).AB_fixup != INVALID);
			relo_entry.fixup_offset = (*j).AB_fixup + (*i)->SR_offset;
			if ((*j).area)
			{
				// This relo is an inter-section area.
				relo_entry.reladdr = (*j).area->SR_offset;
				if ((*j).area->symbol)
					relo_entry.symbol_handle = (*j).area->symbol->get_handle();
				else
					relo_entry.symbol_handle = 0;
			}
			else
			{
				// This relo is a symbol.
				assert((*j).symbol);
				SMemRef memref = (*j).symbol->get_memref();
				if (memref.handle == handle)
				{
					// This symbol is within this section.
					relo_entry.reladdr = memref.offset;	// This was filled in above in first loop.
					if ((*j).area->symbol)
						relo_entry.symbol_handle = (*j).area->symbol->get_handle();
					else
						relo_entry.symbol_handle = 0;
				}
				else if (memref.handle)
				{
					// This symbol is another section within the file.
					
					// The relocation entry cannot be filled in since the
					//  section might have not been fixed yet.
					relo_entry.reladdr = INVALID;
					relo_entry.symbol_handle = (*j).symbol->get_handle();
				}
				else
				{
					// This symbol is out of file.
					relo_entry.reladdr = INVALID;
					relo_entry.symbol_handle = (*j).symbol->get_handle();
				}
			}
			relos.push_back(relo_entry);
		}
	}

	section_info.header.RelocationOffset = section_info.header.SectionSize;

	// Write out relocations.
	std::vector<SRelocationEntry, std::allocator<SRelocationEntry> >::iterator j;
	for (j = relos.begin(); j != relos.end(); ++j)
	{
		WriteFile(hfile, &(*j), sizeof(SRelocationEntry), &lbytes, NULL);
		assert(lbytes == sizeof(SRelocationEntry));

		++section_info.header.RelocationCount;
	}

	section_info.header.SectionSize += section_info.header.RelocationCount*sizeof(SRelocationEntry);

	return section_info.header.SectionSize;
}

#ifdef _DEBUG
void CSection::dump()
{
	std::cout << "Section @" << (void*)this << ":\n";
	section_info.dump();
	std::cout << "<Areas>(list):\n";
	std::vector<CArea*, std::allocator<CArea*> >::iterator i;
	for (i = areas.begin(); i != areas.end(); ++i)
		(*i)->dump();
}
#endif

//*********************************************************************************************
// CSection::SSectionInfo

CSection::SSectionInfo::SSectionInfo()
{
	memset(&header, 0, sizeof(SSectionHeader));
	image_addr = 0;
	relocations = 0;
}

CSection::SSectionInfo::SSectionInfo(const SSectionHeader& sh)
{
	header = sh;
	image_addr = 0;
	relocations = 0;
}

bool CSection::SSectionInfo::operator==(const SMemRef& memref)
{
	return header.SectionHandle == memref.handle;
}

#ifdef _DEBUG
void CSection::SSectionInfo::dump()
{
	std::cout << "Section Info:\n";
	// Dump header.
	std::cout << " <SectionHandle>" << header.SectionHandle << '\n';
	std::cout << " <Flags>0x";
	std::cout.setf(std::ios::hex, std::ios::basefield);
	std::cout << header.Flags << '\n';
	std::cout.setf(std::ios::dec, std::ios::basefield);
	std::cout << " <SectionOffset>" << header.SectionOffset << '\n';
	std::cout << " <SectionSize>" << header.SectionSize << '\n';
	std::cout << " <RelocationOffset>" << header.RelocationOffset << '\n';
	std::cout << " <RelocationCount>" << header.RelocationCount<< '\n';

	// Dump rest.
	std::cout << " <image addr>@" << (void*)image_addr;
	std::cout << " <relocations addr>@" << relocations;
	std::cout << " <resolve to symbols>" << resolve_to_symbols << '\n';
}
#endif

//*********************************************************************************************
//
// CArea

int CArea::write_data(const void* data, int size)
{
	memory.write(data,size);
	return size;
}

int CArea::write_reference(CArea* area)
{
	SAreaRelocation relo(area);
	relo.AB_fixup = memory.off_free;
	AB_relocations.push_back(relo);
	// TODO:
	char dummy[4] =	{'\0','\0','\0','\0'};
	memory.write(dummy,4);
	return 4;
}

int CArea::write_reference(TSymbol* sym)
{
	SAreaRelocation relo(sym);
	relo.AB_fixup = memory.off_free;
	AB_relocations.push_back(relo);
	// TODO:
	char dummy[4] =	{'\0','\0','\0','\0'};
	memory.write(dummy,4);
	return 4;
}

#ifdef _DEBUG
void CArea::dump()
{
	std::cout << "Area @" << (void*)this << ":\n";
	memory.dump();
	std::cout << "<SR offset>" << SR_offset << '\n';
	std::cout << "<Symbol> ";
	if (symbol)
		symbol->dump();
	else
		std::cout << "0\n";
	std::cout << "<Area relocations>(list):\n";
	std::vector<SAreaRelocation, std::allocator<SAreaRelocation> >::iterator i;
	for (i = AB_relocations.begin(); i != AB_relocations.end(); ++i)
		(*i).dump();
}
#endif

//*********************************************************************************************
//
// CArea::SAreaRelocation

#ifdef _DEBUG
void CArea::SAreaRelocation::dump()
{
	std::cout << "Area Relocation:";
	std::cout << " <AB fixup offset>" << AB_fixup;
	std::cout << " <Area of fixup>@" << (void*)area;
	std::cout << " <Symbol of fixup> ";
	if (symbol)
		symbol->dump();
	else
		std::cout << "0\n";
}
#endif

//*********************************************************************************************
//
// CArea::SAreaMemory

CArea::SAreaMemory::SAreaMemory(int initial_size)
{
	assert (initial_size > 0);
	pstart = (char*)malloc(initial_size);
	assert(pstart);
	off_free = 0;
	off_end = initial_size;
}

CArea::SAreaMemory::~SAreaMemory()
{
	free(pstart);
}

char* CArea::SAreaMemory::reserve(int size)
{
	if (size > off_end - off_free)
	{
		int new_size = max(DEF_AREA_SIZE, size);
		pstart = (char*)realloc(pstart, new_size);
		off_end += new_size;
	}
	char* pnew = pstart + off_free;
	off_free += size;
	return pnew;
}

void CArea::SAreaMemory::write(const void* pbuf, int size)
{
	char* pnew = reserve(size);
	memcpy(pnew, (char*)pbuf, size);
}

#ifdef _DEBUG
void CArea::SAreaMemory::dump()
{
	std::cout << "Area Memory:";
	std::cout << " <start address>@" << (void*)pstart;
	std::cout << " <free offset>" << off_free;
	std::cout << " <end offset>" << off_end;
}
#endif
