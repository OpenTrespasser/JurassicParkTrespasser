/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Mesh.hpp.
 *
 * To do:
 *		Perform more vertex merging after splitting polygons.
 *		Detect and eliminate very short edges, especially after splitting (also in CMeshPolygon).
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/Mesh.cpp                                                $
 * 
 * 175   10/06/98 9:44p Jpaque
 * Fixed a problemo with loading a save game with an animating map not activated, activating it,
 * then loading the save game.
 * 
 * 174   98.09.30 10:36p Mmouni
 * Bounding volume is now adjusted for shadow mesh.
 * 
 * 173   9/24/98 8:14p Asouth
 * Added explicit reference to the atArray element
 * 
 * 172   9/22/98 2:40p Mmouni
 * MakeSurfacesUnique now calles destructor for the surfaces it gets rid of.
 * 
 * 171   98.09.22 12:26a Mmouni
 * Changed so that the unlit property is on a per mesh instead of per texture basis.
 * 
 * 170   9/13/98 10:55p Kmckis
 * only print error message if loading
 * 
 * 169   98.09.12 8:47p Mmouni
 * Changed degenerate polygon message back to assert, although you can retry through it and get
 * a valid optimzed groff.
 * 
 * 168   98.09.12 5:41p Mmouni
 * Added texture co-ordinate range checking to Validate function.
 * Validate function is now called based on the VER_TEST switch.
 * 
 * 167   9/10/98 2:31p Rwyatt
 * Floating point consistency is enabled because it was causing vertex normals to be different
 * in release and debug.
 * 
 * 166   98.09.09 11:24p Mmouni
 * Changed degenerate polygon assert to an error message.
 * 
 * 165   98.09.09 4:18p Mmouni
 * Fixed mesh merging funkyness.
 * 
 * 164   9/08/98 8:50p Rwyatt
 * All mesh data now goes into the global mesh fast heap
 * 
 * 163   8/30/98 9:00p Agrant
 * Fix animated mesh save bug
 * Animated meshes now start at frame 0.
 * 
 * 162   8/26/98 4:21p Pkeet
 * Removed multiply defined value.
 * 
 * 161   8/26/98 3:24p Asouth
 * New and Improved! Now fewer casts!
 * 
 * 157   8/25/98 4:41p Rwyatt
 * Reset heaps
 * 
 * 156   98.08.21 7:17p Mmouni
 * Made mesh optimized conditional, added support for Optimized mesh heaps.
 * 
 * 155   8/17/98 4:31p Mmouni
 * Fixed reset crash in CMeshAnimating::pcLoad().
 * 
 * 154   8/15/98 6:35p Mmouni
 * Disablec K6_3D assembly that was causing crashes under vc5.
 * 
 * 153   8/14/98 8:09p Mmouni
 * 
 * 152   8/14/98 5:04p Jpaque
 * fixed bogus assert
 * 
 * 151   98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 150   8/11/98 2:15p Agrant
 * allow saving animating meshes
 * 
 * 149   8/10/98 5:23p Rwyatt
 * Removed the mesh stats and replaced them with memory counters
 * 
 * 148   98.08.10 4:21p Mmouni
 * Cleaned up CMeshAnimating.  Added support for animating the texture on just a sub-material.
 * 
 * 147   8/07/98 11:57p Rwycko
 * Prints out verts of bad triangles
 * 
 * 146   98/08/06 16:26 Speter
 * Added v3Max to store strict bounding extents, for physics. Added wrap and occlusion arrays,
 * and bump matrices, to stats and uMemSize().
 * 
 * 145   98.07.28 4:36p Mmouni
 * Bounding volume is now constructed from wrap points if possible.
 * 
 * 144   98/07/24 13:58 Speter
 * psMeshSize now shown in debug window with other mesh stats; removed from stats window.
 * 
 * 143   98/07/23 18:30 Speter
 * Added GetExtents() implementation.
 * 
 * 142   7/22/98 10:04p Agrant
 * Removed data daemon and pre-fetching
 * 
 * 141   7/22/98 11:21a Jpaque
 * fixed build break
 * 
 * 140   7/21/98 8:58p Agrant
 * explain which mesh has the error
 * 
 * 139   7/20/98 7:09p Agrant
 * added freeze frame to animated textures
 * 
 * 138   98/07/14 20:11 Speter
 * Indented info message.
 * 
 * 137   7/10/98 5:10p Mlange
 * Poly iterators now take a render context instead of a camera.
 * 
 * 136   7/03/98 3:44p Pkeet
 * Added the 'bAlwaysFaceMesh' flag.
 * 
 * 135   98/06/29 16:08 Speter
 * Rendering functions now take the CInstance* parameter. Made iNumTriangles virtual,
 * implemented in CMesh. Also, ApplyCurves() now called in CMesh constructor, not loader.
 * 
 * 134   6/27/98 6:12p Agrant
 * Fixed animated textures
 * 
 * 133   6/26/98 7:24p Agrant
 * Animating mesh enchancements
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Mesh.hpp"
#include "MeshIterator.hpp"

#include "ClipMesh.hpp"
#include "Lib/Loader/DataDaemon.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/FastHeap.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Renderer/Line2D.hpp"
#include "Lib/Renderer/Clip.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Renderer/Pipeline.hpp"
#include "Lib/Renderer/Primitives/FastBump.hpp"
#include "Lib/GeomDBase/Plane.hpp"
#include "Lib/GeomDBase/Terrain.hpp"
#include "Lib/Transform/VectorRange.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "AsmSupport.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Renderer/ShapePresence.hpp"
#include "Lib/EntityDBase/RenderDB.hpp"

#include <memory.h>
#include <malloc.h>
#include <list>


//**********************************************************************************************
//
// Constants.
//

// Number of attempts at randomly perturbing hull points to get a hull with less points.
#define iNUM_PERTURBANCE_TRIES	0

// Use the quickhull algorithm.
#define	bUSE_QUICKHULL			0

// Turn this on to visually debug polygon merging (or to see pretty fireworks).
#define bLINE_DRAW				0

// Turn this on to enable logging of merge information.
#define bLOG_MERGE				0
#define bLOG_SPLIT				1

// Determines whether to check for linear normal gradients when merging.
#define bMERGE_CHECK_NORMALS	0

#define uMAX_POLYGONS_INPUT	    (2048)
#define uMAX_POINTS_BEFORE_WRAP	(512)
#define uMAX_POINTS_WRAP	    (128)

// Provide a high tolerance for gradients when merging.
const float fCOPLANAR_TOLERANCE		= 0.01;
const float fTEX_GRAD_TOLERANCE		= 0.04;		// Texture gradients when poly is normalized to 1.0 size.
const float fNORM_GRAD_TOLERANCE	= 0.10;
const float fTEX_LINEAR_TOLERANCE	= 0.10;		// Amount middle vertex may be of by when merging co-linear edges.
const float fCOLINEAR_TOLERANCE		= 1e-6;
const TReal rMIN_POLYGON_AREA		= 0.000;
const float fMIN_SPLIT_RATIO		= 0.01;
const float fSHADOW_TOL_ADJUST		= 10.0;
const int	iPOLYGONS_SHADOW		= 10;
const float	fSHADOW_REDUCE_THRESHOLD= 0.80;

const CAngle angNORMAL_SHARE_DEFAULT	= 75 * dDEGREES;	// Share vertices unless faces are almost right angles.

//
// Private variables.
//

// Heap used during temporary mesh building.
static aptr<CMesh::CHeap> pmhHeap;

// Stats.

// Not static so we can look at it elsewhere.

static CProfileStat		
	psTriangles/*("Triangles", &psMeshes, Set(epfMASTER))*/,
		psRejectTexture/*("RejectTex", &psTriangles)*/,
		psRejectNormal/*("RejectNorm", &psTriangles)*/,
		psRejectHole/*("RejectHole", &psTriangles)*/,
		psSplitConcave;/*("Concave", &psTriangles)*/


#if bLINE_DRAW
	#include "Lib/Sys/W95/Render.hpp"
	#include "Lib/View/LineDraw.hpp"
#endif

#if bLOG_MERGE || bLOG_SPLIT
	#include "Lib/Loader/Loader.hpp"
#endif

//**********************************************************************************************
//
uint uCreateWrap
(
	CVector3<>* av3,
	uint u_num_pts
);
//
// Function selects only vertices that 'wrap' the set of vertices provided in the array.
//
// Returns the number of vertices in the reduced array.
//
//**********************************


//**********************************************************************************************
//
uint uCreateWrapSubdivide
(
	CVector3<>* av3,
	uint u_num_pts
);
//
// Function selects only vertices that 'wrap' the set of vertices provided in the array.
//
// Returns the number of vertices in the reduced array.
//
//**********************************


//**********************************************************************************************
//
// Private functions.
//

	//**********************************************************************************************
	CAngle angNormalShare(ENormal enl)
	{
		switch (enl)
		{
			case enlFLAT:
				// No normals are shared.
				return 0;
			case enlCURVED:
				// All normals are shared.
				return dPI;
			default:
				Assert(0);
			case enlGUESS:
			case enlSPECIFIED:
				return angNORMAL_SHARE_DEFAULT;
		}
	}


	//******************************************************************************************
	template<class AD, class AS> void Copy(AD pa_dest, AS pa_src)
	{
		for (int i = 0; i < pa_src.size(); i++)
			pa_dest[i] = pa_src[i];
	}

	//******************************************************************************************
	inline void AssertFinite(const CVector2<>& v2)
	{
		Assert(_finite(v2.tX));
		Assert(_finite(v2.tY));
	}

	//******************************************************************************************
	inline void AssertPosFinite(const CVector2<>& v2)
	{
		Assert(v2.tX >= 0 && _finite(v2.tX));
		Assert(v2.tY >= 0 && _finite(v2.tY));
	}

	//******************************************************************************************
	void ErrorDegenerate(const CMesh::SPolygon* pmp, const CMesh::CHeap& mh)
	{
#if VER_TEST
		char str_buffer[100];
		sprintf(str_buffer, "Degenerate triangle: points %d, %d, %d",
			pmp->papmvVertices[0]->pv3Point - mh.mav3Points,
			pmp->papmvVertices[1]->pv3Point - mh.mav3Points,
			pmp->papmvVertices[2]->pv3Point - mh.mav3Points);
		TerminalError(ERROR_ASSERTFAIL, true, str_buffer);
#endif
	}

	//******************************************************************************************
	inline bool bFurryEquals(const CVector2<>& v2_a, const CVector2<>& v2_b, TReal r_tolerance)
	{
		return bFurryEquals(v2_a.tX, v2_b.tX, r_tolerance) && 
			   bFurryEquals(v2_a.tY, v2_b.tY, r_tolerance);
	}

	//******************************************************************************************
	bool operator ==(const CMesh::SVertex& mv_a, const CMesh::SVertex& mv_b)
	{
		return mv_a.pv3Point		== mv_b.pv3Point &&
			   Fuzzy(mv_a.d3Normal) == mv_b.d3Normal &&
			   mv_a.tcTex			== mv_b.tcTex;
	}

	//******************************************************************************************
	bool bIsPlane
	(
		const CVector3<>& v3_a, const CVector3<>& v3_b, const CVector3<>& v3_c
										// Points to test.
	)
	//
	// Returns:
	//		Whether these points form a valid plane.
	//
	//**********************************
	{
		CVector3<> v3 = (v3_c - v3_b) ^ (v3_a - v3_b);
		return !v3.bIsZero();
	}

	//******************************************************************************************
	int iColinear
	(
		const CVector3<>& v3_a, const CVector3<>& v3_b, const CVector3<>& v3_c,
										// Points to test.
		TReal r_tolerance = fCOLINEAR_TOLERANCE
	)
	//
	// Returns:
	//		1 if the points are co-linear, and joint positively.
	//		-1 if they are co-linear, and join negatively (backtrack).
	//		0 if they are not co-linear (they form a plane).
	//
	//**********************************
	{
		TReal r_dot = CDir3<>(v3_c - v3_b) * CDir3<>(v3_b - v3_a);
		if (r_dot >= 1.0 - r_tolerance)
			return 1;
		else if (r_dot <= -1.0 + r_tolerance)
			return -1;
		return 0;
	}

#if bLINE_DRAW
	//******************************************************************************************
	void DrawPolygon
	(
		CDraw& draw,
		const CMesh::SPolygon& mp,
		const CMatrix3<>& mx3_flatten
	)
	{
		// Begin by moving to final vertex.
		draw.MoveTo(mp.v3Point(mp.papmvVertices.uLen-1) * mx3_flatten);
		for (uint u_x = 0; u_x < mp.papmvVertices.uLen; u_x++)
			draw.LineTo(mp.v3Point(u_x) * mx3_flatten);
	}
#endif

	//******************************************************************************************
	class GradientData
	{
	private:
		CVector2<> pv2_du;				// Texture coord gradients.
		CVector2<> pv2_dv;
		CVector2<> pv2_dnx;				// Vertex normal gradients.
		CVector2<> pv2_dny;
		TReal	   rArea;				// Area of poly.

	public:
		GradientData
		(
			const CMesh::SPolygon& mp,
			const CMatrix3<>& mx3_flatten
		)
		{
			// We can calculate it based on the first 3 vertices, assuming that a larger polygon
			// would not have been constructed had all the polygon's gradients not matched.

			#if VER_DEBUG
				CVector2<> v2_temp = mp.v3Point(0);
				AssertFinite(v2_temp);
				v2_temp = mp.v3Point(1);
				AssertFinite(v2_temp);
				v2_temp = mp.v3Point(2);
				AssertFinite(v2_temp);
			#endif

			// First calculate the divisor for the derivatives, 
			// which is proportional to the area of the triangle.
			CVector2<> v2_01 = (mp.v3Point(1) - mp.v3Point(0)) * mx3_flatten;
			CVector2<> v2_02 = (mp.v3Point(2) - mp.v3Point(0)) * mx3_flatten;

			AssertFinite(v2_01);
			AssertFinite(v2_02);

			// Compute area and inverse area.
			rArea = (v2_01 ^ v2_02);
			TReal r_invd = 1.0f / rArea;

			// U derivatives.
			CVector2<> v2_t01 = mp.tcTexCoord(1) - mp.tcTexCoord(0);
			CVector2<> v2_t02 = mp.tcTexCoord(2) - mp.tcTexCoord(0);

			AssertFinite(mp.tcTexCoord(0));
			AssertFinite(mp.tcTexCoord(1));
			AssertFinite(mp.tcTexCoord(2));

			AssertFinite(v2_t01);
			AssertFinite(v2_t02);
			
			pv2_du.tX = v2_t01.tX * v2_02.tY - v2_t02.tX * v2_01.tY;
			pv2_du.tY = v2_t01.tX * v2_02.tX - v2_t02.tX * v2_01.tX;
			pv2_du *= r_invd;

			AssertFinite(pv2_du);

			// V derivatives.
			pv2_dv.tX = v2_t01.tY * v2_02.tY - v2_t02.tY * v2_01.tY;
			pv2_dv.tY = v2_t01.tY * v2_02.tX - v2_t02.tY * v2_01.tX;
			pv2_dv *= r_invd;

			AssertFinite(pv2_dv);

			// NX derivatives.
			CVector2<> v2_d01 = ( CVector3<>(mp.d3Normal(1)) - CVector3<>(mp.d3Normal(0)) ) * mx3_flatten;
			CVector2<> v2_d02 = ( CVector3<>(mp.d3Normal(2)) - CVector3<>(mp.d3Normal(0)) ) * mx3_flatten;

			pv2_dnx.tX = v2_d01.tX * v2_02.tY - v2_d02.tX * v2_01.tY;
			pv2_dnx.tY = v2_d01.tX * v2_02.tX - v2_d02.tX * v2_01.tX;
			pv2_dnx *= r_invd;

			// NY derivatives.
			pv2_dny.tX = v2_d01.tY * v2_02.tY - v2_d02.tY * v2_01.tY;
			pv2_dny.tY = v2_d01.tY * v2_02.tX - v2_d02.tY * v2_01.tX;
			pv2_dnx *= r_invd;
		}

		bool texture_gradients_equal(const GradientData &gd, float f_tolerance = fTEX_GRAD_TOLERANCE)
		{
			// Multiply tolerance by inverse square of area of combined poly.
			// Square root is necessary because we a comparing linear measures.
			TReal r_inv_area = 1.0f / sqrt(rArea + gd.rArea);

		#if (0)
			dprintf("Comparing: Area = %f, Diffs = %f,%f,%f,%f\n", rArea + gd.rArea,
					fabs(pv2_du.tX - gd.pv2_du.tX), fabs(pv2_du.tY - gd.pv2_du.tY), 
					fabs(pv2_dv.tX - gd.pv2_dv.tX),	fabs(pv2_dv.tY - gd.pv2_dv.tY));
		#endif

			return Fuzzy(pv2_du, f_tolerance*r_inv_area) == gd.pv2_du &&
				   Fuzzy(pv2_dv, f_tolerance*r_inv_area) == gd.pv2_dv;
		}

		bool normal_gradients_equal(const GradientData &gd, float f_tolerance = fNORM_GRAD_TOLERANCE)
		{
			// Multiply tolerance by inverse square of area of combined poly.
			// Square root is necessary because we a comparing linear measures.
			TReal r_inv_area = 1.0f / sqrt(rArea + gd.rArea);

			return Fuzzy(pv2_dnx, f_tolerance*r_inv_area) == gd.pv2_dnx &&
				   Fuzzy(pv2_dny, f_tolerance*r_inv_area) == gd.pv2_dny;

			return false;
		}
	};

	//******************************************************************************************
	//
	void SetPolygonPoints
	(
		CPArray< CVector3<> > pav3,		// Point array to fill.
		const CMesh::SPolygon& mp		// Mesh polygon whose points to extract.
	)
	//
	// Fills the array with the polygon's points.
	//
	//**********************************
	{
		for (int i = 0; i < mp.papmvVertices.uLen; i++)
			pav3[i] = mp.v3Point(i);
	}

/*
	//******************************************************************************************
	//
	bool bOppositePolygons
	(
		const CMesh::SPolygon& mp_1,
		const CMesh::SPolygon& mp_2
	)
	//
	// Returns:
	//		Whether these polygons are the same but opposite, representing the sides of a
	//		2-sided polygon.
	//
	//**********************************
	{
		// Find polygon 1's first point in polygon 2.
		for (int i_2 = 0; i_2 < mp_2.papmvVertices.uLen; i_2++)
		{
			if (&mp_2.v3Point(i_2) == &mp_1.v3Point(0))
			{
				// Found it.  Now make sure others are shared 
			}
		}
	}

	//******************************************************************************************
	bool bIntersect
	(
		const CPlane& pl,				// Plane to test.
		const CMesh::SPolygon& mp		// Polygon to test.
	)
	//
	// Returns:
	//		Whether the polygon intersects the given plane.
	//
	//**********************************
	{
		ESideOf esf = 0;

		for (uint u = 0; u < mp.papmvVertices.uLen; u++)
			esf |= pl.esfSideOf(mp.v3Point(u));
		return esf == esfINTERSECT;
	}

	//******************************************************************************************
	bool bPolygonsIntersect(const CMesh::SPolygon& mp_1, const CMesh::SPolygon& mp_2)
	{
		// True if each polygon intersects the other's plane.
		return bIntersect(mp_1.plPlane, mp_2) && bIntersect(mp_2.plPlane, mp_1);
	}
*/

//**********************************************************************************************
//
// CMesh implementation.
//

// static fast heap that all final mesh data is allocated from
CFastHeap	CMesh::fhGlobalMesh(1 << 25);		// 32Mb

	//******************************************************************************************
	//
	// CMesh::SVertex implementation.
	//

		//**************************************************************************************
		CMesh::SVertex::SVertex(const SVertex& mv_a, const SVertex& mv_b, TReal r_t, CHeap& mh)
		{
			// The intersection must occur along the line segment.
			Assert(bWithin(r_t, 0.0, 1.0));

			TReal r_s = 1.0 - r_t;

			// Create new point on the heap.
			pv3Point = new(mh.mav3Points) CVector3<>
			(
				r_s * mv_a.pv3Point->tX  +  r_t * mv_b.pv3Point->tX,
				r_s * mv_a.pv3Point->tY  +  r_t * mv_b.pv3Point->tY,
				r_s * mv_a.pv3Point->tZ  +  r_t * mv_b.pv3Point->tZ
			);

			// Interpolate normals and tex coords also.
			d3Normal = mv_a.d3Normal * r_s  +  mv_b.d3Normal * r_t;

			tcTex = CTexCoord
			(
				r_s * mv_a.tcTex.tX  +  r_t * mv_b.tcTex.tX,
				r_s * mv_a.tcTex.tY  +  r_t * mv_b.tcTex.tY
			);
		}

	//******************************************************************************************
	//
	// CMesh::SPolygon implementation.
	//

		//******************************************************************************************
		CMesh::SPolygon::SPolygon(CHeap& mh, int i_vertex_start, int i_vertex_count)
			: papmvVertices(mh.mapmvVertices.paAlloc(i_vertex_count)), bOcclude(0), bCache(0),
			  pmx3ObjToTexture(0)
		{
			// Fill the pointer array with the addresses of each vertex.
			for (int i = 0; i < i_vertex_count; i++)
				papmvVertices[i] = &mh.mamvVertices[i_vertex_start + i];
		}

		//******************************************************************************************
		CMesh::SPolygon::SPolygon(CHeap& mh, int i_vertex_0, int i_vertex_1, int i_vertex_2)
			: papmvVertices(mh.mapmvVertices.paAlloc(3)), bOcclude(0), bCache(0),
			  pmx3ObjToTexture(0)
		{
			// Fill the pointer array with the addresses of each vertex.
			papmvVertices[0] = &mh.mamvVertices[i_vertex_0];
			papmvVertices[1] = &mh.mamvVertices[i_vertex_1];
			papmvVertices[2] = &mh.mamvVertices[i_vertex_2];
		}

		//******************************************************************************************
		TReal CMesh::SPolygon::rGetArea() const
		{
			// Slow accurate method: sum the areas of each sub-triangle.
			TReal r_area = 0;
			for (int i = 0; i < iNumVertices()-2; i++)
			{
				CVector3<> v3_cross = (v3Point(i+1) - v3Point(0)) ^ (v3Point(i+2) - v3Point(0));
				r_area += v3_cross.tLen() * 0.5;
			}
			return r_area;
		}
		
		//**************************************************************************************
		CVector3<> CMesh::SPolygon::v3GetCentre() const
		{
			Assert(iNumVertices() > 2);

			// Start with the first point.
			CVector3<> v3_centre = v3Point(0);

			// Accumulatively add the remaining points.
			for (int i = 1; i < iNumVertices(); i++)
			{
				v3_centre += v3Point(i);
			}
			v3_centre /= TReal(iNumVertices());
			return v3_centre;
		}


	//******************************************************************************************
	//
	// CMesh::CHeap implementation.
	//

		//**************************************************************************************
		CMesh::CHeap::CHeap(int i_num_polygons) :
			mampPolygons(i_num_polygons),
			mapmvVertices(i_num_polygons * 16),
			mamvVertices(i_num_polygons * 4),
			mav3Points(i_num_polygons * 4),
			mav3Wrap(i_num_polygons * 4),
			masfSurfaces(i_num_polygons)
		{
			bOptimized = false;
		};

		//**************************************************************************************
		CMesh::CHeap::~CHeap()
		{
		}

		//**************************************************************************************
		void CMesh::CHeap::Reset()
		{
			// Reset all sub-arrays.
			mampPolygons.Reset(0,false,0);
			mapmvVertices.Reset(0,false,0);
			mamvVertices.Reset(0,false,0);
			mav3Points.Reset(0,false,0);
			mav3Wrap.Reset(0,false,0);

			// The surfaces must be properly destroyed, so pass 'true'.
			masfSurfaces.Reset(0, true, 0);

			bOptimized = false;
		}

	//******************************************************************************************
	bool CMesh::bMergePolygons	= true;
	bool CMesh::bSplitPolygons	= false;
	bool CMesh::bQuashPolygons	= true;
	bool CMesh::bCurveBumps	= false;

	//******************************************************************************************
	CMesh::CHeap& CMesh::mhGetBuildHeap()
	{
		// Create our heap if not done already.
		if (!pmhHeap)
			pmhHeap = new CHeap(uMAX_POLYGONS_INPUT);

		// Make sure this is an empty heap; i.e. the CMesh constructor was called after
		// last filling the heap.

		Assert(pmhHeap->mampPolygons.uLen == 0);
		Assert(pmhHeap->mapmvVertices.uLen == 0);
		Assert(pmhHeap->mamvVertices.uLen == 0);
		Assert(pmhHeap->mav3Points.uLen == 0);
		Assert(pmhHeap->mav3Wrap.uLen == 0);
		Assert(pmhHeap->masfSurfaces.uLen == 0);

		return *pmhHeap;
	}

	// HACK.
	static bool bAutoShadow		= false;
	static int i_shadow_mesh	= 0;


	//******************************************************************************************
	CMesh::CMesh(CHeap& mh, CAngle ang_max_vertex_share,
				 bool b_merge_polygons, bool b_split_polygons, bool b_curved)
		: bvbVolume(mh.mav3Wrap), bCacheableMesh(true), bAlwaysFaceMesh(false), bUnlit(false)
	{
		//
		// Note: For polygon merging, splitting, and other massaging, this constructor utilises
		// the temporary private heaps for newly created polygons and vertices.  When done, the
		// mesh data is copied from the input arrays and the temporary heap into a freshly 
		// allocated heap for this mesh.  The input arrays are no longer used, and the constructor
		// frees them.
		//
		CCycleTimer	ctmr;
		CCycleTimer ctmr2;

		static rptr<CTexture> ptexEmpty = rptr_new CTexture(0);

		// If there are no surfaces, create a default one.
		// For shadow meshes, this will make the last surface the default one.
		if (mh.masfSurfaces.uLen == 0 || i_shadow_mesh)
			new(mh.masfSurfaces) SSurface(ptexEmpty);

		if (!mh.masfSurfaces[0].ptexTexture)
			mh.masfSurfaces[0].ptexTexture = ptexEmpty;

		SSurface* psf_empty = &mh.masfSurfaces(-1);

		// Compute the actual extents of the mesh.
		CBoundVolBox bvb(mh.mav3Points);
		v3Max = bvb.v3GetMax();

		// If we don't have any wrap points, create the bounding volume from regular points.
		if (mh.mav3Wrap.uLen == 0)
			bvbVolume = CBoundVolBox(v3Max);

		if (!mh.bOptimized)
		{
			// Perform some preliminary processing on the polygons.
			SPolygon* pmp;
			for (pmp = mh.mampPolygons; pmp < mh.mampPolygons.end(); pmp++)
			{
				// Help out polygons without surface pointers, by assigning them the first one.
				pmp->bHidden = false;
				if (!pmp->pSurface)
					pmp->pSurface = &mh.masfSurfaces[0];

				// Set the face normal from the first 3 points.
				// Access the protected pmpCurrent member in order to modify it.
				pmp->plPlane = CPlane(pmp->v3Point(0), pmp->v3Point(1), pmp->v3Point(2));

				// Set shadow mesh's vertex normals to dummy values.
				if (i_shadow_mesh)
				{
					// For shadowing, only transparent textures matter; solid textures are not accessed.
					// So set all solid textures the same for merging efficiency.
					if (!pmp->pSurface->ptexTexture->seterfFeatures[erfTRANSPARENT])
						pmp->pSurface = psf_empty;
				}

				// Set all vertices to share values whenever possible.
				for (int i = 0; i < pmp->iNumVertices(); i++)
				{
					if ((!pmp->pSurface->ptexTexture->seterfFeatures[erfBUMP] && !pmp->pSurface->ptexTexture->seterfFeatures[erfLIGHT_SHADE]) || i_shadow_mesh)
					{
						// Unshaded non bumpmapped vertices get the same normal, as the normal is unused.
						pmp->papmvVertices[i]->d3Normal = d3ZAxis;
						//pmp->papmvVertices[i]->bSpecified = true;
					}

					// Set texture coords of solid polygons to default values.
					if (!pmp->pSurface->ptexTexture->seterfFeatures[erfTEXTURE])
					{
						pmp->papmvVertices[i]->tcTex = CTexCoord(0, 0);
					}
				}
			}

			extern CProfileStat psMeshPolySetup;
			psMeshPolySetup.Add(ctmr2());

			//CalculateFaceNormals(mh);
			if (!i_shadow_mesh)
				CalculateVertexNormals(mh, ang_max_vertex_share);

			extern CProfileStat psMeshVertNormals;
			psMeshVertNormals.Add(ctmr2());

			psTriangles.Add(0, mh.mampPolygons.uLen);

			// HACK:  Don't slow down loading too much by processing huge meshes 
			// (typically terrain surrogates).
			if (mh.mampPolygons.uLen < 0x800)
			{
				MergeVertices(mh);

				if (bMergePolygons && b_merge_polygons)
					MergePolygons(mh);

				// After all that trouble...
				if (bSplitPolygons && b_split_polygons)
					SplitIntersectingPolygons(mh);

				MakePolygonsPlanar(mh);
			}

			extern CProfileStat psMeshMerge;
			psMeshMerge.Add(ctmr2());

			//
			// Assign curved flag, and check for concave polygons.
			//
			for (pmp = mh.mampPolygons; pmp < mh.mampPolygons.end(); pmp++)
			{
				pmp->bCurved = false;


				if (pmp->pSurface->ptexTexture->seterfFeatures[erfBUMP][erfLIGHT_SHADE] && !i_shadow_mesh)
				{
					for (int i_v = 0; i_v < pmp->iNumVertices(); i_v++)
					{
						if (Fuzzy(pmp->d3Normal(i_v) * pmp->plPlane.d3Normal) != 1.0)
						{
							if (pmp->pSurface->ptexTexture->seterfFeatures[erfBUMP])
							{
								// for bump maps, if any vertex normal is not equal to the face normal then set the
								// curved flag to the curved text prop state.
								pmp->bCurved = b_curved || bCurveBumps;
							}
							else
							{
								// For non bump maps, any vertex normal not equal to face makes it curved.
								pmp->bCurved = true;
							}
							break;
						}
					}
				}

	/*#if 0 && VER_DEBUG
				// Each subtriangle must have the same plane.
				// If a segment is concave, the plane normal will be opposite.
				const float f_coplanar_tol	= i_shadow_mesh ? fCOPLANAR_TOLERANCE * 1 : fCOPLANAR_TOLERANCE;
				for (int i_v = 2; i_v < pmp->iNumVertices(); i_v++)
				{
					if (bIsPlane(pmp->v3Point(i_v - 2), pmp->v3Point(i_v - 1), pmp->v3Point(i_v)))
					{
						CPlane pl(pmp->v3Point(i_v - 2), pmp->v3Point(i_v - 1), pmp->v3Point(i_v));
						Assert(Fuzzy(pl, f_coplanar_tol) == pmp->plPlane);
					}
				}
	#endif*/
			}
		}

		// Copy input data into final per-mesh arrays.
		AllocateFinalData(mh);

		//
		// Check for occluding polygons.
		//
		IdentifyOccludingPolygons();

		MEMLOG_ADD_COUNTER(emlOcclude, papmpOcclude.uLen);

		//
		// Some code to check whether the vertex normals make sense.
		// Unfortunately, currently, some of them do not.
		//
/*
		{
		for (CPolyIterator pi(*this); pi.bNext(); )
		{
			for (uint u = 0; u < pi.iNumVertices(); u++)
				Assert(pi.d3Normal(u) * pi.d3Normal() >= 0);
//				if (pi.d3Normal(u) * pi.d3Normal() < 0)
//					pi.pmpCurrent->seterfFace -= erfTEXTURE;
		}
		}
*/

		// Verify that we have a good mesh.
		AlwaysAssert(pampPolygons.uLen <= 1024);

#if VER_TEST
		Validate();
#endif

		// Warn if we have too many polygons.
		if (pampPolygons.uLen > 300)
			dprintf("%s  More than 300 polygons in mesh (%d)\n", CLoadWorld::pgonCurrentObject->strObjectName, pampPolygons.uLen);

		// HACK for rptr hole during construction.
		if (bAutoShadow && !i_shadow_mesh && iNumPolygons() > iPOLYGONS_SHADOW)
		{
			uRefs++;
			if (!pshGetTerrainShape() || pshGetTerrainShape() == rptr_this(this))
			{
				i_shadow_mesh++;		// HACK.
				rptr<CMesh> pmsh_shadow = rptr_new CMesh(mh, 0, true, true);
				i_shadow_mesh--;		// HACK.
				if (pmsh_shadow->iNumPolygons() < iNumPolygons() * fSHADOW_REDUCE_THRESHOLD)
				{
					SetShadowShape(rptr_cast(CShape, pmsh_shadow));
					dprintf("  Mesh: %d polys, %d vertices, %d points\n", 
						iNumPolygons(), iNumVertices(), iNumPoints());
					dprintf("  Shadow: %d polys, %d vertices, %d points\n", 
						pmsh_shadow->iNumPolygons(), pmsh_shadow->iNumVertices(), pmsh_shadow->iNumPoints());
				}
			}
			uRefs--;
		}

		// Apply curvature to curved bump-mapped polygons.
		// HACK to get around bug with using rptr_this in constructors.
		uRefs++;
		ApplyCurves(rptr_this(this));
		uRefs--;
		extern CProfileStat psCurve;
		psCurve.Add(ctmr2());

		MEMLOG_ADD_COUNTER(emlMeshes, 1);
	}

	//******************************************************************************************
	CMesh::CMesh(CHeap& mh, ENormal enl, bool b_merge_polygons, bool b_split_polygons, bool b_curved)
	{
		new(this) CMesh(mh, angNormalShare(enl), b_merge_polygons, b_split_polygons, b_curved);
	}

	//******************************************************************************************
	CMesh::~CMesh()
	{
		uint32 u4;

		MEMLOG_ADD_COUNTER(emlDeletedMeshes, 1);
		MEMLOG_ADD_COUNTER(emlDeletedMeshMemory,(int)uSize());

		//
		// All of the arrays within the mesh are within fast heaps so the memory does not
		// need to be deleted BUT the classes need to be destroyed so that any other memory
		// is freed and rptr's are kept upto date.
		//
		// If an array is in a fast heap we need to destroy all of the elements but not delete
		// the memory block holding the array. Therefore we just go through the whole array
		// and explicitly call the destructor of the correct type. 
		// If the array is not in the mesh heap then we just delete the array as normal.
		//

		// Array of SPolygon structures
		if (bInMeshHeap((SPolygon*)pampPolygons))
		{
			for (u4 = 0; u4<pampPolygons.size(); u4++)
			{
				// call the destructor but do not delete the memory
				pampPolygons[u4].~SPolygon();
			}
		}
		else
		{
			delete[] pampPolygons.atArray;
		}


		// array of SVertex*
		if (bInMeshHeap((SVertex**)papmvVertices))
		{
			// pointers have no destructor!
		}
		else
		{
			delete[] papmvVertices.atArray;
		}


		// array of SVertex structures
		if (bInMeshHeap((SVertex*)pamvVertices))
		{
			for (u4 = 0; u4<pamvVertices.size(); u4++)
			{
				// call the destructor but do not delete the memory
				pamvVertices[u4].~SVertex();
			}
		}
		else
		{
			delete[] pamvVertices.atArray;
		}
		

		// array of CVector3<> classes
		if (bInMeshHeap((CVector3<>*)pav3Points))
		{
			for (u4 = 0; u4<pav3Points.size(); u4++)
			{
				// call the destructor but do not delete the memory
			#ifdef __MWERKS__
				pav3Points[u4].~CVector3();
			#else
				pav3Points[u4].~CVector3<>();
			#endif
			}
		}
		else
		{
			delete[] pav3Points.atArray;
		}


		// array of SSurface
		if (bInMeshHeap((SSurface*)pasfSurfaces))
		{
			for (u4 = 0; u4<pasfSurfaces.size(); u4++)
			{
				// call the destructor but do not delete the memory
				pasfSurfaces[u4].~SSurface();
			}
		}
		else
		{
			delete[] pasfSurfaces.atArray;
		}


		// array of CVector3<> classes
		if (bInMeshHeap((CVector3<>*)pav3Wrap))
		{
			for (u4 = 0; u4<pav3Wrap.size(); u4++)
			{
				// call the destructor but do not delete the memory
			#ifdef __MWERKS__
				pav3Wrap[u4].~CVector3();
			#else
				pav3Wrap[u4].~CVector3<>();
			#endif
			}
		}
		else
		{
			delete[] pav3Wrap.atArray
			;
		}

		// array of SPolygon*
		if (bInMeshHeap((SPolygon**)papmpOcclude))
		{
			// pointers have no destructor!
		}
		else
		{
			delete[] papmpOcclude.atArray;
		}
	}

	//******************************************************************************************
	uint CMesh::uSize() const
	{
		uint u_size = sizeof(*this) + 
			pampPolygons.uMemSize() +
			papmvVertices.uMemSize() +
			pamvVertices.uMemSize() +
			pav3Points.uMemSize() +
			pasfSurfaces.uMemSize() +
			pav3Wrap.uMemSize() +
			papmpOcclude.uMemSize();

		// The SPolygon.pmx3ObjToTexture is the only thing missing from this stat.
		for (int i = 0; i < pampPolygons.size(); i++)
			if (pampPolygons[i].pmx3ObjToTexture != 0)
				u_size += sizeof(*pampPolygons[i].pmx3ObjToTexture);

		return u_size;
	}

	//******************************************************************************************
	int CMesh::iNumTriangles() const
	{
		// Simply count the vertices in each polygon to determine the number of triangles.
		int i_num_triangles = 0;

		for (CPolyIterator pi(*this, 0, 0); pi.bNext(); )
			i_num_triangles += pi.iNumVertices() - 2;

		return i_num_triangles;
	}

	//******************************************************************************************
	void CMesh::GetExtents(CInstance* pins, const CTransform3<>& tf3_shape, 
		CVector3<>& rv3_min, CVector3<>& rv3_max) const
	{
		// Just get from bounding volume. To do: use wrap.
		bvbVolume.GetWorldExtents(tf3_shape, rv3_min, rv3_max);
	}

	//******************************************************************************************
	void CMesh::AdjustBoundingVolumeForDetailReducedVersions()
	{
		rptr<CMesh> pmsh = rptr_this(this);

		// Adjust for detail reduced meshes.
		while (pmsh->pshCoarser)
		{
			pmsh = rptr_static_cast(CMesh, pmsh->pshCoarser);
			Assert(rptr_dynamic_cast(CMesh, pmsh));

			bvbVolume += pmsh->bvbVolume;
		}

		// Adjust for shadow mesh.
		if (pmsh->pshShadow)
		{
			pmsh = rptr_static_cast(CMesh, pmsh->pshShadow);
			Assert(rptr_dynamic_cast(CMesh, pmsh));

			bvbVolume += pmsh->bvbVolume;
		}
	}

	//******************************************************************************************
	void CMesh::Rescale(TReal r_scale)
	{
		// Scale points.
		for (int i_vert = pav3Points.uLen - 1; i_vert >= 0; --i_vert)
		{
			pav3Points[i_vert] *= r_scale;
		}

		// Scale the bounding volume.
		bvbVolume *= r_scale;
	}

	//******************************************************************************************
	void CMesh::CalculateFaceNormals(CHeap& mh)
	{
		//
		// For each SPolygon, set the face normal.
		// Iterate using the functions for CShape.
		//
		for (SPolygon* pmp = mh.mampPolygons; pmp < mh.mampPolygons.end(); ++pmp)
		{
			// Set the face normal from the first 3 points.
			// Access the protected pmpCurrent member in order to modify it.
			pmp->plPlane = CPlane(pmp->v3Point(0), pmp->v3Point(1), pmp->v3Point(2));
		}
	}

	//******************************************************************************************
	struct SVertexRecord
	// Prefix: mvr
	// Track vertex instances found, and the faces they belong to.
	{
		CMesh::SVertex* pmvVertex;
		CVector3<> v3Face;

		SVertexRecord()
		{
		}

		SVertexRecord(CMesh::SVertex* pmv, const CVector3<>& v3)
			: pmvVertex(pmv), v3Face(v3)
		{
		}
	};

	//******************************************************************************************
	struct SFaceVert
	// Face, Vertex record.
	{
		int i_face;
		int i_vert;

		SFaceVert()
		{
		}

		SFaceVert(int i_f, int i_v)
		{
			i_face = i_f;
			i_vert = i_v;
		}
	};


// enable consistent floating point for this function.
#pragma optimize("p",on)

	//******************************************************************************************
	void CMesh::CalculateVertexNormals(CHeap& mh, CAngle ang_max_vertex_share)
	{
		const TReal rMIN_COSINE_EQUAL	= 0.999;	// Tolerance for considering face normals equal.

		float f_max_cos_split = ang_max_vertex_share.fCos();


		// Point to face mapping array of lists.
		CAArray< std::list<SFaceVert> > paPointToFaces(mh.mav3Points.uLen);

		// Create a point to face mapping.
		for (int i_face = 0; i_face < mh.mampPolygons.uLen; i_face++)
		{
			// Add to the appropriate point list.
			for (int i_v = 0; i_v < mh.mampPolygons[i_face].papmvVertices.uLen; i_v++)
			{
				uint u_index = mh.mampPolygons[i_face].papmvVertices[i_v]->pv3Point - mh.mav3Points;
				paPointToFaces[u_index].push_front(SFaceVert(i_face, i_v));
			}
		}

		// Calculate the normals that haven't yet been set in each SVertex.
		for (uint u_mv = 0; u_mv < mh.mamvVertices.uLen; u_mv++)
		{
			SVertex* pmv = &mh.mamvVertices[u_mv];

			if (!pmv->bSpecified)
			{
				//
				// This normal has not been set, so we must calculate it. There is no reference 
				// back from a vertex to the polygons that use it, so we need to do a search 
				// through the array of polygons in the mesh to find the ones that use this 
				// vertex's point.  We combine these into groups of faces that have similar 
				// normals.  We then calculate the normal from the average of these faces.  
				// So there.
				//

				// A 2D array for storing all the sharing groups.
				typedef CMSArray<SVertexRecord, 64>  VertexSharingGroup;
				CMSArray< VertexSharingGroup, 16> sasamvr_groups;

				// Determine array index for face list.
				uint u_index = pmv->pv3Point - mh.mav3Points;

				// Look at polygons that share this point.
				for (std::list<SFaceVert>::iterator it = paPointToFaces[u_index].begin(); it != paPointToFaces[u_index].end(); it++)
				{
					SPolygon& mp = mh.mampPolygons[(*it).i_face];

					int i_v = (*it).i_vert;

					Assert(mp.papmvVertices[i_v]->pv3Point == pmv->pv3Point);

					CDir3<> d3_face = mp.plPlane.d3Normal;
	
					// The face shares the point of this vertex.  We don't care whether it 
					// currently shares the actual vertex.
					Assert(!mp.papmvVertices[i_v]->bSpecified);

					// Try to fit into an existing group.
					int i_g;
					for (i_g = 0; i_g < sasamvr_groups.uLen; i_g++)
					{
						// Search the faces in each group.
						for (int i_f = 0; i_f < sasamvr_groups[i_g].uLen; i_f++)
						{
							if (sasamvr_groups[i_g][i_f].v3Face.bIsZero())
								// This entry's normal is inactive.
								continue;

							TReal r_cosine = d3_face * sasamvr_groups[i_g][i_f].v3Face;
							if (r_cosine >= rMIN_COSINE_EQUAL)
							{
								// This face normal is already in a list.
								// Add the vertex pointer, but set this face normal to 0, so it
								// doesn't get included in the average.  (We don't want to count
								// identical normals twice).

								// If we assert because this array overflows, we have too many sides in a single polygon.
								sasamvr_groups[i_g] << SVertexRecord(mp.papmvVertices[i_v], CVector3<>(0, 0, 0));
								goto continue_face;
							}

							if (r_cosine <= f_max_cos_split)
							{
								// This face normal is different, so doesn't belong in this group.
								goto continue_group;
							}
						}

						// This face normal is similar to all others in the group.
						// Therefore, it belongs in this group.
						break;

					continue_group:
						;
					}

					if (i_g == sasamvr_groups.uLen)
					{
						// Didn't match any groups.  Create a new one.
						sasamvr_groups << VertexSharingGroup();
					}
					sasamvr_groups[i_g] << SVertexRecord(mp.papmvVertices[i_v], d3_face);

				continue_face:
					;
				}

				//
				// For each group, calculate an average normal, and assign it to the vertices
				// of the faces in that group.
				//
				for (int i_g = 0; i_g < sasamvr_groups.uLen; i_g++)
				{
					// Average all face normals in this group.
					CVector3<> v3_normal(0, 0, 0);

					int i_f;
					for (i_f = 0; i_f < sasamvr_groups[i_g].uLen; i_f++)
						v3_normal += sasamvr_groups[i_g][i_f].v3Face;

					CDir3<> d3_normal = v3_normal.bIsZero() ? CDir3<>() : CDir3<>(v3_normal);

					// Assign to all vertices.
					for (i_f = 0; i_f < sasamvr_groups[i_g].uLen; i_f++)
					{
						if (!sasamvr_groups[i_g][i_f].pmvVertex->bSpecified)
						{
							sasamvr_groups[i_g][i_f].pmvVertex->d3Normal = d3_normal;

							// Flag as specified, so we don't calculate this all over again.
							sasamvr_groups[i_g][i_f].pmvVertex->bSpecified = true;
						}
					}
				}
			}
		}
	}

// restore optimizations
#pragma optimize("",on)

	//******************************************************************************************
	void CMesh::MergeVertices(CHeap& mh)
	{
		//
		// Optimise the mesh by finding any duplicate vertices, and merging them.
		//
		CLArray(uint, pau_remap, mh.mamvVertices.uLen);

		for (uint u = 0; u < mh.mamvVertices.uLen; u++)
		{
			SVertex* pmv = &mh.mamvVertices[u];

			// Default remapping is identity.
			pau_remap[u] = u;

			for (uint u_2 = 0; u_2 < u; u_2++)
			{
				SVertex* pmv2 = &mh.mamvVertices[u_2];
				if (*pmv == *pmv2)
				{
					// Sorry, u.  You don't stay.
					pau_remap[u] = u_2;
					break;
				}
			}
		}

		// Now remap the vertices.
		for (SPolygon* pmp = mh.mampPolygons; pmp < mh.mampPolygons.end(); pmp++)
		{
			for (int i = 0; i < pmp->iNumVertices(); i++)
				pmp->papmvVertices[i] = &mh.mamvVertices[ pau_remap[(uint)(pmp->papmvVertices[i] - mh.mamvVertices)] ];
		}

/*
		//
		// Do da same t'ing wit' points.
		//
		for (u = 0; u < mh.mav3Points.uLen; u++)
		{
			CVector3<>* pv3 = &mh.mav3Points[u];

			// Default remapping is identity.
			pau_remap[u] = u;

			for (uint u_2 = 0; u_2 < u; u_2++)
			{
				CVector3<>* pv3 = &mh.mav3Points[u_2];
				if (*pv3 == *pv32)
				{
					// Sorry, u.  You don't stay.
					pau_remap[u] = u_2;
					break;
				}
			}
		}

		// Now remap the points.
		for (SVertex* pmv = mh.mamvVertices; pmv < mh.mamvVertices.end(); pmv++)
		{
			pmv->pv3Point = &mh.mav3Points[ pau_remap[pmv->pv3Point - mh.mav3Points] ];
		}
*/
	}

	//******************************************************************************************
	void CMesh::MergePolygons(CHeap& mh)
	{
		// Find polygons which are co-planar, have the same surface, and share an edge.  
		// Merge these together.

		// WARNING!  Heavy data mungeing herein.  Array data is reallocated, copied, inserted, deleted,
		// and mutilated, all in a manner intimately tied to the mesh's data structure.

#if bLOG_MERGE
		conLoadLog.Print(" [%d] Merge\n", iNumPolygons());
#endif
		bool b_merged = false;
		int i_reject_texture = 0;
		int i_reject_normal = 0;

		const float f_coplanar_tol	 = i_shadow_mesh ? fCOPLANAR_TOLERANCE * 5 : fCOPLANAR_TOLERANCE;
		const float f_tex_grad_tol	 = i_shadow_mesh ? fTEX_GRAD_TOLERANCE * 5 : fTEX_GRAD_TOLERANCE;
		const float f_colinear_tol	 = i_shadow_mesh ? fCOLINEAR_TOLERANCE * 1 : fCOLINEAR_TOLERANCE;
		const float f_tex_linear_tol = i_shadow_mesh ? fTEX_LINEAR_TOLERANCE * 5 : fTEX_LINEAR_TOLERANCE;

		// Get the maximum negative coordinate of the mesh, and use it to offset vectors.
		// This is required by the CLine2D class, which needs positive numbers.
		TReal r_offset = -Min(Min(bvbVolume[ebeMIN_X], bvbVolume[ebeMIN_Y]), bvbVolume[ebeMIN_Z]) + 1.0;
		const CVector2<> v2_offset(r_offset, r_offset);

#if (0)
		// Remove any degenerate polygons.
		SPolygon* pmp_1 = mh.mampPolygons;
		while (pmp_1 < mh.mampPolygons.end())
		{
			// Check for degenerate polygons.
			if (pmp1.rGetArea() < 0.0001)
			{
				// Delete pmp_1 from the array.
				for (SPolygon* pmp_2 = pmp_1; pmp_2 < mh.mampPolygons.end()-1; pmp_2++)
					pmp_2[0] = pmp_2[1];
				mh.mampPolygons.uLen--;
				continue;
			}

			// Next polygon.
			pmp_1++;
		}
#endif

		// Count the number of polygons split because of concavity.
		// These are stuck onto the end of the array, and we must avoid re-merging them.
		int i_split_polygons = 0;

		for (SPolygon* pmp = mh.mampPolygons; pmp < mh.mampPolygons.end(); pmp++)
		{
			bool b_merged_poly = false;

			CMatrix3<> mx3_flatten = CRotate3<>(pmp->plPlane.d3Normal, d3ZAxis);

#if bLINE_DRAW
			// Set virtual rectangle to reverse Y direction, and scale from about 1.4.
			CRectangle<> rect_extents(3.0, -3.0);
#endif

			// Find the texture gradients.  They must match in merged polygons.
			GradientData gradients(*pmp, mx3_flatten);

			for (SPolygon* pmp_2 = pmp+1; pmp_2 < mh.mampPolygons.end() - i_split_polygons; pmp_2++)
			{
				// Compare these two.  Must share surface pointer and plane equation.
				if (pmp->pSurface == pmp_2->pSurface && Fuzzy(pmp->plPlane, f_coplanar_tol) == pmp_2->plPlane)
				{
					// Co-planar.  Now check for a shared edge.
					for (uint u = 0; u < pmp->papmvVertices.uLen; u++)
					{
						// Get the two points of this edge.
						uint u_b = (u + 1 < pmp->papmvVertices.uLen ? u + 1 : 0);

						// Look in the second polygon for this edge (going opposite direction).
						for (uint u_2 = 0; u_2 < pmp_2->papmvVertices.uLen; u_2++)
						{
							// Get the two points of this edge.
							uint u_b2 = (u_2 + 1 < pmp_2->papmvVertices.uLen ? u_2 + 1 : 0); 

							//
							// Compare the vertices.  Since identical vertices have already been merged,
							// the pointers must be the same.
							//
							if (pmp->papmvVertices[u]   == pmp_2->papmvVertices[u_b2] && 
								pmp->papmvVertices[u_b] == pmp_2->papmvVertices[u_2])
							{
#if bLINE_DRAW
								// Draw merging polygons, in green.
								CDraw draw(prasMainScreen, rect_extents);
								draw.Colour(CColour(0.0, 1.0, 0.0));

								if (!b_merged_poly)
								{
									// First merge.  Clear, and draw the first poly.
									draw.Clear(CColour(0, 0, 0));
									DrawPolygon(draw, *pmp, mx3_flatten);
								}

								DrawPolygon(draw, *pmp_2, mx3_flatten);
								draw.Flip();
#endif
								// Find the texture gradients.  They must match in merged polygons.
								GradientData gradients_2(*pmp_2, mx3_flatten);

								if (!gradients.texture_gradients_equal(gradients_2, f_tex_grad_tol))
								{
									i_reject_texture++;
									goto continue_poly_2;
								}
						
								if (bMERGE_CHECK_NORMALS)
								{
									if (gradients.normal_gradients_equal(gradients_2))
									{
										i_reject_normal++;
										goto continue_poly_2;
									}
								}

								// BINGO! Total match-up.  Find how many vertices coincide, if more than 2.
								int i_merged_vertices = 2;

								// Search for extra matches in 1-forward, 2-backward.
								while (u_b != u && u_b2 != u_2)
								{
									uint u_c = (u_b + 1 < pmp->papmvVertices.uLen ? u_b + 1 : 0);
									uint u_a2  = (u_2 > 0 ? u_2 - 1 : pmp_2->papmvVertices.uLen-1);
									if (pmp->papmvVertices[u_c] == pmp_2->papmvVertices[u_a2])
									{
										// More match-ups.
										u_b = u_c;
										u_2 = u_a2;
										i_merged_vertices++;
									}
									else
										break;
								}

								// Search for extra matches in 1-backward, 2-forward.
								while (u_b != u && u_b2 != u_2)
								{
									uint u_c2 = (u_b2 + 1 < pmp_2->papmvVertices.uLen ? u_b2 + 1 : 0);
									uint u_a  = (u > 0 ? u - 1 : pmp->papmvVertices.uLen-1);
									if (pmp->papmvVertices[u_a] == pmp_2->papmvVertices[u_c2])
									{
										// More match-ups.
										u_b2 = u_c2;
										u = u_a;
										i_merged_vertices++;
									}
									else
										break;
								}

								// We've matched all contiguous edges.  Now, if there are OTHER 
								// vertices somewhere that coincide (or have the same point), 
								// we have a problem.  We can't merge, or we'd have holes and such.
								for (uint u_x = (u_b+1 < pmp->papmvVertices.uLen ? u_b+1 : 0); u_x != u; )
								{
									const CVector3<>* pv3 = &pmp->v3Point(u_x);

									for (uint u_x2 = (u_b2+1 < pmp_2->papmvVertices.uLen ? u_b2+1 : 0); u_x2 != u_2; )
									{
										const CVector3<>* pv3_2 = &pmp_2->v3Point(u_x2);
										if (pv3 == pv3_2)
										{
											// This is a deal-breaker.
											psRejectHole.Add(0, 1);
											goto continue_poly_2;
										}
										if (++u_x2 == pmp_2->papmvVertices.uLen)
											u_x2 = 0;
									}
									if (++u_x == pmp->papmvVertices.uLen)
										u_x = 0;
								}

#if bLOG_MERGE
								conLoadLog.Print("  [%d] Merge %dv: P%d [%dv] (%d..%d) with P%d [%dv] (%d..%d): ",
									mh.mampPolygons.uLen,		i_merged_vertices,
									pmp - mh.mampPolygons,	pmp->papmvVertices.uLen,	u,	u_b,	
									pmp_2 - mh.mampPolygons,	pmp_2->papmvVertices.uLen,	u_2, u_b2
								);
#endif

								// Calculate number of vertices to add to polygon 1's array.
								int i_extra_vertices = pmp_2->papmvVertices.uLen - 2*i_merged_vertices + 2;

								CPArray<SVertex*> papmv_new;
								if (b_merged_poly)
								{
									// This polygon's vertex array was already allocated on the heap.
									// Re-use part of it by starting it at the first kept vertex, u_b.
									Assert(pmp->papmvVertices.end() == mh.mapmvVertices.end());
									mh.mapmvVertices.paAlloc(i_extra_vertices + u_b);
									papmv_new = CPArray<SVertex*>(pmp->papmvVertices.uLen + i_extra_vertices, pmp->papmvVertices + u_b);
								}
								else
								{
									// Allocate a new vertex pointer array from the mesh heap.
									papmv_new = mh.mapmvVertices.paAlloc(pmp->papmvVertices.uLen + i_extra_vertices);
								}

								// Copy the vertex pointers in.
								uint u_new = 0;

								// From the first polygon: u_b to u.
								while (u_b != u)
								{
									papmv_new[u_new++] = pmp->papmvVertices[u_b];
#if bLOG_MERGE
									conLoadLog.Print(" %d", pmp->papmvVertices[u_b] - papmvVertices);
#endif
									u_b++;
									if (u_b == pmp->papmvVertices.uLen)
										u_b = 0;
								}

#if bLOG_MERGE
								conLoadLog.Print(",");
#endif
								// From the second polygon: u_b2 to u_2.
								while (u_b2 != u_2)
								{
									papmv_new[u_new++] = pmp_2->papmvVertices[u_b2];
#if bLOG_MERGE
									conLoadLog.Print(" %d", pmp_2->papmvVertices[u_b2] - papmvVertices);
#endif
									u_b2++;
									if (u_b2 == pmp_2->papmvVertices.uLen)
										u_b2 = 0;
								}

#if bLOG_MERGE
								conLoadLog.Print("\n");
#endif
								pmp->papmvVertices = papmv_new;

								// Delete pmp_2 from the array.
								for (SPolygon* pmp_3 = pmp_2; pmp_3 < mh.mampPolygons.end()-1; pmp_3++)
									pmp_3[0] = pmp_3[1];
								mh.mampPolygons.uLen--;

								b_merged_poly = true;
								b_merged = true;

								// Reset the second polygon back to the start, to check all polygons again.
								// This will catch some we rejected because there was not yet a shared edge.
								pmp_2 = pmp;
								goto continue_poly_2;
							}
						}
					}
				}

				continue_poly_2:
					;
			}

//			if (b_merged_poly)
			{
				bool b_check = true;

				while (b_check && pmp->papmvVertices.uLen > 3)
				{
					b_check = false;

					// Check for co-linear edges.
					uint u_v;
					for (u_v = 0; u_v < pmp->papmvVertices.uLen; u_v++)
					{
						uint u_va = u_v;
						uint u_vb = (u_v + 1 < pmp->papmvVertices.uLen ? u_v + 1 : 0);
						uint u_vc = (u_vb + 1 < pmp->papmvVertices.uLen ? u_vb + 1 : 0);

						CVector3<> v3_edge1(pmp->v3Point(u_vc) - pmp->v3Point(u_vb)),
								   v3_edge0(pmp->v3Point(u_vb) - pmp->v3Point(u_va));

						if (v3_edge0.bIsZero() || v3_edge1.bIsZero())
							ErrorDegenerate(pmp, mh);
						TReal r_dot = CDir3<>(v3_edge1) * CDir3<>(v3_edge0);

						if (Fuzzy(Abs(r_dot), f_colinear_tol) == 1.0)
						{
							// Edges are co-linear.  They may join in a positive or negative direction.
							// In either case, we want to eliminate the middle vertex.
							bool b_remove;
							if (r_dot > 0)
							{
								// Edges connect positively.  We can only join them if the texture coord
								// is actually intermediate.
								TReal r_tb = (pmp->v3Point(u_vb) - pmp->v3Point(u_va)).tLen() /
											 (pmp->v3Point(u_vc) - pmp->v3Point(u_va)).tLen();
								Assert(r_tb > 0 && r_tb < 1);
								CVector2<> v2_diff = pmp->tcTexCoord(u_va) * (1.0 - r_tb) + pmp->tcTexCoord(u_vc) * r_tb
												   - pmp->tcTexCoord(u_vb);
								b_remove = Fuzzy(v2_diff.tX, f_tex_linear_tol) == 0 && 
										   Fuzzy(v2_diff.tY, f_tex_linear_tol) == 0;
							}
							else
								// Removing degenerate edges.  Don't care about texture coords.
								b_remove = true;

							if (b_remove)
							{
								// If we have a colinear edge in a triangle, it's degenerate.
								if (pmp->papmvVertices.uLen <= 3)
								{
									ErrorDegenerate(pmp, mh);
									continue;
								}

#if bLINE_DRAW
								CDraw draw(prasMainScreen, rect_extents);
								if (!b_merged_poly)
									// Clear for first draw.
									draw.Clear(CColour(0, 0, 0));

								// Re-draw the polygon in green.
								draw.Colour(CColour(0.0, 1.0, 0.0));
								DrawPolygon(draw, *pmp, mx3_flatten);

								// Draw the removed segment in red.
								draw.Colour(CColour(1.0, 0.0, 0.0));
								draw.Line
								(
									pmp->v3Point(u_vb) * mx3_flatten,
									pmp->v3Point(u_vc) * mx3_flatten
								);
								draw.Flip();
#endif
								// Remove centre vertex.
								for (; u_vb < pmp->papmvVertices.uLen-1; u_vb++)
									pmp->papmvVertices[u_vb] = pmp->papmvVertices[u_vb+1];
								pmp->papmvVertices.uLen--;
								u_v--;
							}
						}
					}

					// Check for a concave polygon.
					// Each subtriangle must have the same plane.
					// If a segment is concave, the triangle normal will be opposite.
					for (u_v = 0; u_v < pmp->papmvVertices.uLen; u_v++)
					{
						uint u_va = u_v;
						uint u_vb = (u_v + 1 < pmp->papmvVertices.uLen ? u_v + 1 : 0);
						uint u_vc = (u_vb + 1 < pmp->papmvVertices.uLen ? u_vb + 1 : 0);

						// If the edge is concave, split it.  All tri-points should form a plane, because
						// we have removed co-linear edges.
						CPlane pl(pmp->v3Point(u_va), pmp->v3Point(u_vb), pmp->v3Point(u_vc));
						//if (Fuzzy(pmp->plPlane, f_coplanar_tol) != pl)
						if (pl.d3Normal * pmp->plPlane.d3Normal < 0)
						{
							if (pmp->papmvVertices.uLen <= 3)
								ErrorDegenerate(pmp, mh);
#if bLINE_DRAW
							CDraw draw(prasMainScreen, rect_extents);
							if (!b_merged_poly)
								// Clear for first draw.
								draw.Clear(CColour(0, 0, 0));

							// Re-draw the polygon in green.
							draw.Colour(CColour(0.0, 1.0, 0.0));
							DrawPolygon(draw, *pmp, mx3_flatten);

							// Draw last half of the segment in red.
							draw.Colour(CColour(1.0, 0.0, 0.0));
							draw.Line
							(
								(pmp->v3Point(u_va) * 0.25 + pmp->v3Point(u_vb) * 0.75) * mx3_flatten,
								pmp->v3Point(u_vb) * mx3_flatten
							);
							draw.Flip();
#endif

							// Find a target vertex that allows this polygon to be convex.
							for (;;)
							{
								u_vc++;
								if (u_vc == pmp->papmvVertices.uLen)
									u_vc = 0;
								if (u_vc == u_va)
								{
									u_vc = u_va;
									Assert(false);
									break;
								}
								int i_colinear = iColinear(pmp->v3Point(u_va), pmp->v3Point(u_vb), pmp->v3Point(u_vc));
								if (i_colinear < 0)
									// Don't even think about splitting here.
									continue;
								if (i_colinear > 0 ||
									// Allow a colinear edge to be created.  This will be removed
									// or split next time through loop.
									Fuzzy(pmp->plPlane, f_coplanar_tol) == 
									CPlane(pmp->v3Point(u_va), pmp->v3Point(u_vb), pmp->v3Point(u_vc)))
								{
									// Before we accept this edge, we must make sure it doesn't cross any other edges.
									CVector2<> v2_b = pmp->v3Point(u_vb) * mx3_flatten;
									CVector2<> v2_c = pmp->v3Point(u_vc) * mx3_flatten;

#if 0
									for (uint u_1 = 0; u_1 < pmp->papmvVertices.uLen; u_1++)
									{
										uint u_2 = (u_1 + 1) % pmp->papmvVertices.uLen;

										// Test only non-connected edges.
										if (u_1 != u_vb && u_1 != u_vc && u_2 != u_vb && u_2 != u_vc)
										{
											CVector2<> v2_1 = pmp->v3Point(u_1) * mx3_flatten;
											CVector2<> v2_2 = pmp->v3Point(u_2) * mx3_flatten;

											CVector2<> v2_dummy_result;			// We don't care about the actual point of intersection.
											if (bIntersection2D(v2_dummy_result, v2_v, v2_c, v2_1, v2_2))
												break;
										}
									}
#else
									// Get flattened points, and offset them to positive, so CLine2D intersection test works.
									CLine2D line(v2_b + v2_offset, v2_c + v2_offset);
									uint u_1;
									for (u_1 = 0; u_1 < pmp->papmvVertices.uLen; u_1++)
									{
										uint u_2 = (u_1 + 1 < pmp->papmvVertices.uLen ? u_1 + 1 : 0);

										// Test only non-connected edges.
										if (u_1 != u_vb && u_1 != u_vc && u_2 != u_vb && u_2 != u_vc)
										{
											CVector2<> v2_1 = pmp->v3Point(u_1) * mx3_flatten;
											CVector2<> v2_2 = pmp->v3Point(u_2) * mx3_flatten;

											if (line.bDoesIntersect(v2_1 + v2_offset, v2_2 + v2_offset))
												break;
										}
									}
#endif
									if (u_1 < pmp->papmvVertices.uLen)
										// Premature break indicates intersection.  Can't use this edge.
										continue;

									// We found an edge.
									break;
								}
							}

							if (u_vb > u_vc)
								Swap(u_vb, u_vc);
							if (!Verify(u_vc - u_vb > 1))
								continue;

#if bLOG_MERGE
							conLoadLog.Print("   Split P%d [%dv] @(%d,%d) V(%d,%d):",
								pmp - mh.mampPolygons, pmp->papmvVertices.uLen,
								u_vb, u_vc, 
								pmp->papmvVertices[u_vb] - papmvVertices, 
								pmp->papmvVertices[u_vc] - papmvVertices
							);
#endif

/*
#if bLINE_DRAW
							// Draw splitting line in blue.
							draw.Colour(CColour(0.0, 0.0, 1.0));
							draw.Line(pmp->v3Point(u_vb) * mx3_flatten, pmp->v3Point(u_vc) * mx3_flatten);
							draw.Flip();
#endif
*/
							// Create a new polygon on the temporary heap, copied from original.
							SPolygon* pmp_2 = new(mh.mampPolygons) SPolygon(*pmp);
							i_split_polygons++;

							// Allocate this polygon's new vertex pointer array.
							int i_num_vertices_2 = u_vc + 1 - u_vb;
							pmp_2->papmvVertices = mh.mapmvVertices.paAlloc(i_num_vertices_2);

							// Fill the array.
							for (uint u_v2 = 0; u_v2 < pmp_2->papmvVertices.uLen; u_v2++)
								pmp_2->papmvVertices[u_v2] = pmp->papmvVertices[u_vb + u_v2];

							// Remove these vertices from polygon 1's array.
							int i_removed_vertices = pmp_2->papmvVertices.uLen - 2;
							for (uint u_v1 = u_vb + 1; u_v1 < pmp->papmvVertices.uLen - i_removed_vertices; u_v1++)
								pmp->papmvVertices[u_v1] = pmp->papmvVertices[u_v1 + i_removed_vertices];
							pmp->papmvVertices.uLen -= i_removed_vertices;

#if bLINE_DRAW
							// Draw new polygon in blue.
							draw.Colour(CColour(0.0, 0.0, 1.0));
							DrawPolygon(draw, *pmp_2, mx3_flatten);
							draw.Flip();
#endif

#if bLOG_MERGE
							// Dump the results, for debugging.
							for (u_v1 = 0; u_v1 < pmp->papmvVertices.uLen; u_v1++)
								conLoadLog.Print(" %d", pmp->papmvVertices[u_v1] - papmvVertices);
							conLoadLog.Print(", P%d: ", pmp_2 - mh.mampPolygons);
							for (u_v2 = 0; u_v2 < pmp_2->papmvVertices.uLen; u_v2++)
								conLoadLog.Print(" %d", pmp_2->papmvVertices[u_v2] - papmvVertices);
							conLoadLog.Print("\n");
#endif
							// Check again.
							b_check = true;
							psSplitConcave.Add(0, 1);
							break;
						}
					}
				}
			}
		}

		// Log merge failures.
		if (i_reject_texture)
		{
			#if (VER_DEBUG)
			dprintf(" %d merge failures due to texture in %s.\n", i_reject_texture, CLoadWorld::pgonCurrentObject->strObjectName);
				conLoadLog.Print(" %d merge failures due to texture.\n", i_reject_texture);
			#endif

			psRejectTexture.Add(0, i_reject_texture);
		}

		if (i_reject_normal)
		{
			#if (VER_DEBUG)
			dprintf(" %d merge failures due to normal in %s.\n", i_reject_normal, CLoadWorld::pgonCurrentObject->strObjectName);
				conLoadLog.Print(" %d merge failures due to normal.\n", i_reject_normal);
			#endif

			psRejectNormal.Add(0, i_reject_normal);
		}
	}

	//******************************************************************************************
	void CMesh::MakePolygonsPlanar(CHeap& mh)
	{
/*
		UNIMPLEMENTED
		for (SPolygon* pmp = mh.mampPolygons; pmp < mh.mampPolygons.end(); pmp++)
		{
			CMatrix3<> mx3_flatten = CRotate3<>(pmp->plPlane.d3Normal, d3ZAxis);

			// Find the texture gradients.  They must match in merged polygons.
			CVector2<> v2_grad_u, v2_grad_v;
			CVector2<> v2_grad_nx, v2_grad_ny;
			GetGradients(*pmp, mx3_flatten, &v2_grad_u, &v2_grad_v, &v2_grad_nx, &v2_grad_ny);
		}
*/
	}

	//******************************************************************************************
	void CMesh::SplitIntersectingPolygons(CHeap& mh)
	{
		int i_intersections = 0, i_hidden = 0, i_quashed = 0, i_overlap = 0;

		// Double-loop search for intersections.  I know you're there...
		int i;
		for (i = 0; i < mh.mampPolygons.uLen; i++)
		{
			// Try splitting mp by all other polygons.
			SPolygon& mp = mh.mampPolygons[i];

			// Ignore this polygon if it is for occlusion.
			if (mp.pSurface->ptexTexture->seterfFeatures[erfOCCLUDE])
				continue;
			
			// Do not process polygons to be removed.
			if (mp.papmvVertices.uLen == 0)
				continue;

			// Compare with other polygons.
			for (int i_2 = 0; i_2 < mh.mampPolygons.uLen; i_2++)
			{
				if (i_2 == i)
					continue;

				// Construct a clip polygon from polygon 2.
				SPolygon& mp_2 = mh.mampPolygons[i_2];

				// Ignore this polygon if it is for occlusion.
				if (mp_2.pSurface->ptexTexture->seterfFeatures[erfOCCLUDE])
					continue;

				// Do not process polygons to be removed.
				if (mp_2.papmvVertices.uLen == 0)
					continue;

				CClipPolygon clpg(mp_2);

				// Save original mp in case we need to restore it.
				SPolygon mp_save = mp;

				// Split mp by polygon clpg if they intersect.
				ESideOf esf = clpg.esfClipPolygon(mp, mh, esfINSIDE | esfOUTSIDE);
				if (esf == esfINTERSECT)
				{
					ESideOf esf_edges;

					// A new polygon has been created, OUTSIDE of clpg.  mp is now inside it.  
					// The new polygon does not inherit mp's hidden flag.
					mh.mampPolygons(-1).bHidden = false;

					esf = clpg.esfIntersects(mp, mh, &esf_edges);
					Assert(esf != esfINTERSECT);

					TReal r_area_in = mp.rGetArea();
					TReal r_area_out = mh.mampPolygons(-1).rGetArea();

					// Throw away tiny polygons.
					if (bQuashPolygons && r_area_out < Max(r_area_in * fMIN_SPLIT_RATIO, rMIN_POLYGON_AREA))
					{
						// Mark it as unused.
						mh.mampPolygons(-1).papmvVertices.uLen = 0;
						i_quashed++;
#if bLOG_SPLIT
						conLoadLog.Print("   Quashed outside\n");
#endif
					}
					else if (bQuashPolygons && r_area_in < Max(r_area_out * fMIN_SPLIT_RATIO, rMIN_POLYGON_AREA))
					{
						// Mark it as unused.
						mp.papmvVertices.uLen = 0;
						i_quashed++;
#if bLOG_SPLIT
						conLoadLog.Print("   Quashed inside\n");
#endif
					}
					else if (esf_edges == esfINSIDE)
					{
						// mp has been made redundant, unless another polygon comes along to save it.
						mp.bHidden = true;
					}
					else if (i_2 > i)
					{
						//
						// This is the first time this pair has been encountered.
						// Since polygon 1 turned into 2 polygons, and neither was 
						// hidden or quashed, undo this split.
						// We'll let polygon 1 split polygon 2 when the pair is encountered again.
						//

						// Restore polygon 1.
						mp = mp_save;

						// Remove the just-created outside polygon.
						mh.mampPolygons.uLen--;

						continue;
					}

					i_intersections++;
#if bLOG_SPLIT
					conLoadLog.Print("  Split #%d\\%d %f, %f\n", i, i_2, r_area_in, r_area_out);
#endif

					if (mp.papmvVertices.uLen == 0)
						// This polygon was just quashed.
						break;
				}

				else if (esf == esfOUTSIDE)
				{
					if (mp.bHidden)
					{
						// mp is outside of mp_2.  If it's actually TOUCHING mp_2, then it's
						// no longer hidden.  
						for (int i = 0; i < mp.iNumVertices(); i++)
							if (mp_2.plPlane.esfSideOf(mp.v3Point(i)) != esfOUTSIDE)
							{
								// A point is on mp_2's plane.
								mp.bHidden = false;
								break;
							}
					}
				}

				else if (esf == esfON)
				{
					// These polygons coincide; if they face the same way, that's bad.
					TReal r_dot = mp.plPlane.d3Normal * mp_2.plPlane.d3Normal;
					if (Fuzzy(r_dot) == 1)
						i_overlap++;
				}
			}

			TReal r_area;

			if (mp.bHidden)
			{
				// Sorry, mp, you didn't make the cut (literally, har har har).
				// Mark it as removed by setting its vertices to 0.
				mp.papmvVertices.uLen = 0;
				i_hidden++;
#if bLOG_SPLIT
				conLoadLog.Print("   Hidden\n");
#endif
			}

			else if (bQuashPolygons && mp.papmvVertices.uLen > 0 &&
			(r_area = mp.rGetArea()) < rMIN_POLYGON_AREA)
			{
				// Mark it also as removed.
				mp.papmvVertices.uLen = 0;
				i_quashed++;
#if bLOG_SPLIT
				conLoadLog.Print("   #%d Quashed %f\n", i, r_area);
#endif
			}
		}

		if (i_hidden || i_quashed)
		{
			// Remove unused polygons.
			int i_new = 0;
			for (i = 0; i < mh.mampPolygons.uLen; i++)
			{
				if (mh.mampPolygons[i].papmvVertices.uLen > 0)
					// This polygon has earned its keep by being visible.
					mh.mampPolygons[i_new++] = mh.mampPolygons[i];
			}
			mh.mampPolygons.uLen = i_new;
		}

		// Print the number of intersections found.
		if (i_intersections || i_quashed)
		{
			dprintf(" Polys %d", mh.mampPolygons.uLen);
			if (i_intersections)
				dprintf(", intersect %d", i_intersections);
			if (i_hidden)
				dprintf(", hidden %d", i_hidden);
			if (i_quashed)
				dprintf(", quashed %d", i_quashed);
			if (i_overlap)
				dprintf(", overlapping %d", i_quashed);
			dprintf("\n");
#if bLOG_SPLIT
			conLoadLog.Print(" Polys %d", mh.mampPolygons.uLen);
			if (i_intersections)
				conLoadLog.Print(", intersect %d", i_intersections);
			if (i_hidden)
				conLoadLog.Print(", hidden %d", i_hidden);
			if (i_quashed)
				conLoadLog.Print(", quashed %d", i_quashed);
			conLoadLog.Print("\n");
#endif
		}
	}

	//******************************************************************************************
	void CMesh::AllocateFinalData(CHeap& mh)
	{
		// Polygons.
		pampPolygons = mh.mampPolygons.paDup( new(fhGlobalMesh) SPolygon[mh.mampPolygons.size()] );
		MEMLOG_ADD_COUNTER(emlMeshHeap,pampPolygons.uMemSize());

		// Vertex pointers.
		uint u_vertex_pointers = 0;
		SPolygon* pmp;
		for (pmp = pampPolygons; pmp < pampPolygons.end(); pmp++)
			u_vertex_pointers += pmp->papmvVertices.uLen;

		// Allocate a CMArray from the Mesh fast heap
		CMArray<SVertex*> mapmv(u_vertex_pointers, new(fhGlobalMesh) SVertex*[u_vertex_pointers] );
		MEMLOG_ADD_COUNTER(emlMeshHeap,mapmv.uMemSize());

		for (pmp = pampPolygons; pmp < pampPolygons.end(); pmp++)
		{
			CPArray<SVertex*> papmv_poly = mapmv.paAlloc(pmp->papmvVertices.uLen);
			Copy(papmv_poly, pmp->papmvVertices);
			pmp->papmvVertices = papmv_poly;
		}
		papmvVertices = mapmv;

		//
		// Vertices.
		// Remove no-longer-used vertices.  These have been orphaned by polygon merging
		// and duplicate vertex merging.
		//

		// Create an array to store the mapping between old and new vertex pointers.
		CLArray(uint, pau_remap, Max(mh.mamvVertices.size(), mh.mav3Points.size()));
		pau_remap.Fill(0);

		// Identify and count used vertices.
		uint u_vertex_count = 0;
		for (pmp = pampPolygons; pmp < pampPolygons.end(); pmp++)
		{
			for (uint u = 0; u < pmp->papmvVertices.uLen; u++)
			{
				uint u_index = pmp->papmvVertices[u] - mh.mamvVertices;
				if (!pau_remap[u_index])
				{
					pau_remap[u_index] = 1;
					u_vertex_count++;
				}
			}
		}

		// Allocate and transfer the vertices.
		pamvVertices = CPArray<SVertex>(u_vertex_count, new(fhGlobalMesh) SVertex[u_vertex_count] );
		MEMLOG_ADD_COUNTER(emlMeshHeap,pamvVertices.uMemSize());

		u_vertex_count = 0;
		uint u;
		for (u = 0; u < mh.mamvVertices.size(); u++)
		{
			if (pau_remap[u])
			{
				pau_remap[u] = u_vertex_count;
				pamvVertices[u_vertex_count++] = mh.mamvVertices[u];
			}
		}

#if bLOG_MERGE
		conLoadLog.Print(" Vertices: %d -> %d\n", dbamv.uLen, pamvVertices.uLen);
#endif

		// Remap the polygons' pointers to vertices.
		for (pmp = pampPolygons; pmp < pampPolygons.end(); pmp++)
		{
			for (int i_v = 0; i_v < pmp->papmvVertices.uLen; i_v++)
			{
				// Find original index, and remap it.
				int i_index = pmp->papmvVertices[i_v] - mh.mamvVertices;
				pmp->papmvVertices[i_v] = &pamvVertices[ pau_remap[i_index] ];
			}
		}

		//
		// Points.
		// Remove no-longer-used points.
		//

		// Identify and count used points in just-compressed vertex array.
		pau_remap.Fill(0);
		uint u_point_count = 0;
		for (SVertex* pmv = pamvVertices; pmv < pamvVertices.end(); pmv++)
		{
			uint u_index = pmv->pv3Point - mh.mav3Points;
			if (!pau_remap[u_index])
			{
				pau_remap[u_index] = 1;
				u_point_count++;
			}
		}

		// Allocate in the mesh global heap and transfer the points.
		pav3Points = CPArray< CVector3<> >(u_point_count, new(fhGlobalMesh) CVector3<>[u_point_count] );
		MEMLOG_ADD_COUNTER(emlMeshHeap,pav3Points.uMemSize());

		u_point_count = 0;
		for (u = 0; u < mh.mav3Points.size(); u++)
		{
			if (pau_remap[u])
			{
				pau_remap[u] = u_point_count;
				pav3Points[u_point_count++] = mh.mav3Points[u];
			}
		}

		// Remap the vertices' pointers to points.
		for (u = 0; u < pamvVertices.uLen; u++)
		{
			// Find original index, and remap it.
			int i_index = pamvVertices[u].pv3Point - mh.mav3Points;
			pamvVertices[u].pv3Point = &pav3Points[ pau_remap[i_index] ];
		}

		if (mh.mav3Wrap.uLen != 0)
		{
			// Allocate and copy the wrap.
			pav3Wrap = mh.mav3Wrap.paDup( new(fhGlobalMesh) CVector3<>[mh.mav3Wrap.size()] );
		}

		// Surfaces.
		pasfSurfaces = mh.masfSurfaces.paDup( new(fhGlobalMesh) SSurface[mh.masfSurfaces.size()] );

		// Remap the polygons' pointers to surfaces.
		for (u = 0; u < pampPolygons.uLen; u++)
		{
			// Find original index, and remap it.
			int i_index = pampPolygons[u].pSurface - mh.masfSurfaces;
			pampPolygons[u].pSurface = &pasfSurfaces[i_index];
		}

		// Assign indices to vertices.
		for (u = 0; u < pamvVertices.uLen; u++)
		{
			pamvVertices[u].u4ShapeVertex = u;
			pamvVertices[u].u4ShapePoint = pamvVertices[u].pv3Point - pav3Points;
		}
		
		MEMLOG_ADD_COUNTER(emlTotalPolygon,		pampPolygons.uLen);
		MEMLOG_ADD_COUNTER(emltotalVertPoint,	pav3Points.uLen);
		MEMLOG_ADD_COUNTER(emlTotalVertex,		pamvVertices.uLen);
		MEMLOG_ADD_COUNTER(emlWrapPoints,		pav3Wrap.uLen);
	}
	
	//******************************************************************************************
	void CMesh::Validate() const
	{
		uint u;

		// Test the surfaces of all polygons.
		for (u = 0; u < pampPolygons.uLen; u++)
			AlwaysAssert(pampPolygons[u].pSurface);
		for (u = 0; u < pasfSurfaces.uLen; u++)
			pasfSurfaces[u].ptexTexture->Validate();

		// Check the number of vertices in all the polygons.
		for (u = 0; u < pampPolygons.uLen; u++)
		{
			// This assert means that a single polygon has more than 16 sides.
			AlwaysAssert(pampPolygons[u].papmvVertices.uLen <= 16);
		}

		// Check the range of texture co-ordinates for all the polygons.
		int i_surface_err = -1;
		for (u = 0; u < pampPolygons.uLen; u++)
		{
			// Verify that the texture co-ordinates are in range.
			if (pampPolygons[u].pSurface->ptexTexture->seterfFeatures[erfTEXTURE])
			{
				rptr<CRaster> pras_texture = pampPolygons[u].pSurface->ptexTexture->prasGetTexture(0);

				if (pras_texture->bNotTileable)
				{
					for (int i = 0; i < pampPolygons[u].papmvVertices.uLen; i++)
					{
						float tX = pampPolygons[u].papmvVertices[i]->tcTex.tX * 
									pras_texture->fWidth + fTexEdgeTolerance;
						float tY = pampPolygons[u].papmvVertices[i]->tcTex.tY * 
									pras_texture->fHeight + fTexEdgeTolerance;

						if (tX > 32767.0f || tX < -32767.0f || tY > 32767.0f || tY < -32767.0f)
						{
							int i_surface = pampPolygons[u].pSurface - pasfSurfaces;

							// Only print error once per surface.
							if (i_surface != i_surface_err)
							{
								dprintf("!ERROR!: %s Has very large texture co-ordinates, Material #%d!!!\n", 
										CLoadWorld::pgonCurrentObject->strObjectName, i_surface+1);

								i_surface_err = i_surface;
							}
						}
					}
				}
				else
				{
					for (int i = 0; i < pampPolygons[u].papmvVertices.uLen; i++)
					{
						float tX = pampPolygons[u].papmvVertices[i]->tcTex.tX * pras_texture->fWidth;
						float tY = pampPolygons[u].papmvVertices[i]->tcTex.tY * pras_texture->fHeight;

						if (tX > 32767.0f || tX < -32767.0f || tY > 32767.0f || tY < -32767.0f)
						{
							int i_surface = pampPolygons[u].pSurface - pasfSurfaces;

							// Only print error once per surface.
							if (i_surface != i_surface_err)
							{
								if (CLoadWorld::pgonCurrentObject)
								{
									dprintf("!ERROR!: %s Has very large texture co-ordinates, Material #%d!!!\n", 
											CLoadWorld::pgonCurrentObject->strObjectName, i_surface+1);
								}

								i_surface_err = i_surface;
							}
						}
					}
				}
			}
		}
	}

	//******************************************************************************************
	void CMesh::MakeVerticesUnique()
	{
		Assert(pamvVertices);
		Assert(pampPolygons);

		// Allocate a new CPArray for vertices on the fast heap
		CPArray<SVertex> pamv(papmvVertices.uLen, new(fhGlobalMesh) SVertex[papmvVertices.uLen]);

		//
		// Iterate through the polygons, and create a unique entry for every vertex for every
		// polygon.
		//
		int i_unique = 0;	// Unique vertex index.

		for (CPolyIterator pi(*this, 0, 0); pi.bNext(); )
		{
			// Iterate through the vertices of the polygon.
			for (int i = 0; i < pi.iNumVertices(); i++)
			{
				// Copy each vertex into a new spot in the array.
				pamv[i_unique] = *pi.pmvVertex(i);

				// Re-assign vertex index.
				pamv[i_unique].u4ShapeVertex = i_unique;

				// Reassign the vertex pointer to its new spot.
				pi.pmpPolygon()->papmvVertices[i] = &pamv[i_unique];
				i_unique++;
			}
		}

		// Delete the existing vertex array and replace by the new one.
		if (bInMeshHeap((SVertex*)pamvVertices))
		{
			// If the existing vertex array is within a fast heap then this memory will be waste.
			MEMLOG_ADD_COUNTER(emlDeletedMeshMemory, pamvVertices.uMemSize());
		}
		else
		{
			delete[] pamvVertices.atArray;
		}

		pamvVertices = pamv;
	}

	//******************************************************************************************
	void CMesh::MakeSurfacesUnique()
	{
		// Create a new surface array.
		CPArray<SSurface> pasf(pampPolygons.uLen,new(fhGlobalMesh) SSurface[pampPolygons.uLen]);

		// Assign each polygon its unique surface pointer.
		for (int i = 0; i < pampPolygons.uLen; i++)
		{
			// Copy polygon's current surface info to the corresponding new surface.
			pasf[i] = *pampPolygons[i].pSurface;
			pampPolygons[i].pSurface = &pasf[i];
		}

		// Delete the existing surface array and replace by the new one.
		if (bInMeshHeap((SSurface*)pasfSurfaces))
		{
			// If the existing surface array is within a fast heap then this memory will be waste.
			for (int u = 0; u < pasfSurfaces.size(); u++)
			{
				// call the destructor but do not delete the memory
				pasfSurfaces[u].~SSurface();
			}

			MEMLOG_ADD_COUNTER(emlDeletedMeshMemory, pasfSurfaces.uMemSize());
		}
		else
		{
			delete[] pasfSurfaces.atArray;
		}

		// Replace the surface array.
		pasfSurfaces = pasf;
	}

	//******************************************************************************************
	CPArray< CVector3<> > CMesh::pav3GetWrap() const
	{
		// If a wrap is available, use it, otherwise use the shared points for the polygon.
		if (pav3Wrap.uLen > 0 && pav3Wrap.atArray)
			return pav3Wrap;

		return pav3Points;
	}
	
	//******************************************************************************************
	void CMesh::CreateWrap()
	{
		if (pav3Points.uLen > uMAX_POINTS_BEFORE_WRAP)
		{
			CreateWrapBox();
			return;
		}

		//
		// Create a temporary array with the number of vertices.
		//
		CLArray(CVector3<>, av3_scratch, pav3Points.uLen);

		// Copy values to the scratch list.
		for (uint u = 0; u < pav3Points.uLen; ++u)
			av3_scratch[u] = pav3Points[u];

		// Create approximate convex hull for points.
		av3_scratch.uLen = uCreateWrap(av3_scratch, pav3Points.uLen);
		RemoveDuplicatePoints(av3_scratch, TReal(0.0125));

		if (av3_scratch.uLen > uMAX_POINTS_WRAP)
		{
			CreateWrapBox();
			return;
		}

		// Allocate the permanent array and copy the values over.
		pav3Wrap = av3_scratch.paDup();
	}
	
	//******************************************************************************************
	void CMesh::MakeNoTexture()
	{
		for (uint u = 0; u < pasfSurfaces.uLen; ++u)
			//pasfSurfaces[u].MakeNoTexture();
			pasfSurfaces[u].ptexTexture->seterfFeatures[erfTEXTURE][erfTRANSPARENT][erfBUMP] = 0;
	}
	
	//******************************************************************************************
	void CMesh::CreateWrapBox()
	{
		CVector3<> v3_min;	// Min value defining the box.
		CVector3<> v3_max;	// Max value defining the box.

		// Allocate the permanent array and copy the values over.
		pav3Wrap.uLen    = 8;
		pav3Wrap.atArray = new(fhGlobalMesh) CVector3<>[8];
		MEMLOG_ADD_COUNTER(emlMeshHeap, pav3Wrap.uMemSize());

		// Find the min and max values.
		v3_min = v3_max = pav3Points[0];
		for (uint u = 1; u < pav3Points.uLen; ++u)
		{
			v3_min.SetMin(pav3Points[u]);
			v3_max.SetMax(pav3Points[u]);
		}

		// Points 0 and 1.
		pav3Wrap[0] = v3_min;
		pav3Wrap[1] = v3_max;
		
		// Point 2.
		pav3Wrap[2].tX = v3_min.tX;
		pav3Wrap[2].tY = v3_min.tY;
		pav3Wrap[2].tZ = v3_max.tZ;
		
		// Point 3.
		pav3Wrap[3].tX = v3_min.tX;
		pav3Wrap[3].tY = v3_max.tY;
		pav3Wrap[3].tZ = v3_min.tZ;
		
		// Point 4.
		pav3Wrap[4].tX = v3_min.tX;
		pav3Wrap[4].tY = v3_max.tY;
		pav3Wrap[4].tZ = v3_max.tZ;
		
		// Point 5.
		pav3Wrap[5].tX = v3_max.tX;
		pav3Wrap[5].tY = v3_min.tY;
		pav3Wrap[5].tZ = v3_min.tZ;
		
		// Point 6.
		pav3Wrap[6].tX = v3_max.tX;
		pav3Wrap[6].tY = v3_min.tY;
		pav3Wrap[6].tZ = v3_max.tZ;
		
		// Point 2.
		pav3Wrap[7].tX = v3_max.tX;
		pav3Wrap[7].tY = v3_max.tY;
		pav3Wrap[7].tZ = v3_min.tZ;
	}

	//******************************************************************************************
	void CMesh::CreateMipMaps
	(
		uint32 u4_smallest
	)
	{
		//
		// Iterate through the surfaces and call the generate mipmap functions for each of
		// the textures.
		//
		for (int i = 0; i < pasfSurfaces.uLen; i++)
		{
			pasfSurfaces[i].ptexTexture->GenerateMipLevels(u4_smallest);
		}
	}

	static int iSimpleMax = 2;

	//******************************************************************************************
	bool CMesh::bSimpleShape()
	{
		return pampPolygons.uLen <= iSimpleMax;
	}

	//******************************************************************************************
	//
	void CMesh::IdentifyOccludingPolygons
	(
	)
	//
	// Iterates through polygons to locate and mark occluding polygons, and then stores an
	// array of pointers to them.
	//	
	//**************************************
	{
		// Allocate memory from the stack to record found occluding polygons.
		CMLArray(SPolygon*, papmp, pampPolygons.uLen);

		// Find and mark occluding polygons.
		for (int i = 0; i < pampPolygons.uLen; i++)
		{
			Assert(pampPolygons[i].pSurface);
			
			// Set default.
			pampPolygons[i].bOcclude = false;

			//
			// If the surface points to a signal texture, et the occluding flag and record
			// the pointer.
			//
			if (pampPolygons[i].pSurface->ptexTexture->seterfFeatures[erfOCCLUDE])
			{
				// Set the polygon's flag.
				pampPolygons[i].bOcclude = true;

				//
				// Set the polygon's world area and local centre point.
				//
				// Note: This value assumes a scale of 1.0 in the presence of the CInstance
				// owning the mesh.
				//
				pampPolygons[i].rWorldArea = pampPolygons[i].rGetArea();

				// Record the polygon's pointer.
				papmp << &pampPolygons[i];
			}
		}

		//
		// Copy polygon pointers to a special list.
		//

		papmpOcclude = papmp.paDup( new(fhGlobalMesh) SPolygon*[papmp.size()] );
		MEMLOG_ADD_COUNTER(emlMeshHeap,papmpOcclude.uMemSize());
	}

	//*****************************************************************************************
	void CMesh::OnPrefetch(bool b_in_thread) const
	{
#ifdef USING_FETCH
		// Must snag all textures.
		if (pddDataDaemon->cLastPrefetch != cLastPrefetch)
		{

			int i = pasfSurfaces.uLen-1;
			for ( ; i >= 0; --i)
			{
				for (int i2 = pasfSurfaces[i].ptexTexture->iGetNumMipLevels() - 1; i2 >=0 ; --i2)
				{
	//				pddDataDaemon->Prefetch(pasfSurfaces[i].ptexTexture->prasGetTexture(i2).ptGet(), b_in_thread);
					pasfSurfaces[i].ptexTexture->prasGetTexture(i2)->OnPrefetch(b_in_thread);
				}
			}

			// Should also snag any geometry, but we'll ignore it for now.

			// Update the last prefetch field so that we don't fetch the mesh again.
			((CMesh*)this)->cLastPrefetch = pddDataDaemon->cLastPrefetch;

		}
#endif
	}

	//*****************************************************************************************
	void CMesh::OnFetch()
	{
		// Must snag all textures.
		

		// Should also snag any geomtry, but we'll ignore it for now.

	}

	//*****************************************************************************************
	void CMesh::OnUnfetch()
	{
		// For now, do nothing and let VMM handle it.
	}

	//******************************************************************************************
	void CMesh::SetPolygonAsCache()
	{	
		// Find and mark all polygons for image caching.
		for (uint u = 0; u < pampPolygons.uLen; ++u)
		{
			pampPolygons[u].bCache = true;
		}
	}

	//******************************************************************************************
	CShape::CPolyIterator* CMesh::pPolyIterator(const CInstance* pins, const CRenderContext* prenc) const
	{
		return new CPolyIterator(*this, pins, prenc);
	}

	//******************************************************************************************
	//
	// CMesh::CPolyIterator implementation.
	//

		//**************************************************************************************
		CMesh::CPolyIterator::CPolyIterator(const CMesh& msh, const CInstance*, const CRenderContext* prenc)
			: mshMesh(msh), pcamCamera(0)
		{
			if (prenc)
				pcamCamera = &prenc->Camera;

			pmpCurrent = msh.pampPolygons - 1;
		}

		//******************************************************************************************
		CMesh::CPolyIterator::~CPolyIterator()
		{
		}

		//******************************************************************************************
		void CMesh::CPolyIterator::TransformPoints
		(
			const CTransform3<>&	tf3_shape_camera,
			const CCamera&			cam,
			CPArray<SClipPoint>		paclpt_points,
			bool					b_outcodes
		)
		{
			if (b_outcodes || VER_DEBUG)
			{
				const CBoundVolCamera* pbvcam_clip = cam.pbvcamClipVolume();

			#if (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
				int i_cnt = mshMesh.pav3Points.uLen;
				const CTransform3<>* ptf3_mat = &tf3_shape_camera;
				const CVector3<>* pav3_src_point_0 = mshMesh.pav3Points;
				const SClipPoint* paclpt_dest_point_0 = paclpt_points;

				typedef CVector3<> tdCVector3;
				typedef CTransform3<> tdCTransform3;

				const uint u_down_left_masks[2] = {1<<eocDOWN, 1<<eocLEFT};
				const uint u_up_right_masks[2]  = {1<<eocUP,   1<<eocRIGHT};
				const uint u_near_far_masks[2]  = {1<<eocNEAR, 1<<eocFAR};

			  if (cam.campropGetProperties().bPerspective)
			  {
				// Assumptions made by this code:
				// This code only works if pbvcam points to a CCameraDefPerspectiveNorm 
				//   versus to a CCameraDefPerspective!!!  (since seteocOutCodes differs 
				//   for each type of CBoundVolCamera that pbvcam can point to)
				// (fCAMERA_PLANE_TOLERANCE	== 1e-4) to match #define in GeomTypesCamera.cpp
				// ((char *)&pbvcam_clip->CCameraDefPerspectiveNorm.clpNear.rPos - (char *)pbvcam_clip == 24);
				// ((char *)&pbvcam_clip->CCameraDefPerspectiveNorm.clpFar.rPos - (char *)pbvcam_clip == 32);
				//   (these three really need to be replaced by proper symbolic references)

				#define NearClipOffset	24
				#define FarClipOffset	32

				double vf_near_far_const;

				#define fCAMERA_PLANE_TOLERANCE	1e-4
				const float f_tolerance   = fCAMERA_PLANE_TOLERANCE;
				const float f_toleranceP1 = fCAMERA_PLANE_TOLERANCE + 1.0f;

				__asm
				{
					femms										//ensure fast switch

					mov		eax,[pbvcam_clip]
					jmp		StartAsm1

					align	16
				StartAsm1:
					movd	mm0,[f_tolerance]					;m0= f_tolerance

					movd	mm2,[eax+NearClipOffset]			;m2= CCameraDefPerspectiveNorm.clpNear.rPos

					movd	mm3,[eax+FarClipOffset]				;m3= CCameraDefPerspectiveNorm.clpFar.rPos
					movq	mm1,mm0								;m1= f_tolerance

					pfmul	(m0,m2)								;m0= clpNear.rPos * f_tolerance
																;(intentional long decode due to degraded predecode)
					mov		ecx,[i_cnt]
					pfmul	(m1,m3)								;m1= clpFar.rPos * f_tolerance

					mov		edx,[ptf3_mat]
					pfsub	(m0,m2)								;m0= clpNear.rPos * f_tolerance - clpNear.rPos

					mov		eax,[pav3_src_point_0]
					pfadd	(m1,m3)								;m1= clpFar.rPos * f_tolerance + clpFar.rPos

					mov		ebx,[paclpt_dest_point_0]
					add		edx,8								;force zero disp in first usage of edx below
																;  (to avoid degraded predecode)
					test	ecx,ecx								;check if any points to be transformed
					punpckldq mm0,mm1							;m0= clipFar_const | clipNear_const

					movq	[vf_near_far_const],mm0
					jnz		XformOutcodeLoop1

					jmp		SkipXformOutcodeLoop1

			// EAX = ptr to first element in array of points to be transformed
			// EBX = ptr to first element in array of transformed points
			// ECX = number of points to be transformed
			// EDX = ptr to transform matrix

					align	16
					nop											;establish 3 byte starting code offset
					nop
					nop
				XformOutcodeLoop1:
					movd	mm0,[eax]tdCVector3.tX				;m0= X
					nop											;1-byte NOOP to avoid degraded predecode 
																;  and maintain decode pairing
					movd	mm1,[eax]tdCVector3.tY				;m1= Y
					nop											;1-byte NOOP to avoid degraded predecode
																;  and maintain decode pairing
					movd	mm2,[eax]tdCVector3.tZ				;m2= Z
					punpckldq mm0,mm0							;m0= X | X

					movd	mm3,[edx-8]tdCTransform3.mx3Mat.v3X.tZ ;m3= m02
					punpckldq mm1,mm1							;m1= Y | Y

					movd	mm4,[edx-8]tdCTransform3.mx3Mat.v3Y.tZ ;m4= m12
					punpckldq mm2,mm2							;m2= Z | Z

					movd	mm5,[edx-8]tdCTransform3.mx3Mat.v3Z.tZ ;m5= m22
					pfmul	(m3,m0)								;m3= m02*X

					movd	mm6,[edx-8]tdCTransform3.v3Pos.tZ	;m6= m32
					pfmul	(m4,m1)								;m4= m12*Y

					movq	mm7,[edx-8]tdCTransform3.mx3Mat.v3X.tX ;m7= m01 | m00
					pfmul	(m5,m2)								;m5= m22*Z

					pfadd	(m4,m3)								;m4= m02*X + m12*Y
					movq	mm3,[edx-8]tdCTransform3.mx3Mat.v3Y.tX ;m3= m11 | m10

					pfadd	(m6,m5)								;m6= m22*Z + m32
					pfmul	(m7,m0)								;m7= m01*X | m00*X

					movq	mm5,[edx-8]tdCTransform3.mx3Mat.v3Z.tX ;m5= m21 | m20
					pfmul	(m3,m1)								;m3= m11*Y | m10*Y

					pfadd	(m6,m4)								;m6=  0  | resultZ
					movq	mm4,[edx-8]tdCTransform3.v3Pos.tX	;m4= m31 | m30

					movd	[ebx]SClipPoint.v3Point.tZ,mm6
					pfmul	(m5,m2)								;m5= m21*Z | m20*Z

					movd	mm0,[f_toleranceP1]			;m0= f_tolerance + 1
					pfadd	(m7,m3)								;m7= m01*X + m11*Y | m00*X + m10*Y

					movq	mm1,[vf_near_far_const]		;m1= clipFar_const | clipNear_const
					pfadd	(m5,m4)								;m5= m21*Z + m31   | m20*Z + m30

					movq	mm4,[u_down_left_masks]		;m4= eocLEFT | eocDOWN
					lea		eax,[eax + SIZE tdCVector3]			;advance to next point to be transformed

					pfadd	(m7,m5)								;m7=    resultY    | resultX
					movq	[ebx]SClipPoint.v3Point.tX,mm7

					nop									;1-byte NOOP to avoid degraded predecode 
					movq	mm5,[u_near_far_masks]		;m5=  eocFAR | eocNEAR

					punpckldq mm6,mm7					;m6= X | Z
					psrlq	mm7,32						;m7= 0 | Y

					nop									;1-byte NOOP's to avoid degraded predecode 
					nop									;  and maintain decode pairing 

					movq	mm2,mm7						;m2= 0 | Y
					psllq	mm7,32						;m7= Y | 0

					pfsubr	(m7,m2)						;m7= -Y | Y
					pfmul	(m2,m0)						;m2= Y*f_toleranceP1

					movq	mm0,[u_up_right_masks]		;m0= eocRIGHT | eocUP
					cmp		ebx,0						;3-byte NOOP to avoid degraded predecode

					pfadd	(m7,m1)						;m7= clipFar_const-Y | clipNear_const+Y
					punpckldq mm2,mm2					;m2= Y*f_toleranceP1 | Y*f_toleranceP1

					movq	mm3,mm2						;m3= Y*f_toleranceP1 | Y*f_toleranceP1
					pfadd	(m2,m6)						;m2= Y*f_toleranceP1+X | Y*f_toleranceP1+Z

					psrad	mm7,31						;m7= sign(fFar) | sign(fNear)
					pfsub	(m3,m6)						;m3= Y*f_toleranceP1-X | Y*f_toleranceP1-Z

					psrad	mm2,31						;m2= sign(fLeft) | sign(fDown)
					pand	mm7,mm5						;m7= eocFAR? | eocNEAR?

					psrad	mm3,31						;m3= sign(fRight) | sign(fUp)
					pand	mm2,mm4						;m2= eocLEFT? | eocDOWN?

					por		mm7,mm2						;m7= accumulate eoc's
					pand	mm3,mm0						;m3= eocRIGHT? | eocUP?

					por		mm7,mm3						;m7= accumulate eoc's
					dec		ecx							;decrement loop counter

					movq	mm6,mm7						;m6= copy of low eoc accumulation
					punpckhdq mm7,mm7					;m7= copy of hight eoc accumulation

					por		mm7,mm6						;m7= accumulation of all six eoc's
					movd	[ebx]SClipPoint.seteocOut,mm7

					lea		ebx,[ebx + SIZE SClipPoint]	;advance to next transformed point
					jnz		XformOutcodeLoop1			;if not done, go do next point

				SkipXformOutcodeLoop1:
					femms								//clear MMX state and ensure fast switch
				}
			  }
			  else
			  {
				// Assumptions made by this code:
				// This code only works if pbvcam points to a CCameraDefParallelOpen 
				//   versus to a CCameraDefParallel!!!  (since seteocOutCodes differs 
				//   for each type of CBoundVolCamera that pbvcam can point to)
				// ((char *)&pbvcam_clip->CCameraDefParallelOpen.clpLeft.rPos - (char *)pbvcam_clip == 8);
				// ((char *)&pbvcam_clip->CCameraDefParallelOpen.clpRight.rPos - (char *)pbvcam_clip == 16);
				// ((char *)&pbvcam_clip->CCameraDefParallelOpen.clpDown.rPos - (char *)pbvcam_clip == 24);
				// ((char *)&pbvcam_clip->CCameraDefParallelOpen.clpUp.rPos - (char *)pbvcam_clip == 32);
				//   (these four really need to be replaced by proper symbolic references)

				#define LeftClipOffset	8
				#define RightClipOffset	16
				#define DownClipOffset	24
				#define UpClipOffset	32

				double vf_down_left_const;
				double vf_up_right_const;

				const float f_tolerance = fPLANE_TOLERANCE;

				__asm
				{
					femms										//ensure fast switch

					mov		eax,[pbvcam_clip]
					jmp		StartAsm2

																;(intentional long decode due to degraded predecode)
					align	16
					nop											;establish 4 byte starting code offset
					nop
					nop
					nop
				StartAsm2:
					movd	mm0,[f_tolerance]					;m0= f_tolerance

					movd	mm4,[eax+LeftClipOffset]			;m4= CCameraDefPerspectiveNorm.clpLeft.rPos

					movd	mm5,[eax+RightClipOffset]			;m5= CCameraDefPerspectiveNorm.clpRight.rPos

					movd	mm6,[eax+DownClipOffset]			;m6= CCameraDefPerspectiveNorm.clpDown.rPos
					pfsubr	(m4,m0)								;m4= f_tolerance - clpLeft.rPos

					movd	mm7,[eax+UpClipOffset]				;m7= CCameraDefPerspectiveNorm.clpUp.rPos
					pfadd	(m5,m0)								;m5= f_tolerance + clpRight.rPos

					mov		ecx,[i_cnt]
					pfsubr	(m6,m0)								;m6= f_tolerance - clpDown.rPos

					mov		edx,[ptf3_mat]
					pfadd	(m7,m0)								;m7= f_tolerance + clpUp.rPos

					mov		eax,[pav3_src_point_0]
					punpckldq mm6,mm4							;m6= clipLeft_const  | clipDown_const

					mov		ebx,[paclpt_dest_point_0]
					movq	[vf_down_left_const],mm6

					punpckldq mm7,mm5							;m7= clipRight_const | clipUp_const
					test	ecx,ecx								;check if any points to be transformed

					movq	[vf_up_right_const],mm7
					add		edx,8								;force zero disp in first usage of edx below
																;  (to avoid degraded predecode)
					jnz		XformOutcodeLoop2

					jmp		SkipXformOutcodeLoop2

			// EAX = ptr to first element in array of points to be transformed
			// EBX = ptr to first element in array of transformed points
			// ECX = number of points to be transformed
			// EDX = ptr to transform matrix

					align	16
					nop											;establish 3 byte starting code offset
					nop
					nop
				XformOutcodeLoop2:
					movd	mm0,[eax]tdCVector3.tX				;m0= X
					nop											;1-byte NOOP to avoid degraded predecode 
																;  and maintain decode pairing
					movd	mm1,[eax]tdCVector3.tY				;m1= Y
					nop											;1-byte NOOP to avoid degraded predecode
																;  and maintain decode pairing
					movd	mm2,[eax]tdCVector3.tZ				;m2= Z
					punpckldq mm0,mm0							;m0= X | X

					movd	mm3,[edx-8]tdCTransform3.mx3Mat.v3X.tZ ;m3= m02
					punpckldq mm1,mm1							;m1= Y | Y

					movd	mm4,[edx-8]tdCTransform3.mx3Mat.v3Y.tZ ;m4= m12
					punpckldq mm2,mm2							;m2= Z | Z

					movd	mm5,[edx-8]tdCTransform3.mx3Mat.v3Z.tZ ;m5= m22
					pfmul	(m3,m0)								;m3= m02*X

					movd	mm6,[edx-8]tdCTransform3.v3Pos.tZ	;m6= m32
					pfmul	(m4,m1)								;m4= m12*Y

					movq	mm7,[edx-8]tdCTransform3.mx3Mat.v3X.tX ;m7= m01 | m00
					pfmul	(m5,m2)								;m5= m22*Z

					pfadd	(m4,m3)								;m4= m02*X + m12*Y
					movq	mm3,[edx-8]tdCTransform3.mx3Mat.v3Y.tX ;m3= m11 | m10

					pfadd	(m6,m5)								;m6= m22*Z + m32
					pfmul	(m7,m0)								;m7= m01*X | m00*X

					movq	mm5,[edx-8]tdCTransform3.mx3Mat.v3Z.tX ;m5= m21 | m20
					pfmul	(m3,m1)								;m3= m11*Y | m10*Y

					pfadd	(m6,m4)								;m6=  0  | resultZ
					movq	mm4,[edx-8]tdCTransform3.v3Pos.tX	;m4= m31 | m30

					movd	[ebx]SClipPoint.v3Point.tZ,mm6
					pfmul	(m5,m2)								;m5= m21*Z | m20*Z

					movq	mm0,[vf_down_left_const]	;m0= clipLeft_const  | clipDown_const
					pfadd	(m7,m3)								;m7= m01*X + m11*Y | m00*X + m10*Y

					lea		eax,[eax + SIZE tdCVector3]			;advance to next point to be transformed
					pfadd	(m5,m4)								;m5= m21*Z + m31   | m20*Z + m30

					nop									;1-byte NOOP to avoid degraded predecode 
					movq	mm1,[vf_up_right_const]		;m1= clipRight_const | clipUp_const

					pfadd	(m7,m5)								;m7=    resultY    | resultX
					movq	[ebx]SClipPoint.v3Point.tX,mm7

					movq	mm4,[u_down_left_masks]		;m4= eocLEFT | eocDOWN
					nop									;1-byte NOOP to avoid degraded predecode 

					punpckldq mm6,mm7					;m6= X | Z

					pfadd	(m0,m6)						;m0= clipLeft_const+X  | clipDown_const+Z

					movq	mm5,[u_up_right_masks]		;m5= eocRIGHT | eocUP
					pfsub	(m1,m6)						;m1= clipRight_const-X | clipUp_const-Z

					psrad	mm0,31						;m0= sign(fLeft) | sign(fDown)

					psrad	mm1,31						;m1= sign(fRight) | sign(fUp)
					pand	mm0,mm4						;m0= eocLEFT? | eocDOWN?

					pand	mm1,mm5						;m1= eocRIGHT? | eocUP?
					test	ecx,ecx						;2-byte NOOP to avoid degraded predecode 

					por		mm1,mm0						;m1= accumulate eoc's
					dec		ecx							;decrement loop counter

					movq	mm0,mm1						;m0= copy of low eoc accumulation
					punpckhdq mm1,mm1					;m1= copy of hight eoc accumulation

					por		mm1,mm0						;m1= accumulation of all six eoc's
					movd	[ebx]SClipPoint.seteocOut,mm1

					lea		ebx,[ebx + SIZE SClipPoint]	;advance to next transformed point
					jnz		XformOutcodeLoop2			;if not done, go do next point

				SkipXformOutcodeLoop2:
					femms								//clear MMX state and ensure fast switch
				}
			  }

			#else // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

				for (int i = 0; i < mshMesh.pav3Points.uLen; i++)
				{
					// Transform point.
					paclpt_points[i].v3Point = mshMesh.pav3Points[i] * tf3_shape_camera;

					// Also generate outcode.
					paclpt_points[i].seteocOut = 
						pbvcam_clip->seteocOutCodes(paclpt_points[i].v3Point);

				#if (VER_DEBUG)
					if (paclpt_points[i].seteocOut - eocFAR)
					{
						// This point is outside the view volume, so let's make
						// sure we weren't told to disable clipping.
						// We ignore the far clipping plane for now, since if it's violated it's not
						// critical, and the render cache currently has problems staying within in.
						Assert(b_outcodes);
					}
				#endif
				}

			#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
			}
			else
			{
				// Similar procedure, but transform only, skipping clip test.

			#if (0 && TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

				int i_cnt = mshMesh.pav3Points.uLen;
				const CTransform3<>* ptf3_mat = &tf3_shape_camera;
				const CVector3<>* pav3_src_point_0 = mshMesh.pav3Points;
				const SClipPoint* paclpt_dest_point_0 = paclpt_points;

				typedef CVector3<> tdCVector3;
				typedef CTransform3<> tdCTransform3;

				__asm
				{
					femms										//ensure fast switch
					mov		ecx,[i_cnt]
					mov		edx,[ptf3_mat]
					mov		eax,[pav3_src_point_0]
					mov		ebx,[paclpt_dest_point_0]

					add		edx,8								;force zero disp in first usage of ebx below
																;  (to avoid degraded predecode)
					test	ecx,ecx								;check if any points to be transformed
					jnz		XformLoop

					jmp		SkipXformLoop

					align	16
					nop											;establish 3 byte starting code offset
					nop
					nop
				XformLoop:
					movd	mm0,[eax]tdCVector3.tX				;m0= X
					nop											;1-byte NOOP to avoid degraded predecode 
																;  and maintain decode pairing
					movd	mm1,[eax]tdCVector3.tY				;m1= Y
					nop											;1-byte NOOP to avoid degraded predecode
																;  and maintain decode pairing
					movd	mm2,[eax]tdCVector3.tZ				;m2= Z
					punpckldq mm0,mm0							;m0= X | X

					movd	mm3,[edx-8]tdCTransform3.mx3Mat.v3X.tZ ;m3= m02
					punpckldq mm1,mm1							;m1= Y | Y

					movd	mm4,[edx-8]tdCTransform3.mx3Mat.v3Y.tZ ;m4= m12
					punpckldq mm2,mm2							;m2= Z | Z

					movd	mm5,[edx-8]tdCTransform3.mx3Mat.v3Z.tZ ;m5= m22
					pfmul	(m3,m0)								;m3= m02*X

					movd	mm6,[edx-8]tdCTransform3.v3Pos.tZ	;m6= m32
					pfmul	(m4,m1)								;m4= m12*Y

					movq	mm7,[edx-8]tdCTransform3.mx3Mat.v3X.tX ;m7= m01 | m00
					pfmul	(m5,m2)								;m5= m22*Z

					pfadd	(m4,m3)								;m4= m02*X + m12*Y
					movq	mm3,[edx-8]tdCTransform3.mx3Mat.v3Y.tX ;m3= m11 | m10

					pfadd	(m6,m5)								;m6= m22*Z + m32
					pfmul	(m7,m0)								;m7= m01*X | m00*X

					movq	mm5,[edx-8]tdCTransform3.mx3Mat.v3Z.tX ;m5= m21 | m20
					pfmul	(m3,m1)								;m3= m11*Y | m10*Y

					pfadd	(m6,m4)								;m6= resultZ
					movq	mm4,[edx-8]tdCTransform3.v3Pos.tX	;m4= m31 | m30

					movd	[ebx]SClipPoint.v3Point.tZ,mm6
					pfmul	(m5,m2)								;m5= m21*Z | m20*Z

					lea		eax,[eax + SIZE tdCVector3]			;advance to next point to be transformed
					pfadd	(m7,m3)								;m7= m01*X + m11*Y | m00*X + m10*Y

					lea		ebx,[ebx + SIZE SClipPoint]			;advance to next transformed point
					pfadd	(m5,m4)								;m5= m21*Z + m31   | m20*Z + m30

					sub		ecx,1								;decrement loop counter (and use SUB 
																;  vs. DEC to avoid degraded predecode)
					pfadd	(m7,m5)								;m7=    resultY    | resultX
					movq	[ebx - SIZE SClipPoint]SClipPoint.v3Point.tX,mm7

					jnz		XformLoop							;if not done, go do next point

				SkipXformLoop:
					femms										//clear MMX state and ensure fast switch
				}

			#else // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

				for (int i = 0; i < mshMesh.pav3Points.uLen; i++)
				{
					// Transform point.
					paclpt_points[i].v3Point = mshMesh.pav3Points[i] * tf3_shape_camera;
				}

			#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
			}
		}


//**********************************************************************************************
//
// Function implementations.
//

//**********************************************************************************************
uint uCreateWrapSubdivide(CVector3<>* av3, uint u_num_pts)
//
// Find the convex hull of a set of vertices by testing all planes. Subdivide if
// the numver of points is greater that or equal to 32 to try to speed things up.
//
//**********************************
{
	const uint u_subdivide = 32;

	// Subdivide if there are too many points.
	if (u_num_pts >= u_subdivide)
	{
		uint u_num_a = u_num_pts >> 1;
		uint u_num_b = u_num_pts - u_num_a;
		CVector3<>* av3_b = av3 + u_num_a;

		// Call subdivision recursively.
		u_num_a = uCreateWrapSubdivide(av3, u_num_a);
		u_num_b = uCreateWrapSubdivide(av3_b, u_num_b);

		// Repack CVector array.
		for (uint u = 0; u < u_num_b; ++u, ++u_num_a)
			av3[u_num_a] = av3_b[u];
		u_num_pts = u_num_a;
	}

	uint* au_count = (uint*)_alloca(sizeof(uint) * u_num_pts);
	memset(au_count, 0, sizeof(uint) * u_num_pts);

	//
	// For every combination of points, test if a plane can be made for which all points
	// lie to one side.
	//
	for (uint u_0 = 0; u_0 < u_num_pts - 2; ++u_0)
	{
		for (uint u_1 = u_0 + 1; u_1 < u_num_pts - 1; ++u_1)
		{
			for (uint u_2 = u_1 + 1; u_2 < u_num_pts; ++u_2)
			{
				// Do nothing if the points will not form a plane.
				if (Fuzzy(av3[u_0]) == av3[u_1])
					continue;
				if (Fuzzy(av3[u_0]) == av3[u_2])
					continue;
				if (Fuzzy(av3[u_1]) == av3[u_2])
					continue;
				if (iColinear(av3[u_0], av3[u_1], av3[u_2]) != 0)
					continue;

				// Construct a plane.
				CPlane pl(av3[u_0], av3[u_1], av3[u_2]);

				// Test that all points lay to one side of the plane.
				ESideOf esf = pl.esfSideOf(CPArray< CVector3<> >(u_num_pts, av3));

				// Flag points.
				if (esf != esfINTERSECT)
				{
					// Keep these points.
					au_count[u_0] = 1;
					au_count[u_1] = 1;
					au_count[u_2] = 1;
				}
			}
		}
	}

	// Repack vectors.
	uint u_to, u_from;
	for (u_to = 0, u_from = 0; u_from < u_num_pts; ++u_from)
	{
		if (au_count[u_from])
			av3[u_to++] = av3[u_from];
	}

	return u_to;
}


//**********************************************************************************************
inline uint uCreateWrapFast(CVector3<>* av3, uint u_num_pts)
//
// Helper function that either calls uCreateWrapSubdivide or iQuickHull.
//
//**********************************
{
#if (bUSE_QUICKHULL)
	extern int iQuickHull(CVector3<>* av3_pts, int i_num_pts);
	uint u_hull_pts;

	if (u_num_pts <= 16 || (u_hull_pts = iQuickHull(av3, u_num_pts)) == 0)
	{
		// Use old routine if quick hull fails or # of points is <= 16.
		u_hull_pts = uCreateWrapSubdivide(av3, u_num_pts);
	}

	return u_hull_pts;
#else
	return uCreateWrapSubdivide(av3, u_num_pts);
#endif
}


//**********************************************************************************************
uint uCreateWrap(CVector3<>* av3, uint u_num_pts)
//
// Attempt to create a best convex hull by randomly perturbing the initial hull points outward.
//
//**********************************
{
//	dprintf("Original Points = %d\n", u_num_pts);

	// Create the convex hull.
	u_num_pts = uCreateWrapFast(av3, u_num_pts);

//	dprintf("Hull Points = %d\n", u_num_pts);

	// Find appxoimate center (average).
	CVector3<> v3_cent = av3[0];
	for (uint u = 1; u < u_num_pts; ++u)
	{
		v3_cent += av3[u];
	}

	v3_cent /= (float)u_num_pts;

	// Best points.
	uint u_best_len = u_num_pts;
	CLArray(CVector3<>, av3_best, u_num_pts);

	// Scratch array.
	CLArray(CVector3<>, av3_scratch, u_num_pts);

	int tries = iNUM_PERTURBANCE_TRIES;
	while (tries--)
	{
		// Randomly perturb points outward.
		for (uint u = 0; u < u_num_pts; ++u)
		{
			float f_perturb = 1.0f + (rand() * (1.0f/(float)RAND_MAX) * 0.10f);

			av3_scratch[u].tX = v3_cent.tX + (av3[u].tX - v3_cent.tX) * f_perturb;
			av3_scratch[u].tY = v3_cent.tY + (av3[u].tY - v3_cent.tY) * f_perturb;
			av3_scratch[u].tZ = v3_cent.tZ + (av3[u].tZ - v3_cent.tZ) * f_perturb;
		}

		uint u_scratch_len = uCreateWrapFast(av3_scratch, u_num_pts);

		if (u_scratch_len < u_best_len)
		{
			u_best_len = u_scratch_len;

			for (uint u = 0; u < u_scratch_len; ++u)
				av3_best[u] = av3_scratch[u];
		}
	}

	if (u_best_len < u_num_pts)
	{
		// Copy best points to output.
		u_num_pts = u_best_len;

		for (uint u = 0; u < u_best_len; ++u)
			av3[u] = av3_best[u];

//		dprintf("Perturbed Hull Points = %d\n", u_num_pts);
	}

	return u_num_pts;
}


//**********************************************************************************************
//
// class CMeshPolygon implementation.
//

	//******************************************************************************************
	CMeshPolygon::CMeshPolygon(CPArray< CVector3<> > pav3_points, CPArray<CTexCoord> patc_tex, SSurface sf)
	{
		Assert(pav3_points.uLen >= 3);

		// Create arrays of vertices and vertex pointers.
		pav3Points = pav3_points;
		pamvVertices = CPArray<SVertex>(pav3_points.uLen);
		papmvVertices = CPArray<SVertex*>(pav3_points.uLen);

		int i;
		for (i = 0; i < pav3_points.uLen; i++)
		{
			AssertFinite(patc_tex[i]);
			pamvVertices[i] = SVertex(&pav3Points[i], patc_tex[i]);

			// Assign indices to vertices.
			pamvVertices[i].u4ShapeVertex = i;
			pamvVertices[i].u4ShapePoint = i;

			papmvVertices[i] = &pamvVertices[i];
		}

		// Create a single surface.
		pasfSurfaces = PArray(1, &sf).paDup();

		// Create single polygon, and manually set relevant data.
		pampPolygons = CPArray<SPolygon>(1);
		pampPolygons[0] = SPolygon(papmvVertices);
		pampPolygons[0].bCurved = false;
		pampPolygons[0].pSurface = &pasfSurfaces[0];

		// Set the face normal from 3 points distributed around the polygon.
		pampPolygons[0].plPlane = CPlane
		(
			pav3Points[0], 
			pav3Points[1 * pav3Points.uLen / 3], 
			pav3Points[2 * pav3Points.uLen / 3]
		);

		// Copy to vertex normals.
		for (i = 0; i < pav3_points.uLen; i++)
		{
			pamvVertices[i].d3Normal = pampPolygons[0].plPlane.d3Normal;
		}

		bvbVolume = CBoundVolBox(pav3Points);
	}

	//******************************************************************************************
	CMeshPolygon::CMeshPolygon(TReal r_x, TReal r_y, bool b_y_downward, SSurface sf)
	{
		// Create a rectangular point array, then call polygon constructor.
		CPArray< CVector3<> > pav3_rect(4);

		pav3_rect[0] = CVector3<>(-r_x/2, -r_y/2, 0);
		pav3_rect[1] = CVector3<>(+r_x/2, -r_y/2, 0);
		pav3_rect[2] = CVector3<>(+r_x/2, +r_y/2, 0);
		pav3_rect[3] = CVector3<>(-r_x/2, +r_y/2, 0);

		// Generate texture coordinates.
		CPArray<CTexCoord> patc_tex(4);
		patc_tex[0] = CTexCoord(0, (TReal)b_y_downward);
		patc_tex[1] = CTexCoord(1, (TReal)b_y_downward);
		patc_tex[2] = CTexCoord(1, (TReal)!b_y_downward);
		patc_tex[3] = CTexCoord(0, (TReal)!b_y_downward);

		// Construct the mesh polygon.
		new(this) CMeshPolygon(pav3_rect, patc_tex, sf);
	}

//**********************************************************************************************
//
// class CMeshCache implementation.
//

	//******************************************************************************************
	CMeshCache::CMeshCache(CPArray< CVector3<> > pav3_points, CPArray<CTexCoord> patc_tex, rptr<CTexture> ptex)
	{
		Assert(pav3_points.uLen >= 3);

		// Copy points and texture coords.
		pamvVertices = CPArray<SVertex>(pav3_points.uLen);

		for (int i = 0; i < pav3_points.uLen; i++)
		{
			AssertFinite(patc_tex[i]);

			pamvVertices[i].v3Point = pav3_points[i];
			pamvVertices[i].tcTex = patc_tex[i];
		}

		// Copy texture.
		ptexTexture = ptex;

		// Set the face normal from 3 points distributed around the polygon.
		plPlane = CPlane
		(
			pav3_points[0], 
			pav3_points[1 * pav3_points.uLen / 3], 
			pav3_points[2 * pav3_points.uLen / 3]
		);

		// Create bounding volume.
		bvbVolume = CBoundVolBox(pav3_points);
	}


//**********************************************************************************************
//
// CMeshBox implementation.
//

	//******************************************************************************************
	CMeshBox::CMeshBox(const CVector3<>& v3_extents, SSurface sf)
	{
		CHeap& mh_build = mhGetBuildHeap();

		// Build the data on the heap.

		mh_build.mav3Points
			<< CVector3<>( v3_extents.tX, -v3_extents.tY,  v3_extents.tZ)
			<< CVector3<>(-v3_extents.tX, -v3_extents.tY,  v3_extents.tZ)
			<< CVector3<>(-v3_extents.tX, -v3_extents.tY, -v3_extents.tZ)
			<< CVector3<>( v3_extents.tX, -v3_extents.tY, -v3_extents.tZ)

			<< CVector3<>( v3_extents.tX,  v3_extents.tY,  v3_extents.tZ)
			<< CVector3<>( v3_extents.tX,  v3_extents.tY, -v3_extents.tZ)
			<< CVector3<>(-v3_extents.tX,  v3_extents.tY, -v3_extents.tZ)
			<< CVector3<>(-v3_extents.tX,  v3_extents.tY,  v3_extents.tZ)
		;

		const CTexCoord tc0(0.0, 0.0),
						tc1(0.0, 1.0),
						tc2(1.0, 0.0),
						tc3(1.0, 1.0);

		mh_build.mamvVertices
			<< SVertex(mh_build, 0, tc0) << SVertex(mh_build, 1, tc1) << SVertex(mh_build, 2, tc3) << SVertex(mh_build, 3, tc2)
			<< SVertex(mh_build, 0, tc0) << SVertex(mh_build, 3, tc1) << SVertex(mh_build, 5, tc3) << SVertex(mh_build, 4, tc2)
			<< SVertex(mh_build, 4, tc0) << SVertex(mh_build, 5, tc1) << SVertex(mh_build, 6, tc3) << SVertex(mh_build, 7, tc2)

			<< SVertex(mh_build, 2, tc0) << SVertex(mh_build, 1, tc1) << SVertex(mh_build, 7, tc3) << SVertex(mh_build, 6, tc2)
			<< SVertex(mh_build, 1, tc0) << SVertex(mh_build, 0, tc1) << SVertex(mh_build, 4, tc3) << SVertex(mh_build, 7, tc2)
			<< SVertex(mh_build, 3, tc0) << SVertex(mh_build, 2, tc1) << SVertex(mh_build, 6, tc3) << SVertex(mh_build, 5, tc2)
		;

		mh_build.mampPolygons
			<< SPolygon(mh_build, 0, 4)
			<< SPolygon(mh_build, 4, 4)
			<< SPolygon(mh_build, 8, 4)
			<< SPolygon(mh_build, 12, 4)
			<< SPolygon(mh_build, 16, 4)
			<< SPolygon(mh_build, 20, 4)
		;

		mh_build.masfSurfaces << sf;

		new(this) CMesh(mh_build);
		mh_build.Reset();
		CreateWrapBox();
	}

//**********************************************************************************************
//
// class CMeshAnimating implementation.
//

	//******************************************************************************************
	CMeshAnimating::CMeshAnimating(CHeap& mh, CAngle ang_max_vertex_share, bool b_merge_polygons, bool b_split_polygons, bool b_curved) :
		CMesh(mh, ang_max_vertex_share, b_merge_polygons, b_split_polygons, b_curved), aptexTextures(32)
	{
		// Just like a mesh.

		// No time step.
		bAnimateMaps = false;
		sTimeStep = 0.0f;

		// We are starting at the beginning.  No valid additional textures yet.
		iCurrentIndex = -1;

		// Animate all surfaces by default.
		iAnimateSurface = -1;

		// No scrolling by default.
		bAnimateCoords = false;
		v2TexCoordDelta = CVector2<>(0.0f,0.0f);

		// Normally, track two is same as track one.
		iTrackTwo = 0;

		// No freeze frame by default.
		iFreezeFrame = -1;

		// Start at the first frame.  This is necessary for savegames-
		// You can never recover the initial state of the mesh via savegame(the old -1 frame),
		// so do not allow the mesh to ever use it.
		SetFrame(0);

		// Remember this animated mesh.
		ps_renderDB->lpmaAnimatedMeshes.push_back(this);
	}

	//******************************************************************************************
	CMeshAnimating::CMeshAnimating(CHeap& mh, ENormal enl, bool b_merge_polygons, bool b_split_polygons, bool b_curved)
		: aptexTextures(32,new(fhGlobalMesh) rptr<CTexture>[32])
	{
		new(this) CMeshAnimating(mh, angNormalShare(enl), b_merge_polygons, b_split_polygons, b_curved);
	}

	//******************************************************************************************
	void CMeshAnimating::AddTexture
	(
		rptr<CTexture> ptex
	)
	{
		aptexTextures << ptex;
		ptex->GenerateMipLevels();
	}

	//******************************************************************************************
	void CMeshAnimating::SetFrame(int i_frame)
	{
		// Only change if index has changed AND frame is within bounds.
		if (i_frame != iCurrentIndex && i_frame < aptexTextures.uLen)
		{
			iCurrentIndex = i_frame;

			// Index has changed, change the texture pointer.
			Assert(iCurrentIndex >= -1);
			Assert(iCurrentIndex < aptexTextures.uLen);

			if (iAnimateSurface == -1)
			{
				// Go through all surfaces and set their textures to the current texture.
				for (int i = pasfSurfaces.uLen - 1; i >= 0; --i)
				{
					// Move to the next texture.
					pasfSurfaces[i].ptexTexture = (rptr<CTexture>)aptexTextures[iCurrentIndex];
				}
			}
			else
			{
				pasfSurfaces[iAnimateSurface].ptexTexture = (rptr<CTexture>)aptexTextures[iCurrentIndex];
			}
		}
	}

	//******************************************************************************************
	void CMeshAnimating::SetInterval(float f_time_step)
	{
		if (f_time_step > 0.0f)
		{
			// Automatic texture animation.
			bAnimateMaps = true;
			sTimeStep = f_time_step;
			sTimeToChange = sTimeStep + CMessageStep::sStaticTotal;
		}
		else
		{
			// No auto texture animation.
			bAnimateMaps = false;
		}
	}

	//******************************************************************************************
	void CMeshAnimating::SetSurface(int i_surface_no)
	{
		AlwaysAssert(iAnimateSurface < pasfSurfaces.size());

		iAnimateSurface = i_surface_no;
	}

	//******************************************************************************************
	void CMeshAnimating::SetCoordDeltas(float f_dx, float f_dy)
	{
		bAnimateCoords = true;
		v2TexCoordDelta.tX = f_dx;
		v2TexCoordDelta.tY = f_dy;
	}

	//******************************************************************************************
	void CMeshAnimating::SetTrackTwo(int i_track_two)
	{
		Assert(i_track_two > 0);
		Assert(i_track_two <= aptexTextures.uLen - 1);

		iTrackTwo = i_track_two;
	}

	//******************************************************************************************
	void CMeshAnimating::SetFreezeFrame(int i_freeze_frame)
	{
		Assert(i_freeze_frame >= -1);
		Assert(i_freeze_frame <= aptexTextures.uLen - 1);

		iFreezeFrame = i_freeze_frame;
	}

	//******************************************************************************************
	void CMeshAnimating::Render
	(
		const CInstance*			pins,				// The instance owning this shape.
		CRenderContext&				renc,				// The rendering context.
		CShapePresence&				rsp,				// The shape-to-world transform.
		const CTransform3<>&		tf3_shape_camera,	// The shape to camera transform.
		const CPArray<COcclude*>&	papoc,				// Array of occluding objects.
		ESideOf						esf_view			// Shape's relation to the view volume
														// (for trivial acceptance).
	) const
	{
		// Evilly cast away const.
		CMeshAnimating* pma = (CMeshAnimating*)this;		// Need a non_const version of "this".

		if (bAnimateMaps)
		{	
			// Save the old index for comparison.
			int i_new_index = iCurrentIndex;

			// First, see if we need to change.
			while (sTimeToChange < CMessageStep::sStaticTotal)
			{
				if (iCurrentIndex == -1)
				{
					// This mesh has never been rendered.  Start the animation cycle now.
					pma->sTimeToChange = CMessageStep::sStaticTotal;
					i_new_index++;
				}
				else
				{
					// Must change!
					pma->sTimeToChange += sTimeStep;
					i_new_index++;

					// If we will be changing a whole lot, move the change time up so we don't waste our time.
					if (pma->sTimeToChange + sTimeStep * 30.0f < CMessageStep::sStaticTotal)
						pma->sTimeToChange = CMessageStep::sStaticTotal + sTimeStep;
				}

				// Have we overflowed track two?
				if (i_new_index >= aptexTextures.uLen)
				{
					// Yes!  Restart track.
					i_new_index = iTrackTwo;
				}
				else 
				{
					// No!  Have we overflowed track one?
					if (i_new_index == iTrackTwo)
					{
						// Yes!  Restart track one.
						i_new_index = 0;
					}
				}
			}
			
			// If we are going to be waiting for a long time, jump ahead.
			if (sTimeToChange - CMessageStep::sStaticTotal > sTimeStep)
			{
				// Somehow, we are waiting too long.
				pma->sTimeToChange = CMessageStep::sStaticTotal + sTimeStep;
			}

			// Where the magic happens!
			pma->SetFrame(i_new_index);

			if (i_new_index != -1 && i_new_index == iFreezeFrame)
			{
				// Stop animation.
				pma->bAnimateMaps = false;
			}
		}
		
		if (bAnimateCoords)
		{
			// Scrolling texture code.
			float f_smallest_x = 1.0f;
			float f_smallest_y = 1.0f;

			// Calculate texture coord delta for last frame.
			CVector2<> v2_delta = v2TexCoordDelta * CMessageStep::sStaticStep;

			// Go through all vertices and set increment their texture coords.
			for (int i = pamvVertices.uLen - 1; i >= 0; --i)
			{
				pma->pamvVertices[i].tcTex += v2_delta;
				if (pma->pamvVertices[i].tcTex.tX < f_smallest_x)
					f_smallest_x = pma->pamvVertices[i].tcTex.tX;
				if (pma->pamvVertices[i].tcTex.tY < f_smallest_y)
					f_smallest_y = pma->pamvVertices[i].tcTex.tY;
			}

			// Do we need to fix texture coords?
			if (f_smallest_x >= 1.0f || f_smallest_x < 0.0f)
			{
				// Yes!  Fix them.
				float f_correction = -1.0f;
				if (f_smallest_x < 0.0f) 
					f_correction = - f_correction;

				// Go through all vertices and reduce  their texture coords by 1.
				for (int i = pamvVertices.uLen - 1; i >= 0; --i)
				{
					pma->pamvVertices[i].tcTex.tX += f_correction;
				}
			}

			// Do we need to fix texture coords?
			if (f_smallest_y >= 1.0f || f_smallest_y < 0.0f)
			{
				// Yes!  Fix them.
				float f_correction = -1.0f;
				if (f_smallest_y < 0.0f) 
					f_correction = - f_correction;

				// Go through all vertices and reduce  their texture coords by 1.
				for (int i = pamvVertices.uLen - 1; i >= 0; --i)
				{
					pma->pamvVertices[i].tcTex.tY += f_correction;
				}
			}
		}
				
		// Now do the normal thing.
		CMesh::Render(pins, renc, rsp, tf3_shape_camera, papoc, esf_view);
	}

	//*****************************************************************************************
	char *CMeshAnimating::pcSave
	(
		char* pc
	) const
	{
		pc = pcSaveT(pc, sTimeStep);
		pc = pcSaveT(pc, sTimeToChange);
		pc = pcSaveT(pc, iCurrentIndex);
		pc = pcSaveT(pc, v2TexCoordDelta);
		pc = pcSaveT(pc, iTrackTwo);
		pc = pcSaveT(pc, iFreezeFrame);

		return pc;
	}

	//*****************************************************************************************
	const char *CMeshAnimating::pcLoad
	(
		const char* pc
	)
	{
		pc = pcLoadT(pc, &sTimeStep);

		// Since we do not load bAnimateMaps we should call SetInterval to do it for us.
		SetInterval(sTimeStep);

		pc = pcLoadT(pc, &sTimeToChange);

		int i_current_frame;
		pc = pcLoadT(pc, &i_current_frame);

		// Only set frame if it has changed.
		if (i_current_frame != -1)
			SetFrame(i_current_frame);
		else
			SetFrame(0);

		pc = pcLoadT(pc, &v2TexCoordDelta);
		pc = pcLoadT(pc, &iTrackTwo);
		pc = pcLoadT(pc, &iFreezeFrame);

		return pc;
	}



//**********************************************************************************************
//
// class CMeshPlanted implementation.
//

	//******************************************************************************************
	CMeshPlanted::CMeshPlanted(CHeap& mh, const CPresence3<>& pr3, TReal r_plant_distance, CAngle ang_max_vertex_share, bool b_merge_polygons, bool b_split_polygons, bool b_curved) :
		CMesh(mh, ang_max_vertex_share, b_merge_polygons, b_split_polygons, b_curved)
	{
		CMSArray<int, 64> sai_planted_points;

		CVector3<> v3;

		// Height below which a vertex must be planted.
		TReal r_plant_cutoff_value = pr3.v3Pos.tZ - r_plant_distance;

		// Must transform each point by pr3 and determine whether or not the point must be planted.
		int i;
		for (i = 0; i < pav3Points.uLen; ++i)
		{
			// For each world-transformed point....
			v3 = pav3Points[i] * pr3;

			// Is it below the cutoff?
			if (v3.tZ < r_plant_cutoff_value)
			{
				// Yes!  Plant it.
				sai_planted_points << i;
			}
		}
			
		// Create the planted point arrays.
		new (&aiPlantedPoints)CAArray<int>(sai_planted_points.uLen);
		new (&av3PlantedPoints)CAArray<CVector3<> >(sai_planted_points.uLen);

		// Save the original, unplanted points.
		for (i = 0; i < sai_planted_points.uLen; ++i)
		{
			aiPlantedPoints[i] = sai_planted_points[i];
			av3PlantedPoints[i] = pav3Points[ aiPlantedPoints[i] ];
		}
	}

	//******************************************************************************************
	void CMeshPlanted::Render
	(
		const CInstance*			pins,				// The instance owning this shape.
		CRenderContext&				renc,				// The rendering context.
		CShapePresence&				rsp,				// The shape-to-world transform.
		const CTransform3<>&		tf3_shape_camera,	// The shape to camera transform.
		const CPArray<COcclude*>&	papoc,				// Array of occluding objects.
		ESideOf						esf_view			// Shape's relation to the view volume
														// (for trivial acceptance).
	) const
	{
		CPresence3<> pr3_inverse = rsp.pr3GetWorldShape();
		CMeshPlanted* pmp = (CMeshPlanted*)this;
		CVector3<> v3_world_point;
		CVector3<> v3_local_point;
		int i_point = 0;
 
		{
			// Move the points in the planted array so that the transformed points end up in the right spot.
			for (int i = 0; i < aiPlantedPoints.uLen; ++i)
			{
				i_point = aiPlantedPoints[i];

				// Grab the point in world space.
				v3_world_point = av3PlantedPoints[i] * rsp.pr3GetShapeWorld();
				
				// Slam the Z of the point.
				if (renc.Renderer.pSettings->bTerrainHeightRelative)
					v3_world_point.tZ = 0;
				else
				{
					CTerrain* ptrr = CWDbQueryTerrain().tGet();
					if (ptrr)
						v3_world_point.tZ = ptrr->rHeightTIN(v3_world_point.tX, v3_world_point.tY);
					else
						v3_world_point.tZ = 0;
				}

				// Send it back into the mesh space.
				v3_local_point = v3_world_point * pr3_inverse;

				// Clamp the point to keep it in the bounding volume.
				v3_local_point = bvbVolume.v3Clamp(v3_local_point);
								
				// Save back into the mesh.
				pmp->pav3Points[i_point] = v3_local_point;
			}
		}

		// Now do the normal thing.
		CMesh::Render(pins, renc, rsp, tf3_shape_camera, papoc, esf_view);

		// Now restore the points.  This will ensure that the image cache point cloud code
		// still works well.
		for (int i = 0; i < aiPlantedPoints.uLen; ++i)
		{
			i_point = aiPlantedPoints[i];

			// Send it back into the mesh.
			pmp->pav3Points[i_point] = av3PlantedPoints[i];
		}
	}
/*
	//******************************************************************************************
	void CMeshPlanted::TransformPoints
	(
		const CPresence3<>&		pr3_shape_world,
		const CTransform3<>&	tf3_shape_camera,
		const CCamera&			cam,
		CPArray<SClipPoint>		paclpt_points,
		bool					b_outcodes
	) const
	{
		// For now, let's first call parent to transform all points normally.
		CMesh::TransformPoints(pr3_shape_world, tf3_shape_camera, cam, paclpt_points, b_outcodes);

		//
		// Now, replace planted points with adjusted values.
		//

		// Convert presence to transform for speed.
		CTransform3<> tf3_shape_world = pr3_shape_world;
		CTransform3<> tf3_to_camera = cam.tf3ToNormalisedCamera();

		const CBoundVolCamera* pbvcam_clip = cam.pbvcamClipVolume();

		for (int i = 0; i < aiPlantedPoints.uLen; ++i)
		{
			int i_point = aiPlantedPoints[i];

			// Grab the point in world space.
			CVector3<> v3_world_point = pav3Points[i_point] * tf3_shape_world;
			
			// Slam the Z of the point.
			v3_world_point.tZ = fake_terrain(v3_world_point.tX, v3_world_point.tY);

			// Send it to final camera space.
			paclpt_points[i_point].v3Point = v3_world_point * tf3_to_camera;

			if (b_outcodes || VER_DEBUG)
			{
				// Also generate outcode.
				paclpt_points[i_point].seteocOut = 
					pbvcam_clip->seteocOutCodes(paclpt_points[i_point].v3Point);

			#if (VER_DEBUG)
				if (paclpt_points[i_point].seteocOut - eocFAR)
				{
					// This point is outside the view volume, so let's make
					// sure we weren't told to disable clipping.
					// We ignore the far clipping plane for now, since if it's violated it's not
					// critical, and the render cache currently has problems staying within in.
					Assert(b_outcodes);
				}
			#endif
			}
		}
	}
*/
