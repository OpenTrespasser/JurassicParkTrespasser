/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Test Query classes for the world database, used only in the AI Test App.
 *
 * Bugs:
 *
 * To do:
 *		Remove the dummy class definition when the compiler bug has been fixed.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/AI/QueryTest.hpp                                                 $
 * 
 * 15    9/16/98 12:36a Agrant
 * more code rot
 * 
 * 14    5/09/98 11:17a Agrant
 * #include commander
 * 
 * 13    12/17/97 3:41p Agrant
 * AI Test code rot repairs
 * 
 * 12    97/08/01 16:09 Speter
 * Updated for new param to CWdbQueryShapes (currently 'true' to include terrain).
 * 
 * 11    4/30/97 9:19p Rwyatt
 * Moved esfView member variable from CPartition and put it into the list that the partition
 * functions return. These functions used to return a list of CPartition* now they return a list
 * of structures that contain a CPartition* and esfSideOf.
 * 
 * 10    3/15/97 5:05p Agrant
 * Updated for spatial partition changes
 * 
 * 9     97/03/14 12:31p Pkeet
 * Changed references to 'ppartsSpace' to 'ppartPartitions.'
 * 
 * 8     97/03/08 7:15p Pkeet
 * Removed container include.
 * 
 * 7     97/03/04 5:14p Pkeet
 * Changed query interface to use a partition object instead of discrete presences and bounding
 * volumes.
 * 
 * 6     2/05/97 8:07p Agrant
 * Added wall query class.
 * 
 * 5     1/07/97 6:50p Agrant
 * rptr's and other random structure changes integrated into test app.
 * 
 * 4     97/01/07 11:18 Speter
 * Put all CRenderTypes in rptr<>.
 * 
 * 3     12/10/96 7:41p Agrant
 * Fixing AI Test App for code rot.
 * World DBase changes, mostly.
 * 
 * 2     11/17/96 5:25p Agrant
 * Fixed for new query class API.
 * 
 * 1     11/14/96 2:40p Agrant
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_AI_QUERYTEST_HPP
#define HEADER_TEST_AI_QUERYTEST_HPP


//#include <list.h>
//#include "Lib/GeomDBase/PartitionSpace.hpp"

#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "Lib/GeomDBase/PartitionSpace.hpp"


//#include "Lib/EntityDBase/Query.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"

//#include "Lib/Renderer/GeomTypes.hpp"


#include "FakeShape.hpp"
#include "TestAnimal.hpp"
#include "TestTree.hpp"



class CTest;
class CTestWall;

//*********************************************************************************************
//
// Forward declarations for the query classes.
//


//*********************************************************************************************
//
class CWDbQueryTestShape : CContainer< list<CInstance*> >
//class CWDbQueryTestShape : list<CInstance*>
//
// World database query container.
//
// Prefix: cqa
//
// Notes:
//		Calling the constructor for this object will instantiate it with a list of CInstances
//		which all have Test Shapes.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//


	CWDbQueryTestShape()
	{
		CWDbQueryRenderTypes cqs;
		
		foreach(cqs)
		{
			CInstance*	pins;


			(cqs.tGet().ppart)->Cast(&pins);

			Assert(pins!=0);

			// Add the shape if it intersects the given bounding volume
			if (rptr_const_dynamic_cast(CTestShapeInfo, pins->prdtGetRenderInfo()))
			{	
				push_back(pins);
			}
		}

		Begin();
	}

	CWDbQueryTestShape(const CBoundVol& bv, const CWorld& w = wWorld)
	{
		CPartitionSpace parts(bv);

		CWDbQueryRenderTypes cqs(&parts, true, w);
		
		foreach(cqs)
		{
			CInstance* pins;

			cqs.tGet().ppart->Cast(&pins);
			// Add the shape if it intersects the given bounding volume.
			if (rptr_const_dynamic_cast(CTestShapeInfo, pins->prdtGetRenderInfo()))
			{	
				push_back(pins);
			}
		}

		Begin();
	}
};

//*********************************************************************************************
//
class CWDbQueryTest : public CContainer< list<CTest*> >
//class CWDbQueryTest : public list<CTest*>
//
// World database query container.
//
// Prefix: cqa
//
// Notes:
//		Calling the constructor for this object will instantiate it with a list of CInstances
//		which are also AI Test objects.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//

	CWDbQueryTest()
	{
		CWDbQueryRenderTypes cqs;

		foreach(cqs)
		{
			CInstance* pins;

			cqs.tGet().ppart->Cast(&pins);

			CTest *ptest = dynamic_cast<CTest*>(pins);
			if (ptest)
			{	
				push_back(ptest);
			}
		}

		Begin();
	}

	CWDbQueryTest(const CBoundVol& bv, const CWorld& w = wWorld)
	{
		CPartitionSpace parts(bv);

		CWDbQueryRenderTypes cqs(&parts, true, w);

		foreach(cqs)
		{
			CTest *ptest = dynamic_cast<CTest*>(cqs.tGet().ppart);
			if (ptest)
			{	
					push_back(ptest);
			}
		}

		Begin();
	}

};


//*********************************************************************************************
//
class CWDbQueryTestWall : public CContainer< list<CTestWall*> >
//class CWDbQueryTestWall : public list<CTestWall*>
//
// World database query container.
//
// Prefix: cqtw
//
// Notes:
//		Calling the constructor for this object will instantiate it with a list of CInstances
//		which are also AI Test Walls.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructors and destructor.
	//


	CWDbQueryTestWall()
	{
		CWDbQueryRenderTypes cqs;

		foreach(cqs)
		{
			CTestWall *ptestw = dynamic_cast<CTestWall*>(cqs.tGet().ppart);
			if (ptestw)
			{	
					push_back(ptestw);
			}
		}

		Begin();
	}

	CWDbQueryTestWall(const CBoundVol& bv, const CWorld& w = wWorld)
	{
		CPartitionSpace parts(bv);

		CWDbQueryRenderTypes cqs(&parts, true, w);

		foreach(cqs)
		{
			CTestWall *ptestw = dynamic_cast<CTestWall*>(cqs.tGet().ppart);
			if (ptestw)
			{	
					push_back(ptestw);
			}
		}

		Begin();
	}

};




//*********************************************************************************************
//
template<class T> class CDummy_HEADER_LIB_ENTITYDBASE_QUERY_HPP_ICKY_ICKY_ICKY
//
// Dummy class declaration to get around a compiler bug.
//
// Prefix:
//
//**************************************
{
};


//#ifndef HEADER_TEST_AI_QUERYTEST_HPP
#endif
