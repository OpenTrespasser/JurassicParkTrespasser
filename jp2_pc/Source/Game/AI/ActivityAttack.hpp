/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CActivity child classes for attack behaviors.
 *
 * Naming Conventions:
 *	All activities begin with "CActivity" followed by successive subclasses.
 *
 * Bugs:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/ActivityAttack.hpp                                            $
 * 
 * 24    9/17/98 2:51p Agrant
 * Lead your target for bites
 * 
 * 23    9/08/98 7:40p Agrant
 * Different behavior when target faces away from you
 * 
 * 22    9/06/98 7:49p Agrant
 * added some head cock 
 * 
 * 21    8/27/98 1:44a Agrant
 * eat & drink tweak
 * 
 * 20    8/26/98 7:13p Agrant
 * Major EAT DRINK rework
 * 
 * 19    8/20/98 11:11p Agrant
 * implemented tail swipe attack
 * 
 * 18    7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 17    6/25/98 4:51p Agrant
 * Some combat appearance tweaks
 * 
 * 16    5/16/98 7:04p Agrant
 * eat hacking
 * 
 * 15    5/16/98 3:05p Agrant
 * Charge activities, drinking first pass
 * 
 * 14    5/15/98 4:33p Agrant
 * Feint now inherits from bite.
 * Feint implemented.
 * 
 * 13    5/15/98 3:13p Agrant
 * Targeting code to make it possible to dodge dino attacks
 * 
 * 12    5/14/98 8:09p Agrant
 * moved constructors to the .cpp file
 * 
 * 11    2/13/98 1:53p Agrant
 * Rework multipliers, continues, and was used last time data
 * 
 * 10    2/12/98 1:11p Agrant
 * Added descriptions.
 * 
 * 9     2/06/98 3:09p Agrant
 * Better Grab rating function
 * 
 * 8     2/05/98 12:15p Agrant
 * Better dino resource settings.
 * 
 * 7     2/03/98 4:33p Agrant
 * Updated some dino resource requirements.
 * 
 * 6     1/08/98 3:14p Agrant
 * Pseudocode implementations of new attack actitivities
 * 
 * 5     1/07/98 5:11p Agrant
 * Shell of future activities
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

#ifndef HEADER_GAME_AI_ACTIVITYATTACK_HPP
#define HEADER_GAME_AI_ACTIVITYATTACK_HPP

#include "Activity.hpp"
#include "ActivityPhased.hpp"

#include "Synthesizer.hpp"



//*********************************************************************************************
//
class CActivityBite : public CActivity
//
//	Prefix: ab
//
//	A behavior in which the animal bites the target.
//
//  Notes:
//		The default rating feeling for this activity has a high affinity for Anger and Hunger.
//
//*********************************************************************************************
{
public:
	CVector3<>	v3Target;		// The point we will try to bite once we have a target lock.
	TReal		rLockDistance;	// The distance at which we acquire target lock.
								// (Head to target distance)
	TReal		rCinematicAngle;
	TReal		rHeadCock;		// The head cock angle.
	TReal		rLeadFactor;	// How aggressively do we lead?  Zero means none.

//
//  Member function definitions.
//

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityBite
		(
		);

		//*****************************************************************************************
		virtual void MaybeSetTargetLock(CInfluence *pinf);


	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		//
		virtual CRating rtRate
		(
			const CFeeling&	feel,		// The feeling used to evaluate the action.
			CInfluence*		pinf		// The direct object of the action.
		) override;
		//
		//******************************
		
		//*********************************************************************************
		//
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		) override;
		//
		//	Bite the influence.
		//
		//******************************

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif

};


//*********************************************************************************************
//
class CActivityClaw : public CActivity
//
//	Prefix: ac
//
//	A behavior in which the animal claws the target with a foreclaw.
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

		CActivityClaw
		(
		);
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		//
		virtual CRating rtRate
		(
			const CFeeling&	feel,		// The feeling used to evaluate the action.
			CInfluence*		pinf		// The direct object of the action.
		) override;
		//
		//******************************
		
		//*********************************************************************************
		//
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		) override;
		//
		//	Claw the influence.
		//
		//******************************

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif

};

//*********************************************************************************************
//
class CActivityFeint : public CActivityBite
//
//	Prefix: af
//
//	A behavior in which the animal fakes a bite attack.
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

	CActivityFeint();


	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel,	CInfluence*		pinf) override;

		//*****************************************************************************************
		virtual void MaybeSetTargetLock(CInfluence *pinf) override;
	
#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif

};


//*********************************************************************************************
//
class CActivityRam : public CActivity
//
//	Prefix: ar
//
//	A behavior in which the animal slams into the target.
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	CVector3<> v3Target;		// Where are we ramming to?
	CVector3<> v3HeadPoint;		// Where are we sticking our head?
	TReal	rHeadCock;			// The head cock angle.

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityRam();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		//
		virtual CRating rtRate
		(
			const CFeeling&	feel,		// The feeling used to evaluate the action.
			CInfluence*		pinf		// The direct object of the action.
		) override;
		//
		//******************************
		
		//*********************************************************************************
		//
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		) override;
		//
		//	Ram.
		//
		//******************************

		//*********************************************************************************
		//
		virtual void SetHeadPoint
		(
			CInfluence*		pinf
		);
		//
		//	Set the direction that we'll point our head.
		//
		//******************************



#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif

};


//*********************************************************************************************
//
class CActivityEat : public CActivityPhased
//
//	Prefix: ae
//
//	A behavior in which the animal takes a bite of some inert thing.
//
//*********************************************************************************************
{
public:

	CVector3<>	v3Target;		// The point we will try to bite once we have a target lock.
								// We target part of of the thing we eat, not necessarily its center.
	CVector3<>	v3TearTarget;	// The point we go to to tear the food off.
	TSec		sMouthTime;		// When we switch from opening to closing the mouth, or vice versa.
	TSec		sChewTime;		// When do we stop chewing?
	bool		bOpening;
	TReal rHeadCock;			// The head cock angle.

//
//  Member function definitions.
//

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

	CActivityEat();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel,	CInfluence*		pinf) override;

		//*************************************************************************************
		virtual void Start(CInfluence* pinf) override;

		//*************************************************************************************
		virtual void DoPhaseOne(CRating rt_importance,	CInfluence*	pinf) override;

		//*************************************************************************************
		virtual void DoPhaseTwo(CRating rt_importance,	CInfluence*	pinf) override;

		//*************************************************************************************
		virtual void DoPhaseThree(CRating rt_importance,	CInfluence*	pinf) override;

		//*************************************************************************************
		virtual void DoPhaseFour(CRating rt_importance,	CInfluence*	pinf) override;


#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


//*********************************************************************************************
//
class CActivityDrink : public CActivityPhased
//
//	Prefix: ad
//
//	A behavior in which the animal takes a drink.
//
//*********************************************************************************************
{
public:

	CVector3<>	v3TargetOpen;
	CVector3<>	v3TargetClosed;
	bool		bOpening;


//
//  Member function definitions.
//




	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

	CActivityDrink();
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel,	CInfluence*		pinf) override;

		//*************************************************************************************
		virtual void Start(CInfluence* pinf) override;

		//*************************************************************************************
		virtual void Stop(CInfluence* pinf) override;

		//*************************************************************************************
		virtual void DoPhaseOne(CRating rt_importance,	CInfluence*	pinf) override;

		//*************************************************************************************
		virtual void DoPhaseTwo(CRating rt_importance,	CInfluence*	pinf) override;

		//*************************************************************************************
		virtual void DoPhaseThree(CRating rt_importance,	CInfluence*	pinf) override;

		//*************************************************************************************
		virtual void DoPhaseFour(CRating rt_importance,	CInfluence*	pinf) override;


#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif

};



//*********************************************************************************************
//
class CActivityJumpAndBite : public CActivity
//
//	Prefix: ad
//
//	A behavior in which the animal takes a drink.
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

	CActivityJumpAndBite();
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		//
//		virtual CRating rtRate
//		(
//			const CFeeling&	feel,		// The feeling used to evaluate the action.
//			CInfluence*		pinf		// The direct object of the action.
//		);
		//
		//******************************
		
		//*********************************************************************************
		//
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		) override;
		//
		//	Jump at foe and bite them as you get near.
		//
		//******************************

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif

};


//*********************************************************************************************
//
class CActivityJumpAndClaw : public CActivity
//
//	Prefix: ad
//
//	A behavior in which the animal takes a drink.
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

		CActivityJumpAndClaw();
	

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		//
//		virtual CRating rtRate
//		(
//			const CFeeling&	feel,		// The feeling used to evaluate the action.
//			CInfluence*		pinf		// The direct object of the action.
//		);
		//
		//******************************
		
		//*********************************************************************************
		//
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		) override;
		//
		//	Jump on foe and stomp/claw them.
		//
		//******************************

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityShoulderCharge : public CActivityRam
//
//	Prefix: ad
//
//	A behavior in which the animal takes a drink.
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

		CActivityShoulderCharge();


		
		//*********************************************************************************
		virtual void SetHeadPoint(CInfluence*		pinf) override;

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityTailSwipe : public CActivityPhased
//
//	Prefix: ad
//
//	A behavior in which the animal takes a drink.
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	CVector2<> v2TailTarget;				// The direction to point the tail during the swipe.
	CVector2<> v2TailTargetIntermediate;	// The direction to point the tail during the swipe.
	TReal rHeadCock;			// The head cock angle.

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

	CActivityTailSwipe();
	
	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel,	CInfluence*		pinf) override;

		//*************************************************************************************
		virtual void Start();

		//*************************************************************************************
		virtual void DoPhaseOne(CRating rt_importance,	CInfluence*	pinf) override;

		//*************************************************************************************
		virtual void DoPhaseTwo(CRating rt_importance,	CInfluence*	pinf) override;

		//*************************************************************************************
		virtual void DoPhaseThree(CRating rt_importance,	CInfluence*	pinf) override;

		//*************************************************************************************
		virtual void DoPhaseFour(CRating rt_importance,	CInfluence*	pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityGrab : public CActivity
//
//	Prefix: ad
//
//	A behavior in which the animal takes a drink.
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

		CActivityGrab();
	
	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		//
//		virtual CRating rtRate
//		(
//			const CFeeling&	feel,		// The feeling used to evaluate the action.
//			CInfluence*		pinf		// The direct object of the action.
//		);
		//
		//******************************
		
		//*********************************************************************************
		//
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		) override;
		//
		//	Grab target with mouth.
		//
		//******************************

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityThrow : public CActivity
//
//	Prefix: ad
//
//	A behavior in which the animal takes a drink.
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

		CActivityThrow();


	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		//
//		virtual CRating rtRate
//		(
//			const CFeeling&	feel,		// The feeling used to evaluate the action.
//			CInfluence*		pinf		// The direct object of the action.
//		);
		//
		//******************************
		
		//*********************************************************************************
		//
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		) override;
		//
		//	Only works when Grabbing-  throw thing held in your mouth.
		//
		//******************************

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityShake : public CActivity
//
//	Prefix: ad
//
//	A behavior in which the animal takes a drink.
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

	CActivityShake();
	
	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*************************************************************************************
		//
//		virtual CRating rtRate
//		(
//			const CFeeling&	feel,		// The feeling used to evaluate the action.
//			CInfluence*		pinf		// The direct object of the action.
//		);
		//
		//******************************
		
		//*********************************************************************************
		//
		virtual void Act
		(
			CRating			rt_importance,
			CInfluence*		pinf
		) override;
		//
		//	Shake the thing held in your mouth, much as a dog does.
		//
		//******************************

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};



//#ifndef HEADER_GAME_AI_ACTIVITYATTACK_HPP

#endif

