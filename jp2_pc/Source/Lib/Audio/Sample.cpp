/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CSample 
 *
 * Bugs:
 *
 * Notes:
 *
 * A CSample is the lowest level of storage for a sample.
 * All the sample members check to see if pDSBuffer is NULL, if it is it means that the class
 * as no DS buffer attached to it. This is not an error condition because a machine with no
 * sound will operate like this. When this is the case as little as possible is done before
 * exiting the functions.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/Sample.cpp                                                  $
 * 
 * 26    10/02/98 3:50a Rwyatt
 * All samples are checked for removal from the pseudo list.
 * 
 * 25    9/29/98 11:44p Rwyatt
 * Fixed typo in set volume call.
 * 
 * 24    9/28/98 5:02a Rwyatt
 * All audio is now defer loaded
 * 
 * 23    9/23/98 12:09p Rwyatt
 * Added changes suggested by CRL to audio init.
 * Buffer sizes are now rounded to the sample block size
 * Added support for NULL samples while audio is enabled
 * 
 * 22    9/12/98 10:33p Rwyatt
 * Sample defer callback function is set before the sample is added to the loader, in case it
 * calls the funtion immediately
 * 
 * 21    9/01/98 3:04p Rwyatt
 * Better inirial values, fustrum and outside volume are now kept in the sample.
 * Restarting streamed audio now works
 * 
 * 20    8/25/98 4:47p Rwyatt
 * Adjusted sample init order for systesm with no sound card
 * 
 * 19    8/23/98 8:03p Rwyatt
 * With no audio only add samples to the streamer if they are streamed!
 * 
 * 18    8/23/98 2:23a Rwyatt
 * Sample processing has been modifed when audio is disabled.
 * 
 * 17    8/21/98 5:59p Rwyatt
 * Subtitles are no longer created in the sample but on the first stream callback.
 * 
 * 16    8/21/98 2:30a Rwyatt
 * Adjustments for the new type of streaming. No disk access is performed in the foreground.
 * 
 * 15    8/04/98 3:58p Rwyatt
 * Streamed buffers contain no initial data, the buffer is set to zero.
 * TEMP: Sub titles are disabled
 * 
 * 14    6/08/98 12:55p Rwyatt
 * Added master volume support
 * 
 * 13    5/06/98 3:36p Rwyatt
 * Buffer lock fails on sample create are now handled.
 * New function to set/reset the transfer function of a sample.
 * 
 * 12    4/27/98 7:25p Rwyatt
 * New member in CSample for max volume distance on pseudo and 3D samples.
 * 
 * 11    3/22/98 5:02p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 10    3/09/98 10:54p Rwyatt
 * New hash function for sound identifiers, removes slash and case sensitivity.
 * 
 * 9     2/21/98 5:56p Rwyatt
 * Chnaged instance code to return psamINSTANCE_FAILED if we cannot create an instance.
 * 
 * 8     2/03/98 2:28p Rwyatt
 * Temp check in.
 * 
 * 7     12/17/97 2:57p Rwyatt
 * Removed debug new
 * Cross instancing of spatial types
 * 
 * 6     11/26/97 2:05p Rwyatt
 * Instance function now checks the deferred load flag properly and does not always return
 * psamINSTANCE_DEFER_LOAD
 * 
 * 5     11/22/97 10:44p Rwyatt
 * Uses new global audio pointer which is a static member of the CAudio class, therefore there
 * can be only one.
 * Deferred loading with maximum of 100K chunks is implemented, this results in slight chnages
 * to constructor and destructor. Also slight changes to every member to check the deferred flag
 * before operating.
 * 
 * 4     11/18/97 9:39p Rwyatt
 * Made string hash function covert to upper case
 * 
 * 3     11/18/97 3:19p Rwyatt
 * If the sample constructor failes to create a DS buffer we return and do not try to use it.
 * 
 * 2     11/14/97 7:20p Rwyatt
 * Moved empoty contructor into cpp file so memory logs could be added.
 * 
 * 1     11/13/97 11:43p Rwyatt
 * Initial check in of new sample implementation. This has been split off from audio.cpp which
 * just contains the base audio system.
 * Samples are now constructed with a loader class which will load the data, samples know
 * nothing about data compression and all loading is done through the abstract loader. Therefore
 * compressed samples can be transparently loaded. Non streaming samples will delete the loader
 * when they have finished constructing, streamed samples will leave the loader until the sample
 * is deleted at which point the loader is deleted.
 * The type of sample is now transparent so you can have a compressed streamed 3D sample
 * attached to an object.
 * 
 ***********************************************************************************************/

// DO NOT INCLUDE COMMON.HPP IN THIS FILE BECAUSE IT IS USED EXTERN TO THE GUIAPP (IN ALL THE
// AUDIO TOOLS) AND I DO NOT WANT TO INCLUDE HALF OF THE PROJECT.
#include "Audio.hpp"
#include "Lib/Sys/MemoryLog.hpp"


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
// CSample Implementation
//
CSample::CSample
(
	CCAULoad*		pcau,
	uint32			u4_flags,
	TDeferCallback	pdlcbfn,
	void*			pv_user
)
//	
//**************************************
{
	uint32		u4_buffer_length;
	HRESULT		hr;

	MEMLOG_ADD_COUNTER(emlSoundControl,sizeof(CSample));	

	// init important variables
	pdlcbfnCallback			= NULL;
	pvUser					= NULL;
	pDSBuffer				= NULL;
	pDS3DBuffer				= NULL;
	pasubSubtitle			= NULL;
	u4CreateFlags			= u4_flags;
	u4Length				= 0;
	fMasterVolume			= 0.0f;
	fVolume					= 0.0f;				// maximum volume
	fWorldX					= 0.0f;
	fWorldY					= 0.0f;				// default world positon is the origin
	fWorldZ					= 0.0f;
	fAtten					= 1.0f;				// default attenuation is 1db per meter
	fMaxVolDistance			= 0.0f;				// Attenuation starts at the source of the sample
	fFustrumAngle			= 360.0f;
	fOutsideVolume			= 0.0f;

	if (pcau == NULL)
	{
		pcauSample = NULL;
		return;
	}

	// calculate the play time of the sample...
	fPlayTime				= (float)pcau->u4DecompressedLength() / (float)pcau->u4BytesPerSecond();

	if ((CAudio::pcaAudio->bAudioActive()==false) || (u4_flags & AU_CREATE_NULL))
	{
		if ((u4_flags & AU_CREATE_STREAM) == 0)
		{
			// delete the loader if the sample is not streamed...
			delete pcau;
			pcauSample = NULL;
		}
		else
		{
			pcauSample = pcau;
		}
		return;
	}


	if (u4_flags & AU_CREATE_STREAM)
	{
		// if we are streaming a file then the buffer length is one second of audio and we keep
		// a copy of the loader class so we can get more data.
		u4_buffer_length = (float) (fSTREAM_LENGTH * 
							(float)(pcau->u4Frequency() * pcau->u4BytesPerSample() * pcau->escChannels()) );

		// a 3D buffer is 1.5 times the normal buffer length
		if (u4_flags & AU_CREATE_SPATIAL_3D)
		{
			u4_buffer_length= (uint32) ((float)u4_buffer_length * 1.5f);
		}
	}
	else
	{
		// if we are not streaming then we load the whole thing and set the loader class to NULL,
		// it is deleted below as it is no longer needed.
		u4_buffer_length = pcau->u4DecompressedLength();
	}

	pcauSample = pcau;

	// these are unused by static samples
	u4StreamLoadPosition	= 0;				// the next buffer offset to load at
	u4LastPlayCsr			= 0;				// last offset of the play cursor
	u4BytesPlayed			= 0;				// the number of actual bytes sent to the DAC
	u4Length				= u4_buffer_length;	// length in bytes of the sound buffer

	// set the standard format word with frequency, bits and channels in the same DWORD
	u4Format				= pcau->u4Frequency();
	u4Format				|= (pcau->escChannels()==escMONO)?0:AU_FORMAT_STEREO;
	u4Format				|= (pcau->esbBits()==esb8BIT)?0:AU_FORMAT_16BIT;

	MEMLOG_ADD_COUNTER(emlSoundSamples,u4_buffer_length);

	pDSBuffer = CAudio::pcaAudio->CreateDSBuffer(u4_buffer_length,pcau->u4Frequency(),pcau->escChannels(),pcau->esbBits(),u4_flags);

	if (pDSBuffer == NULL)
	{
		pDSBuffer = NULL;
		pDS3DBuffer = NULL;
		u4Length = 0;
		u4CreateFlags = 0;
		delete pcau;
		pcauSample = NULL;
		return;
	}

	// are we creating a Real Direct Sound 3D sample??
	if (u4_flags & AU_CREATE_SPATIAL_3D)
	{
		Assert ((u4_flags & AU_CREATE_PSEUDO_3D)==0);
		Assert ((u4_flags & AU_CREATE_STEREO)==0);

		// yes we are so we need to derive the 3D interface for it
		hr=pDSBuffer->QueryInterface(IID_IDirectSound3DBuffer, (void**)&pDS3DBuffer);
		if (hr != DS_OK)
		{
			// if we failed to create a 3D buffer delete the 2D buffer and make a shell sample...
			pDSBuffer->Release();
			pDSBuffer = NULL;
			pDS3DBuffer = NULL;
			u4Length = 0;
			u4CreateFlags = 0;
			delete pcau;
			pcauSample = NULL;
			return;
		}
	}
	else if (u4_flags & AU_CREATE_PSEUDO_3D)
	{		
		Assert ((u4_flags & AU_CREATE_STEREO)==0);
		CAudio::pcaAudio->AddPseudoSample(this);
	}


	//
	// Only load the data when nothing else can go wrong
	//

	if (u4CreateFlags & (AU_CREATE_DEFER_LOAD|AU_CREATE_DEFER_LOAD_ON_PLAY))
	{
		// we are on a deferred load so add the number of bytes to the loader thread,
		// do not delete the sample loader because we have not finished with it yet.

		// this will fill the sample with a full buffer of data, when finished it will
		// mark the sample as non-deferred so the rest of the system can act on it.

		// Set the callback function before we defer load the sample
		pdlcbfnCallback		= pdlcbfn;
		pvUser				= pv_user;

		// we are not yet loading this sample
		if (u4CreateFlags & AU_CREATE_DEFER_LOAD_ON_PLAY)
		{
			u4CreateFlags &= ~(AU_CREATE_DEFER_LOAD|AU_CREATE_DEFER_PLAY_ON_CALLBACK);
		}
		else
		{
			u4CreateFlags &= ~AU_CREATE_DEFER_LOAD_ON_PLAY;
			CAudio::pcaAudio->AddDeferredSampleFill(this);
		}
	}
	else 
	{
		// We are loading a static buffer......
		if ((u4_flags & AU_CREATE_STREAM)==0)
		{
			// lock the whole buffer.
			Lock(0,u4_buffer_length);
			uint8* pu1_buffer = pu1Lock1Buffer();
			
			//
			// Check we have a valid buffer before we load into it, this will save a page fault.
			//
			if (pu1_buffer == NULL)
			{
				Unlock();
				// We have failed to lock the buffer
				pDS3DBuffer->Release();
				pDS3DBuffer = NULL;
				pDSBuffer->Release();
				pDSBuffer = NULL;
				u4Length = 0;
				u4CreateFlags = 0;
				delete pcau;
				return;
			}

			pcau->u4LoadSampleData( pu1_buffer, u4Lock1BufferLength(), false );

			Unlock();

			// if we are not streaming then we can delete the loader
			delete pcau;
		}
	}
}



//**********************************************************************************************
//
CSample::CSample
(
)
//	
//**************************************
{
	MEMLOG_ADD_COUNTER(emlSoundControl,sizeof(CSample));	
}



//**********************************************************************************************
//
CSample::~CSample
(
)
//	
//**************************************
{
	Assert( !bSampleError(this) );

	MEMLOG_SUB_COUNTER(emlSoundControl,sizeof(CSample));

	// this can be NULL if there is no sound in the machine.
	if (pDSBuffer)
	{
		// remove this sample from the deferred load list, if it is not there who cares
		CAudio::pcaAudio->RemovePseudoSample(this);
		CAudio::pcaAudio->RemoveDeferred(this);
		CAudio::pcaAudio->StopFade(this);

		Stop();					// this will remove the sample from the threader

		if (u4CreateFlags & (AU_CREATE_STREAM|AU_CREATE_DEFER_LOAD|AU_CREATE_DEFER_LOAD_ON_PLAY))
		{
			// this is a streamed sample, (which is now not playing) or this is
			// a deferred sample which as not finished loading.
			// Delete the loader for this stream, in any case the sample has been detatched from
			// the background loader.
			delete pcauSample;
		}

		// release the 3D interface for this sample if it is present
		if (pDS3DBuffer)
		{
			pDS3DBuffer->Release();
			pDS3DBuffer = NULL;
		}

		// release the 2D sample interface
		pDSBuffer->Release();
		pDSBuffer = NULL;

		// keep track of the memory
		MEMLOG_SUB_COUNTER(emlSoundSamples,u4Length);
	}
	else
	{
		if (u4CreateFlags & AU_CREATE_STREAM)
		{
			CAudio::pcaAudio->RemoveStreamedSample(this);

			if (pcauSample)
			{
				// if the loader is still present then delete it
				delete pcauSample;
			}
		}
	}

	// we delete the subtitle even if there is no audio buffer
	delete pasubSubtitle;
}



//**********************************************************************************************
//
bool CSample::bPlay(uint32 u4_play_flags)
//	
//**************************************
{
	Assert( !bSampleError(this) );


	// if we are still loading do nothing
	if (u4CreateFlags & AU_CREATE_DEFER_LOAD)
		return false;

	// set just the looped bit if it is specifed, always clear the resume bit as it is a control
	// bit and does not require to be stored.
	u4CreateFlags &=~ (AU_PLAY_LOOPED|AU_PLAY_RESUME);
	u4CreateFlags |= (u4_play_flags & AU_PLAY_LOOPED);


	// If the sample pointer is NULL there is no audio.. This means that either the machine
	// has no audio or the sample was created with AU_CREATE_NULL. In either case if this
	// is a streamed sample you need to add it to the streamer so it can process the sub-title.
	if (pDSBuffer == NULL)
	{
		if ( ((CAudio::pcaAudio->bAudioActive() == false) || (u4CreateFlags & AU_CREATE_NULL)) && bSampleStreamed() )
		{
			CAudio::pcaAudio->bAddStreamedSample(this);
		}
		return true;	
	}

	// if this sample is marked to defer load on play the do just that....
	if (u4CreateFlags & AU_CREATE_DEFER_LOAD_ON_PLAY)
	{
		u4CreateFlags |= AU_CREATE_DEFER_LOAD;
		u4CreateFlags &= ~AU_CREATE_DEFER_LOAD_ON_PLAY;
		u4CreateFlags |= AU_CREATE_DEFER_PLAY_ON_CALLBACK;		// play when we have finished loading

		CAudio::pcaAudio->AddDeferredSampleFill(this);
		return true;
	}

	// if this sample was created as a stream then we need to act differently...
	if (bSampleStreamed())
	{
		//
		// if we have started playing the stream, reset it to the beginning. DO NOT change the
		// stream position pointers because you will cause a glitch. The effect of restarting
		// a stream will not be heard for 1 buffer period. The effect can be made instant but
		// a blocking read operation will be required.
		//

		// Only reset the pointer if we are NOT resuming play, ie we are not continuing after
		// a pause.
		if ((u4_play_flags & AU_PLAY_RESUME) == 0)
		{
			// the buffer is partly played, we need to initialize it again with a buffer
			// of valid data.. This will have to block
			if (u4BytesPlayed>0)
			{
				u4BytesPlayed			= 0;
				u4StreamLoadPosition	= 0;				// the next buffer offset to load at
				u4LastPlayCsr			= 0;				// last offset of the play cursor

				pcauSample->ResetToStartPosition();
				pDSBuffer->SetCurrentPosition(0);

				// clear the stream init flag so the stream will load when it next gets to this
				// sample.
				u4CreateFlags &=~ AU_STREAM_INIT;
			}
		}

		if (CAudio::pcaAudio->bAddStreamedSample(this) == false)
		{
			// if we failed to add the stream to the active stream list stop it...
			pDSBuffer->Stop();
			return false;
		}

		// If we are resuming play, now we have added the sample to the stream list, play if from w
		// here it currently is.
		if (u4_play_flags & AU_PLAY_RESUME)
		{
			if (pDSBuffer->Play(0,0,DSBPLAY_LOOPING) != DS_OK)
			{
				CAudio::pcaAudio->RemoveStreamedSample(this);
				return false;
			}
		}
	}
	else
	{
		// if we are not resuming play then set play cursor to the start
		if ((u4_play_flags & AU_PLAY_RESUME) == 0)
		{
			pDSBuffer->SetCurrentPosition(0);
		}

		if (pDSBuffer->Play(0,0,u4_play_flags&AU_PLAY_LOOPED?DSBPLAY_LOOPING:0) != DS_OK)
		{
			return false;
		}
	}

	return true;
}


//**********************************************************************************************
//
void CSample::SetTransfer
(
	const SSoundTransfer&	st,
	float					f_velocity, 
	float					f_x,
	float					f_y,
	float					f_z,
	bool					b_commit
)
//**************************************
{
	Assert( !bSampleError(this) );

	if (pDSBuffer == NULL)
		return;

	// set the DirectSound Pitch
	float	f_pitch		= st.fCalculatePitch(f_velocity);

	// scale the pitch to a multiplying factor that can be applied to the frequency
	if (f_pitch < 0.0f)
	{
		f_pitch = 1.0f + (f_pitch * 0.75f);
	}
	else
	{
		f_pitch = (f_pitch * 3.0f) + 1.0f;
	}

	SetFrequency(f_pitch);
	SetVolume( -40.0f + (st.fCalculateVolume(f_velocity)*40.0f) );
	SetPosition(f_x,f_y,f_z, b_commit);
	SetAttenuation(st.fAttenuate,b_commit);
}


//**********************************************************************************************
//
bool CSample::bPlay
(
	const SSoundTransfer&	st,
	float					f_velocity, 
	float					f_x,
	float					f_y,
	float					f_z, 
	uint32					u4_play_flags
)
//	
//**************************************
{
	Assert( !bSampleError(this) );

	if (pDSBuffer == NULL)
		return true;

	// set the transfer now
	SetTransfer(st,f_velocity,f_x,f_y,f_z,true);

	return bPlay(u4_play_flags);
}



//**********************************************************************************************
// Position a sound, this can be used to move a sound while it is playing
//
void CSample::SetPosition
(
	float f_x,
	float f_y,
	float f_z,
	bool  b_immediate
)
//	
//**************************************
{
	Assert( !bSampleError(this) );

	// if there is no sample then don't bother with the position because you cannot play it 
	// when you have set its position.
	if (pDSBuffer == NULL)
		return;

	fWorldX = f_x;
	fWorldY = f_y;
	fWorldZ = f_z;

	if (pDS3DBuffer == NULL)
	{
		if (b_immediate)
		{
			if ((u4CreateFlags & AU_CREATE_STEREO) == 0)
			{
				// the sample is a normal sample position it ourselves
				SetVolume( fCalculateVolumeAttenuation() );
			}
		}
	}
	else
	{
		// use D3D Sound to position the samples.....
		pDS3DBuffer->SetPosition(f_x,f_y,f_z,b_immediate?DS3D_IMMEDIATE:DS3D_DEFERRED);
	}
}



//**********************************************************************************************
// set the orientation of a sound, this only has a meaning if the sound cone projection angle
// has been set, by default the angle is 360 degrees and therefore setting the orientation will
// do nothing.
//
void CSample::SetOrientation
(
	float f_x,
	float f_y,
	float f_z,
	bool  b_immediate
)
//	
//**************************************
{
	Assert( !bSampleError(this) );

	// if there is no 3D interface or no sample interface then just ignore the call, this will
	// becomes a no-op on stereo and pseudo 3D buffers

	if ((pDSBuffer == NULL) || (pDS3DBuffer == NULL))
		return;


	// use D3D Sound to position the samples.....
	pDS3DBuffer->SetConeOrientation(f_x,f_y,f_z,b_immediate?DS3D_IMMEDIATE:DS3D_DEFERRED);
}




//**********************************************************************************************
// The frutum of a sample is defined to be a cone covering f_angle degress of arc centered
// around the orientation vector. The volume specified is the volume on the outside of the code
// which is constant for all positions not within the cone. The volume fades towards maximum
// (0 dB) as a function of angular displacement from the orientation vector.
//
void CSample::SetFrustum
(
	float f_angle,			// angular displacement of the cone in degrees
	float f_volume,			// volume in dBs (0 = full volume -100dB silent) outside the cone
	bool  b_immediate
)
//	
//**************************************
{
	Assert( !bSampleError(this) );

	// angle must be in range
	Assert(f_angle>0.0f);
	Assert(f_angle<360.0f);

	// volume must in between 0 and -100 dBs..
	Assert(f_volume>=-100.0f);
	Assert(f_volume<=0.0f);

	// If there is no sound interface then sound is probably disabled so return,
	if (pDSBuffer == NULL)
		return;

	// we must be a real 3D sample
	Assert (u4CreateFlags & AU_CREATE_SPATIAL_3D);

	// there must be a 3D buffer
	Assert(pDS3DBuffer != NULL);

	// set the angle of the sound cone
	pDS3DBuffer->SetConeAngles( (uint32)(f_angle*0.3f), (uint32)f_angle,
												b_immediate?DS3D_IMMEDIATE:DS3D_DEFERRED);

	// set the outside volume of the cone
	pDS3DBuffer->SetConeOutsideVolume( (long)(f_volume*100.0f), 
												b_immediate?DS3D_IMMEDIATE:DS3D_DEFERRED);

	fFustrumAngle	= f_angle;
	fOutsideVolume	= f_volume;
}




//**********************************************************************************************
// attenuation is in dB/m (1.0 = 1dB/m therefore half volume will be at 6 meters)
//
void CSample::SetAttenuation
(
	float f_atten, 
	float f_min,
	bool b_immediate
)
//	
//**************************************
{
	Assert( !bSampleError(this) );

	if (pDSBuffer == NULL)
		return;

	// cannot be a stereo sample
	Assert (u4CreateFlags & (AU_CREATE_SPATIAL_3D|AU_CREATE_PSEUDO_3D));

	fAtten = f_atten;
	fMaxVolDistance = f_min;

	if (pDS3DBuffer==NULL)
	{
		// the sample is a normal sample position it ourselves
		if (b_immediate)
		{
			SetVolume( fCalculateVolumeAttenuation() );
		}
	}
	else
	{
		//
		// The fTHREE_DBS/f_atten in the line below may seem a little strange 
		// so below is an explanation.
		//
		// DirectSound has no attenuation on a per sample basis but it does have a minimim distance
		// which is the point at which the audio gets no louder and at twice this distance it is at
		// half volume (-6.0205dBs or 20*log10(2) ). So from this is can be derived that the minimum
		// distance for a given roll off is (20*log10(2)) / dbm / 2.0 = 10*log10(2) / dbm, where
		// dbm is decibels per meter. 10*log10(2) = 3.010299957 which is fTHREE_DBS
		//

		//
		// Read 3D samples ignore the f_min parameter
		//
		pDS3DBuffer->SetMinDistance(fTHREE_DBS/f_atten,b_immediate?DS3D_IMMEDIATE:DS3D_DEFERRED);
	}
}





//**********************************************************************************************
//
void CSample::SetFrequency
(
	float f_freq
)
//	
//**************************************
{
	Assert( !bSampleError(this) );

	if (pDSBuffer == NULL)
		return;

	Assert(u4CreateFlags & AU_CREATE_CTRL_FREQUENCY);
	Assert(f_freq>0.0f);

	pDSBuffer->SetFrequency( (uint32) ((float)(u4Format & AU_FORMAT_FREQ) * f_freq) );
}





//**********************************************************************************************
//
void CSample::SetVolume
(
	float f_vol
)
//	
//**************************************
{
	Assert( !bSampleError(this) );

	if (pDSBuffer == NULL)
		return;

	// volume must in between 0 and -100 dBs..
	Assert(f_vol>=-100.0f);
	Assert(f_vol<=0.0f);
	Assert(u4CreateFlags & AU_CREATE_CTRL_VOLUME);

	// Adjust for the master volume
	f_vol += fMasterVolume;

	if (f_vol>0.0f)
		f_vol = 0.0f;

	if (f_vol<-100.0f)
		f_vol = -100.0f;

	if (pDSBuffer->SetVolume((int32)(f_vol*100.0f)) == DS_OK)
	{
		// If the call returns no error set the class copy of the volume
		fVolume = f_vol;
	}
}


//**********************************************************************************************
//
void CSample::Stop()
{
	Assert( !bSampleError(this) );

	// if no buffer then do nothing
	if (pDSBuffer == NULL)
		return;

	// If this sample is currently loading as a deferred load then make sure that its auto
	// play flag is cleared. Therefore the stop will be honored.
	// As we actually change the flags we need to take the crtical section
	CAudio::pcaAudio->LockDefer();
	if (u4CreateFlags & AU_CREATE_DEFER_LOAD)
	{
		Assert((u4CreateFlags & AU_CREATE_STREAM)==0);
		u4CreateFlags &=~ AU_CREATE_DEFER_PLAY_ON_CALLBACK;
		CAudio::pcaAudio->UnlockDefer();
		return;
	}
	CAudio::pcaAudio->UnlockDefer();

	if (u4CreateFlags & AU_CREATE_STREAM)
	{
		CAudio::pcaAudio->RemoveStreamedSample(this);
	}

	// stop the buffer from playing
	pDSBuffer->Stop();
}


//**********************************************************************************************
// create another CSample that is an instance of this one...
// Thre return value is either NULL or a psamXXX_XXX value for a specific error.
//
CSample* CSample::psamCreateInstance
(
	uint32	u4_instance_type		// same flags as the sample contructor takes or 0
)
{
	Assert( !bSampleError(this) );

	uint32					u4_create_flags;
	CSample*				psam_new = NULL;
	LPDIRECTSOUNDBUFFER		pds_instance = NULL;
	LPDIRECTSOUND3DBUFFER	pds3d_instance = NULL;
	HRESULT					hr;

	// if the instance flags are zero, get the creation flags of this sample as a reference but
	// strip out its control type.
	if (u4_instance_type == 0)
	{
		u4_create_flags = u4CreateFlags;
	}
	else
	{
		// an instance type was passed in so copy the other flags from this sample but adjust the spatial
		// type to be the reqested type.
		u4_create_flags = u4_instance_type | (u4CreateFlags &~AU_CREATE_TYPE_MASK);
	}

	// if this is shell sample then just return NULL and let the caller handle it!
	if (pDSBuffer == NULL)
	{
		return NULL;
	}

	// If this sample is a streaming sample then we cannot instance it and we must return NULL.
	// To duplicate a streaming buffer has no meaning because the data is shared which means
	// that the samples cannot be played independently of each other.
	// Also, the loader class cannot be instanced so another one would have to be created.
	// If we return NULL the caller should recreate the sample and the loader.
	//
	if (u4CreateFlags & (AU_CREATE_STREAM|AU_CREATE_MAPPED))
		return NULL;

	// if the sample is currently under deferred loading then return a special handle so
	// the caller knows that they can try again later or they can reload the sample.
	CAudio::pcaAudio->LockDefer();
	if (u4CreateFlags & AU_CREATE_DEFER_LOAD)
	{
		CAudio::pcaAudio->UnlockDefer();
		return psamINSTANCE_IGNORE;
	}
	CAudio::pcaAudio->UnlockDefer();

	// before we new the memory for the CSample make sure we can create a duplicate DS buffer
	// we must be really careful in handling the error code because if a LOCHARDWARE buffer
	// is duplicated and there is no more hardware resources the call will fail. It is up to
	// us to create a software version.
	hr = CAudio::pcaAudio->pdsGetDSInterface()->DuplicateSoundBuffer(pDSBuffer, &pds_instance);

	// return a specific error code which for a failed instance. This should force the caller
	// to create a new sample.
	if (hr != DS_OK)
	{
		//dprintf("Duplicate sound buffer failed..\n");
		return psamINSTANCE_FAILED;
	}

	// if we are instancing to a 3D buffer then we need to query the new sound interface for
	// its 3D component.
	if (u4_create_flags & AU_CREATE_SPATIAL_3D)
	{
		hr=pds_instance->QueryInterface(IID_IDirectSound3DBuffer, (void**)&pds3d_instance);
		if (hr != DS_OK)
		{
			dprintf("Query 3D from duplicate sound buffer failed..\n");
			// query 3d failed, delete the 2D instance and return NULL to force the caller
			// to reload the sample
			pds_instance->Release();
			return NULL;
		}
	}


	// we now have new interfaces for 2D and possibly 3D components so it is safe to new
	// the class.

	// create an empty sample...
	psam_new = new CSample();

	// fill the contents of the class with useable data, most data will be copied from the
	// source sample.

	// data sahred fom the source sample
	psam_new->u4CreateFlags		= u4_create_flags;
	psam_new->u4Length			= u4Length;
	psam_new->fPlayTime			= fPlayTime;
	psam_new->u4Format			= u4Format;

	// DS interfaces
	psam_new->pDSBuffer			= pds_instance;
	psam_new->pDS3DBuffer		= pds3d_instance;

	// new a copy of the sub title if one exists, otherwise set the subtitle pointer to NULL
	if (pasubSubtitle)
	{
		psam_new->pasubSubtitle		= new CAudioSubtitle( (*pasubSubtitle) );
	}
	else
	{
		psam_new->pasubSubtitle = NULL;
	}

	// not instanced data..
	psam_new->fVolume			= 0.0f;		// default max volume
	psam_new->pcauSample		= NULL;		// no loader, we cannot be streamed
	psam_new->fAtten			= 1.0f;		// default attenuation of 1dB per meter
	psam_new->fWorldX			= 0.0f;	
	psam_new->fWorldY			= 0.0f;		// instanced sample is located at the origin
	psam_new->fWorldZ			= 0.0f;

	// if we are instancing to a pseudo 3D sample add it to the active list
	if (u4_create_flags & AU_CREATE_PSEUDO_3D)
	{
		CAudio::pcaAudio->AddPseudoSample(psam_new);
	}	

	return psam_new;
}




//*****************************************************************************************
// Standard hash function but converts to lower case
//
uint32 sndhndHashIdentifier(const char* str)
{
	char	str_local[1024];
	char*	pstr = str_local;

	Assert(str);
	Assert(strlen(str) > 0);
	Assert(strlen(str) < 1024);

	// Remove case sensitivity and forward/backward slash sensitivity.
	while (*str)
	{
		if (*str == '\\')
			*pstr = '/';
		else
		{
			*pstr = tolower(*str);
		}
		str++;
		pstr++;
	}
	*pstr = 0;
	pstr++;

	// hash the result
	return u4Hash(str_local,0,true);
}
