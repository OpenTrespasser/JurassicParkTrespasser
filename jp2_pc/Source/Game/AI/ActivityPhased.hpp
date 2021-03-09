/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Contents:
 *		CActivity child classes for phased actions
 *
 * Bugs:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/ActivityPhased.hpp                                            $
 * 
 * 2     8/26/98 7:13p Agrant
 * added stop
 * 
 * 1     5/17/98 7:50p Agrant
 * initial rev
 * 
 **********************************************************************************************/

#ifndef HEADER_GAME_AI_ACTIVITYPHASED_HPP
#define HEADER_GAME_AI_ACTIVITYPHASED_HPP

#include "Activity.hpp"
#include "Synthesizer.hpp"


#define MAX_PATH 256

//*********************************************************************************************
//
class CActivityPhased : public CActivity
//
//	Prefix: av
//
//	A behavior in which the animal makes a sound.
//
//  Notes:
//		Base class for lots of complicated guys.
//
//*********************************************************************************************
{

public:

//
//  Member function definitions.
//

public:
	//*****************************************************************************************
	//
	//	Constructors and destructor.
	//

		// Probably won't ever need a default vocal activity.
		CActivityPhased
		(
			char *ac_name
		);
	


	//*****************************************************************************************
	//
	//	Functions.
	//

		//*************************************************************************************
		//
		virtual void Start(CInfluence* pinf);
		//
		//	Called the first time the activity fires after not firing for a while.
		//
		//******************************

		//*************************************************************************************
		//
		virtual void Stop(CInfluence* pinf);
		//
		//	Called if the activity times out.
		//
		//******************************

		//*************************************************************************************
		//
		virtual void GotoPhaseOne();
		//
		//	Next time, we do phase one.
		//
		//******************************

		//*************************************************************************************
		//
		virtual void DoPhaseOne(CRating rt_importance,	CInfluence*	pinf);
		//
		//	Called to perform phase one actions.
		//
		//******************************

		//*************************************************************************************
		//
		virtual void GotoPhaseTwo();
		//
		//	Next time, we do phase two.
		//
		//******************************

		//*************************************************************************************
		//
		virtual void DoPhaseTwo(CRating rt_importance,	CInfluence*	pinf);
		//
		//	Called to perform phase two actions.
		//
		//******************************

		//*************************************************************************************
		//
		virtual void GotoPhaseThree();
		//
		//	Next time, we do phase 3.
		//
		//******************************

		//*************************************************************************************
		//
		virtual void DoPhaseThree(CRating rt_importance,	CInfluence*	pinf);
		//
		//	Called to perform phase 3 actions.
		//
		//******************************

		//*************************************************************************************
		//
		virtual void GotoPhaseFour();
		//
		//	Next time, we do phase one.
		//
		//******************************

		//*************************************************************************************
		//
		virtual void DoPhaseFour(CRating rt_importance,	CInfluence*	pinf);
		//
		//	Called to perform phase one actions.
		//
		//******************************

	//*****************************************************************************************
	//
	//	Overrides.
	//
	
		//*********************************************************************************
		virtual void Act(CRating rt_importance,	CInfluence*	pinf) override;

};


#endif