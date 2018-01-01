/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The AI's methods for sending commands to other systems.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/SendCommand.hpp                                               $
 * 
 * 1     9/19/96 1:41p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_SENDCOMMAND_HPP
#define HEADER_GAME_AI_SENDCOMMAND_HPP

//
// Function prototypes.
//

//*********************************************************************************************
//
void AISendMove
(
	CObject*			pobj,		// Handle of object receiving command.
	const CVector2<>&   v2_dir,		// Direction and magnitude of desired motion.
);
//
//	Notifies the physics system of the desire of "pobj" to have velocity "v2_dir".
//
//**************************************

// #ifndef HEADER_GAME_AI_SENDCOMMAND_HPP
#endif