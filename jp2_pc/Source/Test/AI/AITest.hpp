/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		AI test code.  Specific to AI.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/AITest.hpp                                                    $
 * 
 * 28    9/20/98 7:33p Agrant
 * make AI Test selection 2d
 * 
 * 27    98.09.12 12:18a Mmouni
 * Changed shape query to render type query.
 * 
 * 26    97/09/28 3:09p Pkeet
 * Replace the use of 'CPresence3<>().v3Pos' with 'v3Pos().'
 * 
 * 25    4/30/97 9:17p Rwyatt
 * Moved esfView member variable from CPartition and put it into the list that the partition
 * functions return. These functions used to return a list of CPartition* now they return a list
 * of structures that contain a CPartition* and esfSideOf.
 * 
 * 24    3/15/97 5:05p Agrant
 * Updated for spatial partition changes
 * 
 * 23    97/03/04 5:12p Pkeet
 * Changed query interface to use partitions instead of discrete presences and bounding volumes.
 * 
 * 22    2/10/97 12:55p Agrant
 * added some consts
 * 
 * 21    2/05/97 1:05p Agrant
 * Added a graph test mode for testing A* graph searches.
 * 
 * 20    2/04/97 2:10p Agrant
 * Updated for new query and message .hpp files
 * 
 * 19    1/30/97 2:35p Agrant
 * Added a silhouette drawign routine.
 * 
 * 18    1/23/97 2:23p Agrant
 * Added step mode to AI test app.
 * 
 * 17    1/07/97 6:50p Agrant
 * rptr's and other random structure changes integrated into test app.
 * 
 * 16    12/10/96 7:41p Agrant
 * Fixing AI Test App for code rot.
 * World DBase changes, mostly.
 * 
 * 15    96/12/04 20:34 Speter
 * A big change: Updated for new CPresence3<>, and CInstance functions.
 * 
 * 14    11/20/96 1:24p Agrant
 * CAISystem is now a CSubsystem, and processes step messages.
 * 
 * 13    11/17/96 5:25p Agrant
 * Fixed for new query class API.
 * 
 * 12    11/14/96 4:49p Agrant
 * AI Test more in tune with the new object hierarchy
 * 
 * 11    11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 10    11/02/96 7:15p Agrant
 * added a way to find the nearest object to a particular point in the worldDB
 * 
 * 9     10/31/96 7:26p Agrant
 * changed over to world database as seen in the Game.  Yahoo.
 * 
 * 8     9/27/96 5:44p Agrant
 * Added Trees
 * Added some notion of acceleration to dino model.
 * 
 * 7     9/26/96 5:59p Agrant
 * AI system basic structure implemented
 * AI Test app has basic herding behavior based on two simple activities
 * TReal used as basic world coordinate type.
 * 
 * 6     9/03/96 7:52p Agrant
 * Added a function for the AI system to use to do its stuff.
 * 
 * 5     8/30/96 4:51p Agrant
 * objects now have facing and velocity
 * 
 * 4     8/29/96 5:31p Agrant
 * Major revision
 * Mostly changed code to match coding spec
 * Turned the object list into a class
 * Incorporated a timer
 * Now using TDefReal instead of float
 * 
 * 3     8/28/96 4:13p Agrant
 * new function prototypes
 * 
 * 2     8/27/96 4:39p Agrant
 * Added prototype for LeftButtonClicked()
 * 
 * 1     8/21/96 3:47p Agrant
 * AI test bed initial revision
 * 
 * 1     8/14/96 3:11p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_AI_AI_TEST_HPP
#define HEADER_TEST_AI_AI_TEST_HPP

#include <float.h>

#include "Game/AI/AIMain.hpp"
#include "Lib\EntityDBase\Instance.hpp"
#include "Lib\EntityDBase\Query\QRenderer.hpp"

#include "Game/AI/Silhouette.hpp"


//*********************************************************************************************
//
void Create
(
);
//
// Call once after application initialization.
//
//**************************************

//*********************************************************************************************
//
void Step
(
);
//
// Called when nothing else is going on. Simulation code should be placed here.
//
//**************************************

//*********************************************************************************************
//
void Paint
(
);
//
// Called when the application draws to screen. Line drawing code should be place here.
//
//**************************************

//*********************************************************************************************
//
void Destroy
(
);
//
// Call once before the application is destroyed.
//
//**************************************

//*********************************************************************************************
//
void LeftButtonClicked
(
	TReal		r_x,		//  x location of mouse click in world coords
	TReal		r_y		//  y location of mouse click in world coords
);
//
// Called each time the left mouse button is pressed
//
//**************************************

//*********************************************************************************************
//
void RightButtonClicked
(
	TReal		r_x,		//  x location of mouse click in world coords
	TReal		r_y		//  y location of mouse click in world coords
);
//
// Called each time the right mouse button is pressed
//
//**************************************

//*********************************************************************************************
//
void LeftButtonReleased
(
	TReal		r_x,		//  x location of mouse click in world coords
	TReal		r_y		//  y location of mouse click in world coords
);
//
// Called each time the left mouse button is released
//
//**************************************

//*********************************************************************************************
//
void RightButtonReleased
(
	TReal		r_x,		//  x location of mouse click in world coords
	TReal		r_y		//  y location of mouse click in world coords
);
//
// Called each time the right mouse button is released
//
//**************************************

//*********************************************************************************************
//
void KeyPressed
(
	int virtkey		// virutal key code pressed 
);
//
// Called each time a standard keyboard key is pressed
//
//**************************************

//*********************************************************************************************
//
void Command
(
	int i_id	//  ID of menu item or accelerator selected
);
//
// Called each time a menu item or accelerator is selected.
//  Handles application specific details.
//
//**************************************

//*********************************************************************************************
//
//void AITestCycle
//(
//);
//
// Called each time the Ai's get a chance to think.
// Lets each AI think.
//
//**************************************

//*********************************************************************************************
void AddObject
(
	CInstance*	pins
);
//
// Adds the object to the test app's world.
//
//**************************************

//*********************************************************************************************
inline CInstance* pinsGetNearest
(
	const CVector2<>& v2
)
//
// Gets the object closest to point v2 in 2d world coords.
//
//	Returns:
//		0 if none found.
//
//**************************************
{
	TReal			r_smallest = FLT_MAX;	// Smallest distance to point so far.
	TReal			r_current;				// Distance of current object to point.
	CInstance*		pins_best = 0;			// Address of best object.
	CVector2<>		v2_delta;				// Vector from point to current object.

	CWDbQueryRenderTypes cqs;

	foreach(cqs)
	{
		CInstance*	pins;

		cqs.tGet().ppart->Cast(&pins);

		// Calculate delta.
		v2_delta = cqs.tGet().ppart->v3Pos() - v2;

		// Squared length of delta vector.
		r_current = v2_delta.tLenSqr();

		// Compare with best value so far.
		if (r_current < r_smallest && pins->prdtGetRenderInfo())
		{
			// Save best values.
			r_smallest = r_current;
			pins_best = pins;
		}
	}

	return pins_best;
}



//*********************************************************************************************
void DrawSilhouette
(
	const CSilhouette* psil
);
//
// Draws the silhouette.
//
//**************************************

//*********************************************************************************************
void MaybePerformSearch
(
);
//
// Performs a search on the current graph, if start and stop are well-defined.
//
//**************************************


// A sphere big enough to swallow the world.
//extern CBoundVolSphere bvsBigSphere;



// Ends #ifndef HEADER_TEST_AI_AI_TEST_HPP
#endif
