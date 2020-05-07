/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	CAudioDaemon implementation
 *
 * Bugs:
 *
 * To do:
 *
 *  Add response messages for all AudioMessages
 *	May need to listen for delete messages so object attachments can be removed
 *  Global audio memory management so we do not exceed our memory limit.
 *  Deferred loading of all sample types, this required CSample and CAudio to be modifed
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/AudioDaemon.cpp                                             $
 * 
 * 135   10/03/98 11:13p Pkeet
 * Added safety check.
 * 
 * 134   10/02/98 3:52a Rwyatt
 * Ambients are now removed on clean up.
 * 
 * 133   10/02/98 2:35a Agrant
 * don't defer collisions
 * 
 * 132   10/01/98 6:49p Mlange
 * Improved collision message stats.
 * 
 * 131   10/01/98 4:05p Mlange
 * Improved move message stat reporting.
 * 
 * 130   9/30/98 5:59p Rwyatt
 * Sound effects now can either defer load or not
 * 
 * 129   9/29/98 4:36p Rwyatt
 * Fixed sound effect problem when hardware was enabled.
 * 
 * 128   9/29/98 1:04a Rwyatt
 * Ambient samples are only saved if they have a valid attachment.
 * All attachment timimgs are done eith real time and not elapsed time
 * 
 ***********************************************************************************************/

// AudioDaemon is the exception and it includes common.hpp and audio.hpp
#include "common.hpp"
#include "AudioDaemon.hpp"
#include "Lib/Audio/Audio.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"

#include "Lib/EntityDBase/Container.hpp"
#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/EntityDBase/Subsystem.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgAudio.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgCollision.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPaint.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgSystem.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/EntityDBase/TextOverlay.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include "Lib/Groff/EasyString.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "Game/AI/AIMain.hpp"

#include "lib/sys/memorylog.hpp"
#include "Lib/Sys/FileEx.hpp"
#include "Lib/Sys/Profile.hpp"

#include "Lib/View/RasterVid.hpp"


//*********************************************************************************************
// Define this to dump out all audio information
//#define AUMSG(x) dprintf(x);
#define AUMSG(x)

//*********************************************************************************************
#define fVOICEOVER_EXTEND	(1.0f) + fSTREAM_LENGTH;

//*********************************************************************************************
// The sample extend bit is defined as user bit 1
#define AU_ATTACHED			AU_USER_BIT1

//*********************************************************************************************
// define DUMP_COLLISIONS to show object collisions
// #define DUMP_COLLISIONS

//*********************************************************************************************
// Global Audio Daemon pointer, initialized at the very beginning of setup along with the
// audio system.
//
CAudioDaemon* padAudioDaemon = 0;


static CSample* psamTest = 0;



//*********************************************************************************************
// Static settings that will survive deleting the audio daemon and recreating it.
//
bool	CAudioDaemon::bTerrainSound			= true;
// all features are enabled by default...
uint32	CAudioDaemon::u4FeaturesEnabled		= AUDIO_FEATURE_VOICEOVER | AUDIO_FEATURE_EFFECT | 
											AUDIO_FEATURE_MUSIC | AUDIO_FEATURE_AMBIENT |
											AUDIO_FEATURE_SUBTITLES;

char        CAudioDaemon::str_DataPath[_MAX_PATH]	= {"\0"};


//*********************************************************************************************
CAudioDaemon::CAudioDaemon()
{
	uint32 u4_sample_count;

	SetInstanceName("Audio Daemon");

	MEMLOG_ADD_COUNTER(emlSoundControl,sizeof(CAudioDaemon));	

	bVoiceOver							= false;			// there is no active voice over at startup
	bMusic								= false;
	bDelayedVoiceOver					= false;
	bExit								= false;
	msgAudioVoiceOver.sndhndSample		= 0;
	bAmbientMuted						= false;
	sAmbientRestore						= 0.0f;
	sndhndVoiceOver						= 0;
	sndhndMusic							= 0;
	

	// clear out the delayed voice over lists
	for (u4_sample_count = 0; u4_sample_count<MAX_DELAYED_VOICEOVER; u4_sample_count++)
	{
		apsamVoiceOver[u4_sample_count] = NULL;
	}

    if (strlen(str_DataPath) == 0)
    {
        SetDataPath("\\\\Trespasser\\SiteB\\AudioPackFiles\\");
    }

	OpenAudioDatabases();

	// No defaults yet.
	pc_defaults = 0;

	// Register this entity with the message types it needs to receive.
	     CMessageStep::RegisterRecipient(this);
	    CMessagePaint::RegisterRecipient(this);
	    CMessageAudio::RegisterRecipient(this);
	     CMessageMove::RegisterRecipient(this);
	CMessageCollision::RegisterRecipient(this);
	   CMessageSystem::RegisterRecipient(this);
}



//*********************************************************************************************
CAudioDaemon::~CAudioDaemon()
{
	   CMessageSystem::UnregisterRecipient(this);
	CMessageCollision::UnregisterRecipient(this);
	     CMessageMove::UnregisterRecipient(this);
	    CMessageAudio::UnregisterRecipient(this);
	    CMessagePaint::UnregisterRecipient(this);
	     CMessageStep::UnregisterRecipient(this);


	MEMLOG_SUB_COUNTER(emlSoundControl,sizeof(CAudioDaemon));

	CleanUp();

	CloseAudioDatabases();

	// Free settings values.
	delete pc_defaults;

    padAudioDaemon = 0;
}


//**********************************************************************************************
// This is called by the destructor to make sure it is safe to delete the audio daemon. It is
// also called before we pcLoad() the audio daemon to make sure any old data is removed.
//
void CAudioDaemon::CleanUp
(
)
//*************************************
{
	uint32 u4_sample_count;

	bExit = true;

	//OutputDebugString("Entering CAudioDaemon::CleanUp()\n");
	//
	// at this point the audio system could either be alseep, blocked, inside semaphore or suspended.
	//
	
	//
	// This will get the audio system into a known state, afterwards the thread will be running but doing nothing
	//
	CAudio::pcaAudio->CleanUp();

	if (bMusic)
	{
		RemoveSoundAttachment(psamMusic);
		delete psamMusic;
		bMusic = false;
	}

	if (bVoiceOver)
	{
		RemoveSoundAttachment(psamVoiceOver);
		delete psamVoiceOver;
		bVoiceOver = false;
	}

	// delete any delayed voiceovers
	bDelayedVoiceOver = false;
	for (u4_sample_count = 0; u4_sample_count<MAX_DELAYED_VOICEOVER; u4_sample_count++)
	{
		RemoveSoundAttachment(asatSoundObjects[u4_sample_count].psam);
		delete apsamVoiceOver[u4_sample_count];
		apsamVoiceOver[u4_sample_count] = NULL;
	}

	// clear out the ambient array
	for (u4_sample_count = 0; u4_sample_count<MAX_AMBIENT_SOUNDS; u4_sample_count++)
	{
		//
		// If sample pointer is still not valid, something has gone wrong so do not delete
		// the sample otherwise we will crash. It is better to have a memory leak!
		//
		// this element contains a genuine element, but the element may be loading
		RemoveSoundAttachment(amsndAmbients[u4_sample_count].psam);
		delete amsndAmbients[u4_sample_count].psam;

		amsndAmbients[u4_sample_count].psam	= NULL;
		amsndAmbients[u4_sample_count].sTime = 0.0f;
		amsndAmbients[u4_sample_count].shSample	= 0;
		amsndAmbients[u4_sample_count].bActive = false;
		amsndAmbients[u4_sample_count].u4Flags = 0;
	}

	// clear out the collision cache
	for (u4_sample_count = 0; u4_sample_count<COLLISION_CACHE_LENGTH; u4_sample_count++)
	{
		RemoveSoundAttachment(asatSoundObjects[u4_sample_count].psam);
		delete acceCollisions[u4_sample_count].psam;
		acceCollisions[u4_sample_count].psam		= NULL;
		acceCollisions[u4_sample_count].u4Flags		= 0;
		acceCollisions[u4_sample_count].shSample	= 0;
		acceCollisions[u4_sample_count].shStopSample = 0;
		acceCollisions[u4_sample_count].sTime		= 0.0f;
		acceCollisions[u4_sample_count].u4ID		= 0;
	}

	// delete any remaining attached samples
	for (u4_sample_count = 0; u4_sample_count<MAX_ATTACHED_SOUNDS; u4_sample_count++)
	{
		CSample* psam = asatSoundObjects[u4_sample_count].psam;
		RemoveSoundAttachment(asatSoundObjects[u4_sample_count].psam);

		// list any remaining attached samples in the debug window, there should not be
		// any at all as all sounds should be allocated through the AudioDaemon.
		if (psam)
		{
			dprintf("Left over attached sample [%x].\n", psam);
		}
	}

	//OutputDebugString("Leaving CAudioDaemon::CleanUp()\n");
}


//*********************************************************************************************
// Process a step message: we must update all positions of sounds.
void CAudioDaemon::Process(const CMessageStep& msg_step)
{
	ProcessAmbientSounds();

	if (bVoiceOver)
	{
		// we have a voice sample, maybe no sound buffer, we still have to process the subtitle
		if (u4GetFeatures(AUDIO_FEATURE_SUBTITLES))
		{
			ProcessSubtitle(psamVoiceOver);
		}

		if (psamVoiceOver->bStopFlagged())
		{
			// it is time to remove our last voice sample
			AUMSG("Stop voice over\n");

			Assert(psamVoiceOver);

			// If this was a 3D voice over (of any type) then it must have been attached to an
			// instance. You cannot have an un-attached positional voice over.
			// Remove the attachment before we delete the sample class in case the object is moving.
			// This could create a fault if the move message is somehow processed
			if (psamVoiceOver->u4CreateFlags & (AU_CREATE_PSEUDO_3D|AU_CREATE_SPATIAL_3D))
			{
				RemoveSoundAttachment(psamVoiceOver);
			}

			// if there another voice over waiting for us??
			if (bDelayedVoiceOver)
			{
				// delete the old voice over which will tidy up the stream if it was streamed
				delete psamVoiceOver;
				psamVoiceOver = NULL;

				bool b_find = false;

				// we have to find the delayed voice over and play it...
				for (uint32 u4=0; u4<MAX_DELAYED_VOICEOVER; u4++)
				{
					if (apsamVoiceOver[u4] != NULL)
					{
						psamVoiceOver = apsamVoiceOver[u4];
						apsamVoiceOver[u4] = NULL;

						//
						// Check to see if there is more delayee voiceovers
						b_find = false;
						for (uint32 u4_check=0; u4_check<MAX_DELAYED_VOICEOVER; u4_check++)
						{
							if (apsamVoiceOver[u4_check] != NULL)
								b_find = true;
						}
						
						// play the sample
						if (psamVoiceOver->bPlay(AU_PLAY_ONCE))
						{
							//sVoiceFinish = msg_step.sElapsedRealTime+psamVoiceOver->fSamplePlayTime()+fVOICEOVER_EXTEND;
							break;
						}
						else
						{
							// if we have failed, delete it and try the next.
							// This may upset the subtitles if there is another delayed voice over,
							dprintf("Delayed voiceover failed to start\n");
							delete psamVoiceOver;
							continue;
						}
					}
				}

				// we have not found a voice over in the delay list so we have done
				if (!b_find)
				{
					bDelayedVoiceOver = false;
				}
			}
			else
			{
				// delete the old voice over which will tidy up the stream if it was streamed
				delete psamVoiceOver;
				psamVoiceOver = NULL;

				// no delayed voice over so disable voice overs
				bVoiceOver = false;
				bDelayedVoiceOver = false;
			}

			// if we have stopeed playing a voice over and there is music playing
			// then puts its volume back to normal...
			if (!bVoiceOver)
			{
				if (bMusic)
				{
					float f_mvol = psamMusic->fGetVolume() + MUSIC_VOLUME_ADJUST;

					if (f_mvol>0.0f)
						f_mvol = 0.0f;

					psamMusic->SetVolume(f_mvol);
				}
			}
		}
	}


	if (bMusic)
	{
		//
		// Is the sample marked as finished streaming?
		//
		if ( psamMusic->bStopFlagged() )
		{
			// it is time to remove our last voice sample
			AUMSG("Stop music stream\n");
	
			Assert(psamMusic);

			// remove from the fade list just in case it is fading
			CAudio::pcaAudio->StopFade(psamMusic);

			// If this was a 3D music stream (of any type) then it must have been attached to an
			// instance.
			if (psamMusic->u4CreateFlags & (AU_CREATE_PSEUDO_3D|AU_CREATE_SPATIAL_3D))
			{
				RemoveSoundAttachment(psamMusic);
			}

			delete psamMusic;

			bMusic = false;
			psamMusic = NULL;
		}
	}


/*	if (GetAsyncKeyState('G')<0)
	{
		CMessageAudio	msg
		(
			sndhndHashIdentifier("GUN - MAC10"),
			eamPOSITIONED_EFFECT,		// type of sound
			NULL,						// sender (NULL if using play function)
			padAudioDaemon,				// receiver (NULL if using play function)
			NULL,						// parent of the effect (can be NULL for stereo)
			0.0f,						// volume 0dBs
			1.0f,						// attenuation (only for pseudo/real 3D samples)
			AU_SPATIAL3D,				// spatial type
			360.0f,						// fustrum angle (real 3D only)
			-15.0f,						// outside volume (real 3D only)
			true,						// looped
			0,							// loop count
			10000.0f,					// distance before sound is stopped (pseudo/real 3D only)
			0,
			0
		);

		ptr<CCamera> pcam = CWDbQueryActiveCamera().tGet();

		msg.SetDeferLoad(false);

		msg.SetPositionalInfo
		(
			pcam->pr3Presence().v3Pos
		);

		bImmediateSoundEffect(msg,NULL);
	}*/


/*	if (GetAsyncKeyState('G')<0)
	{

		CMessageAudio	msg
		(
			0x8d6b6c73,
			eamVOICEOVER,				// type of sound
			NULL,						// sender (NULL if using play function)
			padAudioDaemon,				// receiver (NULL if using play function)
			NULL,						// parent of the effect (can be NULL for stereo)
			0.0f,						// volume 0dBs
			1.0f,						// attenuation (only for pseudo/real 3D samples)
			AU_STEREO,				// spatial type
			360.0f,						// fustrum angle (real 3D only)
			-15.0f,						// outside volume (real 3D only)
			false,						// looped
			0,							// loop count
			10000.0f,					// distance before sound is stopped (pseudo/real 3D only)
			0,
			0
		);

		msg.Dispatch();
	}*/


	// check if any looped effects need to be stopped
	ProcessLoopedCollisions();

	// check if any attachments have timed out
	ProcessAttachments();

	// Is it time to restore the ambient sounds
	if ((bAmbientMuted) && (CMessageStep::sStaticTotal > sAmbientRestore))
	{
		RestoreAmbientSounds();
	}


	// only commit the new 3D audio settings one per frmae to save a shit load of calculation
	// time. This is especially true when you move the listener because all samples need to
	// be recalculated.

	CAudio::pcaAudio->CommitSettings();
}


//*****************************************************************************************
// Process a system message. This is primarily used to start and stop the game. When the
// game is stopped all playing audio should be paused.
void CAudioDaemon::Process(const CMessageSystem& msg)
{
	if (msg.escCode == escSTART_SIM)
	{
		//OutputDebugString("System sim start: Play audio.\n");

		AUMSG("System sim start: Play audio.\n");
		RestartPausedAudio();
		CAudio::pcaAudio->ResumeAudioLoader();
	}
	else if (msg.escCode == escSTOP_SIM)
	{
		//OutputDebugString("System sim stop: Pause audio.\n");

		AUMSG("System stop sim: Pause audio.\n");
		PauseAllPlayingAudio();
		CAudio::pcaAudio->SuspendAudioLoader();
	}
}



//*****************************************************************************************
void CAudioDaemon::SetFeatures(uint32 u4_mask, uint32 u4_set)
{
	u4FeaturesEnabled &= ~u4_mask;		// clear the bits in the mask
	u4FeaturesEnabled |= u4_set;		// set the bits in the feature set

	if ((u4FeaturesEnabled & AUDIO_FEATURE_VOICEOVER)==0)
	{
		// Switch off voice overs
		KillVoiceovers();
	}

	if ((u4FeaturesEnabled & AUDIO_FEATURE_MUSIC)==0)
	{
		// Switch off music if it is playing
		KillMusic();
	}

	if ((u4FeaturesEnabled & AUDIO_FEATURE_AMBIENT)==0)
	{
		// Switch off any ambients
		KillAmbientSamples();
	}

	if ((u4FeaturesEnabled & AUDIO_FEATURE_SUBTITLES)==0)
	{
		// Switch off sub titles
		RemoveSubtitle();
	}
}



//*****************************************************************************************
char *CAudioDaemon::pcSaveSample
(
	char*			pc,
	TSoundHandle	sndhnd,
	CSample*		psam
) const
{
	uint32			u4_type;
	CInstance*		pins = NULL;

	// save the handle of this sound
	pc = pcSaveT(pc, sndhnd);

	switch (psam->u4CreateFlags & AU_CREATE_TYPE_MASK)
	{
	case AU_CREATE_STEREO:
		u4_type = AU_STEREO;
		break;

	case AU_CREATE_SPATIAL_3D:
		u4_type = AU_SPATIAL3D;
		break;

	case AU_CREATE_PSEUDO_3D:
		u4_type = AU_PSEUDO3D;
		break;
	}

	// save the type of this sound
	pc = pcSaveT(pc, u4_type);

	// find what sample we are attached to.
	pins = pinsFindSampleAttachment(psam);

#if VER_TEST
	if (u4_type == AU_PSEUDO3D)
	{
		if (pins == NULL)
			_asm int 3;
	}
#endif

	// save the object this sound is attached to
	pc = pcSaveInstancePointer(pc, pins);

	// save all of the other sample properties
	pc = pcSaveT(pc, psam->fVolume);
	pc = pcSaveT(pc, psam->fAtten);
	pc = pcSaveT(pc, psam->fFustrumAngle);
	pc = pcSaveT(pc, psam->fOutsideVolume);
	pc = pcSaveT(pc, psam->fMaxVolDistance);
	pc = pcSaveT(pc, psam->fMasterVolume);

	return pc;
}


//*****************************************************************************************
const char *CAudioDaemon::pcLoadSample
(
	const char*			pc,
	CMessageAudio&		msg
)
{
	pc = pcLoadT(pc, &msg.sndhndSample);
	pc = pcLoadT(pc, &msg.u4SpatialType);
	pc = pcLoadInstancePointer(pc, &msg.pinsParent);

	pc = pcLoadT(pc, &msg.fVolume);
	pc = pcLoadT(pc, &msg.fAtten);
	pc = pcLoadT(pc, &msg.fFrustumAngle);
	pc = pcLoadT(pc, &msg.fOutsideVolume);
	pc = pcLoadT(pc, &msg.fMaxVolDistance);
	pc = pcLoadT(pc, &msg.fMasterVolume);

	return pc;
}


//*****************************************************************************************
char *CAudioDaemon::pcSave
(
	char* pc
) const
{
	// Version number.
	pc = pcSaveT(pc, 6);

	pc = pcSaveT(pc, CAudio::pcaAudio->u4GetFormat());
	pc = pcSaveT(pc, CAudio::pcaAudio->esconGetSpeakerConfig());
	pc = pcSaveT(pc, u4FeaturesEnabled);

	//
	// Now save what samples are playing
	//
	pc = pcSaveT(pc, bVoiceOver);
	pc = pcSaveT(pc, bMusic);

	if (bVoiceOver)
	{
		pc = pcSaveSample(pc, sndhndVoiceOver, psamVoiceOver);
	}

	if (bMusic)
	{
		pc = pcSaveSample(pc, sndhndMusic, psamMusic);
	}

	//
	// New for version 5
	//

	// Save out the ambient mute state
	pc = pcSaveT(pc, bAmbientMuted);
	pc = pcSaveT(pc, sAmbientRestore);

	// Write out the number of ambient samples
	uint32 u4_samples = u4CountAmbientSounds();
	pc = pcSaveT(pc, u4_samples);

	for (uint32 u4_sample_count = 0; u4_sample_count<MAX_AMBIENT_SOUNDS; u4_sample_count++)
	{
		// if this element is active and is not currently an invalid deferred load pointer
		if ( (amsndAmbients[u4_sample_count].bActive) && (pinsFindSampleAttachment(amsndAmbients[u4_sample_count].psam) !=NULL ))
		{
			pc = pcSaveSample(pc, amsndAmbients[u4_sample_count].shSample, amsndAmbients[u4_sample_count].psam);


			// If the End time of this ambient is more than the current time+sample length then the sample must be looped
			if (amsndAmbients[u4_sample_count].sEndTime > CMessageStep::sElapsedRealTime+amsndAmbients[u4_sample_count].psam->fPlayTime)
			{
				// we are looped
				pc = pcSaveT(pc, (bool)true);		// looped

				// If the end time is huge this sample is looped continously
				if (amsndAmbients[u4_sample_count].sEndTime > CMessageStep::sElapsedRealTime+10000000.0f)
				{
					pc = pcSaveT(pc, (int32)-1);		// loop count
				}
				else
				{
					float f_loop = (amsndAmbients[u4_sample_count].sEndTime - CMessageStep::sElapsedRealTime) / amsndAmbients[u4_sample_count].psam->fPlayTime;
					pc = pcSaveT(pc, (int32)(f_loop+0.5f));		// loop count
				}
			}
			else
			{
				// we are not looped
				pc = pcSaveT(pc, (bool)false);		// looped
				pc = pcSaveT(pc, (int32)0);		// loop count
			}

			pc = pcSaveT(pc, amsndAmbients[u4_sample_count].fMaxDistance);
			pc = pcSaveT(pc, amsndAmbients[u4_sample_count].u4Flags);
			pc = pcSaveT(pc, amsndAmbients[u4_sample_count].fRestoreVolume);

			u4_samples--;
		}
	}

	// Make sure we saved them all
	Assert(u4_samples == 0);

	return pc;
}


//*****************************************************************************************
const char *CAudioDaemon::pcLoad
(
	const char* pc
)
{
	int iVersion;
	CMessageAudio	msg_voice(padAudioDaemon,NULL);
	CMessageAudio	msg_music(padAudioDaemon,NULL);
	CMessageAudio*	pmsg_amb = NULL;
	bool			b_voice, b_music;
	uint32			u4_ambient_samples = 0;

	// Cleam out the audio system to its initial state.
	CleanUp();

	pc = pcLoadT(pc, &iVersion);

	if ((iVersion >= 1) && (iVersion <= 6))
	{
		uint32 u4_format;
		ESpeakerConfig escon;

		pc = pcLoadT(pc, &u4_format);
		pc = pcLoadT(pc, &escon);
		pc = pcLoadT(pc, &u4FeaturesEnabled);


		if (iVersion<6)
		{
			bool b_temp;
			uint32 u4_temp;
			pc = pcLoadT(pc, &b_temp);
			pc = pcLoadT(pc, &b_temp);
			pc = pcLoadT(pc, &u4_temp); 
		}

		CAudio::pcaAudio->SetSpeakerConfig(escon);
		CAudio::pcaAudio->SetOutputFormat(u4_format & AU_FORMAT_FREQ, 
										 ((u4_format & AU_FORMAT_STEREO) ? escSTEREO : escMONO),
										 ((u4_format & AU_FORMAT_16BIT) ? esb16BIT : esb8BIT)
										);

		if (iVersion >= 3)
		{
			// Version 3 contains playing sample information
			pc = pcLoadT(pc, &b_voice);
			pc = pcLoadT(pc, &b_music);

			if (b_voice)
			{
				msg_voice.u4Flags			= eamVOICEOVER;
				msg_voice.fMaxDistance		= 100000.0f;
				msg_voice.bLooped			= false;
				msg_voice.i4LoopCount		= 0;
				msg_voice.sndhndLoopStop	= 0;
				pc = pcLoadSample(pc,msg_voice);
			}

			if (b_music)
			{
				msg_music.u4Flags			= eamMUSIC;
				msg_music.fMaxDistance		= 100000.0f;
				msg_music.bLooped			= false;
				msg_music.i4LoopCount		= 0;
				msg_music.sndhndLoopStop	= 0;
				pc = pcLoadSample(pc,msg_music);
			}
		}
		else
		{
			bVoiceOver = false;
			bMusic = false;
		}

		//
		// Version 4 is bad.
		// Version 5 is bad.
		// Version 6 is a working version of version 4 and 5
		//
		AlwaysAssert(iVersion!=4);
		//AlwaysAssert(iVersion!=5);


		if (iVersion >= 6)
		{
			pc = pcLoadT(pc, &bAmbientMuted);
			pc = pcLoadT(pc, &sAmbientRestore);

			uint32 u4_count = 0;
			pc = pcLoadT(pc, &u4_ambient_samples);

			if (u4_ambient_samples>0)
			{
				pmsg_amb = new CMessageAudio[u4_ambient_samples];
				memset(pmsg_amb,0,sizeof(CMessageAudio)*u4_ambient_samples);
			}

			while (u4_count<u4_ambient_samples)
			{
				new(pmsg_amb+u4_count) CMessageAudio(padAudioDaemon,NULL);

				pc = pcLoadSample(pc,pmsg_amb[u4_count]);

				bool	b_looped;
				int32	i4_loop_count;
				float	f_max_dist;
				uint32	u4_flags;
				float	f_restore;

				pc = pcLoadT(pc, &b_looped);
				pc = pcLoadT(pc, &i4_loop_count);
				pc = pcLoadT(pc, &f_max_dist);
				pc = pcLoadT(pc, &u4_flags);
				pc = pcLoadT(pc, &f_restore);

				// Master volume is the ambient resotre volume, the master volume saved may be a muted volume.
				pmsg_amb[u4_count].fMasterVolume = f_restore;

				if (u4_flags & AMBIENT_NEVER_MUTE)
				{
					pmsg_amb[u4_count].u4Flags = eamPERMAMBIENT;
				}
				else
				{
					pmsg_amb[u4_count].u4Flags = eamAMBIENT;
				}
				pmsg_amb[u4_count].fMaxDistance = f_max_dist;

				pmsg_amb[u4_count].bLooped		= b_looped;
				pmsg_amb[u4_count].i4LoopCount	= i4_loop_count;

				u4_count++;
			}
		}
	}
	else
	{
		AlwaysAssert("Unknown version of audio settings");
	}

	if (padatEffects)
		padatEffects->Reset();

	if (padatStreams)
		padatStreams->Reset();

	if (padatAmbient)
		padatAmbient->Reset();

	// do we need to restore a voice over??
	if (b_voice)
	{
		msg_voice.Dispatch();
	}

	// Do we need to restore music??
	if (b_music)
	{
		msg_music.Dispatch();
	}
	
	// if we have a block of ambient samples, delete them
	if (pmsg_amb)
	{
		// Start all the ambients;
		for (uint32 u4_c=0; u4_c<u4_ambient_samples; u4_c++)
		{
			pmsg_amb[u4_c].Dispatch();
		}
		delete[] pmsg_amb;
	}

	bExit = false;

	return pc;
}


//*****************************************************************************************
//
void CAudioDaemon::SaveDefaults()
{
	char ac_buffer[4096];
	char *pc_end;

	pc_end = pcSave(ac_buffer);

	if (pc_defaults)
		delete pc_defaults; 

	int i_len = pc_end - ac_buffer;
	pc_defaults = new char[i_len];

	if (pc_defaults)
		memcpy(pc_defaults, ac_buffer, i_len);
}


//*****************************************************************************************
//
void CAudioDaemon::RestoreDefaults()
{
	if (pc_defaults)
		pcLoad(pc_defaults);
}


//*********************************************************************************************
// This will create a sample from a audio message and set all of the correct flags based on the
// contents of the message. The returned sample is ready for playing.
//
CSample* CAudioDaemon::psamCreateMessageStream
(
	const CMessageAudio& msg
)
//
//**************************************
{
	// load the voice over specified in the message
	uint32		u4_flags;
	CSample*	psam;

	Assert ((msg.fVolume<=0.0f) && (msg.fVolume>=-100.0f));
	Assert ((msg.fMasterVolume<=0.0f) && (msg.fMasterVolume>=-100.0f));
	Assert (msg.u4SpatialType<=2);
	Assert (msg.fAtten>=0.0f);
	Assert ((msg.fOutsideVolume<=0.0f) && (msg.fOutsideVolume>=-100.0f));
	Assert ((msg.fFrustumAngle>=0.0f) && (msg.fFrustumAngle<=360.0f));

	// We can only have a loop stop sample for an effect
	Assert (msg.sndhndLoopStop == 0);

	u4_flags = AU_CREATE_STREAM;

	switch (msg.u4SpatialType)
	{
	case 0:				// stereo
		u4_flags |= AU_CREATE_CTRL_VOLUME;
		break;

	case 1:				// Pseudo
		u4_flags |= (AU_CREATE_PSEUDO_3D | AU_CREATE_CTRL_VOLUME);
		break;

	case 2:				// spatial
		u4_flags |= (AU_CREATE_SPATIAL_3D | AU_CREATE_CTRL_VOLUME);
		break;

	default:
		Assert(0);
		break;
	}

	if ( (msg.u4GetAudioType() == eamVOICEOVER) && (u4GetFeatures(AUDIO_FEATURE_VOICEOVER)==0) )
	{
		// if voiceovers are not enabled we need to create the sample so the sub title works
		u4_flags |= AU_CREATE_NULL;
	}

	psam = psamCreateSampleWithRetry(msg.sndhndSample, padatStreams, u4_flags);

	// we failed to create the sample (or maybe the loader) so we have little
	// option but to ignore the sound and continue the game,
	if (psam == NULL || bSampleError(psam))
	{
		dprintf("Failed to create streamed sample '%x'\n", (uint32)msg.sndhndSample);
		return NULL;
	}

	SetupSample(psam,msg,true);
	return psam;
}




//*********************************************************************************************
// Create a new voice over...
//
void CAudioDaemon::CreateVoiceover
(
	const CMessageAudio& msg
)
//
//**************************************
{
	CSample* psam;

	psam = psamCreateMessageStream(msg);

	// Remeber what the voice over is...
	msgAudioVoiceOver = msg;

	// we have failed to create a sample...
	if (psam == NULL || bSampleError(psam))
	{
		msgAudioVoiceOver.sndhndSample = 0;
		return;
	}

	if (bVoiceOver)
	{
		// a voice over is already playing so add this to a play list. This will be played
		// when the current voice over is done.
		DelayVoiceOver(psam);
		return;
	}

	// if there is music playing and audiable voice overs are on, adjust the music volume by 10dB
	if (bMusic && u4GetFeatures(AUDIO_FEATURE_VOICEOVER))
	{
		float f_mvol = psamMusic->fGetVolume() - MUSIC_VOLUME_ADJUST;

		// we do not know what the music volume is so clamp it
		if (f_mvol<=-100.0f)
			f_mvol = -100.0f;

		psamMusic->SetVolume( f_mvol);
	}

	if (psam->bPlay(AU_PLAY_ONCE) == false)
	{
		// we have failed to play so we may as well delete the sample because it of no use
		// to us.
		delete psam;
		return;
	}

	// leave 2 seconds at the end to allow for timer errors, this also gives a nice delay between
	// queued voice over
	//sVoiceFinish = CMessageStep::sElapsedRealTime+psam->fSamplePlayTime()+fVOICEOVER_EXTEND;
	sndhndVoiceOver = msg.sndhndSample;
	bVoiceOver = true;
	psamVoiceOver = psam;
}


//*********************************************************************************************
void CAudioDaemon::ReplayLastVoiceover
(
)
//**************************************
{
	// if a voice over is playing (or queued) or the last message contains a zero sound handle 
	// then we cannot play the last voice over.
	if ((bVoiceOver) || (msgAudioVoiceOver.sndhndSample == 0))
		return;

	CreateVoiceover(msgAudioVoiceOver);	
}



//*********************************************************************************************
// Create a new music stream
//
void CAudioDaemon::CreateMusic
(
	const CMessageAudio& msg
)
//
//**************************************
{
	CSample* psam;

	// if music is already playing ignore the message
	if (bMusic)
	{
		return;
	}

	psam = psamCreateMessageStream(msg);
	if (psam == NULL || bSampleError(psam))
		return;

	// if there is a voice over playing the play this music piece quiter
	if (bVoiceOver)
	{
		float f_mvol = psam->fGetVolume() - MUSIC_VOLUME_ADJUST;

		// we do not know what the music volume is so clamp it
		if (f_mvol<=-100.0f)
			f_mvol = -100.0f;

		psam->SetVolume( f_mvol);
	}

	if (psam->bPlay(AU_PLAY_ONCE) == false)
	{
		// we have failed to play so we may as well delete the sample because it of no use
		// to us.
		delete psam;
		return;
	}

	sndhndMusic = msg.sndhndSample;
	bMusic = true;
	psamMusic = psam;
}



//*********************************************************************************************
// Create a new music stream
//
void CAudioDaemon::CreateAmbient
(
	const CMessageAudio& msg
)
//
//**************************************
{
	CSample*		psam;
	uint32			u4_activate_element;

	// We can only have a loop stop sample for an effect
	Assert (msg.sndhndLoopStop == 0);

	// this will either return NULL, meaning we have to load the sample or a CSample
	// pointer. This pointer could be an existing sample that is not playing or it
	// could be an instance of an exiting sample.
	psam = psamFindInAmbientList(msg.sndhndSample, msg.u4SpatialType, u4_activate_element);

	// we did not find a sample with the correct handle in the list
	if (psam == NULL)
	{
		uint32	u4_flags;

		AUMSG("Ambient Load..\n");

		Assert ((msg.fVolume<=0.0f) && (msg.fVolume>=-100.0f));
		Assert (msg.u4SpatialType<=2);
		Assert (msg.fAtten>=0.0f);
		Assert ((msg.fOutsideVolume<=0.0f) && (msg.fOutsideVolume>=-100.0f));
		Assert ((msg.fFrustumAngle>=0.0f) && (msg.fFrustumAngle<=360.0f));

		// an ambient is loaded as a defered sample, so there could be a small delay before it
		// is heard.
		u4_flags = AU_CREATE_STATIC | (msg.bDeferLoad?AU_CREATE_DEFER_LOAD_ON_PLAY:0);

		switch (msg.u4SpatialType)
		{
		case 0:				// stereo
			u4_flags |= AU_CREATE_CTRL_VOLUME;
			break;

		case 1:				// Pseudo
			u4_flags |= (AU_CREATE_PSEUDO_3D | AU_CREATE_CTRL_VOLUME);
			break;

		case 2:				// spatial
			u4_flags |= (AU_CREATE_SPATIAL_3D | AU_CREATE_CTRL_VOLUME);
			break;

		default:
			Assert(0);
			break;
		}
		psam = CAudio::psamCreateSample(msg.sndhndSample, padatAmbient, u4_flags); 

		if (psam == NULL || bSampleError(psam))
		{
			// If we failed to create the sample the do nothing else
			dprintf("failed to create ambient..\n");
			return;
		}
	}
	else if (bSampleError(psam))
	{
		// we have tried to instance a sample that is still loading, just ignore the request
		return;
	}

	// if we do not have an element we have instanced an existing element or loaded a new one so need to add it
	// to the cache..
	if (u4_activate_element == 0xffffffff)
	{
		// this will activate the element as well as insert it to the list
		u4_activate_element = AddToAmbientList(psam, msg.sndhndSample, msg.bLooped, msg.i4LoopCount, msg.fMaxDistance);
	}
	else
	{
		// start the ambient playing if we found an unused element in the ambient cache
		amsndAmbients[u4_activate_element].u4Flags	= 0;
		ActivateAmbientElement(psam, msg.sndhndSample, msg.bLooped, msg.i4LoopCount, msg.fMaxDistance, u4_activate_element);
	}

	float f_master_vol = msg.fMasterVolume;

	amsndAmbients[u4_activate_element].fRestoreVolume = f_master_vol;

	//
	// Is this sample a permanent ambient?? If so sets its never mute flag so it will not get muted when
	// a dino howels.
	// If it is a normal ambient then clear the never mute flag so it will be quite when a dino talks.
	//
	if (msg.u4GetAudioType() == eamPERMAMBIENT)
	{
		amsndAmbients[u4_activate_element].u4Flags |= AMBIENT_NEVER_MUTE;
	}
	else
	{
		amsndAmbients[u4_activate_element].u4Flags &= ~AMBIENT_NEVER_MUTE;
	}

	if ( (bAmbientMuted) && ((amsndAmbients[u4_activate_element].u4Flags & AMBIENT_NEVER_MUTE) == 0) )
	{
		// Ambient
		f_master_vol = -100.0f;
		amsndAmbients[u4_activate_element].u4Flags |= AMBIENT_MUTED;
	}
	else
	{
		// Our sample is not currently muted either because ambients are not in a muted state or because this sample
		// cannot be muted.
		amsndAmbients[u4_activate_element].u4Flags &= ~AMBIENT_MUTED;
	}

	//
	// attach the ambient sample to the correct object..
	//

	switch (msg.u4SpatialType)
	{
		// stereo
	case 0:
		Assert(msg.pinsParent);
		psam->SetVolume(msg.fVolume);
		psam->SetMasterVolume(f_master_vol);
		// we attach stereo samples to obejcts just so they have a position in the world, this
		// has to be known so we can stop the sound if we leave the trigger
		AttachSoundToObject(msg.pinsParent,psam, msg.bLooped);
		break;

		// pseudo
	case 1:
		Assert(msg.pinsParent);
		psam->SetVolume(msg.fVolume);
		psam->SetMasterVolume(f_master_vol);
		psam->SetAttenuation(msg.fAtten, msg.fMaxVolDistance);
		AttachSoundToObject(msg.pinsParent,psam,msg.bLooped);
		break;

		// real 3D
	case 2:
		Assert(msg.pinsParent);
		psam->SetVolume(msg.fVolume);
		psam->SetMasterVolume(f_master_vol);
		psam->SetAttenuation(msg.fAtten);
		AttachSoundToObject(msg.pinsParent,psam, msg.bLooped);

		// only set the frustum if it is going to have an effect
		if ((msg.fFrustumAngle<360.0f) && (msg.fOutsideVolume<0.0f))
		{
			psam->SetFrustum(msg.fFrustumAngle, msg.fOutsideVolume);
		}
		break;

		// unknown spatial type
	default:
		Assert(0);
		break;
	}

	//
	// Play the ambient
	//
	if (psam->bPlay(msg.bLooped?AU_PLAY_LOOPED:AU_PLAY_ONCE) == false)
	{
		// we have failed to play so we may as well delete the sample because it of no use
		// to us.
		amsndAmbients[u4_activate_element].psam		= NULL;
		amsndAmbients[u4_activate_element].shSample	= 0;
		amsndAmbients[u4_activate_element].u4Flags	= 0;
		amsndAmbients[u4_activate_element].bActive	= false;
		delete psam;
		return;
	}
}







//*********************************************************************************************
// Process audio messages: create sounds as necessary.
void CAudioDaemon::Process(const CMessageAudio& msg)
{
	switch (msg.u4GetAudioType())
	{
	case eamVOID:
		return;

	case eamVOICEOVER:
		CreateVoiceover(msg);
		break;

    case eamREPLAY_VOICEOVER:
        ReplayLastVoiceover();
        break;

	case eamPERMAMBIENT:
	case eamAMBIENT:
		// if ambient sounds are not enabled, return
		if (u4GetFeatures(AUDIO_FEATURE_AMBIENT) == 0)
			return;
		CreateAmbient(msg);
		break;

	case eamMUSIC:
		// if music is not enabled, return
		if (u4GetFeatures(AUDIO_FEATURE_MUSIC) == 0)
			return;
		CreateMusic(msg);
		break;

	case eamATTACHED_EFFECT:
	case eamPOSITIONED_EFFECT:
	case eamDINO_VOCAL_EFFECT:
		bPlaySoundEffect(msg,NULL);
		break;

	case eamCTRL_FADEMUSIC:
		// if there is no music playing then we just ignore the fade request
		if (psamMusic==NULL)
			return;

		// the volume is the fader value for this message
		CAudio::pcaAudio->FadeSample(psamMusic, msg.fDeltaVolume, msg.bStopAfterFade);
		break;
	}
}


//*********************************************************************************************
//
CSample* CAudioDaemon::psamFindInCollisionCache
(
	TSoundHandle	th_sound,
	uint32			u4_flags,
	uint32			u4_id,
	TSoundHandle	sndhnd_stop
)
//
//**************************************
{
	uint32		u4;
	uint32		u4_sample_status;
	uint32		u4_instance = 0xffffffff;
	uint32		u4_oldest_playing=0xffffffff;		// index of the oldest playing sample
	float		f_oldest_playing=CMessageStep::sElapsedRealTime+100000.0f;	// time of the oldest is sometime in the future
	bool		b_instance = false;
	uint32		u4_total = 0;			// total number of instances of this sample that I have found
	CSample*	psam;

	for (u4=0; u4<COLLISION_CACHE_LENGTH; u4++)
	{
		if (acceCollisions[u4].shSample == th_sound)
		{
			u4_total++;

			// we have found an entry of the correct sound material, if it is playing we have
			// to instance it, we cannot return its pointer
			u4_sample_status = acceCollisions[u4].psam->u4Status();

			if (u4_sample_status == AU_BUFFER_STATUS_NOTPLAYING)
			{
				//this sample is not playing so we can use it...
				AUMSG("Collision cache hit (direct)..\n");
				acceCollisions[u4].sTime = CMessageStep::sElapsedRealTime;

				// Is this sample supposed to be looped??
				if (u4_flags & COLLISION_CACHE_LOOPED)
				{
					Assert(u4_id != 0);
					if (((acceCollisions[u4].u4Flags & (COLLISION_CACHE_LOOPED|COLLISION_CACHE_LOOP_UPDATE)) != 0) && (u4_id != acceCollisions[u4].u4ID))
					{
						// we cannot use this guy because he is flagged to be looped and the loop id is different, but we can instance from it
						if (acceCollisions[u4].sTime<f_oldest_playing)
						{
							f_oldest_playing = acceCollisions[u4].sTime;
							u4_oldest_playing = u4;
						}
						u4_instance = u4;				
						continue;
					}
					acceCollisions[u4].u4Flags = (COLLISION_CACHE_LOOPED|COLLISION_CACHE_LOOP_UPDATE);
					acceCollisions[u4].u4ID = u4_id;
				}
				else
				{
					acceCollisions[u4].u4Flags = 0;
				}

				// make sure the effect flag is set for this element..
				if (u4_flags & COLLISION_CACHE_EFFECT)
				{
					acceCollisions[u4].u4Flags |= COLLISION_CACHE_EFFECT;
				}

				acceCollisions[u4].shStopSample = sndhnd_stop;
				return acceCollisions[u4].psam;
			}
			else if (u4_sample_status == AU_BUFFER_STATUS_PLAYING)
			{
				if (acceCollisions[u4].sTime<f_oldest_playing)
				{
					f_oldest_playing = acceCollisions[u4].sTime;
					u4_oldest_playing = u4;
				}

				u4_instance = u4;
			}
			else
			{
				// the buffer is lost, corrupt, faulty..Kill it and empty the cache entry.
				// Keep searching because we may find a good buffer.
				AUMSG("Collision cache entry lost..\n");
				RemoveSoundAttachment(acceCollisions[u4].psam);
				delete acceCollisions[u4].psam;
				acceCollisions[u4].psam = NULL;
			}
		}
	}

	// we need to create an instance of the cache element u4_instance...
	if (u4_instance!=0xffffffff)
	{
		if (u4_total>=COLLISION_CACHE_MAXINSTANCE)
		{
			AUMSG("Collision cache hit (Reuse oldest)..\n");
			// we cannot instance because we have used all of the available instances for this sample so lets use
			// oldest one and use it again!
			acceCollisions[u4_oldest_playing].sTime = CMessageStep::sElapsedRealTime;
			acceCollisions[u4_oldest_playing].u4Flags = 0;

			// make sure the effect flag is set for this element..
			if (u4_flags & COLLISION_CACHE_EFFECT)
			{
				acceCollisions[u4_oldest_playing].u4Flags |= COLLISION_CACHE_EFFECT;
			}

			// Is this sample supposed to be looped??
			if (u4_flags & COLLISION_CACHE_LOOPED)
			{
				Assert(u4_id != 0);
				acceCollisions[u4_oldest_playing].u4Flags |= (COLLISION_CACHE_LOOPED|COLLISION_CACHE_LOOP_UPDATE);
				acceCollisions[u4_oldest_playing].u4ID = u4_id;
			}

			acceCollisions[u4_oldest_playing].shStopSample = sndhnd_stop;
			return acceCollisions[u4_oldest_playing].psam;
		}
		else
		{
			uint32 u4_inst_count = 0;

			AUMSG("Collision cache hit (Instance)..\n");
			psam = acceCollisions[u4_instance].psam->psamCreateInstance();

			//
			// If NULL is returned we cannot instance this sample so do not bother trying again,
			// In this case we return an error code to stop us loading it agaib.
			if (psam == NULL)
				return psamINSTANCE_IGNORE;

			while (bSampleError(psam) && u4_inst_count<4)
			{
				if (psam == psamINSTANCE_FAILED)
				{
					// Remove an existing instance from the cache, but not one of our own!
					// Only do this if the failure was due to a duplicate call failing. Do not
					// remove a cache entry if we failed to instance because of a loading sample.
					RemoveCollisionCacheEntry(acceCollisions[u4_instance].shSample);
				}

				// try to instance it again
				psam = acceCollisions[u4_instance].psam->psamCreateInstance();

				if (psam == NULL)
					return psamINSTANCE_IGNORE;

				u4_inst_count++;
			}

			// We should now not have an error
			// A NULL sample cannot get to here
			if (!bSampleError(psam))
			{
				AddToCollisionCache(psam, acceCollisions[u4_instance].shSample, u4_flags, u4_id, sndhnd_stop);
				//dprintf("Sample instanced..\n");
			}

			return psam;
		}
	}

	//dprintf("Sample not instanced..\n");
	// not in the cahce
	return NULL;
}




//*********************************************************************************************
//
void CAudioDaemon::AddToCollisionCache
(
	CSample*		psam, 
	TSoundHandle	sh,
	uint32			u4_flags,
	uint32			u4_id,
	TSoundHandle	sndhnd_stop
)
//
//**************************************
{
	TSec	s_oldest_time = CMessageStep::sElapsedRealTime + 10000.0f;		// oldest time is 10000 seconds from now
	uint32	u4;
	uint32	u4_entry = 0;

	// We cannot specify the loop update flag
	Assert((u4_flags & COLLISION_CACHE_LOOP_UPDATE) == 0);

	for (u4=0; u4<COLLISION_CACHE_LENGTH; u4++)
	{
		if (acceCollisions[u4].psam == NULL)
		{
			AUMSG("cache fill (empty)..\n");

			acceCollisions[u4].sTime			= CMessageStep::sElapsedRealTime;
			acceCollisions[u4].shSample			= sh;
			acceCollisions[u4].psam				= psam;
			acceCollisions[u4].u4Flags			= u4_flags;
			acceCollisions[u4].u4ID				= u4_id;
			acceCollisions[u4].shStopSample		= sndhnd_stop;

			if (u4_flags & COLLISION_CACHE_LOOPED)
			{
				// If a sample is looped then it must have a valid ID.
				Assert(u4_id != 0);
				acceCollisions[u4_entry].u4Flags |= COLLISION_CACHE_LOOP_UPDATE;
			}

			return;
		}
		else
		{
			// if we are inserting a collision it cannot remove an effect that is playing
			if (((u4_flags & COLLISION_CACHE_EFFECT) == false) &&						// We are adding a collision
				(acceCollisions[u4].u4Flags & COLLISION_CACHE_EFFECT) &&				// current element is an effect
				(acceCollisions[u4].psam->u4Status() == AU_BUFFER_STATUS_PLAYING) )		// and it is playing
			{
				continue;
			}

			if (acceCollisions[u4].sTime<s_oldest_time)
			{
				s_oldest_time = acceCollisions[u4].sTime;
				u4_entry = u4;
			}
		}
	}

	//if we get here there is no room in the cache but we have found the oldest one
	//which we will evict.
	AUMSG("cache fill (evict)..\n");

	RemoveSoundAttachment(acceCollisions[u4_entry].psam);
	delete acceCollisions[u4_entry].psam;

	acceCollisions[u4_entry].sTime		= CMessageStep::sElapsedRealTime;
	acceCollisions[u4_entry].shSample	= sh;
	acceCollisions[u4_entry].psam		= psam;

	// Set the flags for this entry
	acceCollisions[u4_entry].u4Flags	= u4_flags;

	// If this sample is looped then enure that it is flagged as updated for this frame, it if is
	// not updated on the next frame it will be stopped.
	if (u4_flags & COLLISION_CACHE_LOOPED)
	{
		// If a sample is looped then it must have a valid ID.
		Assert(u4_id != 0);
		acceCollisions[u4_entry].u4Flags |= COLLISION_CACHE_LOOP_UPDATE;
	}

	acceCollisions[u4_entry].shStopSample	= sndhnd_stop;
	acceCollisions[u4_entry].u4ID			= u4_id;
}


//*********************************************************************************************
// This will set the loop update flag of the collision cahce entry with the specified ID and
// enable it to loop for another frame.
//
uint32 CAudioDaemon::u4UpdateCollisionLoopStatus
(
	uint32			u4_id
)
//
//**************************************
{
	for (uint32 u4=0; u4<COLLISION_CACHE_LENGTH; u4++)
	{
		if (acceCollisions[u4].u4ID == u4_id)
		{
			// we have found our ID
			acceCollisions[u4].u4Flags |= COLLISION_CACHE_LOOP_UPDATE;
			acceCollisions[u4].sTime = CMessageStep::sElapsedRealTime;		// Update the time so it does not get evicted.
			//dprintf("Slide entry %x found.\n", u4_id);
			return u4;
		}
	}
	//dprintf("Slide entry %x not found.\n", u4_id);
	return 0xffffffff;
}




//*********************************************************************************************
// Go through the collision cache and stop any sample to is looping and has not been updated
//
void CAudioDaemon::ProcessLoopedCollisions
(
)
//
//**************************************
{
	for (uint32 u4=0; u4<COLLISION_CACHE_LENGTH; u4++)
	{
		if ( (acceCollisions[u4].u4Flags & (COLLISION_CACHE_LOOPED|COLLISION_CACHE_LOOP_UPDATE)) == COLLISION_CACHE_LOOPED)
		{
			// we have found a looped sample that has not been updated so stop it, do not delete
			// it so then it will hang around inside the cache

			if (acceCollisions[u4].shStopSample)
			{
				//
				// Before we stop the current playing sample, lets start the release sample playing. We do this so if
				// the release sample has to be loaded there is not a delay between stopping the looping sample and 
				// playing the release sample.
				//

				uint32 u4_create = 0;

				if (acceCollisions[u4].psam->u4CreateFlags & AU_CREATE_STEREO)
				{
					u4_create = AU_STEREO;
				}
				else if (acceCollisions[u4].psam->u4CreateFlags & AU_CREATE_SPATIAL_3D)
				{
					u4_create = AU_SPATIAL3D;
				}
				else if (acceCollisions[u4].psam->u4CreateFlags & AU_CREATE_PSEUDO_3D)
				{
					u4_create = AU_PSEUDO3D;
				}

				CMessageAudio	msg
				(
					acceCollisions[u4].shStopSample,
					eamPOSITIONED_EFFECT,		// type of sound
					NULL,						// sender (NULL if using play function)
					NULL,						// receiver (NULL if using play function)
					NULL,						// parent of the effect (can be NULL for stereo or positioned effects)
					acceCollisions[u4].psam->fGetVolume(),		// volume 0dBs
					acceCollisions[u4].psam->fAtten,						// attenuation (only for pseudo/real 3D samples)
					u4_create,					// spatial type
					360.0f,						// fustrum angle (real 3D only)
					-15.0f,						// outside volume (real 3D only)
					false,						// looped
					0,							// loop count
					acceCollisions[u4].psam->fMaxVolDistance,	// distance before sound is stopped (pseudo/real 3D only),
					0,
					acceCollisions[u4].psam->fGetMasterVolume()
				);

				//
				// Directly set the position of the sample.
				// This can only be done for positined effects. Attached effects must be attached to an instance
				//
				msg.SetPositionalInfo
				(
					CVector3<>(	acceCollisions[u4].psam->fWorldX,
								acceCollisions[u4].psam->fWorldY,
								acceCollisions[u4].psam->fWorldZ )
				);

				bImmediateSoundEffect(msg,NULL);				
			}

			// If this sample is attached to something then make sure we remove it
			if (acceCollisions[u4].psam->u4CreateFlags & AU_ATTACHED)
			{
				// We are about to stop an attached looping sample
				// So remove its attachment
				RemoveSoundAttachment(acceCollisions[u4].psam);
			}

			//dprintf("Slide entry %x stopped\n", acceCollisions[u4].u4ID);
			acceCollisions[u4].psam->Stop();
			acceCollisions[u4].u4ID = 0;
			acceCollisions[u4].u4Flags &= ~COLLISION_CACHE_LOOPED;
			AUMSG("Looped effect stopped.\n");
		}
		acceCollisions[u4].u4Flags &= ~COLLISION_CACHE_LOOP_UPDATE;
	}
}


//*********************************************************************************************
// This function will remove the 3D buffer from the cache in an attempt to free up hardware
// resources. Collisions will be removed before effects.
// This function should never be called if software 3D sound is being used.
//
void CAudioDaemon::RemoveCollisionCacheEntry
(
	TSoundHandle	sndhnd_ignore		// zero is the default parameter
)
//
//**************************************
{
	TSec	s_oldest_time = CMessageStep::sElapsedRealTime + 10000.0f;		// oldest time is 10000 seconds from now
	TSec	s_oldest_playing_time = s_oldest_time;
	uint32	u4;
	uint32	u4_entry = 0xffffffff;
	uint32	u4_entry_playing = 0xffffffff;

	// Make sure we are using hardware
	Assert(CAudio::pcaAudio->bUsingHardware());

	for (u4=0; u4<COLLISION_CACHE_LENGTH; u4++)
	{
		if (acceCollisions[u4].psam != NULL)
		{
			// This is not a 3D sample so we can ignore it
			if (acceCollisions[u4].psam->pDS3DBuffer == 0)
				continue;

			Assert(acceCollisions[u4].shSample!=0);
			if (sndhnd_ignore == acceCollisions[u4].shSample)
				continue;

			// Sample is playing so do not consider it
			if ((acceCollisions[u4].psam->u4Status() == AU_BUFFER_STATUS_PLAYING))
			{
				if (acceCollisions[u4].sTime<s_oldest_playing_time)
				{
					s_oldest_playing_time = acceCollisions[u4].sTime;
					u4_entry_playing = u4;
				}
				continue;
			}

			if (acceCollisions[u4].sTime<s_oldest_time)
			{
				s_oldest_time = acceCollisions[u4].sTime;
				u4_entry = u4;
			}
		}
	}

	// Now we have been through the array and we should have the oldest sample and the oldest playing sample
	if (u4_entry != 0xffffffff)
	{
		RemoveSoundAttachment(acceCollisions[u4_entry].psam);
		delete acceCollisions[u4_entry].psam;
		acceCollisions[u4_entry].psam		= NULL;
		acceCollisions[u4_entry].sTime		= 0;
		acceCollisions[u4_entry].shSample	= 0;
		acceCollisions[u4_entry].u4Flags	= 0;
		return;
	}

	// We did not have an oldest sample for non-playing samples so we must stop a playing sample.
	if (u4_entry_playing != 0xffffffff)
	{
		RemoveSoundAttachment(acceCollisions[u4_entry_playing].psam);
		delete acceCollisions[u4_entry_playing].psam;
		acceCollisions[u4_entry_playing].psam		= NULL;
		acceCollisions[u4_entry_playing].sTime		= 0;
		acceCollisions[u4_entry_playing].shSample	= 0;
		acceCollisions[u4_entry_playing].u4Flags	= 0;
		return;
	}

	//
	// If we get to here then no sample has been removed because the cache is empty.
	//
}


//*********************************************************************************************
//
CSample* CAudioDaemon::psamCreateCollision
(
	TSoundHandle	sndhnd,
	uint32			u4_id		// set to non zero for sliding collisions
)
//
//**************************************
{
	CSample*		psam;

	Assert(sndhnd!=0);

	if (padatEffects==NULL)
		return 0;

	// look for a sample with the given properties in the collision cache
	// this will create an instance of the sample if it is in the cache but is in use.
	if (u4_id)
	{
		psam = psamFindInCollisionCache(sndhnd,COLLISION_CACHE_EFFECT|COLLISION_CACHE_LOOPED, u4_id);
	}
	else
	{
		psam = psamFindInCollisionCache(sndhnd,0);
	}

	//
	// If the instancer fails to create a new sample, it is probably because the hardware
	// has ran out of channels. In this case do not load a software version of the sample
	// because it sounds different. It is better just to miss the collision.
	//
	if (bSampleError(psam))
		return NULL;

	// if we did not find sample 1 in the cache, load it and add it to the cache
	if (psam == NULL)
	{
		AUMSG("Loading sample...\n");

		psam  = psamCreateSampleWithRetry( sndhnd, padatEffects, AU_CREATE_SPATIAL_3D | AU_CREATE_STATIC |
										AU_CREATE_CTRL_FREQUENCY | AU_CREATE_CTRL_VOLUME /*| AU_CREATE_DEFER_LOAD_ON_PLAY*/);

		if (psam == NULL || bSampleError(psam))
			return NULL;

		if (u4_id)
		{
			AddToCollisionCache(psam, sndhnd,COLLISION_CACHE_EFFECT|COLLISION_CACHE_LOOPED, u4_id);
		}
		else
		{
			AddToCollisionCache(psam, sndhnd);
		}
	}

	return psam;
}


extern CProfileStat psCollisionMsgAudio;

//*********************************************************************************************
// Process collision messages, usually sent from the physics system.
//
// This is responsible for looking up the sound materials in the sound database and playing
// them in 3D space with respect to their velocity based transfer functions.
//
// If one of the two sound materials is zero then the zero material is replaced by the other.
//
void CAudioDaemon::Process(const CMessageCollision& msg)
{
	CTimeBlock tmb(&psCollisionMsgAudio);

	//
	// Make sure the collision contains valid data
	//
#if 0
	AlwaysAssert(_finite(msg.rVelocityHit));
	AlwaysAssert(_finite(msg.rVelocitySlide));
	AlwaysAssert(_finite(msg.v3Position.tX));
	AlwaysAssert(_finite(msg.v3Position.tY));
	AlwaysAssert(_finite(msg.v3Position.tZ));
#endif


	//  HACK HACK HACK
	//   Brutal evil hack to avoid sounds on Anne's body unless she's dead.
	//   This is all so that we can make a thump sound when she dies and falls over.  Great, huh?
	if (!gpPlayer->bDead())
	{
		if (gpPlayer->apbbBoundaryBoxes[ebbBODY] == msg.pins1 ||
			gpPlayer->apbbBoundaryBoxes[ebbBODY] == msg.pins2)
		{
			return;
		}
	}


	//
	//  Now the real code....  Handle an audio collision sound.
	//

	float	f_vel = msg.fEnergyMaxNormLog();

	Assert ((f_vel<=1.0f) && (f_vel>=0.0f));

	//f_vel = 1.0f;

	// effects are not enabled or the database is invalid
	if (!padatEffects || (u4FeaturesEnabled & AUDIO_FEATURE_EFFECT)==0)
		return;

	// one of the sound materials is zero, not much use to us.
	if ((msg.smatSound1 == 0) || (msg.smatSound2 == 0))
		return;

	SAudioCollision* pcol = padatEffects->pcolFindCollision(u8CollisionHash(msg.smatSound1, msg.smatSound2));

	//
	// A collision between this pair of materials was not found
	//
	if (pcol == NULL)
	{
		return;
	}

	uint32 u4_col = pcol->u4CollisionSamples();

//	dout << "Normalized Vel: " << f_vel << " (Actual: " << msg.rVelocityHit << ")\n";

	//dout << "Normalized Vel: " << f_vel << " (Actual: " << msg.rVelocityHit << ")\n";

	//
	// Process the collisions, if a collision exists and the velocity is above zero
	//
	if ((msg.fEnergyMax > 0.0) && (u4_col>0))
	{
		//
		// We have a valid collision but can it be used??
		//
		if (CMessageStep::sStaticTotal>(pcol->fTimeLastUsed+pcol->fMinTimeDelay))
		{
			// Set the last used time.
			pcol->fTimeLastUsed = CMessageStep::sStaticTotal;

			Assert(u4_col<=2);

			CSample*	psam_mat1 = NULL;
			CSample*	psam_mat2 = NULL;

			// Is the velocity of this collision above its minimum??
			if (pcol->stTransfer[0].fMinVelocity< f_vel)
			{
				psam_mat1 =  psamCreateCollision( pcol->sndhndSamples[0] );
			}

			// if there is a second sample do the same again for sample 2
			if (u4_col == 2)
			{
				if (pcol->stTransfer[1].fMinVelocity< f_vel)
				{
					psam_mat2 =  psamCreateCollision( pcol->sndhndSamples[1] );
				}
			}

			// play sample 1 if it is valid.
			if (psam_mat1)
			{
				psam_mat1->bPlay( pcol->stTransfer[0], f_vel, 
					msg.v3Position.tX,msg.v3Position.tZ,msg.v3Position.tY, AU_PLAY_ONCE);
				//
				// Inform the AI system of the sound so dino's can hear, we only do this for sample 1,
				// sample 2 is not always present.
				//
				gaiSystem.Handle3DSound
				(
					msg.v3Position,
					-40.0f + (pcol->stTransfer[0].fCalculateVolume(f_vel)*40.0f),
					-pcol->stTransfer[0].fAttenuate
				);
			}

			// play sample 2 if it is valid and it is not a disabled terrain sound.
			if (psam_mat2 && (bTerrainSound || !ptCast<CTerrain>(msg.pins2)))
			{
				psam_mat2->bPlay( pcol->stTransfer[1], f_vel, 
					msg.v3Position.tX,msg.v3Position.tZ,msg.v3Position.tY, AU_PLAY_ONCE);
			}
		}
	}

	//
	// Process the slide component of this collision if one exists and the velocity
	// is above zero, and there is no hit component.
	//
	if ( /*(msg.fEnergyMax == 0.0) &&*/ (msg.fEnergySlide > 0.0) && (pcol->bSlide()) )
	{
		// Make a quick ID that should be unique for all sliding collisions.
		uint32 u4_id = (uint32)msg.smatSound1 + (uint32)msg.smatSound2 + (uint32)msg.pins1 + (uint32)msg.pins2;

/*		dprintf("Frame: %d\n\n", CMessageStep::u4Frame);
		dprintf("Slide Mat 1: %x\n", (uint32)msg.smatSound1);
		dprintf("Slide Mat 2: %x\n", (uint32)msg.smatSound2);
		dprintf("Slide pins 1: %x\n", (uint32)msg.pins1);
		dprintf("Slide pins 2: %x\n", (uint32)msg.pins2);
		dprintf("Hash id: %x\n", (uint32)u4_id);*/

		float  f_slide_vel	= msg.fEnergySlideNormLog();

		// get the cache entry of this slide collision, we do not want a stop sample
		uint32 u4_entry = u4UpdateCollisionLoopStatus(u4_id);

		if ( u4_entry == 0xffffffff)
		{
			// We have not found the collision with the specified ID so we need to create it

			if (pcol->stTransfer[2].fMinVelocity< f_slide_vel)
			{

				CSample*	psam_slide	=  psamCreateCollision( pcol->sndhndSamples[2], u4_id );;

				// play slide sample if it is valid.
				if (psam_slide)
				{
					psam_slide->bPlay( pcol->stTransfer[2], f_slide_vel, 
						msg.v3Position.tX,msg.v3Position.tZ,msg.v3Position.tY, AU_PLAY_LOOPED);
				}
				//dprintf("Start Sample.\n");
			}
		}
		else
		{
			Assert(u4_entry<COLLISION_CACHE_LENGTH);

			if (acceCollisions[u4_entry].psam)
			{
				// this sample is already playing so update its position
				acceCollisions[u4_entry].psam->SetTransfer(pcol->stTransfer[2], f_slide_vel, 
						msg.v3Position.tX,msg.v3Position.tZ,msg.v3Position.tY);
				//dprintf("Start Continue Sample.\n");
			}
		#if VER_TEST
			else
			{
				dprintf("Null looped sample in collision array.\n");
			}
		#endif
		}
		//dprintf("\n");
	}
}


extern CProfileStat psMoveMsgAudio;

//*********************************************************************************************
// Process move message for all objects.
//
// The code within this message is responsible for updating the position of sounds attached to
// moving objects
// The position of the listener is actually the camera not the player, for it is the view of
// the camera that the user sees on the screen so therefore is what sounds should be relative
// to. If the camera is attached to the player or the players head then the system functions
// as if the camera was on that object...
//
void CAudioDaemon::Process(const CMessageMove& msg_move)
{
	CTimeBlock tmb(&psMoveMsgAudio);

	// If this is a sleep or wake message then we do not care. We only care about genuine
	// move messages.
	if (msg_move.etType != CMessageMove::etMOVED)
		return;

	ptr<CCamera> pcam = CWDbQueryActiveCamera().tGet();

	// if the camera has moved update the 3D listeners position
	// AT THE MOMENT THIS IS A LITTLE INEFFICIENT BECAUSE THE CAMERA SENDS A MOVE
	// MESSAGE EVERY FRAME REGARDLESS OF WEATHER IT IS MOVING OR NOT.
	if (msg_move.pinsMover == (const CInstance*)pcam)
	{
		// get the position and orientation of the moved object in world space
		CPresence3<>	p3_pres		= msg_move.pinsMover->pr3Presence();

		// world space vectors pointing forwards and up
		CVector3<>		v3_in		= d3YAxis * p3_pres.r3Rot;	// forward vector
		CVector3<>		v3_up		= d3ZAxis * p3_pres.r3Rot;	// up vector

		// The player has moved we need to update the position of the listener.
		// Positons are already in meters it is just that the axis need to be flipped around
		// so they match with D3D.
		CAudio::pcaAudio->PositionListener(p3_pres.v3Pos.tX,p3_pres.v3Pos.tZ,p3_pres.v3Pos.tY,
									false);

		CAudio::pcaAudio->OrientListener(v3_in.tX,v3_in.tZ,v3_in.tY,
									v3_up.tX,v3_up.tZ,v3_up.tY, false);
	}
	else
	{
		// This instance does not have a sample attached so ignore it
		if (msg_move.pinsMover->bIsSampleAttached() == false)
			return;

		// We can stop looking once we have found a match because an instance can only have a
		// single sound attached to it.
		for (uint32 u4_sample_count = 0; u4_sample_count<MAX_ATTACHED_SOUNDS; u4_sample_count++)
		{
			if (asatSoundObjects[u4_sample_count].pins == msg_move.pinsMover)
			{
				// get the position and orientation of the moved object in world space
				CPresence3<>	p3_pres		= msg_move.pinsMover->pr3Presence();

				// world space vectors pointing forwards and up
				CVector3<>		v3_in		= d3YAxis * p3_pres.r3Rot;	// forward vector
				CVector3<>		v3_up		= d3ZAxis * p3_pres.r3Rot;	// up vector

#if VER_TEST
				AlwaysAssert((uint32)asatSoundObjects[u4_sample_count].psam > 32);
#endif

				asatSoundObjects[u4_sample_count].psam->SetPosition(p3_pres.v3Pos.tX,p3_pres.v3Pos.tZ,p3_pres.v3Pos.tY, false);
				asatSoundObjects[u4_sample_count].psam->SetOrientation(v3_in.tX,v3_in.tZ,v3_in.tY, false);

				return;
			}
		}
	}
}




//*********************************************************************************************
// Go through the attachment list to see if any have timed out...
//
void CAudioDaemon::ProcessAttachments
(
)
//
//**************************************
{
	for (uint32 u4_sample_count = 0; u4_sample_count<MAX_ATTACHED_SOUNDS; u4_sample_count++)
	{
		if (asatSoundObjects[u4_sample_count].pins)
		{
			// this element contains an instance
			if (asatSoundObjects[u4_sample_count].sDetatchTime>0.0f)
			{
				// we are timing a sample so we use real time
				if (CMessageStep::sElapsedRealTime>asatSoundObjects[u4_sample_count].sDetatchTime)
				{
					// It is time for this sample to be detached and this element freed.
					asatSoundObjects[u4_sample_count].pins->SetSampleAttached(false);
					asatSoundObjects[u4_sample_count].pins = NULL;
					asatSoundObjects[u4_sample_count].sDetatchTime = 0.0f;
					asatSoundObjects[u4_sample_count].psam->u4CreateFlags &= ~AU_ATTACHED;
					asatSoundObjects[u4_sample_count].psam = NULL;
				}
			}
		}
	}
}



//**********************************************************************************************
// Attach a sound to an object, the location and orientation of the sound is based on the
// presence of the instance. An instance can only have one sample attached to it.
//
void CAudioDaemon::AttachSoundToObject
(
	CInstance*	pins,
	CSample*	psam,
	bool		b_sample_looped
)
//
//**************************************
{
	uint32	u4_index = 0xffffffff;

	for (uint32 u4_sample_count = 0; u4_sample_count<MAX_ATTACHED_SOUNDS; u4_sample_count++)
	{
		// if the instance is already in the attachment list then replace its sample.
		if (asatSoundObjects[u4_sample_count].pins == pins)
		{
			u4_index = u4_sample_count;
			break;
		}

		//
		// Keep track of a NULL entry
		if (asatSoundObjects[u4_sample_count].psam == NULL)
		{
			// just set the index once, from the beginning of the array
			if (u4_index == 0xffffffff)
				u4_index = u4_sample_count;
			// Do not break, we have to go all the way through the list because we may still find
			// the instance
		}
	}

	// we have failed to find an empty slot
	Assert(u4_index!=0xffffffff);
	Assert(u4_index<MAX_ATTACHED_SOUNDS);

	// put us at the index we have found, check the index with code so release mode does
	// not crash.
	if (u4_index!=0xffffffff)
	{
		asatSoundObjects[u4_index].psam = psam;
		asatSoundObjects[u4_index].pins = pins;

		CPresence3<>	p3_pres		= pins->pr3Presence();
		// world space vectors pointing forwards and up
		CVector3<>		v3_in		= d3YAxis * p3_pres.r3Rot;	// forward vector

		// Position the sample
		asatSoundObjects[u4_index].psam->SetPosition(p3_pres.v3Pos.tX,p3_pres.v3Pos.tZ,p3_pres.v3Pos.tY, true);
		asatSoundObjects[u4_index].psam->SetOrientation(v3_in.tX,v3_in.tZ,v3_in.tY, true);

		// Mark the sample as attached to something
		asatSoundObjects[u4_index].psam->u4CreateFlags |= AU_ATTACHED;

		// Update the partition so we know that this sample is attached
		pins->SetSampleAttached(true);

		// set the detatch time
		if (b_sample_looped)
		{
			asatSoundObjects[u4_index].sDetatchTime = -1.0f;		// negative time means do not detatch.
		}
		else
		{
			asatSoundObjects[u4_index].sDetatchTime = CMessageStep::sStaticTotal + psam->fSamplePlayTime();
		}
	}
}




//**********************************************************************************************
// This will remove the attachment for the specified sample, only 1 object can be attached to
// a specific sample.
//
void CAudioDaemon::RemoveSoundAttachment
(
	CSample*	psam
)
//	
//**************************************
{
	for (uint32 u4_sample_count = 0; u4_sample_count<MAX_ATTACHED_SOUNDS; u4_sample_count++)
	{
		// if we have a sample match...
		if ((asatSoundObjects[u4_sample_count].psam == psam) && (asatSoundObjects[u4_sample_count].psam != NULL))
		{
			asatSoundObjects[u4_sample_count].pins->SetSampleAttached(false);

			asatSoundObjects[u4_sample_count].psam->u4CreateFlags &= ~AU_ATTACHED;
			asatSoundObjects[u4_sample_count].psam			= NULL;
			asatSoundObjects[u4_sample_count].pins			= NULL;
			asatSoundObjects[u4_sample_count].sDetatchTime	= 0.0f;
			return;
		}
	}
}


//**********************************************************************************************
CInstance* CAudioDaemon::pinsFindSampleAttachment 
(
	CSample*	psam
) const
//	
//**************************************
{
	for (uint32 u4_sample_count = 0; u4_sample_count<MAX_ATTACHED_SOUNDS; u4_sample_count++)
	{
		// if we have a sample match...
		if ((asatSoundObjects[u4_sample_count].psam == psam) && (psam != NULL))
		{
			return asatSoundObjects[u4_sample_count].pins;
		}
	}

	return NULL;
}


//**********************************************************************************************
// This will remove the attachment for the specified instance, only 1 sample can be attahced
// to a single instance.
//
void CAudioDaemon::RemoveSoundAttachment
(
	CInstance*	pins
)
//	
//**************************************
{
	for (uint32 u4_sample_count = 0; u4_sample_count<MAX_ATTACHED_SOUNDS; u4_sample_count++)
	{
		// if we have a sample match...
		if ((asatSoundObjects[u4_sample_count].pins == pins) && (pins != NULL))
		{
			asatSoundObjects[u4_sample_count].pins->SetSampleAttached(false);

			asatSoundObjects[u4_sample_count].psam->u4CreateFlags &= ~AU_ATTACHED;
			asatSoundObjects[u4_sample_count].psam			= NULL;
			asatSoundObjects[u4_sample_count].pins			= NULL;
			asatSoundObjects[u4_sample_count].sDetatchTime	= 0.0f;
			return;
		}
	}
}




//**********************************************************************************************
// This used to delay a voice over when another is already playing. The CSample that contains
// the voice over is added to a list which is processed when the current voice over finishes.
//
void CAudioDaemon::DelayVoiceOver
(
	CSample*	psam
)
//	
//**************************************
{
	for (uint32 u4=0; u4<MAX_DELAYED_VOICEOVER; u4++)
	{
		if (apsamVoiceOver[u4] == NULL)
		{
			apsamVoiceOver[u4] = psam;
			bDelayedVoiceOver = true;
			return;
		}
	}

	// there is no room in the delayed voiceover list.
	Assert(0);
}



//**********************************************************************************************
// Activate an existing element in the ambient list
//
void CAudioDaemon::ActivateAmbientElement
(
	CSample*		psam,
	TSoundHandle	sh,
	bool			b_loop,
	int32			i4_loop_count,
	float			f_dist,
	uint32			u4_activate
)
//	
//**************************************
{
	// we have an element to activate....
	AUMSG("Activate ambient element..\n");

	amsndAmbients[u4_activate].sTime	= CMessageStep::sElapsedRealTime;

	if (b_loop)
	{
		if (i4_loop_count < 0)
		{
			// if we are looped continous the end time is along way away, (about 30 months)
			amsndAmbients[u4_activate].sEndTime	= CMessageStep::sElapsedRealTime+100000000.0f;
		}
		else
		{
			Assert (i4_loop_count>0);

			// if we have a loop count the end time is sample length * loop count
			amsndAmbients[u4_activate].sEndTime	= CMessageStep::sElapsedRealTime + (psam->fSamplePlayTime() * i4_loop_count);
		}
		amsndAmbients[u4_activate].u4Flags |= AMBIENT_LOOP;
	}
	else
	{
		// if we are not looped then the end time is the current time+sample time
		amsndAmbients[u4_activate].sEndTime	= CMessageStep::sElapsedRealTime+psam->fSamplePlayTime();
		amsndAmbients[u4_activate].u4Flags &= ~AMBIENT_LOOP;
	}
	amsndAmbients[u4_activate].fMaxDistance	= f_dist*f_dist;

	amsndAmbients[u4_activate].psam		= psam;
	amsndAmbients[u4_activate].bActive	= true;
	amsndAmbients[u4_activate].shSample	= sh;
}



//**********************************************************************************************
// This function will add the specified ambient to the current ambient list
// It returns the cache element that we put it in, so we can modify it later
//
uint32 CAudioDaemon::AddToAmbientList
(
	CSample*		psam,
	TSoundHandle	sh,
	bool			b_loop,
	int32			i4_loop_count,
	float			f_dist
)
//	
//**************************************
{
	uint32	u4_inactive = 0xffffffff;
	TSec	s_oldest = CMessageStep::sElapsedRealTime + 100000000.0f;		// some time in the future

	AUMSG("Insert Ambient..\n");
	// we have no element so we have to search for an old one...
	uint32 u4_sample_count;
	for (u4_sample_count = 0; u4_sample_count<MAX_AMBIENT_SOUNDS; u4_sample_count++)
	{
		// if we have a NULL entry use it
		if (amsndAmbients[u4_sample_count].psam == NULL)
		{
			amsndAmbients[u4_sample_count].u4Flags	= 0;
			ActivateAmbientElement(psam, sh, b_loop, i4_loop_count, f_dist, u4_sample_count);
			return u4_sample_count;
		}
		else
		{
			// this sample is not NULL but is it inactive?
			if (!amsndAmbients[u4_sample_count].bActive)
			{
				// yes, this sample is inactive but is it the oldest inactive sample?
				if (amsndAmbients[u4_sample_count].sTime < s_oldest)
				{
					// we have found the oldest inactive sample..
					u4_inactive = u4_sample_count;
					s_oldest = amsndAmbients[u4_sample_count].sTime;
				}
			}
		}
	}

	// have we been all the way through the case and not found an empty or inactive sample?
	if (u4_inactive != 0xffffffff)
	{
		AUMSG("Insert Ambient (evict)..\n");

		Assert(amsndAmbients[u4_inactive].psam);

		// delete the oldest sample in the cache that is not playing
		delete amsndAmbients[u4_inactive].psam;

		amsndAmbients[u4_sample_count].u4Flags	= 0;
		ActivateAmbientElement(psam, sh, b_loop, i4_loop_count, f_dist, u4_inactive);
	}
	else
	{
		// too many ambient sounds playing, increase MAX_AMBIENT_SOUNDS
		Assert(0);
	}

	return u4_inactive;
}




//**********************************************************************************************
// this just goes through all the current ambients and checks if they need to be stopped or
// are out of range.
//
void CAudioDaemon::ProcessAmbientSounds
(
)
//	
//**************************************
{
	for (uint32 u4_sample_count = 0; u4_sample_count<MAX_AMBIENT_SOUNDS; u4_sample_count++)
	{
		// if this element is active...
		if (amsndAmbients[u4_sample_count].bActive)
		{
			float	f_x,f_y,f_z;

			// we must have a valid psam if we are active
			Assert(amsndAmbients[u4_sample_count].psam);

			// defered loading samples are in the ambient list so they can be instanced
			// but are ignored. They will get activated later when their load cycle has
			// finished
			if (amsndAmbients[u4_sample_count].psam->bSampleLoading())
			{
				AUMSG("Ambient loading, skipped..\n");
				continue;
			}

			// get the position of the sample...
			f_x = amsndAmbients[u4_sample_count].psam->fWorldX;
			f_y = amsndAmbients[u4_sample_count].psam->fWorldY;
			f_z = amsndAmbients[u4_sample_count].psam->fWorldZ;

			// update the time stap of this sample..
			amsndAmbients[u4_sample_count].sTime = CMessageStep::sElapsedRealTime;

			if ((CMessageStep::sElapsedRealTime > amsndAmbients[u4_sample_count].sEndTime) ||
				(CAudio::pcaAudio->fGetSquaredListenerDistance(f_x, f_y, f_z) >
								amsndAmbients[u4_sample_count].fMaxDistance))
			{
				AUMSG("Suspend ambient..\n");
				// either we are out of range or we have come to the end of the sample, mark the sample as inactive,
				// detach it from its object and stop it, but do not delete it.
				amsndAmbients[u4_sample_count].psam->Stop();
				RemoveSoundAttachment(amsndAmbients[u4_sample_count].psam);
				amsndAmbients[u4_sample_count].bActive = false;
			}
		}
	}
}


//**********************************************************************************************
uint32 CAudioDaemon::u4CountAmbientSounds
(
) const
//	
//**************************************
{
	uint32	u4_total = 0;

	for (uint32 u4_sample_count = 0; u4_sample_count<MAX_AMBIENT_SOUNDS; u4_sample_count++)
	{
		// if this element is active and is not currently an invalid deferred load pointer
		if (amsndAmbients[u4_sample_count].bActive)
		{
			// Check we actually find an attachment for this sample, if not do not count it.
			if (pinsFindSampleAttachment(amsndAmbients[u4_sample_count].psam))
			{
				u4_total++;
			}
		}
	}

	return u4_total;
}


//**********************************************************************************************
// Go through the ambient list and find a sample of the given handle, if one cannot be found
// return NULL which will force the caller to take action, usually by reloading the sample.
// If a sample is found and it is not currently playing then it will be returned and not futher
// action is required. If a sample is found that is playing it will be instanced and a new CSample
// will be returned.
//
CSample* CAudioDaemon::psamFindInAmbientList
(
	TSoundHandle		sh,
	uint32				u4_spatial_type,
	uint32&				u4_active_element
)
//	
//**************************************
{
	CSample*	psam;
	uint32		u4_instance;
	bool		b_instance = false;
	uint32		u4_create_flag;

	u4_active_element = 0xffffffff;
	

	switch (u4_spatial_type)
	{
	case AU_STEREO:
		u4_create_flag = AU_CREATE_STEREO;
		break;

	case AU_PSEUDO3D:
		u4_create_flag = AU_CREATE_PSEUDO_3D;
		break;

	case AU_SPATIAL3D:
		u4_create_flag = AU_CREATE_SPATIAL_3D;
		break;

	default:
		// unknown spatial type....
		Assert(0);
		break;
	}


	for (uint32 u4_sample_count = 0; u4_sample_count<MAX_AMBIENT_SOUNDS; u4_sample_count++)
	{
		// do this sample have the correct handle??
		if (amsndAmbients[u4_sample_count].shSample == sh)
		{
			// if this element is active...
			if (amsndAmbients[u4_sample_count].bActive)
			{
				// instance the sample..

				// keep a copy of the sample that we will instance if we do not find
				// another identical sample futher down the play list.
				if (!b_instance)
				{
					b_instance = true;
					u4_instance = u4_sample_count;
				}
			}
			else
			{
				// this is not active, use it..
				AUMSG("Ambient Cache hit (direct)...\n");
				Assert( amsndAmbients[u4_sample_count].psam );

				// does the spatial type match what we require??
				if (amsndAmbients[u4_sample_count].psam->u4CreateFlags & u4_create_flag)
				{
					//
					// the activate element is required for the add to list function as it needs to
					// know which element in the cahce to activate. It could search for it but what
					// is the point when we already know?
					//

					u4_active_element = u4_sample_count;

					return amsndAmbients[u4_sample_count].psam;
				}

				AUMSG("Ambient type mismatch - must instance\n");

				//
				// This sample is the correct sample and it is inactive but it is not the correct
				// spatial type so returning it would probably crash.
				// We know what the create flags of the new buffer should be so lets instance it
				// to a different spatial type, this will save us reloading.
				//
				if (!b_instance)
				{
					b_instance = true;
					u4_instance = u4_sample_count;
				}
			}
		}
	}

	// did we find an instance??
	if (b_instance)
	{
		// yes, we have to instance. We have been all the way through the cache and did not find
		// a sample of the correct type that was inactive. So we will make an instance of an
		// active one.
		// If we try to instance a deferred loading sample, the instance function will return
		// psamDEFER_LOAD and it is the responsibility of the caller to take special action.
		AUMSG("Ambient cache hit (Instance)...\n");

		psam = amsndAmbients[u4_instance].psam->psamCreateInstance(u4_create_flag);

		// if we have failed to create the instance, assume it does not exist
		if (psam == psamINSTANCE_FAILED)
			return NULL;

		return psam;
	}

	// not in the list..
	return NULL;
}



//**********************************************************************************************
// Voice overs once killed can be deleted because they will not be required again.
//
void CAudioDaemon::KillVoiceovers
(
)
//*************************************
{
	if (bVoiceOver)
	{
		Assert (psamVoiceOver);

		RemoveSoundAttachment(psamVoiceOver);
		delete psamVoiceOver;
		bVoiceOver = false;
	}

	if (bDelayedVoiceOver)
	{
		// delete any delayed voiceovers
		bDelayedVoiceOver = false;

		for (uint32 u4_sample_count = 0; u4_sample_count<MAX_DELAYED_VOICEOVER; u4_sample_count++)
		{
			if (asatSoundObjects[u4_sample_count].psam != NULL)
			{
				RemoveSoundAttachment(asatSoundObjects[u4_sample_count].psam);
				delete apsamVoiceOver[u4_sample_count];
				apsamVoiceOver[u4_sample_count] = NULL;
			}
		}
	}
}



//**********************************************************************************************
// Kill ambients by stopping them and clearing their active flag, the sample stays around for
// future use.
//
void CAudioDaemon::KillAmbientSamples
(
)
//*************************************
{
	for (uint32 u4_sample_count = 0; u4_sample_count<MAX_AMBIENT_SOUNDS; u4_sample_count++)
	{
		if (amsndAmbients[u4_sample_count].psam != NULL)
		{
			amsndAmbients[u4_sample_count].psam->Stop();
			amsndAmbients[u4_sample_count].bActive = false;
			RemoveSoundAttachment(amsndAmbients[u4_sample_count].psam);
		}
	}
}



//**********************************************************************************************
// Music once killed can be deleted as it is never cached
//
void CAudioDaemon::KillMusic
(
)
//*************************************
{
	if (bMusic)
	{
		Assert(psamMusic);

		// remove from the fade list just in case it is fading
		CAudio::pcaAudio->StopFade(psamMusic);
		RemoveSoundAttachment(psamMusic);
		delete psamMusic;
		bMusic = false;
	}
}



//**********************************************************************************************
// To kill the collisions we just go through the list and stop them all, in this way they all
// remain in the cahce for future use.
//
void CAudioDaemon::KillCollisions
(
	bool	b_delete
)
//*************************************
{
	for (uint32 u4_sample_count=0; u4_sample_count<COLLISION_CACHE_LENGTH; u4_sample_count++)
	{
		if (acceCollisions[u4_sample_count].psam != NULL)
		{
			acceCollisions[u4_sample_count].u4Flags = 0;
			acceCollisions[u4_sample_count].psam->Stop();

			// If we are to delete the item, kill its cahce entry and delete the sample.
			if (b_delete)
			{
				acceCollisions[u4_sample_count].shSample		= 0;
				acceCollisions[u4_sample_count].shStopSample	= 0;
				acceCollisions[u4_sample_count].sTime			= 0.0f;
				acceCollisions[u4_sample_count].u4ID			= 0;

				RemoveSoundAttachment(acceCollisions[u4_sample_count].psam);
				delete acceCollisions[u4_sample_count].psam;
				acceCollisions[u4_sample_count].psam			= NULL;
			}
		}
	}
}



//**********************************************************************************************
// Removes any existing subtitle
//
void CAudioDaemon::RemoveSubtitle
(
)
//*************************************
{
	CTextOverlay::ptovTextSystem->RemoveText(ettSUBTITLE);
}



//**********************************************************************************************
// When a voiceover is played, if subtitles are enabled, this function is called with a pointer
// to the sample. From this the subtitle data can be extracted and formatted.
//
void CAudioDaemon::ProcessSubtitle
(
	CSample*	psam
)
//*************************************
{
	Assert(psam);

	// this sample has no subtitle...
	if (psam->pasubSubtitle == NULL)
		return;

	if (psam->pasubSubtitle->bSubtitleSetup())
		return;

	// Mark the sub title as been setup...
	psam->pasubSubtitle->SubtitleSetup();

	SSubtitleHeader*	psth = psam->pasubSubtitle->psthGetSubtitleData();

	// there is no subtitle
	if (psth == NULL)
	{
		// we can leave any existing subtitle on the screen.
		return;
	}

	uint32 u4_sect = psth->u4SubtitleSections;

	// skip the header to point to the first section of data.
	// sections in the source data are formatted as follows:
	//			+0			time in 1/5s of a second.
	//			+4			Raw string that may contain 0x13 charcaters for new lines
	//						String is zero terminated.
	//			+n			After terminator is another time block (NOT ALIGNED)
	//			+n+4		String of section section...
	//			...
	//			...
	//			Repeat for the number of sections in the subtitle...
	//
	//
	// The data in the destination buffer in almost exactly the same format but the strings
	// have been formatted. For infomration on the formatting see the DDFont code.
	//
	uint8* pu1_data = (uint8*)(psth+1);

	uint32	u4_prev = CTextOverlay::ptovTextSystem->u4FindSequenceEnd(ettSUBTITLE);
	TSec	s_time;

	// go through all of the sections in the data block...
	while(u4_sect)
	{
		s_time = (float)(*(uint32*)pu1_data)*0.2f;
		pu1_data+=sizeof(uint32);

		u4_prev = CTextOverlay::ptovTextSystem->u4DisplayFormattedString((char*)pu1_data, s_time, TEXT_FORMAT_BOTTOM|TEXT_FORMAT_CENTER, CColour(255,255,255), u4_prev, ettSUBTITLE);

		// skip over the string and the NULL so we point to the next buffer.
		pu1_data+=strlen((char*)pu1_data)+1;

		u4_sect--;
	}
}



//**********************************************************************************************
bool CAudioDaemon::bPlaySoundEffect
(
	const CMessageAudio&	msg,
	SSoundEffectResult*		pser
)
{
	// this function can only play effects
	Assert((msg.u4GetAudioType() == eamATTACHED_EFFECT) || 
		(msg.u4GetAudioType() == eamPOSITIONED_EFFECT) || (msg.u4GetAudioType()==eamDINO_VOCAL_EFFECT) );

	CSample*		psam;

	// if effects are not enabled then do nothing
	if (u4GetFeatures(AUDIO_FEATURE_EFFECT) == 0)
		return true;

	// make an Id that identifies this instance of this sample
	uint32 u4_id = (uint32)msg.sndhndSample + (uint32)msg.pinsParent;

	// If this is a looped effect then chekc the collision loop flags
	if (msg.bLooped) 
	{
		if (u4UpdateCollisionLoopStatus(u4_id)!=0xffffffff)
		{
			// If we update an already looping sample then return a NULL handle
			// and a zero length;
			if (pser)
			{
				pser->u4SoundID = 0;
			}
			return true;
		}
	}
	else
	{
		// we can only have a stop sample on a looped effect
		Assert(msg.sndhndLoopStop == 0);
	}

	// look for a sample with the given properties in the collision/effect cache.
	// this will create an instance of the sample if it is in the cache but is in use, it may
	// also change the state of an collision sample to be an effect sample...
	// If the sample is found in the cahce and it is looped then the looped flag is updated
	psam = psamFindInCollisionCache(msg.sndhndSample, COLLISION_CACHE_EFFECT | (msg.bLooped?COLLISION_CACHE_LOOPED:0), u4_id,msg.sndhndLoopStop);

	/// the cahce entry we may have found could have the wrong buffer type..
#if VER_DEBUG
	if (((uint32)psam>3) && CAudio::pcaAudio->bAudioActive())
	{
		switch (msg.u4SpatialType)
		{
		case 0:				// stereo
			Assert(psam->pDS3DBuffer == NULL);
			break;

		case 1:				// Pseudo
			Assert(psam->pDS3DBuffer == NULL);
			break;

		case 2:				// spatial
			Assert(psam->pDS3DBuffer != NULL)
			break;
		}
	}
#endif

	// if we did not find sample 1 in the cache, load it and add it to the cache
	if (psam == NULL)
	{
		uint32 u4_flags = AU_CREATE_STATIC|(msg.bDeferLoad?AU_CREATE_DEFER_LOAD_ON_PLAY:0);

		switch (msg.u4SpatialType)
		{
		case 0:				// stereo
			u4_flags |= AU_CREATE_CTRL_VOLUME;
			break;

		case 1:				// Pseudo
			u4_flags |= (AU_CREATE_PSEUDO_3D | AU_CREATE_CTRL_VOLUME);
			break;

		case 2:				// spatial
			u4_flags |= (AU_CREATE_SPATIAL_3D | AU_CREATE_CTRL_VOLUME);
			break;

		default:
			Assert(0);
			break;
		}

		AUMSG("Loading Effect sample...\n");
		psam  = psamCreateSampleWithRetry(msg.sndhndSample,padatEffects, u4_flags);

		// if loaded a valid sample add it to the cache for future use and flag it if it is looped
		if (psam!=NULL && !bSampleError(psam))
			AddToCollisionCache(psam, msg.sndhndSample, COLLISION_CACHE_EFFECT|(msg.bLooped?COLLISION_CACHE_LOOPED:0),u4_id,msg.sndhndLoopStop);
	}

	// the sample pointer we have is valid
	if (psam != NULL && !bSampleError(psam))
	{
		SetupSample
		(
			psam,
			msg, 
			((msg.u4GetAudioType() == eamATTACHED_EFFECT) || (msg.u4GetAudioType() == eamDINO_VOCAL_EFFECT))	// Should we attach the sample to the object??
		);

		psam->bPlay(msg.bLooped?AU_PLAY_LOOPED:AU_PLAY_ONCE);

		if (pser)
		{
			pser->u4SoundID = (uint32)psam;
		}

		// if we are playing a dino effect then we need to mute any ambient sounds
		if (msg.u4GetAudioType()==eamDINO_VOCAL_EFFECT)
			MuteAmbientSounds( psam->fSamplePlayTime() );

		return true;
	}


	//
	// if we get to here we have failed to play the requested sample
	//
	if (pser)
	{
		pser->u4SoundID = 0;
	}

	return false;
}


//**********************************************************************************************
// This will mute any mutable ambient sounds while the dinos howel
void CAudioDaemon::MuteAmbientSounds
(
	float	f_play_time
)
//*************************************
{
	// if ambients are not enabled then do nothing
	if ((u4FeaturesEnabled & AUDIO_FEATURE_AMBIENT) == 0)
		return;

	if (bAmbientMuted)
	{
		// AmbientSounds are already muted so lets just check the ending time

		if (CMessageStep::sStaticTotal + (f_play_time*1.25) > sAmbientRestore)
			sAmbientRestore = CMessageStep::sStaticTotal + (f_play_time*1.25);
	}
	else
	{
		// Ambients are not muted so we need to mute them and set the restore time
		bAmbientMuted = true;

		// The time when ambients should be restored...
		sAmbientRestore = CMessageStep::sStaticTotal + (f_play_time*1.25);

		for (uint32 u4_sample_count = 0; u4_sample_count<MAX_AMBIENT_SOUNDS; u4_sample_count++)
		{
			if (amsndAmbients[u4_sample_count].psam != NULL)
			{
				// This sample can be muted..
				if ( (amsndAmbients[u4_sample_count].u4Flags & (AMBIENT_NOT_MUTEABLE|AMBIENT_NEVER_MUTE|AMBIENT_MUTED) == 0) )
				{
					// Take a copy of the master volume so we can restore is later
					amsndAmbients[u4_sample_count].fRestoreVolume = amsndAmbients[u4_sample_count].psam->fGetMasterVolume();

					// set the master volume to nothing so we cannot hear anything
					amsndAmbients[u4_sample_count].psam->SetMasterVolume(-100.0);

					// This sample has been muted
					amsndAmbients[u4_sample_count].u4Flags |= AMBIENT_MUTED;
				}
			}
		}
	}
}


//**********************************************************************************************
// This will restore any muted ambient samples to their original volume...
void CAudioDaemon::RestoreAmbientSounds
(
)
//*************************************
{
	for (uint32 u4_sample_count = 0; u4_sample_count<MAX_AMBIENT_SOUNDS; u4_sample_count++)
	{
		if (amsndAmbients[u4_sample_count].psam != NULL)
		{
			if (amsndAmbients[u4_sample_count].u4Flags & AMBIENT_MUTED)
			{
				// Sample cannot be muted
				Assert( (amsndAmbients[u4_sample_count].u4Flags & (AMBIENT_NOT_MUTEABLE|AMBIENT_NEVER_MUTE)) == 0 );
				// This sample is no longer muted
				amsndAmbients[u4_sample_count].u4Flags &= ~AMBIENT_MUTED;

				// restore the old master volume so we can hear it.
				amsndAmbients[u4_sample_count].psam->SetMasterVolume(amsndAmbients[u4_sample_count].fRestoreVolume);
			}
		}
	}

	bAmbientMuted = false;
}


//**********************************************************************************************
// This will stop the specified sound effect.
// Before we call psam->Stop() we must check that the sample is still within the cache. If the
// sample is not present within the cahce then do not stop it as something else has removed it
// from the cahce and it is likely that the psam has been deleted.
void CAudioDaemon::StopSoundEffect
(
	CSample*	psam
)
//*************************************
{
	Assert(psam);

	for (uint32 u4_sample_count=0; u4_sample_count<COLLISION_CACHE_LENGTH; u4_sample_count++)
	{
		if (acceCollisions[u4_sample_count].psam == psam)
		{
			acceCollisions[u4_sample_count].psam->Stop();
			acceCollisions[u4_sample_count].u4Flags = 0;

			// We may as well return because there cannot be two identical CSample addresses
			// in the cahce
			return;
		}
	}
}



//**********************************************************************************************
void CAudioDaemon::SetupSample
(
	CSample*				psam,
	const CMessageAudio&	msg,
	bool					b_attach
)
//*************************************
{
	Assert ((msg.fVolume<=0.0f) && (msg.fVolume>=-100.0f));
	Assert (msg.u4SpatialType<=2);
	Assert (msg.fAtten>=0.0f);
	Assert ((msg.fOutsideVolume<=0.0f) && (msg.fOutsideVolume>=-100.0f));
	Assert ((msg.fFrustumAngle>=0.0f) && (msg.fFrustumAngle<=360.0f));

	switch (msg.u4SpatialType)
	{
		// stereo
	case 0:
		psam->SetVolume(msg.fVolume);
		psam->SetMasterVolume(msg.fMasterVolume);
		break;

		// pseudo
	case 1:
		psam->SetAttenuation(msg.fAtten);

		if (b_attach)
		{
			Assert(msg.pinsParent);
			AttachSoundToObject(msg.pinsParent,psam);
		}
		else
		{
			// If the message contains an absolute position then we just set it, if this is
			// not the case we must have an instance to attach the sound to.
			if (msg.bPosition())
			{
				psam->SetPosition(msg.v3Pos.tX,msg.v3Pos.tZ,msg.v3Pos.tY, true);
			}
			else
			{
				Assert(msg.pinsParent);
				CPresence3<>	p3_pres		= msg.pinsParent->pr3Presence();
				CVector3<>		v3_in		= d3YAxis * p3_pres.r3Rot;	// forward vector

				psam->SetPosition(p3_pres.v3Pos.tX,p3_pres.v3Pos.tZ,p3_pres.v3Pos.tY, true);
				psam->SetOrientation(v3_in.tX,v3_in.tZ,v3_in.tY, true);
			}
		}

		psam->SetMasterVolume(msg.fMasterVolume);
		break;


		// real 3D
	case 2:
		psam->SetVolume(msg.fVolume);
		psam->SetAttenuation(msg.fAtten);
		psam->SetMasterVolume(msg.fMasterVolume);

		// only set the frustum if it is going to have an effect
		if ((msg.fFrustumAngle<360.0f) && (msg.fOutsideVolume<0.0f))
		{
			psam->SetFrustum(msg.fFrustumAngle, msg.fOutsideVolume);
		}

		if (b_attach)
		{
			Assert(msg.pinsParent);
			AttachSoundToObject(msg.pinsParent,psam);
		}
		else
		{
			// If the message contains an absolute position then we just set it, if this is
			// not the case we must have an instance to attach the sound to.
			if (msg.bPosition())
			{
				psam->SetPosition(msg.v3Pos.tX,msg.v3Pos.tZ,msg.v3Pos.tY, true);
				gaiSystem.Handle3DSound(msg.v3Pos,-msg.fAtten,msg.fMasterVolume+msg.fVolume);
			}
			else
			{
				Assert(msg.pinsParent);
				CPresence3<>	p3_pres		= msg.pinsParent->pr3Presence();
				CVector3<>		v3_in		= d3YAxis * p3_pres.r3Rot;	// forward vector

				psam->SetPosition(p3_pres.v3Pos.tX,p3_pres.v3Pos.tZ,p3_pres.v3Pos.tY, true);
				psam->SetOrientation(v3_in.tX,v3_in.tZ,v3_in.tY, true);
				gaiSystem.Handle3DSound(p3_pres.v3Pos,-msg.fAtten,msg.fMasterVolume+msg.fVolume);
			}
		}
		break;

		// unknown spatial type
	default:
		Assert(0);
		break;
	}
}


//**********************************************************************************************
// This will create a sample and if it fails will check if hardware is being used. If hardware
// is being used it will release some resources and try again. This will be repeated 4 time and
// then a software buffer of the same type is created which should not fail.
// The checking is only done for spatial 3D buffers normal sound buffers should never fail on
// the first attempt
//
CSample* CAudioDaemon::psamCreateSampleWithRetry
(
	TSoundHandle	sndhnd,
	CAudioDatabase*	padat,
	uint32			u4_flags
)
//*************************************
{
	//dprintf("Create Sample..\n");
	CSample*	psam  = CAudio::psamCreateSample( sndhnd, padat, u4_flags);
	uint32		u4_count;
	bool		b_buffer;

	//
	// If we have a valid sample pointer it may still not be a valid sample because the DS pointer
	// may be NULL
	if (psam)
	{
		if (CAudio::pcaAudio->bUsingHardware() && (u4_flags & AU_CREATE_SPATIAL_3D))
		{
			//
			// If we are using hardware then the buffer may have failed to be created so we need to delete
			// something from the cahce and try again.
			//
			// Have 5 attempts at creating the buffer buffer
			//
			b_buffer = (psam->pDSBuffer != NULL);
			u4_count = 0;

			while ((b_buffer==false) && (u4_count<=4))
			{
				if (psam->pDSBuffer == NULL)
				{
					//
					// We have failed to allocate a 3D buffer, lets remove 1 from the cache to free up resources
					//
					delete psam;
					psam = NULL;
					RemoveCollisionCacheEntry();
					b_buffer = false;
				}

				//dprintf("HW Load fail: Evicting and retrying\n");
				AUMSG("HW Load fail: Evicting and retrying\n");

				// On the last time around create the buffer as a software 3D buffer.
				psam  = CAudio::psamCreateSample( sndhnd, padat, u4_flags | ((u4_count<4)?0:AU_CREATE_FORCE_SOFTWARE));

				// A null psam is a bad thing, just break out of the loop
				if (psam == NULL)
					return NULL;

				//dprintf("CreateSample returned..\n");
				if (psam->pDSBuffer)
					break;
				
				u4_count++;
			}
		}
	}

	return psam;
}


//**********************************************************************************************
// This is called when a sim stop message is received so all playing audio can be paused....
void CAudioDaemon::PauseAllPlayingAudio
(
)
//*************************************
{
	uint32 u4;

	//
	// Stop collisions, this is only important for looping and long samples but we may as well
	// stop them all. This does not remove any samples, it simply stops them and clears their
	// looping status
	//
	for (u4=0; u4<COLLISION_CACHE_LENGTH; u4++)
	{
		if (acceCollisions[u4].psam)
		{
			acceCollisions[u4].shStopSample = 0;
			acceCollisions[u4].u4ID = 0;
			acceCollisions[u4].psam->Stop();
			acceCollisions[u4].u4Flags &= ~(COLLISION_CACHE_LOOP_UPDATE|COLLISION_CACHE_LOOPED);
		}
	}

	//
	// To stop the voice over simply stop the sample and the loader thread will stop loading
	// data
	//
	if (bVoiceOver)
	{
		Assert (psamVoiceOver);
		psamVoiceOver->Stop();
	}

	//
	// Stop music
	//
	if (bMusic)
	{
		Assert(psamMusic);
		psamMusic->Stop();
	}

	//
	// Stop ambients
	//
	for (u4 = 0; u4<MAX_AMBIENT_SOUNDS; u4++)
	{
		if (amsndAmbients[u4].psam)
		{
			amsndAmbients[u4].psam->Stop();
		}
	}
}


//**********************************************************************************************
// This is called when a sim start message is received, all samples that are paused should be
// restarted.
void CAudioDaemon::RestartPausedAudio
(
)
//*************************************
{
	uint32 u4;

	// Collisions are not restarted, they should simply sort themselves out when the sim is
	// going. This also goes for looped samples as they send a collision message every frame.

	//
	// If bVoiceOver is set then a voice over is part way through and needs to be restarted...
	//
	if (bVoiceOver)
	{
		Assert (psamVoiceOver);
		psamVoiceOver->bPlay(AU_PLAY_RESUME);
	}

	//
	// If bMusic is set then a music stream is part way through and needs to be restarted...
	//
	if (bMusic)
	{
		Assert(psamMusic);
		psamMusic->bPlay(AU_PLAY_RESUME);
	}


	//
	// Restart ambients
	//
	for (u4 = 0; u4<MAX_AMBIENT_SOUNDS; u4++)
	{
		if ((amsndAmbients[u4].psam) && (amsndAmbients[u4].bActive))
		{
			// if the sample is valid and was active then play the from where they left off..
			// Remeber some ambients are looped and should be restored the same way...
			amsndAmbients[u4].psam->bPlay(AU_PLAY_RESUME | 
				(amsndAmbients[u4].u4Flags & AMBIENT_LOOP?AU_PLAY_LOOPED:0) );
		}
	}
}



//**********************************************************************************************
// Open the three permenant packed audio files either from the local hard disk or from M:
// on the network.
void CAudioDaemon::OpenAudioDatabases
(
)
//*************************************
{
	padatEffects = OpenDatabase("Effects.tpa", 16);			// Simultaneous effects are requred, so not share handles
	padatStreams = OpenDatabase("Stream.tpa", 8);			// Simultaneous streams are required, do not share file handle
	padatAmbient = OpenDatabase("Ambient.tpa", 10);			// Simultaneous ambient are required, do not share file handle
}


//**********************************************************************************************
// Close the three packed audio files
void CAudioDaemon::CloseAudioDatabases
(
)
//*************************************
{
	delete padatEffects;
	padatEffects =  NULL;

	delete padatStreams;
	padatStreams =  NULL;

	delete padatAmbient;
	padatAmbient =  NULL;
}



//**********************************************************************************************
// Create a database from the specified file, it will first look locally and the it will
// look on M:
CAudioDatabase* CAudioDaemon::OpenDatabase
(
	const char*	str_name,
	uint32		u4_handles
)
//*************************************
{
	char   str_path[MAX_PATH];
    const char * str_file = str_name;
	//
	// Look for the specified file...
	//
	if (!bFileExists(str_file))
	{
		// if we do not find it, path a netwrok filename and look for that
        strcpy(str_path, str_DataPath);
		//strcpy(str_path,"A:\\");
		strcat(str_path,str_name);
		str_file = str_path;

		// if we still do not find it it must not exist
		if (!bFileExists(str_file))
		{
			// return no database
			return NULL;
		}
	}

	//
	// str_file points to the filename of the database we should load
	//
	return new CAudioDatabase(str_file, u4_handles);
}



//**********************************************************************************************
// HACK HACK HACK
// This can be called directly if you requires the return result.. This is against the message
// protocol but with a message it is a pain in the ass to pass a return result.
bool bImmediateSoundEffect
(
	const CMessageAudio&	msg,
	SSoundEffectResult*		pser
)
//*************************************
{
	return padAudioDaemon->bPlaySoundEffect(msg,pser);
}


//**********************************************************************************************
void StopSoundEffect
(
	uint32	u4_id
)
//*************************************
{
	padAudioDaemon->StopSoundEffect((CSample*)u4_id);
}


//**********************************************************************************************
bool bQuerySoundEffect
(
	TSoundHandle			sndhnd,
	SSoundEffectResult*		pser
)
//*************************************
{
	SSampleFile*	psf;
	
	if (padAudioDaemon->padatEffects == NULL)
		return false;

	psf = padAudioDaemon->padatEffects->psfFindSample(sndhnd);

	if (psf)
	{
		// calculate the play time from the decompressed sample length and the sample format
		pser->fPlayTime		= 	(float)psf->cauheaderIndex.u4DecompressedSize 
				/ (float)(psf->cauheaderIndex.u4Frequency*(psf->cauheaderIndex.u1Bits/8)*psf->cauheaderIndex.u1Channels);
		pser->fMasterVolume	= psf->fMasterVolume;
		pser->fAttenuation	= psf->fAttenuation;
		return true;
	}

	// Sample not found so return zero
	return false;
}


//**********************************************************************************************
//
void InitAudioDaemon
(
	CWorld* pw
)
//*************************************
{
	// Make sure the audio system is going
	AlwaysAssert(CAudio::pcaAudio != NULL);

	// Create audio entity.
	padAudioDaemon = new CAudioDaemon();

	// Since this function should be called from within CWorld::Init(), the database should already be locked.
	Assert(pw->bIsLocked());
	pw->AddWithNoLock(padAudioDaemon);
}
