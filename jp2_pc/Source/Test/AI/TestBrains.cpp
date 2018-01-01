/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of AI TestShell.hpp.
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/TestBrains.cpp                                               $
 * 
 * 4     1/13/98 9:34p Agrant
 * Moved constructor implementations to .cpp
 * 
 * 3     7/31/97 4:37p Agrant
 * A different way of thinking for animals
 * 
 * 2     5/11/97 12:51p Agrant
 * Moved fullbrain constructor to .cpp file
 * 
 * 1     5/09/97 3:17p Agrant
 * initial rev
 * 
 **********************************************************************************************/



#include "Common.hpp"

#include "TestBrains.hpp"
#include "Game/AI/ActivityCompound.hpp"
#include "Game/AI/MoveActivities.hpp"
#include "Game/AI/ActivityAttack.hpp"

//**********************************************************************************************
//
//	Class CFullBrain implementation.
//


	CFullBrain::CFullBrain
	(		
		CAnimal*	pet_owner,
		EArchetype	ear,
		char*		pc_name
	):	 CBrain(pet_owner, ear, pc_name)
	{

		sapactActivities[(int)eatWANDER]->Activate(true);

		// Get a better name.
		if (dynamic_cast<CTestAnimal*>(pet_owner))
		{
			char ac[32];
			sprintf(ac, "%d", (dynamic_cast<CTestAnimal*>(pet_owner))->iNumAnimals);
			strcat(acName, ac);
		}
	};



	CHerdBrain::CHerdBrain
	(
		CAnimal*	pet_owner,
		EArchetype	ear,
		char*		pc_name 
 	) : CBrain(pet_owner, ear, pc_name)
	{

		sapactActivities[(int)eatWANDER]->Activate(true);


		// Get a better name.
		if (dynamic_cast<CTestAnimal*>(pet_owner))
		{
			char ac[32];
			sprintf(ac, "%d", (dynamic_cast<CTestAnimal*>(pet_owner))->iNumAnimals);
			strcat(acName, ac);
		}
	};


	CTestBrain::CTestBrain
	(
		CAnimal*	pet_owner,
		EArchetype	ear,
		char*		pc_name 
 	) : CBrain(pet_owner, ear, pc_name)
	{
		sapactActivities[(int)eatWANDER]->Activate(true);

		
		// Get a better name.
		if (dynamic_cast<CTestAnimal*>(pet_owner))
		{
			char ac[32];
			sprintf(ac, "%d", (dynamic_cast<CTestAnimal*>(pet_owner))->iNumAnimals);
			strcat(acName, ac);
		}
	};
