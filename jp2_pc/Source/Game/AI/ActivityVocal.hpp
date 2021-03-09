/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CActivity child classes for vocalization behaviors.
 *
 * Bugs:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/ActivityVocal.hpp                                             $
 * 
 * 16    9/06/98 7:49p Agrant
 * added some head cock 
 * 
 * 15    8/22/98 7:22p Agrant
 * use startle flag
 * 
 * 14    8/14/98 11:36a Agrant
 * Howl when you are in a mood to celebrate.
 * 
 * 13    7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 12    6/14/98 8:39p Agrant
 * dynamic howling
 * 
 * 11    5/29/98 7:44p Agrant
 * fixes to VocalDie
 * 
 * 10    5/29/98 3:21p Agrant
 * added VocalDie
 * 
 * 9     5/16/98 11:50p Agrant
 * many tweaks
 * 
 * 8     5/12/98 3:53p Agrant
 * Howl implementation cooler
 * 
 * 7     5/08/98 5:34p Agrant
 * more better vocal code
 * 
 * 6     5/08/98 1:02a Agrant
 * Vocals specified in data.
 * 
 * 5     5/06/98 8:59p Agrant
 * reworked vocal activities-
 * Ouch now has actual samples
 * 
 * 4     2/12/98 1:11p Agrant
 * Added descriptions.
 * 
 * 3     2/05/98 12:15p Agrant
 * Better dino resource settings.
 * 
 * 2     1/07/98 5:11p Agrant
 * Shell of future activities
 * 
 * 1     7/19/97 1:33p Agrant
 * intial rev
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_ACTIVITYVOCAL_HPP
#define HEADER_GAME_AI_ACTIVITYVOCAL_HPP

#include "Activity.hpp"
#include "Synthesizer.hpp"


#define MAX_PATH 256

//*********************************************************************************************
//
class CActivityVocal : public CActivity
//
//	Prefix: av
//
//	A behavior in which the animal makes a sound.
//
//  Notes:
//		Base class for vocalizations.  Keeps track of sounds in progress.
//
//*********************************************************************************************
{

public:

//
//  Member function definitions.
//

	EVocalType	evtVocalType;		// Which kind of vocal is this?
	CRating		rtMaxMouth;			// The amount the mouth opens with this call.
	CRating		rtMouthTarget;		// How far we want to open right now.
	TSec		sNextMouthPosition;	// When to shift to a new mouth mouth posision.
	CRating		rtMouthSpeed;
	TReal rHeadCock;			// The head cock angle.
	TSec		sWait;				// The amount of time that must pass after the previous vocal
									// before we'll consider this one again.
	

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		// Probably won't ever need a default vocal activity.
		CActivityVocal(){  Assert(0); }

		CActivityVocal
		(
			char *ac_name
		);
	

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
class CActivityVocalOuch : public CActivityVocal
//
//	Prefix: avo
//
//	A behavior in which the animal screams in pain.
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

		CActivityVocalOuch
		(
		);
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

		//*********************************************************************************
		virtual void Act(CRating	rt_importance, CInfluence* pinf) override;

};


//*********************************************************************************************
//
class CActivityVocalSnarl : public CActivityVocal
//
//	Prefix: avo
//
//	A behavior in which the animal screams in pain.
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

		CActivityVocalSnarl
		(
		);
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		
		//*********************************************************************************
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		) override;

};

//*********************************************************************************************
//
class CActivityVocalHelp : public CActivityVocal
//
//	Prefix: avo
//
//	A behavior in which the animal screams in pain.
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

		CActivityVocalHelp
		(
		);
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		
		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

		//*********************************************************************************
		virtual void Act(CRating	rt_importance, CInfluence* pinf) override;

};

//*********************************************************************************************
//
class CActivityVocalHowl : public CActivityVocal
//
//	Prefix: avo
//
//	A behavior in which the animal screams in pain.
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	CVector3<> v3HeadPoint;
	TSec		sNextHeadPoint;

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityVocalHowl();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence*	pinf) override;
		
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence*	pinf) override;

};

//*********************************************************************************************
//
class CActivityVocalCroon : public CActivityVocal
//
//	Prefix: avo
//
//	A behavior in which the animal screams in pain.
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

		CActivityVocalCroon();
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		
		//*********************************************************************************
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		) override;

};

//*********************************************************************************************
//
class CActivityVocalDie : public CActivityVocal
//
//	Prefix: avo
//
//	A behavior in which the animal screams in pain.
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

		CActivityVocalDie();
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		
		//*********************************************************************************
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		) override;

		//*************************************************************************************
		virtual CRating rtRate
		(
			const CFeeling&	feel,		// The feeling used to evaluate the action.
			CInfluence*		pinf		// The direct object of the action.
		) override;
};


#endif  //#ifndef HEADER_GAME_AI_ACTIVITYVOCAL_HPP
