/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of PreDefinedShapes.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/PreDefinedShapes.cpp                                           $
 * 
 * 45    98.08.21 7:12p Mmouni
 * Added call the reset mesh heap.
 * 
 * 44    12/17/97 3:40p Agrant
 * Replaced stdafx.h with common.hpp
 * stdafx.h was needed for precompiled headers, which will not be needed now that predfined
 * shapes is in the geomdbase lib
 * 
 * 43    97/10/15 22:14 Speter
 * Changed CMeshCube to CMeshBox, moved to Mesh module.
 * 
 * 42    8/18/97 3:59p Agrant
 * Now initializing the static objects in PredefinedShapes dyanically to avoid
 * intitialization order dependencies.
 * 
 * 41    97/07/24 13:57 Speter
 * Put InitStaticObjects back here, dispensed with separate Init module.
 * 
 * 40    97/07/23 18:26 Speter
 * Changed all shape constructors to use new CHeap mesh constructor.  Moved InitStaticObjects to
 * PreDefinedShapesInit.cpp.
 * 
 * 39    97/07/09 16:36 Speter
 * Changed SPolygon() constructor to take a CPArray.
 * 
 * 38    97/07/07 4:07p Pkeet
 * Added bCreateWrap functions for all the predefined meshes.
 * 
 * 37    97/05/29 16:37 Speter
 * Moved pmshSky creation inside CSky.
 * 
 * 36    97/05/25 18:01 Speter
 * Changed all mesh initialisers into specialised class constructors, updated for new CMesh
 * structure
 * 
 * 35    97/05/21 18:12 Speter
 * Updated all shapes to work with new CMesh polygonal structure.
 * 
 * 34    4/13/97 7:11p Cwalla
 * Added sky mesh.
 * 
 * 33    1/10/97 6:34p Pkeet
 * Added an init class for static objects to allow compilation under Visual C++ 5.0.
 * 
 * 32    97/01/07 12:10 Speter
 * Made all CRenderTypes use rptr<>.  Changed all predefined meshes to allocated rptrs<>.
 * 
 * 
 * 31    96/12/12 12:17 Speter
 * Moved texture loading functions to new module, where they belong: Lib/Loader/LoadTexture.
 * 
 * 30    96/12/12 11:41 Speter
 * Fixed bug which failed to detect transparency!!!
 * 
 * 29    96/12/02 18:55 Speter
 * Changed bLoadTexture to work with CMesh's new shared surfaces.
 * Added a default parameter.
 * 
 * 28    96/11/25 17:36 Speter
 * Changed dimensions of mshLightPt to be near 1.
 * 
 * 27    96/11/13 18:36 Speter
 * Changed LoadColour to use new CTexture constructor.
 * 
 * 26    11/07/96 3:35p Pkeet
 * Added the 'ApplyCurves' function to the mesh loader.
 * 
 * 25    11/06/96 11:59a Gstull
 * Added 1 to the string length
 * 
 * 24    11/05/96 10:13p Pkeet
 * Added a toggle for transparency.
 * 
 * 23    11/05/96 9:29p Pkeet
 * Added facility for the LoadTexture function to automatically load a bumpmap based on the
 * extension.
 * 
 * 22    11/05/96 7:06p Pkeet
 * Turned the transparent #define off, but pass a 'true' for transparency through the SSurface
 * constructor.
 * 
 * 21    11/05/96 5:33p Pkeet
 * Automatically initializes the bumpmapped object with transparency.
 * 
 * 20    10/10/96 2:41p Pkeet
 * Added menu item to add a test bumpmap object.
 * 
 * 19    96/10/09 16:44 Speter
 * Added hacky code which tries to find textures in directories other than their absolute path
 * name.
 * 
 * 18    96/10/04 18:51 Speter
 * Changed SMesh to CMesh, like it's sposta be.
 * 
 * 17    96/10/04 18:12 Speter
 * Updated for new CMesh structure.
 * 
 * 16    96/09/26 16:17 Speter
 * Added temporary check when loading textures to load only unique textures.  Makes a rather
 * huge difference.
 * Put back constructor which omits solid pixel, because now average pixel calculation is
 * faster.
 * 
 * 15    96/09/25 19:49 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 14    9/24/96 2:48p Pkeet
 * Added the 'FastBump.hpp' include.
 * 
 * 13    96/09/23 17:01 Speter
 * Moved average pixel calculation code from here to Texture.cpp.
 * 
 * 12    96/09/18 14:51 Speter
 * Updated constructors and initialisers for new CRPtr<>.
 * 
 * 11    9/16/96 8:07p Gstull
 * Successfully merged between bumpmapping version and loader version.
 * 
 * 10    96/09/16 12:26 Speter
 * Updated for simplified CBumpMap structure, and for CRefPtr<CTexture>.
 * 
 * 9     9/11/96 11:25a Pkeet
 * Added a bumpmap loader. Currently defaults to bumpmapping.
 * 
 * 8     96/09/09 18:35 Speter
 * Changed pMaterial* parameter in bLoadTexture to const.
 * 
 * 7     96/09/05 11:36 Speter
 * Changed prasvReadBMP back to prasReadBMP.
 * 
 * 6     8/22/96 6:23p Pkeet
 * Added 'MAKE_TRANSPARENT' macro.
 * 
 * 5     8/22/96 3:46p Pkeet
 * Temporarily created transparent portions to texturemaps.
 * 
 * 4     96/08/15 19:01 Speter
 * Now call prasvReadBMP.
 * 
 * 3     8/13/96 3:03p Pkeet
 * Change 'LoadTexture' function to 'bLoadTexture' function.
 * 
 * 2     8/09/96 3:53p Pkeet
 * Initial implemenation.
 * 
 **********************************************************************************************/

//#include "stdafx.h"
#include "Common.hpp"
#include "PreDefinedShapes.hpp"


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

rptr<CMesh> pmshLightDir;

//*********************************************************************************************
//
// Shape for Point Light: tetrahedron.
//

//*********************************************************************************************
//
class CMeshLightPt: public CMesh
//
//
{
public:
	CMeshLightPt()
	{
		CHeap& mh_build = mhGetBuildHeap();

		// Build the data on the heap.

		mh_build.mav3Points
			<< CVector3<>(  0.0f,   0.0f,   0.87f)
			<< CVector3<>(  0.0f,   0.87f, -0.5f)
			<< CVector3<>(  0.87f, -0.5f,  -0.5f)
			<< CVector3<>( -0.87f, -0.5f,  -0.5f)
		;

		mh_build.mamvVertices
			<< SVertex(mh_build, 2, tc3) << SVertex(mh_build, 1, tc1) << SVertex(mh_build, 0, tc0)
			<< SVertex(mh_build, 3, tc0) << SVertex(mh_build, 2, tc3) << SVertex(mh_build, 0, tc0)
			<< SVertex(mh_build, 1, tc1) << SVertex(mh_build, 3, tc0) << SVertex(mh_build, 0, tc0)
			<< SVertex(mh_build, 1, tc1) << SVertex(mh_build, 2, tc3) << SVertex(mh_build, 3, tc0)
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

rptr<CMesh> pmshLightPt;

//*********************************************************************************************
//
// Shape for Point Directional Light.
//

//*********************************************************************************************
//
class CMeshLightPtDir: public CMesh
//
//
{
public:
	CMeshLightPtDir()
	{
		CHeap& mh_build = mhGetBuildHeap();

		// Build the data on the heap.

		mh_build.mav3Points
			<< CVector3<>(  0.0f,   0.0f,   1.0f)
			<< CVector3<>(  0.0f,   0.25f, -0.5f)
			<< CVector3<>(  0.25f,  0.0f,  -0.5f)
			<< CVector3<>(  0.0f,  -0.25f, -0.5f)
			<< CVector3<>( -0.25f,  0.0f,  -0.5f)

			<< CVector3<>(  0.0f,   0.0f,   0.5f)
			<< CVector3<>(  0.0f,   0.43f, -0.5f)
			<< CVector3<>(  0.43f, -0.5f,  -0.5f)
			<< CVector3<>( -0.43f, -0.5f,  -0.5f)
		;

		mh_build.mamvVertices
			<< SVertex(mh_build, 0, tc0) << SVertex(mh_build, 1, tc1) << SVertex(mh_build, 3, tc0)
			<< SVertex(mh_build, 0, tc0) << SVertex(mh_build, 3, tc0) << SVertex(mh_build, 1, tc1)
			<< SVertex(mh_build, 0, tc0) << SVertex(mh_build, 2, tc3) << SVertex(mh_build, 4, tc3)
			<< SVertex(mh_build, 0, tc0) << SVertex(mh_build, 4, tc3) << SVertex(mh_build, 2, tc3)

			<< SVertex(mh_build, 7, tc3) << SVertex(mh_build, 6, tc1) << SVertex(mh_build, 5, tc0)
			<< SVertex(mh_build, 8, tc0) << SVertex(mh_build, 7, tc3) << SVertex(mh_build, 5, tc0)
			<< SVertex(mh_build, 6, tc1) << SVertex(mh_build, 8, tc0) << SVertex(mh_build, 5, tc0)
			<< SVertex(mh_build, 6, tc1) << SVertex(mh_build, 7, tc3) << SVertex(mh_build, 8, tc0)
		;

		mh_build.mampPolygons
			<< SPolygon(mh_build, 0, 3)
			<< SPolygon(mh_build, 3, 3)
			<< SPolygon(mh_build, 6, 3)
			<< SPolygon(mh_build, 9, 3)

			<< SPolygon(mh_build, 12, 3)
			<< SPolygon(mh_build, 15, 3)
			<< SPolygon(mh_build, 18, 3)
			<< SPolygon(mh_build, 21, 3)
		;

		new(this) CMesh(mh_build);
		mh_build.Reset();
		CreateWrapBox();
	}
};

rptr<CMesh> pmshLightPtDir;

//*********************************************************************************************
rptr<CMesh> pmshCube;

//*********************************************************************************************
//
// class CInitStaticObjects implementation.
//
//**************************************

	CInitStaticObjects::CInitStaticObjects()
	{
		pmshLightDir	= rptr_cast(CMesh, rptr_new CMeshLightDir());
		pmshLightPt		= rptr_cast(CMesh, rptr_new CMeshLightPt());
		pmshLightPtDir	= rptr_cast(CMesh, rptr_new CMeshLightPtDir());
		pmshCube		= rptr_cast(CMesh, rptr_new CMeshBox());
	}

	// Do this dynamically at program start, please.  That way we avoid construction order problems.
	// You'll find this in the GUIApp constructor code.
// Just declare the init object for the predefined shapes.
//static CInitStaticObjects InitStaticObjects;
