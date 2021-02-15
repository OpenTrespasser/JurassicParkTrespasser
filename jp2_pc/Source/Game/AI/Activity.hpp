/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CActivity base classes for AI library.  CActivity's collectively handle all actions 
 *		for an animal.
 *
 * Bugs:
 *
 * To do:
 *		Implement a pathfinder.  This isn't really an Activity comment, but the DoMove
 *		functions mention it in their descriptions....
 *		Implement the resources required and used functions.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/Activity.hpp                                                  $
 * 
 * 44    9/19/98 1:44a Agrant
 * blind head move for cooler eating
 * 
 * 43    9/05/98 8:28p Agrant
 * allow AI to set body and tail damage
 * 
 * 42    9/02/98 5:25p Agrant
 * activities allocated from fastheap
 * 
 * 41    8/26/98 7:12p Agrant
 * set head damage function
 * 
 * 40    8/26/98 11:15a Agrant
 * temp flag rework
 * 
 * 39    8/26/98 1:03a Agrant
 * made activate virtual
 * 
 * 38    8/16/98 3:21p Agrant
 * Much tweaking to simplify activity balancing
 * 
 * 37    7/23/98 9:34p Agrant
 * reworked vocals to allow open and close times for the mouth
 * 
 * 36    7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 35    7/01/98 7:09p Agrant
 * DoVocal becomes bDoVocal, allowing vocals to fail
 * 
 * 34    6/25/98 4:51p Agrant
 * Some combat appearance tweaks
 * 
 * 33    6/18/98 4:45p Agrant
 * Replaced attack vocal function with general vocal function
 * 
 * 32    6/14/98 8:41p Agrant
 * Added the DoAttackVocal function
 * 
 * 31    6/14/98 3:29p Agrant
 * the howl repeat bug and other fixes
 * 
 * 30    6/09/98 5:50p Agrant
 * More useful version of bWasChosen
 * 
 * 29    6/05/98 6:08p Agrant
 * Revised synthesizer to handle random-access acitivities.
 * Also handles exclusive ones (the old method).
 * 
 * 28    5/19/98 9:18p Agrant
 * A way to move without bothering to pathfind
 * 
 * 27    5/17/98 9:30p Agrant
 * changes for ActivityPhased
 * 
 * 26    5/16/98 11:50p Agrant
 * many tweaks
 * 
 * 25    5/15/98 3:11p Agrant
 * more helper functions
 * 
 * 24    5/10/98 6:26p Agrant
 * AI's now can ignore selected tangible objects.
 * misc. changes
 * 
 * 23    5/06/98 8:59p Agrant
 * reworked vocal activities-
 * Ouch now has actual samples
 * 
 * 22    3/25/98 9:22p Agrant
 * Improved distance activity to allow better control of falloff function
 * 
 * 21    2/23/98 5:07p Agrant
 * AI save/load
 * 
 * 20    2/13/98 1:53p Agrant
 * Rework multipliers, continues, and was used last time data
 * 
 * 19    1/30/98 6:18p Agrant
 * iGetDescription goes to CActivity
 * 
 * 18    12/20/97 7:04p Agrant
 * Cleaning up includes,
 * add the active flag to activities
 * 
 * 17    12/18/97 7:38p Agrant
 * Added the CONITNUE flag, for activities that should try to keep going.
 * 
 * 16    12/17/97 9:27p Agrant
 * More dino actions
 * 
 * 15    7/31/97 4:39p Agrant
 * Resources fix
 * 
 * 14    7/19/97 1:29p Agrant
 * Audio support added
 * Dino resource management added
 * 
 * 13    5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 12    5/18/97 3:11p Agrant
 * Moved a bunch of virtual functions to .cpp files
 * Fixed a rating bug
 * 
 * 11    5/11/97 12:54p Agrant
 * init pinfTarget with 0
 * virtualized some functions
 * 
 * 10    5/04/97 9:30p Agrant
 * Reworked the way activities are handled-  each now operates on an
 * InfluenceList.
 * 
 * 9     4/29/97 6:42p Agrant
 * CFeeling now float-based instead of CRating-based.
 * Activities now rate/act based on a list of influences.
 * Better rating functions for activities.
 * Debugging tools for activity processing.
 * 
 * 8     97-03-31 21:53 Speter
 * Updated for new set0 constant.
 * 
 * 7     11/02/96 7:16p Agrant
 * Added Head motion and mouth commands
 * 
 * 6     10/15/96 9:20p Agrant
 * Synthesizer reworked to be better, stronger, smarter, faster than before.
 * Activity Rate and Act and Register functions now have better defined roles.
 * Added some accessor functions to smooth future changes.
 * 
 * 5     10/10/96 7:17p Agrant
 * Modified code for code specs.
 * 
 * 4     9/26/96 5:58p Agrant
 * basic AI structure done
 * AI system adapted to TReal for world coords
 * AI Test App exhibits very basic herding with two very simple actvities.
 * 
 * 3     9/23/96 2:57p Agrant
 * Separated rating and acting.
 * 
 * 2     9/19/96 1:29p Agrant
 * Setting up Direct Object AI structure for Activities
 * 
 * 1     9/03/96 8:12p Agrant
 * initial revision
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_ACTIVITY_HPP
#define HEADER_LIB_AI_ACTIVITY_HPP

#include "Lib/Std/Array.hpp"
#include "Lib/Std/Set.hpp"

#include "Lib/Transform/Vector.hpp"
#include "Classes.hpp"
#include "AITypes.hpp"
#include "Rating.hpp"
#include "Feeling.hpp"

#include "Lib/Audio/SoundDefs.hpp"

#include "Lib/Sys/Timer.hpp"

#include "string.h"


enum EActivityFlag
//	Prefix: eaf
//  Random activity flags.
{
	eafCONTINUE,			//  true if we ought to try to continue this activity.  Reset after synthesis..
	eafCONTINUENEXTTIME,	//  true if we ought to try to continue this activity next AI cycle
	eafACTIVE,				//  true if the Activity is active in the AI model, else false.
	eafWASCHOSENLASTTIME,	//  true if the Activity was chosen last time.
	eafWASCHOSENTHISTIME,
	eafPHASEONE,			//  true if we are in phase 1 of this activity
	eafPHASETWO,
	eafPHASETHREE,
	eafPHASEFOUR,
	eafEXCLUSIVE,			// true if we need to treat this activity as an exclusive, rather than random, action
	eafEND
};

class CEntity;
class CInfluence;
class CInfluenceList;
struct SVocal;

//*********************************************************************************************
//
class CActivity
//
//	Prefix: act
//
//	An action that an animal might perform.
//
//	Notes:
//		The CActivity class is little more than a function that uses a mental state to 
//		decide upon actions in a particular way.  All Activities must inherit from this base 
//		class.
//
//		Rating Feelings' parameters ought to total to 0 for an average activity.
//		Any rating feeling totalling more than 0 will be more likely to happen,
//		and less than 0 will be less likely to happen.
//
//		The idea behind the 0 centered approach is as follows:
//		If I am completely ambivalent, all parameters equal, then I am unlikely
//		to do anything.  Pretend that you have all 1's for example, and analyze 
//		the activity from that point of view.  If each of the parameters changes,
//		are you more or less likely to perform the activity?  If more, increase the 
//		parameter.  If less, decrease it.
//
//
//
//	Command Functions:
//		Command functions always begin with the string "Do" and give a command to the physics
//		system.  In the case of "Move" functions, they first invoke the Brain's 
//		pathfinder, if it has one.  
//
//*********************************************************************************************
{


//
//	Variable declarations.
//
public:
	CFeeling	feelRatingFeeling;	// This feeling rates the importance of each
									// emotional parameter to this Actvity.
									// See rtRateWithRatingFeeling();

	CRating		rtThreshold;		// The minimum rating required for this activity
									// to have any real effect.

	float		fMultiplier;		// Always multiply the rating by this number.  Gets reset to 1.0f after the rating functions are called.
	float		fNextMultiplier;	// Always multiply the rating by this number.  Gets reset to 1.0f after the rating functions are called.
									
	CSet<EActivityFlag> sFlags;		// See declaration of EActivityFlag

	const CInfluence* pinfTarget;			// Used when rating a list.
									// Saves the most significant influence in the list.

	CSet<EDinoResource> sResourcesRequired;		// Which resources do we need to complete the activity?
	CSet<EDinoResource> sResourcesUsed;			// Which resources are no longer available if we perform the activity?

	TSec		sCanAct;			// The time at which this activity becomes a valid action.
	TSec		sGiveUp;			// The time at which we should try something else.

#define iMAX_ACTIVITY_NAME_LENGTH 16
	char		acName[iMAX_ACTIVITY_NAME_LENGTH];	// The name of the activity for debugging.
//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

	CActivity
	(  const char *ac_name = "UNKNOWN"
	);
	
	virtual ~CActivity
	(
	)
	{
		// This is not our memory.  Don't delete it.
		pinfTarget = 0;
	};


	//*****************************************************************************************
	//
	//	Member functions.
	//

	public:
		//*****************************************************************************************
		//
		void* operator new
		(
			uint u_size_type	// Amount of memory to allocate in bytes.
		);
		//
		// Returns pointer to memory allocated from the next position in the heap.
		//
		//**************************************

		//*************************************************************************************
		//
		void SetMultiplier
		(
			float f	// New multiplier.
		)
		//
		//	Sets the multiplier for this activity for the next rating phase.
		//
		//******************************
		{
			fMultiplier = f;
			fNextMultiplier = f;
		}

		//*************************************************************************************
		//
		void TryToContinue
		(
			float f_multiplier = 1.3f
		)
		//
		//	Lets the group containing the activity know that this activity should be continued next time.
		//
		//******************************
		{
			sFlags[eafCONTINUE]				= true;
			sFlags[eafCONTINUENEXTTIME]		= true;
			SetMultiplier(f_multiplier);
		}

		//*************************************************************************************
		//
		void DontContinue
		(
		)
		//
		//	Lets the group containing the activity know that this activity should be continued next time.
		//
		//******************************
		{
			sFlags[eafCONTINUE]			= false;
			sFlags[eafCONTINUENEXTTIME]	= false;
			SetMultiplier(0.0f);
		}

		//*************************************************************************************
		//
		bool bWasChosen
		(
			const CInfluence* pinf = 0
		)
		//
		//	Was the activity chosen last time?.
		//	
		//	Notes:  if pinf is NULL, it is ignored.  If valid, the function returns true iff the activity ran
		//		last time AND ran on the given influence.
		//
		//******************************
		{
			if (!pinf || pinf == pinfTarget)
				return sFlags[eafWASCHOSENLASTTIME];
			else
				return false;
		}

		//*************************************************************************************
		//
		void SetWasChosen
		(
		)
		//
		//	Sets the WasChosen flag.
		//
		//******************************
		{
			sFlags[eafWASCHOSENTHISTIME] = true;
		}

		//*************************************************************************************
		//
		virtual void Activate
		(
			bool b_active_state	// True if activity is to be active.
		)
		//
		//	Sets the activity to active or inactive.
		//
		//******************************
		{
			sFlags[eafACTIVE] = b_active_state;
		}

		//*************************************************************************************
		//
		bool bIsActive
		(
		)
		//
		//	Returns true if activity is active.
		//
		//******************************
		{
			return sFlags[eafACTIVE];
		}

		//*************************************************************************************
		//
		bool bIsExclusive
		(
		)
		//
		//	Returns true if activity is active.
		//
		//******************************
		{
			return sFlags[eafEXCLUSIVE];
		}

		//*************************************************************************************
		//
		virtual void Act
		(
			CRating			rt_importance,	// The importance of the activity.
			CInfluence*		pinf			// The direct object of the action.
		)
		//
		//	Examines influence to see how the dino should act, and calls
		//	command functions to register actions with physics.
		//
		//	Side effects:
		//		Puts commands on the synthesizer command queue.
		//		Might update information about the influence.
		//
		//******************************
		{
			// Base class, no action implemented.
			Assert(false);
		}

		//*************************************************************************************
		//
		virtual CRating rtRate
		(
			const CFeeling&	feel,		// The feeling used to evaluate the action.
			CInfluence*		pinf		// The direct object of the action.
		);
		//
		//	How important is it to the animal to perform this actvity upon the subject of 
		//	the "feel" feeling?
		//
		//	Returns:
		//		A rating of the importance of this activity.
		//
		//******************************

		//*************************************************************************************
		//
		virtual void ResetTempFlags
		(
		);
		//
		//	Resets all temporary flags.
		//
		//	Notes:  
		//		Called after the synthesis.
		//		Useful to get information from an Act() call to next frame's rtRate() and Act() calls.
		//
		//******************************

		//*************************************************************************************
		//
		void ClearPhaseFlags
		(
		)
		//
		//	Resets all phase flags to false.
		//
		//******************************
		{
			sFlags[eafPHASEONE]		= false;
			sFlags[eafPHASETWO]		= false;
			sFlags[eafPHASETHREE]	= false;
			sFlags[eafPHASEFOUR]	= false;
		}


		//*************************************************************************************
		//
		void Register
		(
			CRating			rt_importance,	// The importance of the action.
			CInfluence*		pinf			// The direct object of the action.
		);
		//
		//	Registers the activity with the synthesizer.
		//
		//	Notes:
		//		When an activity is registered, it is telling the synthesizer that it wants 
		//		a chance to Act.  The synthesizer catalogs all registered activities and
		//		allows the most important one to Act.
		//
		//******************************

		//*********************************************************************************
		//
		void RegisterFleeHint
		(
			const CRating		rt_importance,	// The importance of the hint.
			const CVector2<>&	v2_direction	// The direction of the hint.
		);
		//
		//	Adds a hint about how to flee to the synthesizer.
		//
		//	Notes:
		//		Hints may or may not be ignored by whatever Activity wins the movement
		//		controls.
		//
		//******************************

		//*********************************************************************************
		//
		void RegisterHerdHint
		(
			const CRating		rt_importance,	// The importance of the hint.
			const CVector2<>&	v2_direction	// The direction of the hint.
		);
		//
		//	Adds a hint about how to herd to the synthesizer.
		//
		//	Notes:
		//		Hints may or may not be ignored by whatever Activity wins the movement
		//		controls.
		//
		//******************************

		//*************************************************************************************
		//
		virtual CRating rtRateAndRegister
		(
			const CFeeling&	feel,		// The feeling used to evaluate the action.
			CInfluence*		pinf		// The direct object of the action.
		);
		//
		//	Rates and Registers the activity with the synthesizer.
		//
		//	Notes:
		//		If the rating is below the threshold value, the activity will not 
		//		bother registering itself.
		//
		//******************************

		//*************************************************************************************
		//
		virtual CRating rtRateList
		(
			const CFeeling&	feel,		// The feeling used to evaluate the action.
			CInfluenceList*	pinfl		// All influences known.
		);
		//
		//	Rates  the activity.
		//
		//******************************
		
		//*************************************************************************************
		//
		virtual void RegisterList
		(
			const CRating rt,			// The rating at which to register.
			CInfluenceList*	pinfl		// All influences known.
		);
		//
		//	Registers the activity.
		//
		//	Notes:
		//		Requires that rtRateList has been called to get the rating for the activity.
		//		rtRateList saves the highest ranked influence.
		//
		//******************************

		//*************************************************************************************
		//
		virtual CRating rtRateAndRegisterList
		(
			const CFeeling&	feel,		// The emotional state used to evaluate the action.
			CInfluenceList*	pinfl		// All influences known.
		);
		//
		//	Rates and Registers the activity with the synthesizer, but only with the 
		//		highest ranked influence.
		//
		//	Notes:
		//		If the rating is below the threshold value, the activity will not 
		//		bother registering itself.
		//
		//******************************

		//*************************************************************************************
		//
		virtual CRating rtRateAndRegisterAll
		(
			const CFeeling&	feel,		// The feeling used to evaluate the action.
			CInfluenceList*	pinfl		// All influences known.
		);
		//
		//	Rates and Registers the activity for each influence in the list.
		//
		//	Notes:
		//		If the rating is below the threshold value, the activity will not 
		//		bother registering itself.
		//
		//******************************

		//*************************************************************************************
		//
		inline bool bResourcesAreAdequate
		(
			const CSet<EDinoResource>&	set_resources	// a set of available resources.
		) const
		//
		//	Can this activity be run with the available resources?
		//
		//	Returns:
		//		true if the activity can be run.
		//
		//	Notes:
		//		A resource is available if true.
		//
		//******************************
		{
			return (set_resources & sResourcesRequired) == sResourcesRequired;
		}
		
		//*************************************************************************************
		//
		inline void UseResources
		(
			CSet<EDinoResource>*	pset_resources	// a set of available resources.
		) const
		//
		//	Mark resources used by this activity as used.  
		//
		//	Notes:
		//		A resource is available if true.
		//
		//******************************
		{
			*pset_resources -= sResourcesUsed;
		}

#if VER_TEST		
		//*****************************************************************************************
		//
		virtual int iGetDescription(char *buffer, int i_buffer_length);
		//
		//  A human-readable string describing the instance.
		//
		//	Returns:
		//		The number of bytes used of buffer.
		//
		//**************************
#endif

	protected:
		//*************************************************************************************
		//
		CRating rtRateWithRatingFeeling
		(
			const CFeeling&		feel  // The feeling used to rate the activity.
		) const
		//
		//	How important is it to the animal to perform this actvity upon the subject of 
		//	the "pfeel" feeling?  Uses the RatingFeeling.
		//
		//	Returns:
		//		a rating of the importance of this activity
		//
		{
			return feelRatingFeeling.rtDot(feel);
		}
		//******************************


	//
	//	Command functions
	//

		//*********************************************************************************
		//
		void DoSetHeadDamage
		(
			const CRating		rt_importance,	// The importance of the move command.
			const CRating		rt_damage		// 0 no damage, 1 max damage
		);
		//
		//	Sends a damage command to the head
		//
		//******************************

		//*********************************************************************************
		//
		void DoSetBodyDamage
		(
			const CRating		rt_importance,	// The importance of the move command.
			const CRating		rt_damage		// 0 no damage, 1 max damage
		);
		//
		//	Sends a damage command to the body
		//
		//******************************

		//*********************************************************************************
		//
		void DoSetTailDamage
		(
			const CRating		rt_importance,	// The importance of the move command.
			const CRating		rt_damage		// 0 no damage, 1 max damage
		);
		//
		//	Sends a damage command to the tail
		//
		//******************************

		//*********************************************************************************
		//
		void DoMoveToLocation
		(
			const CRating		rt_importance,	// The importance of the move command.
			const CVector2<>&	v2_location,	// Where to go.
			const CRating		rt_speed		// How quicly to move there.
		);
		//
		//	Sends a movement command to physics.
		//
		//	Notes:
		//		A move command will first be processed by the pathfinder before going on
		//		to the physics system.  
		//
		//******************************

		//*********************************************************************************
		//
		void DoMoveToLocationBlind
		(
			const CRating		rt_importance,	// The importance of the move command.
			const CVector2<>&	v2_location,	// Where to go.
			const CRating		rt_speed		// How quicly to move there.
		);
		//
		//	Sends a movement command to physics.
		//
		//	Notes:
		//		No pathfinding.  
		//
		//******************************

		//*********************************************************************************
		//
		void DoMoveInDirection
		(
			const CRating		rt_importance,	// The importance of the move command.
			const CVector2<>&	v2_direction,	// Which direction to move.
			const CRating		rt_speed		// How quicly to move there.
		);
		//
		//	Sends a movement command to physics.
		//
		//	Notes:
		//		A move command will first be processed by the pathfinder before going on
		//		to the physics system.  
		//
		//		Directional commands are interpreted somewhat more loosely by the pathfinder.
		//		It assumes that if the animal has a specific goal, it will specify a target
		//		location.  The animal will, however, do its best to use the direction
		//		given, as long as obstructions allow.
		//
		//******************************

		//*********************************************************************************
		//
		void DoMoveTo
		(
			CRating			rt_importance,	// How important is this command?
			CInfluence*		pinf_target,	// Which influence?
			CRating			rt_speed		// How quickly should I move?
		);
		//
		//	Send a movement command to physics to move to the target influence.
		//
		//	Notes:
		//		A move command will first be processed by the pathfinder before going on
		//		to the physics system.  
		//
		//******************************

		//*********************************************************************************
		//
		void DoAction
		(
			CRating				rt_importance,		// How important is this command?
			EPhysicsPrimitive	epp,				// Which action is to be performed?
			CEntity*			pet_DO				// Which object is the action to be 
													//  performed upon?
		);
		//
		//
		//******************************

		//*********************************************************************************
		//
		void DoOpenMouth
		(
			const CRating	rt_importance,		// How important is this command?
			const CRating	rt_open_amount,		// What proportion of fully open should
													// the mouth be?
			const CRating	rt_speed			// How quickly should the jaw change positions?
		);
		//
		//	Opens and closes the mouth.
		//
		//******************************

		//*********************************************************************************
		//
		void DoCockHead
		(
			const CRating	rt_importance,		// How important is this command?
			float			f_cock_angle,		// angle in radians of head tilt.
			const CRating	rt_speed			// How quickly should the jaw change positions?
		);
		//
		//	Cocks the head.
		//
		//******************************

		//*********************************************************************************
		//
		void DoPointHead
		(
			const CRating		rt_importance,		// How important is this command?
			const CVector3<>&	v3_head_direction,	// Which direction should the head point?
			const CRating		rt_speed			// How quickly should the head change positions?
		);
		//
		//	Moves the head.
		//
		//******************************

		//*********************************************************************************
		//
		void DoMoveHeadTo
		(
			const CRating		rt_importance,		// How important is this command?
			const CVector3<>&	v3_head_destination,// Where do you want the head?
			const CRating		rt_speed			// How quickly should the head change positions?
		);
		//
		//	Moves the head.
		//
		//******************************

		//*********************************************************************************
		//
		void DoMoveHeadToBlind
		(
			const CRating		rt_importance,		// How important is this command?
			const CVector3<>&	v3_head_destination,// Where do you want the head?
			const CRating		rt_speed			// How quickly should the head change positions?
		);
		//
		//	Moves the head.
		//
		//******************************

		//*********************************************************************************
		//
		void DoStartVocalizing
		(
			const SVocal&	sv
		);
		//
		//	Use your vocal chords to play the sample.
		//
		//******************************

		//*********************************************************************************
		//
		void DoStopVocalizing
		(
		);
		//
		//	Stop the vocalization your are currently making.
		//
		//******************************

		//*********************************************************************************
		//
		bool bDoVocal(EVocalType evt);
		//
		//	Play a vocal from the specified set.
		//
		//	Returns:  true if successful, otherwise false.
		//
		//******************************

		//*********************************************************************************
		//
		void DoWagTail
		(
			const CRating		rt_importance,		// How important is this command?
			float				f_wag_amplitude,	// Radians.
			float				f_wag_frequency		// Wags/second
		);
		//
		//	Moves the head.
		//
		//******************************

		//*********************************************************************************
		//
		void DoCrouch
		(
			const CRating		rt_importance,		// How important is this command?
			const CRating		rt_speed = 1.0f		// How quickly should we drop to the ground?
		);
		//
		//	Lowers the body in a crouch.
		//
		//******************************

		//*********************************************************************************
		//
		void DoJump
		(
			const CRating		rt_importance,		// How important is this command?
			const CRating		rt_speed = 1.0f		// How quickly should we drop to the ground?
		);
		//
		//	Jump now!
		//
		//******************************

		//*********************************************************************************
		//
		void DebugSay
		(
			const char *ac		// what to say
		);
		//
		//	Outputs debug info from a specific animal.
		//
		//******************************


	//*****************************************************************************************
	//
	//	Static Functions.
	//
		
	public:
		//*************************************************************************************
		//
		static CActivity *pactLoad
		(
			const char	*ac_filename
		);
		//
		//	Loads the activity in the specified file.  Returns a new actvity.
		//
		//	Notes:
		//		This is a static function rather than a constructor to allow animals to share 
		//		identical Activities and easily construct arbitrary Activity child classes.
		//
		//	Returns:
		//		An activity matching the file.
		//
		//	Side effects:
		//		Allocated memory with "new".
		//
		//******************************

		//*****************************************************************************************
		virtual char * pcSave(char *  pc_buffer) const;

		//*****************************************************************************************
		virtual const char * pcLoad(const char *  pc_buffer);

};


//*********************************************************************************************
//
class CActivityDistance : public CActivity
//
//	Prefix: actd
//
//	An activity whose rating falls off linearly with distance.
//
//	Notes:
//
//*********************************************************************************************
{
public:

	TReal rStartFalloff;		// Distance at which we start falling off.  Default 1.0 meters
	TReal rFalloffFactor;		// How quickly does the graph fall off after the start point?  Default 1.0
								// Both must be positive.

	//	Fallof graph is-
	//
	//  Multiplier M
	// 
	//		for d <= rStartFalloff
	//			M = 1
	//		for d >  rStartFalloff
	//			M = 1 / (1 + rFalloffFactor * (d - rStartFalloff))

	//		So at d == rStartFalloff
	//			M = 1 / (1 + rFalloffFactor * (rStartFalloff - rStartFalloff)) = 1 / (1 + 0) = 1
	//
	//

	//
	//	Variable declarations.
	//
public:

	CActivityDistance(char *ac_name);
	CActivityDistance();

	//*************************************************************************************
	//
	virtual CRating rtRate
	(
		const CFeeling&	feel,		// The feeling used to evaluate the action.
		CInfluence*		pinf		// The direct object of the action.
	) override;
	//
	//******************************

#if VER_TEST
	//*****************************************************************************************
	virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


// A helper macro to make activity writing easier.
#define pbrBRAIN (gaiSystem.pbrGetCurrentBrain())
#define paniANIMAL (gaiSystem.pbrGetCurrentAnimal())
#define rDISTANCE_SQR(v1, v2)  ((v1 - v2).tLenSqr())
#define rDISTANCE(v1, v2)  ((v1 - v2).tLen())
#define rATTACK_RANGE  (3 * pbrBRAIN->rHeadReach)


//#ifndef HEADER_LIB_AI_ACTIVITY_HPP
#endif
