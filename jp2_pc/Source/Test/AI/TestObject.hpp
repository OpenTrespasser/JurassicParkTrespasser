/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CTestObject class for AI test app.  An object draws itself, locates itself, 
 *		moves itself, and orients itself.
 *		
 *
 * Bugs:
 *
 *
 * To do:
 *		Add facing information.
 *		Improve world boundary checking in Move()- right now it uses the viewport size and
 *			blatantly breaks abstraction
 *	
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/TestObject.hpp                                               $
 * 
 * 10    10/31/96 7:26p Agrant
 * changed over to world database as seen in the Game.  Yahoo.
 * 
 * 9     10/23/96 7:39p Agrant
 * more debugging
 * now using the dialog box for printing some text
 * first pass pathfinder
 * 
 * 8     10/14/96 12:21p Agrant
 * Added walls to test app.
 * 
 * 7     9/26/96 5:59p Agrant
 * AI system basic structure implemented
 * AI Test app has basic herding behavior based on two simple activities
 * TReal used as basic world coordinate type.
 * 
 * 6     9/23/96 2:52p Agrant
 * Changed v2Position to v2Location
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
 * 2     8/27/96 4:48p Agrant
 * added type declaration TObjectList
 * 
 * 1     8/21/96 4:55p Agrant
 * initial revision
 * temporary objects for test app
 *
 ********************************************************************************************/

#ifndef HEADER_TEST_AI_TESTOBJECT_HPP
#define HEADER_TEST_AI_TESTOBJECT_HPP

#error TestObject.hpp no longer used.

#ifdef GOOFY_BOY

#include <vector.h>			// for STL list class

#include "Lib/Transform/Vector.hpp"
#include "Lib/EntityDBase/Entity.hpp"

#include "Game/AI/Classes.hpp"

#include "TestTypes.hpp"

//*********************************************************************************************
//
class CTestObject : public CEntity
//
//	Prefix: cto
//
//	The CTestObject class is placeholder for the object system in the final game.
//
//*********************************************************************************************
{
	//
	// Variable declarations.
	//

public:

	CVector2<>		v2Location;		// Location of object (meters).
	CVector2<>		v2Velocity;		// Velocity of object (m/sec).
	CVector2<>		v2Front;		// A vector pointing forward from the object's center,
									// used as a facing reference.  Must be normalised.
	bool			bSelected;		// TRUE if object has been selected, else FALSE

	ETestObjType	etotType;		// Type of object

	CBrain *		pbrBrain;		// Object's brain, if any.

//	CVector2<>		v2DesiredVelocity;
									// Object's desired velocity, if animate.

	CSArray<char,64>	sacDebugString;	// Holds debug info to be displayed with object.
	


	//
	// Member function definitions.
	//

public:
	//****************************************************************************************
	//
	//	Constructors and destructor
	//

	CTestObject
	(
	);
	
	~CTestObject
	(
	);

	//****************************************************************************************
	//
	//	Operator functions
	//

	bool operator ==
	(
		const CTestObject &cto
	)
	{
		// Objects are only identical if they are the same piece of memory
		// This ought to work since they are always passed by reference
		// except when created and placed in the object list.
		return this == &cto;
	}


	//****************************************************************************************
	//
	//	Member functions
	//

		//*********************************************************************************
		//
		virtual void Draw
		(
		);
		//
		//	Draws the object in current pen color at current location.
		//
		//	Returns:
		//		void
		//
		//	Notes:
		//
		//
		//	Side effects:
		//		modifies window bitmap
		//
		//	Runtime requirements:
		//
		//
		//	Globals:
		//
		//
		//	Cross references:
		//
		//
		//	Example:
		//
		//
		//******************************



		//*********************************************************************************
		//
		virtual void Move
		(
			TSeconds secElapsedTime
		);
		//
		//	Informs the object that the specified amount of time has passed since its last
		//	update.  The object updates itself as necessary, including movement according 
		//	to its velocity.
		//
		//	Side effects:
		//		modifies tvLocation
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
		virtual void ToggleSelect
		(
		)
		//
		//	Toggles the selection status of the TestObject (selected, unselected).
		//
		//	Side effects:
		//		modifies "bSelected"
		//
		//******************************
		{
			bSelected = !bSelected;
		}

		//******************************************************************************************
		//
		void DrawDebugString
		(
		);
		//
		//	Draws the current debug string assosiated with the object. 
		//
		//******************************

		//******************************************************************************************
		//
		void AddDebug
		(
			char c
		)
		//
		//	Adds the character to the debug string.
		//
		//******************************
		{
			sacDebugString << c;
		}


		

};

#endif
//  ends #ifndef HEADER_TEST_AI_TESTOBJECT_HPP
#endif   