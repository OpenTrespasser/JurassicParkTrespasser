/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	CAudioDaemon definition
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/AudioDaemon.hpp                                             $
 * 
 * 52    9/29/98 11:43p Rwyatt
 * Removed class name from function definitions
 * 
 * 51    9/28/98 5:02a Rwyatt
 * All audio is now defer loaded
 * 
 * 50    9/24/98 11:19a Rwyatt
 * saving ambients
 * 
 * 49    9/21/98 3:33a Rwyatt
 * New member variable, bExit
 * 
 * 48    9/17/98 3:03p Rwyatt
 * Subtitles now use the text system
 * 
 * 47    9/14/98 12:10a Rwyatt
 * Default parameter on KillCollisions()
 * 
 * 46    9/12/98 10:34p Rwyatt
 * New member function
 * 
 * 45    9/01/98 3:06p Rwyatt
 * Basic load and save
 * 
 * 44    8/25/98 11:30a Rvande
 * removed redundant access specifiers
 * 
 * 43    8/23/98 2:24a Rwyatt
 * Removed all reference to music and voice over time.
 * Passed 3D sound info to AI to allow dinos to hear
 * 
 * 42    8/21/98 2:29a Rwyatt
 * We can now have 12 queued voice overs instead of 2
 * 
 * 41    8/20/98 3:02p Rwyatt
 * Set the number of queued voice overs to 16
 * 
 * 40    8/04/98 4:00p Rwyatt
 * Open database now takes the number of simultaneous handles that can obtained.
 * 
 * 39    7/14/98 3:59p Rwyatt
 * Adjusted creation flags on loading ambinet samples.
 * Ambient samples now contain a looping flag
 * All samples are paused when leaving simulation mode.
 * 
 * 38    6/30/98 1:04a Rwyatt
 * Support for after looping stop samples.
 * Positioned effects can now be positioned directly and not through an instance
 * 
 * 37    6/29/98 1:06a Rwyatt
 * Mute ambient samples when dinos make noises.
 * 
 * 36    6/24/98 8:53p Rwyatt
 * Moved SetFeatures to cpp file
 * 
 * 35    6/15/98 11:58a Rwyatt
 * Master volume now gets set for ambients correctly
 * 
 * 34    6/08/98 12:56p Rwyatt
 * The database of a sample is now passed all the way down to the loader. This enables the
 * loader to close the file correctly for databases with static file handles.
 * 
 * 33    5/27/98 1:22a Rwyatt
 * Sample instances can no longer fill the collision cache. One the maximum quota has been
 * reached earlier versions of the same sample are re-used and stopped if they are playing.
 * 
 * 32    5/24/98 10:31p Rwyatt
 * New member function to remove a cache entry
 * 
 * 31    5/22/98 3:12a Rwyatt
 * Maximum number of deferred samples has been increased to 12
 * 
 * 30    98.05.14 7:01p Mmouni
 * Added save/restore of default settings.
 * 
 * 29    5/08/98 4:58p Rwyatt
 * Added StopSoundEffect global function and the corresponding AudioDaemon function
 * 
 * 28    5/06/98 3:35p Rwyatt
 * Looped samples and Sliding collisions
 * 
 * 27    4/28/98 11:21p Rwyatt
 * Added the ReplayLastVoiceOver function so the last voice over can be repeated.
 * 
 * 26    4/27/98 7:27p Rwyatt
 * Ambient sounds now have a max volume distance, under this distance the volume is maximum.
 * Effect processing has changed to allow positioned and attached effects,
 * Samples are now attached to instances on a one instance one sample basis.
 * 
 * 25    4/21/98 2:58p Rwyatt
 * Added support for the system message
 * 
 * 24    98.04.03 3:02p Mmouni
 * Added save/load of audio settings.
 * 
 * 23    3/22/98 5:02p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 22    3/21/98 3:19p Shernd
 * added a static class variable to allow the Data Path to be set before loading.
 * 
 * 21    3/09/98 10:52p Rwyatt
 * Constructs the sound databases.
 * All requests for audio are now made through the sound databses.
 * COLLISIONS STILL USE THE OLD SYSTEM
 * 
 * 20    2/21/98 5:55p Rwyatt
 * New functions for sound effects and modifed the collision cahce to handle the effects. Sound
 * effects can stop collision sounds buf collisions can not stop effects.
 * 
 * 19    2/11/98 4:37p Rwyatt
 * New members to control GDI subtitles
 * 
 * 18    2/06/98 8:19p Rwyatt
 * Processor subtitles
 * Creates the subtitle font
 * 
 * 17    12/17/97 2:52p Rwyatt
 * Now allow stereo ambients and cross instancing of spatial types
 * 
 * 16    12/14/97 1:18a Rwyatt
 * Fixed a problem with instancing defered samples when the sample failed to load.
 * 
 * 15    12/02/97 11:54p Rwyatt
 * Added the kill functions to silence the various sample types and a KillAll function that
 * kills them all. When a sample is killed it may or may not be deleted.
 * 
 * 14    11/22/97 10:45p Rwyatt
 * Now uses CAudio::pcaAudio rather than the global pointer, which has been removed.
 * Defer loads ambient sounds, significant modification to allow for the callbakc function to
 * successfully start the ambinet when it has finished loading, sometime later. This will be
 * extended to voiceovers and music effects.
 * 
 * 13    11/18/97 9:41p Rwyatt
 * Temp checkin:
 * Added callback function for defered loading
 * 
 * 12    11/16/97 4:58a Rwyatt
 * Increased MAX_AMBIENT_SOUNDS which defines how many ambient sounds can be loaded at once,
 * this is not how many can be played
 * 
 * 11    11/14/97 7:18p Rwyatt
 * 
 * 10    11/13/97 11:37p Rwyatt
 * BIG CHANGES.....
 * This is part of the new audio system which is more abstract and far more flexible than the
 * old system. In this system the audio daemon is responsible for tracking of the samples that
 * it creates and not the audio system.
 * Samples can be attached to objects.
 * Samples move with moving objects
 * 
 * 9     10/23/97 6:53p Rwyatt
 * New access functions to set and clear feature bits
 * 
 * 8     10/17/97 6:06p Rwyatt
 * Added feature flags
 * 
 * 7     10/03/97 11:41a Agrant
 * Added bool to turn terrain sounds on and off.
 * 
 * 6     6/15/97 6:16p Agrant
 * Simplified loading by moving col file loading to CLoadWorld and
 * CAudioDaemon.
 * 
 * 5     6/09/97 12:10p Rwyatt
 * New members for Audio streaming and collisions have been temporarly removed so matrix
 * collisions can be inserted.
 * 
 * 4     6/02/97 4:52p Rwyatt
 * Added a timer for killing the current voice over.
 * 
 * 3     5/25/97 8:31p Rwyatt
 * Removed all of Brandon's RSX code and renamed AudioDeemone to AudioDaemon.
 * 
 ***********************************************************************************************/

#ifndef HEADER_LIB_AUDIO_AUDIODAEMONE_HPP
#define HEADER_LIB_AUDIO_AUDIODAEMONE_HPP

#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/EntityDBase/Subsystem.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgAudio.hpp"


class CSample;
class CAudioDaemon;
class CAudioDatabase;


//********************************************************************************************
// if the normalized vel if less then this the ignore the collision
#define fMIN_VELOCITY			(0.01f)

//********************************************************************************************
// the maximum number of sounds that can be attached to objects
#define MAX_ATTACHED_SOUNDS		(32)

//********************************************************************************************
// the maximum number of ambient sounds that can be attached to objects
#define MAX_AMBIENT_SOUNDS		(16)

//********************************************************************************************
// The maximum number of delayed voice overs at any one time...
#define MAX_DELAYED_VOICEOVER	(12)

//********************************************************************************************
// The number of dBs that the music volume is reduced by when a voiceover plays
#define MUSIC_VOLUME_ADJUST		(10.0f)

//********************************************************************************************
// The number of samples that can be in the collision cache
#define COLLISION_CACHE_LENGTH	18

//********************************************************************************************
// The number of instances of a sample that can exist in the collision cahce, once these
// instances are used old playing samples will start to be reused.
#define COLLISION_CACHE_MAXINSTANCE	3

//********************************************************************************************
// The amount of memory that the collision cache can take at maximum
#define COLLISION_CACHE_MEMORY	(600*1024)


//********************************************************************************************
// These flags should be set in the u4FeaturesEnabled word to enable the speified feature.
//
#define	AUDIO_FEATURE_VOICEOVER		(0x00000001)
#define AUDIO_FEATURE_EFFECT		(0x00000002)
#define AUDIO_FEATURE_MUSIC			(0x00000004)
#define AUDIO_FEATURE_AMBIENT		(0x00000008)
#define AUDIO_FEATURE_SUBTITLES		(0x00000010)

#define AUDIO_FEATURE_ALL			(0x0000001F)		// keep this upto date


//**********************************************************************************************
//
struct SSoundObjAttachment
// prefix: sat
//
//**************************************
{
	CSample*	psam;				// the sample
	CInstance*	pins;				// the object that it is attached to
	TSec		sDetatchTime;		// time when the sound should be detached from the object

	//*****************************************************************************************
	SSoundObjAttachment()
	{
		psam			= NULL;
		pins			= NULL;
		sDetatchTime	= 0;
	}
};


//*********************************************************************************************
#define AMBIENT_MUTED			0x00000001
#define AMBIENT_NOT_MUTEABLE	0x00000002		// Set while a sample loads
#define AMBIENT_NEVER_MUTE		0x00000004		// Set for samples that should not be muted
#define AMBIENT_LOOP			0x00000008		// Set for ambient samples that loop

//*********************************************************************************************
//
struct SAmbientSound
// prefix: amsnd
//
//**************************************
{
	CSample*		psam;				// the sample
	TSec			sEndTime;			// the time when this sample is going to end
	float			fMaxDistance;		// stop the sample if futher than this
	TSec			sTime;				// frame time when this sample was last used.
	TSoundHandle	shSample;			// handle of this sample..
	bool			bActive;			// should this sample be considered for processing
	float			fRestoreVolume;		// Put current volume here when muting ambient sounds
	uint32			u4Flags;			// Ambient control flags

	//*****************************************************************************************
	SAmbientSound()
	{
		psam			= NULL;
		sEndTime		= 0.0f;
		fMaxDistance	= 0.0f;
		sTime			= 0.0f;
		shSample		= 0;
		bActive			= false;
		u4Flags			= 0;
	}
};



//*********************************************************************************************
//
#define COLLISION_CACHE_EFFECT			0x00000001
#define COLLISION_CACHE_LOOPED			0x00000002
#define COLLISION_CACHE_LOOP_UPDATE		0x00000004


//*********************************************************************************************
//
struct SCollisionCacheEntry
// predix: cce
//
//**************************************

{
	CSample*		psam;				// the sample
	TSoundHandle	shSample;			// sound handle of the sample
	TSoundHandle	shStopSample;		// sound handle to play when a looped effect is stopped, or 0
	TSec			sTime;				// frame time when this was last used
	uint32			u4Flags;			// cache flags
	uint32			u4ID;				// hash value for this collision (used by the slides)

	//*****************************************************************************************
	SCollisionCacheEntry()
	{
		psam			= NULL;
		shSample		= 0;
		shStopSample	= 0;
		sTime			= 0.0f;
	}
};



//*********************************************************************************************
//
class CAudioDaemon : public CSubsystem
// Prefix: ad
//
//**************************************
{
public:
	CAudioDaemon();
	~CAudioDaemon();

	//******************************************************************************************
	// Called by the destructor to clean to clean up before we delete
	void CleanUp();

	//******************************************************************************************
	// The messages that we respond to.
	//
	void Process(const CMessageStep& msg_step);
	void Process(const CMessageMove& msg);
	void Process(const CMessageCollision& msg);
	void Process(const CMessageAudio& msg_audio);
	void Process(const CMessageSystem& msg);

	//*****************************************************************************************
	char* pcSave(char*  pc) const;

	//*****************************************************************************************
	const char* pcLoad(const char*  pc);

	//*****************************************************************************************
	char *pcSaveSample
	(
		char*			pc,
		TSoundHandle	sndhnd,
		CSample*		psam
	) const;

	//*****************************************************************************************
	const char *pcLoadSample
	(
		const char*			pc,
		CMessageAudio&		msg
	);

	//*****************************************************************************************
	//
	void SaveDefaults();
	//
	// Save the default values of the user modifiable settings.
	//
	//**********************************

	//*****************************************************************************************
	//
	void RestoreDefaults();
	//
	// Restore the default values of the user modifiable settings.
	//
	//**********************************

	//******************************************************************************************
	//
	CSample* psamCreateMessageStream(const CMessageAudio& msg);

	//******************************************************************************************
	//
	void CreateVoiceover(const CMessageAudio& msg);

	//******************************************************************************************
	//
	void CreateMusic(const CMessageAudio& msg);

	//******************************************************************************************
	//
	void CreateAmbient(const CMessageAudio& msg);

	//******************************************************************************************
	//
	CSample* psamCreateCollision
	(
		TSoundHandle	sndhnd,
		uint32			u4_id = 0		// set to non zero for slide collisions
	);

	//******************************************************************************************
	// Give a search key for a collision between two sound materials.
	// If one of the collisions materials is 0 then the collision hash value is as if both
	// materials where the non zero material.
	//
	uint64 u8CollisionHash(TSoundMaterial mat1, TSoundMaterial mat2)
	{
		if (mat1 == 0)
		{
			mat1 = mat2;
		}
		else if (mat2 == 0)
		{
			mat2 = mat1;
		}

		if (mat1<mat2)
		{
			return ((uint64)mat1) | (((uint64)mat2)<<32);
		}
		else
		{
			return ((uint64)mat2) | (((uint64)mat1)<<32);
		}
	}

	//******************************************************************************************
	//
	CSample* psamFindInCollisionCache
	(
		TSoundHandle	th_sound,
		uint32			u4_flags = 0,
		uint32			u4_id = 0,
		TSoundHandle	sndhnd_stop = 0
	);

	//******************************************************************************************
	//
	void AddToCollisionCache
	(
		CSample*		psam, 
		TSoundHandle	sh,
		uint32			u4_flags = 0,
		uint32			u4_id = 0,
		TSoundHandle	sndhnd_stop = 0
	);

	//******************************************************************************************
	void RemoveCollisionCacheEntry(TSoundHandle sndhnd_ignore = 0);

	//******************************************************************************************
	uint32 u4UpdateCollisionLoopStatus
	(
		uint32			u4_id
	);

	//******************************************************************************************
	void ProcessLoopedCollisions();

	//******************************************************************************************
	void SetFeatures(uint32 u4_mask, uint32 u4_set);

	//******************************************************************************************
	//
	uint32 u4GetFeatures(uint32 u4_mask = AUDIO_FEATURE_ALL)
	{
		return u4FeaturesEnabled & u4_mask;
	}

	//******************************************************************************************
	void ProcessAttachments();

	//******************************************************************************************
	// Attach a sample to an object
	//
	void AttachSoundToObject
	(
		CInstance*	pins,
		CSample*	psam,
		bool		b_sample_looped = false
	);


	//******************************************************************************************
	CInstance* pinsFindSampleAttachment
	(
		CSample*	psam
	) const;

	//******************************************************************************************
	// Delay a voice over
	//
	void DelayVoiceOver
	(
		CSample*	psam
	);

	//******************************************************************************************
	// Remove all sound attachments for a given instance
	//
	void RemoveSoundAttachment
	(
		CInstance*	pins
	);

	//******************************************************************************************
	// Remove the attachment for the given sample
	//
	void RemoveSoundAttachment
	(
		CSample* psam
	);

	//******************************************************************************************
	// Activate an existing element in the ambient list
	//
	void ActivateAmbientElement
	(
		CSample*		psam,
		TSoundHandle	sh,
		bool			b_loop,
		int32			i4_loop_count,
		float			f_dist,
		uint32			u4_activate
	);


	//******************************************************************************************
	// Cancel the ambient sample in the specified list element
	//
	void CancelAmbient(uint32 u4_element)
	{
		amsndAmbients[u4_element].psam = NULL;
		amsndAmbients[u4_element].shSample = 0;
		amsndAmbients[u4_element].bActive = false;
	}

	//******************************************************************************************
	uint32 u4CountAmbientSounds
	(
	) const;
	
	//******************************************************************************************
	//
	uint32 AddToAmbientList
	(
		CSample*		psam,
		TSoundHandle	sh,
		bool			b_loop,
		int32			i4_loop_count,
		float			f_dist
	);


	//******************************************************************************************
	// this just goes through all the current ambients and checks if they need to be stopped or
	//
	void ProcessAmbientSounds
	(
	);


	//******************************************************************************************
	void ReplayLastVoiceover();

	//******************************************************************************************
	//Function to play start an effect
	bool bPlaySoundEffect
	(
		const CMessageAudio&	msg,
		SSoundEffectResult*		pser			// NULL if no result is required
	);

	//******************************************************************************************
	void StopSoundEffect
	(
		CSample*	psam
	);

	//******************************************************************************************
	void SetupSample
	(
		CSample*				psam,
		const CMessageAudio&	msg,
		bool					b_attach
	);

	//**********************************************************************************************
	CSample* psamCreateSampleWithRetry
	(
		TSoundHandle	sndhnd,
		CAudioDatabase*	padat,
		uint32			u4_flags
	);

	//**********************************************************************************************
	void MuteAmbientSounds
	(
		float	f_play_time
	);

	//**********************************************************************************************
	void RestoreAmbientSounds();

	//******************************************************************************************
	// Find a sample of the given handle, if the sample is playing it will be instanced.
	//
	CSample* psamFindInAmbientList
	(
		TSoundHandle		sh,
		uint32				u4_spatial_type,
		uint32&				u4_activate
	);

	//******************************************************************************************
	// Kill all sounds but leave the cahces in tact where possible
	//
	void KillAllSounds()
	{
		KillVoiceovers();
		KillAmbientSamples();
		KillMusic();
		KillCollisions();
	}

	//******************************************************************************************
	void KillVoiceovers();

	//******************************************************************************************
	void KillAmbientSamples();

	//******************************************************************************************
	void KillMusic();

	//******************************************************************************************
	void KillCollisions
	(
		bool b_delete = false		// by default do not delete the samples
	);

	//******************************************************************************************
	void PauseAllPlayingAudio();

	//******************************************************************************************
	void RestartPausedAudio();

	//******************************************************************************************
	void ProcessSubtitle
	(
		CSample*	psam
	);


	//******************************************************************************************
	//
	void RemoveSubtitle();


	//******************************************************************************************
	//
	void OpenAudioDatabases();

	//******************************************************************************************
	//
	void CloseAudioDatabases();

	//******************************************************************************************
	//
	CAudioDatabase* OpenDatabase
	(
		const char*	str_name,
		uint32		u4_handles
	);


	//******************************************************************************************
	//
    static void SetDataPath
    (
        const char * str_Path
    )
    {
        strcpy(str_DataPath, str_Path);
    }

	bool					bVoiceOver;				// is there a voice over playing
	bool					bDelayedVoiceOver;		// set to true if there is a delayed voice over in the list
	bool					bMusic;					// is there a music track playing
	bool					bExit;					// set to true during destruction
	CSample*				psamVoiceOver;			// the sample of the voice over
	CSample*				psamMusic;				// the sample of the current music piece
	TSoundHandle			sndhndVoiceOver;		// handle of the voiceover sample
	TSoundHandle			sndhndMusic;			// handle of the music sample
	CMessageAudio			msgAudioVoiceOver;		// Sound handle of the last voiceover
	bool					bAmbientMuted;			// Set to true while ambients are muted.
	TSec					sAmbientRestore;		// Frame time when the ambient sounds should be restored

	CAudioDatabase*			padatEffects;	// Handle based audio databases.
	CAudioDatabase*			padatStreams;
	CAudioDatabase*			padatAmbient;

	SCollisionCacheEntry	acceCollisions[COLLISION_CACHE_LENGTH];
	SSoundObjAttachment		asatSoundObjects[MAX_ATTACHED_SOUNDS];
	SAmbientSound			amsndAmbients[MAX_AMBIENT_SOUNDS];
	CSample*				apsamVoiceOver[MAX_DELAYED_VOICEOVER];

	static bool				bTerrainSound;						// should the terrain make collision messages
	static uint32			u4FeaturesEnabled;					// flags for which sound systems are enabled
    static char				str_DataPath[_MAX_PATH];            // Data path location

	char *pc_defaults;		// Buffer containing default values for settings.
};

extern CAudioDaemon* padAudioDaemon;

#endif


