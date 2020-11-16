/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	CAudio
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/Audio.cpp                                                   $
 * 
 * 96    10/06/98 2:40p Rwyatt
 * Thread control of the no audio thread is the same as the audio thread.
 * 
 * 95    10/02/98 3:53a Rwyatt
 * Pseudo sample array is zeroed in cleanup()
 * 
 * 94    9/30/98 2:10a Rwyatt
 * Added logic to prevent semaphore being taken twice
 * 
 * 93    9/29/98 1:06a Rwyatt
 * Added a warning when audio is about to break up.
 * 
 * 92    9/28/98 4:59a Rwyatt
 * All audio is now defer loaded
 * 
 * 91    9/23/98 6:35p Ksherr
 * GetVolume() and SetVolume() now return or set respectively a linear number from 0 to 100.
 * 
 * 90    9/23/98 12:09p Rwyatt
 * Added support for NULL samples while audio is enabled
 * 
 * 89    9/21/98 3:32a Rwyatt
 * Loader thread cano now be paused. All clenup code has been modifed to handle this.
 * 
 * 88    9/15/98 8:40p Shernd
 * audio volume changes
 * 
 * 87    9/14/98 12:10a Rwyatt
 * Adjusted thread block/wait code
 * 
 * 86    9/12/98 10:33p Rwyatt
 * TEMP: All lock delays are set to INFINITE time
 * 
 * 85    9/08/98 4:09a Rwyatt
 * Dynamically load either A3D.dll or DSound.dll. If A3D.dll is loaded set the resource
 * management to dynamic as this prevents the A3D crash!.
 * 2D buffers are mixed by software to enable more 3D buffers to be mixed by the hardware,
 * 
 * 84    9/04/98 2:58a Rwyatt
 * New compile flag to control A3D builds.
 * 2D Samples are now not forced to hardware due to typo!
 * timeSetEvent is no longer used for the thread callback because on certain machines the
 * windows called never returned. Normal threads are now used with sleep periods to emulate a
 * periodic timer
 * 
 * 83    9/01/98 3:02p Rwyatt
 * Active stream array ia now an array of structures and not sample pointers
 * 
 * 82    8/25/98 11:26a Rvande
 * Loop variables mis-scoped
 * 
 * 81    8/23/98 4:30p Rwyatt
 * with no audio made sure all variables are properly initialized.
 * 
 * 80    8/23/98 2:23a Rwyatt
 * Added support thread when audio is disabled. This thread allows sub titles to be created in
 * the background without blocking. Construction and destruction of samples has been changed to
 * give a data flow flow to that when audio is enabled.
 * 
 * 79    8/21/98 6:00p Rwyatt
 * Create subtitle sfrom the stream thread
 * 
 * 78    8/21/98 2:25a Rwyatt
 * Fixed streaming cutoff
 * The cleanup code now checks that audio is present.
 * 
 * 77    8/20/98 3:04p Rwyatt
 * Streams are now flagged to be stopped when they finish streaming.
 * 
 * 76    8/10/98 5:24p Rwyatt
 * Initial EAX settings now have no reverb
 * 
 * 75    8/04/98 3:57p Rwyatt
 * Support to set all EAX environment properties
 * 
 * 74    7/29/98 10:50a Rwyatt
 * Added initial EAX support
 * 
 * 73    6/08/98 12:55p Rwyatt
 * Added some debug info to help find the A3D crash, currently commented out.
 * 
 * 72    5/24/98 10:00p Rwyatt
 * Hardware is now properly detected through DirectSound. Creation flags have been modifed to
 * better used available hardware channels.
 * 
 * 71    5/23/98 9:18p Rwyatt
 * Removed all traces of the load library code.
 * 
 * 70    5/22/98 3:11a Rwyatt
 * A3D.DLL is no longer used.
 * DirectSound is no longer dynamically loaded, the hardware caps are derived from the DS caps.
 * 
 * 69    5/06/98 3:34p Rwyatt
 * Either DSound.dll or a3d.dll are dynamically loaded. This means that we do not have to
 * install the a3d.dll on the end users machine.
 * 
 * 68    4/29/98 7:00p Rwyatt
 * CAudio destructor now waits for the thread to exit properly. This should fix the deadlock
 * that some people have been having. Changed the fade sample member function so that it takes a
 * stop flag. The threaded fader now sets a request stop flag instead of stopping the sample.
 * 
 * 67    3/22/98 5:02p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 66    3/09/98 10:50p Rwyatt
 * New sample construction for handle based sounds
 * 
 * 65    2/06/98 8:20p Rwyatt
 * CAudio quality settings are static so they survive the world database being destroyed.
 * 
 * 64    1/09/98 7:04p Rwyatt
 * Fixed an order dependency with the fader thread and the code that clears out the fade array.
 * 
 * 63    1/07/98 5:19p Shernd
 * Added SetVolume to Primary Sound Buffer
 * 
 * 62    12/17/97 2:54p Rwyatt
 * Removed the debug new
 * 
 * 61    11/26/97 2:04p Rwyatt
 * If we get a NULL loader, due to an invalid filename do not create an empty sample
 * 
 * 60    11/22/97 10:43p Rwyatt
 * Uses new global audio pointer which is a static member of the CAudio class, therefore there
 * can be only one.
 * Deferred loading with maximum of 100K chunks is implemented
 * 
 * 59    11/18/97 9:41p Rwyatt
 * Temp checkin:
 * Added callback function for defered loading
 * 
 * 58    11/14/97 7:18p Rwyatt
 * Memory log counters
 * 
 * 57    11/13/97 11:47p Rwyatt
 * Moved sample class to its own file.
 * Initial implementation fo the new audio system. This system also supports the A3D hardware
 * and hence uses A3DCreate rather than Directsound create. The hardware support is transparent
 * after creation.
 * The audio system now has 1 thread that controls all streaming, fading and will also perform
 * the deferred loading. Streamed samples are no different too static samples and all of the
 * same operations can be performed on them,
 * The audio system is now fully 3D, with attached and directional sounds.
 * 
 * 56    10/23/97 6:03p Rwyatt
 * New functions to control speaker configuration
 * 
 * 55    10/03/97 10:21p Rwyatt
 * changed header files so the audio library can be used in other tools without requiring the
 * rest of the source base.
 * 
 * 54    8/19/97 6:33p Rwyatt
 * Fixed a bug again with the 3DListener. The pDS3Dlistener is now checked before it is used, in
 * certain cases audio can be created when 3D fails.
 * 
 * 53    8/19/97 6:16p Rwyatt
 * When audio init fails it now sets lpDSListener to NULL and disables audio. The previous
 * version did not set the lpDSListener to NULL so a GPF was generated at program close when the
 * interface was freed.
 * 
 * 52    97/07/24 11:20a Pkeet
 * Added include.
 * 
 * 51    7/23/97 8:35p Rwyatt
 * New function for finding a sample within the cahce
 * Added memory logging for sample memory
 * 
 * 50    7/14/97 12:27p Rwyatt
 * New audio library with a generic set of flags for creating any sample type.
 * 
 * 49    7/07/97 11:54p Rwyatt
 * Initial tidy up after E3 crunch, sample class still needs to be done
 * 
 * 48    6/25/97 8:28p Mlange
 * Now uses the (fast) float to int conversion functions. Added missing #include for Common.hpp.
 * 
 * 47    6/16/97 2:51a Rwyatt
 * Made the hash string function non fast call because the compiler was really screwing up when
 * in-lining it.
 * 
 * 46    6/13/97 6:07p Rwyatt
 * Samples are duplicated (by reloading) when a second request is processed while the required
 * sample is playing.
 * 
 * 45    6/12/97 6:55p Rwyatt
 * Audio is now enabled.
 * Load sample has better error checking
 * 
 * 44    6/11/97 9:21p Rwyatt
 * Audio can be disabled without any GPFs.
 * AUDIO IS HARD CODED TO OFF IN THIS VERSION
 * 
 * 43    6/09/97 12:07p Rwyatt
 * New CSample type for streaming audio, audio streams are protetced by a semaphore to stop them
 * crashing on exit.
 * 
 * 42    6/06/97 12:10p Rwyatt
 * Ambient sound support. This includes some new sample members for looping and stopping and an
 * active ambient list which is used when the listeners position is chnaged.
 * 
 * 41    6/04/97 7:17p Rwyatt
 * Load sample does not cause a localize calculation anymore.
 * 
 * 40    6/04/97 7:01p Rwyatt
 * Added memeber functions to CAudio for controlling the output format
 * 
 * 39    6/02/97 4:53p Rwyatt
 * Added Many things:
 * DirectSound LostBuffer support.
 * Support for system with no audio.
 * Relative path names, for samples.
 * 
 * 38    5/29/97 6:21p Rwyatt
 * 
 * 37    5/29/97 4:16p Rwyatt
 * DirectSound audio support.
 * 
 * 36    5/25/97 8:31p Rwyatt
 * Removed all of Brandon's RSX code and renamed AudioDeemone to AudioDaemon.
 * 
 ***********************************************************************************************/

// DO NOT INCLUDE COMMON.HPP IN THIS FILE BECAUSE IT IS USED EXTERN TO THE GUIAPP (IN ALL THE
// AUDIO TOOLS) AND I DO NOT WANT TO INCLUDE HALF OF THE PROJECT.
//#include "Common.hpp"
#include "Audio.hpp"
#include <math.h>
#include "AudioLoader.hpp"

#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include "Lib/Sys/DWSizeStruct.hpp"
#include "Eax.h"			// Creative environmental audio



//**********************************************************************************************
// setup debug new handler so we get the file and line number of leaks
//
/*#ifdef _DEBUG
void* __cdecl operator new(size_t nSize, const char* lpszFileName, int nLine);
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#endif*/
//	
//**************************************


//**********************************************************************************************
//
// CAudio Implementation
//

// could not get INIT_GUID macro to work because the project inclused DXGUID.lib in multiple places.
// Therefore duplicate symbols where found and the app failed to link, to combat this I have explictly
// included the EAX GUIDs.
static GUID DSPROPSETID_EAX = {0x4a4e6fc1, 0xc341, 0x11d1, 0xb7, 0x3a, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
static GUID DSQUERY_A3D = {0xd8f1eee1, 0xf634, 0x11cf, 0x87, 0x0, 0x0, 0xa0, 0x24, 0x5d, 0x91, 0x8b};


// global class pointer
CAudio*	CAudio::pcaAudio = NULL;


//**********************************************************************************************
// Initial quality settings....
//
uint32			CAudio::u4Frequency = 22050;
ESoundChannels	CAudio::escChannels = escSTEREO;
ESoundBits		CAudio::esbSampleBits = esb16BIT;



//**********************************************************************************************
//
// This will create the main direct sound interface and attach it to the main window, this class
// will still be created correctly even if no sound is present. This simplifies the work that
// the game is required to do.
// This will always look for a hardware card and will default to DirectSound if one is not used.
//
CAudio::CAudio
(
	void*			h_wnd,
	bool			b_enable,		// if this is false then audio is disabled for good.
	bool			b_hardware		// should we use 3d audio hardware if present
)
//	
//**************************************
{
	// There can be only one.....
	Assert (CAudio::pcaAudio == NULL);

	MEMLOG_ADD_COUNTER(emlSoundControl,sizeof(CAudio));

	bDeferredGoing = false;
	u4MaxDeferredBytes = 100*1024;	
	psamDeferred = NULL;
	ahSemaphore[0] = NULL;
	ahSemaphore[1] = NULL;
	pDSInterface = NULL;
	pDSPrimary = NULL;
	pDSEAX = NULL;
	pDSEAX3D = NULL;
	pA3D = NULL;
	hLibrary = NULL;
	pDSPropertySet = NULL;
	hWnd = (HWND)h_wnd;
	bSoundEnabled = false;
	bCanStream = false;
	bChangeFormat = false;
	bThreadLocked = false;
	strBasePath = "";		// no base path to start with
	bMasterEnable = b_enable;

	// set the listener to be at the origin
	fListenerX = 0.0f;
	fListenerY = 0.0f;
	fListenerZ = 0.0f;

	// Create some critical sections
	InitializeCriticalSection(&csStream);
	InitializeCriticalSection(&csFade);
	InitializeCriticalSection(&csDefer);

	// this constructor will always search for a hardware card
	CreateAudio(h_wnd, b_enable);

	UseHardware(b_hardware);		// set the hardware state if it is present
	CAudio::pcaAudio = this;
}



//**********************************************************************************************
//
CAudio::~CAudio
(
)
//	
//**************************************
{
	MEMLOG_SUB_COUNTER(emlSoundControl,sizeof(CAudio));
	DestroyAudio(true);

	DeleteCriticalSection(&csStream);
	DeleteCriticalSection(&csFade);
	DeleteCriticalSection(&csDefer);

	CAudio::pcaAudio = NULL;
}



//**********************************************************************************************
// Create the support thread that will load all streamed samples.
//
bool CAudio::bCreateStreamSupport
(
)
//	
//**************************************
{
	int32	i4;

	// empty the stream list
	for (i4 = 0;i4<MAX_CONCURRENT_STREAMS;i4++)
	{
		assamStreams[i4].psam = NULL;
	}

	// empty the fade list
	for (i4 = 0; i4<MAX_FADING_SAMPLES; i4++)
	{
		afFadingSamples[i4].psam = NULL;
	}

	// empty the deferred list
	for (i4 = 0;i4<MAX_DEFERRED_OPS;i4++)
	{
		apsamDeferred[i4] = NULL;
	}

	u4StreamsGoing = 0;

	// create a semaphore so we do not delete the streamed samples while they are being filled.
	// By default the semaphore is taken so the thread is not running.
	ahSemaphore[0] = CreateSemaphore(NULL,0,1,"__TRESPASSER_AUDIO_STREAM_LOADER_0__");
	if (ahSemaphore[0] == NULL)
		return false;

	bThreadLocked = true;

	// second semaphore is not taken
	ahSemaphore[1] = CreateSemaphore(NULL,1,1,"__TRESPASSER_AUDIO_STREAM_LOADER_1__");
	if (ahSemaphore[1] == NULL)
	{
		CloseHandle(ahSemaphore[0]);
		ahSemaphore[0] = NULL;
		return false;
	}

	bThreadExit = false;
	hThread = CreateThread(NULL,0,HandleStreamStub,this,0,(DWORD*)&u4ThreadID);

	if (hThread == 0)
	{
		// if we failed to create the thread remember to close the semaphore
		CloseHandle(ahSemaphore[0]);
		ahSemaphore[0] = NULL;
		CloseHandle(ahSemaphore[1]);
		ahSemaphore[1] = NULL;
		return false;
	}

	dprintf("Thread Create: CAudio [Thread handle 0x%x]\n", u4ThreadID);

	return true;
}




//**********************************************************************************************
// This closes the streaming system and deletes any streamed samples if audio is enabled, if
// there is no audio this basicially closes down the thread and tidies things up.
//
void CAudio::RemoveStreams
(
)
//	
//**************************************
{
	// Force the thread loop to exit, this may take upto 1 sleep period.
	bThreadExit = true;

	// Wait for the thread to become signalled indicating that it has terminated.
	// wait upto 1 second, if it does not exit something is wrong so lets take action.
	if (WaitForSingleObject(hThread,1000) != WAIT_OBJECT_0 )
	{
		// The thread has not terminated yet so lets kill it.
		TerminateThread(hThread,0);
	}

	// close the thread handle
	CloseHandle(hThread);
	hThread = NULL;

	// close the semaphore
	CloseHandle(ahSemaphore[0]);
	ahSemaphore[0] = NULL;
	CloseHandle(ahSemaphore[1]);
	ahSemaphore[1] = NULL;

	bCanStream = false;
}




//**********************************************************************************************
// Function to add samples as streams to the active stream list so the  stream handler will
// service them.
// We should not have to take the semaphore for this operation because we only set one pointer
// which will me a monotonic operation.
//
bool CAudio::bAddStreamedSample
(
	CSample* psam
)
//	
//**************************************
{
	int32	i;
	int32	i4_slot = -1;

	// make sure we are not one of the special values.
	Assert( !bSampleError(this));

	// we must be a streamed sample to be added to the stream list
	Assert(psam->u4CreateFlags & AU_CREATE_STREAM);

	EnterCriticalSection(&csStream);

	// first pass is to go through the active streams and ensure that we are not already playing.
	// if we are just return and continue. While doing this keep track of unused entries in case
	// we have to add
	for (i = 0;i<MAX_CONCURRENT_STREAMS;i++)
	{
		if (assamStreams[i].psam == psam)
		{
			LeaveCriticalSection(&csStream);
			return true;
		}

		if (assamStreams[i].psam == NULL) 
		{
			i4_slot = i;
		}
	}

	// if we found an empty slot then use it
	if (i4_slot>=0)
	{
		assamStreams[i4_slot].psam = psam;
		LeaveCriticalSection(&csStream);
		return true;
	}

	LeaveCriticalSection(&csStream);

	// out of stream entries...Increase MAX_CONCURRENT_STREAMS
	AlwaysAssert(0);
	return false;
}



//**********************************************************************************************
// Remove a given streamed sample from the streamer
// This must use the semaphore because when this function returns the DS sample interface will
// get deleted which is not good if the thread is currently loading data into it.
//
void CAudio::RemoveStreamedSample
(
	CSample* psam
)
//	
//**************************************
{
	// make sure we are not one of the special values.
	Assert( !bSampleError(this));

	// Wit for the thread to finish with the streams
	EnterCriticalSection(&csStream);

	// delete the samples, this will remove the CAU loaders as well
	for (int32 i4 = 0;i4<MAX_CONCURRENT_STREAMS;i4++)
	{
		if (assamStreams[i4].psam == psam)
		{
			assamStreams[i4].psam = NULL;
		}
	}

	// free up the semaphore so the threaded streamer can continue
	LeaveCriticalSection(&csStream);
}




//**********************************************************************************************
// This is called by the timer thread at the specified frequency... THIS MUST BE STATIC as it is
// called by windows which knows nothing about the 'this' pointer
//
DWORD _stdcall CAudio::HandleStreamStub
(
	void*	pv_user
)
//	
//**************************************
{
	CAudio*		pca = (CAudio*)pv_user;
	return pca->u4HandleStream();
}


//**********************************************************************************************
uint32 CAudio::u4HandleStream
(
)
//	
//**************************************
{
	CSample*	psam;
	int32		i4_len;				// the number of bytes to load
	int32		i4_played;			// the number of bytes that have played
	uint32		u4_play_csr,u4_slen;

	while (bThreadExit == false)
	{
		// Sleep for our period
		SleepEx(iSTREAM_CALLBACK_PERIOD,false);

		// take the semaphore and timeout after 0.5 seconds
		if (WaitForMultipleObjects(2, ahSemaphore, true, 500) != WAIT_OBJECT_0)
		{
			//dprintf("HandleStream thread function is blocked...Retrying....\n");
			// time out, if the thread has been marked to terminate it will exit
			// after this continue.
			continue;
		}

		EnterCriticalSection(&csStream);
		// go round all active streams and process their requests....
		int32 i4_count;
		for (i4_count = 0; i4_count<MAX_CONCURRENT_STREAMS; i4_count++)
		{
			psam = assamStreams[i4_count].psam;

			// make sure we are not one of the special values.
			Assert( !bSampleError(this));

			// empty slot in the active stream array
			if ( psam == NULL)
				continue;

			if (psam->u4CreateFlags & AU_CREATE_NULL)
			{
				// This sample was created NULL so handle it as in the case
				// of no audio
				ThreadProcessNoAudioSample(psam,i4_count);
				continue;
			}

			// make sure we have a DS buffer attacked to this sample
			Assert(psam->pDSBuffer);

			// make sure we have a loader class
			Assert(psam->pcauSample);

			// 
			// This stream has not been initialized
			//
			if ((psam->u4CreateFlags & AU_STREAM_INIT) == 0)
			{
				// Mark the stream as initialized...
				psam->u4CreateFlags |= AU_STREAM_INIT;

				//
				// Fill the buffer for the first time..
				//

				// lock the whole buffer.
				psam->Lock(0,psam->u4Length);

				uint8* pu1_buffer = psam->pu1Lock1Buffer();
			
				//
				// Check we have a valid buffer before we load into it, this will save a page fault.
				//
				if (pu1_buffer == NULL)
				{
					psam->Unlock();
					assamStreams[i4_count].psam = NULL;
					psam->FlagStop();
					continue;
				}

				psam->pcauSample->u4LoadSampleData( pu1_buffer, psam->u4Lock1BufferLength(), false );

				psam->Unlock();

				psam->pasubSubtitle = psam->pcauSample->pasubCreateSubtitle();

				if (psam->pDSBuffer->Play(0,0,DSBPLAY_LOOPING) != DS_OK)
				{
					assamStreams[i4_count].psam = NULL;
					psam->FlagStop();
				}

				// For this pass do not stream any more data...
				continue;
			}

			psam->pDSBuffer->GetCurrentPosition((ulong*)&u4_play_csr, (ulong*)&u4_slen);

			i4_len = u4_play_csr - psam->u4StreamLoadPosition;

			// if len is negative the play cursor has wrapped back to the start and the play position
			// is still at the end of the buffer, so add the buffer length
			if (i4_len<0)
				i4_len += psam->u4SoundBufferLength();

			// the number of actual bytes played since the last callback
			i4_played = u4_play_csr - psam->u4LastPlayCsr;
			if (i4_played<0)
				i4_played += psam->u4SoundBufferLength();

			psam->u4BytesPlayed += i4_played;			// the number of bytes played
			psam->u4LastPlayCsr =  u4_play_csr;

			// if we have run out of bytes and the sample is not going to loop then stop it
			if ( (psam->u4BytesPlayed > psam->pcauSample->u4DecompressedLength()) && 
				 ((psam->u4CreateFlags & AU_PLAY_LOOPED) == 0) )
			{
				assamStreams[i4_count].psam = NULL;
				psam->pDSBuffer->Stop();
		
				//
				// Flag the stop so that the foreground can stop the sample.
				//
				psam->FlagStop();
			}


			if (i4_len > 0)
			{
#if VER_TEST
				if ( i4_len > (psam->u4SoundBufferLength() - (psam->u4SoundBufferLength()/5)) )
				{
					// the load length for this pass is greater than 80% of the buffer length, audio is now
					// very likely to break up.
					dprintf("Audio stream loading more than 80% of buffer, likely to break up (%d bytes)\n", i4_len);
				}
#endif
				psam->Lock(psam->u4StreamLoadPosition,i4_len);
				
				// fill the first section.
				if (psam->pu1Lock1Buffer()) 
				{
					psam->pcauSample->u4LoadSampleData
					(
						psam->pu1Lock1Buffer(),
						psam->u4Lock1BufferLength(),
						psam->bSampleLooped()
					);
				}

				// fill the first section.
				if (psam->pu1Lock2Buffer()) 
				{
					psam->pcauSample->u4LoadSampleData
					(
						psam->pu1Lock2Buffer(),
						psam->u4Lock2BufferLength(),
						psam->bSampleLooped()
					);
				}

				psam->Unlock();

				// keep track of the latest load position
				psam->u4StreamLoadPosition += i4_len;

				// mod the load position with the buffer length
				if ( psam->u4StreamLoadPosition >= psam->u4SoundBufferLength())
				{
					psam->u4StreamLoadPosition -= psam->u4SoundBufferLength();
				}
			}
		}
		LeaveCriticalSection(&csStream);


		//
		// handle fading samples....
		//
		EnterCriticalSection(&csFade);
		for (i4_count = 0; i4_count<MAX_FADING_SAMPLES; i4_count++)
		{
			if (afFadingSamples[i4_count].psam != NULL)
			{
				if ((afFadingSamples[i4_count].psam->u4CreateFlags & AU_CREATE_NULL)==0)
				{
					float f_vol = afFadingSamples[i4_count].psam->fGetVolume();
					f_vol += afFadingSamples[i4_count].fDeltaVol;

					// if we have gone above maximum, clamp and stop the fade
					if (f_vol>0.0f)
					{
						f_vol = 0.0f;
						afFadingSamples[i4_count].psam = NULL;
					}

					// if we have gone below minimum, clamp and stop the fade
					if (f_vol<-60.0f)
					{
						// If we were told to stop the sample then do so.
						if (afFadingSamples[i4_count].bStopAtMinimum)
						{
							//
							// NOTE: DO NOT CALL THE STOP MEMEBER OTHERWISE WE WILL
							// DEADLOCK AS THE SEMAPHORE IS TAKEN.
							// Instead flag the stop and the foreground will stop
							// the sample.
							//
							afFadingSamples[i4_count].psam->FlagStop();
						}

						f_vol = -60.0f;
						afFadingSamples[i4_count].psam = NULL;
					}

					if (afFadingSamples[i4_count].psam)
					{
						afFadingSamples[i4_count].psam->SetVolume(f_vol);
					}
				}
			}
		}
		LeaveCriticalSection(&csFade);



		//
		// handle deferred loading....
		//
		EnterCriticalSection(&csDefer);
		// do we currently have a deferred operation going????
		if (bDeferredGoing)
		{
			// are we at the last load stage??
			if (u4DeferredLoadRemain <= u4MaxDeferredBytes)
			{
				// load the final block and do the callback
				psamDeferred->pcauSample->u4LoadSampleData( psamDeferred->pu1Lock1Buffer()
					+ u4DeferredLoadOffset, u4DeferredLoadRemain, false );

				psamDeferred->Unlock();

				// use a volatile pointer to stop the compiler optimizing the local pointer away. In this case
				// we need to ensure the pointer is set to zero while the block is still valid, this will
				// prevent other threads from deleting the same block. The we can delete the loader block.
				CCAULoad*	volatile pcau = psamDeferred->pcauSample;
				psamDeferred->pcauSample = NULL;
				delete pcau;

				// remove the sample from the defer load list
				apsamDeferred[u4DeferredIndex] = NULL;
		
				// if there is a callback function...call it
				if (psamDeferred->pdlcbfnCallback)
				{
					psamDeferred->pdlcbfnCallback(psamDeferred,psamDeferred->pvUser);
				}
				else
				{
					//
					// The default callback is to play the sample from the start unless stop as already been
					// called, in which the the AU_CREATE_DEFER_PLAY_ON_CALLBACK will be clear and nothing
					// should be done.
					//
					if (psamDeferred->u4CreateFlags & AU_CREATE_DEFER_PLAY_ON_CALLBACK)
					{
						psamDeferred->pDSBuffer->SetCurrentPosition(0);
						psamDeferred->pDSBuffer->Play(0,0,psamDeferred->u4CreateFlags&AU_PLAY_LOOPED?DSBPLAY_LOOPING:0);
					}
				}

				// clear all of the defer load flags
				psamDeferred->u4CreateFlags &= ~(AU_CREATE_DEFER_LOAD|AU_CREATE_DEFER_PLAY_ON_CALLBACK|AU_CREATE_DEFER_LOAD_ON_PLAY);

				// next time look for another deferred operation
				bDeferredGoing = false;
				psamDeferred = NULL;

				SetupNextDeferLoad();

				LeaveCriticalSection(&csDefer);
			}
			else
			{
				// Leave the defer load critical section before the load because we do not want the other side
				// to wait while the load completes
				LeaveCriticalSection(&csDefer);

				// load a full block...
				psamDeferred->pcauSample->u4LoadSampleData( psamDeferred->pu1Lock1Buffer() + 
					u4DeferredLoadOffset, u4MaxDeferredBytes, false );
				u4DeferredLoadOffset += u4MaxDeferredBytes;
				u4DeferredLoadRemain -= u4MaxDeferredBytes;
			}
		}
		else
		{
			//
			// Initialize the next deferred operation
			//
			SetupNextDeferLoad();
			LeaveCriticalSection(&csDefer);
		}

		// release both semaphores
		ReleaseSemaphore(ahSemaphore[0],1,NULL);
		ReleaseSemaphore(ahSemaphore[1],1,NULL);
	}

	return 0;
}


//**********************************************************************************************
// Helper funciton called from above thread to setup the next sample which is to be defer loaded.
void CAudio::SetupNextDeferLoad
(
)
//*************************************
{
	CSample*	psam;

	for (int32 i4_count = 0; i4_count<MAX_DEFERRED_OPS; i4_count++)
	{
		psam = apsamDeferred[i4_count];

		Assert( !bSampleError(this) );

		if (psam != NULL)
		{
			if (psam->u4CreateFlags & AU_CREATE_NULL)
				continue;

			psamDeferred = psam;

			// The defered load sample cannot be streamed. Streams are inherently defer loaded.
			Assert ( (psamDeferred->u4CreateFlags & AU_CREATE_STREAM) == 0 );

			// lock the whole buffer.
			psam->Lock(0,psam->u4Length);
			u4DeferredLoadOffset = 0;
			u4DeferredLoadRemain = psam->u4Lock1BufferLength();
			u4DeferredIndex = (uint32) i4_count;
			bDeferredGoing = true;
			break;
		}
	}
}


//**********************************************************************************************
typedef HRESULT (__stdcall *FP_DSOUND)(LPSTR, LPDIRECTSOUND *, LPSTR);


//**********************************************************************************************
// Create the DirectSound and DS3DListener objects.
// If any of this creation fails sound is disabled but the system will remain stable.
//
void CAudio::CreateAudio
(
	void*			h_wnd,
	bool			b_enable
)
//	
//**************************************
{
	HRESULT					hr;
	CDDSize<DSBUFFERDESC>	dsbd;

	// clear out all of the pseudo 3d sample pointers
	uint32 u4_p_count;
	for (u4_p_count = 0; u4_p_count<MAX_PSEUDO3D_SAMPLES; u4_p_count++)
	{
		apsamPseudo[u4_p_count] = NULL;
	}

	// clear out all of the fading audio pointers
	for (u4_p_count = 0; u4_p_count<MAX_FADING_SAMPLES; u4_p_count++)
	{
		afFadingSamples[u4_p_count].psam = NULL;
	}

	// No deferred samples loading
	for (u4_p_count = 0; u4_p_count<MAX_DEFERRED_OPS; u4_p_count++)
	{
		apsamDeferred[u4_p_count] = NULL;
	}


    FP_DSOUND fp_ds;

	if (b_enable)
	{
		hLibrary = LoadLibrary("A3d.dll");
		if (hLibrary)
		{
			// we have loaded the A3D dll, so lets use it
			fp_ds = (FP_DSOUND) GetProcAddress(hLibrary, "_A3dCreate@12");

			if (fp_ds == NULL)
			{
				// we have failed to find the A3D create function, try the normal
				// C call function name
				fp_ds = (FP_DSOUND) GetProcAddress(hLibrary, "A3dCreate");
				if (fp_ds == NULL)
				{
					// we have failed again so close the DLL and try direct sound
					FreeLibrary(hLibrary);
					hLibrary = NULL;
				}
			}

			//
			// We now have a pointer to the A3D create function
			//
			hr = (fp_ds)(NULL, &pDSInterface, NULL);

			if ((hr != DS_OK) && (hr != A3D_OK))
			{
				SetNoAudio();
				return;
			}

			//
			// we have found an A3D card so lets query for its interface.
			//
			if (hr == A3D_OK)
			{
				hr = pDSInterface->QueryInterface(DSQUERY_A3D, (void**)&pA3D);
				if (hr == NOERROR)
				{
					if (pA3D)
					{
						pA3D->SetResourceManagerMode(A3D_RESOURCE_MODE_DYNAMIC);
					}
				}
			}
		}


		if (hLibrary == NULL)
		{
			// Either A3D dll was not found or it had trouble finding its create function.
			hLibrary = LoadLibrary("DSound.dll");
			if (hLibrary == NULL)
			{
				// failed to find direct sound
				SetNoAudio();
				return;
			}

			// look for the DS create function
			fp_ds = (FP_DSOUND) GetProcAddress(hLibrary, "DirectSoundCreate");

			hr = (fp_ds)(NULL, &pDSInterface, NULL);

			if (hr != DS_OK)
			{
				SetNoAudio();
				return;
			}
		}
	}
	else
	{
		// Audio is not to be enabled.
		SetNoAudio();
		return;
	}

    // Note we need to set the coop level to be priority to set the
    // format of the primary buffer
	hr = pDSInterface->SetCooperativeLevel(hWnd,DSSCL_PRIORITY);

	bHardware = false;
	b3DHardwareFound = false;
	bSoundEnabled = false;

	// Get the DS caps
	CDDSize<DSCAPS>	dscaps;

	if (pDSInterface->GetCaps(&dscaps) == DS_OK)
	{
		// Do we have hardware 3D caps??
		if (dscaps.dwMaxHw3DAllBuffers>0)
		{
			b3DHardwareFound = true;
			bHardware = true;
		}
	}

	// Set up the primary direct sound buffer with 3D caps.
	dsbd.dwFlags=DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
    hr = pDSInterface->CreateSoundBuffer(&dsbd,&pDSPrimary,NULL);
	if (hr != DS_OK)
	{
		pDSInterface->Release();
		SetNoAudio();
		return;
	}

	SetOutputFormat(u4Frequency,escChannels,esbSampleBits);

	hr = pDSPrimary->QueryInterface(IID_IDirectSound3DListener,(void**)&pDS3DListener);

	if (hr != DS_OK)
	{
		// release the primary buffer
		pDSPrimary->Release();
		pDSInterface->Release();
		SetNoAudio();
		return;
	}

	// play the primary buffer.
	hr = pDSPrimary->Play(0,0,DSBPLAY_LOOPING);
	if (hr != DS_OK)
	{
		// release the 3D listener
		if (pDS3DListener)
		{
			pDS3DListener->Release();
		}
		
		// release the primary buffer
		pDSPrimary->Release();
		// release DS
		pDSInterface->Release();
		SetNoAudio();
		return;
	}

	bCanStream = bCreateStreamSupport();


	//
	// Enable the EAX property set
	//
	EnableEnvironmentalAudio();	


	// all went well set sound to be enabled
	bSoundEnabled = true;
}




//**********************************************************************************************
// Destroy Audio and any associated caches
//
void CAudio::DestroyAudio
(
	bool b_shutdown
)
//	
//**************************************
{
	RemoveStreams();

	if (pDS3DListener)
	{
		pDS3DListener->Release();
		pDS3DListener = NULL;
	}

	// stop and release the primary buffer if we have one
	if (pDSPrimary)
	{
		pDSPrimary->Stop();
		pDSPrimary->Release();
		pDSPrimary=NULL;
	}

	// Remove the EAX interface and support buffer if it is present.
	RemoveEnvironmentalAudio();

	// if we have the A3D interface the delete it as well
	if (pA3D)
	{
		pA3D->Release();
		pA3D = NULL;
	}

	// free the main DirectSound Object, if one exists
	if (pDSInterface)
	{
		pDSInterface->Release();
		pDSInterface=NULL;
	}

	// If we have a DLL then close it.
	if (hLibrary)
	{
		FreeLibrary(hLibrary);
		hLibrary = NULL;
	}

	bSoundEnabled = false;
	bChangeFormat = false;

	if (!b_shutdown)
	{
		// If we have just destroyed audio for the sake of it then create set no audio. If we
		// have destroyed audio because we are closing down then do not bother with this.
		SetNoAudio();
	}
}



//**********************************************************************************************
void CAudio::SetNoAudio
(
)
//	
//**************************************
{
	// Clear all of the interface pointers
	pDSInterface	= NULL;
	pDSPrimary		= NULL;
	pDS3DListener	= NULL;
	b3DHardwareFound= false;
	bHardware		= false;
	bSoundEnabled	= false;

	// we cannot have streams if we have got this far
	Assert(bCanStream == false);

	// empty the stream list
	for (int i4 = 0;i4<MAX_CONCURRENT_STREAMS;i4++)
	{
		assamStreams[i4].psam = NULL;
		assamStreams[i4].u4StopTick = 0;
	}

	// create a semaphore so we do not delete the streamed samples while they are being filled.
	// By default the semaphore is taken so the thread is not running.
	ahSemaphore[0] = CreateSemaphore(NULL,0,1,"__TRESPASSER_NOAUDIO_STREAM_0__");
	if (ahSemaphore[0] == NULL)
		return;

	bThreadLocked = true;

	// second semaphore is not taken
	ahSemaphore[1] = CreateSemaphore(NULL,1,1,"__TRESPASSER_NOAUDIO_STREAM_1__");
	if (ahSemaphore[1] == NULL)
	{
		CloseHandle(ahSemaphore[0]);
		ahSemaphore[0] = NULL;
		return;
	}

	bThreadExit = false;

	hThread = CreateThread(NULL,0,HandleNoAudioSubtitleStub,this,0,(DWORD*)&u4ThreadID);

	if (hThread == 0)
	{
		// if we failed remember to close the semaphore
		CloseHandle(ahSemaphore[0]);
		ahSemaphore[0] = NULL;
		CloseHandle(ahSemaphore[1]);
		ahSemaphore[1] = NULL;

		return;
	}

	dprintf("Thread Create: CAudio [Thread handle 0x%x]\n", u4ThreadID);

	//
	// Now we have a thread that we handle subtitle creation.
	//
}


//**********************************************************************************************
// Create a helper thread when there is no audio to allow us to load subtitles in the background.
//
DWORD _stdcall CAudio::HandleNoAudioSubtitleStub
(
	void* pv_user
)
//	
//**************************************
{
	CAudio*			pca = (CAudio*)pv_user;

	return pca->u4HandleNoAudioSubtitle();
}


//**********************************************************************************************
// Create a helper thread when there is no audio to allow us to load subtitles in the background.
//
uint32 CAudio::u4HandleNoAudioSubtitle
(
)
//	
//**************************************
{
	CSample*		psam;

	while (bThreadExit == false)
	{
		SleepEx(250,false);

		// take the semaphore and timeout after 0.5 seconds
		if (WaitForMultipleObjects(2, ahSemaphore, true, 500) != WAIT_OBJECT_0)
		{
			//dprintf("HandleNoAudioSubtitle thread is blocked...Retrying....\n");
			// time out, if the thread has been marked to terminate it will exit
			// after this continue.
			continue;
		}

		EnterCriticalSection(&csStream);
		// go round all active streams and process their requests....
		for (int32 i4_count = 0; i4_count<MAX_CONCURRENT_STREAMS; i4_count++)
		{
			psam = assamStreams[i4_count].psam;

			// make sure we are not one of the special values.
			Assert( !bSampleError(this) );

			// empty slot in the active stream array
			if ( psam == NULL)
				continue;

			ThreadProcessNoAudioSample(psam,i4_count);
		}
		LeaveCriticalSection(&csStream);

		ReleaseSemaphore(ahSemaphore[0],1,NULL);
		ReleaseSemaphore(ahSemaphore[1],1,NULL);
	}

	return 0;
}


//**********************************************************************************************
// This function is called by both thread functions.
// In the no audio case it is called for all streamed samples.
// In the audio case it is called for streamed samples created withe the AU_CREATE_NULL flag
//
void CAudio::ThreadProcessNoAudioSample
(
	CSample*	psam,		// the sample
	int32		i4_count	// the index of this sample in the stream array
)
//**************************************
{
	// 
	// This stream has not been initialized
	//
	if ((psam->u4CreateFlags & AU_STREAM_INIT) == 0)
	{
		// Mark the stream as initialized...
		psam->u4CreateFlags |= AU_STREAM_INIT;

		if (psam->pcauSample != NULL)
		{
			psam->pasubSubtitle = psam->pcauSample->pasubCreateSubtitle();

			assamStreams[i4_count].u4StopTick = GetTickCount() + (int)(psam->fPlayTime*1000.0f);
		}
		else
		{
			// Flag the sample to be stopped if there is no loader....
			assamStreams[i4_count].psam = NULL;
			psam->FlagStop();
		}

		// Delete the loader for this sample
		delete psam->pcauSample;
		psam->pcauSample = NULL;
	}
	else
	{
		if (GetTickCount()>=assamStreams[i4_count].u4StopTick)
		{
			// time until the sample has finished at which point we set the
			// stop flag
			assamStreams[i4_count].psam = NULL;
			psam->FlagStop();
		}
	}
}


//**********************************************************************************************
void CAudio::EnableEnvironmentalAudio
(
)
//	
//**************************************
{
	CDDSize<DSBUFFERDESC>	dsbd;
	HRESULT					hr;
	WAVEFORMATEX			format;

	pDSEAX = NULL;
	pDSEAX3D = NULL;

	if (pDSInterface == NULL)
		return;

	// Create a 128 byte static buffer that will not get deleted...
	dsbd.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRL3D;

	dsbd.dwBufferBytes		= 128;
	dsbd.lpwfxFormat		= &format;

	format.wFormatTag		= WAVE_FORMAT_PCM;
	format.nChannels		= 2;
	format.nSamplesPerSec	= 22050;
	format.nAvgBytesPerSec	= 88200;
	format.nBlockAlign		= 4;
	format.wBitsPerSample	= 16;
	format.cbSize			= 0;

	// create the EAX sound buffer
	hr=pDSInterface->CreateSoundBuffer(&dsbd,&pDSEAX,NULL);

	if (hr != DS_OK)
	{
		pDSEAX = NULL;
		pDSPropertySet = NULL;
		return;
	}


	// Query this sound buffer for the 3D interface
	hr=pDSEAX->QueryInterface(IID_IDirectSound3DBuffer, (void**)&pDSEAX3D);
	if (hr != DS_OK)
	{
		// Query for the 3D interface failed so we cannot go any futher creating the
		// EAX property set.
		pDSEAX->Release();
		pDSEAX = NULL;
		pDSEAX3D = NULL;
		pDSPropertySet = NULL;
		return;
	}


	// Query the 3D sound buffer for the property set interface...
	hr=pDSEAX3D->QueryInterface(IID_IKsPropertySet, (void**)&pDSPropertySet);
	if (hr != DS_OK)
	{
		//
		// Card that do not support property sets will fail here, NT4.0 will fail here.
		// Make sure we release the 3D buffer so we do not waste any resources.
		//
		pDSEAX3D->Release();
		pDSEAX->Release();
		pDSEAX = NULL;
		pDSEAX3D = NULL;
		pDSPropertySet = NULL;
		return;
	}


	// Now create the EAX reverb interface from the DS property set interface....
	hr=pDSPropertySet->QuerySupport(DSPROPSETID_EAX, DSPROPERTY_EAX_ALL, (unsigned long*)&u4EAXPropertySupport);

	if ((hr != DS_OK) || ((u4EAXPropertySupport & (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET)) != (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET)))
	{
		// Either the query for EAX support failed or we cannot get and set EAX properties, in
		// either case we fail the EAX support and continue without it.
		pDSPropertySet->Release();
		pDSEAX3D->Release();
		pDSEAX->Release();
		pDSEAX = NULL;
		pDSEAX3D = NULL;
		pDSPropertySet = NULL;
		return;
	}

	//
	// If we get to here EAX is initialized and running...
	//

	// Set the defaults so there is no reverb.
	SetEAXEnvironment(0);
	SetEAXReverbVolume(0.0f);
	SetEAXReverbDecay(0.1f);
}


//**********************************************************************************************
void CAudio::RemoveEnvironmentalAudio
(
)
//	
//**************************************
{
	if (pDSEAX == NULL)
		return;

	if (pDSPropertySet)
		pDSPropertySet->Release();
	if (pDSEAX3D)
		pDSEAX3D->Release();
	if (pDSEAX)
		pDSEAX->Release();

	pDSEAX = NULL;
	pDSEAX3D = NULL;
	pDSPropertySet = NULL;
}


//**********************************************************************************************
SAudioEnvironment CAudio::aenvGetEnvironment()
{
	SAudioEnvironment	aenv;
	uint32				u4_size;

	if (pDSPropertySet == NULL)
	{
		aenv.u4Environment = -1;
		return aenv;
	}

	if (pDSPropertySet->Get(DSPROPSETID_EAX,DSPROPERTY_EAX_ALL, NULL, 0, &aenv, sizeof(aenv), (unsigned long*)&u4_size) != DS_OK)
	{
		aenv.u4Environment = -1;
		return aenv;
	}

	Assert(u4_size == sizeof(aenv));

	return aenv;	
}


//**********************************************************************************************
void CAudio::SetEAXEnvironment(uint32 u4_env)
{
	if (pDSPropertySet == NULL)
	{
		return;
	}

	if (pDSPropertySet->Set(DSPROPSETID_EAX,DSPROPERTY_EAX_ENVIRONMENT, NULL, 0, &u4_env, sizeof(uint32)) != DS_OK)
	{
		Assert(0);
		return;
	}
}


//**********************************************************************************************
void CAudio::SetEAXReverbVolume(float f_rvol)
{
	if (pDSPropertySet == NULL)
	{
		return;
	}

	if (pDSPropertySet->Set(DSPROPSETID_EAX,DSPROPERTY_EAX_VOLUME, NULL, 0, &f_rvol, sizeof(float)) != DS_OK)
	{
		Assert(0);
		return;
	}
}


//**********************************************************************************************
void CAudio::SetEAXReverbDecay(float f_decay)
{
	if (pDSPropertySet == NULL)
	{
		return;
	}

	if (pDSPropertySet->Set(DSPROPSETID_EAX,DSPROPERTY_EAX_DECAYTIME, NULL, 0, &f_decay, sizeof(float)) != DS_OK)
	{
		Assert(0);
		return;
	}
}


//**********************************************************************************************
void CAudio::SetEAXReverbDamping(float f_damp)
{
	if (pDSPropertySet == NULL)
	{
		return;
	}

	if (pDSPropertySet->Set(DSPROPSETID_EAX,DSPROPERTY_EAX_DAMPING, NULL, 0, &f_damp, sizeof(float)) != DS_OK)
	{
		Assert(0);
		return;
	}
}


//**********************************************************************************************
// Enables or disables th audio system.
// This will open and close the whole audio system without destroying the pcaAudio global class.
// Therefore the rest of the game can continue as normal and assume that the audio class is
// always present
//
void CAudio::AudioEnable
(
	bool b_enable
)
//	
//**************************************
{
	if (b_enable)
	{
		// we are enabling the sound system, does it already exist??
		if (pDSPrimary)
			return;

		void*			h_wnd;
		h_wnd = hWnd;

		// create the new audio system
		CreateAudio(h_wnd, bMasterEnable);
	}
	else
	{
		// the system is already disabled
		if (pDSPrimary == NULL)
			return;

		DestroyAudio(false);
	}
}



//**********************************************************************************************
// get the current speaker config that DS is using. The default is stereo
//
ESpeakerConfig CAudio::esconGetSpeakerConfig
(
)
//	
//**************************************
{
	uint32			u4_dsspeaker;
	ESpeakerConfig	escon = esconSTEREO;

	// get the current speaker type, if this returns an error we do not care because STEREO will
	// be returned which is the default.
	if (pDSInterface)
	{
		if (pDSInterface->GetSpeakerConfig((DWORD*)&u4_dsspeaker)!=DS_OK)
			return esconSTEREO;
	}
	else
	{
		return esconSTEREO;
	}

	switch (u4_dsspeaker)
	{
	case DSSPEAKER_HEADPHONE:
		escon = esconHEADPHONES;
		break;

	case DSSPEAKER_MONO:
		escon = esconMONO;
		break;

	case DSSPEAKER_STEREO:
		escon = esconSTEREO;
		break;

	case DSSPEAKER_QUAD:
		escon = esconQUAD;
		break;

	case DSSPEAKER_SURROUND:
		escon = esconSURROUND;
		break;

	default:
		// if it is something we do no know about just return STEREO
		escon = esconSTEREO;
		break;
	}

	return escon;
}



//**********************************************************************************************
// set the current speaker config...
//
void CAudio::SetSpeakerConfig
(
	ESpeakerConfig escon_speakers
)
//	
//**************************************
{
	static uint32	u4_dsspeakers[] =
	{
		DSSPEAKER_MONO,
		DSSPEAKER_STEREO,
		DSSPEAKER_QUAD,
		DSSPEAKER_SURROUND,
		DSSPEAKER_HEADPHONE
	};

	Assert(escon_speakers<esconLAST);

	if (pDSInterface)
	{
		pDSInterface->SetSpeakerConfig(u4_dsspeakers[escon_speakers]);
	}
}




//**********************************************************************************************
// Change/set the format of the primary buffer.
//
void CAudio::SetOutputFormat
(
	uint32			u4_frequency,
	ESoundChannels	esc,
	ESoundBits		esb
)
//	
//**************************************
{

	WAVEFORMATEX	format;

	bChangeFormat = false;

	format.wFormatTag		= WAVE_FORMAT_PCM;
	format.nChannels		= esc;
	format.nSamplesPerSec	= u4_frequency;
	format.nAvgBytesPerSec	= (u4_frequency * esc * esb) >> 3;
	format.nBlockAlign		= (esc * esb) >> 3;
	format.wBitsPerSample	= esb;
	format.cbSize			= 0;

	// is there a primary buffer?? If not then just return
	if (pDSPrimary == NULL)
		return;

	// set the format of the primary buffer
	if ( pDSPrimary->SetFormat(&format) == DS_OK)
	{
		bChangeFormat = true;

		u4Format = u4_frequency;
		if (esc == escSTEREO)
		{
			u4Format |= AU_FORMAT_STEREO;
		}

		if (esb == esb16BIT)
		{
			u4Format |= AU_FORMAT_16BIT;
		}

		// copy into the static members...
		u4Frequency			= u4_frequency;
		escChannels			= esc;
		esbSampleBits		= esb;
	}
}





//**********************************************************************************************
// Position the listener in the world. 
// NOTE: CO-ORDINATES ARE IN D3D CO-ORDINATES WHICH ARE Z FORWARDS, Y UP
//
void CAudio::PositionListener
(
	float f_x,
	float f_y,
	float f_z,
	bool  b_immediate
)
//	
//**************************************
{
	if (bSoundEnabled == FALSE)
		return;

	// keep a copy of the listener position
	fListenerX = f_x;
	fListenerY = f_y;
	fListenerZ = f_z;

	// position the DS3D listener if we have one
	pDS3DListener->SetPosition(f_x, f_y, f_z, b_immediate?DS3D_IMMEDIATE:DS3D_DEFERRED);

	// if the immediate flag is set the update the volume of all pseudo 3D samples
	if (b_immediate)
	{
		CommitPseudoSettings();
	}
}




//**********************************************************************************************
// the two vectors passed to this member specify the forwards and up vectors of the listener.
// The forward vector if from the center of your head through your nose and the up vector is
// from the center through the top of your head.
//
void CAudio::OrientListener
(
	float in_x,
	float in_y,
	float in_z,

	float up_x,
	float up_y,
	float up_z,

	bool  b_immediate
)
//	
//**************************************
{
	if (bSoundEnabled == FALSE)
		return;

	pDS3DListener->SetOrientation(in_x,in_y,in_z,up_x,up_y,up_z,b_immediate?DS3D_IMMEDIATE:DS3D_DEFERRED);
}



//**********************************************************************************************
// Do an update calculation on any new settings for 3D buffers. This should be done as little as
// possible....
//
void CAudio::CommitSettings
(
)
//	
//**************************************
{
	if (bSoundEnabled == FALSE)
		return;

	pDS3DListener->CommitDeferredSettings();

	CommitPseudoSettings();
}


//******************************************************************************************
//
void CAudio::CommitPseudoSettings()
{
	for (uint32 u4=0 ; u4<MAX_PSEUDO3D_SAMPLES; u4++)
	{
		if (apsamPseudo[u4] != NULL)
		{
			apsamPseudo[u4]->SetVolume( apsamPseudo[u4]->fCalculateVolumeAttenuation() );
		}
	}
}


//******************************************************************************************
//
void CAudio::SetVolume
(
    float f_volume
)
{
    if (pDSPrimary)
    {
        // we need to convert our linear (0, 100) scale
        // to (-10,000, 0) log scale

        //if low enough snap to off
        if ( f_volume <= 1.0f )
        {
            pDSPrimary->SetVolume((int32) -10000);
        }
        else
        {
            pDSPrimary->SetVolume( (int32)( log10((double)f_volume) * 10000.0f / 2.0f ) - 10000.0f);
        }
    }
}



//******************************************************************************************
//
float CAudio::GetVolume()
{
    long        l;
    float       fReturn = 0.0f;

    if (pDSPrimary)
    {
        pDSPrimary->GetVolume(&l);

        //we need to convert our log (-10000, 0) scale
        // to (0, 100) scale

        //snap any num < -9600 as -10000
        if ( l > -9600L)
        {
            fReturn = (float) (  pow( (double)10, (double)(((l + 10000.0f) * 2.0f) / 10000.0f)) );
        }
    }

    return fReturn;
}



//**********************************************************************************************
//
LPDIRECTSOUNDBUFFER	CAudio::CreateDSBuffer
(
	uint32			u4_bytes,
	uint32			u4_frequency,
	ESoundChannels	esc_channels,
	ESoundBits		esb_bits,
	uint32			u4_flags
)
//	
//**************************************
{
	LPDIRECTSOUNDBUFFER		psbuffer;
	CDDSize<DSBUFFERDESC>	dsbd;
	HRESULT					hr;
	WAVEFORMATEX			format;

	Assert(pDSInterface);
	Assert(bSoundEnabled);


	if (!bUsingHardware())
	{
		// If we are not using hardware and this is a streaming buffer or a 3D buffer
		// then make sure that the buffer is created in software.
		if (u4_flags & (AU_CREATE_STREAM|AU_CREATE_SPATIAL_3D))
		{
			dsbd.dwFlags = DSBCAPS_LOCSOFTWARE;
		}
		else
		{
			// if we are not streaming then the sound buffer can be on card memory
			// create the direct sound structure
			dsbd.dwFlags = DSBCAPS_STATIC;
		}
	}
	else
	{
		// We are using 3D hardware so make sure 3D buffers have the LOC_HARDWARE set, all other buffers
		// cab do the default thing. The default is for direct sound to try and create a hardware buffer
		// and if that fails drop back to software. If the LOC_HARDWARE flag is set then the buffer will
		// fail to be created if the hardware resources are all used up.
		if (u4_flags & AU_CREATE_SPATIAL_3D)
		{
			dsbd.dwFlags = DSBCAPS_LOCHARDWARE;

			// if we are not streaming then the sound buffer is also static. This enables the sound
			// driver to be more efficient in its allocations.
			if ((u4_flags & AU_CREATE_STREAM) == 0)
			{
				dsbd.dwFlags |= DSBCAPS_STATIC;
			}
		}
		else
		{
			dsbd.dwFlags = DSBCAPS_LOCSOFTWARE;
		}
	}

	//
	// If the force software flag is set then force the LOCSOFTWARE flag on and the LOCHARDWARE flag off.
	//
	if (u4_flags & AU_CREATE_FORCE_SOFTWARE)
	{
		dsbd.dwFlags |=  DSBCAPS_LOCSOFTWARE;
		dsbd.dwFlags &=~ DSBCAPS_LOCHARDWARE;
	}

	if (u4_flags & AU_CREATE_SPATIAL_3D)
	{
		dsbd.dwFlags |= (DSBCAPS_CTRL3D);
	}

	// only allow volume control if is explicitly requested.
	if (u4_flags & AU_CREATE_CTRL_VOLUME)
	{
		dsbd.dwFlags |= (DSBCAPS_CTRLVOLUME);
	}

	// only allow frequency control if is explicitly requested.
	if (u4_flags & AU_CREATE_CTRL_FREQUENCY)
	{
		dsbd.dwFlags |= DSBCAPS_CTRLFREQUENCY;
	}

	// only allow pan control if it is explicitiy requested
	if (u4_flags & AU_CREATE_CTRL_PAN)
	{
		dsbd.dwFlags |= DSBCAPS_CTRLPAN;
	}

	uint32 u4_block_bytes = ((esc_channels * esb_bits) >> 3);

	// round our buffer upto the next block size, just in case it isn't
	u4_bytes += (u4_block_bytes-1);
	u4_bytes &= ~(u4_block_bytes-1);

	dsbd.dwBufferBytes		= u4_bytes;
	dsbd.lpwfxFormat		= &format;

	format.wFormatTag		= WAVE_FORMAT_PCM;
	format.nChannels		= esc_channels;
	format.nSamplesPerSec	= u4_frequency;
	format.nAvgBytesPerSec	= u4_frequency * u4_block_bytes;
	format.nBlockAlign		= u4_block_bytes;
	format.wBitsPerSample	= esb_bits;
	format.cbSize			= 0;

	// create the sound buffer
	hr=pDSInterface->CreateSoundBuffer(&dsbd,&psbuffer,NULL);
	if (hr != DS_OK)
	{
		return(NULL);
	}

	return(psbuffer);
}




//**********************************************************************************************
// Create a CSample from a CAU file that is on disk
// If this function returns NULL it is the responsibility of this function or below to ensure
// that any deferred load request is cancelled.
//
CSample* CAudio::psamCreateSample
(
	const char*		str_fname,
	uint32			u4_flags,
	TDeferCallback	pdlcbfn,
	void*			pv_user
)
//	
//**************************************
{
	CCAULoad*	pcau;
	CSample*	psam;

	Assert((u4_flags & AU_CREATE_MAPPED) == false);

	char	buf[MAX_PATH];
	wsprintf(buf,"%s%s",pcaAudio->GetBasePathName().c_str(),str_fname);

	pcau = CCAULoad::pcauCreateAudioLoader(buf);

	//
	// If we get a NULL loader, do not create an empty sample, just return..
	//
	if (pcau == NULL)
		return NULL;

	Assert (((u4_flags & AU_CREATE_DEFER_LOAD) && (pdlcbfn!=NULL)) || 
			((u4_flags & AU_CREATE_DEFER_LOAD)==0));

	//  a NULL loader passed to here will result in a empty sample.
	psam = new CSample(pcau,u4_flags,pdlcbfn,pv_user);
	
	return psam;
}



//**********************************************************************************************
// Create a CSample from a CAU file that is in a packed audio file. If audio is disabled then
// do not create a loader and pass NULL to the sample constructor which in turn will create a
// shell sample and consume minimum memory
//
CSample* CAudio::psamCreateSample
(
	TSoundHandle	sndhnd_sample,
	CAudioDatabase*	padat,
	uint32			u4_flags,
	TDeferCallback	pdlcbfn,		// Numm funtion pointer for default callback whichs checks the play flag
	void*			pv_user
)
//	
//**************************************
{
	CCAULoad*	pcau;
	CSample*	psam;

	Assert((u4_flags & AU_CREATE_MAPPED) == false);

	// if the database is null return a null sample
	if (padat ==  NULL)
		return NULL;

 	pcau = CCAULoad::pcauCreateAudioLoader(padat,sndhnd_sample);

	//
	// If we get a NULL loader, do not create an empty sample, just return..
	//
	if (pcau == NULL)
		return NULL;

	//  a NULL loader passed to here will result in a empty sample.
	psam = new CSample(pcau,u4_flags,pdlcbfn,pv_user);

	return psam;
}



//**********************************************************************************************
// The fading array is marked as volatile so the compiler will no try any clever optimizations.
// This is really important as we set the psam element of the fade structure last, until the
// psam is set the thread will ignore the fade element.
// All other elements in the array must be set before the psam otherwise if we catch the thread
// at the correct time it will fade with invalid data. The alternative to this method which must
//
void CAudio::FadeSample
(
	CSample*	psam,
	float		f_fader,
	bool		b_stop
)
//	
//**************************************
{
	uint32	u4_sample_count;

	// make sure we are not one of the special values.
	Assert( !bSampleError(this) );

	EnterCriticalSection(&csFade);

	// make sure the fade is within range, 100dBs will effectively start and stop the sound.
	Assert ((f_fader>=-100.0f) && (f_fader<=100.0f));

	for (u4_sample_count = 0; u4_sample_count<MAX_FADING_SAMPLES; u4_sample_count++)
	{
		if (afFadingSamples[u4_sample_count].psam == NULL)
		{
			afFadingSamples[u4_sample_count].fDeltaVol	= f_fader;

			// make sure the psam element is set last, so the thread does not act on it until
			// all elements are set
			afFadingSamples[u4_sample_count].psam			= psam;
			afFadingSamples[u4_sample_count].bStopAtMinimum	= b_stop;

			LeaveCriticalSection(&csFade);
			return;
		}
	}

	LeaveCriticalSection(&csFade);

	// no room in the fading list.....
	AlwaysAssert(0);
}




//**********************************************************************************************
// Stop the specified sample from fading. This is important if you are about to delete a sample
// and it could still be fading.
//
// If a fading sample is deleteD you will crash and burn in the audio thread.
//
void CAudio::StopFade
(
	CSample*	psam
)
//	
//**************************************
{
	uint32	u4_sample_count;

	// make sure we are not one of the special values.
	Assert( !bSampleError(this) );

	EnterCriticalSection(&csFade);

	for (u4_sample_count = 0; u4_sample_count<MAX_FADING_SAMPLES; u4_sample_count++)
	{
		if (afFadingSamples[u4_sample_count].psam == psam)
		{
			afFadingSamples[u4_sample_count].psam = NULL;
		}
	}

	LeaveCriticalSection(&csFade);
}



//**********************************************************************************************
// Add a deferred load operation to the audio thread, this operation will load 1 buffer of data
// at the start of the sample buffer. This makes this function suitable for loading whole
// samples and loading the initial block of a streamed sample.
//
void CAudio::AddDeferredSampleFill
(
	CSample*	psam
)
//	
//**************************************
{
	uint32	u4_sample_count;

	// make sure we are not one of the special values.
	Assert( !bSampleError(this) );

	EnterCriticalSection(&csDefer);

	for (u4_sample_count = 0; u4_sample_count<MAX_DEFERRED_OPS; u4_sample_count++)
	{
		if (apsamDeferred[u4_sample_count] == NULL)
		{
			apsamDeferred[u4_sample_count] = psam;
			// we have found an empty entry

			LeaveCriticalSection(&csDefer);
			return;
		}
	}

	LeaveCriticalSection(&csDefer);

	// no room in the deferred array make MAX_DEFERRED_OPS bigger
	AlwaysAssert(0);
}




//**********************************************************************************************
// To remove a deferred load operation that has not yet completed we must do it under control
// of the semaphore. This is because we are about to delete the sample so we have to make sure
// that we are safe to prevent a hard crash.
//
void CAudio::RemoveDeferred
(
	CSample*	psam
)
//	
//**************************************
{
	// make sure we are not one of the special values.
	Assert( !bSampleError(this) );

	EnterCriticalSection(&csDefer);

	//
	// While under the protection of the defer critical section check this samples defer load bit
	//
	if ((psam->u4CreateFlags & AU_CREATE_DEFER_LOAD)==0)
	{
		LeaveCriticalSection(&csDefer);
		return;
	}

	// are we removing the sample that is currently loading??
	if (psam == psamDeferred)
	{
		psamDeferred	= NULL;
		bDeferredGoing	= false;
	}

	// go through the deferred list and remoe the sample if it is in it
	for (int32 i4 = 0;i4<MAX_DEFERRED_OPS;i4++)
	{
		// we have found the specifed sample in the deferred list
		if (apsamDeferred[i4] == psam)
		{
			// if there is a callback function...call it with the special sample
			// handle to singal that it has been deleted.
			if (psam->pdlcbfnCallback)
			{
				psam->pdlcbfnCallback(psamDEFER_LOAD_DELETE, psam->pvUser);
			}

			// clear out the sample from the deferred list
			apsamDeferred[i4] = NULL;
		}
	}
	LeaveCriticalSection(&csDefer);
}




//**********************************************************************************************
// Force clean up the low level audio system. Basically make sure all the buffers are empty and
// the thread semaphore is not taken.
// Any other samples that the audio system knows about should have been deleted. This function
// does not pay any regard to samples, it only gets the audio system into a stable state from
// an unknown state.
//
void CAudio::CleanUp
(
)
//	
//**************************************
{
	// Take the second semaphore to prevent the audio loader doing another pass.
	// We do not care what state the first semaphore is in
	WaitForSingleObject(ahSemaphore[1],INFINITE);

	//OutputDebugString("CAudio cleanup enter: Loader thread is blocked.\n");

	// We have no streaming buffers....
	int32 i4_count;
	for (i4_count = 0; i4_count<MAX_CONCURRENT_STREAMS; i4_count++)
	{
		// All streams should be NULL
		assamStreams[i4_count].psam = NULL;
		assamStreams[i4_count].u4StopTick = 0;
	}

	// We have no fading buffers...
	for (i4_count = 0; i4_count<MAX_FADING_SAMPLES; i4_count++)
	{
		// All fading samples should be NULL
		afFadingSamples[i4_count].psam = NULL;
	}

	// No deferred samples loading
	for (i4_count = 0; i4_count<MAX_DEFERRED_OPS; i4_count++)
	{
		apsamDeferred[i4_count] = NULL;
	}

	for (i4_count = 0; i4_count<MAX_PSEUDO3D_SAMPLES; i4_count++)
	{
		apsamPseudo[i4_count] = NULL;
	}

	bDeferredGoing = false;
	psamDeferred = NULL;

	//OutputDebugString("CAudio cleanup leave: Loader thread is running.\n");

	// free up the semaphore so the threaded streamer can continue
	ReleaseSemaphore(ahSemaphore[1],1,NULL);
}


//**********************************************************************************************
void CAudio::ResumeAudioLoader()
{
	bThreadLocked = false;
	// free up the semaphore so the threaded streamer can continue
	ReleaseSemaphore(ahSemaphore[0],1,NULL);
}


//**********************************************************************************************
void CAudio::SuspendAudioLoader()
{
	if (bThreadLocked)
	{
		// Just a warning assert, multiple suspends should not happen
		Assert(0);
		return;
	}
	bThreadLocked = true;
	WaitForSingleObject(ahSemaphore[0],INFINITE);
}
