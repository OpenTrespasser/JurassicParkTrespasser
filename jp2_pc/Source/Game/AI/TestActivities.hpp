/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CActivity child classes for test behaviors.
 *
 * Naming Conventions:
 *	All activities begin with "CActivity" followed by successive subclasses.
 *
 * Bugs:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/TestActivities.hpp                                            $
 * 
 * 5     7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 4     5/01/98 1:37p Agrant
 * test head position with random speed
 * 
 * 3     2/11/98 12:57p Agrant
 * More complete descriptions for test activities.
 * 
 * 2     12/18/97 7:35p Agrant
 * Better grouped behavior through the CONTINUE flag.
 * 
 * 1     12/17/97 9:28p Agrant
 * Initial revision
 * 
 * 4     5/26/97 1:43p Agrant
 * AI files now all compile individually under LINT without errors.
 * 
 * 3     5/04/97 9:30p Agrant
 * Reworked the way activities are handled-  each now operates on an
 * InfluenceList.
 * 
 * 2     4/29/97 6:42p Agrant
 * CFeeling now float-based instead of CRating-based.
 * Activities now rate/act based on a list of influences.
 * Better rating functions for activities.
 * Debugging tools for activity processing.
 * 
 * 1     11/02/96 7:15p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_TESTACTIVITIES_HPP
#define HEADER_GAME_AI_TESTACTIVITIES_HPP

#include "Activity.hpp"
#include "Synthesizer.hpp"

//*********************************************************************************************
//
class CActivityTest : public CActivity
//
//	Prefix: at
//
//	A test behavior
//
//*********************************************************************************************
{
public:
	TSec sDuration;		// How long to hold the test.
	TSec sStart;		// When the test started.
	TSec sStop;			// When the test will end.
	CRating	rtSpeed;	// How fast should we perform the action?

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityTest
		(
			const char* str_name
		);

#if VER_TEST			
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif

};

//*********************************************************************************************
//
class CActivityTestHeadCock : public CActivityTest
//
//	Prefix: 
//
//	A behavior in which the animal orients its head.
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	float fAngle;	// The target head cock angle.


	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityTestHeadCock 
		(
		) : CActivityTest("Test Head Cock")
		{
		};
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
		
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence*		pinf);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};


//*********************************************************************************************
//
class CActivityTestHeadOrient : public CActivityTest
//
//	Prefix: atho
//
//	A behavior in which the animal orients its head.
//
//*********************************************************************************************
{
public:
	CVector3<> v3Target;	// Where we want the head to point.

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityTestHeadOrient 
		(
		) : CActivityTest("Test Head Orient")
		{
		};
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
		
		//*********************************************************************************
		virtual void Act(CRating rt_importance,	CInfluence*	pinf);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};


//*********************************************************************************************
//
class CActivityTestWagTail : public CActivityTest
//
//	Prefix: 
//
//*********************************************************************************************
{

public:
	float fWagAmplitude;		// Radians.
	float fWagFrequency;		// Wags/second.
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityTestWagTail 
		(
		) : CActivityTest("Test Wag Tail")
		{};
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
		
		//*********************************************************************************
		virtual void Act(CRating rt_importance,	CInfluence*	pinf);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};

//*********************************************************************************************
//
class CActivityTestHeadPosition : public CActivityTest
//
//	Prefix: atho
//
//	A behavior in which the animal orients its head.
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	CVector3<> v3Target;
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityTestHeadPosition 
		(
		) : CActivityTest("Test Head Position")
		{};
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
		
		//*********************************************************************************
		virtual void Act(CRating rt_importance,	CInfluence*	pinf);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};

//*********************************************************************************************
//
class CActivityTestMouth : public CActivityTest
//
//	Prefix: atho
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	CRating rtOpen;	// Proportion of open-ness of the mouth.
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityTestMouth
		(
		) : CActivityTest("Test Mouth")
		{
		};
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
		
		//*********************************************************************************
		virtual void Act(CRating rt_importance,	CInfluence*	pinf);

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length);
#endif
};

//#ifndef HEADER_GAME_AI_ACTIVITYATTACK_HPP

#endif

