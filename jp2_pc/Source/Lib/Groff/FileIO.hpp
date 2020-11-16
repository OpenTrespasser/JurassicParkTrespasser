/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1996
 *
 * Contents: This class contains all the functions which comprise the implementation of the 
 * COFF oriented file structure.  This module contains routines for construction of the image
 * as well as I/O routines for reading or writing the image to or from disk.
 *
 * 
 * Bugs:
 *
 * To do:
 *  1. Convert the low level file I/O calls to Win32 equivalent.
 *
 *  2. Resolve remaining relocation issues.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/FileIO.hpp                                                 $                                                                                      $
 * 
 * 9     8/25/98 9:17a Shernd
 * Added callback functionality
 * 
 * 8     98.08.21 7:19p Mmouni
 * Added  bResetSection.
 * 
 * 7     98.05.19 2:33p Mmouni
 * Optmized symbol table lookup so loading isn't so slow.
 * 
 * 6     11/09/97 4:44p Agrant
 * Made several strings const
 * 
 * 5     6/18/97 7:34p Gstull
 * Added changes to support fast exporting.
 *
 *********************************************************************************************/

#ifndef HEADER_LIB_FILEIO_HPP
#define HEADER_LIB_FILEIO_HPP

#include <string.h>

#ifdef USE_MAX_TYPES
#include "StandardTypes.hpp"
#else
#include "common.hpp"
#endif

#include "Lib/Sys/SysLog.hpp"
#include "Lib/Sys/SmartBuffer.hpp"

#undef min
#undef max

#include <vector>
#include <set>

#ifndef PFNWORLDLOADNOTIFY
typedef uint32 (__stdcall * PFNWORLDLOADNOTIFY)(uint32 dwContext, uint32 dwParam1, uint32 dwParam2, uint32 dwParam3);
#endif

//
// Define the handle types for referencing symbols and handles.
//
// Prefix: syh
typedef uint TSymbolHandle;

// Prefix: seh
typedef uint TSectionHandle;

// Version number to keep track of architectural changes for upward compatibility.
#define VERSION_ID		10;			// Version 1.0

// "Arbitrary" magic number to distinguish this file structure from others. 
#define MAGIC_NUMBER	0xACEBABE

// Constant for use with the GROFF file header flags field.
#define uHASREFERENCES	0x00000001
#define uHASRELOCATION	0x00000002
#define uHASSECTIONS   	0x00000004
#define uHASSYMBOLS 	0x00000008
#define uCRCENCODING	0x00000010
#define uUSELOGFILE		0x00000020

// Constants for specifying file creation criteria.
enum EOpenModes
// Prefix: om
{
	eClosed,	// No file is currently is use.
	eRead,		// Read operations will be performed on the file.
	eWrite		// Write operations will be performed on the file.
};

// Reason codes which describe why an operation didn't complete successfully.
enum EReasonCode
// Prefix: rc
{
	eSuccess,				// Last operation was successful.
	eFileAccessError,		// Unable to open file (directory or sharing error).
	eFileNotFound,			// Requested file to open for reading does not exist.
	eNoFileHandles,			// All available file handles have been allocated.
	eInvalidMode,			// Attempted to access the file in wrong mode.
	eFileClosed,			// Attempted a file operation on a closed file.
	eInvalidImage,			// The image was rejected by the validator.
	eRangeError,			// Parameter is out of range.
	eFileCreationError,		// Unable to create the file requested.
	eCRCError,				// CRC failure.
	eUnrecognizedSymbol,	// The symbol was not located in the symbol table.
	eBadHandle,				// The handle specified is invalid.
	eBadFile,				// File has a bad or invalid header.
	eReadPastEOF,			// Attempted to read past the end of file.
	eDiskFull,				// The disk being written to is full.
	eFileNotOpen,			// The file must be open for this operation to succeed.
	eSectionListEmpty,		// Attempt to perform an operation on an empty section list.
	eUnableToDelete,		// Unable to delete smart buffer.
	eNullBuffer,			// Attempted to perform operation with null buffer.
	eNullBufferNonZero		// Attempted to perform operation with null buffer and non zero count.
};


// Definition of the file header structure.
struct SFileHeader
// Prefix: fh
{
	uint uMagicNumber;	// Magic number to identify a GROFF file.
	uint uFileSize;		// The actual size of the file.
	uint uSectionCount;	// Number of sections in the file.
	uint uSymtabEntries;	// Size of the symbol table.
	uint uSymtabSize;		// The size of the symbol table.
	uint uSymtabOffset;	// Offset to the start of the symbol table.
	uint uTimeStamp;		// Encoded date and time of GROFF file.
	uint uFlags;			// Field providing general file information.
	uint uVersionNumber;	// Version number of this file.

	uint uReserved[2];	// Reserved for future use. (Assume = 0)

	uint uCRC;			// CRC of the header.
} ;


// Define the structure for section headers.
struct SSectionHeader
// Prefix: sh
{
	TSymbolHandle	syhSectionName;		// Handle to the section name symbol.
	uint			uSectionFlags;		// Flags field used by FileIO implementation .
	uint			uSectionOffset;		// Offset to start of the section data.
	uint			uSectionSize;		// The size of the data within the section.
	uint			uRelocationOffset;	// Offset to start of the relocation data.
	uint			uRelocationCount;	// Number of relocations for this section.
	uint			uUserFlags;			// Flags field defined and used by the user.
	TSectionHandle	sehSectionHandle;	// Handle to this section.
};


// Define the structure for symbol table entries.
struct SSymbolEntry
// Prefix: se
{
	TSymbolHandle	syhHandle;			// Handle to the symbol entry.
	uint			uNameLength;		// Includes terminating null char.
	uint			uReferenceCount;	// The number of times a symbol is referenced. 
	char*			strName;			// Pointer to the string.
};


// Define the structure for relocation entries.
struct SRelocationRecord
// Prefix: rr
{
	uint			uID;		// The ID of this relocation record
	uint			uOffset;	// Offset from start of section to reloc word.
	uint			uFlags;		// Flags indicating what the section is doing,
};


// Define the structure to keep the list of relocations.
struct SRelocationList
// Prefix: rl
{
	SRelocationRecord*	prrRecord;	// Pointer to the relocation record
	SRelocationList*	prlNext;	// Pointer ot the next relocation record;				
};


// Define the structure for keeping track of the section header list.
struct SSectionList
// Prefix: sl
{
	SSectionHeader*		pshSection;	// Pointer to the section header
	TBufferHandle		bhHandle;	// Handle to the buffer for this section.
	SRelocationList*	prlReloc;	// Pointer to the list of relocation records
};


//*********************************************************************************************
//
class SSymbolHandleLess
//
// An object to compare two CPal* objects.
//
// Prefix: Not required.
//
//**************************************
{
public:
	bool operator()
	(
		SSymbolEntry* const& sym_0,
		SSymbolEntry* const& sym_1
	) const
	//
	// Returns a comparison between the addresses of the two palettes.
	//
	//**************************************
	{
		return sym_0->syhHandle < sym_1->syhHandle;
	}
};


//*********************************************************************************************
//
class SSymbolNameLess
//
// An object to compare two CPal* objects.
//
// Prefix: Not required.
//
//**************************************
{
public:
	bool operator()
	(
		SSymbolEntry* const& sym_0,
		SSymbolEntry* const& sym_1
	) const
	//
	// Returns a comparison between the addresses of the two palettes.
	//
	//**************************************
	{
		return strcmp(sym_0->strName, sym_1->strName) < 0;
	}
};


// Define the structure for the symbol table entries.
struct SSymbolTable
// Prefix: st
{
	std::set<SSymbolEntry*, SSymbolHandleLess>	setHandle;		// Set sorted by handle.
	std::set<SSymbolEntry*, SSymbolNameLess>		setName;		// Set sorted by name.

	// Insert symbol.
	void insert(SSymbolEntry* pse_symbol)
	{
		setHandle.insert(pse_symbol);
		setName.insert(pse_symbol);
	}

	// Check if both sets are emtpy.
	bool empty()
	{
		return setHandle.empty() || setName.empty();
	}

	// Find string.
	std::set<SSymbolEntry*, SSymbolNameLess>::iterator find(const char* str_name)
	{
		SSymbolEntry se_temp;

		se_temp.syhHandle = 0;
		se_temp.uReferenceCount = 0;
		se_temp.uNameLength = strlen(str_name) + 1;
		se_temp.strName = (char *)str_name;

		return setName.find(&se_temp);
	}

	// Find handle.
	std::set<SSymbolEntry*, SSymbolHandleLess>::iterator find(TSymbolHandle syh_handle)
	{
		SSymbolEntry se_temp;

		se_temp.syhHandle = syh_handle;
		se_temp.uReferenceCount = 0;
		se_temp.uNameLength = 0;
		se_temp.strName = 0;

		return setHandle.find(&se_temp);
	}
};


// Define the section information record.
struct SSectionInfo
// Prefix: si
{
	TSymbolHandle	syhHandle;			// Handle to the section name.
	uint			uReferenceCount;	// The number of unresolved references.
	uint			uSectionSize;		// The size of this section in bytes.
	uint			uFlags;				// The user flags asscoiated with the section.
};


// Define the file information record.
struct SFileInfo
// Prefix: fi
{
	uint			uVersionNumber;	// Version number of this file format.
	uint			uSectionCount;	// The number of sections in the file.
	SSectionInfo*	siSectionInfo;	// A list of info records for each section.
	TSectionHandle*	sehHandles;		// A list of section handles to this file.
};


// Define a structure to keep track of the file structure.
struct SFileStructure
// Prefix: fs
{
	SFileHeader			 fhHeader;		// The file header.
	std::vector<SSectionList> slSections;	// A list of all the sections.
	SSymbolTable		 pstSymbols;	// A list of all the symbols.
	int					 fdFile;		// Define a handle to the file using streams.
	EOpenModes			 omOpenMode;	// In what mode was the file opened?
	bool				 bImageOpen;	// Is there currently an image in memory.
	EReasonCode			 rcErrorCode;	// When an error occurs, keep track of the reason.
	CSysLog				 slLogfile;		// Keep a logfile for debugging purposes.
	bool				 bInfoValid;	// Is the user info structure morphology current?
	SFileInfo			 fiInfo;		// User information describing the current file.
	CSmartBuffer		 sbBuffers;		// Smart buffers containing raw section data.
};


//*********************************************************************************************
//
class CFileIO
//
// Prefix: fio
//
// This class is intended to be used as the lower layer of a two layer file structure which
// represents a section based file structure which supports relocation and cross referencing.
// The upper layer, known as the GRaphics Object File Format (GROFF) is encapsulated within
// the individual data sections.  Routines are provides for creating, loading and saving file
// images as well as randomly accessing the data within the sections.  
//
// The structures which represent the file are not intended to be directly accessed by the 
// user and should never be externally modified.  Member functions have been provided to 
// assist the user in accessing the specific information required within the raw data section.
// Internally, all sections of a file are internally represented in a collection of smart 
// buffers which manage the users data.  These buffers operate in different modes depending
// upon whether the file was initially opened in either read or write mode.  More on this 
// later.
//
// Each section has an associated handle which is supplied to the I/O functions for accessing 
// the data within the sections.  This implementation supports byte aligned accesses as well
// as byte size sections.
//
// This file management module operates in one of two modes:  With read or write.  Read mode
// operates by supplying the open function with a file name.  Once a file has been opened,
// the actual image needs to be loaded from disk into the internal representation.  This is
// accomplished through the LoadImage entry point.  The FileInfo entry point will return a 
// list of information about the sections which exist within this file, including section
// handles which are used to access the data within the sections.
//
// Section handles are used to identify the section that a specific I/O operation should work
// on.  When a file has been opened in read mode, I/O operations, such as read, will not allow
// the user to run off of the end of the section.
//
// When a file has been opened in write mode, certain behaviors of the I/O functions change. 
// First, when the file is initially opened, if it previously existed, it is truncated to 
// zero length.  Sections are added to the file by calling the create section function which
// returns a section handle to the caller.  Data is added to the section by writing to it. As
// the section grows in size, the underlying buffers will automatically grow in size.
//
// Once all the sections in the image have been created and loaded with data, the image needs
// to be written to disk through the write image entry point.  At this point the close entry
// point should be called to close the internal file.
//
//*********************************************************************************************
{
	SFileStructure fsFile;	// Structural file information for the current file.

	//*****************************************************************************************
	//
	// Disk read routine
	bool bRead
	(
		void* v_buffer, 
		int   i_count
	);
	//
	// This is a low level read function used internally by this class to read blocks of data
	// from the file system.  This function should NEVER be used outside of this class.
	//
	// Returns:
	//		Boolean which indicates if the entire requested block could be read.
	//
	// 
	//*****************************************************************************************
	
	//*****************************************************************************************
	//
	// Disk write routine
	bool bWrite
	(
		void* v_buffer, 
		int   i_count
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Routine for converting a section handle into an index.
	TSectionHandle sehToHandle
	(
		uint u_index
	)
	{ 
		return (0x80000000 + u_index); 
	};
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Routine for converting an index into a section handle.
	uint uToIndex
	(
		TSectionHandle seh_handle
	) 
	{ 
		return (seh_handle - 0x80000000); 
	};
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Function for processing any errors which arise as a result from file system exceptions.
	void ProcessError
	(
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Lookup a section list structure using the section handle.
	SSectionList* slLookup
	(
		TSectionHandle seh_handle
	);
	//
	//*****************************************************************************************

public:
	//*****************************************************************************************
	//
	// Constructor 
	CFileIO
	(
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Destructor
	~CFileIO
	(
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	SSymbolEntry* pseNewNode
	(
		const char* str_name		// Name of the new symbol.
	);
	//
	// Insert a symbol into the table.
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// This method is used for opening the file.
	bool bOpen
	(
		const char* str_filename, 
		EOpenModes  om_mode
	);
	//
	//*****************************************************************************************
	
	//*****************************************************************************************
	//
	// This method closes the current file,
	bool bClose
	(
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// The method is used to create new section in the currently open file.
	TSectionHandle sehCreate
	(
		const char* str_section_name, 
		uint		u_flags
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// The method is used to delete an existing section in the currently open file.
	// Note: this is not currently working.
	//
	bool bRemove
	(
		TSectionHandle sh_handle
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Make the section zero size.
	//
	bool bResetSection
	(
		TSectionHandle sh_handle
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Insert a symbol in the symbol table.
	TSymbolHandle syhInsert(const char* str_symbol_name);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Delete a symbol from the symbol table.
	bool bDelete
	(
		const char* str_symbol_name
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Delete a symbol from the symbol table.
	bool bDelete
	(
		TSymbolHandle syh_handle
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Lookup a symbol's handle.
	TSymbolHandle syhLookup
	(
		const char* str_symbol_name
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Lookup a section's handle.
	TSectionHandle sehLookup
	(
		const char* str_symbol_name
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Lookup a symbol name based upon a handle.
	const char* strLookup
	(
		TSymbolHandle syh_handle
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Read a number of bytes from the current file position in the section.
	int	iRead
	(
		TSectionHandle seh_handle, 
		void*		   v_buffer, 
		int			   i_count
	);
	//
	//*****************************************************************************************
	
	//*****************************************************************************************
	//
	// Write a number of bytes to the current file position in the section.
	int	iWrite
	(
		TSectionHandle seh_handle, 
		void*		   v_buffer, 
		int			   i_count
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Set the current file position in a section.
	int	iSeek
	(
		TSectionHandle seh_handle, 
		int			   i_position
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Return the current file position in a section.
	int	iTell
	(
		TSectionHandle seh_handle
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
  	// Return the number of bytes in a section's raw data buffer.
	int	iCount
	(
		TSectionHandle seh_handle
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Set the flags associated with a section.
	bool bEOF
	(
		TSectionHandle seh_handle, 
		bool*		   b_result
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Set the flags associated with a section.
	bool bSetFlags
	(
		TSectionHandle seh_handle, 
		uint		   u_flags
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Get the flags associated with a section.
	bool bGetFlags
	(
		TSectionHandle seh_handle, 
		uint*		   u_flags
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Member function to validate the file image prior to output.
	bool bValidateImage
	(
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Member function to read a file image in from disk and into memory.
	bool bReadImage
	(
        PFNWORLDLOADNOTIFY pfnWorldNotify = NULL, 
        uint32 ui32_NotifyParam = 0
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Member function to write a memory file image out to disk.
	bool bWriteImage
	(
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Member function which deallocates and reinitializes any image structures.
	bool bDeleteImage
	(
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Get information which describes the current file.
	SFileInfo* FileInfo
	(
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Dump the internal representation of the current file.
	void Dump
	(
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// These methods returns an error or warning message describing the condition. 
	const char* GetMessage()
	{
		// Error messages
		const char* str_message[] = {
			"Operation successful.",
			"File was either, a directory, read-only file opened for writing, or a file sharing error.",
			"File or path was not found.",
			"No more file handles available.",
			"Attempted to access disk file opened in incompatible mode.",
			"File is not opened.",
			"The image in it's current state is invalid.",
			"Specified range for requested operation is invalid.",
			"Unable to create the requested.",
			"Cyclic redundency check error.",
			"The symbol provided was not found in the symbol table,",
			"Invalid handle.",
			"File has a bad or invalid header.",
			"Attempt to read past the end of the file.",
			"The disk is full.",
			"The file must be open for this operation to succeed.",
			"Attempted to perform an operation on an empty section list.",
			"Unable to delete smart buffer.",
			"Attempted to perform an operation on a null buffer.",
			"Attempted to perform an operation on a null buffer, with non-zero count.",
		};


		// Return the message.
		return str_message[fsFile.rcErrorCode];
	}
	//
	//*****************************************************************************************
};

#endif