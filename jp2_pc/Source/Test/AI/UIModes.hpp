/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Mode information and documentation for AI Test App interface
 *
 * Bugs:
 *
 * To do:
 *		Implement mouse cursor changes to show UI mode.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/UIModes.hpp                                                   $
 * 
 * 12    9/09/97 8:51p Agrant
 * Move, goto modes
 * 
 * 11    5/09/97 12:13p Agrant
 * Mouse player debug option
 * 
 * 10    4/08/97 6:42p Agrant
 * Debugging improvements for AI Test app.
 * Allows test app treatment of GUIApp AI's.
 * Better tools for seeing what is going on in an AI's brain.
 * 
 * 9     2/05/97 1:05p Agrant
 * Added a graph test mode for testing A* graph searches.
 * 
 * 8     11/14/96 11:17p Agrant
 * Added Club mode to fake collisions with dinos.
 * 
 * 7     11/02/96 7:12p Agrant
 * added meat and grass
 * 
 * 6     10/14/96 12:21p Agrant
 * Added walls to test app.
 * 
 * 5     10/02/96 8:11p Agrant
 * Added Sheep and Wolves.
 * Added Influence Debugging.
 * Added AI frame rate counter.
 * Unified all meter/pixel conversions.
 * 
 * 4     9/27/96 5:44p Agrant
 * Added Trees
 * Added some notion of acceleration to dino model.
 * 
 * 3     8/30/96 4:52p Agrant
 * objects now have facing and velocity
 * 
 * 2     8/29/96 5:31p Agrant
 * Major revision
 * Mostly changed code to match coding spec
 * Turned the object list into a class
 * Incorporated a timer
 * Now using TDefReal instead of float
 * 
 * 1     8/28/96 4:11p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_AI_UIMODES_HPP
#define HEADER_TEST_AI_UIMODES_HPP


//
// Typedefs.
//



enum EUIMode
// Mode ID's and descriptions.
// Prefix: eui
{
	euiADDDINOSAUR=0,			// waiting for user to pick a location for new dino
	euiADDTREE,					// waiting for user to pick a location for new tree
	euiADDWOLF,					// waiting for user to pick a location for new wolf
	euiADDSHEEP,				// waiting for user to pick a location for new sheep
	euiADDMEAT,
	euiADDGRASS,
	euiADDHINT,
	euiREMOVEOBJECT,			// waiting for user to pick object to remove
	euiSELECTOBJECT,			// waiting for user to select one or more objects
	euiBEGINFACINGOBJECT,		// waiting for user to begin to alter facing
	euiCURRENTLYFACINGOBJECT,	// waiting for user to finish altering facing
	euiADDWALL,					// waiting for user to begin adding a wall
	euiCURRENTLYADDINGWALL,		// user has picked an endpoint for the wall, waiting other pt
	euiCLUB,					// user can club animals to give them collision events
	euiADDTESTNODE,				// user can click to add a test node
	euiSETSTARTNODE,			// user can click to select the start node
	euiSETSTOPNODE,				// user can click to select the stop node
	euiREMOVETESTNODE,			// user can click to delete the nearest node
	euiMOUSEPLAYER,				// animals see the mouse as a player
	euiSTARTMOVE,				// user can select object to move
	euiMOVING,					// user is moving an object
	euiGOTO,					// user can issue a command to the selected animal
	euiEND						// the number of modes
};

//
// Variables.
//

extern EUIMode euiMode;		// global mode for AI user interface

//
//   Function declarations.
//

//*********************************************************************************************
//
void InitMode
(
);
//
// Initializes the mode system.
//

//*********************************************************************************************
//
void SetMode
(
	EUIMode eui
);
//
//	Sets the mode of the UI to "eui".  May change mouse cursor to match mode.
//
//	Side Effects:
//		Alters euiMode.
//
//	To do:
//		Change mouse cursor to match mode.
//
//	Bugs:
//		The mouse cursor changes do not work.
//
//****************************************

//*********************************************************************************************
//
inline EUIMode euiGetMode
(
)
//
//	Gets the mode of the UI.
//
//****************************************
{
	return euiMode;
}


// Ends #ifndef HEADER_TEST_AI_UIMODES_HPP
#endif 