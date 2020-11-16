/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		
 *		Inanimate CTestFOO classes for AI Test App.
 *		
 *
 * Bugs:
 *
 *
 * To do:
 *	
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/AI/TestTree.hpp                                                 $
 * 
 * 20    8/23/98 3:03a Agrant
 * AI Test compile fixes
 * 
 * 19    6/08/98 5:46p Agrant
 * Fixes for the new AI Type system
 * 
 * 18    5/10/98 6:20p Agrant
 * allow loading scenes
 * 
 * 17    97/09/28 3:09p Pkeet
 * Replace the use of 'CPresence3<>().v3Pos' with 'v3Pos().'
 * 
 * 16    9/09/97 8:52p Agrant
 * trees are now tall
 * 
 * 15    7/31/97 4:37p Agrant
 * Trees are tall
 * 
 * 14    2/05/97 8:07p Agrant
 * Added a wall/line segment intersection test.
 * 
 * 13    1/30/97 2:36p Agrant
 * Trees are now 1 meter cubes!
 * 
 * 12    1/07/97 6:50p Agrant
 * rptr's and other random structure changes integrated into test app.
 * 
 * 11    97/01/07 11:18 Speter
 * Put all CRenderTypes in rptr<>.
 * 
 * 10    12/10/96 7:41p Agrant
 * Fixing AI Test App for code rot.
 * World DBase changes, mostly.
 * 
 * 9     96/12/04 20:34 Speter
 * A big change: Updated for new CPresence3<>, and CInstance functions.
 * 
 * 8     11/14/96 11:17p Agrant
 * now using accessor functions to update pInfo.
 * 
 * 7     11/14/96 4:49p Agrant
 * AI Test more in tune with the new object hierarchy
 * 
 * 6     11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 5     11/02/96 7:20p Agrant
 * Created a generic inanimate test template class
 * 
 * 4     10/31/96 7:26p Agrant
 * changed over to world database as seen in the Game.  Yahoo.
 * 
 * 3     10/14/96 12:21p Agrant
 * Added walls to test app.
 * 
 * 2     10/02/96 8:10p Agrant
 * Added Sheep and Wolves.
 * Added Influence Debugging.
 * Added AI frame rate counter.
 * Unified all meter/pixel conversions.
 * 
 * 1     10/01/96 7:44p Agrant
 * initial revision
 * 
 ********************************************************************************************/

#ifndef HEADER_TEST_AI_TESTTREE_HPP
#define HEADER_TEST_AI_TESTTREE_HPP

#include "Test/AI/TestAnimal.hpp"
#include "Lib/GeomDBase/LineSegment.hpp"

#include <stdio.h>

class CAIInfo;

extern rptr<CShapeTree> rpsShapeTree;
extern rptr<CShapeWall> rpsShapeWall;

//*********************************************************************************************
//
class CTestThing : public CInstance, public CTest
//
//	Prefix: tth
//
//	The CTestThing makes it easier to create inanimate test entities.
//
//*********************************************************************************************
{
public:
	//
	// Member function definitions.
	//

static int i_num_test_objects;

public:
	//****************************************************************************************
	//
	//	Constructors and destructor
	//

	CTestThing
	(
		TReal		r_x,	// X location in world coords
		TReal		r_y,	// Y location in world coords
		rptr<CRenderType> pshi,		// rendering info
		CAIInfo*	paii,
		CPhysicsInfo* pphi = 0

	) : CInstance(pshi, pphi, paii)
	{
		char buffer[256];

		sprintf(buffer, "Test%2d", i_num_test_objects);
		++i_num_test_objects;
		SetInstanceName(buffer);

		// Set the location.
		SetPos(CVector3<>(r_x, r_y, 0));

		if (pphi == 0)
			SetPhysicsInfo(new CTestPhysicsInfo(1,1,10));
	}

	CTestThing
	(
		TReal		r_x,	// X location in world coords
		TReal		r_y,	// Y location in world coords
		rptr<CRenderType> pshi,		// rendering info
		EAIRefType	eai	= eaiDONTCARE,
		CPhysicsInfo* pphi = 0
	) : CInstance(pshi, pphi, new CAIInfo(eai))
	{
		char buffer[256];

		sprintf(buffer, "Test%2d", i_num_test_objects);
		++i_num_test_objects;
		SetInstanceName(buffer);

		// Set the location.
		SetPos(CVector3<>(r_x, r_y, 0));

		if (pphi == 0)
			SetPhysicsInfo(new CTestPhysicsInfo(1,1,10));
	}

	/*
	CTestThing
	(
		TReal		r_x,	// X location in world coords
		TReal		r_y		// Y location in world coords
	) : CEntityPhysics()
	{
		v3Pos = CVector3<>(r_x, r_y, 0);

		// Create a physics model.
		pPhysicsInfo = new CPhysicsInfo(this);

		// Create a default shape model.
		pShapeInfo	 = new CRenderType();

	}
*/
	//****************************************************************************************
	//
	//	Overrides.
	//

};


//*********************************************************************************************
//
class CTestTree : public CTestThing
//
//	Prefix: tt
//
//	The CTestTree class is placeholder for the object and physics systems in the final game.
//	It represents a tree.
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

	CTestTree
	(
		TReal r_x, 
		TReal r_y

	) : CTestThing(r_x, r_y, 
					rptr_cast(CRenderType,rpsShapeTree),
					eaiUNKNOWN,
					new CTestPhysicsInfo(1,1,10, false))
	{
	}

	//****************************************************************************************
	//
	//	Overrides.
	//

};


//*********************************************************************************************
//
class CTestWall : public CTestThing
//
//	Prefix: wall
//
//	The CTestWall class is placeholder for the object and physics systems in the final game.
//	It represents a wall.
//
//*********************************************************************************************
{
public:

	//
	// Member variables.
	//

	CVector2<>	v2SecondPoint;	// The location and this point together define the
								// line segment that is a wall.

	//
	// Member function definitions.
	//

public:
	//****************************************************************************************
	//
	//	Constructors and destructor
	//

	CTestWall
	(
		TReal r_x, 
		TReal r_y

	) : CTestThing(r_x,r_y, 
					rptr_cast(CRenderType,rpsShapeWall))
	{
		// Make up a second point to define the line.
		SetSecondPoint(r_x, r_y);
	}


	//****************************************************************************************
	//
	//	Member functions.
	//

		//*********************************************************************************
		//
		void SetSecondPoint
		(
			TReal r_x, 
			TReal r_y
		)
		//
		//	Sets the second point to define the line.
		//
		//******************************
		{
			v2SecondPoint = CVector2<>(r_x, r_y);
		}

		//*********************************************************************************
		//
		bool bIntersects
		(
			const CLineSegment2<>& seg
		) const
		//
		//	Returns true if the wall intersects the line segment.
		//
		//******************************
		{
			CLineSegment2<> seg_me(v3Pos(), v2SecondPoint);
			return seg_me.bIntersects(seg);
		}
		

	//****************************************************************************************
	//
	//	Overrides.
	//
};


//*********************************************************************************************
//
template <EAIRefType eai, class SHAPE>
class CTestInanimate : public CTestThing
//
//	Prefix: ti
//
//*********************************************************************************************
{
public:
	//****************************************************************************************
	//
	//	Constructors and destructor
	//

	CTestInanimate
	(
		TReal r_x, 
		TReal r_y

	) : CTestThing(r_x, r_y, 
						rptr_cast(CRenderType,(    
						rptr_new SHAPE()
						)),
						eai)
	{
	}
};



//  ends #ifndef HEADER_TEST_AI_TESTTREE_HPP
#endif   
