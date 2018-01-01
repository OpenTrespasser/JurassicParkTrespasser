/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CMentalState class for AI library.  CMentalState handles all data
 *		for an individual dino.
 *		
 *
 * Bugs:
 *
 * To do:
 *		Write the CMentalState class description.
 *		Add an error parameter that works better than a time factor.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/MentalState.hpp                                               $
 * 
 * 15    2/23/98 5:07p Agrant
 * AI save/load
 * 
 * 14    7/19/97 1:24p Agrant
 * Emotions now change over time, going back to their default values.
 * 
 * 13    5/06/97 7:54p Agrant
 * delete those pointers
 * 
 * 12    4/07/97 4:03p Agrant
 * Closer to True Vision(tm) technology!
 * 
 * 11    3/28/97 2:00p Agrant
 * Brain now points to graph, mentalstate, and worldview
 * Pathfinding more efficient
 * 
 * 10    3/26/97 5:28p Agrant
 * Added number of pathfinding nodes to brain constructor, mentalstate, and worldview.
 * 
 * 9     11/20/96 1:26p Agrant
 * Now using world database queries for perception.
 * Now using archetypes for default personality behaviors.
 * 
 * 8     11/14/96 4:50p Agrant
 * AI subsystem now in tune with the revised object hierarchy
 * 
 * 7     11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 6     11/07/96 5:56p Agrant
 * Only objects that matter need to added to influence lists.
 * 
 * 5     10/10/96 7:18p Agrant
 * Modified code for code specs.
 * 
 * 4     9/27/96 5:43p Agrant
 * Added the objectless sub-brain and created a CWanderActivity
 * 
 * 3     9/23/96 3:02p Agrant
 * Worldview and the Notice() function added.
 * 
 * 2     9/19/96 1:26p Agrant
 * Mental State modified to be closer to spec.  
 * 
 * 1     9/03/96 8:12p Agrant
 * initial revision
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_MENTALSTATE_HPP
#define HEADER_LIB_AI_MENTALSTATE_HPP

#include "Lib/EntityDBase/Instance.hpp"

//#include "Lib/Sys/Timer.hpp"


#include "Feeling.hpp"
#include "WorldView.hpp"


//*********************************************************************************************
//
class CMentalState
//
//	Prefix: mstate
//
//	All memory and state data for a particular animal.
//
//  Notes:
//		The CMentalState class records all memory and state data for an  
//		animal.  
//
//*********************************************************************************************
{

	//*****************************************************************************************
	//
	//	Member Variables
	//
public:
	CFeeling		feelEmotions;		// The animal's emotional state.

	CFeeling		feelDefault;		// The animal's natural emotional state.

	CFeeling		feelRestoringForce;	// The rate at which the emotions return to their natural state,
										//  in one second.
	
//	CWorldView		wvWorldView;		// The animal's picture of the world around it.

	CVector2<>		v2LastDirection;	// Direction desired at end of last AI cycle.
	
	//*****************************************************************************************
	//
	//	Constructors
	//
public:
	CMentalState
	(
	);

	~CMentalState
	(
	)
	{};

	//*****************************************************************************************
	//
	//	Member functions
	//
public:

// Disable the Notice function until it's actually used.

		//*********************************************************************************
		//
//		void Notice
//		(
//			CInstance*	pins,			// Object to be noticed.
//			TSec		s_current_time	// When the object is noticed.
//		);
		//
		//	Forces the mental state to take note of the entity.
		//
		//	Notes:
		//		Creates an influence related to it, if appropriate and none currently exist.
		//		If the object has already been noticed, updates the accuracy of its influence.
		//		The "delay" parameter indicates some uncertainty about exactly where the 
		//		influence is located.
		//
		//	Example:
		//		A 1 second delay means that the animal thinks that the influence might be 
		//		up to a second's travel time away from where it is now.  
		//
		//
		//******************************

		//*********************************************************************************
		//
		void RestoreEmotions
		(
			TSec		s_time	// How long since the last restoration.
		);
		//
		//	Restores feelEmotions toward feelDefault based on time and feelRestoringForce.
		//
		//******************************

		//*****************************************************************************************
		virtual char * pcSave(char *  pc_buffer) const;

		//*****************************************************************************************
		virtual const char * pcLoad(const char *  pc_buffer);


};


#endif   // #ifndef HEADER_LIB_AI_MENTALSTATE_HPP
