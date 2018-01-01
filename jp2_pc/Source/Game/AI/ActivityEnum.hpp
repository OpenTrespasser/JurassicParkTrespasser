/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Non-class type definitions for AI library.
 *		
 *
 * Bugs:
 *
 * To do:
 *			
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/ActivityEnum.hpp                                              $
 * 
 * 16    9/05/98 4:38p Agrant
 * add jump and bite activity back in
 * 
 * 15    8/26/98 11:37a Agrant
 * jump, again.  Hopefully it'll build this time.
 * 
 * 14    8/26/98 10:10a Shernd
 * fixed build break
 * 
 * 13    8/26/98 1:04a Agrant
 * replaces jump and bite with jump
 * 
 * 12    8/20/98 11:10p Agrant
 * removed some unused activities
 * 
 * 11    7/29/98 3:09p Agrant
 * added some, removed some
 * 
 * 10    6/09/98 5:51p Agrant
 * added Dash and Approach activities
 * 
 * 9     5/29/98 3:21p Agrant
 * added VocalDie activity
 * 
 * 8     5/19/98 9:15p Agrant
 * two more activities
 * 
 * 7     5/15/98 3:10p Agrant
 * New JumpBack activity
 * 
 * 6     3/23/98 5:29p Agrant
 * Some new activities.
 * 
 * 5     2/13/98 3:32p Agrant
 * Circle activity
 * 
 * 4     1/07/98 5:13p Agrant
 * More activities, and a few symbol changes
 * 
 * 3     1/07/98 2:34p Agrant
 * added numerous bogus activities that will soon be filled in
 * 
 * 2     12/22/97 6:16p Agrant
 * Look At activity added
 * 
 * 1     12/18/97 8:08p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_ACTIVITYENUM_HPP
#define HEADER_LIB_AI_ACTIVITYENUM_HPP

enum EActivityType
//	Prefix: eat
//  Indexes the activities for reference purposes.
{
	// Attacks
	eatEAT=0,	// The activity that involves ingesting comestibles.
	eatBITE,	// BITING target
//	eatCLAW,	// Clawing target
	eatFEINT,	// Fake bite attack
	eatHEAD_BUTT,		// Head butt
	eatDRINK,	// Put head into water and slurp
	eatJUMP_N_BITE,
	eatSHOULDER_CHARGE,
	eatTAIL_SWIPE,
	
	// Vocalizations
	eatOUCH,	// Vocalize pain
	eatHELP,	// Vocalize fear
	eatHOWL,	// Boredom/frustration vocalization
	eatSNARL,	// Angry vocalization
	eatCROON,	// Hunting vocalization
	eatDIE,		// Gasping, dying activity

	// Movement
	eatMOVE_TOWARD,	// Move to target
	eatJUMP,
//	eatMOVE_HERD,
	eatMOVE_AWAY,	// Move away from target
	eatDONT_TOUCH,	// Try not to touch target
	eatWANDER,		// Random movement
	eatSTALK,		// Stealthy movement
	eatCIRCLE,		// Orbit target
	eatSTAY_NEAR,	// Never exceed a distance X from target
	eatSTAY_AWAY,	// Never approach within distance X of target (could replace DONT_TOUCH)
	eatFLEE,		// Run away as quickly as possible.
	eatPURSUE,		// Run towards as quickly as possible.
	eatJUMP_BACK,	// Get out of the immediate area of the target.
	eatGET_OUT,		// When you find yourself inside of something you shouldn't be inside, get out.
	eatAPPROACH,	// Run towards something, but in a cinematic, inefficient manner
	eatDASH,		// Quick sprint in a random direction
	eatMOVE_BY,		// Sprint past target, putting you into better attack position

	// Curiosity
	eatLOOK_AT,		// Looks at target.
	eatTASTE,
	eatSNIFF_TARGET,
	eatCOCK_HEAD,

	// Fear
	eatREAR_BACK,	// Rear back in fear
	eatBACK_AWAY,	// Back off
	eatCOWER,		// Huddle down in fear

	// Anger
	eatGLARE,


	// Other
//	eatSLEEP,		// snooze
//	eatSCRATCH,		// Grooming
//	eatLIE_DOWN,	// like SLEEP, but more aware???
	eatSNIFF,		// Sniff the air, the ground
	eatLOOK_AROUND,	// Look about the area
	eatNOTHING,		// Do absolutely nothing.
	
	// Tests
	eatTEST_HEAD_COCK,		// Test head tilt
	eatTEST_HEAD_ORIENT,	// Test head orientation
	eatTEST_WAG_TAIL,		// Test tail wagging
	eatTEST_HEAD_POSITION,	// Test Head Position
	eatTEST_MOUTH,			// test mouth open/close

	eatEND
};

	
#endif  // HEADER_LIB_AI_ACTIVITYENUM_HPP
