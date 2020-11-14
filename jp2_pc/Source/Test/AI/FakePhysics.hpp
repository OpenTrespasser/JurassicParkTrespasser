/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Fake information classes for
 *			CEntityShape
 *			CEntityPhysics
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/AI/FakePhysics.hpp                                               $
 * 
 * 11    97/10/06 11:01 Speter
 * Updated for new CBoundVolBox interface.
 * 
 * 10    9/09/97 8:52p Agrant
 * allow fake physics objects to immoveable
 * 
 * 9     7/31/97 4:37p Agrant
 * Fake physics objects are tangible
 * 
 * 8     1/30/97 2:36p Agrant
 * Added dimension to the test object constructor
 * 
 * 7     12/10/96 8:40p Agrant
 * make sure fake physics objects have valid bounding volumes.
 * 
 * 6     12/10/96 7:41p Agrant
 * Fixing AI Test App for code rot.
 * World DBase changes, mostly.
 * 
 * 5     96/12/04 20:34 Speter
 * A big change: Updated for new CPresence3<>, and CInstance functions.
 * 
 * 4     11/14/96 4:49p Agrant
 * AI Test more in tune with the new object hierarchy
 * 
 * 3     11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 2     11/01/96 3:47p Agrant
 * AI to fake physics now event driven.
 * 
 * 1     10/31/96 7:25p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_AI_FAKEPHYSICS_HPP
#define HEADER_TEST_AI_FAKEPHYSICS_HPP

#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/Physics/InfoBox.hpp"

#include "Lib/Sys/Timer.hpp"

#include "Game/AI/Rating.hpp"


//**********************************************************************************************
//
class CTestPhysicsInfo : public CPhysicsInfoBox
//
// Prefix: phi
//
// Fake class holding physics info.
//
//	Notes:
//		This class takes the place of the real game CPhyscisInfo, but only in the 
//		AI Test App.
//
//**************************************
{
public:
//	CInstance*			pinsInstance;	// The entity associated with this physics model.
	
//	CVector2<>			v2Velocity;		// The current velocity of the entity.

//	CVector2<>			v2Destination;
										// The current movement target, if "petEntity" is an animal.

//	CRating				rtDestinationSpeed;
										// How quickly ought we head to the destination point?
	
//	CRating				rtDestinationUrgency;
										// How important is it to get to the destination?

	//******************************************************************************************
	//
	// Constructors.
	//

	CTestPhysicsInfo
	(
		TReal r_xdim = 1,
		TReal r_ydim = 1,
		TReal r_zdim = 1,
		bool b_movable = true
		) :
			CPhysicsInfoBox
			(
				CBoundVolBox(r_xdim, r_ydim, r_zdim),
				SPhysicsData(b_movable, true)
			) 
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

		//*********************************************************************************
		//
		virtual void Move
		(
			TSec secElapsedTime,
			CInstance*	pins
		) const;
		//
		//	Moves pins according to the elapsed time.
		//
		//	Side effects:
		//		modifies pins location
		//
		//	Globals:
		//		References world extent variables
		//
		//	Example:
		//		Move(.25);		// moves the object as if a quarter second
		//						// had elapsed
		//
		//******************************

		
		//*********************************************************************************
		//
		virtual TReal rGetMaxSpeed
		(
		) const
		//
		//	Returns the max speed in meters/second
		//
		//******************************
		{
			return 10.0;
		}


		//******************************************************************************************
		virtual void HandleMessage
		(
			const CMessagePhysicsReq& msgpr,
			CInstance *pins
		) const;
};



//**********************************************************************************************
//
class CTestPhysicsInfoAnimal : public CTestPhysicsInfo
//
// Prefix: phia
//
// Fake class holding animal physics info.
//
//	Notes:
//		This class takes the place of the real game CPhyscisInfo, but only in the 
//		AI Test App.
//
//**************************************
{
public:
	
	//******************************************************************************************
	//
	// Constructors.
	//

	CTestPhysicsInfoAnimal
	(

	)
	{
	}
	//******************************************************************************************
	//
	// Overrides.
	//

		//*********************************************************************************
		//
		virtual void Move
		(
			TSec secElapsedTime,
			CInstance*	pins
		) const;
		//
		//	Adds intentionality to animal movement.
		//
		//******************************
};


//#ifndef HEADER_TEST_AI_FAKEPHYSICS_HPP
#endif
