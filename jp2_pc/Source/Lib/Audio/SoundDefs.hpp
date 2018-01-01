/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *	Sound definitions.
 *
 * Bugs:
 *
 * To do:
 *
 * NOTES:
 *							THIS FILE CAN BE INCLUDED FROM ANYWHERE
 *
 * This is a stand alone header file that defines the audio structures that are going to be used
 * in the rest of the game. The purpose of this header file is to bridge the gap between the game
 * and the audio ysytem. This is required because the audio header file includes the sirect sound
 * files which in turn include Windows.h
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Audio/SoundDefs.hpp                                               $
 * 
 * 21    9/27/98 10:02p Rwyatt
 * Added new flag for defer load on play
 * 
 * 20    9/23/98 12:09p Rwyatt
 * Added support for NULL samples while audio is enabled
 * 
 * 19    9/17/98 3:54p Rwyatt
 * Added some user bits to the sample flags
 * 
 * 18    8/21/98 2:30a Rwyatt
 * New flag for stream initialized
 * 
 * 17    7/10/98 4:37p Rwyatt
 * New function to query the specific properties of a sample
 * 
 * 16    6/24/98 3:23p Rwyatt
 * Collision transfer now have a minimum velocity
 * 
 * 15    5/24/98 10:33p Rwyatt
 * New creation flag to force a buffer to be in software
 * 
 * 14    4/29/98 7:02p Rwyatt
 * Added some new user flags (8 bits). This saves adding another flag word to every sample.
 * 
 * 13    3/22/98 5:02p Rwyatt
 * New binary audio collisions
 * New binary instance hashing with new instance naming
 * 
 * 12    3/09/98 10:53p Rwyatt
 * New hashing function for sound handles and a new sound type.
 * 
 * 11    2/21/98 5:54p Rwyatt
 * Added a sound effect structure which contains the result info on a sound effect that has just
 * been played.
 * 
 * 10    12/17/97 2:51p Rwyatt
 * Added numeric IDs for spatial types
 * 
 * 9     11/18/97 9:43p Rwyatt
 * Added AU_CREATE_DEFER_LOAD flag
 * 
 * 8     11/13/97 11:31p Rwyatt
 * New flagsm, structures and enums for controlling the new audio system.
 * 
 * 7     7/23/97 8:39p Rwyatt
 * New flags for audio control
 * 
 * 6     7/14/97 12:26p Rwyatt
 * Modified #defines for sample types
 * 
 * 5     6/16/97 2:52a Rwyatt
 * Removed fastcall from u4HashString
 * 
 * 4     6/09/97 12:07p Rwyatt
 * New definitions for collision matrix
 * 
 * 3     6/02/97 4:49p Rwyatt
 * Added specifc flags for the different sample types, such as VoiceOver, Ambient, Collision etc
 * 
 * 2     5/29/97 4:18p Rwyatt
 * Header file that links all other subsystems to the audio system
 * 
 * 1     5/29/97 2:49p Rwyatt
 * Initial implementation
 * 
 ***********************************************************************************************/

#ifndef HEADER_LIB_AUDIO_DEFS_HPP
#define HEADER_LIB_AUDIO_DEFS_HPP

#include "Lib/std/utypes.hpp"


//**********************************************************************************************
// A SoundMaterial is a binary number hashed from the sound material textual name. This is used
// to aid faster searching of the material.
//
// prefix: smat
typedef uint32 TSoundMaterial;



//**********************************************************************************************
// prefix: sndhnd
typedef uint32	TSoundHandle;


//**********************************************************************************************
// flags
//
// Types of sample that can be created and controls that can be applied to them
// All of these flags must be unique as they are combined in various different ways..
//
// spatial types
// only 1  can apply
#define	AU_CREATE_STEREO			0x00000001
#define AU_CREATE_SPATIAL_3D		0x00000002
#define AU_CREATE_PSEUDO_3D			0x00000004
#define AU_CREATE_TYPE_MASK			0x00000007

// If this flag is set and hardware is being used the buffer will be created in software as a
// normal direct sound mixed buffer. This is true for 2D and 3D buffers.
#define AU_CREATE_FORCE_SOFTWARE	0x00000008

// types of buffer
#define AU_CREATE_STATIC			0x00000000
#define AU_CREATE_STREAM			0x00000100
#define AU_CREATE_MAPPED			0x00000200
#define AU_CREATE_DEFER_LOAD		0x00000400		// start defer load from constructor
#define AU_CREATE_NULL				0x00000800

// methods of playing
#define AU_PLAY_ONCE				0x00000000
#define AU_PLAY_LOOPED				0x00001000
#define AU_PLAY_RESUME				0x00002000

// controls on a sound buffer
#define AU_CREATE_CTRL_FREQUENCY	0x00010000
#define AU_CREATE_CTRL_PAN			0x00020000
#define AU_CREATE_CTRL_VOLUME		0x00040000

// Stream control flags
#define AU_FLAG_STOP				0x01000000
#define AU_STREAM_INIT				0x02000000
#define AU_CREATE_DEFER_LOAD_ON_PLAY			0x04000000		// start a sefer load when play is called
#define AU_CREATE_DEFER_PLAY_ON_CALLBACK		0x08000000		// default callback will play when defer load finished, clear if not to play

#define AU_USER_BIT1				0x10000000
#define AU_USER_BIT2				0x20000000
#define AU_USER_BIT3				0x40000000
#define AU_USER_BIT4				0x80000000

//**********************************************************************************************
// Numeration of spatial types, this could be an enum
//
#define	AU_STEREO					0
#define AU_PSEUDO3D					1
#define AU_SPATIAL3D				2



//**********************************************************************************************
// macros to decode the format DWORD
//
#define AU_FORMAT_FREQ		(0x3fffffff)
#define AU_FORMAT_STEREO	(1<<31)
#define AU_FORMAT_16BIT		(1<<30)



//**********************************************************************************************
//
#define	AU_COMPRESS_PCM			0
#define	AU_COMPRESS_ADPCM		1
#define	AU_COMPRESS_VOICE		2
//	
//**************************************



//**********************************************************************************************
//
#define AU_SOUND_FUSTRUM_DEFAULT_ANGLE	160.0f
#define AU_SOUND_FUSTRUM_DEFAULT_VOLUME	-15.0f
//	
//**************************************


//**********************************************************************************************
//
#define AU_BUFFER_STATUS_NOTPLAYING		0
#define AU_BUFFER_STATUS_PLAYING		1
#define AU_BUFFER_STATUS_LOST			2
//	
//**************************************


//**********************************************************************************************
//
// Setup enumerations
//
enum ESoundChannels
// prefix: esc
{
	escMONO=1,
	escSTEREO=2
};


enum ESoundBits
// prefix: esb
{
	esb8BIT=8,
	esb16BIT=16
};



//**********************************************************************************************
// Types of speaker configurations that are supported.
//
enum ESpeakerConfig
{
	esconMONO = 0,
	esconSTEREO,
	esconQUAD,
	esconSURROUND,
	esconHEADPHONES,
	esconLAST
};



//**********************************************************************************************
// describes the pitch and volume transfers at a give velocity. The elements of this structure
// simply descirbe a straight line in the form y=mx+c, the min and max members are the min and
// max y values in the previous equation.
//
struct SSoundTransfer
// prefix: st
{
	float	fVolMax;
	float	fVolMin;
	float	fVolSlope;
	float	fVolInt;

	float	fPitchMax;
	float	fPitchMin;
	float	fPitchSlope;
	float	fPitchInt;

	float	fAttenuate;
	float	fMinVelocity;		// Normalized from 0.0 .. 1.0

	// helper functions to return the volume and pitch as a scalar
	// volume is from 0.0 to 1.0
	// pitch is from -1.0 to 1.0

	// at the moment both are simple straight line equations that are clamped
	float	fCalculateVolume(float f_vel) const
	{
		float f_vol = (fVolSlope * f_vel) + fVolInt;

		if (f_vol < fVolMin)
			return fVolMin;

		if (f_vol > fVolMax)
			return fVolMax;

		return f_vol;
	}

	float	fCalculatePitch(float f_vel) const
	{
		float f_pit = (fPitchSlope * f_vel) + fPitchInt;

		if (f_pit < fPitchMin)
			return fPitchMin;

		if (f_pit > fPitchMax)
			return fPitchMax;

		return f_pit;
	}
};


//*********************************************************************************************
// prefix: ser
//
struct SSoundEffectResult
{
	uint32	u4SoundID;			// ID to identify the sound (not sample).
	float	fPlayTime;			// length of the playing sample in seconds.
	float	fMasterVolume;		// Ideal master volume for this sample
	float	fAttenuation;		// Ideal attenuation for this sample
};


//**********************************************************************************************
// a normal C function does the hashing!
//
extern uint32 u4HashString(const char* str);
extern uint32 sndhndHashIdentifier(const char* str);

#define matHashIdentifier(str)  sndhndHashIdentifier((str))

//**********************************************************************************************
// Defined in hash.cpp
extern uint32 u4Hash
(
	const void* pv,					// Pointer to the data to derive a hash value from.
	int         i_size_bytes,		// Size of data.
	bool        b_string			// conversions.
);

#endif
