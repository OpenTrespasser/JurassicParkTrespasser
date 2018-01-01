/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *			CFileMapping, Win32 memory mapped files.
 *
 * Bugs:
 *
 * To do:
 *			Writable memory mapped files.
 *
 * Notes:
 *			All windows types have opaque types so we do not have to include WinInclude from
 *			within this file.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/FileMapping.hpp                                               $
 * 
 * 2     1/30/98 5:50p Rwyatt
 * First tested checkin
 * 
 * 1     1/30/98 5:38p Rwyatt
 * Initial Implementation
 * 
 ***********************************************************************************************/

#ifndef HEADER_LIB_SYS_FILEMAPPING
#define HEADER_LIB_SYS_FILEMAPPING


//**********************************************************************************************
//
class CFileMapping
//
// Prefix: fmap
//
//*************************************
{
public:

	//******************************************************************************************
	//
	CFileMapping
	(
		const char* str_fname,			// name of disk file
		bool b_sequential = false,		// file is accessed sequentiall
		bool b_readonly = true			// read only file
	);
	//
	//*********************************


	//******************************************************************************************
	//
	~CFileMapping
	(
	);
	//
	//*********************************


	//******************************************************************************************
	//
	void* pvGetMappedAddress
	(
	)
	//
	//*********************************
	{
		Assert(hFile);
		Assert(hFileMapping);
		Assert(pvBase);
		return pvBase;
	}


	//******************************************************************************************
	//
	uint32 u4GetMappedLength
	(
	)
	//
	//*********************************
	{
		Assert(hFile);
		Assert(hFileMapping);
		Assert(pvBase);
		return u4Length;
	}

protected:
	//******************************************************************************************
	//
	void*	hFile;					// Handle of file
	void*	hFileMapping;			// Handle of mapping object
	void*	pvBase;					// Base address of file mapping
	uint32	u4Length;				// length of mapped file.
};

#endif //HEADER_LIB_SYS_FILEMAPPING
