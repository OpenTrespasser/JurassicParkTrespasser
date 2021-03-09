/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		CActivity child classes for emotional behaviors.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/EmotionActivities.hpp                                         $
 * 
 * 8     9/08/98 7:40p Agrant
 * dont cower when your foe is not looking
 * 
 * 7     9/06/98 7:49p Agrant
 * added some head cock 
 * 
 * 6     8/22/98 7:20p Agrant
 * rearback responds to startle flag
 * 
 * 5     7/29/98 3:08p Agrant
 * added nothing activity, cleaned up
 * 
 * 4     7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 3     2/12/98 1:11p Agrant
 * Added descriptions.
 * 
 * 2     1/08/98 6:07p Agrant
 * Pseudocode implementations of emotional activities
 * 
 * 1     1/07/98 6:50p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_EMOTIONACTIVITIES_HPP
#define HEADER_GAME_AI_EMOTIONACTIVITIES_HPP

#include "Activity.hpp"

//*********************************************************************************************
//
class CActivityLiftTail : public CActivity
//
//	Prefix: ala
//
//	Exhibits curiosity
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityLiftTail();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


//*********************************************************************************************
//
class CActivityRearBack : public CActivity
//
//	Prefix: ala
//
//	Exhibits curiosity
//
//*********************************************************************************************
{
public:
		TReal rHeadCock;			// The head cock angle.
//
//  Member function definitions.
//



	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityRearBack();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

		//*********************************************************************************
		virtual void Act(CRating	rt_importance, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityBackAway : public CActivity
//
//	Prefix: ala
//
//	Exhibits curiosity
//
//*********************************************************************************************
{
public:
		TReal rHeadCock;			// The head cock angle.

//
//  Member function definitions.
//

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityBackAway();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityCower : public CActivity
//
//	Prefix: ala
//
//	Exhibits curiosity
//
//*********************************************************************************************
{
public:
		TReal rHeadCock;			// The head cock angle.

	//
	//  Member function definitions.
	//


	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityCower();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityLashTail : public CActivity
//
//	Prefix: ala
//
//	Exhibits curiosity
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityLashTail();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivitySleep : public CActivity
//
//	Prefix: ala
//
//	Exhibits curiosity
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivitySleep();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityLieDown : public CActivity
//
//	Prefix: ala
//
//	Exhibits curiosity
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityLieDown();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityScratch : public CActivity
//
//	Prefix: ala
//
//	Exhibits curiosity
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityScratch();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityNothing : public CActivity
//
//	Prefix: an
//
//	Brain dead activity.
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityNothing();
		
	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


//#ifndef HEADER_GAME_AI_EMOTIONACTIVITIES_HPP
#endif