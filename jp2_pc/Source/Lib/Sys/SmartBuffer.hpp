/***********************************************************************************************
 *
 * Copyright (c) 1996 DreamWorks Interactive, 1996
 *
 * Contents:
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/SmartBuffer.hpp                                               $
 * 
 * 6     98.08.21 7:17p Mmouni
 * Added SetSize().
 * 
 * 5     6/19/97 2:53p Mlange
 * Moved Profile.hpp to Lib/Sys.
 * 
 * 4     6/18/97 7:51p Gstull
 * Added  changes for fast exporting.
 * 
 * 2     9/16/96 8:18p Gstull
 * Added project relative path to the include statements.
 * 
 * 1     9/12/96 5:46p Gstull
 *
 **********************************************************************************************/

#ifndef HEADER_COMMON_SMARTBUFFER_HPP
#define HEADER_COMMON_SMARTBUFFER_HPP

#undef min
#undef max

#include <vector>

#ifdef USE_MAX_TYPES
#include "StandardTypes.hpp"
#else
#include "common.hpp"
#endif

#include "Lib/Sys/SysLog.hpp"

#define MINIMUMGRANULARITY 64

typedef uint32 TBufferHandle;


//*********************************************************************************************
//
struct SBuffer
// Prefix: bf 
{
	uint32	uHandle;		// Handle for this buffer
	uint8*	vBuffer;		// Pointer to the buffer
	int		iLast;			// Location of the last user byte accessed
	int		iNext;			// Current file pointer position.
	int		iSize;			// The size of the buffer

};


//*********************************************************************************************
//
class CSmartBuffer
//
// Prefix: sb
//
//
//*********************************************************************************************
{
	CSysLog			slLogfile;
	std::vector<SBuffer>	bfBuffer;

	TBufferHandle	bhToHandle(uint u_index)		  { return (0x40000000 + u_index); };
	uint			uToIndex(TBufferHandle bh_handle) { return (bh_handle - 0x40000000); };

	SBuffer*		bfLookup(TBufferHandle bh_handle);
	int				iCalcSize(int i_amount);

public:
	//*****************************************************************************************
	//
	CSmartBuffer
	(
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	~CSmartBuffer
	(
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Create a new buffer of an initial size.
	//
	TBufferHandle bhCreate
	(
		int i_size
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Delete an existing buffer.
	//
	bool bDelete
	(
		TBufferHandle bh_handle
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Set the size of the buffer.
	// Note: this resets all data in the buffer.
	//
	void SetSize
	(
		TBufferHandle bh_handle,
		int i_size
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Read a number of bytes from the current file pointer position.
	//
	int iRead
	(
		TBufferHandle bh_handle, 
		void*		  v_buffer, 
		int			  i_byte_count
	); 
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Write a number of bytes to the current file pointer position.
	//
	int iWrite
	(
		TBufferHandle bh_handle, 
		void*		  v_buffer, 
		int			  i_byte_count
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Return the current position of the file pointer.
	//
	int iTell
	(
		TBufferHandle bh_handle
	); 
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Move the file pointer to a specified position in the file.
	//
	int iSeek
	(
		TBufferHandle bh_handle, 
		int			  i_position
	); 
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Move the last byte pointer to the specified position.
	//
	int iLast
	(
		TBufferHandle bh_handle,
		int			  i_position
	); 
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Return a boolean indicating if we are at the end of the file.
	//
	bool bEOF
	(
		TBufferHandle bh_handle, 
		bool*		  b_eof
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Return the number of bytes in the current file.
	//
	int iCount
	(
		TBufferHandle bh_handle
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Return the size of the specified buffer.
	//
	int iSize
	(
		TBufferHandle bh_handle
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Return the address of the specified buffer.
	//
	void* Address
	(
		TBufferHandle bh_handle
	);
	//
	//*****************************************************************************************

	//*****************************************************************************************
	//
	// Dump the current data structures and their data to the logfile.
	//
	void Dump
	(
	);	
	//
	//*****************************************************************************************
};

#endif
