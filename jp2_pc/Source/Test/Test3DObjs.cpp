/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Test3DObjs.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/Test3DObjs.cpp                                                   $
 * 
 * 41    97/03/18 19:22 Speter
 * Replaced inefficient CDir3<>('z') with d3ZAxis.
 * 
 * 40    97/01/26 20:01 Speter
 * Changed matPLASTIC to matSHINY.
 * 
 * 39    97/01/07 13:12 Speter
 * Made all CRenderTypes use rptr<>.  Updated for new rptr_new.
 * 
 * 37    12/30/96 6:33p Mlange
 * Updated for changes to the base library.
 * 
 * 36    10/28/96 7:14p Pkeet
 * Removed the global light list.
 * 
 * 35    10/24/96 6:35p Pkeet
 * Removed the base world object.
 * 
 * 34    96/10/22 11:11 Speter
 * Changed CEntity to CPresence in appropriate contexts.
 * 
 * 33    96/10/04 18:51 Speter
 * Changed SMesh to CMesh, like it's sposta be.
 * 
 * 32    96/10/04 18:09 Speter
 * Updated for new CMesh structure.
 * Removed surface info in CEntityTest, and LoadTexture function.  Meshes can now be constructed
 * directly with surface info.
 * 
 * 31    96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 30    96/09/23 17:04 Speter
 * Moved calculation of average pixel value from here to Texture.hpp.
 * 
 * 29    96/09/18 19:43 Speter
 * Replaced the infamous cone data with a class that constructs one dynamically, with settable
 * number of sides.  Looks much better with 16 sides.
 * 
 * 28    96/09/18 14:52 Speter
 * Updated constructors and initialisers for new CRPtr<>.
 * 
 * 27    96/09/16 15:32 Speter
 * Fixed for CRefPtr<CTexture>.
 * 
 * 26    8/23/96 10:38a Gstull
 * Changed prasvReadBMP() back to prasReadBMP().  Now creates a CRasterVid only if D3D support
 * exists.
 * 
 * 25    96/08/21 18:59 Speter
 * Replaced d3ZAxis with CDir3('z').
 * 
 * 24    96/08/19 13:09 Speter
 * Changed deformed cone back to simpler, yet still wrong, version, with no degenerate
 * triangles.
 * 
 * 23    96/08/15 20:55 Speter
 * Added cool new textures.
 * Fixed texture coordinates to include entire texture.
 * 
 * 22    96/08/15 19:01 Speter
 * Now call new prasvReadBMP function.
 * Lock() and Unlock() before reading texture.
 * 
 * 21    96/08/06 18:25 Speter
 * Changes for hierarchical Entities.  Now all objects defined in hierarchy.  A single top-level
 * entity defines objects in scene.
 * 
 * 20    96/07/29 11:30 Speter
 * Changed cone back to a cone, changed texture maps.
 * 
 * 19    7/26/96 6:37p Mlange
 * Updated for CMeshTriangle name change. Now performs CMesh::SetSurfaceAll here.
 * 
 * 18    7/25/96 4:33p Mlange
 * Updated for CObjPoint name change.
 * 
 * 17    96/07/25 15:15 Speter
 * 
 * 16    96/07/23 16:32 Speter
 * Made LoadTexture choose a pix_solid value that is really the average colour.
 * 
 * 15    96/07/22 15:51 Speter
 * Changes for new render object format:
 * Added CEntityTest class, which has additional information for test program.
 * Added Lights.
 * Added a Cone object.
 * 
 * 14    7/18/96 6:59p Mlange
 * Updated for CVertex to CPoint name change.
 * 
 * 13    7/11/96 5:59p Mlange
 * Updated for changes to object definition.
 * 
 * 12    7/10/96 12:42p Mlange
 * Updated for changes to the object definitions.
 * 
 * 11    7/08/96 8:47p Mlange
 * Modified single polygon object.
 * 
 * 10    6/28/96 3:15p Mlange
 * Added a single polygon mesh definition.
 * 
 * 9     6/26/96 12:30p Mlange
 * Updated for changes to 3d object definitions.
 * 
 * 8     96/06/18 10:07a Mlange
 * Updated for changes to 3d object definitions.
 * 
 * 7     6-06-96 11:41a Mlange
 * Updated for change in coordinate system.
 * 
 * 6     5-06-96 1:10p Mlange
 * Updated cube for changes in object definition.
 * 
 * 5     5-06-96 12:02p Mlange
 * Updated pyramid definition for changes in object data structure. Commented out cube
 * definition.
 * 
 * 4     31-05-96 10:10p Mlange
 * Added pyramid object.
 * 
 * 3     29-05-96 5:44p Mlange
 * Offsets are now defined by the size of SRenderCoord.
 * 
 * 2     29-05-96 4:58p Mlange
 * Corrected plane equation definitions.
 * 
 * 1     28-05-96 8:23p Mlange
 * Simple 3d object definitions for debugging.
 * 
 **********************************************************************************************/

#include "GblInc/Common.hpp"
#include "Test3DObjs.hpp"

#include "Lib/View/RasterVid.hpp"


//**********************************************************************************************
const CTexCoord tc0(0.0f, 0.0f),
				tc1(0.0f, 1.0f),
				tc2(1.0f, 0.0f),
				tc3(1.0f, 1.0f),
				tcmid(0.50f, 0.50f);


//**********************************************************************************************
CVector3<> av3Cube[] =
{
	CVector3<>( 1.0f, -1.0f,  1.0f),
	CVector3<>(-1.0f, -1.0f,  1.0f),
	CVector3<>(-1.0f, -1.0f, -1.0f),
	CVector3<>( 1.0f, -1.0f, -1.0f),

	CVector3<>( 1.0f,  1.0f,  1.0f),
	CVector3<>( 1.0f,  1.0f, -1.0f),
	CVector3<>(-1.0f,  1.0f, -1.0f),
	CVector3<>(-1.0f,  1.0f,  1.0f),
};

CMesh::SVertex amvCube[] =
{
	CMesh::SVertex(0, tc0), CMesh::SVertex(1, tc1), CMesh::SVertex(2, tc3),
	CMesh::SVertex(0, tc0), CMesh::SVertex(2, tc3), CMesh::SVertex(3, tc2),

	CMesh::SVertex(0, tc0), CMesh::SVertex(3, tc1), CMesh::SVertex(5, tc3),
	CMesh::SVertex(0, tc0), CMesh::SVertex(5, tc3), CMesh::SVertex(4, tc2),

	CMesh::SVertex(4, tc0), CMesh::SVertex(5, tc1), CMesh::SVertex(6, tc3),
	CMesh::SVertex(4, tc0), CMesh::SVertex(6, tc3), CMesh::SVertex(7, tc2),

	CMesh::SVertex(2, tc0), CMesh::SVertex(1, tc1), CMesh::SVertex(7, tc3),
	CMesh::SVertex(2, tc0), CMesh::SVertex(7, tc3), CMesh::SVertex(6, tc2),

	CMesh::SVertex(1, tc0), CMesh::SVertex(0, tc1), CMesh::SVertex(4, tc3),
	CMesh::SVertex(1, tc0), CMesh::SVertex(4, tc3), CMesh::SVertex(7, tc2),

	CMesh::SVertex(3, tc0), CMesh::SVertex(2, tc1), CMesh::SVertex(6, tc3),
	CMesh::SVertex(3, tc0), CMesh::SVertex(6, tc3), CMesh::SVertex(5, tc2),
};


CMesh::STriangle amtriCube[] =
{
	CMesh::STriangle(0, 1, 2),
	CMesh::STriangle(3, 4, 5),
	CMesh::STriangle(6, 7, 8),
	CMesh::STriangle(9, 10, 11),
	CMesh::STriangle(12, 13, 14),
	CMesh::STriangle(15, 16, 17),
	CMesh::STriangle(18, 19, 20),
	CMesh::STriangle(21, 22, 23),
	CMesh::STriangle(24, 25, 26),
	CMesh::STriangle(27, 28, 29),
	CMesh::STriangle(30, 31, 32),
	CMesh::STriangle(33, 34, 35)
};

CPArray< CVector3<> >     pav3Cube(  iNumArrayElements(av3Cube),   av3Cube);
CPArray<CMesh::SVertex>   pamvCube(  iNumArrayElements(amvCube),   amvCube);
CPArray<CMesh::STriangle> pamtriCube(iNumArrayElements(amtriCube), amtriCube);




//**********************************************************************************************
#define v2Circle(radius, angle)	\
	(angConeSide*angle).fCos() * radius, (angConeSide*angle).fSin() * radius

struct CMeshCone: public CMesh
{
	CMeshCone(TReal r_radius, TReal r_height, uint u_sides = 16, SSurface sf = SSurface())
	{
		pav3Points = CPArray< CVector3<> >(u_sides + 2);
		pamvVertices = CPArray<CMesh::SVertex>(3*u_sides + 1);
		pamtTriangles = CPArray<CMesh::STriangle>(2*u_sides);

		CAngle ang_side = d2_PI / (float)u_sides;
		uint u;

		//
		// Fill the points.
		//

		// Top and bottom points.
		pav3Points[0] = CVector3<>(0, 0,  r_height / 2.0);
		pav3Points[1] = CVector3<>(0, 0, -r_height / 2.0);

		// Side points.
		for (u = 0; u < u_sides; u++)
			pav3Points[2 + u] = CVector3<>((ang_side*(int)u).fCos(), (ang_side*(int)u).fSin(), -r_height / 2.0);

		//
		// Fill the vertices.
		//

		// Tip top vertices.
		for (u = 0; u < u_sides; u++)
			pamvVertices[u] = CMesh::SVertex(0, tcmid);

		// Side bottom, and bottom, vertices.
		for (u = 0; u < u_sides; u++)
		{
			pamvVertices[u_sides + u] = pamvVertices[2*u_sides + u] = CMesh::SVertex
			(
				2 + u, CTexCoord
				(
					pav3Points[2+u].tX / 2.0f + 0.5f, 
					pav3Points[2+u].tY / 2.0f + 0.5f
				)
			);
		}

		// Bottom center vertex.
		pamvVertices[3*u_sides] = CMesh::SVertex(1, tcmid);

		//
		// Fill the triangles.
		//

		for (u = 0; u < u_sides; u++)
		{
			// Sides of cone.
			pamtTriangles[u] = CMesh::STriangle
			(
				u, u_sides + u, u_sides + (u+1) % u_sides
			);

			// Bottom of cone.
			pamtTriangles[u_sides+u] = CMesh::STriangle
			(
				3*u_sides, 2*u_sides + (u+1) % u_sides, 2*u_sides + u
			);
		}

		new(this) CMesh(pav3Points, pamvVertices, pamtTriangles, sf);
	}
};


//**********************************************************************************************
CVector3<> av3Pyramid[] =
{
	CVector3<>( 0.0f,  0.0f,  1.0f),
	CVector3<>(-1.0f, -1.0f, -1.0f),
	CVector3<>( 1.0f, -1.0f, -1.0f),
	CVector3<>( 1.0f,  1.0f, -1.0f),
	CVector3<>(-1.0f,  1.0f, -1.0f),
};

CMesh::SVertex amvPyramid[] =
{
	CMesh::SVertex(0, tcmid), CMesh::SVertex(1, tc0), CMesh::SVertex(2, tc1),
	CMesh::SVertex(0, tcmid), CMesh::SVertex(2, tc1), CMesh::SVertex(3, tc3),
	CMesh::SVertex(0, tcmid), CMesh::SVertex(3, tc3), CMesh::SVertex(4, tc2),
	CMesh::SVertex(0, tcmid), CMesh::SVertex(4, tc2), CMesh::SVertex(1, tc0),

	CMesh::SVertex(4, tc2), CMesh::SVertex(3, tc3), CMesh::SVertex(2, tc1),
	CMesh::SVertex(4, tc2), CMesh::SVertex(2, tc1), CMesh::SVertex(1, tc0),
};

CMesh::STriangle amtriPyramid[] =
{
	CMesh::STriangle(0, 1, 2),
	CMesh::STriangle(3, 4, 5),
	CMesh::STriangle(6, 7, 8),
	CMesh::STriangle(9, 10, 11),

	CMesh::STriangle(12, 13, 14),
	CMesh::STriangle(15, 16, 17),
};

CPArray< CVector3<> >    pav3Pyramid(iNumArrayElements(av3Pyramid),		av3Pyramid);
CPArray<CMesh::SVertex>  pamvPyramid(iNumArrayElements(amvPyramid),		amvPyramid);
CPArray<CMesh::STriangle> pamtriPyramid(iNumArrayElements(amtriPyramid),	amtriPyramid);


//**********************************************************************************************
CVector3<> av3Light[] =
{
	CVector3<>( 0.0f, 0.0f,  0.0f),
	CVector3<>( 0.5f, 0.0f, -1.0f),
	CVector3<>(-0.5f, 0.0f, -1.0f),
};

CMesh::SVertex amvLight[] =
{
	CMesh::SVertex(0), CMesh::SVertex(1), CMesh::SVertex(2)
};


CMesh::STriangle amtriLight[] =
{
	CMesh::STriangle(0, 1, 2),
};


CPArray< CVector3<> >    pav3Light(iNumArrayElements(av3Light),	av3Light);
CPArray<CMesh::SVertex>  pamvLight(iNumArrayElements(amvLight),		amvLight);
CPArray<CMesh::STriangle> pamtriLight(iNumArrayElements(amtriLight),	amtriLight);







	SMakeObjects::SMakeObjects()
	{
		pmshCube = rptr_new CMesh
		(
			pav3Cube.paDup(),
			pamvCube.paDup(),
			pamtriCube.paDup(),
			SSurface(rptr_new CTexture(prasReadBMP("BinData/Coppeel3.bmp")), &matMATTE)
		);

		pettCube = new CEntityTest
		(
			CInstance::SInit
			(
				CPresence3<>(CRotate3<>(), 0.015f, CVector3<>(-0.04, 0.125, 0.0)),
				rptr_cast(CRenderType, pmshCube),
				0,
				0,
				"Cube"
			)
		);


		pmshCone = rptr_cast(CMesh, rptr_new CMeshCone
		(
			1.0, 2.0, 24, 
			SSurface(rptr_new CTexture(prasReadBMP("BinData/Stone008.bmp")), &matMETAL)
		));

		pettCone = new CEntityTest
		(
			CInstance::SInit
			(
				CPresence3<>(CRotate3<>(), 0.025f, CVector3<>(0.04, 0.125, 0)),
				rptr_cast(CRenderType, pmshCone),
				0,
				0,
				"Cone"
			)
		);


		pmshPyramid = rptr_new CMesh
		(
			pav3Pyramid.paDup(),
			pamvPyramid.paDup(),
			pamtriPyramid.paDup(),
			SSurface(rptr_new CTexture(prasReadBMP("BinData/TieDye.bmp")), &matSHINY)
		);

		pettPyramid = new CEntityTest
		(
			CInstance::SInit
			(
				CPresence3<>(CRotate3<>(), 0.025f, CVector3<>(0, 0.125, 0)),
				rptr_cast(CRenderType, pmshPyramid),
				0,
				0,
				"Pyramid"
			)
		);


		pmshLight = rptr_new CMesh
		(
			pav3Light.paDup(),
			pamvLight.paDup(),
			pamtriLight.paDup(),
			SSurface(rptr_new CTexture(prasReadBMP("BinData/Bricks.bmp")))
		);

		pettLight = new CEntityTest
		(
			CInstance::SInit
			(
				CPresence3<>(CRotate3<>(d3ZAxis, CDir3<>(0, 1, -1)), 0.005),
				rptr_cast(CRenderType, pmshLight),
				0,
				0,
				"DirLight"
			)
		);
	};
