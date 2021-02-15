/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CTestAnimal class for AI test app.  This is a special case object derived from CTestObject
 *		and is the base class for all animal objects.  It handles the faux physics for animals
 *		in the test app.
 *		
 *		CTestSheep class
 *		CTestWolf  class
 *
 * Bugs:
 *
 *
 * To do:
 *	
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Ai/TestAnimal.hpp                                               $
 * 
 * 25    8/23/98 3:03a Agrant
 * AI Test compile fixes
 * 
 * 24    8/13/98 1:43p Mlange
 * The step message now requires registration of its recipients.
 * 
 * 23    6/08/98 5:46p Agrant
 * Fixes for the new AI Type system
 * 
 * 22    12/17/97 3:41p Agrant
 * AI Test code rot repairs
 * 
 * 21    97/09/28 3:09p Pkeet
 * Replace the use of 'CPresence3<>().v3Pos' with 'v3Pos().'
 * 
 * 20    5/07/97 11:51a Agrant
 * fixed AI Test memory leak
 * 
 ********************************************************************************************/

#ifndef HEADER_TEST_AI_TESTANIMAL_HPP
#define HEADER_TEST_AI_TESTANIMAL_HPP

//#include "Test/AI/TestObject.hpp"
#include "FakeShape.hpp"
#include "Lib/EntityDBase/Animal.hpp"
#include "FakePhysics.hpp"
#include "Lib/GeomDBase/Shape.hpp"
#include "Game/AI/AIInfo.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"

#include "Game/AI/Graph.hpp"


extern rptr<CShapeSheep> rpsShapeSheep;
extern rptr<CShapeDinosaur> rpsShapeDinosaur;
extern rptr<CShapeWolf> rpsShapeWolf;



//*********************************************************************************************
//
class CTest
//
//	Prefix: tst
//
//	A class that has useful stuff for objects in the AI test app.
//
//*********************************************************************************************
{
public:
//	bool bSelected;		// true if currently selected.

	CVector2<>			v2Velocity;		// The current velocity of the entity.

	CVector2<>			v2Destination;
										// The current movement target, if "petEntity" is an animal.

	CRating				rtDestinationSpeed;
										// How quickly ought we head to the destination point?
	
	CRating				rtDestinationUrgency;
										// How important is it to get to the destination?

	CTest() : v2Velocity(0,0),
			  v2Destination(0,0),
			  rtDestinationSpeed(1),
			  rtDestinationUrgency(1)
//			  bSelected(false)
	{
	}

public:
	//*********************************************************************************
	//
	void ToggleSelect
	(
	)
	//
	//	Toggles the select state of the CTest.
	//
	//******************************
	{
		Assert(0);
//		bSelected = !bSelected;
	}



	

};


//*********************************************************************************************
//
class CTestAnimal : public CAnimal, public CTest
//
//	Prefix: ta
//
//	The CTestAniaml class is placeholder for the object and physics systems in the final game.
//
//*********************************************************************************************
{

	//
	// Variable definitions.
	//
public:

//	CVector2<>		v2Destination;	// The animal's movement target.

	static int		iNumAnimals;	// Number of animals created so far.

	//
	// Member function definitions.
	//

public:
	//****************************************************************************************
	//
	//	Constructors and destructor
	//
/*
	CTestAnimal
	(
		TReal r_x,
		TReal r_y
	) : CAnimal()
	{
		// Set the location.
		v3Pos = (CVector3<>(r_x, r_y, 0));

		// Create a physics model.
		pPhysicsInfo	= new CPhysicsAnimal(this);

		// Create default shape info.
		pShapeInfo		= new CShapeInfo();


		// Increment animal counter.
		iNumAnimals++;
	}
*/
	
	CTestAnimal
	(
		TReal				r_x,	// x location in world coords
		TReal				r_y,	// y location in world coords
		rptr<CRenderType>	pshi,	// Shape information.
		CAIInfo*			paii,
		CPhysicsInfo*		pphi = 0
	) : CAnimal()
	{
		// Set the location.
		SetPos(CVector3<>(r_x, r_y, 0));
		v2Destination = CVector2<>(r_x, r_y);

		Assert(paii->eaiRefType);

		// Make sure we have valid physics info.
		if (pphi == 0)
		{
			SetPhysicsInfo(new CTestPhysicsInfoAnimal());
		}

		// Make sure we have valid render info.
		if (pphi == 0)
		{
			SetRenderInfo(pshi);
		}
		
		// Make sure we have valid physics info.
		if (pphi == 0)
		{
			SetAIInfo(paii);
		}

		// Increment animal counter.
		iNumAnimals++;

		// Register this entity with the message types it needs to receive.
		CMessageStep::RegisterRecipient(this);
	}


	CTestAnimal
	(
		TReal		r_x,	// x location in world coords
		TReal		r_y,	// y location in world coords
		rptr<CRenderType> pshi,		// Shape information.
		EAIRefType  eai = eaiDONTCARE,
		CPhysicsInfo* pphi = 0
	) : CAnimal()
	{
		// Set the location.
		SetPos(CVector3<>(r_x, r_y, 0));
		v2Destination = CVector2<>(r_x, r_y);

		// Make sure we have valid physics info.
		if (pphi == 0)
		{
			SetPhysicsInfo(new CTestPhysicsInfoAnimal());
		}

		// Make sure we have valid render info.
		if (pphi == 0)
		{
			SetRenderInfo(pshi);
		}
		
		// Make sure we have valid physics info.
		if (pphi == 0)
		{
			SetAIInfo(new CAIInfo(eai));
		}

		// Increment animal counter.
		iNumAnimals++;

		// Register this entity with the message types it needs to receive.
		CMessageStep::RegisterRecipient(this);
	}


	~CTestAnimal()
	{
		CMessageStep::UnregisterRecipient(this);
	}

	//****************************************************************************************
	//
	//	Member fucntions.
	//
	TReal rGetMaxSpeed()
	{
		// Placeholder function and value, waiting for true animal class.
		return 10.0;
	}


	//****************************************************************************************
	//
	//	Overrides.
	//

	//******************************************************************************************
	virtual void Process
	(
		const CMessageStep& msgstep
	) override
	{
		if (pphiGetPhysicsInfo())
		{
			const CTestPhysicsInfo* ptphi = dynamic_cast<CTestPhysicsInfo*>((CPhysicsInfo*)pphiGetPhysicsInfo());
			if (ptphi)
				ptphi->Move(msgstep.sStep, this);
		}
	}


};


//*********************************************************************************************
//
class CTestSheep : public CTestAnimal
//
//	Prefix: ts
//
//	The CTestSheep class is placeholder for the object and physics systems in the final game.
//	It represents a sheep.
//
//*********************************************************************************************
{
public:
	//
	// Member function definitions.
	//

public:
	//****************************************************************************************
	//
	//	Constructors and destructor
	//

	CTestSheep
	(
		TReal r_x, 
		TReal r_y

	) : CTestAnimal(r_x,r_y, 
		rptr_cast(	CRenderType,
					rpsShapeSheep),
					eaiANIMAL)
	{
	}

	//****************************************************************************************
	//
	//	Overrides.
	//
};



//*********************************************************************************************
//
class CTestDinosaur : public CTestAnimal
//
//	Prefix: ts
//
//	The CTestDinosaur class is placeholder for the object and physics systems in the final game.
//	It represents a dino.
//
//*********************************************************************************************
{
public:
	//
	// Member function definitions.
	//

public:
	//****************************************************************************************
	//
	//	Constructors and destructor
	//

	CTestDinosaur
	(
		TReal r_x, 
		TReal r_y

	) : CTestAnimal(r_x,r_y, 
				rptr_cast(CRenderType,rpsShapeDinosaur),
				new CAIInfo(eaiANIMAL))
	{
	}

};




//*********************************************************************************************
//
class CTestWolf : public CTestAnimal
//
//	Prefix: tw
//
//	The CTestWolf class is placeholder for the object and physics systems in the final game.
//	It represents a wolf.
//
//*********************************************************************************************
{
public:
	//
	// Member function definitions.
	//

public:
	//****************************************************************************************
	//
	//	Constructors and destructor
	//

	CTestWolf
	(
		TReal r_x, 
		TReal r_y

	) : CTestAnimal(r_x,r_y, 
					rptr_cast(CRenderType,rpsShapeWolf),
					eaiANIMAL)
	{
	}

	//****************************************************************************************
	//
	//	Overrides.
	//
};

//  ends #ifndef HEADER_TEST_AI_TESTANIMAL_HPP
#endif   
