/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	CThinker class for AI library.  CThinker handles all processing for a dino
 *		
 *
 * Bugs:
 *
 * To do:
 *	write the thinker
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/Thinker.hpp                                                   $
 * 
 * 1     9/03/96 8:13p Agrant
 * initial revision
 * 
 * 1     9/03/96 7:59p Agrant
 * initial revision
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_THINKER_HPP
#define HEADER_LIB_AI_THINKER_HPP

#include "Modifier.hpp"
#include "Activity.hpp"
#include "AITypes.hpp"
class CBrain;

//*********************************************************************************************
//
class CThinker
//
//	Prefix: thinker
//
//	The CThinker class is an object associated with one or more CBrain instances.
//  It is loaded from a resource (maybe a file) and determines how a dino will act
//
//	Example:
//		none
//
//*********************************************************************************************
{

//  Variable declarations
public:
	CModifier* amod[emtNUM_MODIFIERS];	//  all modifiers for the thinker
	CActivity* aact[eatNUM_ACTIVITIES];	//  all activities for the thinker

protected:
private:

//  Member function definitions
public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	CThinker
	(
	);

	~CThinker();


	//*****************************************************************************************
	//
	//	Member functions
	//

		//*********************************************************************************
		//
		void Modify
		(
			CBrain*		pbrain
		);
		//
		//	Examines the world to see how the mental state of the provided brain 
		//	ought to change.
		//
		//	Side effects:
		//		modifies pbrain->pmstate
		//
		//	Notes:
		//		Right now, this function just goes through every modifier sequentially.
		//		This could be better, I think.
		//
		//******************************

		//*********************************************************************************
		//
		void Act
		(
			CBrain*		pbrain
		);
		//
		//	Using the brain's mental state, determines a course of action and issues the 
		//	messages(events) necessary to enact it.
		//
		//	Side effects:
		//		Issues events.
		//
		//******************************


};


#endif   // #ifndef HEADER_LIB_AI_THINKER_HPP