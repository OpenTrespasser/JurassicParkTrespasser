/***********************************************************************************************
 *
 * $Source::																				   $	
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The AI classes.  This header file should be the first AI-specific header included by 
 *		any AI .cpp file.  It is not needed for the .cpp files, but instead for the .hpp files
 *		included by that file.
 *
 *		This file is useful because it makes references of the form 
 *		CClass *cls meaningful in a .hpp file without requiring that the .hpp file include 
 *		another .hpp file.  This reduces compile times and makes it possible to create two
 *		classes that reference each other.
 *
 *		This file can always be included at any point without breaking a build.
 *		Or that's the theory.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/Classes.hpp                                                   $
 * 
 * 8     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 7     2/16/97 5:37p Agrant
 * Even better pathfinding.
 * 
 * 6     10/31/96 7:27p Agrant
 * changed over to game object system and world database
 * 
 * 5     10/23/96 7:38p Agrant
 * more debugging info
 * first pass at the tree avoiding pathfinder
 * 
 * 4     10/15/96 9:20p Agrant
 * Synthesizer reworked to be better, stronger, smarter, faster than before.
 * Activity Rate and Act and Register functions now have better defined roles.
 * Added some accessor functions to smooth future changes.
 * 
 * 3     10/10/96 7:18p Agrant
 * Modified code for code specs.
 * 
 * 2     9/30/96 3:05p Agrant
 * modified for enum code spec
 * 
 * 
 * 1     9/23/96 3:03p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_CLASSES_HPP
#define HEADER_GAME_AI_CLASSES_HPP



class CActivity;
class CActivityCompound;
class CBrain;
class CActivityDOSubBrain;
class CFeeling;
class CInfluence;
class CPath;
class CPathfinder;
class CPerception;
class CSynthesizer;


inline bool bValid(void *pointer)
{
	return (pointer != 0 &&
			(uint) pointer != 0xcdcdcdcd);
}

// #ifndef HEADER_GAME_AI_CLASSES_HPP
#endif
