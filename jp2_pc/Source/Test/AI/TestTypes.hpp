/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Simple type definitions for AI test app.
 *		
 *
 * Bugs:
 *
 * To do:
 *
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/TestTypes.hpp                                                 $
 * 
 * 7     10/14/96 12:21p Agrant
 * Added walls to test app.
 * 
 * 6     10/02/96 8:10p Agrant
 * Added Sheep and Wolves.
 * Added Influence Debugging.
 * Added AI frame rate counter.
 * Unified all meter/pixel conversions.
 * 
 * 5     9/27/96 5:44p Agrant
 * Added Trees
 * Added some notion of acceleration to dino model.
 * 
 * 4     9/03/96 7:49p Agrant
 * Added the SIMPLE test object type, for the boring objects.
 * 
 * 3     8/29/96 5:31p Agrant
 * Major revision
 * Mostly changed code to match coding spec
 * Turned the object list into a class
 * Incorporated a timer
 * Now using TDefReal instead of float
 * 
 * 2     8/27/96 4:50p Agrant
 * removed improper include
 * 
 * 1     8/21/96 5:32p Agrant
 * initial revision
 * types for the AI test app
 *
 **********************************************************************************************/

#ifndef HEADER_TEST_AI_TESTTYPES_HPP
#define HEADER_TEST_AI_TESTTYPES_HPP

// Time in seconds.
//  Prefix  sec
typedef float			TSeconds;


enum ETestAnimalType
//  Prefix: etat
{
	etatDINOSAUR,
	etatWOLF,
	etatSHEEP,
	etatEND
};

enum ETestObjType
//	Prefix:	etot
{
	etotNONE=0,
	etotSIMPLE,
	etotANIMAL,
	etotTREE,
	etotWALL,
	etotNUM_TYPES
};


// Ends #ifndef HEADER_TEST_AI_TESTTYPES_HPP
#endif	
