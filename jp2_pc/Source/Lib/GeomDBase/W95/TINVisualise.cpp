/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of TINVisualise.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/W95/TINVisualise.cpp                                    $
 * 
 * 19    11/13/97 12:54p Mlange
 * No longer draws the camera bounding sphere.
 * 
 * 18    11/05/97 6:00p Mlange
 * Now draws bounding volumes in the wireframe view for the camera used to refine the TIN quad
 * tree.
 * 
 * 17    10/30/97 2:11p Mlange
 * The TIN triangle class no longer contains link pointers to the previous and next triangle.
 * Instead, the TIN mesh now references the triangles through an array of pointers.
 * 
 * 16    97/10/12 22:14 Speter
 * CTexture() now takes CMaterial* parameter.  rvIntensity changed to cvIntensity.
 * 
 * 15    8/28/97 4:10p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 16    97/08/22 13:14 Speter
 * Fixes to make sure CTINVisualise drawing code works with new TIN position.
 * 
 * 15    97/08/22 11:33 Speter
 * Replaced rMaxX and rMaxY members with a CRectangle<> describing extents.  
 * 
 * 14    8/05/97 10:58a Mlange
 * DrawWireframe() is now implemented with the CDraw object. DrawHeightMap() has been disabled
 * in this revision because it is now upside down wrt the wireframe view.
 * 
 * 13    7/22/97 6:11p Mlange
 * Updated for changes to CTINTriangle iteration.
 * 
 * 12    7/22/97 2:03p Mlange
 * Updated for new CBlockAllocator<> and CTINMesh interface.
 * 
 * 11    97/07/07 13:50 Speter
 * Removed SRenderVertex.iX and .iY.
 * 
 * 10    6/25/97 8:29p Mlange
 * Now uses (fast) float to int conversion functions.
 * 
 * 9     6/24/97 1:40p Mlange
 * The DrawHeightMap() function now initialises the iX and iY data members in SRenderVertex.
 * 
 * 8     97/06/17 2:03p Pkeet
 * Removed the reference counting pointer from prasScreen.
 * 
 * 7     6/08/97 2:53p Mlange
 * The wireframe and height map rendering now fills the entire screen.
 * 
 * 6     97/05/23 17:36 Speter
 * Updated CTinMeshVisualise for new CDrawPolygon.
 * 
 * 5     97/05/13 13:38 Speter
 * Made CShape polygon-capable.  Changed CTriIterator to CPolyIterator, added iNumVertices()
 * member.
 * 
 * 4     5/08/97 11:33a Mlange
 * DrawHeightmap now initialises the texture coordinates of a vertex.
 * 
 * 3     4/25/97 8:00p Mlange
 * CTINMeshVisualise::DrawWireframe now no longer uses prasMainScreen but the screen raster
 * contained in the CScreenRender class.
 * 
 * 2     4/17/97 2:53p Mlange
 * Updated for changes to the rMESH_BOUNDARY_TOLERANCE_PCT parameter.
 * 
 * 1     4/10/97 6:38p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#include "GblInc/Common.hpp"

#include "../TINVisualise.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Lib/Renderer/ScreenRender.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "Lib/Renderer/PipeLineHeap.hpp"
#include "Lib/Renderer/PipeLineHelp.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/View/LineDraw.hpp"
#include "Lib/GeomDBase/WaveletQuadTree.hpp"


//**********************************************************************************************
//
// CTINMeshVisualise implementation.
//

	//**********************************************************************************************
	void CTINMeshVisualise::DrawHeightMap(CScreenRender& sr) const
	{
		TReal r_height_min = FLT_MAX;
		TReal r_height_max = -FLT_MAX;
		CPipelineHeap plh;				// Local version of the pipeline heap.

		//
		// Iterate through all the triangles in the mesh and determine the minimum and maximum
		// height values of their vertices.
		//
		for (int i_tri = 0; i_tri < iNumTriangles(); i_tri++)
		{
			const CTINTriangle* pttri_curr = papttriList[i_tri];

			r_height_min = Min(r_height_min, pttri_curr->tvtVertexA().v3Pos.tZ);
			r_height_min = Min(r_height_min, pttri_curr->tvtVertexB().v3Pos.tZ);
			r_height_min = Min(r_height_min, pttri_curr->tvtVertexC().v3Pos.tZ);

			r_height_max = Max(r_height_max, pttri_curr->tvtVertexA().v3Pos.tZ);
			r_height_max = Max(r_height_max, pttri_curr->tvtVertexB().v3Pos.tZ);
			r_height_max = Max(r_height_max, pttri_curr->tvtVertexC().v3Pos.tZ);
		}

		// Handle the case where the mesh has uniform height.
		if (r_height_min == r_height_max)
			r_height_max = 1;

		SSurface sf_greyscale
		(
			// Create a new texture, which has white as its solid colour.
			rptr_new CTexture(CColour(1.0, 1.0, 1.0), &matDEFAULT)
		);

		//
		// Create scaling factors for rendering to the screen.
		// Scale by a value slightly smaller than half the screen size, because the TINMesh actually
		// contains border points slightly outside of its extents.
		//

		TReal r_adj_x = rcExtent.tWidth()  * rMESH_BOUNDARY_TOLERANCE_PCT / 100;
		TReal r_adj_y = rcExtent.tHeight() * rMESH_BOUNDARY_TOLERANCE_PCT / 100;

		CRectangle<> rc_tin
		(
			rcExtent.tX0()    - r_adj_x,     rcExtent.tY1()     + r_adj_y, 
			rcExtent.tWidth() + r_adj_x * 2, -(rcExtent.tHeight() + r_adj_y * 2)
		);

		CTransLinear2<> tlr2_raster
		(
			rc_tin, 

			// Add 0.5 to raster dimensions to implement rounding.
			CRectangle<>(0.5, 0.5, sr.prasScreen->iWidth, sr.prasScreen->iHeight),
			true
		);

		// Create arrays which can contain all the triangles and vertices.
		CMAArray<CRenderPolygon> parpoly(iNumPolygons());
		CAArray<CRenderPolygon*> paprpoly(iNumPolygons());
		CAArray<SRenderVertex>   parv_vertices(iNumVertices());
		CMAArray<SRenderVertex*> paprv_indices(iNumPolygons() * 3);

		for (aptr<CShape::CPolyIterator> ppi = pPolyIterator(); ppi->bNext(); )
		{
			// Construct a CRenderPolygon on the array
			CRenderPolygon& rpoly = *new(parpoly) CRenderPolygon;

			// Get 3 vertex index slots.
			rpoly.paprvPolyVertices = paprv_indices.paAlloc(3);

			rpoly.ptexTexture = sf_greyscale.ptexTexture.ptGet();

			// Find the intersection of the global and triangle render features.
			rpoly.seterfFace = sf_greyscale.seterfFeatures & sr.pSettings->seterfState;
			rpoly.iFogBand = 0;

			//
			// Process the triangle vertices.
			//
			for (uint u_v = 0; u_v < 3; u_v++)
			{
				// Don't worry about duplicating processing; it's no big deal.
				SRenderVertex& rv = parv_vertices[ppi->iShapeVertex(u_v)];

				// Scale and transform to raster.
				rv.v3Screen = CVector2<>(ppi->v3Point(u_v)) * tlr2_raster;
				rv.v3Screen.tZ = .5;

				Assert(bWithin(rv.v3Screen.tX, 0, sr.prasScreen->iWidth));
				Assert(bWithin(rv.v3Screen.tY, 0, sr.prasScreen->iHeight));

				// Set intensity proportional to point height.
				TReal r_height = ppi->v3Point(u_v).tZ;

				rv.cvIntensity = rpoly.ptexTexture->ppcePalClut->pclutClut->cvFromLighting
				(
					(r_height - r_height_min) / (r_height_max - r_height_min)
				);

				// Set some dummy texture values.
				rv.tcTex.tX = rv.tcTex.tY = 0;

				rpoly.paprvPolyVertices[u_v] = &rv;
			}
		}

		// Create the array of pointers.
		MakePointerList(parpoly, plh);

		sr.DrawPolygons(plh.parppolyPolygons());
	}




	//**********************************************************************************************
	void CTINMeshVisualise::DrawWireframe(CScreenRender& sr, CColour clr) const
	{
		// Attempt to cast to a raster win.
		CRasterWin* pras_win = dynamic_cast<CRasterWin*>(sr.prasScreen);

		// Failed, cannot draw anything.
		if (!pras_win)
			return;

		//
		// Setup draw class.
		//

		TReal r_adj_x = rcExtent.tWidth()  * rMESH_BOUNDARY_TOLERANCE_PCT / 100;
		TReal r_adj_y = rcExtent.tHeight() * rMESH_BOUNDARY_TOLERANCE_PCT / 100;

		CRectangle<> rc_tin
		(
			rcExtent.tX0()    - r_adj_x,     rcExtent.tY1()     + r_adj_y, 
			rcExtent.tWidth() + r_adj_x * 2, -(rcExtent.tHeight() + r_adj_y * 2)
		);

		CDraw draw(rptr_this(pras_win), rc_tin, true);

		draw.Colour(clr);


		//
		// Iterate through all the triangles in the mesh.
		//
		for (int i_tri = 0; i_tri < iNumTriangles(); i_tri++)
		{
			const CTINTriangle* pttri_curr = papttriList[i_tri];

			//
			// Draw each of the edges of the triangle.
			//
			CVector2<> v2_a = pttri_curr->tvtVertexA().v2GetXY();
			CVector2<> v2_b = pttri_curr->tvtVertexB().v2GetXY();
			CVector2<> v2_c = pttri_curr->tvtVertexC().v2GetXY();

			draw.MoveTo(v2_a.tX, v2_a.tY);

			draw.LineTo(v2_b.tX, v2_b.tY);
			draw.LineTo(v2_c.tX, v2_c.tY);
			draw.LineTo(v2_a.tX, v2_a.tY);
		}

		if (pqntinRoot->pcamGetViewCam() != 0)
		{
			draw.Colour(CColour(0.0, 1.0, 0.0));

			// Draw camera bounding volumes.
			CSArray<CVector3<>, 8> sav3_cam_extents;
			pqntinRoot->pcamGetViewCam()->WorldExtents(sav3_cam_extents);

			for (int i = 0; i < 4; i++)
				sav3_cam_extents[i] = CVector2<>(sav3_cam_extents[i]) * pqntinRoot->mpConversions.tlr2WorldToQuad;

			CVector2<> v2_cam_pos = CVector2<>(pqntinRoot->pcamGetViewCam()->v3Pos()) * pqntinRoot->mpConversions.tlr2WorldToQuad;

			draw.MoveTo(sav3_cam_extents[0].tX, sav3_cam_extents[0].tY);
			draw.LineTo(sav3_cam_extents[1].tX, sav3_cam_extents[1].tY);
			draw.LineTo(sav3_cam_extents[2].tX, sav3_cam_extents[2].tY);
			draw.LineTo(sav3_cam_extents[3].tX, sav3_cam_extents[3].tY);
			draw.LineTo(sav3_cam_extents[0].tX, sav3_cam_extents[0].tY);

			draw.MoveTo(sav3_cam_extents[0].tX, sav3_cam_extents[0].tY);
			draw.LineTo(v2_cam_pos.tX,          v2_cam_pos.tY);
			draw.MoveTo(sav3_cam_extents[1].tX, sav3_cam_extents[1].tY);
			draw.LineTo(v2_cam_pos.tX,          v2_cam_pos.tY);
			draw.MoveTo(sav3_cam_extents[2].tX, sav3_cam_extents[2].tY);
			draw.LineTo(v2_cam_pos.tX,          v2_cam_pos.tY);
			draw.MoveTo(sav3_cam_extents[3].tX, sav3_cam_extents[3].tY);
			draw.LineTo(v2_cam_pos.tX,          v2_cam_pos.tY);
		}
	}
