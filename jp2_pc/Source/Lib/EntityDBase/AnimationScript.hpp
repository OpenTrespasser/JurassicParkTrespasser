/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		Playback of scripted animations.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/AnimationScript.hpp                                   $
 * 
 * 7     9/03/98 5:16p Mlange
 * If the camera is controlled by the animation script it will now be freed first and
 * re-attached to the original instance when done.
 * 
 * 6     8/17/98 12:55p Mlange
 * Can now disable auto frame grabber at end of animation script sequence.
 * 
 * 5     8/05/98 3:15p Mlange
 * Added support for forced playback rate. Increased version number.
 * 
 * 4     8/04/98 5:53p Mlange
 * Now saves animation state in scene file. Added quaternion interpolation.
 * 
 * 3     8/03/98 6:34p Mlange
 * Can now animate scale.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_ENTITYDBASE_ANIMATIONSCRIPT_HPP
#define HEADER_LIB_ENTITYDBASE_ANIMATIONSCRIPT_HPP

#include "Lib/Std/Array.hpp"
#include "Lib/Sys/Timer.hpp"
#include "Lib/Transform/Presence.hpp"
#include "Lib/EntityDBase/Subsystem.hpp"
#include <list>

class CInstance;
class CScriptParser;

//**********************************************************************************************
//
class CAnimationScript
//
// Playback of scripted instance animations.
//
// Prefix: ans
//
//**************************************
{
public:
	struct SAnimFrame
	// A single frame for instance animation.
	// Prefix: afr
	{
		TSec sTime;
		CPresence3<> pr3Location;
	};

	struct SInstanceAnimInfo
	// Animation data for a single instance.
	// Prefix: iai
	{
		uint32 u4HashName;					// Hash of name of animating instance, or zero if it is the camera.
		CPArray<SAnimFrame> paafrFrames;	// The animation frames.
	};

	struct SInstanceAnimPlay
	// Prefix: iap
	{
		CInstance* pinsInstance;			// Pointer to animating instance.
		float fInitialScale;				// Initial scale of instance.
		int iFrameNum;						// Last frame # used in animation.
	};

private:
	class CPriv;
	friend class CPriv;
	friend class CScriptParser;


	float fPlaybackRate;					// Rate at which to play anim (FPS), zero if determined by step time.

	CInstance* pinsCameraOn;				// The instance the camera was on before the animation script started playing.
	CPlacement3<> p3CameraRelative;			// The relative placement of the camera wrt that instance.
	bool bCameraOnHead;						// 'true' if the camera was on the head of that instance.
	bool bScriptHasMovedCamera;				// 'true' if the script has animated the camera.

	bool bDisableAutoGrab;					// 'true' if auto grab feature should be disabled at end of animation.

	bool bActive;							// 'true' if animation is currently playing.
	TSec sTotalElapsed;						// Seconds elapsed since start of animation.

	CPArray<SInstanceAnimInfo> paiaiAnimation;

	CPArray<SInstanceAnimPlay> paiapPlayback;// Data used whilst playing animation.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
public:
	CAnimationScript(const char* str_name, const char* str_base_dir);

	CAnimationScript(const char* str_filename, bool b_disable_autograb);

	~CAnimationScript();

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	bool bIsPlaying() const
	//
	// Returns:
	//		'true' if animation is currently playing.
	//
	//**************************************
	{
		return bActive;
	}


	//******************************************************************************************
	//
	void Rewind();
	//
	// Reset animation back to first frame.
	//
	//**************************************


	//******************************************************************************************
	//
	void Start();
	//
	// Start (activate) this animation.
	//
	//**************************************


	//******************************************************************************************
	//
	void Stop();
	//
	// Stop (deactivate) this animation.
	//
	//**************************************


	//******************************************************************************************
	//
	void FrameAdvance
	(
		TSec s_elapsed
	);
	//
	// Advance animation to next frame.
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual char * pcSave(char *  pc_buffer) const;
	//
	//
	//**************************************

	//*****************************************************************************************
	//
	virtual const char * pcLoad(const char *  pc_buffer);
	//
	//
	//**************************************
};


//**********************************************************************************************
//
class CAnimations : public CSubsystem
//
// Manager class for scripted animations.
//
// Prefix: anim
//
//**************************************
{
	std::list<CAnimationScript*> ltansAnims;

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
public:
	CAnimations();

	~CAnimations();

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Add
	(
		CAnimationScript* pans
	);
	//
	// Add given animation script.
	//
	//**************************************


	//******************************************************************************************
	//
	void Remove
	(
		CAnimationScript* pans
	);
	//
	// Remove given animation script.
	//
	//**************************************


private:
	//******************************************************************************************
	//
	CAnimationScript* pansFind
	(
		const CAnimationScript* pans
	) const;
	//
	// Find animation script.
	//
	// Returns:
	//		Named anim script, or null if none that match.
	//
	//**************************************

public:
	//******************************************************************************************
	//
	// Overrides.
	//

	//*****************************************************************************************
	virtual void Process(const CMessageStep&) override;

	//*****************************************************************************************
	virtual char * pcSave(char *  pc_buffer) const override;

	//*****************************************************************************************
	virtual const char * pcLoad(const char *  pc_buffer) override;
};

// Single global instance.
extern CAnimations* pAnimations;


#endif
