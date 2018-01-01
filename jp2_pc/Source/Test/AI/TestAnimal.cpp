/**********************************************************************************************
 *
 * $Source::																				  $
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of all Test Object child classes.
 *		TestAnimal.hpp
 *			Dinosaurs
 *			Wolves
 *			Sheep
 *		TestTree.hpp
 *			Trees
 *			Walls
 *
 *	Bugs:
 *
 *	To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/AI/TestAnimal.cpp                                               $
 * 
 * 11    5/10/98 6:20p Agrant
 * allow loading scenes
 * 
 * 10    11/02/96 7:25p Agrant
 * Removed dead code
 * Added animal instance counter
 * 
 * 9     10/31/96 7:26p Agrant
 * changed over to world database as seen in the Game.  Yahoo.
 * 
 * 8     10/23/96 7:39p Agrant
 * more debugging
 * now using the dialog box for printing some text
 * first pass pathfinder
 * 
 * 7     10/14/96 12:21p Agrant
 * Added walls to test app.
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
 * 4     9/27/96 5:44p Agrant
 * Added Trees
 * Added some notion of acceleration to dino model.
 * 
 * 3     9/26/96 5:59p Agrant
 * AI system basic structure implemented
 * AI Test app has basic herding behavior based on two simple activities
 * TReal used as basic world coordinate type.
 * 
 * 2     9/23/96 2:54p Agrant
 * Variable name changes for uniformity.
 * Conform! Conform!
 * 
 * 1     9/03/96 8:07p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "TestAnimal.hpp"

#include "Game\AI\Classes.hpp"
#include "Game\AI\Brain.hpp"

#include "AITestShell.hpp"
#include "TestTree.hpp"


// Start with no animals.
int CTestAnimal::iNumAnimals = 0;
int CTestThing::i_num_test_objects = 0;


#ifdef CRUSTY_OLD_CODE
		//******************************************************************************************
	void CTestDinosaur::Draw()
	{
		// A 2 meter long arrow, pointing in the direction of facing.

		bool bSelected = false;

		// Set color based on selection status
		if (bSelected)
			LineColour(255, 0, 0);
		else
			LineColour(55, 200, 255);

		CVector2<> v2Location(1,1);
		CVector2<> v2Front(1,1);
		Assert(0);

		// Use a temporary vector to save some divides
		CVector2<>	v2_temp = v2Front;

		// Body.
		Line(v2Location - v2_temp,
			 v2Location + v2_temp);

		// Head is 1/4 length of body
		CVector2<>	v2_head = v2_temp / 2;

		// Find head position
		v2_temp = v2Location + v2_head;

		// Rotate head 90 degrees
		v2_head = CVector2<>(-v2_head.tY, v2_head.tX);

		// Head.
		Line(v2_temp + v2_head,
		     v2_temp - v2_head);
	}


//**********************************************************************************************
//
//	Class CTestTree implementation.
//

	//******************************************************************************************
	void CTestTree::Draw()
	{
		// An eight barred cross
		
		// Set color based on selection status
//		if (bSelected)
			//LineColour(255, 0, 0);
		//else
			LineColour(0, 255, 0);
			Assert(0);


		CVector2<> v2Location(1,1);
		Assert(0);

		// The cross.
		CVector2<>	v2_temp(1,0);
		Line(v2Location - v2_temp,
			 v2Location + v2_temp);

		v2_temp = CVector2<>(0,1);
		Line(v2Location - v2_temp,
			 v2Location + v2_temp);

		v2_temp = CVector2<>(.7,.7);
		Line(v2Location - v2_temp,
			 v2Location + v2_temp);

		v2_temp = CVector2<>(.7,-.7);
		Line(v2Location - v2_temp,
			 v2Location + v2_temp);
	}


//**********************************************************************************************
//
//	Class CTestWall implementation.
//
	//******************************************************************************************
	void CTestWall::Draw()
	{
		bool bSelected = false;
		Assert(0);

		// Set color based on selection status
		if (bSelected)
			LineColour(255, 0, 0);
		else
			LineColour(0, 0, 255);

		CVector2<> v2Location(1,1);
		Assert(0);
		CVector2<> v2Front(1,1);
		Assert(0);
		// The wall itself.
		Line(v2Location, v2SecondPoint);

		// An indicator at the hotspot.

		// Calculate a facing vector.
		CVector2<> v2_perpendicular		= v2SecondPoint - v2Location;

		// If the wall is long enough, we need a hot spot indicator on the base location point.
		if (v2_perpendicular.tLenSqr() > 4.0)
		{
			// Make it actually perpendicular.
			v2_perpendicular = CVector2<>(v2_perpendicular.tY, - v2_perpendicular.tX);

			// Normalise so it's one meter long.
			v2_perpendicular.Normalise();

			// Draw.
			Line(v2Location, v2Location + v2_perpendicular);
		}
	}


	//******************************************************************************************
	void CTestWolf::Draw()
	{
		// A "W" with a bar on top.
		

		bool bSelected = false;
		Assert(0);

		// Set color based on selection status
		if (bSelected)
			LineColour(255, 0, 0);
		else
			LineColour(0, 255, 255);

		CVector2<> v2Location(1,1);
		Assert(0);
		CVector2<> v2Front(1,1);
		Assert(0);

		CVector2<> v2_front = v2Front / 2;
		CVector2<> v2_side	= CVector2<>(v2_front.tY, -v2_front.tX);
		


		// Draw the bar
		CVector2<> v2_a		= v2Location - v2_front - v2_side;
		CVector2<> v2_b		= v2Location - v2_front + v2_side;
		Line(v2_a, v2_b);

		// Draw the "W"
		v2_side /= 2;
		CVector2<> v2_c		= v2Location + v2_front - v2_side;
		CVector2<> v2_d		= v2Location + v2_front + v2_side;

		Line(v2Location, v2_c);
		Line(v2Location, v2_d);
		Line(v2_a, v2_c);
		Line(v2_b, v2_d);
	}


	//******************************************************************************************
	void CTestSheep::Draw()
	{
		// A triangle
		bool bSelected = false;
		Assert(0);
		
		// Set color based on selection status
		if (bSelected)
			LineColour(255, 0, 0);
		else
			LineColour(255, 255, 255);

		CVector2<> v2Location(1,1);
		Assert(0);
		CVector2<> v2Front(1,1);
		Assert(0);

		CVector2<> v2_front = v2Front / 2;
		CVector2<> v2_side	= CVector2<>(v2_front.tY, -v2_front.tX) / 2;

		// The base.
		CVector2<> v2_a		= v2Location - v2_front - v2_side;
		CVector2<> v2_b		= v2Location - v2_front + v2_side;
		Line(v2_a, v2_b);

		// The sides.
		CVector2<> v2_nose	= v2Location + v2_front;
		Line(v2_a, v2_nose);
		Line(v2_b, v2_nose);
	}
#endif