/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CAudioPCM - PCM audio loading class
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *		The code contained within this file should be 100% thread safe, this is because
 *		streamed audio will be loaded from within a thread.
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/AudioPCM.hpp                                                $
 * 
 * 4     8/04/98 3:58p Rwyatt
 * New constructors for loaders for samples created from databases as the cau header is in the
 * database index so does not need to be loaded
 * 
 * 3     6/08/98 12:56p Rwyatt
 * The database of a sample is now passed all the way down to the loader. This enables the
 * loader to close the file correctly for databases with static file handles.
 * 
 * 2     3/09/98 10:51p Rwyatt
 * Uses the new constructor of the base class CCAULoad. This is to enable support of handled
 * based sounds.
 * 
 * 1     11/14/97 12:01a Rwyatt
 * Loader for PCM audio data
 * 
 ***********************************************************************************************/

#ifndef HEADER_AUDIO_PCM_HPP
#define HEADER_AUDIO_PCM_HPP


#include "Lib/Audio/AudioLoader.hpp"


//**********************************************************************************************
//
class CAudioPCM : public CCAULoad
//
// prefix: pcm
//	
//**************************************
{
public:
	//******************************************************************************************
	//
	// Construct a PCM loader from data that is in memory or a memory mapped file
	//
	CAudioPCM(SCAUHeader& pcau) : CCAULoad(pcau)
	{
		u4SourceDataSize	= 0;
		pu1SourceData		= NULL;
	}

	//******************************************************************************************
	//
	// Construct a PCM loader from a file  or from an offset from within a file
	//
	CAudioPCM
	(
		CAudioDatabase*	padat,			// database holding the CAU file or NULL for a Win32 file
		HANDLE			h_file,			// file handle
		uint32			u4_fpos,		// offset of the header in the specified file
		SCAUHeader&		cau				// the header at the above offset
	)
	: CCAULoad(padat, h_file, u4_fpos, cau)
	{
		u4SourceDataSize	= 0;
		pu1SourceData		= NULL;
	}


protected:
	//******************************************************************************************
	//	PCM Data members
	//
	uint8*	pu1SourceData;
	uint32	u4SourceDataSize;

	//******************************************************************************************
	//
	//	Decompression definitions, All audio compression classes must implement these functions
	//
	// Output Mono 8 Bit data, the u4_byte count will be a multiple of 1
	//
	virtual uint32 u4DecompressMono8bit(uint8* pu1_dst, uint32 u4_byte_count) override
	{
		uint32	u4_bytes;

		// if we have enough data in the buffer, copy it otherwise copy what is left.
		u4_bytes = (u4SourceDataSize>=u4_byte_count)?u4_byte_count:u4SourceDataSize;

		memcpy(pu1_dst, pu1SourceData, u4_bytes);

		pu1SourceData+=u4_bytes;
		u4SourceDataSize-=u4_bytes;

		return u4_bytes;
	}

	//******************************************************************************************
	// Output Mono 16 Bit data, the u4_byte count will be a multiple of 2
	// Returns the number of bytes put into the output buffer
	//
	virtual uint32 u4DecompressMono16bit(uint8* pu1_dst, uint32 u4_byte_count) override
	{
		uint32	u4_bytes;

		// if we have enough data in the buffer, copy it otherwise copy what is left.
		u4_bytes = (u4SourceDataSize>=u4_byte_count)?u4_byte_count:u4SourceDataSize;

		memcpy(pu1_dst, pu1SourceData, u4_bytes);

		pu1SourceData+=u4_bytes;
		u4SourceDataSize-=u4_bytes;

		return u4_bytes;
	}

	//******************************************************************************************
	// Output Stereo 8 Bit data, the u4_byte count will be a multiple of 2
	// Returns the number of bytes put into the output buffer
	//
	virtual uint32 u4DecompressStereo8bit(uint8* pu1_dst, uint32 u4_byte_count) override
	{
		uint32	u4_bytes;

		// if we have enough data in the buffer, copy it otherwise copy what is left.
		u4_bytes = (u4SourceDataSize>=u4_byte_count)?u4_byte_count:u4SourceDataSize;

		memcpy(pu1_dst, pu1SourceData, u4_bytes);

		pu1SourceData+=u4_bytes;
		u4SourceDataSize-=u4_bytes;

		return u4_bytes;
	}

	//******************************************************************************************
	// Output Stereo 16 Bit data, the u4_byte count will be a multiple of 4
	// Returns the number of bytes put into the output buffer
	//
	virtual uint32 u4DecompressStereo16bit(uint8* pu1_dst, uint32 u4_byte_count) override
	{
		uint32	u4_bytes;

		// if we have enough data in the buffer, copy it otherwise copy what is left.
		u4_bytes = (u4SourceDataSize>=u4_byte_count)?u4_byte_count:u4SourceDataSize;

		memcpy(pu1_dst, pu1SourceData, u4_bytes);

		pu1SourceData+=u4_bytes;
		u4SourceDataSize-=u4_bytes;

		return u4_bytes;
	}

	//******************************************************************************************
	// Set the base address and size of the current block that is avaliable for decompressing
	// Returns the number of bytes put into the output buffer
	//
	virtual void SetSampleSource(uint8* pu1_src, uint32 u4_src_len) override
	{
		pu1SourceData		= pu1_src;
		u4SourceDataSize	= u4_src_len;
	}

	//******************************************************************************************
	// get the number of bytes remaining in the current block, or 0 if none.
	//
	virtual uint32 u4GetRemainingData() override
	{
		return u4SourceDataSize;
	}
};



#endif