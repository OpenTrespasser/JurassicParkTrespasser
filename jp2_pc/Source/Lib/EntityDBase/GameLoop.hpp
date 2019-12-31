/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		The main game loop.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/GameLoop.hpp                                         $
 * 
 * 11    98/04/09 20:34 Speter
 * Added ability to replay with in specified step increments.
 * 
 * 10    11/11/97 9:29p Agrant
 * Empty the Trash list on deleting the GameLoop
 * 
 * 9     97/09/23 1:15p Pkeet
 * Added the 'bPreload' member function.
 * 
 * 8     97/09/11 20:55 Speter
 * .Step() now optionally take a step time.  
 * 
 * 7     4/09/97 12:09p Rwyatt
 * Added log
 * 
 *********************************************************************************************/

#ifndef HEADER_ENTITYDBASE_GAMELOOP_HPP
#define HEADER_ENTITYDBASE_GAMELOOP_HPP

#include <list>

#include "Container.hpp"
#include "Instance.hpp"
#include "Lib/Sys/Timer.hpp"

//
// Enumeration types.
//

enum EGameMode
// Enumeration of game modes available.
// Prefix: egm
{
	egmPLAY,	// Simulation mode.
	egmDEBUG	// Debug mode.
};


//
// Class definitions.
//

//*********************************************************************************************
//
class CGameLoop
//
// Handles game loop execution.
//
// Prefix: gml
//
//**************************
{
public:

	EGameMode							egmGameMode;	// Game mode.
	bool								bPauseGame;		// Pause flag.
	bool								bPreload;		// Performs preloading.
	CContainer<std::list <CInstance*> >		lpinsTrash;		// Trash list- instances that are to
														// be discarded at end of frame.

public:
	
	//*****************************************************************************************
	//
	// Constructor.
	//

	CGameLoop();

	~CGameLoop();
	
	//*****************************************************************************************
	//
	// Member functions.
	//

	//*****************************************************************************************
	//
	bool bCanStep
	(
	);
	//
	// Returns 'true' if the game can step.
	//
	//**************************

	//*****************************************************************************************
	//
	bool bDebug
	(
	)
	//
	// Returns 'true' if the game is in debug mode.
	//
	//**************************
	{
		return egmGameMode == egmDEBUG;
	}

	//*****************************************************************************************
	//
	void Replay
	(
		TSec s_step = -1.0		// Amount to step through replay.  Default is one original step.
	);
	//
	// Executes one step of the main game loop but accuires data from a replay file.
	//
	//	Notes:
	//		Acts just the same as the step function. By default, one call to this gets one frame 
	//		from the current replay file.  However, if s_step is positive, it runs Step with the
	//		given value, getting a new replay message only as necessary.
	//
	//**************************



	//*****************************************************************************************
	//
	void Step
	(
		TSec s_step = -1.0		// Amount to step, or default for real-time.
	);
	//
	// Executes one step of the main game loop.
	//
	//	Notes:
	//		Deletes everything in trash list at end of loop.
	//
	//**************************


	//*****************************************************************************************
	//
	void Paint() const;
	//
	// Redraws the screen.
	//
	//**************************


	//*****************************************************************************************
	//
	void AddToTrash
	(
		CInstance* pins
	)
	//
	// Adds instance to the trash can.
	//
	//**************************
	{
		lpinsTrash.push_back(pins);
	}

	//*****************************************************************************************
	//
	void EmptyTrash
	(
	);
	//
	// Deletes all trash.
	//
	//**************************

};


//
// Global Variables.
//

// Game loop object.
extern CGameLoop gmlGameLoop;


#endif
