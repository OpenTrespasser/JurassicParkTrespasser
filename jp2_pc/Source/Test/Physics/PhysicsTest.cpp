/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of PhysicsTest.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Physics/PhysicsTest.cpp                                         $
 * 
 * 1     8/14/96 3:10p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "Lib/Sys/Textout.hpp"
#include "PhysicsTestShell.hpp"
#include "PhysicsTest.hpp"


//*********************************************************************************************
void Create()
{
}

//*********************************************************************************************
void Step()
{
}

//*********************************************************************************************
void Paint()
{
	// Test if the left mouse button is pressed and if the mouse is over the window rectangle.
	if (bLeftMouseButton())
	{
		float x = 0.0f;
		float y = 0.0f;

		// Get the position of the mouse cursor.
		GetMousePosition(x, y);

		// Set the line colour.
		LineColour(255, 255, 0);

		// Draw the line.
		Line(0.0f, 0.0f, x, y);
	}
}

//*********************************************************************************************
void Destroy()
{
}