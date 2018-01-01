/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Audio specific messages.
 *			CMessageAudio
 *			CMessageAudioResponse
 *
 *		New message types will be added to this header file if and when they are required.
 *
 * Bugs:
 *
 * Notes:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/MessageTypes/MsgAudio.hpp                             $
 * 
 * 28    9/30/98 5:52p Rwyatt
 * Added a new bool to class to control defer loading
 * 
 * 27    9/02/98 2:22p Rwyatt
 * Made dispatch() queue by default for this message
 * 
 * 26    8/25/98 4:41p Rwyatt
 * Removed an incorrect assert
 * 
 * 25    8/21/98 2:22p Mlange
 * Additional messaging stat printing.
 * 
 * 24    8/19/98 7:11p Shernd
 * Added a ReplayLastVoiceOver message
 * 
 * 23    8/13/98 7:15p Mlange
 * Audio messages now require registration.
 * 
 * 22    8/11/98 8:27p Mlange
 * Resolved constructor call ambigouity.
 * 
 * 21    7/10/98 4:37p Rwyatt
 * New function to query the specific properties of a sample
 * 
 * 20    6/30/98 1:03a Rwyatt
 * Added new functionality for after looping samples.
 * Tided up the construction of audio messages
 * Audio messages can now have a position and not an instance
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGAUDIO_HPP
#define HEADER_LIB_ENTITYDBASE_MESSAGETYPES_MSGAUDIO_HPP


#include "Lib/Groff/EasyString.hpp"
#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/MessageTypes/RegisteredMsg.hpp"
#include "Lib/EntityDBase/Entity.hpp"
#include "lib/audio/SoundDefs.hpp"


//*********************************************************************************************
// Audio message flags for controlling how to play a sound
//
enum EAudioMessage
// prefix eam
{
	// a void action..
	eamVOID = -1,

	// play sound IDs
	eamVOICEOVER = 0,
	eamAMBIENT,
	eamPERMAMBIENT,				// An ambient that cannot be muted
	eamMUSIC,
	eamATTACHED_EFFECT,
	eamPOSITIONED_EFFECT,
	eamDINO_VOCAL_EFFECT,		// this type is always attached
    eamREPLAY_VOICEOVER,
	
	// control sound IDs
	eamCTRL_FADEMUSIC		= 100,
};

 //*********************************************************************************************
#define AUMSG_POSITIONAL	0x80000000


//*********************************************************************************************
//
class CMessageAudio : public CMessage, public CRegisteredMessage<CMessageAudio>
//
// Prefix: ma
//
// Message for audio events.
//
//**************************************
{
	friend class CRegisteredMessage<CMessageAudio>;
	static SRecipientsInfo riInfo;

public:	

	//*****************************************************************************************
	// Construct an empty message
	CMessageAudio()
		: CMessage(&riInfo.rcRecipients)
	{
	}

	//*****************************************************************************************
	// the message points to the filename string, it does not make a copy so please ensure 
	// that the filename passed in is in suitable memory. That is not going to go out of scope
	//
	CMessageAudio
	(
		TSoundHandle	sndhnd_sample,			// numeric identifier of the sample to play
		uint32			u4_flags,				// type of sound, eamVOICEOVER, eamEFFECT etc..
		CEntity*		pet_receive,			// generally to the audio daemon or 0 to broadcast
		CEntity*		pet_sender,				// who sent it.
		CInstance*		pins_parent,			// parent (owner) of the audio. Only requird for 3D
		float			f_vol,					// play volume
		float			f_atten,				// attenuation
		uint32			u4_spatial,				// spatial positioning type
		float			f_frustum,				// frustum angle
		float			f_out_vol,				// outside volume
		bool			b_looped,				// loop flag
		int32			i4_loop_count,			// number of times to loop or -1 for continuous
		float			f_maxdist,				// maximum distance before the sample is stopped
		float			f_maxvoldist = 0.0f,	// distance for which the volume is maximum an no attenuation is applied
		float			f_master_vol = 0.0f		// master volume in dBs
	) 
		: CMessage(pet_sender,pet_receive)
	{
		if (!pet_receive)
			prcRecipients = &riInfo.rcRecipients;

		sndhndSample	= sndhnd_sample;
		pinsParent		= pins_parent;
		u4Flags			= (u4_flags & 0xff);
		fVolume			= f_vol;
		u4SpatialType	= u4_spatial;
		fAtten			= f_atten;
		fFrustumAngle	= f_frustum;
		fOutsideVolume	= f_out_vol;
		fMaxDistance	= f_maxdist;
		bLooped			= b_looped;
		i4LoopCount		= i4_loop_count;
		fMaxVolDistance	= f_maxvoldist;
		fMasterVolume   = f_master_vol;
		sndhndLoopStop	= 0;
		bDeferLoad		= true;				// all samples are defer loaded by default

		Assert ((fVolume<=0.0f) && (fVolume>=-100.0f));
		Assert (u4SpatialType<=2);
		Assert (fAtten>=0.0f);
		Assert ((fOutsideVolume<=0.0f) && (fOutsideVolume>=-100.0f));
		Assert ((fFrustumAngle>=0.0f) && (fFrustumAngle<=360.0f));
		Assert (fMaxDistance>=0.0f);
		Assert ((fMasterVolume<=0.0f) && (fMasterVolume>=-100.0f));
	}


	//*****************************************************************************************
	// Create an empty audio message. This needs to have the required elements set to make it
	// into a valid action.
	//
	CMessageAudio
	(
		CEntity*		pet_receive,			// generally to the audio daemon or 0 to broadcast
		CEntity*		pet_sender			// who sent it, this will receive any reply (NULL for no reply)
	) 
		: CMessage(pet_sender,pet_receive)
	{
		if (!pet_receive)
			prcRecipients = &riInfo.rcRecipients;

		pinsParent		= NULL;
		u4Flags			= eamVOID;
		bDeferLoad		= true;
	}


	//*****************************************************************************************
	// Get the type of audio message
	uint32 u4GetAudioType() const
	{
		return u4Flags & 0xff;	// The message type is in the bottom byte of the flags
	}


	//*****************************************************************************************
	// Set the audio type of this message
	void SetMessageType(EAudioMessage eam)
	{
		u4Flags &= 0xffffff00;
		u4Flags |= (uint32)eam;
	}


	//*****************************************************************************************
	// This can be used to specify the position of a sound. This can be used with any spatial
	// type (ignored by stereo) but the sound type flags must specify eamPOSITIONED_EFFECT.
	// You cannot specify the position of an attached effect because they are inherently
	// attached to an instance.
	// If an instance is specified it will be used in preference to the position specified
	// by this function.
	void SetPositionalInfo
	(
		CVector3<>	v3_pos
	)
	//*********************************
	{
		Assert( u4GetAudioType() == eamPOSITIONED_EFFECT);

		u4Flags |= AUMSG_POSITIONAL;
		v3Pos = v3_pos;
	}


	//*****************************************************************************************
	// Return true if this message contains a specific position
	bool bPosition() const
	{
		return u4Flags & AUMSG_POSITIONAL;
	}


	//*****************************************************************************************
	// Passing zero to this function will remove any stop sample that may be present.
	void SetStopSample(TSoundHandle sndhnd)
	{
		// The message must specify a looped sample before we can add an ending sample
		Assert(bLooped);
		sndhndLoopStop = sndhnd;
	}

	//*****************************************************************************************
	// Passing zero to this function will remove any stop sample that may be present.
	void SetDeferLoad(bool b_defer)
	{
		bDeferLoad = b_defer;
	}

	//*****************************************************************************************
	virtual void Queue() const;

	//*****************************************************************************************
	//
	TSoundHandle	sndhndSample;
	TSoundHandle	sndhndLoopStop;
	uint32			u4Flags;			// Flags contain the sample type
	CInstance*		pinsParent;			// Pointer to parent for positional sounds.
	union
	{
		float		fVolume;
		float		fDeltaVolume;
	};
	float			fAtten;
	uint32			u4SpatialType;
	float			fFrustumAngle;
	float			fOutsideVolume;
	union
	{
		bool		bLooped;
		bool		bStopAfterFade;
	};
	int32			i4LoopCount;
	float			fMaxDistance;
	float			fMaxVolDistance;
	float			fMasterVolume;
	bool			bDeferLoad;

	CVector3<>		v3Pos;

protected:
	//*****************************************************************************************
	//
	void DeliverTo(CEntity* pet) const 
	{ 
		pet->Process(*this); 
	}

	//*****************************************************************************************
	//
	const char* strName() const 
	{
		return "Audio"; 
	}
};



//**********************************************************************************************
// HACK HACK HACK
// This should be a message but a message cannot return a result efficiently. This only fills
// in the sample ID element of the sound effect result all of the other information is returned
// by a call to bQuerySoundEffect. The same structure can be passed to both functions as they
// bith set different elements.
extern bool bImmediateSoundEffect
(
	const CMessageAudio&	msg,
	SSoundEffectResult*		pser
);



//**********************************************************************************************
// Call to stop a sound effect started by the above funtion. The Id can be found in the pser
// structure.
void StopSoundEffect
(
	uint32	u4_id
);



//**********************************************************************************************
// Call to get the info about a particular sample..
bool bQuerySoundEffect
(
	TSoundHandle			sndhnd,
	SSoundEffectResult*		pser
);




#endif
