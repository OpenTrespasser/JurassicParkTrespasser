/**********************************************************************************************
 *
 * Copyright (c) DreamWorks Interactive. 1996
 *
 * Inplementation of module <Module name>:
 * 
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Sys/SmartBuffer.cpp                                              $
 * 
 * 10    98.08.21 7:18p Mmouni
 * Added SetSize().
 * 
 * 9     6/18/97 7:51p Gstull
 * Added  changes for fast exporting.
 *
 *********************************************************************************************/

#include <memory.h>

#ifdef USE_MAX_TYPES
#include "StandardTypes.hpp"
#include "Tools/GroffExp/GUIInterface.hpp"

extern CGUIInterface guiInterface;

#else
#include "Common.hpp"
#endif

#include "Lib/Sys/SysLog.hpp"
#include "Lib/Sys/SmartBuffer.hpp"

CSmartBuffer::CSmartBuffer()
{

#ifdef USE_MAX_TYPES
	// Open the log file.
	char str_logfile[256];

	// Determine whether the file should be active or not.
	if (guiInterface.bGenerateLogfiles())
	{
		// Construct the proper path for the logfile.
		guiInterface.BuildPath(str_logfile, guiInterface.strGetLogfileDirPath(), "SmartBuffer.log");
		slLogfile.Open(str_logfile);

		// Activate the logfile.
		slLogfile.Enable();
	}
	else
	{
		// Deactivate the logfile.
		slLogfile.Disable();
	}
#else
	slLogfile.Disable();
#endif
}


CSmartBuffer::~CSmartBuffer()
{
	uint u_i;

	// Loop through the structures and print out the nodes and the contents
	for (u_i = 0; u_i < bfBuffer.size(); u_i++)
	{
		// Delete all of the dynamic components.
		delete [] bfBuffer[u_i].vBuffer;
	}

	// CLose the log file
	slLogfile.Close();
}


int CSmartBuffer::iCalcSize(int i_amount)
{
	// For buffers which need to be grown, allocate a block which is double 
	// the current size upto a 4k page, then add additional pages as needed.
	if (i_amount <= 64)
		return 64;
	else if (i_amount <= 128)
		return 128;
	else if (i_amount <= 256)
		return 256;
	else if (i_amount <= 512)
		return 512;
	else if (i_amount <= 1024)
		return 1024;
	else if (i_amount <= 2048)
		return 2048;
	else if (i_amount <= 4096)
		return 4096;
	else if (i_amount <= 8192)
		return 8192;
	else
		return ((i_amount/8192)+1)*8192;
}


TBufferHandle CSmartBuffer::bhCreate(int i_size)
{
	int			  i_buffersize;
	uint		  u_index = bfBuffer.size();
	TBufferHandle bh_handle = bhToHandle(u_index);
	
	// Figure out how big of a block to allocate
	i_buffersize = iCalcSize(i_size);

	// Now setup the buffer node.
	SBuffer bf_buffer = { bh_handle, new uint8[i_buffersize], -1, 0, i_buffersize };

	// Were we able to allocate the buffer?
	if (bf_buffer.vBuffer == 0)
	{
		// Return an invalid buffer handle.
		return 0;
	}

	// Add the node to the list.
	bfBuffer.push_back(bf_buffer);

	// If the buffer is valid then clear it.
	// memset(bf_buffer->vBuffer, 0, i_buffersize);

	// Return a handle to the node
	return bh_handle;
}


// Success means the requested handle is not in the list.
bool CSmartBuffer::bDelete(TBufferHandle bh_handle)
{
	uint u_end   = bfBuffer.size();
	uint u_index = uToIndex(bh_handle);
	
	// Is this a valid handle?
	if (u_index >= u_end)
	{
		// No, return success.
		return false;
	}

	// Is this handle in the list?
	if (bfBuffer[u_index].vBuffer != 0)
	{
		delete [] bfBuffer[u_index].vBuffer;
		bfBuffer[u_index].vBuffer = 0;
	}

	// The buffer was not found so return an error
	return false;
}


SBuffer* CSmartBuffer::bfLookup(TBufferHandle bh_handle)
{
	uint u_end   = bfBuffer.size();
	uint u_index = uToIndex(bh_handle);
	
	// Is the list empty?
	if (u_end == 0)
	{
		// The list is already empty.
		return 0;
	}

	// Is this a valid handle?
	if (u_index >= u_end)
	{
		// No, return and invalid buffer address.
		return 0;
	}

	// Return whatever is in the list.
	return &bfBuffer[u_index];
}


void CSmartBuffer::SetSize(TBufferHandle bh_handle, int i_size)
{
	SBuffer* bf_buffer = bfLookup(bh_handle);

	// Did we find a buffer?
	if (bf_buffer == 0)
	{
		return;
	}

	// Determine the new buffer size
	int i_buffersize = iCalcSize(i_size);

	// Allocate the new buffer and initialize it.
	uint8* u1_new_buffer = new uint8[i_buffersize];

	// Were we successful?
	if (u1_new_buffer == 0)
	{
		return;
	}

	// Update the buffer node information
	bf_buffer->iSize = i_buffersize;
	bf_buffer->iLast = 0;
	bf_buffer->iSize = 0;
	bf_buffer->iNext = 0;

	// Deallocate the old buffer
	delete[] bf_buffer->vBuffer;

	// Setup the new buffer in the buffer node.
	bf_buffer->vBuffer = u1_new_buffer;

}


int	CSmartBuffer::iRead(TBufferHandle bh_handle, void* v_buffer, int i_byte_count)
{
	SBuffer* bf_buffer = bfLookup(bh_handle);

	// Did we find a buffer?
	if (bf_buffer == 0)
	{
		// No! Return an error
		return -1;
	}

	// Does the user wish to read past the end of the file?
	if (bf_buffer->iNext > bf_buffer->iLast)
	{
		// Yes!  Return 0 bytes read.  Don't touch the current file pointer.
		return 0;
	}

	// Is there any data in the buffer?
	if (bf_buffer->iLast == -1)
	{
		// No! Return since the buffer is empty.
		return 0;
	}

	// Are we at EOF?
	if (bf_buffer->iNext == bf_buffer->iLast+1)
	{
		// Yes!  Return 0 bytes read.
		return 0;
	}

	// Is the end in range?
	int i_last = (bf_buffer->iNext + i_byte_count) - 1;
	if (i_last > bf_buffer->iLast)
	{
		// Adjust the count into range
		i_byte_count = bf_buffer->iLast - bf_buffer->iNext;
	}

	// Read the contents of the buffer
	memcpy(v_buffer, &bf_buffer->vBuffer[bf_buffer->iNext], i_byte_count);

	// Update the file position pointer
	bf_buffer->iNext += i_byte_count;

	// Return the number of bytes read
	return i_byte_count;
}


int	CSmartBuffer::iWrite(TBufferHandle bh_handle, void* v_buffer, int i_byte_count)
{
	int		 i_last;
	SBuffer* bf_buffer = bfLookup(bh_handle);

	// Did we find a buffer?
	if (bf_buffer == 0)
	{
		// No! Return an error
		return -1;
	}

	// Is the buffer corrupted or invalid?
	if (bf_buffer->iLast > bf_buffer->iSize)
	{
		return -2;
	}

	// Determine the offset into the array of the last byte to be written.
	i_last = (bf_buffer->iNext + i_byte_count) - 1;

	// Is this within the range of the buffer?
	if (i_last > (bf_buffer->iSize-1))
	{
		// Determine the new buffer size
		int i_buffersize = iCalcSize(i_last+1);

		// Allocate the new buffer and initialize it.
		uint8* u1_new_buffer = new uint8[i_buffersize];

		// Were we successful?
		if (u1_new_buffer == 0)
		{
			// No!  Return a 'Can't grow the buffer' error.
			return -3;
		}

		// Clear the buffer.
		// memset(u1_new_buffer, 0, i_buffersize);

		// Copy the contents of the old buffer into the new buffer
		memcpy(u1_new_buffer, bf_buffer->vBuffer, bf_buffer->iLast+1);

		// Update the buffer node information
		bf_buffer->iSize = i_buffersize;

		// Deallocate the old buffer
		delete[] bf_buffer->vBuffer;

		// Setup the new buffer in the buffer node.
		bf_buffer->vBuffer = u1_new_buffer;
	}

	// Write the data into the buffer
	memcpy(&bf_buffer->vBuffer[bf_buffer->iNext], (uint8 *) v_buffer, i_byte_count);

	// Is this a new high water mark in the buffer?
	if (i_last > bf_buffer->iLast)
	{
		// Yes!  Then update the buffer content marker.
		bf_buffer->iLast = i_last;
	}

	// Update the current file pointer.
	bf_buffer->iNext += i_byte_count;


	// Return the number of bytes written to the buffer.
	return i_byte_count;
}


int	CSmartBuffer::iTell(TBufferHandle bh_handle)
{
	SBuffer* bf_buffer = bfLookup(bh_handle);

	// Did we find a buffer?
	if (bf_buffer == 0)
	{
		// No! Return an error
		return -1;
	}

	// Return the current file pointer.
	return bf_buffer->iNext;
}


int	CSmartBuffer::iSeek(TBufferHandle bh_handle, int i_position)
{
	SBuffer* bf_buffer = bfLookup(bh_handle);

	// Did we find a buffer?
	if (bf_buffer == 0)
	{
		// No! Return an error
		return -1;
	}

	// Setup the current file position.
	bf_buffer->iNext = i_position;

	// Return the offset of the last character written
	return i_position;
}


int	CSmartBuffer::iLast(TBufferHandle bh_handle, int i_position)
{
	SBuffer* bf_buffer = bfLookup(bh_handle);

	// Did we find a buffer?
	if (bf_buffer == 0)
	{
		// No! Return an error
		return -1;
	}

	// Is the last position within range?
	if (i_position >= bf_buffer->iSize)
	{
		// No! return an error.
		return -1;
	}

	// Setup the current file position.
	bf_buffer->iLast = i_position;

	// Return the offset of the last character written
	return i_position;
}


int	CSmartBuffer::iCount(TBufferHandle bh_handle)
{
	SBuffer* bf_buffer = bfLookup(bh_handle);

	// Did we find a buffer?
	if (bf_buffer == 0)
	{
		// No! Return an error
		return -1;
	}


	// Return the offset of the last character written
	return bf_buffer->iLast+1;
}


bool CSmartBuffer::bEOF(TBufferHandle bh_handle, bool* b_eof)
{
	SBuffer* bf_buffer = bfLookup(bh_handle);

	// Did we find a buffer?
	if (bf_buffer == 0)
	{
		// No! Return an error
		return false;
	}

	// Return the offset of the last character written
	*b_eof = bf_buffer->iNext > bf_buffer->iLast;


	// Return a successful result.
	return true;
}


int	CSmartBuffer::iSize(TBufferHandle bh_handle)
{
	SBuffer* bf_buffer = bfLookup(bh_handle);

	// Did we find a buffer?
	if (bf_buffer == 0)
	{
		// No! Return an error
		return -1;
	}


	// Return the size of the buffer.
	return bf_buffer->iSize;
}


void* CSmartBuffer::Address(TBufferHandle bh_handle)
{
	SBuffer* bf_buffer = bfLookup(bh_handle);

	// Did we find a buffer?
	if (bf_buffer == 0)
	{
		// No! Return an error
		return 0;
	}


	// Return the address of the buffer.
	return bf_buffer->vBuffer;
}

void CSmartBuffer::Dump()
{
	uint u_i;

	// Add a header to the file.
	slLogfile.Printf("----- Start of SmartBuffer dump -----\n");

	// Is the list empty?
	if (bfBuffer.end() - bfBuffer.begin() == 0)
	{
		slLogfile.Printf("\n <EMPTY>\n");
	}
	else
	{
		// Loop through the structures and print out the nodes and the contents
		for (u_i = 0; u_i < bfBuffer.size(); u_i++)
		{
			// Display a header and the contents of the node
			slLogfile.Printf("\nNode: %d, Handle: 0x%X\n", u_i, bfBuffer[u_i].uHandle);
			slLogfile.Printf("Handle: 0x%X\n", bfBuffer[u_i].uHandle );
			slLogfile.Printf("Buffer: 0x%X\n", bfBuffer[u_i].vBuffer );
			slLogfile.Printf("Last  : %d\n",   bfBuffer[u_i].iLast );
			slLogfile.Printf("Next  : %d\n",   bfBuffer[u_i].iNext );
			slLogfile.Printf("Size  : %d\n\n", bfBuffer[u_i].iSize );

			// Dump the contents of the buffer
			slLogfile.Buffer("Buffer contents:", bfBuffer[u_i].vBuffer, bfBuffer[u_i].iLast+1);
		}
	}

	// Add a header to the file.
	slLogfile.Printf("\n----- End of dump -----\n");
}
