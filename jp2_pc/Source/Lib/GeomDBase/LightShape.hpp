/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998
 *
 * Contents:
 *		Pre-defined shape data for light shapes and a test shape.
 *
 * Bugs:
 *
 * To do:
 *
 * Notes:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/LightShape.hpp                                         $
 * 
 * 2     98.08.21 7:12p Mmouni
 * Added call the reset mesh heap.
 * 
 * 1     4/28/98 3:48p Agrant
 * this file exists so we can have this data in both Trespass and GUIApp
 * 
 *********************************************************************************************/

#ifndef HEADER_GEOMDBASE_LIGHTSHAPE_HPP
#define HEADER_GEOMDBASE_LIGHTSHAPE_HPP

#include "Lib/GeomDBase/Mesh.hpp"

//
// Predefined shapes.
//

// Set default light position and direction to match sound demo.
const CVector3<> v3DEFAULT_LIGHT_POS	(31.612, -23.302, 26.178);
const CVector3<> v3DEFAULT_LIGHT_TARGET	(-0.909,   0.974,  0.0577);


//*********************************************************************************************
//
// Useful texture coordinates.
//

const CTexCoord tc0(0.0f, 0.0f),
				tc1(0.0f, 1.0f),
				tc2(1.0f, 0.0f),
				tc3(1.0f, 1.0f),
				tcmid(0.5f, 0.5f);


//*********************************************************************************************
//
// Shape for Directional Light: Two intersecting triangles (2-sided).
//

//*********************************************************************************************
//
class CMeshLightDir: public CMesh
//
//
{
public:
	CMeshLightDir()
	{
		CHeap& mh_build = mhGetBuildHeap();

		// Build the data on the heap.

		mh_build.mav3Points
			<< CVector3<>(  0.0f,   0.0f,   0.5f)
			<< CVector3<>(  0.0f,   0.25f, -0.5f)
			<< CVector3<>(  0.25f,  0.0f,  -0.5f)
			<< CVector3<>(  0.0f,  -0.25f, -0.5f)
			<< CVector3<>( -0.25f,  0.0f,  -0.5f)
		;

		mh_build.mamvVertices
			<< SVertex(mh_build, 0, tc0) << SVertex(mh_build, 1, tc1) << SVertex(mh_build, 3, tc0)
			<< SVertex(mh_build, 0, tc0) << SVertex(mh_build, 3, tc0) << SVertex(mh_build, 1, tc1) 
			<< SVertex(mh_build, 0, tc0) << SVertex(mh_build, 2, tc3) << SVertex(mh_build, 4, tc3) 
			<< SVertex(mh_build, 0, tc0) << SVertex(mh_build, 4, tc3) << SVertex(mh_build, 2, tc3)
		;

		mh_build.mampPolygons
			<< SPolygon(mh_build, 0, 3)
			<< SPolygon(mh_build, 3, 3)
			<< SPolygon(mh_build, 6, 3)
			<< SPolygon(mh_build, 9, 3)
		;

		new(this) CMesh(mh_build);
		mh_build.Reset();
		CreateWrapBox();
	}
};




#endif