/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The physics test shell.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Physics/PhysicsTestShell.hpp                                     $
 * 
 * 1     8/14/96 3:11p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_PHYSICS_PHYSICSTESTSHELL_HPP
#define HEADER_TEST_PHYSICS_PHYSICSTESTSHELL_HPP


//
// Function prototypes.
//

//*********************************************************************************************
//
void LineColour
(
	int i_red,
	int i_green,
	int i_blue
);
//
// Sets the RGB line colour for GDI line drawing. Colour values are between 0 and 255.
//
//**************************************

//*********************************************************************************************
//
void Line
(
	float f_x0,	// First coordinate pair
	float f_y0,
	float f_x1,	// Second coordinate pair.
	float f_y1
);
//
// Draws a line from the first pair of coordinates to the last pair of coordinates. The lesser
// dimension of width and height represents a value between 0.0 and 1.0.
//
//**************************************

//*********************************************************************************************
//
void GetMousePosition
(
	float& f_x,	// Position of the mouse in virtual coordinates.
	float& f_y
);
//
// Get the position of the mouse cursor.
//
// Returns 'true' if the mouse is over the main window.
//
//**************************************

//*********************************************************************************************
//
bool bLeftMouseButton
(
	bool b_is_in_window = true,	// If 'true', the mouse cursor must be over a portion of the
								// main window before the button press will be acknowledged.
	bool b_is_in_rect = true	// If 'true', the mouse cursor must be over the window
								// rectangle before the button press will be acknowledged.
);
//
// Returns 'true' if the left mouse button is pressed.
//
//**************************************


#endif
