/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	CModifier class for AI library.  CModifers collectively handle all sensory input
 *	and mental state changes for a dino
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Game/AI/Modifier.hpp                                                  $
 * 
 * 1     9/03/96 8:12p Agrant
 * initial revision
 * 
 * 1     9/03/96 7:59p Agrant
 * initial revision
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_AI_MODIFIER_HPP
#define HEADER_LIB_AI_MODIFIER_HPP

#include "AITypes.hpp"
class CBrain;

//*********************************************************************************************
//
class CModifier
//
//	Prefix: cmod
//
//	The CModifer class is little more than a function that modifies a mental state
//  in a particular way.  All modifiers must inherit from this base class.
//
//*********************************************************************************************
{

//  Variable declarations

//  Member function definitions
public:
	//*****************************************************************************************
	//
	//	Constructors and destructor
	//

	CModifier
	(
	)
	{};

	~CModifier
	(
	)
	{};


	//*****************************************************************************************
	//
	//	Member functions
	//

		//*********************************************************************************
		//
		void Modify
		(
			CBrain*		pbrain
		)
		//
		//	Examines the world to see how the mental state of the provided brain 
		//	ought to change.
		//
		//	Side effects:
		//		modifies pbrain->pmstate
		//
		{
			// This function should never be called.  This is a base class only.
			Assert(0);
		}
		//******************************

};

//#ifndef HEADER_LIB_AI_MODIFIER_HPP
#endif