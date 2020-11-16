/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		An object list class for the AI Test App
 *		
 *
 * To do:
 *
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/ObjectList.hpp                                               $
 * 
 * 7     10/31/96 7:26p Agrant
 * changed over to world database as seen in the Game.  Yahoo.
 * 
 * 6     10/23/96 7:39p Agrant
 * more debugging
 * now using the dialog box for printing some text
 * first pass pathfinder
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
 * 3     9/23/96 2:51p Agrant
 * Added some test helper functions
 * to fake worldview inputs
 * 
 * 2     9/19/96 1:22p Agrant
 * Add an AI call that lets every animal think one cycle.
 * 
 * 1     9/03/96 8:08p Agrant
 * initial revision
 * 
 * 1     8/30/96 10:55a Agrant
 * initial revision
 * 
 *********************************************************************************************/

#ifndef HEADER_TEST_AI_OBJECTLIST_HPP
#define HEADER_TEST_AI_OBJECTLIST_HPP

#error ObjectList.hpp no longer a valid include file.

#include <list.h>

#include "Game/AI/Brain.hpp"

#include "TestObject.hpp"
#include "TestAnimal.hpp"

//
// Class definitions.
//

//*********************************************************************************************
//
class CObjectList : public list<CTestObject *>
//
// Prefix: col
//
//		CObjectList contains a bunch of CTestObject's.  It has several functions that compare each 
//		element in the list	or update each element in the list.
//
//		When a member function ends in "All" it performs an action on all objects.
//		When a member function ends in "Selected" it performs an action on selected objects.
//
//**************************************
{

	//*****************************************************************************************
	//
	//	Member functions
	//
	public:

		//*********************************************************************************
		//
		virtual void DrawAll
		(
		)
		//
		//		Draws each object at its current location.
		//
		//	Side effects:
		//		modifies window bitmap
		//
		//******************************
		{
			for (iterator pcto = begin(); pcto != end(); pcto++)
				(*pcto)->Draw();
		}


		//*********************************************************************************
		//
		virtual void MoveAll
		(
			TMSec msec_elapsed_time
		)
		//
		//		Moves each object for "sec" seconds.
		//
		//	Side effects:
		//		Modifies object locations.
		//
		//******************************
		{
			for (iterator pcto = begin(); pcto != end(); pcto++)
				(*pcto)->Move(sec);
		}

		//*********************************************************************************
		//
		virtual void SetSelectedAll
		(
			bool b_selected
		)
		//
		//		Selects every object if b_selected is TRUE
		//		Unselects every object if b_selected is FALSE
		//
		//	Side effects:
		//		Modifies objects.
		//
		//******************************
		{
			for (iterator pcto = begin(); pcto != end(); pcto++)
				(*pcto)->bSelected = b_selected;
		}

		
		//*********************************************************************************
		//
		void SetVelocityToFrontAll
		(
		)
		//
		//		Rotates velocity vector so that each object is moving towards its front
		//
		//	Side effects:
		//		Modifies object velocities.
		//
		//******************************
		{
			TReal r_len;
			for (iterator pcto = begin(); pcto != end(); pcto++)
			{
				// Find velocity magnitude.
				r_len = (*pcto)->v2Velocity.tLen();

				// Set velocity 
				(*pcto)->v2Velocity = (*pcto)->v2Front * r_len;
			}
		}

		
		//*********************************************************************************
		//
		CTestObject *pctoGetNearestAll
		(
			CVector2<>	v2_point
		);
		//
		//	requests a reference to the object nearest v2_point in world coords
		//	
		//
		//	Returns:
		//		A pointer to the object nearest v2_point
		//		0 if none exists.
		//
		//	Notes:
		//		If this object is later deleted or removed from the list, this pointer 
		//		becomes invalid.  Do not store references to this object.  Be sure to 
		//		use them immediately.
		//
		//******************************

		//*********************************************************************************
		//
		void CObjectList::AnimalsThinkAll
		(
		);
		//
		//	Causes all animals to run their AI's
		//
		//******************************

		//*********************************************************************************
		//
		void CObjectList::TellAnimalAboutAll
		(
			CTestAnimal*	pta
		)
		//
		//	Informs "*pta" about all objects in the liat.
		//
		//******************************
		{
			for (iterator ptobj = begin(); ptobj != end(); ptobj++)
			{
				pta->pbrBrain->msState.Notice(*ptobj);
			}
		}


		//*********************************************************************************
		//
		void CObjectList::TellAnimalsAboutObjectAll
		(
			CTestObject*	ptobjSeen
		)
		//
		//	Informs each animal in the list that "*ptobjSeen" exists.
		//
		//******************************
		{
			for (iterator ptobj = begin(); ptobj != end(); ptobj++)
			{
				if ((*ptobj)->etotType == etotANIMAL)
					((CTestAnimal*)*ptobj)->pbrBrain->msState.Notice(ptobjSeen);
			}
		}



		//*********************************************************************************
		//
		virtual void RemoveSelected
		(
		)
		//
		//		Destroys all objects currently selected
		//
		//	Side effects:
		//		Destroys objects
		//
		//******************************
		{
			for (iterator pcto = begin(); pcto != end(); )
			{
				if ((*pcto)->bSelected)
					erase(pcto++);
				else
					pcto++;
			}
		}

 		//*********************************************************************************
		//
		virtual void SetFrontSelected
		(
			CVector2<>	v2_front
		)
		//
		//		Sets the front of all objects currently selected to "v2_front"
		//
		//	Side effects:
		//		Alters objects.
		//
		//******************************
		{
			// The front vector must have length 1.
			Assert(v2_front.bIsNormalised());

			for (iterator pcto = begin(); pcto != end(); pcto++)
			{
				if ((*pcto)->bSelected)
					(*pcto)->v2Front = v2_front;
			}
		}


 		//*********************************************************************************
		//
		void DrawInfluencesSelected
		(
		);
		//
		//	Draws the influences of all selected animals.
		//
		//******************************

 		//*********************************************************************************
		//
		void DrawDebug
		(
		);
		//
		//	Draws debugging info on a per-object basis
		//
		//******************************



 };
// Ends #ifndef HEADER_TEST_AI_OBJECTLIST_HPP
#endif 
