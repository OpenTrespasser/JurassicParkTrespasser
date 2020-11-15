/**********************************************************************************************
 *
 * $Source::																				  $
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of FakeGame.hpp.
 *
 *	Bugs:
 *
 *	To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/FakePhysics.cpp                                              $
 * 
 * 14    6/08/98 9:45p Agrant
 * SetPos already sends message-  don't do it ourselves!
 * 
 * 13    98/04/09 13:12 Speter
 * Adjusted for CMessageMove::etType. 
 * 
 * 12    1/13/98 9:34p Agrant
 * Added necessary include
 * 
 * 11    97/09/28 3:09p Pkeet
 * Replace the use of 'CPresence3<>().v3Pos' with 'v3Pos().'
 * 
 * 10    2/12/97 7:31p Agrant
 * Sped up animal reaction time
 * 
 * 9     2/10/97 12:54p Agrant
 * Move messages
 * 
 * 8     2/04/97 2:10p Agrant
 * Updated for new query and message .hpp files
 * 
 * 7     1/30/97 2:37p Agrant
 * Now updates the rotation in addition to the translation.
 * 
 * 6     96/12/04 20:34 Speter
 * A big change: Updated for new CPresence3<>, and CInstance functions.
 * 
 * 5     11/14/96 4:49p Agrant
 * AI Test more in tune with the new object hierarchy
 * 
 * 4     11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 3     11/02/96 7:23p Agrant
 * Changed physcis request message structure
 * 
 * 2     11/01/96 3:47p Agrant
 * AI to fake physics now event driven.
 * 
 * 1     10/31/96 5:57p Agrant
 * initial revision
 *
 **********************************************************************************************/

#include "common.hpp"
#include "FakePhysics.hpp"

//#include "Lib/EntityDBase/MessageTypes.hpp"
#include "TestAnimal.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgMove.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgPhysicsReq.hpp"


// Time required to change velocity to desired velocity.
#define sTimeToTurn TSec(.3)

//**********************************************************************************************
//
//	Class CPhysicsInfo implementation.
//

	//******************************************************************************************
	void CTestPhysicsInfo::Move(TSec sec_elapsed_time, CInstance* pins) const 
	{
		CTest *ptest = dynamic_cast<CTest*>(pins);
		Assert(ptest);

		// Update position
		CVector2<> v2Location = (ptest->v2Velocity * sec_elapsed_time) + (CVector2<>)pins->v3Pos();

		// Now check for leaving the world

		// Get the world extent
		extern CVector2<>	v2WorldMax;
		extern CVector2<>	v2WorldMin;

#define TOROIDAL_WORLD
#undef FENCED_WORLD
#ifdef TOROIDAL_WORLD
		CVector2<> v2_extent = v2WorldMax - v2WorldMin;

		// Check X direction
		if (v2Location.tX > v2WorldMax.tX)
		{
			// Wrap.
			v2Location.tX	-= v2_extent.tX;
		}
		else if (v2Location.tX < v2WorldMin.tX)
		{
			// Wrap.
			v2Location.tX	+= v2_extent.tX;
		}
	
		// Check Y direction
		if (v2Location.tY > v2WorldMax.tY)
		{
			// Wrap.
			v2Location.tY	-= v2_extent.tY;
		}
		else if (v2Location.tY < v2WorldMin.tY)
		{
			// Wrap.
			v2Location.tY	+= v2_extent.tY;
		}
#endif

#ifdef FENCED_WORLD
		// Check X direction
		if (v2Location.tX > v2WorldMax.tX)
		{
			// Stay inside the fence
			v2Location.tX	= v2WorldMax.tX;
			// And bounce
			v2Velocity.tX	= - v2Velocity.tX;
			v2Front.tX		= - v2Front.tX;
		}
		else if (v2Location.tX < v2WorldMin.tX)
		{
			v2Location.tX	= v2WorldMin.tX;
			v2Velocity.tX	= - v2Velocity.tX;
			v2Front.tX		= - v2Front.tX;
		}
	
		// Check Y direction
		if (v2Location.tY > v2WorldMax.tY)
		{
			// Stay inside the fence
			v2Location.tY	= v2WorldMax.tY;
			// And bounce
			v2Velocity.tY	= - v2Velocity.tY;
			v2Front.tY		= - v2Front.tY;
		}
		else if (v2Location.tY < v2WorldMin.tY)
		{
			v2Location.tY	= v2WorldMin.tY;
			v2Velocity.tY	= - v2Velocity.tY;
			v2Front.tY		= - v2Front.tY;
		}
#endif
		//
		// Add a frictive force to stop motion.
		//
		
		// An arbitrary frictive deceleration.
		TReal r_deceleration	=	3.0;		// in m/s/s
		
		// Current speed of object.
		TReal r_speed		=	ptest->v2Velocity.tLen();

		// Reduce speed by deceleration over specified time.
		r_speed -= r_deceleration * sec_elapsed_time;
		
		// Cannot turn around from friction
		if (r_speed <= 0)
		{
			ptest->v2Velocity = CVector2<>(0,0);			
		}
		else
		{
			// Set velocity to adjusted value.
			ptest->v2Velocity.Normalise();
			ptest->v2Velocity	*=	r_speed;
		}

		// Finally, set the entity's location:
		pins->SetPos(CVector3<>(v2Location.tX, v2Location.tY, 0));
	}

	//******************************************************************************************
	void CTestPhysicsInfo::HandleMessage
	(
		const CMessagePhysicsReq& msgpr, CInstance* pins
	) const 
	{
		CTest *ptest = dynamic_cast<CTest*>(pins);
		ptest->v2Destination = msgpr.subMoveBody.dData;
	}



//**********************************************************************************************
//
//	Class CPhysicsAnimal implementation.
//

	//******************************************************************************************
	void CTestPhysicsInfoAnimal::Move(TSec sec_elapsed_time, CInstance *pins) const 
	{
		// Use default physics movement.
		CTestPhysicsInfo::Move(sec_elapsed_time,pins);

		CTest *ptest = dynamic_cast<CTest*>(pins);
		Assert(ptest);

		// Then correct velocity for desired destination.
		CVector2<> v2DesiredVelocity = ptest->v2Destination - (CVector2<>) pins->v3Pos();
		if (!v2DesiredVelocity.bIsZero())
		{
			// For now move at max speed.
			v2DesiredVelocity.Normalise(rGetMaxSpeed());
		}

		// All animals try to turn toward their destinations
		if (sec_elapsed_time < sTimeToTurn)
		{
			CVector2<>	v2Delta = v2DesiredVelocity - ptest->v2Velocity;
			v2Delta *= sec_elapsed_time / sTimeToTurn;
			ptest->v2Velocity += v2Delta;
		}
		else
			ptest->v2Velocity = v2DesiredVelocity;

		// For animals, set their rotations.  They face down positive Y.
		if (!ptest->v2Velocity.bIsZero())
			pins->pr3Presence().r3Rot = CRotate3<>(CDir3<>(0,1,0), (CVector3<>)ptest->v2Velocity);

		
		// v2Front no longer used!
//		if (v2Velocity.tX != 0 && v2Velocity.tY != 0)
		//{
//			v2Front		= v2Velocity;
//			v2Front.Normalise();
//		}
	}
