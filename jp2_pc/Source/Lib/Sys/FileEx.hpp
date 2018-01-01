/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		File path storage and manipulation.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/FileEx.hpp                                                    $
 * 
 * 5     6/10/98 4:54p Rwyatt
 * Removed compression from ReadImage.
 * Added an option to u4FileSize to allow it to return the decompressed size of a compressed
 * file.
 * 
 * 4     2/18/98 1:01p Rwyatt
 * Added time stamp functions and a CanCreateFile function
 * 
 * 3     12/09/97 2:33p Rwyatt
 * New function called MakeCompressedName
 * 
 * 2     97-04-30 13:08 Speter
 * Added new CPushDir class, for changing and restoring directories; and strStartupPath()
 * function.  Replaces SetPath()/RestoreGamePath().
 * 
 * 1     97/02/05 7:48p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_SYS_FILEEX_HPP
#define HEADER_LIB_SYS_FILEEX_HPP


//*********************************************************************************************
//
class CPushDir
//
// Prefix: pshd
//
// Class which changes directory, then automatically restores it upon destruction.
//
//**************************************
{
private:
	char* strOldDir;

public:
	//*****************************************************************************************
	//
	// Constructor and destructor.
	//

	// Constructor changes directory.
	CPushDir(const char* str_path);

	// Destructor restores it.
	~CPushDir();
};

//*********************************************************************************************
//
const char* strStartupPath();
//
// Returns:
//		The initial path that existed at startup.
//
//**************************************

//*********************************************************************************************
//
bool bFileExists
(
	const char* str_filename	// File name.
);
//
// Returns 'true' if the file exists, otherwise returns false.
//
//**************************************

//*********************************************************************************************
//
uint32 u4FileSize
(
	const char* str_filename,
	bool b_decom = false		// set to true if you want the decompressed size of a compressed file.
);
//
// return 0xffffffff if the file cannot be found or opened.
//
//**************************************

//*********************************************************************************************
//
uint64 u8FileTimeCreated(const char* str_filename);
//
// 64bit time when the file was created. This is compatible with a Windows FILETIME structure..
//
//**************************************

//*********************************************************************************************
//
uint64 u8FileTimeLastAccessed(const char* str_filename);
//
// 64bit time when the file was last accessed. This is compatible with a Windows FILETIME structure..
//
//**************************************

//*********************************************************************************************
//
uint64 u8FileTimeLastWritten(const char* str_filename);
//
// 64bit time when the file was written to. This is compatible with a Windows FILETIME structure..
//
//**************************************


//*********************************************************************************************
//
bool bFileSetTimeStamp(const char* str_filename, uint64 u8_time);
//
// 64bit time when the file was written to. This is compatible with a Windows FILETIME structure..
//
//**************************************


//*********************************************************************************************
//
bool bCanCreateFile(const char* str_filename);
//
// Checks to see if the specified file can be created, if the file already exists this will
// return false.
//
//**************************************


//*********************************************************************************************
//
const char* strLeafName(const char* str_filename);
//
// Returns the filename element of a fully qualified pathname..
//
//**************************************


//*********************************************************************************************
//
void SaveImage
(
	const char* str_filename,	// File name.
	const void* pv_data,		// Data to store.
	uint32      u4_bytes		// Size of data in bytes.
);
//
// Saves the memory image to the file referenced by the filename.
//
//**************************************

//*********************************************************************************************
//
void ReadImage
(
	const char* str_filename,	// File name.
	void*       pv_data,		// Memory location to load to.
	uint32      u4_bytes		// Size of data in bytes.
);
//
// Loads the memory image from the file referenced by the filename.
//
//**************************************



#endif // HEADER_LIB_SYS_FILEEX_HPP
