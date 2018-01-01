/**********************************************************************************************
 *
 * $Source::																				  $
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of TestObject.hpp.
 *
 *	Bugs:
 *
 *	To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/TestObject.cpp                                               $
 * 
 * 13    10/23/96 7:39p Agrant
 * more debugging
 * now using the dialog box for printing some text
 * first pass pathfinder
 * 
 * 12    10/15/96 9:18p Agrant
 * minor housekeeping for synthesizer rework, and associated changes
 * 
 * 11    10/10/96 7:18p Agrant
 * Modified for code spec.
 * 
 * 10    10/02/96 8:10p Agrant
 * Added Sheep and Wolves.
 * Added Influence Debugging.
 * Added AI frame rate counter.
 * Unified all meter/pixel conversions.
 * 
 * 9     9/27/96 5:44p Agrant
 * Added Trees
 * Added some notion of acceleration to dino model.
 * 
 * 8     9/26/96 5:59p Agrant
 * AI system basic structure implemented
 * AI Test app has basic herding behavior based on two simple activities
 * TReal used as basic world coordinate type.
 * 
 * 7     9/23/96 2:52p Agrant
 * Changed v2Position to v2Location to match other variables in the AI Test App.
 * 
 * 6     9/19/96 1:21p Agrant
 * Add friction to physics model.
 * 
 * 5     9/03/96 7:50p Agrant
 * Added object types.
 * 
 * 4     8/30/96 4:52p Agrant
 * objects now have facing and velocity
 * 
 * 3     8/29/96 5:31p Agrant
 * Major revision
 * Mostly changed code to match coding spec
 * Turned the object list into a class
 * Incorporated a timer
 * Now using TDefReal instead of float
 * 
 * 2     8/27/96 4:43p Agrant
 * Actual first implementation
 * 
 * 1     8/21/96 7:22p Agrant
 * initial revision	cribbed from PhysicsTest
 *
 **********************************************************************************************/

#include "Common.hpp"
#include "TestObject.hpp"

#include <list.h>

#include "AITestShell.hpp"


//**********************************************************************************************
//
//	Class CTestObject implementation.
//

	//****************************************************************************************
	CTestObject::CTestObject() :
		v2Location(0,0), 
		v2Velocity(0,0), 
		v2Front(1,0),
		bSelected(FALSE),
		etotType(etotSIMPLE)
//		v2DesiredVelocity(0,0)
	{
		// Make sure that the front vector is normalised.
		v2Front.Normalise();
	}
	
	//****************************************************************************************
	CTestObject::~CTestObject()
	{
	}
	
	//******************************************************************************************
	void CTestObject::Draw()
	{
		sacDebugString << '\0';
		Text(v2Location.tX + 1, v2Location.tY - 1, sacDebugString);

		DrawDebugString();
	}

	//******************************************************************************************
	void CTestObject::DrawDebugString()
	{
		if (sacDebugString.uLen < 1)
			return;
		if (sacDebugString[sacDebugString.uLen - 1] != '\0')
			sacDebugString << '\0';
		Text(v2Location.tX + 1, v2Location.tY - 1, sacDebugString);
	}

	//******************************************************************************************
	void CTestObject::Move(TSeconds sec_elapsed_time)
	{
		// Update position
		v2Location += v2Velocity * sec_elapsed_time;

		// Now check for leaving the world

		// Get the world extent
		extern CVector2<>	v2WorldMax;
		extern CVector2<>	v2WorldMin;

#define TOROIDAL_WORLD
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
		TReal r_speed		=	v2Velocity.tLen();

		// Reduce speed by deceleration over specified time.
		r_speed -= r_deceleration * sec_elapsed_time;
		
		// Cannot turn around from friction
		if (r_speed <= 0)
		{
			v2Velocity = CVector2<>(0,0);			
		}
		else
		{
			// Set velocity to adjusted value.
			v2Velocity.Normalise();
			v2Velocity	*=	r_speed;
		}

	}
