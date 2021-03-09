/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CActivity child classes for movement behaviors.
 *
 *  Movement behaviors come in two flavors:  commands and hints.
 *	A "command" is a full fledged movement command.  The strongest movement command rules the
 *  animal's motion.  
 *  A "hint" influences the animal to tend to go in the indicated direction.  It will bend the 
 *  animals command slightly in the direction of the hint.  Hints are useful for herding and 
 *  fleeing.
 *
 * Naming Conventions:
 *	All activities begin with "CActivity" followed by successive subclasses.
 *
 * Bugs:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/MoveActivities.hpp                                            $
 * 
 * 37    9/13/98 8:04p Agrant
 * Save and load stay near and away targets
 * 
 * 36    9/11/98 12:41p Agrant
 * get out now gets out of steep slopes
 * 
 * 35    8/26/98 12:37p Rvande
 * Removed uneeded CActivityJump class scope.  Caused Metrowerks build to break.
 * 
 * 34    8/26/98 11:16a Agrant
 * jump activity first pass
 * 
 * 33    8/23/98 2:20p Agrant
 * circle now distance based
 * 
 * 32    7/31/98 7:50p Agrant
 * complete circle rework
 * 
 * 31    7/29/98 3:07p Agrant
 * added move by, cleaned up
 * 
 * 30    7/24/98 6:29p Agrant
 * make stay away and stay near work independent of load order
 * 
 * 29    7/20/98 10:11p Rwyatt
 * Added VER_TEST to all description text.
 * 
 * 28    7/20/98 12:27p Agrant
 * Better GetOut implementation
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_MOVEACTIVITIES_HPP
#define HEADER_GAME_AI_MOVEACTIVITIES_HPP

#include "Activity.hpp"

class CInstance;

//*********************************************************************************************
//
class CActivityMoveCommandToward : public CActivityDistance
//
//	Prefix: amt
//
//	The CMoveToCommandActivity class is a behavior in which the animal moves towards things it likes.
//
//  Notes:
//		The default rating feeling for this activity has a high affinity for Anger, Love, Hunger, 
//		Thirst, and Curiosity.
//
// 
//
//*********************************************************************************************
{

	CRating rtSpeed;

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityMoveCommandToward();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityMoveHerd : public CActivity
//
//	Prefix: amht
//
//	The CMoveToHintActivity class is a behavior in which the animal moves towards things it likes.
//	The default rating feeling for this activity has a high affinity for Anger, Love, Hunger, 
//	Thirst, and Curiosity.  The movement commands are hints only.
//
// 
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

		CActivityMoveHerd
		(
		);

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityMoveCommandAway : public CActivityDistance
//
//	Prefix: amca
//
//	The CActivityMoveCommandAway class is a behavior in which the animal moves away from things
//  it dislikes.
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

		CActivityMoveCommandAway
		(
		);

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};



//*********************************************************************************************
//
class CActivityMoveCommandDontTouch : public CActivity
//
//	Prefix: amdt
//
//	Do not come into physical contact, but do not run away.
//
//	The CDontTouchActivity class is a behavior in which the animal avoids touching
//	objects in the world, instead staying some distance away.  This distance tends to be
//  fairly small.  This is a full movement command.
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

		CActivityMoveCommandDontTouch
		(
		);
	
	//*****************************************************************************************
	//
	//	Overrides.
	//

		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityMoveHintWander : public CActivity
//
//	Prefix: wact
//
//	The CWanderActivity class is a behavior in which the animal moves randomly at a fairly slow
//  speed.  This activity is usually pretty weak.
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

		CActivityMoveHintWander
		(
		);
		
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
class CActivityMoveCommandStalk : public CActivity
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

		CActivityMoveCommandStalk
		(
		);
	
	//*****************************************************************************************
	//
	//	Overrides.
	//

		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


//*********************************************************************************************
//
class CActivityMoveCommandCircle : public CActivityDistance
//
//*********************************************************************************************
{

//
//  Member function definitions.
//

public:
	TReal		rNearDistance;	// How far to the closest approach point?
	CVector3<>	v3Start;		// Where were we when we started?
	CRating		rtSpeed;		// How fast do we go?

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityMoveCommandCircle
		(
		);
	
	//*****************************************************************************************
	//
	//	Overrides.
	//

		//*********************************************************************************
		virtual CRating rtRate(	const CFeeling&	feel,CInfluence*		pinf) override;

		//*********************************************************************************
		virtual void Act(CRating			rt_importance,	CInfluence*		pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


//*********************************************************************************************
//
class CActivityMoveCommandStayNear : public CActivity
//
//*********************************************************************************************
{
public:
	uint32	u4Target;		// Handle of pinsTarget.  Used to find pinsTarget if it isn't loaded yet.
	CInstance* pinsTarget;	// The thing that we want to stay near.
	TReal rTooFarAway;		// The distance which is considered too far away to be from the target.			
	TReal rCloseEnough;		// The distance which is considered to be comfortably close to the target.

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityMoveCommandStayNear
		(
			CInstance*	pinsTarget,
			TReal		rTooFarAway,
			TReal		rCloseEnough
		);
	
	//*****************************************************************************************
	//
	//	Overrides.
	//

		//*********************************************************************************
		virtual CRating rtRate(	const CFeeling&	feel,CInfluence*		pinf) override;

		//*********************************************************************************
		virtual void Act(CRating			rt_importance,	CInfluence*		pinf) override;

		//*****************************************************************************************
		virtual char * pcSave(char *  pc_buffer) const override;

		//*****************************************************************************************
		virtual const char * pcLoad(const char *  pc_buffer) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


//*********************************************************************************************
//
class CActivityMoveCommandStayAway : public CActivity
//
//*********************************************************************************************
{
public:
	uint32	u4Target;		// Handle of pinsTarget.  Used to find pinsTarget if it isn't loaded yet.
	CInstance* pinsTarget;	// The thing we want to stay away from.
	TReal rTooClose;		// The distance which is considered too close for comfort.
	TReal rFarEnough;		// The distance which is considered to be comfortably far from the target.

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityMoveCommandStayAway
		(
			CInstance*	pinsTarget,
			TReal		rTooClose,
			TReal		rFarEnough
		);
	
	//*****************************************************************************************
	//
	//	Overrides.
	//

		//*********************************************************************************
		virtual CRating rtRate(	const CFeeling&	feel,CInfluence*		pinf) override;

		//*********************************************************************************
		virtual void Act(CRating			rt_importance,	CInfluence*		pinf) override;

		//*****************************************************************************************
		virtual char * pcSave(char *  pc_buffer) const override;

		//*****************************************************************************************
		virtual const char * pcLoad(const char *  pc_buffer) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


//*********************************************************************************************
//
class CActivityPursue : public CActivityDistance
//
//	Prefix: amt
//
//	The CActivityPursue class is a behavior in which the animal moves quickly towards things.
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

		CActivityPursue
		(
		);

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


//*********************************************************************************************
//
class CActivityFlee : public CActivityDistance
//
//	Prefix: amt
//
//	The CActivityPursue class is a behavior in which the animal moves quickly towards things.
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

		CActivityFlee
		(
		);

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


//*********************************************************************************************
//
class CActivityJumpBack : public CActivity
//
//	Prefix: amt
//
//	Tne animal gets away from the target (want to be X distance away).
//	Very similar to StayAway, but not as powerful.
//
//*********************************************************************************************
{
public:

	TReal   rSafeDistance;
	TReal	rUnsafeDistance;
	TReal	rCinematicAngle;

//
//  Member function definitions.
//

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityJumpBack
		(
		);

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


//*********************************************************************************************
//
class CActivityDash : public CActivity
//
//	Prefix: 
//
//	Tne animal leaves an influence that it finds itself inside.
//
//*********************************************************************************************
{
public:

//
//  Member function definitions.
//
	CVector3<> v3Target;		// Where are we dashing to?


	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityDash
		(
		);

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityApproach : public CActivityDistance
//
//	Prefix: 
//
//	Tne animal leaves an influence that it finds itself inside.
//
//*********************************************************************************************
{
public:
	TReal rCinematicAngle;

//
//  Member function definitions.
//

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityApproach
		(
		);

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityGetOut : public CActivity
//
//	Prefix: 
//
//	Tne animal leaves an influence that it finds itself inside.
//
//*********************************************************************************************
{
public:
	const CInfluence*	pinfInside;	// The influence we are trying to escape.

	CVector2<>			v2Direction;	// Direction to travel if going down a hill.
										// Use this when pinfInside is zero.
//
//  Member function definitions.
//

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityGetOut
		(
		);

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};


//*********************************************************************************************
//
class CActivityMoveBy : public CActivityDistance
//
//	Prefix: 
//
//	Tne animal runs past the influence.
//
//*********************************************************************************************
{
public:
		TReal		rNearDistance;	// How far to the closest approach point?
		CVector3<>	v3Start;		// Where were we when we started?

//
//  Member function definitions.
//

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityMoveBy();

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//*********************************************************************************************
//
class CActivityJump : public CActivityDistance
//
//	Prefix: 
//
//	Tne animal runs past the influence.
//
//*********************************************************************************************
{
public:

//
//  Member function definitions.
//

	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		CActivityJump();


		//*********************************************************************************
		CRating rtRateSingleInfluence
		(
			const CFeeling&	feel,
			const CInfluence*		pinf
		);
		//
		//	Rates an influence.  Used for rtRate.
		//
		//**************



	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance, CInfluence* pinf) override;

		//*************************************************************************************
		virtual CRating rtRate(const CFeeling&	feel, CInfluence* pinf) override;

		//*************************************************************************************
		virtual void Activate(bool b_active_state) override;

#if VER_TEST
		//*****************************************************************************************
		virtual int iGetDescription(char *buffer, int i_buffer_length) override;
#endif
};

//#ifndef HEADER_GAME_AI_MOVEACTIVITIES_HPP
#endif