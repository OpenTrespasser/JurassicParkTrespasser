/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CCAULoad - Audio loading base class
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
 * $Log:: /JP2_PC/Source/Lib/Audio/AudioLoader.hpp                                             $
 * 
 * 16    8/21/98 8:18p Rwyatt
 * CCAULoader (base class of all audio loaders) now has a virtual destructor.
 * 
 * 15    8/19/98 1:42p Rwyatt
 * Fixes to VC6.0 warnings
 * 
 * 14    8/04/98 4:02p Rwyatt
 * New version, CAU headers are now in the database sample map. This enables headers to be
 * obtained with disk activity.
 * File handles are all created with the database so no open file calls are required.
 * 
 * 13    7/06/98 8:22p Rwyatt
 * Added a rest function to reset the audio databses to their initial state
 * 
 * 12    6/24/98 3:24p Rwyatt
 * Version of TPA is now 0x140 and samples can have properties
 * 
 * 11    6/08/98 12:54p Rwyatt
 * Audio databases can now use a static file handle. This can only be used for databses that do
 * not support multiple loads. Use this flag if possible because it is more efficient as the
 * databse file does not have to be re-opened.
 * 
 * 10    5/22/98 7:09p Rwyatt
 * New element in collision structure for min time delay.
 * New version number - Not backwards compatible.
 * Loader no longer loads old versions.
 * 
 * 9     5/19/98 10:15p Rwyatt
 * Moved FileSetPointer to CPP file.
 * Added support for version 0x121 of TPA file, this includes sample play times in the
 * index/indentifier list.
 * Added a member to give the length of a sample based on a Sound Handle.
 * 
 * 8     5/15/98 7:00p Rwyatt
 * New members to access the hash maps of audio databses.
 * 
 * 7     5/07/98 5:45p Rwyatt
 * Incremented audio database version to 0x120. This is so we can have a identifier count in the
 * header.
 * If there is no collision database then the find collision function will return NULL
 * 
 * 6     3/22/98 5:02p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 5     3/20/98 5:57p Agrant
 * temp fix for missing sample crash
 * 
 * 4     3/10/98 2:21p Rwyatt
 * Uses a duplicate file handle for the sample loaders when loading from a pack file. This
 * enables two streams to be read from the pack file at the same time without the file pointers
 * getting messed up.
 * 
 * 3     3/09/98 10:54p Rwyatt
 * New class to handle sound databases.
 * Modified to read from a database file.
 * 
 * 2     2/03/98 2:28p Rwyatt
 * Temp check in.
 * 
 * 1     11/14/97 12:02a Rwyatt
 * Base class for the audio loading system
 * 
 ***********************************************************************************************/

#ifndef HEADER_AUDIO_LOADER_HPP
#define HEADER_AUDIO_LOADER_HPP

#pragma warning(disable:4786)
#include "SoundDefs.hpp"
#include "Subtitle.hpp"
#include "map.h"

#pragma pack(push,1)

//*********************************************************************************************
// Versions:
//		0x100			Audio pack files
//		0x110			Same format audio pack files but with binary collision
//		0x120			New element in the header that states how many identifers are present,
//						now that samples are instanced there may be more identifiers than
//						samples.
//		0x121			Sample file entries for the sample identifiers now each contain an
//						additional field for the play time of the sample.
//		0x130			Not backwards compatible.
//						Added minimum collision delays
//		0x140			Added minimum collision velocity and per sample properties
//		0x150			Added cau header to sample indentifier index
#define u4MINLOAD_AUDIO_VERSION	0x150
#define u4PACKED_AUDIO_VERSION	0x150


//*********************************************************************************************
#define u4DATABASE_FILE_LEN		128


//*********************************************************************************************
struct SPackedAudioHeader
//prefix: pah
{
	uint32	u4Version;				// version of packed audio file
	uint32	u4Samples;				// number of samples in this packed file
	uint32	u4SampleFileOffset;		// offset to the list of sample files
	uint32	u4Collisions;			// number of collisions in this file
	uint32	u4CollisionFileOffset;	// file offset to this collison list
	uint32	u4Identifiers;			// Number of sample identifiers
};


//**********************************************************************************************
//
struct SCAUHeader
// prefix: cauheader
//	
//**************************************
{
	uint32		u4Magic;				// 'ROBW'
	uint32		u4Version;				// version number *100 (ver 1.00 = 100);
	uint32		u4Offset;				// offset from file strt to the sample bits
	uint32		u4BlockAlignment;		// block alignment for decompressing
	uint32		u4DataSize;				// size in bytes of the sample data
	uint32		u4DecompressedSize;		// size when decompressed (may be 1 less than this)
	uint32		u4Frequency;			// frequency in Hz
	uint8		u1Bits;					// bits per sample 
	uint8		u1Channels;				// number of channels
	uint8		u1Compression;			// type of compression. (0=none, 1=adpcm, 2=voice)
	uint8		u1Flags;				// Various flags..				

	// new for version 1.10
	uint32		u4SubtitleOffset;		// Offset to the subtitle data
};


//*********************************************************************************************
// Compression types

struct SSampleFile
// prefix: sf
{
	uint32		u4Hash;
	uint32		u4CAUStart;
	uint32		u4CAULen;
	float		fAttenuation;
	float		fMasterVolume;
	SCAUHeader	cauheaderIndex;
};


//*********************************************************************************************
// This structure contains specific details about a collision. The collision is identified
// by referencing a map. From this structure this is no way to determine what two materials
// this collision represents.
//
struct SAudioCollision
//prefix: col
{
	uint32			u4SampleFlags;
	float			fMinTimeDelay;
	float			fTimeLastUsed;
	TSoundHandle	sndhndSamples[3];
	SSoundTransfer	stTransfer[3];

	//******************************************************************************************
	bool bSlide()
	{
		return ((u4SampleFlags & 0x80) != 0);
	}

	//******************************************************************************************
	bool bCollision()
	{
		return ((u4SampleFlags & 0x03) != 0);
	}

	//******************************************************************************************
	uint32 u4CollisionSamples()
	{
		return (uint32)(u4SampleFlags & 0x03);
	}
};


//*********************************************************************************************
// This is the format of the audio collision as it is within the pack file.
//
struct SFileCollision
// prefix: fcol
{
	uint64			u8Key;
	SAudioCollision	colCollision;
};

#pragma pack(pop)


//**********************************************************************************************
//
#define	AUDIO_LOADER_VERSION			110		// we can only load version <= 1.10 CAU files
#define AUDIO_LOADER_SUBTITLE_VERSION	110		// first file version that had subtitles
//	
//**************************************


//**********************************************************************************************
// This is the maximum amount of memory that can be used for a load buffer when streaming 
// from a disk file. This amount is used if the u4BlockAlignment is 0, which it usually is for
// raw PCM data.
//
#define AUDIO_FILE_BUFFER			12288
//	
//**************************************



//**********************************************************************************************
class CAudioDatabase;


//**********************************************************************************************
//
class CCAULoad
// prefix cau
//	
//**************************************
{
public:
	//
	// Construct a CAULoad class from data that is in memory
	//
	CCAULoad(SCAUHeader& pcau);

	//
	// Construct a CAULoad class from a file handle and memory map it is requested
	//
	CCAULoad
	(
		CAudioDatabase*	padat,			// database holding the CAU file or NULL for a Win32 file
		HANDLE			h_file,			// file handle
		uint32			u4_fpos,		// offset of the header in the specified file
		SCAUHeader&		cau				// the header at the above offset
	);

	virtual ~CCAULoad();

	uint32 u4LoadSampleData(uint8* pu1_dst, uint32 u4_byte_count, bool b_looped);

	CAudioSubtitle* pasubCreateSubtitle();

	void SelectDecoder();

	static CCAULoad* pcauCreateAudioLoader(SCAUHeader& cau);
	static CCAULoad* pcauCreateAudioLoader(char* str_fname);
	static CCAULoad* pcauCreateAudioLoader(CAudioDatabase* padat,TSoundHandle sndhnd);

	uint32	u4Frequency()
	{
		return cauheaderLocal.u4Frequency;
	}

	ESoundChannels	escChannels()
	{
		return (ESoundChannels)cauheaderLocal.u1Channels;
	}

	ESoundBits	esbBits()
	{
		return (ESoundBits)cauheaderLocal.u1Bits;
	}

	uint32	u4BytesPerSample()
	{
		return (uint32)esbBits()/8;
	}

	uint32 u4DecompressedLength()
	{
		return cauheaderLocal.u4DecompressedSize;
	}

	uint32 u4BytesPerSecond()
	{
		return u4Frequency()*((uint32)escChannels())*u4BytesPerSample();
	}

	void ResetToStartPosition();

protected:
	SCAUHeader		cauheaderLocal;				//Header of the loading sample
	HANDLE			hFile;						//file handle
	CAudioDatabase*	padatSource;				//pointer to the database the sample we are loading is in.
	uint8*			pu1Base;	
	uint8*			pu1Samples;
	uint32			u4SampleBlockLength;		//Number of bytes in the load block for this sample
	uint32			u4SampleOffset;				//Current offset into the sample block
	uint8			u1SilentValue;				//Value of the silence (128 for 8bit, 0 for 16bit)
	uint32			u4DataChunkRemain;			//Number of bytes that remain in the data chunk
	uint32			u4CAUFileBase;				//File base position of the CAU file

	// pointer to the decompress function for this sample type
	uint32			(CCAULoad::*u4DecompressBlock)(uint8* pu1_dst, uint32 u4_byte_count);

	// pure virtual members that decompress classes must implement
	virtual uint32 u4DecompressMono8bit(uint8* pu1_dst, uint32 u4_byte_count) = 0;
	virtual uint32 u4DecompressMono16bit(uint8* pu1_dst, uint32 u4_byte_count) = 0;
	virtual uint32 u4DecompressStereo8bit(uint8* pu1_dst, uint32 u4_byte_count) = 0;
	virtual uint32 u4DecompressStereo16bit(uint8* pu1_dst, uint32 u4_byte_count) = 0;
	virtual void SetSampleSource(uint8* pu1_src, uint32 u4_src_len) = 0;
	virtual uint32 u4GetRemainingData() = 0;
};


//**********************************************************************************************
typedef map< uint32, SSampleFile*, less<uint32> >		TFileSampleHash;
// prefix: fsh

//**********************************************************************************************
typedef map< uint64, SAudioCollision*, less<uint64> >	TCollisionHash;
// prefix: ch


//**********************************************************************************************
struct SAudioHandle
// prefix: ah
{
	HANDLE	hFile;
	bool	bInUse;
};


//**********************************************************************************************
//
class CAudioDatabase
// prefix: adat
{
public:
	//******************************************************************************************
	//
	// Constructor and destructor
	//
	CAudioDatabase
	(
		const char* str_filename, 
		uint32		u4_handles = 0		// all share an handle
	);
	~CAudioDatabase();

	//******************************************************************************************
	// Reset the audio database to a just loaded state
	void Reset();

	//******************************************************************************************
	// Add a new sample to the current database
	//
	void AddSample(SSampleFile* psf)
	{
		fshSamples[psf->u4Hash] = psf;
	}

	//******************************************************************************************
	// Add a new sample to the current database
	//
	void AddCollision(uint64 u8_key, SAudioCollision* pcol)
	{
		(*pchCollisions)[u8_key] = pcol;
	}

	//******************************************************************************************
	// Add a new sample to the current database
	//
	SSampleFile* psfFindSample(TSoundHandle sndhnd)
	{
		TFileSampleHash::iterator i;

		i = fshSamples.find(sndhnd);

		if (i == fshSamples.end())
			return NULL;
		else 
			return (*i).second;
	}

	//******************************************************************************************
	SAudioCollision* pcolFindCollision(uint64 u8_key)
	{
		TCollisionHash::iterator i;

		if (pchCollisions == NULL)
			return NULL;

		i = pchCollisions->find(u8_key);
		if (i == pchCollisions->end())
			return NULL;
		else 
			return (*i).second;
	}

	//******************************************************************************************
	TSoundHandle sndhndGetMissingID()
	{
		return sndhndMissing;
	}

	//******************************************************************************************
	TCollisionHash* pchGetCollisions()
	{
		return pchCollisions;
	}

	//******************************************************************************************
	TFileSampleHash* pfshGetSamples()
	{
		return &fshSamples;
	}

	//******************************************************************************************
	HANDLE hDuplicateDatabaseFileHandle();

	//******************************************************************************************
	void CloseDatabaseFile(HANDLE h_file);

	//******************************************************************************************
	float fGetSampleLength(TSoundHandle	sndhnd)
	{
		SSampleFile*	psf = psfFindSample(sndhnd);

		if (psf)
		{
			return (float)psf->cauheaderIndex.u4DecompressedSize 
				/ (float)(psf->cauheaderIndex.u4Frequency*(psf->cauheaderIndex.u1Bits/8)*psf->cauheaderIndex.u1Channels);
		}

		return 0.0f;
	}

protected:
	SPackedAudioHeader		pahHeader;
	TFileSampleHash			fshSamples;
	SSampleFile*			psfIdentifiers;
	TSoundHandle			sndhndMissing;
	TCollisionHash*			pchCollisions;
	SAudioCollision*		acolCollisions;

	HANDLE					hDatabase;			// file handle if sharing or NULL
	uint32					u4HandleCount;		// number of handles in the array below
	SAudioHandle*			aahHandles;			// pointer to an array of handles
};

#endif