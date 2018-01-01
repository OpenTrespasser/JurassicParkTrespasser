/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *		Definition of CPresence, a class for defining position, orientation and scale.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/ShapePresence.hpp                                        $
 * 
 * 2     6/04/98 11:42a Pkeet
 * Added the 'pr3GetCamWorld' member function. Added comments for member functions.
 * 
 * 1     6/03/98 11:08a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_SHAPEPRESENCE_HPP
#define HEADER_LIB_RENDERER_SHAPEPRESENCE_HPP


//**********************************************************************************************
//
class CShapePresence
//
// Prefix: sp
//
// Allows a presence to be calculated only once.
//
//**************************************
{
	// Calculated data members.
	bool         bWorldShape;
	bool         bCamShape;
	CPresence3<> pr3WorldShape;
	CPresence3<> pr3CamShape;

	// Stored external references.
	const CPresence3<>& pr3ShapeWorld;
	const CPresence3<>& pr3CamWorld;

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CShapePresence
	(
		const CPresence3<>& pr3_shape_world,
		const CPresence3<>& pr3_cam_world
	)
		: pr3ShapeWorld(pr3_shape_world), pr3CamWorld(pr3_cam_world), bWorldShape(false),
		  bCamShape(false)
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	const CPresence3<>& pr3GetShapeWorld
	(
	)
	//
	// Returns the shape to world presence.
	//
	//**************************************
	{
		return pr3ShapeWorld;
	}

	//******************************************************************************************
	//
	const CPresence3<>& pr3GetCamWorld
	(
	)
	//
	// Returns the camera to world presence.
	//
	//**************************************
	{
		return pr3CamWorld;
	}

	//******************************************************************************************
	//
	const CPresence3<>& pr3GetWorldShape
	(
	)
	//
	// Returns the world to shape presence.
	//
	//**************************************
	{
		if (!bWorldShape)
		{
			bWorldShape   = true;
			pr3WorldShape = ~pr3ShapeWorld;
		}
		return pr3WorldShape;
	}

	//******************************************************************************************
	//
	const CPresence3<>& pr3GetCamShape
	(
	)
	//
	// Returns the camera to shape presence.
	//
	//**************************************
	{
		if (!bCamShape)
		{
			bCamShape   = true;
			pr3CamShape = pr3CamWorld * pr3GetWorldShape();
		}

		// Transform, store and return the results.
		return pr3CamShape;
	}

};


#endif // HEADER_LIB_RENDERER_SHAPEPRESENCE_HPP