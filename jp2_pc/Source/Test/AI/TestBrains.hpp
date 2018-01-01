/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	Test Brain classes for AI Test app.  The test classes have predefined sets of activities and
 *  perceptions instead of file-loaded sets.
 *		
 *
 * Bugs:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/TestBrains.hpp                                                $
 * 
 * 16    1/13/98 9:34p Agrant
 * Moved constructor implementations to .cpp
 * 
 * 15    5/11/97 12:51p Agrant
 * Moved fullbrain constructor to .cpp file
 * 
 * 14    5/04/97 9:40p Agrant
 * New brain types for debugging.
 * 
 * 13    4/29/97 6:40p Agrant
 * Already adding a move away activity in Brain::Brain
 * 
 * 12    3/19/97 2:20p Agrant
 * Some activities are now in the base Brain class.
 * 
 * 11    11/20/96 1:25p Agrant
 * Brains now have an Archetype, which describes a set of default behaviors.
 * 
 * 10    11/02/96 7:20p Agrant
 * Added animal names
 * Added biting activity
 * 
 * 9     10/31/96 7:26p Agrant
 * changed over to world database as seen in the Game.  Yahoo.
 * 
 * 8     10/23/96 7:39p Agrant
 * more debugging
 * now using the dialog box for printing some text
 * first pass pathfinder
 * 
 * 7     10/15/96 9:18p Agrant
 * minor housekeeping for synthesizer rework, and associated changes
 * 
 * 6     10/10/96 7:18p Agrant
 * Modified for code spec.
 * 
 * 5     10/02/96 8:10p Agrant
 * Added Sheep and Wolves.
 * Added Influence Debugging.
 * Added AI frame rate counter.
 * Unified all meter/pixel conversions.
 * 
 * 4     9/30/96 3:06p Agrant
 * modified for enum code spec
 * 
 * 
 * 3     9/27/96 5:44p Agrant
 * Added Trees
 * Added some notion of acceleration to dino model.
 * 
 * 2     9/26/96 5:59p Agrant
 * AI system basic structure implemented
 * AI Test app has basic herding behavior based on two simple activities
 * TReal used as basic world coordinate type.
 * 
 * 1     9/23/96 3:03p Agrant
 * initial revision
 **********************************************************************************************/

#ifndef HEADER_TEST_AI_TESTBRAIN_HPP
#define HEADER_TEST_AI_TESTBRAIN_HPP

#include "Game\AI\Brain.hpp"

#include "TestAnimal.hpp"

#include <stdio.h>


//*********************************************************************************************
//
class CHerdBrain : public CBrain
//
//	Prefix: hdb
//
//	The CHerdBrain class is the brain of a herd animal.  It has acticities and perceptions
//  like a herd animal might.
//	
//*********************************************************************************************
{

//  Member function definitions
public:

	//*****************************************************************************************
	//
	//	Constructor and destructor
	//

	CHerdBrain
	(
		CAnimal*	pet_owner,
		EArchetype	ear,
		char*		pc_name = "Generic"
 	);
	
	~CHerdBrain
	(
	)
	{};

};

//*********************************************************************************************
//
class CTestBrain : public CBrain
//
//	Prefix: tb
//
//	The CTestBrain class is a brain that does whatever we're testing at the momnet.
//	
//*********************************************************************************************
{

//  Member function definitions
public:

	//*****************************************************************************************
	//
	//	Constructor and destructor
	//

	CTestBrain
	(
		CAnimal*	pet_owner,
		EArchetype	ear,
		char*		pc_name = "Generic"
 	);

	~CTestBrain
	(
	)
	{};

};

//*********************************************************************************************
//
class CFullBrain : public CBrain
//
//	Prefix: fb
//
//	The CFullBrain class is a brain that does everything.
//	
//*********************************************************************************************
{

//  Member function definitions
public:

	//*****************************************************************************************
	//
	//	Constructor and destructor
	//

	CFullBrain
	(
		CAnimal*	pet_owner,
		EArchetype	ear,
		char*		pc_name
 	);

	~CFullBrain
	(
	)
	{};

};

// #ifndef HEADER_TEST_AI_TESTBRAIN_HPP
#endif

