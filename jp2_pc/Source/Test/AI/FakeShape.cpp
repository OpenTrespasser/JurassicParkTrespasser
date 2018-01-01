/**********************************************************************************************
 *
 * $Source::																				  $
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of FakeShape.hpp.
 *
 *	Bugs:
 *
 *	To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/FakeShape.cpp                                                $
 * 
 * 10    8/23/98 3:03a Agrant
 * AI Test compile fixes
 * 
 * 9     97/09/28 3:09p Pkeet
 * Replace the use of 'CPresence3<>().v3Pos' with 'v3Pos().'
 * 
 * 8     4/08/97 6:42p Agrant
 * Debugging improvements for AI Test app.
 * Allows test app treatment of GUIApp AI's.
 * Better tools for seeing what is going on in an AI's brain.
 * 
 * 7     2/05/97 5:15p Agrant
 * Walls now look like walls again....
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
 * Added grass and meat
 * 
 * 2     10/31/96 7:22p Agrant
 * Added specific shape info types.
 * 
 * 1     10/31/96 5:58p Agrant
 * initial revision
 *
 **********************************************************************************************/

#include "Common.hpp"
#include "FakeShape.hpp"
#include "FakePhysics.hpp"

#include "AITestShell.hpp"
#include "TestAnimal.hpp"
#include "TestTree.hpp"
 

inline void ColorMeSelected(CInstance* pins)
{
	// Overrrides the color to bright red if pins is currently selected.
	if (gaiSystem.bIsSelected(pins))
		LineColour(255,0,0);
}

inline CVector2<> v2GetFront(CInstance* pins)
{
	// Returns a vector pointing forward from the object.
	return dynamic_cast<CTest*>(pins)->v2Velocity;
//	return CVector2<>(0,0);
}


rptr<CShapeTree> rpsShapeTree = rptr_new CShapeTree();
rptr<CShapeWall> rpsShapeWall	= rptr_new CShapeWall();
rptr<CShapeSheep> rpsShapeSheep	= rptr_new CShapeSheep();
rptr<CShapeDinosaur> rpsShapeDinosaur	= rptr_new CShapeDinosaur();
rptr<CShapeWolf> rpsShapeWolf	= rptr_new CShapeWolf();



//**********************************************************************************************
//
//	Class CShapeInfo implementation.
//

	//******************************************************************************************
	void CTestShapeInfo::Draw(CInstance* pins)  const
	{
		// A cross
		
		// Set color.
		LineColour(0, 255, 0);
		ColorMeSelected(pins);

		CVector2<> v2Location = pins->v3Pos();

		// The cross.
		CVector2<>	v2_temp(1,0);
		Line(v2Location - v2_temp,
			 v2Location + v2_temp);

		v2_temp = CVector2<>(0,1);
		Line(v2Location - v2_temp,
			 v2Location + v2_temp);
	}



//**********************************************************************************************
//
//	Class CShapeTree implementation.
//

	//******************************************************************************************
	void CShapeTree::Draw(CInstance* pins) const
	{
		// An eight barred cross
		
		// Set color based on selection status
		LineColour(0, 255, 0);
		ColorMeSelected(pins);


		CVector2<> v2Location = pins->v3Pos();

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
//	Class CShapeWall implementation.
//

	//******************************************************************************************
	void CShapeWall::Draw(CInstance* pins) const
	{
		// A line
		
		// Set color based on selection status
		LineColour(64, 255, 255);
		ColorMeSelected(pins);

		CVector2<> v2Location	= pins->v3Pos();
		CVector2<> v2_end		= ((CTestWall*)pins)->v2SecondPoint;


		// The line.
		Line(v2Location, v2_end);

		// Then a little jag to the left.
		CVector2<> v2_jag = v2_end - v2Location;
		CVector2<> v2_half = v2_jag / 2 + v2Location;

		v2_jag = CVector2<>(-v2_jag.tY, v2_jag.tX);
		v2_jag /= 16;


		// Color the jag differently, then draw.
		LineColour(128,128,255);
		ColorMeSelected(pins);
		Line(v2_half, v2_half + v2_jag);
	}


//**********************************************************************************************
//
//	Class CShapeDinosaur implementation.
//

	//******************************************************************************************
	void CShapeDinosaur::Draw(CInstance* pins) const
	{
		// A 2 meter long arrow, pointing in the direction of facing.
		
		// Set color based on selection status
		LineColour(55, 200, 255);
		ColorMeSelected(pins);

		// Use a temporary vector to save some divides
		CVector2<>	v2_temp		= v2GetFront(pins);
		CVector2<>  v2Location	= pins->v3Pos();

		if (v2_temp.bIsZero())
			v2_temp.tX = 1.0;
		v2_temp.Normalise();

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
//	Class CShapeWolf implementation.
//

	//******************************************************************************************
	void CShapeWolf::Draw(CInstance* pins) const
	{
		// A "W" with a bar on top.

		// Set color based on selection status
		LineColour(0, 255, 255);
		ColorMeSelected(pins);


		CVector2<> v2_front = v2GetFront(pins);

		if (v2_front.tX == 0.0 && v2_front.tY == 0.0)
			v2_front.tX = 1.0;
		v2_front.Normalise(.5);

		CVector2<>	v2_side	= CVector2<>(v2_front.tY, -v2_front.tX);
		CVector2<>	v2Location = pins->v3Pos();
		
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


	
//**********************************************************************************************
//
//	Class CShapeSheep implementation.
//

	//******************************************************************************************
	void CShapeSheep::Draw(CInstance* pins) const
	{
		// A triangle
		
		// Set color based on selection status
		LineColour(255, 255, 255);
		ColorMeSelected(pins);

		CVector2<> v2Location = pins->v3Pos();

		CVector2<> v2_front = v2GetFront(pins);
		if (v2_front.tX == 0.0 && v2_front.tY == 0.0)
			v2_front.tX = 1.0;
		v2_front.Normalise(.5);

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

//**********************************************************************************************
//
//	Class CShapeMeat implementation.
//

	//******************************************************************************************
	void CShapeMeat::Draw(CInstance* pins) const
	{
		// A square.
		
		// Set color based on selection status
		LineColour(255, 128, 128);
		ColorMeSelected(pins);

		CVector2<> v2Location	= pins->v3Pos();

		CVector2<> v2_corner(.4, .4);
		CVector2<> v2_corner_perp(.4, -.4);

		Line(v2Location + v2_corner, v2Location + v2_corner_perp);
		Line(v2Location + v2_corner, v2Location - v2_corner_perp);
		Line(v2Location - v2_corner, v2Location + v2_corner_perp);
		Line(v2Location - v2_corner, v2Location - v2_corner_perp);
	
	}

//**********************************************************************************************
//
//	Class CShapeGrass implementation.
//

	//******************************************************************************************
	void CShapeGrass::Draw(CInstance* pins) const
	{
		// A square.
		
		// Set color based on selection status
		LineColour(8, 255, 32);
		ColorMeSelected(pins);

		CVector2<> v2Location	= pins->v3Pos();

		CVector2<> v2_corner(.4, .4);
		CVector2<> v2_corner2(-.4, .8);

		Line(v2Location + v2_corner, v2Location);
		Line(v2Location + v2_corner2, v2Location);
	}
