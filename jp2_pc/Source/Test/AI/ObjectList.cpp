/**********************************************************************************************
 *
 * $Source::																				  $	
 * Copyright © DreamWorks Interactive. 1996
 *
 *		Implementation of ObjectList.hpp
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/ObjectList.cpp                                               $
 * 
 * 7     10/23/96 7:39p Agrant
 * more debugging
 * now using the dialog box for printing some text
 * first pass pathfinder
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
 * 4     9/26/96 5:59p Agrant
 * AI system basic structure implemented
 * AI Test app has basic herding behavior based on two simple activities
 * TReal used as basic world coordinate type.
 * 
 * 3     9/23/96 2:50p Agrant
 * add "classes" include
 * 
 * 2     9/19/96 1:22p Agrant
 * Add a call to let each animal think one cycle.
 * 
 * 1     9/03/96 8:08p Agrant
 * initial revision
 * 
 * 1     8/30/96 10:55a Agrant
 * initial revision
 * 
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "ObjectList.hpp"  

#include <float.h>

#include "Game\AI\Classes.hpp"
#include "Game\AI\Brain.hpp"

#include "TestAnimal.hpp"
#include "AITestShell.hpp"
#include "airesource.h"

//*********************************************************************************************
//
//	Class CTestObject
//

		//*********************************************************************************
		//
		CTestObject *CObjectList::pctoGetNearestAll
		(
			CVector2<>	v2_point	// Point we're trying to get near to.
		)
		//
		//	Returns:
		//		A pointer to the object nearest v2_point
		//		0 if none exists.
		{
			TReal		r_smallest = FLT_MAX;	// Smallest distance to point so far.
			TReal		r_current;				// Distance of current object to point.
			CTestObject*	pcto_best = 0;			// Address of best object.
			CVector2<>		v2_delta;				// Vector from point to current object.

			// Iterate through all objects.
			for (iterator pcto = begin(); pcto != end(); pcto++)
			{
				// Calculate delta.
				v2_delta = (*pcto)->v2Location - v2_point;

				// Squared length of delta vector.
				r_current = v2_delta.tLenSqr();

				// Compare with best value so far.
				if (r_current < r_smallest)
				{
					// Save best values.
					r_smallest = r_current;
					pcto_best = (*pcto);
				}
			}

			return pcto_best;
		}


		//*********************************************************************************
		//
		void CObjectList::AnimalsThinkAll
		(
		)
		//
		//	Causes all animals to run their AI's
		//
		{
			for (iterator pcto = begin(); pcto != end(); pcto++)
				if ((*pcto)->etotType == etotANIMAL)
					((CTestAnimal*)*pcto)->pbrBrain->Think();
		}




 		//*********************************************************************************
		//
		void CObjectList::DrawDebug
		(
		)
		//
		//	Draws debugging info on a per-object basis
		//
		//******************************
		{
			bool b_selected_influences		= IsMenuItemChecked(ID_DEBUG_INFLUENCES);
			bool b_selected_destinations	= IsMenuItemChecked(ID_DEBUG_DESTINATIONS);
			bool b_strings					= IsMenuItemChecked(ID_DEBUG_STRINGS);

			for (iterator pto = begin(); pto != end(); pto++)
			{
				// Draw the debug info pushed by the AI system.
				if (b_strings)
					(*pto)->DrawDebugString();

				// Clear the debug info for next frame.
				(*pto)->sacDebugString.Reset();

				if ((*pto)->bSelected && 
					(*pto)->etotType == etotANIMAL)
				{
					if (b_selected_influences)
					{
						// Tell the brain to draw its influences.
						Assert((*pto)->pbrBrain != 0);
						(*pto)->pbrBrain->DrawInfluences();
					}

					if (b_selected_destinations)
					{
						// Tell the aniaml to draw its destination.
						((CTestAnimal*)(*pto))->DrawDestination();
					}

				}
			}
		}

	
