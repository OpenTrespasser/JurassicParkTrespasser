/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *			Implementation of CFileMapping, Win32 memory mapped files
 *
 * Bugs:
 *
 * To do:
 *			Writable memory mapped files.
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/FileMapping.cpp                                               $
 * 
 * 2     1/30/98 5:50p Rwyatt
 * First tested checkin
 * 
 * 1     1/30/98 5:38p Rwyatt
 * Initial Implementation
 * 
 ***********************************************************************************************/

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "FileMapping.hpp"


//**********************************************************************************************
//
CFileMapping::CFileMapping
(
	const char* str_fname,			// name of disk file
	bool b_sequential,				// file is accessed sequentially
	bool b_readonly					// read only file
)
//
//*************************************
{
	// we can only do read only files at the moment...
	Assert(b_readonly);
	Assert(str_fname);

	hFile = (void*)CreateFile(str_fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		b_sequential?FILE_FLAG_SEQUENTIAL_SCAN:FILE_FLAG_RANDOM_ACCESS, 0 );

	Assert(hFile != (void*)INVALID_HANDLE_VALUE);

	u4Length = GetFileSize((HANDLE)hFile,NULL);
	
	Assert(u4Length != 0xffffffff);

	hFileMapping = (void*)CreateFileMapping((HANDLE)hFile, NULL, PAGE_READONLY, 0, 0, NULL );

	Assert(hFileMapping);

	pvBase = (void*)MapViewOfFile((HANDLE)hFileMapping, FILE_MAP_READ, 0, 0, 0 );

	Assert(pvBase);
}



//**********************************************************************************************
//
CFileMapping::~CFileMapping
(
)
//
//*************************************
{
	UnmapViewOfFile(pvBase);
	CloseHandle((HANDLE)hFileMapping);
	CloseHandle((HANDLE)hFile);
}

