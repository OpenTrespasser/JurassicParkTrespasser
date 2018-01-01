/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of FileEx.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/W95/FileEx.cpp                                                $
 * 
 * 14    6/12/98 2:34p Rwyatt
 * bfileExists now finds files even if they are open by other applications as writeable.
 * 
 * 13    6/10/98 4:54p Rwyatt
 * Removed compression from ReadImage.
 * Added an option to u4FileSize to allow it to return the decompressed size of a compressed
 * file.
 * 
 * 12    2/18/98 1:01p Rwyatt
 * Added time stamp functions and a CanCreateFile function
 * 
 * 11    2/02/98 3:05p Agrant
 * Open raster files shared read only, so that we don't collide on the network.
 * 
 * 10    1/30/98 7:20p Rwyatt
 * bFileExists now opens file as shared in case it is already open.
 * 
 * 9     12/09/97 2:30p Rwyatt
 * Compressed files are now is a sub directory call Compressed, this directroy should be in the
 * same directory that contains the uncompressed file. A function called MakeCompressedName has
 * been provided to convert from an uncompressed to a compressed name.
 * 
 * 8     12/09/97 1:06p Rwyatt
 * temp check in to fix compression on NT
 * 
 * 7     12/08/97 6:38p Rwyatt
 * ReadImage can now read compressed files.
 * 
 * 6     6/14/97 3:54p Agrant
 * The function calls inside those asserts are necessary.
 * Restructured so we can merely assert without losing
 * functionality.
 * 
 * 5     6/15/97 1:49a Bbell
 * Changed always verifies to asserts.
 * 
 * 4     6/12/97 3:53p Agrant
 * Allow a null path "", replacing it with current dir "."
 * 
 * 3     97-04-30 13:08 Speter
 * Added new CPushDir class, for changing and restoring directories; and strStartupPath()
 * function.  Replaces SetPath()/RestoreGamePath().
 * 
 * 2     97/02/05 7:47p Pkeet
 * Made include relative.
 * 
 * 1     97/02/05 7:36p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

//
// Includes.
//
#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "../FileEx.hpp"
#include <memory.h>
#include <direct.h>


//*********************************************************************************************
//
// class CPushDir implementation.
//

	const int iPATH_SIZE = _MAX_DRIVE + _MAX_DIR;

	//*****************************************************************************************
	CPushDir::CPushDir(const char* str_path)
	{
		// Remember current dir.
		strOldDir = new char[iPATH_SIZE + 1];
		bool b_success = _getcwd(strOldDir, iPATH_SIZE) != 0;
		Assert(b_success);

		// Allow a null directory.
		if (str_path[0] == '\0')
			str_path = ".";

		// Barf if we can't change to the given directory.
		b_success = _chdir(str_path) == 0;
		Assert(b_success);
	}

	//*****************************************************************************************
	CPushDir::~CPushDir()
	{
		Assert(strOldDir);
		bool b_success = _chdir(strOldDir) == 0;
		Assert(b_success);
		delete[] strOldDir;
	}

static char strTheStartupPath[iPATH_SIZE];

//*********************************************************************************************
//
class CInitStartupPath
//
// Class to initialise strTheStartupPath.
//
//**************************************
{
public:
	CInitStartupPath()
	{
		// Make sure we weren't already set.
		if (!*strTheStartupPath)
		{
			bool b_success = _getcwd(strTheStartupPath, iPATH_SIZE) != 0;
			Assert(b_success);
		}
	}
};

static CInitStartupPath initStartupPath;

//*********************************************************************************************
const char* strStartupPath()
{
	if (!*strTheStartupPath)
		// Somehow, we were called before the static initialising object was initialised.
		// Invoke the initialising constructor to set the path.
		CInitStartupPath();
	return strTheStartupPath;
}



//*********************************************************************************************
uint64 u8FileTimeCreated(const char* str_filename)
{
	Assert(str_filename);

	// Open a windows file handle.
	HANDLE hfile = CreateFile
	(
		(LPCTSTR)str_filename,	// Pointer to name of the file.
		GENERIC_READ,			// Access (read-write) mode.
		FILE_SHARE_READ,		// Share mode.
		0,						// Pointer to security descriptor.
		OPEN_EXISTING,			// How to create.
		FILE_ATTRIBUTE_NORMAL,	// File attributes.
		0 						// Handle to file with attributes to copy.
	);

	// If the function fails, it is because no file exists.
	if (hfile == INVALID_HANDLE_VALUE)
		return 0;


	uint64 u8_filetime;

	if (!GetFileTime(hfile, (LPFILETIME)&u8_filetime,NULL,NULL))
	{
		u8_filetime = 0;
	}


	CloseHandle(hfile);

	return u8_filetime;
}


//*********************************************************************************************
uint64 u8FileTimeLastAccessed(const char* str_filename)
{
	Assert(str_filename);

	// Open a windows file handle.
	HANDLE hfile = CreateFile
	(
		(LPCTSTR)str_filename,	// Pointer to name of the file.
		GENERIC_READ,			// Access (read-write) mode.
		FILE_SHARE_READ,		// Share mode.
		0,						// Pointer to security descriptor.
		OPEN_EXISTING,			// How to create.
		FILE_ATTRIBUTE_NORMAL,	// File attributes.
		0 						// Handle to file with attributes to copy.
	);

	// If the function fails, it is because no file exists.
	if (hfile == INVALID_HANDLE_VALUE)
		return 0;

	uint64 u8_filetime;

	if (!GetFileTime(hfile, NULL,(LPFILETIME)&u8_filetime,NULL))
	{
		u8_filetime = 0;
	}

	CloseHandle(hfile);

	return u8_filetime;
}


//*********************************************************************************************
uint64 u8FileTimeLastWritten(const char* str_filename)
{
	Assert(str_filename);

	// Open a windows file handle.
	HANDLE hfile = CreateFile
	(
		(LPCTSTR)str_filename,	// Pointer to name of the file.
		GENERIC_READ,			// Access (read-write) mode.
		FILE_SHARE_READ,		// Share mode.
		0,						// Pointer to security descriptor.
		OPEN_EXISTING,			// How to create.
		FILE_ATTRIBUTE_NORMAL,	// File attributes.
		0 						// Handle to file with attributes to copy.
	);

	// If the function fails, it is because no file exists.
	if (hfile == INVALID_HANDLE_VALUE)
		return 0;

	uint64 u8_filetime;

	if (!GetFileTime(hfile, NULL,NULL,(LPFILETIME)&u8_filetime))
	{
		u8_filetime = 0;
	}

	CloseHandle(hfile);

	return u8_filetime;
}


//*********************************************************************************************
bool bFileSetTimeStamp(const char* str_filename, uint64 u8_time)
{
	Assert(str_filename);
	Assert(u8_time);

	// Open a windows file handle.
	HANDLE hfile = CreateFile
	(
		(LPCTSTR)str_filename,	// Pointer to name of the file.
		GENERIC_WRITE,			// Access (read-write) mode.
		FILE_SHARE_WRITE,		// Share mode.
		0,						// Pointer to security descriptor.
		OPEN_EXISTING,			// How to create.
		FILE_ATTRIBUTE_NORMAL,	// File attributes.
		0 						// Handle to file with attributes to copy.
	);

	if (hfile == INVALID_HANDLE_VALUE)
		return false;

	bool b_res = true;

	if (!SetFileTime(hfile, (LPFILETIME)&u8_time, (LPFILETIME)&u8_time, (LPFILETIME)&u8_time))
	{
		b_res = false;
	}

	CloseHandle(hfile);

	return b_res;
}


//*********************************************************************************************
const char* strLeafName(const char* str_filename)
{
	const char* str_end = str_filename + strlen(str_filename);
	const char* str_begin = str_end;

	// back up along the string.
	while (str_begin > str_filename && *str_begin != '/' && *str_begin != '\\' &&
		*str_begin != ':')
	{
		--str_begin;
	}

	// if we are on one of the path control characters, go forward 1.
	if (*str_begin == '/' || *str_begin == '\\' || *str_begin == ':')
		++str_begin;

	Assert(str_begin < str_end);

	return str_begin;
}


//*********************************************************************************************
bool bFileExists(const char* str_filename)
{
	Assert(str_filename);

	// Open a windows file handle.
	HANDLE hfile = CreateFile
	(
		(LPCTSTR)str_filename,					// Pointer to name of the file.
		GENERIC_READ|GENERIC_WRITE,				// Access (read-write) mode.
		FILE_SHARE_READ|FILE_SHARE_WRITE,		// Share mode.
		0,										// Pointer to security descriptor.
		OPEN_EXISTING,							// How to create.
		FILE_ATTRIBUTE_NORMAL,					// File attributes.
		0 										// Handle to file with attributes to copy.
	);

	// If the function fails, it may be because we asked for write access. Lets try again
	// with just read access. We must ask for write access in case the file is already open 
	// with share write set.
	if (hfile == INVALID_HANDLE_VALUE)
	{
		hfile = CreateFile
		(
			(LPCTSTR)str_filename,					// Pointer to name of the file.
			GENERIC_READ,							// Access (read only) mode.
			FILE_SHARE_READ,						// Share mode.
			0,										// Pointer to security descriptor.
			OPEN_EXISTING,							// How to create.
			FILE_ATTRIBUTE_NORMAL,					// File attributes.
			0 										// Handle to file with attributes to copy.
		);

		if (hfile == INVALID_HANDLE_VALUE)
		{
			// Open the file as read only failed
			return false;
		}
	}

	// Close the open file.
	Verify(CloseHandle(hfile));

	// The function succeeded, therefore the file exists.
	return true;
}


//*********************************************************************************************
// This will create a file under the specified name as a temporary file which gets deleted
// immediately. If the specified file exists then this function will return false.
//
bool bCanCreateFile(const char* str_filename)
{
	Assert(str_filename);
	// Open a windows file handle.
	HANDLE hfile = CreateFile
	(
		(LPCTSTR)str_filename,	// Pointer to name of the file.
		GENERIC_WRITE,			// Access (read-write) mode.
		0,						// Share mode.
		0,						// Pointer to security descriptor.
		CREATE_NEW,				// How to create.
		FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,	// File attributes.
		0 						// Handle to file with attributes to copy.
	);

	if (hfile == INVALID_HANDLE_VALUE)
		return false;

	CloseHandle(hfile);
	return true;
}


//*********************************************************************************************
uint32 u4FileSize(const char* str_filename, bool b_decom)
{
	Assert(str_filename);

	if (b_decom)
	{
		int			i_handle;
		OFSTRUCT	ofs;

		// We want the decompressed size so we need to use the LZ file api
		i_handle = LZOpenFile((char*)str_filename, &ofs, OF_READ);

		// failed to open the file as a compressed file
		if (i_handle<0)
			return 0xffffffff;

		uint32 u4_size = (uint32)LZSeek(i_handle,0,2);
		LZClose(i_handle);

		return u4_size;
	}
	else
	{
		// Open a windows file handle.
		HANDLE hfile = CreateFile
		(
			(LPCTSTR)str_filename,	// Pointer to name of the file.
			GENERIC_READ,			// Access (read-write) mode.
			FILE_SHARE_READ,		// Share mode.
			0,						// Pointer to security descriptor.
			OPEN_EXISTING,			// How to create.
			FILE_ATTRIBUTE_NORMAL,	// File attributes.
			0 						// Handle to file with attributes to copy.
		);

		// If the function fails, it is because no file exists.
		if (hfile == INVALID_HANDLE_VALUE)
			return 0xffffffff;

		uint32 u4_file_size = GetFileSize(hfile, NULL);

		// Close the open file.
		Verify(CloseHandle(hfile));

		// return the size of the file.
		return u4_file_size;
	}
}


//*********************************************************************************************
void SaveImage(const char* str_filename, const void* pv_data, uint32 u4_bytes)
{
	Assert(pv_data);
	Assert(u4_bytes > 0);
	Assert(str_filename);

	// Open a windows file handle.
	HANDLE hfile = CreateFile
	(
		(LPCTSTR)str_filename,	// Pointer to name of the file.
		GENERIC_WRITE,			// Access (read-write) mode.
		0,						// Share mode.
		0,						// Pointer to security descriptor.
		OPEN_ALWAYS,			// How to create.
		FILE_ATTRIBUTE_NORMAL,	// File attributes.
		0 						// Handle to file with attributes to copy.
	);

	Verify(hfile);
	Verify(hfile != INVALID_HANDLE_VALUE);

	uint32 u4_num_bytes_written;	// Number of bytes written to the file.

	// Write to the file.
	Verify(WriteFile
	(
		hfile,							// Handle to file to write to.
		(LPCVOID)pv_data,				// Pointer to data to write to file.
		u4_bytes,						// Number of bytes to write.
		(LPDWORD)&u4_num_bytes_written,	// Pointer to number of bytes written.
		(LPOVERLAPPED)0 				// Pointer to structure needed for overlapped I/O.
	));

	Verify(u4_bytes == u4_num_bytes_written);

	// Close the open file.
	Verify(CloseHandle(hfile));
}

//*********************************************************************************************
void ReadImage(const char* str_filename, void* pv_data, uint32 u4_bytes)
{
	Assert(pv_data);
	Assert(u4_bytes > 0);
	Assert(str_filename);

	//
	// Win32 Read
	//
	HANDLE hfile = CreateFile
	(
		(LPCTSTR)str_filename,	// Pointer to name of the file.
		GENERIC_READ,			// Access (read-write) mode.
		FILE_SHARE_READ,		// SHARE mode.
		0,						// Pointer to security descriptor.
		OPEN_EXISTING,			// How to create.
		FILE_ATTRIBUTE_NORMAL,	// File attributes.
		0 						// Handle to file with attributes to copy.
	);

	Verify(hfile);
	Verify(hfile != INVALID_HANDLE_VALUE);

	uint32 u4_num_bytes_read;	// Number of bytes read from the file.


	// Read in file to memory.
	Verify(ReadFile
	(
		hfile,							// Handle of file to read.
		(LPVOID)pv_data,				// Address of buffer that receives data.
		u4_bytes,						// Number of bytes to read.
		(LPDWORD)&u4_num_bytes_read,	// Address of number of bytes read.
		(LPOVERLAPPED)0 				// Address of structure for data.
	));

	Verify(u4_bytes == u4_num_bytes_read);

	// Close the open file.
	Verify(CloseHandle(hfile));
}

/*
//*********************************************************************************************
TCHAR* tstrGetCurrentDirectory()
{
	const uint32 u4_temp_buffer_size = 2048;
	uint32       u4_perm_buffer_size;
	TCHAR        tstr_path_temp_buffer[u4_temp_buffer_size];
	TCHAR*       tstr_path_perm_buffer;

	//
	// Get the path of the executable upon initialization.
	//
	GetCurrentDirectory
	(
		u4_temp_buffer_size,	// Size, in characters, of directory buffer.
		tstr_path_temp_buffer	// Address of buffer for current directory.
	);

	//
	// Get only the necessary size of string for the path.
	//
	for (u4_perm_buffer_size = 0; u4_perm_buffer_size < u4_temp_buffer_size;
		 u4_perm_buffer_size++)
	{
		// Search for the last (null) character.
		if (tstr_path_temp_buffer[u4_perm_buffer_size] == 0)
			break;
	}

	// Add one to the permanent buffer size.
	u4_perm_buffer_size++;

	// Create a new string.
	tstr_path_perm_buffer = new TCHAR[u4_perm_buffer_size];

	// Copy the path.
	if (u4_perm_buffer_size >= 2)
	{
		memcpy
		(
			(void*)tstr_path_perm_buffer,
			(const void*)tstr_path_temp_buffer,
			sizeof(TCHAR) * (u4_perm_buffer_size - 1)
		);
	}

	// Add the null termination character.
	tstr_path_perm_buffer[u4_perm_buffer_size - 1] = 0;

	// Return the new string.
	return tstr_path_perm_buffer;
}
*/