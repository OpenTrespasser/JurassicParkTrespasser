/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CAudioADPCM - ADPCM audio loading and decompression class
 *
 *		This ADPCM class is based on the IMA algorithm as defined and put into the public
 *		domain by Intel.
 *		The documentation on this format can be found at www.ima.org
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
 * $Log:: /JP2_PC/Source/Lib/Audio/AudioADPCM.hpp                                              $
 * 
 * 8     8/21/98 8:17p Rwyatt
 * Destructor is now virtual
 * 
 * 7     8/21/98 8:06p Rwyatt
 * Added decompress block size, the number of bytes to be decompressed on the next step.
 * 
 * 6     8/20/98 3:05p Rwyatt
 * Decompression support for 16bit ADPCM samples.
 * 
 * 5     8/10/98 5:24p Rwyatt
 * Started to add audio compression
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
 * Audio loader for ADPCM data
 * 
 ***********************************************************************************************/

#ifndef HEADER_AUDIO_ADPCM_HPP
#define HEADER_AUDIO_ADPCM_HPP


#include "Lib/Audio/AudioLoader.hpp"


//**********************************************************************************************
//
class CAudioADPCM : public CCAULoad
//
// prefix: adpcm
//	
//**************************************
{
public:
	//******************************************************************************************
	//
	// Construct a ADPCM loader from a file or from an offset from within a file
	//
	CAudioADPCM(SCAUHeader& pcau);

	//******************************************************************************************
	//
	// Construct a ADPCM loader from a file
	//
	CAudioADPCM
	(
		CAudioDatabase*	padat,			// database holding the CAU file or NULL for a Win32 file
		HANDLE			h_file,			// file handle
		uint32			u4_fpos,		// offset of the header in the specified file
		SCAUHeader&		cau				// the header at the above offset
	);

	virtual ~CAudioADPCM();

protected:
	//******************************************************************************************
	//	ADPCM Data members
	//
	uint8*	pu1SourceData;
	uint32	u4SourceDataSize;
	uint8*	pu1BlockBuffer;				// A buffer big enough to hold a single decompressed block
	uint8*	pu1Next;					// pointer to the next bytes in the above buffer
	uint32	u4DecompressedBlockSize;	// The size of the decompressed block
	uint32	u4FreshBytes;				// Number of sample bytes in the decomp buffer
	uint32	u4BlockSize;				// Number of actual bytes in the source buffer

	//******************************************************************************************
	//
	//	Decompression definitions, All audio compression classes must implement these functions
	//
	// Output Mono 8 Bit data, the u4_byte count will be a multiple of 1
	//
	virtual uint32 u4DecompressMono8bit(uint8* pu1_dst, uint32 u4_byte_count);


	//******************************************************************************************
	// Output Mono 16 Bit data, the u4_byte count will be a multiple of 2
	// Returns the number of bytes put into the output buffer
	//
	virtual uint32 u4DecompressMono16bit(uint8* pu1_dst, uint32 u4_byte_count);

	//******************************************************************************************
	// Output Stereo 8 Bit data, the u4_byte count will be a multiple of 2
	// Returns the number of bytes put into the output buffer
	//
	virtual uint32 u4DecompressStereo8bit(uint8* pu1_dst, uint32 u4_byte_count);

	//******************************************************************************************
	// Output Stereo 16 Bit data, the u4_byte count will be a multiple of 4
	// Returns the number of bytes put into the output buffer
	//
	virtual uint32 u4DecompressStereo16bit(uint8* pu1_dst, uint32 u4_byte_count);

	//******************************************************************************************
	// Set the base address and size of the current block that is avaliable for decompressing
	// Returns the number of bytes put into the output buffer
	//
	virtual void SetSampleSource(uint8* pu1_src, uint32 u4_src_len);

	//******************************************************************************************
	// get the number of bytes remaining in the current block, or 0 if none.
	//
	virtual uint32 u4GetRemainingData();
};



#endif