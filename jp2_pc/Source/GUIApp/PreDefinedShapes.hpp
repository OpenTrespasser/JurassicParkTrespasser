/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
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
 * $Log:: /JP2_PC/Source/GUIApp/PreDefinedShapes.hpp                                          $
 * 
 * 12    97/07/23 18:25 Speter
 * Moved CInitStaticObjects to PreDefinedShapesInit.cpp.
 * 
 * 11    4/13/97 7:09p Cwalla
 * added sky msh.
 * 
 * 10    97/01/07 12:10 Speter
 * Made all CRenderTypes use rptr<>.  Changed all predefined meshes to allocated rptrs<>.
 * 
 * 
 * 9     96/12/12 12:17 Speter
 * Moved texture loading functions to new module, where they belong: Lib/Loader/LoadTexture.
 * 
 * 8     96/12/02 18:55 Speter
 * Changed bLoadTexture to work with CMesh's new shared surfaces.
 * Added a default parameter.
 * 
 * 7     10/10/96 2:41p Pkeet
 * Added menu item to add a test bumpmap object.
 * 
 * 6     96/10/04 18:51 Speter
 * Changed SMesh to CMesh, like it's sposta be.
 * 
 * 5     9/16/96 8:18p Gstull
 * Added project relative path to the include statements.
 * 
 * 4     96/09/09 18:35 Speter
 * Changed pMaterial* parameter in bLoadTexture to const.
 * 
 * 3     8/13/96 3:03p Pkeet
 * Change 'LoadTexture' function to 'bLoadTexture' function.
 * 
 * 2     8/09/96 3:26p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_GUINTERFACE_PREDEFINEDSHAPES_HPP
#define HEADER_GUINTERFACE_PREDEFINEDSHAPES_HPP

#include "Lib/GeomDBase/Mesh.hpp"

//
// Predefined shapes.
//

// Test cube.
extern rptr<CMesh> pmshCube;

// Directional light shape.
extern rptr<CMesh> pmshLightDir;

// Point light shape.
extern rptr<CMesh> pmshLightPt;

// Point directional light shape.
extern rptr<CMesh> pmshLightPtDir;

// Sky shape, well sort of.
extern rptr<CMesh> pmshSky;

//*********************************************************************************************
//
class CInitStaticObjects
//
// Init class to create all meshes.
//
//**************************************
{
public:
	CInitStaticObjects();
};

#endif
