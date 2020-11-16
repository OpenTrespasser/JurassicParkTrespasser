/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Test child classes of CShapeInfo
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/AI/FakeShape.hpp                                                 $
 * 
 * 10    97/10/06 11:01 Speter
 * Updated for new CBoundVolBox interface.
 * 
 * 9     97/06/04 8:02p Pkeet
 * Changed the bounding volume from a sphere to a box.
 * 
 * 8     12/10/96 8:25p Agrant
 * Fixed boundvol for fake shapes.
 * 
 * 7     12/10/96 7:41p Agrant
 * Fixing AI Test App for code rot.
 * World DBase changes, mostly.
 * 
 * 6     11/25/96 12:30p Pkeet
 * Removed reference counting code.
 * 
 * 5     11/14/96 11:15p Agrant
 * Default test shape is now a cube- it should never be used in a renderer,
 * 
 * 4     11/14/96 4:49p Agrant
 * AI Test more in tune with the new object hierarchy
 * 
 * 3     11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 2     11/02/96 7:17p Agrant
 * Added a bunch of shapes
 * Added a bSelected accessor function
 * 
 * 1     10/31/96 7:25p Agrant
 * initial revision
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_AI_FAKESHAPE_HPP
#define HEADER_TEST_AI_FAKESHAPE_HPP

#include "Lib/EntityDBase/Instance.hpp"

#include "Lib/GeomDBase/Shape.hpp"
#include "Lib/GeomDBase/Mesh.hpp"
#include "GUIApp/PreDefinedShapes.hpp"


#include "Lib/Sys/Timer.hpp"

//**********************************************************************************************
//
class CTestShapeInfo : public CMesh
//
// Prefix: si
//
// Fake class holding rendering info.
//
//	Notes:
//		This class takes the place of the real game CShapeInfo, but only in the 
//		AI Test App.
//
//**************************************
{
public:

//	bool	bSelected;		// True if object is currently selected.

	//******************************************************************************************
	//
	// Constructors.
	//
	CTestShapeInfo() : CMesh()
	{
		bvbVolume = CBoundVolBox(2.0f, 2.0f, 2.0f);
	}




	//******************************************************************************************
	//
	// Member functions.
	//

		//*********************************************************************************
		//
		virtual void Draw
		(
			CInstance*	pins	// The object that is being drawn.
		) const;
		//
		//	Draws the object at current location with "this" shape.
		//
		//******************************


	//******************************************************************************************
	//
	// Overrides.
	//
};


//**********************************************************************************************
//
class CShapeTree : public CTestShapeInfo
//
// Prefix: st
//
// Fake class holding rendering info for trees.
//
//
//**************************************
{
	//******************************************************************************************
	//
	// Overrides.
	//

		//*********************************************************************************
		//
		virtual void Draw
		(
			CInstance*	pins	// The object that is being drawn.
		) const;
		//
		//	Draws the object at current location with "this" shape.
		//
		//******************************
};

//**********************************************************************************************
//
class CShapeWall : public CTestShapeInfo
//
// Prefix: st
//
// Fake class holding rendering info for walls.
//
//
//**************************************
{
	//******************************************************************************************
	//
	// Overrides.
	//

		//*********************************************************************************
		//
		virtual void Draw
		(
			CInstance*	pins	// The object that is being drawn.
		) const;
		//
		//	Draws the object at current location with "this" shape.
		//
		//******************************
};


//**********************************************************************************************
//
class CShapeDinosaur : public CTestShapeInfo
//
// Prefix: sd
//
// Fake class holding rendering info for dinos.
//
//
//**************************************
{
	//******************************************************************************************
	//
	// Overrides.
	//

		//*********************************************************************************
		//
		virtual void Draw
		(
			CInstance*	pins	// The object that is being drawn.
		) const;
		//
		//	Draws the object at current location with "this" shape.
		//
		//******************************
};

//**********************************************************************************************
//
class CShapeSheep : public CTestShapeInfo
//
// Prefix: ss
//
// Fake class holding rendering info for sheep.
//
//
//**************************************
{
	//******************************************************************************************
	//
	// Overrides.
	//

		//*********************************************************************************
		//
		virtual void Draw
		(
			CInstance*	pins	// The object that is being drawn.
		) const;
		//
		//	Draws the object at current location with "this" shape.
		//
		//******************************

};


//**********************************************************************************************
//
class CShapeWolf : public CTestShapeInfo
//
// Prefix: sw
//
// Fake class holding rendering info for wolves.
//
//
//**************************************
{
	//******************************************************************************************
	//
	// Overrides.
	//

		//*********************************************************************************
		//
		virtual void Draw
		(
			CInstance*	pins	// The object that is being drawn.
		) const;
		//
		//	Draws the object at current location with "this" shape.
		//
		//******************************
};


//**********************************************************************************************
//
class CShapeMeat : public CTestShapeInfo
//
// Prefix: sm
//
// Fake class holding rendering info for meat.
//
//
//**************************************
{
	//******************************************************************************************
	//
	// Overrides.
	//

		//*********************************************************************************
		//
		virtual void Draw
		(
			CInstance*	pins	// The object that is being drawn.
		) const;
		//
		//	Draws the object at current location with "this" shape.
		//
		//******************************
};

//**********************************************************************************************
//
class CShapeGrass : public CTestShapeInfo
//
// Prefix: sd
//
// Fake class holding rendering info for grass.
//
//
//**************************************
{
	//******************************************************************************************
	//
	// Overrides.
	//

		//*********************************************************************************
		//
		virtual void Draw
		(
			CInstance*	pins	// The object that is being drawn.
		) const;
		//
		//	Draws the object at current location with "this" shape.
		//
		//******************************
};


//#ifndef HEADER_TEST_AI_FAKESHAPE_HPP
#endif
