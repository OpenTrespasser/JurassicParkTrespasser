/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 *	Implementation of ActivityPhased.hpp.
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/ActivityPhased.cpp                                           $
 * 
 * 4     8/26/98 7:13p Agrant
 * added stop
 * 
 * 3     8/20/98 11:10p Agrant
 * some fixes
 * 
 * 2     5/17/98 9:30p Agrant
 * now compiles
 * 
 * 1     5/17/98 7:49p Agrant
 * initial rev
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "ActivityPhased.hpp"

#include "AIMain.hpp"

//*********************************************************************************************
//
//	Class CActivityPhased implementation.
//

CActivityPhased::CActivityPhased(char *ac_name) : CActivity(ac_name)
{
}

void CActivityPhased::Act(CRating rt_importance, CInfluence* pinf)
{

	// Perform start activities if appropriate.
	if (!bWasChosen())
	{
		GotoPhaseOne();
		Start(pinf);
	}

	// Stop if we are ready to give up.
	if (sGiveUp < gaiSystem.sNow)
	{
		DontContinue();
		Stop(pinf);
	}
	else
	{
		TryToContinue();
	}

	// Perform the correct phase actions.
	if (sFlags[eafPHASEONE])
	{
		DoPhaseOne(rt_importance, pinf);
	}
	else if (sFlags[eafPHASETWO])
	{
		DoPhaseTwo(rt_importance, pinf);
	}
	else if (sFlags[eafPHASETHREE])
	{
		DoPhaseThree(rt_importance, pinf);
	}
	else if (sFlags[eafPHASEFOUR])
	{
		DoPhaseFour(rt_importance, pinf);
	}
}


void CActivityPhased::GotoPhaseOne()
{
	ClearPhaseFlags();
	sFlags[eafPHASEONE] = true;
}

void CActivityPhased::GotoPhaseTwo()
{
	ClearPhaseFlags();
	sFlags[eafPHASETWO] = true;
}

void CActivityPhased::GotoPhaseThree()
{
	ClearPhaseFlags();
	sFlags[eafPHASETHREE] = true;
}

void CActivityPhased::GotoPhaseFour()
{
	ClearPhaseFlags();
	sFlags[eafPHASEFOUR] = true;
}

void CActivityPhased::Start(CInfluence* pinf)
{
}

void CActivityPhased::Stop(CInfluence* pinf)
{
}

void CActivityPhased::DoPhaseOne(CRating rt_importance, CInfluence* pinf)
{
	Assert(false);
}

void CActivityPhased::DoPhaseTwo(CRating rt_importance, CInfluence* pinf)
{
	Assert(false);
}

void CActivityPhased::DoPhaseThree(CRating rt_importance, CInfluence* pinf)
{
	Assert(false);
}

void CActivityPhased::DoPhaseFour(CRating rt_importance, CInfluence* pinf)
{
	Assert(false);
}
