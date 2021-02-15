/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Definitions of simple 3d objects used for debugging purposes only.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Test3DObjs.hpp                                                   $
 * 
 * 24    97/09/28 3:10p Pkeet
 * Replace the use of 'CPresence3<>().v3Pos' with 'v3Pos().'
 * 
 * 23    97/02/03 16:04 Speter
 * Avoid rotation by zero vector.
 * 
 * 22    97/01/07 13:12 Speter
 * Made all CRenderTypes use rptr<>.  Updated for new rptr_new.
 * 
 * 21    12/30/96 6:33p Mlange
 * Updated for changes to the base library.
 * 
 * 20    96/12/04 20:28 Speter
 * A big change: Changed all CInstance placement accessors to pr3Presence().  Changed v3T to
 * v3Pos, r3T to r3Rot.
 * 
 * 19    11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 18    10/28/96 7:14p Pkeet
 * Removed the global light list.
 * 
 * 17    96/10/25 14:31 Speter
 * Updated for changes in CShape and CMesh.
 * 
 * 16    10/24/96 6:35p Pkeet
 * Removed the base world object.
 * 
 * 15    96/10/22 11:11 Speter
 * Changed CEntity to CPresence in appropriate contexts.
 * 
 * 14    96/10/04 18:51 Speter
 * Changed SMesh to CMesh, like it's sposta be.
 * 
 * 13    96/10/04 18:09 Speter
 * Updated for new CMesh structure.
 * Removed surface info in CEntityTest, and LoadTexture function.  Meshes can now be constructed
 * directly with surface info.
 * 
 * 12    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 11    9/20/96 9:46a Cwalla
 * added SMesh mshPyramid
 * 
 * 10    96/09/18 14:52 Speter
 * Updated constructors and initialisers for new CRPtr<>.
 * 
 * 
 * 9     96/08/06 18:25 Speter
 * Changes for hierarchical Entities.  Now all objects defined in hierarchy.  A single top-level
 * entity defines objects in scene.
 * 
 * 8     7/25/96 4:34p Mlange
 * Updated for deleted types such as CVector etc.
 * 
 * 7     96/07/23 16:32 Speter
 * Made LoadTexture choose a pix_solid value that is really the average colour.
 * 
 * 6     96/07/22 15:51 Speter
 * Changes for new render object format:
 * Added CEntityTest class, which has additional information for test program.
 * Added Lights.
 * Added a Cone object.
 * 
 * 5     6/28/96 3:15p Mlange
 * Added a single polygon mesh definition.
 * 
 * 4     96/06/18 10:08a Mlange
 * Updated for changes to 3d object definitions.
 * 
 * 3     31-05-96 10:10p Mlange
 * Added pyramid object.
 * 
 * 2     29-05-96 5:48p Mlange
 * Added #include.
 * 
 * 1     28-05-96 8:23p Mlange
 * Simple 3d object definitions for debugging.
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_TEST3DOBJS_HPP
#define HEADER_TEST_TEST3DOBJS_HPP

#include "Lib/EntityDBase/Entity.hpp"
#include "Lib/GeomDBase/Mesh.hpp"


class CEntityTest: public CEntity
// Prefix: ett.
// Add some additional fields.
{
public:
	CVector3<>	v3Velocity;			// Positional velocity (per frame).
	CVector3<>	v3RotateVelocity;	// Rotational velocity (per frame).

public:

	CEntityTest
	(
		const CInstance::SInit& initins
	)
		: CEntity(initins), v3Velocity(0,0,0), v3RotateVelocity(0,0,0)
	{
	}


	virtual void Process(const CMessageStep& msgstep) override
	{
		if (!v3RotateVelocity.bIsZero())
			pr3Presence().r3Rot *= v3RotateVelocity;
		CVector3<> v3 = v3Pos();
		v3 += v3Velocity;
		SetPos(v3);
	}
};


struct SMakeObjects
{
	CEntityTest* pettCube;
	CEntityTest* pettCone;
	CEntityTest* pettPyramid;
	CEntityTest* pettLight;

	rptr<CMesh> pmshCube;
	rptr<CMesh> pmshCone;
	rptr<CMesh> pmshPyramid;
	rptr<CMesh> pmshLight;


	SMakeObjects();
};


#endif

