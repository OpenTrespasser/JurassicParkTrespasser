/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 *	Implementation of Pathfinder.hpp and PathFailure.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/Ai/Pathfinder.cpp                                               $
 * 
 * 2     5/19/98 9:16p Agrant
 * pathfailure and path obstacle and pathavoider all gone
 * 
 * 1     1/28/97 6:44p Agrant
 * pathfinding files
 * 
 **********************************************************************************************/

#if 0
#include "Common.hpp"
#include "Pathfinder.hpp"
#include "PathFailure.hpp"

//*********************************************************************************************
//
//	Class CPathfinder implementation.
//


	//*********************************************************************************
	void CPath::SetFailure
	(
		CPathFailure* pf
	)
	{
		if (pfFailure != 0)
			delete pfFailure;
		pfFailure = pf;
	}



#endif