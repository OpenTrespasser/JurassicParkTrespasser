/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The AI test shell.  The shell contains utilities & tools &
 *		basic building blocks not entirely specific to AI test.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/AI/AITestShell.hpp                                               $
 * 
 * 10    5/17/98 8:03p Agrant
 * numerous AI test app interface improvements
 * 
 * 9     9/11/97 1:49p Agrant
 * Added SetMenuCheck function
 * 
 * 8     1/23/97 2:23p Agrant
 * Added step mode to AI test app.
 * 
 * 7     10/23/96 7:39p Agrant
 * more debugging
 * now using the dialog box for printing some text
 * first pass pathfinder
 * 
 * 6     10/02/96 8:10p Agrant
 * Added Sheep and Wolves.
 * Added Influence Debugging.
 * Added AI frame rate counter.
 * Unified all meter/pixel conversions.
 * 
 * 5     9/26/96 5:59p Agrant
 * AI system basic structure implemented
 * AI Test app has basic herding behavior based on two simple activities
 * TReal used as basic world coordinate type.
 * 
 * 4     8/30/96 4:51p Agrant
 * objects now have facing and velocity
 * 
 * 3     8/29/96 5:31p Agrant
 * Major revision
 * Mostly changed code to match coding spec
 * Turned the object list into a class
 * Incorporated a timer
 * Now using TDefReal instead of float
 * 
 * 2     8/27/96 4:40p Agrant
 * Altered to reference screen space in term of world coordinates (meters) rather than relative
 * screen coordinates.
 * 
 * 1     8/21/96 3:47p Agrant
 * AI test bed initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_AI_AITESTSHELL_HPP
#define HEADER_TEST_AI_AITESTSHELL_HPP

#include <vector.h>

#include "Lib/Transform/Vector.hpp"

class CFeeling;


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
//	Sets the RGB line colour for GDI line drawing. Colour values are between 0 and 255.
//
//	Bugs:
//		This ought to use CColour, perhaps.
//
//**************************************

//*********************************************************************************************
//
void Line
(
	TReal r_x0,	// First coordinate pair
	TReal r_y0,
	TReal r_x1,	// Second coordinate pair.
	TReal r_y1
);
//
// Draws a line from the first pair of coordinates to the last pair of coordinates. The coordinates
//  are in meters from the lower left of the world.
//
//**************************************

//*********************************************************************************************
//
void Line
(
	CVector2<>	v2_from,	// First coordinate pair
	CVector2<>	v2_to	// Second coordinate pair
);
//
// Draws a line from the first pair of coordinates to the last pair of coordinates. The coordinates
//  are in meters from the lower left of the world.
//
//**************************************

//*********************************************************************************************
//
void Text
(
	TReal r_x,
	TReal r_y,
	char *pc_buffer
);
//
// Draws the given text at location r_x, r_y.  Coords are in meters from the lower left of the world.
//
//**************************************


//*********************************************************************************************
//
void GetMousePosition
(
	TReal& rr_x,	// Position of the mouse in virtual coordinates.
	TReal& rr_y
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
// Returns 'true' if the left mouse button is currently held down.
//
//**************************************

//*********************************************************************************************
//
void SetViewScale
( 
	TReal r_pixels_per_meter
);
//
// Sets the scale of the viewport relative to the world.
//
//**************************************

//*********************************************************************************************
//
void SetViewCenter
( 
	TReal r_x,
	TReal r_y
);
//
// Sets the center of the viewport in world coords
//
//**************************************

//*********************************************************************************
//
void DrawFeeling
(
	const CFeeling		&feel,
	const CVector2<>	&v2_upper_left
);
//
//	Draws the feeling at the specified location.
//
//******************************

//*********************************************************************************************
//
bool IsMenuItemChecked
(
	uint u4_id	
);
//
//  Returns:
//		true if menu item is checked, else false.
//		Menu item is specified by ID.
//
//**************************************

//*********************************************************************************************
//
void ToggleMenuCheck
(
	uint u4_id	
);

void SetMenuCheck
(
	uint u4_id,
	bool b_checked
);

// True when a step ought to occur
extern bool bStepRequested;

// Ends #ifndef HEADER_TEST_AI_AITESTSHELL_HPP
#endif

