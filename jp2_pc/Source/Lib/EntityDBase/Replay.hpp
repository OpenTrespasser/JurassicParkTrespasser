/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CReplay
 *
 *		Defines a class which is intended to be global that will support the replay system.
 *		Replays are created by storing selected messages and playing them back, messages are
 *		flushed to disk at the end of each frame so if a crash occurs the file ends on a frame
 *		boundary.
 *
 *		The actual data to save for each message is derived by the message itself in the 
 *		ExtractReplayData member function. See MessageTypes.cpp for more info
 *
 *		All message classes that have a ExtractReplayData function, ie. save data in the replay
 *		file MUST be friends with the replay class defined in this file. This is so the replay
 *		playback code can have access to protected members in the respective classes. This
 *		could also be achived by having a constructor in every message class that allowed all
 *		the message parameters to be passed in but this would take significant code and is not
 *		it for a trivial task. This access is required while recording and playing replays, the
 *		current constructors set the data elements of the class for now, while playing a replay
 *		we need (certain) elements to be how the replay file states and these elements may not
 *		be elements that have access functions. Basically using a friend class was the easy way
 *		to go.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Replay.hpp                                           $
 * 
 * 10    98/04/09 20:34 Speter
 * Added ability to replay with in specified step increments.
 * 
 * 9     98/04/01 11:57 Speter
 * Modified CReplay to handle nested messages in the proper order.
 * 
 * 8     98/03/31 17:08 Speter
 * Updated for Control changes. SReplayChunkControl removed; just use SInput.  Removed support
 * for unused CMessageInput.
 * 
 * 7     5/08/97 4:10p Mlange
 * Added #include for WinAlias.hpp.
 * 
 * 6     4/29/97 1:59p Rwyatt
 * Added elapsed time to replay control structure
 * 
 * 5     4/25/97 7:13p Rwyatt
 * Modified the replay format to reflect the new control message contents.
 * 
 * 4     97-04-24 16:07 Speter
 * Added destructor declaration in CReplay class def.
 * 
 * 3     4/09/97 11:11p Rwyatt
 * Added seperate open/close functions for loading and saving replays. Also took the create
 * replay file code out of the constructor.
 * 
 * 2     4/09/97 11:38a Rwyatt
 * Initial Implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_REPLAY_HPP
#define HEADER_LIB_ENTITYDBASE_REPLAY_HPP


#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/EntityDBase/Message.hpp"
#include "Lib/W95/WinAlias.hpp"
#include "Lib/Sys/Timer.hpp"

//**********************************************************************************************
//
enum EReplayChunkType
//
// enum of chunks allowed in a replay file.
//
// Prefix:	ect
//
// Notes:
//			The replay file is an chunk based file, the ID of the chunks reflect weather the
//			contained data is required for the game to play back the sequence. 
//
//			bits 24-31 is the purpose:
//					0x00		Unused
//					0x01		Essential chunk
//
//			bits 16-24 is unused and should be 0xFF
//			bits 0-15 is chunk number	
//					
//**************************************
{
	ectReplayChunkStep		=	0x01FF0001,
	ectReplayChunkControl	=	0x01FF0002
};


// pack structures to a byte boundard so we know what we are saving out, push the old packing
#pragma pack(push,1)


//**********************************************************************************************
//
struct SReplayFileHeader
//
// replay chunk header structure
//
// Prefix:	rfh
//
// Notes:
//			every chunk requires a header, this is save out directly before one of the chunk
//			structures below.
//
//			at the moment this is not used because the header is not filled until the file is
//			closed, if the game crahses the file will be closed by force when the task exits
//			and Windows tidies up.
//**************************************
{
	uint32				rfh_u4Magic;				// magic number for this type of replay
	uint32				rfh_u4Chunks;				// number of chunks in the file
	uint32				rfh_u4frames;				// number of frames (step messages)
};




//**********************************************************************************************
//
struct SReplayChunkHeader
//
// replay chunk header structure
//
// Prefix:	rch
//
// Notes:
//			every chunk requires a header, this is saved out directly before one of the chunk
//			structures below.
//**************************************
{
	EReplayChunkType	rch_ectChunkID;				// ID of chunk
	uint32				rch_u4Length;				// bytes to skip from this to the next
};



//**********************************************************************************************
//
struct SReplayChunkStep
//
// replay chunk step structure
//
// Prefix:	chs
//
// Notes:
//		Step chunk based on the data contained within a step message (see msgstep.hpp)
//**************************************
{
	float				chs_fFrameTime;				// message specific data
	float				chs_fElapsedTime;
};


#pragma pack(pop)



//**********************************************************************************************
//
class CReplay
//
// Replay class to control the memory and files required. All data written to the replay file
// must be done through this class using replay chunks.
//
// Prefix: crp
//
// Notes:
//
//**************************************
{
public:

	
	//
	// The constructor should be given the filename of the destination replay file.
	//
	CReplay();

	~CReplay();

	// Get the state of the save replay flag
	bool bSaveActive() const
	{
		return crp_bSaveActive;
	}


	// Set the state of the save replay flag
	void SetSave(bool b_state)
	{
		crp_bSaveActive=b_state;
		if (!b_state)
		{
			CloseReplayFile();
		}
	}


	// Get the state of the load replay flag
	bool bLoadActive() const
	{
		return crp_bLoadActive;
	}


	// Set the state of the load replay flag
	void SetLoad(bool b_state)
	{
		crp_bLoadActive=b_state;
		if (!b_state)
		{
			CloseReplayLoadFile();
		}
	}


	//
	// Open/Create the specified replay file for saving to,
	//
	bool bOpenReplay(const char* str_fname);


	//
	// close the current replay files
	//
	void CloseReplayFile();


	//
	// opens the specifed replay file for playing back
	// returns true or false depending on the success
	//
	bool bOpenReadReplay(const char* str_fname);


	//
	// close the replay load file
	//
	void CloseReplayLoadFile();


	//
	// write a chunk to the current replay file
	//
	uint32 ulWriteReplayChunk
	(
		EReplayChunkType	ect_chunk,				// type of the chunk to write
		uint8*				pu1_data,				// data for the chunk
		uint32				u4_data_size			// size of the data
	);

	//*********************************************************************************************
	//
	void RunFrame
	(
		TSec s_step = -1.0				// How long to step through replay.
										// Default is one original frame.
	);
	//
	// Fires off messages for the next frame in the replay file.
	//
	//**********************************

	//*********************************************************************************************
	//
	void PlayMessage
	(
		EReplayChunkType ect,			// Requested message type.
		TSec s_step = -1.0				// Max step time requested; default is original frame.
	);
	//
	// Plays the next message from the replay file, asserting that it is of the requested type.
	//
	//**********************************

protected:
	bool						crp_bSaveActive;		// set to true if we are saving
	bool						crp_bLoadActive;		// set to true if we are loading 
	HANDLE						crp_hReplay;			// Windows handle for the file
	HANDLE						crp_hReadReplay;		// Handle used to play back a replay
	SReplayChunkHeader*			crp_prchChunk;			// Pointer to the last header returned
	TSec						sStepRemaining;			// Leftover step value for slow playback.

	//*********************************************************************************************
	//
	SReplayChunkHeader* prchReadNextReplayChunk();
	//
	// Reads a chunk from the current file and allocate the memory for it, if
	// there is no current reply file then one will be opened and the first chunk read
	// in. The memory returned by this function is allocated and freed by the function
	// and does not need to be freed, the memory used by the final call will be freed
	// by the destructor.
	//
	//**********************************


};


// global replay class declared in Replay.cpp
extern CReplay		crpReplay;


//#ifndef HEADER_LIB_ENTITYDBASE_REPLAY_HPP
#endif

