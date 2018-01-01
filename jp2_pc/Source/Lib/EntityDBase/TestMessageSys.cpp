/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Module for testing the message and object systems.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/TestMessageSys.cpp                                    $
 * 
 * 25    3/11/97 3:21p Blee
 * Revamped trigger system.
 * 
 * 24    12/11/96 4:00p Mlange
 * Updated for the changes to the terrain query.
 * 
 * 23    96/12/04 20:33 Speter
 * A big change: Updated for new CPresence3<>, and CInstance functions.
 * 
 * 22    11/21/96 1:53p Blee
 * Take out SoundMessage.
 * 
 * 21    11/19/96 3:49p Mlange
 * Updated for CPlane name change.
 * 
 * 20    11/17/96 3:12p Pkeet
 * Did stuff.
 * 
 * 19    11/16/96 7:31p Pkeet
 * Did stuff.
 * 
 * 18    11/16/96 4:19p Pkeet
 * Removed the trigger test.
 * 
 * 17    11/16/96 4:18p Mlange
 * Updated for changes to world database query classes.
 * 
 * 16    11/15/96 6:38p Pkeet
 * Did stuff.
 * 
 * 15    11/13/96 8:04p Pkeet
 * Ditched code for file extension tests.
 * 
 * 14    96/11/09 10:00p Mlange
 * Now uses the world database query classes.
 * 
 * 13    11/05/96 9:26p Pkeet
 * File extension test.
 * 
 * 12    11/05/96 7:02p Pkeet
 * Test new terrain query container.
 * 
 * 11    96/11/05 3:50p Mlange
 * Renamed rGet() to tGet().
 * 
 * 10    11/01/96 2:14p Pkeet
 * Added #define switch to toggle the terrain query test on and off.
 * 
 * 9     11/01/96 2:04p Pkeet
 * Added a test function to test the terrain query mechanism.
 * 
 * 8     96/10/31 12:32p Mlange
 * Added some (commented-out) test code.
 * 
 * 7     96/10/25 14:41 Speter
 * Changed CEntityPresence to CEntity.
 * 
 * 6     96/10/24 6:05p Mlange
 * Some more test code.
 * 
 * 5     96/10/24 4:31p Mlange
 * Some more test code.
 * 
 * 4     96/10/23 6:42p Mlange
 * Commented out some test code.
 * 
 * 3     10/22/96 7:16p Pkeet
 * Added includes.
 * 
 * 2     10/22/96 2:33p Pkeet
 * Added the common.hpp include.
 * 
 * 1     10/22/96 2:30p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/


//
// Includes.
//
#include "Common.hpp"
#include "WorldDBase.hpp"
#include "Lib/EntityDBase/Message.hpp"
#include "Lib/EntityDBase/MessageTypes.hpp"
#include "Lib/EntityDBase/FrameHeap.hpp"
#include "Lib/EntityDBase/QueueMessage.hpp"
#include "Lib/EntityDBase/MessageLog.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Query.hpp"
#include "Lib/GeomDBase/Terrain.hpp"
#include "FilterIterator.hpp"
#include "Lib/Std/stringex.hpp"

#include "Lib/Renderer/Light.hpp"

//
// Defines.
//
#define bTEST_TERRAIN_QUERY false


class CEntityA : public CEntity
{
	int iA;

public:
	virtual void Process
	(
		const CMessageStep& msg
	)
	{
		// Do something with the message that has been passed.
		iA = 99;
	}

	virtual void Process
	(
		const CMessageMoveReq& msg
	)
	{
		// Do something with the message that has been passed.
		iA = 42;
	}
};


class CEntityB : public CEntity
{
	int iB;

public:
	virtual void Process
	(
		const CMessageStep& msg
	)
	{
		// Do something with the message that has been passed.
		iB = 33;
	}
};


class CEntityC : public CEntity
{
	int iC;

public:
};


CEntityA etA;
CEntityB etB;
CEntityC etC;


//*********************************************************************************************
void PrintPlane(CConsoleBuffer& con, const CPlane& pl)
{
	con.SetTabSize(15);
	con << "X: " << pl.d3Normal.tX << "\t";
	con << "Y: " << pl.d3Normal.tY << "\t";
	con << "Z: " << pl.d3Normal.tZ << "\t";
	con << "D: " << pl.rD << "\n";
}


//*********************************************************************************************
void TerrainQueryTest()
{
	conMessageLog << "\n\nTesting terrain query system...\n\n";

	CVector3<> v3(1.4, -23.3, 10);
	CBoundVolSphere bvs(v3, 1.4);

	// Get the results of a query.
	CWDbQueryTerrain wqpe(bvs);

	// Print the results of the query.
	for (wqpe; wqpe.bIsNotEnd(); wqpe++)
	{
		PrintPlane(conMessageLog, wqpe.tGet().plPlane);
	}

	conMessageLog << "\nFinished testing terrain query system!\n\n";
}


//**************
//#include "Lib/Std/Random.hpp"
#include "Lib/Transform/Vector.hpp"
//**************

void TestMessageSys()
{

	CMessageMoveTriggerTo
	(
		"Make Noise",
		CPlacement3<>(CVector3<>(0.0f, -0.4f, 0.0f))
	);

//**************
	/*
	CRandom rnd;

	CVector3<>* av3_points = new CVector3<>[1000];

	for (int i = 0; i < 1000; i++)
	{
		av3_points[i].tX = rnd(10.0, 17.0);
		av3_points[i].tY = rnd(10.0, 17.0);
		av3_points[i].tZ = rnd(10.0, 17.0);
	}

	av3_points[0] = CVector3<>(10, 10, 10);
	av3_points[1] = CVector3<>(17, 17, 17);


	CPArray< CVector3<> > pa_pts(1000, av3_points);

	CBoundingSphere bb(pa_pts);

	CVector3<> av3_test(99, 99, 99);

	Assert(bb.bContains(av3_test));
	*/

//**************

	if (bTEST_TERRAIN_QUERY)
		TerrainQueryTest();

	
//	trrTest.Query(v3, bs);

/*
	conMessageLog << "\n\nTesting messaging system...\n\n";

	qmQueueMessage.Play();
	conMessageLog << "\nPlaying messages for the first time...\n\n";

	// Send a step message to all.
	CMessageStep msgstep;
	msgstep.Dispatch();

	// Send a move request from C to A.
	CMessageMoveReq msgmvrq(&etC, &etA);
	msgmvrq.Dispatch();

	// Send a move request from C to B.
	CMessageMoveReq msgmvrq2(&etC, &etB);
	msgmvrq2.Dispatch();

	// Send a sound message from B to all.
	CMessageSound msgsnd2(&etB);
	msgsnd2.Dispatch();


	conMessageLog << "\nFlipping queues...\n\n";
	qmQueueMessage.Flip();

	conMessageLog << "\nPlaying messages from previous frame...\n\n";
	qmQueueMessage.Play();

	conMessageLog << "\nFinished Test!\n\n";
*/
}




