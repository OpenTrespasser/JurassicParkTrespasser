/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 *	Implementation of Thinker.hpp.
 *
 *	Bugs:
 *
 *	To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/Thinker.cpp                                                  $
 * 
 * 1     9/03/96 8:13p Agrant
 * initial revision
 * 
 * 1     9/03/96 7:59p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#include "Common.hpp"

#include "AITypes.hpp"
#include "Thinker.hpp"


//**********************************************************************************************
//
//	Class CThinker implementation.
//

	//****************************************************************************************
	CThinker::CThinker()
	{
		// Zero all modifiers
		for (int i = emtNUM_MODIFIERS - 1; i >= 0; i--)
			amod[i] = 0;

		// Zero all activities
		for (i = eatNUM_ACTIVITIES - 1; i >= 0; i--)
			aact[i] = 0;
	}
	
	//****************************************************************************************
	CThinker::~CThinker()
	{
		// Delete all modifiers
		for (int i = emtNUM_MODIFIERS - 1; i >= 0; i--)
			if (amod[i])
				delete amod[i];

		// Delete all activities
		for (i = eatNUM_ACTIVITIES - 1; i >= 0; i--)
			if (aact[i])
				delete aact[i];
	}
	
	//****************************************************************************************
	void CThinker::Modify( CBrain* p_brain )
	{
		// Run each modifier
		for (int i = emtNUM_MODIFIERS - 1; i >= 0; i--)
			if (amod[i])
				amod[i]->Modify(p_brain);
	}
	

	//****************************************************************************************
	void CThinker::Act( CBrain* p_brain )
	{
		// Run each activity
		for (int i = eatNUM_ACTIVITIES - 1; i >= 0; i--)
			if (aact[i])
				aact[i]->Act(p_brain);
	}
