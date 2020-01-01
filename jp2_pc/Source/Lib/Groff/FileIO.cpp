/***********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1996
 *
 * Implementation of module FileIO.
 *
 * NOTE: If the GRF file is opened as read only it is opened with the LZ... API to enable
 *			processing of compressed GRF files. If write access is requested then the normal
 *			_open, _close functions are used.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Groff/FileIO.cpp                                                  $                                                                                      $
 * 
 * 22    9/27/98 2:13a Mmouni
 * Fixed load progress to be more consistent.
 * 
 * 21    8/26/98 4:38p Asouth
 * Loop variables for MW; fixed unusual _open fn call
 * 
 * 20    8/25/98 9:17a Shernd
 * Added callback functionality
 * 
 * 19    98.08.21 7:21p Mmouni
 * Added bResetSection.
 * 
 * 18    8/20/98 1:58p Rwyatt
 * Added LZ compression support to GRF files.
 * 
 * 17    98.05.19 2:33p Mmouni
 * Optmized symbol table lookup so loading isn't so slow.
 * 
 * 16    98/02/20 19:39 Speter
 * Added _O_TRUNC to mode for files opened for write.  Previous version left old file contents
 * at end when rewriting files.
 * 
 * 15    11/09/97 4:44p Agrant
 * Made several strings const
 * 
 * 14    10/22/97 1:34a Gstull
 * Changes to support version 12 of the GROFF file structure definitions.  This mainly includes
 * support for text attributes for use with the attribute compiler.
 * 
 * 13    6/23/97 7:30p Gstull
 * Added backward compatibility fix to lookup sections by the 'old' handle encoding method if
 * the next method fails.
 * 
 * 12    6/19/97 6:57p Gstull
 * Fixed indexing bug in symbol lookup routine.
 * 
 * 11    6/18/97 7:34p Gstull
 * Added changes to support fast exporting.
 *
 **********************************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#ifndef __MWERKS__
 #include <sys\types.h>
 #include <sys\stat.h>
#endif
#include "Lib/W95/WinInclude.hpp"

#ifdef USE_MAX_TYPES
#include "StandardTypes.hpp"
#else
#include "Common.hpp"
#endif

#include "Lib/Sys/SysLog.hpp"
#include "Lib/Sys/SmartBuffer.hpp"
#include "Lib/Groff/FileIO.hpp"

#ifdef USE_MAX_TYPES
#include "Tools/GroffExp/GUIInterface.hpp"

extern CGUIInterface guiInterface;
#endif


//**********************************************************************************************
//
// Class CFileIO implementation.
//

//**********************************************************************************************
//
// Implementation of CFileIO::CFileIO
//

CFileIO::CFileIO()
{
	// Set the file handle to a closed file.
	fsFile.fdFile = 0;

	// Set the error code to success.
	fsFile.rcErrorCode = eSuccess;

	// Mark the user information as invalid.
	fsFile.bInfoValid = false;

	//
	// Initialize the file structure.
	//
	memset(&fsFile.fiInfo, 0, sizeof(SFileInfo));
	memset(&fsFile.fhHeader, 0, sizeof(SFileHeader));

	// First initialize the file header.
	fsFile.fhHeader.uMagicNumber	= MAGIC_NUMBER;
	fsFile.fhHeader.uVersionNumber	= VERSION_ID;

	// Mark this structure indicating the structure is empty (i.e. no-image).
	fsFile.bImageOpen = false;

#ifdef USE_MAX_TYPES
	// Determine whether the file should be active or not.
	if (guiInterface.bGenerateLogfiles())
	{
		// Open the log file.
		char str_logfile[256];

		// Construct the proper path for the logfile.
		guiInterface.BuildPath(str_logfile, guiInterface.strGetLogfileDirPath(), "FileIO.log");
		fsFile.slLogfile.Open(str_logfile);

		// Activate the logfile.
		fsFile.slLogfile.Enable();
	}
	else
	{
		// Deactivate the logfile.
		fsFile.slLogfile.Disable();
	}

#else
	fsFile.slLogfile.Disable();
#endif
}


//**********************************************************************************************
//
// Implementation of CFileIO::~CFileIO.
// 
CFileIO::~CFileIO()
{
	// Is the file currently open?  If it was opened as writeable, this should never happen.
	if (fsFile.fdFile != -1)
	{
		// Then close the file.
		fsFile.fdFile = close(fsFile.fdFile);
	}

	// Destroy any existing image in memory.
	bDeleteImage();

	// Close the logfile.
	fsFile.slLogfile.Close();
}



//**********************************************************************************************
//
// Implementation of CFileIO::ProcessError.
// 
void CFileIO::ProcessError()
{
	// Determine what happened?
	switch (errno)
	{
		case EACCES:
			// Specified file is either: 1) a directory 2) sharing modes dont allow us to open.
			fsFile.rcErrorCode = eFileAccessError;

			break;

		case ENOENT:
			// Specified file was not found.
			fsFile.rcErrorCode = eFileAccessError;

			break;

		case EMFILE:
			// Specified file is either: 1) a directory 2) sharing modes dont allow us to open.
			fsFile.rcErrorCode = eNoFileHandles;

			break;

		case EBADF:
			// This is a bad file handle.
			fsFile.rcErrorCode = eBadHandle;

			break;

		case ENOSPC:
			// This is a bad file handle.
			fsFile.rcErrorCode = eDiskFull;

			break;
	}
}


//**********************************************************************************************
//
// Implementation of CFileIO::bRead.
// 
bool CFileIO::bRead(void* v_buffer, int i_count)
{
	char	str_message[256];	
	int		i_bytes_read;


	// Is the user loading an existing file or creating a new one?
	
	// Attempt to read from the file.

	if (fsFile.omOpenMode == eRead)
	{
		i_bytes_read = LZRead(fsFile.fdFile, (char*)v_buffer, i_count);
		// Are we using a bad file descriptor?
		if (i_bytes_read < 0)
		{
			// Yes! Setup an error code and return an error result.
			fsFile.rcErrorCode = eBadHandle;

			// Return an error result.
			return false;
		}

	}
	else
	{
		i_bytes_read = _read(fsFile.fdFile, (char*)v_buffer, i_count);

		// Are we using a bad file descriptor?
		if (i_bytes_read == -1)
		{
			// Yes! Setup an error code and return an error result.
			fsFile.rcErrorCode = eBadHandle;

			// Return an error result.
			return false;
		}

	}

	// Were we successful?
	if (i_bytes_read == i_count)
	{
		// Return a successful result.
		return true;
	}

	// Are we at the end of the file?
	if (i_bytes_read == 0)
	{
		// Yes! Setup a error code and return an error result.
		fsFile.rcErrorCode = eReadPastEOF;

		// Return a successful result.
		return false;
	}

	// We must have been able to read only a portion of the requested number of bytes.
	sprintf(str_message, "Attempted to read %d bytes and was only able to read %d bytes.",
		i_count, i_bytes_read);
	fsFile.slLogfile.Msg(str_message);

	// Return an error result.
	return false;
}


//**********************************************************************************************
//
// Implementation of CFileIO::bWrite.
// 
bool CFileIO::bWrite(void* v_buffer, int i_count)
{
	int		i_bytes_written;

	// Attempt to write to the file
	i_bytes_written = _write(fsFile.fdFile, (char*)v_buffer, i_count);

	// Were we successful?
	if (i_bytes_written == i_count)
	{
		// Return a successful result.
		return true;
	}

	// Are we at the end of the file?
	if (i_bytes_written == 0)
	{
		// Yes! Setup a error code and return an error result.
		fsFile.rcErrorCode = eReadPastEOF;

		// Return an error result.
		return false;
	}

	// Are we using a bad file descriptor?
	if (i_bytes_written == -1)
	{
		// Figure out and report what happened.
		ProcessError();

		// Return an error result.
		return false;
	}

	// We must have been able to read only a portion of the requested number of bytes.
	fsFile.slLogfile.Printf("Attempted to write %d bytes and was only able to write %d bytes.\n", i_count, i_bytes_written);

	// Return an error result.
	return false;
}


//**********************************************************************************************
//
// Implementation of CFileIO::stNewNode.
// 
SSymbolEntry* CFileIO::pseNewNode(const char* str_name)
{
	SSymbolEntry* pse_symbol = new SSymbolEntry;

	// Allocate memory for the string
	pse_symbol->uNameLength = strlen(str_name) + 1;
	pse_symbol->strName		= new char[pse_symbol->uNameLength];

	// Setup the reference count for this symbol.
	pse_symbol->uReferenceCount = 1;

	// Setup the node
	strcpy(pse_symbol->strName, str_name);
	pse_symbol->syhHandle = (TSymbolHandle) pse_symbol;

	// Insert into the symbol table.
	fsFile.pstSymbols.insert(pse_symbol);

	return pse_symbol;
}


//**********************************************************************************************
//
// Implementation of CFileIO::syhInsert.
// 
TSymbolHandle CFileIO::syhInsert(const char* str_name)
{
	SSymbolEntry* pse_symbol;

	// Is this file structure marked closed?
	if (!fsFile.bImageOpen)
	{
		// Mark the file structure as open.
		fsFile.bImageOpen = true;
	}

	// Is the list empty?
	if (fsFile.pstSymbols.empty())
	{
		// Mark this file as having symbols.
		fsFile.fhHeader.uFlags |= uHASSYMBOLS;

		// Setup the node.
		pse_symbol = pseNewNode(str_name);
	}
	else
	{
		// Check if this symbol is in the list.
		std::set<SSymbolEntry*, SSymbolNameLess>::iterator it = fsFile.pstSymbols.find(str_name);
		if (it == fsFile.pstSymbols.setName.end())
		{
			// Add to list.
			pse_symbol = pseNewNode(str_name);
		}
		else
		{
			// Increment reference count.
			(*it)->uReferenceCount++;
		}
	}

	// Return the handle to the symbol.
	return pse_symbol->syhHandle;
}
	
	
//**********************************************************************************************
//
// Implementation of CFileIO::bDelete.
// 
bool CFileIO::bDelete(const char* str_name)
{
	// Lookup the symbol by name.
	std::set<SSymbolEntry*, SSymbolNameLess>::iterator it_name = fsFile.pstSymbols.find(str_name);

	// Was the symbol found?
	if (it_name != fsFile.pstSymbols.setName.end())
	{
		SSymbolEntry* pse_symbol = (*it_name);

		// Is the reference count == 1?
		if (pse_symbol->uReferenceCount == 1)
		{
			// Yes! Get rid of this node.
			fsFile.pstSymbols.setName.erase(it_name);

			// Remove it from the handle set as well.
			std::set<SSymbolEntry*, SSymbolHandleLess>::iterator it_handle = fsFile.pstSymbols.find(pse_symbol->syhHandle);
			Assert(it_handle != fsFile.pstSymbols.setHandle.end());
			fsFile.pstSymbols.setHandle.erase(it_handle);

			// Free it's memory.
			delete [] pse_symbol->strName;
			delete    pse_symbol;

			// Is the symbol table empty?
			if (fsFile.pstSymbols.empty())
			{
				// Turn off the symbol flag.
				fsFile.fhHeader.uFlags &= ~uHASSYMBOLS;
			}
		}
		else
		{
			// Decrement the reference count on this symbol.
			pse_symbol->uReferenceCount--;
		}

		// Return to the caller with a successful result.
		return true;
	}

	// Yes!  So, the symbol is not in the symbol table.  Report the error.
	fsFile.rcErrorCode = eUnrecognizedSymbol;

	// Return an error result.
	return false;
}


//**********************************************************************************************
//
// Implementation of CFileIO::
// 
bool CFileIO::bDelete(TSymbolHandle syh_handle)
{
	// Lookup the symbol by handle.
	std::set<SSymbolEntry*, SSymbolHandleLess>::iterator it_handle = fsFile.pstSymbols.find(syh_handle);

	// Was the symbol found?
	if (it_handle != fsFile.pstSymbols.setHandle.end())
	{
		SSymbolEntry* pse_symbol = (*it_handle);

		// Is the reference count == 1?
		if (pse_symbol->uReferenceCount == 1)
		{
			// Yes! Get rid of this node.
			fsFile.pstSymbols.setHandle.erase(it_handle);

			// Remove it from the name set as well.
			std::set<SSymbolEntry*, SSymbolNameLess>::iterator it_name = fsFile.pstSymbols.find(pse_symbol->strName);
			Assert(it_name != fsFile.pstSymbols.setName.end());
			fsFile.pstSymbols.setName.erase(it_name);

			// Free it's memory.
			delete [] pse_symbol->strName;
			delete    pse_symbol;

			// Is the symbol table empty?
			if (fsFile.pstSymbols.empty())
			{
				// Turn off the symbol flag.
				fsFile.fhHeader.uFlags &= ~uHASSYMBOLS;
			}
		}
		else
		{
			// Decrement the reference count on this symbol.
			pse_symbol->uReferenceCount--;
		}

		// Return to the caller with a successful result.
		return true;
	}

	// Report that the user passed in a bad section handle.
	fsFile.rcErrorCode = eBadHandle;

	// Return an error result.
	return false;
}


//**********************************************************************************************
//
// Implementation of CFileIO::syhLookup.
// 
TSymbolHandle CFileIO::syhLookup(const char* str_name)
{
	// Lookup the symbol by name.
	std::set<SSymbolEntry*, SSymbolNameLess>::iterator it_name = fsFile.pstSymbols.find(str_name);

	// Was the symbol found?
	if (it_name != fsFile.pstSymbols.setName.end())
	{
		return (*it_name)->syhHandle;
	}

	// Yes!  So, the symbol is not in the symbol table.  Report the error.
	fsFile.rcErrorCode = eUnrecognizedSymbol;

	// Return an invalid symbol handle.
	return 0;
}


//**********************************************************************************************
//
// Implementation of CFileIO::shLookup.
// 
TSectionHandle CFileIO::sehLookup(const char* str_name)
{
	// Start out by looking up the sections symbol name.
	TSymbolHandle syh_handle = syhLookup(str_name);

	// Was the symbol found?
	if (syh_handle == 0)
	{
		// No! Return a null handle.
		return 0;
	}

	// 
	// Now attempt to locate the section with this symbol.
	// 

	// Are there any sections in the file?
	for (uint u_index = 0; u_index < fsFile.slSections.size(); u_index++)
	{
		// Is this the section list node we are looking for?
		if (fsFile.slSections[u_index].pshSection->syhSectionName == syh_handle)
		{
			// Yes!  Return the address of this section list node.
			return fsFile.slSections[u_index].pshSection->sehSectionHandle;
		}
	}

	// Entire list was traversed and the node was not found.  Bad section handle.
	fsFile.rcErrorCode = eBadHandle;

	// Return an invalid section handle.
	return 0;
}


//**********************************************************************************************
//
// Implementation of CFileIO::strLookup.
// 
const char* CFileIO::strLookup(TSymbolHandle syh_handle)
{
	// Lookup the symbol by handle.
	std::set<SSymbolEntry*, SSymbolHandleLess>::iterator it_handle = fsFile.pstSymbols.find(syh_handle);

	// Was the symbol found?
	if (it_handle != fsFile.pstSymbols.setHandle.end())
	{
		return (*it_handle)->strName;
	}

	// Yes!  The user has passed in a bad handle.  Setup an error.
	fsFile.rcErrorCode = eBadHandle;

	// Return an error.
	return 0;
}


//**********************************************************************************************
//
// Implementation of CFileIO::slLookup.
// 
SSectionList* CFileIO::slLookup(TSectionHandle seh_handle)
{
	uint u_index = uToIndex(seh_handle);

	// Is this a valid handle?
	if (u_index < fsFile.slSections.size())
	{
	 	// Get the section at the specified index.
		return &fsFile.slSections[u_index];
	}

	// 
	// GROFF Version 11 - Compatibility change.
	//

	//
	// The symbol was not found so look through the section list individually to see if the 
	// section handle is present.
	//
	for (uint u_i = 0; u_i < fsFile.slSections.size(); u_i++)
	{
		// Is this is section we are looking for?
		if (seh_handle == fsFile.slSections[u_i].pshSection->sehSectionHandle)
		{
			// Yes! Then return this section.
			return &fsFile.slSections[u_i];
		}
	}

	// Entire list was traversed and the node was not found.  Bad section handle.
	fsFile.rcErrorCode = eBadHandle;

	// Return an error.
	return 0;
}


//**********************************************************************************************
//
// Implementation of CFileIO::bClose.
// 
bool CFileIO::bClose()
{
	if (fsFile.omOpenMode == eRead)
	{
		// The request must have been to open the file for writing.
		LZClose( fsFile.fdFile );
		fsFile.fdFile = 0;
	}
	else
	{
		if (fsFile.fdFile != 0 && fsFile.fdFile != -1)
			fsFile.fdFile =_close( fsFile.fdFile );
	}

	// Were we successful?
	if (fsFile.fdFile == -1)
	{
		// Process the error.
		ProcessError();

		// Return an error result.
		return false;
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
// Implementation of CFileIO::bOpen.
// 
bool CFileIO::bOpen(const char* str_filename, EOpenModes om_mode) 
{
	char str_message[256];
	OFSTRUCT	ofs_s;

	// Attempt to open the file.
	fsFile.omOpenMode = om_mode;

	// Is the user loading an existing file or creating a new one?
	if (om_mode == eRead)
	{
		// Attempt to open the file in read mode.
		//fsFile.fdFile = _open(str_filename, _O_RDONLY | _O_BINARY);
		fsFile.fdFile = LZOpenFile((char*)str_filename, &ofs_s, OF_READ);

		// Were we successful?
		if (fsFile.fdFile < 0)
		{
			// Process the error.
			ProcessError();

			// Return an error result.
			return false;
		}

		// 
		// Since this file is opened for reading, attempt to load the file into memory
		// and verify the header, sections, etc, are valid.
		//

		if (LZSeek( fsFile.fdFile, 0L, 0 ) < 0)
		{
			// Attempt to process the error
			ProcessError();

			// The request must have been to open the file for writing.  Don't check for
			// an error condition because the first error should be reported.
			fsFile.fdFile = _close(fsFile.fdFile);

			// Return an error since the seek failed.
			return false;
		}

		// Now attempt to read in the file header.
		if (!bRead(&fsFile.fhHeader, sizeof(SFileHeader)))
		{
			// Log the problem in the logfile.
			sprintf(str_message, "Error reading file header: %s", GetMessage());
			fsFile.slLogfile.Msg( str_message );

			// Return an error result.
			return false;
		}

		// Are we are looking at a valid file header?
		if (fsFile.fhHeader.uMagicNumber != MAGIC_NUMBER || fsFile.fhHeader.uVersionNumber != 10)
		{
			// No! Setup an error code, close the file and return.
			fsFile.rcErrorCode = eBadFile;

			// Don't check the result because we want the first error to be returned.
			fsFile.fdFile = _close(fsFile.fdFile);

			// Return an error result.
			return false;
		}
	}
	else
	{
		// The request must have been to open the file for writing.
		fsFile.fdFile = _open(str_filename, _O_RDWR | _O_CREAT | _O_TRUNC | _O_BINARY
	#ifndef __MWERKS__
			, _S_IWRITE
	#endif
			);

		// Were we successful?
		if (fsFile.fdFile == -1)
		{
			// Process the error.
			ProcessError();

			// Return an error result.
			return false;
		}
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
// Implementation of CFileIO::bWriteImage.
// 
bool CFileIO::bWriteImage()
{
	uint u_symbol_count;
	uint u_symbol_size;

	// Is the image valid?
	if (!bValidateImage())
	{
		// Return an error result.
		return false;
	}

	// Is the file currently open?
	if (fsFile.omOpenMode == eClosed)
	{
		// We cannot operate on a closed file.
		fsFile.rcErrorCode = eFileClosed;

		// Return an error result.
		return false;
	}

	// Was the file opened for writing?
	if (fsFile.omOpenMode != eWrite)
	{
		// We cannot write an image to a file not opened for writing.
		fsFile.rcErrorCode = eInvalidMode;

		// Return an error result.
		return false;
	}

	// Make sure the file pointer is at the start of the file.  Were we successful?
	if (_lseek(fsFile.fdFile, 0L, SEEK_SET) == -1)
	{
		// Something went wrong.  Let the error processor figure it out.
		ProcessError();

		// Return an error result.
		return false;
	}

	//
	// First we need to look through all the nodes and calculate all the offsets for indices
	// to the sections in the file.  Start out by determining the amount of storage required
	// by the symbol table.
 	// 

	// Determine the size of the symbol table.
	u_symbol_count = 0;
	u_symbol_size  = 0;

	// Is the table empty?
	if (!fsFile.pstSymbols.empty())
	{
		// Calculate the size of the symbol table.
		std::set<SSymbolEntry*, SSymbolNameLess>::iterator it = fsFile.pstSymbols.setName.begin();
		while (it != fsFile.pstSymbols.setName.end())
		{
			// Count the amount of information.
			u_symbol_size += (*it)->uNameLength + sizeof(SSymbolEntry) - sizeof(char *);
			u_symbol_count++;

			// Next symbol.
			it++;
		}
	}

	// Determine the offsets of each of the section's raw data.
	uint u_section_offset = fsFile.fhHeader.uSectionCount * sizeof(SSectionHeader) + sizeof(SFileHeader);

	// Loop through the sections and calculate the offsets.
	uint u_index;
	for (u_index = 0; u_index < fsFile.slSections.size(); u_index++)
	{
		// Calculate the offsets for each section.
		fsFile.slSections[u_index].pshSection->uSectionOffset = u_section_offset;
		u_section_offset                     = fsFile.slSections[u_index].pshSection->uSectionOffset + 
											   fsFile.slSections[u_index].pshSection->uSectionSize;
	}

	// Update the file header structure.
	fsFile.fhHeader.uSymtabOffset	= u_section_offset; 
	fsFile.fhHeader.uSymtabEntries	= u_symbol_count; 
	fsFile.fhHeader.uSymtabSize		= u_symbol_size; 
	fsFile.fhHeader.uFileSize		= u_section_offset + u_symbol_size;

	//
	// Now write the image to disk.
	//

	// Write the file header to disk.
	if (!bWrite(&fsFile.fhHeader, sizeof(SFileHeader)))
	{
		// Unable to write the header for some reason.
		return false;
	}

	// Write the section headers to disk.
	for (u_index = 0; u_index < fsFile.slSections.size(); u_index++)
	{
		// Write out the section header.
		if (!bWrite(fsFile.slSections[u_index].pshSection, (int) sizeof(SSectionHeader)))
		{
			// Unable to write the header for some reason.
			return false;
		}
	}

	// Write out the raw data sections.
	for (u_index = 0; u_index < fsFile.slSections.size(); u_index++)
	{
		// Write out the section header.
		if (!bWrite(fsFile.sbBuffers.Address(fsFile.slSections[u_index].bhHandle), fsFile.slSections[u_index].pshSection->uSectionSize))
		{
			// Unable to write the header for some reason.
			return false;
		}
	}

	// Write out the symbol table.
	if (!fsFile.pstSymbols.empty())
	{
		// Calculate the size of the symbol table.
		std::set<SSymbolEntry*, SSymbolNameLess>::iterator it = fsFile.pstSymbols.setName.begin();
		while (it != fsFile.pstSymbols.setName.end())
		{
			// Write out the symbol entry information.
			if (!bWrite((*it), (sizeof(SSymbolEntry) - sizeof(char *))))
			{
				// Unable to write the header for some reason.
				return false;
			}

			if (!bWrite((*it)->strName, (*it)->uNameLength))
			{
				// Unable to write the header for some reason.
				return false;
			}

			// Next symbol.
			it++;
		}
	}

	// Dump the contents of the image.
	Dump();

	// Return a succesful result.
	return true;
}


//**********************************************************************************************
//
// Implementation of CFileIO::bReadImage.
// 
bool CFileIO::bReadImage(PFNWORLDLOADNOTIFY pfnWorldNotify /* = NULL */, 
                         uint32 ui32_NotifyParam /* = 0 */)
{
	uint u_count;

	// Is this file structure marked closed?
	if (fsFile.bImageOpen)
	{
		//
		// Yes.  This is a problem, because we are about to destroy the current image which
		// exists in memory.  We now need to destroy the existing image and initialize the
		// file structure in preparation to load a new image.
		
		// Attempt to destroy the current image.  Were we succesful?
		if (!bDeleteImage())
		{
			// Something went wrong, so return an error.
			return false;
		}
	}

	// Mark the image as open.
	fsFile.bImageOpen = true;

	// Is the file currently open?
	if (fsFile.omOpenMode == eClosed)
	{
		// We cannot operate on a closed file.
		fsFile.rcErrorCode = eFileClosed;

		// Return an error result.
		return false;
	}

	// Was the file opened for reading?
	if (fsFile.omOpenMode != eRead)
	{
		// We cannot write an image to a file not opened for writing.
		fsFile.rcErrorCode = eInvalidMode;

		// Return an error result.
		return false;
	}

	// Make sure the file pointer is at the start of the file.  Were we successful?
	if (LZSeek(fsFile.fdFile, 0L, 0) < 0)
	{
		// Something went wrong.  Let the error processor figure it out.
		ProcessError();

		// Return an error result.
		return false;
	}

	// Read the file header to disk.
	if (!bRead(&fsFile.fhHeader, sizeof(SFileHeader)))
	{
		// Unable to write the header for some reason.
		return false;
	}

	// Read in each of the section headers.
	for (u_count = 0; u_count < fsFile.fhHeader.uSectionCount; u_count++)
	{
		SSectionList sl_section_list;

		sl_section_list.pshSection = new SSectionHeader;
		sl_section_list.prlReloc = 0;

		// Read in the section header.
		if (!bRead(sl_section_list.pshSection, (int) sizeof(SSectionHeader)))
		{
			// Unable to write the header for some reason.
			return false;
		}

		// Create a buffer for the raw data.
		sl_section_list.bhHandle = fsFile.sbBuffers.bhCreate(sl_section_list.pshSection->uSectionSize);

		// First allocate a node for the section list.
		fsFile.slSections.push_back(sl_section_list);
	}

	if (pfnWorldNotify)
	{
		// Show 1% completion.
		(pfnWorldNotify)(ui32_NotifyParam, 1, 1, 100);
	}

	// Read in the raw data from the sections.
	for (u_count = 0; u_count < fsFile.slSections.size(); u_count++)
	{
		// Write out the section header.
		if ( !bRead(fsFile.sbBuffers.Address(fsFile.slSections[u_count].bhHandle), fsFile.slSections[u_count].pshSection->uSectionSize) )
		{
			// Unable to write the header for some reason.
			return false;
		}

		// Advance the pointer in the file.
		fsFile.sbBuffers.iLast(fsFile.slSections[u_count].bhHandle, fsFile.slSections[u_count].pshSection->uSectionSize - 1);

		if (pfnWorldNotify)
		{
			// Show completion 1%-6%
			int i_percent = 1 + (u_count * 5) / fsFile.slSections.size();

			(pfnWorldNotify)(ui32_NotifyParam, 1, i_percent, 100);
		}
	}

	// Stuff so that we periodically call progress update.
	uint u_step = fsFile.fhHeader.uSymtabEntries / 100;
	uint u_next_count = 0;

	// Read in the symbol table.
	for (u_count = 0; u_count < fsFile.fhHeader.uSymtabEntries; u_count++)
	{
		// First allocate a node for the section list.
		SSymbolEntry* pse_symbol = new SSymbolEntry;

		// Read in the symbol descriptor and length.
		if (!bRead(pse_symbol, (int)(sizeof(SSymbolEntry) - sizeof(char *))))
		{
			// Unable to read the header for some reason.
			return false;
		}

		// Allocate memory for the symbol name.
		pse_symbol->strName = new char[pse_symbol->uNameLength];

		// Read the symbol into memory.
		if (!bRead(pse_symbol->strName, pse_symbol->uNameLength))
		{
			// Unable to read the name for some reason.
			return false;
		}

		// Add the new symbol to the sets.
		fsFile.pstSymbols.insert(pse_symbol);

		if (pfnWorldNotify && u_count >= u_next_count)
		{
			u_next_count = u_count + u_step;

			// Show completion 6%-10%
			int i_percent = 6 + (u_count * 4) / fsFile.fhHeader.uSymtabEntries;

			(pfnWorldNotify)(ui32_NotifyParam, 1, i_percent, 100);
		}
	}

	if (pfnWorldNotify)
	{
		// Show 10% completion.
		(pfnWorldNotify)(ui32_NotifyParam, 1, 10, 100);
	}

	// Return a successful result.
	return true;
}


//**********************************************************************************************
//
// Implementation of CFileIO::bDeleteImage.
// 
bool CFileIO::bDeleteImage()
{
	// Are there any file info structures?
	if (fsFile.fiInfo.siSectionInfo != 0)
	{
		// Deallocation the user file information structures.
		delete [] fsFile.fiInfo.siSectionInfo;
	}

	// Is there a section handle list?
	if (fsFile.fiInfo.sehHandles != 0)
	{
		// Deallocation the user file information structures.
		delete [] fsFile.fiInfo.sehHandles;
	}

	// Initialize the file info structure to NULL for sanity reasons.
	memset(&fsFile.fiInfo, 0, sizeof(SFileInfo));

	// Now delete the section list structures.
	for (uint u_index = 0; u_index < fsFile.slSections.size(); u_index++)
	{
		// ***** Add support for deallocation of the relocation list *****

		// Deallocate the smart buffer associated with the section.
		fsFile.sbBuffers.bDelete(fsFile.slSections[u_index].bhHandle);

		// Delete the section structures.
		delete fsFile.slSections[u_index].pshSection;
	}

	// Loop through the symbol table an delete all the symbol entries.
	std::set<SSymbolEntry*, SSymbolNameLess>::iterator it = fsFile.pstSymbols.setName.begin();
	while (it != fsFile.pstSymbols.setName.end())
	{
		// Delete the symbol name and symbol entry.
		delete [] (*it)->strName;
		delete	  (*it);

		// Next symbol.
		it++;
	}

	// Erase the section list.
	fsFile.slSections.erase(fsFile.slSections.begin(), fsFile.slSections.end());

	// Erase everything in the sets.
	fsFile.pstSymbols.setName.erase(fsFile.pstSymbols.setName.begin(), fsFile.pstSymbols.setName.end());
	fsFile.pstSymbols.setHandle.erase(fsFile.pstSymbols.setHandle.begin(), fsFile.pstSymbols.setHandle.end());

	// Initialize the file structure to contain correct initial values.
	memset(&fsFile.fhHeader, 0, sizeof(SFileHeader));
	memset(&fsFile.fiInfo, 0, sizeof(SFileInfo));
	
	fsFile.rcErrorCode = eSuccess;
	fsFile.bInfoValid  = false;
	fsFile.bImageOpen  = false;

	//Setup the current version number.
	fsFile.fhHeader.uMagicNumber   = MAGIC_NUMBER;
	fsFile.fhHeader.uVersionNumber = VERSION_ID;
	
	// Return a successful result.
	return true;
}

// Create a new section in a file for output.
TSectionHandle CFileIO::sehCreate(const char* str_section_name, uint u_flags)
{
	SSectionList	sl_node;
	SSectionHeader*	sh_section;

	// Is this file structure marked closed?
	if (!fsFile.bImageOpen)
	{
		// Mark the file structure as open.
		fsFile.bImageOpen = true;
	}

	//
	// First setup the new section node, then add it to the list.
	//

	// Setup the information which will not change over the life of this file.
	fsFile.fiInfo.uVersionNumber = VERSION_ID;

	// Construct a section list structure, since one will be needed.
	sl_node.pshSection	= new SSectionHeader;
	sl_node.prlReloc	= 0;

	// Allocate a smart buffer for the raw section data.
	sl_node.bhHandle = fsFile.sbBuffers.bhCreate(64);

	// Setup a pointer to the section header
	sh_section = sl_node.pshSection;

	// Initialize the section header.
	memset(sh_section, 0, sizeof(SSectionHeader));

	// Setup the section structure
	sh_section->syhSectionName	 = syhInsert(str_section_name);
	sh_section->uUserFlags		 = u_flags;
	sh_section->sehSectionHandle = sehToHandle(fsFile.fhHeader.uSectionCount);

	// Add the section to the end of the list.
	fsFile.slSections.push_back(sl_node);

	// Now add the new section node to the list. Is this the first node?
	if (fsFile.slSections.size() == 1)
	{
		// Turn on a flag to indicate that this file now contains at least one section.
		fsFile.fhHeader.uFlags |= uHASSECTIONS;
	}

	// Increment the section counter.
	fsFile.fhHeader.uSectionCount++;

	// Invalidate the user section info structure.
	fsFile.bInfoValid = false;

	// Return a handle to the section.
	return sehToHandle(fsFile.fhHeader.uSectionCount-1);
}
	

//**********************************************************************************************
//
// Implementation of CFileIO::bRemove.
// 
bool CFileIO::bRemove(TSectionHandle seh_handle)
{
	// Has a section list header been allocated yet?
	if (fsFile.slSections.size() == 0)
	{
		// Report the error.
		fsFile.rcErrorCode = eSectionListEmpty;

		// No!  Return an error since we cannot delete a non-existant section.
		return false;
	}

	uint u_index = uToIndex(seh_handle);

	// Is the section handle valid?
	if (u_index >= fsFile.slSections.size())
	{
		// Report the error.
		fsFile.rcErrorCode = eBadHandle;

		// No!  Return an error since we cannot delete a non-existant section.
		return false;
	}

	// Make sure this is the node we want.
	if (seh_handle == fsFile.slSections[u_index].pshSection->sehSectionHandle)
	{
		// Invalidate the user section info structure.
		fsFile.bInfoValid = false;

		//
		// Now deallocate the contents of this node.
		//
		
		// Deallocate the section header.
		delete fsFile.slSections[u_index].pshSection;
		fsFile.slSections[u_index].pshSection = 0;

		// Remove the section from the list (screws up all the handles).
		fsFile.slSections.erase(fsFile.slSections.begin() + u_index);
		
		// Decrement the section count.
 		fsFile.fhHeader.uSectionCount--;

		// ***** Add code here to delete the relocation structures *****

		// Deallocate the buffer associated with the section.
		if (!fsFile.sbBuffers.bDelete(fsFile.slSections[u_index].bhHandle))
		{
			// Report the error
			fsFile.rcErrorCode = eUnableToDelete;

			// Return an error.
			return false;
		}

		// Return a successful result.
		return true;
	}

	// The node was not found in the list, so setup and error.
	fsFile.rcErrorCode = eBadHandle;

	// Return an error result
	return false;
}

	
//**********************************************************************************************
//
// Implementation of CFileIO::bResetSection
// 
bool CFileIO::bResetSection(TSectionHandle seh_handle)
{
	SSectionList* sl_node;

	// Lookup the section list node for this section handle.
	sl_node = slLookup(seh_handle);

	// Were we able to locate the section?
	if (sl_node != 0)
	{
		// Reset the buffers size.
		fsFile.sbBuffers.SetSize(sl_node->bhHandle, 0);

		sl_node->pshSection->uSectionSize = 0;

		return true;
	}

	return false;
}


//**********************************************************************************************
//
// Implementation of CFileIO::iRead.
// 
int CFileIO::iRead(TSectionHandle seh_handle, void* v_buffer, int i_count)
{
	bool		  b_eof;
	bool		  b_result;
	int			  i_bytes_read;	
	SSectionList* sl_node;
	
	// Were we passed a valid buffer?
	if (v_buffer == 0)
	{
		// No!  Log an error.
		fsFile.slLogfile.Printf("Error: iRead invoked with v_buffer == NULL.\n");

		// Was the count non zero?
		if (i_count == 0)
		{
			// Report the error.
			fsFile.rcErrorCode = eNullBuffer;
		}
		else
		{
			// Report the error.
			fsFile.rcErrorCode = eNullBufferNonZero;
		}
	}
	else
	{
		// Lookup the section list node for this section handle.
		sl_node = slLookup(seh_handle);

		// Were we able to locate the section?
		if (sl_node != 0)
		{
			// Attempt to read a number of bytes from the section.
			i_bytes_read = fsFile.sbBuffers.iRead(sl_node->bhHandle, v_buffer, i_count);

			// Were we successful?
			if (i_bytes_read == i_count)
			{
				// Yes! Return the number of bytes that were read,
				return i_count;
			}
			// Possibly a few bytes were read.  Figure out what happened.
			else if (i_bytes_read >= 0)
			{
				// Are we at EOF?
				b_result = bEOF(sl_node->bhHandle, &b_eof);

				if (b_result && b_eof)
				{
					// Yes!  Then we read all the remaining bytes in the buffer.
					return i_bytes_read;
				}

				// No!  Something is really wrong.  Log a message to the file.
				fsFile.slLogfile.Printf("Error in iRead: Section handle: 0x%X, Buffer: 0x%X:, Count: %d, Bytes read: 0x%X\n",
					seh_handle, v_buffer, i_count, i_bytes_read);
			}
			else
			{
				// Something went wrong in the smart buffer manager?
				switch(i_bytes_read)
				{
					case -1:
						fsFile.slLogfile.Printf("Error in iRead: Section handle: 0x%X, Buffer: 0x%X:, Count: %d\n",
							seh_handle, v_buffer, i_count);
						fsFile.slLogfile.Printf("Smart buffer reported a bad buffer handle.  Handle: 0x%X\n", 
							sl_node->bhHandle);

						break;

					default:
						fsFile.slLogfile.Msg("Unsupported error condition in iWrite:\n");
				}
			}
		}
	}

	// No!  Notify the caller something went wrong.
	return -1;
}
	

//**********************************************************************************************
//
// Implementation of CFileIO::iWrite
// 
int CFileIO::iWrite(TSectionHandle seh_handle, void* v_buffer, int i_count)
{
	int			  i_bytes_written;	
	SSectionList* sl_node;
	
	// Were we passed a valid buffer?
	if (v_buffer == 0)
	{
		// Was a non-zero count provided?  This is an error.
		if (i_count > 0)
		{
			// No!  Log an error.
			fsFile.slLogfile.Printf("Warning: CFileIO: iWrite invoked with v_buffer address 0, and count: %d\n", i_count);

			// Report the error
			fsFile.rcErrorCode = eNullBufferNonZero;

			// Return an invalid length.
			return -1;
		}
		else
		{
			// A null buffer with a count of 0 is inefficient, not necessaryily an error.
			return 0;
		}
	}
	else
	{
		// Lookup the section list node for this section handle.
		sl_node = slLookup(seh_handle);

		// Were we able to locate the section?
		if (sl_node != 0)
		{
			// Attempt to write a number of bytes from the section.
			i_bytes_written = fsFile.sbBuffers.iWrite(sl_node->bhHandle, v_buffer, i_count);

			// Were we successful?
			if (i_bytes_written == i_count)
			{
				// Make sure the section size field is correct.
				sl_node->pshSection->uSectionSize = fsFile.sbBuffers.iCount(sl_node->bhHandle);

				// Return the number of bytes written to the section.
				return i_count;
			}
			// Possibly a few bytes were read.  Figure out what happened.
			else if (i_bytes_written >= 0)
			{
				// Something is really wrong.  Log a message to the file.
				fsFile.slLogfile.Printf("Error in iWrite: Section handle: 0x%X, Buffer: 0x%X:, Count: %d, Bytes read: 0x%X\n",
					seh_handle, v_buffer, i_count, i_bytes_written);
				
				// Make sure the section size field is correct
				sl_node->pshSection->uSectionSize = fsFile.sbBuffers.iCount(sl_node->bhHandle);

				// Return an error
				return i_bytes_written;
			}
			else
			{
				// Look for special error codes.
				switch (i_bytes_written)
				{
					case -1:	
						// Bad buffer handle.
						fsFile.slLogfile.Printf("Error in iWrite: Section handle: 0x%X, Buffer: 0x%X:, Count: %d\n",
							seh_handle, v_buffer, i_count);
						fsFile.slLogfile.Printf("Smart buffer reported a bad buffer handle.  Handle: 0x%X\n", 
							sl_node->bhHandle);

						break;
					case -2:
						// Smart buffer corruption
						fsFile.slLogfile.Printf("Error in iWrite: Section handle: 0x%X, Buffer: 0x%X:, Count: %d\n",
							seh_handle, v_buffer, i_count);
						fsFile.slLogfile.Printf("Smart buffer reported internal corruption:\n");

						fsFile.sbBuffers.Dump();

						break;
					default:
						fsFile.slLogfile.Printf("Unsupported error condition in iWrite:\n");
				}
			}
		}
	}


	// No!  Notify the caller something went wrong.
	return -1;
}


//**********************************************************************************************
//
// Implementation of CFileIO::iSeek.
// 
int CFileIO::iSeek(TSectionHandle seh_handle, int i_position)
{
	int			  i_location;	
	SSectionList* sl_node;
	
	// Lookup the section list node for this section handle.
	sl_node = slLookup(seh_handle);

	// Were we able to locate the section?
	if (sl_node != 0)
	{
		// Attempt to change the current file pointer position.
		i_location = fsFile.sbBuffers.iSeek(sl_node->bhHandle, i_position);

		// Were we successful?
		if (i_location == i_position)
		{
			// Yes! Return the location. 
			return i_location;
		}
		else
		{
			// Look for special error codes.
			switch (i_location)
			{
				case -1:	
					// Bad buffer handle.
					fsFile.slLogfile.Printf("Error in iSeek: Section handle: 0x%X, Seek position: %d\n",
						seh_handle, i_position);
					fsFile.slLogfile.Printf("Smart buffer reported a bad buffer handle.  Handle: 0x%X\n", 
						sl_node->bhHandle);

					break;
				default:
					fsFile.slLogfile.Printf("Unsupported error condition in iSeek:\n");
			}
		}
	}

	// No!  Notify the caller something went wrong.
	return -1;
}


//**********************************************************************************************
//
// Implementation of CFileIO::iTell.
// 
int	CFileIO::iTell(TSectionHandle seh_handle)
{
	int			  i_location;	
	SSectionList* sl_node;

	// Lookup the section list node for this section handle.
	sl_node = slLookup(seh_handle);

	// Were we able to locate the section?
	if (sl_node != 0)
	{
		// Attempt to change the current file pointer position.
		i_location = fsFile.sbBuffers.iTell(sl_node->bhHandle);

		// Were we successful?
		if (i_location >= 0)
		{
			// Yes!
			return i_location;
		}
		else
		{
			// Something went wrong.
			switch (i_location)
			{
				case -1:	
					// Bad buffer handle.
					fsFile.slLogfile.Printf("Error in iTell: Section handle: 0x%X, Tell position: %d\n",
						seh_handle, i_location);
					fsFile.slLogfile.Printf("Smart buffer reported a bad buffer handle.  Handle: 0x%X\n",
						sl_node->bhHandle);

					break;
				default:
					fsFile.slLogfile.Printf("Unsupported error condition in iTell:\n");
			}
		}
	}

	// No!  Notify the caller something went wrong.
	return -1;
}


//**********************************************************************************************
//
// Implementation of CFileIO::iCount.
// 
int	CFileIO::iCount(TSectionHandle seh_handle)
{
	int			  i_count;	
	SSectionList* sl_node;
	

	// Lookup the section list node for this section handle.
	sl_node = slLookup(seh_handle);

	// Were we able to locate the section?
	if (sl_node != 0)
	{
		// Attempt to change the current file pointer position.
		i_count = fsFile.sbBuffers.iCount(sl_node->bhHandle);

		// Were we successful?
		if (i_count >= 0)
		{
			// Yes!
			return i_count;
		}
		else
		{
			// Something went wrong.
			switch (i_count)
			{
				case -1:	
					// Bad buffer handle.
					fsFile.slLogfile.Printf("Error in iCount: Section handle: 0x%X, Section size: %d\n",
						seh_handle, i_count);
					fsFile.slLogfile.Printf("Smart buffer reported a bad buffer handle.  Handle: 0x%X\n", 
						sl_node->bhHandle);

					break;
				default:
					fsFile.slLogfile.Printf("Unsupported error condition in iCount:\n");
			}
		}
	}

	// No!  Notify the caller something went wrong.
	return -1;
}


//**********************************************************************************************
//
// Implementation of CFileIO::bEOF.
// 
bool CFileIO::bEOF(TSectionHandle seh_handle, bool *b_eof)
{
	bool		  b_result;	
	SSectionList* sl_node;

	// Lookup the section list node for this section handle.
	sl_node = slLookup(seh_handle);

	// Were we able to locate the section?
	if (sl_node != 0)
	{
		// Attempt to change the current file pointer position.
		b_result = fsFile.sbBuffers.bEOF(sl_node->bhHandle, b_eof);

		// Were we successful?
		if (b_result)
		{
			// Yes! Return a successful result.
			return true;
		}
		else
		{
			// No. Bad buffer handle.
			fsFile.slLogfile.Printf("Error in bEOF: Section handle: 0x%X",	seh_handle);
			fsFile.slLogfile.Printf("Smart buffer reported a bad buffer handle.  Handle: 0x%X", 
				sl_node->bhHandle);
		}
	}

	// No!  // Return an error result.
	return false;
}


//**********************************************************************************************
//
// Implementation of CFileIO::bSetFlags.
// 
bool CFileIO::bSetFlags(TSectionHandle seh_handle, uint u_flags)
{
	SSectionList* sl_node = slLookup(seh_handle);
	
	// Were we able to locate the section?
	if (sl_node != 0)
	{
		// Set the user flags field.
		sl_node->pshSection->uUserFlags = u_flags;

		// Return a successful result.
		return true;
	}

	// Return an error result.
	return false;
}


//**********************************************************************************************
//
// Implementation of CFileIO::bGetFlags.
// 
bool CFileIO::bGetFlags(TSectionHandle seh_handle, uint* u_flags)
{
	SSectionList* sl_node = slLookup(seh_handle);
	
	// Were we able to locate the section?
	if (sl_node != 0)
	{
		// Set the user flags field.
		*u_flags = sl_node->pshSection->uUserFlags;

		// Return a successful result.
		return true;
	}

	// Return an error result.
	return false;
}


//**********************************************************************************************
//
// Implementation of CFileIO::FileInfo.
// 
SFileInfo* CFileIO::FileInfo()
{
	SSectionInfo*	si_info;
	SSectionHeader* sh_section;
	TSectionHandle*	seh_handles;

	// Setup the main structure.
	fsFile.fiInfo.uVersionNumber  = VERSION_ID;
	fsFile.fiInfo.uSectionCount   = fsFile.fhHeader.uSectionCount;

	// Are there any sections in the section list?
	if (fsFile.fhHeader.uSectionCount == 0)
	{
		// Make sure the things are in a sane state.
		fsFile.fiInfo.siSectionInfo = 0;
		fsFile.fiInfo.sehHandles	= 0;
	}
	else
	{
		// Has the morphology of the section list changed?
		if (!fsFile.bInfoValid)
		{
			// Is there a section descriptor array?
			if (fsFile.fiInfo.siSectionInfo != 0)
			{
				// Yes!  Deallocate it since it may be the wrong size.
				delete fsFile.fiInfo.siSectionInfo;
			}
			
			// Setup the new array of the proper size.
			fsFile.fiInfo.siSectionInfo = new SSectionInfo[fsFile.fhHeader.uSectionCount];

			// Is there a section handle array?
			if (fsFile.fiInfo.sehHandles != 0)
			{
				// Yes!  Deallocate it since it may also be the wrong size.
				delete fsFile.fiInfo.sehHandles;
			}
			
			// Setup the new array of the proper size.
			fsFile.fiInfo.sehHandles = new TSectionHandle[fsFile.fhHeader.uSectionCount];
		}

		// Setup the structures.
		si_info    = fsFile.fiInfo.siSectionInfo;
		seh_handles = fsFile.fiInfo.sehHandles;

		// Loop through the section list an update all the information.
		for (uint u_index = 0; u_index < fsFile.slSections.size(); u_index++)
		{
			// Setup a pointer to the section header.
			sh_section = fsFile.slSections[u_index].pshSection;

			// Setup the section info structure first.
			si_info[u_index].syhHandle	   = sh_section->syhSectionName;
			si_info[u_index].uReferenceCount = 0;
			si_info[u_index].uSectionSize    = fsFile.sbBuffers.iCount(fsFile.slSections[u_index].bhHandle);
			si_info[u_index].uFlags		   = fsFile.slSections[u_index].pshSection->uUserFlags;

			// Now setup the section handle.
			seh_handles[u_index] = sh_section->sehSectionHandle;
		}
	}

	// Reset the info valid flag.
	fsFile.bInfoValid = true;

	// Return a pointer to the file information structure.
	return &fsFile.fiInfo;
}


//**********************************************************************************************
//
// Implementation of CFileIO::bValidateImage.
// 
bool CFileIO::bValidateImage()
{
	int				i_exceptions;
	uint			u_section_size;
	uint			u_section_count;
	int				i_file_size = 0;
	SSectionList*	psl_node;


	//
	// Loop through all the structures and make sure all the values are correct.
	// 

	i_exceptions	= 0;
	u_section_count = 0;

	// Start with the sections.
	for (uint u_index = 0; u_index < fsFile.slSections.size(); u_index++)
	{
		psl_node = &fsFile.slSections[u_index];

		// Verify the size of each section.
		u_section_size = fsFile.sbBuffers.iCount(psl_node->bhHandle);

		// Is the section the correct size?
		if (psl_node->pshSection->uSectionSize != u_section_size)
		{
			// Log a message that indicates a section size mismatch.
			fsFile.slLogfile.Printf("Warning: Section(%d) size mismatch. Existing: %d, Current: %d\n",
				u_section_count, psl_node->pshSection->uSectionSize, 
				iCount(psl_node->pshSection->sehSectionHandle));
			
			// Update the section size field.
			psl_node->pshSection->uSectionSize = iCount(psl_node->bhHandle);

			// Increment the exception counter.
			i_exceptions++;
		}

		// Increment the section counter.
		u_section_count++;
	}

	// Make sure the section count is correct.
	if (fsFile.fhHeader.uSectionCount != u_section_count)
	{
		// Log a warning message and update the value.
		fsFile.slLogfile.Printf("Warning: Section count mismatch.  Existing: %d, Current: %d",
			fsFile.fhHeader.uSectionCount, u_section_count);

		// Update the section count in the header.
		fsFile.fhHeader.uSectionCount = u_section_count;

		// Increment the exception counter
		i_exceptions++;
	}

	// There are no errors which can return a fatal type, so return success.
	return true;
}


//**********************************************************************************************
//
// Implementation of CFileIO::Dump.
// 
void CFileIO::Dump()
{
	int i;

	// Validate the internal representation.
	bValidateImage();

	// First off dump the file header.
	fsFile.slLogfile.Msg("----- File Header -----");
	fsFile.slLogfile.Msg("Magic number        ", fsFile.fhHeader.uMagicNumber);
	fsFile.slLogfile.Msg("File size           ", (int) fsFile.fhHeader.uFileSize);
	fsFile.slLogfile.Msg("Number of sections  ", (int) fsFile.fhHeader.uSectionCount);
	fsFile.slLogfile.Msg("Symbol table offset ", fsFile.fhHeader.uSymtabOffset);
	fsFile.slLogfile.Msg("Symbols in table    ", (int) fsFile.fhHeader.uSymtabEntries);
	fsFile.slLogfile.Msg("File flags          ", fsFile.fhHeader.uFlags);
	fsFile.slLogfile.Msg("File version ID     ", (int) fsFile.fhHeader.uVersionNumber);

	// Loop through the symbol table and dump all the symbols.
	fsFile.slLogfile.Msg("----- Symbol table -----");

	i = 0;
	// Is the table empty?
	if (fsFile.pstSymbols.empty())
	{
		// Report this to the log.
		fsFile.slLogfile.Msg("<EMPTY>");
	}
	else
	{
		// Begin dumping the symbol table.
		std::set<SSymbolEntry*, SSymbolNameLess>::iterator it = fsFile.pstSymbols.setName.begin();
		while (it != fsFile.pstSymbols.setName.end())
		{
			// Dump the symbol information to the logfile.
			fsFile.slLogfile.Printf("Symbol(%d):  Reference count: %d, Symbol Handle: 0x%X, Name: \"%s\"\n",
				i++, (*it)->uReferenceCount, (*it)->syhHandle, (*it)->strName);

			// Next symbol.
			it++;
		}
	}

	// Begin dumping the sections.
	fsFile.slLogfile.Printf("----- File Sections -----");

	// Setup a pointer to the head of the list.
	SSectionHeader*	psh_section;	

	// Is the table empty?
	if (fsFile.slSections.size() == 0)
	{
		// Report this to the log.
		fsFile.slLogfile.Printf("\n<EMPTY>\n");
	}
	else
	{
		// Loop through all the sections in the section list.
		for (uint u_index = 0; u_index < fsFile.slSections.size(); u_index++)
		{
			// Setup a pointer to this node.
			SSectionList* psl_node = &fsFile.slSections[u_index];

			// Setup a pointer to the section header.
			psh_section = psl_node->pshSection;

			// Dump the section header.
			fsFile.slLogfile.Printf("\nSection(%d): \"%s\", Size: %d, Flags: 0x%X, Section Handle: 0x%X, Buffer: 0x%X\n\n", 
				u_index, strLookup(psh_section->syhSectionName), 
				iCount(psh_section->sehSectionHandle), psh_section->uSectionFlags,
				psh_section->sehSectionHandle, fsFile.sbBuffers.Address(psl_node->bhHandle));
			
			// Dump the raw data in the buffer.
			fsFile.slLogfile.Buffer("Raw section data:", 
				fsFile.sbBuffers.Address(psl_node->bhHandle),
				iCount(psh_section->sehSectionHandle));
		}
	}

	// Dump the symbol table trailer.
	fsFile.slLogfile.Msg("\n----- File dump complete -----\n");
}