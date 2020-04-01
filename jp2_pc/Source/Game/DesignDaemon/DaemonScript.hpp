/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Script for the Design Daemon.
 *
 * Bugs:
 *
 * Notes:
 * 		Towards the goal of simplifying the appearance of the DaemonScript implementation
 * 		module, the body of many functions have been implemented in "Daemon.cpp."
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/DesignDaemon/DaemonScript.hpp                                   $
 * 
 * 8     97/01/31 12:42p Pkeet
 * Added the 'iRandom' function.
 * 
 * 7     12/16/96 9:31p Pkeet
 * Added dino hits.
 * 
 * 6     12/16/96 3:38p Pkeet
 * Added the fRandom function.
 * 
 * 5     12/16/96 3:27p Pkeet
 * Changed the interface to the player hit function giving the number of hit points endured and
 * the number of hit points left.
 * 
 * 4     12/14/96 3:30p Pkeet
 * Added the 'RegisterTimeTrigger' functions.
 * 
 * 3     11/18/96 4:06p Pkeet
 * Added the 'PlayerIsHit' function and sample code.
 * 
 * 2     11/18/96 1:51p Pkeet
 * Added the 'VoiceOver' function call.
 * 
 * 1     11/16/96 4:59p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GAME_DESIGNDAEMON_DAEMONSCRIPT_HPP
#define HEADER_GAME_DESIGNDAEMON_DAEMONSCRIPT_HPP


//
// Includes.
//

#include <map>
#include "Gblinc/Common.hpp"


//
// Type definitions.
//

// Type representing a pointer to a function.
typedef void* TPFunction;
// Prefix: pf

// Type representing a function map.
typedef std::map<uint32, TPFunction, std::less<int> > TPFMap;
// prefix: pfmap


//
// Global functions.
//

//*********************************************************************************************
//
void RegisterScript
(
	TPFMap& rpfmap // Key to function map.
);
//
// Functions representing the Design Daemon script are registered here.
//
//**************************************

//*********************************************************************************************
//
void RegisterPlayerTrigger
(
	TPFMap&     rpfmap,			// Associative map between key and function pointer.
	const char* str,			// Name for associative map key.
	bool (*b_fun)(),			// Pointer to the function associated with the key.
	double      d_distance,		// Distance from the player to the trigger to cause activation.
	bool        b_view = false	// If 'true' the trigger will activate when the player sees it,
								// if 'false' the trigger will rely only on proximity.
);
//
// Maps a function to a string.
//
// Notes:
//		Towards the goal of simplifying the appearance of the DaemonScript implementation
//		module, the body of this function has been implemented in "Daemon.cpp."
//
//**************************************

//*********************************************************************************************
//
void RegisterTimeTrigger
(
	const char* str,		// Name for associative map key.
	bool (*b_fun)(),		// Pointer to the function associated with the key.
	double      d_time		// Time in seconds to activate the trigger.
);
//
// Creates a time trigger, and associates a function with it.
//
//**************************************

//*********************************************************************************************
//
void RegisterTimeTrigger
(
	TPFMap&     rpfmap,			// Associative map between key and function pointer.
	const char* str,		// Name for associative map key.
	bool (*b_fun)(),		// Pointer to the function associated with the key.
	double      d_time		// Time in seconds to activate the trigger.
);
//
// Creates a time trigger, and associates a function with it.
//
//**************************************

//*********************************************************************************************
//
void Register
(
	TPFMap&     rpfmap,	// Associative map between key and function pointer.
	const char* str,	// Name for associative map key.
	bool (*b_fun)()		// Pointer to the function associated with the key.
);
//
// Maps a function to a string.
//
//**************************************

//*********************************************************************************************
//
void Register
(
	TPFMap& rpfmap,	// Associative map between key and function pointer.
	uint32  u4_key,	// Associative map key.
	bool (*b_fun)()	// Pointer to the function associated with the key.
);
//
// Maps a function to an integer key.
//
//**************************************

//*********************************************************************************************
//
void Say
(
	const char* str	// String to write to the message log.
);
//
// Writes a string to the message log.
//
//**************************************

//*********************************************************************************************
//
void VoiceOver
(
	const char* str	// Wave file to play.
);
//
// Plays a wave file.
//
//**************************************

//*********************************************************************************************
//
void PlayerIsHit
(
	float fHitPointsLeft,	// The number of hit points left.
	float fHitPoints		// The number of hit points in this strike.
);
//
// Designers should enter code here for handling hits to the player.
//
//**************************************

//*********************************************************************************************
//
void DinoIsHit
(
	float fHitPointsLeft,	// The number of hit points left.
	float fHitPoints		// The number of hit points in this strike.
);
//
// Designers should enter code here for handling hits to a dino.
//
//**************************************

//*********************************************************************************************
//
float fRandom
(
	float f_a,
	float f_b
);
//
// Returns a random number between f_a and f_b.
//
//**************************************

//*********************************************************************************************
//
int iRandom
(
	int i_a,
	int i_b
);
//
// Returns a random number a <= random number <= b.
//
//**************************************


#endif
