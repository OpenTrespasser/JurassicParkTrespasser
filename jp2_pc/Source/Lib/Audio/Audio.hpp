/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	Audio classes:
 *
 * Bugs:
 *
 * To do:
 *
 * NOTE:
 *			THIS FILE SHOULD NOT BE INCLUDED FROM ANYWHERE WITHIN THE PROJECT EXCEPT
 *			FROM WITHIN THE AUDIO SYSTEM OR GUIAPP.CPP.. IF IT IS INCLUDED IT WILL
 *			CAUSE BUILD ERRORS DUE TO THE DIRECTSOUND FILES.
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/Audio.hpp                                                   $
 * 
 * 78    10/02/98 3:53a Rwyatt
 * Changed an int 3 to an assert
 * 
 * 77    9/30/98 2:10a Rwyatt
 * Added bool for thread locked
 * 
 * 76    9/29/98 1:04a Rwyatt
 * Thread callback is now set to 80ms
 * 
 * 75    9/28/98 4:59a Rwyatt
 * All Audio is now defer loaded
 * 
 * 74    9/23/98 12:09p Rwyatt
 * Added support for NULL samples while audio is enabled
 * 
 * 73    9/21/98 3:32a Rwyatt
 * Loader thread cano now be paused. All clenup code has been modifed to handle this.
 * 
 * 72    98/09/17 17:47 Speter
 * Must include BuildVer for VER_TEST etc.
 * 
 * 71    9/15/98 8:40p Shernd
 * audio volume changes
 * 
 * 70    9/08/98 4:10a Rwyatt
 * New member for the dynamically loading of the sound DLL and a new interface pointer for the
 * A3D control interface.
 * 
 * 69    9/04/98 2:56a Rwyatt
 * New constructor for CAudio, new thread members
 * 
 * 68    9/01/98 3:05p Rwyatt
 * New data elements in CSample.
 * Assert in release mode if the pseudo 3D buffers overflow as this will corrupt the following
 * data.
 * 
 * 67    8/25/98 11:27a Rvande
 * Extra access specifier removed
 * 
 * 66    8/23/98 2:23a Rwyatt
 * Added support thread when audio is disabled. This thread allows sub titles to be created in
 * the background without blocking. Construction and destruction of samples has been changed to
 * give a data flow flow to that when audio is enabled.
 * 
 * 65    8/21/98 2:25a Rwyatt
 * Fixed streaming cutoff
 * The cleanup code now checks that audio is present.
 * 
 * 64    8/19/98 1:42p Rwyatt
 * Fixes to VC6.0 warnings
 * 
 * 63    98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 62    8/10/98 5:24p Rwyatt
 * Initial EAX settings now have no reverb
 * 
 * 61    8/04/98 3:57p Rwyatt
 * Support to set all EAX environment properties
 * 
 * 60    7/29/98 10:50a Rwyatt
 * Added initial EAX support
 * 
 * 59    7/20/98 10:24p Pkeet
 * Added headers to allow the library to compile.
 * 
 * 58    7/14/98 3:57p Rwyatt
 * Changed the number of fading samples
 * 
 * 57    6/08/98 12:53p Rwyatt
 * Samples now have a master volume.
 * 
 * 56    5/25/98 6:47p Pkeet
 * Added casts to allow the project to build.
 * 
 * 55    5/24/98 10:01p Rwyatt
 * New functions to return the presence of 3D audio hardware and the use of hardware.
 * 
 * 54    5/23/98 9:18p Rwyatt
 * Removed all traces of the load library code.
 * 
 * 53    5/09/98 5:35p Bbell
 * Attenuate from MaxVolDistance to edge of trigger,  rather than having a discontinuity at the
 * MaxVolDistance.
 * 
 * 52    5/06/98 3:35p Rwyatt
 * New CAudio memebers to hold the DLL handle of wither DSound.dll or A3D.dll. New CSample
 * function to set the transfer.
 * 
 * 51    4/29/98 7:00p Rwyatt
 * Changed the fade sample member function so that it takes a stop flag. The threaded fader now
 * sets a request stop flag instead of stopping the sample.
 * 
 * 50    4/27/98 7:26p Rwyatt
 * New data member in CSample and the deferred ambient structure for max volume distance.
 * 
 * 49    3/09/98 10:53p Rwyatt
 * New static function to create a sample through a sound handle and a database
 * 
 * 48    3/09/98 10:51p Rwyatt
 * Corrected bSampleError macro to account for new error messages
 * 
 * 47    2/21/98 5:54p Rwyatt
 * Added a new CSample value for a failed instance, psamINSTANCE_FAILED
 * 
 * 46    2/06/98 8:20p Rwyatt
 * CAudio quality settings are static so they survive the world database being destroyed.
 * 
 * 45    2/03/98 2:28p Rwyatt
 * Temp check in.
 * 
 * 44    1/07/98 5:19p Shernd
 * Added SetVolume to Primary Sound Buffer
 * 
 * 43    12/17/97 2:53p Rwyatt
 * CreateInstance can now instance samples from one spatial type to another, by default the
 * instance is the same.
 * 
 * 42    11/22/97 10:43p Rwyatt
 * Uses new global audio pointer which is a static member of the CAudio class, therefore there
 * can be only one.
 * Deferred loading with maximum of 100K chunks is implemented
 * 
 * 41    11/18/97 9:40p Rwyatt
 * temp check-in:
 * Added callback structure for deferred loading
 * 
 * 40    11/16/97 4:58a Rwyatt
 * Changed the MAX_PSEUDO3D_SAMPLES from 8 to 48, this value must be larger than
 * MAX_AMBIENT_SAMPLES defined in AudioDaemon.cpp
 * 
 * 39    11/14/97 7:20p Rwyatt
 * A new constructor used in sample instancing
 * 
 * 38    11/13/97 11:52p Rwyatt
 * New audio and sample classes for the new system
 * Mnay modifications, many new access members and lots more functionality.
 * 
 * 37    11/07/97 9:22a Shernd
 * Add member function to Expose the Direct Sound Interface for use with Smacker
 * 
 * 36    10/23/97 6:52p Rwyatt
 * New functions to control speaker configuration
 * 
 * 35    7/23/97 8:38p Rwyatt
 * New functions for finding a sample in the cache and for getting the last cache hash value
 * 
 * 34    7/14/97 12:27p Rwyatt
 * New audio library with a generic set of flags for creating any sample type.
 * 
 * 33    7/07/97 11:54p Rwyatt
 * Initial tidy up after E3 crunch, sample class still needs to be done
 * 
 * 32    6/13/97 6:06p Rwyatt
 * Sample cahce is now 48 to account for the duplicate samples that could get loaded.
 * 
 * 31    6/09/97 12:09p Rwyatt
 * CSample now has members to control Audio streaming,
 * 
 * 30    6/06/97 12:13p Rwyatt
 * New members to CSample to stop and loop samples. New structures and defines so that CAudio
 * can handle ambient loops
 * 
 * 29    6/04/97 7:02p Rwyatt
 * Added memeber for getting and setting the output format.
 * 
 * 28    6/02/97 4:52p Rwyatt
 * Added support for a Listener and relative path names,
 * 
 * 27    5/29/97 8:31p Rwyatt
 * Added DirectX path to DirectX includes
 * 
 * 26    5/29/97 6:20p Rwyatt
 * 
 * 25    5/29/97 4:17p Rwyatt
 * DirectSound audio support.
 * 
 * 24    5/25/97 8:31p Rwyatt
 * Removed all of Brandon's RSX code and renamed AudioDeemone to AudioDaemon.
 * 
 ***********************************************************************************************/

#ifndef HEADER_LIB_AUDIO_AUDIO_HPP
#define HEADER_LIB_AUDIO_AUDIO_HPP


// DO NOT INCLUDE COMMON.HPP IN AUDIO FILES,
// ALL OTHER AUDIO FILES SHOULD INCLUDE AUDIO.HPP INSTEAD
#pragma warning(disable:4237)
#pragma warning(disable:4786)

#include "buildver.hpp"
#include "SoundTypes.hpp"
#include "SoundDefs.hpp"
#include "dsound.h"
#include "Ia3d.h"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Sys/MemoryLog.hpp"
#include <string>
#include "AudioLoader.hpp"
#include "Subtitle.hpp"
#include <math.h>

//**********************************************************************************************
//

// the number of seconds of in memory sound for a streamed sample
#define iSTREAM_BUFFER_LENGTH	1

// the maximum audio streams
#define MAX_CONCURRENT_STREAMS	4

// the maximum number of pseudo 3D sounds that can exist at once, not all of these can be played
#define MAX_PSEUDO3D_SAMPLES	32

// the maximum number of samples that can be faded at a time
#define MAX_FADING_SAMPLES		1			// Only music can fade at this time

// the maximum number of deferred actions that can be present at any given time
#define MAX_DEFERRED_OPS		16

// length in seconds of a streaming buffer
#define fSTREAM_LENGTH			(1.5f)

// the period in ms of the callback thread, the value here should be carefully choosen based
// on the following factors. The length of the streaming buffers (defined above), the number of
// streams that are likely to be playing at once and complexity of the stream. If 1 call back is
// done every second, 4 streams are playing and the streams are all compressed then at every
// callback a total of 4 seconds needs to be loaded and decompressed which may have an effect
// on the frame rate.
#define iSTREAM_CALLBACK_PERIOD	(80)		// milliseconds

// half of half power in dBs or 20*log10(2)/2
#define fTHREE_DBS		(3.010299957f)


//**********************************************************************************************
// forward declarations
//
class CSample;



//*********************************************************************************************
// This is returned as the sample handle from the instance functions if the source of the 
// instance is still loading
//
#define psamINSTANCE_IGNORE			(CSample*)0x00000001

//**********************************************************************************************
// This is passed to the callback function if a deferred sample is deleted. The callback may or
// may not care about this but it probably needs to free allocated memory.
//
#define psamDEFER_LOAD_DELETE		(CSample*)0x00000002

//*********************************************************************************************
// This is returned by the create instance function when we failed to create the instance.
//
#define psamINSTANCE_FAILED			(CSample*)0x00000003

//*********************************************************************************************
// This is returned by the create instance function when we failed to create the instance.
//
#define bSampleError(psam)			( (((uint32)(psam))<=(0x00000003)) && ((psam)!=0) )



//**********************************************************************************************
// define the callback function for defered load operations
//
typedef void (*TDeferCallback)(CSample*, void*);
// prefix: dlcbfn
//
//**************************************



//**********************************************************************************************
// structure that controls fading audio...
//
struct SAudioFade
// prefix: af
//
//**************************************
{
	CSample*	psam;
	float		fDeltaVol;
	bool		bStopAtMinimum;
};


//**********************************************************************************************
// structure that controls the EAX enviroment. This is binary compatible with the 
// EAX_REVERBPROPERTIES defined in EAX.h
//
struct SAudioEnvironment
// prefix: aenv
//
//**************************************
{
    uint32		u4Environment;	// 0 to EAX_ENVIRONMENT_COUNT-1
    float		fVolume;		// 0 to 1
    float		fDecayTime;		// seconds, 0.1 to 100
    float		fDamping;		// 0 to 1
};


//**********************************************************************************************
struct SStreamedSample
// prefix: ssam
{
	CSample*	psam;
	uint32		u4StopTick;		// only used when audio is disabled.
};


//**********************************************************************************************
#define u4SEMAPHORE_COUNT	2

//**********************************************************************************************
//
// CAudio
//

class CAudio
// prefix: au
{
public:
	//*****************************************************************************************
	// constructors and deconstrutors
	//

	//*****************************************************************************************
	//
	CAudio
	(
		void*	h_wnd,
		bool	b_enable,		// set to false and the game will run without audio
		bool	b_hardware		// should we use 3D audio hardware
	);

	//*****************************************************************************************
	//
	~CAudio();


	//*****************************************************************************************
	// Create the required audio COM objects
	//
	void CreateAudio
	(
		void*	h_wnd,
		bool	b_enable
	);


	//*****************************************************************************************
	//
	void DestroyAudio(bool b_shutdown);


	//*****************************************************************************************
	//
	void SetBasePathName(const char* str_path);


	//*****************************************************************************************
	//
	void AudioEnable(bool b_enable);


	//*****************************************************************************************
	//
	void SetNoAudio();


	//*****************************************************************************************
	//
	bool bAudioActive()
	{
		return bSoundEnabled;
	}


	//*****************************************************************************************
	//
	std::string& GetBasePathName()
	{
		return strBasePath;
	}


	//*****************************************************************************************
	//
	bool bCanChangeFormat()
	{
		return bChangeFormat;
	}


	//*****************************************************************************************
	//
	void SetOutputFormat
	(
		uint32			u4_frequency,
		ESoundChannels	esc,
		ESoundBits		esb
	);


	//*****************************************************************************************
	//
	ESpeakerConfig esconGetSpeakerConfig();


	//*****************************************************************************************
	//
	void SetSpeakerConfig
	(
		ESpeakerConfig escon_speakers
	);


	//*****************************************************************************************
	//
	uint32 u4GetFormat()
	{
		return u4Format;
	}


	//*****************************************************************************************
	//
	bool bAddAmbient
	(
		CSample* psam,		// sample to use for ambient, must be looping
		float f_x,			// x pos in world
		float f_y,			// y pos in world (up)
		float f_z,			// z pos in world (into screen)
		float f_atten		// attenuation in 100ths of a dB per meter.
	);


	//*****************************************************************************************
	// sample creation functions
	//
	CSample* CreateSample
	(
		const char* str_fname,
		uint32		u4_flags
	);


	//*****************************************************************************************
	//
	void PositionListener(float f_x,float f_y,float f_z, bool b_immediate = true);


	//*****************************************************************************************
	//
	void OrientListener(float in_x,float in_y,float in_z,
						float up_x,float up_y,float up_z, bool b_immediate = true);


	//*****************************************************************************************
	//
	void CommitSettings();


	//*****************************************************************************************
	// Create a raw direct sound buffer
	//
	LPDIRECTSOUNDBUFFER	CreateDSBuffer
	(
		uint32			u4_bytes,
		uint32			u4_frequency,
		ESoundChannels	esc_channels,
		ESoundBits		esb_bits,
		uint32			u4_create_flags
	);


	//******************************************************************************************
	//
	static CSample* psamCreateSample
	(
		const char*		str_fname,
		uint32			u4_flags,
		TDeferCallback	pdlcbfn = NULL,
		void*			pv_user = NULL
	);

	
	//******************************************************************************************
	// Memory constructors do not have a callback parameter
	//
	static CSample* psamCreateSample
	(
		TSoundHandle	sndhnd_sample,
		CAudioDatabase*	padat,
		uint32			u4_flags,
		TDeferCallback	pdlcbfn = NULL,
		void*			pv_user = NULL
	);


	//******************************************************************************************
	// Add the specifed sample as a stream to active stream list
	//
	bool bAddStreamedSample(CSample*);


	//******************************************************************************************
	// Remove the specifed sample as a stream to active stream list
	//
	void RemoveStreamedSample(CSample*);

	//******************************************************************************************
	void ThreadProcessNoAudioSample
	(
		CSample*	psam,		// the sample
		int32		i4_count	// the index of this sample in the stream array
	);

	//******************************************************************************************
	// functions to return the position in world space of the listener
	//
	float fGetListenerX()
	{
		return fListenerX;
	}

	float fGetListenerY()
	{
		return fListenerY;
	}

	float fGetListenerZ()
	{
		return fListenerZ;
	}


	//******************************************************************************************
	// return the distance to the listener.
	//
	float fGetSquaredListenerDistance
	(
		float f_x,
		float f_y,
		float f_z
	)
	{
		return ((f_x - fListenerX)*(f_x - fListenerX)) +
					((f_y - fListenerY)*(f_y - fListenerY)) +
					((f_z - fListenerZ)*(f_z - fListenerZ));
	}


	//******************************************************************************************
	//
	void AddPseudoSample(CSample* psam)
	{
		for (uint32 u4=0 ; u4<MAX_PSEUDO3D_SAMPLES; u4++)
		{
			if (apsamPseudo[u4] == NULL)
			{
				apsamPseudo[u4] = psam;
				return;
			}
		}

#if VER_TEST
		AlwaysAssert(0);
#endif
	}


	//******************************************************************************************
	//
	void RemovePseudoSample(CSample* psam)
	{
		for (uint32 u4=0 ; u4<MAX_PSEUDO3D_SAMPLES; u4++)
		{
			if (apsamPseudo[u4] == psam)
			{
				apsamPseudo[u4] = NULL;
				return;
			}
		}
	}


	//******************************************************************************************
	//
	void CommitPseudoSettings();


	//******************************************************************************************
	//
	bool bHardwareFound()
	{
		return b3DHardwareFound;
	}


	//******************************************************************************************
	// This will always return false if there is no sound in the machine
	//
	bool bUsingHardware()
	{
		if (bHardware)
		{
			// If the flags say we are using hardware then make sure that there really is an
			// interface. It would be bad if this function returned true when there is no
			// sound present!
			Assert(pDSInterface);
		}
		return bHardware;
	}

	//******************************************************************************************
	//
	void UseHardware(bool b_hw)
	{
		if (b3DHardwareFound)
		{
			bHardware = b_hw;
		}
	}

    //*****************************************************************************************
    // returns the pointer to the direct sound interface.  This is necessary for 3rd party
    // application plugins like Smacker.
    //
    LPDIRECTSOUND pdsGetDSInterface()
    {
        return pDSInterface;
    }

    //*****************************************************************************************
    //
    void SetVolume
    (
        float   f_volume
    );

    //*****************************************************************************************
    //
    float GetVolume();

	//*****************************************************************************************
	//
	void FadeSample
	(
		CSample*	psam,
		float		f_fader,
		bool		b_stop = true		// stop the sample when we hit minimum volume?
	);


	//******************************************************************************************
	//
	void StopFade
	(
		CSample*	psam
	);

	//******************************************************************************************
	//
	void AddDeferredSampleFill
	(
		CSample* psam
	);

	//******************************************************************************************
	//
	void RemoveDeferred
	(
		CSample*	psam
	);


	//******************************************************************************************
	void EnableEnvironmentalAudio();

	//******************************************************************************************
	void RemoveEnvironmentalAudio();

	//******************************************************************************************
	// Return true if EAX is present otherwise false
	bool bEAXPresent()
	{
		return pDSEAX != NULL;
	}

	//******************************************************************************************
	SAudioEnvironment aenvGetEnvironment();

	//******************************************************************************************
	void SetEAXReverbVolume(float f_rvol);

	//******************************************************************************************
	void SetEAXReverbDecay(float f_decay);

	//******************************************************************************************
	void SetEAXReverbDamping(float f_damp);

	//******************************************************************************************
	void SetEAXEnvironment(uint32 u4_env);

	//******************************************************************************************
	void CleanUp();

	//******************************************************************************************
	void ResumeAudioLoader();

	//******************************************************************************************
	void SuspendAudioLoader();

	//******************************************************************************************
	void SetupNextDeferLoad();

	//******************************************************************************************
	void LockDefer()
	{
		EnterCriticalSection(&csDefer);
	}

	//******************************************************************************************
	void UnlockDefer()
	{
		LeaveCriticalSection(&csDefer);
	}


protected:
	bool					bCreateStreamSupport();
	void					RemoveStreams();

	bool					bMasterEnable;		// enable parameter passed to constructor
	bool					bHardware;			// set to true if we are using a card with 3D hardware
	bool					b3DHardwareFound;	// set to true if we found a card with 3D hardware
	bool					bSoundEnabled;		// set to false if not present or disabled
	bool					bChangeFormat;		// true if the primary format can be changed
	bool					bThreadExit;		// set to true to get the loader thread to exit
	bool					bThreadLocked;		// used to stop us taking the semaphore twice
	uint32					u4Format;			// format of output buffer, bits 30,31 = 16bit,stereo
	LPDIRECTSOUND			pDSInterface;		// interface pointer to DirectSound
	LPDIRECTSOUNDBUFFER		pDSPrimary;			// interface for the primary (hardware) buffer
	LPDIRECTSOUND3DLISTENER	pDS3DListener;		// interface for the 3D listener
	HWND					hWnd;				// window handle DS is attached to
	HMODULE					hLibrary;			// The DLL that we used to create the sound interface
	LPIA3D					pA3D;				// the A3D interface

	LPDIRECTSOUNDBUFFER		pDSEAX;				// 128 byte secondary buffer for EAX to set global reverb with, cannot use primary
	LPDIRECTSOUND3DBUFFER	pDSEAX3D;			// 3D interface to the above sound buffer
	LPKSPROPERTYSET			pDSPropertySet;
	uint32					u4EAXPropertySupport;

	std::string					strBasePath;		// the path to which all samples are loaded from

	float					fListenerX;
	float					fListenerY;
	float					fListenerZ;

	bool					bCanStream;			// true if we can stream audio
	uint32					u4StreamsGoing;		// number of active streams...
	HANDLE					hThread;
	uint32					u4ThreadID;

	CRITICAL_SECTION		csStream;
	CRITICAL_SECTION		csFade;
	CRITICAL_SECTION		csDefer;

	SStreamedSample			assamStreams[MAX_CONCURRENT_STREAMS];
	CSample*				apsamPseudo[MAX_PSEUDO3D_SAMPLES];
	CSample*				apsamDeferred[MAX_DEFERRED_OPS];
	SAudioFade				afFadingSamples[MAX_FADING_SAMPLES];

	// a static member function for the thread callback
	static DWORD _stdcall HandleStreamStub(void* pv_user);
	static DWORD _stdcall HandleNoAudioSubtitleStub(void* pv_user);

	// The above functions just call these two class member functions
	uint32					u4HandleStream();
	uint32					u4HandleNoAudioSubtitle();

	HANDLE					ahSemaphore[u4SEMAPHORE_COUNT];
	bool					bDeferredGoing;
	uint32					u4DeferredLoadOffset;
	uint32					u4DeferredLoadRemain;
	uint32					u4MaxDeferredBytes;
	uint32					u4DeferredIndex;
	CSample*				psamDeferred;


	static uint32			u4Frequency;
	static ESoundChannels	escChannels;
	static ESoundBits		esbSampleBits;

public:
	static CAudio*			pcaAudio;
};



//**********************************************************************************************
//
// CSample
//
class CSample
//prefix: sam
{
public:
	//**********************************************************************************************
	// constructors and deconstrutors
	//
	CSample();

	CSample
	(
		CCAULoad*		pcau,
		uint32			u4_flags,
		TDeferCallback	pdlcbfn = NULL,
		void*			pv_user = NULL
	);

	//**********************************************************************************************
	~CSample();
	
	//**********************************************************************************************
	bool bPlay
	(
		uint32 u4_flags = AU_PLAY_ONCE
	);

	//**********************************************************************************************
	bool bPlay
	(
		const SSoundTransfer&	st,
		float					f_velocity, 
		float					f_x,
		float					f_y,
		float					f_z, 
		uint32					u4_play_flags = AU_PLAY_ONCE
	);

	//**********************************************************************************************
	//
	void SetTransfer
	(
		const SSoundTransfer&	st,
		float					f_velocity, 
		float					f_x,
		float					f_y,
		float					f_z,
		bool					b_commit = false
	);

	//**********************************************************************************************
	void Stop();

	//**********************************************************************************************
	uint32 u4Status()
	{
		HRESULT	hr;
		uint32	u4Status;

		// if we have no buffer we cannot be playing....
		if (pDSBuffer == NULL)
			return false;

		// get the status from direct sound
		hr = pDSBuffer->GetStatus((DWORD*)&u4Status);
		Assert(hr==DS_OK);

		// convert it to our format
		if ((u4Status == DSBSTATUS_LOOPING) || (u4Status == DSBSTATUS_PLAYING))
			return AU_BUFFER_STATUS_PLAYING;

		if (u4Status == DSBSTATUS_BUFFERLOST)
			return AU_BUFFER_STATUS_LOST;

		return AU_BUFFER_STATUS_NOTPLAYING;
	}

	//**********************************************************************************************
	// The instance flags are the same as the AU_CREATE flags passed to the sample constructor.
	// This enables instances of a sample to be of a different spatial type to their siblings. If
	// instance flags are zero then an identical instance is created.
	//
	CSample* psamCreateInstance
	(
		uint32	u4_instance_flags = 0
	);


	//**********************************************************************************************
	void SetPosition
	(
		float f_x,
		float f_y,
		float f_z,
		bool  b_immediate = true
	);

	//**********************************************************************************************
	void SetOrientation
	(
		float f_x,
		float f_y,
		float f_z,
		bool  b_immediate = true
	);

	//**********************************************************************************************
	void SetFrustum
	(
		float f_angle = AU_SOUND_FUSTRUM_DEFAULT_ANGLE,			// angular displacement of the cone
		float f_volume = AU_SOUND_FUSTRUM_DEFAULT_VOLUME,		// volume in dBs (0 = full volume -100dB silent) outside the cone
		bool  b_immediate = true
	);

	//**********************************************************************************************
	// attenuation is in dB/m (1.0 = 1dB/m therefore half volume will be at 6 meters)
	void SetAttenuation
	(
		float f_atten, 
		float f_mindist = 0.0f,		// distance at which the attenuation is applied
		bool b_immediate = true
	);

	//**********************************************************************************************
	void SetFrequency
	(
		float f_freq
	);

	//**********************************************************************************************
	void SetVolume
	(
		float f_vol
	);


	//**********************************************************************************************
	float fGetVolume()
	{
		return fVolume;
	}

	//**********************************************************************************************
	float fGetMasterVolume()
	{
		return fMasterVolume;
	}

	//**********************************************************************************************
	//Change the master volume of a sample
	void SetMasterVolume(float f_vol)
	{
		fMasterVolume = f_vol;
		SetVolume(fVolume);
	}

	//**********************************************************************************************
	// Get the actual play volume of a sample once it has been adjusted by its master volume
	float fGetAdjustedVolume()
	{
		float f_vol = fVolume;

		f_vol += fMasterVolume;

		if (f_vol>0.0f)
			f_vol = 0.0f;

		if (f_vol<-100.0f)
			f_vol = -100.0f;

		return f_vol;
	}

	//**********************************************************************************************
	void Lock
	(
		uint32 u4_offset, 
		uint32 u4_length
	)
	{
		HRESULT	hr;

		Assert(pDSBuffer);

		hr = pDSBuffer->Lock(u4_offset, u4_length, (void**)&pu1Lock1, (DWORD*)&u4Lock1Length, (void**)&pu1Lock2, (DWORD*)&u4Lock2Length, 0);
		Assert(hr == DS_OK);
	}

	//**********************************************************************************************
	void Unlock()
	{
		HRESULT	hr;

		Assert(pDSBuffer);

		hr = pDSBuffer->Unlock(pu1Lock1, u4Lock1Length, pu1Lock2, u4Lock2Length);
		Assert(hr == DS_OK);
	}

	//**********************************************************************************************
	uint8* pu1Lock1Buffer()
	{
		return pu1Lock1;
	}

	//**********************************************************************************************
	uint8* pu1Lock2Buffer()
	{
		return pu1Lock2;
	}

	//**********************************************************************************************
	uint32 u4Lock1BufferLength()
	{
		return u4Lock1Length;
	}

	//**********************************************************************************************
	uint32 u4Lock2BufferLength()
	{
		return u4Lock2Length;
	}

	//**********************************************************************************************
	bool bSampleStreamed()
	{
		return (u4CreateFlags & AU_CREATE_STREAM) != 0;
	}

	//**********************************************************************************************
	bool bSampleLooped()
	{
		return (u4CreateFlags & AU_PLAY_LOOPED) != 0;
	}

	//**********************************************************************************************
	bool bSampleLoading()
	{
		return (u4CreateFlags & AU_CREATE_DEFER_LOAD) != 0;
	}

	//**********************************************************************************************
	uint32 u4SoundBufferLength()
	{
		return u4Length;
	}

	//**********************************************************************************************
	float fSamplePlayTime()
	{
		return fPlayTime;
	}

	//**********************************************************************************************
	// This is called when the streamer wants to stop the sample. It cannot call stop because of
	// semaphore issues. Instead it sets this lfag an expects the foreground to take care of it
	// when it gets a chance.
	void FlagStop()
	{
		u4CreateFlags |= AU_FLAG_STOP;
	}

	//**********************************************************************************************
	// Return true or flase depending on the state of the stop flag.
	bool bStopFlagged()
	{
		return (u4CreateFlags & AU_FLAG_STOP) != 0;
	}

	//**********************************************************************************************
	float fCalculateVolumeAttenuation()
	{
		float	f_vol;
		float	f_dist;

		// linear attenuation, distance * roll off per meter in dBs
		f_dist = sqrt(CAudio::pcaAudio->fGetSquaredListenerDistance(fWorldX, fWorldY, fWorldZ));// * -fAtten;

		// if the distance is less than the max vol distance then return max volume
		if (f_dist<=fMaxVolDistance)
			return 0.0f;

		f_vol = (f_dist - fMaxVolDistance) * -fAtten;

		if (f_vol<-100.0f)
			f_vol = -100.0f;

		return f_vol;
	}

	//**********************************************************************************************
	//
	//
	uint32					u4CreateFlags;	// flags that the buffer was created with;
	uint32					u4Length;		// length of the sound buffer in bytes
	float					fPlayTime;		// length of the sample in seconds
	float					fVolume;		// current volume of the sample
	uint32					u4Format;		// format dword of the sample....

	CCAULoad*				pcauSample;		// the loader responsible for this sample or NULL
	LPDIRECTSOUNDBUFFER		pDSBuffer;		// directSound interface
	LPDIRECTSOUND3DBUFFER	pDS3DBuffer;	// 3d interface, NULL if not 3D
	CAudioSubtitle*			pasubSubtitle;	// pointer to the sub title or NULL

	uint8*					pu1Lock1;		// base address of first half of lock
	uint8*					pu1Lock2;		// base address of second half of lock
	uint32					u4Lock1Length;	// length of lock1 block
	uint32					u4Lock2Length;	// length of lock2 block

	uint32					u4StreamLoadPosition;
	uint32					u4LastPlayCsr;
	uint32					u4BytesPlayed;

	float					fAtten;
	float					fMaxVolDistance;// Within this distance there is no attenuation
	float					fWorldX;
	float					fWorldY;
	float					fWorldZ;
	float					fFustrumAngle;
	float					fOutsideVolume;

	float					fMasterVolume;	// What we should adjust the final volume by (+100 to -100 db, 0 being no change)

	TDeferCallback			pdlcbfnCallback;
	void*					pvUser;
};


#endif
