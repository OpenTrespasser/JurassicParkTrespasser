/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Repaly.hpp and replay support code
 *
 * To Do:
 *		
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/Replay.cpp                                          $
 * 
 * 17    98/08/28 12:05 Speter
 * Fixed bug in physics-step replay.
 * 
 * 16    98/08/21 18:00 Speter
 * Fixed bug in replay: sStaticStep not set to sStep.
 * 
 * 15    98/04/09 20:34 Speter
 * Added ability to replay with in specified step increments.
 * 
 * 14    98/04/01 11:57 Speter
 * Modified CReplay to handle nested messages in the proper order.
 * 
 * 13    98/03/31 17:09 Speter
 * Updated for Control changes. Upped replay version, for strafe support. SReplayChunkControl
 * removed; just use SInput.  Removed support for unused CMessageInput.
 * 
 * 12    2/03/98 2:25p Rwyatt
 * When opening a file for reading the share always flag is set.
 * 
 * 11    97/12/11 14:20 Speter
 * Fixed replay crash upon termination.
 * 
 * 10    97/12/02 13:14 Speter
 * Added fixes to allow replays to reset and re-replay.
 * 
 * 9     11/12/97 7:17a Shernd
 * Added the ability to use flags to not have output files used
 * 
 * 8     5/30/97 11:14a Agrant
 * LINT tidying
 * 
 * 7     4/29/97 1:55p Rwyatt
 * Inserts the stored elapsed time into re-generated control messages.
 * 
 * 6     4/25/97 7:13p Rwyatt
 * Modified the replay format to reflect the new control message contents.
 * 
 * 5     97-04-24 16:07 Speter
 * Removed (void) from destructor declaration.
 * 
 * 4     4/14/97 1:06p Rwyatt
 * At end of replay we now return to GUIApp debug mode and the load replay is closed. Any
 * replay being saved is left open so if the game is continued new data continues to be
 * recorded.
 * 
 * 3     4/09/97 11:05p Rwyatt
 * Full load and save support and is much more stable
 * 
 * 2     4/09/97 11:37a Rwyatt
 * Added file support
 * 
 * 1     4/09/97 11:25a Rwyatt
 * Initial implementation of file based replay system
 * 
 *********************************************************************************************/

//
// Includes.
//


#include "Lib/W95/WinInclude.hpp"

#include <string.h>
#include "gblinc/common.hpp"
#include "Replay.hpp"

#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgControl.hpp"

#include "Lib/Control/Control.hpp"

// so we can send menu messages to the GUIApp....
#include "GUIApp/resource.h"

//**********************************************************************************************
//
CReplay	crpReplay;
//
// Notes:
//		Global Replay class for the primary replay
//**************************************



// Magic number that idienties a replay file version.
static const uint32 u4_REPLAY_MAGIC = 'GP03';  //lint !e742

extern bool bUseReplayFile;



//*********************************************************************************************
CReplay::CReplay()
{
	// replay is not playing back
	crp_bLoadActive=FALSE;
	crp_bSaveActive=FALSE;
	crp_prchChunk=NULL;
	crp_hReplay=INVALID_HANDLE_VALUE;
	crp_hReadReplay=INVALID_HANDLE_VALUE;
}



//*********************************************************************************************
CReplay::~CReplay()
{
	CloseReplayFile();
	CloseReplayLoadFile();

	crp_hReplay = 0;
	crp_hReadReplay = 0;


	// if there is an outstanding chunk pointer, free it.
	delete crp_prchChunk;
}



//*********************************************************************************************
void CReplay::CloseReplayFile()
{
	// if the opne file is valid then close it...
	if (crp_hReplay!=INVALID_HANDLE_VALUE)
	{
		::CloseHandle(crp_hReplay);
		crp_hReplay=INVALID_HANDLE_VALUE;
	}
	delete crp_prchChunk;
	crp_prchChunk=NULL;
}



//*********************************************************************************************
void CReplay::CloseReplayLoadFile()
{
	// close any active replay read file
	if (crp_hReadReplay!=INVALID_HANDLE_VALUE)
	{
		::CloseHandle(crp_hReadReplay);
		crp_hReadReplay=INVALID_HANDLE_VALUE;
	}
	delete crp_prchChunk;
	crp_prchChunk=NULL;
}



//*********************************************************************************************
//
// Opens a replay for saving and writes the header, the file pointer is ready for savung chunks
// when this function exits.
//
// At the end of the replay the file pointer should be moved back to the start of the file and
// the header written again with valid data, this is not essential to play the replay back.
//
bool CReplay::bOpenReplay(const char* str_fname)
{
	SReplayFileHeader	rfh_header={u4_REPLAY_MAGIC,0,0};
	ulong				ul_count;

	Assert(str_fname!=NULL);

    if (!bUseReplayFile)
    {
        return FALSE;
    }

	crp_hReplay=::CreateFile(str_fname,
						GENERIC_WRITE,
						0,
						NULL,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);

	if (crp_hReplay==INVALID_HANDLE_VALUE)
	{
		Assert(0);			// invalid file handle
		return(FALSE);
	}

	// write the header and assert the number of bytes written out
	::WriteFile(crp_hReplay, &rfh_header, sizeof(SReplayFileHeader), &ul_count, NULL);
	if (ul_count != sizeof(SReplayFileHeader) )
	{
		Assert(0);			// not a replay file

		CloseReplayFile();
		return(FALSE);
	}

	return(TRUE);
}



//*********************************************************************************************
//
// Open a replay file for playback, read the header and verify that this is a replay file.
// this file pointer is position so that the first chunk is ready to be read.
//
bool CReplay::bOpenReadReplay(const char* str_fname)
{
	SReplayFileHeader		rfh_header;
	ulong					ul_count;

	Assert(str_fname!=NULL);

	sStepRemaining = 0.0;

	// open a read only file...
	crp_hReadReplay=::CreateFile(str_fname,
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);

	if (crp_hReadReplay==INVALID_HANDLE_VALUE)
	{
		Assert(0);			// invalid file handle
		return(FALSE);
	}

	// read and verify the replay header
	ReadFile(crp_hReadReplay,&rfh_header,sizeof(SReplayFileHeader),&ul_count,NULL);

	if (rfh_header.rfh_u4Magic!=u4_REPLAY_MAGIC)
	{
		Assert(0);			// not a replay file

		CloseReplayLoadFile();
		return(FALSE);
	}
	// file pointer is now at the first chunk....
	return TRUE;
}



//*********************************************************************************************
//
// Write the specifed data to the file under the chunk type specifed. This function will take
// care of ensuring that any additional data in the chunk header is setup. This enables any
// data to be written to the replay file. Ensure that nay additional chunks follow the number
// convention specified in replay.hpp
//
uint32 CReplay::ulWriteReplayChunk
(
	EReplayChunkType	ect_chunk,				// type of the chunk to write
	uint8*				pu1_data,				// data for the chunk
	uint32				u4_data_size			// size of the data
)
{
	SReplayChunkHeader	rch_header;
	ulong				ul_count;

	// calculate the size of the chunk with the header and fill the header
	rch_header.rch_ectChunkID=ect_chunk;
	rch_header.rch_u4Length=sizeof(SReplayChunkHeader)+u4_data_size;

	// write out the chunk header
	::WriteFile(crp_hReplay,&rch_header,sizeof(SReplayChunkHeader),&ul_count,NULL);

	// write out the chunk data
	::WriteFile(crp_hReplay,pu1_data,u4_data_size,&ul_count,NULL);

	// return the number of bytes in the chunk
	return rch_header.rch_u4Length;
}



//*********************************************************************************************
//
// Gets the next chunk from the replay file and returns it with a header, the data for the
// chunk follows on direclty after the header.
//
// The memory for the returned should not be freed by the caller because this function may 
// reuse or free it for the next message. The memory for the final message is freed in the
// class destructor.
//
// This will return NULL if there is a memory error or the end of the reply file has been
// reached.
//
SReplayChunkHeader* CReplay::prchReadNextReplayChunk()
{
	// free any outstanding message..
	delete crp_prchChunk;
	crp_prchChunk = NULL;

	SReplayChunkHeader	rch_header;
	ulong				ul_count;

	// read the chunk header and allocate memory for the whole chunk (header+data)
	ReadFile(crp_hReadReplay,&rch_header,sizeof(SReplayChunkHeader),&ul_count,NULL);
	if (ul_count<sizeof(SReplayChunkHeader))
	{
		return NULL;
	}

	crp_prchChunk=(SReplayChunkHeader*)new char[rch_header.rch_u4Length];
	if (crp_prchChunk==NULL)
	{
		Assert(0);			// new failed...
		return(NULL);
	}

	// fill in the newly allocated chunk block
	::memcpy(crp_prchChunk,&rch_header,sizeof(SReplayChunkHeader));
	ReadFile(crp_hReadReplay,
			(crp_prchChunk+1),										// byte after header
			rch_header.rch_u4Length-sizeof(SReplayChunkHeader),		// load just data bytes
			&ul_count,
			NULL);

	if (ul_count < rch_header.rch_u4Length-sizeof(SReplayChunkHeader) )
	{
		return NULL;
	}

	return crp_prchChunk;
}


//*********************************************************************************************
void CReplay::RunFrame(TSec s_step)
{
	//
	// RunFrame fires off one frame of messages from the replay file, in the same order as they 
	// are recorded in. It does this by playing the next Step message in the file. All other replayed
	// messages in the frame are played as a result of Step handlers (e.g. Control).
	//
	// After this function you should call CMainLoop::Play to empty any queue
	// that these messages may have created. This should be called with a FALSE parameter to
	// prevent the play function generating a step message.
	//

	if (sStepRemaining)
	{
		// Send a partial-frame step message.
		if (s_step <= 0.0)
			// No step given; play remainder.
			s_step = sStepRemaining;
		else
			SetMin(s_step, sStepRemaining);

		// Create a message for the given step size.
		// This will increment the total time settings by that amount.
		CMessageStep	msgstep(s_step);
		msgstep.Send();

		sStepRemaining -= s_step;
	}
	else
		PlayMessage(ectReplayChunkStep, s_step);
}

//*********************************************************************************************
void CReplay::PlayMessage(EReplayChunkType ect, TSec s_step)
{
	if (sStepRemaining)
		// Don't get messages if we're still working through a step.
		return;

	//
	// get the first chunk of the frame and keep getting them until we get another Step,
	// at this point we are at the next frame so don't send the last step and back the
	// file pointer up so we can send it next time...
	//
	SReplayChunkHeader*	rch_next = prchReadNextReplayChunk();
	if (rch_next==NULL)
	{
		//
		// loading the next chunk has failed, this is either due to a memory
		// error or more probably because we have hit the end of the file.
		// In any case go into debug mode and close the replay load file.
		// the save file stays open and will continue to record if normal play
		// mode is resumed.
		//
		HWND	hwnd=::GetActiveWindow();

		::SendMessage(hwnd, WM_COMMAND, MENU_DEBUG, 0);
		::SendMessage(hwnd, WM_COMMAND, MENU_REPLAYLOAD, 0);
		return;
	}

	AlwaysAssert(rch_next->rch_ectChunkID == ect);

	uint8* pu1_data=(uint8*)(rch_next+1);

	switch (rch_next->rch_ectChunkID)
	{
		// The message from the replay file is a step message.
		case ectReplayChunkStep:
		{
			// Setup the message to be.
			CMessageStep		msgstep(((SReplayChunkStep*)(pu1_data))->chs_fFrameTime);
			msgstep.sTotal = ((SReplayChunkStep*)(pu1_data))->chs_fElapsedTime;
			msgstep.sStaticTotal = ((SReplayChunkStep*)(pu1_data))->chs_fElapsedTime;   //lint !e1705

			if (s_step > 0.0 && s_step < msgstep.sStep)
			{
				// Send step only for partial frame.

				TSec s_remain = msgstep.sStep - s_step;

				// Adjust message values for partial step.
				msgstep.sStep = msgstep.sStaticStep = s_step;
				msgstep.sTotal -= s_remain;
				msgstep.sStaticTotal -= s_remain;

				// Send the message now.
				msgstep.Send();

				// Store our remaining time for this frame.
				// This prevents further messages from being read until all the step time has
				// been played out.
				sStepRemaining = s_remain;
			}
			else
				// Send the message now.
				msgstep.Send();

			break;
		}

		// the message from the replay file is a control message
		case ectReplayChunkControl:
		{
			SInput				input = *(const SInput*)pu1_data;
			CMessageControl		msgcon(input);
			msgcon.Send();
			break;
		}

		default:
			//
			// this message is flagged to be a essential message but I don't know what
			// it is, so go to the debugger. It is safe to continue from here but this
			// chunk will be lost
			//
			Assert(0);
			break;
	}
}

//*********************************************************************************************
